# Water opacity subtraction and scalar getter tail

This continues the reviewed [opacity getters](water-opacity-getters-20261105.md) from2B9428. The real subtraction consumes the previously retained MaxOpacity load2B9408 and distinct BaseOpacity reload2B9418. The result is published to443ED8 at2B9430, in ChannelScale0's call delay slot. Four channel-scale getters, five frame-rate getters and WakeScale then continue through the same Config. The new frontier is2B9528 before integer getter158B30("EnvColorRed",80,443EA0).

## Exact bounded subtraction contract

The Water wrapper is a value-only reduction of pinned `GitHubProUser67/pcsx2@0392a64e992ac3fa5445dbe484933616cb204674` PS2Float Sub/DoAdd with CHECK_FPU_SOFT_ADDSUB selected. That option is separate from the earlier division option. This is not host IEEE subtraction, EE silicon equivalence, arbitrary emulator configuration, instruction-pipeline behavior or live FCR31 emulation.

The wrapper admits +0, or either sign of a normal word with biased exponent107..156. It rejects negative zero, subnormals and all other exponents before producing a result. This contains every actual operand from the predecessor's <=9-total-digit, <=6-fractional-digit decimal subset and its exact defaults32/64. It does not broaden accepted input syntax or replace source operand ownership. No host floating arithmetic is used.

It reuses the existing pure-value sourceParticleFogSubtract50LA, freshly retrieved at immutable RacerRevengePC main e566993fc25f628ce5b4aa5def424c6003f53b75. The helper brings no Particle Config, output receipt or source-call event. Its actual integer computation is reviewed against complete pinned PS2Float, FPU, BitUtils and fixed-type/build sources.

For normal operands, reverse the right operand's effective sign and put the larger exponent first. If exponent difference d>=25, the model returns the greater-exponent effective operand unchanged. For d=1..24, clear the low d-1 significand bits of the smaller operand; for d=0 clear none. The large aligned magnitude is its24-bit significand times64. The smaller aligned magnitude is its masked significand times64 divided by2^d, exactly a multiple of32. Their signed sum has magnitude N either zero or a multiple of32 below2^31. Exact cancellation returns+0. Otherwise its highest bit p is5..30 and the result exponent is E+p-29, hence83..157 for admitted E107..156. All nonzero results remain normal, with neither overflow/saturation nor underflow/flush reached.

Normalization shifts N to its23-bit fraction without a nearest-even increment. This final truncation follows the original smaller-operand mask, so the whole operation must not be labeled ordinary IEEE round-toward-zero subtraction. Zero-minus-normal flips its sign; normal-minus-zero preserves it; +0-minus+0 is+0. The helper refuses nonnormal output, and the Water wrapper independently checks its proven result range. Failure leaves outputs unchanged and stops at2B9428, retaining both actual input loads and all earlier source effects.

## Operation-local flag effect

The pinned raw PS2Float results have no newly asserted overflow/underflow flags in this domain. FPU SUB_S writes the result, checks O|SO, then U|SU. Its checkOverflowUnderflowSoft nonexception branch tests the O mask even for the underflow call. The exact local transformation therefore clears O (00008000) only and preserves U, all sticky flags and every other bit.

The World history records a local clearMask00008000 and no newly asserted flags. It does not assign a shared control-register value, infer that incoming U/D/I/sticky bits were clear, or borrow a previous division's effect. A future live FCR31 or flag-dependent consumer still requires the actual shared owner and all intervening writers.

## Source publication and actual destinations

After the single subtraction result is retained, the next call's delay slot2B9430 publishes it to the same App process owner at443ED8. The native publication version is recorded without a fallible gap after that store. Failure to admit the process lane stops at2B9430 with the subtraction result and local flag effect retained, before any ChannelScale0 lookup. A later getter failure does not roll back the delay-slot store.

The following158B80 occurrences all use the same selected section, authentic row/token lookup, process dependencies and bounded decimal conversion. Source-missing and source-found remain distinct, and unsupported found strings never become defaults:

- ChannelScale0..3: calls2B942C/2B9444/2B945C/2B9474, defaults+0, actual Water+98/+9C/+A0/+A4 destinations.
- FramesPerSecond0..4: calls2B948C/2B94A4/2B94BC/2B94D4/2B94EC, defaults+0, actual Water+84/+88/+8C/+90/+94 destinations.
- WakeScale: call2B950C, default41200000 (10), actual shared443B88 destination.

Channel lanes are not initially unknown at this point: the original base constructor already zeroed them at2B093C/2B0944/2B094C/2B0954. This packet joins their live typed fields to those exact constructor stores, before the later getters overwrite them. Existing zeroWords entries remain historical constructor receipts rather than incorrectly asserting the current fields must stay zero. The frame-rate lanes have no earlier represented write and remain unknown until their respective getters store.

Before a getter stores, earlier live field values survive. Each completed missing getter has an engaged false return and still allows the caller to continue. A native lookup/conversion refusal has no source return or destination store. Fresh getter-local scratch is independent between occurrences while retained tails within each row loop preserve prior knownness.

The source's caller setup instructions and destination delay slots retain their exact order. WakeScale writes shared443B88 directly inside158B80 and then records its version without a fallible gap. No EnvColorRed default, integer conversion, color write, next section, Water return or World completion is inferred at the new2B9528 frontier.

## Custody and partial state

Preflight relates current channel fields either to their actual constructor write or the latest completed getter, frame-rate fields to their own getters, the subtraction to its two retained inputs, and process publications to their monotonic historical versions. It recomputes only the bounded integer result for consistency; this is not another source arithmetic occurrence. Later same-process overwrites can supersede current values without invalidating historical receipts. Unknown process aliases block newly admitted stores rather than inventing known shared state.

Native Config/provider withdrawal retains earlier source histories and all App process publications. It releases existing file/Track and provider custody in the established order, without claiming a source destructor, source free, allocator failure, global reset or completed loader. Original EE heap/free-list/address/errno/success/failure remain unrepresented by the earlier success-only native backing adaptation.

## Identity, integration and verification

The five modified successor preimages come from the exact accepted but unpublished opacity packet, patch SHA25639a4a06eb5ad530c62f418842d755a600d29a3bdaad0c34691135c8e49cf0599. The constructor provider preimage is an unchanged published main dependency. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75; no blocked publication was retried. The manifest records exact predecessor hashes rather than inventing a published successor commit.

The inherited process-globals Water knownness addition still requires a narrow reviewed rebase if the pending vectors/section checkpoint lands first. This packet does not modify that header and must not overwrite those vector changes during integration.

Original SLUS_202.68 SHA256 is c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Evidence includes original caller/literal/base-constructor disassembly and raw spans, complete immutable numerical sources, fixed-type/build/normalization provenance, a mathematical range proof and the exact-commit helper retrieval.

Validation is static source/ELF reads, hashes, diffs and text-patch reconstruction only. No build, compilation, syntax/type check, tests, probes, helper/model/target execution, emulation, runtime, publication, gameplay or GPU-readiness claim occurred.
