#include "original_follow_camera.h"
#include "n64_pillar_camera_shake50nh.h"
#include "chariot_scene49u.h"
#include "scalar_minmax48o.h"
#include "../assets/pod_physics49n.h"
#include "native_zero_gravity_step50zg.h"
#include <cfenv>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using V=OriginalFollowVector;using C=OriginalFollowCamera;
using Status=OriginalFollowStatus;using Result=OriginalFollowResult;
Result fail(Status s,std::uint32_t pc,std::size_t i=0) noexcept {return {s,pc,i};}
// The follow camera is the player's (racer slot 0, as its only caller in the
// race session visits slot 0). FLAG: keyed on the camera's racer, not on a
// course or pod. True only in zero-g Flight / Recover for that racer.
bool zeroGravityCameraFollowsAttitude50ZG() noexcept {
    return zeroGravityPublishedMode50ZG[0]!=ZeroGravityMode50ZG::Normal;
}
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
bool scalar(float x) noexcept {
    std::uint32_t u;std::memcpy(&u,&x,4);const auto e=u&0x7f800000u;
    return e!=0x7f800000u&&(e!=0||(u&0x7fffffu)==0);
}
struct Math {
    bool ok=true;
    float keep(float a) noexcept {ok=ok&&scalar(a);return a;}
    bool input(float a,float b) noexcept {ok=ok&&scalar(a)&&scalar(b);return ok;}
    float add(float a,float b) noexcept {if(!input(a,b))return 0;volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {if(!input(a,b))return 0;volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {if(!input(a,b))return 0;volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {if(!input(a,b)||b==0){ok=false;return 0;}volatile float r=a/b;return keep(r);}
    float root(float a) noexcept {if(!input(a,0)||a<0){ok=false;return 0;}volatile float r=std::sqrt(a);return keep(r);}
    float min(float a,float b) noexcept {if(!input(a,b))return 0;return detail::scalarMin48O(a,b);}
    float max(float a,float b) noexcept {if(!input(a,b))return 0;return detail::scalarMax48O(a,b);}
    V normalize(V a) noexcept {
        const float x=mul(a.x,a.x),y=mul(a.y,a.y),z=mul(a.z,a.z);
        const float sum=add(add(x,y),mul(1.f,z));
        const float q=sum==0?std::numeric_limits<float>::max():div(1,root(sum));
        return {mul(a.x,q),mul(a.y,q),mul(a.z,q),add(a.w,0)};
    }
    V cross(V a,V b) noexcept {
        return {sub(mul(a.y,b.z),mul(a.z,b.y)),sub(mul(a.z,b.x),mul(a.x,b.z)),sub(mul(a.x,b.y),mul(a.y,b.x)),0};
    }
    float filter(OriginalFollowFilter& f,float target,float dt) noexcept {
        f.blend=div(min(dt,f.tau),f.tau);
        f.value=add(f.value,mul(f.blend,sub(target,f.value)));return f.value;
    }
    V filters(std::array<OriginalFollowFilter,3>& f,V a,float dt) noexcept {
        return {filter(f[0],a.x,dt),filter(f[1],a.y,dt),filter(f[2],a.z,dt),a.w};
    }
};
bool identity(const OriginalFollowIdentity& i) noexcept {
    return i.sessionGeneration&&i.racerKey&&i.mainBodyKey&&i.containerKey;
}
bool bound(const C& c) noexcept {
    return c.prepared&&c.sessionGeneration&&c.racerKey&&c.mainBodyKey&&c.containerKey&&
        c.bank.memberIdentity&&c.bank.records&&c.bank.count&&c.current100<c.bank.count;
}
bool same(const char* a,const char* b) noexcept {
    if(!a||!b)return false;
    for(;;++a,++b){const char x=*a>='A'&&*a<='Z'?char(*a+32):*a;
        const char y=*b>='A'&&*b<='Z'?char(*b+32):*b;if(x!=y)return false;if(!x)return true;}
}
const OriginalFollowAuthoredSection* section(const OriginalFollowCamSource& s,const char* name) noexcept {
    const OriginalFollowAuthoredSection* found=nullptr;
    for(std::size_t i=0;i<s.sectionCount;++i)if(same(s.sections[i].name,name)){
        if(found)return nullptr;found=&s.sections[i];}
    return found;
}
const OriginalFollowAuthoredValue* value(const OriginalFollowAuthoredSection& s,const char* key) noexcept {
    if(!s.values&&s.count)return nullptr;const OriginalFollowAuthoredValue* found=nullptr;
    for(std::size_t i=0;i<s.count;++i)if(same(s.values[i].key,key)){
        if(found)return nullptr;found=&s.values[i];}
    return found;
}
bool integer(const OriginalFollowAuthoredSection& s,const char* key,std::int32_t& out) noexcept {
    const auto* v=value(s,key);if(!v||!v->value||!v->valueBytes||v->valueBytes>11)return false;
    std::size_t i=0;bool neg=false;if(v->value[0]=='-'||v->value[0]=='+'){neg=v->value[0]=='-';++i;}
    if(i==v->valueBytes)return false;
    //158920 accepts initialT/F/Y/N and wholeON/OFF, then base0 integer.
    //The authenticated CAM set uses T/F and ordinary decimal IDs/counts.
    if(!neg&&i==0){const char ch=v->value[0];
        if(ch=='T'||ch=='t'||ch=='Y'||ch=='y'){out=1;return true;}
        if(ch=='F'||ch=='f'||ch=='N'||ch=='n'){out=0;return true;}}
    std::uint64_t n=0;
    for(;i<v->valueBytes;++i){const char c=v->value[i];if(c<'0'||c>'9')return false;
        n=n*10+unsigned(c-'0');if(n>(neg?2147483648ull:2147483647ull))return false;}
    out=neg?static_cast<std::int32_t>(-static_cast<std::int64_t>(n)):static_cast<std::int32_t>(n);return true;
}
bool number(const OriginalFollowAuthoredSection& s,const char* key,float& out) noexcept {
    const auto* v=value(s,key);return v&&assets::parsePodPhysicsNumber49N(v->value,v->valueBytes,out);
}
void selected(C& c,std::size_t index) noexcept {
    if(c.bank.records[c.current100].filter28==0&&c.bank.records[index].filter28==1)c.snap10C=true;
    c.current100=index;const auto& p=c.bank.records[index];
    c.angleY08=p.angleY04;c.angleX0C=p.angleX08;c.distance04=p.distance0C;
    c.fov10=p.fov10;c.cameraSwing14=p.cameraSwing38;c.lookSwing18=p.lookSwing3C;
    c.distanceOffset58=0;c.yawOffset5C=0;c.pitchOffset60=0;c.fovOffset64=0;
}
float wrapped(float x,Math& m) noexcept {
    m.keep(x);if(!m.ok)return 0;
    //13C8B0: source software-double fmod with the widened originalfloat2*pi.
    volatile float r=static_cast<float>(std::fmod(double(x),double(bits(0x40c90fdbu))));
    float y=m.keep(r);if(y>bits(0x40490fdbu))y=m.sub(y,bits(0x40c90fdbu));
    else if(y<bits(0xc0490fdbu))y=m.add(y,bits(0x40c90fdbu));return y;
}
void updateAngles(C& c,const OriginalFollowPreset& p,float swing,Math& m) noexcept {
    c.angleY08=m.sub(p.angleY04,c.yawOffset5C);
    c.angleY08=wrapped(m.sub(c.angleY08,m.mul(c.cameraSwing14,swing)),m);
    c.angleX0C=m.sub(m.add(c.pitchOffset60,p.angleX08),c.reverseAngle78);
    const float lo=bits(0xbfbde464u),hi=bits(0x3fbde464u);
    if(c.angleX0C<lo){c.angleX0C=lo;c.pitchOffset60=m.add(c.reverseAngle78,m.sub(lo,p.angleX08));}
    else if(c.angleX0C>hi){c.angleX0C=hi;c.pitchOffset60=m.sub(m.sub(hi,p.angleX08),c.reverseAngle78);}
}
bool rotate(V axis,V in,float angle,V& out) noexcept {
    VisualVector49R a{{axis.x,axis.y,axis.z,axis.w}},b{{in.x,in.y,in.z,in.w}},r{{out.x,out.y,out.z,out.w}};
    if(!detail::rotateVector49U(a,b,angle,r))return false;
    out={r.v[0],r.v[1],r.v[2],r.v[3]};return true;
}
float nextUnit(std::uint32_t& raw,Math& m) noexcept {
    raw=raw*0x41c64e6du+0x3039u;
    volatile float f=static_cast<float>(static_cast<std::int32_t>(raw&0x7fffffffu));
    return m.mul(bits(0x30000000u),f);
}
Result finish(C& c,OriginalFollowPoseWork& work,const OriginalFollowCollisionResult& hit,
    CoupledNoiseContext49M& random,TVCameraUpdateOutput49F& output) noexcept {
    if(!work.pending||!bound(c)||c.revision!=work.baseRevision||
        c.sessionGeneration!=work.after.sessionGeneration||c.racerKey!=work.after.racerKey)
        return fail(Status::owner,0x17a648);
    if(hit.status!=OriginalFollowCollisionStatus::Complete)return fail(Status::missingCollision,hit.sourcePC);
    Math m;auto raw=random.sharedRandomState;C next=work.after;auto out=output;
    //17A650 copies unshaken position to output.17BB70 writes that copy only;
    //17A684 saves the unshaken source stack vector to C0 afterwards.
    V shake{0,0,0,1};const float x=nextUnit(raw,m),y=nextUnit(raw,m),z=nextUnit(raw,m);
    shake.x=m.sub(x,m.sub(1,x));shake.y=m.sub(y,m.sub(1,y));shake.z=m.sub(z,m.sub(1,z));
    shake=m.normalize(shake);
    const float dx=m.mul(next.shakeAmplitude170,m.mul(next.shakeScale174,shake.x));
    const float dy=m.mul(next.shakeAmplitude170,m.mul(next.shakeScale174,shake.y));
    const float dz=m.mul(next.shakeAmplitude170,m.mul(next.shakeScale174,shake.z));
    out.position=hit.position70;out.position.x=m.add(out.position.x,dx);
    out.position.y=m.add(out.position.y,dy);out.position.z=m.add(out.position.z,dz);
    out.target=work.collision.target80;out.fov=work.fov118;
    if(n64PillarCameraShake50NH.scale>0.f){
        static const bool trace=std::getenv("RR_TEST_N64_PILLAR_CAMERA")!=nullptr;
        if(trace)std::fprintf(stderr,"n64_pillar_camera seconds=%g eye_y=%g target_y=%g\n",
            n64PillarCameraShake50NH.elapsed,double(n64PillarCameraShake50NH.eyeHeight()),double(n64PillarCameraShake50NH.targetHeight()));
        out.position.y=m.add(out.position.y,n64PillarCameraShake50NH.eyeHeight());
        out.target.y=m.add(out.target.y,n64PillarCameraShake50NH.targetHeight());
    }
    //115CE8 dispatch initializes camera mode1 and calls virtual+14;17A050
    //does not write the mode. No inherited procedural mode0 may leak through.
    out.mode=1;next.historyC0=hit.position70;next.historyD0=work.collision.target80;
    // PC ENHANCEMENT (zero-g): follow the pod's full attitude - up is the
    // pod's up (0x80042510..5D4), with no pitch / cubic roll damping
    // (0x800425DC, 0x8004266C skipped). Mode 0 hands look and up to
    // 115060/composeCameraView49F as the attached camera does; mode 1 would
    // rebuild up from world Y there. Position, target and FOV are unchanged.
    if(zeroGravityCameraFollowsAttitude50ZG()){
        const V look=m.normalize(V{m.sub(out.target.x,out.position.x),m.sub(out.target.y,out.position.y),
            m.sub(out.target.z,out.position.z),1});
        const V podUp=work.collision.upB0;const float along=m.add(m.add(m.mul(podUp.x,look.x),m.mul(podUp.y,look.y)),m.mul(podUp.z,look.z));
        const V up=m.normalize(V{m.sub(podUp.x,m.mul(along,look.x)),m.sub(podUp.y,m.mul(along,look.y)),m.sub(podUp.z,m.mul(along,look.z)),1});
        out.look=look;out.up=up;out.mode=0;
    }
    if(!m.ok)return fail(Status::arithmetic,0x17bb70);
    ++next.revision;c=next;random.sharedRandomState=raw;output=out;work.pending=false;return {};
}
}
Result readOriginalFollowPresets(const OriginalFollowCamSource& s,OriginalFollowPreset* storage,
    std::size_t capacity,OriginalFollowPresetBank& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x2868c0);
    if(!s.memberIdentity||!s.sections||!s.sectionCount||s.pod60<0||!storage)return fail(Status::authoring,0x2867a0);
    const auto* countSection=section(s,"NumberOfPresets");std::int32_t count=0;
    if(!countSection||!integer(*countSection,"NumberOfPresets",count)||count<=0)return fail(Status::authoring,0x286820);
    constexpr std::size_t nativeRecordCapacity=64;
    if(std::size_t(count)>capacity||std::size_t(count)>nativeRecordCapacity)return fail(Status::capacity,0x286864);
    std::array<OriginalFollowPreset,nativeRecordCapacity> records{};Math m;
    for(std::size_t i=0;i<std::size_t(count);++i){
        char name[32];const char* suffix=s.layout118==0?"":s.layout118==1?"h":s.layout118==2?"v":"q";
        std::snprintf(name,sizeof name,"Preset%u%s",unsigned(i+1),suffix);
        const auto* sec=section(s,name);if(!sec)return fail(Status::authoring,0x28697c,i);auto& p=records[i];
        if(!integer(*sec,"ID",p.id00)||!number(*sec,"AngleY",p.angleY04)||!number(*sec,"AngleX",p.angleX08)||
            !number(*sec,"FollowingDistance",p.distance0C)||!number(*sec,"FOV",p.fov10)||
            !number(*sec,"ReverseAngleXOffset",p.reverseAngle14)||!number(*sec,"ReverseFollowingOffset",p.reverseDistance18)||
            !number(*sec,"ReverseLookAheadOffset",p.reverseLookAhead1C)||!number(*sec,"ForwardReverseSwitchSpeed",p.switchSpeed20)||
            !integer(*sec,"CycleThruThisPreset",p.cycle24)||!integer(*sec,"FilterThisPreset",p.filter28)||
            !number(*sec,"LookAheadDistance",p.lookAhead30)||
            !integer(*sec,"IsLookBackCam",p.lookBack34)||!number(*sec,"CameraSwing",p.cameraSwing38)||
            !number(*sec,"LookSwing",p.lookSwing3C)||!number(*sec,"MinFollowingDistance",p.minDistance40)||
            !number(*sec,"MaxFollowingDistance",p.maxDistance44)||!number(*sec,"MinFOV",p.minFov48)||!number(*sec,"MaxFOV",p.maxFov4C))
            return fail(Status::authoring,0x286984,i);
        if(value(*sec,"DontRotate")){std::int32_t d=0;if(!integer(*sec,"DontRotate",d))return fail(Status::authoring,0x286b08,i);p.dontRotate2C=d;}
        p.angleY04=m.mul(p.angleY04,bits(0x3c8efa34u));p.angleX08=m.mul(p.angleX08,bits(0x3c8efa34u));
    }
    if(!m.ok)return fail(Status::arithmetic,0x2869dc);
    for(std::size_t i=0;i<std::size_t(count);++i)storage[i]=records[i];
    out={s.memberIdentity,s.pod60,s.layout118,storage,std::size_t(count)};return {};
}
Result prepareOriginalFollowCamera(C& c,const OriginalFollowIdentity& id,const OriginalFollowPresetBank& bank) noexcept {
    if(c.prepared||!identity(id)||!bank.memberIdentity||!bank.records||!bank.count||bank.pod60!=id.pod60||bank.count>std::size_t(INT32_MAX))return fail(Status::owner,0x179580);
    C next;next.sessionGeneration=id.sessionGeneration;next.racerKey=id.racerKey;next.mainBodyKey=id.mainBodyKey;
    next.containerKey=id.containerKey;next.bank=bank;next.prepared=true;
    next.bodySpeedScale88=bits(0x42cd5518u); //179D90, before179580.
    next.target28.fill({0,.1f,1});next.position34.fill({0,.1f,1});next.direction40.fill({.4f,.1f,1});
    next.fov4C={80,.55f,1};next.roll50={0,0,1};next.extra12C={0,.1f,1};next.extra154={0,.01f,1};next.velocity130.fill({0,.3f,1});
    next.collision.localPoint1A0={0,0,0,1};selected(next,0);next.revision=1;c=next;return {};
}
Result activateOriginalFollowCamera(C& c) noexcept {
    if(!bound(c))return fail(Status::owner,0x179f40);
    c.racerModeTimer164=0;c.snap10C=true;c.active=true;++c.revision;return {};
}
Result selectOriginalFollowPreset(C& c,std::int32_t id,bool save,bool restore) noexcept {
    if(!bound(c))return fail(Status::owner,0x17bd00);auto next=c;
    //17BD00 resets the collision positional accumulator even when no ID matches.
    next.collision.localPoint1A0={0,0,0,1};next.scratch180={0,0,0,1};
    if(save&&!restore)next.savedOffsets68={{next.distanceOffset58,next.yawOffset5C,next.pitchOffset60,next.fovOffset64}};
    for(std::size_t i=0;i<next.bank.count;++i)if(next.bank.records[i].id00==id){selected(next,i);break;}
    if(restore&&!save){next.distanceOffset58=next.savedOffsets68[0];next.yawOffset5C=next.savedOffsets68[1];
        next.pitchOffset60=next.savedOffsets68[2];next.fovOffset64=next.savedOffsets68[3];}
    ++next.revision;c=next;return {};
}
Result applyOriginalFollowInput(C& c,OriginalFollowInput& input) noexcept {
    if(!bound(c))return fail(Status::owner,0x179dd0);
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x179dd0);
    if(c.debugCountdown168<=0&&input.debugQuery4_0_12)return fail(Status::unsupportedDebug,0x179df0);
    if(input.reloadFlag08)return fail(Status::unsupportedReload,0x2845e0);
    if(c.debugCountdown168==INT32_MIN)return fail(Status::unsupportedDebug,0x179ef8);
    auto next=c;auto in=input;Math m;--next.debugCountdown168;
    in.axis20=m.mul(in.axis20,.25f);in.axis24=m.mul(in.axis24,.25f);
    if(in.distanceFlag00)next.distanceOffset58=m.add(next.distanceOffset58,m.mul(bits(0xbe99999au),in.axis24));
    if(in.fovFlag04)next.fovOffset64=m.add(next.fovOffset64,m.mul(-.5f,in.axis24));
    if(!in.distanceFlag00&&!in.fovFlag04){
        next.yawOffset5C=m.add(next.yawOffset5C,m.mul(.05f,in.axis20));
        if(next.yawOffset5C>bits(0x40490fdbu))next.yawOffset5C=m.sub(next.yawOffset5C,bits(0x40c90fdbu));
        else if(next.yawOffset5C<bits(0xc0490fdbu))next.yawOffset5C=m.add(next.yawOffset5C,bits(0x40c90fdbu));
        const float pitch=m.mul(.05f,in.axis24);
        if(!(pitch<0&&next.terrain110))next.pitchOffset60=m.add(next.pitchOffset60,pitch);
        if(next.pitchOffset60>bits(0x40490fdbu))next.pitchOffset60=m.sub(next.pitchOffset60,bits(0x40c90fdbu));
        else if(next.pitchOffset60<bits(0xc0490fdbu))next.pitchOffset60=m.add(next.pitchOffset60,bits(0x40c90fdbu));
    }
    if(!m.ok)return fail(Status::arithmetic,0x2845d0);
    ++next.revision;c=next;input=in;return {};
}
Result setOriginalFollowShake(C& c,float scale,float amplitude) noexcept {
    if(!bound(c))return fail(Status::owner,0x17bb60);
    //17BB60 is copy-only: validity is checked only when a later pose consumes it.
    c.shakeScale174=scale;c.shakeAmplitude170=amplitude;++c.revision;return {};
}
Result readOriginalFollowSubject(const OriginalFollowIdentity& id,const SceneGraph49P& graph,
    SceneNodeIndex49P node,std::int32_t mode,V velocity,float a4,OriginalFollowSubject& out) noexcept {
    const auto index=static_cast<std::int32_t>(node);
    if(!identity(id)||!graph.nodes||index<0||std::uint32_t(index)>=graph.count||graph.count>maxSceneNodes49P||
        !validateSceneGraph49P(graph))return fail(Status::owner,0x17a1e0);
    std::array<SceneNode49P,maxSceneNodes49P> copy{};for(std::size_t i=0;i<graph.count;++i)copy[i]=graph.nodes[i];
    const SceneGraph49P staged{copy.data(),graph.count};
    if(!refreshSceneWorld49P(staged,node))return fail(Status::arithmetic,0x28b1f0);
    const auto& w=copy[std::size_t(index)].world100.v;
    OriginalFollowSubject next{id.sessionGeneration,id.racerKey,id.mainBodyKey,id.containerKey,mode,
        {w[12],w[13],w[14],1},{w[8],w[9],w[10],1},{w[4],w[5],w[6],1},velocity,a4};
    for(std::size_t i=0;i<graph.count;++i)graph.nodes[i]=copy[i];out=next;return {};
}
Result beginOriginalFollowPose17A050(const C& c,const OriginalFollowSubject& s,float dt,
    std::uint8_t raceFlags,OriginalFollowPoseWork& out) noexcept {
    if(!bound(c)||!c.active||c.sessionGeneration!=s.sessionGeneration||c.racerKey!=s.racerKey||
        c.mainBodyKey!=s.mainBodyKey||c.containerKey!=s.containerKey)return fail(Status::owner,0x17a050);
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x17a050);
    if(c.terrainProvider120Present)return fail(Status::terrainStub,0x179ce4);
    OriginalFollowPoseWork work;work.after=c;work.baseRevision=c.revision;auto& next=work.after;
    const auto& p=c.bank.records[c.current100];Math m;
    if(s.racerMode64==1||s.racerMode64==2)next.racerModeTimer164=m.min(1.5f,m.add(c.racerModeTimer164,dt));
    else next.racerModeTimer164=m.max(0,m.sub(c.racerModeTimer164,dt));
    next.dynamicFov15C=m.mul(10,m.div(next.racerModeTimer164,1.5f));next.dynamicDistance160=next.dynamicFov15C;
    float fov=m.add(next.fov10,next.fovOffset64);
    if(fov>p.maxFov4C){next.fovOffset64=m.sub(p.maxFov4C,next.fov10);fov=p.maxFov4C;}
    else if(fov<p.minFov48){next.fovOffset64=m.sub(p.minFov48,next.fov10);fov=p.minFov48;}
    if(!p.lookBack34&&!(raceFlags&0x10u))fov=m.add(next.dynamicFov15C,fov);
    V forward=s.worldForward,up=s.worldUp,position=s.worldPosition;forward.w=up.w=position.w=1;
    // PC ENHANCEMENT, NOT ORIGINAL (native_zero_gravity_step50zg.h): in N64
    // zero-g (ZON|ZOFF, 0x80041FF0) the chase cam builds its offset in the
    // pod's own plane (0x80042510..5D4) instead of levelling it to world up.
    const bool zeroG=zeroGravityCameraFollowsAttitude50ZG();
    V flat=forward;if(!zeroG)flat.y=0;flat=m.normalize(flat);
    const V velocity=m.filters(next.velocity130,s.bodyVelocity70,dt);
    const float ratio=m.div(fov,p.maxFov4C),clamped=m.min(m.max(-1,-m.keep(s.bodyA4)),1);
    const float swing=m.mul(ratio,m.mul(m.mul(bits(0x3e570a3du),next.cameraSwing14),clamped));
    //17A414..17A438 computes speed even though the current286150 result does
    //not consume it. Retain reached numeric-domain admission in source order.
    const float speed=m.root(m.add(m.add(m.mul(velocity.x,velocity.x),m.mul(velocity.y,velocity.y)),m.mul(velocity.z,velocity.z)));
    (void)speed;
    const V right=m.cross(flat,up);
    next.distance04=m.add(next.distanceOffset58,p.distance0C);
    if(next.distance04<p.minDistance40){next.distance04=p.minDistance40;next.distanceOffset58=m.sub(next.distance04,p.distance0C);}
    else if(next.distance04>p.maxDistance44){next.distance04=p.maxDistance44;next.distanceOffset58=m.sub(next.distance04,p.distance0C);}
    if(!p.lookBack34&&!(raceFlags&0x10u))next.distance04=m.add(next.distance04,next.dynamicDistance160);
    updateAngles(next,p,swing,m);
    V rotated{0,0,0,1},direction{0,0,0,1};
    if(!m.ok||!rotate(right,forward,next.angleX0C,rotated)||!rotate(up,rotated,next.angleY08,direction))return fail(Status::arithmetic,0x17a590);
    V camera{m.add(position.x,m.mul(next.distance04,direction.x)),m.add(position.y,m.mul(next.distance04,direction.y)),
        m.add(position.z,m.mul(next.distance04,direction.z)),1};
    V target=position;float roll=0;
    //2863F0 calls179CA0 twice. The proven null120 branch has no writes.
    if(p.filter28&&!next.snap10C){
        target=m.filters(next.target28,target,dt);camera=m.filters(next.position34,camera,dt);
        fov=m.filter(next.fov4C,fov,dt);
        //Cold roll50 tau0 is real. No VU-Q rule is substituted for scalarDIV.S.
        //Until scalar zero-divide semantics are admitted, report this exact join.
        if(next.roll50.tau==0)return fail(Status::arithmetic,0x2866a0);
        roll=m.filter(next.roll50,roll,dt);
    }else if(next.snap10C){
        next.target28[0].value=target.x;next.target28[1].value=target.y;next.target28[2].value=target.z;
        next.position34[0].value=camera.x;next.position34[1].value=camera.y;next.position34[2].value=camera.z;
        next.snap10C=false; //286770; FOV/roll filter values are NOT reseeded.
    }
    if(!m.ok)return fail(Status::arithmetic,0x17a050);
    work.collision={camera,right,target,flat,up,dt};work.fov118=fov;work.roll11C=roll;work.pending=true;out=work;return {};
}
Result advanceOriginalFollowCamera(C& c,const OriginalFollowSubject& subject,float dt,std::uint8_t raceFlags,
    const OriginalFollowCollisionSources& sources,OriginalCameraQueryPairOwner& pair,
    CoupledNoiseContext49M& random,TVCameraUpdateOutput49F& output,OriginalFollowCollisionResult& hitOut) noexcept {
    OriginalFollowPoseWork work;const auto started=beginOriginalFollowPose17A050(c,subject,dt,raceFlags,work);
    if(started.status!=Status::ok)return started;
    auto actual=sources;actual.mainBodyVelocity70=subject.bodyVelocity70;
    actual.cameraDistance04=work.after.distance04;
    actual.selectedProfile34=std::uint32_t(c.bank.records[c.current100].lookBack34);
    OriginalFollowCollisionResult hit;
    const auto status=advanceOriginalFollowCollision17A870(work.collision,actual,work.after.collision,pair,hit);
    if(status!=OriginalFollowCollisionStatus::Complete)return fail(Status::missingCollision,hit.sourcePC);
    const auto done=finish(c,work,hit,random,output);if(done.status==Status::ok)hitOut=hit;return done;
}
Result advanceOriginalFollowCameraView(C& c,const OriginalFollowSubject& subject,float dt,std::uint8_t raceFlags,
    const OriginalFollowCollisionSources& sources,OriginalCameraQueryPairOwner& pair,
    CoupledNoiseContext49M& random,TVCameraUpdateOutput49F& output,CameraSceneState49F& scene,
    CameraViewOutput49F& view,OriginalFollowCollisionResult& hitOut,std::uint8_t rootFlags,std::uint8_t sceneFlags) noexcept {
    if((rootFlags&4u)||(sceneFlags&4u))return fail(Status::sceneFlags,0x115cb0);
    C next=c;auto randomNext=random;auto out=output;OriginalFollowCollisionResult hit;
    auto result=advanceOriginalFollowCamera(next,subject,dt,raceFlags,sources,pair,randomNext,out,hit);
    if(result.status!=Status::ok)return result;
    CameraViewOutput49F projected;if(!composeCameraView49F(scene,out,projected))return fail(Status::arithmetic,0x115350);
    c=next;random.sharedRandomState=randomNext.sharedRandomState;output=out;scene=projected.sceneAfter;view=projected;hitOut=hit;return {};
}
}
