#include "chariot_root_authoring49u.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::assets {
namespace {
using K=ChariotRootAuthoredKey49U;
using E=ChariotRootAuthoringError49U;
struct Mapping { const char* section; const char* key; K id; };
constexpr Mapping mapped[] = {
    {"Pod Animation", "CableForce", K::CableForce},
    {"Pod Animation", "CableRadiusForce", K::CableRadiusForce},
    {"Pod Animation", "ChariotImpactForce", K::ChariotImpactForce},
    {"Pod Animation", "ChariotHeadOnImpactForce", K::ChariotHeadOnImpactForce},
    {"Pod Animation", "ChariotRepulsorImpactForce", K::ChariotRepulsorImpactForce},
    {"Pod Animation", "ChariotLandingImpulseScale", K::ChariotLandingImpulseScale},
    {"Pod Animation", "ChariotMaxImpulse", K::ChariotMaxImpulse},
    {"Pod Animation", "ChariotMaxRepulsorImpulse", K::ChariotMaxRepulsorImpulse},
    {"Pod Animation", "ChariotVelocityRateForce", K::ChariotVelocityRateForce},
    {"Pod Animation", "ChariotVelocityForce", K::ChariotVelocityForce},
    {"Pod Animation", "ChariotPowerslideVelocityForce", K::ChariotPowerslideVelocityForce},
    {"Pod Animation", "ChariotVelocityDamping", K::ChariotVelocityDamping},
    {"Pod Animation", "ChariotAngularRateDamping", K::ChariotAngularRateDamping},
    {"Pod Animation", "ChariotMaxYawRate", K::ChariotMaxYawRate},
    {"Pod Animation", "ChariotMaxYawAngle", K::ChariotMaxYawAngle},
    {"Pod Animation", "ChariotMaxPowerslideYawAngle", K::ChariotMaxPowerslideYawAngle},
    {"Pod Animation", "MinChariotTurbulenceScale", K::MinChariotTurbulenceScale},
    {"Pod Animation", "MaxChariotTurbulenceScale", K::MaxChariotTurbulenceScale},
    {"Pod Animation", "MinChariotTurbulenceTimeScale", K::MinChariotTurbulenceTimeScale},
    {"Pod Animation", "MaxChariotTurbulenceTimeScale", K::MaxChariotTurbulenceTimeScale},
    {"Pod Animation", "ChariotOrientationScale", K::ChariotOrientationScale},
};
static_assert(sizeof(mapped)/sizeof(mapped[0])==chariotRootAuthoredKeyCount49U);
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
bool fail(ChariotRootAuthoringDiagnostic49U& d,PodPhysicsError49N e,std::size_t at) noexcept {
    d={e==PodPhysicsError49N::missing_key?E::missing_key:E::text,{e,at},K::Count}; return false;
}
}
bool readChariotRootAuthoring49U(const char* text,std::size_t bytes,
    ChariotRootAuthoredValues49U& output,ChariotRootAuthoringDiagnostic49U& diagnostic,
    ChariotRootAuthoredProvenance49U* provenance) noexcept {
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
    ChariotRootAuthoredValues49U result{};
    ChariotRootAuthoredProvenance49U audit{};
    bool present[chariotRootAuthoredKeyCount49U]{};
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
        for (const auto& m:mapped) if (equal(section,m.section) && equal(key,m.key)) {
            const auto id=static_cast<std::size_t>(m.id);
            if (!parsePodPhysicsNumber49N(value.p,value.n,result.values[id])) {
                fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
                diagnostic.key=m.id; return false;
            }
            present[id]=true; audit.tokens[id]=token; break;
        }
    }
    for (std::size_t i=0;i<chariotRootAuthoredKeyCount49U;++i) if (!present[i]) {
        fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
        diagnostic.key=static_cast<K>(i); return false;
    }
    output=result; if (provenance) *provenance=audit;
    diagnostic={E::none,{PodPhysicsError49N::none,0},K::Count}; return true;
}

namespace {
float wordFloat(std::uint32_t bits) noexcept { float f; std::memcpy(&f,&bits,4); return f; }
float multiply(float a,float b) noexcept { volatile float result=a*b; return result; }
}
bool bindFreshChariotRootParameters49U(const ChariotRootAuthoredValues49U& values,
    const rr::game::FreshPodGeometryResult49O& geometry,
    const rr::game::VehicleVisualParameters49R& visual,
    rr::game::ChariotRootParameters49U& output, ChariotRootAuthoringDiagnostic49U& d,
    float* convertedUnconsumedA90) noexcept {
    d={E::none,{PodPhysicsError49N::none,0},K::Count};
    if(std::fegetround()!=FE_TONEAREST){d.error=E::rounding_mode;return false;}
    for(std::size_t i=0;i<chariotRootAuthoredKeyCount49U;++i)if(!std::isfinite(values.values[i])){
        d.error=E::nonfinite;d.key=static_cast<K>(i);return false;}
    if(!std::isfinite(geometry.parameters.length854)||geometry.parameters.length854<0){d.error=E::geometry;return false;}
    if(!std::isfinite(visual.effectiveSpeedSquaredA54)){d.error=E::nonfinite;return false;}
    rr::game::ChariotRootParameters49U p{};
    p.cableForceA6C=values.values[static_cast<std::size_t>(K::CableForce)];
    p.radiusForceA70=values.values[static_cast<std::size_t>(K::CableRadiusForce)];
    p.collisionXYAA0=values.values[static_cast<std::size_t>(K::ChariotImpactForce)];
    p.collisionZAA4=values.values[static_cast<std::size_t>(K::ChariotHeadOnImpactForce)];
    p.groundGainAA8=values.values[static_cast<std::size_t>(K::ChariotRepulsorImpactForce)];
    p.groundTransitionGainAAC=values.values[static_cast<std::size_t>(K::ChariotLandingImpulseScale)];
    p.collisionBoundA98=values.values[static_cast<std::size_t>(K::ChariotMaxImpulse)];
    p.groundBoundA9C=values.values[static_cast<std::size_t>(K::ChariotMaxRepulsorImpulse)];
    p.returnGainA7C=values.values[static_cast<std::size_t>(K::ChariotVelocityRateForce)];
    p.restoringA74=values.values[static_cast<std::size_t>(K::ChariotVelocityForce)];
    p.slideRestoringA78=values.values[static_cast<std::size_t>(K::ChariotPowerslideVelocityForce)];
    p.linearDampingA80=values.values[static_cast<std::size_t>(K::ChariotVelocityDamping)];
    p.angularDampingA84=values.values[static_cast<std::size_t>(K::ChariotAngularRateDamping)];
    p.yawInputRangeA88=values.values[static_cast<std::size_t>(K::ChariotMaxYawRate)];
    p.yawAngleA8C=values.values[static_cast<std::size_t>(K::ChariotMaxYawAngle)];
    p.minAmplitudeA4C=values.values[static_cast<std::size_t>(K::MinChariotTurbulenceScale)];
    p.maxAmplitudeA50=values.values[static_cast<std::size_t>(K::MaxChariotTurbulenceScale)];
    p.minTimeA3C=values.values[static_cast<std::size_t>(K::MinChariotTurbulenceTimeScale)];
    p.maxTimeA40=values.values[static_cast<std::size_t>(K::MaxChariotTurbulenceTimeScale)];
    p.orientationScaleA60=values.values[static_cast<std::size_t>(K::ChariotOrientationScale)];
    const float degreeFactor=wordFloat(0x3c8efa34u);
    p.yawInputRangeA88=multiply(p.yawInputRangeA88,degreeFactor);
    p.yawAngleA8C=multiply(p.yawAngleA8C,degreeFactor);
    const float a90=multiply(values.values[static_cast<std::size_t>(K::ChariotMaxPowerslideYawAngle)],degreeFactor);
    if(!std::isfinite(p.yawInputRangeA88)||!std::isfinite(p.yawAngleA8C)||!std::isfinite(a90)){
        d.error=E::arithmetic;return false;}
    p.effectiveSpeedSquaredA54=visual.effectiveSpeedSquaredA54;
    p.orientationGainA94=wordFloat(0xc3960000u);
    p.restLength854=geometry.parameters.length854;
    output=p;if(convertedUnconsumedA90)*convertedUnconsumedA90=a90;return true;
}
void initializeChariotRootConstructorRecords49U(rr::game::ChariotRootState49U& state,
    float& phase864) noexcept {
    state.vehicle838=1;state.vehicle83C=0;state.vehicle840=1;state.previous85C=0;
    state.response.position={{wordFloat(0x80000000u),wordFloat(0x80000000u),-1,1}};
    state.response.linearVelocity={{0,0,0,1}};state.response.angularVelocity={{0,0,0,1}};
    state.response.orientation={{1,0,0,0}};phase864=0;
}
void initializeChariotRootSharedColdRecords49U(rr::game::ChariotRootShared49U& shared) noexcept {
    shared.speed434750={0,0};shared.yaw434758={0,0};shared.up434650={{0,1,0,1}};
}
} // namespace rr::assets
