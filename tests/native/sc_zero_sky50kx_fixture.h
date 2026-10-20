#pragma once
#include "sc_cold_sources50la.h"
#include <array>
#include <memory>
#include <string>

namespace sc_zero_sky50KX {
// Authenticated absence, not a substitute cube or a GPU readiness receipt.
// Retains the real one-attempt SC source owner through every camera-zone read.
class Owner final {
    std::shared_ptr<const sc_cold_sources50LA::Owner> source_;
    std::array<float,4> clear_{};
    static const rr::assets::ParameterBlock48F* block(const rr::assets::ParameterDocument48F& p,const char* name) noexcept {
        const rr::assets::ParameterBlock48F* out=nullptr;
        for(const auto& b:p.blocks)if(b.name==name){if(out)return nullptr;out=&b;}return out;
    }
    static const std::string* field(const rr::assets::ParameterBlock48F& b,const char* name) noexcept {
        const std::string* out=nullptr;
        for(const auto& f:b.fields)if(f.name==name){if(out)return nullptr;out=&f.value;}return out;
    }
public:
    bool prepare(std::shared_ptr<const sc_cold_sources50LA::Owner> source,std::string& error) {
        if(source_){error="SC zero-sky owner already prepared";return false;}
        const auto* decoded=source?source->sourcesDecoded():nullptr;
        const auto* receipt=source?source->zeroSkyReceipt():nullptr;
        if(!decoded||!receipt||!receipt->authoredZeroAndArchiveHasNoCube()||decoded->world.zones.size()!=30){
            error="SC actual authenticated zero-sky source required";return false;}
        const auto& ordinals=receipt->sourceZoneOrdinals();
        for(std::size_t i=0;i<ordinals.size();++i)if(ordinals[i]!=i+1||decoded->world.zones[i].zone!=i+1){
            error="SC zero-sky source zone identity differs";return false;}
        const auto* fog=block(decoded->world.parameters,"fog");
        if(!fog){error="SC original Fog block missing or duplicated";return false;}
        const auto* rgb=field(*fog,"colorrgb");const auto* haze=field(*fog,"haziness");const auto* visibility=field(*fog,"visibility");
        // These are exact fields of the source owner's hash-authenticated SCB.
        // Per-zone interpolation/retail shader parity is not asserted here.
        if(!rgb||*rgb!="47,36,23"||!haze||*haze!="1"||!visibility||*visibility!="1"){
            error="SC authored global fog differs";return false;}
        clear_={{47.f/255.f,36.f/255.f,23.f/255.f,1.f}};
        source_=std::move(source);error.clear();return true;
    }
    bool ready()const noexcept {return source_&&source_->sourcesDecoded()&&source_->zeroSkyReceipt();}
    const float* clearRgba()const noexcept {return ready()?clear_.data():nullptr;}
    bool select(std::uint32_t actualCameraZone48,bool& enabled)const noexcept {
        if(!ready()||actualCameraZone48>=30)return false;
        const auto* decoded=source_->sourcesDecoded();
        if(decoded->world.zones[actualCameraZone48].zone!=actualCameraZone48+1)return false;
        enabled=false;return true;
    }
    // No texture/mesh/frame is manufactured. Root must omit the sky draw and
    // keep the actual world fog projection at authored fractions1 and1.
};
}
