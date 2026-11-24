// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_position_stats_render.inl.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct Damage {
    static constexpr U maximumQuads=game::OriginalDamageNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalDamageNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover Damage");
    std::shared_ptr<const game::OriginalDamage50LA> owner;
    HudViewports viewports;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0,cameraPublication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,1> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,1> policies{};
    std::array<U,1> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalDamageNativePacket50LA,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<Damage> damage;
bool damagePreparing=false,damageUiCreationFailed=false;
bool hasDamageResources()const noexcept;
bool prepareDamageResources(std::shared_ptr<const game::OriginalDamage50LA> owner) noexcept;
bool prepareDamageFrame(const game::OriginalDamage50LA* owner) noexcept;
bool drawDamage(U bank) noexcept;
bool deleteDamageNames(VehicleWindowDiagnostic50DS& d) noexcept;
