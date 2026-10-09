#pragma once
#include "type3_query48p.h"
#include "group_shape48t.h"

namespace rr::game {
// References are table indices plus one; zero denotes an absent binding. Opaque
// nonzero candidate identities are unique per object table, independently of
// how often an object occurs in the outer order or in group member orders.
using GroupQueryRef48U=std::uint32_t;
struct GroupQueryObject48U {
    const Type3Candidate48P* candidate=nullptr;
    GroupQueryRef48U group=0,shape0=0,shape45=0;
    std::uint16_t position[3]{};
    std::int8_t scale10=0,scale11=0;
    bool hasShape=true;
};
struct GroupQueryGroup48U {
    const GroupQueryRef48U* members=nullptr;std::size_t memberCapacity=0;
    std::int32_t memberCount=0;
    float base180=1,other184=1,base18C=1,other194=1;
};
// One entry is one physical shape. Its canonical mesh supplies BOTH traversal
// geometry and the root-node center/half consumed by the48T group setter.
// Candidate geometry matrices/mesh pointers are not an alternate source when
// shape0 is bound; geometry.id must still match that candidate's identity.
struct GroupQueryShape48U {
    BodyMatrix48G local40{};
    const assets::CollisionMesh48I* mesh=nullptr;
};
// The five fields135CF0 changes on shape4/5, stored by physical shape identity,
// not member object. References sharing an entry must agree on shape tag4 or5;
// the different physical layouts cannot be aliased through this compact view.
// This is a native layout boundary, not an original mixed-alias rejection.
struct GroupQueryShape45State48U {
    Vector3_48C translation{};float scale18C=0,scale180=0;
};
enum class GroupQueryCallback48U : std::uint8_t { None,Static294BB0,Unknown };
struct GroupQueryInput48U {
    const GroupQueryObject48U* objects=nullptr;std::size_t objectCount=0;
    const GroupQueryGroup48U* groups=nullptr;std::size_t groupCount=0;
    const GroupQueryShape48U* shapes=nullptr;std::size_t shapeCount=0;
    const std::uint32_t* shape45Tags=nullptr;std::size_t shape45Count=0;
    const GroupQueryRef48U* outerOrder=nullptr;std::size_t outerCount=0;
    float worldPositionScale=1;
    // Optional SAME query object in the object table. Its identity/shape3 and
    // activation must agree with the template. Preparation updates its sphere;
    // flags and copied40/50 follow the query, including later group visits.
    GroupQueryRef48U queryObject=0;
    // Explicit query+E4 identity, authoritative instead of callbackEnabled.
    GroupQueryCallback48U queryCallback=GroupQueryCallback48U::Static294BB0;
};
struct GroupQueryObjectState48U {
    SpringVector4_48N center{};float radius=0;
    std::uint8_t flagsF8=0;
    std::array<float,4> copied40{},copied50{};
};
struct GroupQueryState48U {
    const GroupQueryObjectState48U* objects=nullptr;std::size_t objectCapacity=0;
    const GroupShapeState48T* shapes=nullptr;std::size_t shapeCapacity=0;
    const GroupQueryShape45State48U* shapes45=nullptr;std::size_t shape45Capacity=0;
};
struct GroupQueryBuffers48U {
    Type3RayBuffers48P rays{};
    SpringRecord48N* records=nullptr;std::size_t recordCapacity=0;
    GroupQueryObjectState48U* objects=nullptr;std::size_t objectCapacity=0;
    GroupShapeState48T* shapes=nullptr;std::size_t shapeCapacity=0;
    GroupQueryShape45State48U* shapes45=nullptr;std::size_t shape45Capacity=0;
};
struct GroupQueryWorkspace48U {
    GroupQueryBuffers48U buffers{};
    std::uint32_t* treeStack=nullptr;std::size_t treeStackCapacity=0;
    // Optional exact-count permutation ordered by current nonzero identities.
    // Revalidated every call, including references and strict key order. Null
    // plus zero keeps the small-table pairwise identity check. This is a read-
    // only hint, not a cache or trusted validation token.
    const GroupQueryRef48U* identityOrder=nullptr;std::size_t identityOrderCount=0;
};
// Additive original122660 type3 outer-group seam. Explicit LimitedList context,
// positive ray count and zero endpoint metadata only. Direct collision-list
// tags take precedence over group tags. Group XZ coarse bounds are inclusive;
// members bypass ordinary sphere/filter gates and enter126F40 after135CF0.
// Tag0 members update canonical shared shape state through48T in source order;
// shape4/5 update their separately bound compact shared shape state.
// Reached null shapes/shape bindings, unknown callbacks and
// dynamic294BB0 bodies are explicit UnsupportedScope boundaries. Compound1
// retains the existing dispatcher gate-specific UnsupportedScope boundary;
// other nonzero member tags preserve the original setter no-op/dispatch miss.
//
// Every declared mutable span (including inactive tails) is mutually disjoint
// and avoids all input descriptors/nested mesh/filter/member/name spans. All
// source ray/length/record slots are copied; only table counts of object/shape
// state commit. Destination tails remain unchanged. Any non-Complete status
// preserves ALL destination/result bytes; scratch may change. No allocation,
// owning filter copy, thread, hidden world lookup or persistent cache occurs.
// Mesh topology retains the48P importer precondition and finite numeric scope.
// candidateCount counts outer list entries; geometryHits/callbacks count member
// or direct geometry/callback visits, including repeated references.
Type3QueryStatus48P queryStaticType3GroupsBounded48U(
    const Type3QueryInput48P&,const Type3QueryContext48P&,const Type3QuerySource48P&,
    const GroupQueryInput48U&,const GroupQueryState48U&,
    const GroupQueryWorkspace48U&,const GroupQueryBuffers48U& destination,
    Type3QueryResult48P&) noexcept;
}
