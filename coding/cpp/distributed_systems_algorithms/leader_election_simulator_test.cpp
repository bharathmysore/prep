#include "leader_election_simulator.h"

#include <cassert>
#include <iostream>

namespace {

void delayed_vote_from_an_old_term_is_ignored() {
    storage_prep::Cluster cluster({"A", "B", "C"});

    cluster.startElection("A");  // A enters term 1 and votes for itself.
    cluster.observeHigherTerm("A", 2);
    cluster.deliverVoteResponse("A", "B", 1, true);

    assert(cluster.termOf("A") == 2);
    assert(cluster.roleOf("A") == storage_prep::Role::Follower);
    assert(!cluster.isLeader("A"));
}

void stale_heartbeat_does_not_reinstall_an_old_leader() {
    storage_prep::Cluster cluster({"A", "B", "C"});

    cluster.observeHigherTerm("C", 2);
    const bool accepted = cluster.deliverHeartbeat("C", "A", 1);

    assert(!accepted);
    assert(cluster.termOf("C") == 2);
    assert(cluster.roleOf("C") == storage_prep::Role::Follower);
}

void a_partition_without_a_majority_cannot_elect_a_leader() {
    storage_prep::Cluster cluster({"A", "B", "C", "D", "E"});

    for (const char* left : {"A", "B"}) {
        for (const char* right : {"C", "D", "E"}) {
            cluster.setLink(left, right, false);
            cluster.setLink(right, left, false);
        }
    }

    cluster.startElection("A");
    cluster.startElection("C");

    assert(!cluster.isLeader("A"));
    assert(cluster.isLeader("C"));
}

void a_restart_preserves_a_term_vote() {
    storage_prep::Cluster cluster({"A", "B", "C"});

    cluster.startElection("A", false);  // Persist term 1 and self-vote; send no RPCs.
    cluster.crashAndRestart("A");

    assert(cluster.termOf("A") == 1);
    assert(!cluster.requestVote("A", "B", 1));
}

void a_former_leader_write_is_fenced_after_a_new_term() {
    storage_prep::Cluster cluster({"A", "B", "C"});

    cluster.startElection("A");
    assert(cluster.isLeader("A"));

    cluster.observeHigherTerm("B", 1);
    cluster.startElection("B");
    assert(cluster.isLeader("B"));

    assert(!cluster.acceptWrite("C", "A", 1));
    assert(cluster.acceptWrite("C", "B", 2));
}

}  // namespace

int main() {
    delayed_vote_from_an_old_term_is_ignored();
    stale_heartbeat_does_not_reinstall_an_old_leader();
    a_partition_without_a_majority_cannot_elect_a_leader();
    a_restart_preserves_a_term_vote();
    a_former_leader_write_is_fenced_after_a_new_term();
    std::cout << "leader election simulator tests passed\n";
}
