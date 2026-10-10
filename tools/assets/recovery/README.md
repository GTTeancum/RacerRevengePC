# Source-only native asset preparation reconstruction

Status: authored and statically reviewed only. No compilation, script execution,
Python import/syntax checking, importer execution, tests, sanitizer, or gameplay
checks were performed. This is an untested candidate for the root-owned slot.
All files are new and isolated here; the source repository was not modified.

## Root-owned command (NOT EXECUTED)

Use a new output name whose parent exists. This single command compiles two
small cold tools, stages authenticated data, imports all eight original pods,
selects the pinned 48F original visuals, extracts every GC archive member,
builds GC visuals/textures/sky and ATM metadata, then compares four generated
indexes byte-for-byte with retained source fixtures. It never compiles or
launches the game, invokes historical scripts/binaries, or emulates guest code.
Only root should launch it when its centralized execution policy permits.

    python3 -B /workspace/shared/recovery-20261010/asset-preparation-reconstruction/prepare_recovered_assets.py \
      --repo /workspace/shared/rr-recovery-git \
      --inputs /workspace/shared/recovery-20261010/asset-inputs \
      --output /workspace/shared/recovery-20261010/native-assets-prepared-01 \
      --execute-preparation

The command requires an installed C++17 compiler (default g++, configurable
with --compiler), its system headers, zlib headers/link library, Python 3.11+
(existing selector uses hashlib.file_digest), and FFmpeg with IPUM decoding.
Nothing is installed automatically. Availability and compatibility were NOT
tested. Each subprocess has a 30-minute ceiling. Existing FFmpeg decoder calls
retain their 30-second ceiling. Preparation output and originals are private.

## Source-derived compiler recipe

The importer recipe is directly grounded in the repository's existing
`tools/assets/original_roster/build_original_roster_tools.sh`:

    g++ -std=c++17 -O2 -ffp-contract=off -I REPO/src -I REPO/third_party \
      REPO/tools/assets/original_roster/import_original_pods.cpp \
      REPO/src/assets/import_decode48c.cpp \
      REPO/src/assets/import_scene48d.cpp \
      REPO/src/assets/import_texture48e.cpp \
      REPO/src/assets/scene_asset48d.cpp \
      REPO/src/assets/native_asset48b.cpp \
      REPO/src/assets/pod_physics49n.cpp -lz -o NEW/import_original_pods

The new extractor needs only its own translation unit plus the existing
self-contained resource decoder translation unit and zlib:

    g++ -std=c++17 -O2 -ffp-contract=off -I REPO/src -I REPO/third_party \
      HERE/extract_gc_members.cpp REPO/src/assets/import_decode48c.cpp \
      -lz -o NEW/extract_gc_members

The full build script's second target, validate_roster_visuals, is intentionally
not built or invoked by this preparation wrapper. No validation/test binary is
run. Decoder round-trip checks already inside the cold importer remain intact.
Source snapshot is HEAD 791dce663909f04e1c4e45fe580e6bfbcd67cc51, including
root's restored kind0 original 4-bit UI support; no historical decoder is used.
`REPOSITORY-SOURCES.sha256` freezes source/headers, asset tools, native-json, and
retained indexes used here. Any mismatch fails rather than silently adopting
changes. Do not regenerate pins to evade a failure; review source changes first.

## Missing intake bridge now represented in source

`extract_gc_members.cpp` calls the existing `decodeResource48C` and
`extractResMember48C`. It admits the exact GC archive byte size, checks all
member names before writing, rejects absolute/traversal/empty components,
non-ASCII unexpected characters, case-fold collisions and file/directory
collisions. It retains all member types, including SCB, CUB and ATM, unchanged
under lowercase `intake/original-members/`. Its limits are 10,000 members,
8 MiB/member and 128 MiB total. Outputs use exclusive creation and mode 0600.
Its parent must be a private trusted directory, as the wrapper creates it.
The standalone extractor requires a caller to check GC SHA first; the wrapper
pins GC SHA before invocation and writes an unchanged `original/GC.RES` copy.

Python adds original byte sizes, SHA-256 values, original name spelling and
logical offsets to `intake/archive-inventory.json`, then invokes the current
source importer into `native-import/import.json` and its referenced resources.
No schema or fixture substitutions are made. Original SCB/CUB hashes are
independently enforced by the unchanged assembly/sky source tools.

## Authentic input staging and no-overwrite boundary

All five ZIP hashes are hard-pinned, including Full48F SHA
60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c and generation
3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39. Only entries
already listed in the recovered extraction inventory are staged. Each is
compared to its matching member inside its authenticated archive AND the
recovered local copy. Original GC is pinned separately to SHA
69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9.

Only historical evidence/work data are mapped; historical top-level binaries,
scripts and launchers are excluded. Executable/script extensions and executable
magic are rejected, except the exact pinned original SLUS_202.68: that file is
copied as mode-0600 data only and is never a command target. Original files and
ZIPs are never changed. Full48F is read only by the source-owned selector;
its old binaries/scripts are not extracted or run. Audio duplicates must have
identical spelling and bytes. Other path collisions fail. Existing destinations
are never resumed or overwritten. New output ownership isolates existing
source scripts that intentionally rewrite their own generated metadata.

## Output, failure behavior, and honest stopping boundary

On success, `PREPARATION-COMPLETE.json` means cold preparation only. It records
commands, input and source hashes, compared index names and explicit lack of
gameplay validation. Root may inspect generated artifacts/logs before deciding
whether and how to attach this data root to a separately built current client.
All relative runtime lookups would need this new root as the working directory.
No launch command or environment change is performed here.

A failure before output creation changes nothing. A later failure leaves the
new directory quarantined with `FAILURE.json` and subprocess logs. Do not launch
or reuse it; fix the source/root decision and select a different new output.
The wrapper never deletes originals, partial results or old outputs. Manual
inspection is required for unexpected decoder/identity/index failures, with
no fallback to TA/POD01, replacement fixtures, or relaxed expected hashes.

No original resource/provenance input is currently known to be irrecoverable:
the exact fifth Full48F archive is present. Unverified risks remain: compiler
closure/tool availability, GC member admission, unsupported referenced assets,
FFmpeg output agreement with pinned source indexes, and comprehensive runtime
dependency closure. None can be ruled out by source inspection. Matching
prepared assets do not prove all-eight-original-AI gameplay. Current GC world
joins remain explicitly incomplete (zone11/LostAI/Gate sound), and the root's
opt-in fog integration is separately untested. No root-owned hourly execution
slot was consumed by this source-only work.
