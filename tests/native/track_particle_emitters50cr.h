#pragma once
// Common rewrite (W0 round 2, request r7 #3): the ONE track ground-effects owner,
// common_track50CR::Roles::emitters, for every course. Declarations frozen by W0;
// implemented by area E (lights + media) in its own TU.
//
// What it is: the runtime of the TOB [ParticleEmitters] table (27DE00 rows,
// 27C600/27C7B0 placements), the EMT definitions those rows name, the SoulTree
// attachments of their instances, the 15AEA0 registry reservation in the one
// particle manager (Roles::particles, 1B4DC0), and the per-frame emission of each
// placement into that manager. The effect behaviour is chosen ONLY by each row's
// TypeID, the original's own switch on data:
//   0 smoke, 2 sparks, 8 fire, 11 firework, 12 fog.
// So the GB fires and fireworks, MC fires and water sparks, RC fog, fire and smoke,
// RA attached fires and GC fog and generator fires all come back through this one
// owner, with no course code. A TypeID outside the switch is refused (logged), not
// guessed.
//
// Base (area E): generalise the retired MC lane at b607b29
// (tests\native\mc_particle_source50mc.h, mc_particle_attachments50mc.h,
// mc_particle_runtime50mc.h) with the per-TypeID numerics kept in
// tests\native\mc_type2_numeric50mc.h (TypeID 2) and gb_type11_numeric50gb.h
// (TypeID 11), the RC Type12 leaf src\game\original_rc_fog_numeric50rc.h, and the
// GC fire update 15E288 that lived in native_gc_water_session50KG; drop every
// member/size/SHA pin and every fixed descriptor count.
//
// Storage rule: definitions, rows and placements are sized once from the TOB's own
// counts through common_track50CR::Block (BlockId::particleEmitters,
// particleDefinitions, particleAttachments); every write bounds-checked; nothing
// writes guest memory. Logs common_track_data role=particles.emitters /
// particles.attachments / particles.definitions (trackDataNamed).
#include "common_track50cr.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>

namespace rr::game { class OriginalTrackParticleCold50LA; struct HostMonotonicClock50ID; }
namespace rr::platform { struct OriginalUiMainApplication50KG; }
namespace native_gc_water_session50KG { struct Application; }

namespace track_particle_emitters50CR {

// TOB [ParticleEmitters] TypeID: the original's per-row effect switch (data).
enum class Effect : std::uint32_t { smoke=0u, sparks=2u, fire=8u, firework=11u, fog=12u };

// One TOB emitter row (27DE00).
struct Descriptor final {
    std::uint32_t emitterNumber=0;   // 1-based EmitterNumber as authored (trigger Particles actions name it)
    std::uint32_t typeId=0;          // authored TypeID
    Effect effect=Effect::smoke;     // typeId as the switch value (valid only when the row was admitted)
    std::size_t definition=0;        // index into the definitions (distinct EmitterFilename)
    std::size_t firstAttachment=0;   // first placement of this row
    std::uint32_t instances=0;       // authored instance count (placements of this row)
    bool initiallyOn=false;          // authored start state
};
// One placement (27C600/27C7B0, SoulTree attachment).
struct Attachment final {
    std::size_t descriptor=0;        // owning row
    std::uint32_t instanceNumber=0;  // 1-based InstanceNumber as authored
    std::size_t worldIndex=0;        // SCB world instance it is attached to
    const rr::assets::WorldInstance48F* worldInstance=nullptr;  // borrowed from track.documents.world
};
// Process inputs the emitters share with the rest of the race (never owned here).
struct Inputs final {
    std::shared_ptr<rr::game::OriginalTrackParticleCold50LA> manager;      // track.roles.particles (05.06)
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> process;  // process clock 101A50
    std::shared_ptr<native_gc_water_session50KG::Application> globals;     // shared particle globals (434630, 433970, sparks)
    std::shared_ptr<rr::game::HostMonotonicClock50ID> clock;
    std::uint64_t generation=0;                                             // session generation
};
// Per placement, whether the frame that drew its world instance completed.
enum class Completion : std::uint8_t { unavailable, stale, current };

class Owner final {
public:
    struct State;                                    // area E defines
    Owner()noexcept;
    ~Owner();
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // 05.06, after the manager is prepared: read the TOB rows, the EMT definitions and
    // the placements; size every block from them; reserve the 15AEA0 registry in
    // inputs.manager. No rows: returns true with zero emitters (Roles::emitters stays null
    // by the caller's choice). Refuses (d) on any unreadable row or an unknown TypeID.
    bool prepare(const common_track50CR::Track& track,const Inputs& inputs,common_track50CR::Diagnostic& d)noexcept;
    bool ready()const noexcept;
    std::size_t emitterCount()const noexcept;
    std::size_t attachedCount()const noexcept;
    std::size_t definitionCount()const noexcept;
    const Descriptor* descriptor(std::size_t row)const noexcept;
    const Attachment* attachment(std::size_t placement)const noexcept;
    // Trigger Particles action (R5 ActionSink adapter, area E): EmitterNumber /
    // InstanceNumber as authored (1-based; 0 instance = every instance of the row), TurnOn.
    // Bounds-checked; an unknown emitter or instance is refused (logged).
    bool applyTriggerAction(std::uint32_t emitterNumber,std::uint32_t instanceNumber,bool turnOn,
        common_track50CR::Diagnostic& d)noexcept;
    // One frame: advance the manager once (161180 order) and every enabled placement by
    // its Effect. `completed` has one entry per placement (attachedCount()); a shorter
    // span is refused.
    bool advance(float dt,std::span<const Completion> completed,common_track50CR::Diagnostic& d)noexcept;
    // 1AC8E0 161F60 (World+4A0 particles): every placement back to its authored start state.
    bool restart161F60(common_track50CR::Diagnostic& d)noexcept;
    // Teardown, before the manager pool closes: stop emitting, then release.
    bool quiesce()noexcept;
    bool closeAfterPool()noexcept;
private:
    std::unique_ptr<State> state_;
};

} // namespace track_particle_emitters50CR
