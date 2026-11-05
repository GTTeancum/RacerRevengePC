#include "../../src/game/original_water_opacity_numeric50la.h"
#include "../../src/game/original_water_scale_numeric50la.h"
#include <cstdio>
#include <cstdint>
#include <initializer_list>

int main() {
    using namespace rr::game::detail;
    int failures=0;
    const auto check=[&](bool ok,const char* label){
        if(!ok){std::fprintf(stderr,"FAIL: %s\n",label);++failures;}
    };
    std::uint32_t out=0,mask=0;
    check(sourceWaterOpacitySubtract50LA(0x42800000u,0x42000000u,out,mask)&&
          out==0x42000000u&&mask==0x8000u,"default opacity 64-32");
    check(sourceWaterOpacitySubtract50LA(0x42000000u,0x42800000u,out,mask)&&
          out==0xc2000000u,"negative opacity difference");
    check(sourceWaterOpacitySubtract50LA(0x42800000u,0x42800000u,out,mask)&&
          out==0,"exact cancellation");
    check(sourceWaterOpacitySubtract50LA(0,0x42000000u,out,mask)&&
          out==0xc2000000u,"zero minus normal");
    check(sourceWaterOpacitySubtract50LA(0,0,out,mask)&&out==0,"positive zero pair");
    // Adjacent normals at the minimum admitted exponent reach the cancellation
    // end of the proven output range, rather than being flushed to zero.
    check(sourceWaterOpacitySubtract50LA(0x35800001u,0x35800000u,out,mask)&&
          out==0x2a000000u,"minimum-exponent adjacent cancellation");
    for(auto bad:{0x80000000u,1u,0x35000000u,0x4e800000u,0x7f800000u,0x7fc00000u}) {
        out=0x12345678u;mask=0xabcdef01u;
        check(!sourceWaterOpacitySubtract50LA(bad,0,out,mask)&&
              out==0x12345678u&&mask==0xabcdef01u,"left refusal atomicity");
        check(!sourceWaterOpacitySubtract50LA(0,bad,out,mask)&&
              out==0x12345678u&&mask==0xabcdef01u,"right refusal atomicity");
    }
    check(sourceWaterScale16Native50LA(0x3fc00000u,0x41800000u,out,mask)&&
          out==0x41c00000u&&mask==0x8000u,"1.5 times 16");
    check(sourceWaterScale16Native50LA(0xbfc00000u,0x41800000u,out,mask)&&
          out==0xc1c00000u,"negative scale");
    check(sourceWaterScale16Native50LA(0,0x41800000u,out,mask)&&out==0,"zero scale");
    check(sourceWaterScale16Native50LA(0x00800000u,0x41800000u,out,mask)&&
          out==0x02800000u,"minimum normal scale");
    check(sourceWaterScale16Native50LA(0x7d7fffffu,0x41800000u,out,mask)&&
          out==0x7f7fffffu,"maximum admitted scale");
    for(auto bad:{0x80000000u,1u,0x7d800000u,0x7f800000u,0x7fc00000u}) {
        out=0x12345678u;mask=0xabcdef01u;
        check(!sourceWaterScale16Native50LA(bad,0x41800000u,out,mask)&&
              out==0x12345678u&&mask==0xabcdef01u,"scale refusal atomicity");
    }
    out=0x12345678u;mask=0xabcdef01u;
    check(!sourceWaterScale16Native50LA(0x3f800000u,0x41000000u,out,mask)&&
          out==0x12345678u&&mask==0xabcdef01u,"right operand must be 16");
    if(!failures)std::puts("Water numeric reconstruction checks passed");
    return failures?1:0;
}
