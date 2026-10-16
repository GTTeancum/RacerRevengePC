# Stage8: native Tournament stat-bar subsection

After9ef965c. No builds/tests/checks/syntax validation, launch or gameplay.

Files:
src/assets/original_ui_material50kg.h
src/assets/original_ui_material50kg.cpp
platform/linux/native/original_ui_tournament_career_resources50kg.h
platform/linux/native/original_ui_tournament_career_resources50kg.cpp
this report; tournament-stat-counts.txt (offline static byte transcription).

Adds original six stat groups from1F13AC..1F1778 with appearance1F1F10..
1F1FC8 and draw1F2760..1F27A8. The existing canonical statSnapshot supplies
same-career current1920,maximum0408 and true profile ownership. No new profile
storage or writes. Profile generation/mode is checked before publication.

188DE0 maximum and188D70 current counts use separate binary32 multiply20,
add0.5, truncation and upper cap20. Negative converted counts, nonfinite/unrepresentable inputs, or
current>maximum reject; no out-of-range original array behavior is invented.
Actual six3D60 counts exposed separately from packets.
Source notch size5x10, x156+7*n,y164+18*row, object1060+row*780+n*60.
Unfilled333333 and filled selected17FA00 preserve sourceRGB>>1/alpha>>2.
All source counts are at most20; output order is row then notch.
Popup29 bit0 suppresses the stat subsection. It belongs after portrait and
before the medal subsection, not before the four background quadrants.

Bank image12 adds authenticated Tournament notch.pst, original absent-PSM
2475C0 fallback at1F1424. New dedicated provenance origin AND exact
sourceIdentityKnown whitelist check canonical archive/PST/page, no PSM,
source call, registered names and common ScreenMT inheritance/pass invariants.
The material array grows12->13; existing0..11 indices unchanged.

Corrects lifetime comment: raw returned packet pointers DO NOT retain the
owner. Caller must retain it through actual submission/renderer retirement.
This patch supplies no frame seal, lifecycle admission or retirement receipt.

Source-ready: bounded career assets/materials; numbers; medal subsection
with appearance; six stat groups. Still missing: animated medstar clock and
material-update hook, portrait and backgrounds, widget/controller tree,
complete native pod material submission, aperture/prompts, popup/save and
retained full-frame registry/submission/retirement. Frontend stays closed.
No shared frontend/profile/result source was edited.
