// Private exact Course Widgets7/8 and53..56; two distinct actual embedded Quads per row.
    struct CourseSelectable {
        C entity;
        const C* manager30=nullptr;
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0,field4C=0;
        std::int32_t field50=0,field54=0;
        std::uint32_t field58=0;
        std::array<CourseWidgetQuad,2> quads;
        bool constructed=false,loaded=false;
        // No source5C producer; safe native storage is not source knowledge.
    };
    std::array<std::unique_ptr<CourseSelectable>,84> courseSelectableRows_;
    std::array<bool,84> courseSelectableAttempted_{};
    static bool courseSelectableOrdinalAdmitted(std::size_t i)noexcept {
        return i==6||i==7||i==52||i==53||i==54||i==55;
    }
    C* courseSelectableEntity(std::size_t i)noexcept {
        return courseSelectableOrdinalAdmitted(i)&&courseSelectableRows_[i]&&courseSelectableRows_[i]->constructed?
            &courseSelectableRows_[i]->entity:nullptr;
    }
    CourseWidgetQuad* courseSelectableQuadOccurrence(std::size_t i,std::size_t slot)noexcept {
        return courseSelectableOrdinalAdmitted(i)&&slot<2&&courseSelectableRows_[i]?&courseSelectableRows_[i]->quads[slot]:nullptr;
    }
    bool courseSelectableQuadSuffix(std::size_t i,std::size_t slot,
        const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        auto* state=courseSelectableQuadOccurrence(i,slot);
        if(!state||!state->backing)return fail(slot?0x2AD734:0x2AD0DC);
        auto& q=*state->backing;
        if(!q.resource58||!q.canMutatePacket2DF550()||!video_||!video_->originalMain||
           video_->packetZMaximum!=0xffffff||std::fegetround()!=FE_TONEAREST)return fail(slot?0x2AD734:0x2AD0DC);
        // Exact authored branch: absent PartialRect copies loaded texture dimensions
        // into source stack defaults; absent MapRect then reads actual X/YLocation.
        const char* partial=slot?"DeselectedPartialRect":"SelectedPartialRect";
        const char* map=slot?"DeselectedMapRect":"SelectedMapRect";
        if(section.find(partial))return fail(slot?0x2AD770:0x2AD118);
        if(section.find(map))return fail(slot?0x2AD810:0x2AD1B8);
        const float width=q.uv30[1],height=q.uv30[2];
        if(!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0||
           width>0x1000000||height>0x1000000||std::trunc(width)!=width||std::trunc(height)!=height)
            return fail(slot?0x2AD748:0x2AD0F0);
        std::int32_t x=0,y=0;
        if(!assets::originalUiInteger50KG(section,slot?"DeselectedXLocation":"SelectedXLocation",0,x,d.config,
                slot?0x2AD830:0x2AD1D8))return fail(slot?0x2AD830:0x2AD1D8);
        if(!assets::originalUiInteger50KG(section,slot?"DeselectedYLocation":"SelectedYLocation",0,y,d.config,
                slot?0x2AD848:0x2AD1F0))return fail(slot?0x2AD848:0x2AD1F0);
        // These retained authored coordinates are exactly representable; avoid
        // broadening the source arithmetic envelope to unchecked signed overflow.
        const bool right=i==7||i==53||i==55;
        const std::int32_t expectedX=right?(slot?557:552):60;
        const std::int32_t expectedY=i<8?46:(i<54?100:146);
        if(x!=expectedX||y!=expectedY)return fail(slot?0x2AD850:0x2AD1F8);
        const float xf=float(x),yf=float(y);
        if(q.position10[0]!=xf||q.position10[1]!=yf){
            q.position10[0]=xf;q.position10[1]=yf;q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
        }
        // Map width/height are exactly the loaded dimensions, so2AD2D8/350
        // (and deselected equivalents) compare equal and perform no size stores.
        const char* angle=slot?"DeselectedAngle":"SelectedAngle";
        float angleValue=0;
        if(!assets::originalUiNumber50KG(section,angle,0,angleValue,d.config,slot?0x2ADA04:0x2AD3AC))
            return fail(slot?0x2ADA04:0x2AD3AC);
        if(section.find(angle))return fail(slot?0x2ADA0C:0x2AD3B4); // Present-angle/pivot branch not this authored occurrence.
        std::int32_t alpha=255;
        if(!assets::originalUiInteger50KG(section,slot?"DeselectedAlpha":"SelectedAlpha",255,alpha,d.config,
                slot?0x2ADB1C:0x2AD4C4))return fail(slot?0x2ADB1C:0x2AD4C4);
        const auto alpha16=static_cast<std::uint16_t>(alpha);
        if(q.alpha48!=alpha16){
            const auto value=std::uint32_t(std::int16_t(alpha16)>>2);
            for(std::size_t n:{std::size_t(13),std::size_t(10),std::size_t(7),std::size_t(4)})q.resource58[n].words[3]=value;
            q.alpha48=alpha16;
        }
        float depth=1;
        if(!assets::originalUiNumber50KG(section,slot?"DeselectedDepth":"SelectedDepth",1,depth,d.config,
                slot?0x2ADB78:0x2AD520))return fail(slot?0x2ADB78:0x2AD520);
        if(depth!=1)return fail(slot?0x2ADB80:0x2AD528); // Exact authored default; no invented nonpositive branch.
        q.field40=depth;
        volatile float z=float(video_->packetZMaximum)-depth;
        for(std::size_t n:{std::size_t(14),std::size_t(11),std::size_t(8),std::size_t(5)})
            q.resource58[n].words[2]=std::bit_cast<std::uint32_t>(float(z));
        std::array<float,3> rgb{};
        // Original selected branch reads "Color", NOT authored "SelectedColor".
        if(!assets::originalUiTriple50KG(section,slot?"DeselectedColor":"Color",{{200,200,200}},rgb,d.config,
                slot?0x2ADC44:0x2AD5EC))return fail(slot?0x2ADC44:0x2AD5EC);
        for(float c:rgb)if(!std::isfinite(c)||c<0||c>255||std::trunc(c)!=c)return fail(slot?0x2ADC4C:0x2AD5F4);
        const auto packed=0xff000000u|(std::uint32_t(rgb[0])<<16)|(std::uint32_t(rgb[1])<<8)|std::uint32_t(rgb[2]);
        if(q.color44!=packed){
            for(std::size_t channel=0;channel<3;++channel)
                for(std::size_t n:{std::size_t(13),std::size_t(10),std::size_t(7),std::size_t(4)})
                    q.resource58[n].words[channel]=std::uint32_t(rgb[channel])>>1;
            q.color44=packed;
        }
        // SelectedAlphaEnabled and DeselectedAlphaEnabled are NOT read by2ACFE0.
        return true;
    }
    bool loadCourseSelectable1CD160(std::size_t i,const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD498;
        if(thread_!=std::this_thread::get_id()||terminal_||!courseSelectableOrdinalAdmitted(i)||courseSelectableAttempted_[i]||
           !issuer_||!scope_||!construction_||!modeEffect_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||
           courseWidgetOrdinal_!=i||loaderStopPC_!=0x1CD8A0||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_)!=&section||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        const auto* type=section.find("Type");if(!type||type->text!="SelectableIcon")return false;
        courseSelectableAttempted_[i]=true;
        auto& row=courseSelectableRows_[i];row.reset(new(std::nothrow) CourseSelectable); // Source malloc1CD4C0.
        if(!row)return fail(0x2AD010); // NULL ctor skipped; first Config getter gets derived output3C.
        // Native wrappers admitted before actual counted constructor or embedded ctor writes.
        for(auto& q:row->quads)if(!admitCourseWidgetQuadNative(q))return fail(0x2ACF00);
        d.entity=construction_->constructCourseSelectable2ACF00(i,row->entity);
        if(!d.entity.okay())return fail(d.entity.sourcePC);
        row->constructed=true;row->manager30=&scope_->manager_;
        row->field38=0;row->type34=0;row->field3C=0;row->field40=0;row->field44=0;row->field48=0;
        row->field4C=0;row->field50=0;row->field54=0;row->field58=0;
        if(!constructCourseWidgetQuad2DEF40(row->quads[0],i,0,d)||
           !constructCourseWidgetQuad2DEF40(row->quads[1],i,1,d))return false;
        const char* keys[]{"EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect"};
        std::uint32_t* fields[]{&row->field3C,&row->field40,&row->field44,&row->field48};
        for(std::size_t n=0;n<4;++n){std::int32_t value=0;const auto pc=std::uint32_t(0x2AD010+24*n);
            if(!assets::originalUiInteger50KG(section,keys[n],0,value,d.config,pc))return fail(pc);*fields[n]=std::uint32_t(value);}
        row->type34=6; //2AD060 after all four Config getters.
        for(std::size_t slot=0;slot<2;++slot){
            const auto* texture=section.find(slot?"DeselectedTexture":"SelectedTexture");
            const char* expected=courseQuadTexture(i,slot);
            if(!texture||texture->text!=expected)return fail(slot?0x2AD6E8:0x2AD090);
            if(!loadCourseWidgetQuad2DF0B0(row->quads[slot],i,slot,texture->text,d)||
               !courseSelectableQuadSuffix(i,slot,section,d))return false;
        }
        row->loaded=true;d.sourcePC=0x1CD8A8;return true;
    }
    bool preflightNativeCourseSelectableIcons()const noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        // Validate every acquired early/late owner and both payloads BEFORE
        // another widget family can free anything. Tail gates belong later.
        for(std::size_t i=0;i<courseSelectableRows_.size();++i){
            const auto& row=courseSelectableRows_[i];
            if(!courseSelectableOrdinalAdmitted(i)){
                if(row||courseSelectableAttempted_[i])return false;continue;
            }
            if(!row)continue;
            if(row->constructed&&(!scope_||!construction_||construction_->pageAcquired_||
               !construction_->registrationCurrent()||construction_->courseSelectableIcons_[i]!=&row->entity||
               row->manager30!=&scope_->manager_))return false;
            for(const auto& q:row->quads)if(!preflightCourseWidgetQuadNative(q))return false;
        }
        return true;
    }
    bool abortNativeCourseSelectableRange(std::size_t first,std::size_t end)noexcept {
        if(!preflightNativeCourseSelectableIcons()||first>end||end>courseSelectableRows_.size())return false;
        for(std::size_t later=end;later<courseSelectableRows_.size();++later)
            if(courseSelectableRows_[later])return false;
        for(std::size_t reverse=end;reverse>first;--reverse){const auto i=reverse-1;
            auto& row=courseSelectableRows_[i];if(!row)continue;
            if(!courseSelectableOrdinalAdmitted(i))return false;
            if(row->constructed&&(!construction_||!construction_->preflightAbortNativeCourseSelectable(i,row->entity)||
               construction_->courseSelectableIcons_[i]!=&row->entity||row->manager30!=&scope_->manager_))return false;
            // BOTH renderer/material gates before freeing either payload or the counted outer.
            for(auto& q:row->quads)if(!preflightCourseWidgetQuadNative(q))return false;
            if(!abortCourseWidgetQuadNative(row->quads[1])||!abortCourseWidgetQuadNative(row->quads[0]))return false;
            if(row->constructed){const auto r=construction_->abortNativeCourseSelectable(i,row->entity);if(!r.okay())return false;row->constructed=false;}
            row.reset();
        }
        return true; // Native reverse unwind, never a source2ACF50 completion receipt.
    }
    bool abortNativeCourseLateSelectableIcons()noexcept{return abortNativeCourseSelectableRange(52,56);}
    bool abortNativeCourseSelectableIcons()noexcept{return abortNativeCourseSelectableRange(6,8);}
