# Concrete native Rider profile source owner

Source-only candidate, not integrated or validated. No builds, tests, syntax
checks, sanitizer, project parser/importer, gameplay, PS2/VU emulation or guest
execution were run. Static ELF instruction/constant reads and independent bounded
Python-standard-library archive data inspection only. Parent owns integration.

## Deliverable

New platform/linux/native/original_ui_rider_profile50kg.{h,cpp} implements a
concrete application-lifetime owner of the fresh profile's Rider-facing fields.
It retains the same OriginalUiMainProfile50KG object as Main/Options and derives
mode34 from it on every observation. No public ownerReady setter or arbitrary
stat injection is provided. Initialization requires the retained common owner
and an authenticated original podPhysics archive. Missing source data rejects
initialization; a page cannot create a replacement owner.

The three src/assets archive files are exact-preimage replacements. They append
podPhysics=11, preserve all existing IDs/member ordinals, and add23 original PHY
member identities. The catalog bounds become12 archives/1060 members. Nothing
in CMake, Application, frontend, renderer, session or gameplay was changed here.

API:
- initializeAtApplicationStartup(commonProfile, podPhysics, diagnostic)
- initialized(), commonProfile() identity pointer
- selectionSnapshot(out): live mode and all remembered/unlock/hidden rows
- statSnapshot(selectedRider, panelY4DC, out): same-rider stats/medals/track/accent
- colors(rider, out): original accent17FA00 and secondary17FA10 packed RGB
- stamp(): generation plus live mode34
- PRIVATE commitBack(decision, expectedStamp): exact admitted Rider Back write;
  only friend OriginalUiFrontend50KG can call it, no public forwarding adapter

The session must retain an expected stamp with its selection/input proposal,
verify current source selection/lifecycle itself, then let the shared transition
manager perform full destination admission. commitBack rederives the existing
original_ui_route_decisions leaf and compares the request, sourcePC and sole
remembered-rider change before mutation. Rejected/stale calls have no effects.
The commit method is private to the concrete OriginalUiFrontend50KG friend.
Public snapshots or a fabricated admitted decision cannot directly authorize
profile mutation. The friend must use its genuine session-retained proposal and
current source selection/lifecycle before actual manager destination admission.
It does not admit a destination or create readiness by itself.

## Actual source lifecycle and dependencies

Original17DF80 constructor sets the23 represented pairs of packed colors at
+A8+8*rider/+AC+8*rider, later returned by17FA00/17FA10. Each pair in the native
table has its exact store PCs. These are not palette guesses or renderer-normalized
values. Zero/absent riders0,5,6,13 do not acquire a made-up stat/color row.

17F970 calls1870B0 to load authored ability values,17FFB0 to reset the career/pod
matrix, then1864B0 for settings, unlocks and remembered choices.1870B0 opens
UI\\podphys.res, loops1..26, formats pods\\pod%02d\\pod%02d.phy and reads:
- BaseAbilities into +180+24*rider
- MaxAbilities into +408+24*rider
- Six keys in order: MaxThrust, TheoreticalTopSpeed, SpoilerYawMoment, Defense,
  BaseRepairAmount, BaseCoolingTime

The source supports missing file/key skip-writes. Native admission here is
intentionally stricter for the23 actual represented riders: both sections and
all12 values must exist, parse and be finite/nonnegative. Reader is the shared
original DAT reader with preserveInOrder, preserving source first-present key
semantics and exact native numeric conversion. It never uses a fallback ability.
The original pod26 speed is1.75 in BOTH BaseAbilities and MaxAbilities. It stays
1.75; source188D70/188DE0 caps the converted notch count at20, not the ability.

17FFB0 resets a27x27 career/pod matrix. Rider deliberately reads its diagonal:
- current[i] = profile.float(1920 + rider*894 + rider*50 + i*4)
- medals[i] = profile.byte(1938 + rider*894 + rider*50 + i)
- reset-available byte = that same medal[0]
- career track = profile.byte(219C + rider*894) &31

The new owner represents only this UI-facing diagonal and low-five-bit track.
The genuine reset copies each pod's base six abilities to each career row,
zeros all13 medal bytes, and clears the low five track bits. These initialize
the native subset with actual source reset semantics. No raw guest memory or
full savefile buffer exists in the native owner.

186EF0 resets unlock words for indices0..26. Exactly2,4,10,11,12,15,19,20 are
unlocked. Exactly1,14,21,25,26 have hidden730=1; other source entries have0.
186678..186684 then sets6F0,6F4,6F8,6FC all to19. This is an explicit application
fresh-profile reset, never a missing-owner fallback or an unlock-all shortcut.

## Application join

The existing real native startup is tests/native/original_ui_main_entry50kg.h,
Application constructor: it creates process-owned Main/Options/GameSettings,
performs their explicit reset helpers and sets known=true once. Add a retained
shared_ptr<OriginalUiRiderProfileOwner50KG> to that same Application, load the
catalogued podPhysics archive through readArchive(assetRoot,Id::podPhysics),
then call initializeAtApplicationStartup(profile,archive,diagnostic). A forward
declaration of readArchive is needed if called before its existing definition.
This subset's authored-load/reset writes are disjoint from the Main/Options
reset fields already established there. Do not reset on entry, bank change,
Back, Defaults, Cancel, focus or frontend destruction. Give the frontend and
Rider sessions this exact retained owner; compare commonProfile() identity to
their actual Main profile pointer. Register the new platform cpp in the same
Linux native source list as other original UI session owners.

Sibling restore_original_rider_selection owns those joins. Parent alone may
integrate the staged files after checking exact preimages.

## Artifact identity and availability

Actual existing cloud file, read-only stat/hash verified:
/workspace/shared/recovery-20261010/ui-inputs/game-files/UI/PODPHYS.RES
46563 bytes; SHA2568361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8.
No PC transfer was initiated. This matches incoming manifest
/workspace/shared/recovery-20261010/ui-inputs/archive-manifests/RacerRevenge-original-game-UI-menu-and-videos.json
SHA2564593101cfa6d6880da73657c26eac92ae815bf2d546991dcb4581d4da9925d20.
Independent bounded stdlib inspection produced565248 original logical bytes,
23 member hashes, and byte-preserved PHY evidence. The exact report is
archive-member-identities.json; the read-only data reader is retained in evidence.
No game project parser or importer ran.

Original executable static reads:
/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68
SHA256c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
VA mapping VA-100000+80, instruction decoder only, never an interpreter.

## Deliberate boundary

This fresh-profile UI subset supplies real initial selection/stat information
and exact Back persistence. It is not an imported saved-career owner, original
memory-card implementation, full off-diagonal career/progression system,
comparison-row/confirm-to-gameplay owner or reset-popup implementation. With
source reset medal[0]==0 the career Reset prompt is genuinely unavailable.
Those dependencies must be added before admitting imported progress, unlock
updates, save IO, reset popup or racing. No gameplay code was touched.

Suggested parent checks, NOT RUN: archive authentication/full member identities;
all23 actual PHY loads and source colors; above1 pod26 stats; mode0/1/2 plus
both players' remembered choices; hidden skip versus visible-locked selection;
wrong shared profile identity; repeated initialization; rejected/stale Back;
Main->Rider->Back->Rider retained rider; exact source scene/widget/stat color use.

## Frozen revision2

Provenance review identified that a public commitBack would allow a caller to
forge admitted flags with a publicly observed stamp. The narrow correction makes
commitBack private, friending only the actual OriginalUiFrontend50KG coordinator.
The existing stamp, mode, source shape and exact route-leaf checks are unchanged.
No public adapter is supplied. Frozen revision1 is retained under revisions/frozen1
for audit. No tests or builds were run for this correction.
