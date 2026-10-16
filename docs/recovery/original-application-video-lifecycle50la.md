# Application-owned native Video lifecycle

## Existing production callers

The existing `runOriginalFrontend50LA` and `runSession50DU` now obtain their
native Video from `Application::videoForSession`. Application retains one Video
object for its whole serial lifetime. The same immutable mode is used on every
entry: Original Main uses 640x448; direct gameplay keeps 640x480.

The existing frontend/race cleanup still retires the renderer and all local GPU
texture owners before closing the real host. Restart and Course handoff still
create new native windows/GL contexts and reload session GPU resources. This
change preserves the C++ Video identity, not native GL objects, context handles,
texture names, readiness or rendered output. No original retail display-lifetime
parity is claimed.

Reentry rejects a still-created host or any unretired native host resources.
Application cannot be copied, and Video construction is private to Application.
No session frame, original AI, geometry, asset, material-selection, alpha,
sampler, HUD, lighting, capture or Windows host implementation is changed.

This production-only subset uses existing source/header dependencies. It does
not depend on the dormant canonical material/program/texture packets and adds
no build registration. It is source-only and untested.

## Optional canonical CPU context continuation

The separate continuation after the texture-demand packet caches one exact
`OriginalStandardMaterialContext50LA` in the same Application. Canonical tables
pin that exact capability and cannot silently replace it with a fresh but
field-equivalent capability on restart. The factory requires the actual
Application-owned Video and retains its authoritative shared-pointer control
block, rather than trusting a caller pointer with merely the same raw address.

Context, Video, immutable material runtime, authentic APPSTART/templates and
the distinct Application443870 CPU texture list have no ownership cycle:
Application owns Context and Video; Context owns their required backing; none
of that backing owns Application. Resource cleanup can still unregister after
host closure. Material work requires the host to be actually created/current.
After complete session cleanup, reopening the same host owner and unchanged
source mode permits CPU material work through the same retained capability.
No old GPU publication is made valid by this.

The canonical factory still admits only originalMain/640x448x32, depth-base280,
depth-format0x31. Direct640x480 remains outside that proven construction domain;
it is never assigned 448-line fields. Existing direct rendering is unchanged.

## Actual track consumer remains a separate, blocked join

The real existing route remains `track_materials50fa_fixture::Owner::load` ->
`bank`/`spans` -> `trackGpuTextures.create` -> `renderRequest.track` -> existing
renderer. This patch does not populate canonical managers next to it and does
not claim canonical resource consumption, demand-driven mip selection or
rendered gameplay. In particular, the following work is still necessary:

1. Extend authenticated track archive preparation to retain exact original
   PSG/PSM/PST members. Current geometry is converted RRSCENE1, PSM pass fields
   are conversion records, and original extraction only occurs for unresolved
   texture callbacks. Converted records cannot replace original whole members.
2. Join the one actual mutable Application36B970 field and its genuine source
   writer/restorer chronology. The source writes `textures\` at17CA6C/17CA70
   and17D814/17D818, but that writer is not yet in this production call path.
   A matching string guessed from a track name is not a join.
3. Use the same full382B38/program382B70/dynamic383400 managers and genuine
   model26E640/1C4 refs, preserving the separate AIDamage shared-manager join.
   Every material slot in a chosen PSG is retained; a single supported material
   does not make a mixed whole-model closure supported.
4. Split source preparation from context-dependent canonical publication. The
   existing track load occurs before Video acquisition and host creation.
   Its ATM consumers also need the existing CPU bank before host creation;
   delaying that bank requires deliberately moving those dependent cold steps.
5. Extend the texture domain for real TA assets. Existing documented
   `ta_sand8`/`ta_rock2` PSTs are kind1, while the canonical resource admits only
   kind0. The concrete MA kind0 `MA_Shield` PSM requires TexGenMatrix/unique
   vertex animation and cannot be substituted into a static no-parameter path.

Once an entire genuine model/material closure is supported, the actual track
slot must obtain material pass -> dynamic383400 row -> canonical resource,
and match that exact original member, chosen mip and dimensions to the existing
authenticated retained RGBA before loading the same50EA bank. The old exported
selector cannot remain independently authoritative for a supposedly joined slot.
The existing native renderer and all original assets stay in place.

Demand-controlled residency still needs actual254AD0/reset and254B20 phases.
Collector14D440, native program lowering, source geometry/child/virtual44 scene
continuation and general EE arithmetic parity remain outside this correction.

## Verification

Manual source review only. No builds, tests, compiler/syntax/lint/scanner,
sanitizer, runtime asset-decoder, gameplay, emulation or guest-execution checks
were run. Packaging hashes identify exact bytes, not validation results.
