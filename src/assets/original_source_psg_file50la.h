#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_ui_archive50kg.h"
#include <memory>
#include <string>
#include <thread>
#include <vector>
namespace rr::game {class OriginalRacerModelSceneOwner50LA;}
namespace rr::assets {
class OriginalRaTrackSourceFile50LA;
class OriginalSourcePsgMissingLookup50LA;
// One native attempt around the actual FileManager132820 PSG lookup/interval.
// Native model ID, provider ordinal and source signed handle are distinct.
class OriginalSourcePsgFile50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalStandardModelGeometry50LA;
    // Native custody only; source132820 itself does not retain Geometry.
    std::shared_ptr<OriginalSourcePsgFile50LA> warmCreatorFile_;
    const OriginalStandardModelGeometry50LA* warmGeometry_=nullptr;
    const rr::game::OriginalRacerModelSceneOwner50LA* warmCreator_=nullptr;
    const void* warmResourceBorrower_=nullptr; // Geometry's exact pending/live ledger only.
    std::size_t warmLookupBorrowers_=0;
    bool returnedWarm_=false,warmLookupWithdrawn_=false;
    bool sourceGeometryWithdrawalAllowedNative(const rr::game::OriginalRacerModelSceneOwner50LA*)const noexcept;
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> foundPredecessor_;
    bool foundPredecessorWithdrawn_=false;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root_;
    OriginalStandardModelFileId50LA modelId_{};
    OriginalUiMemberView50KG expected_{};
    std::string name_;
    std::size_t mount_=static_cast<std::size_t>(-1),ordinal_=static_cast<std::size_t>(-1);
    std::uint64_t generation_=0,revision_=0;
    std::int32_t row_=-1,handle_=-1;
    const std::thread::id thread_=std::this_thread::get_id();
    const OriginalStandardModelGeometry50LA* associatedGeometry_=nullptr;
    const rr::game::OriginalRacerModelSceneOwner50LA* associatedModel_=nullptr;
    bool associationStored_=false,associationWithdrawn_=false,sourceCloseReturned_=false;
    std::int32_t sourceCloseResult_=0;
    bool searchReached_=false,returnedCold_=false,nativeClosed_=false;
    OriginalSourcePsgFile50LA()noexcept=default;
    bool read12FB30(void*,std::size_t,std::size_t& done,OriginalStandardTextureDiagnostic50LA&)noexcept;
public:
    ~OriginalSourcePsgFile50LA()noexcept=default; // Explicit close required before releasing attempt.
    OriginalSourcePsgFile50LA(const OriginalSourcePsgFile50LA&)=delete;
    OriginalSourcePsgFile50LA& operator=(const OriginalSourcePsgFile50LA&)=delete;
    static bool capture132820(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalRaTrackSourceFile50LA>,const std::string&,
        OriginalStandardModelFileId50LA,std::shared_ptr<OriginalSourcePsgFile50LA>&,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    static bool captureWarm132820(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalRaTrackSourceFile50LA>,const std::string&,
        OriginalStandardModelFileId50LA,std::shared_ptr<OriginalSourcePsgFile50LA> creatorFile,
        const OriginalStandardModelGeometry50LA*,const rr::game::OriginalRacerModelSceneOwner50LA*,
        std::shared_ptr<OriginalSourcePsgFile50LA>& pending,OriginalStandardTextureDiagnostic50LA&)noexcept;
    //Continue the exact retained found132988 attempt; no search replay.
    static bool continueFound132988(std::shared_ptr<OriginalSourcePsgMissingLookup50LA>,
        OriginalStandardModelFileId50LA,std::shared_ptr<OriginalSourcePsgFile50LA>& pending,
        OriginalStandardTextureDiagnostic50LA&,std::shared_ptr<OriginalSourcePsgFile50LA> creatorFile={},
        const OriginalStandardModelGeometry50LA* geometry=nullptr,
        const rr::game::OriginalRacerModelSceneOwner50LA* creator=nullptr)noexcept;
    bool returnedWarm()const noexcept{return returnedWarm_;}
    const OriginalStandardModelGeometry50LA* warmGeometry()const noexcept{return returnedWarm_?warmGeometry_:nullptr;}
    const rr::game::OriginalRacerModelSceneOwner50LA* warmPrototypeOwner()const noexcept{return returnedWarm_?warmCreator_:nullptr;}
    bool warmGeometryCurrent(const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*)const noexcept;
    const OriginalSourcePsgFile50LA* creatorSourceFile()const noexcept{return warmCreatorFile_.get();}
    bool hasLiveWarmBorrowersNative()const noexcept{return warmLookupBorrowers_!=0;}
    struct ReadReceipt {
        std::size_t requested=0,done=0,before=0,after=0;
        bool entered=false,returned=false,afterKnown=false,exact=false;
        OriginalStandardTextureDiagnostic50LA diagnostic{};
        std::vector<std::uint8_t> copiedPrefix; // Only done bytes are known source writes.
    };
private:
    ReadReceipt lastRead_;
    bool readTerminal_=false;
public:
    const ReadReceipt& lastReadReceipt()const noexcept{return lastRead_;}
    bool readExactNative(void*,std::size_t,std::size_t& logicalCursor,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool preflightNative()const noexcept;
    bool current()const noexcept;
    bool returnedCold()const noexcept{return returnedCold_;}
    bool nativeClosed()const noexcept{return nativeClosed_;}
    const OriginalSourcePsgMissingLookup50LA* foundPredecessorNative()const noexcept{return foundPredecessor_.get();}
    const OriginalStandardTextureResolver50LA* resolver()const noexcept{return resolver_.get();}
    const OriginalRaTrackSourceFile50LA* root()const noexcept{return root_.get();}
    OriginalStandardModelFileId50LA nativeFile()const noexcept{return modelId_;}
    OriginalUiMemberView50KG expectedMember()const noexcept{return expected_;}
    // Valid after returnedCold()/returnedWarm(); warm t1=0 returns signed64 -1.
    std::int64_t sourceHandle()const noexcept{return returnedCold_?std::int64_t(row_):-1;}
    // Native observation of the12F520 value; does not issue guest tell guards.
    bool cursor12F520(std::size_t&,OriginalStandardTextureDiagnostic50LA&)const noexcept;
    bool associateGeometry1326A0(const std::shared_ptr<OriginalStandardModelGeometry50LA>&,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool associatedGeometryCurrent(const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*)const noexcept;
    bool withdrawGeometryNative(const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeSource131040(OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourceCloseReturned()const noexcept{return sourceCloseReturned_;}
    std::int32_t sourceCloseResult()const noexcept{return sourceCloseResult_;}
    bool associationStored()const noexcept{return associationStored_;}
    bool sourceAssociationStored()const noexcept{return associationStored_;}
    bool nativeCacheAssociated()const noexcept{return associationStored_&&!associationWithdrawn_;}
    bool associationWithdrawnNative()const noexcept{return associationWithdrawn_;}
    // Native withdrawal only: never a receipt for caller26EE90 or load success.
    bool closeNative(OriginalStandardTextureDiagnostic50LA&)noexcept;
};
}
