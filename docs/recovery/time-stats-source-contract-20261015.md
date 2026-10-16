# Original Time Stats HUD: bounded source contract

Static research only. Original SLUS_202.68 SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Addresses below are original virtual addresses. No guest execution, project execution, implementation, or runtime parity claim.

## Owner and lifetime

Race owner allocates 0x5C0 at 1A8BD4, constructs at 193100, calls virtual loader +30, stores race+D0 at 1A8C04, and binds at 193BB0 via 1A8C20. Bind stores race+30, copies race mode8C to owner520, and lays out view1 via193BE0.

The object contains six ScreenText objects at +40, stride70; six ScreenTexture objects at +2E0, stride60; and a separate ScreenText at+550. The separate text is constructed/destroyed but no loading/drawing of it was found in193240–196F20. Do not fabricate its contents.

Destructor1931A0 destroys extra text550, then textures4C0→2E0, then texts270→40, then Entity1413E0, followed by optional allocation free. Reverse array traversal is explicit at2E87F0. ScreenTexture2DEFE0 frees buffer58, clears primitive54->10, and destroys primitive54. ScreenText2E1340 delegates2DFC60 to free text/glyph buffers and owned primitive arrays. Submitted native renderer borrows must retire before these owners/resources die.

## Authentic resources

Loader193240 loads `stats` six times through2DF0B0. Font-manager282430 receives two names at193A00–193A10: `eurostileboldbold13ingame` and `denmarkbold16ingame`. The six visible texts use13ingame, capacity40, colorFFE5D96C, initially empty string3A8F20. The nearby string3A8F80 is `%.2f`, but is not the loader's initial text.

Root-authenticated OVERLAYS.RES:154528 bytes, SHA256 `ab51e88aef70d4a619ca3e614c64b90bad07deb3b9f0ec918577a44a8effb15c`. Five visible-asset members are pinned in original_ui_archive_catalog50kg.inc:

|Member|Logical offset|Bytes|SHA256|
|---|---:|---:|---|
|font\\eurostileboldbold13ingame.cel|319488|1480|e16851c1793669a7ea54dd18b0a47675856cdef816c19c7daf5c54eea0b83021|
|font\\eurostileboldbold13ingame.pst|344064|8284|a91259d5c1c5aed65e15ffc835a8c837f84af3e62d4f6cf2d4a87d04d51de103|
|font\\eurostileboldbold13ingame.psm|1253376|1061|224306298db6cfce54964b43f8d439083bb34e7ce10ff4869193c9daeac1073d|
|ui\\textures\\stats.psm|1572864|1090|1c196c9272f353218df3ad5710e5906c181b8979e8e14c593758947bd9a7ee28|
|ui\\textures\\stats.pst|1622016|17436|43332d151814a7cb29907e0ab6fa0131bf11997a6885a07d81b60ebce759aacf|

These five are not complete loader parity: both named fonts participate in loading/resource demand. Root owns resource recovery. Raw archive framing: metadataF6, size tableEA,117 uint16 entries including terminator; indexFE,112 records,index length104C; compressed blocks start1152. Five members fit blocks13,14,51,64,66 respectively. Root completed raw-only extraction with authenticated archive/framing, exact unique names/extents, bounded24576-byte zlib blocks and individual hashes. It also recovered denmarkbold16ingame CEL/PST. No archive-controlled output paths or pixel conversion were used.

Root-authenticated ScreenMT:748 bytes, SHA256 `34a6e34f523a4babe9e593037340ddf6c8ae15b6a7a174f349e688c2ead39982`. ScreenObject, alphaReference0,wrap1/1,textureMapping1,fog0,blend1,alphaTest0,depthMethod2; other fields inherit. Evidence: raw-screen-template/manifest.json and appstart__screenmt.psm.

Authentic English localization, independently recovered by root in raw-localization/time-labels.json:458 STR_IN_GAME_HUD_LAP=`LAP:`;459 TIME=`TIME:`;460 LEAD=`LEAD:`;461 BEST=`BEST:`.

## Existing configuration and racer identity

**Correct field is configuration+0x10F94, not0x110F94.**17FA2C/30/38 adds10000 then loadsF94. Reuse existing ControlPresets49D.slots[view].field10F94 through SourceCamera ActualOwners.preferences. initializeControlPresets49D already owns defaults7,7,1 for10F94,10F9C,10FA4, matching186500–186588. No duplicate configuration object.

Row predicates17FA20,17FAA0,17FB20 accept respectively {1,2,3,7}, {2,4,5,7}, and3 or signed>=5. They also require actual view racer40==0. This is vehicle state, not human/class58. SourceCamera Reads.state40 already binds bodies.main.vehicleState40. Manager1AF330 uses genuine manager44[view], bounded by count30. Preserve original AI class58 and actual spectator view mapping.

## Layout and animation

193BE0 takes one-based view. mode520=0 gives offsets0/0; mode1 gives x0 and y0 forview1 else−27; other modes give y0 and x0 forview1 else−20.

Base texture pairs:2E0/340 at(x,30+y),3A0/400 atY52,460/4C0 atY74. Left xoffset0,width149; right xoffset149,width32; height19. UV leftU.5..0.78125,rightU.7890625..1,V.796875..0.9375. Text label/value x30/167,y31/53/75. Text objects row1=120/270,row2=40/190,row3=B0/200. Justification globals3C4738=0 labels and3C4758=4 values. Disabled-row sliding uses original staggered min/max sequence; do not replace with one global x translation.

Reset194CC0 clears transition524/528, hold52C/530, observed lap544/548; sets offsets534/538=−165 and directions53C/540=1. Update194D00 services both view states: opening/closing transition.2s and hold4.5s; crossing hold zero starts closing; Entity141ED0 follows. Preserve source subtraction/crossing order.

Prepare194F40 first lays out Application268+1, then resolves actual view racer and readsCC8. When observedLap<CC8 and race+B4 manager58 bit0 clear, sets direction1,transition.2, and increments observedLap by1 (does not assignCC8). No fabricated lap event.

## Drawing and text inputs

194FF0 selects Application268. View1 requires race1B0 low2>=2.19505C–1950B0 submits texture rows, then text rows, then Entity141D70. Both passes gate race1B0 bit4 clear,race29 bit2 clear,raceF8->1E4 bit0 clear. Existing nativeHudVisible alone exposes only the last gate and is insufficient.

1950E0 draws pairs in ascending row order when corresponding predicate is true OR view offset!=−165.195250 follows label/value order and resolves signed low nibble of race48[view]->A8 as record index. Read race+B4->3C records; records are racer+C90. Record20/24/28/2C mapCB0/CB4/CB8/CBC;38/3C/48 mapCC8/CCC/CD8.

Row1: label458;CC8+1. Subtype11258==1 caps to configuration1125C low5 and formats `%d/%d`; otherwise `%d`.
Row2: label459; subtype3 usesCB0, other subtypesCB8; formatter189000.
Row3: subtype1 label460 and gap rules below; otherwise label461 andCB4.

189000 formats nonpositive or out-of-range time as `--:--.--`; otherwise original hundredths rounding and minute cap99. Existing private original_race_hud50kg.cpp time189000 is a possible shared implementation after exact numeric-subset review.

ScreenTexture2DF550 submits owned primitive54 through246760. ScreenText2E0170 refreshes through2E0F50 and submits active primitive array entries ascending through246760. Preserve source passes, material inheritance, alpha, geometry, and renderer borrow lifetimes.

### Full subtype1 gap asymmetry

Both paths first test selected record48 bit0: set -> formatter189000(0), unprefixed sentinel.

Otherwise signed rank(record3C)>=2: format ownCBC(record2C). Nonzero prefixes byte0x40; zero has no prefix. Prefix is literal source character, not an assumed replacement sign.

Rank<2 scans ascending records for rank2, bounded by race+B0->30:
- Predicate-active path1962B4–1963DC: if none found OR race+B4->50==1, formats **ownCBC unprefixed**.1962FC loads f12 in the branch delay slot;196378–196394 does not replace it. Otherwise formats rank2CBC with0x26 prefix only when nonzero.
- Sliding-only path1969B4–196AD0: if none found formats **zero**, and has **no manager50 check**. Otherwise formats rank2CBC with0x26 prefix only when nonzero.

Do not merge these branches. Sliding-only is reached when17FB20false and offset!=−165 at196814–19684C. Each branch draws labelB0 before value200. String changes use length/content comparison, bounded backing storage, null termination and dirty bits; preserve glyph capacity semantics.

## Resource demand hook, not destruction

194E20 resolves first stats ScreenTexture2E0 through2DF8A0: primitive54->material handle00, material manager382B38 lookup298E60, material4C->signed texture handle1A, texture manager383400 lookup298E60. It then performs `if (!(10.0 <= resource10)) resource10=10.0` at194E38–194E58.

Next, font managerApplication328 lookup282210 resolves13ingame then denmark16ingame. Missing font entries are skipped. Getter281F90 resolves font408 texture handle through383400 (−1 means null); loader readiness must ensure found fonts have valid textures. Same direct floor at194E98–194EB8 and194EF8–194F18. Finally Entity141C30 dispatches eligible child virtual40 hooks. This hook is not release, TTL, or reference-count mutation.

**Native equivalent already exists:** OriginalStandardTextureResource50LA::demand10_ (header236), read-only demand10() and private demand operations. Tournament direct-floor methods in cpp730–766 already implement this exact comparison. demand104A00 is not interchangeable: it caps/transforms projected-size requests. A future TimeStats owner should receive a narrow provenance-bound direct-floor facet on the same resolved resources, with authentic PCs194E58/194EB8/194F18 and existing context/liveness/numeric validation. Do not expose arbitrary public demand setters or reuse Tournament identity.

Original14D180 traverses the canonical live texture list and resets resource10 at14D1A8. Native OriginalStandardTextureLifetime50LA already owns the matching list; resetDemand14D180 is private and currently has no caller. Do not reset demand at HUD lookup/draw or add a second list. GPU residency/allocator parity is not implied by this CPU demand field.

## Remaining implementation prerequisites

1. Expose a coherent read-only snapshot of actual view/race gates, signed record index, rank/gap/timing records and manager50; reuse preferences and actual vehicle40 owners.
2. Authenticate/decode complete required resources and inherited ScreenMT policy through root-owned recovery, including fonts demanded by source loader/hook.
3. Recover exact staggered layout arithmetic and verify shared189000 numeric behavior before coding; this document is not a full arithmetic transcription.
4. Connect dedicated TimeStats draw ownership with actual race/renderer retirement and narrow direct resource-demand access. Existing lap HUD is human-gated; do not broaden that gate to fabricate TimeStats ownership.
5. Establish genuine virtual40/resource-demand phase ordering against any future canonical texture-manager reset. Current native reset has no caller; adding one is broader lifecycle work and must not be slipped into HUD construction.

No localization gap remains for458–461. No new configuration owner is required. No production files were edited for this contract.

## Completed arithmetic follow-up (2026-10-15)

Full retained static listings: `time-layout-full-words.txt` (193BE0–194CBC) and `time-formatter-full-words.txt` (189000–18919C). This section resolves the staggered-layout prerequisite above; remaining numeric portability qualification is stated explicitly below.

### Layout193BE0: exact arithmetic and mutations

Let v be the one-based argument (admitted1 or2), X=f20,Y=f21. Mode owner520:
- mode0: X=0,Y=0.
- mode1: X=0,Y=(v==1 ? 0 : −27).
- every other mode: X=(v==1 ? 0 : −20),Y=0.

At193C6C load O=owner[530+4*v] (534 or538), and D=owner[538+4*v] (53C or540). D!=0 denotes opening. All adds/subtracts below are separate source single-precision operations, not reassociated or fused expressions.

Stagger accumulator S, used only by predicate-false rows:
1. Row1,193C74–193CA4: if D!=0, S=min(0,O+50); otherwise S=O.
2. Row2,1941B8–19421C: if D==0, S=max(−165,S−25); otherwise reload O and set S=min(0,25+O).
3. Row3,194730–194788: if D==0, S=max(−165,S−25); otherwise reload O and set S=min(0,O).

Raw46160229 is MIN.S f8,f0,f22;46161229 is MIN.S f8,f2,f22;46160228 is MAX.S f8,f0,f22. Opening caps only at0; closing rows2/3 floor only at−165. Do not add an extra clamp to row1, or simplify sequential closing subtraction across its intermediate clamp. Predicates are independently evaluated17FA20/17FAA0/17FB20 with v−1. Predicate-true row placement ignores S but does not reset or suppress the accumulator updates.

Row object/base-position mapping:

|Row|Left texture|Right texture|Label text|Value text|Texture Y|Text Y|
|---|---|---|---|---|---:|---:|
|1|2E0|340|120|270|30|31|
|2|3A0|400|40|190|52|53|
|3|460|4C0|B0|200|74|75|

For each row, predicate-true x positions are X, (149+X), (30+X), (167+X). Predicate-false x positions are (X+S), ((149+X)+S), ((30+X)+S), ((167+X)+S), in precisely that operation order. Y is corresponding base+Y, independent of predicate and slide. Position fields are each embedded object's+10/+14.

Row ranges: row1 true193CC8–193F2C,false193F30–1941B0; row2 true194240–1944A4,false1944A8–194728; row3 true1947AC–194A10,false194A14–194C94. Each object compares stored x/y against newly computed x/y; if both equal it does not rewrite them or alter dirty bits. On change:
- Text: write both coordinates and set flags+5A bits0 and1, preserving other bits.
- Texture: write both coordinates and set flags+5C bit0; additionally set bit1 if object+3C!=0, otherwise preserve its previous bit1. Do not clear bit1 merely because +3C is zero.

These scalar setters only alter position/dirty state. They do not overwrite layout dimensions, UVs, colors, text or animation owner offsets. Signed zero equality and nonfinite behavior require the same admitted numeric domain as other original screen arithmetic.

### Formatter189000 comparison

Original sequence:
1.189008 compares seconds<=0; true selects literal `--:--.--` through309230.
2.189018–189030 compares seconds against float bits4BA3D70A, exactly21474836.0; only seconds<threshold enters formatting. Otherwise sentinel.
3.189050–18906C computes p=single(seconds*100.0), r=single(0.5+p), then CVT.W.S to signed integer ticks. Preserve two separate single-precision operations and the established EE truncation contract; do not substitute decimal rounding, double multiplication, fused multiply-add or nearbyint.
4.189074–1890EC derives hundredths=ticks%100, wholeSeconds=ticks/100, secondsDigitPair=wholeSeconds%60, minutes=wholeSeconds/60, caps minutes only to99.
5.1890F0–189190 emits exactly `mm:ss.hh` and terminating zero at8. Large admitted values retain modulo seconds/hundredths after minute capping; they do not saturate the whole string to99:59.99.

Existing private `src/game/original_race_hud50kg.cpp:45–53` has the same threshold, multiply then add structure, truncating integer conversion and digit decomposition. For nonnegative ticks, ticks/6000 equals (ticks/100)/60, so its minute formula is equivalent. Positive sufficiently tiny ordinary inputs can round to ticks0 and produce `00:00.00`; input zero itself produces the sentinel. Negative ordinary inputs and both zeros produce sentinel.

Admission differences/qualification:
- Native rejects nonfinite input with false before any text mutation. Original has no separate isfinite guard; its sentinel decision is made by the two comparisons. Under ordinary IEEE unordered-comparison interpretation a NaN would take sentinel, but actual EE nonfinite bit-pattern semantics are not established here. Therefore do not claim universal NaN/infinity parity or broaden native admission based on that interpretation.
- Native additionally rejects nonfinite rounded value or rounded>=2^31. For finite binary32 seconds in (0,21474836), this overflow guard does not exclude an ordinarily rounded value: the preceding float below threshold is21474834, whose product rounds below2^31. Keep the guard as defensive admission.
- Existing code does not explicitly enforce host floating environment or suppress contraction/excess precision. Structural source equality does not prove bit-exact EE multiply/add rounding for every finite float. Sharing the helper is supportable only under the project's established ordinary-input/host arithmetic contract, or after separately recovering/admitting an exact numeric subset. No tests or execution-based equivalence check were performed.

No source-proven finite ordinary-input algorithm discrepancy was found. There is a deliberate narrower native failure contract for nonfinite values, and general EE floating-point parity remains outside this static comparison.

## Font ownership join (2026-10-15)

### Original source distinction: font resource loading versus text material loading

193A0C calls Application328 font-manager282430 with two names and reload argument0.282430 loops names in input order;2824A8 looks up282210. Existing font objects are reused without reload when argument0. A missing entry allocates0x40C, constructs281590 (name+4,256 glyph pointers+8,texture handle408=−1), loads2817D0 at282504, and appends via281FE0 at282510. Lookup282210 walks manager's singly linked list and compares the actual stored name through3090E8.281FE0 appends, preserving order; manager282190 destroys its owned fonts/list nodes. No per-ScreenText font retain/release was found:2DFD90 stores a borrowed font pointer at+4C and font glyph-table address at+60 (2DFF48–2DFF58).

2817D0 parses CEL, then temporarily changes material-directory36B970 to font-directory36B420 (281EC0–281EE0), builds the texture name, calls canonical texture manager383400 virtual+1C at281F14, stores font408, retains it through298A30 at281F28, populates29E340 at281F34, and restores the directory281F44. This path loads a texture resource directly; it does not request a font PSM.281F90 reads408, returns null for−1, otherwise resolves that SAME383400 handle through298E60. Font destruction281620 frees glyph records, releases408 through29E400 at2816D4, and frees its copied name. Fonts must outlive borrowing ScreenText objects.

Separately, each TimeStats visible text load at193A58 calls2DFD90 with font object13ingame and explicit13ingame material name.2DFF9C calls2DF8E0. That routine saves/replaces/restores material-directory36B970 at2DF960–2DF980 and2DFB08–2DFB14; creates primitive batches of at most36 characters, then invokes primitive virtual+0C at2DFAB0 with type15, font name, and ScreenMT pointer3833F0. For capacity40 there are two batches per text. The13ingame PSM exists in authenticated OVERLAYS.

**No TimeStats16ingame missing-PSM call is demonstrated or required by these paths.**16ingame is requested by the font manager and demand hook, not instantiated as a visible TimeStats text material. The relevant visible-text outer caller is193A58, with primitive material dispatch2DFAB0, but it resolves existing13ingame.psm. Do not invent a new16ingame fallback origin or reuse lap-HUD1A8DD4 provenance. Missing13ingame PSM should fail authentication, not fall back.

### Actual native classes and smallest missing joins

- Application already retains OriginalStandardMaterialDomain50LA, OriginalStandardMaterialContext50LA, and OriginalStandardTextureLifetime50LA. The domain exposes the canonical materials382B38, programs382B70, textures383400, resolver3E1A70 and archiveProvider3E1A70. Reuse those exact objects and context.
- OriginalStandardTextureTable50LA already implements name lookup, canonical row ownership, population29E340 and release29E400. However acquire299330AndRetain298A30, mutable resolve298E60 and release29E400 are private and currently friend only OriginalStandardMaterialObject50LA. A font cannot yet acquire its authentic independent texture reference through an existing public font path. A narrow font-resource facet/friend is needed; a synthetic material solely to force a texture retain is not faithful.
- OriginalUiFont50KG and decodeOriginalUiCel50KG already represent authenticated CEL metrics. OriginalUiScreenAssets50KG owns screen-local immutable decoded font/texture arrays with source provenance and same-name CEL/PST association. Its indexed font() is not Application328, has no global name identity or canonical texture408 handle, and its Texture48E/MaterialTextureAssets50EA bank is not383400.
- Repository source search found no native Application328/282430 name-reusing font manager. Application's existing fields likewise have no such owner. Therefore the missing piece is a bounded **first** Application-owned font registry/lease join reusing OriginalUiFont50KG metric decoding and the existing canonical texture domain; do not create a second material/texture manager or pretend screen-local font copies already supply original global identity. A limited cold-only implementation may admit the two exact authentic names and reload0 while leaving reload1 unsupported.
- Root must decide the shared decoded-pixel bridge for rendering: ScreenAssets may serve immutable upload evidence, but it cannot replace canonical texture identity, refs or demand. The font registry needs stable entries, authentic CEL/name identity and canonical texture leases; both TimeStats and any future recovered font consumers borrow those entries rather than registering independent copies. Actual ScreenText borrowers retire before registry removal; font leases release before canonical texture/domain teardown.
- The194E20 font demand hook must resolve through each font's existing canonical texture lease. Add narrowly authenticated direct-floor access for194EB8/194F18 on that same object. No demand104A00 substitution, no parallel demand field, and no lookup-time resets.

Concrete stopping boundary: existing components cover CEL decoding, authenticated archives, canonical texture population/lifetime and material loading. Missing are the Application328 bounded registry, its non-material canonical texture acquisition/release/resolve capability, the TimeStats ScreenText material acquisition under actual193A58 provenance, and stable renderer bridging. No missing-PSM allowance is justified by current TimeStats source evidence. General font reload behavior, global resource lifecycle and GPU residency are outside this bounded recovery.

### Actual all-AI Sullust caller availability

Source check of terrain_interactive_drive50du.cpp3202–3205: nativeModelDomain is acquired for `(isTaTrack||isSaTrack)&&originalCameraEnabled&&originalMainApplication.enabled`. At4725–4730, track canonicalDomain/context are acquired for `(isTaTrack||isSaTrack)&&originalMainApplication.enabled`, with no humanCourseEnabled condition. At5342–5344, SourceCamera ActualOwners receives that domain's materials382B38 and the actual Application directory whenever canonicalDomain exists. Thus the passing original-Main all-AI Sullust path already supplies the canonical material owner. The SourceCamera header comment calling this only an Application/TA extension is stale wording, not the actual admission condition. Existing lap HUD load/pointer remains human-gated separately at5327/5341.

This closes availability of the underlying canonical domain on the target path. It does not supply the absent Application328 font registry or independent font texture leases, and a standalone decoded image bank must not be substituted for those source ownership/demand joins.
