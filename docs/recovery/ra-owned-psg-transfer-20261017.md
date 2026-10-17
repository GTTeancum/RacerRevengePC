# Transfer RA-authenticated PSG storage into the native resource

UNTESTED. No builds, tests, syntax checks, smoke or benchmarks executed.

The authenticated RA world extracts each original PSG into a local vector and
performs all existing source/provider and wide-scene identity checks before
preparing its resource. That local is not used afterward. A new private rvalue
prepareOriginalLimits overload lets the friend RA owner transfer it rather than
copying the buffer into Storage again.

Both private overloads use one forwarding-reference implementation. Diagnostic
reset, alreadyPrepared and input checks remain before transfer. Storage creation
and member-name copy still precede byte assignment inside the existing try.
The public/default prepare and existing const-input overload forward a const
lvalue, so their copy and caught-allocation behavior remain unchanged. Do not
replace this with a by-value parameter: that could copy/throw outside the guard.

All hierarchy and scene decoding already reads next->bytes. Views are created
after ownership transfer and remain valid when the unique_ptr is published last.
The private RA input may be consumed on a later decode failure; its caller returns
without reuse. The old public input remains untouched on failure. No provider,
identity, hierarchy, count or decoder-limit check is removed.

Original byte size/content and decoded results are preserved. Buffer capacity,
allocation identity and bad_alloc opportunities may differ on the new owned path.
Root source inspection found downstream model provenance uses byte equality and
stable owned storage, not a required allocation address/capacity. Rebuild native
consumers; no external binary compatibility claim is made.

The complete RA route requires 710 retained payloads; this removes one explicit
retained-byte copy for each successful resource. Actual bytes, memory peak and
load-time improvement have not been measured. Two independent design reviews
accepted it. Successor should validate public copy/failure behavior, private
consumed-input failures, exact original/decoded bytes and cold-world teardown
before measuring load time separately from track FPS.
