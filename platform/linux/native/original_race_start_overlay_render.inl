// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage after original_race_hud_render.inl.
// Native bounded layer for the race's PodRaceStartOverlay (196F20): eight
// authentic OVERLAYS 321_* textures, ScreenMT fallback materials, up to four
// sprites per frame from the owner's 197C10 draw. No guest packets.
struct RaceStartOverlay {
    static constexpr U imageCount=game::OriginalRaceStartOverlay50HU::textureCount;
    static constexpr U maximumQuads=game::OriginalRaceStartOverlayDraw50HU::maximumSprites;
    struct Packet {U first=0,count=0,image=0;};
    std::shared_ptr<const game::OriginalRaceStartOverlay50HU> owner;
    std::uint64_t publication=0,serial=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,imageCount> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,imageCount> policies{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<Packet,maximumQuads> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<RaceStartOverlay> raceStartOverlay;
bool raceStartOverlayPreparing=false,raceStartOverlayUiCreationFailed=false;
bool hasRaceStartOverlayResources()const noexcept;
bool prepareRaceStartOverlayResources(std::shared_ptr<const game::OriginalRaceStartOverlay50HU> owner) noexcept;
bool prepareRaceStartOverlayFrame(const game::OriginalRaceStartOverlay50HU* owner) noexcept;
bool drawRaceStartOverlay(U bank) noexcept;
bool deleteRaceStartOverlayNames(VehicleWindowDiagnostic50DS& d) noexcept;
