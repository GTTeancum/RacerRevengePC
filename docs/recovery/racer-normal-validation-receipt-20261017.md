Per-submit racer normal validation receipt (untested)

Base: 02e69a0989eaabd6f4d10283c9a562a755120c9e
Patch: rr-racer-normal-receipt-02e69a0-20261017T0017.patch
Scope: platform/linux/native/vehicle_window_gl50ds.cpp only.
Independent source review and integration source review accepted. Untested.

Change
retainedMesh already checks all component mesh/material/source-color identities
and then validates every live normal with std::hypot. The CPU preflight revisits
the same batches in validateOpponents, copyNativeGcLights and, when enabled,
prepareSpecularFrame50LA. A stack-only receipt now reuses the last successful
normal scan for that exact batch, component count, and each component's vertex
pointer/count and normal pointer/count. Every mesh/material/provenance check
still runs before the receipt match. No normal arithmetic, tolerance, geometry,
lighting, clipping, GL command, or error check changes.

First scan and diagnostics
- Empty receipts cannot match: completedBatch starts null. accept() is called
  only after validateRacerNormals returns true, including the original component
  count, pointer/count-presence, exact vertex-count, finite/unit/zero checks.
- Zero normal streams still pass through their original presence/count checks
  before any receipt is accepted. There is no inference from an empty count.
- A mismatch clears completion and performs the original full scan. Failure
  returns at the same retainedMesh site before any receipt can be accepted.
- With opponents present, validateOpponents still performs the first scan in
  original racer order. With zero opponents, copyNativeGcLights performs the
  first player scan, as before.
- Original validation call order is unchanged. retainedMesh still resets
  diagnostic component/material immediately before its normal step. The
  original successful normal scan makes no diagnostic writes, so a receipt
  hit preserves exactly that successful diagnostic state. Initial rejection
  precedence and concrete diagnostic failures are retained.

Lifetime and callback boundary
- Receipts exist only in an explicit submit-local block covering the three
  owned CPU preflight functions. They are passed to internal helpers, never
  placed in renderer state, and destroyed on every exit from that block.
- The block neither calls host/currentness/GL callbacks nor writes the live
  normal streams. Material, cable, dynamic-world, animation-frame and selected
  light checks are owned synchronous read-only validators.
- Camera conversion/specular UV preparation is callback-free. Its writes are
  to private UV/workspace fields and model distance/fog flags; sourceNormals
  is not a writable target. OriginalRacerSpecular50LA also requires nonoverlap.
- ready() deliberately receives no receipt and still performs its original
  full scan after caller currentness/texture callbacks. Nothing is reused
  across that external callback boundary, across frames, or during drawing.
- The public renderer contract forbids concurrent/reentrant calls. Actual
  player normal vectors are allocated in VehicleDrawAssets50CR::load, exposed
  by batchInput, and written by buildVehicleDrawBatch50CO before submit.
  Opponent DrawOwner owns separate cold normal vectors and only prepare()
  writes them. The native loop prepares these outputs before renderer.submit
  and does no further preparation until that synchronous call returns.

Source evidence (paths relative to the base repository)
- platform/linux/native/vehicle_window_gl50ds.cpp: retainedMesh,
  validateRacerNormals, validateOpponents, copyNativeGcLights,
  prepareSpecularFrame50LA, ready, submit.
- platform/linux/native/vehicle_window_gl50ds.h: class-level single-threaded,
  no-reentrancy contract around line 941.
- src/assets/vehicle_draw_assets50cr.cpp: Component normalStaging storage,
  cold resize around line 171, batchInput normal pointer publication around223.
- tests/native/native_racer_draw50hn_fixture.h: DrawOwner::initialize/prepare.
- tests/native/terrain_interactive_drive50du.cpp: player build around7457,
  opponent DrawOwner preparation around7846, synchronous submit around7966.
- src/game/original_racer_model_packets50la.h: prepareDistanceFlags241280
  mutates only model bounds/distance/flags and packet distanceFlags20.
- src/game/original_racer_specular50la.h: callback-free/nonoverlap contract
  and private map-2 UV/workspace writes.

Expected work reduction, not a measurement
With opponents and source specular enabled, three preflight normal scans per
racer become one; the player's separate ready() scan is retained. Thus two
duplicate std::hypot loops per submitted racer are avoided when normals exist.
With no source normals, only repeated structural scan work is avoided; with
no specular, the source-preflight reuse opportunity is smaller. No frame-rate
or wall-time improvement has been measured.

No builds, tests, syntax checks, gameplay runs, benchmarks, or instrumentation
execution were performed. Source inspection only; patch is untested.
