// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// VehicleWindowGl50DS::Storage. Native raster-operand arithmetic shared by the
// racer coverage/pair bridges and the GS destination-alpha track bridge, plus
// the one course-independent pixel provenance diagnostic (RR_PIXEL_PROVENANCE).
// No course owner participates; every operand comes from the uploaded track or
// racer geometry and the current submitted matrix.
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
    // Scratch exists only within one synchronous source MSCNT group or one
    // proven-disjoint batch of consecutive groups. It retains no resource
    // pointer and is invalidated before every group/batch. Sized so a bounded
    // batch's consecutive vertex indices do not evict one another.
    mutable std::array<CachedNativeRasterVertex,256> racerRasterOperands{};
    bool racerRasterCacheActive=false;
    U racerRasterSlot=0,racerRasterComponent=0,racerRasterFirst=0,racerRasterCount=0;
    void beginRacerRasterOperandGroup(U slot,U component,U first,U count)noexcept;
    void endRacerRasterOperandGroup()noexcept;
    bool nativeRasterVertex(const std::array<float,3>& v,NativeRasterVertex& out)const noexcept;
    std::array<I,4> nativeTriangleRasterOperands(const std::array<NativeRasterVertex,3>& vertices,
        const U* cacheIndices=nullptr)const noexcept;
    std::array<I,4> nativeTriangleRasterRegion(const std::array<std::array<float,3>,3>& triangle)const noexcept;
    std::array<I,4> racerTriangleRasterRegion(U slot,U component,U first)const noexcept;
    // Single-sample only: the two cached, fully unclipped triangles at these
    // element offsets provably cover no common sample (shared edge, or a
    // robust separating line). False whenever proof is unavailable.
    bool racerTrianglesSeparable(U slot,U component,U firstA,U firstB)const noexcept;
    // Optional read-only source provenance for the actual capture-selected
    // gameplay frame, any course (RR_PIXEL_PROVENANCE=1, default off). These
    // conservative rectangle candidates are NOT GPU coverage/winners: texture
    // alpha, depth, samples, later racers and HUD are not evaluated.
    // The static world's mesh, still readable after create because the one
    // track load's static world (its owner) is leased (trackRoleLeases).
    // Null without a common track lease: callers then get full-target regions.
    const assets::Mesh48B* retainedTrackMesh=nullptr;
    bool pixelProvenanceLogged=false;
    void tracePixelProvenance() noexcept;
