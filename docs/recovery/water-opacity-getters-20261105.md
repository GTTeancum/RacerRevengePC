# Water opacity getters up to the original subtraction

This continues the reviewed [first three Water float getters](water-float-getters-20261105.md) in the same real2B9300 loader and stack Config. It implements two further158B80 calls, retains their actual destinations and subsequent loads, and stops at2B9428 before the original `sub.s f0,f0,f1` instruction. There is no computed opacity difference or completed Water load.

## Source order and destinations

The fourth call at2B93D8 reads the actual3C0808 `BaseOpacity` key. Its saved default is42000000 (32), and its JAL delay slot2B93DC supplies the actual Water+60 destination. This field is unknown before the getter writes it; the Water constructor did not initialize+60. The native Water owner now retains its typed scalar directly, rather than adopting a detached Config result.

The fifth call at2B93F4 reads3C0818 `MaxOpacity` with default42800000 (64). Its delay slot2B93F8 supplies the actual loader stack slotsp+A40. That caller-owned optional scalar is the getter's direct output destination and remains distinct from Config history and process globals. The caller ignores both getters' true/false return values. A genuine missing key writes its saved default at158BDC and returns0; a found supported value writes at158BC4 and returns1. Native refusal does neither.

After both calls return,2B93FC loads Water+60 and2B9408 loads stack+A40. Their words are retained separately. Source2B9414 stores the first Water+60 load to the shared App process lane443ED4. This store occurs before2B9418 reloads Water+60 into the second subtraction operand. The implementation retains both reads rather than substituting one detached value for the other. These moves and stores do not perform floating arithmetic.

The App process-scalar owner gains one lane for443ED4 and a native publication version. It is not reset on Water/World withdrawal. Admission refuses an unknown/unavailable process owner or exhausted native publication version before the2B9414 store. In that case both getter writes and the preceding loads remain intact, but the reload and subsequent instructions are unreached. Later same-process overwrites can supersede the value without erasing this occurrence's historical publication receipt.

## Explicit float bounds

Both new getter occurrences use the same reviewed text lookup, byte knownness, local tokenizer, original process folding, and bounded302E10/310440 projection as their predecessor. They do not import values from a test fixture or a separate parsed configuration.

Admitted found strings contain an optional plus/minus, at least one integer digit, an optional dot followed by at least one fractional digit, at most nine total digits and six fractional digits, then the known terminating NUL. No whitespace, exponent, suffix, empty value or longer decimal is admitted. Unsupported found strings stop at the real conversion frontier; they never become missing/default results.

For this domain the source309C18 fast branch is allocation/errno-free, captures the same process reentrancy owner, and checks the actually reached dispatch/power-table provenance. The accumulator is at most999999999; the denominator is a power of ten no greater than10^6. Every admitted nonzero value has magnitude between10^-6 and999999999, hence is normal binary64 and binary32. Original nearest-even binary64 division is followed by original sticky/ties-to-even binary32 narrowing, rather than direct rational-to-float or host floating conversion. Negative zero becomes positive zero through the original zero-minus-value sign path. Missing defaults32 and64 are exact normal binary32 words.

These operand bounds do not establish EE SUB.S semantics. Equal operands may cancel to zero; unequal operands may require exponent alignment, rounding, sign selection and exception/flag behavior not represented by the decimal converter. This packet deliberately stops before2B9428 without calling a host subtract, using IEEE assumptions, borrowing an unrelated arithmetic receipt, computing a difference, changing FCR31, or publishing443ED8 in2B9430's later delay slot. ChannelScale0 at2B942C has not been entered.

## Partial state and withdrawal

The Config retains five distinct getter stack occurrences. Completed missing getters still count as returned even when their optional bool value isfalse. A native stop in the fourth getter leaves Water+60 unknown; a stop in the fifth retains the fourth write but leaves stack+A40 unknown. A stop at2B9414 retains both outputs and first loads while leaving the process publication and reload unreached.

Preflights join each historical result to the actual live caller destination until provider retirement. After native retirement, source history remains without dereferencing freed provider backing. The existing Config withdrawal still releases numeric member/Track custody first; provider retirement then releases its counted entity and backing. No source Config destructor, source free, reset of process globals, loader return or World completion is inferred.

## Provenance and verification

The exact accepted predecessor is the isolated first-three-getter packet with patch SHA256 `603dddaab7c506f0ea6154c84f057700f3ef155537eb3e40343acfbf8848f301`. It and the section checkpoint remain unpublished at this packet's creation. Published main remains `e566993fc25f628ce5b4aa5def424c6003f53b75`; no blocked publication was retried here. The manifest binds exact predecessor afterimages and their hashes, not an invented published commit.

The inherited process-globals header also has an independently reviewed vectors change waiting in the blocked vectors/section checkpoint. If that checkpoint is applied first, the predecessor's Water knownness additions require a narrow reviewed rebase that preserves the vector changes. This opacity packet does not modify that header or resolve that integration qualification.

Original SLUS_202.68 SHA256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Static Water caller, literal and158B80 extracts accompany this packet; full original decimal/soft-float evidence is frozen in the predecessor. The original allocator remains a success-only native backing projection with EE heap/free-list/address/failure behavior unknown.

Only static source/ELF reads, hashes, diffs and text-patch reconstruction are permitted validation. No build, compilation, syntax/type check, tests, probes, helper/model/target execution, emulation or runtime were performed. No rendering, gameplay or GPU readiness is implied.
