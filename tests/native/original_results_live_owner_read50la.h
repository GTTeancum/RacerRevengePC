#pragma once
#include "original_results_roster_read50la.h"
#include "original_ui_source_timing50lh.h"
#include "../../src/assets/original_application_localization50kg.h"
#include <optional>
#include <thread>
namespace original_main_entry50KG {struct Video;}
namespace original_results_feedback50LA {class Registration;}
namespace rr::platform {class OriginalUiRiderProfileOwner50KG;}
namespace rr::assets {class OriginalApplicationFonts50LA;}
namespace rr::game {struct OriginalWreckComponent50LA;struct OriginalResults104Diagnostic50LA;}
namespace original_results_roster50LA {
//Private lexical reference views, not snapshots or ownership/readiness receipts.
//Construction binds direct members of guaranteed-live argument objects only.
struct Read::RaceAccess {
    const rr::game::OriginalRaceConstructor50LA& actual;
    const std::thread::id& resultsFeedbackThread_;
    const std::unique_ptr<rr::game::OriginalResults10450LA>& results104_;
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& application_;
    const std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG>& profile_;
    const rr::game::OriginalWreckComponent50LA& entity_;
    const bool& counted_;const bool& rolledBack_;
    const std::uint8_t& flags1B0KnownMask_;const std::uint8_t& flags1B0KnownValue_;
    const std::weak_ptr<original_results_feedback50LA::Registration>& resultsFeedback_;
    bool managerFlags(const original_camera_session50KX::Session&,std::uint8_t&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
};
struct Read::RootAccess {
    const original_main_entry50KG::Application& actual;
    const std::thread::id& garageAudioOwnerThread_;
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entitiesOwner_;
    const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>& standardMaterialDomainOwner_;
    const std::shared_ptr<rr::assets::OriginalApplicationFonts50LA>& fonts328Owner_;
    const original_ui_source_timing50LH::PublicationView& nativeTaRootTiming_;
    const std::uint64_t& nativeTaRootPublication_;
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& sourceRace28COwner_;
    const std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG>& riderProfile;
    const rr::game::OriginalRaceConstructor50LA* const& nativeTaRootRace_;
    const std::uint64_t& nativeTaRootGeneration_;
    const bool& nativeTaRootEntered_;const bool& nativeTaRootAborted_;
    const std::int32_t& sourceApplicationMode22C_;
    const bool& sourceUpdateRenderBypass228Bit3_;
    const std::shared_ptr<original_main_entry50KG::Video>& videoOwner_;
    const std::optional<float>& sourceApplicationElapsed23C_;
    const std::shared_ptr<rr::assets::OriginalApplicationLocalization50KG>& sourceLocalization32COwner_;
    bool finalCameraCurrent(const rr::game::OriginalRaceConstructor50LA&,
        const original_camera_session50KX::Session&)const noexcept;
    bool publishedRoot(const rr::game::OriginalRaceConstructor50LA&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool borrowLocalizationText(rr::assets::OriginalApplicationLocalization50KG&,std::int32_t,
        std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText>&,
        rr::assets::OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
};
}
