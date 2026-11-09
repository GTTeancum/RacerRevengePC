#pragma once
// UIUnlock dynamic 3D widgets: User_Rotate pods 9..23 and Soultree_Character
// pilots 24..38 (PodUIPodControl, control factory 20A850 = copy of Rider's
// 1E9AC0). Same native owners as Rider Select: OriginalUiRiderPodScene50KG,
// OriginalUiRiderCharacterScene50KG, two binder helpers per pod, the flat
// visibility owner and the material-domain ledger. Callback semantics follow
// OriginalUiRiderDynamicOwner50KG::apply (select loads, deselect unloads when
// CleanFromMemory, character clearSelection loads 1D2650).
#include "original_ui_unlock_camera50pd.h"
#include "original_ui_unlock_page50pd.h"
#include "original_ui_rider_model_scene50kg.h"
#include "original_ui_rider_binder_animation50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_rider_model_packet50kg.h"
#include "model_playback50kg.h"
#include "coupled_body_advancement49m.h"
#include "../assets/original_ui_rider_model_materials50kg.h"
#include "../assets/original_ui_rider_binder_assets50kg.h"
#include "../assets/original_ui_ipum_texture_cache50kg.h"
#include <array>
#include <memory>
#include <vector>

namespace rr::game {
enum class OriginalUiUnlockModelError50PD : std::uint8_t {
    none,input,alreadyBound,notBound,pod,character,material,binder,footprint,lifetime,visibility,model,packet,phase,allocation
};
struct OriginalUiUnlockModelDiagnostic50PD {
    OriginalUiUnlockModelError50PD error=OriginalUiUnlockModelError50PD::none;
    std::uint16_t widget=0;std::uint32_t sourcePC=0;
    assets::OriginalUiRiderPodDiagnostic50KG pod{};assets::OriginalUiRiderCharacterAssetDiagnostic50KG character{};
    assets::OriginalUiRiderModelMaterialDiagnostic50KG material{};assets::OriginalUiRiderBinderAssetDiagnostic50KG binder{};
    OriginalUiRiderMaterialLifetimeDiagnostic50KG lifetime{};OriginalUiRiderVisibilityDiagnostic50KG visibility{};
    OriginalUiRiderModelDiagnostic50KG model{};
};
// Process-shared playback/IK/libm/RNG owners, borrowed from the Application.
struct OriginalUiUnlockModelShared50PD {
    ModelMotionSharedState49T* motion=nullptr;ModelMode0IkShared50KG* ik=nullptr;
    SourceLibmState49T* libm=nullptr;CoupledNoiseContext49M* rng=nullptr;
};
struct OriginalUiUnlockModelInput50PD {
    std::shared_ptr<const assets::OriginalUiArchive50KG> unlock,appStart,podTextures;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    const assets::OriginalUiIpumTextureCache50KG* ipumCache=nullptr;
    const OriginalUiUnlockPage50PD* page=nullptr; // authenticated unlock.dat controls
    std::uint32_t podMask=0;                      // bit p: prepare pod p's pod+pilot
};
// Cold, immutable per-pod population (pod widget, its pilot widget, materials).
struct OriginalUiUnlockColdModel50PD {
    std::int32_t pod=-1;std::uint16_t podWidget=0,characterWidget=0;
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets;
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets;
    std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> podMaterials,characterMaterials;
    OriginalUiRiderMaterialFootprint50KG podFootprint,characterFootprint;
};
struct OriginalUiUnlockModelPacket50PD {
    OriginalUiRiderModelSelection50KG selection{};
    OriginalUiRiderModelPacket50KG packet{};
    const assets::OriginalUiRiderModelMaterials50KG* materials=nullptr;
    const assets::OriginalUiRiderCharacterAssets50KG* characterAssets=nullptr; // pilots only
    bool emitsGeometry=false;
};
struct OriginalUiUnlockBinderPacket50PD {OriginalUiRiderBinderFrame50KG image{};OriginalUiLitBinderPacket50KM geometry{};};
// Candidate pods a showing can display: every pod of a pending check now, plus
// Vader (26) while locked, since granting 1/25/21 during this screen can make
// check 7 true at a later 20AE40 re-activation.
std::uint32_t originalUiUnlockCandidatePods50PD(const OriginalUnlockFacts50PD&) noexcept;
class OriginalUiUnlockModels50PD final {
public:
    OriginalUiUnlockModels50PD() noexcept;~OriginalUiUnlockModels50PD();
    OriginalUiUnlockModels50PD(const OriginalUiUnlockModels50PD&)=delete;
    OriginalUiUnlockModels50PD& operator=(const OriginalUiUnlockModels50PD&)=delete;
    bool prepareCold(const OriginalUiUnlockModelInput50PD&,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    const std::vector<OriginalUiUnlockColdModel50PD>* cold() const noexcept;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binderAssets() const noexcept;
    bool dynamicWidget(std::uint16_t) const noexcept;
    // Live binding (session bind): shared Application owners.
    bool bindLive(const OriginalUiUnlockModelShared50PD&,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    // One generic widget callback on a dynamic widget, in authored order,
    // after the widget bank applied the same callback. selected/shown are the
    // bank's resulting generic state for that widget.
    bool apply(const OriginalUiOptionsMenuCallback50KG&,bool selected,bool shown,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    // Child update 2A6BB0 -> 1D38B0 (pod) / 1D2730 (pilot), shown + loaded only.
    bool advance(float elapsedSeconds,bool ancestorsShown,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    // Render traversal: selection, palette and fog for one shown, loaded widget.
    bool beginFrame(const OriginalUiUnlockCameraFrame50PD&,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    bool loaded(std::uint16_t widget) const noexcept;
    bool prepare(std::uint16_t widget,const OriginalUiUnlockCameraFrame50PD&,OriginalUiUnlockModelPacket50PD&,
        OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    bool binderPresent(std::uint16_t podWidget,std::size_t pair) const noexcept;
    bool projectBinder(std::uint16_t podWidget,std::size_t pair,const OriginalUiUnlockCameraFrame50PD&,
        std::uint32_t milliseconds,bool animate,OriginalUiUnlockBinderPacket50PD&,OriginalUiUnlockModelDiagnostic50PD&) noexcept;
    // Explicit teardown of every live graph/helper/membership.
    bool drain(OriginalUiUnlockModelDiagnostic50PD&) noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
