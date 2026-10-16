# Isolated native Speed/Boost GL renderer candidate

speed-renderer.patch modifies only VehicleWindowGl50DS.h/.cpp and adds original_speed_boost_render.inl. Source-only, no central edits, builds, tests or syntax checks. Depends on root's pending Game runtime API exactly as assigned: OriginalSpeedBoostNativeDraw50LA::maximumPrimitives/maximumPackets; primitives with vertices/colors/depth/vertexCount; packets with first/count/material/leaf/vertexCount; primitiveCount/packetCount/serial/cameraPublication/viewport/prepared. No terrain activation.

bindOriginalSpeedBoost(shared_ptr<game::OriginalSpeedBoost50LA>, diagnostic) retains the owner strongly through successful checked GPU cleanup. Frame.originalSpeedBoost must identify that exact owner. Requested invalid frame rejects whole submission. Diagnostics expose originalSpeedBoostPrimitives/Leaves/Rejected. Existing TimeStats and lap bind failures also recognize a failed Speed shared-UI creation, preventing reuse of partial shared GL state.

Twelve exact immutable image/policy slots use the owner-provided canonical material order panel,a1..a9,italic26,13. Each policy must be admitted source-alpha, GEQUAL/write and compatible sampler by existing authentic UI parsers; every group must remain none. Twelve distinct actual images are required. No substitute state or fallback shader/material is added. Owner still validates canonical populated provenance and collector; renderer preserves prepared nondecreasing leaf order without re-sorting.

All primitives occupy four reserved vertex slots in the shared UI buffer; three-vertex primitive's fourth slot is zero-initialized and NEVER indexed. Shared index buffer is exactly0,1,2,0,2,3 per slot (vehicle_window_gl50ds.cpp3192). Triangle packet requires material9, vertexCount3 and count1; coherent path draws exactly3 indices at packet.first*6, barrier path issues one3-index draw. Quad packets require vertexCount4 and cannot masquerade as triangle9; each uses count*6 indices. All primitives must be covered once, with matching vertex counts. Culling is explicitly disabled before any Speed geometry, avoiding inherited opaque-model winding state.

Private source-alpha FBO preserves raw source alpha without altering gameplay framebuffer alpha semantics. Both coherent-fetch and texture-barrier paths reuse the actual shared UI program; composition stays source-alpha. Actual640x448 view is aspect-scaled to current native drawable with letterbox/scissor. All requested primitives must have exact bounded depth16777214 and finite XY/UV. Source application383680 is not fabricated: this is explicit native renderer scheduling with actual current GL resources, and does not claim retail global traversal.

Cleanup participates in usesTextures/hasGlResources, global close glFinish, unit unbinding, checked name retirement and frame clearing. Failed bind drains actual work before deleting Speed names; ambiguous cleanup keeps owner/resources retained. Order is host HUD, lap HUD, TimeStats, Speed, then existing gameplay blur. It does not suppress native duplicate heat/speed labels or modify source panel positions; that presentation choice remains root-owned.

## Central integration and corrected phase boundary

The root stages Speed/Boost before TimeStats, finishes both against the same
last populated collector, binds both real GPU owners after camera preparation,
and consumes synchronous actual Bank/camera observations before submission.
The selected racer's body speed/heat/boost are distinct from the source view-slot
vehicle40 gate. Source race D8/E8 comes from the existing Session's accepted-world
prefix using the same live Application pause lane. No driver/input is changed.

Do not read DisplayInputs.applicationMode22C as a drawing-phase capability:
that existing field is0 in native video-request scope. It is removed from the
Speed frame projection. This bounded native-host presentation uses the actual
active/frameSet/current-context renderer transaction and retained GL resources.
It does not fabricate source Application22C=5 or global383680 and does not claim
full retail drawing-dispatch traversal. Source arithmetic, assets and primitive
order remain the recovered leaves under explicit native scheduling.

Speed draw preparation invalidates its prior CPU packet before mutation;
accepted world ticks also invalidate it. The renderer copies bounded primitives
and only the actual completed gameplay window image produces the display log.
Root closes the renderer before releasing both HUD results and their canonical
refs. Native diagnostic TimeStats clearance remains independent; original HUD
coordinates are not moved. No software test has run for this integration yet.
