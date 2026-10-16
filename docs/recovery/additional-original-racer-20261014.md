# POD07 / Gasgano cold admission candidate

Base: `79fc223eda2da3d98929c9f3b1253745d17e7c2a`. Source-only, unexecuted.
POD07 is already a retained original opponent/legacy selection; it is a new
**Course/Rider admission candidate**, not a newly discovered original archive.

## Evidence and boundaries

- `src/game/original_ui_course_race_domain50kg.h:10` admits only
  2/4/10/11/12/15/19/20. POD07 is outside that domain at this base.
- `src/game/original_racer_selection50ij.h:22` binds racer60 7 to Gasgano,
  POD07 and `pod07/pod.json`. Do not interpret 7 as a bank or roster slot.
- `tests/native/original_player_identity50ik_fixture.h:52` supplies the exact
  354874-byte RES, PHY, own speech-category and Dax identity pins. The new
  preparer copies these pins; it has not independently read original assets.
- `tools/assets/original_roster/select_original_roster.py:20` includes POD07
  in the fixed eight. Its line 204 name association authenticates UIACCEPT and
  joins the original UI comment to the pod speech prefix. The new candidate
  preserves that method with POD07's pinned TAUNTS and `gas` prefix.
- `docs/native-roster-rigs50hw.md:50` requires PHY-selected PSG/MCF models and
  MCF NAME/VUE motion identities, specifically noting POD07's distinct order.
  No geometry, motion ordering, handling values or material names are invented.
- `src/game/original_ui_rider_voice_choice50kg.cpp:13` lacks Rider07; line 32
  rejects an absent range. UIACCEPT names alone do not establish constructor
  first/count fields. Do not infer a confirm event range arithmetically.
- `tools/assets/recover_unlocked_rider_ipum50kg.py:18` does not supply Rider07
  texture pins. This work makes no Rider07 portrait/stat/model readiness claim.

## New preparation source

`tools/assets/prepare_additional_original_racer_20261014.py` takes explicit
`--resource`, `--uiaccept`, `--announcer`, `--importer`,
`--importer-sha256`, `--output` and `--execute-cold-preparation` arguments.
The last flag is intended only for a later parent-authorized preparation run.
It was not invoked during this task. Input names may have any directory layout;
the resource basename must be POD07.RES. Output must be new and its parent
must already exist. A failed run can leave partial candidate files; discard or
quarantine them under the parent's policy and choose a different new output.
Only a completed `candidate.json` marks completion of this preparation phase.

The wrapper authenticates the RES, UIACCEPT and named announcer before import,
requires the parent's reviewed native importer hash, then calls only the
existing `prepare_pod` function (`prepare_original_roster.py:39`). It never
calls that module's disc-enumerating main. The importer preserves original
member bytes and native geometry/material/texture conversions; the existing
helper checks native PHY acceptance and local component/material bindings.
Original PHY bytes remain the handling source. Unresolved texture bindings
are retained, not replaced or treated as a render pass.

Output is `pod07/pod.json`, `pod07/import.json`, the importer's retained source
and native objects, three original provenance INIs, and `candidate.json`.
The receipt deliberately uses a new candidate schema, not a roster schema;
it cannot represent a one-racer replacement field. It records UI comment byte
offset, source hashes, importer/helper hashes, unresolved bindings and pending
owners. All three admission/readiness flags remain false even after success.
The externally supplied importer hash records parent trust, not independent
proof of converter correctness. Existing helper/importer source remains a
trusted dependency; parent must review it against its advanced integration base.

## Parent integration sequence (outside this worker's ownership)

1. Review these two new files against the parent's current tree. No C++ header
   is needed: adding a second runtime identity/admission table would duplicate
   the already pinned POD07 row and obscure the still-missing UI evidence.
2. In a separately authorized preparation phase, provide the exact POD07.RES,
   original UIACCEPT.INI and POD07ANN.INI plus a reviewed native host importer
   and its SHA-256. Run the new wrapper into a fresh staging directory. No ELF,
   guest executor, synthesized driver or input sequence is an input.
3. Compare that output to authenticated Full48F POD07 scene/mesh/material and
   texture identities using the existing visual-selection path. Authenticate
   original PSG/MCF/MOT/VUT/SLT/COL and PHY ownership with the existing native
   rig/collision loaders. Resolve every active material/texture dependency
   through actual original provenance. This wrapper alone does not prove those
   runtime consumers or GPU publication.
4. Keep the fixed eight original roster rows. Install the reviewed candidate
   at the existing POD07 canonical path through the parent's staging process;
   do not append POD07 to `additional_racers` or replace POD01 metadata with it.
   `tests/native/original_course_roster50kg_fixture.h:35` already searches the
   fixed rows, and lines 89 onward leave existing-field permutation to the
   caller. Move the complete POD07 rig/handling/COL/audio identity to followed
   slot0 through that existing path, retaining eight distinct originals.
5. Recover/authenticate Rider07's original portrait, stat, character model,
   material and texture inputs, and original constructor voice first/count
   evidence. Extend the appropriate Rider source catalog and voice owner only
   after that evidence exists. Preserve source UI unlock/profile behavior;
   neither this candidate nor Course capability grants a profile unlock.
6. Only after those owners are ready, extend
   `src/game/original_ui_course_race_domain50kg.h` for 7 and reconcile all
   downstream Course/Rider admission consumers. Do not bypass an unsupported
   Rider frame or relabel Rider19 assets. Keep all controls, including followed
   slot0, on original in-game AI. Parent owns all source review, checks, builds,
   rendering/audio/handling validation and eventual integration decisions.

## Unavailable provenance and no-checks status

Original input presence, current converted object hashes, Rider07 UI assets,
constructor voice-range evidence, current PCM availability and successful
runtime admission are not established here. Original directories were not
enumerated or read. No preparation, extraction, import, executable probing,
project script, build, compilation, syntax check, test, sanitizer, gameplay,
network request, guest execution or `git diff --check` was run. No shared
admission list, existing preparer, fixture or cache was modified.
