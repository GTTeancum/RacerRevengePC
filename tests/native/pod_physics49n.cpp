#include "../../src/assets/pod_physics49n.h"
#include "../../evidence/authoredInitialization49N/authoring-source/conversion_cases49n.h"
#include "../../evidence/authoredInitialization49N/authoring-source/pod01_projection49n.h"
#include <cfenv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <new>
#include <stdexcept>
#include <string>

static std::uint64_t allocations=0;
void* operator new(std::size_t n) { if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p,std::size_t) noexcept { std::free(p); }
void operator delete[](void* p,std::size_t) noexcept { std::free(p); }
using namespace rr::assets;
using namespace rr::game;
static std::uint64_t checks=0,calls=0,rejections=0,numericCases=0,loads=0;
static void require(bool ok,const char* m) { ++checks;if(!ok)throw std::runtime_error(m); }
static std::uint32_t bits(float f) { std::uint32_t b;std::memcpy(&b,&f,4);return b; }
static float value(std::uint32_t b) { float f;std::memcpy(&f,&b,4);return f; }
static bool parse(const char* p,std::size_t n,float& f,std::uint64_t* d=nullptr) {
    const auto before=allocations;const bool ok=parsePodPhysicsNumber49N(p,n,f,d);
    require(allocations==before,"number conversion allocated");++calls;return ok;
}
static bool read(const char* p,std::size_t n,AuthoredParameters49N& o,PodPhysicsDiagnostic49N& d,PodPhysicsProvenance49N* a=nullptr) {
    const auto before=allocations;const bool ok=readPodPhysics49N(p,n,o,d,a);
    require(allocations==before,"PHY reader allocated");++calls;return ok;
}
static void reject(const std::string& text,PodPhysicsError49N expected) {
    AuthoredParameters49N out;std::memset(&out,0x5a,sizeof(out));
    PodPhysicsProvenance49N audit;std::memset(&audit,0xa5,sizeof(audit));
    unsigned char old[sizeof(out)],oldAudit[sizeof(audit)];
    std::memcpy(old,&out,sizeof(out));std::memcpy(oldAudit,&audit,sizeof(audit));
    PodPhysicsDiagnostic49N d{};
    require(!read(text.data(),text.size(),out,d,&audit),"bad PHY accepted");
    require(d.code==expected,"unexpected reader error code");
    require(d.byteOffset<=text.size(),"diagnostic outside source");
    require(std::memcmp(&out,old,sizeof(out))==0,"failed read changed destination");
    require(std::memcmp(&audit,oldAudit,sizeof(audit))==0,"failed read changed provenance");++rejections;
}
static std::string replaceOne(std::string text,const char* old,const char* replacement) {
    const auto at=text.find(old);require(at!=std::string::npos,"mutation source not found");
    text.replace(at,std::strlen(old),replacement);return text;
}
static void numeric() {
    for(const auto& c:pod_phy49n_original::cases) {
        float f=value(0x7fc0dead);std::uint64_t d=0xdeadbeef;
        require(parse(c.token,std::strlen(c.token),f,&d),"closed original token rejected");
        if(bits(f)!=c.binary32 || d!=c.binary64){std::cerr<<"Token mismatch "<<c.token<<'\n';require(false,"original conversion differs");}
        ++numericCases;
    }
    const char* invalid[]={"","+","-",".",".1","1.","1e3","1E-3","1f","1;","1,2"," 1","1 ","1 2","nan","NaN","inf","Infinity","0x1","1000000000","0.0000001","123.4567890","--1","1\t"};
    for(const auto* s:invalid) {
        float f=value(0xdeadbeef);std::uint64_t d=0xfeedface;
        require(!parse(s,std::strlen(s),f,&d),"unsupported numeric grammar accepted");
        require(bits(f)==0xdeadbeef && d==0xfeedface,"failed numeric parse wrote outputs");++rejections;
    }
    float f=9;std::uint64_t d=9;require(!parse(nullptr,1,f,&d),"null number accepted");
    require(!parse("1",0,f,&d),"empty number accepted");
    require(!parse("1",100,f,&d),"overlong number accepted");
    const char embedded[]={'1',0,'2'};require(!parse(embedded,3,f,&d),"embedded NUL numeric accepted");
    // Conversion uses integer operations and is independent of ambient FP mode.
    for(int mode:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}) {
        require(std::fesetround(mode)==0,"host rounding mode unavailable");
        for(const char* s:{"-0.0","0.3","-0.3","16777217","16777219","0.000001"}) {
            const pod_phy49n_original::Case* expected=nullptr;
            for(const auto& c:pod_phy49n_original::cases)if(std::strcmp(c.token,s)==0){expected=&c;break;}
            require(expected!=nullptr,"rounding sample missing from original corpus");
            require(parse(s,std::strlen(s),f,&d),"rounding-mode parse rejected");
            require(bits(f)==expected->binary32 && d==expected->binary64,"host rounding changed conversion");
        }
    }
    require(std::fesetround(FE_TONEAREST)==0,"restore rounding mode");
}
static void valid(const std::string& text) {
    AuthoredParameters49N out{};PodPhysicsProvenance49N a{};PodPhysicsDiagnostic49N d{};
    require(read(text.data(),text.size(),out,d,&a),"actual Pod01 PHY rejected");++loads;
    require(d.code==PodPhysicsError49N::none,"successful diagnostic mismatch");
    require(std::size(pod_phy49n_fixture::fields)==authoredKeyCount49N,"complete expected scalar map");
    for(const auto& e:pod_phy49n_fixture::fields) {
        const auto i=static_cast<std::size_t>(e.key);require(out.values[i].present,"actual mapped scalar missing");
        require(bits(out.values[i].value)==e.word,"actual scalar original word mismatch");
        require(a.scalars[i].keyOffset==e.keyOffset && a.scalars[i].valueOffset==e.valueOffset && a.scalars[i].valueBytes==e.valueBytes,"actual source provenance mismatch");
    }
    require(out.leftEnginePosition.present && out.rightEnginePosition.present,"actual vectors missing");
    require(bits(out.leftEnginePosition.value.v[0])==0xbe99999a && bits(out.rightEnginePosition.value.v[0])==0x3e99999a,"actual vector original X conversion");
    require(bits(out.leftEnginePosition.value.v[1])==0 && bits(out.leftEnginePosition.value.v[2])==0 && bits(out.rightEnginePosition.value.v[1])==0 && bits(out.rightEnginePosition.value.v[2])==0,"actual vector zero words");
    require(out.tuning.sectionPresent && out.tuning.useValues.present && out.tuning.useValues.value==1,"actual UseValues");
    require(out.tuning.useAValues.present && out.tuning.useBValues.present && out.tuning.useCValues.present && out.tuning.useAValues.value==0 && out.tuning.useBValues.value==0 && out.tuning.useCValues.value==0,"actual class flags");
    require(out.tuning.upgradePercent.present && bits(out.tuning.upgradePercent.value)==0,"actual UpgradePercent");
    require(a.tuning[0].valueOffset==6477 && a.tuning[4].valueOffset==6579,"tuning offsets");
    const std::string noTuning=text.substr(0,text.find("[Tuning]"));
    require(read(noTuning.data(),noTuning.size(),out,d,&a),"optional tuning rejected");++loads;
    require(!out.tuning.sectionPresent && !out.tuning.useValues.present && !out.tuning.upgradePercent.present,"reader synthesized tuning defaults");
    auto lower=text;for(char& c:lower)if(c>='A' && c<='Z')c=char(c+32);
    require(read(lower.data(),lower.size(),out,d),"ASCII case-insensitive source lookup rejected");++loads;
    require(bits(out.values[static_cast<std::size_t>(AuthoredKey49N::BasicDrag)].value)==0x3ca3d70a,"case-insensitive value mismatch");
    auto lf=text;for(std::size_t i=0;(i=lf.find('\r',i))!=std::string::npos;)lf.erase(i,1);
    require(read(lf.data(),lf.size(),out,d),"LF source rejected");++loads;
    auto spaces=replaceOne(text,"LeftEnginePosition = -0.3,0.0,0.0","LeftEnginePosition\t =  -0.3 , 0.0 , 0.0 \t");
    require(read(spaces.data(),spaces.size(),out,d),"admitted vector delimiter spaces rejected");++loads;
    auto missingTuningKey=replaceOne(text,"UseAValues = 0\r\n","");
    require(read(missingTuningKey.data(),missingTuningKey.size(),out,d),"optional tuning key rejected");++loads;
    require(!out.tuning.useAValues.present,"missing optional tuning flag default synthesized");
}
static void malformed(const std::string& text) {
    reject("",PodPhysicsError49N::invalid_input);
    reject(std::string(maxPodPhysicsBytes49N+1,' '),PodPhysicsError49N::text_limit);
    reject(std::string(256,' ')+"\n"+text,PodPhysicsError49N::line_limit);
    auto bad=text;bad[1]=0;reject(bad,PodPhysicsError49N::invalid_character);
    bad=text;bad[10]=char(0xff);reject(bad,PodPhysicsError49N::invalid_character);
    bad=text;bad[10]=char(127);reject(bad,PodPhysicsError49N::invalid_character);
    reject("Weight=1\n"+text,PodPhysicsError49N::syntax);
    reject(replaceOne(text,"[Physical Properties]","[Physical Properties"),PodPhysicsError49N::syntax);
    reject(replaceOne(text,"Weight = 1000","Weight 1000"),PodPhysicsError49N::syntax);
    reject(replaceOne(text,"Weight = 1000","Weight = "),PodPhysicsError49N::syntax);
    reject(replaceOne(text,"Weight = 1000","Wei ght = 1000"),PodPhysicsError49N::syntax);
    reject(replaceOne(text,"Weight = 1000\r\n",""),PodPhysicsError49N::missing_key);
    reject(replaceOne(text,"LeftEnginePosition = -0.3,0.0,0.0\r\n",""),PodPhysicsError49N::missing_key);
    reject(text+"[Engine]\n",PodPhysicsError49N::duplicate_section);
    reject(replaceOne(text,"Weight = 1000","Weight = 1000\nweight = 1"),PodPhysicsError49N::duplicate_key);
    reject(replaceOne(text,"Name = \"Anakin Skywalker\"","Name = \"Anakin Skywalker\"\nNAME = other"),PodPhysicsError49N::duplicate_key);
    reject(replaceOne(text,"Weight = 1000","Weight = 1e3"),PodPhysicsError49N::numeric_syntax);
    reject(replaceOne(text,"Weight = 1000","Weight = 1;comment"),PodPhysicsError49N::numeric_syntax);
    reject(replaceOne(text,"Weight = 1000","Weight = \"1000\""),PodPhysicsError49N::numeric_syntax);
    reject(replaceOne(text,"Weight = 1000","Weight = 1000000000"),PodPhysicsError49N::numeric_syntax);
    for(const char* v:{"1,2","1,2,3,4",",2,3","1,,3","1,2,","1,2,nan","1,2,3f"})
        reject(replaceOne(text,"-0.3,0.0,0.0",v),PodPhysicsError49N::vector_syntax);
    for(const char* v:{"UseValues = 2","UseValues = -1","UseValues = T","UseValues = 01"})
        reject(replaceOne(text,"UseValues = 1",v),PodPhysicsError49N::integer_syntax);
    bad.clear();for(unsigned i=0;i<33;++i)bad+="[Section"+std::to_string(i)+"]\n";
    reject(bad+text,PodPhysicsError49N::section_limit);
    bad="[Extra]\n";for(unsigned i=0;i<513;++i)bad+="Key"+std::to_string(i)+"=1\n";
    reject(bad+text,PodPhysicsError49N::key_limit);
    // Every truncated authentic prefix must fail atomically unless it contains
    // the complete required keys (Tuning is deliberately optional).
    const auto complete=text.find("[Tuning]");
    for(std::size_t n=0;n<complete;n+=37) {
        AuthoredParameters49N out{};PodPhysicsDiagnostic49N d{};
        const auto prefix=text.substr(0,n);
        const bool ok=read(prefix.data(),prefix.size(),out,d);
        if(ok)require(n>text.find("[MaxAbilities]"),"accepted truncation before final mandatory section");
        else {require(d.code!=PodPhysicsError49N::none,"truncation rejection missing diagnostic");++rejections;}
    }
}
int main(int argc,char** argv) {
    try {
        const char* path=argc>1?argv[1]:"evidence/authoredInitialization49N/authoring-source/Pod01.phy";
        std::ifstream file(path,std::ios::binary);require(bool(file),"PHY fixture missing");
        const std::string text((std::istreambuf_iterator<char>(file)),{});
        require(text.size()==6584,"actual byte-exact fixture extent mismatch");
        numeric();valid(text);malformed(text);
        std::cout<<"PASS PodPhysics49N: "<<numericCases<<" original-word numeric cases, "<<loads<<" admitted loads, "<<rejections<<" malformed/domain rejections, "<<checks<<" checks, "<<calls<<" zero-allocation native calls\n";
        return 0;
    } catch(const std::exception& e) {std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
