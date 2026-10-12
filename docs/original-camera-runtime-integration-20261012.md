# Native original camera, fades and display history

Integrated on October 12, 2026. Source integration only; this revision has not
been built or launched. The earlier GA startup smoke predates these changes.

The fresh GC/GA eight-original-AI route now owns the original camera session:
mode4 initialization, procedural mode7 intro, saved Attached mode5, and the
single authored 5.2-second countdown. Every racer, including camera-followed
slot0, remains under original in-game AI. Original camera matrices feed native
GPU rendering without host chase-camera reframing.

The real final intro display precedes the original fade-out. A GPU-completed
Attached image remains private until fade-in presents it. Native fade samples
and gameplay/menu displays update blur history only after actual completion.
Camera blur consumes original requests; it does not invent a speed/boost effect.
Source fog changes reach the existing default-enabled world fog owner.

Linux and Windows build inventories include the five newly consumed source
units. The central interactive list contains 316 C++ units; Windows has 315 plus
six vendored C units. Original CAM resources must be staged at
work/native/project/disc/PODS/PODxx/PODxx.CAM.

Native policies remain explicit: frozen underlay during fade-in, original
640x448 projection stretched to the drawable, native sky far plane100000, and
existing selected-racer audio ownership. Runtime Follow/TV/free-camera switching
and original AI-specific audio ownership remain separate engineering work.
No PS2/VU emulation or guest instruction execution is introduced.

The next centralized check is a basic build/startup smoke. Human testers own
visual, audio and full-race acceptance; no capture is implied by this integration.
