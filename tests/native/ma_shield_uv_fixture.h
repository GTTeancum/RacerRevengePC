#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/game/original_ma_shield_uv.h"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <vector>

// No archive/PSM/image decoder, clock read, renderer call or GPU state here.
// The existing track owner has already authenticated source material records.
namespace ma_shield_uv_fixture {
constexpr std::uint32_t unresolved=0xffffffffu;
constexpr const char* materialSha=
    "a6621de3dd3812bc4c3ccac06519f2ee276623b1e16c0cfced5cbf2c358e0f5a";
struct Diagnostic { std::uint32_t sourceSpanIndex=unresolved; std::string message; };
struct Binding {
    std::uint32_t sourceSpanIndex=0, worldInstanceIndex=0;
    std::uint32_t materialSelection=0, sourceMaterialIndex=0;
};
class Owner final {
    struct Instance {
        std::uint32_t worldInstanceIndex=0;
        rr::game::OriginalMaShieldUv state{};
    };
    struct Storage {
        // The single pinned shader pass owns a prototype. Original 29B1D0
        // returns this prototype when no explicit instance table exists.
        rr::game::OriginalMaShieldUv prototype{};
        std::vector<Binding> bindings;
        std::vector<std::uint32_t> spanBinding;
        std::vector<Instance> instances;
        bool serviceStarted=false;
    };
    std::unique_ptr<Storage> storage_;
    rr::game::OriginalMaShieldUv* resolve(std::uint32_t span) noexcept {
        if(!storage_ || span>=storage_->spanBinding.size()) return nullptr;
        const auto binding=storage_->spanBinding[span];
        if(binding==unresolved || binding>=storage_->bindings.size()) return nullptr;
        if(storage_->instances.empty()) return &storage_->prototype;
        const auto world=storage_->bindings[binding].worldInstanceIndex;
        for(auto& instance:storage_->instances)
            if(instance.worldInstanceIndex==world) return &instance.state;
        // Once an instance table exists, 29B1D0 requires an exact model match.
        // Missing instances must not silently fall back to a shared prototype.
        return nullptr;
    }
    const rr::game::OriginalMaShieldUv* resolve(std::uint32_t span) const noexcept {
        if(!storage_ || span>=storage_->spanBinding.size()) return nullptr;
        const auto binding=storage_->spanBinding[span];
        if(binding==unresolved || binding>=storage_->bindings.size()) return nullptr;
        if(storage_->instances.empty()) return &storage_->prototype;
        const auto world=storage_->bindings[binding].worldInstanceIndex;
        for(const auto& instance:storage_->instances)
            if(instance.worldInstanceIndex==world) return &instance.state;
        return nullptr;
    }
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool loaded() const noexcept { return bool(storage_); }
    const std::vector<Binding>* bindings() const noexcept {
        return storage_?&storage_->bindings:nullptr;
    }
    const rr::game::OriginalMaShieldUvRows* rows(std::uint32_t sourceSpanIndex) const noexcept {
        const auto* state=resolve(sourceSpanIndex);
        return state?&state->matrix:nullptr;
    }
    // Call for a source span only when its material is actually serviced by
    // the chosen native render boundary. Multiple surfaces/placements sharing
    // a source controller in one application tick update that controller once.
    bool service(std::uint32_t sourceSpanIndex, std::uint32_t sourceApplicationTimestamp,
        bool applicationPaused, bool callbackAllowsUpdate=true) noexcept {
        auto* state=resolve(sourceSpanIndex);
        if(!state) return false;
        const bool ok=rr::game::serviceOriginalMaShieldUv(*state,sourceApplicationTimestamp,
            applicationPaused,callbackAllowsUpdate);
        if(ok) storage_->serviceStarted=true;
        return ok;
    }
    // Mirrors an EXPLICIT 26EF10/29B050 registration, not a declaration of
    // VertexAnimation=unique. Do not call merely because a world span exists.
    // Repeated surfaces for one material/model reuse the same mutable clone.
    // The present MA TOB has no animation trigger requiring these registrations.
    bool registerOriginalModelInstance(std::uint32_t worldInstanceIndex, Diagnostic& d) noexcept {
        using pod_materials50ed_fixture::detail::need;
        try {
            d={}; need(storage_&&!storage_->serviceStarted,"MA UV registration requires its cold owner");
            bool present=false;
            for(const auto& binding:storage_->bindings) present|=binding.worldInstanceIndex==worldInstanceIndex;
            need(present,"MA UV registration has no authenticated source model/material");
            for(const auto& instance:storage_->instances)
                if(instance.worldInstanceIndex==worldInstanceIndex) return true;
            need(storage_->instances.size()<4096,"MA UV instance capacity");
            const Instance added{worldInstanceIndex,rr::game::cloneOriginalMaShieldUv(storage_->prototype)};
            storage_->instances.push_back(added);
            std::sort(storage_->instances.begin(),storage_->instances.end(),
                [](const auto& a,const auto& b){return a.worldInstanceIndex<b.worldInstanceIndex;});
            return true;
        } catch(const std::exception& e) { try { d.message=e.what(); } catch(...) {} return false; }
          catch(...) { try { d.message="MA UV instance preparation failure"; } catch(...) {} return false; }
    }
    // Cold read-only association with authenticated material identity. There is
    // one source material/pass here even if repeated scene indices name it.
    // This does not automatically create per-model source registrations.
    bool load(const track_materials50fa_fixture::Owner& track, Diagnostic& d) noexcept {
        return load<track_materials50fa_fixture::Owner>(track,d);
    }
    // Read-only cold borrow of the existing authenticated Span/Selection
    // records. Direct owners keep their original indices; no view, reference,
    // or material owner is retained after association.
    template<class StaticMaterialOwner>
    bool load(const StaticMaterialOwner& track, Diagnostic& d) noexcept {
        namespace shared=pod_materials50ed_fixture::detail;
        using shared::need;
        try {
            d={}; need(!storage_&&track.loaded(),"MA UV requires the existing loaded material owner");
            const std::vector<track_materials50fa_fixture::Span>* borrowedSpans=track.spans();
            const std::vector<track_materials50fa_fixture::Selection>* borrowedSelections=track.selections();
            need(borrowedSpans&&borrowedSelections,"MA UV source spans/selections unavailable");
            const auto& spans=*borrowedSpans; const auto& selections=*borrowedSelections;
            need(spans.size()<=1000000,"MA UV source span capacity");
            const auto key=shared::sha256(std::string("rr-native-material-48E-v1\n")+materialSha);
            auto next=std::make_unique<Storage>();
            next->spanBinding.assign(spans.size(),unresolved);
            for(std::uint32_t si=0;si<spans.size();++si) {
                d.sourceSpanIndex=si;
                const auto& span=spans[si];
                need(span.materialSelection<selections.size(),"MA UV source material selection extent");
                const auto& selection=selections[span.materialSelection];
                if(shared::normalized(selection.materialName)!="MA_SHIELD") continue;
                need(selection.track==track_source_fixture::Track::ma && selection.materialKey==key &&
                    selection.pass==1 && selection.passCount==1 && selection.slot==1 &&
                    shared::normalized(selection.program)=="DIFFUSETEXTRANSCOMPRESSED" &&
                    shared::normalized(selection.textureName)=="MA_SHIELD" &&
                    !selection.projectedPrelit && !selection.staticAnimationFrame &&
                    !selection.clampU && !selection.clampV && selection.textureIndex!=unresolved &&
                    span.sourcePass==1 && span.sourcePassCount==1 && span.projectionPass==0 &&
                    span.textureIndex==selection.textureIndex,
                    "MA UV exact original material/pass/texture binding");
                need(next->bindings.size()<4096,"MA UV binding capacity");
                next->spanBinding[si]=static_cast<std::uint32_t>(next->bindings.size());
                next->bindings.push_back({si,span.worldInstanceIndex,span.materialSelection,selection.materialIndex});
            }
            need(!next->bindings.empty(),"No original MA_Shield placed material bindings");
            storage_=std::move(next); d={}; return true;
        } catch(const std::exception& e) { try { d.message=e.what(); } catch(...) {} return false; }
          catch(...) { try { d.message="MA UV source association failure"; } catch(...) {} return false; }
    }
};
}
