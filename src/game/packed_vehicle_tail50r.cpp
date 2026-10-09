#include "packed_vehicle_tail50r.h"
#include "float_unsigned50r.h"
#include <cstring>
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
std::uint32_t bits(float f)noexcept{std::uint32_t b;std::memcpy(&b,&f,4);return b;}
float scalar(std::uint32_t b)noexcept{float f;std::memcpy(&f,&b,4);return f;}
bool admitted(const float&f)noexcept{std::uint32_t b;std::memcpy(&b,&f,4);const auto e=b&0x7f800000u;return e!=0x7f800000u&&(e||!(b&0x7fffffu));}
unsigned direction(float f)noexcept{return f<=0.f?(f<=-20.f?0u:1u):(f<20.f?2u:3u);}
}
PackedVehicleResult50R packVehicleTail50R(const PackedVehicleInput50R&in)noexcept{
 PackedVehicleResult50R out{};
 if(std::fegetround()!=FE_TONEAREST)return out;
 for(unsigned i=0;i<4;++i){if(!admitted(in.drive[i]))return {};volatile float q=in.drive[i]/scalar(0x3eaa7efau);float value=q;if(!admitted(value))return {};auto code=sourceFloatUnsigned50R(bits(value))&255u;if(code>=4)code=3;out.bytesC00[0]|=static_cast<std::uint8_t>(code<<(2*i));}
 unsigned brake;
 if(!in.vehicle58&&in.configurationEntryNonzero){
  if(!admitted(in.filteredB70))return {};brake=0;
  if(!(in.filteredB70<scalar(0x3f666666u))){if(!admitted(in.filteredB78))return {};brake=!(in.filteredB78<scalar(0x3f666666u));}
 }else{if(!admitted(in.actionB80))return {};brake=sourceFloatUnsigned50R(bits(in.actionB80))&255u;}
 float square[3];for(unsigned i=0;i<3;++i){if(!admitted(in.impactBA0[i]))return {};volatile float q=in.impactBA0[i]*in.impactBA0[i];square[i]=q;if(!admitted(square[i]))return {};}
 volatile float xy=square[0]+square[1];volatile float total=xy+square[2];float sum=total;if(!admitted(sum))return {};volatile float root=std::sqrt(sum);float length=root;if(!admitted(length))return {};
 unsigned impact=64u;if(!(length<=5.f))impact=direction(in.impactBA0[0])|(direction(in.impactBA0[1])<<2)|(direction(in.impactBA0[2])<<4);
 out.bytesC00[1]=static_cast<std::uint8_t>(impact|(brake<<7));out.bytesC00[2]=static_cast<std::uint8_t>(in.state40|((in.flagsB9C&1u)<<5)|((in.flagsB9C&2u)<<5));
 const unsigned order[4]={1,0,3,2};for(unsigned i=0;i<4;++i){float damage=in.damage7D0[order[i]];if(!admitted(damage))return {};volatile float product=255.f*(damage<1.f?damage:1.f);float value=product;if(!admitted(value))return {};out.bytesC00[3+i]=static_cast<std::uint8_t>(sourceFloatUnsigned50R(bits(value)));}
 out.complete=true;return out;
}
}
