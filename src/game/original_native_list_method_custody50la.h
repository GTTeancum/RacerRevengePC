#pragma once
#include <cstddef>
#include <cstdint>
#include <exception>
#include <array>
#include <memory>
#include <optional>
#include <vector>
#include "original_selected_array_guards27ed40.h"
#include "original_search_block_guards27e3c0.h"
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
struct CollectionTree50BX;struct SearchStorage50BS;class OriginalApplicationEntities50LA;
class OriginalSourcePhysicsCollision50LA;
namespace original_gc_gameplay_lighting_candidate {class Owner;}
//Owner-local lifetime extension of the SAME actual native List service. No
//second selected array, registry, geometry, public readiness or dispatch API.
class OriginalNativeListMethodCustody50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner; //Same World source Model membership custody, no public API.
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class OriginalSourcePhysicsCollision50LA; //Exact COL membership occurrence custody only.
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
    //A pin survives selection refusal and later native entry withdrawal until
    //the exact COL occurrence has retired its usable List pointer.
    std::vector<const void*> sourceCollisionRegistrationBorrowers_;
    bool sourceCollisionEntriesCurrentNative()const noexcept {
        //Opaque occurrence identities only: never dereference a COL borrower.
        for(std::size_t i=0;i<sourceCollisionRegistrationBorrowers_.size();++i){
            if(!sourceCollisionRegistrationBorrowers_[i])return false;
            for(std::size_t j=0;j<i;++j)if(sourceCollisionRegistrationBorrowers_[j]==sourceCollisionRegistrationBorrowers_[i])return false;
        }
        const auto& s=source_;const auto& e=sourceEntries_;
        if(!s.pointer3C||!s.blockCount34||!s.lastUsed38||!s.field40)return false;
        if(!search_block_guards27E3C0::valid(e.tableGuard)||!search_block_guards27E3C0::valid(e.blockGuard)||
           bool(e.pending)!=bool(e.pendingIdentity)||(e.pendingSlot&&!e.pending)||(e.pendingConsumed&&!e.pendingSlot)||
           (e.abandonedConsumed&&!e.terminal))return false;
        if(!e.entered){if(e.tableAttempted||e.blockAttempted||e.pending||e.table||e.words||*s.pointer3C||*s.blockCount34||
            *s.lastUsed38!=1024||*s.field40)return false;}
        if(e.tablePublished&&(!e.tableAttempted||!e.tableGuard.returned||*s.pointer3C!=e.table.get()))return false;
        if(!e.tablePublished&&*s.pointer3C)return false;
        if(e.blockAttempted&&!e.tablePublished)return false;
        if(e.blockPublished&&(!e.blockAttempted||!e.table||e.table[0]!=e.words.get()))return false;
        if(e.ready&&(!e.blockPublished||!e.blockGuard.returned))return false;
        if(!e.ready){if(*s.blockCount34||*s.lastUsed38!=1024||*s.field40||e.pendingSlot)return false;
            for(unsigned i=0;i<1024;++i)if(e.wordKnown[i]||e.members[i]||e.borrowers[i])return false;return true;}
        if(*s.blockCount34!=1||*s.lastUsed38>1024||*s.field40>1024||
           (e.abandonedConsumed&&(!*e.abandonedConsumed||*e.abandonedConsumed>*s.lastUsed38)))return false;
        std::array<bool,1024> freed{};auto next=*s.field40;
        while(next){if(next>*s.lastUsed38||freed[next-1]||e.wordKnown[next-1]||e.members[next-1]||e.nativeFreeNext[next-1]>1024)return false;freed[next-1]=true;next=e.nativeFreeNext[next-1];}
        for(unsigned i=0;i<1024;++i){if(i>=*s.lastUsed38){if(e.wordKnown[i]||e.members[i]||e.borrowers[i])return false;continue;}
            if(e.members[i]){if(!e.borrowers[i]||e.wordKnown[i]||freed[i])return false;}
            else if(e.borrowers[i]||(!freed[i]&&!(e.pendingConsumed&&e.pendingSlot==i+1)&&e.abandonedConsumed!=i+1))return false;}
        return !e.pendingSlot||(e.pendingConsumed?e.pendingSlot<=*s.lastUsed38:e.pendingSlot==(*s.field40?*s.field40:*s.lastUsed38+1));
    }
    bool sourceMode_=false;
    bool arrayReference_=false,nodeReference_=false,begun_=false,nodeAliasWasAcquired_=false;
public:
    OriginalNativeListMethodCustody50LA()noexcept=default;
    OriginalNativeListMethodCustody50LA(const OriginalNativeListMethodCustody50LA&)=delete;
    OriginalNativeListMethodCustody50LA& operator=(const OriginalNativeListMethodCustody50LA&)=delete;
    ~OriginalNativeListMethodCustody50LA(){if(reference00_||!sourceCollisionRegistrationBorrowers_.empty())std::terminate();}
};
}
