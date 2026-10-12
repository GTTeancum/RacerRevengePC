#include "original_attached_camera.h"
#include "chariot_scene49u.h"
#include "scalar_minmax48o.h"
#include "../assets/pod_physics49n.h"
#include <cfenv>
#include <cmath>
#include <cstring>
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
    float keep(float x) noexcept {ok=ok&&scalar(x);return x;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0){ok=false;return 0;}volatile float r=a/b;return keep(r);}
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
    const float scaled=m.div(m.mul(m.sub(input,low),m.sub(outLow,outHigh)),m.sub(high,low));
    float value=m.add(outHigh,scaled);
    if(outLow<=outHigh){if(value<outLow)value=outLow;else if(!(value<=outHigh))value=outHigh;}
    else {if(!(value<=outLow))value=outLow;else if(value<outHigh)value=outHigh;}
    return value;
}
void unusedVerticalMap(Math& m,float input,float range,float angle) noexcept {
    const float low=-m.keep(range),high=m.keep(range),outHigh=m.keep(angle),outLow=-angle;
    if(high==low)return;
    const float scaled=m.div(m.mul(m.sub(input,low),m.sub(outHigh,outLow)),m.sub(high,low));
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
    Math m;const float roll=-m.keep(in.mainBodyA4),vertical=-m.keep(in.mainBodyVelocityY74);
    const V right=m.cross(forward,up);lazy(globals);
    const float yaw=yawAngle(m,roll,globals.yawInputRange3EB0,globals.yawAngle3EC0);
    unusedVerticalMap(m,vertical,globals.verticalInputRange3EB8,globals.verticalAngle3EC8);
    V look{0,0,0,1};if(!m.ok||!rotate(up,forward,yaw,look))return fail(Status::arithmetic,0x13e1c4);
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
    out.position=camera;out.look=look;out.up=cameraUp;out.fov=c.fov04;out.mode=0;++c.revision;return {};
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
