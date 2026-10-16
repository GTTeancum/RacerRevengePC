# Actual primary-left source-color and MODULATE consumer

Source-only increment against `9d1330349974afe85e0d891c087713ae748e9abe`,
tree `84541aa865fb1e054cb27d9dd6f1a3c232217fc2`.

## Connected production path

The existing primary-left canonical owner now optionally enables source body
colors during the same cold material binding as the right engine. Its real
model, geometry1B4, references1C4, shared382B38/383400 domain, source-row palette,
PSG/PSM/PST identities, material ordinals and native texture bank are checked by
the existing canonical join. No second Rig, material population, texture bank,
resource reference or geometry import is added.

The optional selection is restricted to the actual primary-left component.
Each selected body must use SpecularMapDualCompressed, two maps/two contexts,
no animation or runtime context override and no parameter count/mask/storage.
The consumed base context requires MODULATE/TCC1, textured Gouraud STQ, color
clamp, opaque blending, disabled alpha/destination tests and writable depth.
The complete material-sized mask leaves flame entries zero. Fallible checks
and allocations precede the final vector swaps; failure cannot publish a
partial mask or change the existing bindings.

During player and active-opponent rendering, one combined descriptor contains
both primary-engine masks and the same already-completed selected-light
publication. Left consumes its own component's rows, actual source normals,
authored qNormal and current per-node position palette. Right retains its
existing paired-specular descriptor and bank. The shared native diffuse leaf
and shader are unchanged: source-ordered binary32 early-bias RGBA arithmetic
and non-perspective RGB interpolation feed byte MODULATE floor/clamp /128.
There is no normal mirroring, normalization, imported vertex RGB multiplier,
extra light selection or substitute light constant.

The CPU and renderer now admit source body colors only for primary left/right.
Both still require complete masks and exact light owner, publication, graphics
sequence and racer-slot provenance. A new explicit cold right-only pair guard
preserves the boundary previously implied by the right-only diffuse guard.
The source-specular geometry producer remains right-only.

## Lifetime and existing behavior

The Visual owns each immutable mask for the same lifetime as its existing
material bindings and renderer borrows. The combined helper writes its output
only after both components are valid. Existing runtime staging and publication
remain unchanged. Stopped opponents retain their completed geometry and
matching light packet. Menu/inactive/focus/minimize/recovery pause gates still
precede this work; no additional light, animation, flame, RNG or material-clock
advance is introduced. The native-lighting opt-out keeps both old base paths.

POD02 lower engines, chariot, real flame clones, AI, model/animation aliases and
reverse cleanup remain unchanged. No translation unit, platform declaration,
source-inventory entry or GL operation is added.

## Retained source basis and explicit limits

Original ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
`docs/original-racer-lighting50km.md` and
`docs/recovery/original-racer-source-color50la.md` retain the numerical evidence:
registration150B3C selects the dual packet32C3F0; the actual packet extent is
32C410..32E2A0. The shared preamble3400D0..3400E0 scales selected direction rows
by authored qNormal before node-basis composition. Dual32CA40..32CA70 and shared
337868..337978 support the light dots, nonnegative amounts and ordered early
bias/clamp packing. This arithmetic is program-specific, not left/right-specific.
These are retained source interpretations, not executed instruction words.

The retained original-eight catalog has equal per-pod body material names on
left and right, but different PSG hashes/geometry. Catalog SHA256:
`02ff1ee1d5726e266a56f89b8e6c6b6961f743dfbac90c1153cdca9fe1eafa03`.
Material-name correspondence corroborates the program; it does not authorize
reusing right geometry or light rows. Runtime authentication remains based on
each actual canonical source owner, not the catalog comparison.

This increment consumes left base RGB only. Left generated alpha remains CPU
data; output framebuffer alpha remains opaque. Left second-map sampling,
source pair fog/depth behavior and source destination-alpha composition are
not enabled. Its complete canonical second resource remains owned as before.
The existing right bank is load-once; future multi-model pairs need coordinated
cold publication of authentic texture/context projections and matching native
source-group/bounds admission, rather than replacing a live right publication.
Chariot multipass and optional lower source-color/pair obligations remain open.
Original raster/clipping precision, full fog/depth/filtering and retail light
cache/scheduling parity are not claimed. Existing right specmap5 header and
fog-evidence limits remain unchanged.

Manual source/evidence, API and complete diff review only. No builds, tests,
compiler/syntax/lint/scanner/sanitizer, asset decoders/loaders, gameplay,
emulation or guest execution. This packet is unbuilt and untested.
