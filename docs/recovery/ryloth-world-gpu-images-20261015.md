# Exact RA full-world image publication candidate

Base: 607cf2e. Source-only authoring and manual inspection; no execution checks.

## API and actual ownership

`MaterialTextureGl50EB::createRaStaticWorld(request, const ra_cold_aggregate50LA::Owner&, diagnostic)` obtains only the private actual aggregate world. There is no mutable/public world injection. Aggregate storage changes from unique_ptr to shared_ptr solely so the existing GPU image owner can retain that same world. Construction remains private in the existing aggregate stage; no source/manager/bank duplication.

Before any GL allocation: require nonterminal live aggregate, current 44-family/49-image cold stage, current Application/context/provider revision/collector, actual PSM bytes/DefaultMT/map token, current source texture bytes/name/positive refs, and exact canonical child and bank identities. Crowd uses the existing retained-placement current-map proof, including all six real host-image leases. This operation never samples a clock or advances animation.

Every actual final canonical sampler must equal the proven tuple `{0,0,1,5,0,-75}`. Horizontal wrap must be repeat; vertical is repeat except exact authenticated `textures/largeflag.psm`, which must clamp. TEXA is checked against the retained ordinary CPU lowering or the actual crowd map proof. Unsupported states fail before leasing/allocation. Original raster/blend/light fields are not overridden or certified by this image-only stage.

All 49 immutable bank entries are uploaded through the existing GL owner with exact index+1 identities. All original mip vectors, canonical levels, source compressed allocations and six frame leases remain retained. Only level zero is uploaded; no GPU mip support is claimed. The source MXL0/base-linear subset determines this bounded admission. Identical frame bytes retain distinct bank/child identities.

## Lifetime and failure

Private world image lease is acquired before existing `create` allocation/GL calls. The publisher holds a strong shared_ptr to the actual world. World `canClose` rejects image or future renderer publication, and aggregate close preflights world before touching particles or any other prefix. Aggregate destruction keeps existing checked-close/terminate discipline; it cannot silently orphan an active publication.

Existing upload/deletion failure semantics remain: complete failed-upload cleanup releases the source lease; incomplete cleanup retains GPU storage and source for actual close retry. Successful close clears the lease only after checked GL texture deletion finishes. No caller drained boolean exists. Existing four-fire route is unchanged apart from mutual exclusion with the new bank.

A private future renderer publication pointer is reserved, with no acquisition/retirement API yet. Generic validBorrow rejects this bank until the real typed renderer lease is implemented; the ordinary track route also explicitly rejects the full-world bank. This prevents existing generic/four-fire/racer rendering from accepting it prematurely. Future renderer work must install its real lease, retain actual image publisher/context lifetime, drain through genuine completion and checked cleanup, and only then retire the lease. This patch does not mint completion receipts or native scene stamps.

## Remaining joins

No call activates the API. Caller must supply the real current native GL request and explicit byte budget; no Application/context is created. No full-world mesh/render path is admitted. Black/DiffuseCompressed remains a genuine static normal/light lowering dependency, with dark-gray source texels, and cannot be routed through racer lighting or replaced with prelit/constant-black output. Dynamic crowd draw-time map selection must continue using the canonical child-to-bank mapping at each actual model draw. Full-world blend/depth/alpha/order lowering and completion/fire-subset mapping remain separate.

Five touched files, no new translation unit: existing material_texture_gl50eb.cpp compiles the implementation and includes the actual aggregate definition. No generated data or output pins change. No tests, builds, syntax checks, converters or gameplay were run.
