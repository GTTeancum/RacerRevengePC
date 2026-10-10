# Original menu routes: source evidence and native wiring contract

Static-source recovery, 2026-10-10. No active-repository edits, builds, tests, syntax checks, imports, RES decoding, guest execution, VU emulation, or gameplay execution were performed. The staged header is uncompiled, inert native metadata. The active 8bb2bfe candidate was not changed.

Source: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

ELF loaded section maps VA 0x100000 to file offset 0x80. `read_mips_words.py` prints static instruction words and a conservative textual interpretation; it executes no guest instructions. Unknown opcodes remain marked UNKNOWN. Evidence below rests on explicit integer loads/stores, branches, call targets, tables, and string/RTTI pointers.

## Closed factory identities

Global factory 0x1B7DF0 checks unsigned ID <15, then indexes jump table 0x3AC790. IDs, literal loader resource tokens, cases, and constructor/loader addresses are preserved in `original_ui_route_registry50kg.h`.

- 0 UILang; 1 UIArt1; 2 UIArt2; 3 UIArt3
- 4 UIMain; 5 UIRider; 6 UICourse; 7 UIWin
- 8 has no factory, entering undefined-choice branch 0x1B8278
- 9 UITourn; 10 UIUnlock; 11 UIHOF; 12 UIOption; 13 UIGarage; 14 UITest

The resource token is a literal passed to the original loader, not a guessed member path. The exact page filename comes from ui.dat `[Page N].PageFile`: 0x1CEC6C..1CECD4 forms `Page N`, reads `PageFile`, prefixes it with the original base, and invokes manager vtable+0x90 with a1=N. The page factory index really is the one-based page number.

Relevant closed page factories:

- Screen 4 factory 0x1CFB80: page 1 Main (ctor 0x1E5400); 2 Legal (0x1E49C0); 3 Opening (0x1D7C80); 4 Demo (0x1D8500); 5 GalleryMenu (0x20CA40)
- Screen 5 factory 0x1CFF00: page 1 RiderSelect constructor 0x1E7AC0 with player index 0; page 2 the same constructor with player index 1
- Screen 6 factory 0x1D0170: page 1 CourseSelect constructor 0x1D8D70
- Screen 9 factory 0x1D03E0: page 1 Standings constructor 0x1ED7E0
- Screen 12 factory 0x1D0E30 / table 0x3ADE00: page 1 OptionsMenu (0x1F8DD0); 2 ControllerConfig index 0 (0x1F2F60); 3 ControllerConfig index 1; 4 GameOptions (0x1F46A0); 5 MemoryCard (0x1F6880); 6 Credits (0x1F7500)

The staged header additionally records the exact original class names through vtable → typeinfo → name-pointer indirection. It is a closed identity registry, not an assertion that native implementations or assets for these pages exist.

## Closed main and continuation routes

Numbers here are original screen/page IDs and original input-event IDs. Host keys remain a separate binding concern.

### Main, screen 4/page 1, input handler 0x1E5E00

Existing native dispatch matches these route requests:

- Slot 1: profile+0x34 = 0; profile byte+0x1125C human bits = 0x20; screen 5/page 1
- Slot 2: requires controller 2 present; mode+0x34 = 1; human bits = 0x40; screen 5/page 1
- Slot 3: mode+0x34 = 2; human bits = 0x20; screen 5/page 1
- Slot 4: screen 12/page 1
- Slot 5: screen 4/page 5

Main vtable 0x3D11E0 +0xA0 points to 0x1D6130, accepting `(page, screen)`. +0xA4 points to 0x1D6190, accepting a page in the current screen. This proves the route argument 1 is a page, not an abstract animation argument. Both helpers refuse changes while manager+0x170 is 2.

### Rider selection, screen 5, input handler 0x1EA460

The constructor player index is stored as page+0x4C0; the selected original rider value is page+0x4C4. Input event 4 confirms, event 6 backs out. Page 1 accepts controller index 0; page 2 accepts index 1. Popup/blocking state is checked first. Confirmation is rejected when profile word `[0x10FAC + rider*4]` is zero.

Page 1 confirmation at 0x1EA504..1EA640:

- Saves the rider in profile+0x6F0 for mode34=0, +0x6F4 for mode34=1, otherwise +0x6FC
- Commits rider1 into byte+0x1125D bits 3..7 and profile word+0x100C0
- For human-count bits ==1: mode34==2 requests screen 9/page 1 (0x1EA5E4); otherwise screen 6/page 1 (0x1EA604)
- For human-count bits ==2: same screen/page 2 (0x1EA630)
- Back requests screen 4/page 1 (0x1EA810), retaining the selection in the mode-specific remembered field

Page 2 confirmation at 0x1EAB30..1EAD74:

- Commits rider2 into byte+0x1125E bits 0..4
- Preserves the original comparison selecting profile+0x100C0 from the two riders' data; do not replace it with an arbitrary default rider
- Remembers selection in profile+0x6F8 and requests screen 6/page 1
- Back returns to same screen/page 1 (0x1EADC0)

These are route decisions, not a full RiderSelect port. Visual swaps, sounds, transient page flags, unlock ownership, and the rider-data comparison still need native owners.

### Gallery menu, screen 4/page 5, input handler 0x20CEE0

Controller 0, event 4: selected slots 1/2/3 request screen 1/2/3 respectively, page 1, at 0x20CF6C/0x20CF88/0x20CFA4. Event 6 requests same screen/page 1 at 0x20D014. Availability comes from profile+0x11018 bits 5..6 and the original enable callbacks (0x20D1A0 onwards), not merely from the existence of gallery assets.

### Options menu, screen 12/page 1, input handler 0x1F9640

Dispatch table 0x3B00F0 maps selected slots:

- 1 → same screen/page 2
- 2 → same screen/page 3 if controller 2 is present; otherwise the original controller-required popup
- 3 → same screen/page 4
- 4 → sets profile mode34=5, then screen 11/page 1 (UIHOF)
- 5 → same screen/page 5
- 6 → same screen/page 6
- Event 6 back → screen 4/page 1

Selection is remembered at profile+0x6E4. This does not authorize inventing settings save/apply behavior; that belongs to each destination's unported owner.

## Course commit and race-launch ownership

### Two different mode fields

Application global pointer is VA 0x382B80. Its +0x2DC field owns the profile object.

- Main's original 0/1/2 mode is profile word+0x34, written by 0x17DE50; previous value is retained at +0x38 when it changes.
- Race behavior subtype is profile word+0x11258. This is a different field. Course confirmation writes it at 0x1DED80 / 0x1DEE20 / 0x1DEEB0.
- With mode34=0, subtype11258 comes from course-page+0x14F4, which is the selected one-based slot in group 2 (0x1DF218..1DF224; equivalent forward branch 0x1DF4D0 onwards).
- With mode34=1 or 2, subtype11258 is forced to 1.

Therefore there is no valid direct cast of main slot-1 to a native race subtype or RR_RACE_MODE. Native support must check the independently selected subtype. The existing `LapConfigTa50ID` exposes subtype11258, packed1125C, and selection1125F; the route evidence does not broaden its supported modes/tracks.

### Course confirmation, screen 6/page 1, handler 0x1DEB90

Input table 0x3AF320 maps event 4 to 0x1DEBF4. It first checks course-page+0x42C bit 0. Mode34=1 also requires both controllers. Original course selection is one-based course-page+0x14E4, but the committed track value is the selected control's word+0x44, passed to 0x187300 and written into profile byte+0x1125F low nibble. Do not substitute `selectedSlot-1` without the actual control data.

The confirmation then:

1. Writes opponent count into packed1125C bits 7..10: `(page14F0 - humanCount) & 15` when mode34==1 or subtype14F4==2; otherwise `(8 - humanCount) & 15`.
2. Sets owning manager+0x16C to 8 at 0x1DED58.
3. For mode34=0 writes subtype14F4 to +0x11258 and page14EC low 5 bits to +0x1125C; preserves remembered page14E4/14EC/14F0/14F4 at profile+0x700/704/708/70C.
4. For mode34=1 forces subtype1, writes page14EC low 5 bits, remembers at +0x710/714/718.
5. For mode34=2 forces subtype1 and low 5 bits=3.
6. Clears global UI manager +0x474/+0x478. Exit proceeds through the lifecycle below.

Event 6 backs out via mode-specific remembered settings: mode34=0 → screen 5/page 1; mode34=1 → screen 5/page 2; other mode → screen 9/page 1. Source 0x1DEFB4..1DF0C4. The tournament continuation itself was not reconstructed here.

### Delayed transition lifecycle, not an immediate swap

Manager fields: +0x164 requested screen; +0x168 requested page; +0x16C lifecycle state; +0x170 transition state; +0x150 current page index; +0x14C current page object.

State-table 0x3BEC60 is consumed by 0x2A3C80. Source-proven cases:

- Screen request helper 0x1D6130 saves requested page/screen through 0x1CCF40/0x1CCF70, then state6. State6 → state3, transition2, current-page virtual+0x7C(2). State3 polls current-page virtual+0x80; only when it reports completion does it set state9. State9 queues application flags2 with `(screen,page)` through 0x17DE10.
- Same-screen helper 0x1D6190 saves requested page, preserves screen, then state7. State7 → state4, transition2. Completion changes state4 → state1. State1 selects the requested page through 0x2A4020, performs original entry callbacks, then waits for entry completion before state0.
- Course launch state8 → state5, transition2, current-page virtual+0x7C(2). State5 waits for virtual+0x80 completion; only then state10. State10 queues application flags9 with zero arguments at 0x2A3F88..0x2A3F9C.

0x17DE10 writes application+0x318/31C/320 and attaches its embedded PodLauncher at application+0x2E8 to the entity tree. These are request flags, not a race enum. PodLauncher tick 0x2C6CD0 handles bits in order:

- bit8 calls 0x17DCE0, which detaches/destroys the current global UI manager and clears application+0x2D8
- bit1 calls 0x17D7A0, which configures LoadingPageContainer with kind0 through 0x17C580
- bit2 (ordinary screen change) calls 0x17DB80 with request screen/page; existing global UI invokes vtable+0x78, which is factory 0x1B7DF0

LoadingPageContainer tick 0x17C0A0, after its readiness flag +0x3C, kind0 branch at 0x17C14C creates PodRaceEvent with 0x1A65C0, stores it at application+0x28C, invokes its init 0x1A6AA0, and attaches it to the entity tree. RTTI confirms `PodLauncher`, `LoadingPageContainer`, and `PodRaceEvent` names.

## Native implementation boundary

Closed now: exact screen registry, bounded page identities, route decisions, packed selection field destinations, original lifecycle/queue ownership through PodRaceEvent initialization.

Still unresolved/unimplemented here:

- Exact PageFile values and destination asset populations; no new RES members were decoded. Load original ui.dat/Widgets.dat/PageFile content only through already-authorized native asset paths at a future permitted execution point.
- Native RiderSelect/CourseSelect/Options/Gallery page controllers, visuals, transition callbacks, unlock checks, sounds, and owning widget populations. A registry match alone must not set supportedGameModes/supportsScreen12/supportsPage5 true.
- Native completion semantics for virtual+0x7C/+0x80. Do not skip the exit gate or call gameplay directly from the main selection callback.
- Tournament/garage/standings continuation, per-page settings persistence, full rider comparison, and complete PodRaceEvent asset/load ownership. Only the bounded identities and chain above are closed.
- Mapping admitted original selection values to the existing host's constrained native track/rider/subtype configuration. No automatic fallback to TA, another rider, or another mode is justified.

A safe integration preserves one owner for profile selections and manager lifecycle, commits the source fields in source order, resolves real one-based PageFile/widget data, and admits only fully native destination controllers. Keep unsupported paths explicit until those owners are available. `original_ui_route_registry50kg.h` supplies metadata lookup only and deliberately performs no allocation, asset loading, transition completion, or gameplay launch.
