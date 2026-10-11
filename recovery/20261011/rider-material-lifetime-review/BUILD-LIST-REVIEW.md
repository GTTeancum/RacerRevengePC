# Independent source review: Rider ledger interactive build-list addendum

Reviewed 2026-10-11 against baseline `18c25134a8a7c6893822d21dc9ec8ea8a881ebdf` at `/workspace/shared/rr-recovery-git`.

## Disposition

**No blocking source defect found.** The addendum adds exactly one inactive ledger translation unit to the centralized interactive build's existing explicit source tuple. Apply it together with the original ledger source/header stage; the ledger implementation is absent at this baseline, so this is not a standalone buildable change.

No generator, import, build, test, syntax check, project parser/decoder/importer, probe or launch was executed. Review used source text, ordinary searches/diffs and SHA-256 only. No implementation, active checkout, or original ledger-stage file was edited.

## Exact reviewed SHA-256 identities

- `preimages/tools/prepare_interactive_build50dv.py`: `7fa8c0f3248cd03ab24d2dbb24f9ab16a9daef2914735043ec48e896a2d6fe20`
- `staged/tools/prepare_interactive_build50dv.py`: `32c4e2f16a6178f37799076de52bf65a751159be320bcad0560b9960dc593b38`
- `material-lifetime-ledger-build-list.patch`: `f9d80c9396e1d445e17679d406f1ede465f5c2d3c0abab1e3dc74112f1c5d6b6`

The preimage hash matches both the clean checkout file and `git show 18c25134a8a7c6893822d21dc9ec8ea8a881ebdf:tools/prepare_interactive_build50dv.py`. Regenerating only the textual unified diff with the patch's `a/` and `b/` labels produced the exact patch hash above; this did not execute or parse the generator.

## Delta and insertion semantics

1. The sole diff adds `'src/game/original_ui_rider_material_lifetime50kg.cpp',` at staged line 109, immediately after Rider backend and before Rider panel in the existing inactive original-UI source tuple. No other source, flag, output destination, executable selection, callback, readiness condition or generator logic changes.
2. The existing loop at staged lines 128-131 constructs `absolute = str(ROOT / source)` and inserts it immediately before `-lz` only if that exact absolute string is not already in `argv`. This avoids a second copy if an earlier source collection already supplied the same translation unit. The new literal occurs exactly once in this staged file and is absent from the baseline tools source lists inspected.
3. Baseline `prepare_offscreen_build50do.py::build_command` produces source arguments using the same `str(ROOT / source)` convention and explicitly appends `-lz`; it also deduplicates its own source list before constructing argv. Its imported `ROOT` is the resolved checkout root from `check_terrain_registered_drive50bv.py`. These paths are therefore compatible with the interactive loop's existing exact-string duplicate check.
4. Each missing tuple entry is inserted at the current `-lz` position, after entries inserted in prior iterations, preserving tuple order for newly added sources. Already-present arguments keep their previous positions. On this baseline, the newly added ledger argument follows the newly inserted backend argument. This change does not attempt arbitrary path-alias deduplication, nor require it for the inspected consistent path construction.
5. The ordinary archive, character, pod and binder asset translation units the inactive ledger depends on are already in the same interactive tuple. The addendum complements the original stage's native CMake registration; it changes no runtime wiring.

## Original ledger stage remains unchanged

Its three reviewed implementation files were rehashed without modification:

- Header: `5b205bc3528d7bdef1666bbf5c72b043526c78ed2e83c95a275e137c9af8bf4e`
- C++: `0d428ddf7a47e9d6e2da7ffa8f2e4d0d0ca10c0504472194cfa4e32e418ff035`
- Native CMake: `76caeaf42a67f311f6481f78466d410598cc9de8750bec610430528b4030263d`

The original ledger review's integration prerequisites remain in force. This addendum establishes source-list coverage only. Generated argv, compilation, linking and runtime behavior were not exercised and are not claimed verified.
