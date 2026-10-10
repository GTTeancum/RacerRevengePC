#include "sky50gw_fixture.h"
#include <iostream>

int main() {
    namespace sky = sky50gw_fixture;
    try {
        sky::Owner owner; sky::Diagnostic d;
        sky::Request request{"work/native/project/disc/TRACKS/TA.RES", "evidence/sky50GW"};
        auto missing = request; missing.textureRoot = "solo-sky50gw/no-such-image-root";
        sky::detail::need(!owner.load(missing, d) && !owner.loaded() && !owner.mesh(),
            "Missing original decode must reject transactionally");
        sky::detail::need(owner.load(request, d), d.message);
        const auto* mesh = owner.mesh(); const auto* spans = owner.spans(); const auto* bank = owner.bank();
        sky::detail::need(mesh && spans && bank && mesh->vertices.size() == 192 &&
            mesh->indices.size() == 288 && spans->size() == 48 && bank->textureCount() == 48 &&
            bank->slotCount() == 48 && bank->rgbaByteCount() == 3145728, "Actual sky closure counts");
        sky::detail::need(mesh->bounds.minimum[0] == -25600 && mesh->bounds.maximum[0] == 25600 &&
            mesh->bounds.minimum[1] == 1024 && mesh->bounds.maximum[1] == 26624 &&
            mesh->bounds.minimum[2] == -25600 && mesh->bounds.maximum[2] == 25600, "Source camera-relative bounds");
        for (std::size_t i = 0; i < spans->size(); ++i) {
            const auto& span = (*spans)[i];
            sky::detail::need(span.indexStart == i*6 && span.vertexStart == i*4 && span.indexCount == 6 &&
                span.vertexCount == 4 && span.textureIndex == i && span.face < 5 &&
                (span.face == 4 || span.tile < 8), "Authored active mask only");
            for (unsigned j = 0; j < 6; ++j)
                sky::detail::need(mesh->indices[i*6+j] >= i*4 && mesh->indices[i*6+j] < i*4+4,
                    "Sky tile index extent");
            const auto* texture = bank->texture(static_cast<std::uint32_t>(i));
            sky::detail::need(texture && texture->texture.width == 128 && texture->texture.height == 128,
                "Actual selected mip size");
        }
        sky::detail::need(!owner.load(request, d) && owner.mesh() == mesh && owner.bank() == bank,
            "Published sky owner must reject reload without invalidation");
        std::cout << "PASS: actual TA sky48tiles,192vertices,288indices,3145728RGBAbytes; source bounds, masks, indexes, missing-image transaction and reload checked\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
