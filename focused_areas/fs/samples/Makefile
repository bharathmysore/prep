CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror
BUILD ?= /tmp/fs-learning-build
NAMES := block_device journal_model striping
BINS := $(addprefix $(BUILD)/,$(NAMES))

.PHONY: all test
all: $(BINS)

$(BUILD):
	mkdir -p "$(BUILD)"

$(BUILD)/%: %.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) "$<" -o "$@"

test: all
	"$(BUILD)/block_device"
	"$(BUILD)/journal_model"
	"$(BUILD)/striping"
