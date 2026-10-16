# Pause primitive reference construction facet

Source-only extension to original-pause-owner-20261016.md. No builds, syntax
checks, tests, parser/decoder runs or guest execution were performed.

## Exact count and order

The original load reaches 36 separate canonical primitive references:

| Native index | Original field | Source material | Source call |
|---|---|---|---|
| 0 | A0 | results1 | 1B8840 |
| 1 | 100 | results2 | 1B898C |
| 2 | 40 | results2 | 1B8AD8 |
| 3,4 | 38,3C | LetterBoxPause | 1B8E10 loop |
| 5,6 | 180 | denmarkbold16pause | 1B8C08 |
| 7 | 890 | icon_xpause | 1BA3FC |
| 8,9 | 2D0 | denmarkbold16pause | 1BA4D8 |
| 10,11 | 340 | denmarkbold16pause | 1BA5F4 |
| 12..33 | 3B0+i*70, i=0..10 | denmarkbold16pause | 1BA8B0 loop |
| 34 | 210 | arrwleftPause | 1BACBC |
| 35 | 270 | arrwrghtPause | 1BAD50 |

Fourteen texts own 28 references: 2DF8E0 splits capacity into at most36 glyphs
per primitive, then 2DFAB0 invokes primitive virtual+0C with type15 and actual
ScreenMT3833F0. Capacity40 gives36/4; capacity60 gives36/24. This is allocation
capacity, not string length or currently emitted glyph count. See the raw words
2DF9E0..2DFB00 in position-full-resource-source-20261015.txt and the existing
native PositionStats resource owner. Four vertices of kind1 LetterBoxPause each
fit one original primitive, giving two references in the two-element loop.

## Implemented boundary

OriginalPausePrimitiveReferences50LA owns material00/start/retain state for the
36 actual primitive slots. acquireNext only associates the next source slot; it
cannot accept caller-selected rows or reorder associations. Actual directory,
Application, canonical domain and retained source-data owner must agree. Authored
PSM bytes must match the same sealed provider. results1/results2 must have absent
PSMs in BOTH authenticated OVERLAYS and mounted provider, with exact PST bytes
present. A row from another mounted PSM is therefore not silently treated as the
original fallback. Canonical find298C80 and retainScreen14D590 remain the real
registry path; no synthetic OriginalUiMaterial or GPU policy is created.

material00 is stored before retain. Failure leaves earlier retained slots and
current started/stored state inspectable and stops further acquisition. Release
is explicit and idempotent per slot; starting teardown prevents later acquisition.
Destructor terminates if numeric references remain, forcing the eventual source
owner to perform actual cleanup rather than silently releasing before GPU drain.
All runtime entrypoints remain uncalled. The separate driver include patch only
makes the three cold headers visible to the root's next actual build.

## Required integration and remaining gates

This is a constructor/load MATERIAL REFERENCE FACET, not completed Entity or
ScreenObject ownership. The concrete Pause owner must call acquireNext only at
the reached source299BE0 boundary after the corresponding original allocation.
It must finish each ScreenTexture2DF144/2DF150 population pair before proceeding
into the next load, preserve the font registry borrow and directory restore, and
implement original geometry/alias/dirty initialization. It must release slots in
the original destructor order after genuine renderer retirement. Failed cold
construction must use this same partial teardown, not reset or retry the object.

results1/results2 population will use the canonical table's existing genuine
missing-PSM constructor path after ScreenMT association; native raster parsing
still needs dedicated exact fallback provenance admission if it uses the legacy
OriginalUiMaterial50KG policy API. The new facet makes no such admission claim.

After actual population and collector seal, the concrete primitive's retained
material00 must resolve to its FINAL representative and actual source texture or
zero-map state. There is no collector or native readiness shortcut here. Shared
1B9220/derived1BB010 draw, source1BD7C0 callbacks and driver lifecycle remain
required before replacing the host Pause page. No current gameplay path changed.
