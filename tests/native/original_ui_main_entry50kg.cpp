// Non-template native frontend implementation. Keep ownership and layout in
// the shared declaration header; each body below has one program definition.
#include "original_ui_main_entry50kg.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "../../src/game/original_loading_manager33050la.h"
#include "original_race_display_lifetime50la.h"
#include "original_native_video_packet_invocation50la.inl"
#include "../../src/game/original_loading_screen40_mode050la.inl"
#include "original_course_loading_prefix50la.inl"
#include "../../platform/linux/native/original_ui_course_owned_lifecycle50kg.h"
#include "../../platform/linux/native/original_ui_course_page_payload50kg.inl"
#include "../../src/assets/original_standard_texture_transfer_cold50la.h"
#include "../../src/assets/original_archive_staged_reader50la.h"
#include <fstream>
#include <iostream>
#include <limits>

namespace original_main_entry50KG {

bool Application::sampleAnimationTimestamp(std::uint32_t& out)const noexcept {
        std::uint32_t sampled=0;
        if(!animationClockBackend_||!rr::game::sampleHostTimestamp50ID(*animationClockBackend_,sampled)||
            !publishAnimationTimestamp(*animationClockBackend_,sampled))return false;
        out=sampled;return true;
    }

bool Application::beginUpdateClock140AC0()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||clock.updateInProgress_||clock.updatePublication_==std::numeric_limits<std::uint64_t>::max())return false;
        std::uint32_t sampled=0;
        if(!sampleAnimationTimestamp(sampled))return false;
        //140B10..28 calls140D70 when234==0. That reset samples once into
        //both230/234 and this first update therefore computes zero elapsed.
        if(!clock.previous234_)clock.previous234_=sampled;
        clock.elapsed250_=rr::game::OriginalApplicationAnimationClock50KG::delta140BEC(
            clock.value_.current230,clock.previous234_);
        ++clock.updatePublication_;clock.updateInProgress_=true;return true;
    }

bool Application::commitUpdateClock140D30()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||!clock.updateInProgress_)return false;
        //Source reloads230 here AFTER callbacks. Do not commit a cached entry
        //timestamp if an intervening authentic source sampler updated230.
        clock.previous234_=clock.value_.current230;clock.updateInProgress_=false;return true;
    }

bool Application::resetUpdateClock140D70()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||clock.updateInProgress_)return false;
        std::uint32_t sampled=0;
        if(!sampleAnimationTimestamp(sampled))return false;
        clock.previous234_=sampled; //140E38/3C same sampled value into234/230.
        //140D70 does not write250. Preserve it, with no active publication.
        return true;
    }

bool Application::stageArchiveReader(std::shared_ptr<rr::assets::OriginalArchiveStagedReader50LA>& out) const noexcept {
        if(out||!standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return false;
        const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
        return provider&&provider->issueStagedReader(*this,provider,out);
    }

rr::game::OriginalFrontendCameraCold50LA::Result Application::stageFrontendCameraCold(
        std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& out)const noexcept {
        return rr::game::OriginalFrontendCameraCold50LA::construct(this,entitiesOwner_,out);
    }

rr::game::OriginalFrontendCameraCold50LA::Result Application::stageFrontendCameraViewportPrefix1149D4(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept {
        return owner.stageViewportPrefix1149D4(this,entitiesOwner_);
    }

rr::game::OriginalFrontendCameraCold50LA::Result Application::releaseFrontendCameraCold(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept {
        return owner.release(this,entitiesOwner_);
    }

bool Application::stageUiSearchListCold(
        std::shared_ptr<rr::platform::OriginalUiSearchListCold50KG>& out,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept {
        using S=rr::platform::OriginalUiSearchListColdStatus50KG;d={};
        if(out||!entitiesOwner_||!entitiesOwner_->application10()){
            d={S::input,0x27E260,false};return false;
        }
        try {
            out=std::shared_ptr<rr::platform::OriginalUiSearchListCold50KG>(
                new rr::platform::OriginalUiSearchListCold50KG(*this,entitiesOwner_));
            return out->construct27E260(this,entitiesOwner_,d);
        }catch(...){d={S::allocation,0x1CCD14,false};return false;}
    }

bool Application::releaseUiSearchListCold(rr::platform::OriginalUiSearchListCold50KG& owner,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept {
        return owner.releaseCold(this,entitiesOwner_,d);
    }

bool Application::stageTextureTransferCold(
        std::shared_ptr<rr::assets::OriginalStandardTextureTransferCold50LA>& out,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept {
        using S=rr::assets::OriginalStandardTextureTransferColdStatus50LA;d={};
        if(out||!textureLifetimeOwner_||!entitiesOwner_||!entitiesOwner_->application10()){
            d={S::input,0x252C70,false};return false;
        }
        try {
            out=std::shared_ptr<rr::assets::OriginalStandardTextureTransferCold50LA>(
                new rr::assets::OriginalStandardTextureTransferCold50LA(*this,textureLifetimeOwner_));
            return out->construct252C70(this,textureLifetimeOwner_,d);
        }catch(...){d={S::allocation,0x2543AC,false};return false;}
    }

bool Application::releaseTextureTransferCold(rr::assets::OriginalStandardTextureTransferCold50LA& owner,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept {
        return owner.releaseCold(this,textureLifetimeOwner_,d);
    }

std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> Application::standardMaterialDomain() const {
        require(materialApplication&&materialApplication->prepared(),
            "canonical domain requires actual application APPSTART owners");
        require(bool(standardMaterialDomainOwner_),"application early native service owner missing");
        if(standardMaterialDomainOwner_->prepared())
            require(standardMaterialDomainOwner_->application_==materialApplication&&
                standardMaterialDomainOwner_->serviceAppStart_==materialApplication->appStart(),
                "application canonical material/input owner changed after preparation");
        if(!standardMaterialDomainOwner_->prepared()){
            rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
            require(standardMaterialDomainOwner_->prepareFresh(materialApplication,diagnostic),
                "application canonical material/program/texture domain rejected");
        }
        return standardMaterialDomainOwner_;
    }

std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> Application::fonts328() const {
        require(materialContextOwner_&&materialContextOwner_->live()&&
            materialContextOwner_->materialApplication()==materialApplication,
            "font registry requires actual live application construction context");
        if(!fonts328Owner_){
            const auto domain=standardMaterialDomain();
            fonts328Owner_=std::shared_ptr<rr::assets::OriginalApplicationFonts50LA>(
                new rr::assets::OriginalApplicationFonts50LA(domain,materialContextOwner_));
        }
        return fonts328Owner_;
    }

bool Application::loadTimeStatsFonts193A0C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadTimeStats193A0C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }

bool Application::loadSpeedBoostFonts18A99C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadSpeedBoost18A99C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }

bool Application::loadPositionStatsFont190E64(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadPositionStats190E64(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }

std::shared_ptr<const rr::assets::OriginalUiArchive50KG> Application::appStart(
        const std::filesystem::path& originalRoot) const {
        require(bool(materialApplication),"application material owner missing");
        if(!materialApplication->prepared()){
            require(bool(standardMaterialDomainOwner_),"application early native service owner missing");
            // Retain the first actual authenticated input before any template
            // work. A failed template attempt reuses it, never remounts a copy.
            auto archive=standardMaterialDomainOwner_->serviceAppStart_;
            if(!archive)archive=standardMaterialDomainOwner_->resolver_->registeredAppStart17CAB8();
            if(!archive)archive=readArchive(originalRoot,rr::assets::OriginalUiArchiveId50KG::appStart);
            // Actual17CAB8, activate=0: source names/rows precede both native
            // mounting and DefaultMT/ScreenMT consumers. A completed call is
            // resumed, never replayed, when a later native/template step fails.
            rr::assets::OriginalStandardTextureDiagnostic50LA registrationDiagnostic;
            require(standardMaterialDomainOwner_->resolver_->registerAppStart17CAB8(archive,registrationDiagnostic),
                "application fresh APPSTART source registration rejected");
            rr::assets::OriginalStandardMaterialDiagnostic50LA serviceDiagnostic;
            require(standardMaterialDomainOwner_->retainNativeAppStart(archive,serviceDiagnostic),
                "application early native APPSTART input custody rejected");
            require(standardMaterialDomainOwner_->prepareAppStartNames17CAD4(serviceDiagnostic),
                "application first APPSTART material-name source read rejected");
            require(standardMaterialDomainOwner_->prepareAppStartPrograms17CAE8(serviceDiagnostic),
                "application early canonical program-name table rejected");
            rr::assets::OriginalUiMaterialDiagnostic50KG diagnostic;
            require(materialApplication->prepareSourceApplication(std::move(archive),
                standardMaterialDomainOwner_->resolver_,standardMaterialDomainOwner_->programs_,diagnostic),
                "application authentic DefaultMT/ScreenMT owners rejected");
        }else{
            // Compatibility for callers that prepared the public material owner
            // directly. Retain its actual input, but do not infer that this
            // late native mount preceded those already-completed consumers.
            require(bool(standardMaterialDomainOwner_),"application early native service owner missing");
            if(!standardMaterialDomainOwner_->serviceAppStart_){
                rr::assets::OriginalStandardMaterialDiagnostic50LA serviceDiagnostic;
                require(standardMaterialDomainOwner_->retainNativeAppStart(
                    materialApplication->appStart(),serviceDiagnostic),
                    "application existing native APPSTART input custody rejected");
            }
            require(standardMaterialDomainOwner_->serviceAppStart_==materialApplication->appStart(),
                "application APPSTART owner changed after native input custody");
        }
        return materialApplication->appStart();
    }

void Application::saveAcceptedSettings() const noexcept {
        using Result=rr::platform::native_beta50kg::Result;
        betaSettingsStatus=gameSettingsProfile&&controlsProfile?
            betaSettings.save(*gameSettingsProfile,*controlsProfile):Result::unavailable;
        if(betaSettingsStatus!=Result::ok){
            const char* reason="preferences could not be saved";
            const char* recovery="Check the save directory and Accept again to retry.";
            switch(betaSettingsStatus){
            case Result::invalid:
                reason="settings validation rejected the accepted values or existing file";
                recovery="Preserve any existing settings-v1.bin before repairing it; no invalid file was overwritten.";break;
            case Result::unavailable:
                reason="the settings directory or exclusive save lock is unavailable";
                recovery="Check the per-user save location and close another game using it, then Accept again.";break;
            case Result::ioError:
                reason="the settings file could not be read or written";
                recovery="Check free disk space and save-directory access, then Accept again.";break;
            case Result::durabilityUnknown:
                reason="replacement completed but disk durability is unconfirmed";
                recovery="The new file may be present; a confirmed durable save is not claimed.";break;
            default:break;
            }
            std::cerr<<"BETA SETTINGS: accepted for this session, but "<<reason
                <<" (status "<<static_cast<unsigned>(betaSettingsStatus)<<"). "<<recovery<<'\n';
        }
    }

Application::~Application() {
        // Never silently erase a live native packet command/marker owner.
        if(nativeVideoPacketInvocation50LA_)std::terminate();
        // Actual native teardown hook: revoke pad transport and source-input
        // use before shared Player borrowers can outlive this Application.
        // Deferred input backing exists only for native partial cancellation.
        if(!closeSourceInput04Native())std::terminate();
        if(courseLoadingPrefix50LA_.frontend)std::terminate();
        // Checked native partial-prefix unwind, not complete17BF00 or screen closure.
        if(!retireNativeLoading330Prefix())std::terminate();
        if(!nativeTaCameraSession50LA_.expired())std::terminate();
        if(memoryCardConstructorOwner_||(state&&state->sharedDialogs)||(sharedUiScopeOwner_&&!sharedUiScopeOwner_->closed_))std::terminate();
        // A live source slot may still own renderer/context and counted state.
        // Application destruction cannot substitute for its checked retirement.
        if(sourceAural00Owner_||sourceRace28COwner_||resultsHallTransfer50LA_||pauseInputContinuation50LA_||pauseMenuTransfer50LA_||sourceLightManager426F60Owner_||sourceLocalization32COwner_||localizationRendererCall14C5A0_)std::terminate();
        // Fail closed before invalidating the actual clock. This is not
        // cleanup: failed-prefix consumers and their mixer remain owned.
        if(garageAudioOwnerThread_!=std::this_thread::get_id()||garageAudioBackingOwner_||garageCustodyOwner_)std::terminate();
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationMayRetire(*this))std::terminate();
        // Native source-prefix revocation only. Keep partial constructor and
        // publication backing until normal member destruction, with no claim
        // that25A300 teardown or any original video frame call has returned.
        if(!sourceVideoProduction_.closeNative(*this))std::terminate();
        // Checked audio retirement must precede lifetime invalidation. This
        // assertion does not clear rows, flush registrations, or drain owners.
        if(!rr::platform::OriginalAudioPendingWaves50KG::applicationMayRetire(*this))std::terminate();
        animationClockOwner_->applicationAlive_=false;
    }

Application::Application(std::shared_ptr<rr::game::HostMonotonicClock50ID> clock,
        std::shared_ptr<rr::game::ControlPresets49D> controls,bool retainPlayerWindow)
        :animationClockBackend_(clock),retainPlayerWindow50DU_(retainPlayerWindow),controlsProfile(std::move(controls)) {
        // Retain the one application epoch even on the disabled-Main branch.
        require(bool(animationClockBackend_),"application animation clock owner missing");
        // Same actual Application constructor's represented17C650 lane:
        //17C714 unconditionally writes1, after its fresh28C/2DC null stores.
        // This native normal-UI entry does not replay original argc/argv boot.
        // In particular disabled Main, a host track option or an environment
        // override never fabricates17C9EC's separate predefined-track write0.
        sourceUseProfile2E0_=1;
        // Same constructor writes the process audio directory at17C7BC/C0/C4.
        // Retain exact writable-global custody, not an assumed native path.
        require(sourceAudioDirectory36AFE0_.write17C7BC(*this),
            "application original audio directory owner rejected");
        // Actual Application lifetime, before APPSTART IO or template consumers,
        // including direct entry and disabled Main. This is native ownership;
        // original13FB6C publication/12CEA0 registration are still unimplemented.
        auto domain=std::make_shared<rr::assets::OriginalStandardMaterialDomain50LA>();
        rr::assets::OriginalStandardMaterialDiagnostic50LA serviceDiagnostic;
        require(domain->prepareFreshNativeService(serviceDiagnostic),
            "application early native file capacity/provider rejected");
        standardMaterialDomainOwner_=std::move(domain);
        const char* originalRoot=std::getenv("RR_ORIGINAL_ASSET_ROOT");
        require(!originalRoot||*originalRoot,"RR_ORIGINAL_ASSET_ROOT must name a nonempty directory");
        decodeRiderImagesInProcess=originalRoot&&*originalRoot;
        const char* enabledSetting=std::getenv("RR_ORIGINAL_MAIN_MENU");
        require(!enabledSetting || std::string(enabledSetting)=="0" || std::string(enabledSetting)=="1",
            "RR_ORIGINAL_MAIN_MENU must be 0 or 1");
        enabled=enabledSetting && std::string(enabledSetting)=="1";
        if(const char* previews=std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")){
            require(std::string(previews)=="3,7,9,23,24",
                "RR_ORIGINAL_RIDER_PREVIEWS must be exactly 3,7,9,23,24");
            require(enabled,"RR_ORIGINAL_RIDER_PREVIEWS requires RR_ORIGINAL_MAIN_MENU=1");
            requestedPreviewRiders=(1u<<3)|(1u<<7)|(1u<<9)|(1u<<23)|(1u<<24);
        }
        const char* visibleRiderSetting=std::getenv("RR_ORIGINAL_TEST_VISIBLE_RIDER");
        std::uint32_t visibleRiderTest=0;
        if(visibleRiderSetting){
            const std::string value(visibleRiderSetting);
            require(value=="3"||value=="8"||value=="22"||value=="14"||value=="21"||value=="25"||value=="26",
                "RR_ORIGINAL_TEST_VISIBLE_RIDER must be exactly 3, 8, 22, 14, 21, 25 or 26; other gameplay owners are not admitted");
            visibleRiderTest=value=="3"?3u:value=="8"?8u:value=="14"?14u:value=="21"?21u:value=="25"?25u:value=="26"?26u:22u;
            require(enabled&&!std::getenv("RR_ORIGINAL_TEST_PROFILE"),
                "Visible Rider test override requires Main and excludes other test profiles");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Visible Rider test override requires original-disc Rider decoding or an authenticated pixel cache");
        }
        const char* wanMbSetting=std::getenv("RR_ORIGINAL_TEST_WAN_MB");
        const bool wanMbTest=wanMbSetting!=nullptr;
        if(wanMbTest){
            require(std::string(wanMbSetting)=="1",
                "RR_ORIGINAL_TEST_WAN_MB must be exactly 1 when present");
            require(enabled&&!visibleRiderSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE"),
                "Wan/MB test profile requires Main and excludes other test profiles");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Wan/MB test profile requires the normal human-controlled race");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Wan/MB test profile requires original-disc Rider decoding or an authenticated pixel cache");
        }
        const char* gasganoScSetting=std::getenv("RR_ORIGINAL_TEST_GASGANO_SC");
        const bool gasganoScTest=gasganoScSetting!=nullptr;
        if(gasganoScTest){
            require(std::string(gasganoScSetting)=="1",
                "RR_ORIGINAL_TEST_GASGANO_SC must be exactly 1 when present");
            require(enabled&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Gasgano/SC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Gasgano/SC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Gasgano/SC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct SC provenance never impersonates the legacy Gasgano fixture.
            requestedPreviewRiders|=1u<<7;
        }
        const char* tzidikTcSetting=std::getenv("RR_ORIGINAL_TEST_TZIDIK_TC");
        const bool tzidikTcTest=tzidikTcSetting!=nullptr;
        if(tzidikTcTest){
            require(std::string(tzidikTcSetting)=="1",
                "RR_ORIGINAL_TEST_TZIDIK_TC must be exactly 1 when present");
            require(enabled&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Tzidik/TC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Tzidik/TC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Tzidik/TC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct TC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<24;
        }
        const char* scorchRcSetting=std::getenv("RR_ORIGINAL_TEST_SCORCH_RC");
        const bool scorchRcTest=scorchRcSetting!=nullptr;
        if(scorchRcTest){
            require(std::string(scorchRcSetting)=="1",
                "RR_ORIGINAL_TEST_SCORCH_RC must be exactly 1 when present");
            require(enabled&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Scorch/RC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Scorch/RC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Scorch/RC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct RC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<17;
        }
        const char* sebulbaTbSetting=std::getenv("RR_ORIGINAL_TEST_SEBULBA_TB");
        const bool sebulbaTbTest=sebulbaTbSetting!=nullptr;
        if(sebulbaTbTest){
            require(std::string(sebulbaTbSetting)=="1",
                "RR_ORIGINAL_TEST_SEBULBA_TB must be exactly 1 when present");
            require(enabled&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Sebulba/TB test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Sebulba/TB test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Sebulba/TB test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct TB provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<18;
        }
        const char* kraidGbSetting=std::getenv("RR_ORIGINAL_TEST_KRAID_GB");
        const bool kraidGbTest=kraidGbSetting!=nullptr;
        if(kraidGbTest){
            require(std::string(kraidGbSetting)=="1",
                "RR_ORIGINAL_TEST_KRAID_GB must be exactly 1 when present");
            require(enabled&&!sebulbaTbSetting&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Kraid/GB test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Kraid/GB test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Kraid/GB test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct GB provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<23;
        }
        const char* youngAnakinMcSetting=std::getenv("RR_ORIGINAL_TEST_YOUNG_ANAKIN_MC");
        const bool youngAnakinMcTest=youngAnakinMcSetting!=nullptr;
        const std::uint32_t mcSelectedRider=visibleRiderTest?visibleRiderTest:1u;
        if(youngAnakinMcTest){
            require(std::string(youngAnakinMcSetting)=="1",
                "RR_ORIGINAL_TEST_YOUNG_ANAKIN_MC must be exactly 1 when present");
            require(enabled&&!kraidGbSetting&&!sebulbaTbSetting&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&(!visibleRiderSetting||mcSelectedRider==14||mcSelectedRider==21||mcSelectedRider==25||mcSelectedRider==26)&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Young Anakin/MC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Young Anakin/MC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Young Anakin/MC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct MC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<mcSelectedRider;
        }
        const char* testProfileSetting=std::getenv("RR_ORIGINAL_TEST_PROFILE");
        require(!testProfileSetting||std::string(testProfileSetting)=="gasgano"||std::string(testProfileSetting)=="mawhonic-ryloth",
            "unsupported explicit test profile");
        const bool gasganoTest=testProfileSetting&&std::string(testProfileSetting)=="gasgano";
        const bool mawhonicRylothTest=testProfileSetting&&std::string(testProfileSetting)=="mawhonic-ryloth";
        if(testProfileSetting){
            const auto exact=[](const char* name,const char* value){
                const auto* setting=std::getenv(name);return setting&&std::string(setting)==value;
            };
            const auto* captureDirectory=std::getenv("RR_WINDOW_CAPTURE_DIR");
            require(enabled&&exact("RR_UNATTENDED_UI_ROUTE",mawhonicRylothTest?"main-mawhonic09-course-ra":"main-gasgano07-course-sa")&&
                exact("RR_WINDOW_CAPTURE_AUTO","1")&&exact("RR_UNATTENDED_CAPTURE","1")&&
                exact("RR_ORIGINAL_AI_ALL_RACERS","1")&&exact("RR_TRACK_FPS_BASELINE","1")&&
                captureDirectory&&*captureDirectory,
                "Explicit test profile requires the explicit complete original-AI window evidence route");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Explicit test profile requires original-disc Rider decoding or an authenticated pixel cache");
        }
        if(!enabled)return;
        const char* root=std::getenv("RR_ORIGINAL_UI_ROOT");
        require(root && *root,"RR_ORIGINAL_MAIN_MENU requires explicit RR_ORIGINAL_UI_ROOT");
        assetRoot=std::filesystem::path(root);
        const char* audio=std::getenv("RR_ORIGINAL_MAIN_AUDIO");
        require(audio && (std::string(audio)=="muted" || std::string(audio)=="enabled"),
            "Main entry requires explicit RR_ORIGINAL_MAIN_AUDIO=muted or enabled");
        if(std::string(audio)=="enabled"){
            audioMode=rr::platform::OriginalUiMainAudioMode50KG::enabled;
            const char* audioRootSetting=std::getenv("RR_ORIGINAL_MAIN_AUDIO_ROOT");
            require(audioRootSetting && *audioRootSetting,
                "enabled Main audio requires explicit RR_ORIGINAL_MAIN_AUDIO_ROOT pointing to AUDIO");
            audioRoot=std::filesystem::path(audioRootSetting);
        }else{
            require(!std::getenv("RR_AUDIO_WAV"),"muted original Main cannot produce an audio proof recording");
        }
        profile=std::make_shared<rr::platform::OriginalUiMainProfile50KG>();
        optionsProfile=std::make_shared<rr::platform::OriginalUiOptionsProfile50KG>();
        gameSettingsProfile=std::make_shared<rr::game::OriginalUiGameSettingsProfile50KG>();
        soundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
        state=std::make_shared<rr::platform::OriginalUiMainApplication50KG>();
        require(bool(clock),"original application source clock owner missing");
        state->clock101A50=clock; // Same retained epoch for startup, materials and Rider voice seed.
        // The last shared owner performs application destruction. A frontend
        // pins this same object until its consumers/cleanup have finished, so
        // even exceptional unwinding cannot destroy it under live borrowers.
        using LightManager=rr::game::OriginalUiLightManagerLifetime50KG;
        lightManager=std::shared_ptr<LightManager>(new LightManager,
            [](LightManager* manager) noexcept {manager->destroyApplication();delete manager;});
        rr::game::OriginalUiLightManagerLifetimeDiagnostic50KG lightDiagnostic;
        require(lightManager->constructAtApplicationStartup(lightDiagnostic),
            "original shared LightManager startup rejected");
        lightManagerStartupWitness_=lightManager; // Exact existing constructor owner, not a new concrete startup call.
        // Fresh native backing starts with no imported unlocks or debug bits.
        // Source1864B0/186EF0 reset only its proven Main fields.
        rr::platform::resetOriginalUiMainProfileFields50KG(*profile);
        rr::platform::resetOriginalUiOptionsProfileFields50KG(*optionsProfile);
        rr::game::resetOriginalUiGameSettingsProfileFields50KG(*gameSettingsProfile);
        // These are explicit fresh process-owned profile fields. Construct and
        // reset once here, never on page entry, Defaults, Cancel or bank switch.
        profile->known=true;optionsProfile->known=true;gameSettingsProfile->known=true;
        //17F970/1870B0: read the authenticated authored ability tables once
        // at explicit new-profile startup, sharing the actual Main profile.
        // No missing-save fallback and no reset on Rider entry or Back.
        const char* cacheRoot=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
        if(decodeRiderImagesInProcess||cacheRoot){
            if(cacheRoot){
                require(*cacheRoot,"RR_ORIGINAL_RIDER_IPUM_CACHE must name a nonempty directory");
                riderCacheRoot=std::filesystem::path(cacheRoot);
            }
            riderProfile=std::make_shared<rr::platform::OriginalUiRiderProfileOwner50KG>();
            rr::platform::OriginalUiRiderProfileDiagnostic50KG riderProfileDiagnostic;
            const auto physics=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::podPhysics);
            require(youngAnakinMcTest?
                riderProfile->initializeYoungAnakinMcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic,mcSelectedRider):kraidGbTest?
                riderProfile->initializeKraidGbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):sebulbaTbTest?
                riderProfile->initializeSebulbaTbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):scorchRcTest?
                riderProfile->initializeScorchRcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):tzidikTcTest?
                riderProfile->initializeTzidikTcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):gasganoScTest?
                riderProfile->initializeGasganoScTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):wanMbTest?
                riderProfile->initializeWanMbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):visibleRiderTest?
                riderProfile->initializeVisibleRiderTestProfileAtApplicationStartup(profile,physics,visibleRiderTest,riderProfileDiagnostic):mawhonicRylothTest?
                riderProfile->initializeMawhonicRylothTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):gasganoTest?
                riderProfile->initializeGasganoTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):
                riderProfile->initializeAtApplicationStartup(profile,physics,riderProfileDiagnostic),
                "Rider startup profile initialization rejected");
            riderProfile->bindNativeBetaAutosave(gameSettingsProfile);
            // Original17DE80/1801D0 fresh-profile record producer. Populate
            // once at APPLICATION startup, never Hall entry or missing-save recovery.
            const auto recordLocalization=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::localization);
            rr::assets::OriginalUiEnglish50KG recordEnglish;
            rr::assets::OriginalUiLocalizationDiagnostic50KG recordLocalizationDiagnostic;
            require(rr::assets::readOriginalUiEnglish50KG(*recordLocalization,recordEnglish,recordLocalizationDiagnostic)&&
                riderProfile->initializeHallRecordsAtApplicationStartup(recordEnglish,riderProfileDiagnostic),
                "Original fresh Hall record initialization rejected");
            require(riderProfile->gasganoTestProfile()==gasganoTest&&
                riderProfile->mawhonicRylothTestProfile()==mawhonicRylothTest&&
                riderProfile->visibleRiderTestProfile()==(youngAnakinMcTest?0u:visibleRiderTest)&&
                riderProfile->wanMbTestProfile()==wanMbTest&&
                riderProfile->gasganoScTestProfile()==gasganoScTest&&
                riderProfile->tzidikTcTestProfile()==tzidikTcTest&&
                riderProfile->scorchRcTestProfile()==scorchRcTest&&
                riderProfile->sebulbaTbTestProfile()==sebulbaTbTest&&
                riderProfile->kraidGbTestProfile()==kraidGbTest&&riderProfile->youngAnakinMcTestProfile()==youngAnakinMcTest,"Rider test profile provenance differs");
            if(youngAnakinMcTest)std::cerr<<"TEST UNLOCK OVERRIDE: MC rider="<<mcSelectedRider<<" initially remembered; original course prefix10 and MC slot10 (course6/planet5). Process-local human testing only; no earned progression or save write. Actual MC gameplay owners remain mandatory.\n";
            if(kraidGbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Kraid23 initially remembered; original course prefix8 and GB slot8 (course9/planet1). Process-local human testing only; no earned progression or save write. GB gameplay capability remains separately gated.\n";
            if(sebulbaTbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Sebulba18 initially remembered; original course prefix6 and TB slot6 (course1/planet0). Process-local human testing only; no earned progression or save write. TB gameplay capability remains separately gated.\n";
            if(scorchRcTest)std::cerr<<"TEST UNLOCK OVERRIDE: Scorch17 initially remembered; original course prefix12 and RC slot12 (course3/planet2). Process-local human testing only; no earned progression or save write. RC gameplay capability remains separately gated.\n";
            if(tzidikTcTest)std::cerr<<"TEST UNLOCK OVERRIDE: Tzidik24 initially remembered; original course prefix13 and TC slot13 (course2/planet0). Process-local human testing only; no earned progression or save write. TC gameplay capability remains separately gated.\n";
            if(gasganoScTest)std::cerr<<"TEST UNLOCK OVERRIDE: Gasgano7 initially remembered; original course prefix9 and SC slot9 (course12/planet4). "
                "Process-local human testing only; no earned progression or save write. SC gameplay capability remains separately gated.\n";
            if(wanMbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Wan16 initially remembered; original course prefix7 and MB slot7. "
                "Process-local testing only; no earned progression or save write. Real Wan/MB cold-loader checks remain mandatory.\n";
            if(visibleRiderTest&&!youngAnakinMcTest)std::cerr<<"TEST UNLOCK OVERRIDE: rider="<<visibleRiderTest
                <<" available and initially remembered for this process only. Native test patch, not retail cheat execution; "
                "no earned unlock, career progress, saved-profile import or save write. All gameplay loader gates remain active.\n";
            if(mawhonicRylothTest)std::cerr<<"TEST PROFILE: Mawhonic9 available and initially remembered; original course prefix availability5 for Brightlands, process-local only. "
                "No earned unlock, qualification, career progress or save write is claimed. RA race admission remains subject to genuine runtime readiness.\n";
            if(gasganoTest)std::cerr<<"TEST PROFILE: Gasgano available and initially remembered for this process only; "
                "no earned unlock, qualification, saved-profile import or save write is claimed.\n";
        }
        // Restore preferences before the real sound-mode owner and all later
        // frontend/race borrowers observe this application's canonical fields.
        if(controlsProfile){
            betaSettingsStatus=betaSettings.load(rr::platform::native_beta50kg::defaultDirectory(),
                *gameSettingsProfile,*controlsProfile);
            using Result=rr::platform::native_beta50kg::Result;
            if(betaSettingsStatus!=Result::ok&&betaSettingsStatus!=Result::missing)
                std::cerr<<"BETA SETTINGS: saved preferences unavailable (status "
                    <<static_cast<unsigned>(betaSettingsStatus)
                    <<"). Using session defaults; existing settings file will not be overwritten.\n";
        }
        require(rr::game::validOriginalUiGameSettingsProfile50KG(*gameSettingsProfile),
            "fresh original Game Options profile rejected");
        require(soundMode->initializeFromProfile1101C(gameSettingsProfile->soundMode1101C),
            "fresh original sound mode initialization rejected");
        // One native startup seed from the existing process clock, never per
        // frame, capture or focus change. Exact original boot seeding unclaimed.
        require(rr::game::sampleHostTimestamp50ID(*state->clock101A50,state->rawRandom308030),
            "Main startup shared application timestamp unavailable");
        state->known=true;
        std::cerr<<"ORIGINAL MAIN: "<<(youngAnakinMcTest?"explicit Young Anakin/MC human test profile":kraidGbTest?"explicit Kraid/GB human test profile":sebulbaTbTest?"explicit Sebulba/TB human test profile":scorchRcTest?"explicit Scorch/RC human test profile":tzidikTcTest?"explicit Tzidik/TC human test profile":gasganoScTest?"explicit Gasgano/SC human test profile":wanMbTest?"explicit Wan/MB test unlock profile":visibleRiderTest?"explicit visible-Rider test unlock profile":mawhonicRylothTest?"explicit Mawhonic/Ryloth test profile":gasganoTest?"explicit Gasgano test profile":"native profile (loaded or fresh)")
            <<"/application ownership; "
            "one clock-derived native RNG seed="<<state->rawRandom308030
            <<"; Main audio="<<audio<<". Main, Options menu and Game Options routing is wired; "
            "Game Options Accept requires enabled concrete audio, commits shared settings and attempts native preference persistence. "
            "The original catalog contains 23 racers and 13 courses; actual choices still require original unlock state, authenticated resources and a concrete destination. Catalog coverage is not runtime verification. "
            "Mode0 Rider Confirm to Course and Back supports explicit mute or the original Confirm voice/Song11 owner. Course navigation follows the retained profile's unlocked selections. "
            "Controls uses original presets and its animated controller: Left/Right chooses, Tab changes Normal/Advanced, R rotates, Enter/Space accepts and Escape cancels. Accept applies to the page's controller; Cancel preserves its saved preset. "
            "Second-controller Controls requires a present native controller. Credits uses its authored cards, music and Back/automatic return routes. "
            "Memory Card provides the separate native beta profile Save/confirmed Load route, not original-card emulation. Gallery art screens remain unavailable. "
            "Supported human Course selections use the typed whole-world cold-load boundary, including unchanged selections, and the real selected-track owners. Native beta profile/settings persistence is separate from original memory-card format; original boot/save parity and full racer/course runtime verification are not claimed.\n";
    }

bool Video::acquireCourseMode2A3A90(const Application& root,const std::shared_ptr<Video>& retained,
        rr::game::OriginalUiEntityScope50KG::CourseConstruction& construction,
        const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
        std::unique_ptr<CourseModeEffect>& output)noexcept {
        using Construction=rr::game::OriginalUiEntityScope50KG::CourseConstruction;
        if(output||retained.get()!=this||root.videoOwner_!=retained||
           root.videoOwner_.owner_before(retained)||retained.owner_before(root.videoOwner_)||
           !originalMain||courseModeEffect_||optionsModeEffect_||!construction.issuedBy(&root,root.sharedUiScopeOwner_)||
           !construction.current(frontend,manager)||construction.videoEffect_||
           construction.phase_!=Construction::Phase::beforeVideo2A3A90)return false;
        std::unique_ptr<CourseModeEffect> next;
        try {next.reset(new CourseModeEffect(retained,construction,root,frontend,manager));}
        catch(...){return false;}
        if(optionsModeEffect_||courseModeEffect_||root.videoOwner_!=retained||
           !construction.issuedBy(&root,root.sharedUiScopeOwner_)||
           !construction.current(frontend,manager)||construction.videoEffect_||
           construction.phase_!=Construction::Phase::beforeVideo2A3A90)return false;
        next->before_=sourceMode44_;
        // All allocation/owner validation precedes this callback-free interval.
        courseModeEffect_=next.get();construction.videoEffect_=next.get();
        sourceMode44_=1; // Actual2A3A90 only; no race-display companion effect.
        construction.phase_=Construction::Phase::afterVideo2A3A90;
        next->acquired_=true;output=std::move(next);return true;
    }
bool Video::acquireOptionsMode2A3A90(const Application& root,const std::shared_ptr<Video>& retained,
        rr::game::OriginalUiEntityScope50KG::OptionsConstruction& construction,
        const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
        std::unique_ptr<OptionsModeEffect>& output)noexcept {
        using Construction=rr::game::OriginalUiEntityScope50KG::OptionsConstruction;
        if(output||retained.get()!=this||root.videoOwner_!=retained||
           root.videoOwner_.owner_before(retained)||retained.owner_before(root.videoOwner_)||
           !originalMain||courseModeEffect_||optionsModeEffect_||!construction.issuedBy(&root,root.sharedUiScopeOwner_)||
           !construction.current(frontend,manager)||construction.videoEffect_||
           construction.phase_!=Construction::Phase::beforeVideo2A3A90)return false;
        std::unique_ptr<OptionsModeEffect> next;
        try {next.reset(new OptionsModeEffect(retained,construction,root,frontend,manager));}
        catch(...){return false;}
        if(courseModeEffect_||optionsModeEffect_||root.videoOwner_!=retained||
           !construction.issuedBy(&root,root.sharedUiScopeOwner_)||
           !construction.current(frontend,manager)||construction.videoEffect_||
           construction.phase_!=Construction::Phase::beforeVideo2A3A90)return false;
        next->before_=sourceMode44_;
        // All allocation/owner validation precedes this callback-free interval.
        optionsModeEffect_=next.get();construction.videoEffect_=next.get();
        sourceMode44_=1; // Actual2A3A90 only; no race-display companion effect.
        construction.phase_=Construction::Phase::afterVideo2A3A90;
        next->acquired_=true;output=std::move(next);return true;
    }

std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>
    Video::standardMaterialConstructionContext(const std::shared_ptr<Video>& owner,
        const Application& application) const {
        require(owner.get()==this&&application.videoOwner_.get()==this&&
            materialConstructionPinLive(*this),
            "standard material construction requires the actual current original video owner");
        require(application.materialRuntimeOwner_&&
            &application.materialRuntime==application.materialRuntimeOwner_.get()&&
            application.materialRuntime.overrideKnown&&
            !application.materialRuntime.override435670&&
            application.materialApplication&&application.materialApplication->prepared()&&
            application.textureLifetimeOwner_,
            "standard material construction requires actual application templates, runtime and texture lifetime");
        using Context=rr::assets::OriginalStandardMaterialContext50LA;
        if(application.materialContextOwner_){
            const auto& context=application.materialContextOwner_;
            require(context->videoIdentity()==this&&context->live()&&
                context->materialApplication().get()==application.materialApplication.get(),
                "standard material construction cannot replace its application ownership domain");
            return context;
        }
        // Raw identity alone cannot prove that a caller's shared_ptr owns its
        // pointee. Retain the application's authoritative control block.
        application.materialContextOwner_=std::shared_ptr<const Context>(new Context(application.videoOwner_,
            application.materialApplication,application.materialRuntimeOwner_,
            application.textureLifetimeOwner_,&Video::materialConstructionPinLive,applicationAnimation_));
        return application.materialContextOwner_;
    }

rr::platform::OriginalUiMainVideoOwner50KG Video::publication(
        const std::shared_ptr<Video>& owner) const {
        require(owner.get()==this && originalMain && host.created() && host.current(),
            "Main video publication requires its actual current native owner");
        return {owner,width,height,colorBits,packetZMaximum,zBufferFormat};
    }

bool Frontend::uploadLifetimeLive(const UploadLifetime& token)noexcept {
        const auto* owner=token.frontend_;
        return owner&&owner->uploadLifetime_.get()==&token&&owner->materialApplicationOwner&&
            token.video_&&owner->materialApplicationOwner->videoOwner_.get()==token.video_.get()&&
            token.video_->originalMain&&token.video_->host.created()&&token.video_->host.current();
    }

void Frontend::reportStageRange(const char* stage,StageClock::time_point start,StageClock::time_point end,bool okay,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer) const {
        const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        const auto snapshot=coordinator.snapshotWithoutGarageRuntime();const auto& route=snapshot.route;
        stageHistory[nextStageObservation]={stage,start,end,frameCount,snapshot.updates,route.screen,route.page,okay};
        nextStageObservation=(nextStageObservation+1)%stageHistory.size();
        if(stageObservationCount<stageHistory.size())++stageObservationCount;
        if(!traceStages)return;
        // End the observation before formatting/output; logging overhead still
        // remains real work in the next accepted update interval.
        std::cerr<<"original_ui_stage {\"stage\":\""<<stage<<"\",\"completed_frame\":"<<frameCount
            <<",\"accepted_update_sequence\":"<<snapshot.updates<<",\"screen\":"<<unsigned(route.screen)
            <<",\"page\":"<<unsigned(route.page)<<",\"wall_nanoseconds\":"<<elapsed
            <<",\"steady_begin_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count()
            <<",\"steady_end_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count()
            <<",\"okay\":"<<(okay?"true":"false");
        if(renderer)std::cerr<<",\"source_coherent_framebuffer_fetch\":"<<(renderer->originalUiCoherentFramebufferFetch?"true":"false")
            <<",\"draws\":"<<renderer->originalUiDraws<<",\"source_triangles\":"<<renderer->originalUiSourceTriangles
            <<",\"source_texture_barriers\":"<<renderer->originalUiTextureBarriers;
        std::cerr<<"}\n";
    }

void Frontend::reportStage(const char* stage,StageClock::time_point start,bool okay,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer) const {
        reportStageRange(stage,start,StageClock::now(),okay,renderer);
    }

void Frontend::accountColdInterval(const rr::platform::OriginalUiFrontendColdInterval50KG& interval) {
        if(!interval.measured)return;
        require(interval.end>=interval.begin,"UI measured cold clock moved backwards");
        const auto actual=std::chrono::duration_cast<std::chrono::nanoseconds>(interval.end-interval.begin).count();
        require(static_cast<std::uint64_t>(actual)==interval.nanoseconds,"UI measured cold duration mismatch");
        require(original_ui_elapsed50LH::creditCold(interval.nanoseconds,
            pendingUpdateColdNanoseconds,pendingMaterialColdNanoseconds),"UI cold interval overflow");
    }

void Frontend::accountBankPublication(const rr::platform::OriginalUiFrontendColdInterval50KG& interval,
        const rr::platform::OriginalUiFrontendWorkObservation50KG& drain,bool okay) {
        // Diagnostic stage ranges never feed the clocks. Only the coordinator's
        // exact owned bank-publication receipt credits the two consumers.
        if(drain.measured)reportStageRange("bank_gpu_drain",drain.begin,drain.end,okay);
        if(interval.measured)reportStageRange("cold_bank_publication",interval.begin,interval.end,okay);
        accountColdInterval(interval);
    }

const char* Frontend::presentationPolicyName()const noexcept {
        switch(riderPresentationPolicy){
        case RiderPresentationPolicy::driverSwapInterval1:return "driver_swap_interval1";
        case RiderPresentationPolicy::nativePcDeadline60Hz:return "native_pc_deadline60hz";
        case RiderPresentationPolicy::restoring:return "restoring";
        default:return "inactive";
        }
    }

static bool unattendedRiderPresentation50KG()noexcept {
        const char* value=std::getenv("RR_UNATTENDED_CAPTURE");
        return value&&std::strcmp(value,"1")==0;
    }

bool Frontend::available()const noexcept  {
        if(!presentationHost||!presentationNow||!presentationHost->hasWindow()||!presentationHost->current()||hostPaused()){std::fprintf(stderr,"original_rider_presentation_failure method=available site=1\n");return false;}
        const auto& info=presentationHost->info();
        if((!info.focused&&!unattendedRiderPresentation50KG())||info.minimized||info.drawableWidth<=0||info.drawableHeight<=0){std::fprintf(stderr,"original_rider_presentation_failure method=available site=2\n");return false;}
        if(!presentationActive)return riderPresentationPolicy==RiderPresentationPolicy::inactive;
        if(info.drawableWidth!=presentationWidth||info.drawableHeight!=presentationHeight){std::fprintf(stderr,"original_rider_presentation_failure method=available site=3\n");return false;}
        return (riderPresentationPolicy==RiderPresentationPolicy::driverSwapInterval1&&info.swapInterval==1)||
            (riderPresentationPolicy==RiderPresentationPolicy::nativePcDeadline60Hz&&info.swapInterval==0);
    }

bool Frontend::begin()noexcept  {
        if(presentationActive||!available()){std::fprintf(stderr,"original_rider_presentation_failure method=begin site=1\n");return false;}
        savedSwapInterval=presentationHost->info().swapInterval;
        presentationWidth=presentationHost->info().drawableWidth;
        presentationHeight=presentationHost->info().drawableHeight;
        presentationBeginSwaps=riderPresentationSwaps;
        presentationCompletedWaits=0;presentationCompletedWaitNanoseconds=0;riderDeadline.reset();
        // Any failed setter leaves an explicit restoration obligation until
        // the exact saved native setting is verified; no assumed rollback.
        presentationActive=true;riderPresentationPolicy=RiderPresentationPolicy::restoring;
        rr::platform::GameHostDiagnostic50ER d;
        if(presentationHost->setSwapInterval(1,d))riderPresentationPolicy=RiderPresentationPolicy::driverSwapInterval1;
        else {
            const bool swapControlFailure=d.error==rr::platform::GameHostError50ER::Window&&
                d.window.error==rr::platform::WindowHostError50DQ::SwapIntervalFailure;
            if(!presentationHost->setSwapInterval(savedSwapInterval,d)){std::fprintf(stderr,"original_rider_presentation_failure method=begin site=2\n");return false;}
            // Optional native-PC policy, never a fabricated interval1 result.
            // Wrong context/thread, nonzero saved intervals and unaccepted
            // audio/clock baselines cannot activate this alternate owner.
            if(!allowNativePcRiderDeadline||!swapControlFailure||savedSwapInterval!=0||
               presentationHost->info().swapInterval!=0||!lastUpdateNanoseconds||
               original_ui_elapsed50LH::project(lastUpdateNanoseconds,presentationNow(),pendingUpdateColdNanoseconds).error!=
                   original_ui_elapsed50LH::Error::none){
                presentationActive=false;riderPresentationPolicy=RiderPresentationPolicy::inactive;{std::fprintf(stderr,"original_rider_presentation_failure method=begin site=3\n");return false;}
            }
            riderPresentationPolicy=RiderPresentationPolicy::nativePcDeadline60Hz;
        }
        if(!available()){std::fprintf(stderr,"original_rider_presentation_failure method=begin site=4\n");return false;}
        std::fprintf(stderr,"original_rider_presentation_begin policy=%s actual_interval=%d saved_interval=%d physical_scanout_measured=0\n",
            presentationPolicyName(),presentationHost->info().swapInterval,savedSwapInterval);
        return true;
    }

bool Frontend::end()noexcept  {
        if(!presentationActive)return true;
        rr::platform::GameHostDiagnostic50ER d;
        if(!presentationHost||!presentationHost->setSwapInterval(savedSwapInterval,d)){std::fprintf(stderr,"original_rider_presentation_failure method=end site=1\n");return false;}
        std::fprintf(stderr,"original_rider_presentation_end policy=%s restored_interval=%d swap_calls=%llu completed_waits=%llu completed_wait_scope_ns=%llu physical_scanout_measured=0\n",
            presentationPolicyName(),presentationHost->info().swapInterval,
            static_cast<unsigned long long>(riderPresentationSwaps-presentationBeginSwaps),
            static_cast<unsigned long long>(presentationCompletedWaits),
            static_cast<unsigned long long>(presentationCompletedWaitNanoseconds));
        presentationActive=false;riderPresentationPolicy=RiderPresentationPolicy::inactive;
        riderDeadline.reset();presentationWidth=0;presentationHeight=0;return true;
    }

bool Frontend::pollPresentationInput()noexcept {
        if(!presentationActive||!available()){std::fprintf(stderr,"original_rider_presentation_failure method=pollPresentationInput site=1\n");return false;}
        rr::platform::GameHostInput50ER input;rr::platform::GameHostDiagnostic50ER d;
        if(!presentationHost->poll(input,d)||input.keys.quitRequested||
           (input.keys.activityLost&&!unattendedRiderPresentation50KG())||
           input.keys.resized||input.keys.eventBudgetExhausted||!available()){std::fprintf(stderr,"original_rider_presentation_failure method=pollPresentationInput site=2\n");return false;}
        return true;
    }

bool Frontend::waitBeforeSample()noexcept  {
        if(!pollPresentationInput()){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=1\n");return false;}
        // Driver policy retains its existing real swap wait. The opt-in
        // interval0 host policy waits BEFORE render, inside actual sample dt.
        if(riderPresentationPolicy==RiderPresentationPolicy::driverSwapInterval1)return true;
        if(riderPresentationPolicy!=RiderPresentationPolicy::nativePcDeadline60Hz){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=2\n");return false;}
        using Error=original_ui_rider_pacing50LB::Error;
        const auto begin=presentationNow();
        if(riderDeadline.beginWait(begin)!=Error::none){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=3\n");return false;}
        for(;;){
            if(!pollPresentationInput()||!checkpointAudio()){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=4\n");return false;}
            const auto now=presentationNow();const auto observed=riderDeadline.observe(now);
            if(observed.error!=Error::none){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=5\n");return false;}
            if(observed.due){
                if(now<begin||now-begin>std::numeric_limits<std::uint64_t>::max()-presentationCompletedWaitNanoseconds||
                   presentationCompletedWaits==std::numeric_limits<std::uint64_t>::max()||
                   riderDeadline.finishWait(now)!=Error::none){std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=6\n");return false;}
                presentationCompletedWaitNanoseconds+=now-begin;++presentationCompletedWaits;return true;
            }
            // Sleep requests are at most1ms; oversleep remains real elapsed
            // work. All measured active time remains in the PCM accounting domain.
            try{std::this_thread::sleep_for(std::chrono::nanoseconds(observed.sleepNanoseconds));}
            catch(...){{std::fprintf(stderr,"original_rider_presentation_failure method=waitBeforeSample site=7\n");return false;}}
        }
    }

bool Frontend::materialClock(rr::platform::OriginalUiMainFrameClock50KG& out)noexcept  {
        if(!presentationClock){std::fprintf(stderr,"original_rider_presentation_failure method=materialClock site=1\n");return false;}
        try{out=sampleClock(*presentationClock,true);return true;}catch(...){{std::fprintf(stderr,"original_rider_presentation_failure method=materialClock site=2\n");return false;}}
    }

bool Frontend::checkpointAudio()noexcept  {
        if(!presentationActive||!available()){std::fprintf(stderr,"original_rider_presentation_failure method=checkpointAudio site=1\n");return false;}
        const auto now=presentationNow();
        // Blocking presentation requires an already accepted active baseline;
        // it cannot use the first/resumed-page zero-delta convention.
        if(!lastUpdateNanoseconds){std::fprintf(stderr,"original_rider_presentation_failure method=checkpointAudio site=2\n");return false;}
        const auto interval=original_ui_elapsed50LH::project(lastUpdateNanoseconds,now,pendingUpdateColdNanoseconds);
        if(interval.error!=original_ui_elapsed50LH::Error::none){std::fprintf(stderr,"original_rider_presentation_failure method=checkpointAudio site=3\n");return false;}
        rr::platform::AudioOutputDiagnostic50HD d;
        if(!audioOutput.advanceAcceptedNanoseconds(interval.activeNanoseconds,d)){std::fprintf(stderr,"original_rider_presentation_failure method=checkpointAudio site=4\n");return false;}
        // These actual steady-clock intervals are mixed exactly once during
        // blocking native presentation. They are excluded from next page dt;
        // material timestamps retain their normal advancing application clock.
        // A successful checkpoint consumes update credit exactly once. The
        // separately sampled material clock still owns its own cold credit.
        pendingUpdateColdNanoseconds=0;lastUpdateNanoseconds=now;return true;
    }

bool Frontend::present(rr::platform::VehicleWindowGl50DS& renderer,
        const rr::platform::OriginalUiPrivatePublication50KP& image,
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept  {
        if(!presentationActive||!available()||!image.completed()||!pollPresentationInput()){std::fprintf(stderr,"original_rider_presentation_failure method=present site=1\n");return false;}
        rr::platform::GameHostDiagnostic50ER hostDiagnostic;
        const auto& info=presentationHost->info();
        if(info.drawableWidth!=image.width()||info.drawableHeight!=image.height()){std::fprintf(stderr,"original_rider_presentation_failure method=present site=2\n");return false;}
        rr::platform::OriginalUiWindowBackBufferCopy copy;
        if(!renderer.copyOriginalUiPrivatePublicationToWindow(image,copy,d)||!copy.commandsAccepted()||
           !presentationHost->swapWindow(hostDiagnostic)){std::fprintf(stderr,"original_rider_presentation_failure method=present site=3\n");return false;}
        ++riderPresentationSwaps;
        return checkpointAudio();
    }

rr::platform::OriginalUiMainFrameClock50KG Frontend::sampleClock(
        rr::game::HostMonotonicClock50ID& applicationClock,bool active) {
        std::uint32_t rawSample=0;
        require(rr::game::sampleHostTimestamp50ID(applicationClock,rawSample),
            "original UI shared material timestamp unavailable");
        require(materialApplicationOwner&&materialApplicationOwner->publishAnimationTimestamp(applicationClock,rawSample),
            "original UI canonical time requires the same application backend");
        const auto now=applicationClock.lastNanoseconds;
        if(!sourceSampleKnown)activeSourceNanoseconds=now;
        else {
            require(now>=lastSourceNanoseconds,"original UI material clock moved backwards");
            const auto elapsed=now-lastSourceNanoseconds;
            require(pendingMaterialColdNanoseconds<=elapsed,"UI cold interval exceeds sampled wall interval");
            if(active&&previousSourceActive){
                const auto accepted=elapsed-pendingMaterialColdNanoseconds;
                require(accepted<=std::numeric_limits<std::uint64_t>::max()-activeSourceNanoseconds,
                    "UI active material clock overflow");
                activeSourceNanoseconds+=accepted;
            }
        }
        pendingMaterialColdNanoseconds=0;lastSourceNanoseconds=now;
        sourceSampleKnown=true;previousSourceActive=active;
        // Keep the real application clock/RNG owner unchanged. Quantize only
        // this measured active presentation timeline through the existing
        // native source timer conversion, excluding focus and cold loading.
        require(rr::game::hostNanosecondsToSourceTimestamp50ID(activeSourceNanoseconds,sourceMilliseconds),
            "original UI active material timestamp conversion failed");
        return {true,active,sourceMilliseconds,materialApplicationOwner->animationClock()};
    }

void Frontend::prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest,
        std::shared_ptr<const rr::platform::OriginalUiCourseRaceDestination50KG> raceDestination,
        std::uint64_t (*nativeNow)()noexcept,std::uint64_t nativeSessionIndex) {
        require(!materialApplicationOwner,"frontend material directory already entered");
        {std::uint32_t raceScreen=0,racePage=0;
         const bool raceReturn=application.nativeRaceReturnRoute50PD(raceScreen,racePage);
         require(!raceReturn||(raceScreen<=255u&&racePage<=255u),"race return route out of range");
         nativeRaceReturnScreen_=raceReturn?static_cast<std::uint8_t>(raceScreen):0;
         nativeRaceReturnPage_=raceReturn?static_cast<std::uint8_t>(racePage):0;
         // A 1AB5BC request has its own receiver; only 1CA6F0's is the Hall's.
         nativeResultsHallEntry_=bool(application.resultsHallTransfer50LA_)&&!raceReturn;}
        nativePauseMainEntry_=application.nativePauseMainReceiverCurrent50LA();
        nativePauseCourseEntry_=application.nativePauseCourseReceiverCurrent50LA();
        require(!application.pauseMenuTransfer50LA_||(nativePauseMainEntry_!=nativePauseCourseEntry_),
            "Pause receiver requires an exact retired-session request");
        require(!(nativeResultsHallEntry_&&(nativePauseMainEntry_||nativePauseCourseEntry_)),"competing native receiver requests");
        require(!(nativeRaceReturnScreen_&&(nativePauseMainEntry_||nativePauseCourseEntry_)),"competing native race-return receiver requests");
        require(!nativePauseMainEntry_||application.nativePauseMainReceiverCurrent50LA(),
            "Pause Main receiver requires exact post-batch, post-session request");
        require(!nativeResultsHallEntry_||application.nativeResultsHallReceiverCurrent50LA(),
            "Results Hall receiver requires the exact post-session request");
        materialApplicationOwner=&application;
        require(!(nativePauseMainEntry_||nativePauseCourseEntry_)||application.bindNativePauseMenuReceiver50LA(*this,video),
            "Pause request requires this exact native frontend/Video receiver");
        application.enterUiMaterialDirectory17DB80();
        using Id=rr::assets::OriginalUiArchiveId50KG;
        rr::platform::OriginalUiFrontendCold50KG cold;
        cold.appStart=application.appStart(application.assetRoot);
        cold.mainMenu=readArchive(application.assetRoot,Id::mainMenu);
        cold.options=readArchive(application.assetRoot,Id::options);
        cold.hall=readArchive(application.assetRoot,Id::hallOfFame);
        // Optional original screens 10/7; absent archives keep only them unavailable.
        for(const auto id:{Id::unlock,Id::win}){
            const auto* identity=rr::assets::originalUiArchiveIdentity50KG(id);
            require(identity,"original Unlock/Win archive identity missing");
            if(std::filesystem::exists(application.assetRoot/std::filesystem::path(identity->discPath)))
                (id==Id::unlock?cold.unlock:cold.win)=readArchive(application.assetRoot,id);
        }
        cold.fonts=readArchive(application.assetRoot,Id::fonts);
        cold.controllerPopup=readArchive(application.assetRoot,Id::controllerPopup);
        cold.overlays=readArchive(application.assetRoot,Id::overlays);
        cold.localization=readArchive(application.assetRoot,Id::localization);
        languageArchive=cold.localization;fontArchive=cold.fonts;
        cold.runtime=application.materialRuntime;cold.video=video->publication(video);
        cold.profile=application.profile;cold.application=application.state;cold.applicationAnimation=application.animationClock();cold.courseRaceDestination=std::move(raceDestination);
        presentationHost=&video->host;presentationNow=nativeNow;
        // Absent retains strict driver-only admission. This request changes
        // host pacing only, never Rider profile/resource/selection guards.
        if(const char* policy=std::getenv("RR_ORIGINAL_RIDER_PRESENTATION")){
            require(std::string(policy)=="allow-native-pc-deadline60hz",
                "RR_ORIGINAL_RIDER_PRESENTATION must be allow-native-pc-deadline60hz");
            allowNativePcRiderDeadline=true;
        }
        presentationClock=application.state->clock101A50.get();cold.riderPresentation=this;
        cold.lightManager=application.lightManager;
        cold.riderProfile=application.riderProfile;
        // Original-disc mode decodes in process, with no external image pack.
        // Both paths preserve the same source and decoded-image hash admission.
        if(application.decodeRiderImagesInProcess||!application.riderCacheRoot.empty()){
            require(application.riderProfile && application.riderProfile->initialized(),
                "original Rider application profile owner unavailable");
            using Cache=rr::assets::OriginalUiIpumTextureCache50KG;
            require(Cache::catalogAvailable(),"original Rider IPUM output catalog is unavailable");
            cold.rider=readArchive(application.assetRoot,Id::rider);
            // Course is an optional destination. A missing archive must not
            // remove existing Main/Options/Rider Back capability. If supplied,
            // its bytes still authenticate normally; malformed input is fatal.
            const auto* courseIdentity=rr::assets::originalUiArchiveIdentity50KG(Id::course);
            require(courseIdentity,"original Course archive identity missing");
            if(std::filesystem::exists(application.assetRoot/std::filesystem::path(courseIdentity->discPath)))
                cold.course=readArchive(application.assetRoot,Id::course);
            else
                std::cerr<<"Original Course archive absent; Course destination remains unavailable.\n";
            // Optional genuine Tournament archive, independent of Course. A
            // missing owner keeps only that destination unavailable.
            const auto* tournamentIdentity=rr::assets::originalUiArchiveIdentity50KG(Id::tournament);
            require(tournamentIdentity,"original Tournament archive identity missing");
            if(std::filesystem::exists(application.assetRoot/std::filesystem::path(tournamentIdentity->discPath)))
                cold.tournament=readArchive(application.assetRoot,Id::tournament);
            else std::cerr<<"Original Tournament archive absent; Tournament destination remains unavailable.\n";
            // Original 1B74E0 (ui\uibkg.res open) and 1B7618 (its %s%02d_bkg images).
            cold.backgrounds=readArchive(application.assetRoot,Id::backgrounds);
            rr::platform::loading_progress50DU::step(.1f,0x1b74e0);
            rr::platform::loading_progress50DU::step(.1f,0x1b7618);
            cold.podTextures=readArchive(application.assetRoot,Id::podTextures);
            // Original 1B7860: after ui\uipodtex.res.
            rr::platform::loading_progress50DU::step(.2f,0x1b7860);
            auto cache=std::make_shared<Cache>();
            rr::assets::OriginalUiIpumCacheDiagnostic50KG cacheDiagnostic;
            if(application.decodeRiderImagesInProcess){
                if(!cache->loadOriginal(cold.rider,cacheDiagnostic))
                    throw std::runtime_error("original Rider IPUM native decode/admission rejected: "+
                        cacheDiagnostic.detail+" error="+std::to_string(static_cast<unsigned>(cacheDiagnostic.error))+
                        " mip="+std::to_string(cacheDiagnostic.mip));
                for(std::size_t source=0;source<Cache::additionalCatalogSourceCount();++source)
                    if(!cache->loadAdditionalOriginal(source,cold.rider,cacheDiagnostic))
                        throw std::runtime_error("additional Rider IPUM native decode/admission rejected: "+
                            cacheDiagnostic.detail+" source="+std::to_string(source)+
                            " error="+std::to_string(static_cast<unsigned>(cacheDiagnostic.error))+
                            " mip="+std::to_string(cacheDiagnostic.mip));
            }else{
            std::vector<std::vector<std::uint8_t>> mips;
            mips.reserve(rr::assets::originalUiIpumMipCount50KG);
            for(std::size_t mip=0;mip<rr::assets::originalUiIpumMipCount50KG;++mip){
                const char* filename=Cache::retainedMipFilename(mip);
                require(filename && *filename,"original Rider IPUM retained filename unavailable");
                const auto path=application.riderCacheRoot/filename;
                std::ifstream input(path,std::ios::binary|std::ios::ate);
                if(!input)throw std::runtime_error("cannot open original Rider IPUM mip: "+path.string());
                const std::streamoff size=input.tellg();
                require(size>0 && static_cast<std::uint64_t>(size)<=16u*1024u*1024u,
                    "original Rider IPUM retained mip size rejected");
                std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
                input.seekg(0);input.read(reinterpret_cast<char*>(bytes.data()),
                    static_cast<std::streamsize>(bytes.size()));
                require(static_cast<bool>(input),"original Rider IPUM retained mip read failed");
                mips.push_back(std::move(bytes));
            }
            require(cache->load(cold.rider,mips,cacheDiagnostic),
                "original Rider IPUM retained mip authentication rejected");
            // A compiled additional catalog requires its real retained bytes.
            // With no extra catalog, no additional images are fabricated.
            for(std::size_t source=0;source<Cache::additionalCatalogSourceCount();++source){
                std::vector<std::vector<std::uint8_t>> additionalMips;
                const auto count=Cache::additionalMipCount(source);
                additionalMips.reserve(count);
                for(std::size_t mip=0;mip<count;++mip){
                    const char* filename=Cache::additionalRetainedMipFilename(source,mip);
                    require(filename && *filename,"additional Rider IPUM filename unavailable");
                    const auto path=application.riderCacheRoot/filename;
                    std::ifstream input(path,std::ios::binary|std::ios::ate);
                    if(!input)throw std::runtime_error("cannot open additional Rider IPUM mip: "+path.string());
                    const std::streamoff size=input.tellg();
                    require(size>0 && static_cast<std::uint64_t>(size)<=16u*1024u*1024u,
                        "additional Rider IPUM retained mip size rejected");
                    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
                    input.seekg(0);input.read(reinterpret_cast<char*>(bytes.data()),
                        static_cast<std::streamsize>(bytes.size()));
                    require(static_cast<bool>(input),"additional Rider IPUM retained mip read failed");
                    additionalMips.push_back(std::move(bytes));
                }
                require(cache->loadAdditional(source,cold.rider,additionalMips,cacheDiagnostic),
                    "additional Rider IPUM retained mip authentication rejected");
            }
            }
            cold.riderIpumCache=std::move(cache);
        }
        // Reconstructed on every frontend lifetime from the SAME Application.
        // coordinator.prepare requires genuine inputs; Rider bind prepares every requested pair.
        cold.prepareCleggLockedPreview=application.prepareCleggLockedPreview;
        cold.requestedPreviewRiders=application.requestedPreviewRiders;
        cold.optionsProfile=application.optionsProfile;cold.gameSettingsProfile=application.gameSettingsProfile;
        cold.soundMode=application.soundMode;cold.controlsProfile=application.controlsProfile;
        cold.audioMode=application.audioMode;
        require(video&&application.videoOwner_==video&&textureRequest.context==&video->host&&
            rendererRequest.context==&video->host&&textureRequest.isCurrent&&
            textureRequest.isCurrent==rendererRequest.isCurrent&&!uploadLifetime_,
            "frontend upload requires its actual Application Video and context callbacks");
        uploadLifetime_=std::shared_ptr<UploadLifetime>(new UploadLifetime(*this,video,&uploadLifetimeLive));
        using UploadBinding=rr::assets::OriginalNativeTextureUploadBinding50LA;
        cold.textureRequest=textureRequest;
        cold.textureRequest.originalUploadBinding=std::shared_ptr<const UploadBinding>(new UploadBinding(
            uploadLifetime_,application.textureLifetimeOwner_,&video->host,textureRequest.isCurrent));
        cold.textureRequest.samplers=nullptr;cold.textureRequest.samplerCount=0;
        cold.textureRequest.maximumGpuBytes=32u*1024u*1024u;
        if(application.audioMode==rr::platform::OriginalUiMainAudioMode50KG::enabled){
            rr::platform::AudioOutputRequest50HD outputRequest;
            outputRequest.tryDevice=video->host.hasWindow();outputRequest.wavPath=std::getenv("RR_AUDIO_WAV");
            if(outputRequest.wavPath&&*outputRequest.wavPath&&nativeSessionIndex){
                const std::filesystem::path requested(outputRequest.wavPath);
                sessionWavPath=(requested.parent_path()/(requested.stem().string()+"-menu-session-"+
                    std::to_string(nativeSessionIndex+1)+requested.extension().string())).string();
                outputRequest.wavPath=sessionWavPath.c_str();
            }
            if(const char* setting=std::getenv("RR_AUDIO_DEVICE")){
                require(std::string(setting)=="0"||std::string(setting)=="1","UI RR_AUDIO_DEVICE must be 0 or 1");
                outputRequest.tryDevice=std::string(setting)=="1";
            }
            std::string error;rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
            if(!application.prepareFrontendAudioOutput(*this,outputRequest,error,outputDiagnostic))throw std::runtime_error(error);
            require(audioOutput.soundModeOwner50KG()==application.soundMode.get(),
                "original UI output did not retain the application sound mode owner");
            cold.audio=audioOutput.transport();
            if(!application.prepareFrontendMenuAudio(*this,cold,error))throw std::runtime_error(error);
            const auto info=audioOutput.info();
            std::cerr<<"Original UI audio device="<<rr::platform::audioDeviceStatusName50HD(info.deviceStatus)
                <<" capture="<<info.captureActive<<" rate="<<info.sampleRate<<" detail="<<info.deviceDetail.data()<<'\n';
        }
        // Original 1B7B54: after ui_music.juk and UIevents.ini (muted runs skip the
        // port's audio block above; the original always loads them).
        rr::platform::loading_progress50DU::step(.1f,0x1b7b54);
        const auto prepared=nativePauseCourseEntry_?
            coordinator.prepareNativePauseCourse(cold,application.pauseCourseReceiverOrigin50LA(*this),diagnostic):
            nativeResultsHallEntry_?coordinator.prepareNativeResultsHall(cold,application,diagnostic):
            nativeRaceReturnScreen_?coordinator.prepareNativeRaceReturn(cold,application,
                nativeRaceReturnScreen_,nativeRaceReturnPage_,diagnostic):
            coordinator.prepare(cold,diagnostic);
        if(prepared!=Status::ok)fail("preparation");
        require(coordinator.bindGarageDestinationRoot50KG(*this),"original UI same-Root destination binding failed");
        if(coordinator.configureRendererRequest(rendererRequest,diagnostic)!=Status::ok)fail("renderer configuration");
    }

void Frontend::showBetaSettingsStatus() const noexcept {
        if(materialApplicationOwner)materialApplicationOwner->showBetaPersistenceStatus(
            "Star Wars: Racer Revenge");
    }

void Frontend::activate(rr::platform::VehicleWindowGl50DS& renderer) {
        if((nativePauseMainEntry_||nativePauseCourseEntry_)&&(!materialApplicationOwner||
           !(nativePauseCourseEntry_?materialApplicationOwner->nativePauseCourseReceiverCurrent50LA():
             materialApplicationOwner->nativePauseMainReceiverCurrent50LA())))fail("Pause menu receiver currentness");
        if(nativeResultsHallEntry_&&(!materialApplicationOwner||
           !materialApplicationOwner->nativeResultsHallReceiverCurrent50LA()))fail("Results Hall receiver currentness");
        if(nativeRaceReturnScreen_){std::uint32_t screen=0,page=0;
            if(!materialApplicationOwner||!materialApplicationOwner->nativeRaceReturnRoute50PD(screen,page)||
               screen!=nativeRaceReturnScreen_||page!=nativeRaceReturnPage_)fail("race return receiver currentness");}
        std::string languageError;
        if(!materialApplicationOwner||!materialApplicationOwner->prepareFrontendLanguageServices(*this,renderer,languageError))
            throw std::runtime_error("original UI language startup rejected: "+languageError);
        if(!nativeResultsHallEntry_&&!nativePauseCourseEntry_&&(!materialApplicationOwner||!coordinator.prepareSharedDialogsInitial(*materialApplicationOwner,renderer,diagnostic)))
            fail("shared UI initial ownership");
        if(coordinator.activate(renderer,diagnostic)!=Status::ok)fail("activation");
        if(nativeResultsHallEntry_&&!materialApplicationOwner->acceptNativeResultsHallReceiver50LA())
            fail("Results Hall actual entry commit");
        // 1AB5BC race return: the coordinator has entered the real Garage13,
        // Hall11, Win7 or Unlock10 manager as this lifetime's first screen;
        // consume the request.
        if(nativeRaceReturnScreen_&&!materialApplicationOwner->acceptNativeRaceReturnReceiver50PD())
            fail("race return first-screen actual entry commit");
        if(nativeRaceReturnScreen_==13){
            // The original application timer (140C50) runs continuously
            // through 17DE10 and ld_ui, whose loading frames use the same
            // loop: the Garage's first frame has a real frame interval, not
            // this native frontend's fresh-lifetime zero delta. Commit one
            // timestamp at the end of activation (the Garage cold load is
            // already behind it); the first update measures from here.
            require(presentationNow&&!lastUpdateNanoseconds&&!sourceDispatchActiveNanoseconds&&
                nativeRootAudioPhase_==NativeRootAudioPhase::absent&&!nativeRootAudioPublication_,
                "Garage race return clock continuation requires a fresh frontend clock");
            const auto now=presentationNow();
            original_ui_source_timing50LH::Sample primed;
            require(now&&sourceDispatchTiming.prepare(now,true,primed)&&sourceDispatchTiming.commit(),
                "Garage race return clock continuation rejected");
            lastUpdateNanoseconds=now;sourceDispatchActiveNanoseconds=now;pendingUpdateColdNanoseconds=0;
            nativeRootAudioPublication_=1; // The committed continuation timestamp.
        }
        if((nativePauseMainEntry_||nativePauseCourseEntry_)&&
           !materialApplicationOwner->acceptNativePauseMenuReceiver50LA(*this,renderer))
            fail("Pause actual destination entry commit");
        showBetaSettingsStatus();
    }

std::string Frontend::garageRuntimeFailureText() const {
        const auto& g=diagnostic.garageRuntime;
        if(g.status==rr::assets::OriginalStandardMaterialStatus50LA::complete&&!g.sourcePC&&diagnostic.garageDetail.empty())return {};
        return " garage_runtime="+std::to_string(static_cast<unsigned>(g.status))+
            " garage_runtime_pc="+std::to_string(g.sourcePC)+" garage_runtime_row="+std::to_string(g.row)+
            " garage_runtime_slot="+std::to_string(g.slot)+" garage_runtime_pass="+std::to_string(g.pass)+
            " garage_runtime_map="+std::to_string(g.map)+" garage_runtime_field="+(g.field?g.field:"")+
            " garage_source_effects="+(g.sourceEffectsCommitted?"true":"false")+
            " garage_document="+std::to_string(static_cast<unsigned>(g.document.error))+
            " garage_document_pc="+std::to_string(g.document.sourcePC)+
            " garage_document_offset="+std::to_string(g.document.byteOffset)+
            " garage_program="+std::to_string(static_cast<unsigned>(g.program.status))+
            " garage_program_pc="+std::to_string(g.program.sourcePC)+" garage_program_row="+std::to_string(g.program.row)+
            " garage_texture="+std::to_string(static_cast<unsigned>(g.texture.status))+
            " garage_texture_pc="+std::to_string(g.texture.sourcePC)+" garage_texture_row="+std::to_string(g.texture.row)+
            " garage_texture_offset="+std::to_string(g.texture.byteOffset)+" garage_detail="+diagnostic.garageDetail;
    }

[[noreturn]] void Frontend::fail(const char* operation) const {
        throw std::runtime_error(std::string("original UI ")+operation+" rejected: status="+
            std::to_string(static_cast<unsigned>(diagnostic.status))+" source="+std::to_string(diagnostic.sourcePC)+
            " main="+std::to_string(static_cast<unsigned>(diagnostic.main.status))+
            " options="+std::to_string(static_cast<unsigned>(diagnostic.options.status))+
            " hall="+std::to_string(static_cast<unsigned>(diagnostic.hall.status))+
            " hall_pc="+std::to_string(diagnostic.hall.sourcePC)+
            " hall_draw="+std::to_string(static_cast<unsigned>(diagnostic.hall.draw.error))+
            " hall_draw_pc="+std::to_string(diagnostic.hall.draw.sourcePC)+
            " hall_draw_widget="+std::to_string(diagnostic.hall.draw.widgetId)+
            " hall_widget="+std::to_string(static_cast<unsigned>(diagnostic.hall.widget.error))+
            " hall_resource="+std::to_string(static_cast<unsigned>(diagnostic.hallResources.error))+
            " options_draw="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.error))+
            " options_draw_pc="+std::to_string(diagnostic.options.draw.sourcePC)+
            " options_draw_widget="+std::to_string(diagnostic.options.draw.widgetId)+
            " options_resource="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.resource.error))+
            " options_model="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.resource.controlsModel.error))+
            " options_model_pc="+std::to_string(diagnostic.options.draw.resource.controlsModel.sourcePC)+
            " rider="+std::to_string(static_cast<unsigned>(diagnostic.rider.status))+
            " rider_pc="+std::to_string(diagnostic.rider.sourcePC)+
            " rider_widget="+std::to_string(static_cast<unsigned>(diagnostic.rider.widget.error))+
            " rider_dynamic="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.error))+
            " rider_dynamic_widget="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.widget))+
            " rider_model="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.error))+
            " rider_model_pc="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.sourcePC))+
            " rider_playback="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.error))+
            " rider_playback_pc="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sourceStage))+
            " rider_sample="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.error))+
            " rider_sample_track="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.track))+
            " rider_sample_channel="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.channel))+
            " rider_backend="+std::to_string(static_cast<unsigned>(diagnostic.rider.backend.error))+
            " rider_draw="+std::to_string(static_cast<unsigned>(diagnostic.rider.draw.error))+
            " rider_draw_pc="+std::to_string(diagnostic.rider.draw.sourcePC)+
            " rider_draw_widget="+std::to_string(diagnostic.rider.draw.widget)+
            " rider_draw_surface="+std::to_string(diagnostic.rider.draw.geometrySurface)+
            " rider_draw_component="+std::to_string(diagnostic.rider.draw.geometryComponent)+
            " rider_draw_colors="+std::to_string(diagnostic.rider.draw.geometryStoredColors)+
            " rider_draw_geometry="+std::to_string(diagnostic.rider.draw.geometryStatus)+
            " rider_draw_material_index="+std::to_string(diagnostic.rider.draw.material)+
            " rider_draw_pass="+std::to_string(diagnostic.rider.draw.pass)+
            " tournament="+std::to_string(static_cast<unsigned>(diagnostic.tournament.status))+
            " tournament_pc="+std::to_string(diagnostic.tournament.sourcePC)+
            " tournament_resource="+std::to_string(static_cast<unsigned>(diagnostic.tournamentResources.error))+
            " tournament_widgets="+std::to_string(static_cast<unsigned>(diagnostic.tournamentWidgets.error))+
            " tournament_draw="+std::to_string(static_cast<unsigned>(diagnostic.tournament.draw.error))+
            " tournament_model="+std::to_string(static_cast<unsigned>(diagnostic.tournament.model.error))+
            " course="+std::to_string(static_cast<unsigned>(diagnostic.course.status))+
            " course_pc="+std::to_string(diagnostic.course.sourcePC)+
            " renderer="+std::to_string(static_cast<unsigned>(diagnostic.renderer.error))+garageRuntimeFailureText());
    }

Frontend::Status Frontend::input(std::uint8_t sourceInput,rr::platform::WindowControllerPresence50DQ presence,
        std::uint8_t controller) {
        const auto current=coordinator.route();
        const bool nativeCard=current.screen==12&&current.page==5;
        // Match the authored Memory Card artwork: Square/R LOAD, Circle/Tab SAVE.
        // The bounded page returns unavailableStorage for both, not an operation intent.
        constexpr auto nativeLoadInput=static_cast<std::uint8_t>(rr::assets::OriginalUiMovementInput50KG::rpadLeft);
        constexpr auto nativeSaveInput=static_cast<std::uint8_t>(rr::assets::OriginalUiMovementInput50KG::rpadRight);
        if(!nativeCard)nativeBetaLoadPending=false;
        // Native beta Load is explicit and destructive to unsaved in-memory
        // profile changes. First R/Square arms; second R/Square confirms; Escape
        // cancels without accidentally leaving the page. While that prompt is
        // visible it owns the input: navigation, Save and the other controller
        // must neither dismiss it silently nor reach the underlying card page.
        // In particular, do not clear the latch on an ignored direction while
        // leaving a stale "Confirm Load / Cancel" title on screen.
        if(nativeCard&&nativeBetaLoadPending){
            if(controller!=0)return Status::noChange;
            if(sourceInput==6){
                nativeBetaLoadPending=false;
                materialApplicationOwner->showBetaPersistenceStatus(
                    "Star Wars: Racer Revenge");
                return Status::noChange;
            }
            if(sourceInput!=nativeLoadInput)return Status::noChange;
            // Confirm still traverses the genuine page/session readiness gates
            // below. A temporarily unavailable page keeps the prompt armed;
            // it does not claim a completed Load or consume Cancel authority.
        }
        rr::platform::OriginalUiMainSessionInput50KG in;in.controller=controller;in.originalInput=sourceInput;in.secondControllerObservation=presence;
        rr::platform::OriginalUiFrontendInputReceipt50KG receipt;
        const auto inputStart=stageStart();
        const auto status=coordinator.input(in,receipt,diagnostic);++inputCount;
        reportStage("source_input",inputStart,status==Status::ok||status==Status::noChange||
            status==Status::routeAccepted||status==Status::unavailableDestination||status==Status::secondControllerRequired);
        accountColdInterval(receipt.coldInterval);
        if(receipt.routeAccepted&&receipt.requested.screen==12&&receipt.requested.page==5)
            materialApplicationOwner->showBetaPersistenceStatus(
                "Star Wars: Racer Revenge");
        if(nativeCard&&controller==0&&status==Status::unavailableDestination&&
            receipt.options.pageAccepted&&(sourceInput==nativeLoadInput||sourceInput==nativeSaveInput)){
            const auto* session=coordinator.optionsSession();
            const auto* card=session?session->memoryCardState():nullptr;
            if(card&&card->entered&&!card->inputBlocked4CC&&!card->closing3AC&&card->timer398<=0){
                const auto profile=materialApplicationOwner->riderProfile;
                if(sourceInput==nativeLoadInput&&!nativeBetaLoadPending){
                    nativeBetaLoadPending=true;
                    materialApplicationOwner->showBetaPersistenceStatus(
                        "Star Wars: Racer Revenge");
                    return Status::ok;
                }
                nativeBetaLoadPending=false;
                const bool completed=profile&&(sourceInput==nativeSaveInput?profile->saveNativeBetaProgress():
                    profile->reloadNativeBetaProgressFromMenu());
                materialApplicationOwner->showBetaPersistenceStatus(completed?
                    (sourceInput==nativeSaveInput?"Star Wars: Racer Revenge":
                        "Star Wars: Racer Revenge"):
                    "Star Wars: Racer Revenge");
                std::cerr<<"BETA PROFILE: "<<(sourceInput==nativeSaveInput?"manual save":"manual load")
                    <<(completed?" completed":" failed; no successful save/load claimed")<<'\n';
                return Status::ok;
            }
        }
        if(receipt.routeAccepted&&receipt.source.screen==12&&receipt.source.page==5){
            nativeBetaLoadPending=false;
            showBetaSettingsStatus();
        }
        if(receipt.routeAccepted&&receipt.source.screen==12&&
            ((receipt.source.page==4&&receipt.options.gameOptions.intent==rr::game::OriginalUiGameOptionsIntent50KG::accept)||
             ((receipt.source.page==2||receipt.source.page==3)&&
              receipt.options.controls.intent==rr::game::OriginalUiControlsIntent50KG::accept)))
        {
            materialApplicationOwner->saveAcceptedSettings();
            showBetaSettingsStatus();
        }
        if(receipt.routeAccepted&&receipt.source.screen==5)
            materialApplicationOwner->showBetaPersistenceStatus();
        if(receipt.options.popupChanged||receipt.main.popupChanged){
            const auto ui=coordinator.snapshot();const auto& popup=receipt.main.popupChanged?ui.main.controllerPopup:ui.options.controllerPopup;
            std::cerr<<"original_controller_popup {\"source_pc\":"<<(receipt.main.popupChanged?receipt.main.popupSourcePC:receipt.options.popupSourcePC)
                <<",\"device_observation\":"<<unsigned(presence)<<",\"active\":"<<(popup.active3CC?"true":"false")
                <<",\"closing\":"<<(popup.closing3C8?"true":"false")<<",\"result\":"<<popup.result3D8
                <<",\"timer\":"<<popup.timer3C4<<",\"cancel_label\":"<<(popup.hasCancel3D4?"true":"false")<<"}\n";
        }
        if(status==Status::unavailableDestination||status==Status::secondControllerRequired){
            ++unavailableCount;std::cerr<<"original_ui_destination unavailable screen="<<unsigned(receipt.source.screen)
                <<" page="<<unsigned(receipt.source.page)
                <<" status="<<unsigned(status)<<" original_input="<<unsigned(sourceInput)<<" device_observation="<<unsigned(presence)<<" source="<<diagnostic.sourcePC
                <<" native_course_load="<<unsigned(diagnostic.courseLoad)
                <<" main="<<unsigned(diagnostic.main.status)<<" options="<<unsigned(diagnostic.options.status)
                <<" garage_phase="<<rr::platform::originalUiGarageDestinationStatusName50KG(diagnostic.garage.status)
                <<" garage_request_pc="<<diagnostic.garage.requestSourcePC<<" garage_pending_pc="<<diagnostic.garage.pendingSourcePC
                <<" garage_asset_error="<<unsigned(diagnostic.garage.scene.error)<<" garage_asset_pc="<<diagnostic.garage.scene.sourcePC
                <<garageRuntimeFailureText()
                <<" tournament_status="<<unsigned(diagnostic.tournament.status)<<" tournament_pc="<<diagnostic.tournament.sourcePC
                <<" tournament_resource="<<unsigned(diagnostic.tournamentResources.error)<<" tournament_resource_pc="<<diagnostic.tournamentResources.sourcePC
                <<" tournament_widgets="<<unsigned(diagnostic.tournamentWidgets.error)<<" tournament_widgets_pc="<<diagnostic.tournamentWidgets.sourcePC
                <<" tournament_model="<<unsigned(diagnostic.tournament.model.error)<<" tournament_draw="<<unsigned(diagnostic.tournament.draw.error)
                <<" hall_status="<<unsigned(diagnostic.hall.status)<<" hall_pc="<<diagnostic.hall.sourcePC
                <<" hall_resource="<<unsigned(diagnostic.hallResources.error)<<" hall_resource_pc="<<diagnostic.hallResources.sourcePC
                <<" options_resource="<<unsigned(diagnostic.optionsResources.error)
                <<" options_resource_widget="<<diagnostic.optionsResources.widgetId
                <<" options_resource_pc="<<diagnostic.optionsResources.sourcePC
                <<" options_camera="<<unsigned(diagnostic.optionsResources.camera.status)
                <<" options_controls_model="<<unsigned(diagnostic.optionsResources.controlsModel.error)
                <<" options_controls_model_pc="<<diagnostic.optionsResources.controlsModel.sourcePC
                <<" options_controls_asset="<<unsigned(diagnostic.optionsResources.controlsModel.asset.error)
                <<" options_controls_geometry="<<diagnostic.optionsResources.controlsModel.asset.geometry.code
                <<" options_controls_geometry_offset="<<diagnostic.optionsResources.controlsModel.asset.geometry.offset
                <<" options_controls_geometry_detail="<<diagnostic.optionsResources.controlsModel.asset.geometry.detail
                <<" options_fire="<<unsigned(diagnostic.optionsResources.fire)
                <<" options_lighting="<<unsigned(diagnostic.optionsResources.lighting)
                <<" options_render_state="<<unsigned(diagnostic.optionsResources.renderState)
                <<" options_widget_status="<<unsigned(diagnostic.optionsResources.widgets.status)
                <<" options_widget_id="<<diagnostic.optionsResources.widgets.widgetId
                <<" options_widget_controller="<<unsigned(diagnostic.optionsResources.widgets.controller)
                <<" options_widget_parse="<<unsigned(diagnostic.optionsResources.widgets.parse.error)
                <<" options_widget_parse_offset="<<diagnostic.optionsResources.widgets.parse.byteOffset
                <<" options_widget_parse_pc="<<diagnostic.optionsResources.widgets.parse.sourcePC
                <<" options_widget_text="<<unsigned(diagnostic.optionsResources.widgets.text)
                <<" options_widget_icon="<<unsigned(diagnostic.optionsResources.widgets.icon)
                <<" preparation_status="<<unsigned(diagnostic.preparationStatus)
                <<" rider="<<unsigned(diagnostic.rider.status)<<" rider_pc="<<diagnostic.rider.sourcePC
                <<" course="<<unsigned(diagnostic.course.status)<<" course_pc="<<diagnostic.course.sourcePC
                <<" course_resource="<<unsigned(diagnostic.courseResources.error)
                <<" course_resource_pc="<<diagnostic.courseResources.sourcePC
                <<" course_resource_widget="<<diagnostic.courseResources.widgetId
                <<" course_resource_material="<<diagnostic.courseResources.materialIndex
                <<" course_resource_image="<<diagnostic.courseResources.imageIndex
                <<" course_material="<<unsigned(diagnostic.courseResources.material.error)
                <<" course_material_field="<<diagnostic.courseResources.material.field
                <<" course_material_pc="<<diagnostic.courseResources.material.sourcePC
                <<" course_material_parse="<<unsigned(diagnostic.courseResources.material.document.error)
                <<" course_material_parse_offset="<<diagnostic.courseResources.material.document.byteOffset
                <<" course_material_parse_pc="<<diagnostic.courseResources.material.document.sourcePC
                <<" course_images="<<unsigned(diagnostic.courseResources.resource.error)
                <<" course_images_image="<<diagnostic.courseResources.resource.image
                <<" course_image_decode="<<diagnostic.courseResources.resource.texture.code
                <<" course_image_decode_offset="<<diagnostic.courseResources.resource.texture.offset
                <<" course_atm="<<diagnostic.courseResources.atm.code
                <<" course_atm_offset="<<diagnostic.courseResources.atm.offset
                <<" course_font="<<unsigned(diagnostic.courseResources.resource.font.error)
                <<" course_font_offset="<<diagnostic.courseResources.resource.font.byteOffset
                <<" course_font_record="<<diagnostic.courseResources.resource.font.record
                <<" course_font_ascii="<<diagnostic.courseResources.resource.font.ascii
                <<" course_widget="<<unsigned(diagnostic.courseResources.widgets.error)
                <<" course_widget_id="<<diagnostic.courseResources.widgets.widget
                <<" course_widget_controller="<<unsigned(diagnostic.courseResources.widgets.controller)
                <<" course_widget_parse="<<unsigned(diagnostic.courseResources.widgets.parse.error)
                <<" course_widget_parse_offset="<<diagnostic.courseResources.widgets.parse.byteOffset
                <<" course_widget_parse_pc="<<diagnostic.courseResources.widgets.parse.sourcePC
                <<" course_text="<<unsigned(diagnostic.courseResources.text.error)
                <<" course_text_line="<<diagnostic.courseResources.text.line
                <<" course_text_glyph="<<unsigned(diagnostic.courseResources.text.glyph.error)
                <<" course_text_character="<<diagnostic.courseResources.text.glyph.character
                <<" course_text_ascii="<<diagnostic.courseResources.text.glyph.ascii
                <<" course_text_pc="<<diagnostic.courseResources.text.glyph.sourcePC
                <<" course_exit_text="<<unsigned(diagnostic.courseResources.exitText.status)
                <<" course_exit_text_pc="<<diagnostic.courseResources.exitText.sourcePC
                <<" course_hologram_pc="<<diagnostic.courseHolograms.sourcePC
                <<" rider_resource="<<unsigned(diagnostic.riderResources.error)
                <<" rider_resource_widget="<<diagnostic.riderResources.widgetId
                <<" rider_resource_material="<<diagnostic.riderResources.materialIndex
                <<" rider_resource_image="<<diagnostic.riderResources.imageIndex
                <<" rider_widget="<<unsigned(diagnostic.riderResources.widgets.error)
                <<" rider_widget_id="<<diagnostic.riderResources.widgets.widget
                <<" rider_parse="<<unsigned(diagnostic.riderResources.widgets.parse.error)
                <<" rider_parse_offset="<<diagnostic.riderResources.widgets.parse.byteOffset
                <<" rider_parse_pc="<<diagnostic.riderResources.widgets.parse.sourcePC
                <<" rider_selection="<<unsigned(diagnostic.riderResources.widgets.selection.status)
                <<" rider_selection_parse="<<unsigned(diagnostic.riderResources.widgets.selection.parse.error)
                <<" rider_selection_parse_offset="<<diagnostic.riderResources.widgets.selection.parse.byteOffset
                <<" rider_selection_parse_pc="<<diagnostic.riderResources.widgets.selection.parse.sourcePC
                <<" rider_material="<<unsigned(diagnostic.riderResources.material.error)
                <<" rider_material_field="<<diagnostic.riderResources.material.field
                <<" rider_material_parse="<<unsigned(diagnostic.riderResources.material.document.error)
                <<" rider_material_parse_offset="<<diagnostic.riderResources.material.document.byteOffset
                <<" rider_material_parse_pc="<<diagnostic.riderResources.material.document.sourcePC
                <<" rider_images="<<unsigned(diagnostic.riderResources.resource.error)
                <<" rider_images_image="<<diagnostic.riderResources.resource.image
                <<" rider_font="<<unsigned(diagnostic.riderResources.resource.font.error)
                <<" rider_font_offset="<<diagnostic.riderResources.resource.font.byteOffset
                <<" rider_font_record="<<diagnostic.riderResources.resource.font.record
                <<" rider_font_ascii="<<diagnostic.riderResources.resource.font.ascii
                <<" rider_text="<<unsigned(diagnostic.riderResources.text.error)
                <<" rider_text_line="<<diagnostic.riderResources.text.line
                <<" rider_text_glyph="<<unsigned(diagnostic.riderResources.text.glyph.error)
                <<" rider_text_character="<<diagnostic.riderResources.text.glyph.character
                <<" rider_text_ascii="<<diagnostic.riderResources.text.glyph.ascii
                <<" rider_text_pc="<<diagnostic.riderResources.text.glyph.sourcePC
                <<" rider_exit_text="<<unsigned(diagnostic.riderResources.exitText.status)
                <<" rider_exit_text_pc="<<diagnostic.riderResources.exitText.sourcePC
                <<" rider_exit_parse="<<unsigned(diagnostic.riderResources.exitText.parse.error)
                <<" rider_exit_parse_offset="<<diagnostic.riderResources.exitText.parse.byteOffset
                <<" rider_exit_parse_pc="<<diagnostic.riderResources.exitText.parse.sourcePC
                <<" rider_exit_glyph="<<unsigned(diagnostic.riderResources.exitText.text.error)
                <<" rider_exit_character="<<diagnostic.riderResources.exitText.text.character
                <<" rider_exit_ascii="<<diagnostic.riderResources.exitText.text.ascii
                <<" rider_exit_glyph_pc="<<diagnostic.riderResources.exitText.text.sourcePC
                <<" rider_stats="<<unsigned(diagnostic.riderStats.error)
                <<" rider_binder="<<unsigned(diagnostic.riderBinder.error)
                <<" rider_draw="<<unsigned(diagnostic.rider.draw.error)
                <<" rider_draw_pc="<<diagnostic.rider.draw.sourcePC
                <<" rider_draw_surface="<<diagnostic.rider.draw.geometrySurface
                <<" rider_draw_component="<<diagnostic.rider.draw.geometryComponent
                <<" rider_draw_colors="<<diagnostic.rider.draw.geometryStoredColors
                <<" rider_draw_geometry="<<unsigned(diagnostic.rider.draw.geometryStatus)
                <<" rider_draw_material="<<unsigned(diagnostic.rider.draw.materialDetail.error)
                <<" rider_draw_material_index="<<diagnostic.rider.draw.material
                <<" rider_draw_material_pass="<<diagnostic.rider.draw.materialDetail.pass
                <<" rider_draw_widget="<<diagnostic.rider.draw.widget
                <<" rider_draw_field="<<diagnostic.rider.draw.materialDetail.field
                <<" rider_dynamic="<<unsigned(diagnostic.rider.dynamic.error)
                <<" rider_dynamic_widget="<<diagnostic.rider.dynamic.widget
                <<" rider_pod="<<unsigned(diagnostic.rider.dynamic.pod.error)
                <<" rider_pod_pc="<<diagnostic.rider.dynamic.pod.sourcePC
                <<" rider_pod_text="<<unsigned(diagnostic.rider.dynamic.pod.text.error)
                <<" rider_pod_text_offset="<<diagnostic.rider.dynamic.pod.text.byteOffset
                <<" rider_pod_hierarchy="<<unsigned(diagnostic.rider.dynamic.pod.hierarchy.code)
                <<" rider_pod_hierarchy_offset="<<diagnostic.rider.dynamic.pod.hierarchy.byteOffset
                <<" rider_pod_geometry="<<diagnostic.rider.dynamic.pod.geometry.code
                <<" rider_pod_geometry_offset="<<diagnostic.rider.dynamic.pod.geometry.offset
                <<" rider_pod_geometry_detail="<<diagnostic.rider.dynamic.pod.geometry.detail
                <<" rider_character="<<unsigned(diagnostic.rider.dynamic.character.error)
                <<" rider_character_pc="<<diagnostic.rider.dynamic.character.sourcePC
                <<" rider_character_motion="<<diagnostic.rider.dynamic.character.motionOrdinal
                <<" rider_character_model="<<unsigned(diagnostic.rider.dynamic.character.model.code)
                <<" rider_character_model_offset="<<diagnostic.rider.dynamic.character.model.byteOffset
                <<" rider_character_binding="<<unsigned(diagnostic.rider.dynamic.character.binding.error)
                <<" rider_character_binding_offset="<<diagnostic.rider.dynamic.character.binding.offset
                <<" rider_character_geometry="<<diagnostic.rider.dynamic.character.geometry.code
                <<" rider_character_geometry_offset="<<diagnostic.rider.dynamic.character.geometry.offset
                <<" rider_backend="<<unsigned(diagnostic.rider.backend.error)
                <<" gpu="<<unsigned(diagnostic.gpu.error)
                <<" gpu_operation="<<(diagnostic.gpu.operation?diagnostic.gpu.operation:"")
                <<" gpu_texture="<<diagnostic.gpu.texture
                <<" gpu_bank_texture_count="<<diagnostic.gpu.bankTextureCount
                <<" gpu_bank_rgba_bytes="<<diagnostic.gpu.bankRgbaBytes
                <<" gpu_maximum_bytes="<<diagnostic.gpu.maximumGpuBytes
                <<" gpu_bank_maximum_bytes="<<diagnostic.gpu.maximumBankRgbaBytes
                <<" gpu_validated_prefix_bytes="<<diagnostic.gpu.validatedPrefixRgbaBytes
                <<" gpu_candidate_bytes="<<diagnostic.gpu.candidateRgbaBytes
                <<" gpu_candidate_identity="<<diagnostic.gpu.candidateIdentity
                <<" gpu_candidate_width="<<diagnostic.gpu.candidateWidth
                <<" gpu_candidate_height="<<diagnostic.gpu.candidateHeight
                <<" renderer="<<unsigned(diagnostic.renderer.error)
                <<"; no destination or gameplay entered.\n";
        }else if(status!=Status::ok&&status!=Status::noChange&&status!=Status::routeAccepted)fail("input");
        return status;
    }

void Frontend::reportElapsedFailure(const original_ui_elapsed50LH::Interval& interval,std::uint64_t before,
        std::uint64_t now,std::uint64_t pending,StageClock::time_point observedNow) const {
        const auto snapshot=coordinator.snapshot();
        std::cerr<<"original_ui_interval_failure {\"error\":"<<unsigned(interval.error)
            <<",\"frame\":"<<frameCount<<",\"accepted_update_sequence\":"<<snapshot.updates
            <<",\"screen\":"<<unsigned(snapshot.route.screen)<<",\"page\":"<<unsigned(snapshot.route.page)
            <<",\"course\":"<<snapshot.course.selectedCourse44<<",\"planet\":"<<snapshot.course.selectedPlanet40
            <<",\"course_refresh_updates\":"<<snapshot.course.deferred14D0
            <<",\"update_clock_domain\":\"monotonicNow50DU_relative\",\"stage_clock_domain\":\"steady_clock_since_epoch\""
            <<",\"host_sample_observed_steady_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(observedNow.time_since_epoch()).count()
            <<",\"update_begin_nanoseconds\":"<<before<<",\"update_end_nanoseconds\":"<<now
            <<",\"wall_nanoseconds\":"<<interval.wallNanoseconds<<",\"pending_cold_nanoseconds\":"<<pending
            <<",\"excluded_cold_nanoseconds\":"<<interval.excludedColdNanoseconds
            <<",\"active_nanoseconds\":"<<interval.activeNanoseconds<<",\"recent_stages\":[";
        const auto oldest=(nextStageObservation+stageHistory.size()-stageObservationCount)%stageHistory.size();
        for(std::size_t i=0;i<stageObservationCount;++i){const auto& stage=stageHistory[(oldest+i)%stageHistory.size()];
            if(i)std::cerr<<',';
            std::cerr<<"{\"stage\":\""<<stage.name<<"\",\"frame\":"<<stage.frame<<",\"sequence\":"<<stage.sequence
                <<",\"screen\":"<<unsigned(stage.screen)<<",\"page\":"<<unsigned(stage.page)
                <<",\"steady_begin_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.begin.time_since_epoch()).count()
                <<",\"steady_end_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.end.time_since_epoch()).count()
                <<",\"wall_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.end-stage.begin).count()
                <<",\"okay\":"<<(stage.okay?"true":"false")<<'}';
        }
        std::cerr<<"]}\n";
    }

std::uint64_t Frontend::acceptedElapsed(std::uint64_t now) {
        // Observation paired immediately after the caller's relative host
        // sample. It labels clock domains; it never advances either clock.
        const auto observedNow=StageClock::now();
        const auto before=lastUpdateNanoseconds,pending=pendingUpdateColdNanoseconds;
        const auto interval=original_ui_elapsed50LH::consume(lastUpdateNanoseconds,now,pendingUpdateColdNanoseconds);
        if(interval.error!=original_ui_elapsed50LH::Error::none)reportElapsedFailure(interval,before,now,pending,observedNow);
        require(interval.error!=original_ui_elapsed50LH::Error::backwards,"original UI update clock moved backwards");
        require(interval.error!=original_ui_elapsed50LH::Error::coldExceedsWall,"UI cold interval exceeds update interval");
        // Native wall accounting is not the source-dispatched simulation factor.
        // Original140C50 preserves raw250, then140C54..8C caps the factor passed
        // through the real root/UI routers. Keep PCM/material time uncapped.
        std::uint64_t active=now;
        if(before){
            require(interval.activeNanoseconds<=std::numeric_limits<std::uint64_t>::max()-sourceDispatchActiveNanoseconds,
                "UI source dispatch active timeline overflow");
            active=sourceDispatchActiveNanoseconds+interval.activeNanoseconds;
        }
        require(sourceDispatchTiming.prepare(active,!before,sourceDispatchSample),
            "UI source dispatch timestamp/factor preparation rejected");
        sourceDispatchActiveNanoseconds=active;
        if(interval.longActiveInterval)
            std::cerr<<"original_ui_long_interval active_nanoseconds="<<interval.activeNanoseconds
                <<" source_delta_milliseconds="<<sourceDispatchSample.deltaMilliseconds
                <<" source_raw250="<<sourceDispatchSample.raw250
                <<" source_update_seconds="<<sourceDispatchSample.updateSeconds
                <<" policy=source_capped_dispatch_pcm_full_interval\n";
        return interval.activeNanoseconds;
    }

rr::platform::OriginalUiFrontendUpdateReceipt50KG Frontend::update(
        const rr::platform::OriginalUiMainSessionUpdate50KG& in,std::uint64_t elapsedNanoseconds) {
        rr::platform::OriginalUiFrontendUpdateReceipt50KG receipt;
        const auto updateStart=stageStart();
        const bool garageRoot=coordinator.route().screen==13;
        const bool sourceAudio=materialApplicationOwner&&materialApplicationOwner->garageAudioBackingOwner_;
        const bool ordinarySourceAudio=sourceAudio&&!garageRoot;
        original_ui_source_timing50LH::PublicationView garageTiming;
        original_ui_source_timing50LH::Sample garageSample;std::uint64_t garagePublication=0;
        if(garageRoot){
            // Root13 owns its real Light/Aural/dispatcher traversal. Capture
            // THIS prepared publication before it commits; do not invoke the
            // ordinary-manager prefix or manufacture another audio sample.
            // A Garage-first race return has no preceding ordinary manager:
            // its very first update is this Garage Root's publication 1.
            const bool firstGarageFrame=nativeRootAudioPhase_==NativeRootAudioPhase::absent&&
                nativeRootAudioPublication_==1&&!lastAudioSequence;
            require(sourceAudio&&(firstGarageFrame||(nativeRootAudioPhase_==NativeRootAudioPhase::returned&&
                nativeRootAudioPublication_!=std::numeric_limits<std::uint64_t>::max()&&
                lastAudioSequence!=std::numeric_limits<std::uint64_t>::max()))&&
                sourceDispatchTiming.retainPublication(garageTiming)&&
                (firstGarageFrame||garageTiming.sameOwner(nativeRootAudioTiming_))&&
                garageTiming.readPrepared(garageSample,garagePublication)&&
                garagePublication==nativeRootAudioPublication_+1&&
                garageSample.updateSeconds==in.elapsedSeconds,
                "Garage host lost its exact preceding Root/audio/timing publication");
        }
        std::string rootAudioError;
        if(ordinarySourceAudio&&!materialApplicationOwner->beginNativeFrontendRootAudio140CE0(*this,in,diagnostic,rootAudioError)){
            reportStage("source_update",updateStart,false);throw std::runtime_error(rootAudioError);
        }
        const auto beforeUpdateRoute=coordinator.snapshotWithoutGarageRuntime();
        bool updated=coordinator.update(in,receipt,diagnostic)==Status::ok&&receipt.accepted&&receipt.sequence==lastAudioSequence+1;
        if(ordinarySourceAudio&&updated)
            updated=materialApplicationOwner->finishNativeFrontendRootAudio140CFC(*this,receipt,diagnostic,rootAudioError);
        if(updated){
            if(garageRoot){
                original_ui_source_timing50LH::Sample stillPrepared;std::uint64_t pending=0;
                updated=receipt.source.screen==13&&receipt.source.page==1&&receipt.sourceTimingCommitted&&
                    receipt.sourcePublication==garagePublication&&
                    receipt.update.elapsedSeconds==garageSample.updateSeconds&&receipt.update.paused==in.paused&&
                    !garageTiming.readPrepared(stillPrepared,pending);
            }else updated=!receipt.sourceTimingCommitted&&!receipt.sourcePublication&&receipt.source.screen!=13;
            if(!updated)rootAudioError="Native frontend returned an inconsistent Root timing/audio receipt; source effects retained";
        }
        reportStage("source_update",updateStart,updated);
        accountBankPublication(receipt.coldInterval,receipt.bankDrain,updated);
        if(!updated){
            if(!rootAudioError.empty())throw std::runtime_error(rootAudioError);
            fail("update");
        }
        // Memory Card Back completes asynchronously: its accepted return is
        // emitted by update, not by the Escape input receipt. Clear page-only
        // Save/Load help on that actual request while retaining real warnings.
        const auto afterUpdateRoute=coordinator.snapshotWithoutGarageRuntime();
        if(beforeUpdateRoute.route.screen==12&&beforeUpdateRoute.route.page==5&&
            beforeUpdateRoute.transition.requested.screen==12&&beforeUpdateRoute.transition.requested.page==5&&
            (afterUpdateRoute.transition.requested.screen!=12||afterUpdateRoute.transition.requested.page!=5)){
            nativeBetaLoadPending=false;
            showBetaSettingsStatus();
        }
        // One PCM advance for this exact accepted source update; never for
        // loading, rejected updates, or a second screen's copied clock.
        rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
        const auto audioStart=stageStart();
        const bool advanced=audioOutput.advanceAcceptedNanoseconds(elapsedNanoseconds,outputDiagnostic);
        reportStage("pcm_advance",audioStart,advanced);
        if(!advanced)
            throw std::runtime_error(std::string("original UI PCM advance failed: ")+outputDiagnostic.detail.data());
        lastAudioSequence=receipt.sequence;
        if(garageRoot){
            // PCM above is still required: Root267BE0 drains controls, not
            // samples. Observe its actual completed publication only after
            // this one PCM drain succeeds. Preserve chronology for a later
            // ordinary manager instead of resetting its prefix cursor.
            nativeRootAudioTiming_=garageTiming;nativeRootAudioPublication_=garagePublication;
            nativeRootAudioExpectedUpdate_=receipt.sequence;nativeRootAudioPhase_=NativeRootAudioPhase::returned;
            nativeRootAudioSourcePC_=0x140D60;
        }
        return receipt;
    }

void Frontend::render(const rr::platform::OriginalUiMainFrameClock50KG& clock,
        rr::platform::VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height) {
        const auto buildStart=stageStart();
        const bool rendered=coordinator.render(clock,diagnostic)==Status::ok;
        reportStage("source_build",buildStart,rendered);
        if(!rendered)fail("render");
        const auto submitStart=stageStart();
        const bool submitted=coordinator.submit(renderer,width,height,diagnostic)==Status::ok;
        reportStage("native_submit",submitStart,submitted,&diagnostic.renderer);
        if(!submitted)fail("submission");
        const auto submittedState=coordinator.snapshot();submittedScreen=submittedState.route.screen;
        submittedGaragePublication=submittedScreen==13&&submittedState.route.page==1&&
            submittedState.garageTargetPublished?submittedState.garagePublication:0;
        submittedPublication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
        submittedDraws=diagnostic.renderer.originalUiDraws;submittedQuads=diagnostic.renderer.originalUiQuads;
        submittedTriangles=diagnostic.renderer.originalUiSourceTriangles;
        submittedTextureBarriers=diagnostic.renderer.originalUiTextureBarriers;
        submittedCoherentFramebufferFetch=diagnostic.renderer.originalUiCoherentFramebufferFetch;
        sourceFrameGeneration=diagnostic.renderer.originalUiSourceFrameGeneration;
        sourceTargetGeneration=diagnostic.renderer.originalUiSourceTargetGeneration;
    }

void Frontend::commitDeferred(rr::platform::VehicleWindowGl50DS& renderer) {
        const auto deferredStart=stageStart();
        rr::platform::OriginalUiFrontendDeferredReceipt50KG receipt;
        const auto status=coordinator.commitDeferred(renderer,receipt,diagnostic);
        const bool okay=status==Status::ok||status==Status::noChange||status==Status::courseLaunchReady;
        reportStage("deferred_commit",deferredStart,okay);
        accountBankPublication(receipt.coldInterval,receipt.bankDrain,okay);
        if(status==Status::courseLaunchReady){
            require(materialApplicationOwner,"Course material directory owner missing");
            materialApplicationOwner->enterGameplayMaterialDirectory17D7A0();
            imagePublished=false;return;
        }
        if(status==Status::ok){imagePublished=false;std::cerr<<"original_ui_screen_activated "<<unsigned(coordinator.snapshot().route.screen)<<'\n';}
        else if(status!=Status::noChange)fail("deferred activation");
    }

bool Frontend::advanceCourseRaLoadingPrefix50LA(std::uint32_t& pc)noexcept {
        pc=0x17d7a0;
        return materialApplicationOwner&&materialApplicationOwner->advanceCourseRaLoadingPrefix50LA(*this,pc);
    }

bool Frontend::closeUi() noexcept {
        if(uploadLifetime_&&(uploadLifetime_->active_||
            uploadLifetime_->thread_!=std::this_thread::get_id()))return false;
        const auto status=coordinator.close(diagnostic);
        if(status!=Status::ok&&status!=Status::noChange)return false;
        if(materialApplicationOwner&&!materialApplicationOwner->withdrawCourseRaLoadingPrefix50LA(*this))return false;
        if((nativeResultsHallEntry_||nativeRaceReturnScreen_)&&materialApplicationOwner&&
           !materialApplicationOwner->cancelNativeResultsHallTransfer50LA())return false;
        if((nativePauseMainEntry_||nativePauseCourseEntry_)&&materialApplicationOwner&&
           !materialApplicationOwner->cancelNativePauseMenuTransfer50LA())return false;
        if(uploadLifetime_){uploadLifetime_->retired_=true;uploadLifetime_.reset();}
        return true;
    }

bool Frontend::closeAudio() noexcept {
        rr::platform::AudioOutputDiagnostic50HD d;const bool okay=audioOutput.close(d);
        if(!okay)std::cerr<<"original UI audio close error="<<unsigned(d.error)<<'\n';return okay;
    }

const rr::game::OriginalUiNavigation50KG* Frontend::navigation() const noexcept {
        const auto route=coordinator.route();
        if(route.screen==4&&route.page==1){
            const auto* resources=coordinator.mainResources();
            return resources&&resources->manager()?resources->manager()->navigation(1):nullptr;
        }
        if(route.screen==6&&route.page==1){
            const auto* resources=coordinator.courseResources();
            return resources&&resources->widgets()?resources->widgets()->navigation():nullptr;
        }
        if(route.screen==9&&route.page==1){
            const auto* session=coordinator.tournamentSession();return session?session->navigation():nullptr;
        }
        if(route.screen==11&&route.page==1){
            const auto* session=coordinator.hallSession();const auto* state=session?session->navigationState():nullptr;
            return state?&state->navigation:nullptr;
        }
        if(route.screen==12){
            const auto* resources=coordinator.optionsResources();
            const auto* widgets=resources?resources->widgets():nullptr;
            if(!widgets)return nullptr;
            if(route.page==1)return widgets->navigation();
            if(route.page==5)return widgets->memoryCardNavigation();
            if(route.page==6&&widgets->creditsState())return &widgets->creditsState()->navigation;
            if(route.page==4){const auto* state=widgets->gameOptionsState();return state?&state->navigation:nullptr;}
            if(route.page==2||route.page==3){const auto* session=coordinator.optionsSession();
                const auto* state=session?session->controlsState(route.page-2):nullptr;return state?&state->navigation:nullptr;}
        }
        return nullptr;
    }

void Frontend::report(const char* reason,const Application& application,std::uint64_t worldTicks,std::uint64_t mixedFrames) const {
        const auto snapshot=coordinator.snapshot();const bool main=snapshot.route.screen==4;
        // Observation only: do not force a Rider-readable mode while Main owns
        // mode4. The public read-only snapshot legitimately returns false then.
        const auto reportRiderProfile=[&]() {
            if(!application.riderProfile)return;
            rr::game::OriginalUiRiderSelectionProfile50KG profile;
            const bool readable=application.riderProfile->selectionSnapshot(profile);
            const auto stamp=application.riderProfile->stamp();
            const auto& common=*application.profile;
            const auto& settings=*application.gameSettingsProfile;
            std::cerr<<",\"rider_profile_initialized\":"<<(application.riderProfile->initialized()?"true":"false")
                <<",\"rider_profile_readable\":"<<(readable?"true":"false")
                <<",\"rider_profile_common_link\":"<<(application.riderProfile->commonProfile()==application.profile.get()?"true":"false")
                <<",\"rider_profile_generation\":"<<stamp.generation<<",\"rider_profile_mode\":"<<stamp.mode34
                <<",\"rider_profile_remembered\":";
            if(readable)std::cerr<<'['<<profile.remembered6F0<<','<<profile.remembered6F4<<','
                <<profile.remembered6F8<<','<<profile.remembered6FC<<']';
            else std::cerr<<"null";
            std::cerr<<",\"main_profile_mode\":"<<common.mode34<<",\"main_profile_previous_mode\":"<<common.previousMode38
                <<",\"main_profile_saved_slot\":"<<common.savedSlot6E0
                <<",\"main_profile_availability\":"<<unsigned(common.availability11018)
                <<",\"main_profile_flags\":"<<unsigned(common.flags11054)
                <<",\"main_profile_player_bits\":"<<unsigned(common.playerBits1125C)
                <<",\"application_game_profile\":{\"sound_mode\":"<<settings.soundMode1101C
                <<",\"volumes\":["<<settings.volume11020[0]<<','<<settings.volume11020[1]<<','
                <<settings.volume11020[2]<<','<<settings.volume11020[3]<<']'
                <<",\"flags\":"<<unsigned(settings.flags11050)<<",\"saved_group\":"<<settings.savedGroup6E8
                <<",\"sound_revision\":"<<application.soundMode->revision()<<'}'
                <<",\"light_manager_generation\":"<<application.lightManager->managerGeneration()
                <<",\"light_cache_state\":"<<unsigned(application.lightManager->cacheState());
        };
        if(snapshot.route.screen==13){
            const auto& aperture=snapshot.garageAperture;
            const bool accepted=snapshot.route.page==1&&snapshot.frameSubmitted&&snapshot.garageTargetPublished&&
                snapshot.garageRootReturned&&snapshot.garageTimingCommitted&&submittedScreen==13&&
                snapshot.garagePublication!=0&&submittedGaragePublication==snapshot.garagePublication;
            std::cerr<<"original_garage_state {\"reason\":\""<<reason<<"\",\"screen\":13,\"page\":"<<unsigned(snapshot.route.page)
                <<",\"presentation\":\"native_source_rgba_target\",\"switches\":"<<snapshot.switches
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"loaded\":"<<(snapshot.garageLoaded?"true":"false")<<",\"selected_stat\":"<<(snapshot.garageLoaded?std::to_string(snapshot.garageSelectedStat):"null")
                <<",\"source_publication\":"<<snapshot.garagePublication
                <<",\"root_returned\":"<<(snapshot.garageRootReturned?"true":"false")
                <<",\"timing_committed\":"<<(snapshot.garageTimingCommitted?"true":"false")
                <<",\"target_published\":"<<(snapshot.garageTargetPublished?"true":"false")
                <<",\"accepted_native_target\":"<<(accepted?"true":"false")
                <<",\"lifecycle\":"<<snapshot.garageManagerLifecycle<<",\"phase\":"<<snapshot.garageManagerTransition
                <<",\"aperture_bound\":"<<(aperture.bound?"true":"false")<<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"selected_group\":null,\"selected_slot\":null,\"gpu_publication\":null"
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==6){
            require(snapshot.route.page==1&&coordinator.courseSession(),"original Course session report unavailable");
            const auto& course=snapshot.course;const auto& aperture=course.aperture;
            const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=submittedScreen==6&&submittedPublication==publication&&snapshot.frameSubmitted;
            std::cerr<<"original_course_state {\"reason\":\""<<reason<<"\",\"screen\":6,\"page\":1"
                <<",\"presentation\":\"native_host_rgb\",\"switches\":"<<snapshot.switches
                <<",\"gpu_publication\":"<<publication<<",\"updates\":"<<snapshot.updates
                <<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"mode34\":"<<course.mode34<<",\"tier14E8\":"<<course.tier14E8
                <<",\"entry_writes_laps_entrants\":"<<(course.mode34==0?"true":"false")
                <<",\"course_slot\":"<<course.courseSlot14E4<<",\"subtype_slot\":"<<course.subtypeSlot14F4
                <<",\"selected_planet\":"<<course.selectedPlanet40<<",\"selected_course_index\":"<<course.selectedCourse44
                <<",\"selection_refresh_updates\":"<<course.deferred14D0
                <<",\"laps\":"<<course.laps14EC<<",\"entrants\":"<<course.entrants14F0
                <<",\"input_shown\":"<<(course.inputContainerShown?"true":"false")
                <<",\"popup_enabled\":"<<(course.popupEnabled438?"true":"false")
                <<",\"prompt_mask\":"<<course.promptMask42C
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
                <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
                <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
                <<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"accepted_native_frame\":"<<(accepted?"true":"false")
                <<",\"quads\":"<<(accepted?submittedQuads:0)<<",\"draws\":"<<(accepted?submittedDraws:0)
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
                <<",\"audio_suppressed\":"<<course.audio.suppressedByMutedMode
                <<",\"audio_submitted\":"<<course.audio.submitted<<",\"audio_failed\":"<<course.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"audio_captured_frames\":"<<audioOutput.info().capturedFrames
                <<",\"audio_rate\":"<<audioOutput.info().sampleRate
                <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()
                <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioOutput.info().deviceStatus)<<"\""
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==5){
            require(snapshot.route.page==1&&coordinator.riderSession(),"original Rider session report unavailable");
            const auto& rider=snapshot.rider;const auto& aperture=rider.aperture;
            const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=submittedScreen==5&&submittedPublication==publication&&sourceFrameGeneration!=0;
            std::cerr<<"original_rider_state {\"reason\":\""<<reason<<"\",\"screen\":5,\"page\":1"
                <<",\"switches\":"<<snapshot.switches<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"selected_slot\":"<<rider.slot4C8<<",\"selected_rider\":"<<rider.rider4C4
                <<",\"selection_attached\":"<<(rider.selectionAttached?"true":"false")
                <<",\"bank_generation\":"<<rider.bankGeneration<<",\"registry_generation\":"<<rider.registryGeneration
                <<",\"input_shown\":"<<(rider.inputContainerShown?"true":"false")
                <<",\"input_blocked\":"<<(rider.blocked4E0?"true":"false")<<",\"panel_y\":"<<rider.panelY4DC
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
                <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
                <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
                <<",\"prompt_mask\":"<<rider.promptMask42C<<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"accepted_source_frame\":"<<(accepted?"true":"false")
                <<",\"source_frame_generation\":"<<(accepted?sourceFrameGeneration:0)
                <<",\"source_target_generation\":"<<(accepted?sourceTargetGeneration:0)
                <<",\"quads\":"<<(accepted?submittedQuads:0)<<",\"draws\":"<<(accepted?submittedDraws:0)
                <<",\"source_triangles\":"<<(accepted?submittedTriangles:0)
                <<",\"source_coherent_framebuffer_fetch\":"<<(accepted&&submittedCoherentFramebufferFetch?"true":"false")
                <<",\"source_texture_barriers\":"<<(accepted?submittedTextureBarriers:0)
                <<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
                <<",\"audio_suppressed\":"<<rider.audio.suppressedByMutedMode
                <<",\"audio_submitted\":"<<rider.audio.submitted<<",\"audio_failed\":"<<rider.audio.failed
                <<",\"audio_source_gate_suppressed\":"<<rider.audio.suppressedBySourceGate
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"audio_captured_frames\":"<<audioOutput.info().capturedFrames
                <<",\"audio_rate\":"<<audioOutput.info().sampleRate
                <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()
                <<",\"audio_clock_microseconds\":"<<audioOutput.info().simulationMicroseconds
                <<",\"audio_remainder_nanoseconds\":"<<audioOutput.remainderNanoseconds()
                <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioOutput.info().deviceStatus)<<"\""
                <<",\"audio_device_consumed_frames\":"<<audioOutput.info().deviceConsumedFrames
                <<",\"audio_device_queued_frames\":"<<audioOutput.info().deviceQueuedFrames
                <<",\"audio_device_underrun_frames\":"<<audioOutput.info().deviceUnderrunFrames
                <<",\"audio_device_overflow_frames\":"<<audioOutput.info().deviceOverflowFrames
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";
            return;
        }
        if(snapshot.route.screen==9){
            require(snapshot.route.page==1&&coordinator.tournamentSession(),"original Tournament session report unavailable");
            const auto& tournament=snapshot.tournament;const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=snapshot.frameSubmitted&&submittedScreen==9&&submittedPublication==publication&&sourceFrameGeneration!=0;
            const auto* nav=navigation();require(nav,"original Tournament navigation report unavailable");
            const auto group=nav->controllerGroup[0];
            const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
            std::cerr<<"original_tournament_state {\"reason\":\""<<reason<<"\",\"screen\":9,\"page\":1"
                <<",\"career\":"<<tournament.career<<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
                <<",\"page_shown\":"<<(tournament.pageShown?"true":"false")<<",\"input_shown\":"<<(tournament.inputContainerShown?"true":"false")
                <<",\"camera_active\":"<<(tournament.cameraActive?"true":"false")<<",\"timer4064\":"<<tournament.timer4064
                <<",\"audio_submitted\":"<<tournament.audio.submitted<<",\"audio_failed\":"<<tournament.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"prompt_mask\":"<<tournament.promptMask42C<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"aperture_state\":"<<unsigned(tournament.aperture.state)
                <<",\"source_rgba_accepted\":"<<(accepted?"true":"false")
                <<",\"source_frame\":"<<sourceFrameGeneration<<",\"source_target\":"<<sourceTargetGeneration
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==11){
            require(snapshot.route.page==1&&coordinator.hallSession(),"original Hall session report unavailable");
            const auto& hall=snapshot.hall;const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=snapshot.frameSubmitted&&submittedScreen==11&&submittedPublication==publication&&sourceFrameGeneration!=0;
            const auto* nav=navigation();require(nav,"original Hall navigation report unavailable");
            const auto group=nav->controllerGroup[0];
            const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
            std::cerr<<"original_hall_state {\"reason\":\""<<reason<<"\",\"screen\":11,\"page\":1"
                <<",\"course_index\":"<<hall.courseIndex<<",\"planet_index\":"<<hall.planetIndex
                <<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
                <<",\"audio_submitted\":"<<hall.audio.submitted<<",\"audio_failed\":"<<hall.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"deferred1460\":"<<hall.deferred1460<<",\"video_paused\":"<<(hall.videoPaused84?"true":"false")
                <<",\"presentation_pending\":"<<(hall.movementPresentationPending?"true":"false")
                <<",\"prompt_mask\":"<<hall.promptMask42C<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"aperture_state\":"<<unsigned(hall.aperture.state)
                <<",\"source_rgba_accepted\":"<<(accepted?"true":"false")
                <<",\"source_frame\":"<<sourceFrameGeneration<<",\"source_target\":"<<sourceTargetGeneration
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==7||snapshot.route.screen==10){
            // UIWin 7 / UIUnlock 10: page state from the frontend snapshot.
            std::cerr<<"original_"<<(snapshot.route.screen==7?"win":"unlock")<<"_state {\"reason\":\""<<reason<<"\",\"screen\":"
                <<unsigned(snapshot.route.screen)<<",\"page\":"<<unsigned(snapshot.route.page)
                <<",\"page_shown\":"<<(snapshot.screenPageShown?"true":"false")<<",\"input_shown\":"<<(snapshot.screenInputShown?"true":"false")
                <<",\"aperture_state\":"<<unsigned(snapshot.screenAperture.state)<<",\"input_enabled\":"<<(snapshot.screenAperture.inputEnabled268?"true":"false")
                <<",\"unlock_pod\":"<<snapshot.unlockPod2D40<<",\"unlock_first_pending\":"<<snapshot.unlockFirstPending
                <<",\"unlock_activations\":"<<snapshot.unlockActivations<<",\"unlock_grants\":"<<snapshot.unlockGrants
                <<",\"win_circuit\":"<<snapshot.winCircuit<<",\"win_place\":"<<snapshot.winPlace<<",\"win_career\":"<<snapshot.winCareer
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        const auto* optionsResources=coordinator.optionsResources();
        const auto* nav=navigation();require(nav,"original UI live navigation missing");
        const auto group=nav->controllerGroup[0];
        const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
        const auto* gameOptions=snapshot.route.screen==12&&snapshot.route.page==4&&optionsResources?
            optionsResources->widgets()->gameOptionsState():nullptr;
        const auto* mainSession=coordinator.mainSession();
        const auto* fire=main?(mainSession?mainSession->fire():nullptr):(optionsResources?optionsResources->fire():nullptr);
        require(fire,"original UI live particle owner missing");
        const auto aperture=main?snapshot.main.aperture:snapshot.options.aperture;
        const auto audio=main?snapshot.main.audio:snapshot.options.audio;
        const auto* draw=coordinator.draw();const auto audioInfo=audioOutput.info();
        std::cerr<<"original_main_state {\"reason\":\""<<reason<<"\",\"screen\":"<<unsigned(snapshot.route.screen)
            <<",\"page\":"<<unsigned(snapshot.route.page)<<",\"switches\":"<<snapshot.switches
            <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
            <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
            <<",\"gpu_publication\":"<<(coordinator.gpu()?coordinator.gpu()->publicationToken():0)
            <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<frameCount
            <<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
            <<",\"saved_slot\":"<<(main?application.profile->savedSlot6E0:application.optionsProfile->savedSlot6E4)
            <<",\"saved_group\":"<<application.gameSettingsProfile->savedGroup6E8
            <<",\"game_options_accepts\":"<<snapshot.options.gameOptionsAcceptCount
            <<",\"game_sound_apply_revision\":"<<snapshot.options.lastGameOptionsSoundRevision
            <<",\"application_sound_revision\":"<<application.soundMode->revision()
            <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
            <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"fire_serial\":"<<application.state->fireSerial433970
            <<",\"input_shown\":"<<((main?snapshot.main.inputContainerShown:snapshot.options.inputContainerShown)?"true":"false")
            <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
            <<",\"prompt_mask\":"<<(main?snapshot.main.promptMask42C:snapshot.options.promptMask42C)
            <<",\"aperture_state\":"<<unsigned(aperture.state)
            <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
            <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
            <<",\"particles\":"<<fire->particleCount()<<",\"source_packets\":"<<fire->packetCount()
            <<",\"quads\":"<<(draw?draw->draw.quadCount:0)<<",\"spans\":"<<(draw?draw->draw.spanCount:0)
            <<",\"source_milliseconds\":"<<sourceMilliseconds<<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
            <<",\"audio_suppressed\":"<<audio.suppressedByMutedMode<<",\"audio_submitted\":"<<audio.submitted
            <<",\"audio_source_gate_suppressed\":"<<audio.suppressedBySourceGate<<",\"audio_failed\":"<<audio.failed
            <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()<<",\"audio_mixed_frames\":"<<audioInfo.mixedFrames
            <<",\"audio_captured_frames\":"<<audioInfo.capturedFrames<<",\"audio_rate\":"<<audioInfo.sampleRate
            <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()<<",\"audio_clock_microseconds\":"<<audioInfo.simulationMicroseconds
            <<",\"audio_remainder_nanoseconds\":"<<audioOutput.remainderNanoseconds()
            <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioInfo.deviceStatus)<<"\""
            <<",\"audio_device_consumed_frames\":"<<audioInfo.deviceConsumedFrames<<",\"audio_device_queued_frames\":"<<audioInfo.deviceQueuedFrames
            <<",\"audio_device_underrun_frames\":"<<audioInfo.deviceUnderrunFrames<<",\"audio_device_overflow_frames\":"<<audioInfo.deviceOverflowFrames
            <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
        const auto& popup=main?snapshot.main.controllerPopup:snapshot.options.controllerPopup;
        std::cerr<<",\"controller_popup_active\":"<<(popup.active3CC?"true":"false")
            <<",\"controller_popup_closing\":"<<(popup.closing3C8?"true":"false")
            <<",\"controller_popup_result\":"<<popup.result3D8<<",\"controller_popup_timer\":"<<popup.timer3C4;
        const auto& saved=*application.gameSettingsProfile;
        std::cerr<<",\"game_profile_sound_mode\":"<<saved.soundMode1101C
            <<",\"game_profile_volumes\":["<<saved.volume11020[0]<<','<<saved.volume11020[1]<<','<<saved.volume11020[2]<<','<<saved.volume11020[3]<<']'
            <<",\"game_profile_flags\":"<<unsigned(saved.flags11050);
        if(gameOptions&&gameOptions->loaded){
            const auto& local=gameOptions->local;
            std::cerr<<",\"game_local_auto_save\":"<<(local.autoSave?"true":"false")
                <<",\"game_local_sound_mode\":"<<local.soundMode
                <<",\"game_local_volumes\":["<<local.volume[0]<<','<<local.volume[1]<<','<<local.volume[2]<<','<<local.volume[3]<<']'
                <<",\"game_local_vertical_split\":"<<(local.verticalSplit?"true":"false")
                <<",\"game_local_vibration\":"<<(local.vibration?"true":"false");
        }
        if(snapshot.route.screen==12&&snapshot.route.page==5){
            const auto* session=coordinator.optionsSession();const auto* card=session?session->memoryCardState():nullptr;
            if(card)std::cerr<<",\"memory_card_dialog_state\":"<<card->dialogState30
                <<",\"memory_card_active\":"<<(card->dialogActive?"true":"false")
                <<",\"memory_card_closing\":"<<(card->closing3AC?"true":"false")
                <<",\"memory_card_complete\":"<<(card->completed3A4?"true":"false")
                <<",\"memory_card_timer\":"<<card->timer398
                <<",\"memory_card_storage_supported\":false";
        }
        if(snapshot.route.screen==12&&snapshot.route.page==6&&optionsResources){
            const auto* credits=optionsResources->widgets()->creditsState();
            if(credits)std::cerr<<",\"credits_slot\":"<<credits->navigation.groups[1].selectedSlot
                <<",\"credits_phase\":"<<unsigned(credits->phase7D4)
                <<",\"credits_remaining\":"<<credits->remaining7D8
                <<",\"credits_input_flags\":"<<credits->inputFlags42C;
        }
        if(application.controlsProfile){
            std::cerr<<",\"controls_accepts\":"<<snapshot.options.controlsAcceptCount
                <<",\"controls_saved_presets\":["<<application.controlsProfile->slots[0].preset11048<<','
                <<application.controlsProfile->slots[1].preset11048<<']';
            if(snapshot.route.screen==12&&(snapshot.route.page==2||snapshot.route.page==3)){
                const auto* session=coordinator.optionsSession();
                const auto* control=session?session->controlsState(snapshot.route.page-2):nullptr;
                if(control)std::cerr<<",\"controls_local_preset\":"<<control->preset4D8
                    <<",\"controls_local_configuration\":"<<control->configuration4D4
                    <<",\"controls_front_view\":"<<(control->frontView4CC?"true":"false")
                    <<",\"controls_model_blocked\":"<<(control->modelBlocked4D0?"true":"false");
            }
        }
        if(application.riderProfile)std::cerr<<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0);
        reportRiderProfile();std::cerr<<"}\n";
    }

std::shared_ptr<Video> Application::videoForSession() const {
    require(rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this),
        "Video factory requires its live Application owner thread");
    require(raceDisplayOwner50LA_.expired(),"previous race display lifetime still retained");
    if(!videoOwner_){
        videoOwner_=std::shared_ptr<Video>(new Video(enabled,animationClock(),*this));
        videoOwner_->sourcePreFlip18_.bindPublishedVideo(videoOwner_);
    }
    require(videoOwner_->originalMain==enabled,
        "native application cannot replace its original/direct video mode");
    require((!videoOwner_->host.created()&&!videoOwner_->host.ownsResources())||
        (retainPlayerWindow50DU_&&enabled&&videoOwner_->host.hasWindow()&&
         videoOwner_->host.created()&&videoOwner_->host.current()),
        "native session requires retired display and closed or retained current player Video");
    return videoOwner_;
}

bool Application::closePlayerWindow50DU(rr::platform::GameHostDiagnostic50ER& d)const noexcept {
    d={};
    // Session locals and Root service borrowers must retire first. GameHost
    // independently refuses closure while any display pins/resources survive.
    if(!raceDisplayOwner50LA_.expired()){
        d.error=rr::platform::GameHostError50ER::AlreadyOwned;
        d.operation="terminal player window still has a display owner";return false;
    }
    // Terminal path also covers stopping before the incoming frontend starts
    // or a failed receiver preparation. Never cancel ahead of real session
    // cleanup, and never dereference publication-local producer/observer pins.
    if((pauseMenuTransfer50LA_&&sourceRace28COwner_)||pauseInputContinuation50LA_||!cancelNativePauseMenuTransfer50LA()){
        d.error=rr::platform::GameHostError50ER::AlreadyOwned;
        d.operation="terminal player window still has a Pause menu transfer";return false;
    }
    return !videoOwner_||!videoOwner_->host.ownsResources()||videoOwner_->host.close(d);
}

bool Application::stageTournamentPodReferencesCold(
    std::shared_ptr<const rr::platform::OriginalUiTournamentCareerResources50KG> resource,
    std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>& out,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;d={};
    const auto reject=[&]()noexcept{d.status=S::phase;d.sourcePC=0x1D3590;return false;};
    if(out||!resource||!resource->current()||!riderProfile||!profile||
       riderProfile->commonProfile()!=profile.get()||!resource->sameProfileOwner(*riderProfile)||
       !videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
       !materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       materialContextOwner_->materialApplication()!=materialApplication||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return reject();
    std::uint8_t packed=0;std::uint32_t selected=0;
    const auto* career=resource->career();
    if(!career||!riderProfile->riderConfirmSnapshot(packed,selected)||selected!=(packed>>3)||
       career->career100C0!=selected||riderProfile->stamp().mode34!=2)return reject();
    try {
        // Publish owner before acquiring any canonical numeric reference: a
        // failure leaves its actual retained prefix available for cleanup.
        if(tournamentPodConstructionGeneration_==std::numeric_limits<std::uint64_t>::max()){
            d.status=S::overflow;d.sourcePC=0x1D3590;return false;
        }
        out=std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>(
            new rr::platform::OriginalUiTournamentPodReferences50KG(*this));
        const auto generation=++tournamentPodConstructionGeneration_;
        const bool okay=out->stage(this,std::move(resource),standardMaterialDomainOwner_,
                                  materialContextOwner_,entitiesOwner_,generation,materialDirectory36B970_,d);
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||
            out->constructionEffects();
        return okay;
    }catch(...){d.status=S::allocation;return false;}
}

bool Application::releaseTournamentPodReferencesUnpublished(
    rr::platform::OriginalUiTournamentPodReferences50KG& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    return owner.releaseUnpublished(this,d);
}

bool Application::stageTimeStatsNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,TimeStatsColdResult50LA& out,bool deferPopulation)const noexcept {
    using namespace rr::assets;using Phase=TimeStatsColdResult50LA::Phase;
    // Never erase a prior partial result or replace an outstanding owner.
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.font.sourceEffectsCommitted||out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}
        return false;
    };
    try {
        if(!actualSessionGeneration||!timeStatsColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||
           !Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("TimeStats requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;
        const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        const auto localization=readArchive(originalRoot,OriginalUiArchiveId50KG::localization);
        auto english=std::make_shared<OriginalUiEnglish50KG>();
        if(!readOriginalUiEnglish50KG(*localization,*english,out.localization))
            return fail("TimeStats authentic English localization rejected");
        out.phase=Phase::sources;
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        for(const char* name:{"ui\\textures\\stats.psm","font\\eurostileboldbold13ingame.psm"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("TimeStats authentic PSM missing");
            auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
            const std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(member.identity->name,bytes,out.source))
                return fail("TimeStats original PSM parse rejected");
            materials.push_back(std::move(source));
        }
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        for(const char* name:{"ui\\textures\\stats.pst","font\\eurostileboldbold13ingame.pst",
                             "font\\denmarkbold16ingame.pst"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("TimeStats authentic PST missing");
            textures.push_back({member.identity->name,
                std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        // Construct the genuine detached base before resource loader effects.
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalTimeStats50LA>(new rr::game::OriginalTimeStats50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays,localization,english));
        timeStatsColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("TimeStats canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("TimeStats canonical corpus append rejected");
        {
            // Actual1A8A6C/1A8AA0 outer path, restored at native boundary
            // corresponding to1A90CC even on partial failure/exception.
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::stats;
            if(!out.owner->stageStats193240(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("TimeStats stats resource stage rejected");
            out.phase=Phase::fonts;
            if(!loadTimeStatsFonts193A0C(overlays,out.font)||
               !out.owner->borrowFonts193A24(*fonts328Owner_,out.object))
                return fail("TimeStats Application font stage rejected");
            out.phase=Phase::text;
            if(!out.owner->stageTextReferences193A58(materialDirectory36B970_,out.object))
                return fail("TimeStats text-reference stage rejected");
        }
        out.phase=Phase::pixels;
        const auto stats=overlays->find("ui\\textures\\stats.pst");
        const auto font=overlays->find("font\\eurostileboldbold13ingame.pst");
        const auto cel=overlays->find("font\\eurostileboldbold13ingame.cel");
        if(!cel.identity||!cel.data||cel.size!=cel.identity->bytes)
            return fail("TimeStats original13ingame CEL missing");
        // IDs0/1 are local bank slots only, not original resource identities.
        // Exact member provenance is retained separately by the existing owner.
        std::vector<OriginalUiImageSource50KG> images{
            {0,stats,0,stats.size,{}},{1,font,0,font.size,cel}};
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("TimeStats original pixel bridge rejected");
        out.pixels=pixels;
        if(!out.owner->borrowTextPixels(out.pixels,1,out.object))
            return fail("TimeStats canonical font/pixel bridge mismatch");
        if(deferPopulation){out.phase=Phase::awaitingFinalVerification;return true;}
        out.phase=Phase::population;
        // Explicit native cold policy corresponding to the later original
        //1AA5A0 resource pair and1A7B14 collector. This is not six per-text
        //population calls and does not certify full retail race chronology.
        if(!domain->populate(context,out.material))
            return fail("TimeStats later canonical population/collector rejected");
        out.sourceEffectsCommitted=true;
        out.phase=Phase::awaitingFinalVerification;
        if(!out.owner->finishNativeColdPreparation50LA(out.pixels,0,1,
            videoOwner_->width,videoOwner_->height,out.object))
            return fail("TimeStats canonical cold verification rejected");
        // CPU/source resource preparation only. Actual view binding, frame
        //projection, GPU upload and renderer retirement are separate joins.
        out.phase=Phase::cpuPrepared;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("TimeStats cold preparation exception");}
}

bool Application::finishTimeStatsNativeCold50LA(TimeStatsColdResult50LA& out)const noexcept {
    using Phase=TimeStatsColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       timeStatsColdOwner_.lock()!=out.owner||!videoOwner_||
       !Video::materialConstructionPinLive(*videoOwner_))return false;
    if(!out.owner->finishNativeColdPreparation50LA(out.pixels,0,1,
        videoOwner_->width,videoOwner_->height,out.object)){
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.failure="TimeStats final shared collector verification rejected";return false;
    }
    out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
}

bool Application::stageDamageNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,DamageColdResult50LA& out)const noexcept {
    using namespace rr::assets;using Phase=DamageColdResult50LA::Phase;
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}return false;
    };
    try {
        if(!actualSessionGeneration||!damageColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("Damage requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        out.phase=Phase::sources;
        const auto pst=overlays->find("ui\\textures\\dmgradar.pst");
        if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
           overlays->find("ui\\textures\\dmgradar.psm").identity)
            return fail("Damage authentic PST/archive absence proof rejected");
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        textures.push_back({pst.identity->name,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalDamage50LA>(new rr::game::OriginalDamage50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays));
        damageColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("Damage canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("Damage canonical source append rejected");
        {
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::textures;
            if(!out.owner->stageTextures18C6F0(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("Damage texture stage rejected");
        }
        out.phase=Phase::pixels;
        std::vector<OriginalUiImageSource50KG> images{{0u,pst,0,pst.size,{}}};
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("Damage authentic pixel bridge rejected");
        out.pixels=pixels;
        // Root's later real population/collector join owns final sealing.
        // This owner never rebuilds the collector to manufacture readiness.
        out.phase=Phase::awaitingFinalVerification;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("Damage cold preparation exception");}
}

bool Application::finishDamageNativeCold50LA(DamageColdResult50LA& out)const noexcept {
    using Phase=DamageColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       damageColdOwner_.lock()!=out.owner)return false;
    try {
        if(!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           out.owner->context_!=materialContextOwner_||
           out.owner->domain_!=standardMaterialDomainOwner_||
           !out.owner->finishNativeColdPreparation50LA(out.pixels,0,out.object)){
            out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted;
            out.failedAt=out.phase;out.phase=Phase::failed;
            out.failure="Damage final canonical collector verification rejected";return false;
        }
        out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
    }catch(...){out.failedAt=out.phase;out.phase=Phase::failed;return false;}
}

bool Application::stageSpeedBoostNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,SpeedBoostColdResult50LA& out)const noexcept {
    using namespace rr::assets;using Phase=SpeedBoostColdResult50LA::Phase;
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.font.sourceEffectsCommitted||out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}return false;
    };
    try {
        if(!actualSessionGeneration||!speedBoostColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("Speed/Boost requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        const auto localization=readArchive(originalRoot,OriginalUiArchiveId50KG::localization);
        auto english=std::make_shared<OriginalUiEnglish50KG>();
        if(!readOriginalUiEnglish50KG(*localization,*english,out.localization))
            return fail("Speed/Boost authentic English localization rejected");
        out.phase=Phase::sources;
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        for(const char* name:{"font\\eurostileboldbolditalic26ingame.psm","font\\eurostileboldbold13ingame.psm"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("Speed/Boost authentic font PSM missing");
            auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
            const std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(member.identity->name,bytes,out.source))return fail("Speed/Boost original font PSM rejected");
            materials.push_back(std::move(source));
        }
        constexpr const char* names[]={"ui\\textures\\spdbost1","ui\\textures\\boost_a1",
            "ui\\textures\\boost_a2","ui\\textures\\boost_a3","ui\\textures\\boost_a4",
            "ui\\textures\\boost_a5","ui\\textures\\boost_a6","ui\\textures\\boost_a7",
            "ui\\textures\\boost_a8","ui\\textures\\boost_a9",
            "font\\eurostileboldbolditalic26ingame","font\\eurostileboldbold13ingame"};
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        std::array<OriginalUiMemberView50KG,12> sourceImages{};
        for(std::size_t i=0;i<12;++i){
            const auto member=overlays->find(std::string(names[i])+".pst");
            if(!member.identity||!member.data||member.size!=member.identity->bytes||
               (i<10&&overlays->find(std::string(names[i])+".psm").identity))
                return fail("Speed/Boost authentic texture/absence proof rejected");
            sourceImages[i]=member;
            textures.push_back({member.identity->name,std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalSpeedBoost50LA>(new rr::game::OriginalSpeedBoost50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays,localization,english));
        speedBoostColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("Speed/Boost canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("Speed/Boost canonical source append rejected");
        {
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::textures;
            if(!out.owner->stageTextures1895B0(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("Speed/Boost texture stage rejected");
            out.phase=Phase::triangle;
            if(!out.owner->stageTriangle18A77C(materialDirectory36B970_,out.object))
                return fail("Speed/Boost triangle stage rejected");
            out.phase=Phase::fonts;
            if(!loadSpeedBoostFonts18A99C(overlays,out.font)||
               !out.owner->borrowFonts18A9B4(*fonts328Owner_,out.object))
                return fail("Speed/Boost actual font stage rejected");
            out.phase=Phase::text;
            if(!out.owner->stageTextReferences18A9DC(materialDirectory36B970_,out.object))
                return fail("Speed/Boost text-reference stage rejected");
        }
        out.phase=Phase::pixels;
        std::vector<OriginalUiImageSource50KG> images;
        for(std::size_t i=0;i<12;++i){
            OriginalUiMemberView50KG cel{};
            if(i>=10){cel=overlays->find(std::string(names[i])+".cel");
                if(!cel.identity||!cel.data||cel.size!=cel.identity->bytes)
                    return fail("Speed/Boost authentic CEL missing");}
            images.push_back({static_cast<std::uint32_t>(i),sourceImages[i],0,sourceImages[i].size,cel});
        }
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("Speed/Boost authentic pixel bridge rejected");
        out.pixels=pixels;
        // No final population/collector here. Native root stages TimeStats
        // next; its later actual1AA5A0/1A7B14 join resolves BOTH sets of refs.
        out.phase=Phase::awaitingFinalVerification;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("Speed/Boost cold preparation exception");}
}

bool Application::finishSpeedBoostNativeCold50LA(SpeedBoostColdResult50LA& out)const noexcept {
    using Phase=SpeedBoostColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       speedBoostColdOwner_.lock()!=out.owner)return false;
    try {
        constexpr std::array<std::uint32_t,12> images{{0,1,2,3,4,5,6,7,8,9,10,11}};
        if(!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !out.owner->finishNativeColdPreparation50LA(out.pixels,images,
                videoOwner_->width,videoOwner_->height,out.object)){
            out.failedAt=out.phase;out.phase=Phase::failed;
            out.failure="Speed/Boost final canonical population/collector verification rejected";return false;
        }
        out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
    }catch(...){out.failedAt=out.phase;out.phase=Phase::failed;return false;}
}

std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path& root,rr::assets::OriginalUiArchiveId50KG id) {
    const auto* identity=rr::assets::originalUiArchiveIdentity50KG(id);
    require(identity,"original UI archive catalog identity missing");
    // The compiled authenticated archive catalog is the only path authority.
    // No private workspace path, recursive search, basename fallback or cache.
    const auto path=root/std::filesystem::path(identity->discPath);
    std::ifstream input(path,std::ios::binary|std::ios::ate);
    if(!input)throw std::runtime_error("cannot open original UI archive: "+path.string());
    const std::streamoff bytes=input.tellg();
    // The stock size is a receipt (the archive owner logs it); the read is
    // bounded by the shared UI decode capacity the owner decodes within.
    if(bytes<=0 || static_cast<std::uint64_t>(bytes)>rr::assets::DecodeLimits48C{}.resource_file_bytes)
        throw std::runtime_error("original UI archive outside the shared UI capacity: "+path.string());
    std::vector<std::uint8_t> raw(static_cast<std::size_t>(bytes));
    input.seekg(0);
    input.read(reinterpret_cast<char*>(raw.data()),static_cast<std::streamsize>(raw.size()));
    if(!input)throw std::runtime_error("cannot read complete original UI archive: "+path.string());
    auto archive=std::make_shared<rr::assets::OriginalUiArchive50KG>();
    rr::assets::OriginalUiArchiveDiagnostic50KG diagnostic;
    if(!archive->loadOwned(id,std::move(raw),diagnostic))
        throw std::runtime_error("original UI archive authentication/decode rejected: "+path.string()+
            " status="+std::to_string(static_cast<unsigned>(diagnostic.status))+
            " member="+std::to_string(diagnostic.member));
    return archive;
}

void Application::showBetaPersistenceStatus(const char* normalTitle) const noexcept {
    if(!videoOwner_||!videoOwner_->host.hasWindow())return;
    using Result=rr::platform::native_beta50kg::Result;
    using Progress=rr::platform::OriginalUiRiderProfileOwner50KG::BetaPersistenceStatus;
    const auto progress=riderProfile?riderProfile->betaPersistenceStatus():Progress::notStarted;
    const bool settingsFailed=betaSettingsStatus!=Result::ok&&betaSettingsStatus!=Result::missing;
    const bool progressFailed=progress==Progress::unavailable||progress==Progress::locked||
        progress==Progress::invalid||progress==Progress::readError||progress==Progress::writeError||
        progress==Progress::durabilityUnknown;
    if(!normalTitle&&!settingsFailed&&!progressFailed)return;
    rr::platform::GameHostDiagnostic50ER d;
    try {
        std::string title=normalTitle?normalTitle:"Star Wars: Racer Revenge";
        if(settingsFailed)title+=betaSettingsStatus==Result::durabilityUnknown?
            " | SETTINGS SAVE UNCONFIRMED":" | SETTINGS NOT SAVED";
        if(progressFailed)title+=progress==Progress::durabilityUnknown?
            " | CAREER SAVE UNCONFIRMED":" | CAREER NOT SAVED";
        if(!videoOwner_->host.setWindowTitle(title.c_str(),d))
            std::cerr<<"BETA SAVE: native window status unavailable.\n";
    }catch(...){
        videoOwner_->host.setWindowTitle("Star Wars: Racer Revenge | SAVE STATUS UNAVAILABLE - check application log",d);
    }
}

} // namespace original_main_entry50KG

namespace original_main_entry50KG {
bool Application::stageNativeLoading330Prefix(std::uint32_t& pc)const noexcept {
    pc=0x17cde4;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!entitiesOwner_||
       entitiesOwner_->nativeOwnerThread_!=std::this_thread::get_id()||!entitiesOwner_->application10())return false;
    if(nativeLoading330Attempted_){
        pc=sourceLoading330Owner_?sourceLoading330Owner_->stopPC_:0x17cde4u;
        return sourceLoading330Owner_&&sourceLoading330Owner_->prefixRetained(); //No replay.
    }
    if(sourceLoading330Owner_)return false;
    nativeLoading330Attempted_=true;
    try {sourceLoading330Owner_=std::shared_ptr<rr::game::OriginalLoadingManager33050LA>(
        new rr::game::OriginalLoadingManager33050LA(this,entitiesOwner_));}
    catch(...){return false;}
    auto& owner=*sourceLoading330Owner_;
    if(!owner.construct17CDF8())return false;
    owner.slot330Published_=true; //17CE0C actual counted owner, before init.
    return owner.prepare17BF90(pc);
}
bool Application::retireNativeLoading330Prefix()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    if(!sourceLoading330Owner_)return true;
    auto& owner=*sourceLoading330Owner_;
    if(owner.camera_){
        const auto display=owner.viewportDisplay_.lock();
        const bool selected=display&&display->sourceSelectedCamera0_==owner.camera_;
        // A Page/Options prior borrow must retire before this process owner.
        // Never invalidate a live dependent or substitute a new Display.
        if(owner.camera_.use_count()!=(selected?2:1))return false;
        if(selected){
            if(display->application_!=this||!display->ownerThread()||!display->sourceCurrentCamera0Known_)return false;
            display->selectFrontendCamera26F940({}); //Explicit native shutdown withdrawal.
        }
    }
    if(!sourceLoading330Owner_->retireNativePrefix())return false;
    sourceLoading330Owner_.reset();return true; //Native withdrawal, never source330 null proof.
}
}


namespace original_main_entry50KG {
bool Application::prepareFrontendLoading330Viewport(
    const std::shared_ptr<RaceDisplayLifetime50LA>& display,
    rr::platform::VehicleWindowDiagnostic50DS& diagnostic,std::uint32_t& pc)const noexcept {
    pc=0x114a00;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    // Same actual Application-issued Display, not an arbitrary native window
    // or copied buffer-count receipt. Gameplay create has unknown Camera0 and
    // no243CB4 completion, and cannot pass this existing initialized predicate.
    if(!display||display!=raceDisplayOwner50LA_.lock()||display->application_!=this||
       !display->garageFirstViewportCurrent50KG(diagnostic))return false;
    if(!stageNativeLoading330Prefix(pc))return false;
    auto& owner=*sourceLoading330Owner_;
    if(owner.viewportAttempted_){
        pc=owner.stopPC_;
        // A serial frontend recreation does not repeat a process constructor
        // or its old renderer invocation. No callback may consume that older
        // Display merely because this new one has equivalent dimensions.
        return owner.viewportCompleted_&&owner.prefixRetained();
    }
    auto& camera=*owner.camera_;
    if(!owner.prefixRetained()||camera.issuer_!=this||camera.entities_!=entitiesOwner_||
       !camera.coldRetirementPreflight().okay())return false;
    // Pin exact same renderer/video for this synchronous invocation only.
    // No retained GPU work or callback is produced; the later closed
    //constructor suffix may select the exact330 Camera before attachments.
    display->pin();
    owner.viewportAttempted_=true;owner.boundary114A00_=false;
    owner.viewportDisplay_=display;
    const bool completed=[&]()noexcept {
        const std::uint32_t count=display->sourceBufferCount0C_+std::uint32_t{1};
        camera.fields_.count210=count; //114A24, BEFORE20C.
        camera.fields_.count20C=count; //114A28.
        camera.phase_=rr::game::OriginalFrontendCameraCold50LA::Phase::insideViewport114A00;
        owner.stopPC_=pc=0x245900;
        std::int32_t returned=0;
        const auto service=rr::game::serviceOriginalRenderer244D60(display->dispatch_,true,
            display->renderer_,diagnostic,returned,rr::game::OriginalCameraDrainPolicy::initializedContext);
        if(service.status!=rr::game::OriginalCameraStartStatus::complete){
            owner.stopPC_=pc=service.sourcePC;return false;
        }
        owner.viewportServicePhase_=1;owner.stopPC_=pc=0x245910;
        if(!display->renderer_.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)return false;
        owner.viewportServicePhase_=2;
        //245918/91C reads this actual known-null Camera0, skipping directly
        //to245C68. Neither zoom writes nor packet/scissor branch is reached.
        camera.phase_=rr::game::OriginalFrontendCameraCold50LA::Phase::afterViewport114A00;
        owner.viewportCompleted_=true;owner.stopPC_=pc=0x1149e0;
        owner.suffixAttempted_=true;
        using Camera=rr::game::OriginalFrontendCameraCold50LA;
        auto& f=camera.fields_;
        const auto fp=[](std::uint32_t bits)noexcept{return Camera::floatFromBits(bits);};
        const auto words=[](const std::array<float,16>& input)noexcept {
            Camera::Words16 result{};
            for(std::size_t i=0;i<16;++i)result[i]=Camera::floatBits(input[i]);
            return result;
        };
        const auto vector=[&](const Camera::Words4& v)noexcept {
            return rr::assets::TVCameraVector49E{fp(v[0]),fp(v[1]),fp(v[2]),fp(v[3])};
        };
        rr::game::CameraSceneState49F scene;
        scene.look=vector(f.look190);scene.up=vector(f.up1A0);scene.fovDegrees=fp(f.fov1D0);
        scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
        scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
        while(owner.constructorCallbacks_<2){
            owner.stopPC_=pc=owner.constructorCallbacks_?0x11471c:0x1149e0;
            rr::game::CameraViewOutput49F core;float focal=0,angle=0;
            const auto rebuilt=rr::game::rebuildOriginalSceneCore115350(scene,vector(f.position180),
                static_cast<std::int32_t>(f.viewport1E0[2]),core,focal);
            if(rebuilt.status!=rr::game::OriginalCameraStartStatus::complete){owner.stopPC_=pc=rebuilt.sourcePC;return false;}
            const auto guarded=rr::game::computeOriginalGuardAngle2371D0(focal,angle);
            if(guarded.status!=rr::game::OriginalCameraStartStatus::complete){owner.stopPC_=pc=guarded.sourcePC;return false;}
            f.viewC0=words(core.view);f.projection80=words(core.projection);
            f.combined100=words(core.combined);f.focal1D8=Camera::floatBits(focal);f.coreKnown=true;
            //Same bounded constructor treatment as real Page/Options: early
            //source bounds are unwritten; dependent caches remain UNKNOWN.
            f.screen140Known=false;f.combinedScreen250Known=false;
            f.guardProjection290Known=false;f.guardCombined2D0Known=false;
            f.guardAngle330=Camera::floatBits(angle);++owner.constructorCallbacks_;
        }
        owner.stopPC_=pc=0x26f940;
        if(!display->garageFirstViewportCurrent50KG(diagnostic)||display->sourceSelectedCamera0_||
           !(camera.entity_.flags29&1u))return false;
        display->selectFrontendCamera26F940(owner.camera_); //26F948 BEFORE dimension comparison.
        camera.phase_=Camera::Phase::selectedBeforeBounds236214;
        if(display->sourceWidth4_!=static_cast<std::int32_t>(f.fullWidth204)||
           display->sourceHeight8_!=static_cast<std::int32_t>(f.fullHeight208))return false;
        rr::game::OriginalCameraViewportFields numeric;
        const auto prepared=rr::game::prepareOriginalRaceViewport2361C0(numeric,camera.generation_,camera.key_,scene);
        if(prepared.status!=rr::game::OriginalCameraStartStatus::complete){owner.stopPC_=pc=prepared.sourcePC;return false;}
        f.depth310=Camera::floatBits(numeric.depth310);f.depth314=Camera::floatBits(numeric.depth314);
        f.left318=Camera::floatBits(numeric.left318);f.right31C=Camera::floatBits(numeric.right31C);
        f.top320=Camera::floatBits(numeric.top320);f.bottom324=Camera::floatBits(numeric.bottom324);
        f.aspect1F8=Camera::floatBits(scene.aspect1F8);f.matrix140=words(numeric.screen140);f.screen140Known=true;
        f.combinedScreen250=words(numeric.combinedScreen250);f.combinedScreen250Known=true;
        f.guardCombined2D0=words(numeric.guardCombined2D0);f.guardCombined2D0Known=true;
        f.guardX334=Camera::floatBits(numeric.guardX334);f.guardY338=Camera::floatBits(numeric.guardY338);
        camera.phase_=Camera::Phase::viewportInitialized2361C0; //290 remains unknown.
        owner.stopPC_=pc=0x17c06c;
        const auto attached=entitiesOwner_->entityAuthority_.attachChildren(owner.manager_,camera.entity_,
            0xffffffffu,entitiesOwner_->attachmentLock423F58());
        if(attached.status!=rr::game::OriginalWreckComponentStatus50LA::complete)return false;
        owner.cameraAttached_=true;
        //17C07C table+1C=1415A0, then actual node and Camera+1C=1415A0.
        //Exact freshly constructed two-child tree; bit3 skip is genuinely clear.
        if(owner.manager_.firstChild18!=&owner.node_||owner.node_.next14!=&camera.entity_||
           camera.entity_.next14||owner.node_.firstChild18||camera.entity_.firstChild18||
           (owner.node_.flags29&8u)||(camera.entity_.flags29&8u))return false;
        owner.manager_.flags29&=static_cast<std::uint8_t>(~3u);
        owner.node_.flags29&=static_cast<std::uint8_t>(~3u);
        camera.entity_.flags29&=static_cast<std::uint8_t>(~3u);
        //1415A0 changes activity only; NEVER clears selected Display0.
        owner.stopPC_=pc=0x17ce2c;
        const auto parented=entitiesOwner_->entityAuthority_.attachChildren(*entitiesOwner_->application10(),
            owner.manager_,0xffffffffu,entitiesOwner_->attachmentLock423F58());
        if(parented.status!=rr::game::OriginalWreckComponentStatus50LA::complete)return false;
        owner.managerAttached_=true;owner.managerInitialized_=true;
        owner.stopPC_=pc=0x17ce34;
        return true; //Only actual330 manager; screen40/1CC900 remain unimplemented.
    }();
    // Both services are synchronous. Failure records its reached prefix and
    // never retries GL work; no new queue submission or callback is retained.
    display->unpin();
    return completed;
}
}


namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::frontendLoadingCameraLifetime50LA(
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& camera)const noexcept {
    if(!camera)return true; //No prior borrow; current-null is checked separately.
    if(!ownerThread()||!application_||!sourceCurrentCamera0Known_)return false;
    const auto& owner=application_->sourceLoading330Owner_;
    if(!owner||!owner->managerInitialized_||!owner->prefixRetained()||owner->camera_!=camera||
       owner->camera_.owner_before(camera)||camera.owner_before(owner->camera_)||
       owner->viewportDisplay_.lock().get()!=this||camera->issuer_!=application_||
       camera->entities_!=application_->entitiesOwner_||!camera->enrolled_||camera->entity_.reference00!=1||
       camera->fields_.registration34||camera->fields_.parent38||camera->fields_.resource224)return false;
    return true; //Inactive is authentic17C07C, not a reason to fabricate null.
}
std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>
RaceDisplayLifetime50LA::loadingPriorViewportCamera50LA()const noexcept {
    return frontendLoadingCameraLifetime50LA(sourceSelectedCamera0_)?sourceSelectedCamera0_:
        std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>{};
}
bool RaceDisplayLifetime50LA::frontendPriorViewportCurrent50LA(
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& prior)const noexcept {
    return ownerThread()&&sourceCurrentCamera0Known_&&sourceSelectedCamera0_==prior&&
        !sourceSelectedCamera0_.owner_before(prior)&&!prior.owner_before(sourceSelectedCamera0_)&&
        frontendLoadingCameraLifetime50LA(prior);
}
bool RaceDisplayLifetime50LA::applyLoadingPriorViewportZoom2458C0(
    const rr::game::OriginalFrontendCameraCold50LA& supplied,
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& prior)noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    if(!frontendPriorViewportCurrent50LA(prior)||supplied.issuer_!=application_||
       !supplied.onAffinity()||!supplied.actualEntitiesAlive()||
       supplied.phase_!=Camera::Phase::insideViewport114A00||!supplied.scalarPrefixStateValid(true))return false;
    if(!prior)return true; //24591C authentic null branch, no zoom writes.
    rr::game::OriginalCameraViewportFields input,current;
    input.prepared=true;current.prepared=true;
    input.width1E8=static_cast<std::int32_t>(supplied.fields_.viewport1E0[2]);
    input.height1EC=static_cast<std::int32_t>(supplied.fields_.viewport1E0[3]);
    auto& f=prior->fields_;
    current.left318=Camera::floatFromBits(f.left318);current.right31C=Camera::floatFromBits(f.right31C);
    current.top320=Camera::floatFromBits(f.top320);current.bottom324=Camera::floatFromBits(f.bottom324);
    const auto computed=rr::game::updateOriginalViewportZoom2458C0(input,current);
    if(computed.status!=rr::game::OriginalCameraStartStatus::complete)return false;
    f.zoom328=Camera::floatBits(current.zoom328); //245AD4 then245AD8.
    f.zoom32C=Camera::floatBits(current.zoom32C);
    return true; //Actual a2=0 skips packet/scissor245B14..C64; no renderer receipt.
}
}
