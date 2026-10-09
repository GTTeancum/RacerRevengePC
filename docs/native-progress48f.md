# Checkpoint 48F: native world cache and original static placement

Status: verified native world integration, 2026-10-03. The complete native PC game goal remains active. This checkpoint does not establish playable races, gameplay FPS, progression, audio, original shader fidelity or Xbox execution.

## Implementation

The built-in converter accepts the recovered BINARY1 SCB world format and writes versioned `RRWORLD1` objects. These contain typed zones, portal connections/vertices, original model identities, placement matrices and every original parameter field with byte provenance. Runtime cache loading does not invoke the original SCB parser. The cache checks structural bounds, raw provenance, counts, typed/source consistency and exact matrix derivation. The overall converter is `rr-native-content-48F-v1`; geometry, texture and material object versions remain unchanged, so existing content can be reused byte-for-byte.

`--world --cache <cache> --container TRACKS/TA.RES --member tracks/ta.scb` selects the recovered original static layout. Original model references resolve in the same resource container. Present but unsupported/damaged PSGs fail rather than being silently classified as collision-only. The runtime holds the existing cache read lock across world and geometry selection, preventing a mixed generation during an import commit. Unique source scenes load through one verified manifest and share object-validation work.

Static placement uses retained local vertices and replaces the saved PSG root matrix, following the original scene setters. Immutable placed buffers are built once before rendering. Textures and material bindings are deduplicated across source scenes, but the placed GPU geometry is currently duplicated per instance. This is an explicit load-time assembly policy, not GPU instancing or an Xbox memory claim. World loading bounds retained scene payloads at 512 MiB and unique texture payloads at 256 MiB; reported payload counts exclude allocator and transient overhead.

The renderer uses original per-surface material references and mip payloads, with the existing diagnostic first-pass/first-slot opaque RGB shader. It draws all supported original static instances together. Original portal/exclusion visibility, alpha, fog, lighting, sky, animations and material programs remain unimplemented. Original collision-only references remain recorded without substitute triangles.

## Reference coverage

The unchanged 48E Python decoder independently accepts 12 complete track layouts: GA, GB, GC, MA, MB, RA, RC, SA, SC, TA, TB and TC. Together these retain 186 zones, 206 portals and 15, 340 model records. Native tests compare original fields and scalar matrices, original SCB/TOB round trips, and typed native cache round trips. The scalar contract is ordinary ordered binary64 derived from original binary32 vectors, not PS2 VU RSQRT equivalence.

Twenty-three END resource occurrences share an unzoned static-model schema, while MC contains duplicate parameter labels. Complete structurally validated variants receive named unsupported reasons; missing/truncated/malformed required data remains fatal. Their original bytes and independent rejection records are retained in `evidence/worldImport48F`.

## Remaining game work

The assembled static world supplies original scene data for the next vehicle/collision integration. Recovered control, body, engine, gravity and aerodynamic functions are still not a live driving loop. Next work must recover/contact repulsor behavior and collision queries, second-body interaction, original scene synchronization, cached-speed sequencing, physical action mapping and timestep, then connect camera/AI/progression/audio and original rendering. Representative full races at 60 presented FPS and 1280x720 remain the actual acceptance target.

## Verified builds, import and captures

Final build: `evidence/nativePc48F-build-20261003-150950-633`. All ten CTest programs pass on both architectures, including **416,524 world checks** each. The decoder's twelve canonical world fixtures are byte-identical across architectures. Existing geometry, texture, material, control, body and force regressions pass as well.

- x64 application SHA256: `66c27791cfe1b47113cb4a2bbd4505e280b244750a09023c01b52170edf7f2d3`.
- x86 application SHA256: `e0d3189b674e43d07f98d4237254b0af9635a9d503569ae86f1b3fac1e6407c7`.
- Final end-to-end tests: `evidence/import48F-x64-jr57ewco` and `evidence/import48F-x86-7y9j09s2`, sixteen native invocations each. These cover migration cancellation, reuse, old-object preservation, mip/material/world/geometry corruption and repair, rejection of invalid manifests and output aliases, cache locking and actual scene/world captures.
- Final audit: `evidence/native48F-parent/audit.json`. Source selection, source hashes, executable hashes, maps/import/module names and GPU readbacks pass. This is not an OS-level file-open trace.

The complete original-directory import across 103 RES containers accepts twelve new worlds and reuses all 15,515 preceding geometry/texture/material entries. It reports 18,601 unsupported members. Generation: `3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39`. The import ran before the subsequent runtime-only binding/indexing fix; its actual executable hash and logs remain preserved. The unchanged world converter's produced bytes match both final-build checker fixtures. Independent verification also proves every one of the 20,750 old cache files matches its recorded 48E archive SHA256. The final application successfully loads this complete cache on both architectures.

TA renders 318 original entries: 284 visual instances, 34 collision-only references, 228 unique source meshes and 42 replaced roots. Its 110,196 vertices and 69,838 triangles match the independent original-placement RRMESH bytes exactly: `4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7`. The renderer shares 21 textures with 89 mip levels; 279 of 353 spans have a resolved first-pass base texture. Full-cache and small-fixture captures match across x64/x86: PPM SHA256 `dcf6b6c4929180b54aef63ea192344dafe2394ef6654e9700c33d6d1d0489063`.

The captured image shows the assembled textured desert course and settlement. Dark shading and white unresolved surfaces remain visible; this is not final material fidelity. Captures use 960x720 hardware D3D11 with three offscreen submissions and zero presents. Original PPMs and lossless, pixel-equal PNG encodings are retained. Earlier ground and chariot capture hashes remain unchanged from 48E. No racing FPS is inferred.

## Findings preserved during validation

The first 416,500-check reports exposed a test-only lifetime error: a range loop used a member of a temporary JSON document, so nine new decimal edge cases did not execute. Retaining the document and asserting its nine-case count raises accepted coverage to 416,524. Reports, discovery notes and both builds remain archived; the extra checks actually run in the accepted final build.

Full-cache rendering exposed an existing material-resolution fault. TA's unsupported `cord.pst` was replaced with a different, supported same-named resource from GB/GC. Original source hashes differ. Name resolution now considers all original candidates, gives a local original ownership of its name even when unsupported, and requires matching source identity/status for cross-container fallback. The regression proves TA's resource remains explicitly unresolved; the erroneous capture and its launch remain preserved as `full-world.*`, with fixed results under `full-world-fixed-*`.

The same investigation replaced repeated whole-manifest scans with one name index per bulk load. Observed x64 full-cache world loading fell from 20,614 ms to 2,117 ms; the corrected x86 run took 2,494 ms. These are individual startup observations under the recorded run conditions, not a controlled throughput benchmark or gameplay FPS result. Final scene assembly remains about 75 ms and occurs before the frame loop.

Source/evidence archives are produced by `tools/archive_checkpoint48f.py`; it reopens every ZIP member and verifies its exact SHA256, preserves the 48E dependency hashes, all intermediate builds, logs, captures, source and format blockers. Completion of the full game remains unproven.
