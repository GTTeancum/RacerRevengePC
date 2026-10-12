# Native AI integer-volume spatial routing

The live AI logical start and spatial-control consumers now pass their actual integer volume38/volume44 to the existing recovered projectOriginalSoundRouting26CBA050KG helper. Directional integer division therefore happens after multiplication by the source volume, as represented by 26CC14..26CE7C. For example, stereo engine angle 45 with source volume 37 projects left 18 and right 37 percent. Applying a separately normalized 37-percent gain to routing projected at volume 100 instead yields 18.5 percent on the left.

New opt-in playSourceVolume/controlsSourceVolume mixer methods store the resulting L/R percentages in the original-mode coefficients and unity in voice.gain. This applies the source volume once. Actual sound-mode ownership, clip/rate arithmetic bounds, group binding, logical sound generation, and mixer playback-revision guards remain in force. The logical sound retains its source integer fields; unity mixer voice gain is a native backend representation, not a changed source volume.

Legacy float-gain controls, original-angle controls, and the prior float-gain source-rate methods preserve their previous policy. Their routing helper defaults to volume 100. Only the genuine AI logical start and spatial consumers opt into the new integer-volume methods. The new methods require an actual bound sound-mode owner and atomically reject invalid volume, routing, rate, or group inputs before voice changes.

Limits remain explicit: native PCM interpolation and group/master gain arithmetic are unchanged; original hardware queue/channel identity is not reproduced; unsupported surround phase selectors and active zero/negative source rates remain backend boundaries. No new C++ translation unit or build-list metadata is required.

Source reading and editing only. No build, test, syntax check, gameplay, or decoder execution was performed. Audible acceptance remains with human testers.
