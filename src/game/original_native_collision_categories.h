#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// One declared native registry namespace shared by the current live track,
// racer, spring, terrain and reset-query owners. Values are HOST-assigned bits,
// not frozen guest/global values. Existing bindings1..64 remain unchanged.
// Source3C9FF8 looks up name POD (literal3B3000), then3CA004 stores the returned
// bit at3B14B0.128 is this namespace's new, previously unused bit for that name.
struct OriginalNativeCollisionCategories final {
    static constexpr std::uint32_t character=1u,physicsBody=2u,staticObject=4u,
        trigger=8u,floor=16u,water=32u,tube=64u,pod=128u;
    struct NamedBinding {const char* name;std::uint32_t bit;};
    inline static constexpr std::array<NamedBinding,8> bindings{{
        {"CHARACTER",character},{"PHYSICSBODY",physicsBody},
        {"STATICOBJECT",staticObject},{"TRIGGER",trigger},
        {"FLOOR",floor},{"WATER",water},{"TUBE",tube},{"POD",pod}
    }};
    // Check canonical existing owners before adding POD. Call with actual
    // current query/material/category values, never matching literal copies.
    static constexpr bool matchesExisting(std::uint32_t actualPhysicsBody,
        std::uint32_t actualStaticObject,std::uint32_t actualTrigger,
        std::uint32_t actualFloor,std::uint32_t actualWater,
        std::uint32_t actualTube) noexcept {
        return actualPhysicsBody==physicsBody&&actualStaticObject==staticObject&&
            actualTrigger==trigger&&actualFloor==floor&&actualWater==water&&actualTube==tube;
    }
};
}
