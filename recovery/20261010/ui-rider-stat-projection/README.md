# Rider custom stat/text reconstruction (source-only staging)

## Deliverable and boundary

`original_ui_rider_stat_projection50kg.{h,cpp}` is a NEW bounded, non-rendering native projection of `1EBBF0..1ECBE0`, with the stat conversion leaves `188D70/188DE0` and original color-update observations from `1EB640`. It stages at most 156 ordered custom draw observations. It does not fabricate a UI, create textures, load profiles, change the repository, or claim screen parity. No build, syntax check, tests, project parser, importer, sanitizer, gameplay, or guest execution was run. The parent owns all validation and integration.

The adjacent static disassemblies came directly from ELF bytes using the provided decoder. Decoder output labels some instructions UNKNOWN (including deliberate BREAK assertions and EE-specific instructions); no guest instruction was executed. Source arithmetic interpretation must account for branch delay slots and EE MULT writing rd.

## Exact source data dependencies

Let `P = *( *(0x382B80) + 0x2DC )`, `R = page.word4C4` (original rider index, NOT navigation slot), and `row = P + 0x1920 + R*0x894`.

- mode = P.word34.
- unlocked = P.word(0x10FAC + 4*R). Render directly reads this word; do not substitute a different unlock helper with mode-specific behavior.
- accent packed color = P.word(0xA8 + 8*R), leaf `17FA00`.
- secondary packed color = P.word(0xAC + 8*R), leaf `17FA10`.
- current stat i float = row.float(0x50*R + 4*i), i=0..5. Call at 1EBBB4 passes the SAME R in both a1 and a2. This diagonal lookup is intentional and must not be simplified into a single rider stride.
- maximum stat i float = P.float(0x408 + 24*R + 4*i).
- medal i byte = row.byte(0x18 + 0x50*R + i), i=0..12. Value1 draws gold, value2 silver, value3 bronze; all other values draw outline only.
- career marker track = row.byte(0x87C) & 31, equivalently P.byte(0x219C + R*0x894)&31. This is distinct from the medal matrix.
- Each count is `CVT.W.S(0.5f + 20.0f * sourceFloat)`, with the multiply and add materialized separately, then an upper cap20. Existing text metric conversion establishes the native EE CVT.W.S finite-domain truncation contract; the new leaf reuses it. Original has no lower clamp. Projection rejects a negative converted count rather than silently changing it.
- The caller must establish authoritative profile readiness and bind all supplied observations to the same R. The projection does not manufacture zero stats to cover a missing profile.

## Render layout and order

Coordinates below are original local screen coordinates before renderer/video projection. Y always includes live float page+4DC except the explicitly hidden marker case.

If mode34==2 AND unlocked:

1. Thirteen medal outlines: object page+0x540+i*0x60; x=260+22*i+gap(i), y=245+panelY. gap=0 for i<4,12 for i<8,24 otherwise.
2. Immediately after each outline, its earned medal if value1..3: page+0xA20+i*0x120+(value-1)*0x60, same position.
3. Career marker page+0x18C0. For track<13, x=254+22*track+gap(track), y=239+panelY. For track>=13, source sets x641,y449 without panelY. Do NOT clamp track to12 or draw an invented last marker.

In every mode, including locked:

4. Existing base widget table index3 / widget ID4 gets position x460, integer CVT.W.S(320+panelY), from 1EC304 or 1EC5BC. Its actual draw remains base widget-owned.
5. `podstat` page+0x2340 at x220,y=(mode2?244:251)+panelY. Mode2 sets height135; other modes retain construction/current height.
6. `podstat2` page+0x23A0 at x348,sameY. Width292 always; mode2 height135. Other modes preserve height. Projection width/height zero means preserve original constructor/current dimension, not literal zero.

Only if unlocked:

7. `%02d_flrg` for R, page+0x1920+R*0x60 at x250,y300+panelY.
8. Six notch rows, each draws exactly maximumCount[i] objects from page+0x2400+i*0x780+j*0x60. x450+7*j, y271+18*i+panelY. Width5,height10 are set during construction. Objects j<currentCount[i] use accent color; others use packed0x333333. Source also repositions currentCount objects but draws only maximumCount objects; do not draw up to current when current exceeds maximum.
9. Six label objects page+0x5100+i*0x70 at x445,y268+18*i+panelY. They use RIGHT justification, so x445 is the right anchor, not left glyph edge. Source draws all six after all notch rows.

Color update 1EB640:

- Secondary color to base widget IDs2,1,7,6.
- Accent to IDs87,88,89,90,4,117,118,119; IDs88..90 use their icon-specific setter.
- Accent to two panels, six custom text objects, thirteen medal outlines and earned notches.
- No source color refresh of earned medal gold/silver/bronze, portrait, or animated marker in this range; preserve their constructor/material state.
- Explicit sprite RGB packet components are `(packed >> 16,8,0 & 255) >>1`. Grey0x333333 becomes25, not51. Do not normalize the source color a second time. The projection carries packed color only and does NOT infer alpha.

`1ECBF0..1ECDDC` visits material state for two panels, thirteen outlines plus three earned medal slots each, selected portrait, and all6*20 notch objects. Through2DF8A0 it clamps material float+0x10 to at least10, then calls base routine141C30. This is NOT a geometry coordinate clamp or label metric. The caller must preserve the original material ordering/depth behavior rather than treating this projection as a complete owner.

## Actual text, font, image and material owners

Creation at1E81BC resolves font name `denmrk14` through the shared font manager. Six constructors at1E8280,1E83D0,1E8520,1E8670,1E87C0,1E8910 receive localized text-table entries200..205, capacity20, the same font and material name. `0x3C4758` is integer4, stored as each object's justification. Use `OriginalUiEnglish50KG::at(200+i)` against authenticated LOC.RES; do not substitute English strings or assume their spelling. No decoded localization bytes were available in the supplied evidence, and no asset was decoded here.

Reuse existing owners:

- assets/original_ui_localization50kg.{h,cpp}: LOC.RES / loc/english/UI.loc + L_UI_ENG.bin.
- assets/original_ui_font50kg.{h,cpp}: authenticated shared UIFONTS font/denmrk14.cel and same-name PST.
- game/original_ui_text50kg.{h,cpp}: real glyph metrics and atlas UVs.
- game/original_ui_text_position50kg.{h,cpp}: metric conversion and right justification. Custom live panelY is float; do not discard the fraction by coercing it to an authored integer descriptor before geometry. Source custom text has its own ScreenText owner, not a DAT widget ID.
- game/original_ui_text_geometry50kg.{h,cpp}: shared cold geometry logic, with a deliberate custom owner adaptation for live panelY. Current main renderer checks live positions against cold geometry, so merely supplying a moved descriptor to existing static widget geometry will reject. Regenerate/transform using the original leaf's rounding order, not arbitrary host glyph offsets.
- assets/original_ui_material50kg.{h,cpp}: original ScreenMT/PSM provenance, multi-pass and GS behavior.
- platform/linux/native/original_ui_main_draw50kg.cpp appendOverlay / text packet building: native vertex ordering, material/image binding, original atlas, packet color, animation and ordered submission. Reuse this, not a new flat-color or browser-style renderer.

Authenticated Rider UI/UIRIDER/UIRIDER.RES members in the existing archive catalog:

- ui/textures/podstat.pst at0 size17436; podstat2.pst at147456 size17436; notch.pst at172032 size1308.
- medsmicn.pst at196608; medsmgld.pst221184; medsmslv.pst245760; medsmbrz.pst270336, each size2076; corresponding authored PSMs at1548288,1572864,1597440,1622016.
- medstrsm.atm at294912 size18696 and medstrsm.psm at1646592 size2775. Preserve original texture animation owner; never replace it with a static invented marker.
- `%02d_flrg.pst` for authored R values1..26 excluding5,6,13. The creation loop matches those exact omissions. `%02d_bkg` is a separate character/background route, not the stats portrait.
- font/denmrk14.psm at1474560 size990; font CEL/PST comes from shared UIFONTS, not a fabricated local atlas.

CRITICAL MISSING-PSM ADAPTER: The full archive catalog has NO podstat.psm, podstat2.psm or notch.psm. Sprite constructor2DF0B0 resolves the requested name with ScreenMT at global3833F0 (not DefaultMT), flags15. These require a source-attested missing-PSM fallback path. Existing material provenance only exposes mainLogoMissingPsmFallback / mainPromptMissingPsmFallback; do not mislabel Rider assets with those identities or bypass authentication. Extend the bounded provenance/caller contract after parent validation of the common material creation fallback. The projection intentionally does not fabricate material ownership.

## Remaining integration work

1. Parent validates staged code and independently checks addresses/conversion boundaries; no validation results are claimed here.
2. Bind a current profile observation owner (read-only), same R as navigation/controller, including source accent and secondary colors and career bytes.
3. Bind the source custom sprite/text construction, image/material ownership and 1ECBF0 material phase; authenticated missing-PSM ScreenMT adapter is required.
4. Feed panelY from original page+4DC controller phase; retain float rounding order and original visibility/draw phases.
5. Supply actual localization200..205 and authentic font atlas, then submit through existing native rendering and shared text geometry. A successful projection alone does not make the Rider page visually complete.
