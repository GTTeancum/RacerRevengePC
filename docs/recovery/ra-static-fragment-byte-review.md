# RA static fragment byte leaf: source review only

Base inspected: `8e4f188459e8d98c3437ba9ab1f9217a25facfdf` in the central
repository, read-only. Deliverable is a new, isolated header at
`src/game/original_ra_static_fragment_bytes.h`. Nothing was inserted into a
build, renderer, profile decoder, material admission, or central checkout.

## Scope and evidence

The supplied `RA-FRAGMENT-RECOVERY.txt` records the authenticated static RA
numeric combinations and the inherited AREF correction. This pass did not
re-extract archives or independently reauthenticate the 44 source materials.
It inspected these existing source definitions directly:

- `platform/linux/native/vehicle_window_gl50ds.cpp`, source-composition GLSL
  branch: MODULATE floor/clamp, GREATER0 alpha test, fog RGB, signed-floor ALPHA,
  final clamp, and independent source-alpha replacement, in that order.
- `platform/linux/native/vehicle_shader_gl50cz.cpp`, source-pair branch:
  corroborating byte MODULATE; its distinct fog/alpha-test ordering changes
  neither alpha nor rejection because fog changes RGB only. Its racer-specific
  blend modes and opaque-alpha shortcut are not reused.
- `src/game/original_gameplay_blur_leaf.h`, `gameplayBlurSignedFloor128`:
  reused verbatim through the existing header, without invoking the blur blend
  wrapper, effect producer, or lifecycle operations.
- `src/assets/original_ui_material50kg.cpp`, bounded one-map state checks:
  confirms the canonical numeric selector meanings; UI-only admission guards
  remain unchanged and are not treated as RA authorization.

This is the existing native byte-stage arithmetic applied to explicitly
already-sampled/interpolated byte operands. It is not proof of PS2 filtering or
interpolation precision, nor permission to quantize an actual fractional
sample/packet operand to bytes. That upstream contract remains unestablished
for a renderer until independently reviewed and authorized.

## Boundary and ordering

`modulateAndTestRaStaticBytes` rejects unsupported numeric selector tuples
before arithmetic, evaluates RGBA MODULATE, and returns either rejection or a
privately constructed arithmetic token. Rejection has no surviving token and
must cause neither color nor depth writes. No writes occur in this header.

The surviving token exposes its modulated RGBA. Its `compose` method requires
separately typed, already-resolved preblend RGB. The external owner must perform
any required fog RGB operation between these two calls. For disabled fog it
supplies unchanged modulated RGB. This leaf deliberately supplies neither a fog
value nor a default bypass and does not attest that the supplied RGB has valid
provenance. There is no combined "ready fragment" operation. Alpha remains
the surviving MODULATE alpha throughout; callers cannot substitute destination
alpha or an altered fog/coverage alpha in `compose`.

Enabled blending requires a typed current destination sample, obtained in
source primitive order by a separate owner. It is not a framebuffer snapshot
contract. Destination alpha is unused. Disabled blending also receives the
typed argument but does not use it or request any destination fetch itself.

The numeric selector subset admits exactly four projections of the observed
profiles: ATE off/AREF0/ABE off; GREATER0/ABE off; GREATER0/ABE on; and
GREATER120/ABE on. It requires TCC1/TFX0, ATST6, AFAIL0, ALPHA0/1/0/1/FIX128,
COLCLAMP1/FBA0/PABE0 even where those selectors are inactive. This check is not
a complete final-map profile, family authentication, or runtime admission.

## Manual numeric review

Byte products for MODULATE lie in [0,65025]. Nonnegative integer division by128
is exact floor; clamping the quotient to255 is identical to floor after clamp
on this nonnegative domain. All four channels use that operation before the
alpha test. Threshold rejection is `source.alpha <= aref`, giving strict
GREATER0 or GREATER120 acceptance. With ATE off, alpha zero survives.

RGB differences lie in [-255,255], so `(Cs-Cd)*As` lies in [-65025,65025].
The existing signed-floor helper negates only this bounded input; its largest
negated-plus127 intermediate is65152, comfortably inside int32. Its result
lies in [-509,508], and even the loose bound after adding destination is
[-509,763]. Final saturation therefore cannot overflow and covers factors
above128. Negative nonmultiples retain floor semantics rather than C++ signed
truncation toward zero. The blur wrapper's FIX<=128 restriction and missing
saturation are not inherited. Output alpha is the original surviving source
byte, with no source-over accumulation or normalized alpha factor.

## Remaining blockers and verification limits

Sampling/interpolation ownership, static fog production, projected depth,
per-sample destination acquisition, crowd AA/coverage, actual profile binding,
and complete-world renderer authorization are all outside this deliverable.
No callbacks or readiness flags stand in for those owners.

Only source reading and editing were performed. No tests, builds, syntax
checks, sanitizers, shader/gameplay checks, converters, project execution, or
runtime activation were performed. The new header is uncompiled and untested.
