#pragma once
#include <cstdio>
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "../../platform/linux/native/test_results_input50pd.h"
#include <cstdlib>
#include "native_display_capacity50du.h"
#include <exception>
#include "../../platform/linux/native/original_ui_controller_navigation50la.h"
#include "../../platform/linux/native/original_ui_main_exit_game50xg.h" // PC-NATIVE ADDITION (EXIT GAME)
// Native original Main session. Included by50DU after its host diagnostics and
// SessionExit50DU declaration. This lifetime owns no vehicle, world, race audio,
// shared particle manager, or cold gameplay renderer. Course publishes only a
// typed cold request, consumed after UI/GPU teardown. Normal player entry keeps
// the same native window/context; no session GL publication is retained.
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
 auto sourceDisplayLifetime=originalMainApplication.raceDisplayForSession50LA();
 auto& host=nativeVideo->host;
 auto& renderer=sourceDisplayLifetime->renderer();
 original_main_entry50KG::Frontend originalMain;
 original_course_targets50KX::Catalog courseTargets;
 platform::GameHostDiagnostic50ER hostDiagnostic;
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic;
 bool courseCatalogPrepared=false,courseCorpusPresent=false;
 std::uint64_t windowSwaps=0;
 int terminalInputError=0;
 const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool allAiEnabled=allAiSetting&&std::string(allAiSetting)=="1";
 const bool explicitOriginalRoot=
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool directHumanCourseSupported=!allAiEnabled&&explicitOriginalRoot;
 // Capability only: the actual catalog/profile/roster and typed cold-load
 // guards below still authenticate the selected rider and destination.
 const auto directHumanTargetSupported=[&](original_course_targets50KX::Track track) noexcept {
  return directHumanCourseSupported&&original_course_targets50KX::directHumanTrack(track);
 };
 const auto courseTargetModeSupported=[&](original_course_targets50KX::Track track) noexcept {
  // A rooted direct-human target cannot use the ALL8 fallback to turn its
  // actual Player0 into AI. Legacy unrooted targets and bounded RA retain
  // their independent all-AI policy; GA never has a legacy human fallback.
  const bool directHumanRequired=track==original_course_targets50KX::Track::GA||
      (explicitOriginalRoot&&original_course_targets50KX::directHumanTrack(track));
  return directHumanRequired?directHumanTargetSupported(track):allAiEnabled;
 };
 const char* raceSetting=std::getenv("RR_RACE");
 const bool raceRequested=!(raceSetting&&std::string(raceSetting)=="0");
 // Close retains context and texture owners on failure. The coordinator must
 // retire live Rider source publications before the renderer or bank dies.
 auto closeWindow=[&](){
  bool clean=true;
  // Garage Draw retirement needs its real process FIFO. Retire only that
  // retained manager first, not the generic renderer/UI. Partial source
  // failure retains both custody and queued work.
  if(originalMain.coordinator.ownsGarageCustody()){
   const auto retired=originalMain.coordinator.retireGarageBeforeClose(originalMain.diagnostic);
   if(retired!=rr::platform::OriginalUiFrontendStatus50KG::ok&&
      retired!=rr::platform::OriginalUiFrontendStatus50KG::noChange){
    std::cerr<<"original frontend Garage retirement source="<<originalMain.diagnostic.sourcePC
        <<" status="<<static_cast<unsigned>(retired)<<" detail="<<originalMain.diagnostic.garageDetail<<'\n';return false;
   }
  }
  // Retire only the actual empty process collector. Queued source records,
  // draining work, registry pins or population owners still refuse closure.
  // Do not close an unborrowed renderer first: partial startup can own real
  // UI payloads before coordinator.activate publishes its renderer borrow.
  rr::assets::OriginalStandardMaterialDiagnostic50LA collector;
  if(!originalMainApplication.retireNativeCollector1A6948(collector)){
   std::cerr<<"original frontend collector retirement source="<<collector.sourcePC
       <<" status="<<static_cast<unsigned>(collector.status)<<'\n';return false;
  }
  // Diagnostic only: every silent refusal below names its owner once per try.
  const auto refused=[&](const char* step){
   std::cerr<<"frontend_close_refused owner=session step="<<step
       <<" renderer_owns="<<renderer.ownsResources()<<" renderer_dependents="<<renderer.hasSessionDependents50LA()
       <<" renderer_operation="<<(rendererDiagnostic.operation?rendererDiagnostic.operation:"")
       <<" renderer_cleanup_incomplete="<<rendererDiagnostic.cleanupIncomplete
       <<" coordinator_status="<<static_cast<unsigned>(originalMain.diagnostic.status)
       <<" coordinator_pc="<<std::hex<<originalMain.diagnostic.sourcePC<<std::dec
       <<" coordinator_renderer_operation="<<(originalMain.diagnostic.renderer.operation?originalMain.diagnostic.renderer.operation:"")
       <<" route_screen="<<static_cast<unsigned>(originalMain.coordinator.route().screen)<<'\n';
   return false;
  };
  if(!originalMain.closeUi())return refused("close_ui");
  if(!originalMain.closeAudio())clean=false;
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic))return refused("renderer_close");
  if(renderer.ownsResources())return refused("renderer_still_owns_resources");
  if(!sourceDisplayLifetime->beginCloseAfterDependents())return refused("display_begin_close_after_dependents");
  if(!originalMainApplication.retainsPlayerWindow50DU()&&
     host.ownsResources()&&!host.close(hostDiagnostic))clean=false;
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
  window.title="Star Wars: Racer Revenge";
  window.width=nativeVideo->width;window.height=nativeVideo->height;
#if defined(_WIN32)
  // Integer-sized host presentation; original Video and source UI remain unchanged.
  if(!headlessLive&&nativeVideo->width==640&&nativeVideo->height==448){
   window.width=1280;window.height=896;
   // Player video setting (RacerRevengePC.ini WindowWidth/WindowHeight).
   if(const char* size=std::getenv("RR_VIDEO_WINDOW_SIZE")){
    int w=0,h=0;
    if(std::sscanf(size,"%dx%d",&w,&h)==2&&w>=640&&h>=448&&w<=3840&&h<=2160){window.width=w;window.height=h;}
   }
  }
  // Unattended test only: open the window at an explicit size (WxH).
  if(const char* size=std::getenv("RR_TEST_WINDOW_SIZE");size&&captureRun.unattended()){
   int w=0,h=0;
   if(std::sscanf(size,"%dx%d",&w,&h)==2&&w>=640&&h>=448&&w<=3840&&h<=2160){window.width=w;window.height=h;}
  }
#endif
  // GL4.0 core first (UI AA1 / Garage models use gl_SampleID); 3.2 fallback.
  window.major=4;window.minor=0;window.profile=platform::WindowGlProfile50DQ::Core;
  window.samples=0;window.swapInterval=0;window.resizable=true;
  // Physical menu devices belong to this actual frontend window. There is no
  // racer/control owner in this session, including the ALL8 launch route.
  window.gameControllers=!headlessLive;
  window.redBits=window.greenBits=window.blueBits=window.alphaBits=8;
  window.depthBits=24;window.stencilBits=0;
  hostRequest.headless={platform::OffscreenGlPlatform50DC::MesaSurfaceless,nativeVideo->width,nativeVideo->height,
      3,2,platform::OffscreenGlProfile50DC::Core,8,8,8,8,24,0,0};
  if(host.created()){
   require50DU(originalMainApplication.retainsPlayerWindow50DU()&&host.hasWindow()&&host.current(),
       "original frontend requires the same retained current player window");
  }else if(!host.create(hostRequest,hostDiagnostic)){
   std::cerr<<"native_window_gl4_unavailable retrying=3.2\n";
   window.major=3;window.minor=2;hostDiagnostic={};
   if(!host.create(hostRequest,hostDiagnostic)){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("original frontend host creation failed");
   }
  }
  rr::platform::loading_progress50DU::pulse("Preparing Main");
#if defined(_WIN32)
  // Observe the actual current frontend context, never a guessed GPU identity.
  using NativeGlString=const unsigned char*(*)(unsigned int);
  const auto nativeGlString=reinterpret_cast<NativeGlString>(host.procAddress("glGetString"));
  if(nativeGlString){
   const char* labels[]={"vendor","renderer","version"};
   const unsigned int fields[]={0x1f00u,0x1f01u,0x1f02u};
   for(unsigned i=0;i<3;++i){const auto* value=nativeGlString(fields[i]);
    std::cerr<<"native_gl_"<<labels[i]<<'='<<(value?reinterpret_cast<const char*>(value):"unavailable")<<'\n';}
  }
#endif
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
  {  // Resizable/maximizable frontend window: cover any attached monitor.
   std::int32_t displayWidth=0,displayHeight=0;
   native_display_capacity50DU::capacity(maximumWidth,maximumHeight,displayWidth,displayHeight);
   maximumWidth=displayWidth;maximumHeight=displayHeight;
  }
  std::cerr<<"original_frontend_target_request "<<J{
      {"logical_width",nativeVideo->width},{"logical_height",nativeVideo->height},
      {"startup_drawable_width",initialDrawableWidth},{"startup_drawable_height",initialDrawableHeight},
      {"maximum_width",maximumWidth},{"maximum_height",maximumHeight}}.dump()<<'\n';
  // The renderer independently verifies GL renderbuffer/viewport limits before
  // allocation. These CPU bounds do not claim GPU capacity or a rendered frame.
  platform::VehicleWindowRequest50DS uiRequest;
  // Use the same retained Application display/context identity as source
  // services. UI upload ownership remains the actual Frontend UploadLifetime.
  sourceDisplayLifetime->bindFrontendCreationRequest(uiRequest);
  uiRequest.maximumWidth=maximumWidth;uiRequest.maximumHeight=maximumHeight;
  uiRequest.output=headlessLive?platform::VehicleWindowOutput50DS::headless:platform::VehicleWindowOutput50DS::defaultBackBuffer;
  platform::MaterialTextureGlRequest50EB textureRequest;
  textureRequest.context=&host;textureRequest.isCurrent=uiRequest.isCurrent;
  const char* missingTextureProc=nullptr;
  require50DU(platform::loadMaterialTextureFunctions50EE(&host,uiRequest.procAddress,
      textureRequest.gl,missingTextureProc),"original frontend texture GL entry point unavailable");
  auto destination=original_main_entry50KG::courseRaceDestination([&](const original_course_targets50KX::Selection& selected){
   const auto* target=courseTargets.admits(selected);
   return target&&courseCatalogPrepared&&(target->track==original_course_targets50KX::Track::RA||
       directHumanTargetSupported(target->track)||courseCorpusPresent)&&
       raceRequested&&!courseColdLoad.pending()&&(!allAiEnabled||selected.laps==3)&&
       renderer.initialized()&&courseTargetModeSupported(target->track);
  },[&](const original_course_targets50KX::Selection& selected){return courseTargets.availability(selected);});
  // This reads genuine UI resources only. Race target authentication is deferred
  // until the real Course screen is active; Main/Options/Rider need no corpus.
  originalMain.prepare(originalMainApplication,nativeVideo,textureRequest,uiRequest,
      std::move(destination),&monotonicNow50DU,sessionIndex);
  // Authentic APPSTART was prepared above; this same native host is current.
  // Retain the existing Application-owned canonical construction context
  // before the dedicated language/font service borrows it.
  require50DU(bool(nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication)),
      "original frontend requires its genuine canonical material context");
  platform::OriginalFrontendWindowRequest50LA rendererRequest;
  rendererRequest.context=uiRequest.context;rendererRequest.procAddress=uiRequest.procAddress;
  rendererRequest.isCurrent=uiRequest.isCurrent;rendererRequest.maximumWidth=uiRequest.maximumWidth;
  rendererRequest.maximumHeight=uiRequest.maximumHeight;rendererRequest.output=uiRequest.output;
  rendererRequest.enableSourceRgba=uiRequest.enableOriginalUiSourceRgba;
  rendererRequest.cpuTextures=uiRequest.originalUiCpuTextures;rendererRequest.textures=uiRequest.originalUiTextures;
  if(!sourceDisplayLifetime->createOriginalFrontend(rendererRequest,rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("dedicated original frontend renderer creation failed");
  }
  // Genuine243CB4/known-null Camera0 exists now, before any Page camera.
  // This process-owned prefix remains unselected/unattached and does not
  // replace the Main coordinator or authorize loading-screen progress.
  std::uint32_t loading330PC=0;
  if(!originalMainApplication.prepareFrontendLoading330Viewport(sourceDisplayLifetime,rendererDiagnostic,loading330PC)){
   reportRenderer50DU(rendererDiagnostic);
   std::cerr<<"loading330 viewport prefix stopped source="<<loading330PC<<'\n';
   throw std::runtime_error("original loading-manager viewport prefix rejected");
  }
  originalMain.activate(renderer);
  windows_capture50LD::Session windowCapture;
  windowCapture.prepare(captureRun,sessionIndex,maximumWidth,maximumHeight,"frontend");
  // A frontend opened by a race's 1AB5BC request starts in its Garage, or in
  // the tournament Hall11 / Win7 / Unlock10 that leads on to it.
  const auto firstScreen=originalMain.coordinator.snapshot().route.screen;
  const bool raceReturnGarage=firstScreen==13;
  // Free Play's 1AB908 row opens Course6 first; a fresh frontend opens Main4.
  const bool raceReturnScreen=raceReturnGarage||firstScreen==11||firstScreen==7||firstScreen==10||firstScreen==6;
  if(raceReturnScreen){std::cerr<<"frontend_race_return first_screen="<<unsigned(firstScreen)<<'\n';captureRun.noteRaceReturn();}
  // Unattended test only: RR_TEST_RESULTS_HALL_CAPTURE=1 writes the settled
  // race-return Hall (screen 11, the 1AB5BC Hall row) once, as hall-N.
  const bool raceReturnHallCapture=raceReturnScreen&&firstScreen==11&&[]{
      const char* v=std::getenv("RR_TEST_RESULTS_HALL_CAPTURE");return v&&std::string(v)=="1";}();
  if(raceReturnHallCapture)std::cerr<<"test_results_hall_capture armed first_screen=11\n";
  original_ui_capture_navigation50LA::TournamentRoute tournamentRoute(captureRun.navigateTournamentTa()&&!raceReturnScreen);
  original_ui_capture_navigation50LA::TournamentReturnRoute tournamentReturnRoute(captureRun.navigateTournamentTa()&&raceReturnScreen);
  original_ui_capture_navigation50LA::HallRoute hallRoute(captureRun.navigateHallTa()&&!raceReturnScreen);
  // Unattended RR_TEST_RESULTS_EXIT run: after the free-play Results returned
  // to Main, this frontend's route stays idle (no second race is started).
  const bool resultsExitReturned=rr::platform::testResultsInput50PD().returnedToMain;
  if(resultsExitReturned)std::cerr<<"test_results_exit frontend_after_results route=idle first_screen="<<unsigned(firstScreen)<<'\n';
  // Unattended test only (PC-NATIVE ADDITION, EXIT GAME): RR_TEST_MAIN_EXIT=fresh selects
  // EXIT GAME on the first frontend's Main (no route is driven there); =return does it in a
  // frontend opened after a race, first pressing Back on Rider/Course/Hall/Options until
  // Main. Every step is an ordinary menu input through originalMain.input, as a player would.
  const char* mainExitSetting=captureRun.unattended()?std::getenv("RR_TEST_MAIN_EXIT"):nullptr;
  const bool mainExitFresh=mainExitSetting&&std::string(mainExitSetting)=="fresh";
  const bool mainExitReturn=mainExitSetting&&std::string(mainExitSetting)=="return";
  const bool mainExitTest=(mainExitFresh&&sessionIndex==0)||(mainExitReturn&&sessionIndex>0);
  if(mainExitSetting)std::cerr<<"test_main_exit mode="<<mainExitSetting<<" session="<<sessionIndex
      <<" armed="<<(mainExitTest?1:0)<<" first_screen="<<unsigned(firstScreen)<<'\n';
  std::uint64_t mainExitLastActionFrame=0;std::uint32_t mainExitActions=0;
  bool mainExitOpenSent=false,mainExitConfirmSent=false,mainExitUnavailableLogged=false,mainExitNoStepLogged=false;
  original_ui_capture_navigation50LA::Route captureRoute(captureRun.navigate()&&!captureRun.navigateTournamentTa()&&!captureRun.navigateHallTa()&&!resultsExitReturned&&!raceReturnScreen&&!mainExitTest);
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
    const bool capturedHall=capturedUi.route.screen==11&&capturedUi.route.page==1;
    const bool capturedTournament=capturedUi.route.screen==9&&capturedUi.route.page==1;
    const bool capturedGarage=capturedUi.route.screen==13&&capturedUi.route.page==1;
    if(capturedRider){
     require50DU(originalMain.coordinator.riderSession(),"original Rider capture session missing");
     capturedSlot=capturedUi.rider.slot4C8;
    }else if(!capturedGarage){
     const auto* capturedNavigation=originalMain.navigation();
     require50DU(capturedNavigation,"original UI capture navigation missing");
     const auto group=capturedNavigation->controllerGroup[0];capturedGroup=group;
     capturedSlot=group>0&&static_cast<std::size_t>(group)<capturedNavigation->groups.size()?
         capturedNavigation->groups[group].selectedSlot:0;
    }
    const auto capturedAperture=capturedUi.route.screen==4?capturedUi.main.aperture:
        capturedRider?capturedUi.rider.aperture:capturedCourse?capturedUi.course.aperture:capturedHall?capturedUi.hall.aperture:capturedTournament?capturedUi.tournament.aperture:capturedGarage?capturedUi.garageAperture:capturedUi.options.aperture;
    J receipt={{"sequence",++originalMain.captureSequence},{"native_frame",originalMain.frameCount},
        {"screen",capturedUi.route.screen},{"page",capturedUi.route.page},{"switches",capturedUi.switches},
        {"requested_screen",capturedGarage?J(nullptr):J(capturedUi.transition.requested.screen)},{"requested_page",capturedGarage?J(nullptr):J(capturedUi.transition.requested.page)},
        {"selected_group",capturedGroup},{"selected_slot",capturedGarage?J(nullptr):J(capturedSlot)},
        {"saved_group",originalMainApplication.gameSettingsProfile->savedGroup6E8},
        {"lifecycle",capturedGarage?capturedUi.garageManagerLifecycle:static_cast<unsigned>(capturedUi.transition.lifecycle)},
        {"phase",capturedGarage?capturedUi.garageManagerTransition:static_cast<unsigned>(capturedUi.transition.phase)},
        {"aperture_state",static_cast<unsigned>(capturedAperture.state)},
        {"aperture_input_enabled",capturedAperture.inputEnabled268},
        {"gpu_publication",capturedGarage?J(nullptr):J(originalMain.coordinator.gpu()->publicationToken())},
        {"accepted_update_sequence",originalMain.lastAudioSequence},
        {"file",captureFile.string()},{"width",width},{"height",height},
        {"audio_mixed_frames",mainAudioInfo.mixedFrames},
        {"audio_source_frame_origin",originalMain.audioOutput.sourceFrameOrigin()},
        {"audio_output_source_mixed_frames",originalMain.audioOutput.outputSourceMixedFrames()},
        {"audio_output_serial",originalMain.audioOutput.retainedOutputSerial()},
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
    if(capturedGarage){
     // Real Garage is a completed Root target, not the legacy immutable
     // SourceFrame/GPU bank or a copied Options navigation object.
     const bool accepted=capturedUi.frameSubmitted&&capturedUi.garageTargetPublished&&
         capturedUi.garageRootReturned&&capturedUi.garageTimingCommitted&&
         originalMain.submittedScreen==13&&capturedUi.garagePublication!=0&&
         originalMain.submittedGaragePublication==capturedUi.garagePublication;
     receipt["presentation"]="native_source_rgba_target";
     receipt["garage_loaded"]=capturedUi.garageLoaded;
     receipt["selected_stat"]=capturedUi.garageLoaded?J(capturedUi.garageSelectedStat):J(nullptr);
     receipt["source_publication"]=capturedUi.garagePublication;
     receipt["root_returned"]=capturedUi.garageRootReturned;
     receipt["timing_committed"]=capturedUi.garageTimingCommitted;
     receipt["target_published"]=capturedUi.garageTargetPublished;
     receipt["accepted_native_target"]=accepted;
     receipt["aperture_bound"]=capturedAperture.bound;
     receipt["aperture_top"]=capturedAperture.top284;
     receipt["aperture_bottom"]=capturedAperture.bottom28C;
    }
    if(capturedTournament){
     const bool sourceAccepted=capturedUi.frameSubmitted&&originalMain.submittedScreen==9&&originalMain.sourceFrameGeneration!=0&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["career"]=capturedUi.tournament.career;receipt["prompt_mask"]=capturedUi.tournament.promptMask42C;
     receipt["camera_active"]=capturedUi.tournament.cameraActive;receipt["accepted_source_frame"]=sourceAccepted;
     receipt["source_frame_generation"]=sourceAccepted?originalMain.sourceFrameGeneration:0;
     receipt["source_target_generation"]=sourceAccepted?originalMain.sourceTargetGeneration:0;
     receipt["source_quads"]=sourceAccepted?originalMain.submittedQuads:0;
     receipt["source_draws"]=sourceAccepted?originalMain.submittedDraws:0;
     receipt["source_triangles"]=sourceAccepted?originalMain.submittedTriangles:0;
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
  platform::OriginalUiControllerNavigation50LA originalUiControllerNavigation,originalUiSecondControllerNavigation;
  reportUiState("bound");
  std::cerr<<"Original UI controls: arrows select/change; Enter/Space confirm; Escape/menu back; "
      "R defaults/rotate; Tab controller configuration. Pad: D-pad or left stick select/change, A confirm, Y back. Center the stick between steps; only Course keeps its original horizontal held repeat. Accepted Course Confirm retains that input device for driving unless RR_HUMAN_INPUT_DEVICE overrides it; keyboard wins simultaneous confirms. In race: Start opens Pause/resumes from its main page, D-pad selects, A confirms, Y cancels/backs out. Window Close, quit, or stdin EOF exits.\n";
  // Bounded, opt-in observations only. Keep focus, timing and draw admission
  // unchanged; a bound frontend is not evidence that this gate ever opened.
  std::uint32_t tracePolls=0,traceGateReports=0,previousGateMask=0;
  std::int32_t previousDrawableWidth=0,previousDrawableHeight=0;
  bool previousDrawableOversized=false;
  std::uint32_t drawableCapacityReports=0;
#if defined(_WIN32)
  std::array<std::int64_t,6> loggedSelection{{-1,-1,-1,-1,-1,-1}};
#endif
  // Original 1B7D98 (after 2AE5E0..2AED10), the last menu step; stand-in: end of activation.
  rr::platform::loading_progress50DU::step(.1f,0x1b7d98);
  rr::platform::loading_progress50DU::finish();
  PlayerLoadingSession50DU::checkStop();
  // A race-return Garage's teardown needs its first Root frame (its scene,
  // droid routing and page retirement start there). A close request that
  // arrives before that frame is honoured right after it: one frame.
  bool closeAfterGarageFirstFrame=false;
  const auto garageFirstFramePending=[&]()noexcept{return raceReturnGarage&&originalMain.frameCount==0;};
  for(;;){
   const auto frontendIterationStart=std::chrono::steady_clock::now();
   // PC-NATIVE ADDITION (EXIT GAME): a confirmed Main EXIT GAME takes the same clean
   // close as the window Close button below (requestQuit, then closeWindow after the loop).
   if(rr::platform::takeOriginalUiMainExitGameRequest50XG()){
    std::cerr<<"pc_native_exit_game frontend_close route_screen="<<unsigned(originalMain.coordinator.route().screen)
        <<" session="<<sessionIndex<<" race_return="<<(raceReturnScreen||sessionIndex>0?1:0)<<'\n';
    originalMain.requestQuit();reportUiState("exit_game");break;
   }
   if(closeAfterGarageFirstFrame&&!garageFirstFramePending()){
    std::cerr<<"frontend_close_after_garage_first_frame\n";originalMain.requestQuit();break;}
   if(captureRun.deadlineExpired()&&!closeAfterGarageFirstFrame){
    if(!garageFirstFramePending()){originalMain.requestQuit();break;}
    closeAfterGarageFirstFrame=true;
   }
   // Test only: RR_TEST_CLOSE_GARAGE_AT_FRAME=N requests a window close at
   // native frame N of a race-return Garage (0 = before its first frame).
   if(raceReturnGarage&&captureRun.unattended()&&!closeAfterGarageFirstFrame){
    static const long closeAt=[]{const char* v=std::getenv("RR_TEST_CLOSE_GARAGE_AT_FRAME");return v&&*v?std::strtol(v,nullptr,10):-1L;}();
    if(closeAt>=0&&originalMain.frameCount>=static_cast<std::uint64_t>(closeAt)){
     std::cerr<<"test_close_garage_at_frame frame="<<originalMain.frameCount<<'\n';
     if(!garageFirstFramePending()){originalMain.requestQuit();break;}
     closeAfterGarageFirstFrame=true;
    }
   }
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
   if(input.quitRequested&&!garageFirstFramePending()){originalMain.requestQuit();reportUiState("quit");break;}
   if(input.quitRequested)closeAfterGarageFirstFrame=true;
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
   platform::OriginalUiControllerNavigation50LA::Events menuEvents,secondMenuEvents;
   require50DU(originalUiControllerNavigation.observe(input,active,menuEvents),
       "original frontend physical controller snapshot invalid");
   // Separate physical identity and release-to-arm lifetime for port two.
   // An inactive Controls2 route disarms it; keyboard always remains port zero.
   const auto secondInputRoute=originalMain.coordinator.route();
   require50DU(originalUiSecondControllerNavigation.observe(input,active&&
       secondInputRoute.screen==12&&secondInputRoute.page==3,secondMenuEvents,1),
       "original frontend second physical controller snapshot invalid");
   const auto mainClock=originalMain.sampleClock(applicationClock,active);
   originalMain.audioOutput.pause(!active);
   if(!active){
    originalMain.lastUpdateNanoseconds=0;captureCommitted();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));continue;
   }
   const auto route=originalMain.coordinator.route();
   const bool gameOptions=route.screen==12&&route.page==4;
   const bool memoryCard=route.screen==12&&route.page==5;
   const bool controls=route.screen==12&&(route.page==2||route.page==3);
   const bool rider=route.screen==5&&route.page==1;
   const bool course=route.screen==6&&route.page==1;
   const bool hall=route.screen==11&&route.page==1;
   const bool tournament=route.screen==9&&route.page==1;
   const bool garage=route.screen==13&&route.page==1;
   if(physicalInput){
   if(gameOptions||controls||memoryCard||rider||course||hall||tournament||garage){
    menuEvents[0]=menuEvents[0]||input.pressed[80];
    menuEvents[1]=menuEvents[1]||input.pressed[79];
    menuEvents[2]=menuEvents[2]||input.pressed[82]||input.pressed[26];
    menuEvents[3]=menuEvents[3]||input.pressed[81]||input.pressed[22];
   }else{
    menuEvents[2]=menuEvents[2]||input.pressed[82]||input.pressed[80]||input.pressed[26];
    menuEvents[3]=menuEvents[3]||input.pressed[81]||input.pressed[79]||input.pressed[22];
   }
   // Preserve the actual release-armed pad edge before merging keyboard input.
   // Keyboard wins simultaneous confirms, matching its explicit user action.
   const bool courseConfirmFromController=menuEvents[4]&&
       !(input.pressed[40]||input.pressed[88]||input.pressed[44]);
   menuEvents[4]=menuEvents[4]||input.pressed[40]||input.pressed[88]||input.pressed[44];
   menuEvents[5]=menuEvents[5]||((gameOptions||controls||memoryCard||rider||tournament)&&input.pressed[21]);
   menuEvents[7]=menuEvents[7]||((controls||memoryCard)&&input.pressed[43]);
   menuEvents[6]=menuEvents[6]||input.pressed[41]||hostInput.menuRequested;
   // Same source-ID merge/order as the existing race-hosted frontend. One
   // actual keyboard/pad confirm edge dispatches once even if both are pressed.
   for(const unsigned event:{0u,1u,2u,3u,4u,5u,7u,6u})
    if(menuEvents[event]){
     const auto accepted=originalMain.input(static_cast<std::uint8_t>(event),input.secondControllerPresence);
     if(course&&event==4&&accepted==platform::OriginalUiFrontendStatus50KG::routeAccepted)
      originalMainApplication.courseHumanController=courseConfirmFromController;
    }
   // Never merge two physical ports or retag keyboard input as controller one.
   if(secondInputRoute.screen==12&&secondInputRoute.page==3&&
      input.secondControllerPresence==platform::WindowControllerPresence50DQ::Present)
    for(const unsigned event:{0u,1u,2u,3u,4u,5u,7u,6u}){
     const auto currentRoute=originalMain.coordinator.route();
     if(currentRoute.screen!=12||currentRoute.page!=3)break;
     if(secondMenuEvents[event])originalMain.input(static_cast<std::uint8_t>(event),input.secondControllerPresence,1);
    }
   }
   if(tournamentRoute.enabled()){
    require50DU(originalMain.inputCount==tournamentRoute.issuedActions(),"physical menu input interrupted unattended tournament route");
    const auto snapshot=originalMain.coordinator.snapshot();
    const auto* navigation=originalMain.navigation();
    const auto group=navigation?navigation->controllerGroup[0]:0;
    const auto slot=navigation&&group>0&&static_cast<std::size_t>(group)<navigation->groups.size()?
        navigation->groups[group].selectedSlot:0;
    const auto action=tournamentRoute.poll(snapshot,group,slot,
        courseCatalogPrepared&&raceRequested&&!courseColdLoad.pending()&&renderer.initialized());
    if(action>=0){originalMain.input(static_cast<std::uint8_t>(action));tournamentRoute.validateDispatch(originalMain.coordinator.snapshot());}
   }
   if(tournamentReturnRoute.enabled()){
    require50DU(originalMain.inputCount==tournamentReturnRoute.issuedActions(),"physical menu input interrupted unattended tournament return route");
    const auto action=tournamentReturnRoute.poll(originalMain.coordinator.snapshot(),
        courseCatalogPrepared&&raceRequested&&!courseColdLoad.pending()&&renderer.initialized());
    if(action>=0){
     std::cerr<<"tournament_return_route action="<<action<<" screen="<<unsigned(originalMain.coordinator.snapshot().route.screen)<<'\n';
     const auto status=originalMain.input(static_cast<std::uint8_t>(action));
     const bool ending=status==platform::OriginalUiFrontendStatus50KG::unavailableDestination&&
         originalMain.diagnostic.sourcePC==0x1E40A0&&originalMain.diagnostic.hall.sourcePC==0x1E40A0;
     tournamentReturnRoute.validateDispatch(originalMain.coordinator.snapshot());
     if(ending){tournamentReturnRoute.reachEnding(originalMain.coordinator.snapshot());
      originalMain.requestQuit();reportUiState("tournament_ending");break;}}
   }
   if(hallRoute.enabled()){
    require50DU(originalMain.inputCount==hallRoute.issuedActions(),"physical menu input interrupted unattended Hall route");
    const auto snapshot=originalMain.coordinator.snapshot();
    const auto* navigation=originalMain.navigation();
    const auto group=navigation?navigation->controllerGroup[0]:0;
    const auto slot=navigation&&group>0&&static_cast<std::size_t>(group)<navigation->groups.size()?
        navigation->groups[group].selectedSlot:0;
    const auto action=hallRoute.poll(snapshot,group,slot,windowCapture.captured(windows_capture50LD::Stage::hall));
    if(action>=0){originalMain.input(static_cast<std::uint8_t>(action));
     const auto* after=originalMain.navigation();const auto afterGroup=after?after->controllerGroup[0]:0;
     hallRoute.validateDispatch(originalMain.coordinator.snapshot(),afterGroup,
         after&&afterGroup>0&&static_cast<std::size_t>(afterGroup)<after->groups.size()?after->groups[afterGroup].selectedSlot:0);}
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
    const auto action=captureRoute.poll(snapshot,group,slot,
        windowCapture.captured(windows_capture50LD::Stage::main),windowCapture.captured(windows_capture50LD::Stage::rider),
        windowCapture.captured(windows_capture50LD::Stage::course),sourceAccepted,
        // This only permits the harness to press Confirm. The normal input
        // callback builds the actual selection from the retained profile and
        // calls courseRaceDestination above; validateDispatch then requires
        // its accepted result. Do not duplicate the obsolete all-AI/corpus
        // restriction here: rooted direct-human tracks need neither.
        courseCatalogPrepared&&raceRequested&&!courseColdLoad.pending()&&
        renderer.initialized(),
        originalMain.sourceFrameGeneration,originalMain.submittedPublication);
    if(action>=0){originalMain.input(static_cast<std::uint8_t>(action));captureRoute.validateDispatch(originalMain.coordinator.snapshot());}
   }
   // Unattended EXIT GAME test (see RR_TEST_MAIN_EXIT above): one menu input at most every
   // 20 native frames, only on a settled screen, never while another unattended route runs.
   if(mainExitTest&&!captureRoute.enabled()&&!tournamentRoute.enabled()&&!tournamentReturnRoute.enabled()&&
      !hallRoute.enabled()&&originalMain.frameCount>=mainExitLastActionFrame+20){
    const auto snapshot=originalMain.coordinator.snapshot();
    const auto& t=snapshot.transition;
    const bool idle=snapshot.prepared&&snapshot.active&&!snapshot.faulted&&!snapshot.quitRequested&&snapshot.frameSubmitted&&
        !snapshot.deferredQueue&&t.bound&&!t.initialRestartPending&&!t.applicationRequestQueued&&
        t.lifecycle==rr::game::OriginalUiTransitionLifecycle50KG::idle&&t.phase==rr::game::OriginalUiTransitionPhase50KG::none&&
        snapshot.route.page==1&&snapshot.route.screen==t.current.screen&&snapshot.route.page==t.current.page;
    // Win7 / Unlock10 have no shared settled predicate; same open-page rule as the others.
    const bool screenSettled=idle&&(snapshot.winBound||snapshot.unlockBound)&&snapshot.screenPageShown&&
        snapshot.screenInputShown&&snapshot.screenFramePublished&&snapshot.screenAperture.bound&&
        snapshot.screenAperture.inputEnabled268&&snapshot.screenAperture.state==rr::game::OriginalUiApertureState50KG::open;
    const bool settled=original_ui_capture_navigation50LA::Route::settled(snapshot)||
        original_ui_capture_navigation50LA::HallRoute::settled(snapshot)||
        ((snapshot.route.screen==7||snapshot.route.screen==10)&&screenSettled);
    const auto exitStatus=rr::platform::originalUiMainExitGameStatus50XG();
    int action=-1;const char* step=nullptr;
    if(settled&&snapshot.route.screen!=4){
     // What the ORIGINAL screen itself accepts on the way back to Main:
     //  Rider5 / Course6 / Options12: Back (6).
     //  Hall11: its prompt says which. mode5 (Options Hall, 42C bit1): Back -> Options.
     //    A race-return Hall (mode0/2, 42C bit0, 1E48E0) has NO Back (1E3F70 takes 6 only
     //    in mode5); Cross (4) continues: Free Play -> Course6 (1E41C4), tournament ->
     //    Win7/Unlock10/Garage13 (1E3FC0 table).
     //  Win7 (2089D0) and Unlock10 (20B48C): Cross (4) only -> Unlock10/Garage13/Course6.
     const auto screen=snapshot.route.screen;
     if(mainExitReturn&&(screen==5||screen==6||screen==12)){action=6;step="back";}
     else if(mainExitReturn&&screen==11&&(snapshot.hall.promptMask42C&1u)){action=4;step="hall_confirm";}
     else if(mainExitReturn&&screen==11&&(snapshot.hall.promptMask42C&2u)){action=6;step="back";}
     else if(mainExitReturn&&(screen==7||screen==10)){action=4;step="confirm";}
     else if(!mainExitNoStepLogged){
      // Garage13 / Tournament9 are not walked by this driver (tournament returns).
      std::cerr<<"test_main_exit no_step screen="<<unsigned(screen)<<'\n';mainExitNoStepLogged=true;}
    }else if(settled&&!exitStatus.rowAvailable){
     if(!mainExitUnavailableLogged){std::cerr<<"test_main_exit row_unavailable\n";mainExitUnavailableLogged=true;}
    }else if(settled&&exitStatus.dialogOpen){
     if(exitStatus.dialogAwaitingInput&&!mainExitConfirmSent){action=4;step="dialog_confirm";mainExitConfirmSent=true;}
    }else if(settled&&exitStatus.rowSelected){
     if(!mainExitOpenSent){action=4;step="row_confirm";mainExitOpenSent=true;}
    }else if(settled&&mainExitActions<16){action=2;step="previous";++mainExitActions;} // walks up and wraps into EXIT GAME
    if(action>=0){
     mainExitLastActionFrame=originalMain.frameCount;
     const auto status=originalMain.input(static_cast<std::uint8_t>(action));
     std::cerr<<"test_main_exit step="<<step<<" event="<<action<<" screen="<<unsigned(snapshot.route.screen)
         <<" status="<<static_cast<unsigned>(status)<<'\n';
    }
   }
   const auto elapsedNanoseconds=originalMain.acceptedElapsed(monotonicNow50DU());
   platform::OriginalUiMainSessionUpdate50KG update;
   update.elapsedSeconds=originalMain.preparedSourceUpdateSeconds();
   if(physicalInput){
    update.courseHorizontalHeld=originalUiControllerNavigation.courseHorizontalHeld();
   } // Actual same-poll levels only; unattended route actions do not hold keys.
   const auto acceptedSource=originalMain.update(update,elapsedNanoseconds);
   // Garage Root already committed its exact source timing; PCM was still advanced once.
   if(!acceptedSource.sourceTimingCommitted)originalMain.commitSourceUpdateTiming();
   originalMain.render(mainClock,renderer,info.drawableWidth,info.drawableHeight);
#if defined(_WIN32)
   // Change-only human navigation diagnostics, not a per-frame timing trace.
   {
    const auto observed=originalMain.coordinator.snapshotWithoutGarageRuntime();
    const std::array<std::int64_t,6> selection{{observed.route.screen,observed.route.page,
        observed.rider.rider4C4,observed.rider.slot4C8,
        observed.course.selectedCourse44,observed.course.selectedPlanet40}};
    if(selection!=loggedSelection){
     std::cerr<<"windows_menu_selection screen="<<selection[0]<<" page="<<selection[1]
         <<" rider="<<selection[2]<<" slot="<<selection[3]<<" course="<<selection[4]
         <<" planet="<<selection[5]<<" source_frame="<<originalMain.frameCount<<'\n';
     loggedSelection=selection;
    }
   }
#endif
   if(!originalMain.coordinator.snapshotWithoutGarageRuntime().rider.deferred4EC){
    const auto snapshot=originalMain.coordinator.snapshotWithoutGarageRuntime();
    const auto stage=snapshot.route.screen==4?windows_capture50LD::Stage::main:
        snapshot.route.screen==5?windows_capture50LD::Stage::rider:
        snapshot.route.screen==6?windows_capture50LD::Stage::course:
        ((hallRoute.enabled()||raceReturnHallCapture)&&snapshot.route.screen==11)?windows_capture50LD::Stage::hall:windows_capture50LD::Stage::other;
    const bool sourceAccepted=originalMain.submittedScreen==5&&originalMain.sourceFrameGeneration&&
        originalMain.submittedTriangles&&originalMain.coordinator.gpu()&&
        originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
    const bool settled=(stage==windows_capture50LD::Stage::hall?original_ui_capture_navigation50LA::HallRoute::settled(snapshot):
        original_ui_capture_navigation50LA::Route::settled(snapshot))&&
        (stage!=windows_capture50LD::Stage::rider||sourceAccepted);
    const bool captureWindowImage=host.hasWindow()&&captureRoute.captureAllowed(snapshot,sourceAccepted,originalMain.sourceFrameGeneration,
        originalMain.submittedPublication)&&(!hallRoute.enabled()||hallRoute.captureAllowed(snapshot))&&windowCapture.due(stage,settled);
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
   const auto nextRoute=originalMain.coordinator.route();
   if(!courseCatalogPrepared&&nextRoute.screen==6&&nextRoute.page==1){
    // Cold readiness work is outside noexcept source input callbacks. Missing
    // resources disable race Confirm; malformed existing roster remains fatal.
    const auto begin=monotonicNow50DU();
    courseTargets.prepare(trackAdmission);courseCatalogPrepared=true;
    courseCorpusPresent=corpusPath&&std::filesystem::is_regular_file(std::filesystem::path(corpusPath));
    if(!courseCorpusPresent)std::cerr<<"Legacy Course corpus absent; admitted direct-disc tracks use original native resources.\n";
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
    const auto* target=courseTargets.admits(selected);
    // RA leaves Course through the same typed cold-load exit as every other
    // course (the original 1465C0 Course exit reads no track id). The former
    // RA-only loading prefix (advanceCourseRaLoadingPrefix50LA) always stopped
    // at 1465D0 and nothing consumed it, so it is not called here.
    require50DU(target&&(target->track==original_course_targets50KX::Track::RA||
       directHumanTargetSupported(target->track)||courseCorpusPresent)&&
        courseTargetModeSupported(target->track)&&(!allAiEnabled||selected.laps==3)&&
        courseColdLoad.publish(courseTargets,selected,sessionIndex+1),
        "actual Course exit could not publish its typed cold-load request");
    std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<original_course_targets50KX::targetCode(*target)
        <<" rider="<<selected.rider<<" outgoing_world_ticks=0 frontend_only=1\n";
    break;
   }
   // Host idle between frontend polls (swap interval 0). Every other screen
   // keeps its fixed 8 ms rest. The Garage only rests for whatever part of
   // the same 8 ms its own frame has not already spent, so a slow Garage
   // frame is not lengthened by a full extra rest. Source timing is measured.
   if(nextRoute.screen==13&&nextRoute.page==1){
    const auto spent=std::chrono::steady_clock::now()-frontendIterationStart;
    if(spent<std::chrono::milliseconds(8))std::this_thread::sleep_for(std::chrono::milliseconds(8)-spent);
   }else std::this_thread::sleep_for(std::chrono::milliseconds(8));
  }
  require50DU(terminalInputError==0,"original frontend input stream failed");
 }catch(...){
  const auto originalFailure=std::current_exception();
  // Preserve the original failure before any strict retained-owner cleanup
  // can terminate. This observation does not relax or skip that cleanup.
  try{std::rethrow_exception(originalFailure);}
  catch(const PlayerLoadingCancelled50DU&){std::cerr<<"original frontend loading cancelled by window Close"<<std::endl;}
  catch(const std::exception& e){std::cerr<<"original frontend failure before cleanup: "<<e.what()<<std::endl;}
  catch(...){std::cerr<<"original frontend failure before cleanup: non-standard exception"<<std::endl;}
  if(!closeWindow()&&!closeWindow()){
   std::cerr<<"original frontend cleanup unresolved; retaining context and owners by terminating\n";std::terminate();
  }
  // Actual output close has stopped/drained its native voices. The enclosing
  // terminal boundary retires Root services only after these session locals
  // (including the real Display) unwind, before Application destruction.
  try{std::rethrow_exception(originalFailure);}
  catch(const PlayerLoadingCancelled50DU&){return SessionExit50DU::Quit;}
 }
 bool closed=closeWindow();
 if(!closed&&!(closed=closeWindow())){
  std::cerr<<"original frontend cleanup unresolved; retaining context and owners by terminating\n";std::terminate();
 }
 require50DU(closed,"original frontend cleanup reported a failure");
 const auto audio=originalMain.audioOutput.info();
 J receipt={{"native_frames",originalMain.frameCount},{"captures",originalMain.captureSequence},
     {"world_ticks",0},{"window_swaps",windowSwaps},{"cleanup_complete",true},{"frontend_only",true},
     {"audio_mixed_frames",audio.mixedFrames},{"audio_source_mixed_frames",originalMain.audioOutput.sourceMixedFrames()},{"audio_source_frame_origin",originalMain.audioOutput.sourceFrameOrigin()},
            {"audio_output_source_mixed_frames",originalMain.audioOutput.outputSourceMixedFrames()},
     {"audio_output_serial",originalMain.audioOutput.retainedOutputSerial()},
     {"audio_captured_frames",audio.capturedFrames},{"audio_rate",audio.sampleRate},
     {"audio_accepted_nanoseconds",originalMain.audioOutput.acceptedNanoseconds()}};
 std::cout<<"original_main_closed "<<receipt.dump()<<'\n';
 return courseColdLoad.pending()?SessionExit50DU::CourseColdLoad:SessionExit50DU::Quit;
}
