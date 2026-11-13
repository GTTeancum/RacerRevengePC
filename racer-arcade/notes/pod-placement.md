# Pod part placement, per-pod physics, DefinePilot flag (Racer Arcade program)

Nov 12 2026. Addresses are CPU addresses (file offset + 0x0C00FC00). Tools used: `tools\sh4an.py`,
new `tools\podre_lin.py` (linear dump of 0C100000-0C160000 to `analysis\podre_lin.txt`),
`tools\podre_tab.py` (table dumper), `tools\podre_npcpods.py` (decodes the NPC placement table).
Anything marked **guess** is not proven by the code read.

## Summary

- There is **no per-kind offset table in the draw code**. The draw code puts every part with its own
  4x4 world matrix taken from a per-pod "display record" (12 x 332 B at 0x0CEAFE40). Cockpit,
  cockpit-mounted binder model and pilot share the body matrix; each engine has its own matrix.
- The engine matrices are produced by the pod **simulation**, which places the engines from
  **per-kind offsets**, in two places:
  - **NPC (AI) pods, all 18 kinds**: table **0C2F904C** (18 pointers to 168-byte records,
    0C2F847C..0C2F8FA4): body (cockpit) offset, engine count, per-engine offset and roll angle.
    Decoded below for all 18 kinds.
  - **The local player's pod**: physics-parameter pointer table **0C25C39C** (only 11 entries, kinds
    0..10) -> 144-byte blocks copied into the player object. Only 4 distinct blocks: ANA (shares the
    "default" block with kinds 0,3,5,6,7,9,10), BEN, GAS, SEB. These hold engine rest offsets,
    cockpit offset, collision boxes and handling values.
  The two sources agree exactly for ANA, BEN, GAS, SEB (same numbers, NPC frame raised by the
  engine height).
- Extra cables/energy-binder pieces are drawn only for ANA, BEN, GAS, SEB by per-kind draw
  callbacks (pod table +132), stretched between fixed points on the cockpit and on each engine.
- DefinePilot flag = (slot != 0). Slot 0 is the local player's pod: flag 0 table 0C2A733C. Every
  other slot (other cabinets / NPCs): flag 1 table 0C2A6C7C.

## 1. Draw side

### Display pod state (0x0CEB21B0 + slot*220), as filled by DefineDispPod f_0C1439EE / f_0C14381E
| Off | Contents |
|---|---|
| +0 | 1 = defined |
| +4 + part*48 | part (LOD) 0..3: +0 texgroup handle, +4 objA (cockpit) ObjectInfo, +12 engine ObjectInfo[4], +44 objB (binder/cable) ObjectInfo (other words unused) |
| +196 | per-kind draw callback (pod table +132): ANA 0C143F62, BEN 0C1442E2, GAS 0C144434, SEB 0C14452C, others 0 |
| +200 | pod light group, +204 engine light groups[4] ("pod lgroup", "engine lgroup allocate failed") |

DefineDispPod: slot 0 loads **only part 0** (f_0C14381E with r6=0, texgroups by val); slots 1..11 load
parts 1..3 (loop 0C143BD0..0C143BE2). So part 0 is the player-only detail model, parts 1..3 the LODs.

### Pod display record (0x0CEAFE40 + slot*332), getter f_0C143054 ("get pod id")
Initialised by f_0C142E58 (identity matrices via f_0C012B00):
| Off | Contents |
|---|---|
| +0 | pod kind (low 5 bits of packet word 0; checked 0..17 by f_0C1401C2 "POD WATER REF KIND") |
| +4..+11 | flag bitfields |
| +12 | body (cockpit) 4x4 matrix, translation at +60 |
| +76 + i*64 | engine i 4x4 matrix (i = 0..3), translation at +124 + i*64 |

### Draw f_0C1447EC(slot, rec332, lod, dispstate) (called per slot from 0C144CE6)
```
0C144800  push; f_0C012740(rec+12)            ; body matrix
0C14486E  PutStaticObject(objA[lod])           ; f_0C1493B0 (cockpit)
0C144902  if lod<2: f_0C1330A0(slot, rec)       ; PutPilot (pilot object 0C412F40+slot*96, motion pose)
0C144978  PutStaticObject(objB[lod])           ; binder model, still in body space
0C14497E  pop
0C1449D0  for i<4: push; f_0C012740(rec+76+i*64); PutStaticObject(engine[lod][i]); pop
0C144A04  f_0C1401C2(rec, slot)                ; water reflection copy
0C144A18  f_0C1436AE(rec, slot, lod)
0C144A2E  callback(+196)(0, lod, dispstate, rec)  ; per-kind extras
```
So the pilot and the binder model have **no per-kind offset in code**: both are drawn in the
cockpit frame. Object nodes of the pod parts (e.g. objects 0..3, 22..24, 108) are single identity
nodes; the pilot objects (e.g. 339/340, 356/357) are skeletons whose root node is at the origin, so
the pilot sits where the cockpit model is authored around its origin (**guess** that the motion data
does not add a seat offset; not checked).

### Per-kind cable / energy-binder callbacks (kinds 1, 2, 4, 8)
f_0C143E7A(matrix, out, x, y, z) transforms a local point by a part matrix; f_0C143F10(obj, a, b)
translates to a, aims at b (f_0C14D5B4), scales z by |b-a| (f_0C012A44) and draws obj: a unit cable
stretched between the two points. Attach points (local to the part, x y z):
| Kind | cockpit points (rec+12 frame) | engine point (each engine frame) | cables |
|---|---|---|---|
| ANA 0C143F62 | (+-0.837, 0.112, -2.098) | (0, 0.41, 0.25) | 2 (object 65 at 0CEB5660), + animated objects 0CEB5520/5570/55C0/5610 |
| BEN 0C1442E2 | (+-0.78, 0.022, -1.858), (+-0.7815, -0.075, -1.858) | (+-0.6405, 0.6865, 2.3545) | 4 |
| GAS 0C144434 | (+-0.689, 0.113, -0.978) | (0, 1.2555, 0.705) | 2 |
| SEB 0C14452C | (+-0.645, -0.064, -2.181) | (0, 1.511, 1.7795) | 2, plus more parts (0.2782, 0.5656, +-0.2129; not decoded) |
Sign pairing of the +- points to engine 0/1 not checked. The other 14 kinds have no cable callback
(their binder is only the static objB model).

## 2. Where the matrices come from

### Network geometry packet (all slots other than the local pod)
Every non-local pod's record is filled from a 128-byte packet: "NS GET GEO" f_0C1397F6 ->
f_0C140550 (unpack) and "NS SET GEO" f_0C139790 -> f_0C140726 (pack). Packet: +0 kind/flags,
+4 flags, +8 body matrix (24 B), +32 + i*24 engine i matrix. Matrix = 3 words of packed 11/10/11-bit
rotation rows (scales 1023.5 / 511.5, f_0C1403D4 / f_0C14059A) + 3 float position. So remote pods
carry already-placed engine matrices; placement is done by whoever simulates the pod.

### Local player pod: f_0C100D5E (per frame) -> f_0C1000D4 builds rec at player+204
Player object p (size 0x2390, created by f_0C100F94(kind,...), only caller f_0C149F38 which also
does DefineDispPod(0, kind, 0)). Kind is stored at p+204 (0C100FC6).
f_0C1010C0(p, kind): `memcpy(p+536, *(0C25C39C + kind*4), 144)` -> per-kind physics block.
Matrix build (f_0C1000D4, `analysis\podre_f1000D4.txt`):
```
body  (p+216 = rec+12), 0C100204..0C100272:
  I; T(p692, p696+p740+0.7, p700); RotY(p1036); RotX(p1032); RotZ(cam roll*0.8);
  T(shake); T(p612, p732, p620)  [p612/p620 = block+76/+84: cockpit offset]; T(p680, 0, p688)
engine0 (rec+76),  0C1002B6..0C1002E8:  ... T(p888+p716, p892+p844+p604, p896+p724)
engine1 (rec+140), 0C1002EC..0C10036E:  ... T(p876+p704, p880+p832+p592, p884+p712)
if kind == 2 (BEN), 0C100374:
engine2 (rec+204): engine0 position + (1.4625, -2, 0)
engine3 (rec+268): engine1 position + (-1.4625, -2, 0)
```
p+704 / p+716 are the current engine positions (x, z used), reset from the block's engine rest
offsets p+588 / p+600 (copies at 0C1031F4..0C10320C and 0C10344C..), and integrated every frame from
velocities p+852 / p+864 (0C100AA4..0C100B0A). The y comes from the rest offset y (p+592 / p+604) plus
dynamic terms p+832/p+844 and p+880/p+892 (**guess**: suspension / engine sway; writers not traced).
So the player's engines are simulated points whose rest positions are the per-kind block offsets.
The BEN 4-engine branch keys on kind 2, as the original does.

### NPC pods: table 0C2F904C (all 18 kinds)
NPC module ("NPCGI : CREATE GUIDE", 0C13Bxxx). Init 0C13B33E: f_0C13A7F2(npc+1400, kind):
`rec = *(0C2F904C + kind*4)`; parts copied by f_0C13A6AA. Per frame f_0C13B3F4 -> f_0C13A8D8(npc+1400,
npc+1752 (=rec332), pos, reset, ...):
```
M = T(pos) RotY RotX [RotZ unless flags bit0]       ; stored npc+1400+288
body:   f_0C13A6FC(part body, rec+12, M)
engine i < count: f_0C13A6FC(part i, rec+76+i*64, M)
f_0C13A6FC: point = M * part.offset; spring/lag state (part+16..; f_0C13A578);
            T(part.offset); ...; RotZ(part.k * 0.0174533 + roll); store matrix
```
Record layout (168 B): +0 int part (engine) count; +4 body part def; +36 + i*32 engine part def i;
+164 int flags (bit0: no body roll; only MAR has it). Part def (32 B): +0 x, y, z offset from the pod
reference point; +12 k = roll angle in degrees (x 0.0174533 at 0C13A6BE); +16 a, +20 b, +24 c, +28 d
copied into the spring state (**guess**: stiffness / damping / limits). Forward is -z: the cockpit
sits at +z behind the engines. Decoded (`python -I tools\podre_npcpods.py img\eprom.bin`):

| Kind | Code | engines | body offset (x,y,z) | engine offsets (x,y,z) [roll deg] | flags |
|---|---|---|---|---|---|
| 0 | ALD | 2 | (0, 2, 9.25) | (-4.1375, 0, 0) [12], (4.1375, 0, 0) [-12] | 0 |
| 1 | ANA | 2 | (0, 0.785, 8.25) | (-+1.8875, 0, 0) | 0 |
| 2 | BEN | 4 | (0, 3.1375, 10.312) | (-+3.4, 2, 0), (-+1.9375, 0, 0) | 0 |
| 3 | EBE | 2 | (0, 2.2, 9.25) | (-+2, 0, 0) | 0 |
| 4 | GAS | 2 | (0, 1.2473, 5.1641) | (-+1.5212, 0, 0) | 0 |
| 5 | MAR | 2 | (0, 1.3, 11) | (-+5, 0, 0) | 1 |
| 6 | MAW | 2 | (0, 0.76, 8) | (-+1.84, 0, 0) | 0 |
| 7 | RAT | 2 | (0, 2.5, 8) | (-+2, 0, 0) | 0 |
| 8 | SEB | 2 | (0, 1.575, 9.3) | (-+2.2, 0, 0) | 0 |
| 9 | TEE | 2 | (0, 3.2, 11.5) | (-+2.4, 0, 0) | 0 |
| 10 | ODY | 2 | (0, 3, 10) | (-+2.75, 0, 0) | 0 |
| 11 | ARK | 2 | (0, 2, 7.5) | (-+2.6, 0, 0) | 0 |
| 12 | BOL | 2 | (0, 3, 11) | (-+3, 0, 0) | 0 |
| 13 | CLE | 2 | (0, 2.5, 12.5) | (-+3, 0, 0) | 0 |
| 14 | DUD | 2 | (0, 1.2, 9.5) | (-+2.4, 0, 0) | 0 |
| 15 | ELA | 2 | (0, 1.5, 8) | (-+2.75, 0, 0) | 0 |
| 16 | NEV | 0 | (0, 1.2, 0) | none | 0 |
| 17 | WAN | 2 | (0, 1.5, 8) | (-+2.75, 0, 0) | 0 |

"-+" = engine 0 at -x, engine 1 at +x. All parts: body a,b,c,d = 100, 15, 4, 6; engines 300, 30, 4, 6;
k = 0 except ALD. Engine counts match the display pod table +32 (BEN 4, NEV 0, rest 2).
Cross-check with the player blocks below: ANA engines +-1.8875 / cockpit z 8.2347 (NPC 8.25), SEB
+-2.2 / 9.3, GAS +-1.5212 / 5.1641, BEN +-3.4 / 10.312 with lower pair at 3.4-1.4625 = 1.9375; the
player frame has the engines at y = -(NPC body y) (ANA -0.7875, SEB -1.575, GAS -1.2473) or, for BEN,
-1.1375 = -(3.1375-2).

## 3. Per-pod physics / handling (player blocks, 0C25C39C)

Pointer table 0C25C39C has **11 entries** (kinds 0..10; the word after entry 10 is the string
"create guide"), so only kinds 0..10 can be the local player. Entries: 0,1,3,5,6,7,9,10 -> 0C25C15C
("default", used by ANA); 2 BEN -> 0C25C30C; 4 GAS -> 0C25C27C; 8 SEB -> 0C25C1EC. The per-kind camera
vector tables 0C25FA64/0C25FAE8/0C25FB6C/0C25FBF0 (11 x 12 B, copied by f_0C10968C to 0C400F94+176..)
have real values only for ANA, BEN, GAS, SEB, and only those four have a separate player pilot
(below): the selectable pods are ANA, BEN, GAS, SEB (inference from data).
Block copied to p+536; field (block offset, player offset):

| Off | p+ | default/ANA | SEB | GAS | BEN | Meaning |
|---|---|---|---|---|---|---|
| +0 | 536 | 1000 | 5000 | 3200 | 2000 | unknown (**guess** mass) |
| +4 | 540 | 3.3 1 5.5 | 5 3 7 | 3.3 1.8 6.5 | 3.5 1 6 | engine 1 collision box half extents (f_0C101ABC builds 8 corners around p+588 with p+540) |
| +16 | 552 | 3.3 1 5.5 | 5 3 7 | 3.3 1.8 6.5 | 3.5 1 6 | engine 0 box half extents (same pattern, **guess**) |
| +28 | 564 | 1 0.5 3.5 | 2 1.5 3 | 0.8 0.5 1.3 | 1 0.5 3.5 | **guess** cockpit box half extents |
| +40 | 576 | 0.001 | 0.001 | 0.001 | 0.001 | unknown |
| +44 | 580 | 0.000398406 | 6.89655e-05 | 0.00016129 | 8.69565e-05 | unknown (= 1/2510, 1/14500, 1/6200, 1/11500) |
| +48 | 584 | 0.0025 | 0.002 | 0.002 | 0.001 | unknown |
| +52 | 588 | 1.8875 -0.7875 0 | 2.2 -1.575 0 | 1.5212 -1.2473 0 | 3.4 -1.1375 0 | engine 1 rest offset |
| +64 | 600 | -1.8875 -0.7875 0 | -2.2 -1.575 0 | -1.5212 -1.2473 0 | -3.4 -1.1375 0 | engine 0 rest offset |
| +76 | 612 | 0 0 8.2347 | 0 0 9.3 | 0 0 5.1641 | 0 0 10.312 | cockpit offset (x, z used in body matrix) |
| +88 | 624 | 755 | 3945 | 2305 | 1500 | unknown |
| +92 | 628 | 13000 | 79000 | 44500 | 30000 | unknown |
| +96 | 632 | 250 | 900 | 600 | 370 | unknown |
| +100 | 636 | 8 | 30 | 90 | 30 | unknown |
| +104 | 640 | 0.193 | 0.194 | 0.1985 | 0.193 | unknown |
| +108 | 644 | 0.1841 | 0.157 | 0.1722 | 0.182 | unknown |
| +112 | 648 | int 2500 | 3000 | 3300 | 2800 | int threshold compared with p+7640/p+7644 (0C104F10); **guess** speed-related |
| +116 | 652 | int 947 | 829 | 823 | 940 | speed cap: if int(p+1092 * 0.65) > it, velocity p+816 *= 0.97 (0C100C16); **guess** top speed |
| +120 | 656 | 0.2 0.2 1.5 | 1.8 1.3 1.5 | 0.2 1 0.5 | 0.6 0.3 0.5 | unknown vector |
| +132 | 668 | 0 0.4 -0.5 | 0.6 1.5 -1 | 0 1.05 -0.3 | -0.2 0.5 0 | unknown vector |

The 14 non-selectable kinds have no handling values of their own beyond the NPC part table; NPC
driving parameters were not traced.

## 4. DefinePilot flag

DefineDispPod calls DefinePilot(slot, kind, flag) at 0C143BE6 with `flag = 0` on the slot-0 path
(delay slot 0C143BAC) and `flag = 1` after the slot != 0 loop (0C143BE4). Slot 0 is the local player:
f_0C149F38 calls DefineDispPod(0, kind, 0) and creates the physics player (f_0C100F94); NPCs and remote
pods get other slots. Tables: flag 0 -> 0C2A733C (player's own pod), flag 1 -> 0C2A6C7C (all others).
They differ only for ANA, BEN, GAS, SEB: the player version has its own texgroup/object (ANA 29/340 vs
28/339, BEN 32/343 vs 31/342, GAS 39/350 vs 38/349, SEB 46/357 vs 45/356) and 11 extra motions in
slots 11..21 (e.g. ANA 30 31 32 37 33 34 E0 E1 38 39 40), which are -1 in the other table.

## Open
- Meaning of most handling fields; the dynamic engine terms p+832/844, p+876..896, p+732.
- NPC part spring fields a, b, c, d (f_0C13A578) beyond "copied into spring state".
- SEB callback extra parts; ANA animated extras (objects 0CEB5520.., 20 frames).
