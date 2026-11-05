# Water RGB integers and following scalar getters

This continues the reviewed [Water scalar tail](water-opacity-subtract-scalar-tail-20261105.md) from2B9528. Six original158B30 calls load environmental and froth RGB words, followed by five original158B80 calls. The same stack Config and selected General Information section remain authoritative. The next frontier is2B9670 before RenderInfinitePlane's integer getter. The packet is source-only and unpublished.

## Integer getter158B30 is not atoi

158B30 saves its actual destination/default and calls158920. That helper calls158240 into its own1024-byte stack string. Missing lookup returns0 without writing the integer; only the outer158B60 then stores the caller default80 and returns0.

Found strings follow ordered tests:309538 length-one comparisons with `T` and `F`; full308F00 comparisons with `ON` and `OFF`; then length-one `Y` and `N`. These are not full-string true/false comparisons. If none matches,30AE70 calls30AC38 with base0 and a real local end pointer, returning a signed64-bit result. Source158A3C/40 takes its low32 and sign-extends it;158A44 stores that low32 word into the original destination. The outer getter returns1. A malformed or unsupported found string is not missing and cannot receive the caller default.

This initial implementation deliberately admits only optional plus/minus followed by either single0 or one through nine decimal digits beginning1..9, then the known terminating NUL. Magnitude is at most999999999. Whitespace, fractions, exponents, suffixes, multicharacter leading zeros, hexadecimal, booleans, empty strings and larger numbers stop at158950 before any source boolean probe is projected. They are native refusals, not source failures/defaults/zeros. This is a narrower language than the original conversion.

For admitted numeric strings all six boolean tests necessarily mismatch on the first character, but their original reads still occur. Each309538 length-one call reaches its first pair of ctype reads309580/584 and its final unsigned-byte pair3095F0/F4. ON/OFF use the existing same-process308F00 implementation and its actual first-mismatch reads. No host locale or detached comparison receipt is substituted.

## Numeric dependency ownership and conversion

The actual current process reentrancy pointer is read at30AE84 and captured as the typed38544C identity, requiring the existing closed reent385160 owner. It is required even though the admitted subset cannot overflow. Initial30AC94 classifies the first character for whitespace, then each digit and the final NUL is classified at30ADC0. New process-owned knownness explicitly covers the reached full3C6391 bytes, not merely the prior fold consumer's bit0: sign bytes are10, digits04, and NUL20. Thus masks8,4 and3 follow their genuine source branches. This state is shared and poisoned by unjoined aliases rather than reset for each Config/getter.

Literal0, including+0 and-0, genuinely chooses base8. All other admitted tokens choose base10. The signed64 magnitude limit is7FFFFFFFFFFFFFFF for positive and8000000000000000 for negative. Original unsigned remainder30F260 and quotient30EC90 derive cutlim and cutoff. Their reached normalization paths read separate immutable tables3C70C0 and3C6FC0 at offsetbase; entries8 and10 are4 in both tables. A separate process-owned provenance flag guards each reached table read before the native arithmetic result is established. This is not inferred from the float parser's different dispatch/power-table state.

Every admitted digit is below the selected base and every intermediate accumulator remains at most999999999, far below the signed64 cutoff. The pure30EC30 multiply and following DADDU can therefore be projected with bounded unsigned64 arithmetic without invoking the overflow/errno34 path. Existing errno is not cleared or manufactured. Negation uses modulo64 arithmetic; negative zero remains0. The local end pointer is retained as the semantic offset of the known NUL, never invented EE pointer bits. The64-bit result and low32 word are both retained; within this subset no signed32 value is lost. No0..255 clamp or boolean normalization is added to the RGB words.

## Shared lookup code without shared results

The private FieldLookup base now contains only158240's row/token/scratch/output state. FloatGetter and IntegerGetter each own a separate base instance per actual source call. Sharing implementation does not reuse another field's found flag, value, cursor or buffer. Existing float lookup/tokenization, partial scratch writes, retained tails within a row loop, quote handling, source order and missing behavior are unchanged.

The six integer calls are ordered after the existing WakeScale return and before any following float call. Native preflight checks both sides of that transition. Each integer retains probe progress, current reent, classification progress, cutoff-table reach, magnitude, end-pointer offset, inner158920 return and outer158B30 return. Engaged false means a genuine completed missing getter; an unengaged return means a native stop and cannot advance the caller.

## Actual stores and following float fields

All six integer calls use default80 and directly write the same App process words:

-2B9528 EnvColorRed ->443EA0.
-2B9544 EnvColorGreen ->443EA4.
-2B9560 EnvColorBlue ->443EA8.
-2B957C FrothColorRed ->443EB0.
-2B9598 FrothColorGreen ->443EB4.
-2B95B4 FrothColorBlue ->443EB8.

The next five float calls use the existing exact bounded decimal/soft-float conversion, without changing its syntax or numeric contract:

-2B95D8 PolygonScale ->383550, default bits3F7FF972.
-2B95F8 AdditionalFov ->383508, default40800000 (4).
-2B9618 PoolDepth ->443B98, default42C80000 (100).
-2B9634 WaterBaseSwellHeight ->443E80, default+0.
-2B9654 WaterBaseSwellPeriod ->443E88, default40A00000 (5).

The native App process-scalar owner gains exactly these eleven word lanes and independent publication versions. Actual getter stores write those destinations directly. A receipt is recorded immediately afterward, with no fallible operation in between. Earlier process values and already-written fields survive a later native refusal. A later joined same-process overwrite may supersede a current value without erasing historical receipts. Unknown process aliases prevent new admitted stores and do not become invented known values.

The six RGB writes do not establish alpha443EAC or443EBC. No alpha lane is initialized or copied here, and no claim is made that the original global alpha bytes were globally uninitialized. Their independent source provenance remains required at later2B9768/2B9798 reads. Later full-vector443EC0/443ED0 loads also remain distinct from existing scalar receipts.

## Stops, withdrawal and integration

Lookup/conversion refusal retains every earlier source effect. Missing lookup may default only at its genuine source store. A process-admission refusal stops before the corresponding getter body without an output or return. Native file/Track withdrawal and provider retirement preserve source history and all process publication versions; no source destructor, free, global reset or loader completion is inferred.

The predecessor is the accepted but unpublished scalar-tail packet, patch SHA256225252d8c5f849143e226a318786a5ae59a7291846618a656092ec6ca1a89135. Its unchanged Config implementation/process header are inherited from the accepted first-getter packet; exact hashes are bound in the manifest. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. No blocked publication was retried. The process-globals header now also adds integer ctype and cutoff-table provenance, so integration must narrowly rebase all Water additions while preserving the pending vectors checkpoint's changes. Never overwrite that checkpoint's owner changes.

Original ELF SLUS_202.68 SHA256 is c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Frozen evidence covers158B30/158920, both comparisons,30AE70/30AC38, full integer arithmetic helpers, actual ctype/normalization bytes, Water call order and literals. Original allocator uncertainty remains: earlier allocations are success-only native storage projections, not EE heap/free-list/address/errno/availability receipts.

Validation is static source/ELF reading, hashing, differences and strict text-patch reconstruction only. No build, compilation, syntax/type check, tests, probes, helper/model/target execution, emulation, runtime or publication occurred. No gameplay or GPU readiness is implied.
