#pragma once
// Cold constructor/lifetime only. No zone assignment, members, observer or draw API.
#include "../../../src/game/original_application_entities50la.h"
#include "../../../src/game/search_cold50bu.h"
#include <memory>
#include <new>
#include <exception>
namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
enum class OriginalUiSearchListColdStatus50KG : std::uint8_t {
    complete,input,identity,alreadyStarted,allocation,registry,phase
};
struct OriginalUiSearchListColdDiagnostic50KG {
    OriginalUiSearchListColdStatus50KG status=OriginalUiSearchListColdStatus50KG::complete;
    std::uint32_t sourcePC=0;
    bool sourceEffectsCommitted=false;
};
class OriginalUiSearchListCold50KG final {
    friend struct ::original_main_entry50KG::Application;
    using D=OriginalUiSearchListColdDiagnostic50KG;
    using S=OriginalUiSearchListColdStatus50KG;
    const original_main_entry50KG::Application* const creator_;
    const std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    std::uint64_t reference00_=0;
    std::uint32_t usage10_=0,selectedCount18_=0,selectedGrowth20_=0,selectedCapacity24_=0;
    // Represent only the source-owned bit; untouched upper flags are not invented.
    bool selectedOwned28Bit0_=false;
    std::unique_ptr<const game::OriginalWreckComponent50LA*[]> selected1C_;
    std::uint8_t soultreeType2C_=0xffu,searchableType2D_=0xffu;
    bool soultreeKnown_=false,searchableKnown_=false;
    game::SearchStorage50BS storage_{}; // No block backing has been published.
    bool attempted_=false,derivedConstructed_=false,complete_=false,released_=false;
    OriginalUiSearchListCold50KG(const original_main_entry50KG::Application& creator,
        std::shared_ptr<game::OriginalApplicationEntities50LA> application)noexcept
        :creator_(&creator),application_(std::move(application)){}
    bool fail(D& d,S status,std::uint32_t pc)const noexcept {
        d={status,pc,attempted_};return false;
    }
    bool same(const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application)const noexcept {
        return creator==creator_&&application&&application==application_;
    }
    bool construct27E260(const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d)noexcept {
        d={};if(!same(creator,application))return fail(d,S::identity,0x27E260);
        if(attempted_||released_)return fail(d,S::alreadyStarted,0x27E260);
        if(!application_->application10())return fail(d,S::input,0x27E260);
        attempted_=true;
        reference00_=1; //27E9DC ->1048F0. SearchList is NOT Entity141300.
        //27ED10 initializes selected-array scalars and clears owned bit0.
        selectedCount18_=0;selectedGrowth20_=0;selectedCapacity24_=0;
        selectedOwned28Bit0_=false;
        usage10_=0; //27EA0C, before27ED40.
        //27ED40(30,30): cold fresh positive-capacity branch. Unused elements
        // remain unread/uninitialized; no synthetic selected Entity is stored.
        selected1C_.reset(new(std::nothrow) const game::OriginalWreckComponent50LA*[30]);
        selectedGrowth20_=30;selectedCapacity24_=30;selectedOwned28Bit0_=true;
        //27E9D0 ignores allocation return: actual registry history still runs.
        auto tag=application_->types383408().findOrAppend2A31E0("SoultreeObject");
        if(!tag.okay())return fail(d,S::registry,tag.sourcePC);
        soultreeType2C_=tag.type;soultreeKnown_=true;
        tag=application_->types383408().findOrAppend2A31E0("SearchableGameObject");
        if(!tag.okay())return fail(d,S::registry,tag.sourcePC);
        searchableType2D_=tag.type;searchableKnown_=true;
        //27E27C..27E2A0. No malloc block or pointer table exists until insertion.
        if(game::initializeColdSearchStorage50BU(storage_,0x400)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E288);
        derivedConstructed_=true;
        if(!selected1C_)return fail(d,S::allocation,0x27EE38);
        complete_=true;d={S::complete,0x27E2B0,true};return true;
    }
    bool releaseCold(const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d)noexcept {
        d={};if(!same(creator,application))return fail(d,S::identity,0x104970);
        if(released_)return true;
        if(!attempted_){released_=true;return true;}
        // No retain, member insertion, zone or observer API exists. Fail closed
        // if a future extension changes that invariant without joining teardown.
        if(reference00_!=1||usage10_||selectedCount18_||selectedGrowth20_!=30||selectedCapacity24_!=30||
           !selectedOwned28Bit0_||storage_.blockCount||storage_.freeHead||
           (derivedConstructed_?(storage_.perBlock!=0x400||storage_.lastUsed!=0x400):
                                (storage_.perBlock!=0||storage_.lastUsed!=0)))
            return fail(d,S::phase,0x27E2C0);
        --reference00_; //104970 last release dispatches actual derived/base teardown.
        //27E2C0's block loop/table free is absent for this real zero-block state.
        //27EA70 -> free selected array ->104910. No Entity counter decrement.
        selected1C_.reset();
        released_=true;complete_=false;d={S::complete,derivedConstructed_?0x27E384u:0x27EAE0u,true};return true;
    }
public:
    struct Snapshot {
        bool attempted=false,derivedConstructed=false,complete=false,released=false;
        std::uint64_t reference00=0;
        std::uint32_t usage10=0,selectedCount18=0,selectedGrowth20=0,selectedCapacity24=0;
        bool selectedOwned28Bit0=false,selectedBackingPresent=false,soultreeKnown=false,searchableKnown=false;
        std::uint8_t soultreeType2C=0xffu,searchableType2D=0xffu;
        std::uint32_t perBlock30=0,blockCount34=0,lastUsed38=0,freeHead40=0;
    };
    Snapshot snapshot()const noexcept {
        return {attempted_,derivedConstructed_,complete_,released_,reference00_,usage10_,
            selectedCount18_,selectedGrowth20_,selectedCapacity24_,selectedOwned28Bit0_,bool(selected1C_),
            soultreeKnown_,searchableKnown_,soultreeType2C_,searchableType2D_,
            storage_.perBlock,storage_.blockCount,storage_.lastUsed,storage_.freeHead};
    }
    ~OriginalUiSearchListCold50KG(){if(reference00_&&!released_)std::terminate();}
    OriginalUiSearchListCold50KG(const OriginalUiSearchListCold50KG&)=delete;
    OriginalUiSearchListCold50KG& operator=(const OriginalUiSearchListCold50KG&)=delete;
};
}
