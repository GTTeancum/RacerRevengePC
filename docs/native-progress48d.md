# Checkpoint 48D: native scene cache and recovered vehicle forces

Date: 2026-10-03. Goal remains active. This is concrete progress toward the full native PC game, not completion of a playable race or the 60 presented FPS target. The previous goal turn answered the usage question; this continuation changes native source and supplies new execution evidence.

## Native converter and scene path

The expanded rigid geometry decoder now runs inside the built-in Windows importer. It retains node hierarchy, local/bind transforms, original bounds, materials, all LOD cutoffs, local positions/normals, UVs, color presence and rigid bone selection. New `RRSCENE1` objects accompany the compatible `RRMESH01` export. Converter version `rr-native-scene-48D-v1` invalidates reuse of old objects without removing them. Hash verification, original-file read locks, staged object writes, cancellation/resume and single-pointer generation promotion remain in effect. See `native-scene-cache48d.md` and `native-scene-converter48d.md`.

The full original directory scan read 103 containers and 34,128 members. It converted 5,069 unique objects and reused 489 duplicate references: **5,558 accepted geometry entries**, versus 2,184 in 48C. It reports 28,570 unsupported members, including other asset types. The accepted generation is `9e1461eac8f80997957b6e99c24f70e2d59bc63ca5c31c4e7d73e445702867b0`. Source and per-member hashes are retained in `evidence/importFull48D-20261003/`.

`--scene` now builds native draw buffers from semantic vertices and bind transforms and draws each LOD0 surface span. It does not use the stored compatibility mesh as its draw source. Standalone scene loading checks hierarchy composition, all-LOD original bounds, finite values, count/index/bone ranges and compatibility-mesh agreement. Original normals, LODs and materials are retained but their complete runtime behavior is not yet rendered.

## Texture recovery

The original loader/upload/TEX0 path and supplied PCSX2 tables establish that source PST1 indices are linear, while palette lookup swaps index bits 3 and 4. This resolves the previous candidate ambiguity for the bounded revision1/flags0/type1 subset. Seven TA/Pod01 textures and their material/pass/slot bindings are preserved. Validation covers 554 checks, 25 original mip surfaces, 375,040 RGBA texels, and 49 upload-layout shapes containing 4,129,024 unique labels. Exact source RGBA bytes are unchanged.

The texture converter is still offline Python; integrating it into the built-in converter remains work. Original shader modulation, blending, alpha interpretation, filtering, mip selection, specular maps, flames and sky are not accepted. The native ground inspection uses the proved source texture with the existing diagnostic shader, so its brightness is not a final fidelity result. See `texture-material48d.md`.

## Native vehicle functions

Original engine update at 0x2E7720, gravity at 0x2827D0 and engine application at 0x2E7C10 are recovered into ordinary native data/functions and compose with the 48C body core. Engine smoothing remains per invocation because the original function never reads elapsed time. Thrust uses the ready body basis, while original torque terms are added directly in their established order. This avoids invented vehicle dynamics or assumed action mappings.

The exact original-word oracle, saved connected snapshot and partial trajectories pass **423,693 checks per architecture**, including four rounding modes. These functions are compiled into the native game library and exercised by the native regression executable; they are not yet driving the renderer. Aerodynamics, repulsor/collision, scheduler/action mapping, cached-speed refresh and scene synchronization still need recovery. See `vehicle-forces48d.md`.

## Builds and execution evidence

Final build: `evidence/nativePc48D-build-20261003-134330-366`. Both architectures pass all seven CTest executables: asset414, control130349, legacy importer3250, body299019, expanded scene7384, semantic cache121260 and vehicle forces423693 checks. The scene tests cover all 228 accepted TA models and all 13 Pod01 models against frozen independent evidence. The two unsupported TA animation variants remain named exclusions.

- x64 application SHA256: `764eda51391cdbcf6c21f9ea87065190314bc21bc1f537695530089561c10caf`.
- x86 application SHA256: `b0ca25cac4deb24199be97e0c53fde602eab6ba4381104ad32a8b5ce3cd06b11`.
- End-to-end importer checks: `evidence/import48D-x64-jjjc79ng` and `evidence/import48D-x86-1tk5i74g`; 15 native invocations each, including semantic-file corruption repair and all228 mesh comparisons.
- Textured ground scene captures: `nativePc48D-run-x64-20261003-134711-789` and `nativePc48D-run-x86-20261003-134712-430`; identical PPM SHA256 `34656bb195372682f0cb3df2273168cc8fcbefd1b36a0016c15e60482fb443c5`.
- Pod01 chariot scene captures: `nativePc48D-run-x64-20261003-135122-542` and `nativePc48D-run-x86-20261003-135123-103`; identical PPM SHA256 `7ecb59653ea59e40ad34742e4e12b7eefe5b66fde466be9d2baef62f2fad4891`.
- Captures use hardware D3D11, 960x720, three completed offscreen submissions and zero presents. They are not gameplay benchmarks. Original PPM and lossless PNG encodings are retained; the latter were visually inspected.
- Source-selection/link-map/import/module/readback audit: `evidence/native48D-parent/audit.json`. This is not an operating-system trace of every file opened.

The first build's syntax error in the new physics test and its failed log are preserved. Full-cache verification also exposed Python3.12+'s compensated `sum()` versus native explicit left-to-right binary64 matrix composition at POD14's unused r_hand_dummy node. The verifier preserves the frozen decoder, requires exact original mesh bytes, uses a separately named ordered matrix contract for semantic encoding, and records each original Python arithmetic discrepancy. The full verification passes for all 34,128 original member hashes and all 5,558 accepted mesh/scene pairs (5,069 unique originals), recording matrix-arithmetic differences in five objects. It applies no blanket numeric tolerance. See the full-cache independent-verification log/report and preserved first-failure log.

## Recovery and remaining work

The checkpoint source/evidence archives depend on verified48C archives, whose manifest retains the earlier dependency chain. `tools/archive_checkpoint48d.py` checks build/source/test records, writes member hashes, reopens every archive member and verifies its SHA256. The adjacent output manifest is the authoritative final archive hash record.

Next integration must connect native textures/material bindings and original shading to scene surfaces, recover scene instances/runtime pose/visibility, and continue the missing vehicle force/collision/timing path into an actual controllable race. Full races, progression, AI, camera, audio, original handling, sustained60FPS at1280x720 and Xbox execution remain unverified. No emulator execution is introduced into the native production path, and no smaller milestone replaces those completion requirements.
