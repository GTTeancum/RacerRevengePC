# Actual native frontend controller navigation

Base d68359f. Source-only implementation; no application/input simulation,
build, test, syntax/sanitizer check, gameplay or decoder execution performed.

## Existing physical transport

WindowHost50DQ already owns SDL mapped GameController enumeration/open/close,
attachment and instance identity, sampled levels, and drained button down/up
edges. Slots do not shift on removal. Events are admitted only for the same
opened, still-connected instance. A real press/release within one event batch
keeps its pressed edge; focus-loss batches suppress edges. Unknown joystick
layouts stay unsupported, and absent/failed-open devices stay absent.

Previously windowRequest.gameControllers depended only on the human-driving
selection, which is always false in the required ALL8 original-AI route. The
window now also enables that actual transport when originalUiStarted and the
host is a real native window. No backend readiness or connection is invented.
The existing optional transport requires its existing SDL2.0.18+ controller and
rumble symbols; this patch does not weaken that backend capability check.

## Bounded frontend adapter

OriginalUiControllerNavigation50LA borrows only the actual slot0 snapshot and
returns the existing original2A7410/2AB010 event IDs0..7:

- SDL D-pad left/right/up/down13/14/11/12 -> source0/1/2/3.
- SDL A/X/Y/B0/2/3/1 -> source right-pad down/left/up/right4/5/6/7.

This matches the positional native mapping already documented by
selected_control_input50ky.h. Crucially North/Y is source Back6; East/B is
source event7 (Controls configuration), not a substituted Back action.

Only actual pressed edges dispatch. There is no held-button repeat timer,
analog-stick direction inference, synthetic event, fake controller presence,
second-controller page admission or missing-controller popup. Start/Select,
shoulder/trigger/click actions remain outside this bounded navigation adapter.
Holding a button does not repeat, matching the existing native keyboard edge
policy. It does not claim original low-level driver repeat timing recovery.

The adapter validates mapped button/edge values and instance identity. On a
new device, disconnect, focus/activity interruption, minimize, event-budget
exhaustion or explicit frontend pause, it disarms. A real released navigation
snapshot rearms; held/reconnect input cannot activate a page unexpectedly. A
quick tap while already armed is retained even when final levels are released.

## Driver ordering and ALL8 isolation

Observation occurs inside originalUiActive before the existing inactive-frame
return, so inactive polls actually disarm the latch. Active keyboard and pad
edges merge by source ID, then dispatch once in existing keyboard order
0,1,2,3,4,5,7,6 (Back remains after Controls configuration). Simultaneous physical
confirm keys therefore cannot create duplicate original input4 in one poll.
Keyboard mapping, including keypad Enter, is preserved. Existing original
page callbacks, movement descriptors, aperture/transition gates and destination
admission own all resulting state/audio/profile mutations.

The session input still declares original controller0; slot1 never changes
secondControllerPresent. A device remaining in slot1 after slot0 disconnect
is not silently promoted or used to admit Options page3.

No change is made to usePhysicalGameController, humanControlsPermitted,
composePacket, selected driving presets, original racer class, ALL8 policy,
feedback ownership or race Start/pause handling. Outside originalUiActive the
new adapter is never dispatched. Enabling device observation cannot enable
racer human input; all eight original AI owners remain authoritative.

No new UI assets or replacement panels are introduced. Root owns centralized
build and real-device acceptance. Larger Options destination/popup and original
race-pause UI gaps remain as described in the navigation audit.
