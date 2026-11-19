#pragma once
// N64 track hazards for converted N64 Racer tracks: the N64 trigger volumes (plane gate,
// capsule), the N64 "carrier surface" gate, and three trigger actions the original Revenge
// does not have: ExplodePod (N64 trigger 104), BreakObject (102) and AnimationToggle (the
// door, 101). Read from the track's own TOB TriggerN / TriggerNActionM rows.
//
// PORT EXTENSION, NOT ORIGINAL REVENGE. Original Revenge treats an unknown ActionType as a
// fatal assertion (2DB760 -> 2DBAE4 "CreateActionClass undefined trigger type", then
// 2DC16C.. "Error in .toy: Undefined ActionType requested in %s." and a debug `break`;
// only with the SuppressAssert command-line switch (17D094 clears 36C100) is the action
// skipped). So these rows exist only in converted content; no stock TOB has N64Volume or any
// of these action types, and with no such row nothing here runs (inert on all 13 courses).
//
// N64 sources (racer-n64\notes\n64-hazards-spec.md): trigger test 0x800665A4, volume
// 0x800666A0..0x80066A44, handler dispatch 0x8007CD50; Explode 0x80070764 (PC 0x4741D0);
// crash-hit 0x8007C818 (PC 0x47CA90); door 0x8007CE30. Revenge space: +Y up.
//
// Host memory only. Nothing here writes game, guest or racer memory: it returns events that
// the caller hands to Revenge's own owners (destruction 225C50 dispatch, impact damage
// 221754, animation runtime fire/hide, sound). Every array index is bounds-checked; the
// limits struct below is the one place this layout is defined.
#include "../assets/import_world48f.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

namespace rr::game {

// ---- The one place that defines this owner's capacities. -------------------------------
struct N64HazardLimits50NH {
    static constexpr std::size_t rows = 32;            // N64 rows per track (Vengeance: 25)
    static constexpr std::size_t actionsPerRow = 1;    // one N64 action per row (its sound is a key of the action)
    static constexpr std::size_t surfaceObjects = 16;  // carrier names per row (Vengeance: up to 6, the door)
    static constexpr std::size_t ownersPerName = 64;   // static owners one carrier name may resolve to
    static constexpr std::size_t slots = 8;            // race slots
    static constexpr std::size_t nameBytes = 50;       // 27A524 instance-name rule (slt to first dot, 50)
    static constexpr float maxExtent = 5000.f;         // ft; refuse larger half sizes
};

enum class N64Volume50NH : std::uint8_t { plane, capsule };
enum class N64HazardKind50NH : std::uint8_t { explodePod, breakObject, animationToggle };
enum class N64ExplodeSide50NH : std::uint8_t { random, left, right };
enum class N64ExplodeGate50NH : std::uint8_t { all, local, localOrNearCamera };
enum class N64AfterDestroy50NH : std::uint8_t { retire, respawn };

struct N64ExplodeParams50NH {
    N64ExplodeSide50NH side = N64ExplodeSide50NH::random;   // N64: rand()/2^31 < 0.5 -> mode 1 (right)
    N64ExplodeGate50NH gate = N64ExplodeGate50NH::localOrNearCamera; // N64 LOCAL || FORCE_GROUND
    float nearCameraDistance = 1000.f;                      // ft; N64 LOD far edge (n64-loader-re 2.7) [I]
    N64AfterDestroy50NH after = N64AfterDestroy50NH::respawn;
    float respawnDelay = 3.f;                               // s after the destroy commit [open: N64 death camera]
};
struct N64BreakParams50NH {
    std::int32_t animationNumber = -1;   // target row, as Revenge's Animation actions address it
    std::string animationNameRef;        // used when animationNumber == -1 (last match wins, 2DA190)
    float thrustDeceleration = 2.5f;     // stdMath_Decelerator(2.5, dt) one-frame thrust factor
    float damageTotal = 0.15f;           // N64 300 / 2000
    float damageBoostScale = 4.f;        // N64 WAS_BOOSTING x4
    std::uint32_t damageHits = 3;        // N64 3 random parts
    std::int32_t effectAnimation = -1;   // debris row (Animation number), -1 = none
    std::string effectNameRef;
    float effectAhead = 12.f;            // human: effect follows the pod, 12 ft ahead (N64 vB x 12)
    float effectAiScale = 8.f;           // AI: fixed at the gate centre, scale 8
    float effectFade = 2.f;              // alpha (2 - t)/2, hidden at t >= 2
    std::string sound;                   // converted N64 sound 0x39
    float soundPitch = .25f, soundPitchRandom = .15f, soundMinDistance = 10.f, soundMaxDistance = 500.f;
};
struct N64ToggleParams50NH {
    std::int32_t animationNumber = -1;
    std::string animationNameRef;
    float guard = 3.f;                   // s between toggles (timer > 3.0)
    float rate = 20.f;                   // +-20 x the motion's own speed (13.33 s / 20 = 0.67 s)
};
struct N64HazardAction50NH {
    N64HazardKind50NH kind = N64HazardKind50NH::explodePod;
    std::uint32_t authoredAction = 0;    // 1-based TriggerNActionM
    N64ExplodeParams50NH explode;
    N64BreakParams50NH breakObject;
    N64ToggleParams50NH toggle;
};
struct N64HazardRow50NH {
    std::uint32_t trigger = 0;           // 1-based TriggerN (log identity)
    std::string name;
    N64Volume50NH volume = N64Volume50NH::plane;
    float centre[3]{}, direction[3]{};   // direction: horizontal unit (y = 0)
    float halfWidth = 0.f, halfHeight = 0.f;
    std::array<std::string, N64HazardLimits50NH::surfaceObjects> surface{};
    std::size_t surfaceCount = 0;        // 0 = tested under any surface (N64 global trigger rule)
    float minSpeed = 0.f;                // N64 flag 0x2: speed >= 150
    bool localOnly = false;              // N64 flag 0x20
    std::uint8_t lapMask = 0x7;          // N64 flags 0x4/0x8/0x10 -> laps 1/2/3 allowed (bit k = lap k+1)
    bool oneTimeOnly = false, startEnabled = true;
    std::array<N64HazardAction50NH, N64HazardLimits50NH::actionsPerRow> actions{};
    std::size_t actionCount = 0;
};

// ---- Reader -----------------------------------------------------------------------------
namespace n64_hazards50NH_detail {
struct Fail { const char* message; };
inline void need(bool ok, const char* m) { if (!ok) throw Fail{m}; }
inline const std::string* field(const assets::ParameterBlock48F& b, const char* key) {
    const std::string* found = nullptr;
    for (const auto& f : b.fields) if (f.name == key) { need(!found, "duplicate N64 trigger label"); found = &f.value; }
    return found;
}
inline const assets::ParameterBlock48F* block(const assets::ParameterDocument48F& p, const std::string& name) {
    const assets::ParameterBlock48F* found = nullptr;
    for (const auto& b : p.blocks) if (b.name == name) { need(!found, "duplicate N64 trigger block"); found = &b; }
    return found;
}
inline float number(const std::string& s) {
    char* end = nullptr; const float v = std::strtof(s.c_str(), &end);
    need(end && end != s.c_str() && *end == '\0' && std::isfinite(v), "N64 trigger decimal");
    return v;
}
inline float numberOr(const assets::ParameterBlock48F& b, const char* key, float def) {
    const auto* v = field(b, key); return v ? number(*v) : def;
}
inline bool boolean(const std::string& s) {
    need(s == "TRUE" || s == "FALSE" || s == "T" || s == "F", "N64 trigger boolean");
    return s == "TRUE" || s == "T";
}
inline void vector3(const std::string& s, float out[3]) {
    std::size_t at = 0;
    for (int i = 0; i < 3; ++i) {
        const auto end = s.find(',', at);
        need((end == std::string::npos) == (i == 2), "N64 trigger vector extent");
        out[i] = number(s.substr(at, end == std::string::npos ? std::string::npos : end - at));
        at = end + 1;
    }
}
inline std::string name(const std::string& s, std::size_t limit) {
    need(!s.empty() && s.size() <= limit, "N64 trigger name length");
    return s;
}
inline void target(const assets::ParameterBlock48F& a, std::int32_t& number_, std::string& ref) {
    if (const auto* v = field(a, "animationnumber")) {
        const long n = std::strtol(v->c_str(), nullptr, 0);
        need(n >= 1 && n <= 0x7fffffff, "N64 action AnimationNumber");
        number_ = static_cast<std::int32_t>(n);
    }
    if (const auto* v = field(a, "animationnameref")) { need(v->size() <= 0x27, "AnimationNameRef length"); ref = *v; }
}
} // namespace n64_hazards50NH_detail

// True when the TriggerN block is an N64 row (it carries N64Volume). The common trigger
// reader calls this first and hands such rows here instead of to the Revenge chains.
inline bool isN64HazardTrigger50NH(const assets::ParameterBlock48F& trigger) noexcept {
    for (const auto& f : trigger.fields) if (f.name == "n64volume") return true;
    return false;
}
inline bool isN64HazardActionType50NH(std::string_view type) noexcept {
    return type == "ExplodePod" || type == "BreakObject" || type == "AnimationToggle";
}

// Reads one N64 row. `number` is the 1-based TriggerN. Output written only on success.
inline bool readN64HazardRow50NH(const assets::ParameterDocument48F& p, const assets::ParameterBlock48F& b,
                                 std::uint32_t number, N64HazardRow50NH& out, std::string& error) noexcept {
    namespace d = n64_hazards50NH_detail;
    using L = N64HazardLimits50NH;
    try {
        N64HazardRow50NH r; r.trigger = number;
        if (const auto* v = d::field(b, "name")) r.name = *v;
        const auto* volume = d::field(b, "n64volume");
        d::need(volume && (*volume == "Plane" || *volume == "Capsule"), "N64Volume must be Plane or Capsule");
        r.volume = *volume == "Plane" ? N64Volume50NH::plane : N64Volume50NH::capsule;
        const auto* pos = d::field(b, "position"); d::need(pos, "N64 trigger Position");
        d::vector3(*pos, r.centre);
        const auto* dir = d::field(b, "n64direction"); d::need(dir, "N64Direction");
        d::vector3(*dir, r.direction);
        // N64 gate segments are horizontal (N64 z is height = Revenge y).
        d::need(r.direction[1] == 0.f, "N64Direction must be horizontal (y = 0)");
        const float len = std::sqrt(r.direction[0] * r.direction[0] + r.direction[2] * r.direction[2]);
        d::need(len > .5f && len < 2.f, "N64Direction length");
        r.direction[0] /= len; r.direction[2] /= len;
        r.halfWidth = d::numberOr(b, "n64halfwidth", 0.f);
        r.halfHeight = d::numberOr(b, "n64halfheight", 0.f);
        d::need(r.halfWidth > 0.f && r.halfWidth <= L::maxExtent && r.halfHeight > 0.f && r.halfHeight <= L::maxExtent,
                "N64HalfWidth / N64HalfHeight");
        if (const auto* v = d::field(b, "surfaceobject")) {
            std::size_t at = 0;
            while (at <= v->size()) {
                const auto end = v->find(',', at);
                d::need(r.surfaceCount < L::surfaceObjects, "too many SurfaceObject names");
                r.surface[r.surfaceCount++] = d::name(v->substr(at, end == std::string::npos ? std::string::npos : end - at), L::nameBytes);
                if (end == std::string::npos) break;
                at = end + 1;
            }
        }
        r.minSpeed = d::numberOr(b, "minspeed", 0.f);
        d::need(r.minSpeed >= 0.f && r.minSpeed < 10000.f, "MinSpeed");
        if (const auto* v = d::field(b, "localonly")) r.localOnly = d::boolean(*v);
        if (const auto* v = d::field(b, "laps")) {
            r.lapMask = 0;
            for (char c : *v) { if (c == ',') continue; d::need(c >= '1' && c <= '3', "Laps entries 1..3"); r.lapMask |= std::uint8_t(1u << (c - '1')); }
            d::need(r.lapMask != 0, "Laps empty");
        }
        const auto* once = d::field(b, "onetimeonly"); d::need(once, "OneTimeOnly");
        r.oneTimeOnly = d::boolean(*once);
        if (const auto* v = d::field(b, "startenabled")) r.startEnabled = d::boolean(*v);
        const auto* n = d::field(b, "numberofactions"); d::need(n, "NumberOfActions");
        const long count = std::strtol(n->c_str(), nullptr, 10);
        d::need(count >= 1 && static_cast<std::size_t>(count) <= L::actionsPerRow, "N64 row action count");
        std::size_t hazards = 0;
        for (long k = 1; k <= count; ++k) {
            const auto* a = d::block(p, b.name + "action" + std::to_string(k));
            d::need(a, "N64 TriggerNActionM missing");
            const auto* type = d::field(*a, "actiontype"); d::need(type, "N64 action ActionType");
            // A Revenge action on an N64 row would fire on the N64 test, not Revenge's: refused.
            d::need(isN64HazardActionType50NH(*type), "N64 row action must be ExplodePod, BreakObject or AnimationToggle");
            auto& x = r.actions[r.actionCount++]; x.authoredAction = static_cast<std::uint32_t>(k); ++hazards;
            if (*type == "ExplodePod") {
                x.kind = N64HazardKind50NH::explodePod;
                if (const auto* v = d::field(*a, "side"))
                    x.explode.side = *v == "Left" ? N64ExplodeSide50NH::left : *v == "Right" ? N64ExplodeSide50NH::right
                        : (d::need(*v == "Random", "ExplodePod Side"), N64ExplodeSide50NH::random);
                if (const auto* v = d::field(*a, "gate"))
                    x.explode.gate = *v == "All" ? N64ExplodeGate50NH::all : *v == "Local" ? N64ExplodeGate50NH::local
                        : (d::need(*v == "LocalOrNearCamera", "ExplodePod Gate"), N64ExplodeGate50NH::localOrNearCamera);
                x.explode.nearCameraDistance = d::numberOr(*a, "nearcameradistance", x.explode.nearCameraDistance);
                d::need(x.explode.nearCameraDistance > 0.f && x.explode.nearCameraDistance <= 100000.f, "NearCameraDistance");
                if (const auto* v = d::field(*a, "afterdestroy"))
                    x.explode.after = *v == "Retire" ? N64AfterDestroy50NH::retire
                        : (d::need(*v == "Respawn", "ExplodePod AfterDestroy"), N64AfterDestroy50NH::respawn);
                x.explode.respawnDelay = d::numberOr(*a, "respawndelay", x.explode.respawnDelay);
                d::need(x.explode.respawnDelay >= 0.f && x.explode.respawnDelay <= 30.f, "RespawnDelay");
            } else if (*type == "BreakObject") {
                x.kind = N64HazardKind50NH::breakObject;
                auto& q = x.breakObject;
                d::target(*a, q.animationNumber, q.animationNameRef);
                q.thrustDeceleration = d::numberOr(*a, "thrustdeceleration", q.thrustDeceleration);
                q.damageTotal = d::numberOr(*a, "damagetotal", q.damageTotal);
                q.damageBoostScale = d::numberOr(*a, "damageboostscale", q.damageBoostScale);
                q.damageHits = static_cast<std::uint32_t>(d::numberOr(*a, "damagehits", float(q.damageHits)));
                d::need(q.thrustDeceleration >= 0.f && q.thrustDeceleration < 1000.f && q.damageTotal >= 0.f && q.damageTotal <= 1.f &&
                        q.damageBoostScale >= 0.f && q.damageBoostScale <= 16.f && q.damageHits >= 1 && q.damageHits <= 8,
                        "BreakObject thrust / damage values");
                if (const auto* v = d::field(*a, "effectanimationnumber")) {
                    const long e = std::strtol(v->c_str(), nullptr, 0); d::need(e >= 1 && e <= 0x7fffffff, "EffectAnimationNumber");
                    q.effectAnimation = static_cast<std::int32_t>(e);
                }
                if (const auto* v = d::field(*a, "effectnameref")) { d::need(v->size() <= 0x27, "EffectNameRef length"); q.effectNameRef = *v; }
                q.effectAhead = d::numberOr(*a, "effectahead", q.effectAhead);
                q.effectAiScale = d::numberOr(*a, "effectaiscale", q.effectAiScale);
                q.effectFade = d::numberOr(*a, "effectfade", q.effectFade);
                d::need(q.effectAhead >= 0.f && q.effectAhead <= 1000.f && q.effectAiScale > 0.f && q.effectAiScale <= 64.f &&
                        q.effectFade > 0.f && q.effectFade <= 30.f, "BreakObject effect values");
                if (const auto* v = d::field(*a, "filename")) q.sound = d::name(*v, 0x103);
                q.soundPitch = d::numberOr(*a, "pitch", q.soundPitch);
                q.soundPitchRandom = d::numberOr(*a, "pitchrandom", q.soundPitchRandom);
                q.soundMinDistance = d::numberOr(*a, "mindistance", q.soundMinDistance);
                q.soundMaxDistance = d::numberOr(*a, "maxdistance", q.soundMaxDistance);
                d::need(q.soundPitch > 0.f && q.soundPitchRandom >= 0.f && q.soundMinDistance >= 0.f &&
                        q.soundMaxDistance >= q.soundMinDistance, "BreakObject sound values");
            } else {
                x.kind = N64HazardKind50NH::animationToggle;
                auto& q = x.toggle;
                d::target(*a, q.animationNumber, q.animationNameRef);
                d::need(q.animationNumber >= 1 || !q.animationNameRef.empty(), "AnimationToggle needs AnimationNumber or AnimationNameRef");
                q.guard = d::numberOr(*a, "toggleguard", q.guard);
                q.rate = d::numberOr(*a, "togglerate", q.rate);
                d::need(q.guard >= 0.f && q.guard <= 600.f && q.rate > 0.f && q.rate <= 1000.f, "AnimationToggle guard / rate");
                if (const auto* v = d::field(*a, "firsttrigger")) d::need(*v == "Freeze", "AnimationToggle FirstTrigger (Freeze only)");
            }
        }
        d::need(hazards == 1 && r.actionCount == 1, "an N64 row carries exactly one ExplodePod, BreakObject or AnimationToggle");
        out = std::move(r); error.clear(); return true;
    } catch (const n64_hazards50NH_detail::Fail& f) { error = f.message; }
    catch (...) { error = "N64 trigger row allocation failed"; }
    return false;
}

// ---- Volume tests (N64 0x800666A0..0x80066A44), Revenge axes (y up) ---------------------
struct N64VolumeHit50NH { bool hit = false; float t = 0.f; int crossingSign = 0; };
// Segment A..B = centre +- halfWidth * side, side = (dz, 0, -dx) (horizontal, across d).
inline float n64SegmentParam50NH(const N64HazardRow50NH& r, const float p[3], float& dx, float& dz) noexcept {
    const float sx = r.direction[2], sz = -r.direction[0];
    const float ax = r.centre[0] - r.halfWidth * sx, az = r.centre[2] - r.halfWidth * sz;
    const float bx = 2.f * r.halfWidth * sx, bz = 2.f * r.halfWidth * sz;
    const float t = ((p[0] - ax) * bx + (p[2] - az) * bz) / (bx * bx + bz * bz);
    const float tc = t < 0.f ? 0.f : t > 1.f ? 1.f : t;
    dx = p[0] - (ax + tc * bx); dz = p[2] - (az + tc * bz);
    return t;
}
inline N64VolumeHit50NH n64VolumeTest50NH(const N64HazardRow50NH& r, const float previous[3], const float current[3]) noexcept {
    N64VolumeHit50NH h; float dx = 0.f, dz = 0.f;
    const float t = n64SegmentParam50NH(r, current, dx, dz); h.t = t;
    if (!(std::fabs(current[1] - r.centre[1]) < r.halfHeight)) return h;
    if (r.volume == N64Volume50NH::capsule) { h.hit = dx * dx + dz * dz < r.halfWidth * r.halfWidth; return h; }
    if (!(t > 0.f && t < 1.f)) return h;                       // strictly inside the span (types 102/104)
    const float s0 = (previous[0] - r.centre[0]) * r.direction[0] + (previous[2] - r.centre[2]) * r.direction[2];
    const float s1 = (current[0] - r.centre[0]) * r.direction[0] + (current[2] - r.centre[2]) * r.direction[2];
    // Either direction (0x8006692C..0x800669C0). Exact zero handling there is [I].
    if ((s0 < 0.f && s1 >= 0.f) || (s0 >= 0.f && s1 < 0.f)) { h.hit = true; h.crossingSign = s1 >= 0.f ? 1 : -1; }
    return h;
}

// ---- Per-race owner -----------------------------------------------------------------------
// The caller resolves SurfaceObject names to static collision owner indices once per course
// (same owner numbering the surface rules and zero-g line table use) and passes, per step,
// the owner set the N64 rule reads: Normal / Recover = the racer's own collected ground owners;
// Flight = ZeroGravity50ZG::stepLineOwner(slot) (floor under the line at the racer's progress).
struct N64HazardStep50NH {
    std::size_t slot = 0;
    bool human = false;                 // vehicle +58 == 0
    bool alive = true;                  // not destroyed / limping / respawning (N64 0x7000 / EXPLODING gate)
    const char* notAliveReason = "dead"; // logged when !alive: dead | limping | respawning
    float previous[3]{}, current[3]{};  // root position before / after this completed step
    float speed = 0.f;                  // cached speed +34 (ft/s)
    std::uint32_t lap = 0;              // 0-based current lap
    const std::uint32_t* groundOwners = nullptr; std::size_t groundOwnerCount = 0;
    float nearestCameraDistance = 1e30f; // to the nearest viewport camera eye (ft)
};
struct N64HazardEvent50NH {
    std::size_t row = 0, action = 0, slot = 0;
    N64HazardKind50NH kind = N64HazardKind50NH::explodePod;
    int crossingSign = 0;
    float t = 0.f;                      // segment parameter of the hit (logs)
    bool explodeLeft = false;           // ExplodePod side after the draw
    const char* refused = nullptr;      // ExplodePod gate refusal: dead | limping | respawning | local | far
    bool nearCamera = false;            // ExplodePod passed by the near-camera half of the gate
};
// A gate that refused a volume hit (logged once per trigger, slot and lap by the caller).
struct N64HazardSkip50NH { std::size_t row = 0, slot = 0; const char* reason = ""; };
class N64TrackHazards50NH {
public:
    using L = N64HazardLimits50NH;
    // Cold, per course. rows: from readN64HazardRow50NH; resolvedOwners[i]: owner indices for
    // row i's SurfaceObject names (empty with surfaceCount 0 = any surface). seed: per-race seed
    // of this owner's own generator (never Revenge's shared RNG 308030, so no Revenge draw moves).
    bool load(std::vector<N64HazardRow50NH> rows, std::vector<std::vector<std::uint32_t>> resolvedOwners,
              std::uint32_t seed, std::string& error) noexcept {
        reset();
        if (rows.size() > L::rows || resolvedOwners.size() != rows.size()) { error = "N64 hazard rows exceed the owner"; return false; }
        for (std::size_t i = 0; i < rows.size(); ++i) {
            if (resolvedOwners[i].size() > L::ownersPerName * L::surfaceObjects) { error = "N64 carrier owner list too long"; return false; }
            if (rows[i].surfaceCount && resolvedOwners[i].empty()) { error = "N64 SurfaceObject names no static owner"; return false; }
        }
        rows_ = std::move(rows); owners_ = std::move(resolvedOwners); state_.assign(rows_.size(), {});
        for (std::size_t i = 0; i < rows_.size(); ++i) state_[i].enabled = rows_[i].startEnabled;
        random_ = seed ? seed : 1u; loaded_ = !rows_.empty(); return true;
    }
    void reset() noexcept { rows_.clear(); owners_.clear(); state_.clear(); loaded_ = false; }
    // Race (re)start: every row enabled as authored, fired cleared (N64 'Load'/'RSet').
    void raceReset(std::uint32_t seed) noexcept {
        for (std::size_t i = 0; i < rows_.size(); ++i) state_[i] = {}, state_[i].enabled = rows_[i].startEnabled;
        random_ = seed ? seed : 1u;
    }
    bool active() const noexcept { return loaded_; }
    std::size_t size() const noexcept { return rows_.size(); }
    const N64HazardRow50NH& row(std::size_t i) const { return rows_.at(i); }
    // After the racer's completed step. Writes at most `capacity` events; returns the count.
    // The N64 order is gates first, then the volume (0x800665A4); all must pass, so the
    // volume is tested first here only to report which gate refused a real hit (skips).
    std::size_t step(const N64HazardStep50NH& in, N64HazardEvent50NH* events, std::size_t capacity,
                     N64HazardSkip50NH* skips = nullptr, std::size_t skipCapacity = 0,
                     std::size_t* skipCount = nullptr) noexcept {
        if (skipCount) *skipCount = 0;
        if (!loaded_ || in.slot >= L::slots || !events) return 0;
        std::size_t used = 0, skipped = 0;
        auto skip = [&](std::size_t row, const char* reason) {
            if (skips && skipped < skipCapacity) skips[skipped++] = {row, in.slot, reason};
        };
        for (std::size_t i = 0; i < rows_.size(); ++i) {
            const auto& r = rows_[i]; auto& s = state_[i];
            if (!s.enabled) continue;                                              // N64 flag 0x1
            const auto h = n64VolumeTest50NH(r, in.previous, in.current);
            if (!h.hit) continue;
            if (r.oneTimeOnly && s.fired) { skip(i, "fired"); continue; }          // FIRED (FindOrCreate NULL)
            if (r.minSpeed > 0.f && in.speed < r.minSpeed) { skip(i, "speed"); continue; } // flag 0x2
            if (r.localOnly && !in.human) { skip(i, "local"); continue; }          // flag 0x20
            if (in.lap < 3 && !(r.lapMask & (1u << in.lap))) { skip(i, "lap"); continue; } // flags 0x4/0x8/0x10
            if (!carrierUnder(i, in)) { skip(i, "surface"); continue; }           // surface under the pod
            s.fired = true;
            for (std::size_t k = 0; k < r.actionCount; ++k) {
                if (used >= capacity) break;                                       // refused, never overrun
                N64HazardEvent50NH e; e.row = i; e.action = k; e.slot = in.slot; e.kind = r.actions[k].kind;
                e.crossingSign = h.crossingSign; e.t = h.t;
                if (e.kind == N64HazardKind50NH::explodePod) {
                    const auto& x = r.actions[k].explode;
                    // Explode early return (RESPAWN / RESPAWN_INVINC / DEAD / EXPLODING).
                    if (!in.alive) e.refused = in.notAliveReason ? in.notAliveReason : "dead";
                    else if (x.gate == N64ExplodeGate50NH::local && !in.human) e.refused = "local";
                    else if (x.gate == N64ExplodeGate50NH::localOrNearCamera && !in.human &&
                             !(in.nearestCameraDistance < x.nearCameraDistance)) e.refused = "far";
                    else {
                        e.nearCamera = !in.human && x.gate == N64ExplodeGate50NH::localOrNearCamera;
                        // N64: r < 0.5 -> Explode(pod, 1) = right engine; else mode 2 = left.
                        e.explodeLeft = x.side == N64ExplodeSide50NH::left ||
                                        (x.side == N64ExplodeSide50NH::random && unit() >= .5f);
                    }
                }
                events[used++] = e;
            }
        }
        if (skipCount) *skipCount = skipped;
        return used;
    }
    // N64 'Hitt' T201: total (x boost scale when boosting) split into equal hits; each hit picks
    // one of `parts` Revenge impact components at random. Returns the hit count written.
    std::size_t breakDamage(const N64BreakParams50NH& q, bool boosting, std::size_t parts,
                            std::uint32_t* partOut, float* amountOut, std::size_t capacity) noexcept {
        if (!parts || !partOut || !amountOut) return 0;
        const float total = q.damageTotal * (boosting ? q.damageBoostScale : 1.f);
        std::size_t n = 0;
        for (std::uint32_t k = 0; k < q.damageHits && n < capacity; ++k) {
            partOut[n] = static_cast<std::uint32_t>(unit() * float(parts)) % static_cast<std::uint32_t>(parts);
            amountOut[n] = total / float(q.damageHits); ++n;
        }
        return n;
    }
    float soundPitch(const N64BreakParams50NH& q) noexcept { return q.soundPitch + q.soundPitchRandom * unit(); }
    // stdMath_Decelerator (PC 0x480650): 1 - 33.333336 dt / (33.333336 dt + k).
    static float decelerator(float k, float dt) noexcept {
        const float a = dt * 33.333336f; return 1.f - a / (a + k);
    }
private:
    struct State { bool enabled = true, fired = false; };
    bool carrierUnder(std::size_t i, const N64HazardStep50NH& in) const noexcept {
        if (!rows_[i].surfaceCount) return true;
        if (!in.groundOwners) return false;
        for (std::size_t g = 0; g < in.groundOwnerCount; ++g)
            for (const auto o : owners_[i]) if (o == in.groundOwners[g]) return true;
        return false;
    }
    float unit() noexcept {                        // host-side LCG, [0,1)
        random_ = random_ * 1664525u + 1013904223u;
        return float(random_ >> 8) * (1.f / 16777216.f);
    }
    std::vector<N64HazardRow50NH> rows_;
    std::vector<std::vector<std::uint32_t>> owners_;
    std::vector<State> state_;
    std::uint32_t random_ = 1u;
    bool loaded_ = false;
};

// ---- Door (N64 0x8007CE30) as a state machine over one animation row --------------------
// Idle: the row plays as authored (SequenceActive1 T, LoopSequence1 T: the 13.33-s swing).
// First fire: time 0, rate 0, loop off (frozen at the first key = closed); guard timer starts.
// Later fire with timer > guard: timer 0; time <= 0 -> rate +R (open), else rate -R (close);
// playback stops at the end it runs to. Race (re)start: back to Idle.
struct N64ToggleCommand50NH { bool apply = false, setTime = false, loop = true; float time = 0.f, rate = 1.f; const char* event = "none"; };
class N64AnimationToggle50NH {
public:
    void raceReset() noexcept { armed_ = false; timer_ = 0.f; }
    void advance(float dt) noexcept { if (armed_) timer_ += dt; }      // N64 F0 timer +0x10 (not while paused)
    N64ToggleCommand50NH fire(const N64ToggleParams50NH& q, float animationTime) noexcept {
        N64ToggleCommand50NH c;
        if (!armed_) { armed_ = true; timer_ = 0.f; c.apply = c.setTime = true; c.time = 0.f; c.rate = 0.f; c.loop = false; c.event = "freeze"; return c; }
        if (!(timer_ > q.guard)) { c.event = "guard"; return c; }
        timer_ = 0.f; c.apply = true; c.loop = false;
        c.rate = animationTime <= 0.f ? q.rate : -q.rate; c.event = animationTime <= 0.f ? "open" : "close";
        return c;
    }
    bool armed() const noexcept { return armed_; }
    float sinceToggle() const noexcept { return timer_; }
private:
    bool armed_ = false; float timer_ = 0.f;
};

// ---- Animation target, resolved as Revenge's own Animation action (2DA190) -------------
// number >= 1 -> row number-1; number == -1 -> the LAST row whose ReferenceName equals ref.
// referenceNames: the course's animation rows in order. Returns false when nothing matches
// ("Error in .toy: Trigger References non existant Animation" in the original).
template<class Names>
inline bool resolveN64AnimationTarget50NH(std::int32_t number, const std::string& ref, const Names& referenceNames,
                                          std::size_t& index) noexcept {
    const std::size_t n = referenceNames.size();
    if (number >= 1) { if (static_cast<std::size_t>(number) > n) return false; index = static_cast<std::size_t>(number) - 1; return true; }
    bool found = false;
    for (std::size_t i = 0; i < n; ++i) if (referenceNames[i] == ref) { index = i; found = true; }
    return found;
}

// ---- Test-only switch (muted logs-only runs on stock courses) ---------------------------
// RR_TEST_N64_HAZARD="<kind>@<node>[/<arg>][,...]"  kind = explode | break | door
//   explode@N[/halfwidth]   plane gate across the map line at node N, ExplodePod Random,
//                           Gate All, AfterDestroy = RR_TEST_N64_AFTER (retire|respawn, default retire)
//   break@N[/anim]          plane gate, BreakObject on animation row `anim` (0 = no target: pod side only)
//   door@N/anim             capsule at node N, AnimationToggle on row `anim`
// Rows get no SurfaceObject (any surface), half height 80 ft, default half width 60 ft.
// Parsed here; the caller places them from the stock map spline and appends them within limits.
struct N64TestHazard50NH { N64HazardKind50NH kind = N64HazardKind50NH::explodePod; std::uint32_t node = 0; float arg = 0.f; bool hasArg = false; };
inline std::vector<N64TestHazard50NH> parseN64TestHazards50NH(const char* text, std::string& error) {
    std::vector<N64TestHazard50NH> out; error.clear();
    if (!text || !*text) return out;
    const std::string s(text); std::size_t at = 0;
    while (at < s.size()) {
        auto end = s.find(',', at); if (end == std::string::npos) end = s.size();
        const std::string item = s.substr(at, end - at); at = end + 1;
        const auto atSign = item.find('@');
        if (atSign == std::string::npos) { error = "RR_TEST_N64_HAZARD entry without @"; return {}; }
        N64TestHazard50NH h; const std::string kind = item.substr(0, atSign);
        if (kind == "explode") h.kind = N64HazardKind50NH::explodePod;
        else if (kind == "break") h.kind = N64HazardKind50NH::breakObject;
        else if (kind == "door") h.kind = N64HazardKind50NH::animationToggle;
        else { error = "RR_TEST_N64_HAZARD kind"; return {}; }
        const std::string rest = item.substr(atSign + 1); const auto slash = rest.find('/');
        char* e = nullptr; const long node = std::strtol(rest.substr(0, slash).c_str(), &e, 10);
        if (node < 0 || node > 4095 || (e && *e)) { error = "RR_TEST_N64_HAZARD node"; return {}; }
        h.node = static_cast<std::uint32_t>(node);
        if (slash != std::string::npos) { h.arg = std::strtof(rest.substr(slash + 1).c_str(), nullptr); h.hasArg = true; }
        if (h.kind == N64HazardKind50NH::animationToggle && !h.hasArg) { error = "door@N needs /anim"; return {}; }
        if (out.size() >= N64HazardLimits50NH::rows) { error = "RR_TEST_N64_HAZARD too many entries"; return {}; }
        out.push_back(h);
    }
    return out;
}

} // namespace rr::game
