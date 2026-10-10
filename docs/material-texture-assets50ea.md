# Material texture ownership 50EA

Status at addition: **source-reviewed, uncompiled and untested**. No build,
test, validation script, renderer, runtime probe or guest execution was run for
this addition. Shared verification remains with the parent task. Build files,
renderers and original decoders are unchanged.

`src/assets/material_texture_assets50ea.{h,cpp}` adds
`rr::assets::MaterialTextureAssets50EA`, a cold, load-once owner for resolved
native texture bytes and explicit material-slot mappings. It reuses
`parseTexture48B` from `native_asset48b.cpp` to construct each `Texture48B`.
This supplies stable CPU texture storage for future track/pod material binding;
it does not yet put those textures on screen.

## Caller contract

Pass a `MaterialTextureAssetsInput50EA` with:

1. A texture table. Each `MaterialTextureAssetInput50EA` supplies an opaque
   `uint64_t identity` and a pointer to actual resolved `RRTEX001` bytes.
2. An ordered table of `MaterialTextureSlotInput50EA` declarations. Each gives
   a slot key, the exact requested texture identity and a missing-resource
   policy.
3. Explicit `MaterialTextureBudgets50EA` capacities.

Texture identities are caller-assigned exact identifiers. All 64-bit values,
including zero, are valid. The owner does not hash names, infer resource paths,
search a directory, case-fold strings, match basenames or silently select among
duplicate IDs. A repeated identity in the texture table rejects, even if its
payload is identical. Multiple slots may explicitly share one texture identity;
that texture is parsed and retained once. Different identities with the same
bytes remain separate resources and each consumes budget.

A slot key contains `(assetIdentity, materialIndex, pass, slot)`.

- `assetIdentity` is an opaque caller identity for the source asset/material
  table. Give distinct static assets and vehicle components distinct identities
  when they share a bank. A material index alone is not globally unique.
- `materialIndex` retains the original material-table index.
- `pass` and `slot` retain the caller's explicit source mapping. When mapped
  from `Material48E`, use its original one-based `ShaderPass_N` and
  `TextureMap_N` numbers. The bank does not require those numbers to be
  contiguous or reinterpret zero, large values or ordering.

The complete key must be unique. The owner preserves both input tables in
their original order, with separate sorted indexes for lookup. No material,
pass or slot is collapsed, reordered in the public table, or promoted to an
assumed base-color channel. The bank does not infer render-pass order from
lexicographic lookup order. The caller is responsible for declaring every slot
it intends to retain and for the authenticity of its source mapping.

For existing cold geometry owners, a static draw span's source asset plus
`SceneSpan48D::material_index`, or a vehicle component plus that material index,
can identify the relevant source material table. The caller must still resolve
the authored material, explicit pass/map slot, and actual texture resource.
This owner does not perform those scene/material/archive joins itself.

Input arrays, byte vectors and the diagnostic object must be valid, distinct
storage with stable extents and contents throughout the serial `load` call.
Caller-provided array extents and external identity provenance cannot be
validated from pointers. Input bytes and descriptors can be released after a
successful load: the bank owns copied slot records, identities and pixels.

## Missing resources are explicit

Every slot selects one of these policies:

- `MaterialTextureRequirement50EA::required`: the requested texture identity
  must exist in the supplied texture table. Absence rejects the whole load
  with `missingRequiredTexture` and the original input slot index.
- `MaterialTextureRequirement50EA::optionalUntextured`: absence is admitted.
  The output still contains the original slot key, requested texture identity
  and policy, with `textureIndex == unresolvedMaterialTexture50EA`.

An absent resource is omitted from the texture table. A null byte-vector pointer
is an invalid descriptor, not an alternate spelling for absence. A present
texture with malformed native bytes also rejects, even if only optional slots
reference it. The bank does not hide corrupt resources by changing them to
untextured slots. A caller deliberately choosing to omit a resource remains
responsible for that choice and any diagnostic it needs to show the player.

`textureForSlot(index)` returns null for an unresolved optional slot. Inspect
`slot(index)` first to distinguish it from an invalid slot index or an unloaded
bank. There are no fabricated white/checkerboard pixels and no implicit binding
to texture zero. A renderer must make its own explicit untextured choice, rather
than accidentally retain the preceding draw's texture.

All-optional-unresolved banks with zero textures are valid. Set a sufficient
slot capacity; texture, byte and dimension capacities may all be zero. An empty
bank with zero-initialized input is also a valid successful load. Use `loaded()`
to distinguish this deliberate empty bank from a never-loaded/failed owner.

## Bounded admission before allocation

Capacities are ceilings, not reservation sizes. Zero allows no resources of
that class; `dimensionCapacity` must be positive when textures are supplied.
Each field may only reduce these hard ceilings:

| Field | What it bounds | Hard ceiling |
| --- | --- | ---: |
| `textureCapacity` | Supplied texture records | 4,096 |
| `slotCapacity` | Declared material-slot records | 65,536 |
| `sourceByteCapacity` | Sum of all supplied RRTEX001 byte lengths | 256 MiB + 65,536 bytes |
| `rgbaByteCapacity` | Sum of validated decoded RGBA payload sizes | 256 MiB |
| `dimensionCapacity` | Width and height of every texture | 4,096 |

The native format remains `RRTEX001`, followed by little-endian uint32 width and
height, then exactly `4 * width * height` RGBA bytes. Each texture also obeys
the native parser's individual 64 MiB + 16-byte file limit.

Admission runs in this order:

1. Validate pointer/count descriptors, hard/caller capacities and table counts.
2. Sum **all** texture byte-vector extents using subtraction-before-addition
   checks. Reject aggregate source overage before examining any header.
3. Without allocation, scan every supplied native texture: require at least
   16 bytes and the individual file bound; check exact magic/version; read
   dimensions bytewise; require width/height in 1..4096; compute pixel bytes in
   uint64; require exact header-plus-payload length; check caller dimensions;
   and bound aggregate RGBA bytes before addition.
4. Validate every slot's policy enum. Only now allocate the bounded metadata
   tables and sorted lookup indexes.
5. Reject ambiguous texture identities and slot keys; resolve each declaration
   by exact numeric identity; reject any missing required texture.
6. Call `parseTexture48B` for each texture and reconcile its dimensions/payload
   size with the already admitted header. Commit only after every parse succeeds.

Thus a large final resource cannot cause earlier pixel allocations before the
aggregate check, and forged dimensions cannot bypass budgets by supplying a
short or overlong payload. The preflight duplicates only the native framing
rules needed to bound allocation; the existing parser remains responsible for
constructing `Texture48B`. It receives no guessed format or dimensions.

`sourceByteCount()` reports admitted serialized bytes, not retained raw file
copies. `rgbaByteCount()` reports retained pixel payload bytes. Neither claims
total heap usage: texture/slot records, lookup arrays, vector/allocator overhead,
diagnostic strings, and caller-owned source vectors are additional. Array
metadata is bounded by the table capacities. Allocations use admitted actual
counts, not capacity-sized reservations. No memory/performance measurement has
been run for this addition.

## Immutable access and integration

- `texture(index)` returns `const MaterialTextureAsset50EA*`, including its
  caller identity and owned `Texture48B`.
- `slot(index)` returns `const MaterialTextureSlot50EA*`, retaining the original
  key, requested texture identity, policy and resolved input texture-table
  index or unresolved sentinel.
- `findTexture(identity)` and `findSlot(key)` return matching immutable records
  in O(log N). They use exact identity/key comparisons only.
- `textureForSlot(index)` returns `const Texture48B*` for a resolved slot.
- Count getters return zero when unloaded. Pointer getters return null when
  unloaded or absent/out of range.

Every getter is allocation-free. Successful storage is nonmoving: the owner is
noncopyable/nonmovable, has no reset/reload, and never changes its vectors after
publication. Borrowed pointers stay valid until destruction. Keep the owner
alive through all readers, including any pending upload. The API introduces no
thread synchronization; publish the fully loaded owner through the caller's
normal synchronization before concurrent const access, and never destroy it
while readers are active.

RGBA bytes, row order, raw alpha and dimensions are exactly those in the
supplied native file. There is no channel normalization, alpha scaling,
premultiplication, row flipping, resampling, synthesized mip chain or color-space
conversion. Original compressed texture decoding and mip selection happen
before this owner, for example when a caller explicitly supplies a decoded
`Texture48E::base_rrtex`. The bank does not make that choice itself.

Renderer integration still has to choose the authored channel/pass, implement
its blend/depth/sampler/color-space policy, create/upload GPU objects and bind
the proper resource for each draw. This code neither uploads to GL/D3D nor
changes a renderer or the build graph. Compile/link the new `.cpp` alongside
`native_asset48b.cpp` when adding the owner to a target.

## Failure and verification boundary

`load` is `noexcept` and transactional. Failures destroy candidate storage and
leave a previously empty owner unloaded and retryable. A second load on a
successful owner returns `alreadyLoaded` without altering published values or
pointer identities, including for a deliberately empty bank.

The diagnostic distinguishes invalid descriptors/policy, budget overage,
malformed framing, duplicate identity/key, missing required resource, native
parser failure, allocation failure and unexpected exception. Relevant texture
and slot fields use original input-table indexes; `0xffffffff` means no
relevant index. `byteOffset` identifies a bad dimension/header/payload boundary
where applicable. `parserMessage` carries the existing parser's message if it
fails. Success clears error/location fields. Diagnostics are not retained in
the owner.

Still unverified by execution: compilation/linking, exact-budget success and
one-over-budget rejection, malformed headers, duplicate IDs/keys, shared
resource resolution, optional-only/empty bank behavior, transaction/retry and
immutable getter lifetimes. No claim is made yet about displayed material
parity or playable rendering.
