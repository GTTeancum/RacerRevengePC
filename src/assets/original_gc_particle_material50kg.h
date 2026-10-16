#pragma once
#include "original_ui_material50kg.h"
#include <array>

namespace rr::assets {
// GC members have their own identity domain. These are not UI archive members,
// and no UI archive ID, logical offset or material provenance is fabricated.
struct OriginalGcParticleMember50KG {
    const char* member;
    std::size_t bytes;
    const char* sha256;
};
const OriginalGcParticleMember50KG& originalGcParticlePsm50KG() noexcept;
const OriginalGcParticleMember50KG& originalGcParticlePst50KG() noexcept;
bool authenticOriginalGcParticlePsm50KG(const std::vector<std::uint8_t>&) noexcept;
bool authenticOriginalGcParticlePst50KG(const std::vector<std::uint8_t>&) noexcept;
struct OriginalGcParticleMaterial50KG {
    const OriginalGcParticleMember50KG* identity=nullptr;
    // Expected source texture identity. The cold owner must authenticate PST
    // bytes and pass their actual decode; dimensions alone are not provenance.
    const OriginalGcParticleMember50KG* textureIdentity=nullptr;
    const OriginalUiMemberIdentity50KG* templateIdentity=nullptr;
    // Alternate GS contexts selected by source signed runs, NOT two passes.
    std::array<OriginalUiNativeMaterial50KG,2> states{};
    bool fogEnabled=false,alphaTestGreaterZero=false;
    bool noDestinationAlphaConsumer=false;
    // Exact decoded atlas maximum, for vertexAlpha * atlasMaximum <=16384.
    // No texture alpha is normalized, replaced or clamped to128.
    std::uint8_t maximumTextureAlpha=0;
};
// Cold, transactional native policy for the exact GC particle\\demoprt.psm.
// Consumes the real authenticated APPSTART DefaultMT owner and owned ordinary
// 435670 runtime. Does not create templates, decode/upload a texture, claim a
// scene alpha-store owner, or borrow Main/Options material/archive identity.
// texture must be the exact authenticated demoprt PST decode supplied by the
// GC cold owner; public Texture48E is not itself an authentication capability.
bool prepareOriginalGcParticleMaterial50KG(const std::vector<std::uint8_t>& psmBytes,
    const OriginalUiMaterialTemplates50KG&,const OriginalUiMaterialRuntime50KG&,
    const Texture48E& texture,OriginalGcParticleMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
}
