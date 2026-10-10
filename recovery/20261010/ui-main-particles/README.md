# Native Main ParticleCode reconstruction

Staged implementation only. No active repository files were edited. No build,
syntax check, shader check, importer, test, game, or guest instruction execution
was run. Static source-data decoding was used to read original instruction
words; the production implementation is direct native arithmetic and GL.

## Deliverable and joins

- `src/assets/original_ui_main_particle_material50kg.{h,cpp}` admits the real
  authenticated UIMAIN `particle/mmprt.psm`, actual DefaultMT owner, normal
  material runtime, and the caller's real decoded `particle/mmprt.pst`.
- `src/game/original_ui_main_particle_geometry50kg.{h,cpp}` expands one actual
  fire-owner packet into a CPU quad; no particle generation is added here.
- `platform/linux/native/original_ui_main_particle_adapter50kg.h` converts
  either selected state into the shared ordered native draw protocol.
- `original_ui_draw50kg.h`, `original_ui_native_draw50kg.h`, and
  `vehicle_window_gl50ds.cpp` add a typed particle mode and bounded parallel fog
  stream. Exact active-file preimages are in `preimages/`.

The geometry function receives the recovered particle camera and explicit
source clip matrix, fog coefficients, canvas offset, and four lighting colors.
It returns visible=false for source center clipping, otherwise one quad and
four fog factors. Native corner order BL,TL,TR,BR preserves the source triangle
strip diagonal BL--TR with the existing 012/023 index buffer.

The assembler owns original signed run parsing, which selects state0 for
positive and state1 for negative runs of at most12 particles. It appends at the
actual particle-manager tree point and drains the recovered mmprt leaf first,
preserving FIFO within that leaf. Two maps are selectable states in ONE source
shader group, not two successive render passes over each particle. The actual
mmprt image must be added to the same immutable UI resource bank, with linear
min/mag and repeat S/T. The adapter's state-specific sampler supplies that
policy rather than a texture-name guess.

The final draw sets `particleFog`, `particleFogCount` (exactly4*quadCount), and
`particleFogColor[3]` (packed source FOGCOL bytes /255). UI fog slots are zero;
particle slots are source integer fog coefficient divided by256. The ordinary
textured and untextured fragment branches retain their previous equations.

## Source evidence

Original executable: authenticated SLUS_202.68, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Program registration15065C resolves ParticleCode to3212A0. The program header
loads158 instruction pairs; `evidence/particle-program.words.txt` records their
static decoding and `read_particle_words.py` is the bounded read-only decoder.
It never executes or simulates any decoded instruction. Unknown irrelevant
lower words remain labeled unknown rather than silently guessed.

-3214B0..3214C8: center times source screen matrix.
-3214D8..3214F8: center times clip matrix;321518/321538 drive whole-quad clip.
-3214E8: reciprocal centerW;321520/321528: screen center and size scaling.
-321548..3215C8: actual TL,BL,TR,BR corners, asymmetric0/4094 clamps, fixed4 XY.
-3213C8..3213E0 and pipelined continuation: alpha from brightness+8388608 float
 bits, yielding nearest-even integer alpha in the admitted0..128 range.
-3214D0/3214F0: fog=(quantizedSourceDepth-fogStart33Z)*fogScale33W;
 321510/321530 clamp0..254; packed FTOI4/XYZF2 retains integer fog.
-321410/321440: positive run selects data50, negative run selects data54.
-2480D0 two-map builder and literal getter map are recorded in the companion
 `two-map-builder.words.txt` and `two-map-field-literals.txt`.

Original Main mmprt PST SHA256:
`a75199550efd67e725f1a1432498436439f0d825b7ecc2f586d25240fe88e84f`.
The atlas is128x256, one decoded indexed-RGBA level, and contains real raw
alpha255. The initial texture-only alpha<=128 assumption was disproved by
independent original palette/used-index inspection and removed before delivery.
The main scene/fire owner and particle-camera/lighting owners retain their own
source evidence and authenticated identities.

## Actual material and alpha policy

Particle creation160FF4 reads global3833E8. Startup17CAE0..17CAE8 binds that
owner to AppStart/DefaultMT, not ScreenMT. Source mmprt leaves fog/alpha-test
fields blank; DefaultMT therefore supplies fog ON, alpha-test GREATER0 with
KEEP-on-fail. Both actual states use MODULATE/RGBA, source-alpha RGB factor,
GEQUAL depth, no Z writes, repeat sampling, no DATE/FBA/PABE, no AA or alternate
fragment target. State0 computes(Cs-Cd)*As/128+Cd; state1 computes Cs*As/128+Cd.
Context0/1 selects the corresponding source state, not a second UI pass.
The source framebuffer/depth equality is concrete:24C480..24C4FC writes FRAME1
and FRAME2 from the same262E90 getter and width;24C500..24C584 writes both ZBUF
registers from identical262EE0/262EF0 getters. Group prologue24AFAC..24B018
patches both FRAME lanes from262E90(global443A60); its ZBUF lanes are likewise
paired. Evidence is in the assembler stage's `restore-target-registers-24C480-24C740.txt`
and `group-prologue-24AF30-24B190.txt`. This supports folding the two material
contexts into the same native Main presentation/depth owner.

The native shader applies RGB255/128 modulation and clamps before blending;
blend alpha uses(255/128)^2 with raw normalized texture and vertex alpha.
The cold renderer records each immutable atlas's actual maximum alpha. Before
target writes, every blended textured span requires each vertexAlpha times
atlasMaximumAlpha <=16384. Interpolation and linear sampling preserve those
upper bounds. Thus original texture255 with actual Main UI vertex63 or fire
vertex64 is valid, while unrepresentable blend factors>1 reject explicitly.
The older texture128/vertex128 domain remains valid too. No alpha byte is
rescaled, clipped, or replaced. Main fire is nongrowing size7, active age<=life,
and source brightness64*age/life, so its source maximum is64.
The alpha-test discards values whose modulated stored alpha is below one source
byte. Fog changes RGB only and uses the source weights F/256 and(255-F)/256.
The GS manual's fog equation (section3.5) explicitly uses the denominator256,
not255. Alpha is not fogged.

Per-leaf `noDestinationAlphaConsumer` means neither actual particle state reads
framebuffer alpha: DATE=0 and ALPHA.C=source alpha. It does NOT establish the
whole frame promise. The final Main assembler must join all other leaves,
original frame/depth ownership and final consumers before asserting
`opaquePresentationAlphaOnly`. Native presentation clears alpha1, writes RGB
only, and is not presented as recovery of source framebuffer-alpha bytes.

## Native precision and limits

CPU equations use separately rounded IEEE binary32 operations with nearest-even
rounding and no multiply-add contraction. This is a finite host rendering
policy, not an implementation of VU pipeline/flags/denormal behavior. Divides
use host binary32 rather than a guest reciprocal-unit simulator. Positive
packetZ is truncated to the integer carried by packed XYZF2; XY is truncated to
fixed4 before subtracting source XYOFFSET(1728,1824).

GPU sampling/interpolation, fog RGB and blending use native GL arithmetic.
Intermediate GS texture/filter/fog byte truncations are not reproduced bit for
bit; the authored atlas, material, color scale, selected blend, fog weights,
source alpha test, geometry and ordered submission are preserved. No hardware
instruction interpreter or generic GS emulation is included.

Invalid nonfinite inputs, out-of-range packet/frame/bucket values or unsafe
coordinate ranges fail before output mutation. Actual source center clipping
returns a successful invisible quad. Other failures must not consume stale
packets or prior geometry. The GL branch requires the same real published image
and sampler ownership as ordinary textured UI, a complete bounded fog stream,
and the explicit source depth and opaque-presentation-alpha contract.

## Integration status

Material, geometry, adapter and GL implementation are ready for root integration.
Camera worker is finishing authenticated fog upload constants and packed fog
color provenance; scene worker owns exact lighting (all four Main RGB49).
These joins must be
recorded in the final Main compositor, not guessed here. This stage is untested.

Static preimage SHA256 pairs match the active repository exactly at17:11UTC:
draw header9da1dadbddd5779306c1acaa0e1af860b7e6f3ce9ad33b356084166da39413dd;
native draw header5a91d8ca8c2fc12305c7dbb4f78e1b9c781c24c0560c36e0f7eaf7dfb4008207;
renderer76f6ab80821563d3811db69af2f1f7f4234761122c5dd371f4b5fadc360858e3.
