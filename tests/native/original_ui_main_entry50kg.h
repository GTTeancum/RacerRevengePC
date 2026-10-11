#pragma once
#include "original_ui_main_output50kg.h"
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include "../../src/game/lap_host_clock50id.h"
#include "../../src/game/original_ui_light_manager_lifetime50kg.h"
#include <filesystem>
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
inline void require(bool accepted,const char* message) {
    if(!accepted)throw std::runtime_error(message);
}
inline std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path&,rr::assets::OriginalUiArchiveId50KG);
// NEW native application startup for the original-UI opt-in. This is a real
// process owner, not an imported save or a reconstruction of retail boot.
// The otherwise loaded gameplay world is never advanced in this mode. A
// future gameplay destination MUST join this same application RNG owner before
// it is admitted; copying these fields into an independently running RNG is
// not supported. The frontend cannot restart into the host driving path.
struct Application {
    bool enabled=false;
    std::filesystem::path assetRoot,audioRoot,riderCacheRoot;
    rr::platform::OriginalUiMainAudioMode50KG audioMode=
        rr::platform::OriginalUiMainAudioMode50KG::muted;
    rr::assets::OriginalUiMaterialRuntime50KG materialRuntime{};
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> profile;
    // One fresh application profile owner; destinations only borrow it.
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> riderProfile;
    std::shared_ptr<rr::platform::OriginalUiOptionsProfile50KG> optionsProfile;
    std::shared_ptr<rr::game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> state;
    //140084/147FF8 application lifetime, shared by every frontend incarnation.
    std::shared_ptr<rr::game::OriginalUiLightManagerLifetime50KG> lightManager;
    explicit Application(rr::game::HostMonotonicClock50ID& clock) {
        const char* enabledSetting=std::getenv("RR_ORIGINAL_MAIN_MENU");
        require(!enabledSetting || std::string(enabledSetting)=="0" || std::string(enabledSetting)=="1",
            "RR_ORIGINAL_MAIN_MENU must be 0 or 1");
        enabled=enabledSetting && std::string(enabledSetting)=="1";
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
            require(riderProfile->initializeAtApplicationStartup(profile,
                readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::podPhysics),riderProfileDiagnostic),
                "fresh original Rider profile initialization rejected");
        }
        require(rr::game::validOriginalUiGameSettingsProfile50KG(*gameSettingsProfile),
            "fresh original Game Options profile rejected");
        require(soundMode->initializeFromProfile1101C(gameSettingsProfile->soundMode1101C),
            "fresh original sound mode initialization rejected");
        // One native startup seed from the existing process clock, never per
        // frame, capture or focus change. Exact original boot seeding unclaimed.
        require(rr::game::sampleHostTimestamp50ID(clock,state->rawRandom308030),
            "Main startup shared application timestamp unavailable");
        state->known=true;
        materialRuntime={true,false}; // Process-owned fresh native normal-material mode.
        std::cerr<<"ORIGINAL MAIN ONLY: fresh native profile/application ownership; "
            "one clock-derived native RNG seed="<<state->rawRandom308030
            <<"; Main audio="<<audio<<". Main, Options menu and Game Options routing is wired; "
            "Accept requires enabled concrete audio and commits process-owned settings. "
            "Rider19 entry additionally requires the authenticated compiled IPUM cache; enabled audio uses original Song7. "
            "other riders, Course/Gallery and other Options subpages remain unavailable. "
            "No gameplay launch or original boot/save parity is claimed.\n";
    }
};
// Owns the actual host/context and its native source-canvas mode together.
// shared_ptr conversions retain this object and use its real destructor.
// RGBA8/D24 private targets are owned by the actual VehicleWindowGl renderer;
// source Z format0x31 is the admitted original24-bit packet representation.
struct Video {
    rr::platform::GameHost50ER host;
    const bool originalMain;
    const std::int32_t width=640,height,colorBits=32,packetZMaximum=0xffffff;
    const std::uint32_t zBufferFormat=0x31;
    explicit Video(bool main):originalMain(main),height(main?448:480){}
    rr::platform::OriginalUiMainVideoOwner50KG publication(
        const std::shared_ptr<Video>& owner) const {
        require(owner.get()==this && originalMain && host.created() && host.current(),
            "Main video publication requires its actual current native owner");
        return {owner,width,height,colorBits,packetZMaximum,zBufferFormat};
    }
};
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
struct Frontend {
    using Status=rr::platform::OriginalUiFrontendStatus50KG;
    AudioOutput audioOutput;
    rr::platform::OriginalUiFrontend50KG coordinator;
    rr::platform::OriginalUiFrontendDiagnostic50KG diagnostic{};
    std::uint64_t lastUpdateNanoseconds=0,inputCount=0,unavailableCount=0,frameCount=0;
    std::uint64_t captureSequence=0,lastAudioSequence=0;
    // Actual accepted renderer receipt; diagnostic is reused by later calls.
    std::uint8_t submittedScreen=0;
    std::uint64_t submittedPublication=0,sourceFrameGeneration=0,sourceTargetGeneration=0;
    std::uint32_t submittedDraws=0,submittedQuads=0,submittedTriangles=0;
    std::uint64_t lastSourceNanoseconds=0,activeSourceNanoseconds=0;
    std::uint64_t pendingUpdateColdNanoseconds=0,pendingMaterialColdNanoseconds=0;
    std::uint32_t sourceMilliseconds=0;
    bool sourceSampleKnown=false,previousSourceActive=false;
    bool imagePublished=false,hostPaused=false;
    rr::platform::OriginalUiMainFrameClock50KG sampleClock(
        rr::game::HostMonotonicClock50ID& applicationClock,bool active) {
        std::uint32_t rawSample=0;
        require(rr::game::sampleHostTimestamp50ID(applicationClock,rawSample),
            "original UI shared material timestamp unavailable");
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
        return {true,active,sourceMilliseconds};
    }
    void prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest) {
        using Id=rr::assets::OriginalUiArchiveId50KG;
        rr::platform::OriginalUiFrontendCold50KG cold;
        cold.appStart=readArchive(application.assetRoot,Id::appStart);
        cold.mainMenu=readArchive(application.assetRoot,Id::mainMenu);
        cold.options=readArchive(application.assetRoot,Id::options);
        cold.fonts=readArchive(application.assetRoot,Id::fonts);
        cold.controllerPopup=readArchive(application.assetRoot,Id::controllerPopup);
        cold.overlays=readArchive(application.assetRoot,Id::overlays);
        cold.localization=readArchive(application.assetRoot,Id::localization);
        cold.runtime=application.materialRuntime;cold.video=video->publication(video);
        cold.profile=application.profile;cold.application=application.state;
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
            cold.riderIpumCache=std::move(cache);
        }
        cold.optionsProfile=application.optionsProfile;cold.gameSettingsProfile=application.gameSettingsProfile;
        cold.soundMode=application.soundMode;
        cold.audioMode=application.audioMode;
        cold.textureRequest=textureRequest;
        cold.textureRequest.samplers=nullptr;cold.textureRequest.samplerCount=0;
        cold.textureRequest.maximumGpuBytes=32u*1024u*1024u;
        if(application.audioMode==rr::platform::OriginalUiMainAudioMode50KG::enabled){
            rr::platform::AudioOutputRequest50HD outputRequest;
            outputRequest.tryDevice=video->host.hasWindow();outputRequest.wavPath=std::getenv("RR_AUDIO_WAV");
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
        if(coordinator.update(in,receipt,diagnostic)!=Status::ok||!receipt.accepted||receipt.sequence!=lastAudioSequence+1)
            fail("update");
        // One PCM advance for this exact accepted source update; never for
        // loading, rejected updates, or a second screen's copied clock.
        rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
        if(!audioOutput.advanceAcceptedNanoseconds(elapsedNanoseconds,outputDiagnostic))
            throw std::runtime_error(std::string("original UI PCM advance failed: ")+outputDiagnostic.detail.data());
        lastAudioSequence=receipt.sequence;
    }
    void render(const rr::platform::OriginalUiMainFrameClock50KG& clock,
        rr::platform::VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height) {
        if(coordinator.render(clock,diagnostic)!=Status::ok)fail("render");
        if(coordinator.submit(renderer,width,height,diagnostic)!=Status::ok)fail("submission");
        submittedScreen=coordinator.snapshot().route.screen;
        submittedPublication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
        submittedDraws=diagnostic.renderer.originalUiDraws;submittedQuads=diagnostic.renderer.originalUiQuads;
        submittedTriangles=diagnostic.renderer.originalUiSourceTriangles;
        sourceFrameGeneration=diagnostic.renderer.originalUiSourceFrameGeneration;
        sourceTargetGeneration=diagnostic.renderer.originalUiSourceTargetGeneration;
    }
    void commitDeferred(rr::platform::VehicleWindowGl50DS& renderer) {
        const auto status=coordinator.commitDeferred(renderer,diagnostic);
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
        if(route.screen==12){
            const auto* resources=coordinator.optionsResources();
            const auto* widgets=resources?resources->widgets():nullptr;
            if(!widgets)return nullptr;
            if(route.page==1)return widgets->navigation();
            if(route.page==4){const auto* state=widgets->gameOptionsState();return state?&state->navigation:nullptr;}
        }
        return nullptr;
    }
    void report(const char* reason,const Application& application,std::uint64_t worldTicks,std::uint64_t mixedFrames) const {
        const auto snapshot=coordinator.snapshot();const bool main=snapshot.route.screen==4;
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
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames<<"}\n";
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
        std::cerr<<"}\n";
    }
};
}
