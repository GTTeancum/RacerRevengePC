#pragma once
#include "original_application_entities50la.h"
#include "original_application_input04_source50la.h"
#include "original_source_world_rows_borrow50la.h"
#include "original_player_race_music_borrow50la.h"
#include "original_player_overlay_renderer_lease50la.h"
#include "original_racer_entity_cold50la.h"
#include <array>
#include <memory>
#include <new>
#include <cstring>
#include <thread>
#include <limits>
#include <type_traits>

namespace ra_cold_aggregate50LA { class Owner; }
namespace original_racer_manager_entity { class Owner; }
namespace rr::game {
// Private future-issuer primitive, NOT a Player/selected-ID/POD receipt. No
// existing Session or loaded Racer can be promoted into this construction.
// 1ADF80's caller joins (Profile, human index, World) remain outside this owner.
class OriginalPlayerConstructorPrefix50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::original_racer_manager_entity::Owner; //Exact private1AE32C occurrence only.
    using C=OriginalWreckComponent50LA;
    enum class Status { input,nativeCapacity,allocation,entity,typeRegistry,playerConstructorReturned };
    struct Result { Status status=Status::input;std::uint32_t sourcePC=0x1adf80; };
    // Host bytes are initialized for safety, NEVER evidence of source stores.
    // Only known bits may be observed. Entity00..29 lives solely in entity;
    // flags29's upper nibble is unknown despite the native primitive's storage.
    struct Allocation {
        C entity;
        std::array<std::uint8_t,0x10d0> bytes{},known{};
        // A raw unknown-value COPY, not a reference to the mutable CF8 field.
        // Identity is this nonmovable allocation epoch + original source offset.
        // Later writes to CF8 must NOT resolve or rewrite the captured78 value.
        const Allocation* copied78Epoch=nullptr;
        std::uint16_t copied78SourceOffset=0;
        void copyUnknownCF8To78()noexcept {
            copied78Epoch=this;copied78SourceOffset=0xcf8;
            // Do not read host bytes, convert to float, or invent known bits.
            // Both fields remain numerically unknown at this boundary.
        }
        void byte(std::size_t at,std::uint8_t value,std::uint8_t mask=0xff)noexcept {
            bytes[at]=static_cast<std::uint8_t>((bytes[at]&~mask)|(value&mask));
            known[at]=static_cast<std::uint8_t>(known[at]|mask);
        }
        void word(std::size_t at,std::uint32_t value)noexcept {
            for(unsigned i=0;i<4;++i)byte(at+i,static_cast<std::uint8_t>(value>>(8*i)));
        }
        void vf0(std::size_t at)noexcept {
            word(at,0);word(at+4,0);word(at+8,0);word(at+12,0x3f800000);
        }
    };
    std::shared_ptr<OriginalApplicationEntities50LA> app_;
    std::unique_ptr<Allocation> allocation_;
    std::thread::id thread_;
    std::uint64_t generation_=0,applicationRootGeneration_=0,managerKey_=0;
    //Borrowed from the ONE owning manager. Its close cancels this Player
    //before retiring/freeing that manager's Entity; no public caller can bind.
    const C* sourceManager_=nullptr;
    bool attempted_=false,counted_=false,enrolled_=false;
    Result result_{};
    using Input=OriginalApplicationInput04Source50LA;
    std::shared_ptr<Input> sourceInput_;
    Input::Controller* logicalController_=nullptr;
    std::array<std::unique_ptr<Input::Axis>,4> sourceAxes_;
    std::unique_ptr<Input::Bytes<0x14>> sourceFF4_;
    bool loadAttempted_=false,inputBorrowed_=false;
    std::uint32_t loadPC_=0;
    std::unique_ptr<C> source7F8_;
    std::unique_ptr<OriginalPodAIDamageOverlay50LA> sourceOverlay240_;
    std::shared_ptr<OriginalPlayerOverlayRendererLease50LA> sourceOverlayRenderer_;
    struct SourcePointerArray {
        std::unique_ptr<const void*[]> values;
        std::uint32_t extent=0,initialized=0;
        bool published=false;
    };
    // Native typed pointer storage, never guest addresses or default-known0.
    std::array<SourcePointerArray,6> sourcePointerArrays_; //800,804,808,81C,80C,818.
    bool pointerArraysAttempted_=false,overlayAttached_=false,overlayMask12Written_=false;
    bool overlayAttempted_=false,overlayCounted_=false,overlayEnrolled_=false;
    bool baseLoadAttempted_=false,source7F8Counted_=false,source7F8Enrolled_=false;
    const void *sourceTerrain330_=nullptr,*sourceWater334_=nullptr,*sourceWorld338_=nullptr;
    std::shared_ptr<const OriginalPlayerRaceMusicBorrow50LA> sourceMusic33C_;
    std::array<char,6> sourceLoadPodName_{};
    std::array<char,12> sourceLoadPodDirectory_{};
    // World custody is joined by the authentic manager occurrence, not by
    // a public pointer/ordinal receipt or unrelated native prewarm operation.

    OriginalPlayerConstructorPrefix50LA()noexcept=default;
    static std::uint64_t key(const C& e)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e));
    }
    // Allocation is a native typed adaptation of the source0x10D0 request at
    // 1ADFD4. It is NOT guest ABI storage, nor permission to run1ADF80 early.
    Result beginNativePrefix(const std::shared_ptr<OriginalApplicationEntities50LA>& app,const C& manager)noexcept {
        if(attempted_)return result_;
        attempted_=true;
        if(!app||!app->application10()||!app->application26C()||app->attachmentLock423F58())return result_;
        auto& authority=app->entityAuthority_;
        if(manager.kind!=OriginalWreckComponentKind50LA::entity141300||!manager.sessionGeneration||
           !manager.objectKey||manager.reference00!=1||!authority.canRetire(manager))return result_;
        if(!authority.reserveAdditional(1))return result_={Status::nativeCapacity,0x1adfd4};
        app_=app;thread_=std::this_thread::get_id();
        applicationRootGeneration_=app_->application10()->sessionGeneration;
        sourceManager_=&manager;managerKey_=manager.objectKey;generation_=manager.sessionGeneration;
        allocation_.reset(new(std::nothrow) Allocation);
        if(!allocation_)return result_={Status::allocation,0x1adfd4};
        auto& a=*allocation_;auto& e=a.entity;
        if(!authority.canEnroll(e,key(e)))return result_={Status::entity,0x141300};
        // 1ADFE8->1A02CC->20F3C0->27E13C. One actual App counter.
        const auto base=initializeOriginalWreckComponent50LA(e,OriginalWreckComponentKind50LA::racer27E130,
            generation_,key(e),true,app_->objectCount423F60());
        counted_=e.reference00!=0;
        if(base.status!=OriginalWreckComponentStatus50LA::complete)return result_={Status::entity,base.sourcePC};
        if(!authority.enroll(e))return result_={Status::entity,0x1413c0};
        enrolled_=true;
        // 27E154 is SH, not SW; no invention of neighbouring bytes.
        a.byte(0x30,0);a.byte(0x31,0);a.byte(0x32,0xff);
        a.byte(0x33,0,7);a.word(8,0x3ccee0);
        const auto type=app_->types383408().searchableGameObject27E1BC();
        if(!type.okay())return result_={Status::typeRegistry,type.sourcePC};
        a.byte(0x32,type.type);a.byte(0x33,1,7);
        a.word(8,0x3d1790); //20F3D4, still Racer base, NOT Player3CED50.
        constexpr std::size_t first[]={0x210,0x220,0x270,0x2b0,0x2c0,0x2d0,0x2e0,0x2f0,0x300,0x310,0x320};
        for(auto at:first)a.vf0(at);
        // 2E8860 invokes1048E0 twice, embedded stride16, no cookie/heap.
        for(unsigned i=0;i<2;++i)a.vf0(0x7b0+16*i);
        constexpr std::size_t second[]={0x870,0x880,0x890,0x8c0,0x8d0,0x8e0,0x910,0x920,0x930,
            0x960,0x970,0x980,0x9b0,0x9c0,0x9d0,0xaf0,0xba0,0xbb0,0xbc0,0xbd0,0xbe0,0xbf0,
            0xc10,0xc20,0xc40,0xc50,0xc70,0xca0,0xce0};
        for(auto at:second)a.vf0(at);
        // 2104C0 writes ONLY first16 bytes in each32-byte embedded record.
        for(unsigned i=0;i<20;++i)a.vf0(0xd50+32*i);
        a.word(0xd2c,0);a.word(0xd30,0xffffffff);
        a.word(0xd40,0);a.word(0xd44,0);a.word(0xd48,0);a.word(0x38,1);
        a.byte(0xb9d,0,1);a.word(0x3c,0);a.byte(0xb9c,0,0x80);a.byte(0xb9c,0,0x10);
        // 217940: known NULL D40 skips child dispatch.3D1790+A8 is
        // 173CB0 jr ra;nop (verified ELF word), with no native virtual call.
        a.word(0x68,0);a.word(0x64,0);
        // 217990/217994 are unconditional LWC1/SWC1, with no arithmetic,
        // comparison or dereference of the unknown value. Preserve the raw
        // allocation-epoch value symbolically; the later2103A8 zero cannot
        // retroactively resolve this copy. No host-uninitialized read occurs.
        a.copyUnknownCF8To78(); //217940 then returns;20F390 has NOT returned.
        // 20F6A4..20F6D4, independent of the unknown78 payload.
        for(std::size_t at=0x40;at<=0x54;at+=4)a.word(at,0);
        a.word(0x58,0xffffffff);a.word(0x5c,0xffffffff);
        // 309230 calls at20F6E4/20F6F8 copy the actual NUL string3B14B8.
        // These destinations end in C, so the source takes byte-copy path
        // 309320 and writes exactly one byte, not a zeroed entire buffer.
        a.byte(0x8c,0);a.byte(0x18c,0);
        a.word(0x190,0);a.word(0x194,0);a.word(0x240,0);
        for(std::size_t at=0x1a0;at<=0x20c;at+=4)a.word(at,0);
        a.word(0x244,0xffffffff);a.word(0x26c,0xffffffff);
        constexpr std::size_t zeros[]={0x248,0x230,0x238,0x234,0x23c,0x280,0x288,0x284,
            0x28c,0x290,0x294,0x298,0x29c,0x2a0,0x248,0x24c,0x250,0x254,0x264,0x268};
        for(auto at:zeros)a.word(at,0);
        a.word(0x25c,0x40100000);a.word(0x258,1);
        //20F950 and following loads borrow the SAME App-owned434630,
        //not an equal-valued vector. Actual startup table3CC548->3C9F70,
        //SQ3C9FDC produces this owner. app_ retains its entire lifetime.
        const auto source434630=[&a,this](std::size_t at)noexcept {
            for(unsigned lane=0;lane<4;++lane)
                a.word(at+4*lane,app_->detectorFrame434630_[lane]);
        };
        constexpr std::size_t vectors[]={0x2b0,0x2c0,0x2d0,0x2e0,0x300,0x310,0x320};
        for(auto at:vectors)source434630(at);
        constexpr std::size_t zeroBefore378[]={0x330,0x334,0x338,0x33c,0x690,0x694,
            0x340,0x344,0x348,0x350,0x370,0x374};
        for(auto at:zeroBefore378)a.word(at,0);
        a.word(0x378,0x4200cccd);a.word(0x37c,0x4200cccd);
        constexpr std::size_t zeroBefore7B0[]={0x380,0x384,0x488,0x58c,0x698,0x7a0,0x7a4};
        for(auto at:zeroBefore7B0)a.word(at,0);
        source434630(0x7b0);source434630(0x7c0);
        a.word(0x7a8,0);a.word(0x7ac,0);
        for(std::size_t at=0x7d0;at<=0x7e8;at+=4)a.word(at,0);
        a.word(0x7ec,0x41200000);
        for(std::size_t at=0x7f0;at<=0x834;at+=4)a.word(at,0);
        a.word(0x838,1);a.word(0x83c,0);a.word(0x840,1);a.word(0x844,1);
        a.word(0x848,0x3f800000);
        constexpr std::size_t zeroBefore4668[]={0x84c,0x864,0x8b0,0x900,0x950,0x9a0,0x85c};
        for(auto at:zeroBefore4668)a.word(at,0);
        const auto floatBits=[](float value)noexcept {
            std::uint32_t bits=0;std::memcpy(&bits,&value,4);return bits;
        };
        //20FBC8..20FC04 NEG.S on actual434660 XYZ: preserve negative zero.
        for(unsigned lane=0;lane<3;++lane)
            a.word(0x870+4*lane,floatBits(app_->cableForward434660_[lane])^0x80000000u);
        a.word(0x87c,0x3f800000);
        source434630(0x880);source434630(0x890);
        const auto quaternion382B50=[&a,this](std::size_t at)noexcept {
            for(unsigned lane=0;lane<4;++lane)a.word(at+4*lane,app_->racerQuaternion382B50_[lane]);
        };
        quaternion382B50(0x8a0);
        constexpr std::size_t responses[]={0x8c0,0x910,0x960,0x9b0};
        for(auto at:responses){source434630(at);source434630(at+16);source434630(at+32);quaternion382B50(at+48);}
        a.word(0x860,0);
        // Immediate source words, retaining exact bits rather than host float
        // arithmetic. Registers remain live from20F708..20F7FC until consumed.
        struct Store {std::size_t at;std::uint32_t value;};
        constexpr Store tuning[]={
            {0x9f0,0x41200000},{0x9f4,0xc1a00000},{0x9f8,0x42a00000},{0x9fc,0x42700000},
            {0xa00,0x42f00000},{0xa04,0xc2700000},{0xa08,0x41f00000},{0xa0c,0xc0a00000},
            {0xa10,0x41a00000},{0xa14,0x40000000},{0xa18,0x40e00000},{0xa1c,0x40000000},
            {0xa20,0x40000000},{0xa24,0xc1a00000},{0xa28,0x41f00000},{0xa2c,0xc0a00000},
            {0xa30,0x40a00000},{0xa68,0x4392aa7f},{0xa6c,0x42200000},{0xa70,0x42c80000},
            {0xa7c,0x42480000},{0xa74,0x41200000},{0xa78,0x41200000},{0xa80,0xc0a00000},
            {0xa84,0xc0a00000},{0xa88,0x3f800000},{0xa8c,0x3db2b8c1},{0xa90,0},
            {0xa94,0xc3960000},{0xa98,0},{0xa9c,0},{0xaa0,0},{0xaa4,0},{0xaa8,0},{0xaac,0},
            {0xa34,0x40a00000},{0xa40,0x41a00000},{0xa3c,0x40a00000},{0xa38,0x41a00000},
            {0xa44,0x3d4ccccd},{0xa48,0x3e4ccccd},{0xa4c,0x3d4ccccd},{0xa50,0x3e4ccccd},
            {0xa54,0x48a80dd5},{0xa58,0x43480000},{0xa5c,0x40e00000},{0xa64,0x3c23d70a},{0xa60,0x3c23d70a}};
        for(const auto& store:tuning)a.word(store.at,store.value);
        const auto sharedFloatVector=[&a,&floatBits](std::size_t at,const std::array<float,4>& v)noexcept {
            for(unsigned lane=0;lane<4;++lane)a.word(at+4*lane,floatBits(v[lane]));
        };
        sharedFloatVector(0xc20,app_->binderTriple434650_);
        sharedFloatVector(0xc10,app_->cableForward434660_);
        a.word(0xc08,0);a.word(0xc0c,0);a.word(0x850,0);a.word(0xab0,0);
        a.word(0xab4,0x3f800000);a.word(0xab8,0x3f490fd9);a.word(0xabc,0);a.word(0xac0,0);
        sharedFloatVector(0xaf0,app_->binderTriple434650_);
        for(std::size_t at=0xb00;at<=0xb20;at+=4)a.word(at,0);
        a.word(0xb58,0);
        for(std::size_t at=0xb24;at<=0xb38;at+=4)a.word(at,0);
        a.word(0xac4,0);a.word(0xac8,0);a.word(0xad0,1);a.word(0xacc,1);
        for(std::size_t at=0xad4;at<=0xae8;at+=4)a.word(at,0);
        for(std::size_t at=0xb5c;at<=0xb78;at+=4)a.word(at,0);
        //210194/1AC/1C4 preserve unknown remaining flags, never read them.
        a.byte(0xb9c,0,1);a.byte(0xb9c,0,2);a.byte(0xb9c,0,4);
        a.word(0xb7c,0);a.word(0xb80,0);a.word(0xb84,0);
        a.word(0xb88,0x3f800000);a.word(0xb8c,0x3f800000);a.word(0xb90,0x3f800000);
        a.word(0xb94,0x40800000);a.word(0xb98,0x40800000);
        a.byte(0xb9c,0,0x20);a.byte(0xb9c,0x40,0x40);
        source434630(0xbc0);sharedFloatVector(0xbd0,app_->cableForward434660_);
        a.word(0xc30,0);a.word(0xc34,0);
        source434630(0xba0);source434630(0xbb0);source434630(0xbe0);source434630(0xbf0);
        for(std::size_t at=0xc00;at<=0xc06;++at)a.byte(at,0); //C07 untouched.
        a.word(0xc80,1);a.word(0xc84,0);
        a.word(0xc88,1);a.word(0xc8c,0); //2102FC/210308 read those exact prior writes.
        a.word(0xc38,0);source434630(0xc40);a.word(0xcc0,0);a.word(0xcb4,0);
        for(std::size_t at=0x64;at<=0x70;at+=4)a.word(at,0);
        a.word(0x74,0x3dcccccd);a.byte(0xb9c,0,8);
        a.word(0x7c,0);a.word(0x84,0x33d6bf95);a.word(0x80,0x3ba3d70a);a.word(0x88,0x3e2e147b);
        a.word(0xcf0,0);a.word(0xcf4,0);
        a.word(0xcf8,0); //2103A8: DOES NOT change epoch-captured unknown78.
        a.word(0xcfc,0);a.word(0xd00,0);a.word(0xd04,0xffffffff);a.word(0xd28,0);
        constexpr std::size_t tailZeros[]={0xd0c,0xd08,0xd18,0xd1c,0xd10,0xd14,0xd20,0xd24,0xd34};
        for(auto at:tailZeros)a.word(at,0);
        a.byte(0x33,2,2); //210430; upper five bits remain unknown.
        //210434..210464 replaces only first16 and next4 of each32-byte
        //record. The final12 bytes in each record remain unknown.
        for(unsigned i=0;i<20;++i){source434630(0xd50+32*i);a.word(0xd60+32*i,0);}
        a.word(0xfd0,0);a.word(0xfd4,19);
        //Actual20F390 epilogue210480..2104B0 returns this SAME object.
        //1A02E0 installs Player dispatch only AFTER that base return.
        a.word(8,0x3ced50);
        //1A02E8->2A0F30 on embedded1010; no external calls or ownership.
        //The five initialVF0 stores are repeated by explicit stack copies
        //of the same lanes; preserve that instruction-order history here.
        for(unsigned i=0;i<5;++i)a.vf0(0x1010+16*i);
        for(unsigned i=0;i<5;++i)a.vf0(0x1010+16*i);
        a.word(0x1060,0x43960000); //+50=300.0f, exact source immediate.
        a.word(0x1064,0);a.word(0x1068,0);a.word(0x106c,0);a.word(0x1070,0);
        a.byte(0x1089,0,1);a.word(0x1074,0x3f800000);
        a.byte(0x1089,0,2);a.byte(0x1089,0,4);a.byte(0x1089,0,8);
        a.byte(0x1089,0,0x10);a.byte(0x1089,0,0x40);a.byte(0x1089,0,0x80);
        a.byte(0x1089,0,0x20); //All8 flags known only after2A10C4.
        a.word(0x1078,0);a.byte(0x1088,0);
        a.word(0x107c,0);a.word(0x1080,0);a.word(0x1084,0);
        //108A/B and108C..108F have no represented helper writes.
        //2A0F30 returns, then1A0304..1A047C derived stores.
        a.word(0x58,0);
        for(std::size_t at=0xfe0;at<=0xfec;at+=4)a.word(at,0);
        a.byte(0x10cc,0,1);a.byte(0x10cc,0,2);
        a.word(0xff8,0);a.word(0x1094,0);a.word(0x1090,0xbf800000);a.word(0xff4,0);
        //1A0380 pair loop writes eight words, then1A03BC writes ninth.
        for(unsigned i=0;i<8;i+=2){a.word(0x10a8+4*i,0xffffffff);a.word(0x10ac+4*i,0xffffffff);}
        a.word(0x10c8,0xffffffff);
        a.word(0x109c,0);a.byte(0x10cc,0,4);a.byte(0x10cc,0,0x10);
        a.word(0x1004,0);a.word(0x10a0,0);
        a.byte(0x10cd,0,0xf0);a.byte(0x10cd,0,0x0f);a.byte(0x10cc,0,8);
        //1A0488 returns the constructed Player with captured78 still unknown.
        //This conditional PRIVATE constructor does not execute1ADF80, obtain
        //Profile/human selection, issue selected-ID/POD or retrofit Sessions.
        return result_={Status::playerConstructorReturned,0x1a0488};
    }
    template<class Application> bool beginPlayerAxes1A0570(const Application& actual,
        std::uint32_t human,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=loadAttempted_?loadPC_:0x1a0570;
        if(loadAttempted_||!allocation_||result_.status!=Status::playerConstructorReturned||
           thread_!=std::this_thread::get_id()||actual.entities()!=app_||human>1||!actual.enabled)return false;
        // Native admission checks do not substitute for the preceding genuine
        // manager/World call, enforced by the private owning manager issuer.
        const auto input=actual.sourceInput04_;
        if(!input||!actual.sourceInput04NativeBorrowCurrent(input.get())||
           input->application_!=&actual||input->thread_!=thread_||input->nativeClosed_||
           !input->startupReturnedWithNativeTransport_||!input->allocation_||
           input->playerBorrows_==std::numeric_limits<std::size_t>::max())return false;
        auto& manager=*input->allocation_;
        if(!manager.slotsKnown[human])return false;
        auto* controller=manager.slots10[human];
        if(controller)for(unsigned selector: {0u,1u,2u,5u})
            if(controller->mappingCounts[selector]!=0)return false; // Not a repeated/widened load.
        loadAttempted_=true;sourceInput_=input;++input->playerBorrows_;inputBorrowed_=true;
        auto& a=*allocation_;a.word(0xffc,human); //1A05A0.
        logicalController_=controller;for(unsigned i=0;i<4;++i)a.known[0xff0+i]=0;
        if(!controller){pc=loadPC_=0x1a05e8;return false;} // Before diagnostic, not fake return.
        static constexpr unsigned selectors[4]={0,1,2,5};
        static constexpr std::uint32_t allocations[4]={0x1a05f8,0x1a065c,0x1a06b8,0x1a071c};
        for(unsigned i=0;i<4;++i){
            pc=loadPC_=allocations[i];sourceAxes_[i].reset(new(std::nothrow) Input::Axis);
            if(sourceAxes_[i]){auto& axis=*sourceAxes_[i];auto& x=axis.source;axis.selector=selectors[i];
                const auto id=Input::axisCounter3E08E0_.fetch_add(1,std::memory_order_relaxed)+1u;
                //127C50 base constructor, then127BB0 parameters and arithmetic.
                for(unsigned off=0;off<0x3c;off+=4)x.word(off,0);
                x.word(8,0xffffffffu);x.word(12,0xffffffffu);x.word(16,0xffffffffu);
                x.word(0x2c,0xbf800000);x.word(0x30,0x3f800000);
                x.word(0x24,0);x.word(0x28,0);x.word(0x34,0x3e4ccccd);x.word(0x38,0xbe4ccccd);
                x.word(4,id);x.word(8,selectors[i]);
            }
            //Each allocation/constructor result is published, including NULL,
            //before the next allocation. Guest pointer bits remain unknown.
            for(unsigned b=0;b<4;++b)a.known[0xfe0+4*i+b]=0;
        }
        static constexpr std::uint32_t registration[4]={0x1a078c,0x1a07a8,0x1a07c4,0x1a07e0};
        static constexpr std::uint32_t negative[6]={0xfffffffeu,0xfffffffbu,0xfffffff9u,0xfffffff7u,0xfffffff5u,0xfffffff3u};
        static constexpr std::uint32_t positive[6]={0xfffffffdu,0xfffffffcu,0xfffffffau,0xfffffff8u,0xfffffff6u,0xfffffff4u};
        for(unsigned i=0;i<4;++i){pc=loadPC_=registration[i];
            auto* axis=sourceAxes_[i].get();if(!axis){pc=loadPC_=0x14370c;return false;}
            auto& x=axis->source;axis->controller00=controller;
            for(unsigned b=0;b<4;++b)x.known[b]=0;
            x.word(0x1c,0x3c23d70a);x.word(0x20,0x3c23d70a);
            //144090's two process-once assertions have known passing inputs:
            //constructor bit0=1 and this non-null axis. No diagnostic is faked.
            if(!Input::guard8390_.load(std::memory_order_relaxed)){
                Input::assertion426EC0_.store(1,std::memory_order_relaxed);Input::guard8390_.store(1,std::memory_order_relaxed);}
            if(!Input::guard8394_.load(std::memory_order_relaxed)){
                Input::assertion426EC8_.store(1,std::memory_order_relaxed);Input::guard8394_.store(1,std::memory_order_relaxed);}
            const unsigned selector=axis->selector;
            //Actual fresh logical controller count0/capacity1 means NO growth
            //1442C0 occurs. Initial malloc4 failure remains NULL; stop before
            //the actual offending append, never pretend143700 returned false.
            if(!controller->mappingTables[selector]){pc=loadPC_=0x1441c0;return false;}
            controller->mappingTables[selector][0]=axis;controller->mappingCounts[selector]=1;
            controller->source.word(0x63c+selector*0x14,1);axis->registered=true;
            x.word(0x0c,negative[selector]);x.word(0x10,positive[selector]);
        }
        pc=loadPC_=0x1a07e8;sourceFF4_.reset(new(std::nothrow) Input::Bytes<0x14>);
        if(sourceFF4_){sourceFF4_->word(0,0);sourceFF4_->word(4,0x3f800000);
            sourceFF4_->word(12,0x7f7fffff);sourceFF4_->word(16,0xff7fffff);sourceFF4_->word(8,0x3f800000);}
        for(unsigned i=0;i<4;++i)a.known[0xff4+i]=0;
        pc=loadPC_=0x1a0848;return false; //Before genuine base resource load210770.
    }
    template<class Application> bool continueBaseLoad210770(const Application& actual,
        const std::shared_ptr<const OriginalSourceWorldRowsBorrow50LA>& world,
        std::uint32_t pod,std::uint32_t human,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=loadPC_;
        if(baseLoadAttempted_||loadPC_!=0x1a0848||!loadAttempted_||!allocation_||!world||pod>31||human>1||
           thread_!=std::this_thread::get_id()||actual.entities()!=app_||!actual.enabled||
           !sourceInput_||!actual.sourceInput04NativeBorrowCurrent(sourceInput_.get()))return false;
        const void *terrain=nullptr,*water=nullptr;
        const OriginalRaceConstructor50LA* race=nullptr;
        if(!world->read2107FC(actual,terrain,water,race))return false;
        auto& authority=app_->entityAuthority_;
        if(app_->attachmentLock423F58()||!authority.canRetire(allocation_->entity)||!authority.reserveAdditional(1))return false;
        baseLoadAttempted_=true;auto& a=*allocation_;
        //141C20 is the exact identity-return accessor: no second Racer object.
        pc=loadPC_=0x2107c4;source7F8_.reset(new(std::nothrow) C);
        if(source7F8_){
            if(!authority.canEnroll(*source7F8_,key(*source7F8_))){pc=loadPC_=0x141300;return false;}
            const auto initialized=initializeOriginalWreckComponent50LA(*source7F8_,
                OriginalWreckComponentKind50LA::entity141300,generation_,key(*source7F8_),true,app_->objectCount423F60());
            source7F8Counted_=source7F8_->reference00!=0;
            if(initialized.status!=OriginalWreckComponentStatus50LA::complete){pc=loadPC_=0x141300;return false;}
            if(!authority.enroll(*source7F8_)){pc=loadPC_=0x1413c0;return false;}source7F8Enrolled_=true;
        }
        //2107E0 publishes the nullable counted allocation before scalar stores.
        for(unsigned i=0;i<4;++i)a.known[0x7f8+i]=0;
        a.word(0x3c,0);a.word(0x5c,human); //Exact1AE0D8/1AE0DC arguments.
        pc=loadPC_=0x2107ec;
        if(!world->read2107FC(actual,terrain,water,race))return false;
        sourceTerrain330_=terrain;sourceWater334_=water;
        sourceWorld338_=world->world_.get();
        for(unsigned i=0;i<4;++i){a.known[0x330+i]=0;a.known[0x334+i]=0;a.known[0x338+i]=0;}
        pc=loadPC_=0x210810;
        sourceMusic33C_=actual.borrowSourcePlayerMusic210810(race);
        if(!sourceMusic33C_)return false; //Unknown/absent10C is NOT sourceNULL.
        for(unsigned i=0;i<4;++i)a.known[0x33c+i]=0;
        a.word(0x60,pod);
        //308C90 exact constant formats: Pod%02d, then %s%s\ with Pods\.
        sourceLoadPodName_={{'P','o','d',char('0'+pod/10),char('0'+pod%10),0}};
        sourceLoadPodDirectory_={{'P','o','d','s','\\','P','o','d',char('0'+pod/10),char('0'+pod%10),'\\',0}};
        pc=loadPC_=0x210848;return false; //Before nullable0x80 child199840/load199950.
    }
    template<class Application> bool continueOverlay210848(const Application& actual,
        const std::shared_ptr<const OriginalSourceWorldRowsBorrow50LA>& world,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=loadPC_;
        if(overlayAttempted_||loadPC_!=0x210848||!baseLoadAttempted_||!allocation_||!world||
           !sourceMusic33C_||thread_!=std::this_thread::get_id()||actual.entities()!=app_||
           !sourceInput_||!actual.sourceInput04NativeBorrowCurrent(sourceInput_.get()))return false;
        const void *terrain=nullptr,*water=nullptr;const OriginalRaceConstructor50LA* race=nullptr;
        if(!world->read2107FC(actual,terrain,water,race)||race!=sourceMusic33C_->race_.get())return false;
        auto& authority=app_->entityAuthority_;
        if(app_->attachmentLock423F58()||!authority.canRetire(allocation_->entity)||!authority.reserveAdditional(1))return false;
        overlayAttempted_=true;sourceOverlay240_.reset(new(std::nothrow) OriginalPodAIDamageOverlay50LA);
        if(sourceOverlay240_){auto& overlay=*sourceOverlay240_;
            if(!authority.canEnroll(overlay.component,key(overlay.component))){pc=loadPC_=0x199840;return false;}
            const bool returned=overlay.construct199840(generation_,key(overlay.component),app_->objectCount423F60());
            overlayCounted_=overlay.component.reference00!=0;
            if(!returned){pc=loadPC_=0x199840;return false;}
            if(!authority.enroll(overlay.component)){pc=loadPC_=0x1413c0;return false;}overlayEnrolled_=true;
        }
        //210864 publication occurs even NULL, before actual199950 call.
        for(unsigned b=0;b<4;++b)allocation_->known[0x240+b]=0;
        if(!sourceOverlay240_){pc=loadPC_=0x199978;return false;} //Before original NULL store.
        if(!sourceOverlay240_->bindSource199950(allocation_->entity,*race))return false;
        std::int32_t mode=0;pc=loadPC_=0x199980;
        if(!actual.readSourcePlayerRaceMode199980(race,mode))return false;
        sourceOverlay240_->mode40=mode;sourceOverlay240_->sourceMode40Known_=true;
        pc=loadPC_=0x199990;return false;
        //Before first0x50 primitive allocation/setup2E1AE0. Actual same-source
        //renderer/drain custody is NOT established by Video material context.
    }
    template<class Application> bool continueOverlayArrays199990(const Application& actual,
        const std::shared_ptr<const OriginalSourceWorldRowsBorrow50LA>& world,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=loadPC_;
        if(loadPC_!=0x199990||!sourceOverlay240_||!sourceOverlay240_->sourceMode40Known_||
           !world||!sourceMusic33C_||thread_!=std::this_thread::get_id()||actual.entities()!=app_||
           !sourceInput_||!actual.sourceInput04NativeBorrowCurrent(sourceInput_.get()))return false;
        const void *terrain=nullptr,*water=nullptr;const OriginalRaceConstructor50LA* race=nullptr;
        if(!world->read2107FC(actual,terrain,water,race)||race!=sourceMusic33C_->race_.get()||
           sourceOverlay240_->sourceRace3C_!=race)return false;
        if(!sourceOverlayRenderer_)sourceOverlayRenderer_=actual.borrowSourcePlayerOverlayRenderer199990(race);
        if(!sourceOverlayRenderer_)return false;
        const bool returned=sourceOverlayRenderer_->constructArrays(*sourceOverlay240_,pc);
        loadPC_=pc;if(!returned)return false;
        pc=loadPC_=0x210880;return false; //199950 returned; before141960 attachment.
    }
    template<class Application> bool continuePointerArrays210880(const Application& actual,
        const std::shared_ptr<const OriginalSourceWorldRowsBorrow50LA>& world,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=loadPC_;
        if(loadPC_!=0x210880||pointerArraysAttempted_||!sourceOverlay240_||!sourceOverlayRenderer_||
           !sourceOverlayRenderer_->current(true)||!sourceOverlay240_->prepared_||!world||!sourceMusic33C_||
           thread_!=std::this_thread::get_id()||actual.entities()!=app_||!allocation_||
           !sourceInput_||!actual.sourceInput04NativeBorrowCurrent(sourceInput_.get()))return false;
        const void *terrain=nullptr,*water=nullptr;const OriginalRaceConstructor50LA* race=nullptr;
        if(!world->read2107FC(actual,terrain,water,race)||race!=sourceMusic33C_->race_.get()||
           terrain!=sourceTerrain330_||water!=sourceWater334_)return false;
        auto& authority=app_->entityAuthority_;auto& parent=allocation_->entity;
        auto& child=sourceOverlay240_->component;
        if(app_->attachmentLock423F58()||parent.firstChild18||parent.descendants24||
           !authority.canRetire(parent)||!authority.canRetire(child)||child.parent1C||child.mask20)return false;
        pc=loadPC_=0x141960;
        const auto attached=authority.attachChildren(parent,child,0xcu,app_->attachmentLock423F58());
        if(attached.status!=OriginalWreckComponentStatus50LA::complete){pc=loadPC_=attached.sourcePC;return false;}
        overlayAttached_=true;overlayMask12Written_=true;
        //a2=12 is mask20, NOT a category28 write. Reference00 is unchanged.
        pointerArraysAttempted_=true;
        constexpr std::size_t offsets[]={0x800,0x804,0x808,0x81c,0x80c,0x818};
        constexpr std::uint32_t counts[]={4,4,2,2,4,10};
        constexpr std::uint32_t faultPC[]={0x2108a4,0x2108d0,0x2108f8,0x210920,0x210944,0x21096c};
        for(unsigned i=0;i<6;++i){
            if(i==3&&!water)continue; //Actual retained World334, not a NONE receipt.
            auto& array=sourcePointerArrays_[i];array.extent=counts[i];
            array.values.reset(new(std::nothrow) const void*[array.extent]);
            array.published=true;
            for(unsigned b=0;b<4;++b)allocation_->known[offsets[i]+b]=0;
            pc=loadPC_=faultPC[i];if(!array.values)return false; //PublishedNULL, before first SW.
            //The tenth818 zero is2109B8, delay slot of the unresolved World4A0
            //branch. Preserve it UNKNOWN until that authentic argument joins.
            const auto stores=i==5?9u:array.extent;
            while(array.initialized<stores)array.values[array.initialized++]=nullptr;
        }
        pc=loadPC_=0x2109b4;return false;
        //Need captured actual World4A0 from1AA57C; resetNULL or prewarmed
        //nativeParticleManager4A0 cannot select this branch. No7FC publication.
    }
    bool cancelPointerArraysNative()noexcept {
        if(!pointerArraysAttempted_)return true;
        if(!app_||thread_!=std::this_thread::get_id())return false;
        for(const auto& array:sourcePointerArrays_){
            if(array.initialized>array.extent||(!array.values&&array.initialized))return false;
            for(std::uint32_t i=0;i<array.initialized;++i)if(array.values[i])return false;
            //Unwritten slots (notably818[9]) are not read or released as objects.
        }
        //Source210538..2105AC array-storage release order, with no claim that
        //the remaining full2104D0 destructor has run on this partial Player.
        constexpr unsigned order[]={0,1,2,4,5,3};
        for(auto i:order){auto& array=sourcePointerArrays_[i];array.values.reset();array.initialized=0;}
        return true;
    }
    bool cancelOverlayNative()noexcept {
        if(!cancelPointerArraysNative())return false;
        if(sourceOverlay240_){auto& overlay=*sourceOverlay240_;auto& e=overlay.component;
            if(overlayAttached_){
                if(!app_||!allocation_||thread_!=std::this_thread::get_id()||app_->attachmentLock423F58()||
                   e.parent1C!=&allocation_->entity||e.previous10||e.next14||e.firstChild18||
                   allocation_->entity.firstChild18!=&e||e.mask20!=0xcu||e.descendants24)return false;
                const auto detached=app_->entityAuthority_.detach(e,app_->attachmentLock423F58());
                if(detached.status!=OriginalWreckComponentStatus50LA::complete)return false;
                overlayAttached_=false;
            }
            // Real ordered array cleanup precedes counted Entity retirement.
            // Failure retains both the partial arrays and exact renderer pin.
            if(overlay.sourceArraysAttempted_&&(!sourceOverlayRenderer_||
               !sourceOverlayRenderer_->cancelArrays(overlay)))return false;
            if(overlay.array30||overlay.array34||overlay.attempted_)return false;
            if(overlayCounted_){
                if(!app_||thread_!=std::this_thread::get_id()||app_->attachmentLock423F58()||
                   !app_->objectCount423F60()||e.reference00!=1||e.sessionGeneration!=generation_||e.objectKey!=key(e)||
                   e.kind!=OriginalWreckComponentKind50LA::overlay199840||e.parent1C||e.previous10||e.next14||e.firstChild18||
                   e.mask20!=(overlayMask12Written_?0xcu:0u)||e.descendants24||e.category28!=0xff||(e.flags29&15)!=3||e.nativeFlags29)return false;
                if(overlayEnrolled_&&!app_->entityAuthority_.canRetire(e))return false;
                if(overlayEnrolled_&&!app_->entityAuthority_.retire(e))return false;
                overlayEnrolled_=false;--e.reference00;--app_->objectCount423F60();overlayCounted_=false;
            }
            overlay.racer38=nullptr;overlay.sourceRace3C_=nullptr;sourceOverlay240_.reset();
            sourceOverlayRenderer_.reset();
        }
        return true; //Native unlinked prefix cleanup, not1998A0/2104D0.
    }
    bool cancelBaseLoadNative()noexcept {
        if(!cancelOverlayNative())return false;
        if(source7F8Counted_){
            if(!app_||!source7F8_||thread_!=std::this_thread::get_id()||app_->attachmentLock423F58()||
               !app_->objectCount423F60()||source7F8_->reference00!=1||source7F8_->sessionGeneration!=generation_||
               source7F8_->objectKey!=key(*source7F8_)||source7F8_->kind!=OriginalWreckComponentKind50LA::entity141300||
               source7F8_->parent1C||source7F8_->previous10||source7F8_->next14||source7F8_->firstChild18||
               source7F8_->descendants24||source7F8_->mask20||source7F8_->category28!=0xff||
               (source7F8_->flags29&15)!=3||source7F8_->nativeFlags29)return false;
            if(source7F8Enrolled_&&!app_->entityAuthority_.canRetire(*source7F8_))return false;
            if(source7F8Enrolled_&&!app_->entityAuthority_.retire(*source7F8_))return false;
            source7F8Enrolled_=false;--source7F8_->reference00;--app_->objectCount423F60();source7F8Counted_=false;
        }
        source7F8_.reset();sourceMusic33C_.reset();sourceTerrain330_=sourceWater334_=sourceWorld338_=nullptr;
        return true; //Native custody withdrawal, not2104D0 source teardown.
    }
    bool cancelPlayerAxesNative()noexcept {
        if(!inputBorrowed_)return !sourceInput_;
        if(!sourceInput_||sourceInput_->thread_!=std::this_thread::get_id()||!sourceInput_->playerBorrows_)return false;
        //Backing survives App revocation through this private shared owner.
        //Remove only THIS Player's native registrations before freeing axes;
        //not a claim of143700 inverse/source Player destructor execution.
        for(const auto& axis:sourceAxes_)if(axis&&axis->registered){
            const auto k=axis->selector;
            if(!logicalController_||!logicalController_->mappingTables[k]||
               logicalController_->mappingCounts[k]!=1||logicalController_->mappingTables[k][0]!=axis.get())return false;}
        for(auto& axis:sourceAxes_)if(axis&&axis->registered){
            const auto k=axis->selector;logicalController_->mappingTables[k][0]=nullptr;
            logicalController_->mappingCounts[k]=0;logicalController_->source.word(0x63c+k*0x14,0);axis->registered=false;}
        sourceFF4_.reset();for(auto& axis:sourceAxes_)axis.reset();logicalController_=nullptr;
        --sourceInput_->playerBorrows_;if(sourceInput_->nativeClosed_&&!sourceInput_->playerBorrows_)sourceInput_->allocation_.reset();
        inputBorrowed_=false;sourceInput_.reset();return true;
    }
    bool cancelNativePartial()noexcept {
        if(!cancelBaseLoadNative())return false;
        if(!cancelPlayerAxesNative())return false;
        if(!allocation_){sourceManager_=nullptr;app_.reset();return true;}
        if(!app_||std::this_thread::get_id()!=thread_)return false;
        auto& e=allocation_->entity;
        if(counted_){
            if(!app_->application10()||app_->application10()->sessionGeneration!=applicationRootGeneration_||
               !sourceManager_||sourceManager_->sessionGeneration!=generation_||sourceManager_->objectKey!=managerKey_||
               sourceManager_->kind!=OriginalWreckComponentKind50LA::entity141300||sourceManager_->reference00!=1||
               !app_->entityAuthority_.canRetire(*sourceManager_)||
               app_->attachmentLock423F58()||e.reference00!=1||e.sessionGeneration!=generation_||
               e.objectKey!=key(e)||e.kind!=OriginalWreckComponentKind50LA::racer27E130||
               e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24||
               e.category28!=0xff||(e.flags29&15)!=3||e.nativeFlags29||e.nativeEffectControlId||
               !app_->objectCount423F60())return false;
            if(enrolled_&&!app_->entityAuthority_.canRetire(e))return false;
            if(enrolled_&&!app_->entityAuthority_.retire(e))return false;
            enrolled_=false;
            // Native cancellation of ONE unlinked counted base. Not2104D0:
            // that destructor has no joined full derived teardown in this owner.
            --e.reference00;--app_->objectCount423F60();counted_=false;
        }
        allocation_.reset();sourceManager_=nullptr;app_.reset();return true;
    }
public:
    OriginalPlayerConstructorPrefix50LA(const OriginalPlayerConstructorPrefix50LA&)=delete;
    OriginalPlayerConstructorPrefix50LA& operator=(const OriginalPlayerConstructorPrefix50LA&)=delete;
    ~OriginalPlayerConstructorPrefix50LA(){if(!cancelNativePartial())std::terminate();}
};
} //namespace rr::game
