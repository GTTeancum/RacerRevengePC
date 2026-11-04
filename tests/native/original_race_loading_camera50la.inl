// Included only by original_race_camera_owner50la.cpp. Same counted Race34,
// same retained loadDisplay_; no gameplay Session, Follow, Bank or World images.
namespace original_main_entry50KG {
bool Application::initializeConsumedRaLoadingCamera2361C0(
    rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& pc)const noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    using Step=rr::game::OriginalRaceConstructor50LA::CameraInitStep;
    auto& n=race.loadingCamera34_;pc=n.stopPC;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||race.resultsFeedbackThread_!=std::this_thread::get_id()||
       sourceRace28COwner_.get()!=&race||raceConstructorColdOwner50LA_.lock().get()!=&race||
       race.application_!=entitiesOwner_||race.profile_!=riderProfile||!race.counted_||race.rolledBack_||
       !race.world74_||!race.nativeIntroWorldEntity()||
       race.worldInitStep_!=rr::game::OriginalRaceConstructor50LA::WorldInitStep::camera||
       race.cameraInitStep_!=Step::nativePending||race.cameraNativeAdapter34_||!race.camera34_||
       n.entered||n.terminal||std::fegetround()!=FE_TONEAREST)return false;
    auto& camera=*race.camera34_;auto& f=camera.fields_;
    const auto display=race.loadDisplay_;
    rr::platform::VehicleWindowDiagnostic50DS diagnostic;
    // Native admission, before new source stores. Ordinary gameplay create()
    // deliberately has unknown source0/0C/4/8 and cannot satisfy this contract.
    // frontendOnly is NOT waived: genuine backend World image publication is a
    // separate future prerequisite, not granted by this source initializer.
    if(!display||raceDisplayOwner50LA_.lock()!=display||display->application_!=this||display->video_!=videoOwner_||
       !display->frontendOwnerCurrent50KG()||!display->canRetain()||!display->frontendCreated_||
       !display->sourceBufferCount0CInitialized_||display->sourceBufferCount0C_!=2u||
       !display->sourceCurrentCamera0Known_||display->sourceWidth4_!=640||display->sourceHeight8_!=448||
       display->selectedGameplayCamera0_||display->localizationServiceActive_||
       display->textureLifetime_->nativeUploads_||display->dispatch_.pendingCallbackDC||
       display->dispatch_.count54||display->dispatch_.count60||
       !display->renderer_.frontendCameraViewportIdle50KG(diagnostic)||
       camera.phase_!=Camera::Phase::beforeViewport114A00||
       !camera.unpublishedAbortCurrent(this,entitiesOwner_))return false;
    const auto prior=display->loadingPriorViewportCamera50LA();
    if(!display->frontendPriorViewportCurrent50LA(prior))return false;
    const auto fp=[](std::uint32_t x)noexcept{return Camera::floatFromBits(x);};
    const auto words=[](const std::array<float,16>& input)noexcept {
        Camera::Words16 output{};for(std::size_t i=0;i<16;++i)output[i]=Camera::floatBits(input[i]);return output;};
    const auto vector=[&](const Camera::Words4& v)noexcept {
        return rr::assets::TVCameraVector49E{fp(v[0]),fp(v[1]),fp(v[2]),fp(v[3])};};
    rr::game::CameraSceneState49F scene;
    scene.look=vector(f.look190);scene.up=vector(f.up1A0);scene.fovDegrees=fp(f.fov1D0);
    scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
    scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
    // Pure native arithmetic admission precedes every new source effect.
    // These fixed inputs are already produced by the counted constructor and
    // scalar prefix. Empty dispatch and same-thread custody exclude a callback
    // changing them. This calculation is NOT a source call/return receipt.
    rr::game::CameraViewOutput49F admittedCore;float admittedFocal=0,admittedAngle=0;
    const auto admittedRebuild=rr::game::rebuildOriginalSceneCore115350(scene,vector(f.position180),
        static_cast<std::int32_t>(f.viewport1E0[2]),admittedCore,admittedFocal);
    if(admittedRebuild.status!=rr::game::OriginalCameraStartStatus::complete)return false;
    if(rr::game::computeOriginalGuardAngle2371D0(admittedFocal,admittedAngle).status!=
        rr::game::OriginalCameraStartStatus::complete)return false;
    rr::game::OriginalCameraViewportFields numeric;
    auto admittedSuffixScene=scene;
    if(rr::game::prepareOriginalRaceViewport2361C0(numeric,camera.generation_,camera.key_,admittedSuffixScene).status!=
        rr::game::OriginalCameraStartStatus::complete)return false;
    display->pin();n.pinned=true;n.prior=prior;n.entered=true;
    const auto fail=[&](std::uint32_t at)noexcept {n.terminal=true;n.stopPC=pc=at;return false;};
    f.count210=display->sourceBufferCount0C_+1u; //114A24, actual retained Display0C.
    f.count20C=f.count210;                     //114A28.
    camera.phase_=Camera::Phase::insideViewport114A00;
    std::int32_t returned=0;
    const auto service=rr::game::serviceOriginalRenderer244D60(display->dispatch_,true,
        display->renderer_,diagnostic,returned,rr::game::OriginalCameraDrainPolicy::initializedContext);
    if(service.status!=rr::game::OriginalCameraStartStatus::complete)return fail(service.sourcePC);
    n.drains=1; // D8-disabled source branch still reaches the independent245910.
    if(!display->renderer_.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)return fail(0x245910);
    n.drains=2;
    if(!display->applyLoadingPriorViewportZoom2458C0(camera,n.prior))return fail(0x245918);
    camera.phase_=Camera::Phase::afterViewport114A00;
    while(n.callbacks<2){
        //115350 core stores precede2371D0 guard stores. Native arithmetic
        // refusal was admitted before entering this source interval, not here.
        f.viewC0=words(admittedCore.view);f.projection80=words(admittedCore.projection);
        f.combined100=words(admittedCore.combined);
        f.focal1D8=Camera::floatBits(admittedFocal);f.coreKnown=true;
        // Both236DD0 invocations precede source318..324. Retain unknown
        // dependent caches instead of reading native zero backing as source.
        f.screen140Known=false;f.combinedScreen250Known=false;
        f.guardProjection290Known=false;f.guardCombined2D0Known=false;
        f.guardAngle330=Camera::floatBits(admittedAngle);++n.callbacks;
    }
    {
        rr::platform::OriginalOptionsMutationInterlock50KG::Operation operation;
        if(!operation.admitted()||!(camera.entity_.flags29&1u)||
           !display->frontendPriorViewportCurrent50LA(n.prior))return fail(0x11473c);
        //26F948 publication precedes26F978 dimension comparison.
        display->selectFrontendCamera26F940(race.camera34_);n.selected=true;
        camera.phase_=Camera::Phase::selectedBeforeBounds236214;
        if(display->sourceWidth4_!=static_cast<std::int32_t>(f.fullWidth204)||
           display->sourceHeight8_!=static_cast<std::int32_t>(f.fullHeight208))return fail(0x26f978);
    }
    f.depth310=Camera::floatBits(numeric.depth310);f.depth314=Camera::floatBits(numeric.depth314);
    f.left318=Camera::floatBits(numeric.left318);f.right31C=Camera::floatBits(numeric.right31C);
    f.top320=Camera::floatBits(numeric.top320);f.bottom324=Camera::floatBits(numeric.bottom324);
    f.aspect1F8=Camera::floatBits(admittedSuffixScene.aspect1F8);f.matrix140=words(numeric.screen140);f.screen140Known=true;
    f.combinedScreen250=words(numeric.combinedScreen250);f.combinedScreen250Known=true;
    f.guardCombined2D0=words(numeric.guardCombined2D0);f.guardCombined2D0Known=true;
    f.guardX334=Camera::floatBits(numeric.guardX334);f.guardY338=Camera::floatBits(numeric.guardY338);
    camera.phase_=Camera::Phase::viewportInitialized2361C0;
    n.returned=true;n.stopPC=pc=0x1a93b8;
    // Only initializer return. Caller result test/later1A9240, actual World
    // metadata/GPU preparation and all Session/Follow consumers stay unjoined.
    return true;
}

bool Application::abortConsumedRaLoadingCameraNative34(
    rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& pc)const noexcept {
    auto& n=race.loadingCamera34_;pc=0;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||race.resultsFeedbackThread_!=std::this_thread::get_id()||
       raceConstructorColdOwner50LA_.lock().get()!=&race||race.application_!=entitiesOwner_||
       race.world74_||race.cameraNativeAdapter34_||!n.entered)return false;
    const auto display=race.loadDisplay_;
    if(!display||display->application_!=this||display->video_!=videoOwner_||!display->ownerThread())return false;
    if(n.pinned&&!display->localizationServiceIdentityCurrent50KG())return false;
    if(display->selectedGameplayCamera0_)return false;
    rr::platform::OriginalOptionsMutationInterlock50KG::Operation mutation;
    rr::platform::VehicleWindowDiagnostic50DS diagnostic;
    if(!mutation.admitted()||display->textureLifetime_->nativeUploads_||
       display->localizationServiceActive_||display->localizationServiceFaulted_||
       display->dispatch_.pendingCallbackDC||display->dispatch_.count54||display->dispatch_.count60||
       !display->renderer_.frontendCameraViewportIdle50KG(diagnostic))return false;
    if(!n.cameraDisposed){
        const auto expected=1+(n.selected&&!n.selectionWithdrawn?1:0);
        if(race.camera34_.use_count()!=expected)return false;
        if(!race.camera34_||!race.camera34_->unpublishedAbortCurrent(this,entitiesOwner_,true))return false;
        if(n.selected&&!n.selectionWithdrawn&&(!display->sourceCurrentCamera0Known_||
            display->sourceSelectedCamera0_!=race.camera34_))return false;
        if(n.selectionWithdrawn&&(display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_))return false;
        if(!n.selected&&display->sourceSelectedCamera0_==race.camera34_)return false;
        std::array<rr::game::OriginalWreckComponent50LA*,4> cameraSlots;
        if(!n.arrayPublished){
            // A different real array writer may have acquired a raw borrow
            // after initializer return without changing shared_ptr use_count.
            // Never erase that owner or retire its referenced Entity.
            if(!entitiesOwner_->snapshotCameras140200(cameraSlots))return false;
            for(const auto* slot:cameraSlots)if(slot==&race.camera34_->entity_)return false;
        }
        if(n.arrayPublished&&!n.arrayWithdrawn){
            const std::array<rr::game::OriginalWreckComponent50LA*,4> expectedSlots{{&race.camera34_->entity_,nullptr,nullptr,nullptr}};
            if(!entitiesOwner_->snapshotCameras140200(cameraSlots)||cameraSlots!=expectedSlots||
               entitiesOwner_->cameraCount264_!=1u)return false;
        }
        if(n.arrayWithdrawn&&(entitiesOwner_->cameraSlots254Known_||
            entitiesOwner_->cameraSlots254_!=std::array<rr::game::OriginalWreckComponent50LA*,4>{}))return false;
        // Preflight every release gate before withdrawing the exact selection.
        // This is native cancellation, not a retail1155C0/26F940(NULL) receipt.
        if(n.arrayPublished&&!n.arrayWithdrawn){
            // Native raw-borrow withdrawal only. Historical count264 remains;
            // unknown source-array knowledge forbids claiming1401B0(NULL) or
            // restoring the observed priorNULL tuple as a source operation.
            entitiesOwner_->cameraSlots254_.fill(nullptr);
            entitiesOwner_->cameraSlots254Known_=false;n.arrayWithdrawn=true;
        }
        if(n.selected&&!n.selectionWithdrawn){
            display->sourceSelectedCamera0_.reset();display->sourceCurrentCamera0Known_=false;
            display->invalidateGameplayCameraObservation50LA();n.selectionWithdrawn=true;
        }
        n.terminal=true;
        const auto retired=race.camera34_->abortUnpublished236180(this,entitiesOwner_,true);
        if(!retired.okay()){pc=retired.sourcePC;return false;}
        n.cameraDisposed=true;race.camera34_.reset();
    }
    n.prior.reset();
    if(n.pinned){display->unpin();n.pinned=false;}
    race.cameraInitStep_=rr::game::OriginalRaceConstructor50LA::CameraInitStep::retired;
    return true;
}
} // namespace original_main_entry50KG

namespace rr::game {
bool OriginalRaceConstructor50LA::loadingCameraSourceNullSuffixCurrent()const noexcept {
    // The same counted World actually executed2705BC. Absence in native
    // storage alone would not prove the original1A97B8 branch.
    return resultsFeedbackThread_==std::this_thread::get_id()&&nativeIntroWorldEntity()&&
        worldInitStep_==WorldInitStep::camera&&world74_&&!world74_->terminal_&&!world74_->closed_&&
        world74_->worldColdConstructed_&&world74_->worldReset270550Complete_&&
        !world74_->nativeFog3EC_&&!nativeTaController48_&&
        fields48_==std::array<std::uint32_t,4>{};
}
}
namespace original_main_entry50KG {
bool Application::finishConsumedRaLoadingCamera1A9750(
    rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& pc)const noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    auto& n=race.loadingCamera34_;pc=n.stopPC;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||race.resultsFeedbackThread_!=std::this_thread::get_id()||
       sourceRace28COwner_.get()!=&race||raceConstructorColdOwner50LA_.lock().get()!=&race||
       race.application_!=entitiesOwner_||race.profile_!=riderProfile||!race.counted_||race.rolledBack_||
       !n.entered||!n.returned||n.terminal||n.suffixEntered||!n.pinned||!n.selected||n.selectionWithdrawn||
       n.stopPC!=0x1a93b8||race.cameraNativeAdapter34_||!race.camera34_||
       race.cameraInitStep_!=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativePending||
       (race.flags1B0KnownMask_&3u)!=3u||(race.flags1B0KnownValue_&3u)!=1u||
       !race.loadingCameraSourceNullSuffixCurrent()||std::fegetround()!=FE_TONEAREST)return false;
    auto& camera=*race.camera34_;auto& f=camera.fields_;
    const auto display=race.loadDisplay_;
    rr::platform::VehicleWindowDiagnostic50DS diagnostic;
    if(!display||raceDisplayOwner50LA_.lock()!=display||display->application_!=this||display->video_!=videoOwner_||
       !display->frontendOwnerCurrent50KG()||!display->localizationServiceIdentityCurrent50KG()||
       !display->sourceBufferCount0CInitialized_||display->sourceBufferCount0C_!=2u||
       !display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_!=race.camera34_||
       display->sourceWidth4_!=640||display->sourceHeight8_!=448||display->selectedGameplayCamera0_||
       display->textureLifetime_->nativeUploads_||display->localizationServiceActive_||display->localizationServiceFaulted_||
       display->dispatch_.pendingCallbackDC||display->dispatch_.count54||display->dispatch_.count60||
       !display->renderer_.frontendCameraViewportIdle50KG(diagnostic)||
       camera.phase_!=Camera::Phase::viewportInitialized2361C0||
       !camera.unpublishedAbortCurrent(this,entitiesOwner_,true)||race.camera34_.use_count()!=2)return false;
    // Exact actual App camera array admission BEFORE any suffix effects. A
    // foreign Camera consumer cannot be replaced by this narrow fresh route.
    std::array<rr::game::OriginalWreckComponent50LA*,4> priorSlots;
    if(!entitiesOwner_->snapshotCameras140200(priorSlots)||
       priorSlots!=std::array<rr::game::OriginalWreckComponent50LA*,4>{}||
       entitiesOwner_->cameraCount264_>1u)return false;
    const auto fp=[](std::uint32_t word)noexcept{return Camera::floatFromBits(word);};
    const auto words=[](const std::array<float,16>& input)noexcept {
        Camera::Words16 out{};for(std::size_t i=0;i<16;++i)out[i]=Camera::floatBits(input[i]);return out;};
    const auto floats=[&](const Camera::Words16& input)noexcept {
        std::array<float,16> out{};for(std::size_t i=0;i<16;++i)out[i]=fp(input[i]);return out;};
    const auto vec=[&](const Camera::Words4& v)noexcept {
        return rr::assets::TVCameraVector49E{fp(v[0]),fp(v[1]),fp(v[2]),fp(v[3])};};
    rr::game::CameraSceneState49F scene;
    scene.look=vec(f.look190);scene.up=vec(f.up1A0);scene.fovDegrees=fp(f.fov1D0);
    scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
    scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
    rr::game::OriginalCameraViewportFields numeric;
    numeric.sessionGeneration=camera.generation_;numeric.sceneKey=camera.key_;numeric.prepared=true;
    numeric.fullWidth204=numeric.width1E8=640;numeric.fullHeight208=numeric.height1EC=448;
    numeric.aspect1DC=fp(f.aspect1DC);numeric.depth310=fp(f.depth310);numeric.depth314=fp(f.depth314);
    numeric.left318=fp(f.left318);numeric.right31C=fp(f.right31C);
    numeric.top320=fp(f.top320);numeric.bottom324=fp(f.bottom324);
    numeric.zoom328=fp(f.zoom328);numeric.zoom32C=fp(f.zoom32C);
    numeric.screen140=floats(f.matrix140);
    // Native arithmetic admission only; no scene-tick owner or source callback
    // is manufactured.1158A0 repeats the already reached full640/448 expression
    // and therefore the same exact aspect3F400000, with no changed operands.
    if(f.aspect1F8!=0x3f400000u||!f.screen140Known||!f.coreKnown)return false;
    if(rr::game::updateOriginalViewportZoom2458C0(numeric,numeric).status!=rr::game::OriginalCameraStartStatus::complete)return false;
    rr::game::CameraViewOutput49F matrices;
    if(rr::game::rebuildOriginalRaceScene236DD0(numeric,scene,vec(f.position180),matrices).status!=
        rr::game::OriginalCameraStartStatus::complete)return false;
    const auto fail=[&](std::uint32_t at)noexcept {n.terminal=true;n.stopPC=pc=at;return false;};
    n.suffixEntered=true;
    // Real flags1 single-view tests skip38/3C/40 constructors.1A9590 passes1.
    //1158A0 clamps to full dimensions, then stores in the exact original order.
    f.viewport1E0[0]=0;f.viewport1E0[1]=0;f.viewport1E0[2]=640;f.viewport1E0[3]=448;
    f.viewport1E0[4]=0;f.viewport1E0[5]=0x3f800000u;f.aspect1F8=0x3f400000u;
    f.count210=display->sourceBufferCount0C_+1u;f.count20C=f.count210;
    std::int32_t returned=0;
    const auto serviced=rr::game::serviceOriginalRenderer244D60(display->dispatch_,true,display->renderer_,
        diagnostic,returned,rr::game::OriginalCameraDrainPolicy::initializedContext);
    if(serviced.status!=rr::game::OriginalCameraStartStatus::complete)return fail(serviced.sourcePC);
    n.suffixDrains=1;
    if(!display->renderer_.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)return fail(0x245910);
    n.suffixDrains=2;
    if(!display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_!=race.camera34_)return fail(0x245918);
    f.zoom328=Camera::floatBits(numeric.zoom328);f.zoom32C=Camera::floatBits(numeric.zoom32C);
    // One236DD0 callback. Unlike the earlier initializer, all bounds now have
    // genuine producers; preserve core→screen scales→guard→combined order.
    f.viewC0=words(matrices.view);f.projection80=words(matrices.projection);f.combined100=words(matrices.combined);
    f.focal1D8=Camera::floatBits(numeric.focalPixels1D8);f.coreKnown=true;
    f.matrix140[0]=Camera::floatBits(numeric.screen140[0]);f.matrix140[5]=Camera::floatBits(numeric.screen140[5]);
    f.guardAngle330=Camera::floatBits(numeric.guardAngle330);
    f.guardX334=Camera::floatBits(numeric.guardX334);f.guardY338=Camera::floatBits(numeric.guardY338);
    f.combinedScreen250=words(numeric.combinedScreen250);f.combinedScreen250Known=true;
    f.guardProjection290=words(numeric.guardProjection290);f.guardProjection290Known=true;
    f.guardCombined2D0=words(numeric.guardCombined2D0);f.guardCombined2D0Known=true;
    // App27C v44 is1401B0, not Display26F940 or a Session shadow array.
    std::array<rr::game::OriginalWreckComponent50LA*,4> currentSlots;
    if(!entitiesOwner_->snapshotCameras140200(currentSlots)||currentSlots!=priorSlots)return fail(0x1a97a0);
    const std::array<rr::game::OriginalWreckComponent50LA*,4> published{{&camera.entity_,nullptr,nullptr,nullptr}};
    if(!entitiesOwner_->rewriteCameras1401B0(published))return fail(0x1401b0);
    n.arrayPublished=true;
    //1A97B8 actual reset-known FogNULL; actual constructor/controllerNULL
    // branches omit13E920 and v20/v18. Revalidate after array publication.
    if(!race.loadingCameraSourceNullSuffixCurrent())return fail(0x1a97b8);
    n.suffixReturned=true;n.stopPC=pc=0x1a9598;
    // Consume the genuine single-view1A9240 epilogue, then enter the actual
    // outer music occurrence. Native Session completion cannot enter this path.
    // The source music body retains its reached prefix and returns false at
    // the next unimplemented boundary; it never authorizes the track segment.
    return rr::game::OriginalRaceMusicManager50LA::beginSourceAfterLoadingCamera1A6C90(
        *this,race,sourceAural00Owner_,pc);
}
}
