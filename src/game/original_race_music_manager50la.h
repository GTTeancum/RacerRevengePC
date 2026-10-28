#pragma once
#include <cstdint>
#include <memory>
namespace original_main_entry50KG { struct Application; }
namespace original_race_hud_scope50LA { class Registration; class Scope; }
namespace rr::platform { class OriginalApplicationAural50KG; }
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct OriginalRaceMusicManagerDiagnostic50LA {
    std::uint32_t sourcePC=0;
    const char* operation=nullptr;
};
// One actual counted1A39F0 manager. Its128/12C/130 lanes are references into
// the SAME existing native music owner; no second mode or PCM/voice transport.
// Platform child IO is an explicit native adapter, NOT full source1A3C00.
class OriginalRaceMusicManager50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalRaceConstructor50LA;
    friend class OriginalResults10450LA;
    friend class ::original_race_hud_scope50LA::Scope;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalRaceMusicManager50LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t,
        const std::shared_ptr<::original_race_hud_scope50LA::Registration>&);
    bool constructed()const noexcept;
    static bool noNativePreparation(const ::original_race_hud_scope50LA::Registration&)noexcept;
    bool acceptNativeChildren(OriginalRaceConstructor50LA&,
        const std::shared_ptr<const platform::OriginalApplicationAural50KG>&,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool nativeBackendCurrent(const ::original_race_hud_scope50LA::Registration&)const noexcept;
    bool readQueuedCount150(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,std::uint32_t&)const noexcept;
    bool clearQueuedCount1A5C40(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&)noexcept;
    void revokeNativeProviders(const ::original_race_hud_scope50LA::Registration&)noexcept;
    bool retireNativeDetached(OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
public:
    OriginalRaceMusicManager50LA(const OriginalRaceMusicManager50LA&)=delete;
    OriginalRaceMusicManager50LA& operator=(const OriginalRaceMusicManager50LA&)=delete;
    ~OriginalRaceMusicManager50LA()noexcept;
};
}
