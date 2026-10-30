#pragma once
#include <cstddef>
#include <cstdint>
#include <exception>
#include <array>
#include <memory>
#include <optional>
#include "original_selected_array_guards27ed40.h"
#include "original_search_block_guards27e3c0.h"
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
struct CollectionTree50BX;struct SearchStorage50BS;class OriginalApplicationEntities50LA;
namespace original_gc_gameplay_lighting_candidate {class Owner;}
//Owner-local lifetime extension of the SAME actual native List service. No
//second selected array, registry, geometry, public readiness or dispatch API.
class OriginalNativeListMethodCustody50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner; //Same World source Model membership custody, no public API.
    friend class original_gc_gameplay_lighting_candidate::Owner;
    const ::ra_pod_scene50LA::Owner* world_=nullptr;
    const OriginalApplicationEntities50LA* application_=nullptr;
    const CollectionTree50BX* method_=nullptr;
    const SearchStorage50BS* registry_=nullptr;
    const void* selectionOwner_=nullptr; //Exact owning SelectedMethod identity.
    std::size_t ordinal_=0;
    std::uint64_t reference00_=0;
    std::uint32_t usage10_=0;
    //Exclusive genuine source List prefix; old native selected geometry remains absent.
    struct SourceState {
        std::optional<std::uint32_t> vtable08,count18,growth20,capacity24,perBlock30,blockCount34,lastUsed38,field40;
        std::optional<std::uint8_t> type2C,type2D;
        std::unique_ptr<const void*[]> selected1C;
        std::optional<const void**> pointer1C;
        std::optional<const void*> pointer3C;
        bool referenceKnown=false,constructorReference=false,usageKnown=false,ownedBitKnown=false,ownedBit=false;
        bool selectedAllocationReturned=false,constructorReturned=false;
        selected_array_guards27ED40::History guards;
    } source_;
    struct SourceEntries {
        bool entered=false,tableAttempted=false,tablePublished=false,blockAttempted=false,blockPublished=false,ready=false;
        std::unique_ptr<std::uint32_t*[]> table;std::unique_ptr<std::uint32_t[]> words;
        search_block_guards27E3C0::History tableGuard,blockGuard;
        //Occupied source slots contain unknown numeric guest pointers. Native
        //identity is typed custody, not a fabricated token/address DWORD.
        std::array<bool,1024> wordKnown{};std::array<std::uint32_t,1024> nativeFreeNext{};
        std::array<const void*,1024> members{},borrowers{};
        const void* pending=nullptr;const void* pendingIdentity=nullptr;std::uint32_t pendingSlot=0;
        bool pendingConsumed=false,terminal=false;std::optional<std::uint32_t> abandonedConsumed;
    } sourceEntries_;
    bool sourceMode_=false;
    bool arrayReference_=false,nodeReference_=false,begun_=false,nodeAliasWasAcquired_=false;
public:
    OriginalNativeListMethodCustody50LA()noexcept=default;
    OriginalNativeListMethodCustody50LA(const OriginalNativeListMethodCustody50LA&)=delete;
    OriginalNativeListMethodCustody50LA& operator=(const OriginalNativeListMethodCustody50LA&)=delete;
    ~OriginalNativeListMethodCustody50LA(){if(reference00_)std::terminate();}
};
}
