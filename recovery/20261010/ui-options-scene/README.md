# Options menu scene, frame and external-lifecycle session

Source-only reconstruction. No build, compiler, syntax check, project parser/decoder, software test, sanitizer or gameplay run was performed for this package. The only executed original-data research was independently bounded Python-stdlib extraction/decompression/hash reads; source instructions were inspected as static data. The active repository and the two earlier frozen Options packages were not edited.

## Deliverable and prerequisites

Candidates mirror repository paths under `src/` and `platform/`; exact existing-file preimages are under `preimages/`. New source files needing build registration:

- `src/game/original_ui_options_particle_render_state50kg.cpp`
- `platform/linux/native/original_ui_options_session50kg.cpp`

The particle adapter is header-only. Existing scene/fire/lighting/camera/material, Options widget/resources/contribution sources keep their existing build registrations. Shared floating-point source files retain the project's strict binary32/no-contraction requirements.

Prerequisite: the separate exit-transition package supplies `OriginalUiExitText50KG`, `OriginalUiScreenAssets50KG::sourceProvenance`, and generic `bindInitialPage`. Options resources cold-prepare the genuine +440 ScreenText using actual English entry89 and font image0; the collector submits it at the real closing endpoint before both aperture rectangles. Audio event5 was separately staged in `audio/` and already integrated by root as f087350; do not reapply that subtree.

## Actual scene evidence and correction to the prior package

Independent bounded extraction authenticated the original UIOPTION archive (200341 bytes, SHA256 fde295f01154aa9fa6fb7ff5faccdb0a3ac8380214241ba3d13490ebd0d8ade2), including v3 index/block envelopes, then materialized:

| Actual Options member | Size | SHA256 |
|---|---:|---|
| ui/UIOption/opt.scn |3442|147235426b8ac34cf4796dd2e4069efcc2401ef2533e6617791c15407275baa4|
| ui/UIOption/opt.toy |433|760c7458c3879f74a5f3d5e9d697f58bce1db906781ac2d2ff60f5267ddbe96b|

These two byte hashes exactly match Main mm.scn/mm.toy. The four actual Options particle members also exactly match Main mmprt.prt/mmfire.emt/mmprt.psm/mmprt.pst. This corrects the earlier README's outstanding-extraction statement. Different paths/catalog identities do not mean different payload bytes. All six Options identities remain distinct and are used as Options, never relabeled Main.

Actual opt.scn says SceneName=MainMenu, StaticModelCount=0, fog RGB70/10/10 visibility1 haziness0, ambient50/50/50 and directional0/0/0. Actual opt.toy has zero cameras, zero animations, one Type8 emitter at48.055893/11.260490/71.276047, source mmfire.emt, instance1, StartOn/Additive true. Source manager1D06A8/1D06B4 creates/loads this scene, and1D06D0 calls1D0DA0 to attach it.

The historical Main-named numeric scene/fire/particle types now expose explicit Options entry points with genuine Options catalog checks. Main wrappers still require Main. No dummy archive, substituted identity, or guest program is used. The shared source-screen numeric continuation is exported as `prepareOriginalUiParticleCameraFromView50KG(CameraViewOutput49F,float,float,...)`; Main and Options authenticate their own camera before reaching it, and Rider may use this same numeric entry after authenticating its own page camera.

## Concrete native scene and packet sequence

Options camera is the actual manager CameraInfo from Options ui.dat; page1F9530 borrows it. Scene fog overrides that same camera's clipping planes to near1/far4000 at its later scene render boundary, retaining the authored normalized pose/FOV/aspect. Fog is3936..4000; packed FOGCOL is R70,G10,B10; original ambient conversion yields RGB49 on every particle corner, with raw source W bits0x40. No extra camera entity is invented.

The session update order is: dedicated audio preparation; coordinator's one actual manager tick; ordinary widget siblings; Options page update1F9A74 saves aperture delta; OnTop siblings; actual particle advance; borrowed stationary camera; actual Type8 fire emission using the process-owned RNG308030 and serial433970. The cold camera pose has no source animation/debug override in this admitted graph.

Render order is: ordinary widgets; authentic page1F9A80 background and dynamic selection arrows; actual +440 exit glyph submission when source endpoint calls2E0170; aperture +424 then+428; mask42C prompts; OnTop widgets; actual scene/fog/particle tail. The particle tail validates signed runs of at most12 and expands native billboard geometry with real lighting/fog. An authenticated zero-particle batch is complete work; omitting the scene call cannot finish the frame.

Final drain preserves the actual APPSTART Mat_Stbl order, FIFO per leaf: particle/mmprt token2902 first, then the ten existing UI leaf tokens2956..3217. Source submission order is preserved separately. Original integer packet Z, source raw vertex channels, real texture alpha, two particle states and sampler/depth policies survive conversion. This closed graph has no source framebuffer-alpha consumer; only then is the native opaque presentation-alpha policy exposed. No popup/other-page graph receives this certification.

All widgets begin recursively hidden at original manager2A44E0. The closed page bank contains every actual menu control membership plus directly touched widget3, and its callbacks reach no other page's widgets. Only menu1 is admitted. Other manager widgets remain inactive in this closed projection; this is not a claim that every357-widget page controller is reconstructed.

## CPU/GPU identity and lifetime contract

1. Load a fresh `OriginalUiOptionsDrawResources50KG` from actual AppStart, Options, Fonts, ControllerPopup and Overlays archives, original English and real640x448 video owner. Cold preparation includes actual scene, fire, lighting, render state, UI materials, particle material and exit text.
2. Publish exactly its `textures()` with its `samplers()` into an Options GPU bank. Nine source image entries expand to the actual two ATM frame collections; use `textureCount()`, never a guessed fixed count. Fonts and shared overlays retain their authentic source archive identities. Background/arrows/particle retain Options identities.
3. `bindFrame(resources,gpu)` checks every image identity, count, initialized state and nonzero publication token. It pins the actual bank and checks publication again on begin/drain. The new frame's native draw contains this Options bank and token.
4. Root uses renderer.bindOriginalUiTextures with this actual CPU/GPU pair at a safe idle/current-context boundary and activates the corresponding page session as one coordinator transaction. Keep the previous bank through completed old GPU work. Do not overwrite Main images or pass Options packets against Main publication.
5. Widget/fire/exit-text state inside Options resources belongs to one actual Options manager lifetime. A new manager needs fresh mutable resources/session; immutable authenticated GPU image publication may be retained/reused only after identity/sampler compatibility is re-established. Never resume a terminated manager from a copied snapshot.

## External lifecycle / route integration API

`OriginalUiOptionsSessionCold50KG` pins resources, GPU, Options archive, genuine +6E4 profile subset, shared common mode34 profile, process application/audio and an unbound coordinator-owned `externalTransition`. Cold bind does not execute initial lifecycle callbacks, start music, write profiles or advance audio.

Each actual screen manager has one authoritative owner. Root binds that destination owner's `bindInitialPage(12,1,callbacks)` only at actual deferred screen activation. The old source manager remains terminal after queue consumption and is retired; no immortal shared manager is restarted.

Public page boundaries align with Main external mode:

- `notifyTransition(event,diagnostic)` validates actual owner's route/phase/lifecycle, then runs real widget/entry/aperture/input-container effects.
- `pollCompletion(route,phase)` reads the actual aperture and requires current owner route/phase.
- `externalLifecycleOwner()` identifies the exact borrowed owner.
- `prepareUpdateAudio(update,diagnostic)` must run once before root manager.tick; `update` consumes identical arguments and traverses children without another tick/audio update. Duplicate preparation or mismatched/absent consumption faults. Rendering refuses a pending preparation.
- `render(clock,diagnostic)` derives lifecycle from that same actual owner and exposes the fully drained native draw only on success.

Input applies genuine directional sound before the selected Movement_Control5 callback through the new bounded `inputWithPageEffects` bank boundary. It never synthesizes a menu or changes a destination. Every Options subpage and HOF remains unavailable.

Original Back produces `OriginalUiOptionsRouteProposal50KG` (source pointer, one-use serial, exact unadmitted decision) plus actual source lifecycle. Root must:

1. Prepare/retain the complete genuine Main destination and publication.
2. Require `routeProposalCurrent(originalProposal)` immediately before admission. A subsequent input/update/render/notification invalidates it.
3. Promote a COPY of the exact route decision only after real destination admission, then call source owner's `requestRouteDecision`. In root's commitRouteSelection callback, atomically validate and write the sole original +6E4 proposal, preserving the rest of the profile.
4. Immediately call `notifyRouteAccepted(originalProposal,diagnostic)` before ticking. It verifies the actual source request lifecycle6,target4/page1 and already-committed +6E4, then emits BACK5 exactly once. It writes neither profile nor route.
5. Keep updating/rendering source Options until the actual aperture exit/closed predicate completes and the manager queues original application flags2,target4/page1. Root switches bank/session only when consuming that deferred request.

Correct source Back order:1F9994 stores +6E4;1F999C writes lifecycle6;1F99A4/1F99B0 set screen4/page1;1F99D0 plays event5;1F99E8 flushes. The accepted transportE00=-1 is the source no-work flush return, not a fabricated success for an active transport request. The draft audio-before-profile ordering was corrected before freezing.

Original profile reset is directly evidenced by186660(li v0,1),186668(sw v0,+6E4). `resetOriginalUiOptionsProfileFields50KG` sets only that field and does not mark an unrecognized profile known or write a save file. The host may invoke it only in the corresponding actual reset lifecycle.

## Honest remaining boundaries

This package is unbuilt and unobserved. Root host/renderer/route activation and shared exit-package integration remain necessary. Full Options page2 Controller1, page3 Controller2, page4 GameOptions, page5 MemoryCard, page6 Credits, HOF11 and the second-controller popup are unavailable. Their prospective route/profile/audio effects are not partially committed. The page's real labels/selection still exist. No fake settings persistence, fake popup rectangle or replacement artwork is supplied. Deliberate mute remains explicit; audio-enabled use needs the authentic event5 transport and real output owner.

The earlier frozen widget README is preserved. `documentation/options-widget-README.updated.md` is an explicit replacement candidate with its exact original preimage under `preimages/documentation/`; it updates only the stale scene-extraction statement and points here for this later closure.
