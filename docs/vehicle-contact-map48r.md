# Vehicle/contact source map, 48R

## Finding and supported next branch

`219420` is a real stage between detector execution and force/response execution.
It performs a filtered, ordered candidate loop; calculates projected overlap;
updates both bodies' contact-partner lists through virtual slot `98`; and appends
spring records through `2963E0`. It is not equivalent to immediately feeding
`295630` output to `2964C0`.

The bounded implementable branch is:

- explicit initialized candidate order and count, without world enumeration;
- bounds tags **1 and 6** only, using the actual `1202E0` branches;
- ready scene caches, or dirty parentless scenes whose local matrix is copied to
  the world cache;
- body callbacks explicitly identified as `165430`, preserving shared/self body
  identities;
- the existing exact `2963E0` record append semantics.

Dirty parent traversal, other bounds types, unknown virtual targets and actual
first-substep spatial enumeration remain reached boundaries. This is a handler
projection, not a new complete vehicle scheduler. The separate clear-list helper
`165420` belongs after body integration, not inside this handler.

## Frozen evidence and reproduction

Run `python3 -B evidence/vehicleContact48R/map/freeze_map.py` from the repository.
It reads only the already authorized ELF and hash-pinned prior annotation files,
then writes only its own `48R/map` directory. It does not read guest RAM, transfer
files, execute original code or replace a spatial/virtual call with a stub.

The ELF SHA-256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The actual ELF32 little-endian program header is checked before extraction:
`PT_LOAD(type=1, offset=0x80, vaddr=paddr=0x100000, filesz=2972160,
memsz=3470848, flags=7, align=128)`. Every frozen extent must belong to exactly
one file-backed load segment. Thus these words use
`file_offset = VA - 0x100000 + 0x80`; BSS state is not misrepresented as ELF data.

`original-words.json` stores all 1,857 words in 11 bounded blocks, their hashes,
annotation sources, branch/call destinations and extent evidence. The local
scalar decoder is hash-pinned to the prior 48P decoder. Unknown instructions
would remain `[raw word; no local annotation]`; this set has none after exact-word
matching against independently frozen prior annotations. Reusing an annotation
does not establish execution coverage. `dispatch-data.json` separately freezes
the bounds jump table, the two known body-vtable slots, initial candidate-mask
word and profiler-name strings. `freeze-summary.json` records current hashes.

| Block | Half-open original extent | Extent evidence |
|---|---|---|
| Handler | `219420..219810` | Calls `219C14/219C6C`; return `2197FC`, delay `219800`, then three zero words before next function |
| RunSimulation | `219810..21A000` | Prior `recover48b.py` slice; return `219FF8`, delay `219FFC` |
| Scene position | `28B1A0..28B2B0` | Call `2194D4`; return `28B2A8`, delay `28B2AC` |
| Scene axes | `28A700..28A888` | Call `2194E8`; prior `recover48e.py` extent; return `28A880` |
| Candidate bounds | `1202E0..120E68` | Call `219524`; switch `388AB0`; common return `120E60` |
| Dot | `106E70..106E98` | Call `2195B4`; return `106E90` |
| Vector construction | `106790..1067AC` | Calls `2196A0/219730`; previous frozen extent; return `1067A4` |
| Inverse point | `28BD50..28BEE4` | Calls `2196F0/219708`; return `28BEDC` |
| Contact-list clear | `165420..165428` | Known body vtable `3CE000+94`; return and store in delay slot |
| Contact-list insertion | `165430..1654B0` | Known body vtable `3CE000+98`; return `1654A8` |
| Record append | `2963E0..296484` | Call `2197C0`; existing 48N proof; return `29647C` |

The `3CE000` PhysicsRigidBody identity is prior source-backed evidence in
`simulation-recovery48c.md` and `body-wrapper48h.md`; the two slot words are
independently rechecked against this ELF. This does **not** prove that every
candidate's body has that type. Native callers must supply an explicit supported
identity binding; no absent RAM capture is implied.

## RunSimulation order and gates

The caller saves `a0` as vehicle, `f12` as the per-substep factor, and `a1` as the
first-substep flag. It contains no early natural return and does not establish a
Boolean return value. Its final `JR` plus delay slot is `219FF8/219FFC`.

1. **Entry/initial setup, `21982C..219ACC`.** A one-time indexed-name registration
   path uses the literal `PodVehicle::RunSimulation`, direct calls `3090E8` and
   `309618`, and globals. The gravity magnitude path depends on component
   `vehicle+348 -> +104`. Its inactive branch uses `vehicle+37C`. The active path
   considers aero `+EC`, main-body cached speed `+34`, and a world-up value read
   through `28A700(vehicle+190, 0, ...)`; it contains initialized constants
   `0x4212AA7F`, `0x43C13334`, `0x3F34FDF4` and endpoint-clamping branches. This
   map does not simplify those branches into a new gravity rule. The chosen
   float is passed to the gravity component's virtual slot `+7C` at `219A88`.
   Engine `+34` is set iff vehicle `+64` equals 1 or 2; engine `+38` receives
   vehicle byte `+B9C` bit 1. These writes precede the detector gate.
2. **First substep (`a1 != 0`), `219AD4..219C20`.** `122350` is called twice on
   `vehicle+358 -> +38`, each with capacity 64. The first writes count/list
   `vehicle+58C/+590` using arguments `(t0,t1,t2)=(2,1,1)`; the second writes
   `+488/+48C` using `(1,0,0)`. These are actual enumeration dependencies, not
   existing supplied-list work. The caller resets `+384` and concatenates the
   first list followed by the second into `+388`, stopping append operations
   when the combined count reaches 64. Neither source order is sorted/deduped.
   Then detector `+358` slot `+3C` is called at `219BDC`. If **both** `+35C` and
   `+838` are nonzero, detector `+36C` slot `+3C` is also called at `219C08`.
   The contact handler `219420` is called at `219C14`, after these detectors.
3. **Later substep (`a1 == 0`), `219C24..219C74`.** If vehicle `+380` is zero,
   detection **and** handler are both skipped. Otherwise the main detector's
   pointer fields `+70/+74` are temporarily set to vehicle `+58C/+590`, its slot
   `+3C` runs at `219C50`, and these pointers are restored to `+384/+388` before
   `219420` at `219C6C`. The restoration store to `+74` is in the handler call's
   delay slot. There is no optional second detector in this branch.
4. **Post-handler forces, `219C78..219CF8`.** In exact order: component `+348`
   direct `1B1800`; engine `+34C` direct `2E7720`; gravity `+344` direct `2827D0`;
   conditional engine application `2E7C10` when the external game-state chain's
   byte `+58` bit 1 is set; aero `+350` virtual `+78`; spring `+354` virtual
   `+3C` (known `2964C0`); component `+348` direct `1B1A30`. Every call gets the
   saved substep factor. `+344` is gravity; **`+354` is spring**. Neither direct
   component call may be skipped merely because detector and spring kernels
   already exist.
5. **Coupling and integration, `219CFC..219E9C`.** If both `+35C/+838` are
   nonzero, direct helper `2188B0` runs. Main body `+1C0.xyz` is compared component
   by component with globals `434630/34/38`; a non-equal component reaches
   `223030`. Another one-time indexed-name path uses `PodBody`. Main body
   `+340` slot `+3C` runs at `219E80` (known `167C40`); its slot `+94` runs at
   `219E94` (known `165420`, clearing `+20C`); then `219120` runs at `219E9C`.
6. **Late feedback/writeback, `219EA4..219FE0`.** Vehicle `+380` becomes the
   Boolean nonzero test of spring `+44`. Optional vehicle `+3C` enables a
   `14F620` transform from fields `C60/C50`, copies the 64-byte result to scene
   `+190 -> +100` and then to `+C0`, copies vehicle `+C70` into body `+70`, and
   computes the body's cached speed `+34` with scalar SQRT. `28BBB0` transforms
   vehicle `+BA0` through scene `+190` into vehicle `+BB0`. Component `+348 ->
   +104` is copied to engine `+3C` and aero `+E0`; spring `+3C/+44` is copied to
   aero `+D0/+D4`. These are **late** feedback writes, after current aero and
   response, not inputs that may be moved to the front of the same substep.
   `11FF90(vehicle+694, scene+100)` runs before the true return.

The map preserves every caller word, including branches not implemented here.
Known 48Q detector success is always 1 on its natural supported returns; query
hit is a distinct result. This caller does not branch on either detector return
or handler `v0`, so a synthesized Boolean success must not alter scheduling.

## Handler field contract

Offsets below are original fields, not an instruction to use guest addresses in
production. Native identities and caller-owned spans should represent them.

| Owner / field | Use and earliest consumption |
|---|---|
| Vehicle `+1B4` | Mandatory scene for initial position and axes, even when candidate count is zero/negative |
| Vehicle `+58C`, `+590[i]` | Signed candidate count (reloaded each loop test) and ordered candidate pointers |
| Global `3B14B0` | Candidate mask, reloaded for each candidate; initial ELF word is zero, **not** evidence of a permanently zero runtime mask |
| Candidate `+A4` | Flags ANDed with that mask before any bounds/type/body field is read |
| Candidate `+88`, `+8C` | Bounds type and corresponding payload, only after mask acceptance |
| Candidate `+60.xyz` | Cached center used for overlap and final surface point; separate from temporary bounds-center output |
| Candidate `+80` | **Not read by this handler** on tags 1/6; do not substitute it for the computed candidate radius |
| Vehicle `+364 -> +80` | Own scalar radius, read on each mask-accepted candidate after bounds calculation |
| Vehicle `+190` | Scene used for inverse-point operations, only after strict overlap succeeds |
| Vehicle `+340` | Main body, first dereferenced immediately before its reached callback |
| Candidate `+A0` | Other body identity, first loaded on overlap just before callbacks; reloaded for record append |
| Vehicle `+354` | Spring append target, reached after both body callbacks |
| Spring `+84/+88/+8C`, `+D4` | Signed append count/capacity/backing and last material; exact 48N append behavior |
| Body `+20C`, `+210[0..9]` | Known `165430` callback's signed count and deduplicated contact-partner list |

The handler's prologue lazily initializes `434718` to `0.5` under byte flag
`gp-0x7894` and `434720` to `1` under byte flag `gp-0x7890`. The latter scalar is
not subsequently read by this handler. An initialized-constants native route
must name that assumption rather than inventing a missing live global capture.
The multiplier used at `219578` is the current `434718` value.

## Scene helpers and aliasing

Both mandatory calls use `scene = vehicle+1B4`, reference scene zero. For a valid
nonzero scene, `28B1A0` first checks scene byte `+190` bit 0. Ready caches do not
consume `parent +180` or local-matrix data. If dirty, a null parent copies all 64
local matrix bytes at `+C0` to world `+100`, preserving every word, and sets bit 0
without changing the other flag bits. A nonnull parent reaches `2897B0` and
`289940`; this is the explicit parent-traversal boundary. The getter then copies
world translation `+130.xyz` to stack `F0`, whose W was initialized to 1.

`28A700` follows and reads the same now-ready cache. It copies world forward
`+120.xyz` to stack `80`, and up `+110.xyz` to stack `90`. The forward result is
never used by the handler. These getters copy bits; they do not perform overlap
math or justify eager finite validation of all scene data. Position/up become
numeric inputs only on a mask-accepted candidate. Empty/negative-count and
all-mask-rejected paths still perform the cache side effect, but do not consume
bounds, own radius, inverse scene, body links or spring records.

The later `28BD50` inverse scene may be the same original object as `+1B4`, or a
different one. Explicit native scene indices must preserve that relationship.
If shared, the earlier refresh is visible to the inverse call; using two copies
would invent stale state. It has the same ready/dirty-parentless/dirty-parented
gate. The first inverse call refreshes when necessary, and the second reads the
resulting cache. Main and other body may also be identical: the two callbacks
then operate sequentially on one list, so the second can observe the first's
insertion. Distinct native buffers must not silently represent one identity.

## Bounds `1202E0`, narrowed tags 1/6

The actual seven-entry table at `388AB0` maps tags 0 through 6 to:
`120378,1203FC,12044C,1205EC,1209F8,1208E4,1209A8`.
Tag 1 loads local center/payload `+00`, local half/payload `+10` and the matrix at
payload `+80`; tag 6 loads center `+40`, half `+50` and matrix `+00`. Each copies
complete vec4/matrix words, then enters common code `120C4C`.

For each XYZ lane, retain these operations with individually rounded binary32
multiply/add, no contraction/reassociation:

1. The temporary center output is `((M0*Cx + M1*Cy) + M2*Cz) + M3`, with output
   W = 1. It is calculated even though `219420` never reads this temporary
   center. A finite-domain native guard must not skip its actual XYZ arithmetic.
2. Build sparse axes `(Hx,+0,+0)`, `(+0,Hy,+0)`, `(+0,+0,Hz)`. Each is rotated
   by the complete three-term matrix expression, **including all zero
   multiplies and additions**, followed by an additional XYZ `+0` at
   `120D04/120D38/120D6C`. Collapsing these to single scaled matrix columns can
   change signed-zero results and input consumption.
3. Each output half-extent is `abs(rotZ) + (abs(rotX) + abs(rotY))`, in precisely
   that parenthesization (`120D74..120E48`). The helper writes XYZ and leaves the
   output half's preexisting W untouched.

Tags 1/6 do not mutate candidate `+60/+80` or the payload. Tag 3 is not just
another copied bounds form: its pair traversal can rewrite candidate `+60` and
`+80`, and it has additional helper dependencies. Do not claim it is implemented
by reusing the detector's separately prepared query state. Other tags and the
out-of-range tag path remain separate source-mapped work.

Matrix/local W words do not enter this result's XYZ expressions. Preserving them
as raw cache data is distinct from claiming arithmetic support for NaN/exceptional
XYZ values. The broad original routine is frozen for honest boundaries, not
claimed as wholly native.

## Exact overlap and generated-record order

Use `P` for initial position, `U` for initial up, `C` for candidate cached center,
`H` for temporary world half-extents, `S` for own radius, and `k` for the current
initialized multiplier (default 0.5). These labels describe data flow, not new
physical assumptions. The code does not normalize U or clamp radii.

1. `21952C` overwrites H.y with positive zero, after the helper computed it.
   Scalar squares are combined as `(Hx*Hx + Hy*Hy) + Hz*Hz`, then scalar SQRT.
   `R = k * sqrt(...)` is the candidate effective radius. It is not cached
   candidate `+80`.
2. `D = P - C` with W = 1. Call `106E70(D,U)`: multiply XYZ pairwise, add X+Y,
   then add `(0+1)*Z` with the explicit VU accumulator sequence. Call this `q`.
3. `E = D - q*U`, retaining per-component multiplication and subtraction. Compute
   scalar `distance = sqrt((Ex*Ex + Ey*Ey) + Ez*Ez)`.
4. `219648/219650` accepts only `distance < S+R`. Equality is a miss. There is
   no separate zero-distance, positive-radius or normalized-up gate in source.
   Native finite-domain validation is a safety contract, not an original check.
5. For an accepted overlap, recompute the squared length using the VU dot
   sequence and use **VRSQRT(1, lengthSquared)**. Multiply E by Q for N.xyz and
   copy E.w through the `+0` operation for N.w = 1. This is not division by the
   previously calculated scalar distance.
6. Construct `V = P - S*N`, W = 1. `28BD50(scene190,P)` produces record field
   `00`; `28BD50(scene190,V)` produces field `10`. The original constructs the
   transposed cached basis with explicit zero additions, computes the translated
   inverse term as `(-1) * transpose(M) * M3`, then separately computes
   `transpose(M) * point + inverseTranslation`. Replacing that order with
   `transpose(M)*(point-M3)` changes rounding. Both outputs have W = 1, even for
   non-orthogonal input bases; there is no general matrix inversion/determinant.
7. Field `20` is N. Construct field `30 = C + R*N`, W = 1. This is a world
   point, not another inverse-transformed point.
8. Load main body and captured other body. Invoke main `virtual98(other)` at
   `219788`, then captured other `virtual98(main)` at `21979C`. The second call
   reloads the main-body link; the append subsequently reloads candidate `+A0`.
   Unknown callbacks cannot be assumed not to change those links. For the
   explicitly supported `165430`, only its own list/count mutate.
9. Invoke `2963E0` with record fields `00/10/20/30`, other body, scene override
   zero and material zero. Both callbacks already happened even if record
   storage is saturated. The append copies full vec4 bit patterns, writes
   record `40=0`, `44=other`, `4C=0`, increments spring `84`, and writes spring
   `D4=0` only when signed count is below signed capacity. Existing record
   `48`, `50..5F` and every other spring state field remain unchanged. It does
   not reset the record list, set spring contact flags or execute response.
10. Increment index and reread vehicle candidate count. Skipped and accepted
    candidates retain their input order. No sorting or record deduplication is
    present.

Known `165430` first checks signed count `< 10`. Saturation returns without
reading list entries. Otherwise it scans from index zero until it finds the
other body or reaches count; duplicate discovery does not read later tail
entries. On no duplicate it writes the other identity and increments count.
Its original negative-count behavior is unsafe address arithmetic, not a valid
native contract. Native nonnegative-count validation must remain explicit.
Self-body insertion is not forbidden by the original helper. Neither callback
nor handler assigns a semantic success Boolean to v0.

## Return, boundary and consumption requirements

A natural handler completion is the original `2197FC` JR plus `219800` stack
restore, including empty, all-mask-skip, all-miss, callback-saturated and
record-saturated paths. A stopped helper/virtual boundary is not that return.
Any native result should distinguish natural completion from a reached boundary
and expose no fabricated original return integer. Caller detector hit/miss and
spring append count are also separate quantities.

For a prefix-reporting native API, preserve the last source-reached effects:
prologue scene refresh before a bounds boundary; completed earlier candidates;
inverse-scene refresh before a callback boundary; first callback before an
unsupported second callback; and both callbacks before append saturation.
A transactional failure policy may instead leave public outputs untouched, but
must identify it as native safety policy, not claim the original rolled back.
Structural span/identity validation is separate from lazy semantic consumption.
Rejected candidates must not require valid bounds/body/spring numeric values.
Misses must not require inverse scene/body callback data. Unused raw vector W
lanes and copied caches must not be transformed into invented arithmetic inputs.

The independent fixture worker owns execution/oracle proof. This source-map
work itself executes no game instructions and establishes no numerical test
pass. The current Python reference uses individually rounded ordinary IEEE
binary32 and scalar `sqrt(abs(x))`; its VRSQRT uses rounded sqrt followed by
rounded reciprocal, with signed maximum finite Q for zero. Older native helpers
using the historical `ee_sqrt14` exponent-zero treatment must not be silently
substituted for this chosen domain. Physical EE/VU exceptional modes, denormal
policies, flags and all host rounding modes require separate certification.

## Hard boundaries to a full vehicle substep

Before calling a composed vehicle step complete, close first-substep `122350`
world enumeration and actual detector/list ownership; the optional detector and
second-body branches; dirty-parent cache traversal; all reached bounds/provider
and unknown callback routes; the `1B1800` spatial/prequery phase; the remaining
caller helpers and late scene notifications. Then preserve detector -> handler
-> `1B1800` -> engine -> gravity -> conditional engine application -> aero ->
spring response -> `1B1A30` -> optional coupling -> body integration -> list clear
-> late feedback. The older 48Q and 48L components are valid bounded pieces;
this source map does not erase the intervening stages or imply live racing,
world registration, rendering, original-clock proof, Xbox FPS or whole-game RAM
acceptance.
