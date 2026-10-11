# Integration notes

Owner: root integration only. This worker did not edit the checkout, commit, publish, or run verification execution.

- Original target: `6dc2c8b1b3064f8fdeb5f12648fe4583119c1b9e` (`BASE-COMMIT`).
- Concurrent root work advanced HEAD to `6db1578b4d664d61ba1b7185e0be1ba176963476` before final handoff. All four current audio source SHA256 values still match `PREIMAGE-SHA256SUMS`; the working tree was clean at that observation.
- `RIDER-AUDIO-CAPABILITY.patch` uses repository-relative `a/platform/...` and `b/platform/...` labels. Each complete replacement candidate is also available under `candidate/`.
- Match the target files against the exact preimages before integration. Do not overwrite unrelated later edits.
- No build-list change is necessary: all four files already exist in the active source set.
- Integrating this source does not admit Rider directions or typed/presentation PCM pumping. The new entry points remain uncalled until the real transaction and owner joins are implemented and reviewed.
- The new entry points are dormant. The revision metadata updates affect ordinary active mixer/transport command paths and have not been runtime-tested.
- Preserve the current centralized test/publication hold pending user approval. No retries, dependency changes, new execution route, or workaround are included or authorized by this handoff.
