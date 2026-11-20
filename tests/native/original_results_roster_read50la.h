#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
namespace original_position_stats_adapter50LA { class Owner; }
namespace original_camera_session50KX { class Session; }
namespace rr::assets { class OriginalStandardMaterialTable50LA; }
namespace rr::platform { struct OriginalUiMainProfile50KG; class OriginalPersistedProfileName50KG; class OriginalUiRiderProfileOwner50KG; }
namespace rr::game { struct LapRecordQueryResult50ID; }
namespace original_main_entry50KG { struct Application; }
namespace common_roster50CR { class Owner; }
namespace native_racer50HN { class Bank; }
namespace rr::assets { class OriginalStandardMaterialDomain50LA; }
namespace rr::game { class OriginalApplicationEntities50LA; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalResults10450LA; }
namespace original_results_roster50LA {
// Synchronous HOST ownership adaptation of precisely the Results loader's
// manager reads. Never a B0 publication or1AD520/class-construction receipt.
// Its pin is the race's one roster (common_roster50CR::Owner, every course),
// borrowed synchronously by the Results loader, not a copied roster.
class Read final {
    friend struct ::original_main_entry50KG::Application;
    friend class rr::game::OriginalResults10450LA;
public:
    // Kill-credit HUD reads (225FB4..226008): the live profile's 3C[victim60]
    // Localization id for 22BA60, and 1863B0 on the profile's own 1125F course
    // for the attacker's new CD0. Read-only; no record insertion.
    static bool hudRacerName3C(const rr::platform::OriginalUiRiderProfileOwner50KG&,
        std::uint32_t vehicle60,std::int32_t& localizationId)noexcept;
    static rr::game::LapRecordQueryResult50ID hudKnockoutRecord1863B0(
        const rr::platform::OriginalUiRiderProfileOwner50KG&,std::uint32_t countCD0)noexcept;
private:
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
    static std::uint32_t rosterHumanCount(const common_roster50CR::Owner*)noexcept;
    //Private synchronous live Race/Profile facets. No retained snapshot or new pin.
    struct ProfileStamp {std::uint64_t generation=0;std::uint32_t mode34=0;};
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
    //Private roster reads (R10 defines them over common_roster50CR::Owner):
    //the loaded roster still built for this race, its slot's racer60.
    static bool loadedRosterCurrent(const common_roster50CR::Owner*,
        const rr::game::OriginalRaceConstructor50LA&,const native_racer50HN::Bank*)noexcept;
    static bool loadedRosterRacerCurrent(const common_roster50CR::Owner*,
        std::size_t,std::uint32_t)noexcept;
    const original_main_entry50KG::Application* application_;
    const rr::game::OriginalRaceConstructor50LA* race_;
    const common_roster50CR::Owner* roster_=nullptr;
    // Only the private synchronous human caller supplies the actual session
    // identity array; null preserves the existing ALL8 contract exactly.
    const std::array<std::uint32_t,8>* humanRacer60_=nullptr;
    std::uint64_t humanProfileGeneration_=0;
    Read(const original_main_entry50KG::Application&,
        const rr::game::OriginalRaceConstructor50LA&,
        const common_roster50CR::Owner&)noexcept;
    Read(const original_main_entry50KG::Application&,
        const rr::game::OriginalRaceConstructor50LA&,
        const common_roster50CR::Owner&,
        const std::array<std::uint32_t,8>*)noexcept;
    // Private synchronous phases. References bind actual owner fields; no
    // retained snapshot/authority object. Throwing work still reaches current's
    // original catch; Diagnostic and retained handoff live through the final check.
    // The race is the Application's live counted race with its one World
    // (every course); domain/context are the Application's own.
    bool applicationCurrent()const;
    bool launchEntrantsDiffer(const std::size_t&)const;
    bool current(std::size_t& count)const noexcept;
    bool racer60At1AF330(std::int32_t index,std::uint32_t& actual60)const noexcept;
public:
    Read(const Read&)=delete;Read& operator=(const Read&)=delete;
    ~Read();
};
}
