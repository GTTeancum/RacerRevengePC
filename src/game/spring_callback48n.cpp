#include "spring_callback48n.h"
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Range { std::uintptr_t begin,end; };
bool range(const void* p,std::size_t n,std::size_t size,Range& r) noexcept {
    r={0,0};if(!n)return true;
    if(!p||n>std::numeric_limits<std::size_t>::max()/size)return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*size;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;
    r={a,a+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}
}
SpringCallbackStatus48N applySpringStaticCallback48N(const SpringStaticCallbackInput48N& input,
    SpringRecord48N* records,std::size_t storageCapacity,SpringRecordState48N& state,
    SpringStaticCallbackResult48N& output) noexcept {
    // Original294BE4 returns even before loading the spring and query objects.
    if(input.candidateCategoryA4&input.skipMask)return SpringCallbackStatus48N::Skipped;
    if((input.candidateCategoryA4&input.dynamicMask)||input.queryTypeTag!=2)
        return SpringCallbackStatus48N::UnsupportedScope;
    const bool parse=(input.candidateCategoryA4&input.materialMask)&&input.hasMaterialScene;
    Range data{},meta{},result{},descriptor{},pairs{},name{};
    if(!range(records,storageCapacity,sizeof(*records),data)||!range(&state,1,sizeof(state),meta)||
       !range(&output,1,sizeof(output),result)||!range(&input,1,sizeof(input),descriptor)||
       !range(input.pairs,input.pairCount,sizeof(*input.pairs),pairs)||
       (parse&&!range(input.sceneName,input.sceneNameBytes,1,name))||
       (storageCapacity&&reinterpret_cast<std::uintptr_t>(records)%alignof(SpringRecord48N))||
       (input.pairCount&&reinterpret_cast<std::uintptr_t>(input.pairs)%alignof(SpringCumulativePair48N)))
        return SpringCallbackStatus48N::InvalidInput;
    const Range writable[]={data,meta,result};const Range readable[]={descriptor,pairs,name};
    for(unsigned i=0;i<3;++i){
        for(unsigned j=0;j<i;++j)if(overlap(writable[i],writable[j]))return SpringCallbackStatus48N::InvalidInput;
        for(const auto r:readable)if(overlap(writable[i],r))return SpringCallbackStatus48N::InvalidInput;
    }
    if(!input.candidateIdentity||input.pairCount>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
       state.generatedCount84<0||state.generatedCapacity88<0)return SpringCallbackStatus48N::InvalidInput;
    if(storageCapacity<static_cast<std::size_t>(state.generatedCapacity88))return SpringCallbackStatus48N::InsufficientCapacity;
    std::uint32_t material=0;
    if(parse){
        std::size_t delimiter=0;
        while(delimiter<input.sceneNameBytes&&input.sceneName[delimiter]!=0&&input.sceneName[delimiter]!='.')++delimiter;
        if(delimiter==input.sceneNameBytes)return SpringCallbackStatus48N::InvalidInput;
        if(delimiter<3)return SpringCallbackStatus48N::UnsupportedScope;
        if(input.sceneName[delimiter-3]=='f'&&input.sceneName[delimiter-2]=='r'){
            const auto byte=static_cast<unsigned char>(input.sceneName[delimiter-1]);
            const int signedByte=byte<128?int(byte):int(byte)-256;
            material=static_cast<std::uint32_t>(signedByte-48);
        }
    }
    // Preflight above makes every following append either Appended or Saturated.
    // No operation after this point can fail or require dynamic storage.
    if(!state.currentCandidate130||(input.currentCandidateCategoryA4&input.dynamicMask))
        state.currentCandidate130=input.candidateIdentity;
    SpringStaticCallbackResult48N resultValue{};resultValue.material=material;
    for(std::size_t i=0;i<input.pairCount;++i){
        SpringRecordAppend48N append{};
        std::memcpy(&append.field00,&input.pairs[i].first,sizeof(SpringVector4_48N));
        std::memcpy(&append.localPoint10,&input.pairs[i].second,sizeof(SpringVector4_48N));
        std::memcpy(&append.normal20,&input.pairs[i].normal,sizeof(SpringVector4_48N));
        std::memcpy(&append.surfacePoint30,&input.pairs[i].surfacePoint,sizeof(SpringVector4_48N));
        append.scene40=input.queryScene;append.other44={0};append.material4C=material;
        const auto status=appendSpringRecord48N(append,records,storageCapacity,state);
        if(status==SpringRecordStatus48N::Appended)++resultValue.appended;
        else ++resultValue.saturated;
    }
    output=resultValue;
    return SpringCallbackStatus48N::Complete;
}
}
