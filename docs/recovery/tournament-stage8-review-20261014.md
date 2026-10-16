# Tournament stage 8 source-only review

Reviewed 2026-10-14 against frozen repository HEAD `03e604b4c2c7b5d7ed276628453557c067b24b17` and supplied stage 8 patch `364b60dfd362c321a5cf45ae407ba266c681c7b5`. This report is outside the repository. No frozen source edits, patch application, commits, builds, tests, syntax checks, imports, scripts, or project/guest executable launches were performed. Shell operations were limited to reading source/evidence and static original bytes.

## Verdict

Blocking material lookup defect: the new notch provenance whitelist cannot find its canonical texture/page because it uses forward slashes with a helper that only folds case. The existing stage 7 trophy whitelist in the frozen tree has the same defect. Stat counts, resource selection, geometry, appearance, draw order, owner lifetime, and public API use otherwise match by source inspection. Runtime success is unverified.

Update at 21:01 UTC: root corrected the existing trophy-only separator defect in commit `1d74e952de3aacd85bafbdc5e709f1211e3d400f`. Read-only inspection verified canonical backslash page/PST/PSM lookup arguments and unchanged exact forward-slash provenance strings. Stage 8 remains unapplied; its notch whitelist still needs the same narrow correction. No test ran; central release testing is awaiting the requested user approval.

## S8-R1: canonical catalog lookups use the wrong separators

Severity: high; blocks successful career resource loading.

`catalogMember(id, name)` in `src/assets/original_ui_material50kg.cpp` iterates canonical catalog pointers and compares names through local `same()`. That comparison folds uppercase/lowercase only, preserving `/` and `\\` as distinct characters. Authenticated catalog member names use backslashes.

The new notch whitelist uses:

- `catalogMember(id, "ui/textures/notch.pst")`
- `catalogMember(id, "UI/UITourn/standing.dat")`
- `catalogMember(id, "ui/textures/notch.psm")`

The first returns null despite the real `ui\\textures\\notch.pst` being present. Its required `texture && ...` predicate therefore always fails. `prepareOriginalUiNativeMaterial50KG` calls `sourceIdentityKnown()` through the one-map preparation path and rejects the genuine notch pass with material `profile` at `0x2471BC`. The absent-PSM test alone is not reliable with mismatched separators either.

Additionally, the existing frozen stage 7 trophy branch uses the same forward-slash page/PST lookup strings. Its page comparison rejects genuine builders first; it also cannot find any of the six PSTs. Thus a career load already fails at its first trophy bank entry before reaching stage 8 notch admission.

`OriginalUiArchive50KG::find()` **does** fold separators. Consequently the builder successfully finds its real texture/page, but the downstream catalog-only whitelist rejects those exact pointers. This mismatch is not an absent asset or decode failure.

Recommended narrow correction: use canonical backslashes in **catalog lookup arguments** for both trophy and notch whitelists. The forward-slash provenance strings can remain as-is if their exact comparisons match their builders. For the trophy loop, form a separate canonical backslash catalog stem for PST/PSM lookup while preserving the existing registered-name/provenance stem, or consistently change builder and whitelist provenance spelling together. Preserve all finite source identity checks; do not bypass material policy validation or globally loosen unrelated comparisons.

## S8-R2: stage 8 header hunk expects superseded lifetime text

Severity: integration note, not a new implementation defect.

The stage 8 header delta removes “Borrowed pointers retain this owner; no frame seal or GPU submission.” Frozen HEAD already replaced it with the correct three-line borrowed-lifetime explanation. The material-array/stat-accessor hunk needs reconciliation against that current header rather than reintroducing the old text. This was found by text inspection; no patch applicability check was run.

## Stat arithmetic and source correspondence

- Canonical `statSnapshot(selected, 0, stats)` exists with the exact signature and fields used. It requires actual readable initialized profile ownership, authored-present rider, finite panel Y, and returns current abilities from `careers[selected].pods[selected]`, authored maxima `maximum0408`, selected/observed rider equality, and current mode. Original Tournament source supplies the same career as both career and pod arguments at `0x1F1F18/1C`.
- Maximum function `0x188DE0` reads authored `0408`; current function `0x188D70` reads `1920`. Both multiply by binary32 20, add binary32 0.5, convert to signed integer, and cap counts above 20. Static original words `0x188D70..0x188E3C` match the supplied transcription.
- Separate volatile float product and sum preserve the two arithmetic steps. The existing canonical `roundOriginalUiTextMetric50KG(rounded, 1, output)` performs finite/range checks and truncation; multiplication by exact 1 does not change finite binary32 values. Output remains bounded to 0..20. Rejecting a negative **converted count**, unrepresentable/nonfinite input, or current count above maximum is an explicit strict native policy; it does not manufacture extra source objects.
- The report's claim that “Negative ... reject” should mean negative converted counts. Slightly negative input values can round/truncate to zero, just as in the existing Rider count helper; the new code does not independently reject every negative input float. This is a wording clarification, not a blocker for authenticated profile data.
- Source max counts are stored at `3D60+4*row`; the new exposed six-element count array stores those maxima, not filled counts. Original loop draws exactly that many objects.
- Packet offsets `1060+row*780+n*60` match source row stride 1920 and object stride 96. Six rows of at most 20 produce at most 120 packets, with no out-of-bounds bank/material access.
- Row Y is `164+18*row`, from original constants at `0x1F13B0/BC` and arithmetic at `0x1F13D0/E0`. Notch X is `156+7*n`, from `0x1F1670..1690`. Width 5 and height 10 are explicit original overrides at `0x1F154C..1568` and `0x1F15DC..15F8`, so these are not inferred from atlas shape.
- Appearance first initializes all created notches to `0x333333`, then `0x1F1F10..1FC8` colors the first current-count objects with selected `17FA00`. The patch uses the same accent already obtained from canonical profile colors and the canonical RGB-right-shift-one/alpha-right-shift-two packet conversion with alpha 255. Static bytes of the original appearance loop match retained evidence.
- Draw order is row-major then notch-major at original `0x1F2778`; stat subsection follows the portrait and precedes medals. The API comment correctly preserves this boundary rather than presenting the list as a complete frame.

## Finite notch material admission

The authenticated Tournament catalog contains `ui\\textures\\notch.pst` (1308 bytes, source hash `e7d65fd8b3029f3c800b89888dfdb3609762010756076f38484452a80d88b422`) and contains no corresponding notch PSM. Static original bytes at `0x3AF6B8` spell `notch`; `0x1F1424` calls the real ScreenTexture loader with that resource string. The existing recovered ScreenTexture/ScreenMT missing-PSM builder path supports this exact case.

The new helper separately identifies Tournament notch, checks exact authenticated Tournament archive, actual PST data/size, real standing page, absent PSM, exact ScreenMT template/runtime, and records caller `0x1F1424`, creation `0x2475C0`, and same-name registered/attempted paths. It reuses the canonical builder without synthesizing a PSM identity or authored shader section. The new origin is appended, preserving previous origin ordinals. Unlike stage 6, stage 8 includes its whitelist branch; S8-R1 concerns that branch's exact catalog lookups, not its omission.

Once separators are corrected, common source admission still checks null pass identity, exact APPSTART ScreenMT inheritance, pass 1, creation PC, and empty authored section. The owner requires one same-name standalone map, no animation, and unchanged supported native material policy. It converts sampler policy with updated texture-bank count 13. No general synthetic fallback, fake pass, substituted shader, or dropped authored feature is introduced.

## Ownership, gating, compatibility

- Materials grow from 12 to 13 in Storage, header return type, method definition, loop bound, and conversion count. Existing bank entries 0..11 stay unchanged; notch is exactly 12. The name array includes all 12 non-font resources. Image sources and materials remain aligned.
- `OriginalUiTournamentStatPacket50KG` aliases the existing packet shape. Quad corner order remains TL/TR/BL/BR with UV 0..1 and canonical default depth. Geometry is explicit 5x10; the atlas still comes from authenticated decoding.
- `materialName` points into a fixed Storage material string. Names are initialized before packet creation and are not subsequently mutated. Vector growth moves packet records but not their referenced string storage. Unique_ptr publication does not relocate Storage. Borrowed pointers are valid only while the caller retains the resource owner; the corrected header acknowledges this and requires retention through retirement.
- The added stat snapshot is gathered after the initial career snapshot/colors. Final generation/mode equality recheck catches intervening canonical profile mutations before publication. This is the existing single-owner application contract, not thread synchronization.
- `statPackets(popup)` requires `current()` and rejects popup bit 0, matching the normal-page suppression branch. The caller must pass the real popup flag; no lifecycle/route authorization is fabricated. `statCounts3D60()` is a borrowed immutable snapshot accessor, like existing career/medal accessors, and remains available after staleness. Consumers must check currency before using it as live state.
- All new allocations occur before publication and are enclosed by the existing catch-all failure boundary. A failed fresh load publishes no partial assets or packets; existing loaded ownership is not replaced.
- Existing profile, text-position, color conversion, image, template, material, and sampler APIs match the patch by inspection. No current production consumer of the career class was found; future explicit `array<...,12>` consumers must adopt the changed materials return type. No new translation unit or build-list entry is required beyond the already integrated career resource CPP.
- The patch remains a cold subsection owner with no frame seal, route admission, full-screen rendering, material remap/submission, save/popup owner, or renderer retirement receipt. Existing frontend gating must remain closed.

## Limits

The material separator blocker was reported immediately to root. The header reconciliation is documented here as an integration note. This report does not attest build, material decode, numerical all-input EE parity, load success, or renderer success. Central testing, source fixes, and integration remain root-owned.
