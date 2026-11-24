#pragma once

namespace rr::game {
struct N64Explosion50NH {
    bool exploding=false,dead=false;
    float explosionSeconds=0.f,protectionSeconds=0.f;

    void start(float duration) noexcept {
        exploding=true;
        dead=false;
        explosionSeconds=duration; // 80070830 stores 2 + rand()/2^31.
    }
    void advanceRacer(double seconds,float signedSpeed) noexcept {
        if(!exploding||dead||seconds<=0.0)return;
        explosionSeconds=static_cast<float>(double(explosionSeconds)-seconds);
        dead=signedSpeed<200.f||explosionSeconds<0.f; // 8006C32C and 8006C34C.
    }
    void respawn() noexcept {
        *this={};
        protectionSeconds=3.f; // 80041BF8..80041C10.
    }
    void advanceProtection(double seconds) noexcept {
        if(protectionSeconds<=0.f||seconds<=0.0)return;
        const auto remaining=static_cast<float>(double(protectionSeconds)-seconds);
        protectionSeconds=remaining>0.f?remaining:0.f;
    }
};
}
