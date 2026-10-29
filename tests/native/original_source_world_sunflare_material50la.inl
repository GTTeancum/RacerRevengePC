#pragma once
#include "../../platform/linux/native/original_ui_garage_screen_text50kg.h"
#include <algorithm>
#include <cstring>
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceSunFlareMaterial2C5AE0(std::uint32_t& pc)noexcept {
    pc=0;if(!sourceWorldSunFlareMaterialInvocationCurrent())return false;
    return constructSourceSunFlareMaterial245DA0(false,pc);
}
inline bool Owner::constructSourceSunFlareMaterial245DA0(bool embedded,std::uint32_t& pc)noexcept {
    pc=0;if(!nativeSunFlares_||!nativeSunFlares_->sourceMode||!nativeSunFlares_->owner||!sourceSunFlareStack_||
       !sourceLoadApplication_||!sourceTrackMountDomain_)return false;
    auto& owner=*nativeSunFlares_->owner;
    if(embedded){
        if(sourceSunFlarePhase_!=SourceSunFlarePhase::retrieveEntered||!owner.sourceCallbackSelfStored||
           owner.callback74!=&owner||!sourceSunFlareStack_->retrieveKnown)return false;
    }else if(sourceSunFlarePhase_!=SourceSunFlarePhase::materialPrefix||!owner.sprite64||!sourceSunFlareStack_->textureKnown)return false;
    auto& m=embedded?owner.material68:owner.sprite64->material;
    if(m.sourceMode||m.primitive04||m.parameters08)return false;
    //Native string capacity only; no source allocation/publication or directory
    //mutation. Existing same-App namespace and ScreenMT are pinned, not made.
    std::string requested;try{requested.reserve(255);}catch(...){return false;}
    m.sourceRequested=std::move(requested);
    m.sourceTable=sourceTrackMountDomain_->materials382B38();
    m.sourceApplication=sourceLoadApplication_->materialApplication;
    m.sourceMode=true;sourceSunFlarePhase_=embedded?SourceSunFlarePhase::retrieveMaterialEntered:SourceSunFlarePhase::materialEntered;
    auto at=[&](std::uint32_t p){pc=sourceSunFlarePC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    at(0x245db8);m.sourceParametersAttempted=true;
    m.parameters08.reset(new(std::nothrow) NativeSunFlareParameters);
    if(m.parameters08){auto& p=*m.parameters08;p.sourceMode=true;
        at(0x133f84);p.field2C=-1;p.sourceKnownMask|=1;
        at(0x133f88);p.count24=0;p.sourceKnownMask|=2;
        at(0x133f8c);p.count28=0;p.sourceKnownMask|=4;
        at(0x133f90);p.buffer20=nullptr;p.sourceKnownMask|=8;
    }
    //Actual malloc36 call delay publishes nullable parameters BEFORE allocator.
    at(0x245dd8);m.sourceParameters08=m.parameters08.get();m.sourceParametersPublished=true;
    m.sourcePrimitiveAttempted=true;m.primitive04.reset(new(std::nothrow) NativeSunFlarePrimitive);
    if(m.primitive04){auto& p=*m.primitive04;p.sourceMode=true;
        auto store=[&](std::uint32_t where,unsigned bit){at(where);p.sourceKnownMask|=1u<<bit;};
        at(0x299aa8);p.sourceVtable08=0x3d36b0;store(pc,0);
        at(0x299ab0);p.material00=-1;store(pc,1);
        at(0x299ab4);p.optional04.reset();store(pc,2);
        at(0x2461fc);p.sourceVtable08=0x3d20c0;store(pc,3);
        at(0x246200);p.type0C=0;store(pc,4);
        at(0x246208);p.field0D=0;store(pc,5);
        at(0x24620c);p.field0E=255;store(pc,6);
        at(0x246210);p.flags0F=0;store(pc,7);
        at(0x246218);p.scale18=1.f;store(pc,8);
        at(0x24621c);p.scale1C=1.f;store(pc,9);
        at(0x246220);p.parameters10=nullptr;p.sourceParameters10=nullptr;store(pc,10);
        at(0x24623c);p.flags0F&=~1u;store(pc,11);
        at(0x246240);p.optional14=nullptr;store(pc,12);
        at(0x246244);p.field20=0;store(pc,13);
        at(0x246248);p.field22=0;store(pc,14);
    }
    at(0x245dec);m.sourcePrimitive04=m.primitive04.get();m.sourcePrimitivePublished=true;
    if(!m.primitive04)return stop(0x245e00);
    auto& p=*m.primitive04;
    //299BE0 uses the actual current directory, not a replacement textures path.
    const auto& directory=sourceLoadApplication_->materialDirectory36B970();
    const auto& texture=embedded?sourceSunFlareStack_->retrieve180:sourceSunFlareStack_->texture70;
    const auto end=std::find(texture.begin(),texture.end(),'\0');
    if(end==texture.end())return stop(0x299bf8);
    const std::size_t nameLength=static_cast<std::size_t>(end-texture.begin());
    std::size_t used=0;
    auto copy=[&](const char* bytes,std::size_t n,std::uint32_t where){
        at(where);if(n>255-used)return false;
        for(std::size_t i=0;i<n;++i)if(!bytes[i]||static_cast<unsigned char>(bytes[i])>=128)return false;
        std::memcpy(m.sourcePath40.data()+used,bytes,n);used+=n;m.sourcePath40[used]=0;return true;
    };
    at(0x299bf8);
    if(!directory.empty()){
        if(!copy(directory.data(),directory.size(),0x299c14))return stop(pc);
        if(directory.back()!='\\'&&!copy("\\",1,0x299c4c))return stop(pc);
        if(!copy(texture.data(),nameLength,0x299c58))return stop(pc);
    }else if(!copy(texture.data(),nameLength,0x299c68))return stop(pc);
    at(0x299c74);std::size_t dot=used;
    for(std::size_t i=0;i<used;++i)if(m.sourcePath40[i]=='.')dot=i;
    if(dot<used){at(0x299c98);std::memset(m.sourcePath40.data()+dot,0,used-dot);used=dot;}
    m.sourcePathKnown=true;
    //Native adapter failure retains every reached path write.
    try{m.sourceRequested.assign(m.sourcePath40.data(),used);}catch(...){return stop(0x299cac);}
    if(!used)return stop(0x298c80);
    a::OriginalStandardMaterialDiagnostic50LA diagnostic;std::int32_t row=-1;
    at(0x299cac);
    if(!m.sourceTable->find298C80(m.sourceRequested,row,diagnostic))return stop(0x298cf8);
    a::OriginalStandardMaterialRowView50LA view;
    //Read-only native identity capture of the row just found; no guest write.
    if(!m.sourceTable->row(row,view)||!view.name00)return stop(0x299cb4);
    at(0x299cb4);p.material00=row;p.stored=true;p.table=m.sourceTable;p.requested=std::move(m.sourceRequested);
    p.sourceRowName=view.name00;
    using Shared=rr::platform::OriginalUiGarageScreenText50KG;
    at(0x299cb8);
    if(!Shared::guard8CD8().load(std::memory_order_relaxed)){
        at(0x299ccc);Shared::assertion443640().store(1,std::memory_order_relaxed);
        at(0x299cd0);Shared::guard8CD8().store(1,std::memory_order_relaxed);
    }
    //Positive row makes133660(a0!=0) return0 regardless36C100; no missing-name
    //assertion or fresh process-history assumption is manufactured.
    at(0x299d38);if(!p.table->retainScreen14D590(row,diagnostic))return stop(pc);
    p.retained=true;
    at(0x2465cc);p.type0C=15;
    at(0x245e2c);p.flags0F|=1;
    at(0x245e34);p.parameters10=m.parameters08.get();p.sourceParameters10=p.parameters10;
    at(0x245e44);p.flags0F&=~1u;p.configured=true;
    at(embedded?0x2c5c4c:0x2c5ae8);sourceSunFlarePhase_=embedded?SourceSunFlarePhase::retrieveMaterialReturned:SourceSunFlarePhase::materialReturned;return true;
}
}
