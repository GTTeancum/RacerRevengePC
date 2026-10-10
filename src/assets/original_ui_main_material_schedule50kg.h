#pragma once
#include "original_ui_archive50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rr::assets {
constexpr std::size_t originalUiMainScheduledMaterialCount50KG = 16;
enum class OriginalUiMainMaterialScheduleError50KG : std::uint8_t {
    none, alreadyLoaded, archive, member, registryToken, allocation
};
struct OriginalUiMainMaterialScheduleDiagnostic50KG {
    OriginalUiMainMaterialScheduleError50KG error = OriginalUiMainMaterialScheduleError50KG::none;
    std::size_t entry = 0;
    std::uint32_t sourcePC = 0;
};
struct OriginalUiMainMaterialScheduleEntry50KG {
    const char* registeredName = nullptr; // canonical lowercase, forward slashes
    const OriginalUiMemberIdentity50KG* registryIdentity = nullptr;
    std::uint32_t registryTokenOrdinal = 0;
    std::size_t registryByteOffset = 0;
    // Rank in the closed Main submission projection, NOT an original global
    // group address/index. Shared idle groups can occupy intervening indices.
    std::uint8_t projectedLeafOrdinal = 0;
    std::uint8_t shaderPassCount = 1, textureMapCount = 1;
    const OriginalUiMemberIdentity50KG* psmIdentity = nullptr;
    // mmlogo and four shared page button prompts use actual no-PSM branches.
    const OriginalUiMemberIdentity50KG* fallbackTextureIdentity = nullptr;
};
// Concrete cold Main schedule, from actual APPSTART/Mat_Stbl and UIMAIN assets.
// 14D440 rebuilds passes by material-registry order; 29BED0 registers each
// material's shader passes ascending; 29C150/24B7B0 preserve FIFO per leaf.
// All sixteen admitted Main materials have one shader pass (mmprt has TWO maps).
// They cannot alias each other. Shared NoTexture/SplitScreenBars equivalence
// can change an absolute group index but cannot cross another Main leaf.
// This owner provides only the Main projection. Unknown names, other pages,
// shared overlay submissions, runtime shader replacement and foreign scenes
// are outside it. The draw assembler must reject packets not found here.
// It does not admit a particle renderer, alpha-target lifecycle, or GPU state.
class OriginalUiMainMaterialSchedule50KG final {
public:
    bool load(const OriginalUiArchive50KG& appStart,
        const OriginalUiArchive50KG& mainMenu,const OriginalUiArchive50KG& overlays,
        OriginalUiMainMaterialScheduleDiagnostic50KG&) noexcept;
    bool loaded() const noexcept { return loaded_; }
    const std::array<OriginalUiMainMaterialScheduleEntry50KG,
        originalUiMainScheduledMaterialCount50KG>& entries() const noexcept { return entries_; }
    // ASCII case-insensitive; slash/backslash equivalent; exact full name.
    // No basename lookup, extension removal or guessed default material.
    const OriginalUiMainMaterialScheduleEntry50KG* find(const std::string&) const noexcept;
private:
    bool loaded_ = false;
    std::array<OriginalUiMainMaterialScheduleEntry50KG,
        originalUiMainScheduledMaterialCount50KG> entries_{};
};
}
