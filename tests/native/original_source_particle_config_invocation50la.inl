#include "../../platform/linux/native/original_ui_garage_screen_text50kg.h"
// Included after cold World and source transport declarations.
namespace ra_cold_aggregate50LA {
bool Owner::abortSourceParticleConfigNative(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(!sourceParticleConfig_)return sourceParticleConfigInvocation_==nullptr;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceParticleConfig_->abortNative(d)){pc=sourceParticleConfig_->sourcePC_;return false;}
    sourceParticleConfig_.reset();sourceParticleConfigInvocation_=nullptr;return true;
}
bool Owner::releaseSourceParticleInvocationNative(const void* object,
    const g::OriginalWorldParticleInvocation50LA& invocation)noexcept {
    auto* world=const_cast<Owner*>(static_cast<const Owner*>(object));
    if(!world||invocation.world_.get()!=world||world->nativeScbThread_!=std::this_thread::get_id()||
       !world->sourceParticleInvocationBorrowers_||
       invocation.worldBorrowers_!=&world->sourceParticleInvocationBorrowers_)return false;
    // Another occurrence's child can never be retired by this token.
    if(world->sourceParticleConfigInvocation_&&world->sourceParticleConfigInvocation_!=&invocation)return false;
    std::uint32_t pc=0;return world->abortSourceParticleConfigNative(pc);
}
bool Owner::beginSourceParticleConfig1617A0(const g::OriginalWorldParticleInvocation50LA& invocation,
    std::uint32_t& pc)noexcept {
    pc=0x160f64;
    if(nativeScbThread_!=std::this_thread::get_id()||invocation.thread_!=std::this_thread::get_id()||
       invocation.configAttempted_||sourceParticleConfig_||sourceParticleConfigInvocation_||!sourceParticleInvocationBorrowers_||
       invocation.world_.get()!=this||invocation.worldBorrowers_!=&sourceParticleInvocationBorrowers_||
       invocation.worldGeneration_!=worldEntity_.sessionGeneration||invocation.application_!=sourceLoadApplication_||
       !trackParticles_||invocation.particle_.get()!=trackParticles_.get()||
       invocation.particleBorrowers_!=&trackParticles_->sourceConfigInvocationBorrowers_||
       !trackParticles_->sourceConfigInvocationBorrowers_||
       invocation.particleGeneration_!=trackParticles_->generation_||
       trackParticles_->application_!=worldApplication_||trackParticles_->closed_||
       !trackParticles_->sourceEntityEnrolled_||trackParticles_->sourceLoadPC_!=0x160f60||
       !trackParticles_->particles_||trackParticles_->particles_->prepared()||
       !sourceEnvironment3A4_||invocation.environment_!=sourceEnvironment3A4_.get()||!sourceWorldConfig_||
       sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne)return false;
    const char* name=nullptr;std::size_t length=0;
    if(!sourceEnvironment3A4_->readSourceParticleName2771F4(worldEntity_,*sourceWorldConfig_,name,length))return false;
    const auto* directory=sourceParticleDirectory36B530(worldApplication_.get());
    if(!directory||!sourceTrackMountDomain_)return false;
    invocation.configAttempted_=true;
    struct RetainPC {std::uint32_t& stored;std::uint32_t& current;~RetainPC(){stored=current;}} retain{invocation.configPC_,pc};
    try {
        // Native heap lifetime for original stack Config; constructor157320
        // occurs before filename formation. It is not a source nullable malloc.
        sourceParticleConfig_.reset(new a::OriginalParticleSourceConfig50LA());
        sourceParticleConfigInvocation_=&invocation;pc=0x1617c4;
        // Explicit bounded adaptation: source strcpy/strcat have no checks.
        // Do not truncate or claim source return if the stack510..61F extent
        // would be exceeded, or if a host directory carries an embedded NUL.
        constexpr std::size_t pathExtent=0x110;
        if(directory->find('\0')!=std::string::npos||directory->size()>=pathExtent)return false;
        std::string filename=*directory;pc=0x1617d8;
        if(length>=pathExtent-filename.size())return false;
        filename.append(name,length);pc=0x1617e4;
        //1169D0 searches backward for the LAST dot, not path.extension().
        const auto dot=filename.rfind('.');
        const auto prefix=dot==std::string::npos?filename.size():dot;
        pc=dot==std::string::npos?0x161814:0x1617fc;
        if(prefix+4>=pathExtent)return false;
        filename.resize(prefix);filename.append(".prt");pc=0x161824;
        a::OriginalStandardTextureDiagnostic50LA d;
        const bool configReturned=sourceParticleConfig_->begin1617A0(
            sourceTrackMountDomain_->resolver3E1A70(),sourceTrackMountDomain_->archiveProvider3E1A70(),filename,d);
        pc=sourceParticleConfig_->sourcePC_;
        if(!configReturned||!sourceParticleConfig_->sourceOpenReturned_)return false;
        const bool sourceTextReturned=*sourceParticleConfig_->sourceOpenReturned_;
        if(!sourceTextReturned){
            pc=0x161ac0;
            if(!sourceParticleConfig_->missingOpenCurrent())return false;
            if(!trackParticles_->constructSourceFallback161AC0(sourceParticleConfig_->filename_,pc))return false;
            if(!trackParticles_->sourceFallbackFramesReturned_||trackParticles_->sourceFrameIndex_!=61||
               trackParticles_->sourceFrameWordsWritten_!=8)return false;
            pc=0x161e78;trackParticles_->field158_=1; //JAL1573B0 delay-slot BEFORE callee.
        }else{
        pc=0x16183c;bool selected=false;
        if(!sourceParticleConfig_->selectGeneralInfo157D00(selected,d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        //161844 ignores the selector return, including missing section0.
        pc=0x16185c;
        if(!sourceParticleConfig_->readTextureMap158AD0(d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        //Both getters receive the SAME retained source particle's integer
        //lanes. The caller ignores sourcev0; a missing value writes default0.
        pc=0x161874;
        if(!sourceParticleConfig_->readCount158B30(true,trackParticles_->sourceNumFrames154_,d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        pc=0x16188c;
        if(!sourceParticleConfig_->readCount158B30(false,trackParticles_->sourceNumGroups140_,d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        pc=0x1618a8;
        if(!sourceParticleConfig_->readGlowScalar158B80(trackParticles_->glowScalar148_,d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        if(!trackParticles_->beginSourceGroups1618C4(pc))return false;
        const auto count=trackParticles_->sourceNumGroups140_;
        for(std::int32_t group=0;group<count;++group){
            const auto ordinal=static_cast<std::uint32_t>(group);
            pc=0x1618f4;
            if(!sourceParticleConfig_->selectGroup157D00(ordinal,selected,d)){
                pc=sourceParticleConfig_->sourcePC_;return false;
            }
            for(std::uint32_t lane=0;lane<3;++lane){
                trackParticles_->sourceGroupIndex_=ordinal;trackParticles_->sourceGroupLane_=lane;
                std::optional<std::int32_t>* destination=nullptr;std::uint8_t* storage=nullptr;
                if(trackParticles_->sourceGroups13C_){
                    if(!trackParticles_->sourceGroupAllocationPublished_||
                       trackParticles_->sourceGroups13C_!=trackParticles_->sourceGroupStorage13C_.get()||
                       ordinal>=trackParticles_->sourceGroupCapacity_||!trackParticles_->sourceGroupRows_)return false;
                    destination=&trackParticles_->sourceGroupRows_[ordinal].lanes[lane];
                    storage=static_cast<std::uint8_t*>(trackParticles_->sourceGroupStorage13C_.get())+
                        std::size_t(ordinal)*12u+lane*4u;
                }
                pc=lane==0?0x16190c:lane==1?0x161928:0x161944;
                if(!sourceParticleConfig_->readGroupInteger158920(ordinal,lane,destination,storage,d)){
                    pc=sourceParticleConfig_->sourcePC_;return false;
                }
            }
            trackParticles_->sourceGroupIndex_=ordinal+1;trackParticles_->sourceGroupLane_=0;
        }
        if(!trackParticles_->beginSourceFrames161974(pc))return false;
        const auto frames=trackParticles_->sourceNumFrames154_;
        for(std::int32_t frame=0;frame<frames;++frame){
            const auto ordinal=static_cast<std::uint32_t>(frame);
            pc=0x1619b4;
            if(!sourceParticleConfig_->selectFrame157D00(ordinal,selected,d)){
                pc=sourceParticleConfig_->sourcePC_;return false;
            }
            for(std::uint32_t lane=0;lane<4;++lane){
                pc=lane==0?0x1619c8:lane==1?0x1619dc:lane==2?0x1619f0:0x161a04;
                if(!sourceParticleConfig_->readFrameUV158A70(ordinal,lane,d)){
                    pc=sourceParticleConfig_->sourcePC_;return false;
                }
            }
            if(!trackParticles_->storeSourceFrame161A0C(ordinal,sourceParticleConfig_->frameUV770_,pc))return false;
        }
        //Prove this SAME completed loop occurrence before source destruction.
        //No default/missing-file receipt substitutes for the actual text path.
        if(sourceParticleConfig_->nextGroupSection_!=static_cast<std::uint32_t>(count>0?count:0)||
           sourceParticleConfig_->nextFrameSection_!=static_cast<std::uint32_t>(frames>0?frames:0)||
           (count>0&&sourceParticleConfig_->groupFieldOrdinal_!=3)||
           (frames>0&&(sourceParticleConfig_->frameFieldOrdinal_!=4||
                      trackParticles_->sourceFrameIndex_!=static_cast<std::uint32_t>(frames)||
                      trackParticles_->sourceFrameWordsWritten_!=8)))return false;
        pc=0x161ab4;trackParticles_->field158_=0; //JAL1573B0 delay-slot BEFORE destructor.
        }
        pc=sourceTextReturned?0x161ab0:0x161e74;
        if(!sourceParticleConfig_->closeStack1573B0(d)){
            pc=sourceParticleConfig_->sourcePC_;return false;
        }
        //Native occurrence withdrawal after actual stackdestructor, without
        //double-closing numeric handle. Keep closed Config evidence until token
        //release; nativeabort later is idempotent and cannot replay source load.
        if(!sourceParticleConfig_->abortNative(d)){pc=sourceParticleConfig_->sourcePC_;return false;}
        //Engaged false is actual fallback return0 at161E7C, NOT a failure.
        //Caller160F6C ignores v0 and continues the common material suffix.
        trackParticles_->sourceConfigReturn1617A0_=sourceTextReturned;
        pc=trackParticles_->sourceLoadPC_=0x160f6c;
        //Use only the already retained SAME application's actual namespace and
        //DefaultMT. Neither availability nor this continuation prepares a domain.
        const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
        if(domain!=sourceTrackMountDomain_||!domain||!domain->prepared()||
           !sourceLoadApplication_->materialApplication||trackParticles_->material150_||trackParticles_->table_)return false;
        const auto table=domain->materials382B38();
        if(!table||!table->sourceNames17CAD4Current()||
           !table->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return false;
        //Native capacity admission for source stackC0..13F save. Do not truncate.
        const auto& previous=sourceLoadApplication_->materialDirectory36B970();
        pc=trackParticles_->sourceLoadPC_=0x160f74;
        if(previous.size()>=128||previous.find('\0')!=std::string::npos)return false;
        //The captured Environment pointer is still pinned by this invocation.
        //Keep its string copy native-only; source160F94 occurs inside the scope.
        const std::string environmentName(name,length);
        bool returned=false;
        try {
            returned=sourceLoadApplication_->withParticleMaterialDirectory160F88(
                [&](const std::string& actualDirectory)->bool {
                    trackParticles_->sourceMaterialDirectorySelected_=true;
                    pc=trackParticles_->sourceLoadPC_=0x160f94;
                    if(!a::OriginalStandardParticleMaterialReference50LA::acceptsTrackInput160F90(
                        actualDirectory,environmentName))return false;
                    //Existing concrete2461E0 constructor models all cold stores.
                    //Publish its nullable result at150 before any virtual access.
                    pc=trackParticles_->sourceLoadPC_=0x160fd0;
                    trackParticles_->sourceMaterialAllocationAttempted_=true;
                    std::unique_ptr<a::OriginalStandardParticleMaterialReference50LA> primitive(
                        new(std::nothrow) a::OriginalStandardParticleMaterialReference50LA());
                    pc=trackParticles_->sourceLoadPC_=0x160fe8;
                    trackParticles_->material150_=std::move(primitive);
                    trackParticles_->sourceMaterialPublished150_=true;
                    pc=trackParticles_->sourceLoadPC_=0x160ffc;
                    if(!trackParticles_->material150_)return false;
                    //Pin real table, not a second table or synthetic template.
                    trackParticles_->table_=table;
                    a::OriginalStandardMaterialDiagnostic50LA md;
                    auto beforeRetain=[&]()noexcept {
                        using Shared=rr::platform::OriginalUiGarageScreenText50KG;
                        pc=trackParticles_->sourceLoadPC_=0x299cb8;
                        if(!Shared::guard8CD8().load(std::memory_order_relaxed)){
                            pc=trackParticles_->sourceLoadPC_=0x299ccc;
                            Shared::assertion443640().store(1,std::memory_order_relaxed);
                            pc=trackParticles_->sourceLoadPC_=0x299cd0;
                            Shared::guard8CD8().store(1,std::memory_order_relaxed);
                        }
                        //Lookup returned an existing nonnegative row:133660
                        //returns zero, regardless of the process assertion mode.
                        pc=trackParticles_->sourceLoadPC_=0x299d38;
                    };
                    pc=trackParticles_->sourceLoadPC_=0x161004;
                    if(!trackParticles_->material150_->acquireTrack299BE0Impl(
                        table,actualDirectory,environmentName,md,beforeRetain,true)){
                        if(md.sourcePC)pc=trackParticles_->sourceLoadPC_=md.sourcePC;
                        return false;
                    }
                    trackParticles_->sourceMaterialReturned2465B0_=true;
                    pc=trackParticles_->sourceLoadPC_=0x161014;
                    return true;
                });
        }catch(...){
            //Existing helper restores the REAL global before propagating. This
            //is native cancellation, never evidence source161014 executed.
            if(trackParticles_->sourceMaterialDirectorySelected_)
                trackParticles_->sourceMaterialDirectoryNativeUnwound_=true;
            return false;
        }
        //The callback is synchronous and cannot retain the directory reference.
        //False leaves all original material/guard/reference effects inspectable,
        //but native unwind restores global custody. configAttempted_ forbids
        //replay. Successful return alone supplies original161014 restoration.
        if(!returned){
            if(trackParticles_->sourceMaterialDirectorySelected_)
                trackParticles_->sourceMaterialDirectoryNativeUnwound_=true;
            return false;
        }
        trackParticles_->sourceMaterialDirectoryRestored161014_=true;
        pc=trackParticles_->sourceLoadPC_=0x16101c;
        //Original scalar writes, in instruction order, to the actual quad
        //positions. The remaining five words of each vertex stay unknown.
        auto at=[&](std::uint32_t where){pc=trackParticles_->sourceLoadPC_=where;};
        at(0x161020);trackParticles_->field144_=1;
        auto& quad=trackParticles_->quadPositions90_;
        at(0x161028);quad[0][0]=-1.f;
        at(0x161030);quad[0][1]=1.f;
        at(0x161034);quad[0][2]=0.f;
        at(0x161038);quad[1][0]=-1.f;
        at(0x16103c);quad[1][1]=-1.f;
        at(0x161040);quad[1][2]=0.f;
        at(0x161044);quad[2][0]=1.f;
        at(0x161048);quad[2][1]=1.f;
        at(0x16104c);quad[2][2]=0.f;
        at(0x161050);quad[3][0]=1.f;
        at(0x161054);quad[3][1]=-1.f;
        at(0x161058);quad[3][2]=0.f;
        trackParticles_->quadPositions90Known_=true;
        //16105C..70 / A0..B0 / E0..F0 /161120..2C supply the
        //four exact stack vectors. These constants are NOT normalized results.
        //Each inline VU body follows106EA0 with identical eight arithmetic
        //words; fourth LQC2 and each SQ use different address registers. Reuse the accepted
        //strict-native policy, explicitly not EE/VU bit-parity or guest flags.
        using Normalize=g::directional_native_proposal::Words;
        const Normalize inputs[4]={
            {{0xbf800000u,0x3f800000u,0x3e800000u,0x3f800000u}},
            {{0xbf800000u,0xbf800000u,0x3e800000u,0x3f800000u}},
            {{0x3f800000u,0x3f800000u,0x3e800000u,0x3f800000u}},
            {{0x3f800000u,0xbf800000u,0x3e800000u,0x3f800000u}}
        };
        constexpr std::uint32_t firstVU[4]={0x16107c,0x1610bc,0x1610fc,0x16113c};
        constexpr std::uint32_t storeSQ[4]={0x16109c,0x1610dc,0x16111c,0x16115c};
        for(unsigned i=0;i<4;++i){
            at(firstVU[i]);Normalize output{};
            if(g::directional_native_proposal::normalize106EA0(inputs[i],output)!=
               g::directional_native_proposal::Status::complete)return false;
            //Full vector commit includes W from VADDx.w, not guessed W=1.
            //Adapter outputs are transactional; keep earlier SQs on refusal.
            at(storeSQ[i]);
            for(unsigned lane=0;lane<4;++lane)
                std::memcpy(&trackParticles_->normals50_[i][lane],&output[lane],sizeof(float));
            ++trackParticles_->sourceNormalsStored_;
        }
        if(trackParticles_->sourceNormalsStored_!=4||
           !trackParticles_->sourceConfigReturn1617A0_||
           !trackParticles_->sourceMaterialReturned2465B0_||
           !trackParticles_->sourceMaterialDirectoryRestored161014_||
           trackParticles_->sourceMaterialDirectoryNativeUnwound_||
           !trackParticles_->material150_||!trackParticles_->material150_->retained()||
           trackParticles_->material150_->table382B38()!=trackParticles_->table_.get())return false;
        //161160 returns the SAME particle S2, not a newly prepared native owner.
        //This private reached-return flag grants no World issuer, pool readiness,
        //texture demand or renderer permission. Native prepared_ stays false.
        trackParticles_->sourceLoadReturned160E00_=true;at(0x161174);
        return true;
    }catch(...){return false;} //Retain the attempted Config for native abort.
}
}
