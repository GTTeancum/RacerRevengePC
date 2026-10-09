#include "pod_shape49w.h"
#include "import_collision48g.h"
#include <cstring>

namespace rr::assets {
bool decodePodShapeSource49W(const std::vector<std::uint8_t>& bytes,
    game::PodShapeSource49W& source, DecodeError48C& error) {
    Collision48G decoded;
    CollisionLimits48G limits;
    limits.input_bytes = 4096;
    limits.vertices = 16;
    limits.nodes = 27;
    limits.faces = 28;
    if (!decodeCollision48G(bytes, decoded, error, nullptr, limits)) return false;
    game::PodShapeSource49W result{};
    std::memcpy(result.local40.v, decoded.matrix.data(), sizeof(result.local40));
    std::memcpy(result.rootCenter.v, decoded.nodes[0].center.data(), sizeof(result.rootCenter));
    std::memcpy(result.rootHalf.v, decoded.nodes[0].half.data(), sizeof(result.rootHalf));
    source = result;
    return true;
}
}
