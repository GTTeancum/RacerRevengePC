#pragma once
#include "vehicle_static_query50h.h"
#include "indexed_type2_contact50cc.h"
namespace rr::game {
// Indexed variants retain the50H/50J frame admission and current scene preparation.
Type2StaticStatus49X queryMainIndexedStaticVehicle50CD(const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,const MainStaticDetectorAdmission50H&admission,
    const Type2StaticQuery49X&source,const Type2StaticCandidate49X*candidates,std::size_t ownerCount,const std::size_t*indices,std::size_t count,
    const Type2StaticInitial49X&initial,const Type2StaticWorkspace49X&w,
    const Type2StaticDestination49X&d,Type2StaticResult49X&result) noexcept;
Type2StaticStatus49X querySecondaryIndexedStaticVehicle50CD(const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,const MainStaticDetectorAdmission50H&admission,
    const Type2StaticQuery49X&source,const Type2StaticCandidate49X*candidates,std::size_t ownerCount,const std::size_t*indices,std::size_t count,
    const Type2StaticInitial49X&initial,const Type2StaticWorkspace49X&w,
    const Type2StaticDestination49X&d,Type2StaticResult49X&result) noexcept;
}
