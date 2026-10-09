#pragma once
#include "model_motion49o.h"
#include "model_bounds49o.h"
#include "authored_geometry49o.h"

namespace rr::game {
struct FreshPodGeometrySources49O {
    assets::PsgHierarchyView49O chariot;
    assets::PsgHierarchyView49O rightEngine;
    assets::MotionView49O forward;
    assets::MotionView49O back;
    assets::MotionNodeBindings49O chariotBindings;
};
struct FreshPodGeometryWorkspace49O {
    ModelLocalMatrix49O local[64];
    ModelBoundsNode49O nodes[64];
    ModelBoundsWorkspace49O bounds;
};
struct FreshPodGeometryResult49O {
    AuthoredGeometryInputs49O sourceInputs; // Selected pre-animation values and live radius.
    ModelBoundsResult49O postMotionBounds;
    AuthoredGeometryBinding49O parameters;
};
enum class FreshPodGeometryError49O : std::uint32_t {
    none, asset, nullcg, motion, bounds, parameters
};
struct FreshPodGeometryDiagnostic49O {
    FreshPodGeometryError49O error;
    ModelMotionDiagnostic49O motion;
};
// Closed numeric projection for the fresh source model context:
// - unparented identity graphics model roots, source hierarchy in file order;
// - source node0 is the root nullcg in both original PSGs;
// - selected Forward/Back are the MCF records resolved for this pod; no Mid;
// - original initialized bases/trig tables; no additional transform effects.
// Capture PRE-animation Chariot nullcg LOCAL XYZ and right-engine nullcg WORLD
// Z; sample155E70 at(.5,0,nonmirror); compute original26F4B0 box aggregate;
// bind the original secondary body mass/inertia, target and joint geometry.
// Stored PSG sphere radius is never substituted for the live aggregate.
// This does not run the whole original MCF/Create graph, cache clone path,
// optional effect callbacks, general IK, body pose/history or contact stages.
// Source views/bindings must be valid and retain their original member bytes.
// The actual Pod01 MCF selection/provenance is checked separately in evidence.
// Fixed-size load-time workspace is caller-owned and may change on failure.
// Output is unchanged on false. Workspace, sources and output are disjoint.
// Strict binary32/FE_TONEAREST; source-backed native admission applies to
// each component. No dynamic allocation, host trigonometry, I/O or RAM access.
bool buildFreshPodGeometry49O(const FreshPodGeometrySources49O&,
    FreshPodGeometryWorkspace49O&, FreshPodGeometryResult49O&,
    FreshPodGeometryDiagnostic49O&) noexcept;
}
