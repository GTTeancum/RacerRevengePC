// Focused canonical238/23C ownership checks. No resource or game execution.
#include "native_racer_contact50ht_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <type_traits>

using namespace rr::game;
namespace c=native_racer_contact50HT;
namespace {
std::size_t checks=0;
void need(bool okay,const char* why){++checks;if(!okay){std::cerr<<why<<'\n';std::exit(1);}}
RootBodyState48H seeded(std::uint32_t count,std::uint32_t tag){
 RootBodyState48H r{};r.callbackState238=count;
 for(std::size_t i=0;i<BodyContactCapacity48R;++i)r.processedContactPartners23C[i]={tag+static_cast<std::uint32_t>(i)};
 return r;
}
}
int main(){
 static_assert(std::is_trivially_copyable_v<RootBodyState48H>);
 static_assert(offsetof(RootBodyState48H,processedContactPartners23C)==320);
 static_assert(sizeof(RootBodyState48H)==360);
 for(std::uint32_t count=0;count<=BodyContactCapacity48R;++count){
  const auto source=seeded(count,0x1200u);auto output=source;c::ProcessedList scratch;
  need(scratch.load(source),"bounded canonical count load");
  need(scratch.contains(0x1200u)==(count>0),"search uses active count only");
  need(!scratch.contains(0x1200u+count),"inactive slot does not count as contact");
  need(scratch.append(0x7777u),"source bounded append or full no-op");scratch.store(output);
  need(output.callbackState238==(count<10?count+1:count),"canonical count append/full");
  for(std::uint32_t i=0;i<10;++i)need(output.processedContactPartners23C[i].value==
      (count<10&&i==count?0x7777u:source.processedContactPartners23C[i].value),"inactive tail bits preserved");
  const auto once=output;need(scratch.append(0x7777u),"duplicate or full callback");scratch.store(output);
  need(!std::memcmp(&once,&output,sizeof output),"duplicate/full callback is no-op");
 }
 for(const std::uint32_t invalid:{11u,0x7fffffffu,0x80000000u,0xffffffffu}){
  const auto source=seeded(invalid,0x5500u);c::ProcessedList scratch;
  need(!scratch.load(source),"unsafe producer count rejected before canonical commit");
 }
 //1654B0 is count-only even when disabled/missing scene skips integration.
 for(unsigned path=0;path<2;++path){
  const auto source=seeded(3,0x9900u);RootBodyState48H output{};
  need(updateRootBody48H(source,{}, {},path!=0,false,.016f,output),"actual root clear path");
  auto expected=source;expected.callbackState238=0;
  need(!std::memcmp(&expected,&output,sizeof output),"root callback preserves every23C slot");
  c::ProcessedList scratch;need(scratch.load(output)&&!scratch.contains(0x9900u),"cleared count hides stale slot");
  need(scratch.append(0x8800u),"append after actual clear");scratch.store(output);
  need(output.callbackState238==1&&output.processedContactPartners23C[0].value==0x8800u&&
      output.processedContactPartners23C[1].value==source.processedContactPartners23C[1].value,
      "new cadence overwrites one slot only");
 }
 // The bank saves actual owner bytes; it never retains a per-slot arena pointer.
 RootBodyState48H arena=seeded(1,0x100u);native_racer50HN::Bank bank;bank.owner(arena);bank.seal(2);
 bank.visit(1,[&]{arena=seeded(2,0x200u);});
 const auto countRead=bank.bindRead(arena.callbackState238);
 const auto identityRead=bank.bindRead(arena.processedContactPartners23C[0].value);
 need(bank.read(0,countRead)==1&&bank.read(1,countRead)==2&&
      bank.read(0,identityRead)==0x100u&&bank.read(1,identityRead)==0x200u,"bank owns distinct persistent processed lists");
 bank.visit(1,[&]{need(arena.callbackState238==2&&arena.processedContactPartners23C[0].value==0x200u,"slot1 restored owner");});
 need(arena.callbackState238==1&&arena.processedContactPartners23C[0].value==0x100u,"slot0 restored owner");
 native_dynamic_contact50IW::Registry registry;std::string error;
 const native_dynamic_contact50IW::ManagerBinding roster[]={{1,{0xa001u}},{0,{0xb002u}}};
 need(registry.prepare(roster,2,1,error),"reordered source manager registry");
 need(registry.bodyIdentityForManagerIndex(0).value==0xa001u&&registry.bodyIdentityForSlot(0).value==0xb002u,
      "target1008 resolves source manager index rather than bank slot");
 need(registry.bodyIdentityForManagerIndex(2).value==0,"unbound target is not fabricated");
 std::cout<<"processed-contact owner checks="<<checks<<'\n';
}
