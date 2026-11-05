#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include "original_config_process_globals50la.h"
#include "../game/original_text_config_primitives50la.h"
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
//The distinct2B9314 stack+240 Config. Neither a Particle occurrence nor a
//prepared parser/file receipt can enter it. Source bytes survive native abort.
class OriginalWaterSourceConfig50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    using Text=rr::game::OriginalTextConfigPrimitives50LA;
    struct Source {
        std::int64_t file00=-1;
        std::uint32_t binary08=0,opened0C=0,mode10=0,count14=0,index18=0;
        const void* text1C=nullptr;std::uint32_t selected20=0;const void* child24=nullptr;
        std::array<char,1024> name28; //Only byte0 is written by157354.
        std::uint32_t field428=0,field42C=0,field430=0,field434=0,cacheCount438=0;
        std::array<std::uint32_t,32> cache43C{}; //Actual306478(...,0,128).
        const void* names4CC=nullptr;const void* index4BC=nullptr;
        std::uint32_t indexBytes4C0=0;const void* block4C4=nullptr;std::uint32_t field4C8=0;
        Source()noexcept {name28[0]=0;}
    } source_;
    const void* const waterReceiver_; //Exact already-constructed source Entity.
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<OriginalRaTrackSourceFile50LA> track_;
    const void* selectedRoot_=nullptr;
    std::string filename_;
    std::int32_t handle_=-1;
    bool attempted_=false,borrowed_=false,nativeClosed_=false;
    std::uint64_t trackGeneration_=0;
    std::uint32_t pc_=0x1573a4;
    std::optional<std::int64_t> before168_,after168_,managerHandle_;
    std::optional<std::uint32_t> missMode170_;
    std::int32_t memberCount_=0;std::uint32_t rowsCompared_=0;
    bool searchReturned_=false,found_=false,managerReturned_=false;
    std::optional<bool> openReturned_;
    struct Read {
        bool attempted=false,returned=false;
        std::size_t copied=0;
        std::optional<std::size_t> before,after;
        std::array<std::uint8_t,7> bytes; //Only copied prefix is known.
    } magic_,version_,count_;
    std::optional<std::size_t> expectedCursor_;
    bool magicTerminated_=false;
    std::optional<std::int32_t> magicCompare_;
    std::optional<std::uint32_t> versionWord_,countWord_,allocationLow32_;
    std::optional<std::int64_t> allocationArgument_;
    bool seekAttempted_=false,seekReturned_=false,textCallReached_=false;
    bool rejectionCloseAttempted_=false,rejectionCloseReturned_=false;
    Text::Table textTable_;
    Text::Bytes textInput_,textLine_,textName_;
    enum class TextPhase {unentered,reading,splitting,section,lookup,allocation,
        copyName,tell,finishRow,duplicate,finishLast,returned};
    TextPhase textPhase_=TextPhase::unentered;
    bool textEntered_=false,textReturned_=false;
    std::uint64_t textCompletedLines_=0,textReadCalls_=0;
    std::uint32_t textLineOrdinal_=0;
    struct TextRead {
        bool attempted=false,transportReturned=false,returned=false,present=false;
        std::optional<std::size_t> before,after;
        std::uint32_t requested=0;std::size_t copied=0;
    } textRead_;
    std::optional<std::int32_t> textLookup_;
    std::optional<std::uint64_t> textTell_;
    std::uint32_t textAllocationsAttempted_=0,textAllocationsReturned_=0;
    std::optional<Text::AllocationRequest> textAllocation_;
    std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> textAllocationReent_;
    enum class NativeAllocationStop {none,reentUnknown,sidecarRefused,backingRefused};
    NativeAllocationStop textAllocationStop_=NativeAllocationStop::none;
    void syncTextFields()noexcept {
        source_.count14=textTable_.count14;source_.index18=textTable_.capacity18;
        source_.text1C=textTable_.sourceStorage;
    }
    bool readTextLine12EE90(Text::Bytes&,TextRead&,D&,std::uint32_t* =nullptr)noexcept;
    bool lookupText157AF0(const Text::Bytes&,std::int32_t&,std::uint32_t* =nullptr)noexcept;
    bool allocateTextBackingNative()noexcept;
    bool parseText157500(D&)noexcept;
    bool preflightTextNative()const noexcept;
    //The first157D00 occurrence belongs to Water's literal General Information.
    enum class SectionPhase {unentered,lookup,firstSeek,copyName,countRead,countSplit,
        secondSeek,tableAllocation,dataAllocation,pointers,storeRead,storeSplit,copyLine,returned};
    SectionPhase sectionPhase_=SectionPhase::unentered;
    bool sectionAttempted_=false,sectionNameCopied_=false;
    std::optional<bool> sectionReturn_;
    std::optional<std::int32_t> sectionIndex_;
    std::optional<std::uint64_t> sectionPosition_;
    std::optional<std::uint32_t> sectionSpan_;
    std::optional<std::size_t> sectionCursor_;
    std::uint32_t sectionSeeksAttempted_=0,sectionSeeksReturned_=0;
    std::array<std::uint32_t,2> sectionReadCalls_{},sectionScanned_{};
    std::uint32_t sectionCounted_=0,sectionStored_=0,sectionPointersWritten_=0;
    Text::Bytes sectionQuery_,sectionInput_,sectionLine_;
    TextRead sectionRead_;
    struct NativeFree {void operator()(void* p)const noexcept {std::free(p);}};
    std::unique_ptr<void,NativeFree> sectionTable_,sectionData_;
    //Stable semantic roles, never malloc addresses or EE pointer bits. This
    //nonmovable Config issues each role once; later allocation epochs must use
    //new tokens. No role or raw backing borrow escapes this private owner.
    struct SectionRole {const OriginalWaterSourceConfig50LA* owner;std::uint64_t epoch;};
    const SectionRole sectionTableRole_{this,1u},sectionDataRole_{this,2u};
    std::unique_ptr<std::uint32_t[]> sectionOffsets_;
    std::unique_ptr<Text::Bytes[]> sectionRows_;
    std::uint32_t sectionCapacity_=0;
    struct SectionAllocation {
        bool attempted=false,returned=false;
        std::uint32_t requested=0;std::size_t nativeBytes=0;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> reent;
        NativeAllocationStop stop=NativeAllocationStop::none;
    } sectionTableAllocation_,sectionDataAllocation_;
    bool sectionSizeBounded_=false;
    bool allocateSectionBackingNative(bool)noexcept;
    bool seekSection12F640(D&)noexcept;
    bool selectGeneralInformation157D00(D&)noexcept;
    bool preflightSectionNative()const noexcept;
    struct WakeSelection {
        bool attempted=false,queryReady=false,seekAttempted=false,seekReturned=false;
        bool childObserved=false,dataObserved=false,freeCallIssued=false,nonnullFreeEntered=false;
        bool lockReturned=false,heapHeaderBoundary=false,nullChildBoundary=false;
        Text::Bytes query;
        std::optional<std::int32_t> index;
        std::optional<std::uint64_t> position;
        std::optional<std::size_t> cursor;
        std::optional<bool> returned;
        const void* childIdentity=nullptr;const void* dataIdentity=nullptr; //Opaque backing roles, NOT EE bits.
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> freeReent;
    } wakeSelection_;
    bool selectWakeFroth157D00(D&)noexcept;
    bool preflightWakeSelectionNative()const noexcept;
    struct NativeWakeRetirement {
        bool attempted=false,dataRetired=false,tableGateReached=false,tableRetired=false;
        bool countCleared=false,childCleared=false;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> dataAdmissionReent,tableAdmissionReent;
        std::optional<std::uint32_t> adaptedResumePC;
        //Original source PC remains30492C. No source free/heap receipt exists.
    } nativeWakeRetirement_;
    bool retireWakeBackingNativeOnly()noexcept;
    bool preflightWakeRetirementNative()const noexcept;
    enum class WakeLoadPhase {unentered,span,copyName,countRead,countSplit,secondSeek,
        tableAllocation,dataAllocation,pointers,storeRead,storeSplit,copyLine,complete};
    struct NativeWakeLoad {
        WakeLoadPhase phase=WakeLoadPhase::unentered;
        std::uint32_t pc=0x157fe8u;
        bool attempted=false,nameCopied=false,seekAttempted=false,seekReturned=false;
        bool sizeBounded=false,nativeLoaded=false;
        std::optional<std::uint32_t> span;
        std::optional<std::uint64_t> seekPosition;
        std::optional<std::size_t> cursor;
        std::array<std::uint32_t,2> readCalls{},scanned{};
        std::uint32_t counted=0,stored=0,pointersWritten=0,capacity=0;
        const void* pointerZeroRole=nullptr;
        Text::Bytes input,line;TextRead read;
        std::unique_ptr<void,NativeFree> table,data;
        std::unique_ptr<std::uint32_t[]> offsets;
        std::unique_ptr<Text::Bytes[]> rows;
        SectionAllocation tableAllocation,dataAllocation;
    } nativeWakeLoad_;
    const SectionRole wakeTableRole_{this,3u},wakeDataRole_{this,4u};
    std::uint32_t currentSectionCountNative()const noexcept {
        if(nativeArrayLoad_.attempted)return nativeArrayLoad_.counted;
        if(nativeArrayRetirement_.countCleared)return 0u;
        return nativeWakeLoad_.attempted?nativeWakeLoad_.counted:
            (nativeWakeRetirement_.countCleared?0u:sectionCounted_);
    }
    const void* currentSectionChildNative()const noexcept {
        if(nativeArrayLoad_.attempted)return nativeArrayLoad_.tableAllocation.returned?&arrayTableRole_:nullptr;
        if(nativeArrayRetirement_.childCleared)return nullptr;
        if(nativeWakeLoad_.attempted)return nativeWakeLoad_.tableAllocation.returned?&wakeTableRole_:nullptr;
        return sectionTableAllocation_.returned&&!nativeWakeRetirement_.childCleared?&sectionTableRole_:nullptr;
    }
    bool allocateWakeBackingNative(bool)noexcept;
    bool loadWakeFrothNativeOnly(D&)noexcept;
    bool preflightWakeLoadNative()const noexcept;
    //Distinct post-formatter157D00 occurrence. Query bytes are observation
    //metadata, NOT source name28 stores. Current Wake backing remains live.
    struct NativeArraySelection {
        enum class Phase {unentered,lookup,position,seek,child,data,freeReent,heap,missingReturned};
        Phase phase=Phase::unentered;std::uint32_t pc=0;
        bool attempted=false,frame=false,stackReleased=false;
        std::uint8_t saveMask=0,restoreMask=0;
        Text::Bytes query;std::size_t queryKnown=0;const char* queryIdentity=nullptr;
        std::optional<std::int32_t> index;
        std::optional<std::uint64_t> position;
        std::optional<std::size_t> cursor;
        std::optional<bool> returned;
        bool seekAttempted=false,seekReturned=false,childObserved=false,dataObserved=false;
        bool freeCallIssued=false,nonnullFreeEntered=false,lockReturned=false,heapHeaderBoundary=false;
        const void* childIdentity=nullptr;const void* dataIdentity=nullptr;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> freeReent;
    } nativeArraySelection_;
    bool selectArraySectionNativeOnly(const char*,std::size_t,D&)noexcept;
    bool preflightArraySelectionNative()const noexcept;
    struct NativeArrayRetirement {
        bool attempted=false,dataRetired=false,tableGateReached=false,tableRetired=false;
        bool countCleared=false,childCleared=false;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> dataAdmissionReent,tableAdmissionReent;
        std::optional<std::uint32_t> adaptedResumePC;
        //Only current Wake raw backing retires; epochs3/4 and row history remain metadata.
    } nativeArrayRetirement_;
    bool retireArraySectionBackingNativeOnly()noexcept;
    bool preflightArrayRetirementNative()const noexcept;
    enum class ArrayLoadPhase {unentered,span,nameLength,copyName,countRead,countSplit,secondSeek,
        tableAllocation,dataAllocation,pointers,storeRead,storeSplit,copyLine,complete};
    struct NativeArrayLoad {
        ArrayLoadPhase phase=ArrayLoadPhase::unentered;std::uint32_t pc=0x157fe8u;
        bool attempted=false,nameLengthProjected=false,nameCopied=false,seekAttempted=false,seekReturned=false;
        bool sizeBounded=false,nativeLoaded=false,selectionStackReleased=false;
        std::uint8_t selectionRestoreMask=0;
        std::optional<std::uint32_t> span,nameLength;
        std::size_t strlenReadWindowMax=0,strncpyReadQuantumMax=0;
        std::optional<std::uint64_t> seekPosition;
        std::optional<std::size_t> cursor;
        std::array<std::uint32_t,2> readCalls{},scanned{};
        std::uint32_t counted=0,stored=0,pointersWritten=0,capacity=0;
        const void* pointerZeroRole=nullptr;
        Text::Bytes input,line;TextRead read;
        std::unique_ptr<void,NativeFree> table,data;
        std::unique_ptr<std::uint32_t[]> offsets;
        std::unique_ptr<Text::Bytes[]> rows;
        SectionAllocation tableAllocation,dataAllocation;
    } nativeArrayLoad_;
    const SectionRole arrayTableRole_{this,5u},arrayDataRole_{this,6u};
    bool projectArrayNameLengthNative()noexcept;
    bool allocateArraySectionBackingNative(bool)noexcept;
    bool loadArraySectionNativeOnly(D&)noexcept;
    bool preflightArrayLoadNative()const noexcept;
    //Private per-occurrence158240 stack state, shared by implementation only.
    //Each getter owns distinct buffers; only its own row loop retains old tails.
    struct ShaderBytes { //Known caller scratch: ShaderName+140 aperture256, filename+790 aperture128.
        std::array<std::uint8_t,256> value;std::array<bool,256> known{};
        void put(std::size_t i,std::uint8_t v)noexcept {value[i]=v;known[i]=true;}
    };
    struct ShaderCopy309230 {
        bool admissionAttempted=false,admitted=false,returned=false;
        std::size_t sourceBegin=0,length=0,readQuantum=0,requiredKnown=0;
        ShaderBytes stores; //Historical copy bytes, not another source object.
    };
    struct FieldLookup {
        bool lookupReturned=false;std::optional<bool> found;
        Text::Bytes scratch,key,output;std::uint32_t row=0;
        std::optional<std::size_t> tokenNext,keyToken,valueToken,valueBegin,valueLength;
        std::size_t tokenCurrent=0,keyLength=0,outputKnown=0;
    };
    struct FloatGetter : FieldLookup {
        bool attempted=false,decimalAttempted=false,decimalAdmitted=false;
        std::optional<bool> returned;
        std::optional<std::uint32_t> result,converted;
        std::optional<std::uint64_t> decimal64;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> reent;
        bool signTableReached=false,powerTableReached=false;
        std::uint32_t numerator=0,denominator=1;std::size_t fractional=0;
        bool negative=false;
    };
    std::array<FloatGetter,20> floatGetters_;
    bool fieldToken29FFA0(FieldLookup&,bool,std::optional<std::size_t>&)noexcept;
    bool fieldKnownLength(const Text::Bytes&,std::size_t,std::size_t&,std::uint32_t)noexcept;
    bool findWaterField158240(FieldLookup&,std::string_view,ShaderBytes* =nullptr,ShaderCopy309230* =nullptr,std::size_t =256u)noexcept;
    bool convertWaterDecimal302E10(FloatGetter&)noexcept;
    bool readWaterFloat158B80(std::size_t,std::optional<std::uint32_t>&,D&)noexcept;
    bool preflightFloatGettersNative()const noexcept;
    bool preflightFloatValueNative(const FloatGetter&,std::uint32_t,std::uint32_t)const noexcept;
    struct IntegerGetter : FieldLookup {
        bool attempted=false,conversionAttempted=false,numericAdmitted=false,booleanAdmitted=false;
        std::optional<bool> innerReturned,returned;
        std::optional<std::uint32_t> result,converted,cutlim,storePC;
        std::optional<std::size_t> booleanProbe;
        std::optional<std::uint64_t> integer64,limit,cutoff;
        std::optional<OriginalConfigProcessGlobals50LA::StdioIdentity> reent;
        std::optional<std::size_t> endOffset;
        std::size_t admittedDigits=0,firstDigit=0,digitsConsumed=0,digitClassifications=0;
        std::uint32_t probesEntered=0,probesReturned=0,base=0;
        std::uint64_t accumulator=0;
        bool negative=false,whitespaceClassified=false,remainderTableReached=false,quotientTableReached=false;
    };
    std::array<IntegerGetter,12> integerGetters_;
    static std::optional<std::size_t> booleanCandidateNative(std::string_view)noexcept;
    bool compareFirst309538(std::uint8_t,std::uint8_t,bool&)noexcept;
    bool integerClassify30AC38(std::uint8_t,std::uint32_t&,std::uint32_t)noexcept;
    bool convertWaterInteger158920(IntegerGetter&)noexcept;
    bool readWaterInteger158B30(std::size_t,std::optional<std::uint32_t>&,D&)noexcept;
    bool preflightIntegerGettersNative()const noexcept;
    bool preflightIntegerValueNative(const IntegerGetter&,std::uint32_t,std::uint32_t)const noexcept;
    struct ShaderGetter : FieldLookup {
        bool attempted=false;std::optional<bool> returned;
        ShaderCopy309230 lookupCopy,defaultCopy;
    } shaderGetter_;
    static std::size_t shaderScratchReadQuantumNative(std::size_t)noexcept;
    static Text::Bytes shaderLiteralNative(bool)noexcept;
    bool copyShader309230(ShaderCopy309230&,const Text::Bytes&,std::size_t,std::size_t,std::size_t,ShaderBytes&)noexcept;
    bool preflightShaderCopyNative(const ShaderCopy309230&,const Text::Bytes&,std::size_t,std::size_t,std::size_t)const noexcept;
    bool readShaderName158AD0(ShaderBytes&,D&)noexcept;
    bool preflightShaderGetterNative()const noexcept;
    bool preflightShaderValueNative(const ShaderGetter&,std::uint32_t,const Text::Bytes&,std::size_t,std::size_t)const noexcept;
    struct NativeWakeGetters {
        bool entered=false,active=false;
        std::uint32_t pc=0x2b97bcu;
        std::size_t completed=0;
        std::array<FloatGetter,4> floats;
        IntegerGetter alpha;
    } nativeWakeGetters_;
    enum class ArrayFilenameSection {wake,array};
    struct NativeArrayFilename {
        bool entered=false,active=false,frame=false,stackReleased=false;
        std::uint8_t saveMask=0,restoreMask=0;
        std::uint32_t pc=0,count=0;
        std::optional<ArrayFilenameSection> section;
        const void* tableIdentity=nullptr;const void* dataIdentity=nullptr;
        const ShaderBytes* destinationIdentity=nullptr;
        std::optional<std::uint32_t> callerReturnPC;
        ShaderGetter value;
    } nativeArrayFilename_;
    static Text::Bytes arrayFilenameDefaultNative()noexcept;
    bool readArrayFilenameNativeOnly(ShaderBytes&,std::uint32_t,D&)noexcept;
    bool preflightArrayFilenameNative()const noexcept;
    struct NativeArrayScalars {
        bool entered=false,active=false,signature=false;
        std::uint32_t pc=0,count=0;std::size_t completed=0;
        std::optional<ArrayFilenameSection> section;
        const void* tableIdentity=nullptr;const void* dataIdentity=nullptr;
        std::array<const void*,14> destinations{}; //Opaque native identities, never EE pointer bits.
        std::array<FloatGetter,10> floats;
        std::array<IntegerGetter,4> integers;
    } nativeArrayScalars_;
    static bool arrayScalarIntegerNative(std::size_t ordinal)noexcept {return ordinal==5u||ordinal>=11u;}
    static std::size_t arrayScalarIndexNative(std::size_t ordinal)noexcept {
        return ordinal==5u?0u:ordinal>=11u?ordinal-10u:ordinal<5u?ordinal:ordinal-1u;
    }
    bool readArrayScalarNative(std::size_t,bool,std::optional<std::uint32_t>&,D&)noexcept;
    bool preflightArrayScalarsNative()const noexcept;
    std::uint32_t& fieldFlowPCNative()noexcept {
        return nativeArrayScalars_.active?nativeArrayScalars_.pc:
            nativeArrayFilename_.active?nativeArrayFilename_.pc:
            nativeWakeGetters_.active?nativeWakeGetters_.pc:pc_;
    }
    bool readWakeSettingNative(std::size_t,std::optional<std::uint32_t>&,D&)noexcept;
    bool preflightWakeGettersNative()const noexcept;
    bool destructorEntered_=false,selectionCleared_=false,destructorReturned_=false;
    explicit OriginalWaterSourceConfig50LA(const void* receiver)noexcept:waterReceiver_(receiver){}
    bool begin1577D0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,std::shared_ptr<OriginalRaTrackSourceFile50LA>,
        const std::string&,D&)noexcept;
    bool compareName308F00(std::string_view,std::string_view,bool&)noexcept;
    bool read12FB30(Read&,std::size_t,std::uint32_t,D&)noexcept;
    bool rejectBinary157968(std::uint32_t,D&)noexcept;
    bool closeRejectedStack1573B0(D&)noexcept;
    bool preflightNative(const void*)const noexcept;
    bool abortNative(D&)noexcept;
public:
    ~OriginalWaterSourceConfig50LA(){if(borrowed_)std::terminate();}
    OriginalWaterSourceConfig50LA(const OriginalWaterSourceConfig50LA&)=delete;
    OriginalWaterSourceConfig50LA& operator=(const OriginalWaterSourceConfig50LA&)=delete;
};
}
