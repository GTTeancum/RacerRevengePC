# Category6 last-lap and finish music50IE

Integrated native source, 2026-10-09. The Linux mode1 race observed actual
last-lap song29 followed by finish song27 through the shared mixer. The run
completed normally with 174.032 seconds of PCM. Other selected songs, pause,
restart and adverse-provider paths remain unverified. See
[current validation](current-validation.md) for source identity and scope.
Original IOP streaming and fade-envelope parity remain excluded.

## Reconstructed source boundary

The bounded source owner supplies the six1A46E0 calls emitted by category6:

-26:Music\Race\Finish\bad_fin.vag
-27:Music\Race\Finish\best_fin.vag
-28:Music\Race\Finish\good_fin.vag
-29:Music\Race\FinalLap\fin_lap2.vag
-33:Music\Race\FinalLap\fin_lap3.vag
-34:Music\Race\FinalLap\fin_lap1.vag

Keys are exact Song entries in authenticatedRACEMUS.JUK and itsRACEMUS.JUB
companion, not filename-to-event guesses.1A3C50..60 passes the shared manager's
+118 music group to2C0980; this binds exactly native slot117 toMusic118.
No other native slot is reserved, rebound, stopped or started by this owner.

1A46E0 calls admission1A4900 before any source writes. Non-mode1 admits only44/45,
so each of the six bounded requests is genuinely suppressed in another mode.
For mode1, owner12C=2 admits finish26..28 and suppresses final-lap29/33/34;
owner12C=3 does the converse; owner12C=4 suppresses both. The remaining ordinary
owner states admit both. No missing PCM or host/backend failure is called a
source suppression.

Final-lap selection is the upstream category6 owner's shared-RNG result. This
owner performs no draw, reseeding, round robin, or fallback selection.
1A480C..1A4830 resets128, calls1A4AB0 to stop current music, sets12C=2, then
calls234900(key,1).234900 sets transport40=4;2341AC..1C4 translates that state
to the true loop argument of269650. The native transport therefore loops the
actual selected clip. It does not invent a short one-shot last-lap stinger.

Finish calls haveA2=0.1A48A8 first writes130=1, then1A4AB0 overwrites130=-1
and12C=0 while stopping.1A48C4 sets12C=1 before234880(key,1).234880 sets
transport40=1 and requests a non-looping clip. A finish stops the last-lap loop
before starting the actual finish track.234170/2343C0's successful nested
native start returns1; real source admission suppression returns0. The upstream
category6 helper ignores this return, so its acknowledgement must explicitly use0.
The receipt preserves the nested result separately for diagnostics.

## Exact retained stereo bytes

Every selected asset is present. These six files have a0x7E0-byte zero prefix
except the conventional big-endian per-channel payload length and22050Hz rate
at offsets0x0C and0x10. Beginning0x7E0,32-byte pairs hold one16-byte PS-ADPCM
frame for each channel. The total file length is0x7E0+2*declaredChannelBytes.
Flags are0 in both channels until the penultimate pair, whose flags are1 in
both channels. One original070077... padding pair follows. These identities,
exact offsets and raw bytes are in the selected-original-assets evidence.

The native cold loader authenticates the complete executable, both metadata
files and each selected sample before interpreting this narrow layout. It
reframes the original channel bytes into two50HC mono buffers and invokes the
existing native PS-ADPCM implementation, then interleaves the two decoded PCM
channels. No invented samples, fallback music, discarded right channel, generic
stereo heuristic or relaxation of50HC's existing mono contract is used.
The final padding frame is not decoded past the source end flag, consistent
with the existing mono contract.

Expected decoded source frames, derived from raw end-marker positions:

|Song|Frames/channel|Native behavior|
|---|---:|---|
|26|83,244|one shot|
|27|203,924|one shot|
|28|123,424|one shot|
|29|622,440|full-clip loop|
|33|522,396|full-clip loop|
|34|507,724|full-clip loop|

All clips are22050Hz signed16-bit interleaved stereo. Retained PCM is exactly
8,252,608 bytes if all six decode, below the8MiB owner limit. The8MiB encoded
read budget includes source authentication and all six selected files. Temporary
cold buffers, hash working storage, and per-channel decode buffers are separate
from retained PCM. There is no hot allocation, read, decode, or output creation.
Native loop bounds are[0,frames), in source frames, not byte counts or scalar
stereo-sample counts. No file has an embedded loop flag; looping is the source
transport decision. RACEMUS Pitch1881 is an original SPU-unit value and is
never passed as a native float pitch ratio. Native playback uses the actual
header rate and a unity ratio through the existing48000Hz mixer.

The same16-byte interleave and0x7E0 data start also appear in the public
Rainbow Studios VIG layout documentation in
https://github.com/vgmstream/vgmstream/blob/master/src/meta/vag.c.
That reference is corroborative only: its standardVAG magic differs here.
Admission depends on the actual six authenticated originals and their recorded
bytes, not on that public format detector or a copied external decoder.

## Host integration

Constructrace_music50IE::Owner after the stablelive_audio50HF::Owner so the
mixer/groups outlive it. Prepare it once with the same explicitdiscRoot,
audioRoot, mixerForMaterialOwner() andgroupsForProviders(). Its independent bank
is immutable after preparation. The slot117 binding rejects an already active
or already bound slot, including another music owner.

ForMusic1A46E0, validate the upstream kind/call contract, then call
request(argumentA1,argumentA2,actualMode11258). OnlyReceipt.completed() authorizes
the upstream ignored-return acknowledgement0. Unprepared, paused, unsupported,
invalid, unavailable, ownership or backend rejection leaves the exact FIFO
request pending. Missing or invalid selected clips retain named diagnostic
records; they never produce a successful start or a fallback clip. Cold prepare
may finish with unavailable records, so default-mode1 readiness must require
bank().ready==6 andbank().missing/invalid/unsupported==0, plus actual output.

CallpollCompletion() after each actual committed mix block, even when an
output write fails after a mix, and before event dispatch. It observes the real
mixer voice rather than predicting completion using a wall clock.1A4AF0's
relevant state-clear occurs when actual music activity ends. Pause the owner
along with live audio; do not advance mixed frames while paused. Reset/close
must callmusic.reset() before closing or destroying live audio. This stops only
its owned117 voice and restores its fresh source state. Restart uses fresh
ownership/cold preparation as appropriate for the existing session lifecycle.

Native backend boundary: the actual device/capture is the existing shared
50HD output, whose status remains independently observable. A started mixer
voice is not a device-audibility claim. This native transport does not execute
original IOP streaming, reproduce SPU2 interpolation, or reconstruct its volume
ramp machinery. The separate2C0F00/234440 one-shot envelope path includes2s
requests for longer finish clips; the current native backend supplies natural
PCM with unity per-voice gain and the sharedMusic118 group multiplier. This is
an explicit backend limitation, not a claimed reconstruction of the whole music
UI, all race music categories, or the full original audio engine.

## Remaining verification checklist

- Cold authenticate both metadata inputs and exactly six original assets.
- Verify six ready stereo clips, exact frame bounds and8,252,608 PCM bytes.
- Verify each final-lap key really starts nonzero original music on117 and
  remains active after its first full clip duration; no extra slot is touched.
- Verify finish26/27/28 stops the loop and plays once; real mix completion
  resets12C and leaves Dax84 and all other voices unchanged.
- Verify repeated last-lap during12C=2 is a real source suppression.
- Verify mode2 suppression, invalidA2, unknown keys, missing PCM, paused state,
  ownership collision and failed provider admission never forge playback.
- Verify shared Dax begin/end changes theMusic118 group once and restoration
  does not overwrite the ordinary per-voice gain or source-frame position.
- Verify pause, reset, restart and close cannot leave a dangling PCM voice.
