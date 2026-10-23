#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include <cstring>

namespace rr::platform {
// Actual1FDE40 wrapper suffix. This is not an alternate material rectangle:
// the original two2E1920 owners, their aliases and their FIFO records survive.
bool OriginalUiGaragePageOwner50KG::drawShared1D7550(
    OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
    d={};
    if(!draw.identityCurrent()||draw.page_.get()!=this||draw.callback_!=&entity_||
       draw.phase_!=decltype(draw.phase_)::thirdPass||drawPending_||
       drawPublication_!=draw.publication_||drawSourcePC_!=0x1FDD54||
       !sourceEntryReturned1FAE00()||!shared_.state4B4||!shared_.top284||!shared_.bottom28C)
        return fail(d,Status::runtimePending,0x1D7550);
    const auto session=draw.session_.lock();
    if(!session)return fail(d,Status::runtimePending,0x1D7550);
    drawPending_=true;drawSourcePC_=0x1D7550;
    if(!advanceSharedApertures1D7550(*session,draw,d))return false;
    for(std::size_t i=0;i<2;++i){
        if(!canMutateAperture2E21D0(i)||!shared_.apertures424[i]||
           *shared_.apertures424[i]!=apertures_[i].get())
            return fail(d,Status::runtimePending,0x1D76D8);
        const auto& a=*apertures_[i];
        if(a.type0C!=1||a.count04!=4||!a.vertices00)
            return fail(d,Status::runtimePending,0x1D76E0);
        for(std::size_t v=0;v<4;++v){
            const auto& vertex=a.vertices00[v];
            if(vertex.aliases10>2)return fail(d,Status::runtimePending,0x1D770C);
            for(std::size_t alias=0;alias<vertex.aliases10;++alias)
                if(!vertex.stq14[alias])return fail(d,Status::runtimePending,0x1D770C);
        }
    }
    drawPending_=true;drawSourcePC_=0x1D76FC;
    // Source TL,BL,TR,BR; source aliases+14 point to POSITION words despite
    // the historical native member name. Z/W and RGBA/STQ are not overwritten.
    for(std::size_t i=0;i<2;++i){
        auto& a=*apertures_[i];
        const float top=i?static_cast<float>(*shared_.bottom28C):0.f;
        const float bottom=i?448.f:static_cast<float>(*shared_.top284);
        const float x[4]{0.f,0.f,640.f,640.f};
        const float y[4]{top,bottom,top,bottom};
        for(std::size_t v=0;v<4;++v){
            auto& vertex=a.vertices00[v];vertex.authored00[0]=x[v];vertex.authored00[1]=y[v];
            for(std::size_t alias=0;alias<vertex.aliases10;++alias)
                std::memcpy(vertex.stq14[alias]->words.data(),vertex.authored00.data(),16);
        }
        d.sourceEffectsCommitted=true;
    }
    // Both arrays are updated BEFORE the original two submissions.
    sharedApertureOrdinal_=0;
    for(std::size_t i=0;i<2;++i){
        apertures_[i]->dirty3C=1;drawSourcePC_=0x1D798C;
        if(!drawAperture2E21D0(draw,i,d))return false;
        ++sharedApertureOrdinal_;
    }
    if(shared_.field420==0){
        constexpr std::uint32_t offsets[4]{0x2A0,0x300,0x360,0x3C0};
        constexpr std::uint32_t pcs[4]{0x1D7A2C,0x1D7A44,0x1D7A5C,0x1D7A74};
        for(std::size_t i=0;i<4;++i){
            if(!(shared_.field42C&(1u<<i)))continue;
            Quad* q=nullptr;
            for(std::size_t j=0;j<quadCount_;++j)if(quads_[j].offset==offsets[i]){q=&quads_[j];break;}
            drawSourcePC_=pcs[i];
            if(!q||!q->canMutatePacket2DF550()||draw.activePageQuad_||draw.activePageCostText_)
                return fail(d,Status::runtimePending,pcs[i]);
            draw.activePageQuad_=q;
            if(!q->draw2DF550(draw,d.quad))return fail(d,Status::runtimePending,d.quad.sourcePC);
            draw.activePageQuad_=nullptr;
        }
    }
    drawSourcePC_=0x1D79BC;std::uint32_t pendingPC=0;
    if(!draw.drawPageChildren141D70(*this,pendingPC))
        return fail(d,Status::runtimePending,pendingPC?pendingPC:0x1D79BC);
    drawPending_=false;drawSourcePC_=0x1D79D4;d.sourcePC=drawSourcePC_;return true;
}
}
