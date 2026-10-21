#include "original_ui_memory_card_geometry50kg.h"
#include <cmath>
#include <utility>
namespace rr::game {
namespace {
using E=OriginalUiMemoryCardGeometryError50KG;
using D=OriginalUiMemoryCardGeometryDiagnostic50KG;
using A=assets::OriginalUiArchiveId50KG;
bool fail(D& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
float rounded(float x) noexcept {volatile float r=x;return r;}
}
bool OriginalUiMemoryCardGeometry50KG::prepare(const OriginalUiOptionsMenuWidgets50KG& bank,
    const assets::OriginalUiScreenAssets50KG& images,std::uint32_t fontImage,std::uint32_t overlayImage,D& d) noexcept {
    d={};if(prepared_)return fail(d,E::alreadyPrepared,0x1FF020);
    const auto* page=bank.memoryCardPage();
    if(!page||page->identity!=assets::originalUiMemberIdentity50KG(A::options,46)||
        page->popupWidgets!=std::array<std::int32_t,9>{{23,43,41,42,30,31,40,44,45}})
        return fail(d,E::identity,0x1D6698);
    if(!images.loaded()||!images.textures())return fail(d,E::resource,0x1FF020);
    const auto* f=images.sourceProvenance(fontImage);const auto* o=images.sourceProvenance(overlayImage);
    const auto* font=images.font(fontImage);const auto* atlas=images.textures()->texture(fontImage);
    const auto* overlay=images.textures()->texture(overlayImage);
    if(!f||f->memberIdentity!=assets::originalUiMemberIdentity50KG(A::fonts,3)||
        f->celIdentity!=assets::originalUiMemberIdentity50KG(A::fonts,2)||f->pstOffset||
        f->pstBytes!=f->memberIdentity->bytes||!o||o->memberIdentity!=assets::originalUiMemberIdentity50KG(A::controllerPopup,3)||
        o->celIdentity||o->pstOffset||o->pstBytes!=o->memberIdentity->bytes||!font||!font->decoded()||
        !atlas||!overlay||overlay->texture.width!=32||overlay->texture.height!=32)
        return fail(d,E::resource,0x1FF338);
    try {
        std::array<OriginalUiMemoryCardTemplate50KG,9> next;
        for(std::size_t i=0;i<next.size();++i){
            auto& target=next[i];target.widgetId=static_cast<std::uint16_t>(page->popupWidgets[i]);d.widgetId=target.widgetId;
            const auto* descriptor=bank.textDescriptor(target.widgetId);const auto* live=bank.textWidget(target.widgetId);
            if(!descriptor||!live||descriptor->identity!=assets::originalUiMemberIdentity50KG(A::options,1)||
                descriptor->fontName!="denmrk16"||descriptor->materialName!="denmrk16Top"||
                descriptor->textSource==assets::OriginalUiTextSource50KG::dynamic)return fail(d,E::identity,0x203820);
            for(const auto& bytes:descriptor->lines){
                if(bytes.size()>=255||bytes.find('\0')!=std::string::npos)return fail(d,E::text,0x203890);
                OriginalUiMemoryCardCloneLine50KG line;line.text=bytes;
                line.capacity5B=static_cast<std::uint8_t>(bytes.size()+1);
                line.characterCount5C=static_cast<std::uint8_t>(bytes.size());
                line.allocatedPackets5D=static_cast<std::uint8_t>((line.capacity5B+35u)/36u);
                line.color54=live->widgetColor();target.lines.push_back(std::move(line));
            }
            //203974..203994 replaces widget leading with exactly21 per line.
            // This is a private constructor parameter copy, not a fake DAT.
            auto clone=*descriptor;clone.usesExplicitHeight=true;clone.height=21;
            target.geometry=std::make_unique<OriginalUiTextGeometry50KG>();
            if(!target.geometry->prepare(clone,*font,atlas->texture,d.text))return fail(d,E::text,0x2038B0);
        }
        templates_=std::move(next);inverseWidth_=rounded(1.f/float(overlay->texture.width));
        inverseHeight_=rounded(1.f/float(overlay->texture.height));prepared_=true;d={};return true;
    }catch(...){return fail(d,E::allocation,0x20383C);}
}
bool OriginalUiMemoryCardGeometry50KG::project(const OriginalUiMemoryCardRender50KG& r,D& d) noexcept {
    d={};if(!prepared_)return fail(d,E::notPrepared,0x1FFFD0);
    if(!OriginalUiMemoryCard50KG::valid(r.state)||!r.state.entered||!r.state.dialogActive||
        !r.drawDialog||!r.drawStrips||!r.requiresDirectVideoSprite||
        r.directSetupPC!=0x200C88||r.directSpritePC!=0x200C98||r.directRestorePC!=0x200CB0||r.sourcePC!=0x1F6DE0||r.dialogSourcePC!=0x1FFFD0||
        r.drawDialogText||r.drawDialogIcons||r.directAlpha!=0x8000000009ULL||
        r.requiresDirectVideoSprite!=r.drawDialog||(r.drawStrips&&!r.drawDialog))return fail(d,E::render,0x200C5C);
    for(auto id:r.state.textIds)if(id!=-1)return fail(d,E::unsupported,0x201404);
    if(r.drawDialog&&r.state.dialogState30!=4)return fail(d,E::unsupported,0x201404);
    std::array<OriginalUiOverlayQuad50KG,3> next;
    for(std::size_t i=0;i<3;++i){const auto& s=r.strips[i];const auto& retained=r.state.strips[i];
        if(s.x!=0||s.width!=640||!std::isfinite(s.y)||!std::isfinite(s.height)||s.height<0||
            s.x!=retained.x||s.y!=retained.y||s.width!=retained.width||s.height!=retained.height)
            return fail(d,E::render,0x200194);
        auto& q=next[i];q.materialName="ui/textures/overlay1top";q.authoredDepth=1;
        q.sourcePacketColor={{121,66,13,63}};q.sourcePC=0x200CFC;
        const float u=i?rounded(32.f*inverseWidth_):1.f;
        const float v0=i==0?0.f:i==1?rounded(32.f*inverseHeight_):rounded(18.f*inverseHeight_);
        const float v1=i==1?0.f:i==0?1.f:rounded(32.f*inverseHeight_);
        const float right=rounded(s.x+s.width),bottom=rounded(s.y+s.height);
        q.vertices={{{s.x,s.y,0,v0},{right,s.y,u,v0},{s.x,bottom,0,v1},{right,bottom,u,v1}}};
    }
    strips_=std::move(next);stripCount_=r.drawStrips?3u:0u;return true;
}
}
