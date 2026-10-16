#pragma once
#include "original_wreck_owner50la.h"
#include "vehicle_visual49r.h"

namespace rr::game {
// Borrow the actual body288 graph selected by the destruction/detachment
// handler. This is neither a destination pose nor a replacement physics body.
// The caller owns the original collection40 -> application4C0/4C4 zone mapping;
// both refs retain the same authored zone ordinal in their separate namespaces.
struct OriginalWreckActivationSource50LA {
    const SceneGraph49P* graph=nullptr;
    SceneNodeIndex49P body288=SceneNodeIndex49P::none;
    RootBodyState48H* body=nullptr;
    std::uint32_t* sceneFlags190=nullptr;
    const CollectionRef50BX* visibilityZone=nullptr;
    const CollectionRef50BX* collisionZone=nullptr;
    VisualLazyWord49R* sharedOrientationSentinel4433D8=nullptr;
    VisualLazyWord49R* sharedPositionSentinel4433E0=nullptr;
};
enum class OriginalWreckActivationStatus50LA : std::uint8_t {
    complete,owner,sourceBody,component,graph,arithmetic,query,visibility,group,body
};
struct OriginalWreckActivationResult50LA {
    OriginalWreckActivationStatus50LA status=OriginalWreckActivationStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool collectionActivated=false,pieceActivated=false,initialPoseCommitted=false;
    bool visibilityAttached=false,queriesAttached=false,authoredOffsetCommitted=false;
    bool groupActivated=false,bodyEnabled=false;
    OriginalWreckComponentResult50LA component{};
    OriginalWreckQueryResult50LA query{};
    OriginalWreckVisibilityResult50LA visibility{};
};
namespace original_wreck_activation_detail50LA {
using Math=original_wreck_force_detail50LA::Math;
inline bool node(const SceneGraph49P& graph,SceneNodeIndex49P id)noexcept {
    const auto i=static_cast<std::int32_t>(id);
    return i>=0&&static_cast<std::uint32_t>(i)<graph.count;
}
inline bool position(const SceneGraph49P& graph,SceneNodeIndex49P model,
    const SpringVector4_48N& value,VisualLazyWord49R& global)noexcept {
    //28AC90(null), parentless: only XYZ is copied. The original localFC
    //survives;49P performs the exact ancestor/ready descendant flag operations.
    if(!global.guard){global.word=0xffffffffu;global.guard=1;} //28ACB4..28ACC4
    auto matrix=graph.nodes[static_cast<std::int32_t>(model)].localC0;
    std::memcpy(matrix.v+12,value.v,12);return setLocalMatrix49P(graph,model,matrix);
}
inline bool axes(const SceneGraph49P& graph,SceneNodeIndex49P model,
    const Vector3_48C& look,const Vector3_48C& up,VisualLazyWord49R& global)noexcept {
    //28A090(null,look,up,0,1,0): t0!=1 skips orthogonalization.
    //Normalize up first, then look, then cross(up,look). No third normalize.
    if(!global.guard){global.word=0xffffffffu;global.guard=1;}
    Math m;const auto y=m.normalize(up),z=m.normalize(look);Vector3_48C x{};
    x.v[0]=m.sub(m.mul(y.v[1],z.v[2]),m.mul(y.v[2],z.v[1]));
    x.v[1]=m.sub(m.mul(y.v[2],z.v[0]),m.mul(y.v[0],z.v[2]));
    x.v[2]=m.sub(m.mul(y.v[0],z.v[1]),m.mul(y.v[1],z.v[0]));
    if(!m.valid)return false;
    auto matrix=graph.nodes[static_cast<std::int32_t>(model)].localC0;
    for(unsigned i=0;i<3;++i){matrix.v[i]=x.v[i];matrix.v[4+i]=y.v[i];matrix.v[8+i]=z.v[i];}
    matrix.v[3]=matrix.v[7]=matrix.v[11]=0.f;matrix.v[15]=1.f;
    return setLocalMatrix49P(graph,model,matrix);
}
inline void project(OriginalOnePieceWreckOwner50LA& owner,const SceneGraph49P& graph)noexcept {
    const auto& model=graph.nodes[static_cast<std::int32_t>(owner.assets.model14C())];
    owner.physical.body.root.scene=model.world100;
    owner.physical.body.sceneFlags190=(owner.physical.body.sceneFlags190&0xffffff00u)|model.sceneFlags190;
}
}
// Actual22C6A0, bounded to the authenticated one-piece collection. It performs
//the genuine activity, authored model placement, distinct visibility and three
//collision registrations, aggregate recomputation and body enable operations.
//It does not write racer40/CD4, stop the intact pod, release the group, reset
//physics, draw RNG, or certify any later component/particle frame.
//
//Fresh/detached membership route only. The complete cold owner (including real
//nonnull effect constructors if reached) is required; a storage-only prefix
//cannot activate. Each successful source prefix survives a later boundary.
//Do not replay from entry after a failure. Actual collection/component source
//order and the final original model offset are retained, not a teleported
//intact vehicle or invented wreck pose. No allocation or supplied callbacks.
inline OriginalWreckActivationResult50LA activateOriginalOnePieceWreck50LA(
    OriginalOnePieceWreckOwner50LA& owner,const OriginalWreckActivationSource50LA& source)noexcept {
    using namespace original_wreck_activation_detail50LA;
    using S=OriginalWreckActivationStatus50LA;OriginalWreckActivationResult50LA r;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
    if(!owner.prepared()||owner.assets.selected().pieces.size()!=1u||
       owner.group.capacity34||owner.group.attachedCount30||!owner.sceneObjectFlags03||
       owner.visibility.attached())return fail(S::owner,0x22c6a0u);
    for(unsigned i=0;i<3;++i)if(owner.queries.query(i).membership.attached)return fail(S::owner,0x120070u);
    if(!source.graph||!source.body||!source.sceneFlags190||!source.visibilityZone||!source.collisionZone||
       !*source.visibilityZone||*source.visibilityZone!=*source.collisionZone||
       !source.sharedOrientationSentinel4433D8||!source.sharedPositionSentinel4433E0||
       !validateSceneGraph49P(*source.graph)||
       !node(*source.graph,source.body288))return fail(S::sourceBody,0x22c6ecu);
    const auto graph=owner.assets.graph();const auto model=owner.assets.model14C();
    if(!validateSceneGraph49P(graph)||!node(graph,model)||
       graph.nodes[static_cast<std::int32_t>(model)].parent180!=SceneNodeIndex49P::none||
       graph.nodes==source.graph->nodes)return fail(S::graph,0x22c710u);
    //This canonical body.scene is a projection of its real body288 world,
    //including a legitimately dirty cache before the first getter refresh.
    if(std::memcmp(source.body->scene.v,source.graph->nodes[static_cast<std::int32_t>(source.body288)].world100.v,
        sizeof source.body->scene.v)||static_cast<std::uint8_t>(*source.sceneFlags190)!=
        source.graph->nodes[static_cast<std::int32_t>(source.body288)].sceneFlags190)
        return fail(S::sourceBody,0x22c710u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x22c710u);
    r.component=setOriginalWreckBaseActivity50LA(owner.domain,owner.collectionComponent(),true);
    if(r.component.status!=OriginalWreckComponentStatus50LA::complete)return fail(S::component,0x22c708u);
    r.collectionActivated=true;
    if(!refreshSceneWorld49P(*source.graph,source.body288))return fail(S::graph,0x28b1a0u);
    const auto& world=source.graph->nodes[static_cast<std::int32_t>(source.body288)].world100;
    source.body->scene=world;
    *source.sceneFlags190=(*source.sceneFlags190&0xffffff00u)|
        source.graph->nodes[static_cast<std::int32_t>(source.body288)].sceneFlags190;
    SpringVector4_48N origin{{0.f,0.f,0.f,1.f}};
    Vector3_48C look{},up{};
    std::memcpy(origin.v,world.v+12,12);std::memcpy(look.v,world.v+8,12);std::memcpy(up.v,world.v+4,12);
    r.component=activateOriginalWreckPiece50LA(owner.domain,owner.pieceComponent(),owner.piece.contactTime12C);
    if(r.component.status!=OriginalWreckComponentStatus50LA::complete)return fail(S::component,0x22c754u);
    r.pieceActivated=true;
    owner.sceneObjectFlags03[0]=static_cast<std::uint8_t>(owner.sceneObjectFlags03[0]&~4u); //28C700
    owner.piece.previous80=origin;owner.piece.current90=origin;
    if(!position(graph,model,origin,*source.sharedPositionSentinel4433E0)||
       !axes(graph,model,look,up,*source.sharedOrientationSentinel4433D8))
        return fail(S::graph,0x22c7e4u);
    r.initialPoseCommitted=true;project(owner,graph);
    for(unsigned i=0;i<2;++i){r.query=owner.queries.prepareModelQuery(i);
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);}
    project(owner,graph);
    r.query=owner.queries.stageMovement80_90(owner.piece.previous80,owner.piece.current90);
    if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
    r.visibility=owner.visibility.attach(*source.visibilityZone);
    if(r.visibility.status!=OriginalWreckVisibilityStatus50LA::complete)return fail(S::visibility,r.visibility.sourcePC);
    r.visibilityAttached=true;
    for(unsigned i=0;i<3;++i){r.query=owner.queries.attach(i,*source.collisionZone);
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);}
    r.queriesAttached=true;
    //28B9B0 transforms authored50 as a direction (no model translation),
    //then22C908/90C/914 add the saved source world position separately.
    if(!refreshSceneWorld49P(*source.graph,source.body288))return fail(S::graph,0x28b9b0u);
    const auto& currentSource=source.graph->nodes[static_cast<std::int32_t>(source.body288)].world100;
    source.body->scene=currentSource;
    *source.sceneFlags190=(*source.sceneFlags190&0xffffff00u)|
        source.graph->nodes[static_cast<std::int32_t>(source.body288)].sceneFlags190;
    Math m;SpringVector4_48N offsetPosition{{0.f,0.f,0.f,1.f}};
    for(unsigned i=0;i<3;++i){
        float axis=m.mul(currentSource.v[i],owner.piece.authored50.v[0]);
        axis=m.add(axis,m.mul(currentSource.v[4+i],owner.piece.authored50.v[1]));
        axis=m.add(axis,m.mul(currentSource.v[8+i],owner.piece.authored50.v[2]));
        axis=m.add(axis,0.f); //28B9B0 final +vf0 in XYZ
        offsetPosition.v[i]=m.add(origin.v[i],axis);
    }
    if(!m.valid)return fail(S::arithmetic,0x22c908u);
    if(!position(graph,model,offsetPosition,*source.sharedPositionSentinel4433E0))return fail(S::graph,0x22c93cu);
    r.authoredOffsetCommitted=true;project(owner,graph);
    r.visibility=owner.visibility.update();
    if(r.visibility.status!=OriginalWreckVisibilityStatus50LA::complete)return fail(S::visibility,r.visibility.sourcePC);
    for(unsigned i=0;i<3;++i){
        if(i<2){r.query=owner.queries.prepareModelQuery(i);
            if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);}
        r.query=owner.queries.update(i);
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
    }
    project(owner,graph);
    owner.piece.flight34=1;owner.piece.flight38=1; //one-piece route skips count>=2 reset
    r.component=setOriginalWreckBaseActivity50LA(owner.domain,owner.groupComponent(),true);
    if(r.component.status!=OriginalWreckComponentStatus50LA::complete)return fail(S::component,0x22ca10u);
    r.groupActivated=true;
    owner.pieceComponent().mask20=0; //2CA4B0 occurs even with zero nonroot capacity
    if(!recomputeOriginalOnePieceGroup50LA(owner.group))return fail(S::group,0x22ca20u);
    if(setOriginalWreckBodyEnabled50LA(owner.physical,true)!=OriginalWreckBodyControlStatus50LA::complete)
        return fail(S::body,0x22ca38u);
    r.bodyEnabled=true;r.status=S::complete;r.sourcePC=0x22ca58u;return r;
}
} // namespace rr::game
