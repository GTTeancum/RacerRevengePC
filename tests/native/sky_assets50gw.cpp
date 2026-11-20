#include "sky50gw_fixture.h"
#include <fstream>
#include <iostream>
#include <iterator>

// Focused standalone check of the ONE sky load (274AE0 + course CUB images) on the
// stock TA archive: the same loadFromCourseData every course uses (common rewrite
// R6; the per-course Request/load path is retired). The TA counts below are this
// check's expectations for that one archive, not inputs to the loader.
int main() {
    namespace sky = sky50gw_fixture;
    namespace a = rr::assets;
    try {
        std::ifstream file("work/native/project/disc/TRACKS/TA.RES", std::ios::binary);
        sky::detail::need(bool(file), "TA archive unavailable");
        const std::vector<std::uint8_t> raw((std::istreambuf_iterator<char>(file)), {});
        a::ResArchive48C archive; a::DecodeError48C decode; std::vector<std::uint8_t> scb; a::World48F world;
        sky::detail::need(a::decodeResource48C(raw, archive, decode) &&
            a::extractResMember48C(archive, "tracks\\ta.scb", scb, decode) && a::decodeWorld48F(scb, world, decode),
            "TA archive / SCB decode");
        sky::Owner owner; sky::Diagnostic d;
        sky::detail::need(owner.loadFromCourseData(archive, world, d), d.message);
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
        sky::detail::need(!owner.loadFromCourseData(archive, world, d) && owner.mesh() == mesh && owner.bank() == bank,
            "Published sky owner must reject reload without invalidation");
        std::cout << "PASS: TA sky through the one course-data load: 48 tiles, 192 vertices, 288 indices, "
                     "3145728 RGBA bytes; bounds, masks, indexes and reload checked\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
