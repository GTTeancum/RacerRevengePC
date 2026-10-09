#pragma once
#include "scene_graph49p.h"
#include "chariot_scene49u.h"
#include "secondary_activation49v.h"
#include <cstdint>

namespace rr::game {
// Six otherwise-unrepresented full-vector words. The existing49M70/B0/180/190
// words and the exact52-byte49V supplement retain their original owners.
struct GraphBodyExtraLanes49W {
    float position60W, worldAngularVelocityA0W;
    float positionCorrection1A0W, rotationCorrection1B0W;
    float accumulator1C0W, accumulator1D0W;
};
static_assert(sizeof(GraphBodyExtraLanes49W) == 24, "Six additional source words");
struct GraphBodyLaneRefs49W {
    const float& velocity70W;
    const float& angularVelocityB0W;
    float& force180W;
    float& torque190W;
};
// Actual body component routing facts are unrelated to scene children. The
// empty141ED0 route is (componentFlags24&2)==0 OR !firstComponentChildPresent18.
// Observer presence is the physical scene's actual+19C, not vehicle+19C.
// Enabled numerical calls require the observed14EE40 method; every call uses
// the actual1654B0 callback. No constructor/lifecycle claim is made.
enum class GraphBodyIntegrator49W : std::uint32_t { OriginalAdamsBashforth2, Unsupported };
enum class GraphBodyCallback49W : std::uint32_t { Clear238, Unsupported };
struct GraphBodyDomain49W {
    std::uint32_t componentFlags24;
    bool firstComponentChildPresent18, rootObserverPresent19C;
    GraphBodyIntegrator49W integrationMethod290;
    GraphBodyCallback49W callbackMethod9C;
};
enum class GraphBodyDiagnostic49W : std::uint32_t {
    None, StorageAlias, RoundingMode, ComponentRoute, CallbackMethod,
    GraphTopology, SceneBinding, SceneProjection, Observer, IntegrationMethod,
    Arithmetic
};
// Caller-owned scratch, never an independent state authority. Contents after
// either result are unspecified. Keep it disjoint from all input/output storage.
struct GraphBodyWorkspace49W {
    RootBodyState48H body;
    SecondaryActivationSupplement49V supplement;
    GraphBodyExtraLanes49W extra;
    SceneNode49P nodes[maxSceneNodes49P];
    float force180W, torque190W;
};
// Source167C40..16836C including the actual empty141ED0 component route.
// sceneBinding==none is the original missing-body288 branch. Otherwise require
// a parentless node in the complete coherent<=64-node49P forest. Real children,
// including190->194, remain present. root.scene is a bitwise projection of the
// bound node.world100; low sceneFlags190 agrees with its flags, upper24 preserved.
// Ready world may differ from local. Graph is the sole matrix/flag authority.
// Cached speed34/anglesE0, body20C, phase/time are outside this transaction.
//
// FE_TONEAREST, strict binary32/no contraction, serial access, finite consumed
// operands/intermediates, initialized433B80=(+0,+0,+0,1), and finite surviving
// VU scratch self-subtraction operands. No arbitrary observer/component route,
// integrator, callback, ownership, spawn, reset, query or contact is supplied.
// Disabled/missing branches preserve supplement and numerical fields, and
// clear six full load/correction vectors plus callback238 in original order.
// No allocation. False leaves body/supplement/extra/lane refs/flags/graph intact;
// diagnostic and workspace are excluded from that guarantee. Every storage
// range must be disjoint, except that lane refs may lie together in their
// existing49M owner. Parameters/center/domain/graph descriptor also must not
// overlap mutable storage. The diagnostic must not overlap any other range.
bool advanceGraphBody49W(RootBodyState48H& body,
    SecondaryActivationSupplement49V& supplement, GraphBodyExtraLanes49W& extra,
    GraphBodyLaneRefs49W lanes, std::uint32_t& sceneFlags190,
    const BodyParameters48C& parameters, const Vector3_48C& centerOffset,
    SceneNodeIndex49P sceneBinding, bool enabled204,
    const GraphBodyDomain49W& domain, float elapsedFactor,
    const SceneGraph49P& graph, GraphBodyWorkspace49W& workspace,
    GraphBodyDiagnostic49W& diagnostic) noexcept;
namespace detail {
// Source28BAB0. Transaction-scratch leaf; a false result can leave lazy caches
// changed. Input W is unused, output W and VF9.W are source1. Any valid graph
// node is admitted (body wrapper itself requires a parentless root).
bool pointGraph49W(const SceneGraph49P&, SceneNodeIndex49P,
    const VisualVector49R&, SceneVectorContext49U&, VisualVector49R&) noexcept;
// Source28B840/112B20: cosine then sine, second VU axis normalization, matrix
// product, saved translation restoration, ancestor marking and ready-gated
// invalidation. Internal scratch leaf, not an independent atomic transaction.
bool rotateAxisGraph49W(const SceneGraph49P&, SceneNodeIndex49P,
    const VisualVector49R& axis, float angle) noexcept;
}
}
