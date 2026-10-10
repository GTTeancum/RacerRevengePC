# Player event sequencing and held-source selection: 49A-PlayerEvents

2026-10-06. Cloud/Linux continuation of preserved 48Z-EventDispatch.

## Delivered behavior and the important remaining dependency

`src/game/player_events49a.h/.cpp` implements the complete original player press
handler's control flow, ordered held queries, direct state writes and synchronous
call ordering. It also implements manager-held source selection, with category2
resolved through the concrete 48Y held-state implementation. A caller-selected
48Z child binding handles press through this handler and release through the
already recovered original GameObject release router.

This closes another input-routing and state-sequencing layer. It does not recover
the larger action helpers. The first helper,2255B0, alone has379 words, vector
math, object/path traversal, a virtual call and further dependencies. Its two
following scalar stores are not a substitute for its effects. Every reached
unrecovered action requires a synchronous effect binding. A missing binding
reports unavailable before dependent stores; an unavailable invoked binding
retains any effects it already produced. There is no no-op default, command
buffer or deferred-action surrogate.

Therefore the library can route packet-produced inputs into the correct player
action boundary, with proven local state behavior, but cannot claim that the
complete gameplay action happens until its concrete helper is implemented.
Synthetic effect callbacks in tests demonstrate the boundary's ordering and
mutation semantics, not the gameplay meaning of the missing helpers.

## Identity and evidence

Original ELF SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Player constructor1A02C0 installs table3CED50 at player+8. Its slot+60 is1422A0,
the inherited release router; slot+64 is1A16F0, the player press handler.
ControlInterface table3CCE10 and PSXControlInterface table3D1CA0 both map slot+10
to128520. Constructor and table bytes are source provenance, not observations of
live initialization, object ownership or child topology.

The new original-word reference freezes and executes:
- Complete player handler `[1A16F0,1A1A20)`,204 words
- Complete manager-held selector `[128520,128600)`,56 words
- Complete state gate `[1B0B80,1B0BB0)`,12 words

Action/helper calls and the final child-router call are explicit synchronous
oracle hooks. The original selector and gate execute as instructions, rather
than being replaced with three action booleans. Standalone and connected tests
add the existing 48Y original held-query reference and 48Z router/manager tests.
Wrapper, constructor/table and unresolved-helper audit bytes are provenance
only; they do not inflate the executed instruction count.

## Manager-held source selection

`queryManagerHeld49A` reproduces128520:
- Category0 chooses manager+34, the current context source
- Category1 chooses manager+30, the auxiliary source
- Category2 chooses manager+10+4*sourceIndex, one directly selected device
- Any other category returns false without looking up a source
- An absent selected source returns false without calling a virtual method
- Present category0/1 calls source table+18 slot+18
- Present category2 calls source table+18 slot+10
- The source receives the original code and mask, with null scratch-record output
- Any nonzero source result is normalized to true

There is no broadcast loop or negative/all-device selector. Native category2
indices are bounded to0..7, and its concrete 48Y bank admits direct codes0..31
when a permitting mask gate reaches the record. A missing device returns a miss
before code validation. Category0/1 ignore sourceIndex completely; their signed
code is passed through to the supplied source implementation.

Both proven joystick tables resolve category2 slot+10 to143600. The concrete
provider therefore uses exactly-held==1 semantics, the known context-mask gate
and no scratch copy. It never consumes events or held state. Its context presence
comes from this same manager's current-context binding.

Non-null category0/1 implementations remain unknown. They are explicit source
bindings, with absence distinct from present-but-unavailable implementation.
128520 does not itself apply143E30 to those sources. In particular, root flags
alone cannot prove category0 mask80 misses. Constructor127EA0 clears manager+30
and+34; PSX initialization also clears+34. An explicitly supplied null-context
state genuinely proves both player category0 queries miss. That is a valid
initialized/synthetic domain, not a claim that live contexts are always absent.

## Exact player press order

Incoming event and record values do not choose these actions. The player handler
keeps their original identities for its final child-router call and performs the
following three held queries on every invocation.

### 1. Configured category2 condition

Read action6AC from the configuration row indexed by player+FFC. The original
row stride is40 bytes. Query category2, mask3F and source index player+5C. The
mapping-row index and device-source index are different fields; the native API
does not silently equate them.

On a match, require:
- Application->28C byte+29 bit2 clear
- Application->28C->B4 byte+58 bit1 set and bit0 clear, from helper1B0B80
- Player float+1098 strictly less than positive zero

Only then invoke2255B0(player), synchronously. After it completes, write exact
bits40200000 (2.5f) to+1094 and BF800000 (-1.0f) to+1090, in that order.
The original helper's return register does not control these stores. The native
availability result is a support boundary, not a reinterpretation of original
helper return values. Positive/negative zero do not pass the strict comparison.

### 2. Numeric category0 code1A

Reload player+5C and query code1A, category0, mask80. If it matches and live
player+40 equals zero, inspect the lazy global flag originally at gp-7A0C
(global3D5F64 with ELF gp3DD970). When zero, first write2 to global4340A0, then
write1 to the flag. Existing nonzero flag values preserve the global word.

Select exactly one synchronous player virtual from that global word:
- 0: slot94
- 1: slot90
- 2: slot8C
- Anything else: slot88

There is no increment or cycling of the word in this handler. These virtual
implementations are external effect boundaries, not inferred state setters.
Lazy initialization remains visible if the selected effect is unavailable.

### 3. Numeric category0 code12

Reload player+5C and query code12, category0, mask80. After the source returns,
read player+5C again: only exactly0 or1 enters this branch.

Select configuration byte+1125F's low nibble. Original selected rows have stride
228 bytes (E4), with float at+102F0 and integer word at+10388. The typed native
view supplies those fields in a caller-bounded row span. Preserve:
- Copy selected float to player+CB4
- Store selected float minus exact binary32 bits3C23D70A (0.01f) to+CB0
- Store selected word plus1, modulo32 bits, to+CD0

Then synchronously invoke, in this order:
1. 22B170(player, f12=current CB0)
2. 22B370(player, f12=current CB8), loaded after the first helper returns
3. 22B570(player)
4. 22B760(player)
5. 22BA60(player, a1=12)

All original action return values are ignored. The second scalar argument is
live: a first helper's CB8 change must reach22B370. Later unavailable calls retain
all earlier direct writes and completed external effects.

### Final child forwarding and release

After all reached local actions, call the complete press router142350 using the
same incoming event and record references. The handler returns only that router's
handled result. A successful local action does not consume the event or suppress
children. Event/record contents can still be changed indirectly by supplied
external bindings; no blanket immutability is promised.

`playerEventChild49A` binds one explicit caller-selected48Z child index. Press
runs the handler above. The proven inherited release path runs1422A0 through
unchanged48Z code, without any held queries, local actions or configuration-row
access. Actual child identity, registration and topology remain caller-owned.

## State, aliases, bounds and failures

Production is serial, iterative and allocation-free. State, configuration arrays,
source bindings and node storage are caller-owned. Callback lifetime obligations
from48Z remain in force. Opaque effect/source callbacks must honor the typed
bindings' lifetimes and execute synchronously. The library cannot validate an
arbitrary external callback or guarantee its behavior.
Each effect provider must be bound to the same player passed to
`handlePlayerPress49A`; its opaque context carries that already-resolved identity.

One press performs at most three manager queries and seven local effect calls,
then one caller-budgeted child traversal. Allocation/stack/time guarantees cover
the recovered core, not the unresolved effect implementations or arbitrary
caller callbacks. No whole-frame or full-action resource bound is asserted.

Enclosing player, environment and binding objects must stay alive throughout the
call, as must borrowed event/record objects. Rebinding configuration spans is
allowed only to valid complete typed rows. Destroying/replacing those enclosing
objects, heterogeneous byte overlays and player/configuration memory overlap are
outside this contract; this is not an arbitrary emulated-memory alias model.

The original reloads configuration globals between the three branch3 direct
stores. Within the native single-threaded typed domain, no callbacks occur
between those stores and configuration rows do not overlap player state, so the
same selected row can be read directly. Opaque source/action callbacks between
other phases may change all declared bindings; later queries and fields are read
live. Configuration indexes are checked only when their row is actually reached.
No original resource contents, mapping index limits or physical button labels
are fabricated from the native span sizes.

Finite strict host binary32 operands/intermediates are admitted where comparison
or subtraction is performed. Invalid numeric inputs stop at a native support
boundary, not an emulation of original exceptional FPU behavior. This is not a
complete EE NaN/exception model. Unused numeric fields are not broadly rejected.
Integer count increment wraps exactly; state and source selectors remain numeric.

Failures are in-place and non-transactional. Returned progress distinguishes
completed queries and invoked effects. Missing source implementations, action bindings,
unsupported numeric/index domains and failed child routes retain prior effects.
No automatic replay/rollback is safe for arbitrary callbacks. A missing first
action does not apply its following timer stores. A missing later action does
not erase already completed direct writes.

## Validation and limitations

Final counts, exact hashes, sensitivity controls, independent review and host
footprint are in `evidence/playerEvent49A/validation.json` and the49A checkpoint.
The complete preceding48Z/Y/X/W/V/B input regressions are included. Tests separate
executed original instructions from synthetic hook behavior and from static
provenance. Connected tests use packet-produced held state rather than invented
held-query booleans, and deliberately expose an unavailable mandatory effect.

```
python3 tools/gameplay/player_event_recover49a.py --check
python3 tools/test_native_player_events49a.py --output-dir ../player-events49a-release --stack-usage
python3 tools/test_native_player_events49a.py --output-dir ../player-events49a-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_player_events_sensitivity49a.py --output-dir ../player-events49a-sensitivity
```

Use fresh output directories outside the source tree. CMake options are
`RR_NATIVE_PLAYER_EVENTS_ONLY` and `RR_NATIVE_PLAYER_EVENTS_SANITIZERS`.
CMake is absent and configuration/CTest remains unexecuted. LeakSanitizer is
explicitly excluded due to host ptrace; ASan/UBSan remain required.

Still open: concrete action helpers and non-null category0/1 implementations,
physical names/configuration resources, device/OS transport, readiness/polling,
live topology and complete event/simulation scheduling. There is no full-race,
rendering, current Windows or original-Xbox validation or gameplay FPS claim.
No PC/RAM, negative-history investigation, installs or pushes were used.
48Z and earlier source/evidence are preserved. Collision capacity remains
unresolved; its source/evidence and grouped-query/detector guards are unchanged.
