# Native-adapted Wake packed state

This packet continues only the explicit native-adapted route after its five completed Wake getters. Original Config and aggregate source PCs remain30492C, and the original Wake selection return remains absent. The adapted slice covers2B9864..2B9884 and stops before2B9888. No original heap/free completion, source return, later floating-point operation, Water completion or GPU readiness is claimed.

## Current input and exact bit operations

The explicit Owner::packWakeStateNativeOnly entry requires the native-only mode, the same live Config/Water/Application identities and owning thread, completed native Wake getters at their historical2B9864 boundary, no incompatible aggregate borrows or native withdrawal/closure, and all original source cursors still30492C. It is one-shot, has no normal-source caller, and repeated calls report alreadyAttempted without rereading or rewriting.

At adapted2B9868 it reads CURRENT383540 through the same-Application adapted scalar accessor, lane31. The captured word and publication are separate from the earlier AlphaReference getter result/publication. A later legitimate write may have changed that current word; this operation does not substitute the historical result. Unknown, wrong-thread, released or untagged global state refuses before arithmetic or object mutation. Publication exhaustion does not prevent this read-only access to an already known word.

The supported sequence is exact for every32-bit input word:

1. Model LW's sign extension to64 bits using unsigned bit composition. Bit31 set adds0xFFFFFFFF00000000; otherwise the upper32 bits are zero.
2.2B986C/70 form mask0x5000D.
3.2B9874/78 set loader F1 bits to0x41800000, representing16.0. Preserve this distinct loader register receipt; do not overwrite the old constructor Members.rawF1 history.
4.2B987C shifts the sign-extended unsigned64-bit pattern left4, retaining modulo64 behavior.
5.2B9880 ORs the full64-bit pattern with0x5000D.
6.2B9884 stores all eight bytes' semantic word into the same Water provider's nativePackedState2A0.

There is no32-bit truncation, signed-shift undefined behavior, clamp, multiplication, float conversion or host floating-point dependence in this slice. The packed result is not a source allocator/address receipt.

## Target ownership and chronology

The existing SourceWaterProviderOwner is the exact retained allocation/entity from the Water factory. It gains an optional64-bit nativePackedState2A0 field, initially unknown, rather than another provider or detached replacement object. Only the full supported adapted SD establishes the field's value.

Static tracing found a separate SD at2B8AF0, but that is not prior initialization for the present loader. The outer initializer calls the base initializer at2B8714; the base reaches this loader through the virtual call at2B0C5C. Outer2B8AF0 runs only after that nested base/loader call returns. The constructor does not supply a prior2A0 value, and the previous native representation had no2A0 field. This packet neither moves the later store earlier nor pretends the loader has returned.

## State, history and cleanup

NativeWakePackedState retains its own adapted PC, attempted/stored flags, current AlphaReference read/publication, sign-extended word, mask, shifted word, packed result and loader F1 bits. The preceding getter/load/retirement cursors stay historical. Original source cursor and return fields are untouched.

The externally visible native states are unentered, current-global read refusal at2B9868, and completed native store at2B9888. There is no fallible work after the successful current read. Preflight recomputes the exact unsigned64-bit relationships and checks the same-provider field while that provider remains live. It validates the captured global value/provenance only while its recorded publication is still current; newer publications preserve the historical read without claiming it remains current.

Native withdrawal preserves the receipt and does not reset Application globals. After the provider is retired, its field is no longer dereferenced; retained history still documents the adapted store. Repeated entry never fabricates another store or reread. No new allocation or file/stream operation occurs.

## Boundary and verification

The next family starts at2B9888: it reads CURRENT443B90 and performs MUL.S by the retained16.0 before writing that shared lane. That arithmetic and subsequent signature-wave branching are not entered here.

Immediate predecessor: /tmp/racer-water-wake-getters-20261105-0221, patch5e7c62873a0de452a20a4b0b71e932721a95236e38b8991948029e7207610738, final manifestf45b895ec12265c3055215cf7ac67b9c482f8a99a684d19db546423c86cb2abe. The exact unpublished Water chain and inherited narrow vector-header rebase qualification remain. Published main is still e566993fc25f628ce5b4aa5def424c6003f53b75; publication remains paused.

Verification is static source/ELF inspection, file-byte/Git-blob identity and strict text-patch reconstruction only. No compilation, syntax/type checks, build, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
