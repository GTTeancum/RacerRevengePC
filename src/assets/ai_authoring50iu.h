#pragma once
#include "pod_physics49n.h"
#include "track_start50ad.h"
#include "../game/ai_cold_owners50iu.h"
#include <cstring>
#include <cmath>
#include <cfenv>
#include <limits>

namespace rr::assets {
enum class AiAuthoringError50IU : std::uint8_t {
    None,Input,Text,Limit,Duplicate,Number,Missing,OwnerMismatch,Arithmetic,RoundingMode
};
struct AiAuthoringDiagnostic50IU {
    AiAuthoringError50IU error=AiAuthoringError50IU::None;
    std::uint32_t sourcePC=0,node=0;
    std::size_t textOffset=0;
};
namespace ai_authoring_detail50IU {
struct Slice {const char* p=nullptr;std::size_t n=0,offset=0;};
inline char lower(char c) noexcept {return c>='A'&&c<='Z'?static_cast<char>(c+32):c;}
inline bool space(char c) noexcept {return c==' '||c=='\t'||c=='\r';}
inline Slice trim(Slice s) noexcept {
    while(s.n&&space(*s.p)){++s.p;--s.n;++s.offset;}
    while(s.n&&space(s.p[s.n-1]))--s.n;return s;
}
inline bool equal(Slice a,Slice b) noexcept {
    if(a.n!=b.n)return false;
    for(std::size_t i=0;i<a.n;++i)if(lower(a.p[i])!=lower(b.p[i]))return false;
    return true;
}
inline bool equal(Slice a,const char* b) noexcept {return equal(a,{b,std::strlen(b),0});}
inline bool fail(AiAuthoringDiagnostic50IU& d,AiAuthoringError50IU e,
    std::uint32_t pc,std::size_t offset) noexcept {d.error=e;d.sourcePC=pc;d.textOffset=offset;return false;}
inline bool integer(Slice s,std::int32_t& out) noexcept {
    if(!s.n)return false;std::size_t at=0;bool negative=s.p[0]=='-';
    if(negative||s.p[0]=='+')++at;if(at==s.n)return false;
    std::uint64_t n=0;
    for(;at<s.n;++at){if(s.p[at]<'0'||s.p[at]>'9')return false;
        n=n*10u+static_cast<unsigned>(s.p[at]-'0');
        if(n>(negative?2147483648ull:2147483647ull))return false;}
    if(negative&&n==2147483648ull)out=std::numeric_limits<std::int32_t>::min();
    else out=negative?-static_cast<std::int32_t>(n):static_cast<std::int32_t>(n);
    return true;
}
// Cold native admission only: ASCII INI, case-insensitive names, whole-line
// semicolon comments, no duplicate sections/keys. Original permits broader
// strings and first-match duplicates; unsupported text fails explicitly.
// All work is bounded, serial and allocation-free. Unknown keys are not read.
template<class Visit>
inline bool scan(const char* text,std::size_t bytes,std::size_t maximum,
    std::uint32_t pc,Visit&& visit,AiAuthoringDiagnostic50IU& d) noexcept {
    if(!text||!bytes)return fail(d,AiAuthoringError50IU::Input,pc,0);
    if(bytes>maximum)return fail(d,AiAuthoringError50IU::Limit,pc,0);
    for(std::size_t i=0;i<bytes;++i){const auto c=static_cast<unsigned char>(text[i]);
        if((c<32&&c!=9&&c!=10&&c!=13)||c>126)
            return fail(d,AiAuthoringError50IU::Text,pc,i);}
    Slice sections[257]{},keys[128]{};std::size_t sectionsUsed=0,keysUsed=0,at=0;
    Slice section{};
    while(at<bytes){const auto start=at;while(at<bytes&&text[at]!='\n')++at;
        auto line=trim({text+start,at-start,start});if(at<bytes)++at;
        if(line.n>255)return fail(d,AiAuthoringError50IU::Limit,pc,start);
        if(!line.n||line.p[0]==';')continue;
        if(line.p[0]=='['){
            if(line.n<3||line.p[line.n-1]!=']')return fail(d,AiAuthoringError50IU::Text,pc,start);
            section=trim({line.p+1,line.n-2,line.offset+1});
            if(!section.n)return fail(d,AiAuthoringError50IU::Text,pc,start);
            for(std::size_t i=0;i<sectionsUsed;++i)if(equal(section,sections[i]))
                return fail(d,AiAuthoringError50IU::Duplicate,pc,start);
            if(sectionsUsed==257)return fail(d,AiAuthoringError50IU::Limit,pc,start);
            sections[sectionsUsed++]=section;keysUsed=0;
            if(!visit(section,{},{}))return false;continue;
        }
        if(!sectionsUsed)return fail(d,AiAuthoringError50IU::Text,pc,start);
        std::size_t split=0;while(split<line.n&&line.p[split]!='=')++split;
        if(split==line.n)return fail(d,AiAuthoringError50IU::Text,pc,start);
        const auto key=trim({line.p,split,line.offset});
        const auto value=trim({line.p+split+1,line.n-split-1,line.offset+split+1});
        if(!key.n||!value.n)return fail(d,AiAuthoringError50IU::Text,pc,start);
        for(std::size_t i=0;i<keysUsed;++i)if(equal(key,keys[i]))
            return fail(d,AiAuthoringError50IU::Duplicate,pc,start);
        if(keysUsed==128)return fail(d,AiAuthoringError50IU::Limit,pc,start);
        keys[keysUsed++]=key;if(!visit(section,key,value))return false;
    }
    return true;
}
} // namespace ai_authoring_detail50IU

// Call with actual selected Pods\\podNN\\podNN.ai text. Constructor defaults
// are retained on absent sections/keys, as158A8C/15893C skip the store. The
// selected profile is immutable metadata; runtime timers have separate owners.
// Apply applyAiCreateClass50IU AFTER this16E630 load, exactly as16E5CC does.
inline bool readAiProfile50IU(const char* text,std::size_t bytes,
    rr::game::AiProfile50IU& output,AiAuthoringDiagnostic50IU& d) noexcept {
    using namespace ai_authoring_detail50IU;using P=rr::game::AiProfile50IU;
    d={};P result{};
    struct F {const char* section;const char* key;float P::* member;unsigned bit;};
    constexpr F floats[]={
        {"combat","Combativeness",&P::combativeness1054,0},
        {"combat","HealthConsciousness",&P::healthConsciousness105C,2},
        {"combat","WaitToRepairTime",&P::waitToRepairTime1060,3},
        {"combat","RepairTime",&P::repairTime1064,4},
        {"combat","AttackRecoveryTime",&P::attackRecoveryTime1068,5},
        {"combat","StartRaceNoCombatTime",&P::startRaceNoCombatTime106C,6},
        {"combat","HoverBeforeAttackTime",&P::hoverBeforeAttackTime1070,7},
        {"combat","FrontHoverPosition",&P::frontHoverPosition1074,8},
        {"combat","SideHoverPosition",&P::sideHoverPosition1078,9},
        {"combat","CombatZoneLength",&P::combatZoneLength107C,10},
        {"driving","DriveAggression",&P::driveAggression1088,12}};
    auto visit=[&](Slice section,Slice key,Slice value) noexcept {
        if(!key.n)return true;
        for(const auto& f:floats)if(equal(section,f.section)&&equal(key,f.key)){
            if(!parsePodPhysicsNumber49N(value.p,value.n,result.*f.member))
                return fail(d,AiAuthoringError50IU::Number,0x158a70,value.offset);
            result.loadedMask|=1u<<f.bit;return true;}
        std::int32_t* dest=nullptr;unsigned bit=0;
        if(equal(section,"combat")&&equal(key,"TargetPersistence")){dest=&result.targetPersistence1058;bit=1;}
        if(equal(section,"combat")&&equal(key,"CollisionRadius")){dest=&result.collisionRadius1020;bit=11;}
        if(equal(section,"driving")&&equal(key,"MyMinSpeed")){dest=&result.myMinSpeed108C;bit=13;}
        if(dest){if(!integer(value,*dest))return fail(d,AiAuthoringError50IU::Number,0x158920,value.offset);
            result.loadedMask|=1u<<bit;}return true;
    };
    if(!scan(text,bytes,maxPodPhysicsBytes49N,0x16e630,visit,d))return false;
    output=result;return true;
}

// Cold projection of original2A0F30/2A1260 metadata omitted by50EU. Node0-based
// input only, matching that reader. Flags are set when integer lookup succeeds,
// even for authored0, because source2A1678 etc use constant OR masks.
// BoostScale uses the source INTEGER reader then signed CVT.S.W, not a float
// lookup. Admit exactly representable integers here; other ranges fail closed.
inline bool readAiTrackMetadata50IU(const char* text,std::size_t bytes,
    const rr::game::TrackNavigation50EU& track,rr::game::AiTrackMetadata50IU& output,
    AiAuthoringDiagnostic50IU& d) noexcept {
    using namespace ai_authoring_detail50IU;using namespace rr::game;
    d={};if(!track.gatesPrepared||track.track.count<2||track.track.count>maxUndertrackNodes50ET)
        return fail(d,AiAuthoringError50IU::Input,0x2a1260,0);
    AiTrackMetadata50IU result{};result.count=track.track.count;
    bool seen[maxUndertrackNodes50ET]{};bool countSeen=false;std::int32_t current=-1;
    auto visit=[&](Slice section,Slice key,Slice value) noexcept {
        if(!key.n){current=-1;
            if(equal(section,"Track"))return true;
            if(section.n<=4||!equal({section.p,4,section.offset},"Node"))
                return fail(d,AiAuthoringError50IU::Text,0x2a14d4,section.offset);
            if(!integer({section.p+4,section.n-4,section.offset+4},current)||current<0||
               static_cast<std::uint32_t>(current)>=result.count)
                return fail(d,AiAuthoringError50IU::Number,0x2a14d4,section.offset);
            if(seen[current])return fail(d,AiAuthoringError50IU::Duplicate,0x2a14d4,section.offset);
            seen[current]=true;return true;}
        if(current<0){if(equal(key,"NumNodes")){std::int32_t count=0;
                if(!integer(value,count)||count!=static_cast<std::int32_t>(result.count))
                    return fail(d,AiAuthoringError50IU::OwnerMismatch,0x2a1334,value.offset);
                countSeen=true;}
            return true;}
        d.node=static_cast<std::uint32_t>(current);auto& node=result.nodes[current];
        float* scalar=nullptr;
        if(equal(key,"SpeedLimit"))scalar=&node.speedLimit5C;
        if(equal(key,"DistanceToFinish"))scalar=&node.distanceToFinish60;
        if(scalar){if(!parseTrackNumber50AD(value.p,value.n,*scalar))
                return fail(d,AiAuthoringError50IU::Number,0x158a70,value.offset);return true;}
        if(equal(key,"BoostScale")){std::int32_t integerValue=0;
            if(!integer(value,integerValue)||integerValue< -16777216||integerValue>16777216)
                return fail(d,AiAuthoringError50IU::Number,0x2a16a8,value.offset);
            node.boostScale64=static_cast<float>(integerValue);return true;}
        if(equal(key,"BranchID")){if(!integer(value,node.branch68))
                return fail(d,AiAuthoringError50IU::Number,0x2a18cc,value.offset);return true;}
        const char* names[]={"NodeBlocked","NoCombatZone","ShortestBranch","SkipPlayerTest",
            "DriveFullSpeed","SecretBranch","Pretty","Ugly"};
        for(unsigned bit=0;bit<8;++bit)if(equal(key,names[bit])){std::int32_t ignored=0;
            if(!integer(value,ignored))return fail(d,AiAuthoringError50IU::Number,0x158920,value.offset);
            node.flags79=static_cast<std::uint8_t>(node.flags79|(1u<<bit));return true;}
        if(equal(key,"ProgressNode")){std::int32_t ignored=0;
            if(!integer(value,ignored))return fail(d,AiAuthoringError50IU::Number,0x2a18fc,value.offset);
            node.progressNode=true;}return true;
    };
    if(!scan(text,bytes,maxUndertrackText50ET,0x2a1260,visit,d))return false;
    if(!countSeen)return fail(d,AiAuthoringError50IU::Missing,0x2a1334,0);
    for(std::uint32_t n=0;n<result.count;++n){d.node=n;
        if(!seen[n])return fail(d,AiAuthoringError50IU::Missing,0x2a14d4,0);
        //50EU owns geometry/branch independently; catch joining another file.
        if(result.nodes[n].branch68!=track.nodes[n].branch68||
           (result.nodes[n].flags79!=track.nodes[n].flags79))
            return fail(d,AiAuthoringError50IU::OwnerMismatch,0x2a18ec,0);}
    //2A1BB8..2A1C5C uses node0, its ACTUAL Next0, full XYZ distance,
    //then that next node's authored DistanceToFinish. No HOST course sum.
    if(std::fegetround()!=FE_TONEAREST)
        return fail(d,AiAuthoringError50IU::RoundingMode,0x2a1c14,0);
    if(!track.track.nodes[0].nextCount)return fail(d,AiAuthoringError50IU::Missing,0x2a26c0,0);
    const auto next=track.track.nodes[0].next[0];
    if(next>=result.count)return fail(d,AiAuthoringError50IU::OwnerMismatch,0x2a26c0,0);
    const auto& p0=track.track.nodes[0].position;
    const auto& p1=track.track.nodes[next].position;
    auto ordinary=[](float value) noexcept {return std::isfinite(value)&&(value==0.f||std::isnormal(value));};
    for(unsigned axis=0;axis<3;++axis)if(!ordinary(p0[axis])||!ordinary(p1[axis]))
        return fail(d,AiAuthoringError50IU::Arithmetic,0x2a1c14,0);
    volatile float dy=p1[1]-p0[1],dz=p1[2]-p0[2],dx=p1[0]-p0[0];
    volatile float xx=dx*dx,yy=dy*dy,xy=xx+yy,zz=dz*dz,sum=xy+zz;
    volatile float length=std::sqrt(static_cast<float>(sum));
    volatile float total=length+result.nodes[next].distanceToFinish60;
    const float intermediates[]={dy,dz,dx,xx,yy,xy,zz,sum,length,total};
    for(float value:intermediates)if(!ordinary(value))
        return fail(d,AiAuthoringError50IU::Arithmetic,0x2a1c58,0);
    result.trackLength3C=total;
    result.bound=true;output=result;d={};return true;
}

inline bool readAiTrackTuning50IU(const char* text,std::size_t bytes,
    rr::game::AiTrackTuning50IU& output,AiAuthoringDiagnostic50IU& d) noexcept {
    using namespace ai_authoring_detail50IU;using P=rr::game::AiTrackTuning50IU;
    d={};P result{};
    struct F {const char* section;const char* key;float P::* member;unsigned bit;};
    constexpr F floats[]={
        {"PodDamage","WaitToRepairEnvTime",&P::waitToRepairEnvTime6C,13},
        {"PodDamage","RepairEnvUntil",&P::repairEnvUntil70,14},
        {"PodDamage","RepairEnvThreshold",&P::repairEnvThreshold74,15},
        {"AIDriving","Lookahead",&P::lookahead78,0},
        {"AIDriving","RacePace",&P::racePace7C,1},
        {"AIDriving","RacePaceCutoffTime",&P::racePaceCutoffTime80,2},
        {"AIDriving","BehindPlayerClearance",&P::behindPlayerClearance84,3},
        {"AIDriving","StartLineBoostTime",&P::startLineBoostTime88,4},
        {"Packing","FarFrontDist",&P::farFrontDist90,7},
        {"Packing","CloseFrontDist",&P::closeFrontDist94,8},
        {"Packing","FarBehindDist",&P::farBehindDist98,9},
        {"Packing","CloseBehindDist",&P::closeBehindDist9C,10},
        {"Packing","SlowestSpeedPct",&P::slowestSpeedPctA0,11},
        {"Packing","FastestSpeedPct",&P::fastestSpeedPctA4,12}};
    auto visit=[&](Slice section,Slice key,Slice value) noexcept {
        if(!key.n)return true;
        for(const auto& f:floats)if(equal(section,f.section)&&equal(key,f.key)){
            if(!parseTrackNumber50AD(value.p,value.n,result.*f.member))
                return fail(d,AiAuthoringError50IU::Number,0x158a70,value.offset);
            result.loadedMask|=1u<<f.bit;return true;}
        if(equal(section,"Packing")&&(equal(key,"NumPackingFront")||equal(key,"NumPackingRear"))){
            std::int32_t value32=0;if(!integer(value,value32))
                return fail(d,AiAuthoringError50IU::Number,0x158920,value.offset);
            // Source SB truncates to8 bits, later LB sign-extends.
            const auto byte=static_cast<std::uint32_t>(value32)&255u;
            const auto signedByte=static_cast<std::int8_t>(byte<128?static_cast<int>(byte):static_cast<int>(byte)-256);
            if(equal(key,"NumPackingFront")){result.numPackingFront8C=signedByte;result.loadedMask|=1u<<5;}
            else {result.numPackingRear8D=signedByte;result.loadedMask|=1u<<6;}}
        return true;
    };
    if(!scan(text,bytes,rr::game::maxUndertrackText50ET,0x1aa5d0,visit,d))return false;
    result.bound=true;output=result;return true;
}
} // namespace rr::assets
