# Google L7 System Design Prep: Cloud and Distributed Systems Question Catalog

Note: Google does not publish an official system design question bank. These are high-probability Google-style prompts based on public interview prep sources and common Google-scale architecture themes. For L7, use these as architecture leadership drills: clarify scope, separate control plane from data plane, name tradeoffs, and show how the system evolves. The question count is intentionally not encoded in this file name or title so future agents can keep this catalog current as prompts are added, removed, merged, or reordered.

Useful public references:

- Google Cloud TPU architecture: https://cloud.google.com/tpu/docs/system-architecture
- Google Cloud TPU7x / Ironwood architecture: https://docs.cloud.google.com/tpu/docs/tpu7x
- Google Cloud TPU Cluster Director overview: https://docs.cloud.google.com/tpu/docs/all-capacity-overview
- Google Cloud TPU collection scheduling for inference workloads: https://docs.cloud.google.com/tpu/docs/collection-scheduling

## 1. Design Google Search

* Question
  * Design a global web search system.

* Answer
  * Scope
    * Focus on web search: crawling, indexing, query serving, ranking integration, freshness, and availability.
    * Exclude building a perfect ranking algorithm. Treat ranking as a pluggable ML/scoring service.

  * Functional Requirements
    * Crawl and refresh web pages.
    * Parse documents, dedupe content, and build searchable indexes.
    * Serve keyword queries with low latency.
    * Support snippets, freshness, language/location awareness, and spam filtering.
    * Provide admin controls for crawl policy, removal requests, and ranking experiments.

  * Non Functional Requirements
    * Very low query latency, especially p95/p99.
    * Massive scale: billions of documents and high query volume.
    * High availability across regions.
    * Freshness for news and frequently changing pages.
    * Abuse resistance, privacy, observability, and cost control.

  * High level design and diagram (at block level)

```text
Control plane:
Crawler policy UI -> Crawl config service -> Policy store
Ranking experiment UI -> Experiment service -> Ranking config store
Removal/admin tools -> Compliance workflow -> Suppression lists

Data plane:
URL frontier -> Fetchers -> Parser -> Dedupe -> Document store
                                      |
                                      v
                              Index builder
                                      |
                                      v
Client -> Query gateway -> Query planner -> Index serving shards
                                      |          |
                                      v          v
                              Ranking service <- Feature store
                                      |
                                      v
                              Results/snippets/cache
```

    * Explain the blocks
      * URL frontier prioritizes URLs by freshness, importance, robots policy, and crawl budget.
      * Fetchers retrieve pages with politeness rules and failure handling.
      * Parser extracts text, links, metadata, canonical URLs, and structured data.
      * Dedupe removes duplicate or near-duplicate content.
      * Document store keeps raw or normalized documents.
      * Index builder creates inverted indexes and forward indexes.
      * Query gateway handles auth, localization, throttling, and request routing.
      * Query planner fans out to index shards and merges candidates.
      * Ranking service scores candidates using ranking features and experiments.
      * Snippet service builds highlighted summaries.

    * Explain the control flow
      * Admins define crawl policies, robots handling, removal rules, ranking experiments, and quality thresholds.
      * Configs are validated, versioned, and pushed to crawler, indexing, and serving systems.
      * Experiment configs are rolled out gradually and monitored using search quality metrics.
      * Compliance removals publish suppression lists that query serving must enforce immediately.

    * Explain the data flow
      * Crawlers discover URLs and fetch pages.
      * Parsed documents are deduped, stored, and transformed into index segments.
      * Index segments are distributed to serving clusters.
      * Query requests fan out to relevant shards, retrieve candidates, rank them, and return results.
      * Query/click logs feed quality evaluation and ranking feature pipelines.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should indexes be partitioned?
      * Problem: Query latency depends heavily on how the inverted index is distributed.
      * Option 1: Partition by term.
        * Pros: Each term lookup is localized; useful for very common terms.
        * Cons: Multi-term queries require cross-partition merging; hot terms can overload shards.
      * Option 2: Partition by document.
        * Pros: Easy horizontal scaling and replication; balanced by document ID.
        * Cons: Every query may fan out broadly, increasing tail latency.
      * Option 3: Hybrid partitioning.
        * Pros: Handles hot terms and common query classes better.
        * Cons: Higher operational and query-planning complexity.
      * Suggested solution: Use document partitioning for baseline scalability with special handling for hot terms, cached top results, and tiered indexes.

    * Topic: How fresh should search results be?
      * Problem: New documents should appear quickly, but constant reindexing is expensive.
      * Option 1: Batch indexing.
        * Pros: Efficient, predictable, easier to optimize.
        * Cons: Poor freshness.
      * Option 2: Real-time incremental indexing.
        * Pros: Better freshness for news and fast-changing pages.
        * Cons: More expensive and harder to merge with large index segments.
      * Option 3: Tiered freshness indexes.
        * Pros: Small real-time index handles fresh content; large batch index handles stable corpus.
        * Cons: Query serving must merge across tiers.
      * Suggested solution: Tiered indexes: real-time index for fresh documents, periodic compaction into main serving indexes.

## 2. Design Google Drive / Cloud File Storage

* Question
  * Design a cloud file storage and sync system like Google Drive.

* Answer
  * Scope
    * Focus on file upload/download, metadata, sharing, sync, versioning, and permissions.
    * Exclude full collaborative editing internals; Google Docs is covered separately.

  * Functional Requirements
    * Upload and download files.
    * Support folders, metadata, file versions, sharing, and ACLs.
    * Sync changes across devices.
    * Support resumable uploads and large files.
    * Search file names and metadata.

  * Non Functional Requirements
    * High durability for file contents.
    * High availability for metadata and downloads.
    * Strong permission correctness.
    * Efficient storage through chunking and dedupe.
    * Global low-latency access and safe conflict handling.

  * High level design and diagram (at block level)

```text
Control plane:
Admin/security UI -> Policy service -> ACL policy store
Sharing UI -> Permission service -> Metadata DB
Quota UI -> Quota service -> Quota ledger

Data plane:
Client -> API gateway -> Metadata service -> Metadata DB
        |                  |
        |                  v
        |             Change log -> Sync service -> Devices
        |
        v
Upload session service -> Chunker -> Blob/chunk store -> Replication
                                      |
                                      v
Download service/CDN <---------------+
```

    * Explain the blocks
      * Metadata service stores folder tree, file IDs, versions, ownership, and ACL references.
      * Upload session service supports resumable upload and chunk tracking.
      * Chunker splits files into chunks for dedupe and retry.
      * Blob/chunk store stores encrypted content with replication.
      * Change log records metadata mutations for sync clients.
      * Sync service pushes or lets clients poll changes.
      * Permission service evaluates sharing and access.

    * Explain the control flow
      * Users and admins set sharing policies, quotas, retention rules, and organization policies.
      * Policies are validated, stored, audited, and enforced by metadata and permission services.
      * Quota and storage policies are applied before accepting uploads.

    * Explain the data flow
      * Client creates an upload session and sends chunks.
      * Chunks are stored durably, then metadata is committed to point to the completed object version.
      * Metadata changes are appended to a change log.
      * Other devices consume the change log and sync the new version.
      * Downloads resolve metadata, check permissions, and retrieve chunks from blob storage or CDN.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How strong should consistency be?
      * Problem: Permissions and metadata must be correct, but global sync can tolerate delay.
      * Option 1: Strong consistency for everything.
        * Pros: Simple mental model; fewer user-visible conflicts.
        * Cons: Higher latency and lower availability across regions.
      * Option 2: Eventual consistency for everything.
        * Pros: Faster and more available.
        * Cons: Dangerous for permissions and sharing revocation.
      * Option 3: Mixed consistency.
        * Pros: Strong ACL and metadata commit path; eventual device sync.
        * Cons: More complex semantics.
      * Suggested solution: Strong consistency for permissions and metadata commits; eventual consistency for device sync and search indexing.

    * Topic: How should large uploads be handled?
      * Problem: Large files fail often over unreliable networks.
      * Option 1: Single upload request.
        * Pros: Simple implementation.
        * Cons: Poor reliability for large files.
      * Option 2: Chunked resumable upload.
        * Pros: Retries only failed chunks; enables dedupe and parallelism.
        * Cons: Requires session state and chunk manifest management.
      * Option 3: Client-side dedupe before upload.
        * Pros: Saves bandwidth.
        * Cons: Privacy and complexity concerns.
      * Suggested solution: Chunked resumable uploads with server-side dedupe and encrypted storage.

## 3. Design YouTube / Video Streaming

* Question
  * Design a video upload, processing, and streaming platform like YouTube.

* Answer
  * Scope
    * Focus on upload, transcoding, storage, metadata, streaming playback, and CDN delivery.
    * Exclude recommendation ranking details.

  * Functional Requirements
    * Upload videos and metadata.
    * Transcode videos into multiple resolutions and bitrates.
    * Generate thumbnails and manifests.
    * Stream videos globally with adaptive bitrate.
    * Track playback, likes, comments, and moderation state.

  * Non Functional Requirements
    * High availability for playback.
    * Low startup latency and smooth playback.
    * Massive storage and bandwidth efficiency.
    * Durable content storage.
    * Abuse, copyright, and policy enforcement.

  * High level design and diagram (at block level)

```text
Control plane:
Creator/admin UI -> Metadata service -> Video metadata DB
Policy UI -> Moderation/copyright config -> Policy store
Encoding config -> Transcode scheduler config

Data plane:
Uploader -> Upload API -> Raw video object store
                         |
                         v
                 Transcode queue -> Workers -> Encoded segment store
                                      |              |
                                      v              v
                              Thumbnail/manifest   CDN/origin
                                                     |
Viewer -> Playback API -> Metadata/manifest service -+
                         |
                         v
                  Analytics stream
```

    * Explain the blocks
      * Upload API receives large video files using resumable upload.
      * Raw object store keeps original uploads.
      * Transcode queue decouples upload from processing.
      * Workers produce multiple codecs, resolutions, bitrates, thumbnails, and manifests.
      * Encoded segment store holds streamable chunks.
      * CDN caches hot segments close to viewers.
      * Playback API returns metadata, authorization, and manifest URLs.
      * Analytics stream captures playback QoE and engagement.

    * Explain the control flow
      * Creators set metadata, visibility, monetization, and age restrictions.
      * Moderation/copyright systems set policy state that gates playback.
      * Encoding policy controls supported formats and worker priorities.
      * Rollouts of codecs or player behavior are controlled through experiments.

    * Explain the data flow
      * Creator uploads raw video.
      * Transcoding jobs produce adaptive bitrate segments and manifests.
      * Viewer requests playback metadata and receives manifest.
      * Player fetches segments from CDN, falling back to origin on misses.
      * Playback events flow into analytics and quality monitoring.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should adaptive bitrate streaming work?
      * Problem: Users have different devices and changing network conditions.
      * Option 1: Single encoded stream.
        * Pros: Simple pipeline and storage.
        * Cons: Poor experience across network speeds.
      * Option 2: Multiple bitrate encodings.
        * Pros: Player can adapt to bandwidth; better QoE.
        * Cons: Higher storage and transcoding cost.
      * Option 3: Just-in-time transcoding.
        * Pros: Saves storage for rare formats.
        * Cons: High latency and compute spikes.
      * Suggested solution: Pre-transcode popular standard formats; use selective just-in-time processing for rare cases.

    * Topic: How should CDN caching be handled?
      * Problem: Video traffic is bandwidth-heavy and popularity is skewed.
      * Option 1: Cache all videos everywhere.
        * Pros: Great latency.
        * Cons: Unbounded cost and waste.
      * Option 2: Cache only on demand.
        * Pros: Cost efficient.
        * Cons: Cold-start latency for first viewers.
      * Option 3: Predictive pre-warming.
        * Pros: Great for trending videos.
        * Cons: Prediction errors waste capacity.
      * Suggested solution: Demand-driven CDN with pre-warming for trending and subscribed-audience videos.

## 4. Design Google Maps

* Question
  * Design a global maps, routing, and location search system.

* Answer
  * Scope
    * Focus on map tile serving, geocoding, search, routing, and traffic.
    * Exclude building a perfect traffic prediction ML model.

  * Functional Requirements
    * Serve map tiles by viewport and zoom level.
    * Search for places and addresses.
    * Calculate routes for driving, walking, transit, and biking.
    * Show real-time or near-real-time traffic.
    * Support updates to roads, closures, and points of interest.

  * Non Functional Requirements
    * Low-latency map interactions.
    * High availability globally.
    * Fresh dynamic data for traffic and closures.
    * Correctness for routing and safety-sensitive directions.
    * Efficient geospatial indexing and caching.

  * High level design and diagram (at block level)

```text
Control plane:
Map editor/admin -> Map data validation -> Base map store
Traffic ops UI -> Incident/closure service -> Dynamic map store
Experiment UI -> Routing config service -> Routing config store

Data plane:
Map data pipeline -> Tile generator -> Tile store/CDN
Live signals -> Traffic ingestion -> Traffic aggregator -> Dynamic traffic store

Client -> Maps API gateway -> Tile service -> CDN/tile store
                         |-> Geocoding/search service -> Geo index
                         |-> Routing service -> Road graph + traffic store
```

    * Explain the blocks
      * Base map store contains roads, boundaries, places, and geometry.
      * Tile generator precomputes visual tiles by zoom and region.
      * CDN serves tiles at low latency.
      * Geo index supports spatial lookup by lat/lng, bounding box, and text.
      * Routing service computes paths over a road graph.
      * Traffic aggregator processes live signals into road-segment speeds.
      * Dynamic map store keeps closures, incidents, and traffic.

    * Explain the control flow
      * Map updates are validated, versioned, and published to tile generation and routing graph pipelines.
      * Routing policies and experiments are configured centrally.
      * Incident operations can publish urgent closures with fast propagation.

    * Explain the data flow
      * Client requests tiles for viewport and zoom; tile service returns cached tiles.
      * Search queries hit geocoding/place indexes.
      * Route requests query road graph plus traffic data.
      * Live traffic signals flow into aggregators and update segment speeds.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Precompute tiles or render dynamically?
      * Problem: Tile latency must be low, but map data changes frequently.
      * Option 1: Precompute all tiles.
        * Pros: Very fast serving and CDN-friendly.
        * Cons: Expensive for all zoom levels and slow to refresh globally.
      * Option 2: Render all tiles dynamically.
        * Pros: Fresh and flexible.
        * Cons: High compute cost and worse tail latency.
      * Option 3: Hybrid.
        * Pros: Precompute base map; overlay dynamic traffic and incidents.
        * Cons: More client/server composition complexity.
      * Suggested solution: Precompute base tiles; serve dynamic overlays separately.

    * Topic: How should routing handle traffic freshness?
      * Problem: Traffic changes quickly and stale routes can be bad.
      * Option 1: Static shortest path.
        * Pros: Simple and stable.
        * Cons: Ignores real-world congestion.
      * Option 2: Fully live traffic routing.
        * Pros: Fresher ETAs and better routes.
        * Cons: More volatile and compute-heavy.
      * Option 3: Live traffic with smoothing and cache.
        * Pros: Balances freshness, stability, and cost.
        * Cons: Can miss sudden incidents.
      * Suggested solution: Use live segment speeds with smoothing, incident overrides, and cached route candidates.

## 5. Design Gmail / Email System

* Question
  * Design a large-scale email system like Gmail.

* Answer
  * Scope
    * Focus on sending, receiving, mailbox storage, search, spam filtering, and notifications.
    * Exclude full UI and advanced ML model internals.

  * Functional Requirements
    * Send and receive email.
    * Store mailboxes with labels, threads, attachments, and read state.
    * Search inbox content.
    * Filter spam and malware.
    * Support notifications and sync across devices.

  * Non Functional Requirements
    * High durability and availability.
    * Correct mailbox state and permission isolation.
    * Low-latency inbox reads and search.
    * Large mailbox scalability.
    * Strong abuse protection.

  * High level design and diagram (at block level)

```text
Control plane:
User settings UI -> Settings service -> Settings DB
Admin/security UI -> Policy service -> Policy store
Spam config -> Classifier config -> Model/config store

Data plane:
Inbound SMTP -> Mail gateway -> Spam/malware pipeline -> Delivery queue
                                                        |
                                                        v
Client -> Gmail API -> Mailbox service -> Mailbox store
                       |                 |
                       |                 v
                       |           Search indexer -> Search index
                       v
                  Attachment store
                       |
Outbound SMTP <- Send service <- Compose/API
```

    * Explain the blocks
      * Mail gateway receives inbound SMTP and applies protocol-level validation.
      * Spam/malware pipeline classifies and quarantines risky messages.
      * Delivery queue decouples inbound processing from mailbox writes.
      * Mailbox service manages conversations, labels, read state, and user operations.
      * Mailbox store persists messages and per-user state.
      * Search indexer builds per-user or partitioned indexes.
      * Attachment store stores large immutable blobs.
      * Send service handles outbound routing and retry.

    * Explain the control flow
      * Users configure filters, forwarding, labels, and notification settings.
      * Admins configure compliance, retention, security, and spam policies.
      * Policy/config changes are versioned and distributed to mail gateways and mailbox services.

    * Explain the data flow
      * Inbound mail is received, classified, queued, and written to mailbox storage.
      * Indexer asynchronously updates search index.
      * Clients fetch inbox views from mailbox service and search from index service.
      * Outbound mail is accepted, stored in sent mail, then sent through outbound SMTP.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should mailbox storage be partitioned?
      * Problem: Users have uneven mailbox sizes and access patterns.
      * Option 1: Partition by user.
        * Pros: Simple isolation and efficient inbox reads.
        * Cons: Hot users and very large mailboxes are harder to split.
      * Option 2: Partition by message.
        * Pros: Better global balance.
        * Cons: User mailbox reads require scatter/gather.
      * Option 3: User partition plus internal sharding for large users.
        * Pros: Common case is simple; large users can scale.
        * Cons: More complex metadata routing.
      * Suggested solution: Partition by user with split support for very large mailboxes.

    * Topic: How fresh should search be?
      * Problem: Users expect new mail to be searchable quickly.
      * Option 1: Synchronous indexing before delivery.
        * Pros: Search is immediately consistent.
        * Cons: Delivery latency depends on indexing.
      * Option 2: Async indexing.
        * Pros: Fast delivery and better reliability.
        * Cons: Search may lag.
      * Option 3: Hybrid recent-message overlay.
        * Pros: Fast delivery and fresh search for recent items.
        * Cons: More complex query merging.
      * Suggested solution: Async indexing plus a recent-message overlay for newly delivered mail.

## 6. Design Google Photos / Media Storage

* Question
  * Design a media storage and retrieval system like Google Photos.

* Answer
  * Scope
    * Focus on photo/video upload, storage, thumbnails, albums, sharing, search metadata, and ML-derived labels.

  * Functional Requirements
    * Upload photos and videos.
    * Generate thumbnails and previews.
    * Organize by albums, time, location, and people/object labels.
    * Share media with users or links.
    * Search by metadata and derived features.

  * Non Functional Requirements
    * High durability and privacy.
    * Efficient storage and CDN delivery.
    * Low-latency browsing.
    * Eventual consistency for derived metadata.
    * Cost-aware media processing.

  * High level design and diagram (at block level)

```text
Control plane:
Privacy/sharing UI -> Permission service -> ACL store
ML config -> Feature extraction pipeline config
Retention/quota UI -> Quota service -> Quota ledger

Data plane:
Client -> Upload API -> Media object store
                      |
                      v
              Processing queue -> Thumbnail workers -> Thumbnail store/CDN
                                -> Feature workers -> Feature/index store

Client -> Browse/search API -> Metadata service -> Metadata DB
                            -> Search service -> Search/feature index
```

    * Explain the blocks
      * Upload API handles resumable upload and metadata capture.
      * Media object store keeps original encrypted media.
      * Metadata DB stores owner, timestamps, EXIF, albums, and sharing state.
      * Processing queue triggers thumbnail, preview, and ML feature jobs.
      * Feature/index store supports search and clustering.
      * Permission service enforces sharing.

    * Explain the control flow
      * Users configure sharing, album membership, privacy, backup quality, and retention.
      * ML pipelines are controlled by versioned configs and privacy rules.
      * Quota policies determine whether uploads are accepted or compressed.

    * Explain the data flow
      * Client uploads media and metadata.
      * Original is stored durably; metadata commit makes it visible.
      * Async workers create thumbnails and derived indexes.
      * Browse requests read metadata and thumbnails; search requests query indexes.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should dedupe work?
      * Problem: Users upload duplicates across devices, but privacy matters.
      * Option 1: Exact hash dedupe.
        * Pros: Simple and safe for identical files.
        * Cons: Misses resized or edited duplicates.
      * Option 2: Perceptual hash dedupe.
        * Pros: Catches visually similar images.
        * Cons: False positives require careful handling.
      * Option 3: No dedupe.
        * Pros: Simple privacy model.
        * Cons: Higher storage cost.
      * Suggested solution: Exact hash dedupe within user scope; perceptual dedupe as a user-facing suggestion, not destructive merge.

    * Topic: When should ML features become visible?
      * Problem: ML extraction is expensive and can lag behind uploads.
      * Option 1: Synchronous feature extraction.
        * Pros: Search is immediately complete.
        * Cons: Upload latency and compute spikes.
      * Option 2: Async extraction.
        * Pros: Fast upload and scalable processing.
        * Cons: Search results are eventually consistent.
      * Option 3: Prioritized async extraction.
        * Pros: Recent and viewed media become searchable faster.
        * Cons: More scheduling complexity.
      * Suggested solution: Async prioritized extraction with clear product tolerance for temporary search lag.

## 7. Design Google Calendar / Scheduling System

* Question
  * Design a calendar and scheduling system.

* Answer
  * Scope
    * Focus on calendar events, invitations, availability, recurring events, reminders, and sharing.

  * Functional Requirements
    * Create, update, delete, and view events.
    * Invite attendees and track RSVP state.
    * Support recurring events and exceptions.
    * Compute availability and room/resource booking.
    * Send reminders and change notifications.

  * Non Functional Requirements
    * Strong correctness for event state and permissions.
    * Low-latency calendar reads.
    * Robust time-zone handling.
    * Conflict handling for concurrent updates.
    * High availability for reminders and sync.

  * High level design and diagram (at block level)

```text
Control plane:
User settings UI -> Calendar settings service -> Settings DB
Org admin UI -> Resource/policy service -> Policy/resource DB
Reminder config -> Notification config store

Data plane:
Client -> Calendar API -> Event service -> Event store
                       |        |
                       |        v
                       |  Invitation workflow -> Notification queue
                       |
                       v
               Availability service -> Availability index
                       |
                       v
                 Sync/change log -> Devices
```

    * Explain the blocks
      * Event service owns event CRUD, recurrence rules, exceptions, and attendee state.
      * Event store persists canonical events.
      * Availability service builds free/busy views.
      * Invitation workflow manages RSVP and notifications.
      * Resource service manages rooms and shared assets.
      * Sync/change log lets clients update local views.

    * Explain the control flow
      * Users configure time zone, working hours, reminders, and sharing.
      * Admins configure rooms, booking rules, external sharing, and retention.
      * Policies are enforced by event and availability services.

    * Explain the data flow
      * Client writes event to Calendar API.
      * Event service validates permissions and recurrence rules, then commits canonical event.
      * Invitations and reminders are queued asynchronously.
      * Availability index and sync logs are updated.
      * Clients read calendar views from event store or materialized views.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should recurring events be stored?
      * Problem: Infinite or long-running recurrences cannot be fully materialized naively.
      * Option 1: Store every occurrence.
        * Pros: Fast reads and simple queries.
        * Cons: Large storage and hard edits to future events.
      * Option 2: Store recurrence rule only.
        * Pros: Compact and clean.
        * Cons: Expensive range expansion at read time.
      * Option 3: Store rule plus bounded materialization.
        * Pros: Fast common reads and manageable storage.
        * Cons: Requires background expansion.
      * Suggested solution: Store canonical rule, exceptions, and materialized windows for common date ranges.

    * Topic: How should concurrent edits be resolved?
      * Problem: Multiple users can edit the same event or attendee state.
      * Option 1: Last-write-wins.
        * Pros: Simple.
        * Cons: Can lose important changes.
      * Option 2: Strict locking.
        * Pros: Prevents conflicts.
        * Cons: Poor UX and availability.
      * Option 3: Field-level versioning.
        * Pros: Allows RSVP and metadata changes to merge safely.
        * Cons: More complex conflict semantics.
      * Suggested solution: Optimistic concurrency with event versioning and field-aware merges for attendee responses.

## 8. Design a Distributed Cache / Key-Value Store

* Question
  * Design a distributed cache or key-value store.

* Answer
  * Scope
    * Focus on get/put/delete, TTL, sharding, replication, failure handling, and cluster operations.

  * Functional Requirements
    * Store and retrieve key-value pairs.
    * Support TTL and eviction.
    * Scale horizontally.
    * Replicate data for availability.
    * Handle node addition/removal.

  * Non Functional Requirements
    * Very low latency.
    * High throughput.
    * High availability under node failures.
    * Predictable memory usage.
    * Tunable consistency.

  * High level design and diagram (at block level)

```text
Control plane:
Admin/auto-scaler -> Cluster manager -> Membership/config store
Policy UI -> Cache policy service -> TTL/eviction config

Data plane:
Client library -> Request router -> Cache node shard
                                  |-> Replica node
                                  |-> Replica node

Cache nodes -> Gossip/heartbeat -> Cluster manager
Cache nodes -> Metrics -> Monitoring/alerting
```

    * Explain the blocks
      * Client library performs routing, retries, and optional local caching.
      * Request router maps keys to shards using consistent hashing.
      * Cache nodes store data in memory with TTL and eviction.
      * Replicas improve availability.
      * Cluster manager tracks membership and rebalancing.
      * Metrics pipeline detects hot keys and node pressure.

    * Explain the control flow
      * Operators define capacity, replication factor, eviction policy, and consistency mode.
      * Cluster manager publishes membership and shard maps.
      * Clients and routers refresh shard maps.
      * Rebalancing moves key ranges when nodes change.

    * Explain the data flow
      * Client sends get/put/delete.
      * Router maps key to primary or replica.
      * Writes go to primary and then replicas depending on consistency mode.
      * Reads may go to primary, nearest replica, or quorum.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should keys be distributed?
      * Problem: Rebalancing should be efficient when nodes join or leave.
      * Option 1: Modulo hashing.
        * Pros: Simple.
        * Cons: Massive reshuffle when node count changes.
      * Option 2: Consistent hashing.
        * Pros: Limited movement on membership changes.
        * Cons: Needs virtual nodes for balance.
      * Option 3: Range partitioning.
        * Pros: Range scans possible.
        * Cons: Hot ranges and manual splitting.
      * Suggested solution: Consistent hashing with virtual nodes and load-aware placement.

    * Topic: How consistent should reads be?
      * Problem: Cache reads must be fast, but replicas can be stale.
      * Option 1: Primary-only reads.
        * Pros: Stronger consistency.
        * Cons: Higher latency and lower availability.
      * Option 2: Replica reads.
        * Pros: Lower latency and more throughput.
        * Cons: Stale reads possible.
      * Option 3: Quorum reads/writes.
        * Pros: Tunable consistency.
        * Cons: Higher latency and operational complexity.
      * Suggested solution: For cache use cases, default to replica reads with TTL and invalidation; use quorum only for stronger store semantics.

## 9. Design a Notification System

* Question
  * Design a large-scale notification platform.

* Answer
  * Scope
    * Support email, push, SMS, and in-app notifications for many product teams.

  * Functional Requirements
    * Accept notification requests.
    * Apply user preferences, templates, policy, and rate limits.
    * Deliver through multiple channels.
    * Retry failures and dedupe sends.
    * Track delivery status and analytics.

  * Non Functional Requirements
    * High availability and durable ingestion.
    * Low latency for urgent notifications.
    * At-least-once processing with idempotency.
    * Compliance with opt-out and quiet-hour rules.
    * Multi-tenant isolation and observability.

  * High level design and diagram (at block level)

```text
Control plane:
Product team UI -> Template/config service -> Versioned config store
Preference UI -> Preference service -> Preference store
Policy/admin UI -> Compliance/rate-limit config -> Policy store

Data plane:
Product service -> Ingestion API -> Idempotency/validation -> Durable queue
                                                       |
                                                       v
                                              Orchestrator
                                               | | | |
                             Preferences/policy/rate-limit/template
                                                       |
                                                       v
                         Channel queues -> Email/SMS/Push/In-app workers
                                                       |
                                                       v
                                         Provider callbacks/status stream
```

    * Explain the blocks
      * Ingestion API authenticates product teams and validates requests.
      * Durable queue protects against downstream failures.
      * Orchestrator evaluates preferences, policy, dedupe, templates, and channel choice.
      * Channel queues isolate provider failures and scale independently.
      * Workers send to external providers or in-app storage.
      * Status stream records delivery, bounce, open, and failure events.

    * Explain the control flow
      * Product teams define templates, campaigns, priorities, and allowed channels.
      * Users configure preferences and opt-outs.
      * Compliance policies and rate limits are versioned and distributed to runtime services.
      * Bad configs can be rolled back without redeploying data-plane workers.

    * Explain the data flow
      * Product service submits notification request with idempotency key.
      * Request is persisted to durable queue.
      * Orchestrator evaluates rules and emits channel-specific jobs.
      * Workers send through providers and record status.
      * Status events feed analytics, monitoring, and retry logic.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Can we guarantee exactly-once delivery?
      * Problem: External email/SMS/push providers cannot provide true end-to-end exactly-once semantics.
      * Option 1: At-most-once.
        * Pros: No duplicates.
        * Cons: Drops messages during failures.
      * Option 2: At-least-once with idempotency.
        * Pros: Reliable and practical.
        * Cons: Duplicate user-visible sends are still possible in edge cases.
      * Option 3: Claimed exactly-once.
        * Pros: Attractive product contract.
        * Cons: Usually false across third-party providers.
      * Suggested solution: At-least-once internally, idempotency keys, provider request IDs, dedupe tables, and honest product semantics.

    * Topic: Centralized vs regional rate limiting.
      * Problem: Notifications must obey global quotas without making every send depend on a global service.
      * Option 1: Centralized limiter.
        * Pros: Accurate global limits.
        * Cons: Latency and availability bottleneck.
      * Option 2: Regional local limiters.
        * Pros: Fast and resilient.
        * Cons: Temporary global overshoot.
      * Option 3: Hybrid quotas.
        * Pros: Strong enough global control with local speed.
        * Cons: More accounting complexity.
      * Suggested solution: Regional quotas with periodic reconciliation; use stricter synchronous checks for compliance-critical limits.

## 10. Design a Metrics / Logging Platform

* Question
  * Design a distributed metrics and logging system.

* Answer
  * Scope
    * Support service logs, metrics, dashboards, alerts, retention, and query.

  * Functional Requirements
    * Ingest logs and metrics from many services.
    * Store raw logs and aggregated metrics.
    * Query recent and historical data.
    * Trigger alerts.
    * Support retention, sampling, and tenant isolation.

  * Non Functional Requirements
    * Very high write throughput.
    * Queryable recent data within seconds.
    * Cost-effective long-term retention.
    * Backpressure and data-loss controls.
    * High-cardinality protection.

  * High level design and diagram (at block level)

```text
Control plane:
Tenant/admin UI -> Retention/quota service -> Policy store
Alert UI -> Alert rule service -> Alert config store
Schema UI -> Metric/log schema registry

Data plane:
Agents -> Local buffer -> Collectors -> Ingestion stream
                                      |
                                      v
                          Stream processors/aggregators
                            |                    |
                            v                    v
                    Hot metrics/log store    Cold object store
                            |
Client -> Query API -> Query planner -> Hot/cold stores
Alert engine -> Notification system
```

    * Explain the blocks
      * Agents collect logs and metrics from services.
      * Local buffers protect against network failures.
      * Collectors authenticate, batch, and normalize data.
      * Ingestion stream absorbs spikes.
      * Stream processors aggregate metrics and index logs.
      * Hot store serves recent queries; cold store handles retention.
      * Alert engine evaluates rules.

    * Explain the control flow
      * Teams define schemas, retention, quotas, alert rules, and sampling policies.
      * Configs are validated and pushed to agents, collectors, and processors.
      * Quota and cardinality policies protect shared infrastructure.

    * Explain the data flow
      * Agents batch data and send to collectors.
      * Collectors publish to ingestion stream.
      * Processors index logs, aggregate metrics, and write hot/cold stores.
      * Queries fan out to relevant stores.
      * Alert engine reads metric windows and sends notifications.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How to handle high-cardinality metrics?
      * Problem: Too many label combinations can overload storage and queries.
      * Option 1: Accept all labels.
        * Pros: Maximum debugging flexibility.
        * Cons: Unbounded cost and degraded reliability.
      * Option 2: Drop high-cardinality labels.
        * Pros: Protects platform.
        * Cons: Loses debugging detail.
      * Option 3: Quotas plus sampling and exemplars.
        * Pros: Balances protection and utility.
        * Cons: More policy complexity.
      * Suggested solution: Schema registry, label cardinality budgets, sampling, and per-tenant quotas.

    * Topic: Push vs pull collection.
      * Problem: The platform needs reliable data collection from dynamic services.
      * Option 1: Push agents.
        * Pros: Works well across dynamic environments and logs.
        * Cons: More risk of overload during spikes.
      * Option 2: Pull scraping.
        * Pros: Central control and easier rate management.
        * Cons: Harder across NAT, short-lived jobs, and logs.
      * Option 3: Hybrid.
        * Pros: Use pull for metrics, push for logs/events.
        * Cons: More operational surface.
      * Suggested solution: Hybrid: pull where possible for metrics, push/buffered agents for logs and ephemeral workloads.

## 11. Design a Distributed File System Like GFS

* Question
  * Design a distributed file system for large-scale data processing.

* Answer
  * Scope
    * Focus on large files, chunking, replication, metadata, client reads/writes, and failure recovery.

  * Functional Requirements
    * Create, read, append, and delete files.
    * Split files into chunks.
    * Replicate chunks across machines.
    * Detect and recover failed chunk replicas.
    * Support high-throughput batch workloads.

  * Non Functional Requirements
    * High throughput over low per-operation latency.
    * High durability and availability.
    * Fault tolerance for commodity machine failures.
    * Scalable metadata management.
    * Efficient large sequential reads and appends.

  * High level design and diagram (at block level)

```text
Control plane:
Admin UI -> Namespace/quota service -> Master metadata
Placement policy -> Chunk placement manager -> Master metadata

Data plane:
Client -> Master -> Chunk locations/lease info
Client -> Primary chunkserver -> Secondary chunkservers
                         |
                         v
                    Chunk replicas on disk

Chunkservers -> Heartbeats/checksums -> Master
Master -> Re-replication commands -> Chunkservers
```

    * Explain the blocks
      * Master stores namespace, file-to-chunk mapping, chunk locations, leases, and placement.
      * Chunkservers store fixed-size chunks and replicas.
      * Client asks master for metadata, then transfers data directly to chunkservers.
      * Lease mechanism chooses a primary replica for mutation ordering.
      * Heartbeats report health and chunk inventory.

    * Explain the control flow
      * Operators configure replication factor, placement policy, quotas, and namespace rules.
      * Master assigns chunks, leases, and re-replication tasks.
      * Failed chunkservers trigger re-replication and metadata updates.

    * Explain the data flow
      * Client gets chunk locations from master.
      * For reads, client reads directly from a nearby replica.
      * For writes/appends, client sends data to replicas and primary orders the mutation.
      * Chunkservers verify checksums and report status.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Is the master a bottleneck?
      * Problem: A single metadata service can limit scalability and availability.
      * Option 1: Single master.
        * Pros: Simple metadata consistency.
        * Cons: Bottleneck and failover risk.
      * Option 2: Single active master with replicas.
        * Pros: Simpler consistency with improved availability.
        * Cons: Still bounded by one active master.
      * Option 3: Sharded metadata masters.
        * Pros: Better scale.
        * Cons: Cross-directory operations become complex.
      * Suggested solution: Start with active master plus replicated logs; shard metadata if namespace load requires it.

    * Topic: How should append semantics work?
      * Problem: Many clients may append concurrently to large files.
      * Option 1: Strict POSIX-like writes.
        * Pros: Familiar semantics.
        * Cons: Expensive and unnecessary for batch workloads.
      * Option 2: Relaxed record append.
        * Pros: High throughput and simple concurrency.
        * Cons: May produce duplicates/padding that readers must handle.
      * Option 3: External transaction layer.
        * Pros: Stronger semantics.
        * Cons: More complexity and lower throughput.
      * Suggested solution: Relaxed append semantics optimized for data processing workloads.

## 12. Design Google Docs / Collaborative Editing

* Question
  * Design a real-time collaborative document editor.

* Answer
  * Scope
    * Focus on concurrent text editing, comments, presence, offline edits, history, and permissions.

  * Functional Requirements
    * Multiple users edit a document concurrently.
    * Show presence and cursors.
    * Support comments and suggestions.
    * Maintain version history.
    * Support offline edits and sync.

  * Non Functional Requirements
    * Low-latency collaboration.
    * Strong document convergence.
    * Durable operation history.
    * Permission correctness.
    * Graceful handling of disconnects.

  * High level design and diagram (at block level)

```text
Control plane:
Sharing UI -> Permission service -> ACL store
Admin UI -> Retention/audit policy -> Policy store
Experiment UI -> Editor config service -> Config store

Data plane:
Client -> WebSocket gateway -> Collaboration session service
                                  |
                                  v
                         OT/CRDT engine -> Operation log
                                  |              |
                                  v              v
                         Snapshot store     History service
                                  |
                                  v
                         Pub/sub -> Connected clients
```

    * Explain the blocks
      * WebSocket gateway maintains low-latency client connections.
      * Collaboration session service groups users by document.
      * OT/CRDT engine resolves concurrent operations.
      * Operation log stores ordered edits.
      * Snapshot store speeds document load.
      * History service reconstructs previous versions.
      * Permission service gates access.

    * Explain the control flow
      * Owners configure sharing, comment access, suggestion mode, and retention.
      * Policies are enforced before joining sessions or reading snapshots.
      * Editor configs and experiments are rolled out to clients and session services.

    * Explain the data flow
      * Client sends edit operation to session service.
      * Engine transforms or merges operation, persists it, and broadcasts resulting operation.
      * Other clients apply operation locally.
      * Periodic snapshots compact operation history.
      * Offline clients replay pending operations after reconnect.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: OT vs CRDT?
      * Problem: Concurrent edits must converge without losing user intent.
      * Option 1: Operational transformation.
        * Pros: Efficient for centralized collaboration; mature for text editors.
        * Cons: Complex transform logic.
      * Option 2: CRDT.
        * Pros: Strong offline and peer-style convergence.
        * Cons: More metadata overhead and document compaction needs.
      * Option 3: Lock-based editing.
        * Pros: Simple correctness.
        * Cons: Poor collaborative UX.
      * Suggested solution: OT for centralized low-latency editing; consider CRDT if offline-first is dominant.

    * Topic: How should history be stored?
      * Problem: Full operation logs grow indefinitely.
      * Option 1: Store every operation forever.
        * Pros: Perfect audit and replay.
        * Cons: Expensive and slow to load.
      * Option 2: Store snapshots only.
        * Pros: Fast and compact.
        * Cons: Limited history granularity.
      * Option 3: Snapshots plus compacted operation ranges.
        * Pros: Fast load with useful history.
        * Cons: Requires compaction logic.
      * Suggested solution: Periodic snapshots plus retained operation logs based on policy.

## 13. Design a Distributed Job Scheduler

* Question
  * Design a distributed job scheduler for batch and recurring work.

* Answer
  * Scope
    * Focus on job submission, scheduling, dependencies, retries, leases, worker management, and observability.

  * Functional Requirements
    * Submit one-time, recurring, and dependency-based jobs.
    * Assign jobs to workers.
    * Retry failed jobs.
    * Track status and logs.
    * Enforce priority, quotas, and resource constraints.

  * Non Functional Requirements
    * High availability and durable job state.
    * Fair scheduling across tenants.
    * At-least-once execution with idempotency guidance.
    * Scalable worker pools.
    * Clear operational visibility.

  * High level design and diagram (at block level)

```text
Control plane:
Job API/UI -> Job metadata service -> Job DB
Policy UI -> Quota/priority service -> Policy store
Cluster manager -> Resource inventory store

Data plane:
Scheduler -> Ready queues -> Dispatcher -> Workers
    ^                           |         |
    |                           v         v
Job DB <- Heartbeats/leases <- Worker agent -> Logs/status stream
    |
    v
Retry/DLQ manager
```

    * Explain the blocks
      * Job metadata service stores job definitions, schedules, dependencies, and state.
      * Scheduler evaluates readiness, priority, and resource fit.
      * Ready queues buffer runnable jobs by priority/tenant.
      * Dispatcher leases jobs to workers.
      * Worker agent executes jobs and reports heartbeats.
      * Retry/DLQ manager handles failures and poison jobs.

    * Explain the control flow
      * Users define jobs, schedules, dependencies, resource requirements, and retry policies.
      * Admins configure tenant quotas and priorities.
      * Scheduler loads policy and cluster state to make placement decisions.

    * Explain the data flow
      * Job is submitted and stored durably.
      * Scheduler marks it ready when dependencies and time constraints are satisfied.
      * Dispatcher leases job to worker.
      * Worker heartbeats and reports completion or failure.
      * Failed jobs are retried or moved to DLQ.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How to avoid duplicate execution?
      * Problem: Worker failures and retries can cause the same job to run more than once.
      * Option 1: At-most-once execution.
        * Pros: No duplicate side effects.
        * Cons: Jobs can be lost.
      * Option 2: At-least-once with leases.
        * Pros: Reliable completion.
        * Cons: Requires idempotent jobs.
      * Option 3: Exactly-once with transactions.
        * Pros: Strong semantics in narrow cases.
        * Cons: Hard across arbitrary user code.
      * Suggested solution: At-least-once execution with leases, fencing tokens, idempotency keys, and job owner guidance.

    * Topic: Centralized or distributed scheduler?
      * Problem: Scheduler must scale while preserving fairness.
      * Option 1: Single scheduler.
        * Pros: Simple global decisions.
        * Cons: Bottleneck and failover risk.
      * Option 2: Sharded schedulers.
        * Pros: Scales by tenant or queue.
        * Cons: Harder global fairness.
      * Option 3: Hierarchical scheduling.
        * Pros: Global quota with local dispatch.
        * Cons: More complex implementation.
      * Suggested solution: Hierarchical scheduler: global quota/priority control with sharded local dispatchers.

## 14. Design a Global Software Rollout System

* Question
  * Design a system to roll out software safely to a global fleet of machines.

* Answer
  * Scope
    * Focus on package/version distribution, target selection, canaries, health checks, rollback, and audit.

  * Functional Requirements
    * Register software versions.
    * Select target machines or services.
    * Roll out gradually by stage.
    * Monitor health and pause/rollback.
    * Audit who changed what and when.

  * Non Functional Requirements
    * High safety and blast-radius control.
    * High availability of control plane.
    * Secure package distribution.
    * Fast rollback.
    * Scalable to millions of machines.

  * High level design and diagram (at block level)

```text
Control plane:
Release UI/CLI -> Release service -> Release DB
Policy UI -> Rollout policy service -> Policy store
Artifact registry -> Signing/verifier -> Artifact store

Data plane:
Rollout controller -> Target selection -> Stage queues
                                      |
                                      v
Machine agents -> Pull artifact -> Install -> Health report
       ^                                  |
       |                                  v
       +---------- Telemetry/health <-----+
```

    * Explain the blocks
      * Release service records versions, owners, approvals, and rollout plans.
      * Artifact registry stores signed packages.
      * Rollout controller advances stages based on policy and health.
      * Target selection picks machines by region, service, version, and risk cohort.
      * Machine agents pull artifacts and report status.
      * Telemetry validates error rates, latency, and crash loops.

    * Explain the control flow
      * Owner creates release with artifact, target, and rollout policy.
      * Policy service validates approvals, blackout windows, and risk limits.
      * Controller starts canary and advances only when health gates pass.
      * Rollback command or automatic trigger pins fleet to prior version.

    * Explain the data flow
      * Agents poll for assigned desired state.
      * Agents download signed artifacts, verify signatures, install, and restart safely.
      * Health signals stream back to controller.
      * Controller updates stage status and sends next assignments.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Push vs pull rollout agents?
      * Problem: The system must scale and work through network partitions.
      * Option 1: Control plane pushes to machines.
        * Pros: Immediate commands.
        * Cons: Hard at massive scale and across firewalls.
      * Option 2: Agents pull desired state.
        * Pros: Scales better and is resilient to disconnected machines.
        * Cons: Slightly slower propagation.
      * Option 3: Hybrid push notification plus pull.
        * Pros: Fast signal with scalable state retrieval.
        * Cons: More moving parts.
      * Suggested solution: Pull-based desired state with optional push hints.

    * Topic: How should rollback be handled?
      * Problem: Bad releases must be stopped quickly without causing more damage.
      * Option 1: Manual rollback only.
        * Pros: Human judgment.
        * Cons: Slow during incidents.
      * Option 2: Fully automatic rollback.
        * Pros: Fast blast-radius reduction.
        * Cons: Risk of false positives and rollback loops.
      * Option 3: Automatic pause plus guarded rollback.
        * Pros: Balances speed and control.
        * Cons: Requires well-designed health gates.
      * Suggested solution: Automatic pause on threshold breach; automatic rollback for severe signals; human approval for ambiguous cases.

## 15. Design a 100 PB Cross-Region Data Migration

* Question
  * Design a system to move 100 PB of data across regions safely.

* Answer
  * Scope
    * Focus on inventory, transfer planning, throttling, checksums, validation, consistency, and cutover.

  * Functional Requirements
    * Inventory source data.
    * Partition and schedule transfer work.
    * Copy data reliably with resume support.
    * Validate integrity.
    * Support cutover with minimal downtime.

  * Non Functional Requirements
    * Data correctness and durability.
    * Controlled network impact.
    * Observable progress.
    * Failure recovery.
    * Security and access control.

  * High level design and diagram (at block level)

```text
Control plane:
Migration UI/CLI -> Migration coordinator -> Migration DB
Policy UI -> Throttle/priority policy -> Policy store
Cutover plan -> Validation/cutover controller

Data plane:
Source inventory scanner -> Manifest store -> Partition planner
                                           |
                                           v
Transfer queue -> Copy workers -> Destination store
      |               |
      v               v
Progress DB      Checksum validator -> Mismatch repair queue
```

    * Explain the blocks
      * Inventory scanner lists source objects, sizes, versions, and checksums.
      * Manifest store freezes or versions migration scope.
      * Partition planner splits work into resumable chunks.
      * Copy workers transfer data with throttling and retries.
      * Checksum validator compares source and destination integrity.
      * Cutover controller manages dual-write or read-switch phases.

    * Explain the control flow
      * Operators define source, destination, allowed bandwidth, priority, and cutover strategy.
      * Coordinator creates migration plan and monitors progress.
      * Policies throttle workers to avoid harming production traffic.
      * Cutover happens only after validation reaches agreed thresholds.

    * Explain the data flow
      * Scanner builds manifest.
      * Planner enqueues copy tasks.
      * Workers copy data and report progress.
      * Validator checks checksums and enqueues repairs.
      * Final delta sync captures changes since initial snapshot before cutover.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Offline vs online migration?
      * Problem: Large migrations take a long time, and source data may keep changing.
      * Option 1: Offline migration.
        * Pros: Simple consistency.
        * Cons: Long downtime is usually unacceptable.
      * Option 2: Online copy plus final freeze.
        * Pros: Less downtime.
        * Cons: Needs delta tracking and cutover logic.
      * Option 3: Dual-write migration.
        * Pros: Smooth cutover and rollback.
        * Cons: Complex consistency and operational risk.
      * Suggested solution: Online bulk copy, change log/delta sync, validation, then short controlled cutover.

    * Topic: How to validate 100 PB?
      * Problem: Full byte-by-byte validation is expensive.
      * Option 1: Full read validation.
        * Pros: Strong confidence.
        * Cons: Very costly and slow.
      * Option 2: Checksum validation.
        * Pros: Efficient and strong if checksums are reliable.
        * Cons: Requires trusted source checksums.
      * Option 3: Sampling only.
        * Pros: Cheap.
        * Cons: Insufficient for critical migration.
      * Suggested solution: Per-object or per-chunk checksums, targeted full reads for mismatches, and sampled audits.

## 16. Design a URL Shortener

* Question
  * Design a URL shortening service.

* Answer
  * Scope
    * Support creating short URLs, redirecting, expiration, custom aliases, analytics, and abuse controls.

  * Functional Requirements
    * Create short links.
    * Redirect short links to long URLs.
    * Support custom aliases and expiration.
    * Track click analytics.
    * Block malicious URLs.

  * Non Functional Requirements
    * Very low redirect latency.
    * High read availability.
    * Durable mappings.
    * Abuse resistance.
    * Scalable hot-link handling.

  * High level design and diagram (at block level)

```text
Control plane:
Admin/abuse UI -> Abuse policy service -> Blocklist/policy store
User UI/API -> Link management service -> Link metadata DB
Analytics UI -> Analytics query service -> Analytics store

Data plane:
Create API -> ID generator -> Mapping service -> Mapping DB

Browser -> Redirect edge/CDN -> Redirect service -> Mapping cache -> Mapping DB
                                             |
                                             v
                                     Click event stream -> Analytics pipeline
```

    * Explain the blocks
      * Create API validates long URL and creates mapping.
      * ID generator produces compact unique IDs.
      * Mapping DB stores short code, target URL, owner, expiration, and policy state.
      * Redirect edge/CDN handles high-volume reads.
      * Mapping cache reduces DB reads.
      * Analytics pipeline records click events asynchronously.
      * Abuse service blocks unsafe links.

    * Explain the control flow
      * Users create and manage links.
      * Admins and automated systems update blocklists and abuse policies.
      * Analytics retention and privacy rules are configured centrally.

    * Explain the data flow
      * Create request generates short code and stores mapping.
      * Redirect request resolves mapping from cache or DB.
      * Redirect service checks expiration and abuse state, then returns HTTP redirect.
      * Click event is published asynchronously for analytics.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How should short IDs be generated?
      * Problem: IDs must be unique, compact, and hard enough to abuse.
      * Option 1: Auto-increment ID encoded in base62.
        * Pros: Simple and compact.
        * Cons: Predictable and needs centralized allocation.
      * Option 2: Random ID.
        * Pros: Harder to enumerate.
        * Cons: Collision handling required.
      * Option 3: Preallocated ID ranges.
        * Pros: Scales creation across nodes.
        * Cons: Some coordination and unused ranges.
      * Suggested solution: Preallocated numeric ranges encoded in base62; randomize or use longer IDs for public abuse resistance.

    * Topic: Synchronous or async analytics?
      * Problem: Redirect latency is user-facing and must stay low.
      * Option 1: Write analytics synchronously.
        * Pros: Fewer lost events.
        * Cons: Slower redirects and dependency risk.
      * Option 2: Async event stream.
        * Pros: Keeps redirect fast.
        * Cons: Some events may be delayed or dropped.
      * Option 3: Edge aggregation.
        * Pros: Reduces event volume.
        * Cons: Less granular data.
      * Suggested solution: Async click stream with local buffering and best-effort analytics semantics.

## 17. Design Search Autocomplete

* Question
  * Design a search autocomplete service.

* Answer
  * Scope
    * Suggest search queries as users type, using popularity, freshness, location, language, and personalization.

  * Functional Requirements
    * Return top suggestions for a prefix.
    * Support typo tolerance and localization.
    * Incorporate trending queries.
    * Filter unsafe or policy-blocked suggestions.
    * Support experiments and ranking changes.

  * Non Functional Requirements
    * Very low latency per keystroke.
    * High QPS and global availability.
    * Fresh enough for trending topics.
    * Compact in-memory serving structures.
    * Privacy-aware personalization.

  * High level design and diagram (at block level)

```text
Control plane:
Experiment UI -> Ranking/config service -> Config store
Policy UI -> Suggestion filter service -> Blocklist store
Locale config -> Locale/language rules store

Data plane:
Query logs -> Aggregation pipeline -> Suggestion builder -> FST/trie store
Trending stream -> Real-time top-k builder -> Trending overlay

Client -> Suggest API -> Suggestion serving nodes -> FST/trie + overlay
                              |
                              v
                     Rank/filter/personalize
```

    * Explain the blocks
      * Aggregation pipeline computes frequent query prefixes.
      * Suggestion builder creates compact trie/FST structures.
      * Trending overlay captures fresh spikes.
      * Serving nodes keep structures in memory.
      * Rank/filter/personalize layer applies locale, policy, and user context.
      * Experiment config tunes ranking.

    * Explain the control flow
      * Search quality teams configure ranking weights and experiments.
      * Policy teams configure blocked terms and safety filters.
      * Locale configs define language-specific tokenization and normalization.

    * Explain the data flow
      * Query logs are aggregated offline into prefix-to-suggestion maps.
      * Real-time trending pipeline generates overlay suggestions.
      * Client sends prefix on keystroke.
      * Serving node retrieves candidates, merges offline and trending results, filters, ranks, and returns suggestions.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Trie vs FST vs database lookup?
      * Problem: Autocomplete requires extremely fast prefix lookup.
      * Option 1: Trie.
        * Pros: Natural prefix lookup.
        * Cons: Can be memory-heavy.
      * Option 2: Finite state transducer.
        * Pros: Compact and fast.
        * Cons: More complex to build and update.
      * Option 3: Database prefix queries.
        * Pros: Simple to build.
        * Cons: Too slow and costly at keystroke QPS.
      * Suggested solution: Precomputed FST/trie in memory with periodic rebuilds.

    * Topic: How to handle freshness?
      * Problem: Offline suggestions may miss breaking news.
      * Option 1: Offline daily rebuild.
        * Pros: Stable and cheap.
        * Cons: Poor freshness.
      * Option 2: Fully real-time suggestions.
        * Pros: Fresh.
        * Cons: Noisy and expensive.
      * Option 3: Offline base plus real-time overlay.
        * Pros: Balanced freshness and stability.
        * Cons: Requires merge/ranking logic.
      * Suggested solution: Offline base with real-time trending overlay and safety filters.

## 18. Design a Web Crawler

* Question
  * Design a large-scale web crawler.

* Answer
  * Scope
    * Focus on URL discovery, scheduling, fetching, politeness, parsing, dedupe, and content handoff to indexing.

  * Functional Requirements
    * Discover URLs.
    * Fetch pages according to robots.txt and politeness.
    * Parse links and metadata.
    * Deduplicate URLs and content.
    * Refresh important pages.

  * Non Functional Requirements
    * Massive scale and high throughput.
    * Respect site owners and legal constraints.
    * Fault tolerance and retry.
    * Freshness for important pages.
    * Efficient use of network and storage.

  * High level design and diagram (at block level)

```text
Control plane:
Crawl policy UI -> Crawl policy service -> Policy store
Robots/compliance config -> Compliance store
Priority config -> URL scoring service config

Data plane:
Seed URLs -> URL frontier -> Scheduler -> Fetchers -> Parser
                ^                         |          |
                |                         v          v
         Link extractor <------------ Content store  Indexing pipeline
                |
                v
        URL canonicalizer/dedupe
```

    * Explain the blocks
      * URL frontier stores URLs prioritized by importance and recrawl time.
      * Scheduler enforces host politeness and fetch rate limits.
      * Fetchers retrieve pages and robots.txt.
      * Parser extracts links, content, metadata, and canonical URLs.
      * Dedupe removes repeated URLs and near-duplicate pages.
      * Content store hands documents to indexing.

    * Explain the control flow
      * Operators configure crawl budgets, blocked domains, freshness targets, and politeness policy.
      * Policies are pushed to schedulers and fetchers.
      * Scoring service adjusts frontier priorities based on page importance and change rate.

    * Explain the data flow
      * Seeds enter frontier.
      * Scheduler selects allowed URLs and assigns fetchers.
      * Fetchers retrieve content.
      * Parser extracts new links and document content.
      * New URLs are canonicalized, deduped, scored, and reinserted into frontier.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How to enforce politeness?
      * Problem: Crawlers can overload external websites.
      * Option 1: Global fixed rate.
        * Pros: Simple.
        * Cons: Too conservative for large sites and too aggressive for small sites.
      * Option 2: Per-host rate limits.
        * Pros: Better site protection.
        * Cons: Requires host-level scheduling state.
      * Option 3: Adaptive rate limits.
        * Pros: Reacts to errors and latency.
        * Cons: More complex and can be gamed.
      * Suggested solution: Per-host politeness with adaptive backoff and robots.txt compliance.

    * Topic: How to prioritize recrawls?
      * Problem: The web is too large to refresh everything equally.
      * Option 1: Fixed recrawl interval.
        * Pros: Simple.
        * Cons: Wastes resources on stable pages.
      * Option 2: Importance-based recrawl.
        * Pros: Focuses on valuable pages.
        * Cons: May miss changes on less popular sites.
      * Option 3: Change-rate prediction.
        * Pros: Better freshness/cost balance.
        * Cons: Needs modeling and feedback loops.
      * Suggested solution: Combine importance score, historical change rate, and external freshness signals.

## 19. Design a Distributed Lock / Leader Election Service

* Question
  * Design a distributed lock and leader election service.

* Answer
  * Scope
    * Support locks, leases, sessions, heartbeats, leader election, fencing tokens, and failure handling.

  * Functional Requirements
    * Acquire and release locks.
    * Grant time-bound leases.
    * Detect client failure.
    * Elect a single leader for a resource.
    * Provide fencing tokens to protect downstream systems.

  * Non Functional Requirements
    * Strong consistency over availability.
    * Low but not ultra-low latency.
    * Correctness during partitions.
    * Auditable lock ownership.
    * Operational simplicity for critical systems.

  * High level design and diagram (at block level)

```text
Control plane:
Admin UI -> Namespace/policy service -> Policy store
Client registration -> Auth/service identity -> Identity store

Data plane:
Client -> Lock API -> Consensus group leader -> Replicated log
                          |
                          v
                   Lock/session table
                          |
Client <- Lease + fencing token

Clients -> Heartbeats -> Session manager
```

    * Explain the blocks
      * Lock API exposes acquire, release, renew, and watch.
      * Consensus group replicates lock state using a majority protocol.
      * Lock/session table stores owner, lease expiry, and fencing token.
      * Session manager tracks heartbeats and expires leases.
      * Fencing tokens monotonically increase on lock acquisition.

    * Explain the control flow
      * Admins configure namespaces, max lease duration, auth rules, and quotas.
      * Clients register identities and are authorized for lock namespaces.
      * Policy limits prevent unbounded lock hold times.

    * Explain the data flow
      * Client requests lock.
      * Consensus leader appends acquisition to replicated log.
      * Once committed, service returns lease and fencing token.
      * Client renews lease through heartbeats.
      * On expiry or release, another client can acquire a higher fencing token.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: Why are fencing tokens needed?
      * Problem: A client may think it still owns a lock after a pause or partition.
      * Option 1: Lock without fencing.
        * Pros: Simpler API.
        * Cons: Stale owner can corrupt downstream state.
      * Option 2: Fencing tokens.
        * Pros: Downstream systems can reject stale writes.
        * Cons: Downstream systems must enforce token order.
      * Option 3: Infinite locks.
        * Pros: Avoids lease expiry complexity.
        * Cons: Dead clients block progress.
      * Suggested solution: Leases plus monotonically increasing fencing tokens.

    * Topic: Should the service remain available during partitions?
      * Problem: Lock correctness fails if two sides grant the same lock.
      * Option 1: CP consensus service.
        * Pros: Prevents split-brain ownership.
        * Cons: Minority partition cannot acquire locks.
      * Option 2: AP lock service.
        * Pros: More available.
        * Cons: Unsafe for true locks.
      * Option 3: Client-side best-effort lock.
        * Pros: Simple.
        * Cons: Only suitable for low-risk coordination.
      * Suggested solution: Use a CP consensus service; prefer correctness over availability.

## 20. Design an IoT / Telemetry Ingestion System

* Question
  * Design a large-scale telemetry ingestion system for millions of devices.

* Answer
  * Scope
    * Support device authentication, ingestion, buffering, stream processing, time-series storage, alerting, and cold retention.

  * Functional Requirements
    * Authenticate devices.
    * Ingest telemetry events.
    * Handle offline devices and retries.
    * Store recent and historical time-series data.
    * Trigger alerts and support dashboards.

  * Non Functional Requirements
    * High write throughput.
    * Backpressure and load shedding.
    * Per-device ordering where needed.
    * Regional availability.
    * Cost-effective retention.

  * High level design and diagram (at block level)

```text
Control plane:
Device admin UI -> Device registry -> Device identity store
Policy UI -> Ingestion/quota policy -> Policy store
Alert UI -> Alert rule service -> Alert config store

Data plane:
Devices -> Edge gateway -> Auth/rate-limit -> Ingestion stream
                                         |
                                         v
                         Stream processors/validators
                            |              |
                            v              v
                    Hot time-series DB   Cold object store
                            |
Client -> Query API -> Query planner -> Hot/cold stores
Alert engine -> Notification system
```

    * Explain the blocks
      * Device registry stores identity, keys, tenant, and configuration.
      * Edge gateway terminates device protocols and batches events.
      * Auth/rate-limit protects the ingestion path.
      * Ingestion stream absorbs bursts and partitions by tenant/device.
      * Stream processors validate, enrich, aggregate, and route telemetry.
      * Hot time-series DB serves dashboards.
      * Cold object store keeps long-term history.
      * Alert engine evaluates rules over recent windows.

    * Explain the control flow
      * Operators register devices, rotate credentials, define quotas, and configure alert rules.
      * Policy changes are pushed to gateways and processors.
      * Tenants manage schemas and retention policies.

    * Explain the data flow
      * Device sends telemetry to nearest gateway.
      * Gateway authenticates, rate limits, and publishes to stream.
      * Processors validate and write to hot and cold stores.
      * Query API retrieves recent data from hot store and historical data from cold store.
      * Alert engine evaluates windows and sends notifications.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: How to preserve ordering?
      * Problem: Some telemetry requires per-device order, but global ordering is impossible at scale.
      * Option 1: Global ordering.
        * Pros: Simple downstream semantics.
        * Cons: Does not scale and adds latency.
      * Option 2: Per-device ordering.
        * Pros: Scales and matches most device use cases.
        * Cons: Cross-device correlation needs event-time logic.
      * Option 3: No ordering guarantee.
        * Pros: Highest throughput.
        * Cons: Harder downstream processing.
      * Suggested solution: Partition by device ID for per-device ordering; use event timestamps and watermarks for cross-device analytics.

    * Topic: How to handle offline devices and bursts?
      * Problem: Devices may buffer locally and reconnect simultaneously.
      * Option 1: Reject excess traffic.
        * Pros: Protects backend.
        * Cons: Data loss and poor device experience.
      * Option 2: Unlimited ingestion.
        * Pros: Captures all data.
        * Cons: Can overload platform.
      * Option 3: Quotas, backpressure, and priority tiers.
        * Pros: Protects system while preserving important data.
        * Cons: Requires device protocol support.
      * Suggested solution: Device-side buffering, gateway backpressure, per-tenant quotas, and priority-based load shedding.

## 21. Design A TPU / AI Accelerator Cluster Control Plane

* Question
  * Design a control plane for allocating, placing, health-managing, and operating large TPU or AI accelerator clusters for training and inference tenants.

* Answer
  * Scope
    * Focus on reservations, topology-aware placement, slice allocation, health visibility, maintenance, inference availability, workload admission, and operator control.
    * Treat low-level accelerator firmware, compiler internals, and model code as external dependencies.

  * Functional Requirements
    * Let tenants reserve accelerator capacity by duration, topology, accelerator type, and workload class.
    * Allocate single-slice, multi-slice, and inference replica collections.
    * Expose physical topology, health, utilization, maintenance, and fault state to trusted operators.
    * Place tightly coupled jobs near required interconnect domains.
    * Support preemption, lower-priority backfill, planned maintenance, repair, and replacement workflows.
    * Integrate with GKE or a scheduler-facing API for job launch.

  * Non Functional Requirements
    * High placement correctness and predictable capacity semantics.
    * Low scheduling tail latency for interactive inference admission, but batch training can tolerate queueing.
    * Strong tenant isolation across projects, reservations, credentials, and network policy.
    * High goodput for scarce accelerator fleets.
    * Clear observability, auditability, failure recovery, and cost attribution.

  * High level design and diagram (at block level)

```text
Control plane:
Tenant/API -> Reservation service -> Reservation DB
          -> Quota/admission -> Capacity planner -> Placement solver
                                           |              |
Operator UI -> Topology/health service ----+              v
Maintenance UI -> Maintenance coordinator -> Cluster state store
                                                            |
                                                            v
Data plane:
Scheduler/GKE -> Allocation API -> Slice/collection allocator -> TPU/GPU hosts
                                   |              |
                                   v              v
                            Health agents    Metrics/goodput stream
                                   |
                                   v
                         Repair/replacement workflow
```

    * Explain the blocks
      * Reservation service owns capacity contracts, tenant entitlements, duration, accelerator generation, and topology constraints.
      * Quota/admission rejects requests that exceed tenant or fleet budgets before expensive placement work.
      * Capacity planner forecasts demand, reserved headroom, backfill opportunities, and capacity fragmentation.
      * Placement solver maps jobs onto contiguous topology blocks, cubes, pods, racks, or failure domains.
      * Topology/health service stores physical layout, host state, interconnect health, utilization, and maintenance eligibility.
      * Slice/collection allocator creates concrete accelerator slices or inference collections and binds them to scheduler objects.
      * Health agents publish host, accelerator, interconnect, thermal, and goodput signals.
      * Maintenance coordinator drains, fences, repairs, and returns capacity in waves.

    * Core components and low-level design
      * Reservation API
        * `CreateReservation(tenant, accelerator_type, min_topology, duration, workload_class, isolation_policy)` returns a pending reservation with an idempotency key.
        * `CommitReservation(reservation_id, start_time, capacity_mode)` transitions capacity into reserved state after quota, billing, and placement prechecks.
        * Durable state includes reservation id, tenant, accelerator generation, topology requirements, capacity mode, start/end time, admitted workload classes, and billing labels.
        * Invariant: committed capacity cannot be double-booked across tenants or maintenance windows.
      * Placement solver
        * Maintains an interval-indexed view of free capacity by topology domain, health class, and reservation owner.
        * Scores candidates by topology fit, fragmentation cost, health risk, data locality, maintenance risk, and tenant priority.
        * Uses optimistic reservation versions: solve on a snapshot, then commit with compare-and-swap against the cluster state store.
        * Invariant: an allocation contains only healthy or explicitly tolerated degraded hosts from one compatible accelerator generation and isolation domain.
      * Slice and collection allocator
        * For training, allocates topology shapes that match distributed-collective requirements and exposes hostfiles, mesh coordinates, service accounts, and network policy.
        * For inference, groups replicas into availability collections so maintenance and faults do not drain every replica at once.
        * Tracks allocation lifecycle: `Queued -> Placed -> Provisioning -> Running -> Draining -> Released`.
        * Invariant: released capacity must pass cleanup, credential revocation, and health reconciliation before it returns to allocatable inventory.
      * Topology/health service
        * Stores cluster, block, cube, host, chip, interconnect, and maintenance state with versioned updates from agents and repair systems.
        * Separates hard faults from degraded interconnect paths; degraded capacity may be eligible for lower-priority backfill but not latency-critical training.
        * Publishes topology-change events to the placement solver and schedulers.
      * Maintenance coordinator
        * Plans waves by reservation owner, job checkpoint state, inference collection availability, and fault urgency.
        * Fences hosts before repair and requires post-repair validation before reintroducing capacity.
        * Supports all-capacity style operation where tenants see more topology and health detail but accept more operational responsibility.

    * Explain the control flow
      * Tenant requests a reservation or allocation through API, GKE, or an internal scheduler.
      * Quota/admission checks tenant limits, accelerator generation, duration, region, and workload class.
      * Capacity planner decides whether to admit now, queue, offer a smaller topology, or suggest a future reservation.
      * Placement solver selects a topology-compatible slice and commits it atomically.
      * Allocator provisions host access, network policy, service identities, and scheduler objects.
      * Maintenance coordinator later drains or repairs capacity using checkpoint, replica, and priority information.

    * Explain the data flow
      * Health agents stream host, accelerator, interconnect, thermal, and utilization data into the topology/health service.
      * Metrics and goodput streams feed autoscaling, fragmentation reporting, and tenant dashboards.
      * Scheduler launches workloads against allocated hosts; workload status flows back to allocation state.
      * Repair events update cluster state, trigger placement invalidation, and notify affected tenants.

  * Deep dive topics and questions -> Explain the problem and suggest solutions
    * Topic: topology-aware placement versus fleet utilization
      * Problem: Large training jobs need dense, low-latency accelerator topology, but strict contiguity can strand fragments.
      * Option 1: First-fit allocation.
        * Pros: Simple and fast.
        * Cons: Quickly fragments scarce large topologies and hurts future large jobs.
      * Option 2: Pure best-fit by topology size.
        * Pros: Preserves larger blocks better.
        * Cons: Can overpack risky areas and ignore tenant priority or maintenance windows.
      * Option 3: Multi-objective placement.
        * Pros: Balances topology fit, fragmentation, health, locality, and priority.
        * Cons: Solver complexity and harder debugging.
      * Suggested solution: Use multi-objective placement with explicit fragmentation scoring, then expose solver reasons in allocation traces for operability.

    * Topic: managed capacity versus all-capacity operation
      * Problem: Some tenants want maximum usable accelerator capacity and topology control; others want the platform to absorb faults.
      * Option 1: Managed capacity with platform holdbacks.
        * Pros: Easier recovery and stronger service abstraction.
        * Cons: Lower peak utilization for sophisticated tenants.
      * Option 2: All-capacity reservations.
        * Pros: Tenants can use more capacity and make topology-aware choices.
        * Cons: Tenants must reason about failures, maintenance, and lower-level health.
      * Suggested solution: Offer both modes, but make the contract explicit: managed mode optimizes predictability; all-capacity mode optimizes control and utilization with more operator burden.

    * Topic: inference availability collections versus raw slices
      * Problem: Inference workloads care about continuous serving, while training jobs mostly care about contiguous accelerator topology and checkpoint recovery.
      * Option 1: Allocate raw slices for all workloads.
        * Pros: One allocator path.
        * Cons: Maintenance or faults can drain too many inference replicas at once.
      * Option 2: Availability collections for inference.
        * Pros: Groups replicas so enough remain available during faults and maintenance.
        * Cons: Adds scheduling constraints and may reduce packing efficiency.
      * Suggested solution: Use raw topology slices for tightly coupled training and availability collections for inference serving replicas.

    * Topic: failure and maintenance semantics
      * Problem: Accelerator failures are expensive because they can waste hours of training or reduce inference capacity abruptly.
      * Option 1: Immediate repair on fault.
        * Pros: Restores fleet health quickly.
        * Cons: Can kill valuable jobs unnecessarily.
      * Option 2: Tenant-controlled maintenance windows.
        * Pros: Reduces workload disruption.
        * Cons: Faults can linger and degrade fleet quality.
      * Option 3: Severity-based repair policy.
        * Pros: Balances business impact and hardware health.
        * Cons: Requires good health classification and escalation.
      * Suggested solution: Severity-based repair with checkpoint-aware drains, inference collection safeguards, and hard fencing for unsafe faults.

    * Recommended L7 stance
      * Separate reservation, placement, topology health, and maintenance concerns. The hard part is not creating VMs; it is preserving scarce topology, making capacity contracts honest, and keeping large training and inference workloads useful under faults, maintenance, and tenant contention.
