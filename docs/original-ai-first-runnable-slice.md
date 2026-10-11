Historical source/design notes recovered during main consolidation on 2026-10-11. Later native AI integration supersedes the staging status below. These notes establish no current runtime coverage; all new driving validation must use original in-game AI for every racer, including the followed player. See README.md and current-validation.md for dated observations.

# Original AI: first end-to-end runnable slice

## Decision

Build one genuine AI-subtype racer, from cold setup through real terrain physics
and rendering, on the existing original GC world. Keep the actual other racers
visible to its source list/target logic. Begin with ordinary undamaged vehicle
state40=0 and the static-contact body20C=0 domain already represented natively.
Do not claim an eight-racer original-AI race or complete combat in this milestone.

The first deliverable must be a connected cold-start drive, not another isolated
formula: source startup state0 → state4 list driving → state2 ordinary driving
when the genuine CB8 timer exceeds5, ending at a clearly reported unsupported
source branch if one is reached. Never force state2, empty the lists, suppress
combat selection, skip damage, or fake a target to extend the run.

No new code, build, test, syntax check or gameplay is part of this planning pass.
The frozen HOST GC candidate and its scheduled central test remain unchanged.

## Correct startup and per-frame spine

Cold constructor16E290 and reset16E834 initialize FE8=0. The first16EB00 state
update dispatches state0 to16EBA8 and writes FE8=4 at16EBB0. The16E9C0 dispatcher
then calls170B50 followed by1726F0. The ordinary172380/172C50 adapter therefore
cannot be the first driving path.

State4's16EBB4..16EBDC update remains in4 while CB8<=5 and moves to2 when CB8>5.
That timer must come from its source producer and original time domain, not a
five-second HOST counter. Other genuine state/reset conditions still precede
this dispatch. States1/5/6/7 must either have their complete behavior or stop at
the exact unsupported boundary; they must not be remapped to ordinary driving.

Original outer entry16E8C0:
- Writes B94 (X-axis rate limit)=4 at16E8D4 every frame.
- Runs AI dispatch only when global433CB0 is zero, vehicle3C is zero and actual
  app28C→B4 byte58 bit1 is set (16E94C..16E994).
- Calls common228620 afterward even when the AI-dispatch gate is closed.
- Uses the genuine prepared frame factor and source call order.

Required in-frame order is16FFE0 navigation →16EB00 state/stall update →171C60
packing →1734C0 repair decision → current state's steering/speed routine →
common217DE0 controls →2176E0 thermal → original graphics/body/contact phases.

## Implementation sequence

### 1. One real cold AI owner and one shared random owner

Create a typed AI supplement in the existing native racer owner system, using
actual16E260/16E450/16E830 constructor/create/reset writes and its selected .ai
profile. Bind the actual FF0 map track, FF4 AI track, source-assigned C80/C84 and
C88/C8C, genuine racer-manager references, FE8, packing referenceFF8, minimum
speed108C, list counts1038..1048 and their real arrays1024..1034, target1008,
and source timers/progress fields. Inventory setters for CB8, CCC/CB0/CC4,
1090/1094/1098/10A0 before consuming them; do not manufacture their values.

Use50IS cold49N AI-mode handling before the first tick: vehicle58=1, source
class/profile selection, full aero steering and original engine positions.
Preserve actual graph/body/probe/repair/cooling initialization and completed
217440 initialization (B9C bit7). No runtime player-to-AI mode flip.

Introduce one canonical original308030 raw32 owner outside the racer snapshot
bank, with source initialization and consumer ordering. Route every participating
308030 consumer through it. Existing HOST bank snapshots s.noise and XOR-seeds
it per racer; that policy cannot serve as proof of original shared ownership.
Do not add an independently seeded AI random stream or merely rename the field.

### 2. Connect native source navigation and startup list driving

Implement16FFE0 and the helpers it actually reaches, including AI crossing/
next-choice170380/171A90, map crossing170230, authored branches and progress
writes. Reuse existing native track/scene primitives only where their source
semantics match. Current50EU is explicitly the PLAYER1A0DA0 progression path;
calling it for an AI is not the missing implementation.

Important shared write: after an actual AI-node advance,170068..170090 reads
that new node's BoostScale through2A2580 and resets cooldown109C to0 when its
value is greater than1. The50IT owner must receive this real navigation write.
BoostScale is unused in173180's probability comparison, but is not globally
unused by AI. Do not omit this reset or substitute a probability multiplier.

Implement the real startup state's170B50 list producer and1726F0 consumer:
- Desired-position171FE0 is sampled at a fixed source distance200.
- Native172E60 produces steering from that actual desired direction.
- Real1038/103C and other list outputs adjust steering and target speed using
  actual nearby racer observations; no empty-list placeholder is allowed.
- With empty lists actually produced by170B50, use the current authored node
  speed. Preserve the signed minimum108C and final DriveFullSpeed override.
- Preserve1726F0's own boost/ordinary-brake call conditions and arguments. It
  is not interchangeable with172C50's packing/race-pace target selection.

This step must produce controls in the same native owner used by physics.

### 3. Complete state2 direction and join existing speed components

Recover source172380 plus171FE0/172E60 and its genuine support/stall timer
updates. The0.7 and0.2 speed-scaled direction samples use signed cached speed,
motion-direction comparison and source steering arithmetic. HOST centerline
lookahead/atan2/proportional steering is not a replacement.

Run the genuine state2 target-selection/state update, packing171C60 and repair
1734C0 before the ordinary speed call. Implement the branches reached by the
chosen actors. If target selection requests an unsupported combat/reset state,
report that boundary and stop that AI owner; do not force a false query result.

Join already-staged50IR target/brake,50IT real boost provider and50IS native
control binding at their source points. The cohesive entry must supply live
B94=4 from16E8D4 rather than blindly reusing50IS's cold-authored X rate. Preserve
Y rate, D28 and filters. Connect navigation's cooldown reset to the SAME109C
state used by50IT. Run50IT thermal continuation after control binding, using
actual heat/engine history and genuinely absent D40 on its bounded path.

The initial50IS continuation is limited to ordinary FE8 states0/2/3. State4
needs its actual1726F0 target/scheduling entry into the shared binding; simply
broadening that enum admission would feed the wrong speed policy.

### 4. Admit the actual AI frame branches, not just the control write

Add a distinct native AI frame entry that completes the source stages above and
then reaches common228620. It must not invoke the player device reader, player
navigation or force a ready phase. Extend49W only after each reached mode1
branch is implemented. Existing global vehicle58 rejection remains until then.

Two concrete mode1 continuations are bounded and should be joined in the same
implementation series:
- Chariot21A660: retain prefix response/display/blend, then use source21BEB8's
  AI selector. Actual app28C→B4 byte58 bit1 clear selects physical838=1/83C=0;
  bit1 set selects virtual838=0/83C=1. Perform original activation, seed and
  velocity-copy actions only on their source transitions. Existing49V owns
  most mechanics; it currently rejects vehicle58 before this selector.
- Static impact223030/222770: retain the genuine body20C=0 static-contact domain.
  For58=1 and CFC=0, bind and apply actual track EnvDamage+60 AI severity scale.
  Preserve its weakest-threshold partner-state/damage gate and the58=1/CFC!=0
  normalized-Z threshold of−0.85 instead of−0.75 for the rear directional lane.
  The needed damage/CFC/partner fields mostly exist; the authored+60 value
  needs a real load/binding. It must not be defaulted to1.

Keep unsupported dynamic partners and other mode/state branches explicit.
Do not disable collision, damage or recovery merely to keep the run moving.

## Bounded acceptance for the next authorized central run

The first runnable build must show, for one genuine vehicle58=1 owner:
- Original cold role/parameters and first-base initialization.
- Actual0→4 startup, list production, fixed200 planning and native steering.
- Real throttle AND brake actions reaching native engine/aero bindings.
- Persistent source navigation and real-terrain motion/contact, with no pose,
  progression or lap writes from a HOST driver.
- Actual CB8>5 transition to2 and source state2 direction/speed behavior, if no
  earlier unsupported source branch occurs. A stop before this is useful
  diagnosis but does not pass that milestone.
- One shared RNG chronology, persistent109C and any authored-node reset, with
  source boost/thermal order and live B94=4.
- AI chariot mode selected from the real application flag, and authentic impact
  scaling whenever the admitted static-impact branch is reached.

Capture per-frame source state, node, list counts, target, raw steering/actions,
filtered/model bindings, cached speed, cooldown/RNG draw count and first failure
PC. That evidence must establish the connected path, not just a controller unit.
No time-to-completion or full-course claim is justified before this run.

## Exact remaining blockers

No single missing constant or brake call remains. The blocking set is the
source cold/start owner, AI navigation/list/state/direction producers, canonical
runtime B94 and109C writes, original shared RNG ownership, and mode1 frame/
chariot/impact continuations.50IR/50IS/50IT are staged pieces of that path;
they are not currently an end-to-end original-AI driver.

Recommendation: implement steps1–4 as one cohesive feature branch and admit the
single connected slice once. Keep parallel HOST GC investigations separate.
Do not spend another integration cycle adding unjoined speed helpers.
