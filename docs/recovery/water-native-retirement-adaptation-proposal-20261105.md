# Separate proposal: bounded native section retirement

Status: design only. Not implemented or enabled by this packet. Publication remains paused. The source-exact path still stops before original chunk-header read30492C. This proposal does not establish EE allocator execution or equivalence.

## Why it could be useful

The existing Config parser already uses explicitly success-only native backing for original allocations, with semantic pointer roles rather than invented EE addresses. A separately identified paired native-retirement mode could allow further native text-parser development while preserving these limitations. It must never be introduced as a silent substitute for305728/304900 success or make current source-exact tests/receipts claim original heap behavior.

## Required custody and state design

- Keep source-exact and bounded native-retirement claims distinct. An explicit mode/capability must name the adaptation; do not call it originalFreeReturned, allocatorRecovered or readiness.
- Admit only the exact table/data allocation roles created by the same Config occurrence and the success-only backing path. Prove all current borrowers are withdrawn and that semantic child[0] is the original data role. No arbitrary pointer or foreign allocator may enter.
- Preserve original lookup-before-seek-before-teardown ordering and current same-process reentrancy at each distinct free-call boundary. Missing Wake Froth still returns without retirement. NULL child skips both frees and the source count/child clears; zero count alone does not skip retirement.
- Retire the data role first and the table role second, recording native custody separately from EE allocator effects. A later failure must retain exact earlier native effects. The source child24 field remains its original nonnull role until157FC4; do not prematurely clear it just to make native preflight easy.
- Keep semantic identities/epochs distinct from reusable native addresses. A freed native address subsequently reused for another allocation must not resurrect an old borrow or allocation role. Never inspect native malloc headers as EE chunk metadata.
- Preserve historical first-selection counts, name, query, rows, reads, allocations and all getter results independently of the current selected-section owner. Historical getter row bounds must use their originating section, not the new section's count.
- New Wake Froth selection needs its own seeks, input/split scratch knownness, read cursor, count/store passes, allocation roles and source name-tail effects. Continue using the same file/stream owner; no reopened or cached alternate source.
- Keep original low32 shifted table/data requests, zero-request behavior, unconditional child[0] store and partial states explicit. Native safe-size/storage refusal remains an admission boundary, never a fabricated guest mallocNULL/errno path or rollback.
- Preserve original lifetime on abort and native double-free prevention without asserting source destructor/free completion. Shared process globals and section-independent text table survive exactly according to existing custody, not a reset on retry.

## Remaining limitations

Even a well-scoped implementation would not recover original chunk sizes, bins, arena state, coalescing, allocation addresses, failure outcomes, errno or arbitrary aliases. It would establish native paired ownership only. Genuine EE allocator execution requires separately joined original heap state before30492C and every subsequent metadata access.

Before any implementation is called complete, independently review both successful native retirement and every partial/refusal/NULL/zero-count branch. Build/runtime validation remains prohibited in the present task; a future authorized validation stage would need its own explicit results. No adaptation is enabled by this document.
