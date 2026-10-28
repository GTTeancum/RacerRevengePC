#pragma once
// Only original borrowed prefix identities. World owners and nested collision
// Borrow types do not cross this declaration seam; they stay in world.cpp.
#include "terrain_interactive_session50du.h"
#include "terrain_interactive_continuation_types50du.h"
#include "native_borrowed_call50du.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/game/scene_graph49p.h"
#include "json.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <vector>
namespace interactive_loop50DU { struct GameplayDisplayWallTotals50DU; }
class ColdLoadDiagnostics50DU;
namespace checkpoint_recovery50fi_fixture { struct Plan; }
namespace cold50i { struct Mesh; }
namespace gb_pod_sound50GB { class Owner; }
namespace gb_trigger_authoring50GB { class Owner; }
namespace lap_drive50GJ { struct Continuity; }
namespace live_audio50HF { class Owner; }
namespace native_ai50IV { struct Session; }
namespace native_dynamic_contact50IW { class Registry; }
namespace native_dynamic_contact50IW { struct Snapshot; }
namespace native_gc_water_session50KG { struct Application; }
namespace native_racer_collision50HW { struct EndpointPolicy; }
namespace native_racer_collision50HW { class Owner; }
namespace original_course_targets50KX { class ColdLoadRequest; }
namespace original_course_targets50KX { class ConsumedLaunch; }
namespace original_main_entry50KG { struct Application; }
namespace original_main_entry50KG { class RaceDisplayLifetime50LA; }
namespace original_main_entry50KG { struct Video; }
namespace original_racer_manager_entity { class Owner; }
namespace original_subtype3_record { class Owner; }
namespace phase_timing50GK { struct Interval; }
namespace render_diagnostics50GQ { struct Interval; }
namespace rr::assets { struct AiAuthoringDiagnostic50IU; }
namespace rr::assets { struct FinalImpactAuthoring49Y; }
namespace rr::game { struct AiFreshCreateSelection50IU; }
namespace rr::game { struct AiRuntimeReceipt50IV; }
namespace rr::game { struct AiRuntimeState50IV; }
namespace rr::game { struct AiTrackMetadata50IU; }
namespace rr::game { struct AiTrackTuning50IU; }
namespace rr::game { struct AuthoredBinding49N; }
namespace rr::game { struct CollisionFilter48J; }
namespace rr::game { class CollisionNormalModeOwner50JA; }
namespace rr::game { struct ControlPresets49D; }
namespace rr::game { struct DamageEventCursor50ID; }
namespace rr::game { struct DynamicContactEventBindings50IW; }
namespace rr::game { struct DynamicContactEventCursor50IW; }
namespace rr::game { struct DynamicContactMusicCursor50IW; }
namespace rr::game { struct DynamicContactPodEffect50IW; }
namespace rr::game { struct DynamicContactProfile50IW; }
namespace rr::game { struct DynamicImpactProfile50IW; }
namespace rr::game { struct DynamicImpactTuning50IW; }
namespace rr::game { struct HostMonotonicClock50ID; }
namespace rr::game { struct LapEventCursor50ID; }
namespace rr::game { struct MainBodyState49K; }
namespace rr::game { class OriginalRacerModelRenderGlobals50LA; }
namespace rr::game { struct RaceActorResolver50ID; }
namespace rr::game { struct RaceAudioSettings50IC; }
namespace rr::game { struct SharedRandomOwner50ID; }
namespace rr::game { struct Type2ResourceView49X; }
namespace rr::game { struct Type2StaticCandidate49X; }
namespace rr::game { struct Type2StaticQuery49X; }
namespace rr::game { struct VehicleFrameState49W; }
namespace rr::game::original_racer_selection50IJ { struct Entry; }
namespace rr::platform { class GameHost50ER; }
namespace rr::platform { struct GameHostDiagnostic50ER; }
namespace rr::platform { struct HostTrackAdmission50IM; }
namespace rr::platform { class OriginalSoundMode50KG; }
namespace rr::platform { struct SelectedControlInput50KY; }
namespace rr::platform { struct VehicleWindowDiagnostic50DS; }
namespace runtime_startup50fl { struct Fixture; }
namespace tb_pod_sound50TB { class Owner; }
namespace tb_trigger_authoring50TB { class Owner; }
namespace track_session_assets50IG { struct SessionAssets; }
namespace windows_capture50LD { class Run; }
namespace interactive_world50DU {
namespace f=runtime_startup50fl;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
using J=nlohmann::json;
struct Context final {
    using allAiControllerSamplesType = std::uint64_t;
    allAiControllerSamplesType& allAiControllerSamples;
    using allAiEnabledType = const bool;
    allAiEnabledType& allAiEnabled;
    using allAiHostDriverCallsType = std::uint64_t;
    allAiHostDriverCallsType& allAiHostDriverCalls;
    using allAiKeyboardPacketsType = std::uint64_t;
    allAiKeyboardPacketsType& allAiKeyboardPackets;
    using applicationClockType = rr::game::HostMonotonicClock50ID;
    applicationClockType& applicationClock;
    using cameraStartup350Type = const std::int32_t;
    cameraStartup350Type& cameraStartup350;
    using captureFirstWrongWayType = const bool;
    captureFirstWrongWayType& captureFirstWrongWay;
    using capturePathType = const char*;
    capturePathType& capturePath;
    using captureRunType = windows_capture50LD::Run;
    captureRunType& captureRun;
    using capturedFirstWrongWayType = bool;
    capturedFirstWrongWayType& capturedFirstWrongWay;
    using checkpointInputType = const checkpoint_recovery50fi_fixture::Plan*;
    checkpointInputType& checkpointInput;
    using checkpointOutputType = checkpoint_recovery50fi_fixture::Plan;
    checkpointOutputType& checkpointOutput;
    using chosenType = const nlohmann::json*;
    chosenType& chosen;
    using coldLoadDiagnosticsType = ColdLoadDiagnostics50DU;
    coldLoadDiagnosticsType& coldLoadDiagnostics;
    using collisionNormalModeType = rr::game::CollisionNormalModeOwner50JA;
    collisionNormalModeType& collisionNormalMode;
    using consumedLaunchType = original_course_targets50KX::ConsumedLaunch*;
    consumedLaunchType& consumedLaunch;
    using controlPresetsType = rr::game::ControlPresets49D;
    controlPresetsType& controlPresets;
    using corpusType = nlohmann::json;
    corpusType& corpus;
    using courseColdLoadType = original_course_targets50KX::ColdLoadRequest;
    courseColdLoadType& courseColdLoad;
    using courseRaceColdType = const bool;
    courseRaceColdType& courseRaceCold;
    using courseRaceLaunchedType = bool;
    courseRaceLaunchedType& courseRaceLaunched;
    using deviceIndexType = const std::uint32_t;
    deviceIndexType& deviceIndex;
    using directOriginalHumanType = const bool;
    directOriginalHumanType& directOriginalHuman;
    using directOriginalInputsType = const bool;
    directOriginalInputsType& directOriginalInputs;
    using displayWallTotalsType = GameplayDisplayWallTotals50DU;
    displayWallTotalsType& displayWallTotals;
    using dynamicActorResolverType = const rr::game::RaceActorResolver50ID;
    dynamicActorResolverType& dynamicActorResolver;
    using dynamicActorsType = interactive_continuation50DU::DynamicActors50IW;
    dynamicActorsType& dynamicActors;
    using dynamicConsumerFlags11050Type = const std::uint32_t;
    dynamicConsumerFlags11050Type& dynamicConsumerFlags11050;
    using dynamicContactEnabledType = const bool;
    dynamicContactEnabledType& dynamicContactEnabled;
    using dynamicDamageCursorType = rr::game::DamageEventCursor50ID;
    dynamicDamageCursorType& dynamicDamageCursor;
    using dynamicEventBindingsType = rr::game::DynamicContactEventBindings50IW;
    dynamicEventBindingsType& dynamicEventBindings;
    using dynamicEventCursorType = rr::game::DynamicContactEventCursor50IW;
    dynamicEventCursorType& dynamicEventCursor;
    using dynamicEventsType = rr::game::RaceEventQueue50ID<128>;
    dynamicEventsType& dynamicEvents;
    using dynamicFinalTuningType = rr::assets::FinalImpactAuthoring49Y;
    dynamicFinalTuningType& dynamicFinalTuning;
    using dynamicLapCursorType = rr::game::LapEventCursor50ID;
    dynamicLapCursorType& dynamicLapCursor;
    using dynamicMusicCursorType = rr::game::DynamicContactMusicCursor50IW;
    dynamicMusicCursorType& dynamicMusicCursor;
    using dynamicMusicProfileType = rr::game::DynamicContactProfile50IW;
    dynamicMusicProfileType& dynamicMusicProfile;
    using dynamicPeersType = native_dynamic_contact50IW::Snapshot;
    dynamicPeersType& dynamicPeers;
    using dynamicPodEffectType = rr::game::DynamicContactPodEffect50IW;
    dynamicPodEffectType& dynamicPodEffect;
    using dynamicProfileType = rr::game::DynamicImpactProfile50IW;
    dynamicProfileType& dynamicProfile;
    using dynamicRacer60Type = std::array<std::uint32_t,rr::game::defaultHostRaceRacers50HT>;
    dynamicRacer60Type& dynamicRacer60;
    using dynamicRegistryType = native_dynamic_contact50IW::Registry;
    dynamicRegistryType& dynamicRegistry;
    using dynamicSelfSlotType = std::size_t;
    dynamicSelfSlotType& dynamicSelfSlot;
    using dynamicTuningType = rr::game::DynamicImpactTuning50IW;
    dynamicTuningType& dynamicTuning;
    using endpointPolicyType = const native_racer_collision50HW::EndpointPolicy;
    endpointPolicyType& endpointPolicy;
    using gaOriginalDiscType = const bool;
    gaOriginalDiscType& gaOriginalDisc;
    using gbLiveAudioPreparedType = bool;
    gbLiveAudioPreparedType& gbLiveAudioPrepared;
    using gbOriginalDiscType = const bool;
    gbOriginalDiscType& gbOriginalDisc;
    using gbPodSoundType = std::shared_ptr<gb_pod_sound50GB::Owner>;
    gbPodSoundType& gbPodSound;
    using gbPreparedAudioRootType = std::filesystem::path;
    gbPreparedAudioRootType& gbPreparedAudioRoot;
    using gbSoundModeType = std::shared_ptr<rr::platform::OriginalSoundMode50KG>;
    gbSoundModeType& gbSoundMode;
    using gcNativeGateType = const bool;
    gcNativeGateType& gcNativeGate;
    using gcNativeWorldSelectedType = const bool;
    gcNativeWorldSelectedType& gcNativeWorldSelected;
    using gcOriginalDiscType = const bool;
    gcOriginalDiscType& gcOriginalDisc;
    using headlessLiveType = bool;
    headlessLiveType& headlessLive;
    using hostType = rr::platform::GameHost50ER;
    hostType& host;
    using hostDiagnosticType = rr::platform::GameHostDiagnostic50ER;
    hostDiagnosticType& hostDiagnostic;
    using humanControlInputType = rr::platform::SelectedControlInput50KY;
    humanControlInputType& humanControlInput;
    using humanCourseEnabledType = const bool;
    humanCourseEnabledType& humanCourseEnabled;
    using isGaTrackType = const bool;
    isGaTrackType& isGaTrack;
    using isGbTrackType = const bool;
    isGbTrackType& isGbTrack;
    using isGcTrackType = const bool;
    isGcTrackType& isGcTrack;
    using isMaSaTrackType = const bool;
    isMaSaTrackType& isMaSaTrack;
    using isMaTrackType = const bool;
    isMaTrackType& isMaTrack;
    using isMbTrackType = const bool;
    isMbTrackType& isMbTrack;
    using isMcTrackType = const bool;
    isMcTrackType& isMcTrack;
    using isRaTrackType = const bool;
    isRaTrackType& isRaTrack;
    using isRcTrackType = const bool;
    isRcTrackType& isRcTrack;
    using isSaTrackType = const bool;
    isSaTrackType& isSaTrack;
    using isScTrackType = const bool;
    isScTrackType& isScTrack;
    using isTaTrackType = const bool;
    isTaTrackType& isTaTrack;
    using isTbTrackType = const bool;
    isTbTrackType& isTbTrack;
    using isTcTrackType = const bool;
    isTcTrackType& isTcTrack;
    using lapContinuityType = lap_drive50GJ::Continuity;
    lapContinuityType& lapContinuity;
    using lastCommittedWrongWayType = bool;
    lastCommittedWrongWayType& lastCommittedWrongWay;
    using legacyCandidatesType = std::vector<rr::game::Type2StaticCandidate49X>;
    legacyCandidatesType& legacyCandidates;
    using liveAudioType = std::unique_ptr<live_audio50HF::Owner>;
    liveAudioType& liveAudio;
    using loadedAudioSettingsType = const rr::game::RaceAudioSettings50IC;
    loadedAudioSettingsType& loadedAudioSettings;
    using loadedTrackType = const rr::platform::HostTrack50IM;
    loadedTrackType& loadedTrack;
    using mType = rr::game::MainBodyState49K;
    mType& m;
    using maOriginalDiscType = const bool;
    maOriginalDiscType& maOriginalDisc;
    using mainQueryResourceType = const rr::game::Type2ResourceView49X;
    mainQueryResourceType& mainQueryResource;
    using managerRuntimeSelectionType = rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection;
    managerRuntimeSelectionType& managerRuntimeSelection;
    using mbOriginalDiscType = const bool;
    mbOriginalDiscType& mbOriginalDisc;
    using mcOriginalDiscType = const bool;
    mcOriginalDiscType& mcOriginalDisc;
    using meshesType = std::vector<cold50i::Mesh>;
    meshesType& meshes;
    using mode1EnabledType = bool;
    mode1EnabledType& mode1Enabled;
    using mode1RequestedType = const bool;
    mode1RequestedType& mode1Requested;
    using monotonicNow50DUType = std::uint64_t(*)() noexcept;
    monotonicNow50DUType monotonicNow50DU;
    using namesType = std::vector<std::string>;
    namesType& names;
    using nativeGaSelectionType = const bool;
    nativeGaSelectionType& nativeGaSelection;
    using nativeGbSelectionType = const bool;
    nativeGbSelectionType& nativeGbSelection;
    using nativeMaSelectionType = const bool;
    nativeMaSelectionType& nativeMaSelection;
    using nativeMbSelectionType = const bool;
    nativeMbSelectionType& nativeMbSelection;
    using nativeMcSelectionType = const bool;
    nativeMcSelectionType& nativeMcSelection;
    using nativeOrdinaryLaunch11258Type = std::uint32_t;
    nativeOrdinaryLaunch11258Type& nativeOrdinaryLaunch11258;
    using nativeOriginalFleetType = const bool;
    nativeOriginalFleetType& nativeOriginalFleet;
    using nativeRaSelectionType = const bool;
    nativeRaSelectionType& nativeRaSelection;
    using nativeRacerManagerType = std::unique_ptr<original_racer_manager_entity::Owner>;
    nativeRacerManagerType& nativeRacerManager;
    using nativeRcSelectionType = const bool;
    nativeRcSelectionType& nativeRcSelection;
    using nativeRecordActorType = std::unique_ptr<original_subtype3_record::Owner>;
    nativeRecordActorType& nativeRecordActor;
    using nativeSaSelectionType = const bool;
    nativeSaSelectionType& nativeSaSelection;
    using nativeScSelectionType = const bool;
    nativeScSelectionType& nativeScSelection;
    using nativeSelectedLightsType = const bool;
    nativeSelectedLightsType& nativeSelectedLights;
    using nativeTaSelectionType = const bool;
    nativeTaSelectionType& nativeTaSelection;
    using nativeTbSelectionType = const bool;
    nativeTbSelectionType& nativeTbSelection;
    using nativeTcSelectionType = const bool;
    nativeTcSelectionType& nativeTcSelection;
    using nativeVideoType = std::shared_ptr<original_main_entry50KG::Video>;
    nativeVideoType& nativeVideo;
    using nativeWaterApplicationType = native_gc_water_session50KG::Application;
    nativeWaterApplicationType& nativeWaterApplication;
    using nativeWaterEnabledType = const bool;
    nativeWaterEnabledType& nativeWaterEnabled;
    using nativeWaterGlobalsType = const std::shared_ptr<native_gc_water_session50KG::Application>;
    nativeWaterGlobalsType& nativeWaterGlobals;
    BorrowedCall<void()> openNativeVideoCall;
    using originalAiType = rr::game::AiRuntimeState50IV;
    originalAiType& originalAi;
    using originalAiAuthoringDiagnosticType = rr::assets::AiAuthoringDiagnostic50IU;
    originalAiAuthoringDiagnosticType& originalAiAuthoringDiagnostic;
    using originalAiCreateSelectionType = const rr::game::AiFreshCreateSelection50IU;
    originalAiCreateSelectionType& originalAiCreateSelection;
    using originalAiEnabledType = const bool;
    originalAiEnabledType& originalAiEnabled;
    using originalAiHumanCountType = const std::uint32_t;
    originalAiHumanCountType& originalAiHumanCount;
    using originalAiMapMetadataType = rr::game::AiTrackMetadata50IU;
    originalAiMapMetadataType& originalAiMapMetadata;
    using originalAiMetadataType = rr::game::AiTrackMetadata50IU;
    originalAiMetadataType& originalAiMetadata;
    using originalAiOpponentCountType = const std::uint32_t;
    originalAiOpponentCountType& originalAiOpponentCount;
    BorrowedCall<bool(std::size_t) noexcept> originalAiOwnsSlotCall;
    using originalAiReceiptType = rr::game::AiRuntimeReceipt50IV;
    originalAiReceiptType& originalAiReceipt;
    using originalAiSessionType = native_ai50IV::Session;
    originalAiSessionType& originalAiSession;
    using originalAiSettings1125CType = const std::uint16_t;
    originalAiSettings1125CType& originalAiSettings1125C;
    using originalAiTuningType = rr::game::AiTrackTuning50IU;
    originalAiTuningType& originalAiTuning;
    using originalBootstrapCollisionType = native_racer_collision50HW::Owner;
    originalBootstrapCollisionType& originalBootstrapCollision;
    using originalCameraEnabledType = const bool;
    originalCameraEnabledType& originalCameraEnabled;
    using originalMainApplicationType = const original_main_entry50KG::Application;
    originalMainApplicationType& originalMainApplication;
    using originalRecoveryEnabledType = const bool;
    originalRecoveryEnabledType& originalRecoveryEnabled;
    using originalUiActiveType = bool;
    originalUiActiveType& originalUiActive;
    using originalUiStartedType = const bool;
    originalUiStartedType& originalUiStarted;
    using phaseTimingType = phase_timing50GK::Interval;
    phaseTimingType& phaseTiming;
    using playerActionIndexFFCType = const std::uint32_t;
    playerActionIndexFFCType& playerActionIndexFFC;
    using playerArchiveType = const std::filesystem::path;
    playerArchiveType& playerArchive;
    using playerPodIdType = const std::string;
    playerPodIdType& playerPodId;
    using playerRacer60Type = const std::uint32_t;
    playerRacer60Type& playerRacer60;
    using playerSource5CType = const std::uint32_t;
    playerSource5CType& playerSource5C;
    using playerUsesRosterRigType = const bool;
    playerUsesRosterRigType& playerUsesRosterRig;
    BorrowedCall<void(std::shared_ptr<const gb_trigger_authoring50GB::Owner>)> prepareGbPodSoundCall;
    BorrowedCall<void(std::shared_ptr<const tb_trigger_authoring50TB::Owner>)> prepareTbPodSoundCall;
    using qfilterType = rr::game::CollisionFilter48J;
    qfilterType& qfilter;
    using queryType = rr::game::Type2StaticQuery49X;
    queryType& query;
    using raceEnabledType = const bool;
    raceEnabledType& raceEnabled;
    using raceOpponentCountType = const std::uint32_t;
    raceOpponentCountType& raceOpponentCount;
    using raceRacerCountType = const std::uint32_t;
    raceRacerCountType& raceRacerCount;
    using racerContactsEnabledType = const bool;
    racerContactsEnabledType& racerContactsEnabled;
    using rcOriginalDiscType = const bool;
    rcOriginalDiscType& rcOriginalDisc;
    using renderDiagnosticsType = render_diagnostics50GQ::Interval;
    renderDiagnosticsType& renderDiagnostics;
    using reportHost50DUType = void(*)(const rr::platform::GameHostDiagnostic50ER&);
    reportHost50DUType reportHost50DU;
    using reportRenderer50DUType = void(*)(const rr::platform::VehicleWindowDiagnostic50DS&);
    reportRenderer50DUType reportRenderer50DU;
    using requestedWindowSamplesType = const int;
    requestedWindowSamplesType& requestedWindowSamples;
    using sType = rr::game::VehicleFrameState49W;
    sType& s;
    using saOriginalDiscType = const bool;
    saOriginalDiscType& saOriginalDisc;
    using scOriginalDiscType = const bool;
    scOriginalDiscType& scOriginalDisc;
    using selectedAudioSettingsType = rr::game::RaceAudioSettings50IC;
    selectedAudioSettingsType& selectedAudioSettings;
    using selectedOriginalType = const rr::game::original_racer_selection50IJ::Entry*;
    selectedOriginalType& selectedOriginal;
    using selectedRacer60Type = std::uint32_t;
    selectedRacer60Type& selectedRacer60;
    using selectedTrackType = rr::platform::HostTrack50IM;
    selectedTrackType& selectedTrack;
    using sessionIndexType = std::uint64_t;
    sessionIndexType& sessionIndex;
    using sessionRandomType = rr::game::SharedRandomOwner50ID;
    sessionRandomType& sessionRandom;
    using sourceConsumedIntroActivationType = const bool;
    sourceConsumedIntroActivationType& sourceConsumedIntroActivation;
    using sourceConsumedPauseActivationType = const bool;
    sourceConsumedPauseActivationType& sourceConsumedPauseActivation;
    using sourceDisplayLifetimeType = std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA>;
    sourceDisplayLifetimeType& sourceDisplayLifetime;
    using sourceModelRenderGlobalsType = std::unique_ptr<OriginalRacerModelRenderGlobals50LA>;
    sourceModelRenderGlobalsType& sourceModelRenderGlobals;
    using sourceStaticCandidatesType = const bool;
    sourceStaticCandidatesType& sourceStaticCandidates;
    using stackCountType = std::size_t;
    stackCountType& stackCount;
    using startAtMenuType = bool;
    startAtMenuType& startAtMenu;
    using taOriginalDiscType = const bool;
    taOriginalDiscType& taOriginalDisc;
    using tbLiveAudioPreparedType = bool;
    tbLiveAudioPreparedType& tbLiveAudioPrepared;
    using tbOriginalDiscType = const bool;
    tbOriginalDiscType& tbOriginalDisc;
    using tbPodSoundType = std::shared_ptr<tb_pod_sound50TB::Owner>;
    tbPodSoundType& tbPodSound;
    using tbPreparedAudioRootType = std::filesystem::path;
    tbPreparedAudioRootType& tbPreparedAudioRoot;
    using tbSoundModeType = std::shared_ptr<rr::platform::OriginalSoundMode50KG>;
    tbSoundModeType& tbSoundMode;
    using tcOriginalDiscType = const bool;
    tcOriginalDiscType& tcOriginalDisc;
    using tournamentLaunchSelectionType = rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection;
    tournamentLaunchSelectionType& tournamentLaunchSelection;
    using tournamentLaunchStampType = rr::platform::OriginalUiRiderProfileStamp50KG;
    tournamentLaunchStampType& tournamentLaunchStamp;
    using tournamentResultArmedType = bool;
    tournamentResultArmedType& tournamentResultArmed;
    using tournamentResultCommittedType = bool;
    tournamentResultCommittedType& tournamentResultCommitted;
    using trackAdmissionType = const rr::platform::HostTrackAdmission50IM;
    trackAdmissionType& trackAdmission;
    using trackSessionType = const track_session_assets50IG::SessionAssets*;
    trackSessionType& trackSession;
    using usePhysicalGameControllerType = const bool;
    usePhysicalGameControllerType& usePhysicalGameController;
    using xType = runtime_startup50fl::Fixture;
    xType& x;
    SessionExit50DU run();
};
}
