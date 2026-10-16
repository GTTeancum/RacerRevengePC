# Standalone original frontend physical input join

The race-hosted frontend already enables the actual SDL controller backend,
uses `OriginalUiControllerNavigation50LA`, merges real keyboard/controller
edges by original input ID, and accepts keypad Enter. The later standalone
frontend had retained `window.gameControllers=false` and its older keyboard-only
dispatch. Consequently, the active Main-only lifetime could not receive those
physical menu events even though the shared backend and translation existed.

The standalone window now enables that same backend in window mode. Headless
mode remains without a controller backend. Its own translator observes the
actual `WindowInput50DQ` every poll before the inactive-frame early return, so
disconnect, instance replacement, focus loss, pause and event-budget rejection
retain the existing disarm/neutral-rearm behavior. No repeat timer, analog
threshold or synthetic edge is introduced.

Keyboard edges retain their existing route-dependent mapping. Keypad Enter88
joins Return40 and Space44 on source confirm4. Real keyboard and pad edges
are merged by source ID, preventing simultaneous confirmations from firing
twice. Dispatch order remains0,1,2,3,4,5,7,6, with Back last. The existing
physical-input gate still excludes unattended unfocused capture. Capture's
actual-input interruption guard remains unchanged.

This session owns no racers, so the new input join cannot enable human driving
or alter ALL8 AI control. Source page routing/admission remains authoritative;
the physical snapshot is not promoted into the unimplemented original second-
controller presence/popup capability. No Pause lifecycle activation is added.

Source-only review; no build, input injection or gameplay execution performed.
