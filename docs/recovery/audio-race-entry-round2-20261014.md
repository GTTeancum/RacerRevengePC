# Original audio profile at race entry — round two

Base Git commit: 79fc223eda2da3d98929c9f3b1253745d17e7c2a. This delta is relative to ROUND2-BASELINE, which includes the first handoff. Source only; no checks or execution.

## Concrete application

At the beginning of runSession50DU, an original-UI-owned session now reads the sole committed application gameSettingsProfile through readOriginalUiAudioOptions50KG. Unknown/invalid profiles fail closed. Its sound mode must agree with the initialized application sound-mode owner. The resulting four integer levels become loadedAudioSettings, the existing input to liveAudio->prepare. A direct-host session retains its existing effectiveAudioSettings path.

The actual owner already calls groups_.initialize(mixer_, audioSettings) during cold preparation. That path initializes the original category volumes once and retains the same configuration for announcer restoration. All providers continue borrowing the same group and announcer owners. No live setter, new mixer, master gain multiplier or ducking reset was added. The same application sound-mode owner is subsequently bound to the real gameplay mixer by the existing source path.

Every Course launch through the retained in-session UI path now publishes the existing typed cold-load request, including unchanged track/rider selections. The old same-selection shortcut could activate groups prepared before Options edits. The ordinary standalone frontend already uses this cold boundary. Real cleanup and request consumption precede the next runSession50DU and fresh audio-owner preparation.

The original-UI race path requires RR_ORIGINAL_AI_ALL_RACERS=1. This explicitly bounds the handoff to all eight original AI, including the followed racer. Existing native-fleet checks reject RR_AUTODRIVE=1 and prevent host-driver calls. No AI, input packet, camera, or profile-role logic is replaced.

## Source evidence

- tests/native/terrain_interactive_drive50du.cpp:275: committed-profile snapshot; :330: ALL8 admission; :1217: actual liveAudio->prepare consumer; :1227: existing shared mode binding; :5858: same-selection cold request; :7624: existing application request consumption.
- tests/native/live_audio50hf_fixture.h:68-88: cold-only owner preparation validates configuration and initializes the actual groups.
- platform/linux/native/race_audio_groups50ic.h:17-28: initialization requires an unused mixer/group owner and stores configuration for restoration.
- src/game/announcer_callbacks50ic.h:73-82: existing source category factors; :93-107: original initial group writes at 1A7928/1A7970/1A79BC/1A7A04/1A7A4C/1A7A84/1A7A98. Raw profile values must not be scaled a second time.
- src/game/original_ui_audio_options50kg.cpp:4-14 (round one): committed sound/volume mapping from 1F6744/1F6760/1F677C/1F6798/1F67B4; does not read page-local edits.
- recovery/20261010/OPTIONS-GAME-SETTINGS.md:51-61: authored audio controls, profile fields, Accept and Cancel distinction.

## Integration and limits

Parent must retain round one's source/build registration and reconcile this narrow host-file delta with its newer tree. No additional translation units were introduced this round. Application profile import/export and the cloud-owned frontend remain unchanged. The standalone frontend currently also admits a TA human route; this worker's race boundary rejects it. Parent/frontend owner should reconcile admission with this task's ALL8-only requirement before exposing that route. This is an admission limitation, not a synthetic replacement controller.

The legacy inline-UI path now takes the already-established full cold rebuild, so its former same-context course_entry logging no longer occurs. The active standalone path was already cold and keeps its existing flow. Host audio_requested diagnostics still refer to the host-menu selection; audio_effective and liveAudio's configuration reflect the committed profile used for the race.

All runtime rendering/audio, muted-output behavior, profile persistence, gameplay and race-entry acceptance are unverified. Parent owns all tests. Native source uses host CPU/GPU only; no guest code or preparation was executed.
