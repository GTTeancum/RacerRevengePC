// Included privately in the actual aggregate-owned native world. Source model
// identity is the retained Placement, not a caller pointer or a fake guest key.
    bool modelServiceTerminal_=false;
    bool crowdSurfaceProof(std::size_t surfaceIndex,const Placement*& placement,
        const ColdFamily*& family,Diagnostic& d)const noexcept {
        placement=nullptr;family=nullptr;
        try {
        if(modelServiceTerminal_||!coldImagesCurrent()||surfaceIndex>=surfaces_.size())
            return reject(d,"current full-world model surface required");
        const auto& span=surfaces_[surfaceIndex];
        if(span.placement>=placements_.size()||!placements_[span.placement])return reject(d,"owned placement missing");
        const auto& actual=*placements_[span.placement];const auto* scene=actual.payload?actual.payload->scene():nullptr;
        if(!scene||span.sourceLod!=0||scene->lods.empty()||span.sourceSurface>=scene->lods[0].surfaces.size()||
           span.worldInstance!=actual.worldInstance||surfaceIndex<actual.firstSurface||
           surfaceIndex-actual.firstSurface>=actual.surfaceCount||!actual.materials.complete()||
           actual.materials.table382B38()!=domain_->materials382B38().get())return reject(d,"actual source surface owner differs");
        const auto& original=scene->lods[0].surfaces[span.sourceSurface];
        const auto* token=actual.materials.handle1C4(original.material_index);
        if(original.material_index!=span.materialSlot||!token||*token!=span.materialRow||
           original.material_index>=scene->materials.size()||normalized(scene->materials[original.material_index])!="crowd")
            return reject(d,"actual crowd material slot proof rejected");
        for(const auto& candidate:imageFamilies_)if(candidate.materialRow==*token){
            if(family)return reject(d,"ambiguous actual crowd family");family=&candidate;
        }
        if(!family||normalized(family->materialMember)!="textures/crowd.psm"||
           normalized(family->sourceMember)!="textures/crowd.atm"||!family->animatedCurrentMapPending||
           family->imageCount!=6||family->firstImage>coldImages_.size()||
           family->imageCount>coldImages_.size()-family->firstImage||!crowdImages_||crowdImages_->count()!=6)
            return reject(d,"retained crowd family/six-set missing");
        a::OriginalStandardMaterialCollectorEntryView50LA leaf;
        if(!domain_->materials382B38()->coldCollectorPass(imageCollector_,*token,0,leaf)||
           leaf.registration!=family->collector.registration||leaf.finalLeaf!=leaf.registration)
            return reject(d,"actual unique crowd collector changed");
        const auto* material=leaf.material;a::OriginalStandardMaterialPassView50LA pass;
        if(!material||!material->source()||!material->source()->originalBytes()||
           *material->source()->originalBytes()!=*psms_.at(normalized(family->materialMember))->originalBytes()||
           !material->passCount48()||*material->passCount48()!=1||!material->pass4C(0,pass)||
           !pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]!=family->textureToken||
           !pass.textureAnimation10||!pass.textureAnimation10->unique18()||pass.animation10||
           pass.textureAnimation10->materialOwner()!=material||pass.textureAnimation10->passOrdinal()!=0||
           pass.constructionTemplate!=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::defaultMt))
            return reject(d,"actual retained model prototype differs");
        a::OriginalStandardTextureRowView50LA row;
        if(!domain_->textures383400()->row(family->textureToken,row)||!row.references08||*row.references08<=0||
           !row.object04||!row.object04->animated102070()||!row.object04->originalBytes()||
           *row.object04->originalBytes()!=family->sourceBytes||!row.object04->frameCount20()||*row.object04->frameCount20()!=6)
            return reject(d,"actual retained crowd aggregate changed");
        for(std::size_t f=0;f<6;++f){const auto* child=row.object04->frame102050(static_cast<std::int32_t>(f));
            const auto& image=coldImages_[family->firstImage+f];const auto& lease=crowdImages_->frame(f);
            a::OriginalStandardTextureLevelView50LA level;
            if(!child||child!=image.canonical||image.frame!=f||!lease||!child->constructed24DA30()||
               !child->level100(0,level)||level.hostImage!=lease.get()||level.compressed00!=lease->sourceFrame().allocation00||
               lease->resolverIdentity()!=domain_->resolver3E1A70().get())return reject(d,"actual six child/image leases changed");
        }
        placement=&actual;return true;
        }catch(...){placement=nullptr;family=nullptr;return reject(d,"model surface proof allocation failed");}
    }
public:
    struct ModelCrowdMap {
        std::size_t surface=0,worldInstance=0,imageIndex=0;
        a::OriginalStandardMaterialMapBindingView50LA binding;
        // Borrowed CPU views only, no accepted renderer submission/stamp.
        const a::MaterialTextureAsset50EA* image=nullptr;
        StaticFragmentDemands fragmentDemands;
    };
    // Explicit native-host model scheduling boundary. The original model
    // chain24182C/241848->246760->24A620 supplies its own model identity.
    // Here the private actual Placement owns its payload AND source1C4 slots;
    // no retail model object/ancestry or completion identity is fabricated.
    bool serviceCrowdSurface241848(std::size_t surfaceIndex,Diagnostic& d)noexcept {
        d={};const Placement* placement=nullptr;const ColdFamily* family=nullptr;
        if(!crowdSurfaceProof(surfaceIndex,placement,family,d))return false;
        // Real clock comes from this retained actual Application/Video context,
        // never a caller timestamp, frame delta, callback or second phase bank.
        const auto actualClock=context_->applicationAnimationClock();
        if(domain_->materials382B38()->serviceRetainedModelPrototype24A740(
            imageCollector_,family->materialRow,0,actualClock,d.material))return true;
        if(d.material.sourceEffectsCommitted)modelServiceTerminal_=true;
        return reject(d,"actual shared-prototype29AC90 service failed");
    }
    // Distinct read-only24B26C->29AF90->102050 operation. It neither services
    // time nor uses a currently serviced clone. It may select the genuine
    // initial phase when source service has not run; no fake service receipt.
    bool currentCrowdSurfaceMap24B26C(std::size_t surfaceIndex,ModelCrowdMap& output,Diagnostic& d)const noexcept {
        d={};const Placement* placement=nullptr;const ColdFamily* family=nullptr;
        if(!crowdSurfaceProof(surfaceIndex,placement,family,d))return false;
        ModelCrowdMap next;next.surface=surfaceIndex;next.worldInstance=placement->worldInstance;
        if(!domain_->materials382B38()->prepareRetainedModelMap24B26C(
            imageCollector_,family->materialRow,0,0,next.binding,d.material)||
           next.binding.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||
           next.binding.textureToken!=family->textureToken||!next.binding.texture)
            return reject(d,"actual model current-map selection failed");
        using F=a::OriginalUiGsField50KG;
        const auto& fields=next.binding.contextAfter249610.fields;
        if(fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)].value!=255||
           fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis1)].value!=255||
           fields[static_cast<std::size_t>(F::methodOfExpandingTextureAlpha)].value!=1)
            return reject(d,"selected crowd TEXA differs from verified image policy");
        bool found=false;
        for(std::size_t f=0;f<6;++f)if(next.binding.texture==coldImages_[family->firstImage+f].canonical){
            if(found)return reject(d,"selected crowd child has ambiguous image");
            next.imageIndex=family->firstImage+f;next.image=pixelBank_.texture(static_cast<std::uint32_t>(next.imageIndex));found=true;
            a::OriginalStandardTextureLevelView50LA level;
            if(!next.image||next.image->identity!=next.imageIndex+1||!next.binding.texture->level100(0,level)||
               level.hostImage!=crowdImages_->frame(f).get())return reject(d,"selected canonical child/image lease differs");
        }
        if(!found)return reject(d,"selected canonical child outside retained six-set");
        const auto fi=static_cast<std::size_t>(family-imageFamilies_.data());
        try {
            if(!decodeStaticMaterialProfile(fi,next.binding,next.fragmentDemands,d))return false;
        }catch(...){return reject(d,"current crowd profile observation allocation failed");}
        output=std::move(next);return true;
    }
private:
