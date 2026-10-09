#include "spring_detector_pipeline48q.h"
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=SpringDetectorPipelineStatus48Q;
struct Span {std::uintptr_t begin=0,end=0;};
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool span(const T* pointer,std::size_t count,Span& result) noexcept {
    result={};if(!count)return true;
    if(!pointer||reinterpret_cast<std::uintptr_t>(pointer)%alignof(T)||
       count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(pointer),bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    result={begin,begin+bytes};return true;
}
// Four detector spans in each of three buffers, eleven query spans in each of
// three buffers, one traversal stack and one whole result. This fixed storage
// is independent of supplied point/candidate/mesh counts.
struct Guard {
    Span writable[47]{};
    std::size_t count=0;
    template<class T>bool write(T* pointer,std::size_t amount) noexcept {
        Span next{};if(count==47||!span(pointer,amount,next))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(next,writable[i]))return false;
        writable[count++]=next;return true;
    }
    template<class T>bool read(const T* pointer,std::size_t amount) const noexcept {
        Span next{};if(!span(pointer,amount,next))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(next,writable[i]))return false;
        return true;
    }
    bool detectorWrites(const SpringDetectorBuffers48O& buffers) noexcept {
        return write(buffers.scenes,buffers.sceneCapacity)&&
            write(buffers.pairs,buffers.pairVectorCapacity)&&
            write(buffers.staged,buffers.stagedVectorCapacity)&&
            write(buffers.stagedMetadata,buffers.stagedMetadataCapacity);
    }
    bool rayWrites(const Type3RayBuffers48P& buffers) noexcept {
        return write(buffers.localStarts,buffers.capacity)&&write(buffers.localDirections,buffers.capacity)&&
            write(buffers.worldStarts,buffers.capacity)&&write(buffers.worldDirections,buffers.capacity)&&
            write(buffers.normals,buffers.capacity)&&write(buffers.points,buffers.capacity)&&
            write(buffers.hits,buffers.capacity)&&write(buffers.fractions,buffers.capacity)&&
            write(buffers.lengths,buffers.lengthCapacity);
    }
    bool queryWrites(const Type3QueryBuffers48P& buffers) noexcept {
        return rayWrites(buffers.rays)&&write(buffers.records,buffers.recordCapacity)&&
            write(buffers.effects,buffers.effectCapacity);
    }
    bool rayReads(const Type3RaySource48P& rays) const noexcept {
        return read(rays.localStarts,rays.capacity)&&read(rays.localDirections,rays.capacity)&&
            read(rays.worldStarts,rays.capacity)&&read(rays.worldDirections,rays.capacity)&&
            read(rays.normals,rays.capacity)&&read(rays.points,rays.capacity)&&
            read(rays.hits,rays.capacity)&&read(rays.fractions,rays.capacity)&&
            read(rays.lengths,rays.lengthCapacity);
    }
    bool filterReads(const CollisionFilter48J& filter) const noexcept {
        // Capacity, rather than size, protects unused allocated backing too.
        return read(filter.categoryMasks.data(),filter.categoryMasks.capacity())&&
            read(filter.identityLists[0].identities.data(),filter.identityLists[0].identities.capacity())&&
            read(filter.identityLists[1].identities.data(),filter.identityLists[1].identities.capacity());
    }
};
bool guarded(const SpringDetectorPipelineInput48Q& input,const SpringDetectorPipelineSource48Q& source,
    const Type3Candidate48P* candidates,std::size_t count,const SpringDetectorPipelineWorkspace48Q& workspace,
    const SpringDetectorPipelineBuffers48Q& destination,SpringDetectorPipelineResult48Q& output) noexcept {
    Guard guard;
    if(!guard.detectorWrites(workspace.detectorScratch)||!guard.detectorWrites(workspace.detectorPrepared)||
       !guard.detectorWrites(destination.detector)||!guard.queryWrites(workspace.queryScratch.buffers)||
       !guard.queryWrites(workspace.queryPrepared)||!guard.queryWrites(destination.query)||
       !guard.write(workspace.queryScratch.treeStack,workspace.queryScratch.treeStackCapacity)||
       !guard.write(&output,1))return false;
    const auto& detector=source.detector;
    if(!guard.read(&input,1)||!guard.read(&source,1)||!guard.read(&workspace,1)||
       !guard.read(&destination,1)||!guard.read(candidates,count)||
       !guard.read(detector.points,detector.pointCapacity)||!guard.read(detector.scenes,detector.sceneCount)||
       !guard.read(detector.pairs,detector.pairVectorCount)||!guard.read(detector.staged,detector.stagedVectorCount)||
       !guard.read(detector.stagedMetadata,detector.stagedMetadataCount)||!guard.rayReads(source.rays)||
       !guard.read(source.records,source.recordCount)||!guard.filterReads(input.query.filter))return false;
    for(std::size_t i=0;i<count;++i){
        const auto& candidate=candidates[i];
        if(!guard.filterReads(candidate.collision.filter)||
           !guard.read(candidate.sceneName,candidate.sceneNameBytes))return false;
        if(const auto* mesh=candidate.collision.geometry.mesh){
            if(!guard.read(mesh,1)||!guard.read(mesh->nodes.data(),mesh->nodes.capacity())||
               !guard.read(mesh->faces.data(),mesh->faces.capacity())||
               !guard.read(mesh->vertices.data(),mesh->vertices.capacity())||
               !guard.read(mesh->vertex_normals.data(),mesh->vertex_normals.capacity()))return false;
        }
    }
    return true;
}
template<class T>void copy(T* destination,const T* source,std::size_t count) noexcept {
    if(count)std::memcpy(destination,source,count*sizeof(T));
}
void copyDetector(const SpringDetectorBuffers48O& prepared,const SpringDetectorSource48O& source,
    const SpringDetectorBuffers48O& destination) noexcept {
    copy(destination.scenes,prepared.scenes,source.sceneCount);
    copy(destination.pairs,prepared.pairs,source.pairVectorCount);
    copy(destination.staged,prepared.staged,source.stagedVectorCount);
    copy(destination.stagedMetadata,prepared.stagedMetadata,source.stagedMetadataCount);
}
void copyRays(const Type3RaySource48P& source,const Type3RayBuffers48P& destination) noexcept {
    copy(destination.localStarts,source.localStarts,source.capacity);
    copy(destination.localDirections,source.localDirections,source.capacity);
    copy(destination.worldStarts,source.worldStarts,source.capacity);
    copy(destination.worldDirections,source.worldDirections,source.capacity);
    copy(destination.normals,source.normals,source.capacity);
    copy(destination.points,source.points,source.capacity);
    copy(destination.hits,source.hits,source.capacity);
    copy(destination.fractions,source.fractions,source.capacity);
    copy(destination.lengths,source.lengths,source.lengthCapacity);
}
Type3RaySource48P readRays(const Type3RayBuffers48P& buffers,std::size_t count,std::size_t lengths) noexcept {
    return {buffers.localStarts,buffers.localDirections,buffers.worldStarts,buffers.worldDirections,
        buffers.normals,buffers.points,buffers.hits,buffers.fractions,count,buffers.lengths,lengths};
}
Status queryStatus(Type3QueryStatus48P status) noexcept {
    switch(status){
    case Type3QueryStatus48P::Complete:return Status::Complete;
    case Type3QueryStatus48P::UnsupportedScope:return Status::UnsupportedScope;
    case Type3QueryStatus48P::UnsupportedInterpolationBoundary:return Status::UnsupportedInterpolationBoundary;
    case Type3QueryStatus48P::InsufficientCapacity:return Status::InsufficientCapacity;
    case Type3QueryStatus48P::InvalidInput:return Status::InvalidInput;
    }
    return Status::InvalidInput;
}
}
Status runSpringDetectorPipeline48Q(const SpringDetectorPipelineInput48Q& input,
    const SpringDetectorPipelineSource48Q& source,const Type3Candidate48P* candidates,std::size_t count,
    const SpringDetectorPipelineWorkspace48Q& workspace,const SpringDetectorPipelineBuffers48Q& destination,
    SpringDetectorPipelineResult48Q& output) noexcept {
    // Destination query backing is needed even on a prefix-only return, since
    // the unchanged source arrays are part of the whole-state projection.
    if(destination.detector.sceneCapacity<source.detector.sceneCount||
       destination.detector.pairVectorCapacity<source.detector.pairVectorCount||
       destination.detector.stagedVectorCapacity<source.detector.stagedVectorCount||
       destination.detector.stagedMetadataCapacity<source.detector.stagedMetadataCount||
       destination.query.rays.capacity<source.rays.capacity||
       destination.query.rays.lengthCapacity<source.rays.lengthCapacity||
       destination.query.recordCapacity<source.recordCount)return Status::InsufficientCapacity;
    if(!guarded(input,source,candidates,count,workspace,destination,output))return Status::InvalidInput;
    SpringDetectorPipelineResult48Q result{};
    const auto prepared=prepareSpringDetector48O(input.detector,source.detector,
        workspace.detectorScratch,workspace.detectorPrepared,result.detector);
    Status status=Status::InvalidInput;
    switch(prepared){
    case SpringDetectorStatus48O::NoSpring:status=Status::NoSpring;break;
    case SpringDetectorStatus48O::QueryDisabled:status=Status::QueryDisabled;break;
    case SpringDetectorStatus48O::RegistrationBoundary:status=Status::RegistrationBoundary;break;
    case SpringDetectorStatus48O::WorldQueryBoundary:status=Status::WorldQueryBoundary;break;
    case SpringDetectorStatus48O::LimitedQueryBoundary:status=Status::Complete;break;
    case SpringDetectorStatus48O::InvalidInput:return Status::InvalidInput;
    case SpringDetectorStatus48O::UnsupportedScope:return Status::UnsupportedScope;
    case SpringDetectorStatus48O::InsufficientCapacity:return Status::InsufficientCapacity;
    }
    result.query.state=input.query.initial;
    result.query.spring=result.detector.spring;
    result.query.currentCandidate=input.query.initialQueryCurrent;
    result.query.queryFlags=input.query.filter.flagsF8;
    if(status==Status::Complete){
        if(!input.detector.shapePresent)return Status::UnsupportedScope;
        if(destination.query.effectCapacity<count)return Status::InsufficientCapacity;
        Type3QueryContext48P context{};
        context.spring=result.detector.spring;
        context.pairCount=input.detector.shapePairCount;
        context.resetSpring=false;
        context.entry=Type3QueryEntry48P::LimitedList;
        context.bodyScene=input.query.bodyScene;
        context.bodyScenePresent=input.query.bodyScenePresent;
        if(input.bodySceneIndex!=-1){
            if(input.bodySceneIndex>=0&&static_cast<std::size_t>(input.bodySceneIndex)<source.detector.sceneCount)
                context.bodyScene=workspace.detectorPrepared.scenes[static_cast<std::size_t>(input.bodySceneIndex)].world;
            else context.bodyScenePresent=false;
        }
        const Type3QuerySource48P querySource{source.rays,workspace.detectorPrepared.staged,
            source.detector.stagedVectorCount,workspace.detectorPrepared.stagedMetadata,
            source.detector.stagedMetadataCount,source.records,source.recordCount};
        const auto queried=queryStaticType3WithContextBounded48P(input.query,context,querySource,candidates,count,
            workspace.queryScratch,workspace.queryPrepared,result.query);
        status=queryStatus(queried);if(status!=Status::Complete)return status;
        result.queryExecuted=true;
        result.detector.spring=result.query.spring;
    }
    result.detectorReturned=status==Status::Complete||status==Status::NoSpring||status==Status::QueryDisabled;
    result.detectorReturnValue=result.detectorReturned?1u:0u;
    // No fallible work follows: commit arrays and result as one transaction.
    copyDetector(workspace.detectorPrepared,source.detector,destination.detector);
    if(result.queryExecuted){
        copyRays(readRays(workspace.queryPrepared.rays,source.rays.capacity,source.rays.lengthCapacity),destination.query.rays);
        copy(destination.query.records,workspace.queryPrepared.records,source.recordCount);
        copy(destination.query.effects,workspace.queryPrepared.effects,count);
    }else{
        copyRays(source.rays,destination.query.rays);
        copy(destination.query.records,source.records,source.recordCount);
    }
    output=result;return status;
}
}
