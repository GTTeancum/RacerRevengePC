#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../src/game/original_camera_start_native.h"
#include "original_ui_garage_page_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <cstring>
#include <exception>
#include <cstdio>
#include <cmath>
#include <limits>
#include "../../../src/game/original_ui_text_position50kg.h"
#include "../../../src/assets/pod_physics49n.h"
namespace rr::platform {
namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
bool logWidgetRetirementFailure(const char* branch,const OriginalUiGarageWidget1Diagnostic50KG& d,
 std::size_t row,std::size_t line,unsigned route,bool attached,bool retiring,bool retired,
 std::size_t retiredRows,std::size_t retiredLines)noexcept {
 std::fprintf(stderr,"original_garage_widget_retirement_rejected branch=%s route=%u row=%zu line=%zu status=%u pc=%u attached=%u retiring=%u retired=%u retired_rows=%zu retired_lines=%zu text_status=%u text_pc=%u payload_status=%u payload_pc=%u movement_status=%u movement_pc=%u watto_status=%u watto_pc=%u\n",
  branch,route,row,line,static_cast<unsigned>(d.status),d.sourcePC,unsigned(attached),unsigned(retiring),unsigned(retired),
  retiredRows,retiredLines,static_cast<unsigned>(d.text.status),d.text.sourcePC,
  static_cast<unsigned>(d.text.payload.status),d.text.payload.sourcePC,
  static_cast<unsigned>(d.movement.status),d.movement.sourcePC,static_cast<unsigned>(d.watto.status),d.watto.sourcePC);
 return false;
}
struct WidgetSpec {
 const char* section;const char* font;const char* material;const char* id;
 std::int32_t ordinal,x,y,justification;float leading;std::uint32_t rgb;
 std::array<std::int32_t,4> capacities;std::size_t lines;bool dynamic;
 bool highlight=false;
};
const WidgetSpec* textSpec(std::size_t row)noexcept {
 static const WidgetSpec specs[]{
 {"Widget 1","denmrk16","denmrk16Top","STR_ACCEPT",9,516,385,0,1,204,{6},1,false},
 {"Widget 2","denmrk16","denmrk16Top","STR_CANCEL",10,412,385,0,1,204,{6},1,false},
 {"Widget 3","venus16","venus16Top","STR_GARAGE_PAGE_TITLE",206,30,386,0,1,204,{14},1,false},
 {"Widget 7","denmrk16","denmrk16Top","STR_GARAGE_PAGE_APPLY_TRUGUTS",207,320,30,3,.8f,104,{21,36},2,false},
 {"Widget 8","denmrk16","denmrk16",nullptr,-1,20,357,0,.8f,204,{40,40},2,true},
 {"Widget 11","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 12","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 13","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 14","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 15","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 16","denmrk16","denmrk16",nullptr,-1,320,30,0,.8f,102,{40,40,40,40},4,true},
 {"Widget 17","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_ACCELERATION",200,360,270,4,1,104,{12},1,false,true},
 {"Widget 18","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_TOP_SPEED",201,360,288,4,1,104,{9},1,false,true},
 {"Widget 19","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_HANDLING",202,360,306,4,1,104,{8},1,false,true},
 {"Widget 20","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_DEFENSE",203,360,324,4,1,104,{7},1,false,true},
 {"Widget 21","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_REPAIR",204,360,342,4,1,104,{6},1,false,true},
 {"Widget 22","denmrk14","denmrk14","STR_RIDER_SELECTION_PAGE_COOLING",205,360,360,4,1,104,{7},1,false,true}};
 return row<3?&specs[row]:row==6?&specs[3]:row==7?&specs[4]:row>=10&&row<22?&specs[row-5]:nullptr;
}
bool emptyRow(std::size_t row)noexcept{return row==3||row==4||row==8;}
const char* rowSection(std::size_t row)noexcept {
 static constexpr const char* names[]{"Widget 1","Widget 2","Widget 3","Widget 4","Widget 5",
 "Widget 6","Widget 7","Widget 8","Widget 9","Widget 10","Widget 11","Widget 12","Widget 13",
 "Widget 14","Widget 15","Widget 16","Widget 17","Widget 18","Widget 19","Widget 20","Widget 21","Widget 22"};
 return row<22?names[row]:nullptr;
}
void firstUse(std::atomic<std::uint8_t>& guard,std::atomic<std::int32_t>& assertion)noexcept {
    if(!guard.load(std::memory_order_acquire)){
        assertion.store(1,std::memory_order_relaxed);
        guard.store(1,std::memory_order_release);
    }
}
}
// Actual process records. Native atomics do not claim original synchronization.
std::atomic<std::uint8_t>& OriginalUiGarageWidget1Owner50KG::hashGuard8098()noexcept {return OriginalUiTextHashGuards50KG::hashGuard8098();}
std::atomic<std::int32_t>& OriginalUiGarageWidget1Owner50KG::hashAssertion3D6B10()noexcept {return OriginalUiTextHashGuards50KG::hashAssertion3D6B10();}
std::atomic<std::uint8_t>& OriginalUiGarageWidget1Owner50KG::idGuard809C()noexcept {return OriginalUiTextHashGuards50KG::idGuard809C();}
std::atomic<std::int32_t>& OriginalUiGarageWidget1Owner50KG::idAssertion3D6B18()noexcept {return OriginalUiTextHashGuards50KG::idAssertion3D6B18();}
OriginalUiGarageWidget1Owner50KG::~OriginalUiGarageWidget1Owner50KG()noexcept {
    if((bound_&&!tableReleased_)||(grant_&&grant_->stillBorrowingGarageWidget1Owner(this)))std::terminate();
}
bool OriginalUiGarageWidget1Owner50KG::authorityThread(const std::shared_ptr<Authority>& authority)noexcept {
    return authority&&authority->nativeOwnerThread_==std::this_thread::get_id();
}
bool OriginalUiGarageWidget1Owner50KG::threadCurrent()const noexcept {
    return ownerThread_==std::this_thread::get_id()&&
        (!authority_||authority_->nativeOwnerThread_==ownerThread_);
}
bool OriginalUiGarageWidget1Owner50KG::identityCurrent()const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(!threadCurrent())return false;
    return bound_&&!detachedReleased_&&authority_&&authority_->constructed_&&!authority_->released_&&
        grant_&&grant_->current()&&grant_->stillBorrowingGarageWidget1Owner(this)&&
        issuer_==grant_->issuer()&&sameOwner(authority_,grant_->application())&&
        fog_&&fog_->widgetCustodyCurrentFor(this)&&sameOwner(grant_,fog_->grant_)&&issuer_==fog_->issuer_;
}
bool OriginalUiGarageWidget1Owner50KG::rowFieldsCurrent(std::size_t index)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(index>=rows_.size())return false;const auto& r=rows_[index];
    if(!r.reached)return !r.outer&&!r.movement&&!r.watto&&!r.wattoEntity&&!r.constructed&&!r.stored&&!r.empty;
    if(index==9){
        if(r.outer||r.movement||r.empty||r.array60||r.arrayPublished)return false;
        if(r.entityRetired)return !r.watto;
        if(!r.watto||!r.wattoEntity||!r.constructed)return false;
        if(r.retiring){
            std::shared_ptr<Grant> custody;const C* entity=nullptr;bool closed=false;
            return r.watto->retirementCustody(custody,entity,closed)&&sameOwner(custody,grant_)&&
                entity==r.wattoEntity&&r.sourceOuterIdentity==reinterpret_cast<std::uintptr_t>(entity);
        }
        Watto::ColdPublication publication;
        return (runtimePage_?r.watto->managerRowPublication(publication):r.watto->constructorPublication(publication))&&sameOwner(publication.grant,grant_)&&
            publication.widget==r.wattoEntity&&r.sourceOuterIdentity==reinterpret_cast<std::uintptr_t>(publication.widget)&&
            grant_->garageWidgetRowIdentityCurrent(this,index,*publication.widget)&&
            (publication.widget->flags29&3u)==(r.hidden?0u:3u);
    }
    if(!r.constructed&&!r.outer&&!r.movement&&!r.stored&&!r.empty)return !r.array60&&!r.arrayPublished;
    if(r.empty)return emptyRow(index)&&r.stored&&slotsKnown_[index]&&!r.outer&&!r.movement&&!r.constructed;
    if(index==5){
        if(r.entityRetired)return !r.movement||r.movement->phase_==Movement::Phase::retired;
        if(!r.movement)return !r.constructed;
        if(!r.constructed)return !r.movement->constructed_&&r.movement->phase_==Movement::Phase::empty;
        return r.movement->fieldsCurrent()&&r.constructed==r.movement->constructed_;
    }
    const auto* spec=textSpec(index);if(!spec||r.movement)return false;
    if(!r.constructed)return !r.array60&&!r.arrayPublished;
    if(!r.outer)return r.entityRetired;
    const auto& w=*r.outer;const auto& c=w.entity;
    if(c.kind!=game::OriginalWreckComponentKind50LA::uiOverlayText2A8F00||
       c.sessionGeneration!=grant_->generation_||c.objectKey!=reinterpret_cast<std::uintptr_t>(&c)||
       c.reference00!=((r.retiring||r.entityRetired)?0u:1u)||c.category28!=0xff||
       (c.flags29&15u)!=(r.hidden?0u:3u)||w.manager30!=grant_->manager()||w.field64||w.field38!=r.selected38||
       w.field3C||w.field40||w.field44||w.field48||w.field4C!=r.runtimeDisabled4C||w.field50||w.field54||w.field58||
       w.dynamic178!=(r.dynamicLengthKnown?1:0))return false;
    if(r.entryColorStored){
        if(!runtimePage_||!coldDatReleased_||r.entryColorLines>spec->lines||
           w.color17C!=r.entryColor||w.field1A4!=(r.entryColorReturned?1u:0u)||
           (r.entryColorReturned&&r.entryColorLines!=spec->lines))return false;
    }else if(r.entryColorLines||r.entryColorReturned||w.field1A4)return false;
    if(r.attached){
        if(c.parent1C!=grant_->manager()||c.mask20!=0xffffffffu||!c.previous10)return false;
        if(r.retiring||r.entityRetired){if(c.previous10!=r.previousAtRetire||c.next14)return false;}
        else if(c.previous10->next14!=&c||c.previous10->parent1C!=c.parent1C||
            (c.next14&&(c.next14->previous10!=&c||c.next14->parent1C!=c.parent1C)))return false;
    }else if(c.parent1C||c.previous10||c.next14||c.mask20)return false;
    const C* first=nullptr;const C* previous=nullptr;bool hadAttached=false;
    for(std::size_t i=0;i<r.lines.size();++i){const auto& line=r.lines[i];
        if(line.attached)hadAttached=true;
        if(line.constructed&&!line.retired){
            if(!line.owner||line.sourceIdentity!=reinterpret_cast<std::uintptr_t>(&line.owner->entity_)||
               !line.owner->constructed_||!line.slotKnown||i>=r.arrayCapacity)return false;
            if(line.configured&&!line.owner->retainedColorTargetCurrent(
                line.runtimeColorKnown?line.runtimeColor:(0xff000000u|spec->rgb*0x010101u)))return false;
            if(line.configured&&!line.owner->retainedSetupStateCurrent(line.setupEnabled))return false;
            if((line.runtimeColorKnown||line.setupTouched)&&!runtimePage_)return false;
            const auto& child=line.owner->entity_;
            if(child.category28!=0xff||child.descendants24||(child.flags29&15u)!=(r.hidden?0u:3u))return false;
            if(line.attached){
                if(child.parent1C!=&c||child.mask20!=0xffffffffu||child.previous10!=previous)return false;
                if(previous&&previous->next14!=&child)return false;
                if(!first)first=&child;previous=&child;
            }else if(child.parent1C||child.previous10||child.next14||child.mask20)return false;
        }
        if(line.slotKnown&&!r.arrayReleased&&(!r.array60||i>=r.arrayCapacity||
            r.array60[i]!=reinterpret_cast<C*>(line.sourceIdentity)))return false;
    }
    if(previous&&previous->next14)return false;
    if(c.firstChild18!=first||c.descendants24!=(hadAttached?0xffffffffu:0u))return false;
    if(r.countReset&&(w.lineCount168<1||w.lineCount168>static_cast<std::int32_t>(spec->lines)))return false;
    if(r.arrayPublished){
        if(w.lines60!=reinterpret_cast<void*>(r.source60Identity)||!r.source60Identity||
           (!r.arrayReleased&&(!r.array60||r.array60.get()!=w.lines60||!r.arrayCapacity)))return false;
    }else if(w.lines60||r.array60)return false;
    if(r.fontKnown&&std::memcmp(w.font68.data(),spec->font,std::strlen(spec->font)+1))return false;
    if(r.materialKnown&&std::memcmp(w.materialE8.data(),spec->material,std::strlen(spec->material)+1))return false;
    if(r.dynamicLengthKnown&&(!spec->dynamic||w.dynamicLength16C!=40))return false;
    if(spec->dynamic&&w.lineCount168!=(r.dynamicCountKnown?static_cast<std::int32_t>(spec->lines):0))return false;
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::drawTextChild29D3F0(const C& child,
 OriginalUiGarageDrawContribution50KG& draw,D& d)noexcept {
 d={};
 if(!runtimePage_||!pageEntryCustodyCurrent(*runtimePage_)||!grant_||!grant_->current()||
    !child.parent1C)return fail(d,S::source,0x29D3F0);
 TextOverlay* retained=nullptr;
 for(auto& row:rows_)if(row.outer && &row.outer->entity==child.parent1C){
  for(auto& line:row.lines)if(line.owner && &line.owner->entity_==&child){
   if(retained)return fail(d,S::source,0x29D3F0);
   retained=line.owner.get();
  }
 }
 if(!retained||draw.activeText_)return fail(d,S::source,0x29D3F0);
 draw.activeText_=retained;
 if(!retained->draw29D3F0(draw,d.text)){
  // Retain failed callback custody. Never silently retry an earlier primitive.
  d.status=S::textOverlay;d.sourcePC=d.text.sourcePC;return false;
 }
 draw.activeText_=nullptr;return true;
}
bool OriginalUiGarageWidget1Owner50KG::updateTextOuter141ED0(const C& outer,
 const ::original_ui_source_timing50LH::PublicationView& timing,std::uint64_t serial,
 OriginalUiGarageRendererLease50KG& renderer,D& d)noexcept {
 d={};auto reject=[&](std::uint32_t pc)noexcept {
  updateFailed_=true;updateFailurePC_=pc;d.status=S::source;d.sourcePC=pc;return false;
 };
 if(updateFailed_){d.status=S::source;d.sourcePC=updateFailurePC_;return false;}
 ::original_ui_source_timing50LH::Sample sample;std::uint64_t actual=0;
 if(!timing.readPrepared(sample,actual)||actual!=serial||!serial||!runtimePage_||
    !renderer.currentFor(issuer_)||!pageEntryCustodyCurrent(*runtimePage_)||!grant_||
    outer.parent1C!=grant_->manager()||(outer.flags29&0x0du)!=1u||!(outer.mask20&2u))
  return reject(0x141ED0);
 RowRecord* row=nullptr;std::size_t ordinal=0;
 for(std::size_t i=0;i<rows_.size();++i)if(rows_[i].outer && &rows_[i].outer->entity==&outer){row=&rows_[i];ordinal=i;break;}
 if(!row||!textSpec(ordinal)||row->hidden)return reject(0x141F60);
 if(updateOuter_==&outer&&updateSerial_==serial){
  // A returned source callback is not a replayable success wrapper.
  if(updateReturned_)return reject(0x141ED0);
 }else{
  if(updateOuter_&&!updateReturned_)return reject(0x141ED0);
  updateOuter_=&outer;updateSerial_=serial;updateReturned_=false;updateChild_=outer.firstChild18;
 }
 if(!(outer.descendants24&2u)){updateReturned_=true;return true;}
 while(updateChild_){
  const C* child=updateChild_;
  if(child->parent1C!=&outer)return reject(0x141F00);
  if((child->flags29&0x0du)==1u&&(child->mask20&2u)){
   TextOverlay* text=nullptr;
   for(auto& line:row->lines)if(line.owner && &line.owner->entity_==child){text=line.owner.get();break;}
   if(!text)return reject(0x141F60);
   VehicleWindowDiagnostic50DS rd;game::OriginalCameraStartResult result;
   if(!renderer.serviceEntity141F3C(rd,result))return reject(result.sourcePC);
   if(!text->update29CDA0(renderer,sample.updateSeconds,serial,d.text)){
    d.status=S::source;d.sourcePC=d.text.sourcePC;updateFailed_=true;updateFailurePC_=d.sourcePC;return false;
   }
  }
  // Source141F68 reads next14 only after the actual callback returned.
  updateChild_=child->next14;
 }
 updateReturned_=true;d.sourcePC=0x141F80;return true;
}
bool OriginalUiGarageWidget1Owner50KG::fieldsCurrent()const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(cursor_>rows_.size()||(coldDatReleased_&&cursor_!=rows_.size()))return false;
    if(countKnown_&&count158_!=22)return false;
    if(tableKnown_&&(!countKnown_||(!tableReleased_&&(!managerTable154_||source154Identity_!=
        reinterpret_cast<std::uintptr_t>(managerTable154_.get())))))return false;
    for(std::size_t i=0;i<rows_.size();++i){const auto& r=rows_[i];
        if(slotsKnown_[i]!=r.stored||!rowFieldsCurrent(i))return false;
        // Hidden is a constructor fact, not a lifetime invariant after the real
        // Page90/entry visibility calls. rowFieldsCurrent still authenticates
        // each retained hidden witness against the actual subtree flags.
        if(r.stored&&(!tableKnown_||(!runtimePage_&&!r.empty&&!r.hidden)))return false;
        if(r.stored&&!tableReleased_&&managerTable154_[i]!=reinterpret_cast<C*>(r.sourceOuterIdentity))return false;
    }
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::rowPendingCurrent(std::size_t index)const noexcept {
    if(!threadCurrent()||index>=rows_.size()||index!=cursor_||retirementStarted_||!identityCurrent()||!fieldsCurrent())return false;
    const auto* spec=textSpec(index);const auto& r=rows_[index];
    if(!spec||spec->dynamic||r.stage!=Stage::selectedTextPending||!dictionary_||!dictionary_->current()||
       !r.constructed||r.retiring||r.entityRetired||!r.fontKnown||!r.materialKnown||!r.xKnown||!r.yKnown||
       !r.leadingKnown||!r.justificationKnown||!r.typeKnown||!r.lookupReached||!r.hashAssertionReached||
       !r.idAssertionReached||r.lookupOrdinal!=spec->ordinal)return false;
    const auto& w=*r.outer;
    return w.type34==4&&w.lineCount168==1&&w.justify170==spec->justification&&w.leading174==spec->leading&&
        w.x180==spec->x&&w.y184==spec->y&&!w.height188&&w.alpha18C==255&&w.depth190==1&&
        w.color17C==(0xff000000u|spec->rgb*0x010101u)&&w.highlight194==static_cast<std::uint32_t>(spec->highlight)&&
        w.highlight198==(spec->highlight?0xffccccccu:0xffffffffu)&&
        !w.disabled19C&&w.disabled1A0==0xff808080u;
}
bool OriginalUiGarageWidget1Owner50KG::currentPending()const noexcept{return rowPendingCurrent(0);}
bool OriginalUiGarageWidget1Owner50KG::rowColdCurrent(std::size_t index)const noexcept {
    return !runtimePage_&&rowCompleteCurrent(index,true);
}
bool OriginalUiGarageWidget1Owner50KG::rowCompleteCurrent(std::size_t index,bool authoredOnly)const noexcept {
    if(index>=rows_.size()||!identityCurrent()||!fieldsCurrent())return false;
    return rowCompleteAfterFieldsCurrent(index,authoredOnly);
}
bool OriginalUiGarageWidget1Owner50KG::rowCompleteAfterFieldsCurrent(std::size_t index,bool authoredOnly)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(index>=rows_.size())return false;const auto& r=rows_[index];
    if(r.empty)return r.stored;
    if(!r.constructed||r.retiring||r.entityRetired||!r.wantsKnown||(authoredOnly&&!r.hidden)||!r.stored||!r.published||!r.attached)return false;
    if(index==5)return r.movement&&r.movement->payloadCurrent();
    if(index==9)return r.stage==Stage::rowAttached&&r.watto&&r.wattoEntity;
    const auto* spec=textSpec(index);if(!spec||r.stage!=Stage::rowAttached||r.borrow||r.arrayReleased)return false;
    const auto& w=*r.outer;
    if(authoredOnly&&r.entryColorStored)return false;
    const auto expectedColor=r.entryColorStored?r.entryColor:(0xff000000u|spec->rgb*0x010101u);
    if(w.type34!=4||w.lineCount168!=static_cast<std::int32_t>(spec->lines)||w.justify170!=spec->justification||
       w.leading174!=spec->leading||w.x180!=(r.drawPositionKnown?r.drawX:spec->x)||
       w.y184!=(r.drawPositionKnown?r.drawY:spec->y)||(authoredOnly&&r.drawPositionKnown)||w.height188||
       w.alpha18C!=255||w.depth190!=1||w.color17C!=expectedColor||
       w.highlight194!=static_cast<std::uint32_t>(spec->highlight)||
       w.highlight198!=(spec->highlight?0xffccccccu:0xffffffffu)||w.disabled19C||w.disabled1A0!=0xff808080u)return false;
    // The enclosing row/full-pass entry already proved this exact Widget's
    // complete fields. Each Text still validates its own full payload and
    // exact Widget/outer/line identity in this immediate readonly call.
    for(std::size_t i=0;i<spec->lines;++i){const auto& l=r.lines[i];
        if(!l.constructed||!l.loaded||!l.attached||!l.copied||!l.configured||l.retired||!l.owner||
           !l.owner->payloadCurrentAfterWidgetFields(*this,w.entity))return false;
    }
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::currentColdRow1()const noexcept {
    return threadCurrent()&&!retirementStarted_&&rowColdCurrent(0);
}
bool OriginalUiGarageWidget1Owner50KG::currentColdPrefix9()const noexcept {
    if(!threadCurrent()||cursor_!=9||retirementStarted_)return false;
    for(std::size_t i=0;i<9;++i)if(!rowColdCurrent(i))return false;return true;
}
bool OriginalUiGarageWidget1Owner50KG::currentColdComplete()const noexcept {
    if(!threadCurrent()||cursor_!=rows_.size()||!coldDatReleased_||retirementStarted_||
       runtimePage_||!identityCurrent()||!fieldsCurrent())return false;
    // The complete same-owner field/link proof above precedes the first row,
    // exactly as before. This bounded pass is readonly: preserve all 22 rows
    // (including authored holes), movement payload, and all 37 Text payload
    // checks in their original order without repeating the whole proof per row.
    // Standalone row and Text APIs still perform their full own validation.
    for(std::size_t i=0;i<rows_.size();++i)if(!rowCompleteAfterFieldsCurrent(i,true))return false;
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::pageCustodyIdentityCurrent(const Page& page)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(!threadCurrent()||retirementStarted_||cursor_!=rows_.size()||!coldDatReleased_||
       (runtimePage_&&runtimePage_!=&page)||!identityCurrent()||!fieldsCurrent()||
       page.ownerThread_!=ownerThread_||page.issuer_!=issuer_||page.widgets_.get()!=this||
       !sameOwner(page.authority_,authority_)||!sameOwner(page.grant_,grant_)||!sameOwner(page.fog_,fog_)||
       !page.entityLive_||page.entityRefReleased_||page.entityCountReleased_||
       !page.managerAttached1D1C40_||page.loadReturnPhase_!=Page::LoadReturnPhase::returned||
       !grant_->garageAttachedPageCurrent(page,page.entity_)||page.entryAppearanceStep_>18)return false;
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::pageEntryCustodyCurrent(const Page& page)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(!pageCustodyIdentityCurrent(page))return false;
    if(page.entryPhase_==Page::EntryPhase::notEntered||page.entryPhase_==Page::EntryPhase::profilePending)
        return pageActivationCustodyCurrent(page);
    if(!runtimePage_||page.activationPhase_!=Page::ActivationPhase::returned||
       !page.entryProfile_||!page.entryProfileCurrent())return false;
    constexpr std::array<std::size_t,15> order{2,16,17,18,19,20,21,10,11,12,13,14,15,0,1};
    for(std::size_t n=0;n<order.size();++n){const auto& r=rows_[order[n]];const auto step=n<13?n:n+3;
        if(page.entryAppearanceStep_>step){if(!r.entryColorReturned)return false;}
        else if(page.entryAppearanceStep_<step&&r.entryColorStored)return false;
        if(r.entryColorStored&&r.entryColor!=(n<13?page.entryProfile_->primaryColor:page.entryProfile_->secondaryColor))return false;
    }
    if(page.entryPhase_!=Page::EntryPhase::appearancePending&&page.entryAppearanceStep_!=18)return false;
    if(entryCall_.started&&!entryCall_.returned&&
       (entryCall_.key!=entryCallKey(page)||
        !page.entryWidgetCallCurrent(entryCall_.row,entryCall_.operation,entryCall_.argument)))return false;
    // The initial pageCustodyIdentityCurrent already proved identity and
    // the complete fields table. Profile/appearance/call-cursor observations
    // above are readonly; retain every row suffix in this same-call pass.
    for(std::size_t i=0;i<rows_.size();++i)if(!rowCompleteAfterFieldsCurrent(i,false))return false;
    return true;
}
std::array<std::size_t,4> OriginalUiGarageWidget1Owner50KG::activationCallKey(const Page& page)noexcept {
    return {page.activationGroup_,page.activationPass_,page.activationControl_,page.activationWidget_};
}
bool OriginalUiGarageWidget1Owner50KG::pageActivationCustodyCurrent(const Page& page)const noexcept {
    if(!pageCustodyIdentityCurrent(page)||page.entryAppearanceStep_||entryCall_.started||
       (page.entryPhase_!=Page::EntryPhase::notEntered&&page.entryPhase_!=Page::EntryPhase::profilePending))return false;
    if(page.activationPhase_==Page::ActivationPhase::notActivated)
        return !runtimePage_&&!activationCall_.started&&currentColdComplete();
    if((page.activationPhase_==Page::ActivationPhase::showPending||
        page.activationPhase_==Page::ActivationPhase::resetPending)&&activationCall_.started)return false;
    if(activationCall_.started&&!activationCall_.returned&&
       (page.activationPhase_!=Page::ActivationPhase::widgetsPending||
        activationCall_.key!=activationCallKey(page)||!page.activationWidgetCallCurrent(activationCall_.row)))return false;
    const bool allReturned=page.activationPhase_==Page::ActivationPhase::moviePending||
        page.activationPhase_==Page::ActivationPhase::displayPending||page.activationPhase_==Page::ActivationPhase::returned;
    // pageCustodyIdentityCurrent above already proved identityCurrent and
    // the entire fieldsCurrent table. The intervening activation-cursor
    // checks and this ordered row suffix are readonly within the same call.
    // Do not repeat the complete table proof for each of its 22 rows.
    for(std::size_t i=0;i<rows_.size();++i){const auto& r=rows_[i];
        if(r.entryColorStored||r.selected38||!rowCompleteAfterFieldsCurrent(i,false))return false;
        if(r.activationReturned&&!r.activationShown)return false;
        if(!r.empty&&r.constructed&&r.hidden==r.activationShown)return false;
        if(allReturned&&(textSpec(i)||i==9)&&!r.activationReturned)return false;
        for(const auto& line:r.lines)if(line.runtimeColorKnown||line.setupTouched)return false;
    }
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::pageWattoShowCurrent90(const Page& page,const Watto& watto)const noexcept {
    // No callback into Watto publication or Page/Widget full current predicates:
    // this is the exact already-returned recursive show preceding its pose call.
    const auto& r=rows_[9];
    return threadCurrent()&&!retirementStarted_&&runtimePage_==&page&&issuer_==page.issuer_&&
        page.ownerThread_==ownerThread_&&page.widgets_.get()==this&&sameOwner(grant_,page.grant_)&&
        sameOwner(authority_,page.authority_)&&grant_&&grant_->current()&&
        page.entityLive_&&!page.entityRefReleased_&&!page.entityCountReleased_&&page.managerAttached1D1C40_&&
        grant_->garageAttachedPageCurrent(page,page.entity_)&&
        page.activationPhase_==Page::ActivationPhase::widgetsPending&&page.activationWidgetCallCurrent(9)&&
        activationCall_.started&&!activationCall_.returned&&activationCall_.showReturned&&
        activationCall_.row==9&&activationCall_.key==activationCallKey(page)&&
        r.watto.get()==&watto&&r.wattoEntity&&r.constructed&&!r.retiring&&!r.entityRetired&&
        r.activationShown&&!r.hidden&&(r.wattoEntity->flags29&3u)==3u&&
        grant_->garageWidgetRowCurrent(this,9,*r.wattoEntity);
}
bool OriginalUiGarageWidget1Owner50KG::activatePageWidget90(const Page& page,std::size_t index,D& d)noexcept {
    d={};d.row=index;
    if(!pageActivationCustodyCurrent(page)||page.activationPhase_!=Page::ActivationPhase::widgetsPending||
       !page.activationWidgetCallCurrent(index))return fail(d,S::authority,0x2A5C30);
    if(index>=rows_.size()||(!textSpec(index)&&index!=9))return fail(d,S::source,0x2A5C30);
    const auto key=activationCallKey(page);auto& call=activationCall_;auto& r=rows_[index];
    if(call.started&&call.key==key){
        if(call.returned||call.row!=index)return fail(d,S::phase,0x2A5C30);
    }else{
        if(call.started&&!call.returned)return fail(d,S::phase,0x2A5C30);
        call={};call.key=key;call.row=index;call.started=true;
    }
    runtimePage_=&page;
    if(!call.showReturned){
        C* entity=index==9?r.wattoEntity:&r.outer->entity;
        if(!entity)return fail(d,S::source,0x2A79E0);
        const auto step=grant_->activateGaragePageWidget90(this,page,index,*entity);
        if(!step.okay())return fail(d,S::scope,step.sourcePC);
        r.hidden=false;r.activationShown=true;call.showReturned=true;
    }
    if(index==9){
        if(!r.watto||!r.watto->activatePageWidget90(page,*this,d.watto))return fail(d,S::watto,d.watto.sourcePC);
    }
    //Text90 is only2A79E0→141630. In particular it does NOT write38,
    //recolor, select, stop, setup, update or draw a text leaf.
    r.activationReturned=true;call.returned=true;d.sourcePC=index==9?0x1D5C00:0x2A79EC;return true;
}
bool OriginalUiGarageWidget1Owner50KG::setPageEntryColor2AA7F0(
    const Page& page,std::size_t index,std::uint32_t color,D& d)noexcept {
    d={};d.row=index;
    if(!pageEntryCustodyCurrent(page)||page.entryPhase_!=Page::EntryPhase::appearancePending||
       !page.entryProfile_||!page.entryProfileCurrent())return fail(d,S::authority,0x1FBAB0);
    constexpr std::array<std::size_t,18> order{2,16,17,18,19,20,21,10,11,12,13,14,15,22,22,22,0,1};
    const auto step=page.entryAppearanceStep_;
    if(step>=order.size()||index>=rows_.size()||order[step]!=index||
       color!=(step<13?page.entryProfile_->primaryColor:page.entryProfile_->secondaryColor))
        return fail(d,S::source,0x2AA7F0);
    auto& r=rows_[index];const auto* spec=textSpec(index);
    if(!spec||!r.outer||r.entryColorReturned)return fail(d,S::phase,0x2AA7F0);
    if(!r.entryColorStored){
        // Authentic2AA804 store precedes the first source child call. Native
        // progress preserves a partially executed call without rolling it back.
        runtimePage_=&page;r.entryColor=color;r.outer->color17C=color;r.entryColorStored=true;
    }
    while(r.entryColorLines<spec->lines){
        const auto line=r.entryColorLines;d.line=line;auto& leaf=r.lines[line];
        if(!page.entryProfileCurrent()||!fieldsCurrent()||!leaf.owner)
            return fail(d,S::authority,0x2AA824);
        if(!leaf.owner->setColor29DA10(r.outer->color17C,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
        leaf.runtimeColor=r.outer->color17C;leaf.runtimeColorKnown=true;++r.entryColorLines;
    }
    r.outer->field1A4=1;r.entryColorReturned=true; //2AA84C AFTER every actual29DA10.
    d.sourcePC=0x2AA860;return true;
}
std::array<std::size_t,6> OriginalUiGarageWidget1Owner50KG::entryCallKey(const Page& page)noexcept {
    if(page.runtimeWidgetPending_)return {static_cast<std::size_t>(-1),
        static_cast<std::size_t>(page.runtimeWidgetSerial_),0,0,0,0};
    return {static_cast<std::size_t>(page.entryPhase_),page.entryGroupPass_,page.entryGroupControl_,
        page.entryControlWidget_,page.entrySelectionStep_,page.entryExplicitWidget_};
}
bool OriginalUiGarageWidget1Owner50KG::applyPageEntryWidgetCall(
    const Page& page,std::size_t index,std::uint32_t operation,std::int32_t argument,D& d)noexcept {
    d={};d.row=index;
    if(!pageEntryCustodyCurrent(page)||!runtimePage_||!page.entryWidgetCallCurrent(index,operation,argument))
        return fail(d,S::authority,operation==0x2A7A00?0x2A7A00:operation==0x78?0x2AA2A0:0x2AA3E0);
    const auto* spec=textSpec(index);
    if(!spec||index>=rows_.size()||(operation!=0x2A7A00&&operation!=0x78&&operation!=0x7C&&operation!=0x88)||
       (operation==0x2A7A00?(argument!=0&&argument!=1):argument!=0))return fail(d,S::source,0x2A5BA4);
    const bool runtimeVisibility=operation==0x2A7A00&&page.runtimeWidgetPending_;
    if(runtimeVisibility&&
       ((index!=0&&index!=1&&index!=2&&index!=6)||!page.sharedHookPending_||
        page.sharedHookPC_!=(argument?0x1FAFA0u:0x1FAFDCu)||page.runtimeWidgetSourcePC_!=0x2A5BA4||
        !page.runtimeWidgetCallCurrent(index,operation,argument)))return fail(d,S::authority,0x2A7A00);
    auto& r=rows_[index];auto& w=*r.outer;const auto key=entryCallKey(page);
    if(entryCall_.started&&entryCall_.key==key){
        if(entryCall_.returned||entryCall_.row!=index||entryCall_.operation!=operation||entryCall_.argument!=argument)
            return fail(d,S::phase,0x2A5BA4);
    }else{
        if(entryCall_.started&&!entryCall_.returned)return fail(d,S::phase,0x2A5BA4);
        entryCall_={};entryCall_.key=key;entryCall_.row=index;entryCall_.operation=operation;
        entryCall_.argument=argument;entryCall_.started=true;
    }
    auto& call=entryCall_;
    if(operation==0x88){
        // TextWidget+88 is2AA600, not the +84 disable body2AA540.
        // Clear38 before optional base-color calls; preserve4C, setup and visibility.
        if(!call.selectionStored){w.field38=0;r.selected38=0;call.selectionStored=true;}
        if(w.highlight194){
            while(call.colorLines<spec->lines){auto& leaf=r.lines[call.colorLines];d.line=call.colorLines;
                if(!page.entryWidgetCallCurrent(index,operation,argument)||!fieldsCurrent()||!leaf.owner)
                    return fail(d,S::authority,0x2AA63C);
                if(!leaf.owner->setColor29DA10(w.color17C,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
                leaf.runtimeColor=w.color17C;leaf.runtimeColorKnown=true;++call.colorLines;
            }
        }
        call.returned=true;d.sourcePC=0x2AA670;return true;
    }
    if(!call.selectionStored){
        //2A7A10/2A7A2C,2AA2BC or2AA3F4 precedes every delegated child effect.
        w.field38=operation==0x2A7A00?static_cast<std::uint32_t>(argument):operation==0x78?1u:0u;
        r.selected38=w.field38;call.selectionStored=true;
    }
    if(operation==0x2A7A00){
        if(!call.visibilityReturned){
            // Opening/closing group hooks run after real manager publication.
            // Keep their exact live prompt authority separate from cold entry.
            const auto result=runtimeVisibility?
                grant_->applyGarageRuntimePromptVisibility2A7A00(this,page,index,w.entity,argument!=0):
                grant_->applyGaragePageTextVisibility2A7A00(this,page,index,w.entity,argument!=0);
            if(!result.okay())return fail(d,S::scope,result.sourcePC);
            r.hidden=argument==0;call.visibilityReturned=true;
        }
    }else{
        //All admitted source rows have authored enable/disable-on-selection
        //3C/40/44/48==0. Their actual +78/+7C do not change Entity visibility.
        if(w.field3C||w.field40||w.field44||w.field48)return fail(d,S::source,0x2AA2C0);
        if(w.highlight194){
            const auto color=operation==0x78?w.highlight198:w.color17C;
            while(call.colorLines<spec->lines){auto& leaf=r.lines[call.colorLines];d.line=call.colorLines;
                if(!page.entryWidgetCallCurrent(index,operation,argument)||!fieldsCurrent()||!leaf.owner)
                    return fail(d,S::authority,operation==0x78?0x2AA330:0x2AA46C);
                if(!leaf.owner->setColor29DA10(color,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
                leaf.runtimeColor=color;leaf.runtimeColorKnown=true;++call.colorLines;
            }
        }
        //Select always sets up every actual leaf AFTER the optional highlight
        //loop. Non-highlight deselect stops each leaf instead. Highlight
        //deselect has no setup/stop suffix and does not rewrite outer17C/1A4.
        if(operation==0x78||!w.highlight194){
            while(call.setupLines<spec->lines){auto& leaf=r.lines[call.setupLines];d.line=call.setupLines;
                if(!page.entryWidgetCallCurrent(index,operation,argument)||!fieldsCurrent()||!leaf.owner)
                    return fail(d,S::authority,operation==0x78?0x2AA3A0:0x2AA4AC);
                if(operation==0x78){
                    game::OriginalUiTextLineSetup50KG setup;setup.alpha=w.alpha18C;
                    if(!leaf.owner->setup29DA40(setup,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
                    leaf.setupEnabled=true;
                }else{
                    if(!leaf.owner->stop29DB10(d.text))return fail(d,S::textOverlay,d.text.sourcePC);
                    leaf.setupEnabled=false;
                }
                leaf.setupTouched=true;++call.setupLines;
            }
        }
    }
    call.returned=true;d.sourcePC=operation==0x2A7A00?0x2A7A48:operation==0x78?0x2AA3D4:0x2AA4E4;
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::lineInputsCurrent(const TextOverlay* leaf,const C& outer,
    std::string_view font,std::string_view material,std::int32_t capacity,bool& dynamic)const noexcept {
    if(!threadCurrent()||!leaf||!identityCurrent()||!fieldsCurrent())return false;
    return lineInputsAfterFieldsCurrent(leaf,outer,font,material,capacity,dynamic);
}
bool OriginalUiGarageWidget1Owner50KG::lineInputsAfterFieldsCurrent(const TextOverlay* leaf,const C& outer,
    std::string_view font,std::string_view material,std::int32_t capacity,bool& dynamic)const noexcept {
    if(!threadCurrent()||!leaf)return false;
    for(std::size_t row=0;row<rows_.size();++row){const auto* spec=textSpec(row);const auto& r=rows_[row];
        if(!spec||!r.outer||&r.outer->entity!=&outer||!r.constructed||r.entityRetired||
           !r.fontKnown||!r.materialKnown||font!=spec->font||material!=spec->material)continue;
        for(std::size_t i=0;i<spec->lines;++i){const auto& l=r.lines[i];
            if(l.owner.get()!=leaf)continue;
            if(!l.constructed||!l.slotKnown||l.retired||!r.arrayPublished||r.arrayReleased||
               i>=r.arrayCapacity||r.array60[i]!=&leaf->entity_||l.capacity!=capacity||
               capacity!=spec->capacities[i])return false;
            dynamic=spec->dynamic;return true;
        }
    }
    return false;
}
bool OriginalUiGarageWidget1Owner50KG::prepare1CD7A0(const RootInputs& inputs,
    std::shared_ptr<Grant> grant,std::shared_ptr<Fog> fog,D& d)noexcept {
    d={};if(!threadCurrent())return fail(d,S::authority,0x1CD7A0);
    if(bound_||stage_!=Stage::empty)return fail(d,S::phase,0x1CD7A0);
    std::uint32_t pc=0x1CD7A0;
    try {
        const auto authority=inputs.authority();const auto* root=inputs.issuer();
        if(!authority||authority->nativeOwnerThread_!=ownerThread_||!authority->constructed_||authority->released_||
           !root||!sameOwner(authority,root->entities())||
           !grant||!grant->current()||grant->issuer()!=root||!sameOwner(authority,grant->application())||
           !fog||!fog->current()||!sameOwner(grant,fog->grant_)||fog->issuer_!=root||
           !fog->lights_||!fog->lights_->assets_||!fog->lights_->assets_->loaded())return fail(d,S::authority,pc);
        const auto archive=fog->lights_->assets_->archive();const auto provider=fog->lights_->provider_;
        if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)||
           !provider||!provider->sealed()||provider->revision()!=fog->lights_->providerRevision_)return fail(d,S::source,pc);
        pc=0x132820;const auto member=archive->find("UI/UIGarage/Widgets.Dat");
        assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
        if(!member.identity||!member.data||provider->lookup132820(member.identity->name,mounted)!=
           assets::OriginalStandardArchiveProvider50LA::Lookup::present||mounted.size!=member.size||
           !mounted.data||std::memcmp(mounted.data,member.data,member.size))return fail(d,S::source,pc);
        auto step=grant->bindGarageWidget1Payload(this,fog.get());if(!step.okay())return fail(d,S::scope,step.sourcePC);
        grant_=std::move(grant);fog_=std::move(fog);authority_=authority;issuer_=root;
        dictionary_=inputs.dictionary();archive_=archive;provider_=provider;providerRevision_=provider->revision();
        bound_=true;stage_=Stage::bound;
        pc=0x1CD7EC;
        if(!assets::readOriginalUiDat50KG(*archive_,member.identity->name,widgets_,d.parse))
            return fail(d,d.parse.error==assets::OriginalUiPageError50KG::allocation?S::allocation:S::source,pc);
        stage_=Stage::datLoaded;
        const auto* general=widgets_.section("General");if(!general)return fail(d,S::source,0x1CD824);
        pc=0x1CD83C;
        if(!assets::originalUiInteger50KG(*general,"NumberOfWidgets",0,count158_,d.parse,pc))return fail(d,S::source,pc);
        countKnown_=true;stage_=Stage::countStored;if(count158_!=22)return fail(d,S::source,pc);
        pc=0x1CD848;managerTable154_.reset(new C*[22]); // Original88 bytes; native22 pointers, no source null writes.
        source154Identity_=reinterpret_cast<std::uintptr_t>(managerTable154_.get());tableKnown_=true;
        step=grant_->publishGarageWidget1Table(this,managerTable154_.get());if(!step.okay())return fail(d,S::scope,step.sourcePC);
        stage_=Stage::tablePublished;
        return prepareCurrentRow1CD860({},d);
    }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::loadPending2A9000(D& d)noexcept {return loadRow2A9000(0,d);}
bool OriginalUiGarageWidget1Owner50KG::loadRow2A9000(std::size_t index,D& d)noexcept {
    const auto* spec=textSpec(index);if(!spec||index>=rows_.size())return fail(d,S::source,0x2A9000);
    auto& r=rows_[index];d.row=index;
    if(!identityCurrent()||!fieldsCurrent()||r.stage!=Stage::constructed)return fail(d,S::phase,0x2A9000);
    r.stage=Stage::loading;auto& w=*r.outer;std::uint32_t pc=0x2A9038;
    try {
        const auto* row=widgets_.section(spec->section);if(!row)return fail(d,S::source,pc);
        const char* flags[]{"EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect"};
        std::uint32_t* outputs[]{&w.field3C,&w.field40,&w.field44,&w.field48};
        const std::uint32_t calls[]{0x2A9038,0x2A9050,0x2A9068,0x2A9080};
        for(std::size_t i=0;i<4;++i){pc=calls[i];bool value=false;
            if(!assets::originalUiBoolean50KG(*row,flags[i],false,value,d.parse,pc))return fail(d,S::source,pc);
            *outputs[i]=static_cast<std::uint32_t>(value);
        }
        pc=0x2A90B0;const auto* font=row->find("FontName");
        if(!font||font->text!=spec->font||font->text.size()>=w.font68.size())return fail(d,S::source,pc);
        std::memcpy(w.font68.data(),font->text.c_str(),font->text.size()+1);r.fontKnown=true;
        pc=0x2A90CC;const auto* material=row->find("MaterialName");
        const auto& name=material?material->text:font->text;
        if(name!=spec->material||name.size()>=w.materialE8.size())return fail(d,S::source,pc);
        std::memcpy(w.materialE8.data(),name.c_str(),name.size()+1);r.materialKnown=true;
        pc=0x2A90E4;if(!assets::originalUiInteger50KG(*row,"XPosition",0,w.x180,d.parse,pc))return fail(d,S::source,pc);r.xKnown=true;
        pc=0x2A90FC;if(!assets::originalUiInteger50KG(*row,"YPosition",0,w.y184,d.parse,pc))return fail(d,S::source,pc);r.yKnown=true;
        pc=0x2A911C;const auto* justification=row->find("Justification");
        if(justification&&justification->text!=(spec->justification==3?"CenterBoth":spec->justification==4?"Right":"Left"))return fail(d,S::source,pc);
        pc=0x2A9138;if(!assets::originalUiNumber50KG(*row,"LeadingFactor",1,w.leading174,d.parse,pc))return fail(d,S::source,pc);r.leadingKnown=true;
        pc=0x2A9150;const auto* id=row->find("TextID");
        if(spec->dynamic){
            if(id)return fail(d,S::source,pc);
            pc=0x2A9174;
            if(!assets::originalUiInteger50KG(*row,"TextDynamicLineLength",0,w.dynamicLength16C,d.parse,pc)||
                w.dynamicLength16C!=40)return fail(d,S::source,pc);
            w.dynamic178=1;r.dynamicLengthKnown=true; //Source getter presence return2A917C.
            pc=0x2A9190;
            if(!assets::originalUiInteger50KG(*row,"TextDynamicLineCount",0,w.lineCount168,d.parse,pc)||
                w.lineCount168!=static_cast<std::int32_t>(spec->lines))return fail(d,S::source,pc);
            r.dynamicCountKnown=true;
        }else{
            if(!id||id->text!=spec->id)return fail(d,S::source,pc);
            //TextID-present skips16C and both dynamic stores.
            pc=0x2A91B4;
            if(!assets::originalUiInteger50KG(*row,"NumberOfLines",1,w.lineCount168,d.parse,pc)||
                w.lineCount168!=1)return fail(d,S::source,pc);
        }
        pc=0x2A91D0;if(!assets::originalUiInteger50KG(*row,"Alpha",255,w.alpha18C,d.parse,pc))return fail(d,S::source,pc);
        pc=0x2A91EC;if(!assets::originalUiNumber50KG(*row,"Depth",1,w.depth190,d.parse,pc))return fail(d,S::source,pc);
        auto color=[&](const char* key,const std::array<float,3>& fallback,const std::array<float,3>& exact,
            std::uint32_t& out,std::uint32_t* present=nullptr)->bool{
            std::array<float,3> rgb{};
            // Source158C00 splits the third scalar at newline, then302E10
            // reads a decimal prefix. Authenticated17..22 retain their fourth
            // ",104" token; it is neither alpha nor a fourth output component.
            const auto* value=row->find(key);
            if(index>=16&&std::strcmp(key,"Color")==0){
                if(!value||value->text!="104,104,104,104")return false;
                std::size_t start=0;
                for(std::size_t i=0;i<rgb.size();++i){
                    const auto end=value->text.find(',',start);
                    if(end==std::string::npos||!assets::parseOriginalUiNumber49N(
                        value->text.data()+start,end-start,rgb[i]))return false;
                    start=end+1;
                }
            }else if(!assets::originalUiTriple50KG(*row,key,fallback,rgb,d.parse,pc))return false;
            if(rgb!=exact)return false;
            if(present&&row->find(key))*present=1; //Source presence flag precedes packed color store.
            //Only the authenticated integral values enter this bounded leaf.
            //Their source cvt.w.s conversion is exact under every rounding mode.
            out=0xff000000u|(static_cast<std::uint32_t>(rgb[0])<<16)|
                (static_cast<std::uint32_t>(rgb[1])<<8)|static_cast<std::uint32_t>(rgb[2]);return true;
        };
        pc=0x2A921C;if(!color("Color",{40,40,40},{float(spec->rgb),float(spec->rgb),float(spec->rgb)},w.color17C))return fail(d,S::source,pc);
        pc=0x2A9290;const float highlight=spec->highlight?204.f:255.f;
        if(!color("HighlightColor",{255,255,255},{highlight,highlight,highlight},w.highlight198,&w.highlight194))return fail(d,S::source,pc);
        pc=0x2A9314;if(!color("DisabledColor",{128,128,128},{128,128,128},w.disabled1A0,&w.disabled19C))return fail(d,S::source,pc);
        pc=0x2A9380;if(!assets::originalUiInteger50KG(*row,"Height",0,w.height188,d.parse,pc))return fail(d,S::source,pc);
        w.type34=4;r.typeKnown=true; //2A9398, before justification result.
        w.justify170=spec->justification;r.justificationKnown=true; //Exact Right3C4758, CenterBoth3C4750 or Left3C4738.
        if(w.x180!=spec->x||w.y184!=spec->y||w.leading174!=spec->leading||w.height188||
           w.alpha18C!=255||w.depth190!=1)return fail(d,S::source,0x2A9438);
        if(spec->dynamic){r.stage=Stage::constructed;d.sourcePC=0x2A955C;return true;}
        if(!dictionary_||!dictionary_->current())return fail(d,S::authority,0x2A944C);
        Dictionary::Observation dictionaryState;
        if(!dictionary_->inspect(dictionaryState)||!dictionaryState.indexLoaded)return fail(d,S::source,0x2A9458);
        r.lookupOrdinal=0;const bool found=dictionary_->lookup20ED30(id->text,r.lookupOrdinal);r.lookupReached=true;
        firstUse(hashGuard8098(),hashAssertion3D6B10());r.hashAssertionReached=true; //2A9478 then947C.
        if(hashAssertion3D6B10().load(std::memory_order_relaxed)&&!dictionaryState.indexLoaded)return fail(d,S::source,0x2A94A0);
        firstUse(idGuard809C(),idAssertion3D6B18());r.idAssertionReached=true; //2A94E4 then94E8.
        if(idAssertion3D6B18().load(std::memory_order_relaxed)&&!found)return fail(d,S::source,0x2A9508);
        if(!found||r.lookupOrdinal!=spec->ordinal)return fail(d,S::source,0x2A9458);
        const char* text=nullptr;
        dictionary_->selectedText18AC60(r.lookupOrdinal,text,d.localization);
        r.stage=Stage::selectedTextPending;r.pendingPC=d.localization.sourcePC;
        //The actual root continuation alone issues the sealed18AC60 borrow.
        //A compatibility raw pointer never authorizes SetText.
        return fail(d,S::selectedTextPending,r.pendingPC);
    }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::prepareCurrentRow1CD860(
    std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,D& d)noexcept {
    d.row=cursor_;if(!identityCurrent()||!fieldsCurrent()||retirementStarted_||cursor_>=rows_.size())
        return fail(d,S::phase,0x1CD860);
    auto& r=rows_[cursor_];if(r.reached)return fail(d,S::phase,0x1CD860);
    // The real separate Watto owner is supplied only through the root's typed
    // continuation. No generic type skip, fabricated row or Page prerequisite.
    if(cursor_==9)return fail(d,S::wattoConstructorPending,0x1CD6E4);
    std::uint32_t pc=0x1CD874;
    try {
        const auto* row=widgets_.section(rowSection(cursor_));if(!row)return fail(d,S::source,pc);
        bool onTop=false;pc=0x1CD890;
        if(!assets::originalUiBoolean50KG(*row,"OnTop",false,onTop,d.parse,pc)||onTop)return fail(d,S::source,pc);
        const auto* type=row->find("Type");if(!type)return fail(d,S::source,0x1CD194);
        r.reached=true;
        if(emptyRow(cursor_)){
            if(type->text!="Empty")return fail(d,S::source,0x1CD6FC);
            managerTable154_[cursor_]=nullptr;slotsKnown_[cursor_]=true;r.stored=true;r.empty=true;
            const auto step=grant_->noteGarageEmptyRow1CDA10(this,cursor_,managerTable154_.get());
            if(!step.okay())return fail(d,S::scope,step.sourcePC);
            r.published=true;d.sourcePC=0x1CDA10;return true;
        }
        if(cursor_==5){
            if(type->text!="Movement_Control")return fail(d,S::source,0x1CD500);
            pc=0x1CD514;r.movement=std::unique_ptr<Movement>(new Movement);
            if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
            if(!r.movement->construct2AAF40(*issuer_,grant_,this,d.movement))return fail(d,S::scope,d.movement.sourcePC);
            r.constructed=true;r.sourceOuterIdentity=reinterpret_cast<std::uintptr_t>(&r.movement->entity_);
            if(!r.movement->load2AB010(*archive_,widgets_,d.movement))return fail(d,S::source,d.movement.sourcePC);
            bool value=false;pc=0x1CD8E0;
            if(!assets::originalUiBoolean50KG(*row,"WantsJoystickInput1",false,value,d.parse,pc)||
               !r.movement->storeWants1CD8F4(0,value,d.movement))return fail(d,S::source,pc);
            if(!assets::originalUiBoolean50KG(*row,"WantsJoystickInput2",false,value,d.parse,pc)||
               !r.movement->storeWants1CD8F4(1,value,d.movement))return fail(d,S::source,pc);
            r.wantsKnown=true;
            if(!r.movement->hide1CD908(d.movement))return fail(d,S::scope,d.movement.sourcePC);r.hidden=true;
            managerTable154_[cursor_]=&r.movement->entity_;slotsKnown_[cursor_]=true;r.stored=true;
            if(!r.movement->noteRowStored1CD918(d.movement))return fail(d,S::scope,d.movement.sourcePC);r.published=true;
            if(!r.movement->attach1CD9F8(d.movement))return fail(d,S::scope,d.movement.sourcePC);r.attached=true;
            r.stage=Stage::rowAttached;d.sourcePC=0x1CD9F8;return true;
        }
        if(!textSpec(cursor_)||type->text!="Overlay_Text")return fail(d,S::unsupportedType,0x1CD384);
        pc=0x1CD3D4;r.outer=std::make_unique<OverlayText>();
        if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
        const auto step=grant_->constructGarageOverlayText2A8F00(this,cursor_,r.outer->entity);
        if(!step.okay())return fail(d,S::scope,step.sourcePC);
        r.outer->manager30=grant_->manager();r.constructed=true;r.stage=Stage::constructed;
        r.sourceOuterIdentity=reinterpret_cast<std::uintptr_t>(&r.outer->entity);
        if(!loadRow2A9000(cursor_,d))return false;
        if(textSpec(cursor_)->dynamic)return constructDynamic2A955C(cursor_,std::move(fonts),d);
        return fail(d,S::phase,0x2A9438); //Static path must stop for genuine root18AC60.
    }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::selectedInputCurrent(std::size_t index,std::string_view& text)const noexcept {
    if(!threadCurrent()||index>=rows_.size()||!issuer_||!dictionary_)return false;
    const auto* spec=textSpec(index);const auto& r=rows_[index];
    if(!spec||spec->dynamic||!r.borrow||r.lookupOrdinal!=spec->ordinal)return false;
    std::string_view candidate;
    if(!Dictionary::SelectedText::BorrowReader::readCurrent(r.borrow,*issuer_,*dictionary_,spec->ordinal,candidate))return false;
    std::size_t at=0;
    for(std::size_t i=0;i<spec->lines;++i){const auto delimiter=candidate.find('@',at);
        const auto end=delimiter==std::string_view::npos?candidate.size():delimiter;
        if(end-at!=static_cast<std::size_t>(spec->capacities[i]))return false;
        if((i+1<spec->lines)!=(delimiter!=std::string_view::npos))return false;
        at=end+1;
    }
    text=candidate;return true;
}
bool OriginalUiGarageWidget1Owner50KG::selectedInputCurrent(std::string_view& text)const noexcept {return selectedInputCurrent(0,text);}
bool OriginalUiGarageWidget1Owner50KG::configurePosition(std::size_t index,std::size_t line,D& d)noexcept {
    auto& r=rows_[index];auto& l=r.lines[line];auto& w=*r.outer;
    float height=0;if(!l.owner->fontLineHeight281730(height,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
    if(w.height188||!std::isfinite(height))return fail(d,S::source,0x2A9D18);
    //Exact separate binary32 MUL.S/MUL.S/ADD.S and finite EE truncation.
    volatile float advance=w.leading174*height;
    volatile float distance=static_cast<float>(line)*advance;
    volatile float y=static_cast<float>(w.y184)+distance;
    std::int32_t converted=0;
    if(!game::roundOriginalUiTextMetric50KG(y,1,converted))return fail(d,S::source,0x2A9D4C);
    if(!l.owner->setPosition29DB50(w.x180,converted,0,d.text))return fail(d,S::textOverlay,d.text.sourcePC);
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::continueCold2A9540(std::shared_ptr<const Dictionary::SelectedText> selected,
    std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,D& d)noexcept {
    return continueStatic2A9540(0,std::move(selected),std::move(fonts),d);
}
bool OriginalUiGarageWidget1Owner50KG::continueStatic2A9540(std::size_t index,
    std::shared_ptr<const Dictionary::SelectedText> selected,std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,D& d)noexcept {
    d={};d.row=index;if(!threadCurrent())return fail(d,S::authority,0x2A9540);
    if(!rowPendingCurrent(index)||!selected)return fail(d,S::phase,0x2A9540);
    auto& r=rows_[index];const auto* spec=textSpec(index);
    if(r.borrow||r.countReset)return fail(d,S::phase,0x2A9540);
    std::string_view text;
    if(!Dictionary::SelectedText::BorrowReader::readCurrent(selected,*issuer_,*dictionary_,spec->ordinal,text))return fail(d,S::authority,0x18AC60);
    r.borrow=std::move(selected);
    if(!selectedInputCurrent(index,text)){r.borrow.reset();return fail(d,S::authority,0x18AC60);}
    r.stage=Stage::copyingText;auto& w=*r.outer;std::uint32_t pc=0x2A9B68;
    auto childFailure=[&](){return fail(d,d.text.status==OriginalUiGarageTextOverlayStatus50KG::allocation?S::allocation:S::textOverlay,d.text.sourcePC);};
    try {
        w.lineCount168=0;r.countReset=true;
        std::size_t offset=0;
        for(std::size_t i=0;i<spec->lines;++i){d.line=i;r.activeLine=i;auto& l=r.lines[i];
            ++w.lineCount168;r.stage=Stage::lineCountStored; //2A9BF0 BEFORE realloc.
            pc=0x2A9BFC;auto array=std::unique_ptr<C*[]>(new C*[i+1]);
            if(!identityCurrent()||!fieldsCurrent()||!selectedInputCurrent(index,text))return fail(d,S::authority,pc);
            //The actual successful realloc preserves previous known slots.
            for(std::size_t j=0;j<i;++j)array[j]=r.array60[j];
            r.array60=std::move(array);r.arrayCapacity=i+1;w.lines60=r.array60.get();
            r.source60Identity=reinterpret_cast<std::uintptr_t>(w.lines60);r.arrayPublished=true;r.stage=Stage::lineArrayPublished;
            pc=0x2A9C08;l.owner=std::unique_ptr<TextOverlay>(new TextOverlay);
            if(!identityCurrent()||!fieldsCurrent()||!selectedInputCurrent(index,text))return fail(d,S::authority,pc);
            if(!l.owner->construct29C850(*issuer_,grant_,this,w.entity,d.text))return childFailure();
            l.constructed=true;l.sourceIdentity=reinterpret_cast<std::uintptr_t>(&l.owner->entity_);r.stage=Stage::lineConstructed;
            l.capacity=spec->capacities[i];l.inputOffset=offset;
            r.array60[i]=&l.owner->entity_;l.slotKnown=true;r.stage=Stage::lineSlotPublished;
            if(!l.owner->load29CA40(fonts,archive_,w.font68.data(),w.materialE8.data(),l.capacity,d.text))return childFailure();
            l.loaded=true;r.stage=Stage::lineLoaded;
            if(!identityCurrent()||!fieldsCurrent()||!selectedInputCurrent(index,text))return fail(d,S::authority,0x2A9C64);
            if(!l.owner->attach2A9C64(d.text))return childFailure();l.attached=true;r.stage=Stage::lineAttached;
            //2A9C78 receives the ORIGINAL remaining string, not a separately
            //terminated line.29D940 compares full strlen before truncation.
            if(!l.owner->setText29D940(text.substr(offset),d.text))return childFailure();l.copied=true;r.stage=Stage::textCopied;
            if(!l.owner->setAlpha29DBC0(w.alpha18C,0,d.text)||!l.owner->setDepth29DC20(w.depth190,d.text)||
               !l.owner->setJustification29DB30(w.justify170,d.text)||!l.owner->setColor29DA10(w.color17C,d.text))return childFailure();
            if(!configurePosition(index,i,d))return false;
            const game::OriginalUiTextLineSetup50KG setup{};
            if(!l.owner->setup29DA40(setup,d.text))return childFailure();
            l.configured=true;r.stage=Stage::lineConfigured;offset+=std::size_t(l.capacity)+1;
        }
        r.borrow.reset(); //The ENTIRE2A9AD0 call, including every line suffix, returned.
        return finishRow1CD8B4(index,d);
    }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::constructDynamic2A955C(std::size_t index,
    std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,D& d)noexcept {
    const auto* spec=textSpec(index);auto& r=rows_[index];d.row=index;
    if(!spec||!spec->dynamic||!identityCurrent()||!fieldsCurrent()||!r.dynamicLengthKnown||r.arrayPublished||r.borrow)
        return fail(d,S::phase,0x2A955C);
    auto& w=*r.outer;std::uint32_t pc=0x2A956C;
    auto childFailure=[&](){return fail(d,d.text.status==OriginalUiGarageTextOverlayStatus50KG::allocation?S::allocation:S::textOverlay,d.text.sourcePC);};
    try {
        auto array=std::unique_ptr<C*[]>(new C*[spec->lines]);
        if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
        r.array60=std::move(array);r.arrayCapacity=spec->lines;w.lines60=r.array60.get();
        r.source60Identity=reinterpret_cast<std::uintptr_t>(w.lines60);r.arrayPublished=true;r.stage=Stage::lineArrayPublished;
        for(std::size_t i=0;i<spec->lines;++i){r.activeLine=i;d.line=i;auto& l=r.lines[i];
            pc=0x2A9580;l.owner=std::unique_ptr<TextOverlay>(new TextOverlay);
            if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
            if(!l.owner->construct29C850(*issuer_,grant_,this,w.entity,d.text))return childFailure();
            l.constructed=true;l.sourceIdentity=reinterpret_cast<std::uintptr_t>(&l.owner->entity_);l.capacity=w.dynamicLength16C;
            r.array60[i]=&l.owner->entity_;l.slotKnown=true;r.stage=Stage::lineSlotPublished; //2A95B4 BEFORE load.
            if(!l.owner->load29CA40(fonts,archive_,w.font68.data(),w.materialE8.data(),l.capacity,d.text))return childFailure();l.loaded=true;
            const game::OriginalUiTextLineSetup50KG setup{};
            if(!l.owner->setup29DA40(setup,d.text)||!l.owner->setJustification29DB30(w.justify170,d.text)||
               !l.owner->setColor29DA10(w.color17C,d.text)||!l.owner->setText29D940(std::string_view("",0),d.text))return childFailure();
            l.copied=true; //Actual nonnull empty literal3BF728, no18AC60 lookup.
            if(!configurePosition(index,i,d))return false;
            if(!l.owner->setAlpha29DBC0(w.alpha18C,0,d.text)||!l.owner->setDepth29DC20(w.depth190,d.text))return childFailure();
            l.configured=true;
            if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,0x2A9710);
            if(!l.owner->attach2A9710(d.text))return childFailure();l.attached=true;r.stage=Stage::lineConfigured;
        }
        return finishRow1CD8B4(index,d);
    }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::finishRow1CD8B4(std::size_t index,D& d)noexcept {
    if(index!=cursor_||!identityCurrent()||!fieldsCurrent())return fail(d,S::phase,0x1CD8B4);
    auto& r=rows_[index];auto& w=*r.outer;
    const auto* spec=textSpec(index);if(!spec)return fail(d,S::source,0x1CD8B4);
    for(std::size_t i=0;i<spec->lines;++i)if(!r.lines[i].configured||!r.lines[i].attached)return fail(d,S::phase,0x1CD8B4);
    try {
    const auto* row=widgets_.section(spec->section);if(!row)return fail(d,S::source,0x1CD8E0);
    bool first=false,second=false;
    if(!assets::originalUiBoolean50KG(*row,"WantsJoystickInput1",false,first,d.parse,0x1CD8E0))return fail(d,S::source,0x1CD8E0);
    w.field50=static_cast<std::uint32_t>(first);
    if(!assets::originalUiBoolean50KG(*row,"WantsJoystickInput2",false,second,d.parse,0x1CD8E0))return fail(d,S::source,0x1CD8E0);
    w.field54=static_cast<std::uint32_t>(second);r.wantsKnown=true;
    if(first||second||!identityCurrent()||!fieldsCurrent())return fail(d,S::source,0x1CD8E0);
    auto step=grant_->hideGarageWidgetRow1CD908(this,index,w.entity);if(!step.okay())return fail(d,S::scope,step.sourcePC);r.hidden=true;
    managerTable154_[index]=&w.entity;slotsKnown_[index]=true;r.stored=true;r.stage=Stage::rowStored;
    step=grant_->publishGarageWidgetRow1CD918(this,index,w.entity);if(!step.okay())return fail(d,S::scope,step.sourcePC);r.published=true;
    step=grant_->attachGarageWidgetRow1CD9F8(this,index,w.entity);if(!step.okay())return fail(d,S::scope,step.sourcePC);r.attached=true;
    r.stage=Stage::rowAttached;r.pendingPC=0;d.sourcePC=0x1CD9F8;return true;
    }catch(...){return fail(d,S::allocation,0x1CD8E0);}
}
bool OriginalUiGarageWidget1Owner50KG::continueWattoRow1CD8B4(std::shared_ptr<Watto> watto,D& d)noexcept {
    d={};d.row=9;
    if(!threadCurrent()||cursor_!=9||retirementStarted_||!identityCurrent()||!fieldsCurrent())
        return fail(d,S::phase,0x1CD8B4);
    auto& r=rows_[9];if(r.reached||!watto)return fail(d,S::wattoConstructorPending,0x1CD6E4);
    std::uint32_t pc=0x1CD68C;
    try {
    const auto* row=widgets_.section("Widget 10");
    if(!row||!row->find("Type")||row->find("Type")->text!="Watto")return fail(d,S::source,0x1CD68C);
    pc=0x1CD890;bool onTop=false;
    if(!assets::originalUiBoolean50KG(*row,"OnTop",false,onTop,d.parse,0x1CD890)||onTop)
        return fail(d,S::source,0x1CD890);
    pc=0x1CD8B4;Watto::ColdPublication publication;
    if(!watto->constructorPublication(publication))return fail(d,S::wattoConstructorPending,0x1CD6E4);
    if(!sameOwner(publication.grant,grant_)||!publication.widget||!publication.soundGroup||
       !publication.animation220||!publication.responseEmitter||!publication.modelEntity8)
        return fail(d,S::authority,0x1CD8B4);
    auto step=grant_->bindGarageWattoRow1CD8B4(this,watto.get(),*publication.widget,*publication.soundGroup,
        *publication.animation220,*publication.responseEmitter,*publication.modelEntity8);
    if(!step.okay())return fail(d,S::scope,step.sourcePC);
    r.watto=std::move(watto);r.wattoEntity=publication.widget;r.constructed=true;r.reached=true;
    r.sourceOuterIdentity=reinterpret_cast<std::uintptr_t>(publication.widget);r.stage=Stage::constructed;
    pc=0x1CD8E0;for(std::size_t i=0;i<2;++i){bool value=false;
        if(!assets::originalUiBoolean50KG(*row,i?"WantsJoystickInput2":"WantsJoystickInput1",false,value,d.parse,0x1CD8E0)||value)
            return fail(d,S::source,0x1CD8E0);
        if(!r.watto->storeWants1CD8F4(i,value,d.watto))return fail(d,S::watto,d.watto.sourcePC);
    }
    r.wantsKnown=true;
    step=grant_->hideGarageWidgetRow1CD908(this,9,*publication.widget);
    if(!step.okay())return fail(d,S::scope,step.sourcePC);r.hidden=true;r.stage=Stage::hidden;
    managerTable154_[9]=publication.widget;slotsKnown_[9]=true;r.stored=true;r.stage=Stage::rowStored;
    step=grant_->publishGarageWidgetRow1CD918(this,9,*publication.widget);
    if(!step.okay())return fail(d,S::scope,step.sourcePC);r.published=true;
    step=grant_->attachGarageWidgetRow1CD9F8(this,9,*publication.widget);
    if(!step.okay())return fail(d,S::scope,step.sourcePC);r.attached=true;r.stage=Stage::rowAttached;
    d.sourcePC=0x1CD9F8;return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,pc);}
    catch(...){return fail(d,S::source,pc);}
}
bool OriginalUiGarageWidget1Owner50KG::borrowPageWidget1D7284(
    std::size_t authoredId,C*& out,std::array<std::int32_t,2>& wants)const noexcept {
    out=nullptr;wants={};
    if(!currentColdComplete()||authoredId<1||authoredId>rows_.size())return false;
    const auto index=authoredId-1;const auto& row=rows_[index];
    if(!slotsKnown_[index]||!rowColdCurrent(index))return false;
    auto* entity=managerTable154_[index];
    if(!entity)return row.empty;
    if(row.outer){
        if(entity!=&row.outer->entity||!row.wantsKnown)return false;
        wants={{static_cast<std::int32_t>(row.outer->field50),static_cast<std::int32_t>(row.outer->field54)}};
    }else if(row.movement){
        if(entity!=&row.movement->entity_||!row.wantsKnown||!row.movement->payloadCurrent())return false;
        wants=row.movement->wantsJoystick50_;
    }else if(row.watto){
        if(entity!=row.wattoEntity||!row.wantsKnown||!row.watto->borrowPageWants50(entity,wants))return false;
    }else return false;
    out=entity;return true;
}
bool OriginalUiGarageWidget1Owner50KG::ownsWattoRow(const Watto* owner)const noexcept {
    return threadCurrent()&&owner&&rows_[9].watto.get()==owner&&!rows_[9].entityRetired;
}
bool OriginalUiGarageWidget1Owner50KG::finishCold1CDA34(D& d)noexcept {
    if(!threadCurrent()||retirementStarted_||cursor_!=rows_.size()||coldDatReleased_||
       !identityCurrent()||!fieldsCurrent())return fail(d,S::phase,0x1CDA34);
    for(std::size_t i=0;i<rows_.size();++i)if(!rowColdCurrent(i))return fail(d,S::phase,0x1CDA34);
    widgets_=assets::OriginalUiDat50KG{};coldDatReleased_=true;
    d={};d.row=rows_.size();d.sourcePC=0x1CDA3C;return true;
}
bool OriginalUiGarageWidget1Owner50KG::advanceRow1CDA1C(D& d)noexcept {
    if(!threadCurrent()||retirementStarted_||cursor_>=rows_.size()||!rowColdCurrent(cursor_))return fail(d,S::phase,0x1CDA1C);
    const auto step=grant_->advanceGarageWidgetRow1CDA1C(this,cursor_);if(!step.okay())return fail(d,S::scope,step.sourcePC);
    ++cursor_;d.sourcePC=0x1CDA1C;return true;
}
bool OriginalUiGarageWidget1Owner50KG::retireRow2A8F70(std::size_t index,D& d)noexcept {
    auto& r=rows_[index];d.row=index;
    const auto reject=[&](const char* branch,S status,std::uint32_t pc)noexcept {
        fail(d,status,pc);std::size_t retiredRows=0,retiredLines=0;
        for(const auto& row:rows_){if(row.entityRetired)++retiredRows;
            for(const auto& line:row.lines)if(line.retired)++retiredLines;}
        return logWidgetRetirementFailure(branch,d,index,d.line,static_cast<unsigned>(retirementRoute_),
            r.attached,r.retiring,r.entityRetired,retiredRows,retiredLines);
    };
    if(!r.reached||r.entityRetired||r.empty)return true;
    if(index==9){
        if(!r.watto||!r.wattoEntity)return reject("watto_owner",S::authority,0x1D4480);
        if(!r.retiring){
            const auto step=grant_->beginGarageWattoRowRetirement142590(this,r.watto.get(),*r.wattoEntity);
            if(!step.okay())return reject("watto_begin",S::scope,step.sourcePC);r.retiring=true;
        }
        const bool retired=retirementRoute_==RetirementRoute::normalManager?
            r.watto->closeManager142590(d.watto):r.watto->closeCold142590(d.watto);
        if(!retired)return reject("watto_close",S::watto,d.watto.sourcePC);
        const auto step=grant_->noteGarageWattoRowRetired1D4480(this,r.watto.get(),*r.wattoEntity);
        if(!step.okay())return reject("watto_note",S::scope,step.sourcePC);
        r.retiring=false;r.entityRetired=true;r.watto.reset();r.stage=Stage::detachedRetired;return true;
    }
    if(index==5){
        if(r.movement&&!r.movement->retire2AAFA0(d.movement))return reject("movement_retire",S::scope,d.movement.sourcePC);
        r.entityRetired=true;r.movement.reset();return true;
    }
    if(r.attached&&!r.retiring&&r.outer&&r.outer->entity.next14)return reject("outer_not_tail",S::scope,0x142590);
    //The newest child may be a detached failed prefix. Then retire actual
    //attached child tails in reverse source append order, not array index0 first.
    for(std::size_t n=r.lines.size();n>0;--n){auto& l=r.lines[n-1];d.line=n-1;
        if(l.owner&&!l.retired){
            if(!l.owner->preflightRetirement(d.text))return reject("text_preflight",S::textOverlay,d.text.sourcePC);
            if(!l.owner->retire29C9C0(d.text))return reject("text_retire",S::textOverlay,d.text.sourcePC);
            l.retired=true;l.owner.reset();
        }
    }
    r.borrow.reset();
    if(r.constructed&&!r.entityRetired){
        auto& c=r.outer->entity;
        if(!r.retiring){auto step=grant_->preflightGarageWidget1Retirement(this,c);if(!step.okay())return reject("outer_preflight",S::scope,step.sourcePC);
            r.previousAtRetire=c.previous10;
            step=grant_->beginGarageWidget1Retirement(this,c);if(!step.okay())return reject("outer_begin",S::scope,step.sourcePC);r.retiring=true;}
        if(r.array60){r.array60.reset();r.arrayReleased=true;} //Source60 remains stale.
        const auto step=grant_->finishGarageWidget1Retirement(this,c);if(!step.okay())return reject("outer_finish",S::scope,step.sourcePC);
        r.retiring=false;r.entityRetired=true;
    }
    r.outer.reset();r.stage=Stage::detachedRetired;return true;
}
bool OriginalUiGarageWidget1Owner50KG::retireDetached2A8F70(D& d)noexcept {
    return retireRows2A8F70(RetirementRoute::coldAbort,d);
}
bool OriginalUiGarageWidget1Owner50KG::retireManagerRows2A8F70(D& d)noexcept {
    d={};
    //This receipt is produced only by the genuine detached-manager Page/
    //Scene destructor suffix. Color/entry stage is not lifetime authority.
    if(!threadCurrent()||!grant_||!grant_->garageManagerPageRetired1413E0())
        return fail(d,S::authority,0x1413E0);
    const C* manager=grant_->manager();
    if(!manager||manager->parent1C||manager->previous10||manager->next14||manager->reference00!=1)
        return fail(d,S::scope,0x2A35A4);
    return retireRows2A8F70(RetirementRoute::normalManager,d);
}
bool OriginalUiGarageWidget1Owner50KG::preflightFailedSharedPageRows(
    const OriginalUiGaragePageOwner50KG& page,D& d)const noexcept {
    d={};
    if(!threadCurrent()||!currentColdComplete()||retirementStarted_||retirementRoute_!=RetirementRoute::none||
       page.widgets_.get()!=this||!page.sharedPromptFailurePrefixCurrent()||page.entity_.parent1C||
       !fog_||!fog_->currentAttached()||fog_->widgetOwner_!=this||!fog_->entity(0))
        return fail(d,S::phase,0x29B638);
    return preflightRows2A8F70(RetirementRoute::coldAbort,fog_->entity(0),d);
}
bool OriginalUiGarageWidget1Owner50KG::preflightRows2A8F70(
    RetirementRoute route,const C* actualLaterTail,D& d)const noexcept {
    const auto reject=[&](const char* branch,S status,std::uint32_t pc,std::size_t index)noexcept {
        fail(d,status,pc);std::size_t retiredRows=0,retiredLines=0;
        for(const auto& row:rows_){if(row.entityRetired)++retiredRows;
            for(const auto& line:row.lines)if(line.retired)++retiredLines;}
        const auto* row=index<rows_.size()?&rows_[index]:nullptr;
        return logWidgetRetirementFailure(branch,d,index,d.line,static_cast<unsigned>(route),
            row&&row->attached,row&&row->retiring,row&&row->entityRetired,retiredRows,retiredLines);
    };
    if(!identityCurrent()||!fieldsCurrent())return reject("preflight_fields",S::phase,0x2A8F70,rows_.size());
    //No allocation, no source mutation: validate ALL retained live identities,
    //topology and aggregate canonical releases before retiring the first tail.
    std::array<const TextOverlay*,88> leaves{};std::size_t leafCount=0;
    const C* lastAttached=nullptr;std::size_t lastAttachedRow=rows_.size();
    for(std::size_t i=0;i<rows_.size();++i){const auto& r=rows_[i];
        if(r.constructed&&!r.entityRetired){
            if(i==5&&!r.movement->identityCurrent())return reject("preflight_movement_identity",S::authority,0x2AAFA0,i);
            if(i==9){
                if(!r.watto||!r.wattoEntity)return reject("preflight_watto_owner",S::watto,0x142590,i);
                const bool ready=route==RetirementRoute::normalManager?
                    r.watto->preflightManagerRetirement142590(d.watto):r.watto->preflightColdRetirement142590(d.watto);
                if(!ready)return reject("preflight_watto",S::watto,d.watto.sourcePC,i);
                if(r.attached){lastAttached=r.wattoEntity;lastAttachedRow=i;}continue;
            }
            const auto* outer=i==5?&r.movement->entity_:&r.outer->entity;
            if(!(r.retiring?grant_->garageWidgetOuterRetirementCurrent(this,*outer):
                grant_->garageWidgetRowCurrent(this,i,*outer)))return reject("preflight_scope_row",S::scope,0x142590,i);
            if(r.attached){lastAttached=outer;lastAttachedRow=i;}
        }
        for(const auto& l:r.lines)if(l.owner&&!l.retired)leaves[leafCount++]=l.owner.get();
    }
    if(lastAttached&&lastAttached->next14!=actualLaterTail)return reject("preflight_later_tail",S::scope,0x142590,lastAttachedRow);
    if(!TextOverlay::preflightSharedRetirement(std::span<const TextOverlay* const>(leaves.data(),leafCount),d.text))
        return reject("preflight_shared_text",S::textOverlay,d.text.sourcePC,rows_.size());
    return true;
}
bool OriginalUiGarageWidget1Owner50KG::retireRows2A8F70(RetirementRoute route,D& d)noexcept {
    d={};if(!threadCurrent())return fail(d,S::authority,0x2A8F70);
    if(route==RetirementRoute::none||(retirementRoute_!=RetirementRoute::none&&retirementRoute_!=route))
        return fail(d,S::phase,0x2A8F70);
    if(detachedReleased_)return true;
    if(!preflightRows2A8F70(route,nullptr,d))return false;
    retirementRoute_=route;retirementStarted_=true;
    for(std::size_t n=rows_.size();n>0;--n)if(!retireRow2A8F70(n-1,d))return false;
    const auto step=grant_->noteGarageWidget1DetachedReleased(this);if(!step.okay())return fail(d,S::scope,step.sourcePC);
    //A pending-call envelope frees its retained DAT here. A completed loop
    //already destroyed that DAT at1CDA34; neither path rewrites source154.
    widgets_=assets::OriginalUiDat50KG{};dictionary_.reset();detachedReleased_=true;
    stage_=Stage::detachedRetired;d.sourcePC=0x2A8FC4;return true;
}
bool OriginalUiGarageWidget1Owner50KG::retireManagerTable2A3B18(D& d)noexcept {
    d={};if(!threadCurrent())return fail(d,S::authority,0x2A3B18);
    if(tableReleased_)return true;
    if(!bound_||!detachedReleased_||!grant_||!grant_->managerWidget1TableRetirementCurrent(this))return fail(d,S::phase,0x2A3B18);
    for(std::size_t i=0;i<slotsKnown_.size();++i)if(slotsKnown_[i]!=rows_[i].stored)return fail(d,S::phase,0x2A3B18);
    if(tableKnown_&&(!managerTable154_||source154Identity_!=reinterpret_cast<std::uintptr_t>(managerTable154_.get())))return fail(d,S::phase,0x2A3B18);
    managerTable154_.reset(); //Source154/158 remain stale, not invented null/zero stores.
    const auto step=grant_->noteGarageWidget1TableReleased(this);if(!step.okay())std::terminate();
    tableReleased_=true;stage_=Stage::tableRetired;d.sourcePC=0x2A3B18;return true;
}
OriginalUiGarageWidget1Owner50KG::Observation OriginalUiGarageWidget1Owner50KG::inspect()const noexcept {return inspectRow(0);}
OriginalUiGarageWidget1Owner50KG::Observation OriginalUiGarageWidget1Owner50KG::inspectRow(std::size_t index)const noexcept {
    Observation o;if(!threadCurrent()||index>=rows_.size())return o;const auto& r=rows_[index];const auto& l=r.lines[0];
    o.stage=r.stage;o.count158=count158_;o.countKnown=countKnown_;o.tableKnown=tableKnown_;
    o.tableAllocated=bool(managerTable154_);o.source154Identity=source154Identity_;o.slotsKnown=slotsKnown_;
    o.constructed=r.constructed;o.retiring=r.retiring;o.detachedReleased=detachedReleased_;o.tableReleased=tableReleased_;
    o.fontKnown=r.fontKnown;o.materialKnown=r.materialKnown;o.xKnown=r.xKnown;o.yKnown=r.yKnown;o.leadingKnown=r.leadingKnown;
    o.dynamicLengthKnown=r.dynamicLengthKnown;o.justificationKnown=r.justificationKnown;o.typeKnown=r.typeKnown; //16C intentionally unknown.
    o.lookupReached=r.lookupReached;o.hashAssertionReached=r.hashAssertionReached;o.idAssertionReached=r.idAssertionReached;
    o.lookupOrdinal=r.lookupOrdinal;o.pendingPC=r.pendingPC;
    o.lineEntity=l.constructed&&!l.retired&&l.owner?&l.owner->entity_:nullptr;
    o.source60Identity=r.source60Identity;o.lineSlotKnown=l.slotKnown;o.lineConstructed=l.constructed;
    o.lineLoaded=l.loaded;o.lineAttached=l.attached;o.textCopied=l.copied;o.lineConfigured=l.configured;
    o.hidden=r.hidden;o.rowStored=r.stored;o.outerAttached=r.attached;o.selectedBorrowLive=bool(r.borrow);
    o.lineRetired=l.retired;o.lineArrayReleased=r.arrayReleased;
    if(index==9&&r.watto&&r.constructed){
        o.entity=r.wattoEntity;o.manager30=grant_?grant_->manager():nullptr;return o;
    }
    if(index==5&&r.movement&&r.constructed){
        const auto& m=*r.movement;o.entity=&m.entity_;o.manager30=m.manager30_;o.type34=m.type34_;
        o.field50=static_cast<std::uint32_t>(m.wantsJoystick50_[0]);o.field54=static_cast<std::uint32_t>(m.wantsJoystick50_[1]);return o;
    }
    if(!r.constructed||!r.outer)return o;
    const auto& w=*r.outer;o.entity=&w.entity;o.manager30=w.manager30;o.type34=w.type34;o.field38=w.field38;
    o.field3C=w.field3C;o.field40=w.field40;o.field44=w.field44;o.field48=w.field48;o.field4C=w.field4C;
    o.field50=w.field50;o.field54=w.field54;o.field58=w.field58;o.field64=w.field64;o.lineArrayNull=!w.lines60;
    o.font68=w.font68;o.materialE8=w.materialE8;o.lineCount168=w.lineCount168;o.justify170=w.justify170;o.dynamic178=w.dynamic178;
    o.leading174=w.leading174;o.x180=w.x180;o.y184=w.y184;o.height188=w.height188;o.depth190=w.depth190;
    o.alpha18C=w.alpha18C;o.color17C=w.color17C;o.highlight194=w.highlight194;o.highlight198=w.highlight198;
    o.disabled19C=w.disabled19C;o.disabled1A0=w.disabled1A0;o.field1A4=w.field1A4;return o;
}
}
namespace original_main_entry50KG {
bool Application::prepareGarageWidget1_1CD7A0(rr::platform::OriginalUiGarageWidget1Owner50KG& owner,
    std::shared_ptr<rr::game::OriginalUiEntityScope50KG::GarageConstructionGrant> grant,
    std::shared_ptr<rr::platform::OriginalUiGarageManagerFog50KG> fog,
    rr::platform::OriginalUiGarageWidget1Diagnostic50KG& d)const noexcept {
    using S=rr::platform::OriginalUiGarageWidget1Status50KG;d={};
    //Immutable thread identity first, before copying mutable source32C.
    if(!rr::platform::OriginalUiGarageWidget1Owner50KG::authorityThread(entitiesOwner_)){
        d.status=S::authority;d.sourcePC=0x1CD7A0;return false;
    }
    const auto dictionary=sourceLocalization32COwner_;
    if(dictionary&&(dictionary->issuer_!=this||!dictionary->current()||
       dictionary.owner_before(dictionary->issuedOwner_.lock())||dictionary->issuedOwner_.lock().owner_before(dictionary))){
        d.status=S::authority;d.sourcePC=0x2A944C;return false;
    }
    const rr::platform::OriginalUiGarageWidget1Owner50KG::RootInputs inputs(this,entitiesOwner_,dictionary);
    return owner.prepare1CD7A0(inputs,std::move(grant),std::move(fog),d);
}
bool Application::continueGarageWidget1_2A9540(rr::platform::OriginalUiGarageWidget1Owner50KG& owner,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    rr::platform::OriginalUiGarageWidget1Diagnostic50KG& d)const noexcept {
    using W=rr::platform::OriginalUiGarageWidget1Owner50KG;
    using S=rr::platform::OriginalUiGarageWidget1Status50KG;d={};
    if(!W::authorityThread(entitiesOwner_)){d.status=S::authority;d.sourcePC=0x2A9540;return false;}
    const auto dictionary=sourceLocalization32COwner_;
    if(owner.issuer_!=this||!owner.currentPending()||!dictionary||dictionary->issuer_!=this||
       !dictionary->current()||!rr::platform::sameOwner(dictionary,owner.dictionary_)||
       !rr::platform::sameOwner(dictionary,dictionary->issuedOwner_.lock())){
        d.status=S::authority;d.sourcePC=0x2A9540;return false;
    }
    std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText> text;
    if(!borrowLocalizationText18AC60(*dictionary,owner.lookupOrdinal_,text,d.localization)){
        d.status=S::selectedTextPending;d.sourcePC=d.localization.sourcePC;return false;
    }
    return owner.continueCold2A9540(std::move(text),std::move(fonts),d);
}


bool Application::continueGarageWidgetsThrough9_1CD860(rr::platform::OriginalUiGarageWidget1Owner50KG& owner,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    rr::platform::OriginalUiGarageWidget1Diagnostic50KG& d)const noexcept {
    using W=rr::platform::OriginalUiGarageWidget1Owner50KG;using S=rr::platform::OriginalUiGarageWidget1Status50KG;d={};
    if(!W::authorityThread(entitiesOwner_)){d.status=S::authority;d.sourcePC=0x1CD860;return false;}
    const auto dictionary=sourceLocalization32COwner_;
    if(owner.issuer_!=this||owner.retirementStarted_||!owner.identityCurrent()||!owner.fieldsCurrent()||
       !dictionary||dictionary->issuer_!=this||!dictionary->current()||
       !rr::platform::sameOwner(dictionary,owner.dictionary_)||!rr::platform::sameOwner(dictionary,dictionary->issuedOwner_.lock())){
        d.status=S::authority;d.sourcePC=0x1CD860;return false;
    }
    //The existing row1 completion is not a second manager/table construction.
    //Every later static row obtains its own actual root-issued18AC60 borrow.
    while(owner.cursor_<9){
        const auto index=owner.cursor_;auto& row=owner.rows_[index];
        if(owner.rowColdCurrent(index)){
            if(!owner.advanceRow1CDA1C(d))return false;
            continue;
        }
        if(!row.reached){
            if(!owner.prepareCurrentRow1CD860(fonts,d)&&d.status!=S::selectedTextPending)return false;
            if(owner.rowColdCurrent(index))continue;
        }
        if(!owner.rowPendingCurrent(index)){d.status=S::phase;d.sourcePC=0x1CD860;return false;}
        std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText> text;
        if(!borrowLocalizationText18AC60(*dictionary,row.lookupOrdinal,text,d.localization)){
            d.status=S::selectedTextPending;d.sourcePC=d.localization.sourcePC;return false;
        }
        if(!owner.continueStatic2A9540(index,std::move(text),fonts,d))return false;
    }
    if(!owner.currentColdPrefix9()){d.status=S::phase;d.sourcePC=0x1CD860;return false;}
    d={};d.row=9;d.sourcePC=0x1CD860;return true; //BEFORE real Watto10 constructor, not full1CD7A0 return.
}

bool Application::continueGarageWidgetsThrough22_1CD860(rr::platform::OriginalUiGarageWidget1Owner50KG& owner,
    std::shared_ptr<rr::platform::OriginalUiGarageWattoOwner50KG> watto,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    rr::platform::OriginalUiGarageWidget1Diagnostic50KG& d)const noexcept {
    using W=rr::platform::OriginalUiGarageWidget1Owner50KG;using S=rr::platform::OriginalUiGarageWidget1Status50KG;d={};
    if(!W::authorityThread(entitiesOwner_)){d.status=S::authority;d.sourcePC=0x1CD860;return false;}
    const auto dictionary=sourceLocalization32COwner_;
    if(owner.issuer_!=this||owner.retirementStarted_||!owner.identityCurrent()||!owner.fieldsCurrent()||
       !dictionary||dictionary->issuer_!=this||!dictionary->current()||
       !rr::platform::sameOwner(dictionary,owner.dictionary_)||!rr::platform::sameOwner(dictionary,dictionary->issuedOwner_.lock())){
        d.status=S::authority;d.sourcePC=0x1CD860;return false;
    }
    if(owner.coldDatReleased_){d.status=S::phase;d.sourcePC=0x1CDA34;return false;}
    // Continue this exact154 table/cursor, never a second manager/DAT or an
    // arbitrary caller-selected endpoint. Watto cannot be replaced by Page.
    while(owner.cursor_<owner.rows_.size()){
        const auto index=owner.cursor_;auto& row=owner.rows_[index];
        if(owner.rowColdCurrent(index)){
            if(!owner.advanceRow1CDA1C(d))return false;continue;
        }
        if(index==9){
            if(!owner.continueWattoRow1CD8B4(watto,d))return false;continue;
        }
        if(!row.reached){
            if(!owner.prepareCurrentRow1CD860(fonts,d)&&d.status!=S::selectedTextPending)return false;
            if(owner.rowColdCurrent(index))continue;
        }
        if(!owner.rowPendingCurrent(index)){d.status=S::phase;d.sourcePC=0x1CD860;return false;}
        std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText> text;
        if(!borrowLocalizationText18AC60(*dictionary,row.lookupOrdinal,text,d.localization)){
            d.status=S::selectedTextPending;d.sourcePC=d.localization.sourcePC;return false;
        }
        if(!owner.continueStatic2A9540(index,std::move(text),fonts,d))return false;
    }
    return owner.finishCold1CDA34(d);
}

}

namespace rr::platform {
bool OriginalUiGarageWidget1Owner50KG::setPageDrawPosition2AA1D0(
    const Page& page,OriginalUiGarageDrawContribution50KG& draw,std::size_t index,
    std::int32_t x,std::int32_t y,D& d)noexcept {
    d={};d.row=index;
    if(!pageEntryCustodyCurrent(page)||runtimePage_!=&page||index>=rows_.size()||
       !page.drawPending_||page.drawPublication_!=draw.publication_||
       draw.page_.get()!=&page||draw.callback_!=&page.entity_||
       draw.phase_!=decltype(draw.phase_)::thirdPass||!draw.identityCurrent())
        return fail(d,S::authority,0x2AA1D0);
    constexpr std::array<std::uint32_t,6> calls{0x1FCAF8,0x1FCB2C,0x1FCB60,0x1FCB94,0x1FCBC8,0x1FCBFC};
    float baseY=0;
    if(index>=16&&index<=21&&page.drawSourcePC_==calls[index-16]&&x==170)
        baseY=268.f+18.f*static_cast<float>(index-16);
    else if(index==10+page.field4E4_&&page.drawSourcePC_==0x1FD7C0&&x==440)baseY=290.f;
    else if(index==7&&page.drawSourcePC_==0x1FD8A0&&x==385)baseY=245.f;
    else return fail(d,S::source,0x2AA1D0);
    volatile float authored=baseY;
    volatile float actual=authored+page.panelOffset4D8_;
    std::int32_t expectedY=0;
    if(!game::roundOriginalUiTextMetric50KG(actual,1,expectedY)||y!=expectedY)
        return fail(d,S::source,0x2AA1D0);
    auto& r=rows_[index];if(!r.outer)return fail(d,S::source,0x2AA1F4);
    auto& w=*r.outer;w.x180=x;w.y184=y;
    r.drawPositionKnown=true;r.drawX=x;r.drawY=y;
    for(std::size_t line=0;line<static_cast<std::size_t>(w.lineCount168);++line){
        d.line=line;if(!configurePosition(index,line,d))return false;
    }
    d.sourcePC=0x2AA290;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageWidget1Owner50KG::setPageDrawVisibility2A7A00(
    const Page& page,OriginalUiGarageDrawContribution50KG& draw,std::size_t index,bool shown,D& d)noexcept {
    d={};d.row=index;
    if(!pageEntryCustodyCurrent(page)||runtimePage_!=&page||index<10||index>15||
       !page.drawPending_||page.drawPublication_!=draw.publication_||
       draw.page_.get()!=&page||draw.callback_!=&page.entity_||
       draw.phase_!=decltype(draw.phase_)::thirdPass||!draw.identityCurrent())
        return fail(d,S::authority,0x2A7A00);
    const auto pc=shown?0x1FD7D8u:0x1FCC10u+static_cast<std::uint32_t>((index-10)*20u);
    if(page.drawSourcePC_!=pc||(shown&&index!=10+page.field4E4_))return fail(d,S::source,0x2A7A00);
    auto& r=rows_[index];if(!r.outer)return fail(d,S::source,0x2A7A10);
    r.outer->field38=shown?1u:0u;r.selected38=r.outer->field38;
    const auto result=grant_->applyGarageDisplayTextVisibility2A7A00(this,page,index,r.outer->entity,shown);
    if(!result.okay())return fail(d,S::scope,result.sourcePC);
    r.hidden=!shown;d.sourcePC=result.sourcePC;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageWidget1Owner50KG::setPageDrawText2A9FF0(const Page& page,
    OriginalUiGarageDrawContribution50KG& draw,std::size_t index,std::string_view text,D& d)noexcept {
    d={};d.row=index;
    if(!pageEntryCustodyCurrent(page)||runtimePage_!=&page||
       !page.drawPending_||page.drawPublication_!=draw.publication_||
       draw.page_.get()!=&page||draw.callback_!=&page.entity_||
       draw.phase_!=decltype(draw.phase_)::thirdPass||!draw.identityCurrent()||
       !((index==7&&page.drawSourcePC_==0x1FD8AC)||
         (index==10+page.field4E4_&&page.drawSourcePC_==0x1FD7CC)))
        return fail(d,S::authority,0x2A9FF0);
    const auto* spec=textSpec(index);auto& r=rows_[index];
    if(!spec||!spec->dynamic||!r.outer||!r.arrayPublished||r.arrayReleased||text.find('\0')!=text.npos)
        return fail(d,S::source,0x2A9DE0);
    std::array<std::string_view,4> slices{};std::size_t count=0,at=0;
    // Source @ splitting preserves trailing empty line. Preflight actual fixed
    // line count before mutating the retained child array; no new allocation.
    for(;;){if(count>=spec->lines||count>=slices.size())return fail(d,S::source,0x2A9ED0);
        const auto end=text.find('@',at);slices[count++]=text.substr(at,end==text.npos?text.size()-at:end-at);
        if(end==text.npos)break;at=end+1;
    }
    for(std::size_t i=0;i<spec->lines;++i){d.line=i;auto& line=r.lines[i];
        if(!line.owner)return fail(d,S::source,0x2A9F20);
        if(!line.owner->setText29D940(i<count?slices[i]:std::string_view{},d.text)){
            std::fprintf(stderr,"original_garage_text_setter_route route=page_widget row=%zu line=%zu page_pc=%u publication=%llu input_bytes=%zu overlay_status=%u overlay_pc=%u payload_status=%u payload_pc=%u\n",
                index,i,static_cast<unsigned>(page.drawSourcePC_),static_cast<unsigned long long>(draw.publication_),
                i<count?slices[i].size():std::size_t(0),static_cast<unsigned>(d.text.status),static_cast<unsigned>(d.text.sourcePC),
                static_cast<unsigned>(d.text.payload.status),static_cast<unsigned>(d.text.payload.sourcePC));
            return fail(d,S::textOverlay,d.text.sourcePC);
        }
        if(i<count){
            if(!line.owner->setup29DA40(game::OriginalUiTextLineSetup50KG{},d.text))
                return fail(d,S::textOverlay,d.text.sourcePC);
            line.setupTouched=true;line.setupEnabled=true;
        }
    }
    d.sourcePC=0x2A9FE4;return true;
}
}
