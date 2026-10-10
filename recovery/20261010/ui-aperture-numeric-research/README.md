# Aperture CVT.W.S conversion research

Date: 2026-10-10 UTC. Research only; no repository code changes, builds, tests,
guest execution, emulation, importers, or numerical test scripts.

## Result

The original aperture integer step uses **signed truncation toward zero**.
The nearest-even helper in `original_ui_text_position50kg.cpp` is not a valid
conversion reference for this aperture. A native host `int32_t` cast is suitable
for the finite, representable conversion domain; outside it, use the documented
source saturation or explicitly reject unsupported inputs before casting.
Conversion semantics are resolved. General bit-exactness of the preceding
floating-point multiply has not been established by this research.

## Original binary evidence

Original file:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`

SHA256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Its ELF load segment maps file offset `0x80` to VA `0x00100000`.
The original bytes were read independently of the existing reconstruction:

| Virtual address | File offset | Word | Interpretation |
| --- | --- | --- | --- |
| `0x001D75E4` | `0x000D7664` | `46000802` | `MUL.S f0,f1,f0` |
| `0x001D75E8` | `0x000D7668` | `46000024` | `CVT.W.S f0,f0` |
| `0x001D75EC` | `0x000D766C` | `44030000` | `MFC1 v1,f0` |
| `0x001D76B4` | `0x000D7734` | `46000802` | `MUL.S f0,f1,f0` |
| `0x001D76B8` | `0x000D7738` | `46000024` | `CVT.W.S f0,f0` |
| `0x001D76BC` | `0x000D773C` | `44030000` | `MFC1 v1,f0` |

The supplied disassembly in
`recovery/20261010/ui-transition-callback-reconstruction/evidence/aperture-state-motion-render.txt`
agrees. Its three-operand rendering of `cvt.w.s f0,f0,f0` is a formatting artifact:
the instruction has only destination and source register operands.

Both paths load raw speed from object `+0x4B8`, raw saved delta from `+0x298`,
multiply once, convert once, and reuse that signed step for opposite updates of
the integer top/bottom coordinates. State 3 subtracts from bottom and adds to top;
state 1 adds to bottom and subtracts from top. Preserve this order and the later
source comparisons. No minimum-one-pixel step, nearest-even rounding, absolute
value, positive-only substitute, or elapsed-time endpoint replacement is justified.

## Governing hardware evidence

Primary authored source, publicly mirrored: Sony Computer Entertainment,
[EE Core Instruction Set Manual v6.0, p356](https://docs.alexrp.com/mips/ee_insns.pdf#page=356).
Its opcode fields encode `0x46000024` for `fd=fs=0`. The description specifies
conversion to a signed 32-bit result with truncation toward zero. Its operation
uses normal conversion for exponent bits 30:23 at most `0x9D`; greater exponents
produce `0x7FFFFFFF` for sign zero or `0x80000000` for sign one. This explicitly
settles overflow values rather than relying on a host out-of-range cast. The
same manual, p364, describes MFC1 as sign-extending the resulting word.

Primary authored source, publicly mirrored: Sony Computer Entertainment,
[EE Core User's Manual v6.0, §8.7 p164](https://docs.alexrp.com/mips/ee.pdf#page=164).
Only rounding toward zero exists; FCR31 bits 1:0 are hardwired to `01`, not a
programmable rounding mode. Thus an assumed startup FCSR value cannot make
these sites nearest-even. §8.3.2 pp158–159 identifies writable status flags, not
a rounding selector. §8.8 p165 warns that EE arithmetic differs from IEEE 754,
including special values and low-bit computation differences.

An aligned original-ELF search found three `CTC1 t1,FCR31` candidate words at
VA `0x00251DA4`, `0x00252084`, and `0x002CCC08`. Each surrounding sequence loads
FPRs and loads t1 from a memory block immediately before the control-register
write. This is consistent with context restoration, but no execution or call-graph
claim is made. In any case, such writes cannot override hardware-fixed rounding
bits. No startup-initialization inference is needed for the conversion conclusion.

## Native host implementation contract

Given the already-materialized pre-conversion float `p`:

1. Preserve the signed result. For finite binary32 input in
   `[-2147483648, 2147483648)`, `static_cast<std::int32_t>(p)` is the appropriate
   conversion. Its truncation does not depend on the host floating-point rounding
   setting. [C++ draft, conv.fpint](https://eel.is/c++draft/conv.fpint).
2. Do not cast first and clamp afterward. C++ conversion is undefined when its
   truncated result cannot fit. An explicit pre-cast exponent/sign classification
   can implement the source's saturation and avoids treating an IEEE NaN payload
   as an ordinary arithmetic operand. On an IEEE-binary32 host, inspect raw bits
   without altering them: exponent above `0x9D` selects the signed endpoint;
   otherwise the cast has a representable result. This is a source-derived native
   CPU conversion contract, not a request to run an instruction interpreter.
3. If instead admitting only finite bounded inputs, record that narrower domain
   explicitly. Rejection is a host safety boundary, not original saturation.
4. Keep `MUL.S` and `CVT.W.S` conceptually separate. A `volatile float` materializes
   a host binary32 product but does not itself establish EE multiplication parity.
   Exact arithmetic equivalence for all speed/delta inputs remains outside this
   conversion finding; do not silently describe default host nearest-even
   multiplication as proven EE behavior.
5. The later source `ADDU`/`SUBU` instructions retain the low 32-bit result. A
   fully general host reconstruction must avoid C++ signed-overflow undefined
   behavior, or document a source-backed bounded geometry domain.

## Audit scope

`original-bytes.txt` records the ELF mapping, source hash, aperture words, and
control-register-write candidate contexts obtained using read-only file tools.
No source comments were used as hardware proof. No emulator behavior or test
result was used. Existing text/overlay helpers with nearest-even behavior were
not changed; auditing those separate consumers is outside this assignment.
