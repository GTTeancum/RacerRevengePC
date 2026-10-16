# Pause backdrop population and concrete cold storage

Source-only bounded continuation from7196be9. No build, syntax check, test,
project decoder execution or runtime validation was performed.

## Corrected population prerequisite

The prior reference-count note said canonical population uses the existing
missing-PSM constructor path. That statement omitted its deliberately finite
native admission: only Tournament, SpeedBoost and Damage were admitted. A plain
ScreenMT retain of results1/results2 would preserve the reference but fail
population with "missing-file fallback not established". The new dedicated Pause
backdrop proof is therefore required before implementing the first actual load.

`readOriginalPauseBackdropFallbackMaterial50LA` admits only results1/results2 in
authenticated OVERLAYS, with exact present PST and absent PSM, explicit ScreenMT,
known runtime and original first caller1B8840/1B898C. It emits no fake authored
PSM. Legacy native policy identity admission is equally narrow; existing GS,
texture, depth and blend checks remain unchanged.

`retainPauseBackdropFallback14D590` is private, reachable only by the existing
Pause primitive-reference facet. It records the same archive and texture-table
proof with the real numeric ScreenMT retain. Canonical population checks table
identity, rejects conflicting fallback families and constructs original2475C0:
one map, actual ScreenMT program, real texture retain, state from NULL authored
attributes with original defaults. A contradicting PSM is rejected. Actual
canonical object exposes immutable proof for source-owner validation. The two
results2 primitives retain one shared canonical material separately, not two
fake material objects.

## Concrete source segment

`OriginalPauseStorage50LA` owns three ScreenTexture/primitive/packet storage
objects in original load order +A0,+100,+40. Its explicit progress stage is
`baseBackdrops`; there is no loaded-page or draw-ready state and no draw API.
`loadBaseBackdrops1B8810` reaches each real reference, then performs the original
whole-table14D370/29E340 pair immediately for that texture. It validates the
actual populated fallback/provenance/ScreenMT/map and authentic PST bytes before
reading source dimensions and reciprocal dimensions.

Original2DF0B0 initializes native RGBA=[127,127,127,63], XY=0, Z=VideoMaximum-1,
W=1, STQ corners(0,0),(1,0),(0,1),(1,1), Q=1. Loader dirty bit1 is retained;
XY is deliberately not regenerated into an invented layout. Only the existing
admitted Video maximum0xFFFFFF is supported, supplied by the caller.

Source1B8848..1B8ADC sets +A0/+100 width640,height16 and rewrites packetZ using
actual VideoMaximum-1. Source1B8AF4..1B8B2C sets +40 ST bounds0..bits3DCCCCCD;
1B8B44 sets width640 while preserving its genuine atlas height. Dimension changes
set source dirty bits under the original field3C!=0 condition (the equal-zero branch skips bit2). The following polygon stage,1B8DB0, is implemented below; neither stage is a full-page ready state.

Failure retains the exact partial primitive/reference state for cleanup. This
cold owner cannot submit GPU work; closeCold releases the reached references and
storage using base texture destruction order +100,+A0,+40. A future draw-capable
extension must add actual retirement gating before exposing submissions; it must
not reuse unconditional cold cleanup with GPU users. No Entity attachment or
retail parent ownership is fabricated by this storage layer.

## Remaining original owner joins

-1B8DB0 two LetterBoxPause source polygon allocations, aliases and geometry.
-Actual Application328 denmark16 font load/borrow (already supported by the
 native registry), then fourteen ScreenText allocations/batches and source
 directory restore; exact constructor/load colors, text, positions and flags.
-Icon and arrows with their actual material animation/texture resources and
 individual ScreenTexture population boundaries.
-Derived load1BA390 completion, detached Entity lifecycle and source teardown.
-FINAL collector/state projection,1B9220/1BB010 drawing,1BD7C0 callbacks and
 driver activation only after the full authentic owner is complete.

The new storage header may be included by the actual driver for the root's next
build; no runtime call or host Pause replacement is added in this patch.

## Added exact1B8DB0 polygon continuation

The concrete owner now continues from baseBackdrops to basePolygons with the
actual two kind1/four-vertex allocations. Each allocates source vertices, one
packet and one primitive in original2E1AE0 order, then acquires its own canonical
LetterBoxPause reference. There is no invented population between these original
primitive associations. Source vertices retain their actual STQ/color/XYZ aliases;
these are CPU data, not executable guest packets.

Both polygons receive source133F20(0) RGB and direct alpha63. After both complete,
1B8EC4..1B9158 sets bottom XY to (0,448),(640,448),(0,448),(640,448) and top XY to
(0,0),(640,0),(0,0),(640,0), copying authored[XY,1,1] into +14/STQ aliases. These
initial quads are genuinely zero-height. No host letterbox thickness or sourceZ
reinterpretation is added. Native draw semantics remain separate. Base teardown
now releases +38 then+3C before the texture prefix, including failed allocations.
Next exact boundary is1B8B9C font setup/base text; all text/derived-loader and
Entity/render/input work remains unimplemented.
