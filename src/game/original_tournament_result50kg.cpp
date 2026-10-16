#include "original_tournament_result50kg.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&std::numeric_limits<float>::digits==24,"binary32 source arithmetic");
float word(std::uint32_t bits) noexcept {float f;std::memcpy(&f,&bits,4);return f;}
bool ordinary(float f) noexcept {return std::isfinite(f)&&(f==0.f||std::isnormal(f));}
struct Math {
 bool valid=true;
 float keep(float f) noexcept {valid=valid&&ordinary(f);return f;}
 float add(float a,float b) noexcept {volatile float f=a+b;return keep(f);}
 float sub(float a,float b) noexcept {volatile float f=a-b;return keep(f);}
 float mul(float a,float b) noexcept {volatile float f=a*b;return keep(f);}
 float div(float a,float b) noexcept {volatile float f=a/b;return keep(f);}
};
std::int32_t truncSaturating(float f) noexcept {
 if(!ordinary(f))return 0; //Math already rejects this entire native admission.
 if(f>=2147483648.f)return std::numeric_limits<std::int32_t>::max();
 if(f<=-2147483648.f)return std::numeric_limits<std::int32_t>::min();
 return static_cast<std::int32_t>(f);
}
std::int16_t lowHalf(std::uint32_t f) noexcept {const auto u=static_cast<std::uint16_t>(f);std::int16_t s;std::memcpy(&s,&u,2);return s;}
constexpr std::int32_t placePercent[]={0,100,70,50,30,20,10,5,0};
constexpr std::int32_t ordinaryKillPercent[]={0,20,30,50,60,70,80,100};
constexpr std::int32_t taMbKillPercent[]={0,30,50,60,70,80,90,100};
} //namespace
OriginalTournamentResultReceipt50KG applyOriginalTournamentResult50KG(
 OriginalTournamentProfile50KG& profile,std::uint32_t selectedCareer,
 std::uint8_t selectedTrack,const OriginalTournamentResultRacer50KG* rows,
 std::size_t count,std::uint32_t humans,std::uint8_t& availability,
 std::uint32_t& sharedRandom) noexcept {
 OriginalTournamentResultReceipt50KG r;r.sourcePC=0x1875c0;
 if(selectedCareer>=27||!rows||count<1||count>8||humans<1||humans>2||humans>count)return r;
 std::size_t order=13;
 for(std::size_t i=0;i<13;++i)if(originalTournamentCourseOrder50KG[i]==(selectedTrack&15u)){order=i;break;}
 if(order==13)return r;
 for(std::size_t i=0;i<count;++i){
  // Source rank/medal/percentage indices require real1..8 ranks, actual pod
  // identities and human-first manager rows. No host distance/rank substitute.
  if(rows[i].pod60>=27||rows[i].placeCCC<1||rows[i].placeCCC>8||
     ((rows[i].class58==0)!=(i<humans)))return r;
 }
 if(std::fegetround()!=FE_TONEAREST){r.status=OriginalTournamentResultStatus50KG::numeric;return r;}
 const auto& first=rows[0];
 auto next=profile;auto rng=sharedRandom;auto available=availability;
 auto& career=next.careers[selectedCareer];
 r.courseOrder=static_cast<std::uint32_t>(order);
 const auto prior=career.pods[first.pod60].medals1938[order];
 next.tournamentBoundary102CC=0; //18766C, before both early returns.
 r.retired=first.retirementCD4!=0;
 r.priorMedalSkipped=!r.retired&&prior!=0&&first.placeCCC>=prior;
 // Original per-course kill record has7 authored percentage entries; the
 // following word is a distinct course-class field. Validate only when
 // the result reaches this operand, after the genuine early-return gates.
 if(!r.retired&&!r.priorMedalSkipped&&(first.killsCD0<0||first.killsCD0>7||
    static_cast<std::size_t>(first.killsCD0)>=count))return r;
 Math f;
 if(!r.retired&&!r.priorMedalSkipped){
  if(order==(career.progress219C&31u)&&first.placeCCC<4){
   career.progress219C=static_cast<std::uint8_t>((career.progress219C&0xe0u)|(((career.progress219C&31u)+1u)&31u));
   r.careerAdvanced=true;
   if((career.progress219C&31u)<13u){
    // Complete native1AF4B0: grow this career's OTHER pod rows in source
    // pod/lane order, preserving all156 shared308030 transitions.
    auto factor=f.add(word(0x3ecccccdu),f.div(f.mul(f.sub(static_cast<float>(first.placeCCC),3.f),word(0x3e4cccceu)),-2.f));
    factor=detail::scalarMax48O(word(0x3ecccccdu),detail::scalarMin48O(factor,word(0x3f19999au)));
    for(std::size_t pod=0;pod<27;++pod){
     if(pod==selectedCareer)continue;
     for(std::size_t lane=0;lane<6;++lane){
      const auto base=next.base0180[pod][lane],maximum=next.maximum0408[pod][lane],old=career.pods[pod].abilities1920[lane];
      if(!ordinary(base)||!ordinary(maximum)||!ordinary(old)){f.valid=false;break;}
      const auto step=f.div(f.sub(maximum,base),13.f);
      rng=rng*0x41c64e6du+0x3039u;++r.randomDraws;
      const auto unit=f.mul(word(0x30000000u),static_cast<float>(rng&0x7fffffffu));
      const auto growth=f.mul(step,f.add(factor,f.div(unit,2.f)));
      const auto grown=f.add(old,growth);
      career.pods[pod].abilities1920[lane]=detail::scalarMin48O(grown,maximum);
     }
    }
   }
  }
  if((order+1u==4u||order+1u==8u||order+1u==13u)&&first.placeCCC<4)next.tournamentBoundary102CC=1;
  //187900 intentionally reads career.pod[C], independently of human pod60.
  const auto diagonalPrior=career.pods[selectedCareer].medals1938[order];
  std::int32_t prize=0;
  if(first.placeCCC!=8&&(diagonalPrior==0||first.placeCCC<diagonalPrior)){
   const auto proportion=f.div(static_cast<float>(placePercent[first.placeCCC]),100.f);
   const auto amount=f.mul(static_cast<float>(career.pods[selectedCareer].remainingPrizes1946[order]),proportion);
   prize=truncSaturating(f.add(.5f,amount));
  }
  //1877E0 stores only the low16 signed representation after SUBU.
  auto& remaining=career.pods[selectedCareer].remainingPrizes1946[order];
  remaining=lowHalf(static_cast<std::uint32_t>(static_cast<std::int32_t>(remaining))-static_cast<std::uint32_t>(prize));
  std::int32_t bonus=0;
  if(first.killsCD0>0){
   const auto* percentages=(order==0u||order==6u)?taMbKillPercent:ordinaryKillPercent;
   const auto proportion=f.div(static_cast<float>(percentages[first.killsCD0]),100.f);
   bonus=truncSaturating(f.add(.5f,f.mul(static_cast<float>(prize),proportion)));
  }
  r.prize=static_cast<std::uint32_t>(prize);r.killBonus=static_cast<std::uint32_t>(bonus);
  const auto winnings=static_cast<std::uint32_t>(prize)+static_cast<std::uint32_t>(bonus);
  career.earned2190+=winnings;career.unspent2194+=winnings; //source32-bit ADDU
  for(std::size_t i=0;i<count;++i)career.pods[rows[i].pod60].medals1938[order]=static_cast<std::uint8_t>(rows[i].placeCCC);
  career.kills2198+=static_cast<std::uint32_t>(first.killsCD0);
 }
 if(!f.valid){r.status=OriginalTournamentResultStatus50KG::numeric;r.sourcePC=0x1af4b0;return r;}
 //187D80 runs even when1875C0 returned via retirement/prior-medal skip.
 if((career.progress219C&31u)==(available&31u)){
  const auto low=static_cast<std::uint8_t>(((available&31u)+1u)&31u);
  available=static_cast<std::uint8_t>((available&0xe0u)|(low>13u?13u:low));
  r.availabilityAdvanced=available!=availability;
 }
 profile=next;sharedRandom=rng;availability=available;
 r.status=OriginalTournamentResultStatus50KG::complete;r.sourcePC=0x187e1c;return r;
}
}
