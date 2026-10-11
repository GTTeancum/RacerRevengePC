# Native application light-lifetime wiring

Static implementation package, 2026-10-11. No active repository edits. No build,
test, syntax/sanitizer check, project parser, gameplay or emulation was run.

## Integration base and deliverable

Apply `application-light-lifetime.patch` centrally, or copy the four files from
`files/` after comparing the corresponding exact `preimages/`. The patch is an
incremental overlay on Game Options Apply, integrated by root in **b4a2d96**.
Its finalized frontend/header/entry preimages came from
`/workspace/shared/recovery-20261011/game-options-apply/files/`. At the final
static read, active HEAD was **2e24d9e** and all four preimage hashes still
matched the active files. This package preserves the Apply owner's Accept,
shared settings and concrete audio changes.

Prerequisite: **3d88ef4** (source-backed LightManager lifetime leaf, canonical
Rider member spelling, source lists), and its already-integrated visibility
prerequisite. No build-list change is needed here. `active-preimages/` is the
historical snapshot before Game Options Apply; it is NOT the application base.
`preimages/` is the integration authority. `SHA256SUMS` covers both preimages and
staged files; `BASE-SHA256SUMS` contains the four exact repository-relative bases.

Changed files:

- `tests/native/original_ui_main_entry50kg.h`: actual application startup owns
  and constructs one shared lifetime object; frontend cold input borrows it.
- `platform/linux/native/original_ui_frontend50kg.{h,cpp}`: require that actual
  application owner, track its generation/archive lease, hand it across Main
  and Options activation, and release only after consumers have closed.
- `src/game/original_ui_light_manager_lifetime50kg.h`: read-only active archive
  identity accessor for exact lease matching. No cache arithmetic or cache
  state-transition implementation changed.

## Actual startup and teardown

The startup adapter is application code despite its `tests/native` directory.
`terrain_interactive_drive50du.cpp:4313` constructs `Application` before the
session loop; `runSession50DU` receives it by const reference. The frontend is
local to that session (line2667). Its existing close boundary (lines2671-2693)
closes renderer and coordinator with cleanup retries before resources/context
can unwind. Unresolved ownership terminates rather than ending the context
under borrowed GPU names.

The enabled Application constructor allocates the new owner beside its shared
profile/settings/application state, calls `constructAtApplicationStartup` once,
and never creates another at Main, Options or Rider entry. The custom shared
pointer deleter calls `destroyApplication` exactly when the final owner releases
it. Application normally outlives every frontend; the frontend's retained Cold
input also pins this same object through exceptional unwinding. Storage declares
Cold before its native consumers, so reverse destruction drops those consumers
before the final Cold/shared pointer. Per-screen and frontend close do not call
`destroyApplication` or redo147FF8.

## Coordinator ownership and failure paths

These are static source-path analyses, not executed test results.

- Cold preparation requires an already-constructed, known constructor-absent
  cache and no active manager. Preparing or discarding pending resources does
  not begin or release a light manager.
- Initial Main enters only after the real renderer accepts its texture bank,
  before initial-page callbacks. Failed prior preparation/binding leaves the
  light owner untouched. Failed callbacks retain the entered lease for close.
- A lease comprises the retained owner, actual catalog archive pointer and its
  monotonically increasing manager generation. Runtime admission/render/submit
  rejects unknown lifetime or a displaced lease. It cannot release a different
  manager merely because both screens use the same catalog identity.
- Main/Options screen switches finish outgoing GPU work and bind the target
  bank first. Then they release the outgoing UI manager and begin the actual
  authenticated incoming archive before its callbacks. A failure after this
  boundary faults the coordinator; it cannot resume old-source work against
  incoming lighting. Old resources remain pinned for existing retirement.
- Options menu/Game Options page transitions keep the same lease/generation.
- Generic resource `release` never touches the process light manager: it also
  handles cold pending owners and outgoing resources after the target is live.
- Close releases the light-manager lease only after renderer, retiring, pending
  and active session/GPU releases all succeed. A GPU cleanup failure retains the
  lease for retry. A mismatched lease invalidates lifetime proof without releasing
  another owner's scope, clears the local lease and reports cleanup failure;
  a later close can finish already-drained local storage without clobbering it.
- Successful close leaves the process owner and constructor-absent A4 state
  alive. Preparing a fresh Main with the same Application owner is supported;
  its manager generation increases rather than resetting the application.
- Destroying a coordinator without completed close marks the shared lifetime
  untracked. It does not manufacture cleanup or a fresh absent-cache claim.
  Subsequent Main preparation rejects that owner.
- The frontend snapshot exposes cache state, generation and current lease
  ownership for root's centralized integration validation.

## Rider integration dependency

This overlay implements only the concrete Main/Options owners presently in its
base. Rider composer must extend `commitDeferred`'s destination-archive branch
with the real retained Rider archive when adding its actual manager. Pass this
same `Cold::lightManager` into the Rider backend. The backend must not construct,
begin/release or replace it: coordinator owns those boundaries. It can publish
the actual Rider registry after activation and query character lighting using
its authentic current visibility selection. No local absent-cache fallback.

If any gameplay or unjoined manager is later admitted, invalidate the shared
lifetime BEFORE entering it, until that scope has an actual source-backed shared
cache/lifetime implementation. Reopening Main cannot restore the proof.

## Source evidence and validation boundary

Source evidence is retained in sibling `../character-light-cache/evidence/`:
`application-manager-construction.txt`, `manager-lifetime-update-create.txt`,
`ui-manager-destruction.txt`, `ui-light-population.txt`, and
`per-model-light-select.txt`. Source140084/147FF8 creates A4-null once;
1CCB04/148110 UI destruction and148B60 zone setup preserve A4. 141960's0x402 is
an entity mask. This package implements the concrete owner handoff described in
`../character-light-cache/INTEGRATION.md`; its lifetime claim remains bounded to
tracked Main/Options/Rider.

Root retains centralized integration and execution validation. The new Cold
owner is mandatory; an external fixture/caller must provide its actual startup
owner rather than relying on a newly synthesized frontend-local instance.
