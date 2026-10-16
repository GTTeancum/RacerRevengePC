# Explicit native consumed-race retirement

`Application::closeNativeConsumedRace` composes real acquired ownership cleanup
for the same consumed-launch race. It is inactive and does not certify original
race virtual destructor or its unrecovered callbacks.

The actual scheduler must detach the race before this operation. The canonical
race identity/reference and its exact represented six-audio-child chain are
checked; unknown children or unsupported flags reject. The actual Application
registration, profile, display thread and audio epoch must match. No synthetic
parent receipt is created.

Order:

1. Retire actual native audio membership only after every real mixer voice stops.
   If world cleanup subsequently rejects a live borrow, audio remains honestly
   retired and the operation is retryable; callers must not resume that session.
2. Close the same retained world through `closeNativeLoadedWorld`. Its real GPU,
   camera, runtime, navigation and topology gates remain authoritative. External
   B8/BC/C0 and renderer/runtime owners must release their actual borrows first.
3. Use the existing native group unwind kernel to detach and release each actual
   six-child owner. A failed release after detach preserves the detached child
   for the next attempt; only completed releases clear tracked ownership.
4. Retire the actual empty FIFO and release the display borrow with the existing
   native load-prefix cleanup. This does not invent a drain or clear persistent
   GL images. Actual remaining canonical pins may reject this phase.
5. Release the detached race count/reference and retained profile/material
   context/domain through the existing native backing cleanup kernel. Reached
   original scalar/profile side effects are retained.

Repeated successful close succeeds. Every failed stage retains the remaining
actual owner. Existing failure rollback APIs stay available; the composed normal
native close deliberately reuses their ownership mechanics without relabeling
any operation as source destructor completion. No Application28C publication,
Pause scheduler attachment, input activation or driver call is added.

Dependency: native counted-audio retirement and explicit native loaded-world
retirement. Source review only; no builds or runtime checks were performed.
