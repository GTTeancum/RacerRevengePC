# Application-owned APPSTART material templates

Source-only ownership fix. No build, test, syntax/import/lint check, sanitizer,
gameplay, decoder, guest execution or computer operation was run.

## Fixed lifetime

`OriginalUiMaterialApplication50LA` retains the authenticated APPSTART archive
and its actual DefaultMT/ScreenMT objects for one native application. The
existing `original_main_entry50KG::Application` owns it before the main
frontend/race restart loop, including direct gameplay with Original Main off.
First actual material use performs the existing authenticated loads. Returning
from a frontend, Course cold handoff, race restart and later Main visits all
reuse the same retained archive and template object addresses.

Each `OriginalUiMaterialTemplates50KG` is now a shared immutable handle. A
serial first load constructs both templates with the unchanged existing
`loadTemplate` code and publishes them together into its authenticated APPSTART
owner. Later handles for that exact archive pin the same backing. The archive
and template handles both retain it; the backing does not retain the archive,
so there is no cycle. The application additionally retains the owning archive
for material consumers that need its actual member bytes.

The existing frontend coordinator already passes one `cold.appStart` through
Main, Options/Controls, Rider and Course resources and nested model, stat,
binder and hologram loaders. Their existing template handles now borrow the
same stable backing without changes to their public load APIs. Gameplay
Binder/particles and the human race HUD now receive this same application
archive too. Local template handles are safe to destroy at frontend teardown.

## Preserved identities and failure behavior

- Existing APPSTART archive and member authentication, DefaultMT/ScreenMT
  source values, original24A8E0 defaults and template-file overrides are
  unchanged. `3833E8` remains DefaultMT; `3833F0` remains ScreenMT.
- Source member identity pointers still name the immutable archive catalog.
  Backing owns copied strings and fields, not borrowed archive byte ranges.
- No template backing is published if either actual template load fails.
  Failed application preparation remains empty and retryable.
- A second load on an already loaded template handle still returns
  `alreadyLoaded`. A second application prepare is also rejected; later
  sessions borrow its prepared owner instead of preparing again.
- `OriginalUiMaterialRuntime50KG` and application `materialRuntime` are
  unchanged. Runtime material overrides remain applied to each pass, never
  baked into the shared template or reset by a session transition.
- Publication is serial cold work, matching current UI/resource construction.
  This is not a new concurrent-loading API.

## Scope and next consumer

The independently staged AIDamage cold row retains a raw ScreenMT pointer.
Its future application owner can borrow `materialApplication->appStart()` and
`materialApplication->templates()` while retaining this same application
lifetime. This patch does not construct that row or an overlay, synthesize a
material directory, entity counter/type, race context or outer ancestor graph.

It also does not join portal activity, wreck effects, the particle forest,
destruction or nonflight execution. Their existing source boundaries remain.
No new translation unit or build registration is required.
