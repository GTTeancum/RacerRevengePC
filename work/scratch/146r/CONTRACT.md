# 146r common race route: shared contract for the helpers (read before editing)

Worktree (the ONLY tree you edit): `D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b`,
branch `common-route-146r`. Coordinator = helper 146r (sole committer and sole full builder in this tree).
Do NOT commit, do NOT run the game, do NOT run a full relink. No git stash. No files outside this worktree except
reading. Scratch only under `work\scratch\146r\<your-letter>\`.

Rules: `D:\Programming\GitHub\RacerRevengePC\CLAUDE.md` (one common path; no course-name or pod keys) and
`D:\Programming\GitHub\RacerRevengePC\work\scratch\batch145\BRIEF.md` (faithful; flag "PORT STAND-IN" /
"PORT EXTENSION, NOT ORIGINAL"; hooks never write memory someone else owns; bounds-check every write; stock pins become
receipts via `tests\native\course_archive_receipt50ci.h`; absent data = no owner; one plain log line per course/owner).
Plan being implemented: `D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a2601093eb9a4eea9\work\scratch\146c\COMMON-ROUTE-PLAN.md`
(line anchors are stale: re-grep). DLC contract: `D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\DLC-JSON.md`
("Add-on items", course item). Test data (read only):
`D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC` (courses eb/lc/ns/nb/nv; each archive
`TRACKS/<CODE>.RES` holds `tracks\<code>.scb/.tob/.aub`, `tracks\m<code>.spl`, `tracks\a<code>.spl`, `tracks\<code>01..03.vub`,
`tracks\<code>TVCam.pos`, `skies\<code>.cub`, `audio\introAud.ini` (NumEvents 0), `audio\scrptLns.ini` (0 lines), geometry,
textures; NO `.tun` (inherit.tuning tb) and NO `particle\demoprt.*` (inherit.particles ta); nv also `tracks\nv.nft`).
Member list tool: `python D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b\work\scratch\146r\members.py nv sa`.

## The design (already in the tree; do not redo)

1. The SA race chain IS the common route. SA is one data row on it (its stock pins/counts/SHAs are receipts, stock=1);
   an add-on course is another row whose every member name is formed from its own code. Identifiers were renamed
   tree-wide (work\scratch\146r\rename.py): `isSaTrack`->`commonRouteTrack` (true for SA and for a registry course once
   the session sets it), `isMaSaTrack`->`maOrCommonRouteTrack`, `saOriginalDisc`->`commonRouteOriginalDisc`,
   `nativeSaSelection`->`nativeCommonRouteSelection`, `saOriginalWorld`->`commonRouteWorld`,
   namespace `sa_original_world50LM`->`common_route_world50LM` (file names unchanged), `consumedSaStage`->
   `consumedCommonRouteStage`, race constructor `saWorld74_`..->`commonRouteWorld74_`.., `constructSaWorld1A6BC0`->
   `constructCommonRouteWorld1A6BC0`, `saTrigger*`->`commonRouteTrigger*`, `createNativeSaCameraSession50LA`->
   `createNativeCommonRouteCameraSession50LA`, `prepareSaNative`->`prepareCommonRouteNative`, `maSaAtm`->`maCommonRouteAtm`.
   MA keeps its own instantiation (flagged stand-in); the other 11 stock codes keep their loaders (flagged stand-ins).
2. Identity values:
   - `rr::platform::HostTrack50IM::Course` (platform\linux\native\host_track_selection50im.h): the common-route course
     whose code is not stock. `rr::platform::courseRouteCode50IM()` = its latched upper-case code ("NV"), set once per
     consumed launch; `hostTrackCode50IM(Course)` returns it.
   - `track_source_fixture::Track::course` (tests\native\track_source_fixture.h): same, for the source/material layer.
     It names NO stock profile. Wherever a switch on Track picks the SA policy/data rule, `course` takes the same rule;
     wherever it picks SA stock pins/tables, `course` has none (empty table, receipt "stock_row=none", never a refusal).
   - `track_session_assets50IG::SessionAssets::registry` (tests\native\track_session_assets50ig_fixture.h): the session
     row of a registry course (built by `registrySession(code)`): `world->code`="NV", `world->container`="TRACKS/NV.RES",
     `world->worldMember`="tracks\\nv.scb", world counts 0 (= from the data), `archive`= dlc:/ path (read it with
     `runtime_startup50fl::readBytes` / `rr::assets::dlc50DL::host`, which open the package file), `archiveSha256`/
     `archiveBytes` = the listed ones, `terrainOwners`=0 (= take the live count), `impactTuning`=member name,
     `tuningArchive`= stock archive path when inherited (else nullptr = session archive).
   - Row facts: `rr::game::dlc50DL::addonCourseLoadByCode(code, load)` (src\game\dlc_course_entries50dl.h,
     `AddonCourseLoad`: scb/tob/aub/cub/mapSpline/aiSpline/tvCamera/cutCameras, tuning/particle archives (inherited),
     introEvents/scriptLines, loadingScreen, phyClass/aiClass/rivalRacer, name/planetName) and
     `rr::assets::dlc50DL::addonCourseByCode(code)` (src\assets\dlc_registry50dl.h). Use these; never a course name.
3. Rule for a site keyed on SA (or on the 13 codes): replace the SA literal by the row's value formed from
   `trackSession->world->code` (or the owner's code); keep SA's stock pin as a receipt only for a stock code; a
   registry code has no stock pin. A 13-slot table becomes data/registry rows (a registry slot computed from the row).
   A hard SHA/count/fingerprint check becomes `course_archive_receipt50CI::receipt/countReceipt/memberReceipt`.
   Absent member = no owner (logged), not a refusal, unless the original itself requires it.
4. Log lines: one plain line per course/owner, e.g. `course_route_<owner> code=nv stock=0 ...`. Stock SA must log
   the same as before (or stock=1 receipts).
5. Header edits are allowed (one batched rebuild round follows) but stay inside your files. If you need a change in a
   file you do not own, write it down in your report (exact file/line/change) instead of editing it.

## Compile checks (yours only)

`cmd /c D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b\work\scratch\146r\<letter>\vs.cmd D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b\work\scratch\146r\<letter>\one.py tests/native/<file>.cpp [...]`
(copies of work\scratch\146r\one.py / vs.cmd with your own obj folder; at most 2 TUs at once). A header-only fixture is
checked by compiling a .cpp that includes it (find one with grep). Other helpers edit other files at the same time:
if a compile error is in a file you do not own, note it and move on.

## Report back (final message, short)

Files changed, what each site now does, stand-ins kept (flagged), log lines that prove it per course (stock SA and
an add-on), compile status per TU, and any change you need in a file you do not own.
