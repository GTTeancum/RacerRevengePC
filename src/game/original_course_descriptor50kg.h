#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// Shared constructor seeds for profile11278, stride12, in track-ID order.
// This is NOT a live profile or loaded-save override. Actual Profile startup
// copies rival11280 into its own descriptor storage; cold AI selection reads
// these same seeds instead of maintaining a second rival table.
struct OriginalFreshCourseDescriptor50KG {
    char first,second;
    std::int8_t rival11280;
};
inline constexpr std::array<OriginalFreshCourseDescriptor50KG,13>
originalFreshCourseDescriptors50KG{{
    {'T','A',19}, //17EA1C ->11280
    {'T','B', 7}, //17EC0C ->1128C
    {'T','C',18}, //17ED38 ->11298
    {'R','C',24}, //17ED9C ->112A4
    {'M','A',20}, //17EAE0 ->112B0; s1=20 at17E68C
    {'M','B',22}, //17EC70 ->112BC
    {'M','C', 8}, //17EE00 ->112C8
    {'R','A', 9}, //17EA80 ->112D4
    {'G','A',10}, //17EB44 ->112E0
    {'G','B',23}, //17ECD4 ->112EC
    {'G','C',17}, //17EE64 ->112F8
    {'S','A',12}, //17EBA8 ->11304
    {'S','C',16}  //17EEC8 ->11310
}};
}
