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

namespace rr::game {
bool OriginalUiMemoryCardGeometry50KG::prepareLoadClones(
    const OriginalUiOptionsMenuWidgets50KG& bank,const assets::OriginalUiScreenAssets50KG& images,
    std::uint32_t fontImage,std::uint32_t crossImage,std::uint32_t triangleImage,
    std::unique_ptr<LoadCloneSet>& output,D& d) const noexcept {
    d={};if(!prepared_)return fail(d,E::notPrepared,0x201970);
    if(output)return fail(d,E::alreadyPrepared,0x201970);
    const auto* page=bank.memoryCardPage();
    if(!page||page->identity!=assets::originalUiMemberIdentity50KG(A::options,46)||
        page->popupWidgets!=std::array<std::int32_t,9>{{23,43,41,42,30,31,40,44,45}})
        return fail(d,E::identity,0x1D6698);
    if(!images.loaded()||!images.textures())return fail(d,E::resource,0x201970);
    const auto* fp=images.sourceProvenance(fontImage);const auto* font=images.font(fontImage);
    const auto* atlas=images.textures()->texture(fontImage);
    if(!fp||fp->memberIdentity!=assets::originalUiMemberIdentity50KG(A::fonts,3)||
        fp->celIdentity!=assets::originalUiMemberIdentity50KG(A::fonts,2)||fp->pstOffset||
        fp->pstBytes!=fp->memberIdentity->bytes||!font||!font->decoded()||!atlas)
        return fail(d,E::resource,0x201A88);
    std::array<const assets::Texture48B*,2> iconTextures{};
    const std::array<std::uint32_t,2> imageIds{{crossImage,triangleImage}};
    const std::array<std::size_t,2> members{{7,15}};
    for(std::size_t i=0;i<2;++i){
        const auto* provenance=images.sourceProvenance(imageIds[i]);
        const auto* texture=images.textures()->texture(imageIds[i]);
        const auto* identity=assets::originalUiMemberIdentity50KG(A::overlays,members[i]);
        // Ordinary ScreenMT bindings, NOT the same-byte CONTPOP popup owners.
        if(!identity||!provenance||provenance->memberIdentity!=identity||provenance->celIdentity||
            provenance->pstOffset||provenance->pstBytes!=identity->bytes||!texture||
            !images.decodedTexture(imageIds[i])||texture->texture.width!=32||texture->texture.height!=32)
            return fail(d,E::resource,i?0x1FF110:0x1FF058);
        iconTextures[i]=&texture->texture;
    }
    try {
        auto next=std::make_unique<LoadCloneSet>();
        // 201970 executes the two present prompts BEFORE the five body arrays.
        // The two absent extra prompts are not fabricated empty allocations.
        constexpr std::array<std::size_t,7> sourceOrder{{5,6,0,1,2,3,4}};
        for(auto i:sourceOrder){
            auto& target=next->arrays[i];target.widgetId=static_cast<std::uint16_t>(next->textIds[i]);
            d.widgetId=target.widgetId;
            const auto* descriptor=bank.textDescriptor(target.widgetId);const auto* live=bank.textWidget(target.widgetId);
            if(!descriptor||!live||descriptor->identity!=assets::originalUiMemberIdentity50KG(A::options,1)||
                descriptor->fontName!="denmrk16"||descriptor->materialName!="denmrk16Top"||
                descriptor->textSource==assets::OriginalUiTextSource50KG::dynamic)
                return fail(d,E::identity,0x2019E4);
            for(const auto& bytes:descriptor->lines){
                if(bytes.size()>=255||bytes.find('\0')!=std::string::npos)return fail(d,E::text,0x201A68);
                OriginalUiMemoryCardCloneLine50KG line;line.text=bytes;
                line.capacity5B=static_cast<std::uint8_t>(bytes.size()+1);
                line.characterCount5C=static_cast<std::uint8_t>(bytes.size());
                line.allocatedPackets5D=static_cast<std::uint8_t>((line.capacity5B+35u)/36u);
                line.color54=live->widgetColor();target.lines.push_back(std::move(line));
            }
            auto clone=*descriptor;clone.usesExplicitHeight=true;clone.height=21;
            target.geometry=std::make_unique<OriginalUiTextGeometry50KG>();
            if(!target.geometry->prepare(clone,*font,atlas->texture,d.text))return fail(d,E::text,0x201B38);
            // 201CA0/202040 and203100 use authored origin, not justified text.
            // Ack23 is processed after Yes30 and overwrites the SAME X origin.
            if(i==5||i==6||i==2){
                const std::size_t icon=i==6?1u:0u;auto& q=next->icons[icon];
                q.materialName=icon?"ui/textures/icon_tri":"ui/textures/icon_x";
                q.authoredDepth=1;q.sourcePacketColor={{127,127,127,63}};
                const float x=rounded(float(descriptor->x)-32.f),y=rounded(float(descriptor->y)-6.f);
                const float right=rounded(x+float(iconTextures[icon]->width));
                const float bottom=rounded(y+float(iconTextures[icon]->height));
                q.vertices={{{x,y,0,0},{right,y,1,0},{x,bottom,0,1},{right,bottom,1,1}}};
                // Submission PC is state-dependent and lives in LoadDrawItem.
                q.sourcePC=0;
            }
        }
        output=std::move(next);d={};return true;
    }catch(...){return fail(d,E::allocation,0x201A14);}
}
bool OriginalUiMemoryCardGeometry50KG::describeLoadDraw(const LoadCloneSet& clones,
    const OriginalUiMemoryCardState50KG& state,std::uint32_t flags,LoadDrawRecipe& output,D& d) noexcept {
    d={};
    // No operation, device, success or frame authority is inferred here. This
    // private leaf only describes an already-owned settled dialog's geometry.
    if(!state.entered||!state.dialogActive||state.completed3A4||state.closing3AC||!std::isfinite(state.timer398)||
        state.timer398>0||!std::isfinite(state.timer39C)||state.textIds!=clones.textIds||(flags&~3u))
        return fail(d,E::unsupported,0x201078);
    for(const auto& array:clones.arrays)if(!array.geometry||!array.geometry->ready())
        return fail(d,E::notPrepared,0x201078);
    LoadDrawRecipe next;
    const auto add=[&](LoadDrawKind kind,std::uint8_t index,std::uint32_t pc) noexcept {
        next.items[next.count++]={kind,index,pc};
    };
    using K=LoadDrawKind;
    switch(state.dialogState30){
    case 7:
        add(K::textArray,1,0x2010D0);
        if(flags&1u){add(K::cross,0,0x201100);add(K::textArray,5,0x201120);}
        if(flags&2u){add(K::triangle,0,0x201150);add(K::textArray,6,0x201170);}
        break;
    case 8:add(K::textArray,0,0x201090);break;
    case 13:
        add(K::textArray,3,0x201290);
        if(state.timer39C<=0&&(flags&1u)){add(K::cross,0,0x2012DC);add(K::textArray,2,0x2012FC);}
        break;
    case 14:
        add(K::textArray,4,0x201338);
        if(flags&1u){add(K::textArray,2,0x2013D4);add(K::cross,0,0x2013F4);}
        break;
    default:return fail(d,E::unsupported,0x201404);
    }
    output=next;return true;
}
}
