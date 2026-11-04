# Genuine World-load return to the outer Race branch

Status: isolated source-only proposal, November 4, 2026. The active shared working files and published checkpoint were not edited. No compiler, syntax check, build, test, probe, runtime action or emulation was performed.

## Exact continuation

The existing source `1A9EE0` loader receives the Race pointer, operates on its retained World, and records its genuine return in `sourcePostSkyTail_`: V0=1, restored registers, released stack, and continuation `1A6D88`. Its native orchestration boolean remains false because the larger source load is pending. This proposal consumes the explicit original return value, not that host boolean.

The same lexical caller immediately performs:

- `1A6D88`, word `14400003`: branch to `1A6D98` for the actual nonzero V0.
- `1A6D8C`, word `00000000`: execute the original NOP delay slot.
- Stop before `1A6D98`, word `8e040030`: the unresolved load of Race+30.

Only the already represented V0=1 return is admitted. No zero-return failure path or arbitrary caller-supplied result is added. A private Race-owned structure retains one invocation's original result, branch decision, delay-slot history and pending outer PC. It is native saved caller state, not a new source object field or readiness token. The World's historical return PC stays `1A6D88`; the caller's separate pending PC becomes `1A6D98`.

## Same-occurrence admission and lifetime

Admission requires the actual current Application/Race/World owners and threads, current published Application Race and matching retained constructor owner, same source generation/entity, genuine source loading-camera suffix, and an untouched native Session/track-adapter route. It rejects closed, terminal or withdrawn source World state, even though historical source-tail preflight can remain readable after native cancellation. Both actual source-tail preflights, the completed epilogue/return/directory history, and fresh one-shot caller state are checked before the branch history is committed.

No new heap allocation, owner borrow, raw alias or source entity lifetime is introduced. Native teardown leaves scalar caller history as history; it does not reissue a branch or manufacture an unwind/return. The source tail itself already prevents replay, and the new caller state independently refuses a second entry.

## Why Race30 remains unread

Original constructor `1A66AC` writes zero to Race+30. That does not prove its value at `1A6D98` after intervening calls. The existing native backing is therefore not used as source-null evidence.

The bounded audit proved several direct cases but did not prove all transitive aliases:

- Constructor suffix after the store contains no further calls. `141C20` is an identity leaf. The direct `141960` attachment path updates Race+18/+24 and child-link fields, not Race+30.
- `1AA5A0` replaces its incoming Race receiver with process material/manager globals. This removes that direct receiver alias but does not establish transitive global noninterference.
- The preceding original music call `1A6CC0 -> 1A3C00` is still unjoined by the current source route. It can retrieve Race through global Application+28C and registers callbacks through `2C2B10`. Those callbacks can recover Race and dispatch audio-group methods. Registration is not immediate dispatch, but callback reachability/effects before `1A6D88` still require proof.
- `1A9EE0` itself receives Race, not a World-only receiver. Its direct later Race write at `1AA404` is +70, while transitive World/global callbacks remain part of the writer-history obligation.

The audit found no proven nonzero Race30 writer. It also did not establish a complete proof that zero survives. Therefore this proposal neither reads `field30_`, skips `114770`, enters `1A95B0`, nor issues original manager/Player/World-row authority. Original `1AA580` manager loading is substantially later. The upstream original music/caller gap remains, and Player speech independently stays at `2C11C8`.

## Integration boundary

The two code files were captured into exact `before/` snapshots and edited only under this packet's `after/` directory. The World file snapshot includes the then-current working Sky state, which is distinct from the separately frozen published Sky checkpoint. Its precise baseline hash is recorded. Rebase and review only the focused change against the chosen durable baseline; do not replace a published file wholesale with this packet's working-copy postimage. No durable integration is claimed.
