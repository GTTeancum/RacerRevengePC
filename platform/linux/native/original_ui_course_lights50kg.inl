// Private fragment of OriginalUiCourseOwnedLifecycle50KG. Not a standalone header.
    using C=game::OriginalWreckComponent50LA;
    using Vec=std::array<float,4>;
    struct CourseLight {
        C entity;std::uint16_t query30=0;std::uint8_t query32=0xff,flags33=0,knownFlags33=0;
        std::uint32_t type34=0,mode38=0;Vec diffuse3C{},specular4C{},ambient5C{};
        std::array<float,3> position6C{},look78{};float range84=0,field88=0;
        std::array<float,3> attenuation8C{};float cone98=0,cone9C=0;Vec positionA0{},lookB0{};
        const void* fieldC0=nullptr;const void* fieldC4=nullptr;std::int32_t ordinalC8=-1;std::uint32_t luminanceCC=0;
        const void* observerD0=nullptr;const void* fieldD4=nullptr;const void* fieldD8=nullptr;
        bool constructed=false,typed=false,configured=false,registered=false,attached=false,retiring=false,retired=false;
        bool diffuseAlphaKnown=false,specularAlphaKnown=false,ambientAlphaKnown=false;
    };
    struct CourseLightDescriptor {
        Vec position00{},infinity10{},look20{};const Vec* position30=nullptr;const Vec* look34=nullptr;
        CourseLight* light38=nullptr;Vec color3C{};std::uint8_t knownColor=0;float range4C=0;
        std::array<std::uint8_t,5> layers50{};std::uint32_t flareColor58=0;float brightness5C=0;
        std::array<char,64> name60{};std::uint32_t typeA0=0;std::uint8_t flagsA4=0,knownFlagsA4=0;
        bool light38Known=false,parsed=false,flareKnown=false;
    };

    std::unique_ptr<CourseLightDescriptor[]> courseDescriptors29C_;
    CourseLightDescriptor* sourceDescriptors29C_=nullptr;
    bool sourceDescriptors29CKnown_=false,courseUiLightAttempted_=false;
    std::array<std::unique_ptr<CourseLight>,2> courseUiLights_{};
    static bool courseLightFail(Diagnostic& d,std::uint32_t pc)noexcept {d.sourcePC=pc;return false;}
    const CourseLight* courseLightRow(const C& entity)const noexcept {
        for(const auto& row:courseUiLights_)if(row && &row->entity==&entity)return row.get();
        return nullptr;
    }
    CourseLight* courseLightRow(const C& entity)noexcept {
        for(auto& row:courseUiLights_)if(row && &row->entity==&entity)return row.get();
        return nullptr;
    }
    bool courseLightEntityCurrent(const C& entity,std::size_t ordinal)const noexcept {
        return ordinal<2&&construction_&&scope_&&courseUiLights_[ordinal]&&
            &courseUiLights_[ordinal]->entity==&entity&&courseUiLights_[ordinal]->constructed&&
            !courseUiLights_[ordinal]->retired&&construction_->courseUiLights_[ordinal]==&entity&&
            construction_->registrationCurrent()&&entity.reference00==1&&
            entity.kind==game::OriginalWreckComponentKind50LA::light146720&&
            entity.sessionGeneration==construction_->generation_&&entity.objectKey==Scope::key(&entity);
    }
static float fp(std::uint32_t u)noexcept{float f;std::memcpy(&f,&u,4);return f;}
static std::uint32_t word(float f)noexcept{std::uint32_t u;std::memcpy(&u,&f,4);return u;}
static float add(float a,float b)noexcept{volatile float r=a+b;return r;}
static float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
static float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
static float div(float a,float b)noexcept{volatile float r=a/b;return r;}
static bool finite(const Vec& v)noexcept{for(float f:v)if(!std::isfinite(f))return false;return true;}
static bool vuNormalize(Vec& v)noexcept {
    // Same bounded finite VU convention as ChariotRoot49U: rounded binary32
    // sqrt followed by binary32 division, not scalar EE RSQRT's single round.
    if(!finite(v))return false;
    const float x=mul(v[0],v[0]),y=mul(v[1],v[1]),z=mul(v[2],v[2]);
    const float norm=add(add(x,y),mul(1.f,z));
    if(!std::isfinite(norm)||(norm!=0&&(word(norm)&0x7f800000u)==0))return false;
    float q=fp(0x7f7fffffu);
    if(norm!=0){volatile float root=std::sqrt(norm);q=div(1.f,root);}
    for(std::size_t i=0;i<3;++i)v[i]=mul(v[i],q);
    return finite(v);
}
static bool infinityPosition(const Vec& in,Vec& out)noexcept {
    const float x=mul(in[0],in[0]),y=mul(in[1],in[1]),z=mul(in[2],in[2]);
    const float norm=add(add(x,y),z);
    if(!std::isfinite(norm)||norm<=0||(word(norm)&0x7f800000u)==0)return false;
    //107110 scalar RSQRT.S, then1CE200 reciprocal, then1CE214 division.
    volatile float reciprocalRoot=static_cast<float>(1.0/std::sqrt(static_cast<double>(norm)));
    const float length=div(1.f,reciprocalRoot),factor=div(80000.f,length);
    out={{mul(in[0],factor),mul(in[1],factor),mul(in[2],factor),1.f}};return finite(out);
}
bool parseDescriptor1CDCF0(std::size_t ordinal,Diagnostic& d)noexcept {
    auto& row=courseDescriptors29C_[ordinal];
    try {
        const auto* section=loaderConfig_->config_->records_.section(ordinal?"Light2":"Light1");
        if(!section)return courseLightFail(d,0x1CDCF0); // only authenticated authored branches
        const auto* type=section->find("Type");
        if(!type||type->text!=(ordinal?"Directional":"Ambient"))return courseLightFail(d,0x1CE014);
        row.typeA0=ordinal?2u:4u;
        std::array<float,3> rgb{};
        if(!section->find("ColorRGB")||!assets::originalUiTriple50KG(*section,"ColorRGB",{},rgb,d.config,0x1CE094))
            return courseLightFail(d,0x1CE094);
        for(std::size_t i=0;i<3;++i){if(!std::isfinite(rgb[i])||rgb[i]<0||rgb[i]>255)return courseLightFail(d,0x1CEA20);row.color3C[i]=rgb[i];}
        row.knownColor=7; //1CEA20 NEVER writes authored alpha48.
        std::array<float,3> position{};
        if(!assets::originalUiTriple50KG(*section,"Position",{{-2000,4000,1252}},position,d.config,0x1CE0D4))return courseLightFail(d,0x1CE0D4);
        row.position00={{position[0],position[1],position[2],1}};row.position30=&row.position00;
        if(!section->find("Position")&&row.typeA0==4)row.position30=nullptr;
        const auto flag=[&](const char* key,bool fallback,unsigned bit,std::uint32_t pc){bool value=false;
            if(!assets::originalUiBoolean50KG(*section,key,fallback,value,d.config,pc))return false;
            const auto mask=std::uint8_t(1u<<bit);row.flagsA4=std::uint8_t((row.flagsA4&~mask)|(value?mask:0));row.knownFlagsA4|=mask;return true;};
        if(!flag("PlaceLightAtInfinity",false,1,0x1CE154))return courseLightFail(d,0x1CE154);
        if(row.flagsA4&2){if(!infinityPosition(row.position00,row.infinity10))return courseLightFail(d,0x1CE1EC);row.position30=&row.infinity10;}
        row.look34=&row.look20;
        if(section->find("LookVector"))return courseLightFail(d,0x1CE280); // not authored here
        if(section->find("TargetPosition")){
            std::array<float,3> target{};if(!assets::originalUiTriple50KG(*section,"TargetPosition",{},target,d.config,0x1CE2C0))return courseLightFail(d,0x1CE2C0);
            row.look20={{sub(target[0],row.position00[0]),sub(target[1],row.position00[1]),sub(target[2],row.position00[2]),1}};
            if(!vuNormalize(row.look20))return courseLightFail(d,0x1CE310);
        }else{row.look20={{0,0,0,1}};if(row.typeA0==4)row.look34=nullptr;else return courseLightFail(d,0x1CE368);}
        if(!assets::originalUiNumber50KG(*section,"Range",-1,row.range4C,d.config,0x1CE3A0))return courseLightFail(d,0x1CE3A0);
        if(row.range4C==-1)row.range4C=0;
        if(!std::isfinite(row.range4C)||row.range4C<0)return courseLightFail(d,0x147018);
        if(!flag("ShowDebugSphere",false,0,0x1CE424)||!flag("CastsShadows",false,3,0x1CE474)||
           !flag("EmitsLight",true,2,0x1CE4CC)||!flag("HasLensFlare",false,4,0x1CE524))return courseLightFail(d,d.config.sourcePC);
        if(!(row.flagsA4&4))return courseLightFail(d,0x1CDBD0);
        if(row.flagsA4&16){
            // Authenticated original integer tokens only. This bounded reader
            // does not widen native acceptance to arbitrary source atoi text.
            const auto* color=section->find("LensFlareColorRGB");
            if(!color||color->text!="255,255,240")return courseLightFail(d,0x1CE598);
            row.flareColor58=0x00fffff0u;
            if(!assets::originalUiNumber50KG(*section,"LensFlareBrightness",-1,row.brightness5C,d.config,0x1CE5B4))return courseLightFail(d,0x1CE5B4);
            if(row.brightness5C==-1)row.brightness5C=1;
            const auto* layers=section->find("LensFlareLayersVisible");
            if(!layers||layers->text!="T,T,T,T,T")return courseLightFail(d,0x1CE640);
            row.layers50={{1,1,1,1,1}};
            const auto* name=section->find("LensFlareTextureMapName");
            const std::string_view text=name?std::string_view(name->text):std::string_view("overlay\\lensflare.tga");
            if(text.size()>=63)return courseLightFail(d,0x1CE728);
            std::memcpy(row.name60.data(),text.data(),text.size());row.name60[text.size()]='\0';row.flareKnown=true;
        }
        row.parsed=true;return true;
    }catch(...){return courseLightFail(d,0x1CDCF0);}
}
bool constructLight146720(std::size_t ordinal,Diagnostic& d)noexcept {
    try{courseUiLights_[ordinal]=std::make_unique<CourseLight>();}
    catch(...){return courseLightFail(d,0x1CDC40);}
    auto& l=*courseUiLights_[ordinal];const auto step=construction_->constructCourseUiLight146720(ordinal,l.entity);
    if(!step.okay())return courseLightFail(d,step.sourcePC);
    l.constructed=true;l.query30=0;l.query32=0xff;l.flags33=0;l.knownFlags33=7;
    l.type34=0;l.mode38=3;l.diffuse3C=l.specular4C=l.ambient5C={{128,128,128,0}};
    l.diffuseAlphaKnown=l.specularAlphaKnown=l.ambientAlphaKnown=true;
    l.position6C={{0,0,0}};l.look78={{1,0,0}};
    l.range84=fp(game::SourceUiLightMath50KG::range146898());l.field88=1;l.attenuation8C={{1,0,0}};
    l.cone9C=fp(0x3fc90fdbu);l.cone98=div(l.cone9C,2.f);l.luminanceCC=0xbf800000u;l.ordinalC8=-1;
    l.positionA0={{0,0,0,1}};l.lookB0={{0,0,0,1}};
    // C0/C4/D0/D4/D8 are real constructor null writes, not skipped objects.
    const auto type=scope_->application_->types383408().lightEmitter146938();
    if(!type.okay())return courseLightFail(d,type.sourcePC);
    l.query32=type.type;l.typed=true;return true;
}
bool configureLight146F60(std::size_t ordinal,Diagnostic& d)noexcept {
    auto& l=*courseUiLights_[ordinal];const auto& row=courseDescriptors29C_[ordinal];OriginalUiLightManagerOwner50KG::Diagnostic md;
    if(!l.typed||!row.parsed||l.observerD0||l.fieldC0||l.fieldC4||l.fieldD8||!courseLightEntityCurrent(l.entity,ordinal)||std::fegetround()!=FE_TONEAREST)return courseLightFail(d,0x146F60);
    //141C20 reached first is jr ra/move v0,a0: no source state effect.
    l.type34=row.typeA0;l.ordinalC8=static_cast<std::int32_t>(ordinal);
    Vec half{};for(std::size_t i=0;i<3;++i)half[i]=div(row.color3C[i],2.f);
    if(row.typeA0==4){l.ambient5C=half;l.ambientAlphaKnown=false;l.diffuse3C=l.specular4C={{0,0,0,0}};
        l.diffuseAlphaKnown=l.specularAlphaKnown=true;
    }else{l.diffuse3C=l.specular4C=half;l.diffuseAlphaKnown=l.specularAlphaKnown=false;
        l.ambient5C={{0,0,0,0}};l.ambientAlphaKnown=true;}
    if(!courseLight_->incrementCourseRevision149300(*this,md))return courseLightFail(d,md.sourcePC);
    if(!game::SourceUiLightMath50KG::luminance146E60(word(l.diffuse3C[0]),word(l.diffuse3C[1]),word(l.diffuse3C[2]),l.luminanceCC))
        return courseLightFail(d,0x146E60);
    float range=row.range4C;const float maximum=fp(game::SourceUiLightMath50KG::range146898());
    if(range==0||range>maximum)range=maximum;
    if(range!=l.range84){l.range84=range;if(!courseLight_->incrementCourseRevision149300(*this,md))return courseLightFail(d,md.sourcePC);}
    if(row.position00[0]!=l.positionA0[0]||row.position00[1]!=l.positionA0[1]||row.position00[2]!=l.positionA0[2]){
        l.positionA0=row.position00; //1CDC24 passes BASE, NOT descriptor30.
        //147600 has no observer/flare work with actual D0/C0/C4 null.
        l.position6C={{l.positionA0[0],l.positionA0[1],l.positionA0[2]}};
        if(!courseLight_->incrementCourseRevision149300(*this,md))return courseLightFail(d,md.sourcePC);
    }
    if(row.look34&&(row.look20[0]!=l.lookB0[0]||row.look20[1]!=l.lookB0[1]||row.look20[2]!=l.lookB0[2])){
        l.lookB0=row.look20;if(!vuNormalize(l.lookB0))return courseLightFail(d,0x147200);
        l.look78={{l.lookB0[0],l.lookB0[1],l.lookB0[2]}};
        if(!courseLight_->incrementCourseRevision149300(*this,md))return courseLightFail(d,md.sourcePC);
    }
    l.attenuation8C={{1,0,0}};l.mode38=3;l.configured=true;return true;
}
    bool loadManagerUiLights1CDB10(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCD90;
        if(thread_!=std::this_thread::get_id()||terminal_||courseUiLightAttempted_||loaderStopPC_!=0x1CCD90||
           !issuer_||!construction_||!modeEffect_||!courseLight_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           courseLight_->courseZoneOwner_!=this||!courseLight_->zonesComplete_||
           std::fegetround()!=FE_TONEAREST)return false;
        // Native admission reserves only Entity registry capacity; it neither
        // constructs a Light nor preempts any real descriptor/Light malloc.
        d.entity=construction_->admitCourseUiLightCounts();if(!d.entity.okay())return false;
        courseUiLightAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept{terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        const auto* section=loaderConfig_->config_->records_.section("Lights");std::int32_t count=0;
        if(!section||!assets::originalUiInteger50KG(*section,"NumberOfLights",2,count,d.config,0x1CDB4C)||count!=2)
            return stop(0x1CDB4C); // Actual authenticated Course cohort only.
        try {courseDescriptors29C_.reset(new CourseLightDescriptor[2]);}catch(...){}
        if(courseDescriptors29C_)for(std::size_t i=0;i<2;++i){auto& row=courseDescriptors29C_[i];
            row.position00=row.infinity10=row.look20={{0,0,0,1}}; //1CDCD0 array constructor.
        }
        sourceDescriptors29C_=courseDescriptors29C_.get();sourceDescriptors29CKnown_=true; //1CDB90 including NULL.
        if(!courseDescriptors29C_)return stop(0x1CDBB4); // First per-row light38 store, before parsing.
        for(std::size_t i=0;i<2;++i){d.lightOrdinal=i;auto& row=courseDescriptors29C_[i];
            row.light38=nullptr;row.light38Known=true; //1CDBB4, not an array-constructor default.
            if(!parseDescriptor1CDCF0(i,d)||!constructLight146720(i,d)||!configureLight146F60(i,d))return stop(d.sourcePC);
            row.light38=courseUiLights_[i].get(); //1CDC50, only after actual configure returned.
            if(!courseLight_->appendCourseLight148F30(*this,courseUiLights_[i]->entity,d.light))return stop(d.light.sourcePC);
            d.entity=construction_->attachCourseUiLight1CDC8C(i,courseUiLights_[i]->entity);
            if(!d.entity.okay())return stop(d.entity.sourcePC);
            courseUiLights_[i]->attached=true;
        }
        loaderStopPC_=d.sourcePC=0x1CCDA0;return true; // BEFORE actual Fog1CCFA0, not full loader return.
    }
    bool abortNativeCourseUiLights()noexcept {
        for(std::size_t n=2;n>0;--n){const auto i=n-1;auto& row=courseUiLights_[i];
            if(!row)continue;
            if(row->constructed&&!row->retired){
                if(!courseLight_||!courseLightEntityCurrent(row->entity,i)||row->observerD0||row->fieldC0||row->fieldC4||row->fieldD8)return false;
                OriginalUiLightManagerOwner50KG::Diagnostic light;
                if(!courseLight_->removeCourseLight1491F0(*this,row->entity,light))return false;
                const auto retired=construction_->abortNativeCourseUiLight(i,row->entity);if(!retired.okay())return false;
                row->attached=false;row->retired=true;
            }
            row.reset();
        }
        // Native descriptor backing withdrawal after every actual borrowed
        // registry row and counted child is gone. Historical29C not zeroed.
        courseDescriptors29C_.reset();return true;
    }
