#pragma once
#include "original_ui_garage_screen_text50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include "../../../src/game/original_ui_text_widget50kg.h"
#include <string_view>
#include <span>

namespace rr::platform {
class OriginalUiGarageWidget1Owner50KG;
class OriginalUiGarageRendererLease50KG;
class OriginalUiGarageDrawContribution50KG;
enum class OriginalUiGarageTextOverlayStatus50KG : std::uint8_t {
 complete,authority,phase,source,allocation,scope,font,payload,unsupported
};
struct OriginalUiGarageTextOverlayDiagnostic50KG {
 OriginalUiGarageTextOverlayStatus50KG status=OriginalUiGarageTextOverlayStatus50KG::complete;
 std::uint32_t sourcePC=0;
 OriginalUiGarageScreenTextDiagnostic50KG payload{};
 assets::OriginalApplicationFontDiagnostic50LA font{};
};
// Actual counted29C850/29CA40 owner, not a descriptor/geometry replacement.
// The original Entity is at guest offset0 in the0x190 allocation. Native
// backing retains typed fields rather than claiming the guest pointer ABI.
// Concrete Garage manager construction and its typed Page-entry color call
// are admitted; update/draw and packet color flushing remain separate.
class OriginalUiGarageTextOverlay50KG final {
 friend class OriginalUiGarageWidget1Owner50KG;
 friend class OriginalUiGarageDrawContribution50KG;
 friend class game::OriginalUiEntityScope50KG;
 using Application=::original_main_entry50KG::Application;
 using C=game::OriginalWreckComponent50LA;
 using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
 using Authority=game::OriginalApplicationEntities50LA;
 using Widget=OriginalUiGarageWidget1Owner50KG;
 using Archive=assets::OriginalUiArchive50KG;
 using Fonts=assets::OriginalApplicationFonts50LA;
 using Text=OriginalUiGarageScreenText50KG;
 using D=OriginalUiGarageTextOverlayDiagnostic50KG;
 using S=OriginalUiGarageTextOverlayStatus50KG;
 template<class T>struct Known {
  T value{};bool produced=false;
  void store(T v)noexcept{value=v;produced=true;}
 };
 enum class Phase:std::uint8_t {empty,constructed,loading,loaded,failed,retiring,retired};
 OriginalUiGarageTextOverlay50KG()noexcept=default;
 bool construct29C850(const Application&,std::shared_ptr<Grant>,const Widget*,C& outer,D&)noexcept;
 bool load29CA40(std::shared_ptr<const Archive> fonts,std::shared_ptr<const Archive> garage,
     std::string_view fontName,std::string_view materialName,std::int32_t capacity,D&)noexcept;
 bool attach2A9C64(D&)noexcept;
 bool attach2A9710(D&)noexcept;
 bool setText29D940(std::string_view,D&)noexcept;
 bool setAlpha29DBC0(std::int32_t,float duration,D&)noexcept;
 bool setDepth29DC20(float,D&)noexcept;
 bool setJustification29DB30(std::int32_t,D&)noexcept;
 bool setColor29DA10(std::uint32_t,D&)noexcept;
 // Source target/dirty words, not ScreenText54 or rendered packet equality.
 bool colorTargetCurrent(std::uint32_t)const noexcept;
 bool setupStateCurrent(bool enabled)const noexcept;
 // Constructor/entry witnesses remain strict. Actual post-update custody is
 // admitted only after this leaf has executed its own29CDA0 prefix.
 bool retainedColorTargetCurrent(std::uint32_t)const noexcept;
 bool retainedSetupStateCurrent(bool)const noexcept;
 bool draw29D3F0(OriginalUiGarageDrawContribution50KG&,D&)noexcept;
 std::uint64_t drawPublication_=0;bool drawPending_=false;
 bool update29CDA0(OriginalUiGarageRendererLease50KG&,float,std::uint64_t,D&)noexcept;
 enum class UpdateStep:std::uint8_t {never,dirtyCleared,viewportRead,returned};
 UpdateStep updateStep_=UpdateStep::never;
 std::uint64_t updatePublication_=0;
 bool updateFailed_=false;
 std::uint32_t updateFailurePC_=0;
 bool stop29DB10(D&)noexcept;
 bool fontLineHeight281730(float&,D&)const noexcept;
 bool setPosition29DB50(std::int32_t x,std::int32_t y,float duration,D&)noexcept;
 bool setup29DA40(const game::OriginalUiTextLineSetup50KG&,D&)noexcept;
 bool preflightRetirement(D&)const noexcept;
 static bool preflightSharedRetirement(std::span<const OriginalUiGarageTextOverlay50KG* const>,D&)noexcept;
 bool retire29C9C0(D&)noexcept;
 bool identityCurrent()const noexcept;
 bool payloadCurrent()const noexcept;
 bool payloadOwnerAndResourcesCurrent()const noexcept;
 // Named Widget friend only: exact immediate owner identities, never a ready
 // flag. The caller must have just validated that same Widget's full fields.
 bool payloadCurrentAfterWidgetFields(const Widget&,const C& actualOuter)const noexcept;
 bool lineInputsCurrent()const noexcept;
 bool attachLine(std::uint32_t,D&)noexcept;
 bool fail(D&,S,std::uint32_t)noexcept;
 static bool authorityThread(const std::shared_ptr<Authority>&)noexcept;
 C entity_;
 const std::thread::id ownerThread_=std::this_thread::get_id();
 const Application* issuer_=nullptr;
 std::shared_ptr<Authority> authority_;
 game::OriginalApplicationAnimationClockView50KG lifetime_;
 std::shared_ptr<Grant> grant_;
 const Widget* widget_=nullptr;C* outer_=nullptr;
 std::string_view fontInput_,materialInput_;
 std::int32_t capacityInput_=0;bool dynamicCaller_=false;
 std::unique_ptr<Text> screenText30_;
 std::shared_ptr<const Fonts::Entry> font34_;
 //38..64, including source-unknown60/64.29DB50's16-byte copy propagates
 //knownness into40/44 without reading indeterminate native integers.
 std::array<Known<std::int32_t>,12> rectangle38_{};
 float elapsed68_=0,duration6C_=0;
 std::uint32_t enabled70_=0;std::int32_t justification74_=0;
 float timer78_=0,blink7C_=0;std::uint32_t blinkOn80_=0;
 float hold84_=0;std::uint32_t indefinite88_=0;const void* scene8C_=nullptr;
 Known<std::int32_t> alphaFrom90_,alphaTarget94_;
 float field98_=0,alphaElapsed9C_=0,alphaDurationA0_=0;
 Known<float> scaleFromA4_,scaleTargetA8_;float scaleElapsedAC_=0,scaleDurationB0_=0;
 Known<float> rotationTargetB4_,rotationFromB8_;float rotationElapsedBC_=0,rotationDurationC0_=0;
 float scaleC4_=0,scaleC8_=0;std::uint32_t lockCC_=0;float secondDelayD0_=0;
 Known<std::int32_t> secondAlphaD4_;Known<float> secondScaleD8_,secondRotationDC_;
 std::array<std::uint32_t,4> fieldsE0_{};
 std::array<char,64> nameF0_{},name130_{}; //Only byte0 is source produced.
 Known<std::uint32_t> color170_;Known<float> alpha174_,scale178_,rotation17C_;
 std::uint32_t colorDirty180_=0,alphaDirty184_=0,scaleDirty188_=0,rotationDirty18C_=0;
 Phase phase_=Phase::empty;
 bool attached_=false,constructed_=false;
public:
 ~OriginalUiGarageTextOverlay50KG()noexcept;
 OriginalUiGarageTextOverlay50KG(const OriginalUiGarageTextOverlay50KG&)=delete;
 OriginalUiGarageTextOverlay50KG& operator=(const OriginalUiGarageTextOverlay50KG&)=delete;
};
}
