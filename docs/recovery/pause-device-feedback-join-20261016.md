# Pause device feedback suffix1AF770

Source-only proof, no builds, tests, decoder execution or runtime activation.
Authenticated SLUS_202.68 SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30;
file offset is VA-0xFFF80. Words below were read as static data.

## Exact reached leaf

Pause open1BE924 invokes1AF770 on race+B0. The latter initializes index0 and
loops while index < actual manager count34 (1AF7C0..C8). At1AF794..B4 it reads
Application382B80->4, then device[index] at+10, device vtable+18 and slot+40,
passing selector2. It is neither an input queue flush nor a host controller-count
operation.

The concrete PSX device table3D1D80 has word238870 at3D1DC0 (+40), independently
consistent with the retained device-held48Y evidence. The whole leaf is:

| VA | Word | Effect |
| --- | --- | --- |
|238870|10A00004|branch selector0 to238884|
|238874|00000000|delay nop|
|238878|24020002|load2|
|23887C|14A20003|selector other than2 skips first clear|
|238880|24A2FFFF|delay selector-1|
|238884|AC8006CC|duration6CC=+0 bits|
|238888|24A2FFFF|selector-1|
|23888C|2C410002|unsigned(selector-1)<2|
|238890|10200003|skip second clear if false|
|238894|70001628|return register0|
|238898|AC8006D0|duration6D0=+0 bits|
|23889C|70001628|return register0|
|2388A0|03E00008|return|
|2388A4|00000000|delay nop|

Selector2 clears BOTH durations, does not gate on connection/capability, does not
change actuator bytes/counters, and issues no hardware transport command.
Existing `src/game/controller_feedback50kz.h::clearControllerFeedback50KZ`
already implements this exact leaf. Device service later handles stop packets
through its own genuine state and transport scheduling.

## Existing native authority and remaining join

The actual driver owns `ControllerFeedbackState50KZ feedbackDevices[2]` in
terrain_interactive_drive50du.cpp. Those SAME objects are already borrowed by
physical-device service and the original vehicle-impact feedback continuation;
creating another array for Pause would be incorrect. The source current race
count is already retained as `original_camera_session50KX::ActualOwners::humanCount34`.
For its actual zero count, the1AF770 loop performs no device calls. This is an
original empty loop, not successful execution of missing hardware callbacks.
For a future nonzero count, the concrete race/Pause lifecycle must borrow the
actual Application device states, preserve ascending device-index order and
invoke the existing clear helper with selector2. It must reject an unavailable
required device owner rather than substituting a neutral device or host count.

This closes the leaf-semantics recovery gap only. It does not complete Pause
open, authorize changing all-AI racer configuration, establish input traversal,
or replace required parent/Entity/audio sequencing. Application11C, own200,
selection/controller writes, recursive enable, world pause and feedback sound
remain at their exact preceding source boundaries.
