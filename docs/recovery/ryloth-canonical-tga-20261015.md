# RA canonical TGA source/CPU ownership candidate

Isolated source-only candidate against central 6dc24c0. No central edits, project execution, conversion, tests, build or syntax checks. `ra-tga-source.patch` contains two canonical source edits and one private-friend native verifier. `compile-only-includes.patch` proposes inclusion of both the already integrated frame0 verifier and this new verifier in the existing main translation unit, without calls.

## Authentic admission

`ra_tga_source50LA::Verifier::append(actualResolver, actualProvider, diagnostic)` reads all three members directly from the sealed actual provider, owns a byte copy, and hashes each with the existing concrete native SHA256 implementation against fixed compiled pins. No caller digest or hash callback. It verifies RA archive identity (4604561 bytes,1010 members,SHA1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2). The provider remains the existing authenticated archive owner; its mount API is not replaced or elevated into an independent archive hash implementation.

Private resolver entry requires that provider to be the actual already-bound provider, checks mounted byte equality again, and uses the existing append transaction. Wrong provider/pin/shape fails before source append. Source duplicate/order/capacity semantics remain existing; newly owned file pointees remain stable. This is a cold serial API, not concurrent mutation support. No resource reference, texture-manager link or GPU work is created by verifier append.

Exact sources:
- textures\\LargeFlag.tga:65580 bytes,128x128,BGRA32,descriptor8,SHA352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d.
- textures\\MetalGrate.tga:196652 bytes,256x256,BGR24,descriptor0,SHA20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9.
- textures\\TwiDesigns02.tga:49196 bytes,128x128,BGR24,descriptor0,SHA693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15.

Public append retains its original64x64/24bpp structural subset. The wider branch is private to the concrete fixed-pin verifier. Existing enums retain their numeric values, with8888 cases appended. No generic public admission toggle.

## Source numeric and lifetime contract

Retained proof: docs/recovery/ryloth-tga-import-20261014.md, shared original_tga_conversion50la.h and fresh static listing tga-constructor-words.txt.

29EA90/29ECD8 converts BGR24 to top-origin RGB scratch;29EE40/29EF14/29F060 converts BGRA32 to top-origin RGBA scratch with alpha unchanged. Scratch completes before actual29C780 list registration. Registration, source-effects diagnostics, retained context/resolver and1326A0 publication/29FEF0 scratch disposal stay in existing order.

250F10/14 publishes dimensions.250F24/9C/A8 proves actual8888 format,251030/38 obtains E8,251040 writes AC.251048..94 uses flags2 to halve to16, capped5. Canonical native slots are descending:128/64/32/16 or256/128/64/32/16. This differs from ascending RRTEX export ordering intentionally. Generated levels carry absent source ordinal/indices; no fabricated authored PST mip offsets.

250040 allocates final levels before251228 conversion. Existing1555 numeric helper is reused unchanged for BGR24. RGBA8888 owns separate per-level CPU byte buffers; base reduction0 copies actual reader bytes.25124C->251720 produces each mip from its immediate predecessor. Shared168470 preserves the bottom-right-alpha/left-alpha contribution quirk exactly. No premultiplication, alpha normalization or generated output hashes.

Level views borrow their resource-owned final buffers. Destructor drops views and all owned1555/8888 buffers before unregistering actual manager link and clearing actual resolver cache. Partial construction retains actual allocated prefix and scratch for existing terminal cleanup. Cache publication remains existing checked live-context operation.

The original250F7C..98 (1555) and250F9C..B8 (8888) both load F8, AND with-5, OR with4, then store F8 in the branch delay slot. The candidate now performs that write after dimensions250F10/14 and before format251038/40/allocation, recording known-mask bit2. TGA resources therefore expose F8value4/known-mask5 rather than prior value0/known-mask1, including existing specmap5. Indexed PST remains value0/known-mask1. Actual material TEXA only consumes bit0, which remains known zero; no alpha policy changes. Other F8 bits remain unknown and this is not full opaque-field parity.

## Sampling and remaining boundaries

Canonical resources now can own actual CPU TGA levels when genuine later population requests them. They do not certify material/program admission, collector readiness, GPU publication or Course admission. No runtime call added.

1555 stays packed with source coverage bit. Actual later material TEXA must determine sampled alpha; for authenticated RA MetalGrate/TwiDesigns02 PSMs the recovered defaults are TA0=TA1=255,AEM0, so coverage is NOT transparent-alpha policy.8888 alpha remains actual source/mip alpha. No copied diagnostic images.

Original fallback ordering is unchanged: actual provider must prove PST absence, then IPU absence, before the same-provider TGA resolution. Existing live cache validation remains intact. Future RA factory must mount/authenticate actual full RA and templates, bind this provider, call verifier, then use actual material/resource population and renderer checks separately. Existing PreLitCompressed/model material program and texture sampling support must be admitted on their own evidence.
