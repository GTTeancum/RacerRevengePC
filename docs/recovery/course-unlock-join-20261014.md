# Original tournament result and course unlock join, 2026-10-14

Source-only recovery. No builds, tests, syntax checks, sanitizers, game execution,
profile editing, diagnostic launch, or unlock was performed. Original instructions
were read as data; addresses below are provenance, never native dispatch targets.

## Result: how Brightlands is actually earned

The existing native single-race Results presentation cannot unlock Brightlands.
The original availability writer is **187D80**, reached only from the tournament
branch of real race end **1AB520**. Fresh career progress is zero and global course
availability is four. An unretired first human must place in the top three on the
current career course, in original order TA, SA, MA, GA. Those four actual accepted
results move that career's progress through 1,2,3,4. On the fourth result187D80 sees
post-result progress4 equal to availability4 and writes availability5. RA/track7,
original slot5, is then admitted by the original Course prefix owner. This is a
source consequence to implement, not permission to set these values directly.

## Authenticated original source

SLUS_202.68 is the same 2,972,720-byte source authenticated in
ryloth-course-map-20261014.md, SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Executable LOAD mapping: file offset = source virtual address -0xFFF80.
The external static word decoder only reads bytes and formats instructions.
Unknown decoding never supplies semantics; full ranges remain available as bytes.

## Real end dispatch and tournament caller

-1A7DD0..1A7E38 runs after the genuine manager end predicate and countdown.
 subtype11258==1 calls199770 on race+F8 and virtual+20 on race+104; then calls
 HUD stop1AD230 and finally race virtual+78.
-Original race table3CEE80 has word1AB520 at3CEEF8 (=+78). Thus the missing
 virtual end owner is concretely1AB520, not a guessed Results callback.
-1AB538..1AB560 requires race+104 !=null and subtype11258==1.
-1AB568..1AB5A8 calls1ABFC0/1ABE70 and records profile+7E4 bit0 and+7E8.
 These genuine result facts must be recovered, not replaced with host estimates.
-1AB5AC..1AB5DC requires race104+20C!=0, process owner+2E0!=null, and
 profile.mode34==2 before the tournament result branch.
-1AB5EC gets actual selected order through187B10 and reads career index100C0.
-1AB600 calls1875C0 on the actual profile;1AB610 calls187D80 on that SAME
 profile, AFTER1875C0 returns.1AB648 sets profile100BC bit0 (dirty/save state).
-1AB65C calls26D070 on the real owner's+A0+40; this has not been recovered
 here and must not be acknowledged as performed. Further real result routing
 includes retirement branch1AC440, tournament-completion branches and187E30
 unlock-screen eligibility. Do not skip these and label the dispatch complete.

## Qualification owner187C80

187CA4..187CE0 searches13 profile100C4 records at stride40 for1125F&15.
The result is the zero-based course-order index, not track ID or Course slot.
Absent match falls back to0 in source; native admission should require actual
owned selected identity rather than accepting malformed input.

187CF4 and187D10 obtain the actual first human with1AF330(managerB0,0).
187CFC reads its placeCCC;187D18 reads retirementCD4.
Qualification is retirementCD4==0, selected order ==
(profile +219C +2196*career100C0)&31, and signed placeCCC<4.
Source does not add a place>=1 check. Native caller must supply genuine manager
rows so malformed zero/negative ranks never become fabricated earned progress.

## Result writeback1875C0, exact owners and order

-1875F0 obtains first human pod ID+60;1875FC obtains course order;187620
 obtains first human placeCCC.
-187624..187654 loads prior medal byte at profile+1938 +2196*C +80*P +O,
 where C=career100C0, P=human pod60, O=course order.
-187658 calls187B60 (nonzero prior medal test);187674..187690 rejects retired
 human.187698..1876AC returns unchanged if a prior medal exists and the new
 signed place is not better than its old medal. This early return precedes ALL
 progression/winnings writes. Fresh medal zero avoids this skip.
-18766C clears profile102CC, before the retirement/old-medal early return.
-1876B8 calls qualification187C80. Accepted qualification increments low five
 bits of career219C at18770C, preserving high bits.187734..187748 calls actual
 manager1AF4B0 if the resulting progress is below13.
-187750..187784 marks profile102CC=1 when selected order+1 is4,8,or13 and
 place<4. This is a genuine tournament boundary flag, not course availability.
-18778C calls187900 for the real prize and1877DC calls187A20 for the actual
 kill bonus.1877D0..1877E0 reduces signed remaining prize1946[C][C][O]
 by the first prize;187818/187834 adds prize+bonus to career2190/2194.
-187838..1878A8 traverses every actual manager racer in manager30 count,
 stores each racer's signed-byte placeCCC into medal1938[C][pod60][O].
 These are original racer identities/ranks, never replacement HOST standings.
-1878B0..1878DC adds actual first-human kill countCD0 to career2198.

Full career layout: career C stride2196 (0x894); pod P stride80 (0x50);
base abilities1920 six floats; medal1938 thirteen bytes; remaining prizes1946
thirteen signed16 values. Career totals2190/2194/2198 are separate words;
packed progress219C retains upper bits. The pre-implementation native Rider profile stored
ONLY C==P diagonal abilities/medals and separate progress bytes. It cannot absorb
1875C0's off-diagonal medal writes or reward balances faithfully as-is.

Reset17FFB0 initializes ALL27 careers and ALL27 pod rows:17FFEC..180028 copies
actual base[P] abilities,180048 clears13 medals,18005C copies original100C8
prize values at stride40 into13 remaining-prize halfwords,180080..180088 clears
three career totals,1800A4 clears only career progress low five bits.

## Exact global availability writer187D80

Let C be original profile100C0; progress=career219C[C]&31;
old=profile11018.187DA8..187DB4 compares progress to old&31.
If unequal, NO write. If equal,187DBC..187DE4 writes
(old&0xE0)|(((old&31)+1)&31).187DF0..187E18 clamps the new low-five count
above13 to13, preserving upper bits. This source join does not consult whether
RA world assets happen to be ready, and world capability must remain independent.

Distinct1878440 updates11018 bits5..6 from an actual unlock tier and preserves
the course count. It must not be confused with course unlock progression.

## Pre-implementation native blockers and ownership

1. tests/native/terrain_interactive_drive50du.cpp:6136 stops at1A7DD0 and uses
 explicit native presentation substitution. It has real ranks but performs none
 of the result/profile/save/progression dispatch above.
2. platform/linux/native/original_ui_rider_profile50kg.{h,cpp} exposes no real
 result commit. Its Storage lacks full career pod rows, remaining prizes,
 career totals, original career selection100C0, result facts7E4/7E8 and102CC.
3. Main/Rider routing has source mode2 decisions but native tournament screen9
 lifecycle is absent. Registry metadata constructor1ED7E0/input1F2310 does not
 instantiate an owner. A real fresh tournament path needs its genuine career
 selection, standings, course advancement and Garage/result routing.
4. Course widget bounded() and Course session profileCurrent() still demand
 mode0/fresh availability4/source slot<=4. Their current restriction cannot be
 widened before real profile result snapshots and tournament-owned navigation
 exist. Native destination capability alone never changes unlock state.

Implementation-ready owner boundary: one typed native result cursor reproducing
1875C0 effects in source order, with acknowledged real manager1AF4B0 boundary;
private application-profile commit uses the same retained career/profile/manager
identities and original cold-launch snapshot, runs187D80 once AFTER successful
result return, bumps generation and exposes earned availability snapshots. It
must not publish a public arbitrary progress setter or an availability override.
The complete result cursor needs real187900/187A20 rewards and full profile rows;
a progression-only subset may remain closed at its first unowned reward/route
cut and must not claim original result completion. Exact original race-end
1AB520 callers and remaining routing also require independently recovered owners.

## Resumed source-only implementation

Root subsequently assigned result/profile storage ownership. The following new
unbuilt, unexecuted source now exists:

-src/game/original_tournament_result50kg.{h,cpp}: native1875C0,187900,
187A20,1AF4B0 and187D80. Its receipt names those owners only, never outer
1AB520 dispatch, result-screen routing or save IO. Unsupported admission leaves
the bank, availability and actual shared RNG unchanged; admitted operations retain
the source arithmetic/store order without guest execution.
-platform/linux/native/original_ui_rider_profile50kg.{h,cpp}: sole full27x27
career bank, exact reset prizes and read-only career snapshot; private exact
mode2 Rider confirm, race-result arming and result commit boundaries. Arming
binds the same actual launch tuple/stamp, manager address and application address.
Result commit requires the actual manager ended flags, same selection/generation,
first-human pod identity and original8-racer count. A successful commit consumes
the arm, increments profile generation and sets the source dirty bit100BC.

Existing production launch remains mode0-only; tournament lifecycle and actual
mode2 launch admission have NOT been opened. There is no arbitrary progress setter.
The separately owned tournament/frontend caller must join the private boundary
only after its original lifecycle and genuine launch have been admitted.
The caller must arm only after the real source manager becomes active/unended
(flags58 low bits2); a manager with zero/uninitialized flags cannot arm.

### Additional original reward and ability-growth proof

Place percentages at102D0..102EC are100,70,50,30,20,10,5,0, authored by
17F77C..17F7F4. Per-course kill percentages (kills1..7) are20,30,50,60,70,
80,100 except TA/order0 and MB/order6, whose actual entries are30,50,60,70,
80,90,100. Original constant definitions17E68C/17E758..17E7A0 and writes
17EF24..17EF88 establish TA;17F31C..17F380 establish MB; other original
40-byte records retain their independently authored entries. The word after
these seven percentages is the distinct course-class field, not an eighth
bonus percentage. Positive kills outside the real racer cardinality or1..7
are explicitly outside native admission, not silently clamped.
Unused kill operands in the retired/prior-medal early-return path are not read.

187900 uses the original signed16 remaining prize of pod[C], even when first
human pod differs, times placePercent/100, then adds binary32 .5 and truncates.
187A20 independently uses prize times killPercent/100, adds .5 and truncates.
The reset prize/order list is2200,2400,2700,3700,3200,3400,3700,4700,4400,
4700,4900,5200,6000 (17EEE8..17F6D8).

The complete native1AF4B0 leaf adjusts THIS selected career C's other pod rows,
not other careers.1AF4CC..1AF56C reads first real manager racer place and forms
rankFactor=clamp(.4+((place-3)*floatBits3E4CCCCE)/-2,.4,floatBits3F19999A).
1AF570..1AF6BC iterates P0..26, skips P==C and iterates six lanes. Each lane
gets step=(max[P]-base[P])/13 at1AF5AC..1AF5B8; consumes one genuine shared
308030 draw at1AF5C4; forms unit=float(returnedLow31)*2^-31 and unit/2 at
1AF5CC..1AF608; computes old+step*(rankFactor+unit/2) at1AF634..1AF644,
then original MIN.S with max[P] at1AF668..1AF69C. Exactly156 raw RNG
transitions occur when this branch runs. The new leaf borrows the same raw32
application owner and preserves independent binary32 rounding; no seed or
destination-local RNG is introduced.

No build, test, syntax check, sanitizer, commit, gameplay, profile mutation or
unlock was performed. These source edits remain unverified until root's single
authorized integration/validation pass.
