#include "../../src/game/control_presets49d.h"
#include "../../tools/gameplay/control_presets_oracle49d.h"
#include "../../tools/gameplay/device_held_oracle48y.h"
#include "../../tools/gameplay/player_input_oracle48x.h"
#include "../../tools/gameplay/control_binding_oracle48w.h"
#include <array>
#include <cfenv>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <type_traits>

// Count successful-path C++ allocation attempts only. This test does not inject
// allocation failures, exercise allocation ownership, or measure process RAM.
static std::uint64_t allocations=0;
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}

using namespace rr::game;
using u32=std::uint32_t;
using original48w::bits;
using original48w::fp;
static std::uint64_t checks=0,selectorCases=0,defaultCases=0,previewCases=0;
static std::uint64_t resolutionCases=0,eventCases=0,integrationCases=0,rejectedCases=0;
static std::uint64_t instructions49d=0,instructions48y=0,instructions48x=0,instructions48w=0;
static std::array<std::uint64_t,225> selectorVisits{};
static std::array<std::uint64_t,37> defaultVisits{};
static std::array<std::uint64_t,64> previewVisits{};
static std::array<std::uint64_t,8> integratedPresets{};
static std::array<std::uint64_t,4> integratedIndices{},integratedModes{};
static std::uint64_t negativeActionFrames=0,heldActionFrames=0,axisFrames=0;
static void require(bool value,const char* label){++checks;if(!value)throw std::runtime_error(label);}
template<class T> static auto bytes(const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"test byte snapshot requires trivial object");
    std::array<unsigned char,sizeof(T)> result{};
    std::memcpy(result.data(),&value,sizeof value);return result;
}
static std::int32_t signedWord(u32 value){std::int32_t result;std::memcpy(&result,&value,4);return result;}
static original49d::Row rowWords(const ControlActionRow49D& row){
    return {{u32(row.action690),u32(row.action694),u32(row.flag698),u32(row.flag69C),
        u32(row.action6A0),u32(row.action6A4),u32(row.action6A8),u32(row.action6AC),
        u32(row.action6B0),u32(row.flag6B4)}};
}
static ControlActionRow49D nativeRow(const original49d::Row& row){
    return {signedWord(row[0]),signedWord(row[1]),signedWord(row[2]),signedWord(row[3]),
        signedWord(row[4]),signedWord(row[5]),signedWord(row[6]),signedWord(row[7]),
        signedWord(row[8]),signedWord(row[9])};
}
static original49d::State referenceState(const ControlPresets49D& native){
    original49d::State s{};
    for(unsigned i=0;i<2;++i){const auto& n=native.slots[i];
        s.rows[i]=rowWords(n.actions);s.presets[i]=n.preset11048;s.modes[i]=n.configuration11040;
        s.field10F94[i]=n.field10F94;s.field10F9C[i]=n.field10F9C;s.field10FA4[i]=n.field10FA4;
        s.field11030[i]=n.field11030;s.field11038[i]=n.field11038;
    }return s;
}
static void sameState(const ControlPresets49D& native,const original49d::State& expected){
    const auto got=referenceState(native);
    require(got.rows==expected.rows,"49D complete row differential");
    require(got.presets==expected.presets,"49D saved raw preset differential");
    require(got.modes==expected.modes,"49D configuration word differential");
    require(got.field10F94==expected.field10F94&&got.field10F9C==expected.field10F9C&&
        got.field10FA4==expected.field10FA4&&got.field11030==expected.field11030&&
        got.field11038==expected.field11038,"49D five additional default fields differential");
}
static void accumulate(const original49d::Stats& s){
    instructions49d+=s.steps;
    for(unsigned i=0;i<selectorVisits.size();++i)selectorVisits[i]+=s.visits(0x188880+4*i);
    for(unsigned i=0;i<defaultVisits.size();++i)defaultVisits[i]+=s.visits(0x1864f8+4*i);
    for(unsigned i=0;i<previewVisits.size();++i)previewVisits[i]+=s.visits(0x188c10+4*i);
}
static u32 randomWord=0x49d18888u;
static u32 next(){randomWord^=randomWord<<13;randomWord^=randomWord>>17;randomWord^=randomWord<<5;return randomWord;}
static ControlPresets49D sentry(){
    ControlPresets49D state{};
    for(auto& slot:state.slots){original49d::Row row{};for(auto& x:row)x=next();
        slot={nativeRow(row),next(),next(),next(),next(),next(),next(),next()};
    }return state;
}
static constexpr std::array<u32,15> presetWords{{0,1,2,3,4,5,6,7,8,9,255,0x7fffffffu,0x80000000u,0xfffffffeu,0xffffffffu}};
static constexpr std::array<u32,5> invalidSlots{{2,3,0x7fffffffu,0x80000000u,0xffffffffu}};
static bool configAddress(u32 address){
    const auto base=original49d::configBase;
    for(u32 slot=0;slot<2;++slot){
        if(address>=base+0x690+40*slot&&address<base+0x6b8+40*slot)return true;
        for(u32 offset:{0x11048u,0x11040u,0x10f94u,0x10f9cu,0x10fa4u,0x11030u,0x11038u})
            if(address==base+offset+4*slot)return true;
    }return false;
}
using Write=std::array<u32,2>;
struct Writes{std::array<Write,64> data{};unsigned count=0;};
static Writes configWrites(const original49d::Stats& stats){
    Writes result{};
    for(unsigned i=0;i<stats.memoryCount;++i){const auto& m=stats.memory[i];
        if(m.write&&configAddress(m.address)){require(result.count<result.data.size(),"49D fixture write bound");result.data[result.count++]={{m.address,m.value}};}
    }return result;
}
static void appendSelectorWrites(Writes& out,u32 slot,u32 preset,const original49d::State& after){
    const auto base=original49d::configBase;
    out.data[out.count++]={{base+0x11048+4*slot,preset}};
    if(preset>=8)return;
    out.data[out.count++]={{base+0x11040+4*slot,after.modes[slot]}};
    for(unsigned index:{1u,0u,6u,2u,3u,7u,4u,5u,8u,9u})
        out.data[out.count++]={{base+0x690+40*slot+4*index,after.rows[slot][index]}};
}
static void compareWrites(const Writes& a,const Writes& b){
    require(a.count==b.count,"49D original configuration store count");
    for(unsigned i=0;i<a.count;++i)require(a.data[i]==b.data[i],"49D original ordered configuration stores");
}
static void compareSelect(ControlPresets49D& state,u32 slot,u32 preset){
    const auto untouched=bytes(state.slots[1-slot]);auto expected=referenceState(state);original49d::Stats stats{};
    original49d::select(expected,slot,preset,&stats);
    require(selectControlPreset49D(state,slot,preset),"supported49D slot rejected");sameState(state,expected);
    require(bytes(state.slots[1-slot])==untouched,"selector changed other slot");
    require(stats.selectorCalls==1,"selector fixture must execute one original setter");
    Writes wanted{};appendSelectorWrites(wanted,slot,preset,expected);compareWrites(configWrites(stats),wanted);
    accumulate(stats);++selectorCases;
}
static void testSelectorsDefaultsPreviews(){
    static_assert(sizeof(ControlPresets49D)==136&&sizeof(ControlActionRow49D)==40,"49D bounded fixed storage");
    static_assert(sizeof(PlayerEventPresetRows49D)==8,"49A exactly two action6AC words");
    require(controlPresetSlots49D==2,"49D local default-loop slot domain");
    for(u32 slot=0;slot<2;++slot)for(u32 previous:presetWords)for(u32 requested:presetWords){
        auto state=sentry();state.slots[slot].preset11048=previous;compareSelect(state,slot,requested);
    }
    auto evolving=sentry();
    for(unsigned i=0;i<2048;++i)compareSelect(evolving,next()&1u,i%3?next()%8:next());
    for(u32 slot:invalidSlots)for(u32 preset:presetWords){auto state=sentry();const auto before=bytes(state);
        require(!selectControlPreset49D(state,slot,preset),"unsupported49D slot accepted");
        require(bytes(state)==before,"invalid slot changed state");++rejectedCases;
    }
    for(unsigned n=0;n<128;++n){
        auto state=sentry();auto expected=referenceState(state);original49d::Stats stats{};
        original49d::defaults(expected,&stats);initializeControlPresets49D(state);sameState(state,expected);
        require(stats.selectorCalls==2,"default loop must execute original selector exactly twice");
        Writes wanted{};const auto base=original49d::configBase;
        for(u32 slot=0;slot<2;++slot){
            wanted.data[wanted.count++]={{base+0x10f94+4*slot,7}};
            wanted.data[wanted.count++]={{base+0x10f9c+4*slot,7}};
            wanted.data[wanted.count++]={{base+0x10fa4+4*slot,1}};
            wanted.data[wanted.count++]={{base+0x11040+4*slot,0}};
            wanted.data[wanted.count++]={{base+0x11048+4*slot,0}};
            appendSelectorWrites(wanted,slot,0,expected);
            wanted.data[wanted.count++]={{base+0x11030+4*slot,5}};
            wanted.data[wanted.count++]={{base+0x11038+4*slot,0}};
            const auto& s=state.slots[slot];
            require(s.preset11048==0&&s.configuration11040==0&&s.field10F94==7&&s.field10F9C==7&&
                s.field10FA4==1&&s.field11030==5&&s.field11038==0,"complete default-loop golden fields");
        }
        compareWrites(configWrites(stats),wanted);accumulate(stats);++defaultCases;
        const auto initialized=bytes(state);initializeControlPresets49D(state);require(bytes(state)==initialized,"defaults not idempotent");
    }
    for(u32 saved:presetWords)for(u32 requested:presetWords)for(unsigned canonical=0;canonical<2;++canonical){
        auto state=sentry();
        if(canonical){require(selectControlPreset49D(state,0,saved%8),"preview seed rejected");}
        else {
            const original49d::Row payload{{0x7fc01234u,0x7f800001u,0x80000000u,0xffffffffu,
                0x7f800000u,0xff800000u,0x00000001u,0x80000001u,0xdeadbeefu,0x01234567u}};
            state.slots[0].actions=nativeRow(payload);state.slots[0].configuration11040=0x80000000u;
        }
        state.slots[0].preset11048=saved;
        const auto before=referenceState(state);const auto other=bytes(state.slots[1]);
        auto expected=before;original49d::Stats stats{};const auto row=original49d::getter(expected,requested,&stats);
        const auto result=previewControlPreset49D(state,requested);
        require(rowWords(result)==row,"preview exact40-byte row differential");sameState(state,expected);
        require(bytes(state.slots[1])==other,"preview changed slot1");
        require(state.slots[0].preset11048==saved&&stats.selectorCalls==2,"preview saved index/two real setters");
        if(requested>=8)require(row==before.rows[0],"invalid preview must return prior row payload exactly");
        if(saved>=8){
            require(rowWords(state.slots[0].actions)==row,"saved-invalid preview must retain requested row");
            require(state.slots[0].configuration11040==(requested<8?u32(requested>=4):before.modes[0]),
                "saved-invalid preview must retain requested/prior mode");
        }else{
            auto canonicalState=before;original49d::select(canonicalState,0,saved);
            require(rowWords(state.slots[0].actions)==canonicalState.rows[0]&&state.slots[0].configuration11040==canonicalState.modes[0],
                "saved-valid preview reconstructs canonical row rather than custom row");
        }
        auto intermediate=before;original49d::select(intermediate,0,requested);
        Writes wanted{};appendSelectorWrites(wanted,0,requested,intermediate);appendSelectorWrites(wanted,0,saved,expected);
        compareWrites(configWrites(stats),wanted);accumulate(stats);++previewCases;
    }
}

static void testResolution(){
    for(u32 first=0;first<8;++first)for(u32 second=0;second<8;++second){
        auto state=sentry();compareSelect(state,0,first);compareSelect(state,1,second);
        const auto expected=referenceState(state);const auto before=bytes(state);
        for(u32 action=0;action<2;++action)for(u32 configuration=0;configuration<2;++configuration){
            SelectedControlPreset49D out{};require(resolveControlPreset49D(state,action,configuration,out),"valid preset resolution rejected");
            const auto& r=expected.rows[action];
            require(out.actions.action694==signedWord(r[1])&&out.actions.action690==signedWord(r[0])&&
                out.actions.flag698==signedWord(r[2])&&out.actions.flag69C==signedWord(r[3])&&out.actions.flag6B4==signedWord(r[9]),
                "+FFC must select48X action row and correct numeric offsets");
            require(out.eventAction.action6AC==signedWord(r[7]),"+FFC must select49A action6AC");
            require(out.configurationEntryNonzero==(expected.modes[configuration]!=0),"+5C configuration selector conflated with+FFC");
            ++resolutionCases;
        }
        require(bytes(state)==before,"resolution wrote configuration state");
    }
    auto state=sentry();
    state.slots[0].actions={11,12,13,14,15,16,17,18,19,20};
    state.slots[1].actions={21,22,23,24,25,26,27,28,29,30};
    for(u32 mode:{0u,1u,2u,0x80000000u,0xffffffffu}){
        state.slots[0].configuration11040=0;state.slots[1].configuration11040=mode;
        for(u32 action=0;action<2;++action)for(u32 configuration=0;configuration<2;++configuration){
            SelectedControlPreset49D out{};const auto row=rowWords(state.slots[action].actions);
            require(resolveControlPreset49D(state,action,configuration,out),"custom action-row resolution failed");
            require(out.configurationEntryNonzero==(configuration==1&&mode!=0),"configuration raw nonzero normalization");
            require(out.actions.action694==signedWord(row[1])&&out.actions.action690==signedWord(row[0])&&
                out.actions.flag698==signedWord(row[2])&&out.actions.flag69C==signedWord(row[3])&&out.actions.flag6B4==signedWord(row[9])&&
                out.eventAction.action6AC==signedWord(row[7]),"custom distinct selected action offsets/index mismatch");
            ++resolutionCases;
        }
    }
    for(u32 invalid:invalidSlots)for(unsigned which=0;which<3;++which){
        SelectedControlPreset49D out{{-17,19,-23,29,-31},{37},true};const auto before=bytes(out);const auto stateBefore=bytes(state);
        require(!resolveControlPreset49D(state,which==1?0:invalid,which==0?1:invalid,out),"invalid independent index accepted");
        require(bytes(out)==before&&bytes(state)==stateBefore,"invalid resolution must preserve output/state");++rejectedCases;
    }
}

struct EventEffectFixture{
    unsigned calls=0;bool expected=true;
    static bool apply(void* opaque,const PlayerEffectCall49A& call) noexcept{
        auto& f=*static_cast<EventEffectFixture*>(opaque);++f.calls;
        f.expected=f.expected&&call.kind==PlayerEffectKind49A::Function2255B0;
        return true;
    }
};
static void testEventRows(){
    ControlPresets49D state{};initializeControlPresets49D(state);
    for(u32 preset=0;preset<8;++preset){require(selectControlPreset49D(state,preset&1,preset),"event preset selection rejected");
        const auto rows=playerEventPresetRows49D(state);
        require(rows.rows[0].action6AC==8&&rows.rows[1].action6AC==8,"original presets retain exact common49A code8");
    }
    // Distinct caller-supplied rows make index selection observable. All real
    // presets use code8, which alone would not detect accidentally taking slot0.
    state.slots[0].actions.action6AC=3;state.slots[1].actions.action6AC=17;
    auto rows=playerEventPresetRows49D(state);const auto savedRows=rows;
    require(selectControlPreset49D(state,0,4)&&selectControlPreset49D(state,1,7),"event snapshot reselection rejected");
    require(bytes(rows)==bytes(savedRows),"returned event row snapshot changed without caller refresh");
    DeviceButtonBank48Y bank0{},bank1{};bank0.records[3].held=1;bank1.records[17].held=1;
    for(u32 action=0;action<2;++action)for(u32 source=0;source<2;++source){
        PlayerEventEnvironment49A env{};env.held.devices[0]=&bank0;env.held.devices[1]=&bank1;
        env.actionRows=rows.rows;env.actionRowCount=2;env.gate58=2;
        EventEffectFixture effect{};PlayerEventBindings49A bindings{};
        bindings.effects={&effect,EventEffectFixture::apply};bindings.children.firstChild=noEventChild48Z;
        PlayerEventState49A player{0,source,action,-1,19,23,29,31,37,41};
        InputEvent48Z event{0xfedcba98u,2,7,1,0x3f};DeviceButtonRecord48Y record{};
        const auto result=handlePlayerPress49A(player,env,bindings,event,record);
        require(result.status==PlayerEventStatus49A::Complete&&result.queriesCompleted==3,"caller-owned two-row49A environment rejected");
        require(effect.expected&&effect.calls==u32(action==source)&&result.effectsInvoked==effect.calls,
            "49A action+FFC must stay independent of device source+5C/event deviceId");
        require(bits(player.value1094)==bits(action==source?2.5f:19.f)&&bits(player.value1090)==bits(action==source?-1.f:23.f),
            "49A concrete held action did not use caller-selected6AC");++eventCases;
    }
    rows=playerEventPresetRows49D(state);require(rows.rows[0].action6AC==8&&rows.rows[1].action6AC==8,"explicit event row refresh mismatch");
    PlayerEventEnvironment49A env{};env.actionRows=rows.rows;env.actionRowCount=2;
    PlayerEventBindings49A bindings{};bindings.children.firstChild=noEventChild48Z;
    for(u32 index:invalidSlots){PlayerEventState49A p{};p.indexFFC=index;const auto before=bytes(p);
        InputEvent48Z event{};DeviceButtonRecord48Y record{};const auto result=handlePlayerPress49A(p,env,bindings,event,record);
        require(result.status==PlayerEventStatus49A::InvalidActionRow&&result.queriesCompleted==0&&bytes(p)==before,"49A caller row count not bounded to2");++rejectedCases;
    }
}

struct Frame{
    DevicePacket48Y packet{};
    DeviceButtonDescriptors48Y descriptors=initialDualShockDescriptors48Y();
    u32 count=16,deviceId=7;
    DeviceButtonBank48Y previous{};
    DeviceQueryContext48Y query{};
    PlayerReadContext48X reader{{{99,98,97,96}},1,-.25f,0,0,2,0xf8};
    float previousD28=.375f;
    ControlChannels48B filtered{{.125f,-.25f,.375f,-.5f}};
    ControlRateLimits48B rates{4,3};float elapsed=.03125f;
    ControlBindingInput48W binding{{{99,98,97,96}},100,0,false,false,99,98,0,97,96};
    ControlBindingParameters48W parameters{0,1,200,.25f,.5f,.25f};
    ControlModelBindings48W bindings{{.125f,.25f},{.375f,.5f},.625f,7,.75f,.875f,1};
};
static Frame frame(unsigned variant){
    Frame f;
    constexpr std::array<std::uint8_t,16> axis{{0,255,64,192,101,102,103,127,128,152,153,154,1,254,26,25}};
    f.packet.bytes[2]=static_cast<std::uint8_t>(variant*37u);f.packet.bytes[3]=static_cast<std::uint8_t>(variant*91u);
    for(unsigned i=4;i<8;++i)f.packet.bytes[i]=axis[(variant+i*3)%axis.size()];
    for(unsigned i=8;i<32;++i)f.packet.bytes[i]=static_cast<std::uint8_t>((variant*53u+i*19u)%256);
    // Include none/all held and both sides of the original pressure threshold.
    if(variant%8<4){const std::uint8_t pressure=variant%8==0?0:variant%8==1?25:variant%8==2?26:255;
        for(unsigned i=8;i<32;++i)f.packet.bytes[i]=pressure;
        f.packet.bytes[2]=f.packet.bytes[3]=variant%8<2?255:0;
    }
    for(unsigned i=0;i<32;++i)f.previous.records[i]={u32((variant+i)%3),float(i)/32,0x100+i,0x200+i,0x300+i,0x400+i};
    f.query={bool(variant&1),static_cast<std::uint8_t>(variant*11),variant%3,variant*0x10201u};
    f.reader.gateByte58=static_cast<std::uint8_t>(variant%4==0?0:variant%4==1?1:variant%4==2?2:0xfe);
    f.reader.previousFlagsB9C=static_cast<std::uint8_t>(variant*17u);
    f.reader.source7D0=variant%4==0?0.f:variant%4==1?-.25f:.5f;
    f.reader.source7D4=variant%3==0?.25f:-.5f;f.reader.source7D8=variant%3==1?.75f:0;f.reader.source7DC=variant%3==2?.125f:0;
    f.binding.vehicleState40=variant%7;f.binding.bodyCachedSpeed34=float(variant%300);
    f.deviceId=variant%3==0?0xffffffffu:variant%3==1?7u:2u;
    return f;
}
static bool step(const ControlPresets49D& state,u32 action,u32 configuration,const Frame& f,HeldPlayerStep48Y& out){
    return stepReadyPresetPlayerControls49D(state,action,configuration,f.packet,f.descriptors,f.count,f.deviceId,
        f.previous,f.query,f.reader,f.previousD28,f.filtered,f.rates,f.elapsed,f.binding,f.parameters,f.bindings,out);
}
static void samePlayer(const PlayerControlStep48X& got,const PlayerControlStep48X& expected){
    for(unsigned i=0;i<4;++i){require(bits(got.components.values[i])==bits(expected.components.values[i]),"integrated packet axes differential");
        require(bits(got.player.raw.values[i])==bits(expected.player.raw.values[i]),"integrated held override differential");}
    require(bits(got.player.actionB7C)==bits(expected.player.actionB7C)&&bits(got.player.actionB80)==bits(expected.player.actionB80),"integrated action values differential");
    require(got.player.flagsB9C==expected.player.flagsB9C,"integrated action flags differential");
    require(bytes(got.bound.control)==bytes(expected.bound.control),"integrated shaping/filter binary32 differential");
    require(bytes(got.bound.bindings)==bytes(expected.bound.bindings),"integrated engine/aero binding binary32 differential");
}
static void compareIntegration(const ControlPresets49D& state,const original49d::State& ref,u32 action,u32 configuration,const Frame& f){
    const auto stateBefore=bytes(state);const auto frameBefore=bytes(f);HeldPlayerStep48Y got{},direct{};
    require(step(state,action,configuration,f,got),"valid preset-player frame rejected");
    const auto& row=ref.rows[action];
    const PlayerActionCodes48X codes{signedWord(row[1]),signedWord(row[0]),signedWord(row[2]),signedWord(row[3]),signedWord(row[9])};
    auto binding=f.binding;binding.configurationEntryNonzero=ref.modes[configuration]!=0;
    require(stepReadyHeldPlayerControls48Y(f.packet,f.descriptors,f.count,f.deviceId,f.previous,f.query,f.reader,codes,
        f.previousD28,f.filtered,f.rates,f.elapsed,binding,f.parameters,f.bindings,direct),"direct48Y independent-row reference rejected");
    require(bytes(got.device)==bytes(direct.device),"49D wrapper direct48Y packet state/events mismatch");samePlayer(got.player,direct.player);
    require(bytes(state)==stateBefore&&bytes(f)==frameBefore,"preset-player read-only input changed");

    // Fully independent value source:49D selector words ->48Y sample/query words
    // ->48X authored axes/reader words ->48V/48W shaping/filter/model words.
    std::array<original48y::Record,32> bank{};std::array<original48y::Descriptor,32> descriptors{};
    std::array<std::uint8_t,32> packet{};
    for(unsigned i=0;i<32;++i){std::memcpy(bank[i].words.data(),&f.previous.records[i],24);
        descriptors[i]={f.descriptors.records[i].flags,f.descriptors.records[i].packetOffset,f.descriptors.records[i].digitalMask};packet[i]=f.packet.bytes[i];}
    original48y::Stats sampleStats{};const auto sampled=original48y::sample(bank,descriptors,f.count,f.deviceId,packet,true,&sampleStats);instructions48y+=sampleStats.steps;
    require(sampled.packetAccepted&&got.device.status==DevicePacketStatus48Y::Accepted,"original packet acceptance mismatch");
    for(unsigned i=0;i<32;++i)require(std::memcmp(&got.device.buttons.records[i],sampled.records[i].words.data(),24)==0,"integrated original packet held/metadata mismatch");
    require(got.device.transitionCount==sampled.eventCount,"integrated original transition count mismatch");
    for(unsigned i=0;i<sampled.eventCount;++i)require(std::memcmp(&got.device.transitions[i],sampled.events[i].data(),20)==0,"integrated original transition order/deviceId mismatch");
    original48x::ReaderInput reader{};reader.codes={codes.action694,codes.action690,codes.flag698,codes.flag69C,codes.flag6B4};
    reader.deviceIndex=static_cast<int>(action);reader.gateByte=f.reader.gateByte58;reader.previousFlags=f.reader.previousFlagsB9C;
    reader.source7D0=f.reader.source7D0;reader.source7D4=f.reader.source7D4;reader.source7D8=f.reader.source7D8;reader.source7DC=f.reader.source7DC;
    const float numerator[4]={float(f.packet.bytes[6]),float(f.packet.bytes[7]),float(255u-f.packet.bytes[4]),float(f.packet.bytes[5])};
    PlayerControlStep48X expected{};
    for(unsigned i=0;i<4;++i){original48x::Stats stats{};reader.components[i]=original48x::axis(numerator[i],255,{-1,1,0,fp(0x3e4ccccdu),fp(0xbe4ccccdu)},&stats);
        expected.components.values[i]=reader.components[i];instructions48x+=stats.steps;}
    original48x::Stats readStats{};
    const auto read=original48x::reader(reader,[&](std::int32_t code,u32 mask){original48y::Stats stats{};
        const auto result=original48y::query(sampled.records,code,mask,{f.query.present,f.query.rootFlags228,f.query.context518,f.query.context1898},false,{},&stats);
        instructions48y+=stats.steps;return result.matched;},&readStats);instructions48x+=readStats.steps;
    for(unsigned i=0;i<4;++i)expected.player.raw.values[i]=read.raw[i];
    expected.player.actionB7C=read.actionB7C;expected.player.actionB80=read.actionB80;expected.player.flagsB9C=read.flags;
    binding.actionB7C=read.actionB7C;binding.actionB80=read.actionB80;binding.flagsB9C=read.flags;
    binding.source7D0=f.reader.source7D0;binding.source7D4=f.reader.source7D4;
    original48w::Machine machine;machine.prepareBinding(binding,f.parameters,f.bindings,configuration);
    expected.bound.control=machine.run(expected.player.raw,f.previousD28,binding.mode58Nonzero?1u:0u,f.filtered,f.rates,f.elapsed,3);
    expected.bound.bindings=machine.continueBinding();instructions48w+=machine.steps;samePlayer(got.player,expected);
    // The existing producer-input adapter receives the actual integrated model
    // values; it neither creates force effects nor invents a gameplay scheduler.
    EngineInput48D engine{{-.25f,-.5f},123,true,false,true};
    AeroInput48E aero{{{1,2,3}},{{4,5,6}},.75f,true,{{7,8,9}},true,false,true,.33f,.44f,.55f};
    EngineInput48D outEngine{};AeroInput48E outAero{};
    require(applyControlModelBindings48W(got.player.bound.bindings,engine,aero,outEngine,outAero),"integrated engine/aero producer-input adapter rejected");
    for(unsigned i=0;i<2;++i)require(bits(outEngine.drive[i])==bits(expected.bound.bindings.engineDrive[i]),"integrated engine drive mapping mismatch");
    require(bits(outAero.driveE4)==bits(expected.bound.bindings.aeroDrive[0])&&bits(outAero.driveE8)==bits(expected.bound.bindings.aeroDrive[1])&&
        bits(outAero.brakeEc)==bits(expected.bound.bindings.aeroBrakeEC)&&outAero.triggerD8==(expected.bound.bindings.aeroTriggerD8!=0),"integrated aero drive/brake/trigger mapping mismatch");
    require(bits(outEngine.bodySpeed)==bits(engine.bodySpeed)&&outEngine.boost==engine.boost&&outEngine.repair==engine.repair&&outEngine.fullTorque==engine.fullTorque,
        "preset adapter changed unrelated engine inputs");
    ++integrationCases;++integratedPresets[ref.presets[action]<8?ref.presets[action]:0];++integratedIndices[2*action+configuration];
    ++integratedModes[2*u32(f.binding.mode58Nonzero)+u32(ref.modes[configuration]!=0)];
    negativeActionFrames+=codes.action690==-1&&codes.action694==-1;
    heldActionFrames+=read.actionB7C!=0||read.actionB80!=0;
    axisFrames+=bits(read.raw[0])==bits(reader.components[0])&&read.raw[0]!=0;
}
static void testIntegration(){
    unsigned sequence=0;
    for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}){
        require(std::fesetround(rounding)==0,"integration rounding setup");
        for(u32 first=0;first<8;++first)for(u32 second=0;second<8;++second){
            auto state=sentry();auto ref=referenceState(state);
            require(selectControlPreset49D(state,0,first)&&selectControlPreset49D(state,1,second),"integration preset setup");
            original49d::select(ref,0,first);original49d::select(ref,1,second);
            for(u32 action=0;action<2;++action)for(u32 configuration=0;configuration<2;++configuration)for(unsigned mode=0;mode<2;++mode){
                auto f=frame(sequence++);f.binding.mode58Nonzero=mode!=0;
                // Opposite supplied configuration proves that49D replaces this
                // one field while leaving vehicle+58 mode independent.
                f.binding.configurationEntryNonzero=ref.modes[configuration]==0;
                compareIntegration(state,ref,action,configuration,f);
            }
        }
        // All axis-byte boundaries under every preset, plus explicit digital
        // and pressure action states and activeCount boundaries0/16/32.
        for(u32 preset=0;preset<8;++preset)for(unsigned variant=0;variant<32;++variant){
            ControlPresets49D state{};initializeControlPresets49D(state);auto ref=referenceState(state);
            require(selectControlPreset49D(state,1,preset),"boundary preset setup");original49d::select(ref,1,preset);
            auto f=frame(variant);f.binding.mode58Nonzero=(variant&1)!=0;f.count=variant%3==0?0:variant%3==1?16:32;
            compareIntegration(state,ref,1,variant&1,f);
        }
        // Saved-invalid preview keeps the requested row/mode in the real frame
        // path. It is intentionally not normalized back to preset0.
        auto state=sentry();state.slots[0].preset11048=0xffffffffu;auto ref=referenceState(state);
        const auto preview=previewControlPreset49D(state,6);const auto expected=original49d::getter(ref,6);
        require(rowWords(preview)==expected,"integrated invalid-saved preview differential");
        auto f=frame(18);compareIntegration(state,ref,0,0,f);
        for(u32 rawMode:{2u,0x80000000u,0xffffffffu}){
            state.slots[0].configuration11040=rawMode;ref.modes[0]=rawMode;
            f.binding.configurationEntryNonzero=false;compareIntegration(state,ref,0,0,f);
        }
    }
    require(std::fesetround(FE_TONEAREST)==0,"restore integration rounding");
    for(auto count:integratedPresets)require(count!=0,"preset never reached complete pipeline");
    for(auto count:integratedIndices)require(count!=0,"separate index combination never integrated");
    for(auto count:integratedModes)require(count!=0,"vehicle mode/configuration combination never integrated");
    require(negativeActionFrames&&heldActionFrames&&axisFrames,"missing negative-sentinel/held/axis integration branch");
}

static HeldPlayerStep48Y outputSentry(){
    HeldPlayerStep48Y s{};s.device.buttons.records[0]={0x12345678u,37,41,43,47,53};
    s.device.transitionCount=7;s.device.status=DevicePacketStatus48Y::ReadFailed;
    s.player.components={{59,61,67,71}};s.player.player.raw={{73,79,83,89}};
    s.player.player.actionB7C=97;s.player.player.actionB80=101;s.player.player.flagsB9C=0xa5;
    s.player.bound.bindings={{103,107},{109,113},127,131,137,139,149};return s;
}
static void rejected(const ControlPresets49D& state,u32 action,u32 configuration,const Frame& f){
    auto out=outputSentry();const auto before=bytes(out);const auto stateBefore=bytes(state);const auto frameBefore=bytes(f);
    const bool accepted=step(state,action,configuration,f,out);
    if(accepted)std::cerr<<"unexpected accepted rejection case "<<rejectedCases<<" action="<<action<<" configuration="<<configuration<<'\n';
    require(!accepted,"unsupported preset-player frame accepted");
    require(bytes(out)==before,"failed preset-player frame partially wrote output");
    require(bytes(state)==stateBefore&&bytes(f)==frameBefore,"failed preset-player frame wrote input");++rejectedCases;
}
static void testBoundariesAndAllocationCount(){
    ControlPresets49D state{};initializeControlPresets49D(state);auto nominal=frame(18);
    for(u32 slot:invalidSlots){rejected(state,slot,0,nominal);rejected(state,0,slot,nominal);rejected(state,slot,slot,nominal);}
    auto f=nominal;f.packet.bytes[0]=1;rejected(state,0,1,f);
    for(u32 count:{33u,0x80000000u,0xffffffffu}){f=nominal;f.count=count;rejected(state,0,1,f);}
    for(u32 offset:{32u,0x80000000u,0xffffffffu}){f=nominal;f.descriptors.records[15]={2,offset,0};rejected(state,0,1,f);}
    for(std::int32_t code:{-2,std::numeric_limits<std::int32_t>::min(),32,std::numeric_limits<std::int32_t>::max()})
        for(unsigned member=0;member<5;++member){auto bad=state;auto& row=bad.slots[1].actions;
            if(member==0)row.action690=code;else if(member==1)row.action694=code;else if(member==2)row.flag698=code;else if(member==3)row.flag69C=code;else row.flag6B4=code;
            rejected(bad,1,0,nominal);
        }
    // -1 is a proven skip only in action690/action694. Flag fields must query it.
    for(unsigned member=0;member<3;++member){auto bad=state;
        if(member==0)bad.slots[0].actions.flag698=-1;else if(member==1)bad.slots[0].actions.flag69C=-1;else bad.slots[0].actions.flag6B4=-1;
        rejected(bad,0,1,nominal);
        auto gated=nominal;gated.reader.gateByte58=0;HeldPlayerStep48Y out{};
        require(step(bad,0,1,gated,out),"unqueried negative flag field rejected before actual access");
    }
    for(float bad:{std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()})
        for(unsigned member=0;member<19;++member){f=nominal;
            if(member==0)f.reader.source7D0=bad;else if(member==1)f.reader.source7D4=bad;
            else if(member==2)f.reader.source7D8=bad;else if(member==3)f.reader.source7DC=bad;
            else if(member==4)f.previousD28=bad;else if(member<9)f.filtered.values[member-5]=bad;
            else if(member==9)f.rates.x=bad;else if(member==10)f.rates.y=bad;else if(member==11)f.elapsed=bad;
            else if(member==12)f.binding.bodyCachedSpeed34=bad;else if(member==13)f.parameters.speed1B84=bad;
            else if(member==14)f.parameters.speed2B8C=bad;else if(member==15)f.parameters.scale1B88=bad;
            else if(member==16)f.parameters.scale2B90=bad;else if(member==17)f.parameters.scale4346B8=bad;else f.bindings.aeroAux50=bad;
            rejected(state,0,1,f);
        }
    f=nominal;f.rates.x=-1;rejected(state,0,1,f);f=nominal;f.rates.y=-1;rejected(state,0,1,f);f=nominal;f.elapsed=-1;rejected(state,0,1,f);
    f=nominal;f.binding.vehicleState40=0;
    f.parameters.scale1B88=-std::numeric_limits<float>::max();f.parameters.scale2B90=std::numeric_limits<float>::max();rejected(state,0,1,f);
    // Fields replaced by the connected path are deliberately irrelevant, even
    // when the caller supplies non-finite payloads there.
    f=nominal;const float nan=std::numeric_limits<float>::quiet_NaN();
    for(auto& v:f.reader.componentValues.values)v=nan;
    for(auto& v:f.binding.filtered.values)v=nan;
    f.binding.actionB7C=f.binding.actionB80=f.binding.source7D0=f.binding.source7D4=nan;
    HeldPlayerStep48Y out{};require(step(state,0,1,f,out),"replaced frame fields incorrectly validated");
    f=nominal;require(step(state,0,1,f,out),"hot-loop seed failed");
    const auto before=allocations;
    for(unsigned n=0;n<20000;++n){
        if(n%101==0)initializeControlPresets49D(state);
        require(selectControlPreset49D(state,n&1,n%11),"hot selector failed");
        (void)previewControlPreset49D(state,n%13);SelectedControlPreset49D selection{};
        require(resolveControlPreset49D(state,n&1,(n>>1)&1,selection),"hot resolution failed");
        const auto rows=playerEventPresetRows49D(state);require(rows.rows[0].action6AC==8&&rows.rows[1].action6AC==8,"hot event rows mismatch");
        f.packet.bytes[4]=static_cast<std::uint8_t>(n);f.packet.bytes[7]=static_cast<std::uint8_t>(n>>3);
        f.binding.mode58Nonzero=(n&1)!=0;
        f.previous=out.device.buttons;f.previousD28=out.player.bound.control.shaping.smoothedChannel0D28;
        f.filtered=out.player.bound.control.filtered;f.bindings=out.player.bound.bindings;
        require(step(state,n&1,(n>>1)&1,f,out),"hot preset-player step failed");
    }
    require(allocations==before,"49D production hot path allocated");
}

int main(){
    try{
        testSelectorsDefaultsPreviews();testResolution();testEventRows();testIntegration();testBoundariesAndAllocationCount();
        unsigned selectors=0,defaults=0,previews=0;
        for(auto n:selectorVisits)selectors+=n!=0;
        for(auto n:defaultVisits)defaults+=n!=0;
        for(auto n:previewVisits)previews+=n!=0;
        require(selectors==225&&defaults==37&&previews==64,"49D reachable original instruction coverage gap");
        std::cout<<"{\"test\":\"control_presets49d\",\"status\":\"PASS\",\"checks\":"<<checks
            <<",\"cases\":{\"selector\":"<<selectorCases<<",\"defaults\":"<<defaultCases<<",\"preview\":"<<previewCases
            <<",\"resolution\":"<<resolutionCases<<",\"event_rows\":"<<eventCases<<",\"integration\":"<<integrationCases<<",\"rejected\":"<<rejectedCases
            <<"},\"original_instructions\":{\"49d\":"<<instructions49d<<",\"48y\":"<<instructions48y<<",\"48x\":"<<instructions48x<<",\"48w\":"<<instructions48w
            <<"},\"word_coverage\":{\"selector\":["<<selectors<<",225],\"defaults\":["<<defaults<<",37],\"preview\":["<<previews
            <<",64]},\"rounding_modes\":4,\"preset_rows\":8,\"index_pairs\":4,\"mode_pairs\":4,\"hot_iterations\":20000,\"hot_allocations\":0,\"state_bytes\":"<<sizeof(ControlPresets49D)<<"}\n";
        return 0;
    }catch(const std::exception& error){std::fesetround(FE_TONEAREST);std::cerr<<"control_presets49d after "<<checks<<" checks: "<<error.what()<<'\n';return 1;}
}
