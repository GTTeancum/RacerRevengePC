#pragma once
// Actual packed-racer view0 numeric observation. No scheduling or drawing is implied.
namespace original_results_roster50LA { class Read; }
namespace original_main_entry50KG { struct Application; }
namespace rr::game { class OriginalResults10450LA; class OriginalRaceConstructor50LA; }
#include "original_camera_session50kx.h"
#include "original_subtype3_record_owner50la.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_position_stats_frame50la.h"
#include <array>
#include <cmath>
namespace original_position_stats_adapter50LA {
class Owner final {
    friend struct original_main_entry50KG::Application;
    friend class rr::game::OriginalResults10450LA;
    friend class original_results_roster50LA::Read;
    friend class rr::game::OriginalRaceConstructor50LA;
    using Camera=original_camera_session50KX::Session;
    // Source camera preparation constructs these two exact view0 identities.
    // Human count alone cannot authorize an AI/player reinterpretation.
    static bool supportedSourceView(const Camera& c)noexcept {
        if(!c.controller_.bound)return false;
        const auto& binding=c.controller_.binding;
        return (c.actual_.humanCount34==0u&&
                binding.bootstrap==rr::game::CameraBootstrap::native_all_ai&&
                binding.racerClass==rr::game::CameraRacerClass::ai)||
            (c.actual_.humanCount34==1u&&
                binding.bootstrap==rr::game::CameraBootstrap::original_single_player&&
                binding.racerClass==rr::game::CameraRacerClass::player);
    }
    // Only Application supplies these identities, taken from its real display
    // and canonical owners. Equality of camera/display counters is not proof.
    static bool belongsToApplication(const Camera& camera,
        const rr::game::OriginalApplicationEntities50LA* entities,
        const rr::assets::OriginalStandardMaterialTable50LA* materials,
        const std::string* directory,const rr::platform::VehicleWindowGl50DS* renderer,
        const rr::game::OriginalRendererDispatchFields* dispatch)noexcept;

    using Manager=native_ai50IV::Session;
    using Bank=native_racer50HN::Bank;
    const Camera* camera_=nullptr;
    const Manager* manager_=nullptr;
    std::uint64_t generation_=0;
    Bank::ReadHandle<std::uint32_t> state40_,countCD0_;
    bool identityCurrent()const {
        const auto& c=*camera_;const auto& m=*manager_;
        return !c.closed_&&!c.abandoned_&&c.coldBound_&&c.readsBound_&&
            c.generation_==generation_&&c.actual_.bank.count()>0&&c.actual_.bank.count()<=8&&supportedSourceView(c)&&
            &c.actual_.all8Manager==&m.manager&&m.managerBound&&m.rankBound&&
            m.retired40.bound&&m.retired40.activeCount50<=c.actual_.bank.count()&&
            c.controller_.bound&&c.controller_.binding.sessionGeneration==generation_&&
            c.controller_.binding.managerIndex==0&&c.identity_[0].managerIndex==0&&
            c.controller_.binding.racerKey==Camera::key(c.identity_[0].racerTag)&&
            c.controller_.binding.mainBodyKey==Camera::key(c.identity_[0].mainBodyTag)&&
            c.controller_.binding.sceneKey==Camera::key(c.scene_)&&
            (c.raceFlags1B0_&3u)==1u&&c.identitiesCurrent();
    }
public:
    enum class Status:std::uint8_t {complete,owner,alreadyBound,phase,identity,read};
    struct Diagnostic {Status status=Status::owner;};
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // Actual Camera and Manager must outlive this nonmoving borrow. Bind outside
    // every Bank lease. No caller provides a row, racer override or ready flag.
    bool bind(const Camera& camera,const Manager& manager,Diagnostic& d)noexcept {
        d={};if(camera_){d.status=Status::alreadyBound;return false;}
        try {
            if(!camera.generation_||camera.closed_||camera.abandoned_||
               !camera.coldBound_||!camera.readsBound_||!supportedSourceView(camera)||
               (!camera.actual_.bank.count()||camera.actual_.bank.count()>8)||&camera.actual_.all8Manager!=&manager.manager||
               !camera.identitiesCurrent())return false;
            if(camera.frameStage_!=Camera::FrameStage::idle&&camera.frameStage_!=Camera::FrameStage::published){
                d.status=Status::phase;return false;}
            const auto state=camera.actual_.bank.bindRead(camera.actual_.arena.bodies.main.vehicleState40);
            const auto count=camera.actual_.bank.bindRead(camera.actual_.racerTiming.countCD0);
            state40_=state;countCD0_=count;generation_=camera.generation_;
            camera_=&camera;manager_=&manager;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
    // Source1A73DC/1A73F0 passes the SAME manager-loop index to1750A0, which
    // stores its low nibble at175144. The retained native binding mirrors that
    // exact view0 call argument. This is not a followed-racer override; no other
    // view, rebinding or arbitrary index is admitted; the original single-player
    // and native all-AI bootstrap retain the same actual view0 manager identity.
    // Consume synchronously after real rank/camera publication and before the
    // next world tick; publication is an observation, not an async lifetime pin.
    bool capture(rr::game::OriginalPositionStatsNumbers50LA& output,Diagnostic& d)const noexcept {
        d={};if(!camera_||!manager_)return false;
        try {
            const auto& c=*camera_;const auto& m=*manager_;
            if(c.frameStage_!=Camera::FrameStage::published||!c.publicationCount_||
               !c.completedFrame_||!c.viewport_.prepared||!c.viewport_.matricesReady||
               c.viewport_.sessionGeneration!=generation_){d.status=Status::phase;return false;}
            if(!identityCurrent()){d.status=Status::identity;return false;}
            const auto index=c.controller_.binding.managerIndex;
            // Actual native manager3C row identity is manager order, not ranking
            // order. Its original rank pass publishes rank[index].placeCCC.
            if(!m.observed[index].bound||m.observed[index].managerIndex!=index||
               !m.fields[index].bound||m.fields[index].placeCCC!=m.rank[index].placeCCC){
                d.status=Status::identity;return false;}
            rr::game::OriginalPositionStatsNumbers50LA next;
            next.sessionGeneration=generation_;next.cameraPublication=c.publicationCount_;
            next.viewIndex=0;next.cameraManagerIndex=index;next.managerCount30=static_cast<std::uint32_t>(c.actual_.bank.count());
            next.placeCCC=m.rank[index].placeCCC;next.activeCount50=m.retired40.activeCount50;
            next.viewState40=c.actual_.bank.read(0,state40_);
            next.viewCountCD0=c.actual_.bank.read(0,countCD0_);
            next.preference10F9C=c.actual_.preferences.slots[0].field10F9C;
            next.subtype11258=c.actual_.launchKind11258;next.raceMode8C=c.raceMode8C_;
            next.raceFlags1B0=c.raceFlags1B0_;next.raceEntityFlags29=c.raceEntityFlags29_;
            next.uiFlags1E4=c.uiFlags1E4_;
            if(c.frameStage_!=Camera::FrameStage::published||c.publicationCount_!=next.cameraPublication||
               !identityCurrent()){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
private:
    // Private synchronous Results CPU read; not an original managerB0
    // construction receipt, live renderer lease, or public snapshot input.
    // Only the concrete Results owner can request/consume these actual rows.
    struct ResultsRows {
        struct Row {
            std::uint32_t vehicle58=0,racer60=0,retirementCD4=0;
            std::int32_t placeCCC=0,lapCC8=0;
            std::uint8_t flagsCD8=0;
            float timeCB8=0,valueCBC=0,remainingCC4=0;
        };
        std::array<Row,8> rows{};
        std::size_t count=0;
        const Bank* bank=nullptr;
        float trackLength38=0;
        std::uint64_t generation=0,publication=0;
    };
    // Separate selected-row borrow for1C9B10; no extra roster display reads.
    struct ResultsRecordIdentity {
        const Bank* bank=nullptr;
        const rr::game::OriginalApplicationEntities50LA* application=nullptr;
        const rr::assets::OriginalStandardMaterialTable50LA* materials=nullptr;
        std::uint64_t generation=0,publication=0;
        std::size_t managerIndex=0;
        std::uint32_t vehicle58=0,racer60=0;
    };
    bool captureResultsRecordIdentity(const rr::game::OriginalApplicationEntities50LA*,
        const rr::assets::OriginalStandardMaterialTable50LA*,std::uint64_t generation,
        std::size_t managerIndex,ResultsRecordIdentity&,Diagnostic&)const noexcept;
    bool resultsRecordIdentityCurrent(const ResultsRecordIdentity&)const noexcept;
    bool readResultsBestLapCB4(const ResultsRecordIdentity&,float&,Diagnostic&)const noexcept;
    bool readResultsRaceTimeCB8(const ResultsRecordIdentity&,float&,Diagnostic&)const noexcept;
    //Only the three actual lap-array reads reached by1C8430..843C.
    bool readResultsLapCC0(const ResultsRecordIdentity&,std::size_t,float&,Diagnostic&)const noexcept;
    // Actual current lane only: never a credited-destruction certificate.
    bool readResultsObservedTimingCD0(const ResultsRecordIdentity&,std::uint32_t&,Diagnostic&)const noexcept;
    const Bank* resultsBankIdentity()const noexcept {
        return camera_?&camera_->actual_.bank:nullptr; //No Bank dereference.
    }
    bool captureResultsRows(const rr::game::OriginalApplicationEntities50LA*,
        const rr::assets::OriginalStandardMaterialTable50LA*,std::uint64_t,
        ResultsRows&,Diagnostic&)const noexcept;
    bool resultsRowsCurrent(const rr::game::OriginalApplicationEntities50LA*,
        const rr::assets::OriginalStandardMaterialTable50LA*,std::uint64_t,
        std::uint64_t)const noexcept;
    //Private exact end-query facets; reuse selected-row identity/timing reads,
    //not the display roster's sorting or positive-place restrictions.
    struct EndObservation {
        std::uint64_t publication=0;
        std::int32_t managerCount30=0;
    };
    bool captureEndObservation1AB520(const rr::game::OriginalApplicationEntities50LA*,
        const rr::assets::OriginalStandardMaterialTable50LA*,std::uint64_t,
        EndObservation&,Diagnostic&)const noexcept;
    bool readEndRetirementCD4(const ResultsRecordIdentity&,std::uint32_t&,Diagnostic&)const noexcept;
    bool readEndPlaceCCC(const ResultsRecordIdentity&,std::int32_t&,Diagnostic&)const noexcept;
    struct RadarObservation {
        const rr::game::TrackNavigation50EU* map=nullptr;
        rr::game::TrackNavigationView50EU mbMap{};
        rr::game::TrackNavigationView50TC tcMap{};
        rr::game::TrackNavigationView50TB tbMap{};
        rr::game::TrackNavigationView50GB gbMap{};
        rr::game::TrackNavigationView50MC mcMap{};
        std::array<float,3> center{};
        float headingE4=0;
        std::array<std::array<float,3>,8> racerCenters{};
        std::array<std::uint32_t,8> racerStates{};
        std::uint64_t publication=0;
        bool recordBound=false,recordVisible=false;
        std::array<float,3> recordPosition{};
    };
    //1924C0 resolves managerB0[Application268], body340->scene288 and E4.
    //Use that same current view0 racer. The bank visit performs the original
    //lazy world-cache refresh; its canonical cache effects are not rolled back
    //if a later numeric admission fails. No local/world or camera-pose shortcut.
    bool captureRadar1924C0(RadarObservation& output,Diagnostic& d)const noexcept {
        rr::game::OriginalPositionStatsNumbers50LA observation;
        if(!capture(observation,d))return false;
        const auto& c=*camera_;
        const bool mb=c.actual_.actualRadarMapMb.gatesPrepared();
        const bool tc=c.actual_.actualRadarMapTc.gatesPrepared();
        const bool tb=c.actual_.actualRadarMapTb.gatesPrepared();
        const bool gb=c.actual_.actualRadarMapGb.gatesPrepared();
        const bool mc=c.actual_.actualRadarMapMc.gatesPrepared();
        if(mc?(gb||tb||tc||mb||c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapMc.runtimeValid()||
               c.actual_.actualRadarMapMc.role()!=rr::game::NavigationRole50MC::Map||c.actual_.actualRadarMapMc.count()!=173u):
           gb?(tb||tc||mb||c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapGb.runtimeValid()||
               c.actual_.actualRadarMapGb.role()!=rr::game::NavigationRole50GB::Map):
           tb?(tc||mb||c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapTb.runtimeValid()||
               c.actual_.actualRadarMapTb.role()!=rr::game::NavigationRole50TB::Map):
           tc?(mb||c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapTc.runtimeValid()||
               c.actual_.actualRadarMapTc.role()!=rr::game::NavigationRole50TC::Map):
           mb?(c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapMb.runtimeValid()||
               c.actual_.actualRadarMapMb.role()!=rr::game::NavigationRole50MB::Map):
              (!c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapB8->gatesPrepared)){
            d.status=Status::owner;return false;}
        try {
            RadarObservation next;next.map=c.actual_.actualRadarMapB8;
            if(tb)next.tbMap=c.actual_.actualRadarMapTb;
            if(gb)next.gbMap=c.actual_.actualRadarMapGb;
            if(mc)next.mcMap=c.actual_.actualRadarMapMc;
            if(tc)next.tcMap=c.actual_.actualRadarMapTc;
            if(mb)next.mbMap=c.actual_.actualRadarMapMb;
            next.publication=observation.cameraPublication;
            if(observation.subtype11258==3){
                std::uint8_t flags29=0;
                if(!c.actual_.recordManager40||!c.actual_.recordManager40->radarObservation(
                    generation_,flags29,next.recordPosition)){d.status=Status::owner;return false;}
                next.recordBound=true;next.recordVisible=(flags29&1u)!=0;
            }
            for(std::size_t slot=0;slot<c.actual_.bank.count();++slot){
            next.racerStates[slot]=c.actual_.bank.read(slot,c.reads_.state40);
            if(slot!=observation.viewIndex&&next.racerStates[slot]!=0)continue;
            const bool read=c.actual_.bank.visit(slot,[&]()noexcept {
                auto& arena=c.actual_.arena;
                const auto index=c.actual_.bindings.animation.bodyScene288;
                const auto node=static_cast<std::int32_t>(index);
                if(node<0||static_cast<std::uint32_t>(node)>=arena.sceneNodeCount)return false;
                if(!rr::game::refreshSceneWorld49P({arena.nodes,arena.sceneNodeCount},index))return false;
                const auto& world=arena.nodes[static_cast<std::size_t>(node)].world100;
                for(std::size_t axis=0;axis<3;++axis)next.racerCenters[slot][axis]=world.v[12+axis];
                if(slot==observation.viewIndex){next.center=next.racerCenters[slot];
                    next.headingE4=arena.bodies.main.cachedAnglesE0.v[1];}
                for(float value:next.racerCenters[slot])if(!std::isfinite(value))return false;
                return std::isfinite(next.headingE4);
            });
            if(!read){d.status=Status::read;return false;}
            }
            if(c.frameStage_!=Camera::FrameStage::published||c.publicationCount_!=next.publication||
               !identityCurrent()){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
};
inline bool Owner::belongsToApplication(const Camera& camera,
    const rr::game::OriginalApplicationEntities50LA* entities,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,
    const std::string* directory,const rr::platform::VehicleWindowGl50DS* renderer,
    const rr::game::OriginalRendererDispatchFields* dispatch)noexcept {
    return entities&&materials&&directory&&renderer&&dispatch&&camera.displayInputs_&&
        camera.actual_.applicationEntities.get()==entities&&
        camera.actual_.applicationMaterials.get()==materials&&
        camera.actual_.actualMaterialDirectory36B970==directory&&
        &camera.displayInputs_->renderer==renderer&&
        &camera.displayInputs_->rendererDispatch==dispatch;
}
}
