# Native Rider visibility and flat membership

Source-only staged candidate, finalized 2026-10-11. Active checkout was not edited.
No build, test, syntax/sanitizer check, project parser run or gameplay was executed.
Original ELF bytes were read only to produce static instruction listings.

## Deliverable

- `files/src/game/original_ui_rider_visibility50kg.h/.cpp`: concrete native flat
  membership owner, generation-safe registration/removal, current-frame selection,
  independent membership camera, and original eight-corner bounds clipping.
- `files/src/game/original_ui_rider_model_fog50kg.h/.cpp`: narrow export of the
  existing current aggregate-bounds projection, reused by selection and fog.
- `files/src/game/original_ui_main_particle_camera50kg.h/.cpp`: narrow numeric
  export of the existing original tangent polynomial. Existing camera callers
  keep their original arithmetic grouping.
- `preimages/`: exact existing-file inputs, unchanged from active checkout
  `ddd4a3e` when read on October 11. New visibility files have no preimage.
- `rider-visibility-selection.patch`: integration patch with repository-relative
  paths. `SHA256SUMS` identifies the staged source and original preimages.

Add the new visibility `.cpp` to the actual executable/test source list when
integrating. It depends on existing Rider scene, scene graph, model bounds,
Rider clip-camera and particle-camera translation units. No CMake/build-list
files were changed here because the parent owns integration.

## Recovered behavior

The UI owner at `1CCD34..1CCD7C` creates one zone owner with no portals, zone
index `+48=0`, and one flat search installed for both static and dynamic paths.
This staged owner represents its Rider dynamic-model subset. It pins actual
authenticated cold assets and binds the real character/pod scene, rather than
accepting a fabricated selected flag or an arbitrary graph pointer.

`27F650` restores viewport `4C/50/54/58=(-1,+1,-1,+1)` and, on the no-portals
path, invokes `280700/280720` to rebuild membership matrix `60` from the active
camera. The source uses the single half-radians literal `0x3C0EFA34 * FOV`, the
original `13BCA0` tangent, then `near * tangent` and `aspect1F8`. `14DAF0` builds
the off-axis projection before multiplying `viewC0 * projection`.

Crucially, this is NOT the raster projection80:

- membership depth diagonal: `(far + near) / (far - near)`
- membership depth translation: `((-2 * far) * near) / (far - near)`
- raster80 has the independently recovered zero-to-W depth projection

Both the arithmetic grouping and signed-zero offsets remain separate. No host
`tan`, simplified reciprocal, shared raster80 substitute, perspective divide,
or OpenGL projection conversion is used for membership selection.

`2B0580` starts at center-minus-extent, doubles the extent, then walks corners by
adding X, adding Z, subtracting X, adding Y, adding X, subtracting Z, subtracting
X. It retains every separately rounded binary32 operation. Each transformed
corner contributes six strict plane tests against the magnitude of W; equality
is inside. Bit order is `+X,-X,+Y,-Y,+Z,-Z`. Intersection determines rejection;
union records the crossed planes. This is direct native geometry arithmetic,
not an instruction dispatcher, CPU/VU interpreter or guest-register model.

`240C00` uses `rootWorld100 * membership60` with the normal `3833D8=1` gate.
The explicit gate-off path uses original camera100. It preserves the second
guard test against `rootWorld100 * camera2D0`, including the fact that only its
union is consumed and its rejection result cannot overturn first-pass
selection. Source removes far bit `0x10` after that test and performs the exact
`1B2` bit0/04/08/80 updates. Culled models preserve old bit0, retain04, and clear
08/80. Disabled models skip this work and only clear191 bit0.

`26EFC0` copies actual visibility into191. Its enabled-model count includes
culled models and excludes disabled models. Selected-only `26F070` distance
and `24136C` radial-fog computation remain in the existing packet/fog owner.
The additive `recordPacketFog` join retains that owner's independently computed
bit1 for subsequent visibility calls.

## Backend integration

1. Bind `OriginalUiRiderVisibilityOwner50KG` to the authenticated Rider archive.
2. At actual attach, call the appropriate `registerModel` overload with the
   pinned assets, real scene and effective shown state. Character registration
   admits its existing beginLoad graph before finishAttachment, but enabled
   selection requires successful finishAttachment.
3. Retain the returned owner/widget/generation handle. Forward recursive model
   enable/disable to `setShown`; hidden models keep membership.
4. After current camera/fog preparation and model motion, invoke `prepareFrame`
   once with `cameraAfterFog` and the actual source debug policy. Normal source
   defaults are `3833D8=1`, `4354F0=0`.
5. In the actual model render traversal, call `selectModel`. Use its191 bit0 to
   gate palette/packet preparation. `sourceRenderFlags1B2` is the pre-packet
   visibility state. Never equate a widget's highlighted state with this result.
6. Character lighting can validate `isCurrentSelection` and consume actual
   membership's zero zone index. That proof does not create a light registry,
   ambient row or light cache; those remain the real lighting owner's inputs.
7. After successful packet/fog setup, call `recordPacketFog(selection,
   packet.fog.computeRadialFog1B2)`.
8. Drain all borrowed packets, remove the real membership via `removeModel`,
   then destroy/reload the model graph. Scene objects must outlive registrations.

Frame, registration and selection generations reject stale snapshots; a shown
gate change also invalidates the previous selection. Selection must immediately
precede its packet work in the parent's serial phase schedule. This owner does
not observe arbitrary external graph mutation and is not a substitute for that
schedule. `prepareFrame` failure invalidates the previous frame. Selection
failure publishes no flags, counter or result, but can refresh root world cache.

## Explicit boundaries

This owner admits the already-supported multi-node box route only. Single-node
sphere routing, portal traversal, unrelated gameplay zones, UI entity parenting,
binder registration, light-cache ownership, LOD/material-pass order, vertex
clipping, queue admission and GPU submission are not silently synthesized.
The parent compositor owns those required joins and the root owns validation.

Suggested future central coverage: inside/outside/edge bounds; negative W and
all six masks; corner-walk rounding; membership60 versus80 depth; guard-union
handling and ignored guard return;191 clearing on disable; retained1B2 bits on
cull; fog-bit carry; wrong owner/stale generation/frame; unregister/reload; and
real current post-motion character/pod bounds with authenticated assets.
