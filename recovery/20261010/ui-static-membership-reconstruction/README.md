# UI static-count and manager-child ordering reconstruction

## Result and staged candidate

The inspected original UI implementation does not provide an authored static-widget membership list behind `PageInfo.NumberOfStaticWidgets`. Both page creators read that integer into page offset `+0x13C`; the inspected page/manager family never consumes that field to select widget IDs. Control memberships, input-registration memberships, and the update/render entity tree are separate structures.

The actual game-specific manager has an authored `Widget N.OnTop` integer, default zero. For the admitted all-`Overlay_Text` case, ordinary widgets are appended to the manager in ascending widget ID. Nonzero `OnTop` widgets are deferred on a head-linked list and appended after the intervening original setup/page children in descending widget ID. These are entity-child traversal positions, not depth, alpha, blend, or render-pass settings.

`candidate.patch` and `files/` stage six narrow native source/header changes against repository revision `2fab114616469a7a45e83551dc9eb58903a3a4df`:

1. Document `staticWidgetCount` as retained metadata, without inventing a static membership vector.
2. Parse `OnTop` into an `OriginalUiOverlayText50KG::onTop` boolean using the source's integer-default-zero/nonzero branch.
3. Admit that recognized field in the all-text owner and remove its nonzero-static-count rejection.
4. Publish two immutable, one-based ID vectors through `managerWidgetOrder()`: `ordinaryWidgets` ascending and `onTopWidgets` descending. Both together contain each admitted declared widget once. Publication stays atomic and fixed-address.
5. Correct ancestor documentation: those widgets are manager children and siblings of pages, rather than children of their referencing controls or page.

The order vectors are evidence for a future original-manager integration. They do not run callbacks, update widgets, render widgets, introduce a new pass, activate the UI, or fabricate the non-widget children between them. Existing explicit per-widget entry points still require the actual source lifecycle point and established entity-tree gates. Every declared widget still must be admitted `Overlay_Text`; popup/video, unimplemented widget/text/joystick behavior, unknown fields, and other existing restrictions remain rejected.

This is new source-backed native reconstruction, not recovery of historical lost source. The repository was not edited. `preimages/`, `SOURCE-SHA256SUMS`, `STAGED-SHA256SUMS`, `manifest.json`, and `candidate.patch` provide the integration boundary. Root must compare preimages before copying or applying. The candidate has not been compiled or tested.

## Source identity and method

- Original ELF data: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`
- SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`
- ELF first LOAD: file offset `0x80`, VA `0x100000`, file size `0x2D5A00`. Static mapping: `fileOffset = VA - 0x100000 + 0x80`.
- `evidence/` retains address/word cuts and relevant original string/vtable data. The tiny read-only `read_mips_words.py` prints raw little-endian instruction words plus labels for a limited MIPS/R5900 subset. Unsupported instructions remain explicitly unknown. It is not a guest interpreter, emulator, project asset decoder, or importer; it never executes guest instructions. Raw words, branch targets, call targets, string bytes, and pointer-table values are the source evidence.
- No RES archive or DAT member was decoded/read through the project. No project executable, build, test, syntax check, importer, gameplay, GPU submission, or user-computer action ran. Hashes, text inspection, and static byte/disassembly reading only.

## Authored fields, indices, and bounds

### Static count

- `1D6DEC..1D6E00`: string `NumberOfStaticWidgets` at `3AEE80`, output address `page+0x13C` at `1D6DF8`, integer-default-zero read at `1D6DFC` with zero in its delay slot.
- Generic parallel: `2A5FE0..2A5FF4`, string `3BF100`, output `page+0x13C` at `2A5FEC`.
- Constructor zeroes this field at `2A5C90`.
- No static-list allocation, indexed static-ID key, or static-count-controlled loop exists in either inspected page creation routine.
- `evidence/static-count-xrefs.txt` inventories direct immediate `+0x13C` references over the inspected Pod UI family `[1CC8D0,200000)` and generic UI family `[2A3000,2ADC00)`. Besides constructor/read destinations, remaining matches are stack fields and one widget-table element at `1F42E8`, whose base is loaded from manager `+0x154`, not a page. This bounded inventory is not a claim about arbitrary alias arithmetic throughout all executable code.
- Keep the current native admission `0..1024`; do not require the count to equal control 0's size, the first IDs, the `OnTop` count, or the total unique referenced widgets. No such relationship was established by source.

### Control and group references

- `1D6E04..1D6E2C` allocates `NumberOfControls+1` pointers. `1D6EBC..1D6EFC` creates controls `0..NumberOfControls`, inclusive.
- `1D7180..1D72E0` selects `Control %d`. The existence result is saved at control `+0x30`; missing sections stop there (`1D71EC..1D71F4`).
- `1D71FC..1D722C` reads `NumberOfWidgets`, default zero, and allocates that many pointers only when positive.
- `1D7240..1D7264` reads `WidgetID1..WidgetIDN`, default zero. `1D7268..1D7288` resolves each as `manager.widgets[id-1]`; the `lw -4(...)` at `1D7284` establishes the one-based convention. Members stay in authored order and are not deduplicated.
- `1D6E30..1D6E58` and `1D7130..1D7138` allocate/create groups `0..NumberOfGroups`, inclusive.
- Group `ControlID1..N` values are zero-based control IDs; `-1` is a null hole (`1D6FC0..1D7014`). Association IDs are also control IDs, default zero (`1D70B4..1D70EC`). Membership suffixes are one-based. `KeepEnabled` defaults one (`1D7110..1D7124`).
- Original direct array resolutions do not add safe range checks. Current native bounds remain deliberate safety admission: widget IDs `1..widgetCount`, controls `0..255`, groups `0..63`, and at most 4096 aggregate control/group/association memberships. These are not claimed original constants.

### OnTop and manager widget IDs

- Actual Pod manager override `1CD7A0` loads `%sWidgets.Dat` and the same `General.NumberOfWidgets`/`Widget %d` one-based schema (`1CD82C..1CD878`).
- `1CD880..1CD894`: read `OnTop`, string VA `3AD820`, integer default zero; call PC `1CD890`.
- `1CD898..1CD8A4`: pass that value into `1CD160`. For `Overlay_Text`, both `OnTop` branches create the same text-widget class and invoke the same `2A9000` reader (`1CD37C..1CD408`). No text material, alpha, or depth override is introduced by that branch.
- After creation, `WantsJoystickInput1/2` are read into widget `+0x50/+0x54` (`1CD8C0..1CD8FC`), and recursive hide is called (`1CD900..1CD90C`).
- `1CD910..1CD918` stores the widget in manager array slot `ID-1`; the load loop increments IDs at `1CDA18..1CDA28`.
- For `OnTop == 0`, `1CD9E8..1CD9FC` appends that widget to the manager immediately using `141960`.
- For any nonzero value, including negative integers, `1CD91C..1CD9E4` takes the deferred branch. First node stores widget at node `+0x34`, previous at `+0x30 = 0`; later nodes store old head at `+0x30`, widget at `+0x34`, then replace manager head `+0x190`. The original node cleanup mechanics are not copied into native code.
- `1CDA80..1CDAA0` traverses head through `+0x30` and appends `node+0x34` to the manager. Thus deferred `OnTop` IDs are reversed relative to loading. The native all-text subset guarantees unique newly created widget objects, not `Global` aliases or shared pre-parented objects.
- Parser integers remain strict signed32. The staged boolean stores only the source zero/nonzero predicate. Existing widget-count bound 1024 bounds both order vectors; empty and all-one-partition cases have no synthetic member.

## Separate ownership and traversal structures

### Control callbacks and input registration

`1D7294..1D72A4` calls `2A71A0` for each non-null control member. This does not attach a draw/update child. `2A7100` allocates eight lists under a page-owned container (`+0x38`); `2A71A0` walks eight widget words starting at `+0x50` and appends the widget to each enabled bucket. The linked-list insertion preserves repeated input registrations. `2A7290` is the input-dispatch override. The container vtable at `3D3D00` inherits ordinary child update/draw traversal and overrides the input slot.

Controls' show/hide/select/deselect/reset helpers traverse their authored pointer lists (`2A5920..2A5C64`). Control `+0x38` is not the entity child head (`+0x18`). Repeated references intentionally cause repeated callbacks; they do not cause repeated rendering of a single widget node. No new native input registration behavior is implemented here.

The page appends its input container at `1D6DDC..1D6DE8`, and the optional popup child at `1D7140..1D714C`. Manager widgets are not attached to these page-owned reference structures.

### Manager tree construction

Actual Pod manager creation `1CCBA0` calls:

1. Widget loader virtual slot `+0x98` at `1CCDC8..1CCDD0`, which is `1CD7A0` in the inspected Pod manager vtables. This attaches ordinary widgets.
2. Existing setup children through `1CD080` at `1CCDDC`. That helper conditionally appends manager fields `+0x180`, `+0x188`, and `+0x184`, in that order. Their semantics are outside this candidate.
3. Page loader virtual slot `+0x88` at `1CCDF0..1CCDF8`, which is `1CEC10`. It iterates one-based `Page N` definitions. Actual page factories append their newly created pages with `141960`; `1D03E0..1D0464` is a retained explicit example.
4. Deferred `OnTop` attachment through `1CDA70` at `1CCE04..1CCE08`.

Generic manager helper `2A49F0`, retained as slot `+0xA0`, instead scans all slots ascending and appends only non-null widgets with no existing parent (`2A4A14..2A4A50`). It must not replace the specific Pod loader/deferred path above.

`141960` appends an entity to the tail of its parent's child chain: child head `+0x18`, next sibling `+0x14`, previous `+0x10`, parent `+0x1C`; it assigns the supplied traversal mask at child `+0x20` and propagates descendant mask aggregates at ancestors `+0x24`. It does not sort by depth or widget ID. In the recovered attachment calls the mask argument is `-1`.

Each text widget owns its created TextOverlay children in line order. Authored multiline construction appends at `2A97F0` while looping ascending line index; single text appends at `2A99E4`; dynamic construction appends at `2A9710` but remains unsupported by this native owner. Child chains therefore establish widget-major, then source-line order. Text widgets' vtable at `3D4140` inherits `141ED0` update and `141D70` render traversal.

## Visibility, update, and render order

There is no per-page static-count draw sweep and no group-selection-derived render list.

- Entity update `141ED0` requires aggregate mask bit `0x2` at parent `+0x24`, then visits child head/next order. Each child must have `+0x29` bit 0 set, bit 2 clear, bit 3 clear, and own `+0x20` mask bit `0x2` set. It dispatches virtual slot `+0x38` with the source delta time (`141EFC..141F68`). These concrete flags must be established by integration; a guessed page visibility boolean is insufficient.
- Entity draw `141D70` requires aggregate mask `0x8`, then visits the same child chain. Child bit 0 must be set, bit 3 clear, and own mask `0x8` set. It calls the renderer state entry `+0x20` with zero before child virtual slot `+0x48` (`141D98..141DF0`). Update's bit-2 gate is not an extra draw gate.
- Recursive show/hide are inherited entity methods `141630`/`1415A0`, while nonrecursive flag setters are `141560`/`141520`. The loader calls recursive hide, and later authored/source callbacks control visibility. Do not rederive it from control or page metadata.
- UIManager update `2A3C80` processes its own state and optional callback before tail child traversal `141ED0` at `2A3FDC`. The manager render virtual slot is inherited `141D70`. Widgets and pages therefore participate as sibling children in the attachment order above.
- Pod page update `1D72F0` stores the supplied time at `page+0x298` and tail-jumps through `2A6BB0` to `141ED0`. It does not itself advance the page opening/closing geometry there.
- Page render slot `+0x48` is `1D7550` (vtable `3D16C0`). That function advances the existing page transition state and updates its two source primitives, then calls `2E21D0` first for `page+0x424`, then `page+0x428` (`1D797C..1D79A0`, call `1D798C`). If `page+0x420 == 0`, it calls `1D7A10` at `1D79B0`; that routine visits enabled bits `1,2,4,8` of `+0x42C` in that order, submitting corresponding existing objects at `+0x2A0/+0x300/+0x360/+0x3C0` through `2DF550`. Finally `1D79BC` calls `141D70` for page-owned children.
- These facts establish call order only. The staged candidate implements none of these page primitives, transitions, renderer state calls, selectors, backgrounds, or GPU semantics.

## Integration and verification boundary

Use the staged order only for an authenticated all-text Pod manager companion. Ordinary widget entries precede original intermediate children; deferred entries follow them. The two vectors are not two render passes. Running just those vectors as an entire screen would omit source-owned middle children. Binding a page/Widgets pair still does not independently authenticate their original screen pairing or supply the full manager.

A manager can share its widget objects across page control references. This one-page owner remains a narrow owner facade; constructing separate owners for multiple pages would duplicate runtime widgets and must not be claimed equivalent to the original shared manager. A future multi-page manager must own one shared widget population and the complete source-established tree.

Root owns the next authorized compiler/test/asset-decoder slot. Suggested future checks are source-aligned admission and order cases: absent/zero/nonzero/negative `OnTop`; mixed and empty partitions; nonzero static count without synthetic IDs; preserved duplicate callbacks with unique tree entries; immutable pointer lifetime; and independently gated manager/widget/line visibility. Those checks were not executed here.
