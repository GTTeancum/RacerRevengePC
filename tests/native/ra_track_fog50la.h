#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_camera_start_effects.h"

namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace ra_track_fog50LA {
// Private actual RA fog storage. CameraBinding retains this owner before its
// Session and releases it after Session teardown. Never a caller fog tuple.
class Owner final {
    friend class ra_cold_aggregate50LA::Owner;
    friend class ra_source_camera50LA::OwnershipBridge;
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    rr::game::OriginalRaceFogFields fields_;
    bool prepared_=false;
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(prepared_||!source)return false;
        const auto* decoded=source->sourcesDecoded();if(!decoded)return false;
        const auto& doc=decoded->world.parameters;
        if(doc.original_bytes.size()!=540795u)return false;
        const rr::assets::ParameterBlock48F* fog=nullptr;
        for(const auto& block:doc.blocks)if(block.name=="fog"){
            if(fog)return false;fog=&block;
        }
        // Exact fields from authenticated RA SCB, not another track's defaults.
        if(!fog||fog->offset!=38500u||fog->fields.size()!=3)return false;
        const auto& color=fog->fields[0];const auto& haze=fog->fields[1];const auto& visibility=fog->fields[2];
        if(color.name!="colorrgb"||color.value!="180,180,165"||color.value_offset!=38633u||
           haze.name!="haziness"||haze.value!=".5"||haze.value_offset!=38648u||
           visibility.name!="visibility"||visibility.value!=".9"||visibility.value_offset!=38645u)return false;
        rr::game::OriginalRaceFogFields next;
        next.color30=0xffb4b4a5u;next.authoredFraction3C=.9f;next.authoredFraction40=.5f;
        std::uint32_t colorProjection=0;
        // Shared real13E830 range arithmetic, with gameplay's literal minimum.
        if(rr::game::restoreOriginalRaceFog1A98DC(next,colorProjection).status!=
           rr::game::OriginalCameraStartStatus::complete)return false;
        fields_=next;source_=std::move(source);prepared_=true;return true;
    }
    bool current()const noexcept {
        return prepared_&&source_&&source_->sourcesDecoded()&&
            fields_.color30==0xffb4b4a5u&&fields_.authoredFraction3C==.9f&&
            fields_.authoredFraction40==.5f;
    }
};
}
