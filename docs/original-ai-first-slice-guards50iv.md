# Original-AI first slice: terminal world/session guards

This overlay was incorporated into guarded source `93201627`, which compiled, linked and passed one bounded direct-GC observation on 2026-10-10. See [current validation](current-validation.md). That successful prefix did not trigger a failed requisite frame, recovery or session rebuild, so those terminal branches remain runtime-unverified. The original overlay and its frozen predecessor remain in Git history.

## Corrected world ownership boundary

Every actual racer, including the HOST-controlled human and opponents, is a requisite input to the optional original-AI manager. Bank::visit preserves a failed opponent's partial owner state and restores the player arena; Bank::step marks that opponent inactive. The previous caller swallowed the failure, continued the world, and unconditionally ran native manager timing/rank on all slots. That could advance CB8 from an abandoned native frame and later observe incomplete state.

The optional joined path now requires every actual bank slot to be active and at VehicleFramePhase49W::Ready:

- Before any fleet observation, so no rank input is written from an incomplete field.
- Before beginLapManagerClock50ID, which mutates the shared clock before the per-racer timing pass.
- Immediately after each opponent restores opponentFrame=false, outside the existing per-opponent exception catch. A failed slot stops the whole joined world before later racers, world animations, contacts, timing or rank.
- A human frame returning false also terminates the optional world immediately.

The guard inspects every slot. It does not omit failed racers, manufacture Ready, clear a source diagnostic, reset a pose, retry the abandoned frame or advance manager time. The retained AI source boundary and native frame source stage/phase are reported. Successful earlier per-racer work in the interrupted world is not rolled back; the process stops rather than admitting that partial world.

These guards are native admission rules for the declared accepted-world scheduling seam. They do not claim to reconstruct the original application root scheduler.

## Mandatory actual contact detection

RR_ORIGINAL_AI_FIRST_SLICE=1 now rejects RR_DISABLE_RACER_CONTACTS=1 during cold admission. The actual dynamic-contact detection and pre-commit 223110 owner boundary remain mandatory for this slice. No contact miss or disabled pair substitutes for that boundary.

## One cold session

The optional slice rejects automatic portal, damage and undertrack host recovery at their retained source boundary, before preparing a checkpoint. Human failures escape directly; opponent exceptions are restored by Bank and then rejected by the outside-catch fleet guard. No source state is reconstructed to continue the session.

An outer-loop guard additionally rejects every non-Quit SessionExit before changing effective settings, incrementing sessionIndex or beginning another cold run. This includes explicit restart/checkpoint commands and future restart paths. Run a new process for a new first-slice attempt. Use RR_START_MENU=0 for acceptance (already the headless default): the existing interactive front-menu Start returns Restart and is also rejected by this deliberately strict guard. No unreviewed exception for Start is introduced.

Diagnostic prefixes:

- ORIGINAL AI world boundary: failed slot, activity/phase and existing diagnostic source values.
- ORIGINAL AI source-recovery boundary: portal/damage/undertrack boundary; no checkpoint prepared.
- ORIGINAL AI session-rebuild boundary: rejected exit and session index.

The existing terminal exception handler returns exit code 1. Ordinary Quit remains successful. With the optional feature disabled, the existing HOST stopped-opponent and recovery/restart policies are unchanged.

## Static review references and remaining limits

- Native Bank::visit/step: tests/native/native_racer_bank50hn_fixture.h, preserved partial state and inactive-slot semantics.
- Original-AI failure: driveOriginalAi50IV retains sourceBoundary and returns at PostPlayer or the genuine reached source cut; the caller must not consume this as a completed frame.
- Manager mutation: beginLapManagerClock50ID precedes beginRacerLapTiming50ID and native rank in the declared accepted-world seam.
- Packing/repair caller return values are intentionally unused: original16E9E8 calls171C60;16E9F4 calls1734C0;16E9FC immediately loadsFE8. No helper return-result branch occurs.
- No wrongWayTimer1094 update was invented. The recovered AI constructor/reset clear it and173C6C starts it with the prepared factor when zero. No additional AI increment writer was established by this review; an apparently inert source timer is not evidence permitting a fabricated update.

All existing combat, route-choice, reset, lap/finish, dynamic-contact and incomplete-world limitations remain. Bounded runtime acceptance belongs to the parent integration step: verify startup4 then genuine CB8>5/state2 only if no earlier exact source boundary occurs; verify a failed requisite frame cannot advance the manager or start another session. The successful bounded run observed startup 4→2; failure-trigger, recovery and second-session rejection were not exercised.
