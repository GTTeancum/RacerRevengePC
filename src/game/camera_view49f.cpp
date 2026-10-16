#include "camera_view49f.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Vec=assets::TVCameraVector49E;
using Mat=std::array<float,16>;
void copyVector(Vec& to,const Vec& from) noexcept {std::memcpy(&to,&from,sizeof(Vec));}
float constant(std::uint32_t bits) noexcept {
    float f; std::memcpy(&f,&bits,sizeof(f)); return f;
}
// Every original scalar/VU step rounds separately. Do not contract these into
// fused operations. Rejection is native admission, not an original branch.
struct Arithmetic {
    bool ok=true;
    float check(float x) noexcept { if(!std::isfinite(x))ok=false; return x; }
    float add(float a,float b) noexcept {volatile float r=a+b;return check(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return check(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return check(r);}
    float div(float a,float b) noexcept {
        if(b==0 || !std::isfinite(a) || !std::isfinite(b)){ok=false;return 0;}
        volatile float r=a/b;return check(r);
    }
    void xyz(const Vec& v) noexcept {check(v.x);check(v.y);check(v.z);}
};
float dot(Arithmetic& a,const Vec& x,const Vec& y) noexcept {
    const float p=a.mul(x.x,y.x),q=a.mul(x.y,y.y),r=a.mul(x.z,y.z);
    return a.add(a.add(p,q),r);
}
Vec cross(Arithmetic& a,const Vec& x,const Vec& y) noexcept {
    return {a.sub(a.mul(x.y,y.z),a.mul(y.y,x.z)),
            a.sub(a.mul(x.z,y.x),a.mul(y.z,x.x)),
            a.sub(a.mul(x.x,y.y),a.mul(y.x,x.y)),0};
}
Vec normalize(Arithmetic& a,const Vec& v) noexcept {
    a.xyz(v);
    const float d=dot(a,v,v);
    float q=std::numeric_limits<float>::max();
    if(d!=0){volatile float root=std::sqrt(std::fabs(d));q=a.div(1,a.check(root));}
    // Original VADDx.w is arithmetic, including -0 + +0 -> +0 in this model.
    a.check(v.w);
    Vec result{a.mul(v.x,q),a.mul(v.y,q),a.mul(v.z,q),a.add(v.w,0.0f)};
    return result;
}
struct SinCos {float sine,cosine;};
//13BFE0 and13BCA0 share these same separately rounded polynomial steps.
SinCos trig(Arithmetic& a,float x) noexcept {
    a.check(x);
    float sign=1;
    if(x>constant(0x3fc90fdbu)){x=a.sub(constant(0x40490fdbu),x);sign=-1;}
    else if(x<constant(0xbfc90fdbu)){x=a.sub(constant(0xc0490fdbu),x);sign=-1;}
    const float square=a.mul(x,x);
    float c=a.mul(constant(0xb493f27eu),square);
    c=a.add(constant(0x37d00d01u),c);
    c=a.mul(square,c);
    c=a.add(constant(0xbab60b61u),c);
    c=a.mul(square,c);
    c=a.add(constant(0x3d2aaaabu),c);
    c=a.mul(square,c);
    c=a.add(-0.5f,c);
    c=a.mul(square,c);
    c=a.add(1,c);
    c=a.mul(sign,c);
    float s=a.mul(constant(0x3638ef1du),square);
    s=a.add(constant(0xb9500d01u),s);
    s=a.mul(square,s);
    s=a.add(constant(0x3c088889u),s);
    s=a.mul(square,s);
    s=a.add(constant(0xbe2aaaabu),s);
    s=a.mul(a.mul(x,square),s);
    s=a.add(x,s);
    return {s,c};
}
float tangent(Arithmetic& a,float x) noexcept {
    const SinCos sc=trig(a,x);
    return sc.cosine==0 ? std::numeric_limits<float>::max() : a.div(sc.sine,sc.cosine);
}
Mat multiply(Arithmetic& a,const Mat& x,const Mat& y) noexcept {
    Mat z{};
    //115410 and14DED8: each row is VMULAx, VMADDAy, VMADDAz, VMADDw.
    for(unsigned row=0;row<4;++row)for(unsigned col=0;col<4;++col){
        float v=a.mul(y[col],x[row*4]);
        v=a.add(v,a.mul(y[4+col],x[row*4+1]));
        v=a.add(v,a.mul(y[8+col],x[row*4+2]));
        z[row*4+col]=a.add(v,a.mul(y[12+col],x[row*4+3]));
    }
    return z;
}
Mat viewMatrix(Arithmetic& a,const Vec& p,const Vec& look,const Vec& up,float roll) noexcept {
    a.xyz(p);a.xyz(look);a.xyz(up);a.check(roll);
    const Vec right=cross(a,up,look);
    Mat m{right.x,up.x,look.x,0,right.y,up.y,look.y,0,
          right.z,up.z,look.z,0,-dot(a,right,p),-dot(a,up,p),-dot(a,look,p),1};
    if(roll!=0){
        const SinCos sc=trig(a,-roll);
        const Mat rotate{sc.cosine,-sc.sine,0,0,sc.sine,sc.cosine,0,0,
                         0,0,1,0,0,0,0,1};
        m=multiply(a,m,rotate);
    }
    return m;
}
Mat projectionMatrix(Arithmetic& a,const CameraSceneState49F& s) noexcept {
    a.check(s.fovDegrees);a.check(s.aspect1F8);a.check(s.nearPlane);a.check(s.farPlane);
    const float invAspect=a.div(1,s.aspect1F8);
    const float angle=a.mul(constant(0x3c8efa34u),s.fovDegrees);
    const float t=tangent(a,a.mul(0.5f,angle));
    const float nearFar=a.div(s.nearPlane,s.farPlane);
    const float h=a.div(1,t);
    const float z=a.div(1,a.sub(1,nearFar));
    Mat m{};m[0]=h;m[5]=a.mul(h,invAspect);m[10]=z;m[11]=1;
    m[14]=a.mul(-z,s.nearPlane);
    return m;
}
}

bool composeCameraView49F(const CameraSceneState49F& prior,
    const TVCameraUpdateOutput49F& supplied,CameraViewOutput49F& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    Arithmetic a;
    CameraViewOutput49F next;
    std::memcpy(&next.sceneAfter,&prior,sizeof(prior));
    copyVector(next.position,supplied.position);
    a.xyz(supplied.position);
    a.check(supplied.fov);
    next.fovWritten=supplied.fov!=0;
    if(next.fovWritten)next.sceneAfter.fovDegrees=supplied.fov;
    if(supplied.mode==0){
        const Vec look=normalize(a,supplied.look),up=normalize(a,supplied.up);
        copyVector(next.sceneAfter.look,look);
        copyVector(next.sceneAfter.up,up);
    }else{
        //115060 supplies this Up to114F90 before1150C0 runs.
        const Vec carriedUp=normalize(a,prior.up);
        copyVector(next.sceneAfter.up,carriedUp);
        a.xyz(supplied.target);
        if(!(supplied.target.x==supplied.position.x &&
             supplied.target.y==supplied.position.y &&
             supplied.target.z==supplied.position.z)){
            const Vec direction{a.sub(supplied.target.x,supplied.position.x),
                a.sub(supplied.target.y,supplied.position.y),
                a.sub(supplied.target.z,supplied.position.z),1};
            next.sceneAfter.look=normalize(a,direction);
            Vec right{1,0,0,1};
            if(supplied.target.x!=supplied.position.x || supplied.target.z!=supplied.position.z)
                right=normalize(a,cross(a,Vec{0,1,0,1},next.sceneAfter.look));
            next.sceneAfter.up=cross(a,next.sceneAfter.look,right);
            next.targetAccepted=true;
        }
    }
    next.view=viewMatrix(a,next.position,next.sceneAfter.look,next.sceneAfter.up,prior.rollRadians);
    next.projection=projectionMatrix(a,next.sceneAfter);
    next.combined=multiply(a,next.view,next.projection);
    if(!a.ok)return false;
    std::memcpy(&output,&next,sizeof(next));
    return true;
}

bool composePortalZoneView280720(const CameraViewOutput49F& camera,
    const std::array<float,4>& bounds,PortalZoneViewOutput49F& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    Arithmetic a;
    const auto& s=camera.sceneAfter;
    for(float v:bounds)a.check(v);
    for(float v:camera.view)a.check(v);
    a.check(s.fovDegrees);a.check(s.aspect1F8);a.check(s.nearPlane);a.check(s.farPlane);
    if(!a.ok||!(bounds[0]<bounds[1])||!(bounds[2]<bounds[3])||
       !(s.fovDegrees>0&&s.fovDegrees<180)||!(s.aspect1F8>0)||
       !(s.nearPlane>0)||!(s.farPlane>s.nearPlane))return false;
    // 28076C..2807A4: direct half-angle constant, then near*tan,
    // aspect scaling of the Y pair, and the authored viewport bounds.
    const float t=tangent(a,a.mul(constant(0x3c0efa34u),s.fovDegrees));
    const float halfX=a.mul(s.nearPlane,t);
    const float halfY=a.mul(halfX,s.aspect1F8);
    const float left=a.mul(halfX,bounds[0]),right=a.mul(halfX,bounds[1]);
    const float bottom=a.mul(halfY,bounds[2]),top=a.mul(halfY,bounds[3]);
    // 14DAF0: row-vector off-axis matrix. Keep sums, products and divisions
    // separately rounded, including (-2*far)*near in the depth translation.
    Mat projection{};
    const float sumX=a.add(right,left);
    const float twiceNear=a.mul(2.f,s.nearPlane);
    const float width=a.sub(right,left);
    projection[0]=a.div(twiceNear,width);
    const float sumY=a.add(top,bottom);
    projection[8]=a.div(-sumX,width);
    const float height=a.sub(top,bottom);
    projection[5]=a.div(twiceNear,height);
    projection[9]=a.div(-sumY,height);
    const float depthProduct=a.mul(a.mul(-2.f,s.farPlane),s.nearPlane);
    const float depth=a.sub(s.farPlane,s.nearPlane);
    const float depthSum=a.add(s.farPlane,s.nearPlane);
    projection[10]=a.div(depthSum,depth);
    projection[14]=a.div(depthProduct,depth);
    projection[11]=1.f;
    // 2807D4..280860: current view+C0 multiplied by this projection.
    PortalZoneViewOutput49F next;
    next.projection=projection;
    next.matrix60=multiply(a,camera.view,projection);
    if(!a.ok)return false;
    output=next;return true;
}
}
