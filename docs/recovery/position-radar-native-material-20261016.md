# RadarOverlay native zero-map pass preparation

Source-only implementation against 4c56471. No build, test, syntax check,
sanitizer, gameplay, decoder check, guest execution or runtime validation.

## Canonical ownership and pass selection

The existing genuine299BE0 RadarOverlay reference now retains the actual
application program-name registry alongside its actual canonical material table.
Its preparation method accepts a canonical collector seal, never a caller row.
It resolves retained material00/pass0 with coldCollectorPass, then resolves that
entry's finalLeaf with coldCollectorEntry and requires the terminal registration.
The origin is exactly one pass; origin and representative must both be zero-map,
parameter-free, nonanimated, without EnableFillColor or dual-map flags. Program
identity is read through the actual program registry and must be ScreenObject.

No prepareColdMap249610 call is made: map0 is out of range for this authored
material. There is no texture lookup, atlas, sampler, residency, map bind or
fabricated readiness token. A disabled representative returns explicit enabled
false and the native adapter produces no active span.

The effective representative's real state30 context is copied, including every
field, provenance, inactive register, sampler scalar and original video fields.
Policy admission follows prepareOriginalUiNativeUntexturedMaterial50KG's
supported TME=0 field widths, tests, blend and depth semantics directly on this
canonical state. No synthesized UI PSM descriptor, template identity, default
ScreenMT reconstruction, or forced depth/blend override is used. Unsupported
state returns diagnostics and leaves caller output unchanged.

## Native renderer join

platform/linux/native/original_radar_overlay_material_adapter50la.h exposes
prepareOriginalRadarOverlayNativeSpan50LA(retainedReference, collectorSeal,
actualLoweredSpanBounds, output, diagnostic). It reacquires the canonical pass
and invokes existing convertOriginalUiUntexturedMaterialSpan50KG only for an
enabled representative. Output retains origin, FINAL identity, full actual
state, semantic program index and the native material/depth span.

Native TME=0 uses raw RGB/alpha bytes. RGB blend alpha uses A/128; the source
stored alpha remains the raw byte. No texture sampler is set and the existing
unresolved texture sentinel remains intact. Depth is translated from actual
state; separate-alpha requirements remain visible. Existing native validation
in vehicle_window_gl50ds.cpp accepts this explicit untextured span shape.

## Integration obligations and limits

The primitive owner must invoke preparation with its own primitive.material
at the actual draw position and the same domain's current coldCollector14D440.
After the separately recovered type5/ScreenObject lowering, pass real quad
bounds to the adapter, append enabled spans in original order, and retain actual
primitive/material/table owners through both renderer drains. Reacquire after
any table mutation; entries are borrowed, not lifetime or GPU receipts.

This patch does not recover2E21D0/type5 coordinates, create geometry, change STQ,
activate staging, alter traversal, or perform renderer submission. Existing
packet STQ aliases cannot simply become host XYZ. Ordinary submitOriginalUi
requires a real opaquePresentationAlphaOnly proof; a source-alpha consumer
requires the actual sealed source-RGBA owner. Neither is asserted here.

## Native gameplay renderer join (incremental)

The added original_position_stats_render.inl follows the already integrated
TimeStats private source-RGBA path. It binds the typed native PositionStats
adapter, uploads actual stats/font images (maptop shares stats), allocates a
native-drawable-size RGBA8/D24 scratch target, and copies the current pending
race color into that target. It validates and copies the complete ordered CPU
quad/packet frame before rendering. All three textured material policies are
prepared from the adapter's authentic material descriptors. Shared atlas sampler
state must agree; material index is not treated as texture index.

The existing native UI source-composition shader evaluates raw byte RGB and
independent source-alpha replacement. Its coherent-fetch path draws ordered
packets directly; its texture-barrier path preserves triangle order. Every
admitted PositionStats blend uses source alpha, never destination alpha. Thus
initial copied gameplay alpha is not an input to these equations. Radar uses
TME=0 and binds no image. Native depth function/write mask is selected from each
actual policy, rather than retaining the preceding textured state. The result
is copied into the pending race image and presentation alpha restored to1;
source scratch alpha is not passed off as a recovered gameplay alpha owner.

The depth-independent radar vertex converter requires both actual canonical
state and prepared policy to be ALWAYS/no-write. It accepts the genuine expanded
screen pixel XY and raw packet colors, emits inert native Z0, and leaves the
source STQ.z/w in the owner untouched. Source type5 expansion supplies integer
screen point XY plus/minus its3px normalized perpendicular; no guessed /16,
half-pixel offset or camera transform is introduced. This is native CPU/GPU
lowering, not guest execution or source hardware rasterizer emulation.

Public integration: bindOriginalPositionStats(shared_ptr<
OriginalPositionStatsNativeOwner50LA>) after actual adapter cold preparation.
For each coherent frame prepare the adapter draw, then set
VehicleWindowFrame50DS::originalPositionStats to that same adapter pointer.
Renderer preparation checks same owner, viewport, scene/generation, serial,
canonical-resource readiness, texture publication and complete quad coverage.
The pending-frame draw invokes PositionStats after TimeStats and before
SpeedBoost. Zero visible quads is a valid prepared frame. Cleanup includes the
new resources in all shared GL lifetime/drain conditions and retires actual
names before dropping the renderer's strong adapter reference; the adapter
weak-borrows the resource owner to avoid a display lease cycle.

This increment requires the companion original_position_stats_hud_draw50la.h
interface and concrete HUD adapter patch. It does not activate parent staging
or fabricate missing source frames. Root owns activation and combined validation.
No build/test/syntax check/gameplay or runtime success claim accompanies it.
