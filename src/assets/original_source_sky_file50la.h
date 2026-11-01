#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include <array>
#include <memory>
#include <string>
#include <optional>
#include <thread>
#include <vector>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
//Private actual sky131230 occurrence; no Config/PSG/prepared sky authorization.
class OriginalSourceSkyFile50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    const void* receiver_=nullptr; //Actual nullable receiver; only compared, never dereferenced.
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root_;
    const void* selectedRoot_=nullptr;
    std::uint64_t trackGeneration_=0;
    std::string name_;
    bool attempted_=false,borrowed_=false,nativeClosed_=false,searchReturned_=false,found_=false,openReturned_=false;
    std::optional<std::int64_t> before168_,after168_,sourceHandle_;
    std::optional<std::uint32_t> observedMode170_;
    std::int32_t handle_=-1;
    std::uint32_t pc_=0x1295e0;
    struct Read4 {
        bool attempted=false,returned=false;
        std::optional<std::size_t> before,after;
        std::size_t copied=0;
        std::array<std::uint8_t,4> bytes; //Only copied prefix is known.
        std::optional<std::uint32_t> items;
        std::optional<std::uint32_t> pendingSourceItems; //Positive EOF clamp: bound before12FF34, not copied/size.
    } read_,countRead_,paletteOffsetRead_;
    std::array<Read4,3> paletteHeaderReads_;
    void* paletteReceiver_=nullptr;
    bool paletteEntered_=false,paletteWithdrawn_=false,paletteReleasedSource_=false;
    bool paletteScanAttempted_=false,paletteScanReturned_=false,paletteScanMatchDetected_=false;
    std::optional<std::int32_t> paletteScanCount_;
    std::uint32_t paletteScanVisited_=0;
    struct PalettePayloadRead {
        bool attempted=false,returned=false;
        std::optional<std::size_t> before,after;
        std::size_t copied=0;
        std::optional<std::uint32_t> items;
        std::optional<std::uint32_t> pendingSourceItems; //Positive EOF clamp: bound before12FF34, not copied/size.
    };
    std::array<PalettePayloadRead,3> rawPaletteReads_;
    std::optional<std::size_t> expectedCursor_,resourceStart_,resourceSeekTarget_;
    bool restoreAttempted_=false,restoreReturned_=false;
    std::optional<std::size_t> restoreTarget_;
    struct PaletteIteration {
        Read4 offsetRead;
        bool tellAttempted=false,tellReturned=false,seekAttempted=false,seekReturned=false;
        std::optional<std::size_t> start,target;
        void* parserReceiver=nullptr;
        bool parserEntered=false,parserWithdrawn=false,parserReleasedSource=false;
        bool scanAttempted=false,scanReturned=false,scanMatchDetected=false;
        std::optional<std::int32_t> scanCount;
        std::uint32_t scanVisited=0;
        bool restoreAttempted=false,restoreReturned=false;
        std::optional<std::size_t> restoreTarget;
        std::array<Read4,3> headers;
        std::array<PalettePayloadRead,3> payloads;
    };
    std::vector<std::unique_ptr<PaletteIteration>> followingPalettes_;
    struct ItemFieldSpec {std::size_t destination,size;std::uint32_t call,resume;};
    //Index0 is the distinct stack+7E halfword, NOT receiver+4C.
    static ItemFieldSpec itemFieldSpec(std::size_t i)noexcept {
        if(i==0)return {0,2,0x128a24,0x128a2c};
        if(i==1)return {0x4e,2,0x128a48,0x128a50};
        if(i<8)return {0x54+(i-2)*0x100,2,0x128a78,0x128a80};
        if(i<14)return {0x50+(i-8)*0x100,4,0x128ab8,0x128ac0};
        if(i>=23)return {0x98+((i-23)/16)*0x100+((i-23)%16)*4,4,0x128bcc,0x128bd4};
        if(i>=17)return {0xd8+(i-17)*0x100,32,0x128b74,0x128b7c};
        return {0x10+(i-14)*0x10,12,i==14?0x128aecu:i==15?0x128b10u:0x128b34u,
            i==14?0x128af4u:i==15?0x128b18u:0x128b3cu};
    }
    struct ItemFieldRead : PalettePayloadRead {
        std::array<std::uint8_t,32> bytes; //Only copied prefix is known, including stack flags.
    };
    struct ItemFileOccurrence {
        Read4 offsetRead,header48;
        std::array<ItemFieldRead,119> fields;
        bool tellAttempted=false,tellReturned=false,seekAttempted=false,seekReturned=false;
        std::optional<std::size_t> start,target;
        void* receiver=nullptr;
        bool parserEntered=false,nativeWithdrawn=false,parserReleasedSource=false;
        bool tailEntered=false,alignmentSelected=false,alignmentTellAttempted=false,alignmentTellReturned=false;
        bool alignmentSeekAttempted=false,alignmentSeekReturned=false,finalTellAttempted=false,finalTellReturned=false;
        std::optional<std::size_t> alignmentPosition,alignmentTarget,finalPosition;
        std::optional<std::uint32_t> alignmentPadding;
        bool restoreAttempted=false,restoreReturned=false;
        std::optional<std::size_t> restoreTarget;
        std::optional<std::int32_t> restoreResult;
    };
    static bool itemFieldRequired(const ItemFileOccurrence& item,std::size_t index)noexcept {
        if(index<17)return true;
        if(item.fields[0].items!=1u)return false; //Stack flags cannot be read before both bytes arrive.
        const auto flags=std::uint32_t(item.fields[0].bytes[0])|(std::uint32_t(item.fields[0].bytes[1])<<8);
        return (flags&(index<23?4u:1u))!=0;
    }
    static std::optional<std::uint32_t> itemFieldPredecessor(const ItemFileOccurrence& item,std::size_t index)noexcept {
        while(index){--index;if(itemFieldRequired(item,index))return item.fields[index].items;}
        return item.header48.items;
    }
    std::unique_ptr<ItemFileOccurrence> firstItem_;
    std::vector<std::unique_ptr<ItemFileOccurrence>> followingItems_;
    ItemFileOccurrence* itemOccurrence(std::size_t i)const noexcept {
        return i?(i<=followingItems_.size()?followingItems_[i-1].get():nullptr):firstItem_.get();
    }
    bool readItemOffset(D&,std::size_t occurrence=0)noexcept;
    bool tellItemStart(D&,std::size_t occurrence=0)noexcept;
    bool seekItemAbsolute(std::uint32_t,D&,std::size_t occurrence=0)noexcept;
    bool restoreItemStart(D&,std::size_t occurrence=0)noexcept;
    struct ResourcePayloadRead : PalettePayloadRead {
        std::size_t requested=0;
        std::uint8_t* destination=nullptr;
        bool nativeWithdrawn=false;
    };
    struct CompressedTell {bool attempted=false,returned=false;std::optional<std::size_t> position;};
    struct CompressedOffset {CompressedTell tell;Read4 read;bool terminal=false;};
    struct ResourceFileOccurrence {
        std::size_t resourceOrdinal=0;bool entryOpened=false;std::optional<std::size_t> entryCursor;
        bool resourceTailEntered=false,resourceAlignmentSelected=false,resourceAlignmentSeekAttempted=false,resourceAlignmentSeekReturned=false,resourceAlignmentReturned=false;
        CompressedTell resourceAlignmentTell;
        std::optional<std::uint32_t> resourceAlignmentPadding;
        std::optional<std::size_t> resourceAlignmentTarget;
        Read4 resourceSizeA8;std::array<Read4,5> compressedHeaders;CompressedTell compressedInitialTell,compressedFinalTell;std::array<CompressedOffset,12> compressedOffsets;Read4 compressedPayloadSize;ResourcePayloadRead compressedPayload,rawPayload;std::vector<std::unique_ptr<Read4>> followingCompressedSizes;std::vector<std::unique_ptr<ResourcePayloadRead>> followingCompressedPayloads;};
    struct ItemResourceSeek : ResourceFileOccurrence {
        std::size_t occurrence=0;bool attempted=false,returned=false;
        std::optional<std::size_t> before,target;std::optional<std::int32_t> result;
        std::vector<std::unique_ptr<ResourceFileOccurrence>> resourceEntries;
    };
    std::vector<std::unique_ptr<ItemResourceSeek>> itemResourceSeeks_;
    ResourceFileOccurrence* resourceFileOccurrence(std::size_t item,std::size_t ordinal)const noexcept {
        if(item>=itemResourceSeeks_.size()||!itemResourceSeeks_[item])return nullptr;
        auto& root=*itemResourceSeeks_[item];return ordinal?(ordinal<=root.resourceEntries.size()?root.resourceEntries[ordinal-1].get():nullptr):&root;
    }
    ResourceFileOccurrence* activeResourceFile(std::size_t item)const noexcept {
        return item<itemResourceSeeks_.size()&&itemResourceSeeks_[item]?resourceFileOccurrence(item,itemResourceSeeks_[item]->resourceEntries.size()):nullptr;
    }
    bool beginNextResource(std::size_t,std::size_t,std::size_t,D&)noexcept;
    bool seekItemResource(std::size_t,D&)noexcept;
    bool readItemResourceSize(std::size_t,D&)noexcept;
    bool readCompressedResourceHeader(std::size_t,std::size_t,D&)noexcept;
    bool seekResourceAlignment1290B0(std::size_t,D&)noexcept;
    bool alignResource129054(std::size_t,std::uint16_t,D&)noexcept;
    bool tellCompressedFinal(std::size_t,D&)noexcept;
    bool tellCompressedOffset(std::size_t,std::size_t,bool,D&)noexcept;
    bool readCompressedOffset(std::size_t,std::size_t,D&)noexcept;
    bool readCompressedPayloadSize(std::size_t,D&)noexcept;
    bool readCompressedPayload(std::size_t,std::uint8_t*,std::size_t,D&)noexcept;
    bool readFollowingCompressedSize(std::size_t,std::size_t,D&)noexcept;
    bool transferCompressedPayload(ResourcePayloadRead&,std::uint8_t*,std::size_t,D&)noexcept;
    bool readFollowingCompressedPayload(std::size_t,std::size_t,std::uint8_t*,std::size_t,D&)noexcept;
    bool readItemResourcePayload(std::size_t,std::uint8_t*,std::size_t,D&)noexcept;
    bool beginItemParser(void*,std::size_t occurrence=0)noexcept;
    bool releaseItemParserSource(void*,std::size_t occurrence)noexcept;
    bool readItemHeader48(D&,std::size_t occurrence=0)noexcept;
    bool readItemField(std::size_t,D&,std::size_t occurrence=0)noexcept;
    bool finishItemReads(D&,std::size_t occurrence=0)noexcept;
    bool withdrawFirstItemNative()noexcept;
    bool tellAttempted_=false,tellReturned_=false,seekAttempted_=false,seekReturned_=false;
    explicit OriginalSourceSkyFile50LA(const void* receiver)noexcept:receiver_(receiver){}
    bool begin131230(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,std::shared_ptr<OriginalRaTrackSourceFile50LA>,
        const std::string&,D&)noexcept;
    bool sourceReadBoundsCurrentNative()const noexcept;
    bool preflightNative()const noexcept;
    bool current()const noexcept;
    bool read4(Read4&,std::uint32_t,D&,bool initializedSuffix=false)noexcept;
    bool readFirst4(D&)noexcept;
    bool readCount4(D&)noexcept;
    bool readPaletteOffset4(D&)noexcept;
    bool tellResourceStart(D&)noexcept;
    bool seekResourceAbsolute(std::uint32_t,D&)noexcept;
    bool beginPalette2E38C0(void*,std::size_t occurrence=0)noexcept;
    bool readRawPaletteBlock(std::size_t,D&,std::size_t occurrence=0)noexcept;
    bool readPaletteHeader4(std::size_t,D&,std::size_t occurrence=0)noexcept;
    const Read4& paletteHeader(std::size_t occurrence,std::size_t index)const noexcept {
        return occurrence?followingPalettes_[occurrence-1]->headers[index]:paletteHeaderReads_[index];
    }
    const PalettePayloadRead& palettePayload(std::size_t occurrence,std::size_t index)const noexcept {
        return occurrence?followingPalettes_[occurrence-1]->payloads[index]:rawPaletteReads_[index];
    }
    bool withdrawPaletteNative()noexcept;
    bool restorePaletteStart(D&)noexcept;
    bool restoreFollowingPaletteStart(std::size_t,D&)noexcept;
    bool readFollowingPaletteOffset(D&)noexcept;
    bool tellFollowingPaletteStart(D&)noexcept;
    bool seekFollowingPaletteAbsolute(std::uint32_t,D&)noexcept;
    bool scanFreshPalette1327D0(D&,std::size_t occurrence=0)noexcept;
    bool retirePaletteDestinationSource(std::size_t occurrence=0)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalSourceSkyFile50LA()noexcept=default; //Explicit file withdrawal before receiver retirement.
    OriginalSourceSkyFile50LA(const OriginalSourceSkyFile50LA&)=delete;
    OriginalSourceSkyFile50LA& operator=(const OriginalSourceSkyFile50LA&)=delete;
};
}
