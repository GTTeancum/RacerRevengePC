#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <thread>
namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::platform { class OriginalStreamLibrarySource50LA; }
namespace rr::game { class OriginalRaceMusicManager50LA; }
namespace rr::assets {
class OriginalParticleSourceConfig50LA;
class OriginalStreamLibraryFile50LA;
// Closed native source domain, not a mirror of an arbitrary retained retail
// process. Every newly admitted alias/writer must join this owner or poison its
// knownness before execution. Neither Config nor Application construction resets it.
class OriginalConfigProcessGlobals50LA final {
    friend class OriginalParticleSourceConfig50LA;
    friend class OriginalStreamLibraryFile50LA;
    friend class rr::platform::OriginalStreamLibrarySource50LA;
    friend class rr::game::OriginalRaceMusicManager50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalStandardTextureLifetime50LA;
    // Symbolic source-pointer custody, never fabricated numeric EE addresses.
    enum class StdioIdentity { null, reent385160, file385344, file38539C,
        file3853F4, cleanup3046F8, read308D90, write308DF8,
        seek308E78, close308EE0, locale3C6500, decimal3C6538 };
    struct StandardFileSelectedFields {
        // Only fields actually selected by304550, not a fabricated full FILE.
        StdioIdentity cursor=StdioIdentity::null; //+00
        std::uint32_t readCount=0,writeCount=0; //+04,+08
        std::uint16_t flags=0,descriptor=0; //+0C,+0E
        StdioIdentity buffer=StdioIdentity::null; //+10
        std::uint32_t lineBufferSize=0; //+18
        StdioIdentity cookie=StdioIdentity::null; //+1C
        std::array<StdioIdentity,4> callbacks{{StdioIdentity::null,
            StdioIdentity::null,StdioIdentity::null,StdioIdentity::null}}; //+20..2C
        StdioIdentity reent=StdioIdentity::null; //+54
    };
    class StreamLookupAccess {
        friend class rr::platform::OriginalStreamLibrarySource50LA;
        friend class OriginalConfigProcessGlobals50LA;
        StreamLookupAccess()noexcept {}
    };
    class WaterSprintfAccess;
    //Other existing friends cannot manufacture these keys: friendship is not
    //transitive. The Water bridge shares this SAME process State and helpers.
    class StreamSprintfAccess {
        friend class OriginalStreamLibraryFile50LA;
        friend class rr::platform::OriginalStreamLibrarySource50LA;
        friend class OriginalConfigProcessGlobals50LA;
        StreamSprintfAccess()noexcept {} //User-provided: not an aggregate passkey.
    };
    class WaterSprintfAccess {
        friend class ::ra_cold_aggregate50LA::Owner;
        friend class OriginalConfigProcessGlobals50LA;
        WaterSprintfAccess()noexcept {} //Only the concrete World water caller.
    };
    class MusicSprintfAccess {
        friend class rr::game::OriginalRaceMusicManager50LA;
        MusicSprintfAccess()noexcept {} //Only the actual retained Music manager.
    };
    struct State {
        //One process BSS guard3D6480/assertion442E80 for2684E0.
        bool streamIndexGuard8B10=false;std::uint32_t streamIndexAssertion442E80=0;
        bool streamIndexAssertionKnown=true,streamLookupAsciiKnown=true;
        //Loaded3C6391 ASCII table: bit0 marks exactly A..Z. Unknown writers
        //must poison this same lane; no host locale or per-lookup reset.

        const std::thread::id thread=std::this_thread::get_id();
        // Startup100008..100030 clears4338B0, then ctor3C9010 table3CC4D0
        // writes(0,0,0,1) at3C906C. This owner exposes only its XYZ lanes.
        std::array<std::uint32_t,3> zero4338B0{{0,0,0}};
        bool vectorKnown=true,reentrancyKnown=true,cursorKnown=true;
        // Image38544C and385160. This is closed-process startup provenance,
        // not an assertion about an attached/retained retail process.
        StdioIdentity reent38544C=StdioIdentity::reent385160;
        const std::array<StdioIdentity,3> standardFiles{{StdioIdentity::file385344,
            StdioIdentity::file38539C,StdioIdentity::file3853F4}}; //reent+04/08/0C
        std::uint32_t stdioInitialized=0; //image reent+38
        StdioIdentity stdioCleanup=StdioIdentity::null; //image reent+3C
        std::array<StandardFileSelectedFields,3> standardFileFields{};
        // Stable +54 custody is independent of mutable FILE/glue snapshots.
        // Image starts NULL;304570 and stdout fallback30B520 are joined writes.
        std::array<StdioIdentity,3> standardFileReent54{{StdioIdentity::null,
            StdioIdentity::null,StdioIdentity::null}};
        std::array<bool,3> standardFileReent54Known{{true,true,true}};
        StdioIdentity glueNext=StdioIdentity::null,glueFiles=StdioIdentity::null;
        std::uint32_t glueCount=0; //image reent+1D8/+1DC/+1E0
        bool stdioInitializationKnown=true,standardFileFieldsKnown=true,glueKnown=true;
        // No admitted writer; a new writer/alias must join or poison first.
        std::uint32_t byteLimit385450=1;
        StdioIdentity fixedLocale=StdioIdentity::locale3C6500;
        StdioIdentity decimalPoint=StdioIdentity::decimal3C6538;
        char decimalByte='.';
        bool byteLimitKnown=true,fixedLocaleKnown=true;
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
        s.stdioInitializationKnown=false;s.standardFileFieldsKnown=false;s.glueKnown=false;
        s.standardFileReent54Known.fill(false);
        s.streamIndexAssertionKnown=false;s.streamLookupAsciiKnown=false;
        s.byteLimitKnown=false;s.fixedLocaleKnown=false;
    }
    // First step of EVERY Stream308C90 occurrence. Do not initialize the
    // caller's temporary FILE fields before this source read succeeds.
    static bool readStreamReent308C98(const StreamSprintfAccess&,StdioIdentity& captured,
                                    std::uint32_t& pc)noexcept {
        const auto& s=state();pc=0x308c98;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           s.reent38544C!=StdioIdentity::reent385160)return false;
        captured=s.reent38544C;pc=0x308c9c;return true;
    }
    // Caller has now performed308CD0/CD4/CD8/CDC/CE0/CE8 in source order
    // on its owned temporary FILE. Only the captured identity goes into+54.
    // Join2686B8 before status and268860 Song%d. Success is entry30B568,
    // not a formatted-output receipt. FILE and destination live through308CF4.
    static bool initializeStreamSprintf30B528(const StreamSprintfAccess&,
                    StdioIdentity fileReent,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x30b50c;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           fileReent!=StdioIdentity::reent385160)return false;
        //Owned non-NULL FILE+54 takes30B510 branch; no global re-load here.
        pc=0x30b528;
        if(!s.stdioInitializationKnown||s.stdioInitialized>1)return false;
        if(!s.stdioInitialized){
            pc=0x30b534;
            if(!s.standardFileFieldsKnown||!s.glueKnown)return false;
            pc=0x30473c;s.stdioCleanup=StdioIdentity::cleanup3046F8;
            pc=0x304740;s.stdioInitialized=1;
            constexpr std::uint16_t flags[3]={4,9,10};
            for(std::size_t i=0;i<3;++i){
                auto& f=s.standardFileFields[i];
                //Actual calls304750/764/778; only selected source stores.
                pc=0x304570;f.reent=StdioIdentity::reent385160;
                s.standardFileReent54[i]=f.reent;s.standardFileReent54Known[i]=true;
                pc=0x304574;f.flags=flags[i];
                pc=0x304578;f.descriptor=static_cast<std::uint16_t>(i);
                pc=0x30457c;f.callbacks[0]=StdioIdentity::read308D90;
                pc=0x304580;f.callbacks[1]=StdioIdentity::write308DF8;
                pc=0x304584;f.callbacks[2]=StdioIdentity::seek308E78;
                pc=0x304588;f.callbacks[3]=StdioIdentity::close308EE0;
                pc=0x30458c;f.cursor=StdioIdentity::null;
                pc=0x304590;f.readCount=0;
                pc=0x304594;f.writeCount=0;
                pc=0x304598;f.buffer=StdioIdentity::null;
                pc=0x30459c;f.lineBufferSize=0;
                pc=0x3045a4;f.cookie=s.standardFiles[i];
            }
            pc=0x304784;s.glueFiles=s.standardFiles[0];
            pc=0x304788;s.glueCount=3;
            pc=0x30478c;s.glueNext=StdioIdentity::null;
        }
        pc=0x30b568;return true;
    }
    // Fixed status warning only:1336C0 ->30D920 ->30B4F0 prefix.
    // No printf result or source FILE output/callback execution is claimed.
    static bool initializeStreamStatusWarning1336C0(const StreamSprintfAccess& key,
                                                    std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x30d930;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           s.reent38544C!=StdioIdentity::reent385160)return false;
        pc=0x30d93c;
        if(s.standardFiles[1]!=StdioIdentity::file38539C)return false;
        pc=0x30b50c;
        if(!s.standardFileReent54Known[1])return false;
        auto captured=s.standardFileReent54[1];
        if(captured==StdioIdentity::null){
            pc=0x30b51c;
            if(!s.reentrancyKnown||s.reent38544C!=StdioIdentity::reent385160)return false;
            captured=s.reent38544C;
            pc=0x30b520;s.standardFileReent54[1]=captured;
            // Keep the selected snapshot coherent only while it remains known.
            if(s.standardFileFieldsKnown)s.standardFileFields[1].reent=captured;
        }
        return initializeStreamSprintf30B528(key,captured,pc);
    }
    // Run at actual30B5A0.305330 reads shared reent, but3052E8 ignores
    // its argument and returns the fixed locale. No host locale substitution.
    static bool streamLocale305328(const StreamSprintfAccess&,char& decimal,std::uint32_t& pc)noexcept {
        const auto& s=state();pc=0x305330;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           s.reent38544C!=StdioIdentity::reent385160)return false;
        pc=0x3052f0;
        if(!s.fixedLocaleKnown||s.fixedLocale!=StdioIdentity::locale3C6500)return false;
        pc=0x30b5a8;
        if(s.decimalPoint!=StdioIdentity::decimal3C6538||s.decimalByte!='.')return false;
        decimal=s.decimalByte;pc=0x30b5b0;return true;
    }
    // Actual30B658/65C ->306108, only non-NULL in-bounds immutable
    // caller-owned format bytes. Caller keeps any prior wide value when limit0.
    static bool streamByte306108(const StreamSprintfAccess&,unsigned char byte,
            std::uint32_t& wide,std::int32_t& length,std::uint32_t& pc)noexcept {
        const auto& s=state();pc=0x30b658;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           s.reent38544C!=StdioIdentity::reent385160)return false;
        pc=0x30b65c;if(!s.byteLimitKnown)return false;
        if(!s.byteLimit385450){pc=0x306124;length=-1;}
        else {pc=0x306130;wide=byte;pc=0x306138;length=byte?1:0;}
        pc=0x30b670;return true;
    }
    //Narrow water308C90 bridge. No new reentrancy/locale/byte-limit owner,
    //no mutable input identities. Only this outer class constructs the inner
    //Stream key; the World caller cannot extract one from its Water key.
    static bool readWaterReent308C98(const WaterSprintfAccess&,StdioIdentity& captured,
                                   std::uint32_t& pc)noexcept {
        return readStreamReent308C98(StreamSprintfAccess{},captured,pc);
    }
    static bool initializeWaterSprintf30B528(const WaterSprintfAccess&,StdioIdentity captured,
                                           std::uint32_t& pc)noexcept {
        return initializeStreamSprintf30B528(StreamSprintfAccess{},captured,pc);
    }
    static bool waterLocale305328(const WaterSprintfAccess&,char& decimal,std::uint32_t& pc)noexcept {
        return streamLocale305328(StreamSprintfAccess{},decimal,pc);
    }
    static bool waterByte306108(const WaterSprintfAccess&,unsigned char byte,
        std::uint32_t& wide,std::int32_t& length,std::uint32_t& pc)noexcept {
        return streamByte306108(StreamSprintfAccess{},byte,wide,length,pc);
    }
    //Narrow Music2C0C44 bridge into the SAME process State. The manager
    //cannot extract a Stream key; only this outer class constructs that key.
    //Child storage retains its own symbolic identity, never a private G type.
    static bool readMusicReent308C98(const MusicSprintfAccess&,StdioIdentity& captured,
                                   std::uint32_t& pc)noexcept {
        return readStreamReent308C98(StreamSprintfAccess{},captured,pc);
    }
    static bool initializeMusicSprintf30B528(const MusicSprintfAccess&,StdioIdentity captured,
                                           std::uint32_t& pc)noexcept {
        return initializeStreamSprintf30B528(StreamSprintfAccess{},captured,pc);
    }
    static bool musicLocale305328(const MusicSprintfAccess&,char& decimal,std::uint32_t& pc)noexcept {
        return streamLocale305328(StreamSprintfAccess{},decimal,pc);
    }
    static bool musicByte306108(const MusicSprintfAccess&,unsigned char byte,
        std::uint32_t& wide,std::int32_t& length,std::uint32_t& pc)noexcept {
        return streamByte306108(StreamSprintfAccess{},byte,wide,length,pc);
    }
    //158A34 ->30AE70 reads38544C at30AE84 before the admitted decimal
    //30AC38 path. No integer result, errno write, or call return is minted here.
    static bool readMusicIntegerReent30AE84(const MusicSprintfAccess&,std::uint32_t& pc)noexcept {
        const auto& s=state();pc=0x30ae84;
        if(s.thread!=std::this_thread::get_id()||!s.reentrancyKnown||
           s.reent38544C!=StdioIdentity::reent385160)return false;
        pc=0x30ae88;return true;
    }
    //Apply BEFORE Music's native2C0B40 filename-concat adapter. A later
    //Song formatter cannot retroactively establish earlier stdio initialization.
    //This forwards the existing native boundary, not source printf execution.
    static void nativeMusicStdioAdapterBoundary(const MusicSprintfAccess&)noexcept {
        streamNativeDiagnosticBoundary(StreamSprintfAccess{});
    }
    //World1B38DC/F4 and later2746D8 sprintf/1336C0 log occurrences are
    //closed native byte/diagnostic adapters, not joined source stdio. Apply
    //this boundary BEFORE the first such World adapter, and reconcile again
    //before exact Water use. It cannot establish+38=1: preserve it only if a
    //represented earlier startup call already established it. Otherwise its
    //knownness stays invalid and later30B528 reads stop, including Stream.
    //Discard FILE/glue snapshots; no source call/return receipt is fabricated.
    //Unknown source callbacks/aliases still require full poison before admission.
    static void worldNativeStdioAdapterBoundary(const WaterSprintfAccess&)noexcept {
        streamNativeDiagnosticBoundary(StreamSprintfAccess{});
    }
    // Explicit native warning-transport boundary, NOT a retail stdio receipt.
    // The adapter must not execute the stored source FILE/cleanup callbacks.
    // It does not mutate this owner's+38,38544C,385450 or fixed locale.
    // Native fputs cannot change symbolic standardFiles or FILE+54 custody.
    // Once initialized, later sprintf does not inspect standard FILEs/glue, so their
    // discarded snapshots are neither refreshed nor asserted to remain current.
    // Arbitrary/source stdio, external reent aliases or unknown callback effects
    // require poisonUnjoinedAliasesNative BEFORE admission instead.
    static void streamNativeDiagnosticBoundary(const StreamSprintfAccess&)noexcept {
        auto& s=state();s.standardFileFieldsKnown=false;s.glueKnown=false;
        if(s.thread!=std::this_thread::get_id()||s.stdioInitialized!=1)
            s.stdioInitializationKnown=false;
    }
    static bool streamLookupFold308F00(const StreamLookupAccess&,unsigned char byte,
                                       std::int32_t& folded,std::uint32_t&)noexcept {
        const auto& s=state();
        if(s.thread!=std::this_thread::get_id()||!s.streamLookupAsciiKnown||byte>127u)return false;
        folded=byte>=static_cast<unsigned char>('A')&&byte<=static_cast<unsigned char>('Z')?
            std::int32_t(byte)+32:std::int32_t(byte);return true;
    }
    //Water's actual308F00 shares the SAME hardcoded3C6391 ASCII owner.
    //The outer class alone manufactures the lookup key; no key escapes.
    static bool waterFold308F00(const WaterSprintfAccess&,unsigned char byte,
                               std::int32_t& folded,std::uint32_t& pc)noexcept {
        return streamLookupFold308F00(StreamLookupAccess{},byte,folded,pc);
    }
    static bool streamLookupIndexAssertion268514(const StreamLookupAccess&,std::int32_t index,
                                                 std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x268514;
        if(s.thread!=std::this_thread::get_id()||!s.streamIndexAssertionKnown)return false;
        if(!s.streamIndexGuard8B10){pc=0x268528;s.streamIndexAssertion442E80=1;
            pc=0x26852c;s.streamIndexGuard8B10=true;}
        pc=0x268534;
        if(s.streamIndexAssertion442E80){
            //Positive predicate133660 returns0 on both36C100 branches.
            //Missing-index logging/assert continuation is NOT manufactured.
            pc=0x268554;if(index==-1)return false;
        }
        pc=0x268588;return true;
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
