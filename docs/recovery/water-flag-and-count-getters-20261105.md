# Water flag/count getters and genuine boolean branches

## Source-only scope and exact predecessor

This isolated packet extends the accepted but unpublished color/scalar packet at `/tmp/racer-water-colors-20261105-0058`. Its exact patch is `f0b9a2a1c4bcb89ec2fd446e9dcb8b5921ef88733a32f5d958a9c319dba51c20`, and final manifest is `ce17bc0c0097f441ccd0eac815bbf986062895c83a149b594dca00402f97cd93`. The dependency chain through scalar tail, opacity, float getters and section selection remains unpublished. Published main remains `e566993fc25f628ce5b4aa5def424c6003f53b75`.

No shared checkout/predecessor was edited. No compile, syntax/type check, build, test, probe, target/helper/model execution, emulation or runtime was performed. Static ELF reading, source inspection, hashing and strict textual patch reconstruction are the only validation here. This work does not establish renderer, GPU, gameplay or original Water loader readiness.

The inherited process-globals header carries earlier Water additions; they still require a narrow reviewed rebase preserving vector-owner changes if the paused vectors/section checkpoint is integrated first. This packet does not itself change that header or publish anything. Native allocations remain success-only backing adaptations. Original EE allocation address, heap/free-list, failure, errno and availability semantics are not established.

## Original call/store sequence

The actual same stack Config and General Information selection are reused, but each getter performs its own original158240 lookup and retains its own scratch/knownness history.

- JAL2B9670, delay2B9674: RenderInfinitePlane, default1, direct global3834E0.
- JAL2B9688, delay2B968C: EnableBaseDisplacement, default1, direct Water+49C.
- JAL2B96A0, delay2B96A4: LodBias, default3, actual loader stack+A44.
- LW2B96A8 reads that stack word once. SW2B96BC publishes to38360C; SW2B96C4 to383604; SW2B96CC to3835F8; SW2B96E0 to3835F0.
- The last store is the delay slot of JAL2B96DC, before NumSignatureWaves158B30 runs. NumSignatureWaves defaults0 into actual Water+6C.
- JAL2B96F4, delay2B96F8: NumPools, default0, direct Water+64.
- Caller setup reaches2B9714 and stops BEFORE ShaderName158AD0 or its2B9718 delay slot. No string-getter result, later NumZones, color/alpha copies, full-vector copies, new section, or loader completion is invented.

Water+6C is the same live lane written zero by constructor2B0924, represented alongside historical zeroWords index16. The constructor receipt stays zero after later getter overwrite. Supported constructor/initializer prefix does not establish+49C or+64, so those live optional words remain unknown until their actual getters write them. These are not a parallel successful result fixture.

Global lanes3834E0 and38360C/383604/3835F8/3835F0 share the actual App lifetime, unknown poison and publication counters already used by earlier Water globals. Each store is guarded separately before mutation. The four LodBias receipts retain the value of the actual LW, not an assumed default or reread. A guard refusal preserves earlier stores and stops at that exact SW. Later joined writers may supersede current values; historical receipts remain valid. Provider retirement does not reset shared globals or fabricate live object fields.

##158920 boolean and bounded numeric semantics

158B30 calls158920, which first calls158240. A missing key returns found0 from the inner getter without writing its supplied destination, then the outer158B60 stores that getter's original default. A found value never receives a default merely because native admission or a dependency is unsupported.

All original ASCII boolean branches are now admitted and executed in original order:

1.158950 calls309538 against `T`, n=1; equal stores1 at158968.
2.158978 calls309538 against `F`, n=1; equal stores0 at158990.
3.15899C calls308F00 against `ON`; full equality stores1 at1589B4.
4.1589C0 calls308F00 against `OFF`; full equality stores0 at1589D8.
5.1589E8 calls309538 against `Y`, n=1; equal stores1 at158A00.
6.158A10 calls309538 against `N`, n=1; equal stores0 at158A28.

Prefix branches intentionally accept any known field string whose first ASCII byte folds to T/F/Y/N. Their suffix is not inspected by the source comparisons. ON/OFF require complete case-insensitive equality, including terminator comparisons. This is not a generic full-word true/false parser. Native admission identifies a supported shape only; it neither supplies a source comparison result nor skips reads. Every reached comparator still reads the same shared3C6391 ASCII owner at the original read sites. Each309538 n=1 executes its first signed-byte folds and final unsigned-byte folds; full308F00 retains its existing loop/final folds. Partial probe histories are preserved if shared table access refuses.

Each successful boolean path stores its original0/1 value and returns found1, including false-valued branches. These paths never read numeric reentrancy, numeric ctype/division tables, produce a numeric end pointer, or claim a signed64 conversion. The same158920 routine also serves the preceding six RGB getters, so the genuine boolean branches apply there too. Numeric ctype/division-table knownness is distinct from the ASCII comparator knownness; the extension does not impose numeric-only dependencies on booleans.

If no boolean branch can match, the admitted numeric subset is unchanged: optional +/- followed by single0 or `[1-9][0-9]{0,8}`, then known NUL. Multiple leading zeroes, octal/hex extensions, whitespace, decimal points, exponents, suffixes, empty values and larger magnitudes are unsupported and stop at158950 before probes. The represented source path is signed64 base0: literal0 uses base8, other admitted strings use base10, with real current reent38544C, full-byte numeric ctype reads, both source division normalization tables, cutoff/cutlim, magnitude at most999999999, actual local end-pointer offset, negative signed64 bits, and low32 destination store158A44. No host atoi/strtol or float-to-int cast is used.

Per-getter state separates native boolean/numeric admission, entered/returned probes, matched branch/store PC, numeric intermediates, inner found status and outer return. Optional false and optional zero mean established source results, not absence. Preflight enforces source ordering across the intervening five floats, verifies actual destinations and exact partial frontiers, and keeps unexecuted later getters absent.

## Evidence

Original ELF `SLUS_202.68` SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; VA mapping is VA-0x100000+0x80. Frozen packet evidence includes exact caller and constructor words,158920/158B30,309538/308F00, integer conversion and tables, literals and provenance. Evidence was produced by byte-to-text inspection, not guest execution. The packet manifest and SOURCE-IDENTITIES retain exact preimages, inherited dependencies, SHA256 and Git blob identities. Independent review and fuzz0 text reconstruction do not substitute for compilation or runtime validation.
