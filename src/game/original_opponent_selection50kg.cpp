#include "original_opponent_selection50kg.h"
namespace rr::game::original_opponent_selection50KG {
namespace {
constexpr std::array<RacerId,8> class1_3A8940{20,4,10,11,12,15,19,2};
constexpr std::array<RacerId,7> class2_3A8960{3,7,9,10,16,22,23};
constexpr std::array<RacerId,7> class3_3A8980{8,10,17,18,19,16,24};
}
std::uint32_t courseIndex187B10(std::span<const std::uint8_t,13> order,
    std::uint8_t course1125F) noexcept {
 const auto course=course1125F&15u;
 for(std::uint32_t i=0;i<13;++i)if(order[i]==course)return i;
 return 0;
}
bool deterministic188470(std::uint32_t klass,RacerId human,
    std::array<RacerId,7>& out) noexcept {
 if(klass==1){
  auto local=class1_3A8940;
  for(std::uint32_t i=0;i<8;++i){
   if(local[i]==human){local[i]=local[7];local[7]=human;}
   // Deliberately reads the result of the preceding replacement.
   if(human==1&&local[i]==19){local[i]=local[7];local[7]=human;}
  }
  for(std::uint32_t i=0;i<7;++i)out[i]=local[i];
  return true;
 }
 if(klass==2){
  out=class2_3A8960;
  for(auto& id:out)if(id==human)id=19;
  return true;
 }
 if(klass==3){
  out=class3_3A8980;std::int32_t index=-1;
  for(std::uint32_t i=0;i<7;++i){
   if(out[i]==human)index=static_cast<std::int32_t>(i);
   if(human==1&&out[i]==19)index=static_cast<std::int32_t>(i);
   if(human==14&&out[i]==18)index=static_cast<std::int32_t>(i);
  }
  if(index!=-1){auto& id=out[static_cast<std::uint32_t>(index)];
   if(id==22||id==17)id=9;
   else if(id==24||id==19)id=7;
   else id=11;
  }
  return true;
 }
 return false;
}
}
