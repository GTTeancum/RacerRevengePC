#include "original_ui_main_particle_camera50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Matrix=std::array<float,16>;
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,sizeof(f));return f;}
struct Arithmetic {
    bool ok=true;
    float check(float v) noexcept {if(!std::isfinite(v))ok=false;return v;}
    float add(float x,float y) noexcept {volatile float v=x+y;return check(v);}
    float sub(float x,float y) noexcept {volatile float v=x-y;return check(v);}
    float mul(float x,float y) noexcept {volatile float v=x*y;return check(v);}
    float div(float x,float y) noexcept {
        if(y==0||!std::isfinite(x)||!std::isfinite(y)){ok=false;return 0;}
        volatile float v=x/y;return check(v);
    }
};
// Same recovered13BCA0 polynomial as camera_view49f.cpp. Its internal helper
// is deliberately not exported; this bounded continuation keeps the exact
// scalar operation order rather than substituting host std::tan.
float sourceTangent(Arithmetic& a,float x) noexcept {
    a.check(x);float sign=1;
    if(x>bits(0x3FC90FDB)){x=a.sub(bits(0x40490FDB),x);sign=-1;}
    else if(x<bits(0xBFC90FDB)){x=a.sub(bits(0xC0490FDB),x);sign=-1;}
    const float square=a.mul(x,x);
    float c=a.mul(bits(0xB493F27E),square);
    c=a.add(bits(0x37D00D01),c);c=a.mul(square,c);
    c=a.add(bits(0xBAB60B61),c);c=a.mul(square,c);
    c=a.add(bits(0x3D2AAAAB),c);c=a.mul(square,c);
    c=a.add(-0.5f,c);c=a.mul(square,c);c=a.add(1,c);c=a.mul(sign,c);
    float s=a.mul(bits(0x3638EF1D),square);
    s=a.add(bits(0xB9500D01),s);s=a.mul(square,s);
    s=a.add(bits(0x3C088889),s);s=a.mul(square,s);
    s=a.add(bits(0xBE2AAAAB),s);
    s=a.mul(a.mul(x,square),s);s=a.add(x,s);
    return c==0?std::numeric_limits<float>::max():a.div(s,c);
}
Matrix multiply(Arithmetic& a,const Matrix& x,const Matrix& y) noexcept {
    Matrix result{};
    //236E50..236EBC matches115410: VMULAx, VMADDAy, VMADDAz, VMADDw.
    for(unsigned row=0;row<4;++row)for(unsigned col=0;col<4;++col) {
        float v=a.mul(y[col],x[row*4]);
        v=a.add(v,a.mul(y[4+col],x[row*4+1]));
        v=a.add(v,a.mul(y[8+col],x[row*4+2]));
        result[row*4+col]=a.add(v,a.mul(y[12+col],x[row*4+3]));
    }
    return result;
}
}
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiParticleCameraFromView50KG(
    const CameraViewOutput49F& core,float originalWidth,float originalHeight,
    OriginalUiMainParticleCamera50KG& output) noexcept {
    using Status=OriginalUiMainParticleCameraStatus50KG;
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"Main camera requires binary32");
    if(originalWidth!=640||originalHeight!=448)return Status::invalidCamera;
    if(std::fegetround()!=FE_TONEAREST)return Status::unsupportedArithmetic;
    Arithmetic a;OriginalUiMainParticleCamera50KG next;
    for(float v:core.combined)a.check(v);
    const float fov=a.check(core.sceneAfter.fovDegrees);
    const float width=static_cast<float>(originalWidth);
    const float height=static_cast<float>(originalHeight);
    const float halfWidth=a.mul(0.5f,width),halfHeight=a.mul(0.5f,height);
    const float left=a.sub(2048.0f,halfWidth),right=a.add(2048.0f,halfWidth);
    const float top=a.sub(2048.0f,halfHeight),bottom=a.add(2048.0f,halfHeight);
    const float initialX=a.mul(0.5f,a.sub(right,left));
    const float initialY=a.mul(0.5f,a.sub(top,bottom));
    const float depthFar=bits(0x4AFFFFDE); //8388591 from236218..236228.
    //14D840 supplies identity;2362F0..236310 writes diagonal/translation.
    next.sourceScreen140[0]=a.mul(initialX,1.0f); //236E18..236E24 zoom328.
    next.sourceScreen140[5]=a.mul(initialY,1.0f); //236E28..236E38 zoom32C.
    next.sourceScreen140[10]=a.sub(1.0f,depthFar);
    next.sourceScreen140[12]=a.add(initialX,left);
    next.sourceScreen140[13]=a.add(initialY,bottom);
    next.sourceScreen140[14]=depthFar;next.sourceScreen140[15]=1.0f;
    next.combinedSourceScreen250=multiply(a,core.combined,next.sourceScreen140);
    //+1E8 is unsigned source viewport width:114988. Main640 needs no special
    // large-unsigned conversion path from1154B4..1154E4.
    const float halfFov=a.mul(0.5f,fov);
    const float tangent=sourceTangent(a,a.mul(bits(0x3C8EFA34),halfFov));
    next.focalPixels1D8=a.div(a.mul(0.5f,width),tangent);
    if(!a.ok)return Status::unsupportedArithmetic;
    output=next;return Status::ok;
}
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiMainParticleCamera50KG(
    const OriginalUiMainCameraPreparation50KG& camera,OriginalUiMainParticleCamera50KG& output) noexcept {
    assets::OriginalUiMainCameraConfig50KG identity;identity.identity=camera.configurationIdentity;
    if(!assets::isOriginalUiMainCameraConfig50KG(identity))return OriginalUiMainParticleCameraStatus50KG::invalidCamera;
    return prepareOriginalUiParticleCameraFromView50KG(camera.core,camera.originalWidth,camera.originalHeight,output);
}
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiOptionsParticleCamera50KG(
    const OriginalUiOptionsCamera50KG& camera,OriginalUiMainParticleCamera50KG& output) noexcept {
    // Options catalog ordinal0 is the actual UI/UIOption/ui.dat, not a Main alias.
    if(!camera.uiIdentity||camera.uiIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,0))
        return OriginalUiMainParticleCameraStatus50KG::invalidCamera;
    return prepareOriginalUiParticleCameraFromView50KG(camera.core,640,448,output);
}
}
