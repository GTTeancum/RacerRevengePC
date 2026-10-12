// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#include "original_ui_course_render_context50kg.h"
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
using Matrix=std::array<float,16>;
Matrix product(const Matrix& a,const Matrix& b) noexcept {
    Matrix output{};
    for (unsigned r=0;r<4;++r) for (unsigned c=0;c<4;++c) {
        float value=mul(b[c],a[r*4]);value=add(value,mul(b[4+c],a[r*4+1]));
        value=add(value,mul(b[8+c],a[r*4+2]));output[r*4+c]=add(value,mul(b[12+c],a[r*4+3]));
    }
    return output;
}
bool finite(const Matrix& matrix) noexcept {for (float f:matrix) if (!std::isfinite(f)) return false;return true;}
}
bool prepareOriginalUiCourseRenderContext50KG(const assets::OriginalUiArchive50KG& archive,
    const OriginalUiCourseCamera50KG& camera,OriginalUiCourseRenderContext50KG& output,
    OriginalUiCourseRenderContextDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](std::uint32_t pc){d.sourcePC=pc;return false;};
    if (!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)||
        camera.pageIdentity!=archive.find("UI/UICourse/track.dat").identity||
        std::fegetround()!=FE_TONEAREST||!finite(camera.core.view)||!finite(camera.core.projection)) return fail(0x1CE7B0);
    try {
        assets::OriginalUiDat50KG ui;
        if (!assets::readOriginalUiDat50KG(archive,"UI/UICourse/ui.dat",ui,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(0x1CE7F4);
        const auto* fog=ui.section("Fog");if (!fog) return fail(0x1CE7F4);
        OriginalUiCourseRenderContext50KG result;result.fogIdentity=ui.identity();
        if (!assets::originalUiNumber50KG(*fog,"Visibility",1,result.visibility,d.parse,0x1CE81C)||
            !assets::originalUiNumber50KG(*fog,"Haziness",.5f,result.haziness,d.parse,0x1CE8BC)||
            !assets::originalUiTriple50KG(*fog,"ColorRGB",{},result.sourceFogRgb,d.parse,0x1CE964)) return fail(0x1CE81C);
        //1CE840 bounds Visibility and1CE8E0 accidentally rechecks Visibility
        // instead of Haziness. These authenticated values are1/0, so no guessed
        // corrected clamp changes the actual source branch.
        if (result.visibility<0||result.visibility>1) result.visibility=1;
        result.fogEnd=add(900,mul(result.visibility,sub(1200,900)));
        result.fogStart=mul(sub(1,result.haziness),sub(result.fogEnd,200));
        result.packetFogStart33Z=result.fogEnd;
        const float range=sub(result.fogStart,result.fogEnd);
        result.packetFogScale33W=range==0?0:div(255,range);
        if (!std::isfinite(result.fogEnd)||result.fogEnd<=1||!std::isfinite(result.fogStart)||
            !std::isfinite(result.packetFogScale33W)) return fail(0x13E830);
        std::array<std::uint32_t,3> rgb{};
        for (unsigned i=0;i<3;++i) {
            const auto f=result.sourceFogRgb[i];if (!std::isfinite(f)||f<0||f>255) return fail(0x1CE964);
            rgb[i]=static_cast<std::uint32_t>(f);
        }
        result.packedFogColorGs=rgb[0]|(rgb[1]<<8)|(rgb[2]<<16);
        result.cameraAfterFog=camera;auto& core=result.cameraAfterFog.core;
        core.sceneAfter.nearPlane=1;core.sceneAfter.farPlane=result.fogEnd>100000?100000:result.fogEnd;
        const float z=div(1,sub(1,div(core.sceneAfter.nearPlane,core.sceneAfter.farPlane)));
        core.projection[10]=z;core.projection[14]=mul(-z,core.sceneAfter.nearPlane);
        core.combined=product(core.view,core.projection);
        if (!finite(core.combined)||prepareOriginalUiParticleCameraFromView50KG(core,640,448,
            result.cameraAfterFog.sourceScreen)!=OriginalUiMainParticleCameraStatus50KG::ok) return fail(0x13EA30);
        output=result;return true;
    } catch (...) {return fail(0x1CE7B0);}
}
}
