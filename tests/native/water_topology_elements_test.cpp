#include "../../src/game/original_water_topology_elements50la.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <limits>

int main(){
    using E=rr::game::OriginalWaterTopologyElements50LA;
    for(const std::uint32_t count:std::array<std::uint32_t,5>{0,1,2,31,16384}){
        E e;assert(!e.construct()&&!e.knownByte(0));
        assert(e.allocate(count)==E::Allocation::ready);
        assert(e.extent()==std::size_t(count)*48+16&&e.count()==count);
        assert(!e.elementsIdentity()&&!e.knownByte(0));
        assert(e.construct()&&!e.construct());
        assert(static_cast<const std::uint8_t*>(e.elementsIdentity())==
            static_cast<const std::uint8_t*>(e.allocationIdentity())+16);
        // Assert every byte's knownness, including the untouched cookie gap
        // and first32 bytes of every element. Never inspect unknown payload.
        for(std::size_t at=0;at<e.extent();++at){
            const auto value=e.knownByte(at);
            if(at<8){
                const auto word=at<4?48u:count;
                assert(value==std::uint8_t(word>>(8u*(at%4))));
            }else if(at<16||(at-16)%48<32)assert(!value);
            else assert(value==std::uint8_t((at-16)%48<44?0:255));
        }
        assert(!e.knownByte(e.extent())&&!e.knownByte(std::numeric_limits<std::size_t>::max()));
        assert(e.allocate(count)==E::Allocation::alreadyAttempted);
    }
    E refused;assert(refused.allocate(16385)==E::Allocation::capacity);
    assert(!refused.construct()&&!refused.allocationIdentity()&&!refused.knownByte(0));
    std::puts("Water topology elements: cookie, constructor lanes, unknown bytes, zero/max/bounds passed");
}
