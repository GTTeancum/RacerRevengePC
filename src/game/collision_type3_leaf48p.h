#pragma once
#include "body_integration48c.h"
#include "spring_records48n.h"

namespace rr::game {
struct Type3Segment48P {
    SpringVector4_48N start{},direction{};
    float length=0.f;
};
struct Type3Triangle48P {
    Vector3_48C vertices[3]{},vertexNormals[3]{};
    bool hasVertexNormals=false;
    std::uint32_t normalMode=0;
};
struct Type3PairLeafState48P {
    float fraction=-1.f;
    SpringVector4_48N normal{},point{};
    std::uint32_t hit=0;
};
struct Type3SelectedLeafState48P {
    float fraction=1.f;
    SpringVector4_48N normal{},point{},correction{};
    std::uint32_t hitCount=0;
};
enum class Type3LeafStatus48P {
    Miss,Hit,InvalidInput,UnsupportedInterpolationBoundary,UnsupportedNormalMode
};
// One original10E080 leaf/one-pair iteration. Geometric hit counts regardless
// of nearest update; fraction ties never replace a pair or selected result.
// Flat normal W=0; interpolated mode1 normal W=1; point/correction W=1.
// Correction projects (1-pair.fraction)*direction onto the pair normal, with
// NO length factor. Packed triangle planes are not read at this leaf boundary.
// Float vertex/normal arrays only. Normals absent or mode0 use the geometric
// face; mode1 uses the original ordered two-edge interpolation. Other nonzero
// normal modes are unsupported only when an improving hit would use them.
// Missing original interpolation edge slots and nonfinite intermediate results
// are explicit unsupported/invalid boundaries, never fabricated hit values.
// Nonfinite source XYZ/length/fractions rejected; irrelevant W bits preserved.
// Miss and all errors leave both output states unchanged. Ordinary individually
// rounded binary32 only; no claim about all physical EE/VU exceptional modes.
// Fixed stack, allocation-free, serial, no global state or guest pointers.
Type3LeafStatus48P applyType3Triangle48P(const Type3Segment48P&,
    const Type3Triangle48P&,Type3PairLeafState48P&,Type3SelectedLeafState48P&) noexcept;
}
