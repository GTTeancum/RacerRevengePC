#include "scene_graph49p.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using Index = SceneNodeIndex49P;
constexpr Index none = Index::none;
std::int32_t integer(Index index) noexcept { return static_cast<std::int32_t>(index); }
bool inRange(Index index, std::uint32_t count) noexcept {
    return integer(index) >= 0 && static_cast<std::uint32_t>(integer(index)) < count;
}
bool validLink(Index index, std::uint32_t count) noexcept {
    return index == none || inRange(index, count);
}
SceneNode49P& at(const SceneGraph49P& graph, Index index) noexcept {
    return graph.nodes[static_cast<std::uint32_t>(integer(index))];
}
bool admit(const SceneGraph49P& graph, Index index) noexcept {
    return std::fegetround() == FE_TONEAREST &&
        validateSceneGraph49P(graph) && inRange(index, graph.count);
}
void copyMatrix(BodyMatrix48G& destination, const BodyMatrix48G& source) noexcept {
    // Source matrix copies are integer/vector loads and stores, not FP adds.
    std::memmove(&destination, &source, sizeof destination);
}
void storeFlags(const SceneGraph49P& graph, Index index, std::uint8_t value) noexcept {
#ifdef RR_SCENE49P_FLAG_STORE
    const auto previous = at(graph, index).sceneFlags190;
#endif
    at(graph, index).sceneFlags190 = value;
#ifdef RR_SCENE49P_FLAG_STORE
    // Optional compile-time evidence hook, absent from production builds.
    RR_SCENE49P_FLAG_STORE(index, previous, value);
#endif
}
void markAncestors(const SceneGraph49P& graph, Index index) noexcept {
    // 28C4F0's unrolled parent chain. Admission proves termination.
    while (index != none) {
        auto& node = at(graph, index);
        storeFlags(graph, index, static_cast<std::uint8_t>(node.sceneFlags190 | 0x80u));
        index = node.parent180;
    }
}
void invalidate(const SceneGraph49P& graph, Index index) noexcept {
    // Equivalent to 289A20's two-level unrolling. Sibling recursion comes
    // first even when this node was already dirty. Depth is bounded by 64.
    auto& node = at(graph, index);
    if (node.nextSibling188 != none) invalidate(graph, node.nextSibling188);
    if ((node.sceneFlags190 & 1u) == 0) return;
    storeFlags(graph, index, static_cast<std::uint8_t>(node.sceneFlags190 & 0xfeu));
    if (node.firstChild184 != none) invalidate(graph, node.firstChild184);
}
void afterLocalWrite(const SceneGraph49P& graph, Index index) noexcept {
    markAncestors(graph, index);
    auto& node = at(graph, index);
    if ((node.sceneFlags190 & 1u) == 0) return;
    storeFlags(graph, index, static_cast<std::uint8_t>(node.sceneFlags190 & 0xfeu));
    if (node.firstChild184 != none) invalidate(graph, node.firstChild184);
}
struct Math {
    bool valid = true;
    float keep(float value) noexcept {
        valid = valid && std::isfinite(value);
        return value;
    }
    float add(float a, float b) noexcept {
        volatile float value = a + b;
        return keep(value);
    }
    float multiply(float a, float b) noexcept {
        volatile float value = a * b;
        return keep(value);
    }
};
bool product(const BodyMatrix48G& local, const BodyMatrix48G& parent,
             BodyMatrix48G& result) noexcept {
    for (unsigned lane = 0; lane < 16; ++lane)
        if (!std::isfinite(local.v[lane]) || !std::isfinite(parent.v[lane])) return false;
    Math math;
    for (unsigned row = 0; row < 4; ++row) {
        for (unsigned lane = 0; lane < 4; ++lane) {
            // VMULAx, VMADDAy, VMADDAz, VMADDw at289958..28999C.
            // Keep every product/add binary32, including zero/fourth lanes.
            const float x = math.multiply(parent.v[lane], local.v[4 * row]);
            const float y = math.multiply(parent.v[4 + lane], local.v[4 * row + 1]);
            const float xy = math.add(x, y);
            const float z = math.multiply(parent.v[8 + lane], local.v[4 * row + 2]);
            const float xyz = math.add(xy, z);
            const float w = math.multiply(parent.v[12 + lane], local.v[4 * row + 3]);
            result.v[4 * row + lane] = math.add(xyz, w);
        }
    }
    return math.valid;
}
}

bool validateSceneGraph49P(const SceneGraph49P& graph) noexcept {
    if (graph.count > maxSceneNodes49P || (graph.count != 0 && graph.nodes == nullptr)) return false;
    for (std::uint32_t i = 0; i < graph.count; ++i) {
        const auto& node = graph.nodes[i];
        if (!validLink(node.parent180, graph.count) ||
            !validLink(node.firstChild184, graph.count) ||
            !validLink(node.nextSibling188, graph.count)) return false;
        if (node.parent180 == none && node.nextSibling188 != none) return false;
    }
    bool linked[maxSceneNodes49P]{};
    for (std::uint32_t i = 0; i < graph.count; ++i) {
        Index child = graph.nodes[i].firstChild184;
        while (child != none) {
            const auto offset = static_cast<std::uint32_t>(integer(child));
            if (linked[offset] || at(graph, child).parent180 != static_cast<Index>(i)) return false;
            linked[offset] = true;
            child = at(graph, child).nextSibling188;
        }
    }
    for (std::uint32_t i = 0; i < graph.count; ++i) {
        if (linked[i] != (graph.nodes[i].parent180 != none)) return false;
        Index parent = graph.nodes[i].parent180;
        std::uint32_t depth = 0;
        while (parent != none) {
            if (++depth >= graph.count) return false;
            parent = at(graph, parent).parent180;
        }
    }
    return true;
}

bool setParentlessWorldMatrix49P(const SceneGraph49P& graph, Index index,
                                const BodyMatrix48G& matrix) noexcept {
    if (!admit(graph, index) || at(graph, index).parent180 != none) return false;
    copyMatrix(at(graph, index).localC0, matrix);
    afterLocalWrite(graph, index);
    return true;
}
bool addParentlessTranslation49P(const SceneGraph49P& graph, Index index,
                                const SceneDisplacement49P& displacement) noexcept {
    if (!admit(graph, index) || at(graph, index).parent180 != none) return false;
    auto& node = at(graph, index);
    float translated[3];
    Math math;
    for (unsigned lane = 0; lane < 3; ++lane) {
        if (!std::isfinite(node.localC0.v[12 + lane]) ||
            !std::isfinite(displacement.v[lane])) return false;
        translated[lane] = math.add(node.localC0.v[12 + lane], displacement.v[lane]);
    }
    if (!math.valid) return false;
    for (unsigned lane = 0; lane < 3; ++lane) node.localC0.v[12 + lane] = translated[lane];
    afterLocalWrite(graph, index);
    return true;
}
bool setLocalMatrix49P(const SceneGraph49P& graph, Index index,
                      const BodyMatrix48G& matrix) noexcept {
    if (!admit(graph, index)) return false;
    copyMatrix(at(graph, index).localC0, matrix);
    afterLocalWrite(graph, index);
    return true;
}
bool markSceneAncestors49P(const SceneGraph49P& graph, Index index) noexcept {
    if (!admit(graph, index)) return false;
    markAncestors(graph, index);
    return true;
}
bool invalidateSceneReady49P(const SceneGraph49P& graph, Index index) noexcept {
    if (!admit(graph, index)) return false;
    invalidate(graph, index);
    return true;
}
bool refreshSceneWorld49P(const SceneGraph49P& graph, Index index) noexcept {
    if (!admit(graph, index)) return false;
    if ((at(graph, index).sceneFlags190 & 1u) != 0) return true;
    // Stage only the required dirty ancestor chain. This allows arithmetic
    // admission before any mutation, without copying or rewriting other nodes.
    Index chain[maxSceneNodes49P];
    BodyMatrix48G worlds[maxSceneNodes49P];
    std::uint32_t length = 0;
    Index cursor = index;
    while (cursor != none && (at(graph, cursor).sceneFlags190 & 1u) == 0) {
        chain[length++] = cursor;
        cursor = at(graph, cursor).parent180;
    }
    for (std::uint32_t remaining = length; remaining != 0; --remaining) {
        const auto slot = remaining - 1;
        const auto& node = at(graph, chain[slot]);
        if (node.parent180 == none) copyMatrix(worlds[slot], node.localC0);
        else {
            const auto& parentWorld = slot + 1 < length ? worlds[slot + 1] : at(graph, node.parent180).world100;
            if (!product(node.localC0, parentWorld, worlds[slot])) return false;
        }
    }
    // Original order: each ancestor's complete world, then its ready byte,
    // followed by the child's matrix and ready byte. No child/sibling refresh.
    for (std::uint32_t remaining = length; remaining != 0; --remaining) {
        const auto slot = remaining - 1;
        auto& node = at(graph, chain[slot]);
        copyMatrix(node.world100, worlds[slot]);
        storeFlags(graph, chain[slot], static_cast<std::uint8_t>(node.sceneFlags190 | 1u));
    }
    return true;
}
}
