#pragma once
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include "original_ui_course_page_payload50kg.h"
#include "original_ui_course_popup_payload50kg.h"
#include "original_ui_course_manager_config50kg.h"
#include "original_ui_overlay_text_storage50kg.h"
#include "../../../src/game/original_ui_text_position50kg.h"
#include "../../../src/game/original_racer_animation_cold50la.h"
#include "../../../src/game/original_racer_model_scene50la.h"
#include "../../../src/game/original_racer_model_packets50la.h"
#include "../../../src/game/original_motion_library50la.h"
#include "../../../src/game/collection_bounds48s.h"
#include "../../../src/assets/model_bindings49o.h"
#include <algorithm>
#include <limits>
#include <optional>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <thread>
#include <cfenv>
#include <cmath>
#include <bit>
#include "../../../src/assets/original_course_screen_fallback50kg.h"
#include "../../../src/assets/original_course_screen_authored50kg.h"
#include <cstring>
#include <string_view>
#include "../../../src/game/source_acos49t.h"
#include "../../../src/game/original_camera_scene_tick.h"
#include "../../../src/game/original_camera_start_effects.h"
#include "../../../src/game/original_options_mutation_interlock50kg.h"
#include "../../../src/game/original_camera_observer_assertions50kg.h"

namespace rr::platform {
// Separate actual counted Course construction custody. The legacy CPU Course
// projections do NOT establish this history. No current entry calls this owner;
// the real1D0140 loader/page graph must precede eventual dispatcher publication.
class OriginalUiCourseOwnedLifecycle50KG final {
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiLightManagerOwner50KG;
    friend class OriginalUiGarageScreenText50KG;
    friend class OriginalUiCoursePopupPayload50KG;
    using Root=::original_main_entry50KG::Application;
    using Video=::original_main_entry50KG::Video;
    using Scope=game::OriginalUiEntityScope50KG;
    using Construction=Scope::CourseConstruction;
    const Root* issuer_=nullptr;
    const OriginalUiFrontend50KG* frontend_=nullptr;
    const void* managerIdentity_=nullptr;
    std::shared_ptr<Scope> scope_;
    std::shared_ptr<Video> video_;
    // Reverse destruction must withdraw the exclusive Video write BEFORE
    // retiring its counted manager. No retail destructor is implied by abort.
    std::unique_ptr<Construction> construction_;
    std::unique_ptr<Video::CourseModeEffect> modeEffect_;
    OriginalUiCoursePagePayload50KG pagePayload_;
    std::shared_ptr<OriginalUiLightManagerOwner50KG> courseLight_;
    const std::thread::id thread_=std::this_thread::get_id();
    bool terminal_=false,directoriesSelected_=false,loaderAttempted_=false;
    std::uint32_t loaderStopPC_=0x1D0140;
    std::array<char,9> managerName194_{}; // Exact "UICourse" plus terminator.
    bool managerNameKnown_=false;
    const game::OriginalWreckComponent50LA* managerDispatcher2AC_=nullptr;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> loaderDisplay_;
    bool rendererCallsReturned1CCD14_=false;
    std::unique_ptr<OriginalUiCourseManagerConfig50KG> loaderConfig_;
    const OriginalUiCourseManagerConfig50KG::Config157320* sourceConfig298_=nullptr;
    bool sourceConfig298Known_=false;
    std::string formattedConfigPathStack70_;
    bool formattedConfigPathKnown_=false;

    OriginalUiCourseOwnedLifecycle50KG()noexcept=default;
    struct Diagnostic {Scope::Result entity{};OriginalUiCoursePagePayload50KG::Diagnostic page{};
        OriginalUiCoursePopupPayload50KG::Diagnostic popup{};
        assets::OriginalStandardArchiveProvider50LA::Diagnostic archive{};
        assets::OriginalStandardTextureDiagnostic50LA file{};
        assets::OriginalUiPageDiagnostic50KG config{};OriginalUiLightManagerOwner50KG::Diagnostic light{};
        std::uint32_t sourcePC=0;std::size_t lightOrdinal=0;};
#include "original_ui_course_lights50kg.inl"
#include "original_ui_course_fog50kg.inl"
#include "original_ui_course_overlay_text50kg.inl"
#include "original_ui_course_movement50kg.inl"
#include "original_ui_course_quad_resources50kg.inl"
#include "original_ui_course_authored_resources50kg.inl"
#include "original_ui_course_icon50kg.inl"
#include "original_ui_course_selectable50kg.inl"
#include "original_ui_course_soultree_model50kg.inl"
#include "original_ui_course_soultree50kg.inl"
#include "original_ui_course_model_observer50kg.inl"
#include "original_ui_course_widgets50kg.inl"
#include "original_ui_course_popup_owner50kg.inl"
#include "original_ui_course_pages50kg.inl"
#include "original_ui_course_camera50kg.inl"
#include "original_ui_course_camera_observer50kg.inl"
#include "original_ui_course_page_prompts50kg.inl"
#include "original_ui_course_page_background50kg.inl"
#include "original_ui_course_page_collections50kg.inl"
    bool selectCourseDirectories1B8330(Diagnostic& d)noexcept {
        if(directoriesSelected_||!issuer_||thread_!=std::this_thread::get_id())return false;
        try {
            // Allocate native strings before the source write interval. These
            // exact literals come from3AC7D0/E0/F0,3AC800 and screen6/3AC820.
            std::string textures="UI\\Textures\\",geometry="UI\\Geometry\\";
            std::optional<std::string> animation("UI\\Anim\\"),ui("UI\\UICourse\\");
            std::string directory="UI\\UICourse\\";
            issuer_->materialDirectory36B970_.swap(textures); //1B835C..68.
            issuer_->modelDirectory36B860_.swap(geometry); //1B8384..90.
            issuer_->animationDirectory36AED0_.swap(animation); //1B83AC/B0.
            issuer_->trackDirectory36B640_.swap(directory); //1B83C8 then1B8468 append.
            issuer_->uiResourceDirectory36BB90_.swap(ui); //1B83DC then1B847C append.
            directoriesSelected_=true;d.sourcePC=0x1B7ED4;return true;
        }catch(...){d.sourcePC=0x1B8330;return false;}
    }
    // Private sequential loader component. No current Frontend path invokes
    // it; ordinary CPU Course preparation is not a substitute for this call.
    bool loadManagerConfigPrefix1D0140(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
        Diagnostic& d)noexcept {
        d={};d.sourcePC=loaderStopPC_;
        if(thread_!=std::this_thread::get_id()||loaderAttempted_||terminal_||!directoriesSelected_||
           !issuer_||!construction_||construction_->pageAcquired_||!modeEffect_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_)||
           !issuer_->uiResourceDirectory36BB90_||*issuer_->uiResourceDirectory36BB90_!="UI\\UICourse\\"||
           !issuer_->standardMaterialDomainOwner_||!issuer_->standardMaterialDomainOwner_->prepared()||
           !archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return false;
        // Native custody admission is not retail2E8D90. Failure here occurs
        // before any1CCBA0 source writes and does not consume a source attempt.
        try {loaderConfig_=std::unique_ptr<OriginalUiCourseManagerConfig50KG>(new OriginalUiCourseManagerConfig50KG);}
        catch(...){return false;}
        loaderAttempted_=true;
        const auto stop=[&](std::uint32_t pc)noexcept {terminal_=true;loaderStopPC_=d.sourcePC=pc;return false;};
        issuer_->sourceApplicationMode22C_=1; //1CCBE0 precedes name copy309230.
        constexpr char name[]="UICourse";
        std::copy(std::begin(name),std::end(name),managerName194_.begin());managerNameKnown_=true;
        // Actual vA8=1CEF50. Only the genuine represented startup344=0
        // branch is joined; resource-build mode must not be guessed false.
        if(issuer_->sourceBuildUiResFiles344_!=0)return stop(0x1CEF6C);
        const auto provider=issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70();
        const auto resolver=issuer_->standardMaterialDomainOwner_->resolver3E1A70();
        if(!loaderConfig_->mountCourse12CEA0(provider,resolver,std::move(archive),d.archive,d.file))
            return stop(d.file.sourcePC?d.file.sourcePC:0x1CCC44);
        construction_->bank2A8_=1; //1CCC58, only after the actual mount operation.
        if(!loaderConfig_->enterArchiveOnly132A20())return stop(0x1CCC64);
        //1CCC70 allocates actual Config storage; NULL skips the constructor
        // but does NOT skip298 publication or308C90 path formatting.
        loaderConfig_->allocateAndConstruct157320();
        sourceConfig298_=loaderConfig_->config_.get();sourceConfig298Known_=true; //1CCC88.
        try {
            formattedConfigPathStack70_=*issuer_->uiResourceDirectory36BB90_+"ui.dat";
            formattedConfigPathKnown_=true; //308C90 returned at1CCCA4.
            if(!sourceConfig298_)return stop(0x1CCCAC); // Do not invoke1577D0 on NULL.
            if(loaderConfig_->open1577D0(formattedConfigPathStack70_,d.config)!=OriginalUiCourseManagerConfig50KG::OpenResult::opened)
                return stop(0x1CCCAC);
        }catch(...){return stop(0x1CCC9C);}
        if(!loaderConfig_->parsedCurrent())return stop(0x1CCCB4);
        managerDispatcher2AC_=&scope_->dispatcher_; //1CCCD0 actual source a3 from1B7F04.
        const auto display=issuer_->raceDisplayOwner50LA_.lock();
        if(!display||display->application_!=issuer_||display->video_!=video_||
           !display->frontendOwnerCurrent50KG()||!display->created_||!display->frontendCreated_||
           display->retiring_||!display->renderer_.initialized()||!video_->host.created()||!video_->host.current())
            return stop(0x1CCCEC);
        loaderDisplay_=display;
        // Actual243A80 vtable3D2050 slot2C is245130 (jr ra/nop). Both
        //1CCCEC(0,13,0) and1CCD0C(0,14,0) return without an observed result.
        // Real same-Display identity above is required; no GL state is made up.
        rendererCallsReturned1CCD14_=true;
        // Only this component returned. No List/Node, Light, page factory or
        // containing1D0140 completion. The actual next source boundary remains.
        loaderStopPC_=d.sourcePC=0x1CCD14;return true;
    }
    bool loadManagerSearchPrefix1CCD14(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCD14;
        if(thread_!=std::this_thread::get_id()||terminal_||!rendererCallsReturned1CCD14_||
           loaderStopPC_!=0x1CCD14||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !construction_||!modeEffect_||managerDispatcher2AC_!=&scope_->dispatcher_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        d.entity=construction_->constructSearch1CCD14(this);
        loaderStopPC_=d.sourcePC=d.entity.sourcePC;
        if(!d.entity.okay()){terminal_=true;return false;}
        return true; // Only List/Node prefix; Light148B60 remains unreached.
    }
    bool lightBoundaryCurrent148B60()const noexcept {
        if(thread_!=std::this_thread::get_id()||terminal_||loaderStopPC_!=0x1CCD80||
           !issuer_||!rendererCallsReturned1CCD14_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !construction_||!modeEffect_||managerDispatcher2AC_!=&scope_->dispatcher_||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto& c=*construction_;
        return c.searchAttempted_&&c.searchNodeAcquired_&&c.searchNodeAttached_&&c.searchAliases_==2&&
            c.searchList_&&c.sourceList2A0_==c.searchList_.get()&&c.sourceNode2A4_==&c.searchNode_&&
            c.searchOwner_==this&&c.searchCustodyCurrent();
    }
    bool loadManagerLightZone1CCD88(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1CCD80;
        if(courseLight_||!lightBoundaryCurrent148B60()||!issuer_->sourceLightManager426F60Owner_)return false;
        // Hold actual same-Application singleton before source148B60 mutates
        // any zones. A failure retains this owner for exact native abort.
        courseLight_=issuer_->sourceLightManager426F60Owner_;
        if(!courseLight_->beginCourseZone148B60(*issuer_,*this,d.light)){
            terminal_=true;loaderStopPC_=d.sourcePC=d.light.sourcePC;return false;
        }
        loaderStopPC_=d.sourcePC=0x1CCD90;return true; // BEFORE actual1CDB10 UI-light loader.
    }
    static bool constructManager1D0070(const Root& root,const OriginalUiFrontend50KG& frontend,
        const void* actualManager,std::uint64_t generation,
        std::unique_ptr<OriginalUiCourseOwnedLifecycle50KG>& output,Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1b7ed4;
        if(output||!actualManager||!generation||root.garageAudioOwnerThread_!=std::this_thread::get_id()||
           !root.sharedUiScopeOwner_||!root.videoOwner_)return false;
        std::unique_ptr<OriginalUiCourseOwnedLifecycle50KG> next;
        try {next.reset(new OriginalUiCourseOwnedLifecycle50KG);}
        catch(...){return false;}
        next->issuer_=&root;next->frontend_=&frontend;next->managerIdentity_=actualManager;
        next->scope_=root.sharedUiScopeOwner_;next->video_=root.videoOwner_;
        // Retain the partial owner even when a later resource acquisition
        // fails. A caller cannot retry source writes by discarding a receipt.
        output=std::move(next);auto& owner=*output;
        //1B7DF0 selects source screen6 directories before its1D0070 call.
        if(!owner.selectCourseDirectories1B8330(d)){owner.terminal_=true;return false;}
        d.entity=owner.scope_->beginCourseConstructorPrefix(&root,owner.scope_,&frontend,
            actualManager,generation,owner.construction_);
        if(!d.entity.okay()){owner.terminal_=true;d.sourcePC=d.entity.sourcePC;return false;}
        d.sourcePC=0x2a3a90;
        if(!owner.video_->acquireCourseMode2A3A90(root,owner.video_,*owner.construction_,
            &frontend,actualManager,owner.modeEffect_)){owner.terminal_=true;return false;}
        d.sourcePC=0x1d0084;
        if(!owner.construction_->completeDerived1D0070()){owner.terminal_=true;return false;}
        // Complete only the genuine detached1D0070 constructor. No source
        // name194,2A0 search owner,190 fog,29C widget table, page factory,
        // manager attachment or App2D8 destruction is invented here.
        d.sourcePC=0x1b7f00;return true;
    }
    // Counted Page constructor issued only by the genuine bounded Page1
    // virtual90 prefix. It does not certify the loader/factory return.
    bool constructPageComponent1D01B4(Diagnostic& d)noexcept {
        d={};d.sourcePC=0x1D01B4;
        if(thread_!=std::this_thread::get_id()||terminal_||!issuer_||!construction_||
           !modeEffect_||construction_->pageAcquired_||!coursePageBaseAttempted_||
           loaderStopPC_!=0x1CECD0||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        if(!pagePayload_.bindNativeDisplay(*issuer_,issuer_->raceDisplayOwner50LA_.lock(),d.page)){
            d.sourcePC=d.page.sourcePC;terminal_=true;return false;
        }
        d.entity=construction_->allocatePage1D01A0();
        if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;terminal_=true;return false;}
        d.entity=construction_->constructPageBase2A5C70();
        if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;terminal_=true;return false;}
        if(!pagePayload_.constructShared1D5EA0(d.page)||!pagePayload_.constructDerived1D8D70(d.page)){
            d.sourcePC=d.page.sourcePC;terminal_=true;return false;
        }
        // Actual derived subobjects exist, but page loader/materials/camera,
        // table population, attachment and dispatcher publication do not.
        d.sourcePC=0x1D01BC;return true;
    }
    bool abortNative()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        terminal_=true;
        // All collection references and prompt resource releases are admitted
        // before the first new destructive native step. No original Page return.
        if(!preflightNativeCoursePageCollections())return false;
        if(!prepareNativeCoursePagePromptsAbort())return false;
        if(!abortNativeCoursePageCollections())return false;
        CourseCameraDiagnostic camera;
        if(!abortNativeCoursePageCamera(camera))return false;
        if(!abortNativeCoursePopupOwner())return false;
        if(!abortNativeCoursePagesConfigAndTable())return false;
        OriginalUiCoursePagePayload50KG::Diagnostic page;
        if(!pagePayload_.nativeAbortCold(page))return false;
        if(construction_&&!construction_->abortNativePage().okay())return false;
        if(!preflightNativeCourseIcons())return false;
        if(!preflightNativeCourseSelectableIcons())return false;
        if(!preflightNativeCourseOverlayTexts())return false;
        if(!detachNativeCourseFogBeforeWidgets())return false;
        if(!abortNativeCourseFinalIcons())return false;
        if(!abortNativeCourseOverlayTextRange(67,84))return false;
        if(!abortNativeCourseLateIcons())return false;
        if(!abortNativeCourseOverlayTextRange(59,67))return false;
        if(!abortNativeCourseLateSelectableIcons())return false;
        if(!abortNativeCourseOverlayTextRange(26,59))return false;
        if(!abortNativeCourseSoultree())return false;
        if(!abortNativeCourseSelectableIcons())return false;
        if(!abortNativeCourseIcon())return false;
        if(!abortNativeCourseMovement())return false;
        if(!abortNativeCourseOverlayTexts())return false;
        if(!abortNativeCourseWidgetsConfig())return false;
        if(!abortNativeCourseFog())return false;
        if(!abortNativeCourseUiLights())return false;
        if(courseLight_){OriginalUiLightManagerOwner50KG::Diagnostic light;
            if(!courseLight_->abortCourseZoneNative(*this,light))return false;
            courseLight_.reset();
        }
        if(construction_&&!construction_->abortNativeSearch().okay())return false;
        if(loaderConfig_){if(!loaderConfig_->closeNative())return false;loaderConfig_.reset();}
        loaderDisplay_.reset();
        //sourceConfig298_/name publication is not overwritten by native abort.
        //construction_->bank2A8_ remains authoritative until manager custody retires.
        if(modeEffect_){if(!modeEffect_->abortNative())return false;modeEffect_.reset();}
        if(construction_){if(!construction_->abortNative().okay())return false;construction_.reset();}
        return true;
    }
public:
    OriginalUiCourseOwnedLifecycle50KG(const OriginalUiCourseOwnedLifecycle50KG&)=delete;
    OriginalUiCourseOwnedLifecycle50KG& operator=(const OriginalUiCourseOwnedLifecycle50KG&)=delete;
    ~OriginalUiCourseOwnedLifecycle50KG()noexcept {if(!abortNative())std::terminate();}
};
}
