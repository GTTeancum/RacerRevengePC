# UNVERIFIED reconstructed 50HT contact helper / 50HV integration

This document and the accompanying source files were reconstructed from this worker's conversation after the workspace reverted. They have not been rebuilt or tested against a restored repository. Historical pre-rollback test results do not certify these bytes. The full integrated main file is not present here; the hash-gated transformation can recover it only from the exact 50HU main baseline.

## Implemented interface

- `native_racer_contact50HT::Owner::prepare(shape.view, error)`: cold-owned actual Pod01 query/target resources; no pointer into a bank slot is retained.
- `Owner::run(input, count, orderedPairs, pairCount, settings, response, output, receipt)`: separate input/output POD body arrays, up to8 racers, up to56 directed pairs, all-body output committed only after complete success.
- `applyAtWorldBoundary(bank, owner, s, x.bindings, x.parameters, identities, materials, orderedPairs, pairCount, settings, response, receipt)`: sequential gather, native pair pass, all-participant preflight, seven-field commit through independent bank visits.
- `emptyPhysicalRootMaterial = 0`: explicit deterministic HOST policy for an authenticated fresh empty physical-root name; not a claim about unknown source prefix bytes.

## Geometry and source mechanics

The helper cold-extracts `Pods\Pod01\podboxm.col` and `Pods\Pod01\podboxf.col` from `work/native/project/disc/PODS/POD01/POD01.RES` through the existing native decoder. The type-2 query is re-decoded and compared with the caller's existing main query, preserving the caller's explicit file-absent W words. Expected actual topology is24 rays/23 query nodes and16 vertices/27 target nodes/28 target faces.

It calls the existing source query-sphere preparation, pod-shape transform, type-2 traversal, record append and two-body spring response. No invented box/sphere bumper, teleport separation or renderer-only movement is introduced. Each supplied pair runs a separate one-target query; ray/tree/pair order within that query is retained. It does not recreate the original combined terrain/dynamic cumulative candidate list.

The dynamic callback gate is asymmetric: test whether the primary has already been processed by the other; if not, append the primary to the OTHER body's processed list and notify both contact lists. A broadphase-only miss never suppresses a reverse query. The gate is scoped to one explicit host world pass. The original body list has10 slots, enough for7 peers.

Main spring logical capacity40 is retained. The source constructor supplies40 at2122A0 to295C20;295C2C stores spring+88. Secondary capacity60 is a different model. `otherSpring58` is initialized false at295ABC and not changed in the inspected main overrides. Integration passes the existing main settings and response unchanged, including authored restitution and friction values.

## Safe bank integration

Cold-create the Owner outside registered bank spans. Call it once after all participating native frames finish, outside any Bank::visit, before race observations and the world audio mix. Participating roots must be Ready and match the current main190 cached scene. Failed/stopped/finished racers are excluded by the explicit pair list before phase admission; they are not committed.

Input projection is:

- `physical.body.state = s.bodies.main.components.root`
- `physical.body.parameters = x.parameters.bodies.main.body`
- `physical.body.localCenter = x.parameters.bodies.main.centerOffset`

The only committed fields are velocity, angular velocity, loads, positionCorrection1a0, rotationCorrection1b0, accumulator1c0 and accumulator1d0. Main scene/position/orientation/integration histories, all secondary/chariot fields, graph state, terrain state and existing terrain contact ordering remain unchanged. The next normal native integration consumes accumulated corrections. No secondary body is translated or assigned a matching velocity.

The world-frame placement is HOST native pair scheduling. It is not reconstructed original all-racer substep scheduling; velocity caches refresh at the next native integration. Collision damage, scoring attribution and secondary contact shapes remain outside this bounded join.

## Material provenance and explicit host policy

Dynamic material comes from the other body scene's name, not the collision resource filename. The inspected constructor path creates main190 at211784/21178C;289648 sets only name byte scene+38 to NUL. Body vtable+74 at211FC8 resolves to1652E0, binding that main190 node to body+288.

For an empty name,294D04 reads scene+35, three bytes before the name; further suffix reads depend on that first byte. Those prefix-padding bytes were not initialized by the inspected scene/base constructors. The ordinary allocation wrapper2E8D90 calls305700 without clearing the payload. Therefore an empty source root does not by itself establish a byte-equivalent original material0 result.

The authorized native policy chooses0 deterministically because the authenticated empty root has no authored frX suffix. This is a safe reversible host default, explicitly not recovery of unknown padding, heap state, or original dynamic-friction byte equivalence. No further guest-memory/padding recovery is attempted. A named physical root with an authored frX suffix must provide its appropriate material binding.

## 50HV main transformation

`solo-race50hv/reconstruct_main50hv.py INPUT50HU OUTPUT50HV` reads a verified baseline, constructs the narrow contact join, and writes a separate output plus unified patch only if both hashes match:

- Input50HU:2086beee4f95cc2ff5c6927040f9b609964637132d5a40d30ae6a46da254bb21
- Output50HV:d80718849ba6b98cd251c977508aa32336911f571f809f0c4f603048557a08a5

The intended join has contacts on by default for race mode; `RR_DISABLE_RACER_CONTACTS=1` opts out diagnostically. It uses stable body IDs1..8, host material0, active/nonfinished participants, deterministic primary/other index order and deliberate reciprocal queries governed by the source gate. It runs after opponent frames and before sampleRaceField/commitFrame/audio mixing. It checks zero allocations on successful passes and reports cumulative tested/geometry/suppressed/responded pairs, records, failures and last status. Failed contact passes reject without committing body outputs and explicitly report their status.

The 50HU central build was missing one link source needed by this helper: `src/game/body_contact_list48r.cpp`. Add that existing source. The other numerical/decode dependencies were already included. Runtime asset dependencies are the existing POD01.RES members above and the already-bound main PHY settings; no new external resource file is required. This should be reconfirmed against the restored build recipe.

## Validation status

The pre-rollback files passed a161-check focused native normal run and ASan/UBSan run, with LeakSanitizer disabled, including one-pair, four-body, direct eight-body, reciprocal suppression, legitimate misses, disabled-response negative control, conserved momentum, no scene/secondary changes, bank transaction and excluded failed-racer checks. Their eight-body run reported56 directed pairs,10 responding pairs,8 suppressed pairs and86 generated records. Those logs and binaries were lost.

These are historical facts only. The reconstructed C++ has NOT been compiled or run. The integrated main had NOT completed syntax or runtime testing even before rollback. Rebuild the recovered helper and perform fresh normal/sanitizer and real integration checks. The reconstructed build script and check program are supplied for that purpose.

## Fresh post-recovery validation (2026-10-09)

The reconstructed helper, bank adapter and focused check were installed in the restored compatible repository and freshly rebuilt/run. The native check passed161 checks, including the eight-racer56-pair scenario, ten responses, eight reciprocal suppressions,86 records, and zero hot allocations. Main syntax also passed. Evidence is `solo-racercontact50ht/recovery-normal.log`, `solo-race50hv/recovery-syntax.log` and `solo-race50hv/recovery-check-exits.txt`.

The integrated main was reproduced exactly by the hash-gated transformation and installed with known SHA d80718849ba6b98cd251c977508aa32336911f571f809f0c4f603048557a08a5. Its exact2086beee… preimage is preserved. This establishes main-file byte identity; it does not establish gameplay runtime success. No full game link or game run was performed in this recovery task.

The other reconstructed files have no retained pre-rollback hashes available. Their old-byte identity is unknown, and explicit reconstruction comments were added. Fresh focused tests support the current bytes only. ASan/UBSan has not been rerun after recovery; the earlier sanitizer result remains historical and must not certify current files. Source disassembly/range digests have not been re-authenticated here.
