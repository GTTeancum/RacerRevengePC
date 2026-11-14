# Menu (UI) data for the six converted pods (Nov 13 2026)

Tools (one code path for every pod, no pod keys):
- `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\ui_convert.py` (generator)
- `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\ui_verify.py` (port decoders + renders)
- `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\ui_silo.py` (installer, same guarantees as pod_silo.py)

    C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe -I ui_convert.py      (ARK 04, NEV 11, BOL 15, RAT 17, EBE 23, ELA 24)
    C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe -I ui_verify.py
    C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe -I ui_silo.py check | install | restore [--assets assets] [--only UI/UIRIDER/UIRIDER.RES ...]

Output: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\ui\` (archives at game-relative
paths, `converted-content.fragment.json` with all rows, `fragments\*.json` one row per file, `manifest.json`
provenance per member, `members\` loose, `verify\` PNGs + verify.json). Nothing installed, game not run.

## 1. What the menus use per pod (inventory, original game folder)

| Archive (game-relative) | Members per pod NN | Used for | Port loader (silo tree) |
|---|---|---|---|
| `UI\UIRIDER\UIRIDER.RES` | `ui\geometry\NN.psg` (rotating pod: 3 nodes `N`, binderleft1/right1; materials NN_1, NN_2, cable, NN_shdw; no pilot) | rider select | `tests\native\original_ui_main_entry50kg.cpp:959` readArchive; widget `User_Rotate SLT=NN.slt` -> `src\assets\original_ui_rider_pod_assets50kg.cpp:56-101` |
| same | `ui\anim\NNc.psg/.mcf/.vut` + `NNc_ent/_idl1/_idl2/_idl3/_breathe/_sel.mot` (standing character, ShortAxis MOTs, root position track) | rider select | `Soultree_Character MCF=NNc.mcf` -> `src\assets\original_ui_rider_character_assets50kg.cpp:59-123` |
| same | `ui\textures\NNc_1/_2.pst` (character pages; 04c/15c/17c are IPU kind 2, 11c/23c/24c kind 1) + .psm | character | `original_ui_rider_model_materials50kg.cpp:92-133` |
| same | `NN_shdw.pst/.psm`, `NN_flrg.pst` (64x64 rider flag), `NN_1/2/3.psm`, `NN_bkg.psm` | pod shadow, flag, page materials | `src\assets\original_ui_material50kg.cpp` |
| same | `UI\UIRider\rider.dat` Control -> PodIndex -> WidgetID2 (name) / WidgetID3 (Huttese name) -> `Widgets.Dat` TextID | name lookup (data, unchanged) | `src\game\original_ui_rider_dynamic_owner50kg.cpp:76-93` |
| `UI\UIPODTEX.RES` | `UI\Textures\NN_1/2/3.pst` (24 also NN_4) | pod pages for the UI pod | `original_ui_main_entry50kg.cpp:980`, garage `platform\linux\native\original_ui_garage_session50kg.cpp:2695` |
| `UI\UIBKG.RES` | `ui\textures\NN_bkg.pst` 512x256 backdrop | rider / win / unlock backdrop | `platform\linux\native\original_ui_rider_draw_resources50kg.cpp:32-33,198-208` |
| `UI\TEXTURES\OVERLAYS.RES` | `ui\textures\NN_fsm.pst` 32x32 flag | results / position rows | `src\assets\original_results_primitive_references50la.cpp:30-38` |
| `UI\UIGARAGE\NNE.RES` | `ui\geometry\NNe.psg` (both engines, 1 node, 1 LOD, prelit vertex colour), `NNe_1.pst/.psm`, `spcgarge.pst` | garage | `original_ui_garage_session50kg.cpp:2361-2363`; `src\assets\original_ui_garage_scene_assets50kg.cpp:47-48,138-141` |
| `UI\PODPHYS.RES` | `pods\podNN\PodNN.phy` (= the race PHY byte for byte) | stats bars (Base/MaxAbilities) | `platform\linux\native\original_ui_rider_profile50kg.cpp:309-336` |
| `UI\UITOURN\UITOURN.RES` | copies of `NN.psg`, `NN_shdw`, `NN_flrg`, `NN_1/2/3.psm` for some pods | tournament | catalog only |
| `UI\UIUNLOCK\UIUNLOCK.RES` | copies of `NN.psg`, `NNc.*`, `NN_shdw` for some pods | unlock screen | `original_ui_rider_character_assets50kg.cpp loadUnlock` |
| `UI\UIWIN\UIWIN.RES` | `NN_bkg.psm` only (unchanged material) | win screen | |
| `LOC.RES` | `loc\<lang>\L_UI_<LANG>.bin` string 177..199 (`STR_RIDER_SELECTION_PAGE_*`), `loc\<lang>\UI.loc` (key list, max-length table; identical in all 5 languages) | every name row: rider select (Name + Huttese widget), results, tournament, loading | `src\assets\original_ui_localization50kg.cpp:28-67` (English only); index table `original_ui_rider_profile50kg.cpp:61-63` |

Not per pod (checked): `UI\UIART1/2/3` `artpodNN / artpilNN / artplaNN` are gallery page numbers (artpil04 = Anakin
headgear concepts, artpod11 = Vader's pod; captions STR_GALLERY_PAGE_*), so they are not touched. `UIGARAGE.RES`
`tube04 / lightray04` are garage scenery. `Widgets.Dat` comments show pod 05 was once "Ebe Endicott".

In-race name rows in the port come from the comment above `[eventN]` in `AUDIO\UIACCEPT.INI` (port stand-in,
`tests\native\native_racer_rig50hw_cold.cpp:59-83`), not from LOC; not changed here (audio file, SHA-pinned).

## 2. What was generated (A arcade / G generated / D borrowed)

Per pod, 22 members; the same member name in several archives gets the same bytes (UIRIDER, UITOURN, UIUNLOCK).

| Member | Source | Rule |
|---|---|---|
| LOC `L_UI_*.bin` (5 languages) | A names | the string the rider page's Name widget names, replaced by the arcade name in capitals: ARK "BUMPY" ROOSE, NEVA KEE, BOLES ROOR, RATTS TYERELL, EBE ENDOCOTT, ELAN MAK (all languages; the originals use the same capitals in every language). Keys unchanged (STR_RIDER_SELECTION_PAGE_DUD_BOLT now holds ARK "BUMPY" ROOSE). |
| LOC `UI.loc` | G | only the max-length table entries of those 6 strings (= longest over the 5 languages, the original rule, checked on all 467). Header word +16 (0xEFEEE3F2, meaning unknown, not a CRC of any region tried) kept. |
| PODPHYS `PodNN.phy` | D | byte copy of the converted pod's PHY (Name changed; stats = the target's until the stats helper replaces them). Original PODPHYS == original race PHY for all six. |
| UIPODTEX `NN_1/2/3.pst` | A | converted race pages (engines / hull / pilot) resampled to the original UI page size and mip count. `24_4` kept (D, not drawn by the converted pod). |
| UIRIDER `NN.psg` | A geometry, G layout | converted race pod LOD 0, chariot hull + engines (pilot page and flames left out, as the originals have no pilot), uniform scale so the largest side equals the original UI pod's, centred on it. Race layout kept (the original art raises the chariot). Binder nodes at the converted engines' binder nodes. Two generated cable tubes (chariotcable -> enginecable). Shadow quad at y = -(highest point) as on every original; margins = median of the original quads. |
| `NN_shdw.pst` | G | top-down silhouette of that model in the quad's uv space (uv layout fitted on the original quad), blurred, peak alpha 67 like the originals. |
| `NNc.psg` | A | arcade pilot object in its `operate` pose, each segment rigid on its node (pod converter role names under a `root` node). The arcade has only seated pilots (RAT has no legs), so the figure is seated; height = 0.7 x the original standing character (G); placed on the original character's floor (original posed by idl1 frame 0), centred on it. |
| `NNc_1/_2.pst` | A texels, G layout | arcade pilot textures shared over two 256x256 pages by texel area; kind 1 single mip (04c/15c/17c originals were IPU kind 2). Alpha = original page median (or the median of the decodable originals). |
| `NNc_ent / sel / idl1 / idl2 / idl3 / breathe.mot` | A | arcade motions joy / joymade / L_chira / R_chira / L_pro / operate (held for the original length), resampled from 60 fps (assumed) to 15 fps; ShortAxis rotation keys (as the originals, MCF RotationType=SHORTAXIS) on every pilot node, position track on the root (resting place) and the top pilot nodes. |
| `NNc.mcf` / `NNc.vut` | G | original MCF with FrameCount per motion updated; identity VUT (,T on nodes with position tracks). |
| `NN_flrg.pst` / `NN_fsm.pst` | G | most colourful square of the converted hull page placed into the original flag shape (original alpha kept). |
| `NN_bkg.pst` | G (D pattern) | original backdrop pattern recoloured top -> bottom with the converted pod's two main colours, original row saturation/brightness kept. |
| `NNE.RES NNe.psg` | A geometry, G layout | converted engines, each centred on the original engine of the same side and standing on its floor, uniform scale; prelit vertex colours from a linear fit (colour = a + b.normal) of the original's colours. |
| `NNe_1.pst` | A | converted engine page resampled to 256x256. |

## 3. Checks (verify\verify.json, verify\*.png)

- Every written archive re-read with `geo_res.read_res` (port RES3 rules) and `trk_res3`; size/SHA match its row;
  member names and order identical to the original archive.
- Every changed member through the port decoders: PSG `decode_scene48c.decode_psg3_scene`, PST
  `decode_texture48d.decode_texture`, MOT `readMot49O` rules, MCF/VUT the character-loader rules, LOC the
  localization reader rules (467 unique names, <= 480 chars, every string <= its max length) in all 5 languages.
  **0 failures in 14 files / 199 changed members.**
- Renders, original vs converted with the same camera, looked at:
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\ui\verify\pod_NN.png`, `character_NN.png`
  (posed by idl1/ent/sel), `garage_NN.png`, `images_NN.png` (flag, results flag, shadow, backdrop, UI pages,
  character pages, garage page). The original 04c/15c/17c pages are IPU and show as "not decodable here".
- `ui_silo.py check`: every shipped file matches its row and its original; silo copies are still the originals.

## 4. Port pins that refuse these files (silo tree, not edited)

The main gate: **the UI archive loader does not consult converted-content.json at all.**

| file:line | Pin | Effect |
|---|---|---|
| `src\assets\original_ui_archive_catalog50kg.inc` (archive rows: LOC `:6`, OVERLAYS `:7`, UIBKG `:10`, UIRIDER `:13`, UIPODTEX `:15`, PODPHYS `:16`, UIGARAGE `:19`, NNE `:20-42`, UITOURN / UIUNLOCK / UIWIN / UIHOF `:18,46,47`; member rows ~`:57-66` LOC, `:84-106` NN_fsm, `:347-898` UIRIDER, `:958-1036` UIPODTEX, `:1088-1110` PODPHYS, `:2504-2506` UIUNLOCK art) | archive size + SHA, member count, every member name/offset/size/SHA | `src\assets\original_ui_archive50kg.cpp:203-233` returns archiveIdentity / memberTable / memberIdentity |
| `src\assets\original_ui_archive50kg.cpp:15` | exactly 43 archives, 3085 members (compile time) | |
| `tests\native\original_ui_main_entry50kg.cpp:2408-2431` readArchive | throws on any mismatch | **startup aborts** (LOC, OVERLAYS, PODPHYS, UIRIDER, UIBKG, UIPODTEX, UIHOF required; UITOURN/UIUNLOCK/UIWIN pinned when present) |
| `platform\linux\native\original_ui_garage_session50kg.cpp:2361-2363` | UIGARAGE + NNE via the same catalog | garage entry fails |
| `src\assets\original_standard_archive_provider50la.h:362-405` | UI mount re-checked against the catalog | |
| `platform\linux\native\original_application_localization50kg.cpp:28-31` | LOC member 0 must be the catalog's L_UI_ENG.bin | |
| `src\assets\original_ui_ipum_texture_cache50kg.cpp:12-30, 287-318, 342-372, 418, 503-778` + `_generated.inc`, `_additional_generated.inc` | IPU pages 04c_1/2, 15c_1/2, 17c_1/2 (size, SHA, mip layout, decoded hashes; count 29) | **startup throws** (`original_ui_main_entry50kg.cpp:986-995, 1014-1040`); the converted pages are kind 1, so these three pods need to leave the IPU list |
| `src\assets\original_ui_ipum_texture_cache50kg.cpp:808-863` | UIUNLOCK copies of IPU sources must equal the UIRIDER pins | |
| `src\assets\original_ui_material50kg.cpp:1797-1806` unlockedIpumName | 04c/15c/17c expected as IPU | material path refuses a kind-1 page under those names |
| `src\assets\original_ui_material50kg.cpp:1962-2001, 2403-2437` | 17_3 cut-out path (17_3.psm + UIPODTEX 17_3.pst 16..128 mip chain) | converted 17_3 keeps 4 mips 16..128, should pass the shape check; content pinned by the catalog |
| `src\assets\original_ui_material50kg.cpp:254-294` | provenance re-proved against catalog members | |
| `src\game\original_ui_lit_geometry50km.cpp:326-331, 346-354, 368-378` | Knire 22c / Kraid 23c (bytes, SHA, node count 26, 1850 vertices); vertex-coloured surfaces only for these | converted 23c has no vertex colour, so it takes the plain path; the 23c pin itself still refuses the bytes |
| `src\assets\original_ui_garage_scene_assets50kg.cpp:47-48,138-144` | NNe.psg 1 node / 1 LOD; fixed selected-pod position | data fits |
| `src\assets\original_standard_material_object50la.cpp:1788-1812` | NNE mounted by SHA and size | |
| `src\assets\original_ui_localization50kg.cpp:28-67` | 467 strings, <= 480 chars, unique names | data fits |
| `src\game\original_racer_selection50ij.h:18-52`, `src\game\original_ui_rider_voice_choice50kg.cpp:13-37` | hard-coded host-menu name strings ("RACER: TZIDIK WRANTOJO") | stale names (stand-in), not data-driven |
| `platform\linux\native\original_ui_rider_profile50kg.cpp:34-58, 61-63` | per-rider stat colours, LOC index table 177..199 | index table is fine (the text at those indices changes); colours stay the target's |
| `tests\native\native_racer_rig50hw_cold.cpp:59-83` | in-race names from AUDIO\UIACCEPT.INI comments (SHA 245b0bd5...) | in-race rows keep the original names (stand-in) |

What the port needs to admit them (general, data-driven, like the pod identity change):
1. `OriginalUiArchive50KG` / `readArchive`: a listed path in converted-content.json replaces the catalog row
   (archive size/SHA + its `members[]` name/offset/bytes/sha256 rows); member names and order must still equal
   the catalog's (they do), every byte still compared against the row. One place, every UI/LOC archive.
2. IPU cache: a listed UIRIDER/UIUNLOCK whose `NNc_k.pst` member is kind 1 leaves the IPU list (decide by the
   member's own PST kind, not by pod number); `unlockedIpumName` likewise by kind.
3. Lit-geometry 22c/23c pins: for a listed archive, select by "surface has vertex colour" (data), not by bytes.
4. Stand-ins to retire later: name strings in `original_racer_selection50ij.h` / `original_ui_rider_voice_choice50kg.cpp`,
   in-race names from UIACCEPT.INI comments (should read the LOC string the original reads).

## 5. Open / flags

- The Huttese name font `font\swhutt44.cel` has 29 glyphs (space, ( ), A-Z): the quote marks of ARK "BUMPY" ROOSE
  have no Huttese glyph (venus16 has them). Not seen in the game.
- Characters are seated figures (arcade data); no walk-in for the entrance motion (root stays at the resting place).
- UI pod keeps the race layout (original art raises the chariot); cables are generated straight-sag tubes.
- Stats bars and stat colours are the targets' (PODPHYS = converted PHY, stats unchanged; stats helper pending).
- UI.loc header word +16 unchanged (unknown meaning). Original game behaviour with the changed LOC/UI files not run.
- Tournament win scenes `TRACKS\ENDnn.RES` not converted (outside this task).

## 6. Port admission and in-game check (Nov 13, silo builds silo-cv-15 .. -18)

Port changes (silo tree commits 25cd44b; general, never keyed to a pod number):
- `src\assets\original_ui_archive50kg.cpp`: a UI/LOC archive listed in converted-content.json is authenticated
  against its row (archive size+SHA, every member size+SHA; names and order must equal the catalog). The catalog
  archive identity object is kept; each changed member gets ONE process-wide identity row which
  `originalUiMemberIdentity50KG` returns, so every identity-pointer check (catalogMember, localization, Hall
  records, archive views) agrees. Unlisted archives keep the exact catalog path.
- `tests\native\original_ui_main_entry50kg.cpp` readArchive: size from the row for a listed path.
- `src\assets\original_ui_ipum_texture_cache50kg.cpp`: a Rider/UNLOCK member whose own PST kind is not IPU has no
  IPU image (decided by the stored format). The rider draw already picks the material by the texture's kind.
- `platform\linux\native\original_application_localization50kg.cpp`: the 10637-byte English-table pin admits a
  listed LOC.RES of its own size (the 467-string scan validates the content).
- Vertex-colour character pin: no change needed, the callers already branch on "surface has vertex colour".
- Name stand-ins: in-race name rows of converted pods come from their PHY Name
  (`tests\native\native_racer_rig50hw_cold.cpp`, STAND-IN like the UIACCEPT comments; the original shows the LOC
  string). Not changed (host-menu/debug labels only): `src\game\original_racer_selection50ij.h:18-52`,
  `src\game\original_ui_rider_voice_choice50kg.cpp:13-37`.

Converter change: names are written in the glyphs both rider fonts have (Huttese `font\swhutt44.cel`: space ( ) A-Z);
quote pairs become ( ) - ARK (BUMPY) ROOSE. The '"' refused the rider page (run fix13-ui3-11, glyph ascii 34).

In game (runs fix14-ui4-11, fix14-ui5-4/15/17/23/24): the route reaches rider select, course, race for every
converted pod; rider select shows each converted name, the seated arcade pilot, the rotating converted pod, the
flag and stats (contact sheet `tools\a2r\scratch\fix13\rider_six.png`). BOL's figure is large and cut at the
left edge. Garage and results screens are not captured by the existing route (frames stop before the results
screen; the garage only appears on a race return); in-race standings rows show the converted names (logs).
The converted menu files are restored to originals in silo\assets after the checks.
