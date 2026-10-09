# Native original-roster integration 50HW

Status: implemented, awaiting the root-owned scheduled build and focused run. No tests, builds, syntax checks, sanitizers, benchmarks or gameplay were run by the integration worker.

## Field and source ownership

The interactive default remains eight racers. Player zero retains the existing Anakin startup, controls, player audio, countdown, three-lap session, ranking and host course-driver behavior. Seven opponents are selected from `evidence/originalRoster50HW/raw-imports/roster.json`; names come from the verified UIACCEPT-derived visual selection. Every original archive is checked against the roster digest before collision loading.

Each opponent owns permanent, nonmoving Rig, collision, Visual and cable owners. Their original PSG hierarchy, MCF/VUT/MOT bindings, PHY parameters, component palette, topology, materials, textures, cable endpoints, flame bindings and main/secondary collision resources remain distinct. The live bank copies only registered trivial state and borrowed views, never vector metadata or ownership objects. The original owners outlive the bank and renderer.

Cold opponent order is rig installation, own handling, own secondary/display root placement, own model rebase, effect names, own collision/probe installation, own draw/material/cable/flame publication, then grounded grid placement. Grid offsets remain explicit host policy. The per-pod engine/chariot geometry is never collapsed to a whole-pod rigid display transform.

## Queries and snapshot arena

Cold scratch capacities cover the largest main and secondary ray counts and target traversal depths across the entire roster. Static terrain ordering remains unchanged. Every slot uses its own main and secondary type2 resources, its own type0 body shape, spring settings, response parameters, mass and probe authoring.

COL supplies no endpoint W words. The caller explicitly preserves the existing corpus policy: first W is `0x80000000 + index`, second W is `0x7fc10000 + index`. Both original retained player query resources must match that policy before race startup proceeds. These are host-supplied opaque words, not reconstructed original heap contents.

The player's actual pre-enable terrain constructor records are captured before its probe is enabled. A fresh, never-advanced opponent slot installs those captured records and invokes the normal native enable operation with its own authored probe. This does not reset or coerce an already-running racer. The live single-pair probe's self-pointer is rebuilt at its permanent execution-arena address.

Hot navigation, effect-name scope and graph views use each restored binding and active scene count. The standalone legacy player asset arrays remain player-only.

## Rendering and contacts

Each opponent draw owner borrows that racer's own source batch. Renderer archetypes 1 through 7 carry distinct original draw assets, material textures and cable textures; zero remains the existing player path. Dynamic vertices, cables and all authored flame UV slots are published per racer. GPU texture owners remain alive until the renderer closes and are explicitly cleaned before the context closes.

The old shared-Pod01 contact adapter is replaced by the heterogeneous 50HW world-boundary adapter. It uses the primary racer's own query/settings and the target racer's own target mesh/material, with atomic body commits and the prior ordered-pair policy. Empty-root material zero remains an explicit host policy.

## Explicit boundaries

Original AI and full original race scheduling remain unjoined. The existing host course driver and grid spacing remain. Secondary dynamic racer-to-racer contacts, collision damage attribution and opponent event audio are still unjoined; player audio remains enabled. This integration is not a claim of full original-game equivalence or a successful runtime validation.

## Recovery and validation handoff

The 50HW contact-bank adapter also checks each restored active scene count before reading its main-root node. Its preimage is retained beside the main preimage.

Main preimage: `work/roster50hw-preimage/terrain_interactive_drive50du.cpp`.

Preimage SHA-256: `d80718849ba6b98cd251c977508aa32336911f571f809f0c4f603048557a08a5`.

Root owns the next global build-and-focused-test slot at 04:15 UTC on 2026-10-09. Useful coverage is cold setup of all seven distinct originals, own animated geometry/palettes, Pod09 material order, per-racer native ticks and grounded starts, heterogeneous main contacts, one renderer commit with seven opponents, countdown/lap/place/player audio continuity, restart/cleanup, and no successful hot-path allocations. Any failed source boundary must identify the original racer and must not substitute an Anakin clone.
