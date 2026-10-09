#pragma once
// Heterogeneous bank adapter. Per-racer parameters/bindings are restored by
// Bank::visit in the shared arena; contact settings/resources are cold-bound
// independently by native_racer_contact50HW::Owner.
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact50hw_fixture.h"
#include "../../src/game/vehicle_frame49w.h"

namespace native_racer_contact50HW {
// One HOST world-frame boundary only: call after ALL participating independent
// native frames finished at Ready, outside Bank::visit(), before another racer
// begins its next frame. No renderer, source query, or event consumer can run
// concurrently. The bank must contain the full frame state, as50HN already does.
//
// The source game's all-body scheduling is not reconstructed here. Impulses
// enter each native body now; accumulated source position/rotation corrections
// are consumed by its NEXT ordinary integration. Never move the scenes here.
// The secondary/chariot body and its graph/joint caches remain owned by that
// racer's subsequent normal native coupled-body step; no artificial translation
// or matching-velocity overwrite of the secondary body is performed.
//
// Each racer has its own original geometry and authored body/spring parameters.
// The bank MUST snapshot live bindings and parameters as well as frame state.
// identity[i] is a unique opaque main-body id in this isolated contact pass.
// material[i] is explicit; emptyPhysicalRootMaterial is the documented HOST
// policy for the authenticated fresh empty root, not unknown-prefix recovery. Pair order indexes bank slots directly, with no
// sorting/deduplication. A stalled/finished racer should appear only if the host
// deliberately wants it to participate; pair order is the eligibility boundary.
inline Status applyAtWorldBoundary(
    native_racer50HN::Bank& bank,Owner& contacts,
    VehicleFrameState49W& live,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,
    const std::array<std::uint32_t,maxRacers>& identity,
    const std::array<std::uint32_t,maxRacers>& material,
    const Pair* order,std::size_t pairCount,
    Receipt& receipt) {
    const auto count=bank.count();
    if(count<2||count>maxRacers||count!=contacts.count())return Status::InvalidInput;
    if(pairCount>maxRacers*(maxRacers-1)||(pairCount&&!order))return Status::InvalidInput;
    std::array<bool,maxRacers> participates{};
    for(std::size_t p=0;p<pairCount;++p){
        if(order[p].primary>=count||order[p].other>=count||order[p].primary==order[p].other)return Status::InvalidInput;
        participates[order[p].primary]=participates[order[p].other]=true;
    }
    std::array<Body,maxRacers> before{},after{};
    for(std::size_t i=0;i<count;++i){
        const bool valid=bank.visit(i,[&](){
            const auto node=static_cast<int>(bindings.lifecycle.main190);
            if(!validVehicleSceneCount49W(live,bindings)||node<0||
               node>=static_cast<int>(live.sceneNodeCount))return false;
            const auto& root=live.bodies.main.components.root;
            if(participates[i]&&(live.phase!=VehicleFramePhase49W::Ready||
               std::memcmp(&root.scene,&live.nodes[node].world100,sizeof(BodyMatrix48G))))return false;
            before[i].physical.identity=identity[i];before[i].physical.body.state=root;
            before[i].physical.body.parameters=parameters.bodies.main.body;
            before[i].physical.body.localCenter=parameters.bodies.main.centerOffset;
            before[i].material=material[i];return true;
        });
        if(!valid)return Status::InvalidInput;
    }
    Receipt staged{};
    const auto status=contacts.run(before.data(),count,order,pairCount,after.data(),staged);
    if(status!=Status::Complete)return status;
    // Check every participant BEFORE writing any participant. This catches a
    // caller that changed a racer while collecting snapshots. No retained body
    // or scene pointer crosses a visit: only copied POD snapshots do.
    for(std::size_t i=0;i<count;++i)
        if(participates[i]&&!bank.visit(i,[&](){return live.phase==VehicleFramePhase49W::Ready&&
            !std::memcmp(&live.bodies.main.components.root,&before[i].physical.body.state,sizeof(RootBodyState48H));}))
            return Status::InvalidInput;
    // All fallible work is over. Commit the exact two-body numerical response
    // fields only; transform/position/quaternion/history and terrain stay intact.
    const auto commit=[&](std::size_t i){bank.visit(i,[&](){
        auto& dst=live.bodies.main.components.root;const auto& src=after[i].physical.body.state;
        dst.body.velocity=src.body.velocity;dst.body.angularVelocity=src.body.angularVelocity;
        dst.loads=src.loads;dst.positionCorrection1a0=src.positionCorrection1a0;
        dst.rotationCorrection1b0=src.rotationCorrection1b0;
        dst.accumulator1c0=src.accumulator1c0;dst.accumulator1d0=src.accumulator1d0;
    });};
    for(std::size_t i=1;i<count;++i)if(participates[i])commit(i);if(participates[0])commit(0);
    receipt=staged;return Status::Complete;
}
}
