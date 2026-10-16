#pragma once
// Native original Main session. Included by50DU after its host diagnostics and
// SessionExit50DU declaration. This lifetime owns no vehicle, world, race audio,
// shared particle manager, or cold gameplay renderer. Course publishes only a
// typed cold request, consumed after UI/GPU teardown and actual host closure.
// The Application's same Video object survives; no GL publication is retained.
inline SessionExit50DU runOriginalFrontend50LA(const char* corpusPath,bool headlessLive,
    const char* capturePath,std::uint64_t sessionIndex,HostMonotonicClock50ID& applicationClock,
    const rr::platform::HostTrackAdmission50IM& trackAdmission,
    const original_main_entry50KG::Application& originalMainApplication,
    original_course_targets50KX::ColdLoadRequest& courseColdLoad,windows_capture50LD::Run& captureRun) {
 namespace platform=rr::platform;
 require50DU(originalMainApplication.enabled&&!courseColdLoad.pending(),
     "standalone Main requires its real application and no pending Course handoff");
 require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest setup failed");
 auto nativeVideo=originalMainApplication.videoForSession();
 auto& host=nativeVideo->host;
 platform::VehicleWindowGl50DS renderer;
 original_main_entry50KG::Frontend originalMain;
 original_course_targets50KX::Catalog courseTargets;
 platform::GameHostDiagnostic50ER hostDiagnostic;
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic;
 bool courseCatalogPrepared=false,courseCorpusPresent=false;
 std::uint64_t windowSwaps=0;
 int terminalInputError=0;
 const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool allAiEnabled=allAiSetting&&std::string(allAiSetting)=="1";
 const char* raceSetting=std::getenv("RR_RACE");
 const bool raceRequested=!(raceSetting&&std::string(raceSetting)=="0");
 // Close retains context and texture owners on failure. The coordinator must
 // retire live Rider source publications before the renderer or bank dies.
 auto closeWindow=[&](){
  bool clean=true;
  if(!originalMain.coordinator.borrowsRenderer(renderer)&&renderer.ownsResources()&&
      !renderer.close(rendererDiagnostic))return false;
  if(!originalMain.closeUi())return false;
  if(!originalMain.closeAudio())clean=false;
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic))return false;
  if(renderer.ownsResources())return false;
  if(host.ownsResources()&&!host.close(hostDiagnostic))clean=false;
  return clean;
 };
 // Keep the original 640x448 logical/source canvas. Private target capacity
 // follows actual startup pixels, including HiDPI, within a bounded 4K budget.
 constexpr std::int32_t startupWidthLimit=3840,startupHeightLimit=2160;
 std::int32_t maximumWidth=1280,maximumHeight=720;
 try {
  platform::GameHostRequest50ER hostRequest;
  hostRequest.mode=headlessLive?platform::GameHostMode50ER::HeadlessLive:platform::GameHostMode50ER::Window;
  auto& window=hostRequest.window;
  window.title="Racer Revenge | Original UI | Arrows select/change | Enter confirm | R defaults | Esc back";
  window.width=nativeVideo->width;window.height=nativeVideo->height;
  window.major=3;window.minor=2;window.profile=platform::WindowGlProfile50DQ::Core;
  window.samples=0;window.swapInterval=0;window.resizable=true;window.gameControllers=false;
  window.redBits=window.greenBits=window.blueBits=window.alphaBits=8;
  window.depthBits=24;window.stencilBits=0;
  hostRequest.headless={platform::OffscreenGlPlatform50DC::MesaSurfaceless,nativeVideo->width,nativeVideo->height,
      3,2,platform::OffscreenGlProfile50DC::Core,8,8,8,8,24,0,0};
  if(!host.create(hostRequest,hostDiagnostic)){
   reportHost50DU(hostDiagnostic);throw std::runtime_error("original frontend host creation failed");
  }
  const auto initialDrawableWidth=host.info().drawableWidth;
  const auto initialDrawableHeight=host.info().drawableHeight;
  if(initialDrawableWidth<=0||initialDrawableHeight<=0||
      initialDrawableWidth>startupWidthLimit||initialDrawableHeight>startupHeightLimit){
   std::cerr<<"original_frontend_startup_drawable_rejected "<<J{
       {"drawable_width",initialDrawableWidth},{"drawable_height",initialDrawableHeight},
       {"width_limit",startupWidthLimit},{"height_limit",startupHeightLimit}}.dump()<<'\n';
   throw std::runtime_error("original frontend requires a positive startup drawable within 3840x2160");
  }
  maximumWidth=std::max(maximumWidth,initialDrawableWidth);
  maximumHeight=std::max(maximumHeight,initialDrawableHeight);
  std::cerr<<"original_frontend_target_request "<<J{
      {"logical_width",nativeVideo->width},{"logical_height",nativeVideo->height},
      {"startup_drawable_width",initialDrawableWidth},{"startup_drawable_height",initialDrawableHeight},
      {"maximum_width",maximumWidth},{"maximum_height",maximumHeight}}.dump()<<'\n';
  // The renderer independently verifies GL renderbuffer/viewport limits before
  // allocation. These CPU bounds do not claim GPU capacity or a rendered frame.
  platform::VehicleWindowRequest50DS uiRequest;
  uiRequest.context=&host;
  uiRequest.procAddress=[](void* context,const char* name)noexcept->platform::VehicleWindowGlProc50DS {
   return static_cast<platform::GameHost50ER*>(context)->procAddress(name);
  };
  uiRequest.isCurrent=[](void* context)noexcept {
   return static_cast<platform::GameHost50ER*>(context)->current();
  };
  uiRequest.maximumWidth=maximumWidth;uiRequest.maximumHeight=maximumHeight;
  uiRequest.output=headlessLive?platform::VehicleWindowOutput50DS::headless:platform::VehicleWindowOutput50DS::defaultBackBuffer;
  platform::MaterialTextureGlRequest50EB textureRequest;
  textureRequest.context=&host;textureRequest.isCurrent=uiRequest.isCurrent;
  const char* missingTextureProc=nullptr;
  require50DU(platform::loadMaterialTextureFunctions50EE(&host,uiRequest.procAddress,
      textureRequest.gl,missingTextureProc),"original frontend texture GL entry point unavailable");
  auto destination=original_main_entry50KG::courseRaceDestination([&](const original_course_targets50KX::Selection& selected){
   const auto* target=courseTargets.admits(selected);
   return target&&courseCatalogPrepared&&courseCorpusPresent&&raceRequested&&!courseColdLoad.pending()&&
       renderer.initialized()&&allAiEnabled;
  });
  // This reads genuine UI resources only. Race target authentication is deferred
  // until the real Course screen is active; Main/Options/Rider need no corpus.
  originalMain.prepare(originalMainApplication,nativeVideo,textureRequest,uiRequest,
      std::move(destination),&monotonicNow50DU,sessionIndex);
  platform::OriginalFrontendWindowRequest50LA rendererRequest;
  rendererRequest.context=uiRequest.context;rendererRequest.procAddress=uiRequest.procAddress;
  rendererRequest.isCurrent=uiRequest.isCurrent;rendererRequest.maximumWidth=uiRequest.maximumWidth;
  rendererRequest.maximumHeight=uiRequest.maximumHeight;rendererRequest.output=uiRequest.output;
  rendererRequest.enableSourceRgba=uiRequest.enableOriginalUiSourceRgba;
  rendererRequest.cpuTextures=uiRequest.originalUiCpuTextures;rendererRequest.textures=uiRequest.originalUiTextures;
  if(!renderer.createOriginalFrontend(rendererRequest,rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("dedicated original frontend renderer creation failed");
  }
  originalMain.activate(renderer);
  windows_capture50LD::Session windowCapture;
  windowCapture.prepare(captureRun,sessionIndex,maximumWidth,maximumHeight,"frontend");
  original_ui_capture_navigation50LA::Route captureRoute(captureRun.navigate(),captureRun.navigateSa(),captureRun.navigateOcco(),captureRun.navigateDudGa(),captureRun.navigateBenGa(),captureRun.navigateAlderMa(),captureRun.navigateOdyMa(),captureRun.navigateGasganoSa(),captureRun.navigateMawhonicRa());
  std::cerr<<"ORIGINAL MAIN COLD: dedicated frontend; no race corpus, racer, world, or gameplay renderer constructed.\n";
  auto reportUiState=[&](const char* reason){originalMain.report(reason,originalMainApplication,0,0);};
  bool capturePending=false;
  std::vector<std::uint8_t> captureBuffer(headlessLive?static_cast<std::size_t>(nativeVideo->width)*nativeVideo->height*4u:0u);
  std::vector<std::uint8_t> captureRgbRow(headlessLive?static_cast<std::size_t>(nativeVideo->width)*3u:0u);
  auto captureCommitted=[&](){
   if(!capturePending||!originalMain.imagePublished)return;
   require50DU(headlessLive,"capture path not configured");
   platform::VehicleWindowDiagnostic50DS readbackDiagnostic{};
   const auto readbackStart=originalMain.stageStart();
   const bool readbackOkay=renderer.readbackRgba(captureBuffer.data(),captureBuffer.size(),readbackDiagnostic);
   originalMain.reportStage("capture_readback",readbackStart,readbackOkay);
   require50DU(readbackOkay,"live frame readback failed");
   const auto fileStart=originalMain.stageStart();
   const auto width=readbackDiagnostic.readbackWidth,height=readbackDiagnostic.readbackHeight;
   require50DU(width>0&&static_cast<std::size_t>(width)*3<=captureRgbRow.size(),"capture RGB row capacity");
   const auto captureFile=std::filesystem::path(capturePath);
   const auto writingFile=std::filesystem::path(captureFile.string()+".pending");
   std::ofstream image(writingFile,std::ios::binary);
   require50DU(bool(image),"capture file open failed");
   image<<"P6\n"<<width<<' '<<height<<"\n255\n";
   for(int row=height;row>0;--row){
    const auto rowOffset=static_cast<std::size_t>(row-1)*width*4;
    for(int column=0;column<width;++column){
     const auto source=rowOffset+static_cast<std::size_t>(column)*4;
     const auto target=static_cast<std::size_t>(column)*3;
     for(unsigned channel=0;channel<3;++channel)captureRgbRow[target+channel]=captureBuffer[source+channel];
    }
    image.write(reinterpret_cast<const char*>(captureRgbRow.data()),static_cast<std::streamsize>(width)*3);
   }
   image.close(); // Publish the complete file before the capture acknowledgment.
   require50DU(bool(image),"capture file write failed");
   {
    // Same-directory rename atomically publishes the completed PPM before ACK.
    std::filesystem::rename(writingFile,captureFile);
    originalMain.reportStage("capture_file_publication",fileStart);
    const auto mainAudioInfo=originalMain.audioOutput.info();
    const auto capturedUi=originalMain.coordinator.snapshot();
    J capturedGroup=nullptr;
    std::int32_t capturedSlot=0;
    const bool capturedRider=capturedUi.route.screen==5&&capturedUi.route.page==1;
    const bool capturedCourse=capturedUi.route.screen==6&&capturedUi.route.page==1;
    if(capturedRider){
     require50DU(originalMain.coordinator.riderSession(),"original Rider capture session missing");
     capturedSlot=capturedUi.rider.slot4C8;
    }else{
     const auto* capturedNavigation=originalMain.navigation();
     require50DU(capturedNavigation,"original UI capture navigation missing");
     const auto group=capturedNavigation->controllerGroup[0];capturedGroup=group;
     capturedSlot=group>0&&static_cast<std::size_t>(group)<capturedNavigation->groups.size()?
         capturedNavigation->groups[group].selectedSlot:0;
    }
    const auto capturedAperture=capturedUi.route.screen==4?capturedUi.main.aperture:
        capturedRider?capturedUi.rider.aperture:capturedCourse?capturedUi.course.aperture:capturedUi.options.aperture;
    J receipt={{"sequence",++originalMain.captureSequence},{"native_frame",originalMain.frameCount},
        {"screen",capturedUi.route.screen},{"page",capturedUi.route.page},{"switches",capturedUi.switches},
        {"requested_screen",capturedUi.transition.requested.screen},{"requested_page",capturedUi.transition.requested.page},
        {"selected_group",capturedGroup},{"selected_slot",capturedSlot},
        {"saved_group",originalMainApplication.gameSettingsProfile->savedGroup6E8},
        {"lifecycle",static_cast<unsigned>(capturedUi.transition.lifecycle)},
        {"phase",static_cast<unsigned>(capturedUi.transition.phase)},
        {"aperture_state",static_cast<unsigned>(capturedAperture.state)},
        {"aperture_input_enabled",capturedAperture.inputEnabled268},
        {"gpu_publication",originalMain.coordinator.gpu()->publicationToken()},
        {"accepted_update_sequence",originalMain.lastAudioSequence},
        {"file",captureFile.string()},{"width",width},{"height",height},
        {"audio_mixed_frames",mainAudioInfo.mixedFrames},
        {"audio_captured_frames",mainAudioInfo.capturedFrames},{"audio_rate",mainAudioInfo.sampleRate},
        {"audio_clock_microseconds",mainAudioInfo.simulationMicroseconds},
        {"audio_accepted_nanoseconds",originalMain.audioOutput.acceptedNanoseconds()}};
    if(capturedRider){
     const bool sourceAccepted=originalMain.submittedScreen==5&&originalMain.sourceFrameGeneration!=0&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["selected_rider"]=capturedUi.rider.rider4C4;
     receipt["selection_attached"]=capturedUi.rider.selectionAttached;
     receipt["accepted_source_frame"]=sourceAccepted;
     receipt["source_frame_generation"]=sourceAccepted?originalMain.sourceFrameGeneration:0;
     receipt["source_target_generation"]=sourceAccepted?originalMain.sourceTargetGeneration:0;
     receipt["source_quads"]=sourceAccepted?originalMain.submittedQuads:0;
     receipt["source_draws"]=sourceAccepted?originalMain.submittedDraws:0;
     receipt["source_triangles"]=sourceAccepted?originalMain.submittedTriangles:0;
     receipt["source_coherent_framebuffer_fetch"]=sourceAccepted&&originalMain.submittedCoherentFramebufferFetch;
     receipt["source_texture_barriers"]=sourceAccepted?originalMain.submittedTextureBarriers:0;
    }
    if(capturedCourse){
     const bool nativeAccepted=originalMain.submittedScreen==6&&capturedUi.frameSubmitted&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["presentation"]="native_host_rgb";
     receipt["course_slot"]=capturedUi.course.courseSlot14E4;receipt["subtype_slot"]=capturedUi.course.subtypeSlot14F4;
     receipt["laps"]=capturedUi.course.laps14EC;receipt["entrants"]=capturedUi.course.entrants14F0;
     receipt["popup_enabled"]=capturedUi.course.popupEnabled438;receipt["accepted_native_frame"]=nativeAccepted;
     receipt["native_quads"]=nativeAccepted?originalMain.submittedQuads:0;
     receipt["native_draws"]=nativeAccepted?originalMain.submittedDraws:0;
    }
    std::cerr<<"original_main_capture "<<receipt.dump()<<'\n';
    reportUiState("capture");capturePending=false;return;
   }
  };
  platform::GameHostInput50ER hostInput;auto& input=hostInput.keys;
  reportUiState("bound");
  std::cerr<<"Original UI controls: arrows select/change; Enter/Space confirm; Escape/menu back; "
      "R defaults/rotate; Tab controller configuration. Window Close, quit, or stdin EOF exits.\n";
  // Bounded, opt-in observations only. Keep focus, timing and draw admission
  // unchanged; a bound frontend is not evidence that this gate ever opened.
  std::uint32_t tracePolls=0,traceGateReports=0,previousGateMask=0;
  std::int32_t previousDrawableWidth=0,previousDrawableHeight=0;
  bool previousDrawableOversized=false;
  std::uint32_t drawableCapacityReports=0;
  for(;;){
   if(captureRun.deadlineExpired()){originalMain.requestQuit();break;}
   const bool tracePoll=originalMain.traceStages&&tracePolls<2;
   if(tracePoll)std::cerr<<"original_ui_frontend_poll {\"poll\":"<<tracePolls+1
       <<",\"phase\":\"enter\"}\n";
   const bool polled=host.poll(hostInput,hostDiagnostic);
   if(tracePoll){
    std::cerr<<"original_ui_frontend_poll {\"poll\":"<<tracePolls+1
        <<",\"phase\":\"return\",\"okay\":"<<(polled?"true":"false")<<"}\n";
    ++tracePolls;
   }
   if(!polled){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("original frontend event pump failed");
   }
   if(hostInput.inputError){terminalInputError=hostInput.inputError;std::cerr<<"stdin input error "<<terminalInputError<<'\n';}
   if(input.quitRequested){originalMain.requestQuit();reportUiState("quit");break;}
   windowCapture.request(input);
   capturePending=capturePending||hostInput.captureRequested;
   if(hostInput.pauseRequested)originalMain.setHostPaused(true);
   if(hostInput.resumeRequested)originalMain.setHostPaused(false);
   if(hostInput.restartRequested||hostInput.checkpointRequested)
    std::cerr<<"Original Main stays active: restart/checkpoint has no admitted destination.\n";
   const auto& info=host.info();
   const bool drawableOversized=info.drawableWidth>maximumWidth||info.drawableHeight>maximumHeight;
   const bool drawable=info.drawableWidth>0&&info.drawableHeight>0&&
       !drawableOversized;
   if(drawableOversized!=previousDrawableOversized&&drawableCapacityReports<16){
    std::cerr<<"original_frontend_drawable_capacity {\"oversized\":"<<(drawableOversized?"true":"false")
        <<",\"drawable\":"<<(drawable?"true":"false")
        <<",\"drawable_width\":"<<info.drawableWidth<<",\"drawable_height\":"<<info.drawableHeight
        <<",\"maximum_width\":"<<maximumWidth<<",\"maximum_height\":"<<maximumHeight
        <<",\"focused\":"<<(input.focused?"true":"false")
        <<",\"minimized\":"<<(input.minimized?"true":"false")
        <<",\"native_frames\":"<<originalMain.frameCount<<"}\n";
    if(++drawableCapacityReports==16)
     std::cerr<<"Original frontend: further drawable-capacity transition diagnostics suppressed.\n";
   }
   previousDrawableOversized=drawableOversized;
   const bool physicalInput=input.focused&&!input.activityLost;
   const bool active=drawable&&(physicalInput||captureRun.unattended())&&!input.minimized&&!input.eventBudgetExhausted&&!originalMain.hostPaused();
   const std::uint32_t gateMask=(!drawable?1u:0u)|(!input.focused?2u:0u)|
       (input.minimized?4u:0u)|(input.activityLost?8u:0u)|
       (input.eventBudgetExhausted?16u:0u)|(originalMain.hostPaused()?32u:0u);
   if(originalMain.traceStages&&traceGateReports<16&&(!traceGateReports||
       gateMask!=previousGateMask||info.drawableWidth!=previousDrawableWidth||
       info.drawableHeight!=previousDrawableHeight)){
    std::cerr<<"original_ui_frontend_activity_gate {\"report\":"<<traceGateReports+1
        <<",\"native_frames\":"<<originalMain.frameCount
        <<",\"window_width\":"<<info.width<<",\"window_height\":"<<info.height
        <<",\"drawable_width\":"<<info.drawableWidth<<",\"drawable_height\":"<<info.drawableHeight
        <<",\"maximum_width\":"<<maximumWidth<<",\"maximum_height\":"<<maximumHeight
        <<",\"drawable\":"<<(drawable?"true":"false")
        <<",\"focused\":"<<(input.focused?"true":"false")
        <<",\"minimized\":"<<(input.minimized?"true":"false")
        <<",\"activity_lost\":"<<(input.activityLost?"true":"false")
        <<",\"event_budget_exhausted\":"<<(input.eventBudgetExhausted?"true":"false")
        <<",\"events_processed\":"<<input.eventsProcessed
        <<",\"host_paused\":"<<(originalMain.hostPaused()?"true":"false")
        <<",\""<<windows_capture50LD::unattendedMode<<"\":"<<(captureRun.unattended()?"true":"false")
        <<",\"active\":"<<(active?"true":"false")<<"}\n";
    ++traceGateReports;previousGateMask=gateMask;
    previousDrawableWidth=info.drawableWidth;previousDrawableHeight=info.drawableHeight;
   }
   const auto mainClock=originalMain.sampleClock(applicationClock,active);
   originalMain.audioOutput.pause(!active);
   if(!active){
    originalMain.lastUpdateNanoseconds=0;captureCommitted();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));continue;
   }
   const auto route=originalMain.coordinator.snapshot().route;
   const bool gameOptions=route.screen==12&&route.page==4;
   const bool controls=route.screen==12&&(route.page==2||route.page==3);
   const bool rider=route.screen==5&&route.page==1;
   const bool course=route.screen==6&&route.page==1;
   if(physicalInput){
   if(gameOptions||controls||rider||course){
    if(input.pressed[80])originalMain.input(0);
    if(input.pressed[79])originalMain.input(1);
    if(input.pressed[82]||input.pressed[26])originalMain.input(2);
    if(input.pressed[81]||input.pressed[22])originalMain.input(3);
   }else{
    if(input.pressed[82]||input.pressed[80]||input.pressed[26])originalMain.input(2);
    if(input.pressed[81]||input.pressed[79]||input.pressed[22])originalMain.input(3);
   }
   if(input.pressed[40]||input.pressed[44])originalMain.input(4);
   if((gameOptions||controls)&&input.pressed[21])originalMain.input(5);
   if(controls&&input.pressed[43])originalMain.input(7);
   if(input.pressed[41]||hostInput.menuRequested)originalMain.input(6);
   }
   if(captureRoute.enabled()){
    require50DU(originalMain.inputCount==captureRoute.issuedActions(),"physical menu input interrupted unattended original UI route");
    const auto snapshot=originalMain.coordinator.snapshot();
    const auto* navigation=originalMain.navigation();
    const auto group=navigation?navigation->controllerGroup[0]:0;
    const auto slot=navigation&&group>0&&static_cast<std::size_t>(group)<navigation->groups.size()?
        navigation->groups[group].selectedSlot:0;
    const bool sourceAccepted=originalMain.submittedScreen==5&&originalMain.sourceFrameGeneration&&
        originalMain.submittedTriangles&&originalMain.coordinator.gpu()&&
        originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
    // Inspect the actual retained profile and cold catalog. RA is deliberately
    // absent until genuine runtime owners are integrated; UI fixture availability
    // is never a race-readiness certificate.
    std::uint8_t selectedRiderBits=0;std::uint32_t selectedComparison=0;
    const bool mawhonicProfileReady=originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->riderConfirmSnapshot(selectedRiderBits,selectedComparison)&&
        (selectedRiderBits>>3)==9&&selectedComparison==9;
    // Read-only prospective tuple, identical to frontend courseLaunchProposal;
    // source profile track/lap packing is not committed until actual Confirm.
    const original_course_targets50KX::Selection mawhonicSelection{
        snapshot.course.selectedCourse44,snapshot.course.selectedPlanet40,9,
        snapshot.course.subtypeSlot14F4,snapshot.course.laps14EC,snapshot.course.entrants14F0,
        static_cast<std::uint16_t>(3u|(1u<<5)|(7u<<7)|(9u<<11))};
    const bool mawhonicTargetReady=!captureRun.navigateMawhonicRa()||
        (mawhonicProfileReady&&snapshot.course.selectedCourse44==7&&snapshot.course.selectedPlanet40==2&&
         snapshot.course.courseSlot14E4==5&&rr::game::nativeCourseTrackOwned50KG(7,2,5)&&
         courseTargets.admits(mawhonicSelection));
    const auto action=captureRoute.poll(snapshot,group,slot,
        windowCapture.captured(windows_capture50LD::Stage::main),windowCapture.captured(windows_capture50LD::Stage::rider),
        windowCapture.captured(windows_capture50LD::Stage::course),sourceAccepted,
        courseCatalogPrepared&&courseCorpusPresent&&raceRequested&&allAiEnabled&&mawhonicTargetReady,
        originalMain.sourceFrameGeneration,originalMain.submittedPublication);
    if(action>=0){originalMain.input(static_cast<std::uint8_t>(action));captureRoute.validateDispatch(originalMain.coordinator.snapshot());}
   }
   const auto elapsedNanoseconds=originalMain.acceptedElapsed(monotonicNow50DU());
   platform::OriginalUiMainSessionUpdate50KG update;
   update.elapsedSeconds=static_cast<float>(double(elapsedNanoseconds)/1000000000.0);
   originalMain.update(update,elapsedNanoseconds);
   originalMain.render(mainClock,renderer,info.drawableWidth,info.drawableHeight);
   if(!originalMain.coordinator.snapshot().rider.deferred4EC){
    const auto snapshot=originalMain.coordinator.snapshot();
    const auto stage=snapshot.route.screen==4?windows_capture50LD::Stage::main:
        snapshot.route.screen==5?windows_capture50LD::Stage::rider:
        snapshot.route.screen==6?windows_capture50LD::Stage::course:windows_capture50LD::Stage::other;
    const bool sourceAccepted=originalMain.submittedScreen==5&&originalMain.sourceFrameGeneration&&
        originalMain.submittedTriangles&&originalMain.coordinator.gpu()&&
        originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
    const bool settled=original_ui_capture_navigation50LA::Route::settled(snapshot)&&
        (stage!=windows_capture50LD::Stage::rider||sourceAccepted);
    const bool captureWindowImage=host.hasWindow()&&captureRoute.captureAllowed(snapshot,sourceAccepted,originalMain.sourceFrameGeneration,
        originalMain.submittedPublication)&&windowCapture.due(stage,settled);
    const auto swapStart=originalMain.stageStart();
    if(host.hasWindow()&&!windowCapture.swapAndCapture(renderer,host,captureWindowImage,windowSwaps+1,
        [&](){return host.swapWindow(hostDiagnostic);})){
     originalMain.reportStage("window_swap",swapStart,false);
     reportHost50DU(hostDiagnostic);throw std::runtime_error("original frontend window swap failed");
    }
    if(host.hasWindow()){++windowSwaps;originalMain.reportStage("window_swap",swapStart);}
    originalMain.imagePublished=true;++originalMain.frameCount;
    if(originalMain.frameCount==1){
     J ready={{"native_frame",originalMain.frameCount},{"width",info.drawableWidth},{"height",info.drawableHeight},
         {"frontend_only",true}};
     std::cerr<<"original_main_ready "<<ready.dump()<<'\n';reportUiState("ready");
    }
    captureCommitted();
    if(host.hasWindow()){
     if(captureWindowImage){
      J receipt={{"screen",snapshot.route.screen},{"page",snapshot.route.page},{"switches",snapshot.switches},
          {"native_frame",originalMain.frameCount},{"window_swaps",windowSwaps},
          {"accepted_update_sequence",originalMain.lastAudioSequence},{"gpu_publication",originalMain.submittedPublication},
          {"source_frame_generation",originalMain.sourceFrameGeneration},{"source_target_generation",originalMain.sourceTargetGeneration},
          {"source_triangles",originalMain.submittedTriangles},{"accepted_rider_source",sourceAccepted}};
      if(stage==windows_capture50LD::Stage::rider){receipt["selected_rider"]=snapshot.rider.rider4C4;
       receipt["selected_character_ready"]=snapshot.rider.selectedCharacterReady;
       receipt["selected_slot"]=snapshot.rider.slot4C8;receipt["selection_attached"]=snapshot.rider.selectionAttached;}
      if(stage==windows_capture50LD::Stage::course)receipt["course_selection"]={
          {"course",snapshot.course.selectedCourse44},{"planet",snapshot.course.selectedPlanet40},
          {"course_slot",snapshot.course.courseSlot14E4},{"subtype_slot",snapshot.course.subtypeSlot14F4},
          {"laps",snapshot.course.laps14EC},{"entrants",snapshot.course.entrants14F0}};
      windowCapture.record(renderer,host,stage,settled,std::move(receipt));
     }
    }
   }
   originalMain.commitDeferred(renderer);
   const auto nextRoute=originalMain.coordinator.snapshot().route;
   if(!courseCatalogPrepared&&nextRoute.screen==6&&nextRoute.page==1){
    // Cold readiness work is outside noexcept source input callbacks. Missing
    // resources disable race Confirm; malformed existing roster remains fatal.
    const auto begin=monotonicNow50DU();
    courseTargets.prepare(trackAdmission);courseCatalogPrepared=true;
    courseCorpusPresent=corpusPath&&std::filesystem::is_regular_file(std::filesystem::path(corpusPath));
    if(!courseCorpusPresent)std::cerr<<"Course race unavailable: native race corpus is absent.\n";
    if(!raceRequested)std::cerr<<"Course race unavailable: RR_RACE=0 explicitly disables race entry.\n";
    const auto end=monotonicNow50DU();require50DU(end>=begin,"Course cold clock moved backwards");
    const auto elapsed=end-begin;
    require50DU(elapsed<=std::numeric_limits<std::uint64_t>::max()-originalMain.pendingUpdateColdNanoseconds&&
        elapsed<=std::numeric_limits<std::uint64_t>::max()-originalMain.pendingMaterialColdNanoseconds,
        "Course cold interval overflow");
    originalMain.pendingUpdateColdNanoseconds+=elapsed;originalMain.pendingMaterialColdNanoseconds+=elapsed;
   }
   original_course_targets50KX::Selection selected;
   if(originalMain.coordinator.takeCourseLaunch(selected)){
    if(captureRun.navigateMawhonicRa())require50DU(captureRoute.actionsDispatched()&&
        selected.rider==9&&selected.course==7&&selected.planet==2&&selected.laps==3&&selected.entrants==8,
        "Mawhonic/RA capture rejected a mismatched original Course handoff");
    if(captureRun.navigateGasganoSa())require50DU(captureRoute.actionsDispatched()&&
        selected.rider==7&&selected.course==11&&selected.planet==4&&selected.laps==3&&selected.entrants==8,
        "Gasgano/SA capture rejected a mismatched original Course handoff");
    const auto* target=courseTargets.admits(selected);
    require50DU(target&&courseCorpusPresent&&courseColdLoad.publish(courseTargets,selected,sessionIndex+1),
        "actual Course exit could not publish its typed cold-load request");
    std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<target->assets->world->code
        <<" rider="<<selected.rider<<" outgoing_world_ticks=0 frontend_only=1\n";
    break;
   }
   std::this_thread::sleep_for(std::chrono::milliseconds(8));
  }
  require50DU(terminalInputError==0,"original frontend input stream failed");
 }catch(...){
  if(!closeWindow()&&!closeWindow()){
   std::cerr<<"original frontend cleanup unresolved; retaining context and owners by terminating\n";std::terminate();
  }
  throw;
 }
 bool closed=closeWindow();
 if(!closed&&!(closed=closeWindow())){
  std::cerr<<"original frontend cleanup unresolved; retaining context and owners by terminating\n";std::terminate();
 }
 require50DU(closed,"original frontend cleanup reported a failure");
 const auto audio=originalMain.audioOutput.info();
 J receipt={{"native_frames",originalMain.frameCount},{"captures",originalMain.captureSequence},
     {"world_ticks",0},{"window_swaps",windowSwaps},{"cleanup_complete",true},{"frontend_only",true},
     {"audio_mixed_frames",audio.mixedFrames},{"audio_source_mixed_frames",originalMain.audioOutput.sourceMixedFrames()},
     {"audio_captured_frames",audio.capturedFrames},{"audio_rate",audio.sampleRate},
     {"audio_accepted_nanoseconds",originalMain.audioOutput.acceptedNanoseconds()}};
 std::cout<<"original_main_closed "<<receipt.dump()<<'\n';
 return courseColdLoad.pending()?SessionExit50DU::CourseColdLoad:SessionExit50DU::Quit;
}
