#include "vehicle_visual_authoring49r.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::assets {
namespace {
using K=VisualAuthoredKey49R;
using E=VisualAuthoringError49R;
struct Mapping { const char* section; const char* key; K id; };
constexpr Mapping mapped[] = {
    {"Pod Animation", "ImpactForce", K::ImpactForce},
    {"Pod Animation", "GroundImpactForce", K::GroundImpactForce},
    {"Pod Animation", "SeparationForce", K::SeparationForce},
    {"Pod Animation", "ThrustForce", K::ThrustForce},
    {"Pod Animation", "LiftForce", K::LiftForce},
    {"Pod Animation", "DragForce", K::DragForce},
    {"Pod Animation", "EngineSpring", K::EngineSpring},
    {"Pod Animation", "EngineDamping", K::EngineDamping},
    {"Pod Animation", "BinderSpring", K::BinderSpring},
    {"Pod Animation", "YawTorque", K::YawTorque},
    {"Pod Animation", "PowerslideYawTorque", K::PowerslideYawTorque},
    {"Pod Animation", "PowerslidePitchTorque", K::PowerslidePitchTorque},
    {"Pod Animation", "PitchTorque", K::PitchTorque},
    {"Pod Animation", "RollTorque", K::RollTorque},
    {"Pod Animation", "EngineTorque", K::EngineTorque},
    {"Pod Animation", "EngineTorqueDamping", K::EngineTorqueDamping},
    {"Pod Animation", "BinderTorque", K::BinderTorque},
    {"Pod Animation", "MinTurbulenceTimeScale", K::MinTurbulenceTimeScale},
    {"Pod Animation", "MaxTurbulenceTimeScale", K::MaxTurbulenceTimeScale},
    {"Pod Animation", "MinTurbulenceScale", K::MinTurbulenceScale},
    {"Pod Animation", "MaxTurbulenceScale", K::MaxTurbulenceScale},
    {"Pod Animation", "MaxTurbulenceSpeed", K::MaxTurbulenceSpeed},
    {"Pod Animation", "DamageTurbulenceScale", K::DamageTurbulenceScale},
    {"Pod Animation", "DamageOrientationTurbulenceScale", K::DamageOrientationTurbulenceScale},
    {"Pod Animation", "OrientationScale", K::OrientationScale},
    {"Pod Animation", "MaxInputScaleSpeed", K::MaxInputScaleSpeed},
    {"Repulsor", "MaxLiftHeight", K::MaxLiftHeight},
};
static_assert(sizeof(mapped)/sizeof(mapped[0])==visualAuthoredKeyCount49R);
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
bool fail(VisualAuthoringDiagnostic49R& d,PodPhysicsError49N e,std::size_t at) noexcept {
    d={e==PodPhysicsError49N::missing_key?E::missing_key:E::text,{e,at},K::Count}; return false;
}
}
bool readVehicleVisualAuthoring49R(const char* text,std::size_t bytes,
    VisualAuthoredValues49R& output,VisualAuthoringDiagnostic49R& diagnostic,
    VisualAuthoredProvenance49R* provenance) noexcept {
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
    VisualAuthoredValues49R result{};
    VisualAuthoredProvenance49R audit{};
    bool present[visualAuthoredKeyCount49R]{};
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
    for (std::size_t i=0;i<visualAuthoredKeyCount49R;++i) if (!present[i]) {
        fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
        diagnostic.key=static_cast<K>(i); return false;
    }
    output=result; if (provenance) *provenance=audit;
    diagnostic={E::none,{PodPhysicsError49N::none,0},K::Count}; return true;
}
namespace {
bool nullcg(const char* s) noexcept {
    constexpr char target[]="NULLCG";
    for(unsigned i=0;i<6;++i)if(lower(s[i])!=lower(target[i]))return false;
    return s[6]==0;
}
bool offset(const PsgHierarchyView49O& view,rr::game::VisualVector49R& out,E& e) noexcept {
    using namespace rr::game;
    PsgNode49O node{};
    if(!view.nodeCount||view.nodeCount>maxModelNodes49O||!readPsgNode49O(view,0,node)){e=E::hierarchy;return false;}
    if(node.parent!=-1||!nullcg(node.name)){e=E::nullcg;return false;}
    for(std::uint32_t i=1;i<view.nodeCount;++i){PsgNode49O child{};
        if(!readPsgNode49O(view,i,child)||child.parent<0){e=E::hierarchy;return false;}}
    // The source hierarchy root is a CHILD of the separate graphics-model root.
    // Force its exact world product, including every multiply/add of zero.
    SceneNode49P nodes[2]{};
    for(auto& n:nodes){n.parent180=n.firstChild184=n.nextSibling188=SceneNodeIndex49P::none; n.sceneFlags190=0x80;}
    nodes[0].localC0.v[0]=nodes[0].localC0.v[5]=nodes[0].localC0.v[10]=nodes[0].localC0.v[15]=1;
    nodes[0].firstChild184=static_cast<SceneNodeIndex49P>(1);
    nodes[1].parent180=static_cast<SceneNodeIndex49P>(0);
    std::memcpy(nodes[1].localC0.v,node.local,sizeof(node.local));
    if(!refreshSceneWorld49P({nodes,2},static_cast<SceneNodeIndex49P>(1))){e=E::arithmetic;return false;}
    out={{nodes[1].world100.v[12],nodes[1].world100.v[13],nodes[1].world100.v[14],1}};
    out.v[2]=0; return true;
}
float multiply(float a,float b) noexcept { volatile float r=a*b;return r; }
}
bool bindFreshVehicleVisualParameters49R(const VisualAuthoredValues49R& values,
    const PsgHierarchyView49O& right,const PsgHierarchyView49O& left,
    rr::game::VehicleVisualParameters49R& out,VisualAuthoringDiagnostic49R& d) noexcept {
    d={E::none,{PodPhysicsError49N::none,0},K::Count};
    if(std::fegetround()!=FE_TONEAREST){d.error=E::rounding_mode;return false;}
    for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i)if(!std::isfinite(values.values[i])){
        d.error=E::nonfinite;d.key=static_cast<K>(i);return false;}
    rr::game::VehicleVisualParameters49R p{};
    p.impact9F0=values.values[static_cast<std::size_t>(K::ImpactForce)];
    p.groundImpact9F4=values.values[static_cast<std::size_t>(K::GroundImpactForce)];
    p.separation9F8=values.values[static_cast<std::size_t>(K::SeparationForce)];
    p.thrust9FC=values.values[static_cast<std::size_t>(K::ThrustForce)];
    p.liftA00=values.values[static_cast<std::size_t>(K::LiftForce)];
    p.dragA04=values.values[static_cast<std::size_t>(K::DragForce)];
    p.springA08=values.values[static_cast<std::size_t>(K::EngineSpring)];
    p.dampingA0C=values.values[static_cast<std::size_t>(K::EngineDamping)];
    p.binderSpringA10=values.values[static_cast<std::size_t>(K::BinderSpring)];
    p.yawA14=values.values[static_cast<std::size_t>(K::YawTorque)];
    p.slideYawA18=values.values[static_cast<std::size_t>(K::PowerslideYawTorque)];
    p.slidePitchA1C=values.values[static_cast<std::size_t>(K::PowerslidePitchTorque)];
    p.pitchA20=values.values[static_cast<std::size_t>(K::PitchTorque)];
    p.rollA24=values.values[static_cast<std::size_t>(K::RollTorque)];
    p.angleReturnA28=values.values[static_cast<std::size_t>(K::EngineTorque)];
    p.angularDampingA2C=values.values[static_cast<std::size_t>(K::EngineTorqueDamping)];
    p.binderTorqueA30=values.values[static_cast<std::size_t>(K::BinderTorque)];
    p.minTimeA34=values.values[static_cast<std::size_t>(K::MinTurbulenceTimeScale)];
    p.maxTimeA38=values.values[static_cast<std::size_t>(K::MaxTurbulenceTimeScale)];
    p.minAmplitudeA44=values.values[static_cast<std::size_t>(K::MinTurbulenceScale)];
    p.maxAmplitudeA48=values.values[static_cast<std::size_t>(K::MaxTurbulenceScale)];
    p.effectiveSpeedSquaredA54=values.values[static_cast<std::size_t>(K::MaxTurbulenceSpeed)];
    p.damageTranslationA58=values.values[static_cast<std::size_t>(K::DamageTurbulenceScale)];
    p.damageOrientationA5C=values.values[static_cast<std::size_t>(K::DamageOrientationTurbulenceScale)];
    p.orientationScaleA64=values.values[static_cast<std::size_t>(K::OrientationScale)];
    p.effectiveInputSpeedA68=values.values[static_cast<std::size_t>(K::MaxInputScaleSpeed)];
    p.verticalLimit7EC=values.values[static_cast<std::size_t>(K::MaxLiftHeight)];
    const std::uint32_t kWord=0x3fbbbb84u;float k;std::memcpy(&k,&kWord,4);
    const float speed=p.effectiveSpeedSquaredA54;
    p.effectiveInputSpeedA68=multiply(p.effectiveInputSpeedA68,k); //2158E0
    const float first=multiply(k,speed); //2158EC
    const float second=multiply(first,speed); //2158F0
    p.effectiveSpeedSquaredA54=multiply(k,second); //2158F4
    if(!std::isfinite(p.effectiveInputSpeedA68)||!std::isfinite(first)||
       !std::isfinite(second)||!std::isfinite(p.effectiveSpeedSquaredA54)){d.error=E::arithmetic;return false;}
    if(!offset(right,p.rightBase2C0,d.error)||!offset(left,p.leftBase2B0,d.error))return false;
    out=p;return true;
}
void initializeVehicleVisualConstructorRecords49R(rr::game::VehicleVisualState49R& state) noexcept {
    using namespace rr::game;
    const VisualVector49R zero={{0,0,0,1}},up={{0,1,0,1}};
    const VisualResponse49R response={zero,zero,zero,{{1,0,0,0}}};
    state.upC20=up;state.publishedUpAF0=up;state.right=response;state.left=response;
    state.phase8B0=0;state.phase900=0;state.phase950=0;state.phase9A0=0;
}
void initializeVehicleVisualSharedColdRecords49R(rr::game::VisualShared49R& shared) noexcept {
    for(auto& g:shared.globals)g={0,0};
    shared.orientationSentinel4433D8={0,0}; shared.zero434630={{0,0,0,1}};
}
}
