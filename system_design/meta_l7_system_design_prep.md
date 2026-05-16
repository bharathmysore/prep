# Meta L7 System Design Practice Guide

These are publicly reported or Meta-style practice prompts, not an official current Meta question bank. The answers are intentionally framed for L7: scope control, system decomposition, tradeoffs, operational thinking, and deep dives.

Treat this as a living company-specific catalog. Do not encode a fixed question count in this file name or title. Future agents should compute the current count from numbered `## N.` headings and keep numbering consistent after additions, removals, merges, or reordering.

## Maintenance Notes

* Keep this guide Meta-specific and focused on cloud, infrastructure, large-scale product systems, and distributed systems.
* When updating prompts, prefer public reports and representative Meta-style questions; do not present leaked or proprietary interview content as fact.
* Preserve the answer structure used below unless the user asks for a different format.
* Add new questions as numbered `## N.` sections and renumber only when it improves readability.
* Run `rg "^## [0-9]+\\." system_design/meta_l7_system_design_prep.md` from the workspace root after edits to verify the current question catalog.

## Useful Public References

* [IGotAnOffer: Meta system design interview guide](https://igotanoffer.com/blogs/tech/meta-system-design-interview)
* [GeeksforGeeks: Meta/Facebook system design interview questions](https://www.geeksforgeeks.org/system-design/meta-facebook-system-design-interview-questions/)
* [CodingInterview: Meta system design interview questions](https://www.codinginterview.com/guide/meta-system-design-interview-questions/)
* Public AI infrastructure context: [Meta MTIA public reporting](https://www.tomshardware.com/tech-industry/semiconductors/meta-reveals-four-new-mtia-chips-built-for-ai-inference), [Next Gen MTIA public reporting](https://www.investopedia.com/meta-unveils-its-latest-ai-chip-here-is-what-you-need-to-know-8629599), [First-Generation Inference Accelerator Deployment at Facebook](https://arxiv.org/abs/2107.04140), and [TritorX agentic operator generation for ML ASICs](https://arxiv.org/abs/2512.10977).

## Answer Template

Use this pattern in the interview:

1. Clarify scope and constraints.
2. Draw the block-level system.
3. Explain control flow separately from data flow.
4. Deep dive into the highest-risk part.
5. Compare options with pros and cons.

---

## 1. Design Facebook News Feed

* Question
  * Design Facebook News Feed for billions of users.

* Answer

** Scope
  * Include post creation, feed generation, ranking, fanout, likes/comments counts, privacy checks, and freshness.
  * Exclude full ads ranking, full ML training pipeline, and moderation except as extension points.

** Functional Requirements
  * Users can create posts with text/media.
  * Users can see a personalized feed from friends, groups, pages, and recommendations.
  * Feed supports pagination, refresh, likes, comments, shares, hides, and reporting.
  * Feed respects privacy, blocks, groups, and deleted content.
  * Feed should mix fresh content and ranked content.

** Non Functional Requirements
  * Low feed-read latency, ideally p95 under a few hundred ms.
  * High availability for read path.
  * Eventual consistency acceptable for counts and ranking.
  * Stronger consistency required for privacy, deletion, and block rules.
  * Horizontally scalable fanout and ranking.
  * Cost-aware caching because reads are enormous.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Feed API Gateway
  |
  +--> Post Service --> Post Store --> Media Store/CDN
  |
  +--> Feed Retrieval Service
          |
          +--> Timeline Cache
          +--> Social Graph Service
          +--> Candidate Generator
          +--> Ranking Service --> Feature Store / Model Service
          +--> Privacy Service
          +--> Content Store
          |
          v
        Ranked Feed Response

Post Created Event
  |
  v
Event Bus --> Fanout Service --> Timeline Cache / Feed Index
```

*** Explain the blocks
  * Feed API Gateway: authenticates users, rate-limits requests, and routes reads/writes.
  * Post Service: stores post metadata and emits post-created events.
  * Media Store/CDN: stores images/videos and serves them close to users.
  * Social Graph Service: returns friends, follows, groups, and page relationships.
  * Fanout Service: pushes post IDs into followers' timeline caches for normal users.
  * Timeline Cache: stores candidate post IDs per user.
  * Candidate Generator: pulls candidates from cache, graph, groups, recommendations, and celebrity/page sources.
  * Ranking Service: scores candidates using features and ML models.
  * Privacy Service: filters posts based on visibility, blocks, deletions, and audience rules.

*** Explain the control flow
  * Admin/config teams publish ranking model versions, feed mixing rules, experiment configs, cache TTLs, and privacy policy versions.
  * Config is validated, versioned, rolled out gradually, and cached by ranking/feed services.
  * Operational control flow includes kill switches for ranking models, fanout jobs, and recommendation sources.

*** Explain the data flow
  * Write path: user creates post -> Post Service stores it -> event bus -> Fanout Service finds eligible followers -> post IDs enter timeline caches.
  * Read path: user opens feed -> Feed Retrieval gets candidate IDs -> Ranking scores them -> Privacy filters them -> Content Store hydrates final post data -> response returned.
  * Engagement path: likes/comments/shares emit events -> counters and features update asynchronously.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Fanout strategy
  * Problem: fanout-on-write is fast for reads but expensive for users with huge follower counts.
  * Option 1: Fanout-on-write.
    * Pros: very fast feed reads, simple retrieval, good for normal users.
    * Cons: massive write amplification, celebrity posts create hot spots.
  * Option 2: Fanout-on-read.
    * Pros: avoids write explosion, works for celebrities and pages.
    * Cons: slower reads, ranking must query many sources.
  * Option 3: Hybrid fanout.
    * Pros: best practical balance; push normal users, pull celebrities/pages at read time.
    * Cons: more complex feed assembly and ranking.
  * Recommended: hybrid fanout with celebrity/page thresholds and backpressure.

*** Ranking freshness vs latency
  * Problem: fully ranking every possible post on every feed read is too slow.
  * Option 1: Precompute ranked feed.
    * Pros: low read latency.
    * Cons: stale, expensive to update after every engagement.
  * Option 2: Rank entirely at read time.
    * Pros: fresh and personalized.
    * Cons: high latency and compute cost.
  * Option 3: Candidate precompute + online ranking.
    * Pros: good balance of freshness and latency.
    * Cons: needs feature store, candidate cache, and model serving discipline.
  * Recommended: precompute candidates, rank a bounded set online, and refresh asynchronously.

---

## 2. Design Instagram

* Question
  * Design Instagram, including photo/video sharing and personalized feeds.

* Answer

** Scope
  * Include account profiles, media upload, post creation, followers, feed, likes/comments, stories as a related extension, and CDN delivery.
  * Exclude full direct messaging, ads, and complete recommendation ML.

** Functional Requirements
  * Users can upload photos/videos.
  * Users can follow/unfollow other users.
  * Users can create posts, like, comment, save, and view profiles.
  * Users can see home feed and user profile grids.
  * Media should have thumbnails and multiple resolutions.
  * Privacy settings must be enforced.

** Non Functional Requirements
  * High availability for reads and media delivery.
  * Durable media storage.
  * Low-latency feed and profile reads.
  * Eventual consistency for likes/comments counts.
  * Strong correctness for ownership, privacy, and deletion.
  * Scalable media processing pipeline.

** High level design and diagram (at block level)

```text
Mobile/Web Clients
  |
  v
API Gateway
  |
  +--> User/Profile Service --> User DB
  +--> Follow Graph Service --> Graph Store
  +--> Post Service --> Post DB
  +--> Media Upload Service --> Object Store --> CDN
  +--> Feed Service --> Timeline Cache
  +--> Engagement Service --> Counter Store
  +--> Privacy Service

Media Uploaded Event
  |
  v
Queue --> Media Processing Workers --> Derived Media Store/CDN
```

*** Explain the blocks
  * API Gateway: auth, request routing, throttling.
  * Media Upload Service: issues signed upload URLs and records media metadata.
  * Object Store/CDN: stores original and processed media; CDN serves hot content globally.
  * Media Processing Workers: resize, transcode, extract thumbnails, scan content.
  * Post Service: creates and retrieves posts.
  * Follow Graph Service: stores follower/following edges.
  * Feed Service: assembles home feed from timelines and recommendations.
  * Engagement Service: handles likes, comments, saves, and counters.

*** Explain the control flow
  * Platform teams configure media quality ladders, max file sizes, moderation policy, feed ranking versions, and CDN cache behavior.
  * Product teams configure feature flags for reels/stories/profile surfaces.
  * Rollouts use canaries by region, app version, and user cohort.

*** Explain the data flow
  * Upload flow: client requests upload URL -> uploads media to object store -> service emits processing event -> workers create variants -> metadata is updated.
  * Post flow: user creates post referencing processed media -> Post Service stores metadata -> feed fanout event updates follower timelines.
  * Read flow: user opens app -> Feed Service retrieves candidates -> privacy and ranking applied -> post/media URLs returned -> media served from CDN.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Media upload architecture
  * Problem: uploading large media through app servers wastes bandwidth and makes servers bottlenecks.
  * Option 1: Proxy uploads through API servers.
    * Pros: simple access control and validation.
    * Cons: expensive, slow, and hard to scale.
  * Option 2: Direct-to-object-store upload using signed URLs.
    * Pros: scalable, cheaper, resumable-friendly.
    * Cons: needs careful finalization and abuse controls.
  * Recommended: signed URLs plus finalize API, content scanning, and upload expiration.

*** Feed construction
  * Problem: profile grid is simple chronological retrieval, but home feed is personalized and high scale.
  * Option 1: Chronological feed.
    * Pros: simple and explainable.
    * Cons: lower relevance at large scale.
  * Option 2: Ranked feed.
    * Pros: better engagement and personalization.
    * Cons: complex, needs feature store and monitoring.
  * Recommended: ranked home feed, chronological profile grid, and separate candidate sources.

---

## 3. Design WhatsApp / Messenger

* Question
  * Design a global messaging app like WhatsApp or Messenger.

* Answer

** Scope
  * Include one-to-one messaging, group messaging, offline delivery, multi-device sync, media messages, presence, typing indicators, and push notifications.
  * Exclude voice/video calling and full end-to-end encryption protocol details unless asked.

** Functional Requirements
  * Users can send/receive messages in one-to-one and group chats.
  * Users receive messages when online or after reconnecting.
  * Clients show sent, delivered, and read receipts.
  * Support media attachments.
  * Support typing indicators and presence.
  * Maintain message order within a conversation.

** Non Functional Requirements
  * Very low latency for online messages.
  * High availability across regions.
  * Durable storage for offline users.
  * At-least-once delivery with deduplication.
  * Per-conversation ordering.
  * Privacy and security by design.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Connection Gateways (WebSocket/TCP)
  |
  v
Message Router
  |
  +--> Conversation Service --> Conversation DB
  +--> Message Store
  +--> Delivery Queue
  +--> Presence Service
  +--> Push Notification Service
  +--> Media Service --> Object Store/CDN
```

*** Explain the blocks
  * Connection Gateways: maintain long-lived client connections.
  * Message Router: routes messages to recipient sessions or queues.
  * Conversation Service: manages participants, group membership, and sequence numbers.
  * Message Store: durable message persistence.
  * Delivery Queue: stores pending messages per recipient/device.
  * Presence Service: tracks online/offline state with TTLs.
  * Push Notification Service: wakes offline clients.
  * Media Service: handles uploads and media references.

*** Explain the control flow
  * Operators configure region routing, connection limits, retry policy, spam limits, and encryption/version compatibility.
  * Group settings, membership changes, blocked users, and device registrations are control-plane data.
  * Feature flags can enable new receipt behavior or multi-device sync cohorts.

*** Explain the data flow
  * Sender sends message over persistent connection -> gateway authenticates -> router assigns conversation sequence -> message stored durably -> router delivers to online recipients or queues for offline devices.
  * Recipient ACKs delivery/read -> receipt event updates message state and is sent back to sender devices.
  * Media flow: upload media -> send message containing encrypted media pointer -> recipient downloads from CDN/object store.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Delivery semantics
  * Problem: network retries can create duplicates, but dropping messages is unacceptable.
  * Option 1: At-most-once delivery.
    * Pros: simple and no duplicates.
    * Cons: message loss on failure.
  * Option 2: At-least-once with message IDs.
    * Pros: reliable and practical.
    * Cons: duplicates possible unless clients/server dedupe.
  * Option 3: Exactly-once end-to-end.
    * Pros: ideal product semantics.
    * Cons: unrealistic with mobile networks, retries, and multi-device delivery.
  * Recommended: at-least-once with idempotency keys, per-conversation sequence numbers, and client dedupe.

*** Per-conversation ordering
  * Problem: users expect messages in a chat to appear in the same order.
  * Option 1: Global ordering.
    * Pros: simple mental model.
    * Cons: impossible or too expensive at global scale.
  * Option 2: Per-conversation ordering.
    * Pros: matches product need and scales.
    * Cons: hot groups need careful partitioning.
  * Recommended: sequence messages per conversation; shard by conversation ID; split very large groups into fanout partitions.

---

## 4. Design Facebook Chat / Real-Time Messaging

* Question
  * Design Facebook Chat focused on real-time online delivery.

* Answer

** Scope
  * Focus on online chat delivery, typing indicators, read receipts, connection management, and ephemeral state.
  * Exclude long-term media storage and full offline sync except basic fallback.

** Functional Requirements
  * Maintain persistent client sessions.
  * Route messages to online users in near real time.
  * Show typing indicators, presence, and read receipts.
  * Reconnect clients and resume missed messages.
  * Support multiple devices per user.

** Non Functional Requirements
  * Very low p95 message latency.
  * High connection fanout and efficient memory usage.
  * Graceful degradation during regional overload.
  * Eventually consistent presence.
  * Durable message fallback for disconnects.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Edge Connection Gateways
  |
  +--> Session Registry
  +--> Presence Service
  +--> Chat Router --> Message Log
  +--> Ephemeral Event Bus --> Typing/Presence Fanout
  +--> Offline Queue / Push Service
```

*** Explain the blocks
  * Edge Connection Gateways: terminate WebSocket/TCP connections.
  * Session Registry: maps user/device to active gateway.
  * Presence Service: stores online state with heartbeats and TTL.
  * Chat Router: routes messages to active gateways.
  * Message Log: durable append-only store for recovery.
  * Ephemeral Event Bus: handles typing indicators and transient events.
  * Offline Queue: stores missed messages for reconnect.

*** Explain the control flow
  * Control-plane configs define heartbeat intervals, connection limits, regional routing, overload behavior, and event TTLs.
  * Service discovery updates gateways with router/session-registry endpoints.
  * Feature flags control presence accuracy, receipt granularity, and reconnect behavior.

*** Explain the data flow
  * User connects -> gateway registers session -> heartbeats update presence.
  * Message arrives -> router checks session registry -> if recipient online, forwards to recipient gateway; otherwise stores in offline queue and triggers push.
  * Typing events flow through ephemeral bus and are not durably stored.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Presence correctness
  * Problem: exact presence is hard because mobile clients disconnect unpredictably.
  * Option 1: Strong presence state.
    * Pros: accurate when working.
    * Cons: expensive and brittle.
  * Option 2: TTL-based heartbeats.
    * Pros: scalable and failure-tolerant.
    * Cons: temporarily stale status.
  * Recommended: TTL heartbeats with client-side smoothing and "active recently" semantics.

*** Persistent connection scaling
  * Problem: millions of idle connections consume gateway memory.
  * Option 1: Stateless HTTP polling.
    * Pros: simple and compatible.
    * Cons: high latency and wasteful.
  * Option 2: WebSocket/TCP gateways.
    * Pros: low latency and efficient event push.
    * Cons: requires connection balancing and session migration.
  * Recommended: regional connection gateways with consistent routing and reconnect tokens.

---

## 5. Design Instagram Stories / Reels

* Question
  * Design Instagram Stories or Reels for global scale.

* Answer

** Scope
  * Include upload, processing, story/reel metadata, expiry for stories, ranking/tray generation, viewer tracking, and media delivery.
  * Exclude full creator monetization and full ads insertion.

** Functional Requirements
  * Users can upload short videos/images.
  * Stories expire after a configured time, commonly 24 hours.
  * Users can view stories/reels with low startup latency.
  * System records views, likes, replies, and skips.
  * Feed/tray is personalized and ranked.
  * Media supports multiple bitrates/resolutions.

** Non Functional Requirements
  * Low video startup time.
  * High throughput media processing.
  * CDN-heavy delivery.
  * Eventual consistency for view counts.
  * Reliable expiry/deletion.
  * Cost control for transcoding and prefetching.

** High level design and diagram (at block level)

```text
Clients
  |
  v
API Gateway
  |
  +--> Upload Service --> Object Store
  +--> Story/Reel Metadata Service --> Metadata DB
  +--> Tray/Feed Service --> Ranking Service
  +--> View/Event Service --> Event Stream
  +--> CDN Playback Service

Upload Event --> Queue --> Transcoding Workers --> CDN/Object Store
Expiry Scheduler --> Metadata DB / CDN Purge
```

*** Explain the blocks
  * Upload Service: signed uploads and upload finalization.
  * Transcoding Workers: generate playback variants and thumbnails.
  * Metadata Service: stores creator, visibility, expiry, media pointers.
  * Tray/Feed Service: returns story tray or reel candidates.
  * Ranking Service: scores content.
  * View/Event Service: records views, completion, skips, likes.
  * Expiry Scheduler: hides/removes expired stories.

*** Explain the control flow
  * Product controls ranking rules, expiry policies, creator eligibility, experiment variants, and playback quality ladder.
  * Operations controls CDN purge policy, transcoding backpressure, and regional failover.
  * Safety/moderation policies are versioned and applied before broad distribution.

*** Explain the data flow
  * Creator uploads media -> object store emits event -> transcoders create variants -> metadata becomes publishable.
  * Viewer opens stories/reels -> tray/feed service gets candidates -> ranking chooses order -> playback URLs served via CDN.
  * View events stream into counters, ranking features, and analytics.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Video delivery and prefetch
  * Problem: users expect instant playback, but video transfer is expensive.
  * Option 1: No prefetch.
    * Pros: saves bandwidth.
    * Cons: high startup latency.
  * Option 2: Aggressive prefetch.
    * Pros: excellent UX.
    * Cons: high bandwidth cost and battery usage.
  * Option 3: Predictive prefetch.
    * Pros: balances UX and cost.
    * Cons: needs ranking confidence and device/network awareness.
  * Recommended: prefetch top few likely items with adaptive limits.

*** Story expiry
  * Problem: stories must disappear on time, but deleting every copy instantly is hard.
  * Option 1: Physical delete exactly at expiry.
    * Pros: clean semantics.
    * Cons: costly and brittle with caches/CDNs.
  * Option 2: Logical expiry in metadata.
    * Pros: fast and reliable for product visibility.
    * Cons: media may remain in storage until cleanup.
  * Recommended: enforce logical expiry synchronously, then async cleanup and CDN purge.

---

## 6. Design Video Upload And Sharing

* Question
  * Design a video upload and sharing platform.

* Answer

** Scope
  * Include upload, resumability, transcoding, metadata, moderation hooks, playback, thumbnails, and sharing.
  * Exclude live streaming and advanced recommendations.

** Functional Requirements
  * Users can upload large videos reliably.
  * System creates multiple encodings and thumbnails.
  * Users can view and share videos.
  * Video metadata and visibility are editable.
  * Processing status is visible.
  * Playback supports adaptive bitrate.

** Non Functional Requirements
  * Durable storage for original and derived videos.
  * Scalable asynchronous processing.
  * Low playback startup latency.
  * Backpressure during upload/processing spikes.
  * Fault-tolerant job retries.

** High level design and diagram (at block level)

```text
Client
  |
  v
Upload API --> Signed Upload URL --> Object Store
  |
  v
Video Metadata Service --> Metadata DB
  |
Upload Complete Event
  |
  v
Processing Queue --> Transcode Workers --> Derived Video Store --> CDN
                         |
                         v
                  Thumbnail/Moderation Services

Playback API --> Metadata DB --> CDN URLs
```

*** Explain the blocks
  * Upload API: initializes upload and validates ownership.
  * Object Store: stores raw video chunks and final originals.
  * Metadata Service: tracks title, owner, status, visibility, and media variants.
  * Processing Queue: decouples upload from heavy processing.
  * Transcode Workers: create bitrate/resolution variants.
  * Playback API: returns manifest and CDN URLs.
  * CDN: serves video segments at scale.

*** Explain the control flow
  * Control plane defines encoding ladders, worker capacity, queue priorities, moderation rules, and CDN policies.
  * Operators can pause a codec rollout, replay failed jobs, or throttle lower-priority processing.
  * Product configs decide whether a video is visible before full HD processing completes.

*** Explain the data flow
  * Client initializes upload -> receives signed URL -> uploads chunks -> finalizes upload -> event queues processing.
  * Workers read original -> transcode variants -> write outputs -> update metadata.
  * Viewer requests playback -> Playback API returns manifest -> client streams segments from CDN.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Resumable uploads
  * Problem: large mobile uploads often fail mid-transfer.
  * Option 1: Single-shot upload.
    * Pros: simple.
    * Cons: poor reliability for large files.
  * Option 2: Chunked resumable upload.
    * Pros: robust and bandwidth-efficient after failure.
    * Cons: more metadata and finalization complexity.
  * Recommended: chunked upload with checksum validation and upload session expiration.

*** Publish timing
  * Problem: waiting for all encodings delays sharing; early publishing may degrade experience.
  * Option 1: Publish after all processing.
    * Pros: consistent playback quality.
    * Cons: slow.
  * Option 2: Progressive publish after first playable rendition.
    * Pros: fast user feedback.
    * Cons: early viewers may see lower quality.
  * Recommended: publish after safe/moderated first rendition, then upgrade variants as ready.

---

## 7. Design Facebook Live / Live Streaming

* Question
  * Design a live streaming system for Facebook Live.

* Answer

** Scope
  * Include broadcaster ingest, live transcoding, segment distribution, viewer playback, live comments/reactions, and failover.
  * Exclude creator monetization and post-live video editing.

** Functional Requirements
  * Broadcasters can start/stop live streams.
  * Viewers can watch with low latency.
  * System supports adaptive bitrate playback.
  * Viewers can comment/react live.
  * Stream can be recorded for replay.
  * System handles broadcaster/network failure.

** Non Functional Requirements
  * Low glass-to-glass latency.
  * High availability for popular streams.
  * Regional ingest points.
  * Scalable CDN distribution.
  * Backpressure and graceful quality degradation.
  * Strong observability for stream health.

** High level design and diagram (at block level)

```text
Broadcaster
  |
  v
Regional Ingest Service
  |
  v
Live Transcoding Pipeline
  |
  +--> Segment Store --> CDN --> Viewers
  +--> Recording Store
  +--> Stream Health Monitor

Viewer API --> Stream Metadata Service
Live Events API --> Comment/Reaction Stream --> Fanout to Viewers
```

*** Explain the blocks
  * Regional Ingest Service: receives broadcaster stream near source.
  * Transcoding Pipeline: converts stream into multiple bitrates.
  * Segment Store: stores short HLS/DASH/CMAF segments.
  * CDN: distributes segments to viewers.
  * Stream Metadata Service: tracks live status, manifests, and permissions.
  * Live Events API: handles comments and reactions.
  * Health Monitor: tracks bitrate, dropped frames, lag, and failures.

*** Explain the control flow
  * Creator starts stream -> metadata service creates stream session and ingest endpoint.
  * Control plane configures bitrate ladder, moderation rules, latency mode, CDN routing, and failover.
  * Operators can switch ingest regions, disable a stream, or change latency/quality settings.

*** Explain the data flow
  * Broadcaster sends stream to ingest -> transcoders produce segments -> CDN serves segments to viewers.
  * Viewer requests manifest -> receives CDN segment URLs -> downloads segments continuously.
  * Comments/reactions go through event stream and are fanned out to active viewers.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Latency vs reliability
  * Problem: smaller segments lower latency but increase overhead and rebuffer risk.
  * Option 1: Traditional HLS with larger segments.
    * Pros: stable and CDN-friendly.
    * Cons: higher latency.
  * Option 2: Low-latency HLS/DASH/CMAF.
    * Pros: lower latency.
    * Cons: more operational complexity and player sensitivity.
  * Recommended: offer latency modes; use low latency for interactive streams, standard mode for massive broadcasts.

*** Ingest failover
  * Problem: broadcaster connection or regional ingest can fail mid-stream.
  * Option 1: Single ingest endpoint.
    * Pros: simple.
    * Cons: fragile.
  * Option 2: Multi-region ingest failover.
    * Pros: better reliability.
    * Cons: reconnection and stream continuity complexity.
  * Recommended: regional primary ingest with backup endpoint, health detection, and client reconnect tokens.

---

## 8. Design Live Commenting

* Question
  * Design live commenting for posts, live videos, or events.

* Answer

** Scope
  * Include comment creation, ordering, moderation hooks, real-time fanout, pagination, and counters.
  * Exclude full rich-text editor and advanced ranking unless asked.

** Functional Requirements
  * Users can post comments on an object.
  * Viewers receive new comments in near real time.
  * Users can paginate historical comments.
  * Comments can be deleted, hidden, reported, or moderated.
  * Counts update eventually.
  * System supports high-traffic objects.

** Non Functional Requirements
  * Low latency for active viewers.
  * Durable comment storage.
  * Hot-object scalability.
  * Moderation safety.
  * Eventual consistency acceptable for counters.
  * Per-object ordering.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Comment API
  |
  +--> Moderation Service
  +--> Comment Store
  +--> Counter Service
  |
  v
Comment Event Stream
  |
  +--> Real-Time Fanout Service --> WebSocket/SSE Gateways --> Viewers
  +--> Search/Analytics Index
```

*** Explain the blocks
  * Comment API: validates, authenticates, rate-limits, and stores comments.
  * Moderation Service: blocks or flags unsafe content.
  * Comment Store: sharded by object ID and time/comment ID.
  * Counter Service: maintains approximate counts.
  * Event Stream: durable stream for fanout and analytics.
  * Real-Time Fanout Service: sends new comments to active viewers.
  * Gateways: maintain viewer connections.

*** Explain the control flow
  * Moderation policies, rate limits, blocked terms, and per-surface display rules are configured centrally.
  * Product can tune whether comments are chronological, ranked, or filtered by viewer.
  * Operations can throttle comments on hot objects or enable slow mode.

*** Explain the data flow
  * User posts comment -> Comment API validates and stores -> event emitted -> fanout service sends to connected viewers.
  * Viewer opens comments -> API reads recent comments from store/cache -> subscribes to live updates.
  * Delete/moderation events propagate through the stream and update clients.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Hot-object fanout
  * Problem: one viral live video may have millions of viewers and thousands of comments per second.
  * Option 1: Push every comment to every viewer.
    * Pros: fully real-time.
    * Cons: impossible at extreme scale.
  * Option 2: Sample/batch comments.
    * Pros: scalable and cheaper.
    * Cons: not every viewer sees every comment immediately.
  * Option 3: Tiered fanout with hot-object mode.
    * Pros: preserves UX while controlling load.
    * Cons: more complex mode switching.
  * Recommended: push all comments for normal objects; batch/sample/rank for hot objects.

*** Ordering
  * Problem: exact global order is expensive under distributed writes.
  * Option 1: Global total order.
    * Pros: simple display semantics.
    * Cons: bottleneck and cross-region latency.
  * Option 2: Per-object order using generated sequence/time IDs.
    * Pros: scalable and matches product need.
    * Cons: minor tie-breaking complexity.
  * Recommended: per-object ordering with monotonic IDs generated by object shard.

---

## 9. Design Notifications

* Question
  * Design a notification platform for Meta products.

* Answer

** Scope
  * Include event ingestion, preference evaluation, dedupe, rate limits, templates, push/email/SMS/in-app channels, retries, and delivery tracking.
  * Exclude provider-specific implementation details.

** Functional Requirements
  * Product teams can trigger notifications.
  * Users can receive push, email, SMS, and in-app notifications.
  * Users can configure preferences.
  * System supports templates and localization.
  * Deduplicate repeated notifications.
  * Track delivery, opens, clicks, and failures.

** Non Functional Requirements
  * High availability and durable event ingestion.
  * At-least-once processing with idempotency.
  * Low latency for push/in-app.
  * Rate limiting and anti-spam.
  * Auditable policy decisions.
  * Channel isolation so SMS failures do not break push.

** High level design and diagram (at block level)

```text
Product Services
  |
  v
Notification Ingestion API
  |
  v
Durable Event Queue
  |
  v
Notification Orchestrator
  |
  +--> Preference Service
  +--> Template Service
  +--> Policy/Compliance Service
  +--> Dedupe Store
  +--> Rate Limit Service
  |
  +--> Push Queue --> Push Workers --> APNS/FCM
  +--> Email Queue --> Email Workers --> Email Provider
  +--> SMS Queue --> SMS Workers --> SMS Provider
  +--> In-App Store

Delivery Events --> Analytics/Monitoring
```

*** Explain the blocks
  * Ingestion API: validates producers and accepts notification intents.
  * Event Queue: durable buffer for processing.
  * Orchestrator: decides whether, when, and how to send.
  * Preference Service: user opt-ins and channel settings.
  * Template Service: renders localized content.
  * Policy/Compliance Service: legal, safety, quiet hours, and spam rules.
  * Channel Queues/Workers: isolate delivery channels.
  * In-App Store: stores notifications for inbox surfaces.

*** Explain the control flow
  * Product teams configure notification types, templates, priority, experiments, and rate rules.
  * Compliance teams configure opt-in, quiet hours, blocked categories, and regional policies.
  * Operations configure provider failover, retry budgets, and circuit breakers.

*** Explain the data flow
  * Product event arrives -> orchestrator checks preferences/policy -> dedupe/rate limit -> render template -> enqueue channel jobs.
  * Channel workers call providers -> provider responses generate delivery events -> analytics and state stores update.
  * In-app notifications are persisted and read by clients.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Delivery guarantees
  * Problem: external providers do not guarantee exactly-once user-visible delivery.
  * Option 1: At-most-once.
    * Pros: avoids duplicates.
    * Cons: drops notifications during failures.
  * Option 2: At-least-once with dedupe.
    * Pros: reliable and practical.
    * Cons: duplicates can still leak through provider/client edge cases.
  * Recommended: at-least-once internally with idempotency keys, dedupe windows, and product copy tolerant of rare duplicates.

*** Rate limiting
  * Problem: product teams can overwhelm users or providers.
  * Option 1: Global synchronous limiter.
    * Pros: accurate limits.
    * Cons: latency and availability bottleneck.
  * Option 2: Local/regional limiters.
    * Pros: fast and resilient.
    * Cons: temporary quota overshoot.
  * Option 3: Hybrid limiter.
    * Pros: good practical balance.
    * Cons: more complex accounting.
  * Recommended: local fast-path limits plus async global reconciliation; strict sync checks for compliance-sensitive limits.

---

## 10. Design Large-Scale Proximity Service / Nearby Friends

* Question
  * Design a service that finds nearby friends or nearby users.

* Answer

** Scope
  * Include location ingestion, geospatial indexing, privacy filters, nearby queries, TTL-based freshness, and notifications.
  * Exclude turn-by-turn navigation and precise background tracking policy details.

** Functional Requirements
  * Users can opt in/out of location sharing.
  * Clients periodically send location updates.
  * Users can query nearby friends.
  * System returns approximate distance and recency.
  * Privacy and block rules are enforced.
  * Location expires after a TTL.

** Non Functional Requirements
  * Low-latency nearby lookups.
  * High write throughput from mobile updates.
  * Strong privacy controls.
  * Data minimization and TTL cleanup.
  * Approximate results acceptable.
  * Regional data residency where required.

** High level design and diagram (at block level)

```text
Mobile Clients
  |
  v
Location Update API
  |
  +--> Consent/Privacy Service
  +--> Location Store (TTL)
  +--> Geo Index (S2/Geohash Cells)

Nearby Query API
  |
  +--> Social Graph Service
  +--> Geo Index
  +--> Privacy/Block Service
  +--> Ranking/Distance Service
```

*** Explain the blocks
  * Location Update API: validates and rate-limits updates.
  * Consent/Privacy Service: enforces opt-in and sharing settings.
  * Location Store: stores latest location with timestamp and TTL.
  * Geo Index: maps users to geospatial cells.
  * Nearby Query API: checks neighboring cells and filters candidates.
  * Social Graph Service: restricts results to friends or allowed audiences.
  * Ranking/Distance Service: computes approximate distance and sorts.

*** Explain the control flow
  * Control plane manages location precision, TTL, update frequency, geofence rules, and regional privacy policy.
  * Users manage opt-in/out and audience settings.
  * Operators can reduce update frequency during load or disable notifications by region.

*** Explain the data flow
  * Client sends location -> privacy check -> convert to geocell -> store latest location and cell membership with TTL.
  * Nearby query -> get user's cell -> fetch neighboring cells -> intersect with friends/allowed users -> filter privacy/block -> return ranked results.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Geospatial indexing
  * Problem: scanning all friends/users is too slow.
  * Option 1: Store raw lat/lon and scan.
    * Pros: simple.
    * Cons: does not scale.
  * Option 2: Geohash/S2 cell index.
    * Pros: scalable neighborhood queries.
    * Cons: edge cases at cell boundaries.
  * Recommended: S2/geohash cells plus neighboring-cell lookup and final distance filtering.

*** Privacy vs precision
  * Problem: exact location is sensitive.
  * Option 1: Precise lat/lon.
    * Pros: accurate.
    * Cons: high privacy risk.
  * Option 2: Coarse cells or fuzzed location.
    * Pros: safer and often enough for nearby UX.
    * Cons: less accurate.
  * Recommended: store minimum precision required, show approximate distance, enforce TTL, and centralize privacy checks.

---

## 11. Design Search Autocomplete

* Question
  * Design autocomplete for a search engine or Meta search box.

* Answer

** Scope
  * Include prefix suggestions for people, pages, groups, hashtags, and queries.
  * Exclude full search results ranking.

** Functional Requirements
  * Return suggestions as users type.
  * Support typo tolerance, personalization, and trending queries.
  * Filter suggestions by privacy/visibility.
  * Support multiple languages/locales.
  * Update popular/trending terms quickly.

** Non Functional Requirements
  * Very low latency, often p95 below 100 ms.
  * High QPS due to keystroke traffic.
  * High availability.
  * Compact indexes in memory.
  * Eventual freshness acceptable for most suggestions.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Autocomplete API
  |
  +--> Prefix Index Service
  +--> Personalization Service
  +--> Trending Overlay
  +--> Privacy Filter
  +--> Cache

Offline Logs/Entities --> Index Builder --> Prefix Index Shards
Real-Time Events --> Trending Stream Processor --> Trending Overlay
```

*** Explain the blocks
  * Autocomplete API: handles query prefix and user context.
  * Prefix Index Service: serves top suggestions by prefix from trie/FST-like structures.
  * Personalization Service: boosts friends, groups, interests, and recent searches.
  * Trending Overlay: adds fresh popular queries.
  * Privacy Filter: removes invisible people/groups/content.
  * Index Builder: periodically rebuilds compact prefix indexes.

*** Explain the control flow
  * Search team configures ranking weights, language analyzers, typo thresholds, blocked terms, and rollout versions.
  * Index builds are versioned and atomically swapped.
  * Experiments tune personalization vs global popularity.

*** Explain the data flow
  * User types prefix -> API checks cache -> prefix shard returns candidates -> personalization boosts -> privacy filters -> response returned.
  * Query logs and entity updates feed offline index builder.
  * Real-time stream updates trending overlay for fresh suggestions.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Index structure
  * Problem: prefix lookup must be fast and memory efficient.
  * Option 1: Database LIKE queries.
    * Pros: easy to build.
    * Cons: too slow at scale.
  * Option 2: Trie.
    * Pros: natural prefix lookup.
    * Cons: can be memory-heavy.
  * Option 3: FST/compact prefix index.
    * Pros: memory efficient and fast.
    * Cons: more complex to build.
  * Recommended: compact prefix index/FST with top-k suggestions per prefix.

*** Freshness
  * Problem: offline indexes are stale for breaking trends.
  * Option 1: Full rebuild frequently.
    * Pros: simple serving.
    * Cons: expensive and still not real-time.
  * Option 2: Real-time overlay.
    * Pros: fresh trends without full rebuild.
    * Cons: merge complexity.
  * Recommended: periodic base index plus streaming overlay.

---

## 12. Design Facebook Status Search

* Question
  * Design search over Facebook posts/statuses.

* Answer

** Scope
  * Include indexing posts, searching text, permissions filtering, ranking, and deletion/update handling.
  * Exclude full web search and media understanding unless asked.

** Functional Requirements
  * Users can search visible posts/statuses.
  * Search supports keyword queries, filters, pagination, and ranking.
  * Results respect privacy, blocks, groups, and deleted content.
  * Index updates when posts are created, edited, or deleted.
  * Support recent and relevant results.

** Non Functional Requirements
  * Low query latency.
  * High indexing throughput.
  * Near-real-time indexing for fresh posts.
  * Strong privacy correctness.
  * Scalable inverted index.
  * Fault-tolerant reindexing.

** High level design and diagram (at block level)

```text
Post Service --> Post Event Stream --> Indexing Pipeline --> Inverted Index Shards
                                      |
                                      +--> Permission Metadata Index

Client --> Search API --> Query Parser
                    |
                    +--> Index Broker --> Index Shards
                    +--> Privacy/ACL Service
                    +--> Ranking Service
                    +--> Post Store Hydration
```

*** Explain the blocks
  * Post Event Stream: emits create/update/delete events.
  * Indexing Pipeline: tokenizes, normalizes, and writes documents to index shards.
  * Inverted Index Shards: map terms to posting lists.
  * Permission Metadata Index: stores visibility attributes.
  * Search API/Query Parser: validates and parses queries.
  * Index Broker: fans query to relevant shards and merges results.
  * Privacy/ACL Service: filters inaccessible results.
  * Ranking Service: orders by relevance, recency, engagement, and affinity.

*** Explain the control flow
  * Search team configures analyzers, ranking models, index versions, shard assignments, and blocked terms.
  * Reindex jobs are scheduled and monitored.
  * Privacy policy changes can trigger ACL metadata refresh or query-time policy updates.

*** Explain the data flow
  * Post created/updated -> event stream -> indexing pipeline writes terms and metadata.
  * Search query -> parser -> index shards return doc IDs -> privacy filtering -> ranking -> hydrate post snippets -> response.
  * Delete event -> tombstone or remove doc from index; query path must honor deletion quickly.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Privacy filtering
  * Problem: search must never show private posts.
  * Option 1: Precompute visibility into index.
    * Pros: fast queries.
    * Cons: hard to update when relationships/privacy change.
  * Option 2: Query-time ACL filtering.
    * Pros: correct and flexible.
    * Cons: expensive for large candidate sets.
  * Option 3: Hybrid.
    * Pros: coarse prefilter plus precise final check.
    * Cons: more metadata complexity.
  * Recommended: hybrid; index coarse visibility, apply final privacy check before response.

*** Sharding
  * Problem: index must scale for enormous document volume.
  * Option 1: Shard by document ID.
    * Pros: balanced indexing and simple updates.
    * Cons: queries fan out broadly.
  * Option 2: Shard by term.
    * Pros: targeted term lookup.
    * Cons: hot terms and complex ranking merge.
  * Recommended: document-based shards with broker fanout, caching for popular queries, and special handling for hot terms.

---

## 13. Design Search And Recommendation System

* Question
  * Design a search and recommendation system for Meta content.

* Answer

** Scope
  * Include candidate generation, ranking, feature store, embedding retrieval, feedback events, and online serving.
  * Exclude full ML model training details unless asked.

** Functional Requirements
  * Return relevant content/users/groups/pages for a user/query/context.
  * Support personalization.
  * Incorporate fresh engagement signals.
  * Filter unsafe/private content.
  * Learn from user interactions.
  * Support experimentation.

** Non Functional Requirements
  * Low serving latency.
  * High throughput.
  * Fresh features for ranking.
  * Safe filtering before display.
  * Observable quality and regressions.
  * Cost-efficient candidate retrieval.

** High level design and diagram (at block level)

```text
Client --> Search/Rec API
              |
              +--> Candidate Generators
              |       +--> Graph Candidates
              |       +--> Text Index
              |       +--> Embedding ANN Index
              |       +--> Trending/Popular Store
              |
              +--> Feature Store
              +--> Ranking Service / Model Serving
              +--> Policy/Privacy Filter
              +--> Result Hydration

Events --> Stream Processor --> Feature Store
Logs --> Offline Training/Index Builder --> Models/Indexes
```

*** Explain the blocks
  * Search/Rec API: receives context and orchestrates retrieval.
  * Candidate Generators: produce a broad set from graph, text, embeddings, and popularity.
  * Feature Store: serves user/content/context features.
  * Ranking Service: scores candidates with ML models.
  * Policy/Privacy Filter: removes invalid candidates.
  * Result Hydration: fetches display metadata.
  * Offline Training/Index Builder: produces models and indexes.

*** Explain the control flow
  * ML/platform teams publish model versions, feature definitions, ANN index versions, and experiment configs.
  * Safety teams publish policy filters and blocklists.
  * Rollouts compare online metrics, guardrails, and rollback criteria.

*** Explain the data flow
  * Request arrives -> candidate generators return hundreds/thousands of candidates -> features fetched -> model ranks candidates -> policy filters -> hydrated results returned.
  * User events stream into feature updates and offline logs for training.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Candidate generation vs ranking
  * Problem: ranking all content is impossible.
  * Option 1: One-stage ranking over huge corpus.
    * Pros: conceptually simple.
    * Cons: too slow and expensive.
  * Option 2: Two-stage retrieval and ranking.
    * Pros: scalable and standard.
    * Cons: candidate recall becomes critical.
  * Recommended: multiple candidate generators for recall, followed by heavier ranking on bounded candidates.

*** Feature freshness
  * Problem: stale features reduce relevance, but real-time features are expensive.
  * Option 1: Offline-only features.
    * Pros: cheap and stable.
    * Cons: stale.
  * Option 2: Real-time feature store.
    * Pros: fresh and responsive.
    * Cons: operationally complex.
  * Option 3: Hybrid.
    * Pros: practical balance.
    * Cons: consistency/debugging complexity.
  * Recommended: hybrid with critical real-time counters and stable offline embeddings.

---

## 14. Design Social Graph / Follow Graph

* Question
  * Design Meta's social graph or follow graph service.

* Answer

** Scope
  * Include friend/follow edges, block edges, groups/pages relationships, mutual friends, graph reads for feeds/search/privacy, and edge updates.
  * Exclude full graph ML and recommendations unless asked.

** Functional Requirements
  * Users can friend, follow, unfollow, block, and unblock.
  * Services can query friends/followers/following.
  * Support mutual friends and relationship checks.
  * Support privacy-critical block checks.
  * Support high-fanout users/pages.
  * Emit graph change events.

** Non Functional Requirements
  * Low-latency relationship checks.
  * High read throughput.
  * Strong correctness for block/privacy checks.
  * Eventual consistency acceptable for follower counts and recommendations.
  * Scalable adjacency storage.
  * Hot-node handling for celebrities/pages.

** High level design and diagram (at block level)

```text
Clients/Services
  |
  v
Graph API
  |
  +--> Edge Write Service --> Graph Edge Store
  +--> Relationship Query Service --> Graph Cache
  +--> Mutual Friend Service
  +--> Block/Privacy Edge Service
  |
  v
Graph Change Event Stream --> Feed/Search/Notification Consumers
```

*** Explain the blocks
  * Graph API: standard interface for relationship reads/writes.
  * Edge Write Service: validates and writes relationship changes.
  * Graph Edge Store: stores forward/reverse adjacency lists.
  * Graph Cache: caches hot adjacency lists and relationship checks.
  * Mutual Friend Service: computes intersections.
  * Block/Privacy Edge Service: optimized for correctness-sensitive checks.
  * Event Stream: notifies downstream systems of graph changes.

*** Explain the control flow
  * Control plane defines edge types, privacy semantics, cache TTLs, hot-user thresholds, and backfill jobs.
  * Schema changes are versioned because many systems depend on graph data.
  * Operators can invalidate graph caches after policy bugs or migration events.

*** Explain the data flow
  * Follow request -> validation -> write forward and reverse edge -> emit graph event -> feed/search/notification systems update.
  * Relationship check -> Graph API checks cache/store -> returns relationship/visibility result.
  * Mutual friends -> fetch smaller adjacency list -> intersect with cached friend set.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Edge storage model
  * Problem: graph reads need both outgoing and incoming edges.
  * Option 1: Store only one direction.
    * Pros: simpler writes.
    * Cons: expensive reverse queries.
  * Option 2: Store forward and reverse edges.
    * Pros: fast reads in both directions.
    * Cons: write amplification and consistency issues.
  * Recommended: store both directions with idempotent writes and repair jobs.

*** Hot users/pages
  * Problem: celebrities/pages have huge follower lists.
  * Option 1: Store as normal adjacency list.
    * Pros: simple.
    * Cons: hot partitions and huge fanout.
  * Option 2: Partition adjacency lists.
    * Pros: scales reads/writes.
    * Cons: more complex pagination and fanout.
  * Recommended: partition large adjacency lists and use hybrid feed fanout.

---

## 15. Design A Key-Value Database

* Question
  * Build a distributed key-value database.

* Answer

** Scope
  * Include get/put/delete, sharding, replication, durability, compaction, failure recovery, and consistency options.
  * Exclude SQL joins and complex transactions unless asked.

** Functional Requirements
  * Clients can get, put, and delete keys.
  * Support TTL and conditional writes if needed.
  * Data survives node failure.
  * Cluster can add/remove nodes.
  * Support backups and repair.
  * Expose consistency behavior clearly.

** Non Functional Requirements
  * High availability.
  * Low read/write latency.
  * Horizontal scalability.
  * Durable writes.
  * Bounded data loss under failures.
  * Operationally manageable rebalancing.

** High level design and diagram (at block level)

```text
Clients
  |
  v
KV Router / Coordinator
  |
  +--> Placement/Ring Metadata Service
  |
  +--> Storage Node A
  +--> Storage Node B
  +--> Storage Node C

Storage Node:
  WAL -> Memtable -> SSTables/LSM -> Compaction
  Replication -> Read Repair / Anti-Entropy
```

*** Explain the blocks
  * KV Router/Coordinator: routes keys to replica set and coordinates reads/writes.
  * Placement Metadata Service: maps key ranges/hash tokens to nodes.
  * Storage Nodes: store partitions and replicas.
  * WAL: durable write-ahead log.
  * Memtable/SSTables: LSM-tree storage engine.
  * Compaction: merges SSTables and removes tombstones.
  * Anti-Entropy/Repair: fixes replica divergence.

*** Explain the control flow
  * Operators configure replication factor, consistency level, compaction policy, node membership, and rebalancing.
  * Membership changes update placement metadata and trigger streaming/rebalancing.
  * Backups, repair, and compaction are controlled background workflows.

*** Explain the data flow
  * Put: client -> coordinator -> replica nodes append WAL/update memtable -> acknowledgments based on consistency level.
  * Get: client -> coordinator -> one or more replicas -> reconcile versions if needed -> return value.
  * Delete: write tombstone -> compaction eventually removes data after safety window.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Consistency model
  * Problem: strong consistency reduces anomalies but hurts availability/latency.
  * Option 1: Leader-based strong consistency.
    * Pros: simpler semantics.
    * Cons: leader bottleneck and failover latency.
  * Option 2: Quorum reads/writes.
    * Pros: tunable consistency and availability.
    * Cons: conflict resolution complexity.
  * Option 3: Eventual consistency.
    * Pros: high availability and low latency.
    * Cons: stale reads and conflicts.
  * Recommended: expose tunable consistency; use quorum for critical data and eventual for cache-like workloads.

*** Partitioning
  * Problem: data and traffic must distribute evenly.
  * Option 1: Range partitioning.
    * Pros: efficient range scans.
    * Cons: hot ranges.
  * Option 2: Hash partitioning.
    * Pros: balanced load.
    * Cons: poor range scans.
  * Recommended: hash partitioning for general KV; add range-aware design only if range scans are required.

---

## 16. Design A Distributed File System

* Question
  * Design a distributed file system.

* Answer

** Scope
  * Include file metadata, chunking, storage nodes, replication, reads/writes, failure recovery, and consistency.
  * Exclude POSIX-perfect semantics unless explicitly required.

** Functional Requirements
  * Clients can create, read, write, delete, and list files/directories.
  * Large files are split into chunks.
  * Chunks are replicated.
  * Metadata tracks file hierarchy and chunk locations.
  * System detects and repairs corrupt/lost chunks.
  * Supports access control.

** Non Functional Requirements
  * High durability.
  * High throughput for large files.
  * Scalable storage capacity.
  * Metadata availability.
  * Fault tolerance for node/rack failures.
  * Efficient rebalancing.

** High level design and diagram (at block level)

```text
Clients
  |
  v
Metadata Service
  |
  +--> Namespace Store
  +--> Chunk Location Map
  +--> ACL Service
  |
  v
Chunk Storage Nodes
  +--> Replication Manager
  +--> Checksum/Repair Service
  +--> Rebalancer
```

*** Explain the blocks
  * Metadata Service: manages namespace, file metadata, and chunk locations.
  * Namespace Store: stores directories and file entries.
  * Chunk Location Map: maps file chunks to storage nodes.
  * Chunk Storage Nodes: store chunk bytes.
  * Replication Manager: maintains desired replication factor.
  * Checksum/Repair Service: detects corruption and repairs chunks.
  * Rebalancer: moves chunks when nodes are added/removed.

*** Explain the control flow
  * Operators configure chunk size, replication factor, placement policy, repair priority, and quota.
  * Metadata service controls chunk allocation and leases.
  * Repair/rebalance jobs are scheduled based on health metrics.

*** Explain the data flow
  * Write: client asks metadata service for chunk placements -> client writes chunks to storage nodes -> replicas are written -> metadata committed.
  * Read: client asks metadata service for chunk locations -> reads chunks directly from nearest/healthy nodes.
  * Failure: health monitor marks node unhealthy -> replication manager creates new replicas from surviving chunks.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Metadata scaling
  * Problem: metadata can become a bottleneck.
  * Option 1: Single metadata leader.
    * Pros: simple consistency.
    * Cons: scale and availability bottleneck.
  * Option 2: Sharded metadata.
    * Pros: scalable.
    * Cons: complex rename/list operations and transactions.
  * Recommended: start with replicated metadata leader for simpler semantics; shard namespace for large scale.

*** Write consistency
  * Problem: concurrent writes can corrupt file/chunk semantics.
  * Option 1: Strict POSIX semantics.
    * Pros: familiar and correct.
    * Cons: hard to scale.
  * Option 2: Append/lease-based writes.
    * Pros: scalable for large files.
    * Cons: weaker semantics.
  * Recommended: define weaker semantics explicitly; use leases for chunk writes and atomic metadata commits.

---

## 17. Design Photo Storage And CDN Delivery

* Question
  * Design a photo storage and delivery system for Instagram/Facebook.

* Answer

** Scope
  * Include upload, image processing, variant generation, metadata, object storage, CDN caching, deletion, and access control.
  * Exclude feed ranking and comments.

** Functional Requirements
  * Users can upload photos.
  * System generates thumbnails and multiple resolutions.
  * Clients can fetch appropriate image variant.
  * Access control and privacy are enforced.
  * Photos can be deleted or hidden.
  * Hot photos are served efficiently.

** Non Functional Requirements
  * Very high read throughput.
  * Low image load latency globally.
  * Durable original storage.
  * Cost-efficient variant generation.
  * CDN cacheability.
  * Correct deletion/visibility semantics.

** High level design and diagram (at block level)

```text
Client
  |
  v
Photo Upload API --> Signed URL --> Object Store (Original)
  |
Upload Event
  |
  v
Image Processing Queue --> Resizer/Optimizer Workers --> Variant Store --> CDN

Photo Metadata Service --> Metadata DB
Photo Read API --> Privacy Service --> CDN URLs
```

*** Explain the blocks
  * Photo Upload API: authorizes upload and creates media ID.
  * Object Store: stores original immutable image.
  * Image Workers: resize, compress, strip metadata, create thumbnails.
  * Variant Store: stores derived images.
  * CDN: caches and serves variants.
  * Metadata Service: maps photo ID to owner, visibility, variants.
  * Privacy Service: checks viewer access.

*** Explain the control flow
  * Control plane defines image sizes, quality levels, codec choices, cache TTLs, and deletion policy.
  * Rollouts can introduce new image formats gradually by app version/browser support.
  * Operators can invalidate CDN paths for safety/privacy incidents.

*** Explain the data flow
  * Upload -> original stored -> processing event -> variants generated -> metadata updated.
  * Read -> Photo API checks access -> returns signed or tokenized CDN URL -> client downloads from CDN.
  * Delete -> metadata marked deleted -> access denied immediately -> async object/CDN cleanup.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Variant generation
  * Problem: devices need different sizes; generating all variants costs storage.
  * Option 1: Generate on demand.
    * Pros: saves storage for rarely viewed photos.
    * Cons: slow first view and processing spikes.
  * Option 2: Pre-generate common variants.
    * Pros: fast reads and CDN-friendly.
    * Cons: more storage and processing.
  * Option 3: Hybrid.
    * Pros: balance cost and latency.
    * Cons: more orchestration.
  * Recommended: pre-generate common variants; generate rare variants on demand.

*** Access control with CDN
  * Problem: CDN is fast but can serve cached objects after privacy changes.
  * Option 1: Public immutable URLs.
    * Pros: maximum cacheability.
    * Cons: bad for private content.
  * Option 2: Signed URLs/cookies.
    * Pros: better access control.
    * Cons: less cache efficiency and key management complexity.
  * Recommended: public CDN for public media; tokenized/signed access for private media; immediate metadata denial plus CDN purge for sensitive changes.

---

## 18. Design Instagram Auction / Ads Auction System

* Question
  * Design an ads auction system for Instagram.

* Answer

** Scope
  * Include ad request, candidate retrieval, targeting, budget checks, auction ranking, pacing, logging, and feedback events.
  * Exclude full advertiser UI and billing ledger details.

** Functional Requirements
  * Advertisers create campaigns with targeting, bids, budgets, and creatives.
  * When a user opens a surface, system selects eligible ads.
  * Ads are ranked by bid, predicted quality, and relevance.
  * Budgets and frequency caps are enforced.
  * Impressions/clicks/conversions are logged.
  * Unsafe or disallowed ads are filtered.

** Non Functional Requirements
  * Very low ad-decision latency.
  * High availability and high QPS.
  * Accurate enough budget enforcement.
  * Strong auditability for billing and policy.
  * Fresh features for prediction.
  * Revenue/relevance guardrails.

** High level design and diagram (at block level)

```text
Client Feed/Reels Request
  |
  v
Ad Decision API
  |
  +--> User Context Service
  +--> Candidate Retrieval / Targeting Index
  +--> Budget/Pacing Service
  +--> Frequency Cap Service
  +--> Policy Filter
  +--> Prediction Service / Feature Store
  +--> Auction Ranking Service
  |
  v
Selected Ad

Events --> Logging Stream --> Billing/Analytics/Training
Advertiser UI --> Campaign Config Store --> Targeting Index
```

*** Explain the blocks
  * Ad Decision API: orchestrates online ad selection.
  * Candidate Retrieval: finds ads targeting the user/context.
  * Budget/Pacing Service: checks campaign spend and delivery goals.
  * Frequency Cap Service: prevents overexposure.
  * Prediction Service: estimates CTR/CVR/quality.
  * Auction Ranking Service: computes final score.
  * Logging Stream: records impressions, clicks, conversions, and audit events.
  * Campaign Config Store: stores advertiser control-plane data.

*** Explain the control flow
  * Advertiser creates campaign -> policy review -> campaign config published -> targeting index updated.
  * Ads platform controls auction formula, pacing strategy, feature/model versions, and experiment configs.
  * Finance/policy controls audit, billing eligibility, and compliance rules.

*** Explain the data flow
  * Feed/reels requests ad -> decision service gathers user/context -> retrieves eligible campaigns -> filters policy/budget/frequency -> scores/ranks -> returns ad.
  * Impression/click events stream to billing, analytics, pacing, and model training.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Budget enforcement
  * Problem: strict global budget checks add latency and can bottleneck.
  * Option 1: Synchronous global budget service.
    * Pros: accurate.
    * Cons: slower and less available.
  * Option 2: Regional/local budget allocation.
    * Pros: fast and scalable.
    * Cons: possible overspend.
  * Option 3: Hybrid allocation with reconciliation.
    * Pros: practical balance.
    * Cons: accounting complexity.
  * Recommended: allocate budget tokens by region/shard, reconcile asynchronously, and use conservative pacing near budget exhaustion.

*** Auction ranking
  * Problem: maximizing bid alone harms user experience.
  * Option 1: Rank by bid.
    * Pros: simple and revenue-obvious.
    * Cons: low relevance and user harm.
  * Option 2: Rank by expected value plus quality.
    * Pros: balances revenue and relevance.
    * Cons: needs prediction models and calibration.
  * Recommended: score using bid * predicted action rate * quality/relevance adjustments, with safety filters before ranking.

---

## 19. Design Real-Time Analytics For Meta Events

* Question
  * Design a real-time analytics platform for product and operational events.

* Answer

** Scope
  * Include event ingestion, schema validation, durable log, stream processing, aggregation, OLAP storage, dashboards, and alerting.
  * Exclude full offline data lake details except as sink.

** Functional Requirements
  * Services and clients emit events.
  * System validates schema and rejects/isolates bad events.
  * Users can query metrics and dashboards.
  * System supports near-real-time aggregations.
  * Alerts fire on anomalies/thresholds.
  * Raw events are retained for replay.

** Non Functional Requirements
  * Very high ingestion throughput.
  * Low ingestion loss.
  * Near-real-time freshness.
  * Backpressure and replay.
  * Schema evolution support.
  * Multi-tenant isolation.

** High level design and diagram (at block level)

```text
Clients/Services
  |
  v
Event Collectors
  |
  +--> Schema Registry / Validator
  |
  v
Durable Event Log
  |
  +--> Stream Processors --> Real-Time Aggregates Store
  +--> Raw Event Lake
  +--> Anomaly/Alerting Service
  +--> Feature/Experiment Consumers

Query API / Dashboards --> OLAP Store / Aggregates
```

*** Explain the blocks
  * Event Collectors: receive events and apply auth/rate limits.
  * Schema Registry: validates event contracts and versions.
  * Durable Event Log: stores ordered partitions for replay.
  * Stream Processors: compute windows, joins, and aggregates.
  * Aggregates Store/OLAP Store: serves dashboards and queries.
  * Raw Event Lake: long-term storage.
  * Alerting Service: detects anomalies and threshold breaches.

*** Explain the control flow
  * Platform defines event schemas, quotas, retention, partitioning, and tenant access.
  * Teams register schemas before publishing events.
  * Operators manage consumer lag, replay jobs, and degraded-mode policies.

*** Explain the data flow
  * Producer emits event -> collector validates -> durable log append -> stream processors update aggregates -> dashboards query aggregate/OLAP stores.
  * Raw events flow to lake for batch backfills and auditing.
  * Alerts subscribe to processed metrics and notify owners.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Exactly-once analytics
  * Problem: retries and processor failures can double-count events.
  * Option 1: At-most-once processing.
    * Pros: simple.
    * Cons: data loss.
  * Option 2: At-least-once processing.
    * Pros: reliable ingestion.
    * Cons: duplicates unless deduped.
  * Option 3: Exactly-once-like processing with idempotent sinks.
    * Pros: accurate aggregates.
    * Cons: complex and expensive.
  * Recommended: at-least-once log plus idempotent event IDs and sink-side dedupe for critical metrics.

*** Lambda vs streaming-first architecture
  * Problem: real-time and batch systems can diverge.
  * Option 1: Separate batch and streaming pipelines.
    * Pros: mature and accurate batch recomputation.
    * Cons: duplicated logic.
  * Option 2: Streaming-first with replay.
    * Pros: one main computation path.
    * Cons: harder for complex historical joins.
  * Recommended: streaming for freshness, batch/lake for canonical backfills and audits.

---

## 20. Design Privacy / Blocking / Content Visibility Service

* Question
  * Design a privacy, blocking, and content visibility service for Meta.

* Answer

** Scope
  * Include privacy settings, block lists, audience rules, group membership checks, content visibility decisions, cache invalidation, and auditability.
  * Exclude full legal compliance workflows unless asked.

** Functional Requirements
  * Users can set content audience: public, friends, custom, groups, private.
  * Users can block/unblock others.
  * Services can ask if viewer X can see object Y.
  * Visibility updates take effect quickly.
  * Decisions are explainable/auditable.
  * System supports policy changes and experiments.

** Non Functional Requirements
  * Very low latency for read-path checks.
  * Strong correctness for sensitive decisions.
  * High availability.
  * Cache invalidation for privacy changes.
  * Clear policy versioning.
  * Defense in depth: final checks before display.

** High level design and diagram (at block level)

```text
Clients/Product Services
  |
  v
Visibility API
  |
  +--> Policy Engine
  +--> Privacy Settings Store
  +--> Block List Store
  +--> Social Graph Service
  +--> Group Membership Service
  +--> Decision Cache
  +--> Audit Log

Policy Admin --> Versioned Policy Store --> Policy Engine
Privacy Change Events --> Cache Invalidation Stream
```

*** Explain the blocks
  * Visibility API: central interface for can-view decisions.
  * Policy Engine: evaluates rules using policy version and context.
  * Privacy Settings Store: stores object/user audience settings.
  * Block List Store: optimized for block checks.
  * Social Graph/Group Services: provide relationship and membership context.
  * Decision Cache: caches safe, short-lived decisions.
  * Audit Log: records decision inputs/version for debugging/compliance.

*** Explain the control flow
  * Policy teams publish versioned rules with staged rollout.
  * User privacy changes emit invalidation events.
  * Product services integrate via a stable decision API and pass viewer/object/action context.
  * Emergency controls can disable risky caches or force stricter evaluation.

*** Explain the data flow
  * Product wants to render object -> calls Visibility API with viewer/object/action -> API fetches settings, blocks, graph/group context -> policy engine returns allow/deny -> decision logged.
  * User changes privacy/block setting -> setting store updates -> invalidation stream clears affected caches -> subsequent reads use new rules.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Cache correctness
  * Problem: visibility checks are hot, but stale cache can leak private content.
  * Option 1: No caching.
    * Pros: freshest decisions.
    * Cons: too slow and expensive for feed/search.
  * Option 2: Long-lived decision cache.
    * Pros: fast.
    * Cons: dangerous after privacy changes.
  * Option 3: Short TTL plus event invalidation.
    * Pros: strong practical balance.
    * Cons: invalidation complexity.
  * Recommended: short TTL, privacy-change invalidation, and final synchronous checks for sensitive surfaces.

*** Centralized vs embedded policy
  * Problem: many products need visibility checks; duplicating logic causes bugs.
  * Option 1: Embed policy in each service.
    * Pros: local performance and flexibility.
    * Cons: inconsistent behavior and hard policy rollout.
  * Option 2: Central visibility service.
    * Pros: consistency, auditability, easier policy changes.
    * Cons: dependency on central service and latency.
  * Option 3: Shared policy library plus central config.
    * Pros: lower latency with consistent rules.
    * Cons: version skew and rollout complexity.
  * Recommended: central service for authoritative checks; shared library/cache for low-risk prefilters.

---

## 21. Design A Heterogeneous AI Inference Platform

* Question
  * Design a Meta-scale inference platform that serves ranking, recommendation, ads, feed, and GenAI workloads across GPUs, MTIA-style accelerators, and CPU fallback while controlling latency, cost, reliability, and rollout risk.

* Answer

** Scope
  * Include model artifact rollout, request routing, accelerator selection, capacity management, batching, feature fetch, online serving, fallback, observability, and safe deployment.
  * Cover both low-latency ranking/recommendation inference and heavier GenAI inference.
  * Exclude model training internals except for model export, compatibility, and validation gates.

** Functional Requirements
  * Product services can request inference by model name, version, tenant/product, input shape, and latency class.
  * Platform supports multiple hardware pools: GPUs, custom accelerators, CPU fallback, and simulation or canary environments.
  * Model owners can publish artifacts, kernels/operators, runtime constraints, feature dependencies, and rollout policy.
  * Router chooses serving backend by model compatibility, accelerator health, capacity, latency, cost, and policy.
  * Platform supports dynamic batching, priority queues, deadline-aware admission, overload shedding, and graceful fallback.
  * Operators can canary new model versions, runtimes, kernels, and hardware generations with fast rollback.

** Non Functional Requirements
  * Very low p95/p99 latency for ranking and ads paths; bounded tail latency for GenAI.
  * High availability during hardware failures, runtime regressions, bad model pushes, and regional capacity loss.
  * Strong tenant/product isolation so one model or product cannot exhaust shared accelerators.
  * Accurate cost and capacity attribution by product, model, version, hardware pool, and request class.
  * Reproducible validation across heterogeneous hardware despite precision, kernel, and runtime differences.
  * Deep observability for queueing, batching, feature fetch, model runtime, accelerator utilization, memory bandwidth, and fallback rates.

** High level design and diagram (at block level)

```text
Product Services / Feed / Ads / GenAI
  |
  v
Inference Gateway
  |
  +--> Auth, Quota, Deadline, Priority
  +--> Model Routing Policy
  +--> Feature Fetch / Embedding Lookup
  |
  v
Heterogeneous Serving Router
  |
  +--> GPU Serving Pool
  +--> MTIA / Custom Accelerator Pool
  +--> CPU Fallback Pool
  +--> Canary / Shadow Pool
  |
  v
Batcher + Runtime Worker + Accelerator Driver
  |
  v
Prediction / Ranking Scores / Generated Tokens

Control plane:
Model Registry -> Artifact Store -> Compatibility Validator
Capacity Planner -> Placement / Autoscaler -> Hardware Pools
Rollout Controller -> Canary, Shadow, Rollback
Telemetry -> SLOs, Cost, Drift, Hardware Health
```

*** Explain the blocks
  * Inference Gateway receives product requests, validates schema, attaches deadlines, applies quotas, and records product/model attribution.
  * Model Routing Policy maps model families to compatible runtimes, hardware pools, precision modes, fallback order, and rollout constraints.
  * Feature Fetch / Embedding Lookup gathers online features, embedding vectors, and context before model execution; it must be separately budgeted because feature latency can dominate model latency.
  * Heterogeneous Serving Router picks a pool using compatibility, current queue depth, hardware health, memory pressure, SLO class, and cost.
  * Batcher groups compatible requests by model version, input shape, deadline, and hardware target without violating p99 budgets.
  * Runtime Worker loads artifacts, executes kernels/operators through GPU or accelerator drivers, reports fine-grained latency and correctness counters, and returns outputs.
  * Model Registry, Artifact Store, and Compatibility Validator make model/runtime/hardware support explicit before rollout.
  * Capacity Planner and Autoscaler reserve scarce accelerators, rebalance hot models, and scale fallback pools before overload becomes user visible.

*** Core components and low-level design
  * **Model registry and compatibility contract**
    * Durable state includes model ID, version, artifact hash, runtime, supported hardware, precision, max batch size, input shapes, memory footprint, feature dependencies, owner, SLO class, and fallback policy.
    * Compatibility validation runs unit tests, golden-output comparisons, performance smoke tests, operator coverage checks, and precision-drift thresholds before a model is eligible for a hardware pool.
    * Invariant: the router never sends traffic to a model/hardware/runtime tuple that has not passed compatibility validation for the requested version.
  * **Deadline-aware router**
    * Maintains per-pool health, queue length, recent latency histograms, accelerator memory pressure, admission limits, and cost weights.
    * Routing score favors healthy compatible pools with enough slack before deadline; cost optimization is secondary for critical ranking paths.
    * If no primary pool can meet the deadline, the router chooses degraded model, smaller batch, CPU fallback, cached score, or fail-closed depending on product policy.
  * **Batcher and worker isolation**
    * Requests are grouped by model version, input shape, priority, and remaining deadline.
    * Workers use per-model memory reservations so one large GenAI model cannot evict latency-critical ranking models from hot accelerator memory.
    * Backpressure flows from worker queue to router to gateway so overload is shed before queues create unbounded tail latency.
  * **Rollout controller**
    * Supports shadow traffic, canary percentage, product allowlists, hardware-pool allowlists, automatic halt, and rollback to previous artifact hash.
    * Compares latency, error rate, fallback rate, output drift, feature freshness, and accelerator health between baseline and candidate.
    * Rollback changes routing policy first, then drains workers, then unloads artifacts asynchronously.

*** Explain the control flow
  * Model owner exports a model artifact and declares runtime, feature, hardware, precision, and SLO constraints.
  * Registry stores the version and triggers compatibility validation for each target hardware pool.
  * Capacity planner estimates peak QPS, memory footprint, batch efficiency, and hardware demand, then reserves serving slots.
  * Rollout controller starts shadowing, canaries low-risk products, expands traffic by policy, and watches automated guardrails.
  * Operators can freeze rollouts, change fallback policy, lower admission limits, or move traffic away from a degraded hardware pool.

*** Explain the data flow
  * Product request enters gateway with model/version or logical model alias, input features, deadline, priority, and user/product context.
  * Gateway enforces quota and fetches missing online features or embedding inputs.
  * Router resolves logical model alias to a concrete version, selects a compatible pool, and forwards to a deadline-aware batcher.
  * Worker executes inference, returns scores or tokens, and emits telemetry for queue time, batch size, runtime, hardware counters, and output checks.
  * If the primary path fails or misses deadline, fallback returns cached scores, smaller model output, CPU result, or explicit degraded response based on product semantics.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Hardware specialization vs flexible pooled serving
  * Problem: ranking, recommendation, ads, and GenAI models have different compute, memory bandwidth, latency, and batching profiles.
  * Option 1: One generic GPU pool for all models.
    * Pros: simple operations and broad runtime compatibility.
    * Cons: expensive for high-volume inference and vulnerable to noisy-neighbor effects.
  * Option 2: Dedicated custom-accelerator pools per workload family.
    * Pros: better efficiency and predictable performance.
    * Cons: lower flexibility, more validation work, and harder failover when a custom pool is saturated.
  * Option 3: Heterogeneous serving with explicit compatibility and fallback.
    * Pros: balances efficiency and resilience.
    * Cons: requires strong routing, validation, telemetry, and rollout discipline.
  * Recommended: heterogeneous serving with model/hardware compatibility contracts, strict isolation for latency-critical paths, and CPU/GPU fallback for availability.

*** Tail latency vs batch efficiency
  * Problem: batching improves accelerator utilization, but waiting for a larger batch can violate p99 latency.
  * Option 1: Maximize batch size.
    * Pros: strong throughput and cost efficiency.
    * Cons: unacceptable tail latency for feed and ads ranking.
  * Option 2: No batching for critical paths.
    * Pros: simpler latency reasoning.
    * Cons: poor accelerator utilization and higher cost.
  * Option 3: Deadline-aware micro-batching.
    * Pros: captures most efficiency while respecting per-request budgets.
    * Cons: more complex scheduling and instrumentation.
  * Recommended: deadline-aware micro-batching with per-model SLO classes, priority queues, and guardrails that shrink batches during latency spikes.

*** Precision and operator compatibility across hardware
  * Problem: custom accelerators and GPUs may use different kernels, precision formats, and operator support.
  * Option 1: Allow best-effort numerical differences.
    * Pros: fast adoption.
    * Cons: silent ranking drift and hard-to-debug product regressions.
  * Option 2: Require bit-identical output.
    * Pros: strongest correctness gate.
    * Cons: often impractical across different hardware and low-precision formats.
  * Option 3: Golden-output and distributional validation.
    * Pros: practical for ML systems while catching dangerous drift.
    * Cons: requires representative test sets and ongoing monitoring.
  * Recommended: per-model tolerance thresholds, golden datasets, shadow traffic comparison, operator coverage checks, and automatic rollback on drift.

*** Fallback policy for overloaded inference
  * Problem: inference overload can break high-traffic product paths even when the rest of the product is healthy.
  * Option 1: Fail requests quickly.
    * Pros: protects infrastructure.
    * Cons: visible product degradation and revenue impact.
  * Option 2: Queue until capacity is free.
    * Pros: preserves accuracy for completed requests.
    * Cons: creates tail-latency collapse under overload.
  * Option 3: Tiered degradation.
    * Pros: keeps product usable while protecting critical infrastructure.
    * Cons: product teams must define acceptable degraded modes.
  * Recommended: per-product fallback ladder: smaller model, cached score, stale feature snapshot, CPU fallback, or explicit no-result depending on safety and user impact.
