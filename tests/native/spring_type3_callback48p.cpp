#include "spring_type3_callback48p.h"
#include <cmath>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>

static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_TEST_NOINLINE __attribute__((noinline))
#else
#define RR_TEST_NOINLINE
#endif
RR_TEST_NOINLINE void* operator new(std::size_t n){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(n?n:1))return p;
    throw std::bad_alloc();
}
RR_TEST_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
namespace {
std::size_t checks=0,failures=0,caseNumber=0;
void check(bool condition,const char* message){
    ++checks;if(!condition){++failures;if(failures<20)std::cerr<<"case "<<caseNumber<<": "<<message<<'\n';}
}
std::array<std::uint32_t,11> words(const SpringRecordState48N& s){return {
    s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,
    static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
SpringRecordState48N state(const std::uint32_t* p){return {
    p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],static_cast<std::int32_t>(p[8]),static_cast<std::int32_t>(p[9]),p[10]};}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;
    std::ifstream file(std::filesystem::path(argv[1])/"evidence/springCallback48P/static-type3-callback-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;file.read(reinterpret_cast<char*>(&count),4);file.read(reinterpret_cast<char*>(&width),4);
    if(!file||count!=768||width!=378)return 2;
    std::size_t totalAppended=0,totalSaturated=0,totalSkipped=0;
    for(caseNumber=0;caseNumber<count;++caseNumber){
        std::array<std::uint32_t,378> row{};file.read(reinterpret_cast<char*>(row.data()),sizeof(row));if(!file)return 2;
        std::array<SpringVector4_48N,4> starts{},directions{},normals{},points{};
        std::array<std::uint32_t,4> hits{},metadata{};std::array<float,4> lengths{};
        for(unsigned i=0;i<4;++i){
            std::memcpy(&starts[i],row.data()+51+i*18,16);std::memcpy(&directions[i],row.data()+55+i*18,16);
            std::memcpy(&normals[i],row.data()+59+i*18,16);std::memcpy(&points[i],row.data()+63+i*18,16);
            hits[i]=row[67+i*18];metadata[i]=row[68+i*18];
        }
        std::memcpy(lengths.data(),row.data()+31,16);
        std::array<SpringRecord48N,5> records{};std::memcpy(records.data(),row.data()+123,sizeof(records));
        std::array<char,32> name{};std::memcpy(name.data(),row.data()+23,name.size());
        auto s=state(row.data()+12);
        SpringStaticType3CallbackInput48P input{};std::memcpy(&input.bodyScene,row.data()+35,sizeof(input.bodyScene));
        input.candidateIdentity=row[0];input.candidateCategoryA4=row[1];input.currentCandidateCategoryA4=row[2];
        input.skipMask=row[3];input.dynamicMask=row[4];input.materialMask=row[5];input.queryTypeTag=row[6];
        input.hasMaterialScene=row[7]!=0;input.sceneName=name.data();input.sceneNameBytes=row[8];
        SpringStaticType3CallbackSource48P source{starts.data(),4,directions.data(),4,lengths.data(),4,normals.data(),4,points.data(),4,hits.data(),4,metadata.data(),4};
        const auto oldInput=input;const auto oldSource=source;const auto oldStarts=starts,oldDirections=directions,oldNormals=normals,oldPoints=points;
        const auto oldLengths=lengths;const auto oldHits=hits,oldMetadata=metadata;const auto oldName=name;
        SpringStaticCallbackResult48N result{91,92,93};
        watchAllocations=true;const auto status=applySpringStaticType3Callback48P(input,source,records.data(),row[10],s,result);watchAllocations=false;
        check(status==(row[374]?SpringCallbackStatus48N::Skipped:SpringCallbackStatus48N::Complete),"original callback status");
        const auto got=words(s);
        for(unsigned j=0;j<11;++j)check(got[j]==row[243+j],"all state words match original");
        std::array<std::uint32_t,120> recordWords{};std::memcpy(recordWords.data(),records.data(),sizeof(records));
        for(unsigned j=0;j<120;++j)check(recordWords[j]==row[254+j],"record bits match original including W and stale tail");
        if(row[374]){++totalSkipped;check(result.appended==91&&result.saturated==92&&result.material==93,"skipped result untouched");}
        else {check(result.appended==row[375]&&result.saturated==row[376]&&result.material==row[377],"callback diagnostics");totalAppended+=result.appended;totalSaturated+=result.saturated;}
        check(std::memcmp(&input,&oldInput,sizeof(input))==0&&std::memcmp(&source,&oldSource,sizeof(source))==0,"descriptors unchanged");
        check(std::memcmp(starts.data(),oldStarts.data(),sizeof(starts))==0&&std::memcmp(directions.data(),oldDirections.data(),sizeof(directions))==0,"start/direction bits unchanged");
        check(std::memcmp(normals.data(),oldNormals.data(),sizeof(normals))==0&&std::memcmp(points.data(),oldPoints.data(),sizeof(points))==0,"copied payloads unchanged");
        check(std::memcmp(lengths.data(),oldLengths.data(),sizeof(lengths))==0&&hits==oldHits&&metadata==oldMetadata&&name==oldName,"lengths/flags/metadata/name unchanged");
    }
    if(file.peek()!=std::ifstream::traits_type::eof())return 2;
    std::array<SpringRecord48N,6> records{};std::memset(records.data(),0xa5,sizeof(records));const auto originalRecords=records;
    std::array<SpringVector4_48N,4> starts{},directions{},normals{},points{};
    for(unsigned i=0;i<4;++i){starts[i]={{float(i+2),3,-4,-0.f}};directions[i]={{.5f,-.5f,.25f,99}};normals[i]={{1,0,0,-7}};points[i]={{2,3,4,42}};}
    std::array<float,4> lengths{{2,3,4,std::numeric_limits<float>::quiet_NaN()}};
    std::array<std::uint32_t,4> hits{{1,0xffffffffu,0,0}},metadata{{17,19,23,29}};
    const char name[]="trackfr7.col";
    SpringStaticType3CallbackInput48P base{};for(unsigned i=0;i<4;++i)base.bodyScene.v[5*i]=1;
    base.candidateIdentity=17;base.candidateCategoryA4=4;base.skipMask=8;base.dynamicMask=2;base.materialMask=4;
    base.hasMaterialScene=true;base.sceneName=name;base.sceneNameBytes=sizeof(name);
    SpringStaticType3CallbackSource48P source{starts.data(),4,directions.data(),4,lengths.data(),4,normals.data(),4,points.data(),4,hits.data(),4,metadata.data(),4};
    const SpringRecordState48N initial{11,12,13,14,15,16,18,2,1,5,21};
    const SpringStaticCallbackResult48N sentinel{123,456,789};
    auto reject=[&](const SpringStaticType3CallbackInput48P& in,const SpringStaticType3CallbackSource48P& src,SpringRecordState48N s,SpringRecord48N* r,std::size_t n,SpringCallbackStatus48N expected){
        const auto old=s;auto result=sentinel;
        watchAllocations=true;const auto status=applySpringStaticType3Callback48P(in,src,r,n,s,result);watchAllocations=false;
        check(status==expected,"safety status");check(words(s)==words(old),"safety state unchanged");
        check(std::memcmp(records.data(),originalRecords.data(),sizeof(records))==0,"safety records unchanged");
        check(result.appended==sentinel.appended&&result.saturated==sentinel.saturated&&result.material==sentinel.material,"safety result unchanged");
    };
    using Status=SpringCallbackStatus48N;
    auto in=base;auto src=source;
    in.candidateCategoryA4=12;in.queryTypeTag=5;in.sceneName=nullptr;src.hitFlags=nullptr;
    reject(in,src,initial,nullptr,0,Status::Skipped);
    for(auto tag:{0u,1u,2u,5u,6u}){in=base;in.queryTypeTag=tag;reject(in,source,initial,records.data(),6,Status::UnsupportedScope);}
    in=base;in.candidateCategoryA4=6;reject(in,source,initial,records.data(),6,Status::UnsupportedScope);
    in=base;in.candidateIdentity=0;reject(in,source,initial,records.data(),6,Status::InvalidInput);
    in=base;in.sceneName=nullptr;reject(in,source,initial,records.data(),6,Status::InvalidInput);
    in=base;in.sceneName="fr7";in.sceneNameBytes=3;reject(in,source,initial,records.data(),6,Status::InvalidInput);
    in=base;in.sceneName="ab";in.sceneNameBytes=3;reject(in,source,initial,records.data(),6,Status::UnsupportedScope);
    in=base;in.bodyScenePresent=false;reject(in,source,initial,records.data(),6,Status::InvalidInput);
    in=base;in.bodyScene.v[12]=std::numeric_limits<float>::infinity();reject(in,source,initial,records.data(),6,Status::InvalidInput);
    reject(base,source,initial,nullptr,6,Status::InvalidInput);
    reject(base,source,initial,records.data(),4,Status::InsufficientCapacity);
    reject(base,source,initial,records.data(),std::numeric_limits<std::size_t>::max(),Status::InvalidInput);
    reject(base,source,initial,reinterpret_cast<SpringRecord48N*>(reinterpret_cast<char*>(records.data())+1),5,Status::InvalidInput);
    auto s=initial;s.generatedCount84=-1;reject(base,source,s,records.data(),6,Status::InvalidInput);
    s=initial;s.generatedCapacity88=-1;reject(base,source,s,records.data(),6,Status::InvalidInput);
    s=initial;s.pointCount7C=5;reject(base,source,s,records.data(),6,Status::InsufficientCapacity);
    src=source;src.lengthCount=2;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.hitFlagCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.worldStartCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.worldDirectionCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.normalCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.surfacePointCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    src=source;src.sceneMetadataCount=1;reject(base,src,initial,records.data(),6,Status::InsufficientCapacity);
    const auto limit=std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(3);
    src=source;src.hitFlags=reinterpret_cast<const std::uint32_t*>(limit);reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.worldStarts=reinterpret_cast<const SpringVector4_48N*>(limit);reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.lengthCount=std::numeric_limits<std::size_t>::max();reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.worldDirections=nullptr;reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.normals=reinterpret_cast<const SpringVector4_48N*>(reinterpret_cast<const char*>(normals.data())+1);reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.hitFlags=reinterpret_cast<const std::uint32_t*>(records.data());reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.worldStarts=reinterpret_cast<const SpringVector4_48N*>(records.data());reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.lengths=reinterpret_cast<const float*>(records.data());reject(base,src,initial,records.data(),6,Status::InvalidInput);
    src=source;src.sceneMetadata=reinterpret_cast<const std::uint32_t*>(records.data());reject(base,src,initial,records.data(),6,Status::InvalidInput);
    in=base;in.sceneName=reinterpret_cast<const char*>(records.data());reject(in,source,initial,records.data(),6,Status::InvalidInput);
    in=base;reject(in,source,initial,reinterpret_cast<SpringRecord48N*>(&in),6,Status::InvalidInput);
    src=source;reject(base,src,initial,reinterpret_cast<SpringRecord48N*>(&src),6,Status::InvalidInput);
    s=initial;auto result=sentinel;
    check(applySpringStaticType3Callback48P(base,source,reinterpret_cast<SpringRecord48N*>(&s),6,s,result)==Status::InvalidInput,"state/storage alias rejected");
    check(words(s)==words(initial),"aliased state unchanged");
    auto* resultAlias=static_cast<SpringStaticCallbackResult48N*>(static_cast<void*>(&s));
    check(applySpringStaticType3Callback48P(base,source,records.data(),6,s,*resultAlias)==Status::InvalidInput,"state/result alias rejected");
    check(words(s)==words(initial),"result alias state unchanged");
    auto old=starts[1];starts[1].v[0]=std::numeric_limits<float>::infinity();reject(base,source,initial,records.data(),6,Status::InvalidInput);starts[1]=old;
    old=directions[1];directions[1].v[0]=std::numeric_limits<float>::max();reject(base,source,initial,records.data(),6,Status::InvalidInput);directions[1]=old;
    const float savedLength=lengths[2];lengths[2]=std::numeric_limits<float>::infinity();reject(base,source,initial,records.data(),6,Status::InvalidInput);lengths[2]=savedLength;
    // Saturation does not excuse invalid arithmetic: original transforms first.
    s=initial;s.generatedCapacity88=0;old=starts[1];starts[1].v[1]=std::numeric_limits<float>::quiet_NaN();reject(base,source,s,records.data(),6,Status::InvalidInput);starts[1]=old;
    // No-hit ignores body and all non-hit spans; negative signed counts even
    // ignore hit storage, while preserving the material/current-candidate prefix.
    hits={0,0,0,0};src={};src.hitFlags=hits.data();src.hitFlagCount=4;
    in=base;in.bodyScenePresent=false;in.bodyScene.v[0]=std::numeric_limits<float>::quiet_NaN();
    s=initial;s.currentCandidate130=0;result=sentinel;
    check(applySpringStaticType3Callback48P(in,src,records.data(),6,s,result)==Status::Complete,"no-hit unused arrays/body ignored");
    check(s.currentCandidate130==17&&s.generatedCount84==1&&s.lastMaterialD4==21&&result.material==7&&result.appended==0,"no-hit prefix and stale material state");
    check(std::memcmp(records.data(),originalRecords.data(),sizeof(records))==0,"no-hit records untouched");
    src={};s=initial;s.pointCount7C=0xffffffffu;result=sentinel;
    check(applySpringStaticType3Callback48P(in,src,records.data(),6,s,result)==Status::Complete,"negative signed count ignores source");
    // A one-point callback still requires and consumes lengths0/1/2.
    hits={1,0xffffffffu,0,0};s=initial;s.pointCount7C=1;s.generatedCount84=0;result={};
    check(applySpringStaticType3Callback48P(base,source,records.data(),6,s,result)==Status::Complete,"one-point callback");
    check(records[0].field00.v[0]==2&&records[0].localPoint10.v[0]==3&&records[0].localPoint10.v[1]==1.5f&&records[0].localPoint10.v[2]==-3,"first-three length consumption");
    check(records[0].field00.v[3]==1&&records[0].localPoint10.v[3]==1&&records[0].normal20.v[3]==-7&&records[0].surfacePoint30.v[3]==42,"local and copied fourth lanes");
    s=initial;s.generatedCount84=0;s.currentCandidate130=0;result={};
    check(applySpringStaticType3Callback48P(base,source,records.data(),6,s,result)==Status::Complete,"first two-hit callback");
    check(records[0].scene40.value==17&&records[1].scene40.value==19,"raw metadata index i");
    in=base;in.candidateIdentity=31;in.sceneName="trackfr3";in.sceneNameBytes=9;
    check(applySpringStaticType3Callback48P(in,source,records.data(),6,s,result)==Status::Complete,"repeat callback");
    check(s.currentCandidate130==17&&s.generatedCount84==4&&s.lastMaterialD4==3,"static identity preserved and repeated hits reappend");
    check(applySpringStaticType3Callback48P(in,source,records.data(),6,s,result)==Status::Complete,"partially saturated callback");
    check(s.generatedCount84==5&&result.appended==1&&result.saturated==1,"logical saturation succeeds");
    check(std::memcmp(&records[5],&originalRecords[5],sizeof(records[5]))==0,"canary beyond logical capacity");
    constexpr std::size_t repetitions=100000;
    const auto begin=std::chrono::steady_clock::now();watchAllocations=true;
    for(std::size_t i=0;i<repetitions;++i){s=initial;s.generatedCount84=0;applySpringStaticType3Callback48P(base,source,records.data(),6,s,result);}
    watchAllocations=false;const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
    check(observedAllocations==0,"zero runtime allocations");
    std::cout<<"{\"cases\":"<<count<<",\"checks\":"<<checks<<",\"failures\":"<<failures<<",\"appended\":"<<totalAppended
        <<",\"saturated\":"<<totalSaturated<<",\"skipped\":"<<totalSkipped<<",\"runtime_allocations\":"<<observedAllocations
        <<",\"serial_repetitions\":"<<repetitions<<",\"serial_milliseconds\":"<<elapsed<<"}\n";
    return failures?1:0;
}
