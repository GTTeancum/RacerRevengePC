# Original UI stored animation and reverse-loop recovery (50KG)

This is an isolated, source-only stage. No project decoder/importer, compiler,
syntax check, test, game, or Git command was run. No active repository file was
changed. The original UIMAIN upload is still pending, so this stage does not
claim an observed arrow frame count, dimensions, palette kind, decoded image,
or successful rendering. Prior asset-catalog and indexed-atlas stages remain
frozen.

## Actual original resources

The retained historical catalog authenticates `UI/UIMAIN/UIMAIN.RES`, 370304
bytes, SHA-256
`b2019a9dcd541baeba90ef3e3b57480f9a023155290aec84a195ecdf1b990f84`.

| Member | Logical offset | Bytes | SHA-256 |
|---|---:|---:|---|
| `ui\textures\arrwrght.atm` | 344064 | 10392 | `f8f84c469b1f865ae1688f488e980919081d71a2b3473b5de1f707708ef008c3` |
| `ui\textures\arrwleft.atm` | 368640 | 10392 | `2a7bc92483e25499c8b84f50a3e196322fb4f4b585bf816c91a456ab107514ea` |
| `ui\textures\arrwrght.psm` | 589824 | 1191 | `a34a9371a36175d72e007884a8ea354dff3e49132cccf48368bfb4ab8174fd77` |
| `ui\textures\arrwleft.psm` | 614400 | 1191 | `ccc74dfcdc4363f9283586a8f76a3de06a4f935b3a90ef166d5fded26efaaa3e` |

The actual material receipts retain one ScreenObject pass, a same-name texture
map, horizontal and vertical clamp=1, fog=0, alpha blending=1, alpha testing=0,
and `TextureAnimation=unique`. Both supply FPS=16, reverse-on-end=1,
loop-to-frame=0. Neither supplies InitialFrame or Looping. Omitted or empty
material fields are not assigned new visual defaults here.

Original constructor `29B950` proves the missing animation defaults: initial
frame=0 (`29B9C0`), FPS=1 (`29B9E4`), looping=1 (`29BA08`), reverse-on-end=0
(`29BA4C`), loop-to-frame=-1 (`29BA70`). The material overrides therefore give
the arrows initial frame 0, FPS 16, looping true, reverse true, loop-to 0.
The string `unique` at `3BDDE0`, compared at `29BA88..29BAA8`, marks separate
animation ownership. Keep one state per material instance; do not share a
global arrow phase just because the two arrows have the same settings.

## ATM frame splitting

Original loader `102230` reads three little-endian u32 values:

1. Revision, checked against 1 at `102274..10227C`.
2. Authored frame count, read into owner+0x20 at `102304..102318`.
3. Storage selector, read at `10231C..102334`.

For selector 0, each loop iteration calls the original PST stream reader
`24E880` at `10239C`, using the same advancing stream. The count controls the
loop at `1024F0..1024F8`. Frames have no intervening length table or alignment
padding. Original outer dimensions are copied from the first texture after
loading; the splitter retains each frame's own dimensions and does not impose
equal-size frames without caller-specific proof.

Nonzero selector follows a separate size-prefixed TGA route at
`1023AC..1024E0`; the third word is not a reserved-zero field. That route is
explicitly unsupported here.

`splitOriginalUiAtm50KG` takes immutable member bytes and returns exact
member-relative `pstOffset`/`pstBytes` ranges in authored order. The enclosing
archive/member must first pass `OriginalUiArchive50KG` authentication. Each PST
extent is determined from its own revision/flags/kind/mip headers:

- Kind 0: 16 RGBA palette entries, packed two texels per byte.
- Kind 1: 256 RGBA palette entries, one texel per byte.
- Both: 12-byte PST header, then palette, then each 16-byte mip header and its
  original pixel payload. Header order is height, width, two surface words.

The admitted subset matches `decodeTexture48E` plus the kind-0 atlas stage:
revision 1, flags 0, zero surface words, 1..7 ascending mips, power-of-two
dimensions 16..1024. Unsupported kind 2, non-power-of-two images, small mips,
alternate flags, and TGA storage remain explicit failures. The safety envelope
is 8 MiB input and 64 frames; caller limits can reduce these. These are host
admission limits, not claims about original engine maxima. The entire member
must be consumed exactly. Cancellation or rejection leaves output unchanged.

This operation splits original bytes only; it does not decode or invent pixels.
For each frame, use the original member and returned range in
`OriginalUiImageSource50KG`, then let the staged ScreenAssets bridge use the
source-backed `decodeTexture48E` and `parseTexture48B` path. The bridge owns the
immutable CPU textures; the caller assigns a stable, distinct image identity
to every frame and resolves the selected ordinal to that bank identity.
No font CEL should be attached to an arrow image.

## Reverse-loop state and timing

`original_ui_texture_animation50kg.h` recovers original `29A020` plus its
outer controller's timestamp guard, without changing the existing forward-only
`AuthoredTextureAnimation` used by gameplay materials.

Initialization accepts explicit source-authored config plus the frame count
obtained from the ATM header. `originalUiArrowAnimationConfig50KG()` contains
the exact two arrow materials' overrides and original missing-field defaults.
Use it only for those authenticated material identities, not as a generic UI
default.

The caller supplies its existing sampled uint32 application-millisecond clock.
There is no OS clock or render-cadence approximation. The outer original
controller (`29AC90`) compares its previous timestamp to application+0x230 and
skips duplicate samples (`29AC9C..29ACB0`); it also owns pause policy
(`29ACB8..29ACC8`). The native caller must preserve that pause gate rather than
advancing merely because it draws. The numeric helper itself does no pause,
callback, page lifetime, or clock-source invention.

The native state combines the controller and numeric previous timestamps since
they advance together in the admitted no-callback path. Both start at
0xffffffff. A first timestamp equal to that sentinel is skipped exactly as the
original controller does; the first non-sentinel update has zero elapsed time.
A later update that stores 0xffffffff causes the next numeric update to use
zero elapsed time again, matching the original sentinel check.

For each permitted new sample:

1. Subtract uint32 timestamps with wraparound.
2. Convert high-bit elapsed values using the original sticky-half sequence,
   then multiply by exact binary32 word `0x3A83126F` (`29A120..29A160`).
3. Multiply seconds by FPS, then separately add or subtract the advance from
   phase according to direction bit 2 (`29A178..29A1A8`).
4. If phase >= frame count, first compute `fmod(phase,count)`. When reverse
   bit 0 is set, set phase to `(count-1)-remainder` and set direction bit 2
   (`29A1C8..29A20C`). Otherwise follow original loop-to modulo or direct
   remainder (`29A210..29A24C`). A non-looping config sets stopped bit 3.
5. Only in the ELSE branch, if phase < 0, set phase to
   `abs(fmod(phase,count))` and clear direction bit 2 (`29A27C..29A2C8`).
6. If stopped, set phase to count-1 (`29A2CC..29A2F0`).

This deliberately is not a generic ping-pong period formula. A large advance
that crosses the upper end can produce a phase in (-1,0), and the source does
not immediately reflect it again. Signed truncation by the original frame
getter's EE CVT.W.S (`29AFDC`, `29B028`) selects frame zero in that interval.
Duplicate draws at the same timestamp must not reflect it prematurely.

The native helper enforces the project's separately rounded IEEE binary32,
FE_TONEAREST arithmetic convention and keeps CVT.W.S signed truncation
independent of host rounding. That is the project's established convention,
not a claim of every EE denormal/exception/hardware edge-case equivalence.
Config admission is finite initial phase in [0,count), positive FPS <=60,
1..64 frames and loop-to either -1 or a valid frame. Unsupported configs are
rejected, never repaired with clamps or substituted forward motion. Rejection
leaves state and selected-frame output unchanged.

## Evidence and remaining work

`research/` retains fresh static disassembly, exact original opcode bytes,
arrow catalog/material receipts, and source identities. `provenance.json`
authenticates the ELF, source intervals and dependencies. `manifest.json`
identifies every staged artifact; `changes.patch` contains only new files.

Original UIMAIN bytes are still required to read the real ATM headers and
verify frame count, dimensions/kind, exact ranges and visual content. This
stage intentionally contains no guessed frame count and no synthesized static
arrow. Runtime/build/test wiring and renderer/main integration remain outside
this source-only deliverable.
