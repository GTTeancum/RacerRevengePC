# Actual Application250 timing publication for future Pause scheduling

Source-only increment against0573c6a. No builds/tests/syntax checks or execution.
The new API is inactive. Existing UI and world timing behavior is unchanged.

## Existing authority and missing field

Application already owns the genuine HostMonotonicClock50ID and source230
publication. Its animation clock view exposes230/11C/120. Existing frontend
acceptedElapsed measures nanoseconds minus explicit cold intervals and feeds
frontend update directly; it has not represented source234/250. Camera.frameDelta
is the prepared world delta. Neither is silently relabeled as Application250.

## Source port

-13FC40 initializes previous234=0;13FCBC initializes elapsed250=0.
-140AC0 update checks234 at140B10. If zero,140B1C calls reset140D70, which
  samples the actual timer once into BOTH234 and230 at140E38/3C. First elapsed
  therefore becomes zero, including the original sentinel behavior at stamp0.
-Otherwise the same actual backend samples230 at140BE0.140BEC subtracts the
 32-bit words with unsigned wrapping arithmetic.140BF0..140C1C converts to
 float: the top-bit-set branch converts `(delta>>1)|(delta&1)` and adds that
 float to itself. The native helper preserves this branch and arithmetic order.
-140C30 multiplies by exact float bits3A83126F.140C50 stores250 in the branch
 delay slot BEFORE140C54 caps local f20 to bits3D75C28F (.06). The publication
 retains uncapped250; it does not alter world/update factor23C.
-140D30 reloads latest230 AFTER callbacks into234. A sampler invoked during
 callbacks can change230, so commit deliberately does not cache the entry stamp.

`Application::beginUpdateClock140AC0`, `commitUpdateClock140D30`, and
`resetUpdateClock140D70` accept no delta/timestamp/frame-ready argument. Begin
and reset sample the existing Application backend. A serial in-progress guard
rejects nested begin, reset during an update and unmatched commit. A read-only
clock view returns250/230/234 and a native publication sequence only inside an
active begin/commit interval. Native sequence is not an original guest field.

## Exact integration gates

Root must wire begin once around the actual outer Application/UI/Pause update,
not around every fixed world step. Invoke real source parent callbacks and the
last-camera-qualified1B9220 before commit. Existing acceptedElapsed accounting
is not replaced or bypassed by this inactive capability. Source pause/debug
policy, first update/reset boundary, and camera index/count authority still need
real scheduler joins. No independent clock epoch, pause timer override, RNG
mutation, extra frame, or source callback completion receipt is introduced.

Pause+30's source transition is .25 -> max(remaining-Application250,0), then
exact0 -> -1 on a later eligible callback; negative stays. However1B9220 also
performs original backdrop/polygon geometry and other lifecycle work before
that transition. This patch does not isolate/activate a timer-only shortcut.
