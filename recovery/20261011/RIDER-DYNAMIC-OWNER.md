# Concrete Rider dynamic widget callback owner

New pair only, staged source. Uses frozen/integrated original character/pod cold
assets, model scene/playback/IK owners and frozen shared mixed-widget manager.
No builds, syntax checks, project parser execution, tests, sanitizer or gameplay.

## Concrete behavior

Bind at cold119-widget construction. Parse authentic repeated-key-preserving
Widgets.Dat visibility flags with original false defaults. A full mixed callback
batch preflight lazily prepares each required authentic cold model, then requires
real backend resource/capacity preflight before any visible callback executes.
Cold data may persist after rejected preflight; no graph, RNG or navigation effect
is represented as having happened. Missing actual model/motion resources reject.

Each callback executes synchronously in exact mixed-manager order. Character
select loads actual graph, samples real entry.001, invokes backend actual entity,
visibility membership and parent resource synchronization, captures real155730,
binds sampled IK and then does generic selection and source latch clear.
Character clearSelection is virtual88=1D2650: it also LOADS if absent before
clearing selected. It is not generic 'just clear a bool'. Show/reset do not load.
Character deselect drains/detaches/destroys model then generic deselect. Reload
retains widget idle counters/lastIdle/readiness as original lifecycle requires.

Pod select calls generic selection twice around real load, unpauses and recursive
shows. Deselect generic, conditionally unload CleanFromMemory, generic again,
pauses and recursive hides. Source constructor1D30F4 initializes delay250 FLOAT0;
create1D31D0 does not set it. This candidate admits the actual normal immediate
Rider branch, not a fabricated2500ms timer. Externally changed positive delay and
its original elapsed wall-clock branch remain outside this bounded owner.

Generic simultaneous EnableOnSelect+DisableOnSelect (or corresponding deselect)
suppresses both visibility calls, as original2A78C0/2A7940. setEnabled uses direct
2A7A00 selected+recursive visibility, not character+88/load. Repeated commands are
not collapsed. State publishes only after callback success. Partial failure
poisons the owner and mixed manager; earlier actual side effects aren't rolled
back or retried. Explicit drain/destructor cleans every registered or partly
registered scene before its graph can be destroyed.

advanceWidget is called in actual enabled widget traversal order; it uses the
same application RNG, motion/IK and source-libm owners supplied at bind. No seed,
80%-observation reorder, fake idle duration or private shared-state copy. Caller
continues child traversal after it. confirmCharacter and sourceCharacterReadiness
retain source unloaded behavior. The readiness method is strictly original page
polling; it never means renderer/destination-ready.

## Required concrete backend, deliberately not stubbed

The interface requires actual synchronous renderer/entity/cache owners:
- preflight material/image-bank, traversal and callback capacity for entire batch;
- attach priority2 model, true28E8A0 UI visibility membership, actual binder helper
  instances priority100A, then parent+A4 resource synchronization;
- recursive141630/1415A0 shown state on real widget children;
- drain packets, disable/unregister/remove membership/detach before graph destroy.

Parent+A4=2A4B30 invokes14D370,29E340,14D440 resource synchronization. It cannot be
replaced by an acknowledgement flag. Actual model19C membership also selects
character zone ambient; see ../model-packet. Backend attach gets the REAL model
scene/graph and pinned cold resources, never a substitute rig or a fake draw token.

The backend remains unjoined to shared native renderer in this package. Therefore
this callback owner is useful concrete scene orchestration but NOT a completed
Rider destination. Root must supply the real backend before invoking callbacks.
All shared application owners/backend context outlive this object; drain it before
backend or widget-manager destruction. Alias/pointer projections must be drained
before any native renderer bank switch. The combined image bank lives separately
in ../image-bank; current lit renderer packet API is sibling50KM work.
