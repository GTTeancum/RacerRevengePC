#pragma once
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include "../../src/game/lap_host_clock50id.h"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace original_main_entry50KG {
inline void require(bool accepted,const char* message) {
    if(!accepted)throw std::runtime_error(message);
}
// NEW native application startup for the Main-only opt-in. This is a real
// process owner, not an imported save or a reconstruction of retail boot.
// The otherwise loaded gameplay world is never advanced in this mode. A
// future gameplay destination MUST join this same application RNG owner before
// it is admitted; copying these fields into an independently running RNG is
// not supported. The frontend cannot restart into the host driving path.
struct Application {
    bool enabled=false;
    std::filesystem::path assetRoot;
    rr::assets::OriginalUiMaterialRuntime50KG materialRuntime{};
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> profile;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> state;
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
        require(audio && std::string(audio)=="muted",
            "Main entry currently requires explicit RR_ORIGINAL_MAIN_AUDIO=muted");
        require(!std::getenv("RR_AUDIO_WAV"),"muted original Main cannot produce an audio proof recording");
        profile=std::make_shared<rr::platform::OriginalUiMainProfile50KG>();
        state=std::make_shared<rr::platform::OriginalUiMainApplication50KG>();
        // Fresh native backing starts with no imported unlocks or debug bits.
        // Source1864B0/186EF0 reset only its proven Main fields.
        profile->known=true;
        rr::platform::resetOriginalUiMainProfileFields50KG(*profile);
        // One native startup seed from the existing process clock, never per
        // frame, capture or focus change. Exact original boot seeding unclaimed.
        require(rr::game::sampleHostTimestamp50ID(clock,state->rawRandom308030),
            "Main startup shared application timestamp unavailable");
        state->known=true;
        materialRuntime={true,false}; // Process-owned fresh native normal-material mode.
        std::cerr<<"ORIGINAL MAIN ONLY: fresh native profile/application ownership; "
            "one clock-derived native RNG seed="<<state->rawRandom308030
            <<"; explicit muted audio. Rider/Course/Options/Gallery are unavailable. "
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
    // Member order destroys session before GPU before CPU. The outer caller
    // closes renderer then GPU explicitly while Video::host remains current.
    std::shared_ptr<rr::platform::OriginalUiMainDrawResources50KG> resources;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> mainArchive;
    std::shared_ptr<rr::platform::MaterialTextureGl50EB> gpu;
    std::unique_ptr<rr::platform::OriginalUiMainSession50KG> session;
    rr::platform::OriginalUiMainSessionDiagnostic50KG diagnostic{};
    std::uint64_t lastUpdateNanoseconds=0,inputCount=0,unavailableCount=0,frameCount=0;
    std::uint64_t captureSequence=0;
    std::uint32_t sourceMilliseconds=0,lastSourceSample=0;
    bool sourceSampleKnown=false,previousSourceActive=false;
    bool imagePublished=false,hostPaused=false;
    rr::platform::OriginalUiMainFrameClock50KG sampleClock(
        rr::game::HostMonotonicClock50ID& applicationClock,bool active) {
        std::uint32_t sample=0;
        require(rr::game::sampleHostTimestamp50ID(applicationClock,sample),
            "original Main shared material timestamp unavailable");
        if(!sourceSampleKnown)sourceMilliseconds=sample;
        else if(active && previousSourceActive)sourceMilliseconds+=sample-lastSourceSample;
        // The underlying application clock is never reset. This presentation
        // pause gate accumulates its actual quantized active intervals, so
        // material animations cannot spend paused wall time after resuming.
        lastSourceSample=sample;sourceSampleKnown=true;previousSourceActive=active;
        return {true,active,sourceMilliseconds};
    }
    void prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest) {
        using Id=rr::assets::OriginalUiArchiveId50KG;
        rr::platform::OriginalUiMainColdInput50KG cold;
        cold.appStart=readArchive(application.assetRoot,Id::appStart);
        cold.mainMenu=readArchive(application.assetRoot,Id::mainMenu);
        cold.fonts=readArchive(application.assetRoot,Id::fonts);
        cold.controllerPopup=readArchive(application.assetRoot,Id::controllerPopup);
        cold.overlays=readArchive(application.assetRoot,Id::overlays);
        const auto localization=readArchive(application.assetRoot,Id::localization);
        rr::assets::OriginalUiEnglish50KG english;
        rr::assets::OriginalUiLocalizationDiagnostic50KG localizationDiagnostic;
        require(rr::assets::readOriginalUiEnglish50KG(*localization,english,localizationDiagnostic),
            "original English LOC strings rejected");
        cold.english=&english;
        cold.runtime=application.materialRuntime;
        cold.video=video->publication(video);
        resources=std::make_shared<rr::platform::OriginalUiMainDrawResources50KG>();
        if(!resources->load(cold,diagnostic.draw))fail("resource preparation");
        mainArchive=cold.mainMenu;
        gpu=std::make_shared<rr::platform::MaterialTextureGl50EB>();
        auto request=textureRequest;
        const auto* samplers=resources->samplers();
        require(samplers && resources->textures(),"original Main CPU bank missing");
        request.samplers=samplers->data();
        request.samplerCount=static_cast<std::uint32_t>(samplers->size());
        request.maximumGpuBytes=32u*1024u*1024u;
        rr::platform::MaterialTextureGlDiagnostic50EB textureDiagnostic;
        if(!gpu->create(request,*resources->textures(),textureDiagnostic))
            throw std::runtime_error("original Main texture upload rejected: error="+
                std::to_string(static_cast<unsigned>(textureDiagnostic.error)));
        rendererRequest.enableOriginalUi=true;
        rendererRequest.originalUiCpuTextures=resources->textures();
        rendererRequest.originalUiTextures=gpu.get();
        rr::platform::OriginalUiMainSessionCold50KG sessionCold;
        sessionCold.resources=resources;sessionCold.gpu=gpu;sessionCold.mainArchive=mainArchive;
        sessionCold.profile=application.profile;sessionCold.application=application.state;
        sessionCold.audioMode=rr::platform::OriginalUiMainAudioMode50KG::muted;
        session=std::make_unique<rr::platform::OriginalUiMainSession50KG>();
        if(session->bind(sessionCold,diagnostic)!=rr::platform::OriginalUiMainSessionStatus50KG::ok)
            fail("session bind");
    }
    [[noreturn]] void fail(const char* operation) const {
        throw std::runtime_error(std::string("original Main ")+operation+" rejected: status="+
            std::to_string(static_cast<unsigned>(diagnostic.status))+" source="+
            std::to_string(diagnostic.sourcePC)+" draw="+
            std::to_string(static_cast<unsigned>(diagnostic.draw.error))+" drawSource="+
            std::to_string(diagnostic.draw.sourcePC));
    }
    void input(std::uint8_t sourceInput) {
        rr::platform::OriginalUiMainSessionInput50KG input;
        input.controller=0;input.originalInput=sourceInput;
        rr::platform::OriginalUiMainSessionInputReceipt50KG receipt;
        const auto status=session->input(input,receipt,diagnostic);
        using Status=rr::platform::OriginalUiMainSessionStatus50KG;
        ++inputCount;
        if(status==Status::unavailableDestination || status==Status::secondControllerRequired){
            ++unavailableCount;
            std::cerr<<"original_main_destination unavailable status="<<static_cast<unsigned>(status)
                <<"; Main remains active; no destination or gameplay state was entered.\n";
        }else if(status!=Status::ok && status!=Status::noChange)fail("input");
    }
    void report(const char* reason,const Application& application,
        std::uint64_t worldTicks,std::uint64_t mixedFrames) const {
        const auto state=session->snapshot();
        const auto* navigation=resources->manager()->navigation(1);
        require(navigation,"original Main live navigation missing");
        const auto* fire=session->fire();
        require(fire,"original Main particle owner missing");
        const auto* draw=session->draw();
        std::cerr<<"original_main_state {\"reason\":\""<<reason<<"\",\"updates\":"<<state.updateCount
            <<",\"renders\":"<<state.renderCount<<",\"submitted\":"<<frameCount
            <<",\"selected_slot\":"<<navigation->groups[1].selectedSlot
            <<",\"saved_slot\":"<<application.profile->savedSlot6E0
            <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
            <<",\"source_rng\":"<<application.state->rawRandom308030
            <<",\"fire_serial\":"<<application.state->fireSerial433970
            <<",\"input_shown\":"<<(state.inputContainerShown?"true":"false")
            <<",\"lifecycle\":"<<static_cast<unsigned>(state.transition.lifecycle)
            <<",\"aperture_state\":"<<static_cast<unsigned>(state.aperture.state)
            <<",\"aperture_top\":"<<state.aperture.top284
            <<",\"aperture_bottom\":"<<state.aperture.bottom28C
            <<",\"aperture_input_enabled\":"<<(state.aperture.inputEnabled268?"true":"false")
            <<",\"particles\":"<<fire->particleCount()<<",\"source_packets\":"<<fire->packetCount()
            <<",\"quads\":"<<(draw?draw->draw.quadCount:0)
            <<",\"spans\":"<<(draw?draw->draw.spanCount:0)
            <<",\"source_milliseconds\":"<<sourceMilliseconds
            <<",\"audio_muted\":true,\"audio_suppressed\":"<<state.audio.suppressedByMutedMode
            <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames<<"}\n";
    }
};
}
