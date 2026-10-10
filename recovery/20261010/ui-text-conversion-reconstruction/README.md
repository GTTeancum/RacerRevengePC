# Original UI text conversion correction

Date: 2026-10-10 UTC. Source-only staged correction, not integrated.

## Result and scope

The original instructions for text metrics, line leading, justification, and text
RGB are **CVT.W.S**, whose EE behavior truncates toward zero. Both existing
nearest-even helpers are confirmed semantic mismatches. The shared text-metric
helper's icon RGB consumer also uses CVT.W.S, so correcting that helper does not
substitute a different conversion family for this consumer.

`files/` contains the complete four changed files; `preimages/` contains their
complete frozen-checkout inputs. `patch.diff` is a four-file unified diff with
repository-relative labels. The active repository was read but never modified.
The parent supplied frozen capture base `17659ca`; no Git command, integration,
build, test, syntax check, importer, gameplay, emulation, or numerical experiment
was performed. Inspection used source text, original ELF bytes, primary manuals,
file hashing, and textual diffs only. Root owns integration and future validation.

Changed files:

- `src/game/original_ui_text_position50kg.cpp`: replace local nearest-even
  conversion with bounded signed truncation; retain the metric/position API,
  existing arithmetic order, and output-on-success behavior.
- `src/game/original_ui_text_position50kg.h`: correct conversion contracts.
- `src/assets/original_ui_overlay_text50kg.cpp`: replace only its RGB
  nearest-even helper with signed truncation after the existing finite `[0,255]`
  admission check.
- `src/assets/original_ui_overlay_text50kg.h`: correct the RGB conversion comment.

The API name `roundOriginalUiTextMetric50KG` is deliberately retained to avoid a
call-site refactor. Its name no longer implies nearest-even behavior; the header
specifies the actual operation. No icon source edit is needed because the icon
reader already calls this helper. Its post-conversion `[0,255]` check is unchanged.

## Original source identity and byte provenance

ELF:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

The load segment maps file offset `0x80` to VA `0x00100000`, so for all listed
instructions `VA = file_offset + 0x000FFF80`. `evidence/original-bytes.txt` records
the SHA256, ELF load mapping, and original word dumps with surrounding operations.
The words below were read from that ELF, independently of native helper comments.
The three-operand CVT display in the retained disassembly cuts is only a formatting
artifact; the instruction has destination and source operands.

| Original path | VA | File offset | Word | Instruction |
| --- | --- | --- | --- | --- |
| GetHeight, pixel metrics | `0x2E168C` | `0x1E170C` | `46000024` | `CVT.W.S f0,f0` |
| GetHeight, normalized metrics | `0x2E16C4` | `0x1E1744` | `46000024` | `CVT.W.S f0,f0` |
| GetWidth, normalized metrics | `0x2E18D0` | `0x1E1950` | `46000024` | `CVT.W.S f0,f0` |
| GetWidth, pixel metrics | `0x2E18E8` | `0x1E1968` | `46000024` | `CVT.W.S f0,f0` |
| Leading branch 1 | `0x2A96B4` | `0x1A9734` | `46000024` | `CVT.W.S f0,f0` |
| Leading branch 2 | `0x2A98D8` | `0x1A9958` | `46000024` | `CVT.W.S f0,f0` |
| Leading branch 3 | `0x2A9D4C` | `0x1A9DCC` | `46000024` | `CVT.W.S f0,f0` |
| CenterHorizontal X | `0x2E149C` | `0x1E151C` | `46000024` | `CVT.W.S f0,f0` |
| CenterVertical Y | `0x2E14DC` | `0x1E155C` | `46000024` | `CVT.W.S f0,f0` |
| CenterBoth X | `0x2E1510` | `0x1E1590` | `46000864` | `CVT.W.S f1,f1` |
| CenterBoth Y | `0x2E1524` | `0x1E15A4` | `46000024` | `CVT.W.S f0,f0` |
| Right X | `0x2E153C` | `0x1E15BC` | `46000024` | `CVT.W.S f0,f0` |
| Color R | `0x2A9238` | `0x1A92B8` | `460010A4` | `CVT.W.S f2,f2` |
| Color G | `0x2A9250` | `0x1A92D0` | `46000864` | `CVT.W.S f1,f1` |
| Color B | `0x2A9268` | `0x1A92E8` | `46000024` | `CVT.W.S f0,f0` |
| HighlightColor R | `0x2A92BC` | `0x1A933C` | `460010A4` | `CVT.W.S f2,f2` |
| HighlightColor G | `0x2A92D4` | `0x1A9354` | `46000864` | `CVT.W.S f1,f1` |
| HighlightColor B | `0x2A92EC` | `0x1A936C` | `46000024` | `CVT.W.S f0,f0` |
| DisabledColor R | `0x2A9340` | `0x1A93C0` | `460010A4` | `CVT.W.S f2,f2` |
| DisabledColor G | `0x2A9358` | `0x1A93D8` | `46000864` | `CVT.W.S f1,f1` |
| DisabledColor B | `0x2A9368` | `0x1A93E8` | `46000024` | `CVT.W.S f0,f0` |
| Icon Color R | `0x2ACABC` | `0x1ACB3C` | `460010A4` | `CVT.W.S f2,f2` |
| Icon Color G | `0x2ACAC8` | `0x1ACB48` | `46000864` | `CVT.W.S f1,f1` |
| Icon Color B | `0x2ACAD8` | `0x1ACB58` | `46000024` | `CVT.W.S f0,f0` |

The retained source cuts copied into `evidence/` corroborate both the first two
leading branches and all nine Overlay_Text RGB conversions. Their originals are
under the frozen repository's
`recovery/20261010/ui-static-membership-reconstruction/evidence/`.

The metric conversions immediately follow the source final multiply and precede
MFC1. The leading conversions follow separate multiply, multiply, and add steps.
The justification conversions follow the relevant subtraction and are followed
by CVT.S.W before storing positioned coordinates. The original explicit-Height
branch uses integer multiplication/addition, so its arithmetic is left alone.
The RGB paths convert channels before shift/OR packing.

## Governing conversion semantics

Primary Sony-authored manuals, publicly mirrored:

- [EE Core Instruction Set Manual v6.0, p356](https://docs.alexrp.com/mips/ee_insns.pdf#page=356):
  CVT.W.S produces signed 32-bit truncation toward zero, with signed endpoint
  saturation when the encoded exponent exceeds `0x9D`. Its opcode fields identify
  the three words used above as CVT.W.S, not FTOI0.
- [EE Core User's Manual v6.0, section 8.7 p164](https://docs.alexrp.com/mips/ee.pdf#page=164):
  rounding mode is fixed toward zero; FCR31 rounding bits are hardwired. Section
  8.8 distinguishes EE arithmetic from IEEE 754, so native float materialization
  alone does not prove parity for all preceding arithmetic inputs.
- [C++ draft, floating-integral conversions](https://eel.is/c++draft/conv.fpint):
  a representable floating-to-integer conversion truncates; an unrepresentable
  result must be excluded before the cast.

The supplied earlier research at
`../ui-aperture-numeric-research/README.md` was read as background. Its conversion
conclusion is independently applicable here because the original text and icon
opcodes were checked. No aperture implementation is changed by this patch.

## Native bounded behavior and limits

- The local `truncateToInt32` rejects nonfinite values and checks the value against
  signed 32-bit limits in `double` before casting. For the existing binary32 input
  contract this admits `[-2147483648,2147483648)`. Comparing against a rounded
  `float(INT32_MAX)` is avoided. Failed calls do not publish partial results.
- This is an explicit narrower native domain, not an implementation of EE's
  exceptional-value saturation. No host out-of-range cast occurs on an admitted
  conversion. No interpreter or emulation layer is introduced.
- Overlay RGB keeps its existing finite `[0,255]` input guard. Its signed cast is
  representable, and subsequent packing uses unsigned values and shifts.
- The caller domain remains source-reader-produced widgets: at most 64 lines and
  signed16 authored coordinates/height. In that domain the unchanged explicit-
  Height path's widened `int64_t` multiply/add cannot overflow. Arbitrarily
  hand-constructed unbounded widget vectors are outside this contract; this
  narrow correction does not redesign that pre-existing API boundary.
- No new signed integer arithmetic is introduced. Existing measured-metric
  nonnegative admission, empty-line behavior, authored fields, depth/alpha,
  packet color shifts, all FTOI0 paths, and float arithmetic order are unchanged.
- `volatile float` steps remain as the existing bounded native arithmetic. The
  comment no longer claims that they prove EE arithmetic parity. Universal
  bit-exact EE floating-point reconstruction is neither implemented nor made a
  prerequisite for this isolated, source-confirmed conversion correction.

Historical recovery copies and their checksums are intentionally unchanged.
Their old nearest-even statements describe the old reconstruction and must not
be treated as the corrected conversion contract.
