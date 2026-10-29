//Included after actual cold World/model/particle owners and caller definitions.
//Only the authenticated RA four-type8 route. No runtime emission capability.
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeParticleServices274370(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached)return false;
    pc=0x1aa2c4;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeEmptyToyService1B4328Complete_||!nativeCollisionServiceComplete_||
       !nativeCollisionAnimationDirectory1AA2A0_||app.modelDirectory36B860()!="anim\\"||
       !nativeEmptyAnimations2713E0Complete_||nativeEmptyToyList3C8_||
       !nativeSceneInfo270900Current(app,pc)||!worldApplication_||
       worldApplication_->attachmentLock423F58()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       !world_||!world_->modelPrefixes_||world_->closed_||world_->source_!=source_||
       !world_->domain_||!world_->context_||!world_->context_->live()||
       !runtimeQueries_||!particles_||particles_->closed_||!particles_->decoded_||
       particles_->source_!=source_||particles_->application_!=worldApplication_||
       particles_->generation_!=worldEntity_.sessionGeneration||particles_->constructed_!=4u||
       !nativeParticleList3D8_)return false;
    auto& list=*nativeParticleList3D8_;
    if(list.sourceMode||list.serviceTerminal||!list.complete||!list.counted||list.count30!=4||
       list.published!=4u||list.loaded!=4u||!list.pointers34||
       list.actualOwner!=particles_.get()||list.servicesCompleted>4u||
       list.entity.reference00!=1||list.entity.objectKey!=key(&list.entity)||
       list.entity.sessionGeneration!=worldEntity_.sessionGeneration||
       list.entity.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       list.entity.category28!=0xffu||(list.entity.flags29&15u)!=3u||list.entity.nativeFlags29||
       list.entity.parent1C||list.entity.previous10||list.entity.next14||list.entity.mask20||
       list.entity.descendants24!=(list.servicesCompleted?0x402u:0u)||
       list.entity.firstChild18!=(list.servicesCompleted?&particles_->emitters_[0].entity:nullptr))return false;
    auto& models=*world_->modelPrefixes_;
    const auto* data=source_->sourcesDecoded();
    const auto& provider=world_->domain_->archiveProvider3E1A70();
    if(!data||data->world.instances.size()!=1261u||data->world.zones.size()!=10u||
       !provider||!provider->sealed()||provider->revision()!=world_->providerRevision_||
       models.closed_||models.source_!=source_||models.application_!=worldApplication_||
       models.generation_!=worldEntity_.sessionGeneration||models.parents_.size()!=10u||
       models.models_.size()!=1085u||!models.canClose())return false;
    const auto display=runtimeQueries_->nativeColdModelDisplay274160(*worldApplication_,*source_,worldEntity_.sessionGeneration);
    std::array<std::int32_t,4> rectangle{};std::uint64_t selection=0;
    if(!display||!runtimeQueries_->camera_||!runtimeQueries_->camera_->session||
       world_->rendererPublication_!=&display->renderer_||!world_->imagePublication_||
       world_->imagePublication_->raStaticWorldOwner()!=world_.get()||
       !display->readColdGameplayViewport115990(*runtimeQueries_->camera_->session,rectangle,selection))return false;
    g::OriginalGcParticleManager50KG* numeric=nullptr;Entity* managerEntity=nullptr;
    if(!nativeEmitterManager274370Current(numeric,managerEntity,pc)||!numeric||!managerEntity)return false;
    const auto asciiEqual=[](const std::string& a,const std::string& b)noexcept {
        if(a.size()!=b.size())return false;
        for(std::size_t n=0;n<a.size();++n){auto x=static_cast<unsigned char>(a[n]);auto y=static_cast<unsigned char>(b[n]);
            if(!x||!y||x>=128||y>=128)return false;
            if(x>='A'&&x<='Z')x=static_cast<unsigned char>(x+32);
            if(y>='A'&&y<='Z')y=static_cast<unsigned char>(y+32);
            if(x!=y)return false;}
        return true;
    };
    std::array<const g::OriginalRacerSceneObject50LA*,4> attachments{};
    try {
        //Pure whole-source admission BEFORE new effects. Missing native model
        //slots are not claimed retail-null: exhaustive authored name comparison
        //proves they cannot match any admitted attachment. Never generalize.
        for(std::size_t i=0;i<4u;++i){
            const auto& actual=particles_->attachments_[i];const auto& row=list.rows[i];
            const auto& e=particles_->emitters_[i].entity;
            if(!row||list.pointers34[i]!=row.get()||actual.type!=8u||actual.instances!=1u||
               actual.zone!=1u||actual.ordinal!=366u+i||actual.worldInstance!=365u+i||
               particles_->emitters_[i].retired||e.reference00!=1||e.sessionGeneration!=worldEntity_.sessionGeneration||
               e.objectKey!=key(&e)||e.kind!=g::OriginalWreckComponentKind50LA::effect15DE00||
               e.firstChild18||e.descendants24)return false;
            std::size_t matches=0;
            for(std::size_t z=0;z<10u;++z){
                const auto& parent=models.parents_[z];
                if(!parent||parent->released||!parent->constructed||parent->zone!=z+1u||
                   parent->entries34.size()!=data->world.zones[z].models)return false;
                for(std::size_t n=0;n<parent->entries34.size();++n){
                    const auto& slot=parent->entries34[n];
                    if(slot.worldInstance>=data->world.instances.size())return false;
                    const auto& instance=data->world.instances[slot.worldInstance];
                    if(instance.zone!=z+1u||instance.ordinal!=n+1u||instance.parameter_block>=data->world.parameters.blocks.size())return false;
                    const Field* slt=nullptr;
                    if(!findUniqueField(data->world.parameters.blocks[instance.parameter_block],"slt",slt)||!slt)return false;
                    const auto dot=slt->value.find('.');
                    if(dot==std::string::npos||!dot||dot>50u)return false;
                    const auto sourceName=slt->value.substr(0,dot)+".slt";
                    if(!asciiEqual(sourceName,actual.soulTree))continue;
                    ++matches;
                    if(slot.worldInstance!=actual.worldInstance||!slot.model||
                       slot.model->source.worldInstance!=slot.worldInstance||!slot.model->parentAttached||
                       slot.model->referenceReleased||!slot.model->placed||
                       !slot.sourceInitComplete||!slot.model->geometry||slot.model->scene.released())return false;
                    const auto* model=slot.model->scene.model();
                    if(!model||!asciiEqual(model->name38,actual.soulTree)||
                       model->entity().reference00!=1||model->entity().sessionGeneration!=worldEntity_.sessionGeneration)
                        return false;
                    attachments[i]=model;
                }
            }
            if(matches!=1u||!attachments[i])return false;
            if(i<list.servicesCompleted){
                if(!particles_->emitters_[i].enrolled||!row->descriptorRetired||row->descriptor04||!row->emitterPointers00||
                   row->emitterPointers00[0]!=&e||!row->emitterPublished||!row->emitterEnrolled||
                   !row->configured||!row->started||!row->enabled||!row->attached||
                   row->attachment74!=attachments[i]||row->unknown08!=8u||row->unknown0C!=1u||row->unknown10!=1u||
                   e.parent1C!=&list.entity||e.mask20!=0x402u||
                   e.previous10!=(i?&particles_->emitters_[i-1u].entity:nullptr)||
                   e.next14!=(i+1u<list.servicesCompleted?&particles_->emitters_[i+1u].entity:nullptr))return false;
            }else {
                const auto* d=row->descriptor04.get();
                if(particles_->emitters_[i].enrolled||!d||!d->loaded||d->actualAttachment!=&actual||d->type04!=8||d->startingZone00!=1||
                   d->instances330!=1||d->startOn334!=1||!d->child10C||!asciiEqual(*d->child10C,"NONE")||
                   !asciiEqual(d->attach08,actual.soulTree)||!asciiEqual(d->emitter210,"rafire.emt")||
                   !d->offset320||*d->offset320!=std::array<float,3>{{0.f,15.f,0.f}}||
                   row->emitterPointers00||row->emitterPublished||row->emitterEnrolled||row->configured||
                   row->started||row->enabled||row->attached||row->descriptorRetired||row->attachment74||
                   row->unknown08||row->unknown0C||row->unknown10||e.parent1C||e.previous10||e.next14||e.mask20)
                    return false;
            }
        }
        if(list.serviceComplete){if(list.servicesCompleted!=4u)return false;return continueNativeTriggerServices274E90(app,pc);}
        if(list.serviceEntered)return false;
        //Terminal-on-native-failure envelope: every published allocation/borrow
        //is retained for real cleanup. No retry replays source stores or loads.
        list.serviceEntered=true;list.serviceTerminal=true;
        for(;list.servicesCompleted<4u;++list.servicesCompleted){const auto i=list.servicesCompleted;
            auto& row=*list.rows[i];auto& d=*row.descriptor04;auto& emitter=particles_->emitters_[i];
            pc=0x27c81c;row.unknown08=static_cast<std::uint32_t>(d.type04);
            //Source precondition World4A0 was authenticated above. Native
            //validation is not a fabricated443068/debug-assert observation.
            pc=0x27ca44;auto pointers=std::make_unique<Entity*[]>(1u);
            //Explicit existing counted-owner adoption at27CA84: do not replay
            //15DE00, reference init, fractional224, control words or count.
            pc=0x27ca84;pointers[0]=&emitter.entity;
            pc=0x27caa0;row.emitterPointers00=std::move(pointers);row.emitterPublished=true;
            pc=0x27cbe0;row.unknown0C=static_cast<std::uint32_t>(*d.instances330);
            pc=0x27cbf0;row.unknown10=static_cast<std::uint32_t>(*d.startOn334);
            ra_particle_authoring50LA::Owner::Diagnostic diagnostic;
            //Native lifetime reservation, NOT an invented source row+74.
            //Loader may install real emitter74 before a later resource failure.
            row.attachment74=attachments[i];
            pc=0x27da18;
            if(!particles_->loadEmitter15DEF0(i,*numeric,*managerEntity,*attachments[i],provider,
                app.particleDirectory36B530(),d.emitter210,*d.offset320,d.offsetW32C,diagnostic,pc))return false;
            row.configured=true;
            //Nonnull74 skips BOTH27DA30/40 position writes. Preserve actual
            //position30/previous40 and opaque W, not an imagined world refresh.
            pc=0x27da5c;g::startEffect50BC(emitter.controls);row.started=true;
            pc=0x27da6c;emitter.entity.flags29=static_cast<std::uint8_t>((emitter.entity.flags29&~3u)|3u);
            //141560 tail1414C0 has no children on this authenticated owner.
            row.enabled=true;
            if(!enrollNativeParticleEntity274370(emitter.entity,pc))return false;
            emitter.enrolled=true;row.emitterEnrolled=true;
            pc=0x27ce6c;
            if(!attachNativeParticleEntity27CE6C(list.entity,emitter.entity,pc))return false;
            row.attached=true;
            //Exactly one authored match and actual3C8 null: no Toy traversal.
            //Count assertion native admission does not synthesize global443070.
            pc=0x27d754;row.descriptor04.reset();row.descriptorRetired=true;
            pc=0x27d790;
        }
        list.serviceComplete=true;list.serviceTerminal=false;
        //27E044 returns1,2743A0 returns1,caller1AA2C8 delay-slot returns.
        return continueNativeTriggerServices274E90(app,pc); //Actual next World+144 continuation; no fullWorld.
    }catch(...){return false;}
}
}
