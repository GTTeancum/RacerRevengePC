# Original engine-node animation, checkpoint49Q

This stage replaces supplied engine-node local matrices with the original control-driven Pod01 animation producer. It consumes the recovered aero E4/E8 outputs, the application's animation frame factor, persistent filter state and actual engine MOT data. It changes the authored brakes and fins, preserves unaffected nodes, and executes the original dirty/lazy-cache semantics. It does not produce the later engine model-root transforms from21C120.

## Original selection and complete output boundary

The original ELF is `work/native/project/disc/SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. The source selection and all15 retained Pod01 asset hashes are recorded under `evidence/engineAnimation49Q/assessment/motion-selection`.

Source2136CC..213A24 resolves `01_engineleft` and `01_engineright` from the authored MCF record set. They select `50-englt1.mot` and `45-engrt1.mot` respectively. Each motion has five ByteAxis rotation tracks and five Bezier position tracks, bound to brake1, brake2, fin1, fin2 and fin3 in its14-node hierarchy. Binary duration is3FAAAAAB; the MCF text's FPS/FrameCount is not the sampling clock.

Both engine Mid records are absent in the three authentic selected MCFs. Source1523F0(a2=0) returns null after an unsuccessful lookup. This is a bounded resolved-record admission: no foreign matching record or replacement may be present. Unexpected engine Mid records are rejected by the new producer's context gate.

The remaining21A000 code applies Forward/Back and hand IK only to the distinct Chariot hierarchy. Actual Pod01 has both IK chains and targets; they are not assumed absent. The new output therefore represents complete engine node locals at the animation phase, while Chariot matrix/IK effects remain outside the projection. The caller must preserve source-distinct model roots and corresponding descendant binding rows. Later21C120 can move the engine roots and optionally rotate listed nodes; that stage remains separate.

## State and order

Original base update228620 calls controls217DE0/2176E0, reloads global application+23C at2287B8, calls animation21A000, then21C120 and21A660, before later physical substeps. The animation dt is supplied explicitly and must not be replaced with a control argument or physics substep dt. The phase enum is a checked caller assertion, not an implementation of the whole scheduler. A diagnostic that invokes the normal base route must also honor its source mode gates.

`advanceEngineAnimation49Q` preserves this represented order:

1. Lazily initialize shared434748 to1 when its guard byte is zero; a warm zero value skips animation.
2. Update B00 toward aero E4, then B08 toward E8. Each executes `alpha=MIN.S(dt,time)/time`, then `value=old+alpha*(target-old)` in separate binary32 operations.
3. Copy the filter values to B04/B0C. Clamp those mirrors when the earlier Chariot brake gates require it. No Chariot matrix effects are supplied by this engine-only projection.
4. If the left engine's motion/root/byte191-bit0 gate passes, clamp B04 and sample the left engine.
5. Apply the equivalent right gate, clamp B0C and sample the right engine.

Byte191 bit0 remains a numeric source gate; it is not assigned an unproved visibility meaning. The filter's persistent value is never replaced with its clamped mirror. Both filters advance even when both engine gates skip. Each successful single-motion call clears that model's+18 word. Skipped models retain their matrices and model18.

`initializeEngineAnimationFilters49Q` closes the two relevant numeric initializer records: seed from explicitly supplied B04/B0C, set time to3E4CCCCD and alpha to1. The original fresh constructor seeds are separately source-proven zero, but this API does not silently reset a running history or construct a whole vehicle. Source filter+C/+10 bounds are not consumed by21A000 and are not invented clamps.

## Single-motion numeric and scene behavior

`sampleModelMotion49Q` recovers1559F0 with mirror=0 and row-mask=null, exactly as invoked by21A000. Time is fraction multiplied by the selected MOT duration. Every rotation track runs in source order before every position track, including repeated bindings. Binding+38/+3C flags do not select or suppress tracks.

The helper evaluates one motion directly. Calling the paired49O sampler with equal inputs introduces extra quaternion arithmetic and is not source equivalent. Rotation setter28A580 writes the upper12 matrix lanes, explicitly zeroing the three basis fourth lanes, while preserving all four translation lanes. Translation setter28AC10 changes XYZ only. Each track uses the equivalent49P local setter's ancestor80 marking and ready01-gated descendant invalidation. Worlds remain cached until a later explicit getter. Already-dirty nodes with ready descendants are intentionally not repaired eagerly.

The implementation retains the original software-generated trig tables and the existing explicit cold-zero acos padding boundary. It assumes the retained nearest/even separate-binary32 model; it does not establish a new unconditional physical-console FCR claim.

## Native admission and storage

All source views are bounded, successfully decoded, alive and unchanged. Graphs contain at most64 coherent nodes; binding rows explicitly identify graph indices, with no invented hierarchy-offset arithmetic. The engine adapter requires distinct unparented model roots and active bindings confined to their respective subtrees. The sampler accepts nonmirror, unmasked tracks only. Consumed times/fractions/intermediates and represented matrix lanes must meet the documented finite domain; unsupported rounding rejects before represented output mutation.

Mutable graph, state, caller scratch, diagnostic and immutable inputs must be distinct live records. Defensive overlap checks cover the documented spans only. They do not prove pointer lifetime, validate caller-forged views or establish comprehensive alias safety; diagnostic and graph/state versus immutable inputs retain explicit caller preconditions. Under admitted records, failure preserves represented graph and state; scratch may change. The two transaction arrays are fixed caller-owned storage, and the hot path performs no allocation, I/O or callbacks.

Existing49O and49P production files are retained unchanged. Full Chariot animation, container response, complete engine roots, rendering, original heap/lifecycle behavior, authentic spawn and contacts are not closed by this stage. The historical capacity-alias and49C allocation-failure operations remain untouched;49C is excluded and unreleased.

## Evidence

- `single-motion/`: original1559F0, evaluator and scene-setter words, actual six MOT inputs, exact all-lane and ordered flag/track comparisons, semantic controls.
- `flow/`: original21A000 filtering/gates, original initializer words, actual Chariot prefix execution in the reference and explicit engine-only comparison, unchanged unrelated native nodes.
- `connected/`: retained control sequences, real authored brake/fin motion, persistent state and explicit later-world requests.
- `independent-review/`: independent source/assets checks and orthogonal semantic tests.
- `footprint/` and `benchmark/`: installed-host measurements with exact source pins; no FPS or Xbox claim.

Normal and ASan/UBSan modes are required. LeakSanitizer is not claimed under this environment's recorded tracing limitation. Final aggregate and recovery status are recorded by the checkpoint validation and archive manifests.
