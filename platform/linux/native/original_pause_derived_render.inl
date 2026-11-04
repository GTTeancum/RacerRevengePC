// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_damage_render.inl.
// Genuine derived1BB010 or ordered single-view outer-tail batch. One depth
// plane for the entire batch; no complete outer Pause callback is claimed.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct PauseDerived {
    static constexpr U maximumQuads=game::OriginalPauseNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalPauseNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover PauseDerived");
    std::shared_ptr<const game::OriginalPauseNativeDraw50LA> owner;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0,cameraPublication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,game::OriginalPauseNativeDraw50LA::maximumMaterials> samplers{};
    std::array<const game::OriginalPauseNativeResources50LA::Pass*,game::OriginalPauseNativeDraw50LA::maximumMaterials> policies{};
    std::array<U,game::OriginalPauseNativeDraw50LA::maximumMaterials> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalPauseNativeDraw50LA::Packet,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<PauseDerived> pauseDerived;
// Renderer-minted identity of the retained ORIGINAL Pause pixels. Generic
// host menus use the same menu targets, so target membership is not proof.
// Nonzero completedHistory is written only after the real display completion.
struct PauseDisplayPublication {
    const game::OriginalPauseNativeDraw50LA* owner=nullptr; //Strongly held by pauseDerived.
    const game::OriginalCameraViewportFields* viewport=nullptr;
    U target=0;
    I width=0,height=0;
    std::uint64_t serial=0,scene=0,generation=0,texturePublication=0,cameraPublication=0;
    std::uint64_t completedHistory=0;
} pauseDisplayPublication;
void stagePauseDisplayPublication(U target,I w,I h)noexcept;
void completePauseDisplayPublication()noexcept;
bool pauseDisplayIdentityCurrent()const noexcept;
bool completedPauseDisplayCurrent()const noexcept;
bool pauseDerivedPreparing=false,pauseDerivedUiCreationFailed=false;
bool hasPauseDerivedResources()const noexcept;
bool preparePauseDerivedResources(std::shared_ptr<const game::OriginalPauseNativeDraw50LA> owner) noexcept;
bool preparePauseDerivedFrame(const game::OriginalPauseNativeDraw50LA* owner) noexcept;
bool drawPauseDerived(U bank) noexcept;
bool drawPauseDerivedTargets(U sourceTarget,U destinationTarget) noexcept;
bool deletePauseDerivedNames(VehicleWindowDiagnostic50DS& d) noexcept;
