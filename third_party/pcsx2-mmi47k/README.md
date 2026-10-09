# PCSX2 scalar MMI transplant (47K)

The user-supplied `work/reference/pcsx2-master` is the authority. `MMI.cpp`,
`R5900OpcodeTables.cpp`, `BitUtils.h`, and `COPYING.GPLv3` here are byte-identical
snapshots. Exact hashes are in `provenance.json`. No upstream commit or release
has been inferred. Copyright notices and GPL-3.0-or-later licensing are retained.
The copied opcode-table and bit-utility files document decoder/bit-count
provenance; they are not compiled as full PCSX2 translation units.

Run `tools/prepare_mmi47k.py` to reproduce `src/core/mmi47k.cpp` and its test
reference. The production transplant gives 99 handlers and 50 stateful helpers
an explicit `rr::MmiState&`, changes their namespace, and supplies scalar types
and count-leading-sign-bits glue. Instruction expressions remain unchanged.
No production global CPU, TLS, heap allocation, platform API, or SIMD type is
introduced. The compatibility shim's bit-count is a scalar equivalent of the
copied utility, avoiding its C++20 and broader PCSX2 dependencies.

The test oracle includes the unmodified original MMI source with its original
global-CPU calling convention. It is test-only and not reentrant. Both decoders
route from the original opcode table. Differential tests therefore validate the
transplant, not an independent proof of every upstream instruction. Separate
byte-concatenation and saturation checks provide independent QFSRV/PADDUB
expectations. Every register, HI, LO, code, and SA is compared.

Targets are little-endian x86/x64. Caller keeps GPR zero architectural and SA in
0..15 bytes. The port retains upstream union-alias and signed-wrap assumptions:
Clang uses `-fwrapv -fno-strict-aliasing`, and the recorded MSVC/nxdk builds must
continue agreeing at O2. This is not a universal C++/ISA portability claim or
sanitizer clearance. PC, cycles, memory, interrupts, COP1/COP2 and VU execution
are outside this component.

`tools/Build-Mmi47K.ps1` builds Windows x86/x64 tests, a separate diagnostic XBE,
and a Windows x86 runner linked to the exact nxdk-generated production and test
objects. The latter executes those CPU objects on Windows; it does not boot
Xbox firmware, test the Xbox platform layer, or forecast Xbox performance.

This component is not yet wired into the running Windows game. It is the packed
instruction foundation for a compact CPU backend that can fit the Xbox memory
budget. Existing generated code and the PCSX2-backed renderer remain the game
reference. The scalar MMI object's size is not the total CPU runtime size and
does not establish total 64 MiB fit.
