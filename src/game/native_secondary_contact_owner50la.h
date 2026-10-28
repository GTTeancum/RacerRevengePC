#pragma once
#include "vehicle_frame49w.h"
#include <cstddef>
#include <cstdint>

namespace native_racer50HN {class Bank;}
namespace native_racer_model50LA {class Owner;}
namespace native_dynamic_contact50IW {class Registry;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
// Explicit native lifetime adaptation for the existing numerical secondary.
// NOT a guest35C Entity, a scene288 publication, or source constructor return.
//212538/54C creates a DIFFERENT167920 from340; its20C/210 and238/23C
// must never alias the main body's contact identity or ten-slot210 backing.
class NativeSecondaryContactOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::native_dynamic_contact50IW::Registry;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    const ::native_racer50HN::Bank* bank_=nullptr;
    VehicleFrameState49W* frame_=nullptr;
    SecondaryBodyParameters49M* parameters_=nullptr;
    std::uint64_t generation_=0;
    std::size_t slot_=0;
    SceneNodeIndex49P physical19C_{};
    bool revoked_=false;
    NativeSecondaryContactOwner50LA(const ::native_racer50HN::Bank& bank,VehicleFrameState49W& frame,
        SecondaryBodyParameters49M& parameters,std::uint64_t generation,
        std::size_t slot,SceneNodeIndex49P physical)noexcept:
        bank_(&bank),frame_(&frame),parameters_(&parameters),generation_(generation),slot_(slot),
        physical19C_(physical){}
public:
    NativeSecondaryContactOwner50LA(const NativeSecondaryContactOwner50LA&)=delete;
    NativeSecondaryContactOwner50LA& operator=(const NativeSecondaryContactOwner50LA&)=delete;
};
}
