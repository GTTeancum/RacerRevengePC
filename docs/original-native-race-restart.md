# Native ALL8 restart and return to original Main

This is an explicit native application/navigation policy. The preserved driving route has eight genuine original-AI owners, including camera racer0. It does not adopt the retail zero-human finish tail, alter the native race finish policy, or reconstruct retail restart/finish/replay orchestration.

## User paths

- F5 or the native pause menu's **Restart Race** closes the complete old race, then loads a fresh authored-grid race and the original mode4→7 intro. The loaded track, roster and audio settings stay fixed.
- A race launched from the original Main/Course application also offers **Original Main Menu**. Confirmation closes the entire race before creating the next frontend. The process profile, unlocks, remembered Rider/Course selection, RNG, selector caches, clock and sound-mode owner are retained. The existing GA/POD19 destination admission remains in force on the next Course Confirm.
- Direct GA/GC launches can restart; a direct launch without an original Main application does not advertise an unavailable original-menu destination. Checkpoint restore is still rejected on the ALL8 route. Other retained clients keep their existing menu behavior.

These are real cold reloads and may take time. The code does not reset live racers in place, synthesize completed renderer work, or accept a callback as proof of whole-world teardown. `runSession50DU` must complete cleanup and return; C++ destruction of its world, renderer, camera, audio and bank owners precedes the next call.

## Original camera restoration

The two native control/preference rows are now process-owned. Original176DF0→178670 already stores the actual camera mode11030 and Follow preset ID11038. Restart preserves those fields instead of recreating default5/0.

The original intro still runs. Its completed7C reaches176654..176724 and178700. The one-view branch reads the saved preset ID and passes its address to177AB0. A saved mode0 executes the existing concrete Follow child activation and actual visibility detach/rebind; saved mode5 executes Attached. The reset284550 clears only the live Follow offsets. The mode correction176728..1767C4 skips for common racer state0 with camera0/5. No new blur trigger or timer assignment is added.

The handoff frame then runs only the selected pose child. It must not repeat175D70's common timer prefix. Follow reads the actual racer0 graph, velocity, boost fields and50Y history, executes the real query bridge and same application RNG, and commits completed query side effects. Animated collision candidates are refreshed at the existing world/camera cut when the intro is about to select Follow. The real restored-camera private underlay, GPU retirement and existing fade/history backend are shared with Attached. No displayed history receipt is fabricated.

## Application lifetime details

UI-launched races borrow the same `OriginalUiMainApplication50KG.sharedNoise308030` and `sharedRandomSelectors` across restart and return. Direct original-AI races retain the existing `NoiseLifetime` application stream; their shared selector owner now survives at the same application scope. Non-AI clients keep the prior per-session selector owner. Speech initialization remains exactly once per fresh race; restart must not initialize it both during cold audio preparation and race activation.

The application's original UI LightManager is separate from the native race's canonical gameplay lighting owner. A checked **native gameplay** lease begins only after the real UI-manager release. It admits only the already-proved constructor-absent A4 state, records the native race generation, and prevents another UI-manager entry. It never resets A4, invents a cache miss, or claims retail gameplay cache ownership. The lease ends only after `runSession50DU` has returned and destroyed every race owner. Untracked histories remain permanently rejected.

The first UI WAV uses the requested `RR_AUDIO_WAV` path. Later frontend incarnations add `-menu-session-N`; race output retains its distinct existing `-race[-session-N]` naming. Outputs remain exclusive-create. The process-owned profile is never reconstructed to enable return. Original-UI applications retain the same640×448 native canvas on restart.

## Scope

No finish/replay TV activation, retail pause-menu reconstruction, checkpoint restore, disk save, new manual-driving feature, full retail RNG chronology or complete-track parity is claimed. Existing renderer drains, scene teardown, audio calls and original-AI driving remain real. This change adds no translation units or build-list entries.

Source review only. No build, syntax/import probe, test, asset decoder, guest execution or gameplay run was performed for this candidate.
