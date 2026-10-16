# Consumed RA driver staging and native unwind

The actual driver now declares its same Video/display owner, mixer-owning live
audio object and renderer-consumed model globals before RA counted query Storage.
The existing Video-open lambda moves with those owners; its request and existing
call sites remain unchanged. Uncounted RA archive decode/authoring/lighting remains
in place. Actual query Storage reservation remains immediately afterward on the
current route. No unrelated resource prewarm is moved.

Between those two stages, a compiled but deliberately uninvoked initializer
admits only the actual consumed RA launch, ALL8 and original-camera route. Its
ordered effects are:

1. Open the SAME Video, prepare actual AppStart/material directory and canonical
   construction context.
2. Initialize the SAME live-audio mixer storage, without groups/providers.
3. Construct the actual consumed race and publish the actual Application28C slot.
4. Reach actual1A6AA0 renderer/distance/FIFO and1AA5A0 resource population.
5. Construct/load/attach the six canonical race audio groups in source order.
6. Construct the SAME retained RA world, interleaving its seventh audio group.
7. Resume the existing actual counted query Storage reservation. Existing later
   audio preparation configures all seven groups once and binds real providers.

The driver still does not invoke this initializer. Root activation requires the
completed native RA Session/camera and real Pause parent/load scope; there is no
fabricated load-complete flag. The source prefix is not a full slot30 receipt.
The consumed token parameter is mutable because the genuine constructor claims
that same one-shot launch; existing matching and next-session provenance remains.

## Failure and retirement

The early scoped stage owns the real race across every reached failure. It is
declared after live audio and before RA/runtime/GPU borrowers. On final unwind,
those later owners release first; then the stage closes actual live audio and
calls checked native consumed-race retirement. Published28C retains partial
ownership until successful real retirement. Rejection terminates with ownership
preserved rather than silently dropping counted nodes. No active voice is cleared
merely to satisfy a group-empty gate: live-audio close performs its actual normal
stop/output-close operation before group retirement.

If failure precedes race allocation, the initialized audio storage still closes.
If it follows race allocation/publication, the same retained race owns all
partially acquired resources. Existing checked world cleanup handles acquired
Storage/scene/GPU borrows; group/FIFO/race cleanup preserves retry state. Ordinary
Pause shutdown must first restore its actual state, disable/detach its real graph
and release its GPU/font borrows. This guard cannot invent those effects and will
refuse an attached or unsupported race state.

## Smallest remaining activation boundary

Late actual `raSession->construct` and prepared CameraSession/display/bank owners
can authenticate the native loaded route. The genuine Pause must load before
FINAL collection seal, then its real sibling/root attachment and concrete
open/Continue effects must be joined. Unsupported result/restart functionality
stays unavailable. Native lifetime completion is distinct from full original
slot30/destructor callback equivalence. No tests or executable checks were run.
