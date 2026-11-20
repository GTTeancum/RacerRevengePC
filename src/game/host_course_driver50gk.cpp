#include "host_course_driver50gk.h"
#include "../assets/track_start50ad.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace rr::host { namespace {
using Status=CourseDriverStatus50GK;
// The authored route as read by this host aid: the one common map/AI view's
// track nodes and 2A1D80 gates (every course; counts are the file's).
struct Route {
    const game::UndertrackNode50ET* trackNodes=nullptr;
    const game::TrackNavigationNode50EU* gates=nullptr;
    std::uint32_t count=0;
    bool gatesPrepared=false;
};
struct Slice {const char* p;std::size_t n;};
Slice trim(Slice s) noexcept {
    auto space=[](char c){return c==' '||c=='\t'||c=='\r';};
    while(s.n&&space(*s.p)){++s.p;--s.n;}
    while(s.n&&space(s.p[s.n-1]))--s.n;
    return s;
}
bool equal(Slice s,const char* p) noexcept {
    if(s.n!=std::strlen(p))return false;
    for(std::size_t i=0;i<s.n;++i){char a=s.p[i],b=p[i];
        if(a>='A'&&a<='Z')a=static_cast<char>(a+32);
        if(b>='A'&&b<='Z')b=static_cast<char>(b+32);
        if(a!=b)return false;}
    return true;
}
bool index(Slice s,std::uint32_t& out) noexcept {
    if(!s.n||s.n>3)return false;
    std::uint32_t v=0;
    for(std::size_t i=0;i<s.n;++i){if(s.p[i]<'0'||s.p[i]>'9')return false;
        v=v*10u+static_cast<unsigned>(s.p[i]-'0');}
    out=v;return true;
}
bool finite(float x) noexcept {return std::isfinite(x);}
float length(float x,float z) noexcept {return std::hypot(x,z);}
float clamp(float x,float a,float b) noexcept {return std::clamp(x,a,b);}
bool validTuning(const CourseDriverTuning50GK& t) noexcept {
    const float positive[]={t.cruiseSpeed,t.minimumCornerSpeed,t.speedLimitToBodyScale,
        t.lookaheadBase,t.maximumLookahead,t.headingGain,t.speedGain,t.narrowHalfWidth,
        t.offCourseMargin,t.maximumVerticalError,t.progressDistance,t.stuckSeconds,
        t.maximumSecondsPerNode};
    for(float x:positive)if(!finite(x)||x<=0.f)return false;
    return t.minimumCornerSpeed<=t.cruiseSpeed&&t.lookaheadBase<=t.maximumLookahead&&
        finite(t.lookaheadSeconds)&&t.lookaheadSeconds>=0.f&&
        finite(t.authoredSteeringLookaheadSeconds)&&t.authoredSteeringLookaheadSeconds>=0.f&&
        finite(t.yawDamping)&&t.yawDamping>=0.f&&
        finite(t.turnThrottleReduction)&&t.turnThrottleReduction>=0.f&&
        t.turnThrottleReduction<=1.f&&
        (t.steeringPolarity==1.f||t.steeringPolarity==-1.f);
}
bool stop(Status status,CourseDriverDiagnostic50GK& d) noexcept {d.status=status;return false;}
bool latch(Status status,CourseDriverState50GK& s,CourseDriverDiagnostic50GK& d) noexcept {
    s.stopped=true;s.stopReason=status;return stop(status,d);
}
bool routeValid(const Route& route,const CourseSpeedLimits50GK& limits) noexcept {
    const auto count=route.count;
    if(!route.gatesPrepared||count<2||count>game::maxUndertrackNodes50ET||limits.count!=count)return false;
    for(std::uint32_t i=0;i<count;++i){const auto& n=route.trackNodes[i];
        if(!n.nextCount||n.nextCount>game::maxUndertrackNext50ET)return false;
        for(std::uint32_t j=0;j<n.nextCount;++j)if(n.next[j]>=count)return false;
        for(unsigned j=0;j<3;++j)if(!finite(n.position[j]))return false;
        if(!finite(route.gates[i].left54)||!finite(route.gates[i].right58)||
           !finite(limits.values[i])||limits.values[i]<0.f)return false;}
    return true;
}
float halfWidth(const Route& route,std::uint32_t node) noexcept {
    //50EU has already applied TrackWidth/2 to the authored per-side offsets.
    return std::max(1.f,.5f*(route.gates[node].left54+route.gates[node].right58));
}
// Input-only route sampling. Never changes canonical navigation, the graph,
// speed metadata, safety width or a vehicle pose. At most one authored lap.
bool steeringPointAhead(const Route& route,std::uint32_t node,
    const float* start,float distance,float* aim,std::uint32_t& segments) noexcept {
    if(!finite(distance)||distance<=0.f)return false;
    float point[3]={start[0],start[1],start[2]};segments=0;
    for(std::uint32_t visits=0;visits<route.count;++visits){
        const auto& n=route.trackNodes[node];
        const float span=length(n.position[0]-point[0],n.position[2]-point[2]);
        if(!finite(span))return false;
        if(span>.001f){
            if(distance<=span){const float a=distance/span;
                for(unsigned i=0;i<3;++i)aim[i]=point[i]+(n.position[i]-point[i])*a;
                return true;}
            distance-=span;
        }
        for(unsigned i=0;i<3;++i)point[i]=n.position[i];
        node=n.next[0];++segments;
    }
    return false;
}
} // namespace

bool readCourseSteeringLookahead50GK(const char* text,std::size_t bytes,float& out,
    CourseDriverDiagnostic50GK& d) noexcept {
    d={};
    if(!text||!bytes||bytes>game::maxUndertrackText50ET)return stop(Status::InvalidText,d);
    bool inSection=false,seenSection=false,seen=false;
    float value=0.f;std::size_t at=0;
    while(at<bytes){const auto begin=at;while(at<bytes&&text[at]!='\n')++at;
        auto line=trim({text+begin,at-begin});if(at<bytes)++at;d.textOffset=begin;
        if(!line.n||line.p[0]==';')continue;
        if(line.p[0]=='['){
            if(line.n<3||line.p[line.n-1]!=']')return stop(Status::InvalidText,d);
            inSection=equal(trim({line.p+1,line.n-2}),"AIDriving");
            if(inSection){if(seenSection)return stop(Status::InvalidText,d);seenSection=true;}
            continue;
        }
        if(!inSection)continue;
        std::size_t eq=0;while(eq<line.n&&line.p[eq]!='=')++eq;
        if(eq==line.n)return stop(Status::InvalidText,d);
        if(!equal(trim({line.p,eq}),"Lookahead"))continue;
        const auto token=trim({line.p+eq+1,line.n-eq-1});
        if(seen||!assets::parseTrackNumber50AD(token.p,token.n,value)||
           !finite(value)||value<=0.f)return stop(Status::InvalidText,d);
        seen=true;
    }
    if(!seen)return stop(Status::InvalidText,d);
    out=value;d={};return true;
}

bool readCourseSpeedLimits50GK(const char* text,std::size_t bytes,std::uint32_t count,
    CourseSpeedLimits50GK& out,CourseDriverDiagnostic50GK& d) noexcept {
    d={};
    if(!text||!bytes||bytes>game::maxUndertrackText50ET||count<2||count>game::maxUndertrackNodes50ET)
        return stop(Status::InvalidText,d);
    CourseSpeedLimits50GK next{};next.count=count;
    bool seen[game::maxUndertrackNodes50ET]{};
    int section=-1;std::size_t at=0;
    while(at<bytes){const auto begin=at;while(at<bytes&&text[at]!='\n')++at;
        auto line=trim({text+begin,at-begin});if(at<bytes)++at;d.textOffset=begin;
        if(!line.n||line.p[0]==';')continue;
        if(line.p[0]=='['){
            if(line.n<3||line.p[line.n-1]!=']')return stop(Status::InvalidText,d);
            auto name=trim({line.p+1,line.n-2});section=-1;
            if(equal(name,"Track"))continue;
            std::uint32_t node=0;
            if(name.n<=4||!equal({name.p,4},"Node")||!index({name.p+4,name.n-4},node)||node>=count)
                return stop(Status::InvalidText,d);
            section=static_cast<int>(node);continue;
        }
        std::size_t eq=0;while(eq<line.n&&line.p[eq]!='=')++eq;
        if(eq==line.n)return stop(Status::InvalidText,d);
        if(!equal(trim({line.p,eq}),"SpeedLimit"))continue;
        if(section<0||seen[section])return stop(Status::InvalidText,d);
        const auto value=trim({line.p+eq+1,line.n-eq-1});
        float speed=0.f;
        if(!assets::parseTrackNumber50AD(value.p,value.n,speed)||
           !finite(speed)||speed<0.f)return stop(Status::InvalidText,d);
        seen[section]=true;next.values[section]=speed;
    }
    for(std::uint32_t i=0;i<count;++i)if(!seen[i]){d.node=i;return stop(Status::InvalidText,d);}
    out=next;d={};return true;
}

static bool driveRoute50GK(const Route& route,const CourseSpeedLimits50GK& limits,
    const CourseDriverTuning50GK& t,const CourseDriverInput50GK& in,
    CourseDriverState50GK& state,CourseDriverOutput50GK& out,CourseDriverDiagnostic50GK& d) noexcept {
    out={};d={};d.node=in.currentNode;
    if(!in.enabled){state={};return stop(Status::Disabled,d);}
    if(state.stopped)return stop(state.stopReason,d);
    if(!validTuning(t)||!finite(in.elapsedSeconds)||in.elapsedSeconds<0.f||in.elapsedSeconds>.25f||
       !finite(in.yawRate))return stop(Status::InvalidInput,d);
    for(float x:in.pose.v)if(!finite(x))return stop(Status::InvalidInput,d);
    for(float x:in.worldVelocity.v)if(!finite(x))return stop(Status::InvalidInput,d);
    if(!routeValid(route,limits)||in.currentNode>=route.count||in.previousNode>=route.count||
       in.currentNode==in.previousNode)return stop(Status::InvalidRoute,d);
    bool connected=false;
    const auto& previous=route.trackNodes[in.previousNode];
    for(std::uint32_t i=0;i<previous.nextCount;++i)connected|=previous.next[i]==in.currentNode;
    if(!connected)return stop(Status::InvalidRoute,d);
    const auto& current=route.trackNodes[in.currentNode];
    const float px=in.pose.v[12],py=in.pose.v[13],pz=in.pose.v[14];
    const float hx=in.pose.v[8],hz=in.pose.v[10],headingLength=length(hx,hz);
    const float sx=current.position[0]-previous.position[0],sz=current.position[2]-previous.position[2];
    const float segmentLength=length(sx,sz);
    if(!finite(headingLength)||headingLength<.1f||!finite(segmentLength)||segmentLength<.001f)
        return stop(Status::InvalidInput,d);
    const float fx=hx/headingLength,fz=hz/headingLength;
    const float tx=sx/segmentLength,tz=sz/segmentLength;
    const float along=clamp((px-previous.position[0])*tx+(pz-previous.position[2])*tz,0.f,segmentLength);
    const float fraction=along/segmentLength;
    float point[3]={previous.position[0]+tx*along,
        previous.position[1]+(current.position[1]-previous.position[1])*fraction,
        previous.position[2]+tz*along};
    const float steeringStart[3]={point[0],point[1],point[2]};
    d.crossTrack=length(px-point[0],pz-point[2]);
    d.halfWidth=halfWidth(route,in.currentNode);
    d.bodySpeed=length(in.worldVelocity.v[0],in.worldVelocity.v[2]);
    if(!finite(d.crossTrack)||!finite(d.halfWidth)||!finite(d.bodySpeed))return stop(Status::InvalidInput,d);
    if(d.crossTrack>d.halfWidth+t.offCourseMargin||std::fabs(py-point[1])>t.maximumVerticalError)
        return latch(Status::OffCourse,state,d);

    // Follow only authored Next0, starting from the current SOURCE-owned
    // segment. Traversal is for choosing an input target, never progression.
    float lookahead=clamp(t.lookaheadBase+t.lookaheadSeconds*d.bodySpeed,t.lookaheadBase,t.maximumLookahead);
    lookahead=std::min(lookahead,std::max(t.lookaheadBase,d.halfWidth*1.25f));
    d.speedLookahead=lookahead;d.steeringLookahead=lookahead;
    float minimumWidth=d.halfWidth,minimumLimit=limits.values[in.currentNode];
    float bend=0.f,lastX=tx,lastZ=tz;
    std::uint32_t node=in.currentNode;
    bool found=false;
    for(std::uint32_t visits=0;visits<route.count;++visits){
        const auto& n=route.trackNodes[node];
        const float dx=n.position[0]-point[0],dz=n.position[2]-point[2];
        const float distance=length(dx,dz);
        if(!finite(distance))return stop(Status::InvalidRoute,d);
        minimumWidth=std::min(minimumWidth,halfWidth(route,node));
        minimumLimit=std::min(minimumLimit,limits.values[node]);
        if(distance>.001f){
            const float nx=dx/distance,nz=dz/distance;
            bend=std::max(bend,std::acos(clamp(lastX*nx+lastZ*nz,-1.f,1.f)));
            lastX=nx;lastZ=nz;
            if(lookahead<=distance){const float a=lookahead/distance;
                for(unsigned i=0;i<3;++i)d.aim[i]=point[i]+(n.position[i]-point[i])*a;
                found=true;break;}
            lookahead-=distance;
        }
        for(unsigned i=0;i<3;++i)point[i]=n.position[i];
        node=n.next[0];++d.lookaheadSegments;
    }
    if(!found)return stop(Status::InvalidRoute,d);
    const float aimX=d.aim[0]-px,aimZ=d.aim[2]-pz;
    if(length(aimX,aimZ)<.001f)return stop(Status::InvalidInput,d);
    // Positive geometric error is toward positive local X for a +Z-forward
    // upright body. Input polarity is explicit; no guessed physical labels.
    d.headingError=std::atan2(fz*aimX-fx*aimZ,fx*aimX+fz*aimZ);
    if(t.authoredSpeedOnly){
        d.targetSpeed=minimumLimit*t.speedLimitToBodyScale;
    } else {
        const float severity=std::max(std::fabs(d.headingError),bend);
        const float corner=clamp(1.f-severity/1.3f,0.f,1.f);
        const float widthFactor=clamp(minimumWidth/t.narrowHalfWidth,.35f,1.f);
        d.targetSpeed=t.minimumCornerSpeed+(t.cruiseSpeed-t.minimumCornerSpeed)*corner*widthFactor;
        d.targetSpeed=std::min(d.targetSpeed,minimumLimit*t.speedLimitToBodyScale);
    }
    d.steeringSegments=d.lookaheadSegments;
    if(t.authoredSteeringLookaheadSeconds>0.f){
        // Source 172380 uses velocity direction and two speed-scaled previews:
        // actual AIDriving.Lookahead and 0.2s. This HOST centerline projection
        // retains that input-planning idea, not the complete native AI. The
        // short legacy preview above STILL owns the unchanged target speed.
        const float horizons[2]={
            std::max(t.lookaheadBase,t.authoredSteeringLookaheadSeconds*d.bodySpeed),
            std::max(t.lookaheadBase,.2f*d.bodySpeed)};
        const float vx=d.bodySpeed>.001f?in.worldVelocity.v[0]/d.bodySpeed:fx;
        const float vz=d.bodySpeed>.001f?in.worldVelocity.v[2]/d.bodySpeed:fz;
        float bestAlignment=2.f;
        for(unsigned sample=0;sample<2;++sample){
            float aim[3]{};std::uint32_t segments=0;
            if(!steeringPointAhead(route,in.currentNode,steeringStart,horizons[sample],aim,segments))
                return stop(Status::InvalidRoute,d);
            const float dx=aim[0]-px,dz=aim[2]-pz,span=length(dx,dz);
            if(!finite(span)||span<.001f)return stop(Status::InvalidInput,d);
            const float alignment=(vx*dx+vz*dz)/span;
            if(!finite(alignment))return stop(Status::InvalidInput,d);
            if(alignment<=bestAlignment){
                bestAlignment=alignment;
                for(unsigned i=0;i<3;++i)d.aim[i]=aim[i];
                d.steeringLookahead=horizons[sample];d.steeringSegments=segments;
            }
        }
        d.headingError=std::atan2(fz*(d.aim[0]-px)-fx*(d.aim[2]-pz),
            fx*(d.aim[0]-px)+fz*(d.aim[2]-pz));
    }
    const float geometricTurn=clamp(t.headingGain*d.headingError-t.yawDamping*in.yawRate,-1.f,1.f);
    d.turn=t.steeringPolarity*geometricTurn;
    if(!finite(d.headingError)||!finite(d.turn)||!finite(d.targetSpeed))return stop(Status::InvalidInput,d);

    if(!state.initialized){state.initialized=true;state.observedNode=in.currentNode;
        state.checkpoint[0]=px;state.checkpoint[1]=py;state.checkpoint[2]=pz;}
    auto next=state;
    if(in.currentNode!=next.observedNode){next.observedNode=in.currentNode;next.nodeSeconds=0.f;}
    else next.nodeSeconds+=in.elapsedSeconds;
    // Actual spatial displacement only. A separate node deadline prevents a
    // circle/sliding loop from continually resetting this short stall timer.
    if(length(px-next.checkpoint[0],pz-next.checkpoint[2])>=t.progressDistance){
        next.checkpoint[0]=px;next.checkpoint[1]=py;next.checkpoint[2]=pz;next.stalledSeconds=0.f;
    } else next.stalledSeconds+=in.elapsedSeconds;
    d.stalledSeconds=next.stalledSeconds;
    state=next;
    if(next.stalledSeconds>=t.stuckSeconds||next.nodeSeconds>=t.maximumSecondsPerNode)
        return latch(Status::Stuck,state,d);

    float demand=clamp((d.targetSpeed-d.bodySpeed)*t.speedGain,-1.f,1.f);
    const float turnMagnitude=std::fabs(d.turn);
    if(!t.authoredSpeedOnly&&demand>0.f)demand*=1.f-t.turnThrottleReduction*turnMagnitude;
    d.speedDemand=demand;
    // Independent axis clipping at demand=1 made a full turn emit1/0: only
    // the engine half of original preset4 steering, with no inside airbrake.
    // Opt-in steering saturation allocation keeps the requested2*turn span;
    // it necessarily lowers common acceleration demand when both will not fit.
    // Full positive demand/full turn now emits1/-1 through the ordinary packet
    // reader, rate filter and native engine/aero binding. No model writes.
    float commonDemand=demand;
    if(t.fullThrottleSteeringRange&&demand>0.f)
        commonDemand=std::min(demand,1.f-turnMagnitude);
    d.mixedDemand=commonDemand;
    out.left=clamp(commonDemand+d.turn,-1.f,1.f);
    out.right=clamp(commonDemand-d.turn,-1.f,1.f);
    d.status=Status::Driving;return true;
}

bool driveCourse50GK(const game::NavigationView50CR& view,const CourseSpeedLimits50GK& limits,
    const CourseDriverTuning50GK& t,const CourseDriverInput50GK& in,
    CourseDriverState50GK& state,CourseDriverOutput50GK& out,CourseDriverDiagnostic50GK& d) noexcept {
    const Route route{view.nodes(),view.gates(),view.count(),view.runtimeValid()&&view.gatesPrepared()&&view.gates()};
    return driveRoute50GK(route,limits,t,in,state,out,d);
}

bool applyCourseThrottlePacket50GK(const CourseDriverOutput50GK& out,game::DevicePacket48Y& packet) noexcept {
    if(!finite(out.left)||!finite(out.right)||out.left< -1.f||out.left>1.f||out.right< -1.f||out.right>1.f)
        return false;
    auto axis=[](float demand){const float raw=demand>=0.f?128.f-128.f*demand:128.f-127.f*demand;
        return static_cast<std::uint8_t>(std::lround(raw));};
    packet.bytes[7]=axis(out.left);packet.bytes[5]=axis(out.right);return true;
}
}
