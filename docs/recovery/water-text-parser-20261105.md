# Water text Config parser157500

This continues the distinct Water stack+0x240 Config after its retained same-member seek0. It supersedes the text-parser boundary in [the earlier Config-open packet](water-config-open-20261104.md). It does not reuse either Particle Config, a prepared parser, a supplied file receipt, or a caller-selected section index.

## Source continuation

- The private Water Config owns the actual parser call and persistent input, split and section-name buffers. Unknown bytes remain unknown. The existing pure split159440, section-name157548, row-name15761C, row-finalization157654 and last-row157770 helpers perform only their bounded source slices.
- Each12EE90 line call reaches the existing process-shared handle assertions. Remaining length uses low32 subtraction and signed-positive interpretation. Zero or high-bit remaining writes only input[0]=0 and returns the EOF arm without device activation or member cursor movement.
- Positive calls use this Config's same numeric member through the existing130560 archive adapter without member-cursor advance. Exactly the copied prefix becomes known, including when that adapter subsequently refuses. Earlier known suffix bytes and shared device/scratch/window mutations survive. Native refusal is not a completed source zero return.
- Only an admitted full positive read reaches12F1B4. The suffix terminates at the requested amount, handles LF and CRLF, and advances the same member by the source consumed count. Embedded NUL without LF still consumes the requested count. The existing adapter does not certify arbitrary original short/error returns or source-false device activation, so those outcomes are not invented here.
- A section name ends at NUL or the exact literal delimiters `]\n\r`; a closing bracket is not required. The157AF0 text lookup scans only actual retained rows and uses Water's existing same-process308F00 ASCII ownership, including final-return folds. Unknown or non-ASCII provenance remains a stop, not a fresh locale or independent ctype authority.
- New sections retain source allocation order, copy the name before the real12F520 tell, sign-extend that member's low32 cursor into the row's64-bit position, store the line ordinal, finalize the previous row, and then increment count. Refused tell retains the already-copied name. Non-section lines increment the ordinal without creating rows. EOF finalizes only the last existing row.
- Duplicate names stop at1576D0 before the actual1336C0 warning. No diagnostic, lazy assertion initialization, assertion return, duplicate skip or parser return is fabricated.
- A completed parser returns to157AC4, writes field428=0 and reaches Config's actual return1, even if no section rows exist. Water then reaches2B9354 before its distinct157D00("General Information") call. The selection, selected child, getters, Water loader return, base return and World return are still unimplemented.

## Allocation adaptation and original uncertainty

Source first allocation is1575E0 ->305700 ->305990 for0x28F0 bytes. Growth is1575CC ->3080C0 ->3080F0 for the low32 capacity*0x418 product. Growth publishes capacity18 before the call; initial capacity10 is published only after the allocator's returned pointer. Both wrappers read the same process reentrancy identity38544C.

The Water owner admits only a success-side native storage projection: exact requested-size raw backing plus a private knownness sidecar. It checks the current shared reentrancy owner, prepares and copies only known sidecar values before realloc, and completes pointer/capacity publication without fallible work after a successful native allocation. No old raw backing alias escapes. The final12 bytes of each original0x418 row and untouched name tails are not initialized.

This is not execution or proof of the EE allocator. Its arena385468, heap headers/free lists, sbrk state, fragmentation, returned addresses and errno are not represented. Host allocation success provides native storage for the admitted success path only. Host refusal is a native stop before a source allocator result: pending request, old pointer, existing rows, and the already-published growth capacity remain intact. Initial refusal leaves capacity0. It never becomes a fabricated sourceNULL, source errno, orphan-pointer overwrite, clean parse failure or source destructor. The older generic helper's nullable allocation adaptation is deliberately not called by Water.

The reused preparation helper bounds negative source counts/capacities and capacity-product overflow as native limits. The original arithmetic can wrap; this packet does not pretend the original code rejected those values. Native raw byte sizes are the low32 source requests, not an EE address or heap-layout simulation.

## Custody and cancellation

The Config still exclusively owns the open numeric row and Track/provider pins. Private phase/history preflights accept committed partial reads, allocation refusal, copied names before tell, completed rows, EOF, duplicate stop and successful-open frontier. They do not turn those states into external resumable receipts.

Native cancellation withdraws the numeric row and pins without calling source1573B0 or resetting source fields. The retained Config keeps its private table storage and knownness until native owner destruction, so later preflight can inspect the history safely. Table destruction frees native backing only; no source table destructor/free completion is claimed. No external raw row alias or selected child is published.

## Static evidence and verification limits

Evidence comes from the original SLUS_202.68 image, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`, parser157500, line12EE90, tell12F520, allocator wrappers/cores, Config return and Water caller disassembly. Source preimages and unchanged integration dependencies were matched to published main `8a876a2bc85ab4a6fd98789ccf72e63112f9c368`.

Only source/ELF text review, byte/hash identity work, and patch assembly were performed. No compilation, build, syntax/type check, tests, model/target execution, emulation, game runtime, GPU/hardware check or gameplay validation was performed. This private source-only continuation does not imply rendering, frame completion or GPU readiness.
