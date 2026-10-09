# Compact real-game leaves, 47M

The first compact EE integration uses original ELF instruction words for small,
register-only functions. It does not remove their native code from the linked
game yet. It is a correctness and compatibility step toward a smaller backend;
no FPS gain or stock-Xbox memory fit is claimed.

The source audit identified 295 leaves, 733 words, including 56 MMI leaves.
All words matched the original SLUS_202.68 ELF. Calls, memory access, backedges,
traps, syscalls and coprocessor operations are excluded. SA instructions and
QFSRV are also excluded because the legacy runtime's SA convention needs its
own audit. The borrowed PCSX2 instruction core preserves its byte-unit SA.

47 scalar handlers join the 99 borrowed MMI handlers. Windows x86, x64, and
the exact nxdk-compiled objects each pass 593,931 scalar/leaf checks, digest
c03675d4. This includes all ADDIU immediates, SA helper arithmetic, return-target
latching before the delay slot, and rollback on unsupported instructions.
The Xbox XBE builds but has not run on an Xbox. Seven inspected objects contain
11,189 decoded instructions with no detected instruction newer than the target
Pentium III/SSE1 set; external helpers, CRT and hardware execution are outside
that audit. The nxdk scalar and leaf code sections total 5,044 bytes, excluding
MMI, data, runtime and link overhead.

## Discrepancy retained for a separate correction

The first direct comparison of all 295 original generated functions failed:
75,520 cases, 1,056 mismatches. The first reported mismatch is entry 0x2ec05c,
where the original generated MOVN uses SET_GPR_VEC and changes all 128 bits.
The supplied PCSX2 MOVN/MOVZ handlers assign only UD[0], preserving the upper
64 bits. Twelve functions containing either conditional move are conservatively
deferred. The borrowed handlers are not changed to reproduce this discrepancy,
and the canonical generated source remains unchanged. This is a discovered
semantic difference, not proof that it caused a particular game artifact.

The remaining 283 leaves pass 72,448 randomized/edge input comparisons of the
entire initialized R5900Context, digest a769d217. Input includes nonzero physical
register zero, upper register lanes, HI/LO state, both delay flags and sentinel
state outside the supported subset. Original leaf code is called directly;
neither RAM nor a runtime is needed by this restricted subset. This test uses
the real generated native functions, in addition to the PCSX2 reference tests.

## Native connection and validation

Each replacement first verifies that runtime lookup resolves to the exact
original symbol. The adapter preserves unrelated CPU state and physical r0,
maps both HI/LO pairs, captures JR's target before its delay instruction, and
preserves the original branch bookkeeping. Unsupported execution rolls back
before calling the original function. Strict return diagnostics must be off,
as in the verified reference build; configuration rejects the other setting.

Native VBlank 500 startup installed all 283 replacements, executed 498,152 calls,
and used zero fallbacks. EE RAM, IOP RAM, PCM, final image, reported terminal
state, exact stop and unchanged memory cards match the 47L mode-1 startup.
Candidate executable SHA-256:
2e3b1c9fbb41933b7dc908ee9cd6f483e33824caef0c559702907f6bb3036cde.
The full-race check is queued after the ongoing GS sweep and remains unaccepted
until its actual terminal state report passes.

## Reproduction and failure evidence

Run prepare_scalar47m.py, Build-Scalar47M.ps1, and audit_scalar47m.py with a fresh
output directory. Build-Leaf47M.ps1 verifies hashes of the original generated
sources, prepares the adapter, builds it and runs the direct differential.
The source audit manifest remains the original 295-candidate inventory;
build/leaf47M/selection.json records the narrowed 283-function integration.
Run-Native47M.ps1 checks the accepted binary hash before launching.

The first scalar generator used substring replacement in a support table,
turning longer opcode names into invalid identifiers. The compiler rejected it;
the corrected generator matches whole qualified handler calls. The rejected
generator/source/logs are retained. The first failing native differential's
sources, exact binary hashes and logs are retained; its binaries were overwritten
by the second build before a recovery copy could be made. A copy attempt met
the linker's file lock. Subsequent build scripts freeze executables in each
attempt directory before testing. The accepted binaries are saved intact.

Build work started only after the GS mode-1 run recorded VBlank 6960, beyond its
6000–6900 timing window. The earlier standalone scalar build ended before that
window. The two closed GS runs match saved state; the interpolated throughput
ratio is 1.097052x, with overlapping capture-bracket bounds. Treat this as one
modest observed result, not a repeat-confirmed speedup or distinct rendered FPS.
