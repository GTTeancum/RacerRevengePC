#pragma once
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <thread>
namespace original_main_entry50KG {struct Application;}
namespace rr::assets {class OriginalUiArchive50KG;class OriginalStandardMaterialDomain50LA;class OriginalStandardTextureResolver50LA;class OriginalStandardArchiveProvider50LA;class OriginalStandardMaterialTable50LA;class OriginalStandardProgramTable50LA;class OriginalUiMaterialApplication50LA;}
namespace rr::game {
class OriginalApplicationInput04Source50LA;
class OriginalApplicationVideoSource50LA;
class OriginalApplicationStartupCall23A0DCNative50LA;
// Same-Application resumable prefix. Normal native entry is an explicit
// adaptation, not a claim that guest argc/argv or17C910's return are owned.
class OriginalApplicationStartup17C910Source50LA final {
    friend struct ::original_main_entry50KG::Application;
    using Application=::original_main_entry50KG::Application;
public:
    enum class Phase : std::uint8_t {absent,argumentsPending,inputPending,directories,collisionDirectoryPending,
        trackDirectoryPending,appStartInputPending,appStartRegistrationBlocked,appStartRegistered,
        appStartNativeMountBlocked,appStartNamesReadPending,appStartNamesReadBlocked,appStartNamesParsePending,
        appStartNameRowsBlocked,appStartNameRowsPending,appStartMaterialPopulationBlocked,appStartMaterialNamesReturned,
        appStartDefaultTemplateBlocked,appStartDefaultTemplateReturned,
        appStartScreenTemplateBlocked,appStartScreenTemplateReturned,inputMappingsBlocked,inputMappingsReturned};
    enum class Result : std::uint8_t {
        authority,phase,argumentsUnowned,nativeAllocationPending,inputNativePending,
        inputPrefixBlocked,collisionDirectoryPending,collisionDirectoryUsed,appStartInputPending,
        appStartInputUnavailable,appStartRegistrationBlocked,appStartRegistered,
        appStartNativeMountBlocked,appStartNamesReadBlocked,appStartNamesParsePending,
        appStartNameRowsBlocked,appStartNameRowsPending,appStartMaterialPopulationBlocked,appStartMaterialNamesReturned,
        appStartDefaultTemplateBlocked,appStartDefaultTemplateReturned,
        appStartScreenTemplateBlocked,appStartScreenTemplateReturned,inputMappingsBlocked,inputMappingsReturned,
        nativeBodyResultCallerPending,nativeBodyResultConsumed
    };
    struct Observation {
        Phase phase=Phase::absent;
        bool entered=false,nativeNormalEntry=false,inputCallIssued=false,inputReturnObserved=false,nativeClosed=false;
        std::uint32_t sourcePC=0,lastInputPC=0;
        unsigned directoryCopies=0;
        bool appStartInputRetained=false,appStartRegistrationAttempted=false,appStartRegistrationReturned=false;
        bool appStartNativeMountAttempted=false,appStartNativeMountReturned=false;
        bool appStartNamesCallIssued=false,appStartNamesReadReturned=false;
        bool appStartNameRowsCallIssued=false,appStartNameRowsPublished=false;
        bool appStartMaterialPopulationCallIssued=false,appStartMaterialNamesReturned=false;
        bool appStartNativeProgramsAttempted=false,appStartNativeProgramsReturned=false;
        bool appStartDefaultTemplateCallIssued=false,appStartDefaultTemplateReturned=false;
        bool appStartScreenTemplateCallIssued=false,appStartScreenTemplateReturned=false;
        bool inputMappingsAttempted=false,inputMappingsReturned=false;
        unsigned inputMappingCallsIssued=0,inputMappingCallsReturned=0,inputMappingWordsWritten=0;
        //Native body outcome only. No saved guest frame, source return or
        //outer23A0DC continuation is established by observing this value.
        bool nativeBodyResultKnown=false;
        std::uint32_t nativeBodyResult=0;
        bool nativeCallerBound=false,nativeAbiReturnConsumed=false;
        std::uint32_t lastMaterialPC=0;
    };
private:
    const std::thread::id thread_=std::this_thread::get_id();
    const Application* application_=nullptr;
    Phase phase_=Phase::absent;
    bool nativeNormalEntry_=false,inputReturnObserved_=false,nativeClosed_=false;
    // Pin the exact descendant created by THIS call, including NULL backing
    // and partial failures. Never adopt an unrelated prior input constructor.
    std::shared_ptr<OriginalApplicationInput04Source50LA> input_;
    std::uint32_t pc_=0,lastInputPC_=0;
    unsigned directoryCopies_=0;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> appStart_;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> appStartDomain_;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> appStartResolver_;
    bool appStartRegistrationAttempted_=false,appStartRegistrationReturned_=false;
    std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA> appStartProvider_;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> appStartMaterials_;
    std::uint64_t appStartProviderRevision_=0;
    std::uint32_t lastMaterialPC_=0;
    bool appStartNativeMountAttempted_=false,appStartNativeMountReturned_=false;
    bool appStartNamesCallIssued_=false,appStartNamesReadReturned_=false;
    bool appStartNameRowsCallIssued_=false,appStartNameRowsPublished_=false;
    bool appStartMaterialPopulationCallIssued_=false,appStartMaterialNamesReturned_=false;
    std::shared_ptr<rr::assets::OriginalStandardProgramTable50LA> appStartPrograms_;
    std::shared_ptr<rr::assets::OriginalUiMaterialApplication50LA> appStartTemplateApplication_;
    bool appStartNativeProgramsAttempted_=false,appStartNativeProgramsReturned_=false;
    bool appStartDefaultTemplateCallIssued_=false,appStartDefaultTemplateReturned_=false;
    bool appStartScreenTemplateCallIssued_=false,appStartScreenTemplateReturned_=false;
    bool inputMappingsAttempted_=false,inputMappingsReturned_=false;
    unsigned inputMappingCallsIssued_=0,inputMappingCallsReturned_=0,inputMappingWordsWritten_=0;
    bool nativeBodyResultKnown_=false;
    std::uint32_t nativeBodyResult_=0;
    //Bound BEFORE this body's first entry by the actual private native call
    //issuer. Never adopted from an existing body/result or a caller PC number.
    const OriginalApplicationStartupCall23A0DCNative50LA* nativeCaller_=nullptr;
    bool nativeAbiReturnConsumed_=false;
    OriginalApplicationStartup17C910Source50LA()noexcept=default;
    bool current(const Application& app)const noexcept {
        return thread_==std::this_thread::get_id()&&application_==&app&&!nativeClosed_;
    }
    Result begin(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_)return Result::authority;
        if(phase_!=Phase::absent)return Result::phase;
        application_=&app;phase_=Phase::argumentsPending;pc_=0x17c924;
        //First mutable process read remains unowned. No argc=0/default argv
        //or argument-branch result is synthesized from host CLI/environment.
        return Result::argumentsUnowned;
    }
    bool closeNative(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        if(nativeClosed_)return true;
        input_.reset();appStartTemplateApplication_.reset();appStartPrograms_.reset();
        appStartMaterials_.reset();appStartProvider_.reset();
        appStart_.reset();appStartResolver_.reset();appStartDomain_.reset();nativeClosed_=true;
        //Native pin cancellation only; source PC/history and directory stores
        //remain. No original destructor, rollback, or startup return is issued.
        return true;
    }
    bool inspect(const Application& app,Observation& out)const noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        out={};out.phase=phase_;out.entered=phase_!=Phase::absent;
        out.nativeNormalEntry=nativeNormalEntry_;out.inputCallIssued=lastInputPC_!=0;
        out.inputReturnObserved=inputReturnObserved_;out.nativeClosed=nativeClosed_;
        out.sourcePC=pc_;out.lastInputPC=lastInputPC_;out.directoryCopies=directoryCopies_;
        out.appStartInputRetained=bool(appStart_);
        out.appStartRegistrationAttempted=appStartRegistrationAttempted_;
        out.appStartRegistrationReturned=appStartRegistrationReturned_;
        out.appStartNativeMountAttempted=appStartNativeMountAttempted_;
        out.appStartNativeMountReturned=appStartNativeMountReturned_;
        out.appStartNamesCallIssued=appStartNamesCallIssued_;
        out.appStartNamesReadReturned=appStartNamesReadReturned_;
        out.appStartNameRowsCallIssued=appStartNameRowsCallIssued_;
        out.appStartNameRowsPublished=appStartNameRowsPublished_;
        out.appStartMaterialPopulationCallIssued=appStartMaterialPopulationCallIssued_;
        out.appStartMaterialNamesReturned=appStartMaterialNamesReturned_;
        out.appStartNativeProgramsAttempted=appStartNativeProgramsAttempted_;
        out.appStartNativeProgramsReturned=appStartNativeProgramsReturned_;
        out.appStartDefaultTemplateCallIssued=appStartDefaultTemplateCallIssued_;
        out.appStartDefaultTemplateReturned=appStartDefaultTemplateReturned_;
        out.appStartScreenTemplateCallIssued=appStartScreenTemplateCallIssued_;
        out.appStartScreenTemplateReturned=appStartScreenTemplateReturned_;
        out.inputMappingsAttempted=inputMappingsAttempted_;out.inputMappingsReturned=inputMappingsReturned_;
        out.inputMappingCallsIssued=inputMappingCallsIssued_;out.inputMappingCallsReturned=inputMappingCallsReturned_;
        out.inputMappingWordsWritten=inputMappingWordsWritten_;
        out.nativeBodyResultKnown=nativeBodyResultKnown_;out.nativeBodyResult=nativeBodyResult_;
        out.nativeCallerBound=nativeCaller_!=nullptr;out.nativeAbiReturnConsumed=nativeAbiReturnConsumed_;
        out.lastMaterialPC=lastMaterialPC_;return true;
    }
public:
    ~OriginalApplicationStartup17C910Source50LA()noexcept {
        if(application_&&!nativeClosed_)std::terminate();
    }
    OriginalApplicationStartup17C910Source50LA(const OriginalApplicationStartup17C910Source50LA&)=delete;
    OriginalApplicationStartup17C910Source50LA& operator=(const OriginalApplicationStartup17C910Source50LA&)=delete;
};

//Explicit native ABI adaptation of ONE23A0DC call. Embedded with its body in
//the real Application; neither a guest process frame nor platform-init proof.
class OriginalApplicationStartupCall23A0DCNative50LA final {
    friend struct ::original_main_entry50KG::Application;
    using Application=::original_main_entry50KG::Application;
public:
    enum class Phase : std::uint8_t {absent,issuing,entryBlocked,bodyPending,returned,
        initializationIssuing,initializationBlocked,videoAllocationPending,
        videoConstructionIssuing,videoConstructionBlocked,modeTableCallPending,
        modeTableIssuing,modeTableBlocked,descendantAllocationPending,
        descendantsIssuing,descendantsBlocked,callbacksPending,
        intcRegistrationIssuing,intcRegistrationBlocked,intcSyscallPending,
        intcCustodyIssuing,intcCustodyBlocked,intcCustodyPending};
    enum class Result : std::uint8_t {authority,phase,entryBlocked,bodyPending,nativeReturned,
        initializationBlocked,videoAllocationPending,videoConstructionBlocked,modeTableCallPending,
        modeTableBlocked,descendantAllocationPending,descendantsBlocked,callbacksPending,
        intcRegistrationBlocked,intcSyscallPending,intcCustodyBlocked,intcCustodyPending};
    //Native pending-request custody result only, never a syscall return/ID.
    enum class IntcCustodyOutcome : std::uint8_t {absent,authority,phase,otherOwner,exhausted,held};
    struct Observation {
        Phase phase=Phase::absent;
        bool issued=false,bodyEntryReached=false,nativeClosed=false;
        bool nativeReturnKnown=false;
        std::uint32_t nativeReturnValue=0,nativeContinuationPC=0;
        bool nonzeroBranchConsumed=false,initializationCallIssued=false,initializationBodyEntered=false;
        bool prepare237A40Returned=false,policy237950Returned=false,errorFirstByteKnown=false;
        std::uint32_t initializationSourcePC=0;
        bool allocationArgumentKnown=false;
        std::uint32_t allocationArgumentBytes=0;
        bool videoProducerBound=false,videoConstructionCallIssued=false,videoPublicationObserved=false;
        bool modeTableCallIssued=false,modeTablePrefixObserved=false;
        bool descendantsContinuationIssued=false,descendantsPrefixObserved=false;
        bool intcRegistrationCallIssued=false,intcSyscallBoundaryReached=false,intcCustodyAttempted=false;
        IntcCustodyOutcome intcCustodyOutcome=IntcCustodyOutcome::absent;
        std::uint64_t nativeIntcCustodyGeneration=0;
    };
private:
    const std::thread::id thread_=std::this_thread::get_id();
    const Application* application_=nullptr;
    const OriginalApplicationStartup17C910Source50LA* body_=nullptr;
    Phase phase_=Phase::absent;
    bool bodyEntryReached_=false,nativeClosed_=false,nativeReturnKnown_=false;
    std::uint32_t nativeReturnValue_=0,nativeContinuationPC_=0;
    //239C40's frame is0x470 bytes; argument23A0FC begins at+0x370. Bind a
    //native256-byte tail span without claiming the original declared type.
    //Host zero initialization does not make any source byte known.
    struct ErrorBuffer {std::array<std::uint8_t,0x100> data{},known{};};
    ErrorBuffer errorBuffer_;
    struct Initialization237640 {
        const Application* application=nullptr;
        const OriginalApplicationStartupCall23A0DCNative50LA* caller=nullptr;
        ErrorBuffer* error=nullptr;
        bool issued=false,entered=false,prepareReturned=false,policyReturned=false,allocationArgumentKnown=false;
        std::uint32_t pc=0,prepareResult=0,policyResult=0,allocationArgumentBytes=0;
        const OriginalApplicationVideoSource50LA* video=nullptr;
        bool videoCallIssued=false,videoPublicationObserved=false;
        //25A3E0 remains suspended throughout these bounded CPU prefixes.
        bool modeTableCallIssued=false,modeTablePrefixObserved=false;
        bool descendantsContinuationIssued=false,descendantsPrefixObserved=false;
        bool intcRegistrationCallIssued=false,intcSyscallBoundaryReached=false,intcCustodyAttempted=false;
        IntcCustodyOutcome intcCustodyOutcome=IntcCustodyOutcome::absent;
        std::uint64_t nativeIntcCustodyGeneration=0;
    } initialization_;
    bool nonzeroBranchConsumed_=false;
    //Exact verified leaf bodies. Neither probes a display nor chooses a mode.
    static std::uint32_t prepare237A40()noexcept{return 1;}
    static std::uint32_t policy237950(std::uint32_t)noexcept{return 1;}
    OriginalApplicationStartupCall23A0DCNative50LA()noexcept=default;
    bool current(const Application& app)const noexcept {
        return thread_==std::this_thread::get_id()&&application_==&app&&!nativeClosed_;
    }
    bool initializationAllocationPendingCurrent(const Application& app)const noexcept {
        if(!current(app)||phase_!=Phase::videoAllocationPending||!nonzeroBranchConsumed_||
           nativeContinuationPC_!=0x23a100||!nativeReturnKnown_||nativeReturnValue_!=1||
           initialization_.application!=&app||initialization_.caller!=this||initialization_.error!=&errorBuffer_||
           !initialization_.issued||!initialization_.entered||!initialization_.prepareReturned||
           initialization_.prepareResult!=1||!initialization_.policyReturned||initialization_.policyResult!=1||
           initialization_.pc!=0x2376a0||!initialization_.allocationArgumentKnown||
           initialization_.allocationArgumentBytes!=0x510||errorBuffer_.known[0]!=255||errorBuffer_.data[0]!=0)
            return false;
        for(std::size_t i=1;i<errorBuffer_.known.size();++i)if(errorBuffer_.known[i])return false;
        return true;
    }
    bool closeNative(const Application& app)noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        nativeClosed_=true;return true;
        //Revoke only this native call before closing its body. No source
        //return, unwind,304398 termination or history reset is manufactured.
    }
    bool inspect(const Application& app,Observation& out)const noexcept {
        if(thread_!=std::this_thread::get_id()||(application_&&application_!=&app))return false;
        out={};out.phase=phase_;out.issued=phase_!=Phase::absent;
        out.bodyEntryReached=bodyEntryReached_;out.nativeClosed=nativeClosed_;
        out.nativeReturnKnown=nativeReturnKnown_;out.nativeReturnValue=nativeReturnValue_;
        out.nativeContinuationPC=nativeContinuationPC_;out.nonzeroBranchConsumed=nonzeroBranchConsumed_;
        out.initializationCallIssued=initialization_.issued;out.initializationBodyEntered=initialization_.entered;
        out.prepare237A40Returned=initialization_.prepareReturned;out.policy237950Returned=initialization_.policyReturned;
        out.errorFirstByteKnown=errorBuffer_.known[0]==255;out.initializationSourcePC=initialization_.pc;
        out.allocationArgumentKnown=initialization_.allocationArgumentKnown;
        out.allocationArgumentBytes=initialization_.allocationArgumentBytes;
        out.videoProducerBound=initialization_.video!=nullptr;out.videoConstructionCallIssued=initialization_.videoCallIssued;
        out.videoPublicationObserved=initialization_.videoPublicationObserved;
        out.modeTableCallIssued=initialization_.modeTableCallIssued;
        out.modeTablePrefixObserved=initialization_.modeTablePrefixObserved;
        out.descendantsContinuationIssued=initialization_.descendantsContinuationIssued;
        out.descendantsPrefixObserved=initialization_.descendantsPrefixObserved;
        out.intcRegistrationCallIssued=initialization_.intcRegistrationCallIssued;
        out.intcSyscallBoundaryReached=initialization_.intcSyscallBoundaryReached;
        out.intcCustodyAttempted=initialization_.intcCustodyAttempted;
        out.intcCustodyOutcome=initialization_.intcCustodyOutcome;
        out.nativeIntcCustodyGeneration=initialization_.nativeIntcCustodyGeneration;return true;
    }
public:
    ~OriginalApplicationStartupCall23A0DCNative50LA()noexcept {
        if(application_&&!nativeClosed_)std::terminate();
    }
    OriginalApplicationStartupCall23A0DCNative50LA(const OriginalApplicationStartupCall23A0DCNative50LA&)=delete;
    OriginalApplicationStartupCall23A0DCNative50LA& operator=(const OriginalApplicationStartupCall23A0DCNative50LA&)=delete;
};
}
