#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldMaterialAssignmentsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceMaterialAssignmentPhase_==SourceMaterialAssignmentPhase::unreached)
        return !sourceMaterialAssignmentFrame_&&!sourceMaterialAssignmentConfigArg_;
    if(sourceMaterialAssignmentPhase_==SourceMaterialAssignmentPhase::nativeClosed)return !sourceMaterialAssignmentConfigArg_;
    if(!sourceMaterialAssignmentFrame_||!worldApplication_||worldEntity_.reference00!=1||
       (sourceMaterialAssignmentConfigArg_&&sourceMaterialAssignmentConfigArg_!=sourceWorldAudioConfig_.get()))return false;
    const auto& f=*sourceMaterialAssignmentFrame_;
    if(f.nameKnown&&(f.nameKnown>f.name80.size()||f.name80[f.nameKnown-1]))return false;
    if(sourceMaterialAssignmentPhase_==SourceMaterialAssignmentPhase::returned)
        return f.materialCount284&&*f.materialCount284==1&&f.defaultMaterial288&&*f.defaultMaterial288==1&&
            f.objectCount28C&&*f.objectCount28C==0&&f.s0&&*f.s0==2&&f.s1&&*f.s1==1&&sourceAudioBodyReturned_&&
            sourceAudioBodyV0_&&*sourceAudioBodyV0_==1;
    return true; //Stack history only; no live parser views or descriptor184 borrows.
}
bool Owner::abortSourceWorldMaterialAssignmentsNative(std::uint32_t&)noexcept {
    if(!preflightSourceWorldMaterialAssignmentsNative())return false;
    if(sourceMaterialAssignmentPhase_==SourceMaterialAssignmentPhase::unreached||
       sourceMaterialAssignmentPhase_==SourceMaterialAssignmentPhase::nativeClosed)return true;
    sourceMaterialAssignmentConfigArg_=nullptr;
    sourceMaterialAssignmentPhase_=SourceMaterialAssignmentPhase::nativeClosed;
    return true; //Retain every actual scalar/string write; no source rollback.
}
bool Owner::loadSourceWorldMaterialAssignments271600(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||nativeScbRevoked_||
       sourceMaterialAssignmentPhase_!=SourceMaterialAssignmentPhase::unreached||nativeMaterialAssignments271600_||
       sourceTrackStep_!=SourceTrackStep::audioReturned||sourceTrackPC_!=0x2715dc||
       sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::returned||sourceMaterialSoundPC_!=0x2715dc||
       !preflightSourceWorldParticleCountNative()||!preflightSourceWorldAudioNative()||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!sourceWorldAudioConfig_||!sourceWorldAudioConfig_->source_||
       sourceMaterialSoundConfigArg_!=sourceWorldAudioConfig_.get()||!sourceAudioParser38_||
       *sourceAudioParser38_!=sourceWorldAudioConfig_->source_.get()||!sourceTrackFile_||
       sourceWorldAudioConfig_->file_!=sourceTrackFile_||!sourceWorldAudioConfig_->sourceMaterialSoundConfigCurrent()||
       sourceWorldAudioConfig_->sourceMaterialSoundNextSelection_!=26||!sourceWorldAudioConfig_->sourceMaterialSoundSelected_||
       *sourceWorldAudioConfig_->sourceMaterialSoundSelected_!=25)return false;
    sourceMaterialAssignmentFrame_.emplace();sourceMaterialAssignmentPhase_=SourceMaterialAssignmentPhase::entered;
    auto& f=*sourceMaterialAssignmentFrame_;auto& config=*sourceWorldAudioConfig_;
    const auto at=[&](std::uint32_t p){pc=sourceMaterialAssignmentPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceMaterialAssignmentPhase_=SourceMaterialAssignmentPhase::bounded;return false;};
    at(0x271624);f.materialCount284=0;
    at(0x271628);f.defaultMaterial288=0;
    at(0x27162c);f.objectCount28C=0;
    sourceMaterialAssignmentConfigArg_=&config;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept{
        if(!config.readSourceMaterialAssignmentField158240(key,value,present,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    at(0x27163c);if(!config.selectSourceMaterialAssignmentSection157D00(0,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    at(0x271654);if(!read.integer("nummaterials",0,&*f.materialCount284,found))return stop(pc);
    at(0x27166c);if(!read.integer("defaultmaterial",0,&*f.defaultMaterial288,found))return stop(pc);
    at(0x271678);f.s0=1;
    at(0x2717fc);if(*f.materialCount284!=1||*f.defaultMaterial288!=1)return stop(pc); //Only authenticated outer iteration.
    at(0x271688);constexpr char name[]="CollisionObjectsMaterial1";
    std::memcpy(f.name80.data(),name,sizeof(name));f.nameKnown=sizeof(name); //No stack-tail fill.
    at(0x271694);if(!config.selectSourceMaterialAssignmentSection157D00(1,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    //Actual selector0 is ignored: retained Sounds is searched, not a fresh block.
    at(0x2716ac);if(!read.integer("numcollisionobjects",0,&*f.objectCount28C,found))return stop(pc);
    at(0x2716b8);f.s1=1;
    at(0x2717e4);if(*f.objectCount28C!=0)return stop(pc);
    //1<0 is false: no collision descriptor enumeration/default184 store occurs.
    at(0x2717f0);++*f.s0;
    at(0x2717fc); //NumMaterials1 < s0=2, exits actual loop.
    at(0x271828); //Actual vF0 return, integer ignored by2715EC.
    at(0x2715f4); //Actual vEC return, ignored by272918.
    at(0x272918);sourceAudioBodyV0_=1;
    sourceAudioBodyReturned_=true;sourceMaterialAssignmentPhase_=SourceMaterialAssignmentPhase::returned;
    sourceTrackStep_=SourceTrackStep::audioDescriptorsReturned;
    pc=sourceTrackPC_=sourceMaterialAssignmentPC_=0x1aa11c;return true;
}
bool Owner::closeSourceAudioConfig272930(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||nativeScbRevoked_||sourceAudioCloseAttempted_||
       sourceTrackStep_!=SourceTrackStep::audioDescriptorsReturned||sourceTrackPC_!=0x1aa11c||
       !sourceAudioBodyReturned_||!sourceAudioBodyV0_||*sourceAudioBodyV0_!=1||
       sourceMaterialAssignmentPhase_!=SourceMaterialAssignmentPhase::returned||
       !preflightSourceWorldMaterialAssignmentsNative()||!preflightSourceWorldParticleCountNative()||
       !preflightSourceWorldAudioNative()||!sourceWorldAudioConfig_||!sourceWorldAudioConfig_->source_||
       !sourceWorldAudioConfig_->sourceMaterialAssignmentConfigCurrent()||
       sourceWorldAudioConfig_->sourceMaterialAssignmentNextSelection_!=2||
       !sourceWorldAudioConfig_->sourceMaterialAssignmentSelected_||*sourceWorldAudioConfig_->sourceMaterialAssignmentSelected_!=1||
       !worldParser34_||*worldParser34_||!sourceAudioParser38_||
       *sourceAudioParser38_!=sourceWorldAudioConfig_->source_.get()||!sourceTrackFile_||
       sourceWorldAudioConfig_->file_!=sourceTrackFile_||
       !sourceTrackFile_->current(*sourceWorldAudioConfig_,sourceWorldAudioConfig_->source_->file00))return false;
    sourceAudioCloseAttempted_=true;sourceAudioRetiredAllocation_=*sourceAudioParser38_;
    pc=sourceTrackPC_=0x27294c;a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceWorldAudioConfig_->close1573B0(d)){
        pc=sourceTrackPC_=d.sourcePC?d.sourcePC:sourceWorldAudioConfig_->sourcePC_;return false;
    }
    sourceAudioCloseReturned_=true;
    pc=sourceTrackPC_=0x272954;worldParser34_=nullptr; //Original writes34, NOT38.
    //Keep actual38 as a non-usable historical tombstone. The SAME wrapper is
    //still the Sound-list Config argument; only its closed occurrence withdraws.
    pc=sourceTrackPC_=0x1aa130;
    if(!sourceWorldAudioConfig_->abortNative(d)){if(d.sourcePC)pc=sourceTrackPC_=d.sourcePC;return false;}
    sourceAudioCloseNativeWithdrawn_=true;sourceTrackStep_=SourceTrackStep::audioConfigClosed;
    return true; //Actual vF4 return; before vFC topology, no runtime/service grant.
}
}
