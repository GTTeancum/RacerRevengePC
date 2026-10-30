#pragma once
namespace rr::game { class OriginalLoadingManager33050LA; class OriginalUiEntityScope50KG; }
// Cold constructor plus private exact manager Camera membership. No public member or draw API.
#include "../../../src/game/original_application_entities50la.h"
#include "../../../src/game/search_cold50bu.h"
#include "../../../src/game/original_selected_array_guards27ed40.h"
#include "../../../src/game/original_camera_observer_assertions50kg.h"
#include "../../../src/game/original_search_block_guards27e3c0.h"
#include <memory>
#include <array>
#include <new>
#include <exception>
namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
class OriginalUiGarageManagerSearch50KG;
class OriginalUiLightManagerOwner50KG;
class OriginalUiCourseOwnedLifecycle50KG;
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
    friend class rr::game::OriginalLoadingManager33050LA;
    friend class rr::game::OriginalUiEntityScope50KG;
    friend class OriginalUiGarageManagerSearch50KG;
    friend class OriginalUiCourseOwnedLifecycle50KG;
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

    // Native adaptation: this List's fixed inline words/pointer slot were
    // admitted with its wrapper, before source membership. Lazy first-block
    // adoption publishes that retained backing; it does NOT emulate original
    // malloc-null timing, issue a malloc-success receipt, or increase capacity.
    // Typed Course model identities in the SAME source30/34/38/3C List.
    // No selected-buffer writes or collection receipt accompanies membership.
    std::array<const game::OriginalWreckComponent50LA*,84> courseMembers_{};
    std::array<std::uint32_t,84> courseSlots_{};
    // Separate typed Page camera in this SAME List block history. Token340
    // cannot alias any of the84 model-ordinal tokens; no guest pointer cast.
    const game::OriginalWreckComponent50LA* courseCameraMember_=nullptr;
    const game::OriginalWreckComponent50LA* courseCameraPendingMember_=nullptr;
    std::uint32_t courseCameraSlot_=0,courseCameraPendingSlot_=0;
    static constexpr std::uint32_t courseCameraToken_=85u*4u;
    std::size_t coursePendingOrdinal_=84;std::uint32_t coursePendingSlot_=0;
    const std::uint32_t* courseBlockPointers_[1]{};
    const std::uint32_t* const* courseTable3C_=nullptr;
    bool courseMembersEmpty()const noexcept {
        if(courseCameraMember_||courseCameraSlot_)return false;
        for(std::size_t i=0;i<courseMembers_.size();++i)if(courseMembers_[i]||courseSlots_[i])return false;
        return true;
    }
    bool courseStorageCurrent()const noexcept {
        if((coursePendingOrdinal_==84)!=(!coursePendingSlot_)||coursePendingOrdinal_>84||
           bool(courseCameraPendingMember_)!=bool(courseCameraPendingSlot_)||
           (coursePendingSlot_&&courseCameraPendingSlot_)||bool(courseCameraMember_)!=bool(courseCameraSlot_))return false;
        if(managerCamera_||managerTable3C_||managerBlockPointers_[0]||!lightMembersEmpty()||lightTable3C_)return false;
        if(!storage_.blockCount)return !courseTable3C_&&!courseBlockPointers_[0]&&courseMembersEmpty()&&
            !storage_.freeHead&&(derivedConstructed_?(storage_.perBlock==1024&&storage_.lastUsed==1024):
                (!storage_.perBlock&&!storage_.lastUsed));
        if(!derivedConstructed_||storage_.perBlock!=1024||storage_.blockCount!=1||
           storage_.lastUsed>85||courseTable3C_!=courseBlockPointers_||courseBlockPointers_[0]!=storage_.words)return false;
        std::array<bool,85> occupied{};std::size_t live=0;
        for(std::size_t i=0;i<courseMembers_.size();++i){
            const auto slot=courseSlots_[i];const auto* member=courseMembers_[i];
            if(bool(slot)!=bool(member))return false;if(!member)continue;
            if(!slot||slot>storage_.lastUsed||occupied[slot-1]||storage_.words[slot-1]!=(i+1u)*4u)return false;
            for(std::size_t j=0;j<i;++j)if(courseMembers_[j]==member)return false;
            occupied[slot-1]=true;++live;
        }
        if(courseCameraMember_){
            if(!courseCameraSlot_||courseCameraSlot_>storage_.lastUsed||occupied[courseCameraSlot_-1]||
               storage_.words[courseCameraSlot_-1]!=courseCameraToken_)return false;
            for(auto* model:courseMembers_)if(model==courseCameraMember_)return false;
            occupied[courseCameraSlot_-1]=true;++live;
        }
        std::size_t free=0;
        for(auto slot=storage_.freeHead;slot;){
            if(slot>storage_.lastUsed||occupied[slot-1]||++free>storage_.lastUsed)return false;
            occupied[slot-1]=true;const auto word=storage_.words[slot-1];
            if((word&3u)!=2u)return false;slot=word>>2;
        }
        return live+free==storage_.lastUsed;
    }
    bool courseHistoryCurrent(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app)const noexcept {
        return owner&&courseManagerOwner_==owner&&!managerOwner_&&!lightManagerOwner_&&!loadingManagerOwner_&&
            same(issuer,app)&&attempted_&&!released_&&!selectedCount18_&&selectedGrowth20_==30&&selectedCapacity24_==30&&
            selectedOwned28Bit0_&&usage10_<=2&&reference00_==1u+usage10_&&courseStorageCurrent();
    }
    bool selectCourseModelSlot(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        std::size_t ordinal,const game::OriginalWreckComponent50LA& model,std::uint32_t& slot,D& d)noexcept {
        d={};if(!courseHistoryCurrent(owner,issuer,app)||ordinal<8||ordinal>=courseMembers_.size()||
           courseMembers_[ordinal]||courseSlots_[ordinal]||coursePendingSlot_||courseCameraPendingSlot_||usage10_!=2||reference00_!=3||!derivedConstructed_||
           !soultreeKnown_||!searchableKnown_)return fail(d,S::phase,0x27E3B0);
        if(courseCameraMember_==&model)return fail(d,S::identity,0x27E3B0);
        for(const auto* p:courseMembers_)if(p==&model)return fail(d,S::identity,0x27E3B0);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;std::uint32_t selected=0;
        const auto result=next.blockCount?game::selectSearchSlot50BS(next,0,selected):game::adoptFirstSearchBlock50BU(next,selected);
        if(result!=game::SearchStorageStatus50BS::Complete||!selected||selected>85)
            return fail(d,S::phase,0x27E3B0);
        // v20 selection publishes first block/table before caller assertion;
        // existing free/tail selection has no source reservation write.
        storage_.blockCount=next.blockCount;storage_.lastUsed=next.lastUsed;
        courseBlockPointers_[0]=storage_.words;courseTable3C_=courseBlockPointers_;
        coursePendingOrdinal_=ordinal;coursePendingSlot_=slot=selected;
        d={S::complete,0x27E570,true};return true;
    }
    bool insertCourseModelSelected(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        std::size_t ordinal,const game::OriginalWreckComponent50LA& model,std::uint32_t slot,D& d)noexcept {
        d={};if(!courseHistoryCurrent(owner,issuer,app)||ordinal>=84||coursePendingOrdinal_!=ordinal||
            coursePendingSlot_!=slot||!slot||courseMembers_[ordinal]||courseSlots_[ordinal]||usage10_!=2)
            return fail(d,S::phase,0x27E570);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;
        if(game::insertSearchSlot50BS(next,slot,std::uint32_t(ordinal+1u)*4u)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E578);
        storage_.lastUsed=next.lastUsed;storage_.freeHead=next.freeHead;storage_.words[slot-1]=next.words[slot-1];
        courseMembers_[ordinal]=&model;courseSlots_[ordinal]=slot;coursePendingOrdinal_=84;coursePendingSlot_=0;
        d={S::complete,0x27E5A8,true};return true;
    }
    bool cancelCourseSelectionNative(std::size_t ordinal,std::uint32_t slot)noexcept {
        if(coursePendingOrdinal_!=ordinal||coursePendingSlot_!=slot||!slot||!courseStorageCurrent())return false;
        coursePendingOrdinal_=84;coursePendingSlot_=0;return true;
    }
    bool courseModelCurrent(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        std::size_t ordinal,const game::OriginalWreckComponent50LA& model,std::uint32_t slot)const noexcept {
        return courseHistoryCurrent(owner,issuer,app)&&ordinal<courseMembers_.size()&&
            courseMembers_[ordinal]==&model&&courseSlots_[ordinal]==slot&&slot&&usage10_==2&&reference00_==3;
    }
    bool removeCourseModel(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        std::size_t ordinal,const game::OriginalWreckComponent50LA& model,std::uint32_t slot,D& d)noexcept {
        d={};if(!courseModelCurrent(owner,issuer,app,ordinal,model,slot))return fail(d,S::identity,0x27E5B0);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;
        if(game::removeSearchSlot50BS(next,slot,std::uint32_t(ordinal+1u)*4u)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E5B0);
        storage_.freeHead=next.freeHead;storage_.words[slot-1]=next.words[slot-1];
        courseMembers_[ordinal]=nullptr;courseSlots_[ordinal]=0; // Native identities, not source block zeroing.
        d={S::complete,0x27E5B0,true};return true;
    }
    bool selectCourseCameraSlot(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t& slot,D& d)noexcept {
        d={};if(!courseHistoryCurrent(owner,issuer,app)||courseCameraMember_||courseCameraSlot_||
           courseCameraPendingSlot_||coursePendingSlot_||usage10_!=2||reference00_!=3||!derivedConstructed_||
           !soultreeKnown_||!searchableKnown_||camera.kind!=game::OriginalWreckComponentKind50LA::camera236180)
            return fail(d,S::phase,0x27E3B0);
        for(auto* model:courseMembers_)if(model==&camera)return fail(d,S::identity,0x27E3B0);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;std::uint32_t selected=0;
        const auto result=next.blockCount?game::selectSearchSlot50BS(next,0,selected):game::adoptFirstSearchBlock50BU(next,selected);
        if(result!=game::SearchStorageStatus50BS::Complete||!selected||selected>85)return fail(d,S::phase,0x27E3B0);
        storage_.blockCount=next.blockCount;storage_.lastUsed=next.lastUsed;
        courseBlockPointers_[0]=storage_.words;courseTable3C_=courseBlockPointers_;
        courseCameraPendingMember_=&camera;courseCameraPendingSlot_=slot=selected;
        d={S::complete,0x27E570,true};return true;
    }
    bool insertCourseCameraSelected(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t slot,D& d)noexcept {
        d={};if(!courseHistoryCurrent(owner,issuer,app)||courseCameraPendingMember_!=&camera||
           courseCameraPendingSlot_!=slot||!slot||courseCameraMember_||courseCameraSlot_||usage10_!=2)
            return fail(d,S::phase,0x27E570);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;
        if(game::insertSearchSlot50BS(next,slot,courseCameraToken_)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E578);
        storage_.lastUsed=next.lastUsed;storage_.freeHead=next.freeHead;storage_.words[slot-1]=next.words[slot-1];
        courseCameraMember_=&camera;courseCameraSlot_=slot;courseCameraPendingMember_=nullptr;courseCameraPendingSlot_=0;
        d={S::complete,0x27E5A8,true};return true;
    }
    bool courseCameraCurrent(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t slot)const noexcept {
        return courseHistoryCurrent(owner,issuer,app)&&courseCameraMember_==&camera&&courseCameraSlot_==slot&&
            slot&&usage10_==2&&reference00_==3;
    }
    bool cancelCourseCameraSelectionNative(const game::OriginalWreckComponent50LA& camera,std::uint32_t slot)noexcept {
        if(courseCameraPendingMember_!=&camera||courseCameraPendingSlot_!=slot||!slot||!courseStorageCurrent())return false;
        courseCameraPendingMember_=nullptr;courseCameraPendingSlot_=0;return true;
    }
    bool removeCourseCamera(const OriginalUiCourseOwnedLifecycle50KG* owner,
        const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& camera,std::uint32_t slot,D& d)noexcept {
        d={};if(!courseCameraCurrent(owner,issuer,app,camera,slot))return fail(d,S::identity,0x27E5B0);
        game::search_block_guards27E3C0::invalidateNative();
        auto next=storage_;
        if(game::removeSearchSlot50BS(next,slot,courseCameraToken_)!=game::SearchStorageStatus50BS::Complete)
            return fail(d,S::phase,0x27E5B0);
        storage_.freeHead=next.freeHead;storage_.words[slot-1]=next.words[slot-1];
        courseCameraMember_=nullptr;courseCameraSlot_=0;d={S::complete,0x27E5B0,true};return true;
    }
    bool retireCourseBlockHistoryNative()noexcept {
        if(!courseStorageCurrent()||!courseMembersEmpty()||coursePendingSlot_||courseCameraPendingSlot_||selectedCount18_)return false;
        courseTable3C_=nullptr;courseBlockPointers_[0]=nullptr;
        // Actual native backing ceases with List disposal; never reset the
        // reached source lastUsed/freeHead/blockCount to a fake cold state.
        return true;
    }
    bool attempted_=false,derivedConstructed_=false,complete_=false,released_=false;
    bool sourceSelectedGuards_=false,selectedAllocationAttempted_=false;
    game::selected_array_guards27ED40::History selectedGuardHistory_;
    bool sourceSelectedGuardCutCurrent()const noexcept {
        return sourceSelectedGuards_&&attempted_&&!released_&&!selectedAllocationAttempted_&&
            game::selected_array_guards27ED40::preflight(selectedGuardHistory_)&&
            selectedGuardHistory_.entered&&!selectedGuardHistory_.returned&&
            reference00_==1&&!usage10_&&!selected1C_&&!selectedCount18_&&!selectedGrowth20_&&
            !selectedCapacity24_&&!selectedOwned28Bit0_&&!soultreeKnown_&&!searchableKnown_&&
            !derivedConstructed_&&!complete_&&!managerAliases_&&!managerReleasedAliases_&&
            !lightAliases_&&!lightReleasedAliases_&&!storage_.perBlock&&!storage_.blockCount&&
            !storage_.lastUsed&&!storage_.freeHead;
    }
    bool selectedConstructionCurrent()const noexcept {
        if(sourceSelectedGuards_&&(!game::selected_array_guards27ED40::preflight(selectedGuardHistory_)||
            !selectedGuardHistory_.entered))return false;
        if(sourceSelectedGuards_&&!selectedGuardHistory_.returned)return sourceSelectedGuardCutCurrent();
        return selectedAllocationAttempted_&&!selectedCount18_&&selectedGrowth20_==30&&
            selectedCapacity24_==30&&selectedOwned28Bit0_;
    }
    // Manager2A0 is ONE list, aliased by the same node's34/38. This native
    // authority is distinct from the four independent Scene search lists.
    const OriginalUiGarageManagerSearch50KG* managerOwner_=nullptr;
    std::uint8_t managerAliases_=0,managerReleasedAliases_=0;
    const OriginalUiLightManagerOwner50KG* lightManagerOwner_=nullptr;
    const rr::game::OriginalLoadingManager33050LA* loadingManagerOwner_=nullptr;
    const OriginalUiCourseOwnedLifecycle50KG* courseManagerOwner_=nullptr;
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
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d,
        bool sourceSelectedGuards=false)noexcept {
        d={};if(!same(creator,application))return fail(d,S::identity,0x27E260);
        if(attempted_||released_)return fail(d,S::alreadyStarted,0x27E260);
        if(!application_->application10())return fail(d,S::input,0x27E260);
        // Only admitted legacy invocations invalidate; rejected inputs do not.
        if(!sourceSelectedGuards){game::selected_array_guards27ED40::invalidateNative();
            game::search_block_guards27E3C0::invalidateNative();}
        sourceSelectedGuards_=sourceSelectedGuards;attempted_=true;
        reference00_=1; //27E9DC ->1048F0. SearchList is NOT Entity141300.
        //27ED10 initializes selected-array scalars and clears owned bit0.
        selectedCount18_=0;selectedGrowth20_=0;selectedCapacity24_=0;
        selectedOwned28Bit0_=false;
        usage10_=0; //27EA0C, before27ED40.
        if(sourceSelectedGuards_){std::uint32_t guardPC=0;
            if(!game::selected_array_guards27ED40::runFresh30(selectedGuardHistory_,guardPC))
                return fail(d,S::phase,guardPC);}
        //27ED40(30,30): cold fresh positive-capacity branch. Unused elements
        // remain unread/uninitialized; no synthetic selected Entity is stored.
        selectedAllocationAttempted_=true;
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
        if(managerOwner_||lightManagerOwner_||loadingManagerOwner_||courseManagerOwner_)return fail(d,S::phase,0x104970);
        if(released_)return true;
        if(!attempted_){released_=true;return true;}
        // No retain, member insertion, zone or observer API exists. Fail closed
        // if a future extension changes that invariant without joining teardown.
        if(reference00_!=1||usage10_||!selectedConstructionCurrent()||storage_.blockCount||storage_.freeHead||
           (derivedConstructed_?(storage_.perBlock!=0x400||storage_.lastUsed!=0x400):
                                (storage_.perBlock!=0||storage_.lastUsed!=0)))
            return fail(d,S::phase,0x27E2C0);
        const bool guardCut=sourceSelectedGuardCutCurrent();
        --reference00_; //Native withdrawal for an unreturned source guard cut; otherwise104970.
        //27E2C0's block loop/table free is absent for this real zero-block state.
        //27EA70 -> free selected array ->104910. No Entity counter decrement.
        selected1C_.reset();
        released_=true;complete_=false;d={S::complete,guardCut?0u:(derivedConstructed_?0x27E384u:0x27EAE0u),true};return true;
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
        game::search_block_guards27E3C0::invalidateNative();
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
        game::search_block_guards27E3C0::invalidateNative();
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
    // Source primary membership uses the SAME30/34/38/40 scalars, but real
    // nullable host allocations rather than the legacy inline block adoption.
    // Host slots remain index+1 handles; they are never guest pointer values.
    bool sourceLightMode_=false,sourceTableAttempted_=false,sourceTablePublished_=false;
    bool sourceBlockAttempted_=false,sourceBlockPublished_=false,sourceBlockReady_=false;
    std::unique_ptr<std::uint32_t*[]> sourceLightTable_;
    std::unique_ptr<std::uint32_t[]> sourceLightBlock_;
    std::array<bool,1024> sourceLightWordKnown_{};
    game::search_block_guards27E3C0::History sourceTableGuard_,sourceBlockGuard_;
    const void* sourceLightOccurrence_=nullptr;
    const game::OriginalWreckComponent50LA* sourcePendingLight_=nullptr;
    std::uint32_t sourcePendingToken_=0,sourcePendingSlot_=0;
    std::array<std::uint32_t,4> sourcePendingPosition_{},sourcePendingVector_{};
    bool sourceLightHistoryCurrent(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app)const noexcept {
        if(!sourceLightMode_)return lightEmptyCurrent(owner,creator,app)&&lightMembersEmpty()&&!lightTable3C_&&!lightBlockPointers_[0];
        if(!owner||owner!=lightManagerOwner_||!same(creator,app)||!sourceSelectedGuards_||!attempted_||released_||
           !derivedConstructed_||managerOwner_||courseManagerOwner_||loadingManagerOwner_||
           lightAliases_!=2||lightReleasedAliases_>2||reference00_!=3u-lightReleasedAliases_||usage10_!=2||
           !selectedConstructionCurrent()||storage_.perBlock!=1024||storage_.blockCount>1||
           managerCamera_||managerTable3C_||!courseMembersEmpty()||courseTable3C_||lightTable3C_||lightBlockPointers_[0]||
           !game::search_block_guards27E3C0::valid(sourceTableGuard_)||!game::search_block_guards27E3C0::valid(sourceBlockGuard_)||
           bool(sourceLightOccurrence_)!=bool(sourcePendingLight_)||bool(sourcePendingLight_)!=bool(sourcePendingToken_))return false;
        if(sourceTablePublished_&&(!sourceTableAttempted_||!sourceTableGuard_.returned||!sourceLightTable_))return false;
        if(sourceBlockAttempted_&&!sourceTablePublished_)return false;
        if(sourceBlockPublished_&&(!sourceBlockAttempted_||!sourceLightTable_||sourceLightTable_[0]!=sourceLightBlock_.get()))return false;
        if(sourceBlockReady_&&(!sourceBlockPublished_||!sourceBlockGuard_.returned||!sourceLightBlock_))return false;
        if(!sourceBlockReady_){if(storage_.blockCount||storage_.lastUsed!=1024||storage_.freeHead||sourcePendingSlot_||!lightMembersEmpty())return false;
            for(bool known:sourceLightWordKnown_)if(known)return false;return true;}
        if(storage_.blockCount!=1||storage_.lastUsed>1024||sourcePendingSlot_>1024)return false;
        std::array<bool,1024> free{};auto next=storage_.freeHead;
        while(next){if(next>storage_.lastUsed||free[next-1]||!sourceLightWordKnown_[next-1]||lightMembers_[next-1]||
            (sourceLightBlock_[next-1]&3u)!=2)return false;free[next-1]=true;next=sourceLightBlock_[next-1]>>2;}
        for(std::size_t i=0;i<1024;++i){if(i>=storage_.lastUsed){if(sourceLightWordKnown_[i]||lightMembers_[i]||lightTokens_[i])return false;continue;}
            if(!sourceLightWordKnown_[i])return false;
            if(lightMembers_[i]){if(free[i]||!lightTokens_[i]||(lightTokens_[i]&3)||sourceLightBlock_[i]!=lightTokens_[i])return false;}
            else if(!free[i])return false;}
        return !sourcePendingSlot_||(sourcePendingSlot_==(storage_.freeHead?storage_.freeHead:storage_.lastUsed+1));
    }
    bool selectSourceLightSlot(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,const void* occurrence,
        const game::OriginalWreckComponent50LA& light,std::uint32_t ordinal,
        const std::array<std::uint32_t,4>& position,const std::array<std::uint32_t,4>& appVector,
        std::uint32_t& slot,D& d)noexcept {
        d={};if(!occurrence||ordinal<2||ordinal>184||!sourceSelectedGuards_||!derivedConstructed_||
           lightAliases_!=2||lightReleasedAliases_||reference00_!=3||sourceLightOccurrence_||
           !sourceLightHistoryCurrent(owner,creator,app)||
           (sourceLightMode_&&!sourceBlockReady_))return fail(d,S::phase,0x280afc);
        sourceLightMode_=true;sourceLightOccurrence_=occurrence;sourcePendingLight_=&light;sourcePendingToken_=4*(ordinal+1);
        sourcePendingPosition_=position;sourcePendingVector_=appVector;
        if(!storage_.blockCount){
            sourceTableAttempted_=true;sourceLightTable_.reset(new(std::nothrow) std::uint32_t*[1]); //realloc(NULL,4),27E410.
            std::uint32_t pc=0;if(!game::search_block_guards27E3C0::run(0,bool(sourceLightTable_),sourceTableGuard_,pc))return fail(d,S::phase,pc);
            sourceTablePublished_=true; //27E484; table[0] remains UNKNOWN.
            sourceBlockAttempted_=true;sourceLightBlock_.reset(new(std::nothrow) std::uint32_t[1024]); //malloc4096,27E48C.
            sourceLightTable_[0]=sourceLightBlock_.get();sourceBlockPublished_=true; //27E4A4, even NULL.
            if(!game::search_block_guards27E3C0::run(1,bool(sourceLightBlock_),sourceBlockGuard_,pc))return fail(d,S::phase,pc);
            storage_.blockCount=1;storage_.lastUsed=0;sourceBlockReady_=true; //27E52C/530.
        }
        if(storage_.freeHead)sourcePendingSlot_=storage_.freeHead;
        else {if(storage_.lastUsed==1024)return fail(d,S::phase,0x27e410);sourcePendingSlot_=storage_.lastUsed+1;}
        slot=sourcePendingSlot_;d={S::complete,0x27e564,true};return true; //Selection does not reserve/pop/write slot.
    }
    bool insertSourceLightSelected(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,const void* occurrence,
        const game::OriginalWreckComponent50LA& light,std::uint32_t ordinal,
        const std::array<std::uint32_t,4>& position,const std::array<std::uint32_t,4>& appVector,
        std::uint32_t slot,D& d)noexcept {
        d={};if(!sourceLightMode_||!sourceLightHistoryCurrent(owner,creator,app)||!slot||slot!=sourcePendingSlot_||
           occurrence!=sourceLightOccurrence_||&light!=sourcePendingLight_||ordinal<2||ordinal>184||
           sourcePendingToken_!=4*(ordinal+1)||position!=sourcePendingPosition_||appVector!=sourcePendingVector_||lightReleasedAliases_)
            return fail(d,S::identity,0x280b88);
        if(storage_.freeHead==slot)storage_.freeHead=sourceLightBlock_[slot-1]>>2; //27E594.
        else ++storage_.lastUsed; //27E5A0.
        sourceLightBlock_[slot-1]=sourcePendingToken_;sourceLightWordKnown_[slot-1]=true;
        lightMembers_[slot-1]=&light;lightTokens_[slot-1]=sourcePendingToken_; //27E5A8 typedLight+30 identity.
        sourceLightOccurrence_=nullptr;sourcePendingLight_=nullptr;sourcePendingToken_=sourcePendingSlot_=0;
        d={S::complete,0x27e5a8,true};return true; //Observer280B90..BBC belongs to exact caller, not List.
    }
    bool cancelSourceLightSelectionNative(const void* occurrence,D& d)noexcept {
        d={};if(!sourceLightMode_||!occurrence||sourceLightOccurrence_!=occurrence||
           !sourceLightHistoryCurrent(lightManagerOwner_,creator_,application_))return fail(d,S::phase,0);
        sourceLightOccurrence_=nullptr;sourcePendingLight_=nullptr;sourcePendingToken_=sourcePendingSlot_=0;
        d={S::complete,0,true};return true; //No source scalar rollback; allocations survive until List retirement.
    }
    bool withdrawSourceLightNative(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,
        const game::OriginalWreckComponent50LA& light,std::uint32_t slot,D& d)noexcept {
        d={};if(!sourceLightMode_||sourceLightOccurrence_||!sourceLightHistoryCurrent(owner,creator,app)||
           !slot||slot>storage_.lastUsed||lightMembers_[slot-1]!=&light)return fail(d,S::identity,0x27e5b0);
        sourceLightBlock_[slot-1]=(storage_.freeHead<<2)|2u;storage_.freeHead=slot;lightMembers_[slot-1]=nullptr;
        d={S::complete,0,true};return true; //Native reverse borrower withdrawal; preserve allocation/used history.
    }
    bool lightEmptyCurrent(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application)const noexcept {
        return owner&&owner==lightManagerOwner_&&!managerOwner_&&same(creator,application)&&attempted_&&!released_&&
            lightAliases_<=2&&lightReleasedAliases_<=lightAliases_&&
            reference00_==1u+lightAliases_-lightReleasedAliases_&&usage10_==lightAliases_&&
            selectedConstructionCurrent()&&
            !storage_.blockCount&&!storage_.freeHead&&
            (derivedConstructed_?(storage_.perBlock==0x400&&storage_.lastUsed==0x400):
                                 (!storage_.perBlock&&!storage_.lastUsed));
    }
    bool lightHistoryCurrent(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& app)const noexcept {
        if(sourceLightMode_)return sourceLightHistoryCurrent(owner,creator,app);
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
        d={};if(sourceSelectedGuards_||sourceLightMode_||!lightHistoryCurrent(owner,creator,app)||lightAliases_!=2u||
           lightReleasedAliases_||reference00_!=3u||token<12u||token>740u||(token&3u))return fail(d,S::phase,0x27e3b0);
        game::search_block_guards27E3C0::invalidateNative();
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
        d={};if(sourceLightMode_||!lightHistoryCurrent(owner,creator,app)||!lightMemberAt(&light,slot))return fail(d,S::identity,0x27e5b0);
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
        d={};if(!lightHistoryCurrent(owner,creator,application)||!lightMembersEmpty()||sourceLightOccurrence_||field!=this||ordinal!=lightReleasedAliases_+1||
            ordinal>lightAliases_||reference00_<2)return fail(d,S::phase,0x27F200);
        --reference00_;++lightReleasedAliases_;d={S::complete,ordinal==1?0x27F200u:0x27F21Cu,true};return true;
    }
    bool releaseLightPayload(const OriginalUiLightManagerOwner50KG* owner,
        const original_main_entry50KG::Application* creator,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,D& d)noexcept {
        d={};if(!lightHistoryCurrent(owner,creator,application)||!lightMembersEmpty()||sourceLightOccurrence_||reference00_!=1||
            lightAliases_!=lightReleasedAliases_)return fail(d,S::phase,0x1481A8);
        const bool guardCut=sourceSelectedGuardCutCurrent();
        --reference00_;lightTable3C_=nullptr;lightBlockPointers_[0]=nullptr;
        sourceLightBlock_.reset();sourceLightTable_.reset(); //Native payload withdrawal, no source NULL/scalar reset.
        selected1C_.reset();released_=true;complete_=false;
        d={S::complete,guardCut?0u:0x1481A8u,true};return true;
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
