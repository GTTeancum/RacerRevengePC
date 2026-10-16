# Time Stats native integration joins

2026-10-15. Static source inspection against 3d06b45100dc10bc3a30ebdf23b283629ecdbc53. No software execution, syntax check, image conversion, or gameplay validation.

## Actual owners already present

- `tests/native/original_camera_session50kx.h`: ActualOwners.preferences is the existing ControlPresets49D owner. Its slots[view].field10F94 is the original configuration offset 0x10F94, default7. Do not create a second configuration field or use incorrect offset0x110F94.
- Session owns raceFlags1B0_, uiFlags1E4_, and raceEntityFlags29_. The similarly named sceneFlags29_ belongs to the camera scene and must not substitute for race entity flags. `nativeHudVisible()` currently exposes only the UI bit, insufficient alone for source194FF0's complete visibility predicates.
- Actual camera scene/viewport identity and publication are already retained. A read-only HUD projection must preserve actual selected view and racer identity; the class58 discriminator must remain AI for every racer.
- `tests/native/native_ai_session50iv_fixture.h`: Session.rank[slot].placeCCC supplies original rank. Session.retired40.activeCount50 is the original manager+50, distinct from finished44.count54. Session.manager is the real original clock/flags owner.
- `terrain_interactive_drive50du.cpp`: original manager pass computes canonical gaps via computeOriginalAiManagerGaps and stores into lapContinuity.timing.valueCBC. Timing CB0/CB4/CB8 and lapCC8/flagsCD8 live in the actual banked racer. HUD must read that bank through stable read handles, never reuse a temporary arena row or hostRace's double clock/standings.
- Bound source row construction must happen after the accepted actual manager update and remain coherent with the displayed camera generation. Manager+44 view selection and race+48 player nibble must resolve the same genuine displayed racer; a constant selected index cannot certify this.

## Authentic assets

Original APPSTART.RES and LOC.RES recovered from retained original archive ZIP. Whole-archive hashes match existing catalog. Raw extraction only.
- ScreenMT.psm:748bytes SHA25634a6e34f523a4babe9e593037340ddf6c8ae15b6a7a174f349e688c2ead39982.
- English UI.loc:17688bytes SHA256c57ac0eebc9eba4742d7b9bc4a840ca6bb3de23b673353f7cc558d15d229f9b3.
- L_UI_ENG.bin:10637bytes SHA25648fa45093b373dc587637fb7656ad6cb7fb9c4a5d19956cd5b4c427887f1f4d2.
- English indices458/459/460/461 are LAP:/TIME:/LEAD:/BEST: respectively. Preserve indexed localization owner; these observations are not permission to hardcode replacement UI text.
- ScreenMT selects ScreenObject, alphaReference0, clamp1/1, texture mapping1, fog0, blending1, alphaTest0, depthMethod2. Blank fields inherit real constructor state.
- `stats.psm` uses ShaderPassGroup=unique. Native semantic equivalence already treats unique as a nonmerge exclusion. Keep the group's separate instance/order; do not relabel it none to make an existing renderer accept it.
- Visible font is eurostileboldbold13ingame with authentic CEL, PST and PSM. The loader also requests denmarkbold16ingame. Existing raceHudFontMissingPsmFallback is restricted to caller1A8DD4 and is unrelated. TimeStats16ingame is CEL/texture-only: no new missing-PSM fallback is warranted. Its visible13ingame texts resolve an authenticated existing PSM.

## Remaining integration work

1. Separate TimeStats lifetime owner and original six text/six texture state, actual source layout and transitions; keep existing lap-completion owner distinct.
2. Narrow authenticated resource lookup/demand facet on existing Application StandardTexture resources. Resource+10 is demand; direct floor10 is not projected demand104A00 and does not mean destruction.
3. Complete coherent bank/view/manager projection and source placement of update/demand/draw.
4. Original atlas/glyph renderer, preserving unique material grouping, real source depth/blend/raw-alpha rules and packet retirement before owner teardown. Existing lap-only RaceHud renderer cannot be treated as ready for this owner merely because it draws text.
5. Central permitted Windows build and visible smoke with real menu/rider/course/track screenshots and baselineFPS. None has run for TimeStats.

The prepared next Windows package remains exactly3d06b45; this research does not alter its source identity or claim HUD activation.

## Font metadata closure

Bounded raw CEL inspection found 46 glyphs and 46 metric records in the authentic eurostileboldbold13ingame CEL. All bytes required by the recovered English labels, digits, separators and literal gap prefixes 0x40/0x26 have authored glyphs and pixel metrics. No font substitution or invented metrics are needed. This is metadata evidence only; atlas pixels were not converted or rendered.
