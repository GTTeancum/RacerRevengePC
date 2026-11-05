#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_config_process_globals50la.h"
#include <optional>
#include <array>
#include <filesystem>
#include <string_view>
namespace rr::game {class OriginalRaceMusicManager50LA;}
namespace rr::assets {
// Private single1578D0 occurrence. The actual manager supplies its same
// Application FileManager, audio root and automatic Config after validating
// its issuer. A path/name/byte match alone cannot mint this owner.
class OriginalMusicSourceFile50LA final {
    friend class rr::game::OriginalRaceMusicManager50LA;
    friend class OriginalStandardTextureResolver50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    using R=OriginalStandardTextureResolver50LA;
    enum class Occurrence { raceMusic2C0B98,announcerPercentages2C5164 };
    const Occurrence occurrence_; //Immutable per-instance authority, never a retargeted/renamed Music file.
    const original_main_entry50KG::Application* const issuer_;
    const rr::game::OriginalRaceMusicManager50LA* const manager_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<R> resolver_;
    std::shared_ptr<const std::vector<std::uint8_t>> input_;
    const void* config_=nullptr;
    std::filesystem::path audioRoot_;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    std::int32_t device_=-1,member_=-1,handle_=-1,winner_=-1;
    std::uint32_t observedMode170_=0;
    std::optional<std::int32_t> searchCursorBefore_,searchCount_,searchFirst_,searchReturn_;
    std::uint32_t searchProbes_=0;
    std::optional<std::uint32_t> sourceMode170Read_; //Absent when search found a foreign winner.

    bool probeAttempted_=false,probeHelperReturned_=false,probeSnapshotTaken_=false;
    const void* probeOutputIdentity_=nullptr;
    std::size_t probeCopied_=0;
    bool textSeekAttempted_=false,textSeekFirstStore_=false,textSeekSecondStore_=false;
    bool textSeekReturned_=false,textSeekSnapshotTaken_=false;
    std::optional<std::uint32_t> textSeekReload_,textSeekReturnV0_;
    //First parser line only. No file/output mutation or source line return.
    bool textLinePrefixAttempted_=false,textLineAssertionsReturned_=false;
    const void* textLineOutputIdentity_=nullptr;
    std::optional<std::uint32_t> textLineLengthRead_,textLineCursorRead_,textLineRemaining_;
    std::optional<bool> textLinePositiveBranch_;
    struct LineTransfer final {
        bool entered=false;
        const void* outputIdentity=nullptr;
        std::optional<std::int32_t> deviceRead;
        std::optional<std::uint32_t> logicalStartRead;
        std::optional<std::uint32_t> position;
        bool activeAssertionReturned=false;
        std::optional<std::int32_t> deviceIDRead;
        std::optional<std::int32_t> activeHeadRead;
        std::optional<bool> ownHeadBranch;
        std::optional<const void*> handleRead;
        std::optional<bool> nonnullHandleBranch;
        std::optional<std::uint32_t> activationV0;
        bool activationCursorStored=false;
        std::optional<std::uint32_t> requestLimit;
        std::optional<std::uint32_t> request;
        std::optional<std::int32_t> scratchIndexRead;
        bool rawCallEntered=false;
        std::optional<std::uint32_t> rawFrameBytes;
        std::optional<std::uint32_t> rawSavedReturnPC;
        std::array<bool,10> rawSavesIssued{};
        std::optional<std::uint32_t> flagsRead;
        std::optional<bool> compressedBranch;
        std::optional<bool> directBranch;
        std::optional<std::uint32_t> deviceCursorRead;
        std::optional<std::int32_t> scratchOffsetRead;
        std::optional<std::uint32_t> withinBits;
        std::optional<std::int64_t> withinSigned;
        std::optional<bool> cacheHitBranch;
        std::optional<std::uint32_t> copyBytes;
        std::size_t copied=0;
        std::optional<std::uint32_t> deviceCursorReload;
        bool deviceCursorStored=false;
        std::optional<std::uint32_t> returnV0;
        std::optional<std::uint32_t> releasedFrameBytes;
        bool rawReturned=false;
        bool unissued()const noexcept {
            for(const bool saved:rawSavesIssued)if(saved)return false;
            return !entered&&!outputIdentity&&!deviceRead&&!logicalStartRead&&!position&&
                !activeAssertionReturned&&!deviceIDRead&&!activeHeadRead&&!ownHeadBranch&&!handleRead&&
                !nonnullHandleBranch&&!activationV0&&!activationCursorStored&&!requestLimit&&!request&&
                !scratchIndexRead&&!rawCallEntered&&!rawFrameBytes&&!rawSavedReturnPC&&!flagsRead&&
                !compressedBranch&&!directBranch&&!deviceCursorRead&&!scratchOffsetRead&&!withinBits&&
                !withinSigned&&!cacheHitBranch&&!copyBytes&&!copied&&!deviceCursorReload&&
                !deviceCursorStored&&!returnV0&&!releasedFrameBytes&&!rawReturned;
        }
        bool historyCurrent()const noexcept {
            if(!entered)return unissued();
            if(!outputIdentity||!deviceRead||*deviceRead<0||logicalStartRead!=0u||position!=0u)return false;
            const auto rawUnissued=[&]()noexcept {
                for(const bool saved:rawSavesIssued)if(saved)return false;
                return !requestLimit&&!request&&!scratchIndexRead&&!rawCallEntered&&!rawFrameBytes&&!rawSavedReturnPC&&
                    !flagsRead&&!compressedBranch&&!directBranch&&!deviceCursorRead&&!scratchOffsetRead&&!withinBits&&!withinSigned&&
                    !cacheHitBranch&&!copyBytes&&!copied&&!deviceCursorReload&&!deviceCursorStored&&!returnV0&&!releasedFrameBytes&&!rawReturned;
            };
            if(!activeAssertionReturned)
                return !deviceIDRead&&!activeHeadRead&&!ownHeadBranch&&!handleRead&&!nonnullHandleBranch&&!activationV0&&
                    !activationCursorStored&&rawUnissued();
            if(deviceIDRead!=deviceRead||!activeHeadRead||ownHeadBranch!=(*activeHeadRead==*deviceIDRead))return false;
            if(!*ownHeadBranch)return !handleRead&&!nonnullHandleBranch&&!activationV0&&!activationCursorStored&&rawUnissued();
            if(!handleRead||nonnullHandleBranch!=(*handleRead!=nullptr)||activationV0!=1u)return false;
            if(!*nonnullHandleBranch)return !activationCursorStored&&rawUnissued();
            if(!activationCursorStored||requestLimit!=0x3feu||request!=255u||!scratchIndexRead||
               !rawCallEntered||rawFrameBytes!=0xf0u||rawSavedReturnPC!=0x12f1b4u)return false;
            for(const bool saved:rawSavesIssued)if(!saved)return false;
            if(!flagsRead||compressedBranch!=((*flagsRead&0x100u)!=0u))return false;
            const auto copyUnissued=[&]()noexcept {
                return !deviceCursorRead&&!scratchOffsetRead&&!withinBits&&!withinSigned&&!cacheHitBranch&&!copyBytes&&!copied&&
                    !deviceCursorReload&&!deviceCursorStored&&!returnV0&&!releasedFrameBytes&&!rawReturned;
            };
            if(*compressedBranch)return !directBranch&&copyUnissued();
            if(directBranch!=(*scratchIndexRead==-1||*request>=0x6000u))return false;
            if(*directBranch)return copyUnissued();
            if(deviceCursorRead!=position||!scratchOffsetRead||!withinBits||
               *withinBits!=std::uint32_t(*deviceCursorRead-std::uint32_t(*scratchOffsetRead))||
               withinSigned!=((*withinBits&0x80000000u)?std::int64_t(*withinBits)-0x100000000ll:std::int64_t(*withinBits))||
               cacheHitBranch!=(*withinSigned>=0&&*withinSigned<0x6000))return false;
            const auto returnUnissued=[&]()noexcept {
                return !deviceCursorReload&&!deviceCursorStored&&!returnV0&&!releasedFrameBytes&&!rawReturned;
            };
            if(!*cacheHitBranch)return !copyBytes&&!copied&&returnUnissued();
            const auto available=0x6000u-static_cast<std::uint32_t>(*withinSigned);
            if(copyBytes!=(*request<available?*request:available))return false;
            if(!copied)return returnUnissued(); //Native copy admission may refuse; no source zero result.
            if(copied!=*copyBytes||deviceCursorReload!=position||!deviceCursorStored)return false;
            if(copied!=*request)return !returnV0&&!releasedFrameBytes&&!rawReturned;
            return returnV0==request&&releasedFrameBytes==0xf0u&&rawReturned;
        }
    } lineTransfer_;
    bool lineSnapshotTaken_=false,activationSnapshotTaken_=false;
    bool lineTransferHistoryCurrentNative()const noexcept;
    struct LineSuffix final {
        bool entered=false;
        const void* outputIdentity=nullptr;
        std::optional<std::uint32_t> rawV0Read;
        std::optional<bool> nonzeroBranch;
        std::optional<std::uint32_t> requestRead;
        std::optional<std::uint32_t> requestTerminatorOffset;
        bool requestTerminatorWritten=false;
        std::optional<std::uint32_t> scanOffset;
        std::size_t scanReads=0;
        std::uint32_t scanIncrements=0;
        std::array<std::uint8_t,256> scanBytes;
        std::optional<std::int32_t> lastSignedByte;
        std::optional<std::int32_t> rereadByte;
        std::optional<bool> foundLF;
        bool pointerCleared=false;
        bool differenceIssued=false;
        std::optional<std::uint32_t> difference;
        std::optional<std::uint32_t> consumed;
        std::optional<bool> previousByteInRange;
        std::optional<std::int32_t> precedingRead;
        std::optional<bool> precedingNotCRBranch;
        std::optional<std::uint32_t> crWriteOffset;
        bool crStored=false;
        std::optional<std::uint32_t> lineTerminatorOffset;
        bool lineTerminatorWritten=false;
        std::optional<std::uint32_t> memberCursorRead;
        const void* sourcePointerV0=nullptr;
        std::optional<std::uint32_t> memberCursorSum;
        bool memberStored=false;
        std::array<bool,9> restoresIssued{};
        std::optional<std::uint32_t> restoredReturnPC;
        const void* restoredS2=nullptr;
        std::optional<std::int64_t> restoredS1;
        std::optional<std::uint32_t> restoredS0;
        std::optional<std::uint32_t> releasedFrameBytes;
        bool lineReturned=false;
        bool unissued()const noexcept {
            for(const bool restored:restoresIssued)if(restored)return false;
            return !entered&&!outputIdentity&&!rawV0Read&&!nonzeroBranch&&!requestRead&&
                !requestTerminatorOffset&&!requestTerminatorWritten&&!scanOffset&&!scanReads&&!scanIncrements&&
                !lastSignedByte&&!rereadByte&&!foundLF&&!pointerCleared&&!differenceIssued&&
                !difference&&!consumed&&!previousByteInRange&&!precedingRead&&!precedingNotCRBranch&&
                !crWriteOffset&&!crStored&&!lineTerminatorOffset&&!lineTerminatorWritten&&!memberCursorRead&&
                !sourcePointerV0&&!memberCursorSum&&!memberStored&&!restoredReturnPC&&!restoredS2&&
                !restoredS1&&!restoredS0&&!releasedFrameBytes&&!lineReturned;
        }
        bool historyCurrent()const noexcept {
            if(!entered)return unissued();
            if(!outputIdentity||rawV0Read!=255u||nonzeroBranch!=true||requestRead!=255u||requestTerminatorOffset!=requestRead||
               !requestTerminatorWritten||!scanReads||scanReads>scanBytes.size()||!scanOffset||*scanOffset!=scanReads-1u||
               scanIncrements!=*scanOffset)return false;
            const auto signedByte=[](std::uint8_t b)noexcept {return b<128u?std::int32_t(b):std::int32_t(b)-256;};
            for(std::size_t n=0;n+1u<scanReads;++n)if(scanBytes[n]==0u||scanBytes[n]==10u)return false;
            const auto last=scanBytes[scanReads-1u];
            if((last!=0u&&last!=10u)||(scanReads==256u&&last!=0u)||lastSignedByte!=signedByte(last)||rereadByte!=lastSignedByte||
               foundLF!=(last==10u)||pointerCleared==*foundLF||!differenceIssued)return false;
            if(*foundLF){
                if(difference!=scanOffset||consumed!=*difference+1u||previousByteInRange!=(*scanOffset>0u)||!lineTerminatorWritten)return false;
                if(*previousByteInRange){
                    if(precedingRead!=signedByte(scanBytes[*scanOffset-1u])||precedingNotCRBranch!=(*precedingRead!=13))return false;
                    if(!*precedingNotCRBranch){
                        if(!crStored||crWriteOffset!=*scanOffset-1u||lineTerminatorOffset!=scanOffset)return false;
                    }else if(crStored||crWriteOffset||lineTerminatorOffset!=*scanOffset+1u)return false;
                }else if(precedingRead||precedingNotCRBranch||crStored||crWriteOffset||lineTerminatorOffset!=*scanOffset+1u)return false;
            }else if(difference||consumed!=requestRead||previousByteInRange||precedingRead||precedingNotCRBranch||
                     crStored||crWriteOffset||lineTerminatorOffset||lineTerminatorWritten)return false;
            if(memberCursorRead!=0u||sourcePointerV0!=outputIdentity||memberCursorSum!=std::uint32_t(*memberCursorRead+*consumed)||
               !memberStored||restoredReturnPC!=0x157768u||!restoredS2||!restoredS1||*restoredS1<0||restoredS0!=0u||
               releasedFrameBytes!=0x90u||!lineReturned)return false;
            for(const bool restored:restoresIssued)if(!restored)return false;
            return true;
        }
    } lineSuffix_;
    bool suffixSnapshotTaken_=false;
    bool lineSuffixHistoryCurrentNative()const noexcept;
    bool attempted_=false,acquired_=false,registered_=false,returned_=false,closed_=false,nativeClosed_=false;
    bool flushAttempted_=false,flushReturned_=false,flushHandleCall_=false,closeAttempted_=false;
    bool sourceLooseSlotsRetired_=false; //Only actual1311FC/131208 completion, never native cancellation.
    OriginalMusicSourceFile50LA(const original_main_entry50KG::Application*,
        const rr::game::OriginalRaceMusicManager50LA*)noexcept;
    OriginalMusicSourceFile50LA(const original_main_entry50KG::Application*,
        const rr::game::OriginalRaceMusicManager50LA*,Occurrence)noexcept;
    std::string_view sourceNameNative()const noexcept;
    bool nativeAnnouncerCustodyCurrent(const original_main_entry50KG::Application*,
        const rr::game::OriginalRaceMusicManager50LA*,const void* exactConfig,const R* exactResolver)const noexcept;
    bool begin131230(std::shared_ptr<R>,const void* exactConfig,const std::filesystem::path& actualAudioRoot,
        std::string_view exactName,std::int64_t& returnedSourceHandle,D&)noexcept;
    bool current(const void* exactConfig,std::int64_t handle)const noexcept;
    bool readAnnouncerProbe12FB30(const void* exactConfig,std::int64_t handle,void* exactStack48,
        std::size_t& copied,D&)noexcept;
    bool read12FB30(const void* exactConfig,std::int64_t handle,void*,std::size_t,std::size_t&,D&)noexcept;
    bool seekAnnouncerText12F640(const void* exactConfig,std::int64_t handle,D&)noexcept;
    bool beginAnnouncerTextLine12EE90(const void* exactConfig,std::int64_t handle,
        void* exactParserFrame450,D&)noexcept;
    bool transferAnnouncerLine12EFC8(const void* exactConfig,std::int64_t handle,
        void* exactParserFrame450,std::uint32_t capacity,D&)noexcept;
    bool finishAnnouncerLine12F1B4(const void* exactConfig,std::int64_t handle,
        void* exactParserFrame450,std::uint32_t capacity,std::uint32_t savedLineOrdinal,
        std::uint32_t savedCallerReturnPC,D&)noexcept;
    bool seekSet12F640(const void* exactConfig,std::int64_t handle,std::uint32_t position,D&)noexcept;
    bool flush12F290(const void* exactConfig,std::int64_t handle,std::int32_t& returned,D&)noexcept;
    bool close131040(const void* exactConfig,std::int64_t handle,D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalMusicSourceFile50LA()noexcept;
    OriginalMusicSourceFile50LA(const OriginalMusicSourceFile50LA&)=delete;
    OriginalMusicSourceFile50LA& operator=(const OriginalMusicSourceFile50LA&)=delete;
};
}
