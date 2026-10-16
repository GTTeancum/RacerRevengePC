# RA logical/material audio source closure

Source-only, unactivated candidate. No decoder, conversion, playback, build, test or syntax check was run. Original ZIP extraction and raw byte hashing only. No UI/options or mesh-cache audit was inspected.

## Delivered files

`ra-audio-source.patch` appends RA to the existing material provider's enum/profile, preserving prior enum values and every existing track row. It also adds `tests/native/ra_audio_authoring50la.h`, an immutable owner of authenticated RA AUB and both original audio INIs. The closure borrows the actual retained RA archive owner; it retains42 distinct ambient parameter blocks rather than making synthetic Sound/stream objects. There is no terrain/sessionAssets/Course/startup change.

`raw/manifest.json` pins three originals extracted from authenticated RA.RES:

- tracks\\ra.aub:22511 bytes, offset98304, SHA256 c0977667b46b918872c3d351f745ce59260037b9f604b76aea23b11bb5c88c6a
- audio\\scrptLns.ini:98 bytes, offset29614080, SHA256 f7234be2254765e73546711e11d44c7b02828b17c023ef0aeacb75b9cd316bf7
- audio\\introAud.ini:238 bytes, offset29638656, SHA25672aa1f6ad04da68e6c7f345a05471e0d7b6cfcc8166dc18e9ed2fe8de6df76ce

RA.RES identity:4604561 bytes, SHA2561e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2.

## Exact measured material capacity/iteration

AUB Sounds declares12 collision rows,12 drive rows, NumMaterials1, DefaultMaterial1. All source sections1..12 exist in both sound sets. No failed-selector repetition is needed for RA. The current generic material parser already admits12/12; no parser or fixed-array expansion is necessary.

There are24 independent sound rows and21 unique PCM filenames: collision1..4 all use pod_coll; collision5..8 rockhit1..4; collision9..12 metlhit1..4. Drive1..4 podscrp1..4,5..8 rckscrp1..4,9..12 mtlscrp1..4. All paths are original sfx\\race names, extended with.vag by the existing parser.

All12 drive sections author sampleSpeed100,minVolume0,minSpeed1,maxSpeed60,resonantSpeed100,frequencySlope0.75. Distinct drive logical owners retain individual cachedSlope20=-1.

CRITICAL assignment behavior: only `collisionobjectsmaterial2` exists, declaring6 objects: FireCOL01,FireCOL02,FireCOL04,FireCOL03,BridgeWallCOL,BridgeCOL_floor. NumMaterials1 causes271674..271804 to visit material1 only; its absent list defaults0. Consequently the executed ordered assignment list is EMPTY. Neither those six names nor five names from the source strict-less-than inner loop are reached. DefaultMaterial1 is applied only inside a reached assignment iteration, so it is NOT a global fallback here. Keep actual SCB MaterialID/default1 from the genuine collision loader; never repair the authoring by activating material2. Candidate explicitly verifies empty assignments.

## Voice capacities and lifetimes

Host `material_audio50HI::Provider`:16 existing base slots64..79 when called with actual firstVoice64. RA has8 additional rows:collision9..12 at192..195,drive9..12 at196..199. The patch names RA's independently measured8-slot bound; these are selected-track-exclusive native host slots, not guest channels or simultaneous MA/SA ownership. They remain below canonical D44 firstVoice220, above original AI engines160..191, and do not expand maximumSoundCount44 or audioMaximumVoices948. Existing non-TA active/group-slot preflight remains intact.

Canonical all-original-AI D44:8 actual manager rows ×24 sounds =192 distinct logical sound owners. Existing `native_original_ai_material::prepareArrays` uses each genuine Pool::Emitter, actual vehicle248, same Effects110 group and same material provider PCM bank. Actual range for manager ordinal i is `220+44*i .. 243+44*i`; spare20 slots per44-wide reservation remain unused. Last racer uses528..551.21 shared immutable PCM assets never imply21 logical voices, and the host24-slot diagnostic subscriber cannot replace D44.

Each row is constructed only after actual all-ready mono PCM preflight; collision then drive source order279C80/279F60, loop bits0/1 respectively. Missing/unsupported PCM remains an explicit negative load condition. Provider preparation alone can retain missing records; it does NOT certify canonical D44 arrays. Muted output must still complete logical owner/group/PCM binding.

`original_ai_audio_owner.h::prepareMaterialArrays` must run before activation using the real D44 pool created for the actual manager slots. `OriginalPodMaterialAudioState` arrays32 collision/20 drive and existing44 per-racer admission safely contain RA12/12. Full source sample metadata233E90/26C240 remains unresolved; current explicit nativeDecodedRateUnity4096 policy does not mark basePitchKnown=true. Do not silently promote it to source pitch parity.

Lifetime: material provider bank must outlive every borrowed AudioClip sample pointer. Actual groups and mixer must outlive LogicalAudioOwner. On teardown stop/close each canonical material channel before D44 pool teardown and before freeing PCM; existing `original_ai_audio_owner.h::close` closes racer material channels before materialD44_.close(). Also reset/retire the host subscriber while its mixer and bank are live. Actual LogicalAudioOwner stops only playback revisions it owns, protecting foreign reused slots. A failed source construction prefix remains terminal and owned until ordered cleanup, rather than retrying appended rows.

## Remaining42 source ambience and intro ownership

AUB has42 numbered SoulTree sound records,15 unique source clips.29 rows author looping T,13 F;11 have explicit OneShot T and ResetWhenInaudible T. RandomTriggerPercent is100 for33 rows,40 for7,30 for2. Full exact records remain in the authenticated raw AUB and `raw/aub-inventory.json`; the native closure preserves block fields and duplicate labels without inventing selection/trigger behavior.

Unique ambience: rylcrowd(7),torch(4),pump(4),steam02(6),vats(2),rockhum(4),metlwsh2(2),rockwsh1(2),cnyncry3(2),cnyncry2(2),headston(2),catwalk(2),rockwsh2(1),metlwsh1(1),cnyncry1(1). Counts are authored Sound records, not a recovered simultaneous hardware-voice requirement. There is no source-backed42-Sound native scene manager join or stream/camera episode owner in this candidate. Do not reserve arbitrary extra mixer slots or merge repeated-file rows.

scrptLns: NumberOfLines2; Line1 CameraNumber1,2,3,4; Line2 CameraNumber5,6,7. introAud: NumEvents2, speech\\Dax\\Tracks\\Ryloth\\daxrya37.vag and daxrya38.vag; both IsStream1,IsStereo0,IsLoop0. Preserve stream authoring; no fake stream owner, fallback clip or intro event has been created.

## Original clip availability, accurately bounded

`original-clip-identities.json` records all38 unique filenames, exact case-preserving ZIP member paths, bytes, SHA256 and raw48-byte header words. All38 exist in the original AUDIO archive supplied by root:
79019408 bytes, SHA256c246ad58020726d92a65a56e09e4cd9b68ba60cdcc9aa697d2cb48e51275aafb.

Availability includes all21 material clips,15 ambience clips and2 intro clips. It proves original encoded bytes are available only; no PCM decoding, loop validation, frame counts, native stream delivery or audible playback was attempted or asserted. No generated PCM/output hashes exist. Existing loadAudioBank50HC must still admit each original under its actual decoder limits and lifecycle.

## Integration boundary

The provider code is sufficient for a later explicit `Track::ra` cold call. Actual terrain track dispatch currently selects TA/GC/GA/MA/SA and RA admission remains closed. Source assignments must join the same actual SCB zone/ordinal that supplied collision storage. Future RA activation must construct actual manager/D44 identities, use one existing application/group/mixer domain, retain the bank through logical teardown, and separately recover genuine42-Sound scene ownership and two intro/camera-stream dispatch. No source evidence here waives those joins when output is muted.
