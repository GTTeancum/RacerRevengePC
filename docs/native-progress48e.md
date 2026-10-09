# Checkpoint 48E: native materials, original world placement and aerodynamics

Date: 2026-10-03. The preceding 48D goal turn made verified source/evidence progress. This continuation integrates native texture/material conversion and scene binding, recovers original world placements and advances native vehicle dynamics. The full playable 60 FPS PC goal remains active; no race, progression, audio, final shader or Xbox performance acceptance is claimed.

## Built-in native content conversion

The application now converts supported PST1 textures and PSM authored materials directly in C++, with no Python or guest execution. It preserves raw RGBA/alpha, every accepted original mip, original pass/slot names and authored versus unspecified fields. Scene objects retain their 48D version so migration reuses them byte-for-byte. New texture/material versions and the overall 48E generation identity are described in `native-content-cache48e.md` and `native-texture-converter48e.md`.

The full original disc directory import succeeds across 103 RES containers and 34,128 members: **5,558 geometry, 3,850 texture and 6,107 material entries**, totaling 15,515. It writes 3,118 new unique texture/material records, reuses 12,397 existing/duplicate references, and reports 18,613 unsupported entries. Active generation: `0a5b648526e8c700bf904a91a2df5c04719d49b87e44395d22a22117961ea53a`. Evidence is under `evidence/importFull48E-20261003`.

Independent texture/material verification checks 11,317 original mip occurrences and 124,658,432 RGBA texels, every accepted authored material field, and all 1,772 original PST/PSM unsupported occurrences. The separate full-cache verifier checks every original member hash and all 5,558 mesh/scene pairs against the frozen decoder and explicit ordered48D matrix arithmetic. Exact logs/reports are retained; numerical tolerances are not used to hide conversion differences.

The first full import stopped on the original `AppStart\DefaultMT.psm`, which has a complete legacy DefaultShaderParams schema. Investigation identified specific other unsupported original forms: repeated material keys with unproved precedence, complete inactive shader sections, 640x448 UI images and small rectangular mip chains. The decoder now reports those bounded variants as unsupported after structural validation. Invalid syntax, zero dimensions, missing required passes and truncated data remain fatal. The failed result and original representative bytes are preserved.

## Runtime material binding and captures

The native runtime can select a scene by cache/container/member, validate its manifest and objects, resolve original material/pass/slot references and upload native texture mip chains. Each original surface draws with its own first-pass/slot-one texture. Additional programs/maps remain recorded and explicitly unapplied. Original shading, lighting, alpha/blending and sampler fidelity are still pending; inspection remains opaque, two-sided and base-mip sampled.

Both final architectures pass nine end-to-end invocations each, including 48D cache migration/cancellation, reuse, byte-preservation of 684 old scene files, 25 independently derived mip comparisons, damaged mip/material repair, cached ground/chariot rendering, manifest-hash rejection and prevention of cache-output overwrite. Logs: `evidence/import48E-x64-2ugjf7ni` and `evidence/import48E-x86-ln7996qv`. Both inspect 241 scenes, 40 textures and 60 materials from the TA/Pod01 fixture. Original fixture hashes are unchanged.

The chariot draws 3 separately textured surface spans using 3 native textures and 9 mip levels. Its PPM capture is identical across architectures: SHA256 `9132c1dcfad3a0c19b8d79d8bc1bf885e2cbf4cc7531b702dffa82d364c256e4`. Ground matches48D's proved source-texture inspection, SHA256 `34656bb195372682f0cb3df2273168cc8fcbefd1b36a0016c15e60482fb443c5`. Original PPM files and pixel-identical PNG encodings are retained; images were visually inspected.

## Original static world assembly

The offline decoder recovers TA's original parameter-block SCB/TOB records: 9 zones, 10 portals and 318 static placements. All 284 visual instances resolve all 228 static geometry assets; 34 collision-only references are preserved separately. The original scene setters replace 42 nonidentity source root transforms rather than stacking placement onto already transformed meshes. No placement is invented. Two separate animations remain unapplied.

The neutral assembled track has 110,196 vertices and 69,838 triangles. Source/transform tests pass 2,193 checks; proof preserves 510 original ELF words and 499 matching generated instruction comments. `docs/world-scene48e.md` records the math and remaining precision limits. This is a format recovery and diagnostic export; native SCB import, runtime instance handling, portal visibility, lighting and animation still need integration.

Final native world captures are `evidence/nativePc48E-run-x64-20261003-142216-076` and `nativePc48E-run-x86-20261003-142216-732`; identical PPM SHA256 `96fd7b92d1f1607c15583c84e66364da778b99136e6b1557340a57158d4b33fd`. Like the cached-object captures, these use hardware D3D11, 960x720, 3 completed offscreen submissions and 0 presents. They are not gameplay FPS measurements.

## Native aerodynamic behavior

The linked-body/ready-scene aerodynamic producer now runs directly on native data. It retains original lift/drag, damping, differential steering, low-speed, powerslide, inverted-response and timer behavior, original angle approximation and force-application order. The native partial vehicle fixture composes engine, gravity, aerodynamic forces and the previously proven body numerical core. 37 original tuning-key-to-field chains are verified.

Both architectures pass **354,461 aerodynamic checks** against original instruction fixtures, including four rounding modes, all five mode branches, saved data and 2,500 connected partial steps. The first test failure exposed a test-oracle vector-load bug: an instruction overwrote its own base register before later lanes were read. The fix captures the effective address once. Signed-zero MIN/MAX behavior follows the supplied PCSX2 implementation, with the reference excerpt preserved. Failed and passing logs remain under `evidence/gameplay48E`.

`docs/vehicle-aero48e.md` records accepted numerical contracts and missing dependencies. The game functions are compiled/tested native library code, not yet driving the renderer. Repulsor/contact forces, collision geometry/query, second-body interaction, scene synchronization, cached-speed sequencing, physical action mapping and actual timestep still need connection.

## Final build and recovery

Final build: `evidence/nativePc48E-build-20261003-141953-482`. All 9 CTest programs pass on x64 and Win32. Texture/material decoder checks: 256,233 per architecture. The preceding scene/cache/control/body/engine regressions also pass. Source hashes were captured before/after the build and the exact executables are frozen in its architecture folders.

- x64 application SHA256: `48b69bf31fd3f793d6f14074b70579957be1c57be47503d4d83b917c5c4af6c8`.
- x86 application SHA256: `5db414abc79e2e43d03395ec0991f9f5f98eb6838452b20534a71579c802f3eb`.
- Native source-selection/map/import/module/readback audit: `evidence/native48E-parent/audit.json`.
- Cache-render pixel verification: `evidence/native48E-parent/cache-captures.json`.

The initial preliminary build passed its eight then-configured tests but correctly failed the freeze check because an uncompiled in-progress aero oracle changed; that attempt is not accepted as the final source snapshot. Its logs, the initial aerodynamic oracle failure and the initial full-import failure are preserved.

`tools/archive_checkpoint48e.py` verifies final sources/tests/manifests, creates source/evidence ZIPs, reopens every member and checks exact SHA256. The 48E output manifest records final archive hashes and the verified 48D dependency archives. The original broader goal is unchanged: complete races/progression, original behavior/rendering/audio, stable sustained 60 presented FPS at 1280x720 on PC and independently validated eventual Xbox execution. Next work should integrate the recovered world format into the native cache/renderer and connect original contact/repulsor/collision behavior into the live vehicle loop.
