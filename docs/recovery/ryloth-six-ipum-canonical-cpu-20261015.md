# RA six-frame canonical CPU adoption candidate

Source-only candidate; no build, syntax check, execution of verifier, minted lease, or GPU publication was performed by this worker. Root independently generated the six actual outputs. No Course or runtime activation is added.

## Inputs and fixed provenance

The verifier consumes the actual same-resolver `stageIpumFlags0` result and six supplied RRTEX byte vectors, ordinal order 0..5. It accepts only mounted RA `textures\\crowd.atm`, 214578 bytes, SHA256 `76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031`, from RA archive SHA256 `1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2`. Existing stage framing validates all thirty source mip records; the fixed whole-member hash authenticates all thirty payloads. Each admitted frame additionally checks exact PST extent/hash, loaded source mip4 extent/hash, and equality of the actual aligned compressed allocation to that source range.

Actual conversion manifest: `/workspace/scratch/74cd1d3d7a45/ra-crowd-flags0-generated-0759/manifest.json`, SHA256 `bf9fb839e1569bd9875960e27cd9e52016f6b8ccf97d3936df98b154824d62fe`.

All six outputs are 262160-byte RRTEX001 images, 256x256 RGBA. Fixed compiled output hashes:

- frame0: `fcfeb0c00938ab1bbae791a24d705c18989b4b222e9d492ab4593d67dff79e98`
- frame1: `93c38e0dbe0fd044a5e4b88b59cf5937ea106461d96e6ae484bfbab99cd78c60`
- frame2: `2d5612b9cc8539d2b9d8045ace3d0319448ca730cbd99a1210d2e13327f72890`
- frame3: `f8239452fb91c2c4f3845990604d2d7990105807867f8617500eb1c687f5e215`
- frame4: same bytes as frame2, separate frame allocation and lease
- frame5: same bytes as frame1, separate frame allocation and lease

No caller supplies trusted hashes. The immutable owned image copy is verified, including RGB5 expansion and the existing TEXA255/AEM1 output policy. External decoded RGBA is not claimed to be the source compressed1555 allocation.

## Typed caller order

1. Actual canonical resolver/provider has authenticated RA source, and actual Application context is live.
2. `resolver->stageIpumFlags0(resolver, sourceIndex, compressed, diagnostic)` creates the existing CPU staging allocation prefix.
3. `ra_crowd_image50LA::Verifier::verifyRaSixMip4(*resolver, sourceIndex, compressed, sixBytes, images, diagnostic)` privately publishes an immutable complete set only after all six pass. Existing frame0-only API remains available but cannot bind an aggregate.
4. `Verifier::bindVerifiedRaSix(*resolver, images, actualContext, textureDiagnostic)` installs only a weak association. Keep `images` strongly owned until canonical population. Binding rechecks actual context, sealed provider/revision, current mounted bytes and earlier PST/IPU/TGA precedence through the existing resolver. It rejects a cached resource or a distinct live set.
5. Existing genuine texture-table acquisition/population constructs the aggregate and actual ordinary children. This patch adds no artificial table retain, no alternate table, and no population call.

All six leases must be complete, from the same exact stage/resolver/source, and unconsumed before aggregate effects. Each child subsequently checks its own unconsumed frame; earlier siblings are allowed to have adopted theirs.

## Ownership and partial effects

Adoption is **lifetime-once per staged frame**, not an exclusive-live token that can reset. The original aligned allocation is never copied into another compressed buffer. The stage continues to physically own the six allocations; each actual resource takes the typed lease for its own frame and records its exact identity in that frame's adoption state. Source references remain the existing independent1048F0 count, initialized1. Ordinary children register through actual29C780; the aggregate does not invent an ordinary list entry.

Host staging allocated/read the buffers before original resource effects. At source2507F0-equivalent construction, the canonical child adopts that same allocation once; this deliberately does not pretend a second allocation or read occurred. Kind2's source24E960/24E96C format1555/bpp16 and24EA40 F8bit0 are published in the constructor. Only original flags0's final mip is loaded in slot0. Compressed base00/codec18/count1C are exposed separately from decoded host-image pixels and palette fields remain absent.

Aggregate child ownership is stored before each load, preserving the existing partial prefix. Failures retain that prefix for terminal source-reference cleanup. Children retire in ascending order, clear views/adoption owner, release their lease, then unregister from the genuine Application lifetime list. `everAdopted` is never cleared. Immutable external leases can pin CPU backing after logical resource retirement; they cannot resurrect a consumed resource. Stage disposal requires no active adoption owners.

Serial reuse:

- A still-live canonical resource is used through the existing cache, never rebound or replaced.
- After retirement, the consumed stage/set remains unusable for reconstruction even if image leases survive.
- After the old set association expires and actual cache identity is clear, a fresh stage and freshly verified set may bind the same source under the same actual context and current provider checks.
- No token reset occurs behind surviving leases. No concurrent population contract is introduced; existing serial native ownership is retained.

## Build/type boundary

Only four existing files change. Every new out-of-line definition is in already-built `src/assets/original_standard_texture_resource50la.cpp`. The resource header forward-declares image lease/set; the implementation includes their full header. The lease header includes the resource header, with no reverse full-header include. Existing source-format/storage enum values are preserved by appending new values.

## Remaining live join

CPU `constructed24DA30` means genuine canonical resource construction only. This patch provides no GL upload, submitted-frame use, GPU-ready flag, original2544F0/443870 drain implementation, render callback, or model draw identity. Before a renderer consumes the image, it must retain the actual canonical child/lease and provide a checked retirement barrier before source owner teardown. Original F8bit0 changes downstream TEXA context as source requires; the decoded host image's fixed policy must be reconciled with that actual pass context by a future renderer rather than silently treating it as indexed PST pixels.

RA model material animation still requires actual model slot/draw ownership and genuine Application clock servicing. This candidate does not open RA gameplay or claim that all RA assets are ready.
