# Pause feedback uses the existing genuine race emitter

Base f521ca6. Source-only optional extension; no tests/build/syntax checks,
decoder execution, playback or gameplay run.

The existing original_camera_cycle_audio50KX::Owner already represents the
actual race10C+160 response emitter constructed by1A42C0, authenticated
509-byte AUDIO/RACEUI.INI and original Auxiliary124 group. Source1A4B90 loads
that actual+160 and tailcalls292360(eventId,1,0). It is the correct original
owner for both camera event4 and Pause feedback, not a frontend replacement.

Static read of retained original RACEUI.INI gives:
-event2: SFX\UI\advance.vag (open1BE8E4 and continue1BD920).
-event3: SFX\UI\left.vag (selection up1BD844).
-event4: SFX\UI\right.vag (selection down1BD8A0 and existing camera cycle).
All three are original nonlooping mono, nonstream requests. The existing SHA
admission remains unchanged, and exact event properties/names are checked.

New optional preparePauseFeedback loads these three original requests through
the existing real bank loader into the SAME emitter owner, voice159 and actual
Auxiliary124 binding. Existing prepare still requests only key4 and existing
dispatchEvent4 retains its behavior. No runtime caller switches preparation.
New dispatchPauseFeedback accepts only source IDs2..4 and reports started,
unavailable, explicit-muted, unprepared or rejected without fabricating PCM.
A camera-only prepared owner cannot certify unprepared Pause feedback. The
existing explicit RR_AUDIO=0 mute path stays explicit and returns muted.

This preserves the established native exclusive/restarting PCM voice policy;
it does not reproduce guest sound-channel allocation. Bank storage retains the
PCM borrowed by clip descriptors until the actual mixer voice is stopped at
owner close. Do not create a second competing owner for voice159. Root must
select optional preparation on the SAME actual camera/race audio owner when
complete Pause integration is ready.

The source input category2 matching helper1277D0 tests event.code and category;
its input-context check applies only to category0.1BD7C0 separately requires
actual event.deviceId==Pause884 and mode1F0==0. Existing InputEvent48Z has the
exact original five-word shape. No new event dispatcher or host key conversion
is added here, and no driving/control-role change is implied.

Remaining open/close effects still include actual Entity graph/camera admission,
world audio group2 pause, race Entity142040 propagation and real input-device
virtual40(2). Feedback completion alone does not complete1BE8A0 or1BE830.
