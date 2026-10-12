# Native logical audio channel ownership

This source integration routes the existing opponent engine, Binder and boost
voices through a native channel owner. It preserves original PCM selections,
gains, pitch, looping, boost onset serials and existing mixer slots96..116.

An opaque channel token owns an actual mixer epoch, slot and playback revision.
A stale logical sound cannot stop or control a reused foreign playback. Native
start/control/stop operate the real host mixer directly. Source233670 retains
its branch-specific logical field changes; no PS2 command queue is emulated.

This is a functional ownership improvement on existing audible opponent audio.
Full original AI D40 event/factory/child scheduling remains separate ongoing
engineering work; the existing selected-player-derived suffix policy is not
claimed to have been replaced by this patch.

Source-reviewed only. No new build, test, playback, or gameplay check was run.
