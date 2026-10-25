// New reconstruction after workspace rollback; source-only and unverified.
#include "original_ui_course_selection50kg.h"
#include <string>
#include <utility>
namespace rr::game {
namespace {
using S=OriginalUiCourseSelectionStatus50KG;
bool fail(OriginalUiCourseSelectionDiagnostic50KG& d,S s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
bool equalFold(const std::string& a,const char* b)noexcept{
    std::size_t i=0;for(;i<a.size()&&b[i];++i){char c=a[i];if(c>='A'&&c<='Z')c=char(c+32);if(c!=b[i])return false;}
    return i==a.size()&&!b[i];
}
}
bool OriginalUiCourseSelectionData50KG::load(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,OriginalUiCourseSelectionDiagnostic50KG& d)noexcept{
    d={};if(archive_)return fail(d,S::alreadyLoaded,0x1DA230);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return fail(d,S::invalidArchive,0x1DA230);
    try{
        assets::OriginalUiDat50KG index,page,widgets;assets::OriginalUiPageIndex50KG pages;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UICourse/ui.dat",index,d.parse)||
           !assets::readOriginalUiPageIndex50KG(index,pages,d.parse))return fail(d,S::parseFailure,0x1CEC8C);
        if(pages.pageFiles.size()!=1||pages.pageFiles[0]!="track.dat")return fail(d,S::invalidPage,0x1D8F10);
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UICourse/track.dat",page,d.parse)||
           !assets::readOriginalUiDat50KG(*archive,"UI/UICourse/Widgets.Dat",widgets,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,S::parseFailure,0x1D8F10);
        assets::OriginalUiPage50KG parsed;if(!assets::readOriginalUiPage50KG(page,widgets,parsed,d.parse))return fail(d,S::parseFailure,0x1D63B0);
        if(parsed.pageNo!=1||parsed.groups.size()<4||parsed.groups[1].controls.empty())return fail(d,S::invalidPage,0x1DA580);
        std::array<std::int32_t,assets::originalUiControlLimit50KG> indices;indices.fill(-1);
        std::array<std::int8_t,assets::originalUiControlLimit50KG> planets;planets.fill(-1);
        constexpr const char* names[]={"tatooine","gamorr","ryloth","coruscant","sullust","mon calamari"};
        for(const auto id:parsed.groups[1].controls){
            if(id<0||std::size_t(id)>=parsed.controls.size()||std::size_t(id)>=indices.size())return fail(d,S::invalidPage,0x1DA580);
            const auto* section=page.section("Control "+std::to_string(id));
            if(!section||!assets::originalUiInteger50KG(*section,"CourseIndex",-1,indices[std::size_t(id)],d.parse,0x1DA380)||
                indices[std::size_t(id)]<0||indices[std::size_t(id)]>12)return fail(d,S::invalidSelection,0x1DA380);
            const auto* planet=section->find("Planet");if(!planet||planet->text.size()>14)return fail(d,S::invalidSelection,0x1DA398);
            for(std::int8_t n=0;n<6;++n)if(equalFold(planet->text,names[n]))planets[std::size_t(id)]=n;
            if(planets[std::size_t(id)]<0)return fail(d,S::invalidSelection,0x1DA3A8);
        }
        page_=std::move(parsed);courseIndices_=indices;planets_=planets;archive_=std::move(archive);return true;
    }catch(...){return fail(d,S::allocation,0x1DA230);}
}
bool OriginalUiCourseSelectionData50KG::control(std::uint16_t slot,OriginalUiCourseControl50KG& out)const noexcept{
    if(!archive_||!slot||page_.groups.size()<2||slot>page_.groups[1].controls.size())return false;
    const auto id=page_.groups[1].controls[slot-1];if(id<0||std::size_t(id)>=courseIndices_.size()||courseIndices_[std::size_t(id)]<0||planets_[std::size_t(id)]<0)return false;
    out={static_cast<std::uint16_t>(id),static_cast<std::uint32_t>(planets_[std::size_t(id)]),static_cast<std::uint32_t>(courseIndices_[std::size_t(id)])};return true;
}
bool OriginalUiCourseSelectionData50KG::authoredCourseIndex(std::uint16_t slot,std::uint32_t& out)const noexcept{
    OriginalUiCourseControl50KG c;if(!control(slot,c))return false;out=c.course44;return true;
}
S planOriginalUiCourseMove50KG(const OriginalUiCourseSelectionData50KG& data,
    const OriginalUiNavigation50KG& navigation,const OriginalUiCourseRememberedProfile50KG& p,
    bool forward,OriginalUiCourseMove50KG& out)noexcept{
    if(!p.ownerReady)return S::missingProfile;if(p.mode34!=0)return S::unsupportedMode;
    const auto* page=data.page();if(!page||!navigation.page||navigation.page->identity!=page->identity||
        navigation.page->groups.size()<2||navigation.controllerGroup[0]!=1||!navigation.groups[1].enabled)
        return S::invalidNavigation;
    if((p.availability11018&31)<2)return S::noChange; //1DF154/1DF40C
    OriginalUiCourseMove50KG next;next.navigation=navigation;
    if(moveOriginalUiControl50KG(next.navigation,1,forward,next.commands)!=OriginalUiNavigationStatus50KG::ok)
        return S::invalidNavigation;
    next.courseSlot14E4=next.navigation.groups[1].selectedSlot;
    if(!data.control(next.courseSlot14E4,next.selectedControl)||next.commands.count==next.commands.values.size())return S::invalidSelection;
    next.commands.values[next.commands.count++]={next.selectedControl.controlId,OriginalUiControlOperation50KG::reset,false,0x2A5A38};
    next.planetProfileChange={OriginalUiProfileField50KG::rider2Bits1125E,OriginalUiProfileWrite50KG::maskedByte,
        next.selectedControl.planet40<<5,0xe0,forward?0x1DFA44u:0x1DFC54u};
    next.sourcePC=forward?0x1DF9C0:0x1DFBD0;out=next;return S::ok;
}
S planOriginalUiCourseEntryPrefix50KG(const OriginalUiCourseSelectionData50KG& data,const OriginalUiCourseRememberedProfile50KG& p,OriginalUiCourseEntryPrefix50KG& out)noexcept{
    if(!p.ownerReady)return S::missingProfile;if(p.mode34>1)return S::unsupportedMode;
    const auto* page=data.page();if(!page||page->groups.size()<4)return S::invalidPage;
    const auto course=p.mode34==0?p.course700:p.course710;const auto subtype=p.mode34==0?p.subtype70C:1;
    if(!course||course>page->groups[1].controls.size()||course>65535||!subtype||subtype>page->groups[2].controls.size()||subtype>65535)return S::invalidSelection;
    OriginalUiCourseEntryPrefix50KG next;next.courseSlot14E4=static_cast<std::uint16_t>(course);next.subtypeSlot14F4=static_cast<std::uint16_t>(subtype);
    next.laps14EC=p.mode34==0?p.laps704:p.laps718;next.entrants14F0=p.mode34==0?p.entrants708:p.entrants714;
    if(!data.control(next.courseSlot14E4,next.selectedControl))return S::invalidSelection;
    next.planetProfileChange={OriginalUiProfileField50KG::rider2Bits1125E,OriginalUiProfileWrite50KG::maskedByte,next.selectedControl.planet40<<5,0xe0,0x1DA618};
    next.showPopup438=p.cardFlag100BC&&!p.nativeBetaStorage;out=next;return S::ok;
}
}
