// Native widescreen (Hor+) through the real 49F -> GL converter: at 4:3 the
// original projection is unchanged; on wider outputs clip X shrinks by
// (4/3)/aspect (more horizontal view) while clip Y, Z and W are untouched.
#include "../../platform/linux/native/native_camera_view50dx.h"
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
unsigned checks=0;
void need(bool value,const char* what){++checks;if(!value)throw std::runtime_error(std::string("check failed: ")+what);}
rr::game::CameraViewOutput49F core(){
    rr::game::CameraViewOutput49F c{};
    for(int i=0;i<16;++i){c.view[i]=(i%5==0)?1.f:0.f;}
    // 14DB90 layout: h, h/aspect, z, 1, -z*near (original 4:3 aspect 0.75).
    const float h=1.f/std::tan(0.5f*1.2f);
    c.projection[0]=h;c.projection[5]=h/0.75f;c.projection[10]=1.001f;c.projection[11]=1.f;c.projection[14]=-1.001f;
    for(int i=0;i<16;++i)c.combined[i]=c.projection[i]; // Identity view.
    return c;
}
void clip(const rr::platform::NativeCameraMatrix50DX& m,const float p[3],float out[4]){
    // Row vector * column-major storage: out[c] = sum_r p[r]*m[c*4+r].
    for(int c=0;c<4;++c)out[c]=p[0]*m.worldViewProjection[c*4+0]+p[1]*m.worldViewProjection[c*4+1]+
        p[2]*m.worldViewProjection[c*4+2]+m.worldViewProjection[c*4+3];
}
rr::platform::NativeCameraMatrix50DX convert(int w,int h){
    rr::platform::NativeCameraViewContract50DX contract;
    contract.provenance=rr::platform::NativeCameraViewProvenance50DX::currentCore49FForOutput;
    contract.width=w;contract.height=h;contract.clipDepth=rr::platform::NativeCameraClipDepth50DX::negativeOneToOne;
    rr::platform::NativeCameraMatrix50DX m;rr::platform::NativeCameraViewError50DX e;
    need(rr::platform::convertNativeCameraView50DX(core(),contract,m,e),"convert");
    return m;
}
}

int main(){
    try{
        const float point[3]{3.f,2.f,10.f};
        float ref[4],wide[4],fourThree[4],psTwo[4];
        clip(convert(1024,768),point,fourThree);
        // Reference: the original combined matrix with only the GL Z remap.
        const auto c=core();
        ref[0]=point[0]*c.combined[0]+point[1]*c.combined[4]+point[2]*c.combined[8]+c.combined[12];
        ref[1]=point[0]*c.combined[1]+point[1]*c.combined[5]+point[2]*c.combined[9]+c.combined[13];
        need(fourThree[0]==ref[0]&&fourThree[1]==ref[1],"4:3 output keeps the original projection exactly");
        need(std::fabs(rr::platform::nativeWidescreenScaleX50DX(1024,768)-1.f)<1e-7f,"4:3 factor is 1");
        clip(convert(1920,1080),point,wide);
        need(std::fabs(wide[0]-0.75f*fourThree[0])<1e-5f,"16:9 clip X is 0.75 of 4:3 (a third more view)");
        need(wide[1]==fourThree[1]&&wide[2]==fourThree[2]&&wide[3]==fourThree[3],"16:9 keeps Y, Z, W (vertical view unchanged)");
        clip(convert(1280,896),point,psTwo);
        need(std::fabs(psTwo[0]-(4.f*896.f/(3.f*1280.f))*fourThree[0])<1e-5f,"1280x896 corrects the old 7% stretch");
        need(std::fabs(rr::platform::nativeWidescreenScaleX50DX(2560,1080)-(4.f*1080.f/(3.f*2560.f)))<1e-6f,"21:9 factor");
        std::printf("widescreen projection checks=%u passed (16:9 X factor %.4f)\n",checks,
            static_cast<double>(rr::platform::nativeWidescreenScaleX50DX(1920,1080)));
        return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}
}
