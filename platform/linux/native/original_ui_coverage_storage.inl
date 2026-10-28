// Private member declarations; definitions are in vehicle_window_gl50ds_ui.cpp.
// Renderer-private native four-sample UI lowering. Source raster stays 640x448;
// presentation resize never resamples the in-progress per-sample destination.
// No GS edge/sample-position parity is asserted. No AA1-to-AA0 fallback.
    // Requested-format payload: 640*448*(72 color bytes +27 depth bytes)
    // =28,385,280 bytes. Budget depth24 as32 storage:30,965,760 bytes.
    // Driver tiling/metadata are not observable and are not claimed exact.
    static constexpr std::size_t uiCoverageStorageBound=640u*448u*108u;
    static_assert(uiCoverageStorageBound==30965760u);
    struct UiCoverageStorage {
        U scene=0,color=0,depth=0,snapshot=0,snapshotColor=0;
        U raster[2]{},payload[2]{},rasterDepth[2]{}; // center AA0, four-sample AA1
        U program=0,vertex=0,fragment=0,array=0;
        I mode=-1,blend=-1;
        bool ready=false,attempted=false,coherentFetch=false;
        bool owns()const noexcept {return scene||color||depth||snapshot||snapshotColor||
            raster[0]||raster[1]||payload[0]||payload[1]||rasterDepth[0]||rasterDepth[1]||program||vertex||fragment||array;}
    } uiCoverage;
    bool deleteUiCoverage(VehicleWindowDiagnostic50DS& d)noexcept;
    bool uiCoverageDepthTexture(U& name)noexcept;
    using UiCoverageClock=std::chrono::steady_clock;
    static std::uint64_t uiCoverageNs(UiCoverageClock::time_point begin)noexcept;
    struct UiCoverageTimer {std::uint64_t& elapsed;UiCoverageClock::time_point begin=UiCoverageClock::now();
        ~UiCoverageTimer(){elapsed=uiCoverageNs(begin);}};
    std::uint64_t uiCoverageAllocationNs=0,uiCoverageProgramNs=0;
    unsigned uiCoverageSetupReports=0,uiCoverageDrawReports=0,uiCoverageCompletionReports=0,uiCoveragePublishReports=0;
    unsigned uiCoverageSlowReports=0,uiCoverageSlowTotalReports=0;
    bool uiCoveragePromotionReported=false;
    bool uiCoverageSourceDiagnostic=false; // Observation only, never publication authority.
    bool prepareUiCoverageRaw()noexcept;
    bool prepareUiCoverage(const char* phase)noexcept;
    void unbindUiCoverageInputs()noexcept;
    bool beginUiCoverageSpan(bool aa1,I sourceBlend)noexcept;
    bool endUiCoverageSpan(U image,U depth,bool write)noexcept;
    bool drawUiCoverageRange(bool aa1,U index,U indexCount,U image,U depth,bool write,I sourceBlend,
        bool opaqueBatch,const OriginalUiCoverageBounds50KG& region,U sourceSampler=0)noexcept;
    bool promoteUiCoveragePrefix()noexcept;
    bool resolveUiCoverage()noexcept;
