#pragma once
#include "original_ui_controller_popup50kg.h"
#include "original_ui_main_layout50kg.h"
#include "original_ui_text50kg.h"
#include "../assets/original_ui_localization50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rr::platform {class OriginalUiControllerPopupResources50KG;}
namespace rr::game {
enum class OriginalUiControllerPopupMaterial50KG : std::uint8_t {
    dim=0,overlay=1,font=2,cross=3,triangle=4
};
enum class OriginalUiControllerPopupGeometryError50KG : std::uint8_t {
    none,alreadyPrepared,notPrepared,localization,resource,textCapacity,geometry,unsupportedRoute,invalidRender,allocation
};
struct OriginalUiControllerPopupGeometryDiagnostic50KG {
    OriginalUiControllerPopupGeometryError50KG error=OriginalUiControllerPopupGeometryError50KG::none;
    OriginalUiTextDiagnostic50KG text{};
    std::uint32_t sourcePC=0;
    std::uint16_t objectOffset=0;
};
struct OriginalUiControllerPopupText50KG {
    std::string text;
    OriginalUiTextLayout50KG glyphs{};
    float authoredX=0,authoredY=0,positionedX=0,positionedY=0;
    std::int32_t measuredWidth=0,measuredHeight=0;
    std::uint16_t objectOffset=0;
    std::uint8_t capacity5B=0,characterCount5C=0,allocatedPackets5D=0,activePackets5E=0;
    std::uint32_t justification50=0,color54=0x00cccccc;
    std::int16_t alpha58=255;
};
struct OriginalUiControllerPopupPacket50KG {
    const OriginalUiOverlayQuad50KG* quad=nullptr;
    const OriginalUiTextLayout50KG* text=nullptr;
    std::array<std::uint8_t,4> packetColor{};
    OriginalUiControllerPopupMaterial50KG material=OriginalUiControllerPopupMaterial50KG::dim;
    std::uint32_t sourcePC=0;
    std::uint16_t objectOffset=0;
    float packetDepth=0;
};
// Pure bounded projection of2055B0/2064C0. The retained platform owner must pin
// the authentic image bank and enforce actual640x448/Video maximum0xffffff.
// No DAT descriptor, material policy, route, clock, GPU work or readiness claim
// is synthesized. Borrowed packet pointers survive until the next successful
// project, and borrowed glyph atlases require the prepared image bank to live.
class OriginalUiControllerPopupGeometry50KG final {
    friend class rr::platform::OriginalUiControllerPopupResources50KG;
public:
    using Diagnostic=OriginalUiControllerPopupGeometryDiagnostic50KG;
    static constexpr std::size_t maximumPackets=9;
    OriginalUiControllerPopupGeometry50KG()=default;
    OriginalUiControllerPopupGeometry50KG(const OriginalUiControllerPopupGeometry50KG&)=delete;
    OriginalUiControllerPopupGeometry50KG& operator=(const OriginalUiControllerPopupGeometry50KG&)=delete;
    bool prepare(const assets::OriginalUiEnglish50KG&,const assets::OriginalUiScreenAssets50KG&,
        std::uint32_t fontImage,std::uint32_t iconXImage,std::uint32_t iconTriImage,Diagnostic&) noexcept;
    bool project(const OriginalUiControllerPopupRender50KG&,Diagnostic&) noexcept;
    const std::array<OriginalUiControllerPopupText50KG,3>& garagePurchaseTexts()const noexcept{return textVariants_[1];}
    const std::array<OriginalUiControllerPopupText50KG,3>& startTexts(const OriginalUiControllerPopupState50KG& state)const noexcept {
        return textVariants_[state.garagePurchaseText?1:state.hasConfirm3D0&&state.hasCancel3D4?2:0];
    }
    bool prepared()const noexcept{return prepared_;}
    std::size_t packetCount()const noexcept{return packetCount_;}
    const std::array<OriginalUiControllerPopupPacket50KG,maximumPackets>& packets()const noexcept{return packets_;}
    const std::array<OriginalUiControllerPopupText50KG,3>& screenTexts()const noexcept{return textVariants_[selectedTexts_];}
private:
    bool prepared_=false;
    // Cold-prepared original strings: Controller2, Garage, Controller2 after
    // Garage (null a3 retains the original No label and hasCancel flag).
    std::array<std::array<OriginalUiControllerPopupText50KG,3>,3> textVariants_{};
    std::size_t selectedTexts_=0;
    std::array<OriginalUiOverlayQuad50KG,6> quads_{}; // dim, cross, triangle, three strips.
    std::array<OriginalUiControllerPopupPacket50KG,maximumPackets> packets_{};
    std::size_t packetCount_=0;
    float crossWidth_=0,crossHeight_=0,triangleWidth_=0,triangleHeight_=0;
    float overlayReciprocalWidth_=0,overlayReciprocalHeight_=0;
};
}
