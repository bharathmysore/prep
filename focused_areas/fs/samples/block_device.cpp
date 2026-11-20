// E1: regular-file block I/O. POSIX/macOS/Linux; single caller; no raw devices.
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
template<class F> void must_throw(F f) {
    bool threw = false;
    try { f(); } catch (const std::exception&) { threw = true; }
    require(threw, "expected failure was accepted");
}
class BlockDevice {
    int fd_ = -1;
    std::uint64_t blocks_;
    std::size_t size_;
    static void io_error(const char* operation) {
        throw std::runtime_error(std::string(operation) + ": " + std::strerror(errno));
    }
    off_t offset(std::uint64_t block) const {
        if (block >= blocks_) throw std::out_of_range("block ID");
        return static_cast<off_t>(block * size_);
    }
public:
    BlockDevice(const std::string& path, std::uint64_t blocks, std::size_t size)
        : blocks_(blocks), size_(size) {
        const auto max_offset = static_cast<std::uint64_t>(std::numeric_limits<off_t>::max());
        if (blocks == 0 || size == 0 || size > max_offset ||
            size > static_cast<std::size_t>(std::numeric_limits<ssize_t>::max()) ||
            blocks > max_offset / size) throw std::invalid_argument("invalid geometry");
        // Never create, truncate or resize an existing file here.
        fd_ = ::open(path.c_str(), O_RDWR);
        if (fd_ < 0) io_error("open");
        struct stat info {};
        if (::fstat(fd_, &info) != 0) {
            const int saved = errno;
            ::close(fd_); fd_ = -1; errno = saved; io_error("fstat");
        }
        if (!S_ISREG(info.st_mode) || info.st_size != static_cast<off_t>(blocks * size)) {
            ::close(fd_); fd_ = -1;
            throw std::invalid_argument("image must be a regular file with matching geometry");
        }
    }
    BlockDevice(const BlockDevice&) = delete;
    BlockDevice& operator=(const BlockDevice&) = delete;
    ~BlockDevice() { if (fd_ >= 0) ::close(fd_); }
    void write(std::uint64_t block, const std::vector<unsigned char>& data) {
        const off_t start = offset(block);
        if (data.size() != size_) throw std::invalid_argument("write needs one full block");
        std::size_t done = 0;
        while (done < size_) {
            const auto n = ::pwrite(fd_, data.data() + done, size_ - done,
                                    start + static_cast<off_t>(done));
            if (n < 0) { if (errno == EINTR) continue; io_error("pwrite"); }
            if (n == 0) throw std::runtime_error("pwrite made no progress");
            done += static_cast<std::size_t>(n);
        }
    }
    std::vector<unsigned char> read(std::uint64_t block) const {
        const off_t start = offset(block);
        std::vector<unsigned char> data(size_);
        std::size_t done = 0;
        while (done < size_) {
            const auto n = ::pread(fd_, data.data() + done, size_ - done,
                                   start + static_cast<off_t>(done));
            if (n < 0) { if (errno == EINTR) continue; io_error("pread"); }
            if (n == 0) throw std::runtime_error("unexpected EOF: image truncated");
            done += static_cast<std::size_t>(n);
        }
        return data;
    }
    void sync() {
        while (::fsync(fd_) != 0) {
            if (errno == EINTR) continue;
            io_error("fsync");
        }
    }
};
struct TempImage {
    char path[64] = "/tmp/fs-block-sample-XXXXXX";
    int fd;
    TempImage() : fd(::mkstemp(path)) {
        if (fd < 0) throw std::runtime_error("mkstemp failed");
    }
    ~TempImage() { ::close(fd); ::unlink(path); }
};
int main() {
    try {
        TempImage image;
        constexpr std::size_t block_size = 4096;
        constexpr std::uint64_t blocks = 8;
        require(::ftruncate(image.fd, blocks * block_size) == 0, "ftruncate failed");
        const std::vector<unsigned char> first(block_size, 0x41), last(block_size, 0x5a);
        {
            BlockDevice disk(image.path, blocks, block_size);
            require(disk.read(0) == std::vector<unsigned char>(block_size, 0), "new image zeros");
            disk.write(0, first); disk.write(blocks - 1, last); disk.sync();
            must_throw([&] { disk.read(blocks); });
            must_throw([&] { disk.write(blocks, first); });
            must_throw([&] { disk.write(1, {1, 2}); });
            must_throw([&] { BlockDevice bad(image.path, 0, block_size); });
            must_throw([&] { BlockDevice bad(image.path, blocks, 0); });
            must_throw([&] { BlockDevice bad(image.path, UINT64_MAX, block_size); });
            must_throw([&] { BlockDevice bad(image.path, blocks - 1, block_size); });
        }
        {
            BlockDevice disk(image.path, blocks, block_size);
            require(disk.read(0) == first, "first block differs on reopen");
            require(disk.read(blocks - 1) == last, "last block differs on reopen");
            require(disk.read(1) == std::vector<unsigned char>(block_size, 0), "neighbor changed");
            // This is our own disposable image, not a user-supplied file.
            require(::ftruncate(image.fd, block_size) == 0, "test truncation failed");
            must_throw([&] { disk.read(blocks - 1); });
        }
        must_throw([&] { BlockDevice bad(image.path, blocks, block_size); });
        std::cout << "PASS block device: boundaries, validation, sync/reopen, truncation\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n'; return 1;
    }
}
