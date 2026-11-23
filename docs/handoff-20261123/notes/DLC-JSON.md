# dlc.json field reference (phase 1, schema `rrpc-dlc/1`; add-on items at the end, contract `dlc-contract-2`)

For converters writing packages directly (`--dlc-out`). NOT ORIGINAL BEHAVIOUR: add-on content
is a PC enhancement. The game checks every rule below at startup
(`src\assets\dlc_package50dl.cpp`, `src\assets\dlc_files50dl.cpp`); the offline tool
`tools\dlc\dlc_pack.py` applies the same rules.

## Folder

```
<install>\DLC\<id>\dlc.json         (RR_DLC_ROOT overrides the DLC folder, tests only)
<install>\DLC\<id>\<every listed file, at its listed path>
```

- Folder name == `id`. Nothing in the folder except `dlc.json` and the files listed in `files`.
- No links, junctions, programs or scripts (`.exe .dll .com .bat .cmd .ps1 .vbs .js .py .sh .lua .so
  .dylib .scr .msi .jar .wsf .hta`, or any PE/ELF file).
- `dlc.json`: UTF-8 **without BOM**, one JSON object, at most 1 MiB.
- Limits: 4096 files, 512 MiB in total, paths at most 240 characters and 8 levels, relative,
  `/` or `\`, no `.`/`..` parts, no `: * ? " < > |`, no part ending in a space or dot.

## Top-level fields

| Field | Type | Required | Rule |
|---|---|---|---|
| `schema` | text | yes | exactly `"rrpc-dlc/1"` |
| `id` | text | yes | `[a-z0-9][a-z0-9-]{2,47}`, equals the folder name; never reused for a different package |
| `name` | text | yes | 1-63 printable characters |
| `author` | text | yes | at most 63 printable characters |
| `version` | text | yes | `MAJOR.MINOR.PATCH` (digits). A changed variant of the same content (for example a track with scrolling animation) is a version bump of the same package, not a second package |
| `description` | text | no | at most 1024 printable characters |
| `enabled` | true/false | yes | `false`: skipped (logged as disabled), files not checked |
| `kind` | text | yes | phase 1 loads only `"replacement"`. `course`, `pod`, `menu_assets`, `bundle` are refused by this build with "needs a later version of the port" |
| `priority` | whole number | no | -1000..1000, default 0. Settles two packages replacing the same stock file: the higher one wins, the other is refused; equal priority refuses both |
| `requires_port` | text | no | `"dlc-contract-1"` (anything else is refused) |
| `personal` | true/false | no | `true` for a package built from the player's own copy of the source games. Logged at every startup with the note below |
| `personal_note` | text | no | at most 255 characters; default `"built from your own copy of the source games; do not share"` |
| `replace` | true/false | no | must be `true` when `replacements` is not empty |
| `replacements` | list | no | see below; `kind: replacement` needs at least one |
| `items` | list | yes | must be `[]` in phase 1 (add-on courses and pods come later) |
| `files` | list | yes | every file of the package (not `dlc.json`) |
| `dependencies` | list | no | at most 64 |
| `conflicts` | list | no | at most 64 |
| `strings`, `facts`, other fields | any | no | ignored by phase 1 (kept for tools) |

### `files[]`

```json
{ "path": "TRACKS/SC.RES", "bytes": 1751846, "sha256": "36e59b9d...", "role": "optional free text" }
```
Size and SHA-256 (64 hex digits, any case) must match the file exactly. A path may be listed once
(case-insensitive). `dlc_pack.py files <package>` regenerates this list from the folder.

### `replacements[]` (whole-file replacement of a stock file)

```json
{ "target": "TRACKS/SC.RES",
  "expects": { "bytes": 2974353, "sha256": "8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d" },
  "with": "TRACKS/SC.RES" }
```
- `target`: a stock path relative to the install (case-insensitive). Not inside `DLC`.
- `expects`: size and SHA-256 of the stock file it was built for. If the installed stock file differs,
  the package is refused ("built for a different stock file"). This also stops two packages stacking.
- `with`: a path listed in `files`.
- `member` (one member inside an archive) is refused in phase 1.
- The replacing bytes then go through the game's normal admission for that kind of archive, exactly as a
  changed disc file would.

### `dependencies[]` and `conflicts[]`

```json
"dependencies": [ { "id": "shared-ui-arcade-demolition", "version": ">=1.0.0 <2.0.0", "optional": false } ],
"conflicts":    [ { "id": "some-other-package", "reason": "both change the pod menu" } ]
```
- `version`: space-separated comparators `= < <= > >=` with `MAJOR.MINOR.PATCH`, at most 4; empty = any.
- A required dependency that is missing, refused, disabled or the wrong version refuses the package;
  refusals cascade. Required-dependency cycles refuse every package on the cycle. `optional: true`
  never refuses.
- `conflicts`: the declaring package is refused if the named package is admitted.

## Whole-package rule

Any failure refuses the whole package with one `dlc_package ... status=refused reason="..."` log line;
the game continues with stock plus the admitted packages. Nothing in the stock install is written.

## Switches

- `RacerRevengePC.ini` `[DLC]` `Enabled=0` ignores the DLC folder; `Replacements=0` turns every
  replacement off (master switch). Keys absent = on. The game never writes these keys.
- Test process variables (explicit `--assets` launches only): `RR_DLC=0|1`, `RR_DLC_REPLACEMENTS=0|1`,
  `RR_DLC_ROOT=<folder>`.

## Log lines

```
dlc_root path="<abs>" packages=0                                  (no DLC folder: nothing else is logged)
dlc_root path="<abs>" packages=1 replacements_switch=on
dlc_package id=arcade-etti-iv-sc version=1.0.0 status=admitted priority=0 items=0 replacements=1
dlc_personal id=arcade-etti-iv-sc note="built from your own copy of the source games; do not share"
dlc_replace target=TRACKS/SC.RES by=arcade-etti-iv-sc stock_sha=8305641f0ac8fefe new_sha=36e59b9d2b57b8a6 file="<abs>"
dlc_registry packages=1 refused=0 courses=0 pods=0 replacements=1
dlc_open target=TRACKS/SC.RES by=arcade-etti-iv-sc file="<abs>"   (every time the game opens a replaced file)
```

## Minimal example

```json
{
 "schema": "rrpc-dlc/1",
 "id": "arcade-etti-iv-sc",
 "name": "Etti IV (replaces Sebulba's Legacy track)",
 "author": "RacerRevengePC converters",
 "version": "1.0.0",
 "enabled": true,
 "kind": "replacement",
 "requires_port": "dlc-contract-1",
 "personal": true,
 "replace": true,
 "replacements": [ { "target": "TRACKS/SC.RES",
   "expects": { "bytes": 2974353, "sha256": "8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d" },
   "with": "TRACKS/SC.RES" } ],
 "items": [],
 "files": [ { "path": "TRACKS/SC.RES", "bytes": 1751846,
   "sha256": "36e59b9d2b57b8a6450748ef682610416de4e63637fee742ca95826f1c16aeb4" } ]
}
```


---

# Add-on items: kind `course` and `pod` (contract `dlc-contract-2`, phases 2-3)

Written 2026-11-16 by the builder for the converters, before the port code exists. The port will check
exactly these rules; anything not listed is ignored (kept for tools). NOT ORIGINAL BEHAVIOUR. Original
tables, the stock save and stock files are never written; add-on numbers start at 32 in port-owned tables.

## Package level (changes to the phase-1 table)

| Field | Rule for add-on packages |
|---|---|
| `kind` | `"course"` (only course items), `"pod"` (only pod items) or `"bundle"` (any mix, plus replacements). Must agree with `items`. |
| `requires_port` | `"dlc-contract-2"`, required when `items` is not empty. |
| `items` | 1..256 items (rules below). |
| `strings` | `{ "english": { "KEY": "text" }, "french": {...} }`. KEY `[A-Z0-9_]{1,47}`; text 1..255 printable characters the menu fonts draw (others refused). English required for every key used; a missing language falls back to English. Keys are namespaced by the package automatically, so the json writes the bare KEY. |
| `facts` | free object, informational (provenance and validation results welcome). |
| `addon-manifest.json` | must NOT be shipped (an unlisted file refuses the package). |

## Every item

| Field | Rule |
|---|---|
| `kind` | `"course"` or `"pod"` |
| `id` | `[a-z0-9][a-z0-9-]{1,46}`, unique in the package. Item key = `<package-id>/<id>`; saves use the key, never a number. |

## `course` item

| Field | Req. | Rule |
|---|---|---|
| `code` | yes | exactly 2 characters `[a-z][a-z0-9]`; not a stock code (`ta tb tc ra rc ma mb mc ga gb gc sa sc sb`) and not another admitted package's code (first in priority/id order keeps it; the later package is refused). Every member the original names by course code uses it (`tracks\<code>.scb/.tob/.aub/.tun/.cub`, `tracks\m<code>.spl`, `tracks\a<code>.spl`, `<code>01.vub`, `<code>TVCam.pos`, ...). |
| `archive` | yes | package path `TRACKS/<CODE>.RES`; shared course-archive limits and the common race load's own checks. |
| `name_key` | yes | strings KEY of the track name (Course screen, loading, Results, Hall). |
| `planet_name_key` | no | strings KEY of the planet text shown with the track name (e.g. "OOVO IV"). Display text only. In free play (Single Event, Time Trial, VS) there is no planet list: add-on courses follow the 13 stock courses in the course arrow cycle. The planet name "DLC" is reserved for screens where the original lists planets (none in contract 2). |
| `phy_class` | yes | `"A"`, `"B"` or `"C"`. Replaces the original code-letter rule for add-on codes only. |
| `ai_class` | yes | `"A"`, `"B"` or `"C"` (the original takes it from tournament order; an add-on course has none). |
| `rival` | yes | `"stock:NN"` (NN 01..26, an existing stock racer), `"<package-id>/<pod-item-id>"` (falls back to none, logged, if that package is not admitted), or `"none"`. |
| `loading_screen` | yes | `{ "file": "UI/TEXTURES/LD_<CODE>.PST" }` (listed PST passing the loading presenter's layout rule; its fingerprint is a receipt, never a refusal) or `{ "stock": "<stock code>" }` (that stock course's screen from the player's install). |
| `course_background` | no | the motion-blurred full-screen course-screen background: `{ "file": "<listed PST path>" }` (the package's own image; same size/format as the stock course-screen background it stands in for, refused otherwise) or `"stock:<stock code>"` (reuse that stock course's background from the install). Absent = `"stock:<course_screen.stock_template>"`, logged as borrowed. |
| `course_screen` | no | the rotating blue hologram card: `{ "stock_template": "<stock code>", "preview": "<listed PST path>" }`: that stock course's hologram drawn with the package's preview image (same size/format as the stock texture it stands in for, refused otherwise). `preview` optional (absent = the stock course's own image, logged as borrowed). Absent `course_screen` = `{ "stock_template": "ta" }`. Own hologram archives are not in contract 2 (refused). |
| `inherit` | no | `{ "tuning": "<stock code>", "particles": "<stock code>" }`. `tuning`: when the archive has no `tracks\<code>.tun`, race damage / AI pace / packing are read at run time from that stock course's `.tun` in the player's install. `particles`: when the archive has no `particle\demoprt.prt/.pst/.psm`, they are read from that stock course's archive. Members in the archive always win (`inherit` then logged as unused). A missing member with no `inherit` refuses the package. |
| intro | - | no field: `audio\introAud.ini` with `NumEvents 0` and `audio\scrptLns.ini` with `NumberOfLines 0` are accepted (no announcer intro lines; the cut cameras still play). Absent INI members = 0 events. |
| `music` | no | reserved; ignored by contract 2 (whether the original picks songs per course is open). |
| `modes` | no | subset of `["single", "time_trial", "vs"]`, default all three (free play and two-player VS only; add-on courses never enter the tournament). `"circuit"` refused. |
| `records_key` | no | `[a-z0-9-]{1,47}`, default = item id. Change it to reset that course's records. |
| `hall_defaults` | no | `{ "lap": [r,r,r], "race": [r,r,r], "knockouts": [k,k,k] }`, r = `{ "time": seconds, "name": "TEXT" }`, k = `{ "count": n, "name": "TEXT" }`, names at most 21 characters. Absent = empty rows. |
| `enhancements` | no | informational (`surface_rules`, `zero_g`, `n64_floor_table`, `boost_strips`, `n64_hazards`); the port reads the rules from the archive data and logs what it found. |

## `pod` item

| Field | Req. | Rule |
|---|---|---|
| `stem` | yes | `[a-z0-9_]{3,16}`, not `pod\d\d`, unique among admitted packages. Replaces the original `Pod%02d` / `%02d` in every pod-specific name (rule of `racer-arcade\tools\a2r\pod_stem.py`): `Pods\<stem>\<stem>.phy`, `pods\<stem>\*.psg`, `Pods\<stem>\<stem>.ai`, `<stem>_*` textures/materials/motions, `flame<stem>l/r`, `concring<stem>`, speech folder `audio\speech\<stem>\`. PHY `[Pod Setup] Prefix` must equal the stem. Shared original names (cable, binder, Flame, concring, specmap*, particle\*.emt, audio\p_*.ini, wrecks.txt, pod_wndw, ...) keep their names. |
| `archive` | yes | `PODS/<STEM>/<STEM>.RES`; shared pod-archive limits; loaded by the common pod load (2129C0) and wreck chain (165E90) like every stock pod. |
| `camera` | yes | `PODS/<STEM>/<STEM>.CAM`, stock `PODnn.CAM` format. |
| `name_key` | yes | strings KEY of the pilot name (Rider screen, HUD, Results). |
| `short_name_key` | no | strings KEY where a short name is shown; default `name_key`. |
| material names | - | no field: every material/texture name in the pod and UI archives is either a shared original name or starts with the stem (`<stem>_`, `<stem>c`, `<stem>e`, `flame<stem>`, `concring<stem>`). The port admits those names for that pod only; any other new name refuses the package. |
| `ui` | yes | `{ "archive": "UI/<STEM>_UI.RES", "rider_model", "character": [...], "character_pages": [...], "pod_pages": [...], "shadow": [...], "portraits": [...], "background": [...], "results_icon", "garage_model": [...] }`: member names in that archive, exactly as the arcade manifests list them (`character` = psg, mcf, vut and the six motions `_ent _idl1 _idl2 _idl3 _breathe _sel`). Every named member must exist; an archive member no field names refuses the package. `garage_model` required (user decision: add-on pods can be the player in the stock tournament and get Garage upgrades; career, upgrades and records go to the DLC save keyed by package/pod). No `stats_image`: the stats bars read `[BaseAbilities]` / `[MaxAbilities]` from the PHY inside `archive`. |
| `voice` | no | `{ "ini": "<listed path>", "first": n, "count": n }` in stock `UIACCEPT.INI` format (Rider-screen confirm voice). Absent = none. |
| `announcer` | no | listed path of an INI in stock `PODnnANN.INI` format. Absent = generic announcer lines only. |
| `ai` | no | `"addon_courses"` (default: AI only on add-on courses, never an AI opponent in the stock tournament or on stock courses; user decision) or `"never"`. `"anywhere"` refused in contract 2. |
| `unlock` | no | `"always"` (default and only value in contract 2). |

## Log lines (add-on items)

```
dlc_item key=n64-vengeance/nv kind=course code=nv number=32 phy_class=B ai_class=A rival=none loading=file course_background=file course_screen=stock:ta inherit_tuning=- inherit_particles=-
dlc_item key=arcade-ark-roose/a2r-ark kind=pod stem=a2r_ark number=32 camera=1 ui_members=30 voice=0 announcer=0 ai=addon_courses
dlc_registry packages=12 refused=0 courses=5 pods=7 replacements=0
```

## Course example (Vengeance)

```json
{ "schema": "rrpc-dlc/1", "id": "n64-vengeance", "name": "Oovo IV - Vengeance (Episode I Racer, N64)",
  "author": "RacerRevengePC converters", "version": "1.0.0", "enabled": true, "kind": "course",
  "requires_port": "dlc-contract-2", "personal": true,
  "strings": { "english": { "COURSE_NV": "VENGEANCE", "PLANET_NV": "OOVO IV" } },
  "items": [ { "kind": "course", "id": "nv", "code": "nv", "archive": "TRACKS/NV.RES",
     "name_key": "COURSE_NV", "planet_name_key": "PLANET_NV", "phy_class": "B", "ai_class": "A", "rival": "none",
     "loading_screen": { "file": "UI/TEXTURES/LD_NV.PST" }, "modes": ["single", "time_trial"],
     "enhancements": ["surface_rules", "zero_g", "n64_floor_table"] } ],
  "files": [ { "path": "TRACKS/NV.RES", "bytes": 1399140, "sha256": "..." },
             { "path": "UI/TEXTURES/LD_NV.PST", "bytes": 287772, "sha256": "..." } ] }
```

## Pod example (Ark Roose)

```json
{ "schema": "rrpc-dlc/1", "id": "arcade-ark-roose", "name": "Ark 'Bumpy' Roose (Star Wars Racer Arcade)",
  "author": "RacerRevengePC converters", "version": "1.0.0", "enabled": true, "kind": "pod",
  "requires_port": "dlc-contract-2", "personal": true,
  "strings": { "english": { "RIDER_A2R_ARK": "ARK 'BUMPY' ROOSE" } },
  "items": [ { "kind": "pod", "id": "a2r-ark", "stem": "a2r_ark", "archive": "PODS/A2R_ARK/A2R_ARK.RES",
     "camera": "PODS/A2R_ARK/A2R_ARK.CAM", "name_key": "RIDER_A2R_ARK",
     "ui": { "archive": "UI/A2R_ARK_UI.RES", "rider_model": "ui\\geometry\\a2r_ark.psg",
             "character": ["ui\\anim\\a2r_arkc.psg", "ui\\anim\\a2r_arkc.mcf", "ui\\anim\\a2r_arkc.vut", "ui\\anim\\a2r_arkc_ent.mot"],
             "character_pages": [], "pod_pages": [], "shadow": [], "portraits": ["ui\\textures\\a2r_ark_flrg.pst"],
             "background": [], "results_icon": "ui\\textures\\a2r_ark_fsm.pst", "garage_model": [] },
     "ai": "addon_courses", "unlock": "always" } ],
  "files": [] }
```
(Lists shortened; the real package lists every member and file.)

## DLC save side file (phase 3, step 3)

`<save folder>\dlc-progress-v1.json`, schema `rrpc-dlc-progress/1`, shape as DESIGN.md 6.2: course records keyed by
item key (with `records_key` and the archive SHA-256), add-on-pod records on stock courses keyed by stock code,
pod unlock state keyed by item key. Created only once a package was admitted and progress changed.
