#include "pod_physics49n.h"
#include <cstring>
#include <limits>

namespace rr::assets {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
              "Pod physics requires IEEE binary32");
bool parsePodPhysicsNumber49N(const char* text, std::size_t bytes, float& output,
                             std::uint64_t* binary64Bits) noexcept {
    if (!text || !bytes || bytes > 11) return false;
    std::size_t at=0, digits=0, fractional=0;
    const bool negative=text[0]=='-';
    if (negative || text[0]=='+') ++at;
    const std::size_t first=at;
    std::uint64_t n=0, d=1;
    while (at<bytes && text[at]>='0' && text[at]<='9') {
        if (++digits>9) return false;
        n=n*10+static_cast<unsigned>(text[at++]-'0');
    }
    if (at==first) return false;
    if (at<bytes && text[at]=='.') {
        ++at; const std::size_t start=at;
        while (at<bytes && text[at]>='0' && text[at]<='9') {
            if (++digits>9 || ++fractional>6) return false;
            n=n*10+static_cast<unsigned>(text[at++]-'0'); d*=10;
        }
        if (at==start) return false;
    }
    if (at!=bytes) return false;
    std::uint64_t binary64=0;
    std::uint32_t binary32=0;
    if (n) {
        // n and d are exact nonnegative integers <=999999999 and1000000.
        // Normalize a/b into[1,2), then generate53 bits by long division.
        // Remainder compares implement nearest-even without host arithmetic.
        int exponent=0;
        while (n<d) { n*=2; --exponent; }
        while (n>=d*2) { d*=2; ++exponent; }
        std::uint64_t remainder=n-d, significand=1;
        for (unsigned i=0;i<52;++i) {
            remainder*=2; significand*=2;
            if (remainder>=d) { ++significand; remainder-=d; }
        }
        if (remainder*2>d || (remainder*2==d && (significand&1))) ++significand;
        if (significand==(std::uint64_t(1)<<53)) { significand>>=1; ++exponent; }
        binary64=(std::uint64_t(negative)<<63) |
            (std::uint64_t(exponent+1023)<<52) |
            (significand&((std::uint64_t(1)<<52)-1));
        // All admitted nonzero values remain normal binary32, so only the
        // original310498 normal tie-even branch is reachable after310440.
        std::uint32_t m=static_cast<std::uint32_t>(significand>>22);
        if (significand&((std::uint64_t(1)<<22)-1)) m|=1;
        if ((m&127u)==64u) { if (m&128u) m+=64u; } else m+=63u;
        if (m&0x80000000u) { m>>=1; ++exponent; }
        binary32=(std::uint32_t(negative)<<31) |
            (std::uint32_t(exponent+127)<<23) | ((m>>7)&0x7fffffu);
    }
    std::memcpy(&output,&binary32,4);
    if (binary64Bits) *binary64Bits=binary64;
    return true;
}
namespace {
using Key=rr::game::AuthoredKey49N;
struct Mapping { const char* section; const char* key; Key id; };
constexpr Mapping mapped[] = {
    {"Physical Properties", "Weight", Key::Weight},
    {"Physical Properties", "RollInertiaScale", Key::RollInertiaScale},
    {"Physical Properties", "PitchInertiaScale", Key::PitchInertiaScale},
    {"Physical Properties", "YawInertiaScale", Key::YawInertiaScale},
    {"Physical Properties", "Gravity", Key::Gravity},
    {"Physical Properties", "AirborneGravity", Key::AirborneGravity},
    {"Engine", "MaxThrustA", Key::MaxThrustA},
    {"Engine", "MaxThrustB", Key::MaxThrustB},
    {"Engine", "MaxThrustC", Key::MaxThrustC},
    {"Engine", "MinMaxThrust", Key::MinMaxThrust},
    {"Engine", "MaxMaxThrust", Key::MaxMaxThrust},
    {"Engine", "TheoreticalTopSpeedA", Key::TheoreticalTopSpeedA},
    {"Engine", "TheoreticalTopSpeedB", Key::TheoreticalTopSpeedB},
    {"Engine", "TheoreticalTopSpeedC", Key::TheoreticalTopSpeedC},
    {"Engine", "MinTheoreticalTopSpeed", Key::MinTheoreticalTopSpeed},
    {"Engine", "MaxTheoreticalTopSpeed", Key::MaxTheoreticalTopSpeed},
    {"Engine", "BoostThrust", Key::BoostThrust},
    {"Engine", "RepairThrust", Key::RepairThrust},
    {"Engine", "RampUpFilter", Key::RampUpFilter},
    {"Engine", "RampDownFilter", Key::RampDownFilter},
    {"Engine", "RampDownSpeed", Key::RampDownSpeed},
    {"Engine", "ActualTopSpeed", Key::ActualTopSpeed},
    {"Engine", "BoostSpeed", Key::BoostSpeed},
    {"Engine", "RepairSpeed", Key::RepairSpeed},
    {"Engine Aerodynamics", "DefaultAngleOfAttack", Key::DefaultAngleOfAttack},
    {"Engine Aerodynamics", "Lift", Key::Lift},
    {"Engine Aerodynamics", "BasicDrag", Key::BasicDrag},
    {"Engine Aerodynamics", "PitchMoment", Key::PitchMoment},
    {"Engine Aerodynamics", "PitchRatePitchMoment", Key::PitchRatePitchMoment},
    {"Engine Aerodynamics", "SideSlipSideForce", Key::SideSlipSideForce},
    {"Engine Aerodynamics", "RollRateRollMoment", Key::RollRateRollMoment},
    {"Engine Aerodynamics", "SideSlipYawMoment", Key::SideSlipYawMoment},
    {"Engine Aerodynamics", "YawRateYawMoment", Key::YawRateYawMoment},
    {"Engine Aerodynamics", "SpoilerDrag", Key::SpoilerDrag},
    {"Engine Aerodynamics", "SpoilerBrakeDrag", Key::SpoilerBrakeDrag},
    {"Engine Aerodynamics", "SpoilerYawMomentScale", Key::SpoilerYawMomentScale},
    {"Engine Aerodynamics", "SpoilerSideForce", Key::SpoilerSideForce},
    {"Engine Aerodynamics", "PowerslideSideForce", Key::PowerslideSideForce},
    {"Engine Aerodynamics", "PowerslideSpoilerYawMoment", Key::PowerslideSpoilerYawMoment},
    {"Engine Aerodynamics", "PowerslideThrust", Key::PowerslideThrust},
    {"Engine Aerodynamics", "InvertedSideSlipSideForce", Key::InvertedSideSlipSideForce},
    {"Engine Aerodynamics", "InvertedSpoilerSideForce", Key::InvertedSpoilerSideForce},
    {"Engine Aerodynamics", "InvertedSideSlipYawMoment", Key::InvertedSideSlipYawMoment},
    {"Engine Aerodynamics", "SpoilerYawMomentA", Key::SpoilerYawMomentA},
    {"Engine Aerodynamics", "SpoilerYawMomentB", Key::SpoilerYawMomentB},
    {"Engine Aerodynamics", "SpoilerYawMomentC", Key::SpoilerYawMomentC},
    {"Engine Aerodynamics", "MinSpoilerYawMoment", Key::MinSpoilerYawMoment},
    {"Engine Aerodynamics", "MaxSpoilerYawMoment", Key::MaxSpoilerYawMoment},
    {"Engine Aerodynamics", "SpoilerYawMomentOffset", Key::SpoilerYawMomentOffset},
    {"BaseAbilities", "MaxThrust", Key::BaseAbilityMaxThrust},
    {"MaxAbilities", "MaxThrust", Key::MaxAbilityMaxThrust},
    {"BaseAbilities", "TheoreticalTopSpeed", Key::BaseAbilityTheoreticalTopSpeed},
    {"MaxAbilities", "TheoreticalTopSpeed", Key::MaxAbilityTheoreticalTopSpeed},
    {"BaseAbilities", "SpoilerYawMoment", Key::BaseAbilitySpoilerYawMoment},
    {"MaxAbilities", "SpoilerYawMoment", Key::MaxAbilitySpoilerYawMoment},
    {"Joystick", "SteerSpeed1", Key::SteerSpeed1},
    {"Joystick", "SteerScale1", Key::SteerScale1},
    {"Joystick", "SteerSpeed2", Key::SteerSpeed2},
    {"Joystick", "SteerScale2", Key::SteerScale2},
    {"Joystick", "XAxisRateLimit", Key::XAxisRateLimit},
    {"Joystick", "YAxisRateLimit", Key::YAxisRateLimit},
};
static_assert(sizeof(mapped)/sizeof(mapped[0])==rr::game::authoredKeyCount49N,
              "Every represented key has one mapping");
struct Slice { const char* p; std::size_t n, offset; };
char lower(char c) noexcept { return c>='A' && c<='Z' ? static_cast<char>(c+('a'-'A')) : c; }
bool space(char c) noexcept { return c==' ' || c=='\t' || c=='\r'; }
Slice trim(Slice s) noexcept {
    while (s.n && space(*s.p)) { ++s.p; --s.n; ++s.offset; }
    while (s.n && space(s.p[s.n-1])) --s.n;
    return s;
}
bool equal(Slice a,Slice b) noexcept {
    if (a.n!=b.n) return false;
    for (std::size_t i=0;i<a.n;++i) if (lower(a.p[i])!=lower(b.p[i])) return false;
    return true;
}
bool equal(Slice a,const char* b) noexcept { return equal(a,{b,std::strlen(b),0}); }
bool fail(PodPhysicsDiagnostic49N& d,PodPhysicsError49N c,std::size_t at) noexcept {
    d={c,at}; return false;
}
bool vector(Slice value,rr::game::Vector3_48C& out) noexcept {
    float f[3]{}; std::size_t begin=0;
    for (unsigned k=0;k<3;++k) {
        std::size_t end=begin;
        while (end<value.n && value.p[end]!=',') ++end;
        if ((k<2 && end==value.n) || (k==2 && end!=value.n)) return false;
        const auto token=trim({value.p+begin,end-begin,value.offset+begin});
        if (!parsePodPhysicsNumber49N(token.p,token.n,f[k])) return false;
        begin=end+1;
    }
    out={f[0],f[1],f[2]}; return true;
}
}
bool readPodPhysics49N(const char* text,std::size_t bytes,
    rr::game::AuthoredParameters49N& output,PodPhysicsDiagnostic49N& diagnostic,
    PodPhysicsProvenance49N* provenance) noexcept {
    if (!text || !bytes) return fail(diagnostic,PodPhysicsError49N::invalid_input,0);
    if (bytes>maxPodPhysicsBytes49N) return fail(diagnostic,PodPhysicsError49N::text_limit,0);
    for (std::size_t i=0;i<bytes;++i) {
        const auto c=static_cast<unsigned char>(text[i]);
        if ((c<32 && c!=9 && c!=10 && c!=13) || c>126)
            return fail(diagnostic,PodPhysicsError49N::invalid_character,i);
    }
    struct Range { std::uint16_t offset, bytes; };
    struct Seen { Range key; std::uint16_t section; };
    Range sections[maxPodPhysicsSections49N]{};
    Seen keys[maxPodPhysicsKeys49N]{};
    std::size_t sectionCount=0,keyCount=0,at=0;
    Slice section{nullptr,0,0};
    rr::game::AuthoredParameters49N result{};
    PodPhysicsProvenance49N audit{};
    while (at<bytes) {
        const std::size_t start=at;
        while (at<bytes && text[at]!='\n') ++at;
        const std::size_t length=at-start;
        if (at<bytes) ++at;
        if (length>maxPodPhysicsLine49N) return fail(diagnostic,PodPhysicsError49N::line_limit,start);
        Slice line=trim({text+start,length,start});
        if (!line.n || line.p[0]==';') continue;
        if (line.p[0]=='[') {
            if (line.n<3 || line.p[line.n-1]!=']') return fail(diagnostic,PodPhysicsError49N::syntax,line.offset);
            section={line.p+1,line.n-2,line.offset+1};
            for (std::size_t i=0;i<section.n;++i)
                if (section.p[i]=='[' || section.p[i]==']' || section.p[i]=='=' || section.p[i]=='"' || section.p[i]==';' || section.p[i]=='\r' || section.p[i]=='\t')
                    return fail(diagnostic,PodPhysicsError49N::syntax,section.offset+i);
            for (std::size_t i=0;i<sectionCount;++i)
                if (equal(section,{text+sections[i].offset,sections[i].bytes,sections[i].offset}))
                    return fail(diagnostic,PodPhysicsError49N::duplicate_section,section.offset);
            if (sectionCount==maxPodPhysicsSections49N) return fail(diagnostic,PodPhysicsError49N::section_limit,section.offset);
            sections[sectionCount++]={static_cast<std::uint16_t>(section.offset),static_cast<std::uint16_t>(section.n)};
            if (equal(section,"Tuning")) result.tuning.sectionPresent=true;
            continue;
        }
        if (!sectionCount) return fail(diagnostic,PodPhysicsError49N::syntax,line.offset);
        std::size_t split=0; while (split<line.n && line.p[split]!='=') ++split;
        if (split==line.n) return fail(diagnostic,PodPhysicsError49N::syntax,line.offset);
        const Slice key=trim({line.p,split,line.offset});
        const Slice value=trim({line.p+split+1,line.n-split-1,line.offset+split+1});
        if (!key.n || !value.n) return fail(diagnostic,PodPhysicsError49N::syntax,line.offset);
        for (std::size_t i=0;i<key.n;++i)
            if (!((lower(key.p[i])>='a' && lower(key.p[i])<='z') || (key.p[i]>='0' && key.p[i]<='9')))
                return fail(diagnostic,PodPhysicsError49N::syntax,key.offset+i);
        for (std::size_t i=0;i<keyCount;++i)
            if (keys[i].section==sectionCount-1 && equal(key,{text+keys[i].key.offset,keys[i].key.bytes,keys[i].key.offset}))
                return fail(diagnostic,PodPhysicsError49N::duplicate_key,key.offset);
        if (keyCount==maxPodPhysicsKeys49N) return fail(diagnostic,PodPhysicsError49N::key_limit,key.offset);
        keys[keyCount++]={{static_cast<std::uint16_t>(key.offset),static_cast<std::uint16_t>(key.n)},static_cast<std::uint16_t>(sectionCount-1)};
        const PodPhysicsToken49N token{static_cast<std::uint32_t>(key.offset),static_cast<std::uint32_t>(value.offset),static_cast<std::uint32_t>(value.n)};
        bool consumed=false;
        for (const auto& m:mapped) if (equal(section,m.section) && equal(key,m.key)) {
            auto& dest=result.values[static_cast<std::size_t>(m.id)];
            if (!parsePodPhysicsNumber49N(value.p,value.n,dest.value)) return fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
            dest.present=true; audit.scalars[static_cast<std::size_t>(m.id)]=token; consumed=true; break;
        }
        if (consumed) continue;
        if (equal(section,"Engine") && (equal(key,"LeftEnginePosition") || equal(key,"RightEnginePosition"))) {
            const bool left=equal(key,"LeftEnginePosition");
            auto& dest=left?result.leftEnginePosition:result.rightEnginePosition;
            if (!vector(value,dest.value)) return fail(diagnostic,PodPhysicsError49N::vector_syntax,value.offset);
            dest.present=true; (left?audit.leftEnginePosition:audit.rightEnginePosition)=token;
        } else if (equal(section,"Tuning")) {
            const char* names[]={"UseValues","UseAValues","UseBValues","UseCValues"};
            rr::game::AuthoredInteger49N* flags[]={&result.tuning.useValues,&result.tuning.useAValues,&result.tuning.useBValues,&result.tuning.useCValues};
            for (unsigned i=0;i<4;++i) if (equal(key,names[i])) {
                // Narrow native integer admission; original158920 accepts a
                // broader boolean/base-zero integer grammar. Actual flags0/1.
                if (value.n!=1 || (value.p[0]!='0' && value.p[0]!='1')) return fail(diagnostic,PodPhysicsError49N::integer_syntax,value.offset);
                *flags[i]={value.p[0]-'0',true}; audit.tuning[i]=token;
            }
            if (equal(key,"UpgradePercent")) {
                if (!parsePodPhysicsNumber49N(value.p,value.n,result.tuning.upgradePercent.value)) return fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
                result.tuning.upgradePercent.present=true; audit.tuning[4]=token;
            }
        }
    }
    for (const auto& v:result.values) if (!v.present) return fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
    if (!result.leftEnginePosition.present || !result.rightEnginePosition.present) return fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
    output=result; if (provenance) *provenance=audit;
    diagnostic={PodPhysicsError49N::none,0}; return true;
}
}
