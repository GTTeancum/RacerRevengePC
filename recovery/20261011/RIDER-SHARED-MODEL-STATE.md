# Single application RNG and model shared state

Staged source-only against cd9a1d9, exact original_ui_main_session50kg.h preimage included. No active edit or execution check.

The already process-owned OriginalUiMainApplication50KG now embeds the genuine shared CoupledNoiseContext49M required by Rider character idle playback. Its existing rawRandom308030 name is a reference to that context's SAME raw state word. Existing Main/Options/startup callers therefore use the same address and arithmetic without a bridge copy or second RNG. Startup's existing host timestamp seed writes through this alias. No new seed or reset is added.

Copy/move are deleted because an application state is a unique shared object and copying a reference alias could bind the wrong owner. Existing source usage consists of shared_ptr construction/retention and field reads/writes; no source value-copy or aggregate-list construction was found by manual text inspection. Runtime/build compatibility remains untested.

The shared motion interpolation/entry sentinels, mode0 IK lazy records, and libm state are retained here once per application. Future Rider session passes these exact fields to DynamicOwner::bind. Per-character captures/graphs/chains remain in their existing native model owners. Gameplay remains unavailable and cannot independently run another RNG timeline.

This does not enable Rider or change Main/Options route behavior. Hold integration until after root's focused Options observation. Root alone performs any build/test.

Root integrated this source after the 03:17 build failure. It remains unbuilt and untested; Rider destination activation is not included.
