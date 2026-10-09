#include "../../src/game/event_dispatch48z.h"
#include "../../tools/gameplay/event_dispatch_oracle48z.h"
#include "../../tools/gameplay/device_held_oracle48y.h"
#include <array>
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
static std::uint64_t checks = 0, managers = 0, routers = 0, instructions = 0, callbackComparisons = 0;
static std::array<std::uint64_t, (rr_manager_48z_end - rr_manager_48z_start) / 4> managerVisits{};
static std::array<std::uint64_t, (rr_release_router_48z_end - rr_release_router_48z_start) / 4> releaseVisits{};
static std::array<std::uint64_t, (rr_press_router_48z_end - rr_press_router_48z_start) / 4> pressVisits{};
static void require(bool ok, const char* label) { ++checks; if (!ok) throw std::runtime_error(label); }
static original48z::Event eventWords(const InputEvent48Z& e) { original48z::Event r; std::memcpy(r.words.data(), &e, 20); return r; }
static original48z::Record recordWords(const DeviceButtonRecord48Y& r) { original48z::Record x; std::memcpy(x.words.data(), &r, 24); return x; }
static void xorRecord(DeviceButtonRecord48Y& record, u32 bits) noexcept {
    auto words = recordWords(record); for (auto& word : words.words) word ^= bits; std::memcpy(&record, words.words.data(), 24);
}
static void accumulate(const original48z::Stats& s) {
    instructions += s.steps;
    for (unsigned i = 0; i < managerVisits.size(); ++i) managerVisits[i] += s.managerVisits[i];
    for (unsigned i = 0; i < releaseVisits.size(); ++i) releaseVisits[i] += s.releaseRouterVisits[i];
    for (unsigned i = 0; i < pressVisits.size(); ++i) pressVisits[i] += s.pressRouterVisits[i];
}
static void compareTrace(const original48z::CallbackTrace& a, const original48z::CallbackTrace& b) {
    require(a.pc == b.pc && a.press == b.press, "callback original site/order mismatch");
    require(a.eventIndex == b.eventIndex && a.category == b.category && a.device == b.device && a.code == b.code && a.childIndex == b.childIndex, "callback selected identity mismatch");
    require(a.eventAddress == b.eventAddress && a.recordAddress == b.recordAddress, "callback exact pointer identity mismatch");
    require(a.result == b.result, "callback return fixture mismatch");
    require(a.eventBefore.words == b.eventBefore.words && a.eventAfter.words == b.eventAfter.words, "callback event snapshots mismatch");
    require(a.recordBefore.words == b.recordBefore.words && a.recordAfter.words == b.recordAfter.words, "callback record snapshots mismatch");
    ++callbackComparisons;
}
// Effects are explicit supplied callback behavior. Neither fixture calls native
// dispatch to decide expected control flow; the oracle executes frozen words.
enum ManagerChange : u32 { ChangeCode = 1, ChangeCategory = 2, ChangeDevice = 4,
    ChangeTransition = 8, ChangeTag = 16, ChangeContextPresent = 32,
    ChangeContextMask = 64, ChangeCount = 128, ChangeFuture = 256,
    ReplaceEvents = 512, ReplaceCapacity = 1024, RemoveDevice = 2048,
    ReplaceContextRecords = 4096, ReplaceAuxiliaryRecords = 8192, ReplaceDeviceRecords = 16384 };
struct ManagerEffect {
    u32 changes = 0, code = 0, category = 0, device = 0, transition = 0, tag = 0, contextMask = 0;
    bool contextPresent = false, available = true;
    std::int32_t count = 0;
    unsigned futureIndex = 0;
    original48z::Event future{};
    u32 recordXor = 0, result = 0;
};
struct ManagerScript { std::array<ManagerEffect, 160> effects{}; };
static void oracleEffect(original48z::ManagerState& s, unsigned index, original48z::Record& record, const ManagerEffect& e) {
    auto& words = s.events[index].words;
    if (e.changes & ChangeCode) words[0] = e.code;
    if (e.changes & ChangeCategory) words[1] = e.category;
    if (e.changes & ChangeDevice) words[2] = e.device;
    if (e.changes & ChangeTransition) words[3] = e.transition;
    if (e.changes & ChangeTag) words[4] = e.tag;
    if (e.changes & ChangeContextPresent) s.contextPresent = e.contextPresent;
    if (e.changes & ChangeContextMask) s.contextMask = e.contextMask;
    if (e.changes & ChangeCount) s.count = e.count;
    if (e.changes & ChangeFuture) s.events[e.futureIndex] = e.future;
    for (auto& word : record.words) word ^= e.recordXor;
}
struct NativeManager {
    std::array<InputEvent48Z, 64> events{}, replacement{};
    std::array<DeviceButtonRecord48Y, 32> contextRecords{}, auxiliaryRecords{};
    std::array<std::array<DeviceButtonRecord48Y, 32>, 8> devices{};
    InputEventQueue48Z queue{};
    EventContext48Z context{};
    EventRecordSources48Z sources{};
    ManagerScript script{};
    std::array<original48z::CallbackTrace, 160> traces{};
    unsigned calls = 0;
    bool identitiesValid = true;
    explicit NativeManager(const original48z::ManagerState& s, const ManagerScript& actions = {}) : script(actions) {
        for (unsigned i = 0; i < 64; ++i) std::memcpy(&events[i], s.events[i].words.data(), 20);
        for (unsigned i = 0; i < 32; ++i) {
            std::memcpy(&contextRecords[i], s.contextRecords[i].words.data(), 24);
            std::memcpy(&auxiliaryRecords[i], s.auxiliaryRecords[i].words.data(), 24);
            for (unsigned d = 0; d < 8; ++d) std::memcpy(&devices[d][i], s.deviceRecords[d][i].words.data(), 24);
        }
        queue = {events.data(), 64, s.count};
        context = {s.contextPresent, s.contextMask, {contextRecords.data(), 32}};
        sources.auxiliary = {s.auxiliaryPresent ? auxiliaryRecords.data() : nullptr, 32};
        for (unsigned d = 0; d < 8; ++d) sources.devices[d] = {s.devicePresent[d] ? devices[d].data() : nullptr, 32};
    }
    EventCallbackResult48Z callback(bool press, InputEvent48Z& event, DeviceButtonRecord48Y& record) noexcept {
        if (calls >= traces.size()) { identitiesValid = false; return {false, 0}; }
        auto& trace = traces[calls]; const auto& e = script.effects[calls++];
        trace.press = press;
        bool foundEvent = false, foundRecord = false;
        for (unsigned i = 0; i < events.size(); ++i) if (&event == &events[i]) {
            trace.eventIndex = i; trace.eventAddress = original48z::Machine::eventAddress(i); foundEvent = true;
        }
        for (unsigned i = 0; i < 32; ++i) {
            if (&record == &contextRecords[i]) { trace.category = 0; trace.recordAddress = original48z::Machine::context + 0x20 + 24 * i; foundRecord = true; }
            if (&record == &auxiliaryRecords[i]) { trace.category = 1; trace.recordAddress = original48z::Machine::auxiliary + 0x1c + 24 * i; foundRecord = true; }
            for (unsigned d = 0; d < 8; ++d) if (&record == &devices[d][i]) { trace.category = 2; trace.recordAddress = original48z::Machine::deviceAddress(d) + 0x324 + 24 * i; foundRecord = true; }
        }
        identitiesValid = identitiesValid && foundEvent && foundRecord;
        trace.pc = trace.category == 0 ? (press ? 0x12824c : 0x1281f8) : trace.category == 1 ? (press ? 0x1282ec : 0x128294) : (press ? 0x1283a0 : 0x128340);
        trace.code = event.code; trace.device = event.deviceId;
        trace.eventBefore = eventWords(event); trace.recordBefore = recordWords(record); trace.result = e.result;
        if (e.changes & ChangeCode) event.code = e.code;
        if (e.changes & ChangeCategory) event.category = e.category;
        if (e.changes & ChangeDevice) event.deviceId = e.device;
        if (e.changes & ChangeTransition) event.transition = e.transition;
        if (e.changes & ChangeTag) event.contextMask = e.tag;
        if (e.changes & ChangeContextPresent) context.present = e.contextPresent;
        if (e.changes & ChangeContextMask) context.mask1898 = e.contextMask;
        if (e.changes & ChangeCount) queue.count = e.count;
        if (e.changes & ChangeFuture) std::memcpy(&events[e.futureIndex], e.future.words.data(), 20);
        if (e.changes & ReplaceEvents) queue.events = replacement.data();
        if (e.changes & ReplaceCapacity) --queue.capacity;
        if (e.changes & RemoveDevice) sources.devices[e.device] = {nullptr, 0};
        if (e.changes & ReplaceContextRecords) context.records = {auxiliaryRecords.data(), 32};
        if (e.changes & ReplaceAuxiliaryRecords) sources.auxiliary = {contextRecords.data(), 32};
        if (e.changes & ReplaceDeviceRecords) sources.devices[7] = {devices[6].data(), 32};
        xorRecord(record, e.recordXor);
        trace.eventAfter = eventWords(event); trace.recordAfter = recordWords(record);
        return {e.available, e.result};
    }
    static EventCallbackResult48Z release(void* p, InputEvent48Z& e, DeviceButtonRecord48Y& r) noexcept { return static_cast<NativeManager*>(p)->callback(false, e, r); }
    static EventCallbackResult48Z press(void* p, InputEvent48Z& e, DeviceButtonRecord48Y& r) noexcept { return static_cast<NativeManager*>(p)->callback(true, e, r); }
    ManagerEventCallbacks48Z callbacks() noexcept { return {this, release, press}; }
};
static original48z::ManagerState seedManager() {
    original48z::ManagerState s;
    s.contextPresent = s.auxiliaryPresent = true; s.contextMask = 0x89abcdefu; s.devicePresent.fill(true);
    for (unsigned i = 0; i < 64; ++i) s.events[i].words = {i % 32, i % 3, i % 8, i % 2, 0xcafebabeu};
    for (unsigned i = 0; i < 32; ++i) for (unsigned w = 0; w < 6; ++w) {
        s.contextRecords[i].words[w] = 0x10c00000u + 0x100 * i + w;
        s.auxiliaryRecords[i].words[w] = 0x10a00000u + 0x100 * i + w;
        for (unsigned d = 0; d < 8; ++d) s.deviceRecords[d][i].words[w] = 0x10d00000u + 0x10000 * d + 0x100 * i + w;
    }
    return s;
}
static void compareManager(original48z::ManagerState expected, const ManagerScript& script = {}) {
    NativeManager native(expected, script); original48z::Stats stats;
    unsigned oracleCalls = 0;
    original48z::manager(expected, [&](original48z::ManagerState& s, bool, unsigned index, unsigned, unsigned, unsigned, original48z::Record& r) {
        const auto& effect = script.effects.at(oracleCalls++); oracleEffect(s, index, r, effect); return effect.result;
    }, &stats);
    const auto got = dispatchInputEvents48Z(native.queue, native.context, native.sources, native.callbacks());
    require(got.status == EventDispatchStatus48Z::Complete, "valid manager fixture rejected");
    require(got.eventsCompleted == stats.visits(0x128164), "manager completed-event count mismatch");
    require(got.callbacksInvoked == oracleCalls && native.calls == oracleCalls && native.identitiesValid, "manager callback count/identity mismatch");
    require(native.queue.count == expected.count && native.context.present == expected.contextPresent && native.context.mask1898 == expected.contextMask, "manager live queue/context mismatch");
    for (unsigned i = 0; i < 64; ++i) require(eventWords(native.events[i]).words == expected.events[i].words, "manager full event storage mismatch");
    for (unsigned i = 0; i < 32; ++i) {
        require(recordWords(native.contextRecords[i]).words == expected.contextRecords[i].words, "manager context bank mismatch");
        require(recordWords(native.auxiliaryRecords[i]).words == expected.auxiliaryRecords[i].words, "manager auxiliary bank mismatch");
        for (unsigned d = 0; d < 8; ++d) require(recordWords(native.devices[d][i]).words == expected.deviceRecords[d][i].words, "manager device bank mismatch");
    }
    for (unsigned i = 0; i < oracleCalls; ++i) compareTrace(native.traces[i], stats.callbacks[i]);
    ++managers; accumulate(stats);
}

enum RouterChange : u32 { ChangeParent = 1, ChangeFirst = 2, ChangeNext = 4, ChangeFlags = 8,
    ChangeRouterCode = 16, ChangeRouterTag = 32, ReplaceChildren = 64, ReplaceChildCount = 128 };
struct RouterEffect {
    u32 changes = 0, parentFlags = 0, first = original48z::noChild;
    u32 nextNode = 0, next = original48z::noChild, flagsNode = 0;
    std::uint8_t flags = 0;
    u32 code = 0, tag = 0, recordXor = 0, result = 0;
    bool available = true;
};
struct RouterScript { std::array<RouterEffect, 64> effects{}; };
static void oracleEffect(original48z::RouterState& s, const RouterEffect& e) {
    if (e.changes & ChangeParent) s.parentFlags24 = e.parentFlags;
    if (e.changes & ChangeFirst) s.firstChild = e.first;
    if (e.changes & ChangeNext) s.nodes[e.nextNode].next = e.next;
    if (e.changes & ChangeFlags) s.nodes[e.flagsNode].flags29 = e.flags;
    if (e.changes & ChangeRouterCode) s.event.words[0] = e.code;
    if (e.changes & ChangeRouterTag) s.event.words[4] = e.tag;
    for (auto& word : s.record.words) word ^= e.recordXor;
}
struct NativeRouter {
    std::array<EventChild48Z, 32> nodes{}, replacement{};
    EventChildRouter48Z router{};
    InputEvent48Z event{};
    DeviceButtonRecord48Y record{};
    RouterScript script{};
    std::array<original48z::CallbackTrace, 64> traces{};
    unsigned calls = 0;
    bool identitiesValid = true;
    explicit NativeRouter(const original48z::RouterState& s, const RouterScript& actions = {}) : script(actions) {
        for (unsigned i = 0; i < 32; ++i) nodes[i] = {s.nodes[i].next, s.nodes[i].flags29};
        router = {s.parentFlags24, s.firstChild, nodes.data(), 32};
        std::memcpy(&event, s.event.words.data(), 20); std::memcpy(&record, s.record.words.data(), 24);
    }
    static EventCallbackResult48Z callback(void* p, u32 child, bool press, InputEvent48Z& event, DeviceButtonRecord48Y& record) noexcept {
        auto& n = *static_cast<NativeRouter*>(p);
        if (n.calls >= n.traces.size()) { n.identitiesValid = false; return {false, 0}; }
        n.identitiesValid = n.identitiesValid && &event == &n.event && &record == &n.record;
        auto& t = n.traces[n.calls]; const auto& e = n.script.effects[n.calls++];
        t.pc = press ? 0x1423b4 : 0x142304; t.press = press; t.childIndex = child;
        t.category = event.category; t.device = event.deviceId; t.code = event.code;
        t.eventAddress = original48z::Machine::routerEvent; t.recordAddress = original48z::Machine::routerRecord;
        t.eventBefore = eventWords(event); t.recordBefore = recordWords(record); t.result = e.result;
        if (e.changes & ChangeParent) n.router.flags24 = e.parentFlags;
        if (e.changes & ChangeFirst) n.router.firstChild = e.first;
        if (e.changes & ChangeNext) n.nodes[e.nextNode].next = e.next;
        if (e.changes & ChangeFlags) n.nodes[e.flagsNode].flags29 = e.flags;
        if (e.changes & ChangeRouterCode) event.code = e.code;
        if (e.changes & ChangeRouterTag) event.contextMask = e.tag;
        if (e.changes & ReplaceChildren) n.router.children = n.replacement.data();
        if (e.changes & ReplaceChildCount) --n.router.childCount;
        xorRecord(record, e.recordXor);
        t.eventAfter = eventWords(event); t.recordAfter = recordWords(record);
        return {e.available, e.result};
    }
};
static original48z::RouterState seedRouter() {
    original48z::RouterState s; s.parentFlags24 = 0x600; s.firstChild = 0;
    for (unsigned i = 0; i < 32; ++i) s.nodes[i] = {i == 31 ? original48z::noChild : i + 1, 1};
    s.event.words = {17, 2, 7, 1, 0x88776655}; s.record.words = {1, 0x7fc12345, 3, 4, 5, 6};
    return s;
}
static void compareRouter(original48z::RouterState expected, bool press, const RouterScript& script = {}) {
    NativeRouter native(expected, script); original48z::Stats stats; unsigned calls = 0;
    const bool handled = original48z::router(expected, press, [&](original48z::RouterState& s, unsigned, bool) {
        const auto& effect = script.effects.at(calls++); oracleEffect(s, effect); return effect.result;
    }, &stats);
    const auto got = routeChildInputEvent48Z(native.router, press, native.event, native.record, NativeRouter::callback, &native, 64);
    require(got.status == EventRouteStatus48Z::Complete && got.handled == handled, "router result differential");
    require(got.visits == stats.visits(press ? 0x142384u : 0x1422d4u), "router visit count differential");
    require(got.callbacksInvoked == calls && native.calls == calls && native.identitiesValid, "router callback count/identity mismatch");
    require(native.router.flags24 == expected.parentFlags24 && native.router.firstChild == expected.firstChild, "router parent mutation differential");
    for (unsigned i = 0; i < 32; ++i) require(native.nodes[i].next == expected.nodes[i].next && native.nodes[i].flags29 == expected.nodes[i].flags29, "router live-node mutation differential");
    require(eventWords(native.event).words == expected.event.words && recordWords(native.record).words == expected.record.words, "router event/record state differential");
    for (unsigned i = 0; i < calls; ++i) compareTrace(native.traces[i], stats.callbacks[i]);
    ++routers; accumulate(stats);
}

static void testManagerDifferentials() {
    auto s = seedManager();
    for (std::int32_t count : {0, -1, -17, std::numeric_limits<std::int32_t>::min()}) { s.count = count; compareManager(s); }
    for (unsigned category = 0; category < 3; ++category) for (unsigned code = 0; code < 32; ++code)
        for (unsigned device = 0; device < (category == 2 ? 8u : 1u); ++device)
            for (u32 transition : {0u, 1u, 2u, 0xffffffffu}) {
                s.count = 1; s.events[0].words = {code, category, device, transition, 0};
                ManagerScript script; script.effects[0].result = code & 1 ? 0xffffffffu : 0;
                script.effects[0].recordXor = 0xabcdef01;
                compareManager(s, script);
            }
    for (unsigned category : {0u, 1u}) {
        s = seedManager(); s.count = 1; s.events[0].words = {31, category, 0xffffffffu, 0, 0}; compareManager(s);
        s.events[0].words[3] = 1; compareManager(s);
    }
    // No context still tags category1/2 events with zero; no transition does not
    // resolve a missing record or missing virtual callback in the original.
    s = seedManager(); s.contextPresent = false; s.count = 4;
    s.events[0].words = {1, 1, 7, 0, 77}; s.events[1].words = {31, 2, 7, 1, 88};
    s.events[2].words = {0xffffffffu, 0, 0xffffffffu, 2, 99};
    s.events[3].words = {0xffffffffu, 2, 0xffffffffu, 0xffffffffu, 99}; compareManager(s);
    // Category is selected once. Both callback sites retain that category even
    // if release changes category, code, device, transition and tag together.
    for (unsigned category = 0; category < 3; ++category) for (u32 newCategory : {0u, 1u, 2u, 99u}) {
        s = seedManager(); s.count = 2; s.events[0].words = {3, category, 0, 0, 999};
        s.events[1].words = {2, 2, 1, 1, 888};
        ManagerScript script; auto& e = script.effects[0];
        e.changes = ChangeCode | ChangeCategory | ChangeDevice | ChangeTransition | ChangeTag | ChangeContextMask;
        e.code = 29; e.category = newCategory; e.device = 7; e.transition = 1; e.tag = 0xfacefeed; e.contextMask = 0x76543210;
        e.recordXor = 0xf0f0f0f0; e.result = 0xffffffff;
        script.effects[1].recordXor = 0x00ff00ff; script.effects[1].result = 1;
        compareManager(s, script);
    }
    // A changing context is read at every event, never retagged between release
    // and press; retained context records stay valid while presence changes.
    s = seedManager(); s.count = 4;
    for (unsigned i = 0; i < 4; ++i) s.events[i].words = {i, 2, i, 0, 0xffffffff};
    ManagerScript live;
    live.effects[0].changes = ChangeContextPresent | ChangeContextMask; live.effects[0].contextPresent = false; live.effects[0].contextMask = 0x123;
    live.effects[1].changes = ChangeContextPresent | ChangeContextMask; live.effects[1].contextPresent = true; live.effects[1].contextMask = 0x80000000;
    live.effects[2].changes = ChangeContextMask; live.effects[2].contextMask = 0;
    compareManager(s, live);
    // Growing count exposes a freshly supplied event, including at the last
    // storage slot; shrinking does not cancel the already-selected event.
    for (std::int32_t nextCount : {0, -1, std::numeric_limits<std::int32_t>::min(), 1, 2, 64}) {
        s = seedManager(); s.count = 1; s.events[0].words = {2, 2, 1, 0, 5};
        ManagerScript script; auto& e = script.effects[0];
        e.changes = ChangeCount | ChangeTransition | ChangeFuture; e.count = nextCount; e.transition = 1;
        e.futureIndex = nextCount == 64 ? 63 : 1; e.future.words = {31, 1, 7, 0, 123};
        e.result = 0x80000000; e.recordXor = 0x12345678; compareManager(s, script);
    }
    s = seedManager(); s.count = 4;
    ManagerScript shrink; shrink.effects[1].changes = ChangeCount; shrink.effects[1].count = 1; compareManager(s, shrink);
    // Repeated events pass the same record address, with earlier callback edits
    // visible to later callbacks. Float payloads are compared as opaque words.
    s = seedManager(); s.count = 6; ManagerScript repeated;
    for (unsigned i = 0; i < 6; ++i) { s.events[i].words = {31, 2, 7, i & 1, 1}; repeated.effects[i].recordXor = 0x7f812345u + i; }
    s.deviceRecords[7][31].words[1] = 0xff800000; compareManager(s, repeated);
    u32 random = 0x48a5f00du;
    auto next = [&]() { random ^= random << 13; random ^= random >> 17; random ^= random << 5; return random; };
    for (unsigned n = 0; n < 1200; ++n) {
        s = seedManager(); s.count = 1 + next() % 64; s.contextMask = next(); ManagerScript script;
        for (unsigned i = 0; i < 64; ++i) s.events[i].words = {next() % 32, next() % 3, next() % 8, next() % 4, next()};
        for (auto& e : script.effects) {
            e.result = next(); e.recordXor = next();
            if ((next() & 3) == 0) { e.changes = ChangeCode | ChangeDevice | ChangeTransition | ChangeTag | ChangeContextMask; e.code = next() % 32; e.device = next() % 8; e.transition = next() % 4; e.tag = next(); e.contextMask = next(); }
        }
        compareManager(s, script);
    }
}

static void testRouterDifferentials() {
    for (bool press : {false, true}) {
        auto s = seedRouter(); s.firstChild = original48z::noChild; compareRouter(s, press);
        // Exhaust every child byte against each relevant root gate combination.
        for (u32 parent : {0u, 0x200u, 0x400u, 0x600u, 0xfffff9ffu, 0xffffffffu})
            for (unsigned flags = 0; flags < 256; ++flags) {
                s = seedRouter(); s.parentFlags24 = parent; s.nodes[0] = {1, static_cast<std::uint8_t>(flags)};
                s.nodes[1] = {2, static_cast<std::uint8_t>(flags ^ 9)}; s.nodes[2] = {original48z::noChild, 1};
                RouterScript script; script.effects[0].recordXor = 0x12345678;
                script.effects[1].changes = ChangeRouterCode | ChangeRouterTag; script.effects[1].code = 23; script.effects[1].tag = 0xabcdef;
                compareRouter(s, press, script);
            }
        for (unsigned stop = 0; stop < 32; ++stop) for (u32 result : {1u, 0x80000000u, 0xffffffffu}) {
            s = seedRouter(); RouterScript script;
            for (unsigned i = 0; i <= stop; ++i) { script.effects[i].recordXor = 0x10101 * (i + 1); script.effects[i].changes = ChangeRouterTag; script.effects[i].tag = i; }
            script.effects[stop].result = result; compareRouter(s, press, script);
        }
        // Parent gate and first link are snapshots; each visited child's next
        // and flags are live. Skip node1, enable node2, then stop after node4.
        s = seedRouter(); s.nodes[2].flags29 = 8; RouterScript live;
        live.effects[0].changes = ChangeParent | ChangeFirst | ChangeNext | ChangeFlags;
        live.effects[0].parentFlags = 0; live.effects[0].first = original48z::noChild;
        live.effects[0].nextNode = 0; live.effects[0].next = 2; live.effects[0].flagsNode = 2; live.effects[0].flags = 1;
        live.effects[1].changes = ChangeNext | ChangeFlags; live.effects[1].nextNode = 2; live.effects[1].next = 3; live.effects[1].flagsNode = 3; live.effects[1].flags = 9;
        live.effects[2].changes = ChangeNext; live.effects[2].nextNode = 4; live.effects[2].next = original48z::noChild;
        compareRouter(s, press, live);
        // Creating a cycle is safe for the differential only when an explicit
        // later callback consumes it; an unhandled cycle has native-only bounds.
        s = seedRouter(); RouterScript cycle;
        cycle.effects[0].changes = ChangeNext; cycle.effects[0].nextNode = 0; cycle.effects[0].next = 0;
        cycle.effects[7].result = 0xffffffff; compareRouter(s, press, cycle);
        s = seedRouter(); RouterScript terminate;
        terminate.effects[0].changes = ChangeNext; terminate.effects[0].nextNode = 0; terminate.effects[0].next = original48z::noChild;
        compareRouter(s, press, terminate);
    }
}

static void testManagerBoundaries() {
    auto s = seedManager(); s.count = 1; s.events[0].words = {3, 2, 7, 0, 0x1234};
    for (unsigned variant = 0; variant < 2; ++variant) {
        NativeManager n(s); if (variant) n.queue.capacity = 0; else n.queue.events = nullptr;
        const auto before = n.events;
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
        require(got.status == EventDispatchStatus48Z::InvalidQueue && !got.eventsCompleted && !got.callbacksInvoked, "initial invalid queue status");
        require(std::memcmp(n.events.data(), before.data(), sizeof before) == 0 && !n.calls, "initial invalid queue changed storage");
    }
    for (std::int32_t count : {0, -1, std::numeric_limits<std::int32_t>::min()}) {
        InputEventQueue48Z empty{nullptr, 0, count};
        const auto got = dispatchInputEvents48Z(empty, {}, {}, {});
        require(got.status == EventDispatchStatus48Z::Complete && !got.eventsCompleted && !got.callbacksInvoked && empty.count == count, "signed empty queue touched absent bindings");
    }
    for (unsigned variant = 0; variant < 6; ++variant) {
        NativeManager n(s);
        if (variant == 0) { n.events[0].category = 0; n.context.present = false; }
        if (variant == 1) { n.events[0].category = 1; n.sources.auxiliary = {}; }
        if (variant == 2) n.sources.devices[7] = {};
        if (variant == 3) n.events[0].deviceId = 8;
        if (variant == 4) n.events[0].code = 32;
        if (variant == 5) n.sources.devices[7].count = 3;
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
        require(got.status == EventDispatchStatus48Z::MissingRecord && !got.callbacksInvoked && !got.eventsCompleted, "missing record domain accepted");
        require(n.events[0].contextMask == (n.context.present ? s.contextMask : 0), "failed event must retain completed live tag write");
    }
    for (u32 category : {3u, 0xffffffffu}) {
        NativeManager n(s); n.events[0].category = category;
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
        require(got.status == EventDispatchStatus48Z::UnsupportedCategory && !got.callbacksInvoked, "unknown category accepted");
        require(n.events[0].contextMask == s.contextMask, "unsupported category lost prior tagging effect");
    }
    for (u32 transition : {0u, 1u, 2u, 0xffffffffu}) {
        NativeManager n(s); n.events[0].transition = transition;
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, {});
        require(got.status == (transition <= 1 ? EventDispatchStatus48Z::MissingCallback : EventDispatchStatus48Z::Complete), "unneeded callback domain failure");
        require(!got.callbacksInvoked && got.eventsCompleted == (transition > 1 ? 1u : 0u), "missing callback progress mismatch");
    }
    // Failure after an external callback cannot be whole-call atomic. Verify
    // effects on the in-flight event and record, plus completed earlier events.
    s.count = 2; s.events[1].words = {4, 2, 7, 0, 0x9876};
    for (u32 changes : {0u, u32(ReplaceEvents), u32(ReplaceCapacity), u32(ChangeCount)}) {
        ManagerScript script; script.effects[0].recordXor = 0x11111111;
        auto& e = script.effects[1]; e.changes = changes | ChangeTag; e.tag = 0xdeadbeef; e.recordXor = 0x22222222; e.count = 65;
        e.available = changes != 0;
        NativeManager n(s, script);
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
        const auto expected = changes == 0 ? EventDispatchStatus48Z::CallbackUnavailable : changes == ChangeCount ? EventDispatchStatus48Z::InvalidQueue : EventDispatchStatus48Z::StorageChanged;
        require(got.status == expected && got.eventsCompleted == 1 && got.callbacksInvoked == 2, "late manager error/progress mismatch");
        require(n.events[0].contextMask == s.contextMask && n.events[1].contextMask == e.tag, "late error rolled back event effects");
        require(recordWords(n.devices[7][3]).words[0] == (s.deviceRecords[7][3].words[0] ^ 0x11111111) && recordWords(n.devices[7][4]).words[5] == (s.deviceRecords[7][4].words[5] ^ 0x22222222), "late error rolled back record effects");
    }
    s.count = 1;
    for (bool missingRecord : {false, true}) {
        ManagerScript script; auto& e = script.effects[0];
        e.changes = ChangeTransition | ChangeCode | ChangeTag; e.transition = 1; e.code = 5; e.tag = 42; e.recordXor = 0xffffffff;
        if (missingRecord) { e.changes |= RemoveDevice; e.device = 7; }
        NativeManager n(s, script); auto callbacks = n.callbacks(); if (!missingRecord) callbacks.press = nullptr;
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, callbacks);
        require(got.status == (missingRecord ? EventDispatchStatus48Z::MissingRecord : EventDispatchStatus48Z::MissingCallback) && !got.eventsCompleted && got.callbacksInvoked == 1, "late release-to-press boundary mismatch");
        require(n.events[0].code == 5 && n.events[0].contextMask == 42 && recordWords(n.devices[7][3]).words[2] == (s.deviceRecords[7][3].words[2] ^ 0xffffffff), "failed second dispatch lost release effects");
    }
    // Queue storage remains stable while explicit record bindings are replaced.
    // The subsequent press must resolve the new live bank, not cache release's.
    for (unsigned category = 0; category < 3; ++category) {
        auto source = seedManager(); source.count = 1; source.events[0].words = {3, category, 7, 0, 0};
        ManagerScript script; script.effects[0].changes = ChangeTransition | (category == 0 ? ReplaceContextRecords : category == 1 ? ReplaceAuxiliaryRecords : ReplaceDeviceRecords);
        script.effects[0].transition = 1; script.effects[0].recordXor = 0x12345678; script.effects[1].recordXor = 0xabcdef01;
        NativeManager n(source, script);
        const auto got = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
        const u32 expected = category == 0 ? original48z::Machine::auxiliary + 0x1c + 24 * 3 : category == 1 ? original48z::Machine::context + 0x20 + 24 * 3 : original48z::Machine::deviceAddress(6) + 0x324 + 24 * 3;
        require(got.status == EventDispatchStatus48Z::Complete && got.eventsCompleted == 1 && got.callbacksInvoked == 2, "live record rebinding rejected");
        require(n.traces[0].recordAddress != n.traces[1].recordAddress && n.traces[1].recordAddress == expected, "press cached release's record binding");
        require(n.traces[0].recordAfter.words[0] == (n.traces[0].recordBefore.words[0] ^ 0x12345678) && n.traces[1].recordAfter.words[0] == (n.traces[1].recordBefore.words[0] ^ 0xabcdef01), "live rebinding lost distinct callback effects");
    }
    // Stable typed aliases: independent categories may intentionally bind the
    // same complete native record bank, and the second event sees the mutation.
    s.count = 2; s.events[0].words = {3, 0, 0, 0, 0}; s.events[1].words = {3, 1, 0, 1, 0};
    ManagerScript aliases; aliases.effects[0].recordXor = 0xabcdef01; NativeManager n(s, aliases);
    n.context.records = {n.devices[7].data(), 32}; n.sources.auxiliary = n.context.records;
    const auto alias = dispatchInputEvents48Z(n.queue, n.context, n.sources, n.callbacks());
    require(alias.status == EventDispatchStatus48Z::Complete && alias.eventsCompleted == 2 && n.calls == 2, "typed record-bank aliases rejected");
    require(n.traces[0].recordAddress == n.traces[1].recordAddress && n.traces[1].recordBefore.words == n.traces[0].recordAfter.words, "typed aliases lost identity/live edits");
}

static void testRouterBoundaries() {
    for (bool press : {false, true}) {
        auto s = seedRouter();
        NativeRouter disabled(s); disabled.router.flags24 = 0; disabled.router.children = nullptr; disabled.router.firstChild = 999;
        auto got = routeChildInputEvent48Z(disabled.router, press, disabled.event, disabled.record, nullptr, nullptr, 0);
        require(got.status == EventRouteStatus48Z::Complete && !got.handled && !got.visits, "root mask must gate before binding access");
        NativeRouter empty(s); empty.router.firstChild = noEventChild48Z;
        got = routeChildInputEvent48Z(empty.router, press, empty.event, empty.record, nullptr, nullptr, 0);
        require(got.status == EventRouteStatus48Z::Complete && !got.visits, "empty child list required callback/budget");
        for (unsigned variant = 0; variant < 2; ++variant) {
            NativeRouter n(s); if (variant) n.router.firstChild = 32; else n.router.children = nullptr;
            got = routeChildInputEvent48Z(n.router, press, n.event, n.record, NativeRouter::callback, &n, 32);
            require(got.status == EventRouteStatus48Z::InvalidNode && !got.visits && !got.callbacksInvoked, "invalid router binding accepted");
        }
        NativeRouter missing(s);
        got = routeChildInputEvent48Z(missing.router, press, missing.event, missing.record, nullptr, nullptr, 32);
        require(got.status == EventRouteStatus48Z::MissingCallback && got.visits == 1 && !got.callbacksInvoked, "missing child callback mismatch");
        for (auto& node : missing.nodes) node.flags29 = 8;
        got = routeChildInputEvent48Z(missing.router, press, missing.event, missing.record, nullptr, nullptr, 32);
        require(got.status == EventRouteStatus48Z::Complete && got.visits == 32 && !got.callbacksInvoked, "skipped children required callback");
        RouterScript growCycle; growCycle.effects[0].changes = ChangeNext; growCycle.effects[0].nextNode = 0; growCycle.effects[0].next = 0;
        NativeRouter cycle(s, growCycle);
        got = routeChildInputEvent48Z(cycle.router, press, cycle.event, cycle.record, NativeRouter::callback, &cycle, 7);
        require(got.status == EventRouteStatus48Z::VisitBudgetExhausted && got.visits == 7 && got.callbacksInvoked == 7, "unhandled cycle not bounded");
        NativeRouter skippedCycle(s); skippedCycle.nodes[0] = {0, 8};
        got = routeChildInputEvent48Z(skippedCycle.router, press, skippedCycle.event, skippedCycle.record, nullptr, nullptr, 3);
        require(got.status == EventRouteStatus48Z::VisitBudgetExhausted && got.visits == 3 && !got.callbacksInvoked, "skipped-node cycle escaped visit budget");
        NativeRouter budget(s);
        got = routeChildInputEvent48Z(budget.router, press, budget.event, budget.record, NativeRouter::callback, &budget, 0);
        require(got.status == EventRouteStatus48Z::VisitBudgetExhausted && !got.visits && !got.callbacksInvoked, "zero budget touched child");
        for (u32 change : {0u, u32(ReplaceChildren), u32(ReplaceChildCount), u32(ChangeNext)}) {
            RouterScript script; auto& e = script.effects[1]; e.recordXor = 0xabcdef01;
            e.changes = change | ChangeRouterCode; e.code = 99; e.nextNode = 1; e.next = 99; e.available = change != 0;
            NativeRouter n(s, script);
            got = routeChildInputEvent48Z(n.router, press, n.event, n.record, NativeRouter::callback, &n, 32);
            const auto status = change == 0 ? EventRouteStatus48Z::CallbackUnavailable : change == ChangeNext ? EventRouteStatus48Z::InvalidNode : EventRouteStatus48Z::StorageChanged;
            require(got.status == status && !got.handled && got.visits == 2 && got.callbacksInvoked == 2, "late router boundary/progress mismatch");
            require(n.event.code == 99 && recordWords(n.record).words[1] == (s.record.words[1] ^ 0xabcdef01), "late router failure rolled back effects");
        }
        // A consuming callback ends traversal. Replaced storage or an invalid
        // next link is not accessed after the first handled result.
        RouterScript consumed; consumed.effects[0].changes = ReplaceChildren | ChangeNext;
        consumed.effects[0].nextNode = 0; consumed.effects[0].next = 99; consumed.effects[0].result = 1;
        NativeRouter n(s, consumed);
        got = routeChildInputEvent48Z(n.router, press, n.event, n.record, NativeRouter::callback, &n, 32);
        require(got.status == EventRouteStatus48Z::Complete && got.handled && got.visits == 1 && got.callbacksInvoked == 1, "handled child incorrectly examined next storage");
    }
}

static void testAppendBridge() {
    DeviceButtonBank48Y previous{}; DeviceButtonDescriptors48Y descriptors{};
    for (unsigned i = 0; i < 16; ++i) { previous.records[i].held = i & 1; descriptors.records[i] = {0, 0, static_cast<std::uint16_t>(1u << i)}; }
    DevicePacket48Y packet{}; packet.bytes[2] = 0xaa; packet.bytes[3] = 0xaa;
    DeviceButtonSample48Y batch{};
    require(sampleDeviceButtons48Y(true, true, packet, descriptors, 16, 7, previous, batch), "48Y source sample failed");
    require(batch.transitionCount == 16, "48Y alternating transitions fixture");
    std::array<original48y::Record, 32> originalPrevious{};
    std::array<original48y::Descriptor, 32> originalDescriptors{};
    std::array<std::uint8_t, 32> originalPacket{};
    for (unsigned i = 0; i < 32; ++i) {
        std::memcpy(originalPrevious[i].words.data(), &previous.records[i], 24);
        originalDescriptors[i] = {descriptors.records[i].flags, descriptors.records[i].packetOffset, descriptors.records[i].digitalMask};
    }
    std::memcpy(originalPacket.data(), packet.bytes, 32);
    const auto originalBatch = original48y::sample(originalPrevious, originalDescriptors, 16, 7, originalPacket, true);
    require(batch.transitionCount == originalBatch.eventCount && originalBatch.packetAccepted, "48Y bridge original batch count mismatch");
    for (unsigned i = 0; i < 32; ++i) require(recordWords(batch.buttons.records[i]).words == originalBatch.records[i].words, "48Y bridge original record mismatch");
    std::array<InputEvent48Z, 64> events{}; InputEventQueue48Z queue{events.data(), 64, 2};
    events[0] = {31, 1, 4, 0, 123}; events[1] = {30, 0, 5, 1, 456}; const auto before = events;
    require(appendDeviceTransitions48Z(batch, queue) && queue.count == 18, "48Y ordered append rejected");
    require(std::memcmp(events.data(), before.data(), 40) == 0, "append replaced existing events");
    for (unsigned i = 0; i < batch.transitionCount; ++i) {
        require(std::memcmp(&events[i + 2], &batch.transitions[i], 20) == 0, "48Y appended words/order mismatch");
        require(eventWords(events[i + 2]).words == originalBatch.events[i], "48Y-to48Z original queue words/order mismatch");
    }
    auto s = seedManager(); s.count = queue.count;
    for (unsigned i = 0; i < 64; ++i) s.events[i] = eventWords(events[i]);
    for (unsigned i = 0; i < 32; ++i) s.deviceRecords[7][i].words = originalBatch.records[i].words;
    compareManager(s); // Production48Y batch through native48Z and original manager.
    const auto appended = events; auto* const storage = queue.events; const auto capacity = queue.capacity;
    resetInputEventCount48Z(queue);
    require(!queue.count && queue.events == storage && queue.capacity == capacity && std::memcmp(events.data(), appended.data(), sizeof events) == 0, "reset changed queue ownership/payload");
    require(appendDeviceTransitions48Z(batch, queue) && queue.count == 16, "append after reset failed");
    for (unsigned variant = 0; variant < 6; ++variant) {
        auto bad = batch; InputEventQueue48Z q{events.data(), 64, 16};
        if (variant == 0) bad.transitionCount = 33;
        if (variant == 1) q.count = -1;
        if (variant == 2) q.count = 65;
        if (variant == 3) q.capacity = 31;
        if (variant == 4) q.events = nullptr;
        if (variant == 5) { q.count = std::numeric_limits<std::int32_t>::max(); q.capacity = 0xffffffff; }
        const auto saved = events; const auto count = q.count; auto* const pointer = q.events; const auto cap = q.capacity;
        require(!appendDeviceTransitions48Z(bad, q), "invalid bounded append accepted");
        require(q.count == count && q.events == pointer && q.capacity == cap && std::memcmp(events.data(), saved.data(), sizeof events) == 0, "failed append was not atomic");
    }
    DeviceButtonSample48Y none{}; InputEventQueue48Z empty{nullptr, 0, 0};
    require(appendDeviceTransitions48Z(none, empty) && !empty.count, "empty append required storage");
    queue = {events.data(), 16, 0}; require(appendDeviceTransitions48Z(batch, queue) && queue.count == 16, "exact append boundary rejected");
}

struct BridgeProbe {
    struct Visit { u32 child; bool press; InputEvent48Z* event; DeviceButtonRecord48Y* record; };
    std::array<Visit, 256> visits{};
    unsigned count = 0;
    u32 consumeChild = 1;
    bool available = true;
    JoystickApplicationBridge48Z* mutateApplication = nullptr;
    static EventCallbackResult48Z callback(void* p, u32 child, bool press, InputEvent48Z& event, DeviceButtonRecord48Y& record) noexcept {
        auto& self = *static_cast<BridgeProbe*>(p);
        if (self.count == self.visits.size()) return {false, 0};
        self.visits[self.count++] = {child, press, &event, &record};
        ++record.auxiliary14;
        if (self.mutateApplication) self.mutateApplication->rootFlags228 |= 4;
        return {self.available, child == self.consumeChild ? 0x80000000u : 0u};
    }
};
static void testApplicationBridge() {
    // This is a test of the explicitly qualified native bridge contract. The
    // wrapper provenance is static; no full original wrapper oracle is claimed.
    for (unsigned flags = 0; flags < 256; ++flags) for (bool press : {false, true}) {
        std::array<EventChild48Z, 3> children{{{1, 1}, {2, 1}, {noEventChild48Z, 1}}}; BridgeProbe probe;
        JoystickApplicationBridge48Z app{static_cast<std::uint8_t>(flags), {0x600, 0, children.data(), 3}, BridgeProbe::callback, &probe, 3, {}};
        InputEvent48Z event{4, 2, 7, press ? 1u : 0u, 0x12345678}; DeviceButtonRecord48Y record{1, 1.0f, 2, 3, 4, 5};
        const auto got = press ? joystickPressBridge48Z(&app, event, record) : joystickReleaseBridge48Z(&app, event, record);
        if (flags & 8) {
            require(got.available && got.originalReturn && app.lastRoute.handled && !probe.count, "root bit3 bridge consumption failed");
        } else if (press && (flags & 4)) {
            require(!got.available && app.lastRoute.status == EventRouteStatus48Z::UnsupportedMode && !probe.count, "special press mode accepted");
        } else {
            require(got.available && got.originalReturn && app.lastRoute.handled && probe.count == 2 && app.lastRoute.visits == 2, "normal category2 bridge routing failed");
            for (unsigned i = 0; i < probe.count; ++i) require(probe.visits[i].event == &event && probe.visits[i].record == &record && probe.visits[i].child == i && probe.visits[i].press == press, "bridge child exact binding mismatch");
        }
    }
    std::array<EventChild48Z, 3> children{{{1, 1}, {2, 1}, {noEventChild48Z, 1}}}; BridgeProbe probe;
    JoystickApplicationBridge48Z app{0, {0x600, 0, children.data(), 3}, BridgeProbe::callback, &probe, 3, {}};
    InputEvent48Z event{0, 2, 0, 1, 0}; DeviceButtonRecord48Y record{};
    for (u32 category : {0u, 1u, 3u, 0xffffffffu}) {
        event.category = category;
        for (auto callback : {joystickReleaseBridge48Z, joystickPressBridge48Z}) {
            const auto got = callback(&app, event, record);
            require(!got.available && app.lastRoute.status == EventRouteStatus48Z::UnsupportedCategory, "bridge unproven category accepted");
        }
    }
    event.category = 2;
    require(!joystickReleaseBridge48Z(nullptr, event, record).available && !joystickPressBridge48Z(nullptr, event, record).available, "null application binding accepted");
    for (u32 rootMask : {0u, 0x200u, 0x400u, 0x600u}) for (bool press : {false, true}) {
        probe.count = 0; app.root.flags24 = rootMask;
        const auto got = press ? joystickPressBridge48Z(&app, event, record) : joystickReleaseBridge48Z(&app, event, record);
        require(got.available && (got.originalReturn != 0) == ((rootMask & (press ? 0x400u : 0x200u)) != 0), "bridge root gate contract mismatch");
    }
    app.root.flags24 = 0x600; app.childCallback = nullptr;
    require(!joystickPressBridge48Z(&app, event, record).available && app.lastRoute.status == EventRouteStatus48Z::MissingCallback, "unbound bridge fabricated children");
    app.childCallback = BridgeProbe::callback;
    probe.mutateApplication = &app; probe.consumeChild = noEventChild48Z; probe.count = 0; app.rootFlags228 = 0;
    require(!joystickPressBridge48Z(&app, event, record).available && app.lastRoute.status == EventRouteStatus48Z::UnsupportedMode && probe.count == 3, "live post-routing special mode silently skipped");
    probe.consumeChild = 1; probe.count = 0; app.rootFlags228 = 0;
    require(joystickPressBridge48Z(&app, event, record).available && app.lastRoute.handled && probe.count == 2, "handled child incorrectly entered special fallback");
    probe.mutateApplication = nullptr; app.rootFlags228 = 0; probe.count = 0;
    std::array<InputEvent48Z, 2> events{{{3, 2, 7, 0, 999}, {5, 2, 7, 1, 888}}};
    std::array<DeviceButtonRecord48Y, 32> bank{};
    InputEventQueue48Z queue{events.data(), 2, 2}; EventContext48Z context{true, 0x4567, {}}; EventRecordSources48Z sources{}; sources.devices[7] = {bank.data(), 32};
    auto got = dispatchInputEvents48Z(queue, context, sources, {&app, joystickReleaseBridge48Z, joystickPressBridge48Z});
    require(got.status == EventDispatchStatus48Z::Complete && got.eventsCompleted == 2 && got.callbacksInvoked == 2 && probe.count == 4, "child consumption incorrectly stopped manager continuation");
    for (unsigned i = 0; i < 4; ++i) require(probe.visits[i].event == &events[i / 2] && probe.visits[i].record == &bank[i < 2 ? 3 : 5] && probe.visits[i].child == i % 2 && probe.visits[i].press == (i >= 2), "manager-to-application concrete child route mismatch");
    require(events[0].contextMask == context.mask1898 && events[1].contextMask == context.mask1898 && bank[3].auxiliary14 == 2 && bank[5].auxiliary14 == 2, "bridge child effects/tagging not preserved");
    probe.count = 0; app.rootFlags228 = 8;
    got = dispatchInputEvents48Z(queue, context, sources, {&app, joystickReleaseBridge48Z, joystickPressBridge48Z});
    require(got.status == EventDispatchStatus48Z::Complete && got.eventsCompleted == 2 && got.callbacksInvoked == 2 && !probe.count, "root consumption incorrectly stopped manager queue");
    app.rootFlags228 = 0; probe.available = false;
    got = dispatchInputEvents48Z(queue, context, sources, {&app, joystickReleaseBridge48Z, joystickPressBridge48Z});
    require(got.status == EventDispatchStatus48Z::CallbackUnavailable && !got.eventsCompleted && got.callbacksInvoked == 1 && probe.count == 1 && bank[3].auxiliary14 == 3, "unavailable bridge child lost partial effects");
}
struct HotCallbacks {
    std::uint64_t calls = 0;
    static EventCallbackResult48Z manager(void* p, InputEvent48Z&, DeviceButtonRecord48Y& r) noexcept { ++static_cast<HotCallbacks*>(p)->calls; ++r.auxiliary08; return {true, 0xffffffff}; }
    static EventCallbackResult48Z child(void* p, u32 child, bool, InputEvent48Z&, DeviceButtonRecord48Y& r) noexcept { ++static_cast<HotCallbacks*>(p)->calls; ++r.auxiliary0C; return {true, child == 2 ? 1u : 0u}; }
};
static void testAllocationFree() {
    std::array<InputEvent48Z, 8> events{}; for (unsigned i = 0; i < 8; ++i) events[i] = {0, 2, 0, i & 1, 0};
    DeviceButtonRecord48Y record{}; InputEventQueue48Z queue{events.data(), 8, 8}; EventRecordSources48Z sources{}; sources.devices[0] = {&record, 1};
    EventContext48Z context{true, 0xabcdef, {}}; HotCallbacks hot;
    std::array<EventChild48Z, 4> nodes{{{1, 1}, {2, 9}, {3, 1}, {noEventChild48Z, 1}}}; EventChildRouter48Z router{0x600, 0, nodes.data(), 4};
    JoystickApplicationBridge48Z app{0, router, HotCallbacks::child, &hot, 4, {}};
    const auto before = allocations;
    for (unsigned i = 0; i < 20000; ++i) {
        const auto manager = dispatchInputEvents48Z(queue, context, sources, {&hot, HotCallbacks::manager, HotCallbacks::manager});
        require(manager.status == EventDispatchStatus48Z::Complete && manager.eventsCompleted == 8, "hot manager failed");
        const auto child = routeChildInputEvent48Z(router, (i & 1) != 0, events[0], record, HotCallbacks::child, &hot, 4);
        require(child.status == EventRouteStatus48Z::Complete && child.handled && child.visits == 3, "hot router failed");
        const auto bridge = dispatchInputEvents48Z(queue, context, sources, {&app, joystickReleaseBridge48Z, joystickPressBridge48Z});
        require(bridge.status == EventDispatchStatus48Z::Complete && bridge.eventsCompleted == 8, "hot application bridge failed");
    }
    require(allocations == before && hot.calls == 520000, "hot dispatch allocated or callback work was skipped");
}
int main() {
    try {
        testManagerDifferentials(); testRouterDifferentials(); testManagerBoundaries(); testRouterBoundaries(); testAppendBridge(); testApplicationBridge(); testAllocationFree();
        unsigned mv = 0, rv = 0, pv = 0;
        for (auto n : managerVisits) mv += n != 0;
        for (auto n : releaseVisits) rv += n != 0;
        for (auto n : pressVisits) pv += n != 0;
        for (unsigned i = 0; i < managerVisits.size(); ++i) {
            const auto pc = rr_manager_48z_start + 4 * i;
            const bool unsupportedDiagnostic = pc == 0x1281b8 || pc == 0x1281bc || (pc >= 0x1283b0 && pc <= 0x128414);
            require((managerVisits[i] == 0) == unsupportedDiagnostic, "admitted manager coverage gap or excluded diagnostic execution");
        }
        require(rv == releaseVisits.size() && pv == pressVisits.size(), "reachable router original-word coverage gap");
        std::cout << "event_dispatch48z: " << checks << " checks; " << managers << " manager fixtures; " << routers << " router fixtures; " << callbackComparisons << " exact callback traces; " << instructions << " original instructions; coverage " << mv << '/' << managerVisits.size() << " manager, " << rv << '/' << releaseVisits.size() << " release router, " << pv << '/' << pressVisits.size() << " press router; zero hot allocations; wrapper bridge is static-provenance/native-contract only\n";
        for (unsigned i = 0; i < managerVisits.size(); ++i) if (!managerVisits[i]) std::cout << "unvisited manager " << std::hex << rr_manager_48z_start + 4 * i << std::dec << '\n';
        return 0;
    } catch (const std::exception& e) { std::cerr << "event_dispatch48z after " << checks << " checks (" << managers << " managers, " << routers << " routers): " << e.what() << '\n'; return 1; }
}
