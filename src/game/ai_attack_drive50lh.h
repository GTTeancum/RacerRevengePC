#pragma once
#include "ai_combat_candidate50lh.h"

namespace rr::game {
// Process-lifetime owner of source171240..171250. The guarded initialization
// is reached only after B9C.1 is cleared and both authored widths are read.
struct AiAttackDriveGlobals50LH {
    float widthThreshold433CC8=0.f;
    bool widthThresholdInitialized=false; // gp-7AAC source guard byte.
};
struct AiAttackDriveSelection50LH {
    bool wideSteering16EDD0=false;
    bool attackSpeed16FB50=false;
    float currentNodeWidth=0.f;
};
// Source1711E0 prefix. Canonical B9C and104C are explicit references because
// their writes precede the steering/speed leaves and survive a later boundary.
// No mode is synthesized: every mode other than3/4 follows the real172C50
// branch. Runtime stateFE8=1 admission belongs to the source dispatcher.
template<class Navigation>
inline bool beginAiAttackDrive1711E0Shared50MB(const Navigation& source_track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiReady(source_track))
        return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);

    const auto& track=navigation_consumer_detail50MB::read(source_track);
    using ai_driving_detail50IV::fail;selection={};diagnostic={};
    flagsB9C=static_cast<std::uint8_t>(flagsB9C&0xfdu); //171210, unconditionally.
    if(!track.track.count||track.track.count>navigation_consumer_detail50MB::capacity(track)||currentNode>=track.track.count)
        return fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    if(std::fegetround()!=FE_TONEAREST)
        return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x17123cu,currentNode);
    ai_prefix_detail50IV::Math f;
    selection.currentNodeWidth=f.add(track.nodes[currentNode].left54,track.nodes[currentNode].right58);
    if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x17123cu,currentNode);
    if(!globals.widthThresholdInitialized){
        globals.widthThreshold433CC8=250.f; //171248
        globals.widthThresholdInitialized=true; //171250
    }
    if(mode1050==3u){
        const float threshold=f.read(globals.widthThreshold433CC8);
        if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x171268u,currentNode);
        selection.wideSteering16EDD0=!(selection.currentNodeWidth<=threshold);
        selection.attackSpeed16FB50=true;
    }else{
        field104C=-1.f; //1712C0 mode4,1712E8 every other mode.
        selection.attackSpeed16FB50=mode1050==4u;
    }
    return true;
}
inline bool beginAiAttackDrive1711E0(const TrackNavigation50EU& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
inline bool beginAiAttackDrive1711E0(const TrackNavigationView50EU& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track)) {
        selection={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    }
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
// TC-CONSUMER-BEGIN 0
inline bool beginAiAttackDrive1711E0(const TrackNavigationView50TC& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track)) {
        selection={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    }
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
inline bool beginAiAttackDrive1711E0(const TrackNavigationView50TB& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track)) {
        selection={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    }
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
// TB-CONSUMER-END 0

// GB-CONSUMER-BEGIN 0
inline bool beginAiAttackDrive1711E0(const TrackNavigationView50GB& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track)) {
        selection={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    }
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
// GB-CONSUMER-END 0

// MC-CONSUMER-BEGIN 0
inline bool beginAiAttackDrive1711E0(const TrackNavigationView50MC& track,
    std::uint32_t currentNode,std::uint32_t mode1050,std::uint8_t& flagsB9C,
    float& field104C,AiAttackDriveGlobals50LH& globals,
    AiAttackDriveSelection50LH& selection,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track)) {
        selection={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x17121cu,currentNode);
    }
    return beginAiAttackDrive1711E0Shared50MB(track,currentNode,mode1050,flagsB9C,field104C,globals,selection,diagnostic);
}
// MC-CONSUMER-END 0

struct AiAttackSpeedInput50LH {
    const TrackNavigation50EU* trackFF4=nullptr;
    const AiTrackMetadata50IU* metadata=nullptr;
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// Strong immutable MB views retain their authentic storage and final world.
struct AiAttackSpeedInput50MB {
    TrackNavigationView50EU trackFF4{};
    AiTrackMetadataView50IU metadata{};
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// TC-CONSUMER-BEGIN 1
struct AiAttackSpeedInput50TC {
    TrackNavigationView50TC trackFF4{};
    AiTrackMetadataView50TC metadata{};
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// TC-CONSUMER-END 1

// TB-CONSUMER-BEGIN 1
struct AiAttackSpeedInput50TB {
    TrackNavigationView50TB trackFF4{};
    AiTrackMetadataView50TB metadata{};
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// TB-CONSUMER-END 1

// GB-CONSUMER-BEGIN 1
struct AiAttackSpeedInput50GB {
    TrackNavigationView50GB trackFF4{};
    AiTrackMetadataView50GB metadata{};
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// GB-CONSUMER-END 1

// MC-CONSUMER-BEGIN 1
struct AiAttackSpeedInput50MC {
    TrackNavigationView50MC trackFF4{};
    AiTrackMetadataView50MC metadata{};
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,myMinSpeed108C=0;
    float preparedFactor=0.f,selfCachedSpeed34=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
// MC-CONSUMER-END 1

struct AiAttackSpeedReceipt50LH {
    float selfGateDistance=0.f,targetGateDistance=0.f,signedDistance=0.f;
    float targetForwardAcceleration=0.f,desiredSpeed=0.f;
    bool minimumApplied=false,driveFullSpeedOverride=false;
    ai_combat_distance50LH::Receipt distance{};
    AiNonBoostSpeedReceipt50IR speed{};
};
//16FB50 is a distinct attack speed planner, not172C50. It never invokes
//173180, reads heat, touches109C, or draws RNG. The sole terminal control leaf
//is173770, which clears boost bit0 and writes accelerator/brake B7C/B80.
//Input target position/forward/C0 are the real serial body/scene observations.
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50LH&) noexcept {return true;}
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50MB& in) noexcept {return navigation_consumer_detail50MB::aiJoined(in.trackFF4,in.metadata);}
// TC-CONSUMER-BEGIN 2
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50TC& in) noexcept {return navigation_consumer_detail50MB::aiJoined(in.trackFF4,in.metadata);}
// TC-CONSUMER-END 2

// TB-CONSUMER-BEGIN 2
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50TB& in) noexcept {return navigation_consumer_detail50MB::aiJoined(in.trackFF4,in.metadata);}
// TB-CONSUMER-END 2

// GB-CONSUMER-BEGIN 2
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50GB& in) noexcept {return navigation_consumer_detail50MB::aiJoined(in.trackFF4,in.metadata);}
// GB-CONSUMER-END 2

// MC-CONSUMER-BEGIN 2
inline bool applyAiAttackSpeed16FB50Admission50MB(const AiAttackSpeedInput50MC& in) noexcept {return navigation_consumer_detail50MB::aiJoined(in.trackFF4,in.metadata);}
// MC-CONSUMER-END 2

template<class Input>
    requires (std::is_same_v<Input,AiAttackSpeedInput50LH>||std::is_same_v<Input,AiAttackSpeedInput50MB>||std::is_same_v<Input,AiAttackSpeedInput50TC>||std::is_same_v<Input,AiAttackSpeedInput50TB>||std::is_same_v<Input,AiAttackSpeedInput50GB>||std::is_same_v<Input,AiAttackSpeedInput50MC>)
inline bool applyAiAttackSpeed16FB50Shared50MB(const Input& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!applyAiAttackSpeed16FB50Admission50MB(input)) {receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x1706c0u);}

    const auto* trackFF4=navigation_consumer_detail50MB::pointer(input.trackFF4);
    const auto* metadata=navigation_consumer_detail50MB::pointer(input.metadata);
    using ai_driving_detail50IV::fail;diagnostic={};speedDiagnostic={};receipt={};
    if(std::fegetround()!=FE_TONEAREST)
        return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x16fb50u);
    if(!trackFF4||!metadata||!navigation_consumer_detail50MB::read(*metadata).bound||
       navigation_consumer_detail50MB::read(*metadata).count!=navigation_consumer_detail50MB::read(*trackFF4).track.count||!input.target||
       !input.target->bound||!input.selfPosition190)
        return fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    const auto& track=*trackFF4;const auto& target=*input.target;
    if(!ai_combat_candidate_detail50LH::gateDistanceShared50MB(track,input.selfNodeC80,
        input.selfPosition190,receipt.selfGateDistance,diagnostic)){
        if(diagnostic.sourcePC==0x1706c0u)diagnostic.sourcePC=0x16fd44u;return false;
    }
    if(!ai_combat_candidate_detail50LH::gateDistanceShared50MB(track,target.vehicleC80,
        target.position190,receipt.targetGateDistance,diagnostic)){
        if(diagnostic.sourcePC==0x1706c0u)diagnostic.sourcePC=0x16fed8u;return false;
    }
    ai_prefix_detail50IV::Math f;
    receipt.signedDistance=f.sub(receipt.selfGateDistance,receipt.targetGateDistance); //16FEEC delay slot.
    if(input.selfNodeC80!=target.vehicleC80){
        float correction=0.f;
        if(!ai_combat_distance50LH::distanceBetweenNodes2A2DC0(*metadata,
            target.vehicleC80,target.lapCC8,input.selfNodeC80,input.selfLapCC8,
            correction,receipt.distance,input.diagnostic,input.diagnosticContext))
            return fail(diagnostic,receipt.distance.error==ai_combat_distance50LH::Error::Owner?
                AiDrivingError50IV::Owner:AiDrivingError50IV::Arithmetic,receipt.distance.sourcePC);
        receipt.signedDistance=f.add(receipt.signedDistance,correction); //16FF00.
    }
    // Scalar ADDAs/MADD keep this exact grouping; the VU dot has its own
    // accumulator. Do not replace the actual acceleration by speed deltas.
    const float scalarAccumulator=f.add(2.f,receipt.signedDistance); //16FF14.
    receipt.targetForwardAcceleration=f.dot(target.linearAccelerationC0,target.forward190); //16FF24..40.
    const float accelerationStep=f.mul(receipt.targetForwardAcceleration,input.preparedFactor); //16FF48.
    const float feedback=f.add(scalarAccumulator,f.mul(10.f,accelerationStep)); //16FF60.
    const float predicted=f.add(target.cachedSpeed34,feedback); //16FF64.
    const float minimum=f.integer(input.myMinSpeed108C); //16FF68.
    float desired=f.mul(ai_speed_detail50IR::word(0x3f2e8bc1u),predicted); //16FF6C.
    if(desired<minimum){desired=minimum;receipt.minimumApplied=true;}
    if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x16ff6cu);
    //Gate distance already proved self node in bounds before either accessor.
    if(navigation_consumer_detail50MB::read(*metadata).nodes[input.selfNodeC80].flags79&0x10u){
        desired=f.read(navigation_consumer_detail50MB::read(*metadata).nodes[input.selfNodeC80].speedLimit5C);
        receipt.driveFullSpeedOverride=true;
    }
    if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x16ffa0u);
    receipt.desiredSpeed=desired;
    if(!applyAiNonBoostSpeed50IR(desired,input.selfCachedSpeed34,actions,actions,
        receipt.speed,speedDiagnostic))
        return fail(diagnostic,AiDrivingError50IV::Arithmetic,speedDiagnostic.sourcePC);
    return true;
}
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50LH& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50MB& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(input.trackFF4,input.metadata)) {
        receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    }
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
// TC-CONSUMER-BEGIN 3
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50TC& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(input.trackFF4,input.metadata)) {
        receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    }
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
// TC-CONSUMER-END 3

// TB-CONSUMER-BEGIN 3
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50TB& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(input.trackFF4,input.metadata)) {
        receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    }
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
// TB-CONSUMER-END 3

// GB-CONSUMER-BEGIN 3
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50GB& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(input.trackFF4,input.metadata)) {
        receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    }
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
// GB-CONSUMER-END 3

// MC-CONSUMER-BEGIN 3
inline bool applyAiAttackSpeed16FB50(const AiAttackSpeedInput50MC& input,
    PlayerReadResult48X& actions,AiAttackSpeedReceipt50LH& receipt,
    AiDrivingDiagnostic50IV& diagnostic,AiSpeedDiagnostic50IR& speedDiagnostic) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(input.trackFF4,input.metadata)) {
        receipt={};speedDiagnostic={};return ai_driving_detail50IV::fail(diagnostic,AiDrivingError50IV::Owner,0x16fb80u);
    }
    return applyAiAttackSpeed16FB50Shared50MB(input,actions,receipt,diagnostic,speedDiagnostic);
}
// MC-CONSUMER-END 3

} //namespace rr::game
