# Retained actual Course consumption for the next native session

This bounded native handoff extends the existing `ColdLoadRequest` rather than
claiming a new original callback. The existing publish path follows actual
`takeCourseLaunch`, queue 9 completion and renderer drain. The outer driver
consumes only after the outgoing frontend/session returns and destroys its
local owners. Existing `consume` behavior remains unchanged.

`consumeForNextSession` allocates unpublished native backing before calling that
same consume operation. Failure leaves the request and output owner unchanged.
Only successful consumption publishes a noncopyable owner retaining the same
profile, validated Selection/Target and exactly outgoing generation + 1. The
existing generation convention is sessionIndex + 1. Maximum generation is
rejected before consumption. The next runSession checks exact profile identity,
current profile snapshot, loaded track/rider and generation before cold setup.
The main loop releases the owner after that one session returns; restart does
not synthesize a replacement. Exceptions release the owner through normal
stack unwinding. Direct startup has no consumed Course owner.

This proves only the actual native launch handoff. It does not construct or
publish Application28C, increment Entity count, attach a race, enable Pause,
certify a scheduler callback, or make a destination load ready.

## Genuine constructor placement still required

The RA driver currently stages counted SpeedBoost/TimeStats/Damage before
`ra_native_session50LA::Join::construct`. That Join prepares static world,
track-particle manager and static-model constructors before the eight racer
constructors. Both the later `activateColdRace` and Camera Session creation
occur too late to insert original race construction in source order.

Recovered load order is six race110..124 group Entities at1A6B80, world74
allocation/construction at1A6BC0/1A6BD4, racers via1A9240 at1A6C88, race10C
via1A39F0 at1A6CA4, then B8/BC and C0 at their later load sites. World74 itself
constructs its counted World4D8 group. Constructor1A65C0 leaves B8/BC/C0
unwritten. A future connected stage must preallocate native backing and admit
actual retirement dependencies before the first counted construction; it must
not pretend constructor-only full cleanup can skip unknown original fields.

The existing closeWindow also begins terminal display retirement before its
spatial/roster closes. Authentic race cleanup needs its earlier non-retiring
renderer reset/direct-upload-binding retirement and source-ordered child
release before terminal display close. The real FIFO1A6948 leaf is separately
owned; queued work cannot be certified by this launch handoff.

Static source inspection only. No build, test, executable decoder or gameplay
run was performed for this patch.
