#include "original_ui_controller_popup_geometry50kg.h"
#include "original_ui_text_position50kg.h"
#include <cmath>
#include <utility>

namespace rr::game {
namespace {
using Error=OriginalUiControllerPopupGeometryError50KG;
using Diagnostic=OriginalUiControllerPopupGeometryDiagnostic50KG;
using Archive=assets::OriginalUiArchiveId50KG;
using Material=OriginalUiControllerPopupMaterial50KG;
bool fail(Diagnostic& d,Error error,std::uint32_t pc,std::uint16_t offset=0) noexcept {
    d.error=error;d.sourcePC=pc;d.objectOffset=offset;return false;
}
float rounded(float value) noexcept {volatile float result=value;return result;}
bool image(const assets::OriginalUiScreenAssets50KG& images,std::uint32_t index,
    Archive archive,std::size_t member,const assets::Texture48B*& result,
    const assets::OriginalUiMemberIdentity50KG* cel=nullptr) noexcept {
    const auto* identity=assets::originalUiMemberIdentity50KG(archive,member);
    const auto* provenance=images.sourceProvenance(index);
    const auto* texture=images.textures()->texture(index);
    if(!identity||!provenance||provenance->memberIdentity!=identity||provenance->celIdentity!=cel||
        provenance->pstOffset||provenance->pstBytes!=identity->bytes||!texture||
        !images.decodedTexture(index)||!texture->texture.width||!texture->texture.height)return false;
    result=&texture->texture;return true;
}
bool text(OriginalUiControllerPopupText50KG& owner,const std::string& bytes,
    std::uint8_t capacity,std::uint16_t offset,float x,float y,std::uint32_t justification,
    const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,Diagnostic& d) {
    if(bytes.size()>capacity||bytes.find('\0')!=std::string::npos)
        return fail(d,Error::textCapacity,0x2E08D0,offset);
    owner.text=bytes;owner.capacity5B=capacity;owner.characterCount5C=static_cast<std::uint8_t>(bytes.size());
    owner.allocatedPackets5D=static_cast<std::uint8_t>((capacity+35u)/36u);
    // 2DFF28/38 copies the allocated count. 2E0880 derives the active count
    // only when this particular ScreenText is actually rendered.
    owner.activePackets5E=owner.allocatedPackets5D;
    owner.objectOffset=offset;owner.authoredX=x;owner.authoredY=y;owner.justification50=justification;
    OriginalUiTextInput50KG input{&font,&atlas,reinterpret_cast<const std::uint8_t*>(owner.text.data()),owner.text.size(),0,0};
    OriginalUiTextLayout50KG measured;
    if(!layoutOriginalUiText50KG(input,measured,d.text)||
        !roundOriginalUiTextMetric50KG(measured.widthBeforeScaleAndRounding,1,owner.measuredWidth)||
        !roundOriginalUiTextMetric50KG(measured.firstGlyphHeightBeforeScaleAndRounding,1,owner.measuredHeight))
        return fail(d,Error::geometry,0x2E13A0,offset);
    owner.positionedX=x;owner.positionedY=y;
    if(justification==3&&!bytes.empty()) {
        // Original 2E1504..2E1530: integer GetWidth/GetHeight first, then
        // binary32 half/subtract and CVT.W.S toward zero independently.
        std::int32_t px=0,py=0;
        if(!roundOriginalUiTextMetric50KG(rounded(x-rounded(float(owner.measuredWidth)/2.f)),1,px)||
            !roundOriginalUiTextMetric50KG(rounded(y-rounded(float(owner.measuredHeight)/2.f)),1,py))
            return fail(d,Error::geometry,0x2E1504,offset);
        owner.positionedX=float(px);owner.positionedY=float(py);
    }
    input.localX=owner.positionedX;input.localY=owner.positionedY;
    if(!layoutOriginalUiText50KG(input,owner.glyphs,d.text))return fail(d,Error::geometry,0x2E0A30,offset);
    return true;
}
OriginalUiOverlayQuad50KG quad(const char* name,float x,float y,float width,float height,
    float u0,float v0,float u1,float v1,std::array<std::uint8_t,4> color,std::uint32_t pc) noexcept {
    OriginalUiOverlayQuad50KG result;result.materialName=name;result.sourcePC=pc;
    result.authoredDepth=1;result.sourcePacketColor=color;
    const float right=rounded(x+rounded(width*1.f)),bottom=rounded(y+rounded(height*1.f));
    result.vertices={{{x,y,u0,v0},{right,y,u1,v0},{x,bottom,u0,v1},{right,bottom,u1,v1}}};
    return result;
}
}
bool OriginalUiControllerPopupGeometry50KG::prepare(const assets::OriginalUiEnglish50KG& english,
    const assets::OriginalUiScreenAssets50KG& images,std::uint32_t fontImage,
    std::uint32_t iconXImage,std::uint32_t iconTriImage,Diagnostic& d) noexcept {
    d={};if(prepared_)return fail(d,Error::alreadyPrepared,0x2055B0);
    if(!english.loaded()||english.textIdentity()!=assets::originalUiMemberIdentity50KG(Archive::localization,0)||
        english.indexIdentity()!=assets::originalUiMemberIdentity50KG(Archive::localization,1))
        return fail(d,Error::localization,0x1F97E4);
    const auto* message=english.at(OriginalUiControllerPopup50KG::controller2TextId);
    const auto* confirm=english.at(OriginalUiControllerPopup50KG::confirmTextId);
    if(!message||!confirm||message->text.empty()||confirm->text.empty())return fail(d,Error::localization,0x1F97E4);
    if(!images.loaded()||!images.textures())return fail(d,Error::resource,0x2057E8);
    const assets::Texture48B *atlas=nullptr,*cross=nullptr,*triangle=nullptr,*overlay=nullptr;
    const auto* cel=assets::originalUiMemberIdentity50KG(Archive::fonts,6);
    const auto* font=images.font(fontImage);
    if(!cel||!font||!font->decoded()||!image(images,fontImage,Archive::fonts,7,atlas,cel)||
        !image(images,iconXImage,Archive::controllerPopup,6,cross)||
        !image(images,iconTriImage,Archive::controllerPopup,8,triangle))return fail(d,Error::resource,0x2057E8);
    // The API receives the three caller-selected bindings. The strips share
    // this same immutable bank's exact CONTPOP overlay1 PST, never an arbitrary
    // same-sized texture or the Options material's independently sourced image.
    for(std::uint32_t i=0;i<images.textures()->textureCount();++i) {
        const auto* provenance=images.sourceProvenance(i);
        if(provenance&&provenance->memberIdentity==assets::originalUiMemberIdentity50KG(Archive::controllerPopup,3)) {
            if(overlay||!image(images,i,Archive::controllerPopup,3,overlay))return fail(d,Error::resource,0x205B4C);
        }
    }
    if(!overlay)return fail(d,Error::resource,0x205B4C);
    try {
        std::array<OriginalUiControllerPopupText50KG,3> next{};
        // 382E80 -> string3B0548 "venus14"; material3B0550 "venus14popup".
        // Three genuine ScreenText allocations, not fabricated Widgets DAT.
        // 205810/205968/205A40 pass capacities60/40/40 to2DFD90.
        if(!text(next[0],message->text,60,0x270,320,204,3,*font,*atlas,d)||
            !text(next[1],confirm->text,40,0x2E0,312,257,0,*font,*atlas,d)||
            !text(next[2],std::string{},40,0x350,0,0,0,*font,*atlas,d))return false;
        texts_=std::move(next);
        crossWidth_=float(cross->width);crossHeight_=float(cross->height);
        triangleWidth_=float(triangle->width);triangleHeight_=float(triangle->height);
        overlayReciprocalWidth_=rounded(1.f/float(overlay->width));
        overlayReciprocalHeight_=rounded(1.f/float(overlay->height));
        prepared_=true;return true;
    } catch(...) {return fail(d,Error::allocation,0x2055B0);}
}
bool OriginalUiControllerPopupGeometry50KG::project(const OriginalUiControllerPopupRender50KG& render,Diagnostic& d) noexcept {
    d={};if(!prepared_)return fail(d,Error::notPrepared,0x2064C0);
    // Only Options Controller2's authentic message88 / OK90 route is admitted.
    // Null cancel does not clear stale source flags; refuse that unowned branch
    // rather than inventing a Cancel label or silently dropping its icon.
    if(render.drawCancel)return fail(d,Error::unsupportedRoute,0x2072BC,0x350);
    if(render.sourcePC!=0x2064C0||!OriginalUiControllerPopup50KG::valid(render.state)||
        render.drawDim!=render.drawStrips||(render.drawMessage&&!render.drawDim)||
        (render.drawConfirm&&(!render.drawMessage||!render.state.hasConfirm3D0)))
        return fail(d,Error::invalidRender,0x2064C0);
    for(std::size_t i=0;i<render.strips.size();++i) {
        const auto& a=render.strips[i];const auto& b=render.state.strips[i];
        if(a.x!=0||a.width!=640||!std::isfinite(a.y)||!std::isfinite(a.height)||a.height<0||
            a.x!=b.x||a.y!=b.y||a.width!=b.width||a.height!=b.height)
            return fail(d,Error::invalidRender,0x206538,static_cast<std::uint16_t>(0x40+0x60*i));
    }
    std::array<OriginalUiOverlayQuad50KG,6> next{};
    // 205764 writes alpha79 directly; 2057AC calls133F20(0x191919), whose
    // byte>>1 conversion yields12/12/12. Do not convert alpha79 a second time.
    next[0]=quad("ui/textures/dimpopup",0,0,640,448,0,0,0,0,{{12,12,12,79}},0x206DCC);
    next[1]=quad("ui/textures/icon_x_popup",280,247,crossWidth_,crossHeight_,0,0,1,1,{{127,127,127,63}},0x2072AC);
    next[2]=quad("ui/textures/icon_tri_popup",280,247,triangleWidth_,triangleHeight_,0,0,1,1,{{127,127,127,63}},0x2072C8);
    const float u32=rounded(32.f*overlayReciprocalWidth_),v32=rounded(32.f*overlayReciprocalHeight_);
    const float v18=rounded(18.f*overlayReciprocalHeight_);
    for(std::size_t i=0;i<3;++i) {
        const auto& s=render.strips[i];
        // Top retains ScreenTexture's complete0..1 map. Bottom205DA0 reverses
        // source rows32..0; middle205F80 selects source rows18..32.
        next[3+i]=quad("ui/textures/overlay1popup",s.x,s.y,s.width,s.height,
            0,i==0?0:i==1?v32:v18,i==0?1:u32,i==1?0:i==0?1:v32,
            {{121,66,13,63}},0x2072F4);
    }
    quads_=next;packets_={};packetCount_=0;
    // Primitive2E1948, ScreenTexture2DEFA4 and ScreenText2E00EC all default
    // to depth1.2E3020/2DF3F4/2E1110 subtract it from actual Video+60.
    // The surrounding retained owner must prove the documented video contract.
    constexpr float packetDepth=16777214.f;
    const auto addQuad=[&](std::size_t q,Material material,std::uint16_t offset) {
        auto& p=packets_[packetCount_++];p.quad=&quads_[q];p.packetColor=quads_[q].sourcePacketColor;
        p.material=material;p.sourcePC=quads_[q].sourcePC;p.objectOffset=offset;p.packetDepth=packetDepth;
    };
    const auto addText=[&](std::size_t index,std::uint32_t pc) {
        auto& textOwner=texts_[index];textOwner.activePackets5E=static_cast<std::uint8_t>((textOwner.characterCount5C+35u)/36u);
        auto& p=packets_[packetCount_++];p.text=&textOwner.glyphs;
        p.packetColor=originalUiTextPacketColor50KG(textOwner.color54,textOwner.alpha58);
        p.material=Material::font;p.sourcePC=pc;p.objectOffset=textOwner.objectOffset;p.packetDepth=packetDepth;
    };
    // Exact2064C0 call order, including zero-area closing strips.
    if(render.drawDim)addQuad(0,Material::dim,0x220);
    if(render.drawMessage)addText(0,0x206DFC);
    if(render.drawConfirm){addQuad(1,Material::cross,0x160);addText(1,0x2072B4);}
    if(render.drawStrips)for(std::size_t i=0;i<3;++i)addQuad(3+i,Material::overlay,static_cast<std::uint16_t>(0x40+0x60*i));
    return true;
}
}
