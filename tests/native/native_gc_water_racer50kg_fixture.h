#pragma once
#include "native_water_vehicle_consumer50jc_fixture.h"
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "../../src/game/original_wreck_components50la.h"
#include <array>
#include <cstring>
#include <limits>

// Concrete native joining of one real210770 racer to the session's real water
// and particle managers. Eight racers own eight distinct instances here; this
// object must NEVER be copied into the transient racer bank. Its controls are
// the exact19CED0 accumulators also mutated by22A620, never mirrored snapshots.
namespace native_gc_water_racer50KG {
using namespace rr::game;
enum class Status : std::uint8_t {Complete,Owner,Phase,Graph,Spray,Water,Children};
struct Result {
    Status status=Status::Owner;
    std::uint32_t sourcePC=0;
    unsigned side=0;
    OriginalGcSprayReceipt50KG spray{};
    native_water_vehicle50JC::Result water{};
    bool okay() const noexcept{return status==Status::Complete;}
};
class Owner final {
    native_water_runtime50JC::Owner* water_=nullptr;
    OriginalGcParticleManager50KG* particles_=nullptr;
    std::array<OriginalGcSprayOwner50KG,2> sprays_{};
    std::array<OriginalWreckComponent50LA,2> sprayComponents_{};
    native_water_vehicle50JC::State continuation_{};
    TerrainWaterBinding50JC terrain_{};
    float previousBE0_[4]{};
    const float* initialStep434630_=nullptr;
    std::uint32_t racerIdentity_=0,waterIdentity_=0,componentIdentity7FC_=0;
    std::uint64_t tailSequence_=0,childrenSequence_=0;
    bool initialized_=false,childrenHaveSequence_=false;
    unsigned nextChild_=0;
    float childrenDelta_=0;
    bool childrenPaused_=false;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    // Invoke at actual211E34 after the preceding non-water7FC child loads.
    // Their original resource/constructor side effects must already have run.
    // Both nonzero identities denote this actual allocated native object;
    // numeric bank indexes are not substitutes for source component ownership.
    Result constructSprays(native_water_runtime50JC::Owner& water,
        OriginalGcParticleManager50KG& particles,std::uint32_t racerIdentity,
        std::uint32_t waterIdentity,std::uint32_t actualComponent7FC,
        const float (&actualApplicationStep434630)[4],
        const std::vector<std::uint8_t>& originalPodSprayEmt,
        std::uint32_t& sharedRawRandom) noexcept {
        if(water_||!water.initialized()||!water.provider()||!water.globals()||
           !particles.prepared()||!racerIdentity||!waterIdentity||!actualComponent7FC)
            return {Status::Owner,0x211e34u};
        water_=&water;particles_=&particles;racerIdentity_=racerIdentity;
        waterIdentity_=waterIdentity;componentIdentity7FC_=actualComponent7FC;
        initialStep434630_=actualApplicationStep434630;
        //434650 is(0,1,0,1). Source211EBC NEG.S preserves negativezero XYZ
        // for the opposite side; do not replace it with a normalized vector.
        const std::array<float,3> directions[2]{{0.f,1.f,0.f},{-0.f,-1.f,-0.f}};
        for(unsigned i=0;i<2;++i){
            const auto r=sprays_[i].load(particles,originalPodSprayEmt,directions[i],sharedRawRandom);
            if(!r.okay())return {Status::Spray,i?0x211f28u:0x211e88u,i,r};
            // Canonical source141300/141960 fields. The stable cold forest
            // later supplies identities/links on these SAME subobjects.
            sprayComponents_[i].flags29=3;sprayComponents_[i].mask20=0x0402u;
            //211EB0/211F50, after the actual successful load/attachment.
            stopEffect50BC(sprays_[i].controls());
        }
        terrain_={water.provider(),water.provider(),water.globals(),true};
        return {Status::Complete,0x211f5cu};
    }
    //212E28..212E44 reads actual chariot1B4 world position after all cold
    // reset/model calls. InitialBE0 is NOT zero, body190 or a recovery pose.
    Result finishConstructor(VehicleFrameState49W& frame,
        VehicleFrameBindings49W& bindings,VehicleFrameDiagnostic49W& diagnostic) noexcept {
        if(initialized_||!water_||!sprays_[0].loaded()||!sprays_[1].loaded()||
           frame.phase!=VehicleFramePhase49W::Ready)
            return {Status::Phase,0x212e28u};
        const auto model=bindings.lifecycle.model1B4;
        const SceneGraph49P graph{frame.nodes,frame.sceneNodeCount};
        if(model==SceneNodeIndex49P::none||static_cast<std::uint32_t>(model)>=graph.count||
           !refreshSceneWorld49P(graph,model))return {Status::Graph,0x212e30u};
        std::memcpy(previousBE0_,graph.nodes[static_cast<std::uint32_t>(model)].world100.v+12,12);
        previousBE0_[3]=1.f;
        if(!bindVehicleTerrainWater50JC(frame,bindings,terrain_,diagnostic))
            return {Status::Owner,0x212078u};
        initialized_=true;return {Status::Complete,0x212e44u};
    }
    bool initialized() const noexcept{return initialized_;}
    const TerrainWaterBinding50JC& terrainBinding() const noexcept{return terrain_;}
    OriginalGcSprayOwner50KG& spray(unsigned side) noexcept{return sprays_[side];}
    OriginalWreckComponent50LA& sprayComponent(unsigned side) noexcept{return sprayComponents_[side];}
    std::uint32_t componentIdentity7FC()const noexcept{return componentIdentity7FC_;}
    Result sprayChildAfterParentDispatch(unsigned side,float delta,bool paused,
        std::uint32_t& random,SharedRandomOwner50ID& selectors,HostMonotonicClock50ID& clock) noexcept {
        if(!initialized_||side>=2)return {Status::Owner,0x141f54u,side};
        const auto r=sprays_[side].update(delta,paused,random,selectors,clock);
        if(!r.okay())return {Status::Spray,r.sourcePC,side,r};
        return {Status::Complete,0x141f60u,side,r};
    }
    // Called on the canonical frame while its actual bank visit is held.
    // The reset owner's348+1F0 float is lent, not copied from7EC/receipt.
    bool bindResetDomain(const UndertrackPhysicsDomain50EW& source,
        const float& actual348Lift1F0,UndertrackPhysicsDomain50EW& output) const noexcept {
        if(!initialized_||source.terrain330!=UndertrackHeightOwner50EW::Absent||
           source.water334!=UndertrackHeightOwner50EW::Present||
           !coherentTerrainWater50JC(terrain_))return false;
        auto next=source;next.waterProvider334=terrain_.vehicle334;
        next.waterGlobals=terrain_.globals;next.terrain348Lift1F0=&actual348Lift1F0;
        output=next;return true;
    }
    // Actual228B40 nonnull334 branch. Never call plainEffectsVehicleTail50Q
    // as well. The graph/effect/water prefix is retained on failure; source
    // phase advances only after the real22A620 consumer has fully completed.
    Result attachedTail(VehicleFrameState49W& frame,
        const VehicleFrameBindings49W& bindings) noexcept {
        if(!initialized_||bindings.terrainWater50JC!=&terrain_||
           !coherentTerrainWater50JC(terrain_))return {Status::Owner,0x228b40u};
        if(frame.phase!=VehicleFramePhase49W::AfterImpactBlock)
            return {Status::Phase,0x228b40u};
        if(continuation_.stage!=native_water_vehicle50JC::Stage::Actions){
            if(tailSequence_==std::numeric_limits<std::uint64_t>::max())
                return {Status::Owner,0x22a620u};
            ++tailSequence_;
        }
        WaterVehicleEffectInput50JC input{};
        input.query348State118=&frame.bodies.main.components.probe348.field118;
        input.component7FC=&componentIdentity7FC_;input.vehicle58=&frame.fields.vehicle58;
        input.query348WaterY164=frame.terrainProbe.waterPoint160.v+1;
        input.body340Speed34=&frame.bodies.main.cachedSpeed34;
        input.previousBE0=previousBE0_;input.initialStep434630=initialStep434630_;
        input.chariot1B4={bindings.lifecycle.model1B4,true};
        input.model1BC={bindings.visual.left1BC,true};input.model1B8={bindings.visual.right1B8,true};
        input.model1C4={bindings.visual.bottomLeft1C4,true};input.model1C0={bindings.visual.bottomRight1C0,true};
        input.waterOwner334=waterIdentity_;input.waterAttachment334Bound=true;input.table81CBound=true;
        native_water_vehicle50JC::Bindings actual{};
        actual.water=water_;actual.waterOwner334=waterIdentity_;actual.previousBE0=previousBE0_;
        actual.table81CBound=true;
        for(unsigned i=0;i<2;++i){
            input.sprays81C[i]={sprays_[i].ownerId(),&sprays_[i].controls(),true,true};
            actual.sprays81C[i]={sprays_[i].ownerId(),&sprays_[i].controls(),true,true};
        }
        const SceneGraph49P graph{frame.nodes,frame.sceneNodeCount};
        const auto r=native_water_vehicle50JC::resume(tailSequence_,racerIdentity_,graph,input,actual,continuation_);
        if(r.status!=native_water_vehicle50JC::Status::Complete&&
           r.status!=native_water_vehicle50JC::Status::Duplicate)
            return {Status::Water,r.sourcePC,0,{},r};
        frame.phase=VehicleFramePhase49W::PostPlainEffects50Q;
        return {Status::Complete,0x228b78u};
    }
    // Actual component7FC traversal slot, NOT a call after22A620 by assumption.
    // Caller must execute preceding/following real sibling emitters in their
    // original list order around this contiguous two-spray segment. The
    // sequence is the actual component38 visit, not rendered-frame count.
    // A failed derived update is terminal for this visit: replay would repeat
    // partial shared-RNG/pool mutations, so retry does not invoke it again.
    Result sprayChildren(std::uint64_t sequence,float delta,bool applicationPaused,
        std::uint32_t& sharedRawRandom,SharedRandomOwner50ID& selectors,
        HostMonotonicClock50ID& clock) noexcept {
        if(!initialized_)return {Status::Owner,0x141ed0u};
        if(childrenHaveSequence_){
            if(sequence<childrenSequence_||
               (sequence==childrenSequence_&&(std::memcmp(&delta,&childrenDelta_,4)||
                applicationPaused!=childrenPaused_)))return {Status::Children,0x141ed0u};
            if(sequence==childrenSequence_){
                if(nextChild_==2)return {Status::Complete,0x19d9e4u};
                return {Status::Children,0x19ced0u,nextChild_};
            }
            if(nextChild_!=2)return {Status::Children,0x19ced0u,nextChild_};
        }
        childrenHaveSequence_=true;childrenSequence_=sequence;childrenDelta_=delta;
        childrenPaused_=applicationPaused;nextChild_=0;
        for(;nextChild_<2;++nextChild_){
            if(!originalWreckChildEligible50LA(sprayComponents_[nextChild_]))continue;
            const auto r=sprays_[nextChild_].update(delta,applicationPaused,sharedRawRandom,selectors,clock);
            if(!r.okay())return {Status::Spray,r.sourcePC,nextChild_,r};
        }
        return {Status::Complete,0x19d9e4u};
    }
};
} // namespace native_gc_water_racer50KG
