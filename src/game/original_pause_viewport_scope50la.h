#pragma once
#include "original_camera_start_effects.h"
#include <cmath>
namespace original_pause_observation50LA {class Owner;}
namespace rr::game {
// Original115A60 save/copy leaf for1B9E0C..1B9E90. Only the concrete
// observation owner can enter it on its synchronously retained camera.
class OriginalPauseViewportScope50LA final {
    friend class original_pause_observation50LA::Owner;
    OriginalCameraViewportFields* actual_=nullptr;
    std::int32_t x_=0,y_=0,width_=0,height_=0;
    float near_=0,far_=1;
    bool valid_=false;
    explicit OriginalPauseViewportScope50LA(OriginalCameraViewportFields& actual,bool overrideViewport)noexcept {
        if(!actual.prepared||!actual.matricesReady||!actual.sceneKey||!actual.sessionGeneration||
           actual.width1E8<=0||actual.height1EC<=0||
           !std::isfinite(actual.depthNear1F0)||!std::isfinite(actual.depthFar1F4))return;
        valid_=true;if(!overrideViewport)return;
        actual_=&actual;x_=actual.x1E0;y_=actual.y1E4;width_=actual.width1E8;height_=actual.height1EC;
        near_=actual.depthNear1F0;far_=actual.depthFar1F4;
        // Source modifies only the four rectangle words; incoming Z range is
        // retained.115A60 does not rebuild camera matrices or publish a camera.
        actual.x1E0=0;actual.y1E4=0;actual.width1E8=640;actual.height1EC=448;
    }
public:
    OriginalPauseViewportScope50LA(const OriginalPauseViewportScope50LA&)=delete;
    OriginalPauseViewportScope50LA& operator=(const OriginalPauseViewportScope50LA&)=delete;
    ~OriginalPauseViewportScope50LA(){
        if(!actual_)return;
        actual_->x1E0=x_;actual_->y1E4=y_;actual_->width1E8=width_;actual_->height1EC=height_;
        actual_->depthNear1F0=near_;actual_->depthFar1F4=far_;
    }
};
}
