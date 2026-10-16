# Bounded original TA texture consumer

This source-only slice joins two complete original TA model material closures
to the existing submitted track texture bank. It does not introduce a second
renderer, image bank, original asset substitute or independently authoritative
metadata texture selector.

## Actual consumers

The existing `track_materials50fa_fixture::Owner` authenticates its normal TA
archive, retained selection and placed export, then extracts these original
members through `ta_original_members50la_fixture::Sources`:

- World instance 0: `geometry\ta_bigrocka.psg`, `textures\ta_rock2.psm`,
  `textures\ta_rock2.pst`
- World instance 309: `geometry\zone01groundstrips16.psg`,
  `textures\ta_sand8.psm`, `textures\ta_sand8.pst`

Each original PSG must retain its whole one-node, one-LOD, one-material,
one-surface closure. The provider checks exact canonical source spellings,
archive directory extents, independently pinned complete-source hashes,
selection source identities and requester links. Converted material fields,
RRSCENE and RRTEX do not substitute for original PSM/PSG/PST input.

Each actual visual instance owns its genuine `26E640` material-reference
facet, including the real `1C0` count and `1C4` handle. This intentionally
composes the existing source resource and reference owners directly. It does
not instantiate `OriginalStandardModelPrefix50LA`, claim scene registration or
invent the unrelated class lookup and constructor-unwritten header bytes.

Those same `1C4` handles resolve the Application's canonical material rows;
the constructed pass's texture token resolves its canonical resource. The
resource's largest native level 0 selects original source mip 4. Only the
authenticated retained RGBA for that exact PST, source mip and 256x256 extent
can enter the existing bank slot. Shared retained entries are rechecked too.
Both actual target spans must consume their own genuine model handle, with
one submitted span per whole original model. Canonical failure never becomes
optional-untextured success for either target.

The downstream route remains the same `MaterialTextureAssets50EA` bank,
`trackGpuTextures.create`, `renderRequest.track` spans and native renderer.
Existing authored geometry and host shading/alpha/fog policy are preserved.
Every other track/material keeps its existing selector and rendering policy.

## Application ownership and chronology

`Application` owns one `OriginalStandardMaterialDomain50LA`, composing the
same full `382B38` material table, `382B70` program-name owner and `383400`
dynamic texture table. Its original source resolver and PSM owners are bound
once and retained across serial native sessions. A restart must present exactly
the same immutable source corpus; it cannot replace the resolver or cache.
The latest AIDamage table additions and shared-manager terminal guards are
dependencies, unchanged here. Future AIDamage callers must borrow this exact
domain's material table rather than mint another manager.

The Application owns one mutable `36B970` string. The represented source
writers are startup `17CA6C/70` (`textures\` from `3A85C0`), actual frontend
entry `17DC14` (`ui\textures\` from `3A8628`), and gameplay entry
`17D814/18` (`textures\`). The same-session Course handoff invokes that
gameplay writer after the actual coordinator publishes launch readiness.
Track acquisition borrows the field and rejects a still-UI directory. It
does not assign a guessed prefix from a selected asset's name.

Canonical publication now follows successful real `host.create` inside the
existing cleanup-protected block. The track-bank-dependent GC fire span,
MA shield UV and GC/GA/MA-SA ATM cold consumers follow it. Independent mesh,
request, normal, sky, dynamic-material and Binder preparation remains earlier.
The persistent Video/context comes from the lifecycle predecessor: only the
original 640x448 Application mode is admitted. Direct 640x480 remains on its
existing route and supplies no canonical context.

Renderer/GPU borrowers are retired before the real host closes. The track
owner, original model references and canonical domain remain alive through
that cleanup. Existing CPU resource unlink/release permits destruction after
host closure; reopening keeps the same Application/Video identity but creates
new native GL publications. Canonical partial failures retain the original
manager-wide terminal state and cannot reset it with a fresh track owner.

## Kind1 source semantics

The existing canonical resource now supports revision1 kind1 alongside kind0.
Kind1 owns 256 raw RGBA palette entries at the source's distinct `palette208`,
one-byte index payloads, and its authentic five largest-first native slots.
`palette204` remains kind0-only. Native AC/E8 are 8 for kind1 and 4 for kind0.

The proven cold initial `382FC8=1` branch executes native scalar byte-transfer
lowerings of `24D3E0`, `24D470` and `24D6F0`, using the exact retained original
tables. Each kind1 final `100` view points at real owned transfer backing and
has known/set flag30 bit2. Immutable original linear bytes remain separate.
The RGBA consumer never interprets transfer storage as image coordinates.
No PS2/VU/GS emulator, guest execution, synthetic image or cleared-global
bypass is involved. Scoped-zero `382FC8` callers are not admitted by this API.

## Remaining boundaries

- This is a bounded two-model consumer, not whole-track canonical coverage.
- Largest retained mip remains the explicit host policy. Demand10 does not
  drive residency; actual `254AD0/254B20` frame/reset/residency work is open.
- No original program lowering, scene collector, geometry1F0, child141C30,
  complete StandardModelPrefix construction or full source draw chronology is
  established. Retained native rendering policy remains explicit.
- The immutable two-texture source corpus must be extended deliberately for
  broader consumers. Missing members do not establish original file absence.
- UI's existing lifetime ledger is not recast as numeric original refs.
  The separate AIDamage owner join is preserved but not activated here.
- Source allocations, timing/profiling effects and full mutable382FC8 writer
  coverage remain outside the bounded native transfer implementation.

Only manual source and bounded existing evidence inspection was performed.
No build, test, syntax/lint/scanner/sanitizer check, source or asset decoder,
gameplay, guest execution or PC operation ran. The code is uncompiled and
runtime-unverified. Source hashes identify bytes, not successful validation.
