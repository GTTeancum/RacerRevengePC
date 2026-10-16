# Original World4D8 audio pause suffix

Base: ccb609f. Source-only implementation; no build, syntax check, test,
decoder, or application execution was performed.

## Source and scope

The retained static annotation `rr-clock-full-static-fields.txt` identifies
1ACF50 as race+74 -> world+4D8 -> virtual+50. The World4D8 object is
constructed by 2350D0 (call at 26FCF8, stored at 26FD04), with vtable3D1AA0,
source group40=2, state44=0, and the actual Application audio authority at5C.
Its virtual+50 is2356A0.

2356B4 first calls Entity142040 with the requested boolean. 2356BC..2356E4
compares the old state44 and requested boolean; equal boolean states return1
without issuing a lower command. 2356EC stores the requested state before
2356F8 calls26D380 with authority+40 and original group40. 26D45C/26D490
write command tag0x17 and26D4BC writes the group argument. This path does not
alter volume38. Native code does not reproduce its guest command queue/RPC.

The new `RaceAudioGroups50IC::pauseWorld2356EC(bool)` implements only this
changed-state audio suffix. Its caller must separately complete the genuine
Entity142040 prefix first. It is not a receipt for the full virtual callback,
Pause opening, Pause closing, or scheduler propagation, and has no runtime
call site in this patch.

## Native ownership and behavior

The existing non-copyable RaceAudioGroups50IC remains the sole owner of the
actual World4D8 state44. Existing sourceState44 borrowers see that same word;
no substitute Pause-local state is introduced. Existing explicit provider
bindings determine which native voices belong to World4D8. No other owner
(Effects, Engines, Music, Character, Announcer, Auxiliary) is relabeled or
implicitly paused. In particular, source group number2 is not used as a
native enum ordinal.

A private mixer flag freezes each bound voice before sampling, cursor advance,
loop normalization, or end retirement. The active flag, clip, cursor, playback
revision, pitch, per-voice gain, original routing, and group gain are retained.
Resuming clears only this flag. Explicit stop/control/replacement operations
remain explicit operations and are not silently suppressed. New bindings
inherit their actual owner's current state44, so a group with no bound voices
can still change state correctly before later provider binding.

Changed-state updates preflight all affected bindings and control-revision
capacity before writing state44. The existing serial/main-thread mixer
contract makes subsequent private setters infallible. Same-state calls are
inert, as in the source. Mixer reinitialization clears pause flags and changes
the existing epoch, invalidating stale RaceAudioGroups capabilities. No new
resource pointers, retained clips, or independent teardown obligations arise.

Announcer ducking continues to modify only volume38/group gains; copying the
same actual group record preserves state44. Muting and pausing are separate:
zero gain still has its existing advancement semantics, whereas group pause
retains the source position.

## Integration limits

This supplies the native lower audio operation required after the recovered
Entity prefix. It does not prove all World4D8 providers have been constructed,
map all race audio into that group, or complete source Pause callbacks. A
future integration must call the suffix at the original callback boundary,
after the actual Entity142040 work, on the same live race group owner. The
mixer and voice storage retain their existing outlive-owner requirements.
