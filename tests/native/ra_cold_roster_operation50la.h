#pragma once
namespace original_results_roster50LA { class Read; }
namespace ra_native_session50LA { class Join; }
namespace ra_cold_roster50LA { class FreshExecutionArena; }
namespace rr::game { class OriginalResults10450LA; }
#include "ra_cold_roster_resources50la.h"
#include "ra_source_camera_ownership50la.h"
#include "original_racer_material_sources50la_fixture.h"
#include "original_ui_main_entry50kg.h"
#include "native_racer_handling50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_contact50hw_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include "original_ai_physical_recovery.h"
#include "lap_drive50gj_fixture.h"
#include "effect_cold50bj.h"
#include "reset_effects50bk.h"
#include <cstring>
#include <exception>
#include <stdexcept>
#include <utility>
#include "ra_cold_roster_arena50la.h"

// Existing main-TU instrumentation; no second allocator or counter is defined.
extern thread_local std::uint64_t allocations;
namespace ra_cold_roster50LA {
struct Diagnostic {std::size_t slot=0;const char* stage=nullptr;const char* failure=nullptr;std::string detail;};
class Operation final {
    friend class FreshExecutionArena;
    friend class ::ra_source_camera50LA::OwnershipBridge; //Actual native handling lanes during the existing Bank visit.
    friend class ::original_results_roster50LA::Read;
    friend class rr::game::OriginalResults10450LA;
    using Model=native_racer_model50LA::Owner;
    // The bridge whose roster this operation constructs (the retired RA aggregate's
    // runtimeQueries_ was this same object). Common rewrite (area C): no RA world here.
    std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtime_;
    std::shared_ptr<const Resources> resources_;
    std::shared_ptr<const FreshProbe> probe_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> entities_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<g::OriginalMotionLibrary50LA> motions_;
    const original_main_entry50KG::Application* application_;
    native_racer50HN::Bank* bank_;
    const std::uint64_t generation_;
    Arena arena_;
    // Real objects, retained across every partial stage and Bank save.
    std::shared_ptr<RuntimeOwners> owners_;
    std::array<std::unique_ptr<Model>,8>& right_;
    std::array<std::unique_ptr<Model>,8>& left_;
    std::array<std::unique_ptr<Model>,8>& bottomRight_;
    std::array<std::unique_ptr<Model>,8>& bottomLeft_;
    std::array<std::unique_ptr<Model>,8>& chariot_;
    std::array<std::unique_ptr<native_racer50HW::Visual>,8>& visuals_;
    std::array<std::unique_ptr<native_racer50HW::Cables>,8>& cables_;
    std::array<native_racer_contact50HW::Binding,8>& contacts_;
    std::array<a::NativePhyLoad50LA,8> nativePhyLoads_;
    // Native human caller custody begins before cold consumers. The separate
    // source Player allocation/axes/vtable are deliberately not adopted.
    struct NativePlayerLoad final {
        bool armAttempted=false,armed=false,returnAttempted=false,steeringLoaded=false;
        bool steeringStored=false,returned=false,retired=false;
        std::uint32_t pc=0,source5C=0,indexFFC=0,steeringWord=0;
        const g::PlayerEventBindings49A* bindings=nullptr;
        original_camera_session50KX::Session* session=nullptr;
        Model* model=nullptr;
        g::OriginalRacerEntityCold50LA* racer=nullptr;
        g::OriginalRacerEntityCold50LA* nativeReturn=nullptr;
        std::uint64_t racerKey=0;
        // A NEW native manager call after this return, not the earlier Race
        // population receipt. Keep Player return history separate and final.
        bool managerAttempted=false,resourcePairReturned=false;
        std::uint32_t managerPC=0;
        g::OriginalRacerEntityCold50LA* managerReceiver=nullptr;
        const g::OriginalRaceConstructor50LA* resourceRace=nullptr;
        const a::OriginalStandardMaterialDomain50LA* resourceDomain=nullptr;
        const a::OriginalStandardMaterialContext50LA* resourceContext=nullptr;
        a::OriginalStandardMaterialDiagnostic50LA resourceDiagnostic{};
        bool speechForwardAttempted=false,speechForwardBound=false,speechCallAttempted=false;
        bool base9CReturned=false,speechConstructorComplete=false,speechPublished=false,speechRetired=false;
        std::uint32_t speechPC=0;
        g::OriginalWreckComponent50LA* speechEntity=nullptr;
    } nativePlayerLoad_;
    bool nativePlayerCameraOwnersCurrent(const g::PlayerEventState49A* player,
        const g::ManagerHeldSources49A& held)const noexcept {
        return nativePlayerLoad_.armed&&!nativePlayerLoad_.retired&&
            player==arena_.nativePlayerCaller.player_&&arena_.nativePlayerCaller.environment_&&
            &held==&arena_.nativePlayerCaller.environment_->held;
    }
    bool armNativePlayerLoad()noexcept {
        auto& h=nativePlayerLoad_;const auto& c=arena_.nativePlayerCaller;
        if(h.armAttempted||h.retired)return false;
        h.armAttempted=true;
        auto& r=runtime();const auto lane=[&](const auto* p){return p&&r.registeredRange(p,sizeof(*p));};
        if(arena_.humanCount!=1u||c.bank_!=bank_||!c.ingress_||!c.actions_||
           !lane(c.queue_)||!lane(c.application_)||!lane(c.child_)||!lane(c.player_)||!lane(c.environment_)||
           c.authored_!=arena_.selectedAuthored||!c.authored_||
           c.steering10A4_!=arena_.selectedSteering||!c.steering10A4_||
           c.actions_->phase()!=g::ControllerActionPhase50EC::Idle||c.queue_->count!=0||
           c.player_->source5C>=8u||c.actions_->deviceIndex()!=c.player_->source5C||
           c.environment_->held.devices[c.player_->source5C]!=&c.ingress_->buttons()||
           c.child_->player!=c.player_||c.child_->environment!=c.environment_||
           !c.child_->bindings||c.child_->childIndex!=0u||
           (c.application_->rootFlags228&12u)||
           (c.application_->root.flags24&0x600u)!=0x600u||
           c.application_->root.firstChild!=0u||c.application_->root.childCount!=1u||
           !c.application_->root.children||c.application_->visitBudget<1u||
           c.application_->root.children[0].next!=g::noEventChild48Z||
           !(c.application_->root.children[0].flags29&1u)||(c.application_->root.children[0].flags29&8u)||
           c.application_->childCallback!=g::playerEventChild49A||c.application_->childContext!=c.child_||
           arena_.live.state.fields.vehicle58!=0u||arena_.live.state.fields.nativeAiOwnersBound50IV)return false;
        h.source5C=c.player_->source5C;h.indexFFC=c.player_->indexFFC;h.bindings=c.child_->bindings;
        if(!c.racer60_)return false;
        if(c.audio_&&!r.bindNativePlayerSpeechForward22A210(*this))return false;
        h.armed=true;h.pc=0x1a0848u;return true;
    }
    std::array<bool,8> attempted_{},completed_{};
    std::size_t nextSlot_=0;
    // Immutable source closure retained even after a later partial failure.
    original_racer_material_sources50LA::Materials modelMaterials_;
    original_racer_material_sources50LA::Textures modelTextures_;
    bool terminal_=false,closed_=false;
    // A distinct native allocation lifetime, never a claim that an existing
    // warm execution arena was reset into a source constructor.
    std::weak_ptr<FreshExecutionArena> constructorArena_;
    bool constructorOnly_=false;
    std::array<bool,8> constructorSlotComplete_{};
    bool constructorResourcesAttempted_=false,constructorResourcesComplete_=false;
    bool completeConstructorModelResources(Diagnostic&)noexcept;
    bool constructorArenaCurrent()const noexcept;
    bool releaseConstructorArenaNative(FreshExecutionArena&)noexcept;
    static std::shared_ptr<Operation> bindConstructorOnly(
        std::shared_ptr<ra_source_camera50LA::OwnershipBridge>,
        const original_main_entry50KG::Application&,
        const std::shared_ptr<FreshExecutionArena>&,std::uint64_t generation,
        Diagnostic&,std::shared_ptr<RuntimeOwners> ={})noexcept;
    bool constructConstructorSlot(native_racer50HN::Bank&,std::size_t,Diagnostic&)noexcept;
    bool constructorSlotsCurrent(const FreshExecutionArena&)const noexcept;

    Operation(std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtime,
        const original_main_entry50KG::Application& app,native_racer50HN::Bank& bank,
        const Arena& arena,std::shared_ptr<const FreshProbe> probe,std::shared_ptr<RuntimeOwners> owners,
        std::uint64_t generation)
        :runtime_(std::move(runtime)),resources_(runtime_->rosterResources_),
         probe_(std::move(probe)),entities_(app.entities()),
         domain_(app.standardMaterialDomain()),motions_(app.motionLibrary4333C0()),application_(&app),bank_(&bank),
         generation_(generation),arena_(arena),owners_(std::move(owners)),
         right_(owners_->right),left_(owners_->left),bottomRight_(owners_->bottomRight),bottomLeft_(owners_->bottomLeft),
         chariot_(owners_->chariot),visuals_(owners_->visuals),cables_(owners_->cables),contacts_(owners_->contacts){}
    static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
    ra_source_camera50LA::OwnershipBridge& runtime() const noexcept { return *runtime_; }
    bool registered()const noexcept {
        if(constructorOnly_&&!constructorArenaCurrent())return false;
        auto& r=runtime();auto& a=arena_;auto& x=a.live;
        if(!r.cameraBankCurrent()||r.bankIdentity_!=bank_||!r.actual_||&r.actual_->frame!=&x.state||
           !r.actual_->recovery||!r.actual_->movement||!resources_)return false;
        const ra_source_camera50LA::GridArenaOwners grid{x.bindings,x.parameters,x.selectors,a.ai,a.history,resources_->rigs_,a.humanCount};
        if(!r.gridInputsRegistered(grid))return false;
        auto lane=[&](const auto& value){return r.registeredRange(&value,sizeof value);};
        if(!lane(x.configurationEntryNonzero)||!lane(x.handRecords)||!lane(x.secondaryInputs)||
           !lane(x.selectors)||!lane(x.diagnostic)||!lane(a.tuning.boost)||!lane(a.tuning.coolingCF8)||
           !lane(a.tuning.mainSpring)||!lane(a.tuning.mainResponse)||!lane(a.tuning.secondarySpring)||
           !lane(a.tuning.secondaryResponse)||!lane(a.navigation)||!lane(a.lap)||
           !lane(a.rebaseModels)||!lane(a.rebaseCount)||!lane(a.effectNames)||!lane(a.effects)||
           !lane(a.effectCount)||!lane(a.resetBindings)||!lane(a.recoveryModels)||!lane(a.flame))return false;
        const auto& q=*r.actual_;const auto& recovery=*q.recovery;
        if(!lane(recovery.receipt)||!lane(recovery.selectedSource)||!lane(recovery.sourcePrepared)||
           recovery.podFilter.identityLists[0].identities.empty()||
           !r.registeredRange(recovery.podFilter.identityLists[0].identities.data(),
               recovery.podFilter.identityLists[0].identities.size()*sizeof(std::uint32_t))||
           !lane(q.terrain.follow)||!lane(q.terrain.lift1F0))return false;
        // Assignment uses existing fixed vector backing, with no resize while banked.
        const auto& main=q.mainFilter.identityLists[0].identities;
        const auto& secondary=q.secondaryFilter.identityLists[0].identities;
        return main.size()==4u&&secondary.size()==3u&&
            r.registeredRange(main.data(),main.size()*sizeof(main[0]))&&
            r.registeredRange(secondary.data(),secondary.size()*sizeof(secondary[0]));
    }
    // Private live read identity, NOT the cold handoff and NOT a B0 receipt.
    // No completedSteps==0 relaxation is made to the loader's existing Read.
    bool resultsRuntimeBankCurrent(const native_racer50HN::Bank* bank)const noexcept {
        if(terminal_||closed_||!runtime_||!resources_||!bank_||bank_!=bank)return false;
        auto& r=runtime();
        // registered() validates the destruction-bound pin before any Bank
        // dereference. Holding this Operation is not itself a Bank lease.
        if(r.terminal_||r.closed_||!registered()||r.rosterResources_!=resources_||
           !r.rosterRetirementRequired_)return false;
        const auto pin=std::static_pointer_cast<ra_source_camera50LA::OwnershipBridge::BankPin>(r.bankLifetime_.lock());
        if(!pin||pin->identity!=bank_||pin->owner.get()!=&r||pin->rosterOperation.get()!=this||
           bank_->count()!=resources_->racerCount_||nextSlot_!=bank_->count())return false;
        for(std::size_t i=0;i<bank_->count();++i)
            if(!completed_[i]||!r.rosterSlotComplete_[i])return false;
        return true;
    }
    bool resultsRacerIdentityCurrent(std::size_t index,std::uint32_t racer60)const noexcept {
        if(!resources_||index>=resources_->racerCount_||!resources_->rigs_[index])return false;
        const auto* source=g::original_racer_selection50IJ::findAnyByPodId(resources_->rigs_[index]->podId);
        return source&&source->racer60==racer60;
    }
    void effects(){
        auto& a=arena_;auto& s=a.live.state;const auto& b=a.live.bindings;
        need(s.phase==g::VehicleFramePhase49W::Ready&&s.bodies.main.frameFactor==0.f,"effect constructor requires actual fresh state");
        const bool br=b.visual.bottomRight1C0!=g::SceneNodeIndex49P::none;
        const bool bl=b.visual.bottomLeft1C4!=g::SceneNodeIndex49P::none;
        need(br==bl&&br==b.visual.extra1C0Present&&bl==b.visual.extra1C4Present,"optional effect models differ");
        for(auto& owner:a.effects)owner={};a.effectCount=br?14u:8u;
        constexpr unsigned source[]={0,1,2,3,4,4,5,5,2,3,4,4,5,5};
        auto make=[&](unsigned i){const auto& raw=resources_->effects_[source[i]];
            const auto profile=source[i]<4?g::EffectProfile50BE::Particle19A5E0:
                source[i]==4?g::EffectProfile50BE::Ordinary15F100:g::EffectProfile50BE::Fire15DE00;
            need(rr::assets::buildEffectControlOwner50BJ(i+1,profile,reinterpret_cast<const char*>(raw.data()),
                raw.size(),{0,0},a.effects[i]),"original effect control construction failed");};
        for(unsigned i=0;i<8;++i)make(i);
        if(br)for(unsigned i:{9u,11u,13u,8u,10u,12u})make(i);
        a.resetBindings={{1,2},{0,0},{5,6,bl?11u:0u,br?12u:0u},false};
        std::memcpy(a.resetState.timers820,s.impactSupplement.timers820,16);
        const auto before=::allocations;
        need(g::resetEffectStops50BK(a.resetBindings,a.effects,a.effectCount,a.resetState,a.resetWorkspace).status==
            g::EffectDispatchStatus50BE::Complete&&::allocations==before,"original reset effect stop sequence failed");
        std::memcpy(s.impactSupplement.timers820,a.resetState.timers820,16);
    }
public:
    Operation(const Operation&)=delete;Operation& operator=(const Operation&)=delete;
    ~Operation(){if(!closed_)std::terminate();}
private:
    // Common rewrite (area C): constructSlot() was the RA aggregate's warm per-slot constructor
    // (RA cold navigation lease, RA pod live-query bind, evidence/originalRoster50HW visuals) and
    // is retired: every slot of every course is constructed by common_roster50CR::loadAll. The
    // binding below (warm: bind(); fresh: bindConstructorOnly) is over the bridge alone; the
    // handling-class check reads the bridge's Track code (A1), not an RA navigation lease.
    static std::shared_ptr<Operation> bindImpl(std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtime,
        const original_main_entry50KG::Application& app,native_racer50HN::Bank& bank,
        const Arena& arena,std::shared_ptr<const FreshProbe> probe,Diagnostic& d,std::shared_ptr<RuntimeOwners> owners,
        const std::shared_ptr<FreshExecutionArena>& fresh,std::uint64_t generation)noexcept {
        d={};d.stage="retained cold binding";
        try {
            need(generation!=0,"actual session generation required");
            need(fresh||(!arena.humanCount?arena.nativePlayerCaller.bank_==nullptr:
                arena.nativePlayerCaller.bank_==&bank),
                "native human loading requires its actual forward cold caller");
            need((probe&&probe->frame_==&arena.live.state)||fresh,"actual cold probe ownership differs");
            need(arena.humanCount<=1u&&bool(arena.selectedAuthored)==bool(arena.selectedSteering)&&
                (!arena.humanCount||(arena.selectedAuthored&&arena.selectedSteering)),
                "actual single-human or all-AI constructor roles required");
            need(runtime&&!runtime->terminal_&&!runtime->closed_,"actual bridge ownership differs");
            auto& r=*runtime;
            need(r.rosterResources_&&r.rosterResources_->humanCount_==arena.humanCount&&r.rosterResources_->racerCount_==bank.count()&&r.cameraBankCurrent()&&r.bankIdentity_==&bank&&!r.cameraAttempted_&&
                !r.terminal_&&!r.closed_,"actual sealed Bank/roster binding absent");
            auto pin=std::static_pointer_cast<ra_source_camera50LA::OwnershipBridge::BankPin>(r.bankLifetime_.lock());
            need(pin&&!pin->rosterOperation&&!r.rosterRetirementRequired_,"roster operation cannot replace a surviving owner");
            if(!owners)owners=std::make_shared<RuntimeOwners>();
            for(std::size_t i=0;i<8;++i)need(!owners->right[i]&&!owners->left[i]&&!owners->bottomRight[i]&&
                !owners->bottomLeft[i]&&!owners->chariot[i]&&!owners->visuals[i]&&!owners->cables[i],
                "native roster constructor cannot replace existing owners");
            auto out=std::shared_ptr<Operation>(new Operation(runtime,app,bank,arena,std::move(probe),std::move(owners),generation));
            // No model or arena effects yet; rejected binding must be destroyable.
            out->closed_=true;out->constructorOnly_=bool(fresh);out->constructorArena_=fresh;
            need(out->registered(),"actual cold constructor lanes are not registered");
            need(out->domain_&&out->domain_->prepared()&&out->motions_&&
                !out->domain_->materials382B38()->coldCollectorCurrent(out->domain_->coldCollector14D440()),
                "cold model construction cannot invalidate a current collector");
            // A1 (class A): PHY214954 takes the handling class from the track's two-letter code
            // (selectAuthoredClass49N, with its sb/ra/sc exceptions), every course; AI16E590 keeps
            // its own selector. The arena's selection must be that class and its human/AI roles.
            g::AuthoredClass49N handlingClass{};
            need(r.track_!=nullptr,"the bridge's Track is required for the handling class");
            const auto& code=r.track_->identity.code.text;
            const char lowered[2]={static_cast<char>(code[0]>='A'&&code[0]<='Z'?code[0]-'A'+'a':code[0]),
                static_cast<char>(code[1]>='A'&&code[1]<='Z'?code[1]-'A'+'a':code[1])};
            need(g::selectAuthoredClass49N(lowered,2,handlingClass)&&
                arena.selection.inferredClass==handlingClass&&
                !arena.selection.applyProfileOverwrite&&
                (arena.humanCount==1u||arena.selection.configurationEntryNonzero),
                "actual handling selection differs from the track's class and its human/AI roles");
            need(r.inputs_.collisionRoster.size()==bank.count(),"actual complete collision roster required");
            for(std::size_t i=0;i<bank.count();++i)need(r.inputs_.collisionRoster[i]==out->resources_->collisions_[i].get()&&
                bank.completedSteps(i)==0,
                "reserved collision roster differs from retained owners");
            //Bind the actual existing process noise owner only at this private
            //same-Application producer. Never accept an arbitrary frame pointer
            //as proof, register the tables in Bank, seed, or copy their values.
            need(app.state&&app.state->known&&out->entities_&&
                arena.live.state.borrowedNoise50KG==&app.state->sharedNoise308030&&
                !r.sharedSecondaryNoiseOwner_&&!r.sharedSecondaryNoiseEntities_.lock(),
                "actual Application shared noise binding differs");
            r.sharedSecondaryNoiseOwner_=std::shared_ptr<g::CoupledNoiseContext49M>(
                app.state,&app.state->sharedNoise308030);
            r.sharedSecondaryNoiseEntities_=out->entities_;
            pin->rosterOperation=out;out->closed_=false;r.rosterRetirementRequired_=true;return out;
        }catch(const std::exception& e){d.failure="cold roster binding failed";try{d.detail=e.what();}catch(...){}return {};}
        catch(...){d.failure="cold roster binding exception";return {};}
    }
public:
    // PORT STAND-IN: no common caller binds the bridge's roster operation yet (the common roster
    // constructs every slot; the bridge's 2129C0 tail is not wired into the cold load).
    static std::shared_ptr<Operation> bind(std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtime,
        const original_main_entry50KG::Application& app,native_racer50HN::Bank& bank,
        const Arena& arena,std::shared_ptr<const FreshProbe> probe,std::uint64_t generation,Diagnostic& d,
        std::shared_ptr<RuntimeOwners> owners={})noexcept {
        return bindImpl(std::move(runtime),app,bank,arena,std::move(probe),d,std::move(owners),{},generation);
    }
    // Cold terminal teardown only. No borrowed arena/Bank may die first.
    // A later camera/grid/fresh stage must retire through its own real API.
    bool close()noexcept {
        if(closed_)return true;
        if(constructorOnly_&&!constructorArenaCurrent())return false;
        auto& r=runtime();if(!r.cameraBankCurrent()||r.bankIdentity_!=bank_)return false;
        if(!r.cameraRetiredForRoster())return false;
        for(unsigned i=0;i<8;++i)if(r.gridRetirementRequired_[i]||r.freshRetirementRequired_[i])return false;
        if(!r.withdrawNativePlayerSpeechForward22A210(*this))return false;
        // Withdraw borrowed caller/return pointers before any model or input
        // backing can retire. Keep reached history and the actual10A4 scalar.
        nativePlayerLoad_.retired=true;nativePlayerLoad_.session=nullptr;
        nativePlayerLoad_.model=nullptr;nativePlayerLoad_.racer=nullptr;
        nativePlayerLoad_.nativeReturn=nullptr;nativePlayerLoad_.bindings=nullptr;
        nativePlayerLoad_.managerReceiver=nullptr;nativePlayerLoad_.resourceRace=nullptr;
        nativePlayerLoad_.resourceDomain=nullptr;nativePlayerLoad_.resourceContext=nullptr;
        arena_.nativePlayerCaller={};
        // Model::close owns its actual Bank visit. Never nest it in one here.
        // Reverse source role order; partial successes remain retained/closed.
        for(std::size_t slot=resources_->racerCount_;slot-->0;){
            //Abort/terminal retirement is separate from the successful load
            //close. Never fabricate a close receipt for an incomplete slot.
            const a::NativePhyLoad50LA::Context context{this,bank_,generation_,slot};
            if(!nativePhyLoads_[slot].abort(context))return false;
            if(!r.revokeNativePod694(slot,*bank_))return false; //Before any borrowed body/model can retire.
            for(auto* model:{&chariot_[slot],&bottomLeft_[slot],&bottomRight_[slot],&left_[slot],&right_[slot]})
                if(*model&&!(*model)->close())return false;
        }
        // Visual/flame source views can remain in Bank snapshots. Retain all
        // backing through pin destruction; only mark actual model retirement.
        if(constructorOnly_){
            const auto fresh=constructorArena_.lock();
            if(!fresh||!releaseConstructorArenaNative(*fresh))return false;
        }
        r.rosterSlotComplete_.fill(false);r.rosterRetirementRequired_=false;closed_=true;return true;
    }
};
} // namespace ra_cold_roster50LA
// Common rewrite (area C): the RA aggregate members defined here (roster archive mount,
// retained handoff, live trigger query brackets, runtime storage construction) are retired
// with the RA world (common_track50CR / track_collision_world50CR own them for every course).

#include "ra_fresh_execution_arena50la.h"
#include "ra_fresh_roster_operation50la.inl"
