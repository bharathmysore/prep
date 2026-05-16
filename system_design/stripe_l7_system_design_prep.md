# Stripe L7 System Design Interview Prep

These are representative Stripe-style system design questions, not confirmed current interview prompts. They focus on cloud, distributed systems, payments correctness, APIs, reliability, and operational design.

Maintenance note: treat this as a living company-specific catalog. When adding, removing, or reordering prompts, update the numbered question sections and the question index; derive the count from the `## N.` headings instead of maintaining a fixed count in the title or filename.

## Common L7 Stripe Answer Pattern

For almost every prompt, a strong answer should mention:

- Explicit state machines for long-running payment workflows.
- Idempotency for every mutating API and retryable background activity.
- Immutable financial records, usually modeled as double-entry ledger entries.
- Async processing for external dependencies such as processors, banks, merchants, and vendors.
- Reconciliation because external systems are not perfectly reliable.
- Clear operational contracts: observability, auditability, privacy, access control, and recovery.
- Product-facing semantics that are honest, such as "at least once with deduplication" instead of impossible exactly-once claims across third parties.

## 1. Design a Payment Processing System

* **Question**
  Design a payment processing system that accepts customer payments for merchants, supports authorization, capture, failure handling, refunds, webhooks, and reconciliation.

* **Answer**
  Model payment as a durable object with an explicit state machine. A `PaymentIntent` or equivalent object represents the merchant's intent to collect money. The system should separate API request handling, orchestration, ledger posting, processor communication, webhooks, and reconciliation.

**Scope**

- Card payments first; extension points for ACH, wallets, and bank redirects.
- Single merchant charge flow, with support for auth/capture and async confirmation.
- Include correctness and recovery, not just the happy path.

**Functional Requirements**

- Create, confirm, authorize, capture, cancel, refund, and query payments.
- Support idempotent client retries.
- Integrate with external payment processors.
- Emit webhooks for state changes.
- Record ledger entries and enable reconciliation.

**Non Functional Requirements**

- No duplicate charges under client, server, or network retries.
- High availability for API reads and payment creation.
- Strong consistency for money movement records.
- Auditable state transitions.
- Secure handling of payment credentials.

**High level design and diagram (at block level)**

```text
Client / Merchant
    |
    v
API Gateway -> Auth -> Idempotency Store
    |
    v
Payment Service -> Payment DB
    |
    v
Payment Orchestrator
    |        |          |           |
    v        v          v           v
Fraud    Ledger     Processor    Event Bus
Service  Service    Adapter      |
                         |       v
                         v   Webhook Service
                   External Processor
```

***Explain the blocks***

- **API Gateway/Auth** validates merchant identity, API keys, permissions, and rate limits.
- **Idempotency Store** maps idempotency keys to request hashes and responses.
- **Payment Service** owns payment objects and state transitions.
- **Orchestrator** coordinates fraud checks, ledger writes, processor calls, and events.
- **Ledger Service** records immutable financial entries.
- **Processor Adapter** normalizes integrations with card networks and processors.
- **Event Bus/Webhook Service** publishes state changes to merchants.

***Explain the control flow***

- Merchants configure payment methods, capture behavior, risk settings, webhook endpoints, and API keys.
- Operators configure processor routing, retry policies, feature flags, risk thresholds, and regional failover.
- Config changes are versioned, audited, and cached by runtime services.

***Explain the data flow***

- Merchant creates a payment with an idempotency key.
- API validates and stores the initial payment object.
- Orchestrator runs fraud checks, posts pending ledger records, and calls the processor.
- Processor response updates payment state.
- Events are emitted to webhooks and analytics.
- Settlement files later reconcile processor results against internal ledger records.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Delivery semantics and duplicate charges***

The core problem is that clients, internal services, and processors all retry. A timeout does not tell us whether the charge happened.

- **At most once**
  - Pros: low duplicate risk, simpler retry behavior.
  - Cons: lost payments when failures happen after request acceptance.
- **At least once plus idempotency**
  - Pros: practical, reliable, works with retries.
  - Cons: requires idempotency stores, request hashing, and careful state transitions.
- **Exactly once end to end**
  - Pros: ideal product contract.
  - Cons: not achievable across clients, networks, and third-party processors.

Recommended: use at-least-once internally with idempotency keys and processor-side idempotency where available.

***Sync vs async confirmation***

- **Synchronous confirmation**
  - Pros: simple client experience.
  - Cons: couples UX to processor latency and availability.
- **Asynchronous state machine**
  - Pros: resilient, honest about pending states.
  - Cons: clients must handle intermediate states.

Recommended: return durable payment state immediately, then update via polling and webhooks.

## 2. Design an Idempotency Service

* **Question**
  Design an idempotency system for Stripe-like APIs so merchants can safely retry mutating requests.

* **Answer**
  Every mutating request accepts an idempotency key. The service stores key, merchant, endpoint, request hash, execution state, response, and expiry. Retries with the same key and same request return the same result; retries with different parameters are rejected.

**Scope**

- Covers API-level idempotency for create, confirm, refund, payout, and subscription mutations.
- Does not replace lower-level dedupe in queues or ledger writes.

**Functional Requirements**

- Accept client-provided idempotency keys.
- Detect duplicate requests.
- Return the original response for identical retries.
- Reject parameter mismatches.
- Expire old keys safely.

**Non Functional Requirements**

- Strong consistency per merchant/key.
- Low latency on the API hot path.
- High availability.
- Auditability for debugging payment disputes.

**High level design and diagram (at block level)**

```text
Client
  |
  v
API Gateway
  |
  v
Idempotency Middleware
  |              |
  v              v
Idempotency DB   Request Executor
  |              |
  +<-------------+
        stores final response
```

***Explain the blocks***

- **Middleware** computes request hash and checks merchant/key uniqueness.
- **Idempotency DB** stores in-progress and completed request records.
- **Request Executor** runs the underlying business operation once.
- **Response Cache** is usually part of the idempotency record.

***Explain the control flow***

- API owners define which endpoints require idempotency.
- Retention policy defines how long keys are kept.
- Operators monitor key collision rates, lock contention, and stale in-progress requests.

***Explain the data flow***

- First request inserts `merchant_id + idempotency_key` with `IN_PROGRESS`.
- Business operation runs.
- Final response and object reference are stored.
- Retry checks the record and returns the saved response.
- If the first attempt crashed, recovery inspects business object state before retrying.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Where to store idempotency records***

- **Relational DB with unique constraint**
  - Pros: strong semantics, simple correctness story.
  - Cons: can become hot for very high-volume merchants.
- **Distributed KV store**
  - Pros: high scale, low latency.
  - Cons: consistency and transaction semantics need careful design.
- **Local cache only**
  - Pros: fastest.
  - Cons: unsafe across retries, restarts, and regions.

Recommended: strongly consistent persistent store, optionally fronted by cache.

***Handling in-progress retries***

- **Block until first request finishes**
  - Pros: clean client result.
  - Cons: ties up connections.
- **Return conflict/in-progress**
  - Pros: protects system resources.
  - Cons: client must retry later.
- **Attach retry to same workflow**
  - Pros: elegant for long-running operations.
  - Cons: more implementation complexity.

Recommended: short wait for fast operations; otherwise return a retriable in-progress response.

## 3. Design a Double-Entry Ledger

* **Question**
  Design a ledger that records all money movement for payments, refunds, fees, disputes, payouts, and balance updates.

* **Answer**
  Use append-only double-entry accounting. Every financial transaction posts balanced debit and credit entries. Mutable product objects like payments and payouts reference immutable ledger transactions.

**Scope**

- Ledger for internal financial truth.
- Supports multiple currencies and accounts.
- Excludes full general ledger accounting reports, but design should support them later.

**Functional Requirements**

- Post balanced transactions.
- Query merchant balances.
- Support pending, available, reserved, and paid-out funds.
- Link entries to business objects.
- Support reversals, not destructive edits.

**Non Functional Requirements**

- Strong consistency for postings.
- Immutability and auditability.
- High read scale for balances.
- Reconciliation-friendly data model.

**High level design and diagram (at block level)**

```text
Payment / Refund / Payout Services
        |
        v
Ledger API
        |
        v
Posting Validator
        |
        v
Ledger Transaction Store
        |
        +--> Ledger Entries
        +--> Balance Projections
        +--> Audit Log
```

***Explain the blocks***

- **Ledger API** accepts posting requests from payment-domain services.
- **Validator** enforces balanced debits and credits, currency rules, and idempotency.
- **Transaction Store** persists immutable transaction headers.
- **Entries** store account, amount, currency, debit/credit direction, and references.
- **Balance Projections** provide fast reads derived from immutable entries.

***Explain the control flow***

- Finance and platform teams define account types, posting templates, and reversal rules.
- Schema and accounting rule changes are versioned and reviewed.
- Access to manual adjustments is restricted and audited.

***Explain the data flow***

- Payment capture requests a ledger posting.
- Ledger validates that debits equal credits per currency.
- Entries are committed atomically.
- Balance projections update synchronously or through a durable stream.
- Reconciliation compares entries against external settlement records.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Balance computation***

- **Compute from ledger entries on every read**
  - Pros: always derived from source of truth.
  - Cons: too slow at scale.
- **Maintain balance projection table**
  - Pros: fast reads.
  - Cons: projection bugs can show wrong balances.
- **Hybrid: projection plus periodic rebuild/check**
  - Pros: fast and auditable.
  - Cons: more operational complexity.

Recommended: immutable entries as source of truth, with rebuildable projections.

***Corrections***

- **Update bad entries**
  - Pros: simple database operation.
  - Cons: destroys audit trail.
- **Post reversal and corrected transaction**
  - Pros: auditable and accounting-friendly.
  - Cons: more records and harder queries.

Recommended: never mutate posted ledger entries; use reversals.

## 4. Design Webhook Delivery

* **Question**
  Design a webhook delivery system that notifies merchants about payment, refund, dispute, payout, and subscription events.

* **Answer**
  Build an event delivery pipeline with durable event storage, endpoint subscriptions, signing, delivery workers, retry policy, delivery logs, and merchant-facing observability. Guarantee at-least-once delivery with event IDs for dedupe.

**Scope**

- Merchant webhooks over HTTPS.
- Supports retries, endpoint secrets, event filtering, and delivery history.
- Does not require exactly-once delivery to merchant systems.

**Functional Requirements**

- Register webhook endpoints.
- Select event types per endpoint.
- Sign payloads.
- Retry failed deliveries.
- Expose delivery logs and manual replay.

**Non Functional Requirements**

- At-least-once delivery.
- High throughput and backpressure handling.
- Isolation between merchants.
- Secure event signing.

**High level design and diagram (at block level)**

```text
Domain Services
    |
    v
Event Store -> Event Router -> Delivery Queue
                              |
                              v
                        Delivery Workers
                              |
                              v
                      Merchant Endpoints
                              |
                              v
                        Delivery Logs
```

***Explain the blocks***

- **Event Store** records immutable domain events.
- **Event Router** resolves subscribed endpoints and event filters.
- **Delivery Queue** buffers delivery attempts.
- **Workers** sign payloads and call merchant endpoints.
- **Delivery Logs** store status, response code, latency, and retry history.

***Explain the control flow***

- Merchants configure endpoint URL, subscribed event types, and signing secret.
- Operators configure retry schedule, timeouts, max attempts, and abuse controls.
- Endpoint secrets can be rotated with overlapping validity windows.

***Explain the data flow***

- Payment state change emits an event.
- Router creates one delivery task per matching endpoint.
- Worker signs and posts payload.
- Success marks delivery complete.
- Failure schedules retry with exponential backoff and jitter.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Delivery guarantee***

- **At most once**
  - Pros: no duplicate webhook calls.
  - Cons: lost events when merchant endpoint or network fails.
- **At least once**
  - Pros: reliable and practical.
  - Cons: merchants must dedupe.
- **Exactly once**
  - Pros: ideal developer experience.
  - Cons: impossible across merchant-owned HTTP endpoints.

Recommended: at-least-once with stable event IDs, signatures, replay, and docs.

***Ordering***

- **Global ordering**
  - Pros: easiest for consumers to reason about.
  - Cons: severe bottleneck.
- **Per-object ordering**
  - Pros: useful and scalable.
  - Cons: still requires partitioning and blocked queues.
- **No ordering guarantee**
  - Pros: highest throughput.
  - Cons: merchant integrations are harder.

Recommended: per-object sequence numbers where feasible, with current-state fetch APIs.

## 5. Design Refund Processing

* **Question**
  Design a refund system that supports full and partial refunds, async processor confirmation, ledger reversals, and merchant-visible refund state.

* **Answer**
  Treat refunds as first-class objects with their own state machine. A refund references the original payment, validates refundable amount, posts ledger reversals or pending entries, calls the processor, and emits events.

**Scope**

- Card refunds for captured payments.
- Partial and multiple refunds.
- Excludes chargeback handling.

**Functional Requirements**

- Create full or partial refunds.
- Validate remaining refundable amount.
- Handle pending, succeeded, failed, and canceled states.
- Emit refund webhooks.
- Update balances and ledger.

**Non Functional Requirements**

- Prevent over-refunds.
- Idempotent retries.
- Auditable financial history.
- Resilient to processor timeouts.

**High level design and diagram (at block level)**

```text
Merchant API
   |
   v
Refund Service -> Payment DB
   |
   +--> Idempotency Store
   +--> Ledger Service
   +--> Processor Adapter
   +--> Event Bus/Webhooks
```

***Explain the blocks***

- **Refund Service** owns refund creation and state transitions.
- **Payment DB** stores original payment and refundable balance.
- **Ledger Service** records reversal or merchant balance impact.
- **Processor Adapter** submits refund to external processor.
- **Event Bus** informs merchant systems.

***Explain the control flow***

- Merchant config may define refund permissions, automatic refund rules, and reserve policies.
- Operators configure processor-specific refund behavior and retry policies.
- Risk systems may block suspicious refund patterns.

***Explain the data flow***

- Merchant requests refund with amount and idempotency key.
- Service locks or atomically updates refundable amount.
- Refund object is created.
- Ledger entries are posted.
- Processor refund request is sent.
- Final state is updated from processor response or later webhook/file.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Preventing over-refunds***

- **Read then write remaining amount**
  - Pros: simple.
  - Cons: unsafe under concurrent refund requests.
- **Database lock or serializable transaction**
  - Pros: strong correctness.
  - Cons: contention on popular payments.
- **Ledger-derived available-to-refund amount**
  - Pros: auditable.
  - Cons: more complex query path.

Recommended: atomic reservation of refundable amount plus ledger-backed checks.

***When to update merchant balance***

- **Immediately on refund creation**
  - Pros: conservative risk posture.
  - Cons: merchant balance changes before processor confirmation.
- **After refund success**
  - Pros: reflects actual external result.
  - Cons: merchant could withdraw funds before refund clears.
- **Pending hold then finalize**
  - Pros: balances risk and accuracy.
  - Cons: more state complexity.

Recommended: place pending hold/reserve, finalize after success.

## 6. Design Chargeback / Dispute Management

* **Question**
  Design a system for handling card disputes and chargebacks, including evidence submission, deadlines, balance impact, and state updates.

* **Answer**
  Disputes are long-running regulated workflows. Represent each dispute as a durable state machine linked to payment, merchant, card network, evidence, deadlines, balance movements, and final outcome.

**Scope**

- Card disputes after successful payment.
- Evidence collection and submission.
- Balance debits, reversals, and outcomes.

**Functional Requirements**

- Ingest dispute notifications.
- Notify merchants.
- Collect evidence.
- Track deadlines.
- Submit evidence to processor/network.
- Apply ledger movements for won/lost disputes.

**Non Functional Requirements**

- Deadline reliability.
- Auditability.
- Secure document handling.
- Correct balance impact.

**High level design and diagram (at block level)**

```text
Processor Dispute Feed
        |
        v
Dispute Ingestion -> Dispute Service -> Dispute DB
                         |
        +----------------+----------------+
        v                v                v
   Evidence Store   Ledger Service   Notification/Webhooks
        |
        v
Processor Evidence Submission
```

***Explain the blocks***

- **Ingestion** receives dispute events from processors.
- **Dispute Service** owns state, deadlines, and merchant workflow.
- **Evidence Store** secures files and structured evidence.
- **Ledger Service** debits or releases funds.
- **Notifications/Webhooks** inform merchants.

***Explain the control flow***

- Dispute rules depend on network, country, reason code, and deadline.
- Operators configure reason-code mappings and evidence templates.
- Merchant dashboards guide required evidence without exposing internal complexity.

***Explain the data flow***

- Processor sends dispute notice.
- System creates dispute and debits/holds merchant balance.
- Merchant uploads evidence.
- Evidence is submitted before deadline.
- Outcome event updates dispute state and posts final ledger entries.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Deadline handling***

- **Cron scans database**
  - Pros: simple.
  - Cons: can miss or delay time-sensitive actions under load.
- **Durable workflow timers**
  - Pros: reliable per-dispute scheduling.
  - Cons: requires workflow engine maturity.
- **Queue with delayed messages**
  - Pros: scalable.
  - Cons: long delays and reprocessing can be tricky.

Recommended: workflow timers for deadlines plus periodic sweeper as backup.

***Balance impact***

- **Debit immediately**
  - Pros: protects platform from loss.
  - Cons: painful merchant UX for disputes they may win.
- **Hold reserve**
  - Pros: clearer pending state.
  - Cons: needs reserve accounting.
- **Wait until outcome**
  - Pros: merchant-friendly.
  - Cons: platform assumes credit risk.

Recommended: hold or debit based on risk tier and network rules.

## 7. Design a Payment Method Vault

* **Question**
  Design a vault for storing and using card or bank payment methods securely.

* **Answer**
  Isolate sensitive payment credentials into a tightly scoped vault service. The rest of the platform uses opaque tokens. The vault handles encryption, tokenization, key rotation, access control, and secure processor transmission.

**Scope**

- Card storage and tokenization.
- Opaque payment method IDs for platform services.
- PCI-sensitive data isolation.

**Functional Requirements**

- Store encrypted payment credentials.
- Return opaque tokens.
- Support payment method reuse.
- Rotate keys.
- Delete or detach payment methods.

**Non Functional Requirements**

- Strong security isolation.
- Minimal data exposure.
- Auditable access.
- Low latency token lookup for payment flows.

**High level design and diagram (at block level)**

```text
Client Tokenization Flow
       |
       v
Vault API -> AuthZ -> Encryption/KMS/HSM
       |
       v
Secure Credential Store
       |
       v
Opaque Token returned to Platform
```

***Explain the blocks***

- **Vault API** is the only service allowed to receive raw credentials.
- **AuthZ** limits which services can tokenize, read, or transmit credentials.
- **KMS/HSM** protects encryption keys.
- **Credential Store** stores encrypted sensitive data.
- **Opaque Token** is safe for normal platform services to reference.

***Explain the control flow***

- Security team defines access policy, key rotation, retention, and audit requirements.
- Operators manage emergency credential lockdown and regional key availability.
- Compliance controls separate vault deploys and access from general application systems.

***Explain the data flow***

- Client submits card details through secure tokenization path.
- Vault encrypts and stores credential.
- Vault returns opaque payment method ID.
- Payment service later asks vault to transmit credential to processor or produce processor token.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Tokenization model***

- **Store raw encrypted card and send when needed**
  - Pros: flexible processor routing.
  - Cons: larger PCI scope and higher breach impact.
- **Processor token only**
  - Pros: smaller sensitive data footprint.
  - Cons: harder to switch processors or do multi-processor routing.
- **Network tokenization**
  - Pros: lower fraud, lifecycle updates.
  - Cons: integration complexity and network dependency.

Recommended: layered tokenization with minimal raw credential storage and strong isolation.

***Key management***

- **Application-managed keys**
  - Pros: simple to implement.
  - Cons: weak isolation.
- **KMS**
  - Pros: standard rotation, audit, access policy.
  - Cons: external dependency on every decrypt.
- **HSM**
  - Pros: highest protection.
  - Cons: cost, latency, operational complexity.

Recommended: envelope encryption with KMS/HSM depending on risk and compliance scope.

## 8. Design Checkout

* **Question**
  Design a hosted checkout product that lets merchants collect payments without building their own payment UI.

* **Answer**
  A Checkout Session encapsulates amount, currency, merchant, customer, payment methods, expiration, redirect URLs, and payment intent. Hosted checkout handles payment method collection, authentication, confirmation, and final redirect.

**Scope**

- Hosted web checkout.
- One-time payment first; extensible to subscriptions.
- Includes payment confirmation and webhooks.

**Functional Requirements**

- Create checkout sessions.
- Render hosted payment page.
- Collect payment method.
- Confirm payment and redirect user.
- Notify merchant via webhook.

**Non Functional Requirements**

- High conversion and low latency.
- Secure payment collection.
- Mobile-friendly.
- Resilient to abandoned sessions and retries.

**High level design and diagram (at block level)**

```text
Merchant Server
    |
    v
Checkout Session API -> Session Store
    |
    v
Hosted Checkout UI
    |
    v
Payment Method Collection -> Payment Service
    |
    v
Auth/3DS/Processor -> Redirect + Webhook
```

***Explain the blocks***

- **Session API** creates immutable or versioned checkout configuration.
- **Session Store** tracks expiration, status, and linked payment intent.
- **Hosted UI** renders allowed payment methods and localization.
- **Payment Service** confirms the payment.
- **Redirect/Webhook** informs customer and merchant.

***Explain the control flow***

- Merchants configure branding, allowed payment methods, tax behavior, URLs, and fraud rules.
- Product teams roll out payment method changes via feature flags and experiments.
- Runtime UI reads versioned session config to avoid changing behavior mid-session.

***Explain the data flow***

- Merchant creates a checkout session server-side.
- Customer visits hosted page.
- Customer enters payment method.
- Checkout confirms payment through payment service.
- User is redirected and merchant receives webhook.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Hosted vs embedded checkout***

- **Hosted checkout**
  - Pros: lower compliance burden, consistent UX, faster merchant integration.
  - Cons: less merchant customization.
- **Embedded components**
  - Pros: better merchant UX control.
  - Cons: more integration complexity and compliance exposure.

Recommended: support hosted first for safety and speed; offer embedded components for mature merchants.

***Session mutability***

- **Mutable session**
  - Pros: merchant can adjust cart dynamically.
  - Cons: race conditions and confusing user experience.
- **Immutable finalized session**
  - Pros: predictable and auditable.
  - Cons: merchant must create a new session for changes.

Recommended: allow draft-like updates before user starts payment, then freeze critical fields.

## 9. Design a Fraud Detection System

* **Question**
  Design a fraud detection system for real-time payment authorization and asynchronous risk review.

* **Answer**
  Use a layered risk system. A fast synchronous path makes allow, block, or challenge decisions before authorization. An asynchronous path performs deeper analysis and may trigger review, reserves, or later action.

**Scope**

- Card-not-present payment fraud.
- Real-time risk decision in checkout.
- Async review and feedback loops.

**Functional Requirements**

- Score payments.
- Apply rules and ML models.
- Support challenge, block, allow, review.
- Track fraud outcomes and chargebacks.
- Provide merchant-level controls.

**Non Functional Requirements**

- Very low latency on checkout path.
- High availability.
- Explainability for risk decisions.
- Privacy and secure feature handling.

**High level design and diagram (at block level)**

```text
Payment Request
    |
    v
Feature Fetcher -> Risk Rules + ML Scoring
    |
    v
Decision Engine
    |
    +--> Allow/Block/Challenge
    |
    v
Async Review Pipeline -> Model Training/Feedback
```

***Explain the blocks***

- **Feature Fetcher** gathers velocity, device, IP, card, merchant, customer, and historical data.
- **Rules Engine** handles deterministic policy.
- **ML Scoring** estimates fraud risk.
- **Decision Engine** maps risk to action.
- **Feedback Pipeline** incorporates chargebacks and outcomes.

***Explain the control flow***

- Risk teams configure thresholds, rules, model versions, and experiment cohorts.
- Merchants may configure strictness within allowed boundaries.
- Model rollouts use shadow mode and gradual traffic exposure.

***Explain the data flow***

- Payment service sends risk request.
- Risk service fetches online features.
- Rules and models produce decision.
- Payment flow continues, challenges, or blocks.
- Outcomes stream back into analytics and training.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Latency vs model quality***

- **Synchronous rich model**
  - Pros: better fraud detection.
  - Cons: increases checkout latency and dependency risk.
- **Fast synchronous model plus async deep model**
  - Pros: protects conversion and catches more later.
  - Cons: some fraud may pass initially.
- **Async only**
  - Pros: no checkout latency.
  - Cons: misses prevention opportunity.

Recommended: fast sync decision with async enrichment and review.

***Rules vs ML***

- **Rules**
  - Pros: explainable, fast, easy to enforce policy.
  - Cons: brittle and easy to evade.
- **ML**
  - Pros: captures complex patterns.
  - Cons: harder to explain and monitor.
- **Hybrid**
  - Pros: practical balance.
  - Cons: operational complexity.

Recommended: hybrid rules and ML with versioned decisions.

## 10. Design Merchant Onboarding / KYC

* **Question**
  Design a system to onboard merchants, verify identity, collect bank accounts, enforce KYC/KYB requirements, and restrict risky accounts.

* **Answer**
  Use a workflow-driven onboarding platform. Merchant accounts progress through states based on submitted information, verification results, risk review, and regulatory requirements.

**Scope**

- Business onboarding for payments acceptance.
- Identity, business, ownership, bank account, and risk checks.
- Human review for exceptions.

**Functional Requirements**

- Collect onboarding data.
- Validate required fields by country and business type.
- Integrate with KYC vendors.
- Track verification status.
- Apply account capabilities and restrictions.

**Non Functional Requirements**

- Secure PII handling.
- Auditable decisions.
- Configurable country-specific rules.
- Reliable workflow progression.

**High level design and diagram (at block level)**

```text
Merchant Dashboard/API
        |
        v
Onboarding Service -> Requirements Engine
        |
        +--> KYC Vendor Adapters
        +--> Document Store
        +--> Risk Review Queue
        +--> Account Capability Service
```

***Explain the blocks***

- **Onboarding Service** owns merchant profile and workflow state.
- **Requirements Engine** determines needed fields and documents.
- **Vendor Adapters** call external identity and business verification providers.
- **Document Store** secures uploaded documents.
- **Capability Service** enables or restricts accepting payments and payouts.

***Explain the control flow***

- Compliance teams configure requirements by jurisdiction, business type, volume, and risk tier.
- Risk teams configure manual review triggers.
- Product teams expose missing requirements through dashboard and APIs.

***Explain the data flow***

- Merchant submits profile and documents.
- Requirements engine validates completeness.
- Vendor checks run asynchronously.
- Results update account state.
- Capabilities are enabled, limited, or disabled.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Rules engine design***

- **Hard-coded country rules**
  - Pros: quick initially.
  - Cons: unmaintainable across jurisdictions.
- **Config-driven requirements engine**
  - Pros: scalable and auditable.
  - Cons: needs strong validation and tooling.
- **External policy engine**
  - Pros: powerful and reusable.
  - Cons: operational dependency and complexity.

Recommended: versioned config-driven requirements with tests and approval workflows.

***Fast onboarding vs compliance risk***

- **Verify before enabling payments**
  - Pros: low regulatory risk.
  - Cons: conversion loss.
- **Progressive enablement**
  - Pros: better merchant activation.
  - Cons: requires volume limits and monitoring.

Recommended: progressive capabilities based on risk and legal requirements.

## 11. Design Payouts

* **Question**
  Design a payout system that transfers available merchant balances to bank accounts on schedules or on demand.

* **Answer**
  Payouts should be ledger-driven. The system computes available balances from ledger projections, applies reserves and risk holds, creates payout batches, sends instructions to banking rails, and reconciles settlement outcomes.

**Scope**

- Merchant payouts to bank accounts.
- Scheduled and manual payouts.
- Multi-currency support at a high level.

**Functional Requirements**

- Compute available balance.
- Create payouts.
- Support schedules and manual triggers.
- Send bank transfer instructions.
- Reconcile payout success/failure.

**Non Functional Requirements**

- No overpayment.
- Strong audit trail.
- Reliable batch processing.
- Secure bank account handling.

**High level design and diagram (at block level)**

```text
Scheduler / Merchant Request
        |
        v
Payout Service -> Balance Service/Ledger
        |
        +--> Risk/Reserve Service
        +--> Bank Account Vault
        +--> Banking Rail Adapter
        +--> Reconciliation
```

***Explain the blocks***

- **Payout Service** owns payout object and workflow.
- **Balance Service** exposes available funds from ledger projections.
- **Risk/Reserve** applies holds.
- **Bank Account Vault** secures destination credentials.
- **Rail Adapter** sends ACH, wire, SEPA, or local bank instructions.

***Explain the control flow***

- Merchants configure payout schedule, currency, and bank account.
- Risk config determines reserve percentages and payout delays.
- Operators configure banking rail cutoffs, holidays, and retry rules.

***Explain the data flow***

- Scheduler identifies merchants eligible for payout.
- Payout service reserves available balance atomically.
- Banking instruction is sent.
- Bank result updates payout state.
- Ledger finalizes paid-out balance or reverses failed payout.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Preventing overpayment***

- **Compute balance at payout time without reservation**
  - Pros: simple.
  - Cons: race with refunds/disputes.
- **Reserve funds before sending payout**
  - Pros: prevents double spend.
  - Cons: creates pending states and reversal needs.
- **Serialize all merchant balance mutations**
  - Pros: strongest correctness.
  - Cons: throughput bottleneck for large merchants.

Recommended: atomic reservation in ledger/balance service.

***Batch vs real-time payouts***

- **Batch payouts**
  - Pros: efficient, aligns with banking files.
  - Cons: slower merchant access to funds.
- **Instant payouts**
  - Pros: strong product value.
  - Cons: higher cost, fraud risk, and rail complexity.

Recommended: batch as default, instant for eligible merchants with risk controls.

## 12. Design Marketplace Split Payments

* **Question**
  Design marketplace payments where a platform collects from a buyer and splits funds among sellers, platform fees, reserves, and payouts.

* **Answer**
  Represent each split as ledger-backed transfers. The original payment funds a platform account, then ledger entries allocate amounts to seller balances, platform fees, taxes, and reserves.

**Scope**

- One buyer payment split across multiple connected sellers.
- Platform fees and seller payouts.
- Refund and dispute implications.

**Functional Requirements**

- Create split payment.
- Allocate funds to sellers.
- Collect platform fees.
- Handle partial refunds.
- Handle disputes and reversals.

**Non Functional Requirements**

- Accurate money allocation.
- Strong auditability.
- Idempotent transfer creation.
- Regulatory and tax awareness.

**High level design and diagram (at block level)**

```text
Marketplace Platform
        |
        v
Payment Service -> Split/Transfer Service
        |
        v
Ledger Service
   |        |        |
   v        v        v
Seller A  Seller B  Platform Fee
Balances  Balances  Account
```

***Explain the blocks***

- **Payment Service** collects buyer funds.
- **Split/Transfer Service** validates allocation rules.
- **Ledger Service** records all allocation entries.
- **Seller Balances** track payable funds by connected account.
- **Platform Fee Account** tracks marketplace revenue.

***Explain the control flow***

- Platform config defines fee rules, seller eligibility, tax behavior, and refund liability.
- Risk config may delay seller availability.
- Compliance controls determine whether platform or sellers are merchants of record.

***Explain the data flow***

- Buyer payment succeeds.
- Split service calculates allocations.
- Ledger posts transfers to sellers and platform fee account.
- Payout system later pays sellers.
- Refund/dispute reverses allocation graph.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Allocation timing***

- **Allocate after payment authorization**
  - Pros: sellers see pending funds early.
  - Cons: capture may fail.
- **Allocate after capture**
  - Pros: funds are more certain.
  - Cons: delayed seller visibility.
- **Allocate after settlement**
  - Pros: lowest financial risk.
  - Cons: slow and less useful.

Recommended: pending allocation after capture, available after settlement/risk delay.

***Refund liability***

- **Platform absorbs refund**
  - Pros: simpler seller experience.
  - Cons: platform risk.
- **Seller balance reversed**
  - Pros: financial responsibility follows seller.
  - Cons: negative balances and collection issues.
- **Configurable by marketplace contract**
  - Pros: flexible.
  - Cons: complexity in ledger and product behavior.

Recommended: contract-driven reversal rules represented explicitly in ledger.

## 13. Design Subscription Billing

* **Question**
  Design a subscription billing system that supports plans, billing cycles, invoices, payment retries, proration, cancellations, and webhooks.

* **Answer**
  Use subscription state machines and scheduled invoice generation. Billing should produce immutable invoices, attempt payment through the payment system, and manage retries/dunning through durable workflows.

**Scope**

- Recurring card billing.
- Fixed plans, usage extensions, trials, cancellations.
- Payment failure handling.

**Functional Requirements**

- Create and update subscriptions.
- Generate invoices on schedule.
- Calculate prorations and discounts.
- Attempt payments.
- Retry failed payments and notify customers.

**Non Functional Requirements**

- Deterministic billing calculations.
- Reliable scheduling.
- Auditable invoices.
- Time zone and calendar correctness.

**High level design and diagram (at block level)**

```text
Subscription API
      |
      v
Subscription Service -> Billing Schedule Store
      |
      v
Invoice Generator -> Invoice Service
      |
      v
Payment Service -> Webhooks/Notifications
```

***Explain the blocks***

- **Subscription Service** owns plan, customer, status, and lifecycle.
- **Schedule Store** tracks next billing dates and trial boundaries.
- **Invoice Generator** creates draft/final invoices.
- **Invoice Service** owns invoice immutability and totals.
- **Payment Service** collects invoice amount.

***Explain the control flow***

- Merchants configure prices, billing intervals, retry policy, tax, discounts, and cancellation behavior.
- Operators configure billing job partitions and retry windows.
- Product teams version billing rules to avoid retroactive surprises.

***Explain the data flow***

- Subscription is created with plan and payment method.
- Scheduler triggers invoice generation.
- Invoice is finalized.
- Payment is attempted.
- Success renews subscription; failure enters dunning.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Scheduling architecture***

- **Cron scans due subscriptions**
  - Pros: simple and observable.
  - Cons: needs sharding and careful idempotency.
- **Delayed queue per subscription**
  - Pros: natural per-object scheduling.
  - Cons: long delays and migrations are hard.
- **Workflow engine timers**
  - Pros: robust lifecycle management.
  - Cons: operational complexity.

Recommended: partitioned scheduler plus idempotent invoice generation; workflows for complex lifecycles.

***Proration***

- **Calculate dynamically on read**
  - Pros: flexible previews.
  - Cons: inconsistent if rules change.
- **Persist calculation at update time**
  - Pros: auditable.
  - Cons: harder to correct.

Recommended: preview dynamically, persist finalized invoice line items immutably.

## 14. Design Invoicing

* **Question**
  Design an invoicing system for merchants to bill customers, including draft invoices, finalization, taxes, discounts, payment, and credit notes.

* **Answer**
  Separate draft invoice editing from finalized immutable invoices. Finalization freezes line items, taxes, discounts, due date, customer, and currency. Any correction after finalization uses credit notes or adjustment invoices.

**Scope**

- Merchant-generated and subscription-generated invoices.
- Payment collection and invoice lifecycle.
- Excludes full accounting exports, but supports them.

**Functional Requirements**

- Create and edit draft invoices.
- Finalize invoices.
- Calculate totals, discounts, and taxes.
- Collect payment.
- Void, mark uncollectible, or issue credit notes.

**Non Functional Requirements**

- Invoice immutability after finalization.
- Accurate monetary calculations.
- Auditability.
- High availability for invoice retrieval.

**High level design and diagram (at block level)**

```text
Merchant / Billing Service
        |
        v
Invoice Service -> Tax/Discount Engine
        |
        v
Invoice Store
        |
        +--> Payment Service
        +--> PDF/Receipt Service
        +--> Webhooks
```

***Explain the blocks***

- **Invoice Service** owns invoice lifecycle.
- **Tax/Discount Engine** calculates line item adjustments.
- **Invoice Store** persists draft and finalized versions.
- **Payment Service** collects finalized amount.
- **PDF/Receipt** generates customer-facing artifacts.

***Explain the control flow***

- Merchants configure invoice templates, payment terms, tax settings, and accepted payment methods.
- Finance/legal rules define what can change after finalization.
- Product changes to invoice calculation are versioned.

***Explain the data flow***

- Draft invoice is created and edited.
- Tax and discounts are calculated.
- Invoice is finalized and made immutable.
- Payment is attempted or awaited.
- Webhooks and receipts are emitted.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Mutable vs immutable invoices***

- **Mutable invoices always**
  - Pros: easier merchant edits.
  - Cons: poor auditability and customer confusion.
- **Immutable after finalization**
  - Pros: compliance-friendly and predictable.
  - Cons: corrections require extra objects.

Recommended: drafts mutable, finalized invoices immutable.

***Tax calculation***

- **Inline tax calculation**
  - Pros: simple low-latency flow.
  - Cons: hard to evolve and audit.
- **Dedicated tax service**
  - Pros: centralized jurisdiction logic.
  - Cons: dependency and latency risk.

Recommended: dedicated tax service with cached rules and persisted calculation version.

## 15. Design Global Currency and FX Support

* **Question**
  Design support for multi-currency payments, balances, fees, FX conversion, settlement, and reporting.

* **Answer**
  Store all amounts as integer minor units with explicit currency. FX conversions use quote objects with rate, source, expiry, and audit metadata. Ledger entries preserve original and converted amounts.

**Scope**

- Multi-currency payments and merchant balances.
- FX quotes and settlement conversion.
- Does not build a trading platform.

**Functional Requirements**

- Represent amounts safely.
- Create FX quotes.
- Convert between currencies.
- Track merchant balances per currency.
- Report original and converted amounts.

**Non Functional Requirements**

- Monetary precision.
- Auditable rate usage.
- Correct rounding.
- Availability despite FX provider outages.

**High level design and diagram (at block level)**

```text
Payment / Payout Services
        |
        v
Currency Service -> FX Quote Service -> FX Providers
        |
        v
Ledger Service
        |
        v
Reports / Reconciliation
```

***Explain the blocks***

- **Currency Service** validates currency rules, minor units, and rounding.
- **FX Quote Service** fetches and stores conversion quotes.
- **FX Providers** supply rates.
- **Ledger Service** records per-currency entries.
- **Reports** expose settlement and accounting views.

***Explain the control flow***

- Finance config defines supported currencies, settlement currencies, and rounding behavior.
- Risk config defines quote expiry and fallback behavior.
- Operators manage provider failover and stale-rate policy.

***Explain the data flow***

- Payment specifies presentment currency.
- Merchant settlement currency is resolved.
- FX quote is created if conversion is needed.
- Ledger stores original and converted amounts with quote reference.
- Reconciliation validates settlement amount.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***FX quote timing***

- **Use live rate at authorization**
  - Pros: transparent customer price.
  - Cons: settlement may differ.
- **Use live rate at settlement**
  - Pros: reflects real funds movement.
  - Cons: customer or merchant uncertainty.
- **Lock quote for a window**
  - Pros: predictable.
  - Cons: platform takes rate movement risk.

Recommended: quote object with expiry and explicit party responsible for FX risk.

***Amount precision***

- **Floating point**
  - Pros: easy for developers.
  - Cons: unacceptable rounding errors.
- **Decimal**
  - Pros: better precision.
  - Cons: still requires currency-specific scale.
- **Integer minor units**
  - Pros: safest and common for payments.
  - Cons: special cases for zero-decimal and non-standard currencies.

Recommended: integer minor units plus currency metadata.

## 16. Design a Distributed Rate Limiter

* **Question**
  Design a rate limiter for Stripe APIs across merchants, endpoints, regions, and abuse patterns.

* **Answer**
  Use layered rate limiting. Enforce local low-latency limits at the edge and service layer, plus stronger regional or global quota checks for sensitive actions.

**Scope**

- API request limiting for public APIs.
- Merchant, API key, IP, endpoint, and risk-based dimensions.
- Global and regional deployment.

**Functional Requirements**

- Limit request rates.
- Support per-merchant and per-endpoint policies.
- Return useful rate-limit headers.
- Protect critical downstream systems.
- Allow emergency throttles.

**Non Functional Requirements**

- Very low latency.
- High availability.
- Fairness and tenant isolation.
- Configurable policies.

**High level design and diagram (at block level)**

```text
Client
  |
  v
Edge Gateway -> Local Token Bucket
  |
  v
API Service -> Regional Quota Service
  |
  v
Critical Business Services
```

***Explain the blocks***

- **Edge Gateway** applies coarse IP/API-key limits.
- **Local Token Bucket** handles fast-path decisions.
- **Regional Quota Service** enforces merchant and endpoint policies.
- **Config Store** distributes limit rules.
- **Critical Services** may apply additional domain-specific limits.

***Explain the control flow***

- Platform teams define default limits and merchant tiers.
- Abuse teams configure emergency blocks and risk-based throttles.
- Config is versioned and propagated to gateways.

***Explain the data flow***

- Request arrives with API key and endpoint.
- Edge checks local limits.
- API service checks merchant/endpoint quota.
- Allowed request proceeds.
- Rejected request returns 429 with retry guidance.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Centralized vs distributed limiting***

- **Centralized global limiter**
  - Pros: accurate global quota.
  - Cons: latency and availability bottleneck.
- **Local limiter only**
  - Pros: fast and resilient.
  - Cons: quota overshoot across regions.
- **Hybrid**
  - Pros: good balance.
  - Cons: more complex policy design.

Recommended: local enforcement for most traffic, global checks for high-risk operations.

***Algorithm choice***

- **Fixed window**
  - Pros: simple.
  - Cons: bursty at boundaries.
- **Sliding window**
  - Pros: smoother enforcement.
  - Cons: more storage/computation.
- **Token bucket**
  - Pros: supports bursts and steady rate.
  - Cons: approximate under distributed replication.

Recommended: token bucket for API limiting, with stricter counters for sensitive operations.

## 17. Design API Request Logging and Audit Trails

* **Question**
  Design an audit and request logging system for Stripe APIs that supports debugging, compliance, incident response, and merchant-visible request logs.

* **Answer**
  Capture structured audit events for all mutating requests and important reads. Separate hot request logs for debugging from immutable audit records for compliance. Include request IDs, actor, API key, idempotency key, object IDs, state changes, and trace IDs.

**Scope**

- Public API request logs and internal audit trail.
- Supports compliance and debugging.
- Avoids storing raw sensitive credentials.

**Functional Requirements**

- Log API requests and responses safely.
- Track actor and authentication context.
- Link logs to business objects.
- Support search and retention.
- Expose merchant-visible logs.

**Non Functional Requirements**

- High write throughput.
- Privacy and redaction.
- Tamper resistance.
- Low impact on API latency.

**High level design and diagram (at block level)**

```text
API Gateway / Services
        |
        v
Log Collector -> Redaction/Sampling
        |
        +--> Hot Search Store
        +--> Immutable Audit Store
        +--> Data Lake
```

***Explain the blocks***

- **Log Collector** receives structured logs and audit events.
- **Redaction** removes secrets and sensitive fields.
- **Hot Search Store** supports operational debugging.
- **Immutable Audit Store** supports compliance evidence.
- **Data Lake** supports analytics and anomaly detection.

***Explain the control flow***

- Security/compliance define retention, redaction, and access policies.
- Service owners define structured event schemas.
- Operators configure sampling for high-volume non-critical logs while preserving audit events.

***Explain the data flow***

- API request generates structured event.
- Collector validates schema.
- Sensitive fields are redacted.
- Event is written to hot and cold stores.
- Dashboards and investigations query by request ID, object ID, or merchant.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Synchronous vs asynchronous logging***

- **Synchronous audit write on request path**
  - Pros: strongest guarantee.
  - Cons: adds latency and availability dependency.
- **Asynchronous log pipeline**
  - Pros: scalable and lower latency.
  - Cons: possible log loss unless buffered durably.
- **Hybrid**
  - Pros: critical audit durable, verbose logs async.
  - Cons: more system complexity.

Recommended: synchronous minimal audit for critical mutations, async detailed logs.

***Privacy***

- **Store full request/response**
  - Pros: easier debugging.
  - Cons: unacceptable sensitive data exposure.
- **Aggressive redaction**
  - Pros: safer privacy posture.
  - Cons: harder debugging.

Recommended: structured allowlist logging, not raw payload capture.

## 18. Design Real-Time Payment Status Tracking

* **Question**
  Design a system that tracks payment status in real time across internal services, processors, webhooks, settlement files, and merchant queries.

* **Answer**
  Use event-driven state transitions. Persist raw events, validate transitions through a state machine, project current state into query tables, and emit merchant-facing updates.

**Scope**

- Payment status from creation through settlement.
- Multiple event sources.
- Merchant API and webhook visibility.

**Functional Requirements**

- Ingest internal and external status events.
- Validate legal state transitions.
- Query current status.
- Preserve history.
- Notify merchants of changes.

**Non Functional Requirements**

- Low-latency status updates.
- Correct ordering per payment.
- Resilient to duplicate and late events.
- Auditable history.

**High level design and diagram (at block level)**

```text
Internal Services / Processors / Files
        |
        v
Status Event Ingestion
        |
        v
State Machine Validator
        |
        +--> Event Store
        +--> Current State Projection
        +--> Webhooks/Analytics
```

***Explain the blocks***

- **Ingestion** normalizes events from different sources.
- **Validator** enforces allowed state transitions.
- **Event Store** preserves history.
- **Projection** powers API reads.
- **Webhooks/Analytics** consume state changes.

***Explain the control flow***

- Product/engineering define payment state machine versions.
- Processor adapters map external statuses to internal canonical states.
- Operators monitor stuck, invalid, and late transitions.

***Explain the data flow***

- Processor reports status update.
- Ingestion normalizes and dedupes event.
- Validator applies transition.
- Event is persisted.
- Current state projection updates.
- Merchant webhook is emitted.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Out-of-order events***

- **Drop older events by timestamp**
  - Pros: simple.
  - Cons: clocks are unreliable and late events may matter.
- **Use per-source sequence numbers**
  - Pros: strong ordering when available.
  - Cons: not all sources provide them.
- **State machine with precedence rules**
  - Pros: handles real-world messiness.
  - Cons: requires careful domain modeling.

Recommended: dedupe IDs plus state transition rules and source precedence.

***Event sourcing vs current-state table***

- **Current-state only**
  - Pros: simple and fast reads.
  - Cons: poor history and recovery.
- **Event sourcing only**
  - Pros: complete audit trail.
  - Cons: expensive reads.
- **Event store plus projection**
  - Pros: auditability and fast reads.
  - Cons: projection repair complexity.

Recommended: event store as history, projection for serving.

## 19. Design Reconciliation

* **Question**
  Design a reconciliation system that compares internal payment, ledger, refund, dispute, and payout records with processor and bank settlement data.

* **Answer**
  Build a batch and streaming reconciliation platform. Normalize external files/events, match them against internal ledger records, classify discrepancies, create exception workflows, and produce auditable reports.

**Scope**

- Processor and bank settlement reconciliation.
- Payments, refunds, fees, disputes, and payouts.
- Exception management.

**Functional Requirements**

- Ingest external settlement files.
- Normalize records.
- Match internal and external transactions.
- Detect missing, duplicate, and amount mismatches.
- Track exception resolution.

**Non Functional Requirements**

- High accuracy.
- Idempotent reprocessing.
- Auditability.
- Support for large files and delayed data.

**High level design and diagram (at block level)**

```text
Processor / Bank Files
        |
        v
File Ingestion -> Normalization -> Matching Engine
                                      |
        +-----------------------------+
        v
Internal Ledger/Payment Data -> Exceptions Store -> Reports
```

***Explain the blocks***

- **File Ingestion** receives external reports.
- **Normalization** maps provider-specific formats to canonical schema.
- **Matching Engine** compares external records to internal records.
- **Exceptions Store** tracks discrepancies.
- **Reports** support finance and operations.

***Explain the control flow***

- Finance config defines matching tolerances, settlement calendars, and escalation rules.
- Provider schemas are versioned.
- Reconciliation jobs have replay controls and audit logs.

***Explain the data flow***

- External file arrives.
- System validates and normalizes records.
- Matching engine links records to internal ledger entries.
- Matches are marked reconciled.
- Exceptions are queued for investigation or automatic repair.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Matching strategy***

- **Exact ID matching**
  - Pros: precise and fast.
  - Cons: not always available across banks/processors.
- **Composite matching**
  - Pros: handles messy real-world records.
  - Cons: false positives and tuning required.
- **ML/fuzzy matching**
  - Pros: can reduce manual work.
  - Cons: hard to audit for financial correctness.

Recommended: deterministic exact/composite matching first; ML only for suggestions.

***Batch vs streaming reconciliation***

- **Batch**
  - Pros: aligns with settlement files and finance workflows.
  - Cons: delayed detection.
- **Streaming**
  - Pros: faster anomaly detection.
  - Cons: external data may be incomplete.
- **Hybrid**
  - Pros: early warning plus final financial truth.
  - Cons: more pipelines.

Recommended: streaming checks for alerts, batch for official reconciliation.

## 20. Design a Workflow Engine for Payments

* **Question**
  Design a workflow engine for long-running payment workflows such as authorization, capture, refund, dispute, payout, webhook delivery, and billing retries.

* **Answer**
  Build or use a durable workflow system that persists workflow state, executes idempotent activities, supports timers and retries, and exposes observability. Payments need durable orchestration because many steps cross unreliable external systems.

**Scope**

- Internal workflow orchestration for payment domain services.
- Durable execution, retries, timers, and compensation.
- Not a general user-facing automation product.

**Functional Requirements**

- Start workflows with business IDs.
- Execute activities with retries.
- Persist state transitions.
- Support timers and deadlines.
- Resume after worker failure.
- Expose workflow history and controls.

**Non Functional Requirements**

- High reliability.
- Idempotent activity execution.
- Scalable worker model.
- Strong observability and debugging.

**High level design and diagram (at block level)**

```text
Domain Service
     |
     v
Workflow API -> Workflow State Store
     |
     v
Task Queue -> Workers -> External/Internal Activities
     |
     v
Timers / Retry Scheduler / History
```

***Explain the blocks***

- **Workflow API** starts and signals workflows.
- **State Store** persists workflow state and history.
- **Task Queue** dispatches work to workers.
- **Workers** execute idempotent activities.
- **Timers/Retry Scheduler** handles delays, deadlines, and backoff.
- **History** supports debugging and replay.

***Explain the control flow***

- Service teams define workflow definitions and activity contracts.
- Platform teams define retry policies, timeout defaults, and versioning rules.
- Operators monitor stuck workflows, poison tasks, and external dependency failures.

***Explain the data flow***

- Payment service starts a workflow.
- Workflow records initial state.
- Task is queued to worker.
- Worker calls fraud, ledger, processor, or webhook activity.
- Result updates workflow state.
- Timers and retries continue until terminal state.

**Deep dive topics and questions -> Explain the problem and suggest solutions**

***Build vs buy/use existing engine***

- **Custom lightweight engine**
  - Pros: tailored to domain, simpler initial model.
  - Cons: hard to get timers, replay, scaling, and debugging right.
- **Existing workflow engine**
  - Pros: mature retries, timers, state, visibility.
  - Cons: operational dependency and learning curve.
- **Ad hoc queues and jobs**
  - Pros: easy at small scale.
  - Cons: failure handling becomes scattered and fragile.

Recommended: use a mature durable workflow system for complex long-running flows; keep business logic idempotent.

***Activity idempotency***

- **Retry non-idempotent activities directly**
  - Pros: simple code path.
  - Cons: duplicate charges, refunds, or webhooks.
- **Require idempotency key per activity**
  - Pros: safe retries.
  - Cons: more contract discipline.
- **Compensating transactions only**
  - Pros: useful for some failures.
  - Cons: money movement cannot always be cleanly undone.

Recommended: idempotent activities first, compensations for domain-specific recovery.

## Question Index

1. Design a payment processing system.
2. Design an idempotency service.
3. Design a double-entry ledger.
4. Design webhook delivery.
5. Design refund processing.
6. Design chargeback / dispute management.
7. Design a payment method vault.
8. Design Checkout.
9. Design a fraud detection system.
10. Design merchant onboarding / KYC.
11. Design payouts.
12. Design marketplace split payments.
13. Design subscription billing.
14. Design invoicing.
15. Design global currency and FX support.
16. Design a distributed rate limiter.
17. Design API request logging and audit trails.
18. Design real-time payment status tracking.
19. Design reconciliation.
20. Design a workflow engine for payments.

## Useful Public References

- Stripe idempotency blog: https://stripe.com/blog/idempotency
- Stripe idempotent request docs: https://docs.stripe.com/api/idempotent_requests
- Stripe payment API design: https://stripe.dev/blog/payment-api-design
- Educative Stripe system design guide: https://www.educative.io/blog/stripe-system-design-interview-questions
