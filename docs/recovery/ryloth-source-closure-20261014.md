# RA / Brightlands closure list — source/data inspection only

This is cloud-worker source analysis, not a verified Windows Codex app session or a runtime-readiness result. No importer, preparation script, build, test, syntax check, game, guest instruction, or validation run was executed. Independent metadata scripts only decompressed exact original RA.RES and read headers/labelled fields; no pixels, meshes, world transforms or runtime caches were generated.

## Evidence and scope

Original RA.RES: 4604561 bytes, SHA-256 1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2, 1010 members. Read only from the authorized original disc/TRACKS directory. Companion RA-readiness-source-inventory.json retains all member identities, all 50 PSM field maps, all PST/ATM header records and the actual SCB/TOB/AUB labelled records.

Original tracks/ra.scb: 540795 bytes, SHA-256 57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81. Original tracks/ra.tob: 95558 bytes, SHA-256 7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524.

Compared against local importer after c656d401be5ef856066c7f110f50f0ab69351a8c and the centrally retained tools/assets/assemble_ryloth_runtime.py, SHA-256 be97e8ec99a34dec9d988ec112df7d9ef6cf7d0601b7f47ebc9b1eb8a7158f5d at inspection. I authored the RA admission/importer integration; the Ryloth assembler belongs to another worker. No shared assembler/world/renderer source was edited.

## Supported as written: bounded source paths, not executed acceptance

- Geometry: all 710 PSGs are single-node and all are referenced by the static SCB. The exact RA admission enables the existing signed24-in-V4_32 route for 583 pinned PSGs with 39 pinned material dependencies. The remaining 127 use V4_16. The existing one-node 00ff0107 path covers loft01, loft02 and smokenull01 without fabricated vertex colors. Topology/bounds/serialization guards still execute only when root runs conversion.
- Static world: 10 zones, 10 portals, 1261 model instances; 1085 have PSGs, 176 have no PSG but do have COL; zero missing both. 178 instances have COL, so two also have PSG. All 1261 author useLighting=F. Existing assembly demands conversion of every present PSG and reports genuinely missing PSGs as nondrawn; retaining the 176 collision-only references is correct and must not generate scenery.
- Static material families: 44 PSMs appear in static scene material tables: 43 PrelitCompressed (case-preserved spellings) and black/DiffuseCompressed. Forty have an exact local kind1 PST; one has crowd.atm; three have exact TGAs. No material-name fabrication is necessary. The existing material parser retains numbered pass fields, but parsing a program name is not evidence of renderer equivalence.
- Texture bytes: all 44 original PST members are version1/kind1. Forty serve the static PST-backed families; the other four are finishline, sun_ra, flare1 and particle/demoprt. Native48E already has the kind1 indexed-data route. The importer exports the base rrtex; the current assembler separately processes original PST mips and compares the base to native output. No mip contract was changed here.
- Animation byte formats: crowd.atm contains 6 kind2/IPUM frames, each with 16,32,64,128,256 mip dimensions (214578 bytes, SHA-256 76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031). fholobeam.atm contains 16 kind0 frames, each one 32x32 mip (9676 bytes, SHA-256 3013cb65a05f43857af0144e0d9f6e04b7e26173f94e363b5eacfb967b6ccb9f). The assembler's source parsers have these format routes. Frame selection is separately blocked below.

## Blocked on original-data semantics or missing bounded admission

1. Three required static textures are TGA, not absent originals. Current assembler lines 333-365 try PST then require ATM, so these cannot close as written:

| Authored material | Exact texture | Original TGA header | Bytes | SHA-256 | Static instance material-table references |
| --- | --- | --- | ---: | --- | ---: |
| largeflag | textures/LargeFlag.tga | type2, 128x128, 32bpp, descriptor8 | 65580 | 352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d | 6 |
| metalgrate | textures/MetalGrate.tga | type2, 256x256, 24bpp, descriptor0 | 196652 | 20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9 | 13 |
| twidesigns02 | textures/TwiDesigns02.tga | type2, 128x128, 24bpp, descriptor0 | 49196 | 693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15 | 12 |

Existing original_standard_texture_resource50la.cpp:240-251 admits only the pinned 64x64 24bpp specmap5 shape (12332 bytes); its existence does not establish admission for these RA shapes. Recover/admit original 24/32-bit channel, alpha, orientation, quantization and mip semantics explicitly before adding a TGA assembly branch. Do not route these through PST or another track's texture. Four additional debug TGAs are 16bpp and have no static material user in this inventory; preserve them without claiming scene use.

2. Both ATM users omit animatedMapInitialFrame. crowd.psm authors TextureAnimation=unique, FPS10, reverse-loop0 and loop-to-frame0; finishline.psm pass2 authors unique, FPS16, reverse-loop0 and loop-to-frame0. The assembler's animation_bindings at lines140-143 requires an explicitly authored initial frame, so both fail that condition. Crowd has 346 static instance material-table references. The finishline PSM is two-pass (diffuse then Diffuse with fholobeam), has no static SCB material-table user, but the assembler's all-ATM closure still requests it. No initial frame was added to the PSM or guessed in the importer. A separate rider owner references a default0 at original address0x29b950; root should establish the shared original loader semantics for RA before using that as the assembly default.

3. Special program families must not be mislabeled as native draw support. Total archive material passes: PreLitCompressed41 + PrelitCompressed2 + DiffuseCompressed1 + diffuse1 + Diffuse1 + ScreenObject2 + ParticleCode1 + StripOptFogCode1 + Prelit1 = 51 passes in 50 PSMs. The six non-static PSMs are finishline, flarezretrieve, sun_ra, particle/demoprt, flare1 and gfxtest. flarezretrieve and gfxtest author totalTextureMaps=0, outside native48E's current 1..2 map count; preserve their named unsupported result, do not invent a texture. ParticleCode has two demoprt map slots; its actual PST is particle/demoprt.pst, not textures/demoprt.pst. The inventory's per-pass pst_present/atm_present fields deliberately describe the textures/ lookup only; the complete member identity list records the particle-local source. Resolve its original namespace rather than report the asset absent. Finishline's two passes must stay distinct. The assembler's generic ProjectedPrelit limitation is not an RA family: RA has no such program; do not use that text to characterize RA.

4. World-instance types beyond static mesh placement remain semantic binding work, not evidence of readiness merely because SCB/TOB fields are retained:

- SCB contains 185 light records: 4 Ambient, 1 Directional, 15 ObjectPoint and 165 Spotlight. It contains authored negative light colors, zone lighting/fog and sky visibility selection. Static useLighting=F does not erase lights needed by other original owners.
- TOB has 673 static-model parameter records, all with slt and allowshadowreceiving fields. They are authored parameters, not 673 additional placements. Respect the original SLT association.
- Four typeid8 particle emitters attach raFire.emt to FireP0t01..04.slt with offset0,15,0 and starting zone1. Particle/demoprt.prt, raFire.emt, demoprt.psm and demoprt.pst are authentic dependencies. No equivalent live binding was established by this work.
- One sunflare references sun_ra.tga by authored name while the archive provides sun_ra.pst. Its per-zone saturation fields and target/position are retained; follow original texture resolution semantics, not literal-only or guessed replacement.
- Two triggers rightSkull/leftskull author LostAI actions targeting node29/node104 and ignore PLAYER. The four trigger-prefixed sections include two action records; they are not four trigger instances.
- Eight terrainzone records plus nine zonecollisionobject mappings carry friction/traction/drag/dust/dirt metadata. The SCB references nat13.trn, absent from RA.RES; original archive/global-source resolution must be traced within authorized disc sources, not fabricated.
- Seven TOB cscamera records, seven tracks/ra01..07.vub blobs and raTVCam.pos are present. These are source data only; none was executed or interpreted as host instructions. SCB itself authors numberofcameras=0, so conflating SCB and TOB counts is incorrect.
- One sky cube, two SPL paths, AUB/audio INIs and RA tuning are present. The static scene assembler/geometry admission alone cannot establish their runtime ownership.

## Blocked only on central conversion/runtime validation for the already admitted slice

Root must exercise the exact archive authentication, 710 geometry conversions, preserved unsupported material diagnostics, native kind1 PST conversion/mips, raw-source hashes and current RRSCENE/RRMESH/material contracts. The static world counts and one-node source shapes introduce no new geometry-format or hierarchy requirement in this inventory. This statement is limited to those already admitted representations: the TGA/ATM gates and non-static original owners above are implementation/semantic blockers, not mere unrun tests. Budget fit, native selected-track draws, alpha/lighting/animation timing, collision and gameplay behavior remain unmeasured.

## Proposed importer-only fix: stop silently omitting exact RA sources

The current extension whitelist emits entries for 710 PSG + 50 PSM + 44 PST + 1 AUB = 805 members. It silently skips the remaining 205: 178 COL, 7 TGA, 7 VUB, 2 ATM, 2 SPL, 2 INI, and one each TOB, SCB, PRT, EMT, CUB, POS, TUN.

The bounded patch bypasses that whitelist only when raCompressed is true, after existing exact RA authentication. Every original RA member then receives raw source_file, source_bytes and computed source_sha256. Existing native conversions are unchanged; unsupported PSM/PSG/PST retain their named errors. Other member kinds take the existing original_source_retained status, not converted_texture/geometry/material. MA and default RC/POD selection remains identical. No new decoder, texture interpretation, world mapping, serialization, renderer or guest execution is introduced.

This is retention/provenance closure, not conversion of the 205 members. Root's reviewed native-output-index generation must include the newly retained source files before invoking the assembler, which already checks every source_file against that index. Disk/output size increases by authentic member bytes only. This patch must be applied separately after the integrated importer patch; it changes only tools/assets/original_roster/import_original_pods.cpp.
