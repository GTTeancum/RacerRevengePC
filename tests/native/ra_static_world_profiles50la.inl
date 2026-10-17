// Included inside Owner after the authenticated image publication definitions.
// CPU semantic observation only. No raster lease, sampling or fog policy.
public:
    enum class StaticMaterialProfile : std::uint8_t {
        indexedPrelit, indexedDiffuseBlack, tga1555Prelit, indexedAlphaTestOff,
        indexedUnfoggedBlend, tga8888ClampedBlend, currentCrowd1555Blend
    };
    struct StaticFragmentDemands {
        StaticMaterialProfile profile=StaticMaterialProfile::indexedPrelit;
        bool sourceFogByte=true;
        bool nativeCoverage=false;
        bool orderedCurrentDestination=false;
        // All profiles still require independent sample/interpolation and
        // projected-depth/common-target contracts. These are demands only.
        bool sampledInterpolatedOperands=true;
        bool sourceGequalDepthWithWrites=true;
    };
private:
    static bool sameProfileInteger(const a::OriginalStandardMaterialInteger50LA& x,
        const a::OriginalStandardMaterialInteger50LA& y)noexcept {
        return x.value==y.value&&x.origin==y.origin&&x.templateMember==y.templateMember&&
            x.byteOffset==y.byteOffset&&x.sourcePC==y.sourcePC;
    }
    static bool sameProfileCollector(const a::OriginalStandardMaterialCollectorEntryView50LA& x,
        const a::OriginalStandardMaterialCollectorEntryView50LA& y)noexcept {
        return x.materialRow==y.materialRow&&x.pass==y.pass&&x.registration==y.registration&&
            x.redirect18==y.redirect18&&x.limit1C==y.limit1C&&x.finalLeaf==y.finalLeaf&&x.material==y.material;
    }
    // Neither map nor caller-specified state is accepted by a public decoder.
    // Only Owner calls this immediately after its genuine final-map operation.
    bool decodeStaticMaterialProfile(std::size_t fi,
        const a::OriginalStandardMaterialMapBindingView50LA& map,
        StaticFragmentDemands& output,Diagnostic& d)const {
        using F=a::OriginalUiGsField50KG;
        using O=a::OriginalStandardMaterialValueOrigin50LA;
        using P=StaticMaterialProfile;
        if(!coldImagesCurrent()||!context_||!context_->live()||context_->runtime435670()!=0||
           fi>=imageFamilies_.size()||map.map!=0||
           map.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||!map.texture)
            return reject(d,"static profile requires actual current final map");
        const auto& family=imageFamilies_[fi];const auto& table=domain_->materials382B38();
        a::OriginalStandardMaterialCollectorEntryView50LA origin,representative;
        if(!table->coldCollectorPass(imageCollector_,family.materialRow,0,origin)||origin.finalLeaf<0||
           !sameProfileCollector(origin,family.collector)||!sameProfileCollector(origin,map.origin)||
           !table->coldCollectorEntry(imageCollector_,static_cast<std::size_t>(origin.finalLeaf),representative)||
           !sameProfileCollector(representative,map.representative)||representative.redirect18!=-1||
           representative.pass!=0||!origin.material||!representative.material)
            return reject(d,"static profile collector provenance changed");
        const ColdFamily* representativeFamily=nullptr;
        for(const auto& candidate:imageFamilies_)if(candidate.materialRow==representative.materialRow){
            if(representativeFamily)return reject(d,"ambiguous profile representative");
            representativeFamily=&candidate;
        }
        if(!representativeFamily)return reject(d,"profile representative outside retained RA families");
        a::OriginalStandardMaterialPassView50LA pass,representativePass;
        const auto validatePass=[&](const ColdFamily& sourceFamily,
            const a::OriginalStandardMaterialObject50LA* material,
            a::OriginalStandardMaterialPassView50LA& view){
            const auto& memberName=sourceFamily.canonicalMaterialKey;
            const auto retained=psms_.find(memberName);
            if(retained==psms_.end()||!retained->second||!retained->second->originalBytes()||
               !material->source()||!material->source()->member()||
               normalized(*material->source()->member())!=memberName||
               !material->source()->originalBytes()||
               *material->source()->originalBytes()!=*retained->second->originalBytes()||
               !material->passCount48()||*material->passCount48()!=1||!material->pass4C(0,view)||
               !view.flags21||*view.flags21!=2||!view.mapCount20||*view.mapCount20!=1||
               !view.fillRgba2C||!view.texture1A||(*view.texture1A)[0]!=sourceFamily.textureToken||
               !view.program18||!view.state30||view.state30->contextCount()!=1||view.animation10||
               view.constructionTemplate!=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::defaultMt))return false;
            const auto& state=*view.state30->context(0);const auto& retainedState=sourceFamily.constructionContext;
            if(state.runtime435670!=0||retainedState.runtime435670!=0||
               state.videoDepthBase2E0!=context_->depthBufferBase2E0()||
               state.videoDepthFormat5C!=static_cast<std::int32_t>(context_->depthBufferFormat5C())||
               state.videoDepthBase2E0!=retainedState.videoDepthBase2E0||
               state.videoDepthFormat5C!=retainedState.videoDepthFormat5C)return false;
            for(std::size_t i=0;i<state.fields.size();++i)
                if(!state.fields[i].sourcePC||state.fields[i].origin==O::runtime||
                   !sameProfileInteger(state.fields[i],retainedState.fields[i]))return false;
            for(std::size_t i=0;i<state.sampler.size();++i)
                if(!state.sampler[i].sourcePC||state.sampler[i].origin==O::runtime||
                   !sameProfileInteger(state.sampler[i],retainedState.sampler[i]))return false;
            return true;
        };
        if(!validatePass(family,origin.material,pass)||
           !validatePass(*representativeFamily,representative.material,representativePass))
            return reject(d,"static profile pass or retained provenance changed");
        StaticFragmentDemands next;const auto& name=family.canonicalMaterialKey;
        if(name=="textures/black.psm")next.profile=P::indexedDiffuseBlack;
        else if(name=="textures/metalgrate.psm"||name=="textures/twidesigns02.psm")next.profile=P::tga1555Prelit;
        else if(name=="textures/camdroid03.psm"||name=="textures/gatedoor01.psm"||
                name=="textures/gatepost01.psm"||name=="textures/tile003.psm")next.profile=P::indexedAlphaTestOff;
        else if(name=="textures/cityscape.psm"||name=="textures/cityscape02.psm")next.profile=P::indexedUnfoggedBlend;
        else if(name=="textures/largeflag.psm")next.profile=P::tga8888ClampedBlend;
        else if(name=="textures/crowd.psm")next.profile=P::currentCrowd1555Blend;
        else {
            constexpr const char* ordinary[]={"arch01","arena01","arenahead01","arenatile","bridgeground01",
                "building01","building02","gargoylebody","gargoyleface","gatebottom01","grayrough01",
                "lightcoloredtile","newrock","rockcavewall","rockcavewall02","rockfloor","rockfloorfade",
                "rockwall","sand10","sand11","sand8","spear","spearcabledark","spearholder","stairs01",
                "theskull01","twidesigns","twilekflag01","wall001","wall002","wall_design01","wallside01","wallside01_blend"};
            bool found=false;for(const auto* stem:ordinary)if(name==std::string("textures/")+stem+".psm")found=true;
            if(!found)return reject(d,"unobserved RA profile family");
        }
        const bool crowd=next.profile==P::currentCrowd1555Blend;
        const bool tga16=next.profile==P::tga1555Prelit,tga32=next.profile==P::tga8888ClampedBlend;
        const bool blend=crowd||tga32||next.profile==P::indexedUnfoggedBlend;
        const bool fog=next.profile!=P::indexedUnfoggedBlend;
        const char* program=next.profile==P::indexedDiffuseBlack?"diffusecompressed":"prelitcompressed";
        std::int16_t programToken=-1;
        if(family.authored.passes.size()!=1||normalized(family.authored.passes[0].microprogram)!=program||
           !domain_->programs382B70()->find298C80(family.authored.passes[0].microprogram,programToken,d.material.program)||
           programToken!=*pass.program18||programToken!=*representativePass.program18||
           family.animatedCurrentMapPending!=crowd||family.contextAfter249610Available==crowd||
           bool(pass.textureAnimation10)!=crowd||
           bool(representativePass.textureAnimation10)!=crowd||map.textureToken!=family.textureToken||
           map.textureToken!=(*representativePass.texture1A)[0])
            return reject(d,"static profile actual program/map differs");
        if(family.firstImage>coldImages_.size()||family.imageCount>coldImages_.size()-family.firstImage||
           family.imageCount!=(crowd?6u:1u))return reject(d,"static profile image extent");
        const ColdImage* image=nullptr;
        for(std::size_t i=0;i<family.imageCount;++i)if(coldImages_[family.firstImage+i].canonical==map.texture){
            if(image)return reject(d,"static profile image alias");image=&coldImages_[family.firstImage+i];
        }
        const auto& texture=*map.texture;
        const auto imageName=normalized(family.sourceMember);
        const auto expectedName=name.substr(0,name.size()-4)+(crowd?".atm":(tga16||tga32?".tga":".pst"));
        if(!image||image->family!=fi||imageName!=expectedName||
           normalized(image->sourceMember)!=imageName||!texture.sourceMember()||
           normalized(*texture.sourceMember())!=imageName||!texture.originalBytes()||
           *texture.originalBytes()!=family.sourceBytes||!texture.constructed24DA30()||texture.animated102070()||
           !texture.references00()||*texture.references00()<=0||!texture.bitsPerPixelE8()||
           *texture.bitsPerPixelE8()!=(crowd||tga16?16:(tga32?32:8))||
           !texture.flagsF8()||!texture.flagsF8KnownMask()||!(*texture.flagsF8KnownMask()&1u)||
           ((*texture.flagsF8()&1u)!=0)!=crowd||!texture.formatAC()||
           *texture.formatAC()!=(crowd||tga16?1555:(tga32?8888:8)))
            return reject(d,"static profile retained image/TEXA kind differs");
        if(crowd){
            if(!texture.sourceFormat()||*texture.sourceFormat()!=a::OriginalStandardTextureSourceFormat50LA::pstIpum||
               !crowdImages_||image->frame>=crowdImages_->count())return reject(d,"static profile crowd provenance missing");
            a::OriginalStandardTextureLevelView50LA level;
            if(!texture.level100(0,level)||level.hostImage!=crowdImages_->frame(image->frame).get())
                return reject(d,"static profile current crowd lease differs");
        }else if(tga16||tga32){
            if(!texture.sourceFormat()||*texture.sourceFormat()!=(tga32?a::OriginalStandardTextureSourceFormat50LA::tgaBgra32:a::OriginalStandardTextureSourceFormat50LA::tgaBgr24)||
               (*texture.flagsF8KnownMask()&5u)!=5u||(*texture.flagsF8()&5u)!=4u||
               family.sourceBytes.size()<18||family.sourceBytes[16]!=(tga32?32:24))
                return reject(d,"static profile original TGA depth differs");
        }else if(!texture.sourceFormat()||*texture.sourceFormat()!=a::OriginalStandardTextureSourceFormat50LA::pstIndexed||
                  !texture.sourceKind()||*texture.sourceKind()!=1)
            return reject(d,"static profile indexed source kind differs");
        // Complete common-state vector in OriginalUiGsField50KG order. Refuse
        // enum growth until every additional field receives source review.
        constexpr std::int32_t common[]={1,0,255,255,0,0,1,0,1,128,0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,1,6,0,0,0,1,2,0};
        static_assert(sizeof(common)/sizeof(common[0])==a::originalUiGsFieldCount50KG,"review new GS fields");
        std::array<std::int32_t,a::originalUiGsFieldCount50KG> expected{};
        for(std::size_t i=0;i<expected.size();++i)expected[i]=common[i];
        expected[static_cast<std::size_t>(F::methodOfExpandingTextureAlpha)]=crowd?1:0;
        expected[static_cast<std::size_t>(F::wrapModeVertical)]=tga32?1:0;
        expected[static_cast<std::size_t>(F::foggingOnOff)]=fog?1:0;
        expected[static_cast<std::size_t>(F::alphaBlendingOnOff)]=blend?1:0;
        expected[static_cast<std::size_t>(F::onePassAntiAliasingOnOff)]=crowd?1:0;
        expected[static_cast<std::size_t>(F::alphaTestOnOff)]=next.profile==P::indexedAlphaTestOff?0:1;
        expected[static_cast<std::size_t>(F::alphaReferenceValue)]=crowd?120:0;
        const auto& final=map.contextAfter249610;const auto& source=*representativePass.state30->context(0);
        if(final.runtime435670!=0||final.videoDepthBase2E0!=source.videoDepthBase2E0||
           final.videoDepthFormat5C!=source.videoDepthFormat5C)return reject(d,"static profile final video provenance differs");
        for(std::size_t i=0;i<expected.size();++i){
            auto provenance=source.fields[i];
            const bool texa=i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)||
                i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis1)||
                i==static_cast<std::size_t>(F::methodOfExpandingTextureAlpha);
            if(crowd&&texa)provenance={expected[i],O::runtime,nullptr,0,0x24983Cu};
            if(final.fields[i].value!=expected[i]||!sameProfileInteger(final.fields[i],provenance)||
               (!texa&&pass.state30->fields[i].value!=expected[i]))
                return reject(d,"unsupported static GS combination or field provenance");
        }
        constexpr std::int32_t sampler[]={0,0,1,5,0,-75};
        for(std::size_t i=0;i<6;++i)if(final.sampler[i].value!=sampler[i]||
            pass.state30->sampler[i].value!=sampler[i]||!sameProfileInteger(final.sampler[i],source.sampler[i]))
            return reject(d,"unsupported static sampler or provenance");
        next.sourceFogByte=fog;next.nativeCoverage=crowd;next.orderedCurrentDestination=blend;
        output=next;return true;
    }
