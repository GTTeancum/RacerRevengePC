#pragma once
#include "original_ui_rider_material_lifetime50kg.h"
#include "../assets/original_ui_image_bank50kg.h"
#include "../assets/original_ui_material50kg.h"
#include <memory>
#include <string>
#include <vector>
namespace rr::platform {
class OriginalUiTournamentDrawContribution50KG;
class OriginalUiTournamentSession50KG;
class OriginalUiRiderSession50KG;
}
namespace rr::game {
class OriginalUiRiderBackend50KG;
struct OriginalUiTournamentPredecessorPass50KG {
    OriginalUiRiderMaterialRow50KG row{};
    std::string registeredName;
    assets::OriginalUiMaterialPass50KG source;
    // All actual image occurrences for this unique registered row/pass.
    // Empty only for a genuine untextured pass. No mutable shader state.
    std::vector<assets::OriginalUiImageProvenance50KG> images;
};
// Immutable evidence exported only by the real complete Tournament collector.
// Neither parsed copies nor row/hash equality transfer live history ownership.
class OriginalUiTournamentPredecessorInventory50KG final {
public:
    OriginalUiTournamentPredecessorInventory50KG(const OriginalUiTournamentPredecessorInventory50KG&)=delete;
    OriginalUiTournamentPredecessorInventory50KG& operator=(const OriginalUiTournamentPredecessorInventory50KG&)=delete;
    const std::vector<OriginalUiTournamentPredecessorPass50KG>& passes() const noexcept{return passes_;}
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept{return bank_;}
private:
    OriginalUiTournamentPredecessorInventory50KG()=default;
    friend class rr::platform::OriginalUiTournamentDrawContribution50KG;
    friend class rr::platform::OriginalUiTournamentSession50KG;
    friend class OriginalUiRiderBackend50KG;
    const rr::platform::OriginalUiTournamentDrawContribution50KG* creator_=nullptr;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank_;
    std::uint64_t bankGeneration_=0,registryGeneration_=0,materialSequence_=0;
    std::vector<OriginalUiTournamentPredecessorPass50KG> passes_;
};
// Issued only after the SAME collector actually retires its renderer borrows,
// destroys its real graph/helpers and consumes the final material retirements.
// RiderBackend consumes the capability once, before any incoming live attach.
class OriginalUiTournamentPredecessorRetirement50KG final {
public:
    OriginalUiTournamentPredecessorRetirement50KG()=default;
    OriginalUiTournamentPredecessorRetirement50KG(const OriginalUiTournamentPredecessorRetirement50KG&)=delete;
    OriginalUiTournamentPredecessorRetirement50KG& operator=(const OriginalUiTournamentPredecessorRetirement50KG&)=delete;
    bool issued() const noexcept{return inventory_&&creator_&&!consumed_;}
private:
    friend class rr::platform::OriginalUiTournamentDrawContribution50KG;
    friend class rr::platform::OriginalUiTournamentSession50KG;
    friend class OriginalUiRiderBackend50KG;
    std::shared_ptr<const OriginalUiTournamentPredecessorInventory50KG> inventory_;
    const rr::platform::OriginalUiTournamentDrawContribution50KG* creator_=nullptr;
    std::uint64_t materialSequence_=0,submissionWatermark_=0;
    bool consumed_=false;
};
}
