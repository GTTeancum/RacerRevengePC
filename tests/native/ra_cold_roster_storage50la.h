#pragma once
// Existing roster storage and fresh-probe identity only. No adoption, roster
// operation, session construction, or lifecycle order is introduced here.
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_contact50hw_fixture.h"
#include "../../src/game/vehicle_frame49w.h"
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
namespace ra_cold_roster50LA {
namespace g=rr::game;namespace a=rr::assets;
class Operation;
// One-time adoption of the EXISTING active roster objects (capacity eight), before world reservation.
// Unique-pointer movement changes no Rig/COL address. No public array getter:
// the private stable array is the future aggregate grid/camera borrow.
struct SharedOwners final {
    std::array<std::unique_ptr<native_racer50HW::Rig>,8> rigs;
    std::array<std::unique_ptr<native_racer_collision50HW::Owner>,8> collisions;
};
// Captures the actual constructor records before initial terrain enable. No
// setter accepts supplied replacement probe records or a readiness boolean.
class FreshProbe final {
    friend class Operation;
    const g::VehicleFrameState49W* frame_;
    g::VehicleTerrainProbe50FP terrain_;
    g::DisabledProbeState49K main_;
    std::uint32_t domain_;
    explicit FreshProbe(const g::VehicleFrameState49W& s)
        :frame_(&s),terrain_(s.terrainProbe),main_(s.bodies.main.components.probe348),domain_(s.bodies.main.domain.probe100){}
public:
    static std::shared_ptr<const FreshProbe> capture(const g::VehicleFrameState49W& s){
        if(s.phase!=g::VehicleFramePhase49W::Ready||s.bodies.main.frameFactor!=0.f||
           s.terrainProbe.enabled||s.bodies.main.domain.probe100)
            throw std::runtime_error("actual pre-enable probe constructor records unavailable");
        return std::shared_ptr<const FreshProbe>(new FreshProbe(s));
    }
};
// Shared native storage is established before Bank registration. Both the
// driver and retained operation use these exact owners; no model is cloned.
struct RuntimeOwners final {
    std::array<std::unique_ptr<native_racer_model50LA::Owner>,8> right,left,bottomRight,bottomLeft,chariot;
    std::array<std::unique_ptr<native_racer50HW::Visual>,8> visuals;
    std::array<std::unique_ptr<native_racer50HW::Cables>,8> cables;
    std::array<native_racer_contact50HW::Binding,8> contacts{};
};
} // namespace ra_cold_roster50LA
