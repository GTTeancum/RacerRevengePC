#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceWorldTrigger2DBF98(std::uint32_t& pc)noexcept {
    pc=0x2dbf98;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTriggerPhase_!=SourceTriggerPhase::prefixComplete||sourceTriggerPC_!=pc||
       sourceTrackPC_!=pc||sourceTriggerEulerAttempted_||sourceTriggerRow_>=2||
       !preflightSourceWorldTriggerNative()||!sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get()||
       !sourceWorldObjectConfig_->sourceTriggerConfigCurrent()||
       !sourceWorldObjectConfig_->sourceTriggerSelected_||*sourceWorldObjectConfig_->sourceTriggerSelected_!=1+2*sourceTriggerRow_||
       !nativeTriggerList3D0_||nativeTriggerList3D0_->descriptorSlotsPublished!=sourceTriggerRow_+1||
       !nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_])return false;
    auto& desc=*nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_];
    if(!desc.sourceFields)return false;auto& fields=*desc.sourceFields;
    if(!fields.geometryPublished||!fields.geometryConstructed||!fields.sourceGeometry64||
       !fields.geometry64||*fields.sourceGeometry64!=fields.geometry64.get())return false;
    auto& g=*fields.geometry64;
    //Exact admitted +0 operands were actually written by the three missing
    //158B80 getters. Do not adopt native authoring angles or widen finite trig.
    if(g.yaw40!=0||g.pitch44!=0||g.roll48!=0)return false;
    sourceTriggerEulerAttempted_=true;
    const auto stop=[&](std::uint32_t at)noexcept {pc=sourceTriggerPC_=at;sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    //2DBFB0/BC/C4 each multiply +0 by finite positive0x3C8EFA34: exact+0.
    //13BFE0's admitted zero range reduces its polynomial to sin=+0,cos=1.
    //Primary EE signed-zero rules and157100 MUL.A/MADD/MSUB yield exactly
    //the ordered XYZ stores below; no host trigonometry/rounding is used.
    //W is untouched by157100. This is not a general FPU/FCR implementation.
    pc=0x15713c;g.up30[0]=0;
    pc=0x157144;g.up30[1]=0x3f800000;
    pc=0x157148;g.up30[2]=0;
    pc=0x15714c;g.look20[0]=0;
    pc=0x157150;g.look20[1]=0;
    pc=0x157164;g.look20[2]=0x3f800000;
    //Roll zero: first sincos return, then the second call delay stores upY.
    pc=0x157194;g.up30[0]=0;
    pc=0x15719c;g.up30[1]=0x3f800000;
    //Pitch zero: second sincos return; preserve exact store sequence.
    pc=0x1571cc;g.up30[2]=0;
    pc=0x1571d0;g.up30[1]=0x3f800000;
    pc=0x1571f0;g.look20[2]=0x3f800000;
    pc=0x1571fc;g.look20[1]=0;
    //Yaw zero: third sincos return.
    pc=0x157224;g.up30[0]=0;
    pc=0x157228;g.up30[2]=0;
    pc=0x15724c;g.look20[0]=0;
    pc=0x157250;g.look20[2]=0x3f800000;
    sourceTriggerEulerReturned_=true;
    a::OriginalStandardTextureDiagnostic50LA d;bool found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!sourceWorldObjectConfig_->readSourceTriggerField158240(key,value,present,copy,d)){
            pc=d.sourcePC?d.sourcePC:sourceWorldObjectConfig_->sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    pc=0x2dbfd8;if(!read.number("radius",0,g.radius4C,found))return stop(pc);
    sourceTriggerRadiusReturned_=true;
    sourceTriggerPhase_=SourceTriggerPhase::radiusPrefix;pc=sourceTriggerPC_=0x2dbfe0;return true;
}
} //namespace ra_cold_aggregate50LA
