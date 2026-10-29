// Private Course owner fragment. Same actual source manager154 table/158 count.
    std::unique_ptr<OriginalUiCourseManagerConfig50KG> courseWidgetsConfig_;
    std::string courseWidgetsPathStack70_,courseWidgetSectionStack70_;
    std::optional<std::int32_t> courseWidgetCount158_;
    std::unique_ptr<C*[]> courseWidgetTableBacking154_;
    C** sourceWidgetTable154_=nullptr;
    bool sourceWidgetTable154Known_=false,courseWidgetsAttempted_=false,courseWidgetTableNativeReleased_=false;
    bool courseWidgetsReturned_=false;
    std::array<bool,84> courseWidgetSlotsKnown_{}; // Unwritten source slots stay UNKNOWN.
    std::size_t courseWidgetOrdinal_=0;
    std::int32_t courseWidgetOnTopStack64C_=0;bool courseWidgetOnTopKnown_=false;
    bool loadManagerWidgetsConfig1CD7A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCDB0;
        if(thread_!=std::this_thread::get_id()||terminal_||courseWidgetsAttempted_||loaderStopPC_!=0x1CCDB0||
           !issuer_||!construction_||!modeEffect_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           !issuer_->uiResourceDirectory36BB90_||*issuer_->uiResourceDirectory36BB90_!="UI\\UICourse\\")return false;
        // Pre-admit transport for actual stack157320 before entering vB0/v98.
        // No source malloc-null or constructor return is claimed on rejection.
        try {courseWidgetsConfig_.reset(new OriginalUiCourseManagerConfig50KG);}catch(...){return false;}
        if(!courseWidgetsConfig_->admitStackConfigNative(*loaderConfig_)){
            courseWidgetsConfig_->closeNative();courseWidgetsConfig_.reset();return false;
        }
        courseWidgetsAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        // Actual Course vB0=1CCF30 returns0 without effect; caller ignores it.
        // Next v98=1CD7A0 is an actual automatic Widgets.Dat Config owner.
        try {courseWidgetsPathStack70_=*issuer_->uiResourceDirectory36BB90_+"Widgets.Dat";}
        catch(...){return stop(0x1CD7D4);}
        if(!courseWidgetsConfig_->constructAdmittedStack157320())return stop(0x1CD7DC);
        if(courseWidgetsConfig_->open1577D0(courseWidgetsPathStack70_,d.config)!=OriginalUiCourseManagerConfig50KG::OpenResult::opened)
            return stop(0x1CD7EC); // Error-print/destructor/false return not fabricated.
        if(!courseWidgetsConfig_->selectWidgetsSection157D00("General",d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD824);
        const auto* general=courseWidgetsConfig_->config_->records_.section("General");std::int32_t count=0;
        if(!general||!assets::originalUiInteger50KG(*general,"NumberOfWidgets",0,count,d.config,0x1CD83C))
            return stop(0x1CD83C);
        courseWidgetCount158_=count; // Actual158 write BEFORE table allocation.
        if(count!=84)return stop(0x1CD848); // Exact authenticated Course cohort only.
        courseWidgetTableBacking154_.reset(new(std::nothrow) C*[84]);
        sourceWidgetTable154_=courseWidgetTableBacking154_.get();sourceWidgetTable154Known_=true; //1CD850, including NULL.
        // No slot is read/cleared merely because native array storage exists.
        courseWidgetOrdinal_=0;
        try {courseWidgetSectionStack70_="Widget 1";}catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))
            return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;
        loaderStopPC_=d.sourcePC=0x1CD8A0;return true; // BEFORE actual typed1CD160 factory; no fullv98 return.
    }
    bool loadNextCourseOverlayWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           !courseWidgetsAttempted_||!courseOverlayTextOrdinalAdmitted(courseWidgetOrdinal_)||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row)return stop(0x1CD8A0);
        const auto i=courseWidgetOrdinal_;
        if(i>=67){if(!courseFinalTextPrefixCurrent(i))return stop(0x1CD8A0);}
        else if(i>=59){if(!courseDynamicTextPrefixCurrent(i))return stop(0x1CD8A0);}
        else if(i>=26&&!courseStaticTextPrefixCurrent(i))return stop(0x1CD8A0);
        if(!loadCourseOverlayText1CD160(i,*row,d))return stop(d.sourcePC);
        C* entity=courseWidgetEntity(i);if(!entity)return stop(0x1CD8A8);
        // Actual1CD8C0..FC first writes both source input lanes on returned
        // widget before recursive v1C disable and table publication.
        for(std::size_t input=0;input<2;++input){std::int32_t wanted=0;
            const char* key=input?"WantsJoystickInput2":"WantsJoystickInput1";
            // Both keys are absent in these exact authenticated text rows.
            // Keep real32-bit output lanes, not a generic boolean widget policy.
            if(row->find(key)||!assets::originalUiInteger50KG(*row,key,0,wanted,d.config,0x1CD8E0))
                return stop(0x1CD8E0);
            if(input)courseTextRows_[i].outer->field54=static_cast<std::uint32_t>(wanted);
            else courseTextRows_[i].outer->field50=static_cast<std::uint32_t>(wanted);
        }
        d.entity=construction_->disableCourseOverlay1415A0(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        if(!sourceWidgetTable154_)return stop(0x1CD918); // Retain actual loaded resource; NULL table cannot publish it.
        sourceWidgetTable154_[i]=entity;courseWidgetSlotsKnown_[i]=true;
        // Actual authored OnTop is false, so no temporary list190 producer.
        d.entity=construction_->attachCourseOverlay1CD9F8(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))
            return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;
        loaderStopPC_=d.sourcePC=0x1CD8A0;return true; // Next actual widget factory, never full84-row completion.
    }
    static bool courseLateSelectableEmptyOrdinal(std::size_t i)noexcept {return i>=56&&i<=58;}
    bool courseLateSelectablePrefixCurrent(std::size_t i)const noexcept {
        if(i<52||i>59||!courseStaticTextPrefixCurrent(52))return false;
        for(std::size_t previous=52;previous<i;++previous){
            if(!courseWidgetSlotsKnown_[previous])return false;
            if(courseSelectableOrdinalAdmitted(previous)){
                const auto& row=courseSelectableRows_[previous];
                if(!row||!row->constructed||!row->loaded||
                   construction_->courseSelectableIcons_[previous]!=&row->entity||
                   !construction_->courseSelectableAttached_[previous]||sourceWidgetTable154_[previous]!=&row->entity)return false;
            }else if(!courseLateSelectableEmptyOrdinal(previous)||sourceWidgetTable154_[previous])return false;
        }
        return true;
    }
    bool courseDynamicTextPrefixCurrent(std::size_t i)const noexcept {
        if(i<59||i>63||!courseLateSelectablePrefixCurrent(59))return false;
        for(std::size_t previous=59;previous<i;++previous){
            const auto& row=courseTextRows_[previous];
            if(!courseWidgetSlotsKnown_[previous]||!row.constructed||!row.loaded||!row.outer||
               construction_->courseOverlayOuters_[previous]!=&row.outer->entity||
               !construction_->courseOverlayAttached_[previous]||sourceWidgetTable154_[previous]!=&row.outer->entity)return false;
        }
        return true;
    }
    static bool courseStaticTextEmptyOrdinal(std::size_t i)noexcept {
        return i==28||i==34||i==35||i==36||i==41||i==44||i==45||i==46||i==47||i==48||i==49||i==50;
    }
    bool courseStaticTextPrefixCurrent(std::size_t i)const noexcept {
        if(i<26||i>52||!courseSoultreePrefixCurrent(26))return false;
        for(std::size_t previous=26;previous<i;++previous){
            if(!courseWidgetSlotsKnown_[previous])return false;
            if(courseOverlayTextOrdinalAdmitted(previous)){
                const auto& row=courseTextRows_[previous];
                if(!row.constructed||!row.loaded||!row.outer||
                   construction_->courseOverlayOuters_[previous]!=&row.outer->entity||
                   !construction_->courseOverlayAttached_[previous]||
                   sourceWidgetTable154_[previous]!=&row.outer->entity)return false;
            }else if(!courseStaticTextEmptyOrdinal(previous)||sourceWidgetTable154_[previous])return false;
        }
        return true;
    }
    static bool courseCohortEmptyOrdinal(std::size_t i)noexcept {
        return i==10||i==16||i==17||i==18||i==23;
    }
    bool courseSoultreePrefixCurrent(std::size_t i)const noexcept {
        if(i<8||i>26||!construction_)return false;
        if(i==8)return true; // Initial factory still reaches allocations before a NULL154 publication cut.
        if(courseWidgetTableNativeReleased_||!sourceWidgetTable154_)return false;
        // Actual prior source table publications, including Empty gaps, remain
        // required alongside the live typed predecessor chain. No compacted rows.
        for(std::size_t previous=8;previous<i;++previous){
            if(!courseWidgetSlotsKnown_[previous])return false;
            if(courseSoultreeOrdinalAdmitted(previous)){
                if(!courseSoultreeRows_[previous]||!courseSoultreeRows_[previous]->constructed||
                   !courseSoultreeRows_[previous]->loaderReturned||
                   construction_->courseSoultreeRows_[previous].outer!=&courseSoultreeRows_[previous]->entity||
                   !construction_->courseSoultreeRows_[previous].attached||
                   sourceWidgetTable154_[previous]!=&courseSoultreeRows_[previous]->entity)return false;
            }else if(!courseCohortEmptyOrdinal(previous)||sourceWidgetTable154_[previous])return false;
        }
        return true;
    }
    bool loadNextCourseEmptyWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           (courseWidgetOrdinal_!=2&&courseWidgetOrdinal_!=3&&!courseCohortEmptyOrdinal(courseWidgetOrdinal_)&&!courseStaticTextEmptyOrdinal(courseWidgetOrdinal_)&&!courseLateSelectableEmptyOrdinal(courseWidgetOrdinal_)&&courseWidgetOrdinal_!=63)||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        if(courseCohortEmptyOrdinal(courseWidgetOrdinal_)&&!courseSoultreePrefixCurrent(courseWidgetOrdinal_))return stop(0x1CD8A0);
        if(courseStaticTextEmptyOrdinal(courseWidgetOrdinal_)&&!courseStaticTextPrefixCurrent(courseWidgetOrdinal_))return stop(0x1CD8A0);
        if(courseLateSelectableEmptyOrdinal(courseWidgetOrdinal_)&&!courseLateSelectablePrefixCurrent(courseWidgetOrdinal_))return stop(0x1CD8A0);
        if(courseWidgetOrdinal_==63&&!courseDynamicTextPrefixCurrent(63))return stop(0x1CD8A0);
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        const auto* type=row?row->find("Type"):nullptr;
        if(!type||type->text!="Empty")return stop(0x1CD6FC);
        // Actual1CD70C/10 sets source s0=NULL and returns it at1CD780.
        // This is the authored Empty branch, not an unsupported-factory success.
        // Caller1CD8AC takes the NULL branch; no hide/input/Entity/attach call.
        if(!sourceWidgetTable154_)return stop(0x1CDA10);
        sourceWidgetTable154_[courseWidgetOrdinal_]=nullptr;courseWidgetSlotsKnown_[courseWidgetOrdinal_]=true;
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))
            return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;loaderStopPC_=d.sourcePC=0x1CD8A0;return true;
    }
    bool loadNextCourseMovementWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           courseWidgetOrdinal_!=4||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !issuer_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row)return stop(0x1CD8A0);
        if(!loadCourseMovement1CD160(4,*row,d))return stop(d.sourcePC);
        C* entity=courseMovementEntity();if(!entity)return stop(0x1CD8A8);
        for(std::size_t input=0;input<2;++input){
            const auto* value=row->find(input?"WantsJoystickInput2":"WantsJoystickInput1");
            // Actual integer158B30 delegates158920. Its first branch compares
            // one character against literal T(3A6348), then stores int32 1 at
            //158968. This exact authenticated row uses T, not a decimal value.
            if(!value||value->text!="T")return stop(0x1CD8E0);
            if(input)courseMovement_->field54=1;else courseMovement_->field50=1;
        }
        d.entity=construction_->disableCourseMovement1415A0(*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        if(!sourceWidgetTable154_)return stop(0x1CD918);
        sourceWidgetTable154_[4]=entity;courseWidgetSlotsKnown_[4]=true;
        d.entity=construction_->attachCourseMovement1CD9F8(*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))
            return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;loaderStopPC_=d.sourcePC=0x1CD8A0;
        return true; // Widget6 Icon remains unjoined; never a full v98 receipt.
    }
    bool courseFinalTextPrefixCurrent(std::size_t i)const noexcept {
        if(i<67||i>81||!courseStaticIconPrefixCurrent(67))return false;
        for(std::size_t previous=67;previous<i;++previous){const auto& row=courseTextRows_[previous];
            if(!courseWidgetSlotsKnown_[previous]||!row.constructed||!row.loaded||!row.outer||
               construction_->courseOverlayOuters_[previous]!=&row.outer->entity||!construction_->courseOverlayAttached_[previous]||
               sourceWidgetTable154_[previous]!=&row.outer->entity)return false;
        }
        return true;
    }
    bool courseStaticIconPrefixCurrent(std::size_t i)const noexcept {
        if(i<64||i>67||!courseDynamicTextPrefixCurrent(63)||!courseWidgetSlotsKnown_[63]||sourceWidgetTable154_[63])return false;
        for(std::size_t previous=64;previous<i;++previous){const auto& row=courseIconRows_[previous];
            if(!courseWidgetSlotsKnown_[previous]||!row.constructed||!row.loaded||!row.owner||
               construction_->courseIcons_[previous]!=&row.owner->entity||!construction_->courseIconsAttached_[previous]||
               sourceWidgetTable154_[previous]!=&row.owner->entity)return false;
        }
        return true;
    }
    bool courseFinalIconPrefixCurrent(std::size_t i)const noexcept {
        if(i<81||i>83||!courseFinalTextPrefixCurrent(81))return false;
        for(std::size_t previous=81;previous<i;++previous){const auto& row=courseIconRows_[previous];
            if(!courseWidgetSlotsKnown_[previous]||!row.constructed||!row.loaded||!row.owner||
               construction_->courseIcons_[previous]!=&row.owner->entity||!construction_->courseIconsAttached_[previous]||
               sourceWidgetTable154_[previous]!=&row.owner->entity)return false;
        }
        return true;
    }
    bool loadNextCourseIconWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           !courseIconOrdinalAdmitted(courseWidgetOrdinal_)||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !issuer_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row)return stop(0x1CD8A0);
        const auto i=courseWidgetOrdinal_;
        if(i>=81){if(!courseFinalIconPrefixCurrent(i))return stop(0x1CD8A0);}
        else if(i>=64&&!courseStaticIconPrefixCurrent(i))return stop(0x1CD8A0);
        if(!loadCourseIcon1CD160(i,*row,d))return stop(d.sourcePC);
        C* entity=courseIconEntity(i);if(!entity)return stop(0x1CD8A8);
        for(std::size_t input=0;input<2;++input){std::int32_t wanted=0;
            const char* key=input?"WantsJoystickInput2":"WantsJoystickInput1";
            if(row->find(key)||!assets::originalUiInteger50KG(*row,key,0,wanted,d.config,0x1CD8E0))return stop(0x1CD8E0);
            if(input)courseIconRows_[i].owner->field54=wanted;else courseIconRows_[i].owner->field50=wanted;
        }
        d.entity=construction_->disableCourseIcon1415A0(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        if(!sourceWidgetTable154_)return stop(0x1CD918);
        sourceWidgetTable154_[i]=entity;courseWidgetSlotsKnown_[i]=true;
        d.entity=construction_->attachCourseIcon1CD9F8(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        // Actual1CDA20/28 loop exhaustion precedes any next section formatting.
        // Stack Config destructor1573B0(-1) at1CDA34 remains a real owner boundary;
        // this is final factory success, NOT1CD7A0 return or full manager load.
        if(courseWidgetOrdinal_==84){loaderStopPC_=d.sourcePC=0x1CDA34;return true;}
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;loaderStopPC_=d.sourcePC=0x1CD8A0;return true;
    }
    bool loadNextCourseSelectableWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           !courseSelectableOrdinalAdmitted(courseWidgetOrdinal_)||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !issuer_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        const auto i=courseWidgetOrdinal_;
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row)return stop(0x1CD8A0);
        if(i>=52&&!courseLateSelectablePrefixCurrent(i))return stop(0x1CD8A0);
        if(!loadCourseSelectable1CD160(i,*row,d))return stop(d.sourcePC);
        C* entity=courseSelectableEntity(i);if(!entity)return stop(0x1CD8A8);
        for(std::size_t input=0;input<2;++input){std::int32_t wanted=0;
            const char* key=input?"WantsJoystickInput2":"WantsJoystickInput1";
            if(row->find(key)||!assets::originalUiInteger50KG(*row,key,0,wanted,d.config,0x1CD8E0))return stop(0x1CD8E0);
            if(input)courseSelectableRows_[i]->field54=wanted;else courseSelectableRows_[i]->field50=wanted;
        }
        d.entity=construction_->disableCourseSelectable1415A0(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        if(!sourceWidgetTable154_)return stop(0x1CD918);
        sourceWidgetTable154_[i]=entity;courseWidgetSlotsKnown_[i]=true;
        d.entity=construction_->attachCourseSelectable1CD9F8(i,*entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;loaderStopPC_=d.sourcePC=0x1CD8A0;
        return true; // Next genuine factory; no v98 completion or unsupported row success.
    }
    bool loadNextCourseSoultreeWidget1CD8A0(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CD8A0;
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CD8A0||
           !courseSoultreeOrdinalAdmitted(courseWidgetOrdinal_)||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           !issuer_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        const auto i=courseWidgetOrdinal_;
        if(!courseSoultreePrefixCurrent(i))return stop(0x1CD8A0);
        if(!courseWidgetsConfig_->selectedCurrent(courseWidgetSectionStack70_))return stop(0x1CD878);
        const auto* row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row)return stop(0x1CD8A0);
        if(!loadCourseSoultree1CD160(i,*row,d))return stop(d.sourcePC);
        // Actual1CD36C Search/Observer28E8A0 belongs to the factory BEFORE it
        // returns the widget. A loaded animation/model alone is not a return.
        if(!attachCourseModelObserver28E8A0(i,d))return stop(d.sourcePC);
        if(!courseSoultreeRows_[i]||!courseSoultreeRows_[i]->constructed||!courseSoultreeRows_[i]->loaderReturned)return stop(0x1CD780);
        auto& entity=courseSoultreeRows_[i]->entity;
        for(std::size_t input=0;input<2;++input){std::int32_t wanted=0;
            const char* key=input?"WantsJoystickInput2":"WantsJoystickInput1";
            if(row->find(key)||!assets::originalUiInteger50KG(*row,key,0,wanted,d.config,0x1CD8E0))return stop(0x1CD8E0);
            if(input)courseSoultreeRows_[i]->field54=wanted;else courseSoultreeRows_[i]->field50=wanted;
        }
        d.entity=construction_->disableCourseSoultree1415A0(i,entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        if(!sourceWidgetTable154_)return stop(0x1CD918);
        sourceWidgetTable154_[i]=&entity;courseWidgetSlotsKnown_[i]=true;
        d.entity=construction_->attachCourseSoultree1CD9F8(i,entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        ++courseWidgetOrdinal_;courseWidgetOnTopKnown_=false;
        try {courseWidgetSectionStack70_="Widget "+std::to_string(courseWidgetOrdinal_+1);}
        catch(...){return stop(0x1CD86C);}
        if(!courseWidgetsConfig_->selectWidgetsSection157D00(courseWidgetSectionStack70_,d.config))return stop(d.config.sourcePC?d.config.sourcePC:0x1CD878);
        row=courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_);
        if(!row||!assets::originalUiInteger50KG(*row,"OnTop",0,courseWidgetOnTopStack64C_,d.config,0x1CD890))return stop(0x1CD890);
        courseWidgetOnTopKnown_=true;loaderStopPC_=d.sourcePC=0x1CD8A0;
        return true; // Exact authenticated Soultree cohort only; Widget27 Overlay_Text remains unjoined; no full84-row v98 completion.
    }
    bool completeCourseWidgets1CDA34(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CDA34;
        if(thread_!=std::this_thread::get_id()||terminal_||courseWidgetsReturned_||loaderStopPC_!=0x1CDA34||
           !issuer_||!construction_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           !courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||!courseWidgetsConfig_->selectedCurrent("Widget 84")||
           courseWidgetOrdinal_!=84||!courseWidgetCount158_||*courseWidgetCount158_!=84||
           !courseFinalIconPrefixCurrent(83)||!courseWidgetSlotsKnown_[83]||!courseIconRows_[83].owner||
           !courseIconRows_[83].loaded||!construction_->courseIconsAttached_[83]||
           sourceWidgetTable154_[83]!=&courseIconRows_[83].owner->entity)return false;
        if(!courseWidgetsConfig_->closeWidgetsStack1573B0(-1,d.config)||!courseWidgetsConfig_->closedWidgetsStack1573B0()){
            terminal_=true;loaderStopPC_=d.sourcePC=d.config.sourcePC?d.config.sourcePC:0x1CDA34;return false;
        }
        courseWidgetsReturned_=true; //1CDA3C actual return1 only AFTER stack Config retirement.
        loaderStopPC_=d.sourcePC=0x1CCDDC;return true; // Caller true branch, before actual Fog attachment.
    }
    bool abortNativeCourseWidgetsConfig()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        // Exact owned source widget entities/resources must be withdrawn first
        // once this currently-empty table receives actual published slots.
        if(!courseWidgetTableNativeReleased_){
            for(std::size_t i=0;i<courseWidgetSlotsKnown_.size();++i)if(courseWidgetSlotsKnown_[i]){
                if(courseOverlayTextOrdinalAdmitted(i)){if(!construction_||construction_->courseOverlayOuters_[i])return false;}
                else if(i==4){if(!construction_||construction_->courseMovement_)return false;}
                else if(courseIconOrdinalAdmitted(i)){if(!construction_||construction_->courseIcons_[i])return false;}
                else if(courseSelectableOrdinalAdmitted(i)){if(!construction_||construction_->courseSelectableIcons_[i])return false;}
                else if(courseSoultreeOrdinalAdmitted(i)){if(!construction_||construction_->courseSoultreeRows_[i].outer)return false;}
                else if((i!=2&&i!=3&&!courseCohortEmptyOrdinal(i)&&!courseStaticTextEmptyOrdinal(i)&&!courseLateSelectableEmptyOrdinal(i)&&i!=63)||!courseWidgetTableBacking154_||courseWidgetTableBacking154_[i])return false;
            }
            courseWidgetTableBacking154_.reset();courseWidgetTableNativeReleased_=true;
        } // Historical source154 and reached NULL/nonNULL slots are not rewritten.
        if(courseWidgetsConfig_){if(!courseWidgetsConfig_->closeNative())return false;courseWidgetsConfig_.reset();}
        return true;
    }
