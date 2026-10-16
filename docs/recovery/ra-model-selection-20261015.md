# Retained racer model/scene selection — source-only

Base: integrated binder/concring packet `libfile_f53676f6adc08191bf944546bb841f20`. Exact helper preimages, changed files, patch and SHA256 manifest are included. Central line-ending normalization must be accounted for when reconciling. Root alone integrates and tests.

Untested: no builds, tests, syntax checks, sanitizers, converters, gameplay, emulation, guest commands or simulated input. No central checkout/disc edits, collision changes, runtime activation or replacement drivers. All racers remain original AI.

Changes

- Recovered original model Entity vtable3D1F48 slot40=2418F0, a pointer-adjustment thunk to26EFC0. It is NOT a base child traversal. The actual model callback clears selected191.bit0 and returns when flags03.bit2 is set; otherwise virtualBC240C00 culls, writes selected191, calls26F070 only when selected, then increments Application278. Plain scene vtable3D3198 slot40=141C30 traverses its Entity children.
- Extracted the existing private static26EFC0/240C00/26F070 implementation into one private Owner method shared with the retained racer model dispatch. Static method registration, iteration, selected append and barriers remain in place. This avoids duplicating the cull/distance/LOD/material-demand policy.
- The new racer dispatch resolves the actual model owner from Session's retained role arrays inside the existing canonical Bank visit. It verifies finishedFor(racer), Bank, slot, arena, Application and canonical material table identities. Camera matrices/viewport are the genuine enclosing Session publication, under its existing visibility lease. No additional Bank visit, DrawLease, caller matrix or public ready flag.
- Preserved source240CA4: genuine null model observer19C uses the actual current camera projection even when3833D8 is enabled. Nonnull observers still require a real membership owner; no racer membership or static observer is fabricated. Actual scene entities take141C30. Unknown/effect kinds and draw48 remain blocked.
- Warm/childless binder-name lookup now accepts an actually retained rowsLoaded table, including the already implemented26EAC4 row0=model owner. It no longer requires the unrelated fresh-loader-only predicate. Actual constructed/released identities,188-before184 graph order and bounded traversal remain enforced; no synthetic rows or new warm construction route.

Precise next blockers

1. Retained racer models currently have no published aggregate-bounds160/170/198 writer. The shared null-observer path explicitly stops at240EF4 before box projection when radius198 is absent, after the real preceding world-refresh/flags prefix. This prevents constructor bounds from being reported as completed culling. Recover and join the real aggregate-bounds publication on these same model owners before expecting ordinary model selection to complete. The disabled flags03.bit2 branch does not need those bounds. The demand continuation retains its existing missing-radius/geometry guards.
2. No late cold runtime call site was joined. Session's current retained model sequence ends at the chariot7F8 append211998/21199C. The NEXT source call is213380 at2119A8, followed by actual scene lookups/transforms,190/194 parenting,208/20C concrete owners and further effects. Immediately before the late calls,212DDC dispatches racer virtualA0 in normal mode and212DE8 runs2146C0's PHY/body work. These prerequisites are not represented by a typed completed owner sequence. Calling late strips immediately after constructRacerModelPrefixes would skip them.
3. The eventual genuine order is212DF0->213A40,212DF8->213E90,212E00->214110 (actual no-op),212E08->214120, then212E10->212ED0. Existing beginLateStripPrefix and beginSelectionAllocationPrefix remain explicit, inactive adapters. No caller success flags were introduced to bridge the missing prefix.
4. Dynamic binder/ring attribute writers, unsupported effect callbacks, full renderer work and the root/fallback selection tail remain unjoined. Collision permission remains pending; RA stays inactive.

Manual source-review checklist (not tests)

- Reconcile exact preimages with central line endings; review the shared-method extraction for unchanged static selection order.
- Confirm3D1F88=2418F0 ->26EFC0 and3D31D8=141C30 in raw evidence.
- Review disabled-model early return, selected191 stores, conditional26F070 and counter278 order.
- Review actual Bank/model/Session identity checks and null19C projection branch.
- Keep the240EF4 aggregate-bounds stop until the actual producer is retained; do not issue completion from constructor bounds.
- Keep2119A8/213380 and212DE8/2146C0 prerequisites explicit before joining late construction. Do not bypass unknown effects.
- Warm lookup must consume actual existing rows, never rerun a fresh PSG loader for a cache hit.

Evidence: original read-only SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. ranges.json hashes each extracted interval. Vtable entries are data addresses; annotations are inspection aids, not execution. The extraction helper depends on the prior private word-label helper and is not a test harness.
