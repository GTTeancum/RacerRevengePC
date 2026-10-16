# Original Ryloth course mapping, 2026-10-14

Source/data inspection only. No project executable, build, syntax check, test,
sanitizer, gameplay, profile modification, unlock, or launch was performed.
This packet establishes original identities and identifies missing joins; it
does not admit either Ryloth track for production runtime.

## Authenticated original evidence

- Original `SLUS_202.68`: 2,972,720 bytes, SHA256
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
  ELF LOAD offset0x80 maps virtual0x100000; source PCs below map to file
  offset `PC - 0xFFF80`. Its instructions were inspected as data only.
- Original `UI/UICOURSE/UICOURSE.RES`: 1,227,424 bytes, SHA256
  `f45585dd7fe7ba2a24cfc5b6ce4d3fb5efa6bc279f636e071aecf5ccf970c855`,
  supplied in `RacerRevenge-original-game-UI-menu-and-videos.zip`.
- Original member `UI\\UICourse\\track.dat`: 4,706 bytes, SHA256
  `d445012d2342ba33c685e3b2b1f78e3b2d648c4d8057fa1427d5f58639ea7268`.
- Original member `UI\\UICourse\\Widgets.Dat`: 12,846 bytes, SHA256
  `3ea165fa6bf0071786e454cf79982f0f5e438694d98264d724ff7c29a1ec75e6`.
- Original `LOC.RES`: 35,925 bytes, SHA256
  `b02e9c437311fd18b9dcd52c0d02222320be4eb7e5bfa17a60f564b05e6d4a0e`,
  supplied in `RacerRevenge-original-game-APPSTART-LOC.zip`.
- Original `loc\\english\\UI.loc`: 17,688 bytes, SHA256
  `c57ac0eebc9eba4742d7b9bc4a840ca6bb3de23b673353f7cc558d15d229f9b3`.
- Original `loc\\english\\L_UI_ENG.bin`: 10,637 bytes, SHA256
  `48fa45093b373dc587637fb7656ad6cb7fb9c4a5d19956cd5b4c427887f1f4d2`.
- Full48F native-cache generation
  `3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39`
  independently retains RA and RC original archive/member identity rows.
  It does not provide original unsupported text/COL/SPL/TOB/TUN payloads.

## Original Course identity domain

| Field | Ryloth A | Ryloth C |
| --- | --- | --- |
| Authenticated English name | THE BRIGHTLANDS | THE NIGHTLANDS |
| Original archive | TRACKS/RA.RES | TRACKS/RC.RES |
| Group1 one-based selected slot | 5 | 12 |
| Authored control ID | 3 | 12 |
| Authored CourseIndex / track ID1125F low nibble | 7 | 3 |
| Planet text / native planet index | ryloth / 2 | ryloth / 2 |
| Rotating representation widget | 12 | 23 |
| Original hologram MCF | holo_ra.mcf | holo_rc.mcf |
| Name widget | 30 | 41 |
| Original localization symbol | STR_TRACK_RYLOTH_A | STR_TRACK_RYLOTH_C |
| Descriptor address relative to profile | 112CC | 1129C |
| Original localization integer | 108 | 117 |
| Fresh original course-record pod | 9 | 24 |
| Fresh course-order zero-based position | 4 | 11 |
| Original fresh AI creation class | A / 1 | C / 3 |

The authored Group1 ControlID order is
`1,4,5,2,3,6,9,7,13,14,11,12,10`.
The separate profile course-order track-ID array is
`0,11,4,8,7,1,5,9,12,6,10,3,2`.
Course slot, control ID, track ID, planet ID, and course-record pod must remain
distinct. Widget/control numbers do not imply a track ID.

The source Course page has no Ryloth B control. Widgets18/36 are Empty, while
Ryloth A/C have real original holograms/name symbols. Full48F has RA/RC rows
and no RB or RY archive. This does not justify inventing an RB/RY identity.

### Static executable provenance

- RA string `ra\\0` is at virtual3A89E8;17E698/17E69C forms its address in s3.
  17EA44 forms profile112CC =11278 +12*7;17EA54..17EA5C copies the three
  original bytes.17EA64 loads108 and17EA6C stores localization key112D0.
  17EA78 loads9;17EA80 stores fresh record byte112D4 =11280 +12*7.
- RC string `rc\\0` is at virtual3A8A28;17E72C/17E730 forms its address in t5.
  17ED64 forms profile1129C =11278 +12*3;17ED6C..17ED74 copies the bytes.
  17ED80 loads117;17ED88 stores localization key112A0.
  17ED94 loads24;17ED9C stores fresh record byte112A4 =11280 +12*3.
- Existing recovered187B10 scan and16E590 class selection use the separate
  order array: position<5 gives class1; position<9 gives class2; else class3.
  Thus RA order4 is A and RC order11 is C. The current selected rider is not
  either fresh course-record pod.
- Disc strings `\\TRACKS\\RA.RES;1` and `\\TRACKS\\RC.RES;1` occur at virtual
  3A2330 and3A2370. They are actual archive names, not RY/RB substitutions.

## Full48F original archive and world provenance

| Archive | Bytes | Original SHA256 | SCB zones / portals / placements |
| --- | ---: | --- | --- |
| RA.RES | 4,604,561 | 1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2 | 10 / 10 / 1261 |
| RC.RES | 4,655,141 | ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b | 15 / 15 / 1277 |

- RA original `tracks\\ra.scb` SHA256
  `57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81`;
  native world key `89c668060947be701ae28acb37b4772156cb6d26f614a353318bfd1f6d0582f8`,
  world SHA256 `3f3811b6304fa56d4edaccdc0347fe6693a3179f0c0ad98fcc0b3b8571b5f49a`.
- RC original `tracks\\rc.scb` SHA256
  `1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a`;
  native world key `eeccac5f32592da1735be91de65d9251c1f44f5c59c5eb66fbcb05d8364938fa`,
  world SHA256 `51ef4b7630711ec82acda41b6ef59c7062712fa18ee430850b94c724d4b2fa76`.

## Original unlock domain and current native restrictions

The fresh original availability low-five-bit count is4. OpeningAC disables
all Group1 controls and enables only those whose CourseIndex matches the
first availability entries of the profile course-order array. Thus original
fresh enabled courses are TA/SA/MA/GA. RA is the fifth entry and RC the twelfth;
neither is enabled by the fresh profile. This source prefix domain is distinct
from a native cold destination capability. RA would require availability>=5
and RC>=12 under the represented original prefix model; these are observations,
not permission or an implementation plan to modify the profile.

Current additional native boundaries:

- `original_ui_course_widget_controller50kg.cpp::bounded` accepts course700
  only1..4 and availability==4. `moveCourse` rejects a resulting slot>4.
- `original_ui_course_session50kg.cpp::bounded/profileCurrent` independently
  requires that same fresh profile domain throughout the live session.
- `original_ui_course_race_domain50kg.h::nativeCourseTrackOwned50KG` has
  TA(0,0,1), SA(11,4,2), MA(4,5,3), GA(8,1,4), GC(10,1,11) only.
- Frontend preflight and profile `commitCourseLaunch` both require this
  native capability independently of source unlocks. Adding a capability row
  must not silently change availability, remembered selection, or unlock state.

## Production joins currently absent

The following is the inspected pre-intake source baseline at
`35fba3f4b5d535f7a7b4758601e75f5d77709e39`, before parallel source-only Ryloth
preparation. Later metadata-only edits must be reviewed separately and do not
imply production admission or a changed original unlock domain.

1. `track_session_assets50ig_fixture.h`: RA/RC are WorldMetadata only;
   `sessionAssets` returns null. No genuine archive/splines/tuning/visual/sky
   SessionAssets binding exists; `select` explicitly rejects unavailable data.
2. `host_track_selection50im.h`: HostTrack enum/code/admission contains only
   TA/GC/GA/MA/SA. No RA/RC selector or admission exists.
3. `original_course_race_targets50kx.h`: no RA/RC target/catalog row exists.
   Existing archive authentication, selected-rider roster admission, exact
   profile snapshot, deferred launch, world teardown and cold consume checks
   must remain intact when any new destination is integrated.
4. `ai_cold_owners50iu.h::bindFreshAiCreateSelection50IU`: accepts only the five
   existing destinations. RA requires actual ID7/order4/classA/record9; RC
   requires ID3/order11/classC/record24. Neither can inherit GC or selected pod.
5. `terrain_interactive_drive50du.cpp`: initial-track booleans and effective
   HostTrack mapping have no RA/RC branch. Per-track source authentication,
   collision, map/AI splines, camera/TOB, trigger/dynamic/effect owners,
   material textures/animation, lighting/fog/sky/audio and lifecycle consumers
   still need authentic independent joins and central validation. Retained
   RRWORLD metadata or visual cache alone establishes none of that readiness.

## English course-name closure

The independently authenticated original localization closes the name mapping:

- Entry108: `STR_TRACK_RYLOTH_A` at UI.loc offset0xD7C maps to
  `THE BRIGHTLANDS` at L_UI_ENG.bin offset0xF16.
- Entry117: `STR_TRACK_RYLOTH_C` at UI.loc offset0xE39 maps to
  `THE NIGHTLANDS` at L_UI_ENG.bin offset0xFB1.

UI.loc count is read as little-endian u16 at26, first symbol offset as u32 at4,
and table offset as u32 at8. Symbols and English text are each NUL-separated
and joined by their actual zero-based index. This matches the recovered
14BC30/14C5A0 localization ownership represented by the native reader; no
English name was inferred from track letter, scenery, or public references.

Therefore original **Brightlands requires RA.RES**, not RC.RES. Its exact
destination tuple is courseID7/planet2/sourceSlot5. Original **Nightlands
requires RC.RES**, with tuple courseID3/planet2/sourceSlot12. This evidence
does not grant user authorization to launch either destination or modify its
locked profile state.
