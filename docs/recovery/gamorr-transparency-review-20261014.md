# Gamorr hanging-banner transparency: source-only review

Reviewed current source a67bcaf on 2026-10-14 and the user's circled image at `user-transparency-reference/image(3).png`. No production edits, builds, tests, syntax checks, imports/converters, project executable/guest execution, or remote operations were performed. Only retained source, assets, metadata, hashes and raw byte counts were inspected.

## Finding

The current source already contains the directly matching correction for these GA hanging banners. This is a missing inherited alpha-test state, not missing texture alpha and not a reason to turn every black pixel transparent. Commit `2eaa71a7661ae13cfb0fc3189e0c22010a48795e` repairs static GA material publication; `34b352f1db0ffda9c6476c756daf6793428b7e46` applies the same inherited policy to moving GA surfaces. Both are ancestors of a67bcaf, and their call sites remain connected. No further GA production change is justified by this image alone.

The screenshot is consistent with the already documented Watchtower banner defect. The exact individual on-screen instance cannot be assigned to a particular flag/frame from this screenshot alone; however, the retained materials and geometry establish the actual GA banner asset family, and the previous correction explicitly targeted the same hanging banners.

## Actual resource identities and independently inspected alpha

Retained packet: `/workspace/scratch/74cd1d3d7a45/transparency-report/packet/`.

Original `TRACKS/GA.RES` materials, all 443 bytes, one `PreLitCompressed` pass:

- `textures\\s_flaga.psm`: logical offset 18677760; SHA256 `a440549ae7fcaac1fb6b664ca3ba521fdccd825e24b80cb45190859f1686196a`; TextureMap_1=s_flaga; initial frame 1, 14 FPS.
- `textures\\s_flagb.psm`: offset 15212544; SHA256 `8235c9527e1bb7be4667b5d0acb2661b10db53b81cd9e3087ddd757da84ce390`; TextureMap_1=s_flagb; initial frame 5, 12 FPS.
- `textures\\s_flagc.psm`: offset 15237120; SHA256 `78cbdf98f3ecc14a0c70bc4dd8c8e765208546aee9dd7ec8488b5d873496b06d`; TextureMap_1=s_flagc; initial frame 3, 15 FPS.

These PSM bytes were directly read and hashed. They omit the alpha-test fields. Their identities agree with `tests/native/ga_atm_animation_index.inc:3-5`.

Starting retained textures are 128x128 RGBA RRTEX001, with their 16-byte header interpretation independently read in `src/assets/native_asset48b.cpp:99-116`. Counting every fourth payload byte using shell byte inspection produced:

- `s_flaga-frame1-mip3.rrtex`: 8,872 alpha-zero texels, 7,512 nonzero; SHA256 `cd4c1c35a373d85a87b37eb380baab8eef32f58f64ff110ccd7ece53a7b33f1a`.
- `s_flagb-frame5-mip3.rrtex`: 9,237 alpha-zero, 7,147 nonzero; SHA256 `eb4bd47e59bce4da18cbdcd350e7eecfcc221fad794a1b4ac3e32716a14e878c`.
- `s_flagc-frame3-mip3.rrtex`: 9,359 alpha-zero, 7,025 nonzero; SHA256 `8333fab0da5503a8a70d3f79b75f98cee9d78588eca9be027ddb26f0f17c0678`.

All three texture hashes match `tests/native/source_texture_ga_index.inc:2-4`. No image was regenerated or converted.

The retained `metadata/prepared/evidence/visualGA/asset-selection.json` ties these materials to authentic geometry: s_flaga to markethuta/d/f; s_flagb to those plus housestands/housestandsb, mining_droid/mining_droid_s, smoke_stack_a; s_flagc to markethuta/d and housestands/housestandsb. This is material/geometry ownership evidence, not a guessed classification based on texture names.

## Original alpha policy and live native consumption

`original/APPSTART.RES` independently hashes to `04eb376a507d0745925e4351903a8ef7a1024d37d8250b78892cba0c74220b89`. Retained `DefaultMT-source.txt` hashes to `1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7` and explicitly sets `alphaReferenceValue=0`. Constructor source fields (`src/assets/original_ui_material_fields50kg.inc:29-32`) supply enabled alpha test, method 6/GREATER, failure method 0/KEEP. The constructor reference120 is overridden by this genuine template's zero; substituting the constructor literal would be incorrect.

The resulting flag policy rejects alpha <= 0, retaining original opaque/depth-writing behavior for accepted fragments. Enabling blending is not the equivalent fix, because alpha testing also prevents rejected fragments from writing depth.

Current chain:

1. `tests/native/material_alpha50gx_fixture.h:128-162`, selectGa, authenticates the actual DefaultMT owner, inherits four alpha fields, then applies authored overrides. It preserves the previous admitted GA blend/lighting policy. Omitted alpha-test fields no longer imply disabled coverage.
2. Static world: `tests/native/terrain_interactive_drive50du.cpp:5083,5099-5100` selects selectGa with live templates/runtime and publishes sourceAlphaTest/sourceAlphaReference.
3. Moving world: `tests/native/ga_dynamic_window_fixture.h:42,50-51` uses the same selection and publication.
4. Renderer retains both fields for static and dynamic spans in `platform/linux/native/vehicle_window_gl50ds.cpp:1331-1332,1413-1414`; draw paths bind mode2 and reference at 4361-4362 and 4445-4446.
5. Fragment shader at 2229-2239 multiplies original vertex alpha and sampled texture alpha, computes its declared GS-unit approximation, and discards byte values <= source reference. The existing opaque output alpha1 is downstream of that discard, so it cannot bring rejected fragments back.

## Retained verification versus this review

`docs/ga-banner-alpha-inheritance-20261014.md` records an earlier Windows Ben/GA visual pass for source1791f95, completed 13:19:41 UTC, with the hanging black rectangles absent, and the moving-publication34b352f basic pass at 14:24:22-14:25:07 UTC. This review confirms source and retained raw assets; it did not rerun or independently rewatch that historical runtime evidence. Neither historical pass establishes current full-scene pixel parity. Current a67bcaf's general passing build is not a new targeted banner visual proof.

## Bounded plan

- Keep the already merged static and moving GA alpha-inheritance repairs. No new color-key, texture rewrite, alpha scaling, blend toggle, geometry edit or generic transparency patch is warranted.
- If the supplied image is an old capture, explain that it depicts the defect already addressed by these commits; the original alpha data was present throughout.
- If the user reports these borders in a capture made from current a67bcaf, first establish that capture's source/executable provenance and exact location. At a separately authorized central visual check, compare the same banner area and retain its actual material/frame/bound alpha state. Do not launch that check as part of this source-only investigation.
- Only if a current authenticated capture still exhibits the defect, trace the precise span through the chain above. A disagreement between retained frame alpha, inherited material policy, publication and bound shader mode would justify a narrow correction. This review found no such remaining disagreement.
