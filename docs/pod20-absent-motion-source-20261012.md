# POD20 authored absent motion and hand owners

Source-only assessment, 2026-10-12. No build, test, decoder, importer, syntax check,
or gameplay execution was performed for this change. This document does not
assert successful prepared-asset admission or runtime validation.

## Original inputs

Original raw extracted PSG/VUT/MOT files are retained under
`/workspace/shared/unlocked-race-original-motion-20261012/POD20/` with the root
extraction manifest. MCF text is under
`/workspace/shared/unlocked-race-original-text-20261012/POD20/Pods/Pod20/`.
Original executable:
`/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`.
The inspected executable mapping is file offset = virtual address - 0x100000 +
0x80. Instruction words and strings were read with `od`; no decoder was run.

POD20 chariot MCF authors only BrakeLeft, BrakeRight and Mid. Its PSG contains
8 nodes and no handlt, handrt, l_hand_dummy or r_hand_dummy. Engines contain
11 nodes each with primary and Mid motions. No missing motion or scene node
is manufactured by this patch.

SHA256 identities used for the finite exceptional admissions:

- chariot.psg: 136b0690d569ef1ed641055534371f9e99a02ac7d0a33599683f5c6d170c9ca3
- engrt1.psg: be9a0b394caf482167db3fbdb9080476eebdb818b7458dd3c0924e58d25e116f
- engrt1.vut: b50ef5b5ac03ca6aac484fd4b67364b7b435701d796fc3053a1a43f7d968da65

The right-engine VUT begins `nullcg=nullcg\r\n` (15 bytes), followed by
`binderleft1=binderleft1`. Its PSG owns binderright1. The existing recovered
153248..15325C missing-name diagnostic/skip policy therefore applies to exactly
one unknown VUT key at byte15. It does not rename the wrong key or shift rows.
`readMotionNodeBindings49O` constructs the complete PSG-derived binding rows
before interpreting VUT assignments; skipped unknown assignments leave these
rows intact.

## Original constructor guards

The following are exact virtual addresses and raw words:

- 211A4C `0c0a3164`: node lookup, name pointer3B1628 = handlt.
- 211A58 `12600013`: absent wrist skips to211AA8.
- 211A64 `12a00010`: absent elbow skips to211AA8.
- 211A70 `1280000d`: absent shoulder skips to211AA8.
- 211A9C `0c0a3ae4`: left IK constructor28EB90, only after those guards.
- 211AB0 `0c0a3164`: node lookup, name pointer3B1630 = handrt.
- 211ABC `12600020`: absent wrist skips to211B40.
- 211AC8 `12a0001d`: absent elbow skips to211B40.
- 211AD4 `1280001a`: absent shoulder skips to211B40.
- 211B34 `0c0a3ae4`: right IK constructor28EB90, only after those guards.
- 211B48/211B5C `0c0a3164`: independent target lookups, name pointers3B1638
  and3B1648 = l_hand_dummy and r_hand_dummy; absent lookups remain null.

The native finite POD20 branch therefore sets all chain indices to explicit
`SceneNodeIndex49P::none`, sets handRecords to null and skips both cold IK
constructors. It does not call a constructor against identity/dummy nodes or
mutate shared libm state on behalf of absent hands. Other pods retain their
existing mandatory chain admission and constructors.

Cold motion initialization:

- 213808 `8e250280`: load Forward lookup result.
- 21380C `10a0000a`: if absent, branch213838.
- 213814 `8e260284`: load Back lookup result.
- 213818 `10c00007`: if absent, branch213838.
- 213830 `0c05579c`: blend155E70, reached only with both motions.
- 213838 `8e250288`: load independent Mid lookup.
- 21383C `10a00007`: skip absent Mid.
- 213854 calls1559F0 when Mid exists.

Both cold geometry bounds and fresh graph installation preserve authored
chariot locals when the pair is absent, then apply the real optional Mid.

## Numeric MOT/VUT row audit

Raw headers were read at fixed offsets, interpreted using the existing
`model_geometry49o.cpp::readMot49O` schema. All seven POD20 motions have21 keys,
rotation type0, one position track and one rotation track. Position header is
byte28; its21 keys each occupy40 bytes, so rotation header is byte876. Both
channels use the same row in each file:

- 20_brklt.mot:1
- 20_brkrt.mot:2
- 20_mid.mot:7
- englt_bk.mot and engrt_bk.mot:2
- englt_md.mot and engrt_md.mot:10

Chariot rows are below8 and engine rows below11. This is a raw header/range
review, not execution of numeric key validation or sampling.

## Runtime consumer closure

`Rig::motion()` returns null when the authentic MotionView data is absent.
The forward()/back() reference accessors have no other source call sites.
`vehicle_frame49w.cpp::admit` requires actual graph/cable/body topology, but
contains no mandatory Forward/Back or hand-record condition. Its animation
stage invokes `advanceVehicleAnimation49T`.

`vehicle_animation49t.cpp` already:

- gates the pair on both forward280 and back284 (line118), and only enters
  filter/sample work inside that condition (line156);
- validates hand bindings and handRecords only for a non-none target (line125);
- invokes the IK solver only when at least one target is present (line173).

Original hot guards21A42C `1060007c` and21A438 `10600079` both skip to21A620
when Forward/Back respectively are absent, matching the native pair gate.
No runtime animation-kernel change is required. Brake/Mid and engine primary/
Mid remain driven by the authentic available records.

## Patch boundary

`pod20-after12.patch` is incremental after the independent POD12 VUT-skip
patch. It changes only the rig fixture and the two pod_model_geometry49o files,
plus this evidence document. POD20 domain/Course admission is a separate parent
change. Full prepared-asset/native-run validation remains pending the approved
central execution slot.
