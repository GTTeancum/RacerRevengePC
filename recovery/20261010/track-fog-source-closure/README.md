# Gameplay track fog source closure

Source-only evidence, 2026-10-10. No builds, tests, syntax checks, project parser
executions, gameplay, sanitizers, guest instruction execution or PS2/VU emulation.
No active-repository files were edited. Evidence used bounded offline archive
block decompression and byte/word inspection of the original data/executable.

## Result

**Keep the existing gameplay minimum 1333.300048828125.** It is the literal
binary32 0x44A6A99A in the retail gameplay loader, unlike Main's computed 4000/3.
The current maximum4000, base64, TA visibility1, GC visibility0.5, haziness0.5
and range formula are all now independently source-established. This closes the
parameter provenance gap, not retail world-shader parity. Keep the opt-in gate
until the actual track material fog coordinate/quantization path is closed.

- TA: source start1968, end4000; fog-phase camera near1/far4000.
- GC: source start1301.324951171875, end2666.64990234375;
  fog-phase camera near1/far2666.64990234375.
- A real reset phase restores near1/far100000. The subsequent scene fog proxy
  reinstates the same track-specific far plane. Thus near/far is render-phase
  state, not an invariant far100000 and not a global permanent far4000.

## Gameplay scene owner, not inferred from Main

1A6BC0..1A6BDC allocates0x550 and constructs1B2870, storing the scene at
PodRaceEvent+74. Constructor1B2898 installs vtable3CF110. Its slot110 at3CF220
is273C30. 1A9EE0 is the gameplay scene loader, whose own diagnostic references
`PodRaceEvent.cpp` at3AAB50. The selected track name comes from application+2DC
through187330; loader1AA000..1AA074 selects `tracks\\` and passes the track to
the same scene's virtual7C.

1AA158..1AA178 calls this scene's virtual110 with f12=0x457A0000 (4000) and
f13=0x44A6A99A (1333.300048828125). In contrast, Main1CF850..1CF878 divides
4000 by3. Their minimums must not be unified.

Scene parameter loading270B10..270B54 allocates the fog-authoring record at
scene+3AC, constructed by278430, then invokes its virtual8=278570. The vtable
is3D2AA0; word3D2AA8 is278570. After loading,273C30 creates the runtime fog
at scene+3EC, reset at+3F4 and proxy at+3F0. 273D54..273D78 passes authoring
record+10 visibility and+14 haziness, the caller's maximum/minimum, plus
base64 (0x42800000), to13E780. These are distinct from the front-end manager's
ui.dat fog. The authoring record is released at273F7C after owner creation.

## Authenticated track authoring and defaults

`evidence/ta.scb` and `evidence/gc.scb` are exact unmodified decompressed
`tracks\\ta.scb` / `tracks\\gc.scb` members from the original prepared archives.
Both begin at archive logical offset73728. Their SHA256 values match the
existing native sky fixture's authenticated source table:

- TA.RES SHA256 b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8
- TA SCB SHA256 c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d
- GC.RES SHA256 69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9
- GC SCB SHA256 d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d

Both Fog blocks are index2 of the original BINARY1 block table. TA block9881
has exactly one field: ColorRGB at9928, `175,182,207`. GC block27626 has
exactly two: ColorRGB at27716 `151,160,177`, Visibility at27728 `0.5`.
There is no TA Visibility field and neither track has a Haziness field.
`evidence/track-authored-fog.json` records exact field offsets/counts and ranges.

Original278430 initializes authoring visibility+10 to1 and haziness+14 to0.5
(278458..278464). Reader278570 selects `Fog` at3BADC0, reads color, then calls
158B80 with default1 for `Visibility` at3BADC8 (2785D8..27860C) and default0.5
for `Haziness` at3BADD8 (278610..278628). Getter158B80 preserves f12 and writes
it to the destination on missing-key failure at158BDC. Thus these are actual
retail defaults for absent authored fields, not values inferred from appearance.

## Arithmetic and camera/render continuation

13E780 stores maximum+50/minimum+54/base+58. 13E830 does separately rounded
binary32 operations, in this order:

    span = maximum - minimum
    scaled = visibility * span
    end = minimum + scaled
    spanAfterBase = end - base
    hazeMultiplier = 1 - haziness
    start = hazeMultiplier * spanAfterBase

13E9C0 (fog virtual40) and13EA10's enabled render path both call114F20 with
near1 and far=owner+38/end, then active-camera virtual8C to rebuild its matrices.
114F20 clamps near against1 and far against100000, writes+1FC/+200 and invokes
virtual7C. The admitted TA/GC defaults lie within those limits.

Gameplay AttachScene calls1B48A0 at1AADE4 with its scene+74 and race root.
That appends scene fog+3EC at1B4928, reset+3F4 at1B4940, then other children,
and fog proxy+3F0 at1B49A8. Reset13F910 calls114C80: if different, it restores
near1/far100000 through114F20 and then rebuilds the camera. Proxy13FA20 calls
its referenced fog owner's virtual48=13EC30, which reaches13EA10 and restores
the track-specific projection range. A gameplay render consumer needs the
correct phase's camera matrices; changing only fog RGB/range is insufficient
for full source projection fidelity.

13EA74..13EAA8 sends source start and end as renderer states36/37. Setter245140
stores start at context+B0, end at+B4 and255/(start-end) at+B8. Packet24BE38..
24BE78 uploads end directly to VU33.z and that scale directly to VU33.w.
For TA the scale is -0.125492125749588; GC -0.18676872551441193. These values
are source constants, not evidence that a Euclidean native distance fog formula
is the correct world-material vertex program. No guest vector program was run.

## Integration recommendation and remaining boundaries

Do not change the current gameplay numeric defaults or substitute Main's
minimum. Replace claims that their range provenance is pending with references
to this evidence. The current world vertex shader's Euclidean distance,
interpolation, eligibility and multipass policies still need source closure;
keep RR_AUTHORED_WORLD_FOG opt-in and preserve that distinction in logs/docs.

Cold authenticated asset admission can verify the Fog block field counts and
GC's visibility bytes above, in addition to existing full-member hashes/RGB.
No new loader or runtime change is necessary to correct the default values.
The scene graph proves fog/reset/proxy ownership and ordering; this report does
not claim all weather/debug fog edits or every gameplay consumer's camera phase
have been mapped, nor that existing native world projection matches them.

Original executable SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
