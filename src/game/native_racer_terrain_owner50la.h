#pragma once
#include <cstddef>
#include <cstdint>
namespace native_racer50HN {class Bank;}
namespace native_racer_model50LA {class Owner;}
namespace native_dynamic_contact50IW {class Registry;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
class OriginalRacerEntityCold50LA;
struct VehicleFrameState49W;
//Distinct registered native racer-owner identity. Never a BodyContactIdentity,
//narrowed native pointer, source manager index or original guest9C word.
struct NativeRacerTerrainIdentity50LA {std::uint32_t value=0;};
class NativeRacerTerrainOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::native_dynamic_contact50IW::Registry;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    const ::native_racer50HN::Bank* bank_;
    const VehicleFrameState49W* frame_;
    const void* model_;
    const std::uint64_t generation_;
    const std::size_t slot_;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    std::uint64_t racerKey_=0;
    bool published_=false,revoked_=false;
    NativeRacerTerrainOwner50LA(const ::native_racer50HN::Bank& bank,
        const VehicleFrameState49W& frame,const void* model,std::uint64_t generation,
        std::size_t slot)noexcept
        :bank_(&bank),frame_(&frame),model_(model),generation_(generation),slot_(slot){}
public:
    NativeRacerTerrainOwner50LA(const NativeRacerTerrainOwner50LA&)=delete;
    NativeRacerTerrainOwner50LA& operator=(const NativeRacerTerrainOwner50LA&)=delete;
};
}
