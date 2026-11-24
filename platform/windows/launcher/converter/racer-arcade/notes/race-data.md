## 15. Per-course hooks (landscape +40 / +44 / +48)

- **+40 = fog and lights** (called once with 1): fog block = window 1 (f_0C14E63E(1)) +36..+39 RGBA
  bytes, +40 fog near, +44 fog far; lights via f_0C145A0E (tag-list arguments, not decoded) using
  vectors at @0C3E80FC..; sun lens flare via f_0C137AD6 / f_0C13790C (courses 0, 1, 3, 4; corrected in section 18 - not Kaadu).

  | Course | fog RGB | near | far | light data |
  |---|---|---|---|---|
  | 0 SA_COURSE2 | 192,224,255 | 1000 | 10000 | @0C3E80FC (-1,2,-1) |
  | 1 ETTI4 | 255,255,255 | 1000 | 8000 | @0C3E8114 (0.5,1,0.025) |
  | 2 MALASTARE | 30,7,64 (night) | 700 | 1000 | 8 lights; positions @0C3E8120 (-2286,-95.5,1101), @0C3E82A0, @0C3E84E0 |
  | 3 SA_COURSE4 | 112,120,160 | 1000 | 8000 | @0C3E8108 (-1,2,-1) |
  | 4 Kaadu | 255,255,255 | 1000 | 8000 | @0C3E8500 (0.5,0.4,0.025) |

- **+44 = extra static objects** (r4=1: load; r4=0: draw every frame with hard-coded transforms
  through f_0C146F3A(ctx, obj, x, y, z, a); a = 4th float, guess yaw in degrees). Full dump:
  `notes\race-hooks.txt` (tools\hookplace.py). Course 0 loads objects 1229-1237 and 1240 and places
  e.g. obj 1229 at (-100,0,380), (-70,0,200), (-65,0,-260), obj 1235 at (-150,0,710), obj 1237 at
  (50,5,-408). Course 3 loads 1241-1251, 1254, E3-E5 (15 objects, 16 placements). Courses 1 and 2
  place nothing here; their +44 rotates an object by 3/4096 turn per frame (f_0C015000) and
  overrides the landscape draw count by player sector (below).
- **Landscape draw count** = landscape state +0xA180 (default = landscape record +36: 104, 32, 70,
  45, 150), the count passed to the landscape drawer f_0C147938 (guess: blocks drawn ahead).
  Overrides: course 1 sectors 10..39 -> 23 and 75..110 -> 32; course 2 70 / 60 by a condition;
  course 0 (+48) sector > 215 -> 65.
- **+48 = per-block float table**: landscape state +260 holds 1024 floats set to 1.0 at
  InitLandscape (0C1480EA). Course 0's +48 (0C1477B4) writes 0.4 for entries 49..55, 59..64,
  76..81, 84..88, 91..93, 109..112; course 3's (0C14788C) writes 0.25 / 0.4 ranges. Read by the
  landscape drawer f_0C147CA2 together with the fog window (guess: per-block fog/brightness factor
  for tunnels and canyons).

## 20. Lights (SetLight f_0C145A0E, decoded; dump tool tools\lightdump.py)

SetLight(id, cmd, args..., 17): all on the stack; light records 76 B at 0x0CEB57B0 + id*76 (ids
< 256). Commands (jump table 0C145AD8): 0 = vector A (3 f32: direction for type 0, position for
type 1); 1 = vector B (3 f32); 2/3 = int +24/+28; **4 = colour, 3 f32 (R,G,B 0..1)**; 5..8 = bytes
+38..+41 (7 = **type**: 0 directional, 1 point); 9 = f32 +44 (**near range**), 10 = f32 +48 (**far
range**); 11/12 = state 1/2 (on/update); 13..16 = flags +68/+72; 17 = end. Light ids are built in
registers; the per-block light-group assignment uses f_0C146440 (landscape state +0x10FC, 1024 x
32 B).

| Course | Light | type | vector | colour |
|---|---|---|---|---|
| 0 | sun | dir | (-1, 2, -1) | 0.95, 0.95, 1.0 |
| 0 | fill | dir | (-1, -2, 1) | 0.5, 0.5, 0.45 |
| 1 | sun | dir | (0.5, 1, 0.025) | 1, 1, 1 |
| 1 | fill | dir | (0, -1, 0) | 0.1, 0.1, 0.1 |
| 2 | key | dir | (0, 1, 2) | 1.0, 0.525, 0.38 |
| 2 | fill | dir | (0, -3, -2) | 0.086, 0.482, 0.169 |
| 2 | dir | dir | (1, 1, 0) | 0.4, 0.4, 0.4 |
| 2 | dir | dir | (-1, 1, 0) | 0.1, 0.1, 0.1 |
| 2 | **32 point lights** | point | positions @0C3E8120 (32 x 12 B) | 0.95, 0.95, 0.75, range 20..50 |
| 3 | sun / fill | dir | (-1, 2, -1) / (-1, -2, 1) | as course 0 |
| 4 | sun / fill | dir | (0.5, 0.4, 0.025) / (0, -1, 0) | 1,1,1 / 0.1 |

Malastare point lights (x, y, z): 0 (-2286.0,-95.5,1101.2) 1 (-2261.3,-84.8,1195.7) 2 (-2202.7,-64.5,
1274.7) 3 (-2114.0,-42.9,1324.2) 4 (-2014.5,-26.6,1336.6) 5 (-1921.8,-17.9,1315.6) 6 (-1847.0,-16.2,
1280.0) 7 (-1772.5,-20.8,1248.8) 8 (-1697.1,-29.8,1220.4) 9 (-1621.6,-39.2,1191.8) 10 (-1542.4,-45.2,
1146.7) 11 (-1489.9,-45.4,1071.9) 12 (-1476.5,-40.3,986.2) 13 (-1458.8,-30.6,912.0) 14 (-1423.6,-17.3,
848.1) 15 (-1369.3,-1.5,798.3) 16 (-1298.8,15.3,760.9) 17 (-573.6,17.5,1319.3) 18 (-476.7,14.2,1347.3)
19 (-370.2,14.0,1323.7) 20 (424.6,18.9,592.5) 21 (395.6,25.6,493.6) 22 (336.0,28.7,416.8) 23 (-1629.7,
-108.5,138.6) 24 (-1937.2,-108.5,399.4) 25 (-2139.9,-110.0,593.2) 26 (-2166.1,-110.0,673.0) 27
(-1457.6,-108.5,92.2) 28 (-1657.4,-108.5,261.7) 29 (-1780.4,-108.5,366.1) 30 (-2006.5,-110.0,567.0)
31 (-2068.1,-110.0,661.9).
Per frame ("set_light_malastare exec", 0C146BFC): all 32 lights get new ranges each frame, the
choice alternating with frame counter parity (0x0C016D48 + light index) between near/far
(20, 50) and (15, 45) (@0C146D64 / @0C146D5C): **flicker**; then directional colours are reset
(0.32 / 0.08 @0C146D68 / @0C146D6C, 0.95, 0.1). Table @0C3E82A0 (36 x 4 ints, -1 = none, e.g.
{0,-1,-1,-1}, {1,0,-1,-1}, {31,0,..}, {33,32,0,-1}) = per-block light lists (guess: which lights
affect which landscape block).

## 21.