# Main/Rider/Course transition callback reconstruction

## Actionable result

The source completion predicates are closed. They are observations of the native page's real visual state, not independent timers:

- Main (screen 4/page 1): entry completes when aperture state is 0; exit completes when it is 2.
- Rider (screen 5/pages 1 and 2): the same aperture condition AND panel Y offset is exactly 0.0f on entry or 140.0f on exit.
- Course (screen 6/page 1): entry completes when aperture state is 0; exit completes when it is 2.
- Other phases return false. Other routes are outside this reconstruction.

The auxiliary child at page+0x134 is `UIWidgetEventHandler`. Its +0x1C/+0x24 calls recursively disable/enable its entity subtree; they are not animation completion callbacks. Page+0x268 is the separate input-admission flag.

The existing `OriginalUiTransitionOwner50KG` can retain its notification and poll interface. What is still needed is a native page controller that owns these actual visual states, source hooks, widget populations, and correct update/render ordering. A timer-only adapter or a caller-supplied completed flag would not provide that owner.

## Scope and source provenance

Static source reconstruction only. No active repository edits, builds, tests, syntax checks, importers, RES decoding, gameplay, guest execution, or emulation were performed. All new files are inside this evidence directory. The native owner was read from `/workspace/shared/rr-recovery-git/src/game/original_ui_transition_owner50kg.{h,cpp}`.

ELF: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

VA 0x100000 maps to file offset 0x80. `exact-source-anchors.json` records source size/hash, 28 bounded instruction ranges with byte hashes, vtable words, RTTI pointers, and literal configuration strings. `evidence/*.txt` retains each raw instruction word plus conservative textual disassembly. The decoder only reads bytes; unrecognized instructions remain UNKNOWN. COP1 c.olt/c.ole names were added for the source comparisons. In its generic COP1 text, compare instructions show a redundant destination field; raw words and actual source operands remain authoritative.

## 1. Exact virtual dispatch

Vtable identities and relevant entries:

| Class | Vtable | +0x38 update | +0x48 render | +0x7C begin | +0x80 poll |
|---|---|---|---|---|---|
| PodUIPageMain | 0x3D11E0 | 0x1E6290 | 0x1E6410 | 0x1D6280 | 0x1D6340 |
| PodUIPageRiderSelect | 0x3D1110 | 0x1EB4F0 | 0x1ECE20 | 0x1ED1C0 | 0x1ECEA0 |
| PodUIPageCourseSelect | 0x3D1450 | 0x1DFDE0 | 0x1DE5C0 | 0x1E0460 | 0x1E0530 |
| PodUIPageController | 0x3D16C0 | 0x1D72F0 | 0x1D7550 | 0x1D6280 | 0x1D6340 |

The +0x7C argument is manager+0x170: 1 entry, 2 exit. The original manager ignores the return from +0x7C, so the current native void notification is appropriate. In particular Rider's exit begin returning 0 does not mean its transition was rejected.

### Main/shared begin and poll

At 0x1D6280..1D6338, phase1 calls 0x1D74E0; phase2 calls 0x1D7480. Other values enter the original bad-transition assertion path.

- Entry helper: if page+0x4B4 is neither 0 nor 1, write 1 and call page virtual+0xAC. It does not complete entry.
- Exit helper: if page+0x4B4 is neither 2 nor 3, write 3, clear page+0x268, and call page virtual+0xB4.
- Shared poll 0x1D6340..1D63AC reads manager+0x170 and returns page+0x4B4==0 for entry, ==2 for exit, otherwise 0. It calls the simple predicates 0x1D7540 and 0x1D7530.

State meanings derived from their writers: 0 open, 1 opening, 2 closed, 3 closing. These are page aperture states, distinct from manager lifecycle states and phases.

## 2. The shared aperture is an actual rendered pair of overlays

`[PageInfo]` parsing at 0x1D64CC..1D650C reads `OpenSize` into page+0x274 (integer default 224) and `Time` into page+0x4B0 (float default 1.0). The literal string pointers are 0x3AEC38/48/58. `Start`, default `Closed`, is read next; `Closed` initializes state2 and both edges at 224, otherwise state0 and edges at OpenSize and 448-OpenSize (0x1D7430).

These parser defaults do not establish the actual Main/Rider/Course data values. Actual PageFile content remains required; no new resource members were decoded here.

Two primitive objects are constructed at page+0x424 and +0x428, each with four vertices and the literal `NoTexture` (0x1D66C0..1D67E0). At 0x1D67E4..1D6808 the source calculates:

    speed4B8 = float(224 - OpenSize274) / Time4B0

The exact motion owner is 0x1D7550, called through render +0x48:

- Closing state3 checks first whether bottom28C <=224 AND top284 >=224. If already at/past both thresholds, clamp both to224, set state2, invoke virtual+0xBC. When manager lifecycle is3, it additionally performs 0x2E0170 on page+0x440 before clamping.
- Otherwise compute the source single-precision product speed4B8 * savedDelta298 and convert with source CVT.W.S; subtract the integer step from bottom28C and add it to top284. If both thresholds were crossed, call virtual+0xB8, but leave state3 for this invocation. State2 is written on a subsequent render invocation.
- Opening state1 first checks bottom28C >=448-OpenSize274 AND top284 <=OpenSize274. Once satisfied, clamp to those endpoints, set state0, set input flag268=1, then invoke virtual+0xB0.
- Otherwise apply the same integer step in the opposite directions. Entry does not immediately recheck its thresholds after the step.

0x1D76D8..1D799C writes the two rectangles' actual vertices, spanning X=0..640 and Y=0..top284 / bottom28C..448, marks their data dirty, and submits both via 0x2E21D0. It then performs further page drawing and child +0x48 traversal.

Preserve source float operation order, the established native equivalent of CVT.W.S, integer geometry, delayed endpoint observation, and callback ordering. Do not substitute a float lerp, minimum-one-pixel step, `elapsed >= Time`, or a forced terminal state. The source predicate is CPU visual-state completion; it is not a GPU fence.

### Update/render ordering matters

Main update stores dt in page+0x298 at 0x1E63F8; Rider at 0x1EB620; Course at 0x1E02BC. Each then delegates child update through 0x2A6BB0 -> 0x141ED0. Main render calls shared aperture rendering at 0x1E6688, Rider at 0x1ECE80, Course at 0x1DEB6C.

Manager 0x2A3C80 processes its lifecycle switch/poll before calling child update at 0x2A3FDC. The render traversal is separate (entity mask8, +0x48, 0x141D70). Native polling must observe state produced by the corresponding real page update/render phases. Repeated manager ticks without the page's visual phase cannot legitimately synthesize progress.

## 3. Rider is a two-stage visual transition

Rider constructor initializes panel offset page+0x4DC to float140.0 at 0x1E7C40. This is real visual motion: e.g. 0x1EBD00..1EBD3C adds it to text Y positions, and numerous later locations use it for the other Rider text/widget geometry.

Rider begin +0x7C at 0x1ED1C0:

- Entry delegates to shared begin, opening the aperture first.
- Exit does nothing and returns0. Its entity update owns the exit sequence.

Rider poll 0x1ECEA0 first checks the shared aperture endpoint for the current phase, then calls 0x1ED210 with that phase. The second predicate is exact float equality: offset4DC==0.0 for entry, ==140.0 for exit. There is no epsilon or timer predicate.

Rider update at 0x1EB598..1EB5C4 invokes 0x1ECFE0 only while manager lifecycle is3 or4, passing manager phase and actual dt:

- Entry: clears page+0x4E0. Only after aperture state0, subtract `(140.0 / Time4B0) * dt` from offset4DC, clamping negative results to0. The aperture opens before the panel slides in.
- Exit: sets page+0x4E0=1. While offset<140.0, add the same source float step, then return. On a subsequent update with offset>=140.0, clamp it to140.0, then inspect the selected group1 control using index page+0x4C8.
- If that control's +0x34 is4, read its widget pointer at control+0x38 -> +0x0C and call 0x1D2710. This function returns1 when widget+0x1D8 is0, otherwise returns widget+0x1E0. Other control types yield ready immediately. This ties the transition to the selected preview's native state; replacing it with asset-presence alone is unjustified.
- If preview readiness is false AND page+0x4E4 is0, leave the aperture unchanged. Otherwise, when aperture state is neither2 nor3, write3, clear input268, and invoke Rider's +0xB4 at 0x1EA360. The aperture closes only after panel motion and preview readiness/explicit page bypass.

Do not set page+0x4E4 simply to make the route advance. Its owner is the original Rider interaction logic. Unported preview readiness means unavailable admission, not unconditional completion.

## 4. Course deliberately delays entry initiation

Course begin 0x1E0460 starts closing state3 and clears input268 through +0xB4 on exit, using the same idempotence guard as the shared helper. Entry does no work.

Course's render path owns when entry may start. At 0x1DEB20..1DEB64, after its earlier popup/native-page conditions, closed aperture state2 with manager phase1 changes to state1 and calls +0xAC. It then invokes shared aperture rendering. Moving this to generic begin would bypass the source page-readiness path.

Course poll at 0x1E0530 has an apparently intended extra readiness branch, but its literal control flow tests the same loaded manager+0x170 value against2 and then5 (0x1E0544/0x1E0550). Both cannot hold. The block that calls 0x2915D0 at 0x1E05C0 is therefore unreachable through this entry. Do not invent an audio gate from that dead block. The reachable predicate is solely state0 for entry/state2 for exit.

The full popup/course controller remains outside this bounded port. This report closes the completion contract, not admission of an incomplete Course page.

## 5. Auxiliary child identity and exact role

Page loader allocates 0x40 bytes, calls constructor0x2A6FD0 and init0x2A7100, stores page+0x134, then attaches it to the page's entity tree at 0x2A5FCC..0x2A5FDC. Constructor installs vtable0x3D3D00. Vtable -> typeinfo0x3BF348 -> name0x3BF2D0 is `UIWidgetEventHandler`.

- Child vtable+0x1C =0x1415A0: clear byte+0x29 bits0 and1 on self, then recursively invoke +0x1C on children, skipping children with bit3 set.
- Child vtable+0x24 =0x141630: set bits0 and1, recursively invoke +0x24 with the same bit3 exclusion.
- Its +0x64 =0x2A7290 translates controller input into an original UI event, checks current page+0x268, calls page virtual+0x78, then dispatches accepted events to registered widgets' +0x8C. Disabled child bit0 prevents normal input traversal at0x142384..1A0.

It is safe to replace the neutral auxiliary callback names with explicit event-handler disable/enable in a native adapter once this concrete child owner exists. Do not disable the page itself as a substitute: the page must continue receiving visual updates/rendering while its input handler is disabled.

## 6. Smallest native integration contract

1. Route admission binds a real source-class page controller, its loaded configuration, overlay primitives, entry/exit hooks, event-handler owner, and (for Rider) the selected preview/control state. Missing any required dependency keeps the route unavailable.
2. Map beginTransition7C to that route's exact begin implementation. It must not complete the transition directly. Preserve Rider exit and Course entry no-op behavior.
3. Advance real visual state in the native equivalents of entity-update and render phases. Shared aperture updates the very geometry rendered; Rider panel motion updates the actual widget/text positions. Keep one owner for each state.
4. pollCompletion remains read-only. For admitted Main/Course: `phase==entry ? aperture==open : phase==exit ? aperture==closed : false`. For admitted Rider add `panelOffset==0.0f/140.0f`. Return unavailable when the bound native state/owner is lost; never fabricate a terminal observation.
5. Map pageAuxiliary1C/24 to recursive disable/enable of the concrete event handler. Retain the existing manager notification timing: screen exit disables after completion; Course exit disables immediately; entry disables at its start and enables only after completion.
6. Preserve page hooks and source state writes before allowing lifecycle queue progression. The queue consumer remains separate. No callback starts gameplay, destroys the manager inline, or executes a guest address.

The predicate does not scan every widget animation or wait for GPU completion. It also cannot replace the hooks: Main's +0xB4 at0x1E6960 disables source widgets/groups and starts other native effects; Rider/Course hooks differ. Route readiness requires those owners even though their completion is not independently polled.

## Remaining integration prerequisites

Actual decoded PageFile values, full native page/widget/preview/popup owners, source float/integer conversion equivalence, original render submission/pass ownership, and end-to-end runtime validation are not established here. The current transition owner needs no new elapsed-time field. This evidence is sufficient to implement its adapter only when those concrete dependencies are available. No runtime support or successful test claim is made.
