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
