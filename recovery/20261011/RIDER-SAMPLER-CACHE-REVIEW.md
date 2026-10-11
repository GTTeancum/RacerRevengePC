# Independent source review: sampler cache

Reviewer: parent investigation agent, independent of candidate author
`cache_submission_sampler_queries`. Reviewed the complete 43-line sampler-block
replacement and surrounding admission, publication, texture-owner and draw code.

Verdict: acceptable for root integration on the captured preimage. No runtime
claim. No build, test, syntax check, project parser/import/decoder, or program
launch was performed.

## Exact reviewed identity

- Preimage: `fdcb8fe450362d6f39d64ff48aed50cebf237f852abc87c4e8b331f8976df0b1`
- Candidate: `9947bea8f3719bdc174df6e065a731bfbfef1bc878c6fadccf2ba606df5a4516`
- Patch: `c9965c4d8d4b2baa756b99d6b7c32591c02e8722fef80e756c4646a27b7b2ea7`

## Checked properties

The cache keys the actual nonzero GL texture object name returned by the
validated bank. It is automatic storage scoped to this one submission, after
current-context/thread/publication checks. No owner close/recreate is permitted
during the borrowed operation; no texture-parameter mutations occur in this
preflight. Texture-name reuse across contexts or submissions cannot reuse a
record. Every later span independently recomputes and compares all six expected
parameters, so conflicting expectations still fail rather than becoming a hit.

The admission bounds `spanCount` to `originalUiMaximumSpans50KG == 2048`. Before
iteration i, `observedCount <= i < spanCount`; the first-hit search and prospective
miss reference therefore stay in bounds. Only a fully queried, fully accepted
record increments the count. On a partial query failure the loop terminates,
so no uninitialized value can be reused. The fixed storage is 56 KiB, without
heap allocation; worst-case lookup is roughly 2.1 million simple comparisons.
This is a bounded tradeoff, not measured proof of faster submission.

Each first observation preserves bind, parameter query, immediate GL check,
expected-value comparison and early rejection. Cache hits issue no GL calls,
so they introduce no skipped-call error to conceal. Both zero-map bypass and
original invalid index/name/asset/sampler admission are untouched. All errors
still occur before framebuffer clearing or source work starts. Every draw
binds its own required texture, so the final preflight binding is unobservable
in framebuffer output. Draw/span ordering, shaders, all primitive streams,
source work poisoning, final status checks and publication remain unchanged.
