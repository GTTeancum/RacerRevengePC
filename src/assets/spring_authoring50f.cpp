#include "spring_authoring50f.h"
#include <cstring>
#include <cstdint>
#include <limits>
#include <cmath>
namespace rr::assets { namespace {
struct Mapping { const char* section; const char* key; };
constexpr Mapping mappings[]={
 {"Collisions","MaxCompression"},
 {"Collisions","Spring"},
 {"Collisions","Damping"},
 {"Collisions","ImpulseSpring"},
 {"Collisions","ImpulseDamping"},
 {"Collisions","Friction"},
 {"Collisions","WallRestitution"},
 {"Collisions","PodRestitution"},
 {"Collisions","MomentScale"},
 {"Collisions","AngularImpulseScale"},
 {"Collisions","FrictionTorqueScale"}
};
static_assert(sizeof(mappings)/sizeof(mappings[0])==springKeyCount50F);
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
}
bool readSpringCollision50F(const char* text,std::size_t bytes,SpringCollisionAuthoring50F& output,PodPhysicsDiagnostic49N& diagnostic,SpringCollisionProvenance50F* provenance) noexcept {
    struct Span { std::uintptr_t a,z; };
    const auto range=[](const void*p,std::size_t n,Span& s) noexcept {s={0,0};if(!n)return true;if(!p)return false;const auto a=reinterpret_cast<std::uintptr_t>(p);if(n>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+n};return true;};
    Span spans[4]{};
    if(!range(text,bytes,spans[0])||!range(&output,sizeof output,spans[1])||!range(&diagnostic,sizeof diagnostic,spans[2])||!range(provenance,provenance?sizeof(*provenance):0,spans[3]))return false;
    for(unsigned i=0;i<4;++i)for(unsigned j=0;j<i;++j)if(spans[i].a<spans[j].z&&spans[j].a<spans[i].z)return false;
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
    SpringCollisionAuthoring50F result{};
    bool present[springKeyCount50F]{};
    SpringCollisionProvenance50F audit{};
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
        for (std::size_t i=0;i<springKeyCount50F;++i) if (equal(section,mappings[i].section) && equal(key,mappings[i].key)) {
            if (!parseImpactNumber49Y(value.p,value.n,result.values[i])) return fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
            present[i]=true;
            audit.fields[i]={static_cast<std::uint32_t>(section.offset),static_cast<std::uint32_t>(section.n),static_cast<std::uint32_t>(key.offset),static_cast<std::uint32_t>(key.n),static_cast<std::uint32_t>(value.offset),static_cast<std::uint32_t>(value.n)};
            break;
        }
    }
    for(std::size_t i=0;i<springKeyCount50F;++i)if(!present[i])return fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
    output=result; if(provenance) *provenance=audit;
    diagnostic={PodPhysicsError49N::none,0};return true;
}

bool applySpringCollision50F(const SpringCollisionAuthoring50F& a,float mass,
    rr::game::SpringGeneratedParameters48K& parameters,rr::game::SpringResponseParameters48L& response) noexcept {
    const auto overlap=[](const void*x,std::size_t nx,const void*y,std::size_t ny) noexcept {
        const auto b=reinterpret_cast<std::uintptr_t>(x),c=reinterpret_cast<std::uintptr_t>(y);
        return b<c?c-b<nx:b-c<ny;
    };
    if(overlap(&parameters,sizeof parameters,&response,sizeof response)||
       overlap(&a,sizeof a,&parameters,sizeof parameters)||overlap(&a,sizeof a,&response,sizeof response))return false;
    if(!std::isfinite(mass))return false;
    for(float x:a.values)if(!std::isfinite(x))return false;
    rr::game::SpringGeneratedParameters48K p;rr::game::SpringResponseParameters48L r;
    std::memcpy(&p,&parameters,sizeof p);std::memcpy(&r,&response,sizeof r);
    const auto mul=[](float x,float y) noexcept {volatile float z=x*y;return z;};
    const std::uint32_t scaleBits=0x4200cccdu;float scale;std::memcpy(&scale,&scaleBits,4);
    const auto pair=[&](float spring,float damping,float& stiffness,float& damp) noexcept {
        const float first=mul(spring,mass);const float product=mul(scale,first);
        if(!std::isfinite(first)||!std::isfinite(product))return false;
        stiffness=std::fabs(product);damp=0.f;
        if(mass>0.f&&stiffness>0.f){
            const float square=mul(mass,stiffness);if(!std::isfinite(square))return false;
            volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));
            const float twice=mul(2.f,damping);damp=mul(twice,root);
            if(!std::isfinite(twice)||!std::isfinite(damp))return false;
        }
        return true;
    };
    r.maxPenetration90=a.values[0];
    if(!pair(a.values[1],a.values[2],r.stiffness9C,r.dampingA0)||
       !pair(a.values[3],a.values[4],r.impactStiffnessA4,r.impactDampingA8))return false;
    p.impact.restitutionB8=a.values[6];p.impact.restitutionBC=a.values[6];p.impact.restitutionC0=a.values[7];
    p.impact.forceTorqueScaleB4=a.values[8];p.impact.angularImpulseScaleB0=a.values[9];
    p.frictionTorqueScaleC4=a.values[10];p.frictionByMaterial[0]=a.values[5];
    std::memcpy(&parameters,&p,sizeof p);std::memcpy(&response,&r,sizeof r);return true;
}
}
