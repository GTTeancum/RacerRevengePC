//Raw body.scene288 cache, as29650C/296884 consume it. No scene getter,
//clean-cache requirement, parent traversal or root snapshot substitution.
    bool readSharedSpringRawScene(const SharedOwnerLease& lease,g::BodyMatrix48G& output)const noexcept {
        if(!sharedLeaseCurrent(lease)||!actual_||!gridArena_||!lease.producer_->activeSession)return false;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        if(!racer.prepared()||!racer.body340||racer.body340!=stagedPods694_[lease.activeSlot_].bodyA0||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main)return false;
        const auto* scene=racer.body340->scene288();
        if(!scene||scene!=racer.scene190||!scene->constructed||scene->released||
           !scene->entity().reference00||scene->entity().sessionGeneration!=session.generation_||
           scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
           !g::validVehicleSceneCount49W(actual_->frame,gridArena_->bindings))return false;
        const auto index=static_cast<int>(scene->sceneIndex);
        if(index<0||static_cast<std::size_t>(index)>=actual_->frame.sceneNodeCount)return false;
        const auto& node=actual_->frame.nodes[index];
        if(!registeredRange(&node,sizeof(node)))return false;
        std::memcpy(&output,&node.world100,sizeof(output));return true;
    }
    bool observeSharedSpringSentinel(SharedTerrainInvocation& call,unsigned which)noexcept {
        if(which>1u||!sharedLeaseCurrent(call.lease_)||!call.springEntered_)return false;
        const auto app=call.lease_.producer_->activeSession->actual_.applicationEntities;
        if(!app||app->nativeOwnerThread_!=std::this_thread::get_id())return false;
        if(!markSharedDirectQueryEffect(call.lease_))return false;
        auto& word=app->nativeSpringSentinelWords_[which];
        call.result_.stoppedPC=which?0x29686c:0x2965b0;
        if(!word)word=0xffffffffu;
        return *word==0xffffffffu; //Native first-use identity, not guest guard bytes.
    }
    //2969F4/296F10 only. Scratch results are not another body/scene owner.
    //The caller publishes native120 after this exact helper returns.
    bool prepareSharedSpringVelocity(SharedTerrainInvocation& call,
        const g::SpringRecordGeometryPrefix48K& geometry,const g::Vector3_48C& normal,
        std::uint32_t otherIdentity,const g::Vector3_48C& center,
        g::Vector3_48C& contactPoint,g::Vector3_48C& velocity)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(!call.springEntered_||!sharedLeaseCurrent(lease)||out.stoppedPC!=0x2969f4||
           !actual_||!gridArena_||std::fegetround()!=FE_TONEAREST)return false;
        bool valid=true;
        const auto keep=[&](float v){valid=valid&&std::isfinite(v);return v;};
        const auto add=[&](float a,float b){keep(a);keep(b);volatile float v=a+b;return keep(v);};
        const auto sub=[&](float a,float b){keep(a);keep(b);volatile float v=a-b;return keep(v);};
        const auto mul=[&](float a,float b){keep(a);keep(b);volatile float v=a*b;return keep(v);};
        const auto cross=[&](const g::Vector3_48C& a,const g::Vector3_48C& b){
            g::Vector3_48C v{};for(unsigned k=0;k<3;++k)v.v[k]=sub(
                mul(a.v[(k+1)%3],b.v[(k+2)%3]),mul(a.v[(k+2)%3],b.v[(k+1)%3]));return v;
        };
        const auto xyz=[](const g::VisualVector49R& v){g::Vector3_48C r;
            std::memcpy(r.v,v.v,sizeof(r.v));return r;};
        const auto vector=[](const g::Vector3_48C& v){return g::VisualVector49R{{v.v[0],v.v[1],v.v[2],1.f}};};
        g::Vector3_48C point{},arm{};
        for(unsigned k=0;k<3;++k)point.v[k]=add(geometry.worldPoint.v[k],mul(geometry.depth,normal.v[k]));
        if(!valid)return false;
        out.stoppedPC=0x296f10;
        for(unsigned k=0;k<3;++k)arm.v[k]=sub(point.v[k],center.v[k]);
        if(!valid)return false;
        const auto& session=*lease.producer_->activeSession;
        auto& frame=actual_->frame;auto& root=frame.bodies.main.components.root;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        const auto primaryClean=[&](){
            g::BodyMatrix48G matrix;
            if(!readSharedSpringRawScene(lease,matrix)||!racer.scene190)return false;
            const auto i=static_cast<std::size_t>(racer.scene190->sceneIndex);
            return (frame.nodes[i].sceneFlags190&1u)&&
                !std::memcmp(&matrix,&root.scene,sizeof(matrix));
        };
        g::detail::SceneVectorContext49U context{1.f};g::VisualVector49R local{},rotational{};
        out.stoppedPC=0x296fb4;
        if(!primaryClean()||!g::detail::inverseDirection49U({frame.nodes,frame.sceneNodeCount},
            racer.scene190->sceneIndex,vector(arm),context,local))return false;
        const auto spin=cross(root.body.angularVelocity,xyz(local));if(!valid)return false;
        out.stoppedPC=0x296ff4;
        if(!primaryClean()||!g::detail::direction49U({frame.nodes,frame.sceneNodeCount},
            racer.scene190->sceneIndex,vector(spin),context,rotational))return false;
        g::Vector3_48C first{},second{};
        for(unsigned k=0;k<3;++k)first.v[k]=add(rotational.v[k],root.body.velocity.v[k]);
        if(!valid)return false;
        if(!otherIdentity){
            out.stoppedPC=0x29703c;
            const auto app=session.actual_.applicationEntities;
            if(!app||app->nativeOwnerThread_!=std::this_thread::get_id())return false;
            std::memcpy(second.v,app->springZero4435A0_.data(),sizeof(second.v));
        }else{
            out.stoppedPC=0x29704c;
            auto* registry=lease.producer_->bodyRegistry;
            if(!registry||!registry->prepared()||registry->count()!=lease.bank_->count_)return false;
            std::optional<std::size_t> slot;
            for(std::size_t i=0;i<registry->count();++i)
                if(registry->bodyIdentityForSlot(i).value==otherIdentity){if(slot)return false;slot=i;}
            if(!slot||*slot>=session.racerEntities_.size()||!rosterSlotComplete_[*slot])return false;
            const auto& other=session.racerEntities_[*slot];const auto& staged=stagedPods694_[*slot];
            if(staged.slot!=*slot||!other.prepared()||!other.body340||other.body340!=staged.bodyA0||
               other.racer.sessionGeneration!=session.generation_||
               staged.generation!=session.generation_||!other.body340->entity().reference00||
               other.body340->entity().parent1C!=&other.racer||
               other.body340->entity().sessionGeneration!=session.generation_||
               other.body340->canonicalNumericAddress()!=&frame.bodies.main||
               !other.scene190||other.body340->scene288()!=other.scene190||
               !other.scene190->constructed||other.scene190->released||
               !other.scene190->entity().reference00||
               other.scene190->entity().sessionGeneration!=session.generation_||
               other.scene190->sceneIndex!=gridArena_->bindings.lifecycle.main190)return false;
            const auto index=static_cast<int>(other.scene190->sceneIndex);
            if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return false;
            const auto read=[&](void* address,std::size_t bytes,void* output){
                auto* span=sharedSlotSpan(lease,*slot,address,bytes);
                if(!span)return false;std::memcpy(output,span,bytes);return true;
            };
            decltype(frame.sceneNodeCount) savedCount=0;g::SceneNodeIndex49P savedMain;
            if(!read(&frame.sceneNodeCount,sizeof(savedCount),&savedCount)||
               !read(&gridArena_->bindings.lifecycle.main190,sizeof(savedMain),&savedMain)||
               savedMain!=other.scene190->sceneIndex||static_cast<std::size_t>(index)>=savedCount)return false;
            g::BodyMatrix48G raw;g::Vector3_48C localCenter;
            if(!read(&frame.nodes[index].world100,sizeof(raw),&raw)||
               !read(&gridArena_->parameters.bodies.main.centerOffset,sizeof(localCenter),&localCenter))return false;
            g::Vector3_48C otherCenter,otherArm;
            if(!g::prepareTerrainSeedPoint48M(raw,localCenter,otherCenter))return false;
            for(unsigned k=0;k<3;++k)otherArm.v[k]=sub(point.v[k],otherCenter.v[k]);
            if(!valid)return false;
            out.stoppedPC=0x2970d0;
            std::uint8_t flags=0;g::BodyMatrix48G clean,bodyScene;g::Vector3_48C angular;
            if(!read(&frame.nodes[index].sceneFlags190,sizeof(flags),&flags)||!(flags&1u)||
               !read(&frame.nodes[index].world100,sizeof(clean),&clean)||
               !read(&root.scene,sizeof(bodyScene),&bodyScene)||std::memcmp(&clean,&bodyScene,sizeof(clean))||
               !read(&root.body.angularVelocity,sizeof(angular),&angular))return false;
            //Actual clean saved owner getter: pure original vector arithmetic,
            //not a temporary SceneGraph, dirty refresh or nested Bank visit.
            g::VisualVector49R omega;
            if(!g::detail::cleanMatrixDirectionPrefix49U(clean,vector(angular),context,omega))return false;
            const auto rotation=cross(xyz(omega),otherArm);if(!valid)return false;
            g::Vector3_48C linear;
            if(!read(&root.body.velocity,sizeof(linear),&linear))return false;
            for(unsigned k=0;k<3;++k)second.v[k]=add(linear.v[k],rotation.v[k]);
        }
        g::Vector3_48C result{};
        for(unsigned k=0;k<3;++k)result.v[k]=sub(first.v[k],second.v[k]);
        if(!valid||!sharedLeaseCurrent(lease))return false;
        contactPoint=point;velocity=result;out.stoppedPC=0x297178;return true;
    }
    //Real296C10 only, entered from an actual reached private source caller.
    bool applySharedSpringPointForce(SharedTerrainInvocation& call,const g::SpringGeneratedParameters48K& settings,
        std::size_t slot,float sourceTorqueScale,const g::Vector3_48C& applied,const g::Vector3_48C& point)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(!sharedLeaseCurrent(lease)||!call.springEntered_||!actual_||!gridArena_||
           !registeredRange(&settings,sizeof(settings))||std::fegetround()!=FE_TONEAREST||
           (out.stoppedPC!=0x297320&&out.stoppedPC!=0x29736c&&out.stoppedPC!=0x2975a4&&
            out.stoppedPC!=0x2975f0&&out.stoppedPC!=0x298218))return false;
        const auto& session=*lease.producer_->activeSession;auto& frame=actual_->frame;
        auto& root=frame.bodies.main.components.root;
        auto* registry=lease.producer_->bodyRegistry;
        if(!registry||!registry->prepared()||registry->count()!=lease.bank_->count_||
           !registry->bodyIdentityForSlot(slot).value)return false;
        const auto current=[&](std::size_t slot){
            if(slot>=session.racerEntities_.size()||!rosterSlotComplete_[slot])return false;
            const auto& racer=session.racerEntities_[slot];const auto& staged=stagedPods694_[slot];
            return staged.slot==slot&&staged.generation==session.generation_&&racer.prepared()&&
                racer.racer.sessionGeneration==session.generation_&&racer.body340&&
                racer.body340==staged.bodyA0&&racer.body340->entity().reference00&&
                racer.body340->entity().sessionGeneration==session.generation_&&
                racer.body340->entity().parent1C==&racer.racer&&
                racer.body340->canonicalNumericAddress()==&frame.bodies.main&&
                racer.scene190&&racer.scene190->constructed&&!racer.scene190->released&&
                racer.scene190->entity().reference00&&
                racer.scene190->entity().sessionGeneration==session.generation_&&
                racer.body340->scene288()==racer.scene190;
        };
        const auto read=[&](std::size_t slot,void* address,std::size_t size,void* output){
            auto* bytes=sharedSlotSpan(lease,slot,address,size);
            if(!bytes)return false;std::memcpy(output,bytes,size);return true;
        };
        if(!current(slot)||!sharedLeaseCurrent(lease))return false;
        const auto storeAdd=[&](float* address,float value,std::uint32_t pc){
            out.stoppedPC=pc;auto* bytes=sharedSlotSpan(lease,slot,address,sizeof(float));
            if(!bytes||!std::isfinite(value))return false;
            float before;std::memcpy(&before,bytes,sizeof(before));if(!std::isfinite(before))return false;
            volatile float sum=before+value;if(!std::isfinite(sum)||!markSharedDirectQueryEffect(lease))return false;
            const float after=sum;std::memcpy(bytes,&after,sizeof(after));return true;
        };
        constexpr std::uint32_t forcePC[]={0x296c4cu,0x296c5cu,0x296c6cu};
        for(unsigned k=0;k<3;++k)if(!storeAdd(&root.loads.force.v[k],applied.v[k],forcePC[k]))return false;
        //The actual process sentinel is read AFTER every force store.
        out.stoppedPC=0x296c74;const auto app=session.actual_.applicationEntities;
        if(!app||app->nativeOwnerThread_!=std::this_thread::get_id())return false;
        g::Vector3_48C zero;std::memcpy(zero.v,app->springZero4435A0_.data(),sizeof(zero.v));
        if(point.v[0]==zero.v[0]&&point.v[1]==zero.v[1]&&point.v[2]==zero.v[2])return true;
        out.stoppedPC=0x296cf0;
        const auto scene=session.racerEntities_[slot].scene190->sceneIndex;
        const auto index=static_cast<int>(scene);decltype(frame.sceneNodeCount) count=0;
        g::SceneNodeIndex49P savedMain;
        if(index<0||!read(slot,&frame.sceneNodeCount,sizeof(count),&count)||
           static_cast<std::size_t>(index)>=count||static_cast<std::size_t>(index)>=frame.sceneNodeCount||
           !read(slot,&gridArena_->bindings.lifecycle.main190,sizeof(savedMain),&savedMain)||scene!=savedMain)return false;
        g::BodyMatrix48G raw;
        if(!read(slot,&frame.nodes[index].world100,sizeof(raw),&raw))return false;
        //296C10 consumes raw scene100, not28B9B0/refresh/clean admission.
        g::SpringPointForceParameters48J pointParameters{};
        pointParameters.useCustomCenter=settings.impact.customCenter70;
        pointParameters.customLocalCenter=settings.impact.customLocalCenter100;
        pointParameters.yawTorqueOnly=settings.impact.yawOnly6C;
        pointParameters.torqueScale=sourceTorqueScale;
        if(!pointParameters.useCustomCenter&&!read(lease.activeSlot_,
            &gridArena_->parameters.bodies.main.centerOffset,sizeof(pointParameters.primaryLocalCenter),
            &pointParameters.primaryLocalCenter))return false;
        //Even opposite recipient uses primary30->1E0, NOT its own center.
        g::Vector3_48C delta;
        if(!g::prepareSpringTorqueDelta48J(raw,pointParameters,applied,point,delta))return false;
        constexpr std::uint32_t torquePC[]={0x296ee0u,0x296ef0u,0x296f00u};
        for(unsigned k=0;k<3;++k)if(!storeAdd(&root.loads.torque.v[k],delta.v[k],torquePC[k]))return false;
        return true;
    }
    enum class SharedSpringLoadKind { force297190,friction297390 };
    //Actual selected source helper -> first296C10 -> opposite296C10.
    //Shared recipient code, never replay an earlier force helper for friction.
    bool applySharedSpringLoad(SharedTerrainInvocation& call,SharedSpringLoadKind kind,const g::Vector3_48C& normal,
        const g::Vector3_48C& point,float depth,std::uint32_t otherIdentity,
        const g::SpringGeneratedParameters48K& settings,const g::SpringResponseParameters48L* response,
        const g::Vector3_48C& cachedVelocity,float coefficient=0.f)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(!sharedLeaseCurrent(lease)||!call.springEntered_||!actual_||!gridArena_||
           !registeredRange(&settings,sizeof(settings))||
           (kind==SharedSpringLoadKind::force297190&&(!response||!registeredRange(response,sizeof(*response))))||
           std::fegetround()!=FE_TONEAREST)return false;
        const auto& session=*lease.producer_->activeSession;auto& frame=actual_->frame;
        auto* registry=lease.producer_->bodyRegistry;
        if(!registry||!registry->prepared()||registry->count()!=lease.bank_->count_)return false;
        const auto current=[&](std::size_t slot){
            if(slot>=session.racerEntities_.size()||!rosterSlotComplete_[slot])return false;
            const auto& racer=session.racerEntities_[slot];const auto& staged=stagedPods694_[slot];
            return staged.slot==slot&&staged.generation==session.generation_&&racer.prepared()&&
                racer.racer.sessionGeneration==session.generation_&&racer.body340&&
                racer.body340==staged.bodyA0&&racer.body340->entity().reference00&&
                racer.body340->entity().sessionGeneration==session.generation_&&
                racer.body340->entity().parent1C==&racer.racer&&
                racer.body340->canonicalNumericAddress()==&frame.bodies.main&&
                racer.scene190&&racer.scene190->constructed&&!racer.scene190->released&&
                racer.scene190->entity().reference00&&
                racer.scene190->entity().sessionGeneration==session.generation_&&
                racer.body340->scene288()==racer.scene190;
        };
        if(!current(lease.activeSlot_))return false;
        std::optional<std::size_t> otherSlot;
        if(otherIdentity){
            for(std::size_t i=0;i<registry->count();++i)
                if(registry->bodyIdentityForSlot(i).value==otherIdentity){if(otherSlot)return false;otherSlot=i;}
            if(!otherSlot||!current(*otherSlot))return false;
        }
        const auto read=[&](std::size_t slot,void* address,std::size_t size,void* output){
            auto* bytes=sharedSlotSpan(lease,slot,address,size);
            if(!bytes)return false;std::memcpy(output,bytes,size);return true;
        };
        const bool friction=kind==SharedSpringLoadKind::friction297390;
        g::Vector3_48C force;
        if(friction){
            out.stoppedPC=0x297558;float primaryMass;
            if(!read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body.mass,sizeof(primaryMass),&primaryMass)||
               !g::prepareSpringFrictionVector48J(normal,cachedVelocity,coefficient,primaryMass,force))return false;
        }else{
            g::SpringForceParameters48L parameters{};
            parameters.contactVelocity120=cachedVelocity;
            parameters.maxPenetration90=response->maxPenetration90;parameters.maxSpeed94=response->maxSpeed94;
            parameters.stiffness9C=response->stiffness9C;parameters.dampingA0=response->dampingA0;
            parameters.impactStiffnessA4=response->impactStiffnessA4;parameters.impactDampingA8=response->impactDampingA8;
            parameters.impact54=settings.impact54;parameters.otherSpring58=settings.otherSpring58;
            g::BodyParameters48C primaryMass{},otherMass{};
            out.stoppedPC=0x297208;
            if(otherSlot&&(!read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body,sizeof(primaryMass),&primaryMass)||
               !read(*otherSlot,&gridArena_->parameters.bodies.main.body,sizeof(otherMass),&otherMass)))return false;
            //No-other ignores both masses. Transient operands confer no body owner.
            if(!g::prepareSpringWorldForcePrefix48L(normal,depth,parameters,primaryMass,
                otherSlot?&otherMass:nullptr,force))return false;
        }
        const auto recipient=[&](std::size_t slot,const g::Vector3_48C& applied){
            return applySharedSpringPointForce(call,settings,slot,
                friction?settings.frictionTorqueScaleC4:settings.impact.forceTorqueScaleB4,applied,point);
        };
        out.stoppedPC=friction?0x2975a4:0x297320;
        if(!recipient(lease.activeSlot_,force))return false;
        if(otherSlot){
            g::Vector3_48C opposite;for(unsigned k=0;k<3;++k)opposite.v[k]=-force.v[k];
            out.stoppedPC=friction?0x2975f0:0x29736c;if(!recipient(*otherSlot,opposite))return false;
        }
        out.stoppedPC=friction?0x2975f8:0x297374;return true;
    }
    //Actual297610 phases, all within the original held call; no whole354 return.
    bool advanceSharedSpringImpact(SharedTerrainInvocation& call,
        const g::SpringGeneratedParameters48K& settings,const g::StaticSpringState50G& spring,
        const g::Vector3_48C& normal,const g::Vector3_48C& point,float depth,std::uint32_t otherIdentity)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(!sharedLeaseCurrent(lease)||!call.springEntered_||out.stoppedPC!=0x296ae0||
           !actual_||!gridArena_||!registeredRange(&settings,sizeof(settings))||
           !registeredRange(&spring,sizeof(spring))||std::fegetround()!=FE_TONEAREST)return false;
        out.stoppedPC=0x297610;
        const auto& session=*lease.producer_->activeSession;auto& frame=actual_->frame;
        auto& root=frame.bodies.main.components.root;
        const auto current=[&](std::size_t slot){
            if(slot>=session.racerEntities_.size()||!rosterSlotComplete_[slot])return false;
            const auto& racer=session.racerEntities_[slot];const auto& staged=stagedPods694_[slot];
            return staged.slot==slot&&staged.generation==session.generation_&&racer.prepared()&&
                racer.racer.sessionGeneration==session.generation_&&racer.body340&&racer.body340==staged.bodyA0&&
                racer.body340->entity().reference00&&racer.body340->entity().sessionGeneration==session.generation_&&
                racer.body340->entity().parent1C==&racer.racer&&
                racer.body340->canonicalNumericAddress()==&frame.bodies.main&&racer.scene190&&
                racer.scene190->constructed&&!racer.scene190->released&&racer.scene190->entity().reference00&&
                racer.scene190->entity().sessionGeneration==session.generation_&&racer.body340->scene288()==racer.scene190;
        };
        auto* registry=lease.producer_->bodyRegistry;
        if(!registry||!registry->prepared()||registry->count()!=lease.bank_->count_||!current(lease.activeSlot_))return false;
        std::optional<std::size_t> otherSlot;
        if(otherIdentity){
            for(std::size_t i=0;i<registry->count();++i)
                if(registry->bodyIdentityForSlot(i).value==otherIdentity){if(otherSlot)return false;otherSlot=i;}
            if(!otherSlot||!current(*otherSlot))return false;
        }
        const auto read=[&](std::size_t slot,void* address,std::size_t bytes,void* output){
            auto* span=sharedSlotSpan(lease,slot,address,bytes);
            if(!span)return false;std::memcpy(output,span,bytes);return true;
        };
        const auto rawScene=[&](std::size_t slot,g::BodyMatrix48G& matrix){
            if(!current(slot))return false;
            const auto scene=session.racerEntities_[slot].scene190->sceneIndex;
            const auto index=static_cast<int>(scene);decltype(frame.sceneNodeCount) count=0;
            g::SceneNodeIndex49P savedMain;
            return index>=0&&static_cast<std::size_t>(index)<frame.sceneNodeCount&&
                read(slot,&frame.sceneNodeCount,sizeof(count),&count)&&static_cast<std::size_t>(index)<count&&
                read(slot,&gridArena_->bindings.lifecycle.main190,sizeof(savedMain),&savedMain)&&scene==savedMain&&
                read(slot,&frame.nodes[index].world100,sizeof(matrix),&matrix);
        };
        const auto operands=[&](std::size_t slot,g::SpringImpactBody48K& body){
            return read(slot,&root,sizeof(body.state),&body.state)&&
                read(slot,&gridArena_->parameters.bodies.main.body,sizeof(body.parameters),&body.parameters)&&
                read(slot,&gridArena_->parameters.bodies.main.centerOffset,sizeof(body.localCenter),&body.localCenter)&&
                rawScene(slot,body.state.scene); //Transient raw operands, never another mutable root.
        };
        g::SpringImpactBody48K primary,other;
        if(!operands(lease.activeSlot_,primary)||(otherSlot&&!operands(*otherSlot,other)))return false;
        //Original disabled arms/no-other raw zero branch uses the same actual
        //App initializer, not default parameters or an inferred null owner.
        if(settings.impact.disableLeverArms5C||!otherSlot){
            const auto app=session.actual_.applicationEntities;
            if(!app||app->nativeOwnerThread_!=std::this_thread::get_id()||
               app->springZero4435A0_[0]||app->springZero4435A0_[1]||app->springZero4435A0_[2])return false;
        }
        auto parameters=settings.impact;
        parameters.worldCenter110=spring.worldCenter110;parameters.contactVelocity120=spring.contactVelocity120;
        g::SpringImpactImpulsePrefix48K prefix;
        if(!g::prepareSpringImpactImpulsePrefix48K(normal,point,parameters,primary,
            otherSlot?&other:nullptr,prefix))return false;
        const auto storeAdd=[&](std::size_t slot,float* address,float value,std::uint32_t pc){
            out.stoppedPC=pc;auto* bytes=sharedSlotSpan(lease,slot,address,sizeof(float));
            if(!bytes||!std::isfinite(value))return false;
            float before;std::memcpy(&before,bytes,sizeof(before));if(!std::isfinite(before))return false;
            volatile float sum=before+value;
            if(!std::isfinite(sum)||!markSharedDirectQueryEffect(lease))return false;
            const float after=sum;std::memcpy(bytes,&after,sizeof(after));return true;
        };
        const auto recipient=[&](std::size_t slot,bool opposite,const g::Vector3_48C& arm){
            if(!current(slot))return false;
            float inverseMass;
            if(!read(slot,&gridArena_->parameters.bodies.main.body.inverseMass,sizeof(inverseMass),&inverseMass)||
               !std::isfinite(inverseMass))return false;
            g::Vector3_48C delta;
            for(unsigned k=0;k<3;++k){
                const float impulse=opposite?-prefix.impulse.v[k]:prefix.impulse.v[k];
                volatile float value=inverseMass*impulse;if(!std::isfinite(value))return false;delta.v[k]=value;
            }
            constexpr std::uint32_t linearPC[2][6]={{0x297cb8u,0x297cc8u,0x297cd8u,0x297ce8u,0x297cf8u,0x297d08u},
                {0x297e94u,0x297ea4u,0x297eb4u,0x297ec4u,0x297ed4u,0x297ee4u}};
            for(unsigned k=0;k<3;++k)if(!storeAdd(slot,&root.accumulator1c0.v[k],delta.v[k],linearPC[opposite][k]))return false;
            for(unsigned k=0;k<3;++k)if(!storeAdd(slot,&root.body.velocity.v[k],delta.v[k],linearPC[opposite][3+k]))return false;
            //The original inverse getter is AFTER those six canonical stores.
            out.stoppedPC=opposite?0x297eec:0x297d14;
            g::BodyMatrix48G raw,bodyScene;
            if(!rawScene(slot,raw)||!read(slot,&root.scene,sizeof(bodyScene),&bodyScene)||
               std::memcmp(&raw,&bodyScene,sizeof(raw)))return false;
            const auto scene=session.racerEntities_[slot].scene190->sceneIndex;
            const auto index=static_cast<std::size_t>(scene);std::uint8_t flags;
            if(!read(slot,&frame.nodes[index].sceneFlags190,sizeof(flags),&flags)||!(flags&1u))return false;
            g::detail::SceneVectorContext49U context{1.f}; //Finite scratch; final direction W is dead.
            const g::VisualVector49R input{{prefix.impulse.v[0],prefix.impulse.v[1],prefix.impulse.v[2],1.f}};
            g::VisualVector49R local;
            if(slot==lease.activeSlot_){
                if(!g::detail::inverseDirection49U({frame.nodes,frame.sceneNodeCount},scene,input,context,local))return false;
            }else if(!g::detail::cleanMatrixInverseDirectionPrefix49U(raw,input,context,local))return false;
            g::Vector3_48C localImpulse;std::memcpy(localImpulse.v,local.v,sizeof(localImpulse.v));
            g::BodyParameters48C inertia;
            if(!read(slot,&gridArena_->parameters.bodies.main.body,sizeof(inertia),&inertia))return false;
            const float scale=opposite?-settings.impact.angularImpulseScaleB0:settings.impact.angularImpulseScaleB0;
            g::Vector3_48C angular;
            if(!g::prepareSpringImpactAngularPrefix48K(arm,localImpulse,inertia,scale,
                settings.impact.yawOnly6C,angular))return false;
            constexpr std::uint32_t angularPC[2][6]={{0x297dc4u,0x297dd4u,0x297de4u,0x297df4u,0x297e04u,0x297e18u},
                {0x297f98u,0x297fa8u,0x297fb8u,0x297fc8u,0x297fd8u,0x297fe8u}};
            for(unsigned k=0;k<3;++k)if(!storeAdd(slot,&root.accumulator1d0.v[k],angular.v[k],angularPC[opposite][k]))return false;
            for(unsigned k=0;k<3;++k)if(!storeAdd(slot,&root.body.angularVelocity.v[k],angular.v[k],angularPC[opposite][3+k]))return false;
            return true;
        };
        if(prefix.inward){
            if(!recipient(lease.activeSlot_,false,prefix.arm))return false;
            if(otherSlot&&!recipient(*otherSlot,true,prefix.otherArm))return false;
        }
        //297FEC continues in this SAME invocation after all reached impulses.
        bool arithmetic=true;
        const auto keep=[&](float value){arithmetic=arithmetic&&std::isfinite(value);return value;};
        const auto add=[&](float a,float b){keep(a);keep(b);volatile float v=a+b;return keep(v);};
        const auto sub=[&](float a,float b){keep(a);keep(b);volatile float v=a-b;return keep(v);};
        const auto mul=[&](float a,float b){keep(a);keep(b);volatile float v=a*b;return keep(v);};
        const auto cross=[&](const g::Vector3_48C& a,const g::Vector3_48C& b){
            g::Vector3_48C v;for(unsigned k=0;k<3;++k)v.v[k]=sub(
                mul(a.v[(k+1)%3],b.v[(k+2)%3]),mul(b.v[(k+1)%3],a.v[(k+2)%3]));return v;
        };
        const auto getter=[&](std::size_t slot,const g::Vector3_48C& input,bool inverse,
            std::uint32_t pc,g::Vector3_48C& output){
            out.stoppedPC=pc;g::BodyMatrix48G raw,bodyScene;
            if(!rawScene(slot,raw)||!read(slot,&root.scene,sizeof(bodyScene),&bodyScene)||
               std::memcmp(&raw,&bodyScene,sizeof(raw)))return false;
            const auto scene=session.racerEntities_[slot].scene190->sceneIndex;
            const auto index=static_cast<std::size_t>(scene);std::uint8_t flags;
            if(!read(slot,&frame.nodes[index].sceneFlags190,sizeof(flags),&flags)||!(flags&1u))return false;
            const g::VisualVector49R value{{input.v[0],input.v[1],input.v[2],1.f}};
            g::detail::SceneVectorContext49U context{1.f};g::VisualVector49R transformed;
            const bool okay=slot==lease.activeSlot_?
                (inverse?g::detail::inverseDirection49U({frame.nodes,frame.sceneNodeCount},scene,value,context,transformed):
                    g::detail::direction49U({frame.nodes,frame.sceneNodeCount},scene,value,context,transformed)):
                (inverse?g::detail::cleanMatrixInverseDirectionPrefix49U(raw,value,context,transformed):
                    g::detail::cleanMatrixDirectionPrefix49U(raw,value,context,transformed));
            if(!okay)return false;std::memcpy(output.v,transformed.v,sizeof(output.v));return true;
        };
        out.stoppedPC=0x297ff8;g::BodyParameters48C inertia;g::Vector3_48C torque,localRotational,worldRotational;
        if(!read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body,sizeof(inertia),&inertia)||
           !read(lease.activeSlot_,&root.loads.torque,sizeof(torque),&torque)||
           !g::prepareSpringImpactTorqueArm48K(inertia,torque,prefix.arm,localRotational)||
           !getter(lease.activeSlot_,localRotational,false,0x298034,worldRotational))return false;
        float inverseMass;g::Vector3_48C currentForce,acceleration,otherAcceleration;
        if(!read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body.inverseMass,sizeof(inverseMass),&inverseMass)||
           !read(lease.activeSlot_,&root.loads.force,sizeof(currentForce),&currentForce))return false;
        for(unsigned k=0;k<3;++k)acceleration.v[k]=add(mul(inverseMass,currentForce.v[k]),worldRotational.v[k]);
        if(!arithmetic)return false;
        if(otherSlot){
            g::Vector3_48C angularAcceleration,linearAcceleration;
            if(!read(*otherSlot,&root.body.angularAcceleration,sizeof(angularAcceleration),&angularAcceleration))return false;
            const auto localOther=cross(angularAcceleration,prefix.otherArm);
            if(!arithmetic||!getter(*otherSlot,localOther,false,0x2980e8,worldRotational)||
               !read(*otherSlot,&root.body.linearAcceleration,sizeof(linearAcceleration),&linearAcceleration))return false;
            for(unsigned k=0;k<3;++k)otherAcceleration.v[k]=add(linearAcceleration.v[k],worldRotational.v[k]);
        }else{
            out.stoppedPC=0x298144;const auto app=session.actual_.applicationEntities;
            if(!app||app->nativeOwnerThread_!=std::this_thread::get_id())return false;
            std::memcpy(otherAcceleration.v,app->springZero4435A0_.data(),sizeof(otherAcceleration.v));
        }
        g::Vector3_48C relative;
        for(unsigned k=0;k<3;++k)relative.v[k]=sub(acceleration.v[k],otherAcceleration.v[k]);
        const float normalAcceleration=add(add(mul(relative.v[0],normal.v[0]),mul(relative.v[1],normal.v[1])),
            mul(relative.v[2],normal.v[2]));
        if(!arithmetic)return false;
        if(normalAcceleration<0.f){
            const float magnitude=mul(prefix.effectiveMass,mul(-settings.impact.normalForceScale98,normalAcceleration));
            g::Vector3_48C force;for(unsigned k=0;k<3;++k)force.v[k]=mul(magnitude,normal.v[k]);
            if(!arithmetic)return false;
            out.stoppedPC=0x298218;
            if(!applySharedSpringPointForce(call,settings,lease.activeSlot_,settings.impact.forceTorqueScaleB4,force,point))return false;
        }
        out.stoppedPC=0x298220;
        if(settings.impact.correctPenetration64){
            if(!std::isfinite(depth))return false;
            if(depth>0.f){
                g::Vector3_48C correctionImpulse,incoming;
                const float amount=mul(depth,prefix.effectiveMass);
                for(unsigned k=0;k<3;++k)correctionImpulse.v[k]=mul(amount,normal.v[k]);
                if(!read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body.inverseMass,sizeof(inverseMass),&inverseMass))return false;
                for(unsigned k=0;k<3;++k)incoming.v[k]=mul(inverseMass,
                    mul(settings.impact.positionCorrectionScaleCC,correctionImpulse.v[k]));
                if(!arithmetic)return false;
                out.stoppedPC=0x2982e0;g::Vector3_48C currentCorrection,delta;
                if(!read(lease.activeSlot_,&root.positionCorrection1a0,sizeof(currentCorrection),&currentCorrection)||
                   !g::prepareSpringCorrectionDelta48K(currentCorrection,incoming,delta))return false;
                constexpr std::uint32_t positionPC[]={0x165708u,0x165718u,0x165728u};
                for(unsigned k=0;k<3;++k)if(!storeAdd(lease.activeSlot_,&root.positionCorrection1a0.v[k],delta.v[k],positionPC[k]))return false;
                //Position correction is already canonical before the next inverse getter.
                g::Vector3_48C localImpulse,angular;
                if(!getter(lease.activeSlot_,correctionImpulse,true,0x2982f4,localImpulse)||
                   !read(lease.activeSlot_,&gridArena_->parameters.bodies.main.body,sizeof(inertia),&inertia)||
                   !g::prepareSpringImpactAngularPrefix48K(prefix.arm,localImpulse,inertia,
                       settings.impact.angularCorrectionScaleD0,false,angular))return false;
                //Source298340 scales BEFORE yaw mask298370; negative zero is not fabricated by masking first.
                if(settings.impact.yawOnly6C){angular.v[0]=0.f;angular.v[2]=0.f;}
                out.stoppedPC=0x298380;
                if(!read(lease.activeSlot_,&root.rotationCorrection1b0,sizeof(currentCorrection),&currentCorrection)||
                   !g::prepareSpringCorrectionDelta48K(currentCorrection,angular,delta))return false;
                constexpr std::uint32_t rotationPC[]={0x1658c8u,0x1658d8u,0x1658e8u};
                for(unsigned k=0;k<3;++k)if(!storeAdd(lease.activeSlot_,&root.rotationCorrection1b0.v[k],delta.v[k],rotationPC[k]))return false;
            }
        }
        out.stoppedPC=0x298388;return true; //Actual297610 epilogue, not whole354.
    }
    //Actual2983C0 ignores the other-body argument and has no scene getter.
    bool applySharedSpringInwardVelocity(SharedTerrainInvocation& call,
        const g::StaticSpringState50G& spring,const g::Vector3_48C& normal)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(!sharedLeaseCurrent(lease)||!call.springEntered_||!actual_||!gridArena_||
           (out.stoppedPC!=0x296b00&&out.stoppedPC!=0x296b4c)||
           !registeredRange(&spring,sizeof(spring))||std::fegetround()!=FE_TONEAREST)return false;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        auto& body=actual_->frame.bodies.main;
        if(!racer.prepared()||!racer.body340||racer.body340!=stagedPods694_[lease.activeSlot_].bodyA0||
           racer.body340->canonicalNumericAddress()!=&body||!racer.body340->entity().reference00||
           racer.body340->entity().parent1C!=&racer.racer||
           racer.body340->entity().sessionGeneration!=session.generation_||
           !registeredRange(&body,sizeof(body)))return false;
        out.stoppedPC=0x2983c0;g::Vector3_48C delta;
        if(!g::prepareSpringInwardDelta48L(normal,spring.contactVelocity120,delta))return false;
        constexpr std::uint32_t stores[]={0x298448u,0x298458u,0x298470u};
        for(unsigned k=0;k<3;++k){
            out.stoppedPC=stores[k];auto& lane=body.components.root.body.velocity.v[k];
            if(!std::isfinite(lane))return false;
            volatile float result=lane+delta.v[k];
            if(!std::isfinite(result)||!markSharedDirectQueryEffect(lease))return false;
            lane=result;
        }
        return true;
    }
