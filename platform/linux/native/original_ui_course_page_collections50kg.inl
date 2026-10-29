// Actual1D6DAC collections prefix. This is NOT the Garage two-input adapter.
// Eight router lanes are read by2A71A0; source5C has no cold producer on the
// first actual Soultree. Retain Control1's reached widget borrow and stop there.
    struct CoursePageListHeader {
        const void* first00=nullptr;const void* last04=nullptr;
        bool firstKnown=false,lastKnown=false;
    };
    struct CoursePageRouter {
        C entity;const C* manager30=nullptr;const C* page34=nullptr;
        std::unique_ptr<CoursePageListHeader*[]> tableBacking38;
        CoursePageListHeader** source38=nullptr; // Constructor writes genuineNULL.
        std::array<std::unique_ptr<CoursePageListHeader>,8> headers;
        std::array<bool,8> slotsKnown{};
        bool constructed=false,returned=false,attached=false,tableReleased=false;
    };
    struct CoursePageControl {
        C entity;std::int32_t selected30=0,count34=0,field3C=0;
        std::int32_t planet40=-1,courseIndex44=-1;
        std::unique_ptr<C*[]> widgetBacking38;C** source38=nullptr;
        std::array<bool,2> widgetKnown{}; // Only actual Control0/1 admitted here.
        bool constructed=false,returned=false,widgetsReleased=false;
        std::array<char,14> planetStackE8{};bool planetStackKnown=false;
    };
    std::unique_ptr<CoursePageRouter> coursePageRouter_;
    std::array<std::unique_ptr<CoursePageControl>,2> coursePageControls_;
    std::unique_ptr<C*[]> coursePageControlsBacking144_,coursePageGroupsBacking14C_;
    std::array<bool,24> coursePageControlSlotsKnown_{};
    std::unique_ptr<CoursePageListHeader*[]> coursePageExtraBacking150_;
    std::array<std::unique_ptr<CoursePageListHeader>,2> coursePageExtraHeaders_;
    std::array<bool,2> coursePageExtraSlotsKnown_{};
    bool coursePageCollectionsAttempted_=false,coursePageCollectionsReleased_=false;
    bool coursePageControlTableReleased_=false,coursePageGroupTableReleased_=false,coursePageExtraTableReleased_=false;
    std::size_t coursePageControlOrdinal_=0,coursePageRouterScanLane_=0;
    std::size_t coursePageRouterWidgetByteOffset_=0,coursePageRouterSlotByteOffset_=0;
    const C* coursePageRouterScanWidget_=nullptr;
    std::uint32_t coursePageRouterUnknownSourceOffset_=0;
    bool loadCoursePageCollections1D6DAC(Diagnostic& d)noexcept try {
        d.sourcePC=0x1D6DAC;
        if(thread_!=std::this_thread::get_id()||terminal_||coursePageCollectionsAttempted_||
           loaderStopPC_!=0x1D6DAC||!construction_||!construction_->pageAcquired_||
           !construction_->registrationCurrent()||!coursePageConfig_||
           !coursePageConfig_->selectedCurrent("PageInfo"))return false;
        coursePageCollectionsAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        auto& fields=construction_->pageCollections_;
        coursePageRouter_.reset(new(std::nothrow) CoursePageRouter); //1D6DAC malloc40.
        if(!coursePageRouter_)return stop(0x2A7114); // NULL ctor skipped; first loader store.
        auto& router=*coursePageRouter_;
        d.entity=construction_->constructCoursePageRouter2A6FD0(router.entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);
        router.constructed=true;router.page34=construction_->page_.get();router.manager30=&scope_->manager_;
        router.tableBacking38.reset(new(std::nothrow) CoursePageListHeader*[8]);
        router.source38=router.tableBacking38.get(); //2A7128 includesNULL; slots still UNKNOWN.
        for(std::size_t i=0;i<8;++i){
            router.headers[i].reset(new(std::nothrow) CoursePageListHeader); //2A7134, even tableNULL.
            if(!router.source38)return stop(0x2A714C);
            router.source38[i]=router.headers[i].get();router.slotsKnown[i]=true;
            if(!router.headers[i])return stop(0x2A715C);
            router.headers[i]->last04=nullptr;router.headers[i]->lastKnown=true;
            router.headers[i]->first00=nullptr;router.headers[i]->firstKnown=true;
        }
        router.returned=true;fields.router134=&router; //1D6DD8, only after2A7100 return.
        d.entity=construction_->attachCoursePageRouter141960(router.entity);
        if(!d.entity.okay())return stop(d.entity.sourcePC);router.attached=true;
        d.sourcePC=0x1D6DFC;
        const auto* info=coursePageConfig_->config_->records_.section("PageInfo");
        if(!info)return stop(0x1D6DFC);
        if(!assets::originalUiInteger50KG(*info,"NumberOfStaticWidgets",0,fields.staticWidgetCount13C,d.config,0x1D6DFC))return stop(0x1D6DFC);
        d.sourcePC=0x1D6E14;
        if(!assets::originalUiInteger50KG(*info,"NumberOfControls",0,fields.controlCount140,d.config,0x1D6E14))return stop(0x1D6E14);
        if(fields.staticWidgetCount13C!=0||fields.controlCount140!=23)return stop(0x1D6E24);
        coursePageControlsBacking144_.reset(new(std::nothrow) C*[24]);
        fields.controls144=coursePageControlsBacking144_.get(); //1D6E2C, no eager slot initialization.
        d.sourcePC=0x1D6E40;
        if(!assets::originalUiInteger50KG(*info,"NumberOfGroups",0,fields.groupCount148,d.config,0x1D6E40))return stop(0x1D6E40);
        if(fields.groupCount148!=5)return stop(0x1D6E50);
        coursePageGroupsBacking14C_.reset(new(std::nothrow) C*[6]);
        // Group14C store is next allocation's delay slot; both precede150 publication.
        fields.groups14C=coursePageGroupsBacking14C_.get();
        coursePageExtraBacking150_.reset(new(std::nothrow) CoursePageListHeader*[2]);
        fields.extra150=coursePageExtraBacking150_.get(); //1D6E64.
        for(std::size_t i=0;i<2;++i){
            coursePageExtraHeaders_[i].reset(new(std::nothrow) CoursePageListHeader); //1D6E70.
            if(!coursePageExtraBacking150_)return stop(0x1D6E88);
            coursePageExtraBacking150_[i]=coursePageExtraHeaders_[i].get();coursePageExtraSlotsKnown_[i]=true;
            if(!coursePageExtraHeaders_[i])return stop(0x1D6E98);
            coursePageExtraHeaders_[i]->last04=nullptr;coursePageExtraHeaders_[i]->lastKnown=true;
            coursePageExtraHeaders_[i]->first00=nullptr;coursePageExtraHeaders_[i]->firstKnown=true;
        }
        // Actual caller visits Control0, then1. No synthetic index-zero object.
        for(std::size_t i=0;i<2;++i){
            coursePageControlOrdinal_=i;
            coursePageControls_[i].reset(new(std::nothrow) CoursePageControl); //1DA258.
            if(coursePageControls_[i]){
                d.entity=construction_->constructCoursePageControl1D5DB0(i,coursePageControls_[i]->entity);
                if(!d.entity.okay())return stop(d.entity.sourcePC);coursePageControls_[i]->constructed=true;
            }
            const char* sectionName=i?"Control 1":"Control 0"; //Actual308C90 bounded ordinal.
            d.sourcePC=0x1DA294;
            if(!coursePageConfig_->selectWidgetsSection157D00(sectionName,d.config))return stop(d.config.sourcePC);
            if(!coursePageControls_[i])return stop(0x1DA29C); // Selection effects precedeNULL outer store.
            auto& control=*coursePageControls_[i];control.selected30=1;
            d.sourcePC=0x1DA2BC;
            const auto* section=coursePageConfig_->config_->records_.section(sectionName);
            if(!section||!coursePageConfig_->selectedCurrent(sectionName))return stop(0x1DA2BC);
            std::int32_t count=0;
            if(!assets::originalUiInteger50KG(*section,"NumberOfWidgets",0,count,d.config,0x1DA2BC))return stop(0x1DA2BC);
            control.count34=count; //1DA2C8.
            if(count!=(i?2:0))return stop(0x1DA2D0);
            if(count>0){
                control.widgetBacking38.reset(new(std::nothrow) C*[2]);control.source38=control.widgetBacking38.get(); //1DA2E0.
                std::int32_t widgetId=0;
                d.sourcePC=0x1DA310;
                if(!assets::originalUiInteger50KG(*section,"WidgetID1",0,widgetId,d.config,0x1DA310))return stop(0x1DA310);
                if(widgetId!=9||!sourceWidgetTable154Known_||courseWidgetTableNativeReleased_||
                   !sourceWidgetTable154_||!courseWidgetSlotsKnown_[8]||!courseSoultreeRows_[8]||
                   !courseSoultreeRows_[8]->constructed||!courseSoultreeRows_[8]->loaderReturned||
                   sourceWidgetTable154_[8]!=&courseSoultreeRows_[8]->entity)return stop(0x1DA334);
                auto* widget=sourceWidgetTable154_[8];
                if(!control.source38)return stop(0x1DA338);
                control.source38[0]=widget;control.widgetKnown[0]=true; // Borrow BEFORE router call.
                coursePageRouterScanWidget_=widget;coursePageRouterScanLane_=0;
                coursePageRouterWidgetByteOffset_=coursePageRouterSlotByteOffset_=0;
                const auto& source=*courseSoultreeRows_[8];
                const std::array<std::uint32_t,3> known{{std::uint32_t(source.field50),std::uint32_t(source.field54),source.field58}};
                for(const auto value:known){
                    if(value)return stop(0x2A71E0); // No invented membership for a different source cohort.
                    ++coursePageRouterScanLane_; //2A71DC zero branch delay.
                    coursePageRouterWidgetByteOffset_+=4;coursePageRouterSlotByteOffset_+=4; //2A7260/268.
                }
                coursePageRouterUnknownSourceOffset_=0x50+std::uint32_t(coursePageRouterWidgetByteOffset_);
                return stop(0x2A71D4); // GenuineUNKNOWN5C; do not skip real animation60.
            }
            d.sourcePC=0x1DA380;
            if(!assets::originalUiInteger50KG(*section,"CourseIndex",-1,control.courseIndex44,d.config,0x1DA380))return stop(0x1DA380);
            // Authenticated Control0 lacks Planet. Actual158240 text missing-key
            // suffix1588DC..E8 copies literal empty3A6288; no comparison matches.
            d.sourcePC=0x1DA398;
            if(section->find("Planet")||control.courseIndex44!=-1)return stop(0x1DA398);
            control.planetStackE8[0]=0;control.planetStackKnown=true;
            control.returned=true;
            if(!coursePageControlsBacking144_)return stop(0x1D6EEC);
            coursePageControlsBacking144_[i]=&control.entity;coursePageControlSlotsKnown_[i]=true;
        }
        return stop(0x1D6EC4); // Never reached by admitted exact Control1; no group/fullPage receipt.
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
    bool preflightNativeCoursePageCollections()const noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(coursePageCollectionsReleased_)return true;
        if(!coursePageCollectionsAttempted_)return !coursePageRouter_&&!coursePageControls_[0]&&!coursePageControls_[1];
        if(!construction_||!construction_->pageAcquired_||!construction_->registrationCurrent())return false;
        const auto& fields=construction_->pageCollections_;
        if(!coursePageControlTableReleased_&&fields.controls144!=coursePageControlsBacking144_.get())return false;
        if(!coursePageGroupTableReleased_&&fields.groups14C!=coursePageGroupsBacking14C_.get())return false;
        if(!coursePageExtraTableReleased_&&fields.extra150!=coursePageExtraBacking150_.get())return false;
        for(std::size_t i=1;i<coursePageControlSlotsKnown_.size();++i)if(coursePageControlSlotsKnown_[i])return false;
        for(std::size_t i=0;i<2;++i)if(coursePageControls_[i]){
            const auto& c=*coursePageControls_[i];
            if(!c.widgetsReleased&&c.source38!=c.widgetBacking38.get())return false;
            if(coursePageControlSlotsKnown_[i]&&(!c.returned||!coursePageControlsBacking144_||
               coursePageControlsBacking144_[i]!=&c.entity))return false;
            if(c.constructed&&!construction_->preflightNativeCoursePageControl(i,c.entity).okay())return false;
            if(!c.widgetsReleased&&c.widgetKnown[0]){
                if(i!=1||!c.widgetBacking38||!courseSoultreeRows_[8]||
                   c.widgetBacking38[0]!=&courseSoultreeRows_[8]->entity||
                   coursePageRouterScanWidget_!=c.widgetBacking38[0])return false;
            }
            if(c.widgetKnown[1])return false; // No fabricated second widget at blocked first append.
        }
        if(coursePageRouter_){const auto& r=*coursePageRouter_;
            if((r.returned&&fields.router134!=&r)||(!r.returned&&fields.router134))return false;
            if(!r.tableReleased&&r.source38!=r.tableBacking38.get())return false;
            if(r.constructed&&!construction_->preflightNativeCoursePageRouter(r.entity).okay())return false;
            for(std::size_t i=0;i<8;++i)if(r.headers[i]){
                const auto& h=*r.headers[i];if((h.firstKnown&&h.first00)||(h.lastKnown&&h.last04))return false;
                if(r.slotsKnown[i]&&(!r.tableBacking38||r.tableBacking38[i]!=r.headers[i].get()))return false;
            }
        }
        for(std::size_t i=0;i<2;++i)if(coursePageExtraHeaders_[i]){
            const auto& h=*coursePageExtraHeaders_[i];if((h.firstKnown&&h.first00)||(h.lastKnown&&h.last04))return false;
            if(coursePageExtraSlotsKnown_[i]&&(!coursePageExtraBacking150_||coursePageExtraBacking150_[i]!=coursePageExtraHeaders_[i].get()))return false;
        }
        return true;
    }
    bool abortNativeCoursePageCollections()noexcept {
        if(!preflightNativeCoursePageCollections())return false;
        if(coursePageCollectionsReleased_||!coursePageCollectionsAttempted_)return true;
        // Native partial unwind, not2A5CF0 receipt. All fallible topology/reference
        // gates precede any backing release. Raw borrowed widgets remain alive.
        for(std::size_t reverse=2;reverse;--reverse){const auto i=reverse-1;
            if(!coursePageControls_[i])continue;auto& c=*coursePageControls_[i];
            if(!c.widgetsReleased){c.widgetBacking38.reset();c.widgetsReleased=true;}
            if(c.constructed){if(!construction_->abortNativeCoursePageControl(i,c.entity).okay())return false;c.constructed=false;}
            coursePageControls_[i].reset(); // Source144 slot history is not a live pointer after this point.
        }
        if(!coursePageControlTableReleased_){coursePageControlsBacking144_.reset();coursePageControlTableReleased_=true;}
        if(!coursePageGroupTableReleased_){coursePageGroupsBacking14C_.reset();coursePageGroupTableReleased_=true;} // No group constructed.
        if(!coursePageExtraTableReleased_){for(auto& h:coursePageExtraHeaders_)h.reset();coursePageExtraBacking150_.reset();coursePageExtraTableReleased_=true;}
        if(coursePageRouter_){auto& r=*coursePageRouter_;
            if(!r.tableReleased){for(auto& h:r.headers)h.reset();r.tableBacking38.reset();r.tableReleased=true;}
            if(r.constructed){if(!construction_->abortNativeCoursePageRouter(r.entity).okay())return false;r.constructed=false;r.attached=false;}
            coursePageRouter_.reset();
        }
        coursePageCollectionsReleased_=true;return true;
    }
