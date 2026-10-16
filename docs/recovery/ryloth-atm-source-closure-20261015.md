# RA raw animation closure

`ra-atm-authoring.patch` adds only inactive tests/native/ra_atm_authoring50la.h. No tests/build/syntax checks, conversions or runtime activation.

`Owner::decode(shared_ptr<const ra_cold_sources50LA::Owner>, Diagnostic&)` retains the actual authenticated RA archive owner and exact PSM/ATM bytes. Existing material decoder validates genuine pass membership/configuration; existing indexed ATM splitter validates sixteen32x32 single-mip kind0 fholobeam frames without pixels. Crowd's exact six-frame kind2 member remains authenticated raw data because the current indexed splitter rejects kind2. No invented per-frame hash or generated image is used.

Crowd: PSM2738bytes SHA fec34de7468ae375ad3434b8c8113371773ea6100335027738ef25f1a03eb86f; ATM214578bytes SHA76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031. Actual pass1 PrelitCompressed, texture crowd, FPS10,6frames, repeat sampler. Retained source inventory reports346 static material users, not yet a canonical clone/mesh attachment count.

Finishline: PSM2833bytes SHA09ac1aeef799d7b0c884873cc457de96b879dcdfdbc9085267a6f7d8f4d895aa; fholobeam ATM9676bytes SHA3013cb65a05f43857af0144e0d9f6e04b7e26173f94e363b5eacfb967b6ccb9f. Actual animated pass2 Diffuse, texture fholobeam, FPS16,16frames, clampU/V. Its pass1 diffuse/finishline is retained separately in the complete PSM; no static placement is fabricated.

Both source PSMs author unique, reverse0, loopTo0 and omit initial frame/looping/animatesWhilePaused. Existing29B950/29AB30 parser proves defaults0/true/false. Config objects here are immutable authoring evidence, never live animation instances. No timestamp, pause, callback, canonical resource, GPU or renderer readiness is supplied.

Owner is stable/noncopyable/nonmovable. All reads are through retained actual archive. decode is terminal once attempted, including failure; partially retained data stays private. sourceClosure publishes only after both sources validate. Borrowed output is valid only while this owner remains alive.

## Concrete next owner gaps

1. original_standard_material_object50la.cpp currently admits unique texture animation only for one-pass ScreenObject materials. Crowd PrelitCompressed and two-pass finishline Diffuse cannot enter that path.
2. OriginalStandardMaterialTextureAnimation50LA is a null-ScreenTexture prototype with no model-specific clones/26EF10 attachment. Existing vertex-animation clones do not establish texture clone semantics.
3. Current canonical resource resolver uses splitOriginalUiAtm50KG (indexed kinds0/1) and rejects multi-mip ATM children at10239C. Crowd kind2/five-mip children therefore require genuine canonical IPUM aggregate/child support, even before pixels/GPU.
4. Existing serviceTournamentMarker24A740 correctly reads the actual material construction context's Application animation clock, checks explicit pause and duplicate timestamp, then reads leaf and controller commit timestamps. Its narrowly authenticated Tournament/null-object service is not a generic track callback and must not be reused under a renamed identity.
5. Existing MA/SA image banks take a bare timestamp and own host AuthoredTextureAnimation per span. Extending that bank alone would not supply the requested canonical Application domain/unique-instance lifecycle.

Further source recovery must prove actual per-model/pass clone and callback/parent ownership, including retirement, before implementing live lifecycle. Original image cache proofs and GL publication remain separate later work. RA course admission stays closed.
