#pragma once
#include "original_camera_session50kx.h"
#include "../../src/game/original_damage50la.h"
namespace rr::game {
// Native read-only HUD observation owner. It deliberately does not manufacture
// a Player, reinterpret an AI tail, dispatch Player input, or write control data.
// Original scan output lives here for one synchronous camera publication only.
class OriginalDamageObservation50LA final {
    using Camera=original_camera_session50KX::Session;
    const Camera* camera_=nullptr;
    std::weak_ptr<OriginalDamage50LA> owner_;
    std::uint64_t generation_=0;
    bool identity(const Camera& c,const OriginalDamage50LA& owner)const noexcept {
        return !c.closed_&&!c.abandoned_&&c.coldBound_&&c.readsBound_&&c.displayInputs_&&
            c.generation_==generation_&&owner.entity_.session==generation_&&
            c.actual_.humanCount34==0&&c.actual_.bank.count()==8&&c.controller_.bound&&
            c.controller_.binding.bootstrap==CameraBootstrap::native_all_ai&&
            c.controller_.binding.racerClass==CameraRacerClass::ai&&
            c.controller_.binding.sessionGeneration==generation_&&
            c.controller_.binding.managerIndex==0&&c.identity_[0].managerIndex==0&&
            c.controller_.binding.racerKey==Camera::key(c.identity_[0].racerTag)&&
            c.controller_.binding.mainBodyKey==Camera::key(c.identity_[0].mainBodyTag)&&
            c.controller_.binding.sceneKey==Camera::key(c.scene_)&&
            c.actual_.applicationEntities==owner.entity_.application&&
            c.actual_.applicationMaterials==owner.domain_->materials382B38()&&
            owner.nativeCpuResourcesPrepared()&&c.identitiesCurrent();
    }
public:
    enum class Status:std::uint8_t {complete,owner,phase,identity,read,geometry};
    struct Diagnostic {Status status=Status::owner;std::uint32_t sourcePC=0;std::size_t slot=0;};
    bool bind(const Camera& camera,const std::shared_ptr<OriginalDamage50LA>& owner,Diagnostic& d)noexcept {
        d={};if(camera_||!owner)return false;
        generation_=camera.generation_;
        if(!generation_||!identity(camera,*owner))return false;
        if(camera.frameStage_!=Camera::FrameStage::idle&&camera.frameStage_!=Camera::FrameStage::published){d.status=Status::phase;return false;}
        camera_=&camera;owner_=owner;d.status=Status::complete;return true;
    }
    // Call after the actual manager/camera publication outside any Bank lease.
    // Owners must outlive this borrow. Capture finishes before another world tick.
    bool capture(OriginalDamageNativeFrame50LA& output,Diagnostic& d)const noexcept {
        d={};const auto owner=owner_.lock();if(!camera_||!owner)return false;
        try {
            const auto& c=*camera_;
            if(!identity(c,*owner)){d.status=Status::identity;return false;}
            if(c.frameStage_!=Camera::FrameStage::published||!c.publicationCount_||!c.completedFrame_||
               !c.viewport_.prepared||!c.viewport_.matricesReady||c.viewport_.sessionGeneration!=generation_){d.status=Status::phase;return false;}
            OriginalDamageNativeFrame50LA next;
            next.sessionGeneration=generation_;next.cameraPublication=c.publicationCount_;
            next.viewport=&c.viewport_;next.viewIndex=0;next.raceMode8C=c.raceMode8C_;
            next.raceFlags1B0=c.raceFlags1B0_;next.raceEntityFlags29=c.raceEntityFlags29_;next.uiFlags1E4=c.uiFlags1E4_;
            std::array<OriginalDamageProximityRacer50LA,8> racers{};
            std::array<float,3> forward{};
            for(std::size_t slot=0;slot<racers.size();++slot){d.slot=slot;
                racers[slot].state40=c.actual_.bank.read(slot,c.reads_.state40);
                if(slot!=0&&racers[slot].state40!=0)continue;
                const bool read=c.actual_.bank.visit(slot,[&]()noexcept {
                    auto& arena=c.actual_.arena;
                    const auto index=c.actual_.bindings.animation.bodyScene288;
                    const auto nodeIndex=static_cast<std::int32_t>(index);
                    if(nodeIndex<0||static_cast<std::uint32_t>(nodeIndex)>=arena.sceneNodeCount||
                       !refreshSceneWorld49P({arena.nodes,arena.sceneNodeCount},index))return false;
                    const auto& node=arena.nodes[nodeIndex];
                    for(unsigned axis=0;axis<3;++axis)racers[slot].worldPosition[axis]=node.world100.v[12+axis];
                    if(slot==0){
                        for(unsigned axis=0;axis<3;++axis)forward[axis]=node.localC0.v[8+axis];
                        next.viewVehicleState40=arena.bodies.main.vehicleState40;
                        next.damage7D4=arena.fields.damage7D4;next.damage7DC=arena.fields.damage7DC;
                        next.damage7D0=arena.fields.damage7D0;next.damage7D8=arena.fields.damage7D8;
                        next.flagsB9C=arena.bodies.main.controls.player.flagsB9C;
                    }
                    return true;
                });
                if(!read){d.status=Status::read;d.sourcePC=0x28b180;return false;}
            }
            // This query replaces only the Player-owned presentation output,
            // preserving the original scan's actual input owners and algorithm.
            if(!observeOriginalDamageProximity1A1EB0(racers.data(),racers.size(),0,forward,next.proximity)){
                d.status=Status::geometry;d.sourcePC=0x1a1eb0;return false;}
            if(c.frameStage_!=Camera::FrameStage::published||c.publicationCount_!=next.cameraPublication||!identity(c,*owner)){
                d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
};
}
