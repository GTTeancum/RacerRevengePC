#include "wrong_way_progress50go.h"
#include "navigation_consumer_bridge50mb.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game { namespace {
using E=WrongWayProgressError50GO;
using Op=WrongWayHudOperation50GO;
bool fail(WrongWayProgressDiagnostic50GO& d,E error,std::uint32_t pc) noexcept {
    d={error,pc};return false;
}
float fp(std::uint32_t word) noexcept {
    float value;std::memcpy(&value,&word,sizeof value);return value;
}
std::uint32_t bits(float value) noexcept {
    std::uint32_t word;std::memcpy(&word,&value,sizeof word);return word;
}
struct Math {
    bool valid=true;
    float read(float value) noexcept {
        valid=valid&&std::isfinite(value);return value;
    }
    float add(float a,float b) noexcept {
        read(a);read(b);volatile float value=a+b;return read(value);
    }
    float sub(float a,float b) noexcept {
        read(a);read(b);volatile float value=a-b;return read(value);
    }
    float mul(float a,float b) noexcept {
        read(a);read(b);volatile float value=a*b;return read(value);
    }
    void normalizeHorizontal(float (&v)[3]) noexcept {
        // 1A2EF8..1A2F74 / 1A2FEC..1A3060. Scalar ADD.A/MADD,
        // then scalar RSQRT.S; this is not the VU zero/saturated-Q path.
        v[1]=0.f;
        const float x=mul(v[0],v[0]),y=mul(v[1],v[1]);
        const float acc=add(x,y),norm=add(acc,mul(v[2],v[2]));
        if(norm==0.f){
            // 434060 = (0,0,0,1), initialized by 3C9910..3C9990.
            v[0]=0.f;v[1]=0.f;v[2]=0.f;return;
        }
        if(!valid||norm<0.f||(bits(norm)&0x7f800000u)==0){
            valid=false;return;
        }
        // Existing native core's admitted finite scalar square-root convention:
        // rounded binary32 root, followed by a separately rounded division.
        volatile float root=static_cast<float>(std::sqrt(static_cast<double>(norm)));
        volatile float reciprocal=1.f/root;
        read(root);read(reciprocal);
        for(float& component:v)component=mul(component,reciprocal);
    }
    float dot(const float (&a)[3],const float (&b)[3]) noexcept {
        const float x=mul(a[0],b[0]),y=mul(a[1],b[1]),z=mul(a[2],b[2]);
        const float one=add(0.f,1.f),acc=add(x,y);
        return add(acc,mul(one,z));
    }
    float angle(float input) noexcept {
        // Exact 13C770..13C8AC. This helper is NOT originalAngle48G,
        // sourceAcos49T/libm, or terrain48G's different asin-like result.
        const float absolute=std::fabs(read(input));
        if(input>1.f)return 0.f;
        if(input< -1.f)return fp(0x40490fdbu);
        float t=add(fp(0xc372b84fu),mul(fp(0x42a8a11bu),absolute));
        t=add(fp(0x43836812u),mul(t,absolute));
        t=add(fp(0xc3031cc3u),mul(absolute,t));
        t=add(fp(0x41ed4ad5u),mul(absolute,t));
        t=add(fp(0xbfb9d5d7u),mul(absolute,t));
        t=add(fp(0x3d06871eu),mul(absolute,t));
        return input<0.f?add(fp(0x3fc90fdbu),t):sub(fp(0x3fc90fdbu),t);
    }
};
WrongWayHudRequest50GO request(Op operation,void* owner) noexcept {
    WrongWayHudRequest50GO r{};r.operation=operation;r.owner=owner;
    switch(operation){
    case Op::SetColor:
        r.callPC=0x1a318c;r.calleePC=0x29da10;r.returnPC=0x1a3194;
        r.argumentA1=0xffcc2929u;break;
    case Op::SetPosition:
        r.callPC=0x1a31c4;r.calleePC=0x29db50;r.returnPC=0x1a31cc;break;
    case Op::SetText:
        r.callPC=0x1a31e8;r.calleePC=0x29d940;r.returnPC=0x1a31f0;
        r.text="WRONG WAY";break;
    case Op::Show:
        r.callPC=0x1a3238;r.calleePC=0x29da40;r.returnPC=0x1a3240;
        r.argumentA1=230;r.argumentA2=0;r.argumentA3=255;
        r.argumentF[1]=1.5f;r.argumentF[2]=fp(0x3f99999au);
        r.argumentF[4]=-1.f;r.argumentF[5]=1.f;break;
    case Op::Hide:
        r.callPC=0x1a326c;r.calleePC=0x29db10;r.returnPC=0x1a3274;break;
    case Op::None:break;
    }
    return r;
}
bool validMarkers(const LapProgressTrack50GF& markers,std::size_t extent) noexcept {
    return markers.prepared&&markers.nodeCount>0&&
        markers.nodeCount<=extent&&
        markers.progressCount48<=maxLapProgressNodes50GF;
}
} // namespace

template<class Graph>
static bool beginWrongWayProgress50GOShared50MB(const Graph& map,std::size_t extent,
    const LapProgressTrack50GF& markers,const WrongWayProgressInput50GO& in,
    WrongWayProgressState50GO& state,WrongWayProgressContinuation50GO& continuation,
    WrongWayProgressReceipt50GO& receipt,WrongWayProgressDiagnostic50GO& d) noexcept {
    if(continuation.pending.operation!=Op::None)
        return fail(d,E::CallbackPending,continuation.pending.callPC);
    auto next=state;
    WrongWayProgressContinuation50GO nextContinuation{};
    WrongWayProgressReceipt50GO result{};
    next.flags10CC=static_cast<std::uint8_t>(next.flags10CC&~2u); // 1A2E64
    if(in.application34C){
        if(!validMarkers(markers,extent))return fail(d,E::Track,0x1a2e80);
        next.vehicleFF8=markers.progressCount48; // 1A2E8C delay slot
        result.markerCursorFilled=true;
    }else if((in.config58&2u)&&!(in.config58&1u)){
        if(!(in.vehicleCD8&1u)&&in.vehicle40==0){
            if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x1a2ef8);
            if(!map.count||map.count>extent||
               in.vehicleC8C>=map.count||in.vehicleC88>=map.count)
                return fail(d,E::Track,0x1a2f90);
            Math f;
            float forward[3]={in.localForwardE0[0],0.f,in.localForwardE0[2]};
            f.normalizeHorizontal(forward);
            if(!f.valid)return fail(d,E::Arithmetic,0x1a2f48);
            const auto& previous=map.nodes[in.vehicleC8C].position;
            const auto& current=map.nodes[in.vehicleC88].position;
            // The source subtracts all XYZ, then overwrites Y before normalization.
            float direction[3]={f.sub(current[0],previous[0]),
                f.sub(current[1],previous[1]),f.sub(current[2],previous[2])};
            f.normalizeHorizontal(direction);
            if(!f.valid)return fail(d,E::Arithmetic,0x1a3034);
            result.directionDot=f.dot(direction,forward);
            result.sourceAngle=f.angle(result.directionDot);
            if(!f.valid)return fail(d,E::Arithmetic,0x1a308c);
            result.directionEvaluated=true;
            result.wrongWay=result.sourceAngle<fp(0x407b53d1u)&&
                !(result.sourceAngle<=fp(0x4016cbe4u));
            next.flags10CC=static_cast<std::uint8_t>(
                (next.flags10CC&~2u)|(result.wrongWay?2u:0u)); // 1A30F0
        }
        result.warningBranchReached=true;
        if(in.warningOwner){
            if(result.wrongWay){
                // 1A3118..1A31C0: CVT.W.S of 320+f20, 170+f21.
                // Exact FE_TONEAREST results for all three source branches.
                if(std::fegetround()!=FE_TONEAREST)
                    return fail(d,E::RoundingMode,0x1a31a8);
                nextContinuation.positionX=in.viewportMode8C>1?160u:320u;
                nextContinuation.positionY=in.viewportMode8C==0?170u:
                    in.viewportMode8C==1?118u:205u;
                nextContinuation.pending=request(Op::SetColor,in.warningOwner);
            }else nextContinuation.pending=request(Op::Hide,in.warningOwner);
        }
    }
    state=next;continuation=nextContinuation;receipt=result;d={};return true;
}
bool beginWrongWayProgress50GO(const UndertrackTrack50ET& map,
    const LapProgressTrack50GF& markers,const WrongWayProgressInput50GO& in,
    WrongWayProgressState50GO& state,WrongWayProgressContinuation50GO& continuation,
    WrongWayProgressReceipt50GO& receipt,WrongWayProgressDiagnostic50GO& d) noexcept {
    return beginWrongWayProgress50GOShared50MB(map,maxUndertrackNodes50ET,markers,in,state,continuation,receipt,d);
}
bool beginWrongWayProgress50GO(const NavigationView50CR& map,
    const LapProgressTrack50GF& markers,const WrongWayProgressInput50GO& in,
    WrongWayProgressState50GO& state,WrongWayProgressContinuation50GO& continuation,
    WrongWayProgressReceipt50GO& receipt,WrongWayProgressDiagnostic50GO& d) noexcept {
    if(!navigation_consumer_detail50MB::mapReady(map)||!markers.prepared||markers.nodeCount!=map.count())
        return fail(d,E::Track,0x1a2f90);
    return beginWrongWayProgress50GOShared50MB(navigation_consumer_detail50MB::read(map).track,
        map.capacity(),markers,in,state,continuation,receipt,d);
}

bool resumeWrongWayProgress50GO(WrongWayProgressContinuation50GO& continuation,
    WrongWayProgressDiagnostic50GO& d) noexcept {
    const auto operation=continuation.pending.operation;
    if(operation==Op::None)return fail(d,E::NoPendingCallback,0x1a3274);
    if(!continuation.pending.owner)
        return fail(d,E::InvalidContinuation,continuation.pending.callPC);
    Op nextOperation=Op::None;
    switch(operation){
    case Op::SetColor:nextOperation=Op::SetPosition;break;
    case Op::SetPosition:nextOperation=Op::SetText;break;
    case Op::SetText:nextOperation=Op::Show;break;
    case Op::Show:case Op::Hide:break;
    case Op::None:default:return fail(d,E::InvalidContinuation,0x1a3274);
    }
    auto next=continuation;
    next.pending=request(nextOperation,continuation.pending.owner);
    if(nextOperation==Op::SetPosition){
        next.pending.argumentA1=next.positionX;
        next.pending.argumentA2=next.positionY;
    }else if(nextOperation==Op::None)next={};
    continuation=next;d={};return true;
}
}
