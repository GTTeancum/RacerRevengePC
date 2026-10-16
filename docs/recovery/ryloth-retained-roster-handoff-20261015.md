# Retained RA roster grid/fresh handoff

Base: d7feccc65e3e4f05f7ea83ca24bd6e61a22f07d3. Isolated, source-reviewed only. No build, syntax check, test, conversion or gameplay was run. Central checkout unchanged.

## Two callable operations

Application-aware definitions in `ra_cold_roster_operation50la.h`; declarations on the aggregate:

```
bool placeRetainedRuntimeGridSlot(Bank&, std::size_t slot, Diagnostic&) noexcept;
bool attachRetainedRuntimeFreshSlot(Bank&, std::size_t slot,
    const FreshRecoveryArenaOwners&, Diagnostic&) noexcept;
```

The existing main translation unit already includes the Operation header. No additional source or build-list entry is required. The only changed files are `tests/native/ra_cold_aggregate50la.h` and `tests/native/ra_cold_roster_operation50la.h`.

## Admission and identity

The private helper first verifies the existing destruction-bound Bank pin before dereferencing Bank. It then locks that same pin, checks its concrete runtime owner and Bank identity, and borrows **that pin's** retained Operation. No Operation, roster array, model identity or completion receipt comes from the caller.

Admission requires the same open aggregate/source/runtime, actual track-particle manager, exact Application shell, entities, domain, motion library and nonzero session generation. It also requires the same retained Resources/navigation lease, real registered arena lanes, all eight completed constructor records and runtime completion records, and zero completed physics steps in every Bank slot. These records are authored only by the real constructor and finishing visits.

The Application shell and registered external arena retain their existing lifetime obligations. A pointer identity comparison does not authorize early destruction. The existing Application domain getter is rechecked inside exception handling; this is an identity check of the already constructed domain, not permission to substitute a new domain.

## Order and actual algorithms

All eight constructor operations must finish before grid slot 0. Grid proceeds 0 through 7. Every preceding grid must have its real completion record, the current/later slots must be unattempted, and no fresh attempt may exist. The handoff assembles GridArenaOwners only from the Operation's stable bindings, parameters, AI, history and adopted Rig array, then calls the existing `placeGrid` directly. That algorithm owns its Bank visit.

Fresh proceeds 0 through 7 only after all eight real grid completions. Preceding fresh operations must have completed; current/later slots must be unattempted. The aggregate supplies its same retained lighting owner. The caller supplies only the genuine external recovery endpoint vectors, metadata array and Type3QuerySource that the existing `recoveryLanesRegistered` and `attachFresh` validate and retain. These are absent from the current Operation Arena, so no replacement or synthesized endpoint storage is created.

The original membership algorithms and source order are unchanged: real grid/terrain/main/secondary placement, then fresh vehicle visibility 7F0 and actual recovery/movement memberships. Visibility and collision namespaces remain separate.

The previous raw grid/fresh entry points reject a retained-roster route before invoking either algorithm. Existing raw-borrow routes remain unchanged. This prevents an unrelated caller array or skipped completion/order check from bypassing the new handoff.

## Failure and retirement

Any handoff rejection is terminal for forward progress. It invalidates the Operation's completion array, runtime roster completion, grid/fresh completion and aggregate state while preserving actual attempted flags, unbanked retirement obligations, Bank pin, source, model and query backing. Nested original diagnostics are retained. No partial Bank effects are rolled back.

Raw-entry bypass attempts also invalidate runtime completion and mark the aggregate/runtime terminal. Either retirement entry invalidates runtime roster completion immediately, including an otherwise no-op retirement, so retirement cannot be followed by a new forward handoff. Existing cleanup remains callable after terminal failure and does not require successful constructor/grid/fresh completion proof.

Cleanup remains actual camera/Follow retirement if attempted, fresh membership retirement, grid membership retirement, checked Operation model close while Bank lives, then actual Bank destruction. The arena must also survive final aggregate pod-context unbinding. Incomplete cleanup keeps the existing ownership and retry obligations; no caller-supplied drained flag is introduced.

## Boundary

No new manager, clock, mutable owner getter, callback, Ready receipt, source phase or camera is introduced. This patch does not complete model resources, populate a collector, run neutral physics, install Countdown, prepare a camera or admit RA gameplay. The unresolved trigger input history and wreck snapshot retirement constraints are unchanged.
