#include "../../src/game/device_held48y.h"
#include "../../tools/gameplay/device_held_oracle48y.h"
#include "../../tools/gameplay/device_held_profile48y.h"
#include <array>
#include <cfenv>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>

static std::uint64_t allocations=0;
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
using RefBank=std::array<original48y::Record,32>;
static RefBank originalBank(const DeviceButtonBank48Y& b){RefBank r{};for(unsigned i=0;i<32;++i)std::memcpy(r[i].words.data(),&b.records[i],24);return r;}
static original48y::Record originalRecord(const DeviceButtonRecord48Y& r){original48y::Record x{};std::memcpy(x.words.data(),&r,24);return x;}
static original48y::QueryContext originalContext(DeviceQueryContext48Y c){return {c.present,c.rootFlags228,c.context518,c.context1898};}
struct ReferenceProvider {
    const RefBank* bank;
    original48y::QueryContext context;
    static PlayerActionResult48X query(void* opaque,std::int32_t code,std::uint32_t mask) noexcept {
        const auto& p=*static_cast<ReferenceProvider*>(opaque);
        try {return original48y::query(*p.bank,code,mask,p.context,false,{}).matched?PlayerActionResult48X::Match:PlayerActionResult48X::NoMatch;}
        catch(...){return PlayerActionResult48X::Unavailable;}
    }
};
int main(){
    std::uint64_t checks=0,queries=0,samples=0,connected=0,instructions=0,events=0;
    std::array<std::uint64_t,61> queryVisits{};std::array<std::uint64_t,49> gateVisits{};std::array<std::uint64_t,188> sampleVisits{};std::array<std::uint64_t,35> queueVisits{};
    auto require=[&](bool ok,const char* label){++checks;if(!ok)throw std::runtime_error(label);};
    auto record=[&](const original48y::Stats& s){instructions+=s.steps;events+=s.queueCalls;for(unsigned i=0;i<61;++i)queryVisits[i]+=s.visits(0x143600+4*i);for(unsigned i=0;i<49;++i)gateVisits[i]+=s.visits(0x143e30+4*i);for(unsigned i=0;i<188;++i)sampleVisits[i]+=s.visits(0x237ee0+4*i);for(unsigned i=0;i<35;++i)queueVisits[i]+=s.visits(0x127ff0+4*i);};
    auto compareQuery=[&](const DeviceButtonBank48Y& bank,int code,std::uint32_t mask,DeviceQueryContext48Y context,bool copy){
        const DeviceButtonRecord48Y sentry{9,0.625f,10,11,12,13};original48y::Stats stats{};
        const auto expected=original48y::query(originalBank(bank),code,mask,originalContext(context),copy,originalRecord(sentry),&stats);
        bool matched=true;auto got=sentry;
        require(queryDeviceHeld48Y(bank,code,mask,context,matched,copy?&got:nullptr),"valid query rejected");
        require(matched==expected.matched,"held query differential");
        require(std::memcmp(&got,expected.copied.words.data(),24)==0,"query payload copy/preservation");
        ++queries;record(stats);return matched;
    };
    auto compareSample=[&](const DevicePacket48Y& packet,const DeviceButtonDescriptors48Y& descriptors,unsigned count,
                           std::uint32_t deviceID,const DeviceButtonBank48Y& previous,bool success){
        std::array<original48y::Descriptor,32> ds{};for(unsigned i=0;i<32;++i)ds[i]={descriptors.records[i].flags,descriptors.records[i].packetOffset,descriptors.records[i].digitalMask};
        std::array<std::uint8_t,32> bytes{};std::memcpy(bytes.data(),packet.bytes,32);original48y::Stats stats{};
        const auto expected=original48y::sample(originalBank(previous),ds,count,deviceID,bytes,success,&stats);
        DeviceButtonSample48Y got{};require(sampleDeviceButtons48Y(true,success,packet,descriptors,count,deviceID,previous,got),"valid packet sample rejected");
        for(unsigned i=0;i<32;++i)require(std::memcmp(&got.buttons.records[i],expected.records[i].words.data(),24)==0,"sample state/metadata differential");
        require(got.transitionCount==expected.eventCount,"transition count differential");
        for(unsigned i=0;i<got.transitionCount;++i)require(std::memcmp(&got.transitions[i],expected.events[i].data(),20)==0,"ordered event differential");
        require((got.status==DevicePacketStatus48Y::Accepted)==expected.packetAccepted,"packet acceptance differential");
        require(expected.axisUpdateCount==6,"original axis-hook coverage");
        for(unsigned i=0;i<6;++i){require(expected.axisUpdates[i].axis==i&&expected.axisUpdates[i].denominator==255,"original axis update protocol");}
        ++samples;record(stats);return got;
    };
    std::uint32_t random=0x48f14360u;auto next=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
    DeviceButtonBank48Y bank{};for(unsigned i=0;i<32;++i){auto words=std::array<std::uint32_t,6>{{i%3,0x3f000000u+i,0x100+i,0x200+i,0x300+i,0x400+i}};std::memcpy(&bank.records[i],words.data(),24);}
    try{
        const auto configured=initialDualShockDescriptors48Y();
        require(dualShockActiveButtons48Y==16,"original PSX active count");
        for(unsigned i=0;i<32;++i){require(configured.records[i].digitalMask==(i<16?rr_device_profile48y[i][2]:0),"original descriptor mask");require(configured.records[i].packetOffset==(i<16?rr_device_profile48y[i][1]:0),"original pressure offset");require(configured.records[i].flags==(i<16?rr_device_profile48y[i][0]:0),"original descriptor flags");}
        for(std::uint32_t mask:{0u,0x80000000u,0x3fu,1u,0x40u,0x41u,0x80u,0xc0u}) {
            compareQuery(bank,1,mask,{},true);
            compareQuery(bank,2,mask,{},false);
        }
        for(unsigned rootFlags=0;rootFlags<256;++rootFlags)for(std::uint32_t mask:{0u,1u,2u,0x3fu,0x40u,0x41u,0x80u,0xc0u,0x80000000u,0xffffffffu})for(unsigned mode=0;mode<3;++mode){
            compareQuery(bank,1,mask,{true,static_cast<std::uint8_t>(rootFlags),mode,rootFlags},true);
            compareQuery(bank,2,mask,{true,static_cast<std::uint8_t>(rootFlags),mode,rootFlags},false);
        }
        for(unsigned i=0;i<12000;++i){const auto code=static_cast<int>(next()%32);const std::uint32_t mask=(i%4)?next():0x3fu;compareQuery(bank,code,mask,{(i&1)!=0,static_cast<std::uint8_t>(next()),next()%3,next()},i&2);}
        DeviceButtonDescriptors48Y descriptors{};for(unsigned i=0;i<32;++i)descriptors.records[i]={static_cast<std::uint8_t>(i&1?2:0),8u+i%24,static_cast<std::uint16_t>(1u<<(i%16))};
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}){
            require(std::fesetround(rounding)==0,"rounding setup");
            for(unsigned n=0;n<3000;++n){
                DevicePacket48Y packet{};for(auto& b:packet.bytes)b=static_cast<std::uint8_t>(next());packet.bytes[0]=(n%13)?0:1;
                DeviceButtonBank48Y previous=bank;for(unsigned i=0;i<32;++i)previous.records[i].held=next()%3;
                const unsigned count=n%33;const auto deviceID=next();const bool succeeded=n%17!=0;
                const auto got=compareSample(packet,descriptors,count,deviceID,previous,succeeded);
                if(got.status==DevicePacketStatus48Y::Accepted&&n%3==0){
                    const PlayerReadContext48X player{{},1,0,0,1,static_cast<std::uint8_t>(n),static_cast<std::uint8_t>(n>>2)};
                    const PlayerActionCodes48X codes{n&1?2:-1,n&2?3:-1,4,5,6};
                    const ControlBindingInput48W binding{{},100,n%5,false,(n&1)!=0,0,0,0,0,0};
                    const ControlBindingParameters48W params{0,1,200,.25f,.5f,.25f};const ControlModelBindings48W oldBindings{};
                    auto refBank=originalBank(got.buttons);ReferenceProvider refProvider{&refBank,{}};
                    PlayerControlStep48X expected{};require(stepDualShockPlayerControls48X({packet.bytes[4],packet.bytes[5],packet.bytes[6],packet.bytes[7]},player,codes,{&refProvider,ReferenceProvider::query},0,{}, {4,3},.01f,binding,params,oldBindings,expected),"original-query bridge rejected");
                    HeldPlayerStep48Y native{};require(stepReadyHeldPlayerControls48Y(packet,descriptors,count,deviceID,previous,{},player,codes,0,{}, {4,3},.01f,binding,params,oldBindings,native),"held-player composition rejected");
                    require(std::memcmp(&native.device,&got,sizeof got)==0,"connected device result mismatch");
                    require(std::memcmp(&native.player.bound,&expected.bound,sizeof expected.bound)==0,"connected numeric result mismatch");
                    require(native.player.player.flagsB9C==expected.player.flagsB9C,"connected player flags mismatch");
                    ++connected;
                }
            }
            // Exhaust pressure values with digital bits also set: pressure route wins.
            for(unsigned pressure=0;pressure<256;++pressure){DevicePacket48Y packet{};packet.bytes[8]=static_cast<std::uint8_t>(pressure);DeviceButtonDescriptors48Y ds{};ds.records[0]={2,8,0xffff};const auto got=compareSample(packet,ds,1,7,bank,true);require(got.buttons.records[0].held==(pressure>=26?1u:0u),"pressure threshold golden");}
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
        // Record float payload is copied bitwise, even when not an arithmetic input.
        for(std::uint32_t payload:{0x80000000u,0x7fc12345u,0x7f812345u,0xff800000u}){bank.records[1].held=1;std::memcpy(&bank.records[1].value,&payload,4);compareQuery(bank,1,0x3f,{},true);}
        const DeviceButtonRecord48Y sentry{9,1,2,3,4,5};
        for(int code:{-1,-32,32,std::numeric_limits<int>::min(),std::numeric_limits<int>::max()}){
            bool matched=true;auto copy=sentry;
            require(!queryDeviceHeld48Y(bank,code,0x3f,{},matched,&copy),"unsupported direct code accepted");
            require(matched&&std::memcmp(&copy,&sentry,24)==0,"unsupported query mutation");
            require(queryDeviceHeld48Y(bank,code,1,{},matched,&copy)&&!matched,"gate denial must precede code access");
            require(std::memcmp(&copy,&sentry,24)==0,"denied query copy mutation");
        }
        bool matched=false;auto aliasBank=bank;require(queryDeviceHeld48Y(aliasBank,1,0x3f,{},matched,&aliasBank.records[2])&&matched,"query typed alias failure");require(std::memcmp(&aliasBank.records[1],&aliasBank.records[2],24)==0,"query alias copy mismatch");
        DeviceHeldProvider48Y provider{&bank,{}};require(queryHeldPlayerAction48Y(&provider,-1,0x3f)==PlayerActionResult48X::Unavailable,"provider negative-code boundary");require(queryHeldPlayerAction48Y(nullptr,1,0x3f)==PlayerActionResult48X::Unavailable,"null provider accepted");
        DevicePacket48Y packet{};DeviceButtonSample48Y result{};result.buttons=bank;const auto originalResult=result;
        require(sampleDeviceButtons48Y(false,true,packet,descriptors,16,1,result.buttons,result),"not-ready rejected");require(std::memcmp(&result.buttons,&bank,sizeof bank)==0&&result.transitionCount==0&&result.status==DevicePacketStatus48Y::NotReady,"not-ready state changed");
        const auto failed=compareSample(packet,descriptors,16,1,bank,false);require(failed.transitionCount==0&&failed.status==DevicePacketStatus48Y::ReadFailed,"failure queued releases");
        auto invalid=descriptors;invalid.records[3]={2,32,0};result=originalResult;
        require(!sampleDeviceButtons48Y(true,true,packet,invalid,16,1,result.buttons,result),"invalid pressure offset accepted");require(std::memcmp(&result,&originalResult,sizeof result)==0,"late descriptor failure mutation");
        require(!sampleDeviceButtons48Y(true,true,packet,descriptors,33,1,bank,result),"oversized active count accepted");require(std::memcmp(&result,&originalResult,sizeof result)==0,"count error mutation");
        const auto expectedAlias=compareSample(packet,descriptors,16,1,bank,true);result=originalResult;
        require(sampleDeviceButtons48Y(true,true,packet,descriptors,16,1,result.buttons,result),"sample alias rejected");require(std::memcmp(&result,&expectedAlias,sizeof result)==0,"sample alias mismatch");
        const PlayerReadContext48X player{{},1,0,0,1,2,0xff};const PlayerActionCodes48X codes{2,3,4,5,6};
        const ControlBindingInput48W binding{{},100,0,false,true,0,0,0,0,0};const ControlBindingParameters48W params{0,1,200,.25f,.5f,.25f};
        HeldPlayerStep48Y state{};state.device.buttons=bank;
        require(stepReadyHeldPlayerControls48Y(packet,descriptors,16,1,state.device.buttons,{},player,codes,0,{}, {4,3},.01f,binding,params,{},state),"connected seed failed");
        const auto saved=state;auto badCodes=codes;badCodes.flag6B4=-1;
        require(!stepReadyHeldPlayerControls48Y(packet,descriptors,16,1,state.device.buttons,{},player,badCodes,state.player.bound.control.shaping.smoothedChannel0D28,state.player.bound.control.filtered,{4,3},.01f,binding,params,state.player.bound.bindings,state),"negative flag query fabricated");require(std::memcmp(&state,&saved,sizeof state)==0,"connected query failure mutation");
        const auto before=allocations;
        for(unsigned n=0;n<20000;++n){packet.bytes[2]=static_cast<std::uint8_t>(n);packet.bytes[3]=static_cast<std::uint8_t>(n>>8);require(stepReadyHeldPlayerControls48Y(packet,descriptors,16,1,state.device.buttons,{},player,codes,state.player.bound.control.shaping.smoothedChannel0D28,state.player.bound.control.filtered,{4,3},.01f,binding,params,state.player.bound.bindings,state),"hot input step failed");}
        require(allocations==before,"hot device input allocation");
        unsigned qv=0,gv=0,pv=0,ev=0;for(auto x:queryVisits)qv+=x!=0;for(auto x:gateVisits)gv+=x!=0;for(auto x:sampleVisits)pv+=x!=0;for(auto x:queueVisits)ev+=x!=0;
        require(qv==61&&gv==48&&pv==179&&ev==35,"reachable original word coverage gap");
        std::cout<<"device_held48y: "<<checks<<" checks; "<<queries<<" queries; "<<samples<<" samples; "<<connected<<" connected frames; "<<instructions<<" instructions; "<<events<<" original queue emissions; coverage "<<qv<<"/61 "<<gv<<"/49 "<<pv<<"/188 "<<ev<<"/35; zero hot allocations; results "<<sizeof(DeviceButtonSample48Y)<<'/'<<sizeof(HeldPlayerStep48Y)<<" bytes\n";
        for(unsigned i=0;i<61;++i)if(!queryVisits[i])std::cout<<"unvisited query "<<std::hex<<0x143600+4*i<<std::dec<<'\n';
        for(unsigned i=0;i<49;++i)if(!gateVisits[i])std::cout<<"unvisited gate "<<std::hex<<0x143e30+4*i<<std::dec<<'\n';
        for(unsigned i=0;i<188;++i)if(!sampleVisits[i])std::cout<<"unvisited packet "<<std::hex<<0x237ee0+4*i<<std::dec<<'\n';
        return 0;
    }catch(const std::exception& e){std::fesetround(FE_TONEAREST);std::cerr<<"device_held48y after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}
}
