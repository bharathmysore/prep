# System Design Question TOC

Use this as the question-level table of contents for every tracked L7 system-design guide. It is derived from numbered `## N.` design-question headings in the company guides.

## Summary

| Company | Questions | Guide |
| --- | ---: | --- |
| Anthropic | 21 | [Anthropic L7 System Design Prep](./anthropic_l7_system_design_prep.md) |
| Apple | 26 | [Apple L7 System Design Prep](./apple_l7_system_design_prep.md) |
| AWS | 21 | [AWS L7 System Design Prep](./aws_l7_system_design_prep.md) |
| CoreWeave | 21 | [CoreWeave-Style L7 System Design Prep](./coreweave_l7_system_design_prep.md) |
| Databricks | 20 | [Databricks L7 System Design Prep](./databricks_l7_system_design_prep.md) |
| Google | 21 | [Google L7 System Design Prep: Cloud and Distributed Systems Question Catalog](./google_l7_system_design_prep.md) |
| Meta | 21 | [Meta L7 System Design Practice Guide](./meta_l7_system_design_prep.md) |
| Microsoft | 21 | [Microsoft L7 System Design Interview Prep](./microsoft_l7_system_design_prep.md) |
| NVIDIA | 20 | [NVIDIA L7 System Design Prep: Cloud and Distributed Systems Question Catalog](./nvidia_l7_system_design_prep.md) |
| OpenAI | 22 | [OpenAI L7 System Design Prep: Question Catalog](./openai_l7_system_design_prep.md) |
| Oracle | 20 | [Oracle L7 System Design Interview Prep](./oracle_l7_system_design_prep.md) |
| Snowflake | 21 | [Snowflake L7 System Design Interview Prep](./snowflake_l7_system_design_prep.md) |
| Stripe | 20 | [Stripe L7 System Design Interview Prep](./stripe_l7_system_design_prep.md) |
| Total | 275 | 13 guides |

## Anthropic

Source guide: [Anthropic L7 System Design Prep](./anthropic_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design Claude's Real-Time Inference Serving Stack](./anthropic_l7_system_design_prep.md#1-design-claudes-real-time-inference-serving-stack) |
| 2 | [Design Dynamic Batching For Synchronous LLM Requests](./anthropic_l7_system_design_prep.md#2-design-dynamic-batching-for-synchronous-llm-requests) |
| 3 | [Design A Multi-Tenant GPU Scheduler](./anthropic_l7_system_design_prep.md#3-design-a-multi-tenant-gpu-scheduler) |
| 4 | [Design Token Streaming To Millions Of Users](./anthropic_l7_system_design_prep.md#4-design-token-streaming-to-millions-of-users) |
| 5 | [Design A Safety / Safeguards Layer For LLM Requests](./anthropic_l7_system_design_prep.md#5-design-a-safety-safeguards-layer-for-llm-requests) |
| 6 | [Design An Evaluation Platform For New Claude Models](./anthropic_l7_system_design_prep.md#6-design-an-evaluation-platform-for-new-claude-models) |
| 7 | [Design Enterprise RAG Over Private Documents](./anthropic_l7_system_design_prep.md#7-design-enterprise-rag-over-private-documents) |
| 8 | [Design An MCP / Tool-Use Platform](./anthropic_l7_system_design_prep.md#8-design-an-mcp-tool-use-platform) |
| 9 | [Design A Vector Embedding Platform](./anthropic_l7_system_design_prep.md#9-design-a-vector-embedding-platform) |
| 10 | [Design A Batch Inference API](./anthropic_l7_system_design_prep.md#10-design-a-batch-inference-api) |
| 11 | [Design Distributed Rate Limiting For The Claude API](./anthropic_l7_system_design_prep.md#11-design-distributed-rate-limiting-for-the-claude-api) |
| 12 | [Design Conversation History Storage](./anthropic_l7_system_design_prep.md#12-design-conversation-history-storage) |
| 13 | [Design Observability For LLM Inference](./anthropic_l7_system_design_prep.md#13-design-observability-for-llm-inference) |
| 14 | [Design Incident Detection And Auto-Rollback For Model Serving](./anthropic_l7_system_design_prep.md#14-design-incident-detection-and-auto-rollback-for-model-serving) |
| 15 | [Design Model Rollout / Canary Infrastructure](./anthropic_l7_system_design_prep.md#15-design-model-rollout-canary-infrastructure) |
| 16 | [Design A Training-Data Ingestion And Filtering Pipeline](./anthropic_l7_system_design_prep.md#16-design-a-training-data-ingestion-and-filtering-pipeline) |
| 17 | [Design A Distributed Model Artifact Store](./anthropic_l7_system_design_prep.md#17-design-a-distributed-model-artifact-store) |
| 18 | [Design Prompt / KV Caching For LLM Serving](./anthropic_l7_system_design_prep.md#18-design-prompt-kv-caching-for-llm-serving) |
| 19 | [Design A Web Crawler For Model Or Evaluation Data](./anthropic_l7_system_design_prep.md#19-design-a-web-crawler-for-model-or-evaluation-data) |
| 20 | [Design A Distributed File Cache For Model-Serving Clusters](./anthropic_l7_system_design_prep.md#20-design-a-distributed-file-cache-for-model-serving-clusters) |
| 21 | [Design An Agentic Coding Platform For Long-Running Cloud Workflows](./anthropic_l7_system_design_prep.md#21-design-an-agentic-coding-platform-for-long-running-cloud-workflows) |

## Apple

Source guide: [Apple L7 System Design Prep](./apple_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design iCloud Drive](./apple_l7_system_design_prep.md#1-design-icloud-drive) |
| 2 | [Design iCloud Photos](./apple_l7_system_design_prep.md#2-design-icloud-photos) |
| 3 | [Design Apple Music or Apple TV+ Streaming](./apple_l7_system_design_prep.md#3-design-apple-music-or-apple-tv-streaming) |
| 4 | [Design Apple Maps](./apple_l7_system_design_prep.md#4-design-apple-maps) |
| 5 | [Design AirTag / Find My](./apple_l7_system_design_prep.md#5-design-airtag-find-my) |
| 6 | [Design APNs / Push Notification Service](./apple_l7_system_design_prep.md#6-design-apns-push-notification-service) |
| 7 | [Design App Store Backend](./apple_l7_system_design_prep.md#7-design-app-store-backend) |
| 8 | [Design Apple Pay](./apple_l7_system_design_prep.md#8-design-apple-pay) |
| 9 | [Design iMessage](./apple_l7_system_design_prep.md#9-design-imessage) |
| 10 | [Design FaceTime Signaling and Media Routing](./apple_l7_system_design_prep.md#10-design-facetime-signaling-and-media-routing) |
| 11 | [Design Siri / Apple Intelligence RAG Assistant](./apple_l7_system_design_prep.md#11-design-siri-apple-intelligence-rag-assistant) |
| 12 | [Design Xcode Cloud Build System](./apple_l7_system_design_prep.md#12-design-xcode-cloud-build-system) |
| 13 | [Design Build Artifact Storage](./apple_l7_system_design_prep.md#13-design-build-artifact-storage) |
| 14 | [Design iOS Software Update Rollout](./apple_l7_system_design_prep.md#14-design-ios-software-update-rollout) |
| 15 | [Design a Data Center / Cloud Region Control System](./apple_l7_system_design_prep.md#15-design-a-data-center-cloud-region-control-system) |
| 16 | [Design Device Telemetry and Crash Analytics](./apple_l7_system_design_prep.md#16-design-device-telemetry-and-crash-analytics) |
| 17 | [Design Typeahead Search](./apple_l7_system_design_prep.md#17-design-typeahead-search) |
| 18 | [Design Family Sharing / Subscription Entitlements](./apple_l7_system_design_prep.md#18-design-family-sharing-subscription-entitlements) |
| 19 | [Design HomeKit Cloud Sync and Remote Access](./apple_l7_system_design_prep.md#19-design-homekit-cloud-sync-and-remote-access) |
| 20 | [Design Private Relay / Privacy-Preserving Proxy](./apple_l7_system_design_prep.md#20-design-private-relay-privacy-preserving-proxy) |
| 21 | [Design Elastic Disk / EBS-Class Block Storage](./apple_l7_system_design_prep.md#21-design-elastic-disk-ebs-class-block-storage) |
| 22 | [Design Replication And Metadata For Elastic Disk](./apple_l7_system_design_prep.md#22-design-replication-and-metadata-for-elastic-disk) |
| 23 | [Design Point-In-Time Snapshots And Volume Restore](./apple_l7_system_design_prep.md#23-design-point-in-time-snapshots-and-volume-restore) |
| 24 | [Design Continuous Scrubbing And Automated Repair](./apple_l7_system_design_prep.md#24-design-continuous-scrubbing-and-automated-repair) |
| 25 | [Design Multi-Tenant I/O QoS And Noisy-Neighbor Control](./apple_l7_system_design_prep.md#25-design-multi-tenant-io-qos-and-noisy-neighbor-control) |
| 26 | [Design A Cross-Org Replication Foundation For Storage Backends](./apple_l7_system_design_prep.md#26-design-a-cross-org-replication-foundation-for-storage-backends) |

## AWS

Source guide: [AWS L7 System Design Prep](./aws_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design Amazon S3 / Object Storage](./aws_l7_system_design_prep.md#1-design-amazon-s3-object-storage) |
| 2 | [Design DynamoDB / Distributed Key-Value Store](./aws_l7_system_design_prep.md#2-design-dynamodb-distributed-key-value-store) |
| 3 | [Design SQS / Distributed Message Queue](./aws_l7_system_design_prep.md#3-design-sqs-distributed-message-queue) |
| 4 | [Design Kinesis / Event Streaming Platform](./aws_l7_system_design_prep.md#4-design-kinesis-event-streaming-platform) |
| 5 | [Design Lambda / Serverless Compute](./aws_l7_system_design_prep.md#5-design-lambda-serverless-compute) |
| 6 | [Design API Gateway With Distributed Rate Limiting](./aws_l7_system_design_prep.md#6-design-api-gateway-with-distributed-rate-limiting) |
| 7 | [Design CloudWatch / Metrics And Logs Platform](./aws_l7_system_design_prep.md#7-design-cloudwatch-metrics-and-logs-platform) |
| 8 | [Design CloudFront / CDN](./aws_l7_system_design_prep.md#8-design-cloudfront-cdn) |
| 9 | [Design Route 53 / Global DNS And Traffic Routing](./aws_l7_system_design_prep.md#9-design-route-53-global-dns-and-traffic-routing) |
| 10 | [Design IAM / Authorization Service](./aws_l7_system_design_prep.md#10-design-iam-authorization-service) |
| 11 | [Design EBS / Distributed Block Storage](./aws_l7_system_design_prep.md#11-design-ebs-distributed-block-storage) |
| 12 | [Design Aurora / Cloud Relational Database](./aws_l7_system_design_prep.md#12-design-aurora-cloud-relational-database) |
| 13 | [Design ECS/EKS / Container Orchestration Scheduler](./aws_l7_system_design_prep.md#13-design-ecseks-container-orchestration-scheduler) |
| 14 | [Design EventBridge / Distributed Scheduler](./aws_l7_system_design_prep.md#14-design-eventbridge-distributed-scheduler) |
| 15 | [Design Step Functions / Workflow Orchestrator](./aws_l7_system_design_prep.md#15-design-step-functions-workflow-orchestrator) |
| 16 | [Design Multi-Region Checkout / Order System](./aws_l7_system_design_prep.md#16-design-multi-region-checkout-order-system) |
| 17 | [Design Distributed Inventory Management](./aws_l7_system_design_prep.md#17-design-distributed-inventory-management) |
| 18 | [Design Product Search For Amazon Catalog](./aws_l7_system_design_prep.md#18-design-product-search-for-amazon-catalog) |
| 19 | [Design Recommendation Platform](./aws_l7_system_design_prep.md#19-design-recommendation-platform) |
| 20 | [Design Notification System For Prime Day Scale](./aws_l7_system_design_prep.md#20-design-notification-system-for-prime-day-scale) |
| 21 | [Design An Enterprise AI Agent Platform](./aws_l7_system_design_prep.md#21-design-an-enterprise-ai-agent-platform) |

## CoreWeave

Source guide: [CoreWeave-Style L7 System Design Prep](./coreweave_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design a GPU Cluster Scheduler](./coreweave_l7_system_design_prep.md#1-question-design-a-gpu-cluster-scheduler) |
| 2 | [Design a Managed Kubernetes Service For GPU Workloads](./coreweave_l7_system_design_prep.md#2-question-design-a-managed-kubernetes-service-for-gpu-workloads) |
| 3 | [Design An LLM Inference Serving Platform](./coreweave_l7_system_design_prep.md#3-question-design-an-llm-inference-serving-platform) |
| 4 | [Design A Distributed Training Job Orchestrator](./coreweave_l7_system_design_prep.md#4-question-design-a-distributed-training-job-orchestrator) |
| 5 | [Design GPU Capacity Reservations And Quotas](./coreweave_l7_system_design_prep.md#5-question-design-gpu-capacity-reservations-and-quotas) |
| 6 | [Design AI Dataset Storage For Training](./coreweave_l7_system_design_prep.md#6-question-design-ai-dataset-storage-for-training) |
| 7 | [Design A Model Artifact Distribution System](./coreweave_l7_system_design_prep.md#7-question-design-a-model-artifact-distribution-system) |
| 8 | [Design Bare-Metal GPU Node Health Monitoring](./coreweave_l7_system_design_prep.md#8-question-design-bare-metal-gpu-node-health-monitoring) |
| 9 | [Design Observability For GPU Clusters](./coreweave_l7_system_design_prep.md#9-question-design-observability-for-gpu-clusters) |
| 10 | [Design Topology-Aware Placement For Distributed Training](./coreweave_l7_system_design_prep.md#10-question-design-topology-aware-placement-for-distributed-training) |
| 11 | [Design Autoscaling For GPU Inference](./coreweave_l7_system_design_prep.md#11-question-design-autoscaling-for-gpu-inference) |
| 12 | [Design A Multi-Tenant GPU Cloud Control Plane](./coreweave_l7_system_design_prep.md#12-question-design-a-multi-tenant-gpu-cloud-control-plane) |
| 13 | [Design GPU Usage Metering And Billing](./coreweave_l7_system_design_prep.md#13-question-design-gpu-usage-metering-and-billing) |
| 14 | [Design A Distributed Job Queue For Scarce GPU Resources](./coreweave_l7_system_design_prep.md#14-question-design-a-distributed-job-queue-for-scarce-gpu-resources) |
| 15 | [Design Driver And Kubernetes Upgrade Orchestration](./coreweave_l7_system_design_prep.md#15-question-design-driver-and-kubernetes-upgrade-orchestration) |
| 16 | [Design Cross-Cloud Dataset Ingestion](./coreweave_l7_system_design_prep.md#16-question-design-cross-cloud-dataset-ingestion) |
| 17 | [Design Network Isolation For Customer GPU Clusters](./coreweave_l7_system_design_prep.md#17-question-design-network-isolation-for-customer-gpu-clusters) |
| 18 | [Design A Fault-Tolerant Checkpointing System](./coreweave_l7_system_design_prep.md#18-question-design-a-fault-tolerant-checkpointing-system) |
| 19 | [Design A GPU Fleet Incident Detection System](./coreweave_l7_system_design_prep.md#19-question-design-a-gpu-fleet-incident-detection-system) |
| 20 | [Design A Control Plane API For Provisioning GPU Clusters](./coreweave_l7_system_design_prep.md#20-question-design-a-control-plane-api-for-provisioning-gpu-clusters) |
| 21 | [Design A Cross-Cloud AI Training Control Plane](./coreweave_l7_system_design_prep.md#21-question-design-a-cross-cloud-ai-training-control-plane) |

## Databricks

Source guide: [Databricks L7 System Design Prep](./databricks_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design Delta Lake On Cloud Object Storage](./databricks_l7_system_design_prep.md#1-design-delta-lake-on-cloud-object-storage) |
| 2 | [Design A Real-Time Ingestion Pipeline Into A Lakehouse](./databricks_l7_system_design_prep.md#2-design-a-real-time-ingestion-pipeline-into-a-lakehouse) |
| 3 | [Design CDC From OLTP Databases Into A Lakehouse](./databricks_l7_system_design_prep.md#3-design-cdc-from-oltp-databases-into-a-lakehouse) |
| 4 | [Design A Medallion Data Architecture](./databricks_l7_system_design_prep.md#4-design-a-medallion-data-architecture) |
| 5 | [Design A Distributed SQL Query Engine](./databricks_l7_system_design_prep.md#5-design-a-distributed-sql-query-engine) |
| 6 | [Design A Metadata And Catalog Service](./databricks_l7_system_design_prep.md#6-design-a-metadata-and-catalog-service) |
| 7 | [Design Unity Catalog-Style Access Control](./databricks_l7_system_design_prep.md#7-design-unity-catalog-style-access-control) |
| 8 | [Design A Data Pipeline Job Scheduler](./databricks_l7_system_design_prep.md#8-design-a-data-pipeline-job-scheduler) |
| 9 | [Design Autoscaling Spark Clusters](./databricks_l7_system_design_prep.md#9-design-autoscaling-spark-clusters) |
| 10 | [Design Storage Compaction And File Optimization](./databricks_l7_system_design_prep.md#10-design-storage-compaction-and-file-optimization) |
| 11 | [Design Exactly-Once Streaming Writes](./databricks_l7_system_design_prep.md#11-design-exactly-once-streaming-writes) |
| 12 | [Design Late Event Handling For Streaming Aggregations](./databricks_l7_system_design_prep.md#12-design-late-event-handling-for-streaming-aggregations) |
| 13 | [Design A Multi-Tenant Lakehouse Platform](./databricks_l7_system_design_prep.md#13-design-a-multi-tenant-lakehouse-platform) |
| 14 | [Design Query Admission Control](./databricks_l7_system_design_prep.md#14-design-query-admission-control) |
| 15 | [Design Observability For Spark Jobs](./databricks_l7_system_design_prep.md#15-design-observability-for-spark-jobs) |
| 16 | [Design A Feature Store](./databricks_l7_system_design_prep.md#16-design-a-feature-store) |
| 17 | [Design ML Training And Model Registry Platform](./databricks_l7_system_design_prep.md#17-design-ml-training-and-model-registry-platform) |
| 18 | [Design Vector Search And RAG On A Lakehouse](./databricks_l7_system_design_prep.md#18-design-vector-search-and-rag-on-a-lakehouse) |
| 19 | [Design Cross-Region Replication And Disaster Recovery](./databricks_l7_system_design_prep.md#19-design-cross-region-replication-and-disaster-recovery) |
| 20 | [Design Secure Data Sharing Across Organizations](./databricks_l7_system_design_prep.md#20-design-secure-data-sharing-across-organizations) |

## Google

Source guide: [Google L7 System Design Prep: Cloud and Distributed Systems Question Catalog](./google_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design Google Search](./google_l7_system_design_prep.md#1-design-google-search) |
| 2 | [Design Google Drive / Cloud File Storage](./google_l7_system_design_prep.md#2-design-google-drive-cloud-file-storage) |
| 3 | [Design YouTube / Video Streaming](./google_l7_system_design_prep.md#3-design-youtube-video-streaming) |
| 4 | [Design Google Maps](./google_l7_system_design_prep.md#4-design-google-maps) |
| 5 | [Design Gmail / Email System](./google_l7_system_design_prep.md#5-design-gmail-email-system) |
| 6 | [Design Google Photos / Media Storage](./google_l7_system_design_prep.md#6-design-google-photos-media-storage) |
| 7 | [Design Google Calendar / Scheduling System](./google_l7_system_design_prep.md#7-design-google-calendar-scheduling-system) |
| 8 | [Design a Distributed Cache / Key-Value Store](./google_l7_system_design_prep.md#8-design-a-distributed-cache-key-value-store) |
| 9 | [Design a Notification System](./google_l7_system_design_prep.md#9-design-a-notification-system) |
| 10 | [Design a Metrics / Logging Platform](./google_l7_system_design_prep.md#10-design-a-metrics-logging-platform) |
| 11 | [Design a Distributed File System Like GFS](./google_l7_system_design_prep.md#11-design-a-distributed-file-system-like-gfs) |
| 12 | [Design Google Docs / Collaborative Editing](./google_l7_system_design_prep.md#12-design-google-docs-collaborative-editing) |
| 13 | [Design a Distributed Job Scheduler](./google_l7_system_design_prep.md#13-design-a-distributed-job-scheduler) |
| 14 | [Design a Global Software Rollout System](./google_l7_system_design_prep.md#14-design-a-global-software-rollout-system) |
| 15 | [Design a 100 PB Cross-Region Data Migration](./google_l7_system_design_prep.md#15-design-a-100-pb-cross-region-data-migration) |
| 16 | [Design a URL Shortener](./google_l7_system_design_prep.md#16-design-a-url-shortener) |
| 17 | [Design Search Autocomplete](./google_l7_system_design_prep.md#17-design-search-autocomplete) |
| 18 | [Design a Web Crawler](./google_l7_system_design_prep.md#18-design-a-web-crawler) |
| 19 | [Design a Distributed Lock / Leader Election Service](./google_l7_system_design_prep.md#19-design-a-distributed-lock-leader-election-service) |
| 20 | [Design an IoT / Telemetry Ingestion System](./google_l7_system_design_prep.md#20-design-an-iot-telemetry-ingestion-system) |
| 21 | [Design A TPU / AI Accelerator Cluster Control Plane](./google_l7_system_design_prep.md#21-design-a-tpu-ai-accelerator-cluster-control-plane) |

## Meta

Source guide: [Meta L7 System Design Practice Guide](./meta_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design Facebook News Feed](./meta_l7_system_design_prep.md#1-design-facebook-news-feed) |
| 2 | [Design Instagram](./meta_l7_system_design_prep.md#2-design-instagram) |
| 3 | [Design WhatsApp / Messenger](./meta_l7_system_design_prep.md#3-design-whatsapp-messenger) |
| 4 | [Design Facebook Chat / Real-Time Messaging](./meta_l7_system_design_prep.md#4-design-facebook-chat-real-time-messaging) |
| 5 | [Design Instagram Stories / Reels](./meta_l7_system_design_prep.md#5-design-instagram-stories-reels) |
| 6 | [Design Video Upload And Sharing](./meta_l7_system_design_prep.md#6-design-video-upload-and-sharing) |
| 7 | [Design Facebook Live / Live Streaming](./meta_l7_system_design_prep.md#7-design-facebook-live-live-streaming) |
| 8 | [Design Live Commenting](./meta_l7_system_design_prep.md#8-design-live-commenting) |
| 9 | [Design Notifications](./meta_l7_system_design_prep.md#9-design-notifications) |
| 10 | [Design Large-Scale Proximity Service / Nearby Friends](./meta_l7_system_design_prep.md#10-design-large-scale-proximity-service-nearby-friends) |
| 11 | [Design Search Autocomplete](./meta_l7_system_design_prep.md#11-design-search-autocomplete) |
| 12 | [Design Facebook Status Search](./meta_l7_system_design_prep.md#12-design-facebook-status-search) |
| 13 | [Design Search And Recommendation System](./meta_l7_system_design_prep.md#13-design-search-and-recommendation-system) |
| 14 | [Design Social Graph / Follow Graph](./meta_l7_system_design_prep.md#14-design-social-graph-follow-graph) |
| 15 | [Design A Key-Value Database](./meta_l7_system_design_prep.md#15-design-a-key-value-database) |
| 16 | [Design A Distributed File System](./meta_l7_system_design_prep.md#16-design-a-distributed-file-system) |
| 17 | [Design Photo Storage And CDN Delivery](./meta_l7_system_design_prep.md#17-design-photo-storage-and-cdn-delivery) |
| 18 | [Design Instagram Auction / Ads Auction System](./meta_l7_system_design_prep.md#18-design-instagram-auction-ads-auction-system) |
| 19 | [Design Real-Time Analytics For Meta Events](./meta_l7_system_design_prep.md#19-design-real-time-analytics-for-meta-events) |
| 20 | [Design Privacy / Blocking / Content Visibility Service](./meta_l7_system_design_prep.md#20-design-privacy-blocking-content-visibility-service) |
| 21 | [Design A Heterogeneous AI Inference Platform](./meta_l7_system_design_prep.md#21-design-a-heterogeneous-ai-inference-platform) |

## Microsoft

Source guide: [Microsoft L7 System Design Interview Prep](./microsoft_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design OneDrive / Cloud File Sync](./microsoft_l7_system_design_prep.md#1-design-onedrive-cloud-file-sync) |
| 2 | [Design Microsoft Teams Chat](./microsoft_l7_system_design_prep.md#2-design-microsoft-teams-chat) |
| 3 | [Design Teams Video Conferencing](./microsoft_l7_system_design_prep.md#3-design-teams-video-conferencing) |
| 4 | [Design Outlook / Email Delivery](./microsoft_l7_system_design_prep.md#4-design-outlook-email-delivery) |
| 5 | [Design Azure Blob Storage / Object Storage](./microsoft_l7_system_design_prep.md#5-design-azure-blob-storage-object-storage) |
| 6 | [Design a Globally Distributed Database Like Cosmos DB](./microsoft_l7_system_design_prep.md#6-design-a-globally-distributed-database-like-cosmos-db) |
| 7 | [Design a Distributed Cache](./microsoft_l7_system_design_prep.md#7-design-a-distributed-cache) |
| 8 | [Design a CDN / Edge Caching System](./microsoft_l7_system_design_prep.md#8-design-a-cdn-edge-caching-system) |
| 9 | [Design a Distributed Rate Limiter for Azure APIs](./microsoft_l7_system_design_prep.md#9-design-a-distributed-rate-limiter-for-azure-apis) |
| 10 | [Design a Notification Platform](./microsoft_l7_system_design_prep.md#10-design-a-notification-platform) |
| 11 | [Design Azure Monitor / Observability Platform](./microsoft_l7_system_design_prep.md#11-design-azure-monitor-observability-platform) |
| 12 | [Design a Distributed Job Scheduler](./microsoft_l7_system_design_prep.md#12-design-a-distributed-job-scheduler) |
| 13 | [Design a Feature Flag / Config Platform](./microsoft_l7_system_design_prep.md#13-design-a-feature-flag-config-platform) |
| 14 | [Design Microsoft Entra ID / Authentication Service](./microsoft_l7_system_design_prep.md#14-design-microsoft-entra-id-authentication-service) |
| 15 | [Design a Multi-Tenant SaaS Platform](./microsoft_l7_system_design_prep.md#15-design-a-multi-tenant-saas-platform) |
| 16 | [Design Search for Office Documents](./microsoft_l7_system_design_prep.md#16-design-search-for-office-documents) |
| 17 | [Design an Event Streaming Platform Like Event Hubs](./microsoft_l7_system_design_prep.md#17-design-an-event-streaming-platform-like-event-hubs) |
| 18 | [Design a Distributed Lock / Coordination Service](./microsoft_l7_system_design_prep.md#18-design-a-distributed-lock-coordination-service) |
| 19 | [Design Real-Time Collaborative Editing for Word Online](./microsoft_l7_system_design_prep.md#19-design-real-time-collaborative-editing-for-word-online) |
| 20 | [Design Azure Resource Manager / Cloud Provisioning Control Plane](./microsoft_l7_system_design_prep.md#20-design-azure-resource-manager-cloud-provisioning-control-plane) |
| 21 | [Design A Microsoft Foundry Agent Runtime And Tool Platform](./microsoft_l7_system_design_prep.md#21-design-a-microsoft-foundry-agent-runtime-and-tool-platform) |

## NVIDIA

Source guide: [NVIDIA L7 System Design Prep: Cloud and Distributed Systems Question Catalog](./nvidia_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design a Cloud-Based GPU Resource Management System](./nvidia_l7_system_design_prep.md#1-design-a-cloud-based-gpu-resource-management-system) |
| 2 | [Design a Distributed Training System for a Trillion-Parameter LLM](./nvidia_l7_system_design_prep.md#2-design-a-distributed-training-system-for-a-trillion-parameter-llm) |
| 3 | [Design an LLM Inference Serving Platform](./nvidia_l7_system_design_prep.md#3-design-an-llm-inference-serving-platform) |
| 4 | [Design Disaggregated LLM Serving with Separate Prefill and Decode Workers](./nvidia_l7_system_design_prep.md#4-design-disaggregated-llm-serving-with-separate-prefill-and-decode-workers) |
| 5 | [Design KV Cache Management for LLM Inference](./nvidia_l7_system_design_prep.md#5-design-kv-cache-management-for-llm-inference) |
| 6 | [Design a Distributed Cache for GPU Workloads](./nvidia_l7_system_design_prep.md#6-design-a-distributed-cache-for-gpu-workloads) |
| 7 | [Design a High-Throughput Training Data Pipeline](./nvidia_l7_system_design_prep.md#7-design-a-high-throughput-training-data-pipeline) |
| 8 | [Design a Model Artifact Distribution System](./nvidia_l7_system_design_prep.md#8-design-a-model-artifact-distribution-system) |
| 9 | [Design Monitoring for a Large GPU Cluster](./nvidia_l7_system_design_prep.md#9-design-monitoring-for-a-large-gpu-cluster) |
| 10 | [Design Autoscaling for GPU Inference Clusters](./nvidia_l7_system_design_prep.md#10-design-autoscaling-for-gpu-inference-clusters) |
| 11 | [Design a Multi-Tenant GPU Cloud](./nvidia_l7_system_design_prep.md#11-design-a-multi-tenant-gpu-cloud) |
| 12 | [Design a Topology-Aware GPU Job Scheduler](./nvidia_l7_system_design_prep.md#12-design-a-topology-aware-gpu-job-scheduler) |
| 13 | [Design Checkpointing and Recovery for Distributed Training](./nvidia_l7_system_design_prep.md#13-design-checkpointing-and-recovery-for-distributed-training) |
| 14 | [Design a System to Detect and Diagnose Poor Multi-GPU Performance](./nvidia_l7_system_design_prep.md#14-design-a-system-to-detect-and-diagnose-poor-multi-gpu-performance) |
| 15 | [Design a Real-Time Video Analytics Platform Using GPUs](./nvidia_l7_system_design_prep.md#15-design-a-real-time-video-analytics-platform-using-gpus) |
| 16 | [Design an Autonomous-Driving Data Ingestion and Simulation Platform](./nvidia_l7_system_design_prep.md#16-design-an-autonomous-driving-data-ingestion-and-simulation-platform) |
| 17 | [Design a Distributed Rate Limiter for GPU Inference APIs](./nvidia_l7_system_design_prep.md#17-design-a-distributed-rate-limiter-for-gpu-inference-apis) |
| 18 | [Design an AI Workflow Orchestration Platform](./nvidia_l7_system_design_prep.md#18-design-an-ai-workflow-orchestration-platform) |
| 19 | [Design a Secure Model-Serving Platform for Enterprise Customers](./nvidia_l7_system_design_prep.md#19-design-a-secure-model-serving-platform-for-enterprise-customers) |
| 20 | [Design a GPU Utilization Optimization System](./nvidia_l7_system_design_prep.md#20-design-a-gpu-utilization-optimization-system) |

## OpenAI

Source guide: [OpenAI L7 System Design Prep: Question Catalog](./openai_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design ChatGPT At Massive Scale](./openai_l7_system_design_prep.md#1-design-chatgpt-at-massive-scale) |
| 2 | [Design Real-Time LLM Response Streaming](./openai_l7_system_design_prep.md#2-design-real-time-llm-response-streaming) |
| 3 | [Design The OpenAI Playground](./openai_l7_system_design_prep.md#3-design-the-openai-playground) |
| 4 | [Design An LLM-Powered Enterprise Search System](./openai_l7_system_design_prep.md#4-design-an-llm-powered-enterprise-search-system) |
| 5 | [Design A Vector Database For Billions Of Embeddings](./openai_l7_system_design_prep.md#5-design-a-vector-database-for-billions-of-embeddings) |
| 6 | [Design GPU Scheduling For Training And Inference](./openai_l7_system_design_prep.md#6-design-gpu-scheduling-for-training-and-inference) |
| 7 | [Design An Inference Batching Service](./openai_l7_system_design_prep.md#7-design-an-inference-batching-service) |
| 8 | [Design File Upload And RAG Ingestion](./openai_l7_system_design_prep.md#8-design-file-upload-and-rag-ingestion) |
| 9 | [Design API Rate Limiting And Quotas For LLM APIs](./openai_l7_system_design_prep.md#9-design-api-rate-limiting-and-quotas-for-llm-apis) |
| 10 | [Design Billing And Usage Metering For Model APIs](./openai_l7_system_design_prep.md#10-design-billing-and-usage-metering-for-model-apis) |
| 11 | [Design A Safety And Moderation Pipeline](./openai_l7_system_design_prep.md#11-design-a-safety-and-moderation-pipeline) |
| 12 | [Design A Model Gateway And Router](./openai_l7_system_design_prep.md#12-design-a-model-gateway-and-router) |
| 13 | [Design An Evaluation And Experimentation Platform](./openai_l7_system_design_prep.md#13-design-an-evaluation-and-experimentation-platform) |
| 14 | [Design A Distributed Job Scheduler](./openai_l7_system_design_prep.md#14-design-a-distributed-job-scheduler) |
| 15 | [Design A Webhook Delivery System](./openai_l7_system_design_prep.md#15-design-a-webhook-delivery-system) |
| 16 | [Design A Fault-Tolerant Polite Web Crawler](./openai_l7_system_design_prep.md#16-design-a-fault-tolerant-polite-web-crawler) |
| 17 | [Design An In-Memory Database](./openai_l7_system_design_prep.md#17-design-an-in-memory-database) |
| 18 | [Design Slack-Style Chat](./openai_l7_system_design_prep.md#18-design-slack-style-chat) |
| 19 | [Design An Observability Platform For LLM Infrastructure](./openai_l7_system_design_prep.md#19-design-an-observability-platform-for-llm-infrastructure) |
| 20 | [Design A Secure Code Execution Sandbox](./openai_l7_system_design_prep.md#20-design-a-secure-code-execution-sandbox) |
| 21 | [Design An Agent Tool-Use Control Plane](./openai_l7_system_design_prep.md#21-design-an-agent-tool-use-control-plane) |
| 22 | [Design An Agent-Ready Workflow Automation Platform](./openai_l7_system_design_prep.md#22-design-an-agent-ready-workflow-automation-platform) |

## Oracle

Source guide: [Oracle L7 System Design Interview Prep](./oracle_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design OCI Object Storage](./oracle_l7_system_design_prep.md#1-design-oci-object-storage) |
| 2 | [Design A Distributed Database](./oracle_l7_system_design_prep.md#2-design-a-distributed-database) |
| 3 | [Design Cloud Block Storage](./oracle_l7_system_design_prep.md#3-design-cloud-block-storage) |
| 4 | [Design A Multi-Tenant SaaS Platform](./oracle_l7_system_design_prep.md#4-design-a-multi-tenant-saas-platform) |
| 5 | [Design OCI IAM / Authorization](./oracle_l7_system_design_prep.md#5-design-oci-iam-authorization) |
| 6 | [Design Cloud Monitoring And Alarming](./oracle_l7_system_design_prep.md#6-design-cloud-monitoring-and-alarming) |
| 7 | [Design Centralized Logging](./oracle_l7_system_design_prep.md#7-design-centralized-logging) |
| 8 | [Design Notification / Alert Delivery](./oracle_l7_system_design_prep.md#8-design-notification-alert-delivery) |
| 9 | [Design Distributed Rate Limiter](./oracle_l7_system_design_prep.md#9-design-distributed-rate-limiter) |
| 10 | [Design Cloud API Gateway](./oracle_l7_system_design_prep.md#10-design-cloud-api-gateway) |
| 11 | [Design Compute Resource Scheduler](./oracle_l7_system_design_prep.md#11-design-compute-resource-scheduler) |
| 12 | [Design Managed Container Service](./oracle_l7_system_design_prep.md#12-design-managed-container-service) |
| 13 | [Design Multi-Region Active-Active Service](./oracle_l7_system_design_prep.md#13-design-multi-region-active-active-service) |
| 14 | [Design Backup And Disaster Recovery Platform](./oracle_l7_system_design_prep.md#14-design-backup-and-disaster-recovery-platform) |
| 15 | [Design Cloud Data Warehouse](./oracle_l7_system_design_prep.md#15-design-cloud-data-warehouse) |
| 16 | [Design Streaming / Event Bus Service](./oracle_l7_system_design_prep.md#16-design-streaming-event-bus-service) |
| 17 | [Design Distributed Cache](./oracle_l7_system_design_prep.md#17-design-distributed-cache) |
| 18 | [Design Billing And Metering](./oracle_l7_system_design_prep.md#18-design-billing-and-metering) |
| 19 | [Design Secrets / Key Management Service](./oracle_l7_system_design_prep.md#19-design-secrets-key-management-service) |
| 20 | [Design Service Discovery And Health Checking](./oracle_l7_system_design_prep.md#20-design-service-discovery-and-health-checking) |

## Snowflake

Source guide: [Snowflake L7 System Design Interview Prep](./snowflake_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design A Cloud Data Warehouse Like Snowflake](./snowflake_l7_system_design_prep.md#1-design-a-cloud-data-warehouse-like-snowflake) |
| 2 | [Design Distributed SQL Query Execution Over Object Storage](./snowflake_l7_system_design_prep.md#2-design-distributed-sql-query-execution-over-object-storage) |
| 3 | [Design Snowflake-Style Virtual Warehouses](./snowflake_l7_system_design_prep.md#3-design-snowflake-style-virtual-warehouses) |
| 4 | [Design A Metadata And Catalog Service](./snowflake_l7_system_design_prep.md#4-design-a-metadata-and-catalog-service) |
| 5 | [Design Micro-Partitions And Partition Pruning](./snowflake_l7_system_design_prep.md#5-design-micro-partitions-and-partition-pruning) |
| 6 | [Design Automatic Clustering / Reclustering](./snowflake_l7_system_design_prep.md#6-design-automatic-clustering-reclustering) |
| 7 | [Design ACID Transactions Over Cloud Object Storage](./snowflake_l7_system_design_prep.md#7-design-acid-transactions-over-cloud-object-storage) |
| 8 | [Design Time Travel](./snowflake_l7_system_design_prep.md#8-design-time-travel) |
| 9 | [Design Zero-Copy Cloning](./snowflake_l7_system_design_prep.md#9-design-zero-copy-cloning) |
| 10 | [Design Batch Data Ingestion From Cloud Storage](./snowflake_l7_system_design_prep.md#10-design-batch-data-ingestion-from-cloud-storage) |
| 11 | [Design Streaming Ingestion / CDC](./snowflake_l7_system_design_prep.md#11-design-streaming-ingestion-cdc) |
| 12 | [Design A Distributed Query Optimizer](./snowflake_l7_system_design_prep.md#12-design-a-distributed-query-optimizer) |
| 13 | [Design Distributed Joins With Skew Handling](./snowflake_l7_system_design_prep.md#13-design-distributed-joins-with-skew-handling) |
| 14 | [Design Caching For Warehouse Compute](./snowflake_l7_system_design_prep.md#14-design-caching-for-warehouse-compute) |
| 15 | [Design Query Result Cache](./snowflake_l7_system_design_prep.md#15-design-query-result-cache) |
| 16 | [Design Multi-Tenant Isolation](./snowflake_l7_system_design_prep.md#16-design-multi-tenant-isolation) |
| 17 | [Design Cross-Region Replication And Failover](./snowflake_l7_system_design_prep.md#17-design-cross-region-replication-and-failover) |
| 18 | [Design Secure Data Sharing](./snowflake_l7_system_design_prep.md#18-design-secure-data-sharing) |
| 19 | [Design Warehouse Workload Management](./snowflake_l7_system_design_prep.md#19-design-warehouse-workload-management) |
| 20 | [Design Cost Metering And Billing](./snowflake_l7_system_design_prep.md#20-design-cost-metering-and-billing) |
| 21 | [Design A Governed Enterprise Agent Data Platform](./snowflake_l7_system_design_prep.md#21-design-a-governed-enterprise-agent-data-platform) |

## Stripe

Source guide: [Stripe L7 System Design Interview Prep](./stripe_l7_system_design_prep.md)

| # | Design question |
| ---: | --- |
| 1 | [Design a Payment Processing System](./stripe_l7_system_design_prep.md#1-design-a-payment-processing-system) |
| 2 | [Design an Idempotency Service](./stripe_l7_system_design_prep.md#2-design-an-idempotency-service) |
| 3 | [Design a Double-Entry Ledger](./stripe_l7_system_design_prep.md#3-design-a-double-entry-ledger) |
| 4 | [Design Webhook Delivery](./stripe_l7_system_design_prep.md#4-design-webhook-delivery) |
| 5 | [Design Refund Processing](./stripe_l7_system_design_prep.md#5-design-refund-processing) |
| 6 | [Design Chargeback / Dispute Management](./stripe_l7_system_design_prep.md#6-design-chargeback-dispute-management) |
| 7 | [Design a Payment Method Vault](./stripe_l7_system_design_prep.md#7-design-a-payment-method-vault) |
| 8 | [Design Checkout](./stripe_l7_system_design_prep.md#8-design-checkout) |
| 9 | [Design a Fraud Detection System](./stripe_l7_system_design_prep.md#9-design-a-fraud-detection-system) |
| 10 | [Design Merchant Onboarding / KYC](./stripe_l7_system_design_prep.md#10-design-merchant-onboarding-kyc) |
| 11 | [Design Payouts](./stripe_l7_system_design_prep.md#11-design-payouts) |
| 12 | [Design Marketplace Split Payments](./stripe_l7_system_design_prep.md#12-design-marketplace-split-payments) |
| 13 | [Design Subscription Billing](./stripe_l7_system_design_prep.md#13-design-subscription-billing) |
| 14 | [Design Invoicing](./stripe_l7_system_design_prep.md#14-design-invoicing) |
| 15 | [Design Global Currency and FX Support](./stripe_l7_system_design_prep.md#15-design-global-currency-and-fx-support) |
| 16 | [Design a Distributed Rate Limiter](./stripe_l7_system_design_prep.md#16-design-a-distributed-rate-limiter) |
| 17 | [Design API Request Logging and Audit Trails](./stripe_l7_system_design_prep.md#17-design-api-request-logging-and-audit-trails) |
| 18 | [Design Real-Time Payment Status Tracking](./stripe_l7_system_design_prep.md#18-design-real-time-payment-status-tracking) |
| 19 | [Design Reconciliation](./stripe_l7_system_design_prep.md#19-design-reconciliation) |
| 20 | [Design a Workflow Engine for Payments](./stripe_l7_system_design_prep.md#20-design-a-workflow-engine-for-payments) |
