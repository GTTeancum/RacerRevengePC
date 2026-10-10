# Actual Main scene and native fire preparation

## Delivered source, with verification limits

Staged source only:

- `files/src/game/original_ui_main_scene50kg.{h,cpp}`: authenticated original SCN/TOY/PRT/EMT/PSM ownership, typed Main scene/fire data, and original indexed PST decoding.
- `files/src/game/original_ui_main_fire50kg.{h,cpp}`: native stationary Main fire emission, shared RNG consumption, lifetime/motion/frame update, depth bucket ordering, and semantic ParticleCode packet preparation.

No project parser/importer, compiler, syntax check, test, native application, game, guest instruction, or graphics submission was executed. Static source reading and independent Python standard-library asset/ELF inspection are the only verification performed. These files do not assert that the original menu is rendered or integrated. Root owns CMake, application integration, publication, and any future authorized checks.

## Actual authored scene

The exact authenticated `UI/UIMAIN/UIMAIN.RES` contains:

| Member | Bytes | SHA-256 |
|---|---:|---|
| `ui\UIMain\mm.scn` | 3442 | `147235426b8ac34cf4796dd2e4069efcc2401ef2533e6617791c15407275baa4` |
| `ui\UIMain\mm.toy` | 433 | `760c7458c3879f74a5f3d5e9d697f58bce1db906781ac2d2ff60f5267ddbe96b` |
| `particle\mmprt.prt` | 7450 | `83c24fd40241323b4c1e0bf0891be6c88b16f2f647cca5f65394949cd03db55d` |
| `particle\mmfire.emt` | 798 | `118faa7121198d8aac279f196dcf08f9e941c90b11c1ebac73ddcfb010cb4bd6` |
| `particle\mmprt.psm` | 5018 | `68cfa49633146ff0ad4ae09774a78b43f9573b2978dc6d300e7118b42c536171` |
| `particle\mmprt.pst` | 33820 | `a75199550efd67e725f1a1432498436439f0d825b7ecc2f586d25240fe88e84f` |

Archive SHA-256 is `b2019a9dcd541baeba90ef3e3b57480f9a023155290aec84a195ecdf1b990f84`. The independently decompressed members matched the closed archive catalog names, offsets, sizes, and hashes. The originals under `evidence/assets/` are research inputs, not clean-source deliverables: do not copy them into Git or publish them.

Main is an actual zero-static-model scene. `mm.toy` declares zero scene cameras and zero animations. It has exactly one emitter: starting zone1, TypeID8, position `(48.055893,11.260490,71.276047)`, filename `mmfire.emt`, StartOn=T. No geometry or animation needs to be invented. The manager supplies the borrowed camera separately.

The fire references group4, TypeID8, frame range52..75, three eight-frame sets. The actual PST is revision1/kind1, one128x256 indexed level with256 raw palette colors. The PRT contains104 frames/eight groups, including inactive shared groups whose source UVs exceed the small Main atlas. The native owner retains all of them but validates texture bounds only for the real selected fire group. Never enlarge the image or clamp all104 frames to make a global bounds check pass.

SCN fog authors RGB70,10,10, Visibility1, Haziness0; lights are ambient RGB50,50,50 and a zero-RGB directional light with retained source position/target. The whole SCN remains accessible through `sceneAuthoring()` for the separate light/lens-flare/environment owners. The typed subset does not silently certify those effects.

## Preparation APIs

`OriginalUiMainSceneOwner50KG::prepare(mainArchive, diagnostic)` is cold and load-once. It uses the already authenticated `OriginalUiArchive50KG`, the existing source-correct TextConfig-derived text reader, and `decodeTexture48E`. Its output owns copied text and decoded pixels; no archive-byte lifetime is borrowed. `definition()`, `particleTexture()`, `particleMaterial()`, and the original authoring accessors expose immutable records.

The particle material is one shader pass with two texture-map/state slots, both referencing mmprt. It is **not** two arbitrary full-screen layers. `ParticleCode` runtime packet runs choose additive versus ordinary state. Raw material blank values remain blank rather than inheriting ScreenObject assumptions. The separate native ParticleCode material/GPU worker owns this join.

`OriginalUiMainFireOwner50KG::initialize(scene)` creates bounded native storage for the actual stationary, unattached Main emitter. Runtime methods are separate because original manager subscriber ordering is an integration concern:

1. `advance(elapsedSeconds, paused)`: the ParticleManager161180 scalar aging/motion/frame update.
2. `emit(elapsedSeconds, paused, sharedRawRandom, sharedFireSerial)`: FireEmitter15E0D0, preceded by its relevant base15A7A0 gates.
3. `preparePackets(view)`: source162870 depth bucket order and162230 packet semantics.

That list identifies interfaces, not proof of a whole-manager schedule. The actual source attachment/traversal must establish the order before using them as one frame method. No method silently advances another phase. Any state change clears the previously prepared packet count. Both elapsed methods admit finite normal/zero dt in[0,1]seconds; this upper bound is a native arithmetic admission limit, not an original clamp or a claim about the manager clock.

The root application's one existing raw308030 RNG must be passed by reference, shared with other consumers. Do not seed it when entering Main. `sharedFireSerial` is original433970's process-wide increment counter, initialized0 only when constructing the corresponding fresh application owner. It is not a per-emitter seed or counter. These references must not alias.

## Exact recovered fire behavior

- `1B4DC0..1B4E74` is the actual Main scene particle-manager override. It passes capacity5000, not the generic scene1000. ParticleManager160E44 halves this to2500 depth buckets.
- `27C600` reads emitter TypeID/position/name/StartOn. For absent AttachToSoultree, the `NONE` branch sets instance count1 and reads Position; the authored NumberOfInstances also happens to be1. The TOY Additive key is retained metadata; the actual additive bit is read from EMT Properties.
- `27DA04..27DA70` constructs the fire, sets the stationary position twice (so previous==current), starts it, and enables the entity. Emitter constructor15A5B0 sets the first-update flag. Base15A7A0 clears it and returns early on the first unpaused update.
- `15E6D0` reads MinRange/MaxRange and default ApplySpreadToVelocity=T, delegates to base15AEA0, stores the EMT Y velocity40 as vertical spread scale, and resolves group4 to frames52..75.
- Base read constructs flags0x1405: additive, velocity, animated, and FadeNonLinear. Fades and Grows are false, Gravity absent defaultsfalse, Lit absent defaultstrue. Growth-6.5 is stored but does not enable growing.
- `15E224..15E25C` emission count is truncation of `0.6f * (EmitRate * elapsedSeconds)`, with separate binary32 operations. The defined >1 branch clears the old fractional accumulator rather than preserving its remainder.
- `15E2D0..15E338` checks capacity before incrementing433970 and consuming exactly three shared308030 draws. Full pools do not consume extra random values.
- `15E37C..15E3B4` maps the first and third draw into authored X/Z ranges. It does not map rangeY into position. The second draw is multiplied by authored Y velocity40. Stationary emitter velocity itself is zero.
- Third draw selects set0,1,2 at strict thresholds0.33 and0.66. First/second draw values strictly above0.5 control X/Y mirror flags. RNG values are the original31-bit result converted tobinary32 and multiplied by2^-31; no separate generator or integer modulo selection is used.
- Lifetime is `650 - 0.125 * (650 * (serial & 3))`:568.75,487.5,406.25,650, repeating in that original serial order. Moving-emitter-derived birth age is zero for this stationary Main owner.
- `1613C8..161434` adds1000*dt milliseconds and expires on `age > life`, not >=. Removal swaps the last active particle into the hole. The replacement receives motion/frame update but is not aged/rechecked in the same iteration. This original quirk is preserved.
- `161674..1616DC` selects the current eight-frame set using source truncations and adds `trunc(age/(life/8))-1` when the quotient's integer part ispositive. No conventional frame-per-second animation is substituted.

### Explicit native compatibility policy for the source's undefined branch

`15E0D0` saves s1 at entry but does not initialize it before the small-count accumulation path. `15E250` defines it on emission>1; `15E284` defines0 on accumulated==0; `15E29C` defines it on accumulated>1. The branch with0<accumulated<=1 reaches the emission loop without any source definition of s1. The base helper preserves s1; no source-owned persistent spawn count exists in the inspected fire fields. Emulating a caller's incidental register value would invent a meaningful game policy.

As specifically authorized by the parent task, only that undefined branch uses an explicit native fractional policy: emit the integral accumulated count and retain the fractional remainder. `nativeUndefinedCarryPolicy` in the receipt reports when this branch was used. Every defined original branch keeps its original count/reset behavior. This is a native reimplementation decision, not a bit-identical source claim, and makes ordinary small/variable host deltas usable without a fabricated fixed tick.

## Particle packet semantics

`OriginalUiMainParticleView50KG` accepts the actual source camera+250 third column and source camera+1D8 focal-pixel distance. A sibling extension `original_ui_main_particle_camera50kg` supplies `combinedSourceScreen250[2,6,10,14]` and `focalPixels1D8`. Do not pass conventional normalized clip-Z or eye distance into this interface.

- Depth bucket conversion retains both source truncations and the16777000 source depth scale. Buckets are traversed descending, with LIFO insertion inside equal buckets. This preserves ties and source particle pool order rather than substituting a generic Z-sort.
- Packet size is camera1D8*particleSize. The depth is the source's separately quantized depth value. The native ParticleCode expansion worker must consume these meanings, not treat them as direct window-space size/depth.
- `1625D4..162618` applies the non-linear brightness branch when Fades=F and FadeNonLinear=T. With fixed size7, brightness rises as64*age/life. This deliberately preserves the actual branch rather than using a conventional fadeout.
- Normalized UVs are authored coordinates divided by actual128x256 dimensions. There is no+1, half-texel offset, clamping, rescaling, or substitute image.
- `1626E0..1627A8` bypasses the entire mirror block when X mirror isclear. Therefore Y-only mirror has no effect; when X istrue, both requested axes are applied. This source asymmetry is preserved.
- Batches contain at most12 particles. Since every Main particle isadditive, the first particle per batch carries a negative run count; remaining particles carry0. The sign represents material context, not negative alpha or reversed primitive order.

## Scene owner/attachment evidence and exact remaining joins

Manager1CF3D4 allocates a1360-byte source scene and calls constructor1B2870 at1CF3E8. Manager+2B4 owns it. Loader1CF630 uses `ui\UIMain\`/`mm` for scene files, `ui\textures\`, `ui\geometry\`, and `ui\anim\`; the latter two do not authorize inventing absent Main meshes or animations.

Attach1CFAF0 calls1B48A0(scene,manager), rebinds the borrowed menu camera through114770(mode2) to scene+4C0[0], appends the camera to manager at1CFB48, then calls1B4CA0(scene,manager). ParticleManager scene+4A0 is appended in the first phase at1B4AE0 with mode-1. The emitter-list scene+3D8 is appended at1B4D24 with mode0x402. Runtime update/render traversal and visibility gates must preserve that actual attachment ordering; a packet list alone is not a scene scheduler.

Remaining integration joins, not falsely declared complete by this module:

1. Actual Main manager ownership, phase ordering, enable/pause/lifetime and render traversal, plus widget/page/scene interleaving.
2. Borrowed camera activation and particle-camera extension; sibling camera owner provides the numeric leaf.
3. Native `ParticleCode` quad expansion/material/GL submission. The original program registration at15065C names ParticleCode at3A5418 and points toprogram3212A0. The separate GPU worker has this exact anchor.
4. Four-corner lighting from149310, requested by162CD0, and actual source material context/depth/alpha handling. This module supplies exact packet brightness but does not fake RGB lighting.
5. SCN light/lens-flare/environment side effects beyond the represented Main fire, including whether authored zero-RGB directional lens-flare metadata produces a visible object. The source SCN is retained and accessible.

## Static evidence

Original ELF SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

The byte-display text under `evidence/` was produced with the sibling stdlib static decoder using `fileOffset = VA - 0x100000 + 0x80`; it does not execute, interpret, or emulate instructions. Unknown opcodes remain explicitly unknown. Scalar/COP1 branch meanings used above were read from instruction bits; no microprogram was run. The existing source conversion contract `docs/simulation-contract48b.md` states EE CVT.W.S truncation/saturation; every conversion here is bounded safely within signed32.

Use only `files/` plus this description/provenance for clean-source integration. Keep `evidence/assets/` and any original payloads private and out of source publication. No active repository file was changed by this worker.
