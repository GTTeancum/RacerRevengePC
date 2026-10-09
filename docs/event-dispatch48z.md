# Supplied-event manager and child routing: 48Z-EventDispatch

2026-10-06. Cloud/Linux continuation of preserved 48Y-DeviceHeld.

## Production closure

`src/game/event_dispatch48z.h/.cpp` connects 48Y's ordered transition batch to a
caller-owned event queue, recovers the original manager's context rewriting and
mutable callback dispatch, and implements both complete GameObject child routers.
A narrow, constructor/vtable-qualified application bridge connects category2
joystick events to those routers in the normal mode. Concrete child effects
remain supplied bindings, with explicit unavailable results rather than invented
player/menu actions. No physical control names or host device transport are added.

The new production path is serial, iterative and allocation-free. It does not
choose global queue ownership, device polling frequency, a complete gameplay
scheduler or a frame/substep ordering relative to the existing engine/aero path.
It is now possible to route real packet-produced numeric transitions through the
recovered manager and normal application path, but a live platform input driver
and original concrete gameplay leaves remain missing.

## Event and record contracts

An event contains five 32-bit words, in order: code, category, device ID,
transition and context mask. `appendDeviceTransitions48Z` preserves 48Y's original
ascending descriptor order and all five values. It prevalidates the whole batch
and appends without partial writes on a native capacity error. The capacity is
caller-supplied storage, not a recovered original queue size or overflow policy.

`resetInputEventCount48Z` only resets count, preserving storage. The original
manager resets count before its device-update prepass; the native helper does
not run that prepass or imply it is safe to reset at an arbitrary gameplay point.
Dispatch does not remove records or clear count. A second dispatch visits the
same queue unless the caller or a callback changes it.

The dispatch loop is original `[12815C,128430)`, after the unimplemented prepass:

1. Test live signed count. Nonpositive count means no iterations.
2. Before each reached event, overwrite contextMask from live context+1898,
   or zero when the current context is absent.
3. Select category once, before any callback. Admitted categories are 0, 1 and 2.
4. For transition0, call application slot+3C with the event and selected record.
5. Reload transition. If it is now1, call application slot+40, including after
   the same event's release callback. This is not an exclusive if/else branch.
6. Reload live count before deciding whether to continue to the next event.

Record addressing is category-specific, with a 24-byte stride:
- Category0: current context object+20+24*code
- Category1: manager+30 auxiliary object+1C+24*code
- Category2: manager device[deviceId]+324+24*code, from eight device slots

Each callback re-resolves current code, device and source binding. A release
callback may retarget the next press record, replace a source span, change
context presence/mask, append events or shrink count. Changing category does
not change this event's already selected route. Changing context does not
retag the same event before its second callback. The currently borrowed record
reference remains that callback's original record even if it changes event code.

The manager ignores the original callback return value, even if nonzero. Other
transition values call neither hook. Such events need no record lookup. Missing
record bindings are checked only when a hook would actually dereference them.
Native bounds reject invalid records instead of reproducing unchecked pointers.

## Child router and application bridge

The complete original GameObject routers are `[1422A0,142344)` for release and
`[142350,1423F4)` for press. Their contracts are:
- Check parent aggregate flags+24 once: mask200 for release, mask400 for press
- Load first child+18 once, then visit the live sibling chain
- Require child byte+29 bit0 and exclude bit3. Bit2 does not exclude a child
- Do not add an individual child ownMask+20 test
- Invoke child virtual+60/+64 with the same mutable event and record references
- Nonzero result immediately returns handled and stops this router's siblings
- Otherwise read that child's next+14 after its callback, so link edits are live

Consumption stops this router only. It does not stop the manager's remaining
events. A caller-supplied visit budget bounds cycles, including cycles introduced
by callbacks or containing skipped nodes. That budget is an explicit native
support boundary, not an asserted original traversal limit. This is a single
iterative router, not a recovered recursive object tree or fabricated topology.

Source provenance identifies Game, PSXGame and PodGame constructors and their
application tables. All slot+3C entries are140EA0; slot+40 entries are140EF0,
2376F0 and17CBC0 respectively. Game initialization stores a GameObject pointer in the application +26C field;
its table3CD240 binds slots+60/+64 to the recovered routers. Frozen ELF table
identity does not establish a live initialized process or actual child topology.

The `JoystickApplicationBridge48Z` admits category2 only:
- Root byte+228 bit3 consumes immediately without forwarding
- Normal press requires bit2 clear; the special graphics/interface branch is opaque
- Release and normal press use the recovered child router
- After an unhandled press, bit2 is tested again because a child may have set it
- Handled press returns before that special fallback, as the original does

Category2 bypasses PodGame's category0-specific state mutations. Concrete child
callbacks remain explicit mutable bindings. Wrappers and constructor/table
provenance are source-reviewed and native-contract-tested, not executed by the
manager/router oracle. No wrapper instruction is counted as differential coverage.

## Ownership, bounds and partial effects

Queue, record and node storage belong to the caller. The production path allocates
nothing and creates no threads. Queue capacity remains fixed during dispatch;
positive count must fit it. Records use caller-provided typed spans. Events
completed and callbacks invoked are reported separately.

Unlike the prior transactional numeric adapters, dispatch cannot roll back
arbitrary external callback effects. On missing bindings, unsupported category,
unavailable callback, invalid live count, changed storage or exhausted visit
budget, all completed effects and the reached prefix remain. This includes the
current event's rewritten mask and any callback changes before failure. Retrying
the entire queue may repeat effects; callers must choose recovery explicitly.

After a manager callback, queue pointer/capacity changes are reported before
another event access. After an unhandled router callback, child pointer/count
changes are reported before reading the next link. A handled/unavailable child
returns without another child-storage access. Borrowed event/record references
must remain alive throughout callbacks and nested calls. Native guards cannot
validate arbitrary external callback lifetimes or make an unsafe callback safe.

## Original evidence and exact proof boundary

ELF SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

`tools/gameplay/event_dispatch_recover48z.py --check` verifies frozen bytes and
three executed intervals: 181 manager words and 41 words in each router.
The independent original-word interpreter executes branches, delay slots,
branch-likely annulment and complete router stack prologues/epilogues. Synthetic
state is decoded around each explicit callback and re-encoded before original
execution resumes. Native source is not used by this reference.

All 235 admitted words execute: 153 manager words and both 41-word routers.
The retained but unexecuted 28 manager words belong to the unknown-category
branch/assert body, explicitly outside categories0..2. Their exact PCs are
asserted in tests and recorded in the recovery manifest. Four wrapper intervals
and four tables are frozen as provenance only. Independent constructor/init
excerpts confirm the identities without claiming live memory or runtime topology.

Native queues/spans use caller bounds; oracle fixtures are at most 64 events,
32 records per bank, eight devices and 32 child nodes. These are synthetic proof
domains, not original queue-capacity findings. The independent review separately
tests live storage replacement, partial effects and native error boundaries.

Still excluded:
- Manager transport/update/rebuild prepass `[128080,12815C)` and helper128450
- Device readiness negotiation, SDK/OS transport, polling and rumble
- Negative-code history semantics or unproven physical action names
- Special application modes, category0 PodGame effects and concrete child handlers
- Original live object topology and complete event/simulation scheduler
- Full races, rendering, original-Xbox build/memory/FPS and current Windows builds

The original collision buffer-capacity contract remains unresolved. Existing
collision/grouped-query/detector files and all prior input source/evidence are
preserved; this milestone does not modify or investigate those contracts.

## Validation and reproduction

Final counts, hashes, sensitivity controls and review results are in
`evidence/eventDispatch48Z/validation.json` and the 48Z checkpoint. The whole input
chain passes 4,379,437 checks in each normal and ASan/UBSan run. New 48Z contributes
1,316,164 checks, 2,511 manager fixtures, 3,272 router fixtures, 26,591 exact
callback traces and 1,848,633 executed original instructions. Earlier 48Y/X/W/V/B
regressions remain unchanged. All 52 candidate-only semantic mutations fail by
expected assertions, with no compile failures or crashes counted. Hot-loop probes observe zero allocations. An
independent source audit and 73 extra normal/sanitized checks pass.

On Linux x64, GNU size reports 2,275 new text/read-only bytes and zero data/BSS.
GCC per-function stack estimates are 8 bytes for reset/append, 128 for manager and
router, 32 bounded bytes for the shared bridge and 16 for exported wrappers. Caller
state, nested calls and callback frames are additional; these are not a whole-frame
memory bound, Xbox guarantee or performance measurement.

```
python3 tools/gameplay/event_dispatch_recover48z.py --check
python3 tools/test_native_event_dispatch48z.py --output-dir ../dispatch48z-release --stack-usage
python3 tools/test_native_event_dispatch48z.py --output-dir ../dispatch48z-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_event_dispatch_sensitivity48z.py --output-dir ../dispatch48z-sensitivity
```

Use fresh output directories outside the source tree. CMake integration options
are `RR_NATIVE_EVENT_DISPATCH_ONLY` and `RR_NATIVE_EVENT_DISPATCH_SANITIZERS`.
CMake is absent and was not executed. LeakSanitizer is explicitly excluded due to
the host ptrace limitation; ASan and UBSan are enabled in the sanitizer run.
