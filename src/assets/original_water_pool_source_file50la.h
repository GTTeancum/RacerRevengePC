#pragma once
#include "original_standard_texture_resource50la.h"
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
class OriginalStandardArchiveProvider50LA;
class OriginalRaTrackSourceFile50LA;
//Private custody for one bounded2BAF90 invocation. This is neither a second
//FileManager nor a Pool Resource, Entity reference, model kind or cache owner.
class OriginalWaterPoolSourceFile50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    struct Comparison {
        std::int32_t row=-1;const char* filenameIdentity=nullptr;
        bool fast=false,returned=false,equal=false;
        std::size_t equalPrefix=0,foldReads=0;
        unsigned char left=0,right=0;
        //Four distinct reads on a mismatching non-NUL pair: the loop pair,
        //then the final subtraction pair. A refused read leaves its bit clear.
        std::array<std::int32_t,4> folded{};std::uint8_t foldedMask=0;
        std::uint32_t pc=0;
    };
    struct DeviceObservation {
        std::uint32_t cursor08=0,flags0C=0;
        const void* handleIdentity=nullptr;std::size_t encodedCursor=0;
    };
    struct ServiceObservation {
        std::uint32_t memberCursor10=0,memberFlags18=0;
        std::int32_t memberAuxiliary04=0,selected148=-1;
        std::array<std::int32_t,4> active{{-1,-1,-1,-1}};
        std::int32_t current74=-1,window7C=-1,window80=-1,encodedWindowDevice=-1;
        std::uint32_t read174=0;std::size_t windowBytes=0;
        const void* lengthTableIdentity=nullptr;
        std::array<std::int64_t,8> scratchStatus{};
        std::array<std::int32_t,8> scratchOffset{};
        std::vector<DeviceObservation> devices; //Sized BEFORE the observed source call.
    };
    struct VersionRead {
        //No prior source write toP+1EC. Never copy/read the unknown suffix.
        std::array<std::uint8_t,4> bytes;
        std::array<bool,4> known{{false,false,false,false}};
        bool attempted=false,nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::optional<std::int32_t> sourceItems;
        ServiceObservation before,after;D diagnostic{};
        VersionRead()noexcept {} //Do not zero-fill the source destination.
    };
    struct ChildCountRead {
        //The aggregate admits ONLY the actual constructed resource's R+14
        //destination.2BB15C already wrote zero to all four bytes. Native
        //bookkeeping records that preimage; it is not an allocator zero-fill.
        //Until entered binds resourceIdentity, these defaults assert no source
        //construction, destination write, known resource bytes or provenance.
        const void* resourceIdentity=nullptr;
        std::array<std::uint8_t,4> bytes{};
        std::array<bool,4> known{{true,true,true,true}};
        bool entered=false,prepared=false,beforeObserved=false,attempted=false;
        bool nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::optional<std::int32_t> sourceItems;
        ServiceObservation before,after;D diagnostic{};
        std::uint32_t pc=0; //Separate descendant: version/file pc_ is immutable.
    };
    struct MaterialReadSpec {
        std::uint32_t serviceLuiPC=0,serviceLoadPC=0,destinationPC=0,sizePC=0,countPC=0;
        std::uint32_t callPC=0,delayPC=0,returnPC=0,offset=0,size=0;
        std::uint32_t knownPrefix=0,preimageWord=0;
    };
    struct MaterialRead {
        MaterialReadSpec source;
        //Zero bookkeeping lanes with mask zero are NOT destination writes.
        //Only traced preimages and the actual copied prefix are consumed.
        std::array<std::uint8_t,64> preimageMask{},preimageBytes{},knownMask{},bytes{};
        bool entered=false,beforeObserved=false,attempted=false,callIssued=false,delayIssued=false;
        bool nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::optional<std::int64_t> argumentHandle;
        std::optional<std::int32_t> sourceItems;
        std::uint32_t pc=0,helperEntry=0;
        ServiceObservation before,after;
        D helperDiagnostic{},admissionDiagnostic{};
    };
    struct MaterialReader {
        bool entered=false,prepared=false;
        const void* materialIdentity=nullptr;const void* arrayIdentity=nullptr;const void* dataIdentity=nullptr;
        std::uint64_t materialEpoch=0,arrayEpoch=0,dataEpoch=0;
        std::optional<std::int64_t> savedHandleS2;
        std::array<MaterialRead,18> reads;
        D preparationDiagnostic{};
    };
    enum class TopologyEntryKind {none,returnedFirstMaterial,nonpositiveCount};
    struct TopologyCountRead {
        //Only Owner can admit the genuine Data construction and branch. This
        //receipt owns no resource, reference, topology storage or heap result.
        TopologyEntryKind entryKind=TopologyEntryKind::none;
        const void* dataIdentity=nullptr;const void* fileIdentity=nullptr;
        std::uint64_t dataEpoch=0;
        MaterialReadSpec source;
        std::uint32_t preimagePC=0,itemCount=0;
        std::array<std::uint8_t,4> preimageMask{},knownMask{},preimageBytes{},bytes{};
        bool entered=false,prepared=false,beforeObserved=false,attempted=false;
        bool callIssued=false,delayIssued=false,nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::optional<std::int64_t> sourceHandle,argumentHandle,savedHandleS2;
        std::optional<std::int32_t> sourceItems;
        std::uint32_t pc=0,helperEntry=0;
        ServiceObservation before,after;
        D preparationDiagnostic{},helperDiagnostic{},admissionDiagnostic{};
    };
    struct IndexRead {
        std::array<std::uint8_t,4> bytes{}; //Uncopied bytes are unknown, not guest zeroes.
        bool entered=false,prepared=false,beforeObserved=false,attempted=false;
        bool nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::uint32_t ordinal=0,pc=0;
        std::optional<std::int32_t> sourceItems;
        ServiceObservation before,after;
        D helperDiagnostic{},admissionDiagnostic{};
    };
    struct IndexReader {
        bool entered=false,prepared=false;
        const void* dataIdentity=nullptr;const void* arrayIdentity=nullptr;
        std::uint64_t dataEpoch=0;std::uint32_t count=0;
        std::vector<IndexRead> reads;
    } indexReader_;
    struct FirstTopologyRecordRead {
        IndexRead read;
        const void* topologyIdentity=nullptr;
        const void* dataIdentity=nullptr;
        std::uint64_t dataEpoch=0;
    } firstTopologyRecord_,firstTopologyElement_;
    struct InnerCountRead {
        IndexRead read;
        const void* dataIdentity=nullptr;
        const void* objectIdentity=nullptr;
        std::uint64_t dataEpoch=0;
    } innerCount_;
    struct InnerRowCountRead {
        IndexRead read;
        const void* dataIdentity=nullptr;
        const void* objectIdentity=nullptr;
        const void* rowsIdentity=nullptr;
        std::uint64_t dataEpoch=0;
    } innerRowCount_;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    //This is admission's SAME service Track relation, not a presumed winner.
    std::shared_ptr<OriginalRaTrackSourceFile50LA> track_;
    const void* poolIdentity_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    const void* freshIdentity_=nullptr;const void* membersIdentity_=nullptr;
    const void* devicesIdentity_=nullptr;
    std::int32_t memberCount_=0,deviceCount_=0;
    std::size_t serviceTrackMount_=static_cast<std::size_t>(-1);
    const void* serviceMountIdentity_=nullptr;
    std::uint64_t serviceTrackGeneration_=0;
    std::string name_;std::vector<Comparison> comparisons_;
    std::int64_t before160_=-1,before168_=-1,after160_=-1,after168_=-1;
    bool pinOwned_=false,nativeClosed_=false,searchReached_=false,searchReturned_=false;
    bool rotatingAdvanced_=false,rotatingWrapped_=false,rotatingMatchStored_=false,cachedStored_=false;
    bool fastMatch_=false,found_=false;
    std::optional<std::uint32_t> missMode170_;
    std::optional<std::int32_t> sourceReturnV0_;
    std::int32_t row_=-1,device_=-1;
    const void* memberIdentity_=nullptr;const char* filenameIdentity_=nullptr;
    const void* selectedRoot_=nullptr;const void* selectedMountIdentity_=nullptr;
    std::size_t selectedMount_=static_cast<std::size_t>(-1);
    std::uint64_t selectedGeneration_=0;
    std::uint32_t selectedLength_=0;
    bool associationObserved_=false,warmObserved_=false;
    const void* opaqueAssociation_=nullptr; //History only; never dereferenced.
    bool warmT1ZeroBranch_=false;std::optional<std::int64_t> warmDelayV0_;
    bool openAttempted_=false,openReturned_=false;
    D openDiagnostic_{};
    std::int32_t nativeHandle_=-1; //May be live BEFORE source output stores.
    std::optional<std::int64_t> sourceHandle1E0_;
    std::optional<const void*> sourceAssociated1E8_;
    bool outputHandleStored_=false,outputAssociationStored_=false;
    VersionRead read_;
    ChildCountRead countRead_;
    MaterialReader materialReader_; //Neither version nor count history is repurposed.
    TopologyCountRead topologyRead_; //Independent one-shot Data+1C descendant.
    bool sourceCloseAttempted_=false,sourceCloseReturned_=false;
    std::optional<std::int32_t> sourceCloseV0_;
    D sourceCloseDiagnostic_{};
    bool nativeCloseAttempted_=false,nativeHandleClosed_=false;
    D nativeCloseDiagnostic_{};
    std::uint32_t pc_=0;
    OriginalWaterPoolSourceFile50LA()noexcept=default;
    static bool begin132820(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,
        std::shared_ptr<OriginalRaTrackSourceFile50LA>,const void* poolIdentity,
        const std::string& exactName,std::shared_ptr<OriginalWaterPoolSourceFile50LA>& pending,D&)noexcept;
    bool compare308F00(Comparison&,bool&)noexcept;
    bool observeServiceNative(ServiceObservation&)const noexcept;
    bool historyNative(const void* poolIdentity)const noexcept;
    bool preflightNative(const void* poolIdentity)const noexcept;
    bool canCloseNative(const void* poolIdentity)const noexcept;
    //True means the helper result passed native admission. nativeReturned is
    //the exact helper Boolean, separately retained even on a later refusal.
    //sourceItems for a successful clamped1..3 byte transfer is still1.
    bool readVersion12FB30(D&)noexcept;
    //One-shot child count from the SAME still-open occurrence. Only aggregate
    //Owner admits the constructed resource, its typed Pool alias and R+14
    //preimage, then synchronizes the copied prefix into that actual payload.
    //Preparation refusal invokes no source service. Neither native Boolean
    //nor known destination bytes alone establish a source return/LW receipt.
    bool readChildCount12FB30(const void* resourceIdentity,D&)noexcept;
    static const std::array<MaterialReadSpec,18>& materialReadScheduleNative()noexcept;
    bool materialReaderHistoryNative()const noexcept; //Values/opaque identities only; no aggregate callbacks.
    bool beginMaterialReaderNative(const void* material,const void* array,const void* data,
        std::uint64_t materialEpoch,std::uint64_t arrayEpoch,std::uint64_t dataEpoch,D&)noexcept;
    bool readMaterial12FB30(std::size_t ordinal,D&)noexcept;
    bool indexReadHistoryNative()const noexcept;
    bool beginIndexReaderNative(const void*,const void*,std::uint64_t,std::uint32_t,D&)noexcept;
    bool readIndex12FB30(std::uint32_t,D&)noexcept;
    bool firstTopologyRecordHistoryNative()const noexcept;
    bool readFirstTopologyRecord12FB30(const void*,const void*,std::uint64_t,D&)noexcept;
    bool firstTopologyElementHistoryNative()const noexcept;
    bool readFirstTopologyElement12FB30(const void*,const void*,std::uint64_t,D&)noexcept;
    bool innerCountHistoryNative()const noexcept;
    bool readInnerCount12FB30(const void*,const void*,std::uint64_t,D&)noexcept;
    bool innerRowCountHistoryNative()const noexcept;
    bool readInnerRowCount12FB30(const void*,const void*,const void*,std::uint64_t,D&)noexcept;
    bool topologyReadHistoryNative()const noexcept; //Leaf: values and opaque identities only.
    bool readTopologyCount12FB30(const void* data,std::uint64_t dataEpoch,TopologyEntryKind,D&)noexcept;
    //Only the complete non-2 version branch can issue this source close.
    //Caller independently proves fresh2BB10C service load and reached branch.
    bool closeSource131040(D&)noexcept;
    //Idempotent native withdrawal: exact handle first, independent pin last.
    //Never records2BB108/2BB270 or rewrites historical source output handle.
    bool closeNative(D&)noexcept;
    std::optional<std::uint32_t> versionWordNative()const noexcept;
    std::uint32_t pc()const noexcept{return pc_;}
    std::int32_t nativeHandle()const noexcept{return nativeHandle_;}
    bool nativeClosed()const noexcept{return nativeClosed_;}
public:
    ~OriginalWaterPoolSourceFile50LA()noexcept;
    OriginalWaterPoolSourceFile50LA(const OriginalWaterPoolSourceFile50LA&)=delete;
    OriginalWaterPoolSourceFile50LA& operator=(const OriginalWaterPoolSourceFile50LA&)=delete;
};
}
