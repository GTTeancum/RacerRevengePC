#pragma once
// N64 track hazards, race runtime: one owner per course load, fed after every racer's
// completed step, handing ExplodePod / BreakObject / AnimationToggle results to Revenge's
// own owners. PORT EXTENSION, NOT ORIGINAL REVENGE (canonical N64 behaviour by user
// decision). Keyed only on the track's own TriggerN rows with N64Volume (and the test
// switch below); every stock course has none, so nothing here runs and nothing is logged.
//
// Design: work\scratch\n64-hazards\NOTES.md section 5. Volumes, gates and the row reader:
// n64_track_hazards50nh.h. Floor under the line in flight: n64_floor_table50nh.h through
// the zero-g line table.
//
// Hand-offs (no Revenge or guest memory is written here):
//  - ExplodePod: a per-slot request the racer's next frame consumes at its 225C50 point
//    (225E7C slot90 227370 left / 225EC0 slot94 227A40 right); the human limp versus AI
//    wreck is the original's own branch there.
//  - BreakObject: a per-slot damage request the racer's next frame applies through the
//    original impact-damage rule 221754 (impact_damage_apply50nh.h); the target row is hidden
//    and its collision removed, and the debris row started, through the animation port.
//  - AnimationToggle: the door state machine on an animation row through the animation port.
//  - AnimationPlay (312 pillar / 314 methane): show (StartHidden FX) + fire at the authored
//    rate, once; an authored shake goes to Revenge's own CameraShake (camera_shake_relay50cs.h).
//  - Respawn (AfterDestroy = Respawn): after RespawnDelay s of destroyed state the race loop
//    restores the racer (228110 subset) and asks for the 2255B0 placement (228BBC path).
// Animation port: implemented by the common track-animation runtime through
// tests/native/n64_track_animation_port50nh.h (bound at world load). Until it
// is bound every animation-dependent action logs
// "n64_trigger target_unbound reason=animation_runtime" and does nothing else.
//
// Host memory only; every index bounds-checked against N64HazardLimits50NH.
#include "n64_track_hazards50nh.h"
#include "camera_shake_relay50cs.h"
#include "n64_pillar_camera_shake50nh.h"
#include "n64_explosion50nh.h"
#include "undertrack_reposition50et.h"
#include "../assets/import_decode48c.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {

// Log lines written inside a racer's frame (the trigger fire path runs in the player physics
// frame, which must not allocate): formatted on the stack, written with fputs. A line longer
// than the buffer is cut, never allocated.
inline void n64Log50NH(const char* format, ...) noexcept {
    char line[768];
    va_list args; va_start(args, format);
    const int n = std::vsnprintf(line, sizeof line, format, args);
    va_end(args);
    if (n > 0) std::fputs(line, stderr);
}

// ---- Animation port (the common track-animation runtime implements it; 145d) ------------
// Rows are the course's [Animations] rows in TOB order (row = AnimationNumber - 1).
class N64TrackAnimationPort50NH {
public:
    virtual ~N64TrackAnimationPort50NH() = default;
    virtual std::size_t rowCount() const noexcept = 0;
    virtual const std::string& referenceName(std::size_t row) const noexcept = 0; // ReferenceName
    virtual bool fire(std::size_t row) noexcept = 0;                      // 101170: start the row's sequence
    virtual bool hide(std::size_t row, bool removeCollision) noexcept = 0; // 1011A0 + collision unregistered
    virtual bool show(std::size_t row) noexcept = 0;                      // StartHidden row made visible
    virtual bool setRate(std::size_t row, float rate, bool loop) noexcept = 0; // signed playback rate; stop at the end without loop
    virtual bool setTime(std::size_t row, float seconds) noexcept = 0;
    virtual float time(std::size_t row) const noexcept = 0;               // current motion time (s)
    // Per-frame placement override (world 4x4, column-major like BodyMatrix48G) with a
    // uniform scale and an alpha (0..1); cleared by clearTransform.
    virtual bool setTransform(std::size_t row, const float world[16], float scale, float alpha) noexcept = 0;
    virtual bool clearTransform(std::size_t row) noexcept = 0;
};

struct N64ExplodeRequest50NH {
    bool pending = false, left = false, waitForDeath = false;
    std::size_t row = 0;
    const char* gate = "all";             // local | near_camera | all
    float dist = 0.f;                     // to the nearest viewport stand-in (ft)
    N64AfterDestroy50NH after = N64AfterDestroy50NH::retire;
    float respawnDelay = 3.f;
};
struct N64BreakDamageRequest50NH {
    static constexpr std::size_t capacity = 8;  // DamageHits <= 8 (reader bound)
    bool pending = false;
    std::size_t row = 0, count = 0;
    float thrustFactor = 1.f;
    std::array<unsigned, capacity> mask{};      // 221754 component flag: 1 = index 1, 0x10 = index 0
    std::array<float, capacity> amount{};
};

class N64HazardRuntime50NH {
public:
    using L = N64HazardLimits50NH;
    // Cold, every course load (clears the previous course). authored: the common trigger
    // reader's N64 rows (empty on stock). rules: SurfaceRules50ST (static owner names, the
    // 27A524 rule, lower case). archiveBytes/mapMember: the course RES and its map spline,
    // read only for the test switch rows.
    template<class Rules>
    void loadCourse(const std::vector<N64HazardRow50NH>& authored, const Rules& rules,
                    const std::vector<std::uint8_t>* archiveBytes, const char* mapMember, const char* site,
                    const std::vector<std::string>& animatedCarriers, std::uint32_t firstAnimatedIdentity) noexcept {
        reset();
        try {
            std::vector<N64HazardRow50NH> rows = authored;
            appendTestRows(rows, archiveBytes, mapMember);
            if (rows.empty()) return;                                   // stock: silent
            if (rows.size() > L::rows) { std::cerr << "n64_hazards refused site=" << site << " reason=rows_exceed_owner rows=" << rows.size() << '\n'; return; }
            ownerNames_.clear();
            if (rules.loaded()) for (std::size_t k = 0; k < rules.ownerCount(); ++k) ownerNames_.push_back(rules.ownerName(static_cast<std::uint32_t>(k)));
            staticOwnerCount_ = ownerNames_.size();
            firstAnimatedIdentity_ = firstAnimatedIdentity;
            for (const auto& name : animatedCarriers) ownerNames_.push_back(lower(name));
            std::vector<std::vector<std::uint32_t>> resolved(rows.size());
            for (std::size_t i = 0; i < rows.size(); ++i) {
                const auto& r = rows[i];
                for (std::size_t n = 0; n < r.surfaceCount; ++n) {
                    const auto name = lower(r.surface[n]); bool hit = false;
                    for (std::size_t k = 0; k < ownerNames_.size(); ++k)
                        if (ownerNames_[k] == name) {
                            if (resolved[i].size() >= L::ownersPerName * L::surfaceObjects) break;
                            resolved[i].push_back(static_cast<std::uint32_t>(k)); hit = true;
                        }
                    if (!hit) std::cerr << "n64_trigger surface_unresolved trigger=" << r.trigger << " name=" << name
                                        << " owners_named=" << ownerNames_.size() << '\n';
                }
            }
            std::string error;
            owners_ = resolved;
            if (!hazards_.load(rows, resolved, seed, error)) {
                std::cerr << "n64_hazards refused site=" << site << " reason=\"" << error << "\"\n"; reset(); return;
            }
            toggles_.assign(rows.size(), {});
            targets_.assign(rows.size(), {});
            effects_.assign(rows.size(), {});
            unboundLogged_.assign(rows.size(), false);
            skipLogged_.assign(rows.size() * L::slots * 4u, false);
            loaded_ = true;
            std::cerr << "n64_hazards load site=" << site << " rows=" << rows.size() << " authored=" << authored.size()
                      << " test=" << rows.size() - authored.size() << " owners_named=" << ownerNames_.size() << '\n';
            for (std::size_t i = 0; i < rows.size(); ++i) logLoad(i);
            resolveTargets();
        } catch (...) { reset(); std::cerr << "n64_hazards refused site=" << site << " reason=allocation\n"; }
    }
    void reset() noexcept {
        loaded_ = false; hazards_.reset(); owners_.clear(); ownerNames_.clear(); toggles_.clear(); targets_.clear();
        effects_.clear(); unboundLogged_.clear(); skipLogged_.clear(); clearRace();
    }
    // Race (re)start: rows enabled as authored, nothing fired (N64 'Load' / 'RSet').
    void raceReset() noexcept {
        if (!loaded_) return;
        hazards_.raceReset(seed);
        for (auto& t : toggles_) t.machine.raceReset(), t.guardLogged = false;
        for (std::size_t i = 0; i < effects_.size(); ++i) endEffect(i, false);
        std::fill(skipLogged_.begin(), skipLogged_.end(), false);
        clearRace();
        std::cerr << "n64_hazards race_reset rows=" << hazards_.size() << '\n';
    }
    bool active() const noexcept { return loaded_; }
    double firstShownAnimationTime() const noexcept { return firstShownAnimationTime_; }
    void setViewportEye(std::size_t viewport,float x,float y,float z,float lookX,float lookZ) noexcept {
        if(viewport>=cameraEyes_.size())return;
        cameraEyes_[viewport]={x,y,z};
        cameraForward_[viewport]={lookX,lookZ};
        cameraValid_[viewport]=std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z);
    }
    void clearViewportEye(std::size_t viewport) noexcept {
        if(viewport<cameraValid_.size())cameraValid_[viewport]=false;
    }
    std::uint32_t animatedCarrier(std::uint32_t identity) const noexcept {
        if (identity < firstAnimatedIdentity_) return UINT32_MAX;
        const auto index = staticOwnerCount_ + std::size_t(identity - firstAnimatedIdentity_);
        return index < ownerNames_.size() ? static_cast<std::uint32_t>(index) : UINT32_MAX;
    }

    // The common track-animation runtime binds itself here (or nullptr when it goes away).
    void bindImpactSound(std::weak_ptr<N64ImpactSound50NH> sound) noexcept { impactSound_=sound; }
    void bindAnimation(N64TrackAnimationPort50NH* port) noexcept { animation_ = port; if (loaded_) resolveTargets(); }

    // Race loop, once per tick while racing: race clock for logs, door guard timers, effects.
    void advance(double raceSeconds) noexcept {
        if (!loaded_) return;
        const double dt = haveClock_ ? raceSeconds - raceSeconds_ : 0.0;
        raceSeconds_ = raceSeconds; haveClock_ = true;
        if (!(dt > 0.0) || dt > 1.0) return;
        for(auto& phase:explosionPhase_)phase.advanceProtection(dt);
        n64PillarCameraShake50NH.advance(dt);
        for (auto& t : toggles_) t.machine.advance(static_cast<float>(dt));
        for (std::size_t i = 0; i < effects_.size(); ++i) {
            auto& e = effects_[i];
            if(e.quake!=QuakeStage::idle){
                e.age+=static_cast<float>(dt);
                const auto& target=targets_[i];
                if(e.quake==QuakeStage::armed&&e.age>1.5f){
                    const bool hidden=animation_&&animation_->hide(target.row,true);
                    const bool shown=animation_&&animation_->show(target.effect)&&animation_->fire(target.effect);
                    n64Log50NH("n64_quake collapse trigger=%u clock=%g structure_hidden=%d effect_playing=%d\n",
                               hazards_.row(i).trigger,double(e.age),int(hidden),int(shown));
                    if(shown&&firstShownAnimationTime_<0.)firstShownAnimationTime_=raceSeconds_;
                    e.age=0.f;e.quake=QuakeStage::collapsed;
                }else if(e.quake==QuakeStage::collapsed&&e.age>2.2f){
                    const bool hidden=animation_&&animation_->hide(target.effect,false);
                    n64PillarCameraShake50NH={};
                    n64Log50NH("n64_quake clear trigger=%u clock=%g effect_hidden=%d\n",hazards_.row(i).trigger,double(e.age),int(hidden));
                    e={};
                }
                continue;
            }
            if (!e.live) continue;
            e.age += static_cast<float>(dt);
            const auto& q = hazards_.row(i).actions[0].breakObject;
            if (e.age >= q.effectLifetime) { endEffect(i, true); continue; }
            if (!animation_ || !targets_[i].effectValid) continue;
            positionBreakEffect(i);
        }
    }

    // After a racer's COMPLETED step (frame physics, after the zero-g latch). carrier owners:
    // Normal / Recover = the step's collected static owners; Flight = the floor under the
    // line (every owner of the .nft surface, else the zero-g line owner).
    struct StepInput {
        N64HazardStep50NH step;
        bool flight = false;
        bool recovering = false;
        float frameSeconds = 0.f;
        bool boosting = false;            // Revenge boost word (+64) nonzero [I] for N64 WAS_BOOSTING
        float forward[3]{0.f, 0.f, 1.f};  // pod body forward axis (effect placement)
        bool havePosition = true;
    };
    void afterStep(StepInput in) noexcept {
        if (!loaded_ || in.step.slot >= L::slots || !in.havePosition) return;
        const auto slot = in.step.slot;
        if (in.step.human) { for (unsigned k = 0; k < 3; ++k) { humanPos_[slot][k] = in.step.current[k]; forward_[slot][k] = in.forward[k]; } humanValid_[slot] = true; }
        else for (unsigned k = 0; k < 3; ++k) forward_[slot][k] = in.forward[k];
        // Previous position: this owner's own copy of the last completed step's root.
        if (!prevValid_[slot]) { std::memcpy(prev_[slot], in.step.current, sizeof prev_[slot]); prevValid_[slot] = true; return; }
        std::memcpy(in.step.previous, prev_[slot], sizeof prev_[slot]);
        std::memcpy(prev_[slot], in.step.current, sizeof prev_[slot]);
        // PORT STAND-IN (flagged): a step longer than 100 ft is a placement (reset, respawn),
        // not motion; no crossing is tested across it.
        float jump = 0.f; for (unsigned k = 0; k < 3; ++k) jump += (in.step.current[k] - in.step.previous[k]) * (in.step.current[k] - in.step.previous[k]);
        if (jump > 100.f * 100.f) return;
        in.step.speed=N64TrackHazards50NH::signedStepSpeed(in.step.previous,in.step.current,
            in.forward,in.frameSeconds,in.flight||in.recovering);
        auto& phase=explosionPhase_[slot];
        phase.advanceRacer(in.frameSeconds,in.step.speed);
        float nearest=1e30f;
        for(std::size_t view=0;view<cameraEyes_.size();++view){
            if(!cameraValid_[view])continue;
            float distanceSquared=0.f;
            for(unsigned k=0;k<3;++k){
                const auto delta=in.step.current[k]-cameraEyes_[view][k];
                distanceSquared+=delta*delta;
            }
            nearest=std::min(nearest,std::sqrt(distanceSquared));
        }
        in.step.nearestCameraDistance = nearest;
        if (respawn_[slot].armed && in.step.alive) { in.step.alive = false; in.step.notAliveReason = "respawning"; }
        if(phase.exploding||phase.protectionSeconds>0.f){
            in.step.alive=false;
            in.step.notAliveReason=phase.exploding?"exploding":"respawn_invincible";
        }
        N64HazardEvent50NH events[L::rows]{}; N64HazardSkip50NH skips[L::rows]{}; std::size_t skipCount = 0;
        const auto n = hazards_.step(in.step, events, L::rows, skips, L::rows, &skipCount);
        const unsigned lapKey = in.step.lap < 3 ? in.step.lap : 3u;
        for (std::size_t k = 0; k < skipCount; ++k) {
            const std::size_t key = (skips[k].row * L::slots + slot) * 4u + lapKey;
            if (key >= skipLogged_.size() || skipLogged_[key]) continue;
            skipLogged_[key] = true;
            n64Log50NH("n64_trigger skip trigger=%u slot=%u lap=%u reason=%s\n", hazards_.row(skips[k].row).trigger,
                       unsigned(slot), unsigned(in.step.lap), skips[k].reason);
        }
        for (std::size_t k = 0; k < n; ++k) handle(events[k], in);
    }

    // ---- consumers ----------------------------------------------------------------------
    const N64Explosion50NH* exploding(std::size_t slot) const noexcept {
        return slot<L::slots&&explosionPhase_[slot].exploding?&explosionPhase_[slot]:nullptr;
    }
    void cancelExplode(std::size_t slot) noexcept {if(slot<L::slots)explosionPhase_[slot]={};}
    bool takeExplode(std::size_t slot, N64ExplodeRequest50NH& out) noexcept {
        if (slot >= L::slots || !explode_[slot].pending ||
            (explode_[slot].waitForDeath&&!explosionPhase_[slot].dead)) return false;
        out = explode_[slot]; explode_[slot] = {}; return true;
    }
    // The 225C50 point dispatched the request (handler chosen there).
    void explodeDispatched(std::size_t slot, const N64ExplodeRequest50NH& q) noexcept {
        if (slot >= L::slots) return;
        if (q.after == N64AfterDestroy50NH::respawn) { respawn_[slot] = {}; respawn_[slot].armed = true; respawn_[slot].delay = q.respawnDelay; respawn_[slot].row = q.row; }
    }
    bool takeBreakDamage(std::size_t slot, N64BreakDamageRequest50NH& out) noexcept {
        if (slot >= L::slots || !damage_[slot].pending) return false;
        out = damage_[slot]; damage_[slot] = {}; return true;
    }
    // Respawn service (race loop, each racing tick). destroyed = the racer's native destroy
    // record (state 3/4). Returns true once when the respawn is due.
    bool respawnDue(std::size_t slot, bool destroyed, double raceSeconds) noexcept {
        if (slot >= L::slots) return false;
        auto& r = respawn_[slot];
        if (!r.armed) return false;
        if (!destroyed) return false;                       // still limping (227370/227A40 countdown)
        if (!r.destroyedSeen) { r.destroyedSeen = true; r.destroyedAt = raceSeconds; return false; }
        return raceSeconds - r.destroyedAt >= static_cast<double>(r.delay);
    }
    // The loop restored the racer (228110 subset); the next physics step places it (2255B0).
    void respawnRestored(std::size_t slot, double raceSeconds) noexcept {
        if (slot >= L::slots) return;
        auto& r = respawn_[slot];
        n64Log50NH("n64_respawn slot=%u trigger=%u after_s=%g delay=%g restore=228110 placement=pending\n", unsigned(slot),
                   r.row < hazards_.size() ? hazards_.row(r.row).trigger : 0u, raceSeconds - r.destroyedAt, double(r.delay));
        explosionPhase_[slot].respawn();
        r = {}; placement_[slot] = true; prevValid_[slot] = false;
    }
    bool takePlacement(std::size_t slot) noexcept {
        if (slot >= L::slots || !placement_[slot]) return false;
        placement_[slot] = false; prevValid_[slot] = false; return true;
    }
    // True from the explode dispatch until the restore: the rank pass must not retire the
    // racer (PORT EXTENSION: Revenge has no respawn; its 1B06F4 retirement is permanent).
    bool respawnArmed(std::size_t slot) const noexcept { return slot < L::slots && respawn_[slot].armed; }
    bool anyRespawnArmed() const noexcept { for (const auto& r : respawn_) if (r.armed) return true; return false; }
    double raceSeconds() const noexcept { return raceSeconds_; }
    std::uint32_t triggerOf(std::size_t row) const noexcept { return loaded_ && row < hazards_.size() ? hazards_.row(row).trigger : 0u; }

    static constexpr std::uint32_t seed = 0x4e363448u; // 'N64H': this owner's own generator (never 308030)
private:
    struct Toggle { N64AnimationToggle50NH machine; bool guardLogged = false; };
    struct Target { bool valid = false, effectValid = false; std::size_t row = 0, effect = 0; };
    enum class QuakeStage { idle,armed,collapsed };
    struct Effect { QuakeStage quake=QuakeStage::idle; bool live = false, follow = false; std::size_t slot = 0; float age = 0.f; };
    struct Respawn { bool armed = false, destroyedSeen = false; float delay = 3.f; double destroyedAt = 0.0; std::size_t row = 0; };

    static std::string lower(std::string s) { for (auto& c : s) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a'); return s; }
    static const char* kindName(N64HazardKind50NH k) noexcept {
        return k == N64HazardKind50NH::explodePod ? "ExplodePod" : k == N64HazardKind50NH::breakObject ? "BreakObject" :
               k == N64HazardKind50NH::earthquake ? "Earthquake" : k == N64HazardKind50NH::animationPlay ? "AnimationPlay" : "AnimationToggle";
    }
    static const std::int32_t& targetNumber(const N64HazardAction50NH& a) noexcept {
        return a.kind == N64HazardKind50NH::earthquake ? a.quake.animationNumber :
               a.kind == N64HazardKind50NH::breakObject ? a.breakObject.animationNumber :
               a.kind == N64HazardKind50NH::animationPlay ? a.play.animationNumber : a.toggle.animationNumber;
    }
    static const std::string& targetRef(const N64HazardAction50NH& a) noexcept {
        return a.kind == N64HazardKind50NH::earthquake ? a.quake.animationNameRef :
               a.kind == N64HazardKind50NH::breakObject ? a.breakObject.animationNameRef :
               a.kind == N64HazardKind50NH::animationPlay ? a.play.animationNameRef : a.toggle.animationNameRef;
    }
    void clearRace() noexcept {
        for(auto& fx:effects_)fx={};
        n64PillarCameraShake50NH={};cameraValid_.fill(false);
        explosionPhase_.fill({});explode_.fill({}); damage_.fill({}); respawn_.fill({}); placement_.fill(false);
        prevValid_.fill(false); humanValid_.fill(false); haveClock_ = false; raceSeconds_ = 0.0; firstShownAnimationTime_ = -1.0;
    }
    std::string ownerName(std::uint32_t k) const { return k < ownerNames_.size() ? ownerNames_[k] : std::string("none"); }
    const char* ownerNameView(std::uint32_t k) const noexcept { return k < ownerNames_.size() ? ownerNames_[k].c_str() : "none"; }
    void logLoad(std::size_t i) const {
        const auto& r = hazards_.row(i); const auto& a = r.actions[0];
        std::cerr << "n64_trigger load trigger=" << r.trigger << " name=" << (r.name.empty() ? "-" : r.name)
                  << " volume=" << (r.volume == N64Volume50NH::plane ? "plane" : "capsule")
                  << " half_width=" << r.halfWidth << " half_height=" << r.halfHeight
                  << " dir=" << r.direction[0] << ',' << r.direction[1] << ',' << r.direction[2] << " surface=";
        if (!r.surfaceCount) std::cerr << "any";
        for (std::size_t n = 0; n < r.surfaceCount; ++n) std::cerr << (n ? "," : "") << r.surface[n];
        std::cerr << " owners=" << (i < owners_.size() ? owners_[i].size() : 0) << " action=" << kindName(a.kind) << " target=";
        if (a.kind == N64HazardKind50NH::explodePod)
            std::cerr << "pod after=" << (a.explode.after == N64AfterDestroy50NH::respawn ? "respawn" : "retire")
                      << " gate=" << (a.explode.gate == N64ExplodeGate50NH::all ? "all" : a.explode.gate == N64ExplodeGate50NH::local ? "local" : "local_or_near_camera");
        else {
            const auto& number = targetNumber(a);
            const auto& ref = targetRef(a);
            if (number >= 1) std::cerr << "animation#" << number; else if (!ref.empty()) std::cerr << ref; else std::cerr << "none";
        }
        std::cerr << " once=" << r.oneTimeOnly << '\n';
    }
    void resolveTargets() noexcept {
        for (std::size_t i = 0; i < targets_.size(); ++i) {
            auto& t = targets_[i]; t = {};
            const auto& a = hazards_.row(i).actions[0];
            if (a.kind == N64HazardKind50NH::explodePod) continue;
            if (!animation_) continue;   // logged at use: target_unbound reason=animation_runtime
            struct Names { const N64TrackAnimationPort50NH* p; std::size_t size() const { return p->rowCount(); }
                           const std::string& operator[](std::size_t k) const { return p->referenceName(k); } } names{animation_};
            const auto number = targetNumber(a);
            const auto& ref = targetRef(a);
            if (number >= 1 || !ref.empty()) t.valid = resolveN64AnimationTarget50NH(number, ref, names, t.row);
            if(a.kind==N64HazardKind50NH::earthquake)
                t.effectValid=resolveN64AnimationTarget50NH(-1,a.quake.effectNameRef,names,t.effect);
            if (a.kind == N64HazardKind50NH::breakObject && (a.breakObject.effectAnimation >= 1 || !a.breakObject.effectNameRef.empty()))
                t.effectValid = resolveN64AnimationTarget50NH(a.breakObject.effectAnimation, a.breakObject.effectNameRef, names, t.effect);
            std::cerr << "n64_trigger target trigger=" << hazards_.row(i).trigger << " row=" << (t.valid ? std::to_string(t.row) : std::string("missing"));
            if (a.kind == N64HazardKind50NH::breakObject) std::cerr << " effect_row=" << (t.effectValid ? std::to_string(t.effect) : std::string("missing"));
            std::cerr << '\n';
        }
    }
    void unbound(std::size_t row, const char* action) noexcept {
        if (row < unboundLogged_.size() && unboundLogged_[row]) return;
        if (row < unboundLogged_.size()) unboundLogged_[row] = true;
        n64Log50NH("n64_trigger target_unbound trigger=%u action=%s reason=animation_runtime\n", hazards_.row(row).trigger, action);
    }
    void positionBreakEffect(std::size_t i) noexcept {
        const auto& e=effects_[i];
        const auto& q=hazards_.row(i).actions[0].breakObject;
        float w[16]{}; w[0] = w[5] = w[10] = w[15] = 1.f;
        float scale = 1.f;
        if (e.follow && e.slot < L::slots && humanValid_[e.slot]) {
            // N64 swrObjTrig_F2: the pod's position + 12 x its forward axis (vB).
            for (unsigned k = 0; k < 3; ++k) w[12 + k] = humanPos_[e.slot][k] + q.effectAhead * forward_[e.slot][k];
        } else {
            const auto& c = hazards_.row(i).centre;
            for (unsigned k = 0; k < 3; ++k) w[12 + k] = c[k];
            scale = e.follow ? 1.f : q.effectAiScale;
        }
        const float opacity = std::max(0.f,(q.effectFade - e.age) / q.effectFade);
        animation_->setTransform(targets_[i].effect, w, scale, std::floor(255.f*opacity)/255.f);
    }
    void endEffect(std::size_t i, bool log) noexcept {
        if (i >= effects_.size() || !effects_[i].live) return;
        if (animation_ && i < targets_.size() && targets_[i].effectValid) {
            animation_->clearTransform(targets_[i].effect); animation_->hide(targets_[i].effect, false);
        }
        if (log) n64Log50NH("n64_break_effect end trigger=%u t=%g\n", hazards_.row(i).trigger, double(effects_[i].age));
        effects_[i] = {};
    }
    void handle(const N64HazardEvent50NH& e, const StepInput& in) noexcept {
        if (e.row >= hazards_.size()) return;
        const auto& r = hazards_.row(e.row); const auto& a = r.actions[e.action < r.actionCount ? e.action : 0];
        const auto slot = e.slot;
        // Carrier owner for the log: the first step owner the row's set holds.
        // Carrier name borrowed from the load-time table (no copy: this runs in the racer frame).
        const char* carrier = r.surfaceCount ? "none" : "any";
        bool carrierFound = !r.surfaceCount;
        if (r.surfaceCount && in.step.groundOwners)
            for (std::size_t g = 0; g < in.step.groundOwnerCount && !carrierFound; ++g)
                for (const auto o : owners_[e.row]) if (o == in.step.groundOwners[g]) { carrier = ownerNameView(o); carrierFound = true; break; }
        // The capsule (door) fires every frame inside: its fire line is the door event line below.
        if (a.kind != N64HazardKind50NH::animationToggle)
            n64Log50NH("n64_trigger fire trigger=%u slot=%u human=%d volume=%s sign=%s t=%g race_s=%g pos=%g,%g,%g mode=%s carrier_owner=%s\n",
                       r.trigger, unsigned(slot), int(in.step.human), r.volume == N64Volume50NH::plane ? "plane" : "capsule",
                       e.crossingSign >= 0 ? "+1" : "-1", double(e.t), raceSeconds_, double(in.step.current[0]), double(in.step.current[1]),
                       double(in.step.current[2]), in.flight ? "flight" : "normal", carrier);
        switch (a.kind) {
        case N64HazardKind50NH::explodePod: {
            if (e.refused) {
                n64Log50NH("n64_explode refused slot=%u trigger=%u reason=%s dist=%g\n", unsigned(slot), r.trigger, e.refused,
                           double(in.step.nearestCameraDistance));
                return;
            }
            if (explode_[slot].pending||explosionPhase_[slot].exploding) return;
            explosionPhase_[slot].start(e.explosionSeconds);
            auto& q = explode_[slot]; q = {};
            q.pending = true; q.left = e.explodeLeft; q.row = e.row; q.after = a.explode.after; q.respawnDelay = a.explode.respawnDelay;
            q.waitForDeath=!in.step.human;
            q.gate = in.step.human ? "local" : e.nearCamera ? "near_camera" : "all";
            q.dist = in.step.human ? 0.f : in.step.nearestCameraDistance;
            return;
        }
        case N64HazardKind50NH::breakObject: {
            const auto& q = a.breakObject;
            // 'Hitt' T201: 3 equal hits on random parts; the two Revenge impact components.
            std::uint32_t parts[N64BreakDamageRequest50NH::capacity]{}; float amounts[N64BreakDamageRequest50NH::capacity]{};
            const auto hits = hazards_.breakDamage(q, in.boosting, 2, parts, amounts, N64BreakDamageRequest50NH::capacity);
            auto& d = damage_[slot];
            if (!d.pending) { d = {}; d.pending = true; d.row = e.row; }
            char damageText[N64BreakDamageRequest50NH::capacity * 24 + 1] = "";
            std::size_t used = 0;
            for (std::size_t k = 0; k < hits && d.count < N64BreakDamageRequest50NH::capacity; ++k) {
                d.mask[d.count] = parts[k] ? 0x10u : 1u; d.amount[d.count] = amounts[k]; ++d.count;
                if (used + 1 < sizeof damageText) {
                    const int w = std::snprintf(damageText + used, sizeof damageText - used, "%s%d:%g", used ? "," : "",
                                                parts[k] ? 0 : 1, double(amounts[k]));
                    if (w > 0) used = std::min(sizeof damageText - 1, used + static_cast<std::size_t>(w));
                }
            }
            const float thrust = N64TrackHazards50NH::decelerator(q.thrustDeceleration, in.frameSeconds);
            d.thrustFactor *= thrust;
            const float pitch = hazards_.soundPitch(q);
            bool hidden = false; const char* effect = in.step.human ? "follow" : "fixed";
            if (!animation_) unbound(e.row, "BreakObject");
            else {
                const auto& t = targets_[e.row];
                if (t.valid) hidden = animation_->hide(t.row, true);                 // 1011A0 + collision off
                if (t.effectValid) {
                    // N64 FX slot 0 has one model and animation clock across all asteroids.
                    for(std::size_t i=0;i<effects_.size();++i)
                        if(effects_[i].live&&targets_[i].effect==t.effect)endEffect(i,false);
                    if(animation_->show(t.effect)&&animation_->fire(t.effect)&&animation_->setTime(t.effect,0.f)){
                        auto& fx=effects_[e.row];fx.live=true;fx.follow=in.step.human;fx.slot=slot;fx.age=0.f;
                        positionBreakEffect(e.row);
                        if(firstShownAnimationTime_<0.)firstShownAnimationTime_=raceSeconds_;
                    }
                }
            }
            const bool targetValid = animation_ && targets_[e.row].valid;
            n64Log50NH("n64_break trigger=%u slot=%u target=%lld hidden=%d collision_owners=%d thrust_factor=%g boosting=%d"
                       " damage=%s effect=%s sound=%s pitch=%g sound_played=%d\n", r.trigger, unsigned(slot),
                       targetValid ? static_cast<long long>(targets_[e.row].row) : -1LL, int(hidden), hidden ? 1 : 0, double(thrust),
                       int(in.boosting), damageText, animation_ && targets_[e.row].effectValid ? effect : "none",
                       q.sound.empty() ? "-" : q.sound.c_str(), double(pitch), int(playImpact(q,in.step.current,pitch)));
            return;
        }
        case N64HazardKind50NH::earthquake:{
            const auto& target=targets_[e.row];
            if(!animation_||!target.valid||!target.effectValid){unbound(e.row,"Earthquake");return;}
            n64PillarCameraShake50NH.start(a.quake.feetPerUnit,0.25f,8.f,0.f);
            auto& fx=effects_[e.row];fx.age=0.f;fx.quake=QuakeStage::armed;
            n64Log50NH("n64_quake armed trigger=%u lap=%u\n",r.trigger,unsigned(in.step.lap+1));
            return;
        }
        case N64HazardKind50NH::animationToggle: {
            const auto& q = a.toggle;
            if (!animation_ || !targets_[e.row].valid) { unbound(e.row, "AnimationToggle"); return; }
            auto& tg = toggles_[e.row];
            const auto row = targets_[e.row].row;
            const float since = tg.machine.sinceToggle();
            const auto c = tg.machine.fire(q, animation_->time(row));
            if (!c.apply) {
                if (!tg.guardLogged) { tg.guardLogged = true;
                    n64Log50NH("n64_door anim=%u event=%s time=%g since_toggle=%g slot=%u\n", unsigned(row), c.event,
                               double(animation_->time(row)), double(since), unsigned(slot)); }
                return;
            }
            tg.guardLogged = false;
            if (c.setTime) animation_->setTime(row, c.time);
            animation_->setRate(row, c.rate, c.loop);
            n64Log50NH("n64_door anim=%u event=%s time=%g rate=%g since_toggle=%g slot=%u trigger=%u\n", unsigned(row), c.event,
                       double(animation_->time(row)), double(c.rate), double(since), unsigned(slot), r.trigger);
            return;
        }
        case N64HazardKind50NH::animationPlay: {
            // 312 pillar / 314 methane: (show,) start the row's sequence at the authored
            // rate, once (the row's OneTimeOnly), plus Revenge's own CameraShake when authored.
            const auto& q = a.play;
            bool shown = false, fired = false, rated = false;
            if (!animation_ || !targets_[e.row].valid) unbound(e.row, "AnimationPlay");
            else {
                const auto row = targets_[e.row].row;
                if (q.show) shown = animation_->show(row);
                fired = animation_->fire(row);
                rated = q.rate == 1.f || animation_->setRate(row, q.rate, false);
            }
            if(q.n64PillarShakeScale>0.f){
                n64PillarCameraShake50NH.start(q.n64PillarShakeScale);
                n64Log50NH("n64_pillar_shake trigger=%u slot=%u feet_per_unit=%g\n",r.trigger,unsigned(slot),double(q.n64PillarShakeScale));
            }
            if (q.show && shown && fired && firstShownAnimationTime_ < 0.0) firstShownAnimationTime_ = raceSeconds_;
            if (q.shake) cameraShakeRelay50CS.request({q.shakeMagnitude, q.shakeTime, q.shakeDecaying, q.shakeDelay});
            n64Log50NH("n64_play trigger=%u slot=%u target=%lld shown=%d fired=%d rate=%g rate_set=%d shake=%s magnitude=%g time=%g\n",
                       r.trigger, unsigned(slot), animation_ && targets_[e.row].valid ? static_cast<long long>(targets_[e.row].row) : -1LL,
                       int(shown), int(fired), double(q.rate), int(rated), q.shake ? "yes" : "none", double(q.shakeMagnitude), double(q.shakeTime));
            return;
        }
        }
    }
    // RR_TEST_N64_HAZARD rows across the course's own map line (test only; see the parser).
    void appendTestRows(std::vector<N64HazardRow50NH>& rows, const std::vector<std::uint8_t>* archiveBytes, const char* mapMember) {
        const char* text = std::getenv("RR_TEST_N64_HAZARD");
        if (!text || !*text) return;
        std::string error;
        const auto tests = parseN64TestHazards50NH(text, error);
        if (!error.empty()) { std::cerr << "n64_hazards test_refused reason=\"" << error << "\"\n"; return; }
        N64AfterDestroy50NH after = N64AfterDestroy50NH::retire;
        if (const char* v = std::getenv("RR_TEST_N64_AFTER")) {
            const std::string s(v);
            if (s == "respawn") after = N64AfterDestroy50NH::respawn;
            else if (s != "retire") { std::cerr << "n64_hazards test_refused reason=\"RR_TEST_N64_AFTER retire|respawn\"\n"; return; }
        }
        if (!archiveBytes || !mapMember) { std::cerr << "n64_hazards test_refused reason=no_map_spline\n"; return; }
        assets::ResArchive48C archive; assets::DecodeError48C why{}; std::vector<std::uint8_t> bytes;
        if (!assets::decodeResource48C(*archiveBytes, archive, why) || !assets::extractResMember48C(archive, mapMember, bytes, why)) {
            std::cerr << "n64_hazards test_refused reason=map_member\n"; return; }
        auto scratch = std::make_unique<UndertrackReadScratch50ET>(); auto track = std::make_unique<UndertrackTrack50ET>();
        UndertrackDiagnostic50ET d{};
        if (!readUndertrackTrack50ET(reinterpret_cast<const char*>(bytes.data()), bytes.size(), *scratch, *track, d)) {
            std::cerr << "n64_hazards test_refused reason=map_spline\n"; return; }
        for (std::size_t i = 0; i < tests.size(); ++i) {
            const auto& t = tests[i];
            if (rows.size() >= L::rows) { std::cerr << "n64_hazards test_refused reason=rows_exceed_owner entry=" << i << '\n'; break; }
            if (t.node >= track->count || !track->nodes[t.node].nextCount || track->nodes[t.node].next[0] >= track->count) {
                std::cerr << "n64_hazards test_refused reason=node entry=" << i << " node=" << t.node << '\n'; continue; }
            const auto& a = track->nodes[t.node]; const auto& b = track->nodes[a.next[0]];
            N64HazardRow50NH r; r.trigger = 1000u + static_cast<std::uint32_t>(i);
            r.name = std::string("test_") + (t.kind == N64HazardKind50NH::explodePod ? "explode" : t.kind == N64HazardKind50NH::breakObject ? "break" :
                     t.kind == N64HazardKind50NH::animationPlay ? "play" : "door") + "_" + std::to_string(t.node);
            for (unsigned k = 0; k < 3; ++k) r.centre[k] = a.position[k];
            float dx = b.position[0] - a.position[0], dz = b.position[2] - a.position[2]; const float len = std::sqrt(dx * dx + dz * dz);
            if (!(len > 1e-3f)) { std::cerr << "n64_hazards test_refused reason=zero_link entry=" << i << '\n'; continue; }
            r.direction[0] = dx / len; r.direction[1] = 0.f; r.direction[2] = dz / len;
            r.halfHeight = 80.f; r.halfWidth = 60.f;
            r.volume = t.kind == N64HazardKind50NH::animationToggle || t.kind == N64HazardKind50NH::animationPlay ? N64Volume50NH::capsule : N64Volume50NH::plane;
            r.oneTimeOnly = t.kind == N64HazardKind50NH::breakObject || t.kind == N64HazardKind50NH::animationPlay;
            auto& x = r.actions[0]; x.kind = t.kind; x.authoredAction = 1; r.actionCount = 1;
            if (t.kind == N64HazardKind50NH::explodePod) {
                if (t.hasArg) { if (!(t.arg > 0.f && t.arg <= L::maxExtent)) { std::cerr << "n64_hazards test_refused reason=halfwidth entry=" << i << '\n'; continue; } r.halfWidth = t.arg; }
                x.explode.gate = N64ExplodeGate50NH::all; x.explode.side = N64ExplodeSide50NH::random; x.explode.after = after;
            } else if (t.kind == N64HazardKind50NH::breakObject) {
                if (t.hasArg && t.arg >= 1.f) x.breakObject.animationNumber = static_cast<std::int32_t>(t.arg);
            } else if (t.kind == N64HazardKind50NH::animationPlay) {
                if (!(t.arg >= 1.f)) { std::cerr << "n64_hazards test_refused reason=play_anim entry=" << i << '\n'; continue; }
                x.play.animationNumber = static_cast<std::int32_t>(t.arg);
            } else {
                if (!(t.arg >= 1.f)) { std::cerr << "n64_hazards test_refused reason=door_anim entry=" << i << '\n'; continue; }
                x.toggle.animationNumber = static_cast<std::int32_t>(t.arg);
            }
            rows.push_back(std::move(r));
        }
    }

    bool loaded_ = false;
    N64TrackHazards50NH hazards_;
    std::vector<std::vector<std::uint32_t>> owners_;
    std::vector<std::string> ownerNames_;
    std::size_t staticOwnerCount_ = 0;
    std::uint32_t firstAnimatedIdentity_ = 0;
    std::vector<Toggle> toggles_;
    std::vector<Target> targets_;
    std::vector<Effect> effects_;
    std::vector<bool> unboundLogged_, skipLogged_;
    std::weak_ptr<N64ImpactSound50NH> impactSound_;
    std::array<std::array<float,2>,2> cameraForward_{};
    bool playImpact(const N64BreakParams50NH& q, const float* position, float pitch) const noexcept {
        auto owner=impactSound_.lock();
        if(!owner || q.sound.empty())return false;
        float distance=q.soundMaxDistance;
        int angle=0;
        for(std::size_t i=0;i<cameraEyes_.size();++i){
            if(!cameraValid_[i])continue;
            const float x=position[0]-cameraEyes_[i][0],y=position[1]-cameraEyes_[i][1],z=position[2]-cameraEyes_[i][2];
            const float d=std::sqrt(x*x+y*y+z*z);
            if(!(d<distance))continue;
            distance=d;
            const auto& f=cameraForward_[i];
            angle=static_cast<int>(std::atan2(x*f[1]-z*f[0],x*f[0]+z*f[1])*57.29578f);
            if(angle<0)angle+=360;  // Mixer bearing is clockwise, 0..359.
        }
        if(distance>=q.soundMaxDistance)return false;
        const float gain=distance<=q.soundMinDistance?1.f:
            (q.soundMaxDistance-distance)/(q.soundMaxDistance-q.soundMinDistance);
        return owner->playImpact(q.sound,gain,pitch,angle);
    }
    N64TrackAnimationPort50NH* animation_ = nullptr;
    std::array<N64Explosion50NH,L::slots> explosionPhase_{};
    std::array<N64ExplodeRequest50NH, L::slots> explode_{};
    std::array<N64BreakDamageRequest50NH, L::slots> damage_{};
    std::array<Respawn, L::slots> respawn_{};
    std::array<bool, L::slots> placement_{}, prevValid_{}, humanValid_{};
    std::array<std::array<float,3>,2> cameraEyes_{};
    std::array<bool,2> cameraValid_{};
    float prev_[L::slots][3]{}, humanPos_[L::slots][3]{}, forward_[L::slots][3]{};
    double firstShownAnimationTime_ = -1.0;
    double raceSeconds_ = 0.0; bool haveClock_ = false;
};
inline N64HazardRuntime50NH n64HazardRuntime50NH;

} // namespace rr::game
