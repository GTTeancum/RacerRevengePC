# Pause shared base font and text storage

Source-only continuation against the frozen backdrop-storage patch plus its
separate dirty-bit correction. No builds/tests/syntax checks/decoder execution.

## Actual source font request

Static original executable words at382C70 and382C78 both contain pointer3AC9B0,
whose literal is denmarkbold16ingame. Source1B8BC4 invokes282430 with count1 and
reload0, then1B8BDC invokes282210 for that same name. This is the existing actual
Application328 registry, not a new Pause font manager. A private
loadPauseBase1B8BC4 wrapper reuses its existing loadNames282430 implementation for
exactly one denmark16 request, preserving source insertion/reuse and population.
Only the concrete Pause storage is friended; no arbitrary public font-load API.

`loadBaseText1B8B9C` requires that same registry's retained domain and construction
context. It keeps the existing registry alive and borrows its denmark16 Entry,
checking CEL/PST identity against authenticated Pause source data. Aliased archive
shared ownership pins the exact archive object; it creates no archive clone or
extra source texture retain. Source+1F0 is cleared before the font load at1B8BA4.

## Concrete+180 ScreenText

2DFD90 initializes capacity40 and2 batches.2DF8E0 allocates string41,40 glyph
pairs, primitive array2, packet-entry array2, then packet/primitive per batch in
source order. Counts36/4 glyphs yield72/8 vertices. Each glyph's native alias
points into its actual packet storage. References5/6 are retained only after each
primitive allocation. There is no invented population between text primitives.
The actual36B970 is scoped to font\\ and restored before loader color/alpha
setters, including every failed return and exception.

Source2DFBA0 authored pair vertices start[0,0,1,1];2E03F0 packets start with STQ0
and XYZ[0,0,1,0]. Source initial empty text is preserved. The loader writes
RGB127/alpha63, cachedcolorFFFFFFFF/alpha255. Then base source1B8C50/5C positions
x320/y200;1B8CA4 assigns justification3 from global3C4750;1B8D30 applies constructor
color170=00CCCCCC (packet RGB102, alpha unchanged);1B8D58 sets scale40 to exact
bits3F99999A. No glyph XY regeneration, host labels or draw readiness is implied.

Important flags detail independently read from the full executable:2DFDB0..DD8
sets bit3;2DFC60 only tears down old resources and never clears it. Final
2E0108..144 sets bits0/1 and clears bit2 only. Thus this cold text retains low
flags0B. It does not copy the existing PositionStats helper's flags3 simplification.
No existing HUD code was changed by this continuation.

## Partial cleanup and next boundary

Failure preserves allocated arrays, reached numeric references and actual shared
font-manager effects. Original base teardown order remains polygons, then+180
text, then base textures.2DFC60 frees string/glyph storage, iterates primitive
references in ascending order, then packet storage. Borrowed font lease and
registry handle are released without an extra per-text source font retain/free.
This remains a cold-only object incapable of submitting GPU work; future renderer
integration must establish actual retirement before allowing cold cleanup.

Stage baseText represents completion of this shared base storage segment through
1B8DA4, not the derived Pause loader1BA390. Next is the second genuine one-name
font request1BA3E4 followed by icon+890 and the thirteen derived texts, arrows and
source conditional text setup. Entity attachment, canonical FINAL projection,
render1B9220/1BB010, input1BD7C0 and driver activation remain gated.
