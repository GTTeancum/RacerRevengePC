#include "original_ui_rider_selected_packet50kg.h"
#include <cstring>
namespace rr::game {
namespace {
// Original archive catalog identities are process-lifetime canonical objects.
// This hot lookup never constructs a temporary std::string inside noexcept.
const assets::OriginalUiMemberIdentity50KG* riderMember(const char* name) noexcept {
    const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider);
    if(!archive)return nullptr;
    for(std::size_t i=0;i<archive->memberCount;++i){
        const auto* member=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::rider,i);
        if(member&&std::strcmp(member->name,name)==0)return member;
    }
    return nullptr;
}
}

bool prepareOriginalUiRiderSelectedPacket50KG(std::uint16_t widget,
    OriginalUiRiderDynamicOwner50KG& dynamic,OriginalUiRiderBackend50KG& backend,
    const OriginalUiRiderRenderContext50KG& render,const OriginalUiRiderLighting50KG& lighting,
    OriginalUiLightManagerLifetime50KG& lightLifetime,OriginalUiRiderSelectedPacket50KG& output,
    OriginalUiRiderSelectedPacketDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiRiderSelectedPacketDiagnostic50KG::Error;
    const auto fail=[&](E error){d.error=error;return false;};
    auto* visibility=backend.visibility();const auto* membership=backend.membership(widget);
    if(!visibility||!membership||!backend.shown(widget))return fail(E::input);
    OriginalUiRiderSelectedPacket50KG next{};
    if(!visibility->selectModel(*membership,next.selection,d.membership)||
       !visibility->isCurrentSelection(next.selection))return fail(E::membership);
    if(!next.selection.selected()){output=next;return true;}
    const auto state=dynamic.state(widget);
    if(state.kind==OriginalUiRiderWidgetKind50KG::pod){
        const auto assets=dynamic.podAssets(widget);auto* scene=dynamic.podScene(widget);
        if(!assets||!scene||!prepareOriginalUiRiderPodPacket50KG(*assets,*scene,render,lighting,
            next.selection.sourceModelFlags191,next.packet,d.packet))return fail(E::packet);
    }else if(state.kind==OriginalUiRiderWidgetKind50KG::character){
        const auto assets=dynamic.characterAssets(widget);auto* scene=dynamic.characterScene(widget);
        const auto archive=backend.riderArchive();
        if(!assets||!assets->loaded()||!scene||!scene->attached()||scene->scene()!=assets->scene()||
           !archive||!assets->hierarchy()||assets->widgetSource()!=riderMember("UI\\UIRider\\Widgets.Dat"))return fail(E::input);
        original_racer_lighting50KM::LightMatrices lights{};
        if(!lightLifetime.selectRiderCharacter(*assets,next.selection,lights,d.lighting))return fail(E::lighting);
        if(!prepareOriginalUiRiderLitContext50KG(*archive,render,lights,next.packet.context))return fail(E::packet);
        if(!scene->projectPalette(next.selection.sourceModelFlags191,next.packet.palette,d.packet.model))return fail(E::packet);
        if(!prepareOriginalUiRiderModelFog50KG(*assets->hierarchy(),scene->graph(),render.cameraAfterFog.position,
            render.fogStart,next.packet.fog))return fail(E::packet);
        next.packet.scene=assets->scene();next.packet.sourceModelFlags191=next.selection.sourceModelFlags191;
    }else return fail(E::input);
    if(!visibility->recordPacketFog(next.selection,next.packet.fog.computeRadialFog1B2))return fail(E::membership);
    next.emitsGeometry=true;output=next;return true;
}
}
