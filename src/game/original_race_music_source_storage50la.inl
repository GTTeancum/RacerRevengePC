#pragma once
// Included before the music manager Impl. Private source-owned backing only;
// none of these fields is a public readiness or caller-supplied completion token.
#include <cstring>
#include "../assets/original_music_source_file50la.h"
#include "../../platform/linux/native/original_stream_library_source50la.h"
namespace rr::game::original_race_music_source50LA {
struct VectorAssertions final {
    const std::thread::id thread=std::this_thread::get_id();
    bool guard8834=false,guard8838=false;
    std::uint32_t assertion434AB8=0,assertion434AC0=0;
};
inline VectorAssertions& vectorAssertions()noexcept {
    // Original lazy BSS guards persist across serial managers. Other original
    //235CB0 callers must join this SAME projection, not seed a per-child copy.
    static VectorAssertions state;return state;
}
struct Config final {
    std::int64_t file00;
    std::uint32_t binary08,opened0C,mode10,count14,index18;
    const void* text1C;std::uint32_t selected20;const void* child24;
    std::array<char,1024> name28;
    std::uint32_t field428,field42C,field430,field434,cacheCount438;
    std::array<std::uint32_t,32> cache43C;
    const void *names4CC,*index4BC;
    std::uint32_t indexBytes4C0;const void* block4C4;std::uint32_t field4C8;
    Config()noexcept {
        file00=-1;binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
        text1C=nullptr;selected20=0;child24=nullptr;name28[0]=0;
        field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
        cache43C.fill(0);names4CC=nullptr;index4BC=nullptr;
        indexBytes4C0=0;block4C4=nullptr;field4C8=0;
    }
};
struct Child final {
    OriginalWreckComponent50LA entity;
    bool counted=false,constructorReturned=false,published164=false,loadEntered=false;
    // Knownness is independent of contents on partially reached constructors.
    std::optional<std::uint32_t> dispatch08,word30,count34,group38,mode40;
    const void* backend3C=nullptr;
    std::optional<std::uint32_t> vectorCount44,growth4C,capacity50;
    // The source array's contents are unwritten. nullopt means UNKNOWN, not0.
    std::unique_ptr<std::optional<std::uint32_t>[]> storage48;
    std::uint8_t flags54KnownMask=0,flags54KnownValue=0;
    std::optional<std::uint32_t> word58,word5C,word60,word64,word68,word6C,word70,word78;
    std::uint8_t flags74KnownMask=0,flags74KnownValue=0;
    std::optional<Config> config60;
    // The file occurrence belongs to THIS retained automatic Config. Native
    // backing/knownness below does not mint a source open/Config return.
    std::unique_ptr<assets::OriginalMusicSourceFile50LA> configFile;
    assets::OriginalStandardTextureDiagnostic50LA configFileDiagnostic{};
    bool configOpenAttempted=false;
    std::optional<std::uint32_t> configOpenReturn;
    struct Read final {
        std::uint32_t callPC=0;std::size_t requested=0,copied=0;
        bool returned=false;
    };
    std::array<Read,196> configReads{}; //100 open reads plus48 selections, two reads each.
    std::size_t configReadCount=0;
    std::array<std::uint8_t,7> magic48;
    std::array<std::uint8_t,4> version54,sections58,length5C;
    std::size_t magicKnown=0,versionKnown=0,sectionsKnown=0;
    bool magicTerminatorWritten=false,lengthZeroed=false;
    std::uint32_t nameOrdinal=0;
    std::array<std::size_t,48> nameKnown{};
    std::unique_ptr<std::uint8_t[]> names4CC,index4BC; // No zero filling.
    std::size_t indexKnown=0;
    struct Lookup final {
        std::array<char,1024> lowerKey;std::size_t lowerKnown=0;
        std::array<std::int32_t,16> visited{};std::size_t visitedCount=0;
        std::optional<std::int32_t> result;
        std::uint32_t comparisonPC=0;std::uint8_t matchArm=0; //0 single,1 midpoint,2 low,3 high.
    };
    struct Selection final {
        Lookup lookup;std::optional<bool> returned;
        std::optional<std::uint32_t> offset,length;
        std::array<std::uint8_t,4> fieldCount86C;
        bool countZeroed=false,seekReturned=false;
    };
    struct Getter final {Lookup lookup;std::optional<bool> returned;std::size_t produced=0;};
    bool metadataAttempted=false;
    std::array<Selection,48> selections;
    Getter numberGetter;std::array<Getter,47> filenameGetters;
    struct SongLookup {
        std::shared_ptr<platform::OriginalStreamLibrarySource50LA::Lookup> occurrence;
        std::optional<std::int32_t> index;
        std::optional<std::uint32_t> sourceReturn;
        std::size_t filenameKnown=0;bool copied=false;
    };
    std::array<SongLookup,47> songLookups;
    std::size_t songFilenameKnown=0,lookupReturns=0;
    std::unique_ptr<std::uint8_t[]> block4C4,blockKnown;
    std::size_t blockCapacity=0,configNameKnown=1;
    std::array<char,1024> numberStack20;
    std::optional<bool> numberOfSongsReturned;
    struct SongFormat final {
        //This non-friend Child never names or manufactures a Globals passkey.
        //No numeric EE address or independently owned process state is stored.
        enum class ReentIdentity { source385160 };
        std::optional<ReentIdentity> capturedReent,fileReent54;
        std::optional<char*> buffer10,cursor00;
        std::optional<std::uint32_t> remaining08,bufferSize14;
        std::optional<std::uint16_t> flags0C;
        struct Scan {std::size_t offset=0;std::optional<std::uint32_t> wide;std::optional<std::int32_t> length;};
        std::array<Scan,7> scans;std::size_t scanCount=0,copied=0;
        std::array<std::size_t,2> copiedSegments{};std::size_t segmentCount=0;
        bool attempted=false,initializerReturned=false,localeReturned=false,terminated=false;
        std::optional<std::uint32_t> formatterReturn,sourceReturn;
    };
    std::array<SongFormat,47> songFormats; //Native receipts only; no source stack zero fill.
    std::array<char,0x110> songSection860;
    std::size_t songSectionKnown=0;
    std::array<char,0x104> songFilename530;
    std::unique_ptr<std::uint8_t[]> songEntries58;
    const void* sourceSongs58=nullptr;bool songs58Published=false;
    std::size_t songEntryBytes=0;
    std::optional<std::uint32_t> songOrdinal,songContinueS3;

    struct Free {const void* identity=nullptr;std::uint32_t callPC=0;
        bool entered=false,returned=false;};
    struct ConfigDestruction {
        bool entered=false,delayValueWritten=false,flushEntered=false,closeEntered=false;
        std::optional<std::uint32_t> savedResultS0;
        std::optional<std::int32_t> flushReturn,closeReturn;
        bool closeReturned=false,selectedClearReturned=false;
        Free names,index,block;
        bool returned=false,automaticLifetimeEnded=false,nativeFileRetired=false;
        const void* returnedThis=nullptr;std::int32_t flag=-1;
    } configDestruction;
    std::optional<std::uint32_t> load2C0B00Return;
    const OriginalWreckComponent50LA* initialize2C0A60Return=nullptr;
    bool initialize2C0A60Returned=false; //Actual pointer return, not playback/readiness.
    std::array<char,0x110> filename640,filename750; // Unknown beyond reached NUL.
    std::size_t filename640Known=0,filename750Known=0;
    std::uint32_t pc=0x1a3c34;
    Child()noexcept {} // Do not value-initialize unknown source stack buffers.
    Child(const Child&)=delete;Child& operator=(const Child&)=delete;
};
//Native backing for the first original0x250 SoundEmitter allocation. Reached
//fields are optional so an incomplete constructor never invents zeroed bytes.
struct Sound final {
    OriginalWreckComponent50LA entity;
    const OriginalRaceMusicManager50LA* nativeIssuer=nullptr;
    bool counted=false,constructorEntered=false,emitterEntered=false,emitterReturned=false;
    std::optional<std::uint32_t> dispatch08;
    const OriginalWreckComponent50LA* emitterReturn=nullptr;
    std::array<std::optional<std::uint32_t>,4> previous40;
    std::array<std::optional<std::uint32_t>,28> configuration160;
    std::optional<const OriginalWreckComponent50LA*> manager1D8,group30;
    std::optional<const void*> sound1F0;
    std::optional<std::uint32_t> flags158,trigger1DC,chance1E0,serial50,distance1D0,word1E8,word1EC;
    std::optional<std::uint16_t> type1E4;
    std::optional<std::uint8_t> nameFirst54;
    std::uint8_t flags1D4KnownMask=0,flags1D4KnownValue=0;
    bool assertionEntered=false;std::optional<std::uint32_t> assertionReturn;
    const void* sharedEmitterIdentity=nullptr; //Same Application owner, not another process copy.
    struct Vector {
        std::optional<const void*> backing04;
        std::optional<std::uint32_t> count00,growth08,capacity0C;
        std::uint8_t flags10KnownMask=0,flags10KnownValue=0;
        bool constructorEntered=false,constructorReturned=false;
        const Vector* returnedThis=nullptr;
    } vector200,vector214,vector228;
    const Vector* initializeReceiver=nullptr;
    std::optional<std::uint32_t> initializeInitial,initializeGrowth;
    bool initialize293D10Entered=false; //Not the later allocator or a returned vector call.
    std::optional<const OriginalWreckComponent50LA*> constructorReturn;
    std::uint32_t pc=0x2c20f0;
    Sound()noexcept {} //Source unknown buffers/fields are not filled with numeric defaults.
};
//Only the concrete manager's synchronous post-child continuation mutates this
//record. It cannot be supplied as a readiness/attachment token by a caller.
struct ManagerTail final {
    OriginalWreckComponent50LA *parentArgument=nullptr,*childArgument=nullptr,*returnedChild=nullptr;
    std::optional<std::uint32_t> maskArgument;
    bool argumentsPrepared=false,attachmentEntered=false,attachmentReturned=false,nativeChildDetached=false;
    std::unique_ptr<std::uint8_t[]> events158; //Exactly0x280 bytes; contents remain UNKNOWN.
    std::optional<std::uint32_t> allocationBytes;
    const void* allocationReturn=nullptr; //Typed native identity, never guest pointer bits.
    bool allocationEntered=false,allocationReentRead=false,allocationReturned=false,eventsPublished=false,nativeEventsRetired=false;
    std::optional<std::uint32_t> soundOrdinal;
    OriginalWreckComponent50LA* soundLoopOwner=nullptr; //Retained source s2 identity at1A3CA8.
    std::unique_ptr<Sound> firstSound; //UnpublishedC0[0] constructor prefix only.
    std::optional<std::uint32_t> soundAllocationBytes;
    const void* soundAllocationReturn=nullptr;
    bool soundAllocationEntered=false,soundAllocationReentRead=false,soundAllocationReturned=false;
    bool nativeSoundRetired=false;
};

}
