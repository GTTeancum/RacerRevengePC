// Compile-only integration fixture. Does not run a game or synthesize driving.
#include "../../platform/linux/native/original_ui_material_adapter50kg.h"
bool compileOriginalUiMaterialAdapter50KG(
    const rr::assets::OriginalUiNativeMaterial50KG& policy,
    const rr::platform::OriginalUiMaterialSpanBinding50KG& binding,
    rr::platform::OriginalUiMaterialSpan50KG& output) noexcept {
    return rr::platform::convertOriginalUiMaterialSpan50KG(policy, binding, output);
}
