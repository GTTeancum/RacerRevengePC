#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>

namespace rr::game {
class OriginalApplicationEntities50LA;
//Process-static125DD8 guards, not per-shape or per-Application defaults.
//The actual Type0 caller must first prove the prior-vertex scratch operand
//known, then call311038 and3022D0. This leaf begins AFTER that classifier.
class OriginalCollisionNormalGuards50LA final {
    friend class OriginalWreckType0Owners50LA;
    struct Lane {std::uint8_t guard=0;std::uint32_t enabled=0;};
    struct State {std::array<Lane,3> lanes{};std::thread::id thread;};
    static State& state()noexcept {static State s;return s;}
    static std::atomic<bool>& unknown()noexcept {static std::atomic<bool> u{false};return u;}
    static std::atomic_flag& lock()noexcept {static std::atomic_flag f=ATOMIC_FLAG_INIT;return f;}
    //100018..100030 clears3D5A00..44F600. GP3DD970 plus8194/8198/819C
    //selects3D5B04/08/0C; enabled words3E0880/888/890 are in that same range.
    //No native cleanup or second App resets these process histories.
    static void invalidateNativeFloatNormals125DD8()noexcept {
        unknown().store(true,std::memory_order_release);
    }
    enum class Status : std::uint8_t {returned,bounded,owner};
    struct Result {Status status;std::uint32_t sourcePC;};
    struct Receipt {
        const OriginalApplicationEntities50LA* application=nullptr; //History identity only.
        unsigned lane=0;
        std::uint32_t classifier=0,enabledValue=0,pc=0;
        std::uint8_t guardBefore=0;
        bool entered=false,processAdmitted=false,guardRead=false;
        bool enabledStored=false,guardStored=false,enabledRead=false;
        bool assertionReached=false,assertionReturned=false,returned=false;
        bool s0Zero125F18=false,s0Zero125F64=false;
    };
    static Result reachAfterClassifier3022D0(unsigned lane,
        const OriginalApplicationEntities50LA& actualApplication,
        std::uint32_t actualClassifier,Receipt& r)noexcept {
        static constexpr std::uint32_t reads[3]={0x125de8,0x125e6c,0x125ef0};
        static constexpr std::uint32_t calls[3]={0x125e24,0x125ea8,0x125f2c};
        static constexpr std::uint32_t joins[3]={0x125e5c,0x125ee0,0x125f68};
        if(lane>=3||actualClassifier>1||r.entered)return {Status::owner,0x125dd8};
        r.entered=true;r.application=&actualApplication;r.lane=lane;
        r.classifier=actualClassifier;r.pc=reads[lane];
        if(lock().test_and_set(std::memory_order_acquire))return {Status::bounded,r.pc};
        auto& s=state();const auto thread=std::this_thread::get_id();
        if(unknown().load(std::memory_order_acquire)||
           (s.thread!=std::thread::id{}&&s.thread!=thread)){
            lock().clear(std::memory_order_release);return {Status::bounded,r.pc};
        }
        s.thread=thread;r.processAdmitted=true;
        auto& g=s.lanes[lane];r.guardBefore=g.guard;r.guardRead=true;
        if(!g.guard){
            g.enabled=1;r.enabledStored=true; //125DFC/125E80/125F04.
            g.guard=1;r.guardStored=true; //125E00/125E84/125F08, AFTER word.
        }
        r.enabledValue=g.enabled;r.enabledRead=true;
        //125F14 branch delay runs on BOTH taken and non-taken paths,
        //including an enabled-NaN refusal before133660 can be modeled.
        if(lane==2)r.s0Zero125F18=true;
        if(g.enabled){
            r.assertionReached=true;r.pc=calls[lane];
            if(actualClassifier){
                //133660(a0=0) needs actual36C100/diagnostic/debugger policy.
                //Retain prior lazy stores and S0; do not invent its return.
                lock().clear(std::memory_order_release);return {Status::bounded,r.pc};
            }
            //a0=1 returns0 regardless of36C100 (133680/133684).
            //Infinity is accepted:3022D0 tests NaN, NOT generic isfinite.
            r.assertionReturned=true;
            if(lane==2)r.s0Zero125F64=true;
        }
        r.returned=true;r.pc=joins[lane];
        lock().clear(std::memory_order_release);return {Status::returned,r.pc};
    }
    //Pure historical proof. Later native certainty loss must not invalidate
    //cleanup of already reached stores; never dereference the App identity.
    static bool historyCurrent(const Receipt& r,
        const OriginalApplicationEntities50LA* expectedApplication)noexcept {
        static constexpr std::uint32_t reads[3]={0x125de8,0x125e6c,0x125ef0};
        static constexpr std::uint32_t calls[3]={0x125e24,0x125ea8,0x125f2c};
        static constexpr std::uint32_t joins[3]={0x125e5c,0x125ee0,0x125f68};
        if(!r.entered)return !r.application&&!r.processAdmitted&&!r.guardRead&&!r.enabledStored&&
            !r.guardStored&&!r.enabledRead&&!r.assertionReached&&!r.assertionReturned&&!r.returned&&
            !r.s0Zero125F18&&!r.s0Zero125F64&&!r.pc&&!r.classifier&&!r.enabledValue&&!r.guardBefore&&!r.lane;
        if(!expectedApplication||r.application!=expectedApplication||r.lane>=3||r.classifier>1)return false;
        if(!r.processAdmitted)return r.pc==reads[r.lane]&&!r.guardRead&&!r.enabledStored&&
            !r.guardStored&&!r.enabledRead&&!r.assertionReached&&!r.assertionReturned&&!r.returned&&
            !r.s0Zero125F18&&!r.s0Zero125F64&&!r.enabledValue&&!r.guardBefore;
        if(!r.guardRead||!r.enabledRead||r.enabledStored!=!r.guardBefore||
           r.guardStored!=r.enabledStored||r.guardBefore>1||r.enabledValue!=1||
           !r.assertionReached||r.s0Zero125F18!=(r.lane==2))return false;
        if(r.classifier)return !r.assertionReturned&&!r.returned&&!r.s0Zero125F64&&r.pc==calls[r.lane];
        return r.assertionReturned&&r.returned&&r.s0Zero125F64==(r.lane==2)&&r.pc==joins[r.lane];
    }
};
}
