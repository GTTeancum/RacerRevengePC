#include "original_follow_collision.h"
#include "original_camera_query_owner.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using V=OriginalFollowCollisionVector;using S=OriginalFollowCollisionStatus;
float bits(std::uint32_t n) noexcept {float f;std::memcpy(&f,&n,4);return f;}
bool scalar(float f) noexcept {
    std::uint32_t u;std::memcpy(&u,&f,4);const auto e=u&0x7f800000u;
    return e!=0x7f800000u&&(e!=0||(u&0x7fffffu)==0);
}
struct Math {
    bool ok=true;
    float keep(float f) noexcept {ok=ok&&scalar(f);return f;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float f=a+b;return keep(f);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float f=a-b;return keep(f);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float f=a*b;return keep(f);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0){ok=false;return 0;}volatile float f=a/b;return keep(f);}
    float root(float f) noexcept {keep(f);if(f<0){ok=false;return 0;}volatile float r=std::sqrt(f);return keep(r);}
    V add(V a,V b) noexcept {return {add(a.x,b.x),add(a.y,b.y),add(a.z,b.z),1.f};}
    V sub(V a,V b) noexcept {return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z),1.f};}
    V mul(float f,V a) noexcept {return {mul(f,a.x),mul(f,a.y),mul(f,a.z),1.f};}
    float squared(V v) noexcept {const float x=mul(v.x,v.x),y=mul(v.y,v.y),z=mul(v.z,v.z);return add(add(x,y),z);}
    //17AA74,17AB60,17B064: scalar RSQRT followed by DIV.S, not direct sqrt.
    float length(V v) noexcept {
        const float s=squared(v);if(s==0.f)return 0.f;
        volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(s)));
        return div(1.f,keep(reciprocal));
    }
    V normalize(V v) noexcept {
        const float x=mul(v.x,v.x),y=mul(v.y,v.y),z=mul(v.z,v.z);
        const float sum=add(add(x,y),mul(1.f,z));
        const float q=sum==0.f?std::numeric_limits<float>::max():div(1.f,root(sum));
        return {mul(v.x,q),mul(v.y,q),mul(v.z,q),add(v.w,0.f)};
    }
    float filter(OriginalFollowCollisionFilter& f,float desired,float dt) noexcept {
        keep(dt);keep(f.tau);f.blend=div(detail::scalarMin48O(dt,f.tau),f.tau);
        f.value=add(f.value,mul(f.blend,sub(desired,f.value)));return f.value;
    }
    V transform(const BodyMatrix48G& a,V p) noexcept {
        V out{0,0,0,1};float* lane[3]={&out.x,&out.y,&out.z};
        for(unsigned k=0;k<3;++k){
            const float x=mul(a.v[k],p.x),y=mul(a.v[4+k],p.y),z=mul(a.v[8+k],p.z);
            *lane[k]=add(add(add(x,y),z),a.v[12+k]);
        }
        return out;
    }
    BodyMatrix48G inverse(const BodyMatrix48G& world) noexcept {
        BodyMatrix48G result{};result.v[15]=1.f;
        //28BE38..5C transposes the XYZ basis through add-zero lanes.
        for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)
            result.v[row*4+col]=add(world.v[col*4+row],0.f);
        //28BE6C..80 computes the inverse translation first. Do not replace
        // this by subtracting world translation before the basis products.
        for(unsigned k=0;k<3;++k){
            const float x=mul(result.v[k],world.v[12]),y=mul(result.v[4+k],world.v[13]),z=mul(result.v[8+k],world.v[14]);
            result.v[12+k]=mul(add(add(x,y),z),-1.f);
        }
        return result;
    }
};
SpringVector4_48N vector(V p) noexcept {return {{p.x,p.y,p.z,p.w}};}
S fail(OriginalFollowCollisionResult& out,S s,std::uint32_t pc) noexcept {out.status=s;out.sourcePC=pc;return s;}
struct Collision {
    const OriginalFollowCollisionInput& input;
    const OriginalFollowCollisionSources& sources;
    OriginalFollowCollisionState state;
    OriginalCameraQueryPairOwner& owner;
    OriginalFollowCollisionResult& result;
    Math math{};
    bool pair(V target,V end,V center,bool execute=true) noexcept {
        if(!math.ok){fail(result,S::Arithmetic,0x17a870u);return false;}
        const auto a=vector(target),b=vector(end),c=vector(center);OriginalCameraPairResult receipt{};
        const auto status=updateOriginalCameraQueryPair(owner,a,b,c,execute,state.callback,receipt);
        result.world=receipt.world;if(execute)++result.queryPairs;
        if(status!=OriginalCameraPairStatus::Complete){fail(result,S::WorldQuery,receipt.sourcePC);return false;}
        return true;
    }
    bool refine(V first,V direction,float length,float tolerance,V initialClear,V& chosen) noexcept {
        float step=length,travel=0.f;V lastClear=initialClear,candidate=initialClear;
        for(unsigned iteration=0;iteration<8;++iteration){
            step=math.div(step,2.f);
            candidate=math.add(first,math.mul(math.add(travel,step),direction));
            if(!pair(input.target80,candidate,candidate))return false;
            if(state.callback.hit114)travel=math.add(travel,step);else lastClear=candidate;
            if(step<=tolerance)break;
        }
        chosen=state.callback.hit114?lastClear:candidate;return math.ok;
    }
    bool run(const BodyMatrix48G& world) noexcept {
        const V target=input.target80,eye=input.position70;
        const BodyMatrix48G inverse=math.inverse(world);
        const V oldWorld=math.transform(world,state.localPoint1A0);
        const V first=math.sub(math.add(eye,oldWorld),target);
        //17A9AC's unused output still performs the original reached arithmetic.
        (void)math.transform(inverse,eye);
        if(!pair(target,first,first))return false;
        V chosen{};
        if(state.callback.hit114){
            const float speed=math.length(sources.mainBodyVelocity70);
            if(speed<bits(0x4212aa7fu)||sources.selectedProfile34!=0){
                const V delta=math.sub(target,eye);
                const float length=math.length(delta);const V direction=math.normalize(delta);
                if(!refine(first,direction,length,bits(0x3dcccccdu),target,chosen))return false;
                if(chosen.x==target.x&&chosen.y==target.y&&chosen.z==target.z)
                    chosen=math.sub(target,math.mul(bits(0x3c23d70au),direction));
            }else{
                if(!sources.history){fail(result,S::History,0x17ae34u);return false;}
                const auto receipt=readOriginalCameraRacerHistory(*sources.history,sources.cameraDistance04);
                if(receipt.status!=OriginalCameraRacerHistoryStatus::Complete){fail(result,S::History,0x17ae34u);return false;}
                const V historical{receipt.point180[0],receipt.point180[1],receipt.point180[2],receipt.point180[3]};
                const V delta=math.sub(historical,first);const float length=math.length(delta);
                const V direction=math.normalize(delta);
                if(!refine(first,direction,length,1.f,historical,chosen))return false;
            }
        }else{
            if(!pair(target,eye,eye))return false;
            chosen=state.callback.hit114?first:eye;
        }
        V local=math.transform(inverse,math.add(math.sub(chosen,eye),target));
        auto& selected=state.callback.hit114?state.contact13C:state.clear148;
        auto& other=state.callback.hit114?state.clear148:state.contact13C;
        local.x=math.filter(selected[0],local.x,input.elapsed);
        local.y=math.filter(selected[1],local.y,input.elapsed);
        local.z=math.filter(selected[2],local.z,input.elapsed);
        other[0].value=local.x;other[1].value=local.y;other[2].value=local.z;
        const V displacement=math.sub(math.transform(world,local),target);
        V position=eye;position.x=math.add(eye.x,displacement.x);position.y=math.add(eye.y,displacement.y);position.z=math.add(eye.z,displacement.z);
        state.localPoint1A0=local;
        const V origin{world.v[12],world.v[13],world.v[14],1.f};
        if(!pair(origin,origin,origin,false))return false;
        if(!math.ok){fail(result,S::Arithmetic,0x17b55cu);return false;}
        result.position70=position;return true;
    }
};
} // namespace
S advanceOriginalFollowCollision17A870(const OriginalFollowCollisionInput& input,
    const OriginalFollowCollisionSources& sources,OriginalFollowCollisionState& state,
    OriginalCameraQueryPairOwner& owner,OriginalFollowCollisionResult& result) noexcept {
    if(!sources.graph||sources.graph->count>maxSceneNodes49P)return S::Binding;
    const auto& graph=*sources.graph;
    const OriginalCameraOwnerSpan writes[]={{&state,sizeof state},{&result,sizeof result}};
    const OriginalCameraOwnerSpan reads[]={{&input,sizeof input},{&sources,sizeof sources},
        {&graph,sizeof graph},{graph.nodes,static_cast<std::size_t>(graph.count)*sizeof(SceneNode49P)},
        {sources.history,sources.history?sizeof(*sources.history):0u}};
    if(!admitOriginalCameraQueryPairStorage(owner,writes,2,reads,5))return S::Binding;
    result={};
    const auto index=static_cast<std::int32_t>(sources.racerScene190);
    if(!validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count||
       !(graph.nodes[index].sceneFlags190&1u))return fail(result,S::Binding,0x28bab0u);
    if(std::fegetround()!=FE_TONEAREST)return fail(result,S::Arithmetic,0x17a870u);
    Collision calculation{input,sources,state,owner,result};
    if(!calculation.run(graph.nodes[index].world100))return result.status==S::Complete?fail(result,S::Arithmetic,0x17a870u):result.status;
    state=calculation.state;return S::Complete;
}
}
