# ROUND7 Gasgano earned unlock recovery

Outcome: recovered the original course-rival qualification, profile receipt, unlock predicate, writer and concrete UIUnlock caller from static original bytes. Gasgano is the original fresh TB/course1 rival. The genuine route can award rider7 after the unretired first human finishes ahead of that rival and the original unlock-page lifecycle accepts the award. It is NOT a fresh unlock, asset capability flag, first-place requirement or a Malastare/MA identity. No production source or profile was changed.

Local base:79fc223eda2da3d98929c9f3b1253745d17e7c2a, isolated post-ROUND5 source plus ROUND6 report. Parent source is newer and was not inspected. Reasoning effort:medium. checks_run:[]. Earlier handoffs/snapshots preserved. This report supersedes ROUND6's unknown-writer finding with the static evidence below, without rewriting ROUND6.

## Provenance and method

Exact original read-only file:
D:\Programming\GitHub\RacerRevengePC\work\native\project\disc\SLUS_202.68
Known extent2972720 bytes. Get-FileHash in this round observed SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

Existing recovery documentation establishes LOAD file offset0x80 -> VA0x100000. Every address below maps to exact file offset VA-0xFFF80. PowerShell/.NET FileStream reads displayed little-endian words; initial output also displayed static bit fields. No guest instructions were executed, emulated or dispatched; no project decoder/script/module or third-party disassembler ran. A bounded reference scan of VA100000..2FFFFF selected only exact JAL/address words for187EE0 and187F40, finding calls at187E44/20AEBC and20B4C8 respectively. This is not proof against indirect calls or references outside the scanned region.

Targeted read spans:187E30+640,1AB520+2E0,1ABE70+220,17E900+250,20AE70+190,20B3F0+160,1AB7D4+1C0,17EB50+3A0,20ADD0+D0,20B5C0+160,17DE10+150,1D16B0+1B0,1AC090+390,209A80+100,17E400+240,17E640+140,3D0620+D0,3A89D0+80 (hexadecimal extents). Original bytes remained in memory/stdout only; no executable or asset extraction output was produced. Raw words selected below make the key findings reviewable without an execution claim.

## Course identity: TB, original course ID1, fresh designated pod7

-17E6DC/17E6E0 words3C0B003B/25798A08 form t9=3A8A08. Static bytes at3A8A08 are `tb` followed by NUL.
-17EBC0..17EBE8 forms profile+11284 and copies those three t9 bytes. This is descriptor11278 +12*1.
-17EBF0 word2402006F and17EBF8 AC221288 store localization key111 at11288. English display name is not recovered in this round; do not invent it.
-17EC04 word24020007 loads7;17EC0C A022128C writes byte7 at profile+1128C =11280+12*1. This is the fresh course-record/designated rival pod, not the selected human pod. docs/original-ai-cold-owners50iu.md:46-51 independently distinguishes this field from human selection.
-src/game/original_racer_selection50ij.h:22 maps original racer7 to POD07/Gasgano. Course order src/game/original_tournament_result50kg.h:24 is0,11,4,8,7,1,5,9,12,6,10,3,2: TB is sixth, distinct from zero-based ID1. The fresh availability count4 does not admit it.
-tests/native/track_session_assets50ig_fixture.h:31 retains TB metadata TRACKS/TB.RES/tracks\\tb.scb. This is metadata, not runtime world admission. src/game/original_ui_course_race_domain50kg.h:19-23 excludes course1. No original course asset was opened this round. MA remains Mon Calamari and is not a substitute for either TB or the user's unresolved Malastare request.

## Exact qualification owner1ABE70

Let P be the actual application profile, race be the real ended race, M its manager at+B0, T=P[1125F]&15, R=signed byte P[11280+12*T]. Original register/address flow:

-1ABE8C..1ABED4 obtains global application pointer from absolute382B80 (LUI0038 plus displacement2B80), then application+2DC gives P. 1ABEA0 loads race+B0. 1ABEB4 reads selected byte1125F;1ABEBC..1ABEC8 computes12*T;1ABED4 word80311280 loads signed R.
-1ABED8 JAL1AF330 with a1=0 obtains the actual first human.1ABEE4 reads its signed placeCCC.1ABEE8 calls the same first-human accessor;1ABEF0 reads retirementCD4.
-1ABEF4 word14400029 returns false when retirementCD4 !=0 (s0 initialized0 at1ABEDC).1ABF18 reads P[10FAC+4*R];1ABF1C returns false if already unlocked.
-1ABE94/1ABE9C build s2=0x7FFFFFFF.1ABF28 sets candidate manager index s4=1.1ABF68..1ABF80 obtains actual global application+28C race manager+B0 and loops while signed index < count30.
-1ABF2C calls1AF330 for each candidate;1ABF34 reads candidate pod60;1ABF38 compares to R. On the first match,1ABF50 calls the same accessor and1ABF5C loads that rival's placeCCC into s2, then exits the search.
-1ABF88 word0272082A performs signed SLT(firstHumanPlace,rivalPlace).1ABF8C branches when comparison false;1ABF94 word24100001 sets returned result true otherwise.1ABF98 copies that boolean to v0.

Thus the literal predicate is: first human unretired AND target rider still locked AND signed first-human place < selected rival place (or the source sentinel described below). There is NO local place==1, top-three, kill threshold or mode34==2 test in1ABE70. The helper alone does not authorize invocation: outer race-end guards and retained actual objects are required.

Source edge cases must remain explicit: when R is absent from indices1..count-1, the comparison retains INT_MAX, so a normal finite first-human rank can satisfy the literal predicate. The source does not separately require rival unretired, positive ranks, unique rival identity or presence. Do not present this as proof that absence earns a legitimate Gasgano award. A bounded native admission should require genuine valid manager rows and explicitly recover/admit the absent/duplicate edge if needed, not fabricate rival presence or use the sentinel as a shortcut. For the intended genuine field, retain Gasgano as the actual opponent. Profile descriptor/save mutation may affect R; the proven mapping7 is the original fresh constructor, not proof for arbitrary imported saves.

## Profile receipt and outer routing

1AB520 is the original race-end owner already documented in docs/recovery/course-unlock-join-20261014.md:28-45. New static observations:

-1AB538 checks race+104;1AB55C reads actual P+11258 and1AB560 requires subtype1 before this result work.
-1AB568 calls1ABFC0 with a1=0;1AB578..1AB598 retains its low bit in P+7E4. The inspected1ABFC0 body examines actual race-record results; do not relabel this bit as the first-human retirement field. Retirement for this award is the explicit CD4 read above. Full semantics/callers of the record helpers are not newly recovered here.
-1AB594 word0C06AF9C calls1ABE70 with a0=the saved real race (set1AB580).1AB5A8 wordAC6207E8 stores the returned qualification at P+7E8.
-1AB5B0..1AB5C8 then requires the real race104+20C and application+2E0 owners for the subsequent branch. Their complete lifecycle meaning is not inferred here.1AB5D8..1AB5DC selects mode34==2 tournament branch versus other modes.
-Mode2 separately calls1875C0 then187D80 at1AB600/1AB610 (career/course progression). These are NOT the rider unlock writer.
-1AB7D8 calls aggregate eligibility187E30 when that tournament routing reaches it. If true,1AB7F0..1AB7FC requests17DE10 with arguments a1=6,a2=10,a3=1.
-The other-mode branch checks7E4 first at1AB828..1AB830, then calls187E30 at1AB894 if that earlier route is not taken. If true,1AB8E8 also requests(6,10,1), with the source dirty flag set. Consequently this award is not intrinsically tournament-only. Record/other result routing can take precedence and may defer the unlock screen; do not skip it.
-17DE18/1C/24 stores these arguments into application318/31C/320, then queues through141960. These are route arguments, NOT 'Course screen6/page10'. Screen10 is UIUnlock per src/game/original_ui_route_registry50kg.h:30; its original constructor is1D16B0 and loader1D1780.

Exact later callers for every record/high-score diversion remain a connected follow-up; a successful qualification receipt alone is not a completed award.

## Eligibility187EE0 and writer187F40

187E30 aggregates pending awards and calls187EE0 first at187E44 (0C061FB8).187EE0 loads T, computes12*T, loads signed R from11280, computes4*R, loads unlock word10FAC.187F20 rejects an already nonzero word.187F28 loads P+7E8;187F2C rejects zero, otherwise187F34 returns1. Thus this eligibility is the stored genuine result receipt joined with the CURRENT profile course-rival identity and locked state.

187F40 itself has no guard. It loads T and signed R by the same indexed path, loads a2=1 at187F4C, and writes1 to P+10FAC+4*R in the return delay slot187F84 (wordAC260FAC). For T=1/fresh R=7, the destination is exactly P+10FC8. File offset of that write word is88004. This is the concrete writer requested, and its lack of internal guard makes faithful caller/lifetime ownership essential.

## Actual UIUnlock callback, not an immediate Results write

-1D17D0 is the screen's page factory: it accepts page1, allocates2D70 bytes and calls209A80 at1D1810.209A94..209AB0 installs page vtable3D0620.
-Static vtable3D0694 contains20AE40 (offset74), and3D0698 contains20B480 (offset78). These connect the page with the observed callbacks; no vtable was executed.
-20AE40 initializes the page.20AEBC calls187EE0 on the actual application profile, storing its result at page+2D44 at20AEC4. It similarly captures other independent pending awards.20AF5C..20AFC0 selects the corresponding course-rival illustration using actual course/order metadata.
-There is also an original application+344 conditional at20AE68..20AE9C that can replace1125F's low nibble with7 before the pending queries. Its meaning is NOT recovered; it must stay unadmitted, never used to force a selection. The normal route needs authentic stable selected-course ownership.
-20B480 requires callback a1==0 and a2==4 (20B48C..20B4A4). This report does not assign a guessed controller-button name to that event.20B4B0 loads page2D44; if nonzero,20B4BC clears it and20B4C8 (0C061FD0) calls187F40 with actual application+2DC in its delay slot20B4CC.
-After pending awards drain,20B664..20B6B4 reads profile.mode34 and routes onward via the real page virtual callbacks; additional original side effects follow20B6B8..20B6EC. No native acknowledgement/save completion is claimed.

The qualification/eligibility/writer join assumes the same selected course/rival profile survives from race result through page callback. A native implementation must bind/revalidate real profile generation, race selection and pending award identity rather than exposing187F40 as an arbitrary setter. Clearing the pending flag, original callback ordering, dirty/save work, routing and subsequent snapshots must be owned together.

## Existing native ownership and precise missing pieces

All references here describe this older isolated tree, not parent integration:

1. platform/linux/native/original_ui_rider_profile50kg.cpp:61-83 contains actual retained application profile storage, career bank and unlock array, but lacks retained original course-descriptor rival rows and P+7E8 pending qualification. Fresh reset140-142 keeps rider7 locked. Only reset writes the reviewed unlock array; subsequent Rider snapshots read it at175/237/255.
2. src/game/original_tournament_result50kg.h:18-45 and implementation cover1875C0/187900/187A20/1AF4B0/187D80, career bank/medals/prizes/growth and availability. They do NOT cover1ABE70,187EE0,187F40,UIUnlock or complete1AB520 routing. Real result rows already expose pod60/placeCCC/retirementCD4, so those are reusable actual observations, not permission to fabricate a result.
3. platform/linux/native/original_ui_rider_profile50kg.cpp:278-311 already binds tournament result arming/commit to the actual application, launch, profile generation and manager identity/ended flags. It only calls the existing career result owner and dirty flag. A separate exact original qualification receipt must be joined in1AB520 source order before award eligibility, without repurposing course-availability advancement as rider unlock.
4. tests/native/terrain_interactive_drive50du.cpp:6136-6145 stops at needOriginalEndDispatch1A7DD0 with explicit native presentation substitution. That visible Results presentation is not the missing retail end/result/unlock dispatch.
5. src/game/original_ui_route_registry50kg.h:30 is CLOSED metadata for UIUnlock; it is not an instantiated native page/lifecycle. The factory/page callback above, pending award data/art, original event acceptance, guarded private profile commit and onward routing remain connected missing owners. No production code was added here.
6. TB is source course1/order position5/source slot6, absent nativeCourseTrackOwned50KG; current source widget bounded fresh profile in original_ui_course_widget_controller50kg.cpp:21 only admits fresh four. Existing course unlock recovery documents genuine top-three/unretired ordered career progression for course availability; reaching a real sixth-course opportunity requires the corresponding authentic progression and TB world closure, not a manual availability edit. That course prerequisite is distinct from the relative-place Gasgano award.
7. ROUND6 dependencies still apply after an earned unlock: actual Gasgano pixel/cache generation, native confirm audio events15/16 with real frame facts, voice/session/roster/Course admission and complete native world/rig/audio handling. None was executed or admitted here. Native-only/all original AI policy remains unchanged, including followed player and disabled host course driver.

## Parent integration order and limits

First review the static original chain and reconcile with parent's newer profile/result code. Preserve descriptor/course/rival identities and actual real-manager place/retirement rows. Recover remaining outer result/record diversions, application104+20C/+2E0 and UIUnlock event/lifecycle owners; do not skip them. Implement qualification receipt1ABE70->7E8 and eligibility187EE0 on the same application profile, then the private original callback-bound187F40 mutation and generation/snapshot updates with source ordering. Join genuine TB accessibility/world capability and prior Gasgano asset/audio closure before any selectable Gasgano test. Root alone owns implementation integration and future authorized execution/testing.

Static conclusions are manual interpretations of observed instruction/data words, corroborated by existing source-owner layouts; they are not runtime validation. Not established: arbitrary saved-profile descriptor behavior, all indirect callers, every high-score/record continuation, application+344 meaning, English TB display label, central branch implementation or test readiness. User planet choice and actual execution release are independent unresolved requirements. No Malastare substitution or unlock bypass is proposed.

Deliverables: this report and ROUND7-GASGANO-UNLOCK-RECEIPT.json. No production patch, no source edits. checks_run:[]; static source hash read is provenance, not a software check. No tests/builds/compilation/syntax/parser/sanitizer checks, converter/project/preparation execution, gameplay, guest execution/emulation, asset mutation/extraction, network/install/credentials/private logs, Git metadata/branch changes, other sessions/helpers or cleanup attempts. Applicable instruction discovery from prior round found no tracked AGENTS.md/SKILL.md; no newly edited project paths need additional local guidance. Parent/central/user checkout and runtime lifetime untouched.
