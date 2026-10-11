# Actual Rider19 geometry capacity continuation

Apply AFTER frozen composer and its dependencies. Two exact active preimages (original_ui_draw50kg.h, original_ui_lit_geometry50km.h) and two exact frozen-composer preimages (draw contribution CPP, renderer CPP). Frozen composer files are not modified.

Independent authenticated PSG byte research at /workspace/shared/recovery-20261011/rider19-frame-budget/counts.json proves:
- Both original19.psg and19c.psg have one LOD.
- Pod authored triangles across all passes:4272x2 +414x2 +356x1 +53x3 +2x1 +1489x2 =12867.
- Character:1402+1124+563=3089. Models total15956 triangle slots/14 spans.
- Conservative all-widget/stat/fixed source-capacity ceiling1033 slots/229 spans includes mutually exclusive texts, dynamic text capacities, real stat labels/sprites, original binder pair, four backgrounds, two apertures, four prompts and exit text.
- Full bounded Rider19 population16989 slots/243 spans. No geometry simplification, pass omission or guessed placeholder count.
- Pod surface0 contains6126 original vertices; the prior4096-entry lit scratch bound rejected that genuine surface.

Changes:
- Preserve ordinary UI8192-slot constant/admission.
- Add separate sealed source-RGBA17408-slot capacity, exceeding the16989 authentic upper bound while staying finite. Source renderer allocates this larger GPU vertex/W/fog/index capacity only when enableOriginalUiSourceRgba is explicitly set. Ordinary Main/Options renderer allocation remains8192.
- Use that source capacity in the actual Rider composer. Span limit remains2048, comfortably above243.
- Widen source lit surface scratch4096→8192 vertices. Triangle bound8192 is unchanged; source surface maximum4272 fits.
- Existing renderer uses uint32 indices, so no16-bit truncation is introduced.

This is native storage sizing for the complete original geometry, not device emulation. It grants no destination readiness. Root alone integrates/builds/tests. No helper build, syntax check, project parser, gameplay or executable ran.

Root integrated this source after the 03:17 build failure. It remains unbuilt and untested; Rider destination activation is not included.
