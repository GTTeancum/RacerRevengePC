# Staged Main/shared native aperture owner

## Result and status

New source is staged in `files/src/game/original_ui_aperture_owner50kg.{h,cpp}`.
It owns the actual integer aperture edges, saved update delta, source aperture
state, page input-enable flag and both rendered rectangle geometries. It emits
the real Main/shared hook sequence at the matching state-write boundaries and
supplies a read-only Main source-phase poll.

This is a bounded, source-reviewed candidate, not a completed Main controller
or active frontend. No active repository file was edited. No build, test,
syntax check, importer, archive/texture/asset decode, gameplay, guest CPU/VU
execution or GPU execution was run. Root owns integration and validation.

CVT.W.S is now a concrete signed-truncation helper, following the final primary
Sony-manual and opcode evidence in the sibling ui-aperture-numeric-research
README and original-bytes.txt. It admits finite input in
[-2147483648,2147483648), preserving signed and subpixel-to-zero results, and
rejects other values before any host cast. Source saturation is outside this
declared bounded adapter. The existing UI text nearest-even helper is not used.

Upstream float arithmetic uses an explicit bounded native host contract:
individually materialized binary32 operations under FE_TONEAREST with the finite,
normal-or-zero and integer-range checks described below. No separate arithmetic
certification callback is required. This is a native CPU reimplementation;
neither the signed-truncation proof nor the host operation order implies
universal bit-identical EE float arithmetic. No runtime activation is supplied.

## What is concrete

- `bindMain` pins an already-authenticated `OriginalUiArchive50KG` with mainMenu
  identity. It reads UIMain/ui.dat, follows the actual one-based Page 1 PageFile,
  then reads that member's PageInfo. It cannot bind a caller-fabricated typed
  descriptor or a default-only configuration.
- Source OpenSize default 224, Time default 1.0 and Start default Closed apply
  only after an authenticated PageInfo is read. Three authored flags separately
  preserve whether each value was actually present. An authored 224/1.0/Closed
  remains distinguishable from an omitted field. The actual Main values were
  not decoded or claimed in this task.
- Speed is an individually materialized host binary32 conversion/divide of
  `(224 - OpenSize) / Time`. Each moving render multiplies speed by saved dt
  once before the concrete signed-truncation CVT.W.S operation. The upstream
  arithmetic has the explicit native contract above. There is no elapsed
  accumulator, fractional-pixel carry, lerp, minimum-one-pixel step, synthetic
  completed flag or elapsed-time fallback.
- `saveMainUpdateDelta38` is solely the Main update-tail savedDelta298 write.
  It changes no edge/state. The caller must place it at 0x1E63F8 before the
  existing child update; it is not a substitute for the rest of Main update.
- `render48` advances only at the real 0x1E6688 -> 0x1D7550 render call. Every
  real render uses the last saved sample, including reuse without another
  update, matching the source saved-field semantics. Repeated manager ticks
  and polls cannot advance it.
- Closing step writes bottom minus step, then top plus step. A crossing emits
  B8 but stays closing and retains overshoot geometry for that draw. The next
  real render observes the threshold; in lifecycle3 it first emits the +440
  effect while the old closing/overshoot state is visible, then clamps both
  edges to224, writes closed and emits BC.
- Opening step writes bottom plus step, then top minus step, and does not
  post-check. A later render observes the threshold, clamps to OpenSize and
  448-OpenSize, writes open, enables input268, then emits B0.
- Begin entry writes opening before AC and does not change input268. Begin exit
  writes closing and clears input268 before B4. Repeated same-target begins
  are idempotent. Reversing direction preserves the current edges.
- Rectangle0 at source +424 spans `(0,0)` to `(640,top284)`. Rectangle1 at +428
  spans `(0,bottom28C)` to `(640,448)`. Both retain exact source vertex order
  TL,BL,TR,BR, including overshoot and zero-area geometry. Their order for the
  native renderer's TL,TR,BR,BL interface must be permuted 0,2,3,1 explicitly.
- Both primitives are marked dirty then submitted, top first and bottom second.
  Source NoTexture and vertex alpha63 are retained metadata. These are actual
  CPU geometry records, not a promise of a complete material/depth/pass packet.
- Poll returns complete only for entry+open or exit+closed, pending for other
  phases/states, and unavailable for unbound, wrong route, reentry or a lost
  dependency. It owns no independent completion input.

## Newly closed constructor detail

The generic page constructor at 0x2A5CCC writes input268=1, but that is not Main's
final constructor state. Raw static instruction review closes the full chain:

1. Main constructor 0x1E540C calls shared constructor 0x1D5EA0.
2. Shared constructor 0x1D5EAC calls generic constructor 0x2A5C70.
3. Shared constructor 0x1D5F48 clears savedDelta298.
4. Shared constructor 0x1D5F70 clears input268, overriding generic initialization.
5. Main constructor's remainder does not overwrite input268.
6. PageInfo's non-Closed branch 0x1D654C explicitly sets input268=1; its Closed
   branch leaves the shared constructor's zero in place.

Therefore Closed-start input=false is source-backed, rather than inferred from
its aperture state. The supplementary raw words are retained in
`evidence/constructor-and-path-raw-words.txt`. No instruction was executed.

## Native integration boundaries

The callback `available` must certify real owners for Main hook side effects,
the +440 object, both source primitives and their render-pass submission. A
trace collector, empty callback or asset-presence-only probe cannot certify a
real page. The host arithmetic and concrete CVT.W.S operation are implemented
inside this module, not caller-replaceable and not gated on universal EE parity.
Missing callbacks or false admission leave bind unbound; losing a required owner
returns unavailable. Source-address fields are provenance, never function
pointers or executable guest addresses.

Hooks include Main AC=0x1E66C0, B0=0x1E68F0, B4=0x1E6960, B8=0x1E6A50,
BC=0x1D7A00, and the 0x2E0170 operation on page+440. Main B8 and BC are currently
source no-ops but retain their event boundary. Main AC/B0/B4 perform real
widget/group/effect work and cannot be replaced by no-ops when admitting Main.
Each event carries the owner's snapshot after the source writes that precede
that call. The +440 event deliberately carries the pre-clamp state.

The callback `submitOverlay` is the native equivalent of the source primitive
submission point. It must be infallible after read-only admission. CPU visual
state completion is not a GPU fence; rendering failure must not become a
caller-supplied completion result. Further page drawing 0x1D7A10 and page child
render 0x141D70 remain at the real caller's subsequent source positions.

The manager's auxiliary callbacks still need the separate concrete
UIWidgetEventHandler owner. Its recursive disable/enable is not input268, not
an aperture completion check, and not permission to disable the page itself.
The page must remain visually updateable/renderable during transitions.

This owner supplies no general `admitDestination` implementation. A full Main
page must additionally establish the correct manager/entity tree, native
widget populations, input handler, page lifecycle callbacks, assets and all
render passes. Rider panel/selected-preview readiness, Course entry admission,
other source pages and application queue consumption are outside this stage.
`pollCompletion` intentionally accepts only screen4/page1.

## Explicit native restrictions

- Binary32 host operations require FE_TONEAREST and non-fast-math/noncontracting
  compilation. This describes the bounded host arithmetic path, not proof of
  universal EE float parity. Callbacks must not alter the floating-point
  environment. The signed-truncation helper itself is host-mode independent.
- The existing page parser's admitted OpenSize range 0..224 and positive Time
  are retained. Nonfinite/subnormal values, overflow and underflow-to-zero are
  rejected rather than emulated. Delta must be finite, nonnegative and zero or
  normal. No clamp rewrites these supplied values.
- Source integer ADDU/SUBU overflow outside signed32 is outside this candidate's
  admitted arithmetic domain. It is rejected transactionally, not executed as
  signed C++ overflow or forced to an endpoint.
- A real update sample is required before any render, although the constructor
  proves savedDelta's initial zero. This is an explicit native ordering gate,
  not a claimed extra original branch. No constructor clock sample is invented.
- Failure before mutation leaves state, geometry and hooks untouched. Callbacks
  are synchronous, nonthrowing, nonmutating and nonreentrant. Copy/move are
  disabled so no second object can own the same live aperture state.
- Authenticated archive ownership is pinned by shared_ptr; as in the existing
  static-text owner, a no-op alias does not satisfy the ownership contract.
- Exact archive-member matching is used for PageFile. No basename, guessed
  directory, resource-token-derived filename or cross-archive fallback exists.
  Source formats `%s%s` with the 0x36BB90 prefix operand; its ELF bytes are empty.
  A runtime path policy that changes that prefix must be resolved by the source
  asset owner, not guessed by this aperture owner.

## Source reviewed

Primary baseline files:

- `src/game/original_ui_transition_owner50kg.{h,cpp}`
- `src/game/original_ui_static_text_page_owner50kg.{h,cpp}`
- `src/assets/original_ui_pages50kg.{h,cpp}`
- `src/assets/original_ui_archive50kg.h` and its authenticated catalog
- `src/game/original_ui_text_position50kg.{h,cpp}`
- `src/game/original_ui_texture_animation50kg.h`
- `platform/linux/native/original_ui_draw50kg.h`

Primary source evidence is the sibling ui-transition-callback-reconstruction
README, exact-source-anchors.json, and evidence files page-config,
page-start-poll, overlay-construction-speed, aperture-state-motion-render,
main-entity-update, main-render-tail, main-aperture-hooks and vtables-rtti-strings.
Supplemental existing ui-static-membership evidence supplied generic constructor,
PageFile indirection, page-create and primitive-render ordering.
The raw constructor supplement reads the same SLUS_202.68 source whose recorded
SHA-256 is c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

The final numeric proof is in sibling `ui-aperture-numeric-research/README.md`
and `original-bytes.txt`: Sony EE Core Instruction Set Manual v6.0 p356 specifies
signed CVT.W.S truncation, and EE Core User's Manual v6.0 §8.7 p164 establishes
FCR31's hardwired rounding bits. Both actual opcodes are 0x46000024. Those sources
settle conversion despite contradictory existing helper comments; §8.8's
arithmetic caveat remains separate. No neighboring text helper was changed.

## Root's later validation targets (not run)

- No source archive, missing owners, unsupported host arithmetic and wrong
  route remain unavailable; no hook or render callback is fabricated.
- Actual authored/default provenance survives bind, including quoted empty
  Start and omitted versus authored default-valued fields.
- Poll and saved-delta update never move an edge; stable render submits both
  rectangles; no-update render rejects with no effect.
- Zero-step stalls; time does not force completion. Signed/subpixel/noninteger
  conversion cases discriminate the verified converter from nearest-even.
  NaN, infinities and the strict +2^31 boundary reject without altering output;
  -2^31 is inside the helper's signed domain. These are targets, not test results.
- Exact endpoint and overshoot both retain one-render delayed observation.
- Close crossing emits B8 before the later +440/BC sequence; +440 is lifecycle3
  only and sees pre-clamp state. Entry B0 observes open and input enabled.
- Same-direction begins do not repeat hooks; reversal retains edges and the
  source input sequence. Lost dependencies and invalid arithmetic do not move.
- Source submission order, dirty writes, all eight vertex coordinates, zero-area
  cases and renderer vertex-order conversion are preserved.

No validation outcome is claimed. There is no build-system, manager callback,
frontend admission or renderer call-site change in this staged deliverable.
