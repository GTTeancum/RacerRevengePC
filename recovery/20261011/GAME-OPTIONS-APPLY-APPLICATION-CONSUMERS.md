# Existing native Game Options consumers and remaining handoffs

Static source inspection, October 11, 2026. Paths are relative to the active
repository. None of these findings claim a new gameplay destination or runtime
verification. The patch changes the original frontend profile transaction only.

## Four volume categories

The real native race mixer already consumes source group gains:

- `src/game/announcer_callbacks50ic.h`: `RaceAudioSettings50IC` is the four
  integer0..100 source fields11020/24/28/2C. `initializeRaceAudioGroups50IC`
  recovers1A78E4..1A7A9C. Initial/restored group levels are effects90% of SFX,
  engines85% of SFX, world90% of SFX, music70% of music, character80% of voice,
  announcer100% of announcer, and auxiliary100. Binary32 multiplication followed
  by integer truncation is already retained. Begin/end announcer ducking uses
  the actual category group state and current configured restore targets.
- `platform/linux/native/race_audio_groups50ic.h`: `RaceAudioGroups50IC`
  initializes and binds actual voice ranges, retains the real mixer epoch,
  installs group levels through `AudioMixer50HD::setVoiceGroupGain50IC`, and
  propagates recovered ducking writes. `setConfiguredLevels` changes the later
  restore target only; it must not be treated as an immediate whole-mixer apply.
- `platform/linux/native/audio_mixer50hd.cpp`: `mixStereo16` multiplies each
  native voice's samples by its actual retained group gain exactly once.
- `tests/native/race_audio_groups50ic_fixture.h`: explicit source-proven provider
  bindings cover effects, material collisions, countdown, engines and Dax.
  Gameplay catalog binding maps curses/taunts/specials to Character11C.
- `tests/native/race_music50ie_fixture.h`: the real prepared music voice is bound
  to Music118. The concrete speech/ducking bridge is
  `platform/linux/native/announcer_episode50ic.h`.
- `tests/native/live_audio50hf_fixture.h::Owner::prepare` receives an effective
  `RaceAudioSettings50IC`, initializes its actual mixer/groups, and retains those
  configured values. `tests/native/terrain_interactive_drive50du.cpp` currently
  passes its separate host session's immutable `loadedAudioSettings`.

Required future handoff: when an original UI Course/race destination genuinely
exists, derive those four race values from the same committed
`OriginalUiGameSettingsProfile50KG`, initialize the actual new race group owner
at the recovered setup boundary, and retain that shared profile for any later
configured-level reads. Close the frontend PCM output before the race output,
and bind the same `OriginalSoundMode50KG` to the actual new race mixer. Merely
copying a mode enum or routing to the separate host restart/menu path does not
establish that handoff. This candidate performs none of those unadmitted steps.

## Vibration

`src/game/impact_consumers49z.cpp::consumeImpacts49Z` has the concrete source
profile11050 bit2 gate. For a qualifying local-human lateral impact it produces
controller virtual+40/+44 requests at222D8C/222DEC (or the earlier222D70 binding
boundary). `ImpactConsumerInput49Z::profileFlags11050` is the input for the exact
committed profile byte.

Current `terrain_interactive_drive50du.cpp` supplies explicit fresh bit2=4 to its
separate dynamic-contact path, sets `stopBeforeControllerFeedback50IW`, and stops
at the authentic pending boundary because actual vehicle5C/controller registry
and feedback ownership are not joined. `device_held48y.h` likewise explicitly
excludes rumble. There is no evidence of a functioning native rumble backend.

Required future handoff: feed the same committed profile byte into the real race
consumer, resolve the actual player's controller/backend and complete those
source virtual calls at their reached boundaries. No success-looking callback,
keyboard feedback substitute or immediate rumble on Accept is justified.

## Auto-save

The frontend now commits11050 bit0 and page re-entry reads it. The source Accept
routine does not save anything. Inspection of current `src/game` and
`platform/linux/native` found no implemented original profile-save/memory-card
transaction owner consuming that bit. Host checkpoint fixtures are not a retail
save owner and are not an authorized substitute.

Required future work: reconstruct the actual source save triggers, profile data
serialization and storage/error lifecycle, then consume the same profile bit
at those boundaries. No save-file persistence or memory-card readiness is claimed.

## Split screen

The frontend now commits11050 bit1 and page re-entry reads it. Accept contains no
viewport or camera operation. No native original two-human horizontal/vertical
viewport/camera owner consuming this byte was found in current game/platform
sources. The existing native host view is one chase camera, and the original
UI application does not admit a two-player race destination.

Required future work: reconstruct the real two-player route/setup and viewport
consumer, sharing the committed profile byte. Do not synthesize a split layout
on the Options page or equate the profile bit with implemented two-player play.

## Frontend sound mode

Unlike the other fields above,188740 is called immediately by1F66F0 and therefore
must have a concrete current consumer before Accept. This candidate joins the
already installed `OriginalSoundMode50KG` to the actual frontend transport/mixer
and commits it in source order. Centered mono/stereo/surround calls are supported.
The retained downstream nonzero surround packet-selector consumer remains
unrecovered, so positional surround requests still reject without fake effects.
