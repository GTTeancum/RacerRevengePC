// Included inside the private body of the one aggregate-owned world Owner.
// Explicit native cold construction policy, never source draw dispatch.
public:
    struct ColdImage {
        std::string sourceMember;
        std::size_t family=0,frame=0;
        const a::OriginalStandardTextureResource50LA* canonical=nullptr;
        std::vector<a::OriginalStandardTextureLevelView50LA> levels;
        // Ascending dimensions for indexed PST; canonical order for TGA.
        // Crowd contains its sole actual loaded source mip4.
        std::vector<a::TextureMip48E> mips;
        std::vector<std::uint8_t> baseRrtex;
    };
    struct ColdFamily {
        std::string materialMember,requestedTexture,sourceMember;
        std::vector<std::uint8_t> sourceBytes;
        a::Material48E authored;
        std::vector<a::detail::OriginalIpumFrameExtent50LA> compressedSourceFrames; // all30 extents
        std::int32_t materialRow=-1;
        std::int16_t textureToken=-1;
        a::OriginalStandardMaterialCollectorEntryView50LA collector;
        a::OriginalStandardMaterialStateContext50LA context;
        a::OriginalStandardMaterialStateContext50LA constructionContext; // immutable provenance snapshot
        std::size_t firstImage=0,imageCount=0;
        bool animatedCurrentMapPending=false;
        bool contextAfter249610Available=false; // false: construction context only
        // Owned authenticated lookup key; raw materialMember remains unchanged.
        std::string canonicalMaterialKey;
    };
private:
    std::vector<ColdFamily> imageFamilies_;
    std::vector<ColdImage> coldImages_;
    a::MaterialTextureAssets50EA pixelBank_;
    std::shared_ptr<const a::OriginalStandardIpumCompressedFrames50LA> crowdCompressed_;
    std::shared_ptr<const a::OriginalStandardIpumImageSet50LA> crowdImages_;
    a::OriginalStandardMaterialCollectorReceipt50LA imageCollector_;
    bool imagesAttempted_=false,imagesComplete_=false;

    bool selectImageSource(ColdFamily& family,Diagnostic& d) {
        const auto& provider=domain_->archiveProvider3E1A70();
        auto stem=family.requestedTexture;const auto dot=stem.find_last_of('.');
        if(dot!=std::string::npos)stem.erase(dot);
        for(const char* extension:{".pst",".ipu",".tga",".atm"}){
            a::OriginalStandardArchiveProvider50LA::MemberView selected;
            const auto requested=stem+extension;const auto result=provider->lookup132820(requested,selected);
            if(result==a::OriginalStandardArchiveProvider50LA::Lookup::absent)continue;
            if(result!=a::OriginalStandardArchiveProvider50LA::Lookup::present||!selected.member||
               std::strcmp(extension,".ipu")==0)return reject(d,"unresolved/unsupported original image lookup");
            const auto* row=sourceEntry(*selected.member);
            if(!row||!member(*row,family.sourceBytes,d))return reject(d,"same RA provider image source differs");
            family.sourceMember=row->name;return true;
        }
        return reject(d,"actual material image absent");
    }
    bool validateOrdinaryImage(ColdImage& image,const ColdFamily& family,
        const a::OriginalStandardTextureResource50LA& resource,Diagnostic& d){
        if(!resource.constructed24DA30()||resource.animated102070()||!resource.originalBytes()||
           *resource.originalBytes()!=family.sourceBytes||!resource.sourceMember()||
           normalized(*resource.sourceMember())!=normalized(family.sourceMember)||
           !resource.references00()||*resource.references00()<=0||!resource.levelCountB0()||
           *resource.levelCountB0()<1||*resource.levelCountB0()>5)return reject(d,"canonical ordinary image identity/levels");
        image.canonical=&resource;
        for(std::int32_t slot=0;slot<*resource.levelCountB0();++slot){
            a::OriginalStandardTextureLevelView50LA level;
            if(!resource.level100(static_cast<std::size_t>(slot),level)||level.width104<16||level.height110<16)
                return reject(d,"canonical source mip missing");
            image.levels.push_back(level);
        }
        const auto key=normalized(family.sourceMember);
        if(key.size()>=4&&key.substr(key.size()-4)==".pst"){
            a::Texture48E decoded;
            if(!a::decodeTexture48E(family.sourceBytes,decoded,d.decode)||decoded.kind!=1||
               decoded.mips.size()!=image.levels.size()||!resource.sourceKind()||*resource.sourceKind()!=1||
               !resource.formatAC()||*resource.formatAC()!=8||!resource.bitsPerPixelE8()||*resource.bitsPerPixelE8()!=8||
               !resource.flagsF8()||!resource.flagsF8KnownMask()||!(*resource.flagsF8KnownMask()&1u)||(*resource.flagsF8()&1u)||
               !resource.palette208()||std::memcmp(resource.palette208(),decoded.original_palette.data(),1024))
                return reject(d,"indexed source/palette mismatch");
            for(std::size_t i=0;i<image.levels.size();++i){const auto& level=image.levels[i];
                const auto ordinal=decoded.mips.size()-1-i;const auto& mip=decoded.mips[ordinal];
                if(level.sourceMipOrdinal!=ordinal||level.width104!=static_cast<std::int32_t>(mip.width)||
                   level.height110!=static_cast<std::int32_t>(mip.height)||level.sourceHeaderOffset!=mip.source_header_offset||
                   level.sourceIndicesOffset!=mip.source_indices_offset||!level.linearSourceIndices||
                   level.linearSourceBytes!=std::size_t(mip.width)*mip.height||
                   level.sourceIndicesOffset>family.sourceBytes.size()||
                   level.linearSourceBytes>family.sourceBytes.size()-level.sourceIndicesOffset||
                   std::memcmp(level.linearSourceIndices,family.sourceBytes.data()+level.sourceIndicesOffset,level.linearSourceBytes))
                    return reject(d,"indexed canonical mip source mismatch");
            }
            image.mips=std::move(decoded.mips);image.baseRrtex=std::move(decoded.base_rrtex);return true;
        }
        // Final canonical TGA levels, not a second image decoder. Both formats
        // retain their actual generated mip quirks.1555 sampled alpha uses the
        // canonical map's actual TEXA fields; no universal coverage->255 rule.
        if(key.size()<4||key.substr(key.size()-4)!=".tga"||!resource.formatAC()||
           !resource.flagsF8()||!resource.flagsF8KnownMask()||
           (*resource.flagsF8KnownMask()&5u)!=5u||(*resource.flagsF8()&5u)!=4u)
            return reject(d,"canonical exact TGA constructor fields");
        using F=a::OriginalUiGsField50KG;
        const auto ta0=family.context.fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)].value;
        const auto ta1=family.context.fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis1)].value;
        const auto aem=family.context.fields[static_cast<std::size_t>(F::methodOfExpandingTextureAlpha)].value;
        // Exact authenticated MetalGrate/TwiDesigns02 + actual DefaultMT
        // proof: TA0=TA1=255,AEM=0. Do not extend this to a generic sampler.
        if(*resource.formatAC()==1555&&(ta0!=255||ta1!=255||aem!=0))
            return reject(d,"unsupported actual RA1555 TEXA");
        const bool rgba32=family.sourceBytes.size()>17&&family.sourceBytes[16]==32;
        if(family.sourceBytes.size()<18||*resource.formatAC()!=(rgba32?8888:1555))return reject(d,"TGA original depth differs");
        const auto baseWidth=std::uint32_t(family.sourceBytes[12])|(std::uint32_t(family.sourceBytes[13])<<8);
        std::size_t expectedLevels=1;for(auto width=baseWidth;width>16;width>>=1)++expectedLevels;
        if(image.levels.size()!=expectedLevels)return reject(d,"TGA generated level count differs");
        for(std::size_t slot=0;slot<image.levels.size();++slot){const auto& level=image.levels[slot];
            const auto storage=rgba32?(slot?a::OriginalStandardTextureLevelStorage50LA::tgaGenerated8888:a::OriginalStandardTextureLevelStorage50LA::tgaConverted8888):
                (slot?a::OriginalStandardTextureLevelStorage50LA::tgaGenerated1555:a::OriginalStandardTextureLevelStorage50LA::tgaConverted1555);
            if(level.width104!=static_cast<std::int32_t>(baseWidth>>slot)||level.height110!=level.width104||level.storage!=storage||
               level.sourceMipOrdinal!=static_cast<std::uint32_t>(-1)||level.sourceIndicesOffset!=static_cast<std::size_t>(-1)||
               level.sourceHeaderOffset!=(slot?static_cast<std::size_t>(-1):0))return reject(d,"TGA final generated provenance differs");
            a::TextureMip48E mip;mip.width=static_cast<std::uint32_t>(level.width104);
            mip.height=static_cast<std::uint32_t>(level.height110);
            mip.source_header_offset=level.sourceHeaderOffset;mip.source_indices_offset=level.sourceIndicesOffset;
            const auto count=std::size_t(mip.width)*mip.height;mip.rgba.resize(count*4);
            if(*resource.formatAC()==8888){
                if(!level.rgba8888||level.rgba8888Bytes!=count*4)return reject(d,"canonical8888 extent");
                std::memcpy(mip.rgba.data(),level.rgba8888,count*4);
            }else if(*resource.formatAC()==1555){
                if(!level.packed1555||level.packed1555Words!=count)return reject(d,"canonical1555 extent");
                for(std::size_t i=0;i<count;++i){const auto word=level.packed1555[i];
                    mip.rgba[4*i]=static_cast<std::uint8_t>((word&31u)<<3);
                    mip.rgba[4*i+1]=static_cast<std::uint8_t>(((word>>5)&31u)<<3);
                    mip.rgba[4*i+2]=static_cast<std::uint8_t>(((word>>10)&31u)<<3);
                    mip.rgba[4*i+3]=static_cast<std::uint8_t>((word&0x8000u)?ta1:ta0);
                }
            }else return reject(d,"unsupported TGA final format");
            image.mips.push_back(std::move(mip));
        }
        return a::encodeTextureLevel48E(image.mips.front(),image.baseRrtex,d.decode)||reject(d,"canonical TGA image framing");
    }
    bool prepareImages(const std::array<std::vector<std::uint8_t>,6>& actualCrowdRrtex,Diagnostic& d)noexcept {
        d={};if(!staged_||closed_||imagesAttempted_)return reject(d,"world image stage phase");imagesAttempted_=true;
        try{
            const auto& provider=domain_->archiveProvider3E1A70();const auto& resolver=domain_->resolver3E1A70();
            if(!context_->live()||!provider->sealed()||provider->revision()!=providerRevision_)
                return reject(d,"image stage actual context/provider changed");
            // Reject any existing canonical crowd before source corpus mutation.
            // This fresh cold route cannot replace a warm adopted six-set.
            for(std::size_t i=0;i<domain_->textures383400()->rowCount();++i){a::OriginalStandardTextureRowView50LA row;
                if(i>32767||!domain_->textures383400()->row(static_cast<std::int16_t>(i),row))return reject(d,"texture row extent");
                if(row.object04&&row.object04->sourceMember()&&normalized(*row.object04->sourceMember())=="textures/crowd.atm")
                    return reject(d,"warm crowd reuse requires canonical retained-set join");
            }
            std::set<std::string> imageMembers;std::size_t indexed=0,tgas=0,atms=0;
            std::vector<a::OriginalStandardTextureSourceInput50LA> inputs;
            for(const auto& psm:psms_){
                ColdFamily family;family.materialMember=*psm.second->member();
                family.canonicalMaterialKey=psm.first;
                if(!a::decodeMaterial48E(*psm.second->originalBytes(),family.authored,d.decode)||family.authored.passes.size()!=1||
                   family.authored.passes[0].pass!=1||family.authored.passes[0].textures.size()!=1||
                   family.authored.passes[0].textures[0].slot!=1)return reject(d,"actual44-family single-map shape differs");
                const auto& name=family.authored.passes[0].textures[0].name;
                if(name.empty()||name.find_first_of("\\/.:\"")!=std::string::npos)return reject(d,"actual texture basename unsupported");
                family.requestedTexture="textures\\"+name;
                if(!selectImageSource(family,d))return false;
                const auto key=normalized(family.sourceMember);
                if(!imageMembers.insert(key).second)return reject(d,"actual44-family image identity duplicated");
                const auto ext=key.substr(key.size()-4);
                if(ext==".pst"){++indexed;inputs.push_back({family.sourceMember,family.sourceBytes});}
                else if(ext==".tga")++tgas;
                else if(ext==".atm"&&key=="textures/crowd.atm"){
                    std::size_t bad=0;
                    if(!a::detail::frameOriginalIpumAtm50LA(family.sourceBytes,family.compressedSourceFrames,bad)||
                       family.compressedSourceFrames.size()!=6)return reject(d,"crowd original frame extent mismatch");
                    for(const auto& frame:family.compressedSourceFrames)if(frame.count!=5)
                        return reject(d,"crowd original five-mip extent mismatch");
                    ++atms;inputs.push_back({family.sourceMember,family.sourceBytes});
                }
                else return reject(d,"unexpected source image family");
                auto materialName=family.materialMember;materialName.erase(materialName.size()-4);
                if(!domain_->materials382B38()->find298C80(materialName,family.materialRow,d.material))return false;
                imageFamilies_.push_back(std::move(family));
            }
            if(indexed!=40||tgas!=3||atms!=1||imageFamilies_.size()!=44)return reject(d,"actual image closure counts differ");
            const std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> noNewPsms;
            if(!domain_->appendOriginalSources(noNewPsms,inputs,d.material)||
               !ra_tga_source50LA::Verifier::append(*resolver,provider,d.material.texture))return reject(d,"same-provider image corpus rejected");
            std::size_t crowdSource=resolver->sourceCount();
            for(std::size_t i=0;i<resolver->sourceCount();++i)if(resolver->member(i)&&normalized(*resolver->member(i))=="textures/crowd.atm"){
                if(crowdSource!=resolver->sourceCount())return reject(d,"duplicate crowd source");crowdSource=i;}
            if(crowdSource==resolver->sourceCount()||!resolver->stageIpumFlags0(resolver,crowdSource,crowdCompressed_,d.material.texture)||
               !ra_crowd_image50LA::Verifier::verifyRaSixMip4(*resolver,crowdSource,crowdCompressed_,actualCrowdRrtex,crowdImages_,d.crowd)||
               !ra_crowd_image50LA::Verifier::bindVerifiedRaSix(*resolver,crowdImages_,context_,d.material.texture))
                return reject(d,"actual six-frame stage/lease/context rejected");
            // One terminal whole-domain phase. No later hidden registration or
            // rebuild, and no rollback of genuine partial constructor effects.
            if(!domain_->populate(context_,d.material))return reject(d,"canonical whole-domain population failed");
            imageCollector_=domain_->coldCollector14D440();
            coldImages_.reserve(49);
            for(std::size_t fi=0;fi<imageFamilies_.size();++fi){auto& family=imageFamilies_[fi];
                const a::OriginalStandardMaterialObject50LA* material=nullptr;a::OriginalStandardMaterialPassView50LA pass;
                if(!domain_->materials382B38()->resolve298E60(family.materialRow,material,d.material)||!material||
                   !material->source()||!material->source()->originalBytes()||
                   *material->source()->originalBytes()!=*psms_.at(normalized(family.materialMember))->originalBytes()||
                   !material->passCount48()||*material->passCount48()!=1||!material->pass4C(0,pass)||
                   !pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||!pass.state30||pass.state30->contextCount()!=1||
                   pass.constructionTemplate!=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::defaultMt)||
                   !domain_->materials382B38()->coldCollectorPass(imageCollector_,family.materialRow,0,family.collector))
                    return reject(d,"canonical family material/template/collector mismatch");
                std::int16_t program=-1;
                if(!pass.program18||!domain_->programs382B70()->find298C80(family.authored.passes[0].microprogram,program,d.material.program)||
                   program!=*pass.program18)return reject(d,"canonical source program mismatch");
                family.textureToken=(*pass.texture1A)[0];a::OriginalStandardTextureRowView50LA row;
                if(!domain_->textures383400()->row(family.textureToken,row)||!row.references08||*row.references08<=0||
                   !row.object04||!row.object04->sourceMember()||normalized(*row.object04->sourceMember())!=normalized(family.sourceMember)||
                   !row.object04->originalBytes()||*row.object04->originalBytes()!=family.sourceBytes)
                    return reject(d,"actual canonical family resource differs");
                family.firstImage=coldImages_.size();family.context=*pass.state30->context(0);family.constructionContext=family.context;
                const bool crowd=normalized(family.sourceMember)=="textures/crowd.atm";
                if(crowd){
                    a::OriginalStandardMaterialCollectorEntryView50LA finalLeaf;
                    if(family.collector.finalLeaf<0||
                       !domain_->materials382B38()->coldCollectorEntry(imageCollector_,static_cast<std::size_t>(family.collector.finalLeaf),finalLeaf)||
                       finalLeaf.material!=material||finalLeaf.pass!=0||finalLeaf.redirect18!=-1||
                       family.collector.registration!=family.collector.finalLeaf)
                        return reject(d,"unique crowd canonical leaf changed");
                    if(!pass.textureAnimation10||!pass.textureAnimation10->unique18()||
                       pass.textureAnimation10->materialOwner()!=material||pass.textureAnimation10->passOrdinal()!=0||
                       !row.object04->animated102070()||!row.object04->frameCount20()||*row.object04->frameCount20()!=6)
                        return reject(d,"actual crowd prototype/aggregate mismatch");
                    family.animatedCurrentMapPending=true;family.imageCount=6;
                    for(std::size_t f=0;f<6;++f){const auto* child=row.object04->frame102050(static_cast<std::int32_t>(f));
                        a::OriginalStandardTextureLevelView50LA level;const auto& lease=crowdImages_->frame(f);
                        for(std::size_t prior=family.firstImage;prior<coldImages_.size();++prior)
                            if(coldImages_[prior].canonical==child)return reject(d,"crowd child identity alias");
                        if(!child||!child->constructed24DA30()||!child->references00()||*child->references00()<=0||
                           !child->originalBytes()||*child->originalBytes()!=family.sourceBytes||
                           !child->sourceFormat()||*child->sourceFormat()!=a::OriginalStandardTextureSourceFormat50LA::pstIpum||!child->levelCountB0()||*child->levelCountB0()!=1||
                           !child->level100(0,level)||level.hostImage!=lease.get()||level.sourceMipOrdinal!=4||
                           level.compressed00!=lease->sourceFrame().allocation00||level.compressedBytes!=lease->sourceFrame().sourcePayloadBytes||
                           level.codec18!=lease->sourceFrame().codec18||level.codecBytes1C!=lease->sourceFrame().codecBytes1C||
                           level.sourceHeaderOffset!=lease->sourceFrame().sourceHeaderOffset||level.sourceIndicesOffset!=lease->sourceFrame().sourcePayloadOffset||
                           level.width104!=256||level.height110!=256||
                           !child->formatAC()||*child->formatAC()!=1555||!child->bitsPerPixelE8()||*child->bitsPerPixelE8()!=16||
                           !child->flagsF8()||!child->flagsF8KnownMask()||!(*child->flagsF8KnownMask()&1u)||!(*child->flagsF8()&1u))
                            return reject(d,"actual distinct canonical crowd child/lease mismatch");
                        ColdImage image;image.family=fi;image.frame=f;image.sourceMember=family.sourceMember;image.canonical=child;
                        image.levels.push_back(level);image.baseRrtex=lease->authenticatedRrtexBytes();
                        a::TextureMip48E mip;mip.width=256;mip.height=256;mip.source_header_offset=level.sourceHeaderOffset;
                        mip.source_indices_offset=level.sourceIndicesOffset;mip.rgba.assign(lease->rgba(),lease->rgba()+lease->rgbaBytes());
                        image.mips.push_back(std::move(mip));coldImages_.push_back(std::move(image));
                    }
                }else{
                    if(pass.textureAnimation10||row.object04->animated102070())return reject(d,"unexpected animated family");
                    a::OriginalStandardMaterialMapBindingView50LA map;
                    if(!domain_->materials382B38()->prepareColdRepresentativeMap249610(imageCollector_,family.materialRow,0,0,map,d.material)||
                       map.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||map.texture!=row.object04)
                        return reject(d,"actual ordinary final map mismatch");
                    family.context=map.contextAfter249610;family.contextAfter249610Available=true;family.imageCount=1;
                    ColdImage image;image.family=fi;image.sourceMember=family.sourceMember;
                    if(!validateOrdinaryImage(image,family,*row.object04,d))return false;
                    coldImages_.push_back(std::move(image));
                }
            }
            if(coldImages_.size()!=49||provider->revision()!=providerRevision_||
               !domain_->materials382B38()->coldCollectorCurrent(imageCollector_))return reject(d,"final image/collector extent changed");
            // Revalidate all retained source slots, not just the44 family representatives.
            for(const auto& placement:placements_){
                if(!placement||!placement->payload||!placement->payload->scene()||!placement->materials.complete()||
                   placement->materials.table382B38()!=domain_->materials382B38().get())
                    return reject(d,"retained placement material owner changed");
                const auto& names=placement->payload->scene()->materials;
                for(std::size_t slot=0;slot<names.size();++slot){const auto* token=placement->materials.handle1C4(slot);
                    std::int32_t actual=-1;
                    if(!token||!domain_->materials382B38()->find298C80("textures\\"+names[slot],actual,d.material)||*token!=actual)
                        return reject(d,"actual source placement slot changed");
                }
            }
            std::vector<a::MaterialTextureAssetInput50EA> assets;assets.reserve(49);
            std::size_t sourceBudget=0,rgbaBudget=0;
            for(const auto& image:coldImages_){
                const auto bytes=image.baseRrtex.size();
                if(bytes<16||bytes>a::maxMaterialTextureSourceBytes50EA-sourceBudget||
                   bytes-16>a::maxMaterialTextureRGBABytes50EA-rgbaBudget)return reject(d,"source image byte budget");
                sourceBudget+=bytes;rgbaBudget+=bytes-16;
            }
            for(std::size_t i=0;i<coldImages_.size();++i)assets.push_back({i+1,&coldImages_[i].baseRrtex});
            // Source-authenticated CPU image bank only. This is NOT a GL mip or
            // sampler policy; all source/canonical levels remain owned above.
            a::MaterialTextureAssetsInput50EA input{assets.data(),49,nullptr,0,
                {49,0,sourceBudget,rgbaBudget,1024}};
            if(!pixelBank_.load(input,d.images))return reject(d,"immutable full-world CPU image bank rejected");
            imagesComplete_=true;return true;
        }catch(...){return reject(d,"terminal world image failure; actual prefixes retained");}
    }
public:
    bool coldImagesCurrent()const noexcept{return imagesComplete_&&!closed_&&context_&&context_->live()&&
        domain_->archiveProvider3E1A70()->sealed()&&domain_->archiveProvider3E1A70()->revision()==providerRevision_&&
        domain_->materials382B38()->coldCollectorCurrent(imageCollector_);}
    const std::vector<ColdImage>* coldImages()const noexcept{return coldImagesCurrent()?&coldImages_:nullptr;}
    const std::vector<ColdFamily>* coldImageFamilies()const noexcept{return coldImagesCurrent()?&imageFamilies_:nullptr;}
    const a::MaterialTextureAssets50EA* coldPixels()const noexcept{return coldImagesCurrent()?&pixelBank_:nullptr;}
private:
