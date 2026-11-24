#pragma once

namespace rr::game {
// Camera event Shak (800440F0) and the unpaused advance at 80043B80.
// Keep the oscillator in N64 units until publishing the two world-space offsets.
struct N64PillarCameraShake50NH {
    float scale=0.f, offset=0.f, rate=0.f;
    double elapsed=0.0;
    float amplitude=0.05f, duration=4.f;
    void start(float courseScale,float limit=0.05f,float velocity=4.f,float seconds=4.f) noexcept {
        scale=courseScale;offset=0.f;rate=velocity;elapsed=0.0;amplitude=limit;duration=seconds;
    }
    void advance(double dt) noexcept {
        if(scale==0.f||dt<=0.0)return;
        elapsed+=dt;
        if(duration>0.f&&elapsed>duration){*this={};return;} // Trigger object 8007BDF4 sends Shak(0,0).
        offset=static_cast<float>(double(offset)+double(rate)*dt);
        if(offset>amplitude||offset < -amplitude){
            rate=-rate;
            offset=offset>0.f?amplitude:-amplitude;
        }
    }
    float targetHeight() const noexcept {return scale*offset;}
    float eyeHeight() const noexcept {return 0.5f*targetHeight();}
};
inline N64PillarCameraShake50NH n64PillarCameraShake50NH;
}
