#include "original_ui_garage_page_owner50kg.h"
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
struct ApertureScalar2E2ED0 {
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
bool OriginalUiGaragePageOwner50KG::aperturePacketCurrent2E21D0(std::size_t ordinal)const noexcept {
    if(ordinal>=apertures_.size()||!issuer_||!apertures_[ordinal]||
       !shared_.apertures424[ordinal]||*shared_.apertures424[ordinal]!=apertures_[ordinal].get())
        return false;
    const auto& p=*apertures_[ordinal];
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
       primitive.domain!=issuer_->standardMaterialDomainOwner_||!primitive.context||
       !primitive.context->live()||primitive.context!=issuer_->materialContextOwner_)return false;
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
bool OriginalUiGaragePageOwner50KG::canMutateAperture2E21D0(std::size_t ordinal)const noexcept {
    return aperturePacketCurrent2E21D0(ordinal)&&!apertures_[ordinal]->nativeGarageFifoPins_&&
        apertures_[ordinal]->packetRevision_!=std::numeric_limits<std::uint64_t>::max();
}
bool OriginalUiGaragePageOwner50KG::borrowAperturePacket2E21D0(
    std::size_t ordinal,AperturePacketBorrow& out)const noexcept {
    if(!aperturePacketCurrent2E21D0(ordinal))return false;
    const auto& p=*apertures_[ordinal];
    out={p.primitives30[0].get(),p.packets44[0].allocation00.get(),p.packetRevision_};return true;
}
bool OriginalUiGaragePageOwner50KG::drawAperture2E21D0(
    OriginalUiGarageDrawContribution50KG& draw,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){
        d.status=status;d.sourcePC=pc;apertureDrawSourcePC_=pc;return false;};
    static std::atomic<std::uint8_t> guard8F50{0};
    static std::atomic<std::uint32_t> assertion446740{0};
    if(!guard8F50.load(std::memory_order_acquire)){
        assertion446740.store(1,std::memory_order_relaxed);guard8F50.store(1,std::memory_order_release);
    }
    OriginalUiGarageDrawContribution50KG::ApertureRenderInputs input;std::uint32_t pc=0;
    // The Draw hook validates the exact same Page1D798C ordinal and actual
    // mode22C=5 before reading the live renderer and process operands.
    if(!draw.readApertureInputs2E21D0(*this,ordinal,input,pc))
        return stop(Status::phase,pc?pc:0x2E2228u);
    apertureDrawOrdinal_=ordinal;apertureDrawPublication_=input.publication;
    apertureDrawSourcePC_=0x2E225Cu;
    if(!input.sourceApertureEnabled383680){d.sourcePC=apertureDrawSourcePC_=0x2E2408u;return true;}
    if(!canMutateAperture2E21D0(ordinal)||std::fegetround()!=FE_TONEAREST)
        return stop(Status::phase,0x2E23A8u);
    auto& p=*apertures_[ordinal];auto& descriptor=p.packets44[0];
    auto* packet=descriptor.allocation00.get();
    //2E2DD0: rewrite the actual giftag PRIM field, retaining all other bits.
    std::uint64_t giftag=std::uint64_t(packet[2].words[0])|(std::uint64_t(packet[2].words[1])<<32u);
    giftag=(giftag&0xfc007fffffffffffull)|(std::uint64_t(12)<<47u);
    packet[2].words[0]=static_cast<std::uint32_t>(giftag);
    packet[2].words[1]=static_cast<std::uint32_t>(giftag>>32u);
    ++p.packetRevision_;d.sourceEffectsCommitted=true;
    ApertureScalar2E2ED0 arithmetic;
    //2E2ED0 viewport stores precede the dirty branch, even for clean vertices.
    descriptor.tag0C->words={std::bit_cast<std::uint32_t>(arithmetic.unsignedWord(input.viewportWidth1E8)),
        std::bit_cast<std::uint32_t>(arithmetic.unsignedWord(input.viewportHeight1EC)),0x3f800000u,0};
    if(p.dirty3C){
        const auto field18=arithmetic.read(p.field18);
        //2E2FF8 is ordered LESS THAN zero, not <= zero.
        volatile float maximum=static_cast<float>(input.videoDepthMaximum60);
        const float depth=field18<0.f?-field18:arithmetic.sub(maximum,field18);
        float sine=0,cosine=0;
        const bool rotate=arithmetic.read(p.field14)!=0.f;
        if(rotate)arithmetic.sincos13BFE0(p.field14,sine,cosine);
        if(!arithmetic.okay)return stop(Status::runtimePending,0x2E3044u);
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
            if(!arithmetic.okay)return stop(Status::runtimePending,0x2E3838u);
            // Source temporary445450 has300 Vector16 objects constructed
            // at3CC2B4 through1048E0 (SQC2 vf0). Its untouched W is exactly1.
            // Native scalar lowering eliminates that temporary, not the live
            // vertex aliases or their source writes. Both aliases are checked.
            vertex.stq14[0]->words={std::bit_cast<std::uint32_t>(x),std::bit_cast<std::uint32_t>(y),
                std::bit_cast<std::uint32_t>(depth),0x3f800000u};
        }
        p.dirty3C=0; //2E3898 only after all source position writes.
    }
    if(!arithmetic.okay)return stop(Status::runtimePending,0x2E389Cu);
    apertureDrawSourcePC_=0x2E23DCu;
    if(!draw.collectAperturePrimitive246760(*this,ordinal,d.loadResources))
        return stop(Status::runtimePending,d.loadResources.sourcePC?d.loadResources.sourcePC:0x2E23DCu);
    d.sourcePC=apertureDrawSourcePC_=0x2E2408u;return true;
}
bool OriginalUiGaragePageOwner50KG::lowerAperture327E90(std::size_t ordinal,
    std::uint64_t revision,const std::array<std::uint32_t,2>& xyOffset,
    Quad::LoweredPacket327E90& out,Diagnostic& d)const noexcept {
    d={};const auto stop=[&](std::uint32_t pc){d.status=Status::runtimePending;d.sourcePC=pc;return false;};
    if(!aperturePacketCurrent2E21D0(ordinal)||!apertures_[ordinal]->nativeGarageFifoPins_||
       apertures_[ordinal]->packetRevision_!=revision||!revision||apertures_[ordinal]->dirty3C||
       std::fegetround()!=FE_TONEAREST||xyOffset[0]>65535u||xyOffset[1]>65535u)
        return stop(0x3281E8u);
    const auto* packet=apertures_[ordinal]->packets44[0].allocation00.get();
    if(packet[3].words[2]!=0x3f800000u||packet[3].words[3])return stop(0x327ED8u);
    ApertureScalar2E2ED0 arithmetic;
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
    // rectangle or replace this NoTexture Primitive with a material quad.
    constexpr std::array<std::size_t,8> order{{0,2,1,1,1,2,3,3}};
    for(std::size_t i=0;i<order.size();++i){next.vertices[i]=vertices[order[i]];
        next.stq[i]=stq[order[i]];next.rasterW[i]=1.f;next.fog[i]=0.f;}
    next.quadCount=2;out=next;d.sourcePC=0x328378u;return true;
}
}
