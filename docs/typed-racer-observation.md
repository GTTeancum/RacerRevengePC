# Fresh typed racer observations

Status: source-reviewed and centrally integrated after the batching change; uncompiled and untested. No speedup has been measured. The same 44 readiness/observation sweeps remain at their existing source locations and in the same order. Each now copies the required registered scalar values without restoring the complete arena. There is no observation-value cache.

Cold-bound `Bank::ReadHandle<T>` admits only arithmetic or enum values fully contained in one registered span. Handles expose neither arbitrary offsets nor borrowed pointers. Every read copies a fresh value: camera slot zero from its live arena, other slots from their latest saved native state. Invalid, unbound, wrong-bank or leased reads reject. A scoped guard also prevents reads from inside legacy slot-zero callbacks without changing their existing nesting behavior.

The observation fields belong to the existing frame, AI, diagnostic, navigation, lap and binding records. Inline scene-node lanes are individually bound; copied pointer-bearing owners and shared RNG/noise state are not read through handles. Readiness checks, actual first-failure diagnostics, partial native state, source-manager order and success-only dynamic snapshot publication are preserved by source inspection.

Against the batched baseline, static accounting removes another 704 full snapshot passes, or 219,196,032 payload bytes per successful eight-AI world step, replacing them with approximately 9,728 bytes of scalar payload. These are copied-byte counts, not measured elapsed-time gains. Actual native steps, AI dispatch, route selection, collision, manager mutation, rendering, audio generation and clock policy retain their existing work.

The next authorized central run must measure physics CPU and wall/native progression while retaining all-eight original-AI acceptance, genuine source boundaries and audio-sample ownership. Error paths and runtime equivalence remain unverified. The earlier bounded runtime result belongs to source `d3d854b6`, as recorded in [current validation](current-validation.md). No software checks were performed during Git preservation.
