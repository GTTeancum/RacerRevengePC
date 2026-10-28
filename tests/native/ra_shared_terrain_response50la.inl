//Actual1B1A30 native348 response prefix and reached airborne branch. No
//respondEnabledTerrain50FP/applyTerrainFollowing48G transaction or stage=3.
    void advanceSharedTerrainResponse(SharedTerrainInvocation& call)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;auto& row=call.row_;
        if(call.terrainResponseEntered_||!call.springEntered_||out.stoppedPC!=0x219cf0||
           call.cut_!=SharedTerrainInvocation::Cut::returned||!sharedLeaseCurrent(lease)||
           !actual_||!gridArena_)return;
        call.terrainResponseEntered_=true;out.stoppedPC=0x1b1a30;
        const auto& session=*lease.producer_->activeSession;const auto slot=lease.activeSlot_;
        const auto& racer=session.racerEntities_[slot];auto& frame=actual_->frame;
        auto& main=frame.bodies.main;auto& root=main.components.root;
        auto& probe=main.components.probe348;auto& extension=frame.terrainProbe;
        const auto app=session.actual_.applicationEntities;
        if(!app||app->nativeOwnerThread_!=std::this_thread::get_id()||!racer.prepared()||
           !racer.body340||racer.body340!=stagedPods694_[slot].bodyA0||
           racer.body340->canonicalNumericAddress()!=&main||
           racer.body340->entity().parent1C!=&racer.racer||!racer.body340->entity().reference00||
           racer.body340->entity().sessionGeneration!=session.generation_||
           row.slot!=slot||row.role!=storage::Storage::RosterRole::terrain348||
           !row.coldBound||row.generation!=session.generation_||row.nativeTerrainBody128!=racer.body340||
           !row.terrainResource||!row.terrainResource->initialized||
           row.type2Resource!=&row.terrainResource->resource||
           !extension.enabled||main.domain.probe100!=1u||
           !registeredRange(&main,sizeof(main))||!registeredRange(&extension,sizeof(extension))||
           std::fegetround()!=FE_TONEAREST)return;
        //Actual process first-use before the source scene getter. No native
        //parameter projection is relabeled as a global initialized elsewhere.
        if(!markSharedDirectQueryEffect(lease))return;
        auto& angle=app->nativeTerrainResponseLazyWords_[0];out.stoppedPC=0x1b1a58;
        if(!angle)angle=0x3f490fd9u;if(*angle!=0x3f490fd9u)return;
        out.stoppedPC=0x1b1aa4;g::BodyMatrix48G scene;
        if(!readSharedSpringRawScene(lease,scene)||!racer.scene190)return;
        const auto index=static_cast<std::size_t>(racer.scene190->sceneIndex);
        if(!(frame.nodes[index].sceneFlags190&1u)||std::memcmp(&scene,&root.scene,sizeof(scene)))return;
        //28A700 with actual literal null relative copies these axes after its
        //clean getter. No fake relative Entity, dirty refresh or geometry call.
        g::Vector3_48C forward,up;
        std::memcpy(forward.v,scene.v+8,sizeof(forward.v));std::memcpy(up.v,scene.v+4,sizeof(up.v));
        out.stoppedPC=0x1b1aac;probe.field104=0;
        bool valid=true;
        const auto add=[&](float a,float b){
            valid=valid&&std::isfinite(a)&&std::isfinite(b);volatile float v=a+b;
            valid=valid&&std::isfinite(v);return v;
        };
        const auto mul=[&](float a,float b){
            valid=valid&&std::isfinite(a)&&std::isfinite(b);volatile float v=a*b;
            valid=valid&&std::isfinite(v);return v;
        };
        if(probe.field11C){
            if(!probe.field110&&!probe.field114){out.stoppedPC=0x1b1ad8;probe.field11C=0;}
            else{
                out.stoppedPC=0x1b1adc;g::Vector3_48C point;
                for(unsigned k=0;k<3;++k)point.v[k]=add(root.body.position.v[k],extension.response.localOffset1c0.v[k]);
                const float threshold=add(3.f,probe.point150[1]);if(!valid)return;
                if(point.v[1]<=threshold){out.stoppedPC=0x1b1b48;probe.field11C=0;}
            }
        }
        if(!probe.field11C){
            if(probe.field110||probe.field114){
                //Actual current native348 history, never an atomic follow result.
                out.stoppedPC=0x1b1b70;g::Vector3_48C clear;
                std::memcpy(clear.v,app->terrainClear434260_.data(),sizeof(clear.v));
                const auto isClear=[&](const g::Vector3_48C& v){
                    return v.v[0]==clear.v[0]&&v.v[1]==clear.v[1]&&v.v[2]==clear.v[2];
                };
                g::Vector3_48C point;std::memcpy(point.v,probe.point150,sizeof(point.v));
                if(isClear(extension.previousPoint180)){
                    out.stoppedPC=0x1b1be8;if(!markSharedDirectQueryEffect(lease))return;
                    extension.previousPoint180=point;
                }
                out.stoppedPC=0x1b1bec;float distance;
                if(!g::prepareTerrainHistoryDistance48G(extension.previousPoint180,point,
                    extension.normal170,distance))return;
                out.stoppedPC=0x1b1c4c;
                if(isClear(extension.previousNormal190)){
                    out.stoppedPC=0x1b1cc4;if(!markSharedDirectQueryEffect(lease))return;
                    extension.previousNormal190=extension.normal170;
                }
                out.stoppedPC=0x1b1ccc;g::TerrainOrientationPrefix48G orientation;
                if(!g::prepareTerrainOrientationPrefix48G(forward,up,extension.previousNormal190,
                    extension.normal170,orientation))return;
                out.stoppedPC=0x1b1d70;bool detach=false;
                if(!probe.field108){
                    if(!std::isfinite(extension.response.detachDistance20c))return;
                    detach=!(distance<=extension.response.detachDistance20c);
                }
                if(!detach){
                    out.stoppedPC=0x1b1d94;float alignment;
                    if(!g::prepareTerrainOrientationAlignment48G(orientation,alignment))return;
                    if(alignment<0.f){
                        out.stoppedPC=0x1b1dd0;
                        if(!std::isfinite(extension.response.detachAngle208))return;
                        detach=!(orientation.angle<=extension.response.detachAngle208);
                    }
                }
                if(detach){
                    out.stoppedPC=0x1b1de8;if(!markSharedDirectQueryEffect(lease))return;probe.field11C=1;
                    out.stoppedPC=0x1b1df4;extension.previousNormal190=clear;
                    out.stoppedPC=0x1b1e04;extension.previousPoint180=clear;
                }else{
                    out.stoppedPC=0x1b1e0c;if(!markSharedDirectQueryEffect(lease))return;probe.field104=1;
                    out.stoppedPC=0x1b1e14;extension.previousNormal190=extension.normal170;
                    out.stoppedPC=0x1b1e20;extension.previousPoint180=point;
                }
            }else{out.stoppedPC=0x1b1e28;probe.field10C=1;}
        }
        if(probe.field104&&probe.field10C){
            out.stoppedPC=0x1b1e44;g::Vector3_48C point;
            for(unsigned k=0;k<3;++k)point.v[k]=add(root.body.position.v[k],extension.response.localOffset1c0.v[k]);
            const float threshold=add(3.f,probe.point150[1]);if(!valid)return;
            if(!markSharedDirectQueryEffect(lease))return;
            if(point.v[1]<=threshold){out.stoppedPC=0x1b1eb4;probe.field10C=0;}
            else{out.stoppedPC=0x1b1eb8;probe.field104=0;}
        }
        if(probe.field104){
            out.stoppedPC=0x1b1ed0;g::Vector3_48C forceExtra;
            if(!g::prepareTerrainNormalForce48G(root.loads.force,extension.normal170,forceExtra))return;
            constexpr std::uint32_t forceStores[]={0x1b1f2cu,0x1b1f3cu,0x1b1f4cu};
            for(unsigned k=0;k<3;++k){
                out.stoppedPC=forceStores[k];const float value=add(root.loads.force.v[k],forceExtra.v[k]);
                if(!valid||!markSharedDirectQueryEffect(lease))return;root.loads.force.v[k]=value;
            }
            //Only now reach actual28BAB0; dirty getters remain unsupported.
            out.stoppedPC=0x1b1f58;g::BodyMatrix48G pointScene;
            if(!readSharedSpringRawScene(lease,pointScene)||
               !(frame.nodes[index].sceneFlags190&1u)||
               std::memcmp(&pointScene,&root.scene,sizeof(pointScene)))return;
            g::VisualVector49R local{},offset{};
            std::memcpy(local.v,extension.response.localOffset1c0.v,3*sizeof(float));local.v[3]=1.f;
            g::detail::SceneVectorContext49U scratch{};
            if(!g::detail::cleanMatrixPointPrefix49U(pointScene,local,scratch,offset))return;
            out.stoppedPC=0x1b1f7c;if(!markSharedDirectQueryEffect(lease))return;
            auto& translationScale=app->nativeTerrainTranslationScale_;
            if(!translationScale)translationScale=0x3ecccccdu;
            if(*translationScale!=0x3ecccccdu)return;
            float scale;std::memcpy(&scale,&*translationScale,sizeof(scale));
            g::Vector3_48C surface,offsetXYZ,displacement;
            std::memcpy(surface.v,probe.point150,sizeof(surface.v));
            std::memcpy(offsetXYZ.v,offset.v,sizeof(offsetXYZ.v));out.stoppedPC=0x1b1f88;
            if(!g::prepareTerrainTranslation48G(surface,offsetXYZ,extension.normal170,scale,displacement))return;
            out.stoppedPC=0x1b200c;
            //Actual relative=null28AF90, authenticated parentless registered
            //forest. No frame scheduler, body readback or membership update.
            if(!g::validVehicleSceneCount49W(frame,gridArena_->bindings)||
               !registeredRange(frame.nodes,frame.sceneNodeCount*sizeof(frame.nodes[0])))return;
            g::SceneGraph49P graph{frame.nodes,frame.sceneNodeCount};
            auto& target=frame.nodes[index];
            if(!g::validateSceneGraph49P(graph)||target.parent180!=g::SceneNodeIndex49P::none)return;
            constexpr std::uint32_t localStores[]={0x28b10cu,0x28b11cu,0x28b130u};
            for(unsigned k=0;k<3;++k){
                out.stoppedPC=localStores[k];const float value=add(target.localC0.v[12+k],displacement.v[k]);
                if(!valid||!markSharedDirectQueryEffect(lease))return;target.localC0.v[12+k]=value;
            }
            out.stoppedPC=0x28b12c;
            if(!g::markSceneAncestors49P(graph,racer.scene190->sceneIndex))return;
            out.stoppedPC=0x28b134;
            if(target.sceneFlags190&1u){
                out.stoppedPC=0x28b154;target.sceneFlags190=static_cast<std::uint8_t>(target.sceneFlags190&~1u);
                if(target.firstChild184!=g::SceneNodeIndex49P::none){
                    out.stoppedPC=0x28b164;
                    if(!g::invalidateSceneReady49P(graph,target.firstChild184))return;
                }
            }
            //Cached world100/root.scene remain unchanged and dirty. This
            //velocity branch reads actual body70/normal170, not a scene getter.
            out.stoppedPC=0x1b2014;float fraction;
            if(!g::prepareTerrainVelocityFraction48G(root.body.velocity,extension.normal170,fraction))return;
            out.stoppedPC=0x1b2084;if(!angle)return;
            float threshold;std::memcpy(&threshold,&*angle,sizeof(threshold));
            if(!std::isfinite(threshold))return;
            const auto previousVelocity=root.body.velocity; //Actual delay-slot snapshot1B2098.
            if(!(fraction<=threshold)){
                out.stoppedPC=0x1b20a0;g::Vector3_48C amount;
                if(!g::prepareTerrainSteepNormal48G(root.body.velocity,extension.normal170,amount))return;
                constexpr std::uint32_t stores[]={0x1b20e4u,0x1b20f4u,0x1b2108u};
                for(unsigned k=0;k<3;++k){
                    out.stoppedPC=stores[k];
                    if(!std::isfinite(root.body.velocity.v[k])||!std::isfinite(amount.v[k]))return;
                    volatile float value=root.body.velocity.v[k]-amount.v[k];
                    if(!std::isfinite(value)||!markSharedDirectQueryEffect(lease))return;
                    root.body.velocity.v[k]=value; //Source scalar stores preserve actual70.W.
                }
            }else{
                out.stoppedPC=0x1b210c;g::Vector3_48C velocity;
                if(!g::prepareTerrainTangentVelocity48G(root.body.velocity,extension.normal170,velocity))return;
                out.stoppedPC=0x1b21c8;
                //Actual main70.W supplements the same body, not secondary70.W.
                auto& w=frame.bodies.mainSphere.velocity70W;
                if(!registeredRange(&w,sizeof(w))||!markSharedDirectQueryEffect(lease))return;
                root.body.velocity=velocity;w=1.f; //One original fullSQ70 including authoredW.
            }
            out.stoppedPC=0x1b21cc;g::Vector3_48C largest,change;bool replace;
            std::memcpy(largest.v,probe.vector1D0,sizeof(largest.v));
            if(!g::prepareTerrainVelocityChange48G(root.body.velocity,previousVelocity,largest,change,replace))return;
            if(replace){
                out.stoppedPC=0x1b227c;if(!markSharedDirectQueryEffect(lease))return;
                std::memcpy(probe.vector1D0,change.v,sizeof(change.v));probe.vector1D0[3]=1.f;
            }
            //Same actual parentless scene getter. Publish its cache/ready and
            //existing native main mirrors BEFORE inverse arithmetic may fail.
            const auto refreshMain=[&]()->bool {
                g::BodyMatrix48G raw;
                if(!readSharedSpringRawScene(lease,raw)||
                   target.parent180!=g::SceneNodeIndex49P::none||
                   !g::validateSceneGraph49P(graph)||
                   !registeredRange(&frame.bodies.mainSphere.sceneFlags190,sizeof(frame.bodies.mainSphere.sceneFlags190))||
                   !registeredRange(&root.scene,sizeof(root.scene))||!markSharedDirectQueryEffect(lease))return false;
                if(!g::refreshSceneWorld49P(graph,racer.scene190->sceneIndex))return false;
                root.scene=target.world100;
                auto& flags=frame.bodies.mainSphere.sceneFlags190;
                flags=(flags&0xffffff00u)|target.sceneFlags190;
                return true;
            };
            out.stoppedPC=0x1b2280;
            if(!std::isfinite(root.localVelocity80.v[2]))return;
            if(root.localVelocity80.v[2]<0.f){
                out.stoppedPC=0x1b22a4;
                if(!refreshMain())return;
                g::VisualVector49R worldForce{},localForce{};
                std::memcpy(worldForce.v,root.loads.force.v,3*sizeof(float));
                worldForce.v[3]=frame.bodies.mainSphere.force180W;
                //scratch.vf9W was actually set1 by this invocation's28BAB0.
                //Intervening velocity VU leaves do not writeVF9; retain each
                //real direction getter increment instead of fabricatingW1.
                if(!g::detail::cleanMatrixInverseDirectionPrefix49U(target.world100,worldForce,scratch,localForce))return;
                out.stoppedPC=0x1b22b8;
                if(!std::isfinite(localForce.v[2]))return;
                if(localForce.v[2]<0.f){
                    localForce.v[2]=0.f;out.stoppedPC=0x1b22dc;
                    if(!refreshMain())return;
                    g::VisualVector49R restored{};
                    if(!g::detail::cleanMatrixDirectionPrefix49U(target.world100,localForce,scratch,restored))return;
                    out.stoppedPC=0x1b22ec;
                    if(!registeredRange(&frame.bodies.mainSphere.force180W,sizeof(float))||!markSharedDirectQueryEffect(lease))return;
                    std::memcpy(root.loads.force.v,restored.v,3*sizeof(float));
                    frame.bodies.mainSphere.force180W=restored.v[3];
                }
                out.stoppedPC=0x1b22f0;
                if(!std::isfinite(root.localVelocity80.v[2]))return;
                if(root.localVelocity80.v[2]<0.f){
                    out.stoppedPC=0x1b230c;if(!markSharedDirectQueryEffect(lease))return;
                    root.localVelocity80.v[2]=0.f;
                    out.stoppedPC=0x1b231c;if(!refreshMain())return;
                    g::VisualVector49R localVelocity{},worldVelocity{};
                    std::memcpy(localVelocity.v,root.localVelocity80.v,3*sizeof(float));
                    //InputW is not consumed by28B9B0; outputW is the retained
                    //scratch increment, NOT this representation scratch zero.
                    if(!g::detail::cleanMatrixDirectionPrefix49U(target.world100,localVelocity,scratch,worldVelocity))return;
                    out.stoppedPC=0x1b232c;
                    if(!registeredRange(&frame.bodies.mainSphere.velocity70W,sizeof(float))||!markSharedDirectQueryEffect(lease))return;
                    std::memcpy(root.body.velocity.v,worldVelocity.v,3*sizeof(float));
                    frame.bodies.mainSphere.velocity70W=worldVelocity.v[3];
                }
            }
            out.stoppedPC=0x1b2330;
            if(!probe.field104)return; //This active invocation cannot silently take another route.
            g::Vector3_48C orientedForward;
            if(!g::prepareTerrainForward48G(forward,extension.normal170,orientedForward))return;
            //Exact28A090(relative=null,t0=t1=0,t2=1), no normalization.
            out.stoppedPC=0x28a0b8;if(!markSharedDirectQueryEffect(lease))return;
            auto& axesSentinel=app->nativeSceneAxesSentinel4433D8_;
            if(!axesSentinel)axesSentinel=0xffffffffu;if(*axesSentinel!=0xffffffffu)return;
            out.stoppedPC=0x28a0cc;g::Vector3_48C translation;
            std::memcpy(translation.v,target.localC0.v+12,sizeof(translation.v));
            g::ReadyBodyBasis48D basis;
            if(!g::prepareTerrainBasis48G(orientedForward,extension.normal170,basis))return;
            out.stoppedPC=0x28a468;
            if(!g::validateSceneGraph49P(graph)||target.parent180!=g::SceneNodeIndex49P::none||
               !registeredRange(&frame.bodies.mainSphere.sceneFlags190,sizeof(frame.bodies.mainSphere.sceneFlags190)))return;
            g::BodyMatrix48G localMatrix{};
            for(unsigned column=0;column<3;++column)std::memcpy(localMatrix.v+4*column,basis.columns[column].v,3*sizeof(float));
            std::memcpy(localMatrix.v+12,translation.v,sizeof(translation.v));localMatrix.v[15]=1.f;
            out.stoppedPC=0x28a4ac;
            if(!markSharedDirectQueryEffect(lease))return;
            std::memcpy(&target.localC0,&localMatrix,sizeof(localMatrix));
            //Source restores savedXYZ at4C8/4D0/4DC after full matrix copy.
            //They already have those exact saved bits; no arithmetic/readback.
            out.stoppedPC=0x28a4d8;
            if(!g::markSceneAncestors49P(graph,racer.scene190->sceneIndex))return;
            auto& mirrorFlags=frame.bodies.mainSphere.sceneFlags190;
            mirrorFlags=(mirrorFlags&0xffffff00u)|target.sceneFlags190;
            if(target.sceneFlags190&1u){
                out.stoppedPC=0x28a4fc;target.sceneFlags190=static_cast<std::uint8_t>(target.sceneFlags190&~1u);
                mirrorFlags=(mirrorFlags&0xffffff00u)|target.sceneFlags190;
                if(target.firstChild184!=g::SceneNodeIndex49P::none){
                    out.stoppedPC=0x28a50c;if(!g::invalidateSceneReady49P(graph,target.firstChild184))return;
                }
            }
            //No body position or membership mutation. world100 stays the
            //actual last getter cache (or old cache if no getter was reached).
            out.stoppedPC=0x219cfc;advanceSharedSecondaryForces(call);return;
        }
        //1B1EC0 ->1B2330/2334 ->1B239C:104-clear only, no hidden callback.
        out.stoppedPC=0x1b239c;
        auto& scale=app->nativeTerrainResponseLazyWords_[1];
        if(!scale){out.stoppedPC=0x1b23b4;scale=0x3e99999au;}
        if(*scale!=0x3e99999au)return;
        float live;std::memcpy(&live,&*scale,sizeof(live));
        g::Vector3_48C axis;std::memcpy(axis.v,app->terrainAxis434290_.data(),sizeof(axis.v));
        const float factor=mul(live,extension.response.airborneTorque204);
        const float strength=mul(main.cachedAnglesE0.v[2],factor);
        g::Vector3_48C extra;
        extra.v[1]=mul(strength,axis.v[1]);extra.v[0]=mul(strength,axis.v[0]);extra.v[2]=mul(strength,axis.v[2]);
        if(!valid)return;
        constexpr std::uint32_t stores[]={0x1b243cu,0x1b244cu,0x1b245cu};
        for(unsigned k=0;k<3;++k){
            out.stoppedPC=stores[k];const float result=add(root.loads.torque.v[k],extra.v[k]);
            if(!valid||!markSharedDirectQueryEffect(lease))return;root.loads.torque.v[k]=result;
        }
        out.stoppedPC=0x219cfc;advanceSharedSecondaryForces(call); //Actual supported1B1A30 return only.
    }
