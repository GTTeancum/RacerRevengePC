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
    // Paired map-2 spans drawn by the fixed-function blend under coverage.
    std::uint64_t coveragePairFixedSpans=0;
    // Under coverage, admitted paired spans take the fixed-function map-2
    // blend with MULTISAMPLE off (as the bridge payload is AA0). RR_COVERAGE_PAIR_BRIDGE=1
    // keeps every paired span on the per-sample bridge (before/after A/B).
    bool coverageFixedSpecular50LA=true;
    // Test-only A/B pixel proof (RR_TEST_PAIR_ROUTE_COMPARE=1): one admitted
    // paired span every few frames is drawn by the per-sample bridge and again
    // by the fixed-function route from the same restored four-sample colour
    // and depth. The two results are compared per sample on the GPU, the
    // one-byte-per-pixel verdict is read into host memory and only counts are
    // logged. No image is written anywhere; the live frame keeps the fixed result.
    struct PairRouteCompare {
        bool enabled=false,failed=false;
        U colorPre=0,depthPre=0,colorBridge=0,colorFixed=0;
        U resultTexture=0,resultFramebuffer=0,program=0,vertex=0,fragment=0;
        I width=0,height=0;
        std::vector<std::uint8_t> host; // exactly width*height*4, owned here
        std::uint64_t frame=0,eligibleThisFrame=0,eligibleLastFrame=0;
        std::uint64_t compared=0,mismatchedPixels=0,touchedPixels=0,mismatchedSamples=0,maxDelta=0;
    } pairCompare;
    bool pairCompareEligible(U slot,U archetype,U component,U ordinal,const assets::SceneSpan48D& span) noexcept;
    bool pairCompareResources() noexcept;
    bool pairCompareTexture(U& name,U format) noexcept;
    bool pairCompareSpan(std::uint8_t bank,U slot,U archetype,U component,U ordinal,const assets::SceneSpan48D& span) noexcept;
    bool pairCompareEvaluate(std::uint8_t bank,U slot,U component,U ordinal) noexcept;
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
