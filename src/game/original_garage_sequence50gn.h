#pragma once
// Isolated source-only Garage keyframed sequence owner. No page admission,
// rendering, timer animation surrogate, particle, sound or storage owner.
#include "original_motion_library50la.h"
#include "model_playback50kg.h"
#include "original_shared_rng50iu.h"
#include "shared_random_owner50id.h"
#include <array>
#include <cstdint>
namespace rr::platform {class OriginalUiGarageActors50GN;}
namespace rr::game {
struct OriginalGarageAnimationManagerFields50GN;
inline constexpr std::size_t garageActorCount50GN=10;
struct GarageActorDefinition50GN {
 const char* mcf;const char* referenceName;const char* sequenceName;
 std::uint8_t motionCount,entryCount;bool initiallyActive,loopSequence;
 std::array<const char*,4> motionNames;
 std::array<std::uint32_t,4> durationBits;
 std::array<std::uint8_t,14> entries,probabilities;
};
// Authenticated garage.toy and actual MOT-header values, not MCF FrameCount/FPS.
const std::array<GarageActorDefinition50GN,garageActorCount50GN>& garageActorDefinitions50GN() noexcept;
struct GarageSequenceState50GN {
 bool active90=false,differentAC=true;
 std::int32_t current9C=0,nextA0=-1,currentA4=0,nextA8=-1;
 float elapsedB0=0;
};
struct GarageActorBinding50GN {
 // Stable borrowed owners must outlive this sequencer and all service calls.
 // Resource sets hold actual MotionLibrary references; playback descriptors
 // correspond one-to-one to those real resource views, never duration mocks.
 const OriginalModelMotionSet50LA* resources=nullptr;
 std::array<const ModelPlaybackMotion50KG*,4> motions{};
 ModelPlayback50KG* playback=nullptr;
 const assets::MotionNodeBindings49O* rows=nullptr;
 SceneGraph49P graph{};
 const ModelPlaybackIk50KG* ik=nullptr;
};
enum class GarageSequenceError50GN : std::uint8_t {none,input,alreadyBound,resource,playback,clock,range,postSample,state};
struct GarageSequenceDiagnostic50GN {
 GarageSequenceError50GN error=GarageSequenceError50GN::none;
 std::uint32_t sourcePC=0,actor=0;
 ModelPlaybackDiagnostic50KG playback{};
};
struct GarageSequenceActorReceipt50GN {bool started=false,transitioned=false,sampled=false;std::int32_t currentEntry=0,nextEntry=-1;};
struct GarageSequenceReceipt50GN {
 bool paused=false,seeded=false,drew=false;
 std::uint32_t draw308030=0,rawAfter=0,seedCache=0;
 std::int32_t percentile=0;float delta30=0;
 std::array<GarageSequenceActorReceipt50GN,garageActorCount50GN> actors{};
};
using GarageReadTimestamp50GN=bool(*)(void*,std::uint32_t&)noexcept;
// Required continuation AFTER genuine154480, BEFORE the next actor. Enclosing
// owner must implement actual11AD80 graph/zone refresh then10179C..101818 sounds.
// This callback does not grant page admission, and cannot change the sequencer
// or playback records. A numeric checker may record this seam, not impersonate
// a production sound/render owner. False preserves reached prefix and latches
// failure; do not replay that tick or its one shared RNG draw.
using GarageAfterSample50GN=bool(*)(void*,std::size_t,const GarageActorBinding50GN&,
 GarageSequenceDiagnostic50GN&)noexcept;
class OriginalGarageSequence50GN final {
 friend class rr::platform::OriginalUiGarageActors50GN;
 std::array<GarageActorBinding50GN,garageActorCount50GN> bindings_{};
 std::array<GarageSequenceState50GN,garageActorCount50GN> states_{};
 std::size_t initialPoseCount_=0;
 bool bound_=false,failed_=false,servicing_=false;
 bool fail(GarageSequenceDiagnostic50GN&,GarageSequenceError50GN,std::uint32_t,std::size_t=0)noexcept;
 // The concrete retained owner alone supplies its real manager. Publication
 // occurs at1019A8 before any later actor failure, never after service returns.
 bool serviceManager101860(OriginalGarageAnimationManagerFields50GN&,bool,float,
  const std::array<bool,garageActorCount50GN>&,GarageReadTimestamp50GN,void*,
  CoupledNoiseContext49M&,SharedRandomOwner50ID&,ModelMotionSharedState49T&,
  ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,GarageAfterSample50GN,void*,
  GarageSequenceReceipt50GN&,GarageSequenceDiagnostic50GN&)noexcept;
 bool serviceImpl(OriginalGarageAnimationManagerFields50GN*,bool,float,float,
  const std::array<bool,garageActorCount50GN>&,GarageReadTimestamp50GN,void*,
  CoupledNoiseContext49M&,SharedRandomOwner50ID&,ModelMotionSharedState49T&,
  ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,GarageAfterSample50GN,void*,
  GarageSequenceReceipt50GN&,GarageSequenceDiagnostic50GN&)noexcept;
public:
 OriginalGarageSequence50GN()noexcept=default;
 OriginalGarageSequence50GN(const OriginalGarageSequence50GN&)=delete;
 OriginalGarageSequence50GN& operator=(const OriginalGarageSequence50GN&)=delete;
 // Performs source2D946C/2D9480/2D9490 initial pose/start/dt0/stop in actor
 // order. Loading, model placement, real VUT binding and zone registration
 // must already have happened; no invented graph or rest-pose fallback.
 bool bind(const std::array<GarageActorBinding50GN,garageActorCount50GN>&,
  ModelMotionSharedState49T&,ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,
  GarageSequenceDiagnostic50GN&)noexcept;
 // Interleaved genuine construction route: bind the NEXT authored actor only,
 // then enclosing owner performs28EA20 and original sound-row setup BEFORE
 // constructing the next actor. No service/trigger readiness until all ten.
 bool bindNextInitialPose(std::size_t,const GarageActorBinding50GN&,
  ModelMotionSharedState49T&,ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,
  GarageSequenceDiagnostic50GN&)noexcept;
 // Exact101C10 named trigger for reached five RUN/FALL actors. Missing actor
 // or sequence rejects before any mutation, rather than source unsafe index.
 bool trigger(const char* actor,const char* sequence,GarageSequenceDiagnostic50GN&)noexcept;
 // Input Entity enabled lanes must come from the actual actor Entity owners.
 // Shared noise/lazy fields and workspace are borrowed at application lifetime,
 // serial/disjoint. No per-manager RNG copy or unpaused actor draw is allowed.
 bool service(bool paused,float dt,float scale34,
  const std::array<bool,garageActorCount50GN>& enabled,
  GarageReadTimestamp50GN,void* clockContext,CoupledNoiseContext49M&,SharedRandomOwner50ID&,
  ModelMotionSharedState49T&,ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,
  GarageAfterSample50GN,void* afterContext,GarageSequenceReceipt50GN&,GarageSequenceDiagnostic50GN&)noexcept;
 const GarageSequenceState50GN& state(std::size_t i)const noexcept{return states_[i];}
 bool ready()const noexcept{return bound_&&!failed_;}
};
}
