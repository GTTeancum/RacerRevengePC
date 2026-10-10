#include "original_ui_options_camera50kg.h"
#include <cfenv>
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,sizeof(f));return f;}
float add(float x,float y) noexcept {volatile float f=x+y;return f;}
float sub(float x,float y) noexcept {volatile float f=x-y;return f;}
float mul(float x,float y) noexcept {volatile float f=x*y;return f;}
float div(float x,float y) noexcept {volatile float f=x/y;return f;}
assets::TVCameraVector49E vector(const std::array<float,3>& p) noexcept {return {p[0],p[1],p[2],1};}
}
OriginalUiOptionsCameraStatus50KG prepareOriginalUiOptionsCamera50KG(const assets::OriginalUiArchive50KG& archive,
    OriginalUiOptionsCamera50KG& output,OriginalUiOptionsCameraDiagnostic50KG& d) noexcept {
    using Status=OriginalUiOptionsCameraStatus50KG;d={};
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"Options camera requires binary32");
    if(!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))
        return d.status=Status::invalidArchive;
    if(std::fegetround()!=FE_TONEAREST)return d.status=Status::unsupportedArithmetic;
    try {
        assets::OriginalUiDat50KG ui;
        if(!assets::readOriginalUiDat50KG(archive,"UI/UIOption/ui.dat",ui,d.parse))return d.status=Status::parseFailure;
        OriginalUiOptionsCamera50KG c;c.uiIdentity=ui.identity();const auto* s=ui.section("CameraInfo");
        if(s) {
            c.hasCameraInfo=true;
            if(!assets::originalUiTriple50KG(*s,"CameraPosition",{{0,0,0}},c.position,d.parse,0x1D080C)||
                !assets::originalUiTriple50KG(*s,"CameraLook",{{0,-1,0}},c.look,d.parse,0x1D083C)||
                !assets::originalUiTriple50KG(*s,"CameraUp",{{0,0,1}},c.up,d.parse,0x1D0868)||
                !assets::originalUiNumber50KG(*s,"CameraFOV",35,c.fovDegrees,d.parse,0x1D0884)||
                !assets::originalUiNumber50KG(*s,"CameraRoll",0,c.rollRadians,d.parse,0x1D089C))return d.status=Status::parseFailure;
            constexpr const char* fields[]={"CameraPosition","CameraLook","CameraUp","CameraFOV","CameraRoll"};
            for(std::size_t i=0;i<5;++i)c.authoredFields[i]=s->find(fields[i])!=nullptr;
        }
        c.aspect1DC=bits(0x3FAAAAAB);
        const float halfWidth=mul(0.5f,640.f),fullRatio=div(448.f,640.f),halfHeight=mul(0.5f,448.f);
        const float left=sub(2048.f,halfWidth),right=add(2048.f,halfWidth),top=sub(2048.f,halfHeight),bottom=add(2048.f,halfHeight);
        const float viewportRatio=div(sub(bottom,top),sub(right,left));c.aspect1F8=div(viewportRatio,mul(c.aspect1DC,fullRatio));
        CameraSceneState49F prior;prior.aspect1F8=c.aspect1F8;prior.rollRadians=c.rollRadians;
        TVCameraUpdateOutput49F supplied;supplied.mode=0;supplied.position=vector(c.position);supplied.look=vector(c.look);supplied.up=vector(c.up);
        supplied.fov=c.hasCameraInfo?c.fovDegrees:0.f;
        if(!composeCameraView49F(prior,supplied,c.core))return d.status=Status::unsupportedArithmetic;
        output=c;return Status::ok;
    } catch(...) {return d.status=Status::parseFailure;}
}
}
