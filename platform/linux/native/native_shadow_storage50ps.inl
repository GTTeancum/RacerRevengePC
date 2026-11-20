// Private member declarations; definitions are in vehicle_window_gl50ds_shadow50ps.cpp.
// Included only inside VehicleWindowGl50DS::Storage. Original projected pod
// shadows (src/game/original_projected_shadow50ps.h): one 256x256 caster
// target per racer, one receiver pass over the listed static instances of
// the one track world (its spans' worldInstance, every course). Replaces the
// retired native footprint shadow.
    struct ShadowTarget50PS { U framebuffer=0,color=0; };
    std::array<ShadowTarget50PS,game::maxProjectedShadows50PS> shadowTargets{};
    U shadowCasterProgram=0,shadowCasterVertexShader=0,shadowCasterFragmentShader=0;
    I shadowCasterClipUniform=-1,shadowCasterSkinUniform=-1,shadowCasterBonesUniform=-1;
    U shadowReceiverProgram=0,shadowReceiverVertexShader=0,shadowReceiverGeometryShader=0,shadowReceiverFragmentShader=0;
    I shadowReceiverMatrixUniform=-1,shadowReceiverTextureUniform=-1,shadowReceiverImageUniform=-1;
    I shadowReceiverLightUniform=-1,shadowReceiverFacingUniform=-1,shadowReceiverAlphaUniform=-1;
    U shadowCableVertexArray=0,shadowCableVertexBuffer=0;
    // Cold per-instance receiver table (world space) and the world's portals,
    // built once at create.
    std::vector<game::ShadowReceiverStatic50PS> shadowReceiverStatics;
    std::vector<game::ShadowPortal50PS> shadowPortals;
    U shadowZoneCount=0;
    // Caster geometry at the original caster LOD: min(434360/434368 = 2,
    // LOD count - 1) (1B32E0 -> 2316D0), per racer archetype and component.
    // Node-local positions + node index, posed with the part's own palette.
    struct ShadowCasterLod50PS {
        U vertexArray=0,vertexBuffer=0,indexBuffer=0;
        I indexCount=0;
        U lod=0,lodCount=0,nodeCount=0,vertexCount=0;
        bool ready=false;
        std::vector<float> local;          // xyz per vertex
        std::vector<std::uint16_t> node;   // palette entry per vertex
        std::vector<std::uint32_t> indices;
    };
    std::vector<std::array<ShadowCasterLod50PS,maxComponents>> shadowCasterLods;
    // CPU-vertex parts: palette recovered per frame from the part's own
    // displayed (LOD 0) vertices, then the caster LOD posed on the CPU.
    U shadowCpuVertexArray=0,shadowCpuVertexBuffer=0;
    std::vector<float> shadowCpuVertices;
    std::array<std::array<double,16>,game::maxRigidPalette49S> shadowSolveNormal{};
    std::array<std::array<double,12>,game::maxRigidPalette49S> shadowSolveRight{};
    std::array<std::array<float,12>,game::maxRigidPalette49S> shadowSolvedPalette{};
    std::array<std::uint32_t,game::maxRigidPalette49S> shadowSolveCount{};
    std::uint64_t shadowLodGpuParts=0,shadowLodCpuParts=0,shadowLodFallbackParts=0;
    bool shadowResourcesCreated=false;
    // Per-frame copy of the game-side result plus the renderer's receiver gather.
    game::OriginalProjectedShadowFrame50PS frameShadows{};
    std::array<game::ShadowReceiverList50PS,game::maxProjectedShadows50PS> frameShadowReceivers{};
    // Sorted (instance << 1 | facingRule) keys for the draw-time lookup.
    std::array<std::array<std::uint64_t,game::maxShadowReceivers50PS>,game::maxProjectedShadows50PS> frameShadowKeys{};
    std::array<U,game::maxProjectedShadows50PS> frameShadowCasterParts{},frameShadowReceiverDraws{};
    std::array<bool,game::maxProjectedShadows50PS> frameShadowDrawn{},frameShadowHidden{};
    bool frameShadowsSet=false;
    // Pod translucency split for the original material order (commit()):
    // 0 all, 1 chariot (pod_wndw glass) only, 2 engines (flames) only.
    std::uint8_t podEffectPass50PS=0;
    U frameShadowCount=0;
    // Verification (RR_SHADOW_TRACE=1): every 60th frame logs counts, covered
    // texels read back from each target and occlusion-query samples of each
    // receiver pass; the receiver draw state is logged once per run.
    bool shadowTrace=false,shadowStateLogged=false,shadowTraceFrame=false;
    std::uint64_t shadowFrames=0;
    std::array<U,game::maxProjectedShadows50PS> shadowQueries{};
    std::array<std::int64_t,game::maxProjectedShadows50PS> frameShadowCover{},frameShadowSamples{};
    std::vector<std::uint8_t> shadowReadback;

    bool hasShadowResources() const noexcept;
    bool createOriginalShadows() noexcept;
    bool buildShadowReceiverStatics() noexcept;
    bool buildShadowCasterLods() noexcept;
    bool solveShadowPalette(const game::VehicleDrawSubmission50CM& part, const game::VehicleDrawMesh50CM& mesh,
        U nodeCount) noexcept;
    bool drawShadowCasterLod(std::uint8_t bank, const RacerView& selected, U archetype, U component,
        const game::VehicleDrawSubmission50CM& part, U lod) noexcept;
    bool prepareOriginalShadowFrame(const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d) noexcept;
    int shadowReceiverRule(U shadow, U instance) const noexcept; // -1 not listed, else facing rule
    bool drawShadowCasters(std::uint8_t bank, U slot) noexcept;
    bool drawShadowReceivers(std::uint8_t bank, U slot) noexcept;
    void finishShadowReceivers() noexcept;
    void logOriginalShadowFrame() noexcept;
    bool deleteShadowNames(VehicleWindowDiagnostic50DS& d) noexcept;
