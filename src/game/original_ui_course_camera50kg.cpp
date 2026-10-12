// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#include "original_ui_course_camera50kg.h"
#include <cfenv>
#include <cstring>
namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
assets::TVCameraVector49E vector(const std::array<float,3>& v) noexcept {return {v[0],v[1],v[2],1};}
}
bool prepareOriginalUiCourseCamera50KG(const assets::OriginalUiArchive50KG& archive,
    OriginalUiCourseCamera50KG& output,OriginalUiCourseCameraDiagnostic50KG& d) noexcept {
    d={};
    if (!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)||
        std::fegetround()!=FE_TONEAREST) {d.sourcePC=0x1D7A90;return false;}
    try {
        assets::OriginalUiDat50KG page;
        if (!assets::readOriginalUiDat50KG(archive,"UI/UICourse/track.dat",page,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) {d.sourcePC=0x1D7A90;return false;}
        const auto* fields=page.section("PageInfo");
        if (!fields) {d.sourcePC=0x1D7A90;return false;}
        OriginalUiCourseCamera50KG result;result.pageIdentity=page.identity();
        if (!assets::originalUiTriple50KG(*fields,"CameraPosition1",{{0,0,0}},result.position,d.parse,0x1D7B74)||
            !assets::originalUiTriple50KG(*fields,"CameraLook1",{{0,-1,0}},result.look,d.parse,0x1D7BB0)||
            !assets::originalUiTriple50KG(*fields,"CameraUp1",{{0,0,1}},result.up,d.parse,0x1D7BE8)||
            !assets::originalUiNumber50KG(*fields,"CameraFOV1",35,result.fovDegrees,d.parse,0x1D7C10)||
            !assets::originalUiNumber50KG(*fields,"CameraRoll1",0,result.rollRadians,d.parse,0x1D7C34))
            {d.sourcePC=0x1D7B74;return false;}
        const std::uint32_t aspectBits=0x3faaaaabu;std::memcpy(&result.aspect1DC,&aspectBits,4);
        const float halfWidth=mul(.5f,640),halfHeight=mul(.5f,448);
        const float left=sub(2048,halfWidth),right=add(2048,halfWidth);
        const float top=sub(2048,halfHeight),bottom=add(2048,halfHeight);
        result.aspect1F8=div(div(sub(bottom,top),sub(right,left)),mul(result.aspect1DC,div(448,640)));
        CameraSceneState49F prior;prior.aspect1F8=result.aspect1F8;prior.rollRadians=result.rollRadians;
        TVCameraUpdateOutput49F input;input.mode=0;input.position=vector(result.position);
        input.look=vector(result.look);input.up=vector(result.up);input.fov=result.fovDegrees;
        if (!composeCameraView49F(prior,input,result.core)) {d.sourcePC=0x1D7C50;return false;}
        if (prepareOriginalUiParticleCameraFromView50KG(result.core,640,448,result.sourceScreen)!=
            OriginalUiMainParticleCameraStatus50KG::ok) {d.sourcePC=0x236DD0;return false;}
        result.binderCameraColumn={{result.core.view[2],result.core.view[6],result.core.view[10]}};
        output=result;return true;
    } catch (...) {d.sourcePC=0x1D7A90;return false;}
}
}
