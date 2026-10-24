#include "original_ui_shared_dialogs50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <atomic>
#include <bit>
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::platform {
namespace {
struct WarningDimScalar2E2ED0 {
    bool okay=true;
    float read(float x)noexcept{if(!std::isfinite(x))okay=false;return x;}
    float add(float a,float b)noexcept{volatile float x=read(a)+read(b);return read(x);}
    float sub(float a,float b)noexcept{volatile float x=read(a)-read(b);return read(x);}
    float mul(float a,float b)noexcept{volatile float x=read(a)*read(b);return read(x);}
    float word(std::uint32_t x)noexcept{return std::bit_cast<float>(x);}
    float unsignedWord(std::uint32_t x)noexcept {
        if(x<=0x7fffffffu){volatile float v=static_cast<float>(static_cast<std::int32_t>(x));return read(v);}
        volatile float v=static_cast<float>(static_cast<std::int32_t>((x>>1u)|(x&1u)));
        return add(v,v);
    }
    float convertWord(float x)noexcept {
        std::int32_t value=0;
        // EE CVT.W.S truncates toward zero, independent of the host mode.
        // Reuse the existing primary-manual-backed finite signed conversion.
        if(!game::convertOriginalUiApertureCvtWS50KG(read(x),value)){
            okay=false;return 0.f;
        }
        volatile float converted=static_cast<float>(value);
        return read(converted);
    }
    void sincos13BFE0(float angle,float& sine,float& cosine)noexcept {
        float x=read(angle),sign=1.f;
        if(x>word(0x3fc90fdbu)){x=sub(word(0x40490fdbu),x);sign=-1.f;}
        else if(x<word(0xbfc90fdbu)){x=sub(word(0xc0490fdbu),x);sign=-1.f;}
        const auto square=mul(x,x);
        float c=mul(word(0xb493f27eu),square);
        c=add(word(0x37d00d01u),c);c=mul(square,c);c=add(word(0xbab60b61u),c);
        c=mul(square,c);c=add(word(0x3d2aaaabu),c);c=mul(square,c);
        c=add(-.5f,c);c=mul(square,c);c=add(1.f,c);cosine=mul(sign,c);
        float s=mul(word(0x3638ef1du),square);
        s=add(word(0xb9500d01u),s);s=mul(square,s);s=add(word(0x3c088889u),s);
        s=mul(square,s);s=add(word(0xbe2aaaabu),s);s=mul(mul(x,square),s);sine=add(x,s);
    }
};
}
bool OriginalUiSharedDialogs50KG::warning470DimPacketCurrent2E21D0()const noexcept {
    if(!current()||!inputs_.root||!dialog470_.loaded||!dialog470_.dim160)return false;
    const auto& p=*dialog470_.dim160;
    if(p.type0C!=1||p.count04!=4||p.count08!=4||p.count34!=1||p.count48!=1||
       p.capacity4C!=1||p.owns38!=1||!p.vertices00||!p.primitives30||!p.primitives30[0]||
       !p.packets44||!p.packetRevision_)return false;
    const auto& descriptor=p.packets44[0];const auto* packet=descriptor.allocation00.get();
    const auto& primitive=*p.primitives30[0];
    if(!packet||!descriptor.pointersKnown||descriptor.vif04!=packet+1||
       descriptor.gif08!=packet+2||descriptor.tag0C!=packet+3||descriptor.dmaData4!=packet+1||
       descriptor.sourcePrimitive!=12||descriptor.sourceVertices!=4||
       primitive.packet10!=packet||primitive.material00<0||!primitive.materialStored||
       !primitive.materialRetained||primitive.type0C!=15||primitive.optional04||primitive.optional14||
       (primitive.flags0F&1u)||!primitive.domain||!primitive.domain->prepared()||
       primitive.domain!=inputs_.domain||!primitive.context||
       !primitive.context->live()||primitive.context!=inputs_.context)return false;
    // Native pointer provenance, not a caller's vertex/packet projection.
    // Despite historical member names, source+14 is POSITION and+24 is STQ.
    for(std::size_t v=0;v<4;++v){const auto& vertex=p.vertices00[v];
        if(vertex.aliases10!=1||vertex.rgba1C[0]!=packet+4+3*v||
           vertex.stq14[0]!=packet+5+3*v||vertex.xyz24[0]!=packet+6+3*v||
           vertex.rgba1C[1]||vertex.stq14[1]||vertex.xyz24[1])return false;
    }
    return packet[0].words==std::array<std::uint32_t,4>{{0x30000011u,0,0,0}}&&
        packet[1].words==std::array<std::uint32_t,4>{{0x11000000u,0x03000000u,0x02000200u,0x6c0e8000u}}&&
        packet[2].words==std::array<std::uint32_t,4>{{0x8004u,0x30064000u,0x512u,0}}&&
        packet[16].words==std::array<std::uint32_t,4>{{0x140000b0u,0x10000000u,0,0}};
}
bool OriginalUiGarageDrawContribution50KG::readWarning470DimInputs2E21D0(
    const OriginalUiSharedDialogs50KG& shared,ApertureRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E21D0;
    // Exact retained470 callback/operation, not Page identity or a copied flag.
    if(!shared.warning470DimCurrent(*this))return false;
    ApertureRenderInputs next;next.publication=publication_;
    next.sourceApertureEnabled383680=issuer_->entities()->apertureRenderEnabled383680_!=0;
    // Actual383680 disabled return does not consume Camera or Video operands.
    if(!next.sourceApertureEnabled383680){out=next;pc=0;return true;}
    if(!cameraFrame_.current()||!camera_->fields_.coreKnown||!renderer_->display_->video_||
       !renderer_->display_->video_->host.current()||renderer_->display_->video_!=shared.inputs_.video)return false;
    const auto& f=camera_->fields_;if(!f.viewport1E0[2]||!f.viewport1E0[3])return false;
    next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
    next.videoDepthMaximum60=renderer_->display_->video_->packetZMaximum;out=next;pc=0;return true;
}
bool OriginalUiSharedDialogs50KG::drawWarning470Dim2E21D0(
    OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){
        d.status=status;d.sourcePC=pc;return false;};
    auto& assertion=OriginalUiGaragePageOwner50KG::polygonDraw8F50();
    if(!assertion.guard.load(std::memory_order_acquire)){
        assertion.word.store(1,std::memory_order_relaxed);assertion.guard.store(1,std::memory_order_release);
    }
    OriginalUiGarageDrawContribution50KG::ApertureRenderInputs input;std::uint32_t pc=0;
    if(!draw.readWarning470DimInputs2E21D0(*this,input,pc))
        return stop(Status::phase,pc?pc:0x2E2228u);
    if(!input.sourceApertureEnabled383680){d.sourcePC=0x2E2408u;return true;}
    if(!regenerateWarning470DimPacket2E21D0(input.viewportWidth1E8,input.viewportHeight1EC,
        input.videoDepthMaximum60,d))return false;
    const auto fifo=inputs_.root->nativeCollectorFifoOwner_;
    if(!fifo||!fifo->collectShared470Dim246760(draw,*this,d.quad.material))
        return stop(Status::resourcePending,d.quad.material.sourcePC?d.quad.material.sourcePC:0x2E23DCu);
    d.sourcePC=0x2E2408u;return true;
}
bool OriginalUiSharedDialogs50KG::regenerateWarning470DimPacket2E21D0(
    std::uint32_t width,std::uint32_t height,std::int32_t maximumDepth,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    if(!warning470DimPacketCurrent2E21D0()||dialog470_.dim160->nativeGarageFifoPins_||dialog470_.dim160->nativeMenuWarningPins_||
       dialog470_.dim160->packetRevision_==std::numeric_limits<std::uint64_t>::max()||
       std::fegetround()!=FE_TONEAREST)return stop(Status::phase,0x2E23A8u);
    auto& p=*dialog470_.dim160;auto& descriptor=p.packets44[0];
    auto* packet=descriptor.allocation00.get();
    //2E2DD0: rewrite the actual giftag PRIM field, retaining all other bits.
    std::uint64_t giftag=std::uint64_t(packet[2].words[0])|(std::uint64_t(packet[2].words[1])<<32u);
    giftag=(giftag&0xfc007fffffffffffull)|(std::uint64_t(12)<<47u);
    packet[2].words[0]=static_cast<std::uint32_t>(giftag);
    packet[2].words[1]=static_cast<std::uint32_t>(giftag>>32u);
    ++p.packetRevision_;d.sourceEffectsCommitted=true;
    WarningDimScalar2E2ED0 arithmetic;
    //2E2ED0 viewport stores precede the dirty branch, even for clean vertices.
    descriptor.tag0C->words={std::bit_cast<std::uint32_t>(arithmetic.unsignedWord(width)),
        std::bit_cast<std::uint32_t>(arithmetic.unsignedWord(height)),0x3f800000u,0};
    if(p.dirty3C){
        const auto field18=arithmetic.read(p.field18);
        //2E2FF8 is ordered LESS THAN zero, not <= zero.
        volatile float maximum=static_cast<float>(maximumDepth);
        const float depth=field18<0.f?-field18:arithmetic.sub(maximum,field18);
        float sine=0,cosine=0;
        const bool rotate=arithmetic.read(p.field14)!=0.f;
        if(rotate)arithmetic.sincos13BFE0(p.field14,sine,cosine);
        if(!arithmetic.okay)return stop(Status::resourcePending,0x2E3044u);
        for(std::size_t v=0;v<4;++v){auto& vertex=p.vertices00[v];
            float x=arithmetic.convertWord(arithmetic.mul(p.scale10,vertex.authored00[0]));
            float y=arithmetic.convertWord(arithmetic.mul(p.scale10,vertex.authored00[1]));
            vertex.authored00[2]=depth; //2E37C8, before this vertex's rotation.
            if(rotate){
                const float ox=arithmetic.convertWord(p.offset20[0]),oy=arithmetic.convertWord(p.offset20[1]);
                const float dx=arithmetic.sub(x,ox),dy=arithmetic.sub(y,oy);
                x=arithmetic.add(ox,arithmetic.sub(arithmetic.mul(dx,cosine),arithmetic.mul(dy,sine)));
                y=arithmetic.add(oy,arithmetic.add(arithmetic.mul(dx,sine),arithmetic.mul(dy,cosine)));
            }
            if(!arithmetic.okay)return stop(Status::resourcePending,0x2E3838u);
            // Source temporary445450 has300 Vector16 objects constructed
            // at3CC2B4 through1048E0 (SQC2 vf0). Its untouched W is exactly1.
            // Native scalar lowering eliminates that temporary, not the live
            // vertex aliases or their source writes. Both aliases are checked.
            vertex.stq14[0]->words={std::bit_cast<std::uint32_t>(x),std::bit_cast<std::uint32_t>(y),
                std::bit_cast<std::uint32_t>(depth),0x3f800000u};
        }
        p.dirty3C=0; //2E3898 only after all source position writes.
    }
    if(!arithmetic.okay)return stop(Status::resourcePending,0x2E389Cu);
    return true;
}
bool OriginalUiSharedDialogs50KG::lowerWarning470Dim327E90(std::uint64_t revision,const std::array<std::uint32_t,2>& xyOffset,
    Quad::LoweredPacket327E90& out,Diagnostic& d)const noexcept {
    d={};if(!dialog470_.dim160||!dialog470_.dim160->nativeGarageFifoPins_){
        d.status=Status::resourcePending;d.sourcePC=0x3281E8u;return false;
    }
    return lowerWarning470DimPacketBody327E90(revision,xyOffset,out,d);
}
bool OriginalUiSharedDialogs50KG::lowerWarning470DimPacketBody327E90(std::uint64_t revision,
    const std::array<std::uint32_t,2>& xyOffset,Quad::LoweredPacket327E90& out,Diagnostic& d)const noexcept {
    d={};const auto stop=[&](std::uint32_t pc){d.status=Status::resourcePending;d.sourcePC=pc;return false;};
    if(!warning470DimPacketCurrent2E21D0()||
       dialog470_.dim160->packetRevision_!=revision||!revision||dialog470_.dim160->dirty3C||
       std::fegetround()!=FE_TONEAREST||xyOffset[0]>65535u||xyOffset[1]>65535u)
        return stop(0x3281E8u);
    const auto* packet=dialog470_.dim160->packets44[0].allocation00.get();
    if(packet[3].words[2]!=0x3f800000u||packet[3].words[3])return stop(0x327ED8u);
    WarningDimScalar2E2ED0 arithmetic;
    const float width=arithmetic.read(arithmetic.word(packet[3].words[0]));
    const float height=arithmetic.read(arithmetic.word(packet[3].words[1]));
    if(!arithmetic.okay||width<=0||height<=0)return stop(0x327F40u);
    const std::array<float,2> half{{arithmetic.mul(width,.5f),arithmetic.mul(height,.5f)}};
    std::array<assets::Vertex48B,4> vertices{};
    std::array<std::array<float,3>,4> stq{};
    for(std::size_t i=0;i<4;++i){
        const auto& rgba=packet[4+3*i].words;const auto& xyz=packet[5+3*i].words;
        const auto& texture=packet[6+3*i].words;
        for(std::size_t lane=0;lane<4;++lane){
            if(rgba[lane]>(lane==3?128u:255u))return stop(0x3282F0u);
            vertices[i].color[lane]=static_cast<std::uint8_t>(rgba[lane]);
        }
        for(std::size_t lane=0;lane<2;++lane){
            const float gs=arithmetic.sub(arithmetic.add(arithmetic.word(xyz[lane]),2048.f),half[lane]);
            const float fixed=std::trunc(arithmetic.mul(gs,16.f));
            if(!std::isfinite(fixed)||fixed<0||fixed>65535)return stop(0x328298u);
            vertices[i].position[lane]=(fixed-static_cast<float>(xyOffset[lane]))/16.f;
        }
        const float z=std::trunc(arithmetic.word(xyz[2]));
        if(!std::isfinite(z)||z<0||z>16777215.f||xyz[3]!=0x3f800000u)return stop(0x3282A0u);
        vertices[i].position[2]=z;
        for(std::size_t lane=0;lane<3;++lane)stq[i][lane]=arithmetic.read(arithmetic.word(texture[lane]));
        if(texture[2]!=0x3f800000u||texture[3]||!arithmetic.okay)return stop(0x328200u);
        vertices[i].uv[0]=stq[i][0];vertices[i].uv[1]=stq[i][1];
    }
    Quad::LoweredPacket327E90 next;
    // Actual mode1 emits input ordinals0,2,1,3 with ADC on its first two.
    // Preserve both strip triangles and their own attributes, never invent a
    // rectangle or replace the genuine dimpopup Primitive with a Quad.
    constexpr std::array<std::size_t,8> order{{0,2,1,1,1,2,3,3}};
    for(std::size_t i=0;i<order.size();++i){next.vertices[i]=vertices[order[i]];
        next.stq[i]=stq[order[i]];next.rasterW[i]=1.f;next.fog[i]=0.f;}
    next.quadCount=2;out=next;d.sourcePC=0x328378u;return true;
}
}

namespace rr::platform {
bool OriginalUiSharedDialogs50KG::drawMenuWarning470Dim2E21D0(Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    auto& assertion=OriginalUiGaragePageOwner50KG::polygonDraw8F50();
    if(!assertion.guard.load(std::memory_order_acquire)){
        assertion.word.store(1,std::memory_order_relaxed);assertion.guard.store(1,std::memory_order_release);
    }
    MenuWarningDimInputs input;std::uint32_t pc=0;
    if(!readMenuWarningDimInputs2E21D0(input,pc))return stop(Status::phase,pc?pc:0x2E2228u);
    if(!input.sourceApertureEnabled383680){d.sourcePC=0x2E2408u;return true;}
    if(!regenerateWarning470DimPacket2E21D0(input.viewportWidth1E8,input.viewportHeight1EC,
        input.videoDepthMaximum60,d))return false;
    const bool effects=d.sourceEffectsCommitted;
    if(!collectMenuWarning470Dim(d)){d.sourceEffectsCommitted|=effects;return false;}
    d.sourceEffectsCommitted|=effects;d.sourcePC=0x2E2408u;return true;
}
bool OriginalUiSharedDialogs50KG::retainMenuWarningDimPacket(
    OriginalUiMenuWarningPacket50KG& receipt,Diagnostic& d)noexcept {
    d={};const auto reject=[&](std::uint32_t pc){d.status=Status::phase;d.sourcePC=pc;return false;};
    if(!menuWarning470PacketCaptureCurrent(receipt)||!menuWarning470DimCurrent()||
       receipt.owner_!=this||receipt.kind_!=OriginalUiMenuWarningPacket50KG::Kind::dim||receipt.active_||
       !warning470DimPacketCurrent2E21D0())return reject(0x2E23DCu);
    auto& dim=*dialog470_.dim160;
    if(dim.nativeGarageFifoPins_||dim.nativeMenuWarningPins_||dim.dirty3C||!dim.packetRevision_)
        return reject(0x2E23DCu);
    const auto& primitive=*dim.primitives30[0];
    if(primitive.material00!=3228)return reject(0x246798u);
    receipt.payload_=&dim;receipt.primitive_=&primitive;receipt.packet_=dim.packets44[0].allocation00.get();
    receipt.primitiveIndex_=0;receipt.packetRevision_=dim.packetRevision_;receipt.materialToken_=primitive.material00;
    ++dim.nativeMenuWarningPins_;receipt.active_=true;return true;
}
bool OriginalUiSharedDialogs50KG::menuWarningDimPacketMatches(
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    if(!receipt.active_||receipt.owner_!=this||receipt.kind_!=OriginalUiMenuWarningPacket50KG::Kind::dim||
       !dialog470_.dim160||receipt.payload_!=dialog470_.dim160.get())return false;
    const auto& dim=*dialog470_.dim160;
    return dim.nativeMenuWarningPins_&&receipt.packetRevision_&&receipt.packetRevision_==dim.packetRevision_&&
       receipt.primitiveIndex_==0&&dim.primitives30&&dim.primitives30[0]&&dim.packets44&&
       receipt.primitive_==dim.primitives30[0].get()&&receipt.packet_==dim.packets44[0].allocation00.get()&&
       dim.primitives30[0]->packet10==dim.packets44[0].allocation00.get()&&
       receipt.materialToken_==dim.primitives30[0]->material00;
}
bool OriginalUiSharedDialogs50KG::releaseMenuWarningDimPacket(OriginalUiMenuWarningPacket50KG& receipt)noexcept {
    // The authenticated renderer acceptance/retirement check belongs to Shared's
    // publication owner. This method only releases its exact retained payload.
    if(!menuWarningDimPacketMatches(receipt))return false;
    --dialog470_.dim160->nativeMenuWarningPins_;receipt.active_=false;return true;
}
bool OriginalUiSharedDialogs50KG::lowerMenuWarningDimPacket(const OriginalUiMenuWarningPacket50KG& receipt,
    const std::array<std::uint32_t,2>& offset,OriginalUiMenuWarningGeometry50KG& out,Diagnostic& d)const noexcept {
    d={};if(!menuWarning470PacketOffsetCurrent(receipt,offset)||!menuWarningDimPacketMatches(receipt)){
        d.status=Status::resourcePending;d.sourcePC=0x3281E8u;return false;
    }
    Quad::LoweredPacket327E90 next;
    if(!lowerWarning470DimPacketBody327E90(receipt.packetRevision_,offset,next,d))return false;
    try{
        OriginalUiMenuWarningGeometry50KG geometry;
        geometry.vertices.assign(next.vertices.begin(),next.vertices.end());geometry.stq.assign(next.stq.begin(),next.stq.end());
        geometry.rasterW.assign(next.rasterW.begin(),next.rasterW.end());geometry.fog.assign(next.fog.begin(),next.fog.end());
        geometry.quadCount=next.quadCount;out=std::move(geometry);return true;
    }catch(...){d.status=Status::allocation;d.sourcePC=0x328378u;return false;}
}
}
