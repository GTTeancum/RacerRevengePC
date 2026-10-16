# Complete RA source draw-plan candidate

Base17364cc; source-only authoring/manual review, no execution checks.

API: `ra_cold_aggregate50LA::Owner::prepareStaticWorldDrawPlan(diagnostic)`, after successful static image preparation and before GPU publication. Calls only the same world's private preparation. No new source registry, bank, material refs, geometry copy, Application, light manager or mutable owner injection.

The world owns44 `RasterInventory` records and one `SurfaceDrawPlan` for every actual surface across all1085 retained visual placements. Actual per-placement payload/LOD0/source-surface/material-slot and canonical handle proofs are rechecked; every placement and family must participate.176 collision-only records remain untouched. All49 image identities stay in the preceding same-owner image stage.

Raster records retain the full actual final canonical context, including integer field provenance, six sampler fields, video source fields, program token/class, group, pass flags, fillRGBA and current collector origin/finalLeaf. They do not compress source ALPHA/depth/AA state into a lossy host blend enum. Ordinary maps re-resolve the actual canonical child. Crowd uses the existing retained-placement selector; the inventory saves its invariant final raster context and the entire six-image range, never a frozen frame0 binding. Runtime must call the real current-map API again.

Surface records retain exact range and source identity indexes, color-presence, world-space root bypass and authoredUseLighting. Source normals, quantization and matrices remain available only through the same retained Placement/payload/vertex owner. No colorless surface gains a white color stream. Program inventory admits the authenticated43 PrelitCompressed-family shape and exact black/DiffuseCompressed identity; source program tokens are checked. The plan is an inventory, not execution admission, so missing-color semantics remain visible for later renderer validation.

Requirements count every diffuse-lighting, authored AA and dynamic-map surface. These remain unresolved implementation requirements; no supported/ready boolean can erase them. All surfaces are retained regardless of those requirements. Future renderer admission must discharge all relevant policies together.

Preparation is one-attempt; failures retain actual partial inventory and mark the stage terminal, and the aggregate becomes terminal. Getters return only a complete/current inventory. Close clears inventory before releasing material refs, after the existing publication preflight. Borrowed views expire at close and require current collector/source context; they are not GPU leases or completion receipts.

Files: existing world and aggregate headers plus one included `ra_static_world_draw_plan50la.inl`, automatically reached through existing main/GL includes. No new translation unit or activation. Actual counts beyond authenticated input extents have not been measured by execution.
