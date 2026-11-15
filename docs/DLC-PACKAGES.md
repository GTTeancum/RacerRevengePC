# dlc.json field reference (phase 1, schema `rrpc-dlc/1`)

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
