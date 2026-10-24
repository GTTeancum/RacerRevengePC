#pragma once
#include <array>
#include <cstdint>
#include <memory>
namespace rr::assets { class MaterialTextureAssets50EA; struct OriginalUiMemberIdentity50KG; }
namespace rr::platform {
class MaterialTextureGl50EB;
// Internal observations only, NOT a capability or caller-issued raster receipt.
// Only private Session readers populate the retained Shared copy. Renderer
// authority requires the same live Session/Draw invocation and actual GL work.
struct OriginalUiMenuRasterInputs50KG {
    const void* session=nullptr;
    const void* resources=nullptr;
    const void* camera=nullptr;
    const assets::OriginalUiMemberIdentity50KG* configurationIdentity=nullptr;
    std::shared_ptr<const void> videoLifetime;
    const assets::MaterialTextureAssets50EA* cpu=nullptr;
    const MaterialTextureGl50EB* gpu=nullptr;
    std::uint64_t publication=0;
    std::array<std::uint32_t,4> viewport{};
    std::array<std::uint32_t,2> xyOffsetFixed4{};
};
inline bool sameOriginalUiMenuRasterInputs50KG(const OriginalUiMenuRasterInputs50KG& a,
    const OriginalUiMenuRasterInputs50KG& b)noexcept {
    return a.session==b.session&&a.resources==b.resources&&a.camera==b.camera&&
        a.configurationIdentity==b.configurationIdentity&&a.videoLifetime.get()==b.videoLifetime.get()&&
        !a.videoLifetime.owner_before(b.videoLifetime)&&!b.videoLifetime.owner_before(a.videoLifetime)&&
        a.cpu==b.cpu&&a.gpu==b.gpu&&a.publication==b.publication&&
        a.viewport==b.viewport&&a.xyOffsetFixed4==b.xyOffsetFixed4;
}
}
