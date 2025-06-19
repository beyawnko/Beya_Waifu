# Memory: RealCUGAN multi-GPU refresh
Implemented runtime updates for RealCUGAN multi-GPU settings. The checkbox slot now refreshes the processor and logs state changes. GPU add/remove slots prompt for per-GPU thread count and tile size and persist these settings. Model selection triggers a settings reload so running jobs pick up the change. Related memories:
- [Notification sound implemented](2025-06-18T211603Z-sound-notification.md)
- [Compatibility test progress updates](2025-06-18T232724Z-compat-test-progress-bar.md)
