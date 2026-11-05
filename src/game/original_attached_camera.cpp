#include "original_attached_camera.h"
#include "chariot_scene49u.h"
#include "scalar_minmax48o.h"
#include "../assets/pod_physics49n.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#if defined(__SSE__)
#include <xmmintrin.h>
#endif
#include <limits>

namespace rr::game { namespace {
using V=OriginalAttachedVector;using C=OriginalAttachedCamera;using G=OriginalAttachedGlobals;
using Result=OriginalAttachedResult;using Status=OriginalAttachedStatus;
Result fail(Status status,std::uint32_t pc) noexcept {return {status,pc};}
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
bool scalar(float f) noexcept {std::uint32_t u;std::memcpy(&u,&f,4);const auto e=u&0x7f800000u;
    return e!=0x7f800000u&&(e!=0||(u&0x7fffffu)==0);}
struct Math {
    bool ok=true;
    // Diagnostic-only first rejected scalar. No new numeric admission or
    // arithmetic: retain the original short-circuit predicate exactly.
    unsigned stage=0,firstBadStage=0,firstBadOperation=0,firstBadRole=0;
    std::uint32_t firstBadBits=0,firstBadA=0,firstBadB=0;
    void reject(float x,unsigned operation,unsigned role,float a,float b) noexcept {
        if(ok){std::memcpy(&firstBadBits,&x,4);std::memcpy(&firstBadA,&a,4);std::memcpy(&firstBadB,&b,4);
            firstBadStage=stage;firstBadOperation=operation;firstBadRole=role;ok=false;}
    }
    float keep(float x,unsigned operation=0,unsigned role=0,float a=0,float b=0) noexcept {
        if(ok&&!scalar(x))reject(x,operation,role,operation?a:x,b);
        return x;
    }
    //13DF00's velocityY feeds only13E118..13E1BC's discarded vertical
    //map. Admit its finite raw input without changing live-pose Math's
    //normal/zero domain. NaN/Inf and every computed result remain checked.
    float keepDiscardedVerticalVelocity(float x) noexcept {
        if(ok){std::uint32_t u;std::memcpy(&u,&x,4);
            if((u&0x7f800000u)==0x7f800000u)reject(x,0,0,x,0);}
        return x;
    }
    float subtractDiscardedVerticalInput(float a,float b) noexcept {
        keepDiscardedVerticalVelocity(a);keep(b,2,2,a,b);
        volatile float r=a-b;return keep(r,2,3,a,b);
    }
    //13E074..13E114 consumes raw roll through an ordered interval map.
    // Only its input may be finite-subnormal: with the actual range2 the
    // first subtraction rounds to2, then the unchanged mapping gives +0.
    // All other operands and EVERY computed result retain scalar checks.
    float keepYawRollInput(float x) noexcept {
        if(ok){std::uint32_t u;std::memcpy(&u,&x,4);
            if((u&0x7f800000u)==0x7f800000u)reject(x,0,0,x,0);}
        return x;
    }
    float subtractYawRollInput(float a,float b) noexcept {
        keepYawRollInput(a);keep(b,2,2,a,b);
        volatile float r=a-b;return keep(r,2,3,a,b);
    }
    float add(float a,float b) noexcept {keep(a,1,1,a,b);keep(b,1,2,a,b);volatile float r=a+b;return keep(r,1,3,a,b);}
    float sub(float a,float b) noexcept {keep(a,2,1,a,b);keep(b,2,2,a,b);volatile float r=a-b;return keep(r,2,3,a,b);}
    float mul(float a,float b) noexcept {keep(a,3,1,a,b);keep(b,3,2,a,b);volatile float r=a*b;return keep(r,3,3,a,b);}
    float div(float a,float b) noexcept {keep(a,4,1,a,b);keep(b,4,2,a,b);if(b==0){reject(b,4,2,a,b);return 0;}volatile float r=a/b;return keep(r,4,3,a,b);}
    V cross(V a,V b) noexcept {return {sub(mul(a.y,b.z),mul(a.z,b.y)),sub(mul(a.z,b.x),mul(a.x,b.z)),
        sub(mul(a.x,b.y),mul(a.y,b.x)),0};}
    V normalize(V a) noexcept {
        const float x=mul(a.x,a.x),y=mul(a.y,a.y),z=mul(a.z,a.z);
        const float square=add(add(x,y),mul(1,z));float q=std::numeric_limits<float>::max();
        if(square!=0){volatile float root=std::sqrt(std::fabs(square));q=div(1,keep(root));}
        return {mul(a.x,q),mul(a.y,q),mul(a.z,q),add(a.w,0)};
    }
    float filter(OriginalAttachedFilter& f,float target,float dt) noexcept {
        keep(dt);keep(f.tau);f.blend=div(detail::scalarMin48O(dt,f.tau),f.tau);
        f.value=add(f.value,mul(f.blend,sub(target,f.value)));return f.value;
    }
};
bool validBinding(const OriginalAttachedBinding& b) noexcept {
    return b.sessionGeneration&&b.racerKey&&b.mainBodyKey&&b.model1B4Key&&b.pod60>0&&
        b.body288!=SceneNodeIndex49P::none&&b.model1B4!=SceneNodeIndex49P::none;
}
bool bound(const C& c) noexcept {return c.prepared&&validBinding(c.binding)&&c.revision!=std::numeric_limits<std::uint64_t>::max();}
void lazy(G& g) noexcept {
    if(!g.lazy8338){g.yawInputRange3EB0=2;g.lazy8338=true;}
    if(!g.lazy833C){g.verticalInputRange3EB8=100;g.lazy833C=true;}
    if(!g.lazy8340){g.yawAngle3EC0=bits(0x3f060a91u);g.lazy8340=true;}
    if(!g.lazy8344){g.verticalAngle3EC8=bits(0x3f060a91u);g.lazy8344=true;}
}
//13E074..13E114: source interval map/clamp. Ordering is retained rather than
//algebraically simplifying to A4*pi/12, which changes binary32 rounding.
float yawAngle(Math& m,float input,float range,float angle) noexcept {
    const float low=-m.keep(range),high=m.keep(range),outHigh=m.keep(angle),outLow=-angle;
    if(high==low)return 0;
    const float scaled=m.div(m.mul(m.subtractYawRollInput(input,low),m.sub(outLow,outHigh)),m.sub(high,low));
    float value=m.add(outHigh,scaled);
    if(outLow<=outHigh){if(value<outLow)value=outLow;else if(!(value<=outHigh))value=outHigh;}
    else {if(!(value<=outLow))value=outLow;else if(value<outHigh)value=outHigh;}
    return value;
}
void unusedVerticalMap(Math& m,float input,float range,float angle) noexcept {
    const float low=-m.keep(range),high=m.keep(range),outHigh=m.keep(angle),outLow=-angle;
    if(high==low)return;
    const float scaled=m.div(m.mul(m.subtractDiscardedVerticalInput(input,low),m.sub(outHigh,outLow)),m.sub(high,low));
    const float value=m.add(outLow,scaled);(void)value;
    //13E150..13E1BC contains comparisons but publishes no value. f12 yaw is
    //unchanged across this complete dead vertical-swing calculation.
}
bool rotate(V axis,V source,float angle,V& out) noexcept {
    VisualVector49R a{{axis.x,axis.y,axis.z,axis.w}};
    VisualVector49R b{{source.x,source.y,source.z,source.w}};
    VisualVector49R r{{out.x,out.y,out.z,out.w}};
    if(!detail::rotateVector49U(a,b,angle,r))return false;
    out={r.v[0],r.v[1],r.v[2],r.v[3]};return true;
}
float randomUnit(Math& m,CoupledNoiseContext49M& rng) noexcept {
    const auto n=drawOriginalSharedRng50IU(rng);
    volatile float converted=static_cast<float>(static_cast<std::int32_t>(n));
    return m.mul(bits(0x30000000u),converted);
}
V shake(Math& m,V p,const C& c,CoupledNoiseContext49M& rng) noexcept {
    const float x=randomUnit(m,rng),y=randomUnit(m,rng),z=randomUnit(m,rng);
    V v{m.sub(x,m.sub(1,x)),m.sub(y,m.sub(1,y)),m.sub(z,m.sub(1,z)),1};v=m.normalize(v);
    v={m.mul(c.shakeScale2C,v.x),m.mul(c.shakeScale2C,v.y),m.mul(c.shakeScale2C,v.z),1};
    p.x=m.add(p.x,m.mul(c.shakeAmplitude28,v.x));p.y=m.add(p.y,m.mul(c.shakeAmplitude28,v.y));
    p.z=m.add(p.z,m.mul(c.shakeAmplitude28,v.z));return p;
}
Result pose(C& c,const OriginalAttachedFrame& in,const SceneGraph49P& graph,G& globals,
    CoupledNoiseContext49M& rng,TVCameraUpdateOutput49F& out) noexcept {
    if(!bound(c)||!c.active||in.sessionGeneration!=c.binding.sessionGeneration||
       in.racerKey!=c.binding.racerKey||in.mainBodyKey!=c.binding.mainBodyKey||
       in.model1B4Key!=c.binding.model1B4Key)return fail(Status::owner,0x13df74);
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x13df00);
    const auto model=static_cast<std::int32_t>(c.binding.model1B4),body=static_cast<std::int32_t>(c.binding.body288);
    if(!graph.nodes||model<0||body<0||static_cast<std::uint32_t>(model)>=graph.count||
       static_cast<std::uint32_t>(body)>=graph.count||!validateSceneGraph49P(graph))return fail(Status::graph,0x13df94);
    //13DEF0's actual model1B4 override is nonnull in this original1750A0 path.
    if(!refreshSceneWorld49P(graph,c.binding.model1B4))return fail(Status::graph,0x28b1a0);
    const auto& w=graph.nodes[static_cast<std::uint32_t>(model)].world100.v;
    const V position{w[12],w[13],w[14],1},forward{w[8],w[9],w[10],1},up{w[4],w[5],w[6],1};
    Math m;m.stage=1;const float roll=-m.keepYawRollInput(in.mainBodyA4),vertical=-m.keepDiscardedVerticalVelocity(in.mainBodyVelocityY74);
    m.stage=2;const V right=m.cross(forward,up);lazy(globals);
    m.stage=3;const float yaw=yawAngle(m,roll,globals.yawInputRange3EB0,globals.yawAngle3EC0);
    m.stage=4;unusedVerticalMap(m,vertical,globals.verticalInputRange3EB8,globals.verticalAngle3EC8);
    V look{0,0,0,1};
    const bool rotationAttempted=m.ok;
    const bool rotationAccepted=rotationAttempted&&rotate(up,forward,yaw,look);
    if(!m.ok||!rotationAccepted){
        const auto word=[](float value) noexcept {std::uint32_t u;std::memcpy(&u,&value,4);return u;};
        unsigned mxcsrAvailable=0,mxcsr=0;
#if defined(__SSE__)
        mxcsrAvailable=1;mxcsr=_mm_getcsr();
#endif
        std::fprintf(stderr,
            "original_attached_13e1c4_rejected math_ok=%u first_bad_stage=%u first_bad_bits=%08x first_bad_operation=%u first_bad_role=%u first_bad_a=%08x first_bad_b=%08x rotation_attempted=%u rotation_accepted=%u "
            "pod=%d body_node=%d model_node=%d a4=%08x velocity_y=%08x elapsed=%08x yaw=%08x "
            "up=%08x,%08x,%08x forward=%08x,%08x,%08x right=%08x,%08x,%08x "
            "yaw_range=%08x yaw_angle=%08x vertical_range=%08x vertical_angle=%08x rounding=%d mxcsr_available=%u mxcsr=%08x\n",
            unsigned(m.ok),m.firstBadStage,m.firstBadBits,m.firstBadOperation,m.firstBadRole,m.firstBadA,m.firstBadB,unsigned(rotationAttempted),unsigned(rotationAccepted),
            c.binding.pod60,body,model,word(in.mainBodyA4),word(in.mainBodyVelocityY74),word(in.elapsed),word(yaw),
            word(up.x),word(up.y),word(up.z),word(forward.x),word(forward.y),word(forward.z),
            word(right.x),word(right.y),word(right.z),word(globals.yawInputRange3EB0),word(globals.yawAngle3EC0),
            word(globals.verticalInputRange3EB8),word(globals.verticalAngle3EC8),std::fegetround(),mxcsrAvailable,mxcsr);
        return fail(Status::arithmetic,0x13e1c4);
    }
    if(c.snap3C){c.direction30[0].value=look.x;c.direction30[1].value=look.y;c.direction30[2].value=look.z;c.snap3C=false;}
    else {look.x=m.filter(c.direction30[0],look.x,in.elapsed);look.y=m.filter(c.direction30[1],look.y,in.elapsed);
        look.z=m.filter(c.direction30[2],look.z,in.elapsed);}
    look=m.normalize(look);
    //28B9B0 XYZ = three ordered basis products, then VADD+0. Source resultW
    //is not consumed by13E338..13E358; constructed output position has W1.
    float offset[3];const float local[]={c.offset10.x,c.offset10.y,c.offset10.z};
    for(unsigned axis=0;axis<3;++axis){float value=m.mul(w[axis],local[0]);
        value=m.add(value,m.mul(w[4+axis],local[1]));value=m.add(value,m.mul(w[8+axis],local[2]));
        offset[axis]=m.add(value,0);}
    V camera{m.add(position.x,offset[0]),m.add(position.y,offset[1]),m.add(position.z,offset[2]),1};
    camera=shake(m,camera,c,rng);
    const V cameraUp=m.normalize(m.cross(right,look));
    if(!m.ok||!scalar(c.fov04))return fail(Status::arithmetic,0x13e3b0);
    out.position=camera;out.look=look;out.up=cameraUp;out.fov=c.fov04;out.mode=0;++c.revision;
    // Opt-in observation of the actual source pose, before view publication.
    // No camera parameters, scene transforms or RNG draws are changed.
    static const bool trace=[](){const char* value=std::getenv("RR_CAMERA_POSE_TRACE");
        return value&&std::strcmp(value,"1")==0;}();
    if(trace&&(c.revision<5||(c.revision%64)==0))
        std::fprintf(stderr,"original_attached_pose pod=%d revision=%llu body_node=%d model_node=%d "
            "offset=%.9g,%.9g,%.9g model_position=%.9g,%.9g,%.9g "
            "basis_x=%.9g,%.9g,%.9g basis_y=%.9g,%.9g,%.9g basis_z=%.9g,%.9g,%.9g "
            "camera=%.9g,%.9g,%.9g look=%.9g,%.9g,%.9g up=%.9g,%.9g,%.9g fov=%.9g\n",
            c.binding.pod60,static_cast<unsigned long long>(c.revision),body,model,
            local[0],local[1],local[2],position.x,position.y,position.z,
            w[0],w[1],w[2],w[4],w[5],w[6],w[8],w[9],w[10],
            camera.x,camera.y,camera.z,look.x,look.y,look.z,cameraUp.x,cameraUp.y,cameraUp.z,c.fov04);
    return {};
}
struct Slice {const char* data=nullptr;std::size_t size=0,offset=0;};
Slice trim(Slice s) noexcept {while(s.size&&(s.data[0]==' '||s.data[0]=='\t'||s.data[0]=='\r')){++s.data;--s.size;++s.offset;}
    while(s.size&&(s.data[s.size-1]==' '||s.data[s.size-1]=='\t'||s.data[s.size-1]=='\r'))--s.size;return s;}
bool equal(Slice s,const char* key) noexcept {std::size_t i=0;for(;i<s.size&&key[i];++i){
    const char a=s.data[i]>='A'&&s.data[i]<='Z'?char(s.data[i]+32):s.data[i];
    const char b=key[i]>='A'&&key[i]<='Z'?char(key[i]+32):key[i];if(a!=b)return false;}return i==s.size&&!key[i];}
} // namespace

bool readOriginalAttachedOffset215CC0(std::int32_t pod,std::uint64_t member,
    const char* phy,std::size_t bytes,OriginalAttachedOffset& out) noexcept {
    if(pod<=0||!member||!phy||!bytes||bytes>assets::maxPodPhysicsBytes49N)return false;
    OriginalAttachedOffset next;next.pod60=pod;next.sourceMemberIdentity=member;bool selected=false,sectionSeen=false;std::size_t at=0;
    while(at<bytes){const auto begin=at;while(at<bytes&&phy[at]!='\n'){
            const unsigned char c=static_cast<unsigned char>(phy[at]);if((c<32&&c!='\r'&&c!='\t')||c>=127)return false;++at;}
        const auto length=at-begin;if(at<bytes)++at;if(length>assets::maxPodPhysicsLine49N)return false;
        const auto line=trim({phy+begin,length,begin});if(!line.size||line.data[0]==';')continue;
        if(line.data[0]=='['){if(line.size<3||line.data[line.size-1]!=']')return false;
            selected=equal(trim({line.data+1,line.size-2,line.offset+1}),"CameraInfo");
            if(selected&&sectionSeen)return false;sectionSeen=sectionSeen||selected;continue;}
        std::size_t split=0;while(split<line.size&&line.data[split]!='=')++split;if(split==line.size)return false;
        if(!selected||!equal(trim({line.data,split,line.offset}),"FirstPersonCamOffset"))continue;
        if(next.authored)return false;const auto value=trim({line.data+split+1,line.size-split-1,line.offset+split+1});
        float lane[3];std::size_t beginValue=0;
        for(unsigned i=0;i<3;++i){std::size_t end=beginValue;while(end<value.size&&value.data[end]!=',')++end;
            if((i<2)==(end==value.size))return false;
            const auto n=trim({value.data+beginValue,end-beginValue,value.offset+beginValue});
            if(!assets::parsePodPhysicsNumber49N(n.data,n.size,lane[i]))return false;beginValue=end+1;}
        next.racerCE0={lane[0],lane[1],lane[2],1};next.authored=true;
        next.keyOffset=line.offset;next.valueOffset=value.offset;next.valueBytes=value.size;
    }
    out=next;return true;
}
Result prepareOriginalAttachedCamera(C& out,const OriginalAttachedBinding& binding,
    const OriginalAttachedOffset& authored) noexcept {
    if(out.prepared||!validBinding(binding))return fail(Status::owner,0x1750d4);
    if(!authored.sourceMemberIdentity||authored.pod60!=binding.pod60)return fail(Status::authoring,0x175208);
    C next;next.binding=binding;next.fov04=90;next.offset10=authored.racerCE0;
    for(auto& f:next.direction30){f.value=0;f.tau=bits(0x3e19999au);f.blend=1;}
    next.prepared=true;next.revision=1;out=next;return {};
}
Result activateOriginalAttachedCamera(C& c) noexcept {
    if(!bound(c))return fail(Status::owner,0x13e480);c.snap3C=true;c.active=true;++c.revision;return {};
}
Result setOriginalAttachedShake(C& c,float scale,float amplitude) noexcept {
    if(!bound(c))return fail(Status::owner,0x13e530);c.shakeScale2C=scale;c.shakeAmplitude28=amplitude;++c.revision;return {};
}
OriginalAttachedSnapshot snapshotOriginalAttached13E490() noexcept {return {};}
Result advanceOriginalAttachedCamera(C& c,const OriginalAttachedFrame& in,const SceneGraph49P& graph,G& globals,
    CoupledNoiseContext49M& random,TVCameraUpdateOutput49F& output) noexcept {
    if(!graph.nodes||graph.count>maxSceneNodes49P)return fail(Status::graph,0x13df94);
    std::array<SceneNode49P,maxSceneNodes49P> nodes;for(std::size_t i=0;i<graph.count;++i)nodes[i]=graph.nodes[i];
    const SceneGraph49P staged{nodes.data(),graph.count};auto next=c;auto g=globals;auto rng=random;auto out=output;
    const auto result=pose(next,in,staged,g,rng,out);if(result.status!=Status::ok)return result;
    c=next;globals=g;random.sharedRandomState=rng.sharedRandomState;output=out;
    for(std::size_t i=0;i<graph.count;++i)graph.nodes[i]=nodes[i];return {};
}
Result advanceOriginalAttachedCameraView(C& c,const OriginalAttachedFrame& in,const SceneGraph49P& graph,G& globals,
    CoupledNoiseContext49M& random,TVCameraUpdateOutput49F& output,CameraSceneState49F& scene,
    CameraViewOutput49F& view,std::uint8_t rootFlags,std::uint8_t sceneFlags) noexcept {
    if((rootFlags&4u)||(sceneFlags&4u))return fail(Status::sceneFlags,0x115cb0);
    if(!graph.nodes||graph.count>maxSceneNodes49P)return fail(Status::graph,0x13df94);
    std::array<SceneNode49P,maxSceneNodes49P> nodes;for(std::size_t i=0;i<graph.count;++i)nodes[i]=graph.nodes[i];
    const SceneGraph49P staged{nodes.data(),graph.count};auto next=c;auto g=globals;auto rng=random;auto out=output;
    const auto result=pose(next,in,staged,g,rng,out);if(result.status!=Status::ok)return result;
    CameraViewOutput49F projected;if(!composeCameraView49F(scene,out,projected))return fail(Status::arithmetic,0x115350);
    c=next;globals=g;random.sharedRandomState=rng.sharedRandomState;output=out;scene=projected.sceneAfter;view=projected;
    for(std::size_t i=0;i<graph.count;++i)graph.nodes[i]=nodes[i];return {};
}
} // namespace rr::game
