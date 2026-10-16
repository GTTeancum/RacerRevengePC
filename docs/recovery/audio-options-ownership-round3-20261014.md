# Audio Options ownership audit — round three, 2026-10-14

Source-only audit of this isolated clone at Git base 79fc223eda2da3d98929c9f3b1253745d17e7c2a plus the preserved first and second handoffs. ROUND3-BASELINE is the immutable before-state. No runtime validation, software checks, parser/decoder execution or preparation was performed. Central test approval remains outside this worker's authority.

## Conclusion

No source-proven defect requiring an owned code change was established in the inspected local-edit, Accept, Cancel, re-entry and cold-race volume paths under their existing serialized ownership contract. This round is documentation-only. This is not a claim of correctness under execution, concurrent access, mutated foreign owners, persistence, or the parent's newer integration.

Audio controls remain groups 2..6 of original screen12/page4 options.dat. The recovered record identifies sound mode and SFX/music/voice/announcer fields; it does not authorize a separate host menu or arbitrary frontend volume policy (recovery/20261010/OPTIONS-GAME-SETTINGS.md:51-61). The ui-options-audio recovery directory contains its README, whose line 5 authenticates event5 BACK. No absent evidence payload was reconstructed or executed.

## Local state and restoration

| Operation | Actual owner and source evidence | Profile/audio consequence |
| --- | --- | --- |
| Enter page | original_ui_game_options_controller50kg.cpp:98-108, entry74 then openingAC | Copies flags, mode and volumes from committed profile into local state; restores remembered group and synchronizes authored choices. |
| Left/right | same file:151-160 | Mutates local values only; volume steps ten, clamps 0..100, uses source clamp/navigation sounds. |
| Defaults | same file:139-140 | Replaces local state with auto-save/vibration on, horizontal split, stereo, 50/100/80/80; synchronizes choices and requests event2. Does not reset application profile or sound owner. |
| Accept intent | same file:132-138 | Emits proposed intent and remembered group, not profile writes. Session must admit and commit it. |
| Cancel intent | same file:132-138; session.cpp:378-389 | Proposes only remembered-group write and page1 route. Local audio values are not applied. |
| Re-entry | controller.cpp:98-108; session.cpp:299-308 | Reloads committed values. Cancel does not need to overwrite the old hidden edit buffer during exit. |

Full file prefixes above are src/game/ for the controller and platform/linux/native/original_ui_options_ for session.cpp. The exact state types are src/game/original_ui_game_options_controller50kg.h:5-24: local state and known application profile are separate objects. Widget commit at src/game/original_ui_options_menu_widgets50kg.cpp:227-246 writes the local plan only after callbacks succeed. A widget callback failure can fault the UI; it is not a profile commit or a general transactional widget rollback.

Profile reset (controller.cpp:52-58) is a separate explicit initialization operation. It preserves unrelated flag bits, does not set known, and does not certify save-file creation. Defaults followed by Cancel retains the prior committed audio values; Defaults followed by successful Accept commits those defaults. These are source-path deductions, not executed scenarios.

## Accept preflight, ordering and rollback

1. Session input (platform/linux/native/original_ui_options_session50kg.cpp:365-376) requires idle input and concrete audio readiness, describes the exact nine-write shape, and retains the complete before-profile, local values and sound-mode revision. No profile mutation occurs here.
2. routeProposalCurrent (:427-444) checks proposal identity/serial, manager state, real selection and exact decision equality. pendingAcceptCurrent (:157-165) rechecks local values, full profile equality, concrete readiness and unchanged sound-mode revision. Input/update/render/notices invalidate pending proposals (:299, :338, :555, :567, :589). A later input resets pendingAcceptCommitted; it does not prevent a subsequent legitimate Accept.
3. The round-one platform bridge (platform/linux/native/original_ui_audio_options50kg.cpp:6-22) requires exact installed mode-owner pointer identity, valid mode fields and agreement with committed profile mode. It does not compare pending local mode to the current mode; changing mode is allowed.
4. The concrete transport preflight (platform/linux/native/original_ui_main_audio50kg.cpp:374-387, read only this round) requires an available mode setter, room for two transport changes and two mixer changes, and SELECT1 readiness. The latter covers possible stop/restart. These are serialized preflights, not reserved handles or synchronization locks. pendingAcceptCurrent does not retain an arbitrary transport/mixer revision snapshot; it rechecks current readiness/epoch through the transport. The sound-owner revision is explicitly retained.
5. commitGameOptionsAccept (session.cpp:445-474) validates the admitted copy and destination again, writes flags/mode/four volumes/remaining flags in source order, calls 188740, then stores remembered group and increments acceptance accounting. src/game/original_ui_game_options_apply50kg.h:18-29 documents the exact stores and explicitly forbids applying its nine writes as one generic batch: 188740 belongs before the final group write.
6. The actual mode setter (platform/linux/native/original_sound_mode50kg.h:29-41) rejects before mutation, computes fields, then publishes fields/revision. If the transport setter returns false, session restores the full saved profile and returns false before remembered-group/accounting writes. No sound-mode rollback is necessary for that specific rejection contract. Do not add callbacks, allocation or foreign mutations between preflight and these writes.
7. src/game/original_ui_transition_owner50kg.cpp:174-184 admits the destination before invoking the commit callback. After callback success, setRequest only publishes requested page/screen/lifecycle and returns success. It does not perform another fallible admission after the settings commit.
8. platform/linux/native/original_ui_frontend50kg.cpp:435-440 dispatches Accept to the private session transaction; Cancel writes only savedGroup6E8. At :760 it immediately notifies the session after the real route request. Session :498-545 verifies the actual lifecycle/profile, consumes the one-use proposal, then issues SELECT1 (or BACK5 for Cancel). The source flush is the transportE00=-1 no-work case, not an invented device drain.

The rollback boundary ends before successful mode application. If later notification/SELECT fails, the already committed profile/mode and published route are not rolled back; the session/coordinator faults. Retrying Accept as though nothing happened would be incorrect. The normal preflight and immediate callback contract is intended to avoid that late failure, but this audit does not establish it through execution.

## Cancel and audio failure boundary

Cancel deliberately remembers the attached group without applying local audio settings. It is not a promise that no profile byte changes. It also does not use Accept's combined mode/SELECT capacity preflight: session :378-389 forms the route, coordinator :440 stores the group, and session :540-541 attempts BACK5 afterwards. Explicit UI mute suppresses effects (:199-202); Accept still requires enabled concrete audio (:151-155), so mute is not an authorization to commit a fictional mode consumer.

The generic canPlayGameOptionsEffect transport method (:531-537 in original_ui_main_audio50kg.cpp) checks clip availability/routing but not revision capacity. Thus a later audio failure need not imply that the remembered group or local edits were rolled back. This is an observable failure-path limitation, not proof that the source ordering should be reversed. No speculative cancellation transaction or new live setter was added.

If parent elects to strengthen native failure admission, the read-only transport owner must first supply a capacity-aware effect gate, for example retain the existing clip mapping and change the return expression to:

```cpp
return canPlayEffectClip(clip) && canRecordControlChange() &&
    mixer_->canRecordControlChanges(2);
```

Its play method would need to honor that gate before stop/restart, and the session would need to preflight BACK5 before proposing/committing Cancel (while retaining explicit mute semantics). This is a coordinated parent hardening proposal, NOT an applied fix or a claim of recovered original rejection behavior. A session-only check using the current generic gate cannot establish full capacity. Do not use the unrelated mode-apply preflight as a BACK proxy.

## Cold race, category gain and ducking

Round two's tests/native/terrain_interactive_drive50du.cpp:275-288 projects only the committed profile. It rejects unknown/out-of-range values and mismatched surviving application sound-mode fields. The projection in src/game/original_ui_audio_options50kg.cpp:4-14 leaves output unchanged on failure and supplies raw integer fields, not category-normalized gains. The frontend may already have been destroyed; the cold race does not require a stale frontend transport or session snapshot.

The host's existing call at :1217 passes that immutable value to liveAudio->prepare. tests/native/live_audio50hf_fixture.h:68-88 validates the settings and calls the actual groups_.initialize(mixer_,audioSettings). platform/linux/native/race_audio_groups50ic.h:17-28 requires a fresh unused mixer/group owner, initializes source groups and retains the configured settings. Providers continue sharing those groups; the same application mode owner binds to the gameplay mixer at host :1227. No double scaling is introduced.

src/game/announcer_callbacks50ic.h:73-82 maps raw values to source category levels: effects/world 0.90, engines 0.85, character 0.80, music 0.70, announcer raw, auxiliary 100, with the existing integer conversion behavior. Initial source writes are :93-107. Begin (:112-126) lowers eligible current levels and records deltas/flags. End (:132-146) restores from current configured settings with the original category factors, clears flags, and leaves deltas unchanged; it does not add deltas back or invert the ducking factor.

The real group owner begin/end methods (race_audio_groups50ic.h:72-82) synchronize those writes into bound mixer groups. Its existing setConfiguredLevels (:64-67) changes only future restoration configuration; it is NOT an immediate live volume setter. This handoff does not call it. The fresh race snapshot therefore supplies both initial levels and later restoration targets without altering active ducking.

Round two's retained inline Course path (:5858-5865) always queues the actual cold request, including unchanged track/rider. This avoids activating pre-edit groups; teardown and request consumption precede fresh preparation (:7624). The ordinary standalone frontend already used that cold boundary. No invented live settings update is needed.

## Parent-owned integration limits

The inspected clone retains round-two ALL8 admission at host :330 and existing native-fleet host-course-driver rejection. Every newly admitted original-profile race therefore requires original AI for all eight racers, including the followed player. No control, driver, renderer, audio mixer or profile persistence code changes this round.

The read-only tests/native/original_frontend_session50la.h still admits a TA human destination via `(allAiEnabled || target->track==platform::HostTrack50IM::TA)`. This conflicts with the round-two race gate and can offer a route that later rejects. Parent's frontend owner should change only that readiness clause to `allAiEnabled` while retaining its other destination/lifetime conditions:

```cpp
return target && courseCatalogPrepared && courseCorpusPresent && raceRequested &&
    !courseColdLoad.pending() && renderer.initialized() && allAiEnabled;
```

Reconcile against the parent's newer source; this worker did not edit that file or original_ui_frontend50kg.cpp. No direct access to the central checkout was made. Persistence/import remains parent-owned and must keep the same gameSettingsProfile/soundMode lifetime. Prior-round build entries remain necessary; this round introduces no compilation unit. Positional surround audibility, device output, actual page rendering, persistence and race behavior remain unverified.
