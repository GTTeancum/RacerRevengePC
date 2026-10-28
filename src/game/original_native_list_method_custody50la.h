#pragma once
#include <cstddef>
#include <cstdint>
#include <exception>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
struct CollectionTree50BX;struct SearchStorage50BS;class OriginalApplicationEntities50LA;
namespace original_gc_gameplay_lighting_candidate {class Owner;}
//Owner-local lifetime extension of the SAME actual native List service. No
//second selected array, registry, geometry, public readiness or dispatch API.
class OriginalNativeListMethodCustody50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class original_gc_gameplay_lighting_candidate::Owner;
    const ::ra_pod_scene50LA::Owner* world_=nullptr;
    const OriginalApplicationEntities50LA* application_=nullptr;
    const CollectionTree50BX* method_=nullptr;
    const SearchStorage50BS* registry_=nullptr;
    const void* selectionOwner_=nullptr; //Exact owning SelectedMethod identity.
    std::size_t ordinal_=0;
    std::uint64_t reference00_=0;
    std::uint32_t usage10_=0;
    bool arrayReference_=false,nodeReference_=false,begun_=false,nodeAliasWasAcquired_=false;
public:
    OriginalNativeListMethodCustody50LA()noexcept=default;
    OriginalNativeListMethodCustody50LA(const OriginalNativeListMethodCustody50LA&)=delete;
    OriginalNativeListMethodCustody50LA& operator=(const OriginalNativeListMethodCustody50LA&)=delete;
    ~OriginalNativeListMethodCustody50LA(){if(reference00_)std::terminate();}
};
}
