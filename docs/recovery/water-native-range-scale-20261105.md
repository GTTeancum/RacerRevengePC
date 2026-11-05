# Native-adapted range scaling and signature-wave branch

This packet continues only the explicit native-adapted Water route. Original Config and aggregate source PCs remain30492C, with original heap/free and Wake selection return unjoined. It implements current443B90 scaling at2B9888..2B9898 and the following currentWater+6C zero/nonzero branch through its delay slot. Neither branch target body is entered. No Water completion, rendering or GPU readiness is established.

## Current operands and ordered effects

Owner::scaleWakeRangeNativeOnly requires the native-only mode, completed native packed-state store at its historical2B9888 boundary, retained loader F1 bits0x41800000, the same live Config/Water/Application and thread, no incompatible aggregate borrows or withdrawal/closure, and original source cursors still30492C. It has no normal-source caller and is one-shot; repeats report alreadyAttempted without another multiplication or store.

At adapted2B988C it reads CURRENT443B90 through same-App lane4, recording the word, publication and whether the last write was already native-adapted. It does not reuse a saved WaterHeightRange getter result. This adapted consumer can read a known source-exact or native-adapted lane4; lanes30..34 retain their stricter adapted-provenance read gate. Unknown/poisoned/thread/lifetime failure stops before numeric work.

At2B9890 it uses the retained loader F1 from the immediately preceding native packed slice as the RIGHT operand. That is distinct from the constructor's historical rawF1. Numeric refusal retains both operands and current-global read, without a product or global write.

After an admitted product,2B9898 checks current same-App write authority and publication capacity, then overwrites that same lane4, marks native-adapted provenance and increments its publication once. A publication-capacity refusal retains the product/local-mask receipt but does not write. The source-exact scalar/vector accessors continue to reject adapted provenance. Native provider withdrawal does not reset this process word.

At2B989C it reads CURRENT signatureWaves6C from the same Water provider, rather than the historical NumSignatureWaves getter.2B98A0 compares the word exactly to zero;2B98A4 is the NOP delay on both arms. Zero stops before target2B9C40. Every nonzero word, including negative/high-bit values, stops before fallthrough2B98A8. No count sanity rule, clamp or allocation is inserted. A refusal after the global store retains that store; there is no rollback.

## Exact bounded numeric proof

The new sourceWaterScale16Native50LA helper is a value-only specialization of the already pinned scalar PS2Float Mul/DoMul model with SOFT_MUL selected. It is not silicon equivalence, a general EE backend, a host floating operation or proof of the running backend's configuration. The packet retains the exact model source blobs and their previously verified pinned-source metadata as evidence; no model or helper is executed.

The admitted domain is literal positive zero, or either-sign normal input with biased exponent1..250, with RIGHT operand exactly0x41800000. Negative zero, all nonzero exponent0 words, exponent251..254 and exponent255 are conservatively rejected. Rejection is a native aperture, not an original exception, saturation, guest parse failure or substitute zero.

For a nonzero admitted input, the left significand is a=0x800000|fraction and RIGHT16's significand is b=0x800000. In the operand-sensitive MulMantissa circuit, all eight low Booth selectors inspect zero low bits of b. Their partial data/negates and the carry-network low output are zero. The exact product a<<23 has bit15 zero, so the correction bit is zero. The resulting significand is exactly a, with no renormalization. Exponent becomes E+4, in5..254; sign and fraction are unchanged. There is no rounding, underflow or overflow choice in this admitted nonzero range. Positive zero times positive16 yields positive zero.

Thus the helper uses only integer bit composition: sign/fraction retained, exponent incremented by4. It does not call the generic positive-only multiplier for negative inputs or broaden that multiplier's domain. Both output references stay unchanged on refusal.

## Local flag receipt, not live FCR31

The pinned MUL_S path clears O0x8000 for these nonexceptional results. Its later underflow check does not clear an already-set U bit because its clearing branch tests O; sticky and other bits are retained. This packet records only localClearMask=0x8000 with no asserted condition. It neither invents a full current FCR31 nor mutates a shared control-register owner. Other backends/configurations and exceptional words remain outside the proof.

## History, partial states and cleanup

NativeWakeScale owns a separate adapted PC, operand read/provenance/publication, local product/mask, output publication, current signature-word observation and optional branch decision. Getter, loader, retirement and packed-state cursors remain historical; original source PCs/return fields are unchanged.

Preflight distinguishes unentered, current-input refusal at2B988C, unsupported numeric input at2B9890, store-admission refusal at2B9898, a retained successful store before an unavailable object read at2B989C, and either branch target after NOP. Optional zero and optional false are engaged values. A stored publication must be the captured input publication plus one without overflow. When the relevant publication remains current and unpoisoned, value/provenance must match; newer publications preserve history without making an old word current. Native cleanup never rereads retired provider backing, replays the operation or resets Application globals.

## Next boundaries and provenance

Nonzero target2B98A8 begins stack-vector setup and later rereads Water+6C before signature-wave allocation. The branch's captured word is not authority for that later read. Zero target2B9C40 begins a new CURRENT Water+64/NumPools branch, not loader completion. Those bodies remain unimplemented here.

Immediate predecessor: /tmp/racer-water-packed-20261105-0233, patch731da3d21c3623498589f2e95e2c38929167cef74742f5bf1a0706d2525712ad, final manifest34a05bdab4959d627f592eecbf3cfefb199433cc7d77063e5fb044c84cb48ee1. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. The exact unpublished Water chain and narrow vector-header rebase qualification remain; publication is paused.

Verification is static source/ELF/model-source inspection, file-byte/Git-blob identity and strict text-patch reconstruction. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
