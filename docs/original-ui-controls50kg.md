# Original Options Controls owner

This source-only implementation extends the existing Options manager rather than
creating a parallel menu. It uses authenticated `UIOPTION.RES` `config.dat`,
`Widgets.Dat`, original localized labels, and the original controller model and
materials. The shared manager retains its real camera, light-manager lifetime,
texture bank, scene ownership, and material animation history across pages.

## Page and profile contract

- Page 12/2 edits controller index 0. Page 12/3's original index-1 controller is
  prepared but unadmitted in the native frontend. Actual menu-controller input,
  hotplug, and the original missing-controller popup must be joined before it
  can open safely. A caller-supplied presence flag cannot certify that owner.
- Source events 0/1 wrap the four presets in the selected Normal or Advanced
  bank. Event 7 changes bank; event 5 rotates the actual model through its
  authored motions. Events 2/3 do nothing. There is no invented Defaults action.
- Original source callbacks retain their ordered group/widget changes,
  localized labels, model playback observation, background drawing, and aperture
  callbacks. A running model motion uses the original blocked-state gate.
- The application pins one `ControlPresets49D` owner across Main, Options,
  Rider, Course, native race, restart, and return to Main. Original reset starts
  preset 0. The legacy direct race entry retains its explicit preset-4 policy.
- Accept runs the original preset selection against that canonical profile,
  then requests page 1 and emits SELECT. Cancel returns to page 1 without the
  Accept store. Both preserve the source's absence of a remembered-group store.
  This is application-lifetime persistence, not a fabricated disk save.
- Controls labels retain the original preview helper's temporary preset
  selection/restoration, including its actual shared-profile write sequence.

## Native input boundary

The native menu adapts actual keyboard events: Left/Right select a preset,
Tab changes Normal/Advanced, R rotates, Enter/Space accepts, and Escape cancels.
The separately owned native gameplay input adapter must resolve the selected
original action row; a Controls page alone is not evidence of complete physical
controller support, vibration, split-screen, or saved-profile import.

The all-original-AI path neither samples a human driving packet nor depends on
the selected human preset. Source `0x222D34` skips controller feedback whenever
vehicle `+58` is nonzero. Every actual ALL8 racer, including the followed slot,
has that original AI owner. The human-only `+5C` feedback owner remains separate
product work and is not an ALL8 contact blocker.

## Presentation and lifetime

Widget 36 is the real eagerly prepared Soultree controller model. Its motion,
scene observer membership, selected geometry, lighting, and original diffuse
and destination-alpha specular material passes are carried by concrete native
owners. The source RGBA target spans the whole Options bank; no page switch
resets material history or substitutes a placeholder controller image. The
actual authenticated controller header has two nodes, five materials, and one
LOD. Existing native geometry/clipping limits still reject unsupported input;
this packet does not relax those boundaries or claim broad model support.

The background scene setup replaces the earlier UI light registry. Model
selection uses the authenticated opt.scn zone rows (ambient25 and zero
Directional RGB), while the earlier UI fog entity supplies the model's
1200/1000 range. The later scene/particle tail supplies its separate fog state.
Neither the earlier UI directional color nor the particle's converted shade49
is substituted for the model's actual light rows.

Renderer submission seals are issued by the concrete Options collector. Its
resources stay pinned through actual renderer retirement; a failed close keeps
them retained for the existing cleanup retry path. Cold preparation never
claims a manager entry or publishes a light registry.

## Scope and execution

This packet contains engineering source and authenticated-source provenance.
No build, syntax/import check, executable test, asset decoder, gameplay run,
or visual acceptance was performed. Parent-owned integration and its sole
hourly basic smoke remain separate. Human testers own broad menu, visual,
handling, and full-race acceptance.

Gallery, memory-card flows, credits, physical feedback, and additional lifecycle
owners are not made complete by this Controls implementation.
