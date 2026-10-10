# 50GO: source wrong-way and marker-cursor update

## Recovered behavior

`beginWrongWayProgress50GO` represents original `1A2E30..1A328C`. This is the
player update's unconditional `1A1620` call after the admitted navigation branch
and before undertrack `1A162C`. Navigation must see the previous update's live
`10CC` bit 1. Clearing that bit before navigation, or permanently supplying zero,
loses the shortcut suppression consumed by `50GF`.

The represented order is:

1. `1A2E64` clears only player `10CC` bit 1, preserving every other bit.
2. If actual application `34C` is nonzero, `1A2E80` reads map track `48`
   (`ProgressNode` count), and the `1A2E8C` branch delay slot writes it to `FF8`.
   This exits without evaluating direction or touching the optional warning.
3. Otherwise read application `28C -> B4 -> byte58`. Bit 1 must be set and bit 0
   clear. A failing gate returns after the bit clear, without hiding a warning.
   This is exactly the same byte/predicate used by outer navigation `1B0B80`.
4. If `CD8` bit 0 is set or player `40` is nonzero, skip direction evaluation
   and continue to the warning hide branch. Neither path changes `FF8`.
5. Read vehicle `340 -> 288`'s **local** forward matrix column `E0/E4/E8`
   through `28A530`. The unused second output is local `D0/D4/D8`; neither
   getter refreshes world caches. Set forward Y to positive zero, then normalize.
6. Copy map node positions at live `C8C` and `C88` through `2A2410`; subtract
   current minus previous XYZ, set Y to positive zero, then normalize.
7. Take the source-ordered VU dot, then the original `13C770` polynomial.
   `1A30F0` sets bit 1 precisely when the returned angle is strictly greater
   than word `4016CBE4` and strictly less than word `407B53D1` (approximately
   135 and 225 degrees). Do not replace these with a dot threshold or libm acos.
8. If wrong-way, read viewport mode `application28C+8C`, then resolve optional
   `application28C+17C+4*vehicle5C`. A present owner receives the four ordered
   calls below. If not wrong-way, resolve that owner and hide it when present.
   An actual null owner naturally completes without any callback.

`f12` frame factor at entry is not consumed. There is no speed/displacement
predicate, cooldown, audio event or timer in this function. The map's authored
branch-selected live `C8C/C88` pair determines the direction, not a nearest node
or assumed `C88-1` predecessor. Only the application `34C` branch updates `FF8`;
the normal direction calculation never increments or resets the marker cursor.

## Arithmetic and native domain

The scalar normalization squares X and Y separately, puts `X*X + Y*Y` in the
scalar accumulator, then adds the separately rounded `Z*Z`. Exact zero takes
the actual static vector `434060 = (0,0,0,1)`, proven by initializer
`3C9910..3C998C`; it does not invent a forward axis. Nonzero uses scalar
`RSQRT.S` then three separate multiplications. The admitted native convention
matches the existing finite scalar core: rounded binary32 square root followed
by rounded binary32 reciprocal. Positive subnormal squared norms reject
explicitly; exceptional EE/VU silicon behavior is not claimed. Zero arising
from ordinary separately rounded products follows the source zero branch.

The dot is the existing native-core VU sequence:
`mul.xyz`, `vaddw.x(0,1)`, `vadday.x`, `vmaddz.x`. Products and additions round
separately; no FMA/reassociation. Map Y subtraction is performed before it is
discarded, so its consumed operands/intermediate must be finite. Local forward
Y is only copied and then overwritten, so it is not an arithmetic operand.

`13C770` is distinct from `originalAngle48G` (`13C1F0`'s rational atan),
`sourceAcos49T` (the later libm implementation), and `vehicle_contact48g.cpp`'s
private asin-like result. It has no public matching primitive to reuse. The
small private implementation preserves its source constants and operation
order. For input `x > 1`, return positive zero; for `x < -1`, return word
`40490FDB`. Otherwise, with `a = abs(x)` and separately rounded operations:

```
t = C372B84F + 42A8A11B * a
t = 43836812 + t * a
t = C3031CC3 + a * t
t = 41ED4AD5 + a * t
t = BFB9D5D7 + a * t
t = 3D06871E + a * t
result = x < 0 ? 3FC90FDB + t : 3FC90FDB - t
```

Each uppercase hexadecimal value above means its exact binary32 bit pattern,
not an integer converted to float. Signed zero follows the source comparisons.
All consumed floats/intermediates must be finite and arithmetic must use
`FE_TONEAREST` and strict binary32. Storage is fixed-size, backed, disjoint and
serially owned. The component allocates nothing and executes no guest code.

## Caller ownership and integration

- Reuse `UndertrackTrack50ET` from the live prepared MAP, and its matching
  `LapProgressTrack50GF`. The application bypass consumes the latter's actual
  `progressCount48`; it does not require an invented completed-marker list.
- Persist `WrongWayProgressState50GO.flags10CC` in the same canonical player
  flag owner used by the previous navigation's `LapProgressCrossing50GF`.
  Its `vehicleFF8` must be the same canonical lap cursor owned by `50GF`.
  The structure is an explicit call projection, not an additional independently
  ticking player. Copy back both results before any following source operation.
- Use current `SceneNode49P.localC0.v[8..10]` for vehicle `340->288`'s forward
  basis, current navigation `C8C/C88`, current `CD8`, and actual player `40`.
- Existing `NativeVehicleFrameFields49W.applicationByte58` is the correct
  configuration byte when its established outer navigation owner is used.
- Application `34C` must come from its real owner. A host development choice of
  zero may be explicit host configuration; it is not recovered menu state.
- A development host with no original warning owner may pass actual `nullptr`.
  Its own HOST HUD can independently display the resulting source flag. That
  does not instantiate or acknowledge any original HUD owner or effect.

`beginWrongWayProgress50GO` returns true when its native work succeeds, but
`continuation.pending.operation != None` means an effect remains pending.
It commits the represented player stores before exposing the first callback.
Input/arithmetic rejection is an explicitly unsupported-domain rejection and
preserves state, continuation and receipt. It is not evidence that the original
function had a transactional rollback. Already-pending work rejects a fresh
begin, preventing duplicate effects. `resumeWrongWayProgress50GO` may be called
only after the actual requested native effect has completed.

## Ordered optional warning effects

The caller supplies an actual native owner pointer, never a guest pointer word.
The requests expose call/callee/return PCs, integer arguments and floating-point
arguments. Registers with no represented semantic argument are zeroed; the
request is a semantic native call contract, not a fabricated CPU register image.

1. `1A318C -> 29DA10`: color `FFCC2929`.
2. `1A31C4 -> 29DB50`: integer position and `f12=0`. Source mode `0` produces
   `(320,170)`; mode `1` produces `(320,118)`; every other mode `(160,205)`.
   These are exact nearest-rounding results of source offsets `(0,0)`,
   `(0,C24FDF3B)`, `(-160,35)` added to `(320,170)`. The source captures those
   offsets before callbacks, so the continuation retains the result even if
   callback work changes viewport configuration.
3. `1A31E8 -> 29D940`: exact terminated original bytes `WRONG WAY\0`, copied
   from `3A9F38`. The native text pointer is supplied separately from integer
   argument words.
4. `1A3238 -> 29DA40`: `a1=230,a2=0,a3=255`; `f12..f19` are
   `(0,1.5,word3F99999A,0,-1,1,0,0)`; stack floats at `0` and `8` are zero.

The clear branch has one call, `1A326C -> 29DB10`. These providers are not
implemented or silently reported successful by this module. The optional
owner's actual absence is the only callback-free represented warning path.

## Source provenance and verification scope

All source reads were taken from the original `work/native/project/disc/SLUS_202.68`.
Its freshly checked SHA-256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
ELF load mapping is file offset `80`, virtual address `100000`.

Exact byte-range hashes, with exclusive ends:

| Range | SHA-256 |
|---|---|
| `001A2E30..001A3290` | `c539fd78fbe7592867c2ffc9b6be64093e758c96801bac292b2ec4d4ad18ffe5` |
| `0013C770..0013C8B0` | `4e0846b15d64caea4a0fde8f98825e471aa7ddd3e435755acdb9cc1c2513e9e9` |
| `0028A530..0028A578` | `c217eb39e41a32173bc446f65af69111b0c9291e87f89b5d8d1652dd8efc4b1e` |
| `002A2410..002A2430` | `e9616a9918b3c7b59cd4371a99df67cefc1ec7aad8cd7919b50ba9b8c0b7de32` |
| `002A2FC0..002A2FC8` | `c2dccea3593acb52aafef45c48afb09a52ab228b61c457d250e0eecd4bbc58df` |
| `003C9910..003C9990` | `69d783d2eb6459ac44960f38405988e13738731a9c4adf13862dbc1875515c88` |
| `003A9F38..003A9F42` | `d60bdc856e9e7fba80e7e36fe8e9fb1faa1a412239c5d6ed6efe91e7fd7ddb8a` |
| `001A15D0..001A1634` | `96502b43a7bccd078d16736a3e6e887813c541fb4aca038eac86483d7eb35cc0` |

Static source inspection only. No builds, compiler checks, tests, guest
execution, game launches, user-PC activity or existing-source edits were
performed for this component. No held 48U, excluded 49C, `ee.bin`, or sealed
artifacts were read. Integration and verification remain with the parent task.
