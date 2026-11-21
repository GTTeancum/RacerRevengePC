#pragma once
// PORT EXTENSION, NOT ORIGINAL: binds the N64 hazard runtime's animation port
// (rr::game::N64TrackAnimationPort50NH, n64_track_hazards_runtime50nh.h) to the ONE common
// track-animation runtime (track_animation_runtime50AN::Owner, Track::roles.animation).
// Every call goes through the runtime's own Control interface (fire 101170, hide 1011A0
// and the N64 extensions (a)-(d)); nothing here owns animation state. Rows are the TOB's
// [Animations] rows in table order. Inert on stock: the hazard runtime only calls the port
// for N64 rows, and no stock course has any.
//
// Lifetime: holds a raw pointer, never a share of the runtime (the Track's release counts
// the runtime's holders). The session unbinds it (bind(nullptr)) before it drops its own
// reference to the runtime.
#include "track_animation_runtime50an.h"
#include "../../src/game/n64_track_hazards_runtime50nh.h"
#include <cmath>
#include <string>
#include <vector>

namespace n64_track_animation_port50NH {
class Port final : public rr::game::N64TrackAnimationPort50NH {
    track_animation_runtime50AN::Owner* runtime_ = nullptr;
    // Rows this port changed beyond the original 2702E0 reset: hidden with their collision
    // (BreakObject targets) and rate-overridden (AnimationToggle doors). Sized at bind.
    std::vector<std::uint8_t> hiddenWithCollision_, rateOverridden_, shownStartHidden_;
    static const std::string& empty() noexcept { static const std::string e; return e; }
    bool usable(std::size_t row) const noexcept { return runtime_ && runtime_->ready() && row < runtime_->rowCount(); }
public:
    void bind(track_animation_runtime50AN::Owner* runtime) noexcept {
        runtime_ = runtime;
        try { const auto n = runtime ? runtime->rowCount() : 0u; hiddenWithCollision_.assign(n, 0); rateOverridden_.assign(n, 0);
              shownStartHidden_.assign(n, 0); }
        catch (...) { runtime_ = nullptr; hiddenWithCollision_.clear(); rateOverridden_.clear(); shownStartHidden_.clear(); }
    }
    // N64 'Load' / 'RSet' (the track reloaded fresh): every row this port changed goes back
    // to its authored state - broken asteroids shown with their collision again, doors back
    // under the authored sequence. Called with the hazard runtime's raceReset, before the
    // runtime's own 2702E0 race reset re-poses every row.
    void raceReset() noexcept {
        if (!runtime_ || !runtime_->ready()) return;
        for (std::size_t row = 0; row < hiddenWithCollision_.size() && row < runtime_->rowCount(); ++row) {
            if (hiddenWithCollision_[row]) { runtime_->show(row); hiddenWithCollision_[row] = 0; }
            if (rateOverridden_[row]) { runtime_->clearRate(row); rateOverridden_[row] = 0; }
            // A StartHidden row a hazard showed (methane FX, debris) is hidden again.
            if (shownStartHidden_[row]) { runtime_->hide(row, false); shownStartHidden_[row] = 0; }
        }
    }
    bool bound() const noexcept { return runtime_ != nullptr; }
    std::size_t rowCount() const noexcept override { return runtime_ && runtime_->ready() ? runtime_->rowCount() : 0u; }
    const std::string& referenceName(std::size_t row) const noexcept override {
        if (!usable(row)) return empty();
        try { return runtime_->source().row(row).authored.referenceName; } catch (...) { return empty(); }
    }
    bool fire(std::size_t row) noexcept override { return usable(row) && runtime_->fire(row); }
    bool hide(std::size_t row, bool removeCollision) noexcept override {
        if (!usable(row) || !runtime_->hide(row, removeCollision)) return false;
        if (removeCollision && row < hiddenWithCollision_.size()) hiddenWithCollision_[row] = 1;
        return true;
    }
    bool show(std::size_t row) noexcept override {
        if (!usable(row) || !runtime_->show(row)) return false;
        bool startHidden = false;
        try { startHidden = runtime_->source().row(row).authored.startHidden; } catch (...) {}
        if (startHidden && row < shownStartHidden_.size()) shownStartHidden_[row] = 1;
        return true;
    }
    bool setRate(std::size_t row, float rate, bool loop) noexcept override {
        if (!usable(row) || !runtime_->setRate(row, rate, loop, !loop)) return false;
        if (row < rateOverridden_.size()) rateOverridden_[row] = 1;
        return true;
    }
    bool setTime(std::size_t row, float seconds) noexcept override { return usable(row) && runtime_->setTime(row, seconds); }
    float time(std::size_t row) const noexcept override {
        if (!usable(row)) return 0.f;
        try { return runtime_->state(row).time34; } catch (...) { return 0.f; }
    }
    // world: column-major 4x4 (BodyMatrix48G layout); scale applied to the 3x3 basis.
    bool setTransform(std::size_t row, const float world[16], float scale, float alpha) noexcept override {
        if (!usable(row) || !world || !std::isfinite(scale) || !(scale > 0.f)) return false;
        rr::game::BodyMatrix48G m{};
        for (unsigned i = 0; i < 16; ++i) m.v[i] = world[i];
        for (unsigned r = 0; r < 3; ++r) for (unsigned c = 0; c < 3; ++c) m.v[r * 4 + c] *= scale;
        return runtime_->setTransformOverride(row, &m) && runtime_->setAlpha(row, alpha);
    }
    bool clearTransform(std::size_t row) noexcept override {
        return usable(row) && runtime_->setTransformOverride(row, nullptr) && runtime_->setAlpha(row, 1.f);
    }
};
// The one port, beside the one hazard runtime (rr::game::n64HazardRuntime50NH).
inline Port n64TrackAnimationPort50NH;
} // namespace n64_track_animation_port50NH
