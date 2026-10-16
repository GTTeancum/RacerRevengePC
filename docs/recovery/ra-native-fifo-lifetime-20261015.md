# RA collector FIFO / zone scissor source review � 2026-10-15

Status: isolated, inactive, UNTESTED. No tests, builds, syntax checks, sanitizers, converters, emulation, input, or gameplay checks were run. No central checkout or disc file was changed. Root alone merges and tests. This is additive to the integrated emission-inspection delivery libfile_9deeae83f7e08191ae16ccf87ba221b8, not a replacement for its prerequisites.

## Findings
- Zone constructor 27F120 clears A0 at27F15C; destructor27F224 frees that allocation. A0 is the native semantic scissor owner, NOT a SourceCamera scene pointer.
- 27FEE0 raises actual portal depth, updates actual dispatch epoch, prepares A0 through280230, recursively draws children, and only then280114/280118 publishes this zone's A0 to collector443848+4014 before its own methods34/38. Selection must remain separate.
- 280230 allocates0x60; its numeric viewport transformation uses bounds4C/50/54/58 and actual viewport1E0/1E4/1E8/1EC. Two source contexts share the same endpoints. The adapter retains numeric endpoints only, with no command packets, minus-one adjustment, clamp, or GL-origin invention. Out-of-range arithmetic remains unsupported.
- 29C270 initializes collector epoch04 to1. 29C150 redirects to final leaf, appends stable nodes, and sets per-leaf epoch/head/tail. 4096 is a registration/allocation-block parameter, not a maximum total FIFO node count.
- 24B9B0 retains original pass parameters even after representative redirection; node18 borrows the current4014 owner. 24B7B0 drains registrations in source order. Actual allocator reset24B940 precedes epoch increment24B954; an empty queue does not bypass renderer/global work.

## Private implementation
- NativeCollectorFifo50LA is owned by the actual Application and found only through its real RaceDisplay CableInspection pin and canonical material domain. Its epoch is process collector state, not registry generation or host frame.
- Stable native records retain original emission binding, actual strip/allocation attributes, camera bridge, display pin, and scissor owner. Per-leaf links preserve FIFO order. Canonical strip material and collector seals are checked. Material table, allocation, and camera retirement guards retain dependencies across failed or unsupported drains. Material animation map views remain prospective; actual lowering must reacquire their current phase.
- Source lighting Owner privately prepares stable numeric A0 state from its retained zone projection and actual camera lease/application. A separate private publication capability represents280118 after-child ordering. It intentionally has no issuer in this patch.
- Acceptance and completed-drain bodies are private, unreachable groundwork. No existing inspection callback is advanced or wired into insertion. Drain returns unsupported without clearing records or advancing epoch, even when empty. Its private completion capability has no issuer, and draining is never entered. No fake successful drain was introduced.

## Review checklist and remaining work
- [ ] Central source review: include dependencies, ownership guards, exact preimages, source arithmetic and FIFO ordering.
- [ ] Complete the real27FEE0 traversal with actual camera and zone state, including child ordering; only then issue280118 publication. Current selection remains blocked at27F888. Numeric A0 alone is insufficient.
- [ ] Join actual callback/pass continuation to acceptance under canonical Bank/camera ownership. This patch does not yet provide that continuation or an accepted runtime record.
- [ ] Implement genuine GPU lowering, registration-order drain and complete renderer/global semantics; only that backend may issue completion. Retain all queue state on failure. Reacquire animated material/image bindings at drain, not from inspection snapshots.
- [ ] Review Bank destruction ordering before activation: queued bridge pins block explicit camera retirement; Bank lifetime itself remains the existing externally owned lifecycle and destruction contract.
- [ ] Run centrally owned build/tests only after integration review. This helper ran none.

RA activation and trigger geometry remain blocked; original AI and authored materials/colors are unchanged. No MSAA/coverage target changes. This deliverable establishes CPU ownership groundwork and source evidence; it does NOT claim a coherent completed runtime draw/drain path or PS2 parity.

Central review: insertion is still unreachable. Before adding a caller, prove Bank and persistent Cables retirement ordering as well as the bridge/display/allocation pins; a queued bridge alone does not own the external Bank. Do not enable acceptance or treat private unissued completion as a real GPU drain.
