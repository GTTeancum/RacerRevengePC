// Actual Page shared prompt resources, entered only after the camera callback
// has returned. No Page factory return, table publication, or draw receipt.
    using CoursePromptQuad=OriginalUiGarageQuad50KG;
    using CoursePromptText=OriginalUiGarageScreenText50KG;
    using CoursePromptDictionary=assets::OriginalApplicationLocalization50KG;
    bool coursePagePromptsAttempted_=false,coursePageText440Loading_=false,coursePageText440Loaded_=false;
    std::size_t coursePagePromptsLoaded_=0;
    std::shared_ptr<const CoursePromptDictionary::SelectedText> coursePageText440Selected_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> coursePagePromptArchive_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> coursePagePromptDomain_;
    std::array<char,260> coursePageBackgroundStack770_{};
    bool coursePageBackgroundKnown_=false;
    bool ownsCoursePageText440(const CoursePromptText& text)const noexcept {
        // Durable resource custody, independent of temporary Config selection,
        // selected LOC borrow, and Popup material proof after loading.
        return thread_==std::this_thread::get_id()&&issuer_&&construction_&&
            construction_->pageAcquired_&&construction_->registrationCurrent()&&
            coursePagePromptsAttempted_&&coursePageText440Loading_&&
            pagePayload_.pinned_&&pagePayload_.displayCurrent()&&pagePayload_.textCount_&&
            pagePayload_.texts_[0].value==&text&&coursePagePromptArchive_&&coursePagePromptArchive_->loaded();
    }
    std::shared_ptr<const assets::OriginalUiArchive50KG> coursePageTextArchive()const noexcept {
        return coursePagePromptArchive_;
    }
    bool coursePageText440InitialCurrent(const CoursePromptText& text,std::string_view supplied)const noexcept {
        if(!ownsCoursePageText440(text)||coursePageText440Loaded_||!issuer_->sourceLocalization32COwner_)return false;
        std::string_view actual;
        return CoursePromptDictionary::SelectedText::BorrowReader::readCurrent(coursePageText440Selected_,*issuer_,
            *issuer_->sourceLocalization32COwner_,89,actual)&&actual.data()==supplied.data()&&actual.size()==supplied.size();
    }
    bool loadCoursePagePromptQuad(std::size_t i,Diagnostic& d)noexcept try {
        constexpr std::array<const char*,4> names{{"icon_x","icon_tri","icon_sqr","icon_cir"}};
        constexpr std::array<std::uint32_t,4> calls{{0x1D682C,0x1D68DC,0x1D6994,0x1D6A4C}};
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        if(i>=4||i!=coursePagePromptsLoaded_||!coursePopup_||!coursePopupHidden_||
           coursePopup_->payload.phase_!=CoursePopup::Phase::loaded||!coursePopup_->payload.current()||
           !pagePayload_.displayCurrent())return stop(i<4?calls[i]:0x1D6AEC);
        auto& q=pagePayload_.quads_[i];const auto domain=coursePagePromptDomain_;
        const auto context=coursePopup_->payload.context_;
        if(!domain||domain!=coursePopup_->payload.domain_||!domain->prepared()||domain!=issuer_->standardMaterialDomainOwner_||
           !context||context!=issuer_->materialContextOwner_||!context->live())return stop(calls[i]);
        q.domain_=domain;q.context_=context;q.video_=video_;q.garage_=coursePagePromptArchive_;
        d.sourcePC=0x2DF0E8;
        try {q.primitiveOwner_=std::make_unique<CoursePromptQuad::Primitive>();}
        catch(...){q.material54=nullptr;q.stage_=CoursePromptQuad::Stage::primitivePublished;return stop(0x2DF114);}
        q.material54=q.primitiveOwner_.get();q.stage_=CoursePromptQuad::Stage::primitivePublished;
        auto& p=*q.material54;p.domain=domain;p.context=context;
        const auto table=domain->materials382B38();
        d.sourcePC=0x299BE0;
        std::string requested=issuer_->materialDirectory36B970_;
        if(requested.empty())return stop(0x299BE0);
        if(requested.back()!='\\')requested+='\\';requested+=names[i];
        if(!assets::OriginalStandardArchiveProvider50LA::sameName(requested,std::string("ui\\textures\\")+names[i]))return stop(0x299BE0);
        std::int32_t row=-1;d.sourcePC=0x299CF8;
        const bool found=table->find298C80(requested,row,d.page.quad.material);
        p.material00=row;p.materialStored=true;
        auto& guard=CoursePromptText::guard8CD8();auto& assertion=CoursePromptText::assertion443640();
        if(!guard.load(std::memory_order_acquire)){assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);}
        const auto rememberUnacquired=[&]()noexcept {
            if(row>=0){pagePayload_.failedPromptRetainPrimitive_[i]=&p;pagePayload_.failedPromptRetainRows_[i]=row;}
        };
        if((assertion.load()&&row==-1)||!found){rememberUnacquired();return stop(0x299CF8);}
        if(!table->retainScreen14D590(row,d.page.quad.material)){
            rememberUnacquired();return stop(d.page.quad.material.sourcePC);
        }
        p.materialRetained=true;p.type0C=15;p.flags0F&=std::uint8_t(~1u);q.stage_=CoursePromptQuad::Stage::materialRetained;
        d.sourcePC=0x2DF144;
        // These are the SAME four canonical icon rows already constructed by
        // this actual live Popup, not a new widget ordinal or Popup role proof.
        const auto& popupQuad=coursePopup_->payload.quads_[i+3];
        assets::OriginalStandardMaterialRowView50LA view;
        if(!popupQuad.material54||!popupQuad.material54->materialRetained||popupQuad.material54->material00!=row||
           !table->row(row,view)||!view.object04||
           !table->preflightCourseLoadedWidget(row,context,domain->textures383400(),d.page.quad.material))return stop(0x2DF144);
        if(!domain->populateCourseResources2DF144(context,d.page.quad.material))return stop(d.page.quad.material.sourcePC);
        q.stage_=CoursePromptQuad::Stage::populated;
        if(!q.completeZeroPosition2DF158(d.page.quad)){
            if(d.page.quad.sourcePC==0x2DF1CC&&q.stage_==CoursePromptQuad::Stage::dimensionsStored&&!q.packetOwner_){
                q.resource58=nullptr;return stop(0x2DF1E0);
            }
            return stop(d.page.quad.sourcePC);
        }
        constexpr std::array<std::uint32_t,4> xs{{0x43F20000,0x43C68000,0x438E8000,0x43200000}};
        const float x=std::bit_cast<float>(xs[i]),y=378.f;
        if(q.position10[0]!=x||q.position10[1]!=y){q.position10[0]=x;q.position10[1]=y;
            q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
        }
        ++coursePagePromptsLoaded_;return true;
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
    bool loadCoursePagePrompts1D682C(Diagnostic& d)noexcept try {
        d={};d.sourcePC=0x1D682C;
        if(thread_!=std::this_thread::get_id()||terminal_||coursePagePromptsAttempted_||loaderStopPC_!=0x1D682C||
           !issuer_||!construction_||!construction_->pageAcquired_||!construction_->registrationCurrent()||
           pagePayload_.phase_!=OriginalUiCoursePagePayload50KG::Phase::derived||
           pagePayload_.quadCount_!=20||pagePayload_.textCount_!=24||!pagePayload_.texts_[0].value||
           !coursePageConfig_||!coursePageConfig_->selectedCurrent("PageInfo")||
           !coursePopup_||!coursePopupHidden_||coursePopup_->payload.phase_!=CoursePopup::Phase::loaded||
           !coursePopup_->payload.current()||std::fegetround()!=FE_TONEAREST)return false;
        coursePagePromptArchive_=coursePopup_->payload.course_;coursePagePromptDomain_=coursePopup_->payload.domain_;coursePagePromptsAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        while(coursePagePromptsLoaded_<4)if(!loadCoursePagePromptQuad(coursePagePromptsLoaded_,d))return false;
        d.sourcePC=0x1D6AFC;
        const auto fonts=issuer_->fonts328Owner_;if(!fonts)return stop(d.sourcePC);
        const auto font=fonts->find282210(assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16);
        // Source permits NULL Font lookup; the actual Text loader owns its
        // first-use boundary. No fabricated default Font replaces it.
        (void)font;
        d.sourcePC=0x1D6B1C;
        const auto localization=issuer_->sourceLocalization32COwner_;if(!localization)return stop(d.sourcePC);
        assets::OriginalApplicationLocalizationDiagnostic50KG ld;
        // Existing real18AC60 owner shares process guard80A0/assertion3D6B20
        // with this inlined source lookup. It is not per-Page state.
        if(!issuer_->borrowLocalizationText18AC60(*localization,89,coursePageText440Selected_,ld))return stop(ld.sourcePC);
        std::string_view text;d.sourcePC=0x1D6BA8;
        if(!CoursePromptDictionary::SelectedText::BorrowReader::readCurrent(coursePageText440Selected_,*issuer_,*localization,89,text))return stop(d.sourcePC);
        coursePageText440Loading_=true;d.sourcePC=0x1D6BB0;
        auto& t=*pagePayload_.texts_[0].value;
        if(!t.loadCoursePageText4402DFD90(*this,text,d.page.text))return stop(d.page.text.sourcePC);
        coursePageText440Selected_.reset();
        if(t.color54_!=0x00CCCCCCu){
            for(std::size_t g=0;g<t.capacity5B_;++g)for(auto& vertex:t.glyphsC_[g].vertices)
                for(std::size_t a=0;a<vertex.aliases10;++a){auto& rgba=vertex.rgba1C[a]->words;rgba[0]=rgba[1]=rgba[2]=102;}
            t.color54_=0x00CCCCCCu;
        }
        if(t.vector10_[0]!=481.f||t.vector10_[1]!=384.f){t.vector10_[0]=481.f;t.vector10_[1]=384.f;t.flags5A_|=3;}
        coursePageText440Loaded_=true;
        d.sourcePC=0x1D6CE8;
        if(!pagePayload_.displayCurrent())return stop(d.sourcePC);
        pagePayload_.display_->sourceClearMode2C_=0; //1D6CEC delay slot, before getter.
        const auto* info=coursePageConfig_->config_->records_.section("PageInfo");
        if(!info||!coursePageConfig_->selectedCurrent("PageInfo"))return stop(d.sourcePC);
        const auto* background=info->find("Background");
        const std::string_view value=background?std::string_view(background->text):std::string_view("*INVALID*");
        if(value.size()>=coursePageBackgroundStack770_.size()||value.find('\0')!=std::string_view::npos)return stop(d.sourcePC);
        std::memcpy(coursePageBackgroundStack770_.data(),value.data(),value.size());
        coursePageBackgroundStack770_[value.size()]=0;coursePageBackgroundKnown_=true;
        std::array<float,3> rgb{};d.sourcePC=0x1D6D1C;
        if(!assets::originalUiTriple50KG(*info,"BackgroundColor",{{0,255,0}},rgb,d.config,d.sourcePC))return stop(d.config.sourcePC);
        if(rgb[0]!=0||rgb[1]!=0||rgb[2]!=0||std::fegetround()!=FE_TONEAREST)return stop(0x1D6D38);
        pagePayload_.shared_.backgroundColor430=0; // Actual three known CVT.W.S,1D6D70.
        loaderStopPC_=d.sourcePC=0x1D6D74;return true;
    }catch(...){terminal_=true;loaderStopPC_=d.sourcePC;return false;}
    bool preflightNativeCoursePagePrompts()const noexcept {
        if(!coursePagePromptsAttempted_)return true;
        if(thread_!=std::this_thread::get_id())return false;
        if(pagePayload_.phase_==OriginalUiCoursePagePayload50KG::Phase::nativeRetired)return true;
        if(!pagePayload_.displayCurrent())return false;
        const auto table=coursePagePromptDomain_?coursePagePromptDomain_->materials382B38():nullptr;
        const auto total=[&](std::int32_t row)noexcept {
            std::uint64_t n=0;
            for(std::size_t i=0;i<std::min<std::size_t>(4,pagePayload_.quadCount_);++i){const auto& q=pagePayload_.quads_[i];
                if(q.stage_!=CoursePromptQuad::Stage::retired&&q.material54&&q.material54->materialRetained&&q.material54->material00==row)++n;
            }
            if(pagePayload_.textCount_&&pagePayload_.texts_[0].value){const auto& t=*pagePayload_.texts_[0].value;
                if(t.pendingPrimitive_&&t.pendingPrimitive_->materialRetained&&t.pendingPrimitive_->material00==row)++n;
                for(std::size_t i=0;i<t.publishedPrimitives_;++i)if(t.primitives4_&&t.primitives4_[i]&&
                    t.primitives4_[i]->materialRetained&&t.primitives4_[i]->material00==row)++n;
            }
            return n;
        };
        const auto releaseValid=[&](std::int32_t row)noexcept {
            assets::OriginalStandardMaterialRowView50LA v;
            return table&&table->canRetireNativeCollector29C300()&&table->row(row,v)&&v.references08&&
                *v.references08>0&&std::uint64_t(*v.references08)>=total(row);
        };
        OriginalUiCoursePagePayload50KG::Diagnostic d;
        for(std::size_t i=0;i<std::min<std::size_t>(4,pagePayload_.quadCount_);++i){const auto& q=pagePayload_.quads_[i];
            if(q.stage_==CoursePromptQuad::Stage::retired)continue;
            if(pagePayload_.failedPromptRetainPrimitive_[i]){
                const auto* p=q.primitiveOwner_.get();
                if(!pagePayload_.failedPromptRetainRows_[i]||!p||p!=pagePayload_.failedPromptRetainPrimitive_[i]||
                   p!=q.material54||!p->materialStored||p->material00!=*pagePayload_.failedPromptRetainRows_[i]||
                   p->material00<0||p->materialRetained||p->optional04||p->optional14||(p->flags0F&1u)||p->packet10||
                   q.resource58||q.packetOwner_||q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
            }else if(pagePayload_.failedPromptRetainRows_[i]||!q.preflightRetirement2DEFE0(d.quad))return false;
            if(q.material54&&q.material54->materialRetained&&
                (q.material54->domain!=coursePagePromptDomain_||!releaseValid(q.material54->material00)))return false;
        }
        if(pagePayload_.textCount_&&pagePayload_.texts_[0].value){const auto& t=*pagePayload_.texts_[0].value;
            if(!t.preflightRetirement2E1340(d.text))return false;
            if((t.pendingPrimitive_||t.publishedPrimitives_)&&t.domain_!=coursePagePromptDomain_)return false;
            if(t.pendingPrimitive_&&t.pendingPrimitive_->materialRetained&&!releaseValid(t.pendingPrimitive_->material00))return false;
            for(std::size_t i=0;i<t.publishedPrimitives_;++i)if(t.primitives4_&&t.primitives4_[i]&&
                t.primitives4_[i]->materialRetained&&!releaseValid(t.primitives4_[i]->material00))return false;
        }
        return true;
    }
    bool prepareNativeCoursePagePromptsAbort()noexcept {
        if(!preflightNativeCoursePagePrompts())return false;
        // Borrow only: native rollback does not erase copied Text data or any
        // reached source scalar. PagePayload later performs actual resources.
        coursePageText440Selected_.reset();return true;
    }
