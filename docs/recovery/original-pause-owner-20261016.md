# Original in-race Pause owner: cold boundary and integration contract

Source-only recovery against native base `0b31d18`. No build, test, syntax check,
asset decoder execution or gameplay was performed. This document does not claim
an implemented owner, completed original callbacks, or a drawable Pause page.

## Result

The original Pause owner is the programmatically constructed `1BA130` object at
race `+100`, with derived table `3CF420`. Its load is `1BA390`, shared base load
`1B8810`, and parent draw is the `1BAF60 -> 1B9220` trampoline. Reusing the native
Options DAT or relabeling `HostSessionMenu50IF` would not implement this owner.
The full executable is available; the remaining work is semantic translation and
native ownership/material integration, not an unavailable loader fragment.

## Evidence basis

Authenticated source executable supplied for static reading: `SLUS_202.68`,
SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
File offset for the ranges below is VA minus `0xFFF80`. Recovery read little-endian
instruction words and rodata only; no instruction stream was executed.
Existing corroboration: `portal-join-next-20261014.md`, original UI archive catalog,
native ScreenTexture/ScreenText resource and material owners. Source addresses are
hexadecimal, capacities and localization IDs below decimal unless prefixed `0x`.

## Parent ownership and dispatch

- Race construction clears `+100/+104`. At `1A8A40` the Pause allocation is `0x8F0`,
  constructor `1BA130` receives A1=1, and `1A8AB8` stores race `+100`. The caller
  invokes this owner's slot30 load and slot1C. The separate `+104` object has
  constructor `1BE940` and allocation `0x18C0`; it is not Pause.
- `1BA204` installs `3CF420`. Slots: 08=`1BA2C0`, 10=`142590`, 18=`141520`,
  1C=`1415A0`, 30=`1BA390`, 38=`1B9FE0`, 40=`1BAE00`, 48=`1BAF60`,
  64=`1BD7C0`, 74=`1BB010`, 78=`1BAF70`.
- `1415A0` recursively changes flags29 bits0/1, excluding hidden-bit3 descendants.
  This is not merely a host bool permitting an arbitrary frame.
- Race teardown detaches with `141A70` at `1A695C`, calls slot10 at `1A6970`,
  and clears `+100` at `1A6978`.

## Complete load inventory

`1BA3B8` invokes `1B8810(this,1,0,12)`. The shared loader returns at `1B8DA4`;
its initial `1B8828` call is `141C20`. Do not erase that lifecycle boundary.

| Owner field | Source call | Operation / argument |
|---|---|---|
| +A0 | 1B8840 | ScreenTexture `2DF0B0`, `results1` |
| +100 | 1B898C | ScreenTexture `2DF0B0`, `results2` |
| +40 | 1B8AD8 | ScreenTexture `2DF0B0`, `results2` |
| +38, +3C | 1B8B94 -> 1B8DB0 | Two original screen primitives, `LetterBoxPause` |
| +180 | 1B8C08 | ScreenText `2DFD90`, `denmarkbold16pause`, capacity40 |
| +890 | 1BA3FC | ScreenTexture `2DF0B0`, `icon_xpause` |
| +2D0 | 1BA4D8 | ScreenText `2DFD90`, `denmarkbold16pause`, capacity60 |
| +340 | 1BA5F4 | ScreenText `2DFD90`, same name, capacity40 |
| +3B0 + i*70 | 1BA8B0 | Eleven ScreenTexts, same name, capacity40 |
| +210 | 1BACBC | ScreenTexture `2DF0B0`, `arrwleftPause` |
| +270 | 1BAD50 | ScreenTexture `2DF0B0`, `arrwrghtPause` |

The font-resource lookup at `1BA4AC` is `282210` with `denmarkbold16ingame`;
its result's first word (or null when absent) is passed as the font argument to
`2DFD90`. The pause material name and ingame font resource are distinct source
inputs. Calls `282430` at `1BA3E4` and in the base loader use the actual Application
`+328` resource registry; a private replacement registry is not equivalent.

The derived text loop limit at `1BA95C` is eleven. Literal rodata names:
`3AC9B0` denmarkbold16ingame, `3AC9C8` results1, `3AC9D8` results2,
`3AC9F0` denmarkbold16pause, `3ACA08` LetterBoxPause, `3ACA90` icon_xpause,
`3ACAA0` END TIME TRIAL, `3ACAB0` arrwleftPause, `3ACAC0` arrwrghtPause.
Localization `18AC60` calls in load use IDs4 (`1BA748`),8 (`1BA974`),401
(`1BAA18`),424 (`1BAABC`),425 (`1BAC14`). `END TIME TRIAL` is conditional,
including an actual Application `+2DC` dependent field at `+0x11258`; do not
populate it unconditionally. Use authenticated localization instead of guessed
English labels or host menu string order.

## Geometry, material and animation requirements

- Base load writes original geometry/cache aliases and dirty bits. SourceZ is
  derived from real Video global `443A60`, field60, including subtraction; do not
  copy the Damage HUD's depth constant merely because both are UI.
- `1B8DB0` allocates two `0x50` primitive objects using `2E8D90/2E1920`, stored at
  +38/+3C. `1B8E10` calls `2E1AE0` with `(0,4,1,"LetterBoxPause")`. The four-vertex
  loop distinguishes primitive type5 stride60 from stride30, calls `133F20`, and
  writes raw alpha63 through vertex color aliases. This is an actual original
  polygon/material join, not permission to draw invented host letterboxes.
- Source arrow setup writes RGB127 and cached packed `0x00FFFFFF`. That does not
  establish an independently invented alpha128 or host white255.
- Update `1BAE00` reads Application+264/+0C, looks up denmarkbold16ingame and its
  resource through `282210/281F90`, and applies the source10.0 bound. Icon and
  two arrows are serviced via `2DF8A0`; tail `1BAF34` calls `141C30`.
- `OriginalUiScreenAssets50KG` can retain authenticated PST/CEL/ATM source data;
  `OriginalUiText50KG` can perform existing glyph layout. Neither substitutes for
  these source object transforms, draw admission, registry ownership or callbacks.
- Every source PSM and any genuinely absent-PSM fallback needs its own exact
  provenance, source template and retained canonical material reference. Resolve
  through the existing FINAL representative collection. Do not borrow Damage's
  fallback admission, fabricate texture map0 for a zero-map material, or infer
  blend/depth state from a resource name.

The authenticated OVERLAYS catalog includes `results1.pst`, `results2.pst`,
`font/denmarkbold16ingame.cel/.pst`, `letterboxpause.psm`, `icon_xpause.psm`,
`font/denmarkbold16pause.psm`, `icon_x.pst`, `arrwleftPause.psm`, `arrwleft.atm`,
`arrwrghtPause.psm`, `arrwrght.atm`. This inventory is not a decoded PSM-state
claim. Catalog offsets are logical decoded offsets; they must not be used as
physical offsets into compressed OVERLAYS.RES to infer material contents.

## Draw and callback boundaries still requiring translation

`1BAF60` jumps into `1B9220`; it does not call only the derived leaves. Shared
render accesses controller382B20, geometry caches and localization, invokes
slots78 then74, submits source primitives through `2E21D0`, conditionally submits
text+180 via `2E0170`, and continues to child traversal `141D70`.

Slot78 `1BAF70` conditionally draws icon+890 (owner+1F0 and float+30 gate), then
conditionals on +160/+164 control the backdrop path. Backdrop draw calls are
`1BAFE0` +A0, `1BAFE8` +100 and `1BAFF0` +40. Slot74 `1BB010` starts another
substantial dynamic text/layout path; translating the three backdrop calls is
not completion of the parent renderer.

Input slot64 `1BD7C0` first gates on owner+1F0==0 and event+08==owner+884.
Calls `1277D0` at `1BD804/1BD860` use IDs0C/0E, mode2 and mask0x80000000.
Their accepted paths decrement/increment owner+880, wrapping within 0..10,
and call `1A4B90` with source feedback3/4. `1BD8BC` checks matcher ID2 and the
following dispatch distinguishes selections10,9,1,0 before later branches.
These facts do not yet authorize labeling those branches Resume/Restart/Quit.
The rest calls real race/application owners (including `1BE830`, `1AC8E0`,
`267BE0`, `1ABFC0`, `199770`, `1CA8E0`); each accepted outcome must be translated
with its condition and mutations before connecting a host transition.

## Release order

Derived destructor `1BA2C0` releases texture+890, eleven texts+3B0, text+340,
text+2D0, textures+270/+210, then shared destructor `1B8730`.
`1B8768..1B878C` releases nonnull +38/+3C using `2E1980(...,1)` and clears them;
`1B8794` destroys text+180; `1B87AC` destroys the two textures+A0/+100;
`1B87B8` destroys texture+40; `1B87C4` invokes Entity destructor1413E0.
Native ownership must keep canonical references and image data alive through
checked renderer retirement, then release source owners in the corresponding
order. It must not create a renderer/display/owner strong-reference cycle.

## Smallest honest next implementation

1. Translate constructor/load geometry and base polygon setup into a cold owner
   using existing native source types. Keep the source resource registry identity,
   exact capacities, source text, color and primitive order. Fail atomically on
   unavailable resources; do not publish a partially loaded page as ready.
2. Add exact Pause material adapters for the listed authored PSMs and any proven
   missing-PSM branches. Cold-stage retained references before the canonical RA
   seal, finish after final population, then bind GPU resources. Root must validate
   actual effective depth/alpha/texture policies before admitting renderer paths.
3. Translate shared1B9220 plus derived1BB010/1BAF70 projection and actual paused
   material animation clock service. Submit source leaves in source order with
   separate native source-alpha target semantics. Do not invoke an unrelated
   frontend page's update/render to obtain a presentation permit.
4. Translate input1BD7C0 and helper1BE830 outcomes using real menu/event identity.
   Keep navigation in UI only. Reuse existing application pause clock and audio
   service boundaries; leave every racer on original AI.
5. Replace the driver's `sessionMenu.view -> submitHostMenu` branch only after
   this actual owner accepts source input and produces a valid native frame.
   Existing HostSessionMenu outcomes cannot themselves attest original callback
   completion. Preserve accepted submission/swap, source pause timing, explicit
   FPS exclusion for paused UI, restart/leave transaction and cleanup ownership.

Until steps1-4 are joined, exposing a new class with only these asset names would
be a resource inventory, not a genuine cold Pause owner. This bounded patch keeps
that distinction explicit and provides exact continuation points without adding
uncalled or falsely ready runtime leaves.

## Subsequent bounded source-data implementation

`src/assets/original_pause_source_data50la.h` now provides
`OriginalPauseSourceData50LA`. This is solely the immutable data-loading layer,
not the original Entity/page owner. `load(sharedOverlays, localization, diagnostic)`
requires the application's authenticated OVERLAYS identity, pins that exact owner,
resolves nine source-requested members, and parses/copies authenticated English
localization with the existing GameText reader. It publishes all-or-nothing and
keeps successful state on rejected reload. Member views stay valid while this
owner remains alive; copied localization does not borrow its input archive bytes.

The caller may use `member(...)`, `english()` and the retained `overlays()` for
subsequent original material dependency parsing. Only `sourceLoaded()` is exposed;
there is no page/render-ready flag, fabricated image binding, geometry, registry,
canonical reference, callback or route activation. The related icon_x/arrow ATM
catalog entries are deliberately not assumed to be PSM bindings. This header-only
layer adds no build-system registration or production caller. The five integration
steps above still apply before replacing any current host Pause presentation.

Implementation and review remained source-only. No build, syntax check, tests,
asset decoder invocation or runtime validation was performed.

## Authored material parsing layer

`OriginalPauseAuthoredMaterials50LA` adds transactional parsing for the five
actual Pause PSM members using `readOriginalUiMaterial50KG`, the caller's real
APPSTART template owner and known construction runtime. ScreenTexture2DF10C and
ScreenText2DFAA0 explicitly select ScreenMT. The LetterBoxPause primitive follows
2E1AE0's299BE0/14D590 ScreenMT association, already recovered for RadarOverlay.
The new layer pins both source-data and template owners. It does not parse an
unproven substitute, silently choose a default template or publish partial output.

Actual TextureMap_1 values and any source-defined basename fallback are obtained
from the existing parser at runtime, never hardcoded from the catalog. Zero-map
passes remain mapless. Complete authored pass sections preserve animation/group
fields for subsequent original owners; this cache does not service them. No
material bodies were decoded/executed during this source-only implementation,
so effective Pause GS policies or specific map references are not asserted here.

Canonicalization is deliberately NOT done once per distinct PSM. Real ownership
must acquire at each reached299BE0/2DF0B0/2DFD90 source primitive and preserve
stored-before-retain/partial-failure semantics, verify the mounted provider bytes,
and use the same Application382B38 table and actual ScreenMT associated owner.
Only after actual population and collector seal may a primitive's own row resolve
through coldCollectorPass to its FINAL representative. Verify effective state,
program, map counts and animation owners against parsed evidence before preparing
native policy. Results1/results2 missing-PSM fallback and their precise reference
counts also remain separate from these five authored materials. No canonical row,
collector receipt, raster readiness or draw completion is manufactured here.
