// Distinct real Course Page4BC owner. This private component is uncalled until
// the Page caller reaches1D6814; no CPU camera projection grants this custody.
    using CourseCamera=game::OriginalFrontendCameraCold50LA;
    struct CourseCameraDiagnostic {CourseCamera::Result camera{};VehicleWindowDiagnostic50DS renderer{};
        assets::OriginalUiPageDiagnostic50KG config{};std::uint32_t sourcePC=0;};
    std::shared_ptr<CourseCamera> coursePageCamera_,courseCameraPriorViewport_;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> courseCameraDisplay_;
    bool courseCameraDisplayPinned_=false,courseCameraAttempted_=false,courseCamera4BCPublished_=false;
    bool courseCameraCallbackReturned_=false,courseCameraSelected_=false,courseCameraSelectionWithdrawn_=false;
    bool courseCameraZoomApplied_=false,courseCameraNativeDisposed_=false,courseCameraNativeRetired_=false;
    std::int32_t courseCameraReturn_=0;
    std::uint8_t courseCameraDrainPhase_=0,courseCameraCallbacks_=0;
    std::array<float,3> courseCameraPositionStack70_{},courseCameraLookStack80_{{0,-1,0}},courseCameraUpStack90_{{0,0,1}};
    float courseCameraFovStackD8_=35,courseCameraRollStackDC_=0;
    bool courseCameraOwnerCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&issuer_&&scope_&&construction_&&construction_->pageAcquired_&&
            construction_->registrationCurrent()&&pagePayload_.root_==issuer_&&pagePayload_.pinned_&&
            pagePayload_.displayCurrent();
    }
    bool courseCameraStop(CourseCameraDiagnostic& d,std::uint32_t pc)noexcept {
        terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;
    }
    bool loadCoursePageCamera1D7A90(CourseCameraDiagnostic& d)noexcept try {
        d={};d.sourcePC=0x1D6814;
        if(terminal_||courseCameraAttempted_||loaderStopPC_!=0x1D6814||!courseCameraOwnerCurrent()||
           !coursePageConfig_||!coursePageConfig_->selectedCurrent("PageInfo")||std::fegetround()!=FE_TONEAREST)return false;
        const auto display=issuer_->raceDisplayOwner50LA_.lock();
        if(!display||display->application_!=issuer_||display->video_!=video_||!display->canRetain()||
           !display->frontendFirstViewportCurrent50KG(d.renderer)||display->sourceWidth4_!=640||display->sourceHeight8_!=448)return false;
        // Native release custody precedes actual camera allocation, never a
        // synthetic source malloc-success or callback-return receipt.
        courseCameraDisplay_=display;display->pin();courseCameraDisplayPinned_=true;courseCameraAttempted_=true;
        d.camera=CourseCamera::prepareStorage(issuer_,scope_->application_,coursePageCamera_);
        if(!d.camera.okay()){
            if(d.camera.status!=CourseCamera::Status::allocation)return courseCameraStop(d,d.camera.sourcePC);
            pagePayload_.shared_.camera4BC=nullptr;courseCamera4BCPublished_=true; //1D7B08.
            courseCameraReturn_=0;courseCameraCallbackReturned_=true;d.sourcePC=0x1D7C74;return true;
        }
        coursePageCamera_->courseNativeOwner_=this;
        d.camera=CourseCamera::adoptPrepared236180(issuer_,scope_->application_,coursePageCamera_);
        if(!d.camera.okay())return courseCameraStop(d,d.camera.sourcePC);
        pagePayload_.shared_.camera4BC=coursePageCamera_.get();courseCamera4BCPublished_=true;
        auto& camera=*coursePageCamera_;auto& f=camera.fields_;
        d.camera=issuer_->stageFrontendCameraViewportPrefix1149D4(camera);
        if(!d.camera.okay())return courseCameraStop(d,d.camera.sourcePC);
        if(!display->frontendFirstViewportCurrent50KG(d.renderer))return courseCameraStop(d,0x114A0C);
        courseCameraPriorViewport_=display->loadingPriorViewportCamera50LA();
        if(!display->frontendPriorViewportCurrent50LA(courseCameraPriorViewport_))return courseCameraStop(d,0x245918);
        const auto count=display->sourceBufferCount0C_+std::uint32_t{1};f.count210=count;f.count20C=count;
        camera.phase_=CourseCamera::Phase::insideViewport114A00;
        std::int32_t returned=0;
        const auto serviced=game::serviceOriginalRenderer244D60(display->dispatch_,true,display->renderer_,d.renderer,returned,
            game::OriginalCameraDrainPolicy::initializedContext);
        if(serviced.status!=game::OriginalCameraStartStatus::complete)return courseCameraStop(d,serviced.sourcePC);
        courseCameraDrainPhase_=1;
        if(!display->renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)return courseCameraStop(d,0x245910);
        courseCameraDrainPhase_=2;
        if(!display->applyLoadingPriorViewportZoom2458C0(camera,courseCameraPriorViewport_))return courseCameraStop(d,0x245918);
        courseCameraZoomApplied_=true;camera.phase_=CourseCamera::Phase::afterViewport114A00;
        const auto fp=[](std::uint32_t x)noexcept{return CourseCamera::floatFromBits(x);};
        const auto words=[](const std::array<float,16>& input)noexcept {CourseCamera::Words16 output{};
            for(std::size_t i=0;i<16;++i)output[i]=CourseCamera::floatBits(input[i]);return output;};
        const auto vector=[&](const CourseCamera::Words4& x)noexcept {return assets::TVCameraVector49E{fp(x[0]),fp(x[1]),fp(x[2]),fp(x[3])};};
        game::CameraSceneState49F scene;scene.look=vector(f.look190);scene.up=vector(f.up1A0);
        scene.fovDegrees=fp(f.fov1D0);scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
        scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
        while(courseCameraCallbacks_<2){
            game::CameraViewOutput49F core;float focal=0,angle=0;
            auto r=game::rebuildOriginalSceneCore115350(scene,vector(f.position180),static_cast<std::int32_t>(f.viewport1E0[2]),core,focal);
            if(r.status!=game::OriginalCameraStartStatus::complete)return courseCameraStop(d,r.sourcePC);
            r=game::computeOriginalGuardAngle2371D0(focal,angle);
            if(r.status!=game::OriginalCameraStartStatus::complete)return courseCameraStop(d,r.sourcePC);
            f.viewC0=words(core.view);f.projection80=words(core.projection);f.combined100=words(core.combined);
            f.focal1D8=CourseCamera::floatBits(focal);f.coreKnown=true;
            // Both early236DD0 calls encounter source-unwritten318..324.
            // Dependent caches remain unknown, never guessed from future bounds.
            f.screen140Known=false;f.combinedScreen250Known=false;f.guardProjection290Known=false;f.guardCombined2D0Known=false;
            f.guardAngle330=CourseCamera::floatBits(angle);++courseCameraCallbacks_;
        }
        {
            OriginalOptionsMutationInterlock50KG::Operation operation;
            if(!operation.admitted()||!(camera.entity_.flags29&1u)||!display->frontendFirstViewportCurrent50KG(d.renderer)||
               !display->frontendPriorViewportCurrent50LA(courseCameraPriorViewport_))return courseCameraStop(d,0x11473C);
            if(display->sourceWidth4_!=std::int32_t(f.fullWidth204)||display->sourceHeight8_!=std::int32_t(f.fullHeight208))
                return courseCameraStop(d,0x26F978);
            display->selectFrontendCamera26F940(coursePageCamera_);courseCameraSelected_=true;
        }
        courseCameraPriorViewport_.reset();camera.phase_=CourseCamera::Phase::selectedBeforeBounds236214;
        game::OriginalCameraViewportFields numeric;
        const auto prepared=game::prepareOriginalRaceViewport2361C0(numeric,camera.generation_,camera.key_,scene);
        if(prepared.status!=game::OriginalCameraStartStatus::complete)return courseCameraStop(d,prepared.sourcePC);
        f.depth310=CourseCamera::floatBits(numeric.depth310);f.depth314=CourseCamera::floatBits(numeric.depth314);
        f.left318=CourseCamera::floatBits(numeric.left318);f.right31C=CourseCamera::floatBits(numeric.right31C);
        f.top320=CourseCamera::floatBits(numeric.top320);f.bottom324=CourseCamera::floatBits(numeric.bottom324);
        f.aspect1F8=CourseCamera::floatBits(scene.aspect1F8);f.matrix140=words(numeric.screen140);f.screen140Known=true;
        f.combinedScreen250=words(numeric.combinedScreen250);f.combinedScreen250Known=true;
        f.guardCombined2D0=words(numeric.guardCombined2D0);f.guardCombined2D0Known=true;
        f.guardX334=CourseCamera::floatBits(numeric.guardX334);f.guardY338=CourseCamera::floatBits(numeric.guardY338);
        camera.phase_=CourseCamera::Phase::viewportInitialized2361C0;
        std::uint32_t pc=0;if(!bindCourseCameraObserver114770(camera,pc))return courseCameraStop(d,pc);
        d.sourcePC=0x1D7B74;
        const auto* info=coursePageConfig_->config_->records_.section("PageInfo");
        if(!coursePageConfig_->selectedCurrent("PageInfo")||!info)return courseCameraStop(d,0x1D7B74);
        if(!assets::originalUiTriple50KG(*info,"CameraPosition1",{{0,0,0}},courseCameraPositionStack70_,d.config,0x1D7B74))return courseCameraStop(d,0x1D7B74);
        d.sourcePC=0x1D7BB0;
        if(!assets::originalUiTriple50KG(*info,"CameraLook1",{{0,-1,0}},courseCameraLookStack80_,d.config,0x1D7BB0))return courseCameraStop(d,0x1D7BB0);
        d.sourcePC=0x1D7BE8;
        if(!assets::originalUiTriple50KG(*info,"CameraUp1",{{0,0,1}},courseCameraUpStack90_,d.config,0x1D7BE8))return courseCameraStop(d,0x1D7BE8);
        d.sourcePC=0x1D7C10;
        if(!assets::originalUiNumber50KG(*info,"CameraFOV1",35,courseCameraFovStackD8_,d.config,0x1D7C10))return courseCameraStop(d,0x1D7C10);
        d.sourcePC=0x1D7C34;
        if(!assets::originalUiNumber50KG(*info,"CameraRoll1",0,courseCameraRollStackDC_,d.config,0x1D7C34))return courseCameraStop(d,0x1D7C34);
        //114F9C publishes position BEFORE either independently normalized
        //look/up store. Do not use composeCameraView49F here: it also builds
        //matrices which this source setter does not touch and can reject later.
        f.position180={{CourseCamera::floatBits(courseCameraPositionStack70_[0]),CourseCamera::floatBits(courseCameraPositionStack70_[1]),
            CourseCamera::floatBits(courseCameraPositionStack70_[2]),0x3F800000u}};
        const auto normalizeSource=[](const std::array<float,3>& v,CourseCamera::Words4& output)noexcept {
            if(std::fegetround()!=FE_TONEAREST)return false;
            for(float x:v)if(!std::isfinite(x))return false;
            volatile float xx=v[0]*v[0],yy=v[1]*v[1],zz=v[2]*v[2];
            volatile float xy=float(xx)+float(yy),dot=float(xy)+float(zz);
            if(!std::isfinite(float(dot)))return false;
            float inverse=std::numeric_limits<float>::max();
            if(dot!=0){volatile float root=std::sqrt(std::fabs(float(dot)));volatile float reciprocal=1.f/float(root);inverse=reciprocal;}
            volatile float x=v[0]*inverse,y=v[1]*inverse,z=v[2]*inverse,w=1.f+0.f;
            if(!std::isfinite(float(x))||!std::isfinite(float(y))||!std::isfinite(float(z)))return false;
            output={{CourseCamera::floatBits(x),CourseCamera::floatBits(y),CourseCamera::floatBits(z),CourseCamera::floatBits(w)}};return true;
        };
        CourseCamera::Words4 normalizedLook;
        if(!normalizeSource(courseCameraLookStack80_,normalizedLook))return courseCameraStop(d,0x114FAC);
        f.look190=normalizedLook; //114FD0, before starting Up normalization.
        CourseCamera::Words4 normalizedUp;
        if(!normalizeSource(courseCameraUpStack90_,normalizedUp))return courseCameraStop(d,0x114FE0);
        f.up1A0=normalizedUp; //115004.
        f.roll1D4=CourseCamera::floatBits(courseCameraRollStackDC_);if(courseCameraFovStackD8_!=0)f.fov1D0=CourseCamera::floatBits(courseCameraFovStackD8_);
        f.count210=display->sourceBufferCount0C_+std::uint32_t{1};camera.phase_=CourseCamera::Phase::poseSet114F90;
        // Setter does NOT rebuild236DD0. Old constructor core and unknown290
        // remain unchanged until an actual later update dispatch.
        courseCameraReturn_=1;courseCameraCallbackReturned_=true;d.sourcePC=0x1D7C74;return true;
    }catch(...){return courseCameraStop(d,d.sourcePC);}
    bool preflightNativeCoursePageCamera(CourseCameraDiagnostic& d)const noexcept {
        d={};if(courseCameraNativeRetired_)return !coursePageCamera_&&!courseCameraDisplay_&&!courseCameraDisplayPinned_;
        if(!courseCameraAttempted_)return !coursePageCamera_&&!courseCameraDisplayPinned_;
        if(!courseCameraOwnerCurrent()||!courseCameraDisplay_||!courseCameraDisplayPinned_||
           !courseCameraDisplay_->frontendOwnerCurrent50KG()||courseCameraDisplay_->retiring_||
           courseCameraDisplay_->localizationServiceActive_||courseCameraDisplay_->localizationServiceFaulted_||
           courseCameraDisplay_->dispatch_.pendingCallbackDC||courseCameraDisplay_->dispatch_.count54||courseCameraDisplay_->dispatch_.count60)return false;
        if(!coursePageCamera_)return !courseCameraSelected_;
        const auto& c=*coursePageCamera_;
        if(c.inertStorageCurrent(issuer_,scope_->application_))return !courseCamera4BCPublished_&&!courseCameraSelected_;
        if(courseCameraNativeDisposed_)return c.nativeStorageDisposable(issuer_,scope_->application_);
        if(courseCamera4BCPublished_&&pagePayload_.shared_.camera4BC!=&c)return false;
        if(c.fields_.parent38&&c.fields_.parent38!=construction_->sourceNode2A4_)return false;
        if(!c.preflightCourseNativeDisposal(this,c.fields_.parent38,c.fields_.registration34)||
           !preflightCourseCameraObserverNative(c))return false;
        const auto expected=1+(courseCameraSelected_&&!courseCameraSelectionWithdrawn_?1:0);
        if(coursePageCamera_.use_count()!=expected)return false;
        if(courseCameraSelected_&&!courseCameraSelectionWithdrawn_)
            return courseCameraDisplay_->courseCameraSelectedCurrent50KG(this,c,d.renderer);
        if(courseCameraSelectionWithdrawn_)
            return courseCameraDisplay_->courseCameraWithdrawnCurrent50KG(this,c,d.renderer);
        return courseCameraDisplay_->frontendFirstViewportCurrent50KG(d.renderer);
    }
    bool abortNativeCoursePageCamera(CourseCameraDiagnostic& d)noexcept {
        if(courseCameraNativeRetired_||!courseCameraAttempted_)return true;
        if(!preflightNativeCoursePageCamera(d))return false;
        if(coursePageCamera_&&!courseCameraNativeDisposed_){auto& c=*coursePageCamera_;
            if(!c.inertStorageCurrent(issuer_,scope_->application_)){
                std::uint32_t pc=0;if(!abortCourseCameraObserverNative(c,pc)){d.sourcePC=pc;return false;}
                if(courseCameraSelected_&&!courseCameraSelectionWithdrawn_){
                    if(!courseCameraDisplay_->withdrawCourseCameraSelectionNative(this,c,d.renderer))return false;
                    courseCameraSelectionWithdrawn_=true;
                }
                d.camera=c.disposeCourseNative(this,c.fields_.parent38);if(!d.camera.okay())return false;
            }
            courseCameraNativeDisposed_=true;
        }
        coursePageCamera_.reset();courseCameraPriorViewport_.reset();
        if(courseCameraDisplayPinned_){courseCameraDisplay_->unpin();courseCameraDisplayPinned_=false;}
        courseCameraDisplay_.reset();courseCameraNativeRetired_=true;d.sourcePC=0;return true;
        // Keep source4BC and callback return/history intact; no sourceNULL reset.
    }
