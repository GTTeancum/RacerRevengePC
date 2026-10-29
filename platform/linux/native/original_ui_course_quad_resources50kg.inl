// Private actual Course widget Quad resources. No Garage RootInputs admission.
    using CourseQuad=OriginalUiGarageQuad50KG;
    struct CourseWidgetQuad {
        std::unique_ptr<CourseQuad> backing;
        std::shared_ptr<const int> occurrence;
        CourseQuad::Primitive* failedRetainPrimitive=nullptr;
        std::int32_t failedRetainRow=-1;
        bool constructed=false,loadAttempted=false,loaded=false,retired=false;
    };
    static const char* courseQuadTexture(std::size_t ordinal,std::size_t slot)noexcept {
        if(ordinal==5&&slot==0)return "nameblur";
        if(((ordinal>=64&&ordinal<=66)||(ordinal>=81&&ordinal<=83))&&slot==0)return "overlay1";
        if(ordinal==6&&slot<2)return slot?"arrwlefttop_d":"arrwlefttop";
        if(ordinal==7&&slot<2)return slot?"arrwrghttop_d":"arrwrghttop";
        if((ordinal==52||ordinal==54)&&slot<2)return slot?"arrwleft_d":"arrwleft";
        if((ordinal==53||ordinal==55)&&slot<2)return slot?"arrwrght_d":"arrwrght";
        return nullptr;
    }
    CourseWidgetQuad* courseWidgetQuadOccurrence(std::size_t ordinal,std::size_t slot)noexcept {
        if(courseIconOrdinalAdmitted(ordinal)&&slot==0)return &courseIconRows_[ordinal].quad;
        return courseSelectableQuadOccurrence(ordinal,slot);
    }
    // For embedded Selectable wrappers this is native backing admission BEFORE
    // any source outer constructor. For Icon it admits only non-source token;
    // real malloc60 is represented later at2AC68C by construct below.
    bool admitCourseWidgetQuadNative(CourseWidgetQuad& state,bool embedded=true)noexcept {
        if(state.occurrence||state.backing||state.constructed)return false;
        try {
            state.occurrence=std::make_shared<const int>(0);
            if(embedded)state.backing=std::make_unique<CourseQuad>();
            return true;
        }catch(...){state.backing.reset();state.occurrence.reset();return false;}
    }
    bool constructCourseWidgetQuad2DEF40(CourseWidgetQuad& state,std::size_t ordinal,
        std::size_t slot,Diagnostic& d)noexcept {
        if(courseWidgetQuadOccurrence(ordinal,slot)!=&state||!courseQuadTexture(ordinal,slot)||
           !state.occurrence||state.constructed||state.retired){d.sourcePC=0x2DEF40;return false;}
        if(!state.backing){
            // Icon only: actual malloc60 may returnNULL. Constructor skipped;
            // stop BEFORE calling loader withNULL, preserving widget60=NULL.
            if(!courseIconOrdinalAdmitted(ordinal)){d.sourcePC=0x2DEF40;return false;}
            state.backing.reset(new(std::nothrow) CourseQuad);
            if(!state.backing){d.sourcePC=0x2AC6B0;return false;}
        }
        auto& q=*state.backing;
        if(!q.initializeColdPrefix2DEF40(slot?0xC0:0x60,false)){d.sourcePC=0x2DEF40;return false;}
        q.coursePrefixOnly_=true;state.constructed=true;return true;
    }
    bool courseWidgetQuadInputsCurrent()const noexcept {
        return courseFogDisplayPinned_&&courseFogDisplay_==loaderDisplay_&&issuer_&&construction_&&construction_->registrationCurrent()&&loaderConfig_&&loaderConfig_->parsedCurrent()&&
            issuer_->standardMaterialDomainOwner_&&issuer_->standardMaterialDomainOwner_->prepared()&&
            issuer_->materialContextOwner_&&issuer_->materialContextOwner_->live()&&issuer_->materialApplication&&
            issuer_->materialContextOwner_->materialApplication()==issuer_->materialApplication&&
            issuer_->materialContextOwner_->applicationAnimationClock().sameOwner(issuer_->animationClock())&&
            issuer_->standardMaterialDomainOwner_->materials382B38()->screenApplicationMatches(*issuer_->materialApplication)&&
            video_&&video_->originalMain&&video_->packetZMaximum==0xffffff&&
            (issuer_->materialDirectory36B970_=="ui\\textures\\"||issuer_->materialDirectory36B970_=="ui\\textures"||
             issuer_->materialDirectory36B970_=="UI\\Textures\\")&&
            std::fegetround()==FE_TONEAREST;
    }
    bool loadCourseWidgetQuad2DF0B0(CourseWidgetQuad& state,std::size_t ordinal,std::size_t slot,
        const std::string& texture,Diagnostic& d)noexcept {
        auto& qd=d.page.quad;qd={};
        const auto fail=[&](std::uint32_t pc)noexcept {d.sourcePC=pc;return false;};
        const auto* expected=courseQuadTexture(ordinal,slot);
        if(thread_!=std::this_thread::get_id()||courseWidgetQuadOccurrence(ordinal,slot)!=&state||
           !expected||texture!=expected||!state.occurrence||!state.backing||!state.constructed||
           state.loadAttempted||state.retired||!courseWidgetQuadInputsCurrent())return fail(0x2DF0B0);
        state.loadAttempted=true;auto& q=*state.backing;
        q.domain_=issuer_->standardMaterialDomainOwner_;q.context_=issuer_->materialContextOwner_;
        q.video_=video_;q.garage_=loaderConfig_->archive_; // Resource lifetime only, not Garage authority.
        try {q.primitiveOwner_=std::make_unique<CourseQuad::Primitive>();}catch(...){
            q.material54=nullptr;q.stage_=CourseQuad::Stage::primitivePublished;
            return fail(0x2DF114); // Actual2DF100 NULL publication, before vtable dereference.
        }
        q.material54=q.primitiveOwner_.get();q.stage_=CourseQuad::Stage::primitivePublished;
        auto& p=*q.material54;p.domain=q.domain_;p.context=q.context_;
        const auto table=q.domain_->materials382B38();const auto provider=q.domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return fail(0x2465B0);
        std::string requested;
        try {requested=issuer_->materialDirectory36B970_;if(requested.back()!='\\')requested+='\\';requested+=texture;}
        catch(...){return fail(0x299BE0);}
        std::int32_t row=-1;const bool found=table->find298C80(requested,row,qd.material);
        p.material00=row;p.materialStored=true;
        auto& guard=OriginalUiGarageScreenText50KG::guard8CD8();
        auto& assertion=OriginalUiGarageScreenText50KG::assertion443640();
        if(!guard.load(std::memory_order_acquire)){assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);}
        if((assertion.load()&&row==-1)||!found){
            if(row>=0){state.failedRetainPrimitive=&p;state.failedRetainRow=row;}
            return fail(0x299CF8);
        }
        if(!table->retainScreen14D590(row,qd.material)){
            state.failedRetainPrimitive=&p;state.failedRetainRow=row;
            return fail(qd.material.sourcePC?qd.material.sourcePC:0x14D590);
        }
        p.materialRetained=true;p.type0C=15;p.flags0F&=std::uint8_t(~1u);q.stage_=CourseQuad::Stage::materialRetained;
        try {
            assets::OriginalStandardMaterialRowView50LA retained;
            if(!table->row(row,retained))return fail(0x298E60);
            std::vector<std::shared_ptr<const assets::OriginalStandardMaterialSource50LA>> sources;
            std::vector<assets::OriginalStandardTextureSourceInput50LA> textures;
            if(retained.object04&&!table->preflightCourseLoadedWidget(row,q.context_,q.domain_->textures383400(),qd.material))
                return fail(0x2DF144);
            if(!retained.object04){
                // Preserve actual provider selection: positive PSM enters the
                // genuine constructor source path, never the absent fallback.
                using Provider=assets::OriginalStandardArchiveProvider50LA;
                Provider::MemberView psm,pst;
                const auto selected=provider->lookup132820(requested+".psm",psm);
                if(selected==Provider::Lookup::present){
                    if(!prepareCourseAuthoredWidgetSources(state,ordinal,slot,requested,row,psm,sources,textures,d))return false;
                }else{
                    if(selected!=Provider::Lookup::absent||
                       provider->lookup132820(requested+".pst",pst)!=Provider::Lookup::present||!pst.member||!pst.data)
                        return fail(0x29B638);
                    std::shared_ptr<assets::OriginalCourseScreenFallback50KG> proof(new assets::OriginalCourseScreenFallback50KG);
                    proof->occurrence_=state.occurrence;proof->row_=row;proof->name_="ui\\textures\\"+texture;
                    proof->course_=loaderConfig_->archive_;proof->provider_=provider;proof->context_=q.context_;
                    proof->textures_=q.domain_->textures383400();
                    if(!table->bindCourseScreenFallbackAfterRetain(row,proof,qd.material))return fail(qd.material.sourcePC);
                    textures.push_back({*pst.member,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
                }
            } // Already loaded object is reused with its original provenance.
            if(!q.domain_->appendOriginalSources(sources,textures,qd.material))return fail(qd.material.sourcePC);
            // Actual whole pending-set14D370/29E340. Missing other required
            // sources remain fallible; no bank/collector completion invented.
            if(!q.domain_->populateCourseResources2DF144(q.context_,qd.material))
                return fail(qd.material.sourcePC?qd.material.sourcePC:0x2DF144);
            q.stage_=CourseQuad::Stage::populated;
            if(!q.completeZeroPosition2DF158(qd)){
                if(qd.status==CourseQuad::Status::allocation&&qd.sourcePC==0x2DF1CC&&
                   q.stage_==CourseQuad::Stage::dimensionsStored&&!q.packetOwner_){
                    q.resource58=nullptr; // Actual2DF1D4 NULL store; stop before memset2DF1E0.
                    return fail(0x2DF1E0);
                }
                return fail(qd.sourcePC);
            }
            state.loaded=true;d.sourcePC=0x2DF54C;return true;
        }catch(...){return fail(0x2DF144);}
    }
    bool preflightCourseWidgetQuadNative(const CourseWidgetQuad& state)const noexcept {
        if(!state.backing||state.retired)return true;
        const auto& q=*state.backing;if(!state.constructed)return !q.constructed;
        if(state.failedRetainPrimitive){
            const auto* p=q.primitiveOwner_.get();
            return p&&p==state.failedRetainPrimitive&&p==q.material54&&state.failedRetainRow>=0&&
                p->materialStored&&p->material00==state.failedRetainRow&&!p->materialRetained&&
                !p->optional04&&!p->optional14&&!(p->flags0F&1u)&&!p->packet10&&
                !q.resource58&&!q.packetOwner_&&!q.nativeGarageFifoPins_&&!q.nativeMenuWarningPins_;
        }
        CourseQuad::Diagnostic qd;return q.preflightRetirement2DEFE0(qd);
    }
    bool abortCourseWidgetQuadNative(CourseWidgetQuad& state)noexcept {
        if(!preflightCourseWidgetQuadNative(state))return false;
        if(!state.backing||state.retired){state.occurrence.reset();return true;}
        auto& q=*state.backing;
        if(!state.constructed){state.backing.reset();state.occurrence.reset();state.retired=true;return true;}
        if(state.failedRetainPrimitive){
            // Exact actual stored-but-unacquired failure: native backing abort,
            // NOT normal246260 and NEVER a release of an unacquired reference.
            q.primitiveOwner_.reset();q.constructed=false;q.stage_=CourseQuad::Stage::retired;
            state.failedRetainPrimitive=nullptr;
        }else{
            if(!loaderDisplay_||loaderDisplay_->application_!=issuer_||loaderDisplay_->video_!=video_||
               !loaderDisplay_->memoryCardConstructorDrainCurrent50KG())return false;
            CourseQuad::Diagnostic qd;VehicleWindowDiagnostic50DS rd;
            if(!q.retirementDrainPhase_){std::int32_t actualV0=0;
                const auto r=game::serviceOriginalRenderer244D60(loaderDisplay_->dispatch_,true,loaderDisplay_->renderer_,rd,
                    actualV0,game::OriginalCameraDrainPolicy::initializedContext);
                if(r.status!=game::OriginalCameraStartStatus::complete)return false;q.retirementDrainPhase_=1;
            }
            if(q.retirementDrainPhase_==1){
                if(loaderDisplay_->dispatch_.pendingCallbackDC||loaderDisplay_->dispatch_.count54||loaderDisplay_->dispatch_.count60||
                   !loaderDisplay_->renderer_.finishInitializedContextWork(rd)||!rd.contextWorkCompleted)return false;
                q.retirementDrainPhase_=2;
            }
            if(!q.retireResources2DF038(qd))return false;
        }
        state.backing.reset();state.occurrence.reset();state.retired=true;return true;
    }
