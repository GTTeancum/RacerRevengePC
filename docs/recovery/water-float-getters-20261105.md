# Water's first three original float getters

This continues the reviewed [General Information selection](water-section-selection-20261105.md) in the same Water loader and stack Config. The three actual158B80 calls are WaterBaseHeight at2B9370, WorldWidthHeightInFeet at2B939C, and WaterHeightRange at2B93BC. Their default binary32 words are respectively41A00000 (20),45100000 (2304), and42200000 (40). The caller ignores each getter's true/false return.

## Lookup and exact source defaults

The text branch of158240 reads the retained selected20 count and child24 row identities, scans rows in source order, and takes the first matching key. Each invocation has private, initially-unknown1024-byte line, key and output buffers. Line and key scratch retain their known prefixes and old tails across rows within that invocation. The1023 copy cap is preceded by the real source strlen; unknown bytes are never silently treated as a terminator. The two impossible size==4 assertion branches are skipped because this occurrence's scratch size is1024.

The actual local29FF50/29FFA0 tokenizer receives delimiters `=\n` and then `\n`. It is not host strtok or a split-at-first-equals map. A delimiter is replaced by NUL before the next-byte read. Its repeated-delimiter membership loop includes the delimiter string's terminating NUL, so it can advance into retained or unknown tail bytes. Partial NUL writes and cursor movement survive refusal. A missing value skips the key comparison. The key comparison uses the same process-owned308F00 folding lane as section lookup, without host locale or whitespace normalization.

For a matching key the source removes at most one leading and one trailing quote, then strcpy-copies the resulting known string into the actual getter-local buffer. Even an empty matching string is source-found. Missing lookup writes a known empty output and returns0; only this authentic result reaches158BDC and writes the caller's saved default. A malformed value, unknown byte, unsupported decimal, exhausted native extent or unavailable process dependency never becomes a false lookup, default write or getter return.

## Bounded original decimal conversion

Found values enter a deliberately bounded native projection of302E10 ->30AB18 ->309C18 ->310440. Admission requires an optional plus/minus sign, at least one integer digit, optionally one dot followed by at least one fractional digit, at most nine total digits and at most six fractional digits, followed immediately by the known NUL. Whitespace, exponents, suffixes, empty strings and longer strings stop at302E10 before source conversion is claimed. This is not the full permissive original strtod language.

For admitted strings,30AB28 reads the same current process reentrancy pointer, whose typed identity must still be reent385160. An initial sign reaches the original dispatch table3C6670 at309C80. Nonzero values with an effective negative decimal exponent reach the binary64 power table3C6558 at30A1A8. Those tables share process knownness and the existing unknown-alias poison boundary. No new Config resets their provenance.

Nine total digits bound the original accumulator to999999999 and significant count to nine. Fractional trailing zeros do not enter the original accumulated significand/exponent. The bounded path uses exact3101E8 integer conversion, skips the ten-digit combining path, and either returns the exact integer or takes30A1A4's30FF18 division by an exact power of ten no larger than10^6. Neither source heap allocation, bigint conversion nor an errno write is reached. Integers used by the native rational projection and the binary64 denominator are exact. Integer long division rounds the rational to nearest-even binary64, matching the source normal division's quotient and remainder correction. Original310440 sticky reduction and310498 ties-to-even normal narrowing then produce the binary32 word. No host atof/strtod, host floating arithmetic, floating environment or execution probe is used.

All admitted nonzero values lie between10^-6 and999999999, hence stay normal in binary64 and binary32. Negative sign is the source30FC08 subtraction from positive zero, not unconditional sign-bit insertion: authored negative zero becomes positive zero. Conversion history retains numerator, denominator, fractional count, captured reent, reached table dependencies and the resulting binary64/binary32 words. The native integer projection does not claim to materialize every dead source register or stack temporary.

## Real destinations and publication order

The first getter writes the actual retained Water owner's optional+3C scalar at158BC4 for found values or158BDC for missing values. This field is unknown before that write; the constructor never initialized+3C. It is not a detached Config output adopted later.

Water2B9380 loads that just-written field. The second getter JAL's delay slot2B93A0 then publishes the loaded word to process443E90 before the second getter body. A later refusal in the second getter retains that publication. The second and third getters write process443EE0 and443B90 directly at their respective158BC4/158BDC stores. The App's existing process-scalar owner now retains these three additional lanes alongside443EC0/443ED0. Each lane has a native version receipt to distinguish historical publications from later same-process overwrites. Unknown process aliases prevent new admitted stores; history is not current-state authority.

Partial lookup/conversion state, the Water+3C field, the forwarded read and already-published process words are not rolled back on a native stop. Native withdrawal releases the Config's file/Track borrow and then provider custody through the existing order. Process scalar publications survive provider/World retirement. The Config's history/backing remains retained without claiming the source destructor or free paths executed.

After the third call returns, the continuation stops at2B93D8, before158B80("BaseOpacity", Water+60,32). No fourth getter, Water loader/base/factory return or World completion is claimed. Later opacity subtraction and subsequent scalar/integer/string getters remain separate work.

## Evidence and boundaries

Original SLUS_202.68 SHA256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. The isolated packet includes static disassembly of Water's call sequence,158240,29FF50/29FFA0,158B80,302E10/30AB18/309C18, integer conversion, division, binary64 and binary32 packing, plus literal and power-table bytes. The reviewed General Information packet supplies exact preimages for its modified Water Config files; unchanged published dependencies come from verified main e566993fc25f628ce5b4aa5def424c6003f53b75.

The preceding parser/section allocations remain success-only native backing projections. Original EE heap state, free lists, addresses, failure/errno branches and availability remain unrepresented. This getter continuation does not remove that uncertainty or imply the original heap has enough storage.

Validation is static source/ELF review, identity hashes, diffs and strict text-patch reconstruction only. No build, compilation, syntax/type check, tests, probes, helper/model/target execution, emulation or runtime were performed. No rendering, gameplay or GPU readiness is implied.
