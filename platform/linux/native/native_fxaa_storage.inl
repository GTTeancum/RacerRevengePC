// Private member declarations; definitions are in vehicle_window_gl50ds_draw.cpp.
// Included only inside VehicleWindowGl50DS::Storage. PC-native FXAA over the
// completed 3D scene (after coverage resolve, before any HUD/menu layer).
// Selected by RR_VIDEO_FXAA=1 (RacerRevengePC.ini [Video] FXAA, published by
// the Windows player entry). Not an original PS2 effect.
    bool fxaaEnabled=false;
    U fxaaSceneTarget=0,fxaaSceneColor=0;
    U fxaaProgram=0,fxaaVertexShader=0,fxaaFragmentShader=0;
    U fxaaVertexArray=0;
    I fxaaImageUniform=-1,fxaaInverseSizeUniform=-1,fxaaMaximumUvUniform=-1;
    std::uint64_t fxaaDraws=0;

    bool hasFxaaResources() const noexcept;
    bool createFxaa() noexcept;
    bool drawFxaa(std::uint8_t bank) noexcept;
    bool deleteFxaaNames(VehicleWindowDiagnostic50DS& d) noexcept;
