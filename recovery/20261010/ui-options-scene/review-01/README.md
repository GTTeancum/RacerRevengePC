# Options external-owner source review01

Layer this candidate over the frozen Options package, never directly over an older active session file. Exact frozen preimage included. No tests/builds/project executions.

Confirmed: cold bind already rejects missing or bound externalTransition before collector/aperture binding; no initial callbacks or profile writes run during cold prepare. External owner remains retained by shared_ptr and manager phase/route comes from that exact object, not a snapshot imported by caller. Root must retain its borrowed callback context until this owner is retired. New destination manager is activated by bindInitialPage only at deferred queue consumption.

Corrections:
- render rejects reentry before invalidating the outer framePublished state.
- prepareUpdateAudio rejects reentry before duplicate-preparation fault, so a nested call during an active callback cannot poison its outer source transaction.
- successful notification/input/audio preparation/accepted-route effect invalidates the previous published draw immediately.
- draw refuses an in-operation, audio-prepared, or foreign-route owner even if an old publication marker remains.
- render with outstanding audio preparation now faults, matching Main's one-use scheduler contract.

Remaining contracts: source proposals require immediate coordinator validation/commit/notify before any next input/update/render/notification. Mutable Options widgets/fire/exit-text resources must belong to one manager lifetime; the host must not bind two sessions to the same mutable resource instance. The external manager callback context is borrowed; its lifetime cannot be established by the page alone. These requirements were already documented; no blanket destination readiness was added.

## Main counterpart

Root asked for the analogous Main audit; exit worker delegated this bounded overlay while it owns new coordinator files. Active Main already rejected render/audio-prepare reentry before mutation. This overlay invalidates stale published draws on input, update (including standalone), notification and accepted route effect; rejects input/proposal-finalization during pending audio preparation; and gates draw on operation/preparation/current external manager identity. Existing Main standalone capture scheduling, source audio order and transition logic are unchanged. Both preimages are the currently integrated session bytes.

Options proposal-current and route-finalization now also revalidate actual profile/application-known, globalFlags228 gate and transportE00, preventing a stale proposal from surviving a changed process-owner gate.
