#include "spring_type3_callback48p.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Range {std::uintptr_t first,last;};
bool overlap(Range a,Range b) noexcept {return a.first<b.last&&b.first<a.last;}
template<class T>bool range(const T* p,std::size_t n,Range& r) noexcept {
    r={0,0};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto start=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-start)return false;
    r={start,start+bytes};return true;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    SpringVector4_48N inverse(const BodyMatrix48G& m,const SpringVector4_48N& world) noexcept {
        float delta[3];
        for(unsigned axis=0;axis<3;++axis)delta[axis]=sub(world.v[axis],add(m.v[12+axis],0.f));
        SpringVector4_48N result{{0,0,0,1}};
        for(unsigned axis=0;axis<3;++axis){
            float value=mul(add(m.v[4*axis],0.f),delta[0]);
            value=add(value,mul(add(m.v[4*axis+1],0.f),delta[1]));
            result.v[axis]=add(value,mul(add(m.v[4*axis+2],0.f),delta[2]));
        }
        return result;
    }
};
bool transformed(const SpringStaticType3CallbackInput48P& input,const SpringStaticType3CallbackSource48P& source,
    std::size_t index,SpringRecordAppend48N& append) noexcept {
    Math math;
    SpringVector4_48N end{{0,0,0,1}};
    for(unsigned axis=0;axis<3;++axis){
        const float start=source.worldStarts[index].v[axis],direction=source.worldDirections[index].v[axis];
        if(!std::isfinite(start)||!std::isfinite(direction))return false;
        end.v[axis]=math.add(start,math.mul(direction,source.lengths[axis]));
    }
    // Original computes the reconstructed endpoint before the starting point.
    append.localPoint10=math.inverse(input.bodyScene,end);
    append.field00=math.inverse(input.bodyScene,source.worldStarts[index]);
    return math.valid;
}
}
SpringCallbackStatus48N applySpringStaticType3Callback48P(
    const SpringStaticType3CallbackInput48P& input,const SpringStaticType3CallbackSource48P& source,
    SpringRecord48N* records,std::size_t storageCapacity,SpringRecordState48N& state,
    SpringStaticCallbackResult48N& output) noexcept {
    using Status=SpringCallbackStatus48N;
    if(input.candidateCategoryA4&input.skipMask)return Status::Skipped;
    if(input.queryTypeTag!=3||(input.candidateCategoryA4&input.dynamicMask))return Status::UnsupportedScope;
    const bool parse=(input.candidateCategoryA4&input.materialMask)&&input.hasMaterialScene;
    Range writable[3]{},readable[10]{};
    if(!range(records,storageCapacity,writable[0])||!range(&state,1,writable[1])||!range(&output,1,writable[2])||
       !range(&input,1,readable[0])||!range(&source,1,readable[1])||
       (parse&&!range(input.sceneName,input.sceneNameBytes,readable[2])))return Status::InvalidInput;
    auto disjoint=[&]() noexcept {
        for(unsigned i=0;i<3;++i){
            for(unsigned j=0;j<i;++j)if(overlap(writable[i],writable[j]))return false;
            for(auto read:readable)if(overlap(writable[i],read))return false;
        }
        return true;
    };
    if(!disjoint())return Status::InvalidInput;
    // Reuse the native48N material/current-candidate prefix unchanged. This is
    // implementation reuse only; original fixtures execute the real type3 path.
    SpringStaticCallbackInput48N prefix{};
    prefix.candidateIdentity=input.candidateIdentity;prefix.candidateCategoryA4=input.candidateCategoryA4;
    prefix.currentCandidateCategoryA4=input.currentCandidateCategoryA4;
    prefix.skipMask=input.skipMask;prefix.dynamicMask=input.dynamicMask;prefix.materialMask=input.materialMask;
    prefix.hasMaterialScene=input.hasMaterialScene;prefix.sceneName=input.sceneName;prefix.sceneNameBytes=input.sceneNameBytes;
    auto stagedState=state;SpringStaticCallbackResult48N stagedResult{};
    const auto prefixStatus=applySpringStaticCallback48N(prefix,records,storageCapacity,stagedState,stagedResult);
    if(prefixStatus!=Status::Complete)return prefixStatus;
    // Original uses signed SLT. A negative point count produces no iterations.
    const auto count=(state.pointCount7C&0x80000000u)?std::size_t(0):std::size_t(state.pointCount7C);
    std::size_t highestHit=0;
    if(count){
        if(!range(source.hitFlags,source.hitFlagCount,readable[3]))return Status::InvalidInput;
        if(!disjoint())return Status::InvalidInput;
        if(source.hitFlagCount<count)return Status::InsufficientCapacity;
        for(std::size_t i=0;i<count;++i)if(source.hitFlags[i])highestHit=i+1;
    }
    if(highestHit){
        if(!input.bodyScenePresent)return Status::InvalidInput;
        if(!range(source.worldStarts,source.worldStartCount,readable[4])||
           !range(source.worldDirections,source.worldDirectionCount,readable[5])||
           !range(source.lengths,source.lengthCount,readable[6])||
           !range(source.normals,source.normalCount,readable[7])||
           !range(source.surfacePoints,source.surfacePointCount,readable[8])||
           !range(source.sceneMetadata,source.sceneMetadataCount,readable[9])||!disjoint())return Status::InvalidInput;
        if(source.worldStartCount<highestHit||source.worldDirectionCount<highestHit||source.lengthCount<3||
           source.normalCount<highestHit||source.surfacePointCount<highestHit||source.sceneMetadataCount<highestHit)
            return Status::InsufficientCapacity;
        for(unsigned axis=0;axis<3;++axis){
            if(!std::isfinite(source.lengths[axis]))return Status::InvalidInput;
            for(unsigned column=0;column<4;++column)if(!std::isfinite(input.bodyScene.v[column*4+axis]))return Status::InvalidInput;
        }
        for(std::size_t i=0;i<count;++i)if(source.hitFlags[i]){
            SpringRecordAppend48N append{};
            if(!transformed(input,source,i,append))return Status::InvalidInput;
        }
    }
    // Everything consumed is validated before the first visible mutation.
    state=stagedState;
    for(std::size_t i=0;i<count;++i)if(source.hitFlags[i]){
        SpringRecordAppend48N append{};
        (void)transformed(input,source,i,append);
        std::memcpy(&append.normal20,&source.normals[i],sizeof(append.normal20));
        std::memcpy(&append.surfacePoint30,&source.surfacePoints[i],sizeof(append.surfacePoint30));
        append.scene40={source.sceneMetadata[i]};append.other44={0};append.material4C=stagedResult.material;
        const auto status=appendSpringRecord48N(append,records,storageCapacity,state);
        if(status==SpringRecordStatus48N::Appended)++stagedResult.appended;
        else ++stagedResult.saturated;
    }
    output=stagedResult;
    return Status::Complete;
}
}
