# Independent semantic review: exact source-index stream

Reviewed 2026-10-11 UTC. **APPROVED for the narrowly scoped source-semantic change below. No semantic blocker found.** Root owns integration. Allocation, cleanup, and ownership lifetime are separately reviewed; this is not runtime or performance validation.

## Exact identities and scope

- Target: `platform/linux/native/vehicle_window_gl50ds.cpp`
- Exact preimage: `9947bea8f3719bdc174df6e065a731bfbfef1bc878c6fadccf2ba606df5a4516`
- Reviewed candidate: `c30fa4f3553bb66c187a3ae923936e9b2069d5ae44743e404ad9ec8b86ab6a9e`
- Reviewed patch: `44895fff5c99b2e994d22325c90b81c45859353762c80ca5ab66360f68e407a9`
- Active source remained the exact preimage at the final source read. No source was edited by this reviewer.

Read the complete candidate/preimage diff, original source submission and validation, vertex/fragment shaders, index initialization, source receipt/publication paths, and existing constants. Line references below refer to the candidate. The comparison contract is the original per-triple source path and its exact XY-pair skip, replacing the preimage's unsafe admission of extra duplicate halves to an opaque draw.

## Exact retained sequence

The packer at lines 4074–4099 traverses spans in their validated order, each span's quads in increasing order, and each quad's original corner triples in the order `{0,1,2}`, then `{0,2,3}`. It rejects a triple precisely when one of its three pairs has both equal X and equal Y. This is the same comparison, pair order, and short-circuit predicate as the original per-triple path. It does not add an area, Z, W, clipping, projection, or pixel-bound test.

For every retained triple, the three appended values are exactly `q*4+c[0]`, `q*4+c[1]`, `q*4+c[2]`. Those are the values the original immutable six-index-per-quad stream supplied at offset `q*6+t*3`. Thus this is a stable filter of the original primitive sequence, with no regrouping between spans, corner permutation, added triangle, or dropped retained triangle.

“Retained” means **not skipped by the old equal-XY predicate**. It does not mean geometrically nondegenerate. Three distinct collinear XY points, or a triangle that later collapses under transformation, remain submitted exactly as before. Equal-XY pairs with unequal Z/W are still skipped exactly as before. No claim about their hypothetical rasterization is needed because they never enter the new draw stream.

## Bounds, ranges, and empty cases

- Existing prevalidation bounds the source frame to 17,408 quads and 2,048 spans. Nonempty spans exactly partition all quads; the subtraction-based check prevents a span from exceeding the remaining partition.
- Each quad appends at most six indices. The full stream is at most 104,448 indices, 34,816 retained triples, and 417,792 bytes. Every vertex index is at most 69,631, strictly below the 69,632-vertex source capacity.
- `sourceIndexCount` starts at zero each call. Each span records its current first index before writing and its exact count afterward. Both remain multiples of three; all range ends are at most 104,448. Current span entries are always rewritten, and unused old entries are never read.
- The final fully retained quad exactly fills the allocation rather than exceeding it. The candidate's static assertions protect draw-count and byte-size conversions; the actual bounded values also fit `U` arithmetic and byte offsets.
- A zero-retained span has `indexCount=0`: opaque code issues no draw; feedback code issues neither draw nor barrier. An entirely skipped frame has a valid zero-byte upload and no source draw. State setup, span error checks, and successful authored-quad accounting still occur as in the original path.

## Upload and draw semantics

Lines 4209–4212 bind the source-only element buffer and upload the packed prefix once per source submission that reaches preparation. There is no per-span upload. The zero-count case also uses this single call with size zero. The UI VAO is already bound, and all vertex, UV, color, fog, W, and shader data remain unchanged. Ordinary RGB submissions explicitly bind the original immutable index buffer and retain their old draw path.

For opaque spans, lines 4251–4264 keep `sourceBlend=0`, destination texture unit 1 unbound, fixed-function blending disabled, and one `GL_TRIANGLES` draw over exactly that span's retained triples. The existing shaders use no vertex, primitive, or draw ID and no shader memory side effects. Triple order and each triangle's corner/provoking-vertex order are unchanged. Programmable primitive restart is already disabled; unsigned-int fixed restart cannot match any bounded source index.

OpenGL's primitive-order framebuffer semantics preserve the ordered depth/color effects of these opaque triangles, including overlapping triangles. This does not require fragment invocations to execute serially. The unchanged shader's destination fetch is only inside its nonzero source-blend branch. See [OpenGL 3.2 core, sections 2.1 and 2.6](https://registry.khronos.org/OpenGL/specs/gl/glspec32.core.pdf).

Inherited raster/position-transform states receive the same retained primitive inputs. In particular, the change does not need to prove that omitted duplicate halves stay harmless under conservative rasterization, viewport swizzle, W scaling, clipping, or GPU arithmetic: those halves are absent. Existing extreme-input or inherited-state behavior is not newly sanitized, and no new numeric or raster admission gate is introduced.

## Feedback, counters, and receipt boundaries

Feedback still binds the source color image and traverses each packed span in three-index increments. Every retained triple has exactly one `textureBarrier`, immediately followed by exactly one three-index draw, in original order. The same-pixel destination fetch and all blend coefficient, alpha, fog, depth, and discard arithmetic are unchanged. No rectangle or non-overlap assumption is used. This retains the existing single-read/write-per-texel feedback design and prior-write separation required by [ARB_texture_barrier](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_texture_barrier.txt); it does not establish new raster guarantees beyond the original path.

The span GL check remains the commit boundary for successful draw, triangle, and authored-quad counters (lines 4277–4279). Successful opaque draw counts intentionally become one per nonempty span; triangle counts are the exact retained-triple counts. Feedback draw counts equal retained-triple counts. The barrier diagnostic remains an attempted-command counter, incremented before each draw, including attempts in a subsequently failed span. This pre-existing distinction is intentional.

CPU packing occurs after input validation and before GL preflight; it changes only scratch data. The new upload is covered by the existing preparation check. `sourceWorkStarted` stays at the same point immediately after successful preparation and before the span loop (line 4217). New upload errors therefore issue no source draws and do not move the material-history poison boundary. The pre-existing target-generation/owner-retention behavior after a source clear remains unchanged. Once span work starts, partial failure still reaches the existing receipt/poison path.

Restoration rebinds the immutable element buffer while the UI VAO is current and before its final GL check. A restoration failure prevents private publication; prior successful-span diagnostic counters remain diagnostics. Source-to-presentation copying, source generation, successful private publication, and later window-copy failure handling are otherwise unchanged.

## Evidence boundary

Performed ordinary source/specification reads, text diff inspection, hashing, and this staged review write only. No build, test, syntax check, project parser/decoder/importer/script, or native/guest execution occurred. Approval is hash-specific and limited to the exact-index submission semantics. It is not measured pixel equivalence, cross-driver equivalence, frame acceptance, or a speedup claim. The separate lifetime review and root integration checks remain required.
