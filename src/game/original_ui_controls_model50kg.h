#pragma once
#include "original_ui_controls_controller50kg.h"
#include "original_ui_main_scene50kg.h"
#include "original_ui_options_camera50kg.h"
#include "original_ui_lit_geometry50km.h"
#include "model_playback50kg.h"
#include "../assets/original_ui_controls_model_assets50kg.h"
#include <memory>
namespace rr::game {
class OriginalUiLightManagerLifetime50KG;
class OriginalUiControlsModel50KG;
struct OriginalUiControlsLighting50KG {
 const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
 const assets::OriginalUiMemberIdentity50KG* sceneIdentity=nullptr;
 std::array<float,4> ambient50{},ambientSlot60{},directional3C{};
 std::array<float,3> directionalLook78{};
};
struct OriginalUiControlsModelSelection50KG {
 const OriginalUiControlsModel50KG* owner=nullptr;
 const OriginalUiMainSceneOwner50KG* sceneOwner=nullptr;
 std::uint64_t generation=0,serial=0;
 std::uint32_t sourceZoneIndex48=0,gridKey14=0;
 std::uint8_t sourceModelFlags191=0,sourceRenderFlags1B2=0;
 bool disabled=true;
 bool selected()const noexcept{return !disabled&&(sourceModelFlags191&1u);}
};
enum class OriginalUiControlsModelError50KG:std::uint8_t {
 none,alreadyPrepared,asset,authoring,graph,binding,playback,membership,camera,lighting,palette,arithmetic,allocation,operation
};
struct OriginalUiControlsModelDiagnostic50KG {
 OriginalUiControlsModelError50KG error=OriginalUiControlsModelError50KG::none;
 std::uint32_t sourcePC=0;
 assets::OriginalUiControlsModelAssetDiagnostic50KG asset{};
 assets::OriginalUiPageDiagnostic50KG text{};
 ModelPlaybackDiagnostic50KG playback{};ModelPaletteDiagnostic49S palette{};
};
struct OriginalUiControlsModelPacket50KG {
 const assets::Scene48D* scene=nullptr;
 ModelPalette49S palette{};
 OriginalUiLitContext50KM context{};
 OriginalUiControlsModelSelection50KG selection{};
 BodyMatrix48G sourceModelWorld{};
 std::uint32_t sourceLod=0,packedFogColorGs=0;
 bool computeRadialFog=false,emitsGeometry=false;
};
// One genuine eager Widget36 Soultree_Character. The background zone projection
// is tied to the supplied authenticated opt.scn owner. Its source method is the
// default shared 2D grid, not SearchList. No broad scene collector is substituted:
// this widget is directly traversed, and240C00 consumes its real observer zone.
// Serial use; scene and application owners outlive this object and all packets.
class OriginalUiControlsModel50KG final {
public:
 OriginalUiControlsModel50KG()noexcept;~OriginalUiControlsModel50KG();
 OriginalUiControlsModel50KG(const OriginalUiControlsModel50KG&)=delete;
 OriginalUiControlsModel50KG& operator=(const OriginalUiControlsModel50KG&)=delete;
 bool prepare(std::shared_ptr<const assets::OriginalUiArchive50KG>,
  const OriginalUiMainSceneOwner50KG&,OriginalUiControlsModelDiagnostic50KG&)noexcept;
 // Actual manager entry only, after beginUiManager; cold prepare never publishes.
 bool publishRegistry(OriginalUiLightManagerLifetime50KG&,OriginalUiControlsModelDiagnostic50KG&)noexcept;
 bool apply(const OriginalUiControlsEffect50KG&,ModelMotionSharedState49T&,
  ModelMode0IkShared50KG&,OriginalUiControlsModelDiagnostic50KG&)noexcept;
 bool advance(float,ModelMotionSharedState49T&,ModelMode0IkShared50KG&,
  OriginalUiControlsModelDiagnostic50KG&)noexcept;
 bool observation(bool& stopped10)const noexcept;
 bool preparePacket(const OriginalUiOptionsCamera50KG&,OriginalUiLightManagerLifetime50KG&,
  OriginalUiControlsModelPacket50KG&,OriginalUiControlsModelDiagnostic50KG&)noexcept;
 bool prepared()const noexcept;
 const assets::Scene48D* scene()const noexcept;
 const assets::OriginalUiMemberIdentity50KG* widgetIdentity()const noexcept;
 bool isCurrentSelection(const OriginalUiControlsModelSelection50KG&)const noexcept;
private:
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
