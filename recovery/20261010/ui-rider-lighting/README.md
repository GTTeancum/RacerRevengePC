# Rider actual ambient/directional state

New original_ui_rider_lighting50kg pair plus exact-preimage terminology-only
corrections for active selection header and its recovery README. Existing
selection code had only a useSavedColors proposal boolean, so it did not
perform a wrong camera mutation; the misleading comments are corrected.

Source1CDB10 light records are0xB0 each; object pointer38. page1E9E70 uses
UI+29C+E8 = SECOND light pointer, not any camera tint. Ambient captures actual
light-manager426F60+50 by copy. Directional146A30 truncates each currentRGBA
lane into packedARGB; source1E9ED4..1E9FB0 unpacks all four bytes into page530.
The captured directional alpha is not forced255. Initial127.5 becomes127.

Actual ui.dat two-light construction is Ambient80/Directional140,140,130.
146FDC halves sourceRGBA; manager148F90 sets ambientW64. Directional initial
RGBA is70,70,65,127.5. Target0 minusPosition(-1000,1000,1000) is normalized at
1CE310, then normalized AGAIN by106EA0 through147200 into light78/7C/80.
Native code retains two normalizations, using scalar floats/sqrt, not guest
execution or a VU precision emulator. No source shader/lighting policy replaced.

prepare creates actual initial manager current state. Capture must happen at
real opening boundary against current values; callers must not recreate author
colors instead of preserving the live state. apply updates current ambient50
and sole active ambientSlot60 (manager1CCD88 asks for one slot), plus actual
second directional color3C. Locked selects0,0,0,64 and0,0,0,255 respectively;
unlocked/closing-threshold restores actual captured colors. Existing unused
original ambient backups70/80, directional noise fields and scene observers
remain outside this bounded state because global1499A0 does not consume them.
The actual shared manager and renderer must reference this same current owner.

Renderer worker consumes ambient50,directional3C.xyz,directionalLook78 in its
source1499A0 global matrix preparation, then original Diffuse/Compressed path.
This pair does not fabricate renderer readiness or register a destination.
No tests/builds/syntax checks/project parsers ran.
