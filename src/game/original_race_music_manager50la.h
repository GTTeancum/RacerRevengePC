#pragma once
#include <cstdint>
#include <memory>
namespace original_camera_session50KX { class Session; }
namespace original_main_entry50KG { struct Application; }
namespace original_race_hud_scope50LA { class Registration; class Scope; }
namespace rr::platform { class OriginalApplicationAural50KG; struct AudioOutputDiagnostic50HD; }
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct OriginalRaceMusicManagerDiagnostic50LA {
    std::uint32_t sourcePC=0;
    const char* operation=nullptr;
};
// One actual counted1A39F0 manager. Native occurrences borrow128/12C/130
// from their SAME existing music owner. The distinct source occurrence owns
// those lanes itself and cannot adopt native Scope/PCM or listener readiness.
// Native platform child IO remains an explicit adapter, NOT full1A3C00.
class OriginalRaceMusicManager50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalRaceConstructor50LA;
    friend class OriginalResults10450LA;
    friend class ::original_race_hud_scope50LA::Scope;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalRaceMusicManager50LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t,
        const std::shared_ptr<::original_race_hud_scope50LA::Registration>&);
    // Distinct original-source occurrence. No native Scope, PCM, mixer state,
    // listener or prepared Announcer is adopted by this constructor.
    OriginalRaceMusicManager50LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    static bool beginSourceAfterLoadingCamera1A6C90(const ::original_main_entry50KG::Application&,
        OriginalRaceConstructor50LA&,const std::shared_ptr<platform::OriginalApplicationAural50KG>&,
        std::uint32_t&)noexcept;
    bool loadSourceMusicChild1A3C00(const ::original_main_entry50KG::Application&,
        OriginalRaceConstructor50LA&,const std::shared_ptr<platform::OriginalApplicationAural50KG>&,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool openSourceMusicConfig2C0B98(const ::original_main_entry50KG::Application&,
        OriginalRaceConstructor50LA&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool loadSourceMusicMetadata2C0BE4(const ::original_main_entry50KG::Application&,
        OriginalRaceConstructor50LA&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool formatSourceSong2C0C44(std::uint32_t ordinal,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool retireSourceMusicChildNative(OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool constructed()const noexcept;
    static bool noNativePreparation(const ::original_race_hud_scope50LA::Registration&)noexcept;
    static bool freshAll8NativePrefix(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&)noexcept;
    bool stageNativeLoadPrefix(OriginalRaceConstructor50LA&,
        const std::shared_ptr<platform::OriginalApplicationAural50KG>&,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool acceptNativeChildren(OriginalRaceConstructor50LA&,
        const std::shared_ptr<platform::OriginalApplicationAural50KG>&,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool advanceNativeListenerConsumedStep(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,
        const ::original_camera_session50KX::Session&,std::uint64_t beforeMix,
        bool heldSourceScene,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool serviceNativeMenuAudio(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,std::uint32_t elapsedMicroseconds,
        platform::AudioOutputDiagnostic50HD&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool nativeBackendCurrent(const ::original_race_hud_scope50LA::Registration&)const noexcept;
    bool invokeNativeContactSpeech1(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool invokeNativeLowDamageSpeech5(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool restartNativeFade1AC960(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    enum class PauseReturnCall : std::uint32_t { main1BD9F0=0x1BD9F0,course1BDB88=0x1BDB88 };
    bool pauseReturnNativeFade1A4AB0(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,PauseReturnCall,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
    bool readQueuedCount150(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,std::uint32_t&)const noexcept;
    bool dispatchNativeQueuedEvent3_1A4D30(const OriginalRaceConstructor50LA&,
        const ::original_race_hud_scope50LA::Registration&,std::uint32_t savedInitialCount,
        OriginalRaceMusicManagerDiagnostic50LA&)noexcept;
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
