# Original UI packet depth and ordering: static source evidence

Date: 2026-10-10. This is new research, not recovered historical source.
Repository inspected: `/workspace/shared/rr-recovery-git`, task baseline `b695bf2`.
No repository edits, guest execution, PS2/VU emulation, project scripts, asset decoders/importers, compilation, syntax checks, tests, or game launch were performed. The executable was only hashed, read as bytes, and statically inspected. The native header is inactive and unvalidated.

## Result and scope

The default original video mode explicitly selects 640 x 448, 32 color bits, and a packet-Z maximum of `0x00FFFFFF`. The alternative 16-color-bit mode uses maximum `0x0000FFFF`; both source branches retain ZBUF format `0x31`. Color depth and depth-buffer format must not be conflated.

Main background Z is `16777200` (`0x00FFFFF0`), and the initial arrow texture depth of 1 produces `16777214` (`0x00FFFFFE`), for that authenticated default mode. Icon negative authored depth produces packet Z `300`. Text retains its actual authored/assigned depth and must use the source sign branch. Final conversion is FTOI0, truncation toward zero in the admitted range, not text-metric nearest-even conversion.

A final draw list must preserve resolved pass-group aliasing, registration order, pass barriers and FIFO order. Original submission order alone is insufficient. This pass did **not** close absence of downstream framebuffer-alpha consumers or the actual live material registry/enable/animation state. UI must remain inactive until those owners are supplied.

## Primary source identity

Original input:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

ELF loaded segment: VA `0x00100000`, file offset `0x80`, file size `0x2D5A00`. Throughout:
`file_offset = ELF_VA - 0x100000 + 0x80`.

`exact-source-anchors.json` contains exact little-endian words, source offsets and per-region hashes for the critical findings. MIPS PCs below are original executable virtual addresses; embedded VU references are separately labelled payload storage VAs, not native function addresses.

## 1. Actual video owner and mode selection

- `0x2376A0` calls the allocator with `a0 = 0x510` (set at `0x23769C`). `0x2376B0` calls constructor `0x259E00`. `0x2376BC`, word `AC223A60`, stores the returned owner in global `0x443A60`.
- Constructor `0x259F20`, word `AC640008`, stores colorBits `32` in owner+`0x08` (constant set at `0x259EF0`).
- `0x259EE4 = 3C0200FF`; `0x259EE8 = 344CFFFF`: construct `0x00FFFFFF`.
- `0x259F48 = AD6D005C`: stores ZBUF format `0x31` in owner+`0x5C`.
- `0x259F50 = AD6C0060`: stores the maximum in owner+`0x60`.
- Default mode method `0x2379C0` loads that same owner at `0x2379D4`; supplies width `640` (`0x2379DC = 24050280`), height `448` (`0x2379E0 = 240601C0`), and colorBits `32` (`0x2379E8 = 24070020`) to virtual slot `+0x10` at `0x2379F8`.
- Constructor's vtable pointer is `0x3D22C0`; immutable word at vtable+`0x10` names `0x25A7B0`. This method stores requested colorBits into owner+`0x08` at `0x25A7FC`, then calls mode setup `0x25B9F0` at `0x25A864`.
- In `0x25B9F0`, `0x25BAB0` reads colorBits and `0x25BAB8` compares to `32`. The matching path stores maximum `0x00FFFFFF` at `0x25BAD4` (`AE020060`). The nonmatching path stores `0x0000FFFF` at `0x25BB38` (`AE020060`, preceded by `3402FFFF`). Both branches store `0x31` to +`0x5C`.
- The mode table built by `0x25A3E0` contains colorBits 32 and 16. This is not justification to admit arbitrary non-32 values to the native renderer.

The existing renderer accepts D24 packet positions on a 640x448 canvas. Exact safe admission is an authenticated owner whose dimensions are 640x448, colorBits=32, maximum=0xFFFFFF and ZBUF format=0x31. A correctly authenticated 16-color-bit owner must be explicitly rejected by the current D24-only candidate rather than scaled or implicitly treated as the default. Merely being numerically <=0xFFFFFF is insufficient ownership proof.

## 2. Source float-Z formulas

Let `M` be the signed integer video owner+0x60. Let `F32` denote the original single-precision result of its individual arithmetic instruction; the native candidate conservatively restricts subtraction to exact integral operands.

### Assigned ScreenTexture depth and ScreenText

`z_float = depth < 0 ? -depth : F32(float(M) - depth)`.

- Assigned ScreenTexture path `0x2ACA44..0x2ACA88` compares to zero, stores assigned depth at object+`0x40`, negates on the negative branch, otherwise reads video+`0x60`, uses CVT.S.W at `0x2ACA80`, and SUB.S at `0x2ACA84 = 46000801`. It copies the result to all four vertex Z floats at packet+`0x58`, `0x88`, `0xB8`, `0xE8` (`0x2ACA90..0x2ACAA8`).
- ScreenText `0x2E10DC` reads object+`0x48`. The negative branch uses NEG.S at `0x2E10FC = 46000D07`; the nonnegative branch reads video+`0x60` at `0x2E1108`, converts at `0x2E110C`, and subtracts at `0x2E1110 = 46010501`. The following geometry loop copies this value to both sprite-corner Z lanes (`0x2E1178`, `0x2E11A8`) and their actual packet vectors. Its later rotation/scale operations affect XY, not this Z.

### Initial ScreenTexture construction is distinct

`z_float = F32(float(M) - initialDepth)`.

The constructor packet at `0x2DF398..0x2DF408` performs conversion and subtraction with no negative-depth branch. ScreenTexture's initial depth is float 1 at `0x2DEFA4 = AC830040`, from `0x2DEF64 = 3C033F80`. Do not incorrectly generalize the later assignment branch to construction.

### Main and icon cases

- Main sets depth 15 from `0x1E55BC = 3C024170`. Its inlined sign-aware assignment reads M at `0x1E55EC`, CVT.S.W at `0x1E55F0`, SUB.S at `0x1E55F4 = 46010001`, and writes all four corners at `0x1E5608..0x1E5620`. For M=0xFFFFFF, the result is exactly 0xFFFFF0.
- Icon's authored-negative default is a **preceding** operation at `0x2AC848..0x2AC880`: if authoredDepth<0, load integer M (`0x2AC86C`), integer-subtract 300 (`0x2AC870 = 2442FED4`), then CVT.S.W (`0x2AC87C`) to form assignedDepth. The sign-aware assignment above follows. For admitted M, this yields integer packet Z 300. Passing the original negative authoredDepth directly to the generic sign-aware helper would be wrong.

## 3. Actual integer conversion, statically identified

Two registrations select the same immutable DMA/VIF object:

- name `ScreenObjectRendererCode` at string VA `0x3A5620`: blob `0x327E90` passed via `0x15090C`, registration at `0x15091C/0x150920`.
- name `ScreenObject` at string VA `0x3A5808`: same blob via `0x150BCC`, registration at `0x150BDC/0x150BE0`.

The blob starts `3000006D 00327EB0 00000000 00000000`; its following tag is `70000000`. VIF prefix at `0x327EB0` is `00000000 4AD900B0`: MPG upload of 0xD9 instruction pairs at VU instruction address 0xB0. Instruction-pair bytes begin at ELF VA `0x327EB8` and end before `0x328580`. This is solely static packet/instruction provenance.

The object selector is texture float 1 (packet+0x38 at `0x2DF378`) or text float 6 (constant `40C00000` at `0x2E1030`, stored at packet parameter+8 by `0x2E1084`). The blob converts this selector and dispatches: static lower branch `0x327FB0 = 50074046` takes selector1 to `0x3281E8`; `0x327FA0 = 5007507D` takes selector6 to `0x328390`.

The actual texture branch loads XYZ, changes only XY, then converts its four positions:

| Upper-word storage VA | Exact upper word | Static instruction |
| --- | --- | --- |
| 0x32829C | 0183197D | FTOI4.xy vf3,vf3 |
| 0x3282A4 | 0063197C | FTOI0.zw vf3,vf3 |
| 0x3282AC | 0184217D | FTOI4.xy vf4,vf4 |
| 0x3282B4 | 0064217C | FTOI0.zw vf4,vf4 |
| 0x3282BC | 0181097D | FTOI4.xy vf1,vf1 |
| 0x3282C4 | 0061097C | FTOI0.zw vf1,vf1 |
| 0x3282CC | 0186317D | FTOI4.xy vf6,vf6 |
| 0x3282D4 | 0066317C | FTOI0.zw vf6,vf6 |

The text sprite branch similarly uses upper words `0x3283F4=0183197D`, `0x3283FC=0063197C`, `0x328404=0186317D`, `0x32840C=0066317C`. For bounded finite Z in [0,0xFFFFFF], `integerPacketZ = trunc_toward_zero(z_float)`. Z is not multiplied by 16; that is XY's FTOI4. For those bounds a uint32 native cast matches the conversion.

Encoding cross-check: already-present `third_party/pcsx2-mmi47k/R5900OpcodeTables.cpp` lines 668,698..705 gives SPECIAL2 index `(word & 3) | ((word >> 4) & 0x7C)` and FTOI0/FTOI4 entries. [PCSX2's primary VU source](https://github.com/PCSX2/pcsx2/blob/master/pcsx2/VUops.cpp#L807-L823), read only, corroborates truncating conversion and saturation outside the admitted range. No emulator was executed or added to the native candidate.

## 4. Submission versus actual pass order

- Main `0x1E6410` enqueues backgrounds in ordinal0..3 through `0x1E644C`, then right arrow `0x1E65E4`, then left arrow `0x1E6680`, then calls base page `0x1D7550` at `0x1E6688`.
- Base page subsequently handles its extra cover geometry/conditional icons and child rendering (`0x1D798C`, `0x1D79B0`, `0x1D79BC`). This research does not freeze actual visible child/descendant order from those generic calls.
- Texture draw `0x2DF874` calls `0x246760`, which selects the material and calls `0x24A620`.
- `0x24A728..0x24A7CC` walks material passes ascending, checks the enabled bit (`0x24A72C..0x24A738`), resolves animation at `0x24A740`, and looks up that pass's registered group index at material+`0x5C+4*pass` (`0x24A790..0x24A7B0`) before enqueueing through `0x24B9B0`.
- Frame submission calls manager drain `0x24B7B0` at `0x114E38`.

### Registration, merging and pass barriers

`0x29BED0` registers passes ascending through `0x24B7A0 -> 0x29C490`; `0x29BF0C` stores each returned index in material+`0x5C+4*pass`. For a later pass, `0x29BF58..0x29BF74` writes its index into the preceding group's +`0x1C` limit. Group constructor `0x29C0EC/0x29C0F0` starts that limit at 0x7FFFFFFF.

Registration increments manager group count at `0x29C51C..0x29C524`, stores the new shader in the new group's +4, then compares earlier shaders. If equal and `newIndex < oldGroup.limit`, `0x29C604..0x29C644` writes a redirect to the new group in oldGroup+`0x18`. This bounds shader merging by pass-order barriers.

Shader comparison dispatch uses vtable `0x3D2110` slot+0x1C = `0x249BA0`. It checks microprogram, texture identities, flags, generated state-packet size and bytes (`0x249C58..0x249C8C`), animation-related exclusions, and ShaderPassGroup equality (`0x249CF4..0x249D18`). `unique` rejects equivalence. ShaderPassGroup parsing at `0x247530` maps `none` to -1, `unique` to -2, or uses the named registry owner global `0x382FC0` (`0x24759C..0x2475AC`); the result is stored at shader+`0x22` by `0x2474F8`.

Queue insertion `0x29C150` follows redirects, stamps the current frame generation, and appends the new node at tail. For an existing leaf queue, `0x29C248..0x29C250` stores `oldTail.next=newNode`, then `tail=newNode`. Manager drain iterates registered indices ascending (`0x24B8CC..0x24B93C`) and consumes only this generation (`0x24B900..0x24B910`), with optional pre/post actions. The draw loop follows next pointers at `0x24B530..0x24B534`, preserving FIFO within the resolved leaf.

The current concrete registry, loading order, group redirects, per-pass enabled states and animation states are not reconstructed here. A final native span list cannot invent them from filenames or PSM pass order alone.

## 5. Framebuffer alpha remains an explicit blocker

The source supports framebuffer-alpha-consuming states; their actual use on a complete Main path remains unproven:

- `destinationAlphaTestOnOff`/mode are read at `0x247E88`/`0x247EA0`, packed into TEST bits14/15 at `0x247EFC`/`0x247F14`, and written as TEST_1 register0x47 at `0x247F24..0x247F34`.
- `inputAlphaC` is read at `0x247A64`, then packed into ALPHA's C selector at `0x247AF0`, emitted to register0x42 (`0x247AE8..0x247B10`). C=1 selects framebuffer alpha under the GS contract.
- Pass-group draining can run pre/post actions and change render targets/state; raw material resolution also contains RenderTarget handling. The existing native material adapter excludes immediate DATE and destination-alpha blend-factor use, but this does not prove that later source groups or render-target sampling never consume earlier alpha.

No actual UI material binary was decompressed/decoded during this research, and no complete downstream target lifecycle was established. Therefore `opaquePresentationAlphaOnly=true` is **not** justified by the depth findings or by a single admitted span. Do not set it based on this packet.

## 6. Native candidate and remaining limits

`original_ui_packet_depth_candidate.h` contains only pure native bounded formulas and explicit authenticated video-owner admission. It does not load bytes, run an interpreter, sort/group draws, activate UI, or claim alpha ownership. An opaque identity supplied by a caller is a reference to an already authenticated owner, not a mechanism that authenticates arbitrary values.

The candidate handles exact integral subtraction for text, assigned texture depth, initial texture depth and icon's authored-negative special case. It rejects nonintegral subtraction operands until the broader source arithmetic/rounding owner is established; conversion of an already source-owned finite float Z is separately exposed. Negative sign-only depth conversion does not need subtraction rounding. This prevents a new guessed FPU-rounding policy from entering an otherwise exact constant-depth path.

Open prerequisites: actual immutable video lifecycle binding and invalidation; dynamic source depth/transform updates; final registered-group topology and ordering; enable/animation state; source depth-clear/retained-backdrop ownership; and absence or native preservation of downstream framebuffer-alpha consumers. No claim of visual equivalence, successful compilation, tests or runtime readiness is made.
