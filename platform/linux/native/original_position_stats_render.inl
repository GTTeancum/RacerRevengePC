// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_time_stats_render.inl.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct PositionStats {
    static constexpr U maximumQuads=game::OriginalPositionStatsNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalPositionStatsNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover PositionStats");
    std::shared_ptr<const game::OriginalPositionStatsNativeOwner50LA> owner;
    HudViewports viewports;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,2> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,3> policies{};
    assets::OriginalUiNativeUntexturedMaterial50KG radar{};
    std::array<U,3> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalPositionStatsNativePacket50LA,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<PositionStats> positionStats;
bool positionStatsPreparing=false,positionStatsUiCreationFailed=false;
bool hasPositionStatsResources()const noexcept;
static bool positionStatsSampler(const assets::OriginalUiNativeMaterial50KG& p,
    MaterialTextureSampler50EB& sampler)noexcept;
bool preparePositionStatsResources(std::shared_ptr<const game::OriginalPositionStatsNativeOwner50LA> owner) noexcept;
bool preparePositionStatsFrame(const game::OriginalPositionStatsNativeOwner50LA* owner) noexcept;
bool drawPositionStats(U bank) noexcept;
bool deletePositionStatsNames(VehicleWindowDiagnostic50DS& d) noexcept;
