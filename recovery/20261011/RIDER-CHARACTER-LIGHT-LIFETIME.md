# Application-owned Rider light-cache lifetime

Source-only candidate, 2026-10-11. No active repository edits, builds, tests,
syntax/sanitizer checks, project parser runs or gameplay. Static ELF byte reads
and textual source inspection only.

## Result

The authentic initial Main-to-Rider flow can establish cache absence from the
actual application constructor. It does not require inventing a cache miss or
assuming UI teardown resets the cache.

`original_ui_light_manager_lifetime50kg.h/.cpp` owns that bounded shared native
lifetime and actual Rider small light registry. It is an APPLICATION object
constructed once before the first Main manager, retained across Main/Options/
Rider, and borrowed by the Rider backend. A Rider-local fresh instance is not
a valid substitute. Unsupported gameplay or unknown prior history permanently
invalidates its constructor-absence proof.

The same package fixes an independently discovered Rider lighting rejection:
`genuine()` compared the authenticated catalog identity name against lowercase
`ui\uirider\ui.dat`. The catalog actually stores `UI\UIRider\ui.dat`. The staged
one-line correction uses that exact spelling, retaining both the pointer
identity test and archive/catalog provenance. No broad case folding is added.

## Source evidence

- `140074..140098` allocates one 0xB0 LightManager, calls constructor `147ED0`
  at `140084`, then attaches it under the application's entity with mask0x402.
  This number is an entity mask, not a priority.
- `147F8C` publishes the shared manager; `147FF8` initializes its A4 pointer to
  null. Static direct-call search found the constructor called at140084 only.
- UI destruction `1CCB04` invokes `148110`; that method frees zone data but
  never writes A4. `148B60` also calls148110 before zone setup and preserves A4.
- UI `1CCD88` calls148B60 with one zone. Its constructor establishes zone48=0.
- UI light construction `1CDB10..1CDCB0` loads the authenticated Ambient then
  Directional rows and registers them at148F30. The inspected registration,
  deregistration, ambient/color updates and Main particle-lighting path do not
  install an A4 cache. This is a bounded UI-path conclusion, not a claim that
  arbitrary unrelated gameplay or imported history can be ignored.
- `14A0E8..14A108` first examines actual A4 and only calls14A860 when non-null.
  The staged owner reaches the original null branch through its retained
  constructor state, with no cache-hit/miss assertion.
- `14A118..14A120` obtains the zone through model membership+4/zone+48.
  The staged method requires a current selection from the concrete visibility
  owner and the authenticated character asset for that same widget.
- `14A128..14A154` selects149BB0 when count94<5 and returns directly. Its small
  path never reaches cache insertion14A724, even after a real cache miss.
- `14A940` searches an existing cache by the membership key. `14A860` copies
  all three matrices and updates the slot's use stamp on a hit. `14A770` inserts
  at14A990's first zero-stamp/oldest-stamp slot only on the large-registry path.
  None of those routines creates or installs a cache into LightManager+A4.

The evidence directory retains the application creation, manager lifetime,
registration/update code, full cache lookup/insertion/replacement listings, and
the inspected shared-manager references. The broad A4-store candidate list is
an address-navigation aid, not by itself a proof about every possible alias.

## Root/composer integration contract

1. Add this owner to the long-lived native application/frontend storage.
   Invoke `constructAtApplicationStartup(diag)` once, before initial Main.
   Never manufacture an owner during Rider cold preparation or after a race.
2. At actual initial Main manager entry, invoke
   `beginUiManager(*mainArchive,diag)`. The first entry must be authenticated Main.
3. Cold destination asset preparation does not change this owner. At actual
   screen handoff, finish/drain the outgoing source's packet/light use, call
   `releaseUiManager()`, then `beginUiManager(*destinationArchive,diag)` before
   the incoming manager starts using its lighting. Page changes within one
   Options manager do not replace this lifetime or repeat these calls.
4. Rider backend borrows this SAME object. Once actual two-light creation or a
   source color update has completed, call
   `publishRiderRegistry(currentRiderLighting,diag)`. This retains Ambient,
   Directional registration order, current direction and mirrored3C/4C, plus
   actual ambientSlot60. Failed publication disables stale registry reuse.
5. After real visibility selection, call
   `selectRiderCharacter(characterAssets,selection,matrices,diag)`. It validates
   the shared lifetime and genuine current zero-zone member before invoking
   the existing original149BB0 numeric leaf. Pod/global1499A0 and binder ambient
   paths remain distinct and do not use this method as a substitute.
6. On leaving the joined Main/Options/Rider flow for gameplay or any untracked
   owner, call `invalidateForUntrackedScene()` before that work. A return to
   Main does not restore proof. Subsequent character selection remains blocked
   until the real shared-cache or extended lifetime path is joined.
7. At actual application teardown, call `destroyApplication()`. This is terminal
   and cannot be reused as a convenient per-screen reset.

Add the new lifetime `.cpp` to the appropriate root build source list. It depends
on the previous visibility package and existing Rider lighting/scene types. The
package includes a repository-relative patch, exact preimage for the one-line
lighting correction, and hashes identifying all staged source.

## Boundaries and pending validation

This is a native direct implementation of the source-backed constructor-null
UI path. It does not reconstruct an arbitrary existing cache, create an original
cache allocation that has not been proven, or emulate guest memory/registers.
It deliberately refuses unknown prehistory instead of setting `cacheMiss=true`.

Root must wire the actual application lifetime and serial handoff above before
using its result. No independent validation was run. Future central coverage
should include first-Main requirement; Main/Options/Rider round trips preserving
cache state; rejected Rider-local first entry; unknown-scene invalidation that
survives release/reentry; actual locked/unlocked light updates; zero-zone/current
visibility identity; and catalog-case capture/apply acceptance.
