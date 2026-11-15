#include "original_projected_shadow50ps.h"
#include "vehicle_draw50cm.h"
#include <cmath>
#include <initializer_list>
#include <algorithm>
#include <limits>

namespace rr::game {
namespace {
bool finite3(const float* v) noexcept {
    return std::isfinite(v[0]) && std::isfinite(v[1]) && std::isfinite(v[2]);
}
float dot3(const float* a, const float* b) noexcept { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
void cross3(const float* a, const float* b, float* out) noexcept {
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}
bool normalize3(float* v) noexcept {
    const float length = std::sqrt(dot3(v, v));
    if (!(length > 0.f) || !std::isfinite(length)) return false;
    for (int i = 0; i < 3; ++i) v[i] /= length;
    return true;
}
bool reject(ProjectedShadowDiagnostic50PS& d, ProjectedShadowError50PS error, const char* why) noexcept {
    d.error = error; d.operation = why; return false;
}
// 231900 mode 3: d = normalise(-0.3,-0.3,-0.3) (13E3F0), t = normalise(Y x d)
// with Y = 4349A0 = (0,1,0), up = d x t (13E440).
bool lightBasis(float (&d)[3], float (&t)[3], float (&up)[3]) noexcept {
    d[0] = d[1] = d[2] = -0.3f;
    if (!normalize3(d)) return false;
    const float y[3] = {0.f, 1.f, 0.f};
    cross3(y, d, t);
    if (!normalize3(t)) return false;
    cross3(d, t, up);
    return finite3(up);
}
// 2316D0: entry lod = min(lod, model->1B4->10 - 1).
bool addCaster(ProjectedShadow50PS& shadow, const ShadowRacerInput50PS& racer, std::uint32_t slot,
    std::uint32_t lod, std::uint32_t& missing) noexcept {
    if (slot >= shadowCasterSlotCount50PS) return false;
    if (!racer.box[slot].valid || !racer.lodCount[slot]) { ++missing; return true; }
    if (shadow.casterCount >= shadowCasterSlotCount50PS) return false;
    const std::uint32_t last = racer.lodCount[slot] - 1u;
    auto& entry = shadow.casters[shadow.casterCount++];
    entry.slot = static_cast<std::uint8_t>(slot);
    entry.lod = static_cast<std::uint8_t>(lod < last ? lod : last);
    return true;
}
} // namespace

bool shadowBoxFromLocal50PS(const float (&w)[16], const float (&c)[3], const float (&h)[3],
    ShadowBox50PS& box) noexcept {
    box = {};
    for (float v : w) if (!std::isfinite(v)) return false;
    if (!finite3(c) || !finite3(h)) return false;
    // Row vectors, W[i][j] = w[i * 4 + j]. cw = c * W; e_j = sum_i |h_i * W[i][j]|.
    for (int j = 0; j < 3; ++j) {
        const float centre = c[0] * w[j] + c[1] * w[4 + j] + c[2] * w[8 + j] + w[12 + j];
        const float extent = std::fabs(h[0] * w[j]) + std::fabs(h[1] * w[4 + j]) + std::fabs(h[2] * w[8 + j]);
        box.minimum[j] = centre - extent;
        box.maximum[j] = centre + extent;
    }
    box.valid = finite3(box.minimum) && finite3(box.maximum);
    return box.valid;
}

bool shadowBoxFromSubmission50PS(const VehicleDrawSubmission50CM& part, ShadowBox50PS& box,
    bool& sourceBounds) noexcept {
    sourceBounds = false;
    box = {};
    if (part.sourceSpecularGeometry50LA) {
        float w[16], c[3], h[3];
        for (int i = 0; i < 16; ++i) w[i] = part.sourceRootWorld50LA.v[i];
        for (int i = 0; i < 3; ++i) {
            c[i] = part.sourceAggregateBounds50LA.center160[i];
            h[i] = part.sourceAggregateBounds50LA.halfExtent170[i];
        }
        if (shadowBoxFromLocal50PS(w, c, h, box)) { sourceBounds = true; return true; }
    }
    // Stand-in: the part's world display box (conservative for skinned parts).
    if (!part.vertexCount && !part.skin) return false;
    for (int i = 0; i < 3; ++i) {
        box.minimum[i] = part.bounds.minimum[i];
        box.maximum[i] = part.bounds.maximum[i];
        if (!(box.minimum[i] <= box.maximum[i])) { box = {}; return false; }
    }
    box.valid = finite3(box.minimum) && finite3(box.maximum);
    return box.valid;
}

bool buildProjectedShadows50PS(const ShadowFrameInput50PS& input,
    OriginalProjectedShadowFrame50PS& output, ProjectedShadowDiagnostic50PS& d) noexcept {
    d = {};
    output.frame = input.frame;
    output.cameraZone = input.cameraZone;
    for (int a = 0; a < 3; ++a) output.cameraEye[a] = input.cameraEye[a];
    output.shadowCount = output.racersConsidered = 0;
    output.culled = output.viewSkipped = output.engineOnly = output.active = 0;
    output.degenerate = output.missingSlots = 0;
    for (auto& shadow : output.shadows) {
        shadow.culled = true; shadow.racer = 0; shadow.casterCount = 0; shadow.cableTriangles = 0;
        shadow.radius = shadow.inverseRadius = 0.f;
    }
    if (input.racerCount > maxProjectedShadows50PS)
        return reject(d, ProjectedShadowError50PS::capacity, "racer count exceeds the shadow targets");
    if (input.racerCount && !input.racers)
        return reject(d, ProjectedShadowError50PS::input, "racer inputs missing");
    if (!finite3(input.cameraEye))
        return reject(d, ProjectedShadowError50PS::input, "camera position not finite");
    // 274CD0 via 1A75F8: count = (Race+1B0 & 3) == 1 ? 17DB50 : 0.
    const std::uint32_t count = (input.raceViews & 3u) == 1u ? input.racerCount : 0u;
    output.shadowCount = count;
    if (!count) return true;
    float d3[3], t3[3], up3[3];
    if (!lightBasis(d3, t3, up3)) return reject(d, ProjectedShadowError50PS::arithmetic, "light basis");
    // 1B32E0: fp = s0 ? list+34 : list+30.
    const std::uint32_t considered = input.viewMode58 ? input.viewRacerCount : input.racerCount;
    if (considered > count)
        return reject(d, ProjectedShadowError50PS::capacity, "1B32E0 racer count exceeds created shadows");
    output.racersConsidered = considered;
    for (std::uint32_t i = 0; i < count; ++i) {
        auto& shadow = output.shadows[i];
        shadow.racer = i;
        shadow.culled = false;
        for (int a = 0; a < 3; ++a) { shadow.light[a] = d3[a]; shadow.side[a] = t3[a]; shadow.up[a] = up3[a]; }
    }
    for (std::uint32_t i = 0; i < considered; ++i) {
        d.racer = i;
        const auto& racer = input.racers[i];
        auto& shadow = output.shadows[i];
        if (!racer.present) continue;
        // 1000-unit cull of the racer's reference point against the camera.
        if (!racer.referenceValid || !finite3(racer.reference)) {
            shadow.culled = true; ++output.culled; continue;
        }
        float squared = 0.f;
        for (int a = 0; a < 3; ++a) {
            const float delta = racer.reference[a] - input.cameraEye[a];
            squared += delta * delta;
        }
        if (squared > projectedShadowCullDistanceSquared50PS) { shadow.culled = true; ++output.culled; continue; }
        shadow.culled = false;
        if (input.viewMode58 && racer.viewRule58) { ++output.viewSkipped; continue; }
        using S = ShadowCasterSlot50PS;
        const auto slot = [](S s) { return static_cast<std::uint32_t>(s); };
        bool ok = true;
        // 1B35A8: the view rule is evaluated only while viewMode58 (s0).
        if (input.viewMode58 && racer.engineOnlyView) {
            ++output.engineOnly;
            ok = addCaster(shadow, racer, slot(S::right1B8), projectedShadowBodyLod50PS, output.missingSlots) &&
                addCaster(shadow, racer, slot(S::left1BC), projectedShadowBodyLod50PS, output.missingSlots);
            // 1C0 / 1C4 only when the racer has them (non-null).
            for (S s : {S::extra1C0, S::extra1C4})
                if (ok && racer.lodCount[slot(s)]) ok = addCaster(shadow, racer, slot(s), projectedShadowBodyLod50PS, output.missingSlots);
        } else {
            for (S s : {S::chariot1B4, S::right1B8, S::left1BC})
                if (ok) ok = addCaster(shadow, racer, slot(s), projectedShadowBodyLod50PS, output.missingSlots);
            for (S s : {S::extra1C0, S::extra1C4})
                if (ok && racer.lodCount[slot(s)]) ok = addCaster(shadow, racer, slot(s), projectedShadowBodyLod50PS, output.missingSlots);
            // 1D0..1DC only when the pod load created them (214260).
            for (S s : {S::cableL01_1D0, S::cableR01_1D4, S::cableL02_1D8, S::cableR02_1DC})
                if (ok && racer.lodCount[slot(s)]) ok = addCaster(shadow, racer, slot(s), projectedShadowCableLod50PS, output.missingSlots);
        }
        if (!ok) return reject(d, ProjectedShadowError50PS::capacity, "caster list exceeds its slots");
        if (!shadow.casterCount) continue;
        // 231DF0 B.1/B.2: world box of all casters (FLT_MAX initial values).
        float minimum[3], maximum[3];
        for (int a = 0; a < 3; ++a) {
            minimum[a] = std::numeric_limits<float>::max();
            maximum[a] = -std::numeric_limits<float>::max();
        }
        for (std::uint32_t c = 0; c < shadow.casterCount; ++c) {
            const auto& box = racer.box[shadow.casters[c].slot];
            for (int a = 0; a < 3; ++a) {
                if (box.minimum[a] < minimum[a]) minimum[a] = box.minimum[a];
                if (box.maximum[a] > maximum[a]) maximum[a] = box.maximum[a];
            }
        }
        float centre[3], half[3];
        for (int a = 0; a < 3; ++a) {
            centre[a] = 0.5f * (minimum[a] + maximum[a]);
            half[a] = 0.5f * (maximum[a] - minimum[a]);
        }
        const float r = std::sqrt(dot3(half, half));
        if (!(r > 0.f) || !std::isfinite(r) || !finite3(centre)) {
            shadow.casterCount = 0; ++output.degenerate; continue;
        }
        const float inverse = 1.f / r;
        for (int a = 0; a < 3; ++a) {
            shadow.boxMinimum[a] = minimum[a]; shadow.boxMaximum[a] = maximum[a];
            shadow.centre[a] = centre[a]; shadow.halfSize[a] = half[a];
            // B.3 mode 3: eye1 = C - 800 d (perspective, receiver frustum only).
            shadow.eye1[a] = centre[a] - projectedShadowLightDistance50PS * d3[a];
        }
        shadow.radius = r; shadow.inverseRadius = inverse;
        shadow.fovDegrees = 114.59156f * std::atan2(r, projectedShadowLightDistance50PS);
        {
            // 236DD0 with eye1: look-at 14DC80 (side = up x d = t) times the
            // 14DB90 perspective (n 1, f 99840, aspect 1). Receiver test only.
            const float k = 1.f / std::tan(shadow.fovDegrees * (3.14159265f / 360.f));
            const float n = shadowCameraNear50PS, f = shadowCameraFar50PS, q = f / (f - n);
            float* m = shadow.receiverClip;
            for (int i = 0; i < 16; ++i) m[i] = 0.f;
            for (int a = 0; a < 3; ++a) {
                m[a * 4 + 0] = k * t3[a]; m[a * 4 + 1] = k * up3[a];
                m[a * 4 + 2] = q * d3[a]; m[a * 4 + 3] = d3[a];
            }
            m[12] = -k * dot3(t3, shadow.eye1); m[13] = -k * dot3(up3, shadow.eye1);
            m[14] = -q * dot3(d3, shadow.eye1) - n * q; m[15] = -dot3(d3, shadow.eye1);
            for (float v : shadow.receiverClip)
                if (!std::isfinite(v)) return reject(d, ProjectedShadowError50PS::arithmetic, "receiver camera");
        }
        // B.3/E.3: orthographic caster transform, view x/y = dot(P - C, side/up).
        const float sideC = dot3(t3, centre), upC = dot3(up3, centre);
        for (auto& v : shadow.casterClip) v = 0.f;
        for (auto& v : shadow.texture) v = 0.f;
        for (int a = 0; a < 3; ++a) {
            shadow.casterClip[a * 4 + 0] = t3[a] * inverse;
            shadow.casterClip[a * 4 + 1] = up3[a] * inverse;
            shadow.texture[a * 4 + 0] = 0.5f * t3[a] * inverse;
            shadow.texture[a * 4 + 1] = 0.5f * up3[a] * inverse;
        }
        shadow.casterClip[12] = -sideC * inverse;
        shadow.casterClip[13] = -upC * inverse;
        shadow.casterClip[15] = 1.f;
        // B.4: 0.5 scale/bias. GL rows run bottom-up, so t uses +0.5 * y.
        shadow.texture[12] = 0.5f - 0.5f * sideC * inverse;
        shadow.texture[13] = 0.5f - 0.5f * upC * inverse;
        shadow.texture[15] = 1.f;
        for (float v : shadow.casterClip) if (!std::isfinite(v)) return reject(d, ProjectedShadowError50PS::arithmetic, "caster transform");
        for (float v : shadow.texture) if (!std::isfinite(v)) return reject(d, ProjectedShadowError50PS::arithmetic, "texture transform");
        ++output.active;
    }
    d.racer = 0;
    return true;
}

bool shadowReceiverFacingRule50PS(const ShadowReceiverStatic50PS& receiver,
    const float* m) noexcept {
    if (!m) return false;
    // 2B0580: 8 corners through world * view * projection; outcodes
    // +x 1, -x 2, +y 4, -y 8, +z (far) 0x10, -z (near) 0x20.
    unsigned all = 0x3f, any = 0, guardAny = 0;
    for (unsigned corner = 0; corner < 8; ++corner) {
        float p[3];
        for (int a = 0; a < 3; ++a)
            p[a] = receiver.centre[a] + ((corner >> a) & 1u ? receiver.halfSize[a] : -receiver.halfSize[a]);
        float clip[4];
        for (int j = 0; j < 4; ++j) clip[j] = p[0] * m[j * 4] + p[1] * m[j * 4 + 1] + p[2] * m[j * 4 + 2] + m[j * 4 + 3];
        const float w = clip[3];
        unsigned code = 0;
        if (clip[0] > w) code |= 1u;
        if (clip[0] < -w) code |= 2u;
        if (clip[1] > w) code |= 4u;
        if (clip[1] < -w) code |= 8u;
        if (clip[2] > w) code |= 0x10u;
        if (clip[2] < -w) code |= 0x20u;
        all &= code; any |= code;
        // Guard-band matrix camera+2D0: x/y planes widened, near/far unchanged.
        unsigned guard = 0;
        if (clip[0] > projectedShadowGuardX50PS * w) guard |= 1u;
        if (clip[0] < -projectedShadowGuardX50PS * w) guard |= 2u;
        if (clip[1] > projectedShadowGuardY50PS * w) guard |= 4u;
        if (clip[1] < -projectedShadowGuardY50PS * w) guard |= 8u;
        if (clip[2] < -w) guard |= 0x20u;
        guardAny |= guard;
    }
    if (all) return true;          // culled: bit 2 stays 1
    if (!any) return false;        // fully inside: bit 3, fast path
    return guardAny != 0;          // crosses near or guard band (far-only does not)
}

bool shadowBoxVisible50PS(const float (&centre)[3], const float (&half)[3], const float (&m)[16]) noexcept {
    unsigned all = 0x3f;
    for (unsigned corner = 0; corner < 8; ++corner) {
        float p[3];
        for (int a = 0; a < 3; ++a) p[a] = centre[a] + ((corner >> a) & 1u ? half[a] : -half[a]);
        float c[4];
        for (int j = 0; j < 4; ++j) c[j] = m[j] * p[0] + m[4 + j] * p[1] + m[8 + j] * p[2] + m[12 + j];
        const float w = std::fabs(c[3]);
        unsigned code = 0;
        if (c[0] > w) code |= 1u;
        if (c[0] < -w) code |= 2u;
        if (c[1] > w) code |= 4u;
        if (c[1] < -w) code |= 8u;
        if (c[2] > w) code |= 0x10u;
        if (c[2] < -w) code |= 0x20u;
        all &= code;
    }
    return all == 0;
}

namespace {
struct Rect { float x0 = -1.f, y0 = -1.f, x1 = 1.f, y1 = 1.f; };
// 230880: a portal seen by the shadow camera, as a screen rectangle.
bool portalRect(const ShadowPortal50PS& portal, const ProjectedShadow50PS& shadow, Rect& out) noexcept {
    float e1[3], e2[3], n[3];
    for (int a = 0; a < 3; ++a) {
        e1[a] = portal.corner[1][a] - portal.corner[0][a];
        e2[a] = portal.corner[2][a] - portal.corner[0][a];
    }
    cross3(e1, e2, n);
    if (!normalize3(n)) return false;
    // 2308C4..230948: camera +180 holds eye2 at this point (231DF0 B.3).
    // Within the near distance (+1FC) of the portal plane the full rect passes.
    float eye2[3];
    for (int a = 0; a < 3; ++a) eye2[a] = shadow.centre[a] + projectedShadowLightDistance50PS * shadow.light[a];
    if (std::fabs(dot3(n, portal.corner[0]) - dot3(n, eye2)) < shadowCameraNear50PS) { out = Rect{}; return true; }
    // 14EAE0 through cam+100, clip codes (2E7D70): all outside one plane -> not seen.
    float clip[4][4];
    unsigned all = 0x3f;
    for (int v = 0; v < 4; ++v) {
        const float* p = portal.corner[v];
        const float* m = shadow.receiverClip;
        for (int j = 0; j < 4; ++j) clip[v][j] = m[j] * p[0] + m[4 + j] * p[1] + m[8 + j] * p[2] + m[12 + j];
        const float w = std::fabs(clip[v][3]);
        unsigned code = 0;
        if (clip[v][0] > w) code |= 1u;
        if (clip[v][0] < -w) code |= 2u;
        if (clip[v][1] > w) code |= 4u;
        if (clip[v][1] < -w) code |= 8u;
        if (clip[v][2] > w) code |= 0x10u;
        if (clip[v][2] < -w) code |= 0x20u;
        all &= code;
    }
    if (all) return false;
    // 2E7DB0 near clip, divide by w (2309FC..230A28), bound the polygon.
    float poly[8][4];
    int count = 0;
    for (int v = 0; v < 4; ++v) {
        const float* a = clip[v];
        const float* b = clip[(v + 1) & 3];
        const float da = a[2] + a[3], db = b[2] + b[3];
        if (da >= 0.f && count < 8) { for (int j = 0; j < 4; ++j) poly[count][j] = a[j]; ++count; }
        if ((da >= 0.f) != (db >= 0.f) && count < 8) {
            const float t = da / (da - db);
            for (int j = 0; j < 4; ++j) poly[count][j] = a[j] + t * (b[j] - a[j]);
            ++count;
        }
    }
    Rect r{2.f, 2.f, -2.f, -2.f};
    for (int v = 0; v < count; ++v) {
        if (!(poly[v][3] > 0.f)) continue;
        const float x = poly[v][0] / poly[v][3], y = poly[v][1] / poly[v][3];
        r.x0 = std::min(r.x0, x); r.y0 = std::min(r.y0, y);
        r.x1 = std::max(r.x1, x); r.y1 = std::max(r.y1, y);
    }
    r.x0 = std::max(r.x0, -1.f); r.y0 = std::max(r.y0, -1.f);
    r.x1 = std::min(r.x1, 1.f); r.y1 = std::min(r.y1, 1.f);
    if (!(r.x0 <= r.x1 && r.y0 <= r.y1)) return false;
    out = r;
    return true;
}
struct Gather {
    const ProjectedShadow50PS& shadow;
    const ShadowReceiverWorld50PS& world;
    const ShadowReceiverView50PS& view;
    ShadowReceiverList50PS& out;
    bool visited[maxShadowZones50PS + 1]{};
    // 2DE900 over one zone's static-model list (+34). z == 0: every static.
    void zone(std::uint32_t z) noexcept {
        for (std::uint32_t i = 0; i < world.staticCount && out.count < maxShadowReceivers50PS; ++i) {
            const auto& s = world.statics[i];
            if (z && s.zone != z) continue;                       // 2DE948 model +28 vs list +2C
            if (!s.receives) { ++out.flagRejected; continue; }   // 2DE96C +1B3 bit 4
            // 2DE9A0: model +1BC (|+160 + +130 - camera|, 26F070) - radius < +364.
            float delta[3];
            for (int a = 0; a < 3; ++a) delta[a] = s.centre[a] - view.cameraEye[a];
            if (!(std::sqrt(dot3(delta, delta)) - s.radius < shadowReceiverCameraLimit50PS)) { ++out.cameraRejected; continue; }
            // 2DEA50..2DEA7C: |(+160 + +130) - ps+C0|^2 - radius^2 < +368.
            float toCaster[3];
            for (int a = 0; a < 3; ++a) toCaster[a] = s.centre[a] - shadow.centre[a];
            bool accepted = dot3(toCaster, toCaster) - s.radius * s.radius < shadowReceiverNearLimit50PS;
            if (accepted) ++out.nearAccepted;
            else {
                // 2DEA94..2DEB38: +160/+170 box through world * cam+100.
                accepted = shadowBoxVisible50PS(s.centre, s.halfSize, shadow.receiverClip);
                if (accepted) ++out.frustumAccepted; else ++out.frustumRejected;
            }
            if (!accepted) continue;
            // One entry per instance (2806B0 skips a neighbour zone that the
            // camera zone already lists, so the original never lists twice).
            bool listed = false;
            for (std::uint32_t k = 0; k < out.count; ++k) if (out.instance[k] == s.instance) { listed = true; break; }
            if (listed) { ++out.duplicates; continue; }
            out.instance[out.count] = s.instance;
            out.facingRule[out.count] = shadowReceiverFacingRule50PS(s, view.worldViewProjection) ? 1u : 0u;
            if (++out.count >= maxShadowReceivers50PS) out.capped = 1;   // 2DEB74
        }
    }
    void portals(std::uint32_t z, const Rect& parent) noexcept {
        for (std::uint32_t p = 0; p < world.portalCount; ++p) {
            const auto& portal = world.portals[p];
            if (portal.zone[0] != z && portal.zone[1] != z) continue;
            ++out.portalsSeen;
            const std::uint32_t other = portal.zone[0] == z ? portal.zone[1] : portal.zone[0];
            if (!other || other > world.zoneCount || other > maxShadowZones50PS || visited[other]) continue;
            Rect r;
            if (!portalRect(portal, shadow, r)) continue;
            // 263260: intersect with the parent rectangle.
            r.x0 = std::max(r.x0, parent.x0); r.y0 = std::max(r.y0, parent.y0);
            r.x1 = std::min(r.x1, parent.x1); r.y1 = std::min(r.y1, parent.y1);
            if (!(r.x0 <= r.x1 && r.y0 <= r.y1)) continue;
            ++out.portalsVisible;
            // 2DE7C0: zone stamp +A6, its statics, then its own portals.
            visited[other] = true; ++out.zonesVisited;
            zone(other);
            portals(other, r);
        }
    }
};
} // namespace

bool gatherShadowReceivers50PS(const ProjectedShadow50PS& shadow, const ShadowReceiverWorld50PS& world,
    const ShadowReceiverView50PS& view, ShadowReceiverList50PS& out) noexcept {
    out = {};
    // 2DEBC0: nothing when culled or without casters.
    if (!shadow.hasCasters()) return true;
    if ((world.staticCount && !world.statics) || (world.portalCount && !world.portals) ||
        world.portalCount > maxShadowPortals50PS || world.zoneCount > maxShadowZones50PS) return false;
    if (!finite3(view.cameraEye) || !(shadow.radius > 0.f)) return false;
    Gather g{shadow, world, view, out};
    if (!view.cameraZone || view.cameraZone > world.zoneCount) {
        g.zone(0); // camera zone unknown: every static, no portal walk (zones_visited=0)
        return true;
    }
    g.visited[view.cameraZone] = true; ++out.zonesVisited;
    g.zone(view.cameraZone);
    // 383650 (initialised 1; only the debug slot changes it): portal walk.
    g.portals(view.cameraZone, Rect{});
    return true;
}
} // namespace rr::game
