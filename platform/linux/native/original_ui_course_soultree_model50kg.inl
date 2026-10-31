// Private Course1538B0/154140 cold model payload. No Garage/E archive grant.
    struct CourseSoultreeModel {
        using I=game::SceneNodeIndex49P;
        std::shared_ptr<assets::OriginalStandardModelResource50LA> payload;
        assets::PsgHierarchyView49O hierarchy{};
        assets::OriginalUiMemberView50KG source{};
        std::unique_ptr<game::OriginalRacerModelSceneOwner50LA> scene;
        std::array<game::SceneNode49P,assets::maxModelNodes49O+1u> nodes{};
        std::array<I,assets::maxModelNodes49O> indices{};
        std::uint32_t nodeCount=0;
        assets::OriginalStandardMaterialReferences50LA materials;
        std::shared_ptr<assets::OriginalStandardModelGeometry50LA> geometry;
        game::OriginalRacerModelPackets50LA packets;
        std::string directoryStack140,modelPathStack30,resolvedPsg;
        bool directoryCleared=false,directoryRestored=false,scopeRetiring=false,childrenRetired=false;
        bool attempted=false,enrolled=false,attached=false,resourceHeld=false,complete=false,closed=false;
        bool referenceReleased=false,packetsReleased=false,materialsReleased=false,geometryReleased=false;
        bool tableReleased=false,materialArrayReleased=false;
        game::SceneGraph49P graph()noexcept{return {nodes.data(),nodeCount};}
        game::SceneGraph49P graph()const noexcept{return {const_cast<game::SceneNode49P*>(nodes.data()),nodeCount};}
    };
    bool loadCourseSoultreeModel154140(std::size_t i,CourseSoultreeModel& m,Diagnostic& d)noexcept {
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        // Exact aggregate identity is required; an arbitrary retained PSG is not a loader grant.
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i]||&courseSoultreeRows_[i]->model!=&m||m.attempted||m.closed||
           !courseSoultreeRows_[i]->animationEnrolled||!construction_||!construction_->registrationCurrent()||
           !issuer_||!loaderConfig_||!loaderConfig_->parsedCurrent())return fail(0x154140);
        const auto domain=issuer_->standardMaterialDomainOwner_;
        const auto context=issuer_->materialContextOwner_;
        const auto archive=loaderConfig_->archive_;
        if(!domain||!domain->prepared()||!context||!context->live()||!archive||
           archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)||
           !domain->materials382B38()->screenApplicationMatches(*context->materialApplication())||
           !domain->materials382B38()->canRetireNativeCollector29C300())return fail(0x154140);
        const auto provider=domain->archiveProvider3E1A70();const auto resolver=domain->resolver3E1A70();
        if(!provider||!provider->sealed()||!resolver)return fail(0x132820);
        // Actual154140 no-override path saves then clears the ONE source
        // directory. Failure retains that reached clear; no invented rollback.
        auto& f=courseSoultreeRows_[i]->animation60->fields_;
        if(!f.rigOverride188||*f.rigOverride188!=0)return fail(0x154140);
        std::string resolved;m.attempted=true;
        try {
            m.directoryStack140=issuer_->modelDirectory36B860_;
            issuer_->modelDirectory36B860_.clear();m.directoryCleared=true; //15417C.
            m.modelPathStack30=f.directory190+f.rig138; //154214/22C exact already read Config strings.
            // Native backing admission computes the future resolved member;
            // the actual26E820 path publication below still follows model ctor.
            resolved=courseSoultreeLower(m.modelPathStack30);
            const auto dot=resolved.find_last_of('.');
            if(dot==std::string::npos)resolved+=".psg";else resolved.replace(dot,std::string::npos,".psg");
        }catch(...){return fail(0x154140);}
        if(resolved!=courseSoultreeSourceRow(i)->psg)return fail(0x1542EC); // Native exact-input admission before source malloc.
        const auto source=archive->find(resolved);
        assets::OriginalStandardArchiveProvider50LA::MemberView winner;
        if(!source.identity||!source.data||provider->lookup132820(source.identity->name,winner)!=
           assets::OriginalStandardArchiveProvider50LA::Lookup::present||!winner.data||
           winner.size!=source.size||std::memcmp(winner.data,source.data,source.size))return fail(0x1542EC);
        try {
            // Native parse/backing admission is not a source model constructor receipt.
            m.payload=std::make_shared<assets::OriginalStandardModelResource50LA>();
            assets::OriginalStandardResourceDiagnostic50LA rd;
            if(!m.payload->prepare(source.identity->name,std::vector<std::uint8_t>(source.data,source.data+source.size),rd))return fail(0x1542EC);
            assets::GeometryDiagnostic49O geometryDiagnostic;
            if(!assets::readPsgHierarchy49O(source.data,source.size,m.hierarchy,geometryDiagnostic)||
               m.hierarchy.nodeCount!=2||m.payload->nodeCount()!=2)return fail(0x1542EC);
            m.source=source;m.nodeCount=3;m.indices[0]=static_cast<CourseSoultreeModel::I>(1);m.indices[1]=static_cast<CourseSoultreeModel::I>(2);m.attempted=true;
            // The Scope supplies the actual Application counter/lock/types and
            // generation; no secondary count or projection lifetime is created.
            game::OriginalRacerModelSceneBinding50LA binding;
            d.entity=construction_->prepareCourseModelBinding154140(i,m.indices.data(),2,binding);
            if(!d.entity.okay())return fail(d.entity.sourcePC);
            m.scene.reset(new(std::nothrow) game::OriginalRacerModelSceneOwner50LA); // Actual malloc210 at1542F0.
            if(!m.scene){courseSoultreeRows_[i]->animation60->fields_.model04=nullptr;return fail(0x154324);} // Source04NULL publication154314 before virtual1A8 dereference.
            const bool sourceActive=(courseSoultreeRows_[i]->animation60->entity220()->flags29&1u)!=0;
            auto step=m.scene->construct2403A0(binding,m.graph(),sourceActive);
            if(!step.okay())return fail(step.sourcePC);
            courseSoultreeRows_[i]->animation60->fields_.model04=m.scene->model(); // Actual154314, before loading.
            d.entity=construction_->enrollCourseModel2403A0(i,*m.scene);if(!d.entity.okay())return fail(d.entity.sourcePC);
            m.enrolled=true;
            m.resolvedPsg=resolved; //26E910 actual .slt→.psg after real model constructor.
            step=m.scene->constructSeparateRoot26E944(m.graph(),m.resolvedPsg);
            if(!step.okay())return fail(step.sourcePC);
            assets::OriginalStandardModelFileId50LA file;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!resolver->bindModelMember(*m.payload->member(),assets::OriginalStandardModelFileKind50LA::psg,
                *m.payload->originalBytes(),file,cache))return fail(cache.sourcePC);
            assets::OriginalStandardModelFileView50LA found;
            if(!resolver->modelFile(file,found,cache))return fail(cache.sourcePC);
            if(found.geometry240110)return fail(0x26EA10); // True warm owner unresolved: retain reached root/count, never reconstruct cold.
            step=m.scene->loadSourceRows28CAC0(m.graph(),m.hierarchy);if(!step.okay())return fail(step.sourcePC);
            assets::OriginalStandardMaterialPath50LA path{&issuer_->materialDirectory36B970_};
            auto& md=d.page.quad.material;
            const bool acquired=m.materials.acquire26E640(*m.payload,domain->materials382B38(),path,md);
            if(m.materials.started()){
                m.scene->fields().materialCount1C0=static_cast<std::int32_t>(*m.materials.count1C0());
                m.scene->fields().materials1C4=m.materials.allocationBase1C4();
            }
            if(!acquired)return fail(md.sourcePC);
            step=m.scene->loadQuantization240570(*m.payload->scene());if(!step.okay())return fail(step.sourcePC);
            m.geometry=std::make_shared<assets::OriginalStandardModelGeometry50LA>();
            if(!m.geometry->construct240110(m.payload,resolver,file,m.scene.get(),rd))return fail(rd.sourcePC);
            m.resourceHeld=true;m.scene->fields().geometry1B4=m.geometry.get();
            if(!resolver->publishGeometry1326A0(file,m.geometry,cache))return fail(cache.sourcePC);
            step=m.scene->allocateSourceRowTable26EE9C();if(!step.okay())return fail(step.sourcePC);
            const auto packet=m.packets.construct240740(*m.scene,*m.geometry,true);if(!packet.okay())return fail(packet.sourcePC);
            // Enrollment records the EXISTING model Entity contribution only;
            // source PSG rows remain scene-graph children, not Entity18 children.
            d.entity=construction_->attachCourseModel154348(i,*courseSoultreeRows_[i]->animation60,*m.scene);
            if(!d.entity.okay())return fail(d.entity.sourcePC);
            m.attached=true;courseSoultreeRows_[i]->animation60->fields_.model04=m.scene->model();
            m.complete=true;return true;
        }catch(...){return fail(m.scene?0x26E820:0x1542EC);}
    }
    bool preflightCourseSoultreeModelNative(std::size_t i,const CourseSoultreeModel& m)const noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i]||&courseSoultreeRows_[i]->model!=&m)return false;
        if(m.closed||!m.scene||!m.scene->model())return true;
        const auto domain=issuer_?issuer_->standardMaterialDomainOwner_:nullptr;
        if((m.enrolled||m.scopeRetiring)&&(!construction_||construction_->courseSoultreeRows_[i].model!=m.scene.get()))return false;
        if(!domain||!domain->materials382B38()->canRetireNativeCollector29C300())return false;
        if(!m.referenceReleased&&!m.scene->rowsLoaded_&&!m.materials.started()&&!m.resourceHeld&&!m.packets.constructed240740()&&!m.attached)
            return m.scene->preflightFreshUnlinkedNative(m.graph()).okay();
        if(m.scene->released_)return m.scopeRetiring&&m.referenceReleased&&m.scene->referenceReleased_&&
            m.scene->model_.entity().reference00==0&&construction_->courseSoultreeRows_[i].model==m.scene.get()&&construction_->courseSoultreeRows_[i].modelRetiring;
        if(m.childrenRetired){
            if(!m.referenceReleased||!m.scene->referenceReleased_||!m.scene->garageSceneChildrenRetired_||
               !m.scene->rowTableFreed_||m.scene->rowTable_||m.scene->model_.entity().reference00||
               m.graph().nodes[static_cast<std::size_t>(m.scene->model_.sceneIndex)].firstChild184!=game::SceneNodeIndex49P::none)return false;
        }else if(!m.scene->preflightRetainedScene2896D0(m.graph(),m.scene->model_.observer19C).okay())return false;
        if(!m.materialsReleased&&!m.materials.canRelease26E4C0())return false;
        if(m.scene->model_.observer19C&&!courseModelObserverCustodyCurrent(i,*m.scene,m.scene->model_.observer19C))return false;
        if(!m.packetsReleased&&m.packets.constructed240740()&&!m.packets.preflight240450().okay())return false;
        return !m.resourceHeld||(m.geometry&&m.geometry->holdsModelReference(m.scene.get())&&m.geometry->canReleaseForModel(m.scene.get()));
    }
    bool abortNativeCourseSoultreeModel(std::size_t i,CourseSoultreeModel& m,Diagnostic& d)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i]||&courseSoultreeRows_[i]->model!=&m)return false;
        if(m.closed)return true;
        if(!m.scene||!m.scene->model()){m.closed=true;return true;}
        if(!preflightCourseSoultreeModelNative(i,m))return false;
        if(m.scene->released_){if(m.scopeRetiring){d.entity=construction_->finishCourseModelRetirement(i,*m.scene);if(!d.entity.okay())return false;m.scopeRetiring=false;}m.closed=true;m.complete=false;return true;}
        if(!m.referenceReleased&&!m.scene->rowsLoaded_&&!m.materials.started()&&!m.resourceHeld&&!m.packets.constructed240740()&&!m.attached){
            if(m.enrolled){d.entity=construction_->abortNativeCourseFreshModel(i,*m.scene,m.graph());if(!d.entity.okay())return false;m.enrolled=false;}
            else if(!m.scene->closeFreshUnlinked(m.graph()).okay())return false;
            m.closed=true;m.complete=false;return true;
        }
        if(!m.referenceReleased&&m.enrolled&&!preflightCourseModelObserverRetirement(i,*m.scene))return false;
        if(!m.referenceReleased){
            if(m.enrolled){d.entity=construction_->retireCourseModel142590(i,*m.scene);if(!d.entity.okay())return false;m.enrolled=false;m.scopeRetiring=true;}
            else {
                const auto* c=m.scene->modelEntity8();
                if(!c||c->parent1C||c->previous10||c->next14||c->firstChild18)return false;
                if(!m.materials.started()&&!m.resourceHeld){
                    const auto r=m.scene->closeFreshUnlinked(m.graph());if(!r.okay())return false;m.closed=true;return true;
                }
                const game::OriginalWreckComponentDomain50LA detached{construction_->generation_,nullptr,0};
                if(!m.scene->releaseModelReference142590(detached).okay())return false;
            }
            m.referenceReleased=true;m.attached=false;
        }
        // Actual independently owned payloads, retained phases if a later step refuses.
        if(!m.packetsReleased){if(m.packets.constructed240740()&&!m.packets.release240450().okay())return false;m.packetsReleased=true;}
        if(!m.materialsReleased){if(!m.materials.release26E4C0(d.page.quad.material))return false;m.materialsReleased=true;}
        if(!m.geometryReleased){if(m.resourceHeld){assets::OriginalStandardResourceDiagnostic50LA rd;
                if(!m.geometry->release26E598(m.scene.get(),m.materials,rd))return false;m.resourceHeld=false;}m.geometryReleased=true;}
        if(!m.tableReleased){if(!m.scene->freeSourceRowTable26E5CC().okay())return false;m.tableReleased=true;}
        if(!m.materialArrayReleased){if(m.materials.started()&&!m.materials.freeAllocation26E5EC())return false;m.materialArrayReleased=true;}
        const void* historicalObserver=m.scene->model_.observer19C;
        if(!m.childrenRetired){if(!m.scene->retireGarageSceneChildren2896D0(m.graph(),historicalObserver).okay())return false;m.childrenRetired=true;}
        if(!abortCourseModelObserverNative(i))return false;
        if(historicalObserver&&!courseModelObserverRetired(i,*m.scene,historicalObserver))return false;
        if(!m.scene->finishGarageSceneBase1413E0(m.graph(),historicalObserver).okay())return false;
        if(m.scopeRetiring){d.entity=construction_->finishCourseModelRetirement(i,*m.scene);if(!d.entity.okay())return false;m.scopeRetiring=false;}
        m.closed=true;m.complete=false;return true;
    }
    bool finishCourseSoultreeModelPath1543A8(std::size_t i)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i])return false;auto& m=courseSoultreeRows_[i]->model;
        if(!m.directoryCleared||m.directoryRestored||!issuer_||!issuer_->modelDirectory36B860_.empty())return false;
        try {issuer_->modelDirectory36B860_=m.directoryStack140;}catch(...){return false;}
        m.directoryRestored=true;return true;
    }
