// Private member declarations; definitions are in vehicle_window_gl50ds_draw.cpp.
// Included only inside VehicleWindowGl50DS::Storage. Native renderer resources,
// not source gameplay activation or an independently executable render path.
    U gameplaySceneTarget=0,gameplaySceneColor=0;
    U gameplayHistoryTargets[2]{},gameplayHistoryColors[2]{};
    U gameplayBlurProgram=0,gameplayBlurVertexShader=0,gameplayBlurFragmentShader=0;
    U gameplayBlurVertexArray=0;
    I gameplayHistoryUniform=-1,gameplayCurrentUniform=-1,gameplayOriginalSizeUniform=-1;
    I gameplayNativeSizeUniform=-1,gameplayHistorySizeUniform=-1;
    I gameplayViewportUniform=-1,gameplayFixedUniform=-1;
    game::GameplayBlurVideoState gameplayBlurVideo{};
    GameplayBlurFrame50DS frameGameplayBlur{};
    bool frameGameplayBlurEnabled=false;
    unsigned splitPass=0;
    bool awaitingSecondView=false;
    std::uint64_t splitCameraPublication=0;
    I splitWidth=0,splitHeight=0;
    U splitFirstTarget=0;
    std::uint8_t gameplayHistoryPublished=0;
    I gameplayHistoryWidth=0,gameplayHistoryHeight=0;
    I gameplayStagedWidth=0,gameplayStagedHeight=0;
    bool gameplayHistoryValid=false,gameplayHistoryStaged=false;
    std::uint64_t gameplayHistoryGeneration=0;
    // Successful renderer window copy or headless private commit awaiting the
    // explicit display owner below. A private race commit is never history.
    U gameplayAwaitingTarget=0;
    I gameplayAwaitingWidth=0,gameplayAwaitingHeight=0;

    bool hasGameplayBlurResources() const noexcept;
    bool createGameplayBlurTarget(U& target,U& color) noexcept;
    bool createGameplayBlur() noexcept;
    bool validateGameplayBlurFrame(const GameplayBlurFrame50DS* input,VehicleWindowDiagnostic50DS& d) const noexcept;
    bool restoreGameplayBlurState(U target) noexcept;
    bool drawGameplayBlur(std::uint8_t bank) noexcept;
    void noteGameplayDisplayPending(U target,I w,I h) noexcept;
    bool stageGameplayDisplayedHistory(U target,I w,I h) noexcept;
    void acceptGameplayDisplayedHistory() noexcept;
    bool deleteGameplayBlurNames(VehicleWindowDiagnostic50DS& d) noexcept;
