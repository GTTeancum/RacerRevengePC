# Native Options sound-mode consumer, source-only candidate

Frozen candidate prepared October 10–11, 2026. The active repository was read
only. No build, test, syntax check, sanitizer, project parser, PCM decoder,
gameplay, emulator, guest instruction, or IOP device was executed. The small
word-to-text helper only inspected bounded original ELF instruction regions.
Root owns integration and all verification.

## Result and hard boundary

The three original profile modes now have a real shared native consumer in the
staged mixer, plus cold binding through the actual frontend AudioOutput owner.
The existing frontend's actual centered play/controls calls read that same
owner and publish their admitted source routing to the PCM mixer. The output
continues to send its existing one mixed buffer to WAV and/or SDL. No separate
shadow mixer, replacement audio, output-wide phase effect, channel collapse,
new device, or volume-group owner is introduced.

This is bounded spatial-mode application, not complete spatial surround.
The retained original source yields packet selectors 0, 1, 'L' (76), and
'R' (82). Its downstream phase-selector consumer is absent from the retained
inputs. The native candidate accepts only selector 0. It rejects ALL positional
surround requests, including selector 1 in the front quadrants, before touching
voices or gains. Centered surround produces selector 0 and is concretely
supported. Mono suppresses positional panning. Stereo supports the recovered
integer angle law. No claim of rear-channel or hardware surround playback is
made. Present frontend audio is centered, so changing this source mode may
legitimately leave its PCM unchanged. It would be incorrect to manufacture an
audible difference by collapsing its stereo music or phase-inverting everything.

The shared source setters do not refresh existing channel gains. Apply188740
preserves currently playing samples, coordinates, group levels, and device
queues. Later play/controls read the current mode. The candidate preserves this
source timing rather than remapping active voices at every mix.

## Deliverable

`files/` contains seven files:

- New `src/game/original_sound_mode50kg.h`: exact source enum/field projection,
  bounded source angle orientation, and integer channel/selector projection.
- New `platform/linux/native/original_sound_mode50kg.h`: stable shared application
  owner, explicit profile initialization, concrete mixer-bound preflight/commit.
- Overlays for `platform/linux/native/audio_mixer50hd.{h,cpp}`: cold attachment,
  centered existing-call consumer, explicit original engine-angle controls,
  per-voice routing actually consumed by PCM mixing, and strict rejection.
- Overlays for `platform/linux/native/original_ui_main_audio50kg.{h,cpp}`: exact
  active transport capability and apply hooks, retaining its mixer-epoch guard.
- Overlay for `tests/native/original_ui_main_output50kg.h`: optional final cold
  prepare argument for the shared mode owner, and closed/muted-safe wrappers.

`preimages/` contains the exact five replaced files. The audio pair was copied
AFTER the Game Options Event2 overlay was integrated; do not replace it with
an older six-voice source. No new translation unit or build-list edit is needed.

## Integration contract

1. The application owns `shared_ptr<platform::OriginalSoundMode50KG>` beside its
   actual `gameSettingsProfile`. Initialize it once with
   `initializeFromProfile1101C(gameSettingsProfile->soundMode1101C)` only after
   that profile's imported/explicit-fresh lifecycle has established a value.
   Initialization alone never grants Accept capability.
2. Pass the SAME shared pointer as the final argument to frontend output's
   `prepare(audioRoot, request, error, diagnostic, soundMode)`. The output binds
   it to its dedicated actual mixer before preparing its real PCM transport.
   Existing callers omitting this optional argument retain previous behavior
   and cannot certify Apply188740.
3. Coordinator already holds the actual transport from `AudioOutput::transport()`.
   Verify `audio->soundModeOwner50KG() == application.soundMode.get()` and
   `audio->canApplySoundMode18874050KG(selectedMode)` during the complete route
   preflight. A stale mixer epoch, missing attachment, reset transport, pause,
   invalid enum, or exhausted revision denies capability. The concrete output
   additionally exposes the same methods gated by enabled/not-closed state.
4. In the serialized source transaction, commit source1F66F0 profile fields in
   source order, then invoke `audio->applySoundMode18874050KG(selectedMode)` for
   the original188740 effect. Preflight all route/profile/audio work first;
   there must be no reentrant callback or intervening mutation. Then commit
   group6E8, request page1, and emit SELECT1/flush in their original order.
   This package does not itself implement the route/profile transaction.
5. Muted frontend has no output or PCM owner and must not claim this concrete
   audio capability. Explicitly retain unavailable Accept unless another real
   authorized application consumer is joined. Do not pass a Boolean promise.
6. A future race mixer must cold-bind the SAME retained owner when it actually
   replaces frontend output. Merely copying its enum into race settings does
   not establish this handoff. Existing unrelated race mixers stay unchanged.

The low-level owner exposes `canApply188740(mixer, mode)` and
`apply188740(mixer, mode)` for concrete consumers outside the frontend. Neither
accepts a callback or readiness Boolean. It verifies exact owner identity on
an initialized mixer, but caller still owns proving that mixer is the actual
application consumer. The frontend transport/output wrappers establish that
identity for the existing frontend graph.

## Source evidence

Original ELF:
`native-assets-prepared-01/work/native/project/disc/SLUS_202.68`
SHA256 previously authenticated by existing recovery:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The inspected LOAD segment maps `fileOffset = VA - 0x100000 + 0x80`.
Static instruction listings are under `evidence/`.

-188740..18885C reads profile1101C. Mono0 writes managerE38=1, E34=0,
  then232B60(0). Stereo1 writes0/0 then232B60(1). Surround2 writes0/1
  then232B60(1). Invalid enums have no source effect; native rejects them.
-232D30 and232D20 are plain source stores.232B60(0/1) sets9918=0 and
  writes9900 to its argument. Profile surround2 is NOT outputMode9900=2.
  That separate source setter value2 would set reverseAngle9918=1 and is not
  requested by188740.
-The manager embeds its sound engine at+40; E38/E34 are engineDF8/DF4.
  Concrete reads are26C344/26CC14 (mono) and26C37C/26CC84 (surround).
-26CBA0 combined controls normalize engine angles0 and360, and all mono
  angles, to the non-positional-1 branch.26CE70..26CE7C sets both output
  percentages to the requested volume for this branch.
-The positional integer projection26CC84..26CE6C uses division by90 in
  front quadrants and surround rear quadrants; ordinary stereo rear uses
  `(angle-91)/89` or `(269-angle)/89`. Angle180 has both levels unchanged;
  surround requests selector82. Surround rear-right requests76, rear-left82;
  front positional surround retains selector1. The native consumer deliberately
  rejects every nonzero selector rather than guessing its missing downstream
  implementation.
-233950..233990 negates the source angle when9918=0, adds359 if negative,
  then stores it. This is not conventional modulo360 normalization. The pure
  helper preserves it on the bounded source domain[-360,360].
-26C2F0 standalone pan returns immediately in mono, with no channel refresh.
  It is retained as evidence but NOT claimed to be the combined-controls API.

## Native mixing limits and preservation

The explicit mixer angle API receives an already projected ENGINE angle. It
uses the source integer projection at volume100, then the existing native
voice/group/master gain layers. This preserves current native gain ownership;
it does not claim equivalence to arbitrary original integer-volume rounding.
The pure source helper accepts an explicit integer volume0..100 and projects
that source arithmetic directly. The existing frontend uses only centered
routing, where the distinction has no panning-rounding effect.

While attached, ordinary nonzero host-pan calls reject: a[-1,1] scalar cannot
prove the source front/rear azimuth. Unattached legacy mixers retain their
previous pan/gain behavior. Looping, clip validation, voice/group ownership,
quantization, sample clock, queue, and capture code are unchanged. Mixer
reinitialize drops mode attachment; frontend's existing epoch guard then
invalidates its old transport. Mode owner and shared_ptr remain cold-lifetime
objects; hot play/control/mix/apply work is bounded and does not allocate.

## Root-owned verification still needed

Compile/integration; initialize and bind failures; invalid modes; profile
commit/Cancel behavior; shared-owner identity and stale epochs; muted/closed
output denial; exact centered PCM preservation; mono/stereo angle arithmetic;
all nonzero surround selector rejection without partial mutation; active voice
phase/gain preservation across a mode write; group gain preservation; mode
application before the actual Accept SELECT cue; and frontend update/output
scheduling. None of these were executed by this worker.
