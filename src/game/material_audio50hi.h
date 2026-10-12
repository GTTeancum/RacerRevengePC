#pragma once
#include "gameplay_audio50he.h"
#include <cstddef>
#include <cstdint>
namespace rr::game {
enum class MaterialCueKind50HI : std::uint8_t { Collision, DriveOn };
struct MaterialDriveParameters50HI {
    std::int32_t sampleSpeed=0,minVolume=0,minSpeed=0,maxSpeed=0,resonantSpeed=0;
    float frequencySlope=0.f;
    // Native provider base-pitch units, never fabricated PS2 metadata.
    std::uint32_t basePitch=4096;
};
inline constexpr std::size_t maxMaterialAudioRows50HI=20;
struct MaterialAudioParameters50HI {
    MaterialDriveParameters50HI drive[maxMaterialAudioRows50HI]{};
    std::uint32_t rowCount=8; // Legacy TA callers retain their exact8-row bound.
};
struct ContactMaterial50HI { std::uint32_t candidateIdentity=0; std::uint8_t materialA9=0; };
// Identity0 is the actual no-contact state. A nonzero unresolved identity fails;
// friction record material4C is deliberately not consulted.
bool resolveContactMaterial50HI(std::uint32_t currentCandidate130,
    const ContactMaterial50HI*,std::size_t count,std::int32_t& material) noexcept;
struct MaterialAudioInput50HI {
    std::uint32_t channelArgument=3; // Source D40 A8 arguments1,2,3 only.
    std::int32_t contactMaterialA9=-1;
    float speedRatio70=0.f;
    // Genuine selected player's nearest-listener planar distance, including
    // the actual host camera offset. It controls source540/450/400+speed
    // gates. Final source material volume setters overwrite spatial gain;
    // native pan remains centered and original Doppler is not claimed.
    float listenerDistance=0.f;
    bool vehicle220Present=true,disabled24C=false;
};
struct MaterialAudioState50HI {
    std::uint8_t emitterFlags22C=0;
    std::int32_t driveMaterial240=-1,driveKey244=-1;
    std::int32_t cachedSlope20[maxMaterialAudioRows50HI]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
};
struct MaterialAudioCommand50HI {
    AudioOperation50HE operation=AudioOperation50HE::Start;
    MaterialCueKind50HI kind=MaterialCueKind50HI::Collision;
    std::uint32_t key=0;
    float gain=1.f,pitch=1.f;
    bool loop=false;
    std::uint32_t sourceSite=0;
};
inline constexpr std::size_t maxMaterialAudioCommands50HI=3;
// 179070 ->2C2240/2C2530/2C27D0 and293010. Uses the SAME source308030
// shared RNG owner as other subscribers; no private seed. Missing contact is
// the source lazy skip, not material0 or an inferred stop. No hot allocation.
// State/RNG/commands/count are transactional. Prior and next state may alias;
// other mutable output spans must be distinct. Only complete caller-bound four-variant families up to20 rows are
// admitted; no positive out-of-range material is substituted. Playback availability/restart guards belong to the real provider.
AudioEventStatus50HE updateMaterialAudio50HI(const MaterialAudioParameters50HI&,
    const MaterialAudioInput50HI&,const MaterialAudioState50HI&,MaterialAudioState50HI&,
    std::uint32_t& sharedRandomState,MaterialAudioCommand50HI*,std::size_t capacity,
    std::size_t& count) noexcept;
}
