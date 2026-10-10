# Rider selection/data reconstruction: frozen candidate 1

Source-only, UNTESTED. No build, syntax check, test, project parser, importer,
gameplay, guest execution, or PS2/VU emulation was run. Active repo untouched.
The two new files under files/src/game are candidate additions; there are no
existing-file replacements and therefore no existing-file preimages required.
Root owns integration, registration, builds, and runtime validation.

## Delivered behavior

original_ui_rider_selection50kg.{h,cpp} authenticates the existing rider archive
owner, reads exact ui.dat/rider.dat/Widgets.Dat with shared source readers,
retains immutable page membership and PodIndex fields, and provides atomic
selection/appearance proposals. It reuses original_ui_navigation50kg rather than
copying wrap, membership, disabled/hole, or per-widget command behavior.

Both screen5/page1 and page2 use rider.dat with PageNo=1. The factory player
index is independent (0 or1). There are23 authored roster controls, 119 widgets,
OpenSize69, Time0.5, StartClosed. Source direction event0 calls1EB370 (previous),
event1 calls1EB1F0 (next). Controls carry actual PodIndex at+40, never slot-1.
The authored order begins rider19,20,12,10,2,4,11,15, then locked circuit riders,
and finally hidden riders1,14,25,21,26. Native code reads that order from data.

Entry1E9C50 restores6F0 for mode0,6F4/6F8 by player for mode1,6FC for mode2.
Searches all Group1 controls and keeps the last matching one-based slot. Entry
does not filter locked/hidden. Unknown modes/no match reject; no default rider.
The proposal includes group-entry commands from openingAC, not an execution of
all entry/opening side effects. OpeningAC must clear prompt42C after appearance;
openedB0 calls appearance again. Do not move that last refresh earlier.

Directional loops keep moving while unlock10FAC[rider]==0 AND hidden730[rider]
!=0. A locked rider with hidden730==0 remains displayed, but confirm is gated by
unlock. Every intermediate control's deselect/select callbacks is retained.
Original can loop forever with no eligible row; the native leaf bounds one
cycle and leaves all live state unchanged. Row availability is explicit, with
no invented profile defaults. The source capture/restore of first pod-widget
transform, selected-control reset2A5A00, and deferred4EC=3 are explicit proposals,
not completed actions. Appearance is between movement and reset/restore.

Appearance1EAFB0 uses actual saved lighting520/camera tint530 for unlocked riders,
locked colors500/510 otherwise. It calls widget enable2A7A00 in exact order:
- Unlocked: hide4, show1; mode2 show117/118/119; show7 iff mode2 career byte is
  nonzero, otherwise hide7; prompt mask7 for reset available,3 otherwise.
- Locked: show4, hide1,hide7,hide117/118/119; prompt mask2.
The career byte address is profile+1938+rider*(894+50), as the source multiplies
by894 and also adds rider*80 before the byte read. This is not a generic row
field at1938+rider*894. Unlocked non-career branches do not write117..119.

Confirm/back profile changes stay in existing original_ui_route_decisions50kg.
No new route registry, readiness mask, fake popup, save IO, or gameplay launch.

## Lifecycle and required joins

Entry74=1E9C50 clears42C/4E0/4E4, sets4E8=1, hides widget4, disables Group2,
restores selected rider/slot, handles optional4BC child, clears UI popup3D8 and
publishes page430 through global442F70+28. OpeningAC=1E9E70 sets locked colors
(0,0,0,64)/(0,0,0,255), captures actual ambient/light tint, enters Group1 for
actual player, sets globalUI474=0/478=1 for player2, refreshes appearance then
clears42C, optionally requests scene animation7, and disables Group2 again.
OpenedB0=1EA090 enables Group2, updates title widget87 from localized index168
for mode2 or167 otherwise, updates top title widget6 from172/173 for players1/2
on non-career branches, then refreshes appearance.

ClosingB4=1EA360 clears42C, disables Group2, hides the selected control's SECOND
widget (name text), and if4E8!=0 requests the scene animation transition with
float1.0. Closing-thresholdB8=1EA3E0 restores captured lighting520/tint530; for
player2 restores globalUI474=1/478=0. ClosedBC is shared1D7A00.

Rider begin7C1ED1C0 entry delegates shared aperture; exit is no-op. Actual update
1EB4F0 processes deferred4EC work, then only in manager lifecycle3/4 runs panel
motion1ECFE0. Entry waits for aperture0 then subtracts (140/Time)*dt, clamps0.
Exit adds until a subsequent update observes offset>=140, clamps140, and waits
for selected character readiness or real source bypass4E4. Only then begins
aperture closing. Poll1ECEA0 requires aperture0/2 AND panel offset exactly0/140.
Rendering1ECE20 calls source custom Rider draw before shared aperture1D7550.
Shared aperture currently binds Main only; do not mis-bind it to Rider or reuse
Main target hook PCs unchanged. Generalize its actual owner when integrating.

IMPORTANT CORRECTION to earlier transition reconstruction: control+34==4 means
NumberOfWidgets==4, not a control type discriminator. 1E9B48..64 stores the parsed
NumberOfWidgets at+34.1ECF40 tests this count;1ECF70 retrieves member FOUR
(control+38 -> +0C). This is Soultree_Character, not first User_Rotate pod. Actual
character readiness1D2710 returns1 if widget1D8==0, otherwise widget1E0. Assets
alone cannot establish completion. Every authored roster control has4 members.

Input1EA460 returns0 at1EAFA4 even when it handles a direction, unlike Main's
return1. Do not forward its event to registered Movement8 based on a successful
selection plan. Group1 AssociationCount0 also means AssociationID1=24 is ignored.
Back page1 sets4E8=1/4E4=1; page2 back sets4E8=0. Confirm player1 to player2 sets
4E8=0, external screen confirm sets1. Confirm while4E0 is active sets real4E4
and7D24=0.35 for matching player; update decrements that timer and can stop rider
voice at its crossing. Popup reset response3D8==2 calls profile1800E0(selected)
and refreshes appearance;3 clears popup. Those paths are not fake suppression.

## What remains unavailable

Concrete Rider User_Rotate and Soultree_Character dynamic-load callbacks,
character selection animation and readiness, source custom text/stat geometry,
scene/lighting and camera tint ownership, source UI audio/voice RNG ordering,
popup reset, deferred render work, profile transaction and shared aperture Rider
binding/hook joins. Existing Main widget bank supports49 Main widgets only and
must not be widened by replacing unsupported Rider widget kinds with icons.
The staged selection helper cannot advertise a ready Rider destination.

## Provenance

Static executable SLUS_202.68 hash:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
VA mapping is VA-100000+80. evidence/read_mips_words.py only reads instruction
bytes. It does not interpret or execute them. Unknown encodings remain UNKNOWN.
The raw DAT evidence copies already-decoded authenticated members from the
existing authored-menu-data-inspection workspace, not new archive decoding.
Their hashes match the repo catalog:
- rider.dat fc02bd935bcc540f69137a403f11751f71d9f9b87d764d03cc0df0117cff7cb0
- ui.dat 70935409b6d63634340af8129de8b397bc2788f0906bb17864f1c79524dafe7c
- Widgets.Dat 6e5fab48528286a04a6782362457cc66ef0ae478470a4cb77302b871db0f2bd5

Suggested central cases (not run): both players and every mode, remembered rider
with nonidentity slot, hidden skipped vs locked visible, all hidden atomic failure,
missing profile row, repeated intermediate callbacks, wrong controller, duplicate
PodIndex last-match, mode2 reset-byte address, unchanged unlocked non-career stats
visibility, actual appearance ordering and source return0 input propagation.

Frozen revision2 after root source review: Widgets.Dat now explicitly uses
preserveInOrder like the shared Main/Options owners, retaining first-match getter
semantics even for repeated keys. No claim of a duplicate-key parser run. Roster
control indices are independently checked against both model count and fixed
rider array before indexing, in addition to shared page-reader validation.
