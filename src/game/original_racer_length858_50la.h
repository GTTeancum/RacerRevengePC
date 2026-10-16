#pragma once
#include <cstdint>
namespace native_racer_model50LA {class Owner;}
namespace rr::game {
// Sole banked racer858, distinct from the immutable geometry calculation and
// the clamped Joint4C consumer. Only its retained constructor owner may write.
class OriginalRacerLength85850LA final {
    friend class ::native_racer_model50LA::Owner;
    enum class Stage:std::uint8_t {unwritten,initial211A2C,adjusted2126F4,consumed212718};
    float value_=0; //Storage only: unwritten is not an original zero publication.
    Stage stage_=Stage::unwritten;
    std::uint64_t generation_=0,slot_=0; //Native provenance, copied with the same Bank slot.
public:
    const float* value()const noexcept{return stage_==Stage::unwritten?nullptr:&value_;}
};
}
