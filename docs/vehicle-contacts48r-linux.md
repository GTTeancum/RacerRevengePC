# 48R bounded vehicle contact recovery

This packet closes a real intervening vehicle stage without pretending the
whole vehicle scheduler is complete. The exact219810 map places219420 contact
handling after detection and before terrain preparation, engine, gravity,
aerodynamics and spring response. The handler computes body-to-body contacts,
invokes both authenticated partner-list callbacks, and appends spring records.

## Delivered scope

`runVehicleContacts48R` supports explicit supplied candidate order, bounds tags1
and6, cached or dirty-parentless scenes, and known PhysicsRigidBody callback
165430. Scene/body indices express shared identities; self-body calls and shared
position/inverse scenes coalesce correctly. Unknown reached branches return an
explicit status and commit no destination/result changes.

The callback uses ten fixed partner slots and supports its original full-before-
dedup ordering. A separate165420 reset clears only count; the caller invokes it
after main-body integration at219E94. The handler does not clear this list before
response. Reusing fixed storage avoids per-frame growth or collection and keeps
inactive slot bytes intact.

`VehicleContactStatics48R` makes original initialization flags and mutable scale
words explicit. A zero first flag initializes434718 to0.5; a preinitialized flag
retains its supplied scale. The unused second static word remains opaque except
for its authentic initialization. Cached candidate radius is not consumed;
transformed half extents and the separate own-radius input drive the overlap.

Position-scene up is used exactly, without normalization. Dot order, strict
threshold, reciprocal-sqrt multiplication, inverse matrix/translation order,
lazy fields, callbacks-before-append and stale96-byte record contents are all
observable in the original fixtures. The original handler's residual V0 is not
a semantic Boolean and is not exposed as a fabricated success value.

## Defensive native boundaries

Full handler body identities must be nonzero when reached because record.other44
reserves zero for an absent/static body. The standalone callback itself accepts
zero opaque payloads exactly as its original instructions do. Input identities
are not host or guest pointers.

Negative spring capacity is a deliberate native48N append exclusion. Ten
original cases naturally saturate after callbacks; the typed handler rejects
those cases transactionally instead of claiming bitwise completion. Negative
body-list counts and negative generated-record counts likewise remain explicit
unsafe-state boundaries. Unsupported callbacks, bounds or scene parents do not
receive invented return values.

Three zero-lateral cases are supported under the existing finite-max VRSQRT
reference model. They are labeled separately; this is not certification of all
physical EE/VU exceptions, flags or denormal behavior. Caller rounding is
round-to-nearest, with individually rounded binary32 and no contraction. Ignored
cached-radius and copied-only nonfinite values are not made into arithmetic.

## Reproduce

Use a fresh output directory outside the source tree:

    python3 tools/test_native_vehicle_contact48r.py --stack-usage --output-dir /tmp/rr48r-release
    python3 tools/test_native_vehicle_contact48r.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48r-sanitized

The runner is serial, records source/fixture hashes and does not install or
fetch software. ASan/UBSan are checked; the explicit no-leak-check option means
LeakSanitizer is not verified on this ptrace-restricted host. Python/JSON and the
instruction executor are validation tooling, not runtime dependencies.

CMake's optional entry is RR_NATIVE_VEHICLE_CONTACT_ONLY with its matching
RR_NATIVE_VEHICLE_CONTACT_SANITIZERS option. NativePc48R.cmake and
Build-NativePc48R.ps1 retain the Windows target. CMake, Windows, Xbox, rendering
and full gameplay execution remain untested in this cloud environment.

## Evidence and remaining work

Read vehicle-contact-map48r.md for caller order and exact dependencies,
vehicle-contact-proof48r.md for independent original execution,
vehicle-contact48r.md for the runtime contract, vehicle-contact-tests48r.md
for differential/safety coverage and vehicle-contact-review48r.md for review.
The separate helper is documented in body-contact-list48r.md.

Before a vehicle step can be called complete, recover first-substep122350 world
enumeration and live candidate ownership, the1B1800 spatial/prequery phase,
optional detectors/bodies, remaining helper/callback paths, and late scene
feedback. Existing48Q detector and48L responses must not be connected by skipping
these stages. Existing ELF data permits further bounded source/synthetic work;
RAM remains outside the authorized scope. No hardware FPS or whole-game memory
budget is inferred from the headless checks.

## Final aggregate observation

All eight retained/new suite groups passed 4,292,718 checks in each release
and ASan/UBSan configuration, with matching final input hashes. There are zero
new warnings; the three existing terrain warnings remain. The new handler and
callback/reset tests reject all21 compiled negative controls.

The final two-candidate host workload observed507.106 ns median and620.684 ns
maximum batch mean. Scratch and destination backing each use2,284 bytes, with
source storage additional. GCC reports an864-byte handler frame, not total live
stack. The complete test process peaked at18572 KiB RSS, including fixture JSON
and diagnostics. These measurements do not establish Xbox FPS or a whole-game
RAM/frame budget. Final reports are under evidence/nativeVehicleContact48R-linux-20261006.
