# Native Main / Options entry integration

Source-only checkpoint, 2026-10-10 18:27 UTC. No build, syntax check, or software test was run for these changes.

The original UI entry now owns OriginalUiFrontend50KG and supplies authenticated Main, Options, localization, font, popup and overlay archives. Its native host input reaches the real Main/Options controller and coordinator. The source profile, application RNG and audio transport remain shared across fresh screen-manager lifetimes.

Each accepted update advances PCM once; measured cold preparation intervals are excluded from update and material elapsed time. The outgoing frame is rendered, submitted, swapped and optionally captured before deferred screen activation. Capture receipts now identify actual screen, page and switch count. Unsupported destinations remain explicit rejections.

Cleanup closes renderer borrowers before coordinator texture banks while the context is retained. One cleanup retry is allowed; unresolved cleanup terminates before owner teardown instead of unwinding through dangling borrows.

Only Main4/1 to Options12/1 and Options Back to Main are wired. Game Options subpages, Rider selection, and gameplay entry are not claimed by this checkpoint. The latest actual Linux result remains the earlier f89a3c5 Main opening/idle capture, documented in MAIN-OBSERVATION-1755.md. These new paths require the next centralized focused test; earliest normal start is 2026-10-10 18:56:57.403928 UTC.
