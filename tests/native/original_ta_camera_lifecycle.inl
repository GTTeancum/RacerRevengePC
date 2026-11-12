// Included inside Session. Concrete same-Application TA/RA custody and dispatch.
// All post-mutation failures latch through Session::fail; no transition replay.
std::uint32_t taFailurePC_=0;
bool taReject(std::uint32_t pc)noexcept{taFailurePC_=pc;return false;}
bool constructTaOwners174FA4()noexcept {
    if(!coldControllerIdentity174A30()||!nativeTaIssuance_||!nativeTaCutAuthoring_||taCuts_||
       !nativeTaCutAuthoring_->authoring||actual_.configuration34!=2||actual_.launchKind11258!=1||
       !actual_.taIntroAuthoring||actual_.taIntroAuthoring->providerIdentity()!=nativeTaIssuance_->provider.get())return false;
    // Native staging of the same controller: source70 precedes74. If any
    // later cut allocation rejects, keep this real70 for explicit retirement.
    if(!constructNativeRaCinematographer70())return false;
    std::array<rr::assets::OriginalTaCutMemberInputCandidate,8> inputs;
    const auto& source=*nativeTaCutAuthoring_->authoring;
    if(!source.cutCount()||source.cutCount()>7)return false;
    for(unsigned i=0;i<=source.cutCount();++i){
        const auto bytes=i?source.originalVubBytes(i):source.originalTobBytes();
        inputs[i]={nativeTaCutAuthoring_->members[i].name,bytes.data,bytes.size};
    }
    rr::assets::OriginalTaCutAuthoringCandidate::Status status;
    auto copy=rr::assets::OriginalTaCutAuthoringCandidate::create(source.trackId(),inputs.data(),source.cutCount()+1,status);
    if(!copy||source.trackId()!=introTrackId(nativeTaIssuance_->course)||
       actual_.taIntroAuthoring->trackId()!=source.trackId()||!source.cutCount()||source.cutCount()>7)return false;
    OriginalTaCutChildDiagnosticCandidate diagnostic;
    taCuts_=OriginalTaCutChildrenCandidate::construct174FA4(std::move(copy),
        actual_.applicationEntities->cutOrigin443200_,diagnostic);
    if(!taCuts_)return taReject(diagnostic.sourcePC);
    //RA backing remains unpublished until the actual signed A8 load branch.
    //Other courses retain their existing standalone construction API.
    if(nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)
        coldController_.children64[4]=taCuts_.get();
    coldController_.halfA8Value=static_cast<std::uint16_t>((coldController_.halfA8Value&~0x1f0u)|(taCuts_->cutCount()<<4));
    return true;
}
bool bindTaIntroEntity()noexcept {
    if(!nativeIntroSourceCurrent())return false;
    if(!actual_.taIntroAudio||!actual_.taIntroAuthoring||!actual_.taIntroAudio->taIntroReady())return false;
    auto* e=actual_.taIntroAudio->taIntroEntity80();auto& c=coldController_.entity;
    if(!e||e!=c.firstChild18||e->parent1C!=&c||e->previous10||e->next14||e->firstChild18||
       !e->reference00||e->kind!=OriginalWreckComponentKind50LA::wattoEmitter2921C0)return false;
    if(coldController_.script5C!=taLines_.data())return false;
    coldController_.children64[7]=e;
    controllerEntity_.actualController=&c;controllerEntity_.actualIntroAudio=e;
    controllerEntity_.firstEntityChild18=key(*e);controllerEntity_.flags29=c.flags29;
    return true;
}
bool bindTaRegistryBefore7C()noexcept {
    if(!nativeCamera74ConsumersCurrent()||!nativeIntroSourceCurrent())return false;
    const RaceCameraBinding b{generation_,key(scene_),key(identity_[0].racerTag),key(identity_[0].mainBodyTag),
        0,actual_.humanCount34?CameraRacerClass::player:CameraRacerClass::ai,
        actual_.humanCount34?CameraBootstrap::original_single_player:CameraBootstrap::native_all_ai};
    if(!bindFreshOriginalRaceCamera(controller_,b)||!taCuts_||!taCuts_->loaded())return false;
    if(!taCuts_->cutCount()||taCuts_->cutCount()>7)return false;
    childOrder_.fill(0);childOrder_[0]=key(follow_);childOrder_[1]=key(death_);childOrder_[2]=key(attached_);
    for(unsigned i=1;i<=taCuts_->cutCount();++i){const auto* child=taCuts_->child(i);if(!child)return false;childOrder_[2+i]=key(*child);}
    if(!bindFreshOriginalCameraChildRegistry(collection_,generation_,key(controller_),key(scene_),
        childOrder_.data(),3+taCuts_->cutCount()))return false;
    coldController_.scene3C=&scene_;coldController_.racer40=&identity_[0];
    coldController_.children64[0]=&follow_;coldController_.children64[1]=&death_;
    coldController_.children64[2]=&attached_;
    return true;
}
bool selectTaChild115E60(std::uint64_t child)noexcept {
    if(!collection_.bound||!collection_.registered38)return taReject(0x115e60);
    for(std::size_t i=0;i<collection_.count30;++i)if(collection_.registered38[i]==child){
        collection_.selected34=i;return true;
    }
    return taReject(0x115e60);
}
// Real detach/position/attach. Two calls in later cut transitions intentionally
// remain separate, preserving177AB0's inner and1756D0's outer registrations.
bool taReparent(unsigned zone,const OriginalTaCutVector4Candidate& position,
    const OriginalTaCutVector4Candidate* look=nullptr)noexcept {
    lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
    if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
       !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return taReject(0x1148d0);
    auto* world=lease.world();auto* work=lease.workspace();
    if(!world||!work||!zone||zone>lease.zoneCount())return taReject(0x114770);
    result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
    if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return taReject(0x1148d0);
    const SpringVector4_48N point{{position[0],position[1],position[2],position[3]}};
    spatial_.position180=point;pose_.position={position[0],position[1],position[2],position[3]};
    if(look)scene_.look={(*look)[0],(*look)[1],(*look)[2],(*look)[3]};
    result_.spatial=attachOriginalCameraScene114770(*world,spatial_,zone,point,*work);
    if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return taReject(0x114770);
    if(!readOriginalFollowSceneBinding(spatial_,visibilityKeys_.view(),sceneBinding_))return taReject(0x114770);
    return true;
}
//283930 exports only defaults/unspecified tail,115060 overwrites actual Scene
//fields, and283920 consumes nothing. Do not manufacture the unwritten tail.
bool transitionTaMode4_177AB0(unsigned ordinal)noexcept {
    if(controller_.mode44!=4||ordinal<2||ordinal>taCuts_->cutCount()||controller_.flags62&0x60u)return taReject(0x177ab0);
    const auto* previous=taCuts_->child(ordinal-1);const auto* target=taCuts_->child(ordinal);
    if(!previous||!target||!previous->activated())return taReject(0x283930);
    const auto* position=target->position28();if(!position)return taReject(0x17834c);
    //283920 has no stores. Inner115E60 selects the actual target identity.
    if(!selectTaChild115E60(key(*target)))return false;
    std::uint32_t zone=0;
    if(!nativeTaCutAuthoring_->authoring->initialZone(ordinal,zone))return taReject(0x175794);
    if(!taReparent(zone,{{(*position)[0],(*position)[1],(*position)[2],1.f}}))return false;
    controller_.mode44=4;return true;
}
bool taRestoreStartLane()noexcept {
    GridCenterResult50AC grid;
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::gc&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->gcMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ga&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->gaMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::sa&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->saMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ma&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->maMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::sc&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->scMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::rc&&
       actual_.actualRadarMapB8!=nativeTaIssuance_->rcMap)return taReject(0x2a2880);
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::tb){
        //2A2880 on TB's own retained map view (its 270-node Map role).
        if(actual_.actualRadarMapB8||!nativeTaIssuance_->tbMap.sameOwner(actual_.actualRadarMapTb)||
           !source_grid50IG::prepare(actual_.actualRadarMapTb,0,grid))return taReject(0x2a2880);
    }else if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::mc){
        //2A2880 on MC's own retained map view (173-node Map role, the same MC source).
        if(actual_.actualRadarMapB8||!nativeTaIssuance_->mcMap.sameOwner(actual_.actualRadarMapMc)||
           actual_.actualRadarMapMc.sourceIdentity()!=nativeTaIssuance_->mcSource||
           !source_grid50IG::prepare(actual_.actualRadarMapMc,0,grid))return taReject(0x2a2880);
    }else if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::tc){
        //2A2880 on TC's own retained map view (its 268-node Map role).
        if(actual_.actualRadarMapB8||!nativeTaIssuance_->tcMap.sameOwner(actual_.actualRadarMapTc)||
           !source_grid50IG::prepare(actual_.actualRadarMapTc,0,grid))return taReject(0x2a2880);
    }else if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::gb){
        //2A2880 on GB's own retained map view (its 213-node Map role).
        if(actual_.actualRadarMapB8||!nativeTaIssuance_->gbMap.sameOwner(actual_.actualRadarMapGb)||
           !source_grid50IG::prepare(actual_.actualRadarMapGb,0,grid))return taReject(0x2a2880);
    }else if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::mb){
        //2A2880 on MB's own retained map view (same grid helper as the B8 map).
        if(actual_.actualRadarMapB8||!nativeTaIssuance_->mbMap.sameOwner(actual_.actualRadarMapMb)||
           !source_grid50IG::prepare(actual_.actualRadarMapMb,0,grid))return taReject(0x2a2880);
    }else if(!nativeTaIssuance_||(nativeTaIssuance_->course!=OriginalCameraStartCourse::ta&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::ra&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::gc&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::sa&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::ma&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::ga&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::sc&&
       nativeTaIssuance_->course!=OriginalCameraStartCourse::rc)||
       !actual_.actualRadarMapB8||!source_grid50IG::prepare(*actual_.actualRadarMapB8,0,grid))return taReject(0x2a2880);
    OriginalTaCutVector4Candidate point,look;
    for(unsigned i=0;i<4;++i){std::memcpy(&point[i],&grid.position[i],4);std::memcpy(&look[i],&grid.heading[i],4);}
    return taReparent(1,point,&look);
}
bool taTransition7_177AB0()noexcept {
    if(controller_.mode44!=4||!start_.prepared||start_.route[0]==-1||controller_.flags62&0x60u)
        return taReject(0x177ab0);
    lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
    if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
       !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return taReject(0x177c50);
    auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return taReject(0x177c50);
    const auto zones=visibilityKeys_.view();OriginalFollowSceneTransitionInput input;
    if(!actual_.bank.visit(0,[&](){return readOriginalAttachedSceneTransitionInput(attached_.binding,key(scene_),
        {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.collisionWorld,actual_.mainMembership690,zones,input);}))
        return taReject(0x178570);
    unsigned zone=0;for(std::size_t i=0;i<zones.count;++i)if(zones.keys4C0[i]==input.racerZoneKey){zone=i+1;break;}
    if(!zone)return taReject(0x1785ac);
    result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
    if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return taReject(0x177c50);
    // Previous cut export followed by115060; procedural virtual+8 consumes
    // no carried packet fields. Actual2D6DD0 activation follows separately.
    if(!selectTaChild115E60(key(start_)))return false;
    const auto& p=input.mainBodyLocalF0;const SpringVector4_48N point{{p.x,p.y,p.z,1.f}};
    spatial_.position180=point;pose_.position={p.x,p.y,p.z,1.f};
    result_.spatial=attachOriginalCameraScene114770(*world,spatial_,zone,point,*work);
    if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return taReject(0x1785b8);
    const auto& saved=input.racerRegistrationPosition20;
    spatial_.registration34.bounds.center={{saved.x,saved.y,saved.z,saved.w}};
    spatial_.registration34.bounds.cachedCenter=spatial_.registration34.bounds.center;
    if(!readOriginalFollowSceneBinding(spatial_,zones,sceneBinding_))return taReject(0x1785dc);
    controller_.mode44=7;return true;
}
bool taActivate7_2D6DD0()noexcept {
    RaceStartRacerBank bank;GcStartRacerEffectBank unused;
    if(!readRacers(bank,unused)||activateOriginalRaceStartCamera(start_,bank)!=RaceStartStatus::ok)
        return taReject(0x2d6dd0);
    return true;
}
bool taBeginProcedural(bool skip)noexcept {
    if(!taRestoreStartLane())return false;
    if(!skip){--taCutCursor_;coldController_.word60Value=(coldController_.word60Value&~0x1f000u)|(taCutCursor_<<12);}
    if(!start_.prepared||start_.route[0]==-1){
        taExitReason_=skip?2u:3u;controller_.flags62&=0x7fu;frameStage_=FrameStage::exit;return true;
    }
    if(!taTransition7_177AB0())return false;
    if(skip&&!taActivate7_2D6DD0())return false;
    if(!actual_.introMusicFadeRequest(actual_.startMusicContext,.5f))return taReject(0x1a4ab0);
    if(!skip&&!taActivate7_2D6DD0())return false;
    if(skip)return taSkipFade();
    else{controller_.cutRemaining90=2.f;requestGameplayBlur(actual_.blur,100,0,0);}
    for(std::size_t i=0;i<actual_.bank.count();++i)
        actual_.bank.visit(i,[&](){setOriginalCameraRacerIntroActive(actual_.baseTail);});
    controller_.flags62&=0x7fu;return true;
}
bool nextTaCut1756D0()noexcept {
    if(!taCuts_||!taCuts_->loaded()||taCutCursor_>taCuts_->cutCount())return taReject(0x1756d0);
    ++taCutCursor_;
    coldController_.word60Value=(coldController_.word60Value&~0x1f000u)|((taCutCursor_&31u)<<12);
    if(taCutCursor_>taCuts_->cutCount()){
        return taBeginProcedural(false);
    }
    const auto* child=taCuts_->child(taCutCursor_);
    if(!child||!selectTaChild115E60(key(*child)))return false;
    if(taCutCursor_>1&&!transitionTaMode4_177AB0(taCutCursor_))return false;
    const auto* p=child->position28();const auto* l=child->look20();
    if(!p||!l)return taReject(0x1757d4);
    std::uint32_t zone=0;
    if(!nativeTaCutAuthoring_->authoring->initialZone(taCutCursor_,zone))return taReject(0x175794);
    const OriginalTaCutVector4Candidate firstLook{{(*l)[0],(*l)[1],(*l)[2],1.f}};
    if(!taReparent(zone,{{(*p)[0],(*p)[1],(*p)[2],1.f}},&firstLook))return false;
    const auto activated=taCuts_->activate(taCutCursor_);if(!activated.okay())return taReject(activated.sourcePC);
    // Keep original selection/suppression order: call292360 before counter store.
    auto flags=controller_.flags62;AudioCommand50HE command;std::size_t count=0;
    if(!nativeIntroSourceCurrent())return taReject(0x175910);
    if(trackIntroSpeech50HG(actual_.configuration34,taCutCursor_,taLines_.data(),taLineCount_,
        flags,&command,1,count)!=AudioEventStatus50HE::Complete)return taReject(0x175910);
    if(count&&actual_.taIntroAudio->dispatchTaIntro292360(command.cue.key,0,0)==
        rr::platform::RaceAudioGroups50IC::TaIntroDispatchResult::unavailable)return taReject(0x292360);
    controller_.flags62=flags;
    if(taCutCursor_>1){controller_.cutRemaining90=2.f;requestGameplayBlur(actual_.blur,100,0,0);}
    else if(!actual_.startMusicRequest(actual_.startMusicContext,25,0))return taReject(0x1a46e0);
    return true;
}
bool taTimerPrefix()noexcept {
    namespace d=original_race_camera_detail;
    if(!d::scalar(frameDelta_)||frameDelta_<0)return taReject(0x175d70);
    if(controller_.cooldown8C>0){controller_.cooldown8C=d::sub(controller_.cooldown8C,frameDelta_);
        if(!d::scalar(controller_.cooldown8C))return taReject(0x175d98);}
    if(controller_.cutRemaining90>=0){
        std::int32_t strength=0;if(!d::strength(d::mul(100,d::div(controller_.cutRemaining90,2)),strength))return taReject(0x175dc8);
        requestGameplayBlur(actual_.blur,strength,0,0);
        if(controller_.cutRemaining90==0)controller_.cutRemaining90=-1;
        else{const float left=d::sub(controller_.cutRemaining90,frameDelta_);if(!d::scalar(left))return taReject(0x175df4);
            controller_.cutRemaining90=left<=0?0:left;}
    }else if(controller_.shakeRemaining94>=0){
        // 175E44..175F4C: the camera-shake blur timer (set by a course shake
        // action, e.g. during an intro cut). Delay A0 first, then strength 75,
        // or 80*94/98 when decaying (A9 bit1); at zero 94=-1, 98=0 and the
        // mode0/5 shake factors clear (17BB60 / 13E530). The intro camera is
        // mode 4/7 here, which reaches neither call.
        if(!d::scalar(controller_.shakeDelayA0))return taReject(0x175e58);
        if(controller_.shakeDelayA0>0){controller_.shakeDelayA0=d::sub(controller_.shakeDelayA0,frameDelta_);
            if(!d::scalar(controller_.shakeDelayA0))return taReject(0x175e6c);}
        else{
            std::int32_t strength=75;
            if(controller_.shakeDecayingA9){
                if(!d::scalar(controller_.shakeDuration98)||controller_.shakeDuration98==0)return taReject(0x175e9c);
                if(!d::strength(d::mul(80,d::div(controller_.shakeRemaining94,controller_.shakeDuration98)),strength))return taReject(0x175eb8);
            }
            requestGameplayBlur(actual_.blur,strength,0,0);
            if(controller_.shakeRemaining94==0){
                if(controller_.mode44==0||controller_.mode44==5)return taReject(0x175ef8); // ordinary-camera path, not this intro
                controller_.shakeRemaining94=-1;controller_.shakeDuration98=0;
            }else{const float left=d::sub(controller_.shakeRemaining94,frameDelta_);if(!d::scalar(left))return taReject(0x175f38);
                controller_.shakeRemaining94=left<=0?0:left;}
        }
    }
    controller_.flags63Low5&=static_cast<std::uint8_t>(~0x18u);return true;
}
bool advanceTaCut175D70()noexcept {
    if(!original_ta_cut_resource_candidate_detail::arithmeticAdmitted())return taReject(0x2829c0);
    if(!taTimerPrefix())return false;
    if(controller_.flags62&0x80u){
        if(controller_.mode44==7){
            if(!taRestoreStartLane())return false;
            taExitReason_=1;controller_.flags62&=0x7fu;frameStage_=FrameStage::exit;return true;
        }
        if(!taBeginProcedural(true))return false;
    }else{
        const auto* child=taCuts_->child(taCutCursor_);if(!child)return taReject(0x282cb0);
        // Poll BEFORE selected-child update: terminal frame is presented once.
        if(!child->active282CB0()&&!nextTaCut1756D0())return false;
    }
    if(frameStage_==FrameStage::exit||frameStage_==FrameStage::taSkipVideo)return true;
    if(controller_.mode44==7){
        RaceStartRacerBank bank;GcStartRacerEffectBank unused;
        if(!readRacers(bank,unused)||advanceOriginalRaceStartCameraView(start_,bank,frameDelta_,scene_,pose_,matrices_,
            displayInputs_->rootFlags228,sceneFlags29_)!=RaceStartStatus::ok)return taReject(0x2d6f10);
        return true;
    }
    if((displayInputs_->rootFlags228&4u)||(sceneFlags29_&4u))return taReject(0x115cb0);
    OriginalTaCutVector4Candidate position{{pose_.position.x,pose_.position.y,pose_.position.z,pose_.position.w}},
        look{{scene_.look.x,scene_.look.y,scene_.look.z,scene_.look.w}},up{{scene_.up.x,scene_.up.y,scene_.up.z,scene_.up.w}};
    float scalar=scene_.fovDegrees;std::uint32_t mode=0;
    const auto update=taCuts_->update(taCutCursor_,frameDelta_,mode,&position,&look,&up,&scalar);
    if(!update.okay())return taReject(update.sourcePC);
    pose_.mode=mode;pose_.position={position[0],position[1],position[2],position[3]};
    pose_.look={look[0],look[1],look[2],look[3]};pose_.up={up[0],up[1],up[2],up[3]};pose_.fov=scalar;
    CameraViewOutput49F projected;if(!composeCameraView49F(scene_,pose_,projected))return taReject(0x115350);
    scene_=projected.sceneAfter;matrices_=projected;return true;
}
bool retireTaController()noexcept {
    if(!onOwnerThread())return false;
    //Revoke public intro permission before ANY fallible child cleanup. Keep
    //reached source aliases/stores until their identity-checked withdrawal.
    if(actual_.taIntroAudio&&actual_.taIntroAudio->taIntroNativeReserved_)
        actual_.taIntroAudio->taIntroNativeWithdrawn_=true;
    //Actual174C50..74 destroys64 then68 BEFORE all later owned children.
    //Unadopted standalone/native courses preserve their existing lifecycle.
    if(!retireNativeCamera6468()||!retireNativeAttached6C())return false;
    if(taCuts_){
        if(spatial_.registration34.attached)return false;
        if(!withdrawNativeIntroSource())return false;
        if(actual_.taIntroAudio&&!actual_.taIntroAudio->retireTaIntro142590())return false;
        if(coldController_.entity.firstChild18)return false;
        // Retire every borrowed registry/display edge BEFORE real74 arrays.
        display_.controllers48[0]=nullptr;controllerEntity_={};objectChildren_={};
        introChild_={};followChild_={};attachedChild_={};deathChild_={};collection_={};childOrder_.fill(0);
    }
    // Source174C98 destroys70 before the74 list, then174D2C destroys78.
    // Also runs after a74 construction failure, when taCuts_ is still null.
    // Repeated partial cleanup never recreates or double-deletes either child.
    if(!retireNativeCinematographer70())return false;
    if(!withdrawNativeCamera74())return false;
    taCuts_.reset();
    if(!retireNativeTv78())return false;
    if(!retireNativeRaStart7C())return false; //174D34..54, AFTER78 and BEFORE queries.
    coldController_.children64.fill(nullptr);coldController_.script5C=nullptr;
    coldController_.scene3C=nullptr;coldController_.racer40=nullptr;
    return releaseColdController174C20();
}

unsigned taExitReason_=0;
OriginalCameraExitFadeRequest2AED60 taSkipFadeRequest_;
OriginalVideoBlockingFade taSkipBlockingFade_;
bool taSkipFadeFinished_=false;
bool taFinishSkipFade()noexcept {
    auto& app=*displayInputs_;const auto& video=app.video;
    //2AEE90(1500,1,0): synchronous request follows completed260850.
    if(!app.fadeManagerA8&&(video.covered3DC||(video.flags404Low6&16u))){
        app.fadeManagerA8=8;app.fadeDuration98=1500;
    }
    for(std::size_t i=0;i<actual_.bank.count();++i)
        actual_.bank.visit(i,[&](){setOriginalCameraRacerIntroActive(actual_.baseTail);});
    controller_.flags62&=0x7fu;taSkipFadeFinished_=true;return true;
}
bool taSkipFade()noexcept {
    auto& app=*displayInputs_;
    const auto request=taSkipFadeRequest_.invoke(app.fadeManagerA8,app.currentApplicationMode22C(),app.video,actual_.applicationClock);
    if(request.status!=OriginalCameraStartStatus::complete)return taReject(request.sourcePC);
    //176454 uses T0=1: only parent suppression/adjusted-zero skips260850.
    //A260320 bit0 no-op still requires the immediate blocking child. Do not
    //infer this disposition from flags left by another Video invocation.
    if(request.sourcePC!=0x2aee60&&request.sourcePC!=0x2aee00){
        frameStage_=FrameStage::taSkipVideo;return true;
    }
    return taFinishSkipFade();
}
bool dispatchTaEntity141ED0()noexcept {
    if(!taPositive_)return true;
    if(!objectChildren_.boundedChildTree())return taReject(0x141ed0);
    const auto& child=*objectChildren_.actualIntroAudio;
    if(!originalWreckChildEligible50LA(child))return true;
    //3D3370+38=141ED0. Parent performs real renderer service before
    //dispatch; this child's OWN authentic list18 is empty. Sound service is
    //the counted group235720, not a fabricated side effect of this base call.
    const auto result=serviceBeforeChild();
    if(result.status!=OriginalCameraStartStatus::complete)return taReject(result.sourcePC);
    return !child.firstChild18||taReject(0x141ed0);
}
EventCallbackResult48Z routeTaEntityInput(InputEvent48Z& event,DeviceButtonRecord48Y& record,bool press)noexcept {
    if(!taPositive_||!objectChildren_.boundedChildTree())return {false,0};
    const auto& e=*objectChildren_.actualIntroAudio;
    EventChild48Z child{noEventChild48Z,e.flags29};
    const EventChildRouter48Z parent{coldController_.entity.descendants24,0,&child,1};
    const auto callback=[](void* context,std::uint32_t index,bool isPress,InputEvent48Z& ev,DeviceButtonRecord48Y& rec)noexcept->EventCallbackResult48Z {
        const auto* e=static_cast<const OriginalWreckComponent50LA*>(context);
        if(index||!e||e->firstChild18||e->descendants24)return {false,0};
        //Exact3D3370+60/+64=1422A0/142350 base input. Its own actual empty
        //child tree reaches142350/1422A0 and returns0, preserving event/record.
        const EventChildRouter48Z empty{e->descendants24,noEventChild48Z,nullptr,0};
        const auto routed=routeChildInputEvent48Z(empty,isPress,ev,rec,nullptr,nullptr,0);
        return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
    };
    const auto routed=routeChildInputEvent48Z(parent,press,event,record,callback,const_cast<OriginalWreckComponent50LA*>(&e),1);
    return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
}

bool taAudioConstructionAttempted_=false;
bool constructTaIntroAudio177750(std::shared_ptr<const rr::assets::AudioBank50HC> bank,
    std::size_t firstVoice,std::size_t voiceCount)noexcept {
    if(taAudioConstructionAttempted_||!taCuts_||taCuts_->loaded()||generation_||coldBound_||closed_||
       !coldController_.started||coldController_.released||!actual_.taIntroAudio||!actual_.taIntroAuthoring||
       coldController_.script5C||coldController_.entity.firstChild18)return false;
    taAudioConstructionAttempted_=true;
    if(!actual_.taIntroAudio->beginTaIntro177750(actual_.taIntroAuthoring,coldController_.entity,
        std::move(bank),firstVoice,voiceCount))return false;
    if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ra){
        //Prewarm the SAME real attachment once, but withhold80/5C and rows.
        //This is not source177750 completion. Dispatch/service is reserved
        //until the later ordered native source adoption, with no reattachment.
        if(!actual_.taIntroAudio->finishTaIntro177750(coldController_.entity))return false;
        return actual_.taIntroAudio->reserveNativeTaIntro177750(coldController_.entity);
    }
    //177750: events292650 precede real5C initialized source rows. Bytes2..7
    // are unrepresented; do not invent content or assert source heap padding.
    taLineCount_=taCuts_->cutCount();
    if(!taLineCount_||taLineCount_>taLines_.size()||actual_.taIntroAuthoring->trackId()!=taCuts_->trackId())return false;
    //1778B8..1778E8 initialize every source cut row, not just voiced cuts.
    for(std::size_t i=0;i<taLineCount_;++i)taLines_[i]={-1,0};
    for(unsigned i=1;i<=actual_.taIntroAuthoring->eventCount();++i){
        const auto* row=actual_.taIntroAuthoring->event(i);
        if(!row||row->eventId!=i)return false;
        //177988/1779A0: scalar CameraNumber zero or past the cut count is skipped.
        //Repeated mappings overwrite in source event order. RA's comma strings
        //use original158B30 decimal-prefix semantics, not a synthesized list.
        if(!row->cameraNumber||row->cameraNumber>taLineCount_)continue;
        taLines_[row->cameraNumber-1]={static_cast<std::int8_t>(row->eventId),row->span};
    }
    coldController_.script5C=taLines_.data();
    if(!actual_.taIntroAudio->finishTaIntro177750(coldController_.entity))return false;
    return bindTaIntroEntity();
}
