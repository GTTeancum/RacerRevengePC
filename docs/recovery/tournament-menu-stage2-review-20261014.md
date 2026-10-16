# Tournament menu stage 2 source-only review, 2026-10-14

## Scope and disposition

Reviewed the seven source files across `c06a1b0..aefd3b38280b635642b3ae034ab74bc3ab46decf`: the two input files enter in `25b857ee9a63a35f22db1de86570346e7ddefe33`, and the five asset files enter in `aefd3b3`. Reviewed source and retained evidence with `git show`; no source changes were applied. This document is the only reviewer-owned file.

The archive/catalog and cold page-data proposal are consistent with the original static data. The input proposal is consistent with the original input leaf and inherited request helper. No concrete source correctness or declaration incompatibility defect was found within these seven files. This is a static review conclusion, not compilation, runtime, renderer, session, or frontend completion evidence. Admission remains closed.

No build, test, syntax check, project import/execution, sanitizer, gameplay, guest instruction execution, or emulation was performed. Standard-library ZIP/zlib decompression, hashing, and raw byte comparisons below operated only on original data and Git evidence blobs.

## Findings requiring root action

### 1. New translation units require explicit CMake registration before use

`platform/linux/native/CMakeLists.txt` defines `rr_native_gameplay50dj` with explicit source paths. It already lists `original_ui_archive50kg.cpp`, `original_ui_pages50kg.cpp`, and `original_ui_route_decisions50kg.cpp`, but neither new translation unit appears in that list:

- `src/assets/original_ui_tournament_page50kg.cpp`
- `src/game/original_ui_tournament_input50kg.cpp`

Root must register both when integrating these modules into the existing native library; otherwise a consumer of their out-of-line functions will have no corresponding implementation in that target. The modified archive implementation is already registered. The catalog `.inc` is included directly at archive implementation line 10 and does not need separate compilation. Headers need no build registration for linkage. Source-list wiring alone provides no Tournament admission or session integration.

### 2. Retained DAT evidence must be labeled as normalized transcription

`STAGE2.md` lines 31–32 currently describes “Recovered DAT bytes” without stating that Git retained LF-normalized text. The copies are not byte-identical to the original CRLF members:

| File | Git blob bytes | Original member bytes | Original member SHA256 |
| --- | ---: | ---: | --- |
| ui.dat | 1410 | 1473 | 9a38ac2d19c89d86e4932c95a62e23f67fe775671692e297bad3d6a5adf00748 |
| Widgets.Dat | 12537 | 13308 | 754427aa2ce3295dacfb49e4d85f52a48774bd266a0d8a291635758004256bd8 |
| standing.dat | 1355 | 1428 | 82d33a5fd29f77feb33c19bb913e2e7af05e640cfb193fafd1e51ad85c4d809e |

Static comparison establishes that every retained copy is exactly its original member with CRLF replaced by LF. Conversely, converting retained LF to CRLF recovers each original size and SHA256. There is no other content discrepancy. Runtime code correctly reads authenticated archive members rather than these copies, so this is an evidence-description correction, not a runtime substitution defect.

Exact replacement for `STAGE2.md` lines 31–32:

> Movement_Control. The retained DAT files are LF-normalized evidence transcriptions of the original CRLF members, not byte-identical archive members. Runtime parsing uses the authenticated archive; its member sizes and SHA256 values apply to the original bytes.

## Independently verified asset provenance

Original input: Library original UI ZIP `libfile_7b1f19ac52f88191b53da4f351138f95`, archive entry `UI/UITOURN/UITOURN.RES`.

- Original RES is 1,624,190 bytes, SHA256 `bae6a47a7cd406aba1905ea4bfa7d9ba3527eccb9a06955323e33979c3e1c99c`, matching the new archive identity.
- Static decompression yields 344 complete 24,576-byte blocks, 8,454,144 decoded bytes, exact index boundary and exact compressed-file boundary.
- All 224 original member names, order, logical offsets, byte sizes, and SHA256 values equal `tournament-members.json` and the appended runtime catalog entries.
- The runtime catalog contains 1,284 members; appended Tournament range starts at 1,060 with count 224. Existing IDs 0–11 remain unchanged and Tournament is appended as ID 12.
- Existing archive loading authenticates exact RES bytes, exact member-table contents, and every member hash before publishing storage. The new ID uses that same implementation.
- Authored widget counts agree with documentation: 31 Overlay_Text, 23 User_Rotate, four Icon, four Empty, and one Movement_Control.

Original SLUS SHA256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`, equal to `source-identities.txt`. Every retained PC/raw-word pair was compared directly with the original file bytes: screen-request 24, construction 1,152, entry-prefix 152, entry 3,508, input 88, popup-draw 460, and completion 76 words; no raw-word transcription mismatch was found. The partial decoder's printed operand formatting is not a substitute for interpreting raw words, especially shifts, floating instructions and unknown `.word` values.

## API and ownership review

### Cold page-data owner

`OriginalUiTournamentPageData50KG::load` uses existing declared archive/DAT/page APIs and their existing defaults. It requires an already loaded authenticated Tournament archive identity, obtains the authored PageFile from ui.dat, and requires the single `standing.dat` page. It retains ui/page/widgets DAT values and the page together with the same `shared_ptr<const OriginalUiArchive50KG>` in one storage object. Publication occurs only after successful parsing and page shape checks. Failed cold loads leave `storage_` unpublished; duplicate loads reject.

The exact page checks agree with original standing.dat: PageNo 1, three controls/groups including entry zero, popup true, OpenSize 69, Time 0.5, Start Closed. Existing page parser handles the source's inclusive counts and trailing integer annotations. Widgets uses explicit `preserveInOrder`; the API exists at the reviewed revision. No dangling temporary page/index reference is published. Accessors return null when unloaded. The code retains no invented standings/profile state and does not claim `loaded()` is rendering readiness.

The owner is deliberately not a native Tournament manager, widget renderer, popup/save owner, camera/light/model bank owner, dynamic text projection, or GPU frame-retirement owner. Its shape checks and authenticated inputs cannot replace those owners.

### Input proposal

The source leaf at 1F2310 first rejects popup byte29 bit0 and any nonzero controller, then dispatches only events 4, 6 and 5. The proposal preserves the observed destinations and call-site PCs:

- Event 4: Course 6/1, request 1F2374, sound 1 at 1F2394, transport 1F23AC.
- Event 6: Rider 5/1, request 1F23C8, sound 5 at 1F23E8, transport 1F2400.
- Event 5: Garage 13/1, request 1F241C, sound 1 at 1F243C, transport 1F2454.

Inherited helper 1D6130 suppresses its request when actual manager state170 is 2. The caller's subsequent sound and transport remain outside that suppression guard; the proposal correctly retains those effects while request.kind remains none. `status == proposed` therefore must not be interpreted as a present screen request or admission. The source leaf's final constant return value is not modeled because this API returns an effect plan rather than the original callback ABI.

The plan has no profile changes, no externally admitted destination flag, and no side effects. Caller-supplied popup/manager values must come from actual retained owners. A future session must preserve request/audio/transport ordering, validate destination and audio ownership, and recheck same-generation lifecycle facts before consuming it. Garage remains unavailable; the numeric proposal cannot open it.

## Integration boundary

Safe review disposition: root can retain this source/data slice, correct the evidence wording, and explicitly wire the two translation units without claiming native Tournament readiness. This review does not authorize relaxing Rider mode2 confirmation, Course mode2 entry/launch, frontend route/admission masks, profile/result transaction gates, save handling, or renderer ownership. No frontend/session implementation is supplied by these seven files. OriginalUiTournamentSession50KG remains future work, with actual popup, profile, dynamic projection, models/materials/banks, lifecycle callbacks and deferred-frame retention required before admission.

## Stage 3 extension: 05296ffa92080f1ce561860fb4599a692d83cc11

At root's request, also reviewed the subsequent delta to `original_ui_aperture_owner50kg.{h,cpp}` and `STAGE3.md`, without applying source changes or executing the project.

The existing aperture implementation is already explicitly registered in the native CMake target. Stage 3 adds no new translation unit and requires no additional source-list entry beyond the two stage-2 files above. Its private `bindPageImpl` adds a default-false final flag; existing bind callers retain their previous branch selection. Tournament binds authenticated ID12, follows the actual ui.dat PageFile, and retains the archive. Its source dt save uses the existing guarded delta API, while timer4064 remains explicitly external.

The reviewed retained words support the selected lifecycle behavior:

- `1F2E30` does nothing for phase1/entry; phase2 starts closing only outside closed2/closing3, clears input268, and calls B4 at1F2E7C. New Tournament branch and emitted target1EEBE0 agree.
- The selected render branch at1F2DB8 requires aperture state2 and manager phase1, writes state1, and invokes AC at1F2DF8. New entry-at-render API preserves those tests and target1EEB50 for its admitted popup-inactive subset.
- Shared rendering uses existing aperture arithmetic with original 69/0.5 configuration and new B0 target1EEBA0 / B8 target1EEC30. The latter is the source no-op restored callback.
- `1F2F00` returns complete for entry regardless of aperture state, complete for exit when closed, and otherwise writes4060=1 and returns pending. The new poll mirrors this, including none/unknown phase falling into pending with4060 recorded. Wrong route, lost dependencies and operation reentrancy remain native rejection boundaries.
- Direct original byte inspection at1F2470..1F249C supports the separate timer4064 handling and dt298 store before tail-transfer to2A6BB0. No actual update/session implementation is supplied.

### Important partial-branch limitation

The original label1F2DB8 is **not exclusively popup-inactive**. `tournament-popup-draw.txt` proves three arrivals: popup bit0 clear at1F28B0, aperture not closed at1F28C4, and popup active with exitRequested4060 nonzero at1F28D0. In the last case, if the aperture is closed and manager is entry1, the source can still enter the opening transition. The new `beginTournamentEntryAtRender48` rejects every active-popup invocation regardless of retained4060.

This rejection is documented in its header and stage report as a deliberately limited API; it is therefore an incomplete render predicate rather than evidence that active-popup behavior was recovered. Root must not treat it as the full original predicate. Before a complete Tournament session is admitted, the actual popup/save owner must join all source render branches, including the4060 bypass. Do not remove the popup guard merely to make the screen work without that owner.

Exact clarification recommended after `STAGE3.md`'s entry-at-render bullet:

> The1F2DB8 label also receives the active-popup/exitRequested4060 bypass from1F28D0. This API currently admits only its popup-inactive subset; the actual popup/save session must implement the remaining branch before full-screen admission.

### Fresh-owner entry boundary remains mandatory

Source entry74 resets4060 at1EEAB8 or1EEAD0 and timer4064 at1EEAD4. The new owner initializes4060 false on construction but provides no entry74/reset API. Once a pending poll sets it, the bit remains true for that owner's lifetime. A future same-owner re-entry cannot implement source entry74 with the current API. `STAGE3.md` acknowledges this; retain the fresh-per-screen restriction or add the proven entry reset operation with the actual session. Poll's const signature now mutates this explicit source field, which is visible in the revised header contract; callers must not mistake it for an effect-free query.

Stage 3 remains a source-only aperture slice with a known popup/render subset and no full native screen. No admission gate should change on the strength of this delta.
