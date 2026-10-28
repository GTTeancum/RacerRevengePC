#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
namespace original_main_entry50KG { struct Application; }
namespace ra_cold_roster50LA { class Operation; }
namespace ra_cold_aggregate50LA { struct Prefix; class Owner; }
namespace rr::assets { class OriginalStandardMaterialDomain50LA; }
namespace rr::game { class OriginalApplicationEntities50LA; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalResults10450LA; }
namespace original_results_roster50LA {
// Synchronous HOST ownership adaptation of precisely the Results loader's
// manager reads. Never a B0 publication or1AD520/class-construction receipt.
// Its pin is the existing Bank-retained RA Operation, not a copied roster.
class Read final {
    friend struct ::original_main_entry50KG::Application;
    friend class rr::game::OriginalResults10450LA;
    const original_main_entry50KG::Application* application_;
    const rr::game::OriginalRaceConstructor50LA* race_;
    std::shared_ptr<ra_cold_roster50LA::Operation> operation_;
    // Only the private synchronous human caller supplies the actual session
    // identity array; null preserves the existing ALL8 contract exactly.
    const std::array<std::uint32_t,8>* humanRacer60_=nullptr;
    std::uint64_t humanProfileGeneration_=0;
    Read(const original_main_entry50KG::Application&,
        const rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ra_cold_roster50LA::Operation>)noexcept;
    Read(const original_main_entry50KG::Application&,
        const rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ra_cold_roster50LA::Operation>,
        const std::array<std::uint32_t,8>*)noexcept;
    static ra_cold_aggregate50LA::Prefix completedWorldPrefixes(
        const rr::game::OriginalRaceConstructor50LA&)noexcept;
    // Private synchronous phases. References bind actual owner fields; no
    // retained snapshot/authority object. Throwing work still reaches current's
    // original catch; Diagnostic and retained handoff live through the final check.
    static ra_cold_aggregate50LA::Prefix completedOwnerPrefixes(
        const std::shared_ptr<ra_cold_aggregate50LA::Owner>&)noexcept;
    bool applicationCurrent(const std::shared_ptr<ra_cold_aggregate50LA::Owner>&,
        const original_main_entry50KG::Application* const&,
        const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>&,
        const std::uint64_t&,
        const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>&)const;
    bool humanProfileCurrent(const std::uint32_t&,const std::uint32_t&)const;
    bool launchEntrantsDiffer(const std::size_t&)const;
    bool finalHumanProfileChanged()const;
    bool currentBank(std::size_t&)const;
    bool current(std::size_t& count)const noexcept;
    bool racer60At1AF330(std::int32_t index,std::uint32_t& actual60)const noexcept;
public:
    Read(const Read&)=delete;Read& operator=(const Read&)=delete;
    ~Read();
};
}
