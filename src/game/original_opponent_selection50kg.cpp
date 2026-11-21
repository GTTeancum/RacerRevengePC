#include "original_opponent_selection50kg.h"
#include "lap_host_clock50id.h"
#include "original_shared_rng50iu.h"
#include "shared_random_owner50id.h"
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

namespace {
// Original stock candidate domain of 1AEF70: (int)(26.0f*(2^-31*(float)rand))+1.
constexpr std::uint32_t stockCandidates1AEF70=26;
// 1AF1C0..1AF1E0 / 1AF208..1AF228: racers 1, 14, 21, 25, 26 (the unlockable
// riders). Racer-keyed in the original itself.
constexpr bool unlockableRacer1AF1C0(RacerId r) noexcept {return r==1||r==14||r==21||r==25||r==26;}
// One 1AEF70 attempt's acceptance over the already placed racers
// racers[0..placed-1] (humans first, then the opponents placed so far).
// Every rejection branches back to 1AEF9C for a new draw; the checks are made
// per placed racer exactly as the original loop at 1AF0DC does, so with no
// placed racer every candidate is accepted (as in the original).
bool accept1AEF70(RacerId s1,std::span<const RacerId> placed,std::span<const RacerId> humans,
    const std::array<std::uint32_t,27>& unlock10FAC) noexcept {
 for(const RacerId t5:placed){
  if(s1==t5)return false;                                  // 1AF0F0 already in the field
  if(s1==5||s1==6||s1==13)return false;                    // 1AF100..1AF118 never an AI
  if(s1==1&&t5==19)return false;                           // 1AF124
  if(s1==19&&t5==1)return false;                           // 1AF13C
  if(s1==14&&t5==18)return false;                          // 1AF168
  if(s1==18&&t5==14)return false;                          // 1AF194
  if(!unlockableRacer1AF1C0(s1))continue;                  // 1AF1E0 bnel: next placed racer
  bool humanUnlockable=false;                              // t9, 1AF1F4..1AF248 over manager34
  for(const RacerId h:humans)if(unlockableRacer1AF1C0(h))humanUnlockable=true;
  if(!humanUnlockable)return false;                        // 1AF250
  // 1AF258..1AF26C: P+10FAC+4*s1 (s1 is 1..26 here: in range by the set above).
  if(unlock10FAC[static_cast<std::size_t>(s1)]==0)return false;
 }
 return true;
}
}

bool freePlay1AE620(const FreePlayInputs1AE620& in,CoupledNoiseContext49M& shared,
    SharedRandomOwner50ID& owner,HostMonotonicClock50ID& clock,FreePlayResult1AE620& out) noexcept {
 out={};
 FreePlayResult1AE620 r{};
 constexpr std::size_t maxRacers=8; // 1AD520 field limit
 if(in.humans.size()>2||in.opponents>r.opponents.size()||in.humans.size()+in.opponents>maxRacers){
  out.failPC=0x1AE620u;return false;
 }
 // Candidate list: stock 1..26 in the original's draw order, then the add-on
 // racers (PORT EXTENSION). Bounds-checked: every add-on number must be a
 // non-stock racer and the list must fit.
 // Room for every add-on racer number 32..99 (68); more is refused below.
 std::array<RacerId,stockCandidates1AEF70+68> candidates{};
 std::size_t candidateCount=0;
 for(RacerId n=1;n<=static_cast<RacerId>(stockCandidates1AEF70);++n)candidates[candidateCount++]=n;
 for(const RacerId a:in.addonCandidates){
  if(a<=static_cast<RacerId>(stockCandidates1AEF70)||candidateCount>=candidates.size()){out.failPC=0x1AEF70u;return false;}
  candidates[candidateCount++]=a;
 }
 r.candidates=static_cast<std::uint32_t>(candidateCount);
 std::array<RacerId,maxRacers> placed{};
 std::size_t placedCount=0;
 for(const RacerId h:in.humans)placed[placedCount++]=h;
 for(std::uint32_t ordinal=0;ordinal<in.opponents;++ordinal){
  // 1AE700..1AE778: opponent 0 is the course rival unless a human rides it.
  if(ordinal==0&&in.rival11280>0){
   bool humanRides=false;
   for(const RacerId h:in.humans)if(h==in.rival11280)humanRides=true;
   if(!humanRides){
    r.opponents[ordinal]=in.rival11280;r.rivalTaken=true;
    placed[placedCount++]=in.rival11280;continue;
   }
  }
  // 1AE780: 1AEF70(manager, ordinal+humans34).
  const std::span<const RacerId> already(placed.data(),placedCount);
  // Native guard (fail closed, NOT original): the original retries forever.
  // With 26 stock candidates and at most 7 placed racers an accepted
  // candidate always exists, so the guard is never reached on a valid field.
  constexpr std::uint32_t attemptLimit=1u<<20;
  bool accepted=false;
  for(std::uint32_t attempt=0;attempt<attemptLimit&&!accepted;++attempt){
   // 1AEF9C..1AEFE4: lazy seed, re-checked on every attempt (a cached zero
   // timestamp reseeds, as in the original).
   if(!owner.seed.lazy80A8){owner.seed.seed3D6B30=0;owner.seed.lazy80A8=true;}
   if(!owner.seed.seed3D6B30){
    std::uint32_t timestamp=0;
    if(!sampleHostTimestamp50ID(clock,timestamp)){out.failPC=0x1AEFC8u;return false;} // 101A50
    owner.seed.seed3D6B30=timestamp;seedOriginalSharedRng50IU(shared,timestamp);r.seeded=true; // 308020
   }
   const auto random=drawOriginalSharedRng50IU(shared);++r.draws; // 1AEFF0 308030
   // 1AEFF8..1AF024: (int)(N*(2^-31*(float)random)). EE CVT.S.W keeps 24
   // significant bits (toward zero); *2^-31 is exact; the toward-zero product
   // with an integer N < 2^24 has the same integer floor as this rational.
   std::uint32_t quantized=random,shift=0;
   for(auto magnitude=random;magnitude>=0x01000000u;magnitude>>=1)++shift;
   if(shift)quantized&=~((std::uint32_t{1}<<shift)-1u);
   const auto index=static_cast<std::uint64_t>((std::uint64_t{quantized}*candidateCount)>>31);
   // 1AF038..1AF094: range assertion 3D6B38 (guard 80AC), kept fail-closed.
   if(!owner.range.lazy80AC){owner.range.rangeCheck3D6B38=1;owner.range.lazy80AC=true;}
   if(index>=candidateCount){
    if(owner.range.rangeCheck3D6B38){out.failPC=0x1AF094u;return false;}
    continue;
   }
   const RacerId s1=candidates[static_cast<std::size_t>(index)]; // stock: index+1, as 1AF024
   if(!accept1AEF70(s1,already,in.humans,in.unlock10FAC))continue;
   r.opponents[ordinal]=s1;placed[placedCount++]=s1;accepted=true;
  }
  if(!accepted){out.failPC=0x1AEF9Cu;return false;}
 }
 r.count=in.opponents;out=r;return true;
}
}
