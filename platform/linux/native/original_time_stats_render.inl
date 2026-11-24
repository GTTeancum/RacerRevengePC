// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_race_hud_render.inl.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct TimeStats {
    static constexpr U maximumQuads=game::OriginalTimeStatsNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalTimeStatsNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover TimeStats");
    std::shared_ptr<const game::OriginalTimeStats50LA> owner;
    HudViewports viewports;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,2> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,2> policies{};
    std::array<U,2> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalTimeStatsNativePacket50LA,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<TimeStats> timeStats;
bool timeStatsPreparing=false,timeStatsUiCreationFailed=false;
bool hasTimeStatsResources()const noexcept;
bool prepareTimeStatsResources(std::shared_ptr<const game::OriginalTimeStats50LA> owner) noexcept;
bool prepareTimeStatsFrame(const game::OriginalTimeStats50LA* owner) noexcept;
bool drawTimeStats(U bank) noexcept;
bool deleteTimeStatsNames(VehicleWindowDiagnostic50DS& d) noexcept;
