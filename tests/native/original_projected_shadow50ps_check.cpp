// Host check for src/game/original_projected_shadow50ps.*: no GL, no assets.
// Build alongside the module: cl /std:c++17 /EHsc /I. this.cpp
//   ..\..\src\game\original_projected_shadow50ps.cpp (plus the include path the
//   other *_check.cpp files use for src/game/vehicle_draw50cm.h).
#include "../../src/game/original_projected_shadow50ps.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace g = rr::game;
namespace {
int failures = 0;
void expect(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL %s\n", what); ++failures; }
}
bool near(float a, float b, float e = 1e-4f) { return std::fabs(a - b) <= e; }
g::ShadowBox50PS box(float x0, float y0, float z0, float x1, float y1, float z1) {
    g::ShadowBox50PS b; b.minimum[0] = x0; b.minimum[1] = y0; b.minimum[2] = z0;
    b.maximum[0] = x1; b.maximum[1] = y1; b.maximum[2] = z1; b.valid = true; return b;
}
g::ShadowRacerInput50PS pod(float x, float y, float z) {
    g::ShadowRacerInput50PS r; r.present = true; r.referenceValid = true;
    r.reference[0] = x; r.reference[1] = y; r.reference[2] = z;
    for (unsigned s = 0; s < 3; ++s) { r.lodCount[s] = 3; r.box[s] = box(x - 5, y - 2, z - 10, x + 5, y + 2, z + 10); }
    return r;
}
void st(const g::ProjectedShadow50PS& s, const float (&p)[3], float& u, float& v) {
    u = s.texture[0] * p[0] + s.texture[4] * p[1] + s.texture[8] * p[2] + s.texture[12];
    v = s.texture[1] * p[0] + s.texture[5] * p[1] + s.texture[9] * p[2] + s.texture[13];
}
} // namespace

int main() {
    static g::OriginalProjectedShadowFrame50PS frame;
    g::ProjectedShadowDiagnostic50PS d;
    // Worked example (PLAN.md Step 1 F): one caster box.
    {
        g::ShadowRacerInput50PS r; r.present = true; r.referenceValid = true;
        r.reference[0] = 1000; r.reference[1] = 50; r.reference[2] = 2000;
        r.lodCount[0] = 1; r.box[0] = box(995, 48, 1990, 1005, 52, 2010);
        g::ShadowFrameInput50PS in; in.racerCount = 1; in.racers = &r;
        in.cameraEye[0] = 1000; in.cameraEye[1] = 60; in.cameraEye[2] = 2050;
        expect(g::buildProjectedShadows50PS(in, frame, d), "example builds");
        const auto& s = frame.shadows[0];
        expect(frame.shadowCount == 1 && s.casterCount == 1, "example caster count");
        expect(near(s.radius, 11.357817f), "example radius");
        expect(near(s.inverseRadius, 0.08804509f, 1e-6f), "example 1/r");
        expect(near(s.fovDegrees, 1.6267781f), "example fov");
        expect(near(s.eye1[0], 1461.880f, 1e-2f) && near(s.eye1[1], 511.880f, 1e-2f) && near(s.eye1[2], 2461.880f, 1e-2f), "example eye1");
        expect(near(s.side[0], -0.7071068f) && near(s.side[1], 0.f) && near(s.side[2], 0.7071068f), "side axis");
        expect(near(s.up[0], -0.4082483f) && near(s.up[1], 0.8164966f) && near(s.up[2], -0.4082483f), "up axis");
        const float c[3] = {1000, 50, 2000}, hi[3] = {1005, 52, 2010}, drop[3] = {993, 40, 1998},
            lo[3] = {995, 48, 1990}, ground[3] = {1005, 40, 2000};
        float u, v;
        st(s, c, u, v); expect(near(u, 0.5f) && near(v, 0.5f), "centre maps to (0.5,0.5)");
        // GL t = 1 - original t (rows bottom-up).
        st(s, hi, u, v); expect(near(u, 0.65564f) && near(v, 1.f - 0.69770f), "box max corner");
        st(s, drop, u, v); expect(near(u, 0.65564f) && near(v, 1.f - 0.69770f), "its shadow on y=40");
        st(s, lo, u, v); expect(near(u, 0.34435f) && near(v, 1.f - 0.30231f), "box min corner");
        st(s, ground, u, v); expect(near(u, 0.34435f) && near(v, 1.f - 0.94931f), "ground point");
        // Caster clip x = 2u - 1 for the same points.
        const float x = s.casterClip[0] * hi[0] + s.casterClip[4] * hi[1] + s.casterClip[8] * hi[2] + s.casterClip[12];
        expect(near(x, 2.f * 0.65564f - 1.f), "caster clip x matches texture s");
    }
    // 1000-unit cull at 999 / 1001; split-screen has no shadows.
    {
        g::ShadowRacerInput50PS racers[2] = {pod(999, 0, 0), pod(1001, 0, 0)};
        g::ShadowFrameInput50PS in; in.racerCount = 2; in.racers = racers;
        expect(g::buildProjectedShadows50PS(in, frame, d), "cull builds");
        expect(!frame.shadows[0].culled && frame.shadows[0].casterCount == 3, "999 units casts");
        expect(frame.shadows[1].culled && frame.shadows[1].casterCount == 0, "1001 units culled");
        in.raceViews = 2;
        expect(g::buildProjectedShadows50PS(in, frame, d) && frame.shadowCount == 0, "split-screen: 0 shadows");
        in.raceViews = 1; in.racerCount = 9;
        expect(!g::buildProjectedShadows50PS(in, frame, d) && frame.shadowCount == 0, "9 racers refused");
    }
    // Race running (viewMode58): close camera view casts engines only
    // (1B8, 1BC, plus 1C0/1C4 when present); AI racers (beyond manager +34)
    // cast nothing; LOD clamp.
    {
        auto r = pod(0, 0, 0);
        r.engineOnlyView = true;
        r.lodCount[3] = 1; r.box[3] = box(-1, -1, -1, 1, 1, 1);  // 1C0 with one LOD
        r.lodCount[5] = 1; r.box[5] = box(-1, -1, -1, 1, 1, 1);  // cable shadow: not in the close view
        g::ShadowFrameInput50PS in; in.racerCount = 1; in.racers = &r;
        in.viewMode58 = true; in.viewRacerCount = 1;
        expect(g::buildProjectedShadows50PS(in, frame, d), "close view builds");
        const auto& s = frame.shadows[0];
        expect(s.casterCount == 3, "close view casts three");
        expect(s.casters[0].slot == 1 && s.casters[1].slot == 2 && s.casters[2].slot == 3, "close view slots 1B8,1BC,1C0");
        expect(s.casters[0].lod == 2 && s.casters[2].lod == 0, "LOD 2, clamped to 0 for one-LOD model");
        in.viewMode58 = false; // engine-only rule is read only while the race runs
        expect(g::buildProjectedShadows50PS(in, frame, d) && frame.shadows[0].casterCount == 5, "rule ignored outside viewMode58");
        r.engineOnlyView = false; in.viewMode58 = true;
        expect(g::buildProjectedShadows50PS(in, frame, d), "intact builds");
        expect(frame.shadows[0].casterCount == 5 && frame.shadows[0].casters[4].slot == 5 &&
            frame.shadows[0].casters[4].lod == 0, "intact: chariot, engines, 1C0, cable L01 at LOD 0");
    }
    // Running race: only manager +34 humans are processed.
    {
        g::ShadowRacerInput50PS racers[2] = {pod(0, 0, 0), pod(5, 0, 0)};
        g::ShadowFrameInput50PS in; in.racerCount = 2; in.racers = racers;
        in.viewMode58 = true; in.viewRacerCount = 1;
        expect(g::buildProjectedShadows50PS(in, frame, d), "running race builds");
        expect(frame.shadowCount == 2 && frame.shadows[0].casterCount == 3 && frame.shadows[1].casterCount == 0,
            "AI racer casts nothing while the race runs");
    }
    // Receivers: flag, camera limit, 200 cap, facing rule, portal walk.
    {
        auto r = pod(0, 0, 0);
        g::ShadowFrameInput50PS in; in.racerCount = 1; in.racers = &r;
        expect(g::buildProjectedShadows50PS(in, frame, d), "receiver frame builds");
        static g::ShadowReceiverStatic50PS statics[300];
        for (unsigned i = 0; i < 300; ++i) {
            auto& s = statics[i]; s.instance = i; s.zone = 1; s.receives = i != 7;
            s.centre[0] = 0; s.centre[1] = -5; s.centre[2] = 0;
            s.halfSize[0] = s.halfSize[1] = s.halfSize[2] = 2; s.radius = 3.5f;
        }
        statics[3].centre[0] = 900; // beyond the camera limit
        // Identity camera at the origin looking down -z (GL), w = -z.
        const float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, -0.2f, 0, 0, -1, 0};
        g::ShadowReceiverView50PS view; view.cameraZone = 1; view.worldViewProjection = m;
        static g::ShadowReceiverList50PS list;
        g::ShadowReceiverWorld50PS world; world.statics = statics; world.staticCount = 300; world.zoneCount = 2;
        expect(g::gatherShadowReceivers50PS(frame.shadows[0], world, view, list), "gather runs");
        expect(list.count == 200 && list.capped, "200 cap");
        expect(list.flagRejected == 1 && list.cameraRejected == 1, "flag and camera rejections");
        bool sawSeven = false;
        for (unsigned i = 0; i < list.count; ++i) sawSeven |= list.instance[i] == 7;
        expect(!sawSeven, "AllowShadowReceiving = 0 never listed");
        view.cameraZone = 2;
        expect(g::gatherShadowReceivers50PS(frame.shadows[0], world, view, list) && list.count == 0, "other zone, no portal: none");
        // A portal between zone 2 and 1 straddling the pod, seen by the shadow camera.
        static g::ShadowPortal50PS portal;
        portal.zone[0] = 2; portal.zone[1] = 1;
        const float q[4][3] = {{-20, -20, 0}, {20, -20, 0}, {20, 20, 0}, {-20, 20, 0}};
        for (int v = 0; v < 4; ++v) for (int a = 0; a < 3; ++a) portal.corner[v][a] = q[v][a];
        world.portals = &portal; world.portalCount = 1;
        expect(g::gatherShadowReceivers50PS(frame.shadows[0], world, view, list) && list.count == 200 &&
            list.portalsVisible == 1 && list.zonesVisited == 2, "zone 1 statics reached through the portal");
        // Facing rule: box straddling the eye crosses the near plane.
        expect(g::shadowReceiverFacingRule50PS(statics[0], m), "near-plane crossing takes the facing rule");
        g::ShadowReceiverStatic50PS ahead = statics[0];
        ahead.centre[0] = 0; ahead.centre[1] = 0; ahead.centre[2] = -50; ahead.halfSize[0] = ahead.halfSize[1] = ahead.halfSize[2] = 1;
        expect(!g::shadowReceiverFacingRule50PS(ahead, m), "box inside the view: fast path");
    }
    std::printf("original_projected_shadow50ps_check failures=%d\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
