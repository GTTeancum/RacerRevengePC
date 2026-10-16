// Included privately in the retained static model Owner. No public model,
// geometry, material token, matrix, or readiness injection path.
bool completeResourcesAndPlacement(std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver,
    Diagnostic& d)noexcept {
    d={};if(!constructed_||closed_||resourcesAttempted_||!resolver||!canClose())
        return reject(d,"current private static construction prefix required");
    resourcesAttempted_=true;resolver_=std::move(resolver);
    try {
        for(std::size_t i=0;i<models_.size();++i){auto& m=*models_[i];d.placement=i;
            auto& fields=m.scene.fields();auto* references=m.source.materials;
            if(!references||!references->complete()||references->table382B38()!=materials_.get()||
               !references->count1C0()||*references->count1C0()>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
               !m.wrapper.descriptor10||m.wrapper.model8!=&m.scene||m.wrapper.type00!=1||
               m.wrapper.dispatch!=StandardWrapper::Dispatch::standard3D26F0)
                return reject(d,"actual wrapper/material publication inputs changed");
            a::OriginalStandardModelFileId50LA file;
            if(!resolver_->bindModelMember(*m.source.payload->member(),a::OriginalStandardModelFileKind50LA::psg,
                    *m.source.payload->originalBytes(),file,d.cache))return reject(d,"canonical PSG cache binding failed");
            a::OriginalStandardModelFileView50LA source;
            if(!resolver_->modelFile(file,source,d.cache)||!source.bytes||
               *source.bytes!=*m.source.payload->originalBytes())return reject(d,"canonical PSG cache bytes differ");
            fields.materialCount1C0=static_cast<std::int32_t>(*references->count1C0());
            fields.materials1C4=references->handle1C4(0);
            if(fields.materialCount1C0&&!fields.materials1C4)return reject(d,"actual material slot storage missing");
            if(source.geometry240110){
                auto* prototype=source.geometry240110->prototype1C();Model* prototypeModel=nullptr;
                for(std::size_t j=0;j<i;++j)if(&models_[j]->scene==prototype&&models_[j]->resourceRetained&&
                    models_[j]->geometry==source.geometry240110)prototypeModel=models_[j].get();
                if(!prototypeModel||source.geometry240110->payload()!=m.source.payload.get())
                    return reject(d,"warm cache prototype is outside this retained static owner");
                //26EB44..98 copies the prototype's material handles and retains
                // each. This adapter's earlier acquisitions must be identical;
                // no substituted material policy is admitted here.
                const auto& pf=prototype->fields();
                if(fields.materialCount1C0!=pf.materialCount1C0||
                   (pf.materialCount1C0&&!pf.materials1C4))
                    return reject(d,"warm prototype material count/storage differs");
                for(std::int32_t k=0;k<fields.materialCount1C0;++k)
                    if(fields.materials1C4[k]!=pf.materials1C4[k])
                        return reject(d,"warm prototype material handle differs");
                m.geometry=source.geometry240110;
                if(!m.geometry->retain104960(&m.scene,d.resource))return reject(d,"actual warm geometry reference failed");
                m.resourceRetained=true;
                d.scene=m.scene.loadWarmChildlessRows26EAC4(m.graph(),*prototype,prototypeModel->graph());
                if(!d.scene.okay())return reject(d,"actual warm childless row continuation rejected");
                m.rows=true;
                d.scene=m.scene.copyParameters2406E0(*prototype);
                if(!d.scene.okay())return reject(d,"actual warm quantization copy failed");
            }else {
                const auto& bytes=*m.source.payload->originalBytes();a::PsgHierarchyView49O hierarchy{};
                if(!a::readPsgHierarchy49O(bytes.data(),bytes.size(),hierarchy,d.hierarchy)||hierarchy.nodeCount!=1)
                    return reject(d,"retained original PSG hierarchy rejected");
                d.scene=m.scene.loadDirectRootSourceRows28CAC0(m.graph(),hierarchy);
                if(!d.scene.okay())return reject(d,"actual fresh direct-root PSG row construction failed");
                m.rows=true;
                d.scene=m.scene.loadQuantization240570(*m.source.payload->scene());
                if(!d.scene.okay())return reject(d,"actual source quantization failed");
                m.geometry=std::make_shared<a::OriginalStandardModelGeometry50LA>();
                if(!m.geometry->construct240110(m.source.payload,resolver_,file,&m.scene,d.resource))
                    return reject(d,"actual240110 resource/trailer rejected");
                m.resourceRetained=true;
            }
            fields.geometry1B4=m.geometry.get();
            if(!source.geometry240110&&!resolver_->publishGeometry1326A0(file,m.geometry,d.cache))
                return reject(d,"actual geometry cache publication failed");
            d.scene=m.scene.allocateSourceRowTable26EE9C();
            if(!d.scene.okay())return reject(d,"actual source-row table allocation failed");
            const auto& descriptor=*m.wrapper.descriptor10;
            // Actual fresh or bounded warm childless rows now own table1B8.
            // General hierarchy-clone26EA28/26EA50 remains unsupported.
            //27AC4C loads descriptor144: UseDynamicLighting, NOT UseLighting140.
            d.packets=m.packets.construct240740(m.scene,*m.geometry,descriptor.useDynamicLighting144);
            if(!d.packets.okay())return reject(d,"actual CPU packet construction failed");
            //Descriptor148 is initialized1 at27A234 and not overwritten by
            //this bounded Standard reader.27AC74..94 preserves all other bits.
            fields.flags1B3=static_cast<std::uint8_t>((fields.flags1B3&~0x10u)|0x10u);
            //27AD04..44 copies at most127 bytes, then writes a terminator.
            //The real descriptor stem admission above makes this <=54 bytes.
            m.scene.model_.name38=descriptor.modelName180.substr(0,127);
            //28AB90 copies XYZ only, then dirty/ready propagation. Preserve all
            //other local lanes in this native equivalent graph operation.
            auto positioned=m.node.localC0;
            for(unsigned a=0;a<3;++a)positioned.v[12+a]=descriptor.position[a];
            if(!g::setLocalMatrix49P(m.graph(),static_cast<I>(0),positioned))
                return reject(d,"actual position28AB90 preparation failed");
            //289EC0(a3=1,t0=0,t1=0): right=up x look, correctedLook=right x up;
            //normalize up/correctedLook independently, then right=up x look.
            //Shared pure native leaf implements this SAME numeric branch.
            g::BodyMatrix48G oriented;
            if(!g::originalUiUserRotatePose50KG(descriptor.position,descriptor.look,descriptor.up,oriented)||
               !g::setLocalMatrix49P(m.graph(),static_cast<I>(0),oriented))
                return reject(d,"actual authored orientation289EC0 rejected");
            m.placed=true;++d.models;
            //World caches remain lazy. No2897B0, observer28E8A0, selected191,
            //Entity append27AE38, or packet matrix preparation is inferred.
            //Keep descriptor10 alive: source frees it only after later joins.
            //The observer/parent/tail stages remain explicit and separate.
        }
        resourcesComplete_=true;return true;
    }catch(...){return reject(d,"resource/placement allocation failed; actual prefix retained");}
}

bool resourcesCanClose()const noexcept {
    for(const auto& item:models_)if(item&&!item->scene.released()){
        const auto& m=*item;const auto& f=m.scene.fields();
        if(!m.source.materials||!m.source.materials->canRelease26E4C0())return false;
        if(!m.source.materials->released26E4C0()&&f.materials1C4&&
           f.materials1C4!=m.source.materials->handle1C4(0))return false;
        if(f.packets1F0&&f.packets1F0!=&m.packets)return false;
        if(m.packets.constructed240740()&&!m.packets.preflight240450().okay())return false;
        if(f.geometry1B4&&f.geometry1B4!=m.geometry.get())return false;
        if(m.resourceRetained){
            if(!m.geometry||!m.geometry->constructed240110()||!m.geometry->references00())return false;
            std::size_t references=0;bool prototypeOwned=false;
            for(const auto& peer:models_)if(peer&&peer->resourceRetained&&peer->geometry==m.geometry){
                ++references;
                if(m.geometry->prototype1C()==&peer->scene)prototypeOwned=true;
                else if(!m.geometry->canReleaseForModel(&peer->scene))return false;
            }
            // All source references are genuinely owned here. Reverse source
            //placement order releases every warm user before its creator.
            if(!prototypeOwned||*m.geometry->references00()!=static_cast<std::int64_t>(references))return false;
        }
    }
    return true;
}
bool closeResources(Model& m)noexcept {
    if(m.scene.released()||!m.scene.model())return true;
    if(!m.referenceReleased){
        C* outside[2]{};std::size_t count=0;
        if(m.parentAttached){outside[count++]=&parents_[m.parent]->entity;
            if(m.scene.model_.entity8.previous10)outside[count++]=m.scene.model_.entity8.previous10;}
        const g::OriginalWreckComponentDomain50LA domain{generation_,count?outside:nullptr,count};
        // Reverse source order makes this the genuine last sibling.142590
        // repairs its actual parent's list; it does not clear stale own links.
        if(!m.scene.releaseModelReference142590(domain).okay())return false;
        m.referenceReleased=true;
    }
    if(m.packets.constructed240740()&&!m.packets.release240450().okay())return false;
    a::OriginalStandardMaterialDiagnostic50LA material;
    if(!m.source.materials->release26E4C0(material))return false;
    if(m.resourceRetained){a::OriginalStandardResourceDiagnostic50LA resource;
        if(!m.geometry->release26E598(&m.scene,*m.source.materials,resource))return false;
        m.resourceRetained=false;
    }
    if(!m.rowTableFreed){
        if(!m.scene.freeSourceRowTable26E5CC().okay())return false;
        m.rowTableFreed=true;
    }
    // Private-prefix cleanup explicitly removes281490 before observer backing
    // retirement.280920 itself only disposes backing/pool count, not registry.
    if(!retireObserver(m))return false;
    if(!m.scene.releaseSceneBase2896D0(m.graph()).okay())return false;
    return true;
}
