// Exact authored absent-Background path between shared prompt resources and
// counted collections. Positive texture/video paths remain unjoined.
    bool coursePageBackgroundHookAttempted_=false,coursePageBackgroundHookReturned_=false;
    std::int32_t coursePageUseVideoStack8B4_=0;
    std::array<char,272> coursePageBackgroundNameStack50_{};
    bool coursePageBackgroundNameKnown_=false;
    bool continueCoursePageBackground1D6D74(Diagnostic& d)noexcept try {
        d={};d.sourcePC=0x1D6D74;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1D6D74||
           coursePageBackgroundHookAttempted_||!coursePageBackgroundKnown_||!construction_||!issuer_||
           !construction_->pageAcquired_||!construction_->registrationCurrent()||!coursePageConfig_||
           !coursePageConfig_->selectedCurrent("PageInfo")||!pagePayload_.shared_.backgroundColor430.has_value())return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        coursePageBackgroundHookAttempted_=true;
        coursePageUseVideoStack8B4_=0; // Actual1D6D78 delay store before getter output.
        const auto* info=coursePageConfig_->config_->records_.section("PageInfo");
        if(!info||!assets::originalUiInteger50KG(*info,"UseVideoAsTexture",0,coursePageUseVideoStack8B4_,d.config,0x1D6D74))
            return stop(d.config.sourcePC?d.config.sourcePC:0x1D6D74);
        d.sourcePC=0x1D6D8C;
        const std::string_view background(coursePageBackgroundStack770_.data());
        // Actual v8C=1D6080 tests .pss; current authored missing Background
        // yields *INVALID*, so the direct2A6490 path is authentic. Wrapper
        // passes a2=0 regardless of the getter result on this source branch.
        if(background.find(".pss")!=std::string_view::npos)return stop(0x1D60B0);
        const auto& prefix=issuer_->loadingTextureDirectory36BCA0_;
        if(prefix!="UI\\Textures\\")return stop(0x2A64C4);
        if(prefix.size()+background.size()>=coursePageBackgroundNameStack50_.size())return stop(0x2A64C4);
        std::memcpy(coursePageBackgroundNameStack50_.data(),prefix.data(),prefix.size());
        std::memcpy(coursePageBackgroundNameStack50_.data()+prefix.size(),background.data(),background.size());
        coursePageBackgroundNameStack50_[prefix.size()+background.size()]=0;coursePageBackgroundNameKnown_=true;
        // Earlier154/158 destruction calls are skipped ONLY for actual base
        // constructor NULL fields, never for an absent native owner assumption.
        if(construction_->pageBackground154_)return stop(0x2A64E0);
        if(construction_->pageBackground158_)return stop(0x2A6500);
        if(background.empty()||background.front()!='*')return stop(0x2A6538);
        construction_->pageBackground154_=nullptr; //2A6634.
        construction_->pageBackground158_=nullptr; //2A6638.
        coursePageBackgroundHookReturned_=true; // Actual2A6490 returns0; caller ignores it.
        d.sourcePC=0x1D6DA4;
        if(!assets::originalUiInteger50KG(*info,"PageNo",0,construction_->pageCollections_.pageNumber138,d.config,0x1D6DA4))
            return stop(d.config.sourcePC);
        loaderStopPC_=d.sourcePC=0x1D6DAC;return true; // BEFORE actual router allocation.
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
