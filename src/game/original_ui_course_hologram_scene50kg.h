#pragma once
#include "../assets/original_ui_course_hologram_assets50kg.h"
#include "model_playback50kg.h"
#include "model_palette49s.h"
#include "original_ui_navigation50kg.h"
#include <memory>
namespace rr::game {
enum class OriginalUiCourseHologramSceneError50KG : std::uint8_t {none,alreadyLoaded,asset,graph,binding,playback,palette,allocation,operation};
struct OriginalUiCourseHologramSceneDiagnostic50KG {
 OriginalUiCourseHologramSceneError50KG error=OriginalUiCourseHologramSceneError50KG::none;
 std::uint32_t sourcePC=0;
 ModelPlaybackDiagnostic50KG playback{};ModelPaletteDiagnostic49S palette{};
};
// Eager common Soultree_Character2A85F0, not Rider1D1F90. Caller serializes
// operations, graph projection and all frame borrows. No renderer-ready flag.
class OriginalUiCourseHologramScene50KG final {
public:
 OriginalUiCourseHologramScene50KG() noexcept;~OriginalUiCourseHologramScene50KG();
 OriginalUiCourseHologramScene50KG(const OriginalUiCourseHologramScene50KG&)=delete;
 OriginalUiCourseHologramScene50KG& operator=(const OriginalUiCourseHologramScene50KG&)=delete;
 bool load(std::shared_ptr<const assets::OriginalUiCourseHologramAssets50KG>,OriginalUiCourseHologramSceneDiagnostic50KG&) noexcept;
 bool preflightApply(OriginalUiControlOperation50KG,bool,const ModelMotionSharedState49T&,const ModelMode0IkShared50KG&,OriginalUiCourseHologramSceneDiagnostic50KG&)const noexcept;
 bool apply(OriginalUiControlOperation50KG,bool,ModelMotionSharedState49T&,ModelMode0IkShared50KG&,OriginalUiCourseHologramSceneDiagnostic50KG&)noexcept;
 bool advance(float,ModelMotionSharedState49T&,ModelMode0IkShared50KG&,OriginalUiCourseHologramSceneDiagnostic50KG&)noexcept;
 bool projectPalette(std::uint8_t,ModelPalette49S&,OriginalUiCourseHologramSceneDiagnostic50KG&)noexcept;
 bool loaded()const noexcept;bool shown()const noexcept;bool selected()const noexcept;
 const assets::Scene48D* scene()const noexcept;SceneGraph49P graph()noexcept;
private:
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
