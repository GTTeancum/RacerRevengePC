# Actual consumed-launch race constructor ownership

Base55d188e. Static inspection only; no builds, tests, syntax checks, decoder
execution or gameplay. The new factory has no runtime caller.

Application.constructConsumedRaceCold1A65C0 requires the retained result of an
actual Course consumption, the same Application profile, the exact next
session generation and no existing race prefix. Native allocation and shared
control-block allocation finish before counting. The same Application Entity
authority is preflighted, and counter overflow is refused as a native safety
admission before any source mutation.

The reached prefix uses original141300 with A1=1, enrolls that real counted
Entity, then invokes the same profile's private1A66CC writes at the source
constructor position. It retains the constructor's typed numeric fields and
known flag bits. Original1A6808 stores Entity.descendants24=FFFFFFFF; this is
not replaced with the generic detached zero invariant. Constructor1B0 bits0/1
and1B1 bits2..7 remain unrepresented. Source child-null stores acquire no native
child owners. Unwritten B8/BC/C0 are not modeled as null.

Native backing initializes unpublished numeric storage before the count; no
callback can observe that storage until the factory returns. There is no raw
guest allocation, callable source vtable, copied heap layout or second racer
control state. Existing gameplay HUD blink ownership is not switched: the new
constructor remains inactive until its real load/lifetime join is available.

## Explicit native failure policy

rollbackNativeConstructionFailure only accepts this untouched detached
constructor prefix, its actual canonical enrollment, reference1 and the exact
source descendant-mask value for its reached stage. It retires the count and
backing acquired by this native constructor. This is not1A6840,142590, renderer
cleanup, world destruction or a claim that uninitialized child fields are safe
to read. No child adoption API exists. Failure preserves the owner; destroying
an unrolled-back counted owner terminates rather than dropping its count.

Profile writes remain reached after rollback, and the consumed launch remains
claimed after the first actual count, preventing replay against that launch.
Allocation/admission failures before counting leave the launch unclaimed. If
the profile operation refuses after counting, only the native counted prefix
is rolled back before returning failure; no source race destructor is invoked.

## Remaining normal lifecycle prerequisites

Application28C is not published and no child or scheduler callback is enabled.
Normal1A6840 remains unavailable. Its source-order release requires real
B8/BC, renderer reset/direct-upload capability retirement, C0,19C/1A0,
FIFO443848, Pause100/result104, world74, fonts328, profile branches, resolver
scratch and finally base Entity retirement. World74's constructor-only native
rollback cannot certify its loaded-world1B2920/26FD80 destructor. Audio cold
constructors currently admit only empty member lists and no voice bindings.

The future caller must place this constructor before the six race audio
Entities, then construct world74 before its seventh group and before racers.
The current RA counted HUD/static/roster staging needs reordering at that early
boundary. Merely inserting a call at activateColdRace or Camera Session is too
late. No driver activation is included in this patch.
