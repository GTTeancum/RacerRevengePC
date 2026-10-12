# Original GC local fog: source-only closure

Date: 2026-10-12. Scope: original restored GC assets and bounded static executable inspection. No repository files changed; no build, test, syntax check, compiler probe, project script, game, guest instruction execution or emulation was run. All new artifacts are outside the repository. This is not runtime validation or a rendered-match claim.

## Result

The three authored fog volumes are genuine missing local scene content. Their full dependency chain is GC scene -> shared particle manager -> demoprt atlas/material -> fog.emt group 0 -> persistent FogParticleEmitter particles. They are translucent gray smoke tiles lit by the actual scene, not a green overlay. Restoring them cannot by itself be presented as restoring the whole-level green appearance.

The core native implementation is source-supported, but it needs a concrete world-particle owner and its genuine camera/light/material joins. Do not quietly broaden the authenticated Main/Options wrappers or substitute a global tint. Important differences from ordinary smoke/fire: populate once, persistent age, camera-proximity squared-distance fade, phase-driven incremental motion, shared RNG consumption including a one-time clock-derived reseed, fixed authored placements, and actual scene-wide membership rather than a player-zone gate.

## 1. Authenticated input bytes

Root source directory:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/gc-intake/intake/original-members/`

All inspected member hashes independently match the retained GC archive inventory. The original GC.RES identity is SHA256 `69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9`. See `evidence/authored-bindings.json` for every member's size, logical archive offset, hash, and compiled field offsets.

- `tracks/gc.tob`, SHA256 `e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454`, 9,623 bytes, logical archive offset 0.
- `tracks/gc.scb`, SHA256 `d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d`, 359,830 bytes, logical offset 73,728.
- `particle/fog.emt`, SHA256 `3ca0cc75636e0f357e6ed1a8ada590bdc51862fef90cd30a0a3d5085e152e17c`, 1,298 bytes, logical offset 18,530,304.
- `particle/demoprt.prt`, SHA256 `d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f`, logical offset 860,160.
- `particle/demoprt.pst`, SHA256 `3c454a0bd95c12cd75168ec3da3c4ced43e60e5a1d1c4536ec65583d0350a165`, logical offset 884,736.
- `particle/demoprt.psm`, SHA256 `ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1`, logical offset 28,311,552.

The compiled TOB independently agrees with the text TOY. ParticleEmitter2/3/4 blocks start at byte offsets 3,080 / 3,383 / 3,685. Their position value offsets are 3,348 / 3,651 / 3,953:

1. `(12659.9, 1379.3, 9063.10)`
2. `(12693.9, 1279.3, 8119.4)`
3. `(12537.6, 1512.5, 10059.6)`

Each has TypeID 12, StartingZone 10, StartOn T, one instance, fog.emt, and no AttachToSoultree. The compiled emitter count is 4 at byte 4,035. Emitter1 is six attached generator fires; it is not another fog volume.

SCB Environment.ParticleTextureMapName is `demoprt.tga` at byte 27,614. Fog RGB is `151,160,177` at 27,716; visibility 0.5 at 27,728. Ambient RGB `80,80,90` is at 27,930; directional RGB `170,170,160` at 28,428. Source-light position/target strings are at 28,440 / 28,467. Text TOY/SCN hashes and all selected fields are retained too.

Original executable:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`
SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Every code address below is an ELF virtual address. File offset = VA - 0x100000 + 0x80. The original load segment was read using readelf; selected words and vtables are preserved under `evidence/`. The small independent annotation helper leaves unfamiliar words unannotated and never runs them.

## 2. Texture and group ownership

- Scene Environment reader 2771DC..2771F8 stores ParticleTextureMapName at scene-definition +0x41C.
- Actual PodScene override 1B4DC0..1B4E74 creates scene +0x4A0 and passes that name into ParticleManager160E00 with capacity **5,000**. Generic Scene273FA0 uses 1,000, so copying that generic capacity would be wrong here. Manager160E44..160E78 creates 2,500 depth buckets from the actual capacity.
- ParticleManager160F60..160F68 calls 1617A0. That routine replaces the extension with `.prt` (string at 3A7318) and reads the group/frame descriptions. Manager160F90..161008 strips the atlas extension for the material name, and passes actual startup DefaultMT owner global3833E8 into the material-reference creation.
- Fog emitter parser1602B0 reads its ranges and fog-specific values, then calls base15AEA0. Base15AEE0 reads ParticleGroup into emitter +0x1B8. Fog160414..160448 calls group lookup161EA0, which indexes the selected **group number**, returning first/last/current frames.
- `fog.emt` selects group 0. `demoprt.prt` group 0 has TypeID 0 (smoke), first frame 0, last frame 15. This is intentional: the emitter is TypeID 12 while the atlas group is TypeID 0. Do not copy the Main fire check requiring emitter type == group type.
- demoprt.psm is one ParticleCode pass with two alternate material states over the same demoprt image. Nonadditive fog selects state 0, not two successive draws. State 0 explicitly gives (Cs-Cd)*As/128+Cd; state 1 is additive. Both author alpha blending on and depth writes off. Blank fields inherit the real DefaultMT, not ScreenMT or arbitrary GL defaults.

The independent texture audit is at:
`/workspace/shared/gc-fog-atlas-byte-audit-20261012/REPORT.md`
and `evidence.json` beside it. The 128x1024 single-mip PST is directly supported by existing `decodeTexture48E`. Group 0 is a 4x4 arrangement of sixteen 16x16 cells occupying x[0,64), y[448,512). Palette lookup swaps index bits 3 and 4. Across its 4,096 texels, 3,124 are exactly gray; all others satisfy R>=G>=B, with at most 5 bytes R-B separation. Used alpha ranges 1..183. The whole atlas contains alpha 255 elsewhere, so retaining the full image keeps that conservative maximum in a shared bank.

## 3. Actual emitter construction and persistent state

- Descriptor reader27C600 consumes StartingZone, TypeID, optional attachment, Position, EmitterFilename, and StartOn. With no attachment, 27C678..27C6A4 reads Position and forces one instance.
- Type 12 allocation branch27C898..27CB90 allocates a 0x240-byte FogParticleEmitter and calls ctor15FE30. Its vtable is 3CD930; update +0x38 =160160, parameter reader +0x94 =1602B0, stop +0x84 =160490. Type-name record resolves to FogParticleEmitter.
- Dispatch27D7B4..27D7C4 selects12 ->27DBE4, which calls15FF10 with scene +0x4A0, filename, absent attachment, and authored world position. 15FFC0..15FFC8 sets current and previous positions. The EMT reader15AE00 invokes the fog-specific virtual reader. It therefore populates at creation, before StartOn's start/enable calls27DC0C..27DC28.
- 1602DC..1603A8 reads MinRange `(-300,0,-300)`, MaxRange `(300,110,300)`, DistanceToFade 200, ParticlesInVolume 80, OscillationSpeed 10, OscillationMagnitude 0.02. It stores distance squared 40,000 at +0x220 and its reciprocal at +0x224; count +0x228; phase period +0x22C; magnitude +0x234.
- Base data reads ParticleLife100, InitialSize80, Growth0, EmitRate0, AgeAtBirth0.6. At15AFE0..15B008 it multiplies life by birth fraction; each fog particle starts with age approximately60ms and life100ms. These remain binary32 values; do not replace arithmetic with exact decimal constants.
- Properties produce no additive/fades/velocity/growth/gravity/glow flags and Lit=T. Fog1603F8..160410 adds persistent flag0x80 and clears growth. FramesPerSet0 leaves animation flag0x400 clear. Base reader computes an unused reciprocal for that zero field; a native no-animation representation need not manufacture or consume that infinity.
- 160460 calls population160520 once. It first marks any previous particles owned by this same emitter nonpersistent and sets their age to life, then appends up to80 particles subject to the shared manager capacity. Initial loading has no prior particles for this owner. Three volumes normally contribute240 persistent particles, sharing the manager with fires and other race effects.
- ParticleManager1613B8..1613C4 skips age advance/expiration when0x80 is present. EmitRate0 is not a reason to skip these volumes. The base stop method160490 changes emitter start bits; it does not prove removal of existing persistent fog. Do not invent StartOff/despawn semantics beyond the actual authored start-on case.

## 4. Exact initialization RNG chronology

Shared source RNG308030 is a 32-bit LCG: state = state*0x41C64E6D+0x3039 (mod2^32); returned value is state &0x7fffffff. 308020 writes that same shared state at libc owner+0x58. This is a process/shared RNG dependency, not a per-volume random generator.

For each successfully allocated particle with oscillation enabled, population160520 consumes draws in this order:

1. Draw1 at1605DC, draw2 at1605F8, draw3 at160614. Scale each returned value by binary32 2^-31. Map them into authored X/Y/Z ranges and add the emitter's world position at160658..1606FC. Unlike the fire path, **all three position axes** use the authored ranges.
2. Draw4 at160714, draw5 at160738, draw6 at16075C. Their magnitudes are multiplied by0.02. The resulting vector assignment is X=draw6 magnitude, Y=draw5 magnitude, Z=draw4 magnitude at160784..16079C.
3. Draw7 at160798, draw8 at1607E8, draw9 at160838 independently negate X/Y/Z when greater than0.5. The source vector is stored in particle +0x10..+0x18.
4. Store size80, initial size80, owner pointer, life/birth age, and **the current emitter frame** at160894..1608F0. Initially this frame is group.first =0. Mirror-X and mirror-Y bits are derived from the original positional draws1/2, not extra draws (1608A8..160984).
5. Increment manager population at160988..160994.
6. Before choosing the *next* particle's frame,160998..1609E0 lazily initializes shared static3D6B30. If it is zero,101A50 returns a clock-derived integer, stores it there, and308020 seeds the shared RNG. This happens after the first nine draws, not before the first particle. If the stored result remains zero, the same conditional path can recur.101A50 is an original timer-derived clock routine, not an asset-provided seed; its source is recorded, not executed.
7. Draw10 at1609F4 is scaled to the inclusive frame span `(last-first+1)` and truncated, then added to first. At160AB0 it becomes emitter.currentFrame for the **next** particle. The last particle still consumes this draw. No one-shot fire serial is incremented in this fog population path.

Thus normal fresh creation uses800 RNG draws per80-particle volume,2,400 across all three, with at most the lazy initialization reseed(s) described above. Capacity failure exits before that particle's first draw. The integer-to-binary32 conversion can round the largest RNG results to2^31, so the normalized value can equal1.0. The source frame-selection assertion catches a resulting out-of-range next frame; a native admission must not quietly replace the source mapping with modulo or a different distribution. The first particle of each independently parsed volume starts at frame0; subsequent frames use the predecessor's draw10. Initial global RNG state, clock return and earlier scene-side draws are not recoverable from these assets alone. A deterministic native capture must identify its real session RNG/clock inputs and mark any compatibility seed policy honestly. Do not isolate the fog RNG silently or reseed on zone entry.

## 5. Motion and proximity alpha

Fog update160160 precedes base15A7A0. It advances or subtracts dt from phase accumulator +0x230; ratio +0x238 is accumulator/period. On crossing positive/negative limits, direction reverses and accumulator is placed just inside the endpoint by period/1000. This is a triangular phase, not a sine wave. The published ratio is computed before endpoint correction.

Manager161544..1615B4, when0x80 and period!=-1, adds `phaseRatio * storedOscillationVector` to current position. There is no additional dt multiplication in this manager branch and no recomputation from a permanent birth position. Replacing it with sine displacement or dt-scaled velocity changes the source motion. The fog subclass has no early pause check; its base checks pause afterward. Manager1611A4..1611C0 separately returns early on the shared pause flag. Preserve actual caller scheduling/gates rather than assuming the phase helper and manager have identical pause behavior.

Draw1624E8 starts with nonglow brightness64. Fog branch162508..1625A0 computes:

`birthFade = 1 - particle.age * particle.inverseLife`

`distanceSquared = sum((particle.position - activeCamera.position)^2)`

`proximityFade = distanceSquared / 40000 when distanceSquared < 40000, otherwise 1`

`brightness = 64 * birthFade * proximityFade`

The camera is `*global442F70`, position fields +0x180/+0x184/+0x188. The authored birth fade is approximately0.4, so maximum authored brightness is approximately25.6, later rounded to source alpha byte26 by ParticleCode. Distance>=200 means **full authored fade factor**, not full opacity: the particles stay translucent. They become more transparent as the camera approaches them; the 200 value is not a far-distance culling radius.

With the used atlas maximum183 and alpha byte26, source blend coefficient is at most4758/16384 (about0.2904). A conservative full-atlas maximum255 still gives6630/16384 (about0.4047), below1. No raw source alpha should be rescaled or clipped. The existing native texture×vertex-alpha admission is therefore sufficient at this bounded authored brightness.

The source mirror-UV branch1626E0..1627A8 only enters when mirror-X is set; Y-only flags bypass mirroring. Copying two independent generic flips would differ.

## 6. Scene membership, camera, ordering and color

StartingZone10 does not establish a runtime zone gate. The unattached branch27D700..27D738 adds each fog entity to scene+0x3D8 through141960(mode0x402). It does not read descriptor.StartingZone, which is discarded with the descriptor at27D754..27D760. The emitter list is instantiated by2722F0, then iterated in authored order by27DFD0. The six generator-fire instances initialize before the three fog volumes because emitter1 precedes2..4.

Gameplay AttachScene invokes1B48A0 at1AADE4. ParticleManager scene+0x4A0 is added to the race root at1B4AE0(mode-1). Later gameplay1AB150 invokes1B4CA0, which adds the emitter-list scene+0x3D8 at1B4D24(mode0x402). The same shared manager is also passed to race effects at1AA57C. This ordering means the manager update precedes the emitter-list phase update in the attached child order, assuming the original tail-append traversal. Preserve that join rather than running all fog phases arbitrarily first.

Manager prepare162870 has no test for StartingZone/current player zone or a per-fog-zone visibility handle. It iterates all live particles, uses the current camera source-screen matrix at +0x250, depth-buckets them at16299C..162AC8, then computes the four lit corner colors. It uses the source max-depth constant16,777,000 and2,500 buckets with quantized depth; buckets are drained in descending order, with same-bucket LIFO insertion. Final ParticleCode additionally performs whole-center clip rejection. A conservative native visibility optimization would need to prove equivalent bounds; no playerZone==10 check or zone-entry repopulation is supported here.

Fog/light state belongs to the active render phase. Scene fog/reset/proxy are attached before the particle manager. The proxy restores the scene-specific fog/camera range after the reset; the existing track fog source-closure report establishes GC range start1301.324951171875/end2666.64990234375 and source fog upload scale255/(start-end). Particles consume the actual later GC scene fog, not a separate inferred green color. The current native Euclidean world-fog approximation is not automatically a substitute for this particle depth/fog contract.

Lit=T clears the unlit bit0x800. Source16105C..16115C initializes four normalized corner normals from(-1,+1,.25),(-1,-1,.25),(+1,+1,.25),(+1,-1,.25).162AEC..162CD4 builds the camera-facing basis and calls lighting149310 for those normals.1493CC..149464 selects active directional light data;149494..1495EC uses the scalar normal/light dot contribution plus ambient, scales by255/128, truncates packed RGB and162CD8..162D18 clears alpha. The source scene light constructor divides authored RGB by2 before registration, as documented by the existing track-lighting/Main-particle reports.

The actual GC nonzero directional light means the Main helper's constant(49,49,49) shortcut is invalid. The arithmetic applies the same directional scalar to each RGB channel, and both authored GC ambient and directional inputs have R=G. Their lit R/G outputs therefore remain equal; the source textures themselves are gray/slightly warm. These facts do not support a distinct green particle pigment. Compositing over original green terrain can naturally leave green apparent in the final view.

## 7. Smallest justified native implementation and remaining gates

A narrowly admitted GC local-fog owner is feasible. Recommended scope:

1. Cold-load/authenticate actual TOB emitter2..4, fog.emt, demoprt.prt/psm/pst and real DefaultMT. Reuse `src/assets/import_world48f.cpp`'s parameter document for compiled fields and `src/assets/import_texture48e.cpp`'s existing decoder. No alternate texture, recolor or new decoder is needed.
2. Introduce one typed persistent world-fog owner holding three emitter states and up to240 particle records, with explicit admission to the **actual shared5000-particle manager** and shared RNG/session-clock chronology. If initially isolated for engineering reasons, make that limitation explicit and do not claim full source scheduling/RNG parity. Populate during scene initialization, not from frame-time EmitRate.
3. Implement the bounded scalar source equations above natively, including phase order, current-position increments, camera-distance brightness, fixed initial age, frame initialization, source mirror branch, and no age expiry. Use ordinary CPU arithmetic; no instruction interpreter, guest state machine or PS2/VU execution.
4. Reuse the existing generic numeric `prepareOriginalUiParticleCameraFromView50KG` boundary with the gameplay camera and original viewport, not the Main owner. Factor the low-level `projectOriginalUiMainParticleQuad50KG` math into a genuinely shared helper with caller-owned admitted frame ranges; its present52..75 guard is intentionally Main-only. GC needs0..15. Keep source clip, quantized depth, alpha rounding and particle fog weights.
5. Add the general source four-normal/camera-facing lighting calculation for the real GC light owner. Existing `original_ui_main_particle_lighting50kg.cpp` deliberately rejects nonzero directional color; do not relax it into a guessed RGB path. The relevant complete word ranges are preserved, but their full native camera-basis transform and active-light lifecycle join still deserve independent review before implementation.
6. Factor material-state preparation below the existing authenticated Main/Options wrapper, then admit the real GC owner and128x1024 atlas. The existing ParticleCode GPU mode/alpha test/fog/source-alpha blend policy is reusable only after a gameplay depth/target owner and actual material-schedule join exist. Submit at the particle manager's source material-tree point, preserving interleaving with generator fires/other effects and selected nonadditive runs. Do not render a full-screen layer or add fog to all world geometry.

Outstanding integration gates: concrete gameplay shared-RNG/clock input and initialization order; general nonzero-directional corner lighting; actual gameplay camera phase and viewport; shared particle budget/ordering with fires and vehicle effects; source material queue/target/depth join; runtime validation reserved to the root's central slot. The available source is sufficient to design these bounded native parts, not to certify a small one-line graphics fix.

No reference image was requested again. No whole-level visual equivalence or green-hue fix is claimed.
