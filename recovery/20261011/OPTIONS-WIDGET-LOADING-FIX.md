# Options cold-loading correction

## Result and limits

The source-identified cold-load blocker is the generic DAT reader's invented balanced-quote requirement. It rejects the entire authenticated Options Widgets.Dat before either page controller can bind. The issue is present before the 56-widget bank expansion; no enlarged widget admission or original data change is needed.

This package is staged source only. No project parser, importer, native loader, test, syntax check, build, game, emulator, or guest instructions were executed. The active repository and existing validation evidence were not modified. Runtime confirmation belongs to root's next permitted centralized check.

## Exact authenticated trigger

Widgets.Dat is 58,854 bytes, SHA-256 05afba1bd71336fc1861c8045c6b3bde41002ca211558eef592a4d0fcd34117d.

Two genuine credits fields have an opening quote and no closing quote:

- Widget311, line2520, byte43571: `Text="Peter Choi` followed by CRLF.
- Widget251, line3255, byte56115: `Text="ASSOCIATE LOCALIZATION PRODUCERS` followed by CRLF.

The first causes the old `if (quoted || brackets!=0)` check in readOriginalUiDat50KG to return textSyntax at byte43571 with sourcePC0x159440. By static call tracing, that propagates through OptionsMenuController::bind as parseFailure(6), OptionsMenuWidgets::bind as controllerFailure(11), and DrawResources::load as widgets(5). That nested result was not emitted by the old host log, so this is a source-derived diagnosis, not an observed nested runtime receipt. Widget311 is outside the current live population but shares the same authenticated DAT; skipping it would hide a parser contract error.

The retained Oct11 02:18 game.log establishes only options_resource=5. That enum is also used by the later prepareIcon failure branch, so the log alone did not locate the failure at bind.

## Original contract proof

Original ELF SHA-256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Evidence is static word-to-text inspection, never guest execution.

- Whitespace cleaner0x159440 sets quote state0 and bracket depth0 anew on each invocation. At0x1594D4/0x1594D8 it toggles quote state; at0x159510..0x159524 it retains whitespace while quoted or inside brackets. At0x159550 it terminates output and returns without testing quote state. An opening-only quote is admitted.
- Section reading0x1580A4..0x1580C8 and0x1581A4..0x1581C8 fetches each original physical line and invokes that same cleaner independently. Quote state cannot leak to later sections.
- GetString0x158824..0x158830 independently removes a leading quote;0x158858..0x158860 independently removes a final quote if one exists. It does not require a pair.
- File::ReadLine0x12F224..0x12F244 normalizes CRLF to a retained LF. GetString splits on `=\n` at0x158728 and then `\n` at0x1587F4. Thus an opening-only quoted value preserves its internal spaces but does not retain CR or LF. The delimiter bytes are preserved in authentic-quote-specimen.json.

The candidate mirrors these semantics: quote tracking still controls whitespace, but an open quote at the end is no longer a syntax error. CR immediately before a consumed LF is excluded before value extraction. Existing independently applied leading/trailing quote stripping already matches the original and is unchanged. Original bytes, identities, offsets, source-value-presence, key lookup, repeated-key behavior, bracket bounds, encoding limits, capacity limits, and atomic publication remain intact. Section/key quote rejection remains in their respective schema checks.

## Deliverables and merge

1. `parser/changes.patch`: narrow two-file change to src/assets/original_ui_pages50kg.cpp and .h. Exact current preimages and candidate files are under parser/preimages and parser/files; corresponding SHA-256 manifests are alongside them.
2. `diagnostics/changes.patch`: independent nine-line addition within the existing unavailable-destination diagnostic stream in tests/native/original_ui_main_entry50kg.h. It reports resource widget ID, nested widget status/ID/controller, parse error/offset/PC, text status, and icon status. The exact named before/after stream block is also in diagnostics/unavailable-destination.before.txt and .after.txt. Merge this hunk into the concurrently updated page-navigation host entry; do not overwrite that worker's full file.
3. `evidence/authentic-quote-specimen.json`: source file identities, exact raw lines/bytes/offsets, and original delimiter bytes.
4. `evidence/*.words.txt`: exact original instruction words and static mnemonics for the relevant cleaning, string getter, physical-line, and section-reading paths.
5. Authenticated original English UI.loc/L_UI_ENG.bin copies and `game-options-localization.json` document that all newly admitted game-options text references are present. These are original-data-only research inputs, not generated labels.

Root should first review and merge these source deltas, then at its permitted slot run the normal complete Options cold load and real Main→Options navigation. Keep the expanded diagnostics for precise evidence if any later cold stage fails. Loading and UI admission are not yet runtime-verified by this package.
