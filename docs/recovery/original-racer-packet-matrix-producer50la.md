# Original racer packet matrices and origin flame payload

Source-only implementation, 2026-10-13. No build, test, syntax check, scanner,
asset decoder, emulator, guest execution, or runtime operation was performed.

## Actual production increment

The original model's existing `OriginalRacerModelPackets50LA` allocation is now
populated and consumed by the selected native primary-engine draw path. Before
this increment it owned only the cold240740 zero matrix slots. This is a native
scalar/matrix owner, not a binary console packet or instruction stream.

At the actual player and active-opponent render-preparation calls,50DU borrows
the real left/right model owners' packet allocations. It does so after the
existing draw lease has published that racer's selected lights, and passes the
borrows into the existing50CM projection. No new Rig, graph, model, light
manager, texture bank, callback, frame scan, or registry walk is created.

The existing49S projection still evaluates the current graphics model and
source-palette bone worlds using49P's lazy2897B0 arithmetic. The new method
`preparePaletteMatrices241280` writes the actual allocated slots in original
order: selected direction matrix, selected diffuse matrix, model world, then
the original palette's bone worlds. It uses the real packet source IDs and
the same canonical source-row mapping as the projection. It admits exactly
the single nonnull palette already established by the cold primary-engine
material join, with the actual two extra slots allocated by240740.

The current live191 flags from `VehicleFrameState49W`, not the cold model
facet's constructor-cleared191, gate that preparation. The method applies
the source2413D8..241418 diffuse ambient overrides from the real model fields:
1F4 bit0 selects1F8/1FC/200 RGB, and bit1 copies204's float payload into alpha
without integer conversion. It does not modify the selected-light publication.

The output is used immediately: rigid positions, authored world normals and
source body-color coefficients read the packet's bone matrices; color
coefficients read its direction/diffuse extras; right-engine specular geometry
retains the packet's model world. No root matrix is applied a second time.
The successful draw descriptor contains copied vertices, normals and root
inputs, never a pointer to mutable packet matrices. Stopped opponents retain
their previous complete geometry and light publication as before.

The existing engine-flame cold binder now also retains the SAME origin pass's
immutable `parameters34`, paired with its actual per-model clone. It proves
that the three retained seeds match that prototype's TexGen destinations28,
29 and30. The per-frame candidate copies this borrow alongside the existing
clone/revision and already-produced animation rows. The new actual UV exporter
applies origin parameters first (24B3D0..24B404), then those candidate vertex
rows (24B408), and exports the existing six affine UV coefficients. All three
seeds are overwritten by the admitted animation rows, exactly as in this
source domain. This is a connected parameter/animation consumer, not a new
visual effect. It does not use representative-pass parameters or dispatch an
extra animation callback. Both player and opponent UV caller loops use it.
Legacy unjoined owners retain their existing UV export. The lower-engine
flame clones already owned by POD02 use the same per-model payload join.

## Lifetime and failure behavior

- Model packet storage remains owned and released by the existing model
  lifetime. Its accessor succeeds only while that model's resources are
  complete. Model/geometry/palette allocations and cleanup order are unchanged.
- Matrix slots are preparation storage. A failed preparation may change them,
  like the existing projection workspace. No draw output is published on that
  failure, and no accepted-frame receipt or rollback history is fabricated.
- All fallible vertex/color/normal work still finishes before destination
  vertices and the draw descriptor are published. Whole-vehicle callers still
  require all active components before rendering.
- Origin parameter pointers borrow the immutable actual pass vector, not the
  temporary constructor vector. They are published together with the clone
  after binding succeeds and cleared with that model's clone borrow before
  the actual retained material/model release.
- Frame candidates and owner cleanup remain serial. No candidate may survive
  teardown. Export uses the existing clone revision; commit includes the
  paired origin parameter identity in its existing all-model preflight.
- Existing callback count, RNG operations, frame stamps, pause/menu paths,
  original AI, physics, topology, renderer draw order and feedback barriers
  are unchanged. The existing callback's unsupported appActive11C route is
  not newly admitted or newly rejected by this increment.

## Explicit boundaries

This is the matrix-producing and already-admitted flame-payload portion, not
a complete241280 or global gameplay FIFO. It does not write packet flags20,
set the complete-method1B3.08 state, select LOD1B0, enqueue original surface
records, drain registered renderer leaves, publish accepted texture history,
or enable a new window/material pass. No-palette, multiple-palette and null
source-row preserve-existing branches remain outside this narrow caller path.
Chariot/lower body matrix consumers are unchanged; this packet is independent
of the unmerged primary-pair/lower/chariot/window rendering layers.

Full flag production still needs the actual selected-frame distance1BC,
matching model radius194/198, fogStartB0 and live382FA0 threshold ownership.
The existing native selected-light publication supplies usable directions and
diffuse values from the actual draw lease under its explicitly native policy.
This does not prove original14A0A0 spatial selection/cache parity: the source
cache can be keyed by shared racer membership rather than model position.
No zero or identity substitute is introduced for absent source light output.

Global FIFO closure also still lacks the real full scene submission owner and
original ordering across all registered leaves. The latest isolated descriptor
identity bridge is deliberately not included. No per-frame table identity
scan is added here.

The existing player/opponent flare preparation may sample one shared
Application RNG state before their later commits. This pre-existing scheduling
issue is recorded, not repaired or claimed authentic by this bounded change;
no extra callback or new RNG serialization is introduced.

## Source and integration basis

Base40ba85726d0b5853b5fc5f09b91930b433f82ca0, tree
b1ef8c855b34722ee6068a33c2ac54d97418bf6b, followed by the frozen cold collector
and representative-map binding increments. All nine modified source paths
retain exact40ba preimages because those two prerequisites modify different
paths. Main's authentic PSG topology repair and both renderer performance
edits are preserved; no platform renderer file is changed.

Retained source `original-text-all.txt`, SHA256
04b18bf045cd90361606491a5f54e0e7b549003e306001c870f8cfc9fd4fc1ca:
2412AC selection gate;2412E4..2412FC model refresh;
2413D8..241418 overrides;241488..241518 light/model copies;
241518..241598 source palette row copies;2897B0..2899B4 world getter;
229E08..229EF4 temporary draw membership;24B3D0..24B408 origin parameter then
vertex animation application. Original ELF identity:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
The frozen packet retains source excerpts and separate matrix/caller review
notes. These establish source semantics only, not compiled or measured results.

No source inventory change or new translation unit is required. No measured
performance or visual-parity improvement is claimed.
