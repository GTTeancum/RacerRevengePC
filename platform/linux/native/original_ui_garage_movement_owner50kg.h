#pragma once
#include "../../../src/assets/original_ui_pages50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include <array>
#include <thread>

namespace rr::platform {
class OriginalUiGarageWidget1Owner50KG;
enum class OriginalUiGarageMovementStatus50KG : std::uint8_t {
 complete,authority,phase,source,allocation,scope
};
struct OriginalUiGarageMovementDiagnostic50KG {
 OriginalUiGarageMovementStatus50KG status=OriginalUiGarageMovementStatus50KG::complete;
 std::uint32_t sourcePC=0;
 assets::OriginalUiPageDiagnostic50KG parse{};
};
// Actual counted Movement_Control2AAF40 at authored Garage row6. This is not
// Main's logical movement descriptor, an input registration, or a drawable.
// Source allocation is0xA8 with Entity at0; native backing uses typed fields.
// The one manager population owner alone constructs and advances this leaf.
class OriginalUiGarageMovementOwner50KG final {
 friend class OriginalUiGarageWidget1Owner50KG;
 friend class game::OriginalUiEntityScope50KG;
 using Application=::original_main_entry50KG::Application;
 using Widget=OriginalUiGarageWidget1Owner50KG;
 using C=game::OriginalWreckComponent50LA;
 using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
 using Authority=game::OriginalApplicationEntities50LA;
 using Archive=assets::OriginalUiArchive50KG;
 using Dat=assets::OriginalUiDat50KG;
 using D=OriginalUiGarageMovementDiagnostic50KG;
 using S=OriginalUiGarageMovementStatus50KG;
 enum class Phase:std::uint8_t {empty,constructed,loading,loaded,failed,retiring,retired};
 OriginalUiGarageMovementOwner50KG()noexcept=default;
 bool construct2AAF40(const Application&,std::shared_ptr<Grant>,const Widget*,D&)noexcept;
 bool load2AB010(const Archive&,const Dat&,D&)noexcept;
 bool storeWants1CD8F4(std::uint8_t controller,std::int32_t value,D&)noexcept;
 bool hide1CD908(D&)noexcept;
 // The caller must perform the actual table154[5] write before this witness.
 bool noteRowStored1CD918(D&)noexcept;
 bool attach1CD9F8(D&)noexcept;
 bool identityCurrent()const noexcept;
 bool fieldsCurrent()const noexcept;
 bool payloadCurrent()const noexcept;
 bool preflightRetirement(D&)const noexcept;
 bool retire2AAFA0(D&)noexcept;
 bool fail(D&,S,std::uint32_t)noexcept;
 C entity_;
 const std::thread::id ownerThread_=std::this_thread::get_id();
 const Application* issuer_=nullptr;
 std::shared_ptr<Authority> authority_;
 game::OriginalApplicationAnimationClockView50KG lifetime_;
 std::shared_ptr<Grant> grant_;
 const Widget* widget_=nullptr;
 const C* manager30_=nullptr; // Actual borrowed source pointer; no source retain.
 std::uint32_t type34_=0,selected38_=0,enableOnSelect3C_=0,disableOnSelect40_=0;
 std::uint32_t enableOnDeselect44_=0,disableOnDeselect48_=0,field4C_=0;
 std::array<std::int32_t,2> wantsJoystick50_{};
 std::uint32_t field58_=0;
 // Source5C and tailA4 have no constructor/load writes on this path. No
 // native zero or copied descriptor value is exposed as either source word.
 std::int32_t gridSize60_=0;
 std::array<std::int32_t,16> mappings64_{};
 Phase phase_=Phase::empty;
 bool constructed_=false,typeStored_=false,hidden_=false,rowStored_=false,attached_=false;
 std::uint8_t loadStores_=0,wantsStores_=0; // Native exact partial-write witnesses.
public:
 ~OriginalUiGarageMovementOwner50KG()noexcept;
 OriginalUiGarageMovementOwner50KG(const OriginalUiGarageMovementOwner50KG&)=delete;
 OriginalUiGarageMovementOwner50KG& operator=(const OriginalUiGarageMovementOwner50KG&)=delete;
};
}
