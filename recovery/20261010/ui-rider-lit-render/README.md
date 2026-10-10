# Original Rider Diffuse packet and native renderer extension

Frozen source-only increment, 2026-10-10. No build, test, syntax check,
project-parser run, shader compilation, game, guest instructions or image-codec
conversion was executed. Parent owns integration and centralized validation.

## Deliverable

Eight candidate files under `files/`, five exact replacement preimages under
`preimages/`. `candidate-manifest.json` records hashes and new/replacement paths;
`candidate.patch` is a text patch. Active repository was never edited here.

- Shared material admission gains a separate DefaultMT Diffuse/Compressed path.
  Existing ScreenObject entry retains its previous restrictions. Original
  material identities remain unchanged. FGE and GREATER/AREF0/KEEP alpha test
  are explicit; DAT, destination-alpha blend factors, PABE/FBA, unsupported
  samplers and texture kind2 remain rejected.
- New native span adapter maps that typed policy to explicit sourceDiffuse or
  sourceDiffuseCompressed mode. It does not assert registration/readiness,
  framebuffer-alpha ownership, texture publication, animation or model phase.
- New geometry owner consumes actual Scene48D semantic surfaces and the current
  source-index ModelPalette49S, never neutral_lod0 or an invented preview mesh.
  It retains actual material index and triangle order. Each source triangle is
  encoded as [a,b,c,c] in a native four-vertex slot, making the second triangle
  degenerate. Caller controls actual LOD/pass/draw ordering and real ownership.
- Native renderer gains an original-W stream for these modes. UV remains
  perspective-correct; packet color and F remain screen-linear. Depth remains
  the original positive integer24-bit packet Z. Ordinary screen/Main particle
  spans still use W1. All new GL resources have cold allocation, verification
  and cleanup paths; none has been executed.

## Source closure

Original ELF SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
VA to file offset is VA-0x100000+0x80.

The parent numerical stage retains exact DiffuseCompressed3376B0..339600 and
StripPreamble33FD50..340650 byte annotations. `evidence/Diffuse.words.txt`
adds the exact plain Diffuse323C20..325A80 packet. These are static labels,
not an interpreter, execution trace, or guest machine state.

Preamble33FE38..33FEB0 composes model with viewC0;33FEB8..33FF30 composes
projection80;33FF38..33FFB0 composes source screen140. The independent clip
matrix uses camera290, uploaded24BBAC..24BBC8; do not substitute projection80.
Compressed position quantization scales the first three result matrix rows.
3400D0..340140 scales selected-light direction rows by normal quantization
before multiplying each current node basis. No per-vertex renormalization,
inverse-transpose, imported diagnostic color multiply, or generic Lambert
replacement is used. Actual Rider19/pod19 model bit40 is established separately
by source240664..2406C8 and the authenticated PSG quantization values.

Plain323E68..323ED8 and compressed3378F8..337978 calculate all FOUR color lanes:
ambient plus2^23 bias, then three ordered light contributions after max(dot,0),
upper clamp2^23+255 and raw low-byte packing. Actual ambient W determines packet
alpha. Native code preserves binary32 operation boundaries and round-to-nearest
requirement; it does not emulate VU exceptional/denormal/accumulator modes.

Renderer-context1499A0, called24BD7C, constructs a distinct global-light matrix:
optional manager40 light negated look, RGB row0, ambient manager50 row3, first
three color W fields raw integer bits0x40. It is not model14A0A0 selection.
Rider owner recovered pod model1B3 bit0 clear, character bit0 set with actual
nonnull membership19C: character requires the real14A0A0 cache/zone owner.
The geometry interface therefore accepts explicit actual selected matrices.

Binder23CB20 establishes a DIFFERENT instance packet. Exact evidence is copied
in `evidence/binder-instance-23CA50-23CCA0.txt`:
-23CBB4/23CBC0..23CBFC uploads identity directions to rows20..23.
-23CC00..23CC14 zeros color rows24..27.
-23CA9C..23CAB0 replaces row27 with current global-manager50 ambient.
-23CC18/23CC20..23CC5C uploads identity model rows0..3.
-23CC60..23CC74 uploads row42=(1,1,1,1);23CA88 writes flags7.
Thus binder is original ambient-only Diffuse, regardless of the sun. Its source
normals are(0,1,0), from initializer3C9E70 and create1D3ED0; world positions and
UV come from the real binder quad owner. Source strip triangles are(0,1,2) and
(2,1,3). No white tint, fake normals, ScreenObject or fake scene is supplied.

Plain binder radial fog uses the original local-camera construction and
bias524288-end*slope + length*slope, clamp524288..524543, then (bits>>4)&255.
Native scalar sqrt is explicitly an approximation of source ELENG numerics,
not an EFU/device implementation. Model no-fog computation still emits F255;
material FGE is independent and must consume that value when enabled.
The no-fog loaded constant254 is NOT the final byte:323D30/3377D8 ADDi.w
adds vf0.w=1, then FTOI4 writes255<<4=4080. Packet IADDIU4080 corroborates
this. Earlier track-fog research notes mentioning a separate254 constant
must not be interpreted as a final F254 result. Real
Rider render context supplies end1200/start1000/slope255/-200 and white FOGCOL.

## Deliberate unsupported boundaries

- Unknown/partially clipped geometry rejects atomically. No fabricated clipping
  or gameplay projection is supplied. Only finite affine matrices and whole-
  inside source GS extent/positiveD24 are admitted.
- Compressed radial-fog branches are rejected pending their distinct palette
  coordinate/scaling join. Actual near Rider models use separately recovered
  bounds/distance fog selection; caller must supply its result, not assume it.
- Texture kind2 stays rejected by this ordinary material overload. Dedicated
  native original-data image-cache work is separate and must grant admission
  through authenticated provenance plus actual level0/TEX1 semantics.
- SpecularMapDual destination-alpha second pass is not admitted. Existing
  opaquePresentationAlphaOnly proof remains mandatory. No stored-alpha parity
  or downstream destination-alpha consumer is claimed.
- Texture banks are not silently combined. A complete Rider frame needs a real
  authenticated common bank (or a separately proven per-span bank interface),
  actual widget registration/lifetime, current character light selection,
  original animation clocks and scene submission order.
- Host rasterizer/texel interpolation, finite native math and GL fragment
  rounding are not claimed as byte parity with the original hardware.

Integrating this stage does not activate Rider rendering or replace the existing
race shader. It supplies the bounded actual-program path required by that join.
