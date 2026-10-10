# Static-text page container ownership candidate

New native CPU groundwork, not recovered lost source and not a complete original menu. The two files under `src/game/` are staged for root-owned integration. No repository, application entry point, build recipe or frontend activation is changed here.

## What this owns

`OriginalUiStaticTextPageOwner50KG` parses the exact authenticated page and companion Widgets DAT members through the restored readers. It retains both DATs, the typed page, all admitted text descriptors, one fixed-address text runtime per declared widget, and navigation state. It holds genuine shared ownership of the two immutable archive owners. The optional English lookup is used only while binding; resolved string bytes are copied into owned descriptors.

All descriptors finish construction before any runtime pointer is bound. The heap storage itself never moves; only its unique ownership handle is published. Copy, move, reload and mutable access are unavailable. Returned const pointers and projection text pointers remain valid until this owner is destroyed. Do not destroy it while another consumer retains those pointers. Calls are serial; input/output/diagnostic storage must be disjoint and callers must not cast away constness.

Binding is atomic and retryable on failure. It does not guess a page filename, widget companion, initial group, controller or visibility. The caller must establish the original member pairing from source-owned screen setup; successful authentication of two independent members does not prove they belong together. Main dispatch is specifically the restored main-screen leaf, and must not be called on an arbitrary page merely because it has group 1.

## Admission is deliberately all-or-nothing

Every widget declared by `General.NumberOfWidgets` must be an admitted `Overlay_Text`, even one unused by the selected page. Non-text widgets, dynamic text, unimplemented joystick callbacks, unexpected DAT sections and present fields outside the explicit restored schema reject the whole bind. No unsupported widget is omitted and no supported subset is published. Bare `Key=` is absent under the existing DAT contract; it does not request behavior.

The owner also rejects nonzero `NumberOfStaticWidgets`, popup requirements and video-as-texture requirements. The count alone does not establish static widget membership. It does not infer a static list from leading widget IDs or control 0. Restored reader and text-line bounds remain in force, including signed16 raw authored alpha.

On rejection, the diagnostic preserves input kind, widget ID, exact parsed key/section spelling, member byte offset and available source anchor. Native schema restrictions carry source PC zero. Typed reader failures retain their own error/offset/PC. Diagnostics own their labels, so a failed unpublished candidate cannot leave dangling diagnostic strings. `commandCount` and `callbackCount` are committed-batch counts; they stay zero when command preflight fails.

This is intentionally too restrictive to activate a general original menu. `bind == ok` means only that this bounded static-text container was admitted. Page `Start`, `Time`, `OpenSize`, background and other parsed metadata remain evidence for a separate page/screen controller, not implemented behavior. No transition is advanced or marked finished. No font, material, atlas, artwork, route, sound or complete screen is supplied.

## Integration contract

1. Add the staged `.h/.cpp` beside the restored game leaves only after root review. Existing dependencies are `original_ui_pages50kg`, `original_ui_overlay_text50kg`, `original_ui_localization50kg`, `original_ui_navigation50kg`, `original_ui_text_widget50kg` and `original_ui_text_position50kg`, plus their already-existing archive/reader dependencies. This candidate adds no renderer or platform dependency.
2. Bind with genuine `shared_ptr<const OriginalUiArchive50KG>` owners that have already loaded/authenticated original bytes, explicit original member names, and an optional authenticated English lookup. Reject every status other than `ok`; do not fall back to a partial screen. Archive loading, filesystem access and localization parsing are outside this class.
3. Supply only source-established page lifecycle callbacks through `applyCommands`, or use `enterGroup`, `moveControl` and `enableControl`. `enterGroup` does not imply page visibility. There is no implicit enter, show/reset sweep, static membership, or synthesized ancestor state.
4. Each navigation operation works on a temporary native navigation state. Every generated command and all its members are preflighted before callbacks. Commit traverses command order, then that control's authored widget order, without deduplication. `applyCommands` mirrors the navigation leaf's per-control flag writes, but does not infer a group slot or controller attachment. Generated navigation commits its complete state only after callbacks.
5. `dispatchMain` preserves the exact original decision and unavailable/second-controller outcomes. Supply only capabilities backed by actual original-menu implementations. Receiving a route request does not execute a route, change options or start gameplay.
6. Call `updateStatic(widgetId, ancestorsShown, diagnostic)` only at the source update point, in source-established order. Visibility is mandatory input; there is no default `true`, invented page clock, or numeric widget-ID sweep.
7. Resolve fonts/material/depth and validate downstream draw admission separately. Obtain original integer ScreenText metrics, using the existing rounding leaf where appropriate. `positionLine` provides a pure preflight. At the source draw point, `projectLine` computes position and calls the text packet projection without updating first. Hidden/failed projections leave output and last-rendered state unchanged. Success records CPU packet preparation only, never successful GPU submission. Do not call it speculatively just to inspect a line if advancing last-rendered state would be wrong.
8. Preserve source packet color, authored depth and material names as separate evidence. This owner introduces no Z conversion, material flags, alpha normalization, blending defaults, extra artwork, animation timing or draw ordering.

Line setup/color/alpha/stop wrappers retain the text leaf's lock gates and exact results. Timed alpha, blink, finite hold, second phase and transformed setups remain explicitly unsupported. There is no fast-forward to a guessed settled state.

## Provenance and review boundary

Implementation delegates behavior to the restored repository source at `/workspace/shared/rr-recovery-git`:

- `src/assets/original_ui_pages50kg.{h,cpp}`: source page/control/group membership readers (1D6DEC through 1D7284), DAT first-present lookup and native validation bounds.
- `src/assets/original_ui_overlay_text50kg.{h,cpp}`: actual `Overlay_Text` schema and line creation branches (2A9000), type dispatch anchor 2A4748, manager input/initial hide anchors 2A44C0/2A44E0.
- `src/game/original_ui_navigation50kg.{h,cpp}`: exact entry/navigation command order, associations and repeated callbacks (2A6BE0, 2A4DFC through 2A5244), enable helper 2A5B60 and main dispatch 1E5E00.
- `src/game/original_ui_text_widget50kg.{h,cpp}`: fixed-address state, source callback and lock semantics, static animation admission, and separated update/packet preparation. Its retained provenance is `../ui-exact/widget-author/RECOVERY.md` and review is `../ui-exact/widget-author/review/source-review-20261010.md`.
- `src/game/original_ui_text_position50kg.{h,cpp}`: source metric rounding, line placement and justification, plus source packet color bytes. The owner does not duplicate their arithmetic.

The new ownership mechanism, strict schema whitelist, shared archive pinning, command preflight, error aggregation and wrappers are new native integration code. These should not be described as recovered original implementation or as independently re-disassembled source. Unknown fields are rejected as a native safety restriction, not claimed absent from the original executable.

Source inspection covered publication/lifetime order, original one-based widget and zero-based control indexing, empty/absent controls, duplicate callback traversal, navigation copy/commit behavior, unavailable decisions, locked/unsupported line calls, and update/projection separation. No build, compile, syntax check, test, importer, project script, gameplay, GPU validation or user-computer operation ran. Root owns the next centrally authorized validation and all actual screen activation gates.
