#include "repair_authoring50a.h"
#include <cstring>
namespace rr::assets { namespace {
struct Mapping { const char* section; const char* key; };
constexpr Mapping mappings[]={
 {"BaseAbilities","BaseRepairAmount"},{"MaxAbilities","BaseRepairAmount"},
 {"DamageRepair","MinBaseRepairAmount"},{"DamageRepair","MaxBaseRepairAmount"},
 {"DamageRepair","BaseRepairAmountA"},{"DamageRepair","BaseRepairAmountB"},
 {"DamageRepair","BaseRepairAmountC"}
};
static_assert(sizeof(mappings)/sizeof(mappings[0])==repairAuthoringKeyCount50A);
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
bool readRepairAuthoring50A(const char* text,std::size_t bytes,RepairAuthoring50A& output,PodPhysicsDiagnostic49N& diagnostic,RepairProvenance50A* provenance) noexcept {
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
    RepairAuthoring50A result{};
    RepairProvenance50A audit{};
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
        for (std::size_t i=0;i<repairAuthoringKeyCount50A;++i) if (equal(section,mappings[i].section) && equal(key,mappings[i].key)) {
            if (!parseImpactNumber49Y(value.p,value.n,result.values[i].value)) return fail(diagnostic,PodPhysicsError49N::numeric_syntax,value.offset);
            result.values[i].present=true;
            audit.fields[i]={static_cast<std::uint32_t>(section.offset),static_cast<std::uint32_t>(section.n),static_cast<std::uint32_t>(key.offset),static_cast<std::uint32_t>(key.n),static_cast<std::uint32_t>(value.offset),static_cast<std::uint32_t>(value.n)};
            break;
        }
    }
    output=result; if(provenance) *provenance=audit;
    diagnostic={PodPhysicsError49N::none,0};return true;
}
}
