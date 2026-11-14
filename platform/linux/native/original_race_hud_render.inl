// Private member declarations; definitions are in vehicle_window_gl50ds_hud.cpp.
// Included inside VehicleWindowGl50DS::Storage. Native bounded gameplay layer;
// no guest packets, guest execution, host font or caller-created readiness.
struct RaceHud {
    // Eleven TextOverlays x 40 glyphs + two gleams x two quads, one packet
    // per text and per gleam material.
    static constexpr U maximumQuads=448,maximumPackets=20;
    struct Packet {U first=0,count=0,material=0;};
    const game::OriginalRaceHud50KG* owner=nullptr;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,publication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,5> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,9> policies{};
    std::array<U,9> ranks{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<Packet,maximumPackets> packets{};
    game::OriginalUiTextLayout50KG glyphs{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<RaceHud> raceHud;
bool raceHudPreparing=false,raceHudUiCreationFailed=false;
bool hasRaceHudResources() const noexcept;
static float hudMul(float a,float b) noexcept;
static float hudAdd(float a,float b) noexcept;
static float hudSub(float a,float b) noexcept;
static float hudDiv(float a,float b) noexcept;
static bool hudName(const std::uint8_t* p,std::size_t n,const char* expected) noexcept;
static bool hudSampler(const assets::OriginalUiNativeMaterial50KG& p,MaterialTextureSampler50EB& s) noexcept;
bool prepareRaceHudResources(const game::OriginalRaceHud50KG& owner) noexcept;
bool prepareRaceHudFrame(const game::OriginalRaceHud50KG* owner) noexcept;
bool drawRaceHud(U bank) noexcept;
bool deleteRaceHudNames(VehicleWindowDiagnostic50DS& d) noexcept;
