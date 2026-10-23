#include "original_ui_garage_model_construction50kg.h"
#include <cstring>
#include <utility>

namespace rr::game {
struct OriginalUiGarageModelConstruction50KG::Storage {
    // Retains the exact immutable PSG/Scene48D and both authenticated archives
    // for every borrowed source pointer. No independent model ref is claimed.
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets;
    SceneNode49P root{};
};
namespace {
using D=OriginalUiGarageModelConstructionDiagnostic50KG;
using E=OriginalUiGarageModelConstructionError50KG;
bool fail(D& d,E e,std::uint32_t pc)noexcept{d.error=e;d.sourcePC=pc;return false;}
}
OriginalUiGarageModelConstruction50KG::OriginalUiGarageModelConstruction50KG() noexcept=default;
OriginalUiGarageModelConstruction50KG::~OriginalUiGarageModelConstruction50KG()=default;
bool OriginalUiGarageModelConstruction50KG::prepareSelected19E(
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyPrepared,0x1D1CD0);
    if(!assets||!assets->loaded()||!assets->selectedPod()||!assets->selectedPod()->model)
        return fail(d,E::asset,0x1D1CD0);
    const auto* selected=assets->selectedPod();const auto& m=*selected->model;
    if(!m.payload||!m.payload->prepared()||m.payload->nodeCount()!=1||!m.payload->sourceNodes()||
       m.payload->sourceNodes()[0].parent!=-1||m.hierarchy.nodeCount!=1)
        return fail(d,E::hierarchy,0x28CB0C);
    // This leaf is only the exact source-immediate normal19E placement. It is
    // not a generic axis normalizer or a substitute for the289EC0 owner.
    if(selected->look!=std::array<float,3>{{0,0,1}}||selected->up!=std::array<float,3>{{0,1,0}})
        return fail(d,E::asset,0x1D1E80);
    try {
        auto n=std::make_unique<Storage>();n->assets=std::move(assets);
        //2893A0 gives zero cached world;28CAC0 copies source local and ORs
        // flags20|04 into dirty80. Row0's original parent=-1 is skipped.
        std::memcpy(n->root.localC0.v,m.payload->sourceNodes()[0].local,sizeof n->root.localC0.v);
        n->root.sceneFlags190=0xa4;
        //28AC10 first replaces source root XYZ;289EC0 then installs the
        // manager's exact +Z look/+Y up basis with homogeneous0/1 lanes.
        // No original PSG translation survives as a second child transform.
        constexpr float basis[12]{1,0,0,0,0,1,0,0,0,0,1,0};
        std::memcpy(n->root.localC0.v,basis,sizeof basis);
        for(std::size_t i=0;i<3;++i)n->root.localC0.v[12+i]=selected->position[i];
        n->root.localC0.v[15]=1;
        if(!validateSceneGraph49P({&n->root,1}))return fail(d,E::graph,0x28CB0C);
        storage_=std::move(n);return true;
    }catch(...){return fail(d,E::allocation,0x1D1CD0);}
}
bool OriginalUiGarageModelConstruction50KG::prepared()const noexcept{return bool(storage_);}
const SceneNode49P* OriginalUiGarageModelConstruction50KG::root()const noexcept{return storage_?&storage_->root:nullptr;}
const assets::PsgNode49O* OriginalUiGarageModelConstruction50KG::originalRoot()const noexcept{return storage_?source()->model->payload->sourceNodes():nullptr;}
const assets::OriginalUiGarageSelectedPodAsset50KG* OriginalUiGarageModelConstruction50KG::source()const noexcept{return storage_?storage_->assets->selectedPod():nullptr;}
std::uint32_t OriginalUiGarageModelConstruction50KG::graphNodeCount()const noexcept{return storage_?1u:0u;}
SceneNodeIndex49P OriginalUiGarageModelConstruction50KG::sourceRowIndex(std::uint32_t row)const noexcept{return storage_&&row==0?static_cast<SceneNodeIndex49P>(0):SceneNodeIndex49P::none;}
}
