# Apple L7 System Design Prep

These are Apple-style and publicly reported or representative prompts, not an Apple question bank. Apple team loops vary widely, so use this as practice for the patterns Apple tends to value: device-cloud interaction, privacy, offline behavior, high reliability, polished user experience, hardware/software boundaries, and careful operational rollout.

Public prep sources consulted: Educative Apple system design guide, Exponent Apple system design questions, InterviewQuery Apple software engineer guide, InterviewKickstart Apple system design guide, CodingInterview Apple guide, and public LeetCode discussion about Apple data center design.

## Apple Elastic Disk Staff/Principal Focus

This section is a role-specific focus pack inferred from public Apple storage infrastructure postings and public storage-system references. It is not an Apple question bank. For a Staff or Principal loop on a cloud-based Elastic Disk team, optimize for deep reasoning about correctness under concurrency and failure, not just a pleasant box diagram.

Public role signal: Apple's May 1, 2026 Staff Software Engineer, ASE Storage Infrastructure posting for Elastic Disk emphasizes replication and metadata systems, durability, replica reconstruction, continuous scrubbing, replication-metadata checksums, point-in-time backup and snapshot, keeping replication off the critical latency path, cross-org storage foundations, hands-on systems languages, consensus protocols, and production ownership with measurable availability and durability SLOs.

Use this prep stack:

- **System design spine**: practice questions 21-26 first, then review [AWS EBS / Distributed Block Storage](./aws_l7_system_design_prep.md#11-design-ebs-distributed-block-storage), [distributed file systems](./google_l7_system_design_prep.md#11-design-a-distributed-file-system-like-gfs), and [object storage](./aws_l7_system_design_prep.md#1-design-amazon-s3-object-storage).
- **Coding spine**: prioritize [concurrency](../coding/cpp/concurrency/questions.md), [distributed systems algorithms](../coding/cpp/distributed_systems_algorithms/questions.md), [parallel algorithms](../coding/cpp/parallel_algorithms/questions.md), [graphs](../coding/cpp/graphs/questions.md), and [advanced data structures](../coding/cpp/advanced_data_structures/questions.md).
- **Principal-level story**: prepare two narratives where you set multi-year technical direction, reduced reliability risk, drove a migration with compatibility constraints, and influenced storage, compute, control-plane, and SRE partners without direct authority.
- **Interview posture**: separate control plane from data plane early; state the exact failure model; define durability, availability, and latency SLOs; name the invariants; then discuss how you prove them with tests, simulation, model checking, scrubbing, and incident learning.

Useful public references:

- Apple role: [Staff Software Engineer, ASE Storage Infrastructure](https://jobs.apple.com/en-gb/details/200661014/staff-software-engineer-ase-storage-infrastructure)
- Apple cloud infrastructure listings: [Cloud and Infrastructure jobs](https://jobs.apple.com/en-us/search?location=united-states-USA&team=cloud-and-infrastructure-SFTWR-CLD)
- Apple storage paper: [ACOS: Apple's Geo-Distributed Object Store at Exabyte Scale](https://www.usenix.org/system/files/fast26-baron-updated.pdf)
- Apple open source: [FoundationDB](https://opensource.apple.com/projects/foundationdb)
- FoundationDB docs: [Features](https://apple.github.io/foundationdb/features.html)
- Comparable public service docs: [Amazon EBS snapshots](https://docs.aws.amazon.com/ebs/latest/userguide/ebs-snapshots.html)

## 1. Design iCloud Drive

* Question
  Design iCloud Drive for syncing files across iPhone, iPad, Mac, and web clients.

* Answer
  **Scope**
  Support file/folder CRUD, sync across devices, offline edits, conflict handling, sharing, search metadata, and recovery/version history. Exclude full collaborative editing unless asked.

  **Functional Requirements**
  Upload/download files, sync metadata, detect changes, resume interrupted uploads, share files/folders, maintain versions, notify other devices, and support restore/delete.

  **Non Functional Requirements**
  High durability, low perceived latency, offline-first behavior, privacy/security, multi-region availability, efficient bandwidth/battery usage, and graceful conflict handling.

  **High level design and diagram (at block level)**
  ```text
  Device Sync Agent -> Sync API -> Metadata Service -> Metadata DB
         |                |              |
         |                v              v
         +--------> Chunk Upload API -> Object Store
                          |
                          v
                  Version/Conflict Service
                          |
                          v
                    APNs Device Wakeup
  ```

  *** Explain the blocks
  Device Sync Agent watches local filesystem changes and maintains a local sync cursor. Sync API authenticates users and routes requests. Metadata Service owns file tree, versions, permissions, and sync cursors. Chunk Upload API handles large file uploads with resumable chunks. Object Store stores encrypted file chunks. Version/Conflict Service detects concurrent edits. APNs wakes other devices to pull updates.

  *** Explain the control flow
  User/device registration, quota policy, sharing permissions, encryption policy, and sync configuration are set through control-plane APIs. Metadata schema migrations and object lifecycle policies are versioned and rolled out separately from the hot sync path.

  *** Explain the data flow
  A device modifies a file, chunks it, uploads missing chunks, commits metadata with an idempotency token, and receives a new sync version. Other devices receive push notifications or poll using their cursor, fetch metadata deltas, download missing chunks, and apply changes locally.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Conflict resolution for offline edits
  Problem: two devices may edit the same file while offline.
  Options:
  - Last-writer-wins: simple and cheap; can lose user work.
  - Version vectors plus conflict copies: preserves work and is understandable; exposes conflicts to users.
  - File-type-aware merge: best UX for text/documents; complex and not universal.
  Recommendation: use version vectors and conflict copies by default, with type-specific merge for known formats.

  *** Metadata consistency
  Options:
  - Strong consistency for metadata: simple file tree semantics; higher latency/cost globally.
  - Eventual consistency: scalable and available; confusing if folder state briefly differs.
  Recommendation: strong consistency per user namespace or partition; eventual propagation across devices.

## 2. Design iCloud Photos

* Question
  Design iCloud Photos for backup, sync, search, thumbnails, and multi-device viewing.

* Answer
  **Scope**
  Support photo/video upload, dedupe, albums, metadata sync, thumbnails, optimized device storage, sharing, and recovery. Exclude professional editing workflows unless asked.

  **Functional Requirements**
  Upload originals, generate thumbnails, sync albums/metadata, stream/download assets, support delete/restore, search by metadata, and optimize local storage.

  **Non Functional Requirements**
  Extremely durable originals, battery-aware mobile uploads, privacy, low browse latency, background operation, cost-efficient storage, and regional reliability.

  **High level design and diagram (at block level)**
  ```text
  Photos App -> Upload Manager -> Ingestion API -> Object Store
       |              |               |
       v              v               v
  Local Index     Metadata API -> Photo Metadata DB
                                  |
                                  v
                         Media Processing Queue
                                  |
                    +-------------+-------------+
                    v                           v
             Thumbnail Service           Transcode Service
                    |
                    v
                   CDN
  ```

  *** Explain the blocks
  Upload Manager batches, dedupes, and retries. Ingestion API validates, authenticates, and stores originals. Metadata API stores asset IDs, albums, timestamps, device info, and user-visible state. Processing Queue decouples upload from thumbnails/transcodes. CDN serves thumbnails and optimized renditions.

  *** Explain the control flow
  Control plane manages user quota, retention, sharing permissions, processing policies, ML feature availability, and privacy settings. Rollouts for new thumbnail formats or storage tiers are gated and reversible.

  *** Explain the data flow
  Device uploads original media and metadata. The system stores the original, commits metadata, queues derivative generation, and publishes sync updates. Other devices fetch metadata deltas first, then lazily download thumbnails or originals.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Original durability vs storage cost
  Options:
  - Store every original in hot multi-region storage: highest availability; very expensive.
  - Tier originals into warm/cold storage: cheaper; slower recovery/download for old assets.
  - Hybrid hot recent assets plus cold archive: good UX/cost balance; more lifecycle complexity.
  Recommendation: hot recent/user-active assets, durable cold tier for old originals, thumbnails hot.

  *** Cloud ML search vs on-device privacy
  Options:
  - Cloud indexing: powerful cross-device search; privacy-sensitive.
  - On-device indexing: strong privacy; inconsistent compute quality and sync complexity.
  - Privacy-preserving hybrid: device-derived labels synced securely; more complex.
  Recommendation: prefer on-device or privacy-preserving metadata, with explicit user controls.

## 3. Design Apple Music or Apple TV+ Streaming

* Question
  Design a global media streaming system like Apple Music or Apple TV+.

* Answer
  **Scope**
  Support catalog browsing, playback, recommendations, rights/entitlements, adaptive bitrate streaming, offline downloads, and playback telemetry.

  **Functional Requirements**
  Search catalog, start playback, serve adaptive media segments, enforce entitlements/DRM, support playlists/library, offline downloads, and collect quality metrics.

  **Non Functional Requirements**
  Low startup latency, high availability, global scale, CDN efficiency, rights correctness, device compatibility, and cost control.

  **High level design and diagram (at block level)**
  ```text
  Client -> Catalog/Search API -> Catalog DB / Search Index
     |
     v
  Playback API -> Entitlement/Rights Service -> License Service
     |
     v
  Manifest Service -> CDN -> Media Segments
     |
     v
  Playback Telemetry -> Stream Processor -> QoE Analytics
  ```

  *** Explain the blocks
  Catalog/Search exposes discoverability. Playback API authorizes a session. Entitlement/Rights checks subscription, country, licensing window, and device rules. Manifest Service returns adaptive bitrate manifests. CDN serves media segments. Telemetry captures buffering, bitrate, errors, and completion.

  *** Explain the control flow
  Control plane manages catalog ingestion, rights windows, DRM policy, CDN purge rules, recommendation model rollout, and experiment config. Rights changes should invalidate or age out manifests quickly.

  *** Explain the data flow
  Client selects content, gets authorization and manifest, then downloads segments from nearest CDN. Playback telemetry flows asynchronously into analytics and recommendation systems.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Rights correctness vs cache performance
  Options:
  - Long-lived cached manifests: fast and cheap; stale rights risk.
  - Short-lived signed manifests: rights-safe; more control-plane load.
  - Hybrid: long-lived public metadata, short-lived playback tokens.
  Recommendation: signed playback sessions and short-lived manifests; cache media segments aggressively.

  *** Recommendation serving
  Options:
  - Precomputed recommendations: low latency; stale.
  - Online ranking: fresh/personalized; expensive and failure-prone.
  - Candidate precompute plus online rerank: balanced; more moving parts.
  Recommendation: hybrid with fallback to precomputed rows.

## 4. Design Apple Maps

* Question
  Design Apple Maps for search, map rendering, routing, and live traffic.

* Answer
  **Scope**
  Support map tiles, POI search, geocoding, route calculation, live traffic, offline caching, and client navigation updates.

  **Functional Requirements**
  Render maps, search places, convert address to location, compute routes, update ETA, ingest traffic signals, and support mobile caching.

  **Non Functional Requirements**
  Low latency, high freshness for traffic, global coverage, location privacy, high availability, and efficient mobile bandwidth.

  **High level design and diagram (at block level)**
  ```text
  Client -> Edge/API Gateway
       +-> Tile Service -> Tile Store/CDN
       +-> Search/Geocoder -> POI Index
       +-> Routing Service -> Road Graph Store
       +-> Traffic Service -> Stream Processor -> Traffic Store
  ```

  *** Explain the blocks
  Tile Service serves vector/raster map tiles. Search/Geocoder resolves text and addresses. Routing Service computes paths over road graphs. Traffic Service ingests anonymized speed/incidents. Edge gateway handles auth, rate limits, privacy protections, and regional routing.

  *** Explain the control flow
  Control plane manages map data imports, tile generation jobs, road graph versions, POI moderation, privacy policy, and rollout of new map regions or algorithms.

  *** Explain the data flow
  Client requests visible tiles, searches for a destination, asks for a route, and receives route geometry plus ETA. During navigation, the client sends privacy-preserving signals and receives traffic/route updates.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Precomputed vs dynamic routing
  Options:
  - Precompute popular routes: fast; cannot cover long tail well.
  - Fully dynamic routing: accurate; CPU-intensive and latency-sensitive.
  - Dynamic routing with cached subpaths/heuristics: balanced; cache invalidation complexity.
  Recommendation: dynamic routing with hierarchical road graphs, traffic overlays, and regional caches.

  *** Location privacy
  Options:
  - Raw location telemetry: best traffic quality; privacy risk.
  - Aggregated/anonymized telemetry: safer; less precise.
  - On-device differential privacy/noise: strong privacy; harder analytics.
  Recommendation: minimize, aggregate, and decouple identity from location signals.

## 5. Design AirTag / Find My

* Question
  Design a privacy-preserving item location network like AirTag and Find My.

* Answer
  **Scope**
  Support BLE beaconing, anonymous crowd-sourced sightings, owner lookup, lost mode, anti-stalking safety, and battery-efficient operation.

  **Functional Requirements**
  Register item, emit rotating identifiers, upload sightings from nearby devices, let owner retrieve location, notify lost mode, and detect unwanted tracking.

  **Non Functional Requirements**
  Strong privacy, low battery usage, abuse resistance, high availability, low data volume, and global reach.

  **High level design and diagram (at block level)**
  ```text
  AirTag -> BLE Rotating ID -> Nearby Apple Device
                                 |
                                 v
                          Sighting Upload API
                                 |
                                 v
                      Encrypted Sighting Store
                                 |
  Owner Device -> Find My API -> Private Lookup -> Location Result
  ```

  *** Explain the blocks
  AirTag emits rotating public identifiers. Nearby devices detect beacons and upload encrypted sightings. Sighting Store indexes encrypted reports by rotating ID. Owner Device derives lookup keys and decrypts results locally. Safety services detect suspicious movement patterns.

  *** Explain the control flow
  Control plane handles item pairing, key rotation policy, lost mode state, abuse thresholds, firmware update policy, and device trust attestation.

  *** Explain the data flow
  AirTag broadcasts an ephemeral ID. A nearby device uploads encrypted location/time. The owner asks for recent sightings using derived keys and decrypts location locally.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Privacy vs findability
  Options:
  - Stable IDs: easy lookup; enables tracking by observers.
  - Rotating IDs: privacy-preserving; owner lookup and indexing are harder.
  - Rotating IDs with owner-derived keys: strong balance; cryptographic complexity.
  Recommendation: rotating IDs with encrypted sightings and owner-only decryption.

  *** Anti-stalking controls
  Options:
  - Device alerts when unknown tag follows user: user-protective; false positives.
  - Audible alerts from tag: works cross-platform; delayed detection risk.
  - Combined alerts plus platform APIs: best coverage; more product complexity.
  Recommendation: layered detection with conservative thresholds and clear UX.

## 6. Design APNs / Push Notification Service

* Question
  Design Apple Push Notification service for sending notifications to billions of devices.

* Answer
  **Scope**
  Support provider APIs, device tokens, topic auth, priority/TTL, collapse IDs, delivery to online/offline devices, retries, and observability.

  **Functional Requirements**
  Accept provider notifications, authenticate providers, route to devices, queue offline messages, collapse obsolete messages, and expose delivery status where appropriate.

  **Non Functional Requirements**
  Very low latency, massive fanout, high availability, backpressure, abuse prevention, and privacy/security.

  **High level design and diagram (at block level)**
  ```text
  Provider -> APNs Gateway -> Auth/Topic Service
                      |
                      v
               Routing Service -> Device Connection Registry
                      |
             +--------+--------+
             v                 v
        Online Push       Offline Queue
             |                 |
             v                 v
          Device <----- Reconnect Delivery
  ```

  *** Explain the blocks
  APNs Gateway terminates provider connections. Auth/Topic validates certificates/tokens and app topics. Routing Service maps device tokens to connection regions. Device Connection Registry tracks active long-lived device connections. Offline Queue stores bounded TTL messages.

  *** Explain the control flow
  Control plane manages provider credentials, app topics, token lifecycle, abuse policies, per-provider quotas, priority rules, and regional failover configuration.

  *** Explain the data flow
  Provider submits a notification. APNs authenticates it, resolves the device connection, sends immediately if online, or stores/collapses it if offline. Delivery attempts and errors are logged asynchronously.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Delivery semantics
  Options:
  - At-most-once: low duplicate risk; can drop messages.
  - At-least-once: reliable; duplicate risk.
  - Exactly-once: attractive but unrealistic across mobile networks.
  Recommendation: at-least-once internally with collapse IDs, TTLs, and idempotent client handling.

  *** Backpressure
  Options:
  - Reject excess traffic: protects system; provider-visible failures.
  - Queue everything: better acceptance; overload and stale notifications.
  - Priority-aware throttling: balanced; requires careful policy.
  Recommendation: per-provider quotas, priority lanes, TTL-aware dropping, and clear error codes.

## 7. Design App Store Backend

* Question
  Design the backend for App Store browsing, downloads, purchases, and app updates.

* Answer
  **Scope**
  Support app catalog, search, app metadata, reviews/ratings, purchases, entitlements, binary downloads, updates, and fraud controls.

  **Functional Requirements**
  Search apps, view detail pages, buy/download apps, validate entitlement, serve binaries, process updates, and manage reviews.

  **Non Functional Requirements**
  High read availability, purchase correctness, CDN efficiency, security, integrity of binaries, auditability, and launch-event scalability.

  **High level design and diagram (at block level)**
  ```text
  Client -> App Store API
       +-> Catalog Service -> Catalog DB/Search Index
       +-> Purchase Service -> Payment/Receipt Ledger
       +-> Entitlement Service -> Entitlement DB
       +-> Download Service -> Signed URLs -> CDN/Object Store
       +-> Review Service -> Moderation Queue
  ```

  *** Explain the blocks
  Catalog Service owns app metadata. Search Index supports discovery. Purchase Service handles transactions and receipts. Entitlement Service answers whether a user/device can download. Download Service grants signed URLs for app binaries. Review Service moderates user-generated content.

  *** Explain the control flow
  Control plane includes developer submission, review workflow, binary signing, release scheduling, pricing, regional availability, and takedown policies.

  *** Explain the data flow
  User searches/selects an app, purchase/entitlement is checked, client receives a signed download URL, downloads from CDN, and validates binary signatures locally.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Purchase correctness vs catalog availability
  Options:
  - Single strongly consistent backend for everything: simple; poor scale/availability.
  - Separate purchase ledger from catalog reads: scalable; more integration complexity.
  Recommendation: strongly consistent purchase/entitlement path; eventually consistent catalog/search path.

  *** App update rollout
  Options:
  - Immediate global release: simple; high blast radius.
  - Phased rollout: safer; more state and support complexity.
  Recommendation: phased rollout with telemetry gates and fast rollback/takedown.

## 8. Design Apple Pay

* Question
  Design Apple Pay for tokenized in-store and online payments.

* Answer
  **Scope**
  Support device token provisioning, merchant payment authorization, wallet selection, network/bank integration, fraud controls, receipts, and reconciliation.

  **Functional Requirements**
  Provision payment tokens, authenticate user/device, authorize payment, handle merchant callbacks, record ledger events, and reconcile with payment networks.

  **Non Functional Requirements**
  Very high correctness, low checkout latency, PCI/security compliance, fraud resistance, idempotency, auditability, and regional regulation support.

  **High level design and diagram (at block level)**
  ```text
  Wallet/Device -> Payment Session API -> Token Vault/HSM
          |                |
          v                v
     Device Auth     Risk/Fraud Service
          |                |
          v                v
  Payment Orchestrator -> Network/Bank Gateway
          |
          v
    Payment Ledger -> Reconciliation Pipeline
  ```

  *** Explain the blocks
  Wallet signs payment requests. Payment Session API validates session and merchant. Token Vault/HSM protects sensitive tokens/keys. Risk Service scores transactions. Orchestrator talks to payment networks/banks. Ledger records immutable payment state. Reconciliation compares internal records with network settlement.

  *** Explain the control flow
  Control plane manages merchant onboarding, device provisioning, bank/network configuration, risk rules, key rotation, compliance policy, and operational kill switches.

  *** Explain the data flow
  Device creates an authenticated payment cryptogram, merchant submits payment, system validates token and risk, sends authorization to network, records state transitions, and later reconciles settlement.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Idempotent payment authorization
  Options:
  - Retry without idempotency: simple; duplicate charges.
  - Idempotency key per merchant transaction: prevents duplicates; requires durable lookup.
  - Ledger-first state machine: strongest auditability; more latency/complexity.
  Recommendation: durable idempotency plus ledger state machine.

  *** Fraud latency tradeoff
  Options:
  - Full synchronous risk model: safer; slower and less available.
  - Async risk after approval: fast; may approve bad transactions.
  - Tiered risk: synchronous fast checks plus async review.
  Recommendation: tiered risk with strict timeouts and fallback policy.

## 9. Design iMessage

* Question
  Design iMessage for encrypted multi-device messaging.

* Answer
  **Scope**
  Support one-to-one and group messaging, device fanout, offline delivery, attachments, read receipts, reactions, and end-to-end encryption.

  **Functional Requirements**
  Register devices/keys, send messages, deliver to all recipient devices, store offline messages, support attachments, receipts, and group membership.

  **Non Functional Requirements**
  Privacy, low latency, high availability, ordering per conversation, abuse controls, attachment durability, and mobile-friendly operation.

  **High level design and diagram (at block level)**
  ```text
  Sender -> Messaging Gateway -> Identity/Key Service
              |
              v
         Message Router -> Per-Recipient Queues -> Device Connections/APNs
              |
              v
       Attachment Service -> Object Store/CDN
  ```

  *** Explain the blocks
  Identity/Key Service maps accounts to device public keys. Gateway accepts encrypted envelopes. Router fans out to recipient devices. Queues handle offline delivery. Attachment Service stores encrypted blobs. APNs wakes disconnected devices.

  *** Explain the control flow
  Control plane handles account/device registration, key publication, group membership, abuse policies, attachment retention, and feature flags for message types.

  *** Explain the data flow
  Sender encrypts per recipient device, uploads attachment if needed, sends envelope to gateway, router enqueues messages for recipient devices, and devices acknowledge receipt/read state.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** End-to-end encryption vs server features
  Options:
  - Server-readable messages: easier search/moderation; weak privacy.
  - E2E encryption: strong privacy; server cannot inspect content.
  - Client-side features with encrypted metadata minimization: balanced; client complexity.
  Recommendation: E2E content encryption and minimal server metadata.

  *** Message ordering
  Options:
  - Global ordering: simple semantics; expensive and unnecessary.
  - Per-conversation ordering: intuitive; manageable.
  - Best-effort device ordering: cheap; confusing UX.
  Recommendation: per-conversation sequence numbers with client reconciliation.

## 10. Design FaceTime Signaling and Media Routing

* Question
  Design FaceTime call setup, signaling, and media routing.

* Answer
  **Scope**
  Support one-to-one/group calls, device ringing, session negotiation, NAT traversal, media relay fallback, call state, and quality telemetry.

  **Functional Requirements**
  Find recipient devices, ring devices, negotiate codecs, establish media path, handle join/leave, reconnect, and report call quality.

  **Non Functional Requirements**
  Low setup latency, low media latency/jitter, privacy, high availability, efficient bandwidth, and graceful degradation.

  **High level design and diagram (at block level)**
  ```text
  Caller -> Signaling Service -> Identity/Device Registry
             |
             v
         Call State Service -> APNs Ring
             |
             v
      NAT/STUN/TURN Service
             |
       +-----+------+
       v            v
  Peer-to-Peer   Media Relay/SFU
  ```

  *** Explain the blocks
  Signaling Service coordinates call setup. Device Registry finds active devices. Call State tracks sessions and participants. APNs rings devices. NAT/STUN/TURN helps connectivity. Media Relay/SFU is fallback or group media infrastructure.

  *** Explain the control flow
  Control plane manages codec policy, relay capacity, region routing, abuse/rate limits, feature rollout, and emergency kill switches.

  *** Explain the data flow
  Caller creates a call, signaling rings recipient devices, clients exchange session descriptions, attempt peer-to-peer media, and fall back to relays/SFU when direct media fails.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Peer-to-peer vs relay
  Options:
  - Peer-to-peer: low server cost/latency; NAT failures and inconsistent quality.
  - Relay all media: reliable connectivity; expensive and higher latency.
  - Hybrid fallback: best balance; complexity.
  Recommendation: prefer P2P for small calls, relay/SFU for failed NAT and group calls.

  *** Group call architecture
  Options:
  - Mesh P2P: no server media; poor scaling.
  - MCU mixing: simple clients; high server CPU and privacy concerns.
  - SFU forwarding: scalable and common; clients handle more streams.
  Recommendation: SFU with adaptive bitrate and regional placement.

## 11. Design Siri / Apple Intelligence RAG Assistant

* Question
  Design a privacy-aware assistant that uses device context, retrieval, and LLM inference.

* Answer
  **Scope**
  Support user requests, intent classification, device/private context, retrieval from approved sources, model routing, streaming responses, safety policy, and feedback.

  **Functional Requirements**
  Accept voice/text, understand intent, retrieve relevant data, call tools, stream answer, execute permitted actions, and collect quality signals.

  **Non Functional Requirements**
  Low latency, privacy by design, safety, availability under model overload, cost control, personalization, and auditability for actions.

  **High level design and diagram (at block level)**
  ```text
  Client -> On-device Intent/Privacy Filter
       |
       v
  Assistant Gateway -> Policy Service -> Model Router
       |                    |             |
       v                    v             v
  Retrieval API -> Vector/Keyword Index   LLM Inference
       |                                  |
       +-------------> Tool Orchestrator <-+
                          |
                          v
                    Streaming Response
  ```

  *** Explain the blocks
  On-device layer handles wake word, local context, and privacy filtering. Gateway manages sessions. Policy Service gates data/tool access. Model Router chooses local/cloud model. Retrieval fetches grounding data. Tool Orchestrator executes safe actions. Inference streams answer tokens.

  *** Explain the control flow
  Control plane manages model versions, safety policies, tool permissions, data retention, prompt templates, eval gates, rollout cohorts, and kill switches.

  *** Explain the data flow
  Request is classified locally, permitted context is sent or retained locally, retrieval grounds the response, model streams output, tool calls are confirmed/executed, and feedback is logged with privacy constraints.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** On-device vs cloud inference
  Options:
  - On-device only: privacy/latency; limited model size.
  - Cloud only: strongest model; privacy, cost, and network dependency.
  - Hybrid routing: best balance; policy complexity.
  Recommendation: hybrid with sensitive tasks local and explicit privacy boundaries.

  *** RAG freshness and correctness
  Options:
  - Prebuilt index: fast; stale.
  - Real-time retrieval: fresh; slower and failure-prone.
  - Hybrid cache plus live fetch: balanced.
  Recommendation: hybrid with source attribution and confidence handling.

## 12. Design Xcode Cloud Build System

* Question
  Design a cloud CI/build system for Apple developers.

* Answer
  **Scope**
  Support repo webhooks, build scheduling, dependency restore, isolated macOS build workers, signing, artifact storage, logs, test results, and notifications.

  **Functional Requirements**
  Trigger builds, schedule workers, run tests, sign artifacts, store logs/artifacts, report status, and support retries/cancellation.

  **Non Functional Requirements**
  Isolation, reproducibility, throughput, fairness, low queue time, secret safety, observability, and cost efficiency.

  **High level design and diagram (at block level)**
  ```text
  Git Provider -> Webhook API -> Build Orchestrator -> Scheduler
                                      |
                                      v
                              Worker Pool Manager
                                      |
                                      v
                           macOS Build Workers
                              |       |       |
                              v       v       v
                          Cache   Signing  Logs/Artifacts
  ```

  *** Explain the blocks
  Webhook API receives code events. Orchestrator creates build DAGs. Scheduler assigns jobs by priority/quota. Worker Pool Manager provisions isolated workers. Build Workers run builds/tests. Cache speeds dependencies. Signing service protects credentials. Logs/Artifacts persist outputs.

  *** Explain the control flow
  Control plane manages project config, build templates, quotas, worker images, signing permissions, secrets, cache policy, and rollout of Xcode versions.

  *** Explain the data flow
  Commit triggers workflow, scheduler provisions worker, worker checks out code, restores cache, builds/tests/signs, streams logs, uploads artifacts, and sends status notifications.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Worker isolation
  Options:
  - Shared long-lived workers: fast/cheap; contamination risk.
  - Fresh VM per build: secure/reproducible; slower and costly.
  - Warm isolated pool: balance; image hygiene complexity.
  Recommendation: warm isolated VMs with clean snapshots and strict secret injection.

  *** Dependency caching
  Options:
  - No cache: reproducible; slow.
  - Global shared cache: fast; poisoning risk.
  - Content-addressed scoped cache: safe balance; more metadata.
  Recommendation: scoped content-addressed caches with trust boundaries.

## 13. Design Build Artifact Storage

* Question
  Design artifact storage for build outputs, logs, symbols, and test bundles.

* Answer
  **Scope**
  Support immutable artifacts, metadata, access control, retention, dedupe, large downloads, provenance, and audit logs.

  **Functional Requirements**
  Upload artifacts, retrieve by build/version, store logs, dedupe blobs, enforce permissions, expire old data, and support signed URLs.

  **Non Functional Requirements**
  Durability, integrity, high read throughput, cost controls, security, auditability, and reproducibility.

  **High level design and diagram (at block level)**
  ```text
  Build Worker -> Artifact API -> Metadata Service -> Metadata DB
         |              |
         v              v
  Chunker/Hasher -> Blob Store -> Replication/Lifecycle
                         |
                         v
                    CDN/Signed URL
  ```

  *** Explain the blocks
  Artifact API handles uploads/download authorization. Metadata Service tracks builds, artifact manifests, checksums, provenance, and ACLs. Chunker/Hasher enables dedupe and integrity. Blob Store stores immutable chunks. Lifecycle moves/deletes old data. CDN accelerates downloads.

  *** Explain the control flow
  Control plane manages retention policies, repository/team ACLs, legal holds, lifecycle tiers, checksum algorithms, and replication policy.

  *** Explain the data flow
  Worker uploads chunked artifact with checksums. Metadata commit records manifest atomically. Consumers request artifact, authorization is checked, and signed URLs stream blobs from CDN/object storage.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Mutable vs immutable artifacts
  Options:
  - Mutable paths: convenient; breaks reproducibility.
  - Immutable content-addressed artifacts: reproducible; needs alias metadata.
  - Immutable blobs plus mutable labels: balanced; alias audit required.
  Recommendation: immutable artifacts with versioned aliases.

  *** Retention and cost
  Options:
  - Keep all artifacts hot: simple; expensive.
  - Aggressive deletion: cheap; harms debugging/compliance.
  - Policy-based tiering: balanced; operational complexity.
  Recommendation: tier by age, branch importance, release status, and legal hold.

## 14. Design iOS Software Update Rollout

* Question
  Design the system that rolls out iOS/macOS software updates globally.

* Answer
  **Scope**
  Support update eligibility, signed manifests, delta packages, staged rollout, CDN delivery, device checks, telemetry gates, and rollback/blocking.

  **Functional Requirements**
  Publish update metadata, match eligible devices, download packages, verify signatures, install safely, monitor health, and pause/rollback rollout.

  **Non Functional Requirements**
  Security, integrity, availability, massive CDN scale, low device disruption, blast-radius control, and observability.

  **High level design and diagram (at block level)**
  ```text
  Release System -> Signing/Manifest Service -> Update Catalog
                         |
                         v
  Rollout Controller -> Eligibility API -> Device
                         |
                         v
                    CDN/Object Store
                         |
                         v
                 Telemetry/Health Gates
  ```

  *** Explain the blocks
  Release System publishes builds. Signing Service signs manifests/packages. Update Catalog stores available versions. Rollout Controller manages cohorts and percentages. Eligibility API tells devices what to install. CDN distributes packages. Health Gates monitor crashes, install failures, battery, and regressions.

  *** Explain the control flow
  Control plane defines rollout cohorts, device eligibility, regional schedules, blocklists, signing keys, telemetry thresholds, and emergency pauses.

  *** Explain the data flow
  Device checks eligibility, downloads signed manifest/package from CDN, verifies locally, installs, reports health signals, and rollout controller advances or pauses based on metrics.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Phased rollout vs global release
  Options:
  - Global release: fast; huge blast radius.
  - Manual phased rollout: safer; slower operator loop.
  - Automated gated rollout: scalable; risk of bad metrics/policies.
  Recommendation: staged rollout with automated gates and human override.

  *** Delta vs full packages
  Options:
  - Full package: simple/reliable; bandwidth heavy.
  - Delta package: efficient; more variants and validation complexity.
  - Hybrid: delta when safe, full fallback.
  Recommendation: hybrid with cryptographic verification and full fallback.

## 15. Design a Data Center / Cloud Region Control System

* Question
  Design software to manage capacity, scheduling, health, and failure domains in a large data center or cloud region.

* Answer
  **Scope**
  Support machine inventory, cluster scheduling, service placement, failure-domain awareness, capacity planning, health checks, maintenance, and incident response.

  **Functional Requirements**
  Register hosts, place workloads, monitor health, drain hosts, enforce quotas, track capacity, and support regional failover.

  **Non Functional Requirements**
  High availability, fault isolation, operational safety, efficient utilization, auditability, and predictable failure behavior.

  **High level design and diagram (at block level)**
  ```text
  Operators/Services -> Control API -> Scheduler -> Placement DB
                              |           |
                              v           v
                         Inventory    Health Monitor
                              |           |
                              v           v
                        Host Agents -> Metrics/Event Stream
  ```

  *** Explain the blocks
  Control API accepts workload and maintenance requests. Scheduler places workloads based on capacity and failure domains. Placement DB stores desired state. Inventory tracks hosts/racks/power/network. Health Monitor evaluates liveness. Host Agents enforce desired state and report telemetry.

  *** Explain the control flow
  Control plane owns desired state: workload placement, quotas, maintenance windows, admission control, and failover policy. Changes go through validation and staged execution.

  *** Explain the data flow
  Host agents report health and capacity. Scheduler computes placement. Desired state is pushed/pulled to hosts. Metrics/events drive alerts, autoscaling, and capacity planning.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Utilization vs reliability
  Options:
  - Pack workloads tightly: efficient; correlated failure risk.
  - Spread aggressively: resilient; lower utilization.
  - Policy-based placement: balance by workload criticality.
  Recommendation: failure-domain-aware scheduling with workload tiers.

  *** Control plane availability
  Options:
  - Centralized scheduler: simple; single-region bottleneck.
  - Per-cluster schedulers: scalable; global optimization harder.
  - Hierarchical control plane: balanced; coordination complexity.
  Recommendation: hierarchical: global capacity planning, local scheduling.

## 16. Design Device Telemetry and Crash Analytics

* Question
  Design a privacy-conscious telemetry and crash analytics platform for Apple devices.

* Answer
  **Scope**
  Support crash reports, performance metrics, sampled diagnostics, symbolication, aggregation, dashboards, alerting, and privacy controls.

  **Functional Requirements**
  Collect reports, batch uploads, redact sensitive fields, ingest events, symbolize crashes, aggregate metrics, trigger alerts, and support debugging queries.

  **Non Functional Requirements**
  Privacy, high ingestion scale, cost-efficient storage, near-real-time alerts, retention limits, and reliability during incident spikes.

  **High level design and diagram (at block level)**
  ```text
  Device -> Local Buffer/Sampler -> Upload Gateway
                                  |
                                  v
                           Ingestion Stream
                                  |
                  +---------------+--------------+
                  v                              v
          Crash Symbolication              Metrics Aggregation
                  |                              |
                  v                              v
           Crash Store/Search              Dashboards/Alerts
  ```

  *** Explain the blocks
  Local Buffer batches and samples. Upload Gateway authenticates and rate limits. Ingestion Stream absorbs spikes. Symbolication resolves stack traces. Aggregation computes rates and cohorts. Dashboards/Alerts expose health.

  *** Explain the control flow
  Control plane manages consent, sampling rates, privacy redaction rules, retention, symbol files, alert thresholds, and incident-specific collection changes.

  *** Explain the data flow
  Device stores events locally, uploads when conditions permit, gateway redacts/validates, stream processors aggregate or symbolize, and results feed dashboards/alerts.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Privacy vs debuggability
  Options:
  - Rich raw logs: easy debugging; privacy risk.
  - Strict aggregation only: privacy; weak root cause analysis.
  - Redacted structured diagnostics with sampling: balance.
  Recommendation: minimize raw data, redact on device/server, and use scoped sampling.

  *** Incident spike handling
  Options:
  - Provision for peak: reliable; expensive.
  - Drop aggressively: cheap; lose signal.
  - Queue plus adaptive sampling: balanced.
  Recommendation: durable ingestion stream with adaptive per-cohort sampling.

## 17. Design Typeahead Search

* Question
  Design a typeahead suggestion system for Apple services such as App Store, Maps, Music, or Spotlight.

* Answer
  **Scope**
  Support prefix suggestions, typo tolerance, ranking, localization, personalization, abuse controls, and low-latency serving.

  **Functional Requirements**
  Return suggestions as user types, support locale/device context, rank by popularity and personalization, update indexes, and filter unsafe content.

  **Non Functional Requirements**
  Sub-100ms latency, high QPS, global availability, freshness, privacy, and graceful fallback.

  **High level design and diagram (at block level)**
  ```text
  Client -> Edge Suggest API -> Suggestion Service
                             |
              +--------------+--------------+
              v                             v
        Prefix/FST Index              Ranking Service
              |                             |
              v                             v
       Offline Index Builder <- Query Logs/Content Feed
  ```

  *** Explain the blocks
  Edge API provides low-latency global entry. Suggestion Service fetches candidates. Prefix/FST Index stores compressed suggestions. Ranking Service orders candidates. Offline Builder processes query logs/content feeds and publishes new index versions.

  *** Explain the control flow
  Control plane manages index versions, locale rollout, ranking model versions, bad-query filters, privacy policy, and A/B experiments.

  *** Explain the data flow
  Client sends prefix/context, edge routes to nearest serving cluster, candidates are fetched from prefix index, ranked, filtered, and returned. Logs are sampled and used offline to rebuild indexes.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Freshness vs latency
  Options:
  - Fully precomputed index: very fast; stale.
  - Real-time query over source DB: fresh; too slow.
  - Precomputed base plus real-time overlay: balanced.
  Recommendation: versioned offline index with small real-time overlay for trending/new content.

  *** Personalization vs privacy
  Options:
  - Server-side personal profiles: powerful; privacy-sensitive.
  - On-device personalization: private; less global context.
  - Anonymous cohort personalization: middle ground.
  Recommendation: on-device reranking where possible, minimal server context.

## 18. Design Family Sharing / Subscription Entitlements

* Question
  Design entitlement and family-sharing infrastructure for Apple subscriptions and purchases.

* Answer
  **Scope**
  Support purchases, subscriptions, family groups, entitlement lookup, revocation, offline grace, app/service authorization, and audit history.

  **Functional Requirements**
  Create/update subscriptions, share eligible purchases, check entitlement, handle renewal/failure, revoke access, and notify devices/apps.

  **Non Functional Requirements**
  Correctness, low-latency reads, auditability, privacy, regional compliance, and availability during billing-provider issues.

  **High level design and diagram (at block level)**
  ```text
  Purchase/Billing -> Entitlement Writer -> Entitlement DB/Ledger
                             |
                             v
                       Family Graph Service
                             |
  App/Device -> Entitlement Read API -> Cache -> Entitlement DB
                             |
                             v
                         APNs Updates
  ```

  *** Explain the blocks
  Billing emits purchase and renewal events. Entitlement Writer updates immutable history and current state. Family Graph maps sharing relationships. Read API answers authorization. Cache speeds hot checks. APNs informs devices of changes.

  *** Explain the control flow
  Control plane manages product eligibility, family rules, region restrictions, billing grace periods, cache TTLs, and dispute/refund policies.

  *** Explain the data flow
  Purchase event updates ledger/current entitlement. Apps/devices check entitlement via read API or local cache. Changes are pushed to devices and eventually refresh local state.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Cached entitlements vs revocation correctness
  Options:
  - Long local cache: great offline UX; stale access after revocation.
  - Always online check: correct; poor availability/offline UX.
  - Bounded offline grace: balanced.
  Recommendation: signed local entitlement with short TTL and grace policy by product risk.

  *** Family graph consistency
  Options:
  - Strong transactions across family and entitlements: correct; less scalable.
  - Eventual updates: scalable; temporary wrong access.
  - Strong graph writes, eventual entitlement propagation: balanced.
  Recommendation: strong consistency for family membership changes, event-driven entitlement updates.

## 19. Design HomeKit Cloud Sync and Remote Access

* Question
  Design HomeKit sync and remote access for smart-home devices.

* Answer
  **Scope**
  Support local device control, home membership, encrypted state sync, remote commands, home hubs, automations, and offline behavior.

  **Functional Requirements**
  Add devices, sync home config, authorize users, send commands locally/remotely, update state, run automations, and handle hub failover.

  **Non Functional Requirements**
  Privacy, low local latency, high reliability, security, offline local operation, and safe command execution.

  **High level design and diagram (at block level)**
  ```text
  iPhone/Home App -> Home Config Sync API -> Encrypted Config Store
          |
          v
  Local Network/Home Hub -> Device Protocols
          |
          v
  Remote Access Relay -> Command Broker -> Home Hub
  ```

  *** Explain the blocks
  Home App manages user interactions. Config Sync stores encrypted home/device state. Home Hub coordinates local devices and remote access. Remote Relay/Broker forwards encrypted commands when user is away. Device Protocols connect to smart devices.

  *** Explain the control flow
  Control plane manages home membership, device pairing, access roles, automation rules, hub election, certificates, and remote-access policy.

  *** Explain the data flow
  At home, commands flow locally from app/hub to devices. Away from home, app sends encrypted command through relay to home hub, which executes locally and returns status.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Local-first vs cloud-first
  Options:
  - Cloud-first: easier remote control; poor local outage behavior.
  - Local-first: fast/private/reliable at home; remote coordination harder.
  - Local-first plus relay: best Apple-like behavior.
  Recommendation: local-first control with cloud relay for remote access.

  *** Hub failover
  Options:
  - Single fixed hub: simple; single point of failure.
  - Dynamic hub election: resilient; split-brain risk.
  - Priority-based election with leases: balanced.
  Recommendation: lease-based primary hub with standby takeover.

## 20. Design Private Relay / Privacy-Preserving Proxy

* Question
  Design a privacy-preserving relay service that hides user IP from destination sites and browsing destination from ingress relays.

* Answer
  **Scope**
  Support two-hop proxying, user authentication, abuse controls, geo-aware egress, performance, operational visibility without sensitive logs, and fallback behavior.

  **Functional Requirements**
  Authenticate eligible users, accept encrypted traffic, route through ingress and egress relays, preserve approximate geography, prevent abuse, and measure reliability.

  **Non Functional Requirements**
  Strong privacy separation, low latency overhead, high availability, global capacity, abuse resistance, and minimal logging.

  **High level design and diagram (at block level)**
  ```text
  Client -> Ingress Relay -> Egress Relay -> Destination
      |          |                |
      v          v                v
  Auth Token  No Destination   No User Identity
      |
      v
  Control Plane: Token Issuer / Relay Directory / Abuse Policy
  ```

  *** Explain the blocks
  Client obtains privacy-preserving auth token. Ingress Relay sees user IP but not final destination. Egress Relay sees destination but not user identity. Relay Directory helps clients choose relays. Abuse Policy limits misuse without full identity logs.

  *** Explain the control flow
  Control plane manages token issuance, relay health, routing policy, capacity, abuse thresholds, egress geography, key rotation, and emergency disablement.

  *** Explain the data flow
  Client establishes encrypted tunnel to ingress, ingress forwards encrypted destination request to egress, egress connects to destination, and responses return through the same chain.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Privacy vs abuse prevention
  Options:
  - Full logs: easy abuse/debugging; violates privacy goals.
  - No logs/controls: private; invites abuse and hard operations.
  - Privacy-preserving tokens/rate limits: balanced; complex.
  Recommendation: anonymous tokens, aggregate metrics, coarse rate limits, and minimal retention.

  *** Latency overhead
  Options:
  - Random relay selection: stronger unlinkability; worse latency.
  - Nearest relay selection: fast; more predictable routing.
  - Region-aware randomized selection: balanced.
  Recommendation: region-aware relay choice with health and latency scoring.

## 21. Design Elastic Disk / EBS-Class Block Storage

* Question
  Design a cloud Elastic Disk service that provides durable, low-latency block volumes for virtual machines, containers, databases, and analytics jobs.

* Answer
  **Scope**
  Support volume create/delete, attach/detach, resize, read/write block I/O, snapshots, restores, migrations, encryption, multi-tenant quotas, and host failure recovery. Exclude full file-system semantics and customer-visible object APIs.

  **Functional Requirements**
  Create volumes with size, performance class, encryption policy, and placement constraints. Attach a volume to a compute instance through a virtual block device. Serve reads and writes with block offsets and lengths. Replicate data, detect corruption, repair replicas, expose volume health, and support point-in-time snapshots.

  **Non Functional Requirements**
  Single-digit millisecond common-path latency, high write availability within a failure domain, very high durability, predictable IOPS and throughput, tenant isolation, safe fleet operations, auditable control-plane changes, and graceful degradation during disk, host, rack, and zone failures.

  **High level design and diagram (at block level)**
  ```text
  Compute Host
    -> Volume Agent / NVMe-oF Target
    -> Storage Frontend
    -> Replication Group Leader
    -> Replica Storage Nodes

  Control Plane:
  Volume API -> Volume Manager -> Placement Planner -> Metadata Store
                               -> Snapshot Manager -> Backup/Object Store
                               -> Health/Rebalancer -> Repair Workers
  ```

  *** Explain the blocks
  Volume Agent exposes the attached block device to the guest and caches a signed volume mapping. Storage Frontend terminates authenticated I/O, enforces limits, and routes requests. Replication Group Leader orders writes for a volume shard. Replica Storage Nodes persist write-ahead log records and block extents on SSD/NVMe. Volume Manager owns lifecycle state. Placement Planner chooses failure-domain-aware replica sets. Metadata Store persists volume identity, shard maps, epochs, attachment leases, and encryption metadata. Snapshot Manager tracks point-in-time manifests. Health/Rebalancer and Repair Workers restore redundancy.

  *** Core components and low-level design
  Volume metadata should be small, strongly consistent, and cached with epochs. A volume can be split into fixed-size extents or shards, each assigned to a replication group. The hot I/O path should not synchronously call the global control plane; the host and frontend use cached placement maps and reject stale epochs. Writes carry volume ID, shard ID, offset, length, client request ID, checksum, and fencing epoch. The replication group appends the write to a WAL, replicates to quorum or all required replicas, applies it to extent storage, and returns success only after the durability policy is satisfied. Reads can go to the leader, a lease-valid follower, or the nearest healthy replica depending on consistency mode.

  *** Explain the control flow
  Create volume validates quota and policy, allocates shards, selects replica sets across failure domains, writes metadata, initializes encryption keys, and returns a volume ID. Attach volume obtains an attachment lease and distributes a signed map to the compute host. Resize creates new shard ranges and updates metadata through a versioned transition. Rebalancing and repair update placement by adding replicas, copying data, validating checksums, and committing a new epoch.

  *** Explain the data flow
  A guest write enters the host block layer, flows through the Volume Agent to the Storage Frontend, reaches the shard leader, is logged and replicated, then acknowledged. A read resolves the shard from the cached map, fetches from a healthy replica, validates checksums, and may trigger read repair on mismatch.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Replication protocol: primary-backup vs quorum vs consensus per shard
  Primary-backup is simple and fast but needs careful fencing during failover. Quorum replication gives better failure tolerance but complicates read repair and version reconciliation. Full consensus per shard gives clean leadership and membership changes, but adds latency and operational complexity. Prefer consensus for metadata and membership, with a tightly optimized leader-based data path for ordered writes.

  *** Data-plane latency vs durability
  Synchronous cross-zone writes increase durability but can put network latency directly in the write path. Local synchronous replication plus asynchronous remote copy improves latency but leaves a bounded exposure window. A strong answer states the default durability tier, offers higher-durability classes for databases, and makes the tradeoff explicit in the API and SLO.

  *** Failure handling
  Disk failure triggers replica rebuild from healthy copies. Storage-node failure fences the node, elects or confirms leaders, and starts repair. Compute-host failure releases attachment leases after timeout. Metadata-store unavailability should not stop already attached volumes from serving I/O until cached leases expire, but it should block unsafe lifecycle changes.

## 22. Design Replication And Metadata For Elastic Disk

* Question
  Design the replication and metadata subsystem that powers Elastic Disk, with attention to correctness, membership changes, and failover.

* Answer
  **Scope**
  Focus on volume metadata, shard placement, write ordering, replica membership, epochs, leader election, recovery, and metadata checksums. Exclude user-facing billing and high-level compute scheduling.

  **Functional Requirements**
  Maintain authoritative volume state, map blocks to shard groups, elect leaders, replicate writes, fence stale writers, add/remove replicas, recover after crashes, and expose enough state for repair and incident debugging.

  **Non Functional Requirements**
  Linearizable metadata transitions, no acknowledged write loss under the stated failure model, bounded failover, low overhead on the I/O path, corruption detection, deterministic recovery, and operability at large fleet scale.

  **High level design and diagram (at block level)**
  ```text
  Volume Manager -> Metadata Consensus Store
         |
         v
  Placement Map: volume -> shards -> replica groups -> epochs
         |
         v
  Shard Replication Group
     Leader -> Follower A
            -> Follower B
            -> Witness / Metadata Observer
  ```

  *** Explain the blocks
  Metadata Consensus Store stores volume state and membership changes. Placement Map is the cached runtime view consumed by hosts and frontends. Shard Replication Group owns write ordering for a shard. Leader accepts writes for the current epoch. Followers persist WAL entries and block data. Witness or observer can participate in leader election or durability decisions without storing the full data payload.

  *** Core components and low-level design
  Model every shard with `epoch`, `leader`, `members`, `committed_index`, `durability_policy`, and `checksum_root`. Membership changes are two-phase: add a catching-up replica as non-voting, stream a checkpoint plus WAL tail, verify checksums, then promote under a new epoch. Every I/O request includes the epoch observed by the sender. Replicas reject stale epochs and duplicate request IDs. The leader maintains per-replica match indexes and only advances commit when the durability rule is met. Recovery replays the WAL, validates metadata checksums, then reconciles committed indexes before serving writes.

  *** Explain the control flow
  Metadata updates are serialized through consensus. A failover candidate proves it has the latest committed log or obtains it from a quorum before becoming leader. Placement changes are published as signed versioned maps. Hosts and frontends refresh maps in the background and are forced to refresh on stale-epoch errors.

  *** Explain the data flow
  Write data flows from frontend to shard leader, then to followers in parallel. Commit acknowledgments flow back to the leader, then to the client. Metadata changes flow through the consensus store and eventually to caches; data writes never depend on a synchronous global metadata lookup during the common path.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Keeping replication off the critical latency path
  The hard problem is that durability requires coordination, but every extra hop hurts tail latency. Pipeline replication, batch fsync, kernel-bypass networking, per-core queues, and preallocated WAL segments can reduce overhead. Keep the global metadata store out of the hot path; use local leaders and cached placement maps.

  *** Split brain and stale attachments
  Without fencing, a restarted host or old leader can keep writing after ownership changed. Use monotonically increasing epochs, attachment leases, signed maps, and storage-node epoch checks. On failover, old leaders must be fenced before new writes are accepted.

  *** Consensus granularity
  One consensus group per volume is simple but may not scale for large volumes. One group per shard scales but increases metadata and recovery complexity. Prefer shard-level groups for large or high-throughput volumes, with small volumes packed carefully to avoid too many tiny consensus groups.

## 23. Design Point-In-Time Snapshots And Volume Restore

* Question
  Design point-in-time backup, incremental snapshots, and fast restore for Elastic Disk volumes.

* Answer
  **Scope**
  Support crash-consistent snapshots, optional application-consistent coordination, incremental block tracking, restore into new volumes, deletion and retention, encryption, and cross-zone or cross-region copy. Exclude full database-aware backup semantics unless asked.

  **Functional Requirements**
  Create a snapshot at a consistent volume epoch, track changed blocks, store immutable snapshot data and manifests, list snapshots, restore a volume, copy snapshots, enforce retention, and verify backup integrity.

  **Non Functional Requirements**
  Low impact on foreground I/O, durable snapshot storage, fast recovery for recent snapshots, cost efficiency through incrementality, auditable lifecycle operations, encryption isolation, and predictable restore performance.

  **High level design and diagram (at block level)**
  ```text
  Snapshot API -> Snapshot Coordinator -> Metadata Store
                               |
                               v
  Volume Shards -> Changed Block Map -> Snapshot Upload Workers
                               |
                               v
                   Snapshot Manifest + Backup/Object Store
                               |
                               v
                    Lazy Restore / Hydration Workers
  ```

  *** Explain the blocks
  Snapshot Coordinator creates a consistent snapshot barrier. Changed Block Map tracks dirty extents since the previous snapshot. Upload Workers copy immutable block versions to backup storage. Snapshot Manifest records block references, checksums, encryption key IDs, parent snapshot, and volume geometry. Lazy Restore serves reads from snapshot storage while Hydration Workers rebuild local replicas in the background.

  *** Core components and low-level design
  Use redirect-on-write or copy-on-write at the extent layer. Redirect-on-write writes new data elsewhere after the snapshot barrier, which keeps snapshot data immutable but can fragment active volumes. Copy-on-write preserves the active layout but adds write amplification at first overwrite. Each snapshot manifest should be content-addressed or checksum-protected. Restored volumes start with a manifest pointer and a hydration bitmap; reads for unhydrated blocks fetch from backup storage, validate checksums, then populate local replicas.

  *** Explain the control flow
  Snapshot request validates permissions and volume state, records a snapshot intent, establishes a barrier at a shard epoch or log index, freezes the changed-block view, and returns once the manifest is durable. Data upload can continue asynchronously if the snapshot is marked `creating` until all referenced blocks are safe in backup storage. Deletion removes manifests only after reference counts prove no child snapshot needs the blocks.

  *** Explain the data flow
  Foreground writes continue to the active volume. Snapshot workers read stable block versions, upload them with checksums, and update progress. Restore creates a new volume from a manifest, serves immediate reads through lazy fetch, and hydrates replicas until the volume becomes fully local.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Crash-consistent vs application-consistent snapshots
  Crash-consistent snapshots are storage-layer only and easy to provide, but databases may need recovery logs. Application-consistent snapshots require guest agents or filesystem quiesce hooks, which are operationally fragile. Default to crash-consistent snapshots and offer opt-in quiesce hooks with timeout and rollback.

  *** Lazy restore vs eager restore
  Lazy restore gives fast time-to-attach but creates unpredictable first-read latency. Eager restore gives predictable performance but delays recovery. Prefer lazy restore with prefetch for hot ranges, progress visibility, and a way to promote a volume to fully hydrated before critical workloads start.

  *** Snapshot correctness under concurrent writes
  The invariant is that every block in the manifest resolves to the value visible at the snapshot barrier. Use per-shard log indexes, changed-block bitmaps, and immutable block references. Test with randomized writes, crashes, and restores that compare volume hashes at the barrier.

## 24. Design Continuous Scrubbing And Automated Repair

* Question
  Design continuous scrubbing, silent-corruption detection, and automated repair for Elastic Disk replicas.

* Answer
  **Scope**
  Detect and repair corrupted blocks, stale replicas, missing extents, metadata mismatches, and under-replicated shards. Include prioritization, throttling, and customer-impact controls. Exclude physical disk firmware diagnostics except as an input signal.

  **Functional Requirements**
  Store checksums, scan replicas, compare data and metadata, detect corruption, quarantine bad copies, repair from healthy replicas or snapshots, rebuild lost replicas, track risk, and report volume health.

  **Non Functional Requirements**
  Very low foreground I/O impact, high detection coverage, bounded repair time for risky states, no repair from corrupted sources, clear operator visibility, and safe automation with manual override.

  **High level design and diagram (at block level)**
  ```text
  Replica Storage Nodes -> Local Scrubbers -> Scrub Result Stream
                                      |
                                      v
  Health Aggregator -> Risk Scorer -> Repair Scheduler -> Repair Workers
                                      |
                                      v
                         Metadata Checksum Verifier
                                      |
                                      v
                          Volume Health / Alerts / SLOs
  ```

  *** Explain the blocks
  Local Scrubbers read blocks and verify checksums. Scrub Result Stream decouples scanning from fleet decisions. Health Aggregator groups findings by volume, shard, disk, host, rack, and software version. Risk Scorer prioritizes repairs that threaten durability. Repair Scheduler throttles work. Repair Workers copy from healthy sources, reconstruct from parity if available, or restore from snapshots. Metadata Checksum Verifier catches placement-map and replica-state corruption.

  *** Core components and low-level design
  Store checksums at multiple layers: per-block data checksum, WAL record checksum, extent manifest checksum, and replication metadata checksum. A scrubber must record the exact version it scanned; otherwise it can confuse active writes with corruption. Repair should require a quorum or trusted source selection before overwriting a replica. A quarantined replica is removed from reads, but retained for forensic comparison until safe deletion. Repair work uses token buckets per host and volume to protect foreground latency.

  *** Explain the control flow
  Scrub policy defines scan cadence by risk class. Findings create repair intents. Repair Scheduler validates current metadata epoch, chooses a source, reserves bandwidth, copies data, verifies checksums, updates replica state, and closes the intent. Repeated failures escalate to host or disk drain.

  *** Explain the data flow
  Scrub reads flow locally on storage nodes. Results flow to the health pipeline. Repair data flows from healthy replica or backup storage to the replacement replica, then validation metadata flows back to the Metadata Store.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Silent corruption vs availability
  Serving from a corrupt replica preserves apparent availability but violates durability and correctness. Immediately quarantine confirmed corrupt blocks for reads, repair from a healthy copy, and use read-time checksum validation on latency-sensitive paths where the cost is acceptable.

  *** Repair prioritization
  First repair shards below durability target, then shards with correlated risk, then cold low-risk replicas. Use signals such as replica count, failure-domain diversity, age of last scrub, media error rate, and snapshot coverage.

  *** Proving repair safety
  The key invariant is never overwrite the last known-good copy. Enforce source validation, epoch checks, idempotent repair intents, and rollback on checksum mismatch. Use fault injection where corrupt replicas lie, disappear, or return stale versions.

## 25. Design Multi-Tenant I/O QoS And Noisy-Neighbor Control

* Question
  Design I/O quality-of-service for Elastic Disk so many tenants and workloads can share storage nodes without violating latency and throughput commitments.

* Answer
  **Scope**
  Support per-volume performance classes, burst credits, tenant quotas, admission control, scheduling, overload handling, and observability. Exclude customer billing implementation except for usage signals.

  **Functional Requirements**
  Assign IOPS and throughput limits, enforce per-volume and per-tenant budgets, isolate noisy neighbors, support bursts, prioritize repair and snapshot work safely, and expose metrics for saturation and throttling.

  **Non Functional Requirements**
  Predictable tail latency, fairness, low scheduler overhead, work-conserving behavior when spare capacity exists, blast-radius control, and debuggable enforcement decisions.

  **High level design and diagram (at block level)**
  ```text
  I/O Request -> Frontend Classifier -> Tenant / Volume Token Buckets
                                      -> Priority Queues
                                      -> Storage Node Scheduler
                                      -> Device / Network

  Telemetry -> QoS Controller -> Limit Config / Hotspot Mitigation
  ```

  *** Explain the blocks
  Frontend Classifier labels requests by tenant, volume, operation, priority, and source. Token Buckets enforce provisioned limits and bursts. Priority Queues separate foreground reads/writes from background repair/snapshot traffic. Storage Node Scheduler maps queued work to device and network resources. QoS Controller consumes telemetry and adjusts placement, limits, or rebalancing.

  *** Core components and low-level design
  Use hierarchical controls: tenant budget, volume budget, shard budget, and node admission budget. Reads and writes may need different queues because writes consume replication bandwidth and fsync resources. Background work should be preemptible and budgeted. Track queue depth, service time, throttle reason, replica lag, and device utilization. Hot volumes can be split, moved, replicated for read scaling, or upgraded to a higher performance class.

  *** Explain the control flow
  Volume creation sets a performance policy. Runtime controllers detect hot shards or tenants approaching limits. Mitigations include throttling, moving shards, adding read replicas, adjusting burst credits, or asking compute placement to avoid colocating hot volumes on the same storage fleet.

  *** Explain the data flow
  Requests are classified, admitted if tokens are available, queued by priority, dispatched to storage devices, and measured. Throttled requests return retryable backpressure or are delayed locally depending on protocol semantics.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Strict limits vs work-conserving fairness
  Strict limits protect neighbors but waste spare capacity. Work-conserving scheduling improves utilization but can surprise users when spare capacity disappears. Prefer guaranteed baselines plus bounded burst credits, with transparent metrics for throttling and burst exhaustion.

  *** Tail latency vs throughput
  Large sequential writes improve throughput but can block small random reads. Use separate queues, request size normalization, deadline-aware scheduling, and background-work throttles. For critical databases, offer a performance class with stricter placement and lower oversubscription.

  *** Noisy-neighbor diagnosis
  The user-visible symptom may be latency, but the root cause could be network, device saturation, replication lag, or a metadata hotspot. Preserve high-cardinality internal traces while exposing simple volume-level metrics to customers and operators.

## 26. Design A Cross-Org Replication Foundation For Storage Backends

* Question
  Design a shared replication foundation that multiple Apple storage backends can use without forcing every system into the same data model.

* Answer
  **Scope**
  Provide reusable replication, membership, durability, checksumming, repair, and observability primitives for block storage, object storage metadata, distributed databases, and storage engines. Exclude rewriting every storage backend at once.

  **Functional Requirements**
  Offer a common library or service for replication groups, epochs, leader election, membership changes, WAL abstractions, checksums, repair hooks, metrics, and rollout controls. Support backend-specific data formats and durability policies.

  **Non Functional Requirements**
  Correctness, performance, backward compatibility, incremental adoption, debuggability, clear ownership, low integration friction, and safe cross-org governance.

  **High level design and diagram (at block level)**
  ```text
  Storage Backend A -> Replication SDK -> Replication Runtime
  Storage Backend B -> Replication SDK -> Replication Runtime
  Storage Backend C -> Replication SDK -> Replication Runtime

  Shared Control:
  Membership API -> Policy Store -> Rollout Controller -> Observability
                         |
                         v
                  Repair / Scrub Framework
  ```

  *** Explain the blocks
  Replication SDK exposes a narrow API for append, commit, snapshot, membership, and recovery callbacks. Replication Runtime implements protocol mechanics. Membership API manages groups and epochs. Policy Store defines durability classes and failure-domain rules. Rollout Controller gates adoption and protocol upgrades. Observability standardizes metrics, traces, and incident evidence. Repair/Scrub Framework integrates backend-specific validation with shared scheduling.

  *** Core components and low-level design
  Keep the shared layer below product semantics but above raw networking. The API should let each backend define record encoding, state-machine apply, snapshot format, and read semantics. The foundation owns epochs, membership transitions, log replication, fencing, checksummed transport, and protocol metrics. Version every protocol message and support mixed-version clusters during rollout. Provide a simulator and deterministic fault-injection harness so adopters can test their integration.

  *** Explain the control flow
  A backend registers replication groups and durability policies. The shared control plane creates groups, rolls out protocol versions, and publishes health. During migration, a backend can dual-run old and new replication paths, compare checksums and commit indexes, then flip traffic gradually.

  *** Explain the data flow
  Backend writes become replication records. The SDK sends records to the runtime, which orders, replicates, commits, and calls back into the backend's apply function. Health, lag, checksum, and repair events flow to shared observability while backend-specific payloads remain encapsulated.

  **Deep dive topics and questions -> Explain the problem and suggest solutions**
  *** Shared foundation vs bespoke protocols
  A shared foundation reduces duplicated correctness work and standardizes operations, but can become a lowest-common-denominator bottleneck. Bespoke protocols optimize for each backend but multiply incident patterns and expertise requirements. Prefer shared protocol primitives with backend-specific apply and storage layers.

  *** Migration strategy
  Big-bang migration is too risky for storage. Use shadow replication, checksum comparison, read-only canaries, per-volume or per-shard opt-in, automatic rollback, and a compatibility window where old and new replicas can coexist.

  *** Staff/Principal ownership
  The technical problem is only half the interview. Explain how you would set the API boundary, write the design docs, align partner teams, define adoption metrics, own incident response, and retire legacy paths without leaving teams stranded.
