// 48O correction: raw finite scalar MIN.S/MAX.S selection plus independently
// executed original spring-force/secondary edge projections. No timing claim.
#include "scalar_minmax48o.h"
#include "spring_force48l.h"
#include "json.hpp"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#if defined(_MSC_VER)
#include <malloc.h>
#define RR_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_NOINLINE __attribute__((noinline))
#else
#define RR_NOINLINE
#endif
// Setup, fixture decoding, references, and diagnostics are outside hot scopes.
static bool watchAllocations=false;
static std::size_t observedAllocations=0;
RR_NOINLINE void* operator new(std::size_t n){if(watchAllocations)++observedAllocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
RR_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void* operator new(std::size_t n,std::align_val_t alignment){
    if(watchAllocations)++observedAllocations;
    const auto a=static_cast<std::size_t>(alignment);
#if defined(_MSC_VER)
    void* p=_aligned_malloc(n?n:1,a);
#else
    void* p=nullptr;if(posix_memalign(&p,a,n?n:1))p=nullptr;
#endif
    if(p)return p;
    throw std::bad_alloc();
}
RR_NOINLINE void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
RR_NOINLINE void operator delete(void* p,std::align_val_t) noexcept {
#if defined(_MSC_VER)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
RR_NOINLINE void operator delete[](void* p,std::align_val_t a) noexcept {::operator delete(p,a);}
RR_NOINLINE void operator delete(void* p,std::size_t,std::align_val_t a) noexcept {::operator delete(p,a);}
RR_NOINLINE void operator delete[](void* p,std::size_t,std::align_val_t a) noexcept {::operator delete(p,a);}
using namespace rr::game;
using Json=nlohmann::json;
namespace {
using V=Vector3_48C;
std::size_t checks=0,failures=0,utilityPairs=0,zeroPairs=0,secondaryCases=0,forceCases=0;
std::size_t deltaCases=0,secondaryMutationCases=0,secondaryMutationWords=0,utilityMutationSelections=0;
std::size_t safetyCases=0,aliasCases=0,hotScopes=0;
std::string label;
void check(bool ok,const char* message){++checks;if(!ok){++failures;if(failures<=30)std::cerr<<label<<": "<<message<<'\n';}}
std::uint32_t bits(float value){std::uint32_t word;std::memcpy(&word,&value,4);return word;}
float fp(std::uint32_t word){float value;std::memcpy(&value,&word,4);return value;}
// Independent signed-integer expression of the scalar rule. This reference
// neither calls scalarOrderKey48O nor transforms words into an unsigned key.
std::uint32_t reference(std::uint32_t a,std::uint32_t b,bool minimum){
    const std::int64_t x=a<0x80000000u?std::int64_t(a):std::int64_t(a)-0x100000000LL;
    const std::int64_t y=b<0x80000000u?std::int64_t(b):std::int64_t(b)-0x100000000LL;
    if(x<0&&y<0)minimum=!minimum;
    return minimum?(x<y?a:b):(x>y?a:b);
}
float legacyMin(float a,float b){return a<b?a:b;}
float legacyMax(float a,float b){return a>b?a:b;}
template<class F>auto hot(F&& call){
    const auto before=observedAllocations;watchAllocations=true;
    const auto result=call();watchAllocations=false;++hotScopes;
    check(observedAllocations==before,"native hot scope allocated");return result;
}
void pair(std::uint32_t a,std::uint32_t b){
    const float x=fp(a),y=fp(b);
    const auto got=hot([&]{return std::array<std::uint32_t,4>{{
        bits(detail::scalarMin48O(x,y)),bits(detail::scalarMax48O(x,y)),
        bits(detail::scalarMin48O(y,x)),bits(detail::scalarMax48O(y,x))}};});
    const std::array<std::uint32_t,4> expected{{reference(a,b,true),reference(a,b,false),reference(b,a,true),reference(b,a,false)}};
    for(unsigned i=0;i<4;++i)check(got[i]==expected[i],"finite scalar selection differs from signed-word reference");
    utilityMutationSelections+=bits(legacyMin(x,y))!=expected[0];
    utilityMutationSelections+=bits(legacyMax(x,y))!=expected[1];++utilityPairs;
}
void utility(){
    const std::array<std::uint32_t,24> anchors{{
        0,0x80000000u,1,0x80000001u,0x007fffffu,0x807fffffu,
        0x00800000u,0x80800000u,0x00800001u,0x80800001u,
        0x3effffffu,0xbeffffffu,0x3f000000u,0xbf000000u,
        0x3f7fffffu,0xbf7fffffu,0x3f800000u,0xbf800000u,
        0x3f800001u,0xbf800001u,0x7f7ffffeu,0xff7ffffeu,0x7f7fffffu,0xff7fffffu}};
    for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}){
        label="finite scalar utility rounding "+std::to_string(rounding);
        if(std::fesetround(rounding)!=0)throw std::runtime_error("rounding setup failed");
        // All four ordered zero-sign pairs, including equal signs, separately.
        for(auto a:{0u,0x80000000u})for(auto b:{0u,0x80000000u}){pair(a,b);++zeroPairs;}
        for(auto a:anchors)for(auto b:anchors)pair(a,b);
        std::uint32_t random=0x48c04e71u;
        auto next=[&]{random^=random<<13;random^=random>>17;random^=random<<5;return random;};
        auto finiteWord=[&]{std::uint32_t value;do{value=next();}while((value&0x7f800000u)==0x7f800000u);return value;};
        for(unsigned i=0;i<65536;++i){const auto a=finiteWord();pair(a,(i&127u)?finiteWord():a);}
        check(std::fegetround()==rounding,"utility changed rounding mode");
    }
    check(zeroPairs==16,"complete zero pair/rounding matrix");
    check(utilityMutationSelections>=16,"legacy float comparator mutation was not detected");
}
V vector(const float* data){return {{data[0],data[1],data[2]}};}
template<std::size_t N>std::array<float,N> raw(const Json& j){
    if(!j.is_array()||j.size()!=N)throw std::runtime_error("raw fixture width");
    std::array<float,N> out{};for(std::size_t i=0;i<N;++i)out[i]=fp(j.at(i).get<std::uint32_t>());return out;
}
V legacySecondary(const V& normal,const V& cached,const V& velocity){
    volatile float x=cached.v[0]*normal.v[0],y=cached.v[1]*normal.v[1],z=cached.v[2]*normal.v[2];
    volatile float xy=x+y,speed=xy+z;const float correction=-legacyMin(speed,0.f);V out{};
    for(unsigned i=0;i<3;++i){volatile float change=correction*normal.v[i],value=velocity.v[i]+change;out.v[i]=value;}
    return out;
}
void deltas(const Json& row){
    for(const auto& delta:row.at("corrected_path_intermediate_deltas")){
        const auto pc=delta.at("pc").get<std::string>();
        const bool minimum=pc=="002983F0"||pc=="0029724C"||pc=="002972AC";
        check(minimum||pc=="00297294"||pc=="002972C8","recognized original scalar instruction PC");
        const auto word=[&](const char* key){return static_cast<std::uint32_t>(std::stoul(delta.at(key).get<std::string>(),nullptr,16));};
        const auto a=word("first"),b=word("second"),expected=word("corrected");
        check(reference(a,b,minimum)==expected,"original scalar delta agrees with independent signed-word reference");
        const auto got=hot([&]{return bits(minimum?detail::scalarMin48O(fp(a),fp(b)):detail::scalarMax48O(fp(a),fp(b)));});
        check(got==expected,"native scalar selection agrees with original intermediate words");
        const auto legacy=bits(minimum?legacyMin(fp(a),fp(b)):legacyMax(fp(a),fp(b)));
        check(legacy==word("legacy")&&legacy!=expected,"legacy comparator mutation fails original intermediate edge");++deltaCases;
    }
}
void secondary(const Json& row){
    label=row.at("name").get<std::string>();const auto f=raw<9>(row.at("input_words"));
    const auto expected=raw<3>(row.at("expected_words")),legacyWords=raw<3>(row.at("legacy_words"));
    const V normal=vector(f.data()),cached=vector(f.data()+3),velocity=vector(f.data()+6);
    const auto n=normal,c=cached,v=velocity;V out{{17,19,23}};
    check(row.at("source_reads")==0,"secondary original source reads are initialized");
    check(hot([&]{return removeSpringInwardVelocity48L(normal,cached,velocity,out);}),"secondary edge accepted");
    const auto legacy=legacySecondary(normal,cached,velocity);unsigned changed=0;
    for(unsigned i=0;i<3;++i){
        check(bits(out.v[i])==bits(expected[i]),"secondary original raw output word");
        check(bits(legacy.v[i])==bits(legacyWords[i]),"explicit legacy mutation matches original legacy projection");
        changed+=bits(legacy.v[i])!=bits(expected[i]);
    }
    check(changed==row.at("changed_output_indices").size(),"secondary changed-word count");
    secondaryMutationCases+=changed!=0;secondaryMutationWords+=changed;
    check(std::memcmp(&normal,&n,sizeof(n))==0&&std::memcmp(&cached,&c,sizeof(c))==0&&std::memcmp(&velocity,&v,sizeof(v))==0,"secondary immutable inputs");
    for(unsigned which=0;which<3;++which){
        auto an=n,ac=c,av=v;V& aliased=which==0?an:which==1?ac:av;
        check(hot([&]{return removeSpringInwardVelocity48L(an,ac,av,aliased);}),"secondary exact input alias accepted");
        for(unsigned i=0;i<3;++i)check(bits(aliased.v[i])==bits(expected[i]),"secondary alias raw output word");
        ++aliasCases;
    }
    deltas(row);++secondaryCases;
}
SpringImpactBody48K body(const float* p){
    SpringImpactBody48K b{};std::memcpy(b.state.scene.v,p,64);b.parameters.mass=p[16];b.parameters.inverseMass=p[17];
    b.state.loads={vector(p+18),vector(p+21)};return b;
}
void force(const Json& row){
    label=row.at("name").get<std::string>();const auto f=raw<76>(row.at("input_words"));
    const auto expected=raw<15>(row.at("expected_words"));
    SpringForceParameters48L p{vector(f.data()+8),{vector(f.data()+14),vector(f.data()+11),f[17],f[26]!=0,f[27]!=0},
        f[18],f[19],f[20],f[21],f[22],f[23],f[24]!=0,f[25]!=0};
    auto a=body(f.data()+28),b=body(f.data()+52);a.localCenter=p.pointForce.primaryLocalCenter;
    const auto beforeA=a,beforeB=b;const auto beforeP=p;const auto n=vector(f.data()),point=vector(f.data()+3);
    const auto beforeN=n,beforePoint=point;SpringFrictionResult48J result{};
    check(row.at("source_reads")==0,"force original source reads are initialized");
    check(hot([&]{return applySpringForce48L(n,point,f[6],p,a,f[7]!=0?&b:nullptr,result);}),"force original edge accepted");
    const V* fields[]={&result.worldForce,&result.primary.force,&result.primary.torque,&result.other.force,&result.other.torque};
    for(unsigned i=0;i<15;++i)check(bits(fields[i/3]->v[i%3])==bits(expected[i]),"force original raw output word");
    check(std::memcmp(&a,&beforeA,sizeof(a))==0&&std::memcmp(&b,&beforeB,sizeof(b))==0,"force immutable bodies");
    check(std::memcmp(&p,&beforeP,sizeof(p))==0&&std::memcmp(&n,&beforeN,sizeof(n))==0&&std::memcmp(&point,&beforePoint,sizeof(point))==0,"force immutable parameters/geometry");
    deltas(row);++forceCases;
}
void safety(){
    label="native invalid-input and transactional contract";
    const V normal{{0,1,0}},zero{},velocity{{1,-2,3}};
    const float huge=std::numeric_limits<float>::max();
    auto rejectSecondary=[&](const V& n,const V& c,const V& v){
        V out{{fp(0x80000000u),17,23}};const auto saved=out;const auto beforeN=n,beforeC=c,beforeV=v;
        check(!hot([&]{return removeSpringInwardVelocity48L(n,c,v,out);}),"invalid secondary input rejected");
        check(std::memcmp(&out,&saved,sizeof(out))==0,"secondary rejection leaves all output bytes unchanged");
        check(std::memcmp(&n,&beforeN,sizeof(n))==0&&std::memcmp(&c,&beforeC,sizeof(c))==0&&std::memcmp(&v,&beforeV,sizeof(v))==0,"secondary rejection leaves inputs unchanged");++safetyCases;
    };
    for(auto word:{0x7f800000u,0xff800000u,0x7fc12345u})for(unsigned field=0;field<3;++field)for(unsigned lane=0;lane<3;++lane){
        auto n=normal,c=zero,v=velocity;(field==0?n:field==1?c:v).v[lane]=fp(word);rejectSecondary(n,c,v);
    }
    rejectSecondary(V{{0,2,0}},V{{0,huge,0}},zero); // Dot overflow hidden by MIN.
    rejectSecondary(V{{0,2,0}},V{{0,-huge/2,0}},zero); // Correction multiply overflow.
    rejectSecondary(normal,V{{0,-huge,0}},V{{0,huge,0}}); // Final addition overflow.
    SpringForceParameters48L base{};base.contactVelocity120=velocity;base.pointForce.torqueScale=1;
    base.maxPenetration90=2;base.maxSpeed94=10;base.stiffness9C=8;base.dampingA0=4;base.impactStiffnessA4=16;base.impactDampingA8=2;
    SpringImpactBody48K primary{};for(unsigned i=0;i<4;++i)primary.state.scene.v[5*i]=1;
    primary.parameters.mass=2;primary.parameters.inverseMass=.5f;auto other=primary;other.parameters.mass=4;other.parameters.inverseMass=.25f;
    auto rejectForce=[&](const V& n,const V& point,float depth,const SpringForceParameters48L& p,const SpringImpactBody48K& a,const SpringImpactBody48K* b){
        SpringFrictionResult48J out{};out.worldForce={{fp(0x80000000u),17,23}};out.other.torque={{31,37,41}};
        const auto saved=out;const auto beforeA=a;const auto beforeP=p;const auto beforeN=n,beforePoint=point;const auto beforeB=b?*b:SpringImpactBody48K{};
        check(!hot([&]{return applySpringForce48L(n,point,depth,p,a,b,out);}),"invalid force input rejected");
        check(std::memcmp(&out,&saved,sizeof(out))==0,"force rejection leaves all output bytes unchanged");
        check(std::memcmp(&a,&beforeA,sizeof(a))==0&&(!b||std::memcmp(b,&beforeB,sizeof(*b))==0),"force rejection leaves bodies unchanged");
        check(std::memcmp(&p,&beforeP,sizeof(p))==0&&std::memcmp(&n,&beforeN,sizeof(n))==0&&std::memcmp(&point,&beforePoint,sizeof(point))==0,"force rejection leaves parameters/geometry unchanged");++safetyCases;
    };
    for(auto word:{0x7f800000u,0xff800000u,0x7fc12345u}){
        const auto bad=fp(word);
        for(unsigned field=0;field<7;++field){
            auto p=base;float* fields[]={&p.maxPenetration90,&p.maxSpeed94,&p.stiffness9C,&p.dampingA0,&p.contactVelocity120.v[0],&p.contactVelocity120.v[1],&p.contactVelocity120.v[2]};
            *fields[field]=bad;rejectForce(normal,zero,1,p,primary,nullptr);
        }
        for(unsigned lane=0;lane<3;++lane){auto n=normal,point=zero;n.v[lane]=bad;point.v[lane]=bad;rejectForce(n,zero,1,base,primary,nullptr);rejectForce(normal,point,1,base,primary,nullptr);}
        rejectForce(normal,zero,bad,base,primary,nullptr);
        auto p=base;p.impact54=true;p.impactStiffnessA4=bad;rejectForce(normal,zero,1,p,primary,nullptr);
        p=base;p.impact54=true;p.impactDampingA8=bad;rejectForce(normal,zero,1,p,primary,nullptr);
    }
    rejectForce(normal,zero,1,base,primary,&primary);
    auto p=base;p.contactVelocity120={{0,huge,0}};rejectForce(V{{0,2,0}},zero,1,p,primary,nullptr);
    p=base;p.stiffness9C=huge;rejectForce(normal,zero,2,p,primary,nullptr);
    p=base;p.dampingA0=huge;rejectForce(normal,zero,1,p,primary,nullptr);
    auto b=other;b.state.loads.force.v[2]=fp(0x7fc12345u);rejectForce(normal,zero,1,base,primary,&b);
    // A selected opposite-body late failure must not commit primary output.
    b=other;b.state.loads.force.v[1]=huge;p=base;p.contactVelocity120=zero;p.stiffness9C=huge;p.maxPenetration90=1;
    rejectForce(V{{0,-1,0}},zero,1,p,primary,&b);
}
}
int main(int argc,char** argv){
    if(argc!=2){std::cerr<<"usage: scalar_minmax48o_check REPO_ROOT\n";return 2;}
    const int savedRounding=std::fegetround();
    try{
        utility();if(std::fesetround(FE_TONEAREST)!=0)throw std::runtime_error("fixture rounding setup");
        const auto path=std::filesystem::path(argv[1])/"evidence/minmaxAudit48O/edge-fixtures.json";
        std::ifstream in(path);if(!in)throw std::runtime_error("missing minmaxAudit48O edge fixtures");Json fixtures;in>>fixtures;
        label="original edge fixture provenance";
        check(fixtures.at("schema")=="scalar-minmax48o-edge-v1","known fixture schema");
        check(fixtures.at("source_reads")==0&&fixtures.at("no_original_words_patched")==true,"original source evidence is unpatched and initialized");
        if(fixtures.at("secondary").size()!=8||fixtures.at("force").size()!=6)throw std::runtime_error("frozen edge corpus counts");
        for(const auto& row:fixtures.at("secondary"))secondary(row);
        for(const auto& row:fixtures.at("force"))force(row);
        check(secondaryMutationCases==6&&secondaryMutationWords==18,"legacy secondary mutation must fail six cases and eighteen raw words");
        safety();check(observedAllocations==0,"all instrumented native paths allocate zero times");
        check(std::fegetround()==FE_TONEAREST,"native spring APIs preserved fixture rounding mode");
        check(std::fesetround(savedRounding)==0,"caller rounding mode restored");
    }catch(const std::exception& e){watchAllocations=false;std::fesetround(savedRounding);std::cerr<<"scalar48O fixture/test error: "<<e.what()<<'\n';return 2;}
    std::cout<<"scalar48O min/max "<<checks<<" checks, "<<failures<<" failures; "<<utilityPairs<<" finite pairs; "<<zeroPairs
             <<" zero pairs; "<<secondaryCases<<" secondary and "<<forceCases<<" force original edges; "<<deltaCases
             <<" original intermediate deltas; legacy mutation "<<secondaryMutationCases<<" secondary cases/"<<secondaryMutationWords
             <<" words, "<<utilityMutationSelections<<" utility selections; "<<safetyCases<<" safety cases; "<<aliasCases
             <<" aliases; "<<hotScopes<<" hot scopes; "<<observedAllocations<<" allocations\n";
    return failures?1:0;
}
