#include "zone_lifecycle50cl.h"
#include <limits>

namespace rr::game { namespace {
using Status=SearchStorageStatus50BS;
struct Span { std::uintptr_t begin=0,end=0; };

template<class T> bool span(const T* pointer,std::size_t count,Span& out) noexcept {
    out={};
    if(!count) return true;
    const auto address=reinterpret_cast<std::uintptr_t>(pointer);
    if(!pointer||address%alignof(T)||
       count>std::numeric_limits<std::size_t>::max()/sizeof(T)) return false;
    const auto bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-address) return false;
    out={address,address+bytes};
    return true;
}
bool overlaps(Span a,Span b) noexcept {
    return a.begin<b.end&&b.begin<a.end;
}

// Check full capacities, including inactive tails, before touching scratch or
// publishing any state. Read-only descriptions may overlap each other.
bool validLayout(const ZoneLifecycle50CL& state,
    const ZoneLifecycleStorage50CL& storage,std::size_t zones,std::size_t objects,
    const ZoneRegistrationScratch50CJ& scratch) noexcept {
    if(zones>std::numeric_limits<CollectionRef50BX>::max()/2u||
       zones>std::numeric_limits<std::size_t>::max()/4u||
       objects>(std::numeric_limits<CollectionRef50BX>::max()>>2u)||
       zones*2u>storage.registryCapacity||
       objects>storage.registrationCapacity||
       zones*2u>storage.views.methodCapacity||
       zones>storage.views.zoneCapacity||
       zones*4u>storage.views.blockCapacity) return false;
    Span backing[5]{},stateSpan{},description{},temporary{};
    if(!span(storage.registries,storage.registryCapacity,backing[0])||
       !span(storage.registrations,storage.registrationCapacity,backing[1])||
       !span(storage.views.methods,storage.views.methodCapacity,backing[2])||
       !span(storage.views.zones,storage.views.zoneCapacity,backing[3])||
       !span(storage.views.blocks,storage.views.blockCapacity,backing[4])||
       !span(&state,1,stateSpan)||!span(&storage,1,description)||
       !span(&scratch,1,temporary)) return false;
    if(overlaps(temporary,stateSpan)||overlaps(temporary,description)) return false;
    for(unsigned i=0;i<5;++i) {
        if(overlaps(backing[i],stateSpan)||overlaps(backing[i],description)||
           overlaps(backing[i],temporary)) return false;
        for(unsigned j=0;j<i;++j)
            if(overlaps(backing[i],backing[j])) return false;
    }
    return true;
}

bool registryIndex(const ZoneLifecycle50CL& state,const SearchStorage50BS* owner,
    std::size_t& index) noexcept {
    const auto first=reinterpret_cast<std::uintptr_t>(state.storage.registries);
    const auto address=reinterpret_cast<std::uintptr_t>(owner);
    if(!owner||address<first) return false;
    const auto offset=address-first;
    if(offset%sizeof(SearchStorage50BS)) return false;
    index=offset/sizeof(SearchStorage50BS);
    return index<state.zoneCount*2u;
}

// Only states produced by this fresh-owner adapter are admitted. 50CJ performs
// complete source/destination list validation before its transactional mutation.
bool publishedMethodMatches(const ZoneLifecycle50CL& state,
    std::size_t index) noexcept {
    const auto& storage=state.storage;
    const auto& registry=storage.registries[index];
    const auto& method=storage.views.methods[index];
    const auto& list=method.searchList;
    const auto& zone=storage.views.zones[index/2u];
    if(registry.perBlock!=state.perBlock||registry.blockCount>1u||
       registry.lastUsed>state.perBlock||
       (!registry.blockCount&&registry.lastUsed!=state.perBlock)||
       method.method!=CollectionTree50BX::Method::SearchList||
       zone.firstMethod!=static_cast<CollectionRef50BX>((index/2u)*2u+1u)||
       zone.secondMethod!=static_cast<CollectionRef50BX>((index/2u)*2u+2u)||
       list.blockCount!=static_cast<std::int32_t>(registry.blockCount)||
       list.perBlock30!=static_cast<std::int32_t>(registry.perBlock)||
       list.lastUsed38!=static_cast<std::int32_t>(registry.lastUsed)) return false;
    if(!registry.blockCount) return !list.blocks&&!list.blockCapacity;
    const auto* blocks=storage.views.blocks+index*2u;
    return list.blocks==blocks&&list.blockCapacity==2u&&
        blocks[0].words==registry.words&&blocks[0].capacity==registry.perBlock&&
        !blocks[1].words&&!blocks[1].capacity;
}

// Infallible publication after 50CJ has succeeded. Fixed two-descriptor slices
// avoid relocating other methods when a cold method receives its first object.
// This is the same first-block projection as 50BT/50CG; the terminal descriptor
// is required by the original SearchList iterator's terminal pointer read.
void refreshMethod(ZoneLifecycle50CL& state,std::size_t index) noexcept {
    const auto& registry=state.storage.registries[index];
    CollectionTree50BX method{};
    method.method=CollectionTree50BX::Method::SearchList;
    method.searchList={nullptr,0,static_cast<std::int32_t>(registry.blockCount),
        static_cast<std::int32_t>(registry.perBlock),
        static_cast<std::int32_t>(registry.lastUsed)};
    if(registry.blockCount) {
        auto* blocks=state.storage.views.blocks+index*2u;
        blocks[0]={registry.words,registry.perBlock};
        blocks[1]={};
        method.searchList.blocks=blocks;
        method.searchList.blockCapacity=2;
    }
    state.storage.views.methods[index]=method;
}

Status changeRegistration(ZoneLifecycle50CL& state,CollectionRef50BX object,
    SearchStorage50BS* destination,ZoneRegistrationScratch50CJ& scratch) noexcept {
    auto& record=state.storage.registrations[object-1u];
    std::size_t previousIndex=0,nextIndex=0;
    const bool hadPrevious=record.owner!=nullptr;
    if(hadPrevious&&(!registryIndex(state,record.owner,previousIndex)||
       !publishedMethodMatches(state,previousIndex))) return Status::InvalidTopology;
    if(destination&&(!registryIndex(state,destination,nextIndex)||
       !publishedMethodMatches(state,nextIndex))) return Status::InvalidTopology;

    // No fallible operation follows this call. Scratch and all backing spans
    // were checked before entry, and both resulting registries remain bounded
    // to one block, with their two descriptor slots already reserved.
    const auto status=movePreparedRegistration50CJ(record,destination,object<<2u,scratch);
    if(status!=Status::Complete) return status;
    if(hadPrevious) refreshMethod(state,previousIndex);
    if(destination&&(!hadPrevious||nextIndex!=previousIndex))
        refreshMethod(state,nextIndex);
    return Status::Complete;
}

bool validState(const ZoneLifecycle50CL& state,CollectionRef50BX object,
    const ZoneRegistrationScratch50CJ& scratch) noexcept {
    return state.perBlock&&state.perBlock<=1024u&&object&&
        object<=(std::numeric_limits<CollectionRef50BX>::max()>>2u)&&
        object<=state.objectCount&&
        validLayout(state,state.storage,state.zoneCount,state.objectCount,scratch);
}
} // namespace

SearchStorageStatus50BS initializeZoneLifecycle50CL(ZoneLifecycle50CL& state,
    const ZoneLifecycleStorage50CL& storage,std::size_t zoneCount,
    std::size_t objectCount,std::uint32_t perBlock,
    ZoneRegistrationScratch50CJ& scratch) noexcept {
    if(state.perBlock||!perBlock||perBlock>1024u||
       !validLayout(state,storage,zoneCount,objectCount,scratch))
        return Status::InvalidInput;

    // Prepare the cold view through the existing adapter before publishing.
    // Caller scratch provides the only temporary SearchStorage objects.
    auto status=initializeColdSearchStorage50BU(scratch.previous,perBlock);
    if(status!=Status::Complete) return status;
    status=initializeColdSearchStorage50BU(scratch.next,perBlock);
    if(status!=Status::Complete) return status;
    const ZoneSearchBinding50CG binding{&scratch.previous,&scratch.next};
    CollectionTree50BX methods[2]{};
    CollectionZone50BX zone{};
    const ZoneSearchViews50CG prepared{methods,2,&zone,1,nullptr,0};
    status=prepareZoneSearchViews50CG(&binding,1,prepared);
    if(status!=Status::Complete) return status;

    // Parameters and all outputs are validated. 50BU cannot fail with this
    // perBlock; only its four headers are initialized, not opaque backing words.
    const ZoneLifecycle50CL published{storage,zoneCount,objectCount,perBlock};
    for(std::size_t i=0;i<zoneCount*2u;++i) {
        initializeColdSearchStorage50BU(storage.registries[i],perBlock);
        storage.views.methods[i]=methods[i&1u];
    }
    for(std::size_t i=0;i<zoneCount;++i)
        storage.views.zones[i]={static_cast<CollectionRef50BX>(i*2u+1u),
            static_cast<CollectionRef50BX>(i*2u+2u)};
    for(std::size_t i=0;i<objectCount;++i) storage.registrations[i]={};
    state=published;
    return Status::Complete;
}

SearchStorageStatus50BS updateZoneRegistration50CL(ZoneLifecycle50CL& state,
    CollectionRef50BX objectRef,CollectionRef50BX zoneRef,ZoneMethod50CL method,
    ZoneRegistrationScratch50CJ& scratch) noexcept {
    if(!validState(state,objectRef,scratch)||!zoneRef||zoneRef>state.zoneCount||
       (method!=ZoneMethod50CL::First&&method!=ZoneMethod50CL::Second))
        return Status::InvalidInput;
    const auto index=(static_cast<std::size_t>(zoneRef)-1u)*2u+
        static_cast<unsigned>(method);
    return changeRegistration(state,objectRef,state.storage.registries+index,scratch);
}

SearchStorageStatus50BS removeZoneRegistration50CL(ZoneLifecycle50CL& state,
    CollectionRef50BX objectRef,ZoneRegistrationScratch50CJ& scratch) noexcept {
    if(!validState(state,objectRef,scratch)) return Status::InvalidInput;
    return changeRegistration(state,objectRef,nullptr,scratch);
}
} // namespace rr::game
