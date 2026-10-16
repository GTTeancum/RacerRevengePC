#pragma once
#include "authored_runtime_ground50cp_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "base_pose_history50y.h"
#include "pod_scene_rebase50fb.h"
#include "reset_model_bounds50ba.h"
#include <bit>
#include <cmath>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace native_racer50HN {
// Explicit HOST starting-grid policy; not the unrecovered lateral-grid leaf.
// Ground height is always resolved by the actual native reset query against
// authenticated terrain. It never substitutes an interpolated route elevation.
// Lift belongs to the selected racer's PHY, not to the shared track owner.
inline bool groundGridPose(const authored_runtime_ground50cp_fixture::ColdScene&,
    const rr::game::BodyMatrix48G&,float,std::uint32_t*,std::size_t,
    rr::game::BodyMatrix48G&) noexcept;
// Only the retained RA bridge may supply a different authenticated owner span.
// Existing Ground clients keep their original public interface and loop order.
class ColdGroundProbe50HN final {
    friend class ra_source_camera50LA::OwnershipBridge;
    friend bool groundGridPose(const authored_runtime_ground50cp_fixture::ColdScene&,
        const rr::game::BodyMatrix48G&,float,std::uint32_t*,std::size_t,
        rr::game::BodyMatrix48G&) noexcept;
    template<class Owners> static bool run(const Owners& owners,
        const rr::game::BodyMatrix48G& requested,float lift,
        std::uint32_t* stack,std::size_t capacity,
        rr::game::BodyMatrix48G& result) noexcept {
    using namespace rr::game;
    std::uint32_t position[4]{};
    for(unsigned i=0;i<4;++i)position[i]=std::bit_cast<std::uint32_t>(requested.v[12+i]);
    ResetProbe50AG probe{};
    if(!beginResetProbe50AG(position,std::bit_cast<std::uint32_t>(lift),probe))return false;
    while(probe.phase==ResetProbePhase50AG::AwaitingQuery){
        SpringVector4_48N start{},end{};
        std::memcpy(start.v,probe.start,16);std::memcpy(end.v,probe.end,16);
        bool hit=false;std::uint32_t height=0;
        for(const auto& owner:owners){
            if(!(owner.instance.category&16u))continue;
            ResetCandidateResult50AM candidate{};
            if(queryResetCandidate50AM(start,end,owner.instance.world,
                {{owner.instance.sphere.center.v[0],owner.instance.sphere.center.v[1],
                  owner.instance.sphere.center.v[2]}},owner.instance.sphere.radius,
                1,owner.mesh,0,stack,capacity,candidate)!=ResetTreeStatus50AK::Complete)return false;
            if(candidate.hit){hit=true;end=candidate.point;height=std::bit_cast<std::uint32_t>(end.v[1]);}
        }
        if(!acceptResetProbe50AG(probe,hit,height))return false;
    }
    if(probe.phase!=ResetProbePhase50AG::Hit)return false;
    result=requested;
    for(unsigned i=0;i<4;++i)result.v[12+i]=std::bit_cast<float>(probe.staged[i]);
    return true;
}
};
inline bool groundGridPose(const authored_runtime_ground50cp_fixture::ColdScene& ground,
    const rr::game::BodyMatrix48G& requested,float lift,
    std::uint32_t* stack,std::size_t capacity,
    rr::game::BodyMatrix48G& result) noexcept {
    return ColdGroundProbe50HN::run(ground.owners,requested,lift,stack,capacity,result);
}
// Legacy callers retain their explicit Pod01 development-ground context.
// Heterogeneous roster callers must use the overload with their own lift.
inline bool groundGridPose(const authored_runtime_ground50cp_fixture::ColdScene& ground,
    const rr::game::BodyMatrix48G& requested,std::uint32_t* stack,std::size_t capacity,
    rr::game::BodyMatrix48G& result) noexcept {
    return groundGridPose(ground,requested,ground.lift,stack,capacity,result);
}
// Place a FRESH, still-unadvanced native state at the grounded host grid pose.
// Basis is unchanged. Uses native scene setters and actual model-bounds refresh;
// no cache/phase/velocity admission is forged. Never use this for live recovery.
// Caller removes/re-attaches live query memberships around this cold placement.
inline bool placeColdGrid(rr::game::VehicleFrameState49W& state,
    const rr::game::VehicleFrameBindings49W& bindings,
    const rr::game::BodyMatrix48G& requested,
    const rr::game::PodSceneRebaseModel50FB* models,std::uint32_t modelCount,
    rr::game::BasePoseHistoryState50Y& history) noexcept {
    using namespace rr::game;
    if(!models||(modelCount!=3&&modelCount!=5)||state.phase!=VehicleFramePhase49W::Ready||state.bodies.main.frameFactor!=0.f)return false;
    const auto main=static_cast<unsigned>(bindings.lifecycle.main190);
    if(!validVehicleSceneCount49W(state,bindings)||main>=state.sceneNodeCount)return false;
    const auto& previous=state.nodes[main].world100;
    for(unsigned i=0;i<12;++i)if(std::bit_cast<std::uint32_t>(previous.v[i])!=
        std::bit_cast<std::uint32_t>(requested.v[i]))return false;
    SceneDisplacement49P displacement{};
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(requested.v[12+i]))return false;
        displacement.v[i]=requested.v[12+i]-previous.v[12+i];
        if(!std::isfinite(displacement.v[i]))return false;
        if(state.bodies.main.components.root.body.velocity.v[i]!=0.f||
           state.bodies.secondary.root.body.velocity.v[i]!=0.f)return false;
    }
    auto next=state;
    SceneGraph49P graph{next.nodes,next.sceneNodeCount};
    const SceneNodeIndex49P ordinary[]={bindings.lifecycle.main190,bindings.lifecycle.physical19C};
    for(auto node:ordinary)if(!addParentlessTranslation49P(graph,node,displacement)||
        !refreshSceneWorld49P(graph,node))return false;
    ResetBoundNode50BA boundNodes[maxModelBoundsNodes49O]{};ResetBoundsWorkspace50BA workspace{};
    ModelBoundsResult49O result{};
    for(unsigned modelIndex=0;modelIndex<modelCount;++modelIndex){
        const auto& model=models[modelIndex];
        if(model.sourceNodeCount>maxModelBoundsNodes49O||model.sourceNodeCount!=model.hierarchy.nodeCount)return false;
        if(!addParentlessTranslation49P(graph,model.root,displacement))return false;
        for(unsigned i=0;i<model.sourceNodeCount;++i){
            rr::assets::PsgNode49O authored{};
            if(!rr::assets::readPsgNode49O(model.hierarchy,i,authored))return false;
            boundNodes[i].node=model.sourceNodes[i];
            for(unsigned axis=0;axis<3;++axis){
                boundNodes[i].center140.v[axis]=authored.bounds[axis];
                boundNodes[i].halfExtent150.v[axis]=authored.bounds[3+axis];
            }
        }
        if(!refreshResetModelBounds50BA(graph,model.root,boundNodes,
            model.sourceNodeCount,workspace,result))return false;
    }
    runtime_startup50fl::project(next,bindings);
    for(auto* root:{&next.bodies.main.components.root,&next.bodies.secondary.root}){
        if(!matrixToQuaternion48G(root->scene,root->body.orientation))return false;
        std::memcpy(root->body.position.v,root->scene.v+12,12);
    }
    // A grid placement is not a traversed movement ray. The first real frame
    // generates its own displacement and navigation preserves the source CA0=0.
    for(unsigned i=0;i<3;++i){
        next.bodies.main.priorPoint7B0[i]=requested.v[12+i];
        next.bodies.main.currentPoint7C0[i]=requested.v[12+i];
    }
    for(auto& slot:history.historyD50){std::memcpy(slot,requested.v+12,12);slot[3]=0x3f800000u;}
    state=next;return true;
}
inline bool placeColdGrid(rr::game::VehicleFrameState49W& state,
    const rr::game::VehicleFrameBindings49W& bindings,
    const rr::game::BodyMatrix48G& requested,
    const rr::game::PodSceneRebaseModel50FB (&models)[3],
    rr::game::BasePoseHistoryState50Y& history) noexcept {
    return placeColdGrid(state,bindings,requested,models,3,history);
}
}
