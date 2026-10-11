#include "original_ui_rider_model_packet50kg.h"
#include <cmath>
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

namespace {
using E=OriginalUiRiderPacketError50KG;
bool fail(OriginalUiRiderPacketDiagnostic50KG& d,E e) noexcept {d.error=e;return false;}
bool finite(const original_racer_lighting50KM::LightMatrices& lights) noexcept {
    for(const auto* m:{&lights.directions,&lights.diffuse,&lights.specular})
        for(const auto& row:*m)for(float x:row)if(!std::isfinite(x))return false;
    return true;
}
}
bool prepareOriginalUiRiderLitContext50KG(const assets::OriginalUiArchive50KG& archive,
    const OriginalUiRiderRenderContext50KG& render,
    const original_racer_lighting50KM::LightMatrices& lights,OriginalUiLitContext50KM& output) noexcept {
    if(!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider)||
        render.cameraAfterFog.pageIdentity!=riderMember("UI\\UIRider\\rider.dat")||
        render.fogIdentity!=riderMember("UI\\UIRider\\ui.dat")||!render.fogIdentity||
        !render.cameraAfterFog.pageIdentity||!finite(lights)||
        !std::isfinite(render.packetFogStart33Z)||!std::isfinite(render.packetFogScale33W))return false;
    OriginalUiRiderClipCamera50KG clip;
    if(!prepareOriginalUiRiderClipCamera50KG(render.cameraAfterFog,clip))return false;
    OriginalUiLitContext50KM next;
    const auto& camera=render.cameraAfterFog;
    next.viewC0=camera.core.view;next.projection80=camera.core.projection;
    next.clipProjection290=clip.clipProjection290;next.screen140=camera.sourceScreen.sourceScreen140;
    next.cameraPosition180=camera.position;next.lights=lights;
    next.fogEnd33Z=render.packetFogStart33Z;next.fogSlope33W=render.packetFogScale33W;
    next.canvasOffset=render.sourceCanvasOffset;
    output=next;return true;
}
bool prepareOriginalUiRiderPodPacket50KG(const assets::OriginalUiRiderPodAssets50KG& assets,
    OriginalUiRiderPodScene50KG& model,const OriginalUiRiderRenderContext50KG& render,
    const OriginalUiRiderLighting50KG& lighting,std::uint8_t flags,
    OriginalUiRiderModelPacket50KG& output,OriginalUiRiderPacketDiagnostic50KG& diagnostic) noexcept {
    diagnostic={};const auto archive=assets.archive();
    if(!assets.loaded()||!model.loaded()||!archive||!assets.hierarchy()||
        model.scene()!=assets.scene()||lighting.uiIdentity!=riderMember("UI\\UIRider\\ui.dat")||
        !lighting.uiIdentity)return fail(diagnostic,E::input);
    if((flags&1u)==0)return fail(diagnostic,E::selection);
    original_racer_lighting50KM::LightMatrices lights;
    if(projectOriginalUiGlobalLightRows50KM(true,lighting.directionalLook78,lighting.directional3C,
        lighting.ambient50,lights)!=original_racer_lighting50KM::Status::complete)return fail(diagnostic,E::lights);
    OriginalUiRiderModelPacket50KG next{};
    if(!prepareOriginalUiRiderLitContext50KG(*archive,render,lights,next.context))return fail(diagnostic,E::camera);
    if(!model.projectPalette(flags,next.palette,diagnostic.model))return fail(diagnostic,E::palette);
    if(!prepareOriginalUiRiderModelFog50KG(*assets.hierarchy(),model.graph(),render.cameraAfterFog.position,
        render.fogStart,next.fog))return fail(diagnostic,E::bounds);
    next.scene=assets.scene();next.sourceModelFlags191=flags;
    output=next;return true;
}
}
