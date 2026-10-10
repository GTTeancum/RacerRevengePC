#include "model_motion49q.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
              "49Q motion requires IEEE binary32");
#include "model_motion49o_tables.inc"
float value(std::uint32_t u) noexcept { float f;std::memcpy(&f,&u,4);return f; }
using Q=std::array<float,4>;
using V=std::array<float,3>;
struct Eval {
    bool valid=true;
    float finite(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return finite(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return finite(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return finite(x);}
    float div(float a,float b) noexcept {if(b==0){valid=false;return 0;}volatile float x=a/b;return finite(x);}
    float sine(float x) noexcept {
        if(x>value(0x3fc90fdb))x=sub(value(0x40490fdb),x);
        else if(x<value(0xbfc90fdb))x=sub(value(0xc0490fdb),x);
        float t=mul(x,x),c=add(value(0xb9500d01),mul(value(0x3638ef1d),t));
        c=add(value(0x3c088889),mul(t,c));c=add(value(0xbe2aaaab),mul(t,c));
        return add(x,mul(mul(x,t),c));
    }
    float acosPositive(float x) noexcept {
        //14FD00 supplies the nonnegative absolute quaternion dot product.
        if(!(x>=0&&x<1)){valid=false;return 0;}
        float scaled=mul(2048,x);auto i=static_cast<std::uint32_t>(scaled);
        if(i>=2048){valid=false;return 0;}
        float u=sub(scaled,static_cast<float>(i));
        return add(mul(sub(1,u),value(acosWords[i])),mul(u,value(acosWords[i+1])));
    }
    float dot(const Q&a,const Q&b) noexcept {
        return add(add(mul(a[3],b[3]),add(mul(a[1],b[1]),mul(a[2],b[2]))),mul(a[0],b[0]));
    }
    Q blend(const Q&a,const Q&b,float t) noexcept {
        float c=dot(a,b),sign=1;if(c<0){sign=-1;c=-c;}if(!(c<1))return a;
        float theta=acosPositive(c),inverse=div(1,sine(theta)),tt=mul(t,theta);
        float bWeight=mul(sign,mul(inverse,sine(tt))),aWeight=mul(inverse,sine(sub(theta,tt)));
        Q q{};for(unsigned i=0;i<4;++i)q[i]=add(mul(aWeight,a[i]),mul(bWeight,b[i]));return q;
    }
    Q decode(const assets::ByteAxisKey49O& k) noexcept {
        float s=value(sineWords[k.angle]);Q q{value(cosineWords[k.angle]),0,0,0};
        for(unsigned i=0;i<3;++i)q[i+1]=mul(mul(value(0x3c010204),static_cast<float>(k.axis[i])),s);
        return q;
    }
    bool rotation(const assets::MotionView49O& v,std::uint32_t ti,float time,Q& result) noexcept {
        const auto n=v.rotation[ti].keyCount;assets::ByteAxisKey49O a{},b{};
        if(n<2||n>assets::maxMotKeysPerTrack49O||!assets::readRotationKey49O(v,ti,0,a)||
           !assets::readRotationKey49O(v,ti,n-1,b)||time<a.time||time>b.time)return false;
        if(time==b.time){result=decode(b);return true;}
        // Same strict upper interval as145850, with checked finite asset view.
        std::uint32_t lo=0,hi=n-1;
        while(hi-lo>1){auto mid=(lo+hi)/2;assets::ByteAxisKey49O k{};
            if(!assets::readRotationKey49O(v,ti,mid,k))return false;
            if(time<k.time)hi=mid;else lo=mid;
        }
        if(!assets::readRotationKey49O(v,ti,lo,a)||!assets::readRotationKey49O(v,ti,hi,b))return false;
        float u=div(sub(time,a.time),sub(b.time,a.time));Q later=decode(b);
        result=blend(decode(a),later,u);
        if(dot(result,later)<0)for(auto& x:result)x=-x;
        return true;
    }
    bool position(const assets::MotionView49O& v,std::uint32_t ti,float time,V& p) noexcept {
        const auto n=v.position[ti].keyCount;assets::BezierKey49O a{},b{};
        if(n<2||n>assets::maxMotKeysPerTrack49O||!assets::readPositionKey49O(v,ti,0,a)||
           !assets::readPositionKey49O(v,ti,n-1,b)||time<a.time||time>b.time)return false;
        const bool interpolate=time!=b.time;std::uint32_t lo=0,hi=n-1;
        if(interpolate){while(hi-lo>1){auto mid=(lo+hi)/2;assets::BezierKey49O k{};
            if(!assets::readPositionKey49O(v,ti,mid,k))return false;
            if(time<k.time)hi=mid;else lo=mid;
        }}else lo=n-2;
        if(!assets::readPositionKey49O(v,ti,lo,a)||!assets::readPositionKey49O(v,ti,hi,b))return false;
        float u=div(sub(time,interpolate?a.time:b.time),sub(b.time,a.time));
        if(!interpolate){for(unsigned i=0;i<3;++i)p[i]=add(b.position[i],mul(u,sub(b.position[i],a.position[i])));return true;}
        float u2=mul(u,u),u3=mul(u,u2),s=sub(mul(3,u2),mul(2,u3)),aWeight=sub(1,s),t2=sub(u3,u2),t1=add(u,sub(t2,u2));
        for(unsigned i=0;i<3;++i)p[i]=add(add(mul(a.outgoing[i],t1),add(mul(a.position[i],aWeight),mul(b.position[i],s))),mul(b.incoming[i],t2));
        return true;
    }
    void rotationMatrix(const Q&q,BodyMatrix48G& local) noexcept {
        float* m=local.v;float w=q[0],x=q[1],y=q[2],z=q[3];
        float norm=add(add(mul(y,y),add(mul(w,w),mul(x,x))),mul(z,z));
        if(norm<=0){for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c)m[r*4+c]=float(r==c);return;}
        float s=div(2,norm),ys=mul(y,s),xs=mul(x,s),zs=mul(z,s);
        float zz=mul(z,zs),wx=mul(w,xs),xx=mul(x,xs),wy=mul(w,ys),xy=mul(x,ys),wz=mul(w,zs),xz=mul(x,zs),yz=mul(y,zs),yy=mul(y,ys);
        m[0]=sub(1,add(yy,zz));m[1]=add(xy,wz);m[2]=sub(xz,wy);m[3]=0;
        m[4]=sub(xy,wz);m[5]=sub(1,add(xx,zz));m[6]=add(yz,wx);m[7]=0;
        m[8]=add(xz,wy);m[9]=sub(yz,wx);m[10]=sub(1,add(xx,yy));m[11]=0;
    }
};
bool reject(ModelMotionDiagnostic49Q& d, ModelMotionError49Q error,
            std::uint32_t track=0, std::uint32_t channel=0) noexcept {
    d={error,track,channel};return false;
}

// Ordinary fixed-record admission only. These checks prevent transaction
// scratch from overwriting the supplied graph, model state or immutable views.
bool overlaps(const void* a, std::size_t as, const void* b, std::size_t bs) noexcept {
    const auto x=reinterpret_cast<std::uintptr_t>(a), y=reinterpret_cast<std::uintptr_t>(b);
    return x<=y ? y-x<as : x-y<bs;
}
}
bool sampleModelMotion49Q(const assets::MotionView49O& motion,
    const assets::MotionNodeBindings49O& bindings, float fraction,
    const SceneGraph49P& graph, ModelMotionState49Q& state,
    ModelMotionWorkspace49Q& workspace, ModelMotionDiagnostic49Q& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST)return reject(d,ModelMotionError49Q::rounding_mode);
    if(!graph.nodes||!graph.count||graph.count>maxSceneNodes49P||!motion.data||
       !std::isfinite(fraction)||fraction<0||fraction>1||
       !std::isfinite(motion.duration)||!(motion.duration>0))
        return reject(d,ModelMotionError49Q::input);
    if(motion.rotationCount>64||motion.positionCount>64||!bindings.count||bindings.count>64||
       !validateSceneGraph49P(graph))return reject(d,ModelMotionError49Q::topology);
    const auto bytes=sizeof(SceneNode49P)*graph.count;
    if(overlaps(&workspace,sizeof workspace,graph.nodes,bytes)||
       overlaps(&workspace,sizeof workspace,&state,sizeof state)||
       overlaps(&workspace,sizeof workspace,&motion,sizeof motion)||
       overlaps(&workspace,sizeof workspace,&bindings,sizeof bindings)||
       overlaps(&workspace,sizeof workspace,&graph,sizeof graph)||
       overlaps(&workspace,sizeof workspace,motion.data,motion.bytes)||
       overlaps(&state,sizeof state,graph.nodes,bytes))
        return reject(d,ModelMotionError49Q::input);
    for(std::uint32_t i=0;i<graph.count;++i)for(unsigned lane=0;lane<16;++lane)
        if(!std::isfinite(graph.nodes[i].localC0.v[lane])||!std::isfinite(graph.nodes[i].world100.v[lane]))
            return reject(d,ModelMotionError49Q::input,i);
    std::memcpy(workspace.nodes,graph.nodes,bytes);
    const SceneGraph49P staged{workspace.nodes,graph.count};
    Eval e;const float time=e.mul(motion.duration,fraction);
    if(!e.valid)return reject(d,ModelMotionError49Q::arithmetic);
    for(std::uint32_t channel=0;channel<2;++channel){
        const auto count=channel?motion.positionCount:motion.rotationCount;
        for(std::uint32_t i=0;i<count;++i){
            const auto& track=channel?motion.position[i]:motion.rotation[i];
            if(track.rowIndex>=bindings.count||bindings.rows[track.rowIndex].sceneNode>=graph.count)
                return reject(d,ModelMotionError49Q::binding,i,channel);
            const auto target=static_cast<SceneNodeIndex49P>(bindings.rows[track.rowIndex].sceneNode);
            BodyMatrix48G local=workspace.nodes[bindings.rows[track.rowIndex].sceneNode].localC0;
            if(!channel){Q q{};if(!e.rotation(motion,i,time,q))return reject(d,ModelMotionError49Q::sample_time,i,channel);
                e.rotationMatrix(q,local);
            }else{V p{};if(!e.position(motion,i,time,p))return reject(d,ModelMotionError49Q::sample_time,i,channel);
                for(unsigned lane=0;lane<3;++lane)local.v[12+lane]=p[lane];
            }
            if(!e.valid)return reject(d,ModelMotionError49Q::arithmetic,i,channel);
#ifdef RR_MODEL49Q_TRACK_WRITE
            RR_MODEL49Q_TRACK_WRITE(channel,i,bindings.rows[track.rowIndex].sceneNode);
#endif
            //28A580/28AC10 share28C4F0+ready-gated289A20 with28B490.
            if(!setLocalMatrix49P(staged,target,local))return reject(d,ModelMotionError49Q::topology,i,channel);
        }
    }
    std::memcpy(graph.nodes,workspace.nodes,bytes);
    state.model18=0;
    return true;
}
}
