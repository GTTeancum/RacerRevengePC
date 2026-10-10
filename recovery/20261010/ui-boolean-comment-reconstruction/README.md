# Original UI boolean and section-comment correction

Source-only correction, 2026-10-10 UTC. The four approved source files were
edited in `/workspace/shared/rr-recovery-git`; no build, syntax check, test,
project parser, importer, gameplay, guest execution, or emulation was run.
Only source text, original ELF words/strings, and authenticated extracted data
were inspected. Root owns the next central validation slot.

## Changed files

- `src/assets/original_ui_pages50kg.h`: declare a typed boolean reader, append
  `booleanSyntax` without renumbering prior diagnostics, describe the admission.
- `src/assets/original_ui_pages50kg.cpp`: implement that reader, use it for
  `UseVideoAsTexture`, `HasPopup`, and `KeepEnabled`, admit trailing
  section-header semicolon comments, and correct bounded integer admission to
  base-0 with authored semicolon annotations.
- `src/assets/original_ui_icon50kg.cpp`: redirect its existing boolean wrapper.
- `src/assets/original_ui_overlay_text50kg.cpp`: redirect its boolean wrapper.

Generic integers remain numerically typed; alphabetic boolean coercion was not
added to them. No float conversion, material-field schema, renderer, owner,
runtime, or authored asset bytes were changed. `preimages/` preserves the inputs;
`files/` contains their final edited versions; `patch.diff` is their bounded diff.

## Original helper semantics

The original ELF is `SLUS_202.68`, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Its first PT_LOAD maps file offset `0x80` to VA `0x100000`.
`evidence/elf-identity-and-strings.json` records its program headers and exact
string bytes. The accompanying `.words.txt` files are static word decodes, not
an interpreter or executed source oracle.

The defaulted getter at `0x158B30` calls `0x158920` and writes its supplied default
only when the value lookup is absent. In the present-value path, `0x158920` does:

1. Case-insensitive one-character comparisons against `T` and `F`, using
   `0x309538` with length 1. True/false words are therefore recognized by their
   first character, rather than through a new native synonym table.
2. Case-insensitive whole-string comparisons against `ON` and `OFF`, using
   `0x308F00`.
3. Case-insensitive one-character comparisons against `Y` and `N`.
4. A base-0 integer conversion at `0x30AE70`, followed by a low32 store.
   The end pointer is supplied but not checked by this getter.

Exact strings are at VA `0x3A6348`, `0x3A6350`, `0x3A6358`, `0x3A6360`,
`0x3A6368`, and `0x3A6370`. ON/OFF and Y/N support are thus sourced, not invented.
The comparison routines fold ASCII uppercase using the source character table.

The native boolean reader preserves those alphabetic decisions. Its numeric
admission shares the bounded integer reader: a signed32 base-0 token with
optional sign, decimal by default, octal for leading zero, hex for `0x`/`0X`.
Leading/trailing whitespace and a trailing semicolon annotation are admitted;
stored value text is not changed. It converts admitted zero to false and nonzero
to true. It rejects empty quoted values, missing digits, invalid digits,
noncomment suffixes, and overflow rather than pretending to implement the
source's permissive conversion or low32 overflow.
Missing values still use the caller's original default. A failed typed read
leaves output unchanged and reports the original member's value byte offset.
The generic integer API uses that same bounded numeric conversion, but does not
gain the source getter's alphabetic boolean handling. Float readers are unchanged.

## Confirmed call sites

- `KeepEnabled`: `0x1D7120`, default 1; string at `0x3AEF20`.
- `HasPopup`: `0x1D6584`, default 0; string at `0x3AEC70`.
- `UseVideoAsTexture`: `0x1D6D74`, default 0; string at `0x3AEE60`.
- `WantsJoystickInput%d`: `0x2A44C0`, default 0, loop for inputs 1 and 2;
  format string at `0x3BEE20`.
- Icon select/deselect flags: `0x2AC5C8`, `0x2AC5E0`, `0x2AC5F8`, `0x2AC610`.
- Overlay_Text select/deselect flags: `0x2A9038`, `0x2A9050`, `0x2A9068`,
  `0x2A9080`.
- `OnTop`: `0x1CD890`, default 0; string at `0x3AD820`.

These are existing typed native boolean consumers. There is no global T/F
coercion for arbitrary numeric fields. The movement/camera worker was told to
use the new typed reader when adding its separately owned joystick fields.

## Section-header trailing comments

The original section indexer calls `0x159440` to clean whitespace and then
`0x309348` at `0x157550` to find the first of the bytes `]\n\r`
(string at `0x3A6220`). It copies only the name before that delimiter and
terminates it at `0x157574`; a tail after `]` is not part of the section name.

Authenticated Main PSMs contain, at byte 61, the line
`[ShaderPass_1]                            ;---------------------------------------`.
For example `ui\\textures\\mmbkg.psm` has SHA256
`ad952605d662e15db67b92ea396da4aacfa0eb27f13523228cfeeb674e50207d`.
The previous native reader incorrectly required the entire cleaned line to
end with `]` and rejected those real material sections.

The patch admits only a semicolon tail directly following the closing bracket
and optional cleaned whitespace. It stops before comment quotes or brackets
can alter syntax state. Noncomment trailing junk still rejects. Semicolons in
ordinary or quoted values remain untouched. Section/value source offsets and
authenticated archive bytes are preserved. This is a narrower admission than
the original section indexer, which simply discards any tail after the delimiter.

## Authored integer annotations

Root explicitly expanded this same four-file scope to integer annotations and
base-0 correctness. Rider and Course controls contain `WidgetID1=9 ; Rotating ...`;
the previous all-decimal/all-characters reader rejected their retained suffixes.
The original getter's fallback calls `0x30AE70` with base 0 at `0x158A34`, ignores
the end pointer, and stores the low 32 bits. The underlying conversion selects
hex at `0x30ACFC` and octal/decimal at `0x30AD08..18`.

The native parser now admits a valid signed32 integer followed only by whitespace
and/or a semicolon annotation. Other suffixes still reject. For example `08`
cannot be mistaken for decimal 8: it is invalid in this bounded octal domain.
Missing digits and signed32 overflow reject before publication. Accepted source
numbers keep the source base, avoiding the old decimal interpretation of octal.
The same helper is used by numeric boolean values, so those paths cannot diverge.
All output assignments remain after validation. The retained DAT value text and
diagnostic value offsets are untouched; semicolons in string values are not
globally removed. The data worker reported no quotes or unbalanced brackets in
the inspected authored integer annotations.

## Evidence and remaining limits

The authenticated data inventory is under
`/workspace/shared/recovery-20261010/authored-menu-data-inspection`.
`evidence/authored-specimens.json` records exact Main member hashes, selected
raw lines and byte offsets, and relevant boolean inventory counts. Other
authored boolean keys without existing native consumers were not added here.

Original generic integer callers include page membership and count readers,
widget metrics, and shader/GS fields; static retained call records and fresh
call-word cuts confirm their shared `0x158B30` target. Other source-permitted
suffixes, integer overflow/narrowing, failed-conversion coercion, and alphabetic
boolean tokens in numerically typed fields remain deliberately unsupported.
UI owner feature gates and missing dynamic widget owners remain separate work.
No full-menu readiness or runtime equivalence is claimed.

Verification was manual source/diff and static provenance inspection only.
The next central validation should cover authored Main groups, typed widget
flags, original PSM section headers, output-preserving malformed-value rejection,
and bounded base-0 integer annotation handling. No claim of passing tests is made.
