#include "vehicle_secondary_contact50j.h"
#include <cstring>
namespace rr::game {
namespace {
bool overlap(const void*a,std::size_t an,const void*b,std::size_t bn) noexcept {
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return x<y?y-x<an:x-y<bn;
}
}
SecondaryQuerySceneStatus50J prepareSecondaryQueryScene50J(VehicleFrameState49W&s,
    const VehicleFrameBindings49W&b,const MainStaticDetectorAdmission50H&a,VehicleFrameWorkspace49W&w) noexcept {
    using S=SecondaryQuerySceneStatus50J;
    if(overlap(&s,sizeof s,&w,sizeof w)||overlap(&s,sizeof s,&b,sizeof b)||overlap(&s,sizeof s,&a,sizeof a)||
       overlap(&w,sizeof w,&b,sizeof b)||overlap(&w,sizeof w,&a,sizeof a))return S::InvalidInput;
    if(s.phase!=VehicleFramePhase49W::PostWorld||s.bodies.main.substepIndex!=0||!s.bodies.vehicle838)return S::UnsupportedScope;
    if(!a.springPresent||!a.queryPresent||!a.queryEnabled||a.pairsPresent||(a.providerPresent&&a.pointContactsEnabled)||
       !a.explicitCandidates||a.queryType!=2||a.registrationE0)return S::UnsupportedScope;
    const auto index=static_cast<int>(b.lifecycle.physical19C);
    if(index<0||index>=static_cast<int>(vehicleFrameNodeCount49W)||a.queryScene!=b.lifecycle.physical19C)return S::InvalidInput;
    const auto&node=s.nodes[index];const auto&body=s.bodies.secondary;
    if(node.parent180!=SceneNodeIndex49P::none||s.bodies.secondaryDomain.sceneHasParent||
       s.bodies.secondaryDomain.sceneObserverCount||b.secondaryDomain.rootObserverPresent19C)return S::UnsupportedScope;
    if(std::memcmp(&node.world100,&body.root.scene,sizeof node.world100)||
       (body.sceneFlags190&255u)!=node.sceneFlags190)return S::InvalidInput;
    std::memcpy(&w.next,&s,sizeof s);
    if(!refreshSceneWorld49P({w.next.nodes,vehicleFrameNodeCount49W},b.lifecycle.physical19C))return S::GraphFailure;
    const auto&ready=w.next.nodes[index];auto&next=w.next.bodies.secondary;
    std::memcpy(&next.root.scene,&ready.world100,sizeof next.root.scene);
    next.sceneFlags190=(next.sceneFlags190&0xffffff00u)|ready.sceneFlags190;
    std::memcpy(&s,&w.next,sizeof s);return S::Ready;
}
}
