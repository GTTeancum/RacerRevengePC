#include "original_save_service_protocol50kg.h"

namespace rr::game {
OriginalSaveDeviceClassification50KG classifyOriginalSaveDevice23AF3050KG(
    const OriginalSaveDeviceObservation50KG& input,std::uint32_t seen,bool wantsFree) noexcept {
    OriginalSaveDeviceClassification50KG out;
    // Source1(15-block),3(PocketStation),and all other non2 types return0.
    // Only type2 enters format/completion/free-space logic; no inferred device.
    if(input.type!=2)return out;
    if(input.formatted==0){out.rawStatus=input.completionResult<0?-2:-1;return out;}
    if(wantsFree){out.writesFreeBlocks=true;out.freeBlocks=input.freeBlocks;}
    //23B020..48: negative completion plus previously seen returns switched2.
    if(input.completionResult<0&&seen!=0){out.rawStatus=2;return out;}
    //23B04C..74: result0 OR previously unseen publishes seen=1.
    if(input.completionResult==0||seen==0){out.writesSeenCard=true;out.seenCard3C8=1;}
    out.rawStatus=1;return out;
}
}
