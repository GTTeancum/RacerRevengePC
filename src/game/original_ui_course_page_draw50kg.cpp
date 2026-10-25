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
bool tournamentText(const std::string& formatted,std::uint16_t offset,std::uint16_t capacity,
    std::int32_t x,std::int32_t y,assets::OriginalUiTextJustification50KG justification,
    std::uint32_t rgb,const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,float z,
    OriginalUiCoursePageTextSnapshot50KG& snapshot,OriginalUiCoursePagePacket50KG& packet,D& d){
    snapshot={};snapshot.memberOffset=offset;snapshot.capacity=capacity;
    snapshot.authoredX=x;snapshot.authoredY=y;snapshot.color54=rgb;
    snapshot.formattedByteLength=formatted.size();snapshot.text=formatted.substr(0,capacity);
    snapshot.count=static_cast<std::uint16_t>(snapshot.text.size());snapshot.packetCount=(snapshot.count+35u)/36u;
    OriginalUiTextInput50KG input;input.font=&font;input.atlas=&atlas;
    input.text=reinterpret_cast<const std::uint8_t*>(snapshot.text.data());input.length=snapshot.text.size();
    OriginalUiTextLayout50KG glyphs;
    if(!layoutOriginalUiText50KG(input,glyphs,d.text)||
       !roundOriginalUiTextMetric50KG(glyphs.widthBeforeScaleAndRounding,1,snapshot.measuredWidth)||
       !roundOriginalUiTextMetric50KG(glyphs.firstGlyphHeightBeforeScaleAndRounding,1,snapshot.measuredHeight))
        return fail(d,E::geometry,0x2E13A0);
    assets::OriginalUiOverlayText50KG position;position.x=x;position.y=y;position.justification=justification;
    position.lines={snapshot.text};OriginalUiTextPosition50KG placed;
    if(!positionOriginalUiText50KG({&position,0,glyphs.fontLineHeight,snapshot.measuredWidth,snapshot.measuredHeight},placed))
        return fail(d,E::geometry,0x2E13A0);
    snapshot.positionedX=placed.positionedX;snapshot.positionedY=placed.positionedY;
    input.localX=placed.positionedX;input.localY=placed.positionedY;
    if(!layoutOriginalUiText50KG(input,glyphs,d.text))return fail(d,E::geometry,0x2E0A30);
    packet.vertices.clear();packet.vertices.reserve(glyphs.count*4u);
    for(std::size_t i=0;i<glyphs.count;++i)for(const auto& q:glyphs.quads[i].vertices)
        packet.vertices.push_back(vertex(q.x,q.y,z,q.u,q.v));
    color(packet,rgb);return true;
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
        const auto* entrants=english.at(127);const auto* type2=english.at(131);const auto* type3=english.at(132);
        if(!entrants||!type2||!type3)return fail(d,E::localization,0x1DAB00);
        eventLabel125_=a->text;entrantsLabel127_=entrants->text;eventNames_={{b->text,type2->text,type3->text}};
        fontImage_=fontImage;packetZ_=z;lapCount_=3;
        archive_=std::move(archive);images_=std::move(images);pageIdentity_=page.identity();
        packets_=std::move(packets);texts_=std::move(texts);prepared_=true;return true;
    }catch(...){return fail(d,E::allocation,0x1D8F10);}
}
bool OriginalUiCoursePageDraw50KG::setLapCount(std::uint32_t laps,D& d)noexcept{
    return setEventSelection(eventSubtype_,laps,eventEntrants_,d);
}
bool OriginalUiCoursePageDraw50KG::setEventSelection(std::uint32_t subtype,std::uint32_t laps,std::uint32_t entrants,D& d)noexcept{
    d={};if(!prepared())return fail(d,poisoned_?E::poisoned:E::notPrepared,0x1DAF78);
    if(tournament_||subtype<1||subtype>3||laps<1||laps>25||entrants<1||entrants>8)return fail(d,E::geometry,0x1DAB00);
    if(subtype==eventSubtype_&&laps==lapCount_&&entrants==eventEntrants_)return true;
    const auto* font=images_->font(fontImage_);const auto* atlas=images_->textures()->texture(fontImage_);
    if(!font||!atlas)return fail(d,E::images,0x1DB1C8);
    try{
        auto title=packets_[0],detail=packets_[3];title.vertices.clear();detail.vertices.clear();
        OriginalUiCoursePageTextSnapshot50KG titleText,detailText;
        titleText.color54=texts_[0].color54;detailText.color54=texts_[1].color54;
        if(!embeddedText(eventLabel125_+"  "+eventNames_[subtype-1],0xAC0,101,*font,atlas->texture,packetZ_,titleText,title,d))return false;
        // Original mode0 branches: LOC126 + singular/plural laps, LOC127 +
        // entrant count, or no B30/580/5E0 submission for subtype3.
        const auto formatted=subtype==2?entrantsLabel127_+" "+std::to_string(entrants):
            lapLabel126_+"  "+std::to_string(laps)+" "+(laps==1?singularLap133_:pluralLaps134_);
        if(!embeddedText(formatted,0xB30,147,*font,atlas->texture,packetZ_,detailText,detail,d))return false;
        packets_[0]=std::move(title);packets_[3]=std::move(detail);texts_[0]=std::move(titleText);texts_[1]=std::move(detailText);
        eventSubtype_=subtype;lapCount_=laps;eventEntrants_=entrants;return true;
    }catch(...){return fail(d,E::allocation,0x1DAB00);}
}
bool OriginalUiCoursePageDraw50KG::prepareTournamentFresh(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,const assets::OriginalUiEnglish50KG& english,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t fontImage,
    const std::array<std::uint32_t,3>& medalImages,std::int32_t maximum,D& d)noexcept{
    d={};using Id=assets::OriginalUiArchiveId50KG;
    if(prepared_||poisoned_)return fail(d,E::alreadyPrepared,0x1D8F10);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(Id::course))return fail(d,E::archive,0x1D8F10);
    if(!english.loaded()||english.indexIdentity()!=assets::originalUiMemberIdentity50KG(Id::localization,1)||
       english.textIdentity()!=assets::originalUiMemberIdentity50KG(Id::localization,0))return fail(d,E::localization,0x1DB814);
    if(!images||!images->loaded()||!images->textures()||maximum!=16777215)return fail(d,E::images,0x2DEF40);
    const auto* fp=images->sourceProvenance(fontImage);const auto* font=images->font(fontImage);
    if(!fp||!font||!images->textures()->texture(fontImage)||
       fp->memberIdentity!=assets::originalUiMemberIdentity50KG(Id::fonts,17)||
       fp->celIdentity!=assets::originalUiMemberIdentity50KG(Id::fonts,16)||fp->pstOffset||fp->pstBytes!=fp->memberIdentity->bytes)
        return fail(d,E::images,0x1D9A2C);
    try{
        constexpr const char* names[]={"medlgout","medstar","medlgicn"};
        constexpr std::uint16_t offsets[]={0x640,0x7C0,0x820};
        constexpr std::uint32_t pcs[]={0x1DB970,0x1DBF18,0x1DBF9C};
        for(std::size_t i=0;i<3;++i){
            const auto m=archive->find(std::string("ui/textures/")+names[i]+(i==1?".atm":".pst"));
            const auto psm=archive->find(std::string("ui/textures/")+names[i]+".psm");
            const auto* p=images->sourceProvenance(medalImages[i]);const auto* decoded=images->decodedTexture(medalImages[i]);
            if(!m.identity||!psm.identity||!p||p->memberIdentity!=m.identity||p->celIdentity||!decoded||decoded->mips.size()!=1||
               (i!=1&&(p->pstOffset||p->pstBytes!=m.size))||(i==1&&p->pstOffset!=12))return fail(d,E::images,pcs[i]);
            auto& packet=tournamentPackets_[i];packet.materialName=i==0?"ui/textures/medlgout":i==1?"ui/textures/medstar":"ui/textures/medlgicn";
            packet.materialIdentity=psm.identity;packet.sourcePC=pcs[i];packet.pageMemberOffset=offsets[i];packet.minimumTextureResidencyPriority=10;
        }
        for(std::size_t i=0;i<tournamentLabels_.size();++i){const auto* e=english.at(122+i);if(!e)return fail(d,E::localization,0x1DBFCC);tournamentLabels_[i]=e->text;}
        // Original jump table3AF300: the planet ordinals are not LOC order.
        constexpr std::uint32_t planetIds[]{11,15,12,16,14,13};
        for(std::size_t i=0;i<planetNames_.size();++i){const auto* e=english.at(planetIds[i]);if(!e)return fail(d,E::localization,0x1DDF00);planetNames_[i]=e->text;}
        const auto fm=archive->find("font/denmrk14.psm"),top=archive->find("font/denmrk14Top.psm"),page=archive->find("UI/UICourse/track.dat");
        if(!fm.identity||!top.identity||!page.identity)return fail(d,E::material,0x1D9A2C);
        for(std::size_t i=3;i<24;++i){auto& p=tournamentPackets_[i];p.materialName=i==23?"font/denmrk14top":"font/denmrk14";p.materialIdentity=i==23?top.identity:fm.identity;}
        const auto& atlas=images->textures()->texture(fontImage)->texture;const auto z=float(maximum-1);
        using J=assets::OriginalUiTextJustification50KG;
        auto coldText=[&](std::size_t ti,std::size_t pi,std::uint16_t offset,std::uint16_t cap,int x,int y,J j,std::uint32_t rgb,const std::string& value=std::string{}){
            tournamentPackets_[pi].pageMemberOffset=offset;
            return tournamentText(value,offset,cap,x,y,j,rgb,*font,atlas,z,tournamentTexts_[ti],tournamentPackets_[pi],d);};
        if(!coldText(0,3,0xC80,40,223,90,J::left,0xCCCCCC)||!coldText(1,4,0xD60,40,293,130,J::left,0xCCCCCC)||
           !coldText(2,5,0xCF0,40,285,110,J::left,0xCCCCCC)||!coldText(3,6,0xC10,10,0,0,J::right,0xCCCCCC,tournamentLabels_[138-122])||
           !coldText(20,23,0xBA0,40,320,43,J::centerBoth,0xFFFFFF))return false;
        for(std::size_t i=0;i<8;++i)if(!coldText(4+2*i,7+2*i,std::uint16_t(0xDD0+i*0x70),10,0,0,J::centerHorizontal,0xFFFFFF)||
            !coldText(5+2*i,8+2*i,std::uint16_t(0x1150+i*0x70),15,0,0,J::centerHorizontal,0xFFFFFF))return false;
        fontImage_=fontImage;medalImages_=medalImages;packetZ_=float(maximum-1);
        archive_=std::move(archive);images_=std::move(images);pageIdentity_=page.identity;tournament_=true;prepared_=true;
        return true;
    }catch(...){return fail(d,E::allocation,0x1D8F10);}
}
bool OriginalUiCoursePageDraw50KG::setTournamentPlanetName(std::uint32_t planet,D& d)noexcept{
    d={};if(!tournamentFresh()||planet>=planetNames_.size())return fail(d,E::palette,0x1DDF00);
    try{auto& p=tournamentPackets_[23];p.pageMemberOffset=0xBA0;p.sourcePC=0x1DD530;
        if(!tournamentText(planetNames_[planet],0xBA0,40,320,43,assets::OriginalUiTextJustification50KG::centerBoth,
            secondary_,*images_->font(fontImage_),images_->textures()->texture(fontImage_)->texture,packetZ_,tournamentTexts_[20],p,d))return false;
        tournamentPlanet_=true;return true;
    }catch(...){return fail(d,E::allocation,0x1DDF00);}
}
bool OriginalUiCoursePageDraw50KG::updateTournamentFresh(OriginalUiCourseWidgets50KG& widgets,
    const OriginalUiCourseTournamentDrawState50KG& state,D& d)noexcept{
    d={};using J=assets::OriginalUiTextJustification50KG;
    constexpr std::array<std::int32_t,7> taMbPercent{{30,50,60,70,80,90,100}};
    constexpr std::array<std::int32_t,7> ordinaryPercent{{20,30,50,60,70,80,100}};
    if(!tournamentFresh()||!tournamentPlanet_||state.career>=27||state.slot<1||state.slot>13||
       (state.progress&31u)>=13u||state.slot!=(state.progress&31u)+1u||
       state.tier!=(state.slot<5?1u:state.slot<9?2u:3u)||state.medal>8||
       state.remainingPrize<0||state.par < -1||state.par>7||
       state.killPercent!=((state.slot==1||state.slot==7)?taMbPercent:ordinaryPercent))
        return fail(d,E::geometry,0x1DB814);
    const auto* nav=widgets.navigation();if(widgets.poisoned()||!nav||!nav->page||nav->page->identity!=pageIdentity_)return fail(d,E::widget,0x1DB81C);
    try{
        auto packets=tournamentPackets_;std::array<OriginalUiCoursePageTextSnapshot50KG,21> texts{};
        const auto& font=*images_->font(fontImage_);const auto& atlas=images_->textures()->texture(fontImage_)->texture;
        constexpr float xs[]={40,27,44},ys[]={100,87,104};
        for(std::size_t i=0;i<3;++i){auto& p=packets[i];const auto* image=images_->textures()->texture(medalImages_[i]);if(!image)return fail(d,E::images,p.sourcePC);
            const float x=xs[i],y=ys[i],w=float(image->texture.width),h=float(image->texture.height);
            p.vertices={vertex(x,y,packetZ_,0,0),vertex(x+w,y,packetZ_,1,0),vertex(x+w,y+h,packetZ_,1,1),vertex(x,y+h,packetZ_,0,1)};
            color(p,i==1?0xFFFFFF:primary_);
        }
        auto text=[&](std::size_t pi,std::size_t ti,const std::string& value,std::uint16_t offset,std::uint16_t cap,
            std::int32_t x,std::int32_t y,J j,std::uint32_t rgb,std::uint32_t pc){
            auto& p=packets[pi];p.sourcePC=pc;p.pageMemberOffset=offset;
            return tournamentText(value,offset,cap,x,y,j,rgb,font,atlas,packetZ_,texts[ti],p,d);
        };
        // Native beta uses the actual retained prior place on retry, not the
        // fresh-course status or an invented medal/trophy branch.
        const auto priorPlace=state.medal?"Best place: "+std::to_string(state.medal):tournamentLabels_[153-122];
        if(!text(3,0,priorPlace,0xC80,40,223,90,J::left,0xCCCCCC,0x1DC644)||
           !text(4,1,std::to_string(state.remainingPrize)+" "+tournamentLabels_[139-122],0xD60,40,293,130,J::left,0xCCCCCC,0x1DC824)||
           !text(5,2,tournamentLabels_[147-122]+" "+tournamentLabels_[144-122],0xCF0,40,285,110,J::left,0xCCCCCC,0x1DCAA8)||
           !text(6,3,state.par>=0?tournamentLabels_[138-122]:std::string{},0xC10,10,75,
               state.par>=0?225+17*state.par:225,J::right,0xCCCCCC,0x1DD30C))return false;
        // Other course PAR constructor operands remain unrecovered. -1 means
        // omit only the marker/highlight, never substitute a guessed number.
        for(std::size_t i=0;i<8;++i){const auto rgb=i==0?primary_:
            state.par>=0&&i==std::size_t(state.par)?0xCCCCCC:secondary_;
            if(!text(7+2*i,4+2*i,i?std::to_string(i):tournamentLabels_[137-122],std::uint16_t(0xDD0+i*0x70),10,100,225+int(i)*17,J::centerHorizontal,rgb,0x1DD4B8)||
               !text(8+2*i,5+2*i,i?"+"+std::to_string(state.killPercent[i-1])+"%":"% WINNINGS",std::uint16_t(0x1150+i*0x70),15,200,225+int(i)*17,J::centerHorizontal,rgb,0x1DD4D0))return false;
        }
        if(!text(23,20,tournamentLabels_[state.tier-1],0xBA0,40,320,43,J::centerBoth,secondary_,0x1DD530))return false;
        // Fallible immutable geometry preparation is separate from live widget
        // callbacks. Collector interleaves those callbacks with actual source
        // packet/material service in the order1DB834..1DD4F8.
        tournamentPackets_=std::move(packets);tournamentTexts_=std::move(texts);tournamentFrame_=true;tournamentWidgetStage_=0;return true;
    }catch(...){return fail(d,E::allocation,0x1DB81C);}
}
bool OriginalUiCoursePageDraw50KG::applyTournamentWidgetStage(std::uint8_t stage,
    OriginalUiCourseWidgets50KG& widgets,D& d)noexcept{
    d={};if(!tournamentFresh()||!tournamentFrame_||stage!=tournamentWidgetStage_||stage>4)return fail(d,E::widget,0x1DB81C);
    bool ok=false;
    if(stage==0||stage==4){OriginalUiCourseWidgetStep50KG steps[2]{};
        const auto count=stage==0?2u:1u;
        for(std::size_t i=0;i<count;++i){auto& s=steps[i];s.kind=OriginalUiCourseWidgetStepKind50KG::widget;
            s.widget=std::uint16_t(stage==0?60+i:62);s.operation=OriginalUiControlOperation50KG::setEnabled;s.value=true;
            s.sourcePC=stage==0?(i?0x1DB848:0x1DB834):0x1DD4F8;}
        ok=widgets.apply(steps,count,{},d.widget);
    }else{
        constexpr std::uint16_t ids[]{60,61,63};constexpr unsigned labels[]{141,140,142};
        try{ok=widgets.setTournamentDynamicLines(ids[stage-1],{tournamentLabels_[labels[stage-1]-122]},d.widget);}
        catch(...){return fail(d,E::allocation,0x2A9DE0);}
    }
    if(!ok){poisoned_=true;return fail(d,E::widget,stage==0?0x1DB834:stage==1?0x1DC63C:stage==2?0x1DC81C:stage==3?0x1DCA64:0x1DD4F8);}
    ++tournamentWidgetStage_;return true;
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
    primary_=primary;secondary_=secondary;
    if(!tournament_){for(auto& p:packets_)color(p,primary);for(auto& t:texts_)t.color54=primary;}
    for(std::size_t i=18;i<std::size(ids);++i)if(!widgets.setColor(ids[i],secondary,d.widget)){poisoned_=true;return fail(d,E::widget,0x1DD580);}
    // Actual query 2A6DB8 compares controller0's attached group, not selected slot.
    if(!tournament_&&nav->controllerGroup[0]==2){texts_[0].color54=0xCCCCCC;color(packets_[0],0xCCCCCC);}
    else if(!tournament_&&nav->controllerGroup[0]==3){texts_[1].color54=0xCCCCCC;color(packets_[3],0xCCCCCC);}
    return true;
}
}
