// Private fragment of OriginalUiCourseOwnedLifecycle50KG, after Course lights.
// Source Fog/Off/On payloads; no attachment before actual later1CD080.
    struct CourseFogDescriptor {std::uint32_t rgb00=0;float visibility04=0,haziness08=0;};
    struct CourseFogPrimitive {
        //133F80 writes these only. Source00..1F remain unrepresented.
        void* buffer20=nullptr;std::uint32_t count24=0,count28=0;std::int32_t field2C=-1;
    };
    struct CourseFog {
        C entity;std::uint32_t color30=0;float start34=0,end38=0,visibility3C=0,haziness40=0;
        std::uint32_t field44=0,field48=0,field4C=0;float far50=0,near54=0,gap58=0;
        CourseFogPrimitive* helper5C=nullptr;std::uint32_t dirty60=0;
        bool constructed=false,rangeKnown=false,loaded=false;
    };
    struct CourseFogOff {C entity;bool constructed=false;};
    struct CourseFogOn {C entity;const C* fog30=nullptr;bool constructed=false,borrowKnown=false;};
    std::unique_ptr<CourseFogDescriptor> courseFogDescriptor_;
    std::unique_ptr<CourseFog> courseFog_;
    std::unique_ptr<CourseFogPrimitive> courseFogPrimitive_;
    std::unique_ptr<CourseFogOff> courseFogOff_;
    std::unique_ptr<CourseFogOn> courseFogOn_;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> courseFogDisplay_;
    bool courseFogDisplayPinned_=false,courseFogAttempted_=false,courseFogRendererWritten_=false;
    bool courseFogDisplayCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&issuer_&&courseFogDisplay_&&courseFogDisplayPinned_&&
            courseFogDisplay_->sourceFogFieldLifetimeCurrent()&&courseFogDisplay_==loaderDisplay_&&
            courseFogDisplay_->application_==issuer_&&courseFogDisplay_->video_==video_&&
            video_==issuer_->videoOwner_&&courseFogDisplay_->generation_==issuer_->raceDisplayGeneration50LA_&&
            courseFogDisplay_==issuer_->raceDisplayOwner50LA_.lock();
    }
    bool loadManagerFog1CCFA0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCDA0;
        if(thread_!=std::this_thread::get_id()||terminal_||courseFogAttempted_||loaderStopPC_!=0x1CCDA0||
           !issuer_||!construction_||!modeEffect_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           !courseLight_||courseLight_->courseZoneOwner_!=this||!courseLight_->current()||
           courseLight_->count94_!=2||std::fegetround()!=FE_TONEAREST)return false;
        for(std::size_t i=0;i<2;++i)if(!courseUiLights_[i]||!courseUiLights_[i]->registered||
           !courseUiLights_[i]->attached||!courseLightEntityCurrent(courseUiLights_[i]->entity,i))return false;
        // Native count and Display pin admission before any source malloc or
        // Fog writes. No previous renderer-color save/restore is invented.
        d.entity=construction_->admitCourseFogCounts();if(!d.entity.okay())return false;
        if(!loaderDisplay_||!loaderDisplay_->canRetainSourceFogField()||
           loaderDisplay_->application_!=issuer_||loaderDisplay_->video_!=video_||
           loaderDisplay_->generation_!=issuer_->raceDisplayGeneration50LA_||
           loaderDisplay_!=issuer_->raceDisplayOwner50LA_.lock())return false;
        courseFogDisplay_=loaderDisplay_;courseFogDisplay_->pin();courseFogDisplayPinned_=true;
        courseFogAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        courseFogDescriptor_.reset(new(std::nothrow) CourseFogDescriptor); //1CE7C4.
        if(!courseFogDescriptor_)return stop(0x1CE7D4); // First actual default store.
        auto& descriptor=*courseFogDescriptor_;
        descriptor.rgb00=0x008080c0u;descriptor.visibility04=1;descriptor.haziness08=.5f;
        const auto* section=loaderConfig_->config_->records_.section("Fog");
        // Exact authenticated Course member: typo Visibilty is not Visibility.
        // Do not copy the unrelated Garage blue-fog descriptor values.
        if(!section||section->find("Visibility")||!section->find("Visibilty")||
           section->find("Visibilty")->text!="1")return stop(0x1CE7F4);
        if(!assets::originalUiNumber50KG(*section,"Visibility",1,descriptor.visibility04,d.config,0x1CE81C)||
           descriptor.visibility04!=1)return stop(0x1CE81C);
        if(!assets::originalUiNumber50KG(*section,"Haziness",.5f,descriptor.haziness08,d.config,0x1CE8BC)||
           descriptor.haziness08!=0)return stop(0x1CE8BC);
        const auto* color=section->find("ColorRGB");
        if(!color||color->text!="255,255,255")return stop(0x1CE964);
        descriptor.rgb00=0x00ffffffu; // Actual integer RGB pack1CEB20.
        courseFog_.reset(new(std::nothrow) CourseFog); //1CE994.
        if(courseFog_){auto& fog=*courseFog_;
            d.entity=construction_->constructCourseFogEntity(0,fog.entity);
            if(!d.entity.okay())return stop(d.entity.sourcePC);
            fog.constructed=true;fog.field44=0;fog.field4C=0;fog.field48=1;fog.dirty60=1;fog.helper5C=nullptr;
        }
        // A NULL Fog skips13E6B0 but still reaches13E780(NULL).141C20 is
        // return-this; first dereference is13E7C4, before manager180 publication.
        if(!courseFog_)return stop(0x13E7C4);
        auto& fog=*courseFog_;fog.far50=1200;fog.near54=900;fog.gap58=200;
        courseFogPrimitive_.reset(new(std::nothrow) CourseFogPrimitive); //13E7D0.
        fog.helper5C=courseFogPrimitive_.get(); //13E7E8 publishes NULL too.
        //13E830 does not touch helper5C on this actual a2=0 branch.
        fog.color30=descriptor.rgb00|0xff000000u;
        fog.visibility3C=descriptor.visibility04;fog.haziness40=descriptor.haziness08;
        fog.end38=add(fog.near54,mul(fog.visibility3C,sub(fog.far50,fog.near54)));
        fog.start34=mul(sub(1,fog.haziness40),sub(fog.end38,fog.gap58));
        fog.rangeKnown=true;
        if(!courseFogDisplayCurrent())return stop(0x13E8A8);
        courseFogDisplay_->sourceFogColor28_=fog.color30;
        courseFogDisplay_->sourceFogColor28Known_=true;courseFogRendererWritten_=true;
        fog.dirty60=1;fog.loaded=true; //13E8B0 after scalar renderer store.
        d.entity=construction_->publishSourceCourseFogField(0,&fog.entity); //1CE9E0.
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        courseFogDescriptor_.reset(); //1CEA00, source free before Off allocation.
        courseFogOff_.reset(new(std::nothrow) CourseFogOff); //1CCFD8.
        if(courseFogOff_){
            d.entity=construction_->constructCourseFogEntity(1,courseFogOff_->entity);
            if(!d.entity.okay())return stop(d.entity.sourcePC);
            courseFogOff_->constructed=true;
        }
        if(!courseFogOff_)return stop(0x1CCFF4); // NULL virtual receiver load before188 store.
        // Actual v30=13F890->141C20 returns this, no further source writes.
        d.entity=construction_->publishSourceCourseFogField(1,&courseFogOff_->entity); //1CD008.
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        courseFogOn_.reset(new(std::nothrow) CourseFogOn); //1CD020.
        if(courseFogOn_){
            d.entity=construction_->constructCourseFogEntity(2,courseFogOn_->entity);
            if(!d.entity.okay())return stop(d.entity.sourcePC);
            courseFogOn_->constructed=true;
        }
        if(!courseFogOn_)return stop(0x13F9DC); //141C20 returned NULL, first On30 store.
        courseFogOn_->fog30=&fog.entity;courseFogOn_->borrowKnown=true; // No source retain.
        d.entity=construction_->publishSourceCourseFogField(2,&courseFogOn_->entity); //1CD04C.
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        loaderStopPC_=d.sourcePC=0x1CCDB0;return true; //1CCFA0 returned; BEFORE actual vB0.
    }
    bool attachLoadedCourseFog1CD080(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCDDC;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CCDDC||!issuer_||!courseWidgetsReturned_||
           !courseWidgetsConfig_||!courseWidgetsConfig_->closedWidgetsStack1573B0()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||!courseFogDisplayCurrent()||
           !courseFog_||!courseFog_->constructed||!courseFog_->loaded||!courseFogOff_||!courseFogOff_->constructed||
           !courseFogOn_||!courseFogOn_->constructed||!courseFogOn_->borrowKnown||courseFogOn_->fog30!=&courseFog_->entity)return false;
        C* entities[]{&courseFog_->entity,&courseFogOff_->entity,&courseFogOn_->entity};
        for(std::size_t i=0;i<3;++i){
            d.entity=construction_->attachCourseFog1CD080(i,*entities[i]);
            if(!d.entity.okay()){terminal_=true;loaderStopPC_=d.sourcePC=d.entity.sourcePC;return false;}
        }
        loaderStopPC_=d.sourcePC=0x1CCDEC;return true; //1CD080 true; BEFORE actual Page-loader v88=1CEC10.
    }
    bool detachNativeCourseFogBeforeWidgets()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!construction_)return true;
        bool attached=false;for(bool value:construction_->courseFogAttached_)attached|=value;
        return !attached||construction_->detachNativeCourseFogTail().okay();
    }
    bool abortNativeCourseFog()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        // Check every acquired reference before freeing helper payload. A
        // rejected outside Entity borrow must retain all referenced storage.
        if(((courseFog_&&courseFog_->constructed)||(courseFogOff_&&courseFogOff_->constructed)||
            (courseFogOn_&&courseFogOn_->constructed))&&
           (!construction_||construction_->pageAcquired_||!construction_->registrationCurrent()))return false;
        if(courseFogOn_&&courseFogOn_->borrowKnown&&
           (!courseFog_||courseFogOn_->fog30!=&courseFog_->entity))return false;
        // Native partial cleanup, not source1CD080 attachment or142590 callback.
        // The On30 borrow must be gone before its actual Fog owner is retired.
        if(courseFogOn_){
            if(courseFogOn_->constructed){
                const auto step=construction_->abortNativeCourseFogEntity(2,courseFogOn_->entity);
                if(!step.okay())return false;
            }
            courseFogOn_.reset();
        }
        if(courseFogOff_){
            if(courseFogOff_->constructed){
                const auto step=construction_->abortNativeCourseFogEntity(1,courseFogOff_->entity);
                if(!step.okay())return false;
            }
            courseFogOff_.reset();
        }
        if(courseFog_){
            // Cold133F80 helper has no buffer/packet/GPU borrowing. Its actual
            // source133FA0 interval therefore has nothing beyond owned storage.
            if(courseFogPrimitive_&&(courseFogPrimitive_->buffer20||courseFogPrimitive_->count24||
               courseFogPrimitive_->count28||courseFogPrimitive_->field2C!=-1))return false;
            courseFogPrimitive_.reset();
            if(courseFog_->constructed){
                const auto step=construction_->abortNativeCourseFogEntity(0,courseFog_->entity);
                if(!step.okay())return false;
            }
            courseFog_.reset();
        }
        courseFogDescriptor_.reset();
        if(courseFogDisplayPinned_){if(!courseFogDisplay_)return false;courseFogDisplay_->unpin();courseFogDisplayPinned_=false;}
        courseFogDisplay_.reset(); // Never roll back the actual source28 write.
        return true;
    }
