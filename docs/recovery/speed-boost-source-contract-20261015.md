# Original Speed/Boost HUD: bounded source contract

Static recovery only. Original SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. No guest execution, conversion or software verification. Listings in speed-boost-source are primary word evidence; authenticated raw source members are in raw-speed-boost-members. This contract does not establish a physical speed unit.

## Identity, lifetime and source order

Race allocates 0x6F0 at 1A8B24 and retains the object at +C4. Constructor 189450 calls Entity141300, constructs two ScreenTexts at +40/+B0 (stride70), fifteen ScreenTextures at +120 (stride60), and clears cached scalar30, race6C4, selected racer6C8, triangle6C0 and format mode6E0. Vtable3CE920 slots40/44/48/64 are 18BA70/18BBB0/18BC70/18BD60. Loader1895B0 calls Entity141C20, loads all fifteen textures, constructs/loads the triangle, loads the actual Application328 font pair, then constructs both texts. Destructor189500 destroys triangle via2E1980 at189534, reverses texture array at18954C, reverses text array at189564, then Entity1413E0 at189570 and optional allocation release.

The real outer resource directory is ui\\textures\\ under race loader scope1A8A6C..1A90CC. Parent forest attachment remains separate from any bounded native host scheduling policy; do not manufacture ancestry. Later global material population/collector remains required before submission, just as for TimeStats.

## Resources and exact texture layout

Indices0..6 share spdbost1. Their 2DF0B0 calls are1895F4,189758,1898AC,189A0C,189B4C,189CB0,189E28. Indices7..14 are boost_a1..a8 at189F7C,18A080,18A184,18A288,18A38C,18A490,18A594,18A698. Triangle boost_a9 uses18A7C0->2E1AE0, a distinct primitive path. All ten image names have authenticated PST and no OVERLAYS PSM. Missing-PSM ownership must follow actual ScreenMT/null-config construction, not an invented PSM. Existing immutable proof reader is not canonical reference/population ownership.

2DF0B0 retains material, performs14D370 then29E340, resolves atlas dimensions and creates quad storage. Default scale1, rotation0, depth1, RGB127/127/127, alpha63, Z=actual Video+60 minus1. Preserve canonical resource demands and actual final collector order.

Index | dimensions | position before view offset | UV
---|---|---|---
0 |28x28|575,352| U[0,28/W], V[42/H,70/H]
1 |40x40|570,345| U[0,40/W], V[0,40/H]
2 |40x40|570,345| U[40/W,80/W], V[0,40/H]
3 |20x20|442,395| U bits[3EEC0000,3F1E0000], V bits[3ECD4FDF,3F100000]
4 |123x32|462,362| U[0,124/W], V[73/H,105/H]
5 |48x57|563,358| U[82/W,128/W], V[14/H,72/H]
6 |123x22|462,393| U bits[0,3F780000], V bits[3F5170A4,3F7E0000]
7..13 |16x16|447+16*(index-7),398|whole-image UV
14 |16x16|559,397|whole-image UV

W/H are actual resolved atlas dimensions; preserve mismatches between authored pixel spans and geometry dimensions. Layout18AD90 uses cached race mode6CC: mode0 offset(0,0); mode1 offset(0,-197) for one-based view1, (0,-224) otherwise; other modes(-300,0) for view1,(-320,0) otherwise. Texture setters mark dirtybit0 and, if rotated, bit1. Numeric text position559,362; TEMP456,398. Text positions mark both dirty bits.

## Fonts and text

382C08 contains eurostileboldbolditalic26ingame then eurostileboldbold13ingame (3A8E40/3A8E60). Actual Application328->282430 at18A99C loads this pair with reload0. Lookups282210 at18A9B4 and18AAF8 feed2DFD90 at18A9DC/18AB20 with capacity40. Reuse the existing Application font owner and exact source CEL metrics/texture lease. The italic font has an authentic PSM; no font fallback is warranted here.

Numeric color0xfffdff67 and TEMP0xff669bcc pass133F20 (RGB halves only). Independent alpha255 yields primitive alpha63: numeric RGB126/127/51; TEMP51/77/102. TEMP localization index456 is loaded via18AC60 at18ABA4. Numeric starts empty. Ordinary formatting at18C3D8..F0 is CVT.W.S(cached30), then literal "%3d". Preserve source conversion semantics, capacity/string comparison/dirty state. Numeric justification global3C4758=4; TEMP3C4738=0. Both directly submit2E0170, not TextOverlay.

Debug mode6E0=1 formats actual Application238 via "%.2f"; mode2 uses actual Application278/270 and "%d %d". Constructor starts0. Input18BD60 calls1277D0(9,0,event,128); true cycles0/1/2 then forwards Entity142350. Do not fabricate an input event or substitute host timing fields.

## Actual view binding and gates

Bind18AD30 stores race6C4, copies race8C to6CC, lays out view1, then manager1AF330(index0) into6C8. Setter18AD20 can replace6C8. Predraw18BBB0 uses actual Application268 view; view1 requires race1B0 low2>=2. It reads race48[view].A8 signed low nibble, resolves manager1AF330 and stores6C8. Cached30 is binary32 multiplication of selected racer340->34 by bits3F2E8BC1 at18BC44/4C; layout receives view+1. This is not evidence of mph/kph.

Draw18BC70 repeats view1 admission; requires race1B0 bit4 clear, race Entity29 bit2 clear, and raceF8->ui1E4 bit0 clear. It draws textures18BDF0 then texts18C320, then Entitychild draw141D70 even when local leaf gates fail. Each leaf independently resolves actual Application28C raceB0 manager slot=view and requires its vehicle40==0. This is distinct from the selected6C8 binding. Never retag AI classes or assume a human count gate.

Actual configuration offset is 0x10FA4+4*view, owned by existing ControlPresets49D.slots[view].field10FA4 (default1). It gates texture4 and numeric text, not TEMP or the whole HUD.

## Segmented heat and boost icon

18BDF0 first submits textures0,3; conditionally4; then5,6. It reads selected racer+7C at18BE98. With source binary32 operations: n=CVT.W.S(10*heat), terminal=n+7. If terminal>=15, fraction=(heat-8*0.1f)/(2*0.1f); otherwise fraction=(heat-float(n)*0.1f)/0.1f. The constant0.1 is bits3DCCCCCD initialized at433F10. Terminal is capped at15 only. Raw46140229 at18BF5C decodes EE MIN.S f8,f0,f20 with f0=1: upper clamp fraction to1, no lower clamp. Preserve operation order and accepted source input domain; no host clamp substitution.

Loop indices7..terminal inclusive: earlier segments restore rightU=1 and geometry width=actual atlas width14. Final segment7..14 gets rightU=fraction and width=fraction*atlasWidth; width changes mark geometry dirty. Index15 is the genuine triangle, not a sixteenth ScreenTexture.

After heat, vehicle64==0 submits texture0 AGAIN (18C274); state1 submits1; state2 selects1 if race D8[actual view] is nonzero, else2 (18C2EC). Other states submit no extra icon. Preserve the duplicate state0 submission.

Root recovered D8/E8 source: constructor1A6618/1A661C zeros four slots. Update1A7C78 loads0.25;1A7C8C..94 accumulates E8 with f20 and tests ordered less-than. When not less,1A7CA4 resets E8 to0 and1A7CA8..B8 toggles D8. Bounds race1B0 low2. The enclosing update gates and genuine native elapsed-time owner still require proof before implementation; no new host blink clock is authorized.

## Triangle geometry and retirement

18A77C allocates50,18A78C calls2E1920;18A7C0 invokes2E1AE0(nullptr,3,kind2,boost_a9). Kind2 allocates three0x30 native-equivalent vertices, source2E2190, owned flag38=1 and dirty3C=1. Primitive allocation24/ctor2461E0 at2E20B8; virtual+0C at2E20F0 uses actual ScreenMT3833F0, passmask15; borrowed packet storage attaches at2E2144. Native representation can use actual triangle vertices, no guest packets required.

Outer loader sets RGB127/127/127, alpha63 and UV(0,0),(0,1),(1,0). Layout18AD90 sets vertices(575,367),(575,428),(640,367)+offset. Vertex2-vertex1 delta6D0=(65,-61,0,1). Heat updates vertex2=vertex1+fraction*delta and UV2=(fraction,1-fraction), including all live packet aliases; source vertex count remains3.

2E21D0 requires actual Application22C==5 and global383680 nonzero. Kind2 calls2E2DD0 then2E2ED0, then ascending active primitives246760 at2E237C. Viewport dimensions are refreshed from actual renderer1E8/1EC each call. Geometry transform runs only when dirty3C: angle0 avoids unresolved rotation arithmetic; scale10 defaults1. X/Y use CVT.W.S then CVT.S.W at2E378C..2E37BC. Z=actual Video+60 minus depth18(default1) at2E3018..20; it writes both stored vertex Z and packet Z at2E37C8/D0. Negative depth18 has its distinct branch2E3008; retain bounded positive default. Clears dirty3C at2E3898. Subsequent direct heat/layout alias writes must not be gratuitously re-quantized every frame. Initial dirty transform and subsequent writes have different chronology.

Destructor2E1980 first calls actual renderer virtual+20(a1=1) at2E19B4 then+24 at2E19CC. It then destroys primitives in ascending order at2E1A08, frees primitive pointer array, frees packet allocations ascending at2E1A5C, packet array, and owned vertex array2E8750 at2E1A9C. Native GPU retirement must precede freeing CPU geometry/canonical references; strong retained owner until retirement is required.

## Demand and implementation boundary

18BA70 floors actual texture-resource+10 at10: texture indices5..14 (store18BAB8), font pair382C10 (same26,13; store18BB40), triangle first material map through2E2410 (store18BB88). It then callsEntity141C30. This is mutable genuine resource demand, not an independent projected value or GPU readiness.

Smallest next owner join requires: existing shared Application/domain/context/entity count; exact source generation; genuine15 texture references + triangle primitive + two texts/two batches each; immutable pixel bridge separate from resource ownership; existing font registry exact26/13 pair; canonical ScreenMT fallback provenance and references for missing PSMs; source-order population and later collector; exact view/manager/bank projection; real boost64/heat7C and body340.cachedSpeed34; source D8 blink cadence owner; bounded native host scheduling explicitly separate from full retail hierarchy. Geometry must include a true triangle path, not quad replacement. Texture/material raster state and current collector remain authoritative.

Remaining blockers: D8 update enclosing cadence/gates and native owner; canonical Speed/Boost reference facet with missing-PSM construction (immutable proof alone insufficient); genuine texture-demand facet; CPU/native triangle submission and retirement joins; coherent read-only bank/camera adapter; exact actual Application/global rendering gates. Nonzero triangle rotation and other primitive kinds remain outside this bounded HUD contract. Unknown formatter words on those unused paths are not treated as executed evidence. No activation or generated-output claim follows from these notes.

### Blink enclosing update correction (root static proof)

1A7B40 copies its actual f12 argument to f20 at1A7B74. If Application+11C is nonzero at1A7B94, it calls only Entity141ED0 at1A7BA4 and returns before the blink loop. Otherwise the optional race child74 update and fade/1B1 work lead to the loop, with no earlier direct return in1A7BB4..1A7C78. Thus blink uses this genuine race-update delta only on the original accepted branch. Native water's existing Application11C binding is relevant source state, but is not itself a native race D8/E8 owner. The outer caller and accepted native race-step cadence must be joined explicitly; do not add a free-running timer.

### Narrow source review of root preparation

Uncommitted OriginalApplicationFonts50LA expansion was read against existing ownership. No concrete blocker found: exact TimeStats pair13/16 does not acquire26; exact Speed pair26/13 preserves requested order and reload0 reuse. Missing acquisition still rejects any published Entry borrow, and recorded publication order covers only successful first publications, so close282190 follows actual insertion order across either pair ordering. Three entries are bounded by two private exact-pair callers. Existing pending/terminal failure behavior is retained. Stable Application/Video context and actual live context checks remain required.

Committed ed12b30 immutable fallback proof reader matches all ten exact resource identities and source calls, distinguishes ScreenTexture and triangle origins, authenticates OVERLAYS/PST and absent PSM, and retains null authored PSM provenance. It adds no canonical reference or publication and cannot certify render readiness. Neither review includes compilation or runtime verification.
