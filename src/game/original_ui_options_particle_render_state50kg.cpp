#include "original_ui_options_particle_render_state50kg.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
using Matrix=std::array<float,16>;
bool finite(const Matrix& m) noexcept {
    for(float v:m)if(!std::isfinite(v))return false;
    return true;
}
Matrix multiply(const Matrix& a,const Matrix& b) noexcept {
    Matrix out{};
    // Source115410/236E50 row-vector VMULAx/VMADDAy/VMADDAz/VMADDw order.
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c) {
        float v=mul(b[c],a[r*4]);
        v=add(v,mul(b[4+c],a[r*4+1]));
        v=add(v,mul(b[8+c],a[r*4+2]));
        out[r*4+c]=add(v,mul(b[12+c],a[r*4+3]));
    }
    return out;
}
}
OriginalUiOptionsParticleRenderStateStatus50KG prepareOriginalUiOptionsParticleRenderState50KG(
    const OriginalUiMainSceneOwner50KG& scene,
    const OriginalUiOptionsCamera50KG& camera,
    const OriginalUiMainParticleLighting50KG& lighting,
    OriginalUiOptionsParticleRenderState50KG& output) noexcept {
    using Status=OriginalUiOptionsParticleRenderStateStatus50KG;
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,
        "Options particle render state requires binary32");
    const auto* def=scene.definition();
    if(!def||!scene.prepared()||scene.archiveIdentity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options)||
        !camera.uiIdentity||camera.uiIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,0)||
        camera.core.sceneAfter.nearPlane!=1.f||camera.core.sceneAfter.farPlane!=100000.f||
        !finite(camera.core.view)||!finite(camera.core.projection)||!finite(camera.core.combined)||
        def->fogColor!=std::array<float,3>{70.f,10.f,10.f}||
        def->fogVisibility!=1.f||def->fogHaziness!=0.f)
        return Status::input;
    for(unsigned r=0;r<4;++r) {
        if(lighting.sourceRgbPacked[r]!=0x00313131u||
            lighting.sourceLightingWBits46To49[r]!=0x40u)
            return Status::input;
        for(float v:lighting.sourceLightingRgb46To49[r])if(v!=49.f)return Status::input;
    }
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    OriginalUiOptionsParticleRenderState50KG next;
    // Options1D0B00..1D0B28 supplies maximum4000 and separately rounded4000/3.
    // Scene273D54..273D78 supplies base64 to13E780.13E830 computes the range.
    const float maximum=4000.f,minimum=div(maximum,3.f),base=64.f;
    next.sourceFogEnd=add(minimum,mul(def->fogVisibility,sub(maximum,minimum)));
    next.sourceFogStart=mul(sub(1.f,def->fogHaziness),sub(next.sourceFogEnd,base));
    //13EA74/state36 -> context+B0=start;13EA94/state37 -> context+B4=end.
    //2451D4..2451F4 scale=255/(start-end).24BE38..24BE78 uploads B4/B8
    // directly to row33.zw, despite the ParticleCode input's unusual depth.
    next.constants.fogStart33Z=next.sourceFogEnd;
    const float difference=sub(next.sourceFogStart,next.sourceFogEnd);
    next.constants.fogScale33W=difference==0.f?0.f:div(255.f,difference);
    //13EA30..13EA60 changes the active borrowed camera before drawing;
    //114F20 clamps near>=1,far<=100000. The actual end4000 needs no clamp.
    next.sceneCamera=camera;
    auto& core=next.sceneCamera.core;
    core.sceneAfter.nearPlane=1.f;core.sceneAfter.farPlane=next.sourceFogEnd;
    //14DB90/115350 recompute projection; FOV/aspect and normalized view are
    // unchanged. Preserve already prepared XY lanes and replace only Z terms.
    const float nearFar=div(core.sceneAfter.nearPlane,core.sceneAfter.farPlane);
    const float z=div(1.f,sub(1.f,nearFar));
    core.projection[10]=z;
    core.projection[14]=mul(-z,core.sceneAfter.nearPlane);
    core.combined=multiply(core.view,core.projection);
    if(!finite(core.combined)||!std::isfinite(next.constants.fogScale33W)||
        prepareOriginalUiOptionsParticleCamera50KG(next.sceneCamera,next.particleCamera)!=
            OriginalUiMainParticleCameraStatus50KG::ok)
        return Status::arithmetic;
    next.constants.combinedClip100=core.combined;
    // Actual source640x448 GS XYOFFSET, independent of desktop window size.
    next.constants.sourceCanvasOffset={1728.f,1824.f};
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<3;++c)
        next.constants.sourceLighting[r][c]=
            static_cast<std::uint8_t>(lighting.sourceLightingRgb46To49[r][c]);
    //273C64..273CB0 packs source AARRGGBB;13EB90..13EBBC reverses RGB byte
    // placement for FOGCOL register3D. Exact authored integer channels need no
    // rounding-policy substitute, and no alpha component enters GS FOGCOL.
    const auto red=static_cast<std::uint32_t>(def->fogColor[0]);
    const auto green=static_cast<std::uint32_t>(def->fogColor[1]);
    const auto blue=static_cast<std::uint32_t>(def->fogColor[2]);
    next.packedFogColorGs=red|(green<<8)|(blue<<16);
    next.fogColor={div(static_cast<float>(red),255.f),
        div(static_cast<float>(green),255.f),div(static_cast<float>(blue),255.f)};
    output=next;return Status::complete;
}
}
