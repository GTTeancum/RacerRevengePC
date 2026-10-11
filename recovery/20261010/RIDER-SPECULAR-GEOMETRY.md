# Rider single-map SpecularMapCompressed packet leaf

Frozen source-only candidate. No builds, tests, syntax checks, project parsers,
texture decoding, gameplay, guest execution or emulation were performed. The
original executable was inspected only as static bytes. Active files unchanged.

## Deliverable

Two replacements, exact preimages and additive candidate.patch extend the
already-integrated original_ui_lit_geometry50km pair. Preimages match active
c62aab5. The existing Diffuse surface API and its rejection boundaries remain.

New public entries:
- prepareOriginalUiSpecularNormalAxes50KM(sourceModelWorld, viewC0, qNormal, out)
- projectOriginalUiSpecularMapCompressedVertex50KM(preamble, axes, rawPosition,
  rawNormal, out)
- projectOriginalUiSpecularMapCompressedSurface50KM(scene, lod, surface,
  palette, context, out, workspace)

The surface entry consumes actual multi-entry identity-palette Scene48D and
ModelPalette49S. It retains source topology/order, original material index,
per-node projected position, per-node diffuse-light rows, source RGBA, W, fixed
screen/depth and the existing whole-inside clipping boundary. It computes UVs
from the source model/root camera-normal basis, independently of the selected
bone. Imported material UVs are unused by this original program. No reflection
vector, normalization, UV flip, shininess or replacement illumination is added.

One-entry palette ownership remains rejected by the surface adapter. The direct
basis/vertex leaves can express the independently supplied source single-model
basis, but do not prove its runtime packet owner. Negative packed-color cases
retain the shared numeric leaf's strict rejection; this is not full VU numeric
or hardware pixel parity.

This package does not add a material/renderer acceptance path. In particular,
no SpecularMapCompressed pass is renamed Diffuse, and neither destination-alpha
nor fixed255 blending becomes accepted. The existing material reader already
retains the actual separate one-map passes and their original program strings.

## Primary source

Original ELF SHA and exact file offsets/region hashes are recorded in
`evidence/source-identities.json`. VA maps to file offset VA-0x100000+0x80.
Registration150B04..150B20 maps original name3A5780 to descriptor33B8C0.
The complete static listing covers payload33B8E0..33C740. This program is distinct
from race SpecularMapDualCompressed32C3F0, which has two texture outputs.

### Camera-normal basis and palette ownership

- Single-palette33B918..33B928 scales saved StripPreamble modelView rows by
  row42.y, then33BA58/60/68 dots original raw normal XYZ into camera XY.
- Multi-palette33BF90..33C048 explicitly reads rows0..3 and camera rows16..19,
  multiplies them, then33C050/58/60 scales the completed normal rows by row42.y.
 33C1C8/1D0/1D8 forms camera-normal XY. This basis is built once outside the
  per-vertex selected-bone palette walk. Substituting nodeWorld here changes
  the recovered algorithm.
- CPU packet constructor240920..240928 writes UNPACK tag0x6C048000, placing the
  source model matrix at row0.240930..24095C starts each 16-row node palette
  block at row182. Runtime2414F8..241514 copies model root world first, before
 241540..241584 copies every selected source-index node world.
- StripPreamble33FDA0..3402F8 computes and stores each node's transformed
  position/light rows. The specular program reads those rows per vertex for
  position and diffuse lighting while retaining its separate root UV basis.

### UV, color, fog

- UV is camera-normal XY * 0.5 + 0.5, in that source float operation order:
 33BAB0/33BAD0 and33C230/33C250. It is multiplied by Q for source STQ output;
  the shared native W stream provides the equivalent perspective interpolation.
- Color uses the same max(three light dots,0), ambient+2^23 and ordered three
  diffuse-color terms as DiffuseCompressed. Single33BAA8..33BB38 and
  multi33C228..33C308 pack all four RGBA lanes. The ambient alpha is retained.
- This program does not take Diffuse's model radial-fog branch. It emits byte
  F=255: LOI254 then ADDi.w using vf0.w=1 at33B9E8/F0 and33C198/1A0,
  FTOI4 at33BA80/33C1F0, and packed4080 constants at33BAC8/33C248.
  The unclipped branch repeats this at33C598/5A0/5F0/620. Material FGE remains
  a separate source state; F=255 with FGE enabled still uses original /256 fog.

### Additional closed quantization writer

Packet row42 is not a direct copy of all four serialized PSG quantizers.
2408C4..240900 uploads qPosition, qNormal, qU, and 1/qPosition, with DIV at
2408F0 and W store240900. The single-object constructor repeats it at
240A9C..240AD0. The fourth serialized float remains qV. This evidence can support
a later compressed radial-fog join; this candidate does not widen that path.

## Actual Rider assets and remaining blend ownership

-19_1/19_2 each have two ONE-map passes: DiffuseCompressed body, followed by
 SpecularMapCompressed specmap1. Their specular ALPHA tuple is A0/B2/C1/D1,
 with destination-alpha coefficient and no depth write.
- pod_wndw has THREE ONE-map passes, Diffuse/Specular/Diffuse. Its specular
 tuple is A0/B2/C2/D1 with FIX255, requiring coefficient255/128, not clamped1.
- specmap1 is an authenticated kind1 single-mip256x256 PST; every palette alpha
 byte is255. The original body19_1/19_2 textures are kind1 FIVE-mip PSTs from
 UI/UIPODTEX.RES. Their palette alpha varies10..255 and79..255 respectively.
 Thus body destination alpha cannot be replaced by a fixed value.
- Body mips need an explicit authenticated native base-only image policy,
 similar in lifetime/binding shape to the existing typed IPUM policy. Their
 ordinary Texture48E material overload correctly continues to reject five mips.
- The current RGB-only presentation target does not own source framebuffer
 alpha. The source-alpha audit under ui-alpha-reconstruction remains relevant:
 source format/mask/clear, independent alpha replacement, ordered consumers and
 separate opaque presentation must be established before enabling these blends.

Integration must retain all passes. Do not advertise this geometry leaf as a
complete Rider pod renderer or claim runtime visual acceptance.
