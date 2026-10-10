# Static source and untextured color semantics

No original instructions or native candidate were executed for this work.
Existing source listings were read directly. No disassembly or other project
helper was run. The parent owns validation and active integration.

## Actual authored material

Read unchanged authenticated member:
`../../authored-menu-data-inspection/members/mainMenu/ui/textures/notexture.psm`.
Its recorded provenance is in that directory's `archive-member-identities.json`
and `material-authored-dependencies.json`. It declares one ScreenObject pass,
zero maps, textureMappingOnOff0, alphaBlendingOnOff0, alphaTestOnOff0,
foggingOnOff0, depthTestMethod1. It contains no texture filename or sampler.
The material worker joins this with the actual associated template and runtime
override owner. The renderer does not recreate those defaults itself.

## Original state and primitive source

Read `../../ui-alpha-reconstruction/material-registration.words.txt` and
`material-one-map.words.txt`, which name authenticated source ELF SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

- 0x247214 reads totalTextureMaps. At 0x247228 the source branches to its
  counted texture-loop guard, so zero does not execute the TextureMap_1 lookup.
  The material worker's static source inspection establishes the shared
  0x2476A0 state-builder path for zero/one maps.
- 0x247CC0 reads textureMappingOnOff into stack+0x9C. 0x247D78 loads that value,
  0x247D90 shifts it by4, and 0x247DD4/0x247DD8 emit the PRMODE register0x1B.
  Thus this authored zero is the actual TME flag, not an absent-map fallback.
- 0x247CF0 reads alphaBlendingOnOff into stack+0xA4. 0x247D80 loads it and
  0x247DA0 shifts it by6 into the same PRMODE word. The zero disables blending.
  The runtime override at0x247D68..0x247D70 can force ABE1 and is not ignored by
  the material policy. The native caller must supply the authentic override.
- Read `../../ui-transition-callback-reconstruction/evidence/overlay-construction-speed.txt`:
  0x1D66F4 supplies the NoTexture literal; 0x1D6754/0x1D6758 writes source
  per-vertex alpha63, with sibling packet write at0x1D6768/0x1D676C. The current
  aperture owner preserves this metadata and both rectangle geometries.
- `../../ui-depth-reconstruction/SOURCE-EVIDENCE.md` owns the existing packet
  depth and registered pass-order reconstruction. This renderer preserves the
  supplied final order and does not replace it with a texture-sorted queue.

## Primary hardware interpretation

Sony GS User's Manual v6.0: PRMODE p117 defines TME0 as disabling texture
mapping and ABE0 as disabling alpha blending. RGBAQ describes unsigned raw
vertex channels. Section3.8/p69 and ALPHA p100 define RGB blending with an
alpha divisor128; source alpha is stored independently. Section3.4.9's
MODULATE scaling belongs to the texture stage. These imply direct raw RGB and
raw stored alpha for the admitted TME0/AA10/fog0 path, with one `255/128`
conversion only when an RGB blend coefficient is needed.

Primary manual text was read via this
[archival Sony manual copy](https://www.scribd.com/document/784545197/GS-Users-Manual).
An additional [Sony PDF archive](https://github.com/ninjadynamics/PS2Docs/blob/main/GS_Users_Manual.pdf)
was located, but the web tool could not parse its octet-stream raw download.
No emulator implementation, emulator execution, or software GS was introduced.

The shader implements the bounded native normalized-color mapping, not a
claim of bit-identical source integer rasterization or blend rounding. The
existing opaque presentation-alpha contract is unchanged and is not source
alpha recovery.
