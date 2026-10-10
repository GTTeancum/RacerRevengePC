#include "vehicle_draw_batch50co.h"
#include <limits>
namespace rr::game {
bool buildVehicleDrawBatch50CO(const VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    const VehicleDrawBatchInput50CO&input,VehicleDrawBatch50CO&output,
    VehicleDrawWorkspace50CM&workspace,VehicleDrawBatchDiagnostic50CO&diagnostic)noexcept{
    diagnostic={};VehicleDrawBatch50CO next{};
    constexpr VehicleDrawComponent50CM expected[]={VehicleDrawComponent50CM::chariot,
        VehicleDrawComponent50CM::leftEngine,VehicleDrawComponent50CM::rightEngine};
    for(unsigned i=0;i<3;++i){
        diagnostic.component=i;
        if(input.selection[i].component!=expected[i]){
            diagnostic.detail.error=VehicleDrawError50CM::binding;return false;
        }
        if(!buildVehicleDraw50CM(frame,bindings,input.selection[i],input.mesh[i],
            input.staging[i],next.component[i],workspace,diagnostic.detail))return false;
        const auto&component=next.component[i];
        const auto limit=std::numeric_limits<std::uint32_t>::max();
        if(component.vertexCount>limit-next.vertices||component.indexCount>limit-next.indices||
            component.spanCount>limit-next.draws){diagnostic.detail.error=VehicleDrawError50CM::storage;return false;}
        next.vertices+=component.vertexCount;next.indices+=component.indexCount;next.draws+=component.spanCount;
        if(!i)next.bounds=component.bounds;
        else for(unsigned axis=0;axis<3;++axis){
            if(component.bounds.minimum[axis]<next.bounds.minimum[axis])next.bounds.minimum[axis]=component.bounds.minimum[axis];
            if(component.bounds.maximum[axis]>next.bounds.maximum[axis])next.bounds.maximum[axis]=component.bounds.maximum[axis];
        }
    }
    output=next;diagnostic={};return true;
}
}
