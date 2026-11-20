#pragma once
// Common rewrite (area E): the trigger Particles action destination over the one
// track ground-effects owner (track_particle_emitters50CR::Owner), for every course.
// course_trigger_actions50CT::Destinations::particles takes this sink.
//
// Original: a TOB TriggerNActionM "Particles" row names EmitterNumber /
// InstanceNumber (1-based) and TurnOn. On entry the action reaches the emitter's
// switch: 15A500 for TurnOn (virtual 88 timing reset, 80 start, 20 enable) or
// 15A560 for TurnOff (84 stop, 18 disable). ActionSink carries one address; it
// reports 0x15A500 (the TurnOn switch, which every stock Particles action uses)
// for both directions.
//
// preflight is pure: it checks the owner is ready and that the emitter row and
// instance exist (descriptor()/emitterCount(), no mutation). apply calls
// Owner::applyTriggerAction, which repeats those checks bounds-checked and
// refuses (logged) an unknown row or instance.
#include "track_particle_emitters50cr.h"
#include "course_trigger_actions50ct.h"
#include <cstddef>
#include <cstdint>

namespace track_particle_emitters50CR {
namespace sink_detail {
// Same lookup applyTriggerAction uses: the row whose authored EmitterNumber
// matches; instance 0 = every instance of that row, else 1..instances.
inline bool known(const Owner& owner,std::uint32_t emitterNumber,std::uint32_t instanceNumber)noexcept {
    if(!owner.ready()||!emitterNumber)return false;
    for(std::size_t i=0;i<owner.emitterCount();++i){
        const auto* d=owner.descriptor(i);
        if(!d||d->emitterNumber!=emitterNumber)continue;
        return d->instances!=0&&instanceNumber<=d->instances;
    }
    return false;
}
inline bool particlesAction(const course_trigger_actions50CT::ActionEvent& e)noexcept {
    return e.authored&&e.authored->kind==course_trigger_sources50CT::ActionKind::Particles;
}
} // namespace sink_detail

inline course_trigger_actions50CT::ActionSink triggerSink(Owner& owner)noexcept {
    course_trigger_actions50CT::ActionSink sink{};
    sink.context=&owner;
    sink.preflight=[](void* p,const course_trigger_actions50CT::ActionEvent& e)noexcept->bool{
        if(!p||!sink_detail::particlesAction(e))return false;
        return sink_detail::known(*static_cast<const Owner*>(p),e.authored->emitterNumber,e.authored->instanceNumber);
    };
    sink.apply=[](void* p,const course_trigger_actions50CT::ActionEvent& e)noexcept->bool{
        if(!p||!sink_detail::particlesAction(e))return false;
        common_track50CR::Diagnostic d;
        return static_cast<Owner*>(p)->applyTriggerAction(e.authored->emitterNumber,e.authored->instanceNumber,
            e.authored->turnOn,d);
    };
    sink.sourcePc=0x15a500u;   // 15A500 TurnOn switch (TurnOff is 15A560); see the header comment
    return sink;
}
} // namespace track_particle_emitters50CR
