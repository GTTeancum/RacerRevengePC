# Default-unlocked race identity/audio metadata

Adds original POD02, POD12, POD15, and POD20 identity records, including exact archive, PHY, speech-category, and Dax authoring identities. Existing selected-audio, lap-audio, announcer, and AI engine readers resolve these records through their existing strict source-authentication paths. Their speech counts are all 2,3,3,3,4,4,3,3,3, with SOUNDS.INI SHA-256 f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2. Existing semantic cue capacity is sufficient; no borrowed character audio or additional cue slots are introduced.

The additional-pod preparation catalog is consumed by prepare_original_roster.py. Its --additional-pods accepts these four originals and POD19 without changing the eight-racer field. All specified source containers must exist, and catalogued additional containers must pass their exact byte-size/hash admission before output-directory creation or importer invocation. After import, actual PHY and embedded speech authoring hashes must match the catalog. Missing original motion/rig/collision members still fail their existing native cold loaders. The script was edited but not executed.

Identity lookup is distinct from race admission. This change does not add a host-menu selection, Course destination, runtime rig, source file, generated cache, replacement geometry, or substitute handling. Camera/roster readiness gates remain with their respective owners. Existing POD19 preparation remains independent of whether the loose Dax directory is installed; the live audio reader owns its existing strict Dax checks.

## Required original containers

- PODS/POD02/POD02.RES: 385218 bytes; SHA-256 7cee08d049a47d088e8329ab63d622471299945d32f76ce9c9fb21b358e845e0
- PODS/POD12/POD12.RES: 389534 bytes; SHA-256 de144b1067eefd3037f4f0a7325beedc5c1c98e806b3802a08cdfec91bbf6f19
- PODS/POD15/POD15.RES: 324173 bytes; SHA-256 3501ca0a81e0485575c7a5ff217f3ac6a9248c15824d8dff9bbd1bc2f7aae326
- PODS/POD20/POD20.RES: 309724 bytes; SHA-256 8c5f282b53c230861e26acf797b8ab2ba1ef8856b8b476e3cfea55b48e3040ec

## Source provenance

Archive and embedded member pins come from RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Evidence.zip (256356241 bytes; SHA-256 60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c), member evidence/importFull48F-20261003/native-cache/generations/3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39.json. That generation document records the original containers and every original member's source hash, including PHY files that its older converter labeled unsupported. That historical status is not treated as a current native-reader acceptance result.

Current original AUDIO/SPEECH/POD02, POD12, POD15, and POD20 category/count files are present. Their file-byte SHA hashes match the retained embedded-category pins. The DAX/INRACE/PODxxANN.INI files are also present, with actual declared event counts 30/29/30/29 respectively; their byte sizes/hashes are recorded in the catalog and C++ identities. Camera presets and UI presentation assets do not replace the missing race archives.

No build, test, syntax/import check, game, decoder, or emulator was executed. No user-computer access occurred. Source-file reading, archive-directory listing, metadata extraction, hashing, and code editing only.
