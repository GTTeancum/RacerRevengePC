// Included once after actual Screen and Application definitions.
#include "original_loading_screen_layout50la.h"
namespace rr::game {
bool OriginalLoadingScreenLayout50LA::currentNative()const noexcept {
    return thread_==std::this_thread::get_id()&&screen_&&screen_->current()&&!nativeClosed_&&
        screen_->layoutAttempt_.get()==this&&screen_->lsiDisplay_&&screen_->lsiDisplayPinned_&&
        (!constructorReturned_||(sourceAllocation_&&sourceIdentity_==sourceAllocation_.get()&&interrupt_.current(*this)));
}
bool OriginalLoadingScreenLayout50LA::admitNative(OriginalLoadingScreen40Mode0LA& screen,
    std::unique_ptr<OriginalLoadingScreenLayout50LA>& output)noexcept try {
    if(output||!screen.current()||!screen.lsiDisplay_||!screen.lsiDisplayPinned_||
       !OriginalLoadingInterrupt3Custody50LA::canAdmitNative())return false;
    auto attempt=std::unique_ptr<OriginalLoadingScreenLayout50LA>(new(std::nothrow) OriginalLoadingScreenLayout50LA(screen));
    if(!attempt)return false;
    attempt->config_.reset(new(std::nothrow) Config);
    if(!attempt->config_)return false;
    // Native archive and path storage admission only; the source mount has
    // NOT been called, and the source malloc/constructor/global are unreached.
    attempt->archive_=screen.readLoadingIconNative();
    if(!attempt->archive_||!screen.actualLoadingUiDirectory()||
       *screen.actualLoadingUiDirectory()!="UI\\Textures\\")return false;
    attempt->bankPath_.reserve(260);
    output=std::move(attempt);return true;
}catch(...){return false;}
bool OriginalLoadingScreenLayout50LA::allocateAndConstruct1CC610(std::uint32_t& pc)noexcept {
    pc=0x1CC610;if(!currentNative()||sourceAllocationAttempted_||!config_||!archive_)return false;
    sourceAllocationAttempted_=true;
    sourceAllocation_.reset(new(std::nothrow) SourceAllocation18); // Actual malloc(0x18).
    sourceIdentity_=sourceAllocation_.get();
    if(!sourceAllocation_)return true; // Constructor skipped; caller still publishesNULL38/calls loader.
    count00_=0;constructorStores_=1; //2CCC60.
    count04_=0;constructorStores_=2; //2CCC68.
    refresh08_=1;constructorStores_=3; //2CCC6C byte store; bytes09..0B remain unknown.
    frames10_=nullptr;constructorStores_=4;
    sourceAnimations0C_=nullptr;constructorStores_=5;
    gs14_=nullptr;constructorStores_=6;
    pc=0x2CCC80;
    if(!interrupt_.publishConstructor2CCC80(*this,sourceIdentity_))return false;
    constructorReturned_=true;pc=0x1CC628;return true;
}
bool OriginalLoadingScreenLayout50LA::load2CCDA0(std::uint32_t& pc)noexcept try {
    pc=0x2CCDA0;if(!currentNative()||!sourceAllocationAttempted_||loaderAttempted_)return false;
    loaderAttempted_=true;
    auto& display=*screen_->lsiDisplay_;
    rr::platform::VehicleWindowDiagnostic50DS renderer;std::int32_t returned=0;
    const auto drain=serviceOriginalRenderer244D60(display.dispatch_,true,display.renderer_,renderer,returned,
        OriginalCameraDrainPolicy::initializedContext); // Actual second pair,2CCDE4(a1=1).
    if(drain.status!=OriginalCameraStartStatus::complete){pc=drain.sourcePC;return false;}
    loaderDrainPhase_=1;pc=0x2CCDFC;
    if(!display.renderer_.finishInitializedContextWork(renderer)||!renderer.contextWorkCompleted){pc=0x245090;return false;}
    loaderDrainPhase_=2;
    pc=0x2CCE18;
    // Actual sprintf("%s%s.lsi",36BB90,"ld_icon") after both renderer calls.
    bankPath_=*screen_->actualLoadingUiDirectory();bankPath_+="ld_icon.lsi";directoryFormatted_=true;
    if(bankPath_!="UI\\Textures\\ld_icon.lsi")return false;
    pc=0x2CCE44;bankCallReached_=true;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic providerDiagnostic;
    rr::assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    if(!config_->mountLoadingIcon12CEA0(screen_->domain_->archiveProvider3E1A70(),screen_->domain_->resolver3E1A70(),
        archive_,providerDiagnostic,fileDiagnostic)){
        if(fileDiagnostic.sourcePC)pc=fileDiagnostic.sourcePC;return false;
    }
    pc=0x2CCE50;
    if(!loadAnimations2CDAD0(constructorReturned_,pc))return false;
    // Actual Config failure returns0. Bound before the original assertion
    // first-use interval; do not convert it into Layout/Screen success.
    if(animationsReturned_==0){pc=0x2CCE60;return false;}
    pc=0x2CCED4;return false; // No registration/resources without a true loader return.
}catch(...){return false;}
bool OriginalLoadingScreenLayout50LA::preflightNativeClose()const noexcept {
    if(thread_!=std::this_thread::get_id())return false;
    if(nativeClosed_)return true;
    return currentNative()&&preflightAnimationsNative()&&interrupt_.preflightNativeClose(*this);
}
bool OriginalLoadingScreenLayout50LA::closeNative()noexcept {
    if(!preflightNativeClose())return false;if(nativeClosed_)return true;
    // Actual automatic Config may still be live at a partial getter/arithmetic
    // stop. Close its real member/bank before discarding any parser backing.
    if(config_&&!config_->closeNative())return false;
    if(!closeAnimationsNative())return false;
    if(!interrupt_.closeNative(*this))return false;
    config_.reset();archive_.reset();sourceAllocation_.reset();nativeClosed_=true;
    // Preserve sourceIdentity/Screen38/global4442F0 historical aliases. This is
    // native partial cleanup, not source2CCC90 Disable/Remove/array destructor.
    return true;
}
}
