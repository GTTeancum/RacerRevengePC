#pragma once
#include "track_navigation50eu.h"
#include "navigation_view50cr.h"
#include "device_held48y.h"

namespace rr::host {
// HOST-AUTHORED COURSE TEST AID. This is not the original game's AI controller.
// It reads an actual authored route and canonical body observations, and emits
// ordinary player input. It cannot mutate vehicle, contact, route or lap state.
struct CourseSpeedLimits50GK {
    float values[game::maxUndertrackNodes50ET]{};
    std::uint32_t count=0;
};
enum class CourseDriverStatus50GK : std::uint8_t {
    Driving, Disabled, Stuck, OffCourse, InvalidInput, InvalidRoute, InvalidText
};
struct CourseDriverDiagnostic50GK {
    CourseDriverStatus50GK status=CourseDriverStatus50GK::Disabled;
    std::uint32_t node=0,lookaheadSegments=0;
    std::size_t textOffset=0;
    float aim[3]{},headingError=0,turn=0,bodySpeed=0,targetSpeed=0;
    float crossTrack=0,halfWidth=0,stalledSeconds=0;
    float speedLookahead=0,steeringLookahead=0;
    float speedDemand=0,mixedDemand=0;
    std::uint32_t steeringSegments=0;
};
// Reads the actual ATA SpeedLimit entries without modifying navigation. Every
// node must have exactly one finite nonnegative limit. Uses the retained native
// decimal reader. This is a bounded metadata projection, not the AI loader.
bool readCourseSpeedLimits50GK(const char*,std::size_t,std::uint32_t nodeCount,
    CourseSpeedLimits50GK&,CourseDriverDiagnostic50GK&) noexcept;

// Bounded HOST metadata projection of the actual track TUN [AIDriving]
// Lookahead. Requires exactly one finite positive value; no invented default.
// It does not load the original AI configuration/controller. Failure preserves out.
bool readCourseSteeringLookahead50GK(const char*,std::size_t,float& out,
    CourseDriverDiagnostic50GK&) noexcept;

struct CourseDriverTuning50GK {
    // All values below are explicit HOST tuning, unrelated to Pod01.ai Skill or
    // MyMaxSpeed. Speeds and lengths use the caller's canonical body/world units.
    float cruiseSpeed=140.f,minimumCornerSpeed=50.f;
    float speedLimitToBodyScale=1.f;
    float lookaheadBase=45.f,lookaheadSeconds=.35f,maximumLookahead=150.f;
    float headingGain=1.8f,yawDamping=.65f;
    float steeringPolarity=1.f; // Exactly +/-1; verify in a real physics run.
    float speedGain=.035f,turnThrottleReduction=.7f;
    float narrowHalfWidth=70.f,offCourseMargin=120.f;
    float maximumVerticalError=450.f;
    float progressDistance=8.f,stuckSeconds=10.f;
    // Stops even when spinning/sliding keeps moving without genuine navigation
    // advancement. Must accommodate the longest authored segment at test speed.
    float maximumSecondsPerNode=40.f;
    // Opt-in authored pace: use only the minimum lookahead SpeedLimit times
    // speedLimitToBodyScale, without HOST corner/width/cruise speed caps or
    // turnThrottleReduction. Differential steering and stop checks still apply.
    bool authoredSpeedOnly=false;
    // Optional HOST steering-only preview using the actual track TUN horizon
    // and the source direction planner's additional 0.2s sample. Zero preserves
    // legacy steering. Speed preview and all safety limits stay independent.
    float authoredSteeringLookaheadSeconds=0.f;
    // Optional HOST packet mixing: preserve the requested differential while
    // positive common demand would clip an axis. This reduces common demand
    // only as required by steering saturation; target speed is not changed.
    // Zero/negative demand, manual input and default legacy mixing are unchanged.
    bool fullThrottleSteeringRange=false;
};
struct CourseDriverInput50GK {
    game::BodyMatrix48G pose{}; // canonical basis8..10 and translation12..14
    game::Vector3_48C worldVelocity{};
    float yawRate=0.f; // canonical world angular velocity about positive Y
    float elapsedSeconds=0.f; // per-step dt, not absolute time; zero initializes
    std::uint32_t previousNode=0,currentNode=0; // read-only source C84/C80
    bool enabled=false;
};
struct CourseDriverState50GK {
    bool initialized=false,stopped=false;
    CourseDriverStatus50GK stopReason=CourseDriverStatus50GK::Disabled;
    std::uint32_t observedNode=0;
    float checkpoint[3]{},stalledSeconds=0.f,nodeSeconds=0.f;
};
struct CourseDriverOutput50GK {
    // +1 forward throttle, 0 neutral, -1 brake. No buttons are synthesized.
    float left=0.f,right=0.f;
};
// Serial, bounded and allocation-free. False means a reported stop/invalid
// input; output is always neutral in that case. Stuck/off-course latch until
// caller explicitly resets this HOST state, or disables the driver. Disable is
// immediate and clears the host latch; it never resets any game state.
// The caller must preserve its normal packet dispatch/control/physics ordering.
// Route: the one common navigation view (map or AI role) of the race's track.
bool driveCourse50GK(const game::NavigationView50CR&,const CourseSpeedLimits50GK&,
    const CourseDriverTuning50GK&,const CourseDriverInput50GK&,
    CourseDriverState50GK&,CourseDriverOutput50GK&,CourseDriverDiagnostic50GK&) noexcept;

// Changes only the two existing preset4 analog throttle bytes. Does not press,
// release or bypass buttons. Compose a released/manual packet first and then
// call this only while the host driver owns the input. All other bytes survive.
// Zero demand maps to128, full throttle0, full brake255. Invalid output rejects
// without changing the packet. The unchanged native reader/shaper handles it.
bool applyCourseThrottlePacket50GK(const CourseDriverOutput50GK&,
    game::DevicePacket48Y&) noexcept;
}
