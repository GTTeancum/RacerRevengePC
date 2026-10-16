# Speed/Boost race blink: existing Application pause join

Source-only, no implementation.

Original Application constructor13FB30 clears actual this+11C at13FC4C (retained rr-clock-13fb80-fields.txt: AE00011C), timestamp230 at13FC44 and pending120 at13FC50. Original race1A7B40 copies f12 to f20 at1A7B74; nonzero actual Application11C at1A7B94 takes Entity141ED0 and returns before D8/E8 update. Zero proceeds to the recovered0.25-second accumulator/toggle loop.

Existing native authority is tests/native/original_ui_main_entry50kg.h Application.animationClockOwner_, allocated once and retained across sessions. OriginalApplicationAnimationClock50KG.value_.pause11C initializes0. Application::animationPaused() reads this actual field, and setExplicitAnimationPause writes0/1. Accepted explicit menu transitions in terrain_interactive_drive50du.cpp6057 and6069 write it; focus/minimize/recovery/controller-loss suppression does not. It is therefore wrong to synthesize this gate from !advanceAnimations, frame activity, camera visibility or focus. FrameTimingState49I.active11C is a numeric projection of the same original field, not a second Application authority to bind here.

Reading Application::animationPaused() directly is sufficient for the exact zero/nonzero race branch and works before first timestamp publication. Alternatively use sourceApplicationClock.read(sample).pause11C when a genuine published sample already exists. Do not sample another clock merely to obtain pause. The current source application publication survives frontend/race teardown.

Existing native GC water call at terrain4381 currently passes literal0u to its typed actualApplication11C parameter. That is an explicit accepted-world subset, not evidence of a shared pause-word binding. Do not copy it as the blink authority.

Smallest future owner: actual SourceCameraSession race state already owns raceFlags1B0_ and generation, so its actual race prefix can own four zero-initialized D8/E8 lanes and expose read-only selected-view D8. Feed its update from the existing explicit native root accepted-world scheduling, with actual Application pause and the same accepted delta, once per world step; never from renderer/display/FPS cadence. Include the held-start world branch6252..6268 as well as the ordinary accepted-world branch6343..6356 if those are the two explicitly admitted native race-update cuts. Guard against double invocation using genuine sourceCameraTick/generation state rather than a second time source. Preserve reset-to-zero after threshold (not subtract0.25 or catch-up loop), ordered comparison and exact view count race1B0 low2.

This would be an explicitly native-host-scheduled race-prefix join. It must not claim full original race/HUD traversal or claim the fixed native .016f step equals independently sampled original1A7B40 f12. No current native D8/E8 owner was found; the proposal is not active and no blink clock was added. Root must choose exact source-order insertion relative to its accepted child/manager steps and stop on genuine failed frame boundaries.

## Native owner join

The existing SourceCameraSession now owns the genuine four D8/E8 lanes. Their
numeric prefix runs exactly once when a new accepted sourceCameraTick enters
its existing native world phase, using that phase's already validated delta.
Retries within the same in-progress frame cannot advance the lanes twice.
This is explicit native scheduling, not full retail1A7B40 parent chronology.

OriginalApplicationAnimationClockView50KG exposes a read-only pause11C lane
from the same live Application owner without requiring timestamp publication.
The caller does not sample or fabricate time and does not introduce another
pause owner. Nonzero pause skips the blink loop; otherwise each active view
adds the real delta, resets E8 to0 at >=.25 and toggles D8 once without carrying
overshoot. No renderer readiness or completed display follows from this state.
Source review only; no new software test has run.
