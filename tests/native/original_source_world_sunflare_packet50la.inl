#pragma once
#include "../../src/game/original_parameter_packet_history50la.h"
#include <cfenv>
#include <cstring>
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceSunFlarePacket2C5AE8(std::uint32_t& pc)noexcept {
    pc=0;if(!sourceWorldSunFlarePacketInvocationCurrent()||sourceSunFlarePacketFrame_)return false;
    using History=rr::game::OriginalParameterPacketHistory50LA;
    if(!History::enter())return false;
    struct Exit {void(*leave)();~Exit(){leave();}} exit{+[](){History::leave();}};
    sourceSunFlarePacketFrame_.emplace();auto& frame=*sourceSunFlarePacketFrame_;
    sourceSunFlarePhase_=SourceSunFlarePhase::packetEntered;
    auto at=[&](std::uint32_t p){pc=sourceSunFlarePC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    auto& o=*nativeSunFlares_->owner;auto& s=*o.sprite64;auto& stack=*sourceSunFlareStack_;
    if(!o.maxScale38)return stop(0x2c5ae8);
    at(0x2e5f40);s.scale1C=*o.maxScale38;s.sourceSetterMask|=1;
    at(0x2e5f48);s.scale20=*o.maxScale38;s.sourceSetterMask|=2;
    at(0x2e5f60);s.alpha48=0;s.sourceSetterMask|=4;
    constexpr std::uint32_t alphaPC[]{0x2e5f80,0x2e5f94,0x2e5fa8,0x2e5fc0};
    for(unsigned i=0;i<4;++i){at(alphaPC[i]);s.colors38[i]&=0xffffffu;s.sourceSetterMask|=8u<<i;}
    std::int32_t maximum=0;at(0x2c5b0c);
    if(!readSourceSunFlareVideo60(maximum))return stop(pc);
    frame.video60=maximum;
    //Actual ADDIU wraps at32 bits, not host signed overflow.
    const auto depthBits=static_cast<std::uint32_t>(maximum)-10u;
    std::int32_t depth;std::memcpy(&depth,&depthBits,4);
    at(0x2e5fd4);s.depth34=depth;s.sourceSetterMask|=128;
    if(!stack.calleeColor80)return stop(0x2c5b18);
    //Narrow exact310F38 conversions of the actual authored128 scalar tuple.
    //No shared float parser or general host rounding substitute.
    if((*stack.calleeColor80)[0]!=0x43000000u||(*stack.calleeColor80)[1]!=0x43000000u||
       (*stack.calleeColor80)[2]!=0x43000000u)return stop(0x310f38);
    for(unsigned i=0;i<4;++i){at(0x2e5f24);s.colors38[i]=0xff808080u;s.sourceSetterMask|=256u<<i;}
    if(!stack.width4B8||!stack.height4BC)return stop(0x2c5c18);
    at(0x2e5f30);s.width14=static_cast<std::uint32_t>(*stack.width4B8);s.sourceSetterMask|=4096;
    at(0x2e5f38);s.height18=static_cast<std::uint32_t>(*stack.height4BC);s.sourceSetterMask|=8192;
    at(0x2e5f50);s.x0C=0;s.sourceSetterMask|=16384;
    at(0x2e5f58);s.y10=0;s.sourceSetterMask|=32768;
    //Bounded exact numeric cohort: CVT.S.W(128)*2 +0 +.5, CVT.W.S
    //under nearest yields256 (ties-even); no claim of general EE equivalence.
    at(0x2e6098);
    if(std::fegetround()!=FE_TONEAREST||s.width14!=128||s.height18!=128||s.scale1C!=2.f||s.scale20!=2.f)return stop(pc);
    at(0x2e6104);frame.rectangle50=std::array<std::int32_t,4>{{0,0,256,256}};
    std::array<std::int32_t,4> viewport;at(0x2e6114);
    if(!readSourceSunFlareViewport115990(viewport))return stop(pc);
    frame.viewport60=viewport;
    if(viewport!=std::array<std::int32_t,4>{{0,0,640,448}})return stop(0x2e6138);
    //Every intermediate is exactly representable: (2048-.5*extent-origin)*16.
    constexpr std::uint32_t left=27648,top=29184,right=31744,bottom=33280;
    at(0x2e6210);frame.rectangle70=std::array<std::int32_t,4>{{left,top,right,bottom}};
    auto guard=[&](unsigned i,std::uint32_t read,std::uint32_t word,std::uint32_t byte){
        if(!History::guard(i,read,word,byte,pc)){sourceSunFlarePC_=pc;return false;}sourceSunFlarePC_=pc;return true;};
    //Actual134000 guard is reached even for NULL parameters, before first store.
    if(!guard(0,0x134014,0x134030,0x134034))return stop(pc);
    auto* parameter=s.material.parameters08.get();
    if(!parameter)return stop(0x1340ac);
    auto& p=*parameter;p.sourcePacketAttempted=true;
    p.sourcePacketHistorySerial=History::state().serial;
    History::state().scratchSerial=p.sourcePacketHistorySerial;
    History::state().scratchAccessible=true;
    at(0x1340ac);p.field2C=1;
    at(0x306478);for(auto& word:p.unknown00To1C)word=0;p.sourceHeaderKnownMask=255;
    p.packetStarted=true;p.packetKind=NativeSunFlareParameters::PacketKind::sprite;
    if(p.buffer20||p.packet)return stop(0x1340b0); //This first-use cohort has constructorNULL20.
    at(0x1340bc);p.count24=128;
    at(0x1340c4);p.packet.reset(new(std::nothrow) NativeSunFlarePacket);
    at(0x1340cc);p.buffer20=p.packet?p.packet->words.data():nullptr;p.sourceBuffer20=p.buffer20;p.sourceBufferPublished=true;
    if(!guard(1,0x1340d0,0x1340e4,0x1340e8))return stop(pc);
    //Missing buffer needs actual133660(false)/debug policy, not fabricated success.
    if(!p.buffer20)return stop(0x13410c);
    at(0x1341d4);p.count28=0;
    auto& history=History::state();at(0x1341e4);history.target30=0;
    auto reserve=[&](unsigned count,bool scalar){
        if(!history.scratchAccessible||history.scratchSerial!=p.sourcePacketHistorySerial||
           history.serial!=p.sourcePacketHistorySerial){pc=scalar?0x1349a0:0x134860;return false;}
        if(!guard(6,scalar?0x1349a0:0x134860,scalar?0x1349b8:0x134878,scalar?0x1349bc:0x13487c))return false;
        if(p.count28>p.count24)return false; //Unjoined assertion-false branch.
        if(count>p.count24-p.count28){pc=scalar?0x134a30:0x1348f0;return false;}
        return true;
    };
    auto put=[&](std::uint32_t word){
        if(!reserve(1,true))return false;
        at(0x134ac8);p.packet->words[p.count28]=word;p.sourcePacketWordKnown[p.count28]=true;
        at(0x134ad4);++p.count28;return true;
    };
    auto quad=[&](const std::array<std::uint32_t,4>& words,bool tag=false){
        if(tag)frame.tag80=words;else {frame.command40=words;frame.commandKnown=true;}
        if(!reserve(4,false))return false;
        at(tag?0x134518:0x1344a8);history.words38+=4;
        at(0x3062c0);for(unsigned i=0;i<4;++i){p.packet->words[p.count28+i]=words[i];p.sourcePacketWordKnown[p.count28+i]=true;}
        at(tag?0x134538:0x1344c8);p.count28+=4;return true;
    };
    if(!put(0x11000000)||!put(0x03000000)||!put(0x02000000))return stop(pc);
    at(0x1345e0);history.format40=12;
    at(0x1345e8);history.words38=0;
    at(0x134600);history.placeholder28=p.count28;
    at(0x134614);history.target30=800;
    if(!reserve(1,false))return stop(pc);
    at(0x13462c);++p.count28; //Reserved word remains UNKNOWN until134818.
    if(!quad({{0x8001,0x80000000,0xeeeeeeee,0x00eeeeee}},true)||
       !quad({{0,0,0x3f,0}})||!quad({{0x11e,0,0,0}}))return stop(pc);
    if(s.u24!=0.f||s.u2C!=0.f)return stop(0x2e63fc);
    if(!quad({{0x00100010,0,3,0}})||!quad({{0x00808080,0x3f800000,1,0}})||
       !quad({{left|(top<<16),static_cast<std::uint32_t>(s.depth34),5,0}}))return stop(pc);
    if(s.v28!=1.f||s.v30!=1.f)return stop(0x2e6510);
    if(!quad({{0x08000800,0,3,0}})||!quad({{0x00808080,0x3f800000,1,0}})||
       !quad({{right|(bottom<<16),static_cast<std::uint32_t>(s.depth34),5,0}}))return stop(pc);
    if(!guard(4,0x134660,0x134674,0x134678))return stop(pc);
    if(!history.words38)return stop(0x1346a0);
    const auto qwords=(history.words38+3u)/4u;
    if(!guard(5,0x1346ec,0x134700,0x134704))return stop(pc);
    if(qwords>256)return stop(0x134724);
    at(0x134818);p.packet->words[history.placeholder28]=((history.format40|0x60u)<<24)|((qwords&255u)<<16)|(history.target30|0x8000u);
    p.sourcePacketWordKnown[history.placeholder28]=true;
    at(0x13482c);history.target30+=qwords;
    if(!put(0x14000000))return stop(pc);
    if(!guard(3,0x13420c,0x134220,0x134224))return stop(pc);
    if(p.count28>p.count24)return stop(0x134250);
    for(std::uint32_t i=p.count28;i%4u;++i){at(0x1342c4);p.packet->words[i]=0;p.sourcePacketWordKnown[i]=true;}
    at(0x134300);p.unknown00To1C[0]=(p.count28+3u)/4u;
    at(0x134308);p.unknown00To1C[0]=*p.unknown00To1C[0]&0xff00ffffu;
    at(0x13430c);p.unknown00To1C[0]=*p.unknown00To1C[0]|0x30000000u;
    at(0x134310);p.nativePayload04=p.buffer20;p.unknown00To1C[1].reset();p.sourceHeaderKnownMask&=~2u;
    at(0x134314);p.unknown00To1C[2]=0;
    at(0x13431c);p.unknown00To1C[3]=0;p.packetComplete=true;
    //Actual a1=0 skips2F5E50. No material/texture/GPU consumer is invoked.
    at(0x2c5c3c);sourceSunFlarePhase_=SourceSunFlarePhase::packetReturned;return true;
}
}
