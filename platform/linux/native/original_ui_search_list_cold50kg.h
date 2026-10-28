#pragma once
// Cold constructor plus private exact manager Camera membership. No public member or draw API.
#include "../../../src/game/original_application_entities50la.h"
#include "../../../src/game/search_cold50bu.h"
#include <memory>
#include <array>
#include <new>
#include <exception>
namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
class OriginalUiGarageManagerSearch50KG;
class OriginalUiLightManagerOwner50KG;
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
    friend class OriginalUiGarageManagerSearch50KG;
    friend class OriginalUiLightManagerOwner50KG;
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
    // Native bounded backing, owned by this List. Source block publication is
    // still lazy; the native word array replaces malloc/realloc transport, not
    // the source used/free-list state. No capacity is increased.
    game::SearchStorage50BS storage_{};
    const std::uint32_t* managerBlockPointers_[1]{};
    const std::uint32_t* const* managerTable3C_=nullptr;
    const game::OriginalWreckComponent50LA* managerCamera_=nullptr;
    //Same existing List storage30/34/38/3C, private typed RA Light identity.
    //Exact authored Light cohort typed per-slot borrows; no second registry, selected array or collider.
    std::array<const game::OriginalWreckComponent50LA*,1024> lightMembers_{};
    std::array<std::uint32_t,1024> lightTokens_{};
    bool lightMembersEmpty()const noexcept { for(auto* p:lightMembers_)if(p)return false;return true; }
    bool lightMemberAt(const game::OriginalWreckComponent50LA* p,std::uint32_t slot)const noexcept {
        return slot&&slot<=storage_.lastUsed&&lightMembers_[slot-1]==p;
    }
    const std::uint32_t* lightBlockPointers_[1]{};
    const std::uint32_t* const* lightTable3C_=nullptr;
    // Token4 is a private native identity for the one typed Camera binding,
    // never a truncated guest/host pointer. It is not exposed as a guest address.
    static constexpr std::uint32_t cameraToken_=4;

    bool attempted_=false,derivedConstructed_=false,complete_=false,released_=false;
    // Manager2A0 is ONE list, aliased by the same node's34/38. This native
    // authority is distinct from the four independent Scene search lists.
    const OriginalUiGarageManagerSearch50KG* managerOwner_=nullptr;
    std::uint8_t managerAliases_=0,managerReleasedAliases_=0;
    const OriginalUiLightManagerOwner50KG* lightManagerOwner_=nullptr;
    std::uint8_t lightAliases_=0,lightReleasedAliases_=0;
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
        if(managerOwner_||lightManagerOwner_)return fail(d,S::phase,0x104970);
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
    bool managerHistoryCurrent(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application)const noexcept {
        if(!(owner&&owner==managerOwner_&&!lightManagerOwner_&&same(creator,application)&&attempted_&&!released_&&
            managerAliases_<=2&&managerReleasedAliases_<=managerAliases_&&
            reference00_==1u+managerAliases_-managerReleasedAliases_&&usage10_==managerAliases_&&
            !selectedCount18_&&selectedGrowth20_==30&&selectedCapacity24_==30&&selectedOwned28Bit0_))return false;
        if(!storage_.blockCount)return !managerTable3C_&&!managerBlockPointers_[0]&&!managerCamera_&&!storage_.freeHead&&
            (derivedConstructed_?(storage_.perBlock==0x400&&storage_.lastUsed==0x400):
                                 (!storage_.perBlock&&!storage_.lastUsed));
        if(!complete_||!derivedConstructed_||!selected1C_||storage_.perBlock!=0x400||
           storage_.blockCount!=1||storage_.lastUsed!=1||managerTable3C_!=managerBlockPointers_||managerBlockPointers_[0]!=storage_.words)return false;
        return managerCamera_?(!storage_.freeHead&&storage_.words[0]==cameraToken_):
            (storage_.freeHead==1&&storage_.words[0]==2);
    }
    bool managerEmptyCurrent(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application)const noexcept {
        return managerHistoryCurrent(owner,creator,application)&&!storage_.blockCount;
    }
    bool insertManagerCamera(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t& slot,D& d)noexcept {
        d={};if(!managerHistoryCurrent(owner,creator,application)||!complete_||managerCamera_||
            managerAliases_!=2||managerReleasedAliases_||reference00_!=3||usage10_!=2)
            return fail(d,S::phase,0x27E3B0);
        auto next=storage_;std::uint32_t selected=0;
        auto result=next.blockCount?game::selectSearchSlot50BS(next,0,selected):
            game::adoptFirstSearchBlock50BU(next,selected);
        if(result!=game::SearchStorageStatus50BS::Complete||selected!=1||
           game::insertSearchSlot50BS(next,selected,cameraToken_)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E578);
        // Publish only the used words. Unused native backing never supplies
        // source-known values or fake members. A removed first slot is reused.
        storage_.blockCount=next.blockCount;storage_.lastUsed=next.lastUsed;
        storage_.freeHead=next.freeHead;storage_.words[0]=next.words[0];
        managerBlockPointers_[0]=storage_.words;managerTable3C_=managerBlockPointers_;
        managerCamera_=&camera;slot=selected;
        d={S::complete,0x27E5A8,true};return true;
    }
    bool managerCameraCurrent(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t slot)const noexcept {
        return managerHistoryCurrent(owner,creator,application)&&managerCamera_==&camera&&slot==1&&
            managerAliases_==2&&!managerReleasedAliases_&&reference00_==3;
    }
    bool removeManagerCamera(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t slot,D& d)noexcept {
        d={};if(!managerCameraCurrent(owner,creator,application,camera,slot))return fail(d,S::identity,0x27E5B0);
        auto next=storage_;
        if(game::removeSearchSlot50BS(next,slot,cameraToken_)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E5B0);
        storage_.freeHead=next.freeHead;storage_.words[0]=next.words[0];managerCamera_=nullptr;
        // Removal does NOT deallocate its source block, reset lastUsed or
        // decrease usage10. That history belongs to the later List destructor.
        d={S::complete,0x27E5B0,true};return true;
    }
    bool assignManagerAlias(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        std::uint8_t ordinal,OriginalUiSearchListCold50KG*& actualField,D& d)noexcept {
        d={};const auto pc=ordinal==1?0x27F420u:0x27F490u;
        if(!managerEmptyCurrent(owner,creator,application)||!complete_||!selected1C_||
           managerReleasedAliases_||ordinal!=managerAliases_+1||ordinal>2||actualField)
            return fail(d,S::phase,pc);
        // Fresh old field is genuinely null. Preserve source setter order:
        // incoming usage, actual node pointer, then104960 retain.
        usage10_=ordinal;actualField=this;++reference00_;++managerAliases_;
        d={S::complete,ordinal==1?0x27F468u:0x27F4D8u,true};return true;
    }
    bool releaseManagerAlias(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        std::uint8_t ordinal,const OriginalUiSearchListCold50KG* actualField,D& d)noexcept {
        d={};const auto pc=ordinal==1?0x27F200u:0x27F21Cu;
        if(!managerHistoryCurrent(owner,creator,application)||managerCamera_||actualField!=this||
           ordinal!=managerReleasedAliases_+1||ordinal>managerAliases_||reference00_<2)
            return fail(d,S::phase,pc);
        --reference00_;++managerReleasedAliases_; // No usage or dying-field null write.
        d={S::complete,pc,true};return true;
    }
    bool releaseManagerPayload(const OriginalUiGarageManagerSearch50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d)noexcept {
        d={};if(!managerHistoryCurrent(owner,creator,application)||managerCamera_||reference00_!=1||
            managerAliases_!=managerReleasedAliases_)return fail(d,S::phase,0x1CCADC);
        --reference00_;
        //27E2C0 retires the real bounded block/table publication, including
        // removed-Camera history; backing lives inside this native List.
        managerTable3C_=nullptr;managerBlockPointers_[0]=nullptr;selected1C_.reset();released_=true;complete_=false;
        //27EA70 frees the selected array. No Entity count contribution.
        d={S::complete,0x1CCADC,true};return true;
    }
    bool lightEmptyCurrent(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application)const noexcept {
        return owner&&owner==lightManagerOwner_&&!managerOwner_&&same(creator,application)&&attempted_&&!released_&&
            lightAliases_<=2&&lightReleasedAliases_<=lightAliases_&&
            reference00_==1u+lightAliases_-lightReleasedAliases_&&usage10_==lightAliases_&&
            !selectedCount18_&&selectedGrowth20_==30&&selectedCapacity24_==30&&selectedOwned28Bit0_&&
            !storage_.blockCount&&!storage_.freeHead&&
            (derivedConstructed_?(storage_.perBlock==0x400&&storage_.lastUsed==0x400):
                                 (!storage_.perBlock&&!storage_.lastUsed));
    }
    bool lightHistoryCurrent(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app)const noexcept {
        if(!storage_.blockCount)return lightEmptyCurrent(owner,creator,app)&&lightMembersEmpty()&&
            !lightTable3C_&&!lightBlockPointers_[0];
        if(!owner||owner!=lightManagerOwner_||managerOwner_||!same(creator,app)||!attempted_||released_||
           !complete_||!derivedConstructed_||!selected1C_||selectedCount18_||selectedGrowth20_!=30u||
           selectedCapacity24_!=30u||!selectedOwned28Bit0_||lightAliases_>2u||lightReleasedAliases_>lightAliases_||
           reference00_!=1u+lightAliases_-lightReleasedAliases_||usage10_!=lightAliases_||
           storage_.perBlock!=1024u||storage_.blockCount!=1u||!storage_.lastUsed||storage_.lastUsed>1024u||
           lightTable3C_!=lightBlockPointers_||lightBlockPointers_[0]!=storage_.words)return false;
        std::array<bool,1024> freed{};auto next=storage_.freeHead;
        while(next){if(next>storage_.lastUsed||freed[next-1]||lightMembers_[next-1]||
            (storage_.words[next-1]&3u)!=2u)return false;
            freed[next-1]=true;next=storage_.words[next-1]>>2;}
        for(std::size_t i=0;i<1024u;++i){
            if(i>=storage_.lastUsed){if(lightMembers_[i]||lightTokens_[i])return false;continue;}
            if(lightMembers_[i]){if(freed[i]||!lightTokens_[i]||(lightTokens_[i]&3u)||storage_.words[i]!=lightTokens_[i])return false;}
            else if(!freed[i])return false;
        }
        return true;
    }
    bool insertRaFirstLight(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& light,std::uint32_t token,std::uint32_t& slot,D& d)noexcept {
        d={};if(!lightHistoryCurrent(owner,creator,app)||lightAliases_!=2u||
           lightReleasedAliases_||reference00_!=3u||token<12u||token>740u||(token&3u))return fail(d,S::phase,0x27e3b0);
        auto next=storage_;std::uint32_t selected=0;
        const auto result=next.blockCount?game::selectSearchSlot50BS(next,0,selected):
            game::adoptFirstSearchBlock50BU(next,selected);
        if(result!=game::SearchStorageStatus50BS::Complete||!selected||selected>1024u||
           game::insertSearchSlot50BS(next,selected,token)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27e570);
        // Token4*(source ordinal+1) names the same actual Light+30 facet;
        // native typed per-slot borrows preserve duplicates, not a second List.
        storage_=next;lightBlockPointers_[0]=storage_.words;lightTable3C_=lightBlockPointers_;
        lightMembers_[selected-1]=&light;lightTokens_[selected-1]=token;
        slot=selected;d={S::complete,0x27e5a8,true};return true;
    }
    bool removeRaFirstLight(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& light,std::uint32_t slot,D& d)noexcept {
        d={};if(!lightHistoryCurrent(owner,creator,app)||!lightMemberAt(&light,slot))return fail(d,S::identity,0x27e5b0);
        if(game::removeSearchSlot50BS(storage_,slot,lightTokens_[slot-1])!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27e5b0);
        lightMembers_[slot-1]=nullptr;d={S::complete,0x27e5b0,true};return true;
    }
    bool assignLightAlias(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        std::uint8_t ordinal,OriginalUiSearchListCold50KG*& field,D& d)noexcept {
        d={};if(!lightEmptyCurrent(owner,creator,application)||!complete_||!selected1C_||
            lightReleasedAliases_||ordinal!=lightAliases_+1||ordinal>2||field)return fail(d,S::phase,0x27F420);
        usage10_=ordinal;field=this;++reference00_;++lightAliases_;
        d={S::complete,ordinal==1?0x27F468u:0x27F4D8u,true};return true;
    }
    bool releaseLightAlias(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        std::uint8_t ordinal,const OriginalUiSearchListCold50KG* field,D& d)noexcept {
        d={};if(!lightHistoryCurrent(owner,creator,application)||!lightMembersEmpty()||field!=this||ordinal!=lightReleasedAliases_+1||
            ordinal>lightAliases_||reference00_<2)return fail(d,S::phase,0x27F200);
        --reference00_;++lightReleasedAliases_;d={S::complete,ordinal==1?0x27F200u:0x27F21Cu,true};return true;
    }
    bool releaseLightPayload(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d)noexcept {
        d={};if(!lightHistoryCurrent(owner,creator,application)||!lightMembersEmpty()||reference00_!=1||
            lightAliases_!=lightReleasedAliases_)return fail(d,S::phase,0x1481A8);
        --reference00_;lightTable3C_=nullptr;lightBlockPointers_[0]=nullptr;selected1C_.reset();released_=true;complete_=false;d={S::complete,0x1481A8,true};return true;
    }
public:
    struct Snapshot {
        bool attempted=false,derivedConstructed=false,complete=false,released=false;
        std::uint64_t reference00=0;
        std::uint32_t usage10=0,selectedCount18=0,selectedGrowth20=0,selectedCapacity24=0;
        bool selectedOwned28Bit0=false,selectedBackingPresent=false,soultreeKnown=false,searchableKnown=false;
        std::uint8_t soultreeType2C=0xffu,searchableType2D=0xffu;
        std::uint32_t perBlock30=0,blockCount34=0,lastUsed38=0,freeHead40=0;
        bool managerBlockPublished=false,cameraMemberLive=false;
    };
    Snapshot snapshot()const noexcept {
        return {attempted_,derivedConstructed_,complete_,released_,reference00_,usage10_,
            selectedCount18_,selectedGrowth20_,selectedCapacity24_,selectedOwned28Bit0_,bool(selected1C_),
            soultreeKnown_,searchableKnown_,soultreeType2C_,searchableType2D_,
            storage_.perBlock,storage_.blockCount,storage_.lastUsed,storage_.freeHead,
            bool(managerTable3C_),bool(managerCamera_)};
    }
    ~OriginalUiSearchListCold50KG(){if(reference00_&&!released_)std::terminate();}
    OriginalUiSearchListCold50KG(const OriginalUiSearchListCold50KG&)=delete;
    OriginalUiSearchListCold50KG& operator=(const OriginalUiSearchListCold50KG&)=delete;
};
}
