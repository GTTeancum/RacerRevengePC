# Original UI transition owner 50KG

This inert native owner translates the bounded manager request lifecycle. It
does not activate the frontend, allocate a destination, emulate guest code, or
start gameplay. One noncopyable owner holds current/requested original IDs,
manager lifecycle and transition phase. Closed registry identities are required
but never constitute native destination support.

## Evidence

Source: original `SLUS_202.68`, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Static evidence is staged in the recovery `ui-route-reconstruction` package:
`README.md`, `evidence/page-request-helpers.txt`,
`evidence/ui-manager-lifecycle.txt`, `evidence/launcher-consumer.txt`, and
`original_ui_route_registry50kg.h`. Addresses in native events are provenance
only, never function pointers or guest memory accesses.

- `0x1D6130`: screen request saves page/screen, then lifecycle 6.
- `0x1D6190`: same-screen request saves page/preserves screen, then lifecycle 7.
- `0x1DED58`: course confirmation requests lifecycle 8.
- `0x2A3C80`, table `0x3BEC60`: lifecycle dispatch.
  - Screen: `6 -> 3/exit -> 9/none -> flags 2(screen,page)`.
  - Page: `7 -> 4/exit -> 1/none -> 4/entry -> 0/none`.
  - Course: `8 -> 5/exit -> 10/none -> flags 9(0,0)`.
- `0x2A3E50`, `0x2A3EA4`, `0x2A3F24`: page virtual+0x80
  completion gates. No elapsed-time inference is used.
- `0x2A4020`: previous page virtual+0x94, requested page virtual+0x90,
  then publication of current page/index. The following state-1 branch invokes
  page+0x74 and manager+0x84 before starting entry with page+0x7C(1).
- Source child virtual+0x1C/+0x24 notifications retain their exact relative
  order. Course invokes +0x1C immediately after exit start; screen/page invoke
  it after exit completion. Entry invokes +0x1C after start and +0x24 after
  completion. Unknown child semantics are not relabeled as animation/audio.
- `0x2A3F70`, `0x2A3F98`: queue calls to `0x17DE10`. Flags are application
  requests; PodLauncher bit-8 teardown and bit-1 loading, and kind-0
  LoadingPageContainer/PodRaceEvent initialization, are **not implemented here**.

## Admission and callback contract

`bind` adopts an already-entered bounded native page without entry callbacks.
Each request validates the actual closed identity and a caller's read-only
native admission before changing manager state. That admission must establish
all assets, controllers, callback owners and their stable lifetimes, rather
than infer support from resource names. Gallery image screens and other pages
outside the bounded page registry remain explicitly unsupported.

The main-decision adapter only accepts exact successful dispatch shapes from
screen 4/page 1. Slots 1–3 also require an atomic caller-owned profile commit,
after route admission and before manager state changes. Rejected commits must
have no effects. Mode34/player bits stay original values; subtype11258 and
host race modes are separate. Generic request methods do not write profiles.

Course launch requires screen 6/page 1 and independent caller admission of the
fully committed original course configuration and native loading/race owners.
It does not fill in missing track-control values, riders, subtype, course
profile transaction, or global UI+0x474/+0x478 clearing. Those responsibilities
remain with the native course/profile owners before any lifecycle tick.

Each tick advances one source switch case. Callbacks receive exact native
route/phase and source call-site metadata; the caller implements the admitted
native effects. Pending or unavailable completion does not advance a gate.
Notifications cannot fail after admission. Queue rejection must be effect-free
and may be retried; acceptance happens once and leaves this owner terminal.
The queue callback must defer consumption until tick returns and must not
destroy this owner. No source manager teardown/entity update tail is claimed.

Native restrictions are explicit: only one request is admitted while idle;
retargeting during entry or before exit starts is rejected although source
helpers specifically checked transition 2. All callback reentry is rejected.
Initial/resumed state 2, ownership handoff after queue consumption, page asset
loading, transition durations and rendered completion remain outside scope.

## Verification status

Source editing and static reading only. No test, build, syntax check, importer,
asset decode, guest execution or game run was performed for this addition.

## Route-leaf transaction adapter

`requestRouteDecision` connects the Options/Gallery/Rider/Course-back decision
leaves to this lifecycle. It copies the entire bounded decision before any
callback, requires the current source screen/page and all admission flags,
rejects course-launch proposals, and independently checks destination ownership.
Only then may `commitRouteSelection` atomically validate freshness and commit
all proposed profile changes. Its false return must have no effects. On true,
the manager publishes the already-admitted route through an infallible state
write; no launch or callback runs between commit and publication. This adapter
supplies the lifecycle side of the transaction, not a profile/save owner or a
finished page controller. Source review only; not compiled or executed yet.
