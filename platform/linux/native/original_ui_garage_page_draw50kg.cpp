#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_garage_state50kg.h"
#include "../../../src/game/original_ui_text_position50kg.h"
#include <cmath>
#include <string>
#include <cstdio>
#include <limits>
#include "../../../tests/native/original_ui_main_entry50kg.h"
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::drawReceiverCurrent(
    const OriginalUiGarageDrawContribution50KG& draw,const Quad* q)const noexcept {
    if(!q||!drawPending_||drawPublication_!=draw.publication_||draw.page_.get()!=this||
       draw.callback_!=&entity_||draw.phase_!=decltype(draw.phase_)::thirdPass||
       !draw.identityCurrent())return false;
    for(std::size_t i=0;i<quadCount_;++i)if(q==&quads_[i])return true;
    return false;
}
bool OriginalUiGaragePageOwner50KG::drawReceiverCurrent(
    const OriginalUiGarageDrawContribution50KG& draw,const Text* text)const noexcept {
    const bool receiver=text&&(text==costText3340_.value||
        (text==sharedText440_.value&&drawSourcePC_==0x1D75A4));
    return receiver&&drawPending_&&drawPublication_==draw.publication_&&
        draw.page_.get()==this&&draw.callback_==&entity_&&
        draw.phase_==decltype(draw.phase_)::thirdPass&&draw.identityCurrent();
}
bool OriginalUiGaragePageOwner50KG::draw1FBE10(
    OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
    d={};
    if(!draw.identityCurrent()||draw.page_.get()!=this||draw.callback_!=&entity_||
       draw.phase_!=decltype(draw.phase_)::thirdPass||!sourceEntryReturned1FAE00()||
       !entryProfileCurrent()||!entryProfile_||!updateReturned_||updateFailed_||
       updatePublication_!=draw.publication_||drawPending_||drawPublication_==draw.publication_||
       !std::isfinite(panelOffset4D8_)||field4E4_>=6)
        return fail(d,Status::runtimePending,0x1FBE10);
    OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot profile;
    if(!entryProfile_->owner->selectedGarageProfileSnapshot(profile))
        return fail(d,Status::entryProfilePending,0x1FC014);
    std::array<std::int32_t,6> maximum{},base{};
    for(std::size_t row=0;row<6;++row){
        if(!current4E8_[row]||!delta500_[row]||*delta500_[row]!=0||
           !game::convertOriginalUiGarageStat50KG(profile.base0180[row],base[row])||
           !game::convertOriginalUiGarageStat50KG(profile.maximum0408[row],maximum[row])||
           *current4E8_[row]!=base[row]||base[row]<0||maximum[row]<base[row]||maximum[row]>20)
            return fail(d,Status::entryProfilePending,0x1FC2C4);
    }
    // Native bounded fresh-career envelope only. This does not change profile,
    // cash, unlocks or deltas, and does not manufacture nonzero-purchase paths.
    if(!available34B0_||*available34B0_!=0)return fail(d,Status::entryProfilePending,0x1FD864);
    auto quad=[&](std::uint32_t offset)->Quad*{
        for(std::size_t i=0;i<quadCount_;++i)if(quads_[i].offset==offset)return &quads_[i];
        return nullptr;
    };
    // Validate all retained receivers before the first source mutation. Actual
    // FIFO pins are not readiness flags and cannot be silently overwritten.
    for(const auto offset:{0x5E0u,0x520u,0x580u}){
        auto* q=quad(offset);if(!q||!q->canMutatePacket2DF550())return fail(d,Status::runtimePending,0x1FBE10);
    }
    for(std::size_t row=0;row<6;++row)for(std::size_t col=0;col<20;++col){
        auto* q=quad(0x640u+static_cast<std::uint32_t>(row*1920u+col*96u));
        if(!q||!q->canMutatePacket2DF550())return fail(d,Status::runtimePending,0x1FC080);
    }
    drawPublication_=draw.publication_;drawPending_=true;
    auto position=[&](Quad& q,float x,float y){
        if(q.position10[0]==x&&q.position10[1]==y)return;
        q.position10[0]=x;q.position10[1]=y;
        q.flags5CValue|=1u;q.flags5CKnownMask|=1u;
        // The rotation source word is at Quad+3C, not field40.
        if(q.uv30[3]!=0.f){q.flags5CValue|=2u;q.flags5CKnownMask|=2u;}
    };
    auto color=[&](Quad& q,std::uint32_t rgb){
        if(q.color44==rgb)return;
        const std::array<std::uint32_t,3> c{((rgb>>16)&255u)>>1,((rgb>>8)&255u)>>1,(rgb&255u)>>1};
        for(std::size_t vertex=0;vertex<4;++vertex)for(std::size_t lane=0;lane<3;++lane)
            q.resource58[4u+3u*vertex].words[lane]=c[lane];
        q.color44=rgb;
    };
    auto alpha=[&](Quad& q,std::uint16_t value){
        if(q.alpha48==value)return;
        const auto source=static_cast<std::int16_t>(value);
        const auto packed=static_cast<std::uint32_t>(source>>2);
        for(std::size_t vertex=0;vertex<4;++vertex)q.resource58[4u+3u*vertex].words[3]=packed;
        q.alpha48=value;
    };
    auto submit=[&](Quad& q,std::uint32_t pc){
        drawSourcePC_=pc;
        if(draw.activePageQuad_||draw.activePageCostText_)return fail(d,Status::runtimePending,pc);
        draw.activePageQuad_=&q;
        if(!q.draw2DF550(draw,d.quad))return fail(d,Status::runtimePending,d.quad.sourcePC);
        draw.activePageQuad_=nullptr;
        d.sourceEffectsCommitted=true;return true;
    };
    auto& background=*quad(0x5E0);position(background,0.f,251.f+panelOffset4D8_);
    if(!submit(background,0x1FBECC))return false;
    auto& left=*quad(0x520);position(left,374.f,243.f+panelOffset4D8_);
    if(!submit(left,0x1FBF6C))return false;
    auto& right=*quad(0x580);position(right,406.f,243.f+panelOffset4D8_);
    if(!submit(right,0x1FC00C))return false;
    for(std::size_t row=0;row<6;++row)for(std::size_t col=0;col<20;++col){
        auto& q=*quad(0x640u+static_cast<std::uint32_t>(row*1920u+col*96u));
        color(q,0x00333333u);alpha(q,200);
        volatile float rowDistance=18.f*static_cast<float>(row);
        volatile float y=271.f+rowDistance;y=y+panelOffset4D8_;
        volatile float colDistance=7.f*static_cast<float>(col);
        volatile float x=190.f+colDistance;position(q,x,y);
    }
    for(std::size_t row=0;row<6;++row)for(std::int32_t col=0;col<*current4E8_[row];++col){
        auto& q=*quad(0x640u+static_cast<std::uint32_t>(row*1920u+static_cast<std::size_t>(col)*96u));
        alpha(q,255);color(q,entryProfile_->primaryColor);
        if(row==field4E4_)color(q,0x00CCCCCCu); // Actual selected-row second write1FC428.
    }
    for(std::size_t row=0;row<6;++row)for(std::int32_t col=0;col<maximum[row];++col){
        auto& q=*quad(0x640u+static_cast<std::uint32_t>(row*1920u+static_cast<std::size_t>(col)*96u));
        if(!submit(q,0x1FCA98))return false;
    }
    constexpr std::array<std::uint32_t,6> labelCalls{0x1FCAF8,0x1FCB2C,0x1FCB60,0x1FCB94,0x1FCBC8,0x1FCBFC};
    for(std::size_t row=0;row<6;++row){
        volatile float authored=268.f+18.f*static_cast<float>(row);
        volatile float y=authored+panelOffset4D8_;std::int32_t integerY=0;
        drawSourcePC_=labelCalls[row];
        if(!game::roundOriginalUiTextMetric50KG(y,1,integerY)||!widgets_||
           !widgets_->setPageDrawPosition2AA1D0(*this,draw,16+row,170,integerY,d.widget))
            return fail(d,Status::runtimePending,d.widget.sourcePC?d.widget.sourcePC:drawSourcePC_);
    }
    for(std::size_t row=0;row<6;++row){
        drawSourcePC_=0x1FCC10u+static_cast<std::uint32_t>(row*20u);
        if(!widgets_->setPageDrawVisibility2A7A00(*this,draw,10+row,false,d.widget))
            return fail(d,Status::runtimePending,d.widget.sourcePC);
    }
    const auto localization=issuer_->sourceLocalization32COwner_;
    auto selectedText=[&](std::int32_t ordinal,std::string& out)->bool {
        if(!localization)return fail(d,Status::runtimePending,0x18AC60);
        std::shared_ptr<const assets::OriginalApplicationLocalization50KG::SelectedText> token;
        assets::OriginalApplicationLocalizationDiagnostic50KG ld;
        if(!issuer_->borrowLocalizationText18AC60(*localization,ordinal,token,ld))
            return fail(d,Status::runtimePending,ld.sourcePC);
        std::string_view text;
        if(!assets::OriginalApplicationLocalization50KG::SelectedText::BorrowReader::readCurrent(
                token,*issuer_,*localization,ordinal,text))return fail(d,Status::runtimePending,0x18AC60);
        out.assign(text);return true;
    };
    try {
        const auto stat=static_cast<std::size_t>(field4E4_);
        const auto upgrade=*current4E8_[stat]+*delta500_[stat]-base[stat];
        if(upgrade!=maximum[stat]&&upgrade<14){
            constexpr std::array<const char*,6> keys{
                "STR_GARAGE_PAGE_ACCELERATION_%d","STR_GARAGE_PAGE_TOP_SPEED_%d",
                "STR_GARAGE_PAGE_HANDLING_%d","STR_GARAGE_PAGE_DEFENSE_%d",
                "STR_GARAGE_PAGE_REPAIR_%d","STR_GARAGE_PAGE_COOLING_%d"};
            std::array<char,64> key{};const auto n=std::snprintf(key.data(),key.size(),keys[stat],upgrade+1);
            std::int32_t ordinal=0;
            if(n<0||static_cast<std::size_t>(n)>=key.size()||!localization||
               !localization->lookup20ED30(key.data(),ordinal))return fail(d,Status::runtimePending,0x20ED30);
            std::string label,description;
            if(!selectedText(211+static_cast<std::int32_t>(stat),label)||!selectedText(ordinal,description))return false;
            const auto combined=label+"@"+description; //actual3B0368 "%s@%s"
            std::int32_t y=0;volatile float actual=290.f+panelOffset4D8_;
            if(!game::roundOriginalUiTextMetric50KG(actual,1,y))return fail(d,Status::runtimePending,0x1FD7B8);
            drawSourcePC_=0x1FD7C0;
            if(!widgets_->setPageDrawPosition2AA1D0(*this,draw,10+stat,440,y,d.widget))return fail(d,Status::runtimePending,d.widget.sourcePC);
            drawSourcePC_=0x1FD7CC;
            if(!widgets_->setPageDrawText2A9FF0(*this,draw,10+stat,combined,d.widget))return fail(d,Status::runtimePending,d.widget.sourcePC);
            drawSourcePC_=0x1FD7D8;
            if(!widgets_->setPageDrawVisibility2A7A00(*this,draw,10+stat,true,d.widget))return fail(d,Status::runtimePending,d.widget.sourcePC);
        }
        std::string cashLabel;if(!selectedText(208,cashLabel))return false;
        const auto cashText=cashLabel+" "+std::to_string(*available34B0_); //actual3AF208 "%s %d"
        std::int32_t y=0;volatile float actual=245.f+panelOffset4D8_;
        if(!game::roundOriginalUiTextMetric50KG(actual,1,y))return fail(d,Status::runtimePending,0x1FD894);
        drawSourcePC_=0x1FD8A0;
        if(!widgets_->setPageDrawPosition2AA1D0(*this,draw,7,385,y,d.widget))return fail(d,Status::runtimePending,d.widget.sourcePC);
        drawSourcePC_=0x1FD8AC;
        if(!widgets_->setPageDrawText2A9FF0(*this,draw,7,cashText,d.widget))return fail(d,Status::runtimePending,d.widget.sourcePC);
    }catch(...){return fail(d,Status::allocation,drawSourcePC_);}
    try {
        const auto stat=static_cast<std::size_t>(field4E4_);
        const auto total=*current4E8_[stat]+*delta500_[stat];std::string cost;
        if(total==maximum[stat]){if(!selectedText(210,cost))return false;}
        else {
            const auto amount=std::int64_t(pairFirst_[stat])*(std::int64_t(total)+1)+pairSecond_[stat];
            if(amount<std::numeric_limits<std::int32_t>::min()||amount>std::numeric_limits<std::int32_t>::max())
                return fail(d,Status::runtimePending,0x1FD924);
            std::string unit;if(!selectedText(209,unit))return false;
            cost=std::to_string(static_cast<std::int32_t>(amount))+" "+unit; //3AF210 "%d %s"
        }
        if(!costText3340_.value||!costText3340_.value->coldInputsCurrent()||costText3340_.value->nativeGarageFifoPins_)
            return fail(d,Status::runtimePending,0x1FDA50);
        auto& text=*costText3340_.value;
        if(!text.writeText29D990(cost,d.text)){
            std::fprintf(stderr,"original_garage_text_setter_route route=page_cost owner_offset=13120 publication=%llu input_bytes=%zu payload_status=%u payload_pc=%u\n",
                static_cast<unsigned long long>(draw.publication_),cost.size(),static_cast<unsigned>(d.text.status),static_cast<unsigned>(d.text.sourcePC));
            return fail(d,Status::runtimePending,d.text.sourcePC);
        }
        volatile float distance=18.f*static_cast<float>(field4E4_);
        volatile float rowY=267.f+distance;
        volatile float y=rowY+panelOffset4D8_;
        if(text.vector10_[0]!=365.f||text.vector10_[1]!=y){
            text.vector10_[0]=365.f;text.vector10_[1]=y;text.flags5A_|=3u;
        }
        drawSourcePC_=0x1FDB88;
        if(draw.activePageCostText_||draw.activePageQuad_)return fail(d,Status::runtimePending,drawSourcePC_);
        draw.activePageCostText_=&text;
        if(!text.draw2E0170(draw,d.text))return fail(d,Status::runtimePending,d.text.sourcePC);
        draw.activePageCostText_=nullptr;
    }catch(...){return fail(d,Status::allocation,drawSourcePC_);}
    auto* decrement=quad(0x33F0);auto* increment=quad(0x3450);
    if(!decrement||!increment||!decrement->canMutatePacket2DF550()||!increment->canMutatePacket2DF550())
        return fail(d,Status::runtimePending,0x1FDB90);
    volatile float distance=18.f*static_cast<float>(field4E4_);
    volatile float rowY=270.f+distance;
    volatile float y=panelOffset4D8_+rowY;
    volatile float leftBase=191.f-decrement->uv30[1];
    volatile float leftX=-3.f+leftBase;
    position(*decrement,leftX,y);
    if(!submit(*decrement,0x1FDC5C))return false;
    volatile float rightBase=133.f+191.f;
    volatile float rightX=rightBase+5.f;
    position(*increment,rightX,y);
    if(!submit(*increment,0x1FDD24))return false;
    drawPending_=false;drawSourcePC_=0x1FDD54;d.sourcePC=drawSourcePC_;return true;
}
}
