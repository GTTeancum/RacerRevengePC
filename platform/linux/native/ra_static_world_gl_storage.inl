// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// VehicleWindowGl50DS::Storage. Native RA submission ownership, not a guest
// command stream. Original world topology, material profiles and byte operands
// remain owned by the actual aggregate. Source polygon clipping is native GPU
// math; raster/depth/MSAA remain GL.
    std::shared_ptr<ra_static_world50LA::Owner> raWorld;
    const VehicleWindowGl50DS* raWorldRenderer=nullptr;
    std::uint64_t raWorldImagePublication=0;
    U raWorldArray=0,raWorldVertices=0,raWorldOperands=0,raWorldIndices=0;
    U raWorldVertexShader=0,raWorldGeometryShader=0,raWorldFragment[2]{},raWorldProgram[2]{};
    struct RaWorldUniforms { I matrix=-1,sourceMatrix=-1,image=-1,fog=-1,alpha=-1,payload=-1,destination=-1; } raWorldUniform[2];
    struct RaWorldVertex { float position[3],uv[2]; };
    struct RaWorldFrameVertex { float color[4],fog; };
    static_assert(sizeof(RaWorldVertex)==20&&offsetof(RaWorldVertex,position)==0&&
        offsetof(RaWorldVertex,uv)==12,"RA immutable float stream layout");
    static_assert(sizeof(RaWorldFrameVertex)==20&&offsetof(RaWorldFrameVertex,color)==0&&
        offsetof(RaWorldFrameVertex,fog)==16,"RA current float stream layout");
    std::vector<RaWorldVertex> raWorldGeometry;
    std::vector<RaWorldFrameVertex> raWorldFrame;
    std::vector<U> raWorldFrameImages;
    using LocalFlareEmission=original_local_flare50LA::Emission;
    struct RaLocalFlareVertex {float position[3],uv[2],color[4],fog;};
    struct RaLocalFlareFrame {
        std::unique_ptr<LocalFlareEmission::Operands> operands;
        std::array<RaLocalFlareVertex,6> vertices{};
    };
    // One actual Light3 image per renderer/context, shared by all reached
    // occurrences. Never allocate textures once per portal/quad occurrence.
    MaterialTextureGl50EB raLocalFlareTexture;
    assets::MaterialTextureAssets50EA raLocalFlarePixels;
    std::shared_ptr<const std::vector<std::uint8_t>> raLocalFlareArchive;
    const std::string* raLocalFlareMember=nullptr;
    const assets::OriginalStandardTextureResource50LA* raLocalFlareResource=nullptr;
    std::uint64_t raLocalFlarePublication=0;
    U raLocalFlareArray=0,raLocalFlareBuffer=0,raLocalFlareVertexShader=0,raLocalFlareFragmentShader=0,raLocalFlareProgram=0;
    I raLocalFlareMatrix=-1,raLocalFlareImage=-1,raLocalFlareFog=-1;
    bool raLocalFlareFailed=false;
    std::vector<RaLocalFlareFrame> raLocalFlareFrames;
    bool prepareRaLocalFlareImage(const LocalFlareEmission::Operands&)noexcept;
    bool createRaLocalFlareGeometry()noexcept;
    bool finishRaLocalFlareFrame(RaLocalFlareFrame&)noexcept;
    bool drawRaLocalFlare(U,std::size_t,const std::array<I,4>&)noexcept;
    bool deleteRaLocalFlareResources(VehicleWindowDiagnostic50DS&)noexcept;
    struct RaWorldFrameDraw { std::size_t surface=0;std::array<I,4> scissor{};
        bool localFlare=false;std::size_t flare=static_cast<std::size_t>(-1),record=0; };
    std::vector<RaWorldFrameDraw> raWorldFrameDraws;
    ra_static_fog50LA::CameraObservation raWorldCamera;
    float raWorldSourceMatrix[16]{};
    bool raWorldFramePrepared=false;
    bool raPixelProvenanceLogged=false;
    bool hasRaWorldResources() const noexcept;
    void retireRaWorldPublication() noexcept;
    bool raWorldCurrent(VehicleWindowDiagnostic50DS& d) const noexcept;
    bool deleteRaWorldResources(VehicleWindowDiagnostic50DS& d) noexcept;
    bool createRaWorld() noexcept;
    bool prepareRaWorldFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d,bool& flareEntered) noexcept;
    bool raWorldFrameCurrent(VehicleWindowDiagnostic50DS& d) const noexcept;
    void bindRaWorldMaterial(const ra_static_world50LA::Owner::RasterInventory& raster,U program,bool payload,
        bool uploadUniforms) noexcept;
    bool snapshotRaWorld(U bank,const std::array<I,4>& region) noexcept;
    bool raTriangleOutsideNativeClip(U first) const noexcept;
    struct NativeRasterVertex {
        // Producers explicitly initialize every member before consumption.
        // Avoid duplicate zeroing in local arrays filled by producer/copy.
        double clip[4],error[4];
        unsigned outside;
    };
    struct CachedNativeRasterVertex {
        NativeRasterVertex operand;
        std::array<double,8> projectedQuotients{};
        U index=0;
        bool occupied=false,finite=false,projectionReady=false;
    };
    // Scratch exists only within one synchronous source MSCNT group. It
    // retains no resource pointer and is invalidated before every group.
    mutable std::array<CachedNativeRasterVertex,32> racerRasterOperands{};
    bool racerRasterCacheActive=false;
    U racerRasterSlot=0,racerRasterComponent=0,racerRasterFirst=0,racerRasterCount=0;
    void beginRacerRasterOperandGroup(U slot,U component,U first,U count)noexcept;
    void endRacerRasterOperandGroup()noexcept;
    bool nativeRasterVertex(const std::array<float,3>& v,NativeRasterVertex& out)const noexcept;
    std::array<I,4> nativeTriangleRasterOperands(const std::array<NativeRasterVertex,3>& vertices,
        const U* cacheIndices=nullptr)const noexcept;
    std::array<I,4> nativeTriangleRasterRegion(const std::array<std::array<float,3>,3>& triangle)const noexcept;
    std::array<I,4> raTriangleRasterRegion(U first)const noexcept;
    std::array<I,4> racerTriangleRasterRegion(U slot,U component,U first)const noexcept;
    // Capture-only attestation of existing GL attribute/storage, never a draw,
    // source replay, visibility test or pixel-winner claim. Six vertex reads
    // are bounded to the two explicitly requested source floor triangles.
    void traceRaFogGpuAttestation() noexcept;
    // Optional read-only source provenance for the actual capture-selected RA frame.
    // These conservative rectangle candidates are NOT GPU coverage/winners:
    // texture alpha, depth, samples, later racers and HUD are not evaluated.
    void traceRaPixelProvenance() noexcept;
    bool drawRaWorld(U bank) noexcept;
