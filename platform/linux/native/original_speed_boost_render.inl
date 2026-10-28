// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_race_hud_render.inl.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct SpeedBoost {
    static constexpr U maximumPrimitives=game::OriginalSpeedBoostNativeDraw50LA::maximumPrimitives;
    static constexpr U maximumPackets=game::OriginalSpeedBoostNativeDraw50LA::maximumPackets;
    static_assert(maximumPrimitives<=originalUiMaximumQuads50KG,"shared UI buffers must cover SpeedBoost");
    std::shared_ptr<const game::OriginalSpeedBoost50LA> owner;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,12> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,12> policies{};
    std::array<U,12> images{};
    std::array<assets::Vertex48B,maximumPrimitives*4> vertices{};
    std::array<game::OriginalSpeedBoostNativePacket50LA,maximumPackets> packets{};
    U target=0,color=0,depth=0,primitives=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<SpeedBoost> speedBoost;
bool speedBoostPreparing=false,speedBoostUiCreationFailed=false;
bool hasSpeedBoostResources()const noexcept;
bool prepareSpeedBoostResources(std::shared_ptr<const game::OriginalSpeedBoost50LA> owner) noexcept;
bool prepareSpeedBoostFrame(const game::OriginalSpeedBoost50LA* owner) noexcept;
bool drawSpeedBoost(U bank) noexcept;
bool deleteSpeedBoostNames(VehicleWindowDiagnostic50DS& d) noexcept;
