#pragma once
// W0 common rewrite: the one track, world, stage, lap, roster, start and end owners.
#include "common_track50cr.h"
#include "common_race_world50cr.h"
#include "original_consumed_stage50du.h"
#include "common_lap50cr.h"
#include "common_roster50cr.h"
#include "common_race_start50cr.h"
#include "common_race_end50cr.h"
#include "common_race_log50cr.h"
#include "course_trigger_sources50ct.h"
#include "terrain_interactive_cold_types50du.h"
#include "native_borrowed_call50du.h"

namespace interactive_cold50DU {
namespace platform=rr::platform;
using namespace rr::game;
// Synchronous references to original root locals; no resource/callback ownership.
struct ContactsContext final {
    // W0 common rewrite: one track of this race (common_track50cr.h): code, files, counts, role owners
    using trackType = const common_track50CR::Track;
    trackType& track;
    // W0 common rewrite: one roster (common_roster50cr.h)
    using rosterType = common_roster50CR::Owner;
    rosterType& roster;
    using allAiEnabledType = const bool;
    allAiEnabledType& allAiEnabled;
    using applicationClockType = rr::game::HostMonotonicClock50ID;
    applicationClockType& applicationClock;
    using audioEnabledType = const bool;
    audioEnabledType& audioEnabled;
    using completedFramesType = std::uint64_t;
    completedFramesType& completedFrames;
    using contactBindingsType = std::array<native_racer_contact50HW::Binding,rr::game::defaultHostRaceRacers50HT>;
    contactBindingsType& contactBindings;
    using rootedCourseRaceType = const bool;
    rootedCourseRaceType& rootedCourseRace;
    using dynamicContactEnabledType = const bool;
    dynamicContactEnabledType& dynamicContactEnabled;
    using dynamicRacer60Type = std::array<std::uint32_t,rr::game::defaultHostRaceRacers50HT>;
    dynamicRacer60Type& dynamicRacer60;
    using dynamicRegistryType = native_dynamic_contact50IW::Registry;
    dynamicRegistryType& dynamicRegistry;
    using trackAnimationsType = std::shared_ptr<track_animation_runtime50AN::Owner>;
    trackAnimationsType& trackAnimations;
    using triggerAudioType = std::unique_ptr<gc_gate_audio50IN::Owner>;
    triggerAudioType& triggerAudio;
    using graphicsSequenceType = std::array<std::uint64_t,rr::game::defaultHostRaceRacers50HT>;
    graphicsSequenceType& graphicsSequence;
    using lightDiagnosticType = rr::game::original_gc_gameplay_lighting_candidate::Diagnostic;
    lightDiagnosticType& lightDiagnostic;
    using humanCourseEnabledType = const bool;
    humanCourseEnabledType& humanCourseEnabled;
    using initialCollisionZoneType = const rr::game::CollectionRef50BX;
    initialCollisionZoneType& initialCollisionZone;
    using liveWorldType = rr::game::LiveZoneWorld50EN;
    liveWorldType& liveWorld;
    using mType = rr::game::MainBodyState49K;
    mType& m;
    using mainMembershipType = rr::game::LiveZoneMembership50EN;
    mainMembershipType& mainMembership;
    using aiViewType = const common_track50CR::NavigationView;
    aiViewType& aiView;
    using coldNavigationBoundType = bool;
    coldNavigationBoundType& coldNavigationBound;
    using membershipWorkType = rr::game::LiveZoneWorkspace50EN;
    membershipWorkType& membershipWork;
    using nativeOriginalFleetType = const bool;
    nativeOriginalFleetType& nativeOriginalFleet;
    using nativeSelectedLightsType = const bool;
    nativeSelectedLightsType& nativeSelectedLights;
    using navigationProgressType = rr::game::TrackNavigationProgress50EU;
    navigationProgressType& navigationProgress;
    using opponentDrawType = std::array<native_racer50HN::DrawOwner,maxHostRaceOpponents50HT>;
    opponentDrawType& opponentDraw;
    using opponentUvsType = std::array<std::vector<platform::VehicleWindowMaterialUv50DS>,maxHostRaceOpponents50HT>;
    opponentUvsType& opponentUvs;
    using originalAiEnabledType = const bool;
    originalAiEnabledType& originalAiEnabled;
    using originalAiHumanCountType = const std::uint32_t;
    originalAiHumanCountType& originalAiHumanCount;
    using originalAiOpponentCountType = const std::uint32_t;
    originalAiOpponentCountType& originalAiOpponentCount;
    using originalRecoveryEnabledType = const bool;
    originalRecoveryEnabledType& originalRecoveryEnabled;
    interactive_frame50DU::BorrowedCall<bool()> publishRecoveryQueries;
    using qfilterType = rr::game::CollisionFilter48J;
    qfilterType& qfilter;
    using queryType = rr::game::Type2StaticQuery49X;
    queryType& query;
    using raceOpponentCountType = const std::uint32_t;
    raceOpponentCountType& raceOpponentCount;
    using raceRacerCountType = const std::uint32_t;
    raceRacerCountType& raceRacerCount;
    using racerBankType = native_racer50HN::Bank;
    racerBankType& racerBank;
    using racerContactIdsType = std::array<std::uint32_t,native_racer_contact50HW::maxRacers>;
    racerContactIdsType& racerContactIds;
    using racerContactOwnerType = std::unique_ptr<native_racer_contact50HW::Owner>;
    racerContactOwnerType& racerContactOwner;
    using racerContactsEnabledType = const bool;
    racerContactsEnabledType& racerContactsEnabled;
    using racerRigsType = std::array<std::unique_ptr<native_racer50HW::Rig>,8>;
    racerRigsType& racerRigs;
    using racerVisualsType = std::array<std::unique_ptr<native_racer50HW::Visual>,8>;
    racerVisualsType& racerVisuals;
    interactive_frame50DU::BorrowedCall<undertrack_owners50ew_fixture::BorrowedQueries()> recoveryBorrowedQueries;
    using recoveryQueryOwnersType = undertrack_owners50ew_fixture::Owners;
    recoveryQueryOwnersType& recoveryQueryOwners;
    interactive_frame50DU::BorrowedCall<void()> requireRuntimeNavigation;
    using sType = rr::game::VehicleFrameState49W;
    sType& s;
    using secondaryFilterType = rr::game::CollisionFilter48J;
    secondaryFilterType& secondaryFilter;
    using secondaryMembershipType = rr::game::LiveZoneMembership50EN;
    secondaryMembershipType& secondaryMembership;
    using secondaryQueryType = rr::game::Type2StaticQuery49X;
    secondaryQueryType& secondaryQuery;
    using selectedOriginalType = const rr::game::original_racer_selection50IJ::Entry*;
    selectedOriginalType& selectedOriginal;
    using sessionRandomType = rr::game::SharedRandomOwner50ID;
    sessionRandomType& sessionRandom;
    using sourceAnnouncerType = std::unique_ptr<source_announcer50IE::Owner>;
    sourceAnnouncerType& sourceAnnouncer;
    using mapViewType = const common_track50CR::NavigationView;
    mapViewType& mapView;
    using trackCountsType = const track_runtime_storage50IG::Counts;
    trackCountsType& trackCounts;
    using trackStorageType = track_runtime_storage50IG::Storage;
    trackStorageType& trackStorage;
    using xType = runtime_startup50fl::Fixture;
    xType& x;
    using zoneLifecycleType = rr::game::ZoneLifecycle50CL;
    zoneLifecycleType& zoneLifecycle;
    void run();
};
} // namespace interactive_cold50DU
