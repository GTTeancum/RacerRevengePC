# Water topology successor: verified original sequence

This is newly recovered implementation evidence, not an implemented loader or
an allocator-return receipt. The uploaded topology-count boundary remains
before JAL `2BAB54` and delay `2BAB58`. The separate outgoing topology-backing
draft was not available and has not been assumed correct.

`tools/assets/trace_water_topology.py` reads the authorized installed ELF without
modifying it. It verifies the complete ELF SHA256 from the handoff, resolves
addresses through ELF program headers, and emits private evidence under a fresh
repository `evidence` directory. The first extraction is
`evidence/water-topology-trace-20261105/trace.json`, with source hashes and
per-range hashes. `trace.txt` contains the corresponding instruction listing.
The tool explicitly decodes EE LQ/SQ/POR because generic Capstone MIPS64 decoding
misidentifies these as modern MIPS/DSP instructions. Other MMI words remain
explicitly undecoded. Six tests cover address mapping, file-backed bounds,
truncation, machine/identity refusal, and the EE overrides.

## Actual sequence after the current boundary

1. `2BAB54/58` calls raw malloc with the signed word projection of the low32
   bits of `20*T`. `2BAB5C` stores its result at Data `+28`. There is no
   scalar-new zero-size coercion here.
2. `2BAB60..2BABC4` reads an initialization byte relative to GP and a distinct
   assertion-control word at `443FC0`. The source conditionally initializes
   these globals, then may call `133660` with the allocation result, filename
   and line99; its error arm prints and breaks. A native allocation succeeding
   does not establish those global values or the helper's source return.
3. `2BABC8..2BABE0` reloads the count and actual stored topology pointer and
   calls `306478` with zero and low32 `20*T`. The outer topology allocation
   therefore becomes zero-filled only here, not at allocation entry.
4. `2BABE4..2BABF0` performs a second raw malloc for low32 `4*T`, storing its
   result at Data `+20`. It is a different allocation from the topology records
   at `+28`, with no inferred zero fill.
5. `2BABFC..2BAC38` reads four-byte entries into that second allocation while
   signed `ordinal < word(T-1)`. It reads `T-1` entries for ordinary positive
   counts, not `T`. The last allocated word is not initialized by this loop.
   Exceptional/wrapped counts require explicit native admission, not a changed
   source predicate.
6. `2BAC44..2BAE0C` iterates topology records with a20-byte stride. Each record
   starts with an actual four-byte count read at `2BAC68`. Its count drives
   distinct allocations: raw `2*N` at `2BAC88` stored at record `+4`, raw `4*N`
   at `2BACB4` stored at `+8`, and array-new `48*N+16` at `2BACEC` followed by
   array construction `2E8990` with constructors/destructors `133F80/133FA0`.
   The returned constructed-array alias is stored at record `+10` (hex).
7. Each inner entry reads four bytes to the stack at `2BAD48`, then loads the
   signed low halfword and stores it in the `+4` array. A12-byte scalar-new and
   optional `299790` construction produce the object pointer stored in the
   `+8` array. The source then calls `2998D0` on that object and the same file
   handle. The four-byte input must not be silently replaced by a two-byte read.
8. The real outer loop uses BNEL `2BAE08` with SLL in its likely delay slot.
   The five saved registers are restored only at `2BAE10..20`; JR `2BAE24`
   releases the96-byte frame in delay `2BAE28`.

The incoming loader flag is not visibly consumed in this extracted function.
That observation does not erase the caller's authenticated flag provenance.

## Integration work required

The next implementation should extend the existing explicit native-success
backing adaptation, preserving the independent original allocator frontier.
Data needs separate stable owners and semantic aliases for `+28` topology and
`+20` indices; host addresses must not be serialized as guest pointer words.
The current Data byte validator treats both lanes as constructor-written zero,
so alias publication and validation must change together. Initialization at
`306478` must establish known bytes only after its admitted execution.

Every allocation, copied prefix, source file return and inner object needs
distinct custody and partial-failure retirement. Native out-of-memory cannot
become an invented original NULL return. Data retirement must withdraw all
descendant aliases and release their owners before its current reference and
payload retirement. The original allocator's heap state and assertion globals
remain independent source prerequisites; neither host allocation nor this
static trace supplies them.

Before full-loader claims, the positive material loop for counts above one,
the first material's nonzero texture arm, the topology inner object loader,
subsequent caller work and final file/resource retirement must also be joined.
The successful native GA race-start test uses the existing gameplay path and
does not prove completion of this canonical source-owned Water path.

## First allocation implementation

allocateWaterPoolTopologyNativeOnly(nativeSuccessBackingOnly) now continues
an authenticated topology-count history through a separate raw-malloc prefix
and native success backing. The predecessor history stays at2BAB54; the new
allocator record executes its call/delay and retains the reached reentrancy or
arena-load boundary. It never records a successful original malloc return.
The prefix uses the same verified raw allocator arithmetic and small/large-bin
selection as the material-array adaptation, with its own fresh reentrancy
capture and topology-specific caller return at2BAB5C.

Native backing separately rechecks reentrancy and admits count/byte/request
capacity. It owns exactly20*T uninitialized bytes, not the normalized heap
chunk. Zero remains malloc(0); native NULL refuses the adaptation without
inventing an original failure or coercing the request to one byte. There is no
zero fill, index array, nested topology allocation or source file read yet.

Data now owns distinct topology storage and a semantic +28 alias. Publishing
that alias supersedes the constructor's known-zero lane without writing host
pointer bits into guest bytes. Data history checks allocation/alias/lifetime
state, including partial native refusals. Native terminal retirement requires
the existing closed file and discharged borrowers, revokes the alias, and
releases topology backing before material/Data cleanup. It does not claim
original free/destructor execution or restore source bytes.

The continuation stops before the assertion initialization byte read at2BAB60.
Original assertion-state custody is the next prerequisite. This API remains an
explicit source-owned adaptation with no automatic production caller. It does
not establish a complete loader return or runtime Water behavior.

Validation so far: the containing World translation unit compiled with MSVC
before the final extraction of the request-admission helper. The final helper
compiled with /W4 /WX and passed zero, all16384 supported positive counts,
exact byte limits, negative/wrapped source words and sum-overflow cases. Logs
are in build/water-topology-request-test. Full integrated build26 is running;
its receipt is authoritative. Runtime execution of the newly admitted
allocation/retirement branch has not been established.

Full build26 subsequently compiled all614 units, linked and passed input-hash
checks. Runs19/20 verified the rebuilt native Main-to-GA startup path and clean
shutdown; that route does not invoke the new explicit Water backing API, so its
allocation/retirement runtime coverage remains unestablished. The next assertion
state investigation is retained in the private assertion-state.json beside the
trace: both globals lie in ELF zero-fill memory, the nonnull assertion helper
returns0 on either debug branch, and the superficially similar19B4BC writer is
to433FC0 rather than443FC0. No assertion continuation is implemented yet.

## Assertion and zero-fill continuation

The authenticated native topology backing now has a separate one-shot
initializeWaterPoolTopologyNativeOnly continuation. Its allocation record stays
at its previous2BAB60 boundary; initialization retains a distinct result through
2BABE4, before the index-array count load and malloc. The topology-count and
original raw-allocator histories are not rewritten.

OriginalApplicationEntities owns the distinct startup-zero topology pair
3D670C/443FC0, independently of3D66F0/443EE8. World cancellation does not reset
it; the explicit invalidator poisons knownness for unjoined writers. Only the
joined0/0 and1/1 domain is admitted. The first initializer stores assertion1
then guard1; later initializers preserve both. Its authenticated nonnull native
allocation takes the133660 semantic return0 common to either debug36C100 branch.
No debug value or guest pointer bits are fabricated.

The native306478 equivalent zeroes exactly20*T bytes of owned topology backing,
including zero length and every alignment. The original helper's SQ/SD/SB loops
and return were inspected; private range words/hash are in memset-state.json
beside the earlier trace. No normalized heap padding, index allocation or other
Data lanes are initialized. A native preflight re-reads the current known Data
count and authenticates the same +28 role; no admitted intervening writer exists
between that check and the source-equivalent initialization effects.

Data preflight now validates retained initialization results and, until later
source writes are implemented, checks every live initialized byte remains zero.
Retirement retains the result without dereferencing freed payload. Unknown or
unsupported global state refuses before writes and remains a valid partial
history for native cleanup. Repeated calls cannot initialize the same backing
again. No production invocation or full Water loader return is claimed.

water_topology_initialization_test.cpp passes MSVC /W4 /WX after correcting
signed test literals:96 extent/alignment cases, including the maximum admitted
count; repeated state; canaries; zero length; wrong extent/null/wrapped requests;
and unknown/mismatched global refusal without mutations. The actual containing
World translation unit also compiled successfully. Full build28 is running;
its final receipt must be checked before any runtime validation or publication.

Build28 is now complete:614 units compiled, linked and input hashes remained
stable. The final executable SHA256 is
CE8F958F40DF54A710108B1455CBFAEEE488D2D9F305E50458B9006E918C801C.
The final input hashes for the initialization helper, aggregate continuation
and Application owner were rechecked. This proves integrated build readiness,
not runtime invocation of the new initialization API. A redundant GA route was
not used as Water validation. The next index allocation/read work must preserve
unknown raw bytes, T-1 iterations, distinct read observations and the last actual
cursor on failure; it must not replace the existing topology-count receipt.
