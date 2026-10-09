#include "core_suite.h"
#include "cycle_ledger.h"
#include "registers.h"
#include "idle_window.h"
#include "compact_registry.h"
#include "registry_replacements.h"
#include "iop_ram_versions.h"
#include "byte_ownership.h"

namespace {
void check(CoreTestResult& r, bool success, unsigned line) {
    ++r.checks;
    if (!success) { ++r.failures; if (!r.first_failure_line) r.first_failure_line = line; }
}
uint32_t random_word(uint32_t& state) {
    state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state;
}
void digest(CoreTestResult& r, uint32_t value) { r.digest = (r.digest ^ value) * 16777619u; }
using Replacements = rr::RegistryReplacements<uint32_t>;
struct ReplacementPool { Replacements::Page pages[2]; bool used[2]={false,false}; unsigned releases=0; };
Replacements::Page* allocateReplacement(void* context) {
    auto& pool=*static_cast<ReplacementPool*>(context);
    for(unsigned i=0;i<2;++i) if(!pool.used[i]) { pool.used[i]=true; return &pool.pages[i]; }
    return nullptr;
}
void releaseReplacement(void* context,Replacements::Page* page) {
    auto& pool=*static_cast<ReplacementPool*>(context);
    for(unsigned i=0;i<2;++i) if(page==&pool.pages[i]) { pool.used[i]=false; ++pool.releases; }
}
}
#define CHECK(x) check(result, (x), __LINE__)

CoreTestResult run_core_suite() noexcept {
    CoreTestResult result{0, 0, 0, 2166136261u};
    {
        uint32_t guarded[7] = {0xabcddcba, 0, 0, 0, 0, 0, 0x12344321};
        rr::ByteOwnership owned;
        CHECK(!owned.owns(0, 0));
        CHECK(!owned.configure(nullptr, 5, 131));
        CHECK(!owned.configure(guarded + 1, 4, 131));
        CHECK(owned.configure(guarded + 1, 5, 131));
        CHECK(owned.storageBytes() == 20);
        CHECK(owned.owns(131, 0) && !owned.owns(132, 0));
        CHECK(!owned.mark(131, 1) && !owned.mark(UINT32_MAX, 1));
        CHECK(!owned.mark(1, UINT32_MAX));
        uint8_t oracle[131]{};
        uint32_t seed = 0x817526ad;
        for (unsigned round = 0; round < 128; ++round) {
            owned.clear();
            for (auto& byte : oracle) byte = 0;
            for (unsigned step = 0; step < 16; ++step) {
                const unsigned begin = random_word(seed) % 132;
                const unsigned size = random_word(seed) % (132 - begin);
                const bool releasing = (step % 3) == 0;
                CHECK(releasing ? owned.release(begin, size) : owned.mark(begin, size));
                for (unsigned n = begin; n < begin + size; ++n) oracle[n] = releasing ? 0 : 1;
                for (unsigned q = 0; q < 16; ++q) {
                    const unsigned at = random_word(seed) % 132;
                    const unsigned count = random_word(seed) % (132 - at);
                    bool expected = true;
                    for (unsigned n = at; n < at + count; ++n) expected = expected && oracle[n] != 0;
                    CHECK(owned.owns(at, count) == expected);
                }
                for (unsigned n = 0; n < 131; ++n) CHECK(owned.owns(n, 1) == (oracle[n] != 0));
            }
            CHECK(guarded[0] == 0xabcddcba && guarded[6] == 0x12344321);
            digest(result, guarded[3]);
        }
        owned.clear();
        CHECK(owned.mark(0, 131) && owned.owns(0, 131));
        CHECK(owned.release(31, 66));
        CHECK(owned.owns(0, 31) && owned.owns(97, 34));
        CHECK(!owned.owns(31, 1) && !owned.owns(96, 1));
        CHECK(owned.release(131, 0) && !owned.release(131, 1));
        CHECK(!owned.release(1, UINT32_MAX));
        CHECK(owned.release(0, 131) && !owned.owns(0, 1) && !owned.owns(130, 1));
        CHECK(!owned.owns(0, 132));
        CHECK(!owned.configure(guarded + 1, 5, 0));
        CHECK(!owned.owns(0, 0));
    }
    {
        rr::IopRamVersions<> watch;
        rr::IopImportRangeVersion stamp;
        CHECK(!stamp.matches(watch));
        CHECK(stamp.capture(watch, 0xffc, 0x11004));
        CHECK(stamp.count == 18 && stamp.matches(watch));
        CHECK(watch.changed(0x12000, 32) && stamp.matches(watch));
        CHECK(watch.changed(0x1000, 0) && stamp.matches(watch));
        CHECK(watch.changed(0x11003, 1) && !stamp.matches(watch));
        CHECK(stamp.capture(watch, 0xffc, 0x11004));
        CHECK(watch.changed(0xffc, 4) && !stamp.matches(watch));
        CHECK(stamp.capture(watch, 0x1000, 0x2000));
        CHECK(watch.changed(0x2000, 1) && stamp.matches(watch));
        CHECK(watch.changed(0xfff, 2) && !stamp.matches(watch));
        CHECK(!stamp.capture(watch, 0, 19 * 4096));
        CHECK(!stamp.capture(watch, 5, 5));
        CHECK(!stamp.capture(watch, 6, 5));
        CHECK(!stamp.capture(watch, 0, UINT32_MAX));
        CHECK(stamp.capture(watch, watch.Bytes - 4, watch.Bytes));
        CHECK(watch.changed(0, watch.Bytes) && !stamp.matches(watch));
        CHECK(watch.changed(watch.Bytes, 0));
        rr::IopRamVersions<3> shortWatch;
        CHECK(shortWatch.changed(0, 4));
        CHECK(stamp.capture(shortWatch, 0, 4));
        CHECK(shortWatch.changed(0x1000, 4) && stamp.matches(shortWatch));
        CHECK(shortWatch.changed(0x1000, 4) && stamp.matches(shortWatch));
        CHECK(!shortWatch.changed(0x1000, 4) && !stamp.matches(shortWatch));
        CHECK(!shortWatch.changed(0, shortWatch.Bytes));
        CHECK(!shortWatch.trustworthy());
        rr::IopRamVersions<> invalid;
        CHECK(!invalid.changed(invalid.Bytes, 1) && !invalid.trustworthy());
        uint32_t seed = 0x487aec13;
        for (unsigned n = 0; n < 4096; ++n) {
            const uint32_t page = random_word(seed) % watch.Pages;
            const uint32_t other = random_word(seed) % watch.Pages;
            CHECK(stamp.capture(watch, page << 12, (page + 1) << 12));
            CHECK(watch.changed(other << 12, 4));
            CHECK(stamp.matches(watch) == (other != page));
            digest(result, watch.pageVersion(other));
        }
    }
    rr::CompactRegistry registry;
    uint8_t pattern[64] = {};
    pattern[0]=1; pattern[63]=2;
    rr::RegistryPage pages[2]={{0,0,3},{3,0,3}};
    const uint16_t owners[]={0,12,34,0,56,78};
    CHECK(registry.lookup(0)==0);
    CHECK(registry.configure(0x1000,128,pages,2,pattern,1,owners,6,78));
    CHECK(registry.lookup(0x1000)==12 && registry.lookup(0x10fc)==34);
    CHECK(registry.lookup(0x1100)==56 && registry.lookup(0x11fc)==78);
    CHECK(registry.lookup(0x1004)==0 && registry.lookup(0x1104)==0);
    CHECK(registry.lookup(0x1001)==0 && registry.lookup(0x1002)==0 && registry.lookup(0x1003)==0);
    CHECK(registry.lookup(0xffc)==0 && registry.lookup(0x1200)==0 && registry.lookup(0xfffffffc)==0);
    CHECK(!registry.configure(0x1001,128,pages,2,pattern,1,owners,6,78));
    CHECK(registry.lookup(0x1000)==0); // Rejected configuration clears prior view.
    CHECK(!registry.configure(0x1000,128,pages,1,pattern,1,owners,6,78));
    CHECK(!registry.configure(0x1000,128,pages,2,pattern,1,owners,5,78));
    CHECK(!registry.configure(0x1000,128,pages,2,pattern,1,owners,6,77));
    pattern[5]=3;
    CHECK(!registry.configure(0x1000,128,pages,2,pattern,1,owners,6,78));
    pattern[5]=0;
    pages[0].pattern=1;
    CHECK(!registry.configure(0x1000,128,pages,2,pattern,1,owners,6,78));
    pages[0].pattern=0;
    CHECK(!registry.configure(0xfffffffc,2,pages,1,pattern,1,owners,6,78));
    CHECK(registry.configure(0xfffffffc,1,pages,1,pattern,1,owners,6,78));
    CHECK(registry.lookup(0xfffffffc)==12 && registry.lookup(0)==0);
    CHECK(!registry.configure(0,0,nullptr,0,nullptr,0,nullptr,0,0));
    ReplacementPool pool;
    Replacements::Page* directory[3]={};
    Replacements replacements;
    uint32_t replacement=123;
    CHECK(!replacements.get(0,replacement) && replacement==123);
    CHECK(replacements.configure(0x1000,192,directory,3,allocateReplacement,releaseReplacement,&pool));
    CHECK(replacements.set(0x1000,100) && replacements.get(0x1000,replacement) && replacement==100);
    CHECK(!replacements.get(0x1004,replacement)); // Shared original owner must not alias.
    CHECK(replacements.set(0x1004,0) && replacements.get(0x1004,replacement) && replacement==0);
    CHECK(replacements.set(0x1000,200) && replacements.get(0x1000,replacement) && replacement==200);
    CHECK(replacements.set(0x107c,31) && replacements.set(0x1080,32) && replacements.set(0x10fc,63));
    CHECK(replacements.get(0x107c,replacement) && replacement==31);
    CHECK(replacements.get(0x1080,replacement) && replacement==32);
    CHECK(replacements.get(0x10fc,replacement) && replacement==63);
    CHECK(replacements.set(0x1100,300) && replacements.allocatedPages()==2);
    CHECK(!replacements.set(0x1200,400) && !replacements.get(0x1200,replacement)); // Allocation failure preserves state.
    CHECK(replacements.get(0x1000,replacement) && replacement==200);
    CHECK(!replacements.set(0x1001,5) && !replacements.set(0xffc,5) && !replacements.set(0x1300,5));
    replacements.reset();
    CHECK(pool.releases==2 && !pool.used[0] && !pool.used[1] && replacements.allocatedPages()==0);
    CHECK(directory[0]==nullptr && directory[1]==nullptr && directory[2]==nullptr);
    CHECK(!replacements.get(0x1000,replacement));
    CHECK(replacements.configure(0xfffffffc,1,directory,1,allocateReplacement,releaseReplacement,&pool));
    CHECK(replacements.set(0xfffffffc,9) && replacements.get(0xfffffffc,replacement) && replacement==9);
    CHECK(!replacements.set(0,5));
    CHECK(!replacements.configure(0xfffffffc,2,directory,1,allocateReplacement,releaseReplacement,&pool));
    CHECK(pool.releases==3 && replacements.allocatedPages()==0);
    rr::IdleWindow idle;
    uint64_t tick = 100;
    CHECK(!idle.advance(120, tick));
    idle.remember(tick, 200);
    CHECK(idle.advance(120, tick) && tick == 120);
    CHECK(!idle.advance(110, tick) && tick == 120);
    CHECK(idle.advance(250, tick) && tick == 200);
    CHECK(!idle.advance(250, tick));
    idle.remember(tick, 190);
    CHECK(!idle.advance(250, tick));
    idle.remember(tick, 300);
    idle.invalidate();
    CHECK(!idle.advance(250, tick));
    rr::CycleLedger clock;
    CHECK(clock.arm(32));
    CHECK(!clock.account(33));
    CHECK(clock.now() == 0);
    CHECK(clock.account(31));
    CHECK(clock.budget(8) == 1);
    CHECK(!clock.arm(30));
    CHECK(clock.account(1));
    CHECK(clock.due());
    CHECK(!clock.account(1));
    auto advance = clock.synchronize();
    CHECK(advance.ee_cycles == 32 && advance.iop_cycles == 4);
    advance = clock.synchronize();
    CHECK(advance.ee_cycles == 0 && advance.iop_cycles == 0);
    clock.disarm();
    CHECK(clock.account(7));
    CHECK(clock.synchronize().iop_cycles == 0);
    CHECK(clock.account(1));
    CHECK(clock.synchronize().iop_cycles == 1);

    // Model a slow per-EE-tick oracle independently of the deferred ledger.
    // Exercise arbitrary barriers, 8:1 fractional carry, and exact event stops.
    uint32_t rng = 0x93ac187bu;
    for (unsigned run = 0; run < 1000; ++run) {
        const uint64_t base = (uint64_t(random_word(rng)) << 16) | (random_word(rng) & 0xffffu);
        clock.reset(base);
        uint64_t oracle_ee = base, oracle_iop = 0, batched_iop = 0;
        unsigned ticks_to_iop = 8 - unsigned(base & 7);
        for (unsigned event = 0; event < 10; ++event) {
            const uint64_t end = oracle_ee + 1 + random_word(rng) % 511;
            CHECK(clock.arm(end));
            while (oracle_ee < end) {
                unsigned n = clock.budget(1 + random_word(rng) % 97);
                CHECK(n != 0);
                CHECK(clock.account(n));
                for (unsigned i = 0; i < n; ++i) {
                    ++oracle_ee;
                    if (--ticks_to_iop == 0) { ++oracle_iop; ticks_to_iop = 8; }
                }
                if ((random_word(rng) & 3) == 0) batched_iop += clock.synchronize().iop_cycles;
            }
            CHECK(clock.due() && clock.now() == oracle_ee);
            batched_iop += clock.synchronize().iop_cycles;
            CHECK(batched_iop == oracle_iop);
            digest(result, static_cast<uint32_t>(batched_iop));
        }
    }
    clock.reset(UINT64_MAX - 2);
    CHECK(clock.budget(9) == 2);
    CHECK(!clock.account(3));
    CHECK(clock.account(2));
    CHECK(!clock.account(1));
    CHECK(clock.synchronize().ee_cycles == 2);

    rr::Registers regs;
    const uint32_t cases[] = {0, 1, 0x7fffffffu, 0x80000000u, 0xffffffffu, 0x12345678u};
    const rr::Register128 sentinel{{0x13579bdf, 0x2468ace0, 0xa5a5a5a5, 0x5a5a5a5a}};
    for (unsigned reg = 0; reg < 32; ++reg) {
        for (uint32_t value : cases) {
            regs.write128(reg, sentinel);
            regs.write32(reg, value);
            auto v = regs.full(reg);
            const uint64_t expected = reg ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(value))) : 0;
            CHECK(regs.low64(reg) == expected);
            CHECK(v.lane[2] == (reg ? sentinel.lane[2] : 0));
            CHECK(v.lane[3] == (reg ? sentinel.lane[3] : 0));
            regs.writeZeroExtended32(reg, value);
            CHECK(regs.low64(reg) == (reg ? uint64_t(value) : 0));
            regs.write64(reg, 0xfedcba9876543210ull);
            CHECK(regs.low64(reg) == (reg ? 0xfedcba9876543210ull : 0));
            CHECK(regs.low32(reg) == (reg ? 0x76543210u : 0));
            CHECK(regs.full(reg).lane[3] == (reg ? sentinel.lane[3] : 0));
            digest(result, regs.low32(reg));
        }
    }
    return result;
}
