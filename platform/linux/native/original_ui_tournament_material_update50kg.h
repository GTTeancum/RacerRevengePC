#pragma once
#include "original_ui_tournament_career_resources50kg.h"
#include <array>
#include <memory>

namespace rr::platform {
// One original2DF8A0 call, not one distinct texture. Repeated aliases MUST
// survive: source loops visit every constructed ScreenTexture in order.
struct OriginalUiTournamentMaterialDemand50KG {
    std::uint32_t objectOffset=0,sourceCallerPC=0,image=0;
    const char* registeredMaterial=nullptr;
    float minimumDemand10=10.f;
};
// Retained source1F24B0 traversal prefix. This is NOT an applied material
// update, registry resolution receipt, full frame, or renderer retirement.
// No mutable TextureResource is accepted from the caller. A future canonical
// registry join must resolve ScreenTexture54 ->382B38 row ->pass4C map1A ->
//383400 row before raising that SAME live resource's demand10.
class OriginalUiTournamentMaterialUpdate50KG final {
public:
    OriginalUiTournamentMaterialUpdate50KG()=default;
    OriginalUiTournamentMaterialUpdate50KG(const OriginalUiTournamentMaterialUpdate50KG&)=delete;
    OriginalUiTournamentMaterialUpdate50KG& operator=(const OriginalUiTournamentMaterialUpdate50KG&)=delete;
    bool prepare(std::shared_ptr<const OriginalUiTournamentCareerResources50KG> owner) noexcept {
        if(owner_||!owner||!owner->current())return false;
        const auto* materials=owner->materials();
        const auto* medals=owner->medals();
        const auto* trophies=owner->trophyImages();
        const auto* visible=owner->trophyVisibility4050();
        const auto* counts=owner->statCounts3D60();
        if(!materials||!medals||!trophies||!visible||!counts)return false;
        std::array<OriginalUiTournamentMaterialDemand50KG,151> next{};
        std::size_t count=0;
        const auto append=[&](std::uint32_t offset,std::uint32_t pc,std::uint32_t image)->bool {
            if(count==next.size()||image>=materials->size()||
                (*materials)[image].registeredName.empty())return false;
            next[count++]={offset,pc,image,(*materials)[image].registeredName.c_str(),10.f};
            return true;
        };
        if(!append(0x1000,0x1F24D4,13))return false;
        for(unsigned i=0;i<13;++i){
            if(!append(0x4C0+i*0x60,0x1F2508,1)||
                !append(0x9A0+i*0x60,0x1F253C,(*medals)[i].image))return false;
        }
        constexpr std::uint32_t trophyCalls[]{0x1F2584,0x1F25BC,0x1F25F4};
        for(unsigned i=0;i<3;++i){
            if((*visible)[i]>1)return false;
            if((*visible)[i]&&!append(0xE80+i*0x60,trophyCalls[i],(*trophies)[i]))return false;
        }
        //15 identifies medstar's first decoded frame only. The demand target
        // is its original material map token, NOT this image or selected frame.
        if(!append(0xFA0,0x1F2624,15))return false;
        for(unsigned row=0;row<6;++row){
            if((*counts)[row]>20)return false;
            for(unsigned n=0;n<(*counts)[row];++n)
                if(!append(0x1060+row*0x780+n*0x60,0x1F2668,12))return false;
        }
        if(!owner->current())return false;
        requests_=next;count_=count;owner_=std::move(owner);return true;
    }
    // No popup gate:1F24B0 has none. Popup29 gates DRAW1F2700 separately.
    bool current() const noexcept {return owner_&&owner_->current();}
    std::size_t requestCount() const noexcept {return current()?count_:0;}
    const OriginalUiTournamentMaterialDemand50KG* request(std::size_t index) const noexcept {
        return current()&&index<count_?&requests_[index]:nullptr;
    }
    // Keep the lease alive while borrowing request/name pointers. Profile
    // changes invalidate new access but never prematurely release the assets.
    std::shared_ptr<const OriginalUiTournamentCareerResources50KG> resources() const noexcept {return owner_;}
    static constexpr std::uint32_t unresolvedInheritedCallerPC=0x1F26C4;
    static constexpr std::uint32_t unresolvedInheritedTargetPC=0x141C30;
private:
    std::shared_ptr<const OriginalUiTournamentCareerResources50KG> owner_;
    std::array<OriginalUiTournamentMaterialDemand50KG,151> requests_{};
    std::size_t count_=0;
};
}
