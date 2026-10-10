#include "../../src/game/scene_graph49p.h"
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
using namespace rr::game;
using Index = SceneNodeIndex49P;
using Node = SceneNode49P;
using Matrix = BodyMatrix48G;
static std::uint64_t allocations = 0, checks = 0, operations = 0;
[[gnu::noinline]] void* operator new(std::size_t n) {
    void* p = std::malloc(n ? n : 1);
    if (!p) throw std::bad_alloc();
    ++allocations; return p;
}
[[gnu::noinline]] void* operator new[](std::size_t n) { return ::operator new(n); }
[[gnu::noinline]] void operator delete(void* p) noexcept { std::free(p); }
[[gnu::noinline]] void operator delete[](void* p) noexcept { std::free(p); }
[[gnu::noinline]] void operator delete(void* p, std::size_t) noexcept { std::free(p); }
[[gnu::noinline]] void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
static void require(bool value, const char* message) {
    ++checks; if (!value) throw std::runtime_error(message);
}
static Index index(unsigned value) { return static_cast<Index>(value); }
static std::uint32_t bits(float value) { std::uint32_t word; std::memcpy(&word, &value, 4); return word; }
static float fp(std::uint32_t word) { float value; std::memcpy(&value, &word, 4); return value; }
static Matrix identity() { Matrix m{}; for (unsigned i = 0; i < 4; ++i) m.v[5 * i] = 1; return m; }
static Matrix filled(float first) {
    Matrix m{}; for (unsigned i = 0; i < 16; ++i) m.v[i] = first + static_cast<float>(i); return m;
}
static void sameMatrix(const Matrix& a, const Matrix& b, const char* message) {
    for (unsigned i = 0; i < 16; ++i) require(bits(a.v[i]) == bits(b.v[i]), message);
}
static void sameNode(const Node& a, const Node& b, const char* message) {
    sameMatrix(a.localC0, b.localC0, message); sameMatrix(a.world100, b.world100, message);
    require(a.parent180 == b.parent180 && a.firstChild184 == b.firstChild184 &&
        a.nextSibling188 == b.nextSibling188 && a.sceneFlags190 == b.sceneFlags190, message);
}
static void initialize(Node* nodes, unsigned count) {
    for (unsigned i = 0; i < count; ++i) {
        nodes[i].localC0 = identity(); nodes[i].world100 = filled(-100.f - 20.f * i);
        nodes[i].parent180 = nodes[i].firstChild184 = nodes[i].nextSibling188 = Index::none;
        nodes[i].sceneFlags190 = 0;
    }
}
static void attach(Node* nodes, unsigned parent, unsigned child) {
    nodes[child].parent180 = index(parent);
    if (nodes[parent].firstChild184 == Index::none) nodes[parent].firstChild184 = index(child);
    else {
        auto tail = nodes[parent].firstChild184;
        while (nodes[static_cast<unsigned>(tail)].nextSibling188 != Index::none)
            tail = nodes[static_cast<unsigned>(tail)].nextSibling188;
        nodes[static_cast<unsigned>(tail)].nextSibling188 = index(child);
    }
}
template<class Operation> static bool call(Operation operation) {
    const auto before = allocations;
    const bool result = operation();
    const auto after = allocations;
    ++operations;
    require(before == after, "scene operation allocated"); return result;
}
static void readyCombinations() {
    for (unsigned rootFlags = 0; rootFlags < 256; ++rootFlags) {
        for (unsigned childReady = 0; childReady < 2; ++childReady) {
            Node nodes[2]; initialize(nodes, 2); attach(nodes, 0, 1);
            SceneGraph49P graph{nodes, 2};
            nodes[0].sceneFlags190 = static_cast<std::uint8_t>(rootFlags);
            nodes[1].sceneFlags190 = static_cast<std::uint8_t>(0x7eu | childReady);
            nodes[1].localC0.v[12] = 3;
            const Node oldRoot = nodes[0], oldChild = nodes[1];
            Matrix replacement = filled(-8);
            replacement.v[3] = fp(0x80000000u); replacement.v[15] = 2;
            require(call([&] { return setParentlessWorldMatrix49P(graph, index(0), replacement); }), "root assignment failed");
            sameMatrix(nodes[0].localC0, replacement, "whole root matrix copy");
            sameMatrix(nodes[0].world100, oldRoot.world100, "setter changed root cache");
            sameMatrix(nodes[1].localC0, oldChild.localC0, "setter changed child local");
            sameMatrix(nodes[1].world100, oldChild.world100, "setter changed child cache");
            require(nodes[0].sceneFlags190 == ((rootFlags | 0x80u) & 0xfeu), "root flag preservation");
            require(nodes[1].sceneFlags190 == ((rootFlags & 1u) ? 0x7eu : (0x7eu | childReady)), "ready gated child invalidation");
            // Make a supplied ready child below an already-dirty root. A second
            // setter must leave that child ready rather than repair the graph.
            nodes[1].sceneFlags190 |= 1u;
            replacement.v[12] = -3;
            require(call([&] { return setParentlessWorldMatrix49P(graph, index(0), replacement); }), "repeat setter failed");
            require(nodes[1].sceneFlags190 == 0x7fu, "dirty setter visited descendants");
            const Node staleChild = nodes[1];
            require(call([&] { return refreshSceneWorld49P(graph, index(1)); }), "ready getter failed");
            sameNode(nodes[1], staleChild, "ready getter rewrote stale world");
            require((nodes[0].sceneFlags190 & 1u) == 0, "ready getter refreshed dirty parent");
        }
    }
}
static void translationAndFourthLanes() {
    for (unsigned flags = 0; flags < 256; ++flags) {
        Node nodes[2]; initialize(nodes, 2); attach(nodes, 0, 1);
        SceneGraph49P graph{nodes, 2}; nodes[0].localC0 = filled(-20);
        nodes[0].localC0.v[12] = fp(0x80000000u); nodes[0].localC0.v[13] = 9; nodes[0].localC0.v[14] = -4;
        nodes[0].localC0.v[15] = fp(0x80000000u);
        nodes[0].sceneFlags190 = static_cast<std::uint8_t>(flags); nodes[1].sceneFlags190 = 0x45;
        const Node before = nodes[0], childBefore = nodes[1];
        const SceneDisplacement49P displacement{{fp(0x80000000u), -2, 3, 17}};
        require(call([&] { return addParentlessTranslation49P(graph, index(0), displacement); }), "relative add failed");
        for (unsigned i = 0; i < 12; ++i) require(bits(nodes[0].localC0.v[i]) == bits(before.localC0.v[i]), "relative add changed basis");
        require(bits(nodes[0].localC0.v[12]) == 0x80000000u && nodes[0].localC0.v[13] == 7 && nodes[0].localC0.v[14] == -1, "relative add xyz");
        require(bits(nodes[0].localC0.v[15]) == 0x80000000u, "relative add changed w");
        sameMatrix(nodes[0].world100, before.world100, "relative add changed cache");
        sameMatrix(nodes[1].localC0, childBefore.localC0, "relative add child local");
        require(nodes[1].sceneFlags190 == ((flags & 1u) ? 0x44u : 0x45u), "relative add invalidation gate");
    }
}
static void nestedInvalidation() {
    Node nodes[12]; initialize(nodes, 12);
    attach(nodes, 0, 1); attach(nodes, 0, 2); attach(nodes, 0, 3); attach(nodes, 1, 4);
    attach(nodes, 2, 5); attach(nodes, 2, 6); attach(nodes, 5, 7); attach(nodes, 3, 8);
    attach(nodes, 9, 10); attach(nodes, 9, 11);
    for (auto& node : nodes) node.sceneFlags190 = 0x5f;
    nodes[1].sceneFlags190 = nodes[5].sceneFlags190 = 0x5e;
    Node before[12]; std::memcpy(before, nodes, sizeof nodes); SceneGraph49P graph{nodes, 12};
    require(call([&] { return invalidateSceneReady49P(graph, index(1)); }), "nested invalidator");
    for (unsigned i = 0; i < 12; ++i) {
        sameMatrix(nodes[i].localC0, before[i].localC0, "invalidation changed local");
        sameMatrix(nodes[i].world100, before[i].world100, "invalidation changed cache");
        const bool cleared = i == 2 || i == 3 || i == 6 || i == 8;
        require(nodes[i].sceneFlags190 == (cleared ? 0x5eu : before[i].sceneFlags190), "dirty subtree skip or sibling processing");
    }
    // Direct local assignment to a nonroot marks ancestors, not siblings.
    Matrix replacement = filled(2);
    require(call([&] { return setLocalMatrix49P(graph, index(7), replacement); }), "nonroot direct-local setter");
    for (unsigned i = 0; i < 12; ++i) {
        const bool ancestor = i == 0 || i == 2 || i == 5 || i == 7;
        require(bool(nodes[i].sceneFlags190 & 0x80u) == ancestor, "ancestor bit80 route");
    }
    sameMatrix(nodes[7].localC0, replacement, "nonroot copy all lanes");
    const auto previous = nodes[10].sceneFlags190;
    require(call([&] { return markSceneAncestors49P(graph, index(10)); }), "mark ancestors");
    require(nodes[10].sceneFlags190 == (previous | 0x80u) && nodes[9].sceneFlags190 == 0xdfu && nodes[11].sceneFlags190 == 0x5fu, "independent-root ancestor marks");
}
static void lazyProductAndCacheBoundary() {
    Node nodes[4]; initialize(nodes, 4); attach(nodes, 0, 1); attach(nodes, 1, 2); attach(nodes, 0, 3);
    SceneGraph49P graph{nodes, 4};
    // Deliberately general four-lane matrices. Integer arithmetic is exact here.
    nodes[0].localC0 = filled(-3); nodes[1].localC0 = filled(-7);
    nodes[0].sceneFlags190 = 0x2e; nodes[1].sceneFlags190 = 0x9e;
    Node before[4]; std::memcpy(before, nodes, sizeof nodes);
    Matrix expected{};
    for (unsigned row = 0; row < 4; ++row) for (unsigned lane = 0; lane < 4; ++lane) {
        int value = 0;
        for (unsigned k = 0; k < 4; ++k) value += int(nodes[1].localC0.v[4 * row + k]) * int(nodes[0].localC0.v[4 * k + lane]);
        expected.v[4 * row + lane] = float(value);
    }
    require(call([&] { return refreshSceneWorld49P(graph, index(1)); }), "lazy product");
    sameMatrix(nodes[0].world100, nodes[0].localC0, "root lazy copy"); sameMatrix(nodes[1].world100, expected, "row local times parent");
    require(nodes[0].sceneFlags190 == 0x2f && nodes[1].sceneFlags190 == 0x9f, "lazy ready flags");
    sameNode(nodes[2], before[2], "getter refreshed descendant"); sameNode(nodes[3], before[3], "getter refreshed sibling");
    // A ready parent world is the arithmetic source even if its local differs.
    nodes[0].localC0 = filled(200); nodes[1].sceneFlags190 &= 0xfeu;
    require(call([&] { return refreshSceneWorld49P(graph, index(1)); }), "ready parent product");
    sameMatrix(nodes[1].world100, expected, "product ignored ready parent cache");
    Node ready[4]; std::memcpy(ready, nodes, sizeof nodes);
    require(call([&] { return refreshSceneWorld49P(graph, index(1)); }), "repeat getter");
    for (unsigned i = 0; i < 4; ++i) sameNode(nodes[i], ready[i], "repeat getter changed state");
}
static void signedZeroProduct() {
    for (unsigned pattern = 0; pattern < 32; ++pattern) {
        Node nodes[2]; initialize(nodes, 2); attach(nodes, 0, 1); SceneGraph49P graph{nodes, 2};
        for (unsigned i = 0; i < 16; ++i) {
            nodes[0].localC0.v[i] = (i % 5 == 0) ? ((pattern & 1u) ? -1.f : 1.f) : fp((pattern & 2u) ? 0x80000000u : 0u);
            nodes[1].localC0.v[i] = fp((pattern & 4u) ? 0x80000000u : 0u);
        }
        if (pattern & 8u) nodes[1].localC0.v[15] = 1;
        if (pattern & 16u) nodes[0].sceneFlags190 = 1;
        const Matrix& parent = (pattern & 16u) ? nodes[0].world100 : nodes[0].localC0;
        Matrix expected{};
        for (unsigned row = 0; row < 4; ++row) for (unsigned lane = 0; lane < 4; ++lane) {
            volatile float p0 = parent.v[lane] * nodes[1].localC0.v[4 * row];
            volatile float p1 = parent.v[4 + lane] * nodes[1].localC0.v[4 * row + 1];
            volatile float sum1 = p0 + p1;
            volatile float p2 = parent.v[8 + lane] * nodes[1].localC0.v[4 * row + 2];
            volatile float sum2 = sum1 + p2;
            volatile float p3 = parent.v[12 + lane] * nodes[1].localC0.v[4 * row + 3];
            volatile float sum3 = sum2 + p3;
            expected.v[4 * row + lane] = sum3;
        }
        require(call([&] { return refreshSceneWorld49P(graph, index(1)); }), "signed zero product");
        sameMatrix(nodes[1].world100, expected, "signed-zero multiplication elided/reordered");
    }
}
static void deepReverseOrder() {
    Node nodes[64]; initialize(nodes, 64);
    for (unsigned i = 0; i < 63; ++i) attach(nodes, i + 1, i);
    for (auto& node : nodes) { node.localC0.v[12] = 1; node.sceneFlags190 = 0x3e; }
    SceneGraph49P graph{nodes, 64};
    require(validateSceneGraph49P(graph), "reverse node order admission");
    require(call([&] { return refreshSceneWorld49P(graph, index(0)); }), "deep world refresh");
    for (unsigned i = 0; i < 64; ++i) {
        Matrix expected = identity(); expected.v[12] = float(64 - i);
        sameMatrix(nodes[i].world100, expected, "deep world value");
        require(nodes[i].sceneFlags190 == 0x3f, "deep ready flag");
    }
    require(call([&] { return markSceneAncestors49P(graph, index(0)); }), "deep ancestor marking");
    for (const auto& node : nodes) require(node.sceneFlags190 == 0xbf, "deep ancestor flag");
    require(call([&] { return invalidateSceneReady49P(graph, index(63)); }), "deep descendant clearing");
    for (const auto& node : nodes) require(node.sceneFlags190 == 0xbe, "deep dirty flag");
}
static void ordinaryAdmission() {
    Node nodes[3]; initialize(nodes, 3); attach(nodes, 0, 1); attach(nodes, 1, 2);
    SceneGraph49P graph{nodes, 3}; const Matrix m = identity(); const SceneDisplacement49P d{{1, 2, 3, 4}};
    auto unchanged = [&](auto operation) {
        Node before[3]; std::memcpy(before, nodes, sizeof nodes);
        require(!call(operation), "unadmitted operation succeeded");
        for (unsigned i = 0; i < 3; ++i) sameNode(nodes[i], before[i], "failed operation changed state");
    };
    unchanged([&] { return setParentlessWorldMatrix49P(graph, index(1), m); });
    unchanged([&] { return addParentlessTranslation49P(graph, index(1), d); });
    nodes[1].parent180 = index(2); require(!validateSceneGraph49P(graph), "incoherent child parent admitted");
    unchanged([&] { return refreshSceneWorld49P(graph, index(0)); });
    nodes[1].parent180 = index(0);
    nodes[2].firstChild184 = index(0); nodes[0].parent180 = index(2);
    require(!validateSceneGraph49P(graph), "coherent parent cycle admitted");
    unchanged([&] { return invalidateSceneReady49P(graph, index(0)); });
    nodes[2].firstChild184 = Index::none; nodes[0].parent180 = Index::none;
    nodes[1].nextSibling188 = index(1); require(!validateSceneGraph49P(graph), "sibling cycle admitted");
    unchanged([&] { return setLocalMatrix49P(graph, index(0), m); });
    nodes[1].nextSibling188 = Index::none;
    nodes[0].firstChild184 = Index::none; require(!validateSceneGraph49P(graph), "missing child link admitted");
    nodes[0].firstChild184 = index(1);
    nodes[0].localC0.v[0] = std::numeric_limits<float>::max(); nodes[1].localC0.v[0] = 2;
    unchanged([&] { return refreshSceneWorld49P(graph, index(2)); });
    nodes[0].localC0 = identity(); nodes[1].localC0 = identity();
    nodes[0].localC0.v[12] = std::numeric_limits<float>::max();
    SceneDisplacement49P huge{{std::numeric_limits<float>::max(), 0, 0, 0}};
    unchanged([&] { return addParentlessTranslation49P(graph, index(0), huge); });
    nodes[0].localC0 = identity();
    require(std::fesetround(FE_DOWNWARD) == 0, "set non-nearest mode");
    unchanged([&] { return setLocalMatrix49P(graph, index(0), m); });
    unchanged([&] { return refreshSceneWorld49P(graph, index(0)); });
    require(std::fesetround(FE_TONEAREST) == 0, "restore nearest mode");
    require(validateSceneGraph49P(graph), "restored topology rejected");
}
int main() {
    try {
        require(std::fesetround(FE_TONEAREST) == 0, "nearest rounding mode");
        readyCombinations(); translationAndFourthLanes(); nestedInvalidation();
        lazyProductAndCacheBoundary(); signedZeroProduct(); deepReverseOrder(); ordinaryAdmission();
        std::cout << "PASS SceneGraphAPI49P: " << operations << " operations, " << checks
                  << " checks, zero hot allocations\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL SceneGraphAPI49P: " << error.what() << '\n'; return 1;
    }
}
