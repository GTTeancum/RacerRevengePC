# Authenticated Rider19 indexed body base-image policy

Frozen source-only candidate; no builds, tests, syntax/project parser execution,
image conversion or gameplay. Active repository unchanged. Three replacements
with exact preimages taken after Options square provenanceccc17a7 and IPUMc62aab5.
Use the additive patch if concurrent Rider fallback additions have landed.

`prepareOriginalUiRider19BodyMaterial50KM` admits only original19_1/19_2 PSM
pass1, DiffuseCompressed, opaque RGB and alpha-test OFF, paired with the exact
corresponding canonical UIPODTEX PST member in the actual immutable image bank.
The source must be standalone, non-CEL, non-IPUM kind1 with five original mips.
The published image must exactly equal the largest original256x256 mip, including
RGBA bytes. All CPU mip/palette evidence remains intact.

`convertOriginalUiRider19BodyMaterialSpan50KM` checks that same bank owner,
combined-bank index, immutable image identity, and source stored-mip4-of5 before
converting the first-pass raster policy. Reprepare after a real combined-bank
mapping; do not relabel an old bank policy.

This is the explicitly approved native highest-resolution, base-only GPU policy.
It does not assert original dynamic residentF4=0 or emulate memory pressure.
The common policy still requires source MXL0 and equal real min/mag texel filters;
ordinary Texture48E overloads still reject multiple mips and kind2. Source image
alpha, sampler, shader identity, fog and depth are unchanged.

The later SpecularMapCompressed pass is still required and rejected by the
current RGB-only renderer. This first-pass admission does not permit dropping
passes or asserting alpha disposability. Body palette alpha spans10..255 and
79..255, so a genuine stored-alpha owner is necessary for destination-alpha
specular blending. Hardware pixel parity and runtime acceptance are unverified.

Source PST/PSM hashes are in evidence/source-identities.json. Existing source
PST loader24EAD4..24EB9C reverses stored ascending mips into original level order;
last stored is largest level0. Original material sampling247F38..248034 supplies
MXL0 and matching linear source defaults; this policy retains the authored values.
