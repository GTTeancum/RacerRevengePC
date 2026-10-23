#pragma once
#include "../../../src/game/original_wreck_components50la.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include <array>
#include <memory>
#include <string>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
// Reached AUD configuration owners, not live Sounds or playback admission.
class OriginalUiGarageAudioFields50KG final {
    friend class OriginalUiGaragePodScene50KG;
    using C=game::OriginalWreckComponent50LA;
    struct Descriptor final {
        std::string soundResource00="none";
        std::array<float,4> position110{{0,0,0,1}};
        float maxDistance120=0,minDistance124=0;
        std::int32_t looping128=0,oneShot12C=0,resetWhenInaudible130=0,force2D134=0,stereo138=0;
        float oneShotDistance13C=0,randomTriggerPercent140=0;
        // Native cold parameter payload.278650 zeroes all112 source bytes;
        //2787B0 then copies only position160 and distances1A8/1AC. This is
        // real retained data, never a guest allocation/handle or live Sound.
        std::array<std::uint8_t,112> coldParameters150{};
        std::int32_t priority1C0=10;
        std::int32_t sampleSpeed1D0=0,minVolume1D4=0,minSpeed1D8=0,maxSpeed1DC=0,resonantSpeed1E0=0;
        float frequencySlope1E4=0;
        bool driveOn=false,parsed=false;
    };
    struct List final {
        C entity;
        std::int32_t count30=0;
        std::unique_ptr<Descriptor[]> rows34;
        bool live=false,complete=false,retiring=false,retired=false;
    };
    std::unique_ptr<List> ambient3B4,soultree3B8,collision3BC,driveOn3C0;
    std::int32_t materialsCount=0,defaultMaterial=0;
    std::array<std::int32_t,2> collisionObjectsPerMaterial{};
    bool complete=false,closed=false;
    OriginalUiGarageAudioFields50KG()noexcept=default;
public:
    ~OriginalUiGarageAudioFields50KG()noexcept;
    OriginalUiGarageAudioFields50KG(const OriginalUiGarageAudioFields50KG&)=delete;
    OriginalUiGarageAudioFields50KG& operator=(const OriginalUiGarageAudioFields50KG&)=delete;
};
}
