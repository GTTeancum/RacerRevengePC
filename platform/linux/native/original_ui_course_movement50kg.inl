// Private actual Course Widget5 Movement_Control. No Garage capability or resources.
    struct CourseMovement {
        C entity;
        const C* manager30=nullptr;
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0,field4C=0;
        std::int32_t field50=0,field54=0;
        std::uint32_t field58=0;
        std::int32_t grid60=0;
        std::array<std::int32_t,16> mappings64{};
        // Source5C and A4 have no constructor/loader producer: intentionally absent.
        bool constructed=false,loaded=false;
    };
    std::unique_ptr<CourseMovement> courseMovement_;
    bool courseMovementAttempted_=false;
    C* courseMovementEntity()noexcept {
        return courseMovement_&&courseMovement_->constructed?&courseMovement_->entity:nullptr;
    }
    bool loadCourseMovement1CD160(std::size_t i,const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD500;
        if(thread_!=std::this_thread::get_id()||terminal_||i!=4||courseMovementAttempted_||
           !issuer_||!scope_||!construction_||!modeEffect_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||courseWidgetOrdinal_!=i||
           loaderStopPC_!=0x1CD8A0||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_!=0||
           courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_)!=&section||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        const auto* type=section.find("Type");if(!type||type->text!="Movement_Control")return false;
        courseMovementAttempted_=true;
        courseMovement_.reset(new(std::nothrow) CourseMovement); // Actual1CD514 allocation.
        if(!courseMovement_)return fail(0x2AB020); // NULL skips ctor, still calls2AB010; first write faults.
        auto& w=*courseMovement_;
        d.entity=construction_->constructCourseMovement2AAF40(w.entity);
        if(!d.entity.okay())return fail(d.entity.sourcePC);
        w.constructed=true;
        // Actual2A7760 stores, then2AAF5C concrete3D4000 dispatch and2AAF60..84 zeros.
        w.manager30=&scope_->manager_;w.field38=0;w.type34=0;w.field3C=0;w.field40=0;
        w.field44=0;w.field48=0;w.field4C=0;w.field50=0;w.field54=0;w.field58=0;w.grid60=0;
        for(std::size_t n=0;n<16;n+=2){w.mappings64[n]=0;w.mappings64[n+1]=0;}
        w.type34=9; //2AB020, BEFORE redundant manager borrow2AB02C.
        w.manager30=&scope_->manager_;
        if(!assets::originalUiInteger50KG(section,"GridSize",0,w.grid60,d.config,0x2AB040))return fail(0x2AB040);
        static constexpr const char* keys[]{"Lpad_Left","Lpad_Right","Lpad_Up","Lpad_Down",
            "Rpad_Down","Rpad_Left","Rpad_Up","Rpad_Right","Start","Select","L1","R1","L2","R2","L3","R3"};
        static constexpr std::uint32_t pcs[]{0x2AB058,0x2AB070,0x2AB088,0x2AB0A0,0x2AB0B8,0x2AB0D0,
            0x2AB0E8,0x2AB100,0x2AB118,0x2AB130,0x2AB148,0x2AB160,0x2AB178,0x2AB190,0x2AB1A8,0x2AB1C0};
        for(std::size_t n=0;n<16;++n)
            if(!assets::originalUiInteger50KG(section,keys[n],0,w.mappings64[n],d.config,pcs[n]))return fail(pcs[n]);
        // No base UIWidget loader, resource registration, input processing or source5C/A4 read.
        w.loaded=true;d.sourcePC=0x1CD8A8;return true;
    }
    bool abortNativeCourseMovement()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!courseMovement_)return true;
        auto& w=*courseMovement_;
        if(w.constructed){
            // Refuse BEFORE releasing backing if any actual acquired Entity is externally borrowed.
            if(!construction_||construction_->pageAcquired_||!construction_->registrationCurrent()||
               construction_->courseMovement_!=&w.entity||w.manager30!=&scope_->manager_)return false;
            const auto r=construction_->abortNativeCourseMovement(w.entity);if(!r.okay())return false;
            w.constructed=false;
        }
        courseMovement_.reset();return true; // Native unwind only; no source destructor-return certificate.
    }
