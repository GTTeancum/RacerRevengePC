#pragma once
#include "../../../src/game/original_wreck_components50la.h"
#include <array>
#include <atomic>
#include <cstdint>
namespace rr::platform {
class OriginalUiGarageWidget1Owner50KG;
class OriginalUiCourseOwnedLifecycle50KG;
// The actual process8098/809C guards shared by every source2A9000 caller.
// Owner-neutral storage grants no Widget/renderer/resource authority.
class OriginalUiTextHashGuards50KG final {
 friend class OriginalUiGarageWidget1Owner50KG;
 friend class OriginalUiCourseOwnedLifecycle50KG;
 static std::atomic<std::uint8_t>& hashGuard8098()noexcept {static std::atomic<std::uint8_t> v{0};return v;}
 static std::atomic<std::int32_t>& hashAssertion3D6B10()noexcept {static std::atomic<std::int32_t> v{0};return v;}
 static std::atomic<std::uint8_t>& idGuard809C()noexcept {static std::atomic<std::uint8_t> v{0};return v;}
 static std::atomic<std::int32_t>& idAssertion3D6B18()noexcept {static std::atomic<std::int32_t> v{0};return v;}
};
// Owner-neutral native backing of the actual source objects. Default storage
// is NOT a source constructor, source null, reference, readiness or publication.
// The typed owner alone records source field knowledge and acquisition.
struct OriginalUiOverlayTextFields50KG {
        game::OriginalWreckComponent50LA entity;const game::OriginalWreckComponent50LA* manager30=nullptr;
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0;
        std::uint32_t field4C=0,field50=0,field54=0,field58=0;
        void* lines60=nullptr;std::uint32_t field64=0;
        std::array<char,128> font68{},materialE8{};
        std::int32_t lineCount168=0,dynamicLength16C=0,justify170=0;
        float leading174=0;std::int32_t dynamic178=0;
        std::uint32_t color17C=0;std::int32_t x180=0,y184=0,height188=0;
        std::int32_t alpha18C=255;float depth190=1;
        std::uint32_t highlight194=0,highlight198=0,disabled19C=0,disabled1A0=0,field1A4=0;
    };
struct OriginalUiTextOverlayFields50KG {
 template<class T>struct Known {
  T value{};bool produced=false;
  void store(T v)noexcept{value=v;produced=true;}
 };
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
};
}
