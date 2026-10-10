# Character mode-0 IK50KG

Status: source-backed staging implementation; never built, syntax-checked, tested,
run through the project parser, or observed in gameplay. It is not a declaration
that Rider transitions or the original UI route are ready.

## Recovered source and setup boundary

The source ELF is SLUS_202.68, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Staged evidence freezes the mode-0 solver28FFD0..2909FC, constructor28EB90,
mode dispatcher28F580, registration156620, default-vector initializers, inverse
vector helper28BBB0, local incremental rotation28B6A0 and cross13E440.

Registration156620 resolves an endpoint and its immediate parent/grandparent.
With absent axis input,1568DC..156920 passes433400 to the full28EB90 constructor,
then sets mode10=0. The static initializer3C8DD0..3C8E4C writes433400=(1,0,0,1).
The mode-1 Chariot Y axes must not be reused here.

For UI Rider, the original sequence has already started entry and advanced it
by .001 before capture155730 and registration156620. Supply this exact graph to
initializeModelMode0Ik50KG, not raw PSG bind matrices or a later transition
pose. Store the constructor record persistently; never reinitialize each frame.
Register authentic endpoints in source order. Names do not imply which thigh
parent to use: the actual19c hierarchy crosses the l_ik/r_ik labels relative to
leg names. This owner follows actual graph parent links without renaming them.

## Native ownership

The new header is character_mode0_ik50kg.h. Its implementations are added to the
existing chariot_ik49t.cpp, so no new translation unit or CMake source entry is
needed. The old constructor's axis selection is extracted into constructAxis;
the existing mode-1 entry still supplies its exact previous left/right values.
All scalar math, original acos, inverse transform, matrix multiplication,
axis-angle construction and scene cache setters remain owned by the established
49T/49P code. There is no guest-memory representation, guest/vector execution,
interpreter, hardware emulation, synthetic pose or fake solver-success path.

initializeModelMode0Ik50KG constructs one source record using supplied workspace
and source libm state. It derives the two ancestor indices and publishes record,
graph/cache and libm effects together only on success. Caller owns registration
order, missing-endpoint decisions and whole-model construction rollback.

solveModelMode0Ik50KG accepts one immutable record and a world target. It stages
graph nodes in caller workspace and stages both mode-0 lazy words. It publishes
only after success. modelMode0IkCallback50KG is the playback adapter, with a
bounded automatic workspace and immutable ModelMode0IkContext50KG pointing at
persistent records in registration order. It has no hidden mutable context.

ModelMode0IkShared50KG owns443460 and443468 separately from mode-1's443450/443458.
Both are lazily set to wordFFFFFFFF/guard1 when their source guard is zero,
at solve entry, even with debug40=0. They are neither prewarmed during setup nor
reset per frame. Playback must copy/pass/commit this shared object as part of
its outer graph transaction. The agreed ModelPlaybackIkSolve50KG signature is:

    bool callback(const void* context, uint32_t chain,
        const Vector3_48C& worldTarget, ModelMode0IkShared50KG& stagedShared,
        const SceneGraph49P& stagedGraph) noexcept;

## Source arithmetic map

1. 290230..290258 lazily read shoulder, elbow and endpoint world positions.
2. 290264..2902CC form target-minus-shoulder and scalar square-root distance.
3. 2902D0..29033C compute desiredCosine=(record14-distance*distance)*record18,
   clamp above+1 and below-1, and form desiredSine=sqrt(1-desiredCosine^2).
4. 290340..29044C normalize shoulder-minus-elbow and endpoint-minus-elbow;
   dot them for currentCosine and form currentSine=sqrt(1-currentCosine^2).
5. 290454..2904DC cross those normalized directions and scalar-normalize the
   result. The exact-zero path loads443410=(0,0,0,1), established3CADA0..3CAE1C.
6. 2904E0 MULA.S currentCosine,desiredCosine and2904E4 MADD.S
   currentSine,desiredSine compute deltaCosine. DeltaSine is the scalar root
   of1-deltaCosine^2. If currentCosine<desiredCosine, reverse XYZ by multiply-1.
7. 290554..29057C transform that axis into elbow-local direction and apply
   incremental28B6A0 rotation to the existing elbow local. This is not mode-1's
   replacement by a saved rest-axis rotation.
8. 290580..2906D8 reread endpoint world after elbow rotation; normalize the
   new endpoint-minus-shoulder and target-minus-shoulder; cross/normalize and
   transform the alignment axis into shoulder-local direction; apply28B6A0.
9. debug40=0 skips profiling and returns. No wrist-orientation operation occurs.

Both28B6A0 calls preserve all four old local translation lanes, invoke source
ancestor marking and conditional descendant invalidation, and leave world caches
lazy. Cached shoulder position from step1 is reused. There is no eager final
world refresh. Target weight zero does not bypass the solver when the original
playback blend path requests it.

## Admission and degenerate boundaries

- Finite local/world matrices for the whole admitted graph, coherent acyclic
  topology, valid immediate chain links, strict binary32 operations and nearest-
  even host rounding are required. Caller memory/lifetimes are disjoint and
  serially accessed. No allocations, filesystem access or I/O are introduced.
- Full constructor retains all coefficients and acos/libm effects, including
  fields unused by mode0. Existing source-libm mode1/default-matherr/cold-NaN
  admission applies. Finite, nonzero length/projected products are required;
  unsupported constructor degeneracy is reported, never replaced by invented
  coefficients or a shortened constructor that hides original effects.
- Runtime requires mode10=0 and debug40=0. Only record14/18, identities and the
  mode/debug fields are consumed. Unused rest/projection/axis fields do not
  impose new runtime numerical gates. Runtime does not call acos or alter errno.
- Original zero/collinear runtime vector paths remain present, including zero
  bend-axis fallback and saturated zero reciprocal root in existing49T math.
  No arbitrary pole, identity shortcut, target-distance epsilon, angle clamp
  beyond the two original desired-cosine clamps, or signed-angle approximation
  is added. A finite source path may produce a collapsed basis at a degenerate
  direction; success does not promise that an impossible target was reached.
- Existing scalar root handling preserves retained EE absolute-root and
  exponent-zero behavior. Nonzero subnormal VU squared norms, nonfinite input
  and consumed arithmetic, and debug/profiling paths remain unsupported.
- False leaves public graph/record/libm/shared state unchanged as applicable;
  diagnostic and scratch may change. A failed later chain does not publish an
  earlier one when invoked through playback's outer transaction.

## Outstanding verification

Only bounded byte extraction, instruction reading, text editing, hashes and
manual source comparison were performed. No build, syntax check, test,
sanitizer, project parser or gameplay execution occurred. Parent owns the next
eligible validation slot. Recommended coverage: authentic .001 initialization,
zero/intermediate/one blend weights, both clamped-distance branches, both axis
sign branches, source zero/collinear paths, repeated lazy state, ready/dirty
cache combinations, mode/debug rejection and late arithmetic rollback.
