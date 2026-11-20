// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// Private VehicleWindowGl50DS::Storage. Common GS destination-alpha pair route
// (any course whose material data has an A=Cs,B=0,C=Ad,D=Cd pass after a C=As
// pass on the same surface) plus the environment-map UV stream fed by the
// track's VehicleWindowEnvironmentUv50DS producer (request.track.environmentUv).
// The fixed GPU-blend route needs no extra target; the exact per-sample
// composition bridge (fallback and comparison) still needs the AA1 coverage
// resources.
    I gsAlphaPassUniform=-1,gsAlphaPayloadUniform=-1,environmentAmbientUniform=-1,environmentMapUniform=-1;
    // Load-time max vertex byte (RGB) of each span's vertex range, used by the
    // fixed route's admission test for prelit C=Ad passes. Index = span.
    std::unique_ptr<std::uint8_t[]> gsSpanMaxVertexByte;
    // Compact CPU staging, sized once at create from the producer's uvCount().
    std::unique_ptr<std::array<float,2>[]> environmentUvs;
    std::size_t environmentUvCount=0;
    std::array<std::uint8_t,4> environmentAmbient{};
    bool environmentMapPrepared=false;
    // Any span carries a destination-alpha pass mark (set at create).
    bool destinationAlphaSpans=false;
    U activeFrameBank=0;
    struct DestinationAlphaFrameObservation {
        VehicleWindowDiagnostic50DS& diagnostic;bool enabled;
        std::chrono::steady_clock::time_point start;
        DestinationAlphaFrameObservation(Storage& s,VehicleWindowDiagnostic50DS& d)noexcept:
            diagnostic(d),enabled(s.destinationAlphaSpans&&s.request.enableGameplayDisplayWallTiming),
            start(enabled?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{}){}
        ~DestinationAlphaFrameObservation(){if(!enabled)return;
            const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
            diagnostic.destinationAlphaSubmissionWallUs=static_cast<std::uint64_t>(std::max<std::int64_t>(0,elapsed));
            diagnostic.destinationAlphaWallMeasured=true;
            std::fprintf(stderr,"destination_alpha_frame triangles=%u empty=%u compositions_issued=%u full_rectangles=%u cpu_submission_wall_us=%llu batch_accepted=%u error=%u\n",
                diagnostic.destinationAlphaTriangles,diagnostic.destinationAlphaEmptyTriangles,diagnostic.destinationAlphaCompositionsIssued,
                diagnostic.destinationAlphaFullRectangles,static_cast<unsigned long long>(diagnostic.destinationAlphaSubmissionWallUs),
                diagnostic.batchAccepted?1u:0u,static_cast<unsigned>(diagnostic.error));
        }
    };
    bool createEnvironmentUvProjection(const VehicleWindowTrack50DS& track)noexcept;
    bool prepareEnvironmentUvFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d)noexcept;
    bool uploadEnvironmentUvs()noexcept;
    std::array<I,4> gsTriangleRegion(U first)const noexcept;
    bool drawDestinationAlphaTriangle(U bank,U first,U pass,U image,const std::array<I,4>& region,U triangleCount,bool environment)noexcept;
    bool fixedDestinationAlphaAdmitted(const TrackSpan& span,U pass)const noexcept;
    bool drawDestinationAlphaRange(U bank,U first,U count,U pass,U image,const TrackSpan& span)noexcept;
