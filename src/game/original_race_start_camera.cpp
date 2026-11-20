#include "original_race_start_camera.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using V=RaceStartVector;
using C=OriginalRaceStartCamera;
bool scalar(float x) noexcept {
    std::uint32_t u;std::memcpy(&u,&x,4);
    const auto e=u&0x7f800000u;
    return e!=0x7f800000u&&(e!=0||(u&0x007fffffu)==0);
}
struct Math {
    bool ok=true;
    float keep(float x) noexcept {ok=ok&&scalar(x);return x;}
    bool in(float a,float b) noexcept {ok=ok&&scalar(a)&&scalar(b);return ok;}
    float add(float a,float b) noexcept {if(!in(a,b))return 0;volatile float x=a,y=b,r=x+y;return keep(r);}
    float sub(float a,float b) noexcept {if(!in(a,b))return 0;volatile float x=a,y=b,r=x-y;return keep(r);}
    float mul(float a,float b) noexcept {if(!in(a,b))return 0;volatile float x=a,y=b,r=x*y;return keep(r);}
    float div(float a,float b) noexcept {if(!in(a,b)||b==0){ok=false;return 0;}volatile float x=a,y=b,r=x/y;return keep(r);}
    float root(float a) noexcept {if(!in(a,0)||a<0){ok=false;return 0;}volatile float r=std::sqrt(a);return keep(r);}
    float min(float a,float b) noexcept {if(!in(a,b))return 0;return detail::scalarMin48O(a,b);}
    float max(float a,float b) noexcept {if(!in(a,b))return 0;return detail::scalarMax48O(a,b);}
    V add(V a,V b) noexcept {return {add(a.x,b.x),add(a.y,b.y),add(a.z,b.z),1};}
    V sub(V a,V b) noexcept {return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z),1};}
    V mul(V a,float b) noexcept {return {mul(a.x,b),mul(a.y,b),mul(a.z,b),1};}
    float dot(V a,V b) noexcept {
        const float x=mul(a.x,b.x),y=mul(a.y,b.y),z=mul(a.z,b.z);
        return add(add(x,y),z);
    }
    float squared(V a) noexcept {return dot(a,a);}
    float length(V a) noexcept {return root(squared(a));}
    V normalize(V a) noexcept {
        const float s=squared(a);
        if(s==0)return {0,0,0,1}; //444510 initialized by3CBFE0.
        return mul(a,div(1,root(s)));
    }
    V cross(V a,V b) noexcept {
        return {sub(mul(a.y,b.z),mul(a.z,b.y)),sub(mul(a.z,b.x),mul(a.x,b.z)),
            sub(mul(a.x,b.y),mul(a.y,b.x)),0};
    }
    float filter(RaceStartFilter& f,float target,float dt) noexcept {
        const float limited=min(dt,f.tau);
        f.blend=div(limited,f.tau);
        f.value=add(f.value,mul(f.blend,sub(target,f.value)));
        return f.value;
    }
    V filters(std::array<RaceStartFilter,3>& f,V v,float dt) noexcept {
        return {filter(f[0],v.x,dt),filter(f[1],v.y,dt),filter(f[2],v.z,dt),v.w};
    }
};
bool bankValid(const RaceStartRacerBank& b) noexcept {
    if(!b.sessionGeneration||!b.racerCount||b.racerCount>8)return false;
    for(std::size_t i=0;i<b.racerCount;++i){
        if(!b.racers[i].racerKey||!b.racers[i].bodyNodeKey)return false;
        for(std::size_t j=0;j<i;++j)
            if(b.racers[i].racerKey==b.racers[j].racerKey||b.racers[i].bodyNodeKey==b.racers[j].bodyNodeKey)return false;
    }
    return true;
}
bool matches(const C& c,const RaceStartRacerBank& b) noexcept {
    if(!bankValid(b)||c.sessionGeneration!=b.sessionGeneration||c.racerCount!=b.racerCount)return false;
    for(std::size_t i=0;i<b.racerCount;++i)
        if(c.racerKeys[i]!=b.racers[i].racerKey||c.bodyNodeKeys[i]!=b.racers[i].bodyNodeKey)return false;
    return true;
}
V position(const RaceStartRacerBank& b,std::int32_t i,Math& m) noexcept {
    if(i<0||i>=8){m.ok=false;return {};}
    V v=b.racers[static_cast<std::size_t>(i)].localPositionF0;
    //28B180 writes XYZ into source-initialized vf0 temporary.
    v.w=1;return v;
}
struct Random {
    Math& math;
    RaceStartRandomGlobals& globals;
    std::uint32_t& sharedRaw;
    RaceStartSeedSamples seeds;
    RaceStartPreparationReceipt& receipt;
    bool missing=false, invalid=false;
    void log(std::uint32_t pc,std::uint32_t word,bool seed) noexcept {
        if(receipt.randomOperationCount>=receipt.randomOperations.size()){invalid=true;return;}
        receipt.randomOperations[receipt.randomOperationCount++]={pc,word,seed};
    }
    void ensureSeed(std::uint32_t pc) noexcept {
        if(!globals.seed.lazy80A8){globals.seed.seed3D6B30=0;globals.seed.lazy80A8=true;}
        if(globals.seed.seed3D6B30)return;
        if(!seeds.values||receipt.seedSamplesConsumed>=seeds.size){missing=true;return;}
        const auto s=seeds.values[receipt.seedSamplesConsumed++];
        globals.seed.seed3D6B30=s;sharedRaw=s;log(pc,s,true);
    }
    float unit(std::uint32_t pc) noexcept {
        sharedRaw=sharedRaw*0x41c64e6du+0x3039u;
        const auto n=sharedRaw&0x7fffffffu;log(pc,n,false);
        volatile float converted=static_cast<float>(static_cast<std::int32_t>(n));
        return math.mul(0x1p-31f,converted);
    }
    std::int32_t bounded(std::int32_t count,std::uint32_t seedPc,std::uint32_t drawPc) noexcept {
        ensureSeed(seedPc);if(missing||invalid)return 0;
        const float scaled=math.mul(static_cast<float>(count),unit(drawPc));
        if(!globals.range.lazy80AC){globals.range.rangeCheck3D6B38=1;globals.range.lazy80AC=true;}
        if(!math.ok||scaled<0||scaled>=static_cast<float>(count)){invalid=true;return 0;}
        return static_cast<std::int32_t>(scaled); //source CVT.W.S truncation.
    }
};
//2D5740 fixed topology. No data-derived graph guesses.
constexpr std::int32_t links[12][4]={
    {-1,1,-1,4},{0,2,-1,5},{1,3,-1,6},{2,-1,-1,7},
    {-1,5,0,8},{4,6,1,9},{5,7,2,10},{6,-1,3,11},
    {-1,9,4,-1},{8,10,5,-1},{9,11,6,-1},{10,-1,7,-1}};
void grid(C& c,const RaceStartRacerBank& b,Math& m) noexcept {
    V p=position(b,1,m);p.y=m.add(p.y,4);
    const V forward=b.racers[1].forwardE0,up=b.racers[1].upD0;
    const V right=m.cross(up,forward);
    c.grid[0].position=m.sub(m.add(p,m.mul(forward,m.div(100,2))),m.mul(right,m.div(40,2)));
    for(std::size_t i=1;i<4;++i)c.grid[i].position=m.add(c.grid[i-1].position,m.mul(right,40));
    for(std::int32_t row=1;row<3;++row){
        const V rowForward=b.racers[static_cast<std::size_t>((row-1)*3+1)].forwardE0;
        for(std::size_t col=0;col<4;++col){
            const V scaled=m.mul(rowForward,100);
            const V displacement=m.mul(scaled,static_cast<float>(row));
            c.grid[static_cast<std::size_t>(row)*4+col].position=m.sub(c.grid[col].position,displacement);
        }
    }
    for(std::size_t i=0;i<12;++i)for(std::size_t j=0;j<4;++j)c.grid[i].neighbor[j]=links[i][j];
}
std::int32_t edgeDistance(std::int32_t node,std::size_t direction) noexcept {
    std::int32_t n=0;
    while(links[node][direction]!=-1){node=links[node][direction];++n;}
    return n;
}
void appendRoute(C& c,std::size_t direction,std::int32_t limit) noexcept {
    std::int32_t node=c.route[0],n=0;
    while(links[node][direction]!=-1&&n!=limit){
        node=links[node][direction];++n;c.route[static_cast<std::size_t>(n)]=node;
    }
    c.routeCount1FC=1+n;
}
std::int32_t closestAlternate(const C& c,const RaceStartRacerBank& b,V forwardToTarget,Math& m) noexcept {
    if(c.routeCount1FC<3)return -1;
    //2D7C74 loads route[count-2], NOT the final route node.
    const auto penultimate=c.route[static_cast<std::size_t>(c.routeCount1FC-2)];
    const V probe=m.add(c.grid[static_cast<std::size_t>(penultimate)].position,forwardToTarget);
    float best=std::numeric_limits<float>::max();std::int32_t selected=0;
    for(std::int32_t i=0;i<8;++i){
        const float squared=m.squared(m.sub(probe,position(b,i,m)));
        if(squared<best){best=squared;selected=i;}
    }
    return selected;
}
void route(C& c,const RaceStartRacerBank& b,Random& r,Math& m) noexcept {
    const std::int32_t start=r.bounded(12,0x2d58e4,0x2d5908);
    if(r.missing||r.invalid||!m.ok)return;
    c.route.fill(-1);c.route[0]=start;c.routeCount1FC=1;
    if(start==5)appendRoute(c,1,-1); //2D59E8: all the way right.
    else if(start==6)appendRoute(c,0,-1); //2D5A14: all the way left.
    else {
        const bool corner=start==0||start==3||start==8||start==11;
        float weight[4]={-1,-1,-1,-1};
        constexpr std::uint32_t cornerDraw[4]={0x2d5a5c,0x2d5a9c,0x2d5adc,0x2d5b1c};
        constexpr std::uint32_t edgeDraw[4]={0x2d6098,0x2d60e8,0x2d6138,0x2d6188};
        for(std::size_t d=0;d<4;++d)if(links[start][d]!=-1){
            const float u=r.unit(corner?cornerDraw[d]:edgeDraw[d]);
            weight[d]=corner?u:m.mul(static_cast<float>(edgeDistance(start,d)),u);
        }
        //Source tie priority: next row, left, right, previous row.
        std::size_t d=3;
        if(weight[3]<weight[0]||weight[3]<weight[1]||weight[3]<weight[2]){
            d=0;
            if(weight[0]<weight[3]||weight[0]<weight[1]||weight[0]<weight[2]){
                d=1;if(weight[1]<weight[0]||weight[1]<weight[3]||weight[1]<weight[2])d=2;
            }
        }
        const auto count=edgeDistance(start,d);
        constexpr std::uint32_t cornerSeed[4]={0x2d5d04,0x2d5e5c,0x2d5f84,0x2d5bac};
        constexpr std::uint32_t cornerPick[4]={0x2d5d24,0x2d5e7c,0x2d5fa4,0x2d5bcc};
        constexpr std::uint32_t edgeSeed[4]={0x2d6374,0x2d64cc,0x2d65f4,0x2d621c};
        constexpr std::uint32_t edgePick[4]={0x2d6394,0x2d64ec,0x2d6614,0x2d623c};
        const auto hops=1+r.bounded(count,corner?cornerSeed[d]:edgeSeed[d],corner?cornerPick[d]:edgePick[d]);
        if(r.missing||r.invalid||!m.ok)return;
        appendRoute(c,d,hops);
    }
    //2D66E4 sets s1=1 unconditionally: the alternative random/nearest-racer
    // branch at2D6750 is unreachable on this original entry. Target is0.
    c.target1F0=0;c.initialTarget1B0=position(b,0,m);c.routeFov14=c.initialFov08;
    const V first=c.grid[static_cast<std::size_t>(c.route[0])].position;
    const V second=c.grid[static_cast<std::size_t>(c.route[1])].position;
    const V routeDirection=m.normalize(m.sub(second,first));
    const V targetDirection=m.normalize(m.sub(c.initialTarget1B0,first));
    const float alignment=m.dot(routeDirection,targetDirection);
    c.handoff23C=r.unit(0x2d6a68)>0.65f;
    if(c.handoff23C){
        if(c.routeCount1FC<3)c.handoff23C=false;
        else {
            c.alternateTarget1F4=alignment>0?closestAlternate(c,b,targetDirection,m):-1;
            if(c.alternateTarget1F4==c.target1F0)c.handoff23C=false;
        }
    }
    //2D6B08..2D6B24: s1 remains1 from2D66E4 (2D7C30 saves/restores it).
    // Keep the reached draw/alternate work above, then clear on this ordinary
    // entry even when the alternate differs. This admits the later zoom draws.
    c.handoff23C=false;
    if(!c.handoff23C){
        const V target=position(b,c.target1F0,m);
        const V last=c.grid[static_cast<std::size_t>(c.route[c.routeCount1FC-1])].position;
        const float distanceSquared=m.squared(m.sub(last,target));
        c.zoomIn234=false;
        if(alignment>0&&distanceSquared>3000.f)c.zoomIn234=r.unit(0x2d6be4)>0.5f;
        c.zoomOut238=false;
        if(alignment<=0&&!c.zoomIn234)c.zoomOut238=r.unit(0x2d6c5c)>0.5f;
    }else{c.zoomIn234=false;c.zoomOut238=false;}
    c.flag230=false;
    float length=0;
    for(std::int32_t i=0;i<c.routeCount1FC-1;++i){
        const V a=c.grid[static_cast<std::size_t>(c.route[i])].position;
        const V b0=c.grid[static_cast<std::size_t>(c.route[i+1])].position;
        length=m.add(length,m.length(m.sub(b0,a)));
    }
    const float duration=m.add(9.f,m.mul(1.5f,r.unit(0x2d6d5c)));
    c.speed10=m.div(length,duration);
}
bool stateValid(const C& c) noexcept {
    if(!c.prepared||c.routeCount1FC<2||c.routeCount1FC>12)return false;
    for(std::int32_t i=0;i<c.routeCount1FC;++i)if(c.route[i]<0||c.route[i]>=12)return false;
    return c.target1F0>=0&&c.target1F0<8&&c.alternateTarget1F4>=-1&&c.alternateTarget1F4<8;
}
void activate(C& c,Math& m) noexcept {
    const V first=c.grid[static_cast<std::size_t>(c.route[0])].position;
    c.unfilteredPosition1C0=first;c.currentFov1C=c.routeFov14;c.complete240=false;
    const V direction=m.normalize(m.sub(c.initialTarget1B0,first));
    c.look1D0=direction;c.bestDot20=-1;
    c.directionFilter={{{direction.x,1.2f,1},{direction.y,1.2f,1},{direction.z,1.2f,1}}};
    c.positionFilter={{{first.x,0.6f,1},{first.y,0.6f,1},{first.z,0.6f,1}}};
    c.fovFilter={c.routeFov14,0.8f,1};c.nextRoute1F8=1;c.active=true;
}
V updatePosition(C& c,float dt,Math& m) noexcept {
    const V target=c.grid[static_cast<std::size_t>(c.route[c.nextRoute1F8])].position;
    const V direction=m.normalize(m.sub(target,c.unfilteredPosition1C0));
    //2D7100..7134 rounds dt*direction BEFORE speed times that result.
    c.step1E0=m.mul(m.mul(direction,dt),c.speed10);
    c.unfilteredPosition1C0=m.add(c.unfilteredPosition1C0,c.step1E0);
    return m.filters(c.positionFilter,c.unfilteredPosition1C0,dt);
}
V updateLook(C& c,const RaceStartRacerBank& b,V filteredPosition,float dt,Math& m) noexcept {
    V direction;
    if(!c.handoff23C)direction=m.normalize(m.sub(position(b,c.target1F0,m),c.unfilteredPosition1C0));
    else if(c.alternateTarget1F4==-1)direction=c.look1D0;
    else {
        const V middle=m.mul(m.add(c.unfilteredPosition1C0,filteredPosition),0.5f);
        const V next=m.normalize(m.sub(position(b,c.alternateTarget1F4,m),middle));
        const float alignment=m.dot(next,c.look1D0);
        if(alignment>c.bestDot20){c.bestDot20=alignment;direction=c.look1D0;}
        else {c.target1F0=c.alternateTarget1F4;c.handoff23C=false;direction=next;}
    }
    c.look1D0=m.filters(c.directionFilter,direction,dt);return c.look1D0;
}
float updateFov(C& c,V filteredPosition,float dt,Math& m) noexcept {
    float desired=c.currentFov1C;
    if((c.zoomIn234||c.zoomOut238)&&c.nextRoute1F8==c.routeCount1FC-1){
        const V before=c.grid[static_cast<std::size_t>(c.route[c.nextRoute1F8-1])].position;
        const V target=c.grid[static_cast<std::size_t>(c.route[c.nextRoute1F8])].position;
        const float segment=m.length(m.sub(target,before));
        const float remaining=m.length(m.sub(target,filteredPosition));
        float fraction=m.sub(1,m.div(remaining,segment));
        fraction=m.min(1,m.max(0,fraction));
        const float change=m.mul(6,fraction);
        desired=c.zoomIn234?m.sub(desired,change):m.add(desired,change);
        desired=m.min(c.maxFov04,m.max(15,desired));
    }
    c.currentFov1C=m.filter(c.fovFilter,desired,dt);return c.currentFov1C;
}
}
RaceStartStatus prepareOriginalRaceStartCamera(C& destination,const RaceStartRacerBank& bank,
    RaceStartRandomGlobals& globals,CoupledNoiseContext49M& shared,RaceStartSeedSamples samples,
    RaceStartPreparationReceipt& receipt) noexcept {
    if(!bankValid(bank))return RaceStartStatus::invalidOwner;
    if(std::fegetround()!=FE_TONEAREST)return RaceStartStatus::unsupportedNumericDomain;
    C next{};next.sessionGeneration=bank.sessionGeneration;next.racerCount=bank.racerCount;
    next.directionFilter={{{0,1.2f,1},{0,1.2f,1},{0,1.2f,1}}};
    next.positionFilter={{{0,0.6f,1},{0,0.6f,1},{0,0.6f,1}}};
    next.fovFilter={35,0.8f,1};
    for(std::size_t i=0;i<bank.racerCount;++i){next.racerKeys[i]=bank.racers[i].racerKey;next.bodyNodeKeys[i]=bank.racers[i].bodyNodeKey;}
    //2D53A4..2D53B4: fewer than eight actual racers leaves route[0]=-1.
    //The constructed child is retained, but procedural preparation did not succeed.
    //No grid access and no shared RNG draw occur on this authored branch.
    if(bank.racerCount<8){destination=next;receipt={};return RaceStartStatus::ok;}
    Math math;auto stagedGlobals=globals;auto stagedRaw=shared.sharedRandomState;
    RaceStartPreparationReceipt stagedReceipt{};
    Random random{math,stagedGlobals,stagedRaw,samples,stagedReceipt};
    grid(next,bank,math);if(!math.ok)return RaceStartStatus::unsupportedNumericDomain;
    route(next,bank,random,math);
    if(random.missing)return RaceStartStatus::missingClockSample;
    if(random.invalid)return RaceStartStatus::invalidSourceState;
    if(!math.ok)return RaceStartStatus::unsupportedNumericDomain;
    next.prepared=true;
    if(!stateValid(next))return RaceStartStatus::invalidSourceState;
    destination=next;globals=stagedGlobals;shared.sharedRandomState=stagedRaw;receipt=stagedReceipt;
    return RaceStartStatus::ok;
}
RaceStartStatus rerouteOriginalRaceStartCamera2D5890(C& destination,const RaceStartRacerBank& bank,
    RaceStartRandomGlobals& globals,CoupledNoiseContext49M& shared,RaceStartSeedSamples samples,
    RaceStartPreparationReceipt& receipt) noexcept {
    if(!matches(destination,bank)||!destination.prepared||destination.route[0]==-1)return RaceStartStatus::invalidOwner;
    if(std::fegetround()!=FE_TONEAREST)return RaceStartStatus::unsupportedNumericDomain;
    C next=destination;Math math;auto stagedGlobals=globals;auto stagedRaw=shared.sharedRandomState;
    RaceStartPreparationReceipt stagedReceipt{};
    Random random{math,stagedGlobals,stagedRaw,samples,stagedReceipt};
    route(next,bank,random,math);
    if(random.missing)return RaceStartStatus::missingClockSample;
    if(random.invalid)return RaceStartStatus::invalidSourceState;
    if(!math.ok)return RaceStartStatus::unsupportedNumericDomain;
    if(!stateValid(next))return RaceStartStatus::invalidSourceState;
    destination=next;globals=stagedGlobals;shared.sharedRandomState=stagedRaw;receipt=stagedReceipt;
    return RaceStartStatus::ok;
}
RaceStartStatus activateOriginalRaceStartCamera(C& destination,const RaceStartRacerBank& bank) noexcept {
    if(!matches(destination,bank))return RaceStartStatus::invalidOwner;
    if(!stateValid(destination))return RaceStartStatus::invalidSourceState;
    if(std::fegetround()!=FE_TONEAREST)return RaceStartStatus::unsupportedNumericDomain;
    C next=destination;Math math;activate(next,math);
    if(!math.ok)return RaceStartStatus::unsupportedNumericDomain;
    destination=next;return RaceStartStatus::ok;
}
RaceStartStatus advanceOriginalRaceStartCamera(C& destination,const RaceStartRacerBank& bank,
    float elapsed,TVCameraUpdateOutput49F& output) noexcept {
    if(!matches(destination,bank))return RaceStartStatus::invalidOwner;
    if(!stateValid(destination)||!destination.active)return RaceStartStatus::invalidSourceState;
    if(destination.complete240)return RaceStartStatus::requiresControllerHandoff;
    if(destination.nextRoute1F8<1||destination.nextRoute1F8>=destination.routeCount1FC)return RaceStartStatus::invalidSourceState;
    if(std::fegetround()!=FE_TONEAREST||!scalar(elapsed)||elapsed<0)return RaceStartStatus::unsupportedNumericDomain;
    C next=destination;auto pose=output;Math math;
    const V p=updatePosition(next,elapsed,math);
    const V look=updateLook(next,bank,p,elapsed,math);
    const float fov=updateFov(next,p,elapsed,math);
    const V middle=math.mul(math.add(next.unfilteredPosition1C0,p),0.5f);
    const V target=next.grid[static_cast<std::size_t>(next.route[next.nextRoute1F8])].position;
    const float squared=math.squared(math.sub(target,middle));
    const float radiusSquared=math.mul(next.reachRadius0C,next.reachRadius0C);
    if(squared<=radiusSquared){++next.nextRoute1F8;if(next.nextRoute1F8==next.routeCount1FC)next.complete240=true;}
    if(!math.ok)return RaceStartStatus::unsupportedNumericDomain;
    pose.mode=0;pose.position=p;pose.look=look;pose.up={0,1,0,1};pose.fov=fov;
    destination=next;output=pose;return RaceStartStatus::ok;
}
RaceStartStatus advanceOriginalRaceStartCameraView(C& destination,const RaceStartRacerBank& bank,
    float elapsed,CameraSceneState49F& scene,TVCameraUpdateOutput49F& output,
    CameraViewOutput49F& view,std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept {
    if((rootFlags228&4)||(sceneFlags29&4))return RaceStartStatus::invalidSourceState;
    auto next=destination;auto pose=output;
    //115CB0/115060 carry initialized prior scene lanes into the selected child.
    pose.look=scene.look;pose.up=scene.up;
    const auto status=advanceOriginalRaceStartCamera(next,bank,elapsed,pose);
    if(status!=RaceStartStatus::ok)return status;
    CameraViewOutput49F composed{};
    if(!composeCameraView49F(scene,pose,composed))return RaceStartStatus::unsupportedNumericDomain;
    destination=next;output=pose;view=composed;scene=composed.sceneAfter;return RaceStartStatus::ok;
}
}
