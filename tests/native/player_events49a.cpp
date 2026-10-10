#include "../../src/game/player_events49a.h"
#include "../../tools/gameplay/device_held_oracle48y.h"
#include "../../tools/gameplay/player_event_oracle49a.h"
#include "../../tools/gameplay/event_dispatch_oracle48z.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>

static std::uint64_t allocations = 0;
void* operator new(std::size_t n) { ++allocations; if (void* p = std::malloc(n ? n : 1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
using namespace rr::game;
using u32 = std::uint32_t;
static std::uint64_t checks = 0, managerCases = 0, handlerCases = 0, traceComparisons = 0;
static void require(bool ok, const char* label) { ++checks; if (!ok) throw std::runtime_error(label); }
static u32 bits(float f) { u32 r; std::memcpy(&r, &f, 4); return r; }
static float fp(u32 u) { float r; std::memcpy(&r, &u, 4); return r; }
static std::array<u32, 10> playerWords(const PlayerEventState49A& p) {
    return {{p.field40,p.source5C,p.indexFFC,bits(p.value1098),bits(p.value1094),bits(p.value1090),
             bits(p.valueCB4),bits(p.valueCB0),bits(p.valueCB8),p.countCD0}};
}
static std::array<u32, 5> eventWords(const InputEvent48Z& e) { return {{e.code,e.category,e.deviceId,e.transition,e.contextMask}}; }
static std::array<u32, 6> recordWords(const DeviceButtonRecord48Y& r) { return {{r.held,bits(r.value),r.auxiliary08,r.auxiliary0C,r.auxiliary10,r.auxiliary14}}; }
// Every callback below is explicit fixture behavior, not a recovered helper.
// Trace snapshots check stores before/after a mandatory synchronous boundary.
struct Snapshot {
    std::array<u32, 10> player{};
    std::array<u32, 9> environment{};
    std::array<u32, 5> event{};
    std::array<u32, 6> record{};
    bool operator==(const Snapshot& x) const { return player==x.player && environment==x.environment && event==x.event && record==x.record; }
};
struct Trace { u32 type=0, target=0, code=0, mask=0, scalar=0, integer=0, result=0; Snapshot before{}, after{}; };
enum Change : u32 { ChangeSource=1, ChangeField=2, ChangeCB8=4, ChangeSelection=8,
    ChangeLazy=16, ChangeFlags=32, ChangeContext=64, ChangeEvent=128, ChangeRecord=256,
    Change1094=512, Change1090=1024, Change1098=2048, ChangeTuning=4096, ChangeChildren=8192 };
struct Mutation {
    u32 changes=0, source=0, field=0, cb8=0, selection=0, lazyFlag=0, lazyWord=0,
        flags=0, gate=0, contextPresent=0, eventCode=0, eventMask=0, recordXor=0,
        value1094=0, value1090=0, value1098=0, tuningIndex=0, tuningValue=0, tuningWord=0;
};
struct Script {
    std::array<u32, 2> returns{{0x80000000u,0xffffffffu}};
    std::array<bool, 2> sourceAvailable{{true,true}};
    std::array<Mutation, 2> queries{};
    std::array<Mutation, 10> effects{};
    std::array<bool, 10> effectAvailable{{true,true,true,true,true,true,true,true,true,true}};
    std::array<u32, 3> childReturns{{0,0,0}};
    std::array<Mutation, 3> children{};
    bool childAvailable=true;
};
struct Native {
    PlayerEventState49A player{0,0,1,-2.0f,17.0f,19.0f,23.0f,29.0f,31.0f,37};
    std::array<DeviceButtonBank48Y,8> banks{};
    std::array<PlayerActionRow49A,4> actions{};
    std::array<PlayerTuningRow49A,16> tuning{};
    std::array<EventChild48Z,3> children{};
    PlayerEventEnvironment49A env{};
    PlayerEventBindings49A bindings{};
    InputEvent48Z event{0xdeadbeefu,2,0,1,0x12345678u};
    DeviceButtonRecord48Y record{9,fp(0x7fc01234u),11,13,17,19};
    DeviceButtonRecord48Y* liveRecord=&record;
    Script script{};
    std::array<Trace,24> traces{};
    unsigned traceCount=0, effectCount=0, queryCount=0, childCount=0;
    bool valid=true;
    Native() {
        for (unsigned d=0;d<8;++d) for (unsigned i=0;i<32;++i)
            banks[d].records[i]={1,float(i+d),0x1000u+d*32+i,0x2000u+i,0x3000u+i,0x4000u+i};
        for (unsigned i=0;i<actions.size();++i) actions[i].action6AC=std::int32_t(5+i);
        for (unsigned i=0;i<tuning.size();++i) tuning[i]={float(i)+0.125f,0x10203040u+i};
        for (unsigned i=0;i<children.size();++i) children[i]={i+1<children.size()?i+1:noEventChild48Z,1};
        env.held.currentContext={true,this,query};env.held.auxiliary={true,this,auxiliary};
        for (unsigned d=0;d<8;++d) env.held.devices[d]=&banks[d];
        env.held.rootFlags228=4;env.held.context518=1;env.held.context1898=0x80;
        env.flags29=0;env.gate58=2;env.actionRows=actions.data();env.actionRowCount=actions.size();
        env.selection1125F=0xa3;env.tuningRows=tuning.data();env.tuningRowCount=tuning.size();
        env.lazyFlag85F4=0;env.lazyWord4340A0=0xffffffffu;
        bindings={{this,effect},{0x600u,0,children.data(),3},child,this,12};
    }
    Native(const Native&)=delete;
    Native& operator=(const Native&)=delete;
    Snapshot snapshot() const noexcept {
        return {playerWords(player),{{env.flags29,env.gate58,env.selection1125F,env.lazyFlag85F4,
            env.lazyWord4340A0,u32(env.held.currentContext.present),env.held.rootFlags228,
            env.held.context518,env.held.context1898}},eventWords(event),recordWords(*liveRecord)};
    }
    void mutate(const Mutation& m) noexcept {
        if(m.changes&ChangeSource)player.source5C=m.source;
        if(m.changes&ChangeField)player.field40=m.field;
        if(m.changes&ChangeCB8)player.valueCB8=fp(m.cb8);
        if(m.changes&ChangeSelection)env.selection1125F=std::uint8_t(m.selection);
        if(m.changes&ChangeLazy){env.lazyFlag85F4=std::uint8_t(m.lazyFlag);env.lazyWord4340A0=m.lazyWord;}
        if(m.changes&ChangeFlags){env.flags29=std::uint8_t(m.flags);env.gate58=std::uint8_t(m.gate);}
        if(m.changes&ChangeContext)env.held.currentContext.present=m.contextPresent!=0;
        if(m.changes&ChangeEvent){event.code=m.eventCode;event.contextMask=m.eventMask;}
        if(m.changes&ChangeRecord){auto w=recordWords(*liveRecord);for(auto& v:w)v^=m.recordXor;std::memcpy(liveRecord,w.data(),sizeof *liveRecord);}
        if(m.changes&Change1094)player.value1094=fp(m.value1094);
        if(m.changes&Change1090)player.value1090=fp(m.value1090);
        if(m.changes&Change1098)player.value1098=fp(m.value1098);
        if(m.changes&ChangeTuning)tuning[m.tuningIndex]={fp(m.tuningValue),m.tuningWord};
        if(m.changes&ChangeChildren){children[0].next=2;children[1].flags29=0;children[2].flags29=1;}
    }
    Trace* start(u32 type,u32 target) noexcept {
        if(traceCount>=traces.size()){valid=false;return nullptr;}
        auto* t=&traces[traceCount++];t->type=type;t->target=target;t->before=snapshot();return t;
    }
    EventCallbackResult48Z source(bool aux,std::int32_t code,u32 mask) noexcept {
        const unsigned i=code==0x12?1:0;++queryCount;
        auto* t=start(aux?1u:0u,0);if(!t)return {false,0};
        t->code=static_cast<u32>(code);t->mask=mask;t->result=script.returns[i];
        mutate(script.queries[i]);t->after=snapshot();return {script.sourceAvailable[i],t->result};
    }
    static EventCallbackResult48Z query(void* p,std::int32_t c,u32 m) noexcept { return static_cast<Native*>(p)->source(false,c,m); }
    static EventCallbackResult48Z auxiliary(void* p,std::int32_t c,u32 m) noexcept { return static_cast<Native*>(p)->source(true,c,m); }
    static bool effect(void* p,const PlayerEffectCall49A& c) noexcept {
        auto& n=*static_cast<Native*>(p);const auto i=static_cast<unsigned>(c.kind);
        auto* t=n.start(2,i);if(!t)return false;++n.effectCount;
        t->scalar=bits(c.scalarArgument);t->integer=c.integerArgument;t->result=n.script.effectAvailable[i]?1u:0u;
        n.mutate(n.script.effects[i]);t->after=n.snapshot();return t->result!=0;
    }
    static EventCallbackResult48Z child(void* p,u32 c,bool press,InputEvent48Z& e,DeviceButtonRecord48Y& r) noexcept {
        auto& n=*static_cast<Native*>(p);n.valid=n.valid&&&e==&n.event&&&r==n.liveRecord;
        if(c>=3){n.valid=false;return {false,0};}++n.childCount;
        auto* t=n.start(3,c);if(!t)return {false,0};t->integer=press?1u:0u;t->result=n.script.childReturns[c];
        n.mutate(n.script.children[c]);t->after=n.snapshot();return {n.script.childAvailable,t->result};
    }
    PlayerEventResult49A run() noexcept { return handlePlayerPress49A(player,env,bindings,event,*liveRecord); }
};
static void expectTrace(const Native& n,unsigned i,u32 type,u32 target,const char* label) {
    require(i<n.traceCount&&n.traces[i].type==type&&n.traces[i].target==target,label);
}
static void testManagerConcrete48Y() {
    Native n;
    for(bool present:{false,true}) for(u32 root:{0u,4u,0xffu})
    for(u32 mode:{0u,1u,2u}) for(u32 tag:{0u,1u,0x40u,0x80u,0xffffffffu})
    for(u32 mask:{0u,1u,0x3fu,0x40u,0x41u,0x80u,0x81u,0x80000000u,0xffffffffu}) {
        n.env.held.currentContext.present=present;n.env.held.rootFlags228=std::uint8_t(root);
        n.env.held.context518=mode;n.env.held.context1898=tag;
        for(unsigned d=0;d<8;++d) for(std::int32_t code:{0,6,31}) for(u32 held:{0u,1u,2u,0xffffffffu}) {
            n.banks[d].records[unsigned(code)].held=held;
            std::array<original48y::Record,32> bank{};
            for(unsigned i=0;i<32;++i)bank[i].words=recordWords(n.banks[d].records[i]);
            const auto expected=original48y::query(bank,code,mask,{present,std::uint8_t(root),mode,tag},false,{});
            const auto before=n.banks;
            const auto got=queryManagerHeld49A(n.env.held,code,2,mask,d);
            require(got.status==ManagerHeldStatus49A::Complete&&got.matched==expected.matched,"manager concrete48Y original mismatch");
            require(std::memcmp(before.data(),n.banks.data(),sizeof before)==0,"manager held query wrote bank without scratch output");
            ++managerCases;
        }
    }
    for(u32 category:{0u,1u,3u,0xffffffffu}) for(bool present:{false,true}) for(u32 returned:{0u,1u,0x80000000u,0xffffffffu}) {
        Native x;x.env.held.currentContext.present=present;x.env.held.auxiliary.present=present;x.script.returns[0]=returned;
        const auto got=queryManagerHeld49A(x.env.held,0x12345678,category,0xfedcba98,0xffffffffu);
        require(got.status==ManagerHeldStatus49A::Complete&&got.matched==(category<2&&present&&returned!=0),"manager category selection/nonzero normalization");
        require(x.traceCount==(category<2&&present?1u:0u),"absent/unknown manager source queried");
        if(x.traceCount)require(x.traces[0].type==category&&x.traces[0].code==0x12345678&&x.traces[0].mask==0xfedcba98,"manager callback forwarded arguments");
        ++managerCases;
    }
    for(u32 d:{8u,0x80000000u,0xffffffffu})require(queryManagerHeld49A(n.env.held,0,2,0x3f,d).status==ManagerHeldStatus49A::UnsupportedDeviceIndex,"unbounded manager device accepted");
    n.env.held.devices[0]=nullptr;
    const auto absent=queryManagerHeld49A(n.env.held,6,2,0x3f,0);
    require(absent.status==ManagerHeldStatus49A::Complete&&!absent.matched,"49A absent device fabricated held match");
    require(queryManagerHeld49A(n.env.held,-1,2,0x3f,0).status==ManagerHeldStatus49A::Complete,"absent device incorrectly indexed");
    n.env.held.devices[0]=&n.banks[0];n.env.held.currentContext.present=false;
    for(std::int32_t code:{-1,std::numeric_limits<std::int32_t>::min(),32,std::numeric_limits<std::int32_t>::max()}) {
        require(queryManagerHeld49A(n.env.held,code,2,0x3f,0).status==ManagerHeldStatus49A::UnsupportedDeviceCode,"manager invalid code accessed");
        auto denied=queryManagerHeld49A(n.env.held,code,2,0x80,0);
        require(denied.status==ManagerHeldStatus49A::Complete&&!denied.matched,"gate-denied invalid code should miss before index check");
    }
    n.env.held.currentContext={true,&n,nullptr};
    require(queryManagerHeld49A(n.env.held,1,0,2,0).status==ManagerHeldStatus49A::MissingSourceBinding,"present unbound context fabricated query");
    n.env.held.currentContext.query=Native::query;n.script.sourceAvailable[0]=false;
    require(queryManagerHeld49A(n.env.held,1,0,2,0).status==ManagerHeldStatus49A::SourceUnavailable,"unavailable source fabricated result");
}
static std::array<std::uint64_t,204> playerVisits{};
static std::array<std::uint64_t,56> managerVisits{};
static std::array<std::uint64_t,12> gateVisits{};
static std::uint64_t originalInstructions=0;
static void accumulate(const original49a::Stats& s) {
    originalInstructions+=s.steps;
    for(unsigned i=0;i<playerVisits.size();++i)playerVisits[i]+=s.playerVisits[i];
    for(unsigned i=0;i<managerVisits.size();++i)managerVisits[i]+=s.managerVisits[i];
    for(unsigned i=0;i<gateVisits.size();++i)gateVisits[i]+=s.gateVisits[i];
}
static original49a::State originalState(const Native& n) {
    original49a::State s; const auto& p=n.player;
    s.player={p.field40,p.source5C,p.indexFFC,bits(p.value1098),bits(p.value1094),bits(p.value1090),bits(p.valueCB4),bits(p.valueCB0),bits(p.valueCB8),p.countCD0};
    s.application={n.env.flags29,n.env.gate58};s.manager.contextPresent=n.env.held.currentContext.present;
    s.manager.auxiliaryPresent=n.env.held.auxiliary.present;
    for(unsigned i=0;i<8;++i)s.manager.devicePresent[i]=n.env.held.devices[i]!=nullptr;
    for(unsigned i=0;i<n.actions.size();++i)s.config.action6AC[i]=static_cast<u32>(n.actions[i].action6AC);
    s.config.selection1125F=n.env.selection1125F;
    for(unsigned i=0;i<16;++i)s.config.tuning[i]={bits(n.tuning[i].value102F0),n.tuning[i].word10388};
    s.lazyFlag85F4=n.env.lazyFlag85F4;s.lazyWord4340A0=n.env.lazyWord4340A0;
    s.event.words=eventWords(n.event);s.record.words=recordWords(*n.liveRecord);return s;
}
static Snapshot snapshot(const original49a::State& s,const Native& n) {
    const auto& p=s.player;
    return {{{p.field40,p.source5C,p.indexFFC,p.value1098,p.value1094,p.value1090,p.valueCB4,p.valueCB0,p.valueCB8,p.countCD0}},
        {{s.application.flags29,s.application.gate58,s.config.selection1125F,s.lazyFlag85F4,s.lazyWord4340A0,
          u32(s.manager.contextPresent),n.env.held.rootFlags228,n.env.held.context518,n.env.held.context1898}},s.event.words,s.record.words};
}
static void mutate(original49a::State& s,original48z::RouterState& router,const Mutation& m) {
    if(m.changes&ChangeSource)s.player.source5C=m.source;
    if(m.changes&ChangeField)s.player.field40=m.field;
    if(m.changes&ChangeCB8)s.player.valueCB8=m.cb8;
    if(m.changes&ChangeSelection)s.config.selection1125F=std::uint8_t(m.selection);
    if(m.changes&ChangeLazy){s.lazyFlag85F4=std::uint8_t(m.lazyFlag);s.lazyWord4340A0=m.lazyWord;}
    if(m.changes&ChangeFlags){s.application.flags29=std::uint8_t(m.flags);s.application.gate58=std::uint8_t(m.gate);}
    if(m.changes&ChangeContext)s.manager.contextPresent=m.contextPresent!=0;
    if(m.changes&ChangeEvent){s.event.words[0]=m.eventCode;s.event.words[4]=m.eventMask;}
    if(m.changes&ChangeRecord)for(auto& w:s.record.words)w^=m.recordXor;
    if(m.changes&Change1094)s.player.value1094=m.value1094;
    if(m.changes&Change1090)s.player.value1090=m.value1090;
    if(m.changes&Change1098)s.player.value1098=m.value1098;
    if(m.changes&ChangeTuning)s.config.tuning[m.tuningIndex]={m.tuningValue,m.tuningWord};
    if(m.changes&ChangeChildren){router.nodes[0].next=2;router.nodes[1].flags29=0;router.nodes[2].flags29=1;}
}
static u32 effectKind(const original49a::Call& c) {
    using K=original49a::CallKind;
    switch(c.kind) {
        case K::Player2255B0:return 0;
        case K::PlayerVirtual:return c.slot==0x94?1u:c.slot==0x90?2u:c.slot==0x8c?3u:4u;
        case K::Player22B170:return 5;
        case K::Player22B370:return 6;
        case K::Player22B570:return 7;
        case K::Player22B760:return 8;
        case K::Player22BA60:return 9;
        default:throw std::runtime_error("49A test unexpected effect kind");
    }
}
static void compareTrace(const Trace& a,const Trace& b) {
    require(a.type==b.type&&a.target==b.target,"49A differential callback kind/order mismatch");
    require(a.code==b.code&&a.mask==b.mask,"49A differential source query arguments mismatch");
    require(a.scalar==b.scalar&&a.integer==b.integer,"49A differential effect/router arguments mismatch");
    require(a.result==b.result,"49A differential callback result fixture mismatch");
    require(a.before==b.before,"49A differential complete callback pre-state mismatch");
    require(a.after==b.after,"49A differential complete callback post-state mismatch");++traceComparisons;
}
static void compareHandler(Native& n) {
    auto expected=originalState(n);original49a::Stats stats;
    original48z::RouterState router;router.parentFlags24=n.bindings.children.flags24;router.firstChild=n.bindings.children.firstChild;
    for(unsigned i=0;i<3;++i)router.nodes[i]={n.children[i].next,n.children[i].flags29};
    std::array<Trace,24> expectedTrace{};unsigned traceCount=0;
    const auto banksBefore=n.banks;
    const auto returned=original49a::playerEvent(expected,[&](original49a::State& s,const original49a::Call& c)->u32 {
        if(c.kind==original49a::CallKind::SourceQuery&&c.category==2) {
            require(c.a3==0&&c.slot==0x10,"49A original device query scratch/slot mismatch");
            std::array<original48y::Record,32> bank{};
            for(unsigned i=0;i<32;++i)bank[i].words=recordWords(n.env.held.devices[c.source]->records[i]);
            return original48y::query(bank,original49a::signedWord(c.code),c.mask,
                {s.manager.contextPresent,n.env.held.rootFlags228,n.env.held.context518,n.env.held.context1898},false,{}).matched?1u:0u;
        }
        if(c.kind==original49a::CallKind::Tail142350) {
            require(c.eventAddress==original49a::Machine::event&&c.recordAddress==original49a::Machine::record,"49A original tail lost incoming identities");
            router.event.words=s.event.words;router.record.words=s.record.words;
            const auto handled=original48z::router(router,true,[&](original48z::RouterState& r,unsigned child,bool press)->u32 {
                require(child<3&&traceCount<expectedTrace.size(),"49A bounded original child trace");
                s.event.words=r.event.words;s.record.words=r.record.words;
                auto& t=expectedTrace[traceCount++];t.type=3;t.target=child;t.integer=press?1u:0u;t.result=n.script.childReturns[child];t.before=snapshot(s,n);
                mutate(s,r,n.script.children[child]);t.after=snapshot(s,n);
                r.event.words=s.event.words;r.record.words=s.record.words;return t.result;
            });
            s.event.words=router.event.words;s.record.words=router.record.words;return handled?1u:0u;
        }
        require(traceCount<expectedTrace.size(),"49A bounded original callback trace");auto& t=expectedTrace[traceCount++];t.before=snapshot(s,n);
        if(c.kind==original49a::CallKind::SourceQuery) {
            require(c.a3==0&&c.slot==0x18,"49A original context query scratch/slot mismatch");
            const auto index=c.code==0x12?1u:0u;t.type=c.category;t.code=c.code;t.mask=c.mask;t.result=n.script.returns[index];
            mutate(s,router,n.script.queries[index]);t.after=snapshot(s,n);return t.result;
        }
        t.type=2;t.target=effectKind(c);t.scalar=(t.target==5||t.target==6)?c.f12Bits:0;
        t.integer=t.target==9?c.a1:0;t.result=1;mutate(s,router,n.script.effects[t.target]);t.after=snapshot(s,n);
        return 0x81000000u+t.target; // Arbitrary helper v0 is ignored by the original continuation.
    },&stats);
    const auto beforeAllocations=allocations;const auto got=n.run();
    require(allocations==beforeAllocations,"49A production handler allocated on hot path");
    require(got.status==PlayerEventStatus49A::Complete&&n.valid,"49A valid differential fixture rejected");
    require(got.handled==(returned!=0),"49A differential router-only consumption mismatch");
    require(got.queriesCompleted==stats.queryCalls&&stats.queryCalls==3,"49A three ordered manager queries mismatch");
    require(got.effectsInvoked==n.effectCount&&n.traceCount==traceCount,"49A differential callback count mismatch");
    require(n.snapshot()==snapshot(expected,n),"49A differential full final player/environment/event/record state mismatch");
    for(unsigned i=0;i<8;++i)require((n.env.held.devices[i]!=nullptr)==expected.manager.devicePresent[i],"49A differential manager source presence mismatch");
    require(n.env.held.auxiliary.present==expected.manager.auxiliaryPresent,"49A differential auxiliary presence mismatch");
    for(unsigned i=0;i<n.actions.size();++i)require(static_cast<u32>(n.actions[i].action6AC)==expected.config.action6AC[i],"49A differential action rows changed");
    for(unsigned i=0;i<16;++i)require(bits(n.tuning[i].value102F0)==expected.config.tuning[i].value102F0&&n.tuning[i].word10388==expected.config.tuning[i].word10388,"49A differential full tuning rows mismatch");
    require(std::memcmp(banksBefore.data(),n.banks.data(),sizeof banksBefore)==0,"49A handler changed unbound device records");
    for(unsigned i=0;i<traceCount;++i)compareTrace(n.traces[i],expectedTrace[i]);
    require(stats.queries[0].callerPC==0x1a1748&&stats.queries[0].category==2&&stats.queries[0].mask==0x3f,"49A first original query site/arguments mismatch");
    require(stats.queries[1].callerPC==0x1a17dc&&stats.queries[1].code==0x1a&&stats.queries[1].category==0&&stats.queries[1].mask==0x80,"49A second original query site/arguments mismatch");
    require(stats.queries[2].callerPC==0x1a18b8&&stats.queries[2].code==0x12&&stats.queries[2].category==0&&stats.queries[2].mask==0x80,"49A third original query site/arguments mismatch");
    accumulate(stats);++handlerCases;
}
static void testHandlerDifferentials() {
    for(int rounding:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}) {
        require(std::fesetround(rounding)==0,"49A rounding mode unavailable");
        for(unsigned flags=0;flags<256;++flags) {Native n;n.env.flags29=std::uint8_t(flags);compareHandler(n);}
        for(unsigned gate=0;gate<256;++gate) {
            original49a::Stats stats;const auto expected=original49a::stateGate(std::uint8_t(gate),&stats);accumulate(stats);
            Native n;n.env.gate58=std::uint8_t(gate);compareHandler(n);
            require((n.traces[0].type==2)==(expected!=0),"49A exact state gate byte mismatch");
        }
        for(u32 value:{0u,0x80000000u,0x3f800000u,0xbf800000u,1u,0x80000001u,0x7f7fffffu,0xff7fffffu}) {
            Native n;n.player.value1098=fp(value);compareHandler(n);
        }
        for(unsigned flag=0;flag<256;++flag)for(u32 mode:{0u,1u,2u,3u,0x80000000u,0xffffffffu}) {
            Native n;n.env.lazyFlag85F4=std::uint8_t(flag);n.env.lazyWord4340A0=mode;compareHandler(n);
        }
        for(unsigned selection=0;selection<256;++selection) {
            Native n;n.env.selection1125F=std::uint8_t(selection);n.tuning[selection&15].word10388=0xffffffffu;compareHandler(n);
            require(n.player.countCD0==0,"49A selected CD0 modulo increment mismatch");
        }
        for(u32 value:{0u,0x80000000u,1u,0x80000001u,0x3c23d709u,0x3c23d70au,0x3c23d70bu,
                      0x3f800001u,0xbf800001u,0x00800000u,0x80800000u,0x7f7fffffu}) {
            Native n;n.tuning[3].value102F0=fp(value);compareHandler(n);
        }
        for(u32 source:{0u,1u,2u,7u})for(u32 field:{0u,1u,0x80000000u,0xffffffffu})
        for(u32 q26:{0u,1u,0x80000000u})for(u32 q18:{0u,1u,0xffffffffu}) {
            Native n;n.player.source5C=source;n.player.field40=field;n.script.returns={{q26,q18}};compareHandler(n);
        }
        for(bool context:{false,true})for(bool device:{false,true})for(u32 held:{0u,1u,2u,0xffffffffu}) {
            Native n;n.env.held.currentContext.present=context;n.env.held.devices[0]=device?&n.banks[0]:nullptr;n.banks[0].records[6].held=held;compareHandler(n);
        }
        for(u32 consumed:{0u,1u,0x80000000u,0xffffffffu})for(unsigned at=0;at<3;++at) {
            Native n;n.script.childReturns[at]=consumed;compareHandler(n);
            require(n.effectCount==7&&n.childCount==(consumed?at+1:3),"49A actions consumed before child route");
        }
        // Callbacks mutate only fields explicitly represented in this fixture;
        // their effects must be visible at the original following reload sites.
        {
            Native n;auto& first=n.script.effects[0];first.changes=Change1094|Change1090|ChangeSource|ChangeField|ChangeLazy|ChangeEvent|ChangeRecord;
            first.value1094=bits(81.0f);first.value1090=bits(82.0f);first.source=1;first.field=0;first.lazyFlag=255;first.lazyWord=1;
            first.eventCode=0xabcdef01;first.eventMask=0xfedcba98;first.recordXor=0x13579bdf;
            auto& query=n.script.queries[0];query.changes=ChangeLazy;query.lazyFlag=128;query.lazyWord=0;
            auto& virtualEffect=n.script.effects[1];virtualEffect.changes=ChangeSelection|ChangeTuning;virtualEffect.selection=0xfe;virtualEffect.tuningIndex=14;virtualEffect.tuningValue=0x3f800001;virtualEffect.tuningWord=0xffffffffu;
            auto& q18=n.script.queries[1];q18.changes=ChangeSource|ChangeField;q18.source=0;q18.field=0xffffffffu;
            auto& action=n.script.effects[5];action.changes=ChangeCB8|ChangeSelection|ChangeTuning;action.cb8=0x80000000u;action.selection=0xff;action.tuningIndex=14;action.tuningValue=bits(72.0f);action.tuningWord=6;
            n.script.effects[6].changes=ChangeChildren;
            n.script.children[0].changes=ChangeEvent|ChangeRecord;n.script.children[0].eventCode=17;n.script.children[0].eventMask=42;n.script.children[0].recordXor=0x11111111;
            compareHandler(n);
        }
        for(unsigned mutation=0;mutation<4;++mutation) {
            Native n;auto& m=n.script.queries[1];m.changes=ChangeSource;m.source=mutation==0?1:mutation==1?2:mutation==2?0xffffffffu:0;
            compareHandler(n);
        }
        {Native n;n.script.effects[0].changes=ChangeField;n.script.effects[0].field=1;compareHandler(n);}
        {Native n;n.script.effects[0].changes=ChangeSource;n.script.effects[0].source=7;compareHandler(n);}
        {Native n;n.script.effects[0].changes=ChangeContext;n.script.effects[0].contextPresent=0;compareHandler(n);}
        {Native n;n.script.queries[0].changes=ChangeField;n.script.queries[0].field=1;compareHandler(n);}
        {Native n;n.bindings.children.flags24=0;compareHandler(n);}
        {Native n;n.bindings.children.firstChild=noEventChild48Z;compareHandler(n);}
        u32 random=0x49a12345u+u32(rounding);
        auto next=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
        for(unsigned trial=0;trial<320;++trial) {
            Native n;n.player.source5C=next()%8;n.player.indexFFC=next()%4;n.player.field40=next()%2;
            n.player.value1098=fp((next()&0x800fffffu)|0x3f000000u);n.env.flags29=std::uint8_t(next());n.env.gate58=std::uint8_t(next());
            n.env.lazyFlag85F4=std::uint8_t(next());n.env.lazyWord4340A0=next()%5;n.env.selection1125F=std::uint8_t(next());
            n.script.returns={{next()%3,next()%3}};n.env.held.currentContext.present=(next()&1)!=0;
            n.banks[n.player.source5C].records[unsigned(n.actions[n.player.indexFFC].action6AC)].held=next()%3;
            n.script.childReturns[next()%3]=next()%2;
            n.script.effects[5].changes=ChangeCB8;n.script.effects[5].cb8=next();compareHandler(n);
        }
    }
    require(std::fesetround(FE_TONEAREST)==0,"49A rounding restore failed");
}
static void testManagerOracle() {
    for(u32 category:{0u,1u,2u,3u,0xffffffffu})for(bool present:{false,true})
    for(u32 returned:{0u,1u,0x80000000u,0xffffffffu})for(u32 source:{0u,1u,7u}) {
        Native n;n.env.held.currentContext.present=present;n.env.held.auxiliary.present=present;
        n.env.held.devices[source]=present?&n.banks[source]:nullptr;n.script.returns[0]=returned;
        n.banks[source].records[6].held=returned;
        auto s=originalState(n);original49a::Stats stats;unsigned called=0;
        const auto expected=original49a::managerHeld(s,6,category,0x3f,source,[&](original49a::State& state,const original49a::Call& c)->u32 {
            ++called;require(c.category==category&&c.code==6&&c.mask==0x3f&&c.source==source&&c.a3==0,"49A original manager source selection arguments");
            if(category!=2)return returned;
            std::array<original48y::Record,32> bank{};for(unsigned i=0;i<32;++i)bank[i].words=recordWords(n.banks[source].records[i]);
            return original48y::query(bank,6,0x3f,{state.manager.contextPresent,4,1,0x80},false,{}).matched?1u:0u;
        },&stats);
        const auto allocationCount=allocations;
        const auto got=queryManagerHeld49A(n.env.held,6,category,0x3f,source);
        require(allocations==allocationCount,"49A manager hot-path allocation");
        require(got.status==ManagerHeldStatus49A::Complete&&got.matched==(expected!=0),"49A independent original manager mismatch");
        require(n.queryCount==(category<2?called:0u),"49A independent original manager provider count mismatch");
        accumulate(stats);++managerCases;
    }
    Native alias;alias.env.held.devices[7]=alias.env.held.devices[0];alias.env.held.auxiliary=alias.env.held.currentContext;
    for(u32 category:{0u,1u,2u}) {
        const auto got=queryManagerHeld49A(alias.env.held,6,category,0x3f,7);
        require(got.status==ManagerHeldStatus49A::Complete&&got.matched,"49A typed source aliases rejected");
    }
}
static void testBoundaries() {
    for(unsigned variant=0;variant<3;++variant) {
        Native n;if(variant==0)n.env.actionRows=nullptr;if(variant==1)n.player.indexFFC=4;if(variant==2)n.env.actionRowCount=0;
        const auto before=n.snapshot();const auto got=n.run();
        require(got.status==PlayerEventStatus49A::InvalidActionRow&&got.queriesCompleted==0&&got.effectsInvoked==0,"49A invalid action row boundary");
        require(n.snapshot()==before&&n.traceCount==0,"49A invalid action row changed state");
    }
    for(unsigned variant=0;variant<4;++variant) {
        Native n;if(variant<2)n.player.source5C=variant==0?8u:0xffffffffu;else n.actions[1].action6AC=variant==2?-1:32;
        const auto before=n.snapshot();const auto got=n.run();
        require(got.status==PlayerEventStatus49A::QueryUnavailable&&got.queriesCompleted==0&&got.effectsInvoked==0,"49A invalid first held query boundary");
        require(got.lastQuery==(variant<2?ManagerHeldStatus49A::UnsupportedDeviceIndex:ManagerHeldStatus49A::UnsupportedDeviceCode),"49A invalid first held query reason");
        require(n.snapshot()==before&&n.traceCount==0,"49A invalid first held query changed state");
    }
    for(unsigned variant=0;variant<2;++variant) {
        Native n;if(variant)n.script.sourceAvailable[0]=false;else n.env.held.currentContext.query=nullptr;
        const auto got=n.run();
        require(got.status==PlayerEventStatus49A::QueryUnavailable&&got.queriesCompleted==1&&got.effectsInvoked==1,"49A later query error lost partial progress");
        require(bits(n.player.value1094)==0x40200000&&bits(n.player.value1090)==0xbf800000&&n.childCount==0,"49A later query error rolled back prior stores");
        require(got.lastQuery==(variant?ManagerHeldStatus49A::SourceUnavailable:ManagerHeldStatus49A::MissingSourceBinding),"49A later query error reason");
    }
    {
        Native n;n.script.sourceAvailable[1]=false;const auto got=n.run();
        require(got.status==PlayerEventStatus49A::QueryUnavailable&&got.queriesCompleted==2&&got.effectsInvoked==2,"49A third query unavailable partial count");
        require(n.env.lazyFlag85F4==1&&n.env.lazyWord4340A0==2&&n.player.countCD0==37,"49A third query unavailable lost lazy initialization");
    }
    for(unsigned phase=0;phase<3;++phase) {
        Native n;n.bindings.effects.apply=nullptr;
        if(phase>0)n.banks[0].records[6].held=0;
        if(phase>1)n.script.returns[0]=0;
        const auto got=n.run();
        require(got.status==PlayerEventStatus49A::MissingEffectBinding&&got.effectsInvoked==0&&got.queriesCompleted==phase+1,"49A missing effect binding dependency boundary");
        require(n.childCount==0&&bits(n.player.value1094)==bits(17.0f)&&bits(n.player.value1090)==bits(19.0f),"49A missing effect invented dependent1094/1090 writes");
        if(phase==1)require(n.env.lazyFlag85F4==1&&n.env.lazyWord4340A0==2,"49A missing virtual effect erased prior lazy writes");
        if(phase==2)require(n.player.countCD0==n.tuning[3].word10388+1u&&bits(n.player.valueCB4)==bits(n.tuning[3].value102F0),"49A missing tuning effect erased prior direct writes");
    }
    for(unsigned failed=0;failed<10;++failed) {
        Native n;if(failed>=1&&failed<=4){n.env.lazyFlag85F4=255;n.env.lazyWord4340A0=failed==1?0:failed==2?1:failed==3?2:7;}
        n.script.effectAvailable[failed]=false;
        auto& mutation=n.script.effects[failed];mutation.changes=Change1094|ChangeRecord;mutation.value1094=bits(71.0f);mutation.recordXor=0x13579bdf;
        const auto before=n.snapshot();const auto got=n.run();
        require(got.status==PlayerEventStatus49A::EffectUnavailable&&!got.handled&&n.childCount==0,"49A unavailable effect continued or consumed");
        const unsigned expectedCount=failed==0?1:failed<=4?2:failed-2;
        require(got.effectsInvoked==expectedCount&&got.queriesCompleted==(failed==0?1u:failed<=4?2u:3u),"49A unavailable effect partial progress mismatch");
        expectTrace(n,n.traceCount-1,2,failed,"49A unavailable effect ran a later callback");
        require(bits(n.player.value1094)==bits(71.0f)&&recordWords(n.record)[2]==(before.record[2]^0x13579bdf),"49A unavailable effect lost its external partial mutation");
        if(failed==0)require(bits(n.player.value1090)==before.player[5],"49A unavailable2255B0 wrote dependent1090");
        if(failed>=5)require(n.player.countCD0==n.tuning[3].word10388+1u,"49A unavailable tuning effect erased direct CD0 write");
    }
    for(unsigned variant=0;variant<2;++variant) {
        Native n;if(variant)n.env.tuningRowCount=3;else n.env.tuningRows=nullptr;const auto got=n.run();
        require(got.status==PlayerEventStatus49A::InvalidTuningRow&&got.queriesCompleted==3&&got.effectsInvoked==2,"49A invalid tuning row boundary");
        require(bits(n.player.valueCB4)==bits(23.0f)&&n.player.countCD0==37&&n.env.lazyFlag85F4==1,"49A invalid tuning row partial state");
    }
    for(u32 nonfinite:{0x7f800000u,0xff800000u,0x7fc12345u}) {
        Native n;n.player.value1098=fp(nonfinite);auto got=n.run();
        require(got.status==PlayerEventStatus49A::UnsupportedNumeric&&got.queriesCompleted==1&&!got.effectsInvoked,"49A nonfinite direct comparison admitted");
        Native gated;gated.player.value1098=fp(nonfinite);gated.env.flags29=4;got=gated.run();
        require(got.status==PlayerEventStatus49A::Complete,"49A gated numeric field incorrectly validated");
        Native tuning;tuning.tuning[3].value102F0=fp(nonfinite);got=tuning.run();
        require(got.status==PlayerEventStatus49A::UnsupportedNumeric&&got.queriesCompleted==3&&got.effectsInvoked==2&&tuning.player.countCD0==37,"49A nonfinite selected tuning partial boundary");
        Native unselected;unselected.tuning[2].value102F0=fp(nonfinite);got=unselected.run();
        require(got.status==PlayerEventStatus49A::Complete,"49A unselected tuning incorrectly validated");
    }
    for(unsigned variant=0;variant<5;++variant) {
        Native n;if(variant==0)n.bindings.childCallback=nullptr;if(variant==1)n.script.childAvailable=false;
        if(variant==2)n.bindings.children.firstChild=3;
        if(variant==3)n.bindings.childVisitBudget=0;
        if(variant==4){n.children[0].next=0;n.bindings.childVisitBudget=2;}
        const auto got=n.run();
        const auto expected=variant==0?EventRouteStatus48Z::MissingCallback:variant==1?EventRouteStatus48Z::CallbackUnavailable:variant==2?EventRouteStatus48Z::InvalidNode:EventRouteStatus48Z::VisitBudgetExhausted;
        require(got.status==PlayerEventStatus49A::ChildRouteUnavailable&&got.childRoute.status==expected&&!got.handled,"49A child route boundary reason");
        require(got.queriesCompleted==3&&got.effectsInvoked==7&&n.player.countCD0==n.tuning[3].word10388+1u,"49A child error erased successful action progress");
    }
    {Native n;n.bindings.children.flags24=0;n.bindings.childCallback=nullptr;n.bindings.effects.apply=nullptr;
        n.env.held.currentContext.present=false;n.env.held.devices[0]=nullptr;
        auto got=n.run();require(got.status==PlayerEventStatus49A::Complete&&!got.handled&&got.queriesCompleted==3&&!got.effectsInvoked,"49A unnecessary effect/router bindings required");}
    // The device selected for query and the event's supplied complete record can
    // be a typed alias. The explicit effect may mutate it, without a hidden copy.
    {Native n;n.liveRecord=&n.banks[0].records[6];n.script.effects[0].changes=ChangeRecord;n.script.effects[0].recordXor=0x12345678;
        const auto before=recordWords(*n.liveRecord);const auto got=n.run();
        require(got.status==PlayerEventStatus49A::Complete&&n.valid&&recordWords(n.banks[0].records[6])[0]==(before[0]^0x12345678),"49A complete-record typed alias lost identity/mutation");}
}
static void testAdapters() {
    Native n;PlayerEventChildBinding49A binding{7,&n.player,&n.env,&n.bindings,{}};
    require(!playerEventChild49A(nullptr,7,true,n.event,n.record).available,"49A null player adapter accepted");
    require(!playerEventChild49A(&binding,6,true,n.event,n.record).available,"49A wrong player child identity accepted");
    binding.player=nullptr;require(!playerEventChild49A(&binding,7,true,n.event,n.record).available,"49A press without player accepted");
    binding.player=&n.player;binding.environment=nullptr;require(!playerEventChild49A(&binding,7,true,n.event,n.record).available,"49A press without environment accepted");
    binding.environment=&n.env;binding.bindings=nullptr;require(!playerEventChild49A(&binding,7,false,n.event,n.record).available,"49A release without bindings accepted");
    binding.bindings=&n.bindings;
    for(u32 returned:{0u,1u,0x80000000u,0xffffffffu}) {
        Native x;x.script.childReturns[0]=returned;PlayerEventChildBinding49A b{7,&x.player,&x.env,&x.bindings,{}};
        const auto result=playerEventChild49A(&b,7,true,x.event,x.record);
        require(result.available&&result.originalReturn==(returned!=0?1u:0u)&&b.lastResult.queriesCompleted==3&&b.lastResult.effectsInvoked==7,"49A press adapter result/sequence mismatch");
        Native r;r.script.childReturns[0]=returned;r.bindings.effects.apply=nullptr;PlayerEventChildBinding49A release{7,nullptr,nullptr,&r.bindings,{}};
        const auto before=playerWords(r.player);const auto released=playerEventChild49A(&release,7,false,r.event,r.record);
        require(released.available&&released.originalReturn==(returned!=0?1u:0u)&&release.lastResult.queriesCompleted==0&&release.lastResult.effectsInvoked==0,"49A release adapter incorrectly queried/actioned");
        require(playerWords(r.player)==before&&r.queryCount==0&&r.effectCount==0&&r.valid,"49A release adapter touched player state");
        for(unsigned i=0;i<r.traceCount;++i)require(r.traces[i].type==3&&r.traces[i].integer==0,"49A release adapter routed press");
    }
    Native failure;failure.bindings.effects.apply=nullptr;PlayerEventChildBinding49A b{0,&failure.player,&failure.env,&failure.bindings,{}};
    require(!playerEventChild49A(&b,0,true,failure.event,failure.record).available&&b.lastResult.status==PlayerEventStatus49A::MissingEffectBinding,"49A adapter swallowed action availability failure");
}
static void testConnectedPacket() {
    DevicePacket48Y packet{};packet.bytes[2]=packet.bytes[3]=255;packet.bytes[16]=91;
    const auto descriptors=initialDualShockDescriptors48Y();DeviceButtonBank48Y previous{};DeviceButtonSample48Y sample{};
    std::array<original48y::Record,32> originalBank{};std::array<original48y::Descriptor,32> originalDescriptors{};std::array<std::uint8_t,32> originalPacket{};
    for(unsigned i=0;i<32;++i){originalDescriptors[i]={descriptors.records[i].flags,descriptors.records[i].packetOffset,descriptors.records[i].digitalMask};originalPacket[i]=packet.bytes[i];}
    const auto expected=original48y::sample(originalBank,originalDescriptors,16,0,originalPacket,true);
    require(sampleDeviceButtons48Y(true,true,packet,descriptors,16,0,previous,sample),"49A connected packet rejected");
    require(sample.status==DevicePacketStatus48Y::Accepted&&sample.transitionCount==1&&expected.eventCount==1,"49A connected packet edge count");
    for(unsigned i=0;i<32;++i)require(recordWords(sample.buttons.records[i])==expected.records[i].words,"49A connected packet original bank mismatch");
    Native n;n.env.held.currentContext.present=false;n.env.held.devices[0]=&sample.buttons;n.liveRecord=&sample.buttons.records[6];
    n.script.effects[0].changes=ChangeRecord;n.script.effects[0].recordXor=0x10000;
    n.script.childReturns[0]=0x80000000u;
    InputEventQueue48Z queue{&n.event,1,0};require(appendDeviceTransitions48Z(sample,queue),"49A connected transition append failed");
    require(eventWords(n.event)==expected.events[0],"49A connected append event order/payload mismatch");
    EventContext48Z context{false,0xfeedface,{nullptr,0}};EventRecordSources48Z sources{};sources.devices[0]={sample.buttons.records,32};
    PlayerEventChildBinding49A playerChild{0,&n.player,&n.env,&n.bindings,{}};
    EventChild48Z rootChild{noEventChild48Z,1};
    JoystickApplicationBridge48Z bridge{0,{0x600,0,&rootChild,1},playerEventChild49A,&playerChild,4,{}};
    const auto beforeAllocations=allocations;
    const auto dispatch=dispatchInputEvents48Z(queue,context,sources,{&bridge,joystickReleaseBridge48Z,joystickPressBridge48Z});
    require(allocations==beforeAllocations,"49A connected hot path allocated");
    require(dispatch.status==EventDispatchStatus48Z::Complete&&dispatch.eventsCompleted==1&&dispatch.callbacksInvoked==1,"49A connected manager/app/player press path failed");
    require(playerChild.lastResult.status==PlayerEventStatus49A::Complete&&playerChild.lastResult.queriesCompleted==3&&playerChild.lastResult.effectsInvoked==1&&playerChild.lastResult.handled,"49A connected concrete held query/action sequence");
    require(n.queryCount==0&&n.effectCount==1&&n.childCount==1&&n.valid,"49A null manager context fabricated held callback");
    require(bits(n.player.value1094)==0x40200000u&&bits(n.player.value1090)==0xbf800000u&&n.player.countCD0==37&&n.env.lazyFlag85F4==0,"49A connected direct stores or absent-context behavior");
    require(n.event.contextMask==0&&recordWords(*n.liveRecord)[2]==(expected.records[6].words[2]^0x10000u),"49A connected live record/tag identity");
    // A concrete packet release follows the proven player release router only.
    auto held=sample.buttons;held.records[6].held=1;packet.bytes[16]=0;
    DeviceButtonSample48Y released{};require(sampleDeviceButtons48Y(true,true,packet,descriptors,16,0,held,released),"49A connected release packet rejected");
    require(released.transitionCount==1&&released.transitions[0].held==0,"49A connected release edge missing");
    resetInputEventCount48Z(queue);require(appendDeviceTransitions48Z(released,queue),"49A connected release append failed");
    sources.devices[0]={released.buttons.records,32};n.liveRecord=&released.buttons.records[6];n.env.held.devices[0]=&released.buttons;
    const auto beforePlayer=playerWords(n.player);const auto queries=n.queryCount,effects=n.effectCount;n.bindings.effects.apply=nullptr;
    const auto release=dispatchInputEvents48Z(queue,context,sources,{&bridge,joystickReleaseBridge48Z,joystickPressBridge48Z});
    require(release.status==EventDispatchStatus48Z::Complete&&release.eventsCompleted==1&&playerChild.lastResult.queriesCompleted==0&&playerChild.lastResult.effectsInvoked==0,"49A connected release incorrectly entered press handler");
    require(n.queryCount==queries&&n.effectCount==effects&&playerWords(n.player)==beforePlayer,"49A connected release changed action state");
    // Missing mandatory helper is explicit partial progress, never an invented
    // successful no-op. The manager reports its callback boundary as unavailable.
    Native missing;missing.env.held.currentContext.present=false;missing.env.held.devices[0]=&sample.buttons;
    sample.buttons.records[6].held=1;missing.liveRecord=&sample.buttons.records[6];missing.bindings.effects.apply=nullptr;
    InputEventQueue48Z missingQueue{&missing.event,1,0};require(appendDeviceTransitions48Z(sample,missingQueue),"49A unavailable connected append failed");
    PlayerEventChildBinding49A missingChild{0,&missing.player,&missing.env,&missing.bindings,{}};
    JoystickApplicationBridge48Z missingBridge{0,{0x600,0,&rootChild,1},playerEventChild49A,&missingChild,4,{}};
    sources.devices[0]={sample.buttons.records,32};
    const auto failure=dispatchInputEvents48Z(missingQueue,context,sources,{&missingBridge,joystickReleaseBridge48Z,joystickPressBridge48Z});
    require(failure.status==EventDispatchStatus48Z::CallbackUnavailable&&failure.eventsCompleted==0&&failure.callbacksInvoked==1,"49A connected missing helper falsely completed");
    require(missingChild.lastResult.status==PlayerEventStatus49A::MissingEffectBinding&&missingChild.lastResult.queriesCompleted==1&&bits(missing.player.value1094)==bits(17.0f)&&bits(missing.player.value1090)==bits(19.0f),"49A connected missing helper wrote dependent fields");
}
static void checkCoverage() {
    unsigned reachable=0,covered=0;
    const auto check=[&](u32 start,const auto& visits) {
        for(unsigned i=0;i<visits.size();++i) {
            const u32 pc=start+4*i;bool dead=false;for(u32 unreachable:original49a::unreachablePCs)dead|=pc==unreachable;
            if(dead){require(visits[i]==0,"49A declared unreachable instruction executed");continue;}
            ++reachable;if(visits[i])++covered;else {std::cerr<<"uncovered49A PC 0x"<<std::hex<<pc<<std::dec<<'\n';}
        }
    };
    check(rr_player_49a_start,playerVisits);check(rr_manager_49a_start,managerVisits);check(rr_gate_49a_start,gateVisits);
    require(covered==reachable&&reachable==268,"49A full reachable instruction coverage missing");
}
int main() {
    try {
        testManagerConcrete48Y();testManagerOracle();testHandlerDifferentials();testBoundaries();testAdapters();testConnectedPacket();checkCoverage();
        std::cout<<"player_events49a: PASS checks="<<checks<<" manager_cases="<<managerCases<<" player_cases="<<handlerCases
                 <<" callback_traces="<<traceComparisons<<" original_instructions="<<originalInstructions
                 <<" reachable_words=268/268 rounding_modes=4 hot_allocations=0\n";
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"player_events49a: FAIL "<<error.what()<<'\n';return 1;
    }
}
