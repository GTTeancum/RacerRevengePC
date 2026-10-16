#include "impact_authoring49y.h"
#include <cstring>
namespace rr::assets {
bool parseImpactNumber49Y(const char* text,std::size_t bytes,float& output,std::uint64_t* audit) noexcept {
    if (!text || !bytes || bytes>11) return false;
    const std::size_t sign=(text[0]=='+' || text[0]=='-')?1:0;
    if (sign<bytes && text[sign]=='.') {
        char normalized[12];
        if (sign) normalized[0]=text[0];
        normalized[sign]='0';
        std::memcpy(normalized+sign+1,text+sign,bytes-sign);
        return parsePodPhysicsNumber49N(normalized,bytes+1,output,audit);
    }
    return parsePodPhysicsNumber49N(text,bytes,output,audit);
}
namespace {
struct Mapping { const char* section; const char* key; };
constexpr Mapping env[]={{"EnvDamage","hitXL"},{"EnvDamage","hitL"},{"EnvDamage","hitM"},{"EnvDamage","hitS"},{"EnvDamage","hitXS"},{"EnvDamage","pointsXL"},{"EnvDamage","pointsL"},{"EnvDamage","pointsM"},{"EnvDamage","pointsS"},{"EnvDamage","pointsXS"}};
constexpr Mapping aiEnv[]={{"EnvDamage","hitXL"},{"EnvDamage","hitL"},{"EnvDamage","hitM"},{"EnvDamage","hitS"},{"EnvDamage","hitXS"},{"EnvDamage","pointsXL"},{"EnvDamage","pointsL"},{"EnvDamage","pointsM"},{"EnvDamage","pointsS"},{"EnvDamage","pointsXS"},{"PodDamage","AIEnvPointsMultiplier"}};
constexpr Mapping dynamicImpact[]={{"PodDamage","hitXL"},{"PodDamage","hitL"},{"PodDamage","hitM"},{"PodDamage","hitS"},{"PodDamage","hitXS"},{"PodDamage","pointsXL"},{"PodDamage","pointsL"},{"PodDamage","pointsM"},{"PodDamage","pointsS"},{"PodDamage","pointsXS"},{"PodDamage","MinGrindingSpeed"},{"PodDamage","AIPointsMultiplier"},{"PodDamage","AIMultiplierDecrement"},{"PodDamage","PodGrindingPoints"}};
constexpr Mapping finalImpact[]={{"PodDamage","FinalDamageThreshold"},{"PodDamage","FinalDamageIncrement"}};
constexpr Mapping defense[]={{"BaseAbilities","Defense"},{"MaxAbilities","Defense"},{"DamageRepair","MinDefense"},{"DamageRepair","MaxDefense"}};
struct Parsed { float values[14]{}; bool present[14]{}; ImpactAuthoringToken49Y tokens[14]{}; };
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
bool read(const char* text,std::size_t bytes,const Mapping* mappings,std::size_t count,bool required,Parsed& output,PodPhysicsDiagnostic49N& diagnostic) noexcept {
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
    Parsed result{};
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
        for (std::size_t i=0;i<count;++i) if (equal(section,mappings[i].section) && equal(key,mappings[i].key)) {
            if (!parseImpactNumber49Y(value.p,value.n,result.values[i])) return fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
            result.present[i]=true;
            result.tokens[i]={static_cast<std::uint32_t>(section.offset),static_cast<std::uint32_t>(section.n),static_cast<std::uint32_t>(key.offset),static_cast<std::uint32_t>(key.n),static_cast<std::uint32_t>(value.offset),static_cast<std::uint32_t>(value.n)};
            break;
        }
    }
    if (required) for(std::size_t i=0;i<count;++i) if(!result.present[i]) return fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
    output=result; diagnostic={PodPhysicsError49N::none,0}; return true;
}
}
bool readEnvironmentImpact49Y(const char* text,std::size_t bytes,rr::game::StaticImpactTuning49Y& output,PodPhysicsDiagnostic49N& diagnostic,EnvironmentImpactProvenance49Y* provenance) noexcept {
    Parsed p{}; if(!read(text,bytes,env,10,true,p,diagnostic)) return false;
    rr::game::StaticImpactTuning49Y result{}; EnvironmentImpactProvenance49Y audit{};
    for(unsigned i=0;i<5;++i) {result.thresholds00[i]=p.values[i];result.severities14[i]=p.values[5+i];}
    for(unsigned i=0;i<10;++i) audit.fields[i]=p.tokens[i];
    output=result; if(provenance) *provenance=audit; return true;
}
bool readAIEnvironmentImpact49Y(const char* text,std::size_t bytes,rr::game::StaticImpactTuning49Y& output,PodPhysicsDiagnostic49N& diagnostic,AIEnvironmentImpactProvenance49Y* provenance) noexcept {
    Parsed p{}; if(!read(text,bytes,aiEnv,11,true,p,diagnostic)) return false;
    rr::game::StaticImpactTuning49Y result{}; AIEnvironmentImpactProvenance49Y audit{};
    for(unsigned i=0;i<5;++i) {result.thresholds00[i]=p.values[i];result.severities14[i]=p.values[5+i];}
    result.aiEnvironmentMultiplier60=p.values[10];result.aiEnvironmentMultiplierBound=true;
    for(unsigned i=0;i<11;++i) audit.fields[i]=p.tokens[i];
    output=result; if(provenance) *provenance=audit; return true;
}
bool readFinalImpactAuthoring49Y(const char* text,std::size_t bytes,FinalImpactAuthoring49Y& output,PodPhysicsDiagnostic49N& diagnostic,FinalImpactProvenance49Y* provenance) noexcept {
    Parsed p{}; if(!read(text,bytes,finalImpact,2,true,p,diagnostic)) return false;
    const FinalImpactAuthoring49Y result{p.values[0],p.values[1]};
    const FinalImpactProvenance49Y audit{{p.tokens[0],p.tokens[1]}};
    output=result; if(provenance) *provenance=audit; return true;
}
bool readDynamicImpactAuthoring50IW(const char* text,std::size_t bytes,rr::game::DynamicImpactTuning50IW& output,PodPhysicsDiagnostic49N& diagnostic,DynamicImpactProvenance50IW* provenance) noexcept {
    Parsed p{};if(!read(text,bytes,dynamicImpact,14,false,p,diagnostic))return false;
    // Original1AAB30 initializes54=100 (1AAC6C) and5C=0x3D4CCCCD
    // (1AAC7C). Reader158A70 skips absent keys; RA.TUN omits these two.
    // Empty provenance tokens identify constructor values, not authored text.
    for(unsigned i=0;i<14;++i)if(i!=10&&i!=12&&i!=13&&!p.present[i])
        return fail(diagnostic,PodPhysicsError49N::missing_key,bytes);
    //1AAC64 initializes C0+50 to1;1AA8A0 skips an absent PodGrindingPoints.
    if(!p.present[13])p.values[13]=1.f;
    if(!p.present[10])p.values[10]=100.f;
    if(!p.present[12]){const std::uint32_t bits=0x3d4ccccdu;std::memcpy(&p.values[12],&bits,4);}
    rr::game::DynamicImpactTuning50IW result{};DynamicImpactProvenance50IW audit{};
    for(unsigned i=0;i<5;++i){result.thresholds28[i]=p.values[i];result.points3C[i]=p.values[5+i];}
    result.minGrindingSpeed54=p.values[10];result.aiPointsMultiplier58=p.values[11];result.aiMultiplierDecrement5C=p.values[12];result.podGrindingPoints50=p.values[13];result.bound=true;
    for(unsigned i=0;i<14;++i)audit.fields[i]=p.tokens[i];output=result;if(provenance)*provenance=audit;return true;
}
bool readDefenseAuthoring49Y(const char* text,std::size_t bytes,DefenseAuthoring49Y& output,PodPhysicsDiagnostic49N& diagnostic,DefenseProvenance49Y* provenance) noexcept {
    Parsed p{}; if(!read(text,bytes,defense,4,false,p,diagnostic)) return false;
    const DefenseAuthoring49Y result{{p.values[0],p.present[0]},{p.values[1],p.present[1]},{p.values[2],p.present[2]},{p.values[3],p.present[3]}};
    DefenseProvenance49Y audit{}; for(unsigned i=0;i<4;++i) audit.fields[i]=p.tokens[i];
    output=result; if(provenance) *provenance=audit; return true;
}
}
