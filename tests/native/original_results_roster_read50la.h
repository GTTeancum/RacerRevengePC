#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
namespace original_position_stats_adapter50LA { class Owner; }
namespace original_camera_session50KX { class Session; }
namespace rr::assets { class OriginalStandardMaterialTable50LA; }
namespace rr::platform { struct OriginalUiMainProfile50KG; class OriginalPersistedProfileName50KG; }
namespace rr::game { struct LapRecordQueryResult50ID; }
namespace original_main_entry50KG { struct Application; }
namespace ra_cold_roster50LA { class Operation; }
namespace native_racer50HN { class Bank; }
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
    struct Record {
        const native_racer50HN::Bank* bank=nullptr;
        const rr::game::OriginalApplicationEntities50LA* application=nullptr;
        const rr::assets::OriginalStandardMaterialTable50LA* materials=nullptr;
        std::uint64_t generation=0,publication=0;
        std::size_t managerIndex=0;
        std::uint32_t vehicle58=0,racer60=0;
    };
    static bool captureRecord(const original_position_stats_adapter50LA::Owner&,
        const rr::game::OriginalApplicationEntities50LA*,const rr::assets::OriginalStandardMaterialTable50LA*,
        std::uint64_t,std::size_t,Record&)noexcept;
    static bool recordCurrent(const original_position_stats_adapter50LA::Owner&,const Record&)noexcept;
    static bool readBestLap(const original_position_stats_adapter50LA::Owner&,const Record&,float&)noexcept;
    static bool readRaceTime(const original_position_stats_adapter50LA::Owner&,const Record&,float&)noexcept;
    static bool readObservedTiming(const original_position_stats_adapter50LA::Owner&,const Record&,std::uint32_t&)noexcept;
    static bool profileNameCourse(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)noexcept;
    static bool profileFirstName(const rr::game::OriginalRaceConstructor50LA&,rr::platform::OriginalPersistedProfileName50KG&)noexcept;
    static rr::game::LapRecordQueryResult50ID profileLapQuery(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t,float)noexcept;
    static rr::game::LapRecordQueryResult50ID profileRaceQuery(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t,float)noexcept;
    static rr::game::LapRecordQueryResult50ID profileKnockoutQuery(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t,std::uint32_t)noexcept;
    struct RaceAccess;
    struct RootAccess;
    static RaceAccess raceAccess(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    static RootAccess rootAccess(const original_main_entry50KG::Application&)noexcept;
    static bool profileInitialized(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    static std::uint32_t profileMode(const rr::platform::OriginalUiMainProfile50KG&)noexcept;
    static std::uint32_t operationHumanCount(const std::shared_ptr<ra_cold_roster50LA::Operation>&)noexcept;
    //Private synchronous live Race/Profile facets. No retained snapshot or new pin.
    struct ProfileStamp {std::uint64_t generation=0;std::uint32_t mode34=0;};
    static const std::shared_ptr<ra_cold_aggregate50LA::Owner>& raceWorldIdentity(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    static ProfileStamp profileStamp(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    static const rr::platform::OriginalUiMainProfile50KG* commonProfile(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    static bool profileKnown(const rr::platform::OriginalUiMainProfile50KG&)noexcept;
    static std::uint8_t profilePlayerBits(const rr::platform::OriginalUiMainProfile50KG&)noexcept;
    static bool profileRacerName(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t,std::int32_t&)noexcept;
    //Private native projection only; original Owner nested ResultsRows ABI stays unchanged.
    struct Rows {
        struct Row {
            std::uint32_t vehicle58=0,racer60=0,retirementCD4=0;
            std::int32_t placeCCC=0,lapCC8=0;
            std::uint8_t flagsCD8=0;
            float timeCB8=0,valueCBC=0,remainingCC4=0;
        };
        std::array<Row,8> rows{};
        std::size_t count=0;
        const native_racer50HN::Bank* bank=nullptr;
        float trackLength38=0;
        std::uint64_t generation=0,publication=0;
    };
    static bool captureRows(const original_position_stats_adapter50LA::Owner&,
        const rr::game::OriginalApplicationEntities50LA*,const rr::assets::OriginalStandardMaterialTable50LA*,
        std::uint64_t,Rows&)noexcept;
    static bool rowsCurrent(const original_position_stats_adapter50LA::Owner&,
        const rr::game::OriginalApplicationEntities50LA*,const rr::assets::OriginalStandardMaterialTable50LA*,
        std::uint64_t,std::uint64_t)noexcept;
    static bool observerCameraMatches(const original_position_stats_adapter50LA::Owner&,
        const original_camera_session50KX::Session&)noexcept;
    static const native_racer50HN::Bank* observerBankIdentity(const original_position_stats_adapter50LA::Owner&)noexcept;
    static bool observerCurrent(const original_position_stats_adapter50LA::Owner&);
    static std::size_t bankCount(const native_racer50HN::Bank&)noexcept;
    //Private opaque loaded-Operation reads; preserve caller's lexical pin and
    //split observer checks between owner matching and Bank currentness.
    static bool loadedOperationOwnerFieldsCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>&,
        const std::shared_ptr<ra_cold_aggregate50LA::Owner>&,
        const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>&,
        const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>&,
        const std::uint64_t&)noexcept;
    static bool loadedOperationBankCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>&,
        const native_racer50HN::Bank*)noexcept;
    static bool loadedOperationRacerCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>&,
        std::size_t,std::uint32_t)noexcept;
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
