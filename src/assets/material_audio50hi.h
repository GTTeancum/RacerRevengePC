#pragma once
#include "import_world48f.h"
#include "audio_assets50hc.h"
#include "../game/material_audio50hi.h"
#include <array>
namespace rr::assets {
struct MaterialAudioCatalog50HI {
    std::array<std::string,8> collision{},drive{};
    rr::game::MaterialAudioParameters50HI parameters{};
};
// Cold retained TA.aub decoder. Uses actual SoundResourceFile and six drive
// parameters; no guessed VAG names, generic impacts or timing thresholds.
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI&,std::vector<AudioRequest50HC>& uniqueRequests,std::string&error);
// Source track-specific collision loader1B5430 reads MaterialID with default1;
// creator1B570C copies its positive byte toA9 (fresh collision default is0).
// Call with the exact SCB instance that supplied the retained collider.
bool readTrackContactMaterial50HI(const World48F&,std::uint32_t zone,
    std::uint32_t ordinal,std::uint32_t candidateIdentity,rr::game::ContactMaterial50HI&,
    std::string&error);
}
