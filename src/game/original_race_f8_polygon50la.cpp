#include "original_race_f8_storage50la.h"
#include "original_camera_start_effects.h"
#include "original_ui_aperture_owner50kg.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
bool OriginalRaceF850LA::Impl::polygonCurrent(std::size_t i)const noexcept {
    if(i>=polygons30.size()||!polygons30[i])return false;
    const auto& p=*polygons30[i];
    if(p.type0C!=1||p.count04!=4||p.count08!=4||p.count34!=1||p.count48!=1||
       p.capacity4C!=1||p.owns38!=1||!p.vertices00||!p.packets44||
       !p.primitives30||!p.primitives30[0]||!p.packetRevision)return false;
    const auto& descriptor=p.packets44[0];const auto* packet=descriptor.allocation00.get();
    const auto& primitive=*p.primitives30[0];const auto& ref=references[i];
    if(!packet||!descriptor.pointersKnown||descriptor.field10||
       descriptor.vif04!=&packet->vif10||descriptor.gif08!=&packet->gif20||
       descriptor.tag0C!=&packet->viewport30||descriptor.dmaData4!=&packet->vif10||
       primitive.packet10!=packet||primitive.reference!=i||!ref.started||!ref.retained||
       primitive.material00!=ref.row||ref.row<0||!primitive.materialStored||!primitive.materialRetained||
       primitive.optional04||primitive.optional14||primitive.type0C!=15||primitive.field0D||
       primitive.field0E!=255||primitive.flags0F||primitive.scale18!=1.f||primitive.scale1C!=1.f||
       primitive.field20||primitive.field22)return false;
    for(std::size_t v=0;v<4;++v){const auto& vertex=p.vertices00[v];const auto& target=packet->vertices[v];
        if(vertex.aliases10!=1||vertex.stq14!=&target.stq||vertex.rgba1C!=&target.rgba||
           vertex.xyz24!=&target.xyz||vertex.secondary18||vertex.secondaryColor20||vertex.secondaryTexture28)return false;
    }
    return packet->dma00==PolygonStorage::Qword{{0x30000011u,0,0,0}}&&
        packet->vif10==PolygonStorage::Qword{{0x11000000u,0x03000000u,0x02000200u,0x6c0e8000u}}&&
        packet->gif20==PolygonStorage::Qword{{0x8004u,0x30064000u,0x512u,0}}&&
        packet->end100==PolygonStorage::Qword{{0x140000b0u,0x10000000u,0,0}};
}

bool OriginalRaceF850LA::preparePolygon2E2DD0(std::size_t ordinal,
    const OriginalCameraViewportFields& actualCamera,std::uint32_t actualVideoMaximum60,
    OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};d.object=0x30+4*ordinal;
    if(!loadedCold()||impl_->nativeBarBorrows||!impl_->currentLeaf()||!impl_->polygonCurrent(ordinal)){
        d.sourcePC=0x2e23a8;d.operation="F8 actual retained type1 packet required";return false;
    }
    auto& owner=*impl_;auto& p=*owner.polygons30[ordinal];
    if(p.nativePacketPins||p.packetRevision==std::numeric_limits<std::uint64_t>::max()||
       p.scale10!=1.f||p.field14!=0.f||p.field18!=10.f||p.flags40!=3||
       !actualCamera.prepared||!actualCamera.sceneKey||
       actualCamera.sessionGeneration!=owner.component.sessionGeneration||
       actualCamera.width1E8<=0||actualCamera.height1EC<=0||
       actualCamera.width1E8>16777216||actualCamera.height1EC>16777216||
       actualVideoMaximum60!=owner.depth||actualVideoMaximum60!=0xffffffu)
        return owner.stop(d,0x2e2ed0,"F8 bounded actual Camera/Video and mutable source packet");
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24,"F8 native binary32 packet geometry");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return owner.stop(d,0x2e2ed0,"F8 packet geometry requires strict native floating point");
#else
    if(std::fegetround()!=FE_TONEAREST)
        return owner.stop(d,0x2e2ed0,"F8 packet geometry rounding domain");
    auto& packet=*p.packets44[0].allocation00;
    // Exact type1 2E2DD0 PRIM replacement; all other GIF tag bits survive.
    std::uint64_t gif=std::uint64_t(packet.gif20[0])|(std::uint64_t(packet.gif20[1])<<32u);
    gif=(gif&0xfc007fffffffffffull)|(std::uint64_t{12}<<47u);
    packet.gif20[0]=std::uint32_t(gif);packet.gif20[1]=std::uint32_t(gif>>32u);
    ++p.packetRevision;d.sourceEffectsCommitted=true;
    //2E2F94..FCC always precedes the dirty branch, including a clean packet.
    // Positive exact-conversion subset of original unsigned width/height.
    volatile float width=static_cast<float>(actualCamera.width1E8);
    volatile float height=static_cast<float>(actualCamera.height1EC);
    packet.viewport30[0]=width;packet.viewport30[1]=height;
    packet.viewport30[2]=1.f;packet.viewport30[3]=0.f;
    if(p.dirty3C){
        // F8's real1986F4 field18=10, NOT Pause's depth18=1. This branch's
        // exact maximum60 is16777215; no negative-depth/rotation substitution.
        volatile float maximum=static_cast<float>(actualVideoMaximum60);
        volatile float depth=maximum-p.field18;
        const auto convert=[](float input,float scale,float& result)noexcept {
            if(!std::isfinite(input)||(input!=0.f&&std::fpclassify(input)!=FP_NORMAL))return false;
            volatile float scaled=scale*input;std::int32_t integer=0;
            if(!convertOriginalUiApertureCvtWS50KG(scaled,integer)||
               integer < -16777216||integer > 16777216)return false;
            volatile float converted=static_cast<float>(integer);result=converted;return true;
        };
        for(std::size_t i=0;i<4;++i){auto& vertex=p.vertices00[i];float x=0.f,y=0.f;
            if(!convert(vertex.authored00[0],p.scale10,x))
                return owner.stop(d,0x2e3794,"F8 finite signed CVT.W.S X domain");
            if(!convert(vertex.authored00[1],p.scale10,y))
                return owner.stop(d,0x2e37b4,"F8 finite signed CVT.W.S Y domain");
            vertex.authored00[2]=depth; //2E37C8 before copying the source vector.
            // Eliminate only the source445450 temporary. Its Vector16 W was
            // initialized to1 at3CC2B4->1048E0; it is not authored00.W or0.
            *vertex.stq14={{x,y,depth,1.f}};
        }
        p.dirty3C=0; //2E3898 only after every source position copy.
    }
    // CPU prerequisite completed, not2E21D0/246760 or199500. No source
    // assertion/gate, material service, FIFO append, GPU or child call occurred.
    d.sourcePC=0x2e389c;return true;
#endif
}
}
