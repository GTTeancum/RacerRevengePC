#pragma once
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"

namespace rr::platform {class OriginalUiSharedDialogMaterialHistory50KG;}
namespace rr::assets {
enum class OriginalUiMaterialEquivalence50KG : std::uint8_t {
    unsupported, notEquivalent, equivalent
};
enum class OriginalUiMaterialEquivalenceError50KG : std::uint8_t {
    none, source, program, mapCount, textureSource, field, value, animation, allocation
};
struct OriginalUiMaterialEquivalenceDiagnostic50KG {
    OriginalUiMaterialEquivalenceError50KG error=OriginalUiMaterialEquivalenceError50KG::none;
    std::uint32_t sourcePC=0;
    const char* field=nullptr;
    OriginalUiPageDiagnostic50KG document;
};
class OriginalUiMaterialBindingState50KG;
class OriginalUiMaterialEquivalenceKey50KG;
bool prepareOriginalUiMaterialEquivalence50KG(const OriginalUiMaterialPass50KG&,
    const OriginalUiImageProvenance50KG* textureOrNull,
    OriginalUiMaterialEquivalenceKey50KG&,OriginalUiMaterialEquivalenceDiagnostic50KG&) noexcept;
OriginalUiMaterialEquivalence50KG compareOriginalUiMaterialEquivalence50KG(
    const OriginalUiMaterialEquivalenceKey50KG&,
    const OriginalUiMaterialEquivalenceKey50KG&) noexcept;

// A COLD CONSTRUCTION value snapshot for ONE original-registry rebuild.
// No represented shader may have been texture-bound by249610 before this
// comparison. That draw mutates shader+30, and29BEA0 does not reset it. A later
// rebuild of retained drawn shaders needs a separate binding-state owner;
// reconstructing these cold keys does not recover that owner.
// Within this bounded initial rebuild, all inputs must have unchanged source
// video/depth construction state. 2476A0 also reads common video ZBUF base and
// format at248060..2480A4. The owning composition must establish that invariant;
// keys from different video configurations/lifetimes must never be compared.
// This is not a draw admission, registry rank, image/frame identity or GPU key.
// Only prepare() may populate it. Default/unprepared keys compare unsupported.
class OriginalUiMaterialEquivalenceKey50KG final {
public:
    bool prepared() const noexcept {return prepared_;}
    bool excludesMerging() const noexcept {return prepared_&&(uniqueAnimation_||uniqueGroup_);}
private:
    friend class rr::platform::OriginalUiSharedDialogMaterialHistory50KG;
    friend class OriginalUiMaterialBindingState50KG;
    bool prepared_=false,enabled_=false,fillColor_=false;
    bool uniqueAnimation_=false,uniqueGroup_=false;
    std::uint8_t program_=0,mapCount_=0,uvIndex_=0;
    const OriginalUiMemberIdentity50KG* texture_=nullptr;
    std::array<std::int32_t,originalUiGsFieldCount50KG> state_{};
    std::array<std::int32_t,6> sampler_{};
    std::string group_;
    friend bool prepareOriginalUiMaterialEquivalence50KG(const OriginalUiMaterialPass50KG&,
        const OriginalUiImageProvenance50KG*,OriginalUiMaterialEquivalenceKey50KG&,
        OriginalUiMaterialEquivalenceDiagnostic50KG&) noexcept;
    friend OriginalUiMaterialEquivalence50KG compareOriginalUiMaterialEquivalence50KG(
        const OriginalUiMaterialEquivalenceKey50KG&,const OriginalUiMaterialEquivalenceKey50KG&) noexcept;
};
// Input must be the unchanged result of the actual source material owner and
// its resolved texture-registry owner, not a caller-selected same-looking image.
// Zero maps requires nullptr. One map requires canonical PST/ATM provenance.
// ATM identity is its whole original member, never its currently selected PST
// frame: original249BD4 compares the texture resource handle. CEL is irrelevant.
// Only original zero/one-map2476A0 and the four admitted program identities are
// covered. Two-map, unknown fields/animations and out-of-width values reject.
// Output is unchanged on failure. A unique pass is successfully prepared but
// never equivalent, including to itself, matching249CA0..249D18.
}
