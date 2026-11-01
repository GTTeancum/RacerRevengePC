#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <thread>
namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::assets {
class OriginalParticleSourceConfig50LA;
// Closed native source domain, not a mirror of an arbitrary retained retail
// process. Every newly admitted alias/writer must join this owner or poison its
// knownness before execution. Neither Config nor Application construction resets it.
class OriginalConfigProcessGlobals50LA final {
    friend class OriginalParticleSourceConfig50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalStandardTextureLifetime50LA;
    struct State {
        const std::thread::id thread=std::this_thread::get_id();
        // Startup100008..100030 clears4338B0, then ctor3C9010 table3CC4D0
        // writes(0,0,0,1) at3C906C. This owner exposes only its XYZ lanes.
        std::array<std::uint32_t,3> zero4338B0{{0,0,0}};
        bool vectorKnown=true,reentrancyKnown=true,cursorKnown=true;
        //Startup BSS, followed only by represented lazy15AEF4..15AF00.
        std::uint8_t guard3D5DF8=0;
        std::uint32_t assertion4338F0=0;
        bool groupAssertionKnown=true;
        //Startup BSS; lazy type0 guards15C380..15CC00. Same process, not per Config.
        std::array<std::uint8_t,14> type0Guard3D5DFC{};
        std::array<std::uint32_t,14> type0Assertion4338F8{};
        bool type0AssertionsKnown=true;
        std::uint8_t groupLookupGuard3D5E4C=0;
        std::uint32_t groupLookupAssertion4339E0=0;
        bool groupLookupKnown=true;
        //Startup BSS; actual14D050/14D06C and14D068/14D074 are the joined writers/readers.
        std::uint8_t textureListGuard3D5D78=0;
        std::uint32_t textureListAssertion42F9A8=0;
        bool textureListKnown=true;
        // Startup BSS, shared actual24F2EC/24F320/24F324/24F32C occurrence.
        std::uint8_t textureLoadGuard3D62F8=0;
        std::uint32_t textureLoadAssertion4359A8=0;
        bool textureLoadKnown=true;
        std::uint8_t embeddedFCGuard3D63D4=0;
        std::uint32_t embeddedFCAssertion442CD0=0;
        bool embeddedFCKnown=true;
        // BSS-zero cumulative byte statistic, not live allocation ownership.
        // Joined writer24F7F0/808; other source writers24FDC4/FDE4 and
        //250804/810 must join this lane before admission, or poison it.
        std::uint32_t compressedBytes435940=0;
        std::uint64_t compressedBytesSequence=0;
        bool compressedBytesKnown=true;
        // Image38544C->385160; image3851BC (reent+5C) starts NULL.
        // Typed pointer custody avoids inventing EE address bits.
        const void* cursorOwner=nullptr;
        char* cursorBuffer=nullptr;
        std::size_t cursorCapacity=0;
        std::optional<std::size_t> cursor;
    };
    static State& state()noexcept {static State value;return value;}
    static bool reentrancyCurrentNative()noexcept {
        const auto& s=state();return s.thread==std::this_thread::get_id()&&s.reentrancyKnown;
    }
public:
    // Native allocator adaptation admission for actual2E8B00->305728. The
    // concrete caller must still prove its allocation and remove every borrow.
    // This is not an EE allocator/free-list execution receipt.
    static bool descriptorDelete2E8B00AdmittedNative()noexcept {return reentrancyCurrentNative();}
private:
    // Mandatory boundary for any future admitted writer/indirect alias not
    // implemented by this owner. Knownness is never restored by a new Config.
    static void poisonUnjoinedAliasesNative()noexcept {
        auto& s=state();s.vectorKnown=false;s.reentrancyKnown=false;s.cursorKnown=false;s.groupAssertionKnown=false;s.type0AssertionsKnown=false;s.groupLookupKnown=false;s.textureListKnown=false;s.textureLoadKnown=false;s.embeddedFCKnown=false;s.compressedBytesKnown=false;
    }
    static bool readCompressedBytes24F7F0(std::uint32_t& value)noexcept {
        const auto& s=state();if(s.thread!=std::this_thread::get_id()||!s.compressedBytesKnown)return false;
        value=s.compressedBytes435940;return true;
    }
    static bool addCompressedBytes24F808(std::uint32_t bytes,std::uint32_t before,std::uint32_t& after,std::uint64_t& sequence)noexcept {
        auto& s=state();if(s.thread!=std::this_thread::get_id()||!s.compressedBytesKnown||s.compressedBytes435940!=before||s.compressedBytesSequence==~std::uint64_t(0))return false;
        after=before+bytes; //Original ADDU wraps32 bits.
        s.compressedBytes435940=after;sequence=++s.compressedBytesSequence;return true;
    }
    static bool compressedBytesReceiptCurrentNative(std::uint32_t bytes,std::uint32_t before,std::uint32_t after,std::uint64_t sequence)noexcept {
        const auto& s=state();return s.thread==std::this_thread::get_id()&&s.compressedBytesKnown&&sequence&&
            sequence<=s.compressedBytesSequence&&after==before+bytes&&
            (sequence!=s.compressedBytesSequence||after==s.compressedBytes435940);
    }
    static bool checkEmbeddedFC257750(std::optional<bool> pointerIsNull,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x257760;
        if(s.thread!=std::this_thread::get_id()||!s.embeddedFCKnown||s.embeddedFCGuard3D63D4>1||
           s.embeddedFCAssertion442CD0>1||(!s.embeddedFCGuard3D63D4&&s.embeddedFCAssertion442CD0))return false;
        if(!s.embeddedFCGuard3D63D4){pc=0x257778;s.embeddedFCAssertion442CD0=1;pc=0x25777c;s.embeddedFCGuard3D63D4=1;}
        pc=0x257784;
        if(s.embeddedFCAssertion442CD0){pc=0x257790;if(!pointerIsNull)return false;pc=0x2577a4;
            if(!*pointerIsNull){pc=0x13366c;return false;}pc=0x1336a8;
        }
        pc=0x2577d8;return true;
    }
    static bool checkTextureLoad24F2C0(std::uint32_t format,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x24f2ec;
        if(s.thread!=std::this_thread::get_id()||!s.textureLoadKnown||s.textureLoadGuard3D62F8>1||
           s.textureLoadAssertion4359A8>1||(!s.textureLoadGuard3D62F8&&s.textureLoadAssertion4359A8))return false;
        if(!s.textureLoadGuard3D62F8){pc=0x24f320;s.textureLoadAssertion4359A8=1;pc=0x24f324;s.textureLoadGuard3D62F8=1;}
        pc=0x24f32c;
        if(s.textureLoadAssertion4359A8){pc=0x24f348;
            // Actual XORI1/SLTU predicate is format!=1. False requires
            // authentic debug diagnostic owner; no fabricated break/return.
            if(format==1){pc=0x13366c;return false;}pc=0x1336a8;
        }
        pc=0x24f378;return true;
    }
    static bool checkTextureList14D040(bool resourceNonNull,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x14d050;
        if(s.thread!=std::this_thread::get_id()||!s.textureListKnown||s.textureListGuard3D5D78>1||
           s.textureListAssertion42F9A8>1||(!s.textureListGuard3D5D78&&s.textureListAssertion42F9A8))return false;
        if(!s.textureListGuard3D5D78){pc=0x14d068;s.textureListAssertion42F9A8=1;pc=0x14d06c;s.textureListGuard3D5D78=1;}
        pc=0x14d074;
        if(s.textureListAssertion42F9A8){pc=0x14d08c;
            //14D090 delay slot passes actual resource S0, not list-owner+10.
            //True predicate returns0 independently of debug36C100.
            if(!resourceNonNull){pc=0x13366c;return false;}pc=0x1336a8;
        }
        pc=0x14d0c0;return true;
    }
    static bool type0AssertionsCurrentNative(std::size_t completed)noexcept {
        const auto& s=state();if(completed>14||s.thread!=std::this_thread::get_id()||!s.type0AssertionsKnown)return false;
        for(std::size_t i=0;i<completed;++i)
            if(s.type0Guard3D5DFC[i]!=1||s.type0Assertion4338F8[i]!=1)return false;
        return true;
    }
    static bool checkType0Scalar15C380(std::size_t index,std::uint32_t bits,std::uint32_t& pc)noexcept {
        struct Row {std::uint32_t read,store,guard,word,compare,call,next;};
        constexpr Row rows[]={
            {0x15c380,0x15c394,0x15c398,0x15c3a0,0x15c3ac,0x15c3f4,0x15c428},
            {0x15c440,0x15c454,0x15c458,0x15c460,0x15c46c,0x15c498,0x15c4c8},
            {0x15c4e0,0x15c4f4,0x15c4f8,0x15c500,0x15c50c,0x15c538,0x15c56c},
            {0x15c584,0x15c598,0x15c59c,0x15c5a4,0x15c5b0,0x15c5dc,0x15c610},
            {0x15c62c,0x15c640,0x15c644,0x15c64c,0x15c658,0x15c684,0x15c6b8},
            {0x15c6d0,0x15c6e4,0x15c6e8,0x15c6f0,0x15c6fc,0x15c728,0x15c75c},
            {0x15c778,0x15c78c,0x15c790,0x15c798,0x15c7a4,0x15c7d0,0x15c804},
            {0x15c81c,0x15c830,0x15c834,0x15c83c,0x15c848,0x15c874,0x15c8a4},
            {0x15c8bc,0x15c8d0,0x15c8d4,0x15c8dc,0x15c8e8,0x15c914,0x15c948},
            {0x15c960,0x15c974,0x15c978,0x15c980,0x15c98c,0x15c9b8,0x15c9ec},
            {0x15ca08,0x15ca1c,0x15ca20,0x15ca28,0x15ca34,0x15ca60,0x15ca90},
            {0x15caa8,0x15cabc,0x15cac0,0x15cac8,0x15cad4,0x15cb00,0x15cb30},
            {0x15cb48,0x15cb5c,0x15cb60,0x15cb68,0x15cb74,0x15cba0,0x15cbd0},
            {0x15cbe8,0x15cbfc,0x15cc00,0x15cc08,0x15cc14,0x15cc40,0x15cc70}};
        if(index>=14)return false;const auto& r=rows[index];auto& s=state();pc=r.read;
        if(s.thread!=std::this_thread::get_id()||!s.type0AssertionsKnown||
           s.type0Guard3D5DFC[index]>1||s.type0Assertion4338F8[index]>1||
           (!s.type0Guard3D5DFC[index]&&s.type0Assertion4338F8[index]))return false;
        if(!s.type0Guard3D5DFC[index]){pc=r.store;s.type0Assertion4338F8[index]=1;
            pc=r.guard;s.type0Guard3D5DFC[index]=1;}
        pc=r.word;
        if(s.type0Assertion4338F8[index]){
            pc=r.compare;const auto magnitude=bits&0x7fffffffu;
            //No host floating-point comparison. Only finite normals/zero:
            //EE denormal/NaN/exception behavior remains outside this subset.
            if(magnitude>=0x7f800000u||(magnitude&&magnitude<0x00800000u))return false;
            const bool valid=(!(bits&0x80000000u)||magnitude==0)&&(!index?magnitude<=0x3f800000u:true);
            pc=r.call;if(!valid){pc=0x13366c;return false;}pc=0x1336a8;
        }
        pc=r.next;return true;
    }
    static bool checkParticleGroup15AEE8(std::int32_t group,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x15AEE8u;
        if(s.thread!=std::this_thread::get_id()||!s.groupAssertionKnown||
           s.guard3D5DF8>1||s.assertion4338F0>1||(!s.guard3D5DF8&&s.assertion4338F0))return false;
        if(!s.guard3D5DF8){pc=0x15AEFCu;s.assertion4338F0=1;pc=0x15AF00u;s.guard3D5DF8=1;}
        pc=0x15AF08u;
        if(s.assertion4338F0){
            pc=0x15AF2Cu;
            //133660 returns0 for a true predicate for BOTH debug36C100
            //branches. False predicate needs that genuine diagnostic owner;
            //do not invent disabled assertions, output or a trap return.
            if(group<0){pc=0x13366Cu;return false;}
            pc=0x1336A8u;
        }
        pc=0x15AF60u;return true;
    }
    static bool groupLookup161EC0(std::int32_t index,std::int32_t count,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x161EC0u;
        if(s.thread!=std::this_thread::get_id()||!s.groupLookupKnown||s.groupLookupGuard3D5E4C>1||
           s.groupLookupAssertion4339E0>1||(!s.groupLookupGuard3D5E4C&&s.groupLookupAssertion4339E0))return false;
        if(!s.groupLookupGuard3D5E4C){pc=0x161ED4u;s.groupLookupAssertion4339E0=1;pc=0x161ED8u;s.groupLookupGuard3D5E4C=1;}
        pc=0x161EE0u;
        if(s.groupLookupAssertion4339E0){
            pc=0x161EFCu;
            //Actual predicate is signed index<count, NOT index>=0.
            if(!(index<count)){pc=0x13366Cu;return false;}
            pc=0x1336A8u; //True assertion's branch-independent return0.
        }
        pc=0x161F2Cu;return true;
    }
    static bool readDefault4338B0(std::array<std::uint32_t,3>& out)noexcept {
        const auto& s=state();if(s.thread!=std::this_thread::get_id()||!s.vectorKnown)return false;
        out=s.zero4338B0;return true;
    }
    static void expireStackNative(const void* owner)noexcept {
        auto& s=state();
        if(s.cursorOwner!=owner)return;
        // Do not invent a source NULL store when the source stack dies. An
        // outstanding pointer becomes unreadable; the next non-NULL strtok
        // argument can genuinely replace it without reading that old cursor.
        if(s.cursor)s.cursorKnown=false;
        s.cursorOwner=nullptr;s.cursorBuffer=nullptr;s.cursorCapacity=0;
    }
    static bool token30AB70(const void* owner,char* buffer,std::size_t capacity,
                           bool first,char delimiter,std::optional<std::size_t>& token,
                           std::uint32_t& pc)noexcept {
        auto& s=state();token.reset();pc=0x30AB78u;
        if(!owner||!buffer||!capacity||s.thread!=std::this_thread::get_id()||!s.reentrancyKnown)return false;
        std::size_t p=0;
        if(!first){
            pc=0x30ABA0u;
            if(!s.cursorKnown)return false;
            if(!s.cursor)return true;
            if(s.cursorOwner!=owner||s.cursorBuffer!=buffer||s.cursorCapacity!=capacity)return false;
            p=*s.cursor;
        }
        //30ABB4 skips all leading delimiters. Unlike29FFA0, empty comma
        //fields are skipped. Each read remains within the actual getter copy.
        for(;;){pc=0x30ABB4u;if(p>=capacity)return false;
            if(buffer[p]!=delimiter)break;++p;}
        if(!buffer[p]){
            pc=0x30ABE4u;s.cursor.reset();s.cursorKnown=true;
            s.cursorOwner=nullptr;s.cursorBuffer=nullptr;s.cursorCapacity=0;return true;
        }
        const auto start=p++;
        for(;;++p){pc=0x30ABF0u;if(p>=capacity)return false;
            if(buffer[p]!=delimiter&&buffer[p])continue;
            if(buffer[p]){pc=0x30AC14u;buffer[p]=0;s.cursor=p+1;}
            else s.cursor.reset();
            pc=0x30AC20u;s.cursorKnown=true;
            s.cursorOwner=s.cursor?owner:nullptr;s.cursorBuffer=s.cursor?buffer:nullptr;
            s.cursorCapacity=s.cursor?capacity:0;token=start;return true;
        }
    }
};
}
