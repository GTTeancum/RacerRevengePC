# Reserve private RA fog placement observations once

UNTESTED source change. No build, smoke, syntax check or FPS measurement.

Binding::prepare in tests/native/ra_static_fog_binding50la.h now reserves exactly
world_->placements_.size() after its existing initial camera/currentness checks.
The unchanged loop emits one observation per retained placement, in original
order, using the same arithmetic, source checks and push_back operation.

The reached RA render preparation calls prepareNativeStaticFogObservations each
frame. The aggregate discards the previous binding and constructs a fresh private
one. Without a reserve, the observation vector can grow geometrically and copy
its existing records several times during each preparation. Reserving its known
final demand avoids those growth reallocations without retaining state across
publications or borrowing records beyond their owners.

The aggregate publishes only after prepare succeeds and catches allocation
exceptions. Reserve can report allocation failure earlier, before per-row
validation, but still takes the existing unpublished allocation-failure path.
All initial/final currentness checks and failure publication rules remain.
Actual allocation counts, timing and FPS impact have not been measured.

This source change is later than the b4c303e Linux smoke and Windows tester ZIP.
It is not included in that delivered executable.
