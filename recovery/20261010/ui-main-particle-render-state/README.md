# Actual Main particle fog and render-camera state

Source-only native reconstruction. No build, syntax check, project parser,
importer, unit test, runtime, gameplay or guest instruction execution was run.
Static disassembly only reads original ELF bytes. Original ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

## Delivered API

`prepareOriginalUiMainParticleRenderState50KG(scene, coldManagerCamera,
lighting, output)` in the staged `files/src/game/` pair returns
`complete/input/arithmetic`. Failure leaves output unchanged. The record owns:

- `sceneCamera`: same authenticated manager camera and normalized pose/view,
  with the actual particle-phase source near/far1/4000.
- `particleCamera`: existing source screen140/combined250/focal1D8 preparation
  applied to that corrected camera.
- `constants`: exact lighting rows, combined100, VU33.zw and canvas offset.
- `packedFogColorGs`, normalized `fogColor`, and source start/end values.

Prepare once cold for the admitted stationary Main and keep this typed state in
the session. Pass the whole state into the concrete Main draw collector. Its
input remains the original cold near/far1/100000 manager-camera preparation;
never feed normalized pose through the CameraInfo setter a second time. The
projection rebuild copies unchanged FOV/aspect XY terms, recomputes source Z
terms, then performs the original separately rounded row-multiply sequence.
No heap allocation, executable source pointers or guest register model is used.

## Fog ownership and order

There are two real fog owners; their fields are not interchangeable.

1. Manager1CE7B0 reads `UI/UIMain/ui.dat` Fog. The original key at3ADAD0 is
   `Visibility`, while the asset spells `Visibilty=1`; default1 remains.
   Haziness0 and ColorRGB0,0,0 are real. 1CE9B4..1CE9D8 calls13E780 with
   maximum1200 (44960000), minimum900 (44610000), base200 (43480000).
2. Manager1CCFA0 creates fog+180, reset+188, and proxy+184 referencing+180.
   1CD098/1CD0D8/1CD118 append those three children in that order.
3. Main scene loading1CF850..1CF878 passes maximum4000 (457A0000) and
   separately rounded4000/3 into scene virtual110=273C30. 273D54..273D78
   passes SCN visibility1/haziness0 plus base64 (42800000) to13E780.
4. After full Pod child creation, Main1CF410 ->1CFAF0 invokes scene1B48A0.
   Scene+3EC fog appends at1B4928, +3F4 reset at1B4940 and +3F0 proxy at1B49A8.
   Actual particle manager+4A0 appends later at1B4AE0. Entity141960 is a tail
   append:1419A8..1419C4 follows +14 links and writes the last sibling link.
5. Reset render13F910 calls114C80, restoring source near/far1/100000 and
   rebuilding the camera. Proxy13FA20 temporarily sets its fog owner's+4C
   and invokes that owner's virtual48, whose3CD168 entry is13EC30 ->13EA10.
   That reestablishes the scene fog and scene camera range before particles.

Thus particle rendering consumes the later mm.scn owner, RGB70,10,10. The
manager's ui.dat black fog is real but earlier in the source traversal. No
caller-chosen color, inferred tint or artificial scene background is involved.

## Source range and VU upload

13E780 stores maximum+50, minimum+54, base+58. 13E830 computes:

    end38   = minimum + visibility * (maximum - minimum)
    start34 = (1 - haziness) * (end38 - base)

Each arithmetic operation is binary32 and separately rounded. Actual Main
therefore gives end4000, start3936. This is independently established by source,
not the historically inferred track-world fog formula in older documentation.

13EA74..13EA88 sends start34 as renderer state36;13EA94..13EAA8 sends end38
as state37. The concrete renderer constructor243A94..243AA0 installs vtable
3D2050 at context+40; its+30 entry3D2080 is245140. That setter stores:

    context+B0 = state36 = start
    context+B4 = state37 = end
    context+B8 = (start == end) ? 0 : 255 / (start - end)

The exact division is2451D4..2451F4. There is no world-to-screen depth
conversion in this setter. Packet24BE38..24BE4C copies B4 directly to VU33.z;
24BE68..24BE78 copies B8 directly to VU33.w. Main therefore supplies:

    constants.fogStart33Z = 4000
    constants.fogScale33W = -3.984375

Despite the field's convenience name, fogStart33Z receives source end38, not
source start34. Preserve this direct source contract; do not replace it with a
conventional distance fog formula or reinterpret the particle depth packet.
The native ParticleCode worker owns the source clamp0..254/integer/F÷256 path.

## Camera dependency

13EA30..13EA60 first calls114F20(activeCamera, near1, far=end38), then active
camera virtual8C to rebuild. 114F20 writes clamped near+1FC/far+200 and calls
virtual7C. Main's actual end4000 is within the source clips1..100000. This
changes projection Z and combined100/combined250 even though pose is stationary.
Consequently the cold manager far100000 matrices cannot be used unchanged for
the scene particles. The helper supplies their source far4000 continuation.

This is a render-phase cache for the stationary closed Main graph, not a claim
that all camera history, scene visibility registration, listener or debug state
is recovered. Source screen depth1/8388591 and focal-pixel computation remain
unchanged; they are distinct from projection near/far1/4000.

## Packed color

273C64..273CB0 converts and packs source fog channels as AARRGGBB;
13E830 ORs FF000000 and retains that owner color. 13EB90..13EBBC repacks RGB
for GS register3D: red in bits0..7, green8..15, blue16..23. Actual FOGCOL is
`0x000A0A46`, normalized renderer RGB is70/255,10/255,10/255. Alpha is omitted.
13EBEC..13EC04 queues the real cached packet through244A40. The register is
global, not context-specific, and the later source scene write wins.

## Evidence locations

The parent stage `../evidence/` now contains:

- `fog-owner-13E6B0-13EAE0.txt`, `fog-render-13EADC-13EF00.txt`
- `manager-fog-1CE990-1CEB20.txt`, `manager-fog-attach-1CCFA0-1CD130.txt`
- `render-fog-setters-245040-245260.txt`
- `camera-range-114EE0-114F90.txt`, `camera-restore-114C80-114D30.txt`
- `fog-packet-registry-244A40-244D60.txt`
- `entity-append-141960-141AC0.txt`
- existing `scene-particle-capacity.txt` (273C30 fog constructor too) and
  `normal-target-packet-tail-24BE00-24C2A0.txt`

Sibling camera stage has `main-scene-load-1CF630-1CFAF0.txt` and
`main-scene-attach-1B48A0-1B4D50.txt`; session stage has
`render-group-tree-1CE7B0-1CE9B0.txt` and `render-context-13F850-13FC40.txt`.

## Integration notes

Register only this new cpp in the root's real source list. Include its header
from the existing native session/draw pair, which already uses the exact typed
API. Cold core-camera comments remain accurate for their cold scope; any claim
that source render matrices are invariant with far100000 must be narrowed to
the cold phase or replaced by this scene-phase continuation. No active files
were changed here.
