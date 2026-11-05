# Water General Information selection

This continues [Water's reviewed text-parser path](water-text-parser-20261105.md) from2B9354 into the distinct first Config157D00 call, with the actual literal3C07A0 `General Information`. It does not reuse Particle selection state, accept an external section result, or parse a detached file.

## Source behavior

The already-open text Config has opened0C=1 and mode10=0, so the closed-file assertion and flush branches are genuinely skipped. Its existing157AF0 lookup scans the retained section-index table under the same process-owned308F00 folding lane. A missing section returns0 without writing name28, selected20 or child24. Water ignores this return and still reaches its first getter.

For a found row, selection loads its actual64-bit position and seeks the same numeric member at157F88. It never truncates a negative sign-extended tell into a plausible host offset. The fresh NULL child takes157F94/98 and skips both source frees and the selected20 reset. The row's actual line08 supplies the signed line span. The literal's nineteen bytes are copied into name28 at15808C and followed by NUL at158098; the unwritten tail remains unknown.

The first pass executes exactly the signed-positive span count of12EE90 reads and159440 splits. Both passes ignore the original reader return value, so an authentic EOF still reaches split on the known NUL byte. Only nonempty split lines whose first byte is not `;` increment selected20 at1580EC. A native read refusal is not EOF and does not continue the split.

The second real seek at158130 returns to the same retained position. The source then requests selected20*4 bytes for child24 and selected20*1024 bytes for contiguous line storage. It publishes child24 after the first allocator return, stores data as pointer[0] unconditionally, and forms subsequent pointers by adding1024. The second pass rereads and splits the same signed span, copying full nonempty/noncomment split lines including NUL into successive1024-byte destinations. Keys and values are not parsed at this stage. Knownness records only bytes actually copied; raw data tails stay uninitialized.

After the second pass, source158214 stores field428=0 and selection returns1. The World caller now stops at2B9370 before158B80("WaterBaseHeight", Water+3C,20.0f). Missing and found sections reach this same next call. No getter result, WaterBaseHeight store, process-global water assignment, Water loader return, base return or World return is claimed.

## Empty sections and allocation limits

An existing section with zero selected lines is not the missing-section branch. It still executes both seeks and both zero-size allocations, then the unconditional pointer[0] store, before returning1.

Original305990 turns a zero request into a16-byte chunk and successful return is chunk+8, leaving at least8 payload bytes. The new success-only native projection therefore preserves the exact source requested0 while giving each raw allocation at least8 native bytes. This supports the first4-byte pointer lane without pretending selected20 became1. Semantic offsets and the retained data-owner identity represent source pointer values; no invented EE addresses are written into raw32-bit pointer slots. Unwritten additional pointer bytes remain unknown.

Each allocation independently captures the same current process reentrancy owner before obtaining native backing. Sidecar and host failures are native stops before the corresponding source allocator result. Failure of the second native allocation retains the first published child24 and its uninitialized first pointer. No hostNULL becomes sourceNULL, errno, source orphan behavior or an allocation-failure return.

The native metadata aperture bounds counts above UINT32_MAX/1024 before the first source allocation is projected. Exact low32 request arithmetic and prior source selected20/seek effects remain retained; this is not a claim that original wrapping arithmetic rejects such input. Arena/free-list state, fragmentation, sbrk, original allocator addresses and actual EE success/failure remain unrepresented.

## Custody and shared primitives

The line operation now accepts a private caller-owned buffer and read record. Parser157500 still owns its original persistent stack buffers and call counters. Selection has a separate initially-unknown input/split pair reused across both passes, matching its one source stack occurrence. Both use the same numeric member, process assertion owners, partial-read knownness, requested-length suffix and cursor publication. The lookup primitive similarly takes the current private query instead of implicitly using the parser's section-name buffer.

Native phase preflights retain all partial counting, seek, allocation and second-pass state. The Config keeps section backing and sidecars under its own lifetime, including after native file/Track withdrawal. Native destruction releases storage without claiming source1573B0, child frees or source field resets. No raw child or row pointer is returned externally.

## Evidence and limits

The source trace covers157D00..158238, first getter158B80, source zero-size allocator paths and the original Water caller. Original SLUS_202.68 SHA256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. This isolated packet is based on the reviewed Water text-parser afterimages, now published at main `e566993fc25f628ce5b4aa5def424c6003f53b75`. All four preimages and unchanged integration dependencies match that verified commit.

Only static source/ELF reads, hashes, diffs and text-patch assembly were performed. There was no compilation, build, syntax/type check, test, probe, target/model execution, emulation or runtime. No GPU readiness, rendering or gameplay validation is implied. Positive float parsing at the next getter still requires the original token/decimal/software-float ownership and is deliberately not replaced with host atof/strtod.
