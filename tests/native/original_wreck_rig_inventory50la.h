#pragma once
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/game/original_wreck_asset_inventory50la.h"

namespace original_wreck_rig50LA {
// Cold join to the SAME retained roster Rig and authenticated full RES bytes.
// No file is reopened or model imported here beyond the inventory's existing
// RES/PHY readers. Loading piece geometry remains explicit loadNextCold calls.
inline rr::game::OriginalWreckInventoryResult50LA inventoryRetainedRig(
    const native_racer50HW::Rig& rig,const std::vector<std::uint8_t>& retainedRes,
    const rr::game::OriginalWreckConstructorVectors50LA& applicationVectors,
    rr::game::OriginalWreckAssetInventory50LA& inventory,std::string& error) {
    using S=rr::game::OriginalWreckInventoryStatus50LA;
    const auto* selected=rr::game::original_racer_selection50IJ::findAnyByPodId(rig.podId);
    const auto* pin=selected?original_player_identity50IK::find(selected->racer60):nullptr;
    if(!pin||retainedRes.size()!=pin->archiveBytes||
       pod_materials50ed_fixture::detail::sha256(retainedRes)!=pin->archiveSha256||
       pod_materials50ed_fixture::detail::sha256(rig.phy)!=pin->phySha256){
        error="original wreck inventory Rig/RES/PHY authentication differs";return {S::binding,0,0,0};
    }
    const auto pod=native_racer50HW::rig_detail::lower(rig.podId);
    const auto prefix=std::string("pods\\")+pod+"\\";
    rr::game::OriginalWreckAssetBinding50LA binding;
    binding.archivePath=rig.resourceFile.generic_string();binding.memberPrefix=prefix;
    binding.phyMember=prefix+pod+".phy";binding.constructorVectors=&applicationVectors;
    return inventory.inventoryRetained(binding,retainedRes,rig.phy,error);
}
} // namespace original_wreck_rig50LA
