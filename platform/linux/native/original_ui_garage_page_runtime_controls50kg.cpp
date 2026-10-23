#include "original_ui_garage_page_owner50kg.h"
#include <limits>
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::runtimeWidgetCallCurrent(
    std::size_t row,std::uint32_t operation,std::int32_t argument)const noexcept {
    if(!runtimeWidgetPending_||!runtimeWidgetSerial_||!runtimeWidgetTarget_||
       operation!=runtimeWidgetOperation_||argument!=runtimeWidgetArgument_||
       !runtimeWidgetSourcePC_||entryPhase_!=EntryPhase::returned||
       !managerAttached1D1C40_||!grant_||!grant_->current()||!entryProfileCurrent()||
       !grant_->garageAttachedPageCurrent(*this,entity_)||!widgets_||
       row>=widgets_->rows_.size()||!widgets_->managerTable154_||!widgets_->slotsKnown_[row])return false;
    return widgets_->managerTable154_[row]==runtimeWidgetTarget_;
}
bool OriginalUiGaragePageOwner50KG::runtimeWidgetCall(C* target,
    std::uint32_t operation,std::int32_t argument,std::uint32_t sourcePC,Diagnostic& d)noexcept {
    if(runtimeWidgetPending_||!sourceEntryReturned1FAE00()||!target||!widgets_||
       !widgets_->managerTable154_||runtimeWidgetSerial_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::runtimePending,sourcePC);
    std::size_t row=0;
    for(;row<widgets_->rows_.size();++row)
        if(widgets_->slotsKnown_[row]&&widgets_->managerTable154_[row]==target)break;
    if(row==widgets_->rows_.size())return fail(d,Status::runtimePending,sourcePC);
    ++runtimeWidgetSerial_;runtimeWidgetTarget_=target;runtimeWidgetOperation_=operation;
    runtimeWidgetArgument_=argument;runtimeWidgetSourcePC_=sourcePC;runtimeWidgetPending_=true;
    if(!widgets_->applyPageEntryWidgetCall(*this,row,operation,argument,d.widget))
        return fail(d,Status::runtimePending,d.widget.sourcePC);
    runtimeWidgetPending_=false;runtimeWidgetTarget_=nullptr;d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePageOwner50KG::runtimeControlCall(PageControl* control,
    std::uint32_t operation,std::int32_t argument,Diagnostic& d)noexcept {
    bool owned=false;
    for(std::size_t i=0;i<controls_.size();++i)
        if(controls_[i].get()==control&&controlSlotsKnown_[i]&&controlTable_&&controlTable_[i]==control)owned=true;
    if(!owned||!control||!control->loaded||control->retirementStarted||control->payloadRetired||
       control->count34<0||control->widgetsKnown!=static_cast<std::size_t>(control->count34)||
       (control->widgetsKnown&&!control->widgets38))return fail(d,Status::runtimePending,0x2A5A00);
    if(operation==0x2A7A00)control->sectionPresent30=argument;
    const auto pc=operation==0x2A7A00?0x2A5BA4u:operation==0x78?0x2A5A38u:operation==0x88?0x2A5B28u:0x2A5AB0u;
    for(std::size_t i=0;i<control->widgetsKnown;++i){auto* widget=control->widgets38[i];
        if(!widget&&(operation==0x7C||operation==0x2A7A00))continue;
        if(!runtimeWidgetCall(widget,operation,argument,pc,d))return false;
    }
    return true;
}
bool OriginalUiGaragePageOwner50KG::runtimeGroupEnable(std::size_t index,bool enabled,Diagnostic& d)noexcept {
    if(index>=groups_.size()||!groups_[index]||!groupSlotsKnown_[index]||!groupTable_||
       groupTable_[index]!=groups_[index].get()||!sourceEntryReturned1FAE00()||runtimeWidgetPending_)
        return fail(d,Status::runtimePending,0x2A5000);
    auto& g=*groups_[index];
    if(!g.loaded||g.retirementStarted||g.payloadRetired||
       (g.controlsKnown&&!g.controls38)||(g.associatedKnown&&!g.associated48))
        return fail(d,Status::runtimePending,0x2A501C);
    g.enabled30=enabled?1:0;
    for(std::size_t pass=0;pass<2;++pass){
        const auto count=pass?g.associatedKnown:g.controlsKnown;
        auto* table=pass?g.associated48.get():g.controls38.get();
        for(std::size_t i=0;i<count;++i)
            if(table[i]&&!runtimeControlCall(table[i],0x2A7A00,enabled?1:0,d))return false;
    }
    d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePageOwner50KG::runtimeGroupSelect2A4F50(
    std::size_t index,std::int32_t selection,Diagnostic& d)noexcept {
    if(index>=groups_.size()||!groups_[index]||!groupSlotsKnown_[index]||!groupTable_||
       groupTable_[index]!=groups_[index].get()||!sourceEntryReturned1FAE00()||runtimeWidgetPending_)
        return fail(d,Status::runtimePending,0x2A4F50);
    auto& g=*groups_[index];if(!g.loaded||g.retirementStarted||g.payloadRetired)
        return fail(d,Status::runtimePending,0x2A4F50);
    if(!g.count34)return true;
    if(selection<1||selection>g.count34||g.controlsKnown!=static_cast<std::size_t>(g.count34)||!g.controls38)
        return fail(d,Status::runtimePending,0x2A4F70);
    g.selection3C=selection;g.selected40=g.controls38[static_cast<std::size_t>(selection-1)];
    for(std::size_t i=0;i<g.controlsKnown;++i)
        if(g.controls38[i]&&!runtimeControlCall(g.controls38[i],0x7C,0,d))return false;
    if(g.selected40&&!runtimeControlCall(g.selected40,0x78,0,d))return false;
    d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePageOwner50KG::runtimeSetActiveGroup2A6BE0(
    std::size_t index,std::int32_t selection,std::size_t channel,Diagnostic& d)noexcept {
    if(!sourceEntryReturned1FAE00()||runtimeWidgetPending_||index>=groups_.size()||
       channel>=extraHeads_.size()||base_.groupCount148>=groups_.size()||!groupTable_||!extraTable_)
        return fail(d,Status::runtimePending,0x2A6BE0);
    for(std::size_t i=0;i<extraHeads_.size();++i)
        if(!extraHeads_[i]||extraTable_[i]!=extraHeads_[i].get()||!extraHeads_[i]->first0||!extraHeads_[i]->last4)
            return fail(d,Status::runtimePending,0x2A6C28);
    for(std::size_t i=1;i<=base_.groupCount148;++i){
        bool deactivate=true;
        for(const auto& head:extraHeads_)if(*head->first0==static_cast<std::int32_t>(i))deactivate=false;
        if(!deactivate)continue;
        if(!groups_[i]||!groupSlotsKnown_[i]||groupTable_[i]!=groups_[i].get())
            return fail(d,Status::runtimePending,0x2A6C68);
        auto& g=*groups_[i];
        if(!g.loaded||g.retirementStarted||g.payloadRetired||
           (g.controlsKnown&&!g.controls38)||(g.associatedKnown&&!g.associated48))
            return fail(d,Status::runtimePending,0x2A4E40);
        for(std::size_t j=0;j<g.associatedKnown;++j)
            if(g.associated48[j]&&!runtimeControlCall(g.associated48[j],0x7C,0,d))return false;
        for(std::size_t j=0;j<g.controlsKnown;++j)
            if(g.controls38[j]&&!runtimeControlCall(g.controls38[j],g.keepEnabled4C?0x88:0x7C,0,d))return false;
    }
    if(!groups_[index]||groupTable_[index]!=groups_[index].get())return fail(d,Status::runtimePending,0x2A6C94);
    auto& selected=*groups_[index];
    if(!selected.loaded||selected.retirementStarted||selected.payloadRetired||
       (selected.associatedKnown&&!selected.associated48))return fail(d,Status::runtimePending,0x2A4DD0);
    for(std::size_t i=0;i<selected.associatedKnown;++i)
        if(!runtimeControlCall(selected.associated48[i],0x78,0,d))return false;
    if(!runtimeGroupSelect2A4F50(index,selection,d))return false;
    //2A6CC8 stores the actual PageGroup pointer. It is not a guest integer
    //address or a fake list link; the scalar selector is stored afterward.
    extraHeads_[channel]->last4=&selected;
    extraHeads_[channel]->first0=static_cast<std::int32_t>(index);
    d.sourceEffectsCommitted=true;d.sourcePC=0x2A6CD8;return true;
}
}
