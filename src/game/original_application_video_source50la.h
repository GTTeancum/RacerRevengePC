#pragma once
#include "original_video_preflip50kg.h"
#include "original_video_clear_packet25cad0_native50la.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <thread>
#include <utility>

namespace original_main_entry50KG {struct Application;}
namespace rr::game {
class OriginalApplicationStartupCall23A0DCNative50LA;
// Source Video443A60 allocation/constructor/publication, not native Video's
// SDL/GL presentation owner. Application alone can enter this retained prefix.
// No source mode, frame, renderer, sky, or original startup return is issued.
class OriginalApplicationVideoSource50LA final {
public:
    // Diagnostic source call image only. No API accepts this as a backend
    // grant or registration result, and sourceA1 is never a host function pointer.
    struct PendingIntcRegistration {
        std::uint32_t callPC=0,syscallPC=0,returnPC=0,syscallNumberV1=0;
        std::int32_t sourceA0=0;
        std::uint32_t sourceA1=0;
        std::int32_t sourceA2=0;
    };
private:
    friend struct ::original_main_entry50KG::Application;
    using Application=::original_main_entry50KG::Application;
    template<std::size_t N> struct Bytes {
        // Host initialization only prevents C++ indeterminate reads. A zero
        // data byte is NOT source-known unless its corresponding mask says so.
        std::array<std::uint8_t,N> data{},known{};
        void byte(std::size_t p,std::uint8_t v)noexcept {data[p]=v;known[p]=255;}
        void word(std::size_t p,std::uint32_t v)noexcept {
            for(unsigned i=0;i<4;++i)byte(p+i,std::uint8_t(v>>(8*i)));
        }
        void doubleword(std::size_t p,std::uint64_t v)noexcept {
            for(unsigned i=0;i<8;++i)byte(p+i,std::uint8_t(v>>(8*i)));
        }
        void masked(std::size_t p,std::uint8_t mask,std::uint8_t v)noexcept {
            data[p]=std::uint8_t((data[p]&~mask)|(v&mask));known[p]|=mask;
        }
        void zero16(std::size_t p)noexcept {for(unsigned i=0;i<16;++i)byte(p+i,0);}
        void typedPointer(std::size_t p)noexcept {
            // The native allocation, including NULL, owns the pointer result.
            // Neither its host address nor an invented guest address is stored.
            for(unsigned i=0;i<4;++i)known[p+i]=0;
        }
        std::optional<std::uint32_t> readWord(std::size_t p)const noexcept {
            std::uint32_t v=0;
            for(unsigned i=0;i<4;++i){if(known[p+i]!=255)return {};v|=std::uint32_t(data[p+i])<<(8*i);}
            return v;
        }
    };
    struct AlignedDelete {
        void operator()(std::uint8_t* p)const noexcept {
            if(p)::operator delete[](p,std::align_val_t{64});
        }
    };
    struct Buffer2592D0 {
        Bytes<0x20> source;
        std::unique_ptr<std::uint8_t[],AlignedDelete> backing;
        bool backingAllocationAttempted=false,pointer00Published=false;
        bool uncachedAlias00=false,invalidNullAlias00=false,backingZeroed=false;
        bool cursor04Aliases00=false,constructorReturned=false;
    };
    struct Storage {
        Bytes<0x510> source;
        // One original pointer slot. Its contents remain unknown after malloc;
        // this is not a second native callback registration/dispatch authority.
        std::unique_ptr<OriginalVideoPreFlipList50KG::Slot[]> preFlipSlots1C;
        bool preFlipPointerPublished=false,baseReturned=false,constructorReturned=false;
        //25A414's actual typed publication. This is ten source records of
        //five words each, not a host display-mode list or selected GL mode.
        std::unique_ptr<Bytes<0xc8>> modeTable38;
        bool modeTablePointerPublished=false;
        std::uint32_t modeTableWordsWritten=0;
        //25A5EC owns the pending32-byte object BEFORE2592D0 returns. The
        //Video+88 publication waits for25A614 after an owned allocator result.
        std::unique_ptr<Buffer2592D0> buffer88;
        bool buffer88AllocationAttempted=false,buffer88Published=false;
        //25A610/62C/648/664 allocate four distinct48-byte source objects.
        //Their private native metadata never initializes unknown source lanes.
        std::array<std::unique_ptr<Bytes<0x30>>,4> packets;
        std::array<bool,4> packetAllocationAttempted{},packetConstructorReturned{},packetPublished{};
    };
    // Native exclusivity for THIS pending Video259B80 request only, not a
    // kernel INTC registry and not ownership of every handler on channel0.
    // No host hook, function pointer, event source, or source return is stored.
    struct NativePendingIntcProcess {
        const std::thread::id thread=std::this_thread::get_id();
        const OriginalApplicationVideoSource50LA* owner=nullptr;
        const Application* application=nullptr;
        const Storage* sourceIdentity=nullptr;
        std::uint64_t generation=0;
    };
    static NativePendingIntcProcess& nativePendingIntcProcess()noexcept {
        static NativePendingIntcProcess value;return value;
    }
    std::unique_ptr<Storage> allocation_;
    Storage* global443A60_=nullptr;
    const Application* application_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    bool attempted_=false,published443A60_=false,nativeClosed_=false;
    const OriginalApplicationStartupCall23A0DCNative50LA* nativeInitializerCaller_=nullptr;
    bool allocatorReturnUnowned_=false;
    std::uint32_t blockedAllocationCallPC_=0;
    bool modeTableEntered_=false;
    bool modeDescendantsEntered_=false;
    std::optional<PendingIntcRegistration> registrationPending_;
    bool nativeIntcCustodyClaimed_=false,nativeIntcCustodyReleased_=false;
    std::uint64_t nativeIntcCustodyGeneration_=0;
    std::uint32_t pc_=0;

    bool constructBase2AE080(Storage& s)noexcept {
        auto& x=s.source;
        x.word(0x2c,0x3d4470); //2AE098.
        //2AE0A0→2AE1E0 at this+18. Only bit0 of the list flags is written.
        x.word(0x1c,0);x.word(0x18,0);x.word(0x20,0);x.word(0x24,0);x.masked(0x28,1,0);
        x.word(0,0);x.word(4,0);x.word(8,0);x.masked(0x0c,1,0);x.word(0x10,0);x.word(0x14,0);
        //2AE0F8→2AE210(1,1). Share the existing process guard records, never
        // reset or copy them per Application or native Video incarnation.
        auto& records=OriginalVideoPreFlipList50KG::records();
        OriginalVideoPreFlipList50KG::firstUse(records.size8D14);
        // Source size1>0 assertion passes, independently of its enable word.
        OriginalVideoPreFlipList50KG::firstUse(records.empty8D18);
        // Source capacity0==0 assertion passes. The allocation can fail.
        pc_=0x2ae308;
        s.preFlipSlots1C.reset(new(std::nothrow) OriginalVideoPreFlipList50KG::Slot[1]);
        if(!s.preFlipSlots1C){
            //2AE308->2E8CA0->2E8D90. Host failure does not own the mutable
            //new-handler/retry/throw/NULL outcome. Stop before2AE310 stores.
            allocatorReturnUnowned_=true;blockedAllocationCallPC_=0x2ae308;return false;
        }
        s.preFlipPointerPublished=true;x.typedPointer(0x1c); //2AE310 after owned return.
        x.word(0x20,1);x.word(0x24,1);x.masked(0x28,1,1); //2AE314/318/330.
        //2AE080 ignores2AE210's allocation-result boolean and returns this.
        s.baseReturned=true;return true;
    }
    bool construct259E00(Storage& s)noexcept {
        pc_=0x259e1c;if(!constructBase2AE080(s))return false;
        auto& x=s.source;
        x.word(0x2c,0x3d22c0); //259E34.
        x.word(0x84,0);x.word(0x6c,0);x.word(0x70,0);x.word(0x64,0);x.word(0x68,0);
        x.word(0x74,0);x.word(0x78,0);x.word(0x7c,0);x.word(0x2d4,0);x.word(0x88,0);x.word(0x40,0);
        x.masked(0x0c,1,0);x.word(0x38,0);x.word(0x3c,1);x.word(0,0);x.word(4,0);
        x.zero16(0x48); //259ED8→306478(this+48,0,16), native bounded memset.
        x.word(0x2e4,0);x.word(0x2e0,0);x.word(8,32);x.word(0x58,0);
        x.word(0x5c,0x31);x.word(0x60,0x00ffffff);x.word(0x44,0);
        // These are original constructor constants, not a copied native mode.
        // Width/height remain0/0; a24-bit maximum does not prove mode selection.
        x.masked(0x2ef,1,1);x.masked(0x2ef,2,0);x.masked(0x2ef,4,0);
        x.byte(0x2ec,0);x.byte(0x2ed,0);x.byte(0x2ee,127);x.word(0x2f0,128);
        x.byte(0x405,0);x.word(0x408,0);x.word(0x2c0,4);x.byte(0x80,0);x.byte(0x81,0);
        x.word(0x3d4,0);x.word(0x3d8,0);
        for(std::size_t i=0;i<4;++i){ //259FF0..25A0BC, source order per view.
            x.byte(0x2f4+i,0);x.byte(0x300+i,0);x.word(0x324+4*i,0);x.word(0x334+4*i,128);
            x.byte(0x2fc+i,0);x.byte(0x2f8+i,0);x.word(0x344+4*i,0);x.word(0x304+4*i,0);
            x.word(0x314+4*i,0x80000000u);x.word(0x354+4*i,0);x.word(0x374+4*i,0);
            x.word(0x364+4*i,0);x.word(0x3c4+4*i,0);x.zero16(0x384+16*i);
        }
        x.word(0x3ec,128);x.byte(0x3dc,0);x.word(0x3e4,0);x.word(0x3e0,0x80000000u);
        x.word(0x3f0,0);x.word(0x3f4,0);x.word(0x3f8,0);x.word(0x3fc,0);x.word(0x400,0);
        for(unsigned bit=0;bit<6;++bit)x.masked(0x404,std::uint8_t(1u<<bit),0);
        // There is deliberately NO write to3E8. Upper404/2EF/0C/28 bits and
        // every other unwritten byte also remain unknown, rather than zero.
        x.word(0x490,0);
        for(std::size_t i=0;i<4;++i){ //25A1D0..25A22C.
            x.word(0x40c+4*i,0);x.word(0x41c+4*i,0);
            x.doubleword(0x430+8*i,0x0000008000000044ull);x.zero16(0x450+16*i);
        }
        x.word(0x4e4,0);
        for(std::size_t i=0;i<4;++i){ //25A240..25A294.
            x.byte(0x494+i,0);x.byte(0x498+i,0);x.byte(0x49c+i,0);x.byte(0x4a0+i,0);
            x.zero16(0x4a4+16*i);
        }
        x.byte(0x4e8,0);x.word(0x4ec,500);x.word(0x4f0,0);x.word(0x4f4,0);x.word(0x4f8,0);
        x.word(0x500,2);x.word(0x504,2); //25A2CC/2D4.
        s.constructorReturned=true;pc_=0x2376b8;return true;
    }
    OriginalApplicationVideoSource50LA()noexcept=default;
public:
    enum class Result : std::uint8_t {authority,alreadyEntered,publishedConstructorOnly,publishedNullBeforeModeTable,
        allocatorReturnUnowned};
    enum class ModeTableResult : std::uint8_t {
        authority,phase,nullOwnerAt25A3F0,nullTableAt25A428,descendantAllocationPending
    };
    enum class DescendantResult : std::uint8_t {
        authority,phase,invalidUncachedAllocationAt259348,callbacksPending,allocatorReturnUnowned
    };
    enum class RegistrationResult : std::uint8_t {authority,phase,syscallPending};
    enum class NativeIntcCustodyResult : std::uint8_t {
        authority,phase,otherOwner,exhausted,heldPendingSyscall
    };
    enum class NativeModeQueryStatus : std::uint8_t {
        authority,tableUnavailable,unknownSourceData,unownedRow,computed
    };
    enum class NativeClearPacketStatus : std::uint8_t {authority,backingUnavailable,allocation,encodingBounds,encoded};
    struct NativeClearPacket {
        NativeClearPacketStatus status=NativeClearPacketStatus::authority;
        std::unique_ptr<const OriginalVideoClearPacket25CAD0Native50LA> packet;
    };
    struct NativeModeQuery {
        NativeModeQueryStatus status=NativeModeQueryStatus::authority;
        // Numeric query result only. A matching index is NOT a selected mode,
        // source return, writable row, or renderer/interrupt capability.
        std::optional<std::int32_t> index;
    };
    struct Observation {
        bool attempted=false,allocationPresent=false,baseReturned=false,constructorReturned=false;
        bool published443A60=false,publishedNull=false,nativeClosed=false;
        bool nativeInitializerBound=false,allocatorReturnUnowned=false;
        std::uint32_t blockedAllocationCallPC=0;
        bool preFlipPointerPublished=false,preFlipBacking=false;
        bool modeTableEntered=false,modeTablePointerPublished=false,modeTableBacking=false;
        std::uint32_t modeTableWordsWritten=0;
        std::uint32_t nextSourcePC=0;
        std::optional<std::uint32_t> width00,height04,colorBits08,modeCount30,modeField34;
        std::optional<std::uint32_t> mode44,zBufferFormat5C,packetMaximum60,alpha3E8;
        // Per-row source words00/04/08/0C/10. Numeric copies only: no source
        // owner, mode readiness, renderer admission, or writable table handle.
        std::array<std::array<std::optional<std::uint32_t>,5>,10> modeTableRows{};
        std::optional<std::uint32_t> field10;
        bool modeDescendantsEntered=false,buffer88AllocationAttempted=false,buffer88Present=false,buffer88Published=false;
        bool buffer88BackingAllocationAttempted=false,buffer88BackingPresent=false,buffer88Pointer00Published=false;
        bool buffer88UncachedAlias00=false,buffer88InvalidNullAlias00=false,buffer88BackingZeroed=false;
        bool buffer88Cursor04Aliases00=false,buffer88ConstructorReturned=false;
        //Pointer00/04 are typed identities, not numerical zero/host addresses.
        std::array<std::optional<std::uint32_t>,8> buffer88Words{};
        std::array<bool,4> packetAllocationAttempted{},packetPresent{},packetConstructorReturned{},packetPublished{};
        std::array<std::array<std::optional<std::uint32_t>,12>,4> packetWords{};
        std::optional<PendingIntcRegistration> registrationPending;
        bool nativeIntcCustodyClaimed=false,nativeIntcCustodyCurrent=false,nativeIntcCustodyReleased=false;
        std::uint64_t nativeIntcCustodyGeneration=0;
    };
    ~OriginalApplicationVideoSource50LA()noexcept {
        //Application's existing checked native-close path must release its
        //own custody before destroying the retained source allocation.
        if(nativeIntcCustodyClaimed_&&!nativeIntcCustodyReleased_)std::terminate();
    }
    OriginalApplicationVideoSource50LA(const OriginalApplicationVideoSource50LA&)=delete;
    OriginalApplicationVideoSource50LA& operator=(const OriginalApplicationVideoSource50LA&)=delete;
private:
    Result begin2376A0(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_)return Result::authority;
        if(attempted_)return Result::alreadyEntered;
        attempted_=true;application_=&app;pc_=0x2376a0;
        // A native typed allocation represents original requested size0x510.
        // Source bytes have exactly0x510 lanes; extra metadata is never guest data.
        allocation_.reset(new(std::nothrow) Storage);
        if(!allocation_){
            //2E8D90 failure reads mutable383730/383738. No source allocator
            //return,2376A8 NULL branch,ctor skip or443A60 publication is known.
            allocatorReturnUnowned_=true;blockedAllocationCallPC_=0x2376a0;
            return Result::allocatorReturnUnowned;
        }
        if(!construct259E00(*allocation_))return Result::allocatorReturnUnowned;
        //Only the actual returned receiver is published. A genuinely proven
        //source NULL return could use2376A8's skip, but host failure cannot.
        global443A60_=allocation_.get();published443A60_=true;
        pc_=0x2376c0; //The very next25A3E0 mode-table call has NOT happened.
        return Result::publishedConstructorOnly;
    }
    ModeTableResult continueModeTable25A3E0(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app)
            return ModeTableResult::authority;
        if(!attempted_||!published443A60_||modeTableEntered_||pc_!=0x2376c0)
            return ModeTableResult::phase;
        if(global443A60_!=allocation_.get()||
           (allocation_&&(!allocation_->baseReturned||!allocation_->constructorReturned)))
            return ModeTableResult::authority;
        // Exact2376C0→25A3E0 receiver from the preceding publication, never a
        // caller-provided Video, native window, copied dimensions, or ready flag.
        modeTableEntered_=true;pc_=0x25a3f0;
        if(!global443A60_)return ModeTableResult::nullOwnerAt25A3F0;
        auto& s=*global443A60_;auto& x=s.source;
        x.word(0x30,10); //25A3F0, retained even if allocation below fails.
        x.word(0x34,4);  //25A3F8. Merely the source field, not an active mode.
        pc_=0x25a40c;
        //305700 receives (10*5)*4=200 bytes. Typed native backing adapts the
        //allocation only; no original heap-global state or address is invented.
        s.modeTable38.reset(new(std::nothrow) Bytes<0xc8>);
        s.modeTablePointerPublished=true;x.typedPointer(0x38); //25A414, even NULL.
        pc_=0x25a428;
        if(!s.modeTable38)return ModeTableResult::nullTableAt25A428;
        // Source25A428..25A5E8 writes these50 words in increasing offset
        // order. All values come from that source body, not native Video.
        static constexpr std::uint32_t rows[10][5]={
            {256,448,32,0x002b0000,0},
            {320,448,32,0x0025c000,0},
            {384,448,32,0x00208000,0},
            {512,448,32,0x00160000,0},
            {640,448,32,0x000b8000,0},
            {256,448,16,0x002b0000,0},
            {320,448,16,0x0025c000,0},
            {384,448,16,0x00208000,0},
            {512,448,16,0x00160000,0},
            {640,448,16,0x000b8000,0}
        };
        for(std::size_t row=0;row<10;++row)for(std::size_t lane=0;lane<5;++lane){
            s.modeTable38->word(20*row+4*lane,rows[row][lane]);
            ++s.modeTableWordsWritten;
        }
        pc_=0x25a5ec;
        // The next2E8D90 allocation and its25A5F0 delay-slot write of0x400000
        //to source+10 have NOT executed. Neither2592D0 nor later constructors,
        //platform callbacks/MMIO,25A3E0 return, or display-mode setup is issued.
        return ModeTableResult::descendantAllocationPending;
    }
    bool construct18_2592D0(Buffer2592D0& b)noexcept {
        //Reached2592D0(this,18,1): rounded units=(18+7)/8=3,
        //aligned allocation size=3<<7=384, capacity=3<<3=24.
        pc_=0x259314;b.backingAllocationAttempted=true;
        //305500->305530 is a direct aligned allocator with a NULL-return
        //contract (3055C0/3055C4), not the mutable2E8D90 new-handler wrapper.
        //This bounded native adaptation owns backing/NULL, not guest heap state.
        b.backing.reset(static_cast<std::uint8_t*>(
            ::operator new[](384,std::align_val_t{64},std::nothrow)));
        //259324/259328 mask to28 bits;259330 forms the original alias:
        //(address &0x0FFFFFFF)|0x20000000. Never apply this to a native
        //host pointer. Native NULL maps to source alias0x20000000, but no
        //backing is owned for the zeroing call. Stop before259348; this does
        //not assert an inevitable guest fault or any actual guest heap outcome.
        b.pointer00Published=true;b.uncachedAlias00=true;b.source.typedPointer(0);
        b.invalidNullAlias00=!b.backing;pc_=0x259348;
        if(!b.backing)return false;
        std::memset(b.backing.get(),0,384);b.backingZeroed=true;
        b.source.word(8,24); //259358, delay slot of the2593F0 reset call.
        //2593F0 copies pointer00 to04, i.e. the cursor points to the SAME
        //uncached buffer start. It does NOT store numeric zero at04.
        b.source.typedPointer(4);b.cursor04Aliases00=true;
        b.source.word(0x0c,0);b.source.word(0x10,0);b.source.word(0x14,0);
        b.source.word(0x18,0);b.source.word(0x1c,0);
        b.constructorReturned=true;return true;
    }
    DescendantResult continueModeDescendants25A5EC(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app)
            return DescendantResult::authority;
        if(!modeTableEntered_||modeDescendantsEntered_||pc_!=0x25a5ec)
            return DescendantResult::phase;
        if(!published443A60_||!allocation_||global443A60_!=allocation_.get()||
           !allocation_->constructorReturned||!allocation_->modeTablePointerPublished||
           !allocation_->modeTable38||allocation_->modeTableWordsWritten!=50)
            return DescendantResult::authority;
        auto& s=*allocation_;auto& x=s.source;modeDescendantsEntered_=true;
        //25A5F0 executes before the25A5EC allocator enters. Source a0 still
        //holds32 from25A424; no native Video constants or supplied sizes.
        x.word(0x10,0x00400000);s.buffer88AllocationAttempted=true;
        s.buffer88.reset(new(std::nothrow) Buffer2592D0);
        if(!s.buffer88){
            //No source2E8D90 result: retain25A5F0 but do not take25A5F4's
            //NULL skip, call2592D0, publish+88 or enter the next allocation.
            allocatorReturnUnowned_=true;blockedAllocationCallPC_=pc_=0x25a5ec;
            return DescendantResult::allocatorReturnUnowned;
        }
        if(!construct18_2592D0(*s.buffer88))
            return DescendantResult::invalidUncachedAllocationAt259348;
        //The four publications below are the NEXT allocator call's delay
        //slots, so each preceding result becomes visible BEFORE that call.
        static constexpr std::uint32_t allocationPC[4]={0x25a610,0x25a62c,0x25a648,0x25a664};
        static constexpr std::uint32_t constructorPC[4]={0x25a620,0x25a63c,0x25a658,0x25a674};
        static constexpr std::size_t publicationOffset[4]={0x3d4,0x3d8,0x490,0x4e4};
        for(std::size_t i=0;i<4;++i){
            pc_=allocationPC[i];
            if(i==0){s.buffer88Published=true;x.typedPointer(0x88);} //25A614.
            else{s.packetPublished[i-1]=true;x.typedPointer(publicationOffset[i-1]);}
            s.packetAllocationAttempted[i]=true;
            s.packets[i].reset(new(std::nothrow) Bytes<0x30>);
            if(!s.packets[i]){
                //The preceding object's publication occurred in THIS call's
                //delay slot. Preserve it, but no NULL skip/new publication.
                allocatorReturnUnowned_=true;blockedAllocationCallPC_=pc_;
                return DescendantResult::allocatorReturnUnowned;
            }
            {
                pc_=constructorPC[i];auto& q=*s.packets[i];
                //133F80 stores2C=-1,24=0,28=0,20=NULL, then returns this.
                //In particular bytes00..1F remain unknown, not zeroed data.
                q.word(0x2c,0xffffffffu);q.word(0x24,0);q.word(0x28,0);q.word(0x20,0);
                s.packetConstructorReturned[i]=true;
            }
        }
        s.packetPublished[3]=true;x.typedPointer(0x4e4); //25A67C, actual last result.
        pc_=0x25a68c;
        //2F5880(0,259B80,-1) callback installation is next and UNCALLED.
        //No2F66D8 call, MMIO,25A3E0 return, source mode setup, or native
        //frame/renderer/sky admission is manufactured by these CPU owners.
        return DescendantResult::callbacksPending;
    }
    RegistrationResult enterIntcRegistration25A68C(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app)
            return RegistrationResult::authority;
        if(!modeDescendantsEntered_||pc_!=0x25a68c||registrationPending_)
            return RegistrationResult::phase;
        if(!published443A60_||!allocation_||global443A60_!=allocation_.get()||
           !allocation_->buffer88Published)return RegistrationResult::authority;
        for(bool published:allocation_->packetPublished)
            if(!published)return RegistrationResult::authority;
        //25A680/684 form259B80,25A688 supplies-1, and the25A690 delay slot
        //supplies0. The2F5880 stub setsv1=0x10, then reaches SYSCALL2F5884.
        //Retain the exact source request and caller continuation, but DO NOT
        //execute the syscall, invent its return/handler ID, register a native
        //hook, enable a channel, dispatch259B80, or advance the source caller.
        registrationPending_=PendingIntcRegistration{
            0x25a68c,0x2f5884,0x25a694,0x10,0,0x259b80,-1};
        pc_=0x2f5884;
        //There is deliberately no acknowledge/complete/backend-success API.
        //A real owned interrupt/event backend must be joined separately.
        return RegistrationResult::syscallPending;
    }
    bool nativeIntcCustodyCurrent()const noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||!nativeIntcCustodyClaimed_||
           nativeIntcCustodyReleased_||!allocation_||!application_||pc_!=0x2f5884||
           !registrationPending_||global443A60_!=allocation_.get())return false;
        const auto& p=nativePendingIntcProcess();
        //The immutable affinity check precedes all mutable process-ledger reads.
        return p.thread==thread_&&p.owner==this&&p.application==application_&&
            p.sourceIdentity==allocation_.get()&&p.generation==nativeIntcCustodyGeneration_;
    }
    NativeIntcCustodyResult holdPendingIntcNativeCustody(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app)
            return NativeIntcCustodyResult::authority;
        if(pc_!=0x2f5884||!registrationPending_)return NativeIntcCustodyResult::phase;
        if(!modeDescendantsEntered_||!published443A60_||!allocation_||
           global443A60_!=allocation_.get())return NativeIntcCustodyResult::authority;
        if(nativeIntcCustodyClaimed_)
            return nativeIntcCustodyCurrent()?NativeIntcCustodyResult::heldPendingSyscall:NativeIntcCustodyResult::phase;
        auto& p=nativePendingIntcProcess();
        if(p.thread!=thread_)return NativeIntcCustodyResult::authority;
        if(p.owner||p.application||p.sourceIdentity)return NativeIntcCustodyResult::otherOwner;
        if(p.generation==std::numeric_limits<std::uint64_t>::max())return NativeIntcCustodyResult::exhausted;
        //No allocations or fallible actions follow these native-only stores.
        //This is custody of an UNEXECUTED request, not registration. Source
        //PC2F5884, the pending call image, source bytes, and442D08 are unchanged.
        p.owner=this;p.application=&app;p.sourceIdentity=allocation_.get();
        nativeIntcCustodyGeneration_=++p.generation;nativeIntcCustodyClaimed_=true;
        return NativeIntcCustodyResult::heldPendingSyscall;
    }
    bool releasePendingIntcNativeCustody()noexcept {
        if(!nativeIntcCustodyClaimed_||nativeIntcCustodyReleased_)return true;
        if(!nativeIntcCustodyCurrent())return false;
        auto& p=nativePendingIntcProcess();
        p.owner=nullptr;p.application=nullptr;p.sourceIdentity=nullptr;
        nativeIntcCustodyReleased_=true;
        //Keep the generation/history and exact source pending request. This
        //does not execute source Disable/Remove, clear443A60, or acknowledge
        //the pending syscall. It cannot release a different owner's custody.
        return true;
    }
    NativeClearPacket encodeNativeClearPacket25CAD0(const Application& app,
        std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t alpha)const noexcept {
        using S=NativeClearPacketStatus;using Packet=OriginalVideoClearPacket25CAD0Native50LA;
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app||
           !attempted_||!published443A60_||!allocation_||global443A60_!=allocation_.get())return {};
        const auto& storage=*allocation_;
        if(!storage.constructorReturned||!modeDescendantsEntered_||allocatorReturnUnowned_||
           !storage.buffer88AllocationAttempted||!storage.buffer88||!storage.buffer88Published)
            return {S::backingUnavailable,{}};
        const auto& buffer=*storage.buffer88;
        if(!buffer.constructorReturned||!buffer.backingAllocationAttempted||!buffer.backing||
           !buffer.pointer00Published||!buffer.uncachedAlias00||buffer.invalidNullAlias00||!buffer.backingZeroed||
           !buffer.cursor04Aliases00||buffer.source.readWord(8)!=24)return {S::backingUnavailable,{}};
        for(std::size_t offset=0x0c;offset<0x20;offset+=4)
            if(buffer.source.readWord(offset)!=0)return {S::backingUnavailable,{}};
        //Explicit native output allocation, NOT an original allocator call.
        //The source buffer is used only to establish the actual owned capacity;
        //never copy/mutate its bytes, metadata, pending PC or IRQ custody.
        std::unique_ptr<Packet> packet(new(std::nothrow) Packet);
        if(!packet)return {S::allocation,{}};
        if(!packet->encode(red,green,blue,alpha))return {S::encodingBounds,{}};
        return {S::encoded,std::move(packet)};
        //25A87C/25CAD0 were not entered. This independent immutable encoding
        //has no framebuffer, mode, source address, submit or completion grant.
    }
    NativeModeQuery queryNativeMode25A900(const Application& app,
        std::uint32_t width,std::uint32_t height,std::uint32_t colorBits)const noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||application_!=&app||
           !attempted_||!published443A60_||!allocation_||global443A60_!=allocation_.get())
            return {};
        const auto& s=*allocation_;
        // Explicit native data query over this exact source allocation. This
        // does NOT enter25A900 in source control flow, advance pc_, complete
        // the pending syscall, or perform25A7B0's writes/frame operations.
        const auto count=s.source.readWord(0x30); //25A900.
        if(!count)return {NativeModeQueryStatus::unknownSourceData,{}};
        //25A950's signed SLT rejects zero and negative counts before any row
        //pointer/read. Avoid an implementation-defined unsigned-to-signed cast.
        if(*count==0||(*count&0x80000000u)!=0)
            return {NativeModeQueryStatus::computed,std::int32_t{-1}};
        if(!s.modeTablePointerPublished||!s.modeTable38)
            return {NativeModeQueryStatus::tableUnavailable,{}};
        const auto& table=*s.modeTable38;
        for(std::uint32_t i=0;i<*count;++i){
            // A larger source count cannot grant bytes beyond the actually
            // owned200-byte allocation. Never clamp it into a false -1 result.
            if(i>=table.data.size()/20)
                return {NativeModeQueryStatus::unownedRow,{}};
            const std::size_t p=20*std::size_t(i);
            // Keep original short-circuit order: bits, height, then width.
            // A mismatch never reads later unknown lanes of the same row.
            const auto bits=table.readWord(p+8); //25A918/91C.
            if(!bits)return {NativeModeQueryStatus::unknownSourceData,{}};
            if(*bits!=colorBits)continue;
            const auto h=table.readWord(p+4); //25A924/928.
            if(!h)return {NativeModeQueryStatus::unknownSourceData,{}};
            if(*h!=height)continue;
            const auto w=table.readWord(p); //25A930/934.
            if(!w)return {NativeModeQueryStatus::unknownSourceData,{}};
            if(*w==width)return {NativeModeQueryStatus::computed,static_cast<std::int32_t>(i)};
        }
        return {NativeModeQueryStatus::computed,std::int32_t{-1}}; //25A95C.
    }
    bool closeNative(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        if(!releasePendingIntcNativeCustody())return false;
        nativeClosed_=true;
        // Retain diagnostic backing until Application destruction. This revokes
        // native use only; no25A300/2AE120 source destructor or global clear claim.
        return true;
    }
    bool inspect(const Application& app,Observation& out)const noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        out={};out.attempted=attempted_;out.allocationPresent=bool(allocation_);
        out.published443A60=published443A60_;out.publishedNull=published443A60_&&!global443A60_;
        out.nativeInitializerBound=nativeInitializerCaller_!=nullptr;out.allocatorReturnUnowned=allocatorReturnUnowned_;
        out.blockedAllocationCallPC=blockedAllocationCallPC_;
        out.nativeClosed=nativeClosed_;out.nextSourcePC=pc_;
        out.modeTableEntered=modeTableEntered_;
        out.modeDescendantsEntered=modeDescendantsEntered_;
        out.registrationPending=registrationPending_;
        out.nativeIntcCustodyClaimed=nativeIntcCustodyClaimed_;
        out.nativeIntcCustodyCurrent=nativeIntcCustodyCurrent();
        out.nativeIntcCustodyReleased=nativeIntcCustodyReleased_;
        out.nativeIntcCustodyGeneration=nativeIntcCustodyGeneration_;
        if(!allocation_)return true;
        const auto& s=*allocation_;const auto& x=s.source;
        out.baseReturned=s.baseReturned;out.constructorReturned=s.constructorReturned;
        out.preFlipPointerPublished=s.preFlipPointerPublished;out.preFlipBacking=bool(s.preFlipSlots1C);
        out.modeTablePointerPublished=s.modeTablePointerPublished;out.modeTableBacking=bool(s.modeTable38);
        out.modeTableWordsWritten=s.modeTableWordsWritten;out.field10=x.readWord(0x10);
        out.buffer88AllocationAttempted=s.buffer88AllocationAttempted;
        out.buffer88Present=bool(s.buffer88);out.buffer88Published=s.buffer88Published;
        if(s.buffer88){const auto& b=*s.buffer88;
            out.buffer88BackingAllocationAttempted=b.backingAllocationAttempted;
            out.buffer88BackingPresent=bool(b.backing);out.buffer88Pointer00Published=b.pointer00Published;
            out.buffer88UncachedAlias00=b.uncachedAlias00;out.buffer88InvalidNullAlias00=b.invalidNullAlias00;
            out.buffer88BackingZeroed=b.backingZeroed;out.buffer88Cursor04Aliases00=b.cursor04Aliases00;
            out.buffer88ConstructorReturned=b.constructorReturned;
            for(std::size_t i=0;i<8;++i)out.buffer88Words[i]=b.source.readWord(4*i);
        }
        out.packetAllocationAttempted=s.packetAllocationAttempted;
        out.packetConstructorReturned=s.packetConstructorReturned;out.packetPublished=s.packetPublished;
        for(std::size_t i=0;i<4;++i){out.packetPresent[i]=bool(s.packets[i]);
            if(s.packets[i])for(std::size_t lane=0;lane<12;++lane)
                out.packetWords[i][lane]=s.packets[i]->readWord(4*lane);
        }
        if(s.modeTable38)for(std::size_t row=0;row<10;++row)for(std::size_t lane=0;lane<5;++lane)
            out.modeTableRows[row][lane]=s.modeTable38->readWord(20*row+4*lane);
        out.width00=x.readWord(0);out.height04=x.readWord(4);out.colorBits08=x.readWord(8);
        out.modeCount30=x.readWord(0x30);out.modeField34=x.readWord(0x34);out.mode44=x.readWord(0x44);
        out.zBufferFormat5C=x.readWord(0x5c);out.packetMaximum60=x.readWord(0x60);out.alpha3E8=x.readWord(0x3e8);
        return true; //Read-only values are not an owner/mode/renderer capability.
    }
};
}
