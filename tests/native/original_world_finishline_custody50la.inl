// Included by the existing World implementation TU; no new translation unit.
#include "../../src/game/original_finishline_numeric50la.h"
#include "../../src/game/original_parameter_packet_history50la.h"
namespace ra_pod_scene50LA {
bool Owner::preflightSourceFinishLineNative()const noexcept {
    const auto& cleanup=sourceFinishLineMissingCleanup_;
    if(cleanup.entered){
        if(!cleanup.returned||cleanup.releaseMask!=0x1ffu||!cleanup.material544Before||
           sourceFinishLineMaterial544_!=cleanup.material544Before||sourceFinishLineStripOwner_||
           sourceFinishLine540_||sourceFinishLine540Published_||sourceFinishLineMaterialTable_||
           sourceFinishLineDirectory_||!sourceFinishLineHeader_.borrowersWithdrawn)return false;
    }else if(cleanup.returned||cleanup.releaseMask||cleanup.material544Before)return false;
    if(!preflightSourceFinishLineParametersNative()||!preflightSourceFinishLineHeaderNative())return false;
    if(sourceFinishLineIndexLookupEntered_&&!sourceFinishLineHeader_.setupReturned)return false;
    if(sourceFinishLineMaterial544_&&!sourceFinishLineIndexLookupEntered_)return false;
    if(!sourceFinishLineStripOwner_)
        return !sourceFinishLine540_&&!sourceFinishLine540Published_;
    if(!sourceFinishLineConstructorEntered_||sourceFinishLineGroupWordC8_!=4u||
       !sourceFinishLine540Published_||sourceFinishLine540_!=sourceFinishLineStripOwner_.get()||
       !sceneApplication_||sceneApplication_!=worldApplication_||sceneApplication_->attachmentLock423F58())return false;
    const auto& s=*sourceFinishLineStripOwner_;const auto& e=s.entity_;
    //Only reached constructor fields and native ownership invariants. High48
    //bits/native storage defaults are NOT claimed as original authored data.
    if(!(e.kind==g::OriginalWreckComponentKind50LA::strip23BCA0&&e.objectKey==key(&e)&&
        e.sessionGeneration==sceneGeneration_&&e.reference00==1u&&e.category28==0xffu&&(e.flags29&15u)==3u&&
        !e.nativeFlags29&&!e.parent1C&&!e.firstChild18&&!e.previous10&&!e.next14&&!e.mask20&&!e.descendants24&&
        sceneApplication_->entityAuthority_.canRetire(e)&&sceneApplication_->objectCount423F60()))return false;
    if(!preflightSourceFinishLineAttributesNative())return false;
    if(!sourceFinishLineSetupEntered_)
        return !sourceFinishLineSetupMask_&&!sourceFinishLineMaterialReturned_&&!sourceFinishLineBaseReturned_&&
            !s.primitive30_&&!s.groups38_&&!s.count34_&&!s.format3C_&&s.color44_==0x007f7f7fu&&
            (s.flags48_&3u)==1u&&!s.field40_&&!s.color70_;
    if(!sourceFinishLineMaterialTable_||!sourceFinishLineDirectory_||s.format3C_!=7||s.count34_!=1||
       (s.flags48_&3u)!=1u||(sourceFinishLineSetupMask_&~255u))return false;
    const auto mask=sourceFinishLineSetupMask_;
    if(bool(s.groups38_)!=bool(mask&2u)||bool(s.primitive30_)!=bool(mask&64u))return false;
    if((mask&4u)&&s.field40_!=0)return false;
    if(mask&8u){if(s.color44_!=0x00ffffffu||s.color70_!=std::array<float,4>{{127.f,127.f,127.f,64.f}})return false;}
    else if(s.color44_!=0x007f7f7fu||s.color70_)return false;
    if((mask&16u)&&(!s.groups38_||s.groups38_[0]!=4u))return false;
    if(s.primitive30_){const auto& p=*s.primitive30_;
        if(!p.reference.canRelease299AC0()||p.byte0D||p.byte0E!=255u||p.flags0F||p.scale18!=1.f||p.scale1C!=1.f||
           p.attributes10||p.vertices20!=((sourceFinishLineHeader_.primitiveMask&8)?4u:0u)||
           p.groups22!=((sourceFinishLineHeader_.primitiveMask&4)?1u:0u)||p.byte0C!=(sourceFinishLineMaterialReturned_?7u:0u)||
           p.reference.retained()!=sourceFinishLineMaterialReturned_)return false;
    }
    return (!sourceFinishLineMaterialReturned_||(mask==255u&&s.primitive30_))&&
        (!sourceFinishLineBaseReturned_||sourceFinishLineMaterialReturned_);

}
bool Owner::constructSourceFinishLine1B3B80(std::uint32_t& pc)noexcept {
    pc=0x1b3b80;
    if(sourceFinishLineConstructorEntered_||sourceFinishLineStripOwner_||sourceFinishLine540_||
       sourceFinishLine540Published_||sourceFinishLineGroupWordC8_||!sceneApplication_||
       sceneApplication_!=worldApplication_||sceneApplication_->attachmentLock423F58())return false;
    sourceFinishLineConstructorEntered_=true;
    //382C38 is the raw group-length word4, moved through FPU without conversion.
    pc=0x1b3bb0;sourceFinishLineGroupWordC8_=4u; //Allocation JAL delay-slot stack store.
    pc=0x1b3bac;
    try {
        auto strip=std::make_unique<g::OriginalPodCableStripCold50LA>();
        auto& e=strip->entity_;auto& authority=sceneApplication_->entityAuthority_;
        auto& count=sceneApplication_->objectCount423F60();
        //Native admission failure is bounded, NOT original allocator V0=null.
        //Reserve before counted constructor; no fallible work follows count.
        if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
           !authority.canEnroll(e,key(&e)))return false;
        pc=0x1b3bc0;
        if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::strip23BCA0,
            sceneGeneration_,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
           !authority.enroll(e))std::terminate();
        strip->flags48_=static_cast<std::uint8_t>((strip->flags48_&~3u)|1u);
        sourceFinishLineStripOwner_=std::move(strip);
        pc=0x1b3bc8;sourceFinishLine540_=sourceFinishLineStripOwner_.get();sourceFinishLine540Published_=true;
        //Actual vtable3D1EA0+74 is23BEB0. Its resource/material producers are
        //not yet joined: preserve counted/publication effects and stop BEFORE call.
        pc=0x1b3bf4;return true;
    }catch(...){return false;}
}
bool Owner::setupSourceFinishLine150450(
    const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>& table,
    const std::string& directory,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};
    at(0x1b3bf4);
    //The sole derived continuation validates same-Application authority before
    //passing these exact borrowed inputs. Base owns strip resources, not loader.
    if(sourceFinishLineSetupEntered_||!sourceFinishLineStripOwner_||!preflightSourceFinishLineNative()||
       !table||!table->sourceNames17CAD4Current())return false;
    if(directory.find('\0')!=std::string::npos||directory.size()>244u)return false;
    sourceFinishLineMaterialTable_=table;sourceFinishLineDirectory_=&directory;
    auto& s=*sourceFinishLineStripOwner_;
    sourceFinishLineSetupEntered_=true; //One-shot even on native admission failure.
    at(0x23beb0);at(0x150474);s.format3C_=7;
    at(0x150480);s.count34_=1;
    try {
        at(0x150498);sourceFinishLineSetupMask_|=1u;
        //Native allocation failure is bounded here, NOT a fabricated retail
        //NULL return. Reached fields remain owned and explicitly abortable.
        s.groups38_=std::make_unique<std::uint32_t[]>(1);
        at(0x1504a0);sourceFinishLineSetupMask_|=2u;
        at(0x1504a4);s.field40_=0;sourceFinishLineSetupMask_|=4u; //Actual t1=0 at1B3BEC.
        at(0x150560);s.color44_=0x00ffffffu;at(0x15057c);s.flags48_|=1u;
        //Actual23D750: RGB bytes>>1, W fixed64 (not source packed alpha).
        at(0x23d798);s.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}};
        at(0x23d7bc);sourceFinishLineSetupMask_|=8u;
        at(0x1504ec);s.groups38_[0]=*sourceFinishLineGroupWordC8_;sourceFinishLineSetupMask_|=16u;
        at(0x1504f0);sourceFinishLineSetupMask_|=32u;
        s.primitive30_=std::make_unique<g::OriginalPodCableStripCold50LA::Primitive>();
        auto& p=*s.primitive30_;
        //2461E0/299AA0 actual constructor fields, no geometry/material readiness.
        at(0x246200);p.byte0C=0;at(0x246208);p.byte0D=0;at(0x24620c);p.byte0E=255;
        at(0x246210);p.flags0F=0;at(0x246218);p.scale18=1.f;at(0x24621c);p.scale1C=1.f;
        at(0x246220);p.attributes10=nullptr;at(0x246244);p.vertices20=0;at(0x246248);p.groups22=0;
        at(0x150508);sourceFinishLineSetupMask_|=64u;
        rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        auto beforeRetain=[&]()noexcept {using Shared=rr::platform::OriginalUiGarageScreenText50KG;
            at(0x299cb8);if(!Shared::guard8CD8().load(std::memory_order_relaxed)){
                at(0x299ccc);Shared::assertion443640().store(1,std::memory_order_relaxed);
                at(0x299cd0);Shared::guard8CD8().store(1,std::memory_order_relaxed);}
            at(0x299d38);
        };
        //150514 uses actual DefaultMT. Common source leaf preserves a bounded
        //missing lookup without inventing its V0 or a retained numeric handle.
        at(0x150524);at(0x2465b0);at(0x299be0);sourceFinishLineSetupMask_|=128u;
        if(!p.reference.acquireNamed299BE0Impl(table,directory,"finishline",diagnostic,beforeRetain,true)){
            if(diagnostic.sourcePC)at(diagnostic.sourcePC);return false;
        }
        at(0x2465cc);p.byte0C=7;sourceFinishLineMaterialReturned_=true;
        at(0x150550);sourceFinishLineBaseReturned_=true;
        at(0x23bed8);return true; //150450 returned;23BEB0 itself has NOT returned.
    }catch(...){return false;}
}
bool Owner::continueSourceFinishLineAttributes23BED8(std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(sourceFinishLineSetupPC_!=0x23bed8||!sourceFinishLineBaseReturned_||
       !preflightSourceFinishLineNative()||sourceFinishLineAttributes_.stripAttributesEntered)return false;
    auto& strip=*sourceFinishLineStripOwner_;auto& attrs=strip.attributes_;auto& f=sourceFinishLineAttributes_;
    if(strip.count34_!=1||!strip.groups38_||strip.groups38_[0]!=4)return false;
    using Attributes=g::OriginalCableStripAttributes50LA;
    at(0x23bef0);f.stripAttributesEntered=true;f.stripAttributeGroupAttempted=true;
    attrs.group_.reset(new(std::nothrow) Attributes::Group);
    if(attrs.group_){ //2E8990 header/allocation adapter; one real23C050 descriptor.
        constexpr std::uint32_t stores[]{0x23c050,0x23c054,0x23c058,0x23c060,0x23c064,0x23c068,0x23c06c,
            0x23c070,0x23c074,0x23c078,0x23c07c,0x23c080,0x23c084};
        for(unsigned i=0;i<13;++i){at(stores[i]);if(i==4)f.stripAttributeDescriptor10=-1;f.stripAttributeDescriptorMask|=1u<<i;}
    }
    at(0x23bf18);f.stripAttributeGroupPublished=true; //NULL58 does not stop23CB20.
    at(0x23cb34);f.stripCommonAttempted=true;attrs.common_.reset(new(std::nothrow) Attributes::CommonData);
    at(0x23cb3c);f.stripCommonPublished=true;
    at(0x306478);if(!attrs.common_)return stop(pc); //NULL fails in actual memset, not publication.
    auto& common=*attrs.common_;common.layout.reset();common.matrix40={};common.matrix90={};common.matrixE0={};common.vector130={};
    f.stripCommonZeroed=true;
    //Only native numeric semantic fields are represented. Source command headers,
    //self-address and DMA/VIF words have no native consumer or readiness authority.
    for(unsigned i=0;i<16;++i){at(0x23cbc0+4*i);common.matrix40[i/4][i%4]=i/4==i%4?1.f:0.f;f.stripCommon40Mask|=1u<<i;}
    for(unsigned i=0;i<4;++i){at(0x23cc08+4*i);common.matrix90[i]={};f.stripCommon90Mask|=1u<<i;}
    for(unsigned i=0;i<16;++i){at(0x23cc20+4*i);common.matrixE0[i/4][i%4]=i/4==i%4?1.f:0.f;f.stripCommonE0Mask|=1u<<i;}
    constexpr std::uint32_t vectorPC[]{0x23cc64,0x23cc68,0x23cc6c,0x23cc74};
    for(unsigned i=0;i<4;++i){at(vectorPC[i]);common.vector130[i]=1.f;f.stripCommon130Mask|=1u<<i;}
    at(0x23cc98);f.stripCommonReturned=true;
    at(0x23bf28);f.stripAttributeCount5C=strip.count34_;
    at(0x23bf30);f.stripAttributeCountsAttempted=true;attrs.groupCounts_.reset(new(std::nothrow) std::uint32_t[1]);
    at(0x23bf38);f.stripAttributeCountsPublished=true;
    at(0x23bf64);if(!attrs.groupCounts_)return stop(pc);attrs.groupCounts_[0]=strip.groups38_[0];f.stripAttributeCountStored=true;
    //Next actual producer is133F80/134000, with its process-global packet
    //history. Do not skip it, borrow a typed-light packet, or invalidate23CCA0.
    at(0x23bf78);return true; //Prefix only;23BEB0 has NOT returned.
}
bool Owner::preflightSourceFinishLineAttributesNative()const noexcept {
    if(!sourceFinishLineStripOwner_)return !sourceFinishLine540_;
    const auto& a=sourceFinishLineStripOwner_->attributes_;const auto& f=sourceFinishLineAttributes_;
    if(!preflightSourceFinishLineStreamsNative())return false;
    if(!f.stripAttributesEntered)return !a.group_&&!a.common_&&!a.groupCounts_&&
        !f.stripAttributeGroupAttempted&&!f.stripAttributeGroupPublished&&!f.stripAttributeDescriptorMask&&
        !f.stripAttributeDescriptor10&&!f.stripCommonAttempted&&!f.stripCommonPublished&&!f.stripCommonZeroed&&
        !f.stripCommonReturned&&!f.stripCommon40Mask&&!f.stripCommonE0Mask&&!f.stripCommon90Mask&&
        !f.stripCommon130Mask&&!f.stripAttributeCount5C&&!f.stripAttributeCountsAttempted&&
        !f.stripAttributeCountsPublished&&!f.stripAttributeCountStored;
    if(!sourceFinishLineBaseReturned_||!f.stripAttributeGroupAttempted||!f.stripAttributeGroupPublished||
       !f.stripCommonAttempted||!f.stripCommonPublished)return false;
    if(a.group_){
        const auto& streams=sourceFinishLineStreams_;
        if(f.stripAttributeDescriptorMask!=8191u||f.stripAttributeDescriptor10!=
           ((streams.streamDescriptorMask&4)?1:(streams.streamDescriptorMask&2)?0:-1))return false;
    }else if(f.stripAttributeDescriptorMask||f.stripAttributeDescriptor10)return false;
    if(!a.common_)return !f.stripCommonZeroed&&!f.stripCommonReturned&&!f.stripCommon40Mask&&
        !f.stripCommonE0Mask&&!f.stripCommon90Mask&&!f.stripCommon130Mask&&!f.stripAttributeCount5C&&
        !f.stripAttributeCountsAttempted&&!f.stripAttributeCountsPublished&&!f.stripAttributeCountStored&&!a.groupCounts_;
    if(!f.stripCommonZeroed||!f.stripCommonReturned||f.stripCommon40Mask!=65535u||f.stripCommonE0Mask!=65535u||
       f.stripCommon90Mask!=15u||f.stripCommon130Mask!=15u||a.common_->layout||
       f.stripAttributeCount5C!=1u||!f.stripAttributeCountsAttempted||!f.stripAttributeCountsPublished)return false;
    for(unsigned i=0;i<4;++i){if(a.common_->vector130[i]!=1.f)return false;
        for(unsigned j=0;j<4;++j){const float identity=i==j?1.f:0.f;
            if(a.common_->matrix40[i][j]!=identity||a.common_->matrixE0[i][j]!=identity||a.common_->matrix90[i][j]!=0.f)return false;}}
    return a.groupCounts_?(f.stripAttributeCountStored&&a.groupCounts_[0]==4u):!f.stripAttributeCountStored;
}
bool Owner::continueSourceFinishLineParameters23BF78(std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(sourceFinishLineSetupPC_!=0x23bf78||!preflightSourceFinishLineNative()||
       !sourceFinishLineAttributes_.stripAttributeCountStored||sourceFinishLineParameterAttempted_)return false;
    at(0x23bf78);sourceFinishLineParameterAttempted_=true;
    sourceFinishLineParameters54_.reset(new(std::nothrow) SourceFinishLineParameters);
    if(sourceFinishLineParameters54_){auto& p=*sourceFinishLineParameters54_;
        at(0x133f84);p.field2C=-1;p.constructorMask|=1;
        at(0x133f88);p.count24=0;p.constructorMask|=2;
        at(0x133f8c);p.count28=0;p.constructorMask|=4;
        at(0x133f90);p.buffer20=nullptr;p.constructorMask|=8;
    }
    at(0x23bf90);sourceFinishLineParameterPublished_=true;
    using History=g::OriginalParameterPacketHistory50LA;
    if(!History::enter())return stop(0x134014);
    struct Exit{void(*leave)();~Exit(){leave();}} exit{+[](){History::leave();}};
    sourceFinishLineParameterSerial_=History::state().serial;
    if(!History::guard(0,0x134014,0x134030,0x134034,pc))return stop(pc);
    //Actual capacity128 is divisible by4. Null owner first faults at delay-slot2C store.
    at(0x1340ac);if(!sourceFinishLineParameters54_)return stop(pc);
    auto& p=*sourceFinishLineParameters54_;p.packetAttempted=true;p.serial=sourceFinishLineParameterSerial_;
    History::state().scratchSerial=p.serial;History::state().scratchAccessible=true;
    p.field2C=1;at(0x306478);for(auto& word:p.header00To1C)word=0;
    at(0x1340bc);p.count24=128;
    //Default-initialization intentionally leaves the packet WORDS unknown.
    //No payload/header-finalization consumer is enabled by this allocation.
    at(0x1340c4);p.packet.reset(new(std::nothrow) SourceFinishLinePacket);
    at(0x1340cc);p.buffer20=p.packet?p.packet->words.data():nullptr;p.bufferPublished=true;
    if(!History::guard(1,0x1340d0,0x1340e4,0x1340e8,pc))return stop(pc);
    p.bufferAssertionEnabled=History::state().enabled[1]!=0;
    if(*p.bufferAssertionEnabled&&!p.buffer20)return stop(0x13410c);
    at(0x1341d4);p.count28=0;at(0x1341e4);History::state().target30=0;
    at(0x1342a0); //Actual empty leaf; NOT1342B0/header finalization.
    at(0x1341f8);sourceFinishLineParameterReturned_=true;
    //Future stream/payload joins must execute here under THIS serial/lock,
    //not re-enter the parameter initializer or borrow another producer's scratch.
    at(0x23bfb4);return continueSourceFinishLineStreams23CCA0(pc);
}
bool Owner::preflightSourceFinishLineParametersNative()const noexcept {
    const auto* p=sourceFinishLineParameters54_.get();
    if(sourceFinishLineParameterWithdrawn_)return !p&&!sourceFinishLineStripOwner_;
    if(!sourceFinishLineParameterAttempted_)return !p&&!sourceFinishLineParameterPublished_&&
        !sourceFinishLineParameterSerial_&&!sourceFinishLineParameterReturned_;
    if(!sourceFinishLineStripOwner_||!sourceFinishLineAttributes_.stripAttributeCountStored||
       !sourceFinishLineParameterPublished_)return false;
    if(sourceFinishLineParameterSerial_&&
       !g::OriginalParameterPacketHistory50LA::canWithdrawNative(sourceFinishLineParameterSerial_))return false;
    if(!p)return !sourceFinishLineParameterReturned_;
    if(p->constructorMask!=15u||p->count28!=sourceFinishLineStreams_.streamPayloadCount)return false;
    if(!p->packetAttempted){
        for(const auto& word:p->header00To1C)if(word)return false;
        return !sourceFinishLineParameterReturned_&&!p->serial&&p->field2C==-1&&!p->count24&&
            !p->buffer20&&!p->packet&&!p->bufferPublished&&!p->bufferAssertionEnabled;
    }
    if(!p->serial||p->serial!=sourceFinishLineParameterSerial_||p->field2C!=1||p->count24!=128||
       !p->bufferPublished||p->buffer20!=(p->packet?p->packet->words.data():nullptr))return false;
    const auto& header=sourceFinishLineHeader_;
    const std::uint32_t word0=((header.storeMask&1)?((p->count28+header.paddingWords)/4u)&0xffffu:0u)|
        ((header.storeMask&4)?0x30000000u:0u);
    if(p->header00To1C[0]!=word0||p->nativePayload04!=((header.storeMask&8)?p->buffer20:nullptr))return false;
    if(header.storeMask&8){if(p->header00To1C[1])return false;}
    else if(p->header00To1C[1]!=0u)return false;
    for(unsigned i=2;i<8;++i)if(p->header00To1C[i]!=0u)return false;
    //Only words with genuine producing stores are readable. Unknown stack copies
    //stay absent even though the owning packet has native storage.
    for(unsigned i=0;i<128;++i){const auto& word=sourceFinishLineStreams_.streamPayloadWords[i];
        if(word&&(!p->packet||i>=p->count28+sourceFinishLineHeader_.paddingWords||p->packet->words[i]!=*word))return false;}
    return !sourceFinishLineParameterReturned_||
        (p->bufferAssertionEnabled&&(!*p->bufferAssertionEnabled||p->buffer20));
}
bool Owner::continueSourceFinishLineStreams23CCA0(std::uint32_t& pc)noexcept {
    auto& f=sourceFinishLineStreams_;
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    //This continuation is only called synchronously inside the existing134000
    //RAII critical section. External preflight would try to acquire its lock.
    using ParameterHistory=g::OriginalParameterPacketHistory50LA;
    const auto& active=ParameterHistory::state();
    if(sourceFinishLineSetupPC_!=0x23bfb4||!sourceFinishLineStripOwner_||
       !sourceFinishLineParameterReturned_||!sourceFinishLineParameters54_||f.streamEntered||
       !active.busy||active.thread!=std::this_thread::get_id()||!active.scratchAccessible||
       active.serial!=sourceFinishLineParameterSerial_||active.scratchSerial!=sourceFinishLineParameterSerial_||
       sourceFinishLineParameters54_->serial!=sourceFinishLineParameterSerial_)return false;
    auto& strip=*sourceFinishLineStripOwner_;auto& attrs=strip.attributes_;
    if(!attrs.groupCounts_||!strip.groups38_||sourceFinishLineAttributes_.stripAttributeCount5C!=1||attrs.groupCounts_[0]!=4)return false;
    using Attributes=g::OriginalCableStripAttributes50LA;using History=g::OriginalStripStreamHistory23CCA0;
    at(0x23cca0);f.streamEntered=true;
    //Original stackD0 is zeroed then bitfields written; no guest pointer is present.
    at(0x23ccd0);f.streamStackTagD0={};
    at(0x23ccf4);f.streamStackTagD0[0]=0x8000;
    at(0x23cd24);f.streamStackTagD0[1]=0x4000;
    at(0x23cd54);f.streamStackTagD0[1]|=0x00360000; //0x6C shifted left47 in the low64 lane.
    at(0x23cd70);f.streamStackTagD0[1]&=~0x0c000000u;
    at(0x23cd8c);f.streamStackTagD0[1]=(f.streamStackTagD0[1]&~0xf0000000u)|0x30000000u;
    at(0x23cd9c);f.streamStackTagD0[2]=2;
    at(0x23cdac);f.streamStackTagD0[2]|=0x10;
    at(0x23cdc0);f.streamStackTagD0[2]|=0x400;f.streamStackTagKnown=true;
    //Actual count loop reaches its descriptor before any stream pointer use.
    at(0x23ce08);strip.groups38_[0]=attrs.groupCounts_[0];f.streamCountStored=true;
    at(0x23ce0c);if(!attrs.group_)return stop(pc);auto& group=*attrs.group_;
    group.vertexCount=4;f.streamDescriptorMask|=1;
    at(0x23ce24);sourceFinishLineAttributes_.stripAttributeDescriptor10=0;f.streamDescriptorMask|=2;
    at(0x23ce30);sourceFinishLineAttributes_.stripAttributeDescriptor10=1;group.batchCount=1;f.streamDescriptorMask|=4;
    at(0x23ce34);if(!History::begin(0,f.streamBatchAssertion))return stop(pc);
    at(0x23ce70);if(!History::finish(f.streamBatchAssertion,group.batchCount>0))return stop(pc);
    //Seven independent nullable arrays. Native views are not source pointers;
    //their elements remain unknown until corresponding source stores occur.
    at(0x23cea4);f.streamArraysAttempted|=1;group.counts.reset(new(std::nothrow) std::uint32_t[1]);
    at(0x23ceac);f.streamArraysPublished|=1;
    at(0x23ceb4);f.streamArraysAttempted|=2;group.batches.reset(new(std::nothrow) Attributes::Batch[1]);
    at(0x23cebc);f.streamArraysPublished|=2;
    at(0x23cec4);f.streamArraysAttempted|=4;group.positions.reset(new(std::nothrow) Attributes::View<Attributes::Position>[1]);
    at(0x23cecc);f.streamArraysPublished|=4;
    at(0x23ced4);f.streamArraysAttempted|=8;group.triples.reset(new(std::nothrow) Attributes::View<Attributes::Triple>[1]);
    at(0x23cedc);f.streamArraysPublished|=8;
    at(0x23cee4);f.streamArraysAttempted|=16;group.extra.reset(new(std::nothrow) std::optional<Attributes::View<Attributes::Triple>>[1]);
    at(0x23ceec);f.streamArraysPublished|=16;
    at(0x23cef4);f.streamArraysAttempted|=32;group.colors.reset(new(std::nothrow) std::optional<Attributes::View<Attributes::Color>>[1]);
    at(0x23cefc);f.streamArraysPublished|=32;
    at(0x23cf04);f.streamArraysAttempted|=64;group.texcoords.reset(new(std::nothrow) Attributes::View<Attributes::Texcoord>[1]);
    at(0x23cf0c);f.streamArraysPublished|=64;
    at(0x23cf20);f.streamAliasAllocationAttempted=true;group.aliases.reset(new(std::nothrow) Attributes::VertexCopies[4]);
    if(group.aliases){for(unsigned i=0;i<4;++i){at(0x23d420);group.aliases[i].count=0;
        at(0x23d424);group.aliases[i].aliases[1].batch=0;at(0x23d428);group.aliases[i].aliases[0].batch=0;
        at(0x23d42c);group.aliases[i].aliases[1].ordinal=0;at(0x23d430);group.aliases[i].aliases[0].ordinal=0;++f.streamAliasConstructed;}}
    at(0x23cf44);f.streamAliasPublished=true;
    at(0x23cf48); //Descriptor08=0 remains represented native no emitted-word span.
    at(0x23cf58);f.streamDescriptorParameterBegin=sourceFinishLineParameters54_->count28;
    at(0x23cfb4);if(!group.counts)return stop(pc);group.counts[0]=4;f.streamBatchCountStored=true;
    for(unsigned i=0;i<4;++i){at(0x23cfc8);auto& receipt=f.streamAliasAssertions[i];
        if(!History::begin(1,receipt))return stop(pc);
        //Enabled assertion loads actual alias.count before evaluating<2.
        if(receipt.enabled&&!group.aliases)return stop(0x23cff8);
        if(!History::finish(receipt,!group.aliases||group.aliases[i].count<2))return stop(0x23d008);
        at(0x23d038);if(!group.aliases)return stop(pc);auto& alias=group.aliases[i];
        if(alias.count>=alias.aliases.size())return stop(pc); //Native bound after source assertion, no invented write.
        at(0x23d048);alias.aliases[alias.count].batch=0;f.streamAliasStoreMask[i]|=1;
        at(0x23d058);alias.aliases[alias.count].ordinal=i;f.streamAliasStoreMask[i]|=2;
        at(0x23d068);++alias.count;f.streamAliasStoreMask[i]|=4;
    }
    at(0x23d088);return continueSourceFinishLinePayload23D088(pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceFinishLinePayload23D088(std::uint32_t& pc)noexcept {
    auto& f=sourceFinishLineStreams_;
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(sourceFinishLineSetupPC_!=0x23d088||f.streamPayloadEntered||!f.streamStackTagKnown||
       !sourceFinishLineStripOwner_||!sourceFinishLineParameters54_)return false;
    using History=g::OriginalParameterPacketHistory50LA;using Attributes=g::OriginalCableStripAttributes50LA;
    auto& history=History::state();auto& p=*sourceFinishLineParameters54_;auto& strip=*sourceFinishLineStripOwner_;
    auto& attrs=strip.attributes_;
    if(!history.busy||history.thread!=std::this_thread::get_id()||!history.scratchAccessible||
       history.serial!=sourceFinishLineParameterSerial_||history.scratchSerial!=sourceFinishLineParameterSerial_||
       p.serial!=sourceFinishLineParameterSerial_||!attrs.group_||strip.format3C_!=7)return false;
    f.streamPayloadEntered=true;
    //Native semantic backing only, admitted before further source effects. These
    //host arrays never become source addresses or command data.
    attrs.vertices_.reset(new(std::nothrow) Attributes::NativeVertices);
    if(!attrs.vertices_)return stop(pc);f.streamPayloadNativeBacking=true;
    auto& group=*attrs.group_;
    auto guard=[&](unsigned i,std::uint32_t read,std::uint32_t word,std::uint32_t byte){
        const bool okay=History::guard(i,read,word,byte,pc);sourceFinishLineSetupPC_=pc;return okay;};
    auto capacity=[&](unsigned count,bool scalar){
        if(!guard(6,scalar?0x1349a0:0x134860,scalar?0x1349b8:0x134878,scalar?0x1349bc:0x13487c))return false;
        if(history.enabled[6]&&p.count28>p.count24)return stop(scalar?0x1349e8:0x1348a8);
        //This exact first-use cohort fits128. Reallocation is not fabricated.
        if(p.count28>p.count24||count>p.count24-p.count28)return stop(scalar?0x134a30:0x1348f0);
        return true;
    };
    auto write=[&](unsigned index,std::optional<std::uint32_t> word){
        if(!p.packet||!p.buffer20||index>=128)return false;
        if(word)p.packet->words[index]=*word;
        f.streamPayloadWords[index]=word;return true;
    };
    auto begin=[&](unsigned format,bool first){
        at(0x1345e0);history.format40=format;at(0x1345e8);history.words38=0;
        at(0x134600);history.placeholder28=p.count28;
        at(0x134614);if(first)history.target30=0;
        if(!capacity(1,false))return false;
        at(0x13462c);++p.count28;f.streamPayloadCount=p.count28;return true;
    };
    auto copy=[&](const std::array<std::optional<std::uint32_t>,4>& words,unsigned count,unsigned kind){
        if(!capacity(count,false))return false;
        at(kind==0?0x134518:kind==1?0x1344a8:0x134590);history.words38+=4;
        //134550 increments scratch by FOUR even when copying only2/3 words.
        at(0x3062c0);if(!p.packet||!p.buffer20)return false;
        for(unsigned i=0;i<count;++i)if(!write(p.count28+i,words[i]))return false;
        at(kind==0?0x134538:kind==1?0x1344c8:0x1345b0);p.count28+=count;f.streamPayloadCount=p.count28;return true;
    };
    auto finish=[&](unsigned stream){
        if(!guard(4,0x134660,0x134674,0x134678))return false;
        if(history.enabled[4]&&!history.words38)return stop(0x1346a0);
        const auto qwords=(history.words38+3u)/4u;
        if(!guard(5,0x1346ec,0x134700,0x134704))return false;
        if(history.enabled[5]&&qwords>256)return stop(0x134724);
        if(p.field2C!=1)return stop(0x1347bc);
        at(0x134818);if(!write(history.placeholder28,((history.format40|0x60u)<<24)|
            ((qwords==256?0:qwords)<<16)|(history.target30|0x8000u)))return false;
        at(0x13482c);history.target30+=qwords;f.streamPayloadFinished|=1u<<stream;return true;
    };
    if(!begin(12,true))return stop(pc);
    at(0x23d0a8);if(!group.batches)return stop(pc);f.streamPayloadOffsets[0]=p.count28;
    at(0x23d0c4);f.streamStackTagD0[0]=(f.streamStackTagD0[0]&0xffff8000u)|4u;
    std::array<std::optional<std::uint32_t>,4> tag;
    for(unsigned i=0;i<4;++i)tag[i]=f.streamStackTagD0[i];
    if(!copy(tag,4,0))return stop(pc);f.streamPayloadCopies[0]=1;
    if(!finish(0))return stop(pc);
    at(0x23d100);if(!begin(12,false))return stop(pc);
    at(0x23d120);if(!group.positions)return stop(pc);f.streamPayloadOffsets[1]=p.count28;
    group.positions[0]={attrs.vertices_->positions.data(),4};
    for(unsigned i=0;i<4;++i){const std::int32_t control=i<2?-14:14;
        at(0x23d144);std::array<std::optional<std::uint32_t>,4> words{};words[3]=static_cast<std::uint32_t>(control);
        //StackC0/C4/C8 are never initialized by this routine. Do not read or zero them.
        at(0x23d148);if(!copy(words,4,1))return stop(pc);
        attrs.vertices_->positions[i].stripControl=control;++f.streamPayloadCopies[1];}
    if(!finish(1))return stop(pc);
    at(0x23d1a0);if(!begin(8,false))return stop(pc);
    at(0x23d1c0);if(!group.triples)return stop(pc);f.streamPayloadOffsets[2]=p.count28;
    group.triples[0]={attrs.vertices_->triples.data(),4};
    for(unsigned i=0;i<4;++i){at(0x23d1cc);if(!copy({},3,2))return stop(pc);++f.streamPayloadCopies[2];}
    if(!finish(2))return stop(pc);
    //Format7 excludes extra bit32; source array24 elements remain unknown.
    at(0x23d2b0);if(!begin(4,false))return stop(pc);
    at(0x23d2d0);if(!group.texcoords)return stop(pc);f.streamPayloadOffsets[3]=p.count28;
    group.texcoords[0]={attrs.vertices_->texcoords.data(),4};
    for(unsigned i=0;i<4;++i){at(0x23d2dc);if(!copy({},2,2))return stop(pc);++f.streamPayloadCopies[3];}
    if(!finish(3))return stop(pc);
    //Format7 excludes color bit8; source array28 elements remain unknown.
    at(0x23d394);if(!capacity(1,true))return stop(pc);
    at(0x134ac8);if(!write(p.count28,0x17000000u))return stop(pc);
    at(0x134ad4);++p.count28;f.streamPayloadCount=p.count28;
    at(0x23d3bc);f.streamDescriptorParameterEnd=p.count28;
    at(0x23d3d0);f.streamDescriptorParameterSpan=p.count28-*f.streamDescriptorParameterBegin;
    at(0x23d414);f.streamReturned=true;
    at(0x23bfbc);return continueSourceFinishLineHeader23BFBC(pc);
}
bool Owner::preflightSourceFinishLineStreamsNative()const noexcept {
    if(!sourceFinishLineStripOwner_)return !sourceFinishLineStreams_.streamEntered||sourceFinishLineParameterWithdrawn_;
    const auto& f=sourceFinishLineStreams_;const auto& attrs=sourceFinishLineStripOwner_->attributes_;
    const auto& geometry=sourceFinishLineGeometry_;
    for(unsigned kind=0;kind<3;++kind)for(unsigned vertex=0;vertex<4;++vertex){
        const auto& receipts=geometry.assertions[kind][vertex];
        for(const auto& receipt:receipts)if(!g::OriginalStripStreamHistory23CCA0::current(receipt))return false;
        if(receipts[0].entered&&!geometry.entered)return false;
        if((geometry.setterMasks[kind]&(1u<<vertex))&&
           (!receipts[0].ready||!receipts[1].passed||!receipts[2].passed))return false;
    }
    for(auto mask:geometry.setterMasks)if(mask>15u||(mask&(mask+1u)))return false;
    if((geometry.zeroNormalOriginWords&&(!geometry.entered||geometry.setterMasks[0]!=15u))||
       (geometry.setterMasks[0]&&!geometry.entered)||
       (geometry.setterMasks[1]&&geometry.setterMasks[0]!=15u)||
       (geometry.setterMasks[2]&&geometry.setterMasks[1]!=15u)||
       (geometry.entered&&!sourceFinishLineHeader_.setupReturned)||
       (geometry.returned&&(geometry.setterMasks[2]!=15u||geometry.pc!=0x1b4090)))return false;

    using StreamHistory=g::OriginalStripStreamHistory23CCA0;
    if(!StreamHistory::current(f.streamBatchAssertion))return false;
    for(const auto& r:f.streamAliasAssertions)if(!StreamHistory::current(r))return false;
    const auto prefix=[](unsigned value,unsigned maximum){return value<=maximum&&!(value&(value+1u));};
    if(!prefix(f.streamDescriptorMask,7)||!prefix(f.streamArraysAttempted,127)||
       f.streamArraysPublished!=f.streamArraysAttempted||
       (f.streamEntered&&!sourceFinishLineParameterReturned_)||
       (f.streamCountStored&&!f.streamStackTagKnown)||
       (f.streamDescriptorMask&&!f.streamCountStored)||
       (f.streamArraysAttempted&&!f.streamBatchAssertion.passed)||
       (f.streamAliasAllocationAttempted&&f.streamArraysPublished!=127)||
       f.streamAliasPublished!=f.streamAliasAllocationAttempted||
       (f.streamBatchCountStored&&!f.streamDescriptorParameterBegin)||
       (f.streamDescriptorParameterBegin&&(!f.streamAliasPublished||*f.streamDescriptorParameterBegin!=0))||
       f.streamAliasConstructed>4)return false;
    if(!f.streamEntered&&(f.streamStackTagKnown||f.streamCountStored||f.streamDescriptorMask||
       f.streamArraysAttempted||f.streamAliasAllocationAttempted||f.streamPayloadEntered))return false;
    if(f.streamStackTagKnown){const auto low=f.streamPayloadCopies[0]?0x8004u:0x8000u;
        //The low tag count store precedes copying, so a bounded null-packet
        //copy failure can retain8004 with zero successful copies.
        if((f.streamStackTagD0[0]!=low&&!(f.streamPayloadOffsets[0]&&f.streamStackTagD0[0]==0x8004u))||
           f.streamStackTagD0[1]!=0x30364000u||f.streamStackTagD0[2]!=0x412u||f.streamStackTagD0[3])return false;}
    if(attrs.group_){const auto& group=*attrs.group_;
        if(group.vertexCount!=((f.streamDescriptorMask&1)?4u:0u)||
           group.batchCount!=((f.streamDescriptorMask&4)?1u:0u)||
           (!(f.streamArraysPublished&1)&&group.counts)||(!(f.streamArraysPublished&2)&&group.batches)||
           (!(f.streamArraysPublished&4)&&group.positions)||(!(f.streamArraysPublished&8)&&group.triples)||
           (!(f.streamArraysPublished&16)&&group.extra)||(!(f.streamArraysPublished&32)&&group.colors)||
           (!(f.streamArraysPublished&64)&&group.texcoords)||(!f.streamAliasPublished&&group.aliases))return false;
        //Array elements are read only after their source producer receipt.
        if(f.streamBatchCountStored&&(!group.counts||group.counts[0]!=4))return false;
        if(group.aliases){if(f.streamAliasConstructed!=4)return false;
            for(unsigned i=0;i<4;++i){const auto mask=f.streamAliasStoreMask[i];const auto& a=group.aliases[i];
                if(!prefix(mask,7)||a.count!=((mask&4)?1u:0u)||a.aliases[0].batch||
                   a.aliases[0].ordinal!=((mask&2)?i:0u)||a.aliases[1].batch||a.aliases[1].ordinal||
                   (mask&&!f.streamAliasAssertions[i].passed))return false;}}
        else {if(f.streamAliasConstructed)return false;for(auto mask:f.streamAliasStoreMask)if(mask)return false;}
    }else if(f.streamDescriptorMask||f.streamArraysAttempted||f.streamAliasAllocationAttempted)return false;
    if((f.streamPayloadEntered&&(!f.streamStackTagKnown||!f.streamDescriptorParameterBegin||!f.streamBatchCountStored))||
       (f.streamPayloadNativeBacking&&!f.streamPayloadEntered)||
       bool(attrs.vertices_)!=f.streamPayloadNativeBacking||f.streamPayloadCount>128||
       !prefix(f.streamPayloadFinished,15)||
       (f.streamDescriptorParameterEnd&&*f.streamDescriptorParameterEnd!=f.streamPayloadCount)||
       (f.streamDescriptorParameterSpan&&(!f.streamDescriptorParameterEnd||!f.streamDescriptorParameterBegin||
         *f.streamDescriptorParameterSpan!=*f.streamDescriptorParameterEnd-*f.streamDescriptorParameterBegin))||
       (f.streamReturned&&(!f.streamDescriptorParameterSpan||f.streamPayloadFinished!=15||f.streamPayloadCount!=45)))return false;
    for(unsigned i=0;i<4;++i){if(f.streamPayloadCopies[i]>(i?4:1)||
        (f.streamPayloadOffsets[i]&&*f.streamPayloadOffsets[i]>f.streamPayloadCount)||
        (f.streamPayloadCopies[i]&&!f.streamPayloadOffsets[i])||
        ((f.streamPayloadFinished&(1u<<i))&&f.streamPayloadCopies[i]!=(i?4:1)))return false;}
    if(attrs.vertices_){if(!attrs.group_)return false;const auto& group=*attrs.group_;
        if(f.streamPayloadOffsets[1]&&(!group.positions||group.positions[0].values!=attrs.vertices_->positions.data()||group.positions[0].count!=4))return false;
        if(f.streamPayloadOffsets[2]&&(!group.triples||group.triples[0].values!=attrs.vertices_->triples.data()||group.triples[0].count!=4))return false;
        if(f.streamPayloadOffsets[3]&&(!group.texcoords||group.texcoords[0].values!=attrs.vertices_->texcoords.data()||group.texcoords[0].count!=4))return false;
        for(unsigned i=0;i<4;++i){const auto& v=attrs.vertices_->positions[i];
            if(v.stripControl!=(i<f.streamPayloadCopies[1]?(i<2?-14:14):0))return false;
            const auto lane=[&](const auto& scalar,unsigned kind,unsigned k,unsigned stride){
                const bool known=(geometry.setterMasks[kind]&(1u<<i))!=0;
                if(!known)return !scalar.value&&scalar.unknownScratchLane==k;
                if(!scalar.value||scalar.unknownScratchLane!=0xffu||!f.streamPayloadOffsets[kind+1])return false;
                const unsigned word=*f.streamPayloadOffsets[kind+1]+i*stride+k;
                std::uint32_t bits;const float value=*scalar.value;std::memcpy(&bits,&value,4);
                return word<128&&f.streamPayloadWords[word]&&*f.streamPayloadWords[word]==bits;
            };
            for(unsigned k=0;k<3;++k)if(!lane(v.xyz[k],0,k,4)||
                !lane(attrs.vertices_->triples[i].components[k],1,k,3))return false;
            for(unsigned k=0;k<2;++k)if(!lane(attrs.vertices_->texcoords[i].uv[k],2,k,2))return false;}}
    return true;
}
bool Owner::continueSourceFinishLineHeader23BFBC(std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceFinishLineSetupPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    auto& f=sourceFinishLineHeader_;
    using History=g::OriginalParameterPacketHistory50LA;auto& history=History::state();
    //Synchronous tail of134000's existing RAII lease. Never call external
    //preflight/canWithdrawNative here: it would lock this same history again.
    if(sourceFinishLineSetupPC_!=0x23bfbc||f.entered||!sourceFinishLineStreams_.streamReturned||
       !sourceFinishLineStripOwner_||!sourceFinishLineParameters54_||
       !history.busy||history.thread!=std::this_thread::get_id()||!history.scratchAccessible||
       history.serial!=sourceFinishLineParameterSerial_||history.scratchSerial!=sourceFinishLineParameterSerial_||
       sourceFinishLineParameters54_->serial!=sourceFinishLineParameterSerial_)return false;
    auto& p=*sourceFinishLineParameters54_;auto& strip=*sourceFinishLineStripOwner_;
    f.entered=true;at(0x134200);
    if(!History::guard(3,0x13420c,0x134220,0x134224,pc))return stop(pc);
    if(history.enabled[3]&&p.count28>p.count24)return stop(0x134250);
    //Retail allocator305700/305990 and native align64 owner both supply16-aligned
    //payloads; count-based padding therefore reaches the same boundary.
    if(p.count28>128)return stop(0x1342b0);
    for(unsigned i=p.count28;i%4u;++i){at(0x1342c4);
        if(!p.packet||!p.buffer20||i>=128)return stop(pc);
        p.packet->words[i]=0;sourceFinishLineStreams_.streamPayloadWords[i]=0;++f.paddingWords;}
    //1342B0 pads local pointer only; count28/descriptor span remain45, not48.
    at(0x134300);p.header00To1C[0]=(*p.header00To1C[0]&0xffff0000u)|
        (((p.count28+f.paddingWords)/4u)&0xffffu);f.storeMask|=1;
    at(0x134308);p.header00To1C[0]=*p.header00To1C[0]&0xff00ffffu;f.storeMask|=2;
    at(0x13430c);p.header00To1C[0]=(*p.header00To1C[0]&0x00ffffffu)|0x30000000u;f.storeMask|=4;
    at(0x134310);p.nativePayload04=p.buffer20;p.header00To1C[1].reset();f.storeMask|=8;
    at(0x134314);p.header00To1C[2]=0;f.storeMask|=16;
    at(0x13431c);p.header00To1C[3]=0;f.storeMask|=32;
    at(0x134294);f.returned=true;
    at(0x23bfd8);if(!strip.primitive30_)return stop(pc);
    //Actual Primitive+10 takes parameter54, NOT legacy Primitive.attributes10.
    f.primitiveParameters10=&p;f.primitiveMask|=1;auto& primitive=*strip.primitive30_;
    at(0x23bfec);primitive.flags0F&=~1u;f.primitiveMask|=2;
    at(0x23bff8);primitive.groups22=1;f.primitiveMask|=4;
    at(0x23c000);if(!strip.attributes_.groupCounts_)return stop(pc);
    const auto vertices=static_cast<std::uint16_t>(strip.attributes_.groupCounts_[0]);
    at(0x23c014);primitive.vertices20=static_cast<std::uint16_t>(primitive.vertices20+vertices);f.primitiveMask|=8;
    at(0x23c028);f.setupReturn=&strip; //Actual V0=S3, no invented scalar return.
    at(0x23c040);f.setupReturned=true;
    at(0x1b3bfc);return true; //Caller continuation; no material-index/anchor producer yet.
}
bool Owner::preflightSourceFinishLineHeaderNative()const noexcept {
    const auto& f=sourceFinishLineHeader_;
    const auto prefix=[](unsigned v,unsigned max){return v<=max&&!(v&(v+1u));};
    if(!prefix(f.storeMask,63)||!prefix(f.primitiveMask,15)||f.paddingWords>3||
       (f.entered&&!sourceFinishLineStreams_.streamReturned)||
       (!f.entered&&(f.storeMask||f.paddingWords||f.returned||f.primitiveMask||f.setupReturned))||
       (f.storeMask&&f.paddingWords!=3)|| (f.returned&&f.storeMask!=63)||
       (f.primitiveMask&&!f.returned)||(f.setupReturned&&f.primitiveMask!=15))return false;
    if(f.borrowersWithdrawn)return sourceFinishLineParameterWithdrawn_&&!sourceFinishLineParameters54_&&
        !sourceFinishLineStripOwner_&&!f.primitiveParameters10&&!f.setupReturn;
    if(f.primitiveParameters10){if(!(f.primitiveMask&1)||!sourceFinishLineStripOwner_||
        !sourceFinishLineStripOwner_->primitive30_||f.primitiveParameters10!=sourceFinishLineParameters54_.get())return false;}
    else if(f.primitiveMask&1)return false;
    if(f.setupReturn){if(!f.setupReturned||f.setupReturn!=sourceFinishLineStripOwner_.get())return false;}
    else if(f.setupReturned)return false;
    if(f.paddingWords){const auto* p=sourceFinishLineParameters54_.get();
        if(!p||!p->packet||p->count28!=45)return false;
        for(unsigned i=45;i<45u+f.paddingWords;++i)
            if(sourceFinishLineStreams_.streamPayloadWords[i]!=0u||p->packet->words[i]!=0u)return false;}
    return true;
}
bool Owner::continueSourceFinishLineIndex1B3BFC(std::uint32_t& pc)noexcept {
    //The derived caller admitted the same Application's live382B38 table.
    //Reuse that owner, never the legacy World/modelPrefixes material domain.
    if(pc!=0x1b3bfcu||sourceFinishLineIndexLookupEntered_||sourceFinishLineMaterial544_||
       !sourceFinishLineHeader_.setupReturned||sourceFinishLineHeader_.borrowersWithdrawn||
       sourceFinishLineHeader_.setupReturn!=sourceFinishLine540_||!sourceFinishLine540_||
       !sourceFinishLineMaterialTable_||!preflightSourceFinishLineNative())return false;
    sourceFinishLineIndexLookupEntered_=true;
    pc=0x1b3c0c;
    rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
    std::int32_t index=-1;
    //A2=1 at1B3C10 requests the source named lookup. The existing lookup
    //adapter bounds absent/invalid names at298D64; refusal is NOT V0=-1.
    //Do not publish World544 or fabricate the source return on that path.
    try {
        if(!sourceFinishLineMaterialTable_->find298C80("textures\\finishline",index,diagnostic)){
            if(diagnostic.sourcePC)pc=diagnostic.sourcePC;return false;
        }
    }catch(...){return false;} //Name allocation failure preserves abortable lookup prefix.
    pc=0x1b3c14;sourceFinishLineMaterial544_=index;
    //Original1B3C1C reads4342D0 before initializing both stack anchors.
    //No genuine startup3C9CCC/global owner is represented on this path.
    //Stop before that read: do not substitute the legacy constant sentinel,
    //skip it to scan World3C4[0], or claim anchor/geometry readiness.
    pc=0x1b3c1c;return true;
}

//Dedicated source-prefix continuation. No legacy owner or readiness flags.
//Host binary32/FE_TONEAREST is a bounded adaptation, not EE FCSR/ACC/VU parity.
//The caller's FP environment is inspected, never changed; exception flags and
//FTZ/DAZ are not emulated. Nonfinite/subnormal arithmetic stops at its first PC.
bool Owner::continueSourceFinishLineGeometry1B3DDC(
    const std::array<std::uint32_t,4>& leftWords,const std::array<std::uint32_t,4>& rightWords,
    std::uint32_t& pc)noexcept {
    auto& h=sourceFinishLineGeometry_;
    if(pc!=0x1b3ddc||h.entered||!sourceFinishLine540_||!sourceFinishLineMaterial544_||
       !sourceFinishLineHeader_.setupReturned||sourceFinishLineHeader_.borrowersWithdrawn||
       !preflightSourceFinishLineNative())return false;
    h.entered=true;
    auto at=[&](std::uint32_t next){pc=h.pc=next;};
    if(std::fegetround()!=FE_TONEAREST)return false;
    auto& attrs=sourceFinishLineStripOwner_->attributes_;
    auto& streams=sourceFinishLineStreams_;auto& packet=*sourceFinishLineParameters54_;
    if(!attrs.vertices_||!packet.packet||!packet.buffer20)return false;
    //All three source streams and aliases were produced by23CCA0/23D088.
    //Admit their bounded native backing before setters; no allocation/callback
    //or packet-history reacquisition occurs within this numeric continuation.
    for(unsigned kind=0;kind<3;++kind){const unsigned width=kind==0?4:kind==1?3:2;
        if(!streams.streamPayloadOffsets[kind+1]||
           *streams.streamPayloadOffsets[kind+1]+4*width>packet.count28)return false;}
    using Vec3=g::finishline_numeric50LA::Vec3;
    g::finishline_numeric50LA::detail::Math m;
    auto okay=[&](){if(m.badPC){at(m.badPC);return false;}return true;};
    Vec3 left,right;for(unsigned k=0;k<3;++k){std::memcpy(&left[k],&leftWords[k],4);std::memcpy(&right[k],&rightWords[k],4);}
    auto set=[&](unsigned kind,unsigned vertex,const Vec3& value,std::uint32_t callPC){
        at(callPC);using History=g::OriginalStripStreamHistory23CCA0;
        const bool triple=kind==1,uv=kind==2;const unsigned first=uv?2:triple?5:8;
        auto& receipts=h.assertions[kind][vertex];
        at(uv?0x23c820:triple?0x23c278:0x23c0a8);
        if(!History::begin(first,receipts[0]))return false; //Lazy-1 word, no assertion read.
        at(uv?0x23c864:triple?0x23c2b4:0x23c0e4);
        if(!History::begin(first+1,receipts[1]))return false;
        if(receipts[1].enabled&&!attrs.group_){at(uv?0x23c894:triple?0x23c2e4:0x23c114);return false;}
        at(uv?0x23c8a4:triple?0x23c2f4:0x23c124);
        if(!History::finish(receipts[1],attrs.group_&&bool(attrs.group_->aliases)))return false;
        at(uv?0x23c8f4:triple?0x23c32c:0x23c15c);if(!attrs.group_)return false;
        auto& group=*attrs.group_;
        at(uv?0x23c8e8:triple?0x23c334:0x23c164);
        if(!History::begin(first+2,receipts[2]))return false;
        if(receipts[2].enabled&&!group.aliases){at(uv?0x23c92c:triple?0x23c360:0x23c190);return false;}
        at(uv?0x23c93c:triple?0x23c370:0x23c1a0);
        if(!History::finish(receipts[2],group.aliases&&group.aliases[vertex].count<3))return false;
        at(uv?0x23c974:triple?0x23c3a8:0x23c1d8);
        //Authentic23CCA0 created exactly one batch0/ordinal==vertex alias.
        //This native bound does not infer aliases from an unrelated strip.
        if(!group.aliases||group.aliases[vertex].count!=1||
           group.aliases[vertex].aliases[0].batch||group.aliases[vertex].aliases[0].ordinal!=vertex)return false;
        const unsigned width=kind==0?4:kind==1?3:2,lanes=uv?2:3;
        for(unsigned k=0;k<lanes;++k){
            at(uv?(k?0x23c9b8:0x23c9b4):triple?(k==0?0x23c3f0:k==1?0x23c3f8:0x23c400):
                (k==0?0x23c21c:k==1?0x23c224:0x23c22c));
            auto& scalar=kind==0?attrs.vertices_->positions[vertex].xyz[k]:
                kind==1?attrs.vertices_->triples[vertex].components[k]:attrs.vertices_->texcoords[vertex].uv[k];
            scalar={value[k],0xffu};std::uint32_t bits;std::memcpy(&bits,&value[k],4);
            const auto word=*streams.streamPayloadOffsets[kind+1]+vertex*width+k;
            packet.packet->words[word]=bits;streams.streamPayloadWords[word]=bits;
        }
        h.setterMasks[kind]|=1u<<vertex;
        at(uv?0x23c9ec:triple?0x23c42c:0x23c254);return true;
    };
    //Actual source setter order. Position0 survives even a failing Y subtraction;
    //all four positions survive a later cross/normalization admission failure.
    if(!set(0,0,left,0x1b3df0))return false;
    left[1]=m.sub(left[1],40.f,0x1b3e0c);if(!okay())return false;
    if(!set(0,1,left,0x1b3e20)||!set(0,2,right,0x1b3e3c))return false;
    right[1]=m.sub(right[1],40.f,0x1b3e58);if(!okay())return false;
    if(!set(0,3,right,0x1b3e6c))return false;
    Vec3 span,above=left,down,normal;
    span[2]=m.sub(right[2],left[2],0x1b3e90);if(!okay())return false;
    span[1]=m.sub(right[1],left[1],0x1b3ea0);if(!okay())return false;
    span[0]=m.sub(right[0],left[0],0x1b3eac);if(!okay())return false;
    above[1]=m.add(left[1],40.f,0x1b3ec4);if(!okay())return false;
    down[0]=m.sub(left[0],above[0],0x1b3ee8);if(!okay())return false;
    down[1]=m.sub(left[1],above[1],0x1b3eec);if(!okay())return false;
    down[2]=m.sub(left[2],above[2],0x1b3ef4);if(!okay())return false;
    //13E454/13E458 vector cross. Separately rounded host products/subtracts
    //are explicit native adaptations, not an original VU execution receipt.
    for(unsigned k=0;k<3;++k){const unsigned j=(k+1)%3,l=(k+2)%3;
        const float a=m.mul(span[j],down[l],0x13e454);if(!okay())return false;
        const float b=m.mul(span[l],down[j],0x13e458);if(!okay())return false;
        normal[k]=m.sub(a,b,0x13e458);if(!okay())return false;}
    const float xx=m.mul(normal[0],normal[0],0x1b3f28);if(!okay())return false;
    const float yy=m.mul(normal[1],normal[1],0x1b3f2c);if(!okay())return false;
    const float xy=m.add(xx,yy,0x1b3f30);if(!okay())return false;
    const float zz=m.mul(normal[2],normal[2],0x1b3f34);if(!okay())return false;
    const float square=m.add(xy,zz,0x1b3f34);if(!okay())return false;
    if(square==0.f){
        //Actual1B3F4C LQ /1B3F54 SQ copies the current same-App process vector,
        //not the anchor-scan snapshot. Retain W receipt but setters consume XYZ.
        at(0x1b3f4c);std::array<std::uint32_t,4> origin;
        if(!worldApplication_->readWorldOrigin4342D0(origin))return false;
        at(0x1b3f54);h.zeroNormalOriginWords=origin;
        for(unsigned k=0;k<3;++k)std::memcpy(&normal[k],&origin[k],4);
    }else{
        const float inverse=m.reciprocalRoot(square,0x1b3f64);if(!okay())return false;
        constexpr std::uint32_t multiplyPC[]{0x1b3f74,0x1b3f80,0x1b3f8c};
        for(unsigned k=0;k<3;++k){normal[k]=m.mul(normal[k],inverse,multiplyPC[k]);if(!okay())return false;}
    }
    constexpr std::uint32_t normalPC[]{0x1b3fa8,0x1b3fc4,0x1b3fe0,0x1b3ffc};
    for(unsigned v=0;v<4;++v)if(!set(1,v,normal,normalPC[v]))return false;
    constexpr std::uint32_t uvPC[]{0x1b401c,0x1b4040,0x1b4064,0x1b4088};
    for(unsigned v=0;v<4;++v)if(!set(2,v,Vec3{{v>=2?1.f:0.f,(v&1)?1.f:0.f,0.f}},uvPC[v]))return false;
    at(0x1b4090);h.returned=true;return true;
}
bool Owner::destroySourceMissingFinishLine1B3DC0(std::uint32_t& pc)noexcept {
    //The derived caller proves the actual fresh XYZ comparison selected this
    //branch. This base helper owns resource semantics, never the comparison.
    if(pc!=0x1b3dc0||sourceFinishLineMissingCleanup_.entered||
       !sourceFinishLineStripOwner_||!sourceFinishLineMaterial544_||
       !sourceFinishLineHeader_.setupReturned||sourceFinishLineHeader_.borrowersWithdrawn||
       !preflightSourceFinishLineNative())return false;
    //Native admission/borrow withdrawal precedes all original effects. It is
    //not a retail global write or a claim that133FA0 clears process scratch.
    if(!g::OriginalParameterPacketHistory50LA::withdrawNative(sourceFinishLineParameterSerial_))return false;
    auto& h=sourceFinishLineMissingCleanup_;auto& s=*sourceFinishLineStripOwner_;
    h.entered=true;h.material544Before=sourceFinishLineMaterial544_;
    //3D1EA0+10=142590: current detached entity, no children or sibling links.
    //142654 calls104970;104980 decrements sole reference before virtual+8,
    //3D1EA8=23BCF0(a1=1). No native-abort helper is invoked here.
    if(!sceneApplication_->entityAuthority_.retire(s.entity_))std::terminate(); //Native withdrawal before last release.
    pc=0x104980;--s.entity_.reference00;h.releaseMask|=1;
    sourceFinishLineHeader_.primitiveParameters10=nullptr;
    sourceFinishLineHeader_.setupReturn=nullptr;
    sourceFinishLineHeader_.borrowersWithdrawn=true;
    pc=0x23bd24;s.attributes_.common_.reset();h.releaseMask|=2;
    //23BD38→133FA0(1): buffer20 free305728, then parameter delete2E8B00.
    pc=0x133fc8;sourceFinishLineParameters54_->nativePayload04=nullptr;
    sourceFinishLineParameters54_->buffer20=nullptr;sourceFinishLineParameters54_->packet.reset();
    s.attributes_.vertices_.reset(); //Native typed views share source54 payload lifetime.
    pc=0x133fe0;sourceFinishLineParameters54_.reset();sourceFinishLineParameterWithdrawn_=true;h.releaseMask|=4;
    //23BD50 array teardown invokes23BDB0 for each descriptor: source14..2C
    //arrays, then source30 aliases, then descriptor storage. The seven
    //native array owners retain their actual23CEA4..23CF04 allocation order.
    pc=0x23bd50;
    if(s.attributes_.group_){auto& g=*s.attributes_.group_;
        g.counts.reset();g.batches.reset();g.positions.reset();g.triples.reset();
        g.extra.reset();g.colors.reset();g.texcoords.reset();g.aliases.reset();
        s.attributes_.group_.reset();}
    h.releaseMask|=8;
    pc=0x23bd64;s.attributes_.groupCounts_.reset();h.releaseMask|=16;
    //23BD70→1503B0(0): groups38, then primitive30 via3D20C8=246260(1).
    pc=0x1503e4;s.groups38_.reset();h.releaseMask|=32;
    //Primitive flags0F bit0 is clear: its stale source+10 borrow is NOT freed
    //again.2462C4→299AC0 releases exactly the retained material reference.
    pc=0x2462c4;s.primitive30_.reset();h.releaseMask|=64;
    //150414→1413E0(0), count store141420. Native authority was withdrawn
    //before the original last-reference release, not decremented here again.
    pc=0x141420;
    --sceneApplication_->objectCount423F60();h.releaseMask|=128;
    pc=0x23bd88;sourceFinishLineStripOwner_.reset();
    sourceFinishLineMaterialTable_.reset();sourceFinishLineDirectory_=nullptr;
    //Only AFTER virtual+10 returns: unconditional branch delay slot clears540.
    //544 remains numeric history, never released as a second material owner.
    pc=0x1b3dd8;sourceFinishLine540_=nullptr;sourceFinishLine540Published_=false;h.releaseMask|=256;
    h.returned=true;pc=0x1b4090;return true; //Reached epilogue, not caller readiness.
}
bool Owner::retireSourceFinishLineNative()noexcept {
    if(!preflightSourceFinishLineNative())return false;
    if(!sourceFinishLineStripOwner_)return true;
    auto& s=*sourceFinishLineStripOwner_;
    //Explicit native abort of detached partial setup ownership, not a
    //claim that original World destructor or missing-anchor branch executed.
    if(sourceFinishLineParameterSerial_&&
       !g::OriginalParameterPacketHistory50LA::withdrawNative(sourceFinishLineParameterSerial_))return false;
    //Withdraw all typed source borrowers before destroying their dedicated owner.
    //Primitive.attributes10 remains the unrelated legacy attributes projection.
    sourceFinishLineHeader_.primitiveParameters10=nullptr;sourceFinishLineHeader_.setupReturn=nullptr;
    sourceFinishLineHeader_.borrowersWithdrawn=true;
    if(sourceFinishLineParameters54_)sourceFinishLineParameters54_->nativePayload04=nullptr;
    sourceFinishLineParameters54_.reset();sourceFinishLineParameterWithdrawn_=true;
    sourceFinishLine540_=nullptr;sourceFinishLine540Published_=false;
    if(!sceneApplication_->entityAuthority_.retire(s.entity_))std::terminate();
    --s.entity_.reference00;s.attributes_.release23BCF0();
    s.groups38_.reset();s.primitive30_.reset(); //Release only the genuinely retained299BE0 reference.
    sourceFinishLineMaterialTable_.reset();sourceFinishLineDirectory_=nullptr;
    --sceneApplication_->objectCount423F60();sourceFinishLineStripOwner_.reset();
    //Keep scalar constructor/group-word history; cannot replay after abort.
    return true;
}
bool Owner::canRetireNativeFinishLine540()const noexcept {
    if(!nativeFinishLine540_)return true;
    if(!sceneApplication_||sceneApplication_->attachmentLock423F58())return false;
    const auto& f=*nativeFinishLine540_;
    if(f.nativeStartupSentinel4342D0!=std::array<float,4>{{0.f,0.f,0.f,1.f}}||!f.table||!f.table->prepared())return false;
    if(!f.strip)return !f.counted&&!f.setupStarted&&!f.setupComplete&&!f.complete;
    const auto& s=*f.strip;const auto& e=s.entity_;
    if(!f.counted||e.kind!=g::OriginalWreckComponentKind50LA::strip23BCA0||e.objectKey!=key(&e)||
       e.sessionGeneration!=sceneGeneration_||e.reference00!=1u||e.category28!=0xffu||(e.flags29&15u)!=3u||
       e.nativeFlags29||e.parent1C||e.firstChild18||e.previous10||e.next14||e.mask20||e.descendants24||
       !sceneApplication_->entityAuthority_.canRetire(e)||!sceneApplication_->objectCount423F60())return false;
    if(s.primitive30_&&!s.primitive30_->reference.canRelease299AC0())return false;
    if(f.setupComplete&&(!f.setupStarted||!s.primitive30_||!s.primitive30_->reference.retained()||
       s.format3C_!=7u||s.count34_!=1u||s.field40_!=0u||!s.groups38_||s.groups38_[0]!=4u||
       s.color44_!=0x00ffffffu||s.color70_!=std::array<float,4>{{127.f,127.f,127.f,64.f}}||
       s.primitive30_->byte0C!=7u||s.primitive30_->attributes10!=&s.attributes_||
       s.primitive30_->vertices20!=4u||s.primitive30_->groups22!=1u))return false;
    return !f.complete||(f.setupComplete&&f.anchorsWritten&&f.material544&&*f.material544>=0&&!f.terminal);
}
bool Owner::retireNativeFinishLine540()noexcept {
    if(!nativeFinishLine540_)return true;
    if(!canRetireNativeFinishLine540())return false;
    auto& f=*nativeFinishLine540_;
    if(f.strip){auto& s=*f.strip;
        if(!sceneApplication_->entityAuthority_.retire(s.entity_))std::terminate();
        --s.entity_.reference00;
        //Actual standalone142590→23BCF0 resources, then1503B0 Entity base.
        //No later1B4D0C parent/mask8 attachment has been enabled here.
        s.attributes_.release23BCF0();s.groups38_.reset();s.primitive30_.reset();
        --sceneApplication_->objectCount423F60();f.strip.reset();
    }
    //544 was only an index. No spurious material release for its lookup.
    nativeFinishLine540_.reset();return true;
}
bool Owner::setupNativeFinishLine540(std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
    const std::string& directory,std::uint32_t& pc)noexcept {
    pc=0x1b3ba4;
    if(!sceneApplication_||!table||!table->prepared()||sceneApplication_->attachmentLock423F58())return false;
    try {
        if(!nativeFinishLine540_){nativeFinishLine540_=std::make_unique<NativeFinishLine540>();nativeFinishLine540_->table=table;}
        auto& f=*nativeFinishLine540_;
        if(f.table!=table||f.terminal||!canRetireNativeFinishLine540())return false;
        if(f.setupComplete)return true;
        if(f.setupStarted)return false;
        f.terminal=true; //Entered effects never replay after partial failure.
        if(!f.strip){
            using Strip=g::OriginalPodCableStripCold50LA;
            auto strip=std::make_unique<Strip>();auto& e=strip->entity_;
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            pc=0x1b3bc0;
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(e,key(&e)))return false;
            if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::strip23BCA0,
                sceneGeneration_,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(e))std::terminate();
            strip->flags48_=1;f.strip=std::move(strip);f.counted=true; //Real540 publication1B3BC8.
        }
        auto& strip=*f.strip;f.setupStarted=true;
        //Actual+74=23BEB0→150450, not a cable eighteen-vertex substitution.
        pc=0x150498;strip.format3C_=7;strip.count34_=1;strip.groups38_.reset(new std::uint32_t[1]);
        strip.field40_=0;strip.color44_=0x00ffffffu;strip.flags48_|=1u;
        strip.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}}; //23D750 ignores packed alpha for W.
        strip.groups38_[0]=4; //Actual382C38 integerword4, not float4.
        pc=0x1504f0;strip.primitive30_=std::make_unique<g::OriginalPodCableStripCold50LA::Primitive>();
        rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        if(!strip.primitive30_->reference.acquireNamed299BE0(table,directory,"finishline",diagnostic)){
            pc=diagnostic.sourcePC;f.failurePC=pc;return false;}
        strip.primitive30_->byte0C=7;
        strip.attributes_.constructQuad23BEB0(7,pc);
        strip.primitive30_->attributes10=&strip.attributes_;strip.primitive30_->flags0F&=static_cast<std::uint8_t>(~1u);
        strip.primitive30_->vertices20=4;strip.primitive30_->groups22=1;
        f.setupComplete=true;f.terminal=false;pc=0x1b3bfc;return true;
    }catch(...){if(nativeFinishLine540_){nativeFinishLine540_->terminal=true;nativeFinishLine540_->failurePC=pc;}return false;}
}
bool Owner::commitNativeFinishLineGeometry1B3DDC(std::uint32_t& pc)noexcept {
    pc=0x1b3ddc;
    if(!nativeFinishLine540_||!canRetireNativeFinishLine540())return false;
    auto& f=*nativeFinishLine540_;
    if(!f.setupComplete||!f.anchorsWritten||!f.material544||f.complete||f.terminal)return false;
    f.terminal=true;
    //Explicit bounded native atomic numeric admission before source setter
    //publication. No partial retail numeric-failure stores are claimed.
    g::finishline_numeric50LA::Geometry geometry;
    if(!g::finishline_numeric50LA::prepare1B3DDC(f.anchorXYZ[0],f.anchorXYZ[1],geometry,pc)){
        f.failurePC=pc;return false;}
    auto& attributes=f.strip->attributes_;
    constexpr std::uint32_t positionPC[]{0x1b3df0,0x1b3e20,0x1b3e3c,0x1b3e6c};
    constexpr std::uint32_t normalPC[]{0x1b3fa8,0x1b3fc4,0x1b3fe0,0x1b3ffc};
    constexpr std::uint32_t uvPC[]{0x1b401c,0x1b4040,0x1b4064,0x1b4088};
    //23C090/23C260 only XYZ. Do not manufacture source vector W or stripControl.
    for(std::size_t v=0;v<4u;++v){pc=positionPC[v];for(std::size_t k=0;k<3u;++k)
        attributes.vertices_->positions[v].xyz[k]={geometry.positions[v][k],0xffu};}
    for(std::size_t v=0;v<4u;++v){pc=normalPC[v];for(std::size_t k=0;k<3u;++k)
        attributes.vertices_->triples[v].components[k]={geometry.normals[v][k],0xffu};}
    for(std::size_t v=0;v<4u;++v){pc=uvPC[v];attributes.setTexcoord23C800(static_cast<std::uint32_t>(v),geometry.uv[v][0],geometry.uv[v][1]);}
    f.complete=true;f.terminal=false;pc=0x1b4090;return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeFinishLine1B3B80(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa208;
    //Only the actual+11C continuation calls this, AFTER its same coldPhase1
    //Camera/Display/context/idle and both retained-publication checks.
    if(!nativeStandardServiceComplete_||nativeStandardServiceTerminal_||!nativeBuoyCount538_||
       *nativeBuoyCount538_!=0||!world_||!world_->modelPrefixes_||!world_->domain_||
       !source_||!nativeSceneInfo270900Current(app,pc))return false;
    //1B3AF8 null53C returns; nonnull53C reaches538==0 loop test before any
    //element access. Both source branches have no effect.53C remains UNKNOWN.
    nativeBuoyAttachNoEffect1B3AE0_=true;pc=0x1aa210;
    if(nativeFinishLine540_&&nativeFinishLine540_->complete){
        if(!canRetireNativeFinishLine540())return false;
        return continueNativeCollisionObjects2743B0(app,pc); //Same explicit native progress policy, not observed guest330/+30.
    }
    if(nativeFinishLine540_&&nativeFinishLine540_->terminal){pc=nativeFinishLine540_->failurePC;return false;}
    try {
        const auto table=world_->domain_->materials382B38();
        if(!table||app.standardMaterialDomain()!=world_->domain_||!world_->context_->live())return false;
        if(!setupNativeFinishLine540(table,app.materialDirectory36B970(),pc)){
            if(nativeFinishLine540_)nativeFinishLine540_->failurePC=pc;return false;}

        auto& f=*nativeFinishLine540_;f.terminal=true;
        rr::assets::OriginalStandardMaterialDiagnostic50LA material;
        pc=0x1b3c0c;
        std::int32_t row=-1;
        if(!table->find298C80("textures\\finishline",row,material)||row<0){
            f.failurePC=material.sourcePC?material.sourcePC:pc;return false;}
        f.material544=row; //1B3C14 numeric index ONLY; no second reference.
        auto& models=*world_->modelPrefixes_;
        if(!models.canClose()||models.parents_.size()!=10u||!models.parents_[0]||
           models.parents_[0]->count30!=467u||models.parents_[0]->entries34.size()!=467u){f.failurePC=pc;return false;}
        const auto* data=source_->sourcesDecoded();if(!data){f.failurePC=pc;return false;}
        const auto equal=[](const std::string& left,const char* right)noexcept {
            std::size_t i=0;for(;i<left.size()&&right[i];++i){auto a=left[i],b=right[i];
                if(a>='A'&&a<='Z')a=static_cast<char>(a+'a'-'A');if(b>='A'&&b<='Z')b=static_cast<char>(b+'a'-'A');
                if(a!=b)return false;}return i==left.size()&&!right[i];
        };
        std::array<unsigned,2> matches{};
        std::array<std::array<float,3>,2> actual{{
            {{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}},
            {{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}}}};
        //Actual first3C4 list only, preserving source scan order. Unknown
        //retail wrappers are not synthesized: prior+11C now owns every slot.
        for(std::size_t ordinal=0;ordinal<467u;++ordinal){
            auto& slot=models.parents_[0]->entries34[ordinal];pc=0x1b3c30;
            if(!models.nativeStandardSlotCurrent27ABC0(0,ordinal)){f.failurePC=pc;return false;}
            auto* wrapper=models.nativeStandardWrapper(slot);
            if(!wrapper||!wrapper->model8)continue; //Genuine admitted negative-load null.
            if(!slot.model||wrapper->model8!=&slot.model->scene){f.failurePC=pc;return false;}
            auto& model=*slot.model;const auto* scene=model.scene.model();
            if(!scene||model.scene.released()){f.failurePC=pc;return false;}
            const int which=equal(scene->name38,"finish_droidleft.slt")?0:
                equal(scene->name38,"finish_droidright.slt")?1:-1;
            if(which<0)continue;
            if(slot.worldInstance!=465u+static_cast<unsigned>(which)||
               model.source.worldInstance!=slot.worldInstance||slot.worldInstance>=data->world.instances.size()||
               model.node.parent180!=g::SceneNodeIndex49P::none||!model.placed||!model.parentAttached){f.failurePC=pc;return false;}
            pc=which?0x1b3cb0:0x1b3c7c;
            //Actual28B1A0(a1=null): same real ready-cache/parentless graph.
            //If dirty, copy localC0→world100 and set ready bit; NEVER reparse
            //SCB or replace it with a separately calculated world transform.
            if(!g::refreshSceneWorld49P(model.graph(),static_cast<g::SceneNodeIndex49P>(0))){f.failurePC=pc;return false;}
            for(std::size_t k=0;k<3u;++k)actual[which][k]=model.node.world100.v[12u+k];
            ++matches[which]; //Actual corpus requires exactly one of each, not arbitrary last wins.
        }
        pc=0x1b3ce8;
        const std::array<float,3> sentinel{{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}};
        if(matches!=std::array<unsigned,2>{{1u,1u}}||actual[0]==sentinel||actual[1]==sentinel){f.failurePC=pc;return false;}
        //Both genuine authenticated anchors reach positive branch; no claim
        //that guest4342D0 retained immutable frontend state. Private native
        //startup-value domain is an explicit adaptation of3C9CCC's producer.
        f.anchorXYZ=actual;f.anchorsWritten=true;f.terminal=false;
        if(!commitNativeFinishLineGeometry1B3DDC(pc))return false;
        //1B40A4 represented native-adapted return.1AA218 optional progress
        //uses the SAME bounded native progress policy as earlier330 joins;
        //do not claim actual330/+30 read, null/default, or1CC900 execution.
        return continueNativeCollisionObjects2743B0(app,pc); //No update/draw/fullWorld.
    }catch(...){if(nativeFinishLine540_){nativeFinishLine540_->terminal=true;nativeFinishLine540_->failurePC=pc;}return false;}
}
}
