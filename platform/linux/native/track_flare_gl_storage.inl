// Private member declarations; definitions are in vehicle_window_gl50ds_track_flare.inl
// (included by vehicle_window_gl50ds_world.cpp). Included only inside VehicleWindowGl50DS::Storage.
// The ONE track light-flare pass (W0 round 2, area F): every LightN row whose SCB data authors
// HasLightFlare (the lighting owner's trackFlares(), roles.lights, filled at 05.13) is drawn on
// the one track path of every course. Replaces the RA-only local Light3 flare draw that went with
// the deleted RA byte-material world (b607b29 vehicle_window_gl50ds_local_flare.inl). The flare's
// image comes from its own LightFlareTextureMapName material in the course archive; no course,
// zone or light number is named here.
//
// Allocation guards (PORT STAND-IN: guards against a corrupt or hostile file, logged on create,
// not sizes): at most trackFlareLimit rows (= the lights block ceiling of common_track50cr.h),
// at most trackFlareImageLimit distinct images, each at most 1024x1024 (the PST decoder's own
// ceiling) and trackFlareImageBytes RGBA bytes in total.
static constexpr std::size_t trackFlareLimit=1024u;
static constexpr std::uint32_t trackFlareImageLimit=64u;
static constexpr std::size_t trackFlareImageBytes=16u*1024u*1024u;
static constexpr U trackFlareUnresolved=0xffffffffu;
MaterialTextureGl50EB trackFlareTextures;              // one bank: one image per distinct flare map
assets::MaterialTextureAssets50EA trackFlarePixels;    // its CPU bank (publication receipt)
std::uint64_t trackFlarePublication=0;
std::vector<U> trackFlareImage;                         // per cold row: bank index, or unresolved
std::vector<std::uint8_t> trackFlareDepthWrite;          // per bank image: its PSM pass-1 zWriteDisabled==0
std::size_t trackFlareCount=0;                          // cold trackFlares().size()
U trackFlareVertexBuffer=0,trackFlareIndexBuffer=0,trackFlareVertexArray=0;
std::vector<assets::Vertex48B> trackFlareVertices;      // 4 per cold row, rebuilt each frame
std::vector<U> trackFlareFrameRows;                     // rows emitted this frame, in source order
std::uint64_t trackFlareFrames=0;
bool trackFlareRefusalLogged=false;
bool hasTrackFlareResources() const noexcept;
bool createTrackFlares() noexcept;
bool drawTrackFlares() noexcept;
bool deleteTrackFlareNames(VehicleWindowDiagnostic50DS& d) noexcept;
