// Native exception handlers retain the explicit current source-call boundary;
// allocating std::string argument conversion must never escape noexcept.
// Actual1CEC10/Page1 factory prefix. No144 slot is published until the ENTIRE
// virtual90 factory returns; this bounded loader does not reach that return.
    std::unique_ptr<C*[]> coursePagesBacking144_;
    C** sourcePages144_=nullptr;
    bool sourcePages144Known_=false,coursePagesTableNativeReleased_=false;
    bool coursePagesGetter148Reached_=false,coursePagesAttempted_=false,coursePageBaseAttempted_=false;
    std::array<bool,1> coursePageSlotsKnown_{};
    std::size_t coursePageOrdinal_=0;
    std::array<char,256> coursePageSectionStack240_{},coursePageFileStack140_{},coursePagePathStack40_{};
    bool coursePageFileKnown_=false,coursePagePathKnown_=false;
    std::unique_ptr<OriginalUiCourseManagerConfig50KG> coursePageConfig_;
    std::array<char,256> coursePageBinaryPathStack160_{};
    std::array<char,260> coursePageStartStack50_{};
    std::int32_t coursePageHasPopupStack8AC_=0;
    bool coursePageHasPopupKnown_=false;
    std::array<std::int32_t,9> coursePagePopupIdsStack740_{};
    std::array<bool,9> coursePagePopupIdsKnown_{};
    bool loadCoursePagesPrefix1CEC10(Diagnostic& d)noexcept try {
        d={};d.sourcePC=0x1CCDEC;
        if(thread_!=std::this_thread::get_id()||terminal_||coursePagesAttempted_||loaderStopPC_!=0x1CCDEC||
           !issuer_||!construction_||!modeEffect_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           !issuer_->uiResourceDirectory36BB90_||*issuer_->uiResourceDirectory36BB90_!="UI\\UICourse\\")return false;
        coursePagesAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        d.sourcePC=0x1CEC30;
        if(!loaderConfig_->selectWidgetsSection157D00("UIBase",d.config))return stop(d.config.sourcePC);
        d.sourcePC=0x1CEC48;
        const auto* general=loaderConfig_->config_->records_.section("UIBase");
        if(!loaderConfig_->selectedCurrent("UIBase")||!general||
           !assets::originalUiInteger50KG(*general,"TotalPages",0,construction_->pageCount148_,d.config,0x1CEC48))return stop(0x1CEC48);
        coursePagesGetter148Reached_=true; // Sole live source148 lane in Scope.
        if(construction_->pageCount148_!=1)return stop(0x1CEC54); // Exact authenticated Course cohort.
        coursePagesBacking144_.reset(new(std::nothrow) C*[1]);
        sourcePages144_=coursePagesBacking144_.get();sourcePages144Known_=true; //1CEC5C includes real NULL.
        // No initialization/read of the unknown source slot0.
        coursePageOrdinal_=0;
        constexpr char section[]="Page 1";std::memcpy(coursePageSectionStack240_.data(),section,sizeof(section));
        d.sourcePC=0x1CEC84;
        if(!loaderConfig_->selectWidgetsSection157D00(coursePageSectionStack240_.data(),d.config))return stop(d.config.sourcePC);
        d.sourcePC=0x1CEC9C;
        const auto* page=loaderConfig_->config_->records_.section(coursePageSectionStack240_.data());
        const auto* file=page?page->find("PageFile"):nullptr;
        if(!loaderConfig_->selectedCurrent(coursePageSectionStack240_.data())||!file||!file->sourceValuePresent||
           file->text!="track.dat")return stop(0x1CEC9C);
        std::memcpy(coursePageFileStack140_.data(),file->text.c_str(),file->text.size()+1);coursePageFileKnown_=true;
        constexpr char path[]="UI\\UICourse\\track.dat";
        std::memcpy(coursePagePathStack40_.data(),path,sizeof(path));coursePagePathKnown_=true; //Actual1CECB8 format.
        loaderStopPC_=d.sourcePC=0x1CECD0;return true; // BEFORE virtual90, not full1CEC10 return.
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
    bool loadCoursePageBasePrefix1D63B0(Diagnostic& d)noexcept try {
        d={};d.sourcePC=0x1CECD0;
        if(thread_!=std::this_thread::get_id()||terminal_||coursePageBaseAttempted_||coursePageConfig_||loaderStopPC_!=0x1CECD0||
           !coursePagesAttempted_||!coursePagesGetter148Reached_||!sourcePages144Known_||coursePagesTableNativeReleased_||
           !construction_||construction_->pageCount148_!=1||coursePageOrdinal_||coursePageSlotsKnown_[0]||
           !coursePagePathKnown_||!coursePageFileKnown_||!managerNameKnown_||!scope_||!loaderConfig_||!loaderConfig_->parsedCurrent())return false;
        // Native transport/storage admission before entering the actual Page
        // allocation interval. Refusal does NOT impersonate source mallocNULL.
        try {coursePageConfig_.reset(new OriginalUiCourseManagerConfig50KG);}catch(...){return false;}
        if(!coursePageConfig_->admitStackConfigNative(*loaderConfig_)){
            if(!coursePageConfig_->closeNative())return false;coursePageConfig_.reset();return false;
        }
        coursePageBaseAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        // Parent component includes genuine1D01A0 nullable allocation, actual
        // counted constructors, and NULL first-vtable cut1D01BC.
        if(!constructPageComponent1D01B4(d))return stop(d.sourcePC);
        if(!construction_->pageAcquired_||pagePayload_.phase_!=OriginalUiCoursePagePayload50KG::Phase::derived)
            return stop(0x1D63B0);
        auto& fields=pagePayload_.shared_;
        fields.manager270=&scope_->manager_; //1D63CC.
        fields.manager130=&scope_->manager_; //1D63D0.
        if(!coursePageConfig_->constructAdmittedStack157320())return stop(0x1D63E0);
        const auto pathBytes=std::strlen(coursePagePathStack40_.data());
        std::memcpy(coursePageBinaryPathStack160_.data(),coursePagePathStack40_.data(),pathBytes+1);
        // Authenticated final .dat suffix, genuine309230 replacement with.bpb.
        if(pathBytes<4||std::strcmp(coursePageBinaryPathStack160_.data()+pathBytes-4,".dat"))return stop(0x1D6458);
        std::memcpy(coursePageBinaryPathStack160_.data()+pathBytes-4,".bpb",5);
        using Open=OriginalUiCourseManagerConfig50KG::OpenResult;
        Open opened;
        try {opened=coursePageConfig_->open1577D0(coursePageBinaryPathStack160_.data(),d.config);}catch(...){return stop(0x1D6468);}
        if(opened==Open::absent){
            try {opened=coursePageConfig_->open1577D0(coursePagePathStack40_.data(),d.config);}catch(...){return stop(0x1D648C);}
            if(opened==Open::absent){
                if(!coursePageConfig_->closeWidgetsStack1573B0(-1,d.config))return stop(d.config.sourcePC);
                return stop(0x1D64B8); // Genuine early-return boundary, no Page factory completion.
            }
            if(opened!=Open::opened)return stop(d.config.sourcePC?d.config.sourcePC:0x1D648C);
        }else if(opened!=Open::opened)return stop(d.config.sourcePC?d.config.sourcePC:0x1D6468);
        if(!coursePageConfig_->parsedCurrent())return stop(0x1D64C4);
        std::memcpy(fields.name30.data(),managerName194_.data(),std::strlen(managerName194_.data())+1);fields.name30Known=true;
        d.sourcePC=0x1D64D4;
        if(!coursePageConfig_->selectWidgetsSection157D00("PageInfo",d.config))return stop(d.config.sourcePC);
        d.sourcePC=0x1D64EC;
        const auto* info=coursePageConfig_->config_->records_.section("PageInfo");
        if(!coursePageConfig_->selectedCurrent("PageInfo")||!info)return stop(0x1D64D4);
        if(!assets::originalUiInteger50KG(*info,"OpenSize",224,fields.openSize274,d.config,0x1D64EC))return stop(0x1D64EC);
        d.sourcePC=0x1D6508;
        if(!assets::originalUiNumber50KG(*info,"Time",1.f,fields.time4B0,d.config,0x1D6508))return stop(0x1D6508);
        d.sourcePC=0x1D6528;
        const auto* start=info->find("Start");
        const std::string_view startValue=start&&start->sourceValuePresent?std::string_view(start->text):std::string_view("Closed");
        if(startValue.size()>=coursePageStartStack50_.size())return stop(0x1D6528);
        std::memcpy(coursePageStartStack50_.data(),startValue.data(),startValue.size());coursePageStartStack50_[startValue.size()]=0;
        if(startValue!="Closed")return stop(0x1D6548); // Other1D7430 state paths intentionally unjoined.
        fields.closed284=224;fields.closed28C=224;fields.state4B4=2; //Actual1D7430(state2); no268 write.
        coursePageHasPopupStack8AC_=0; //Actual stack initialization1D6588.
        d.sourcePC=0x1D6584;
        const auto* popup=info->find("HasPopup");
        // Exact authored T branch of158920 produces signed32 one, not a bool
        // reinterpretation or widening of generic integer getter authority.
        if(!popup||!popup->sourceValuePresent||popup->text!="T")return stop(0x1D6584);
        coursePageHasPopupStack8AC_=1;coursePageHasPopupKnown_=true;
        constexpr const char* keys[]{"PopupConfirmWidget","PopupFormatWidget","PopupFormatSuccessWidget",
            "PopupFormatFailWidget","PopupYesWidget","PopupNoWidget","PopupAskFormatWidget","PopupNoMemcardWidget","PopupFullMemcardWidget"};
        for(std::size_t i=0;i<9;++i){const auto pc=0x1D65A8u+std::uint32_t(i*24);d.sourcePC=pc;
            if(!assets::originalUiInteger50KG(*info,keys[i],-1,coursePagePopupIdsStack740_[i],d.config,pc))return stop(pc);
            coursePagePopupIdsKnown_[i]=true;
        }
        if(!constructLoadCoursePopup1D6670(coursePagePopupIdsStack740_,d))return stop(d.sourcePC);
        // Actual popup hide completed in parent helper; now original two
        // Aperture allocations/loads/post-alpha-and-color loops in order.
        for(std::size_t i=0;i<2;++i)if(!pagePayload_.loadApertureComponent1D66C0(i,d.page))return stop(d.page.sourcePC);
        loaderStopPC_=d.sourcePC=0x1D6804;return true; // BEFORE division/store4B8 and camera vC4.
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
    bool coursePageCameraCallerAttempted_=false;
    bool continueCoursePageCamera1D6804(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1D6804;
        if(thread_!=std::this_thread::get_id()||terminal_||coursePageCameraCallerAttempted_||
           loaderStopPC_!=0x1D6804||!coursePageBaseAttempted_||!issuer_||!construction_||!modeEffect_||
           !construction_->pageAcquired_||!construction_->registrationCurrent()||!coursePopupHidden_||
           !coursePageConfig_||!coursePageConfig_->selectedCurrent("PageInfo")||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           pagePayload_.shared_.speed4B8.has_value())return false;
        // Admit this exact authenticated scalar cohort/native arithmetic before
        // source DIV.S. No literal310 substitution or altered rounding mode.
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
        return false;
#else
        auto& f=pagePayload_.shared_;
        if(!std::numeric_limits<float>::is_iec559||std::numeric_limits<float>::radix!=2||
           std::numeric_limits<float>::digits!=24||std::fegetround()!=FE_TONEAREST||
           f.openSize274!=69||f.time4B0!=0.5f)return false;
        coursePageCameraCallerAttempted_=true;
        // Original SUBU then signed CVT.S.W. Unsigned arithmetic preserves the
        // source wrap without introducing host signed-overflow undefined behavior.
        const std::int32_t delta=std::bit_cast<std::int32_t>(std::uint32_t(224)-std::uint32_t(f.openSize274));
        volatile float numerator=static_cast<float>(delta); //1D6800.
        volatile float denominator=f.time4B0;
        volatile float speed=numerator/denominator; //1D6804.
        const float storedSpeed=speed;f.speed4B8=storedSpeed; //1D6808 actual previously-unwritten scalar publication.
        loaderStopPC_=0x1D6814;
        CourseCameraDiagnostic camera;
        if(!loadCoursePageCamera1D7A90(camera)){
            terminal_=true;loaderStopPC_=d.sourcePC=camera.sourcePC?camera.sourcePC:0x1D6814;return false;
        }
        if(!courseCameraCallbackReturned_||!courseCamera4BCPublished_||
           (courseCameraReturn_==0?(bool(coursePageCamera_)||f.camera4BC!=nullptr):
            (courseCameraReturn_!=1||!coursePageCamera_||f.camera4BC!=coursePageCamera_.get()))){
            terminal_=true;loaderStopPC_=d.sourcePC=0x1D681C;return false;
        }
        // Caller does not test callback v0: both genuine returned0 NULL and
        // returned1 camera reach this next actual prompt Quad call.
        loaderStopPC_=d.sourcePC=0x1D682C;return true;
#endif
    }
    bool abortNativeCoursePagesConfigAndTable()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        // No source Page factory return/slot publication is admitted yet.
        if(coursePageSlotsKnown_[0])return false;
        if(coursePageConfig_){if(!coursePageConfig_->closeNative())return false;coursePageConfig_.reset();}
        if(coursePagesBacking144_){coursePagesBacking144_.reset();coursePagesTableNativeReleased_=true;}
        // NULL publication is historical too; never overwrite source144/148.
        if(sourcePages144Known_)coursePagesTableNativeReleased_=true;
        return true;
    }
