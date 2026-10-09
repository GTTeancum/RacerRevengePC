#pragma once
#include "model_geometry49o.h"
#include <cstddef>
#include <cstdint>

namespace rr::assets {
struct MotionNodeBinding49O {
    char name[50];
    std::uint8_t sceneNode;
    bool translationFlag38;
    bool listedFlag3c;
};
struct MotionNodeBindings49O {
    std::uint32_t count;
    MotionNodeBinding49O rows[64];
};
enum class MotionBindingError49O : std::uint32_t {
    none, hierarchy, name, limit, line, unknownNode
};
struct MotionBindingDiagnostic49O {
    MotionBindingError49O error;
    std::size_t offset;
};
// Fresh source hierarchy enumeration152EA0: emit siblings in source order,
// then visit each sibling's child group. PSG parents must precede children;
// exactly one root is admitted. Appending children28CC90/28BF50 establishes
// sibling order. Rows are NOT VUT line numbers or assumed PSG indices.
// Names are uppercased as in152EE0..152F18 and VUT153218/30B100.
// VUT lines rename the FIRST matching current row, without moving its scene
// reference. A suffix whose first character is T sets source+38; every listed
// row sets+3C. Unlisted rows retain both flags clear.155E70 does not consume
// these flags, but they are preserved instead of inventing their meaning.
// Narrow text admission:1..16384 bytes,1..255 bytes per nonempty line, ASCII
// identifier names1..49 chars with no whitespace, '=', or ','; LF/CRLF; optional
// one suffix token. Unknown names fail rather than executing source logging.
// Fixed storage, zero allocation; output unchanged on false. The source PSG
// view must remain valid. This is a bounded binding projection, not MCF load.
bool readMotionNodeBindings49O(const PsgHierarchyView49O&,
    const char* vut, std::size_t bytes, MotionNodeBindings49O&,
    MotionBindingDiagnostic49O&) noexcept;
}
