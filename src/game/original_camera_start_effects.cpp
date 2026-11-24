#include "original_camera_start_effects.h"
#include "original_race_fog_far50nf.h"
#include "original_video_blocking_color50la.h"
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
// Private body: only retained caller cursors below may execute this once.
bool sameRequestVideoFields(const OriginalVideoFadeState&,const OriginalVideoFadeState&)noexcept;
OriginalCameraStartResult orderedVideoRequestPrefix(OriginalVideoFadeState&,HostMonotonicClock50ID&,
    std::int32_t mode,std::int32_t duration,bool asynchronous,bool out)noexcept;
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
OriginalCameraStartResult OriginalCameraExitFadeRequest2AED60::invoke(std::int32_t a8,
    std::int32_t appMode,OriginalVideoFadeState& video,HostMonotonicClock50ID& clock)noexcept {
    if(video_){
        if(video_!=&video||clock_!=&clock||epoch_!=clock.epoch||manager_!=a8||mode_!=appMode||
           !sameRequestVideoFields(expected_,video))return result(S::owner,0x2aed60);
        return result_.status==S::complete?result(S::alreadyComplete,result_.sourcePC):result_;
    }
    video_=&video;clock_=&clock;epoch_=clock.epoch;manager_=a8;mode_=appMode;
    const auto stop=[&](OriginalCameraStartResult r){expected_=video;result_=r;return r;};
    if(a8!=0)return stop(result(S::complete,0x2aee60)); //Source v0=0; no child call.
    std::int32_t duration=500;
    if(video.covered3DC||(video.flags404Low6&16u)){ //Actual virtual+40=260300.
        if(!video.alpha3E8)return stop(result(S::unknownVideoAlpha,0x2aedd4));
        // Native arithmetic admission only when the source enters this branch.
        if(std::fegetround()!=FE_TONEAREST)return stop(result(S::arithmetic,0x2aedd8));
        volatile float durationValue=static_cast<float>(duration); //2AEDD8.
        volatile float alphaValue=static_cast<float>(*video.alpha3E8); //2AEDDC.
        Arithmetic a;const float scale=a.sub(1,a.div(alphaValue,128));
        if(!cvt(a.mul(scale,durationValue),duration)||!a.ok)return stop(result(S::arithmetic,0x2aedec));
        if(duration==0)return stop(result(S::complete,0x2aee00)); //Source v0=1; no child/wait.
    }
    // Source2AEE38 calls260320 with a2=0,a3=0. Its ordered partial effects
    // survive failure. T0=1 callers separately own the subsequent260850 wait.
    return stop(orderedVideoRequestPrefix(video,clock,appMode,duration,false,true));
}
OriginalCameraStartResult OriginalResultsBlockingFade2AED60::advance(std::int32_t a8,
    std::int32_t appMode,OriginalVideoFadeState& video,HostMonotonicClock50ID& clock,
    OriginalVideoBlockingBackend* backend) {
    if(requested_&&(video_!=&video||clock_!=&clock||epoch_!=clock.epoch||backend_!=backend))
        return result(S::owner,0x2aee48);
    if(finished_)return result(S::alreadyComplete,0x2aee60);
    if(!requested_){
        // Seal the actual borrowed identities BEFORE the request. Failed
        // requests are terminal here: never repeat a source call/clock sample
        // on a later poll. The owning transaction must cancel on failure.
        video_=&video;clock_=&clock;epoch_=clock.epoch;backend_=backend;requested_=true;
        // This shares ONLY the exact request arithmetic/260320(false,false).
        // Unlike Camera exit's T0=0 caller, Results T0=1 immediately waits.
        requestResult_=request_.invoke(a8,appMode,video,clock);
        if(requestResult_.status!=S::complete)return requestResult_;
        if(requestResult_.sourcePC==0x2aee60||requestResult_.sourcePC==0x2aee00){
            finished_=true;return requestResult_;
        }
    }
    if(requestResult_.status!=S::complete)return request_.invoke(a8,appMode,video,clock);
    // No flag-only or elapsed-time-only completion. The existing concrete
    //260850 cursor captures the accepted image, presents each real sample,
    //then requires restoreAndComplete before it can return complete.
    const auto r=fade_.advance(video,clock,backend);
    if(r.status==S::complete)finished_=true;
    return r;
}
bool OriginalVideoFadeUpdate25B5D0::sameFields(const OriginalVideoFadeState& a,
    const OriginalVideoFadeState& b)noexcept {
    const auto sameFloat=[](const float& x,const float& y)noexcept{return std::memcmp(&x,&y,sizeof(float))==0;};
    return a.covered3DC==b.covered3DC&&a.flags404Low6==b.flags404Low6&&
        a.toColor3E0==b.toColor3E0&&a.fromColor3E4==b.fromColor3E4&&a.alpha3E8==b.alpha3E8&&
        a.targetAlpha3EC==b.targetAlpha3EC&&a.millisecondsPerStep3F0==b.millisecondsPerStep3F0&&
        sameFloat(a.redSlope3F4,b.redSlope3F4)&&sameFloat(a.greenSlope3F8,b.greenSlope3F8)&&
        sameFloat(a.blueSlope3FC,b.blueSlope3FC)&&sameFloat(a.lastMilliseconds400,b.lastMilliseconds400);
}
bool OriginalVideoFadeUpdate25B5D0::current(const OriginalVideoFadeState& video,
    const HostMonotonicClock50ID& clock)const noexcept {
    return video_==&video&&clock_==&clock&&epoch_==clock.epoch&&sameFields(expected_,video);
}
OriginalCameraStartResult OriginalVideoFadeUpdate25B5D0::advance(OriginalVideoFadeState& video,
    HostMonotonicClock50ID& clock,std::int32_t view,std::int32_t count,OriginalVideoFadeDraw& draw)noexcept {
    draw={};
    if(!video_){video_=&video;clock_=&clock;epoch_=clock.epoch;view_=view;count_=count;expected_=video;}
    else if(!current(video,clock)||view_!=view||count_!=count)return result(S::owner,0x25b5d0);
    const auto failPrefix=[&](S status,std::uint32_t pc){
        expected_=video;phase_=Phase::failed;result_=result(status,pc);return result_;
    };
    const auto finish=[&](){expected_=video;phase_=Phase::complete;result_=result(S::complete,0x25b9b8);return result_;};
    const auto drawBoundary=[&](){
        expected_=video;phase_=Phase::draw;draw=draw_;result_=result(S::videoBackend,0x25e610);return result_;
    };
    if(phase_==Phase::failed||phase_==Phase::complete)return result_;
    if(phase_==Phase::draw){draw=draw_;return result_;}
    if(phase_==Phase::fresh){
        if(std::int64_t(view)<std::int64_t(count)-1||(video.flags404Low6&8u))return finish();
        //25B654 occurs BEFORE the actual clock sample in this covered path.
        if(video.covered3DC&&!(video.flags404Low6&3u)){
            draw_={true,video.toColor3E0};return_=DrawReturn::covered;return drawBoundary();
        }
        phase_=Phase::clock;
    }
    float now=0;
    if(!sampleHostFloatMilliseconds50ID(clock,now))return failPrefix(S::clock,0x25b718);
    //25B728/734 read400 and subtract only AFTER the shared clock sample.
    Arithmetic a;const float delta=a.sub(now,video.lastMilliseconds400);
    if(!a.ok)return failPrefix(S::arithmetic,0x25b728);
    const bool increasing=(video.flags404Low6&5u)==5u;
    const bool decreasing=!increasing&&(video.flags404Low6&6u)==6u;
    if(!increasing&&!decreasing){video.lastMilliseconds400=now;return finish();} //25B9B4.
    if(video.millisecondsPerStep3F0==0)return failPrefix(S::arithmetic,increasing?0x25b750:0x25b88c);
    std::int32_t steps=0;
    if(!cvt(a.div(delta,static_cast<float>(video.millisecondsPerStep3F0)),steps)||!a.ok)
        return failPrefix(S::arithmetic,increasing?0x25b758:0x25b894);
    if(!video.alpha3E8)return failPrefix(S::unknownVideoAlpha,increasing?
        (steps>0?0x25b76c:0x25b77c):(steps>0?0x25b8a8:0x25b8b8));
    if(steps>0){
        const std::uint32_t raw=std::uint32_t(*video.alpha3E8)+(increasing?
            std::uint32_t(steps):0u-std::uint32_t(steps));
        std::int32_t next;std::memcpy(&next,&raw,sizeof(next));
        video.alpha3E8=next; //25B774 /25B8B0, before400.
        video.lastMilliseconds400=now; //25B778 /25B8B4. Preserve on later failure.
    }
    if((increasing&&*video.alpha3E8>video.targetAlpha3EC)||
       (decreasing&&*video.alpha3E8<video.targetAlpha3EC))video.alpha3E8=video.targetAlpha3EC;
    const float alpha=static_cast<float>(*video.alpha3E8);
    std::int32_t red=0,green=0,blue=0;
    // Async source order differs from the blocking color evaluator: convert
    // red, then compute green/blue products, then convert green and blue.
    if(!cvt(a.mul(video.redSlope3F4,alpha),red)||!a.ok)
        return failPrefix(S::arithmetic,increasing?0x25b7d4:0x25b910);
    const float greenValue=a.mul(video.greenSlope3F8,alpha),blueValue=a.mul(video.blueSlope3FC,alpha);
    if(!cvt(greenValue,green))return failPrefix(S::arithmetic,increasing?0x25b7e4:0x25b920);
    if(!cvt(blueValue,blue)||!a.ok)return failPrefix(S::arithmetic,increasing?0x25b7f4:0x25b930);
    // Full32-bit ADD packing, including base alpha/carries, then OR alpha.
    // Do not reuse blocking signed64-bit OR packing or clean byte clamps.
    draw_={true,(video.fromColor3E4+(std::uint32_t(red)<<16)+
        (std::uint32_t(green)<<8)+std::uint32_t(blue))|(std::uint32_t(*video.alpha3E8)<<24)};
    return_=increasing?DrawReturn::increasing:DrawReturn::decreasing;
    return drawBoundary(); //No post25E610 completion flags yet.
}
OriginalCameraStartResult OriginalVideoFadeUpdate25B5D0::afterNativeDrawComplete(
    OriginalVideoFadeState& video,HostMonotonicClock50ID& clock)noexcept {
    if(!current(video,clock))return result(S::owner,0x25e610);
    if(phase_==Phase::complete||phase_==Phase::failed)return result_;
    if(phase_!=Phase::draw)return result(S::owner,0x25e610);
    if(return_==DrawReturn::covered){
        video.flags404Low6=std::uint8_t(video.flags404Low6|8u); //25B670, before clock.
        expected_=video;phase_=Phase::clock;draw_={};OriginalVideoFadeDraw unused;
        return advance(video,clock,view_,count_,unused);
    }
    // Prefix already proved/read alpha; exact expected fields remain sealed.
    if(!video.alpha3E8)return result(S::owner,0x25e610);
    if(*video.alpha3E8==video.targetAlpha3EC){
        if(return_==DrawReturn::increasing){
            video.covered3DC=1; //25B828.
            video.flags404Low6=std::uint8_t(video.flags404Low6&~1u); //25B840.
        }else{
            video.covered3DC=0; //25B960.
            video.flags404Low6=std::uint8_t(video.flags404Low6&~16u); //25B980.
            video.flags404Low6=std::uint8_t(video.flags404Low6&~2u); //25B994.
        }
    }
    video.flags404Low6=std::uint8_t(video.flags404Low6|8u); //25B85C /25B9B0.
    expected_=video;phase_=Phase::complete;result_=result(S::complete,0x25b9b8);return result_;
}
OriginalCameraStartResult advanceOriginalVideoFade25B5D0(OriginalVideoFadeState& video,
    HostMonotonicClock50ID& clock,std::int32_t view,std::int32_t count,OriginalVideoFadeDraw& draw) noexcept {
    // Garage's real caller fails terminally when25E610 has no draw owner.
    // It cannot consume post-call flags or repeatedly resample this boundary.
    OriginalVideoFadeUpdate25B5D0 prefix;return prefix.advance(video,clock,view,count,draw);
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
    std::uint32_t& rendererColor,bool split) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x13e968);
    // far50: the original constant 4000 unless the course's Fog block authors the
    // N64-canonical FarDistance (DLC only; stock never does, so stock stays 4000).
    auto n=fog;Arithmetic a;n.far50=split?2000.f:raceFogFarDistance50NF();n.near54=split?2000.f:constant(0x44a6a99a);n.minimum58=64;n.color30|=0xff000000u;
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
    CameraSceneState49F& scene,OriginalCameraSceneTick& tick,std::uint32_t buffers,std::int32_t mode,unsigned view) noexcept {
    if(!viewport.prepared||!tick.bound||tick.sceneKey!=viewport.sceneKey||
       tick.sessionGeneration!=viewport.sessionGeneration)return result(S::owner,0x1158a0);
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x1158d4);
    auto p=viewport;Arithmetic a;p.x1E0=p.y1E4=0;p.width1E8=p.fullWidth204;p.height1EC=p.fullHeight208;
    if(mode<0||mode>2||view>1||(!mode&&view))return result(S::sourcePath,0x1a9840);
    if(mode==1){
        p.height1EC=p.fullHeight208/2;
        if(view)p.y1E4=p.height1EC;
    }else if(mode==2){
        p.width1E8=p.fullWidth204/2;
        if(view)p.x1E0=p.width1E8;
    }
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
bool sameRequestVideoFields(const OriginalVideoFadeState& a,const OriginalVideoFadeState& b)noexcept {
        const auto sameFloat=[](const float& x,const float& y)noexcept{return std::memcmp(&x,&y,sizeof(float))==0;};
        return a.covered3DC==b.covered3DC&&a.flags404Low6==b.flags404Low6&&
            a.toColor3E0==b.toColor3E0&&a.fromColor3E4==b.fromColor3E4&&a.alpha3E8==b.alpha3E8&&
            a.targetAlpha3EC==b.targetAlpha3EC&&a.millisecondsPerStep3F0==b.millisecondsPerStep3F0&&
            sameFloat(a.redSlope3F4,b.redSlope3F4)&&sameFloat(a.greenSlope3F8,b.greenSlope3F8)&&
            sameFloat(a.blueSlope3FC,b.blueSlope3FC)&&sameFloat(a.lastMilliseconds400,b.lastMilliseconds400);
}
OriginalCameraStartResult orderedVideoRequestPrefix(OriginalVideoFadeState& video,HostMonotonicClock50ID& clock,
    std::int32_t mode,std::int32_t duration,bool asynchronous,bool out)noexcept {
    if(mode==6)return result(S::sourcePath,out?0x260354:0x26063c); //Unowned assertion path.
    if(out)video.flags404Low6=std::uint8_t(video.flags404Low6&~32u); //260468, before no-op.
    if(video.flags404Low6&(out?1u:2u))return result(S::complete,out?0x2605e4:0x26082c);
    video.targetAlpha3EC=std::int32_t(((out?video.toColor3E0:video.fromColor3E4)>>24)&255u);
    if(out){
        video.flags404Low6=std::uint8_t(video.flags404Low6|1u); //2604B4.
        video.flags404Low6=std::uint8_t(video.flags404Low6&~2u); //2604C4.
    }else{
        video.flags404Low6=std::uint8_t(video.flags404Low6&~1u); //2606FC.
        video.flags404Low6=std::uint8_t(video.flags404Low6|2u); //26070C.
    }
    // Native numeric admission is at the reached sample point, after stores.
    const auto clockPC=out?0x26057cu:0x2607c4u;
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,clockPC);
    float now=0;if(!sampleHostFloatMilliseconds50ID(clock,now))return result(S::clock,clockPC);
    video.lastMilliseconds400=now; //26058C /2607D4, BEFORE unknown-alpha read.
    if(!video.alpha3E8)return result(S::unknownVideoAlpha,out?0x260594:0x2607d8);
    // Preserve original separately rounded duration/left/right conversions.
    volatile float durationValue=static_cast<float>(duration);
    volatile float left=static_cast<float>(out?video.targetAlpha3EC:*video.alpha3E8);
    volatile float right=static_cast<float>(out?*video.alpha3E8:video.targetAlpha3EC);
    Arithmetic a;const float delta=a.sub(left,right);
    if(delta==0)return result(S::arithmetic,out?0x2605b4:0x2607fc);
    std::int32_t interval=0;
    if(!cvt(a.add(.5f,a.div(durationValue,delta)),interval)||!a.ok)
        return result(S::arithmetic,out?0x2605bc:0x260804);
    video.millisecondsPerStep3F0=interval; //2605C8 /260810. Zero is not rejected here.
    video.flags404Low6=std::uint8_t((video.flags404Low6&~4u)|(asynchronous?4u:0u)); //2605E0 /260828, LAST.
    return result(S::complete,out?0x2605e4:0x26082c);
}

}
OriginalCameraStartResult OriginalLoadingFadePrefix17C5C8::advance(OriginalVideoFadeState& video,
    HostMonotonicClock50ID& clock,std::int32_t mode)noexcept {
    if(mode==6)return result(S::sourcePath,0x260354);
    //26044C..468 precedes even the original bit0 no-op branch.
    video.flags404Low6=static_cast<std::uint8_t>(video.flags404Low6&~32u);
    if(video.flags404Low6&1u)return result(S::complete,0x2605e4);
    video.targetAlpha3EC=static_cast<std::int32_t>((video.toColor3E0>>24u)&255u); //260498.
    video.flags404Low6=static_cast<std::uint8_t>(video.flags404Low6|1u); //2604B4.
    video.flags404Low6=static_cast<std::uint8_t>(video.flags404Low6&~2u); //2604C4.
    // Native numeric admission at the actual sample point, not before stores.
    if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x26057c);
    float now=0;if(!sampleHostFloatMilliseconds50ID(clock,now))return result(S::clock,0x26057c);
    video.lastMilliseconds400=now; //26058C BEFORE260594 input read.
    return result(video.alpha3E8?S::sourcePath:S::unknownVideoAlpha,0x260594);
}
OriginalCameraStartResult OriginalVideoAsyncRequest50LA::invoke(OriginalVideoFadeDirection direction,
    OriginalVideoFadeState& video,HostMonotonicClock50ID& clock,std::int32_t mode,std::int32_t duration)noexcept {
    if(direction!=OriginalVideoFadeDirection::out260850&&direction!=OriginalVideoFadeDirection::in261630)
        return result(S::owner,0x260320);
    const bool out=direction==OriginalVideoFadeDirection::out260850;
    const auto pc=out?0x260320u:0x260610u;
    if(video_){
        if(video_!=&video||clock_!=&clock||epoch_!=clock.epoch||direction_!=direction||mode_!=mode||duration_!=duration||
           !sameRequestVideoFields(expected_,video))return result(S::owner,pc);
        return result_.status==S::complete?result(S::alreadyComplete,result_.sourcePC):result_;
    }
    // Seal the exact invocation BEFORE any source effect, including no-op
    // OUT's redraw-bit clear. There is no implicit reset or replacement.
    video_=&video;clock_=&clock;epoch_=clock.epoch;direction_=direction;mode_=mode;duration_=duration;
    result_=orderedVideoRequestPrefix(video,clock,mode,duration,true,out);
    expected_=video;return result_;
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
        video_=&video;backend_=backend;clock_=&clock;clockEpoch_=clock.epoch;parameters_=video;
        startAlpha_=static_cast<std::int32_t>(((direction_==OriginalVideoFadeDirection::out260850?
            video.fromColor3E4:video.toColor3E0)>>24)&255u);
        alpha_=startAlpha_;
    }else if(video_!=&video||backend_!=backend||clock_!=&clock||clockEpoch_!=clock.epoch||
             !sameRequestVideoFields(parameters_,video))
        return result(S::owner,0x25aa4c);
    const bool out=direction_==OriginalVideoFadeDirection::out260850;
    for(;;){
        // Recheck the borrowed Video/clock after every synchronous backend
        // return, before more source work. A completed backend phase has
        // already advanced, so restored admission never repeats its effects.
        // The real native backend does not modify these source fields; this
        // guards custody across the abstract backend boundary as well.
        if(clock_!=&clock||clockEpoch_!=clock.epoch||!sameRequestVideoFields(parameters_,video))
            return result(S::owner,0x25aa4c);
        // A completed backend can change host rounding before the next phase.
        // Recheck native numeric admission BEFORE counter advancement,
        // post-clock deadline CVT.S.W (2613A4/261C00), or other phase work. The
        // already-completed backend phase stays advanced: retrying with the
        // required rounding never repeats that draw/begin/restoration.
        if(std::fegetround()!=FE_TONEAREST)return result(S::arithmetic,0x260a14);
        OriginalCameraStartResult r{S::complete,0};Arithmetic a;
        switch(phase_){
        case 0:
            r=backend_->begin(direction_,video);if(r.status!=S::complete)return r;
            ++phase_;break;
        case 1:
            if(!sampleHostFloatMilliseconds50ID(clock,startMilliseconds_))return result(S::clock,out?0x260ac0:0x2618a4);
            ++phase_;break;
        case 2:
            if(out?alpha_>video.targetAlpha3EC:alpha_<video.targetAlpha3EC){phase_=7;break;}
            phase_=3;break;
        case 3:{
            // Share only the numeric body. This reached owner still selects
            // start/current alpha and exclusively constructs the source sample.
            const OriginalVideoFadeColorCoefficients50LA color{
                video.fromColor3E4,video.redSlope3F4,video.greenSlope3F8,video.blueSlope3FC};
            std::uint64_t rgbaq=0;
            if(!computeOriginalVideoBlockingColor50LA(color,startAlpha_,alpha_,rgbaq))
                return result(S::arithmetic,out?0x2611e4:0x261a40);
            OriginalVideoBlockingSample sample{direction_,ordinal_,rgbaq,
                alpha_,out?0x262c70u:0x262d80u};
            r=backend_->copyDrawAndComplete(sample);if(r.status!=S::complete)return r;
            ++phase_;break;
        }
        case 4:{
            const auto old=alpha_;alpha_=out?wordAdd(alpha_,step_):wordSub(alpha_,step_);
            if((out?alpha_>video.targetAlpha3EC:alpha_<video.targetAlpha3EC)&&old!=video.targetAlpha3EC)
                alpha_=video.targetAlpha3EC;
            //2612B8/261B18 counter and clamp precede the clock. Distance,
            //interval read and deadline conversion occur only AFTER its return.
            phase_=5;break;
        }
        case 5:{
            float now;if(!sampleHostFloatMilliseconds50ID(clock,now))return result(S::clock,out?0x26137c:0x261bdc);
            //26138C/261BEC follows the actual clock call. Preserve the
            //source elapsed subtraction before MULT/CVT.S.W deadline work.
            std::int32_t interval=0;
            if(out){
                distance_=wordSub(alpha_,startAlpha_); //26138C.
                interval=video.millisecondsPerStep3F0; //261390.
            }else{
                interval=video.millisecondsPerStep3F0; //261BE4.
                distance_=wordSub(startAlpha_,alpha_); //261BEC.
            }
            const float elapsed=a.sub(now,startMilliseconds_);
            if(!a.ok)return result(S::arithmetic,out?0x261394:0x261bf0);
            //MULT retains low32; signed host overflow is never used.
            const auto product=std::uint32_t(interval)*std::uint32_t(distance_);
            volatile float deadline=static_cast<float>(signedWord(product)); //2613A4 /261C00.
            //This adaptive increment occurs ONCE immediately after the draw,
            //not on each polling call in the following busy-wait translation.
            if(elapsed>deadline)step_=wordAdd(step_,1);
            if(elapsed<deadline){phase_=6;return result(S::videoClockWait,out?0x261494:0x261cec);}
            ++ordinal_;phase_=2;
            break;
        }
        case 6:{
            float now;if(!sampleHostFloatMilliseconds50ID(clock,now))return result(S::clock,out?0x261464:0x261cbc);
            const float elapsed=a.sub(now,startMilliseconds_);
            if(!a.ok)return result(S::arithmetic,out?0x261470:0x261cc8);
            //Each original wait iteration reloads3F0 and rebuilds its
            //deadline AFTER the sample/subtraction (261478..488/261CD0..CE0).
            const auto product=std::uint32_t(video.millisecondsPerStep3F0)*std::uint32_t(distance_);
            volatile float deadline=static_cast<float>(signedWord(product));
            if(elapsed<deadline)return result(S::videoClockWait,out?0x261494:0x261cec);
            ++ordinal_;phase_=2;break;
        }
        case 7:
            r=backend_->restoreAndComplete(direction_);if(r.status!=S::complete)return r;
            phase_=8;break;
        case 8:
            //The source draws target itself, advances once again, and stores
            //the OVERSHOOT. Do not clamp this final shared alpha to0 or128.
            //One callback-free source return phase, AFTER actual backend
            //restoration. Do not introduce an artificial yield between stores.
            if(out){
                video.flags404Low6=std::uint8_t(video.flags404Low6&~1u); //2615B4.
                video.alpha3E8=alpha_; //2615B8.
                video.flags404Low6=std::uint8_t(video.flags404Low6&~8u); //2615D4.
                video.flags404Low6=std::uint8_t(video.flags404Low6|16u); //2615E4.
            }else{
                video.covered3DC=0; //261DF8.
                video.flags404Low6=std::uint8_t(video.flags404Low6&~2u); //261E18.
                video.alpha3E8=alpha_; //261E1C.
                video.flags404Low6=std::uint8_t(video.flags404Low6&~16u); //261E30.
            }
            completed_=true;return result(S::complete,out?0x2615e4:0x261e30);
        default:return result(S::owner,0x25aa4c);
        }
    }
}
}
