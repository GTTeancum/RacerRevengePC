// Actual Course plain Icon cohort5/64/65/66/81/82/83. Source60 publishes only after2DF0B0 returns.
    struct CourseIconFields {
        C entity;const C* manager30=nullptr;
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0;
        std::uint32_t field4C=0,field50=0,field54=0,field58=0;
        std::optional<std::uint32_t> field5C,field64; // Not written by2A7760/2AC4B0.
        CourseQuad* source60=nullptr;
    };
    struct CourseIconRow {
        std::unique_ptr<CourseIconFields> owner;CourseWidgetQuad quad;
        bool attempted=false,constructed=false,loaded=false;
        std::string textureStack50;
    };
    std::array<CourseIconRow,84> courseIconRows_;
    static bool courseIconOrdinalAdmitted(std::size_t i)noexcept{return i==5||i==64||i==65||i==66||i==81||i==82||i==83;}
    C* courseIconEntity(std::size_t i)noexcept {
        return courseIconOrdinalAdmitted(i)&&courseIconRows_[i].constructed&&courseIconRows_[i].owner?&courseIconRows_[i].owner->entity:nullptr;
    }
    static const char* courseIconMapRect(std::size_t i)noexcept {
        switch(i){case 5:return "76,564,48,64";case 64:return "0,640,154,186";
            case 65:return "0,640,186,351";case 66:return "0,640,351,383";
            case 81:return "0,640,67,99";case 82:return "0,640,99,137";
            case 83:return "0,640,137,169";default:return nullptr;}
    }
    static const char* courseIconPartialRect(std::size_t i)noexcept {
        switch(i){case 64:case 81:return "0,32,0,32";case 65:case 82:return "0,32,18,32";
            case 66:case 83:return "0,32,32,0";default:return nullptr;}
    }
    bool loadCourseIcon1CD160(std::size_t i,const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD414;
        if(thread_!=std::this_thread::get_id()||terminal_||!courseIconOrdinalAdmitted(i)||courseIconRows_[i].attempted||
           !issuer_||!construction_||!modeEffect_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||
           courseWidgetOrdinal_!=i||loaderStopPC_!=0x1CD8A0||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_)!=&section||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto fail=[&](std::uint32_t pc)noexcept {d.sourcePC=pc;return false;};
        const auto* type=section.find("Type");if(!type||type->text!="Icon")return fail(0x1CD414);
        auto& row=courseIconRows_[i];
        // Native occurrence-token admission before actual malloc68/count ctor.
        if(!admitCourseWidgetQuadNative(row.quad,false))return false;
        row.attempted=true;
        row.owner.reset(new(std::nothrow) CourseIconFields); // Actual1CD468.
        if(!row.owner)return fail(0x2AC5C8); // Loader first getter would targetNULL+3C.
        auto& w=*row.owner;d.entity=construction_->constructCourseIcon2AC4B0(i,w.entity);
        if(!d.entity.okay())return fail(d.entity.sourcePC);
        row.constructed=true;w.manager30=&scope_->manager_;
        // Source base34..58 zeros and source60NULL are represented above;
        // source5C/64 stay unknown rather than silently native-zero knowledge.
        const char* keys[]{"EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect"};
        std::uint32_t* fields[]{&w.field3C,&w.field40,&w.field44,&w.field48};
        for(std::size_t n=0;n<4;++n){std::int32_t value=0;
            const auto pc=std::uint32_t(0x2AC5C8+24*n);
            if(!assets::originalUiInteger50KG(section,keys[n],0,value,d.config,pc))return fail(pc);
            *fields[n]=static_cast<std::uint32_t>(value);
        }
        w.type34=5;w.manager30=&scope_->manager_; //2AC618/61C.
        const auto* texture=section.find("Texture");
        if(!texture||texture->text!=(i==5?"nameblur":"overlay1"))return fail(0x2AC650);
        try {row.textureStack50=texture->text;}
        catch(...){return fail(0x2AC650);}
        // Authenticated name has no '.'; source1169D0 takes no suffix erase.
        if(!constructCourseWidgetQuad2DEF40(row.quad,i,0,d))return false;
        if(!loadCourseWidgetQuad2DF0B0(row.quad,i,0,row.textureStack50,d))return false;
        w.source60=row.quad.backing.get(); // Actual2AC6B8, AFTER loader return.
        auto& q=*w.source60;
        // Actual stack default rectangles read the genuinely loaded dimensions.
        if(!std::isfinite(q.uv30[1])||!std::isfinite(q.uv30[2])||q.uv30[1]<0||q.uv30[2]<0||
           q.uv30[1]>16777216||q.uv30[2]>16777216)return fail(0x2AC6DC);
        std::array<std::int32_t,4> defaults{0,0,std::int32_t(q.uv30[1]),std::int32_t(q.uv30[2])};
        float angle=0;
        if(!assets::originalUiNumber50KG(section,"Angle",0,angle,d.config,0x2AC700)||section.find("Angle"))return fail(0x2AC700);
        // MissingAngle skips Pivot getters. Source158E60 parses left,right,top,bottom
        // into stack left,top,right,bottom; all admitted authored integers are bounded.
        const auto rectangle=[&](const auto* value,const char* expected,
            std::array<std::int32_t,4>& output,std::uint32_t pc)noexcept {
            if(!value||!expected||value->text!=expected){d.sourcePC=pc;return false;}
            std::array<std::int32_t,4> tokens{};std::size_t at=0;
            for(std::size_t n=0;n<4;++n){while(at<value->text.size()&&value->text[at]!=','){
                const auto c=value->text[at++];if(c<'0'||c>'9'){d.sourcePC=pc;return false;}
                tokens[n]=tokens[n]*10+std::int32_t(c-'0');
            }++at;}
            output={tokens[0],tokens[2],tokens[1],tokens[3]};return true;
        };
        std::array<std::int32_t,4> partial=defaults,map=defaults;
        const auto* part=section.find("PartialRect");const bool hasPartial=part!=nullptr;
        if(i==5){if(part)return fail(0x2AC750);}
        else if(!rectangle(part,courseIconPartialRect(i),partial,0x2AC750))return false;
        if(!rectangle(section.find("MapRect"),courseIconMapRect(i),map,0x2AC774))return false;
        std::int32_t alpha=255;float depth=1;std::array<float,3> rgb{};
        if(!assets::originalUiInteger50KG(section,"Alpha",255,alpha,d.config,0x2AC7F4))return fail(0x2AC7F4);
        if(!assets::originalUiNumber50KG(section,"Depth",1,depth,d.config,0x2AC810))return fail(0x2AC810);
        if(!assets::originalUiTriple50KG(section,"Color",{200,200,200},rgb,d.config,0x2AC840))return fail(0x2AC840);
        if(alpha!=255||depth!=1||rgb!=(i==5?std::array<float,3>{76,115,153}:std::array<float,3>{242,133,27}))return fail(0x2AC848);
        // Actual position/size stores mark dirty low bits; no draw/regeneration.
        const auto dirty=[&]()noexcept {q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}};
        if(q.position10[0]!=float(map[0])||q.position10[1]!=float(map[1])){
            q.position10[0]=float(map[0]);q.position10[1]=float(map[1]);dirty();}
        const float width=float(map[2]-map[0]),height=float(map[3]-map[1]);
        if(q.uv30[1]!=width){q.uv30[1]=width;dirty();}
        if(q.uv30[2]!=height){q.uv30[2]=height;dirty();}
        // Actualalpha is unchanged255, so no packet alpha writes occur.
        q.field40=depth;volatile float z=float(video_->packetZMaximum)-depth;
        const auto zb=std::bit_cast<std::uint32_t>(float(z));
        for(std::size_t n=4;n--;)q.resource58[5+3*n].words[2]=zb;
        const std::uint32_t color=0xff000000u|(std::uint32_t(rgb[0])<<16)|(std::uint32_t(rgb[1])<<8)|std::uint32_t(rgb[2]);
        if(q.color44!=color){
            for(std::size_t channel=0;channel<3;++channel)
                for(std::size_t n=4;n--;)q.resource58[4+3*n].words[channel]=(color>>(16-8*channel)&255u)>>1;
            q.color44=color;
        }
        // Angle=0 skips2ACBA4..CC70. PartialRect writes actual UV packet fields,
        // not draw regeneration. Reversed bottom V (32→0) is authored and valid.
        if(hasPartial){
            const auto left=std::bit_cast<std::uint32_t>(mul(float(partial[0]),q.scale4C));
            q.resource58[12].words[0]=left;q.resource58[6].words[0]=left; //C0 then60.
            const auto right=std::bit_cast<std::uint32_t>(mul(float(partial[2]),q.scale4C));
            q.resource58[15].words[0]=right;q.resource58[9].words[0]=right; //F0 then90.
            const auto top=std::bit_cast<std::uint32_t>(mul(float(partial[1]),q.scale50));
            q.resource58[9].words[1]=top;q.resource58[6].words[1]=top; //94 then64.
            const auto bottom=std::bit_cast<std::uint32_t>(mul(float(partial[3]),q.scale50));
            q.resource58[15].words[1]=bottom;q.resource58[12].words[1]=bottom; //F4 thenC4.
        }
        row.loaded=true;d.sourcePC=0x1CD8A8;return true;
    }
    bool preflightNativeCourseIcons()const noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        for(std::size_t i=0;i<courseIconRows_.size();++i){const auto& row=courseIconRows_[i];
            if(!courseIconOrdinalAdmitted(i)){
                if(row.owner||row.attempted||row.quad.backing||row.quad.occurrence)return false;continue;
            }
            if(row.constructed&&(!row.owner||!scope_||!construction_||construction_->pageAcquired_||
               !construction_->registrationCurrent()||construction_->courseIcons_[i]!=&row.owner->entity||
               row.owner->manager30!=&scope_->manager_))return false;
            if(!preflightCourseWidgetQuadNative(row.quad))return false;
        }
        return true;
    }
    bool abortNativeCourseIconRange(std::size_t first,std::size_t end)noexcept {
        if(!preflightNativeCourseIcons()||first>end||end>courseIconRows_.size())return false;
        for(std::size_t later=end;later<courseIconRows_.size();++later){const auto& row=courseIconRows_[later];
            if(row.owner||row.quad.backing||row.quad.occurrence)return false;}
        for(std::size_t reverse=end;reverse>first;--reverse){const auto i=reverse-1;auto& row=courseIconRows_[i];
            if(row.constructed&&(!construction_||!row.owner||!construction_->preflightAbortNativeCourseIcon(i,row.owner->entity)))return false;
            // Per-row tail admission precedes packet/material disposal, including retry.
            if(!preflightCourseWidgetQuadNative(row.quad)||!abortCourseWidgetQuadNative(row.quad))return false;
            if(row.constructed){if(!construction_->abortNativeCourseIcon(i,row.owner->entity).okay())return false;row.constructed=false;}
            row.owner.reset();
        }
        return true; // Native unwind only, never an original destructor completion receipt.
    }
    bool abortNativeCourseFinalIcons()noexcept{return abortNativeCourseIconRange(81,84);}
    bool abortNativeCourseLateIcons()noexcept{return abortNativeCourseIconRange(64,67);}
    bool abortNativeCourseIcon()noexcept{return abortNativeCourseIconRange(5,6);}
