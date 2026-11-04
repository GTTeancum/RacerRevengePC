#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <new>
#include <thread>
namespace original_main_entry50KG {struct Application;}
namespace rr::game {
class OriginalPlayerConstructorPrefix50LA;
// Actual13FD40/237430 ownership. No host controller, SDL index, sample or
// connection result is used to manufacture any of the eight original objects.
class OriginalApplicationInput04Source50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalPlayerConstructorPrefix50LA;
    template<std::size_t N> struct Bytes {
        std::array<std::uint8_t,N> data{},known{};
        void word(std::size_t n,std::uint32_t v)noexcept{for(unsigned i=0;i<4;++i){data[n+i]=std::uint8_t(v>>(8*i));known[n+i]=255;}}
        void half(std::size_t n,std::uint16_t v)noexcept{for(unsigned i=0;i<2;++i){data[n+i]=std::uint8_t(v>>(8*i));known[n+i]=255;}}
        void byte(std::size_t n,std::uint8_t v)noexcept{data[n]=v;known[n]=255;}
        void masked(std::size_t n,std::uint8_t mask,std::uint8_t v)noexcept{
            data[n]=std::uint8_t((data[n]&~mask)|(v&mask));known[n]|=mask;
        }
    };
    struct Axis;
    struct Controller {
        Bytes<0x720> source;
        // Typed pointer publications are separate from numeric source words;
        // host pointer widths never enter guest raw lanes. Null is explicit.
        std::array<std::unique_ptr<Axis*[]>,6> mappingTables;
        std::array<unsigned,6> mappingCounts{}; // Constructor143F00 count00=0.
        std::array<bool,6> mappingPointerWritten{};
        std::unique_ptr<Bytes<12>[]> buttons14;
        bool buttonsPointerWritten=false,constructorReturned=false,setupReturned=false;
    };
    struct Axis {
        Bytes<0x3c> source;
        Controller* controller00=nullptr;
        unsigned selector=0;
        bool registered=false;
    };
    struct Manager {
        Bytes<0xcc8> source;
        std::array<std::unique_ptr<Controller>,8> controllers;
        std::array<Controller*,8> slots10{};
        std::array<bool,8> slotsKnown{};
        Controller* selected0C=nullptr;
        std::unique_ptr<Bytes<0x280>> mappingCBC;
        bool mappingPointerWritten=false,constructorReturned=false;
        // Source0C/CBC start constructor-known NULL; later typed publications
        // require the explicit live native transport adaptation, not fake RPC.
    };
    std::unique_ptr<Manager> allocation_;
    const original_main_entry50KG::Application* application_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    bool attempted_=false,published04_=false,nativeClosed_=false;
    bool startupReturnedWithNativeTransport_=false;
    std::uint32_t pc_=0;
    std::size_t completedControllers_=0,playerBorrows_=0;
    //A cancelled later Player cannot make this input's startup interval cold
    //again. This records only actual private borrower issuance, not readiness.
    bool playerBorrowEverIssued_=false;
    bool startupMappingsAttempted_=false,startupMappingsReturned_=false;
    std::uint32_t startupMappingsPC_=0;
    unsigned startupMappingCallsIssued_=0,startupMappingCallsReturned_=0,startupMappingWordsWritten_=0;
    //Exact17CB84 variadic values, not SDL/host button constants. The caller's
    //following -1 is item17 and is not read:127B84 caps each call at16 stores.
    static constexpr std::array<std::uint32_t,16> startupMappingArguments17CB84_{
        15,13,12,14,2,3,0,1,9,8,6,7,4,5,11,10};
    // ELF PT_LOAD zero-fill covers3E08E0. Static-reference research found
    //127FD0 writes and four direct callers; not exhaustive runtime evidence.
    //One process authority, never restarted for each App/Player or cancellation.
    static inline std::atomic<std::uint32_t> axisCounter3E08E0_{0};
    static inline std::atomic<std::uint8_t> guard8390_{0},guard8394_{0};
    static inline std::atomic<std::uint32_t> assertion426EC0_{0},assertion426EC8_{0};
    // Actual process guard bytes GP8388/838C and enabled426EB0/426EB8;
    // successful known assertions only. Never reset for another Application.
    static inline std::atomic<std::uint8_t> guard8388_{0},guard838C_{0};
    static inline std::atomic<std::uint32_t> assertion426EB0_{0},assertion426EB8_{0};
    static void mappingAssertions143F30()noexcept {
        if(!guard8388_.load(std::memory_order_relaxed)){
            assertion426EB0_.store(1,std::memory_order_relaxed);guard8388_.store(1,std::memory_order_relaxed);}
        //133660 sees count1>0; known pass regardless of enable word.
        if(!guard838C_.load(std::memory_order_relaxed)){
            assertion426EB8_.store(1,std::memory_order_relaxed);guard838C_.store(1,std::memory_order_relaxed);}
        //133660 sees constructor field0C==0. No failure diagnostic/debugger
        //return is fabricated, and process guard history is never per-App reset.
    }
    bool constructController(Controller& c,std::uint32_t ordinal)noexcept {
        auto& x=c.source;
        //237C10→143390→237B10→143260(a1=2).
        x.word(0x18,0x3cd340);for(unsigned bit=0;bit<6;++bit)x.masked(0x10,std::uint8_t(1u<<bit),0);
        x.word(0x14,0);x.word(0,0);x.word(4,0);x.word(8,2);x.word(12,0xffffffffu);
        x.word(0x18,0x3d1d70);x.word(0x18,0x3cd350);
        //2E8860: six in-place143F00 constructors, stride0x14, no heap.
        for(std::size_t i=0;i<6;++i){const auto a=0x63c+i*0x14;
            x.word(a+4,0);x.word(a,0);x.word(a+8,0);x.word(a+12,0);x.masked(a+16,1,0);}
        x.word(0x320,ordinal);x.word(0x31c,0);
        for(std::size_t i=0;i<6;++i)for(std::size_t j=0;j<16;++j){
            x.word(0x1c+i*0x80+j*8,0);x.word(0x20+i*0x80+j*8,0);}
        for(std::size_t i=0;i<6;++i){const auto a=0x63c+i*0x14;
            x.word(0x624+i*4,0);pc_=0x144028;mappingAssertions143F30();
            // Source malloc(4) result published evenNULL. Contents remain
            // unknown; caller143390 ignores allocation-result boolean.
            c.mappingTables[i].reset(new(std::nothrow) Axis*[1]);
            c.mappingPointerWritten[i]=true;x.known[a+4]=x.known[a+5]=x.known[a+6]=x.known[a+7]=0;
            x.word(a+8,1);x.word(a+12,1);x.masked(a+16,1,1);
        }
        for(std::size_t i=0;i<32;++i){x.word(0x324+i*0x18,0);x.word(0x328+i*0x18,0);}
        // Actual derivedApplication17C650 dispatch3CE480 slot68=237A70
        // returns supplied default a2=0 for JoyDirectionFlipped, not a host setting.
        x.masked(0x6b4,1,0);
        x.word(0x18,0x3d1d80);x.word(0x6c4,0);x.word(0x6c8,0);x.masked(0x6b4,6,4);
        x.word(0x6e0,0);x.word(0x6f0,4);x.word(0x6f8,0);
        for(std::size_t i=0;i<6;++i)x.word(0x6fc+i*4,0);
        c.constructorReturned=true; //237C94; upper6B4bits and padding unknown.
        //237D00 receives actual loop ordinal, not physical SDL device index.
        x.word(0x6b8,ordinal);x.word(0x6bc,0);x.word(0x6c0,0);x.word(4,16);
        pc_=0x237d30;c.buttons14.reset(new(std::nothrow) Bytes<12>[16]);c.buttonsPointerWritten=true;
        // Native pointer publication replaces base-knownNULL14. Never read
        // guest raw pointer bytes as a value; typed owner carries exactNULL.
        for(unsigned i=0;i<4;++i)x.known[0x14+i]=0;
        for(std::size_t i=0;i<16;++i){pc_=0x237d54;if(!c.buttons14)return false;
            c.buttons14[i].byte(0,0);c.buttons14[i].word(4,0);c.buttons14[i].half(8,0);}
        for(std::size_t i=0;i<6;++i)x.byte(0x714+i,0);
        x.word(0x6cc,0);x.word(0x6d0,0);x.word(0x6e4,0);x.word(0x6e8,0);x.word(0x6ec,0);
        x.word(0x6d8,0);x.word(0x6dc,0);x.word(0x6d4,0);c.setupReturned=true;return true;
    }
    OriginalApplicationInput04Source50LA()noexcept=default;
    // Source Application owner only. This is a private uncalled startup
    // continuation, not automatic construction in a native Session.
    bool begin13FD40(const original_main_entry50KG::Application& app)noexcept {
        if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id())return false;
        attempted_=true;application_=&app;pc_=0x13fd50;
        allocation_.reset(new(std::nothrow) Manager);
        if(allocation_){auto& m=*allocation_;auto& x=m.source;
            x.word(0xcc0,0x3cce10);x.word(0,0);x.word(4,0);x.word(12,0);
            for(std::size_t i=0;i<8;++i){m.slots10[i]=nullptr;m.slotsKnown[i]=true;}
            x.word(0x30,0);x.word(0x34,0);x.word(0xcbc,0);x.word(0x38,0);x.word(8,0);
            x.word(0xcc0,0x3d1ca0);x.word(0xcc4,0);m.constructorReturned=true;
        }
        published04_=true;pc_=0x13fd68; // includes genuineNULL after allocation failure.
        if(!allocation_){pc_=0x237454;return false;} // before source NULL write.
        auto& m=*allocation_;m.source.word(0xcc4,8);
        for(std::uint32_t i=0;i<8;++i){pc_=0x237468;
            m.controllers[i].reset(new(std::nothrow) Controller);
            // Original branch skips ctor onNULL, but still calls237D00(NULL)
            // BEFORE publishing slot10; retain constructorNULL and stop safely.
            if(!m.controllers[i]){pc_=0x237d0c;return false;}
            if(!constructController(*m.controllers[i],i))return false;
            m.slots10[i]=m.controllers[i].get();m.slotsKnown[i]=true;pc_=0x2374a0;++completedControllers_;
        }
        //Eight actual published identities exist. FIRST platform padPortOpen
        //313000 is pending; do not fake its return or publish later0C/CBC.
        pc_=0x2374bc;return false;
    }
    bool finishNativePadActivation(const original_main_entry50KG::Application& app)noexcept {
        // ONLY the private App issuer calls this after eight exact live-lease
        // native call returns. It is not a public success receipt/factory.
        if(application_!=&app||thread_!=std::this_thread::get_id()||nativeClosed_||
           !allocation_||completedControllers_!=8||pc_!=0x237550||startupReturnedWithNativeTransport_)return false;
        auto& m=*allocation_;auto& x=m.source;
        m.selected0C=m.slots10[0]; //237550/237558, original logical identity.
        for(unsigned i=0;i<4;++i)x.known[0x0c+i]=0; // Typed pointer, not host bits.
        x.word(0x34,0); //237560 delay slot before 127F80.
        pc_=0x127f94;m.mappingCBC.reset(new(std::nothrow) Bytes<0x280>);
        if(m.mappingCBC){
            //1279B0: 16 columns, first row + eight middle rows + last row.
            for(unsigned column=0;column<16;++column){
                m.mappingCBC->word(column*4,0xffffffffu);
                for(unsigned row=1;row<=8;++row)m.mappingCBC->word(row*0x40+column*4,0xffffffffu);
                m.mappingCBC->word(0x240+column*4,0xffffffffu);
            }
        }
        //127FB4 publishes even allocation NULL. 127FB8 stores 1 regardless;
        //237430 then returns that 1 and 13FD78 takes its success branch.
        m.mappingPointerWritten=true;for(unsigned i=0;i<4;++i)x.known[0xcbc+i]=0;
        x.word(8,1);pc_=0x13fd98;startupReturnedWithNativeTransport_=true;return true;
    }
    bool startupMappingBytesCurrent(bool mapped)const noexcept {
        if(!allocation_||!allocation_->mappingPointerWritten||!allocation_->mappingCBC)return false;
        const auto& bytes=*allocation_->mappingCBC;
        for(unsigned offset=0;offset<0x280;offset+=4){
            const auto expected=mapped&&offset>=0x40&&offset<0xc0?
                startupMappingArguments17CB84_[((offset-0x40)%0x40)/4]:0xffffffffu;
            for(unsigned lane=0;lane<4;++lane)
                if(bytes.known[offset+lane]!=255||bytes.data[offset+lane]!=std::uint8_t(expected>>(lane*8)))return false;
        }
        return true;
    }
    bool beforeStartupMappings17CB04(const original_main_entry50KG::Application& app)const noexcept {
        if(thread_!=std::this_thread::get_id()||application_!=&app||nativeClosed_||
           !startupReturnedWithNativeTransport_||!published04_||pc_!=0x13fd98||!allocation_||
           !allocation_->mappingPointerWritten||playerBorrows_||playerBorrowEverIssued_||
           startupMappingsAttempted_||startupMappingsReturned_||startupMappingCallsIssued_||
           startupMappingCallsReturned_||startupMappingWordsWritten_)return false;
        //Native history/knownness guard only. A genuine NULL publication is
        //allowed to enter the call, then stops safely at its first source store.
        return !allocation_->mappingCBC||startupMappingBytesCurrent(false);
    }
    bool applyStartupMappings17CB84(const original_main_entry50KG::Application& app)noexcept {
        if(!beforeStartupMappings17CB04(app))return false;
        startupMappingsAttempted_=true;startupMappingsPC_=0x17cb04;
        for(unsigned row=0;row<2;++row){
            //17CB64/74 use THIS published04 manager and its actual+CBC owner.
            startupMappingsPC_=0x17cb84;++startupMappingCallsIssued_;
            for(unsigned column=0;column<16;++column){
                startupMappingsPC_=0x127b6c;
                if(!allocation_->mappingCBC)return false; // Before source NULL+40 write.
                //127B58..6C type2: receiver+40+(saved a2<<6)+column*4.
                allocation_->mappingCBC->word(0x40+row*0x40+column*4,startupMappingArguments17CB84_[column]);
                ++startupMappingWordsWritten_;
            }
            //127B84/8C reaches its16-entry limit, not the unread -1 sentinel.
            //Native bool is custody outcome, not a fabricated127AD0 v0 value.
            ++startupMappingCallsReturned_;startupMappingsPC_=0x17cb8c;
        }
        //Second increment/test and17CB98 delay slot finish the two-row loop.
        //Leave17CB9C epilogue and17CBA8's startup return1 unexecuted.
        startupMappingsPC_=0x17cb9c;startupMappingsReturned_=true;return true;
    }
    bool startupMappings17CB9CCurrent(const original_main_entry50KG::Application& app)const noexcept {
        return thread_==std::this_thread::get_id()&&application_==&app&&!nativeClosed_&&
            startupReturnedWithNativeTransport_&&published04_&&pc_==0x13fd98&&!playerBorrows_&&!playerBorrowEverIssued_&&
            startupMappingsAttempted_&&startupMappingsReturned_&&startupMappingsPC_==0x17cb9c&&
            startupMappingCallsIssued_==2&&startupMappingCallsReturned_==2&&startupMappingWordsWritten_==32&&
            startupMappingBytesCurrent(true);
    }
    bool closeNative(const original_main_entry50KG::Application& app)noexcept {
        if(nativeClosed_)return true;if(application_!=&app||thread_!=std::this_thread::get_id())return false;
        // No public borrower is issued by this owner, even after adapted startup.
        // Native custody cleanup only; no237340/127F00 source destructor claim.
        // Revoke at actual App close/destruction BEFORE any borrower can
        // outlive the App. Shared private borrowers retain backing only for
        // checked native cancellation, never to continue source execution.
        nativeClosed_=true;if(!playerBorrows_)allocation_.reset();return true;
    }
public:
    ~OriginalApplicationInput04Source50LA()=default;
    OriginalApplicationInput04Source50LA(const OriginalApplicationInput04Source50LA&)=delete;
    OriginalApplicationInput04Source50LA& operator=(const OriginalApplicationInput04Source50LA&)=delete;
};
}
