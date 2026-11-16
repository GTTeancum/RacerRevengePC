#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "native_gc_water_racer50kg_fixture.h"
#include "../../src/game/original_gc_generator_fire50kg.h"
#include "../../src/game/original_gc_pod_sparks50kg.h"
#include "authored_runtime_ground50cp_fixture.h"
#include <array>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace native_gc_water_session50KG {
using namespace rr::game;
struct Application final {
    native_water_runtime50JC::Application water;
    // Source3C9FDC writes434630=(0,0,0,1), once per application.
    float initialStep434630[4]{0,0,0,1};
    std::uint32_t directFireSerial433970=0;
    OriginalGcPodSparksGlobals50KG sparksGlobals;
};
// Explicit native schedule: actual completed native track spans own visibility;
// root service runs once after all racer child prefixes, before lap/camera.
// Source fog manager precedes fire/fog emitter-list updates. This is a declared
// native schedule, not a claim of retail render/update phase equivalence.
class Owner final {
    Application* app_=nullptr;
    OriginalGcParticleManager50KG particles_;
    OriginalGcGeneratorFireOwner50KG fires_;
    native_water_runtime50JC::Owner water_;
    std::array<OriginalGcFireAttachment50KG,6> attachments_{};
    std::array<std::uint32_t,6> worldInstance_{};
    std::vector<std::uint8_t> sprayBytes_,particlePsm_,particlePst_;
    std::uint64_t updateSequence_=0,renderGeneration_=0;
    std::uint16_t sceneRenderStamp_=1;
    bool failed_=false,prepared_=false,active_=false;
    static void need(bool value,const char* text){if(!value)throw std::runtime_error(text);}
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    void prepare(Application& app,const authored_runtime_ground50cp_fixture::ColdScene& ground,
        const char* actualTrackArchive,const char* actualPodArchive) {
        need(!prepared_&&!app_,"GC water session construction is one-shot");app_=&app;
        need(ground.height.sourceBound&&ground.height.nativeWaterResources&&
             ground.height.sourceDomain.terrain330==UndertrackHeightOwner50EW::Absent&&
             ground.height.sourceDomain.water334==UndertrackHeightOwner50EW::Present,
             "GC native water requires actual fresh330/334 ownership");
        auto archive=[&](const char* path,rr::assets::ResArchive48C& out,bool gc){
            need(path&&*path,"particle original archive path missing");
            std::ifstream f(rr::assets::dlc50DL::host(path),std::ios::binary);need(bool(f),"particle original archive missing");
            std::vector<std::uint8_t> raw{std::istreambuf_iterator<char>(f),{}};
            if(gc)need(pod_materials50ed_fixture::detail::sha256(raw)==
                "69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
                "particle GC archive identity differs");
            rr::assets::DecodeError48C error;
            need(rr::assets::decodeResource48C(raw,out,error),"particle original archive rejected");
        };
        rr::assets::ResArchive48C gc,pod;archive(actualTrackArchive,gc,true);archive(actualPodArchive,pod,false);
        auto member=[](const rr::assets::ResArchive48C& archive,const char* name){
            std::vector<std::uint8_t> bytes;water_resources50JC::Member identity;std::string error;
            if(!water_resources50JC::member(archive,name,bytes,identity,error))throw std::runtime_error(error);
            return bytes;
        };
        const auto prt=member(gc,"particle\\demoprt.prt"),fog=member(gc,"particle\\fog.emt");
        const auto tob=member(gc,"tracks\\gc.tob"),fire=member(gc,"particle\\genratorfire.emt");
        sprayBytes_=member(pod,"particle\\podSpray.emt");
        particlePsm_=member(gc,"particle\\demoprt.psm");particlePst_=member(gc,"particle\\demoprt.pst");
        need(particles_.prepare(prt,fog,tob)==OriginalGcParticleStatus50KG::complete,
            "actual GC particle registry/fog authoring rejected");
        need(fires_.prepare(particles_,fire)==OriginalGcParticleStatus50KG::complete,
            "actual six GC generator fire owners rejected");
        std::size_t count=0;
        for(std::size_t i=0;i<ground.world.instances.size();++i){const auto& instance=ground.world.instances[i];
            const auto slash=instance.geometry_member.find_last_of("/\\");
            const auto name=instance.geometry_member.substr(slash==std::string::npos?0:slash+1);
            if(!water_resources50JC::sameName(name,"shade_a.psg"))continue;
            need(count<6,"GC attachment source instance count differs");worldInstance_[count]=static_cast<std::uint32_t>(i);
            auto& attachment=attachments_[count++];
            for(unsigned lane=0;lane<16;++lane)attachment.actualWorldMatrix.v[lane]=static_cast<float>(instance.matrix[lane]);
            // Genuine native static model has a draw owner. Before any completed
            // draw its stamp cannot equal the native current-render stamp.
            attachment.renderOwnerPresent=true;attachment.renderStamp=0;
        }
        need(count==6,"GC six shade attachments could not bind original world instances");
        water_.initialize(app.water,ground.height.nativeWaterResources,native_water_runtime50JC::Policy::OneNativeView);
        prepared_=true;
    }
    // Resource preparation can run while Main owns the input/RNG. Actual
    // gameplay population is one-shot AFTER Course exit or direct activation,
    // before racer child registration. No dormant menu world draws randoms.
    bool activate(std::uint32_t& sharedRawRandom,SharedRandomOwner50ID& selectors,
        HostMonotonicClock50ID& clock) noexcept {
        if(!prepared()||active_)return false;
        failed_=true;
        if(!particles_.populateColdFog(sharedRawRandom,selectors,clock).okay())return false;
        active_=true;failed_=false;return true;
    }
    OriginalGcParticleManager50KG& particles() noexcept{return particles_;}
    native_water_runtime50JC::Owner& water() noexcept{return water_;}
    const std::vector<std::uint8_t>& sprayBytes() const noexcept{return sprayBytes_;}
    const std::vector<std::uint8_t>& particlePsm() const noexcept{return particlePsm_;}
    const std::vector<std::uint8_t>& particlePst() const noexcept{return particlePst_;}
    bool active() const noexcept{return prepared()&&active_;}
    const std::array<std::uint32_t,6>& attachmentWorldInstances() const noexcept{return worldInstance_;}
    bool prepared() const noexcept{return prepared_&&!failed_;}
    // Each bit is OR of actually submitted source spans for that exact world
    // instance, published only after successful GPU completion. A missing
    // instance/span receipt is rejection, never an assumed visible fire.
    bool completedRender(std::uint64_t generation,const std::array<bool,6>& drawn) noexcept {
        if(!active()||!generation||generation<renderGeneration_)return false;
        if(generation==renderGeneration_)return true;
        const auto result=water_.present(generation,true);
        if(result.status!=WaterPresentationStatus50JC::Published&&
           result.status!=WaterPresentationStatus50JC::DuplicateFrame)return false;
        renderGeneration_=generation;++sceneRenderStamp_;
        for(unsigned i=0;i<6;++i)attachments_[i].renderStamp=drawn[i]?sceneRenderStamp_:
            static_cast<std::uint16_t>(sceneRenderStamp_-1u);
        return true;
    }
    bool advance(float delta,std::uint32_t actualApplication11C,
        const float (&actualFollowedPodPosition)[3],std::uint32_t& sharedRawRandom,
        std::uint32_t& actualApplicationFireSerial,SharedRandomOwner50ID& selectors,
        HostMonotonicClock50ID& clock) noexcept {
        (void)selectors;(void)clock;
        if(!active()||updateSequence_==std::numeric_limits<std::uint64_t>::max())return false;
        // A partial real-source prefix is retained and cannot be replayed.
        failed_=true;const bool paused=actualApplication11C!=0;
        if(!particles_.advance(delta,paused).okay())return false;
        // NativeUndefinedCarryPolicy is explicitly the same documented native
        // integral/fraction policy as Main. No undefined guest register value
        // or source-exact particle count is claimed on15E288..298.
        if(!fires_.update(delta,paused,attachments_,sceneRenderStamp_,sharedRawRandom,
             actualApplicationFireSerial,true).okay())return false;
        if(particles_.advanceFogPhases(delta)!=OriginalGcParticleStatus50KG::complete)return false;
        const WaterPresentationView50JC view{actualFollowedPodPosition,0,true,true};
        if(water_.anchor(view).status!=native_water_runtime50JC::Status::Complete)return false;
        if(water_.advance(++updateSequence_,delta,actualApplication11C).status!=
             native_water_runtime50JC::Status::Complete)return false;
        failed_=false;return true;
    }
};
} // namespace native_gc_water_session50KG
