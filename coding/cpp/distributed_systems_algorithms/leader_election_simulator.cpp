#include "leader_election_simulator.h"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <unordered_set>

namespace storage_prep {

struct Cluster::Node {
    std::string id;
    std::uint64_t term = 0;
    Role role = Role::Follower;
    std::optional<std::string> votedFor;
    std::optional<std::string> leaderId;
    std::unordered_set<std::string> votes;
};

Cluster::Cluster(std::vector<std::string> nodeIds) : nodeIds_(std::move(nodeIds)) {
    if (nodeIds_.empty()) {
        throw std::invalid_argument("cluster must contain at least one node");
    }
    for (const std::string& id : nodeIds_) {
        nodes_.push_back({id, 0, Role::Follower, std::nullopt, std::nullopt, {}});
    }
}

Cluster::~Cluster() = default;

void Cluster::startElection(const std::string& candidate, bool sendRequests) {
    Node& n = node(candidate);
    ++n.term;
    n.role = Role::Candidate;
    n.votedFor = candidate;  // Persistent evidence, written before any RPC.
    n.leaderId.reset();
    n.votes = {candidate};

    if (!sendRequests) {
        return;
    }
    const std::uint64_t electionTerm = n.term;
    for (const std::string& voter : nodeIds_) {
        if (voter == candidate || !isReachable(candidate, voter)) {
            continue;
        }
        deliverVoteResponse(candidate, voter, electionTerm,
                            requestVote(voter, candidate, electionTerm));
    }
}

void Cluster::observeHigherTerm(const std::string& id, std::uint64_t term) {
    Node& n = node(id);
    if (term > n.term) {
        n.term = term;
        n.role = Role::Follower;
        n.votedFor.reset();
        n.leaderId.reset();
        n.votes.clear();
    }
}

bool Cluster::deliverHeartbeat(const std::string& receiver,
                               const std::string& leader,
                               std::uint64_t heartbeatTerm) {
    Node& n = node(receiver);
    if (heartbeatTerm < n.term) {
        return false;
    }
    if (heartbeatTerm > n.term) {
        observeHigherTerm(receiver, heartbeatTerm);
    }
    n.role = Role::Follower;
    n.leaderId = leader;
    return true;
}

void Cluster::setLink(const std::string& from, const std::string& to, bool reachable) {
    const std::string key = linkKey(from, to);
    auto it = std::find(blockedLinks_.begin(), blockedLinks_.end(), key);
    if (reachable && it != blockedLinks_.end()) {
        blockedLinks_.erase(it);
    } else if (!reachable && it == blockedLinks_.end()) {
        blockedLinks_.push_back(key);
    }
}

void Cluster::crashAndRestart(const std::string& id) {
    Node& n = node(id);
    n.role = Role::Follower;
    n.leaderId.reset();
    n.votes.clear();
    // term and votedFor deliberately survive: they model durable state.
}

bool Cluster::requestVote(const std::string& voter,
                          const std::string& candidate,
                          std::uint64_t requestedTerm) {
    Node& n = node(voter);
    if (requestedTerm < n.term) {
        return false;
    }
    if (requestedTerm > n.term) {
        observeHigherTerm(voter, requestedTerm);
    }
    if (n.votedFor.has_value() && *n.votedFor != candidate) {
        return false;
    }
    n.votedFor = candidate;  // Persistent before the vote response is sent.
    return true;
}

void Cluster::deliverVoteResponse(const std::string& candidate,
                                  const std::string& voter,
                                  std::uint64_t responseTerm,
                                  bool granted) {
    Node& n = node(candidate);
    if (responseTerm < n.term) {
        return;
    }
    if (responseTerm > n.term) {
        observeHigherTerm(candidate, responseTerm);
        return;
    }
    if (n.role != Role::Candidate || !granted) {
        return;
    }
    n.votes.insert(voter);
    if (n.votes.size() > nodeIds_.size() / 2) {
        n.role = Role::Leader;
    }
}

std::uint64_t Cluster::termOf(const std::string& id) const {
    return node(id).term;
}

Role Cluster::roleOf(const std::string& id) const {
    return node(id).role;
}

bool Cluster::isLeader(const std::string& id) const {
    return roleOf(id) == Role::Leader;
}

bool Cluster::acceptWrite(const std::string& replica,
                          const std::string& leader,
                          std::uint64_t requestTerm) const {
    const Node& target = node(replica);
    const Node& sender = node(leader);
    return target.term == requestTerm && sender.term == requestTerm &&
           sender.role == Role::Leader;
}

Cluster::Node& Cluster::node(const std::string& id) {
    const auto it = std::find_if(nodes_.begin(), nodes_.end(), [&](const Node& n) {
        return n.id == id;
    });
    if (it == nodes_.end()) {
        throw std::out_of_range("unknown node: " + id);
    }
    return *it;
}

const Cluster::Node& Cluster::node(const std::string& id) const {
    return const_cast<Cluster*>(this)->node(id);
}

bool Cluster::isReachable(const std::string& from, const std::string& to) const {
    const std::string key = linkKey(from, to);
    return std::find(blockedLinks_.begin(), blockedLinks_.end(), key) ==
           blockedLinks_.end();
}

std::string Cluster::linkKey(const std::string& from, const std::string& to) {
    return from + "\n" + to;
}

}  // namespace storage_prep
