# Native restart from an open menu

Source-only review of commit `30528592be354e0df55e7b6033cbaf3b7509a812`.

## Concrete defect

`runSession50DU` accepts F5/stdin `restart` before calling
`HostSessionMenu50IF::poll`. Opening the host Pause menu sets the process-owned
application animation pause word (`Application11C`) to one. The menu's accepted
Restart/ReturnOriginalMain/Quit actions clear it before departure, but the
earlier direct restart branch did not. The application and its animation-clock
owner survive the full world teardown and reconstruction. Consequently a restart
from Pause, Controls, or an open confirmation leaves that word set in the new
race even though the new session starts with its menu closed.

The fix clears that word at the accepted direct restart boundary only when a
host menu is open. It uses the existing explicit-pause writer, after the same
released input sample and before teardown. It does not reset the application's
clock, change focus/inactive suppression, or create a generic session-entry
reset. The existing Quit priority and rejected GA restart remain unchanged.

## Ownership evidence

- `tests/native/terrain_interactive_drive50du.cpp`: the direct restart branch
  precedes menu polling; the menu open/closed edge writes explicit pause, and
  accepted menu departures already clear it.
- `tests/native/original_ui_main_entry50kg.h`: `Application` owns the shared
  animation clock; `setExplicitAnimationPause` writes its `pause11C` field.
- The main session loop retains that same Application across native restart and
  original-menu return. No constructor/reset is invoked for it on restart.
- `src/game/original_application_animation_clock50kg.h` exposes the actual pause
  word to consumers. Native water, wreck and material paths read this owner.
- Original Main/course targets retain the RA course7/planet2 and Mawhonic9
  identities. This correction applies to the shared native session boundary;
  it changes no racer selection, course admission, AI driver, or asset owner.

## Verification boundary

Manual source review only. No build, test, syntax check, sanitizer, gameplay,
decoder check, or project helper was run. Root owns centralized validation.
The useful eventual interrupted-flow check is Pause -> Controls (or a restart
confirmation) -> F5, followed by a fresh closed-menu race with application pause
clear. Also compare ordinary F5 and the existing confirmed Restart menu action.
