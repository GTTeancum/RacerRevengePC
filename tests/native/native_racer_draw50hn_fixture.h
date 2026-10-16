#pragma once
#include "vehicle_draw_batch50co.h"
#include "pod_cables50fd.h"
#include <array>
#include <vector>

namespace native_racer50HN {
// Each opponent owns its transformed display vertices, normals and cable output.
// Source mesh/index/span/palette/texture data are borrowed from the very same
// authenticated asset owner selected for that racer. No rigid whole-pod transform is
// substituted: native49S/50CO prepares all actual component/node poses.
class DrawOwner {
public:
    DrawOwner()=default;
    DrawOwner(const DrawOwner&)=delete;
    DrawOwner& operator=(const DrawOwner&)=delete;
    bool initialize(const rr::game::VehicleDrawBatchInput50CO& source) {
        if(initialized_||!rr::game::validVehicleDrawComponentCount50CM(source.componentCount))return false;
        for(unsigned component=0;component<source.componentCount;++component){
            const auto count=source.mesh[component].vertexCount;
            if(!count||count>rr::game::maxRigidVertices49S)return false;
        }
        input_=source;
        for(unsigned component=0;component<source.componentCount;++component){
            vertices_[component].resize(source.mesh[component].vertexCount);
            normals_[component].resize(source.mesh[component].vertexCount);
            input_.staging[component]={vertices_[component].data(),
                static_cast<std::uint32_t>(vertices_[component].size()),
                normals_[component].data(),static_cast<std::uint32_t>(normals_[component].size())};
        }
        initialized_=true;return true;
    }
    bool prepare(const rr::game::VehicleFrameState49W& state,
        const rr::game::VehicleFrameBindings49W& bindings,
        rr::game::VehicleDrawWorkspace50CM& workspace,
        rr::game::VehicleDrawBatchDiagnostic50CO& diagnostic,
        const rr::game::VehicleDrawBatchSourceColors50LA* sourceColors=nullptr) noexcept {
        if(!initialized_)return false;
        valid_=rr::game::buildVehicleDrawBatch50CO(state,bindings,input_,batch_,workspace,diagnostic,sourceColors);
        return valid_;
    }
    const rr::game::VehicleDrawBatch50CO* batch()const noexcept{return valid_?&batch_:nullptr;}
    rr::game::PodCableMesh50FD cables{};
private:
    std::array<std::vector<rr::assets::Vertex48B>,rr::game::maxVehicleDrawComponents50CM> vertices_;
    std::array<std::vector<rr::assets::WorldNormal50HK>,rr::game::maxVehicleDrawComponents50CM> normals_;
    rr::game::VehicleDrawBatchInput50CO input_{};
    rr::game::VehicleDrawBatch50CO batch_{};
    bool initialized_=false,valid_=false;
};
}
