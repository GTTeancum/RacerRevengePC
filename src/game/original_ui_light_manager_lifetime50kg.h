#pragma once
#include "original_ui_rider_lighting50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_racer_light_selection50km.h"
#include <cstdint>

namespace rr::platform {class OriginalUiLightManagerOwner50KG;}
namespace rr::game {
struct OriginalUiCourseLighting50KG;
struct OriginalUiTournamentLighting50KG;
struct OriginalUiGarageLighting50KG;
struct OriginalUiControlsLighting50KG;
struct OriginalUiControlsModelSelection50KG;
enum class OriginalUiLightManagerLifetimeError50KG : std::uint8_t {
    none,phase,archive,untrackedLifetime,registry,membership,arithmetic
};
struct OriginalUiLightManagerLifetimeDiagnostic50KG {
    OriginalUiLightManagerLifetimeError50KG error=OriginalUiLightManagerLifetimeError50KG::none;
    std::uint32_t sourcePC=0;
};
enum class OriginalUiLightCacheState50KG : std::uint8_t {
    notConstructed,absentSinceApplicationConstruction,untracked,destroyed
};
// Shared APPLICATION lifetime, not a Rider-local convenience owner. Source
//140084 constructs147ED0 once and attaches the LightManager with entity mask0402.
//147FF8 initializesA4=null.148110 UI teardown and148B60 zone setup preserveA4.
// The recovered Main/Options/Rider/Course population has no cache installation path.
//
// This bounded native owner carries that actual constructor-absent state across
// the tracked UI flow. It must be created at native application startup, before
// first Main, and retained until application teardown. It rejects unknown or
// already-running histories. Retail gameplay or another unjoined manager
// MUST call invalidateForUntrackedScene. The explicitly separate native race
// lighting owner may borrow a dormant application scope through the checked
// begin/endNativeGameplay methods; it cannot install or query this cache.
// Never construct a new instance when opening Rider or returning from gameplay
// to manufacture absence. A future shared cache implementation can replace
// this bounded owner once its real creation/counter/invalidation path is joined.
class OriginalUiLightManagerLifetime50KG final {
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    // Installed only by the Root-issued concrete constructor. Game-only builds
    // keep the original scalar path without linking a platform implementation.
    // The callback authenticates this exact scalar and commits native projection
    // leases; it never constructs counted source zones or UI/Scene Light rows.
    enum class NativeProjectionOperation : std::uint8_t {
        observe,beginUi,endUi,beginGameplay,endGameplay
    };
    using NativeProjectionBridge=bool (*)(rr::platform::OriginalUiLightManagerOwner50KG*,
        const OriginalUiLightManagerLifetime50KG&,NativeProjectionOperation,
        const assets::OriginalUiArchiveIdentity50KG*,std::uint64_t)noexcept;
    rr::platform::OriginalUiLightManagerOwner50KG* concreteBorrower_=nullptr;
    NativeProjectionBridge concreteProjection_=nullptr;
    // Final source retirement cannot fall back into a new unbound scalar era.
    bool concreteProjectionRetired_=false;
    bool nativeProjectionStep(NativeProjectionOperation,
        const assets::OriginalUiArchiveIdentity50KG* =nullptr,std::uint64_t=0)const noexcept;
    bool nativeProjectionCurrent()const noexcept {
        return nativeProjectionStep(NativeProjectionOperation::observe);
    }
public:
    OriginalUiLightManagerLifetime50KG() noexcept=default;
    OriginalUiLightManagerLifetime50KG(const OriginalUiLightManagerLifetime50KG&)=delete;
    OriginalUiLightManagerLifetime50KG& operator=(const OriginalUiLightManagerLifetime50KG&)=delete;
    bool constructAtApplicationStartup(OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    // Native UI projection entry, not cold resource preparation. First manager
    // must be authenticated Main; release each lease after its packets drain.
    // A concrete singleton verifies this scalar's same-owner receipt without
    // constructing source zones or Light rows. Concrete Garage stays separate.
    bool beginUiManager(const assets::OriginalUiArchive50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    // Native projection release follows1CCB04->148110's lifetime boundary.
    // It releases no concrete source zone and deliberately does NOT resetA4.
    bool releaseUiManager() noexcept;
    // Native projection of Rider1CDB10/148F30 and146C40/149900 colors.
    // Ambient,Directional order is preserved in scalar rendering rows; this
    // never substitutes for concrete counted source Entity registration.
    bool publishRiderRegistry(const OriginalUiRiderLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    bool publishTournamentRegistry(const OriginalUiTournamentLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    // Current exact registry observation; does not publish or renew ownership.
    bool matchesTournamentRegistry(const OriginalUiTournamentLighting50KG&) const noexcept;
    // Legacy scalar-only Garage rendering projection. A retained concrete
    // Light owner requires actual Garage UI/Scene rows and will not admit this
    // scalar Garage lease as a substitute for272A48/273AA0 source ownership.
    bool publishGarageRegistry(const OriginalUiGarageLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    bool matchesGarageRegistry(const OriginalUiGarageLighting50KG&) const noexcept;
    bool publishCourseRegistry(const OriginalUiCourseLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    // Actual Options background272A48 zone reset then277FCC SCN population.
    // Same application cache lifetime and real selected background zone.
    bool publishControlsRegistry(const OriginalUiControlsLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    bool selectControlsModel(const OriginalUiControlsModelSelection50KG&,
        original_racer_lighting50KM::LightMatrices&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) const noexcept;
    //14A0E8 tests this shared owner's real absentA4 state BEFORE entering
    //149BB0. No invented cache miss, per-frame cache bypass or global-row
    // substitute. Small registry returns directly14A154 and never inserts.
    bool selectRiderCharacter(const assets::OriginalUiRiderCharacterAssets50KG&,
        const OriginalUiRiderModelSelection50KG&,
        original_racer_lighting50KM::LightMatrices&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) const noexcept;
    // Native integration policy only: the complete native race uses a separate
    // canonical lighting manager with no borrow of this application cacheA4.
    // Begin after actual UI-manager release; end only after the full native
    // race call has returned and destroyed its world/renderer/lighting owners.
    // These methods never reset cache state and never admit untracked history.
    bool beginNativeGameplay(std::uint64_t raceGeneration,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    bool endNativeGameplay(std::uint64_t raceGeneration,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    std::uint64_t nativeGameplayGeneration()const noexcept{return nativeGameplayGeneration_;}
    void invalidateForUntrackedScene() noexcept;
    void destroyApplication() noexcept;
    OriginalUiLightCacheState50KG cacheState() const noexcept {return cacheState_;}
    std::uint64_t managerGeneration() const noexcept {return managerGeneration_;}
    // Read-only scope identity for the application coordinator's generation
    // lease. It cannot create, reset or substitute a cache/lifetime.
    const assets::OriginalUiArchiveIdentity50KG* activeUiIdentity() const noexcept {return activeUi_;}
    bool riderRegistryReady() const noexcept {return nativeProjectionCurrent()&&riderRegistryReady_;}
    bool controlsRegistryReady()const noexcept{return nativeProjectionCurrent()&&controlsRegistryReady_;}
    bool courseRegistryReady() const noexcept {return nativeProjectionCurrent()&&courseRegistryReady_;}
private:
    OriginalUiLightCacheState50KG cacheState_=OriginalUiLightCacheState50KG::notConstructed;
    const assets::OriginalUiArchiveIdentity50KG* activeUi_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* riderUiIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* riderWidgetIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry riderRegistry_{};
    const assets::OriginalUiMemberIdentity50KG* tournamentUiIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry tournamentRegistry_{};
    std::array<float,4> tournamentAmbient50_{},tournamentDirectional3C_{};
    bool tournamentRegistryReady_=false;
    const assets::OriginalUiMemberIdentity50KG* garageUiIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* garageSceneIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry garageRegistry_{};
    std::array<float,4> garageAmbient50_{},garageDirectional3C_{};
    bool garageRegistryReady_=false;
    const assets::OriginalUiMemberIdentity50KG* courseUiIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry courseRegistry_{};
    const assets::OriginalUiMemberIdentity50KG* controlsUiIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* controlsSceneIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* controlsWidgetIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry controlsRegistry_{};
    bool controlsRegistryReady_=false;
    std::uint64_t managerGeneration_=0,nativeGameplayGeneration_=0;
    bool enteredFirstMain_=false,riderRegistryReady_=false,courseRegistryReady_=false;
};
}
