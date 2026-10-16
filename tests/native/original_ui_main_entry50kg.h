#pragma once
namespace rr::game { class OriginalPositionStatsPrimitiveOwner50LA; class OriginalPositionStatsResources50LA; }
#include "original_ui_main_output50kg.h"
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include "../../src/assets/original_ui_material_application50la.h"
#include "../../src/assets/original_standard_material_context50la.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/original_application_fonts50la.h"
#include "../../src/game/lap_host_clock50id.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/original_motion_library50la.h"
#include "../../src/game/original_time_stats50la.h"
#include "../../src/game/original_speed_boost50la.h"
#include "../../src/game/original_damage50la.h"
#include "../../src/game/original_ui_light_manager_lifetime50kg.h"
#include <filesystem>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace original_main_entry50KG {
// Concrete host lease: the predicate captures stable native owner references,
// performs read-only exact identity/state checks. An authenticated cold-load
// plan admits only queueing; it does not fabricate a loaded world or activate
// physics from a UI callback. The containing session outlives this lease.
template<class Predicate> class CourseRaceDestination final:public rr::platform::OriginalUiCourseRaceDestination50KG {
    Predicate predicate_;
public:
    explicit CourseRaceDestination(Predicate predicate):predicate_(std::move(predicate)){}
    bool admits(const rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& selection)const noexcept override {
        try{return predicate_(selection);}catch(...){return false;}
    }
};
template<class Predicate> auto courseRaceDestination(Predicate predicate){
    return std::make_shared<CourseRaceDestination<Predicate>>(std::move(predicate));
}
inline void require(bool accepted,const char* message) {
    if(!accepted)throw std::runtime_error(message);
}
inline std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path&,rr::assets::OriginalUiArchiveId50KG);
// NEW native application startup for the original-UI opt-in. This is a real
// process owner, not an imported save or a reconstruction of retail boot.
// The cold gameplay world stays unadvanced while the frontend owns input.
// An admitted Course destination borrows this same application RNG/selector
// owner after source exit and GPU drain. A changed track/rider is carried to
// the next cold world only after actual outgoing-world teardown.
// Native whole-race restart/return keeps this same process owner. No copied
// seed or replacement selected racer is used.
struct Video;
class RaceDisplayLifetime50LA;
class NativeCollectorFifo50LA;
struct PositionStatsPrimitiveResult50LA;
struct PositionStatsResourceResult50LA;
// Cold staging result, never a drawable/renderer-ready receipt. On failure
// retain this result through diagnosis: its genuine partial owner and the
// Application's committed canonical table effects are not rolled back.
struct TimeStatsColdResult50LA {
    enum class Phase : std::uint8_t {fresh,archives,sources,owner,stats,fonts,text,pixels,
        population,awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalTimeStats50LA> owner;
    // Fixed image order:0 stats,1 authentic13ingame PST+CEL.
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalTimeStatsDiagnostic50LA object;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiLocalizationDiagnostic50KG localization;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
// Staged source resources only. Awaiting verification is not GPU readiness.
struct SpeedBoostColdResult50LA {
    enum class Phase:std::uint8_t {fresh,archives,sources,owner,textures,triangle,fonts,text,
        pixels,awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalSpeedBoost50LA> owner;
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalSpeedBoostDiagnostic50LA object;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiLocalizationDiagnostic50KG localization;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
// Uncalled cold-only Damage staging. Never a full loader/draw readiness receipt.
struct DamageColdResult50LA {
    enum class Phase:std::uint8_t {fresh,archives,sources,owner,textures,pixels,
        awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalDamage50LA> owner;
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalDamageDiagnostic50LA object;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
struct Application {
    bool enabled=false;
    // Persistent native preview request only; never a profile unlock or race grant.
    // Legacy Clegg request remains separate; each frontend authenticates its caches.
    bool prepareCleggLockedPreview=false;
    // Additive original rider resource pairs only, retained across frontend rebuilds.
    std::uint32_t requestedPreviewRiders=0;
    std::filesystem::path assetRoot,audioRoot,riderCacheRoot;
    rr::platform::OriginalUiMainAudioMode50KG audioMode=
        rr::platform::OriginalUiMainAudioMode50KG::muted;
    // One process-owned fresh normal-material setting for Main AND gameplay.
    // Original100008..100030 clears435670 AND435678 in BSS. Native debug overrides are
    // not admitted; never reinitialize this owner on page/race/session entry.
private:
    friend struct Video;
    friend class NativeCollectorFifo50LA;
    mutable std::weak_ptr<RaceDisplayLifetime50LA> raceDisplayOwner50LA_;
    // Process443848 FIFO identity. Scene/display close cannot silently reset it.
    mutable std::shared_ptr<NativeCollectorFifo50LA> nativeCollectorFifoOwner_;
    mutable std::uint64_t raceDisplayGeneration50LA_=0;
    mutable std::weak_ptr<rr::game::OriginalPositionStatsPrimitiveOwner50LA> positionStatsPrimitivesOwner50LA_;
    mutable std::weak_ptr<rr::game::OriginalPositionStatsResources50LA> positionStatsResourcesOwner50LA_;
    const std::shared_ptr<rr::game::HostMonotonicClock50ID> animationClockBackend_;
    const std::shared_ptr<rr::game::OriginalApplicationAnimationClock50KG> animationClockOwner_=
        std::shared_ptr<rr::game::OriginalApplicationAnimationClock50KG>(
            new rr::game::OriginalApplicationAnimationClock50KG);
    // Native application startup13FDC0..13FE0C, before UI/race ownership.
    // This SAME base Entity lifetime and globals survive every session.
    // Consumers pin it; no race entry creates replacement root ancestry.
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA> entitiesOwner_=
        std::make_shared<rr::game::OriginalApplicationEntities50LA>();
    //3C8D9C..3C8DC0's real process motion library, separate from Entity
    // counts and the file cache. Racer153610/153F50 share its ordered list.
    const std::shared_ptr<rr::game::OriginalMotionLibrary50LA> motionLibraryOwner_=
        std::make_shared<rr::game::OriginalMotionLibrary50LA>();
    // Retained immutable backing for the SAME process setting. The existing
    // public read name below aliases it; material consumers do not own a
    // separately resettable normal-mode copy.
    const std::shared_ptr<const rr::assets::OriginalUiMaterialRuntime50KG> materialRuntimeOwner_=
        std::make_shared<const rr::assets::OriginalUiMaterialRuntime50KG>(
            rr::assets::OriginalUiMaterialRuntime50KG{true,false,true,0u});
    // Original140030/14004C creates443870, a texture manager DISTINCT from
    // rendering Video443A60. Own its genuine CPU resource list once here.
    // The sealed TA construction context below pins this exact owner. No native GPU
    // memory manager or254AD0/254B20 frame caller is claimed or activated.
    const std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA> textureLifetimeOwner_=
        std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA>(
            new rr::assets::OriginalStandardTextureLifetime50LA);
    // One real native Video identity per application, including direct entry.
    // Each serial frontend/race still closes its GL resources and host. A later
    // session reopens this SAME host owner only after that cleanup completed.
    // CPU material resources are not GL objects and retain this stable owner;
    // no renderer, texture name or GPU readiness survives session teardown.
    mutable std::shared_ptr<Video> videoOwner_;
    // Retain the exact construction capability, not just equivalent fields.
    // Its dependencies pin Video/templates/runtime/list, never Application,
    // so these two application-owned handles do not form a reference cycle.
    mutable std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> materialContextOwner_;
    mutable std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomainOwner_;
    // First bounded Application328 registry. Declared after its canonical
    // domain/context so owned fonts release their texture leases first.
    // Scene teardown must retire every ScreenText/renderer Entry borrow;
    // the process registry itself survives serial Video close/reopen.
    mutable std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> fonts328Owner_;
    // At most one live native-host-scheduled TimeStats cold owner. Neither
    // this weak lifetime guard nor the factory activates presentation.
    mutable std::weak_ptr<rr::game::OriginalTimeStats50LA> timeStatsColdOwner_;
    mutable std::weak_ptr<rr::game::OriginalSpeedBoost50LA> speedBoostColdOwner_;
    mutable std::weak_ptr<rr::game::OriginalDamage50LA> damageColdOwner_;
    // Actual mutable application36B970. Startup17CA6C/70 copies3A85C0.
    // UI17DC14 and gameplay17D814 write this SAME field below; canonical
    // model acquisition borrows it, never derives a path from asset metadata.
    mutable std::string materialDirectory36B970_="textures\\";
    // Startup17CA4C/50 copies3A85B0; UI/gameplay write this same field.
    //154140 borrows it for a real save/clear/restore around model setup.
    mutable std::string modelDirectory36B860_="geometry\\";
    //17C710/764/808..814 copies3A85E8's exact particle-directory string.
    const std::string particleDirectory36B530_="particle\\";
    //17C848/84C copies the six original bytes at3A8600 into36B420.
    const std::string fontDirectory36B420_="font\\";
public:
    const rr::assets::OriginalUiMaterialRuntime50KG& materialRuntime=*materialRuntimeOwner_;
    rr::game::OriginalApplicationAnimationClockView50KG animationClock()const noexcept {
        return rr::game::OriginalApplicationAnimationClockView50KG(animationClockOwner_);
    }
    bool publishAnimationTimestamp(const rr::game::HostMonotonicClock50ID& backend,
        std::uint32_t sampled)const noexcept {
        if(animationClockBackend_.get()!=&backend)return false;
        animationClockOwner_->value_.current230=sampled;
        animationClockOwner_->published_=true;return true;
    }
    bool sampleAnimationTimestamp(std::uint32_t& out)const noexcept {
        std::uint32_t sampled=0;
        if(!animationClockBackend_||!rr::game::sampleHostTimestamp50ID(*animationClockBackend_,sampled)||
            !publishAnimationTimestamp(*animationClockBackend_,sampled))return false;
        out=sampled;return true;
    }
    bool animationPaused()const noexcept {return animationClockOwner_->value_.pause11C!=0;}
    // Native explicit pause/resume mapping, not17CC64 input-matcher parity.
    // No single-step120 writer is joined here; constructor120 remains zero.
    void setExplicitAnimationPause(bool paused)const noexcept {
        animationClockOwner_->value_.pause11C=paused?1u:0u;
    }
    std::shared_ptr<Video> videoForSession() const;
    std::shared_ptr<RaceDisplayLifetime50LA> raceDisplayForSession50LA() const;
    bool stagePositionStatsPrimitives50LA(const std::filesystem::path& originalRoot,
        PositionStatsPrimitiveResult50LA&) const noexcept;
    bool stagePositionStatsResources50LA(const std::filesystem::path& originalRoot,
        PositionStatsResourceResult50LA&) const noexcept;
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entities() const noexcept {
        return entitiesOwner_;
    }
    const std::shared_ptr<rr::game::OriginalMotionLibrary50LA>& motionLibrary4333C0()const noexcept {
        return motionLibraryOwner_;
    }
    const std::string& materialDirectory36B970() const noexcept {return materialDirectory36B970_;}
    std::string& modelDirectory36B860() const noexcept {return modelDirectory36B860_;}
    const std::string& particleDirectory36B530() const noexcept {return particleDirectory36B530_;}
    //160F74/88 saves the actual live path then selects36B530.161014 restores
    //that same path. Native exceptions also restore it; no shadow directory.
    template<class Operation> bool withParticleMaterialDirectory160F88(Operation&& operation) const {
        std::string previous=materialDirectory36B970_;
        materialDirectory36B970_=particleDirectory36B530_;
        try {const bool result=operation(materialDirectory36B970_);
            materialDirectory36B970_.swap(previous);return result;}
        catch(...){materialDirectory36B970_.swap(previous);throw;}
    }
    void enterUiMaterialDirectory17DB80() const {
        modelDirectory36B860_="ui\\geometry\\";materialDirectory36B970_="ui\\textures\\";
    }
    void enterGameplayMaterialDirectory17D7A0() const {
        modelDirectory36B860_="geometry\\";materialDirectory36B970_="textures\\";
    }
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomain() const {
        require(materialApplication&&materialApplication->prepared(),
            "canonical domain requires actual application APPSTART owners");
        if(!standardMaterialDomainOwner_){
            auto domain=std::make_shared<rr::assets::OriginalStandardMaterialDomain50LA>();
            rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
            require(domain->prepareFresh(materialApplication,diagnostic),
                "application canonical material/program/texture domain rejected");
            standardMaterialDomainOwner_=std::move(domain);
        }
        return standardMaterialDomainOwner_;
    }
    // Cold factory only. Video must already have minted this application's
    // actual construction context; no caller-provided replacement or ready
    // flag can create a font domain. This does not load fonts or activate HUD.
    std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> fonts328() const {
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
    // Cold caller only, before renderer publication. The font registry cannot
    // be populated through a detached directory or public texture setter.
    bool loadTimeStatsFonts193A0C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadTimeStats193A0C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
    // Source-only Speed/Boost font pair; no startup caller activates this.
    bool loadSpeedBoostFonts18A99C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadSpeedBoost18A99C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
    //Uncalled PositionStats one-font route through this SAME Application328.
    //Authentic PST/provider corpus must precede190E64 in the future full loader.
    bool loadPositionStatsFont190E64(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadPositionStats190E64(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
    // Explicit whole-world COLD policy only, before any renderer borrows or
    // submissions. Caller supplies the real native session generation. No
    // caller boolean can certify the cold boundary. Result must retire before
    // Application/font-manager destruction. The native ALL8 root calls this.
    bool stageTimeStatsNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,TimeStatsColdResult50LA&,bool deferPopulation=false)const noexcept;
    bool finishTimeStatsNativeCold50LA(TimeStatsColdResult50LA&)const noexcept;
    // Unactivated Speed/Boost stages. Stage references before TimeStats,
    // then finish against the actual final whole-table population/collector.
    bool stageSpeedBoostNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,SpeedBoostColdResult50LA&)const noexcept;
    bool finishSpeedBoostNativeCold50LA(SpeedBoostColdResult50LA&)const noexcept;
    bool stageDamageNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,DamageColdResult50LA&)const noexcept;
    bool finishDamageNativeCold50LA(DamageColdResult50LA&)const noexcept;
    // Present even when Original Main is disabled: direct gameplay shares the
    // same process lifetime. Authentic bytes/templates load only at first use.
    // Frontend teardown, Course handoff and race restart never replace it.
    std::shared_ptr<rr::assets::OriginalUiMaterialApplication50LA> materialApplication=
        std::make_shared<rr::assets::OriginalUiMaterialApplication50LA>();
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> appStart(
        const std::filesystem::path& originalRoot) const {
        require(bool(materialApplication),"application material owner missing");
        if(!materialApplication->prepared()){
            auto archive=readArchive(originalRoot,rr::assets::OriginalUiArchiveId50KG::appStart);
            rr::assets::OriginalUiMaterialDiagnostic50KG diagnostic;
            require(materialApplication->prepareFreshApplication(std::move(archive),diagnostic),
                "application authentic DefaultMT/ScreenMT owners rejected");
        }
        return materialApplication->appStart();
    }
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> profile;
    // One fresh application profile owner; destinations only borrow it.
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> riderProfile;
    std::shared_ptr<rr::platform::OriginalUiOptionsProfile50KG> optionsProfile;
    std::shared_ptr<rr::game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<rr::game::ControlPresets49D> controlsProfile;
    std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> state;
    //140084/147FF8 application lifetime, shared by every frontend incarnation.
    std::shared_ptr<rr::game::OriginalUiLightManagerLifetime50KG> lightManager;
    Application(const Application&)=delete;
    Application& operator=(const Application&)=delete;
    ~Application() {animationClockOwner_->applicationAlive_=false;}
    explicit Application(std::shared_ptr<rr::game::HostMonotonicClock50ID> clock,
        std::shared_ptr<rr::game::ControlPresets49D> controls={})
        :animationClockBackend_(clock),controlsProfile(std::move(controls)) {
        // Retain the one application epoch even on the disabled-Main branch.
        require(bool(animationClockBackend_),"application animation clock owner missing");
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
            require(std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Explicit test profile requires an authenticated Rider pixel cache");
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
        if(const char* cacheRoot=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE")){
            require(*cacheRoot,"RR_ORIGINAL_RIDER_IPUM_CACHE must name a nonempty directory");
            riderCacheRoot=std::filesystem::path(cacheRoot);
            riderProfile=std::make_shared<rr::platform::OriginalUiRiderProfileOwner50KG>();
            rr::platform::OriginalUiRiderProfileDiagnostic50KG riderProfileDiagnostic;
            const auto physics=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::podPhysics);
            require(mawhonicRylothTest?
                riderProfile->initializeMawhonicRylothTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):gasganoTest?
                riderProfile->initializeGasganoTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):
                riderProfile->initializeAtApplicationStartup(profile,physics,riderProfileDiagnostic),
                "Rider startup profile initialization rejected");
            require(riderProfile->gasganoTestProfile()==gasganoTest&&
                riderProfile->mawhonicRylothTestProfile()==mawhonicRylothTest,"Rider test profile provenance differs");
            if(mawhonicRylothTest)std::cerr<<"TEST PROFILE: Mawhonic9 available and initially remembered; original course prefix availability5 for Brightlands, process-local only. "
                "No earned unlock, qualification, career progress or save write is claimed. RA race admission remains subject to genuine runtime readiness.\n";
            if(gasganoTest)std::cerr<<"TEST PROFILE: Gasgano available and initially remembered for this process only; "
                "no earned unlock, qualification, saved-profile import or save write is claimed.\n";
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
        std::cerr<<"ORIGINAL MAIN: "<<(mawhonicRylothTest?"explicit Mawhonic/Ryloth test profile":gasganoTest?"explicit Gasgano test profile":"fresh native profile")
            <<"/application ownership; "
            "one clock-derived native RNG seed="<<state->rawRandom308030
            <<"; Main audio="<<audio<<". Main, Options menu and Game Options routing is wired; "
            "Game Options Accept requires enabled concrete audio and commits process-owned settings. "
            "Prepared default-unlocked Riders 2/4/10/11/12/15/19/20 use their authenticated original resources and image caches; enabled audio uses original Song7. "
            "Mode0 Rider Confirm to Course and Back supports explicit mute or the original Confirm voice/Song11 owner. Course Left/Right selects the four fresh unlocked tracks; "
            "Controls uses original presets and its animated controller: Left/Right chooses, Tab changes Normal/Advanced, R rotates, Enter/Space accepts and Escape cancels. Accept applies only controller0; Cancel preserves the saved preset. "
            "Second-controller Controls remains unavailable until native menu-controller/hotplug and the original missing-controller popup are joined. Gallery, Memory Card and Credits remain unavailable. "
            "Course admits bounded TA human driving with one actual Player and seven original AI, or authenticated TA/SA/MA/GA ALL8 driving, using prepared Riders2/4/10/11/12/15/19/20. Human selections always use the typed whole-world cold-load boundary, including unchanged selections. Other human tracks remain unavailable. No original boot/save parity is claimed.\n";
    }
};
// Owns the actual host/context and its native source-canvas mode together.
// shared_ptr conversions retain this object and use its real destructor.
// RGBA8/D24 private targets are owned by the actual VehicleWindowGl renderer;
// source Z format0x31 is the admitted original24-bit packet representation.
struct Video {
    friend struct Application;
    rr::platform::GameHost50ER host;
    const bool originalMain;
    const std::int32_t width=640,height,colorBits=32,packetZMaximum=0xffffff;
    const std::uint32_t zBufferFormat=0x31;
    ~Video()=default;
private:
    rr::game::OriginalApplicationAnimationClockView50KG applicationAnimation_;
    explicit Video(bool main,rr::game::OriginalApplicationAnimationClockView50KG animation)
        :originalMain(main),height(main?448:480),applicationAnimation_(std::move(animation)){}
    static bool materialConstructionPinLive(const Video& owner) noexcept {
        return owner.originalMain&&owner.width==640&&owner.height==448&&
            owner.colorBits==32&&owner.packetZMaximum==0xffffff&&
            owner.zBufferFormat==0x31&&owner.host.created()&&owner.host.current();
    }
public:
    // Canonical CPU construction capability. The bounded TA source-provider
    // join calls this only after native Video ownership is actually live.
    // The context pins this same actual host owner and the application's
    // authentic APPSTART/templates and distinct texture lifetime; no
    // shared_ptr<void> or numeric video
    // fields supplied by another caller can mint an equivalent capability.
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>
    standardMaterialConstructionContext(const std::shared_ptr<Video>& owner,
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
    rr::platform::OriginalUiMainVideoOwner50KG publication(
        const std::shared_ptr<Video>& owner) const {
        require(owner.get()==this && originalMain && host.created() && host.current(),
            "Main video publication requires its actual current native owner");
        return {owner,width,height,colorBits,packetZMaximum,zBufferFormat};
    }
};
inline std::shared_ptr<Video> Application::videoForSession() const {
    require(raceDisplayOwner50LA_.expired(),"previous race display lifetime still retained");
    if(!videoOwner_)videoOwner_=std::shared_ptr<Video>(new Video(enabled,animationClock()));
    require(videoOwner_->originalMain==enabled,
        "native application cannot replace its original/direct video mode");
    require(!videoOwner_->host.created()&&!videoOwner_->host.ownsResources(),
        "native session requires completed outgoing video/host cleanup");
    return videoOwner_;
}
inline bool Application::stageTimeStatsNativeCold50LA(const std::filesystem::path& originalRoot,
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
inline bool Application::finishTimeStatsNativeCold50LA(TimeStatsColdResult50LA& out)const noexcept {
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
inline bool Application::stageDamageNativeCold50LA(const std::filesystem::path& originalRoot,
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
inline bool Application::finishDamageNativeCold50LA(DamageColdResult50LA& out)const noexcept {
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
inline bool Application::stageSpeedBoostNativeCold50LA(const std::filesystem::path& originalRoot,
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
inline bool Application::finishSpeedBoostNativeCold50LA(SpeedBoostColdResult50LA& out)const noexcept {
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
inline std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path& root,rr::assets::OriginalUiArchiveId50KG id) {
    const auto* identity=rr::assets::originalUiArchiveIdentity50KG(id);
    require(identity,"original UI archive catalog identity missing");
    // The compiled authenticated archive catalog is the only path authority.
    // No private workspace path, recursive search, basename fallback or cache.
    const auto path=root/std::filesystem::path(identity->discPath);
    std::ifstream input(path,std::ios::binary|std::ios::ate);
    if(!input)throw std::runtime_error("cannot open original UI archive: "+path.string());
    const std::streamoff bytes=input.tellg();
    if(bytes<0 || static_cast<std::uint64_t>(bytes)!=identity->bytes)
        throw std::runtime_error("original UI archive size mismatch: "+path.string());
    std::vector<std::uint8_t> raw(identity->bytes);
    input.seekg(0);
    input.read(reinterpret_cast<char*>(raw.data()),static_cast<std::streamsize>(raw.size()));
    if(!input)throw std::runtime_error("cannot read complete original UI archive: "+path.string());
    auto archive=std::make_shared<rr::assets::OriginalUiArchive50KG>();
    rr::assets::OriginalUiArchiveDiagnostic50KG diagnostic;
    if(!archive->load(id,raw,diagnostic))
        throw std::runtime_error("original UI archive authentication/decode rejected: "+path.string()+
            " status="+std::to_string(static_cast<unsigned>(diagnostic.status))+
            " member="+std::to_string(diagnostic.member));
    return archive;
}
struct Frontend : rr::platform::OriginalUiRiderPresentationHost50KG {
    const Application* materialApplicationOwner=nullptr;
    using Status=rr::platform::OriginalUiFrontendStatus50KG;
    using StageClock=std::chrono::steady_clock;
    // Host observation only. These samples never feed a source/accepted clock,
    // cold-time exclusion, frame delta or the unchanged one-second stall gate.
    const bool traceStages=[](){const char* v=std::getenv("RR_TRACE_ORIGINAL_UI_STAGES");
        return v&&v[0]=='1'&&v[1]=='\0';}();
    StageClock::time_point stageStart() const noexcept {
        return traceStages?StageClock::now():StageClock::time_point{};
    }
    void reportStage(const char* stage,StageClock::time_point start,bool okay=true,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer=nullptr) const {
        if(!traceStages)return;
        const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(StageClock::now()-start).count();
        const auto snapshot=coordinator.snapshot();const auto& route=snapshot.route;
        // End the observation before formatting/output; logging overhead still
        // remains real work in the next accepted update interval.
        std::cerr<<"original_ui_stage {\"stage\":\""<<stage<<"\",\"completed_frame\":"<<frameCount
            <<",\"accepted_update_sequence\":"<<snapshot.updates<<",\"screen\":"<<unsigned(route.screen)
            <<",\"page\":"<<unsigned(route.page)<<",\"wall_nanoseconds\":"<<elapsed
            <<",\"okay\":"<<(okay?"true":"false");
        if(renderer)std::cerr<<",\"source_coherent_framebuffer_fetch\":"<<(renderer->originalUiCoherentFramebufferFetch?"true":"false")
            <<",\"draws\":"<<renderer->originalUiDraws<<",\"source_triangles\":"<<renderer->originalUiSourceTriangles
            <<",\"source_texture_barriers\":"<<renderer->originalUiTextureBarriers;
        std::cerr<<"}\n";
    }
    rr::platform::GameHost50ER* presentationHost=nullptr;
    std::uint64_t (*presentationNow)()noexcept=nullptr;
    bool presentationActive=false;
    rr::game::HostMonotonicClock50ID* presentationClock=nullptr;
    std::int32_t savedSwapInterval=0;
    std::uint64_t riderPresentationSwaps=0;
    bool available()const noexcept override {
        if(!presentationHost||!presentationNow||!presentationHost->hasWindow()||!presentationHost->current()||hostPaused())return false;
        const auto& info=presentationHost->info();
        return info.focused&&!info.minimized&&info.drawableWidth>0&&info.drawableHeight>0&&
            (!presentationActive||info.swapInterval==1);
    }
    bool begin()noexcept override {
        if(presentationActive||!available())return false;
        savedSwapInterval=presentationHost->info().swapInterval;
        // Keep ownership even after a failed setter: restoring the old native
        // setting is an explicit cleanup obligation, not an assumed rollback.
        presentationActive=true;rr::platform::GameHostDiagnostic50ER d;
        if(presentationHost->setSwapInterval(1,d))return true;
        if(presentationHost->setSwapInterval(savedSwapInterval,d))presentationActive=false;
        return false;
    }
    bool end()noexcept override {
        if(!presentationActive)return true;
        rr::platform::GameHostDiagnostic50ER d;
        if(!presentationHost||!presentationHost->setSwapInterval(savedSwapInterval,d))return false;
        presentationActive=false;return true;
    }
    bool materialClock(rr::platform::OriginalUiMainFrameClock50KG& out)noexcept override {
        if(!presentationClock)return false;
        try{out=sampleClock(*presentationClock,true);return true;}catch(...){return false;}
    }
    bool checkpointAudio()noexcept override {
        if(!presentationActive||!available())return false;
        const auto now=presentationNow();
        if(now<lastUpdateNanoseconds||now-lastUpdateNanoseconds>1000000000ull)return false;
        rr::platform::AudioOutputDiagnostic50HD d;
        if(!audioOutput.advanceAcceptedNanoseconds(now-lastUpdateNanoseconds,d))return false;
        // These actual steady-clock intervals are mixed exactly once during
        // blocking native presentation. They are excluded from next page dt;
        // material timestamps retain their normal advancing application clock.
        lastUpdateNanoseconds=now;return true;
    }
    bool present(rr::platform::VehicleWindowGl50DS& renderer,
        const rr::platform::OriginalUiPrivatePublication50KP& image,
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept override {
        if(!presentationActive||!available()||!image.completed())return false;
        rr::platform::GameHostInput50ER input;rr::platform::GameHostDiagnostic50ER hostDiagnostic;
        if(!presentationHost->poll(input,hostDiagnostic)||input.keys.quitRequested||!available())return false;
        const auto& info=presentationHost->info();
        if(info.drawableWidth!=image.width()||info.drawableHeight!=image.height())return false;
        rr::platform::OriginalUiWindowBackBufferCopy copy;
        if(!renderer.copyOriginalUiPrivatePublicationToWindow(image,copy,d)||!copy.commandsAccepted()||
           !presentationHost->swapWindow(hostDiagnostic))return false;
        ++riderPresentationSwaps;
        return checkpointAudio();
    }
    AudioOutput audioOutput;
    rr::platform::OriginalUiFrontend50KG coordinator;
    rr::platform::OriginalUiFrontendDiagnostic50KG diagnostic{};
    std::uint64_t lastUpdateNanoseconds=0,inputCount=0,unavailableCount=0,frameCount=0;
    std::uint64_t captureSequence=0,lastAudioSequence=0;
    std::string sessionWavPath;
    // Actual accepted renderer receipt; diagnostic is reused by later calls.
    std::uint8_t submittedScreen=0;
    std::uint64_t submittedPublication=0,sourceFrameGeneration=0,sourceTargetGeneration=0;
    std::uint32_t submittedDraws=0,submittedQuads=0,submittedTriangles=0;
    std::uint32_t submittedTextureBarriers=0;
    bool submittedCoherentFramebufferFetch=false;
    std::uint64_t lastSourceNanoseconds=0,activeSourceNanoseconds=0;
    std::uint64_t pendingUpdateColdNanoseconds=0,pendingMaterialColdNanoseconds=0;
    std::uint32_t sourceMilliseconds=0;
    bool sourceSampleKnown=false,previousSourceActive=false;
    bool imagePublished=false;
    bool hostPaused()const noexcept {
        return materialApplicationOwner&&materialApplicationOwner->animationPaused();
    }
    void setHostPaused(bool paused) {
        require(materialApplicationOwner,"explicit UI pause requires application owner");
        materialApplicationOwner->setExplicitAnimationPause(paused);
    }
    rr::platform::OriginalUiMainFrameClock50KG sampleClock(
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
    void prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest,
        std::shared_ptr<const rr::platform::OriginalUiCourseRaceDestination50KG> raceDestination={},
        std::uint64_t (*nativeNow)()noexcept=nullptr,std::uint64_t nativeSessionIndex=0) {
        require(!materialApplicationOwner,"frontend material directory already entered");
        materialApplicationOwner=&application;
        application.enterUiMaterialDirectory17DB80();
        using Id=rr::assets::OriginalUiArchiveId50KG;
        rr::platform::OriginalUiFrontendCold50KG cold;
        cold.appStart=application.appStart(application.assetRoot);
        cold.mainMenu=readArchive(application.assetRoot,Id::mainMenu);
        cold.options=readArchive(application.assetRoot,Id::options);
        cold.fonts=readArchive(application.assetRoot,Id::fonts);
        cold.controllerPopup=readArchive(application.assetRoot,Id::controllerPopup);
        cold.overlays=readArchive(application.assetRoot,Id::overlays);
        cold.localization=readArchive(application.assetRoot,Id::localization);
        cold.runtime=application.materialRuntime;cold.video=video->publication(video);
        cold.profile=application.profile;cold.application=application.state;cold.courseRaceDestination=std::move(raceDestination);
        presentationHost=&video->host;presentationNow=nativeNow;
        presentationClock=application.state->clock101A50.get();cold.riderPresentation=this;
        cold.lightManager=application.lightManager;
        cold.riderProfile=application.riderProfile;
        // Explicit, optional native codec-cache input. This is not a readiness
        // switch: the compiled output catalog, each retained mip, every source
        // archive and the complete destination must still authenticate.
        if(!application.riderCacheRoot.empty()){
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
            cold.backgrounds=readArchive(application.assetRoot,Id::backgrounds);
            cold.podTextures=readArchive(application.assetRoot,Id::podTextures);
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
            auto cache=std::make_shared<Cache>();
            rr::assets::OriginalUiIpumCacheDiagnostic50KG cacheDiagnostic;
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
            cold.riderIpumCache=std::move(cache);
        }
        // Reconstructed on every frontend lifetime from the SAME Application.
        // coordinator.prepare requires genuine inputs; Rider bind prepares every requested pair.
        cold.prepareCleggLockedPreview=application.prepareCleggLockedPreview;
        cold.requestedPreviewRiders=application.requestedPreviewRiders;
        cold.optionsProfile=application.optionsProfile;cold.gameSettingsProfile=application.gameSettingsProfile;
        cold.soundMode=application.soundMode;cold.controlsProfile=application.controlsProfile;
        cold.audioMode=application.audioMode;
        cold.textureRequest=textureRequest;
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
            if(!audioOutput.prepare(application.audioRoot,outputRequest,error,outputDiagnostic,application.soundMode))throw std::runtime_error(error);
            require(audioOutput.soundModeOwner50KG()==application.soundMode.get(),
                "original UI output did not retain the application sound mode owner");
            cold.audio=audioOutput.transport();
            if(!application.riderCacheRoot.empty()&&!cold.audio->prepareRiderMenu(application.audioRoot,error))
                throw std::runtime_error(error);
            if(cold.course&&!cold.audio->prepareCourseMenu(application.audioRoot,error))
                throw std::runtime_error(error);
            const auto info=audioOutput.info();
            std::cerr<<"Original UI audio device="<<rr::platform::audioDeviceStatusName50HD(info.deviceStatus)
                <<" capture="<<info.captureActive<<" rate="<<info.sampleRate<<" detail="<<info.deviceDetail.data()<<'\n';
        }
        if(coordinator.prepare(cold,diagnostic)!=Status::ok)fail("preparation");
        if(coordinator.configureRendererRequest(rendererRequest,diagnostic)!=Status::ok)fail("renderer configuration");
    }
    void activate(rr::platform::VehicleWindowGl50DS& renderer) {
        if(coordinator.activate(renderer,diagnostic)!=Status::ok)fail("activation");
    }
    [[noreturn]] void fail(const char* operation) const {
        throw std::runtime_error(std::string("original UI ")+operation+" rejected: status="+
            std::to_string(static_cast<unsigned>(diagnostic.status))+" source="+std::to_string(diagnostic.sourcePC)+
            " main="+std::to_string(static_cast<unsigned>(diagnostic.main.status))+
            " options="+std::to_string(static_cast<unsigned>(diagnostic.options.status))+
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
            " course="+std::to_string(static_cast<unsigned>(diagnostic.course.status))+
            " course_pc="+std::to_string(diagnostic.course.sourcePC)+
            " renderer="+std::to_string(static_cast<unsigned>(diagnostic.renderer.error)));
    }
    void input(std::uint8_t sourceInput) {
        rr::platform::OriginalUiMainSessionInput50KG in;in.controller=0;in.originalInput=sourceInput;
        rr::platform::OriginalUiFrontendInputReceipt50KG receipt;
        const auto status=coordinator.input(in,receipt,diagnostic);++inputCount;
        if(receipt.coldInterval.measured){
            const auto elapsed=receipt.coldInterval.nanoseconds;
            require(elapsed<=std::numeric_limits<std::uint64_t>::max()-pendingUpdateColdNanoseconds&&
                elapsed<=std::numeric_limits<std::uint64_t>::max()-pendingMaterialColdNanoseconds,"UI cold interval overflow");
            pendingUpdateColdNanoseconds+=elapsed;pendingMaterialColdNanoseconds+=elapsed;
        }
        if(status==Status::unavailableDestination||status==Status::secondControllerRequired){
            ++unavailableCount;std::cerr<<"original_ui_destination unavailable screen="<<unsigned(receipt.source.screen)
                <<" page="<<unsigned(receipt.source.page)
                <<" status="<<unsigned(status)<<" source="<<diagnostic.sourcePC
                <<" main="<<unsigned(diagnostic.main.status)<<" options="<<unsigned(diagnostic.options.status)
                <<" options_resource="<<unsigned(diagnostic.optionsResources.error)
                <<" options_resource_widget="<<diagnostic.optionsResources.widgetId
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
                <<" gpu="<<unsigned(diagnostic.gpu.error)<<" renderer="<<unsigned(diagnostic.renderer.error)
                <<"; no destination or gameplay entered.\n";
        }else if(status!=Status::ok&&status!=Status::noChange&&status!=Status::routeAccepted)fail("input");
    }
    std::uint64_t acceptedElapsed(std::uint64_t now) {
        require(!lastUpdateNanoseconds||now>=lastUpdateNanoseconds,"original UI update clock moved backwards");
        std::uint64_t elapsed=lastUpdateNanoseconds?now-lastUpdateNanoseconds:0;
        if(lastUpdateNanoseconds){
            require(pendingUpdateColdNanoseconds<=elapsed,"UI cold interval exceeds update interval");
            elapsed-=pendingUpdateColdNanoseconds;
        }
        pendingUpdateColdNanoseconds=0;lastUpdateNanoseconds=now;
        require(elapsed<=1000000000ull,"original UI active update stall exceeds one second");
        return elapsed;
    }
    void update(const rr::platform::OriginalUiMainSessionUpdate50KG& in,std::uint64_t elapsedNanoseconds) {
        rr::platform::OriginalUiFrontendUpdateReceipt50KG receipt;
        const auto updateStart=stageStart();
        const bool updated=coordinator.update(in,receipt,diagnostic)==Status::ok&&receipt.accepted&&receipt.sequence==lastAudioSequence+1;
        reportStage("source_update",updateStart,updated);
        if(!updated)
            fail("update");
        // One PCM advance for this exact accepted source update; never for
        // loading, rejected updates, or a second screen's copied clock.
        rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
        const auto audioStart=stageStart();
        const bool advanced=audioOutput.advanceAcceptedNanoseconds(elapsedNanoseconds,outputDiagnostic);
        reportStage("pcm_advance",audioStart,advanced);
        if(!advanced)
            throw std::runtime_error(std::string("original UI PCM advance failed: ")+outputDiagnostic.detail.data());
        lastAudioSequence=receipt.sequence;
    }
    void render(const rr::platform::OriginalUiMainFrameClock50KG& clock,
        rr::platform::VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height) {
        const auto buildStart=stageStart();
        const bool rendered=coordinator.render(clock,diagnostic)==Status::ok;
        reportStage("source_build",buildStart,rendered);
        if(!rendered)fail("render");
        const auto submitStart=stageStart();
        const bool submitted=coordinator.submit(renderer,width,height,diagnostic)==Status::ok;
        reportStage("native_submit",submitStart,submitted,&diagnostic.renderer);
        if(!submitted)fail("submission");
        submittedScreen=coordinator.snapshot().route.screen;
        submittedPublication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
        submittedDraws=diagnostic.renderer.originalUiDraws;submittedQuads=diagnostic.renderer.originalUiQuads;
        submittedTriangles=diagnostic.renderer.originalUiSourceTriangles;
        submittedTextureBarriers=diagnostic.renderer.originalUiTextureBarriers;
        submittedCoherentFramebufferFetch=diagnostic.renderer.originalUiCoherentFramebufferFetch;
        sourceFrameGeneration=diagnostic.renderer.originalUiSourceFrameGeneration;
        sourceTargetGeneration=diagnostic.renderer.originalUiSourceTargetGeneration;
    }
    void commitDeferred(rr::platform::VehicleWindowGl50DS& renderer) {
        const auto deferredStart=stageStart();
        const auto status=coordinator.commitDeferred(renderer,diagnostic);
        reportStage("deferred_commit",deferredStart,status==Status::ok||status==Status::noChange||status==Status::courseLaunchReady);
        if(status==Status::courseLaunchReady){
            require(materialApplicationOwner,"Course material directory owner missing");
            materialApplicationOwner->enterGameplayMaterialDirectory17D7A0();
            imagePublished=false;return;
        }
        if(status==Status::ok){imagePublished=false;std::cerr<<"original_ui_screen_activated "<<unsigned(coordinator.snapshot().route.screen)<<'\n';}
        else if(status!=Status::noChange)fail("deferred activation");
    }
    void requestQuit() noexcept {coordinator.requestQuit();}
    bool closeUi() noexcept {
        const auto status=coordinator.close(diagnostic);
        return status==Status::ok||status==Status::noChange;
    }
    bool closeAudio() noexcept {
        rr::platform::AudioOutputDiagnostic50HD d;const bool okay=audioOutput.close(d);
        if(!okay)std::cerr<<"original UI audio close error="<<unsigned(d.error)<<'\n';return okay;
    }
    const rr::game::OriginalUiNavigation50KG* navigation() const noexcept {
        const auto route=coordinator.snapshot().route;
        if(route.screen==4&&route.page==1){
            const auto* resources=coordinator.mainResources();
            return resources&&resources->manager()?resources->manager()->navigation(1):nullptr;
        }
        if(route.screen==6&&route.page==1){
            const auto* resources=coordinator.courseResources();
            return resources&&resources->widgets()?resources->widgets()->navigation():nullptr;
        }
        if(route.screen==12){
            const auto* resources=coordinator.optionsResources();
            const auto* widgets=resources?resources->widgets():nullptr;
            if(!widgets)return nullptr;
            if(route.page==1)return widgets->navigation();
            if(route.page==4){const auto* state=widgets->gameOptionsState();return state?&state->navigation:nullptr;}
            if(route.page==2||route.page==3){const auto* session=coordinator.optionsSession();
                const auto* state=session?session->controlsState(route.page-2):nullptr;return state?&state->navigation:nullptr;}
        }
        return nullptr;
    }
    void report(const char* reason,const Application& application,std::uint64_t worldTicks,std::uint64_t mixedFrames) const {
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
        if(snapshot.route.screen==6){
            require(snapshot.route.page==1&&coordinator.courseSession(),"original Course session report unavailable");
            const auto& course=snapshot.course;const auto& aperture=course.aperture;
            const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=submittedScreen==6&&submittedPublication==publication&&snapshot.frameSubmitted;
            std::cerr<<"original_course_state {\"reason\":\""<<reason<<"\",\"screen\":6,\"page\":1"
                <<",\"presentation\":\"native_host_rgb\",\"switches\":"<<snapshot.switches
                <<",\"gpu_publication\":"<<publication<<",\"updates\":"<<snapshot.updates
                <<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
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
            <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()<<",\"audio_mixed_frames\":"<<audioInfo.mixedFrames
            <<",\"audio_captured_frames\":"<<audioInfo.capturedFrames<<",\"audio_rate\":"<<audioInfo.sampleRate
            <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()<<",\"audio_clock_microseconds\":"<<audioInfo.simulationMicroseconds
            <<",\"audio_remainder_nanoseconds\":"<<audioOutput.remainderNanoseconds()
            <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioInfo.deviceStatus)<<"\""
            <<",\"audio_device_consumed_frames\":"<<audioInfo.deviceConsumedFrames<<",\"audio_device_queued_frames\":"<<audioInfo.deviceQueuedFrames
            <<",\"audio_device_underrun_frames\":"<<audioInfo.deviceUnderrunFrames<<",\"audio_device_overflow_frames\":"<<audioInfo.deviceOverflowFrames
            <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
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
};
}
