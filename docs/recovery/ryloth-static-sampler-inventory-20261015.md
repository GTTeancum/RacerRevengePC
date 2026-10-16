# RA static sampler source inventory

Read-only analysis of the retained original source inventory; no game, build, decoder or syntax test was run.

Inventory SHA256: a18bfe8723fbe168189399e5f79c2f7c9f9617796243cc86bffd22587f51edca

All 44 static material families were inspected. None has a nonempty authored override for LOD calculation, maximum mip level, mag/min filters, L or K. The only nonempty wrap override is largeflag wrapModeVertical=1. Empty source fields are not treated as literal zero.

The existing recovered constructor sampler is {0,0,1,5,0,-75}. If every actual canonical context retains MXL0/MMAG1/MMIN5, the specific RA static bank can use base-linear host sampling. Full-world admission must verify each final context; inventory alone does not prove a constructed context or runtime override. Retain all source mip data even where this bounded sampler selects the base. General mip-filter support need not be invented as a prerequisite if these exact guards pass.

Material-specific alpha, blend, fog and raster policies remain separate. In particular crowd authors blending1, alphaReference120 and onePassAA1; largeflag/cityscape/cityscape02 author blending1 and zWriteDisabled0. Do not reuse the opaque four-fire lowering for every family.

## Explicit sampler/wrap fields
- textures\largeflag.psm: {"wrapmodevertical": "1"}
