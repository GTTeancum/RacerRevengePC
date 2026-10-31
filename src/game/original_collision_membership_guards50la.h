#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>

namespace rr::game {
//120070's two process-static assertion pairs. GP3DD970 + signed8174/8178
//selects3D5AE4/3D5AE8; enabled words are3E0840/3E0848. Both pairs lie in
//the established startup clear3D5A00..44F600. No App/World reset or pool.
//Only the retained serial, known-enabled domain is admitted. This is not a
//claim that all guest aliases or diagnostic configuration writes are absent.
class OriginalCollisionMembershipGuards50LA final {
    struct Pair {std::uint8_t guard=0;std::uint32_t enabled=0;};
    struct State {std::array<Pair,2> pairs{};std::thread::id thread;};
    static State& state()noexcept {static State s;return s;}
    static std::array<std::atomic<bool>,2>& unknown()noexcept {
        static std::array<std::atomic<bool>,2> value{};return value;
    }
    static std::atomic_flag& lock()noexcept {
        static std::atomic_flag value=ATOMIC_FLAG_INIT;return value;
    }
    static constexpr std::uint32_t readPC(unsigned lane)noexcept {
        return lane?0x120190u:0x1200f0u;
    }
    static constexpr std::uint32_t callPC(unsigned lane)noexcept {
        return lane?0x1201ccu:0x120128u;
    }
    static constexpr std::uint32_t joinPC(unsigned lane)noexcept {
        return lane?0x1201fcu:0x120158u;
    }
public:
    struct History {
        unsigned lane=0;
        std::uint32_t pc=0,enabledValue=0,assertionReturnV0=0;
        std::uint8_t guardBefore=0;
        std::thread::id thread;
        bool entered=false,processAdmitted=false,guardRead=false;
        bool enabledStored=false,guardStored=false,enabledRead=false;
        bool finishAttempted=false,conditionRead=false,nonnull=false;
        bool assertionReached=false,assertionReturned=false,returned=false;
    };
private:
    static bool begin(unsigned lane,History& h)noexcept {
        if(h.entered||!history(lane,h))return false;
        h.entered=true;h.lane=lane;h.pc=readPC(lane);
        if(lock().test_and_set(std::memory_order_acquire))return false;
        auto& s=state();const auto thread=std::this_thread::get_id();
        if(unknown()[lane].load(std::memory_order_acquire)||
           (s.thread!=std::thread::id{}&&s.thread!=thread)){
            lock().clear(std::memory_order_release);return false;
        }
        s.thread=thread;h.thread=thread;h.processAdmitted=true;
        auto& p=s.pairs[lane];h.guardRead=true;h.guardBefore=p.guard;
        if(!p.guard){
            //Actual120104 /1201A4 word stores precede120108 /1201A8 guards.
            p.enabled=1;h.enabledStored=true;
            p.guard=1;h.guardStored=true;
        }
        h.enabledValue=p.enabled;h.enabledRead=true;
        //The service has no public enabled-word setter or startup replay.
        //Enabled=1 is this supported provenance, not a guest warm-word check.
        h.pc=callPC(lane);
        lock().clear(std::memory_order_release);return true;
    }
    static bool finish(unsigned lane,History& h,bool nonnull)noexcept {
        if(h.lane!=lane||!h.entered||!h.processAdmitted||h.finishAttempted||
           !history(lane,h)||h.thread!=std::this_thread::get_id())return false;
        h.finishAttempted=true;h.conditionRead=true;h.nonnull=nonnull;
        h.assertionReached=true;h.pc=callPC(lane);
        if(!nonnull){
            //133660(a0=0) depends on real diagnostic/debugger policy. Bound
            //before inventing its return or executing120138/1201DC suffixes.
            return false;
        }
        //Known true133660 branches at133680 with v0=0 in delay133684;
        //JR1336AC/delay1336B0 completes at1336B4, independently of36C100.
        //Neither this leaf nor its result authorizes observer/zone ownership.
        h.assertionReturnV0=0;h.assertionReturned=true;h.returned=true;
        h.pc=joinPC(lane);return true;
    }
    static bool history(unsigned lane,const History& h)noexcept {
        //Immutable historical receipt: later provenance invalidation cannot
        //erase reached stores or make native cancellation require a new read.
        if(!h.entered)return !h.lane&&!h.pc&&!h.enabledValue&&!h.assertionReturnV0&&!h.guardBefore&&
            h.thread==std::thread::id{}&&!h.processAdmitted&&!h.guardRead&&!h.enabledStored&&!h.guardStored&&
            !h.enabledRead&&!h.finishAttempted&&!h.conditionRead&&!h.nonnull&&!h.assertionReached&&
            !h.assertionReturned&&!h.returned;
        if(h.lane!=lane)return false;
        if(!h.processAdmitted)return h.pc==readPC(lane)&&h.thread==std::thread::id{}&&!h.guardRead&&
            !h.enabledStored&&!h.guardStored&&!h.enabledRead&&!h.enabledValue&&!h.guardBefore&&
            !h.finishAttempted&&!h.conditionRead&&!h.nonnull&&!h.assertionReached&&
            !h.assertionReturned&&!h.assertionReturnV0&&!h.returned;
        if(h.thread==std::thread::id{}||!h.guardRead||!h.enabledRead||h.guardBefore>1||
           h.enabledStored!=!h.guardBefore||h.guardStored!=h.enabledStored||h.enabledValue!=1||
           h.assertionReturnV0)return false;
        if(!h.finishAttempted)return h.pc==callPC(lane)&&!h.conditionRead&&!h.nonnull&&
            !h.assertionReached&&!h.assertionReturned&&!h.returned;
        if(!h.conditionRead||!h.assertionReached)return false;
        if(!h.nonnull)return h.pc==callPC(lane)&&!h.assertionReturned&&!h.returned;
        return h.assertionReturned&&h.returned&&h.pc==joinPC(lane);
    }
public:
    static bool beginAllocation(History& h)noexcept{return begin(0,h);}
    static bool finishAllocation(History& h,bool nonnull)noexcept{return finish(0,h,nonnull);}
    static bool allocationHistory(const History& h)noexcept{return history(0,h);}
    static bool beginZone(History& h)noexcept{return begin(1,h);}
    static bool finishZone(History& h,bool nonnull)noexcept{return finish(1,h,nonnull);}
    static bool zoneHistory(const History& h)noexcept{return history(1,h);}
    //Native certainty markers only, NEVER original guard or enabled writes.
    //No exact native writer is currently identified in canonical2146. Any
    //future adapter implementing one of these pairs must invalidate BEFORE
    //its projected access, including warm access and cold-record reset.
    static void allocationNativeRouteUnknown()noexcept {unknown()[0].store(true,std::memory_order_release);}
    static void zoneNativeRouteUnknown()noexcept {unknown()[1].store(true,std::memory_order_release);}
};
}
