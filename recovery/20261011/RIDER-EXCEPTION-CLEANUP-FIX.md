# Independent cleanup-order review

Verdict: the staged three-file correction is sound on source review. It fixes the demonstrated host/coordinator ownership inversion without changing the stall gate or pretending run03 cleaned up successfully.

`runSession50DU` currently calls `renderer.close()` before `originalMain.closeUi()`. In an active Rider session, coordinator `close()` must first call `retireRider()` through that exact live renderer. Retirement requires the initialized source-RGBA renderer and current context. The host pre-close removes that capability, explaining the observed repeated bank-cleanup failure and terminate during the original stall exception.

The candidate's read-only `borrowsRenderer(renderer)` compares the retained exact pointer. It does not infer lifetime from visible route, activation success or `ownsResources()`. `activate()` stores that pointer before operations that may fail; `close()` clears it only after renderer close succeeds. Thus the query also correctly covers failed activation and a retained borrow after partial cleanup.

When the pointer matches, the host now lets coordinator close perform source-owner retirement, renderer close, session/GPU release and light-manager release in their established order. When no coordinator borrow exists, the old host renderer-first cleanup remains for renderer creation failure before activation. Existing cleanup retry/terminate protections remain unchanged. After coordinator success, the later generic renderer check is harmless/idempotent.

Reviewed exact staged diff and relevant active implementation, read-only. No builds, tests, syntax checks, project parser/decoder/importer, native program or guest execution. No active edits. The change is independent of renderer performance.

Reviewed patch SHA256: `13301499767d51192ef3a1cc12eb8c74b794c0c589910b30bea2d7d6a75cd366`.

Exact preimages are recorded in `BASE-SHA256SUMS`; candidate and report identities are recorded in `SHA256SUMS`. A successful exceptional cleanup remains untested.
