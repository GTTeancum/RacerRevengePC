# 48L: point contacts and remaining spring responses

2026-10-06. This extends the verified 48K Linux spring checkpoint. The frozen
48K Library snapshot remains separately recoverable.

## Recovered behavior

`updateSpringContacts48L` implements the numerical response coordinator over two
caller-prepared lists. Enabled point contacts run before generated contacts.

- Point penetration adds original record `+0x48` to the geometric dot product.
  An explicit record scene is required. Point contacts ignore the record's other
  body identity; their friction requires both the global friction toggle and
  record flags bit 0. Contact bit 1 is updated without disturbing other bits.
- Generated other-body contacts with `otherSpring58` take spring force followed
  by optional friction. This path skips impact and secondary response and applies
  spring force even when `springForce4C` is off.
- The ordinary path preserves impact → secondary68 → spring-force block →
  friction. With secondaryAtLimit60, penetration at or above the original limit
  and impact off insert another secondary call before spring force. This can
  deliberately call secondary response twice for one contact.
- Contact velocity is prepared once for each positive record. Every response
  consumes that same cache; updated body state reaches the next preparation.
- Original `297190` selects its two coefficient pairs according to impact/other
  mode, clamps penetration and cached normal speed in original order, applies
  the smaller mass times the larger body's stored inverse mass, and accumulates
  equal/opposite forces and original primary-center torque semantics.
- Original `2983C0` removes inward cached normal velocity from the primary body's
  velocity only. It does not update impact accumulators, angular state or the
  other body. Its 45 instruction words were checked against the original ELF
  and every generated-source annotation.

The previous `updateGeneratedSpringContacts48K` entry point retains its bounded
48K contract. Expanded tuning and point-list inputs are explicit in the new API.
There are no guessed response forces or patched original instructions.

## Console-oriented ownership

Execution is serial, with no runtime allocations, JSON, filesystem, PS2 state,
threads or platform dependencies. Caller-owned buffers are preallocated/reused.
Flags are output in point-list then generated-list order. Disabled point records
retain their flags without consuming scene/geometry values. Used ranges only are
written; no per-frame resizing or clearing of unused capacity is required.

All inputs, capacities, combined-count arithmetic and overlaps are validated
before scratch writes. Failures do not commit body/flag/metadata changes. Exact
input/output body alias remains supported; scratch is disjoint. Point scenes,
query production and scene refresh are still caller-owned. Original process-local
sentinel initialization is outside this typed numerical interface; its purpose
is not relabeled as a recovered gameplay feature.

This retains the future original-Xbox single-core/low-memory baseline, without
claiming whole-game residency, a fixed whole-game memory budget or Xbox FPS.
Python/JSON fixture generation is development tooling and is not shipped runtime
memory. Only real target execution can validate final platform constraints.

## Independent validation

The five focused tests pass in separate normal and AddressSanitizer/
UndefinedBehaviorSanitizer builds:

- Existing friction: 16,392 checks
- Existing impact: 53,254 checks
- Existing 48K coordinator: 21,318 checks / 196 cases
- New force/secondary helpers: 39,001 checks / 2,048 force and 512 secondary cases
- Expanded coordinator: 50,463 checks / 452 cases

Total: 180,428 checks per build, zero failures, zero measured coordinator
allocations. Build input hashes are rechecked at completion. Windows and the
optional CMake path remain unexecuted. LeakSanitizer is explicitly excluded
because this executor blocks it under ptrace; ASan/UBSan are independently run.

The expanded fixture preserves the previous 196 cases unchanged and adds 256
initialized-original-instruction cases. All 31 conditional coordinator branches
are exercised both ways. Coverage includes both lists, all material indices,
margin/limit boundaries, point friction masks, cached velocity, alternate routing
and double secondary calls. Synthetic original reads use explicitly initialized
bytes only. The saved four-case replay keeps its documented sparse-projection
limits from 48K.

Four intentionally incorrect temporary copies test sensitivity:

- Dropping point margin: 371 failed checks
- Ignoring the point friction mask: 116
- Deduplicating secondary response: 2
- Incorrectly gating alternate spring force by `springForce4C`: 164

All four leave the previous 48K cases passing and are caught by the new cases.
Production source is unchanged by these experiments. Copies/logs are clearly
labeled under `evidence/nativeSpring48L-linux-20261006/mutation-sensitivity/`.

Coordinator fixture SHA-256:
`48bc219338f58c8bb3ddd7d390bb82f6555232e57db9a67f16dc60972fa50667`.
The original ELF and secondary generated function were transferred from the PC
read-only, verified locally and used only for bounded reference extraction.

## Measured component cost

For the four-contact test workload (two point contacts and two generated contacts,
with spring and secondary paths), Linux x64 output-plus-scratch storage is 1,624
bytes and result metadata is 72 bytes. A body record is 404 bytes, generated record
116 bytes and point record 120 bytes. Input storage and parameters are additional.

GCC14.2 release frame estimates report 1,648 bytes for the full coordinator,
192 bytes for spring force and 8 bytes for secondary; nested callees and libc
frames are additional, so these are not a peak-stack measurement. Production
undefined symbols contain only ordinary math/memory helpers and intra-spring
calls, with no allocator/thread/platform symbols.

The serial Linux four-contact microbenchmark measured a 1,634.18 ns median call
and 1,663.65 ns maximum batch mean. This workload does not include the scene,
query producer, full vehicle update, renderer or gameplay. It is neither a game
FPS result nor an original-Xbox forecast.

## Reproduce

The existing runner now includes all five tests:

```sh
python3 tools/test_native_spring48k.py --output-dir ../spring48l-check --stack-usage
python3 tools/test_native_spring48k.py --output-dir ../spring48l-asan --sanitizers --no-leak-check
```

Use a new/empty output directory outside the source tree. The runner downloads
nothing and builds/runs serially. Missing dependencies are BLOCKED, not skipped.
Optional CMake still uses `-DRR_NATIVE_SPRING_ONLY=ON`. The native Windows entry
is now `tools/Build-NativePc48L.ps1`; this is preserved support, not a verified
Windows result for this cloud-only milestone.

Fixture regeneration uses `tools/gameplay/spring_force_fixtures48l.py` and
`spring_response_fixtures48l.py`. They execute frozen original instructions,
never the native implementation. Original words and fixture hashes are pinned.
The secondary proof can be reused without the full ELF after verified extraction.

## Remaining game boundary

This completes the covered numerical response branches, not live collision
production. Lists and current world scenes are still inputs. Next work is to
recover and validate their producer/bindings, connect original vehicle-substep
ordering and then expand controllable gameplay. Full races, camera, AI, audio,
progression, rendering fidelity and representative frame-rate measurements remain
unfinished. The larger legacy tree and historical archives remain on the PC.
