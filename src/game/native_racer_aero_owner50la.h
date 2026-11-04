#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
namespace native_racer50HN {class Bank;}
namespace native_racer_model50LA {class Owner;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
class OriginalRacerEntityCold50LA;
class OriginalRacerEngine34COwner50LA;
class OriginalRacerAero350Owner50LA;
class OriginalRacerMainBodyOwner50LA;
class OriginalApplicationEntities50LA;
struct VehicleFrameState49W;
struct MainBodyParameters49K;
//Current native per-racer Aero flow authority. Not guest350, an Entity child,
//or proof that all original mutable34/40 writer history has been joined.
//Existing coefficients/timers/control lanes remain in their actual Bank owner.
class NativeRacerAeroOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class OriginalRacerEngine34COwner50LA;
    friend class OriginalRacerAero350Owner50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    const ::native_racer50HN::Bank* bank_;
    VehicleFrameState49W* frame_;
    MainBodyParameters49K* parameters_;
    const OriginalRacerMainBodyOwner50LA* body_;
    const OriginalApplicationEntities50LA* application_;
    const void* model_;
    const std::uint64_t generation_;
    const std::size_t slot_;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    std::uint64_t racerKey_=0;
    //Legacy constructor owns this numeric100370 initialization. In the
    //FreshSourceTag branch zero is only initialized host backing, not a
    //known source word until the actual100330/16CCD0 owner writes it.
    std::uint32_t flowEnabled34_=0;
    std::array<std::uint32_t,4> flow40Words_; //Actual App3D6B40 initializer copy.
    // Native legacy coefficients absent from AeroParameters48E, in actual
    // offset order5C,60,64,70,74,7C,80,8C,94,98,9C,A0,A4,A8,AC,B4,BC,C0,C4.
    // The canonical nine used by48E remain in parameters_->aero. These lanes
    // become known only when this native continuation reaches1004B0.
    std::array<std::optional<float>,19> nativeBaseCoefficients_{};
    bool published_=false,revoked_=false;
    //Legacy current-state ctor remains unchanged. Fresh source reservation
    //has no claimed100330/16CCD0 numeric writes or runtime read authority.
    struct FreshSourceTag final {};
    bool sourceFresh_=false,sourceConstructorStarted_=false,sourceConstructorComplete_=true;
    NativeRacerAeroOwner50LA(FreshSourceTag,const ::native_racer50HN::Bank& bank,
        VehicleFrameState49W& frame,MainBodyParameters49K& parameters,
        const OriginalRacerMainBodyOwner50LA& body,const OriginalApplicationEntities50LA& app,
        const void* model,std::uint64_t generation,std::size_t slot)noexcept
        :bank_(&bank),frame_(&frame),parameters_(&parameters),body_(&body),application_(&app),
         model_(model),generation_(generation),slot_(slot),flow40Words_{},
         sourceFresh_(true),sourceConstructorComplete_(false){}

    NativeRacerAeroOwner50LA(const ::native_racer50HN::Bank& bank,VehicleFrameState49W& frame,
        MainBodyParameters49K& parameters,const OriginalRacerMainBodyOwner50LA& body,
        const OriginalApplicationEntities50LA& app,const void* model,std::uint64_t generation,
        std::size_t slot,const std::array<std::uint32_t,4>& processOrigin)noexcept
        :bank_(&bank),frame_(&frame),parameters_(&parameters),body_(&body),application_(&app),
         model_(model),generation_(generation),slot_(slot),flow40Words_(processOrigin){}
public:
    NativeRacerAeroOwner50LA(const NativeRacerAeroOwner50LA&)=delete;
    NativeRacerAeroOwner50LA& operator=(const NativeRacerAeroOwner50LA&)=delete;
};
}
