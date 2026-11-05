#include "original_manager_fields.h"
#include <iostream>
#include <memory>
#include <algorithm>
using namespace rr::game;
struct Fixture {
 std::unique_ptr<VehicleFrameState49W> state=std::make_unique<VehicleFrameState49W>();
 lap_drive50GJ::Continuity lap{};
 std::array<std::uint64_t,4096> unrelated{};
 native_racer50HN::Bank bank;
 original_manager_fields::Access access;
 Fixture(){
  bank.owner(state->fields);bank.owner(lap);bank.owner(unrelated);bank.seal(8);
  for(std::size_t i=0;i<8;++i)bank.visit(i,[&]{
   state->fields.vehicle3C=100+static_cast<unsigned>(i);state->fields.vehicle58=i?1:0;
   state->fields.impactFlagsCD8=0;state->fields.applicationByte58=2;
   lap.state.vehicleCC8=static_cast<unsigned>(i);lap.state.vehicleCD8=0;
   lap.timing.timeCB8=static_cast<float>(i);lap.timingCursor.owner=&lap.timing;
   unrelated.fill(0xabc000+i);
  });
 }
 void pass(bool narrow,unsigned pass,bool fail){
  unsigned ordered=0;
  auto operation=[&](std::size_t slot){
   if(slot!=ordered++||lap.timingCursor.owner!=&lap.timing)throw std::runtime_error("order or owner");
   if(state->fields.vehicle3C!=100+slot||lap.state.vehicleCC8!=slot)throw std::runtime_error("wrong racer");
   lap.timing.timeCB8+=.016f;lap.timing.valueCBC=static_cast<float>(pass+slot);
   lap.timing.wordCD4=pass*10+static_cast<unsigned>(slot);
   lap.timing.lapTimesCC0[slot]=lap.timing.timeCB8;
   lap.timingCursor.lastSequence+=pass+1;state->fields.applicationByte58=static_cast<unsigned char>(pass+2);
   state->fields.impactFlagsCD8=static_cast<unsigned char>(slot&1);lap.state.vehicleCD8=slot&1;
   if(fail&&slot==3)throw std::runtime_error("expected partial failure");
  };
  if(narrow)access.visitAll(bank,*state,lap,operation);
  else for(std::size_t slot=0;slot<8;++slot)bank.visit(slot,[&]{operation(slot);});
 }
};
static void require(bool okay){if(!okay)throw std::runtime_error("manager field regression");}
static void compare(Fixture& a,Fixture& b){
 require(a.state->fields.vehicle3C==100&&b.state->fields.vehicle3C==100);
 for(std::size_t slot=0;slot<8;++slot){
  auto fa=a.state->fields,fb=b.state->fields;lap_drive50GJ::Continuity la{},lb{};
  a.bank.visit(slot,[&]{fa=a.state->fields;la=a.lap;require(std::all_of(a.unrelated.begin(),a.unrelated.end(),[&](auto value){return value==0xabc000+slot;}));require(a.lap.timingCursor.owner==&a.lap.timing);});
  b.bank.visit(slot,[&]{fb=b.state->fields;lb=b.lap;require(std::all_of(b.unrelated.begin(),b.unrelated.end(),[&](auto value){return value==0xabc000+slot;}));require(b.lap.timingCursor.owner==&b.lap.timing);});
  la.timingCursor.owner=lb.timingCursor.owner=nullptr;
  require(std::memcmp(&fa,&fb,sizeof(fa))==0&&std::memcmp(&la,&lb,sizeof(la))==0);
 }
}
int main(){
 Fixture full,narrow;
 for(unsigned pass=0;pass<6;++pass){full.pass(false,pass,false);narrow.pass(true,pass,false);compare(full,narrow);}
 for(auto* fixture:{&full,&narrow}){bool thrown=false;try{fixture->pass(fixture==&narrow,7,true);}catch(const std::runtime_error&){thrown=true;}require(thrown);}
 compare(full,narrow);
 const auto handle=narrow.bank.bindRead(narrow.state->fields.vehicle3C);
 narrow.access.visitAll(narrow.bank,*narrow.state,narrow.lap,[&](std::size_t){
  bool readRejected=false,visitRejected=false;
  try{(void)narrow.bank.read(0,handle);}catch(const std::runtime_error&){readRejected=true;}
  try{narrow.bank.visit(0,[]{});}catch(const std::runtime_error&){visitRejected=true;}
  require(readRejected&&visitRejected);
 });
 compare(full,narrow);
 Fixture other;bool ownerRejected=false;
 try{narrow.access.visitAll(other.bank,*other.state,other.lap,[](std::size_t){});}catch(const std::runtime_error&){ownerRejected=true;}
 require(ownerRejected);
 native_racer50HN::Bank unsealed;original_manager_fields::Access access;bool unsealedRejected=false;
 try{access.visitAll(unsealed,*narrow.state,narrow.lap,[](std::size_t){});}catch(const std::runtime_error&){unsealedRejected=true;}
 require(unsealedRejected);
 // Exercise the actual numeric lap request/resume leaf across separated
 // arena visits. The supplied return is a unit-fixture event acknowledgement,
 // not an assertion that the game's event subscriber was run by this test.
 Fixture eventFull,eventNarrow;
 LapTimingManager50ID managers[2]{{50.f,2},{50.f,2}};
 for(unsigned mode=0;mode<2;++mode){auto& fixture=mode?eventNarrow:eventFull;
  for(unsigned phase=0;phase<2;++phase){
   auto operation=[&](std::size_t slot){
    if(!phase){
     fixture.lap.timing.timeCB0=12.34f;
     const LapTimingInput50ID input{.016f,1,fixture.state->fields.vehicle58,
         static_cast<std::uint32_t>(slot),1,2};
     auto result=beginRacerLapTiming50ID(fixture.lap.timing,managers[mode],input,fixture.lap.timingCursor);
     require(result.status==LapTimingStatus50ID::NeedLapEvent&&fixture.lap.timingCursor.owner==&fixture.lap.timing);
    }else{
     const auto sequence=fixture.lap.timingCursor.pending.sequence;
     require(sequence!=0&&fixture.lap.timingCursor.owner==&fixture.lap.timing);
     auto result=resumeRacerLapTimingAfterEvent50ID(fixture.lap.timing,managers[mode],fixture.lap.timingCursor,sequence);
     require(result.status==LapTimingStatus50ID::Complete&&!fixture.lap.timingCursor.owner&&!fixture.lap.timingCursor.pending.sequence);
    }
   };
   if(mode)fixture.access.visitAll(fixture.bank,*fixture.state,fixture.lap,operation);
   else for(std::size_t slot=0;slot<8;++slot)fixture.bank.visit(slot,[&]{operation(slot);});
  }
 }
 require(managers[0].time30==managers[1].time30&&managers[0].flags58==managers[1].flags58);
 for(std::size_t slot=0;slot<8;++slot){LapTimingFields50ID first{},second{};
  eventFull.bank.visit(slot,[&]{first=eventFull.lap.timing;});
  eventNarrow.bank.visit(slot,[&]{second=eventNarrow.lap.timing;});
  require(std::memcmp(&first,&second,sizeof(first))==0);
 }
 std::cout<<"manager_fields_check passed bytes_per_racer="<<narrow.access.bytesPerRacer()<<" full_fixture_bytes="<<narrow.bank.bytesPerRacer()<<"\n";
}
