#pragma once
#include <cstdint>
#include <array>
#include <optional>
#include <cstddef>
#include <memory>
#include <thread>
namespace interactive_continuation50DU { struct Context; }
namespace interactive_publication50DU { struct Context; }
namespace original_camera_session50KX { class Session; struct Result; }
namespace race_music50IE { class Owner; }
namespace source_announcer50IE { class Owner; }
namespace live_audio50HF { class Owner; }
namespace native_original_ai_audio { class Owner; }
namespace race_audio50HQ { class Owner; }
namespace native_racer50HN { class Bank; }
namespace native_ai50IV { struct Session; }
namespace original_main_entry50KG { struct Application; }
namespace rr::platform { class GameHost50ER; }
namespace rr::game {class OriginalResults10450LA; template<std::size_t> struct RaceEventQueue50ID; struct DynamicContactEventBindings50IW; struct DynamicContactEventCursor50IW; struct RaceEvent50ID; struct LapEventCursor50ID; struct LapEventRequest50ID; struct LapEventResult50ID;struct PodSpeechCounts50HG;struct PodSpeechState50HG;}
namespace rr::game { class OriginalRaceMusicManager50LA; struct OriginalResults104Diagnostic50LA; class OriginalRaceConstructor50LA;class OriginalRaceHud50KG;struct OriginalRaceEndEntryDiagnostic50LA;struct OriginalHumanEndClockResult50KG; }

namespace original_race_hud_scope50LA {
class Scope;
// Narrow synchronous borrow of the ACTUAL setup HUD/manager and ending audio,
// not a generic callback, end receipt, renderer lease or substitute owners.
class Registration final {
    friend class Scope;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::game::OriginalRaceMusicManager50LA;
    friend class rr::game::OriginalResults10450LA;
    friend struct ::original_main_entry50KG::Application;
    friend struct ::interactive_publication50DU::Context;
    const std::thread::id thread_;
    const original_main_entry50KG::Application* const application_;
    const std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    const native_racer50HN::Bank* const bank_;
    const native_ai50IV::Session* const manager_; //Same actual Context-owned manager, no copy.
    const std::uint64_t generation_;
    rr::game::OriginalRaceHud50KG* hud_;
    // The same setup-owned providers used by Loop; lexical revoke precedes
    // their close. Never substitute inactivity when either owner is missing.
    race_music50IE::Owner* music_;
    source_announcer50IE::Owner* announcer_;
    live_audio50HF::Owner* gameplay_;
    native_original_ai_audio::Owner* engines_;
    bool active_=true;
    bool attempted_=false,published_=false;
    const original_camera_session50KX::Session* camera_=nullptr;
    Registration(const original_main_entry50KG::Application&,
        const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>&,
        const native_racer50HN::Bank&,const native_ai50IV::Session&,rr::game::OriginalRaceHud50KG&,
        race_music50IE::Owner*,source_announcer50IE::Owner*,
        live_audio50HF::Owner*,native_original_ai_audio::Owner*,race_audio50HQ::Owner*)noexcept;
    rr::game::RaceEventQueue50ID<128>* events_=nullptr;
    const rr::game::DynamicContactEventBindings50IW* eventBindings_=nullptr;
    const std::uint32_t* lapCC8_=nullptr; //Actual registered Context lane, not actor snapshot.
    bool readNativeListenerScene(const rr::game::OriginalRaceConstructor50LA&,
        const original_camera_session50KX::Session&,bool heldSourceScene,
        std::uint64_t previousPublication,std::uint64_t& publication,
        std::array<const float*,9>& actualXYZ)const noexcept;
    bool prepareNativeContactEvent1(const rr::game::OriginalRaceConstructor50LA&,
        const rr::game::RaceEvent50ID&,rr::game::DynamicContactEventCursor50IW&,
        std::array<std::optional<std::uint32_t>,18>&,rr::game::LapEventRequest50ID&,
        bool&,bool&,bool&,std::uint32_t&,std::uint32_t&)const noexcept;
    bool finishNativeContactEvent1(const rr::game::OriginalRaceConstructor50LA&,
        rr::game::DynamicContactEventCursor50IW&,
        std::array<std::optional<std::uint32_t>,18>&,rr::game::LapEventRequest50ID&,
        bool&,std::uint32_t&)const noexcept;
    bool readNativePodSpeechOwner(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t actor,
        const rr::game::PodSpeechCounts50HG*&,rr::game::PodSpeechState50HG*&,
        std::array<std::uint32_t,3>&,std::array<std::uint32_t,4>&,
        std::array<std::optional<std::uint32_t>,16>&,std::uint32_t&)const noexcept;
    bool playNativePodSpeech(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t actor,
        bool taunt,std::uint32_t selectedOrdinal,std::uint32_t&)const noexcept;
    bool readNativeDamageSpeech5Owner(const rr::game::OriginalRaceConstructor50LA&,
        const rr::game::RaceEvent50ID&,const rr::game::PodSpeechCounts50HG*&,
        rr::game::PodSpeechState50HG*&,std::array<std::uint32_t,3>&,
        std::array<std::uint32_t,4>&,std::array<std::optional<std::uint32_t>,16>&,
        std::uint32_t&)const noexcept;
    bool playNativeDamageSpeech5(const rr::game::OriginalRaceConstructor50LA&,
        std::uint32_t selectedOrdinal,std::uint32_t&)const noexcept;
    bool prepareNativeDamageEvent5(const rr::game::OriginalRaceConstructor50LA&,
        const rr::game::RaceEvent50ID&,std::array<std::optional<std::uint32_t>,7>&,
        rr::game::LapEventRequest50ID&,bool&,bool&,std::uint32_t&)const noexcept;
    bool beginNativeLapEvent6(const rr::game::OriginalRaceConstructor50LA&,std::uint32_t,
        rr::game::LapEventCursor50ID&,std::uint32_t&)const noexcept;
    rr::game::LapEventResult50ID advanceNativeLapEvent6(const rr::game::OriginalRaceConstructor50LA&,
        rr::game::LapEventCursor50ID&,std::uint32_t&)const noexcept;
    bool invokeNativeLapEvent6(const rr::game::OriginalRaceConstructor50LA&,
        const rr::game::LapEventRequest50ID&,std::uint32_t&)const noexcept;

    race_audio50HQ::Owner* countdown_=nullptr; // Exact lexical Context.raceAudio, revoked before close.
    bool endStateCurrent(const rr::game::OriginalRaceConstructor50LA&,
        const original_camera_session50KX::Session&)const noexcept;
    rr::game::OriginalHumanEndClockResult50KG advanceEndClock1A7DA0(
        rr::game::OriginalRaceConstructor50LA&,original_camera_session50KX::Session&,
        float actual23C,std::uint64_t actualRootPublication)noexcept;
    original_camera_session50KX::Result advanceFadeIn1A7BD0(
        rr::game::OriginalRaceConstructor50LA&,original_camera_session50KX::Session&)noexcept;
    //Only the actual owning Race may consume this. It independently borrows
    //its current world's Camera and checks the SAME Bank/HUD/viewport rows.
    bool readEndAssociationHead1A8074(const rr::game::OriginalRaceConstructor50LA&,bool&)const noexcept;
    bool readEndAssociatedCount1A80AC(const rr::game::OriginalRaceConstructor50LA&,std::int32_t&)const noexcept;
    bool stop1AD230(const rr::game::OriginalRaceConstructor50LA&)noexcept;
    //Specific private continuation using the same actual Camera/Bank/Manager.
    //Not a callback executor, query receipt consumer or live end admission.
    void continuePreAck1AB520(rr::game::OriginalRaceConstructor50LA&,
        rr::game::OriginalRaceEndEntryDiagnostic50LA&)noexcept;
    bool pressResults1C8C30(rr::game::OriginalRaceConstructor50LA&,
        rr::platform::GameHost50ER&,rr::game::OriginalResults104Diagnostic50LA&)noexcept;
    //Called only immediately after actual Publication's successful existing
    //construct1A8CC4. No public row setter or loaded-boolean inference.
    static void observeConstructed1A8CC4(::interactive_publication50DU::Context&)noexcept;
public:
    Registration(const Registration&)=delete;
    Registration& operator=(const Registration&)=delete;
    ~Registration()=default;
};
class Scope final {
    friend struct ::interactive_continuation50DU::Context;
    std::shared_ptr<Registration> registration_;
    explicit Scope(::interactive_continuation50DU::Context&,const native_racer50HN::Bank&)noexcept;
    void revoke()noexcept;
public:
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
    Scope(Scope&&)=delete;Scope& operator=(Scope&&)=delete;
    ~Scope()noexcept;
};
}
