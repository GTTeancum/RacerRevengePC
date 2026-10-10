# Original UI framebuffer alpha: source-only audit

Date: 2026-10-10. This is new research, not recovered historical source. The active repository was read only. No build, syntax check, test, importer, archive/RES decoding, game launch, guest execution or emulation was performed. A byte-to-text MIPS listing helper was used only for static inspection of the ELF. Listings intentionally leave unrecognized instructions unknown.

## Result

Do not assert `opaquePresentationAlphaOnly` or activate original UI on this evidence.

The current strict material subset has no immediate destination-alpha read: DATE must be zero; when ABE=1 it accepts only ALPHA=(0,1,0,1) or (0,2,0,1), both with C=source alpha. With ABE=0 the ALPHA tuple is inactive. This is a per-span result, not a proof for Main, a full frame, another render target, or a later consumer.

The original source explicitly supports destination-alpha blend factors, destination-alpha tests, render-target alpha sampling and framebuffer write masks. Their actual presence or absence in the final Main material/pass/target graph remains unproved. In particular, the existing material adapter does not own FRAME, its mask, render-target transitions or callbacks.

A narrower positive fact is established: normal video-state setup in the authenticated 640x448, colorBits=32 mode selects color format PSMCT32 (0), depth format 0x31 and a zero FRAME write mask. The default clear path submits raw alpha byte 0x80. This is not an alpha=1 storage proof. Source-RGBA storage and final presentation alpha must be separate contracts.

## Primary source and provenance

ELF: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

For the inspected loaded segment: `fileOffset = VA - 0x100000 + 0x80`. Exact words and per-region SHA-256 hashes are in `exact-source-anchors.json`. The native file hashes in that file identify the source inspected, not a compiled or tested product.

Existing depth/order evidence remains applicable: `recovery/20261010/ui-depth-reconstruction/SOURCE-EVIDENCE.md`. Its registered-index order, redirect resolution, pass barriers and leaf FIFO must be preserved. This audit does not replace those owners with a painter-order assumption.

## 1. What can read or preserve alpha

### Material blend and tests

- One-map ALPHA.C getter: 0x247A64. Its value is shifted four bits at 0x247AF0 and included in ALPHA_1, register 0x42, at 0x247AE8..0x247B10. The original builder does not globally force C=0. The known runtime override instead uses C=2 and FIX=16 at 0x247AC4..0x247AD0.
- DATE and DATM getters: 0x247E88 and 0x247EA0. They are shifted into TEST bits 14 and 15 at 0x247EFC and 0x247F14, then emitted to TEST_1=0x47 at 0x247F24..0x247F34.
- FBA getter/build is at 0x247C24..0x247C54. AFAIL is also source-authored through 0x247E70. Neither is equivalent to framebuffer alpha being globally irrelevant. PABE depends on source alpha, not destination alpha.
- The current CPU policy rejects DATE, FBA, PABE and alpha testing; its active blending rejects C=1. It therefore does not implement any of those rejected states.

Hardware interpretation cross-check only: C=1 selects destination alpha; DATE inspects the destination alpha most-significant bit; FBA can force that bit; FRAME.FBMSK is a per-bit preserve mask. A nonzero mask does not itself make destination alpha an RGB blend coefficient, but can preserve old alpha for later consumers. These are distinct dependencies. See PCSX2's primary [register definitions](https://github.com/PCSX2/pcsx2/blob/master/pcsx2/GS/GSRegs.h) and [scanline implementation](https://github.com/PCSX2/pcsx2/blob/master/pcsx2/GS/Renderers/SW/GSDrawScanlineCodeGenerator.all.cpp#L2378-L2419). Reading these files did not execute an emulator.

### FRAME masks are outside the one-map material policy

The complete state builder 0x2476A0..0x2480C0 does not emit FRAME_1/FRAME_2. Its source material fields cannot certify their effective inherited values.

A separate render-target preparation function 0x249DF0 initializes its mask variable to zero at 0x249EA4, then reads the actual `textureFrameBufferWriteMask` field at 0x249F20. It passes the result on the stack at 0x24A224..0x24A228 to packet builder 0x24C670.

That callee reads the argument at 0x24CC44, shifts it by 32 at 0x24CC60, joins it with FBP/FBW/PSM at 0x24CC68, and writes both FRAME contexts at 0x24CC70..0x24CC90. Thus authored mask bits can explicitly preserve alpha and/or RGB bits. Default mask zero in this parser is not proof that the real group authored zero.

The same owner accepts `renderTargetLocation=FrameBuffer`: string comparison 0x24A078..0x24A088 and framebuffer address lookup 0x24A090..0x24A0B8. It is not safe to assume the mask mechanism only affects an unrelated offscreen texture.

### Target reuse and sampling

`TextureMap_N=RenderTarget` is recognized at 0x2472A0..0x2472C0 and represented by texture slot -2. Group submission recognizes -2 and calls 0x249920 at 0x24B21C..0x24B238. That function reads target FRAME at 0x249964 and scissor at 0x249968, derives TEX0's address/format/dimensions, and replaces the address fields while retaining the material TCC/TFX bits at 0x249B14..0x249B70.

Consequently an earlier target can become texture input, including its alpha when the subsequent shader's actual TCC/TFX uses it. This establishes a real source mechanism, not proof that Main uses it. The current bounded reader explicitly rejects RenderTarget names. Source group pre/post actions execute around queued draws at 0x24B8EC..0x24B924, so examining only the accepted UI spans cannot exclude intervening target/mask owners.

## 2. Exactly what default640x448 proves

- Default mode request 0x2379DC..0x2379E8 supplies width640, height448 and colorBits32 to the actual video owner.
- The matching setup branch at 0x25BAB0..0x25BAD4 stores color PSM=0 at owner+0x58, ZBUF format0x31 at +0x5C, and packet-Z maximum0xFFFFFF at +0x60. The alternative branch uses color PSM0x0A and maximum0xFFFF; it is outside the current native subset.
- Video state setup 0x25BE80 constructs FRAME_1 at 0x25BEC0..0x25BEF4 from framebuffer base, `(width>>6)<<16` and `colorPSM<<24`; it supplies no high-word mask. FRAME_2 is similarly emitted at 0x25BFE8..0x25C014. With bounded valid framebuffer addresses, the default32-bit mode therefore has FBMSK=0 at these writes.
- The packet helper 0x259510 calls 0x259A40, which stores the low/high data words and register number explicitly at 0x259A50..0x259AA0. This supports interpreting those calls as register writes rather than treating their arguments as unexplained constants.
- The viewport clear path at 0x25C5C0 constructs RGBAQ with alpha0x80 at 0x25C958..0x25C974. Its untextured sprite PRIM is0x06 at 0x25C92C..0x25C938. Flags3 select TEST0x3000D at 0x25C898..0x25C8A8, with destination-alpha testing off. The sibling initial clear path likewise packs alpha0x80 at 0x25C44C..0x25C468. Mode initialization calls that sibling with flags3 at 0x25A890..0x25A8C8.
- Raw alpha0x80 is an eight-bit source value. The constant alone does not select GL0.5, GL128/255, or GL1.0. Any native representation must explicitly establish byte conversion and quantization. Under a deliberately chosen byte-preserving RGBA8_UNORM representation, byte128 is represented by128/255; that is a proposed encoding convention, not a newly discovered original GL value.
- The normal display-control branch writes PMODE=(ALP<<8)|0x63 at 0x25BC7C..0x25BCA0 while owner flag+0x2EF bit0 is set. That constant selects fixed-alpha merge mode. The constructor sets bit0 at 0x259F60..0x259F6C, so this particular branch does not read source framebuffer alpha as the merge factor. This does not prove lifetime invariance of the flags or rule out other presentation paths. The source path that skips the write inherits earlier PMODE state. PMODE bit layout is in the linked register definitions.

Nothing about width640/colorBits32 eliminates actual material overrides, group actions, RenderTarget sampling, alpha writes or future reads. A zero FRAME mask at setup only remains effective until a later FRAME write. The complete Main graph and subsequent target lifecycle were not established, and no actual UI PSM was decoded during this audit.

## 3. Present native behavior and minimum safe contract

Current code requires the Boolean proof at vehicle_window_gl50ds.cpp:3608, requires presentation clear alpha1 at:3616, clears alpha1 at:3696, resets backdrop alpha at:3707..3708, and disables alpha writes at:3710. It uses the same blend function for every channel at:3738, but its alpha write mask makes that channel irrelevant to this presentation target. It intentionally does not recover original alpha. Its RGBA readback and window blit expose presentation data.

There are two legitimate bounded contracts:

### A. Closed RGB-only presentation

Keep the existing target only when an authenticated composition owner establishes all of the following for the whole relevant lifetime:

1. Exact ordered/enabled/animated source draws and all group pre/post actions are covered, with target identity, context, storage format, FRAME mask, depth initial state and clear/retained color ownership.
2. All relevant draws meet the existing strict policy, source RGB masks are implemented or known zero, and no effective DATE or destination-alpha coefficient occurs.
3. The source target's alpha cannot affect any subsequent sampler, copy/alias, draw, presentation merge or later frame before the source lifetime ends or alpha is overwritten independently. A list of current spans alone is insufficient.
4. Alpha is deliberately declared disposable presentation data only after that proof. Replacing this obligation with a Boolean default or texture-bank identity is invalid.

This certificate does not currently exist.

### B. Source-RGBA composition, then opaque presentation

This is the minimum implementation direction when alpha independence cannot be established, not an authorization to enable unsupported source effects:

1. Own a separate source-composition surface with explicit raw color/alpha representation, exact640x448 source dimensions, source format, D24/reverse-depth state, clear/retained-content identity, frame generation and lifetime. Present scaling/letterboxing only after source composition when exact source-pixel semantics matter. Reject unknown retained alpha/depth; copying the current race renderer's presentation RGBA is not source recovery.
2. For the already-admitted MODULATE subset, retain independent source-alpha store and RGB-factor semantics. The existing policy proposes color/stored-alpha scale255/128, RGB blend-factor scale(255/128)^2, and stored-alpha-from-factor scale128/255. If using normalized raw texture/vertex bytes, opaque draws use stored-alpha scale; blended draws need separate alpha replacement, never source-over alpha accumulation.
3. A possible floating native mapping uses separate RGB/alpha blend factors: the existing source-alpha or additive RGB pair, and CONSTANT_ALPHA/ZERO for alpha with constant128/255 when the shader emits factor-units alpha. Opaque blending is disabled and emits stored-units alpha. The current GL table lacks BlendFuncSeparate/BlendColor. These mappings are implementation proposals under the selected normalized encoding, not proof of GS integer rounding or visual equivalence. Specify and validate source sampling, interpolation, MODULATE quantization/clamp and byte-store conversion before relying on stored bytes or threshold tests.
4. Start with source FRAME.PSM=0, context0 and FBMSK=0 only, and reject unimplemented DATE, C=1, FBA/PABE, AFAIL, bitwise masks and RenderTarget consumers. Supporting an actual dependency requires its native operation and ordered target lifetime; merely storing alpha cannot make those consumers implemented. DATE requires the source alpha bit, C=1 requires the source coefficient convention, masks require preserve-bit semantics, and feedback needs an order-safe source surface/copy without an undefined simultaneous read/write hazard.
5. Preserve source alpha throughout all authorized source consumers. Only after the last such consumer, copy composed RGB into a separate opaque presentation output. Do not overwrite the source target alpha for window/readback convenience. Current copyToWindow performs a full-color blit, so a source-alpha path needs an explicit final alpha1 presentation step rather than publishing its raw source surface unchanged.
6. Preserve current transactional publication, immutable texture/sampler checks and failure rejection. Neither this contract nor separate-alpha storage closes missing pass graph, material bytes, video/backdrop ownership or runtime animation state.

No active code or activation flag was changed. The recommended immediate outcome is to leave original UI inactive and retain the unresolved composition-owner blocker.
