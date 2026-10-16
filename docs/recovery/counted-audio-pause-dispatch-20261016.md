# Counted audio-group pause dispatch

Source-only native integration; no build, syntax, gameplay or decoder execution.

## Reached source and owner

The retained `rr-clock-full-static-fields.txt` words at1ACF50..1ACF64 load
race74, then world4D8, then tail-dispatch that group's virtual50. This override
**does not call142040 on the race or its six audio children**. A separate direct
base142040 call would have its own traversal semantics and cannot replace this
route.

2356A0 calls142040 before inspecting state44.142050..14206C replaces only
flags29 bit2 with the low requested bit.142070..142078 checks descendants24
mask20; reached children must be enabled, not hidden (bit3), and have their own
mask20 bit20 before virtual50 dispatch.2356BC..2356E8 skips the audio command
when old state44 and the request have equal Boolean values. Otherwise2356EC
stores the request and2356F8 calls26D380 for the actual allocated group40.

## Concrete implementation

`RaceAudioGroups50IC::pauseColdGroup2356A0` is a private leaf available only to
existing actual race/world friends. It selects this owner's real counted group,
checks the same Application registry enrollment and soundGroups434A10 slot,
thread/mixer epoch, source session/key, and complete canonical topology.
Its current bounded admission requires the genuine constructor's empty child
and logical-member topology; no child virtual callback is guessed. Native
provider voices are separately represented by the existing binding ledger.

Before any source mutation it preflights the same mixer slot identities and
control-change capacity as the existing audio suffix. It then writes the real
Entity bit2, including equal-state requests, and executes the audio suffix on
the same state44 and same bound voices. Group pause preserves sample cursors.
The suffix retains its previous Boolean-change-only command behavior. Existing
public `pauseWorld2356EC` remains an audio-suffix-only API for its legacy caller.

## Remaining integration

There is no driver call or completed Pause callback. The actual retained race
must authenticate its same world74 and world4D8 ownership before calling this
leaf from its1ACF50 override. Parent attachment, actual opening device suffix,
source lifecycle clock and scheduler admission remain separate. Nonempty group
Entity children or logical members are rejected until their concrete source
callbacks are represented. General141630/1415A0 traversal is not supplied by
this patch; renderer visibility toggles do not stand in for these operations.
