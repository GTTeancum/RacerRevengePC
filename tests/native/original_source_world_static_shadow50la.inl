#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstring>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldStaticModelParamsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceStaticModelParamPhase_==SourceStaticModelParamPhase::unreached)return !sourceStaticModelParamFrame_;
    if(!sourceStaticModelParamFrame_)return false;
    const auto& f=*sourceStaticModelParamFrame_;
    if(sourceStaticModelParamPhase_==SourceStaticModelParamPhase::nativeClosed)
        return !f.parentS2&&!f.wrapperS3&&!f.descriptorA0;
    if(!preflightSourceWorldStaticModelsNative()||!sourceStaticOwner_||!sourceStaticOwner_->sourceMode_||
       !sourceStaticOwner_->sourceAllDescriptorsReturned_||!sourceStaticOwner_->sourceLedgerPrepared_||
       sourceStaticOwner_->parents_.size()!=10)return false;
    const auto& owner=*sourceStaticOwner_;
    if(f.parentZone){if(*f.parentZone>=10||f.parentS2!=owner.parents_[*f.parentZone].get())return false;}
    else if(f.parentS2)return false;
    if(f.wrapperZone||f.wrapperSlot){
        if(!f.wrapperZone||!f.wrapperSlot||*f.wrapperZone>=10||
           *f.wrapperSlot>=owner.sourceWrapperLedger_[*f.wrapperZone].size()||
           f.wrapperS3!=owner.sourceWrapperLedger_[*f.wrapperZone][*f.wrapperSlot].get())return false;
    }else if(f.wrapperS3)return false;
    if(f.descriptorZone||f.descriptorSlot){
        if(!f.descriptorZone||!f.descriptorSlot||*f.descriptorZone>=10||
           *f.descriptorSlot>=owner.sourceWrapperLedger_[*f.descriptorZone].size())return false;
        const auto& w=owner.sourceWrapperLedger_[*f.descriptorZone][*f.descriptorSlot];
        if(!w)return false;
        if(f.descriptorBorrowWithdrawn){
            if(sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::returned||f.descriptorA0||
               !w->sourceDescriptorDisposed||w->descriptor10)return false;
        }else if(f.descriptorA0!=w->descriptor10.get())return false;
    }else if(f.descriptorA0)return false;
    if(f.sltKnown&&(!f.sltBytes||*f.sltBytes>=f.slt1A0.size()||f.slt1A0[*f.sltBytes]))return false;
    return true; //No shadow==constructor1 gate; every reached overwrite persists.
}
bool Owner::abortSourceWorldStaticModelParamsNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldStaticModelParamsNative())return false;
    if(sourceStaticModelParamPhase_==SourceStaticModelParamPhase::unreached||
       sourceStaticModelParamPhase_==SourceStaticModelParamPhase::nativeClosed)return true;
    auto& f=*sourceStaticModelParamFrame_;f.parentS2=nullptr;f.wrapperS3=nullptr;f.descriptorA0=nullptr;
    sourceStaticModelParamPhase_=SourceStaticModelParamPhase::nativeClosed;
    return true; //No source shadow rollback or descriptor ownership transfer.
}
bool Owner::loadSourceWorldStaticModelParams271A00(std::uint32_t& pc)noexcept {
    pc=0;if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::unreached||sourceStaticModelParamFrame_||
       !sourceWorldStaticModelParamsInvocationCurrent()||!sourceWorldObjectConfig_||
       !preflightSourceWorldStaticModelsNative()||!sourceStaticOwner_||
       !sourceStaticOwner_->sourceAllDescriptorsReturned_||!sourceStaticOwner_->sourceLedgerPrepared_||
       sourceStaticOwner_->parents_.size()!=10||!worldZones34C_||*worldZones34C_!=10||
       !sourceModelParents3C4_||sourceModelParentsPublished3C4_!=10)return false;
    sourceStaticModelParamFrame_.emplace();auto& f=*sourceStaticModelParamFrame_;
    sourceStaticModelParamPhase_=SourceStaticModelParamPhase::entered;
    auto at=[&](std::uint32_t p){pc=sourceStaticModelParamPC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceStaticModelParamPhase_=SourceStaticModelParamPhase::bounded;return false;};
    auto& config=*sourceWorldObjectConfig_;auto& owner=*sourceStaticOwner_;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceStaticModelParamField158240(key,value,present,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    at(0x271a34);if(!config.selectSourceStaticModelParamSection157D00(0,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x271a3c); //Ignored0 would need a different retained-field cohort.
    std::int32_t n;at(0x271a4c);if(!read.integer("numberofstaticmodelparams",0,&n,found))return stop(pc);f.count2A8=n;
    at(0x271a58);f.s7=0; //Actual unconditional branch delay producer.
    at(0x271b88);if(*f.count2A8!=673)return stop(pc); //Exact authored loop cohort.
    for(;*f.s7<*f.count2A8;){
        const auto row=static_cast<std::uint32_t>(*f.s7);
        at(0x271a68);std::memcpy(f.name90.data(),"Model",5);char digits[10];std::size_t count=0;auto ordinal=row+1;
        do{digits[count++]=static_cast<char>('0'+ordinal%10);ordinal/=10;}while(ordinal);
        std::size_t length=5;while(count)f.name90[length++]=digits[--count];f.name90[length]=0;f.nameKnown=true;
        at(0x271a74);if(!config.selectSourceStaticModelParamSection157D00(row+1,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
        if(!returned)return stop(0x271a7c); //No metadata lookup or skipped getter substitute.
        at(0x271a94);if(!read.text("slt","",f.slt1A0.data(),f.slt1A0.size(),found))return stop(pc);
        f.sltKnown=true;f.sltBytes=std::strlen(f.slt1A0.data());
        at(0x1169d0);f.lastDot.reset();
        //Actual1169D0 searches backward, including the first byte. This is the
        //LAST dot, unlike the descriptor's earlier first-dot/cap50 constructor.
        for(std::size_t i=*f.sltBytes;i>0;--i)if(f.slt1A0[i-1]=='.'){f.lastDot=i-1;break;}
        if(f.lastDot){at(0x271ac4);std::memset(f.slt1A0.data()+*f.lastDot,0,*f.sltBytes-*f.lastDot);f.sltBytes=*f.lastDot;}
        at(0x271ae0);if(!read.integer("allowshadowreceiving",1,&n,found))return stop(pc);f.allow2AC=n;
        //Reader's exact F branch stores0 at158990; missing alone stores default1.
        at(0x271ae8);f.s0=0;
        at(0x271af0);f.s5=0;
        while(*f.s0<*worldZones34C_){
            const auto zone=static_cast<std::size_t>(*f.s0);
            at(0x271af4);if(!sourceModelParents3C4_||zone>=owner.parents_.size())return stop(pc);
            at(0x271af8);f.s1=0;
            at(0x271b00);auto& parent=owner.parents_[zone];
            if(!parent||sourceModelParents3C4_[zone]!=&parent->entity)return stop(pc);
            f.parentS2=parent.get();f.parentZone=zone;
            at(0x271b08);f.s4=0;
            for(;;){
                at(0x271b58);if(!parent->sourceCountKnown)return stop(pc);
                if(static_cast<std::uint32_t>(*f.s1)>=parent->count30)break;
                const auto slot=static_cast<std::size_t>(*f.s1);
                at(0x271b0c);if(!parent->sourceArrayReturned||!parent->sourceEntries34)return stop(pc);
                at(0x271b14);if(slot>=parent->sourceEntriesKnown||slot>=owner.sourceWrapperLedger_[zone].size())return stop(pc);
                auto* wrapper=parent->sourceEntries34[slot];
                if(wrapper!=owner.sourceWrapperLedger_[zone][slot].get())return stop(pc);
                f.wrapperS3=wrapper;f.wrapperZone=zone;f.wrapperSlot=slot;
                if(!wrapper){at(0x271b1c);*f.s4+=4;at(0x271b54);++*f.s1;continue;}
                at(0x271b20);if(!wrapper->sourceMode||!wrapper->sourceTypeKnown)return stop(pc);
                if(wrapper->type00==1){
                    at(0x271b30);f.descriptorA0=wrapper->descriptor10.get();f.descriptorZone=zone;f.descriptorSlot=slot;
                    at(0x271b34);if(!f.descriptorA0||!f.descriptorA0->sourceFields)return stop(pc);
                    auto& actual=*f.descriptorA0->sourceFields;
                    if(!actual.loaded||!actual.name00Known||actual.name00Known>actual.name00.size()||actual.name00[actual.name00Known-1])return stop(pc);
                    //308F00 uses readonly ctype3C6391 bit1: the pinned ASCII
                    //table marks exactly A..Z. No host locale/toLower or dedup.
                    bool equal=true;
                    for(std::size_t k=0;;++k){
                        if(k>=actual.name00Known||k>*f.sltBytes)return stop(0x308f08);
                        auto x=static_cast<unsigned char>(actual.name00[k]);auto y=static_cast<unsigned char>(f.slt1A0[k]);
                        if(x>=128||y>=128)return stop(0x308f18);
                        const auto lower=[](unsigned char c){return c>='A'&&c<='Z'?static_cast<unsigned>(c+32u):static_cast<unsigned>(c);};
                        if(lower(x)!=lower(y)){equal=false;break;}if(!x)break;
                    }
                    ++f.comparisons;
                    if(equal){at(0x271b4c);actual.shadow148=*f.allow2AC;++f.shadowStores;}
                }
                at(0x271b50);*f.s4+=4;
                at(0x271b54);++*f.s1;
            }
            at(0x271b68);*f.s5+=4;
            at(0x271b6c);++*f.s0;
            at(0x271b70);if(!worldZones34C_||*worldZones34C_!=10)return stop(pc);
        }
        at(0x271b80);++*f.s7;
    }
    //No271A00 explicit return-value store. This is just the actual return to
    //271DB0, before outer271DB0 writes v0=1 and returns to its own caller.
    at(0x271db0);sourceStaticModelParamPhase_=SourceStaticModelParamPhase::returned;return true;
}
}
