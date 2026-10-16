# Actual wreck collision and model-visibility owners

This packet adds `OriginalWreckQueries50LA` and `OriginalWreckVisibility50LA`, plus a bounded shared-world bridge. These are actual native owners of the source piece E0/E4/E8 and model19C memberships. They do not enable nonzero player states, replace destruction handlers, register a synthetic wreck, or supply the whole racer370/374 death view.

## Source construction

`prepareCold` reserves stable native backing and stages authored type2/type0 geometry. It writes no canonical query table row. The aggregate continuation calls `publishColdQuery(0, E0component)`, then constructs DC/D8, calls `bindContactQuery()` at2955F0, publishes E4/E8 at their actual positions, and calls `bindMutualExclusions()` at166390..1663D4. After165C30 returns, `bindDerivedResponse()` performs the actual2CB5A0 replacement of E4.response165920 with2CB420. `constructed()` denotes these query operations only, not the complete piece/group/effect constructor.

E0 initially has piece9C and D0.A0.2955F0 replaces9C with DC and installs294BB0 in queryE4. E4 retains piece9C, D0.A0 and the distinct responseE8. E8 retains piece9C with source-null A0 and null queryE4. All three actual11B570 calls clear the borrowed application3E0730 byte. Their source mode3/5 controls F8 bits, not categoryA4. PHYSICSBODY comes from the actual named registry field3E0718; WATER4342A8 is appended separately by collection22C214..22C258. Ordered identity exclusions are E0:[E4,E8] and E4:[E0,E8]. Component29 and collector flags31 are checked as the same source field.

Cold E0 and E4 use the actual model14C world. The type0 previousC0/world80 histories both receive this matrix before11F680. Source-unwritten collision80 is retained for E8; it is not certified until actual bounds preparation. Copy40/50, F8 and geometry live in the shared canonical tables, not in a second wreck world. E4 publishes its actual mesh, composed00 matrix and response-presence bit. A reached unsupported response remains the world-query boundary.

## Registration and visibility

Each query owns separate fixed portal-linked backing and uses genuine50EN attach/update/remove. Initial120070 overlap radius is zero. Model visibility uses a distinct actual application4C0 namespace, its own model-object ref and stable membership backing.28E8A0 removes prior19C first, invokes the real26F4B0 cache gate and28E230 bounds, then attaches with zero initial overlap.28EA20 uses the actual cached model.radius198. The cached bound producer includes the original flat-row0 alias; it is not the separate166400 inertia bound.

A visibility membership is not the renderer19C->4->A4 stamp. Effects needing that source field remain gated until its genuine renderer owner is joined. Graph-body integration must call this actual observer at1682C4; it cannot pretend the observer is absent.

## Shared QueryBridge

Storage reserves exactly three trailing query refs per real cold piece, charging the existing one-block ceiling. `wreckPieceQueries` defaults to zero. Reserved capacity does not enlarge the initial active collector extent or fill spare identities.

`QueryBridge::bindWreckQueryWorldCold` returns borrows of the sole world definitions, state, geometry, copy40/50 table, registration workspace, query staging and normal-mode owner. The constructor requires a caller-computed mesh-stack maximum for real wreck target meshes. The piece constructor then publishes genuine rows. Before Follow preparation publishes the full extent, every reserved owner must have completed its query construction and retained exact identities/geometry/aliases. Refresh preserves the canonical tail. Accepted ordinary-collector F8 updates publish to those same rows. Close refuses to hide still-registered tail rows; actual22CE10 must remove them first.

This packet adds no Session callsite enabling wreck slots, no complete piece frame scheduler and no dynamic-contact dispatch. Ordinary vehicle candidate resolution still requires its actual source providers before registered debris can be included. Engine collections and the whole racer ordered debris view remain pending; a one-piece chariot cannot substitute for them.

## Evidence and verification

Original ELF SHA256: c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Static raw-word evidence is retained in the sibling query-owner/evidence directory; detector/physics ownership evidence is in wreck-owner-research-20261012/evidence/physicsobject-1659c0-168390.txt and components/evidence. Asset worker independently reviewed source cold histories, category versus F8 fields, visibility bounds cache gate and initial/update radii.

Source review only. No build, test, syntax/import check, lint, sanitizer, asset decoder, gameplay or PC operation was run. The native methods were not invoked. Root owns integration and permitted acceptance testing.
