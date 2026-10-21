#pragma once
#include "camera_view49f.h"
#include "original_camera_start_effects.h"
#include <array>
#include <cstdint>
namespace original_camera_session50KX {class Session;}
namespace rr::game {
struct OriginalRacerDiffuseCameraSnapshot50LA {
    std::uintptr_t owner=0;
    std::uint64_t revision=0,generation=0,publication=0,frame=0;
    CameraViewOutput49F camera{};
    std::array<float,16> guardCombined{};
    OriginalRaceFogFields fog{};
};
// Only the actual Session can bind this endpoint. Every read revalidates its
// private fog preparation, including current camera, guard and track fields.
class OriginalRacerDiffuseCamera50LA final {
    friend class ::original_camera_session50KX::Session;
    const void* owner_=nullptr;
    bool (*read_)(const void*,OriginalRacerDiffuseCameraSnapshot50LA&)noexcept=nullptr;
public:
    OriginalRacerDiffuseCamera50LA()=default;
    OriginalRacerDiffuseCamera50LA(const OriginalRacerDiffuseCamera50LA&)=delete;
    OriginalRacerDiffuseCamera50LA& operator=(const OriginalRacerDiffuseCamera50LA&)=delete;
    bool read(OriginalRacerDiffuseCameraSnapshot50LA& out)const noexcept {
        return owner_&&read_&&read_(owner_,out);
    }
};
}
