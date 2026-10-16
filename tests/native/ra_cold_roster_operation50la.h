#pragma once
#include "ra_cold_roster_resources50la.h"
#include "ra_cold_aggregate50la.h"
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

// Existing main-TU instrumentation; no second allocator or counter is defined.
extern thread_local std::uint64_t allocations;
namespace ra_cold_roster50LA {
struct Diagnostic {std::size_t slot=0;const char* stage=nullptr;const char* failure=nullptr;std::string detail;};
// Captures the actual constructor records before initial terrain enable. No
// setter accepts supplied replacement probe records or a readiness boolean.
class FreshProbe final {
    friend class Operation;
    const g::VehicleFrameState49W* frame_;
    g::VehicleTerrainProbe50FP terrain_;
    g::DisabledProbeState49K main_;
    std::uint32_t domain_;
    explicit FreshProbe(const g::VehicleFrameState49W& s)
        :frame_(&s),terrain_(s.terrainProbe),main_(s.bodies.main.components.probe348),domain_(s.bodies.main.domain.probe100){}
public:
    static std::shared_ptr<const FreshProbe> capture(const g::VehicleFrameState49W& s){
        if(s.phase!=g::VehicleFramePhase49W::Ready||s.bodies.main.frameFactor!=0.f||
           s.terrainProbe.enabled||s.bodies.main.domain.probe100)
            throw std::runtime_error("actual pre-enable probe constructor records unavailable");
        return std::shared_ptr<const FreshProbe>(new FreshProbe(s));
    }
};
// Explicit stable arena borrows. This wrapper is never Bank-snapshotted. All
// persistent referred-to lanes must already be registered, with fixed capacity.
struct Arena {
    runtime_startup50fl::Fixture& live;
    native_racer50HW::HandlingTuning tuning;
    const g::AuthoredSelectionContext49N& selection;
    g::AiRuntimeState50IV& ai;
    native_ai50IV::Session& aiSession;
    g::TrackNavigationProgress50EU& navigation;
    lap_drive50GJ::Continuity& lap;
    g::BasePoseHistoryState50Y& history;
    g::PodSceneRebaseModel50FB (&rebaseModels)[5];
    unsigned& rebaseCount;
    g::PodSceneRebaseWorkspace50FB& rebaseWorkspace;
    g::ModelBoundsResult49O (&rebaseBounds)[5];
    g::PodSceneRebaseDiagnostic50FB& rebaseDiagnostic;
    a::EffectNames50BL& effectNames;
    g::EffectOwner50BE (&effects)[14];
    std::size_t& effectCount;
    g::ResetEffectBindings50BK& resetBindings;
    g::ResetEffectState50BK& resetState;
    g::ResetEffectWorkspace50BK& resetWorkspace;
    native_original_ai_recovery::Models& recoveryModels;
    const terrain_probe_authoring50fq_fixture::SourceBindings& terrainBindings;
    a::PodFlameOwners50W& flame;
    // Real application launch kind, not a caller's invented race phase.
    const std::uint32_t& launchKind11258;
};
class Operation final {
    friend class ra_cold_aggregate50LA::Owner;
    using Model=native_racer_model50LA::Owner;
    std::shared_ptr<ra_cold_aggregate50LA::Owner> aggregate_;
    std::shared_ptr<const Resources> resources_;
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation_;
    std::shared_ptr<const FreshProbe> probe_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> entities_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<g::OriginalMotionLibrary50LA> motions_;
    const original_main_entry50KG::Application* application_;
    native_racer50HN::Bank* bank_;
    const std::uint64_t generation_;
    Arena arena_;
    // Real objects, retained across every partial stage and Bank save.
    std::array<std::unique_ptr<Model>,8> right_,left_,bottomRight_,bottomLeft_,chariot_;
    std::array<std::unique_ptr<native_racer50HW::Visual>,8> visuals_;
    std::array<std::unique_ptr<native_racer50HW::Cables>,8> cables_;
    std::array<native_racer_contact50HW::Binding,8> contacts_{};
    std::array<bool,8> attempted_{},completed_{};
    std::size_t nextSlot_=0;
    // Immutable source closure retained even after a later partial failure.
    original_racer_material_sources50LA::Materials modelMaterials_;
    original_racer_material_sources50LA::Textures modelTextures_;
    bool terminal_=false,closed_=false;
    Operation(std::shared_ptr<ra_cold_aggregate50LA::Owner> owner,
        const original_main_entry50KG::Application& app,native_racer50HN::Bank& bank,
        const Arena& arena,std::shared_ptr<const FreshProbe> probe)
        :aggregate_(std::move(owner)),resources_(aggregate_->runtimeQueries_->rosterResources_),
         navigation_(aggregate_->navigationLease_),probe_(std::move(probe)),entities_(app.entities()),
         domain_(app.standardMaterialDomain()),motions_(app.motionLibrary4333C0()),application_(&app),bank_(&bank),
         generation_(aggregate_->trackParticleGeneration_),arena_(arena){}
    static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
    auto& runtime()const noexcept{return *aggregate_->runtimeQueries_;}
    bool registered()const noexcept {
        auto& r=runtime();auto& a=arena_;auto& x=a.live;
        if(!r.cameraBankCurrent()||r.bankIdentity_!=bank_||!r.actual_||&r.actual_->frame!=&x.state||
           !r.actual_->recovery||!r.actual_->movement||!resources_)return false;
        const ra_source_camera50LA::GridArenaOwners grid{x.bindings,x.parameters,a.ai,a.history,resources_->rigs_};
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
    static std::shared_ptr<Operation> bind(std::shared_ptr<ra_cold_aggregate50LA::Owner> aggregate,
        const original_main_entry50KG::Application& app,native_racer50HN::Bank& bank,
        const Arena& arena,std::shared_ptr<const FreshProbe> probe,Diagnostic& d)noexcept {
        d={};d.stage="retained cold binding";
        try {
            need(aggregate&&!aggregate->terminal_&&!aggregate->closed_&&aggregate->runtimeQueries_&&
                aggregate->prefix_.runtimeQueryWorldOwners&&aggregate->prefix_.trackParticleManager&&
                !aggregate->worldImagesAttempted_&&aggregate->rosterMountComplete_&&
                aggregate->rosterMountApplication_==&app&&
                aggregate->rosterMountDomain_==app.standardMaterialDomain()&&
                aggregate->trackParticleHostApplication_==&app&&
                aggregate->trackParticleDomain_==app.standardMaterialDomain()&&
                aggregate->trackParticleApplication_==app.entities()&&aggregate->trackParticleGeneration_&&
                probe&&probe->frame_==&arena.live.state,"actual aggregate/Application/cold probe ownership differs");
            auto& r=*aggregate->runtimeQueries_;
            need(r.rosterResources_&&r.cameraBankCurrent()&&r.bankIdentity_==&bank&&!r.cameraAttempted_&&
                !r.terminal_&&!r.closed_,"actual sealed Bank/roster binding absent");
            auto pin=std::static_pointer_cast<ra_source_camera50LA::OwnershipBridge::BankPin>(r.bankLifetime_.lock());
            need(pin&&!pin->rosterOperation&&!r.rosterRetirementRequired_,"roster operation cannot replace a surviving owner");
            ra_cold_aggregate50LA::Diagnostic ad;
            auto navigation=aggregate->borrowColdNavigation(ad);
            need(bool(navigation),"same-source navigation lease unavailable");
            auto out=std::shared_ptr<Operation>(new Operation(aggregate,app,bank,arena,std::move(probe)));
            // No model or arena effects yet; rejected binding must be destroyable.
            out->closed_=true;
            need(out->registered(),"actual cold constructor lanes are not registered");
            need(out->domain_&&out->domain_->prepared()&&out->motions_&&
                !out->domain_->materials382B38()->coldCollectorCurrent(out->domain_->coldCollector14D440()),
                "cold model construction cannot invalidate a current collector");
            need(out->navigation_->freshSelection().bound&&
                arena.selection.inferredClass==out->navigation_->freshSelection().actualRaceClass&&
                !arena.selection.applyProfileOverwrite&&arena.selection.configurationEntryNonzero,
                "actual all-AI RA handling selection differs");
            need(r.inputs_.collisionRoster.size()==8u,"actual complete collision roster required");
            for(unsigned i=0;i<8;++i)need(r.inputs_.collisionRoster[i]==out->resources_->collisions_[i].get()&&
                bank.completedSteps(i)==0,
                "reserved collision roster differs from retained owners");
            pin->rosterOperation=out;out->closed_=false;r.rosterRetirementRequired_=true;return out;
        }catch(const std::exception& e){d.failure="cold roster binding failed";try{d.detail=e.what();}catch(...){}return {};}
        catch(...){d.failure="cold roster binding exception";return {};}
    }
    bool constructSlot(native_racer50HN::Bank& actualBank,std::size_t slot,Diagnostic& d)noexcept {
        d={};d.slot=slot;d.stage="constructor entry";
        if(terminal_||closed_||&actualBank!=bank_||slot>=8||slot!=nextSlot_||attempted_[slot]||!registered()){
            terminal_=true;if(aggregate_&&aggregate_->runtimeQueries_)runtime().rosterSlotComplete_.fill(false);
            d.failure="cold roster operation is terminal, replaced or not registered";return false;}
        auto& r=runtime();
        if(r.terminal_||r.cameraAttempted_||r.gridAttempted_[slot]||actualBank.completedSteps(slot)||
           domain_->materials382B38()->coldCollectorCurrent(domain_->coldCollector14D440())){
            terminal_=true;r.rosterSlotComplete_.fill(false);d.failure="later world construction has already begun";return false;}
        attempted_[slot]=true;
        // Unbanked obligation precedes the first possible model Entity effect.
        r.rosterRetirementRequired_=true;
        try {
            auto& x=arena_.live;auto& s=x.state;auto& q=*r.actual_;
            const auto& rig=*resources_->rigs_[slot];const auto& collision=*resources_->collisions_[slot];
            actualBank.visit(slot,[&](){
                need(s.phase==g::VehicleFramePhase49W::Ready&&s.bodies.main.frameFactor==0.f&&
                    !q.recovery->sourcePrepared&&!s.shape.registrationE0,
                    "actual initial Ready/recovery constructor state required");
                d.stage="rig installation";
                auto model=[&](std::unique_ptr<Model>& out,unsigned role){
                    out=std::make_unique<Model>();
                    out->prepareEarly(rig,slot,generation_,actualBank,x,entities_,domain_,motions_,
                        application_->modelDirectory36B860(),role);
                };
                model(right_[slot],native_racer50HW::Rig::right);
                model(left_[slot],native_racer50HW::Rig::left);
                if(rig.modelCount==5u){
                    model(bottomRight_[slot],native_racer50HW::Rig::bottomRight);
                    model(bottomLeft_[slot],native_racer50HW::Rig::bottomLeft);
                }
                model(chariot_[slot],native_racer50HW::Rig::chariot);
                right_[slot]->bindSibling(*left_[slot]);
                chariot_[slot]->bindChariotSiblings(*right_[slot],*left_[slot],bottomRight_[slot].get(),bottomLeft_[slot].get());
                rig.install(s,x.bindings,x.handRecords,right_[slot].get(),left_[slot].get(),chariot_[slot].get(),
                    bottomRight_[slot].get(),bottomLeft_[slot].get());
                auto selection=arena_.selection;selection.mode58Nonzero=true;
                d.stage="handling bind";
                const auto receipt=native_racer50HW::bindColdHandling(rig.phy,rig.hierarchy[1],rig.hierarchy[0],
                    rig.geometry,collision.mainTarget().source,selection,x,arena_.tuning,
                    rig.modelCount==5?&rig.hierarchy[native_racer50HW::Rig::bottomRight]:nullptr,
                    rig.modelCount==5?&rig.hierarchy[native_racer50HW::Rig::bottomLeft]:nullptr,chariot_[slot].get());
                d.stage="original AI construction";
                const auto& text=resources_->aiSources_[slot];g::AiProfile50IU profile{};
                rr::assets::AiAuthoringDiagnostic50IU aiDiagnostic{};
                need(rr::assets::readAiProfile50IU(reinterpret_cast<const char*>(text.data()),text.size(),profile,aiDiagnostic),
                    "actual selected original AI profile rejected");
                const auto& selected=navigation_->freshSelection();
                need(selected.bound&&g::applyAiCreateClass50IU(selected.actualRaceClass,profile),"actual RA AI class unavailable");
                const auto* identity=g::original_racer_selection50IJ::findByPodId(rig.podId);
                need(identity,"actual original racer60 absent");g::AiCreateIdentity50IU aiIdentity{};
                need(g::bindAiCreateIdentity50IU(static_cast<std::uint32_t>(slot),0,
                    static_cast<std::int32_t>(identity->racer60),selected.freshCourseRecordPod11280,aiIdentity),
                    "actual all-AI creation identity rejected");
                g::initializeAiRuntime50IV(aiIdentity.ordinalFE0,identity->racer60,
                    static_cast<std::uint32_t>(selected.freshCourseRecordPod11280),profile,receipt.authored.engineDifference5C,arena_.ai);
                need(arena_.ai.fieldFE4==aiIdentity.courseRecordMatchFE4,"original FE4 differs");
                arena_.aiSession.packingReferences[slot]=-1;
                g::AiColdTrackBindings50IU tracks{};
                need(navigation_->bindColdTracks(arena_.navigation,tracks),"same-source RA cold tracks unavailable");
                s.fields.vehicle58=1u;s.fields.nativeAiOwnersBound50IV=true;s.fields.applicationByte58=0u;
                x.parameters.rates.x=4.f;
                g::initializeLapTimingFields50ID(arena_.lap.timing,arena_.launchKind11258);arena_.lap.timingInitialized=true;
                d.stage="handling root placement";native_racer50HW::placeColdHandlingRoots(x);
                d.stage="model rebase";arena_.rebaseCount=rig.modelCount;
                for(unsigned i=0;i<arena_.rebaseCount;++i)arena_.rebaseModels[i]=rig.rebaseModels[i];
                need(g::rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},arena_.rebaseModels,arena_.rebaseCount,
                    arena_.rebaseWorkspace,arena_.rebaseBounds,arena_.rebaseDiagnostic),"actual model rebase failed");
                d.stage="effect name binding";rig.buildEffectNames(s,arena_.effectNames);
                d.stage="effect owner construction";effects();
                d.stage="query resource and native identity assignment";
                const auto& reserved=r.bodyIdentities_[slot];
                need(reserved.collision==&collision&&reserved.main&&reserved.secondary,"actual native reservation differs");
                q.main.resource=&collision.main();q.secondary.resource=&collision.secondary();
                q.mainFilter.identity=reserved.main;q.secondaryFilter.identity=reserved.secondary;
                terrain_probe_authoring50fq_fixture::Owner authoredProbe{};std::string error;
                d.stage="terrain probe authoring";
                const bool probeAuthored=terrain_probe_authoring50fq_fixture::prepare(authoredProbe,q.main,x.parameters,
                    rig.phy,arena_.terrainBindings,error,collision);
                need(probeAuthored,error.empty()?"original probe authoring failed":error.c_str());
                for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=authoredProbe.lowerEndpoint140.v[axis];
                x.parameters.bodies.main.probeLocal140[3]=1.f;
                d.stage="terrain probe installation";
                const bool probeInstalled=terrain_probe_authoring50fq_fixture::installProbeCold(q.terrain,q.terrainLive,authoredProbe,error);
                need(probeInstalled,error.empty()?"original probe installation failed":error.c_str());
                d.stage="selected recovery construction";
                const bool recoveryBound=undertrack_owners50ew_fixture::bindSelectedCold(*q.recovery,collision,q.main,q.secondary,
                    x.parameters,rig.phy,error);
                need(recoveryBound,error.empty()?"original recovery source bind failed":error.c_str());
                const std::uint32_t mainExclusions[]={q.secondaryFilter.identity,q.recovery->podFilter.identity,
                    q.movement->filter.identity,q.terrain.metadata.filter.identity};
                const std::uint32_t secondaryExclusions[]={q.mainFilter.identity,q.recovery->podFilter.identity,q.movement->filter.identity};
                std::copy(std::begin(mainExclusions),std::end(mainExclusions),q.mainFilter.identityLists[0].identities.begin());
                std::copy(std::begin(secondaryExclusions),std::end(secondaryExclusions),q.secondaryFilter.identityLists[0].identities.begin());
                need(native_original_ai_recovery::bindColdModels(s,x.bindings,arena_.rebaseModels,arena_.rebaseCount,
                    arena_.recoveryModels),"actual selected reset models rejected");
            });
            // Observe the saved lanes only after the true constructor visit.
            const auto main=actualBank.bindRead(q.mainFilter.identity),secondary=actualBank.bindRead(q.secondaryFilter.identity);
            need(actualBank.read(slot,main)==r.bodyIdentities_[slot].main&&
                actualBank.read(slot,secondary)==r.bodyIdentities_[slot].secondary,"actual saved native identities differ");
            d.stage="actual pod context binding";ra_cold_aggregate50LA::Diagnostic ad;
            need(aggregate_->bindRuntimePodLiveQuerySlot(actualBank,slot,x.bindings,x.parameters,ad),
                ad.failure?ad.failure:"actual RA pod context bind failed");
            actualBank.visit(slot,[&](){
                d.stage="terrain probe enable";
                need(s.phase==g::VehicleFramePhase49W::Ready&&s.bodies.main.frameFactor==0.f&&
                    x.bindings.podLiveQuery50IZ==r.podContext_.get(),"actual finishing slot state differs");
                s.terrainProbe=probe_->terrain_;s.bodies.main.components.probe348=probe_->main_;s.bodies.main.domain.probe100=probe_->domain_;
                need(g::enableVehicleTerrain50FP(s,x.bindings,q.terrain.initial,q.terrain.follow,x.workspace,x.diagnostic),
                    "original terrain enable failed");
                d.stage="visual construction";
                visuals_[slot]=std::make_unique<native_racer50HW::Visual>(".",
                    runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/original-eight-visuals"),rig);
                d.stage="cable construction";cables_[slot]=std::make_unique<native_racer50HW::Cables>(rig,s,x.bindings);
                d.stage="flame construction";
                arena_.flame=native_racer50HW::buildColdFlames(rig,visuals_[slot]->drawAssets,x.bindings);
                contacts_[slot]={&collision,arena_.tuning.mainSpring,arena_.tuning.mainResponse};
            });
            completed_[slot]=true;r.rosterSlotComplete_[slot]=true;++nextSlot_;return true;
        }catch(const std::exception& e){terminal_=true;r.terminal_=true;r.rosterSlotComplete_.fill(false);
            d.failure="cold constructor failed; actual prefix retained";try{d.detail=e.what();}catch(...){}return false;}
        catch(...){terminal_=true;r.terminal_=true;r.rosterSlotComplete_.fill(false);d.failure="cold constructor exception; real partial prefixes retained";return false;}
    }
    // Cold terminal teardown only. No borrowed arena/Bank may die first.
    // A later camera/grid/fresh stage must retire through its own real API.
    bool close()noexcept {
        if(closed_)return true;
        auto& r=runtime();if(!r.cameraBankCurrent()||r.bankIdentity_!=bank_)return false;
        if(!r.cameraRetiredForRoster())return false;
        for(unsigned i=0;i<8;++i)if(r.gridRetirementRequired_[i]||r.freshRetirementRequired_[i])return false;
        // Model::close owns its actual Bank visit. Never nest it in one here.
        // Reverse source role order; partial successes remain retained/closed.
        for(std::size_t slot=8;slot-->0;){
            for(auto* model:{&chariot_[slot],&bottomLeft_[slot],&bottomRight_[slot],&left_[slot],&right_[slot]})
                if(*model&&!(*model)->close())return false;
        }
        // Visual/flame source views can remain in Bank snapshots. Retain all
        // backing through pin destruction; only mark actual model retirement.
        r.rosterSlotComplete_.fill(false);r.rosterRetirementRequired_=false;closed_=true;return true;
    }
};
} // namespace ra_cold_roster50LA

namespace ra_cold_aggregate50LA {
inline bool Owner::mountRetainedRuntimeRosterArchives(
    const original_main_entry50KG::Application& app,Diagnostic& d)noexcept {
    d={};d.stage=Stage::runtimeQueryStorage;
    if(terminal_||closed_||rosterMountAttempted_||worldAttempted_||trackParticleAttempted_||
       !runtimeQueries_||!prefix_.runtimeQueryStorage||!runtimeQueries_->rosterResources_)
        return fail(d,"roster archive mounting requires retained fresh storage before world staging");
    rosterMountAttempted_=true;
    try {
        rosterMountApplication_=&app;rosterMountDomain_=app.standardMaterialDomain();
        const auto& domain=rosterMountDomain_;
        if(!domain||!domain->prepared()||
           domain->materials382B38()->coldCollectorCurrent(domain->coldCollector14D440()))
            return fail(d,"roster archive mount cannot mutate a current collector domain");
        const auto& provider=domain->archiveProvider3E1A70();
        if(!provider)return fail(d,"actual Application archive provider missing");
        for(std::size_t slot=0;slot<8;++slot){
            d.rosterSlot=slot;
            const auto& rig=runtimeQueries_->rosterResources_->rigs_[slot];
            if(!rig)return fail(d,"retained original Rig missing during archive mount");
            // The existing Archive authenticates the original container against
            // this retained Rig and mounts complete bytes into the SAME provider.
            // This is native cold namespace staging, not model construction.
            original_racer_animation_setup50LA::Archive archive(*rig);
            if(!archive.mount(*provider,d.rosterMount))
                return fail(d,"actual roster archive mount rejected; earlier mounts retained");
        }
        if(!provider->sealNativeMounts(d.rosterMount))return fail(d,"actual roster namespace sealing rejected");
        rosterMountComplete_=true;return true;
    }catch(...){return fail(d,"roster archive exception; actual partial provider retained");}
}
inline bool Owner::prepareRetainedRuntimeModelResources(native_racer50HN::Bank& bank,
    Diagnostic& d)noexcept {
    d={};d.stage=Stage::runtimeQueryWorldOwners;
    auto op=retainedRosterForHandoff(bank,d);if(!op)return false;
    auto& r=*runtimeQueries_;
    if(rosterModelResourcesAttempted_||worldImagesAttempted_||!prefix_.staticWorld||!world_||
       world_->domain_!=op->domain_||!rosterMountComplete_||rosterMountApplication_!=op->application_)
        return failRetainedHandoff(d,"model resource completion requires the same unpopulated world domain once");
    for(unsigned i=0;i<8;++i)if(!r.freshComplete_[i])
        return failRetainedHandoff(d,"model resource completion follows every genuine cold fresh membership");
    rosterModelResourcesAttempted_=true;
    try {
        const auto& domain=op->domain_;const auto& provider=domain->archiveProvider3E1A70();
        const auto& directory=op->application_->materialDirectory36B970();
        if(directory!="textures\\"||!provider||!provider->sealed()||
           provider->revision()!=world_->providerRevision_||
           domain->materials382B38()->coldCollectorCurrent(domain->coldCollector14D440()))
            return failRetainedHandoff(d,"model source directory/provider revision or population order differs");
        using Rig=native_racer50HW::Rig;
        for(std::size_t slot=0;slot<8;++slot){
            d.rosterSlot=slot;
            bank.visit(slot,[&](){
                const auto* rig=op->resources_->rigs_[slot].get();
                ra_cold_roster50LA::Operation::need(rig&&(rig->modelCount==3||rig->modelCount==5)&&
                    bool(op->bottomRight_[slot])==(rig->modelCount==5)&&
                    bool(op->bottomLeft_[slot])==(rig->modelCount==5),"actual retained lower model topology differs");
                for(const auto role:{Rig::right,Rig::left,Rig::bottomRight,Rig::bottomLeft,Rig::chariot}){
                    if(role>=rig->modelCount)continue;
                    d.rosterRole=role;
                    auto* model=(role==Rig::right?op->right_:role==Rig::left?op->left_:
                        role==Rig::bottomRight?op->bottomRight_:role==Rig::bottomLeft?op->bottomLeft_:op->chariot_)[slot].get();
                    ra_cold_roster50LA::Operation::need(model&&op->visuals_[slot]&&model->actualRig()==rig&&
                        model->modelRole()==role&&model->materialDomain()==domain.get(),"actual retained model owner differs");
                    model->completeResources1538B0(directory);
                    const auto* geometry=model->modelGeometry();const auto* refs=model->materialReferences();
                    ra_cold_roster50LA::Operation::need(geometry&&geometry->payload()&&refs&&
                        refs->table382B38()==domain->materials382B38().get(),"actual completed resource/reference missing");
                    ra_cold_roster50LA::Operation::need(original_racer_material_sources50LA::appendModelSources(
                        *geometry->payload(),*refs,directory,*provider,op->modelMaterials_,op->modelTextures_,d.rosterSources),
                        "actual model PSM/PST closure rejected");
                }
            });
        }
        if(provider->revision()!=world_->providerRevision_||!provider->sealed()||
           !domain->appendOriginalSources(op->modelMaterials_,op->modelTextures_,d.world.material))
            return failRetainedHandoff(d,"actual roster source corpus append rejected; model prefixes retained");
        // No14D370/29E340/14D440 here. Existing world image stage performs the
        // single later combined population and records its genuine collector.
        rosterModelResourcesComplete_=true;return true;
    }catch(...){return failRetainedHandoff(d,"model resource completion exception; retained partial model effects require close");}
}
inline bool Owner::failRetainedHandoff(Diagnostic& d,const char* why)noexcept {
    if(runtimeQueries_){
        auto& r=*runtimeQueries_;
        // No Bank dereference: this also works after a lifetime rejection.
        if(auto base=r.bankLifetime_.lock()){
            auto pin=std::static_pointer_cast<ra_source_camera50LA::OwnershipBridge::BankPin>(base);
            if(pin->owner.get()==&r&&pin->rosterOperation){
                pin->rosterOperation->terminal_=true;
                pin->rosterOperation->completed_.fill(false);
            }
        }
        r.rosterSlotComplete_.fill(false);
        // Preserve the algorithm's more specific diagnostic on partial failure.
        const auto* detail=d.runtimeQueries.message;
        r.fail(d.runtimeQueries,detail?detail:why);
    }
    return fail(d,why);
}
inline std::shared_ptr<ra_cold_roster50LA::Operation> Owner::retainedRosterForHandoff(
    native_racer50HN::Bank& bank,Diagnostic& d)noexcept {
    try {
    if(terminal_||closed_||!source_||!prefix_.runtimeQueryWorldOwners||
       !prefix_.trackParticleManager||!trackParticles_||!runtimeQueries_){
        failRetainedHandoff(d,"retained handoff requires the actual open aggregate and manager");return {};
    }
    auto& r=*runtimeQueries_;
    // cameraBankCurrent locks the destruction-bound pin before touching Bank.
    if(r.terminal_||r.closed_||r.cameraAttempted_||!r.cameraBankCurrent()||r.bankIdentity_!=&bank){
        failRetainedHandoff(d,"retained handoff requires the same live idle eight-slot Bank");return {};
    }
    auto pin=std::static_pointer_cast<ra_source_camera50LA::OwnershipBridge::BankPin>(r.bankLifetime_.lock());
    auto op=pin?pin->rosterOperation:nullptr;
    if(!pin||pin->identity!=&bank||pin->owner.get()!=&r||!op||
       op->aggregate_.get()!=this||op->bank_!=&bank||op->resources_!=r.rosterResources_||
       !op->resources_||op->navigation_!=navigationLease_||op->terminal_||op->closed_||
       op->nextSlot_!=8u||!r.rosterRetirementRequired_||
       !op->application_||op->application_!=trackParticleHostApplication_||
       !op->generation_||op->generation_!=trackParticleGeneration_||
       op->entities_!=trackParticleApplication_||op->domain_!=trackParticleDomain_||
       op->application_->entities()!=op->entities_||
       op->application_->standardMaterialDomain()!=op->domain_||
       op->application_->motionLibrary4333C0()!=op->motions_||!op->registered()){
        failRetainedHandoff(d,"retained handoff owner, generation or registered arena differs");return {};
    }
    for(unsigned i=0;i<8;++i){
        if(!op->completed_[i]||!r.rosterSlotComplete_[i]||bank.completedSteps(i)!=0){
            failRetainedHandoff(d,"all eight genuine cold constructors must complete before handoff");return {};
        }
    }
    return op;
    }catch(...){
        failRetainedHandoff(d,"retained handoff Application/owner validation threw; prefixes retained");return {};
    }
}
inline bool Owner::placeRetainedRuntimeGridSlot(native_racer50HN::Bank& bank,
    std::size_t slot,Diagnostic& d)noexcept {
    d={};d.stage=Stage::runtimeQueryWorldOwners;
    auto op=retainedRosterForHandoff(bank,d);if(!op)return false;
    auto& r=*runtimeQueries_;
    if(slot>=8u)return failRetainedHandoff(d,"retained grid slot is outside the actual roster");
    for(unsigned i=0;i<8;++i){
        if(r.freshAttempted_[i]||
           (i<slot?(!r.gridAttempted_[i]||!r.gridCompletion_[i].complete):r.gridAttempted_[i]))
            return failRetainedHandoff(d,"retained grid requires completed slots in original order before any fresh attachment");
    }
    auto& a=op->arena_;
    const ra_source_camera50LA::GridArenaOwners input{
        a.live.bindings,a.live.parameters,a.ai,a.history,op->resources_->rigs_};
    // The real algorithm owns its Bank visit and records completion only after
    // actual placement/terrain/query attachment. Never nest another visit.
    if(!r.placeGrid(bank,slot,input,d.runtimeQueries))
        return failRetainedHandoff(d,"retained grid failed; actual partial memberships remain owned");
    return true;
}
inline bool Owner::attachRetainedRuntimeFreshSlot(native_racer50HN::Bank& bank,
    std::size_t slot,const ra_source_camera50LA::FreshRecoveryArenaOwners& recovery,
    Diagnostic& d)noexcept {
    d={};d.stage=Stage::runtimeQueryWorldOwners;
    auto op=retainedRosterForHandoff(bank,d);if(!op)return false;
    auto& r=*runtimeQueries_;
    if(slot>=8u||!prefix_.lighting||!lighting_)
        return failRetainedHandoff(d,"retained fresh slot requires actual same-source lighting");
    for(unsigned i=0;i<8;++i){
        if(!r.gridAttempted_[i]||!r.gridCompletion_[i].complete||
           (i<slot?(!r.freshAttempted_[i]||!r.freshComplete_[i]):r.freshAttempted_[i]))
            return failRetainedHandoff(d,"retained fresh requires all completed grids then original fresh slot order");
    }
    // Recovery endpoints/metadata remain the actual registered borrowed arena.
    // attachFresh proves those identities and retains the borrows before effects.
    if(!r.attachFresh(bank,slot,lighting_,&recovery,d.runtimeQueries))
        return failRetainedHandoff(d,"retained fresh failed; actual visibility/recovery prefixes remain owned");
    return true;
}
inline bool Owner::prepareRuntimeRosterQueryStorage(
    std::shared_ptr<const ra_cold_roster50LA::Resources> roster,
    undertrack_owners50ew_fixture::Owners* recovery,
    const undertrack_owners50ew_fixture::SourceGlobals& recoveryGlobals,
    const terrain_probe_authoring50fq_fixture::Owner& terrain,const g::Type3QueryInput48P& movement,
    const std::vector<ra_source_camera50LA::WreckReservation>& wreck,Diagnostic& d)noexcept {
    d={};
    if(!roster||!recovery||runtimeStorageAttempted_||registryAttempted_||terminal_||closed_)
        return fail(d,"RA actual roster reservation requires fresh same-source ownership");
    try {
        if(!terrain.prepared||!recoveryGlobals.terrainIdentity348||!recoveryGlobals.identity[0]||
           !recoveryGlobals.identity[1]||terrain.metadata.filter.identity!=recoveryGlobals.terrainIdentity348||
           movement.filter.identity!=recoveryGlobals.identity[1]||
           recoveryGlobals.identity[0]==recoveryGlobals.identity[1]||
           recoveryGlobals.identity[0]==recoveryGlobals.terrainIdentity348||
           recoveryGlobals.identity[1]==recoveryGlobals.terrainIdentity348)
            return fail(d,"actual planned recovery/terrain/movement namespace differs");
        ra_source_camera50LA::RuntimeQueryCapacityInputs input;
        for(const auto& owner:roster->collisions_)input.collisionRoster.push_back(owner.get());
        input.recovery=recovery;input.wreck=wreck;
        // Retain the exact objects even if Storage preparation has partial effects.
        // Construct here rather than calling the public raw-borrow route, which
        // would publish the bridge before its strong roster retention exists.
        if(!enter(d,Stage::runtimeQueryStorage,runtimeStorageAttempted_,prefix_.staticCandidates))return false;
        runtimeQueries_.reset(new ra_source_camera50LA::OwnershipBridge());
        runtimeQueries_->rosterResources_=std::move(roster);
        runtimeQueries_->rosterTerrain_=&terrain;runtimeQueries_->rosterMovement_=&movement;
        runtimeQueries_->rosterAuxiliaryIds_={recoveryGlobals.terrainIdentity348,
            recoveryGlobals.identity[0],recoveryGlobals.identity[1]};
        if(!runtimeQueries_->constructStorage(candidates_,input,d.runtimeQueries))
            return fail(d,"RA retained roster Storage preparation failed; prefix retained");
        prefix_.runtimeQueryStorage=true;return true;
    }catch(...){return fail(d,"RA retained roster reservation exception");}
}
} // namespace ra_cold_aggregate50LA
