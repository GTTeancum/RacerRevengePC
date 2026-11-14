# Silo: admitting converted pod archives (port side)

Nov 13 2026. Silo worktree `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree`, branch
`silo/arcade-sc-override`. Same mechanism as the SC track admission (`silo-override.md`, `silo-build.md`):
`<asset root>\converted-content.json` (schema `a2r-converted-content/1`), helper
`src\assets\converted_content50cv.{h,cpp}`. Installer: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\pod_silo.py`.

## Plain summary

- **34 pinned sites** (plus 10 storage caps and feature gaps, section 1.6) in the code built into the exe refuse or mis-handle a pod archive that is not byte-identical to the
  original (section 1). Almost all of them go back to one table: 23 rows of pod RES size + SHA, PHY SHA, speech
  INI SHAs and announcer pins (`tests\native\original_player_identity50ik_fixture.h`). About 20 consumers compare
  the actual bytes against that table.
- **One change fixes the table for every consumer:** a pod listed in the manifest gets an *effective identity*
  (size/SHA from its row, PHY/speech SHAs from the row's members, speech counts from the row's facts). Every
  consumer still compares the bytes it reads against that identity, so a listed file that differs from its row is
  refused like an altered original. Unlisted pods keep the original pins. Pod number is never a key.
- Three more general sites: the speech layout table (member counts/ordinals/sizes per racer) is replaced by
  lookup-by-name for listed pods; the loose `PODnn.CAM` catalog (2 readers) admits a listed CAM by its row.
- Left as listed pins (section 1, "keep"): shared members the converter copies byte-identical (binder, cable,
  particle EMTs, p_event/p_debris/wrecks); POD01-only bootstrap readers (stand-ins, POD01 is not replaced).

## 1. Refusal list (silo tree before the change; file:line, pin, fix kind)

Fix kinds: **identity** = fixed by the effective identity (one place); **name** = found by name in the pod's own
archive; **row** = admitted by its own manifest row; **keep** = pin on content a converted pod copies
byte-identical, or on a file outside the pod; **cap** = an upper storage bound (data must fit); **stand-in** =
POD01/racer-keyed branch, flagged, not touched.

### 1.1 The identity table and its consumers (all fixed by "identity")

| file:line | Pinned | Outcome | Fix |
|---|---|---|---|
| `tests\native\original_player_identity50ik_fixture.h:30-192` | 23 rows: RES bytes+SHA, PHY SHA, curses/taunts/special SHA, announcer bytes/events/SHA | data | identity |
| same `:209-226` | speech counts + sounds.ini SHA keyed on racer60 (14/19/21/25/26/1, else default) | wrong counts / refuse | identity (counts from facts for listed pods; FLAG stand-in for unlisted) |
| `tests\native\native_racer_rig50hw_cold.cpp:19` | RES bytes+SHA | throw "Original disc pod archive identity differs" (every racer) | identity |
| same `:60` | PHY SHA | throw | identity |
| `tests\native\terrain_interactive_roster_cold50du.cpp:122-125,178-179` | roster rows carry table SHAs; retained RES SHA == row | throw | identity |
| `tests\native\original_course_roster50kg_fixture.h:28-34, 79-97` | directDiscRoster rows; prepared-rider check | throw | identity |
| `tests\native\original_course_race_targets50kx.h:94-96,107-109,120-127` | all 23 RES+PHY at the Course screen (reads the table directly, not via find) | pod unavailable, then **every** rider's field unavailable | identity (`effective()` at both loops) |
| `tests\native\gameplay_binder_candidate_fixture.h:140` | RES bytes+SHA | throw (every racer) | identity |
| `tests\native\original_wreck_rig_inventory50la.h:17-19` | RES bytes+SHA, PHY SHA | debris off (all courses), throw on RA | identity |
| `tests\native\ra_cold_roster_resources50la.h:58-61` | RES+PHY (RA) | throw | identity |
| `tests\native\selected_player_audio50ij_fixture.h:43-48, 92-97, 105-108, 112` | read/decode limit = RES bytes, RES SHA, speech INI SHAs, counts, PHY SHA | throw (player and every AI audio) | identity |
| `tests\native\lap_sound_owner50gh_fixture.h:241-254` | RES bytes/SHA, read limit | throw (every race) | identity |
| `tests\native\pod_audio_owner50he_fixture.h:52-66` | same | throw | identity |
| `tests\native\opponent_engine_audio50hx_fixture.h:111` | PHY SHA | throw (every opponent) | identity |
| `tests\native\original_ai_audio_owner.h:178` | via selected audio | throw | identity |
| `tests\native\live_audio50hf_fixture.h:137, 468-474` | RES via identity | throw | identity |
| `tests\native\original_ta_camera_issuer_candidate.h:101,164,...,813` (12 sites) | PHY SHA per course issuer | session not issued | identity |
| `tests\native\original_subtype3_record_owner50la.h:56,72` | builds a direct Rig | inherits Rig | identity |
| `tests\native\source_announcer_authoring50ie_fixture.h:118-124` | PODnnANN.INI bytes/SHA/event count | throw | identity (row for the loose INI when listed; fact `events`) |
| `tests\native\race_audio50hq_fixture.h:173-179` | player PODnnANN.INI | throw | identity |

### 1.2 Member layout pinned per racer

| file:line | Pinned | Outcome | Fix |
|---|---|---|---|
| `tests\native\native_player_speech_authoring50la.h:105-117` (used `:205-246`) | per racer: member count 89-112, first ordinal of sounds.ini, the 4 speech member sizes | player speech disabled | name (listed pods: count/ordinals/sizes from the pod's own archive) |
| same `:182`, `selected_player_audio50ij_fixture.h:68-79` | POD26 sparse curses | racer-keyed | stand-in, not touched |
| `source_announcer_authoring50ie_fixture.h:62-72,123` | POD17 ANN.INI 2478 bytes, [Event30] offset | racer-keyed | stand-in, not touched (the loose INI is not part of the pod RES) |

### 1.3 Loose CAM

| file:line | Pinned | Outcome | Fix |
|---|---|---|---|
| `src\game\original_follow_cam_catalog.inc:2-26` used by `src\game\original_follow_cam.cpp:75-76` | 23 CAM size+SHA | follow camera refused | row (`pods/podnn/podnn.cam`) |
| `tests\native\original_ta_camera_issuer_candidate.h:24-33` (12 issuers) | same catalog | session not issued | row |
| `original_follow_cam.cpp:80,92,94` | 8 KiB text, 32 sections, 32 keys | refuse | cap |

### 1.4 Shared members pinned by SHA (converter ships them byte-identical)

| file:line | Pinned | Fix |
|---|---|---|
| `tests\native\gameplay_binder_candidate_fixture.h:108,136,147-162` | binder.psm 2791/a9e37280, binder.atm 51492/7dc77ff5, 10x64x64 frames, PHY must omit BinderMeshHalfHeight | keep |
| `tests\native\native_racer_cables50hw_fixture.h:151-160`; `tests\native\source_cable_texture50fc_fixture.h:17-32` | cable.psm 701039c9, cable.pst 2140/e6b0abd3 + decoded SHA | keep |
| `selected_player_audio50ij_fixture.h:87-90,99-101`; `lap_sound_owner50gh_fixture.h:191,215,257-259` | p_event.ini 462/49856d31 (6 events), p_debris.ini (4), wrecks.txt (NumSounds 4) | keep |
| `src\game\original_gc_pod_dust50kg.cpp:146-155`, `original_gc_pod_sparks50kg.cpp:30-35`, `original_gc_pod_fire50kg.cpp:23-47`, `original_wreck_explosion50la.cpp:32-36`, `original_wreck_splash50la.cpp:43-44`, `original_gc_spray50kg.cpp:122` | pod EMT sizes + SHAs | keep |
| `src\assets\original_standard_archive_provider50la.h:164-171` | the same member name in two mounted pod RES must be byte-identical | keep (a converted pod that changes a shared name is refused there) |
| `original_standard_archive_provider50la.h:150-151` | RC policy: POD17 specmap5.tga 12332 bytes | keep (RC only) |

### 1.5 POD01 bootstrap readers (POD01 always, whatever pods race; stand-ins, not replaced in this silo)

| file:line | Pinned |
|---|---|
| `tests\native\runtime_startup50fl_fixture_cold.cpp:12-48` | POD01 RES 349110 + SHA, PHY SHA, named members, node counts 14/14/18 |
| `src\assets\audio_assets50hc.cpp:267-293` | POD01 RES, member counts {6,4,4,19,12,9,2}, total 60 |
| `tests\native\terrain_interactive_setup50du.cpp:2275-2312` | POD01 engines/flames/EMTs for every race; engine nodes 14 |
| `tests\native\terrain_interactive_world50du.cpp:344,358,1068` | POD01 PHY/lift bootstrap body |
| `src\assets\original_ui_archive_catalog50kg.inc:45,2379-2474` (+ `original_pod_source_bank50la.inl:6-7`, `original_racer_manager_entity50la.h:156-164`, `ra_cold_roster_resources50la.h:80-111`) | POD01 as a UI catalog source (96 member pins) |

These pass as long as POD01 is not replaced. A converted POD01 needs them retired into the selected pod's data.

### 1.6 Caps (data must fit; no change)

`tests\native\native_racer_rig50hw_cold.cpp:108,116` (64 nodes per model, forest 80, MCF motions <= 9),
`tests\native\original_racer_visual50hw_fixture.cpp:1164-1242` (64 nodes, 7 mips, **32 unique textures**,
1024 px, 16 MB), `selected_player_audio50ij_fixture.h:49,54` (512 members, 16 MiB decoded, 8 KiB INI),
`tests\native\gameplay_audio_provider50he_fixture.h:16-30` (curses 24, taunts 12, special 11),
`tests\native\opponent_engine_audio50hx_fixture.h:107` (PHY 8 KiB), `terrain_interactive_setup50du.cpp:2322-2327`
(MC only: player RES 1 MiB, decoded 3 MiB). Feature gaps refused from data: single-wrist chariot
(`native_racer_rig50hw_fixture.h:313`), MCF MirrorAxis (`original_racer_animation_setup50la_fixture.h:112`),
motion names must be `<prefix>_...` (`native_racer_rig50hw_cold.cpp:157`), PHY Prefix must equal the pod number
(`:57`). The installer checks the caps it can see (pod_silo.py `CAPS`).

Already data-driven, no change: wreck reader (`src\game\original_wreck_assets50la.h`,
`original_wreck_asset_inventory50la.h`: parts/pieces from the PHY, no 7/9/13 literal), model bindings, PHY
readers, pod load 50pl, AI reader.

## 2. Port change

See the commit on `silo/arcade-sc-override` (section 4 has the id). Sites:
- `original_player_identity50ik_fixture.h`: `find()` / `effective()` / `converted()` / `speechCounts()` return the
  effective identity built once from the manifest (host-side statics; malformed row -> that pod refuses
  everywhere, no terminate). Log row `converted_content pod_identity pod=PODnn ...`.
- `original_course_race_targets50kx.h`: the two direct table loops use `effective()`.
- `original_course_roster50kg_fixture.h`: `supportsPreparedRider` no longer constexpr (find is not).
- `native_player_speech_authoring50la.h`: listed pods find the 4 speech members by name.
- `converted_content50cv.{h,cpp}`: `admittedBytes(path,data,size)` (noexcept); `original_follow_cam.cpp` and
  `original_ta_camera_issuer_candidate.h` admit a listed CAM.

Manifest facts the port reads for a pod row: `speech_numberoflowdamagesounds` ... `speech_numberoflosesounds`
(9 counts, bounded 0..24; when absent, the sounds.ini member must equal the original's). Announcer row (loose
`AUDIO/SPEECH/DAX/INRACE/PODnnANN.INI`, only if shipped): fact `events` (1..255).

## 3. Installer

`pod_silo.py install 04 15 ... --assets assets`: reads `export\a2r\pods\PODnn\install\PODS\PODnn\PODnn.RES`
(+ `PODnn.CAM` if it differs from the original) and `converted-content.fragment.json`; checks members/PHY
prefix/speech/caps; backs originals up into `silo\originals\PODS\...` once; merges rows into the asset copy's
manifest (track row kept; `trk_silo.py` now keeps pod rows too); re-reads every row against its file. Same lock as
trk_silo. `testpack` makes the admission test pod; `restore` puts an original back and drops its row.

## 4. Results

Build `silo-cv-10` (commit 7676131; 142 units recompiled, clean first time). All runs muted logs-only,
`--hold-throttle --steer-invert --camera-cycle --race-full --repair --unstick --results-confirm 30` (AI race, no
forced wrecks), 700 s watchdog. Summary script `silo\podwork\summ.sh`. The runner's exit 1 is the usual
"evidence incomplete" ending; results are read from the log.

### 4.1 Admission test pod (before converted pods existed)

`silo\assets-podtest` = robocopy of `assets-orig`; `pod_silo.py testpack 04`: original POD04.RES re-encoded with
zlib level 9 (340,921 bytes, SHA df14ba6c..., original 341,925 / fd6546c9...), same 89 members, listed in the copy's
manifest. TA via `order0.bin` (field 20, 4, 10, 11, 12, 15, 19 for player 3).

| Run | Pod 04 as | Load | Finish (player) | Pod 04 | Resets | FPS avg / min |
|---|---|---|---|---|---|---|
| silo-podtest-ta-3 | AI (slot 2) | `pod_identity POD04` row used; no refusal | 8th, 3 laps, 282.8 s | 4th, 3 laps; wreck inventory 7/7 pieces | 2 unstick | 62.2 / 53.6 |
| silo-podtest-ta-4 | player | same | 8th, 3 laps, 279.5 s | (player) | 1 unstick | 61.8 / 53.2 |

No `require_failed`, no "identity differs", no "unavailable" lines. Without the change, the first refusal would be
the Course catalog (pod unavailable -> every rider's field unavailable) and then the Rig's archive identity.

### 4.2 Converted pods (ARK->04, NEV->11, BOL->15, RAT->17, EBE->23, ELA->24) in `silo\assets`

Installed with `pod_silo.py install 04 11 15 17 23 24 --assets assets` (all six keep the original member list and
order; differences are PHY name, models, motions, collision, the three skin PSTs; CAM and speech INIs are byte
copies, so no CAM row). Two SC.RES versions were in use:
"SC v1" = 1,705,519 bytes dfbef3c3..., "SC v2" = 1,711,716 bytes dd147df6... (coordinator's divider walls + 2-way AI
fork at node 47), installed with `trk_silo.py sc` (now merges: pod rows kept). SC field: 08 10 17 18 19 16 24 + player.

| Run | Course | Player | Converted AI in field | Player finish | Converted AI results | Player resets | Pods destroyed | FPS avg / min |
|---|---|---|---|---|---|---|---|---|
| pod-sc-4 | SC v1 | 04 | 17, 24 | 7th, 310.3 s | 17: 2nd; 24: destroyed at 135 s (x 5770 z 9298), 1 lap | 0 | AI 24 | 62.3 / 47.4 |
| pod-sc-15 | SC v1 | 15 | 17, 24 | 8th, 320.9 s | 17: 2nd; 24: 7th | 0 | none | 62.3 / 46.8 |
| eb2-origpods-3 (originals) | SC v2 | 03 | none | **no finish**: stuck at x 5755 z 9330 from ~170 s, 110 resets in place, destroyed there | all 7 AI finished 3 laps | 110 | player | 62.0 / 46.8 |
| pod2-sc-23 | SC v2 | 23 | 17, 24 | 7th, 308.2 s | 17: 2nd; 24: destroyed (x 12623 z 14477), 1 lap | 2 | AI 24 | 62.0 / 46.8 |
| pod2-sc-24 | SC v2 | 24 | 17 | **no finish**: stuck at x 5757 z 9326, 62 resets, destroyed | 17: 2nd | 64 | player | 61.6 / 4.5 (restart load) |
| pod2-sc-11 | SC v2 | 11 | 17, 24 | **crash at 1.8 s**: "native engine audio controls rejected" | - | 0 | - | - |
| pod2-sc-17 | SC v2 | 17 | 24 | **no finish**: stuck at x 5755 z 9330, 76 resets, destroyed | 24: 7th, 2 laps when the run ended | 76 | player | 62.1 / 41.0 |
| pod2-ta-3 | TA (original) | 03 | 04, 11, 15 | 8th, 297.1 s | 04: 4th; 11: 7th; 15: 3rd (all 3 laps) | 1 | none | 62.3 / 47.4 |
| pod2-tb-3 | TB (original) | 03 | 23 | 8th, 461.6 s | 23: 7th, 3 laps | 4 | none | 62.2 / 45.8 |
| pod2-sc-4 | SC v2 | 04 | 17, 24 | 7th, 311.0 s | 17: 2nd; 24: destroyed, 1 lap | 0 | AI 24 | 62.3 / 46.2 |
| pod2-sc-15 | SC v2 | 15 | 17, 24 | 8th, 346.1 s | 17: 2nd; 24: 7th, 3 laps | 1 | none | 62.3 / 45.4 |

Findings:
- **Admission: no refusal in any run.** Every converted pod loaded through the common pod load as player and as AI
  (`converted_content pod_identity` rows; wreck inventory complete for each).
- **SC v2 loads and races with the 2-way AI fork**: no AI-spline refusal, AI finish (all 7 in the original-pod run).
- **SC v2 stuck spot x ~5756 z ~9328** (reset anchor node 54 -> 55): the player pod gets wedged there and every reset
  puts it back in the same place; 3 of the 6 SC v2 player runs never finished (pods 03 original, 24, 17). The same
  spot destroyed AI 24 on SC v1 (x 5770 z 9298). This is track data near the new divider/STAT walls, not pod data
  (the original POD03 is caught too). For the converter.
- **Converted POD24 (ELA) as AI is wrecked in 3 of 5 SC races** (the original POD24 was wrecked in 1 of 9 earlier SC runs, `silo-eb-ai2-3`).
  Its PHY differs from the original only in the Name, so the cause is in its model/collision data. For the converter.
- **pod2-sc-11 crash**: 1.8 s after green the player (converted POD11) jumped about 550 units forward (grid
  contact launch), its speed ratio left the [-2, 2] range the port's engine-audio routine accepts
  (`src\game\gameplay_audio50he.cpp:71-72`, a port domain guard), and the race terminated. Collision data of the
  converted pod at the start grid is the likely trigger; the guard turning a physics spike into a terminate is a
  port issue to look at separately (not changed here). Repeat run: see below.
- Repeat `pod2-sc-11b` (converted POD11 as player, SC v2): same crash at 0.8 s. Control `orig11-sc-11` (original
  POD11 as player, same SC v2, same field): no crash, finished 8th in 363.4 s, 1 reset before the finish, FPS 62.1 / 47.0.
  So the crash comes from the converted POD11's data (likely its collision at the start grid); POD11 as AI on TA
  was fine (7th, 3 laps).

### 4.3 Open

- Converter: POD11 (NEV) player start-grid launch; POD24 (ELA) AI wreck rate; SC v2 stuck spot x ~5756 z ~9328.
- Port: the engine-audio speed-ratio guard (`gameplay_audio50he.cpp:71-72`) terminates the race on a physics spike.
- Menus/garage still show the original pods (UIRIDER/UIGARAGE/UI\PODPHYS.RES are separate files, not converted);
  race name rows still show the original names (the UI name table, not the PHY Name).
- POD01 cannot be converted until the POD01 bootstrap readers (1.5) are retired into the selected pod's data.
