# Combined roster and host UI integration

The combined Linux source passed the focused 2026-10-09 17:18 UTC Clegg/menu capture check. Actual Main selection, one expected cold session change, pause/resume with preserved audio ownership, 20 seconds of gameplay capture, final Main, and normal quit succeeded. The headless pause blit correction is included and passed this flow. See [current validation](current-validation.md) for exact source identity and limits.

The implementation retains table-driven eight-racer selection, immutable loaded identities, audio choices applied on explicit Start/Restart, unchanged menu page IDs, native CB8/CB0 timing, and existing audio group ownership. Only Clegg was selected as the human during this check. The remaining human choices, Audio Options edits/application, confirmation paths, and full-race behavior on this build still need focused validation. Results presentation remains separately staged. Other-track work is outside this tested source batch.
