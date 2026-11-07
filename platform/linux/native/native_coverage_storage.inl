// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// Private VehicleWindowGl50DS::Storage members. Demand-gated native coverage;
// no source admission, context creation, quality fallback or GPU receipt here.
    static constexpr U coverageTexture = 0x9100; // TEXTURE_2D_MULTISAMPLE
    bool coverageDemand = false;
    // Per-frame checked bridge submissions, not visible-pixel/GPU-time counts.
    std::uint64_t coveragePairRuns=0,coveragePairTriangles=0,coveragePairCoalescedTriangles=0;
    std::uint64_t coveragePairEmptyTriangles=0,coveragePairFullSingletons=0;
    // Source MSCNT groups drawn inside an earlier group's proven-disjoint batch.
    std::uint64_t coveragePairBatchedGroups=0;
    // Profiling-only reasons a single-sample run ended: an unproven overlap
    // (by identical-vertex count 0/1/2, or missing cached operands), an
    // empty/full next region, the run cap, or the end of the group/batch.
    std::array<std::uint64_t,4> coveragePairUnproven{};
    std::uint64_t coveragePairUnprovenOperands=0,coveragePairBreakRegion=0,
        coveragePairBreakCap=0,coveragePairBreakEnd=0;
    void classifyUnprovenPair(U slot,U component,U firstA,U firstB) noexcept;
    // Paired spans that kept the exact feedback route because a vertex alpha exceeded 64.
    std::uint64_t coveragePairExactFallbackSpans=0;
    struct CoverageExtent {
        U scene[2]{}, color[2]{}, depth[2]{};
        U snapshot=0, snapshotColor=0, raster=0, payload=0, rasterDepth=0;
        I width=0,height=0;
        bool owns() const noexcept {
            return scene[0]||scene[1]||color[0]||color[1]||depth[0]||depth[1]||
                snapshot||snapshotColor||raster||payload||rasterDepth;
        }
    } coverageLive, coverageSpare;
    U coverageProgram=0,coverageVertex=0,coverageFragment=0,coverageArray=0;
    I coverageRaCompositionUniform=-1;
    I coveragePayloadUniform=-1,coverageDepthUniform=-1,coverageDestinationUniform=-1;
    bool hasCoverageResources() const noexcept;
    bool checkCoverageSupport(I major,I minor) noexcept;
    // This helper changes coverage alone, never a depth/blend tuple. Call on
    // every span, after any source-UV callback, independently of local caches.
    void bindTrackCoverage(NativeTrackCoverage50DS policy) noexcept;
    void coverageAa0() noexcept;
    bool deleteCoverageExtent(CoverageExtent& e,VehicleWindowDiagnostic50DS& d) noexcept;
    bool deleteCoverageResources(VehicleWindowDiagnostic50DS& d) noexcept;
    bool coverageTextureStorage(U& name,U format,I w,I h,bool multi,U attachment) noexcept;
    bool coverageFramebuffer(U& name) noexcept;
    bool coverageComplete() noexcept;
    bool createCoverageExtent(CoverageExtent& e) noexcept;
    bool prepareCoverageExtent() noexcept;
    bool createCoverageProgram() noexcept;
    bool drawCoveragePairTriangle(U bank,U first,U image,const std::array<I,4>& region,U sourceArray,U triangleCount,
        bool firstRun,bool firstCoverageSetup) noexcept;
    bool drawCoveragePairGroup(U bank,U slot,U component,U first,U count,U image,
        bool& firstCoverageSetup) noexcept;
    // Single-sample only. Returns how many consecutive groups starting at g
    // (at least one) form one batch: contiguous elements, at most 64
    // triangles, and every triangle pair from DIFFERENT groups rectangle-
    // disjoint or proven separable. Within a group, run rules are unchanged.
    U planSeparableGroupBatch(U slot,U component,const SpecularGroup50LA* groups,U g,U end) noexcept;
    bool resolveCoverage(U bank) noexcept;
