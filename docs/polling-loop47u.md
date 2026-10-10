# 47U read-only lead: timer conversion inside a VBlank polling loop

The 47K race profile records about 123k dispatcher iterations per VBlank.
Several common entries belong to the path below. These are source findings,
not proof of a safe shortcut or a measured speedup. No guest code was edited.

- 0x25AE04 tests the saved floating-point timestamp in f21. It calls 0x25AFB0
  when it needs an initial timestamp; otherwise it advances to 0x25AE2C.
- 0x25AE2C calls 0x25AFB0 again, subtracts f21 and compares against f20. Its
  timeout path writes GS CSR 0x12001000 and IMR 0x12001010.
- 0x25AE68 reads RAM 0x442D08 and loops to 0x25AE04 while the signed value is
  nonpositive. This is consistent with the existing guest-VBlank observation,
  but caller/context validation is still required.
- 0x25AFB0 reads the 16-bit timer at 0x10000000 and the previous value at
  gp-0x77B0. It reads and clears 0x435368 and updates the 64-bit accumulator
  at 0x435370. The wrap path and pending timer/IRQ changes matter.
- 0x25B04C stores the previous timer value, calls 0x2EAE10 with that 64-bit
  accumulator, then multiplies the float result by constant bits 0x3AE38E39.
  This connects frequent software conversion helpers to time polling.

A replacement cannot simply wait for VBlank: it must preserve timer wrap,
accumulator writes, floating-point results, timeout-triggered GS writes, the
existing dispatch cycle charges, interrupt entry/context, and yields. A useful
next experiment is an isolated replay of this loop with captured entry state,
then a proven skip only over iterations whose externally visible effects can
be reconstructed exactly. First validate the original ELF range and registry
ownership rather than assuming a filename identifies the executed function.

For example, sampled entry 0x251C34 is a continuation in FUN_00251B10. Its full
body can continue into device work; the shorter entry_00251C04 file alone would
mislead attribution. The profile's 26.9% inclusive time at this PC is not evidence
that its first conditional branch is expensive. Likewise 0x267C48 calls other
functions and its sample count is only 12, making a precise cost claim weak.

These hypotheses accompany the diagnostic and scheduler experiments. They do
not supersede exact saved-state comparisons or establish Xbox performance.
