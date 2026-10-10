#pragma once
#include "original_ui_material50kg.h"
#include <array>

namespace rr::assets {
// One original ParticleCode shader with two selectable GS contexts. They are
// alternate states selected by signed packet runs, never two rendered passes.
struct OriginalUiMainParticleMaterial50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    const OriginalUiMemberIdentity50KG* textureIdentity=nullptr;
    const OriginalUiMemberIdentity50KG* templateIdentity=nullptr;
    std::array<OriginalUiNativeMaterial50KG,2> states{};
    bool fogEnabled=false,alphaTestGreaterZero=false;
    bool noDestinationAlphaConsumer=false;
};
// Actual authenticated UIMAIN particle/mmprt.psm + DefaultMT association.
// Normal source runtime only. The exact decoded128x256 mmprt atlas is supplied
// for source alpha/sampler admission and remains owned by the caller.
// The output's noDestinationAlphaConsumer is per-leaf policy, not authority
// over every other leaf, target transition or framebuffer-alpha consumer.
bool prepareOriginalUiMainParticleMaterial50KG(const OriginalUiArchive50KG&,
    const OriginalUiMaterialTemplates50KG&,const OriginalUiMaterialRuntime50KG&,
    const Texture48E&,OriginalUiMainParticleMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
// Identical authenticated mmprt bytes in Options, with Options member identity.
// Main-named immutable policy type is retained for numeric/backend compatibility.
bool prepareOriginalUiOptionsParticleMaterial50KG(const OriginalUiArchive50KG& options,
    const OriginalUiMaterialTemplates50KG&,const OriginalUiMaterialRuntime50KG&,
    const Texture48E&,OriginalUiMainParticleMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
}
