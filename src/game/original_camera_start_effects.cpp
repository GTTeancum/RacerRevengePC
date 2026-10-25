#include "original_camera_start_effects.h"
#include "body_scene48g.h"
#include "scalar_minmax48o.h"
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
namespace {
using S=OriginalCameraStartStatus;
OriginalCameraStartResult result(S s,std::uint32_t pc) noexcept {return {s,pc};}
bool cvt(float x,std::int32_t& out) noexcept {
    // Retained EE CVT.W.S signed truncation contract, finite int32 subset.
    if(!std::isfinite(x)||double(x)<-2147483648.0||double(x)>=2147483648.0)return false;
    out=static_cast<std::int32_t>(x);return true;
}
bool sampledMilliseconds(HostMonotonicClock50ID& clock,float& value) noexcept {
    const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now()-clock.epoch).count();
    if(elapsed<0)return false;
    const auto ns=static_cast<std::uint64_t>(elapsed);
    if(ns<clock.lastNanoseconds)return false;
    constexpr std::uint64_t billion=1000000000u,rate=576000u;
    const auto seconds=ns/billion,remainder=ns%billion;
    if(seconds>std::numeric_limits<std::uint64_t>::max()/rate)return false;
    ClockQuantization49J q;
    if(!quantizeCounter49J(seconds*rate+remainder*rate/billion,q))return false;
    value=constant(q.scaledMillisecondBits);clock.lastNanoseconds=ns;return true;
}
bool guards(Arithmetic& a,OriginalCameraViewportFields& p) noexcept {
    const float xx=std::fabs(a.check(p.screen140[0]));
    const float yy=std::fabs(a.check(p.screen140[5]));
    const float x=a.add(p.screen140[12],xx),y=a.add(p.screen140[13],yy);
    const float rx=a.sub(constant(0x457ff000),x),ry=a.sub(constant(0x457ff000),y);
    p.guardX334=a.div(xx,detail::scalarMin48O(x,rx));
    p.guardY338=a.div(yy,detail::scalarMin48O(y,ry));return a.ok;
}
}
OriginalCameraStartResult setOriginalCameraRange237000(float nearPlane,float farPlane,
    const Mat& view,const Mat& screen,float guardX,float guardY,Mat& projection,Mat& combined,
    Mat& combinedScreen,Mat& guardProjection,Mat& guardCombined)noexcept {
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x115510);
    Arithmetic a;
    const float ratio=a.div(nearPlane,farPlane),z=a.div(1,a.sub(1,ratio));
    if(!a.ok)return result(S::arithmetic,0x115538);
    projection[10]=z; //11553C
    const float translation=a.mul(-z,nearPlane);
    if(!a.ok)return result(S::arithmetic,0x115544);
    projection[14]=translation; //115548
    auto product=multiply(a,view,projection);
    if(!a.ok)return result(S::arithmetic,0x11554C);
    combined=product; //1155AC..1155BC
    product=multiply(a,combined,screen);
    if(!a.ok)return result(S::arithmetic,0x237020);
    combinedScreen=product; //237080..23708C
    guardProjection=projection; //23709C..2370B4
    for(std::size_t row=0;row<4;++row){const float x=a.mul(guardProjection[row*4],guardX);
        if(!a.ok)return result(S::arithmetic,0x2370D0+static_cast<std::uint32_t>(row)*16u);
        guardProjection[row*4]=x;}
    for(std::size_t row=0;row<4;++row){const float y=a.mul(guardProjection[row*4+1],guardY);
        if(!a.ok)return result(S::arithmetic,0x237110+static_cast<std::uint32_t>(row)*16u);
        guardProjection[row*4+1]=y;}
    product=multiply(a,view,guardProjection);
    if(!a.ok)return result(S::arithmetic,0x237148);
    guardCombined=product;return result(S::complete,0x2371C0);
}
OriginalCameraStartResult requestOriginalCameraExitFade2AED60(std::int32_t a8,
    std::int32_t appMode,OriginalVideoFadeState& video,HostMonotonicClock50ID& clock) noexcept {
    if(a8!=0)return result(S::complete,0x2aee60); //v0=0, caller ignores it.
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x2aedd8);
    auto v=video;Arithmetic a;std::int32_t duration=500;
    if(v.covered3DC||(v.flags404Low6&16u)){
        if(!v.alpha3E8)return result(S::unknownVideoAlpha,0x2aedd4);
        const float scale=a.sub(1,a.div(static_cast<float>(*v.alpha3E8),128));
        if(!cvt(a.mul(scale,500),duration)||!a.ok)return result(S::arithmetic,0x2aedec);
        if(duration==0)return result(S::complete,0x2aee00);
    }
    //2AED60 passes a2=0: source2605C8 leaves bit2 clear. The NEXT reached
    //25A9C0 invokes blocking260850 at25AA4C;25B5D0 is not its consumer.
    return requestOriginalVideoFade260320(video,clock,appMode,duration,false,false);
}
OriginalCameraStartResult advanceOriginalVideoFade25B5D0(OriginalVideoFadeState& video,
    HostMonotonicClock50ID& clock,std::int32_t view,std::int32_t count,OriginalVideoFadeDraw& draw) noexcept {
    //25B5D0 reaches rendering only on the last actual camera view.
    if(std::int64_t(view)<std::int64_t(count)-1||(video.flags404Low6&8u)){
        draw={};return result(S::complete,0x25b9b8);
    }
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x25b720);
    auto v=video;OriginalVideoFadeDraw d;Arithmetic a;
    const bool increasing=(v.flags404Low6&5u)==5u;
    const bool decreasing=!increasing&&(v.flags404Low6&6u)==6u;
    if((increasing||decreasing)&&!v.alpha3E8)return result(S::unknownVideoAlpha,0x25b76c);
    if((increasing||decreasing)&&v.millisecondsPerStep3F0==0)return result(S::arithmetic,0x25b750);
    if(!std::isfinite(v.lastMilliseconds400))return result(S::arithmetic,0x25b728);
    float now;if(!sampledMilliseconds(clock,now))return result(S::clock,0x25b718);
    if(v.covered3DC&&!(v.flags404Low6&3u)){
        d.reached25E610=true;d.argb=v.toColor3E0;v.flags404Low6|=8u;
    }
    if(increasing||decreasing){
        const float delta=a.sub(now,v.lastMilliseconds400);std::int32_t steps=0;
        if(!cvt(a.div(delta,static_cast<float>(v.millisecondsPerStep3F0)),steps)||!a.ok)
            return result(S::arithmetic,0x25b758);
        if(steps>0){
            // Original ADDU/SUBU wraps low32; decode signed without UB.
            const auto raw=static_cast<std::uint32_t>(*v.alpha3E8)+(increasing?
                static_cast<std::uint32_t>(steps):0u-static_cast<std::uint32_t>(steps));
            std::int32_t signedRaw;std::memcpy(&signedRaw,&raw,4);v.alpha3E8=signedRaw;v.lastMilliseconds400=now;
        }
        if((increasing&&*v.alpha3E8>v.targetAlpha3EC)||(decreasing&&*v.alpha3E8<v.targetAlpha3EC))
            v.alpha3E8=v.targetAlpha3EC;
        std::int32_t red=0,green=0,blue=0;const float alpha=static_cast<float>(*v.alpha3E8);
        if(!cvt(a.mul(v.redSlope3F4,alpha),red)||!cvt(a.mul(v.greenSlope3F8,alpha),green)||
            !cvt(a.mul(v.blueSlope3FC,alpha),blue)||!a.ok)return result(S::arithmetic,0x25b7d4);
        d.reached25E610=true;
        d.argb=(v.fromColor3E4+(static_cast<std::uint32_t>(red)<<16)+
            (static_cast<std::uint32_t>(green)<<8)+static_cast<std::uint32_t>(blue))|
            (static_cast<std::uint32_t>(*v.alpha3E8)<<24);
        if(*v.alpha3E8==v.targetAlpha3EC){
            if(increasing){v.covered3DC=1;v.flags404Low6&=static_cast<std::uint8_t>(~1u);}
            else{v.covered3DC=0;v.flags404Low6&=static_cast<std::uint8_t>(~18u);}
        }
        v.flags404Low6|=8u;
    }else v.lastMilliseconds400=now;
    video=v;draw=d;return result(S::complete,0x25b9b8);
}
OriginalCameraStartResult beginOriginalRaceCountdown1AFD80(OriginalRaceStartCountdown& countdown,
    LapTimingManager50ID& manager,std::int32_t app33C,std::int32_t launch,
    const OriginalCourseStartActions& actions) noexcept {
    if(!actions.authenticated())return result(S::owner,0x1ad344);
    if(launch<1||launch>3)return result(S::sourcePath,0x1afe04);
    countdown.countdown34=constant(app33C?0x3e4ccccd:0x40a66666);
    manager.flags58=static_cast<std::uint8_t>(manager.flags58|4u);
    // Source1AD330 sees authenticated Num321Anims0 and makes no101C10 calls.
    return result(S::complete,0x1afddc); //Next actual call is1A9750(false).
}
OriginalCountdownStep advanceOriginalRaceCountdown1AFA84(OriginalRaceStartCountdown& c,
    const LapTimingManager50ID& manager,float delta) noexcept {
    if((manager.flags58&2u)||!(manager.flags58&4u))return OriginalCountdownStep::inactive;
    if(!std::isfinite(c.countdown34))return OriginalCountdownStep::arithmetic;
    if(c.countdown34<=0)return OriginalCountdownStep::needs1AFE30;
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(delta))return OriginalCountdownStep::arithmetic;
    Arithmetic a;const float n=a.sub(c.countdown34,delta);
    if(!a.ok)return OriginalCountdownStep::arithmetic;c.countdown34=n;return OriginalCountdownStep::advanced;
}
OriginalCameraStartResult restoreOriginalRaceFog1A98DC(OriginalRaceFogFields& fog,
    std::uint32_t& rendererColor) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x13e968);
    auto n=fog;Arithmetic a;n.far50=4000;n.near54=constant(0x44a6a99a);n.minimum58=64;n.color30|=0xff000000u;
    const float distance=a.sub(n.far50,n.near54);
    n.end38=a.add(n.near54,a.mul(n.authoredFraction3C,distance));
    n.start34=a.mul(a.sub(1,n.authoredFraction40),a.sub(n.end38,n.minimum58));
    if(!a.ok)return result(S::arithmetic,0x13e988);
    n.dirty60=1;fog=n;rendererColor=n.color30;return result(S::complete,0x13e99c);
}
void revealOriginalRaceUi199770(std::uint8_t& flags) noexcept {flags=static_cast<std::uint8_t>((flags|0x1cu)&~1u);}
OriginalCameraStartResult prepareOriginalRaceViewport2361C0(OriginalCameraViewportFields& destination,
    std::uint64_t generation,std::uint64_t key,CameraSceneState49F& scene) noexcept {
    if(destination.prepared||!generation||!key)return result(S::owner,0x2361c0);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x236208);
    OriginalCameraViewportFields p;Arithmetic a;p.sessionGeneration=generation;p.sceneKey=key;
    p.fullWidth204=p.width1E8=640;p.fullHeight208=p.height1EC=448;p.aspect1DC=constant(0x3faaaaab);
    const float halfW=a.mul(.5f,640),halfH=a.mul(.5f,448),ratio=a.div(448,640);
    p.left318=a.sub(2048,halfW);p.right31C=a.add(2048,halfW);
    p.top320=a.sub(2048,halfH);p.bottom324=a.add(2048,halfH);p.depth314=constant(0x4affffde);
    const float width=a.sub(p.right31C,p.left318),height=a.sub(p.bottom324,p.top320);
    const float aspect=a.div(a.div(height,width),a.mul(p.aspect1DC,ratio));
    const float xx=a.mul(.5f,width),yy=a.mul(.5f,a.sub(p.top320,p.bottom324));
    p.screen140[0]=xx;p.screen140[5]=yy;p.screen140[10]=a.sub(p.depth310,p.depth314);
    p.screen140[12]=a.add(xx,p.left318);p.screen140[13]=a.add(yy,p.bottom324);
    p.screen140[14]=p.depth314;p.screen140[15]=1;
    p.combinedScreen250[0]=p.combinedScreen250[5]=p.combinedScreen250[10]=p.combinedScreen250[15]=1;
    p.guardCombined2D0=p.combinedScreen250;
    if(!guards(a,p)||!a.ok)return result(S::arithmetic,0x237240);
    p.prepared=true;destination=p;scene.aspect1F8=aspect;return result(S::complete,0x23637c);
}
OriginalCameraStartResult setOriginalFullRaceViewport1158A0(OriginalCameraViewportFields& viewport,
    CameraSceneState49F& scene,OriginalCameraSceneTick& tick,std::uint32_t buffers) noexcept {
    if(!viewport.prepared||!tick.bound||tick.sceneKey!=viewport.sceneKey||
       tick.sessionGeneration!=viewport.sessionGeneration)return result(S::owner,0x1158a0);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x1158d4);
    auto p=viewport;Arithmetic a;p.x1E0=p.y1E4=0;p.width1E8=p.fullWidth204;p.height1EC=p.fullHeight208;
    p.depthNear1F0=0;p.depthFar1F4=1;
    const float width=static_cast<float>(p.width1E8),height=static_cast<float>(p.height1EC);
    const float fullRatio=a.div(static_cast<float>(p.fullHeight208),static_cast<float>(p.fullWidth204));
    const float aspect=a.div(a.div(height,width),a.mul(p.aspect1DC,fullRatio));
    if(!a.ok)return result(S::arithmetic,0x11592c);
    viewport=p;scene.aspect1F8=aspect;tick.countdown20C=tick.countdown210=buffers+1u;
    return result(S::complete,0x115948); //Then actual2458C0 platform drain.
}
OriginalCameraStartResult updateOriginalViewportZoom2458C0(const OriginalCameraViewportFields& supplied,
    OriginalCameraViewportFields& current) noexcept {
    if(!supplied.prepared||!current.prepared)return result(S::owner,0x245918);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x245a30);
    Arithmetic a;std::int32_t width=0,height=0;
    if(!cvt(a.sub(current.right31C,current.left318),width)||!cvt(a.sub(current.bottom324,current.top320),height))
        return result(S::arithmetic,0x245a90);
    // Original unsigned viewport lane conversion; admitted fresh dimensions
    // are positive. Do not reinterpret source negative widths as signed ones.
    if(supplied.width1E8<0||supplied.height1EC<0)return result(S::arithmetic,0x245924);
    const float y=a.div(static_cast<float>(supplied.height1EC),static_cast<float>(height));
    const float x=a.div(static_cast<float>(supplied.width1E8),static_cast<float>(width));
    if(!a.ok)return result(S::arithmetic,0x245acc);
    current.zoom328=x;current.zoom32C=y;return result(S::complete,0x245c68);
}
OriginalCameraStartResult rebuildOriginalSceneCore115350(const CameraSceneState49F& scene,
    const Vec& position,std::int32_t width,CameraViewOutput49F& output,float& focal) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x115350);
    Arithmetic a;CameraViewOutput49F next;next.sceneAfter=scene;next.position=position;
    next.view=viewMatrix(a,position,scene.look,scene.up,scene.rollRadians);
    next.projection=projectionMatrix(a,scene);next.combined=multiply(a,next.view,next.projection);
    const float tanValue=tangent(a,a.mul(constant(0x3c8efa34),a.mul(.5f,scene.fovDegrees)));
    if(width<0)return result(S::arithmetic,0x1154b4);
    const float nextFocal=a.div(a.mul(.5f,static_cast<float>(width)),tanValue);
    if(!a.ok)return result(S::arithmetic,0x1154fc);
    output=next;focal=nextFocal;return result(S::complete,0x11550c);
}
OriginalCameraStartResult computeOriginalGuardAngle2371D0(float focal,float& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x2371d0);
    Arithmetic a;float angle=0;std::int32_t angleInt=0;
    if(!originalAngle48G(2048,focal,angle)||!cvt(a.mul(constant(0x42652ee1),angle),angleInt)||
       angleInt>1073741823||angleInt<-1073741824||!a.ok)return result(S::arithmetic,0x237204);
    output=static_cast<float>(angleInt*2);return result(S::complete,0x237220);
}
OriginalCameraStartResult rebuildOriginalRaceScene236DD0(OriginalCameraViewportFields& viewport,
    const CameraSceneState49F& scene,const Vec& position,CameraViewOutput49F& output) noexcept {
    if(!viewport.prepared)return result(S::owner,0x236dd0);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x115350);
    auto p=viewport;Arithmetic a;CameraViewOutput49F v;v.sceneAfter=scene;v.position=position;
    //115350 reads already-owned scene190/1A0 verbatim. It does NOT invoke
    //114F90 or normalize them again; composeCameraView49F would be wrong here.
    v.view=viewMatrix(a,position,scene.look,scene.up,scene.rollRadians);
    v.projection=projectionMatrix(a,scene);v.combined=multiply(a,v.view,v.projection);
    const float tanValue=tangent(a,a.mul(constant(0x3c8efa34),a.mul(.5f,scene.fovDegrees)));
    if(p.width1E8<0)return result(S::arithmetic,0x1154b4);
    p.focalPixels1D8=a.div(a.mul(.5f,static_cast<float>(p.width1E8)),tanValue);
    p.screen140[0]=a.mul(a.mul(.5f,a.sub(p.right31C,p.left318)),p.zoom328);
    p.screen140[5]=a.mul(a.mul(.5f,a.sub(p.top320,p.bottom324)),p.zoom32C);
    float angle=0;std::int32_t angleInt=0;
    if(!originalAngle48G(2048,p.focalPixels1D8,angle)||
       !cvt(a.mul(constant(0x42652ee1),angle),angleInt)||angleInt>1073741823||angleInt<-1073741824)
        return result(S::arithmetic,0x237204);
    p.guardAngle330=static_cast<float>(angleInt*2);
    if(!guards(a,p))return result(S::arithmetic,0x237240);
    p.combinedScreen250=multiply(a,v.combined,p.screen140);
    p.guardProjection290=v.projection;
    for(unsigned row=0;row<4;++row){p.guardProjection290[row*4]=a.mul(p.guardProjection290[row*4],p.guardX334);
        p.guardProjection290[row*4+1]=a.mul(p.guardProjection290[row*4+1],p.guardY338);}
    p.guardCombined2D0=multiply(a,v.view,p.guardProjection290);
    if(!a.ok)return result(S::arithmetic,0x236fe4);
    p.matricesReady=true;viewport=p;output=v;return result(S::complete,0x236fe8);
}
}

namespace rr::game {
namespace {
std::int32_t signedWord(std::uint32_t word) noexcept {
    std::int32_t value;std::memcpy(&value,&word,4);return value;
}
std::int32_t wordAdd(std::int32_t x,std::int32_t y) noexcept {
    return signedWord(static_cast<std::uint32_t>(x)+static_cast<std::uint32_t>(y));
}
std::int32_t wordSub(std::int32_t x,std::int32_t y) noexcept {
    return signedWord(static_cast<std::uint32_t>(x)-static_cast<std::uint32_t>(y));
}
OriginalCameraStartResult videoRequest(OriginalVideoFadeState& video,HostMonotonicClock50ID& clock,
    std::int32_t mode,std::int32_t duration,bool async,bool redraw,bool out) {
    const std::uint32_t pc=out?0x260320:0x260610;
    if(mode==6||(!out&&redraw)||(redraw&&async))return result(S::sourcePath,pc);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,pc);
    auto next=video;
    if(out)next.flags404Low6=static_cast<std::uint8_t>((next.flags404Low6&~32u)|(redraw?32u:0u));
    if(next.flags404Low6&(out?1u:2u)){video=next;return result(S::complete,out?0x260474:0x2606c0);}
    if(!next.alpha3E8)return result(S::unknownVideoAlpha,out?0x260594:0x2607d8);
    next.targetAlpha3EC=static_cast<std::int32_t>(((out?next.toColor3E0:next.fromColor3E4)>>24)&255u);
    next.flags404Low6=static_cast<std::uint8_t>((next.flags404Low6&~3u)|(out?1u:2u));
    Arithmetic a;const float delta=out?
        a.sub(static_cast<float>(next.targetAlpha3EC),static_cast<float>(*next.alpha3E8)):
        a.sub(static_cast<float>(*next.alpha3E8),static_cast<float>(next.targetAlpha3EC));
    if(delta==0)return result(S::arithmetic,out?0x2605b4:0x2607fc);
    //Native admission precedes the effectful clock sample. On the admitted
    //path the source sample precedes the interval computation and final bit2.
    if(!sampledMilliseconds(clock,next.lastMilliseconds400))return result(S::clock,out?0x26057c:0x2607c4);
    if(!cvt(a.add(.5f,a.div(static_cast<float>(duration),delta)),next.millisecondsPerStep3F0)||!a.ok)
        return result(S::arithmetic,out?0x2605bc:0x260804);
    next.flags404Low6=static_cast<std::uint8_t>((next.flags404Low6&~4u)|(async?4u:0u));
    video=next;return result(S::complete,out?0x2605e0:0x260828);
}
bool sameBlockingParameters(const OriginalVideoFadeState& a,const OriginalVideoFadeState& b) noexcept {
    return a.flags404Low6==b.flags404Low6&&a.covered3DC==b.covered3DC&&
        a.fromColor3E4==b.fromColor3E4&&a.toColor3E0==b.toColor3E0&&
        a.alpha3E8==b.alpha3E8&&a.targetAlpha3EC==b.targetAlpha3EC&&
        a.millisecondsPerStep3F0==b.millisecondsPerStep3F0&&
        a.redSlope3F4==b.redSlope3F4&&a.greenSlope3F8==b.greenSlope3F8&&a.blueSlope3FC==b.blueSlope3FC&&
        a.lastMilliseconds400==b.lastMilliseconds400;
}
}
OriginalCameraStartResult requestOriginalVideoFade260320(OriginalVideoFadeState& v,HostMonotonicClock50ID& c,
    std::int32_t mode,std::int32_t duration,bool asynchronous,bool redrawSource){
    return videoRequest(v,c,mode,duration,asynchronous,redrawSource,true);
}
OriginalCameraStartResult requestOriginalVideoFade260610(OriginalVideoFadeState& v,HostMonotonicClock50ID& c,
    std::int32_t mode,std::int32_t duration,bool asynchronous){
    return videoRequest(v,c,mode,duration,asynchronous,false,false);
}
OriginalCameraStartResult requestOriginalFirstMainFade1B7D50(const OriginalVideoFadeState& v,
    std::int32_t& a8,std::int32_t& duration98) noexcept {
    if(a8||!(v.covered3DC||(v.flags404Low6&16u)))return result(S::complete,0x2aef38);
    a8=8;duration98=500;return result(S::complete,0x2aeef4);
}
OriginalCameraStartResult OriginalVideoBlockingFade::advance(OriginalVideoFadeState& video,
    HostMonotonicClock50ID& clock,OriginalVideoBlockingBackend* backend) {
    if(completed_)return result(S::alreadyComplete,0x25aa80);
    if(!backend)return result(S::videoBackend,0x25aa4c);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x260a14);
    if(!video_){
        //Source25A9C0 precedence: blocking OUT first, then blocking IN.
        if(video.flags404Low6&4u)return result(S::sourcePath,0x25aa38);
        if(!(video.flags404Low6&3u))return result(S::sourcePath,0x25aa70);
        direction_=(video.flags404Low6&1u)?OriginalVideoFadeDirection::out260850:OriginalVideoFadeDirection::in261630;
        if(direction_==OriginalVideoFadeDirection::out260850&&(video.flags404Low6&32u))
            return result(S::sourcePath,0x260b34); //Redraw/copy-tiled variant is a separate backend path.
        if(!std::isfinite(video.redSlope3F4)||!std::isfinite(video.greenSlope3F8)||
           !std::isfinite(video.blueSlope3FC)||!std::isfinite(video.lastMilliseconds400))
            return result(S::arithmetic,0x2610e0);
        video_=&video;backend_=backend;parameters_=video;
        startAlpha_=static_cast<std::int32_t>(((direction_==OriginalVideoFadeDirection::out260850?
            video.fromColor3E4:video.toColor3E0)>>24)&255u);
        alpha_=startAlpha_;
    }else if(video_!=&video||backend_!=backend||!sameBlockingParameters(parameters_,video))
        return result(S::owner,0x25aa4c);
    const bool out=direction_==OriginalVideoFadeDirection::out260850;
    for(;;){
        OriginalCameraStartResult r{S::complete,0};Arithmetic a;
        switch(phase_){
        case 0:
            r=backend_->begin(direction_,video);if(r.status!=S::complete)return r;
            ++phase_;break;
        case 1:
            if(!sampledMilliseconds(clock,startMilliseconds_))return result(S::clock,out?0x260ac0:0x2618a4);
            ++phase_;break;
        case 2:
            if(out?alpha_>video.targetAlpha3EC:alpha_<video.targetAlpha3EC){phase_=7;break;}
            phase_=3;break;
        case 3:{
            //GS RGBAQ is R at bit0,G at8,B at16,A at24,Q at32. Original
            //packed colors3E4 are ARGB; preserve that explicit lane swap.
            const auto base=video.fromColor3E4;
            const float delta=static_cast<float>(wordSub(alpha_,startAlpha_));
            std::int32_t red,green,blue;
            if(!cvt(a.add(static_cast<float>((base>>16)&255u),a.mul(video.redSlope3F4,delta)),red)||
               !cvt(a.add(static_cast<float>((base>>8)&255u),a.mul(video.greenSlope3F8,delta)),green)||
               !cvt(a.add(static_cast<float>(base&255u),a.mul(video.blueSlope3FC,delta)),blue)||!a.ok)
                return result(S::arithmetic,out?0x2611e4:0x261a40);
            //Source widens SIGNED int32 lanes before OR, with no masking.
            const auto lane=[](std::int32_t x){return static_cast<std::uint64_t>(static_cast<std::int64_t>(x));};
            OriginalVideoBlockingSample sample{direction_,ordinal_,
                lane(red)|(lane(green)<<8)|(lane(blue)<<16)|(lane(alpha_)<<24)|(std::uint64_t(0x3f800000)<<32),
                alpha_,out?0x262c70u:0x262d80u};
            r=backend_->copyDrawAndComplete(sample);if(r.status!=S::complete)return r;
            ++phase_;break;
        }
        case 4:{
            const auto old=alpha_;alpha_=out?wordAdd(alpha_,step_):wordSub(alpha_,step_);
            if((out?alpha_>video.targetAlpha3EC:alpha_<video.targetAlpha3EC)&&old!=video.targetAlpha3EC)
                alpha_=video.targetAlpha3EC;
            distance_=out?wordSub(alpha_,startAlpha_):wordSub(startAlpha_,alpha_);
            //MULT writes low32 into v0. Native signed overflow is forbidden.
            const auto product=static_cast<std::uint32_t>(video.millisecondsPerStep3F0)*static_cast<std::uint32_t>(distance_);
            waitUntilDelta_=static_cast<float>(signedWord(product));
            phase_=5;break;
        }
        case 5:{
            float now;if(!sampledMilliseconds(clock,now))return result(S::clock,out?0x26137c:0x261bdc);
            const float elapsed=a.sub(now,startMilliseconds_);if(!a.ok)return result(S::arithmetic,0x261394);
            //This adaptive increment occurs ONCE immediately after the draw,
            //not on each polling call in the following busy-wait translation.
            if(elapsed>waitUntilDelta_)step_=wordAdd(step_,1);
            if(elapsed<waitUntilDelta_){phase_=6;return result(S::videoClockWait,out?0x261494:0x261cec);}
            ++ordinal_;phase_=2;
            break;
        }
        case 6:{
            float now;if(!sampledMilliseconds(clock,now))return result(S::clock,out?0x261464:0x261cbc);
            const float elapsed=a.sub(now,startMilliseconds_);if(!a.ok)return result(S::arithmetic,0x261470);
            if(elapsed<waitUntilDelta_)return result(S::videoClockWait,out?0x261494:0x261cec);
            ++ordinal_;phase_=2;break;
        }
        case 7:
            r=backend_->restoreAndComplete(direction_);if(r.status!=S::complete)return r;
            phase_=8;break;
        case 8:
            //The source draws target itself, advances once again, and stores
            //the OVERSHOOT. Do not clamp this final shared alpha to0 or128.
            video.alpha3E8=alpha_;
            if(out)video.flags404Low6=static_cast<std::uint8_t>((video.flags404Low6&~9u)|16u);
            else{video.covered3DC=0;video.flags404Low6&=static_cast<std::uint8_t>(~18u);}
            completed_=true;return result(S::complete,out?0x2615e4:0x261e30);
        default:return result(S::owner,0x25aa4c);
        }
    }
}
}
