# Native Damage HUD observation and complete leaf

Source-only candidate against eed335c. No build, test, syntax, sanitizer,
asset-decoder execution, gameplay or screenshot inspection was performed.

## Adaptation boundary

The original Player allocation owns nine proximity slots10A8..10C8. An original
AI allocation cannot hold them. This implementation does not pretend otherwise:
OriginalDamageObservation50LA is a separate native HUD observation owner. It
runs the original1A1EB0 scalar query on actual manager-order native AI records,
then passes its nine results and real health/tint inputs into the complete
original18F680 Damage leaf. It creates no Player, dispatches no Player controls,
and changes no AI/control/physics fields. Actual scene world-cache refreshes
are the existing28B180-compatible canonical getter effects.

The owner binds the exact live Camera Session, canonical Application entity and
material owners, actual generation and real view0 manager identity. Each capture
requires a published camera frame, reads current state40 and visits real Bank
slots serially outside other Bank leases, and rechecks the publication/identity.
Self uses local scene E0/E4/E8 forward, NOT cached heading or world forward.
Positions are actual scene world100 translation for self and each state0 rival.
The scan results exist only in that synchronous frame value, not in AI memory.

## Original query and order

The included source words are the previously retained authenticated static source
packet, not regenerated instructions.1A1EB0 clears all9 outputs to-1 each scan,
visits manager indices ascending, skips self5C and nonzero state40. It uses
original172DF0 directionAngle (including its X-zero branch), exact angle words
3F96CBE4/401D1463, and squared-distance limits48EF4200/481C4000/46AFC800.
It preserves conditional angle wrapping, strict sector/distance boundaries,
branch-delay outer/middle/near assignments and prior-slot-dependent clearing.
It never sorts for nearest rival or invents a front-sector marker.

OriginalDamage50LA now prepares all original32 texture objects' selected draws:
base3, both health full-segment loops, partial segments, then all nine occupancy
markers. Health uses1-min(max(7D4,7DC),1) and1-min(max(7D0,7D8),1), admitting
finite nonnegative values above1 without modifying simulation state. Real B9C
bit1 selects repair tint. Atlas coordinates, dimensions, RGB/alpha setters,
layout and packet depth remain the retained original source state.

## Cold and renderer join

Use original_main_entry50KG::DamageColdResult50LA. Call existing
Application.stageDamageNativeCold50LA(root,generation,result) before final RA
material capture, then finishDamageNativeCold50LA after final population beside
TimeStats/SpeedBoost. This stages genuine32 ScreenMT refs and one original
ui/textures/dmgradar.pst. Its missing PSM is an authenticated original fallback;
never add a synthetic dmgradar.psm. No font is needed.

Renderer strongly retains OriginalDamage50LA until checked GL retirement, as
with TimeStats. Damage has no RaceDisplayLifetime lease. Its detached entity,
canonical context/Video/material refs cannot create the PositionStats display
lease cycle. Root releases its result after GPU cleanup, before canonical domain
retirement. No renderer code or50DU activation is changed in this patch.

Root includes tests/native/original_damage_observation50la.h, binds
observation.bind(camera,result.owner,diagnostic), captures OriginalDamageNativeFrame50LA,
then calls result.owner->prepareNativeDisplay(frame,result.object). Renderer
bindOriginalDamage receives the same owner; frame.originalDamage identifies it.
The public draw uses32 quads/32 packets maximum, material0/image0 only, source
packet depth16777214. Native getters match TimeStats. Invalidations occur before
accepted world ticks; a requested invalid frame must reject, not silently hide.

The source HUD sibling order is Damage(C8), PositionStats(CC), TimeStats(D0),
SpeedBoost(C4):1AAE38/48/58/68 append by141960. That helper appends after walking
next14;141D70 draws firstChild18 through next14. Preserve this component order
without claiming that the full retail HUD parent/scheduler has been recreated.

## Other ordinary HUD audit

Rank, active count, lap/time/lead, route/rival markers, speed, heat and boost are
already connected to real bank/manager fields with original numeric formatting.
KO display reads genuine countCD0. Separate source recovery establishes ALL8
humanCount34=0 clears D30 attribution for AI contacts, so original KO zero is
expected; no artificial increments are warranted. Missing destructive gameplay
continuation is separate from this presentation work.

PositionStats191AF0 texture demand prefix remains unjoined, although shared
stats/font resources are demanded by TimeStats and genuinely uploaded. This is
a source-lifecycle bookkeeping gap rather than a missing visible component.
Full retail parent toggle382C00/global renderer traversal remains outside the
explicit native scheduling policy; this patch does not manufacture that tree.
