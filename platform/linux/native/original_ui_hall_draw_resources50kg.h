#pragma once
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/game/original_ui_hall_widgets50kg.h"
#include "../../../src/game/original_ui_hall_records50kg.h"
#include "../../../src/game/original_ui_hall_camera50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <memory>
#include <vector>
namespace rr::platform {
enum class OriginalUiHallDrawError50KG:std::uint8_t {none,alreadyLoaded,notLoaded,archive,videoOwner,widgets,resource,material,animation,geometry,samplerConflict,registry,allocation,input};
struct OriginalUiHallDrawDiagnostic50KG {
 OriginalUiHallDrawError50KG error=OriginalUiHallDrawError50KG::none;
 std::uint16_t widgetId=0;std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
 assets::OriginalUiScreenAssetDiagnostic50KG resource{};assets::OriginalUiMaterialDiagnostic50KG material{};assets::DecodeError48C atm{};
 game::OriginalUiHallWidgetDiagnostic50KG widgets{};game::OriginalUiTextGeometryDiagnostic50KG text{};
 game::OriginalUiHallCameraDiagnostic50KG camera{};game::OriginalUiTextDiagnostic50KG glyph{};game::OriginalUiExitTextDiagnostic50KG exitText{};
};
struct OriginalUiHallVideoOwner50KG {std::shared_ptr<const void> lifetime;std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;std::uint32_t zBufferFormat=0;};
struct OriginalUiHallColdInput50KG {
 std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,hall,fonts,controllerPopup,overlays;
 const assets::OriginalUiEnglish50KG* english=nullptr;assets::OriginalUiMaterialRuntime50KG runtime{};OriginalUiHallVideoOwner50KG video{};
};
struct OriginalUiHallMaterialBinding50KG {
 std::string registeredName;assets::OriginalUiMaterial50KG material;std::vector<std::uint32_t> images;
 std::vector<assets::OriginalUiNativeMaterial50KG> policies;assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
 game::OriginalUiTextureAnimationConfig50KG animation{};bool animated=false;
 std::uint32_t registryToken=0;const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
// Source1E3EF0/1E17D0 input. Session owns navigation/profile mutations. Records
// are the selected authentic native profile row, never authored widget text.
struct OriginalUiHallPageInput50KG {
 std::uint32_t courseIndex=0,planetIndex=0;
 bool showPlanetHeader=true;
 const game::OriginalUiHallCourseRecords50KG* records=nullptr;
 bool colorsKnown=false;std::uint32_t primaryColor79C=0,secondaryColor7A0=0;
};
struct OriginalUiHallPagePacket50KG {
 const game::OriginalUiOverlayQuad50KG* quad=nullptr;
 const game::OriginalUiTextLayout50KG* text=nullptr;
 std::array<std::uint8_t,4> packetColor{{127,127,127,63}};
 std::uint32_t materialIndex=0,sourcePC=0;std::uint16_t objectOffset=0;
};
// Exact retained UIHOF resources, without GPU publication or route admission.
// Shared archive/image/video lifetimes must cover every borrowed projection.
class OriginalUiHallDrawResources50KG final {
public:
 OriginalUiHallDrawResources50KG()noexcept;~OriginalUiHallDrawResources50KG();
 OriginalUiHallDrawResources50KG(const OriginalUiHallDrawResources50KG&)=delete;OriginalUiHallDrawResources50KG& operator=(const OriginalUiHallDrawResources50KG&)=delete;
 bool load(const OriginalUiHallColdInput50KG&,OriginalUiHallDrawDiagnostic50KG&)noexcept;
 bool loaded()const noexcept;
 game::OriginalUiHallWidgets50KG* widgets()noexcept;const game::OriginalUiHallWidgets50KG* widgets()const noexcept;
 const game::OriginalUiHallCamera50KG* camera()const noexcept;
 game::OriginalUiExitText50KG* exitText()noexcept;
 const assets::OriginalUiScreenAssets50KG* images()const noexcept;const assets::OriginalUiImageBank50KG* sourceImageBank()const noexcept;
 const assets::MaterialTextureAssets50EA* textures()const noexcept;const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
 const std::vector<OriginalUiHallMaterialBinding50KG>* materials()const noexcept;const OriginalUiHallVideoOwner50KG* video()const noexcept;
 const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t)const noexcept;
 const game::OriginalUiPositionedLine50KG* textGeometryLine(std::uint16_t,std::size_t)const noexcept;
 std::uint32_t widgetMaterial(std::uint16_t)const noexcept;std::uint32_t materialIndex(const char*)const noexcept;
 // Original raw Texture route, not a material or a PSM fallback. Coruscant3
 // has no Hall course/background and is rejected instead of aliased.
 std::uint32_t backgroundImage(std::uint32_t planet)const noexcept;
 // Retained page+154, initially null (unresolved). Application +44 dispatch
 // reads this before +48 invokes projectPage; this is not current navigation.
 std::uint32_t currentBackgroundImage()const noexcept;
 // Invoke after ordinary widgets at1E3EF0. Applies original theme setters and
 // projects embedded ScreenTexts in original category/rank/name/value order.
 bool projectPage(const OriginalUiHallPageInput50KG&,OriginalUiHallDrawDiagnostic50KG&)noexcept;
 const OriginalUiHallPagePacket50KG* pagePackets()const noexcept;
 std::size_t pagePacketCount()const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
