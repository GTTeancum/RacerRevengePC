# DLC / mods pipeline: design

Nov 15 2026. Read-only design. Nothing under src, tests, platform or tools was edited, built or run.
Governing rules: `D:\Programming\GitHub\RacerRevengePC\CLAUDE.md` (one common path, data only),
`D:\Programming\GitHub\RacerRevengePC\docs\HANDOFF-20261106.md` (faithful, flag stand-ins, bounds-checked
writes, hooks never write memory something else owns). Line numbers are from the working tree at e9d3754.

**NOT ORIGINAL BEHAVIOUR.** The original game has no add-on content. Everything here is a PC enhancement.
It is built so that with no DLC folder (or an empty one) the game runs exactly as today, byte for byte,
and so that every add-on item runs through the same code the stock items use.

---

## 0. Decisions the user needs to make

| # | Question | Options | Recommendation |
|---|---|---|---|
| D1 | Where do add-on courses appear on the Course screen? | (a) one extra planet at the end of the planet cycle (after Mon Calamari) holding every add-on course; (b) each package names its own planet, so several extra planets can appear; (c) add-on courses sit on an existing planet named in the package; (d) a separate menu entry from the Main menu | (a) for the first course phase; (b) later if many packages arrive |
| D2 | Label for that planet / section | "ADD-ON", "DLC", "OUTER RIM", other | "ADD-ON" (it is a label string, changeable at any time) |
| D3 | Which modes can use add-on courses? | free play only (Single Race, Time Trial); plus a separate add-on tournament circuit later; never inside the stock 13-course tournament | free play only first; add-on circuit in a later phase; stock tournament untouched always |
| D4 | Where do add-on pods appear on the Rider screen? | appended after the stock riders in the same carousel; or behind a divider/marker | appended after the stock riders, with the add-on label in the name strip |
| D5 | Are add-on pods available at once? | always unlocked; or unlock rules carried in the package (e.g. "win course X") | always unlocked in free play; package unlock rules only with the tournament phase |
| D6 | Can add-on pods be AI opponents? | never; only on add-on courses; anywhere when the package allows | only on add-on courses. Stock courses keep the stock opponent field, so a stock race is identical whether or not DLC is installed |
| D7 | Records set on a STOCK course while driving an ADD-ON pod | (a) go into the stock Hall of Fame (the stock save format allows it); (b) go into the DLC side file only | (b). The stock save then never carries anything a DLC produced, the stock unlock checks (which read Hall records) cannot be triggered by add-on pods, and removing a DLC leaves a pure stock save |
| D8 | Replacements (packages that overwrite a stock file) | allowed whenever a package flags them; or also require a global "allow replacements" setting | allowed when flagged, plus one global switch (default on) so a player can turn every replacement off at once |
| D9 | Bad package notice | log only; or log plus a one-time message on the title screen naming the refused packages | log plus one-time title message (needs the shared dialog; until then log only) |
| D10 | Records when a package's course changes in a new version | keep them; reset them | keep them unless the package changes its `records_key` (the author decides) |
| D11 | DLC folder location | `<install>\DLC` next to the stock files; somewhere else | `<install>\DLC`; tests use `RR_DLC_ROOT` under `work\scratch` |

Two things are **not** decisions, because the rules already settle them:
- A DLC never runs code. Packages carry data only; the port refuses any package that lists an executable
  or script file.
- A DLC never stores its data inside the original save layout, original tables or "spare-looking" bytes
  (for example the never-read 14th Hall row at profile 0x10E84, pad bytes, or the unused unlock slots).

---

## 1. Plain summary

- **One folder per package.** `<install>\DLC\<package-id>\dlc.json` plus the package's files. The json
  lists every file with its size and SHA-256, what the package adds, and, only when explicitly flagged,
  which stock files it replaces.
- **Add-on, not replace.** Add-on files live under their own name space (`dlc:/<package-id>/...`). No
  add-on file can have the same name as a stock file, so it can never hide one. A stock file is replaced
  only when the package says `"replace": true` and names the exact stock file and its stock SHA-256.
- **New slots go in port-owned storage.** The 13 courses, 23 pods, 27 careers, 13 Hall rows and every
  other original table stay exactly as they are. Add-on courses and pods are appended in a port-owned
  registry sized from the packages that passed validation. Add-on numbers start at 32 so that no add-on
  number can ever pass for an original value; any attempt to store one in an original field is refused by
  the encoding checks the port already has (`course<13`, `rider<32`).
- **One common path.** An add-on course is loaded by the same race load as a stock course, an add-on pod
  by the same pod load. The only difference is where the data comes from. This has one hard consequence:
  **add-on courses need the common race load (the common-race plan, SCHEDULE batches 11-19) to land
  first.** Today every stock course still has its own loader picked by its course code; a new course code
  has no loader to run, and borrowing another course's loader is exactly the per-course stand-in the
  repository rule forbids. Pods are ready sooner: they already go through one common pod load (build129e)
  with identity rows from data (build140c), and seven converted pods already race on it (build140g).
- **Saves.** The stock save is untouched. DLC progress goes into a separate keyed side file. Removing a
  package hides its entries; the side file keeps them, so putting the package back restores them.
- **Validation.** At startup every package is checked in full before anything is used. A bad package is
  refused as a whole with one clear log line, and the game continues with stock plus the good packages.
- **First useful phase:** the package format, validator and file overlay with flagged replacements only.
  That retires `converted-content.json` and the separate silo asset copies immediately: the three arcade
  courses and the seven arcade pods become replacement packages over an untouched stock install. Add-on
  pods in free play come next; add-on courses in free play follow the common race load.

---

## 2. dlc.json schema and folder layout

### 2.1 Folder layout

```
<install>\                       stock install (never written by DLC)
  TRACKS\  PODS\  UI\  AUDIO\  LOC.RES ...
  DLC\                           DLC root (RR_DLC_ROOT overrides; RR_DLC=0 disables)
    etti-iv\                     folder name == package id
      dlc.json
      TRACKS\EB.RES              course archive (members named tracks\eb.*)
      UI\EB_UI.RES               course-screen hologram, preview, record icons (own small UI archive)
    arcade-pods\
      dlc.json
      PODS\A2R_ODY\A2R_ODY.RES   pod archive (members named pods\a2r_ody\...)
      UI\A2R_ODY_UI.RES          rider-screen model, portraits, background, results icon, garage model
      AUDIO\A2R_ODY\...          accept-voice clips and announcer INI (optional)
    arcade-sc-replace\           a replacement package (explicitly flagged)
      dlc.json
      TRACKS\SC.RES
```

Rules for the folder:
- The folder name must equal `id`. Anything inside that is not listed in `files` refuses the package
  (no stray files). `dlc.json` itself is not listed.
- Paths are relative, `/` or `\`, no `..`, no drive or root, no symlinks, junctions or reparse points
  (the existing resolver already refuses parent traversal and symlinks:
  `D:\Programming\GitHub\RacerRevengePC\tests\native\runtime_asset_paths50lb.h:18-27`,
  `D:\Programming\GitHub\RacerRevengePC\tests\native\pod_materials50ed_fixture.h:162`).
- Refused file types: `.exe .dll .com .bat .cmd .ps1 .vbs .js .py .sh .lua .so .dylib`, and any file
  whose first bytes are a PE or ELF header.

### 2.2 Top-level fields

| Field | Type | Required | Rule |
|---|---|---|---|
| `schema` | string | yes | `"rrpc-dlc/1"`. Unknown major version refuses the package. |
| `id` | string | yes | `[a-z0-9][a-z0-9-]{2,47}`, unique, equals the folder name. Never reused for a different package. |
| `name` | string | yes | display name, at most 63 characters, printable. |
| `author` | string | yes | at most 63 characters. |
| `version` | string | yes | `MAJOR.MINOR.PATCH`. |
| `description` | string | no | at most 1024 characters. |
| `enabled` | bool | yes | `false` = skipped without validation beyond the json itself; logged as disabled. |
| `kind` | string | yes | summary of the package: `course`, `pod`, `menu_assets`, `replacement` or `bundle` (several kinds). Must agree with the items. |
| `priority` | int | no | -1000..1000, default 0. Only used to settle replacement conflicts and add-on order. |
| `requires_port` | string | no | lowest port data contract the package needs (`"dlc-contract-1"`). A newer contract than the build knows refuses the package. |
| `replace` | bool | no | default `false`. Must be `true` for any `replacements` entry to be honoured; with `false` a `replacements` list refuses the package. |
| `replacements` | array | no | section 2.4. |
| `items` | array | yes | add-on items, section 2.3. May be empty for a pure replacement package. |
| `strings` | object | no | `{ "english": { "KEY": "text", ... }, "french": {...}, ... }`, section 4.5. English required if any key is used. |
| `files` | array | yes | every file: `{ "path", "bytes", "sha256" }`, optional `"role"` (free text for tools). |
| `dependencies` | array | no | `{ "id", "version": ">=1.0.0 <2.0.0", "optional": false }`. |
| `conflicts` | array | no | `{ "id", "reason" }`: refuse this package if the named package is admitted. |
| `facts` | object | no | converter-computed counts (as in today's `converted-content.json`). Informational only; the port recomputes everything from the bytes. |

Size limits: `dlc.json` at most 1 MiB; at most 4096 files per package; at most 256 items; total package
bytes at most 512 MiB. Each archive kind also has its own existing limit (section 5).

### 2.3 Item kinds

Every item has `kind` and `id` (`[a-z0-9-]{2,47}`, unique inside the package). The full key of an item is
`<package-id>/<item-id>`; that key, never a slot number, is what saves and dependencies use.

**`course`**

| Field | Rule |
|---|---|
| `code` | the course code the archive's members are named with (`"eb"` -> `tracks\eb.tob`, `tracks\meb.spl` ...). Lowercase letters/digits, 2 characters in the first course phase. Must not equal any stock code (`ta tb tc rc ma mb mc ra ga gb gc sa sc`, also `sb`) or another admitted package's code. The PHY class follows from it by the original rule (`D:\Programming\GitHub\RacerRevengePC\src\game\authored_parameters49n.cpp:106-118`: an `a` anywhere = A, else `b` = B, else C); the validator logs the class it gets. |
| `archive` | package path of the course RES. |
| `name_key` | string key for the course name (section 4.5). |
| `planet` | placement key (D1). With option (a) always `"addon"`; with (b) a package planet key plus `planet_name_key`. |
| `ai_class` | `"A"`, `"B"` or `"C"`. The original takes the AI class from the course's tournament order (`D:\Programming\GitHub\RacerRevengePC\src\game\ai_cold_owners50iu.h:92-93`: order < 5 A, < 9 B, else C); an add-on course has no order, so the class is data. |
| `rival` | racer reference for the course rival (`"stock:16"` or `"<pkg>/<item>"`); the original keeps one rival per course in the course descriptor (`D:\Programming\GitHub\RacerRevengePC\src\game\original_course_descriptor50kg.h:14-29`). |
| `loading_screen` | Every add-on track uses the stock race loading presentation, with stock background art and the stock progress indicator. The N64 and arcade builders select `{"stock":"tb"}`. The 3D track preview belongs on the Course selection screen. |
| `course_screen` | `{ "archive": "UI/EB_UI.RES", "hologram": "<widget stem>", "preview": "<member>" }`, or `{ "stock_template": "SC" }` to draw the stock hologram plane with the package's preview image. |
| `hall_defaults` | optional default Hall rows (lap/race/KO times and names), same shape as one original Hall row; absent = empty rows. |
| `records_key` | default = item id. Change it to reset records (D10). |
| `modes` | subset of `["single", "time_trial", "circuit"]`; `circuit` only with the tournament phase. |
| `enhancements` | informational list, e.g. `["surface_rules", "zero_g", "boost_strips"]`. The rules themselves come from the archive's own TerrainZone rows, exactly as on converted tracks today (`D:\Programming\GitHub\RacerRevengePC\work\scratch\ice\NOTES.md`, `D:\Programming\GitHub\RacerRevengePC\work\scratch\zero-g\NOTES.md`); the validator logs whether the data authors them. |

**`pod`**

| Field | Rule |
|---|---|
| `stem` | the name the pod archive's members use, replacing the original's `Pod%02d` (`pods\a2r_ody\a2r_ody.phy`). `[a-z0-9_]{3,16}`, must not match `pod\d\d`, unique among admitted packages. |
| `archive` | package path of the pod RES. PHY, speech (`sounds.ini [Indices]`), model, wreck pieces are all inside it, as for stock pods. |
| `name_key` | display name key (Rider screen, HUD, Results). |
| `ui` | `{ "archive": "UI/A2R_ODY_UI.RES", "rider_model", "portraits": [...], "background", "results_icon", "stats_image", "garage_model" }`: member names inside the package's own UI archive. |
| `voice` | optional `{ "ini": "AUDIO/A2R_ODY/ACCEPT.INI", "first": 1, "count": 2 }`: Rider-screen confirm voice, same INI format as stock `UIACCEPT.INI`. Absent = no confirm voice. |
| `announcer` | optional path to an announcer INI in the stock `PODnnANN.INI` format. Absent = the announcer never names this racer and plays only generic lines (the original's own behaviour when a racer has no named bank). |
| `ai` | `"addon_courses"`, `"never"` or `"anywhere"` (D6). |
| `unlock` | `"always"` in the free-play phases; later `{ "rule": ... }` (D5). |

**`menu_assets`**: extra UI resources an add-on item needs (fonts excluded): `{ "archive", "members": [...] }`.
Only add-on entries can reference them. Changing a stock menu is a replacement, never `menu_assets`.

**Replacement-only kinds** do not exist as items; replacements are listed in `replacements` (2.4).

### 2.4 Replacements

```json
"replace": true,
"replacements": [
  { "target": "TRACKS/SC.RES",
    "expects": { "bytes": 2974353, "sha256": "8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d" },
    "with": "TRACKS/SC.RES" },
  { "target": "UI/UIPODTEX.RES", "member": "UI\\Textures\\16_1.pst",
    "expects": { "bytes": 0, "sha256": "<stock member sha>" },
    "with": "UI/16_1.pst" }
]
```

- `target` is a stock path (case-insensitive, `/` or `\`); `member` (optional) names one member inside a
  stock archive (member-level replacement, a later phase; whole-file replacement first).
- `expects` is the stock file or member the replacement was built for. If the installed stock file does
  not match, the package is refused ("built for a different stock file"). This also stops two packages
  from stacking on each other.
- The replacing bytes then go through the same admission the port applies to any non-stock archive
  (the receipts introduced by 8b55ef3, 983d031, b429596). A replacement is not trusted more than a disc
  file would be.

### 2.5 Priority, conflicts and dependencies

1. Read every `dlc.json`; skip `enabled: false`.
2. Validate each package on its own (schema, files, sizes, hashes, file types, content checks of
   section 5). Any failure refuses the whole package.
3. Resolve dependencies: missing, refused, disabled or wrong version refuses the dependent package;
   cycles refuse every package in the cycle. Optional dependencies only order things.
4. Resolve `conflicts` entries.
5. Resolve collisions, in the order (priority high first, then id alphabetical):
   - replacement target (or target member) claimed twice: the higher priority wins; the loser is
     refused as a whole; equal priority refuses all of them (no silent winner);
   - course code or pod stem claimed twice: the first in that order keeps it; the later package is
     refused as a whole;
   - string keys are namespaced by package (`<package-id>.<KEY>`), so they never collide.
6. Assign add-on numbers in that same order (section 4.1). The order depends only on the json files, so
   the same set of packages always gives the same numbers in one session. Numbers are never saved.

A package is all or nothing: the player never gets half a package.

---

## 3. Virtual file overlay

### 3.1 Today

- One resolver exists: `runtime_asset_paths50LB::resolve`
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\runtime_asset_paths50lb.h:37-50`), 48 call sites.
- But disc paths are also built directly from a disc root in about 44 files (`discRoot/"PODS"/...`,
  `D:\Programming\GitHub\RacerRevengePC\tests\native\original_player_identity50ik_fixture.h:219-222`,
  announcer INIs `D:\Programming\GitHub\RacerRevengePC\tests\native\source_announcer_authoring50ie_fixture.h:99-100`,
  course archives, UI archives `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_archive50kg.cpp`).
- The root itself comes from `RR_ORIGINAL_ASSET_ROOT` in about 10 files
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_session50du.cpp:160-180` and others).

### 3.2 Design

One new owner, `DlcFiles` (new `src\assets\dlc_files50dl.h/.cpp`), is the only place that turns a
game path into a host file. Everything else asks it.

```
open(path):
  if path starts with "dlc:/<id>/":            add-on file
      <id> must be an admitted package, the rest must be a listed file of it  -> package file
      otherwise refuse (never falls back to stock, never leaves the package folder)
  else:                                        stock path
      normalise (fold case, '\' == '/')
      if an admitted replacement targets exactly this path -> the replacing package file
      else -> <stock root>/path   (exactly today's behaviour)
```

- Stock code asks for stock paths. Add-on items are referred to only through the registry, which hands
  out `dlc:/` paths. No add-on file has a stock spelling, so nothing a package adds can shadow a stock
  file; only a flagged replacement can change what a stock path opens.
- With no admitted package the stock branch is the only one taken and returns the same path as today.
- Loose add-on files (PST, INI) are compared with their startup hash when opened; archives are re-hashed
  anyway by the existing admission receipts. A file changed on disk after startup refuses that load with
  a clear message instead of using unchecked bytes.
- Member-level replacement (later): when a stock archive is mounted
  (`D:\Programming\GitHub\RacerRevengePC\src\assets\original_standard_archive_provider50la.h`,
  `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_archive50kg.cpp`), the mount asks
  `DlcFiles` for member overrides by name and builds the decoded member list with the override bytes.
  Blocker: UI members are still addressed by ordinal (stand-in noted in b429596), so member-level
  replacement inside UI archives waits for that stand-in to go.
- Phase 1 moves every site that opens a file a package may replace (track, pod and UI archives, LOC,
  loose announcer and accept INIs, loading PST) onto `DlcFiles`; the remaining direct-root sites move as
  later phases need them. The function sits behind a narrow header so that changing the overlay does not
  touch large headers.

---

## 4. Adding new slots

### 4.1 One registry, one layout

New `src\assets\dlc_registry50dl.h/.cpp` is the single place that defines add-on storage:

- `std::vector<AddonCourse>` and `std::vector<AddonPod>`, sized once at startup from the admitted
  packages, then frozen (no growth during play, so no pointer moves while menus or races hold entries).
- Add-on course numbers: 32, 33, ... ; add-on racer numbers: 32 .. 99 (the port already refuses racer ids
  of 100 and above, `D:\Programming\GitHub\RacerRevengePC\src\game\original_racer_entity_cold50la.h:675`),
  so at most 68 add-on pods; add-on courses capped at 64. Starting at 32 keeps every add-on number outside
  the original encodings (`D:\Programming\GitHub\RacerRevengePC\src\game\original_course_load_selection50kg.h:11-13`:
  course < 13, planet < 8, rider < 32), so an add-on number written by mistake into an original field is
  refused, not stored.
- Accessors used by the common paths: `course(index)` and `pod(racer)` return either the stock row (from
  the original tables, unchanged) or the add-on row (from the registry). Every index is bounds-checked
  before use. This is a data lookup, not a code branch: the same race load and pod load run on whatever row
  comes back.
- The registry never writes into any original table, profile field or save byte.

### 4.2 Courses: every fixed count and table, and how it is extended

| What | Where (original / port) | Fixed count | Add-on extension |
|---|---|---|---|
| Course descriptor (code, rival) | 17EA1C..17EEC8 -> profile 11278 stride 12; `D:\Programming\GitHub\RacerRevengePC\src\game\original_course_descriptor50kg.h:14-29` | 13 | registry rows; `course(index)` gives code and rival; original 13 untouched |
| Course list and planets on the Course screen | `UI/UICourse/track.dat` Control sections (CourseIndex, Planet); `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_course_selection50kg.cpp:28-36` (CourseIndex 0..12, six planet names) | 13 courses, 6 planets | the page reader appends port-owned controls after the authored ones (limit 256, `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_pages50kg.h:13`), one per add-on course, in the add-on planet (D1). Their layout copies the authored control of the same group; text, image and hologram come from the package |
| Planet field in the profile | byte 1125E bits 5-7 (`original_ui_course_selection50kg.h:63-64`, written at 1DFA44/1DFC54/1DA618) | 3 bits, values 0..5 used | not used for add-ons: values 6/7 would invent meaning in an original byte. The add-on planet selection lives in port-owned menu state; 1125E keeps the last stock planet |
| Hologram widgets | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_course_hologram_assets50kg.cpp:57` (13 admitted widget ids) | 13 | add-on hologram rows from the package UI archive (`course_screen`), drawn by the same hologram code; or the stock plane with the package preview |
| Course-screen soultree ordinals | `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_entity_scope50kg.h:3472` | 13 | registry list after the stock list |
| Course race domain (course, planet, slot) | `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_course_race_domain50kg.h:17-33` | 13 | registry answers for add-on numbers |
| Course launch selection | `D:\Programming\GitHub\RacerRevengePC\src\game\original_course_load_selection50kg.h:11-13`, packed 1125C | course < 13 | port-owned selection handle carries the add-on number; original packed fields keep the stock-valid values |
| Course availability (open courses) | profile 11018 bits 0-4 (4..13 open) | 13 | add-on courses are always open in free play (D3); 11018 unchanged |
| Tournament order, prizes, circuits | `D:\Programming\GitHub\RacerRevengePC\src\game\original_tournament_result50kg.h:25-26`; copies in `original_ui_course_widget_controller50kg.cpp:20,23` and `original_ui_hall_controller50kg.cpp:15,40` | 13, 3 circuits ending at orders 3/7/12 | untouched. Add-on circuit (later phase): its own order and prize list from the package, its own progress in the side file |
| Career medals and remaining prizes | career +0x18 (13 x u8), +0x26 (13 x s16); `original_tournament_result50kg.h:11-12` | 13 | side file, keyed `<pkg>/<item>` |
| Hall of Fame | profile 0x102F0 + c*0xE4, c = 0..12; `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_hall_records50kg.h:32`; widgets `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_hall_widgets50kg.h:10` | 13 rows | add-on rows in the side file; Hall page shows add-on courses after the 13 stock rows (same row widget, add-on name) |
| Record insertion / qualification | 1AC440; `D:\Programming\GitHub\RacerRevengePC\src\game\original_race_record_qualification50kg.h:94-107`; race-side `D:\Programming\GitHub\RacerRevengePC\src\game\dynamic_impact50iw.h:57-58` (13) | 13 | same rule (cheat slots, mode 2 skip), target row from the registry/side file |
| Unlock checks | 187E30; `D:\Programming\GitHub\RacerRevengePC\src\game\original_unlock_checks50pd.h:19-24` | 13 | untouched; add-on records never feed stock unlocks |
| Course identity tables (cut cameras, intro, material audio) | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_track_cut_identity50la.h:26`, `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ta_intro_authoring_candidate.h:209-269`, `D:\Programming\GitHub\RacerRevengePC\tests\native\material_audio_provider50hi_fixture.h:55` | 13 | none needed: non-stock archives already get their rows from their own data (8b55ef3); add-on archives take that same route |
| Per-course loaders | 13 stages / worlds / loaders (`D:\Programming\GitHub\RacerRevengePC\work\scratch\branch-inventory\PLAN.md` B-C04..B-C17) | 13 | **blocker**: must be the common race load (common-race T, W1-W6, F). No add-on course runs before that |
| Loading screen | `LD_<code>.PST`, pinned: `D:\Programming\GitHub\RacerRevengePC\tests\native\authored_loading_presenter50du.h:129-136`, `D:\Programming\GitHub\RacerRevengePC\tests\native\original_loading_background_hashes50du.inc` | 13 codes + UI/END/CC | package file or stock planet screen (`loading_screen`); the pin becomes a receipt, as for archives |
| HUD minimap | map spline `tracks\m<code>.spl` inside the course archive (`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\notes\hud-map.md`) | none (data) | nothing to do: comes from the add-on archive |
| PHY class | `authored_parameters49n.cpp:106-118` (by code) | rule | same rule on the add-on code |
| AI class | `ai_cold_owners50iu.h:92-93` (by tournament order) | rule | `ai_class` from the package (an add-on course has no order) |
| Course name strings | LOC (`D:\Programming\GitHub\RacerRevengePC\src\assets\original_application_localization50kg.h:57`, 467 English rows) | 467 | package strings (4.5) |
| Race music | not yet checked whether the original picks songs per course | ? | open question 8.2 |

### 4.3 Pods: every fixed count and table, and how it is extended

| What | Where | Fixed count | Add-on extension |
|---|---|---|---|
| Pod archive and name formatting | 21081C/210820 `Pod%02d`; `D:\Programming\GitHub\RacerRevengePC\tests\native\original_player_identity50ik_fixture.h:49,219` | 23 stock pods, ids 1..26 | registry `pod(racer)` gives the archive path (`dlc:/...`) and the member stem; for stock it returns the original `PODS/PODnn/PODnn.RES` and `Podnn`. The identity row of a non-stock archive is already built from its own data (b429596) |
| Racer selection / name table | `D:\Programming\GitHub\RacerRevengePC\src\game\original_racer_selection50ij.h:20-51` | 8 + 1 + 14 | registry entries after the stock ones |
| Course racer domain | `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_course_race_domain50kg.h:12` | 23 | registry answers for 32..99 |
| Careers, unlock words | profile 0x1920 + k*0x894 (27 careers), 0x10FAC [27] (slots 0/5/6/13 are cheat flags); `D:\Programming\GitHub\RacerRevengePC\src\game\original_tournament_result50kg.h:15,20`, `D:\Programming\GitHub\RacerRevengePC\src\game\original_unlock_checks50pd.h:13` | 27 | untouched. Add-on unlock state and (later) add-on careers in the side file |
| Default unlocks, hidden racers, 8 unlock checks | 186EF0 (2,4,10,11,12,15,19,20 unlocked; 1,14,21,25,26 hidden), 187E30; `original_unlock_checks50pd.h:39-76` | fixed racers | untouched; add-on pods follow D5 |
| Rider screen widgets | `UI/UIRider/Widgets.Dat`, tournament table `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_rider_pod_assets50kg.cpp:40-42` (27), unlock widgets 9..23 (`:50-54`) | 27 | port-owned widget rows built from the authored rider widget's layout with the package's model and image names; appended after the stock riders (D4) |
| Rider portraits and stats | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_rider_stat_assets50kg.cpp:22` (27); stats from `UI/PODPHYS.RES` (23 PHY members, catalog `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_archive_catalog50kg.inc:16`) | 27 / 23 | portrait from the package UI archive; stats read from the PHY inside the add-on pod's own archive (same reader) |
| Pod textures, backgrounds | `UI/UIPODTEX.RES` (`nn_1..3`), `UI/UIBKG.RES` (`nn_bkg`), `UI/UITOURN/UITOURN.RES` | per stock pod | package UI archive members named in `ui` |
| Garage per-pod model | `UI/UIGARAGE/nnE.RES`, 23 catalog rows (`original_ui_archive_catalog50kg.inc:17-39`); DefaultMT rule `%02d_%d` careers 1-26 | 23 | `ui.garage_model`; Garage only with the tournament phase. Upgrade prices and limits are rules plus the pod's own PHY, nothing to extend |
| Rider confirm voice | `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_rider_voice_choice50kg.cpp:13-38` (UIACCEPT event ranges per rider) | 23 | `voice` from the package, played by the same voice-choice code with the package range |
| Announcer named lines | `PODnnANN.INI` per pod, named banks [27]: `D:\Programming\GitHub\RacerRevengePC\tests\native\source_announcer_authoring50ie_fixture.h:19,88-126` | 27 | `announcer` from the package into a port-owned bank row; absent = generic lines only |
| In-race / Results display name | comment line in `UIACCEPT.INI` (stand-in, `D:\Programming\GitHub\RacerRevengePC\tests\native\original_racer_visual50hw_fixture.h:26-47`) | per stock pod | `name_key` from the package |
| Results portrait/icon | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_standard_material_object50la.cpp` (two sites, e9d3754) | per stock pod | `ui.results_icon` |
| AI opponent field | port field policy, `D:\Programming\GitHub\RacerRevengePC\tests\native\original_course_roster50kg_fixture.h:2-5` (itself a stand-in for the original opponent build) | 8 racers | stays 8; add-on pods join only as D6 allows |
| Original racer-keyed rules | AI steering gain for racers 4/8/18/23, announcer variant for racer 16 (`work\scratch\branch-inventory\PLAN.md` A2, A3) | fixed racers | kept exactly; an add-on number never matches them |

### 4.4 What the player sees (recommended choices)

- **Course screen:** after Mon Calamari the planet cycle continues to one more planet, "ADD-ON". Its
  hologram cycle lists every admitted add-on course (name from the package, hologram or preview from the
  package). Records shown under the course come from the side file. Laps, entrants and Time Trial work
  as on stock courses. Nothing changes on the six stock planets.
- **Rider screen:** after the last stock rider the carousel continues with the add-on pods, then wraps to
  the first stock rider. The name strip shows the package name under the pilot name. Stats bars come from
  the pod's own PHY.
- **Tournament:** unchanged (13 stock courses, stock riders' careers). An add-on circuit, if wanted, is a
  separate later phase.
- **Hall of Fame:** the 13 stock rows as today, then one row per add-on course.
- **Race:** The package selects a stock loading screen and supplies the minimap, sky, surface rules and announcer lines.

### 4.5 Strings

The original text table is looked up by key through a hash (`original_application_localization50kg.h`,
467 English rows). Package strings go into a port-owned extension table searched after the stock rows,
with keys namespaced `<package-id>.<KEY>`, so a package can never redefine a stock string (only a flagged
member replacement of LOC could). Missing language -> the package's English text. Limits: 4096 keys per
package, 255 bytes per string, printable characters from the fonts the menus already load (characters the
font lacks are refused at validation, not drawn as garbage).

---

## 5. Validation and logging

### 5.1 When

At startup, before the first menu archive is opened, in one pass, on the main thread. The registry is
frozen afterwards; nothing is admitted later in the session.

### 5.2 Checks (any failure refuses the whole package)

1. `dlc.json`: size, UTF-8, schema version, required fields, field types and ranges (section 2).
2. Folder: name equals `id`; every listed file exists, nothing unlisted is present, no symlinks/junctions,
   no traversal, no refused file types.
3. Every file: size equals `bytes`, SHA-256 equals `sha256` (about a second for a few hundred MB).
4. Item content, using the same rules the port applies to non-stock disc files, never weaker:
   - course archive: shared course-archive capacity (64 MiB file / 128 MiB decoded / 10000 entries,
     `course_archive_receipt50ci.h`, 8b55ef3), members found by name with the declared `code`, the
     storage limits the converter already reports (map/AI nodes <= 256, zones/portals within topology
     storage, collision owners <= 1021, texture budgets), and the archive must be loadable by the common
     race load's own checks (dry decode, no race state touched);
   - pod archive: shared pod capacity (16 MiB, 512 entries, 8 KiB text members, b429596), PHY present
     and parseable, speech counts within their uint8 storage, piece counts as the common pod load reads
     them;
   - UI archives and PST: the UI archive capacity and the PST/IPU decoders' own checks;
   - INIs: the existing INI readers with their size limits;
   - strings: limits of 4.5.
5. Replacements: `replace: true`, target is a stock path, `expects` matches the installed stock bytes,
   replacing bytes pass the same checks as 4.
6. Dependencies, conflicts, collisions (2.5).

### 5.3 Log lines (one per package, one per item, one summary)

```
dlc_root path=<abs> packages=4
dlc_package id=etti-iv version=1.0.0 status=admitted items=1 replacements=0
dlc_item key=etti-iv/etti-iv kind=course code=eb number=32 phy_class=B ai_class=B surface_rules=1 zero_g=0
dlc_package id=arcade-sc-replace version=1.0.0 status=admitted replacements=1
dlc_replace target=TRACKS/SC.RES by=arcade-sc-replace stock_sha=8305641f... new_sha=1aa853d7...
dlc_package id=broken-pod version=0.3.0 status=refused reason="file PODS/X/X.RES sha256 differs from dlc.json"
dlc_registry packages=3 refused=1 courses=1 pods=0 replacements=1
```

With no DLC folder: `dlc_root path=<abs> packages=0` and nothing else. Refusal reasons are written for a
player: which file, which check, expected versus found. The game always continues with stock plus the
admitted packages; a refused package never stops startup. If D9 picks the on-screen notice, the title
screen shows once: "Some add-ons could not be loaded" with the package names; details stay in the log.

### 5.4 Stock must stay bit-identical

- No DLC folder, empty folder, or `RR_DLC=0`: the registry is empty, the overlay takes only its stock
  branch, no side file is created, no random number is drawn, no menu control is appended.
- DLC present but not selected: stock races identical (D6 keeps add-on pods out of stock opponent fields);
  only the menus gain entries.
- Verification for every phase: the muted, logs-only 13-course round with rotated pods (as in
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\archive-admission\regress-140g.txt`) run twice:
  once with no DLC folder, once with packages installed but not selected. Results, receipts and FPS must
  match the previous build. Then a run per add-on item. A captured frame or the user's own look confirms
  the new menu entries are really on screen (the log alone does not prove that).

---

## 6. Save data

### 6.1 Stock save untouched

- Today `progress-v1.bin` (RRPB, 106,764 bytes,
  `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\original_ui_rider_profile50kg.cpp:1642-1735`);
  after draft-saves lands, the original 0xF508 `BASLUS-20268` file
  (`D:\Programming\GitHub\RacerRevengePC\work\scratch\draft-saves\NOTES.md`). Neither format changes.
- Nothing produced by an add-on item is written into it: no add-on Hall row, no add-on career, no add-on
  number in remembered selections. Today's RRPB file saves remembered pod and course (audit-progression
  gap G2); when an add-on item is the current selection, the stock file keeps the last stock selection.
- Under D7 (b), records set by an add-on pod on a stock course go to the side file, not the stock Hall.

### 6.2 DLC side file

`<save folder>\dlc-progress-v1.json` (same folder and same save moments as the stock save; written only
when DLC progress changed; atomic replace through the same writer; never created when no package was
ever admitted).

```json
{
  "schema": "rrpc-dlc-progress/1",
  "courses": {
    "etti-iv/etti-iv": {
      "records_key": "etti-iv",
      "archive_sha256": "1aa853d7...",
      "hall": {
        "lap":  [ { "time": 41.27, "name": "ANAKIN", "racer": "stock:19" } ],
        "race": [ { "time": 128.40, "name": "ANAKIN", "racer": "stock:19" } ],
        "knockouts": [ { "count": 3, "name": "ANAKIN", "racer": "stock:19" } ]
      }
    }
  },
  "stock_course_addon_pod_records": {
    "TA": { "lap": [ { "time": 38.10, "name": "ODY", "racer": "arcade-pods/ody" } ] }
  },
  "pods": { "arcade-pods/ody": { "unlocked": true } },
  "careers": {}
}
```

- Keys are item keys and stock codes, never slot numbers; racer references are `stock:NN` or item keys.
- Shapes and limits mirror the original rows (3 entries per table, 21-character names), bounds-checked on
  load; a malformed file is reported once and set aside as `dlc-progress-v1.bad.json`, never half-loaded;
  the stock save is not affected either way.
- `careers` stays empty until the tournament phase.

### 6.3 When a package is removed or changes

- Removed or refused: its entries stay in the side file untouched and are not shown; log
  `dlc_progress kept_unloaded key=etti-iv/etti-iv`. Putting the package back shows them again. The side
  file is only rewritten with those entries carried over unchanged.
- New version, same `records_key`: records kept; the stored `archive_sha256` is updated and the change is
  logged. New `records_key`: the old entries are kept under the old key (not shown), the new key starts
  empty (D10).
- A selection that pointed at a removed item (menus only; selections are not saved in the original
  layout) falls back to the stock default the original uses at boot.
- Deleting the side file resets only DLC progress.

---

## 7. Migration from the arcade silo

### 7.1 Today

- The arcade converter (`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\trk_convert.py`,
  `pod_convert.py`, `ui_convert.py`, with `trk_silo.py` / `pod_silo.py` / `ui_silo.py`) writes whole
  copies of the install (`work\scratch\racer-arcade\silo\assets`, `assets-smr`, `assets-bwr`, ...) with
  the converted files in place of stock ones, plus `converted-content.json`
  (`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets\converted-content.json`:
  SC.RES, POD04/11/15/16/17/23/24, LOC.RES, PODPHYS, OVERLAYS, UIBKG, UIPODTEX, UIRIDER, UITOURN,
  UIUNLOCK, 16E.RES; each with new and replaced size/SHA, facts and limits).
- Main has no reader of that file; it works because main's admission now accepts non-stock archives
  (8b55ef3, 983d031, b429596, e9d3754).

### 7.2 Step A (with phase 1): replacement packages

- New converter output `--dlc-out <folder> --replace <code>`: writes a package folder with `dlc.json`
  (`kind: replacement`, `replace: true`, one `replacements` row per file, `expects` = the stock size/SHA
  that `converted-content.json` already records as `replaces`, `files` with sizes and SHAs, `facts` and
  `limits` copied as informational).
- Packages: `arcade-etti-iv-sc`, `arcade-spice-mine-run-sc`, `arcade-beedos-wild-ride-sc` (all replace
  `TRACKS/SC.RES`, so at most one is enabled at a time, or priority picks), `arcade-pods` (the seven pod
  archives plus the UI and LOC archives they need).
- Check: the e9d3754 converted-set runs (POD04, 11, 15, 16, 17, 23, 24 on TA; POD16, POD23 on converted
  SC) give the same results with packages over the stock install as with the silo copy.
- Then `converted-content.json` and the whole-install silo copies are no longer needed (deleting the
  scratch copies is the user's call).

### 7.3 Step B (with phases 2 and 3): add-on packages

- Courses: convert with the natural code (`trk_convert.py 1` without `--code-as sc`, giving `eb`),
  members renamed accordingly. Borrowed stock members (`sc.aub`, `introAud.ini`, `scrptLns.ini`,
  `demoprt.*`, `sc.tun` today) become the course's own members under its own code. The loading screen
  uses the stock race presentation selected by `{"stock":"tb"}` in the package manifest.
- Pods: new stems (`a2r_ody` ...) instead of POD04/11/...; UI resources go into the package's own UI
  archive instead of edited stock UI archives; display names, voice and announcer from the package.
- The replacement packages from step A are then retired.

### 7.4 What a converter must never do

Write into the stock install; emit a file it does not list; emit a stock-colliding course code or pod
stem; ship executable content.

---

## 8. Phased build plan

Sizes are in files and rebuild rounds. TU counts are first estimates and must be measured with
`D:\Programming\GitHub\RacerRevengePC\work\scratch\ra-route2\fanout.py` before scheduling. One builder
per tree; batch header changes; never edit mid-build.

| Phase | What the player gets | New files | Edited files | Rounds | Depends on |
|---|---|---|---|---|---|
| **1. Packages + replacements** (smallest useful) | arcade content as packages over an untouched stock install; bad packages refused with a message | `src\assets\dlc_package50dl.h/.cpp` (json + validation), `src\assets\dlc_registry50dl.h/.cpp` (the one layout), `src\assets\dlc_files50dl.h/.cpp` (overlay); tools `tools\dlc\dlc_pack.py`, `tools\dlc\dlc_check.py` (same checks, offline) | `tests\native\runtime_asset_paths50lb.h` (one call into the overlay), pod path `original_player_identity50ik_fixture.h`, course archive opens, `src\assets\original_ui_archive50kg.cpp`, loading presenter, announcer/accept INI opens, startup in `tests\native\original_ui_main_entry50kg.cpp`, `platform\windows\native\interactive50du.sources.json`, Linux CMakeLists; converter `--dlc-out`: about 15-25 files | 1 header round (the resolver header reaches about 50 headers' includers) + single-TU fixes | nothing; can start now |
| **2. Add-on pods, free play** | new pods on the Rider screen, racing on every course in Single Race / Time Trial | `src\game\dlc_rider_entries50dl.h/.cpp` (menu rows from the registry) | racer selection table, course race domain, pod identity `find`, Rider page/pod/stat assets, voice choice, announcer authoring, display name, Results icon, roster policy, side file (pods + D7 records): about 15-20 files | 2 rounds (one is the UI H1-class set, about 135 TUs; plan it together with other H1 work) | phase 1; UI ordinal stand-in for any change inside stock UI archives (add-ons avoid it by shipping their own UI archive) |
| **3. Add-on courses, free play** | the ADD-ON planet on the Course screen; races, loading screens, records | `src\game\dlc_course_entries50dl.h/.cpp` | course descriptor accessor, Course page reader (appended controls), hologram assets, soultree ordinals, race domain, launch selection handle, loading presenter, strings extension in the localization owner, Hall page and records, AI/rival lookup: about 15-20 files | 2 rounds (UI H1-class + race session headers) | phase 1; **common-race T, W1-W6 and F committed** (SCHEDULE batches 11-19) |
| **4. Add-on circuit, careers, Garage, unlock rules** | optional add-on tournament circuit; add-on pods with careers and Garage upgrades | `src\game\dlc_circuit50dl.h/.cpp` | Tournament session, Garage page owner/input, Unlock page, rider profile, side file careers: about 12-18 files | 2 rounds | phases 2-3; draft-saves batch 3b (save moments) |
| **5. Member-level replacement, more menu kinds, languages** | small mods that change one texture or string without shipping whole archives | none or one | archive provider mount, UI archive mount, localization: about 5-8 files | 1 round | UI ordinal stand-in retired |

Phase 1 alone is useful (it retires the silo copies and `converted-content.json`). Add-on courses cannot be
the first phase because a new course code has nothing to load it until the common race load exists; the
honest order is replacements, then add-on pods, then add-on courses.

Verification each phase: section 5.4 (no-DLC and DLC-unselected stock rounds must match the previous
build), then every add-on item with rotated pods, muted, logs only, plus one frame or user look at each new
menu entry.

---

## 9. Risks and open questions

### 9.1 Risks

1. **Common race load is the gate for add-on courses.** Its own estimate is about 11 header rounds and
   the riskiest step is T (the 1A9EE0 chain has only run on RA). Until it lands, the only way to race a
   converted course is a flagged replacement of a stock course.
2. **Disc-path sites outside the resolver.** About 44 files build disc paths from a root directly. Any
   site missed in phase 1 would silently open the stock file instead of a flagged replacement. Mitigation:
   phase 1 adds a `dlc_open` log receipt per opened replaced path, and the regression checks that every
   replacement target logs one.
3. **UI addressed by ordinal.** UI members must keep the catalog's names, order and count (stand-in left
   in b429596). Add-on menu assets avoid it by living in their own small archives; member-level stock UI
   replacement must wait.
4. **Menu page extension.** Appending controls to the Course and Rider pages is new behaviour in pages the
   original builds from `.dat` files. It must use the page's own layout, focus and draw order, and is the
   part most likely to look wrong; it needs a captured-frame check, not just logs.
5. **Opponent field stand-in.** The port's opponent selection is itself a native policy
   (`original_course_roster50kg_fixture.h:2-5`). D6 depends on it; when the original opponent build is
   ported, add-on eligibility must be re-checked so stock races stay identical.
6. **Two-letter course codes collide.** 676 possible codes; third-party packages will collide sooner or
   later. Collisions are refused with a clear message; widening the code (2-8 characters) is possible
   because the PHY class rule already accepts any length, but every port site that stores a course code
   as two characters would need widening first.
7. **Hash cost and memory.** Hashing every package file at startup adds time proportional to package size
   (seconds at most). Add-on assets load only when the page or race needs them, within the same per-course
   and per-pod budgets as stock content.
8. **Distribution.** Packages built from a player's own disc that contain stock bytes (borrowed members)
   should not be shared. See 9.2 item 4.

### 9.2 Open questions

1. D1-D11 above.
2. Does the original pick race music per course? If yes, add-on courses need a `music` field; if not,
   nothing. (Not checked in this pass.)
3. Should add-on courses support split-screen VS once VS is ported? (Same course path, so likely yes.)
4. Should a package be allowed to reference stock content instead of copying it ("borrow
   `tracks\sc.aub` from the player's own `TRACKS/SC.RES`")? This keeps shareable packages free of stock
   bytes but needs a virtual-archive builder; proposed for phase 5.
5. Should add-on pods appear in the Unlock page and the Art gallery? Proposed: no until phase 4.
6. Is a per-package on/off toggle wanted in the game's Options page, or is editing `enabled` in the json
   enough? (An in-game page is new UI; the json flag costs nothing.)
7. Packages for other ports of this project (e.g. a console build) would face the target's own memory
   limits; for original Xbox that is 64 MB in total, so add-on storage would have to be counted against
   the same budget as everything else.

## User decisions (2026-11-15)
All eleven recommendations accepted, with decision 2 changed: the extra planet/group is named "DLC".
1 one extra planet after Mon Calamari; 2 named "DLC"; 3 free play first, add-on circuit later, stock tournament never touched; 4 add-on pods after stock riders; 5 add-on pods unlocked at once in free play; 6 add-on pods as AI only on add-on courses; 7 add-on-pod records on stock courses go to the DLC save; 8 replacements when flagged plus a master switch (default on); 9 bad package: log plus one-time title message; 10 keep records unless the author changes the records key; 11 folder <install>\DLC.

## Step A decisions (coordinator, 2026-11-15, after input from the arcade session)

- SC test replacement packages, one active at a time (equal priority, so two enabled together refuse
  each other): `arcade-etti-iv-sc` (silo\assets), `arcade-malastare-sc` (silo\assets-lc),
  `n64-spice-mine-run-sc` (silo\assets-smr2), `n64-beedos-wild-ride-sc` (silo\assets-bwr). Built from
  copies only.
- Pods: Pugwis (Star Wars: Demolition) ships as its own package `demolition-pugwis` (stock slot 16),
  separate from `arcade-pods` (slots 04, 11, 15, 17, 23, 24). Both depend on one replacement package
  `shared-ui-arcade-demolition` (LOC.RES, UI/PODPHYS.RES, UI/TEXTURES/OVERLAYS.RES, UI/UIBKG.RES,
  UI/UIGARAGE/16E.RES, UI/UIPODTEX.RES, UI/UIRIDER/UIRIDER.RES, UI/UITOURN/UITOURN.RES,
  UI/UIUNLOCK/UIUNLOCK.RES) through `dependencies` (`>=1.0.0 <2.0.0`). In step A pods are replacements
  of their stock slots.
- New schema fields: `personal` (bool) and `personal_note` (text, default "built from your own copy of
  the source games; do not share"); every package with `personal: true` logs a `dlc_personal` line at
  startup.
- A track variant with animation (scroll) is a version bump of the same package, not a second package.
- Converters write packages directly (`--dlc-out`); `tools\dlc\dlc_pack.py` validates a package or a
  whole DLC root (sizes, hashes, schema, dependencies, conflicts, collisions, and with `--stock` the
  `expects` rows) and (re)generates the `files` list. Exact fields: `work\scratch\dlc\DLC-JSON.md`.

## Phase 1 as built (worktree branch worktree-agent-ade308915ee4ada42)

- `src\assets\dlc_package50dl.h/.cpp`: dlc.json reader and package checker (one package layout).
- `src\assets\dlc_files50dl.h/.cpp`: startup (one pass, main thread, before the first original file),
  dependency/conflict/collision resolution, `expects` against the installed stock bytes, and `host()`,
  the overlay every file-open site calls. The registry of section 4 is not needed yet (no add-on
  items); the replacement table in dlc_files50dl.cpp is the one layout for phase 1.
- Every original-file open in the game build (track, pod, UI, LOC, loading PST, audio, CAM, wreck,
  collision, effects) passes through `host()`: 25 calls in 23 files. Saves, logs, the corpus json and
  the Rider IPUM cache are not stock files and are not routed.
- `kind` other than `replacement`, non-empty `items` and member-level replacement are refused with
  "needs a later version of the port".
- Master switch: `RacerRevengePC.ini [DLC] Replacements=0` (read only when present; the game never
  writes it). `[DLC] Enabled=0` ignores the folder. Tests: `RR_DLC`, `RR_DLC_REPLACEMENTS`, `RR_DLC_ROOT`.
- A replacing file that changes on disk after startup refuses that open (logged, error raised) instead
  of using unchecked bytes; it never falls back to stock.
- Bad package: one `dlc_package ... status=refused reason="..."` line; the game continues with stock.
  The one-time title-screen message (D9) is NOT built yet (needs the shared dialog): phase 2.
- Not yet: dry-decode content checks at startup (section 5.2 item 4); the replacing archive is admitted
  by the game's own archive checks when it is opened, as any non-stock disc file is.
- Found while routing: the loading presenter reads LOC.RES with a cap of exactly the stock size
  (35925 bytes, `tests\native\authored_loading_presenter50du.h`), so a LOC replacement larger than
  stock would be refused there. Flagged, not changed (stand-in for the real LOC capacity).

## Phase 1 verification (build\dlc-p1a, muted, logs only, rotated pods; work\scratch\dlc\runs)

| Run | DLC root | Result |
|---|---|---|
| stock-SC-3, stocka-GC-22, stocka-MB-7 | none (no DLC folder) | same result screens and FPS as build140g; SC log differs from build140g only by the one `dlc_root ... packages=0` line; no file or folder created |
| ettia-SC-9, malastarea-SC-20, smra-SC-18, bwra-SC-12 | one SC package each | admitted; every SC load site reports the package bytes (stock=0); 3 result screens each, 58.5-60.3 FPS, no failures |
| badhasha-SC-2 | Etti with a corrupted hash | refused with the file and both hashes named; stock SC raced |
| switchoffa-SC-25 | Etti, replacements switch off | package switched off; stock SC raced |
| missingdepa-TA-4 | arcade-pods without its shared UI package | refused ("needs package shared-ui-arcade-demolition, which is not installed"); stock raced |
| podsa-TA-16 | shared-ui + arcade-pods + demolition-pugwis | all three admitted; 7 pods and 9 menu archives opened from the packages; Pugwis raced TA to Results |

Not checked by eye: no frame was looked at (tests are logs only). The packages are copies of the
silo folders as they were at 12:05 on Nov 15 2026 (the arcade session keeps changing them).

## Step B note (2026-11-15, coordinator)
Add-on courses: the original derives a course's PHY class from its code letter. For add-on courses the class must be an explicit dlc.json field (phy_class), read from the package; the code-letter rule stays for the 13 stock codes only. Codes stay natural (eb, lc, ns, nb). Arcade session's step-B proposals: D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\notes\dlc-out.md. Their regenerated step-A packages: D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\dlc-out\.

## Correction (2026-11-16)
Single Event course select has no planet list: arrows cycle courses by name; planet shows only as a motion-blurred background + rotating blue hologram card. Add-on courses follow the stock 13 in the arrow cycle; each needs name, course_background (own image or stock:<code>) and hologram. "DLC" planet naming applies only where planets are listed (tournament / add-on circuit later). User: "Looks like we only need a motion blurred background per planet, and a rotating blue card".

## Scope decision (2026-11-16)
User: "I think DLC being quickplay only is sufficient." Add-on courses and pods are free play only (Single Event / Time Trial). Phase 4 (add-on circuit, careers, Garage, unlock rules) is dropped. Stock tournament untouched.

## Two-player (2026-11-16)
User: "We will also need to test two player". Add VS/split-screen to verification: runner drives player 2 in-process; verify stock, then add-on course and add-on pod in VS once DLC phases land. Builder to report current 2P state first.

## Correction (2026-11-16, user)
"DLC racers can run tournament, they just don't appear as NPC racers. They get upgrades too." Add-on pods: playable as the player in the tournament and free play, with Garage upgrades; never AI opponents in the tournament (AI on add-on courses only). Add-on courses: free play only. Per decision 7, career/upgrade/tournament progress and records made with an add-on pod live in the DLC save file keyed by package+pod; the stock save is never touched.
- 2026-11-16 user: 2P VS mode also gets DLC pods and DLC tracks (both players may pick add-on pods; add-on courses selectable in VS).
- 2026-11-16 user: two-player is built after the DLC mods.
- 2026-11-16 user: Hall of Fame shows add-on records, merged at display time with stock records (best wins, add-on pod name/icon); stock save untouched; removing a mod drops its entries and stock rows show again. Also the "does it qualify" check must compare against the merged table, not stock only.
