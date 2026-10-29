#pragma once
#include "../../src/game/original_parameter_packet_history50la.h"
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceSunFlareRetrieve2C5C3C(std::uint32_t& pc)noexcept {
    pc=0;if(!sourceWorldSunFlareRetrieveInvocationCurrent())return false;
    using History=rr::game::OriginalParameterPacketHistory50LA;
    if(!History::enter())return false;
    struct Exit {void(*leave)();~Exit(){leave();}} exit{+[](){History::leave();}};
    auto at=[&](std::uint32_t p){pc=sourceSunFlarePC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    auto& o=*nativeSunFlares_->owner;
    sourceSunFlarePhase_=SourceSunFlarePhase::retrieveEntered;
    at(0x2c5c48);o.callback74=&o;o.sourceCallbackSelfStored=true;
    if(!constructSourceSunFlareMaterial245DA0(true,pc))return stop(pc?pc:0x245da0);
    auto& m=o.material68;
    auto guard=[&](unsigned i,std::uint32_t read,std::uint32_t word,std::uint32_t byte){
        if(!History::guard(i,read,word,byte,pc)){sourceSunFlarePC_=pc;return false;}sourceSunFlarePC_=pc;return true;};
    if(!guard(0,0x134014,0x134030,0x134034))return stop(pc);
    if(!m.parameters08)return stop(0x1340ac);
    auto& p=*m.parameters08;p.sourcePacketAttempted=true;p.sourcePacketHistorySerial=History::state().serial;
    History::state().scratchSerial=p.sourcePacketHistorySerial;History::state().scratchAccessible=true;
    at(0x1340ac);p.field2C=1;
    at(0x306478);for(auto& word:p.unknown00To1C)word=0;p.sourceHeaderKnownMask=255;
    p.packetStarted=true;p.packetKind=NativeSunFlareParameters::PacketKind::retrieve;
    at(0x1340bc);p.count24=128;
    at(0x1340c4);p.packet.reset(new(std::nothrow) NativeSunFlarePacket);
    at(0x1340cc);p.buffer20=p.packet?p.packet->words.data():nullptr;p.sourceBuffer20=p.buffer20;p.sourceBufferPublished=true;
    if(!guard(1,0x1340d0,0x1340e4,0x1340e8))return stop(pc);
    if(!p.buffer20)return stop(0x13410c);
    at(0x1341d4);p.count28=0;
    at(0x1341e4);History::state().target30=0;
    if(!guard(6,0x1349a0,0x1349b8,0x1349bc))return stop(pc);
    at(0x134ac8);p.packet->words[0]=0;p.sourcePacketWordKnown[0]=true;
    at(0x134ad4);p.count28=1;
    if(!guard(3,0x13420c,0x134220,0x134224))return stop(pc);
    for(unsigned i=1;i<4;++i){at(0x1342c4);p.packet->words[i]=0;p.sourcePacketWordKnown[i]=true;}
    at(0x134300);p.unknown00To1C[0]=1;
    at(0x134308);p.unknown00To1C[0]=1;
    at(0x13430c);p.unknown00To1C[0]=0x30000001;
    at(0x134310);p.nativePayload04=p.buffer20;p.unknown00To1C[1].reset();p.sourceHeaderKnownMask&=~2u;
    at(0x134314);p.unknown00To1C[2]=0;
    at(0x13431c);p.unknown00To1C[3]=0;p.packetComplete=true;
    //299B50 binds only a private source callback record. It never executes it.
    auto& primitive=*m.primitive04;
    if(primitive.optional04)return stop(0x299b64); //Actual constructorNULL04 cohort only.
    at(0x299b78);m.sourceCallbackAttempted=true;m.sourceCallbackOwner=&o;
    primitive.optional04.reset(new(std::nothrow) NativeSunFlareCallback);
    if(primitive.optional04)primitive.optional04->sourceMode=true;
    at(0x299b80);m.sourceCallback04=primitive.optional04.get();m.sourceCallbackPublished=true;
    if(!primitive.optional04)return stop(0x299b88);
    at(0x299b88);primitive.optional04->sourceFunction=0x2c5610;primitive.optional04->sourceKnownMask|=1;
    at(0x299b90);primitive.optional04->self=&o;primitive.optional04->sourceKnownMask|=2;
    at(0x2c5c84);if(o.zones30!=10)return stop(pc);
    at(0x2c5c88);o.sourceColors44Attempted=true;o.sourceColors44Count=o.zones30;
    o.colors44.reset(new(std::nothrow) std::array<float,4>[o.zones30]);
    at(0x2c5c90);o.sourceColors44=o.colors44.get();o.sourceColors44Published=true;
    at(0x2c5c98);o.sourceColors48Attempted=true;o.sourceColors48Count=o.zones30;
    o.colors48.reset(new(std::nothrow) std::array<float,4>[o.zones30]);
    //Second array publication is the aligned allocator's actual delay store.
    at(0x2c5cac);o.sourceColors48=o.colors48.get();o.sourceColors48Published=true;
    o.sourceReadbackAttempted=true;o.buffer78.reset(new(std::nothrow) NativeSunFlareReadback);
    at(0x2c5cb0);o.sourceReadback78=o.buffer78.get();o.sourceReadbackPublished=true;
    at(0x2c5cb4);sourceSunFlarePhase_=SourceSunFlarePhase::retrieveResourcesReturned;
    //No arrays/readback contents are read by the actual depth callee. NULL
    //allocations continue; later saturation/readback will own their own cuts.
    return continueSourceSunFlareDepth2C5CF0(pc);
}
}
