# Native x86 PC and Xbox migration plan

## Current priority — 2026-10-03

The actionable implementation sequence and acceptance gates are now recorded
in [Native PC plan 48A](native-pc-plan48a.md), with a linked recovery map of
original assets and simulation entry points.

User clarification supersedes the earlier migration framing below: first get
Racer Revenge running well as a native x86 game on PC. The intended deliverable
is a shared native game codebase for PC and original Xbox. Recover game logic
and asset semantics, and replace PS2 execution/rendering machinery with native
game systems. Compiling a PS2 compatibility runtime for x86 is not sufficient.
Keep the existing Windows/PCSX2 path as a correctness and investigation reference;
its performance improvements alone do not establish a native game architecture.

Original Xbox remains a planned target. Account for its single-core CPU and
memory constraints when choosing shared interfaces, but Xbox packaging, hardware
bring-up and platform-specific optimization are not the immediate milestone.
Measure actual native PC gameplay performance and behavior as systems migrate.

Add offline asset conversion and packaging to the plan: recover meshes, textures,
materials, collision and animation into semantic engine assets, then produce
platform-appropriate layouts and streaming packages. Establish the PC asset path
as needed for native rendering; defer Xbox-specific repacking until the asset
schemas and native PC path are validated. The supplied CarsTools source is a
knowledge reference, not a required tool build or proven compatible file format.
See [CarsTools intake](cars-tools-reference.md).

## Historical migration record

The remaining sections record the earlier implementation and evidence. Their
guest-cycle/register requirements apply to the reference compatibility path,
not a requirement to retain PS2 hardware emulation in the final native engine.

Date: 2026-09-30. Goal active; staged implementation authorized by the user.

## Architecture

Preserve the reference Windows game while building a shared native core. Keep
guest addresses as uint32_t; host pointers must never become guest addresses.
Represent EE 128-bit register state without requiring SSE2 or x86-64. Retain
the original instruction semantics, VU pipeline corrections, and guest timing.

The core owns game execution, event accounting, asset decoding and portable
geometry operations. Platform implementations own device input, audio output,
storage, clocks and GPU submission. Windows and Xbox use separate renderers.
nxdk/pbkit is the intended Xbox platform; modern PCSX2 remains a correctness
reference and Windows renderer during migration, not an Xbox dependency.

## Initial implementation and limits

The cycle ledger separates cheap EE cycle accounting from device advancement.
It refuses execution past an armed event boundary, preserves the 8:1 EE/IOP
ratio and fractional carry, and allows explicit synchronization at MMIO/RPC/DMA
boundaries. It does not discover deadlines or run devices. Those responsibilities
remain in the future adapter; batching is unsafe until all relevant interactions
are audited. Never increase guest cycle charges to manufacture an FPS gain.

Portable GPR storage preserves upper lanes during scalar writes, sign extension
for EE 32-bit results and the architectural zero register. It is not a full EE
instruction backend. SIMD/MMI and floating-point operations still need migration.

The register file and cycle ledger are not connected to gameplay yet. The
portable IdleWindow now has an opt-in adapter in the legacy Windows IOP: after
the existing kernel proves there is no ready thread, it remembers the earliest
device/thread deadline and avoids servicing every tiny EE advance. External
mutations invalidate the window. This preserves guest cycle charges and CPU
instruction execution. The initial 47B2 native pair measured 3.19 versus 4.44
capture VBlanks/s, but a later fixed-VBlank comparison exposed a nested-RPC
invalidation defect. That speed difference is not accepted as behavior-preserving.
Candidate 47H invalidates before and after nested guest execution and public
mutations, and before mutable RPC-thread access. With idle batching and ready
selection caching enabled it matches the original at VBlanks 500 and 7,000:
full EE/IOP RAM, PCM, final picture, reported terminal state and exact EE cycle.
The controlled 6,000–6,900 race window improves from 3.13567 to 5.34703 emulated
VBlanks per host second, 1.70523x. This is neither distinct-picture FPS nor a full
race/Xbox acceptance result. Exact state comparison is retained in 47J2.
Diagnostic core binaries are not the Xbox game.

Candidate 47I adds a bounded positive IOP import cache. It retains the original
decoder on misses and tracks writes to the memory pages read by a successful
search. Public writes, reset, and device DMA invalidate affected entries; version
counter saturation permanently disables reuse. Its memory-version component
passes the shared Windows/nxdk-object suite. Native decoder and physical-module
comparisons pass, and native VBlank-500 game state/PCM/picture matches the original
with the cache disabled and enabled. In checkpoint 47K, the combined 47J2 runtime
also matches the accepted original at VBlank 7,000, including full EE/IOP RAM,
PCM, final image and reported terminal state. That run enables host profiling;
it supplies correctness and bottleneck evidence, not a matched FPS measurement.

Candidate 47J2 stores IOP ownership as one bit per byte, reducing its backing
allocation from 2 MiB to 256 KiB. It passes 2,226,205 queries against the actual
original memory implementation and a native VBlank-500 state comparison. The
initial 47J free path mistakenly retained byte indexing and is rejected; its
source, failed comparison and access-violation log are preserved. The correction
clears the exact freed byte range, and the memory owner cannot be copied/moved
while device references and the bitmap view point into it. The complete IOP
candidate builds and passes differential checks on Windows x86 with /arch:IA32;
24 implementation objects contain no detected post-Pentium-III instructions in
the recorded audit. CRT/DLL code and Xbox linking/boot are outside that audit.

Checkpoint 47K transplants all 99 PCSX2 scalar MMI handlers, with 50 stateful
helpers taking an explicit CPU reference. The original source, license and
opcode tables are preserved. The new component passes 2,427,678 checks with
digest f22b8723 on MSVC x86, MSVC x64 and the exact nxdk objects linked into a
Windows x86 runner. The nxdk production object has 18,524 code bytes; its scan
detects no post-Pentium-III instructions. A diagnostic XBE links successfully
but has not booted. Checkpoint 47M connects it, alongside 47 borrowed scalar
handlers, to 283 original-ELF leaves in native startup: 498,152 calls, zero
fallbacks and matching saved state. Twelve MOVN/MOVZ leaves are deferred because
the original generator writes upper lanes differently from PCSX2. No compact
EE speed or whole-game memory-fit claim follows from this small subset.

The 47J2 race profile near VBlanks 6,043–6,913 attributes about 11.3% of sampled
elapsed time to IOP-inclusive work and 27.2% to DMA-inclusive work. The categories
overlap and include preemption; the GS worker/GPU is not independently measured.
The next larger Windows investigation is the duplicate GS frontend work before
the raw-GIF PCSX2 backend, plus guest dispatch/event overhead. Do not forecast
an order-of-magnitude gain from more IOP caching alone.

The immutable compact registry maps exact guest PCs to 16-bit original-function
IDs using shared 64-slot patterns and page-local owners. The generated candidate
preserves all 733,427 slots, including holes and 154,796 registered entries, and
has passed a linked comparison against the actual original initialized native
function pointers. On x86 its data plus bindings occupy 890,386 bytes versus
2,933,708 bytes for the dense pointer table. This excludes executable code and
runtime replacement storage. A separate native candidate now uses a sparse
replacement directory, allocating 64-slot pages only when an address changes.
Explicit null entries remain distinct from unchanged entries. Its real runtime
API passes complete before/after pointer comparisons against the canonical
table. The separately built candidate matches the original at VBlanks 1 and 500,
including full RAM, PCM and final pictures. Whole-race validation remains open.

## Acceptance sequence

1. Compile identical core/tests for Windows x86, Windows x64 and nxdk/Pentium III.
   Run Windows checks; inspect the Xbox image/ISA. Record separately whether an
   XBE has actually executed on hardware or an emulator.
2. Audit generated-code ownership and duplicate spans. Produce a deterministic
   compact translation with valid resume entries; validate original guest state.
3. Adapt event batching to real EE/IOP execution. Flush before cross-device reads
   and writes, RPC, DMA, and interrupt observation. Compare event traces and
   original state, including pending loads/branches and simultaneous deadlines.
4. Run the same original race section with controlled diagnostics and settings.
   Measure host time, unique pictures, guest elapsed time, CPU service categories,
   maximum frame time and memory separately. Existing ~3 VBlanks/s is not an
   Xbox forecast. No disabled sound, altered timing, or repaired guest state.
5. Translate the game's required VU programs and rendering behavior to portable
   operations/NV2A, preserving packet/geometry/image reference evidence.
6. Accept a complete race with normal input, sound and stable whole-course
   rendering. Real Xbox timing is required before claiming a hardware FPS result.

## Memory and recovery

Stock total memory is 64 MiB, shared with graphics. A provisional application
ceiling of 56 MiB reserves 8 MiB for platform needs and uncertainty; it is a
planning gate, not a measured allocation. EE RAM alone consumes 32 MiB if kept
whole. Compact code, shared buffers, bounded caches and asset streaming are
mandatory. A small diagnostic image does not establish that the game will fit.

The previous generated executable has ~96 MiB of machine code. Full PCSX2 VM
allocations and that code layout cannot be the final Xbox runtime. Do not delete
the original source, captures, or regression evidence during this migration.

Checkpoint each verified milestone with source/evidence ZIPs, exact SHA-256
manifests, toolchain revisions, logs, known failures and explicit restoration
instructions. Source packages for the new core should be self-contained; the
legacy reference retains its existing recovery chain.

## Integrated state at 47N

The compact registry now combines with the current 47J2 IOP and 47L graphics
candidate, with and without the 47M leaves. Both VBlank-500 startups match their
accepted references. Link maps prove the dense table is absent. Each linked
Windows image is 10,321,920 bytes (9.84375 MiB) smaller than its corresponding
dense-table build. This includes removed initializer code; all original guest
instruction code remains. The combined image is still 101,605,376 bytes, already
larger than the stock Xbox's 67,108,864-byte total budget before other allocations.
The 32-bit immutable registry representation is 890,386 bytes; this is a component
measurement, not an Xbox game build or a complete budget.

Both combined variants retain sampled Windows process-memory observations.
Those include the PCSX2 renderer and platform dependencies and must not be
presented as Xbox memory requirements. The 283 tiny native leaf spans total only
21,792 bytes in the prior link map, below the diagnostic executor's added cost.
Removing those leaves alone would not solve the code-size problem, so no such
removal is claimed. Broader compact execution or code generation is still needed.

The completed 47L raw-GIF sweep matches saved state in all modes. Full-history
batch elision records 5.910268 versus 5.387408 capture VBlanks/s in one fixed race
window. Additional history suppression records 5.967689 VBlanks/s. Capture-bound
uncertainty overlaps; these are modest one-pair observations, not repeat-confirmed
speedups. History suppression is not the route to the requested large gain.

The compact-leaf full race now matches the accepted reference at VBlank 7000,
with 1,954,337 calls and zero fallbacks. The combined-registry race is active.
A larger rendering investigation measures the bridge's
synchronous GS-thread handoffs: work/pcsx2-bridge41n.cpp stages packets but flush()
waits on a future, and VSync performs a second synchronous handoff. This source
observation is not yet proof of dominant cost. Preserve ordering and actual
readback/FINISH semantics before changing synchronization; do not remove a fence
merely because the Windows renderer is multithreaded. An Xbox renderer will
require its own direct single-core submission path and memory accounting.

Checkpoint 47O adds a diagnostic backend that measures the existing renderer
DLL calls without changing packets, transfer semantics, fences or the DLL.
Startup on/off matches the combined runtime; all counters reconcile with the
500-VBlank scheduler stop. During this startup run, ongoing renderer calls
consume about 0.613% of host elapsed time, plus 6.148% for initialization. The
full-race profile is queued; startup alone does not justify a synchronization
rewrite. See docs/renderer-profile47o.md for interpretation and validation.

Checkpoint 47P completes both the combined-registry and renderer-profile races with exact saved-state matches. Renderer DLL calls total 6.006640% of the measured race window. Main-thread CPU remains dominant; repeated environment scans are confirmed, while frequent file-write samples remain unattributed. See docs/performance-profile47p.md for exact scope, observer effects, recovery hashes and remaining Xbox blockers.
