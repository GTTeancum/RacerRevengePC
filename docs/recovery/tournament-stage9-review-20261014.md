# Tournament stages 8 through 10 source-only review

Reviewed 2026-10-14: stage 9 patch `1099ea906bc1a12079c7686a1f833b41fbcfa5e9` incrementally atop the held stage 8 patch, against the existing recovery APIs and root trophy separator correction `1d74e952de3aacd85bafbdc5e709f1211e3d400f`. Neither feature patch was applied by this reviewer. This report is outside the repository. No production edits, commits, tests, builds, syntax checks, imports, scripts, launches, or guest execution were performed. Original SLUS data was inspected only as static bytes.

## Verdict

One definite C++ type mismatch blocks compilation: the portrait fallback passes a `std::string` to a builder requiring `const char*`. The delta correctly resolves the stage 8 notch catalog separator blocker. Otherwise the finite portrait material provenance, background material/archive split, packet geometry/order, ATM frame ownership, and canonical animation API use match by inspection. The marker remains a bounded accepted-frame sampler with explicitly unfinished application material-update/session integration; it must not be treated as a complete animation lifecycle or frontend admission.

## S9-R1: portrait fallback passes std::string to const char* parameter

Severity: high, compile blocker identified by source inspection only.

New `readOriginalUiTournamentPortraitFallbackMaterial50KG` contains:

`const auto name=riderPortraitName(career), registered=std::string("ui/textures/")+name;`

and later:

`auto next=screenTextureFallback(p,name,*inherited,runtime);`

Canonical `riderPortraitName()` returns `std::string`; canonical `screenTextureFallback()` takes `const char* texture`. There is no implicit conversion. The existing Rider portrait helper already uses `name.c_str()` for this same builder. Required narrow fix: pass `name.c_str()`. The builder synchronously copies it into its owned textureName string, so that temporary borrowed pointer is safe during the call. No build or syntax check was used to establish this finding.

## Prior-stage resolution and integration cautions

- Stage 9 changes all three notch **catalog-only** lookup arguments to canonical backslashes while preserving exact forward-slash provenance strings. This resolves stage 8 S8-R1 for notch. Its new portrait whitelist follows the same correct split: backslash page/PST/PSM lookup, matching forward-slash recorded provenance. Root's trophy-only catalog correction must remain retained; stage 9 intentionally does not duplicate it.
- Stage 9 updates stage 8 count documentation to “negative converted counts,” resolving that prior wording ambiguity.
- The original stage 8 header delta still requires reconciliation with root's already-corrected lifetime comment. Stage 9 depends on stage 8's material/API additions and cannot be applied directly as a self-contained feature. No patch applicability command was run.
- Stage 9 changes load signature to include a Backgrounds owner and changes `materials()` to `array<...,24>`. All existing owner-local image indexes 0..12 stay unchanged. Any future caller must adopt that signature and bank bound.

## Original resource/material identities

- Original entry formats selected `%02d_flrg` and loads ScreenTexture `1000` at `0x1EF034`; the new selected portrait PST comes from authenticated Tournament. The dedicated builder permits the original authored rider set 1..26 excluding 5, 6, and 13, matching existing canonical portrait naming and the pod/profile gates. Unsupported careers reject rather than borrowing another portrait.
- The finite portrait provenance whitelist checks canonical Tournament archive, canonical standing page, null fallback-widget identity, source caller `0x1EF034`, canonical same-career portrait PST, absent corresponding PSM, exact registered/attempted path spelling, and exact texture basename. Shared fallback admission retains null authored pass identity, exact APPSTART ScreenMT inheritance, pass 1, creation PC `0x2475C0`, and empty authored shader section. The added enum is appended without renumbering prior origins. Apart from S9-R1, the helper follows the existing authenticated missing-PSM contract and creates no counterfeit PSM or generic fallback.
- Original selected `%02d_bkg` loads four `3D80+60*i` ScreenTextures at `0x1F17AC`. The current Tournament catalog authors all corresponding supported `_bkg.psm` files; original Backgrounds archive has their PSTs. The patch correctly reads the background PSM from Tournament and the selected exact PST from Backgrounds, without cross-archive basename search.
- Marker PSM and ATM both come from authenticated Tournament. The current catalog records medstar PSM (2774 bytes) and ATM (46344 bytes), matching retained original catalog identities. It is an authored material, not a fallback.
- Images are ordered: 0..12 prior resources, 13 portrait, 14 background, 15..23 nine marker frames. Storage, public materials return type, material loop, native sampler conversion bank count, and image count all agree at 24. Marker per-image bindings all retain the one original `medstar` material name/source identity. This creates copies of immutable CPU material metadata, not nine registry instances or nine animation controllers.

## ATM frame ownership and material validation

- Canonical `splitOriginalUiAtm50KG` signature matches. It reads revision/count/selector, validates each exact consecutive PST extent and supported palette/mip shape, and rejects trailing bytes. Nine frames are required explicitly. No frame search, guessed offset division, or substitute pixels are used.
- The patch uses the actual split member-relative PST offsets/lengths and original authenticated ATM member for image source provenance. Owner-local image IDs combine archive/member ordinal and frame offset; all offsets are bounded before uint64 composition. Static whole-PST IDs use offset zero, while these ATM frame extents begin after the header. Identities are distinct and index ordering matches frame order.
- Every decoded marker atlas must be 64x64. The authored exact medstar pass governs all frames; each frame passes unchanged canonical bounded native material preparation and sampler conversion. Images below 15 reject animation fields; marker entries explicitly allow the authored animation fields rather than pretending they are standalone textures.
- The animation mode must be `unique`; recovered omitted initial frame and looping defaults are read through canonical DAT APIs. The patch requires initial 0, FPS15, looping1, reverse0, loopTo0, exactly matching the supplied authored medstar text and existing recovered constructor defaults. Nine metadata copies are checked, and one retained controller is initialized from the first equivalent canonical pass.
- Parse/decode/load/material/controller failure still occurs before Storage publication and leaves the cold owner empty. The new local ATM diagnostic is not exposed in the public diagnostic, so only the top-level animation source PC is available for that failure; this is a diagnostic limitation, not an authenticity bypass.

## Geometry and source order

- Portrait object `1000` is placed at `(296,100)`, with actual same-name decoded atlas dimensions and full UV rectangle, matching entry `0x1EF014..1EF088`. It retains source default white packet channels and depth.
- Background objects `3D80+60*i` use explicit 320x224 dimensions and source authored depth15. Entry `0x1F17C8/CC` writes float15 to the source depth field; its subsequent packet-depth calculation belongs to the eventual actual video depth owner. The new quad stores authored depth15 rather than manufacturing a native Z value.
- Background origins are `(0,0)`, `(320,0)`, `(0,224)`, `(320,224)` with reciprocal-atlas sample endpoints319 and223, mirrored for right/bottom quadrants. Retained entry `0x1F1934..1F1D38` describes these exact sprite endpoint writes. The new corner order TL/TR/BL/BR matches canonical OverlayQuad. It does not replace the source 319/223 endpoints with dimensions-minus-one.
- Progress marker object `FA0` uses x `41+37*n` for 0..3, `215+37*(n-4)` for 4..7, and `386+37*(n-8)` for 8..12, y301. Completed progress13 uses `(641,449)`. Retained original constants/branches `0x1F10C4..1F139C` match. Existing rejection of progress above13 remains explicit strict native admission.
- Each marker frame packet is full 64x64 with the original default channels/depth. Source appearance recolors outlines/uncompleted icons/trophies and filled stat notches, not this FA0 marker, so default marker white is consistent with the retained appearance range.
- Normal-page source draw order is background four at `0x1F273C`, portrait at `0x1F2758`, stat groups at `0x1F2778`, medal subsection, marker at `0x1F2884`, then three numeric ScreenTexts. The new accessor/source-PC annotations preserve those subsection insertion points. They do not themselves join ordinary/top widgets or produce a complete frame seal.

## Marker clock and retained lifetimes

- Canonical `OriginalUiMainFrameClock50KG` fields are exactly known/advanceAnimations/milliseconds. Forward declaration in the public header plus defining include in CPP is compatible with use by const reference.
- `sampleMarker` requires current generation/mode and a known actual clock, and rejects popup bit0 before changing state. With advancement disabled it selects the retained frame; with advancement enabled it calls the existing exact native animation leaf. No host time is sampled.
- The existing leaf owns initial timestamp sentinel `0xffffffff`, repeat-timestamp suppression, unsigned millisecond delta conversion, separate binary32 arithmetic, looping/fmod and selected-frame truncation. The patch does not reimplement those rules or add a second timestamp owner. Initial sampling selects frame0 without invented elapsed time; repeated timestamps do not advance twice.
- The method copies the controller, advances/selects on that copy, bounds the selected frame, and only then copies the chosen packet and commits the copied controller. Rejection preserves output/state. There is no full-frame commit/rollback boundary: the header correctly restricts calling to an already accepted frame, and root must not use it speculatively before a later frame-admission failure.
- All packet `materialName` pointers refer to strings in a fixed Storage material array initialized before packet creation. Marker packets all refer to the single stored medstar name. Unique_ptr publication does not relocate Storage. The copied marker packet still borrows the owner; callers must retain the resource owner through actual use/submission/retirement.
- `bankInput` adds the same authenticated Backgrounds owner alongside APPSTART/Tournament/fonts and pins the same decoded image owner, so combined-bank reconstruction can authenticate both cross-archive background provenance and ATM extents. It does not itself publish a GPU bank or renderer-lifetime receipt.
- Popup bit0 suppresses returned normal-page geometry. This is an accessor/render gate, not a proven replacement for original material-update timing. Stage 9 explicitly leaves `0x1F24B0` material-update hook and full session integration unfinished. Do not infer that skipping draws necessarily pauses original material updates; the future session must own the real application pause/update policy and accepted-frame clock. This is a disclosed remaining boundary, not an additional claim of a complete lifecycle.

## Remaining limits

S9-R1 was reported immediately. No other definite new source defect was found in the reviewed combined stages after retaining root's trophy correction and applying the narrow portrait argument fix. Stage 8 stat arithmetic and source packet mappings remain as reviewed in `/workspace/shared/rr-stage8-review.md`. No numerical all-input parity, archive decode success, material admission success, compile success, rendering, full-frame ordering, or lifecycle readiness is claimed. Root owns fixes/integration and any separately authorized central test.

## Stage 10 continuation

Reviewed incremental patch `9f820ddc0bee7407c8304ac2f28fc895ab0b2e3c` after completing stage 9. The same no-execution/no-production-edit constraints apply.

### S10-R1: unrelated Controls name.c_str() change introduces a new compile blocker

Severity: high. Reported immediately to root.

Stage 10 correctly changes Tournament portrait's `std::string name` argument to `name.c_str()`, resolving S9-R1. However, its other material CPP hunk changes the existing `readOriginalUiControlsFallbackMaterial50KG` call from `screenTextureFallback(p,name,...)` to `screenTextureFallback(p,name.c_str(),...)`. That function declares `const char* name=nullptr` and assigns string literals/source const-char pointers. A pointer has no `c_str()` member. Keep the Controls call unchanged and retain only the Tournament portrait correction. This finding comes from the actual current source type declaration, without compiler execution.

### Exact material-demand traversal and usefulness

The new header is a concrete immutable, retained traversal plan, not a generic unused callback shell. Its fixed original object offsets, caller PCs, exact owner-local image associations, duplicate visits, and conditional counts are useful source evidence and a future canonical join input. It does **not** implement `0x1F24B0`'s consequential demand writes or inherited child dispatch.

- Portrait `1000` caller `1F24D4` occurs first.
- Thirteen pairs preserve outline `4C0+60*i` caller `1F2508`, then icon `9A0+60*i` caller `1F253C`; aliases are deliberately not deduplicated.
- The three trophy visits are conditional on actual retained zero/one visibility flags, with offsets `E80/EE0/F40` and callers `1F2584/1F25BC/1F25F4`.
- Marker `FA0` caller `1F2624` occurs once. First-frame image15 is explicitly only provenance help, not a live texture-map token or nine per-frame demand visits.
- Six stat groups use retained actual max counts3D60, row stride780, object stride60, caller `1F2668`, preserving row-major ordering.
- Maximum request count151 is exact: 1 portrait +26 outline/icon visits +3 visible trophies +1 marker +120 notches. Arrays and append bounds match that maximum. No background or numeric text request is invented.
- Static original bytes already checked at `0x1F24B0..` and additional static bytes `0x1F2620..1F26EC` agree with those calls/counts and final inherited call `0x1F26C4 ->0x141C30`.
- There is no popup gate in the source material-demand prefix. The new lease correctly does not import the separate draw gate from `0x1F2700`; `current()` only enforces profile generation/mode currency.

### Actual registry boundary remains unimplemented

Original static bytes `0x2DF8A0..D8` show the actual resolution chain: ScreenTexture+54 holds the material reference; its numeric row resolves through real material table382B38 at298E60; first pass+4C provides signed texture token+1A; the real texture table383400 resolves that token at298E60. Source1F24B0 then reads and conditionally raises **that same live TextureResource+10** to float10. Static `0x24E670..6D4` shows the subsequent demand-based resident selection call24E4A0 and source F4 write.

Current `OriginalStandardMaterialTable50LA` has genuine canonical row reads/population and model-surface demand, but its bounded population and representative-map APIs explicitly do not admit this unique UI texture-animation/resident domain. `OriginalStandardTextureTable50LA::resolve298E60(...mutable resource...)` is private and friend-gated to the material object, so a caller cannot legitimately use these immutable recipes to perform arbitrary mutable writes. No new access bypass, demand setter, caller-owned mutable TextureResource, fabricated handle, or resolution receipt is introduced by stage 10.

The demand records' image indexes and material-name strings cannot replace genuine ScreenTexture material references or the same retained tables. The inherited141C30 child traversal also remains unresolved: original source tests flag1000, child flags29/20, and dispatches eligible child virtual+40 in original list order. Two constants identifying that unresolved caller/target are evidence, not its implementation.

Thus stage 10 adds a useful concrete **source traversal lease**, but no completed registry integration. A future canonical consumer must acquire/populate the same application material and texture tables, resolve their genuine nonnull pass/map objects, preserve alias visits, conditionally raise real demand10 with original comparison semantics, and execute the genuine inherited widget traversal before reporting material-update completion. Neither `prepare()` nor requestCount/current success can certify that outcome or open frontend routing.

### Lease lifetime and API compatibility

- The class owns a `shared_ptr<const OriginalUiTournamentCareerResources50KG>`, pinning all actual resource/material name storage. No callback accepts externally supplied textures or handles.
- Preparation reads only canonical owner accessors introduced in stages8/9, checks owner currency before and after collection, builds a local fixed array, and publishes array/count/owner together on success. All failure paths leave the initially empty lease unpublished. A prepared lease rejects re-preparation.
- Every new request/count access checks the retained owner's actual generation/mode currency. A stale lease keeps immutable asset ownership until destruction but yields no new valid request pointer/count. `resources()` deliberately can return the retained owner even when stale; it conveys ownership, not live readiness.
- Material-name pointers point into the immutable fixed owner array; request pointers point into this lease's fixed array. Borrowed request pointers require the lease to live, while borrowed name pointers require a retained lease/resource. Old already-borrowed pointers do not become valid live-demand evidence just because their memory remains safe after profile mutation.
- New header uses existing `current`, `materials`, `medals`, `trophyImages`, `trophyVisibility4050`, and `statCounts3D60` signatures correctly. No CPP/build-list entry is required for the header-only class. It currently has no consumer, and the report accurately discloses that remaining join rather than claiming an applied update.

Combined verdict through stage 10: retain the narrow portrait fix, remove the erroneous Controls `.c_str()` change, preserve root's trophy separator correction, and retain stage9's notch separator fix. No other definite new source defect was identified in this continuation. No production change or execution was performed by this reviewer.
