# Genuine resolver scratch producer and132BD0 reset

Source-only, base6dea53b. No runtime wiring, tests, syntax checks, build or guest
execution. Static words are retained in rr-clock-full-static-fields.txt from the
authenticated SLUS_202.68. No project asset decoder was executed.

## Actual producer, not inferred state

Application13FB4C allocates178,13FB5C calls12C820, and13FB6C publishes the SAME
resolver into global3E1A70. Constructor12C920..958 initializes EIGHT slots at
88,A0,B8,D0,E8,100,118,130: null pointer,64-bit status-2, signed word-0x6000.
These are explicit source stores, not allocator-zero assumptions. The private
native facet stores them only when its explicit producer is invoked; ordinary
resolver construction/file readiness does not manufacture initialized scratch.

12C820 then calls12CBA0(1,10,0x100000). Its12CDF0..CE3C suffix attempts eight
64-aligned0x6000 allocations in ascending order. Each success stores status-1,
each failure-2, every slot's offset10=-0x6000. The last successful ordinal is
stored148 at12CE70.12CE40..68 requires at least two successes. Native failure
retains all successful backing and real failed-slot state for safe retirement;
no rollback or blanket successful initialization is claimed. The private method
implements only this scratch suffix, not12CBA0 file/index/list setup.

## Same-owner implementation

OriginalStandardTextureResolver50LA now privately owns Scratch, with eight real
native aligned CPU allocations. No guest address, guest allocator or PS2 service
is emulated. initializeScratch12CDF0 and restoreScratch132BD0 are private to the
actual Application; no runtime call or public status setter was added. Existing
vector-backed source readers do not use this facet yet, so it cannot certify
that source read/caching operations occurred.

restoreScratch132BD0 examines actual stored statuses in order. Non-(-2) slots
retain their pointer/status/offset. A -2 slot must own no buffer; it attempts the
same aligned native allocation and writes -1/-2 plus-0x6000. selected148 is
untouched. All eight attempts run even if one allocation fails. The native
return/diagnostic exposes unsupported allocation completion while retaining
partial effects; it is not a fabricated original return-register value.
Corrupt pointer/status ownership rejects before mutation instead of reproducing
the original assertion machinery. Diagnostics mark committed effects.

Scratch destruction implements the concrete scratch-only12CB4C..84 suffix:
ascending free of nonnull backing, null pointer, status-2 and offset-0x6000.
It does not claim the surrounding resolver12CA10 file/list destructor. No scratch
borrow escapes, so current native retirement has no independent lease hazard.
Future consumers must retain this SAME resolver and establish borrow protection
before any destructive slot operation is exposed.

## Actual next join

Race load1A7770 calls132A40(resolver,1). That function releases selected148,
classifies slot file IDs through actual resolver150 file rows/flags18 bit2,
invalidates reusable statuses to-1, then frees excess idle buffers and sets-2.
The current native resolver's positive file IDs/cache pointers are not proven
identical to these signed slot IDs or file-row flags. This eviction producer is
therefore NOT replaced by a guessed seven-buffer free. It remains explicit work.
132BD0 can now operate on genuinely constructed native slot state, including
actual allocation failures, but a complete race teardown call requires that
preceding real lifetime/read association.

The facet is compiled through the existing actual resolver .cpp. It is dormant,
adds no automatic allocations or steady-state overhead, and does not resolve
remaining Video44/renderer2C/backend reset or full race owner ordering gates.
