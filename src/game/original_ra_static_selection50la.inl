namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::bindRaStaticSelection(const ::ra_static_model_cold50LA::Owner* owner,std::size_t instance,
    OriginalRacerModelSceneOwner50LA* model,const SceneGraph49P& graph,const void* observer,
    const LiveZoneMembership50EN* membership)noexcept {
    if(!raStaticVisibility(owner)||instance>=storage_->staticSelection.size()||!model||
       storage_->staticModels[instance]!=model||storage_->staticSelection[instance].model||
       !observer||model->model_.observer19C!=observer||!membership||!membership->attached||
       membership->object!=storage_->racerCount+2u+instance||membership->method!=ZoneMethod50CL::First||
       !validateSceneGraph49P(graph)||graph.count!=1||model->model_.sceneIndex!=static_cast<SceneNodeIndex49P>(0))return false;
    storage_->staticSelection[instance]={model,graph,observer,membership};return true;
}

bool Owner::selectRaModel26EFC0(const CameraVisibilityLease& lease,OriginalApplicationEntities50LA& app,
    OriginalRacerModelSceneOwner50LA& scene,const SceneGraph49P& graph,
    const LiveZoneMembership50EN* membershipOwner,const CameraViewOutput49F& camera,
    const OriginalCameraViewportFields& viewport,const assets::OriginalStandardMaterialTable50LA& materials,
    Diagnostic& d)noexcept {
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->zoneProjectionLease!=&lease||
       storage_->staticApplication!=&app||storage_->staticMaterials!=&materials||
       !scene.constructed2403A0()||scene.referenceReleased_||!validateSceneGraph49P(graph)||
       static_cast<std::uint32_t>(scene.model_.sceneIndex)>=graph.count||std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::identity,0x26efc0u);
    auto& s=*storage_;auto& model=scene.model_;auto& fields=scene.fields_;
    //240CA4's genuine null19C branch consumes the actual current camera.
    // A nonnull observer still requires its actual membership binding.
    if(bool(model.observer19C)!=bool(membershipOwner))return fail(d,Status::identity,0x240ca4u);
    namespace n=original_racer_model_view_detail50LA;
    auto product=[&](const BodyMatrix48G& world,const std::array<float,16>& projection,
                     std::array<float,16>& out)->bool {
        for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
            float v=n::mul(projection[c],world.v[4*r]);
            v=n::add(v,n::mul(projection[4+c],world.v[4*r+1]));
            v=n::add(v,n::mul(projection[8+c],world.v[4*r+2]));
            v=n::add(v,n::mul(projection[12+c],world.v[4*r+3]));
            if(!std::isfinite(v))return false;out[4*r+c]=v;
        }
        return true;
    };
        if(model.flags03&4u){model.flags191=static_cast<std::uint8_t>(model.flags191&~1u);return true;}
        if(!refreshSceneWorld49P(graph,model.sceneIndex))return fail(d,Status::arithmetic,0x2897b0);
        fields.flags1B2=static_cast<std::uint8_t>((fields.flags1B2&~0x88u)|4u); //240C44..90
        std::array<float,16> projection=camera.combined;
        if(app.useMembership3833D8_&&membershipOwner){
            const auto& membership=*membershipOwner;
            auto zone=membership.storage.links[0].zone;
            auto rowFor=[&](std::uint32_t ref)->Storage::ZoneProjection* {
                if(!ref||ref>s.world.zones.size())return nullptr;
                auto& row=s.zoneProjection[ref-1];
                return row.admissionA4&&*row.admissionA4==app.portalAdmission8BE0_&&row.bounds4C?&row:nullptr;
            };
            auto* row=rowFor(zone);if(!row)return fail(d,Status::phase,0x240cac);
            auto link=membership.storage.links[0].next;
            if(link){
                // Literal240CBC..240DCC: BOTH pairs consume4C/50, not54/58.
                std::array<float,4> bounds{{(*row->bounds4C)[0],(*row->bounds4C)[1],
                    (*row->bounds4C)[0],(*row->bounds4C)[1]}};
                std::size_t walked=0;
                while(link){
                    if(link>membership.storage.capacity||++walked>membership.storage.capacity||
                       !membership.storage.entries[link-1].used)return fail(d,Status::registration,0x240ce0);
                    const auto& node=membership.storage.links[link-1];auto* other=rowFor(node.zone);
                    if(!other)return fail(d,Status::phase,0x240ce4);
                    const float lo=(*other->bounds4C)[0],hi=(*other->bounds4C)[1];
                    bounds[0]=std::min(bounds[0],lo);bounds[1]=std::max(bounds[1],hi);
                    bounds[2]=std::min(bounds[2],lo);bounds[3]=std::max(bounds[3],hi);
                    if(bounds[0]==lo&&bounds[1]==hi&&bounds[2]==lo&&bounds[3]==hi)row=other;
                    link=node.next;
                }
                const float lo=(*row->bounds4C)[0],hi=(*row->bounds4C)[1];
                if(!(bounds[0]==lo&&bounds[1]==hi&&bounds[2]==lo&&bounds[3]==hi)){
                    PortalZoneViewOutput49F united;
                    if(!composePortalZoneView280720(camera,bounds,united))return fail(d,Status::arithmetic,0x240ddc);
                    projection=united.matrix60;row=nullptr;
                }
            }
            if(row){
                // The source may consume an older matrix. This native domain
                // refuses it explicitly; it does not replace it with current
                // bounds, a full-screen matrix, or an invisible result.
                if(!row->matrix60||!row->dispatchA6||*row->dispatchA6!=app.portalDispatch8BE4_)
                    return fail(d,Status::phase,0x240df0);
                projection=row->matrix60->matrix60;
            }
        }
        // The retained racer projection has not yet joined its aggregate-bounds
        // writer. Its constructor160/170 values must not become a fake cull.
        // Static bindings already publish the actual aggregate bounds together.
        if(!membershipOwner&&!model.radius198)return fail(d,Status::phase,0x240ef4u);
        std::array<float,16> clipMatrix;OriginalUiRiderBoxClip50KG clip;
        const auto& node=graph.nodes[static_cast<std::uint32_t>(model.sceneIndex)];
        if(!product(node.world100,projection,clipMatrix)||
           !projectOriginalUiRiderBoxClip50KG(clipMatrix,model.bounds160,model.bounds170,clip))
            return fail(d,Status::arithmetic,0x240f04);
        const bool selected=!clip.outside;auto remaining=clip.unionMask;
        if(selected){
            if(!remaining)fields.flags1B2|=8u;
            else {
                if(!product(node.world100,viewport.guardCombined2D0,clipMatrix)||
                   !projectOriginalUiRiderBoxClip50KG(clipMatrix,model.bounds160,model.bounds170,clip))
                    return fail(d,Status::arithmetic,0x240fec);
                remaining=clip.unionMask; // Guard's outside return is deliberately ignored.
                if(!remaining)fields.flags1B2|=app.guardInside4354F0_?128u:8u;
            }
        }
        remaining=static_cast<std::uint8_t>(remaining&~0x10u);
        if(selected)fields.flags1B2=static_cast<std::uint8_t>((fields.flags1B2&~5u)|(remaining?5u:0u));
        model.flags191=static_cast<std::uint8_t>((model.flags191&~1u)|(selected?1u:0u));
        if(selected){
            const auto radius=model.nodeCount1A0==1?model.radius194:model.radius198;
            const auto& center=model.nodeCount1A0==1?model.bounds140:model.bounds160;
            if(!radius||!std::isfinite(*radius)||*radius<0.f||!fields.geometry1B4||
               !fields.geometry1B4->constructed240110()||!fields.geometry1B4->payload()||
               !fields.geometry1B4->scale14())return fail(d,Status::identity,0x26f090);
            float distance=0;
            const std::array<float,3> local{{node.localC0.v[12],node.localC0.v[13],node.localC0.v[14]}};
            const std::array<float,3> cameraPosition{{camera.position.x,camera.position.y,camera.position.z}};
            if(!projectOriginalRacerDistance26F070(center,local,cameraPosition,distance))
                return fail(d,Status::arithmetic,0x26f190);
            fields.distance1BC=distance; // Original write precedes LOD/surface work.
            const float edge=n::sub(distance,*radius),denominator=std::max(.1f,edge);
            const float focal=viewport.focalPixels1D8;
            const float factor=n::mul(app.projectedRadius383368_,focal);
            const float numerator=n::mul(2.f,n::mul(*radius,factor));
            volatile float scale=focal/denominator,projectedRadius=numerator/denominator;
            const float threshold=n::mul(scale,app.modelLod383360_);
            if(!std::isfinite(edge)||!std::isfinite(focal)||!std::isfinite(factor)||
               !std::isfinite(numerator)||!std::isfinite(scale)||!std::isfinite(projectedRadius)||
               !std::isfinite(threshold))return fail(d,Status::arithmetic,0x26f1ec);
            const auto& payload=*fields.geometry1B4->payload();const auto* resource=payload.scene();
            if(!resource||resource->lods.empty()||resource->lods.size()>256)
                return fail(d,Status::bounds,0x26f248);
            std::size_t selectedLod=0;
            for(std::size_t lod=0;lod+1<resource->lods.size();++lod){
                const float cutoff=static_cast<float>(resource->lods[lod].cutoff);
                if(!std::isfinite(cutoff)||static_cast<double>(cutoff)!=resource->lods[lod].cutoff)
                    return fail(d,Status::arithmetic,0x26f21c);
                const float value=n::mul(*fields.geometry1B4->scale14(),n::mul(threshold,cutoff));
                if(!std::isfinite(value))return fail(d,Status::arithmetic,0x26f224);
                if(value<1.f)selectedLod=lod+1; // Every cutoff, no early exit.
            }
            fields.field1B0=static_cast<std::uint8_t>(selectedLod); //26F260
            for(std::size_t surface=0;surface<resource->lods[selectedLod].surfaces.size();++surface){
                if(fields.materialCount1C0<0||surface>=static_cast<std::size_t>(fields.materialCount1C0)||
                   !fields.materials1C4)return fail(d,Status::bounds,0x26f2ac);
                assets::OriginalStandardMaterialDiagnostic50LA material;
                //2461E0 installs3D20C0; slot10=246810. This operation uses
                //the same retained material manager, including demand failures.
                if(!materials.demandSurface246810(payload,selectedLod,surface,fields.materials1C4[surface],
                        projectedRadius,scale,material))return fail(d,Status::phase,material.sourcePC);
            }
        }
        ++app.processedModels278_; // Only after COMPLETE selected26F070 tail.
        return true;
}

bool Owner::selectRaStaticMethod44(CameraVisibilityLease& lease,OriginalApplicationEntities50LA& app,
    std::uint32_t methodRef,const CameraViewOutput49F& camera,const OriginalCameraViewportFields& viewport,
    const assets::OriginalStandardMaterialTable50LA& materials,const OriginalRendererDispatchFields& rendererFields,
    platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& rendererDiagnostic,
    Diagnostic& d)noexcept {
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->zoneProjectionLease!=&lease||
       storage_->staticApplication!=&app||storage_->staticMaterials!=&materials||
       !methodRef||methodRef>storage_->world.zones.size()*2u)
        return fail(d,Status::identity,0x27e880);
    auto& s=*storage_;const auto index=methodRef-1u;
    // Real method34 only. Dynamic registrations require their own genuine
    // searchable/racer callbacks; they are not silently treated as statics.
    if((index&1u)||s.zones[index/2u].firstMethod!=methodRef)
        return fail(d,Status::phase,0x27e880);
    auto& method=s.selectedMethods[index];auto& registry=s.registries[index];
    method.complete=false;
    if(!method.constructed||!registry.perBlock||registry.perBlock>1024||
       registry.blockCount>1024/registry.perBlock||registry.lastUsed>registry.perBlock||
       (!registry.blockCount&&registry.lastUsed!=registry.perBlock)||
       method.count00>method.capacity0C||method.capacity0C!=method.selected18.size()||
       std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::registration,0x27e894);
    auto selectModel=[&](Storage::StaticSelectionBinding& binding)->bool {
        auto& scene=*binding.model;auto& model=scene.model_;
        if(model.observer19C!=binding.observer||!binding.membership||!binding.membership->attached||
           !binding.membership->storage.links||!binding.membership->storage.entries||
           !binding.membership->storage.capacity||!binding.membership->storage.entries[0].used||
           scene.sourceRowCount()!=1||binding.graph.count!=1||model.sceneIndex!=static_cast<SceneNodeIndex49P>(0))
            return fail(d,Status::identity,0x26efc0u);
        return selectRaModel26EFC0(lease,app,scene,binding.graph,binding.membership,camera,viewport,materials,d);
    };
    for(std::uint32_t block=0;block<registry.blockCount;++block){
        if(!app.selectedLazy8BCC_){app.selectedAssert443090_=-1;app.selectedLazy8BCC_=true;}
        std::fill(method.selected18.begin(),method.selected18.begin()+method.count00,nullptr);
        method.count00=0; //27F050 reset occurs for EACH physical block.
        const auto count=block+1u==registry.blockCount?registry.lastUsed:registry.perBlock;
        for(std::uint32_t slot=0;slot<count;++slot){
            const auto word=registry.words[block*registry.perBlock+slot];
            if(word&2u)continue; // Deleted/free chain, not an object pointer.
            const auto identity=word&~1u;
            if(!identity||(identity&3u))return fail(d,Status::registration,0x27eb5c);
            const auto ref=identity>>2u;
            // Common rewrite (area E): no flare identity is registered in this
            // namespace any more; every track light flare is drawn by the renderer's
            // one shared flare pass from Owner::trackFlares() (the RA-only Light3
            // flare-identity route through ra_pod_scene50LA is retired).
            if(ref<s.racerCount+2u||ref-s.racerCount-2u>=s.staticSelection.size())
                return fail(d,Status::identity,0x27eb60); // No invented searchable object.
            auto& binding=s.staticSelection[ref-s.racerCount-2u];
            if(!binding.model||binding.model!=s.staticModels[ref-s.racerCount-2u]||
               !binding.membership||binding.membership->object!=ref)
                return fail(d,Status::identity,0x27eb60);
            auto& model=binding.model->model_;
            if(!(model.flags03&1u)||(model.flags03&4u))continue;
            if(model.type02!=method.type2C){
                if(model.type02==method.type2D)return fail(d,Status::phase,0x27ec18);
                continue;
            }
            if(!(model.entity8.flags29&1u))continue;
            if(!selectModel(binding))return false; // EmbeddedEntity40=2418F0 ->26EFC0.
            if(!(model.flags191&1u))continue; // Actual mutation, never bounds-only admission.
            if(method.count00==method.capacity0C){
                try {method.selected18.resize(method.capacity0C+30u,nullptr);}
                catch(...){return fail(d,Status::allocation,0x27efa4);}
                method.capacity0C+=30u;
            }
            method.selected18[method.count00++]=&model.entity8; //27EEA0, ordered duplicates survive.
        }
        std::int32_t result=0;
        const auto serviced=serviceOriginalRenderer244D60(rendererFields,false,renderer,rendererDiagnostic,result);
        if(serviced.status!=OriginalCameraStartStatus::complete)
            return fail(d,Status::phase,serviced.sourcePC); //27EC68 barrier; no substitute drain.
    }
    method.complete=true;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::selectRaDynamicMethod44(CameraVisibilityLease& lease,OriginalApplicationEntities50LA& app,
    std::uint32_t methodRef,std::array<OriginalRacerEntityCold50LA,8>& racers,const OriginalCameraSceneOwner& camera,
    const RaRacerSelectionDispatch50LA& dispatch,const OriginalRendererDispatchFields& rendererFields,platform::VehicleWindowGl50DS& renderer,
    platform::VehicleWindowDiagnostic50DS& rendererDiagnostic,Diagnostic& d)noexcept {
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->zoneProjectionLease!=&lease||
       storage_->staticApplication!=&app||!methodRef||methodRef>storage_->world.zones.size()*2u)
        return fail(d,Status::identity,0x27e880u);
    auto& s=*storage_;const auto index=methodRef-1u;
    if(!(index&1u)||s.zones[index/2u].secondMethod!=methodRef||s.racerCount!=racers.size())
        return fail(d,Status::identity,0x27e880u);
    auto& method=s.selectedMethods[index];const auto& registry=s.registries[index];
    method.complete=false;
    if(!method.constructed||!registry.perBlock||registry.perBlock>1024u||
       registry.blockCount>1024u/registry.perBlock||registry.lastUsed>registry.perBlock||
       (!registry.blockCount&&registry.lastUsed!=registry.perBlock)||
       method.count00>method.capacity0C||method.capacity0C!=method.selected18.size())
        return fail(d,Status::registration,0x27e894u);
    for(std::uint32_t block=0;block<registry.blockCount;++block){
        if(!app.selectedLazy8BCC_){app.selectedAssert443090_=-1;app.selectedLazy8BCC_=true;}
        std::fill(method.selected18.begin(),method.selected18.begin()+method.count00,nullptr);
        method.count00=0; // Each actual physical block, matching27F050.
        const auto count=block+1u==registry.blockCount?registry.lastUsed:registry.perBlock;
        for(std::uint32_t slot=0;slot<count;++slot){
            const auto word=registry.words[block*registry.perBlock+slot];
            if(word&2u)continue;
            const auto identity=word&~1u;
            if(!identity||(identity&3u))return fail(d,Status::registration,0x27eb5cu);
            const auto ref=identity>>2u;
            if(ref==lease.cameraObject30()){
                if(camera.object30!=ref||!camera.sessionGeneration||!camera.sceneKey||
                   !camera.searchableConstructed1143F0||!camera.registration34.attached||
                   camera.registration34.object!=ref||camera.registration34.method!=ZoneMethod50CL::Second)
                    return fail(d,Status::identity,0x27eb60u);
                // Real1143F0 cleared scene33 low3.27EB68 rejects bit0 clear
                // BEFORE a type or callback read. It is neither a racer nor
                // an unknown identity, nor a presumed successful scene callback.
                if(!(camera.searchableFlags33Low3&1u)||(camera.searchableFlags33Low3&4u))continue;
                return fail(d,Status::phase,0x27eb80u); // Future activation requires its actual writer/dispatch.
            }
            if(!ref||ref>racers.size())return fail(d,Status::identity,0x27eb60u);
            auto& racer=racers[ref-1u];d.racer=ref-1u;
            if(!racer.prepared()||racer.actualCounter423F60()!=&app.objectCount423F60()||
               racer.actualLock423F58()!=&app.attachmentLock423F58())
                return fail(d,Status::identity,0x27eb60u);
            if(!(racer.entityFlags33&1u)||(racer.entityFlags33&4u))continue;
            if(racer.entityType32==method.type2C)return fail(d,Status::identity,0x27eba8u);
            if(racer.entityType32!=method.type2D)continue;
            if(!(racer.racer.flags29&1u))continue;
            if(!dispatch.select(ref-1u,lease,d))return false; // Actual229AE0, never draw.
            // Only AFTER the whole callback returned, including its material
            //and both actual forests. Preserve source selected33 and duplicates.
            if(!(racer.entityFlags33&2u))continue;
            if(method.count00==method.capacity0C){
                try {method.selected18.resize(method.capacity0C+30u,nullptr);}
                catch(...){return fail(d,Status::allocation,0x27efa4u);}
                method.capacity0C+=30u;
            }
            method.selected18[method.count00++]=&racer.racer;
        }
        std::int32_t value=0;
        const auto barrier=serviceOriginalRenderer244D60(rendererFields,false,renderer,rendererDiagnostic,value);
        if(barrier.status!=OriginalCameraStartStatus::complete)return fail(d,Status::phase,barrier.sourcePC);
    }
    method.complete=true;return true; // Every admitted concrete callback returned.
}
}
