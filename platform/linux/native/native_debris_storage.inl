// Private member declarations; definitions are in vehicle_window_gl50ds_draw.cpp.
// Included only inside VehicleWindowGl50DS::Storage. PC-native debris pass:
// cold piece-local geometry, per-frame piece-to-world matrices.
    U debrisVertexBuffer=0,debrisIndexBuffer=0,debrisVertexArray=0;
    I debrisIndexCount=0;
    std::vector<VehicleWindowDebrisSpan50DS> debrisSpans;
    std::vector<VehicleWindowDebrisPiece50DS> debrisPieces;
    std::array<VehicleWindowDebrisInstance50DS,maxVehicleWindowDebrisInstances50DS> frameDebris{};
    U frameDebrisCount=0;
    std::uint64_t debrisDraws=0;

    bool hasDebrisResources() const noexcept;
    bool createDebris(const VehicleWindowDebris50DS& input) noexcept;
    bool drawDebris() noexcept;
    bool deleteDebrisNames(VehicleWindowDiagnostic50DS& d) noexcept;

// PC-native wreck particle sprites: fixed dynamic quad buffer, static indices.
    U effectVertexBuffer=0,effectIndexBuffer=0,effectVertexArray=0,effectTexture=0;
    std::array<assets::Vertex48B,maxVehicleWindowEffectQuads50DS*4u> frameEffectVertices{};
    U frameEffectQuads=0,frameEffectBlended=0;
    std::uint64_t effectDraws=0;

    bool hasEffectResources() const noexcept;
    bool createEffects(const VehicleWindowEffects50DS& input) noexcept;
    bool drawEffects() noexcept;
    bool deleteEffectNames(VehicleWindowDiagnostic50DS& d) noexcept;
