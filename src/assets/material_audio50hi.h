#pragma once
#include "import_world48f.h"
#include "audio_assets50hc.h"
#include "../game/material_audio50hi.h"
#include <array>
namespace rr::assets {
struct MaterialAudioCatalog50HI {
    std::array<std::string,rr::game::maxMaterialAudioRows50HI> collision{},drive{};
    struct ObjectMaterial {std::uint32_t material=0;std::string object;};
    std::vector<ObjectMaterial> orderedObjectMaterials;
    std::uint32_t materialCount=0,defaultMaterial=0;
    rr::game::MaterialAudioParameters50HI parameters{};
};
// Cold original selected AUB decoder (bounded8/12/20-row profiles). Uses actual SoundResourceFile and six drive
// parameters; no guessed VAG names, generic impacts or timing thresholds.
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI&,std::vector<AudioRequest50HC>& uniqueRequests,std::string&error);
// Source track-specific collision loader1B5430 reads MaterialID with default1;
// creator1B570C copies its positive byte toA9 (fresh collision default is0).
// Call with the exact SCB instance that supplied the retained collider.
// Original271600, applied BEFORE1B570C publishes A9. Only uniquely named
// consumed fields participate. Unrelated duplicated ambient labels stay opaque.
bool applyTrackMaterialAssignments50HI(const MaterialAudioCatalog50HI&,const World48F&,
    std::uint32_t zone,std::uint32_t ordinal,rr::game::ContactMaterial50HI&,std::string&error);
bool readTrackContactMaterial50HI(const World48F&,std::uint32_t zone,
    std::uint32_t ordinal,std::uint32_t candidateIdentity,rr::game::ContactMaterial50HI&,
    std::string&error);
}
