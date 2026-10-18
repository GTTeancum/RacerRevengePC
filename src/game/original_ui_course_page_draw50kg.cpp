// NEW, UNVERIFIED reconstruction after workspace rollback. Source-only WIP.
#include "original_ui_course_page_draw50kg.h"
#include "original_ui_text_position50kg.h"
#include <iterator>
#include <utility>
namespace rr::game {
namespace {
using E=OriginalUiCoursePageDrawError50KG;
using D=OriginalUiCoursePageDrawDiagnostic50KG;
bool fail(D& d,E e,std::uint32_t pc)noexcept{d.error=e;d.sourcePC=pc;return false;}
void color(OriginalUiCoursePagePacket50KG& p,std::uint32_t packed)noexcept{
    const auto c=originalUiTextPacketColor50KG(packed,255);
    for(auto& v:p.vertices)for(std::size_t j=0;j<c.size();++j)v.color[j]=c[j];
}
assets::Vertex48B vertex(float x,float y,float z,float u,float v)noexcept{
    assets::Vertex48B out{};out.position[0]=x;out.position[1]=y;out.position[2]=z;
    out.uv[0]=u;out.uv[1]=v;return out;
}
bool embeddedText(const std::string& formatted,std::uint16_t offset,std::int32_t y,
    const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,float z,
    OriginalUiCoursePageTextSnapshot50KG& snapshot,OriginalUiCoursePagePacket50KG& packet,D& d){
    snapshot.memberOffset=offset;snapshot.authoredY=y;snapshot.formattedByteLength=formatted.size();
    // Source ScreenText SetText keeps at most the allocated 60 characters.
    snapshot.text=formatted.substr(0,60);snapshot.count=static_cast<std::uint16_t>(snapshot.text.size());
    snapshot.packetCount=static_cast<std::uint16_t>((snapshot.count+35)/36);
    OriginalUiTextInput50KG input;input.font=&font;input.atlas=&atlas;
    input.text=reinterpret_cast<const std::uint8_t*>(snapshot.text.data());input.length=snapshot.text.size();
    OriginalUiTextLayout50KG measured;
    if(!layoutOriginalUiText50KG(input,measured,d.text)||
       !roundOriginalUiTextMetric50KG(measured.widthBeforeScaleAndRounding,1,snapshot.measuredWidth)||
       !roundOriginalUiTextMetric50KG(measured.firstGlyphHeightBeforeScaleAndRounding,1,snapshot.measuredHeight))
        return fail(d,E::geometry,0x2E13A0);
    // Source horizontal centered justification truncates the final float coordinate;
    // vertical justification remains top. Scale1 and angle0 are constructor defaults.
    volatile float halfWidth=static_cast<float>(snapshot.measuredWidth)*0.5f;
    volatile float centered=320.0f-halfWidth;std::int32_t centeredInteger=0;
    if(!roundOriginalUiTextMetric50KG(centered,1,centeredInteger))return fail(d,E::geometry,0x2E13A0);
    snapshot.positionedX=static_cast<float>(centeredInteger);
    snapshot.positionedY=static_cast<float>(y);input.localX=snapshot.positionedX;input.localY=snapshot.positionedY;
    OriginalUiTextLayout50KG positioned;
    if(!layoutOriginalUiText50KG(input,positioned,d.text))return fail(d,E::geometry,0x2E0A30);
    packet.vertices.reserve(static_cast<std::size_t>(positioned.count)*4);
    for(std::size_t i=0;i<positioned.count;++i)for(const auto& q:positioned.quads[i].vertices)
        packet.vertices.push_back(vertex(q.x,q.y,z,q.u,q.v));
    color(packet,snapshot.color54);return true;
}
}
bool OriginalUiCoursePageDraw50KG::prepare(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,const assets::OriginalUiEnglish50KG& english,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t fontImage,std::uint32_t overlayImage,
    std::int32_t packetZMaximum,D& d)noexcept{
    d={};if(prepared_||poisoned_)return fail(d,E::alreadyPrepared,0x1D8F10);
    using Id=assets::OriginalUiArchiveId50KG;
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(Id::course))return fail(d,E::archive,0x1D8F10);
    if(!english.loaded()||english.indexIdentity()!=assets::originalUiMemberIdentity50KG(Id::localization,1)||
       english.textIdentity()!=assets::originalUiMemberIdentity50KG(Id::localization,0))return fail(d,E::localization,0x1DAB00);
    if(!images||!images->loaded()||!images->textures()||packetZMaximum!=16777215)return fail(d,E::images,0x2DEF40);
    const auto* fp=images->sourceProvenance(fontImage);const auto* op=images->sourceProvenance(overlayImage);
    const auto* font=images->font(fontImage);const auto* fa=images->textures()->texture(fontImage);const auto* oa=images->textures()->texture(overlayImage);
    const auto* decoded=images->decodedTexture(overlayImage);
    if(!fp||!op||!font||!fa||!oa||!decoded||decoded->mips.size()!=1||
       fp->celIdentity!=assets::originalUiMemberIdentity50KG(Id::fonts,2)||fp->memberIdentity!=assets::originalUiMemberIdentity50KG(Id::fonts,3)||
       op->memberIdentity!=assets::originalUiMemberIdentity50KG(Id::controllerPopup,3)||op->celIdentity||
       fp->pstOffset||fp->pstBytes!=fp->memberIdentity->bytes||op->pstOffset||op->pstBytes!=op->memberIdentity->bytes||
       !oa->texture.width||!oa->texture.height)return fail(d,E::images,0x1DE450);
    try{
        assets::OriginalUiDat50KG ui,page;assets::OriginalUiPageIndex50KG index;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UICourse/ui.dat",ui,d.parse)||
           !assets::readOriginalUiPageIndex50KG(ui,index,d.parse)||index.pageFiles.empty()||
           !assets::readOriginalUiDat50KG(*archive,"UI/UICourse/"+index.pageFiles[0],page,d.parse))return fail(d,E::parse,0x1D8F10);
        const auto fm=archive->find("font/denmrk16.psm"),om=archive->find("ui/textures/overlay1.psm");
        if(!fm.identity||!om.identity)return fail(d,E::material,0x1D8F10);
        const auto* a=english.at(125);const auto* b=english.at(130);const auto* c=english.at(126);const auto* e=english.at(134);const auto* singular=english.at(133);
        if(!a||!b||!c||!e||!singular)return fail(d,E::localization,0x1DAB00);
        std::array<OriginalUiCoursePagePacket50KG,6> packets{};std::array<OriginalUiCoursePageTextSnapshot50KG,2> texts{};
        constexpr std::uint32_t pcs[]={0x1DAF50,0x1DAF58,0x1DAF60,0x1DB258,0x1DB260,0x1DB268};
        constexpr std::uint16_t offsets[]={0xAC0,0x4C0,0x520,0xB30,0x580,0x5E0};
        for(std::size_t i=0;i<packets.size();++i){auto& p=packets[i];const bool text=i==0||i==3;
            p.materialName=text?"font/denmrk16":"ui/textures/overlay1";p.materialIdentity=text?fm.identity:om.identity;
            p.sourcePC=pcs[i];p.pageMemberOffset=offsets[i];p.minimumTextureResidencyPriority=text?0:10;}
        const float z=static_cast<float>(packetZMaximum-1);
        if(!embeddedText(a->text+"  "+b->text,0xAC0,101,*font,fa->texture,z,texts[0],packets[0],d)||
           !embeddedText(c->text+"  3 "+e->text,0xB30,147,*font,fa->texture,z,texts[1],packets[3],d))return false;
        constexpr std::size_t pi[]={1,2,4,5};constexpr float ys[]={77,109,123,155};
        const float h=static_cast<float>(oa->texture.height);
        for(std::size_t i=0;i<std::size(pi);++i){auto& p=packets[pi[i]];const float v0=(i&1)?1:0,v1=1-v0,y=ys[i];
            p.vertices={vertex(0,y,z,0,v0),vertex(640,y,z,1,v0),vertex(640,y+h,z,1,v1),vertex(0,y+h,z,0,v1)};
            color(p,0xFFFFFF);}
        lapLabel126_=c->text;singularLap133_=singular->text;pluralLaps134_=e->text;
        fontImage_=fontImage;packetZ_=z;lapCount_=3;
        archive_=std::move(archive);images_=std::move(images);pageIdentity_=page.identity();
        packets_=std::move(packets);texts_=std::move(texts);prepared_=true;return true;
    }catch(...){return fail(d,E::allocation,0x1D8F10);}
}
bool OriginalUiCoursePageDraw50KG::setLapCount(std::uint32_t laps,D& d)noexcept{
    d={};if(!prepared())return fail(d,poisoned_?E::poisoned:E::notPrepared,0x1DAF78);
    if(laps<1||laps>25)return fail(d,E::geometry,0x1DAF78);
    if(laps==lapCount_)return true;
    const auto* font=images_->font(fontImage_);const auto* atlas=images_->textures()->texture(fontImage_);
    if(!font||!atlas)return fail(d,E::images,0x1DB1C8);
    try{
        //1DB090/1DB1B4 select the authenticated singular/plural format. Keep
        // the source60-character allocation and original layout/material owner.
        const auto formatted=lapLabel126_+"  "+std::to_string(laps)+" "+(laps==1?singularLap133_:pluralLaps134_);
        auto packet=packets_[3];packet.vertices.clear();OriginalUiCoursePageTextSnapshot50KG snapshot;
        snapshot.color54=texts_[1].color54;
        if(!embeddedText(formatted,0xB30,147,*font,atlas->texture,packetZ_,snapshot,packet,d))return false;
        packets_[3]=std::move(packet);texts_[1]=std::move(snapshot);lapCount_=laps;return true;
    }catch(...){return fail(d,E::allocation,0x1DB1C8);}
}
bool OriginalUiCoursePageDraw50KG::applyPlanetPalette(OriginalUiCourseWidgets50KG& widgets,
    std::uint32_t primary,std::uint32_t secondary,D& d)noexcept{
    d={};if(!prepared())return fail(d,poisoned_?E::poisoned:E::notPrepared,0x1DD580);
    // Retained profile's fresh-course and Ryloth planet rows, source17F800..17F884.
    if(!((primary==0xC2A684&&secondary==0x80674A)||(primary==0x8CC46E&&secondary==0x628036)||
         (primary==0xC0B5A5&&secondary==0x8C8479)||
         (primary==0xCE766D&&secondary==0xFFBAB2)||(primary==0xB26A36&&secondary==0xE5A373)))return fail(d,E::palette,0x1DD580);
    const auto* nav=widgets.navigation();
    if(widgets.poisoned()||!nav||!nav->page||nav->page->identity!=pageIdentity_)return fail(d,E::widget,0x1DD580);
    using K=OriginalUiCourseWidgetKind50KG;
    constexpr std::uint16_t ids[]={52,6,65,66,67,82,83,84,7,8,53,54,55,56,60,61,62,63,2,1};
    constexpr K kinds[]={K::text,K::icon,K::icon,K::icon,K::icon,K::icon,K::icon,K::icon,
        K::selectableIcon,K::selectableIcon,K::selectableIcon,K::selectableIcon,K::selectableIcon,K::selectableIcon,
        K::text,K::text,K::text,K::text,K::text,K::text};
    for(std::size_t i=0;i<std::size(ids);++i)if(widgets.state(ids[i]).kind!=kinds[i])return fail(d,E::widget,0x1DD580);
    for(std::size_t i=0;i<18;++i)if(!widgets.setColor(ids[i],primary,d.widget)){poisoned_=true;return fail(d,E::widget,0x1DD580);}
    for(auto& p:packets_)color(p,primary);for(auto& t:texts_)t.color54=primary;
    for(std::size_t i=18;i<std::size(ids);++i)if(!widgets.setColor(ids[i],secondary,d.widget)){poisoned_=true;return fail(d,E::widget,0x1DD580);}
    // Actual query 2A6DB8 compares controller0's attached group, not selected slot.
    if(nav->controllerGroup[0]==2){texts_[0].color54=0xCCCCCC;color(packets_[0],0xCCCCCC);}
    else if(nav->controllerGroup[0]==3){texts_[1].color54=0xCCCCCC;color(packets_[3],0xCCCCCC);}
    return true;
}
}
