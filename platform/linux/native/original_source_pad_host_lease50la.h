#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <thread>
namespace original_main_entry50KG {struct Application;struct Video;}
namespace rr::game {class OriginalApplicationInput04Source50LA;}
namespace rr::platform {
class GameHost50ER;class WindowHost50DQ;
// EXPLICIT Windows/Linux native no-multitap transport contract. Not PS2 RPC,
// controller connection success, sampled gameplay input, or source route readiness.
class OriginalSourcePadHostLease50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class GameHost50ER;
    friend class WindowHost50DQ;
    enum class Binding {unobserved,absent,opened,ambiguous,revoked,unsupportedMultitap};
    struct alignas(64) Port {
        unsigned port=0,slot=0;
        //313000 prepares two0x80 packet buffers. Native backing has the exact
        //written prefix/fields; unknown bytes remain unknown, never sampled.
        std::array<std::uint8_t,256> bytes{},known{};
        Binding binding=Binding::unobserved;
        std::int32_t hostInstance=-1;
        std::uint64_t observation=0;
        bool prepared=false,nativeCallReturned=false;
        std::uint32_t sourceCall=0; // Actual 237430 occurrence, NOT a PS2 RPC receipt.
        void word(std::size_t at,std::uint32_t v)noexcept{
            for(unsigned n=0;n<4;++n){bytes[at+n]=std::uint8_t(v>>(8*n));known[at+n]=255;}}
        void byte(std::size_t at,std::uint8_t v)noexcept{bytes[at]=v;known[at]=255;}
    };
    std::array<Port,8> ports_{};
    const original_main_entry50KG::Application* app_=nullptr;
    const rr::game::OriginalApplicationInput04Source50LA* source_=nullptr;
    std::shared_ptr<original_main_entry50KG::Video> video_; // Exact existing App Video lifetime pin.
    const GameHost50ER* host_=nullptr;
    const WindowHost50DQ* window_=nullptr;
    const void* state_=nullptr; // Exact live owner epoch; cleared BEFORE teardown.
    const std::thread::id thread_=std::this_thread::get_id();
    bool live_=false;
    std::uint64_t observation_=0;
    unsigned nativeCallsReturned_=0;
    OriginalSourcePadHostLease50LA()noexcept=default;
    void prepare()noexcept {
        for(unsigned i=0;i<8;++i){auto& p=ports_[i];p.port=i/4;p.slot=i%4;
            for(unsigned j=0;j<2;++j){const auto at=j*128;
                p.word(at+0x58,0);p.byte(at+0x70,5);p.byte(at+0x71,2);p.byte(at+0x67,0);
                for(unsigned k=0;k<32;++k)p.byte(at+k,255);
                p.word(at+0x60,0);}
            p.binding=p.slot?Binding::unsupportedMultitap:Binding::unobserved;p.prepared=true;
        }
        // Native port-table lifetime association only: no SDK44C210 RPC-return
        //offset08, registration flag10 or PS2 return code is invented.
    }
    bool returnNativeOpen(unsigned ordinal,std::uint32_t call)noexcept {
        static constexpr std::uint32_t calls[8]={0x2374bc,0x2374d0,0x2374e4,0x2374f8,
            0x23750c,0x237520,0x237534,0x237548};
        if(!live_||thread_!=std::this_thread::get_id()||ordinal>=8||
           nativeCallsReturned_!=ordinal||call!=calls[ordinal])return false;
        auto& p=ports_[ordinal];
        if(!p.prepared||p.nativeCallReturned||p.port!=ordinal/4||p.slot!=ordinal%4)return false;
        // Explicit native adaptation: establish the prepared physical-call
        // record under this live transport epoch. No device open, poll, RPC,
        // connection state, or numeric source return value is synthesized.
        // Unsupported multitap and absent/unobserved pads still have logical
        // calls, just as 237430 never branches on padPortOpen's return value.
        p.sourceCall=call;p.nativeCallReturned=true;++nativeCallsReturned_;return true;
    }
    void observe(unsigned hostSlot,bool connected,std::int32_t instance,bool complete)noexcept {
        if(!live_||hostSlot>=2)return;
        auto& p=ports_[hostSlot*4]; // SDL0/1 -> physical0/1 slot0, NOT source ordinal/4.
        p.binding=connected?Binding::opened:complete?Binding::absent:Binding::ambiguous;
        p.hostInstance=connected?instance:-1;p.observation=observation_;
    }
    void revoke()noexcept {
        live_=false;state_=nullptr;window_=nullptr;host_=nullptr;
        for(auto& p:ports_){p.binding=Binding::revoked;p.hostInstance=-1;}
        //Clear identities before releasing the actual Video pin; nested host
        //teardown may revoke this same lease again and remains harmless.
        video_.reset();
        // Preserve packet/source call history, but no borrowed backing remains usable.
    }
public:
    ~OriginalSourcePadHostLease50LA()=default;
    OriginalSourcePadHostLease50LA(const OriginalSourcePadHostLease50LA&)=delete;
    OriginalSourcePadHostLease50LA& operator=(const OriginalSourcePadHostLease50LA&)=delete;
};
}
