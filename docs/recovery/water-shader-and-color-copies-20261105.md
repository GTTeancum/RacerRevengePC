# Water ShaderName, NumZones and current Env RGB copies

## Exact predecessor and source-only status

This isolated successor uses the independently accepted but unpublished flags packet `/tmp/racer-water-flags-20261105-0112`: patch `0888125dc636566491b713a4a7160525781cb8225f980d77bf6802e2e6e528a7`, final manifest `8bbd026d23943afcd1bbef6a316a9f361ee82936b9f984aab2bbe911e354b68a`. Its exact unpublished color/scalar/opacity/getter/section chain remains required. Published main remains `e566993fc25f628ce5b4aa5def424c6003f53b75`. The unchanged published members leaf was freshly retrieved at that immutable commit; its complete tool response is frozen, SHA256 of source `2499bc6af9eeb8675a9754ac2917062846d79fb5249b356c7d5fb778ab324d61`.

No checkout/predecessor edits or publication occurred. No compile, syntax/type check, build, test, probe, target/helper/model execution, emulation or runtime. Only static source/ELF inspection, metadata identities and strict text-patch reconstruction. No GPU, renderer, gameplay or loader readiness follows. Existing allocator backing remains success-only native adaptation, with original allocation addresses, heap/free-list/failure/errno/availability unestablished. Inherited process-globals Water additions still require a narrow reviewed rebase preserving vector-owner changes if the paused vectors/section checkpoint integrates first.

## Actual string destination and original return semantics

JAL2B9714 calls158AD0 with the same live Config at loader stack+240, key3C0A58 ShaderName, fallback3C0A68 WaterDefault and destination loader stack+140. Delay2B9718 sets limit=-1. This is a distinct caller stack buffer, not158920/158B80's private numeric scratch. The World occurrence owns its256-byte value/knownness facet; the Config borrows it only during this actual call. Historical copy receipts are not competing source buffers or guessed guest addresses.

158AD0 passes the actual destination into158240 at158AEC. Lookup/tokenizer/comparator behavior is unchanged. Found text strips the same optional quotes and invokes309230 at1588B4. Missing lookup first invokes309230 at1588E4 using empty literal3A6288, writing output[0]=NUL; only then does158AD0 call309230 at158B08 with WaterDefault. The wrapper returns found1 or found0 accordingly; caller does not branch on that status. A found empty string remains empty and returns found1. Unsupported lookup/copy knowledge never becomes missing, default, or successful output.

Original limit=-1 does not cap string length. Native admission requires content length<=255 so NUL fits before live Config+240. Length>=256 stops before the copy at1588B4, with no truncation, no fallback and no Config overlap. Destination suffix beyond the actual copied NUL remains unknown/unchanged.

## Conservative309230 read admission, without invented alignment

Original309230 chooses byte copying when either address is not8-aligned, LD8 when both are8-aligned but not both16-aligned, and LQ16 when both are16-aligned. Wide loads inspect a terminal block past NUL; only NUL-free whole blocks are stored, then the terminal block is copied bytewise through NUL. Therefore a known C-string prefix alone does not establish all original read obligations.

This packet deliberately does NOT infer actual guest SP congruence from host pointers or invent absolute stack addresses. Before entering309230, a native whole-copy guard requires a conservative union of all possible source read extents:

- Let loader SP be S. Destination D=S+0x140; found source A=S-0x30-0x10B0+0x890+valueBegin. Thus A-D=valueBegin-0x990, congruent to the final quote-adjusted valueBegin modulo16 for EVERY S. This relative congruence is established without asserting absolute stack alignment.
- If valueBegin is not divisible8, both addresses cannot be8-aligned, so the byte path is guaranteed and quantum1 suffices. If valueBegin is8mod16, they cannot both be16-aligned, so quantum8 covers byte/LD8. If valueBegin is0mod16, quantum16 covers all three paths. Found scratch requires `round_up(content_length+1,quantum)` known bytes from valueBegin. For quantum8/16 this may remain stricter than the actual unknown-alignment branch needs.
- The immutable source literals3A6288 and3C0A68 are each8-aligned but not16-aligned, so no destination congruence can select LQ16. Their required extent rounds up to8.
- Empty literal3A6288 has eight known zero bytes. WaterDefault3C0A68 has exact bytes `57 61 74 65 72 44 65 66 61 75 6C 74 00 00 00 00`. Bytes after NUL are original ELF data, not fabricated padding.

Unknown/out-of-extent lookahead stops at309230 BEFORE helper entry and BEFORE any copy output write. Lookup scratch changes and previous source effects remain. This is a stricter native admission boundary, not a claim original code checks bounds or that it would perform no earlier stores before a later unknown load. Admission metadata records the required union, not a dynamic source-read trace; extra bytes are never asserted to have been read on every alignment path.

After admission, all possible original alignment paths have the same byte effect and destination return: copy the known prefix including NUL, leave the suffix untouched. The bounded projection writes that exact effect directly to the actual caller stack facet, records only written bytes, and joins return309340. It does not claim an intermediate read/store PC for an unknown alignment path. There is no fallible native step inside the admitted copy. Independent copy histories preserve missing lookup's first NUL separately from the subsequent fallback overwrite.

The earlier numeric scratch-copy projection is unchanged. The new string copy does not borrow that projection as proof of aligned-read knownness. All required lookahead comes from the same live lookup scratch, including its actual quote/delimiter writes and retained known tails, or the frozen immutable literals.

## NumZones and first unsupported alpha load

After ShaderName returns, JAL2B972C/delay2B9730 calls original158B30 for NumZones, default16, actual Water+4A8. It has its own lookup/conversion state and requires ShaderName return. +4A8 is unknown before this getter; nearby constructor+4A4 zero does not initialize it. Genuine boolean branches and bounded signed64/base0/low32 numeric semantics are inherited unchanged, with no plausible-range clamp.

Then the actual caller reads current same-App process words separately and copies them into the same constructor-owned Water color lanes:

- LW2B9738 reads443EA0; SW2B9748 writes Water+2E0.
- LW2B9750 reads443EA4; SW2B9754 writes Water+2E4.
- LW2B975C reads443EA8; SW2B9760 writes Water+2E8.

These are live process reads, not replayed RGB getter receipts. Each read checks current App thread/lifetime, unknown poison, known word and source publication. Each records its actual value/publication before its corresponding object store, preserving zero as known. A poisoned/unknown source stops at that LW with earlier object stores intact. Publication overflow blocks writes elsewhere but does not unnecessarily forbid reading a still-known word. Historical read receipts tolerate later joined overwrites and do not manufacture current knowledge after poisoning.

Existing SourceWaterProviderMembers::integerArrays remains the actual color-lane owner. Constructor2B8590 stores all eight integer zeros in original reverse-lane order; new integerConstructorZeros explicitly retains those constructor receipts when current RGB lanes later change. Env alpha and all four Froth lanes keep their prior actual zero values. This is not a second shadow color object. Native withdrawal keeps historical bytes/receipts without claiming a live provider or resetting shared globals.

The final frontier is BEFORE LW2B9768 of global443EAC. That process alpha word has no accepted same-App producer. Object+2EC's constructor zero, RGB globals, or an ELF/BSS reset assumption cannot supply it. Thus no alpha copy, Froth copies, full16-byte443EC0/443ED0 reads, Wake Froth selection, shader resource resolution, or loader completion is claimed. Earlier first-word vector receipts would not justify those later quadword loads even if alpha were joined.

## Evidence and validation limits

Original ELF SLUS_202.68 SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; file mapping VA-0x100000+0x80. Frozen evidence contains158AD0/158240, complete309230, caller2B9714 onward, literal padding, constructors and color-array callback. The manifest/SOURCE-IDENTITIES track exact source preimages, inherited dependencies, immutable fetched members source, SHA256 and Git blob IDs. Static review and exact textual reconstruction remain distinct from unperformed compile/runtime verification.
