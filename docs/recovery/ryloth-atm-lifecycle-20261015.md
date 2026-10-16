# RA texture animation: original ownership and bounded native join

Static original source only. No execution, guest emulation, build or admission. Raw closure is independently delivered/integrated; this document supersedes any assumption that `unique` automatically creates a clone per placement.

## Prototype and leaf

29B890 calls29B950 texture leaf,29BAF0 vertex leaf,29BBD0 pixel leaf in that order. If any exists it allocates32-byte controller and constructs29AAA0, storing pass+10 at29B920.29B950 allocates28-byte texture state: initial direction bits zero, previous tick+10=-1 at29B9C8, phase+0C default0 via29B9DC, parent pass+14 stored29B9E0, FPS+00 default1 via29B9F8, looping default1, reverse default0, loopTo default-1. Literal unique sets leaf+18=1 at29BAA8. Other names use the genuine named manager path; no named-manager substitute is admitted here.

29AAA0 owns the texture/vertex/pixel pointers at+0C/+10/+14. It derives controller unique+1C from leaf unique flags, starts outer stamp+08=-1, sets callback/context+00/+04=null and reads animatesWhilePaused+18 defaultfalse at29AB30.

Each material pass owns its own prototype. RA crowd has one such pass. Finishline has two distinct passes; only pass2 has fholobeam animation. Do not create a synthetic shared combined pass or alias it to crowd.

## Critical lookup behavior

29B1D0(a0=pass,a1=actual model key):
- Null model returns pass.prototype10 at29B200..204.
- Nonunique controller also returns that prototype.
- Unique controller with NULL registration table14 ALSO returns the same prototype. v0 was loaded from pass+10 at29B1E8;29B208 loads only v1;29B214 returns without changing v0.
- Only an existing registration table requires exact sorted model match. Match returns entry+8 at29B28C. Missing entry diagnoses at29B290..2F8 and returns null at29B2FC.

Therefore declaration unique is permission for separate instances, not evidence that instances exist. A native resolver must distinguish absent table from present table/missing key. The current bounded canonical ScreenTexture prototype has no table at all. Existing vertex-only native instance lookup assumes explicit registrations for its admitted racer route; do not generalize that policy to RA merely from the word unique.

## Genuine explicit registration and cloning

26EF10 iterates actual model+1C0 material slots in source order, reads tokens from+1C4, filters optional token or-1/all, resolves canonical material table382B38 through298E60 and calls29BCB0 with the SAME model pointer.29BCB0 visits every declared material pass ascending at29BCDC..29BD08; calls29B050 only if pass+10 exists.

29B050 checks prototype unique+1C. It lazily allocates table14, sorts by actual model address, suppresses duplicates at29B0F8, writes model key BEFORE clone allocation at29B174, allocates32-byte controller and invokes29AB50 at29B184, publishes clone pointer then increments table count at29B198..1A8.

29AB50 copies a unique texture leaf with a literal28-byte memcpy at29AB8C..29ABB0. It copies current phase, previous leaf timestamp, direction/configuration AND borrowed parent pass+14. It does not reset the cloned phase to original initial frame or mint a new texture resource. Controller outer stamp resets-1 at29AC68; callback/context resetnull at29AC6C/70; paused policy copied at29AC64. Vertex/pixel leaves clone through their own routines only when unique. Native allocations must preserve partial-effect reporting if original slot/key writes occurred before a failure.

## Source attachment and RA applicability

The bounded direct call inventory for26EF10 is four racer-engine sites2208D4/220928/220984/2209E0 and track action2DB718. No literal function-address table entry for26EF10/29BCB0/29B050 was found in the authenticated loaded file region; this is supporting static evidence, not a general ban on future callers.

Track action2DB6E0 resolves optional material name (empty ->-1/all), reads actual model at action+04, registers26EF10, then calls26F3F0 with callback2DB690 and context=actual action.2DB690 returns action+00 (orfalse if null).26F3F0 resolves actual model material slots and29BD80 selects existing model clone then assigns callback/context; it does not create clones.

World270210 visits original track object/action structures and only invokes2DB6E0 for type5 at270260. Authenticated RA TOB has only two LostAI actions (targets29/104); its four explicit TypeID values are particle emitters8. Its673 model parameter records contain only slt/allowshadowreceiving. No RA-authored type5 animation action is present. Thus the examined RA source load does not request model texture clones or action callbacks. Do not invent346 registrations from crowd's346 material users.

The other direct prototype callback setter29BD30 at1B2BC8 targets authenticated literal textures\\camdroid_flrred (3ABA80), not crowd or finishline.2DB1B0 is the separate trigger-driven texture callback path; it is not produced by RA's LostAI authoring. A future different world/action mutation must re-open this proof rather than retain a fake permanent callback-null flag.

## Actual draw identity and service

Model renderer2417B0..241858 supplies actual model s0 as a2 to246760 (24182C/241848).246760 forwards it as a3 to24A620 at2467A8/2467D8.24A620 retains a3 in fp at24A658 and supplies fp to29B1D0 at24A744. Therefore the ordinary model route supplies a genuine nonnull model pointer; null registration table still selects the prototype as above. No null-object relabeling is necessary.

24A728 selects each enabled pass;24A740 resolves controller,24A758 services29AC90.29AC90 reads actual Application+230 and+11C, skips equal stamp or paused-without-permission WITHOUT stamping, otherwise calls real callback if any. Callbackfalse skips leaves but still commits outer stamp29AD58. Texture leaf is serviced before vertex/pixel. The existing canonical Tournament service already reads the actual sealed Application/context clock with leaf and outer fresh commit reads; a future RA-specific service must reuse that real authority, not a caller timestamp.

Texture binding24B26C calls29AF90, which returns the first registered clone having a texture leaf if a table exists, otherwise prototype phase.24B274..24B284 invokes the actual aggregate frame virtual method with that ordinal.29AF90 does not consult the current model identity. Do not replace this binding with the currently serviced model clone phase: service lookup and texture binding selection are distinct original operations. RA's proven absent table selects the same shared prototype in both paths.

## Release

29ADC0 visits registered entries ascending at29AE00..29AEB8. For each controller it frees its private texture leaf at29AE24..29AE3C, then virtual-destroys vertex/pixel leaves, then frees controller29AE94. Table storage is freed29AEBC. Prototype is destroyed only afterward29AEC4..29AF44. Texture leaf free does not separately release map tokens: it borrows the owning pass/aggregate resource. Material/pass texture references and renderer retirement stay with their existing canonical owners.

## Smallest faithful native extension

1. Keep RA raw closure and actual archive/provider provenance. Add a narrow pass-origin admission for exact crowd pass1 PrelitCompressed and, separately, finishline pass2 Diffuse; no forced ScreenObject or ScreenMT. Original program table already has PrelitCompressed index39 and Diffuse26. Program name recognition is not renderer parity. Verify genuine model material-template association (DefaultMT rather than assuming screen default) before construction.
2. Extend the existing texture prototype representation to allow actual model keys with absent table selecting its SAME prototype. No RA per-model clone allocation is justified by current authoring. An eventual explicit clone API must implement the table distinction and28-byte-equivalent leaf copy above; present RA does not need activation of it.
3. Resource closure remains blocked for crowd: canonical resolver currently rejects kind2 PST and rejects multi-mip ATM children loaded with flags0. Source-backed102230/10239C aggregate/child handling, immutable source IPUM cache provenance, exact mip traversal and same canonical table resource identities are required. Existing indexed fholobeam splitter only proves its raw16 frames, not a complete finishline renderer.
4. RA-specific material-reference/draw facet must retain actual material table rows, pass tokens, model owner identity and actual Video/Application construction context. Bind and service only at the actual material draw boundary, with actual clock/pause authority and source duplicate/partial-effect chronology. Avoid a new clock or per-span phase bank.
5. Real generated image artifacts/immutable source-to-pixel proofs and checked GPU lifetimes remain missing. Nothing here supplies their hashes or readiness. The shared existing material collector/pass ordering and pending resources must remain authoritative.

This boundary supports future shared-prototype RA animation, but does not yet justify canonical construction or runtime activation. More code now would bypass unresolved IPUM/mip and template/resource joins, so the delivered executable-source candidate remains raw authoring only.

## Follow-on bounded correction (isolated)

The model template dependency is now located in existing OriginalStandardMaterialReferences50LA::acquire26E640/clone26EB44: actual prepared model names, actual36B970 and canonical retainDefault14D590 already establish DefaultMT. RA needs the actual caller/model join, not a duplicate template owner. The separate ra-atm-flags0-candidate/indexed-flags0.patch resolves the indexed kinds0/1 flags0 traversal from24EADC..24EBC8: skip all but last stored mip, load native slot0, publish levelCount1. Kind2 crowd remains closed and no source pass/renderer admission is added. This supersedes the earlier statement that all flags0 multi-mip behavior remained unresolved.
