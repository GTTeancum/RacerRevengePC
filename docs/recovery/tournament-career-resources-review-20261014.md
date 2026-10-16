# Tournament career resources stages 5 and 6 review

Date: 2026-10-14. Scope: source-only review of the supplied stage 5 patch and subsequent stage 6 patch against the current shared recovery tree. No patch application, implementation edits, builds, tests, syntax checks, imports, project scripts, guest execution, or native executable execution were performed. The original `SLUS_202.68` was read only as static bytes with `od`.

## Verdict

The stage 5 composition uses the canonical profile, stamp, page, pod, image owner, and glyph APIs correctly by inspection. The medal and selected trophy **image mappings** agree with the original. Stage 6 fixes the trophy flag attribution, but introduces a blocking missing material-provenance admission branch (R3 below). There is also a minor evidence-description correction. No compile or runtime success is claimed.

## Findings

### R1: trophy image indexes are incorrectly attributed to source visibility fields

The new header documents `trophies()` as “Source4050/4054/4058: earned championship trophy image, or unresolved.” The implementation stores image-bank indexes 8, 9, or 10 for completed championship courses and `0xffffffff` otherwise.

Original offsets `0x4050`, `0x4054`, and `0x4058` are **boolean trophy-present flags**, not image indexes:

- Entry `0x1EF854/858/85C` stores zero to those three fields. Static original words: `aea04050 aea04054 aea04058`.
- `0x1EFA30/38`, `0x1EFBC8/D0`, and `0x1EFD60/68` load integer 1 and store it to the relevant flag after the completed championship branch.
- The actual large trophy ScreenTexture objects are `0xE80`, `0xEE0`, and `0xF40`. The source selects their `trphglda/trphslva/trphbrza` resource by placement.
- Draw `0x1F27BC..0x1F2864` first tests the relevant flag against zero and then submits `page+E80`, `page+EE0`, or `page+F40`.

The bank-index representation is useful as a cold resource plan, and its image choice is correct. Correct the comment/API attribution to identify the actual ScreenTexture offsets and describe an owner-local image index, or introduce a separately typed trophy record with object offset, image, and explicit present flag. A caller must never treat the existing unresolved value as the source flag: `0xffffffff` is truthy and would display an unearned trophy. Do not repurpose the original visibility fields as image IDs.

Severity: medium source-contract defect, currently latent because this patch has no draw/session consumer.

Stage 6 status: resolved. The delta stores independent zero/one `trophyVisibility` flags and bank-index `trophyImages`, and replaces `trophies()` with appropriately documented accessors. Packet construction tests the explicit flag and only accesses a trophy image when present.

### R2: progress rejection is an additional native guard, not an original assertion boundary

`STAGE5.md` says rejection of progress beyond 13 preserves the source assertion boundary. Original `0x1EF88C..898` masks progress to 31 and compares the loop index with it; `0x1EFEC0..C8` bounds the loop to 13. A masked progress of 14..31 causes all 13 entries to use the completed path; it does not read a 14th medal. Original invalid-placement assertion paths are separately visible at `0x1EF9D0..A2C`, `0x1EFB68..BC4`, `0x1EFD00..D5C`, and `0x1EFE30..88`.

Failing progress above 13 is a reasonable strict native-domain choice, but describe it as an additional rejection policy. Do not claim it is necessary to prevent the source loop reading beyond the array or that the source itself asserts on that progress value.

## Canonical API checks

- `OriginalUiRiderProfileOwner50KG::tournamentCareerSnapshot(index, out)` exists publicly. Its exact fields match the patch. It copies career totals/progress and the diagonal `careers[index].pods[index]` medals/prizes, with `stamp()` from the same owner. Original entry keeps the supplied career in `s4`, computes the career base in `s0`, and indexes that same selected pod column in the medal loops, matching this diagonal selection.
- The snapshot's general profile gate is readable initialized ownership, range `<27`, and authored-present rider. The resource owner adds live mode 2; it does not expose a mutation path or manufacture a result commit.
- Canonical stamp is `{uint64 generation, uint32 mode34}`. The final load recheck and `current()` compare both fields. This matches the existing owner contract. Accessors intentionally expose retained snapshots even after `current()` becomes false; consumers must check currency. Stamp is not a thread synchronization primitive or renderer-admission token.
- Tournament archive and shared fonts archive gates use exact canonical identity pointers and loaded owners. `OriginalUiTournamentPageData50KG::load` and `OriginalUiRiderPodAssets50KG::loadTournament(archive, career, diagnostic)` have the exact signatures used. The stage 4 pod API maps valid careers to actual authored widgets 39..61 and rejects unsupported cases.
- `OriginalUiImageSource50KG` aggregate order is identity/member/PST offset/PST bytes/CEL. The patch supplies whole standalone PST members and an exact same-name CEL only for the font. `OriginalUiScreenAssets50KG::load` retains decoded provenance and publishes its immutable image/font bank atomically. Index 0 font and texture accesses match that API.
- All eleven named Tournament PSTs exist in the current authenticated Tournament catalog, and the font CEL/PST exists in shared UIFONTS. No counterfeit identity, generated glyph, substitute texture, or guessed archive member is introduced. The image IDs are distinct by canonical archive/member ordinal; they do not themselves establish material admission.
- Glyph output holds a raw atlas pointer, but the Storage pins the image owner. The successful unique_ptr publication does not move the atlas or font storage. Load failure leaves the resource owner empty/retryable; reload is rejected without replacing the previously published owner.

## Numeric and image semantics

Original constructor evidence resolves `denmrk14` at `0x1EDC38`; the three initialized ScreenTexts are at `0x3F00`, `0x3F70`, `0x3FE0`. Initializer calls are `0x1EDC68`, `0x1EDDC8`, `0x1EDF28` to `0x2DFD90`; the underlying default ScreenText constructor is `0x2E02F0`.

- `0x1EF108`: `%d` of unspent value (`s0+874`, canonical `2194`) into object `3F00`, origin `(550,176)`.
- `0x1EF5FC`: `%d` of earned value (`s0+870`, canonical `2190`) into object `3F70`, origin `(529,206)`.
- `0x1EF734`: `%d` of kills (`s0+878`, canonical `2198`) into object `3FE0`, origin `(473,236)`.
- Capacity 40, color `0x00cccccc`, scale 1, angle 0, and justification 0 agree with the construction evidence. Static word at `0x3C4738` is zero, matching canonical left justification. With this exact left/scale-1/angle-0 case, direct positioned origins are valid inputs to the local glyph primitive; no fake DAT widget identity is needed.
- Decimal conversion explicitly preserves the signed 32-bit `%d` interpretation without unsigned-to-signed narrowing. Every possible result has at most 11 bytes, safely below source capacity 40 and leaf capacity 255.
- Static bytes at `0x187C20..70` match the supplied category predicate: category 1/course 4, category 2/course 8, category 3/course 13. It does not read profile contents.
- Bank 0 is font; bank 1 is `medlgout`; banks 2..4 are gold/silver/bronze ordinary medals; banks 5..7 are gold/silver/bronze championship course icons; banks 8..10 are corresponding large trophies; bank 11 is authored `medlgicn` for uncompleted courses.
- Original completed placement branches accept 1/2/3 and select those corresponding gold/silver/bronze names. Patch formulas `1+place`, `4+place`, and `7+place` are correct for its chosen bank ordering. Uncompleted medals use 11, correctly matching `0x1EFEAC/B0`. Course medal ScreenTexture offsets `9A0+i*60` match source address arithmetic.

The image owner has no PSM/template/material-slot admission, medstar animation, portrait, ScreenTexture positioning, live draw, stat arrays, popup/save, registry, renderer retirement, or frontend handoff. The patch explicitly disclaims these. Loading resources must not open Tournament routing or imply rendering readiness.

## Required build integration

Root must add `platform/linux/native/original_ui_tournament_career_resources50kg.cpp` to both current actual compilation source lists:

1. `platform/linux/native/CMakeLists.txt`, adjacent to the existing Rider profile/Tournament page sources, using `${RR_ROOT}/platform/linux/native/original_ui_tournament_career_resources50kg.cpp`.
2. `tools/prepare_interactive_build50dv.py`, in its explicit source list, using `platform/linux/native/original_ui_tournament_career_resources50kg.cpp`.

No new separate source dependency was identified: profile, page, pod, font/image decode, and glyph implementation sources already occur in those lists. The header does not require a compilation-list entry. Recorder source tuples in `tools/gameplay/record_original_ui_navigation_30hz_recovery.py`, `record_original_rider_30hz_recovery.py`, and `record_original_course_roundtrip.py` are evidence manifests, not build lists; root should include the new pair in the appropriate provenance manifest if a later recording claims to cover these resources. Retained historical recovery scripts are not active build-list integration targets.

## Evidence and verification limits

Reviewed the supplied patch, canonical headers/implementations, current authenticated archive catalog, and prior retained Tournament construction/entry/draw text. Original static-byte spot checks used ELF load mapping `file_offset=VA-0x100000+0x80` for category code, resource strings, numeric construction, completed trophy branch, progress loop, visibility initialization, and justification word. No decompiler or supplied extraction script was executed. Parent owns patch integration, any corrections, tests, and runtime validation.

## Stage 6 continuation

Reviewed `0001-Admit-Tournament-materials-and-compose-original-meda.patch` as a delta atop the unapplied stage 5 text. Root had applied neither patch when this continuation was assigned.

### R3: new fallback origin is not admitted by the existing material whitelist

Severity: high, blocking the new load path.

Stage 6 appends `tournamentTrophyMissingPsmFallback` and implements `readOriginalUiTournamentTrophyFallbackMaterial50KG`, but does not add a corresponding branch to `sourceIdentityKnown()` in `src/assets/original_ui_material50kg.cpp`.

Exact call chain:

1. Career material loop reaches bank image 5 (`trphgldi`) and builds a pass with that new origin.
2. `prepareOriginalUiNativeMaterial50KG` calls `prepareOriginalUiOneMapMaterial50KM`.
3. That function rejects when `!sourceIdentityKnown(pass)` before preparing GS/native material policy.
4. The whitelist recognizes all existing finite fallback origins; the new origin falls through to the Main/Options prompt gate, where `(!main && !options)` returns false.
5. Career `load()` therefore fails with its material error and nested material `profile` diagnostic at `0x2471BC` before publication. Earlier authored-material failure could happen first; a valid earlier material cannot get past this first trophy.

Required correction: add a finite six-name provenance branch to `sourceIdentityKnown()`. Preserve its existing shared requirements: null authored pass identity, exact APPSTART ScreenMT inherited template, creation PC `0x2475C0`, pass 1, and empty authored section. For this branch additionally check canonical Tournament archive, null fallback-widget source, canonical `UI/UITourn/standing.dat` page source, canonical PST identity, exact texture name, exact registered material name and attempted PSM path, and the corresponding explicit source caller PC. The six builder strings currently use forward slashes; a new whitelist must compare the same representation or the builder must use the existing canonical backslash convention consistently. Do not weaken existing branches or admit all materials merely because their new enum value is present.

The bounded helper itself checks archive identity, exact enum range, ScreenMT/runtime ownership, genuine PST bytes, page identity, and absent PSM. These checks do not replace the downstream provenance whitelist.

### R4: raw borrowed packet pointers do not retain the resource owner

Severity: low, lifetime-documentation correction.

The new header says “Borrowed pointers retain this owner.” `medalPackets()` returns a raw pointer to the stored vector, and each quad's `materialName` is a raw `c_str()` pointer into a Storage string. Neither retains anything. Change the comment to require the consumer to retain the resource owner for all borrowed packet/name use. The implementation is stable while that owner lives: materials are a fixed array, all names are assigned before packet construction, and publishing the unique_ptr does not relocate Storage. No implementation lifetime defect was found within the cold owner itself.

### Six-name original fallback proof

The supplied helper is deliberately finite, not a synthetic generic fallback:

- Gold/silver/bronze championship icons: `trphgldi`, `trphslvi`, `trphbrzi`, direct original calls `0x1EF920`, `0x1EF964`, `0x1EF9A8`.
- Gold/silver/bronze awards: `trphglda`, `trphslva`, `trphbrza`, direct original calls `0x1EF938`, `0x1EF97C`, `0x1EF9C0`.
- Original resource strings and branch calls were checked during stage 5, including static bytes of the first championship block. The current authenticated Tournament catalog contains those six PSTs and no corresponding PSMs. It contains the actual standing page and authored denmrk14/outline/ordinary-medal/uncompleted-icon PSMs.
- Static original bytes at `0x2DF0B0..` show the ScreenTexture resource loader and explicit ScreenMT selection at `0x2DF10C`; `0x29B630..` shows the missing-config `-1` branch and vtable call at slot `+18`. Existing retained fallback evidence identifies that slot as `0x2475C0`. Static bytes at `0x2475C0..` match template resolution, one map, same-name texture loading, and the call to `0x2476A0` with null configuration.
- The delta reuses the canonical `screenTextureFallback()` builder, so it inherits genuine template state and explicit source defaults/runtime overrides without inventing an authored PSM or shader section. The appended enum preserves existing ordinal values.

After R3 is fixed, the career owner still admits only exactly one pass, one same-name texture, no authored TextureAnimation, and the unchanged existing bounded native policy. It rejects unsupported authored features rather than dropping passes or substituting a policy. The sampler is converted with the existing inline `convertOriginalUiMaterialSpan50KG(policy, binding, out)` signature and correct `{firstQuad=0, quadCount=1, drawQuadCount=1, textureIndex=i, textureCount=12}` field order. This validates CPU policy/sampler conversion, not source-frame ownership, destination-alpha promises, actual GPU publication, or readiness.

### Medal subsection geometry and gating

- The packet type uses the existing `OriginalUiOverlayQuad50KG`, whose canonical corner order is TL, TR, BL, BR. Stage 6 writes that order correctly, retains default authored depth 1 and source packet color `{127,127,127,63}`, and uses the actual authenticated atlas width/height for full-rectangle size and UV endpoints 0..1.
- Outline positions `{54,91,128,165,228,265,302,339,399,436,473,510,547}` at y314 match retained entry positioning; ordinary icons are outline+4 at y318. Earned championship icons/awards are outline-13 at y298. In the first championship branch, retained source `0x1F0334/338` and `0x1F03AC/B0` set both icon/award to the same x152/y298 values; static bytes of that block match.
- Object offsets and draw order match `0x1F27AC..0x1F2880`: outline `4C0+i*60`, optional earned award `E80/EE0/F40`, then course icon `9A0+i*60`, repeated 13 times. Capacity 29 covers 26 mandatory quads plus at most 3 awards. The exact subsection draw PCs are preserved.
- Original draw `0x1F2720..2C` reads popup object's `+29` bit 0 and suppresses this normal-page branch. `medalPackets(actualPopupFlags29)` applies that bit test and requires `current()`, so generation or mode mismatch returns null. The parameter is supplied by the caller; this helper does not authenticate ownership of a live popup. No current caller, source-frame seal, or frontend permission is added, so the future frontend must supply the genuine popup field and retain its existing route/lifecycle gates.
- `bankInput` uses the exact canonical input structure and pins APPSTART, Tournament, fonts, and the same immutable image owner. Its raw accessor is borrowed; the resource owner still must live. No combined image bank, remapping, GPU upload, or renderer lifetime drain is claimed or performed by this accessor.

### Stage 6 integration impact

The new load signature adds APPSTART and material runtime; any eventual stage 5 consumer must be updated. There is no existing production consumer of the unapplied new class. `original_ui_material50kg.cpp` is already in both actual build lists identified above; stage 6 adds no other new translation unit. The two stage 5 compilation entries remain required. Build/runtime validation and the provenance-whitelist correction belong to root.

## Root stage7 integration follow-through

Stage7 adds the exact six-name sourceIdentityKnown whitelist with canonical
Tournament archive/page/PST identities, matching forward-slash registered and
attempted names, exact original callerPCs and absent PSM requirement. The common
ScreenMT creation/pass/empty-authored gate remains. Root inspected the new
branch and combined stages5-7; fixed borrowed-pointer lifetime wording and
marked progress>13 rejection as stricter native admission. Original accent
color is used for outlines, unearned icons and visible large trophies.
The new CPP is registered in CMake, Windows manifest and build generator;
recorder source-identity lists were updated too. No build or test ran.
Full screen/session and save/popup/retirement admission remain incomplete.
