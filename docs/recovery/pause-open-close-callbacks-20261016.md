# Original Pause open/close: source-only handoff

Static source ELF SLUS_202.68, code VA offset VA-0xFFF80.
Retained new words: /workspace/shared/pause-1bd7c0-1be980.words.txt.
No build/test/syntax check/guest execution or native runtime validation.

## Genuine open1BE8A0

Input a0 is actual Pause owner; a1 is actual opening controller identity.
The exact side-effect/callback order is:
1.1BE8C8 writes Application11C=1.
2.1BE8CC writes Pause880=0;1BE8D0 writes Pause30=.25.
3.1BE8E4 invokes1A4B90 on actual race+10C with argument2 (sound path).
4.1BE8F4 invokes actual Pause virtual+24 = Entity141630 (recursive enable).
5.1BE900 writes Pause200=1;1BE904 writes Pause884=retained input a1.
6.1BE910 invokes1ACF50 on actual race with argument1.
7.1BE924 invokes1AF770 on actual race+B0.
This is the genuine opening reset, not a manual timer override. Do not expose
steps1/2 alone as successful original Pause activation.

## Genuine close1BE830

1.1BE848 writes Application11C=0.
2.1BE84C writes Pause200=0.
3.1BE85C invokes1ACF50(actual race,0).
4.1BE878 invokes actual race virtual+50 with argument0.
5.1BE888 invokes actual Pause virtual+1C = Entity1415A0 (recursive disable).
No30/160/164 store occurs in this close function. The shared1BA120 callback is
instead a reveal/reset of the shared backdrop state after a disconnected input
controller; it must not be called the Pause close action.

## Concrete dependencies recovered

1ACF50 is six words: load race+74, load that owner's+4D8, load target vtable+8,
load virtual+50, tail dispatch preserving argument a1. It is not a simple
Application pause flag setter. Authentic targets/lifetimes still need binding.

1AF770 iterates actual race+B0 object's count34, loads Application+4 array's
controller at index*4+10, and invokes each actual device's virtual+40 with
argument2. It cannot be replaced by a host controller count or synthetic device.

1BD7C0 is Pause input dispatch. It skips its handling if actual mode1F0!=0 or
input event+8 !=Pause884. It checks original1277D0 selectors in source order:
selector12/group2 decrements selection with wrap0->10; selector14/group2
increments with wrap10->0; selector2/group2 dispatches selected action. Each
selection change invokes actual race+10C1A4B90 with3(up)/4(down). Accepted row0
calls genuine1BE830, then sound2. The full input callback eventually forwards
Entity142350. Settings rows/restart/quit branches have further genuine owners.
This note does not authorize substituting host menu key events for1277D0 input.

## Integration gates

Existing native Application clock/pause11C can represent its own original lanes.
The actual Pause vtable is3CF420 (LUI003D plus signed ADDIU F420), file-backed:
+1C=1415A0, +24=141630, +30=1BA390, +38=1B9FE0, +40=1BAE00,
+48=1BAF60 (tail1B9220), +64=1BD7C0, +74=1BB010, +78=1BAF70.
Shared native Entity recursive enable/disable precedent exists in
original_ui_rider_dynamic_owner50kg.h, but the cold Pause does not yet own an
attached Entity child graph. Do not represent those calls by a plain ready bit.
Race virtual+50, race+74/+4D8 target and actual device virtual+40 calls still need
joining to concrete original owners. Search of current native source found no
named1ACF50/1AF770/1BE830/1BE8A0 implementations.

The frozen geometry patch remains private and inactive. Next source work should
resolve these actual targets and ownership before a concrete lifecycle adapter,
then join camera-last admission and source draw tail. No new open/close methods
accepting arbitrary callbacks or fabricated success receipts were added.

## Further resolved world audio target

world26FC90 allocates0x60 at26FCD4, invokes2350D0 with actual
Application+0->A0 and argument1 at26FCF8, and stores the returned owner in
world4D8 at26FD04.2350D0 sets table3D1AA0, owner40=2, owner44=0 and owner5C
to the actual supplied audio authority. Table3D1AA0+50 resolves2356A0.

2356A0 first invokes Entity142040(requested pause), which updates Entity29 bit2
and conditionally propagates to eligible children. It then compares requested
boolean with cached owner44. When unchanged it returns1; when changed it stores
the request in44 and invokes26D380(owner5C+40,owner40). Thus the original
world pause callback reaches actual audio group2 and Entity pause propagation.
The lower audio function's stateful group operation is not yet natively joined;
it is not safe to replace this path with a synthetic successful callback.
