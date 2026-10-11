#pragma once
#include "original_ui_rider_lighting50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_racer_light_selection50km.h"
#include <cstdint>

namespace rr::game {
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
// The recovered Main/Options/Rider population has no cache installation path.
//
// This bounded native owner carries that actual constructor-absent state across
// the tracked UI flow. It must be created at native application startup, before
// first Main, and retained until application teardown. It rejects unknown or
// already-running histories; entering gameplay or another unjoined manager
// MUST call invalidateForUntrackedScene before leaving the tracked scope.
// Never construct a new instance when opening Rider or returning from gameplay
// to manufacture absence. A future shared cache implementation can replace
// this bounded owner once its real creation/counter/invalidation path is joined.
class OriginalUiLightManagerLifetime50KG final {
public:
    OriginalUiLightManagerLifetime50KG() noexcept=default;
    OriginalUiLightManagerLifetime50KG(const OriginalUiLightManagerLifetime50KG&)=delete;
    OriginalUiLightManagerLifetime50KG& operator=(const OriginalUiLightManagerLifetime50KG&)=delete;
    bool constructAtApplicationStartup(OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    // Actual UI manager entry, not cold resource preparation. First manager
    // must be authenticated Main. Later entries admit Main/Options/Rider only;
    // release previous manager after its packets/lights are drained.
    bool beginUiManager(const assets::OriginalUiArchive50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    //1CCB04->148110 frees per-zone data. Deliberately does NOT reset cacheA4.
    bool releaseUiManager() noexcept;
    // Actual Rider1CDB10/148F30 population after zone148B60 setup, followed by
    // current146C40/149900 color changes. First call installs the two original
    // rows in Ambient,Directional order; later calls update those same rows.
    bool publishRiderRegistry(const OriginalUiRiderLighting50KG&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) noexcept;
    //14A0E8 tests this shared owner's real absentA4 state BEFORE entering
    //149BB0. No invented cache miss, per-frame cache bypass or global-row
    // substitute. Small registry returns directly14A154 and never inserts.
    bool selectRiderCharacter(const assets::OriginalUiRiderCharacterAssets50KG&,
        const OriginalUiRiderModelSelection50KG&,
        original_racer_lighting50KM::LightMatrices&,
        OriginalUiLightManagerLifetimeDiagnostic50KG&) const noexcept;
    void invalidateForUntrackedScene() noexcept;
    void destroyApplication() noexcept;
    OriginalUiLightCacheState50KG cacheState() const noexcept {return cacheState_;}
    std::uint64_t managerGeneration() const noexcept {return managerGeneration_;}
    // Read-only scope identity for the application coordinator's generation
    // lease. It cannot create, reset or substitute a cache/lifetime.
    const assets::OriginalUiArchiveIdentity50KG* activeUiIdentity() const noexcept {return activeUi_;}
    bool riderRegistryReady() const noexcept {return riderRegistryReady_;}
private:
    OriginalUiLightCacheState50KG cacheState_=OriginalUiLightCacheState50KG::notConstructed;
    const assets::OriginalUiArchiveIdentity50KG* activeUi_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* riderUiIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* riderWidgetIdentity_=nullptr;
    original_racer_lighting50KM::SmallDirectionalRegistry riderRegistry_{};
    std::uint64_t managerGeneration_=0;
    bool enteredFirstMain_=false,riderRegistryReady_=false;
};
}
