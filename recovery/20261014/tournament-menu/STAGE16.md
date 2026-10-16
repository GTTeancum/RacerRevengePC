# Stage16: canonical Tournament ATM ownership and bounded prototype service

Source implementation only. No tests, builds, syntax checks, decoders, preparations, launches, project executables or guest execution were run. Root owns all integration and validation. This patch does not enable Tournament frontend admission.

Base: helper b259ca0 (stage15 note), runtime base3223697 (stage14). Root reported stage14 integratedfb40565. No central checkout was edited. Apply the single stage16 patch after root's existing stage14 changes; stage15 note is not a runtime prerequisite.

## Required independent dependency

Audio ROUND7 Library libfile_3d632a26a2e48191b81bd4f7194140d7, archive SHA256388c71bdf22bfb9b8ba2df4b2c2dd1ac9e390942fe593fef09a7aa46a4bf20c9, was read in full for its header/report/API. Root must integrate its header src/game/original_application_animation_clock50kg.h, context applicationAnimationClock() accessor, and MainFrameClock.applicationAnimation first. None of those files/changes is included or duplicated here. They are intentionally absent from this helper's older working source. No local buildability or validation is claimed.

The exact API is rr::game::OriginalApplicationAnimationClockView50KG::read(OriginalApplicationAnimationClockSample50KG&) const noexcept -> bool. Sample fields current230,pause11C,pending120 are uint32. Failed/unpublished/retired reads remain failures. Canonical source reads come from this material's sealed construction context's applicationAnimationClock(). The forwarded frame view is checked for consistent values, not presented as owner identity by value. This avoids authorizing a foreign Application clock as the material's authority.

## Implemented connected subset

- Resolver appends authentic ATM bytes as one stable file/texture source. PST/IPU/TGA absence must be explicit in the SAME sealed provider before ATM selection. Positive ATM bytes must match that provider/revision. Source-vector absence never proves fallback. Cache stores exactly one aggregate, with the existing numeric warm104960 retain distinct from shared backing pins.
- Tagged102070 aggregate holds real consecutive PST substream child resources. It does not register in29C6E0 and exposes no ordinary palette/format/mips/F8 fields. Children use the SAME retained file/context, actual canonical conversion backing and lifetime registration; no renamed PST provider rows or frame cache associations exist. Dimensions copy actual child0. frame102050 bounds-checks the actual count without clamp/substitution. pstExtent24E880 exposes original member-relative provenance for real children.
- Admission uses the existing ATM structural splitter in SOURCE only (not executed here), then the canonical PST structural rules. ATM scope is revision1/storage0,1..64 single-mip indexed children. Ordinary PST1..5 mip admission is preserved. Multi-mip ATM is intentionally narrower:10239C passes flags0 while the established table path documents flags2. Generalizing that traversal needs separate proof; authentic medstar has nine single-mip64x64 frames in the existing static evidence.
- Partial construction retains every actually allocated child including a failed child, with sourceEffectsCommitted visible to the existing terminal table population. Cleanup releases child numeric refs in ascending order, destroys their backing/list membership, frees the frame array, then clears the exact aggregate cache. The original loader's unconditional cache publication after failure is NOT claimed for this bounded native terminal path.
- Distinct texture prototype owner parses original defaults/overrides and retains its actual material/pass identity. Only one-map, one-pass ScreenObject/ScreenMT unique texture animation with no vertex/pixel leaf or parameters is admitted. Existing engine vertex owner/registration APIs are preserved. No racer clone, named-manager registration, callback setter, parallel phase or synthetic frame count is created.
- Collector comparison rejects merging when either texture leaf is unique. Ordinary prepareColdRepresentativeMap249610 still rejects texture animation. Explicit prepareColdScreenPrototypeMap249610 represents ScreenTexture's null object key and selects the SAME prototype, then an actual102050 child before ordinary leaf249610 fields. Binding is prospective/borrowed, never residency or accepted FIFO proof.
- acquireTournamentMarker1EFEDC owns a real ScreenMT numeric reference for original medstar at objectFA0 using the actual caller directory. It validates genuine Tournament PSM/ATM and retains actual application/material/texture owners.
- serviceTournamentMarker24A740 authenticates that canonical row/pass/source/aggregate and executes the callback-free29AC90 path. Duplicate current230 and pause11C early-outs do not write either stamp. The live owner is read at entry, texture delta29A118, texture timestamp29A16C, and final controller29AD48. State uses the existing bounded binary32 numeric helper with actual aggregate count. Later failure after a real state write is terminal and does not roll back prior effects.
- Callback false is NOT modeled as an early return: callbacks are permanently absent in this admitted owner. Code explicitly records that future callback support must skip leaves yet freshly read/write the final controller stamp. General callbacks remain unsupported, not simulated by a generic success callback.
- sampleCanonicalMarker forwards clock.applicationAnimation, services the retained canonical prototype, selects its actual child, and matches exact authenticated member/PST extent and dimensions to the existing immutable CPU marker image bank. It does not read legacy milliseconds/advanceAnimations, advance gameplay, decode a second image, or invent a win/progress state. Existing profile-current/popup gates remain. The old sampleMarker stays explicitly legacy and is not canonical lifecycle proof.

## Root caller sequence and remaining boundaries

At the original constructor point acquireTournamentMarker1EFEDC with SAME application/material/texture registries and actual36B970. Root mounts/seals the real provider and supplies authentic PSM/ATM bytes, then performs existing14D370/29E340 population and14D440 collector rebuild. At a genuinely scheduled ScreenTexture material traversal, call sampleCanonicalMarker with the retained reference and current collector receipt. Do not wrap canonical pause policy in legacy advanceAnimations. Retain reference, aggregate/child views and CPU bank through actual renderer retirement; release afterward. This patch does not create a complete Tournament session or renderer drain.

Open/unsupported: aggregate demand-consumption join (stage18 corrects the earlier102570 label: that virtual folds storage summaries, not demand; demand104A00 still rejects aggregate); multi-mip flags0 ATM; nonzero-storage TGA ATM; IPU; named/shared texture animation; callbacks, vertex/pixel combinations and racer instances; full141C30 inherited traversal; complete ScreenTexture reciprocal/geometry lifecycle; paused-only service scheduling and pending120 single-step; residency/upload/accepted renderer retirement; frontend mode2 admission; profile/result writeback. No profile/result files, capture headers, shared frontend or Audio-owned clock files were modified. No new translation units/build-list entries.

## Exact changed source files

1. src/assets/original_standard_texture_resource50la.h
2. src/assets/original_standard_texture_resource50la.cpp
3. src/assets/original_standard_material_animation50la.h
4. src/assets/original_standard_material_object50la.h
5. src/assets/original_standard_material_object50la.cpp
6. src/assets/original_standard_material_table50la.h
7. src/assets/original_standard_material_table50la.cpp
8. src/assets/original_ui_screen_material_reference50kg.h
9. src/assets/original_ui_screen_material_reference50kg.cpp
10. platform/linux/native/original_ui_tournament_career_resources50kg.h
11. platform/linux/native/original_ui_tournament_career_resources50kg.cpp

Supporting note: recovery/20261014/tournament-menu/STAGE16.md. Source review used the supplied ATM owner plan libfile_4937887a5d20819198df34555d1df51b, clock proof libfile_8b852b6dbe8881919ac1eb79e314150b, Audio ROUND7 header/report, existing static Tournament evidence, and manual source/diff reading only. No test claim.
