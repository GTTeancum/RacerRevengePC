# GC Gate opening: staged native reconstruction

## Result and status

This is a newly authored native Gate join, not recovery of the lost pre-reset Gate source or `d546da8` commit. The candidate is staged only. It has not been compiled, syntax-checked, tested, run, or integrated into the active repository.

The source-supported Trigger9 one-shot action now has a concrete staged path from an eligible registered native collision to animation owner 0. The opening mesh and its two active collision parts continue to consume the existing shared animated scene graph. No collision part is removed or disabled to make the track passable. No pose teleport, stuck detector, camera distance, route-node threshold, or forced open state is used.

Runtime opt-in after authorized integration: `RR_GC_NATIVE_GATE=1`. The existing `RR_GC_INCOMPLETE_WORLD=1` and GC race-mode admission remain required. Gate PodSound and the eight LostAI branch triggers remain unjoined. AI state-5 recovery is separately owned and untouched here.

## Files and integration

- `files/` contains complete changed and new native source files.
- `preimages/` retains the exact source text used to author changes.
- `patch.diff` is a relative-path unified patch, including new files.
- `file-manifest.json` records preimage and staged per-file SHA-256 values.
- `exact-source-anchors.json`, `evidence/`, and `action-evidence/` retain static original-ELF/input evidence. These original byte/disassembly artifacts are evidence only; do not add them to the source-only Git deliverable.
- `action-evidence/REPORT.md` describes the action/default/filter/registration source reconstruction in detail.

The task originally named active `17659ca`; a later read observed `413b4e24ecc268fdd95476c05be88b7fa518152f`. All existing staged preimages still matched active file bytes during the independent static review. This worker changed only this staging directory. Integration belongs to the root task and must preserve any intervening changes.

No build-system source entry is added: the three new implementation units are headers consumed from existing compiled paths. This is source inspection, not proof that every build configuration accepts the changes.

## Authentic authored content

Original `tracks/gc.toy` SHA-256:
`a08d92a07942411dcbc8f487e0a25d717677f264f486725696e9749b44be8244`

Original `tracks/gc.tob` SHA-256:
`e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454`

The existing GC owner already authenticates GC.RES and this TOB before parsing. The staged loader consumes Gate fields from that same authenticated parameter document:

- Trigger9, name Gate, authored zone 11
- Center `(11444.788086, 2042.792603, 12822.280273)`, radius `300`
- Two actions, Trigger OneTimeOnly true
- Missing StartEnabled defaults true; missing IgnoreTrigger is empty; missing RandomTriggerPercent is 100
- First action Animation, AnimationNumber 1, resolving to native animated owner 0 / sequence 0
- Second action PodSound, `sfx\race\bigdoor.vag`, position `(11425, 2058, 13227)`, minimum distance 0, maximum distance 2500

The source Animation action parser does not consume its own authored OneTimeOnly field. The enclosing trigger's one-shot flag is the operative rule.

## Reconstructed source semantics

ELF SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

Only its bytes were read as data. No guest PS2/VU instruction was executed or emulated.

- Gate shape construction `0x11C0B0` creates shape 5, with the real center/radius, identity matrix and unit scales. The native registry uses explicit non-guest collider/model identities and native TRIGGER mask 8.
- Constructor `0x11B190` followed by `0x11B570(7)` produces consumed flagsF8 low six bits `0x06`, flags31 low four bits `0x03`, empty deny-lists, accept mask all, and material 0. Gate is registered in the source First method, not the animated owners' Second method.
- Authored zone selection seeds membership through `0x2DC270 -> 0x2A0A80 -> 0x120070`; it is not a runtime zone-equality trigger. The staged Gate uses the existing live portal-membership attachment/update path.
- Sphere bounds `0x1208E4..0x1209A4` support the bounded unparented, unit-scale sphere adapter added to `collection_bounds48s`.
- Ordered query dispatcher `0x122660` runs query E4 before target E8. Existing spring E4 skips category TRIGGER, so the new trigger generates no spring/pair contact record.
- Trigger callback `0x2A00A0` checks enabled, sets contact, rejects already-fired one-time triggers, bypasses RNG completely at probability 100, and commits fired before callbacks.
- Action dispatcher `0x2A0E20` preserves registration order. Gate's animation callback `0x101170` sets active90[0] and resets A0/A8 to -1, matching the existing native activation stores. PodSound follows it.
- Missing native sound is recorded once as unavailable. It is not reported as played, retried, or used to block the authenticated animation action. Actual source PodSound requires the separate `0x1B5B00 -> 0x104140` resource/distance/audio-owner path.

The staged one-shot owner intentionally does not reconstruct generic recurring trigger enter/exit partner-list upkeep. A pristine Gate's first hit has no prior partner; its committed once flag prevents all later action entries. Contact-history bits beyond that bounded action path are observational only.

## Explicit native geometry policy

The original type2-to-shape5 case at `0x121884` calls `0x108E90`, passing type2 node/leaf pointers. That callee's leaf path indexes 20-byte triangle records and reads triangle vertex/normal data. The retained type2 resource reader at `0x126640` and native `Type2Resource49X` instead produce 32-byte endpoint-pair leaves. Reinterpreting native endpoint bytes as triangles would invent invalid memory semantics. The reverse shape5-to-type2 dispatch returns false, and type3 has no shape5 hit route.

The root explicitly admitted a documented native compatibility policy for this gap. `native_gate_geometry50kg.h` therefore:

1. Uses every actual loaded type2 endpoint pair in source order.
2. Applies the current canonical query matrix, with separately rounded float transforms.
3. Tests the actual segment against the authored sphere using finite double-precision geometric distance.
4. In live queries, also tests the previous accepted endpoints and the two triangles joining previous/current endpoints. This is continuous over those linear swept surfaces, including endpoints and tangency, with no arbitrary temporal samples, epsilon, added thickness, or changed radius.
5. Reduces degenerate triangles to their actual edges.

This is not source-identical triangle detection or exact continuously rotating rigid-body geometry. Splitting a nonplanar swept quadrilateral into two triangles is an explicit native approximation.

The policy remains inside the actual registered, bilateral-filtered candidate sequence. Gate is in the First method, whose collection uses the prior cached-sphere overlap. A sweep that is never admitted to that list can still be missed. This is not universal anti-tunneling coverage; the collector is deliberately not widened or bypassed.

Terrain and type3 retain the original shape5 no-hit behavior. Only opt-in indexed racer type2 queries use the native Gate policy.

## Ownership, ordering, and collision synchronization

- Gate candidate is appended after the two animated owners. All cold vector-relocation borrows are repaired; count-derived storage includes the new owner.
- The shared action binding and animation sequence live for the whole track instance, outside per-racer bank copies. All eight racers and both physical bodies can observe the same fired state.
- The previous query matrix comes from the existing accepted query cache. Each racer/body already banks that cache and cold-prepares it from its own grid pose.
- Both indexed and live-wrapper admission guards treat the sole Gate binding as writable and reject aliasing with numerical outputs or immutable inputs. Repeated visits can share one binding; multiple distinct Gate bindings are outside this bounded scope.
- Callback context is a stable GC owner. The callback mutates only sequence activation fields. It does not update collision geometry while an indexed query is reading it.
- Gate state commits synchronously at the candidate's E8 position. Numerical outputs remain transactional, but a later query error cannot undo or replay an already committed Gate action. The header makes this distinction explicit.
- Existing shared world service runs once after the racer group. `Owner::advance` samples the original motion into one scene graph, then projects visuals and recomputes both collision parts from those same nodes before publication. Both collision parts remain active. Their new transforms/bounds are synchronized into the next query candidate refresh; the renderer consumes the same accepted animated state.
- If a racer hits an unrelated unsupported boundary before shared service, the pending activation does not authorize bypassing that boundary. The session can still stop before the gate visibly advances.

## Review and next authorized validation

An independent read-only reviewer inspected candidate storage, collector registration, per-racer matrix cadence, shared action ownership, terrain/type3 paths and alias admission. Its one concrete writable-owner alias issue was corrected and reread. It found no further concrete static integration defect. This is not execution validation.

No tests, builds, syntax checks, importers, RES decoding, gameplay, numerical experiments or guest execution were performed for this candidate. Reading/hashing retained files and producing static byte-to-text evidence are the only executed inspection operations.

At the root's next authorized central slot, validation should cover:

- Opt-in absent: opening remains authored-inactive; TA and existing query fixtures retain behavior
- Opt-in present: cold authentic Gate load, First-method/portal registration, total 554 world candidates plus the existing three query references
- Real current-segment and swept contact; misses, tangency and degenerate segments; bilateral rejection; no arbitrary center-only activation
- First contact dispatches animation once; later contacts from either body/any racer never restart it or consume RNG
- Query trigger hit creates zero spring contact records and retains E4-before-E8 order
- Visual opening and both active collision parts progress together using original motion; no geometry removal
- Pause produces no animation service; resume retains existing cadence
- `GC GATE ACTION` appears once, `gate_dispatches=1`, and `gate_sound_unavailable=1` stays explicit
- Continue the original eight-AI recording far enough to distinguish Gate passage from later state-5/LostAI/native boundaries; do not infer a complete-lap pass from activation alone

The root owns all such execution and the prescribed central-slot timing.
