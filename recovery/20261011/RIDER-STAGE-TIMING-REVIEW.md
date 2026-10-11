# Independent source review: stage observations

Reviewer: parent investigation agent, independent of candidate author
`audit_stall_stage_timing`. Reviewed the complete two-file patch and surrounding
host capture/update/render/exception code, plus the const noexcept frontend
snapshot implementation.

Verdict: acceptable for opt-in diagnostic integration. Enable only via
`RR_TRACE_ORIGINAL_UI_STAGES=1`; no cost attribution is established yet. No build,
test, syntax check, project parser/import/decoder or executable launch was run.

## Checked properties

The new steady-clock samples are local observation values and never assigned to
simulation/audio time, accepted update deltas, cold preparation exclusions or
the one-second stall limit. Existing coordinator/audio return checks preserve
short-circuit evaluation and their original failure calls. Accepted sequence
and PCM advancement remain exactly once in the original order. No GL finish,
fence or query is added. The frontend snapshot only copies existing state.

When disabled, stageStart returns a default time point without sampling the
clock and reportStage returns immediately. When enabled, each output interval
ends before formatting and the snapshot read. Those costs remain real elapsed
work in the outer active interval; they are not hidden from the stall gate.
Screen/page bytes are explicitly streamed as unsigned numbers. Stage names are
fixed literals, booleans are JSON literals, and the output is single-line.

Native submission is host elapsed time and may leave GPU work queued. Readback
may include that deferred work. File-publication time covers conversion,
writing, close and successful atomic rename; failed writes preserve the
original exceptions and do not manufacture a completed-stage receipt. Swap is
reported only if a host window exists. Logging sits before the original ACK;
it can increase observed ACK latency. It does not change receipt fields or the
recorder's source-time rules.

The optional output is diagnostic stderr and can itself block. Treat enabled
runs as instrumented observations, not precise comparisons against prior runs.
No attempt was made to subtract logging cost from accepted time.

## Identity

Exact originals and staged candidates are included in `preimages/` and `files/`.
Patch SHA-256:
`6ae669ec007bc1e2067bdea0e6049475f63262e787bcfcdc53d425a2298a2e78`.
`SHA256SUMS` covers both originals, both candidates, patch, report and this review.
