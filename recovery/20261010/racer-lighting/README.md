# Racer lighting numerical reconstruction

Frozen numerical increment, source-only, 2026-10-10.

Four new candidate paths are under `files/`:

- `src/game/original_racer_light_selection50km.h`
- `src/game/original_racer_material50km.h`
- `tests/native/original_racer_lighting50km_check.cpp`
- `docs/original-racer-lighting50km.md`

They recover the bounded original directional/ambient small-registry leaf,
large-registry negative-light color folding, compressed diffuse RGB and dual
specular-map coordinates. The detailed source equations, exact program ranges,
and remaining live-owner boundaries are in the staged document.

No active source files were changed. These paths are new, so their replacement
preimages are absent. `preimages/` contains exact unchanged inspection baselines,
not files to overwrite. No shared renderer replacement is included.

No build, test, syntax check, sanitizer, project parser, game or guest instruction
execution occurred. The check source is authored and unrun. Static instruction
annotation reads original executable bytes only. Independent stdlib operations
are limited to bounded byte extraction, hashing, metadata and file staging.

The parent owns central integration and the one centralized validation slot.
The pure numerical check source can be compiled independently when authorized;
none of these leaves is automatically activated in a renderer by integration.

A separate Rider preview renderer extension is being coordinated after this
numerical freeze. It must use actual preview geometry, camera, normal/light
owners and original Diffuse versus DiffuseCompressed material identities.
