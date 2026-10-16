# Native POD02 lower-engine canonical lifetime

The actual optional POD02 lower engines now use the same canonical cold model
ownership already used by the primary engines and chariot. The existing Rig,
SceneGraph, bank slot, Application, caches, material domain and motion library
remain the owners; no duplicate vehicle or substitute asset is introduced.

## Source-backed optional pair

POD02's original PHY selects englt2 and engrt2 in addition to englt1, engrt1 and
chariot. All four engine MCFs author zero motions and name their own SLT/VUT.
The lower PSG members and hashes are distinct from the primary ones. These are
separate first-load identities, not a reason to force a shared-cache miss.
Actual live MCF153C20 or PSG26EA28 hits still reject at the existing explicit
warm-clone/retain boundary.

The lower-right owner uses animation1AC/model1C0, published at211210/211254,
with animation/model appends211244/21126C. The lower-left owner uses
animation1B0/model1C4 at21150C/211550 and appends211540/211568. Each uses the
same original1536F0/1538B0 constructor arguments, one model Entity reference,
animation mask2 and model mask1008. The old animation firstChild18 alias remains
when the same model Entity is reappended under detached7F8.

Actual construction, setup, canonical material contributions and Session joins
follow source-relative order: right, left, lower-right, lower-left, chariot.
This differs from Rig's established left/right/chariot/lower-left/lower-right
asset indices. Each setup restores shared directory36B860 before the next
starts. Real zero-motion setup is retained; no primary-engine motion or mapped
row table is borrowed for a lower engine.

Five-way peer admission checks every pair for the same Rig, bank, arena, slot,
generation, Application, material domain, resolver, motion library and shared
path, plus distinct root/source-row ranges, before writing reciprocal links.
The optional pair is complete or absent. Three-model callers retain their
existing defaults. Private model-domain joins admit at most four peer models;
with five animations and four racer-prefix Entities the maximum domain is14,
within the existing64 limit. Generic forest validation is unchanged.

## Useful material and flame consumers

Each lower model supplies its whole authenticated PSG payload and genuine
26E640 references before the existing one shared material population. Canonical
model/root/PSG, source-row, palette, all-LOD ordinal, PSM, program/context and
first-map PST/mip checks select the existing native RGBA bank. Lower engine
base-map, blend, depth, alpha and sampler policy must match the prior admitted
host display. Right source-color/specular output is unchanged; no lower second
map output, new GPU bank or artistic substitute is introduced.

The canonical flame join uses the existing L/R/BL/BR bank ordinals0/1/2/3,
private records2/3/4/5 and immutable side prototypes0/1. Rig lower-left role3
maps explicitly to flame ordinal2; lower-right role4 maps to3. Source220984 and
2209CC register/bind lower-left model1C4 to20F110, and2209E0/220A28 do the same
for lower-right model1C0 and20EE90, with the same actual racer callback owner.

Original same-side flame material names share a pass prototype, but actual
29B050 registrations give each model its own clone, rows, callback, revision
and frame stamp. Registration multiplicity comes from real model references.
The existing callback order, shared side smoothing, RNG, UV arithmetic,
transactional commit and pause behavior are unchanged. The existing cold
record binder publishes all models transactionally, so canonical registration
admits only an actually all-unbound or fully-bound record bank.

## Ordered cleanup and existing gameplay

Session, frontend-only, Course/restart, partial construction and exceptional
RAII cleanup retire chariot, lower-left, lower-right, left, right. An earlier
bound owner cannot close while a later peer remains, including one that has
not reached its Session append. Every concrete release still preflights the
motion, scene, model reference, material/geometry and packet state before
mutation. Model release repairs its current7F8 parent/sibling links while
preserving the old animation alias until the enclosing animation is disposed.
Only native peer pointers are severed afterward; source publication slots are
not artificially reset.

The banked flame borrow for a closing lower model is removed before its real
material references are released. A shared same-side pass survives while its
primary model retains it. Normal close retains owner storage through the
explicit release pass; exceptional RAII may destroy lower storage sooner.
Neither path renders or dispatches a clone between these serial releases, and
final pass destruction does not dereference obsolete clone model pointers.
There is no invented unregister operation or double model destruction.

Original native POD02 EngineMotion5 graphics, lower placement, offsets, bounds,
rebase, cables, impact/effect owners and Binder pairs remain unchanged. AI,
physics, controls, pause and ordinary three-model behavior keep their existing
paths. Generic Rig installation is skipped only for an actual canonical owner.

## Bounds and evidence

This is still an explicit projection of source-relative model-owner order.
Canonical scene-only190/198/194 Entities, particle children, combined model194,
real warm clones, later213380 material selection, complete210770 chronology
and full original visibility/render dispatch are separate obligations. The
lower Entity cutoffs211274/211570 precede material selection and nullcg work;
this packet does not claim those later calls ran as part of model ownership.
Existing native geometry/placement work is preserved rather than replayed.

Original executable SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Retained CPU listing SHA256:
`04b18bf045cd90361606491a5f54e0e7b549003e306001c870f8cfc9fd4fc1ca`.
Original POD02 RES SHA256:
`7cee08d049a47d088e8329ab63d622471299945d32f76ce9c9fb21b358e845e0`.
Lower-left PSG SHA256:
`8e6c3e605adba0fee5a36ee73528c0ba86fadc75125eb826dee984cebdea7229`.
Lower-right PSG SHA256:
`72a75aaf172e64063eaf5171b5b6ec9ad9e4d8cfb47e879069f6f6cd854051f1`.

Base commit a5b95c0878712160ad26acde2e2ceb4533e15260 includes the prior
primary/chariot lifetimes, right renderer, unsigned-literal and Linux source
registration fixes. No new translation unit or source inventory entry is added.
Manual source/diff review and exact file hashes only: no builds, tests, syntax
checks, scanners, sanitizers, asset decoding, gameplay, emulator or guest
execution. Root owns integration and execution acceptance; none is claimed here.
