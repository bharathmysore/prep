# Focused Area: Rate Limiters

Rate limiters are a useful L7 interview topic because they start as a small coding problem and quickly become a distributed-systems design problem. The core pressure point is deciding whether the limiter is optimizing for simple correctness, smooth traffic shaping, burst tolerance, global quota accuracy, or low-latency local enforcement.

## Reference Context

This is a representative focused-topic guide, not an official company question bank. It links to existing local company-design guides and the canonical C++ token-bucket prompt, then expands the standard limiter algorithms into interview-ready implementation and design drills.

## Canonical Prep Links

* **Existing coding prompt**: [Concurrent Token Bucket](../coding/cpp/concurrency/questions.md)
* **Existing coding solution**: [Concurrent Token Bucket C++](../coding/cpp/concurrency/solutions.md#3-concurrent-token-bucket)
* **Existing design prompts**:
  * [AWS API Gateway With Distributed Rate Limiting](../system_design/aws_l7_system_design_prep.md#6-design-api-gateway-with-distributed-rate-limiting)
  * [Anthropic Distributed Rate Limiting For The Claude API](../system_design/anthropic_l7_system_design_prep.md#11-design-distributed-rate-limiting-for-the-claude-api)
  * [OpenAI API Rate Limiting And Quotas For LLM APIs](../system_design/openai_l7_system_design_prep.md#9-design-api-rate-limiting-and-quotas-for-llm-apis)
  * [Microsoft Distributed Rate Limiter For Azure APIs](../system_design/microsoft_l7_system_design_prep.md#9-design-a-distributed-rate-limiter-for-azure-apis)
  * [Oracle Distributed Rate Limiter](../system_design/oracle_l7_system_design_prep.md#9-design-distributed-rate-limiter)
  * [NVIDIA Distributed Rate Limiter For GPU Inference APIs](../system_design/nvidia_l7_system_design_prep.md#17-design-a-distributed-rate-limiter-for-gpu-inference-apis)
  * [Stripe Distributed Rate Limiter](../system_design/stripe_l7_system_design_prep.md#16-design-a-distributed-rate-limiter)
  * [Apple Multi-Tenant I/O QoS And Noisy-Neighbor Control](../system_design/apple_l7_system_design_prep.md#25-design-multi-tenant-io-qos-and-noisy-neighbor-control)

## Algorithm Selection Summary

| Rank | Algorithm | Best Interview Use | Time | Space | Main Tradeoff |
| --- | --- | --- | --- | --- | --- |
| 1 | Token bucket | Average rate plus bounded bursts. Most common coding and design answer. | `O(1)` | `O(1)` per key | Allows bursts; can be unfair immediately after idle periods. |
| 2 | Sliding window counter | Approximate rolling-window enforcement with constant memory. | `O(1)` | `O(1)` per key | Smooths fixed-window edges; approximate near boundaries. |
| 3 | Sliding window log | Exact rolling-window request count. | Amortized `O(1)` | `O(limit)` per key | Accurate; memory grows with allowed burst size. |
| 4 | Fixed window counter | Simple quotas, cheap counters, coarse limits. | `O(1)` | `O(1)` per key | Boundary bursts can double the effective instantaneous rate. |
| 5 | Leaky bucket meter | Smoothing traffic as accumulated debt drains at a fixed rate. | `O(1)` | `O(1)` per key | Stable output shape; less intuitive burst semantics than token bucket. |
| 6 | GCRA | Precise theoretical-arrival-time limiter used by telecom/API systems. | `O(1)` | `O(1)` per key | Compact and accurate; harder to explain quickly. |
| 7 | Keyed or hierarchical limiter | Tenant, user, endpoint, model, or volume limits. | `O(1)` average | `O(active keys)` | Production-shaped; needs eviction, sharding, and policy versioning. |

## Shared C++ Assumptions

These snippets are C++17, single-process, and thread-safe with one mutex per limiter. They use `std::chrono::steady_clock` so wall-clock jumps do not mint or remove quota. For distributed systems, treat these as local enforcement blocks; global correctness needs quota leases, replicated counters, or reconciliation.

```cpp
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

using namespace std;
```

## 1. Fixed Window Counter

* **Question**: Allow at most `N` requests in each fixed time window.
* **Pattern / Idea**: Keep one counter for the current window; reset it when time crosses the next window boundary.
* **C++ Solution**

```cpp
class FixedWindowLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const int64_t maxRequests_;
    const chrono::milliseconds window_;
    Clock::time_point windowStart_;
    int64_t used_ = 0;

public:
    FixedWindowLimiter(int64_t maxRequests, chrono::milliseconds window)
        : maxRequests_(maxRequests),
          window_(window),
          windowStart_(Clock::now()) {
        if (maxRequests_ <= 0 || window_.count() <= 0) {
            throw invalid_argument("invalid fixed-window limiter config");
        }
    }

    bool allow() {
        return allowAt(Clock::now());
    }

    bool allowAt(Clock::time_point now) {
        lock_guard<mutex> lock(mu_);
        if (now - windowStart_ >= window_) {
            auto windowsPassed = (now - windowStart_) / window_;
            windowStart_ += window_ * windowsPassed;
            used_ = 0;
        }

        if (used_ >= maxRequests_) return false;
        ++used_;
        return true;
    }
};
```

* **Complexity**: Time `O(1)` per request; space `O(1)` per key.
* **Performance Improvements**: Use atomic counters for one window when exact reset races are acceptable; shard by key for high-cardinality tenants; store counters in Redis or a regional counter service for shared enforcement.
* **Tradeoffs**: Very cheap and easy to reason about, but a caller can send `N` requests at the end of one window and `N` at the start of the next.

## 2. Sliding Window Log

* **Question**: Allow at most `N` requests in any rolling interval of length `T`.
* **Pattern / Idea**: Store timestamps for accepted requests and evict timestamps older than the rolling window.
* **C++ Solution**

```cpp
class SlidingWindowLogLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const size_t maxRequests_;
    const chrono::milliseconds window_;
    deque<Clock::time_point> hits_;

public:
    SlidingWindowLogLimiter(size_t maxRequests, chrono::milliseconds window)
        : maxRequests_(maxRequests), window_(window) {
        if (maxRequests_ == 0 || window_.count() <= 0) {
            throw invalid_argument("invalid sliding-window log config");
        }
    }

    bool allow() {
        return allowAt(Clock::now());
    }

    bool allowAt(Clock::time_point now) {
        lock_guard<mutex> lock(mu_);
        while (!hits_.empty() && now - hits_.front() >= window_) {
            hits_.pop_front();
        }

        if (hits_.size() >= maxRequests_) return false;
        hits_.push_back(now);
        return true;
    }
};
```

* **Complexity**: Amortized time `O(1)` per request because each accepted timestamp is pushed and popped once; space `O(N)` per key.
* **Performance Improvements**: Use a compact ring buffer when the limit is fixed; cap per-key memory and evict idle keys; aggregate timestamps into small buckets if exactness is not required.
* **Tradeoffs**: Exact rolling-window behavior is easy to test, but memory is proportional to the allowed burst and can become expensive for millions of active keys.

## 3. Sliding Window Counter

* **Question**: Approximate a rolling-window limit with constant memory.
* **Pattern / Idea**: Keep counts for the current and previous fixed windows, then weight the previous count by how much of that previous window still overlaps the rolling interval.
* **C++ Solution**

```cpp
class SlidingWindowCounterLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const int64_t maxRequests_;
    const chrono::milliseconds window_;
    Clock::time_point currentStart_;
    int64_t currentCount_ = 0;
    int64_t previousCount_ = 0;

    void advanceWindow(Clock::time_point now) {
        if (now - currentStart_ < window_) return;

        auto windowsPassed = (now - currentStart_) / window_;
        previousCount_ = (windowsPassed == 1) ? currentCount_ : 0;
        currentCount_ = 0;
        currentStart_ += window_ * windowsPassed;
    }

    double estimatedCount(Clock::time_point now) const {
        const double elapsed = chrono::duration<double>(now - currentStart_).count();
        const double width = chrono::duration<double>(window_).count();
        const double previousWeight = max(0.0, 1.0 - elapsed / width);
        return static_cast<double>(currentCount_) +
               static_cast<double>(previousCount_) * previousWeight;
    }

public:
    SlidingWindowCounterLimiter(int64_t maxRequests, chrono::milliseconds window)
        : maxRequests_(maxRequests),
          window_(window),
          currentStart_(Clock::now()) {
        if (maxRequests_ <= 0 || window_.count() <= 0) {
            throw invalid_argument("invalid sliding-window counter config");
        }
    }

    bool allow() {
        return allowAt(Clock::now());
    }

    bool allowAt(Clock::time_point now) {
        lock_guard<mutex> lock(mu_);
        advanceWindow(now);

        if (estimatedCount(now) + 1.0 > static_cast<double>(maxRequests_)) {
            return false;
        }
        ++currentCount_;
        return true;
    }
};
```

* **Complexity**: Time `O(1)` per request; space `O(1)` per key.
* **Performance Improvements**: Precompute reciprocal window width; use integer bucket weights for lower CPU cost; shard counters by key to reduce lock contention.
* **Tradeoffs**: Much cheaper than a sliding log, but it can reject or allow near the boundary differently from an exact rolling window.

## 4. Token Bucket

* **Question**: Allow bursts up to bucket capacity while enforcing a long-term average refill rate.
* **Pattern / Idea**: Lazily refill tokens using elapsed monotonic time; consume `cost` tokens if enough are available.
* **C++ Solution**

```cpp
class TokenBucketLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const double capacity_;
    const double refillPerSecond_;
    double tokens_;
    Clock::time_point lastRefill_;

    void refill(Clock::time_point now) {
        if (now <= lastRefill_) return;
        const double elapsed = chrono::duration<double>(now - lastRefill_).count();
        tokens_ = min(capacity_, tokens_ + elapsed * refillPerSecond_);
        lastRefill_ = now;
    }

public:
    TokenBucketLimiter(double capacity, double refillPerSecond)
        : capacity_(capacity),
          refillPerSecond_(refillPerSecond),
          tokens_(capacity),
          lastRefill_(Clock::now()) {
        if (capacity_ <= 0.0 || refillPerSecond_ <= 0.0) {
            throw invalid_argument("invalid token-bucket config");
        }
    }

    bool allow(double cost = 1.0) {
        return allowAt(cost, Clock::now());
    }

    bool allowAt(double cost, Clock::time_point now) {
        if (cost <= 0.0 || cost > capacity_) return false;

        lock_guard<mutex> lock(mu_);
        refill(now);
        if (tokens_ < cost) return false;
        tokens_ -= cost;
        return true;
    }
};
```

* **Complexity**: Time `O(1)` per request; space `O(1)` per key.
* **Performance Improvements**: Avoid background refill threads; store rates as fixed-point integers for deterministic accounting; stripe buckets across mutexes; batch token leases from a global quota service.
* **Tradeoffs**: It handles bursts naturally, but idle clients accumulate credit and can produce short spikes unless capacity is tuned carefully.

## 5. Leaky Bucket Meter

* **Question**: Accept a request if the accumulated bucket water remains below capacity while water drains at a fixed rate.
* **Pattern / Idea**: Model accepted work as debt. Debt leaks away over time; new work is rejected when the debt would exceed capacity.
* **C++ Solution**

```cpp
class LeakyBucketLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const double capacity_;
    const double leakPerSecond_;
    double water_ = 0.0;
    Clock::time_point lastLeak_;

    void leak(Clock::time_point now) {
        if (now <= lastLeak_) return;
        const double elapsed = chrono::duration<double>(now - lastLeak_).count();
        water_ = max(0.0, water_ - elapsed * leakPerSecond_);
        lastLeak_ = now;
    }

public:
    LeakyBucketLimiter(double capacity, double leakPerSecond)
        : capacity_(capacity),
          leakPerSecond_(leakPerSecond),
          lastLeak_(Clock::now()) {
        if (capacity_ <= 0.0 || leakPerSecond_ <= 0.0) {
            throw invalid_argument("invalid leaky-bucket config");
        }
    }

    bool allow(double cost = 1.0) {
        return allowAt(cost, Clock::now());
    }

    bool allowAt(double cost, Clock::time_point now) {
        if (cost <= 0.0 || cost > capacity_) return false;

        lock_guard<mutex> lock(mu_);
        leak(now);
        if (water_ + cost > capacity_) return false;
        water_ += cost;
        return true;
    }
};
```

* **Complexity**: Time `O(1)` per request; space `O(1)` per key.
* **Performance Improvements**: Use this as a cheap service-protection meter; pair it with a bounded queue when you want delayed execution instead of immediate rejection.
* **Tradeoffs**: Smooths admission and makes overload state visible as accumulated debt, but a token bucket is often easier for product teams to reason about because it directly exposes burst credits.

## 6. GCRA

* **Question**: Enforce a rate and burst using a single theoretical-arrival-time timestamp.
* **Pattern / Idea**: Track when the next conforming request would arrive. A request is allowed if it is not earlier than the tolerated burst window before that theoretical time.
* **C++ Solution**

```cpp
class GcraLimiter {
    using Clock = chrono::steady_clock;

    mutable mutex mu_;
    const int64_t burst_;
    const Clock::duration interval_;
    const Clock::duration tolerance_;
    bool initialized_ = false;
    Clock::time_point theoreticalArrival_;

    static Clock::duration toClockDuration(double seconds) {
        auto duration = chrono::duration_cast<Clock::duration>(
            chrono::duration<double>(seconds));
        return duration.count() > 0 ? duration : Clock::duration(1);
    }

    static Clock::duration checkedInterval(double ratePerSecond) {
        if (ratePerSecond <= 0.0) {
            throw invalid_argument("invalid GCRA rate");
        }
        return toClockDuration(1.0 / ratePerSecond);
    }

    static int64_t checkedBurst(int64_t burst) {
        if (burst <= 0) {
            throw invalid_argument("invalid GCRA burst");
        }
        return burst;
    }

public:
    GcraLimiter(double ratePerSecond, int64_t burst)
        : burst_(checkedBurst(burst)),
          interval_(checkedInterval(ratePerSecond)),
          tolerance_(interval_ * (burst_ - 1)) {}

    bool allow(int64_t cost = 1) {
        return allowAt(cost, Clock::now());
    }

    bool allowAt(int64_t cost, Clock::time_point now) {
        if (cost <= 0 || cost > burst_) return false;

        lock_guard<mutex> lock(mu_);
        if (!initialized_) {
            theoreticalArrival_ = now;
            initialized_ = true;
        }

        const auto earliestAllowed = theoreticalArrival_ - tolerance_;
        if (now < earliestAllowed) return false;

        theoreticalArrival_ = max(theoreticalArrival_, now) + interval_ * cost;
        return true;
    }
};
```

* **Complexity**: Time `O(1)` per request; space `O(1)` per key.
* **Performance Improvements**: GCRA is already compact; store timestamps as integer ticks and rates as rational values to avoid floating-point drift in very hot paths.
* **Tradeoffs**: It gives precise rate conformance with tiny state, but most interviewers will need a clear explanation of theoretical arrival time before the code feels intuitive.

## 7. Keyed Token Bucket

* **Question**: Apply token-bucket limits independently per tenant, user, endpoint, model, or volume.
* **Pattern / Idea**: Store one bucket state per key and lazily create, refill, and evict buckets.
* **C++ Solution**

```cpp
class KeyedTokenBucketLimiter {
    using Clock = chrono::steady_clock;

    struct Bucket {
        double tokens;
        Clock::time_point lastRefill;
    };

    mutable mutex mu_;
    const double capacity_;
    const double refillPerSecond_;
    unordered_map<string, Bucket> buckets_;

    void refill(Bucket& bucket, Clock::time_point now) const {
        if (now <= bucket.lastRefill) return;
        const double elapsed = chrono::duration<double>(now - bucket.lastRefill).count();
        bucket.tokens = min(capacity_, bucket.tokens + elapsed * refillPerSecond_);
        bucket.lastRefill = now;
    }

public:
    KeyedTokenBucketLimiter(double capacity, double refillPerSecond)
        : capacity_(capacity), refillPerSecond_(refillPerSecond) {
        if (capacity_ <= 0.0 || refillPerSecond_ <= 0.0) {
            throw invalid_argument("invalid keyed token-bucket config");
        }
    }

    bool allow(const string& key, double cost = 1.0) {
        return allowAt(key, cost, Clock::now());
    }

    bool allowAt(const string& key, double cost, Clock::time_point now) {
        if (cost <= 0.0 || cost > capacity_) return false;

        lock_guard<mutex> lock(mu_);
        auto it = buckets_.find(key);
        if (it == buckets_.end()) {
            it = buckets_.emplace(key, Bucket{capacity_, now}).first;
        }

        Bucket& bucket = it->second;
        refill(bucket, now);
        if (bucket.tokens < cost) return false;
        bucket.tokens -= cost;
        return true;
    }

    void eraseIdle(chrono::seconds idleFor) {
        const auto now = Clock::now();
        lock_guard<mutex> lock(mu_);
        for (auto it = buckets_.begin(); it != buckets_.end();) {
            if (now - it->second.lastRefill >= idleFor) {
                it = buckets_.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t activeKeys() const {
        lock_guard<mutex> lock(mu_);
        return buckets_.size();
    }
};
```

* **Complexity**: Average time `O(1)` per request; space `O(active keys)`.
* **Performance Improvements**: Use lock striping by key hash; run TTL eviction in bounded batches; split policy lookup from hot-path token state; pre-allocate regional quota leases for distributed use.
* **Tradeoffs**: This is closer to production shape, but key cardinality, eviction, hot-key contention, and policy changes become part of correctness.

## Focused Design Question: Distributed Rate Limiter With Quota Leasing

* **Question**
  Design a low-latency distributed rate limiter for a multi-region cloud API. It must enforce per-tenant and per-endpoint limits, support bursts, survive regional isolation, and keep global quota overshoot bounded.

* **Answer**
  * **Scope**
    Enforce request, cost-weighted, concurrency, and monthly quota limits at regional gateways. Support local hot-path decisions, centralized policy control, quota leasing, audit events, emergency blocks, and customer-visible retry information.

  * **Functional Requirements**
    * Configure tenant, user, endpoint, region, and global limits.
    * Enforce short-window rate limits and longer-window quotas.
    * Return `429` or service-specific throttling errors with retry hints.
    * Support burst credits, temporary increases, and emergency deny lists.
    * Emit usage events for dashboards, billing, audits, and reconciliation.

  * **Non Functional Requirements**
    * Hot-path decision in single-digit milliseconds or less.
    * Highly available regional enforcement.
    * Bounded global quota overshoot during partitions.
    * Tenant isolation and predictable fairness.
    * Observable decisions with low-cardinality customer metrics and high-cardinality internal traces.

  * **High level design and diagram (at block level)**

    ```text
    Admin / Policy API -> Policy Store -> Config Publisher
                                      \-> Audit Log

    Client -> Regional Gateway -> Local Limiter -> Backend Service
                                  |        |
                                  |        v
                                  |   Local Token Buckets / GCRA
                                  v
                            Usage Event Stream -> Regional Aggregator
                                                   |
                                                   v
                               Global Quota Service / Lease Manager
                                                   |
                                                   v
                                      Rebalance / Reconciliation
    ```

    * **Explain the blocks**
      Regional Gateway is the enforcement point. Local Limiter performs token-bucket, GCRA, concurrency, or cost checks from cached policy. Usage Event Stream records accepted and rejected decisions. Regional Aggregator compresses hot events. Global Quota Service owns entitlements and distributes regional leases. Config Publisher safely rolls policy versions to gateways.

    * **Core components and low-level design**
      The Local Limiter stores keyed bucket state by `(tenant, endpoint, region, policy_version)`. It uses monotonic time, bounded key eviction, lock striping, and clear fail-open/fail-closed policy by API class. The Lease Manager stores global quota, regional lease balances, lease epochs, and reconciliation state. Lease grants are idempotent and versioned; gateways reject stale policy versions after a grace period. Usage aggregation uses idempotency keys so retries do not double-count accepted work.

    * **Explain the control flow**
      Operators update policy through an audited control plane. The policy is validated, versioned, canaried, and pushed to regional gateways. The global service grants each region a lease of quota tokens based on historical demand and current traffic. Rebalancing shifts unused quota from quiet regions to hot regions.

    * **Explain the data flow**
      A request reaches a regional gateway, authenticates, loads cached policy, checks local tokens or theoretical arrival time, and either proceeds or returns throttling metadata. Accepted requests emit usage events. Aggregators reconcile actual usage with regional lease balances. If a region exhausts its lease, it asks for more or throttles locally.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Global accuracy vs availability**
      Central counters are accurate but put a remote dependency in the hot path. Pure local counters are fast but can overshoot global limits during partitions. Prefer regional quota leases with explicit maximum overshoot and central synchronous checks only for strict compliance or unpaid tiers.

    * **Which algorithm should run locally?**
      Token buckets are the default for human-understandable burst credits. GCRA gives compact precise conformance. Sliding logs are useful for exact audit-heavy limits but are expensive at high cardinality. Use token bucket or GCRA for API traffic, and use sliding logs only for sensitive low-volume actions.

    * **Cost-weighted limits**
      Request counts are simple but unfair for variable-cost work such as LLM tokens, large writes, or expensive queries. Estimate cost before admission, reserve tokens, and reconcile actual cost after completion. Bound underestimation with max-request caps and concurrency limits.

    * **Failure behavior**
      Fail-open protects availability but can violate quota and overload backends. Fail-closed protects expensive resources but can cause customer-visible incidents. Choose per API: fail-open with small local caps for low-risk reads; fail-closed or degraded-mode for writes, paid resources, compliance, or abuse controls.

## Design Drill Set

| Rank | Design Question | Existing Full Answer | What To Emphasize |
| --- | --- | --- | --- |
| 1 | Design distributed rate limiting for a global API gateway. | [AWS Q6](../system_design/aws_l7_system_design_prep.md#6-design-api-gateway-with-distributed-rate-limiting), [Microsoft Q9](../system_design/microsoft_l7_system_design_prep.md#9-design-a-distributed-rate-limiter-for-azure-apis) | Control plane vs data plane, local limiter latency, global quota leasing, rollout and rollback. |
| 2 | Design token-aware rate limiting for LLM or GPU inference APIs. | [OpenAI Q9](../system_design/openai_l7_system_design_prep.md#9-design-api-rate-limiting-and-quotas-for-llm-apis), [Anthropic Q11](../system_design/anthropic_l7_system_design_prep.md#11-design-distributed-rate-limiting-for-the-claude-api), [NVIDIA Q17](../system_design/nvidia_l7_system_design_prep.md#17-design-a-distributed-rate-limiter-for-gpu-inference-apis) | Estimated vs actual cost, concurrency caps, tenant tiers, spend limits, abuse response. |
| 3 | Design merchant, endpoint, and abuse-aware API limits. | [Stripe Q16](../system_design/stripe_l7_system_design_prep.md#16-design-a-distributed-rate-limiter) | Hierarchical keys, sensitive operations, fraud spikes, retry headers, customer support workflows. |
| 4 | Design cloud API quota enforcement with regional failover. | [Oracle Q9](../system_design/oracle_l7_system_design_prep.md#9-design-distributed-rate-limiter) | Regional counters, global aggregators, fail-open/fail-closed policy, auditability. |
| 5 | Design storage I/O QoS and noisy-neighbor control. | [Apple Q25](../system_design/apple_l7_system_design_prep.md#25-design-multi-tenant-io-qos-and-noisy-neighbor-control) | Token buckets plus priority queues, burst credits, foreground vs background I/O, tail latency. |

## Interview Test Strategy

* **Coding tests**: empty or invalid config, exact capacity, just-before and just-after window boundaries, idle refill, large cost, concurrent callers, hot key contention, and idle-key eviction.
* **Design tests**: regional partition, policy rollback, quota overshoot bound, traffic shift between regions, usage-event duplication, clock skew in wall-clock systems, and emergency block propagation.
* **Observability**: allowed/denied counts, throttle reason, retry-after distribution, token balance, lease balance, hot keys, policy version, local decision latency, backend overload signals, and customer-visible `429` rate.
