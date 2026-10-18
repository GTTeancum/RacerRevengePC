#pragma once
#include <cstdint>

namespace undertrack_owners50ew_fixture {
// Explicit cold constructor role. The default attach caller remains AI; a
// genuine Player caller must opt in without modifying its source type fields.
enum class ColdQueryRole50LH : std::uint8_t { originalAi, humanPlayer };
constexpr bool coldQueryRoleMatches50LH(ColdQueryRole50LH role,
    std::uint32_t vehicle58,bool nativeAiOwnersBound) noexcept {
    switch(role){
    case ColdQueryRole50LH::originalAi:return vehicle58==1u&&nativeAiOwnersBound;
    case ColdQueryRole50LH::humanPlayer:return vehicle58==0u&&!nativeAiOwnersBound;
    }
    return false;
}
}
