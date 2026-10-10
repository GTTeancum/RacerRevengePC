# 50HI: retained material collision and drive-on audio

## Concrete result and join

This supplies a bounded native host subscriber for the selected player's
material collisions and scrapes. It does not install a fictional original D40
owner or assert an original virtual callback has completed. Keep the existing
canonical effectD40 absent. Subscribe at the same post-223540 phase using the
actual timer830, main spring currentCandidate130, source speedRatio70 and the
same `s.noise.sharedRandomState` used by all existing source RNG consumers.

New implementation:

- `src/assets/material_audio50hi.{h,cpp}`: actual AUB catalog and SCB contact A9
- `src/game/material_audio50hi.{h,cpp}`: bounded stateful event/control adapter
- `tests/native/material_audio_provider50hi_fixture.h`: real original PCM provider
- `tests/native/material_audio50hi_check.cpp`: focused verification

Cold prepare a stable `material_audio50HI::Provider` with
`prepare(discRoot, audioRoot, mixer, 64, error)`. Its 16 dedicated sound slots
are 64–71 collision and 72–79 drive-on, leaving 80–95 unclaimed. The owner
retains 13 unique decoded clips because collision keys1–4 share pod_coll.vag;
all16 authored sound identities remain separate, including duplicate-file
collision owners. It owns its bank, so never move/copy it or resize its storage.
The mixer must stop its voices before that bank is destroyed.

For each retained collider, call `readTrackContactMaterial50HI` with its actual
world, zone, ordinal and the same native identity used by the physics query.
Resolve the current main spring's `records.currentCandidate130` against these
bindings with `resolveContactMaterial50HI`; zero returns -1 (no contact), an
unresolved nonzero identity fails instead of choosing a default material.

At the post-effect phase, prepare `MaterialAudioInput50HI`:

- channelArgument = timer820[4] > 0 ? 2 : 3 (the actual timer830)
- contactMaterialA9 = resolved contact A9
- speedRatio70 = current source-owned speed ratio
- listenerDistance = actual nearest listener planar distance, including camera offset
- vehicle220Present = actual native vehicle ownership
- disabled24C = source D44 disable state, fresh constructor false

Call `Provider::update(input, s.noise.sharedRandomState)`. It returns genuine
mixer results for each command, including explicit unavailable PCM and inactive
sound-owner results. No temporary file access or allocation occurs in update.
Reset only alongside an actual host audio reset, using `Provider::reset()`.

## Static original-source evidence

Original SLUS SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
TA.RES SHA-256:
`b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.
TA.aub SHA-256:
`7474707d4182f29cc8b24203680aa311c9749e745078f3d80545f93f8d97f017`.
Both original archive and AUB identities are checked by the cold provider.

- 224130 /224148: original50B timer830 selects ChannelA8 argument2/3.
- 1A0120 ->179070: material route; argument4 belongs to a different boost owner.
- 1790D4..1790EC: actual vehicle220 ->main354 ->currentCandidate130 ->A9.
- 1B5430..1B54F0: track collision authoring reader loads MaterialID with an
  explicit source default1. 1B56F4..1B570C copies positive object184 to A9.
  This is the same track-specific creator already used by the native
  classifyTrackCollision50AN boundary. Every one of the37 retained TA collider
  SCB instances lacks an override, so actual source default1 is authenticated.
  It is not a guessed fallback, nor the independent friction material4C.
- 179154..1791D8: argument2 sets emitter22C bit3 on its first contact, calls
  2C2240 once on onset, then calls 2C2530 every reached tick. No contact skips
  these operations, including the bit store. It does not synthesize a stop.
- 1791F0..179220: argument3 clears bit3 and stops drive-on D44 virtualA8.
- 2C2240: disabled24C and nearest-player XZ distance<540 gates; source shared
  RNG selects1..4, collision key =4*A9+variant. Positional bound is400+speed,
  speed=trunc(100*vehicle70). Playback volume clamps to20..100. Source event3
  is emitted for a player collision, but its current source dispatcher is a
  genuine no-op, so no wall-hit speech is invented here.
- 2C2530: preserve chosen drive key while material unchanged; otherwise stop
  old drive, draw a new1..4 variant, store material240/key244, then start.
  Source positional bound450; minimum control speed50.
- 2C27D0: stop and restore material240/key244=-1.
- 293010: lazy cached integer pitch slope, source RNG pitch variation, original
  piecewise volume arithmetic and integer truncation. Host base pitch4096 is
  explicitly the native provider's representation, not fabricated PS2 metadata.
- 308030: shared raw32 LCG, state*0x41C64E6D+0x3039, preserve raw32 and return
  state&0x7FFFFFFF. A collision+fresh scrape tick consumes three draws; retained
  scrape controls consume one. No new RNG is seeded.

TA.aub supplies collision keys1–4 pod_coll,5–8 rockhit1..4; drive keys1–4
podscrp1..4,5–8 rckscrp1..4. All drive rows have sample speed100, minimum
volume0, minimum speed1, maximum speed60, resonant speed100, slope0.75.
These are read from the original authoring, not added numeric tuning.

## Bounded limitations

The current join uses the actual camera/listener planar distance for the
source540/450/400+speed gates. 104140 ->102950 computes spatial intensity, and
233AE0 initially writes volume44=trunc(volume38*intensity)/100. However, the
material start calls233410 with flags0, which selects rawvolume38 and copies
it over44 at233550..233564. Continued292F30 ends with virtual2C=233750, which
overwrites44 with raw material control volume again. The native final gain
therefore preserves this exact ordering: source volume/100, with no invented
lasting attenuation factor. The engine-only0.45 multiplier does not apply.
Pan is centered in this host subscriber; original changing pan and Doppler
remain unavailable. Source far-distance>=540 skip is retained. This does not
claim complete 3D audio,
soul-tree ambience, destruction, opponent collision events, wall-hit speech,
or a reconstructed complete D40/D44 object registry. Current native49Z impact
records are controller rumble and are never used as sound events.

Generated commands do not mean a clip played. Missing decoded PCM remains an
Unavailable result. Active collision owners are not restarted. Driver loops
use the original VAG embedded loop when present and otherwise the declared
source loop flag. There are no substitute WAVs, guessed impact thresholds,
guest execution, ee.bin, or recorded-runtime-state dependencies.

## Verification

Focused normal check:122 assertions, all13 original PCM clips ready,16 source
sound rows,37 real collider bindings, zero watched hot allocations,18,254
nonzero mixed samples over12*768 frames. Covers onset/control/stop, source lazy
no-contact behavior, source disable/missing vehicle gates, unknown material
failure, transactional output capacity failure, exact shared-RNG progression,
and missing-audio negative controls. This is not a full-game regression or a
human listening test. ASan/UBSan final-code results are recorded in the
adjacent evidence receipt. LeakSanitizer cannot run under this executor's
ptrace supervision; its failed attempt is retained separately, and the ASan/
UBSan run uses detect_leaks=0. No leak-check pass is claimed.

Compact evidence is preserved at the absolute workspace directory
`/workspace/scratch/74cd1d3d7a45/solo-audio50hi/`.
