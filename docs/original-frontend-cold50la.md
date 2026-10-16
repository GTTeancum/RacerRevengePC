# Dedicated original frontend cold lifetime (50LA)

This is a native ownership split, not a reconstruction of retail boot. `RR_ORIGINAL_MAIN_MENU=1` starts the existing authentic Main/Options/Rider/Course frontend in a dedicated host/context lifetime before the race loader. The existing corpus argument and optional `--headless-live CAPTURE_PPM` form remain compatible; the corpus is neither opened nor required for Main, Options, or Rider.

## Actual resources and rendering

The existing `Application` retains the process profile, controls, shared clock/random selectors, sound mode, normal-material setting and LightManager. `Frontend::prepare` still authenticates its original UI archives, text/material/texture owners, optional original Rider image cache and selected real audio owner. Missing required UI input still fails. Explicit muted audio stays a separate admission; no generated UI resource or substitute glyph/image is introduced.

`VehicleWindowGl50DS::createOriginalFrontend` takes a narrow request containing only the actual context, output bounds, original UI CPU/GPU bank and source-RGBA capability. It creates the existing original UI shader, vertex/index storage and private UI/source targets. It creates no vehicle/world geometry, gameplay shader/adapter, host HUD, gameplay framebuffer, particle atlas, or blur history. Gameplay draw, host-menu draw, fog update, visibility receipt, display completion, blur clear and gameplay fade entry points explicitly reject this capability. UI bank switches, sealed source-frame/byte-blend publication, completion and readback retain their existing contracts. Main uses its existing native original-art presentation; Options/Rider keep their existing source-RGBA admission and GPU capability requirements.

The window remains genuine native OpenGL and SDL ownership. Input/update/render/swap, focus/pause handling, actual UI audio advancement, capture publication and source/private image receipts retain the existing frontend behavior. `original_main_ready` is printed only after a real accepted UI publication and the real window swap when applicable.

## Course boundary

The cold race target catalog is prepared once, only after the original Course screen is active and before its next input. Its actual wall interval is excluded from both existing UI cold-time accumulators, so file authentication does not become source animation/audio time. Catalog preparation is outside the noexcept input-admission predicate.

An absent `evidence/originalRoster50HW/raw-imports/roster.json` produces an explicit unavailable-race diagnostic and no authenticated destinations. A present roster must parse as one complete JSON document and pass exact core/supplemental structure and identity checks; malformed metadata fails. Optional selected pod assets and actual target archive authentication still determine individual destination admission. Course browsing and Back can remain available with genuine UI owners even if race Confirm is unavailable. An absent corpus or `RR_RACE=0` explicitly disables race entry. No catalog receipt means that a world, rig, camera or gameplay renderer has been constructed.

Every accepted Course launch publishes the existing typed `ColdLoadRequest`, including an unchanged selection. The original Course exit and GPU drain precede publication. UI source ownership, renderer, texture banks, audio and context are closed and destroyed before the outer application consumes the request and invokes unchanged race cold construction. The real selected rider/track and profile are rechecked by that request. Unavailable or malformed race assets never become fabricated race readiness.

## Scope and validation

No CPP source-list changes: the new session helper is a header included by the existing 50DU translation unit; the renderer implementation remains in its existing CPP. Existing direct-race behavior and `RR_START_MENU` defaults are preserved. The former embedded UI branch is retained but the outer application now routes UI entry through the dedicated lifetime.

This packet was reviewed as source only. No compiler, syntax/import checker, build, tests, sanitizers, gameplay, project decoder, Windows operation or GL execution was run. First runtime validation belongs to the root-owned Windows slot. Whole-game or retail-render parity is not claimed.
