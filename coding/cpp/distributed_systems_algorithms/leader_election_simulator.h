#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace storage_prep {

enum class Role { Follower, Candidate, Leader, Recovering };

class Cluster {
public:
    explicit Cluster(std::vector<std::string> nodeIds);
    ~Cluster();

    void startElection(const std::string& candidate, bool sendRequests = true);
    void observeHigherTerm(const std::string& node, std::uint64_t term);
    bool deliverHeartbeat(const std::string& receiver,
                          const std::string& leader,
                          std::uint64_t heartbeatTerm);
    void setLink(const std::string& from, const std::string& to, bool reachable);
    void crashAndRestart(const std::string& node);
    bool requestVote(const std::string& voter,
                     const std::string& candidate,
                     std::uint64_t requestedTerm);
    void deliverVoteResponse(const std::string& candidate,
                             const std::string& voter,
                             std::uint64_t responseTerm,
                             bool granted);
    bool acceptWrite(const std::string& replica,
                     const std::string& leader,
                     std::uint64_t requestTerm) const;

    std::uint64_t termOf(const std::string& node) const;
    Role roleOf(const std::string& node) const;
    bool isLeader(const std::string& node) const;

private:
    struct Node;
    Node& node(const std::string& id);
    const Node& node(const std::string& id) const;
    bool isReachable(const std::string& from, const std::string& to) const;
    static std::string linkKey(const std::string& from, const std::string& to);

    std::vector<std::string> nodeIds_;
    std::vector<Node> nodes_;
    std::vector<std::string> blockedLinks_;
};

}  // namespace storage_prep
