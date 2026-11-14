// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// Private VehicleWindowGl50DS::Storage. Common GS destination-alpha pair route
// (any course whose material data has an A=Cs,B=0,C=Ad,D=Cd pass after a C=As
// pass on the same surface) plus the environment-map UV owner. The fixed
// GPU-blend route needs no extra target; the exact per-sample composition
// bridge (fallback and comparison) still needs the AA1 coverage resources.
    I gsAlphaPassUniform=-1,gsAlphaPayloadUniform=-1,environmentAmbientUniform=-1,environmentMapUniform=-1;
    // Load-time max vertex byte (RGB) of each span's vertex range, used by the
    // fixed route's admission test for prelit C=Ad passes. Index = span.
    std::unique_ptr<std::uint8_t[]> gsSpanMaxVertexByte;
    std::unique_ptr<std::array<float,2>[]> rcCrystalUvs;
    std::array<std::uint8_t,4> environmentAmbient{};
    bool environmentMapPrepared=false;
    U activeFrameBank=0;
    struct RcFrameObservation {
        VehicleWindowDiagnostic50DS& diagnostic;bool enabled;
        std::chrono::steady_clock::time_point start;
        RcFrameObservation(Storage& s,VehicleWindowDiagnostic50DS& d)noexcept:
            diagnostic(d),enabled(bool(s.request.track.rcCrystal)&&s.request.enableGameplayDisplayWallTiming),
            start(enabled?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{}){}
        ~RcFrameObservation(){if(!enabled)return;
            const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
            diagnostic.rcCrystalSubmissionWallUs=static_cast<std::uint64_t>(std::max<std::int64_t>(0,elapsed));
            diagnostic.rcCrystalWallMeasured=true;
            std::fprintf(stderr,"rc_crystal_frame triangles=%u empty=%u compositions_issued=%u full_rectangles=%u cpu_submission_wall_us=%llu batch_accepted=%u error=%u\n",
                diagnostic.rcCrystalTriangles,diagnostic.rcCrystalEmptyTriangles,diagnostic.rcCrystalCompositionsIssued,
                diagnostic.rcCrystalFullRectangles,static_cast<unsigned long long>(diagnostic.rcCrystalSubmissionWallUs),
                diagnostic.batchAccepted?1u:0u,static_cast<unsigned>(diagnostic.error));
        }
    };
    bool createRcCrystalProjection(const VehicleWindowTrack50DS& track)noexcept;
    bool prepareRcCrystalFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d)noexcept;
    bool uploadRcCrystalUvs()noexcept;
    std::array<I,4> gsTriangleRegion(U first)const noexcept;
    bool drawDestinationAlphaTriangle(U bank,U first,U pass,U image,const std::array<I,4>& region,U triangleCount,bool environment)noexcept;
    bool fixedDestinationAlphaAdmitted(const TrackSpan& span,U pass)const noexcept;
    bool drawDestinationAlphaRange(U bank,U first,U count,U pass,U image,const TrackSpan& span)noexcept;
