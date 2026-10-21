#include "original_ui_exit_text50kg.h"
#include "original_ui_text_position50kg.h"
#include <new>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiExitTextStatus50KG;
using Archive=assets::OriginalUiArchiveId50KG;
Status fail(OriginalUiExitTextDiagnostic50KG& d,Status status,std::uint32_t pc=0) noexcept {
    d.status=status;d.sourcePC=pc;return status;
}
bool pageSource(OriginalUiTransitionRoute50KG route,Archive& id,const char*& directory) noexcept {
    if(route.screen==9 && route.page==1){id=Archive::tournament;directory="UI/UITourn/";return true;}
    if(route.screen==11 && route.page==1){id=Archive::hallOfFame;directory="UI/UIHOF/";return true;}
    if(route.screen==4 && route.page==1){id=Archive::mainMenu;directory="UI/UIMain/";return true;}
    if(route.screen==12 && (route.page>=1 && route.page<=6)){id=Archive::options;directory="UI/UIOption/";return true;}
    if(route.screen==5 && (route.page==1 || route.page==2)){id=Archive::rider;directory="UI/UIRider/";return true;}
    if(route.screen==6 && route.page==1){id=Archive::course;directory="UI/UICourse/";return true;}
    return false;
}
}
Status OriginalUiExitText50KG::prepare(OriginalUiTransitionRoute50KG route,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG& english,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t imageIndex,
    OriginalUiExitTextDiagnostic50KG& d) noexcept {
    d={};if(prepared())return fail(d,Status::alreadyPrepared);
    Archive id=Archive::mainMenu;const char* directory=nullptr;
    if(!pageSource(route,id,directory))return fail(d,Status::unsupportedPage);
    if(!archive || !archive->loaded() || archive->identity()!=assets::originalUiArchiveIdentity50KG(id))
        return fail(d,Status::archive);
    // Localization catalog ordinals0/1 are original English BIN/LOC. The
    // loaded type can only acquire them by authentic source member parsing.
    if(!english.loaded() ||
        english.textIdentity()!=assets::originalUiMemberIdentity50KG(Archive::localization,0) ||
        english.indexIdentity()!=assets::originalUiMemberIdentity50KG(Archive::localization,1))
        return fail(d,Status::localization,0x1D6BA8);
    const auto* entry=english.at(89);
    if(!entry || entry->text.empty() || entry->text.size()>10)
        return fail(d,Status::textCapacity,0x2E08D0);
    if(!images || !images->loaded() || !images->textures())return fail(d,Status::resource);
    const auto* source=images->sourceProvenance(imageIndex);
    // UIFONTS original ordinals2/3 are denmrk16 CEL/PST. Dimensions or a
    // caller-created image identity alone cannot establish this association.
    const auto* cel=assets::originalUiMemberIdentity50KG(Archive::fonts,2);
    const auto* pst=assets::originalUiMemberIdentity50KG(Archive::fonts,3);
    const auto* font=images->font(imageIndex);
    const auto* atlas=images->textures()->texture(imageIndex);
    if(!source || !pst || !cel || source->memberIdentity!=pst || source->celIdentity!=cel ||
        source->pstOffset || source->pstBytes!=pst->bytes || !font || !atlas)
        return fail(d,Status::resource,0x1D6AFC);
    try {
        assets::OriginalUiDat50KG indexDat,pageDat;
        assets::OriginalUiPageIndex50KG index;
        const std::string prefix(directory);
        if(!assets::readOriginalUiDat50KG(*archive,prefix+"ui.dat",indexDat,d.parse) ||
            !assets::readOriginalUiPageIndex50KG(indexDat,index,d.parse) ||
            route.page>index.pageFiles.size() ||
            !assets::readOriginalUiDat50KG(*archive,prefix+index.pageFiles[route.page-1],pageDat,d.parse) ||
            !pageDat.section("PageInfo"))return fail(d,Status::parse,0x1D64CC);
        const auto material=archive->find("font/denmrk16top.psm");
        if(!material.identity)return fail(d,Status::resource,0x1D6BA4);
        OriginalUiTextLayout50KG geometry;
        const OriginalUiTextInput50KG input{font,&atlas->texture,
            reinterpret_cast<const std::uint8_t*>(entry->text.data()),entry->text.size(),481.f,384.f};
        // Shared setup1D6C84 position, constructor scale1/angle0/depth1 and
        // default justification0. No width/height-based centering is applied.
        if(!layoutOriginalUiText50KG(input,geometry,d.text))return fail(d,Status::geometry,d.text.sourcePC);
        std::string text(entry->text);
        OriginalUiExitTextSnapshot50KG state;
        state.prepared=true;state.route=route;state.capacity5B=10;
        state.characterCount5C=static_cast<std::uint8_t>(text.size());
        // Setup allocates ceil(capacity/36)=1 source packet. Render's2E0880
        // derives active packets from live character count, kept separate.
        state.activePackets5E=1;state.color54=0x00CCCCCC;state.alpha58=255;
        archive_=std::move(archive);images_=std::move(images);text_=std::move(text);
        pageIdentity_=pageDat.identity();materialIdentity_=material.identity;
        imageIndex_=imageIndex;geometry_=geometry;state_=state;
        return fail(d,Status::ok);
    }catch(const std::bad_alloc&){return fail(d,Status::allocation);}
    catch(...){return fail(d,Status::parse);}
}
Status OriginalUiExitText50KG::projectRender2E0170(const OriginalUiApertureHook50KG& event,
    OriginalUiExitTextProjection50KG& output) noexcept {
    if(!prepared())return Status::notPrepared;
    if(event.kind!=OriginalUiApertureHookKind50KG::screenExitObject440 ||
        event.callPC!=0x1D75A4 || event.targetPC!=0x2E0170 || !event.observed.bound ||
        event.observed.state!=OriginalUiApertureState50KG::closing ||
        event.observed.bottom28C>224 || event.observed.top284<224 || event.observed.inputEnabled268)
        return Status::invalidRenderBoundary;
    OriginalUiExitTextProjection50KG next;
    next.glyphs=&geometry_;next.images=images_.get();next.materialIdentity=materialIdentity_;
    next.imageIndex=imageIndex_;next.packetColor=originalUiTextPacketColor50KG(state_.color54,state_.alpha58);
    // Exact2E0880 active-chunk count and2E13A0 default-justification position.
    // Immutable text/pose need no speculative per-frame geometry mutation.
    state_.activePackets5E=static_cast<std::uint8_t>((state_.characterCount5C+35u)/36u);
    state_.positionedX20=481.f;state_.positionedY24=384.f;state_.packetPrepared=true;
    ++state_.renderPreparations;output=next;return Status::ok;
}
}
