# Reuse successful per-surface diffuse coefficient admission

UNTESTED source change. No build, test, smoke, syntax check or benchmark.

The first vertex of each retained RA diffuse surface still calls
projectDiffuseCompressedRgba. A complete result admits the existing float ABI,
FE_TONEAREST mode, nine normal-axis coefficients and sixteen color coefficients.
Later vertices use projectDiffuseCompressedRgbaFiniteInputs, the same arithmetic
core already called by the public wrapper and admitted racer rendering loops.

All dot, multiply/add, finite intermediate, clamp and packed-domain checks remain
inside the unchanged core. First-vertex validation order and failure precedence
remain exact. An empty surface calls neither function. The output push and
vertex diagnostic increment remain after successful projection, so failed output
and vertex position are unchanged. No camera/material freshness check is removed.

The input is a surface-local value copied once from the validated snapshot; the
loop does not mutate it or expose it to a callback. Existing bounds prove
recovered.count == surface.vertices.size() == plan.vertexCount. The destination
vector reserves this count before the loop, and its trivial four-byte element
push cannot allocate within the loop. No loop operation changes rounding mode.
The wrapper does not inspect or clear sticky FP exception flags. This relies on
the existing serial FP contract, not an arbitrary signal handler changing fenv.

For a surface of V > 0 vertices, the source removes V-1 repeated readiness calls
and 25*(V-1) invariant finite predicates. A branch remains per vertex. Compiler
and runtime effects are unmeasured; successor should compare exact diffuse bytes
and failure diagnostics, then assess comparable frame timing. It is not a fix
for the unresolved Windows track-load report.
