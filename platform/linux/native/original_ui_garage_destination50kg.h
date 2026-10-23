#pragma once
#include "../../../src/assets/original_ui_garage_scene_assets50kg.h"
#include <cstdint>
namespace rr::platform {
// Native destination preparation only. None of these states admits a source
// route or certifies a live Page, render, input, sound or retirement callback.
enum class OriginalUiGarageDestinationStatus50KG : std::uint8_t {
    notRequested,rootOwner,origin,profile,archive,sceneAssets,allocation,
    retainedSourcePrefix,completeFrameUnavailable,resourcesPrepared
};
struct OriginalUiGarageDestinationDiagnostic50KG {
    OriginalUiGarageDestinationStatus50KG status=OriginalUiGarageDestinationStatus50KG::notRequested;
    std::uint64_t proposalSerial=0;
    std::uint32_t requestSourcePC=0;
    // Pending operation, not an assertion that this source PC executed.
    std::uint32_t pendingSourcePC=0;
    assets::OriginalUiArchiveId50KG archive=assets::OriginalUiArchiveId50KG::garage;
    assets::OriginalUiGarageSceneAssetDiagnostic50KG scene{};
};
inline const char* originalUiGarageDestinationStatusName50KG(
    OriginalUiGarageDestinationStatus50KG status)noexcept {
    switch(status){
    case OriginalUiGarageDestinationStatus50KG::notRequested:return "not-requested";
    case OriginalUiGarageDestinationStatus50KG::rootOwner:return "same-root-frontend-owner";
    case OriginalUiGarageDestinationStatus50KG::origin:return "current-tournament-origin";
    case OriginalUiGarageDestinationStatus50KG::profile:return "actual-zero-cash-career19-profile";
    case OriginalUiGarageDestinationStatus50KG::archive:return "authenticated-garage-archive";
    case OriginalUiGarageDestinationStatus50KG::sceneAssets:return "immutable-garage-scene-payload";
    case OriginalUiGarageDestinationStatus50KG::allocation:return "native-resource-allocation";
    case OriginalUiGarageDestinationStatus50KG::retainedSourcePrefix:return "existing-source-prefix-retained";
    case OriginalUiGarageDestinationStatus50KG::completeFrameUnavailable:return "complete-garage-frame-composer-unavailable";
    }
    return "unknown";
}
}
