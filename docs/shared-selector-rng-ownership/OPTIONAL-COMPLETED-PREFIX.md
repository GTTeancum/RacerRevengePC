# Optional completed-prefix preservation

Not part of `minimal.patch`, `changedfiles/`, or the next root integration/validation build. Not compiled or tested.

The separate `optional-completed-prefix.patch` is relative to the ownership candidate, not to the original checkout. It changes the GC service's temporary `next` and `raw` values into references and removes the final assignments back to themselves. The existing store/draw order is otherwise unchanged.

This would retain seed-guard initialization, a sampled timestamp/seed, the raw RNG draw, and range-guard initialization when a later numeric rejection or animation `advance` failure occurs. Those stores precede the original downstream work. The original source cannot roll them back as the current native staging does.

This is a behavior change on failed service calls and needs separate root authorization/review before integration. In particular, a caller retry would consume another draw after the earlier committed prefix; root must review fault persistence/retry behavior and native readiness preconditions rather than assuming arbitrary retries are source continuations. `advance` itself still stages animation/motion state. This patch does not claim to recover every partially completed original world-animation side effect.

No assertion-domain or floating-point changes are included in the optional patch. GC's existing unconditional range rejection and category6's direct assertion behavior remain separate questions.
