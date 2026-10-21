#pragma once
#include "import_world48f.h"
#include "audio_assets50hc.h"
#include "../game/material_audio50hi.h"
#include <array>
namespace rr::assets {
struct MaterialAudioCatalog50HI {
    std::array<std::string,rr::game::maxMaterialAudioRows50HI> collision{};
    std::array<std::string,rr::game::maxMaterialDriveRows50HI> drive{};
    // Selected source section for every distinct sound owner. SA collision
    // owners17..32 reread section16 after original157D00 fails to select them.
    std::array<std::uint32_t,rr::game::maxMaterialAudioRows50HI> collisionSectionKeys{};
    std::array<std::uint32_t,rr::game::maxMaterialDriveRows50HI> driveSectionKeys{};
    struct ObjectMaterial {std::uint32_t material=0;std::string object;};
    std::vector<ObjectMaterial> orderedObjectMaterials;
    std::uint32_t materialCount=0,defaultMaterial=0;
    rr::game::MaterialAudioParameters50HI parameters{};
};
// Cold original selected AUB decoder (TA8/8, GC12/12, GA20/20, MA16/12,
// SA32/12 collision/drive profiles). Uses actual SoundResourceFile and six drive
// parameters; no guessed VAG names, generic impacts or timing thresholds.
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI&,std::vector<AudioRequest50HC>& uniqueRequests,std::string&error);
// Exact authenticated MB AUB only:20 collision/12 drive,3 materials,
//25 immutable PCM filenames and29 source-ordered assignment rows. This entry
// does not broaden the legacy reader or admit an arbitrary count policy.
bool readMbMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI&,std::vector<AudioRequest50HC>& uniqueRequests,std::string&error);
// Exact authenticated TC AUB only:16 collision/12 drive,5 declared materials,
//21 PCM names and8 source-ordered material2 assignments. The generic4 bound
//stays unchanged; logical sound owners are never deduplicated with PCM.
bool readTcMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI&,std::vector<AudioRequest50HC>& uniqueRequests,std::string&error);
// Exact authenticated TB AUB only:24 collision/20 drive,5 materials,
//33 immutable PCM names and54 ordered assignments. WOODHIT1..4 PCM is shared
//by distinct one-shot collision and looping drive owners; request.loop remains
//the first collision request's metadata and never governs PCM decoding.
// Exact original MC AUB only: twelve collision rows/eight drive rows,
// five declared materials, three strict-less-than ordered assignments.
bool readMcMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error);
bool readTbMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
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
