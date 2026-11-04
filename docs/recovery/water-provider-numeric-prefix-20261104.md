# Water provider bounded numeric prefix

## Implemented source sequence

This packet continues the reviewed real provider occurrence at2B84C0. It reads its actual retained+384/+D8 words, performs exact signed CVT.S.W result packing for the authenticated small positive integers, then represents the two original divisions and stores in source order:

-2B84D4 converts C-1;2B84D8 computes -2/(C-1);2B84DC stores provider+38C.
-2B84F4 converts C-2;2B84F8 computes +2/(C-2);2B84FC stores provider+390.
-2B8504 reloads the actual+390 result;2B8508 toggles its sign;2B850C stores only the first word of same-App shared443EC0.
-2B8524 converts C+2;2B8528 stores only the first word of shared443ED0.
-2B8548/2B854C convert the actual integer17*B and17*B+2 operands.

It stops before DIV.S at2B8550. The general-ratio division, provider+498, subsequent pointer stores, derived constructor return, World400 publication and virtual+74 initialization are not represented. No native fence or scalar computation is source DMA/CSR/INTC/frame readiness.

## Numeric contract and evidence

The exact factory rows give A in{29,37,45,53,61,69,77,85}, B in{2,3,4}; actual row membership and caller inputs remain owned by the existing factory, not by this numeric leaf. C=3*A+1. Actual conversion inputs are34..258 and precisely representable as binary32. The integer-only helper packs positive words1..258 without host floating arithmetic or signed-conversion ambiguity; unsupported values leave output unchanged.

The division helper uses the already-published sourceParticleFogReciprocal50LA, verified against immutable a29a140 blob502fdb755afafdaef77df8574915a2dbe84ee1f9. The numerator+1 and+/-2 significands are all800000, so the pinned PS2Float soft-DIV25-step recurrence and normalization predicate are identical. Only exponent+1 and sign change. For admitted integral denominators86..255, reciprocal exponent119..121 and translated exponent120..122 stay normal. The adjustment is raw integer exponent arithmetic, not host multiplication or a fictitious source reciprocal event. No zero, denormal, exceptional, saturation or out-of-range case is admitted.

This is raw-result equivalence to the pinned scalar model GitHubProUser67/pcsx2@0392a64e992ac3fa5445dbe484933616cb204674 with CHECK_FPU_SOFT_DIVSQRT selected. It is not silicon parity, general IEEE division, another emulator configuration or live FCR31 ownership. Exact source files, Git identities, ELF spans and the full static induction proof are in evidence/numeric-model/NUMERIC-PROOF.md. No model/helper was executed to obtain result words.

## Flags are local effects, not a fabricated control register

The pinned CVT leaves FCR31 untouched. Each normal soft DIV clears O only and preserves U, D, I, C, all sticky flags and other bits. The U behavior follows the actual checkOverflowUnderflowSoft condition, not an assumed correction. NEG toggles the raw sign and clears O|U. Private history retains these clear masks and false local divide-zero assertions. It never turns false assertedD into a known-clear shared D/SD bit, never supplies a live FCR31 value, and never claims a comparison or flag-dependent branch. A future CFC1/flag consumer must join the actual shared owner and intervening writers first.

## Mutable process words and lifetime

One private same-App owner retains only the first words443EC0/443ED0, with independent native publication serials. Initial lanes are unknown rather than fabricated image/vector values. Successful original stores establish them. The other12 bytes of each vector remain unknown. Later initializer2B8A6C/2B8A84 writers must join this owner before admission; the second uses C rather than constructor C+2, and its later full16-byte loads cannot be satisfied by these scalar receipts.

Unknown future aliases must invalidate currentness. Provider/World retirement does not reset shared words or serials. Historical provider receipts remain valid after a later joined write, and their preflight requires live equality only for the newest known publication. Native withdrawal stops checking a freed provider's live App path but preserves the historical numeric/store sequence.

The new numeric preflight is a leaf below the existing member/count preflight. It authenticates each raw conversion, recomputed quotient, actual stored value, local effect mask, global publication and exact partial-failure PC. No replay or partial return is used. The old combined count-floor retirement remains unchanged.

## Baseline and limitations

Exact predecessor: reviewed /tmp/racer-water-process-20261104-2239 afterimages, built on a29a140 and reviewed factory/found/provider packets. Its shared process header also requires the reviewed AudioStr startup packet and exact21be5188 combined header. This numeric packet does not edit the Config process header, Stream files, World prefix TU, resolver or post-particle leaf. It changes three existing files and adds two leaves plus this document.

The full AudioStr source prerequisite is reviewed, but both startup entry points remain private/unissued and upstream Video still stops2F5884. No actual source initialization, invocation or runtime completion is asserted by this staged work.

Only static source/byte reading, hashes and diffs were used. No builds, compilation, syntax/type checks, tests, source/target execution, emulation or game/GL runtime occurred. Staging and independent static review are not integration, publication or runtime certification.
