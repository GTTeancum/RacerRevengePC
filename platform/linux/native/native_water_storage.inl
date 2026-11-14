// Private member declarations; definitions are in vehicle_window_gl50ds_draw.cpp.
// Included only inside VehicleWindowGl50DS::Storage. Original water surface:
// one dynamic Vertex48B buffer, strips drawn in original order.
    U waterVertexBuffer=0,waterIndexBuffer=0,waterVertexArray=0,waterEnvironmentTexture=0,waterFrothTexture=0,waterMaximumStrips=0;
    const assets::Vertex48B* frameWaterVertices=nullptr;
    U frameWaterStrips=0,frameWaterAlphaReference=0;
    bool frameWaterDepthWrite=false;
    std::uint64_t waterDraws=0;

    bool hasWaterResources() const noexcept;
    bool createWater(const VehicleWindowWater50DS& input) noexcept;
    bool drawWater() noexcept;
    bool deleteWaterNames(VehicleWindowDiagnostic50DS& d) noexcept;
