#pragma once
#include "body_integration48c.h"

namespace rr::game {

struct EngineParameters48D {
    float maxThrust, boostThrust, repairThrust; // Original +40,+44,+48.
    float rampUpFilter, rampDownFilter;         // +4C,+50; per invocation.
    float rampDownSpeed, theoreticalTopSpeed;  // +54,+58; already converted units.
    float boostSpeed, repairSpeed;             // +60,+64; already converted units.
    // Setter 0x2E7680 receives Left/RightEnginePosition-derived vectors here.
    // The update/application path consumes them as direct torque terms; do not
    // replace this behavior with an invented lever-arm cross product.
    Vector3_48C torqueBasis[2];                 // +80,+90.
};
struct EngineInput48D {
    float drive[2];                            // +68,+6C; action mapping external.
    float bodySpeed;                           // Linked body's cached +34 value.
    bool boost, repair, fullTorque;            // +34,+38,+3C; boost has priority.
};
struct EngineState48D {
    float smoothedDrive[2];                    // +70,+74.
    Vector3_48C localThrust[2];                 // +A0,+B0 (XYZ only).
    Vector3_48C torque[2];                     // +C0,+D0 (XYZ only).
};
struct ReadyBodyBasis48D {
    // Resolved columns at associated scene object +100,+110,+120. No dirty
    // scene-graph traversal is implemented by this extraction.
    Vector3_48C columns[3];
};
struct GravityParameters48D {
    float magnitude;                          // Gravity component +38.
    float forceX, forceZ;                      // Preserved component +40,+48.
};
struct EngineGravityStep48D {
    EngineState48D engine;
    BodyLoads48C accumulatedLoads;
    BodyState48C body;
};

// Full original 0x2E7720 engine producer. It never reads elapsed time. Smoothing
// remains per invocation; adding dt scaling would change original behavior.
// Finite inputs and finite intermediates required. Failure does not mutate out.
bool updateEngine48D(const EngineState48D& previous, const EngineParameters48D& p,
                     const EngineInput48D& input, EngineState48D& out) noexcept;

// Original single-target gravity addition, followed (when enabled) by engine
// application. Caller has already resolved membership in the gravity target
// list. Thrust uses the ready basis; torque is added directly, left then right.
// No scene transform means the original skips thrust but still adds torque.
bool accumulateEngineGravity48D(const BodyLoads48C& previous,
                                const EngineState48D& engine,
                                const ReadyBodyBasis48D& basis,
                                GravityParameters48D gravity, float bodyMass,
                                bool applyEngine, bool hasSceneTransform,
                                BodyLoads48C& out) noexcept;

// Connected PARTIAL step: engine producer -> gravity -> conditional engine
// application -> proven 48C body core. Requires the ready scene basis and body
// core entry state. Aerodynamics, repulsor, collision, scene writeback, actual
// action mapping and original scheduler are NOT supplied by this function.
// elapsedFactor is passed solely to 48C body integration, in its original units.
bool stepEngineGravityBody48D(const BodyState48C& body, const BodyParameters48C& bodyParameters,
                             const BodyLoads48C& priorLoads, const EngineState48D& engine,
                             const EngineParameters48D& engineParameters, const EngineInput48D& input,
                             const ReadyBodyBasis48D& basis, GravityParameters48D gravity,
                             bool applyEngine, float elapsedFactor,
                             EngineGravityStep48D& out) noexcept;
}
