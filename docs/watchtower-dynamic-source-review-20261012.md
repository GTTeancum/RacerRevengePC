# Watchtower dynamic/query layer: independent source review

2026-10-12. Source-only review of frozen animated-world patch `ca8f1b8e7e4e667c6b402abdb7585b57167ebec3d3ba32e4ff0367fe86273580`, composed by the root over the previously reviewed assets/audio/AI overlay.

## Result

**Accepted as a corrected source-integration candidate. Two source blockers were found in the frozen dynamic layer; both are closed in the root-composed source read back at 17:32 UTC. No further definite blocker was found.** Both findings were reported promptly to the root and author. GA must remain closed pending authorized preparation/validation. No missing external owner or intentionally closed admission gate is counted as a newly discovered defect.

No build, compiler/syntax/import check, software test, asset decoder, native execution, rendering, audio playback, or gameplay was performed. The base and staging source were not edited.

## Blocking findings

### 1. Unvalidated native-trigger pointer is consumed on non-trigger shapes

Frozen `src/game/indexed_type2_contact50cc.cpp:176-191` discovers, validates, and admits the writable group only for selected candidates whose `shapeType == 5`. Its added pair filter at `:326-327` dereferences any non-null `candidate.nativeTriggers`, without requiring that shape.

Consequently a supported shape0/shape1 candidate carrying the new pointer field passes its ordinary mesh/compound branch while its group pointer bypasses group validity, owner exclusivity, and range/alias admission. The runtime then calls `nativeTriggerPairAllowed50GA` on the unadmitted pointer. Depending on that value, this can read invalid/aliased storage or unexpectedly suppress a non-trigger collision. Existing constructors initialize this field to null; this is a definite hole in the newly broadened core API's malformed-input boundary, not a claim that the current cold constructors produce it.

Required correction: reject a native group on non-shape5 in both indexed and live-query discovery, reject orphan index metadata, and constrain the runtime dereference to its admitted shape5 owner.

Isolated correction reviewed: `delta-v1/watchtower-native-trigger-guard-v1.patch`, SHA-256 `ae5ecacb6ef56599d2323623d6b3b66d23aeedadc63f87e05d7062205d368fa2`. Its two validation changes reject before membership/numerical writes, and its runtime filter additionally requires shape5. The existing E8 dispatch already checks shape5. This closes finding 1 by source inspection. Its base and candidate hashes were verified; no application to root staging is claimed here.

### 2. Trigger action owner is prepared after the first required native queries

In the reviewed combined main:

- `gaTriggerActions` is constructed during candidate creation, before the racer bank and race activation.
- The neutral native setup ticks at `tests/native/terrain_interactive_drive50du.cpp:2665-2693` call `advanceFrame` for all eight actual racers.
- `advanceFrame` unconditionally calls `gaTriggerActions->beginQuery(...)` whenever that pointer exists, before its main/secondary queries at `:2088` and `:2107`.
- `ga_trigger_actions_fixture.h::Owner::beginQuery` rejects an unready owner.
- The actual `prepare()` and initial `observePrepared()` calls occur only at combined main `:2939-2944`, after the setup ticks, in the later dynamic-material-loading block.

After the intentional admission gate is eventually removed, a setup frame that reaches its main query therefore fails with `GA main trigger arena lease unavailable` before execution can reach the late preparation. A selected trigger could also fail the core group-readiness admission. This is a source-order dependency cycle in the prepared implementation, independent of missing generated assets or the intentionally closed launch gate.

Required correction: prepare the real shared action/group owner and observe all sixteen actual cold COL poses after final racer identity/resource/grid installation, but before the first neutral native tick. Do not bypass the query lease or suppress setup triggers. Dynamic material/GPU loading can remain in the later visual setup stage.

Isolated correction reviewed: `delta-v2/watchtower-trigger-startup-v2.patch`, SHA-256 `a7c601701a7bbeb1a611f2d6ab723a4273686aaf64f474fbd4d9431710825e3a`. It moves the unchanged identity/preparation/initial-observation block directly after every final grounded grid placement and before all neutral native ticks. The real audio owner, canonical navigation, per-racer IDs, and actual COL views already exist there. It removes the later duplicate preparation and leaves material/GPU loading at the visual boundary. This closes finding 2 by source inspection. Its frozen worker-client base SHA-256 is `cb7cce71dfec6d8add4b83d17d996f7445c1abd5199c3807067e304e7c8ee56e`; full corrected worker-client SHA-256 is `0e1b23c2625fb3f2619c80798cd3a93521e65553372127cb78a7decde7815f03`. Both were verified. These full-client identities are not substitutes for the root's assets/audio/AI combined client.

## Reviewed identities and scope

- Base remains clean `c6814e73ccfd179c26c700a4337f5005a1d8b0b2`.
- Staging: `/workspace/shared/watchtower-root-staging-20261012`.
- All 42 paths in its current `BASE.json` exist. All base preimages match.
- Every non-main dynamic file matches `watchtower-animated-world-20261012/evidence/manifest.json`. The shared main was reviewed as the root's combined file, not as the author's independent full-file snapshot.
- Reviewed combined main SHA-256: `51d0f347240a83c575b7f4d7e8e9a1d1ce861dc5297172ec8fa691392f866c5a`.
- All 21 original TOB, PSG, VUT, MOT, MCF, COL, PSM, and PST rows listed in the dynamic manifest match the retained original member sizes and SHA-256 values.
- Old 30-path `STAGED-SHA256` data was not treated as the current 42-path source authority.
- `sessionAssets()` remains TA/GC-only, and main also has an explicit GA review/validation guard. This review neither removes those gates nor authorizes preparation or execution.

## Collector and ownership review

- Apart from finding 1, both core discovery loops require exactly one of the GC Gate or GA group for shape5. Multiple distinct groups, distinct GC Gate owners, or a mixture in a query are rejected. The entire one shared group is admitted as a writable span before immutable query/owner/resource/filter spans are checked.
- Indexed range capacity remains sufficient: eleven pre-existing numerical writes plus one mutually exclusive shared owner fill the twelve-entry range table. Live-query's corresponding total remains within its 32-span table.
- Group validation bounds ten trigger states and sixteen racer colliders, requires unique nonzero identities, checks reciprocal lists in both directions, verifies ignored identities, and requires the fixed chance100/action-count subset. The group and its reciprocal state are not registered with the per-racer bank.
- Actual candidate index and collider identity are checked against the selected group entry. The current query identity must be one of the group's sixteen colliders. Existing bilateral category/identity filters remain before the additional PLAYER/disabled/reciprocal checks.
- Shapes are admitted through existing ordered registered-zone collection. Each trigger is installed in its authored zone with method1, real sphere bounds, TRIGGER8 category, and target E8 callback. The trigger E4 path is skipped by the existing spring mask rather than turned into a spring impulse.
- The unchanged `nativeType2GateOverlap50KG` still uses actual loaded COL endpoint pairs and current/previous accepted matrices. No whole-world scan, route-position proxy, or fabricated hit was added. Retail reverse108E90 leaf/descriptor parity remains explicitly unresolved.
- Action callback context remains a trusted caller-owned, stable, disjoint external action destination under the existing core callback contract. This review does not claim the numerical range guard automatically validates arbitrary memory reachable through callback context pointers.

## Racer identity and canonical LostAI writes

- GA assigns main/secondary handles `0x30000000 + 2*racer` and `+1`, producing sixteen distinct native collider identities for eight actual resource owners. These are native handles, not guest addresses or synthetic physical objects.
- Both identity scalars are added to the sealed bank's registered spans. Main and secondary query resource pointers/history already belong to the bank. Each cold racer installation sets its own identities beside its actual COL views.
- Each query receives a stack-lifetime `ActionLease` holding the current bank arena's main identity and `&navigationProgress.reset.vehicleC80`. The lease is opened immediately before and closed immediately after the synchronous main/secondary query. It does not survive a racer visit.
- LostAI preflight matches the current actual racer and bounds each source node target. The source helper compares the entering collider against actual main690 identities. A secondary contact can acquire reciprocal membership but does not write an unrelated racer's C80.
- The eight original LostAI destinations remain 80/85/93/47/63/102/35/40. Reset remains false. No C84, C88, steering, teleport, or route-driver write was introduced.
- PLAYER suppression refers specifically to slot0's actual main collider, including in the all-AI schedule; it is not derived from human/AI control role. The corresponding secondary collider does not become another main destination.

## Trigger entry, exits, and action ordering

- All ten original triggers and eleven actions are retained. Trigger1 stays disabled. Trigger10 alone has the original once-only trigger flag and ordered Animation2 then PodSound callbacks. The eight LostAI triggers retain repeating behavior.
- Source callbacks are preflighted in order before the fired flag/callback sequence is committed. Trigger10's animation activation precedes the real sound submission. If a provider unexpectedly fails afterward, the group retains the committed source-action prefix and marks terminal failure rather than inventing success or replaying the event.
- Reciprocal membership suppresses repeat entry. Exit service consumes high-water slots in order, preserves holes, removes both reciprocal sides, and permits a later LostAI re-entry. Source enter/exit helpers retain their existing behavior.
- Lifecycle geometry stores each actual immutable COL resource and last accepted prepared matrix. Initial snapshots come from each final cold grid pose in reviewed delta-v2; subsequent snapshots update after committed live-query prefixes. That correction makes the owner ready before the initial queries.
- Exit overlap uses the existing recovered122BA0 root helper solely to prune prior contacts; it is not substituted for entry geometry. Exit action callbacks do not access the transient C80 lease and do not mutate navigation.
- Enabled audio requires authenticated BIGDOOR PCM during cold preparation. Voice126 binds to actual `World4D8`, stays disjoint from GA material voices128..151, and uses the existing mixer/provider. Explicit audio-off is represented as mute. Native accepted-camera XZ listener policy remains disclosed; original290E10 listener-object parity is not claimed.

## Dynamic animation, geometry, and materials

- Defensegate uses three source nodes, four root-inclusive scene nodes, and three COL parts; opening uses two/three/two. Graph refresh, motion binding, projection palettes, bounds, radii, and compound publication consistently use each source count rather than GC's fixed two-part assumption.
- COL-to-scene mapping is authenticated and checked as 1/2/3 and 1/2. Compound part matrices combine the real animated scene node and each original mesh local matrix. Previous/current owner and node matrices are retained.
- Both source animations begin inactive and nonlooping. Binary MOT timing is checked at 16 frames/1.0 seconds and 15 frames/2.8 seconds. Original MCF bytes remain authenticated and are not used to retime those MOTs. Existing legacy conversion and native motion sampling are reused.
- The six surfaces retain source material order, 605 vertices, bounded original decoded triangle indices, original UV/color policy, and transformed original normals. Positions/normals and compound collision publication are committed from the same accepted animation state.
- The dynamic material owner resolves six source slots to five exact GA PSTs. `fience02` maps to `fience`; repeated `fience_opening` slots share the same authenticated material/image policy. Kind2 images require the separately pinned GA cold-source table and do not fall back to GC pixels.
- Five image RGBA extents total 229,376 bytes; with their RRTEX headers they fit the 256 KiB source/GPU caps. The 605-vertex/at-most1,815-index dynamic geometry fits the declared 64 KiB/16 KiB buffers. Actual decoding/upload has not been performed.
- The dynamic renderer uses the existing dynamic-world interface with immutable topology and current position/normal arrays. GA and GC dynamic publication are mutually exclusive by selected track identity. Cleanup closes renderer before dynamic texture banks/context, and destroys the BIGDOOR owner while mixer/groups are still alive.

## Shared clock, RNG, and compatibility limits

- GA motion service, static ATM advancement, and trigger exit service run at the existing accepted-world boundary. Motion and ATM receive the shared application timestamp; motion consumes the same application RNG/motion owners, with no private seed or driver clock.
- Successful service is structured around preallocated state/vertex/normal arrays. The existing motion failure-staging limitation remains explicitly documented: source RNG mutation prefixes are not retained when animation advance fails. This was not introduced as a hidden parity claim.
- The GA state/action path is gated by explicit GA identity and all-eight-original-AI/107-node requirements. GC retains its existing Gate owner and animation route; TA has no new trigger owner. Reviewed delta-v1 preserves null/default metadata used by their current constructors.
- Retail reverse geometry, retail scheduler/material cadence, full original camera/listener ownership, remaining environment effects, and gameplay outcomes remain separate limitations. They are not silently labeled complete by this patch.

## Final composed readback, 17:32 UTC

The root composed both reviewed deltas and refreshed the 42-path identity receipt. Final byte hashes and source readback establish:

- `src/game/indexed_type2_contact50cc.cpp`: `845bf2d3a6dd4d4d158681f2e6f16baf371e5ec7a4c9ac8e2b9d00cab0ed981e`.
- `src/game/vehicle_live_query50eq.cpp`: `ecf780c9738f72f4e4efdee36bccd156a171903e8a32d3510e5ff20a842459af`.
- Root combined `tests/native/terrain_interactive_drive50du.cpp`: `957cbcd46ac4d8527a1dbe4fdf7a0a12e8ea7055664d2947db2edd6999ec5dec`.

Both core files are byte-identical to the independently reviewed delta-v1 results. The root main's text difference from the reviewed delta-v2 worker client consists of the previously reviewed audio and original-AI integration hunks; the corrected startup block is present once, directly after final grounded grids, before every neutral tick. Material/GPU preparation remains later. The actual core readback contains the early non-shape5/orphan-index rejections and shape5-only runtime dereference. These three actual hashes agree with refreshed `STAGED-SHA256.json`.

The base remains clean at `c6814e73ccfd179c26c700a4337f5005a1d8b0b2`. Final readback used only text diffs/reads, byte hashes, and repository status; no project or check script was executed.

The corrected layer is source-ready within the expressly approved native geometry policy. Runtime readiness still requires separately authorized preparation and validation, the separately owned camera work, and any other prerequisites the root retains. Neither the review nor the corrections authorize removing GA's admission gates. No runnable or gameplay outcome is claimed.
