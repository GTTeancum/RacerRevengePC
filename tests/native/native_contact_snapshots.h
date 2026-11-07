#pragma once
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact50hw_fixture.h"
#include "../../src/game/vehicle_frame49w.h"

namespace native_racer_contact50HW {
// Exact pointer-free contact inputs/outputs for applyAtWorldBoundary only.
// Same bytes Bank::visit would expose: slot0 from the live arena (the arena
// always belongs to player zero outside a lease), opponents from their saved
// snapshot. Never exposes arena or snapshot storage, invokes a callback, swaps
// a racer, or changes eligibility. Every field is resolved to one registered
// span at construction, before any gather; commit has no failure path, so no
// participant can be partially committed after verification.
class SnapshotAccess final {
    struct Field {const unsigned char* live=nullptr;std::size_t bytes=0,offset=0;};
    native_racer50HN::Bank& bank_;
    VehicleFrameState49W& live_;
    const VehicleFrameBindings49W& bindings_;
    const VehicleFrameParameters49W& parameters_;
    Field stateNodes_,bindingNodes_,main190_,root_,vehicle58_,dynamicBound_,
        body20C_,partners210_,phase_,nodes_,bodyParameters_,centerOffset_;
    // Same idle boundary as original_manager_fields::Access: never inside a
    // full, batched, manager, retirement, animation, query or terminal lease.
    void idle(std::size_t slot) const {
        const auto& b=bank_;
        if(b.retired_||!b.sealed_||slot>=b.count_||b.visiting_||b.readBlocked_||b.visitDepth_||
           b.currentSlot_||b.pendingToken_||b.managerFieldsVisit_||b.retirementVisit_||
           b.mcAnimationBlocked_||b.gbAnimationBlocked_||b.mcQueryActive_||b.gbQueryActive_||
           b.mcTerminalBlocked_||b.gbTerminalBlocked_)
            throw std::runtime_error("contact snapshots require an idle sealed Bank");
    }
    template<class T> Field bind(const T& value) const {
        static_assert(std::is_trivially_copyable_v<T>);
        const auto address=reinterpret_cast<std::uintptr_t>(&value);
        for(const auto& span:bank_.spans_){
            const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
            if(address<begin)continue;
            const auto within=address-begin;
            if(within<=span.bytes&&sizeof value<=span.bytes-within)
                return {reinterpret_cast<const unsigned char*>(&value),sizeof value,span.offset+within};
        }
        throw std::runtime_error("contact field is not registered in this Bank");
    }
    const unsigned char* source(std::size_t slot,const Field& field,std::size_t within=0) const noexcept {
        return slot?bank_.slots_[slot].bytes.data()+field.offset+within:field.live+within;
    }
    void copyOut(std::size_t slot,const Field& field,void* out,std::size_t bytes,std::size_t within=0) const noexcept {
        std::memcpy(out,source(slot,field,within),bytes);
    }
    template<class T> T load(std::size_t slot,const Field& field) const noexcept {
        T result;copyOut(slot,field,&result,sizeof result);return result;
    }
    // Writes only bytes inside an already-bound field; for slot0 the live
    // address is the same object Bank::visit(0) would have written.
    void copyIn(std::size_t slot,const Field& field,const void* in,std::size_t bytes,std::size_t within) noexcept {
        auto* target=slot?bank_.slots_[slot].bytes.data()+field.offset+within:
            const_cast<unsigned char*>(field.live)+within;
        std::memcpy(target,in,bytes);
    }
    void readList(std::size_t slot,BodyContactListState48R& list) const noexcept {
        // Signed source count bits, including unsupported negative states.
        copyOut(slot,body20C_,&list.count20C,sizeof list.count20C);
        copyOut(slot,partners210_,list.slots210.data(),partners210_.bytes);
    }
public:
    SnapshotAccess(native_racer50HN::Bank& bank,VehicleFrameState49W& live,
        const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters)
        :bank_(bank),live_(live),bindings_(bindings),parameters_(parameters) {
        static_assert(sizeof(live.bodies.main.components.body20C)>=sizeof(BodyContactListState48R::count20C));
        static_assert(sizeof(live.mainContactPartners210)==sizeof(BodyContactListState48R::slots210));
        idle(0);
        stateNodes_=bind(live.sceneNodeCount);bindingNodes_=bind(bindings.sceneNodeCount);
        main190_=bind(bindings.lifecycle.main190);root_=bind(live.bodies.main.components.root);
        vehicle58_=bind(live.fields.vehicle58);dynamicBound_=bind(live.fields.dynamicOwnersBound50IW);
        body20C_=bind(live.bodies.main.components.body20C);partners210_=bind(live.mainContactPartners210);
        phase_=bind(live.phase);nodes_=bind(live.nodes);
        bodyParameters_=bind(parameters.bodies.main.body);centerOffset_=bind(parameters.bodies.main.centerOffset);
    }
    SnapshotAccess(const SnapshotAccess&)=delete;SnapshotAccess& operator=(const SnapshotAccess&)=delete;
    // Former gather visit: identical validity rules and copied fields.
    bool gather(std::size_t slot,bool participating,bool joined,Body& body,bool& ai,bool& bound,
        BodyContactListState48R& list) const {
        idle(slot);
        const auto nodes=load<decltype(live_.sceneNodeCount)>(slot,stateNodes_);
        const auto bindingNodes=load<decltype(bindings_.sceneNodeCount)>(slot,bindingNodes_);
        const auto node=static_cast<int>(load<decltype(bindings_.lifecycle.main190)>(slot,main190_));
        // validVehicleSceneCount49W(live,bindings) on the selected slot.
        if(!nodes||nodes>vehicleFrameNodeCount49W||nodes!=bindingNodes||node<0||node>=static_cast<int>(nodes))return false;
        const auto root=load<RootBodyState48H>(slot,root_);
        ai=load<decltype(live_.fields.vehicle58)>(slot,vehicle58_)==1u;
        if(joined){bound=load<decltype(live_.fields.dynamicOwnersBound50IW)>(slot,dynamicBound_);readList(slot,list);}
        if(participating){
            BodyMatrix48G scene;
            copyOut(slot,nodes_,&scene,sizeof scene,static_cast<std::size_t>(
                reinterpret_cast<const unsigned char*>(&live_.nodes[node].world100)-nodes_.live));
            if(load<VehicleFramePhase49W>(slot,phase_)!=VehicleFramePhase49W::Ready||
               std::memcmp(&root.scene,&scene,sizeof(BodyMatrix48G)))return false;
        }
        body.physical.body.state=root;
        body.physical.body.parameters=load<decltype(parameters_.bodies.main.body)>(slot,bodyParameters_);
        body.physical.body.localCenter=load<decltype(parameters_.bodies.main.centerOffset)>(slot,centerOffset_);
        return true;
    }
    // Former verify visit: same phase/root/active/bound/raw20C/210 comparison.
    bool unchanged(std::size_t slot,const RootBodyState48H& before,
        const BodyContactListState48R& list,bool joined) const {
        idle(slot);
        if(load<VehicleFramePhase49W>(slot,phase_)!=VehicleFramePhase49W::Ready||
           std::memcmp(source(slot,root_),&before,sizeof(RootBodyState48H)))return false;
        if(joined&&(!bank_.active(slot)||!load<decltype(live_.fields.dynamicOwnersBound50IW)>(slot,dynamicBound_)||
           std::memcmp(source(slot,body20C_),&list.count20C,sizeof list.count20C)||
           std::memcmp(source(slot,partners210_),list.slots210.data(),partners210_.bytes)))return false;
        return true;
    }
    // Former commit visit: exactly the response fields, 238/23C and joined
    // 20C/210. Transform, position, quaternion, history and terrain untouched.
    // Call only after every participant passed unchanged() in this same pass.
    void commit(std::size_t slot,const RootBodyState48H& src,const BodyContactListState48R& list,bool joined) noexcept {
        // A member's offset within src addresses the same member of the bound root.
        const auto at=[&](const auto& member){return static_cast<std::size_t>(
            reinterpret_cast<const unsigned char*>(&member)-reinterpret_cast<const unsigned char*>(&src));};
        const auto write=[&](const auto& member){copyIn(slot,root_,&member,sizeof member,at(member));};
        write(src.body.velocity);write(src.body.angularVelocity);write(src.loads);
        write(src.positionCorrection1a0);write(src.rotationCorrection1b0);
        write(src.accumulator1c0);write(src.accumulator1d0);
        // Canonical processed-body list is independent of20C/210 damage join.
        write(src.callbackState238);write(src.processedContactPartners23C);
        if(joined){
            copyIn(slot,body20C_,&list.count20C,sizeof list.count20C,0);
            copyIn(slot,partners210_,list.slots210.data(),partners210_.bytes,0);
        }
    }
};
}
