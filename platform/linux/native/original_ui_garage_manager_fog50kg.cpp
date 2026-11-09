#include "original_ui_garage_manager_fog50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::platform {
namespace {
using K=game::OriginalWreckComponentKind50LA;
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
float add(float a,float b)noexcept{volatile float v=a+b;return v;}
float sub(float a,float b)noexcept{volatile float v=a-b;return v;}
float mul(float a,float b)noexcept{volatile float v=a*b;return v;}
}
OriginalUiGarageManagerFog50KG::~OriginalUiGarageManagerFog50KG()noexcept {
    if((bound_&&!payloadReleased_)||(grant_&&grant_->stillBorrowingGarageManagerFogOwner(this)))std::terminate();
}
OriginalUiGarageManagerFog50KG::C* OriginalUiGarageManagerFog50KG::entity(std::size_t i)const noexcept {
    return i==0?(fog_?&fog_->entity:nullptr):i==1?(off_?&off_->entity:nullptr):i==2?(on_?&on_->entity:nullptr):nullptr;
}
bool OriginalUiGarageManagerFog50KG::identityCurrent()const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    return bound_&&!payloadReleased_&&grant_&&grant_->current()&&
        grant_->stillBorrowingGarageManagerFogOwner(this)&&issuer_==grant_->issuer()&&
        lights_&&lights_->current()&&sameOwner(grant_,lights_->grant_)&&issuer_==lights_->issuer_;
}
bool OriginalUiGarageManagerFog50KG::fieldsCurrent()const noexcept {
    const K kinds[]{K::fog13E6B0,K::fogOff13F850,K::fogOn13F980};
    for(std::size_t i=0;i<3;++i){const auto* c=entity(i);
        if(constructed_[i]){
            if(!c||c->kind!=kinds[i]||c->sessionGeneration!=grant_->generation_||c->objectKey!=
               static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c))||c->firstChild18||
               c->reference00!=((retiring_[i]||retired_[i])?0u:1u))return false;
            if(attached_[i]){
                if(c->parent1C!=grant_->manager()||c->mask20!=0xffffffffu||!c->previous10)return false;
                if(retiring_[i]||retired_[i]){
                    if(c->previous10!=previousAtRetire_[i]||c->next14)return false;
                }else if(c->previous10->next14!=c||c->previous10->parent1C!=c->parent1C||
                    (c->next14&&(c->next14->previous10!=c||c->next14->parent1C!=c->parent1C)))return false;
            }else if(c->parent1C||c->previous10||c->next14||c->mask20)return false;
        }else if(managerFields_[i]||attached_[i]||retiring_[i]||retired_[i])return false;
        if(managerFields_[i]&&managerFields_[i]!=c)return false;
    }
    constexpr std::array<std::size_t,3> order{0,2,1};
    if(attachmentCursor_>order.size())return false;
    for(std::size_t i=0;i<order.size();++i)if(attached_[order[i]]!=(i<attachmentCursor_))return false;
    if(attachmentCursor_&&!widgetOwner_)return false;
    if(constructed_[0]){
        if(!constructorKnown_||fog_->field44||fog_->field48!=1||fog_->field4C!=(renderProxyActive_?1u:0u)||fog_->dirty60>1||
           (!fog_->dirty60&&!helperReleased_&&(!helper_||!helper_->packet||!helper_->packet->applied))||
           fog_->far50!=1200||fog_->near54!=900||fog_->gap58!=200)return false;
        if(!helperReleased_&&fog_->helper5C!=helper_.get())return false;
    }
    if(helper_&&(!fog_||!helper_->current(fog_->color30)))return false;
    if(rangeKnown_&&(!fog_||fog_->color30!=0xff0000ffu||fog_->visibility3C!=1||fog_->haziness40!=0||
       fog_->end38!=1200||fog_->start34!=1000))return false;
    if(onBorrowKnown_&&(!on_||!fog_||on_->fog30!=&fog_->entity))return false;
    return true;
}
bool OriginalUiGarageManagerFog50KG::current()const noexcept {
    return (stage_==Stage::complete||stage_==Stage::attaching||stage_==Stage::attached)&&
        identityCurrent()&&fieldsCurrent()&&rendererWritePerformed_&&
        !descriptor_&&renderer_&&renderer_->currentOwnedBy(renderer_,issuer_)&&onBorrowKnown_&&
        managerFields_[0]&&managerFields_[1]&&managerFields_[2]&&!retired_[0]&&!retired_[1]&&!retired_[2]&&!retiring_[0]&&!retiring_[1]&&!retiring_[2];
}
bool OriginalUiGarageManagerFog50KG::currentAttached()const noexcept {
    return stage_==Stage::attached&&attachmentCursor_==3&&current()&&widgetOwner_&&
        grant_->managerFogAttachedCurrent(this,widgetOwner_);
}
bool OriginalUiGarageManagerFog50KG::widgetCustodyCurrentFor(const Widget* widget)const noexcept {
    // Keep the retained source field/payload identity through reverse-tail
    // retirement. Do not pretend an already-retired Fog Entity is live.
    return widget&&(!widgetOwner_||widgetOwner_==widget)&&identityCurrent()&&fieldsCurrent()&&
        rendererWritePerformed_&&!descriptor_&&renderer_&&renderer_->currentOwnedBy(renderer_,issuer_)&&
        onBorrowKnown_&&managerFields_[0]&&managerFields_[1]&&managerFields_[2]&&
        grant_->managerFogWidgetCustodyCurrent(this,widget);
}
bool OriginalUiGarageManagerFog50KG::attachAfterWidgets1CD080(const Widget& widget,Diagnostic& d)noexcept {
    d={};
    if(!current()||!widget.currentColdComplete()||widget.fog_.get()!=this||
       !sameOwner(grant_,widget.grant_)||widget.issuer_!=issuer_||(widgetOwner_&&widgetOwner_!=&widget))
        return fail(d,Status::phase,0x1CD080);
    if(currentAttached()){d.sourcePC=0x1CD14C;return true;}
    widgetOwner_=&widget;stage_=Stage::attaching;
    constexpr std::array<std::size_t,3> order{0,2,1};
    constexpr std::array<std::uint32_t,3> fields{0x1CD08C,0x1CD0C8,0x1CD108};
    while(attachmentCursor_<order.size()){
        const auto i=order[attachmentCursor_];auto* c=entity(i);
        if(!c||managerFields_[i]!=c||attached_[i])return fail(d,Status::phase,fields[attachmentCursor_]);
        const auto step=grant_->attachGarageManagerFog1CD080(this,&widget,i,*c);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        attached_[i]=true;++attachmentCursor_;
    }
    stage_=Stage::attached;d.sourcePC=0x1CD14C;return true;
}
bool OriginalUiGarageManagerFog50KG::colorWriteCurrentFor(const ::original_main_entry50KG::Application* root)const noexcept {
    return root==issuer_&&stage_==Stage::awaitingRenderer&&identityCurrent()&&fieldsCurrent()&&
        constructed_[0]&&!retiring_[0]&&!retired_[0]&&helper_&&rangeKnown_&&descriptorParsed_&&descriptor_&&
        !rendererWritePerformed_&&!managerFields_[0]&&!constructed_[1]&&!constructed_[2];
}
bool OriginalUiGarageManagerFog50KG::prepare1CCFA0(std::shared_ptr<Grant> grant,
    std::shared_ptr<Lights> lights,std::shared_ptr<Lease> lease,Diagnostic& d)noexcept {
    d={};if(bound_||stage_!=Stage::empty)return fail(d,Status::phase,0x1CCFA0);
    if(!grant||!grant->current()||!lights||!lights->current()||!sameOwner(grant,lights->grant_)||
       lights->lights_[1]->entity.next14||!lights->assets_||!lights->assets_->loaded()||
       !lights->provider_||!lights->provider_->sealed()||lights->provider_->revision()!=lights->providerRevision_||
       (lease&&!lease->currentOwnedBy(lease,grant->issuer()))||std::fegetround()!=FE_TONEAREST)return fail(d,Status::input,0x1CCFA0);
    try {
        const auto member=lights->assets_->archive()->find("UI/UIGarage/ui.dat");
        assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
        if(!member.identity||!member.data||lights->ui_.identity()!=member.identity||
           lights->provider_->lookup132820(member.identity->name,mounted)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||
           !mounted.data||mounted.size!=member.size||std::memcmp(mounted.data,member.data,member.size))return fail(d,Status::source,0x132820);
    }catch(...){return fail(d,Status::allocation,0x132820);}
    auto step=grant->bindGarageManagerFogPayload(this,lights.get());if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    grant_=std::move(grant);lights_=std::move(lights);issuer_=grant_->issuer();bound_=true;stage_=Stage::bound;
    std::uint32_t pc=0x1CE7C4;
    try {
        descriptor_=std::make_unique<Descriptor>();stage_=Stage::descriptor;
        descriptor_->rgb00=0x008080c0u;descriptor_->visibility04=1;descriptor_->haziness08=.5f;
        const auto* section=lights_->ui_.section("Fog");
        // Authenticated typo Visibilty is NOT the Visibility key. This reached
        // path uses default1; original Haziness range bug rechecks that1.
        if(!section||section->find("Visibility")||!section->find("Visibilty")||
           section->find("Visibilty")->text!="1"||!section->find("ColorRGB")||
           section->find("ColorRGB")->text!="0,0,255")return fail(d,Status::source,0x1CE7F4);
        pc=0x1CE81C;
        if(!assets::originalUiNumber50KG(*section,"Visibility",1,descriptor_->visibility04,d.parse,pc)||
           descriptor_->visibility04!=1)return fail(d,Status::source,pc);
        pc=0x1CE8BC;
        if(!assets::originalUiNumber50KG(*section,"Haziness",.5f,descriptor_->haziness08,d.parse,pc)||
           descriptor_->haziness08!=0)return fail(d,Status::source,pc);
        descriptor_->rgb00=0x000000ffu;descriptorParsed_=true; // actual integer RGB pack1CEB20
        pc=0x1CE994;fog_=std::make_unique<Fog>();
        step=grant_->constructGarageManagerFogPrefix(this,0,fog_->entity);if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        constructed_[0]=true;fog_->field44=0;fog_->field48=1;fog_->field4C=0;fog_->dirty60=1;fog_->helper5C=nullptr;
        constructorKnown_=true;stage_=Stage::fog;
        //141C20 is literally return-this. These load stores precede allocation.
        fog_->far50=1200;fog_->near54=900;fog_->gap58=200;
        pc=0x13E7D0;helper_=std::make_unique<Primitive>();fog_->helper5C=helper_.get();stage_=Stage::helper;
        fog_->color30=descriptor_->rgb00|0xff000000u;fog_->visibility3C=descriptor_->visibility04;fog_->haziness40=descriptor_->haziness08;
        fog_->end38=add(fog_->near54,mul(fog_->visibility3C,sub(fog_->far50,fog_->near54)));
        fog_->start34=mul(sub(1,fog_->haziness40),sub(fog_->end38,fog_->gap58));
        if(!std::isfinite(fog_->end38)||!std::isfinite(fog_->start34))return fail(d,Status::arithmetic,0x13E89C);
        rangeKnown_=true;stage_=Stage::awaitingRenderer;
    }catch(...){return fail(d,Status::allocation,pc);}
    return continue1CCFA0(std::move(lease),d);
}
bool OriginalUiGarageManagerFog50KG::continue1CCFA0(std::shared_ptr<Lease> lease,Diagnostic& d)noexcept {
    d={};if(stage_!=Stage::awaitingRenderer||!identityCurrent()||!fieldsCurrent())return fail(d,Status::phase,0x13E8A8);
    if(!lease||!lease->currentOwnedBy(lease,issuer_))return fail(d,Status::renderer,0x13E8A8);
    if(!rendererWritePerformed_){
        if(!lease->store13E8A8(*this))return fail(d,Status::renderer,0x13E8A8);
        renderer_=std::move(lease);rendererWritePerformed_=true;fog_->dirty60=1; //13E8B0, after actual renderer store
    }else if(!sameOwner(renderer_,lease))return fail(d,Status::renderer,0x13E8A8);
    managerFields_[0]=&fog_->entity;
    auto step=grant_->publishGarageManagerFogField(this,0,fog_->entity);if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    stage_=Stage::fogPublished;descriptor_.reset(); //1CEA00, not a forged source manager null write
    std::uint32_t pc=0x1CCFD8;
    try {
        off_=std::make_unique<Off>();step=grant_->constructGarageManagerFogPrefix(this,1,off_->entity);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        constructed_[1]=true; //13F890 ->141C20 has no further effects.
        managerFields_[1]=&off_->entity;step=grant_->publishGarageManagerFogField(this,1,off_->entity);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        stage_=Stage::offPublished;pc=0x1CD020;on_=std::make_unique<On>();
        step=grant_->constructGarageManagerFogPrefix(this,2,on_->entity);if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        constructed_[2]=true;on_->fog30=&fog_->entity;onBorrowKnown_=true; //13F9DC, no source retain
        managerFields_[2]=&on_->entity;step=grant_->publishGarageManagerFogField(this,2,on_->entity);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        stage_=Stage::complete;d={Status::complete,0x1CD064};return true;
    }catch(...){return fail(d,Status::allocation,pc);}
}
bool OriginalUiGarageManagerFog50KG::retireOne142590(std::size_t i,Diagnostic& d)noexcept {
    auto* c=entity(i);if(!c||!constructed_[i]||retired_[i])return fail(d,Status::phase,0x142590);
    if(!retiring_[i]){
        auto step=grant_->preflightGarageManagerFogRetirement(this,i,*c);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        previousAtRetire_[i]=c->previous10;
        step=grant_->beginGarageManagerFogRetirement(this,i,*c);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        retiring_[i]=true;
    }
    if(i==0){
        //133FA0 releases the actual owned buffer20 if present. Native244A40
        //copied its fog uniform before returning; no GPU packet borrow remains.
        helper_.reset();helperReleased_=true;
    }
    const auto step=grant_->finishGarageManagerFogRetirement(this,i,*c);
    if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    retiring_[i]=false;retired_[i]=true;return true;
}
bool OriginalUiGarageManagerFog50KG::retireAttached1CD080(Diagnostic& d)noexcept {
    d={};if(payloadReleased_||!attachmentCursor_)return true;
    if(!identityCurrent()||!fieldsCurrent())return fail(d,Status::phase,0x142590);
    // Actual append order0,2,1 gives reverse tails1,2,0. Any later Scene
    // children must already have retired. Detached construction prefixes are
    // deliberately left for the post-widget enclosing failed-prefix cleanup.
    constexpr std::array<std::size_t,3> reverse{1,2,0};
    for(const auto i:reverse)if(attached_[i]&&!retired_[i]&&!retireOne142590(i,d))return false;
    d.sourcePC=0x1413E0;return true;
}
bool OriginalUiGarageManagerFog50KG::retireDetached1CCFA0(Diagnostic& d)noexcept {
    d={};if(payloadReleased_)return true;
    if(!identityCurrent()||!fieldsCurrent())return fail(d,Status::phase,0x142590);
    for(std::size_t i=0;i<3;++i)if(attached_[i]&&!retired_[i])return fail(d,Status::phase,0x142590);
    // Native failed-prefix cleanup of never-attached fields, after widgets.
    for(std::size_t n=3;n>0;--n){const auto i=n-1;
        if(constructed_[i]&&!retired_[i]&&!retireOne142590(i,d))return false;
    }
    const auto step=grant_->noteGarageManagerFogPayloadReleased(this);
    if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    descriptor_.reset();renderer_.reset();payloadReleased_=true;stage_=Stage::retired;
    d.sourcePC=0x1413E0;return true;
}
OriginalUiGarageManagerFog50KG::Observation OriginalUiGarageManagerFog50KG::inspect()const noexcept {
    Observation o;o.stage=stage_;o.attached=attached_;o.retired=retired_;o.attachmentCursor=attachmentCursor_;o.descriptorAllocated=bool(descriptor_);o.descriptorParsed=descriptorParsed_;
    o.constructorKnown=constructorKnown_;o.rangeKnown=rangeKnown_;o.helperAllocated=bool(helper_);
    o.helperEmpty=helper_&&!helper_->buffer20&&!helper_->count24&&!helper_->count28&&helper_->field2C==-1;
    o.rendererWritePerformed=rendererWritePerformed_;o.payloadReleased=payloadReleased_;
    for(std::size_t i=0;i<3;++i){if(constructed_[i]&&!retired_[i])o.live[i]=entity(i);
        o.managerFieldIdentities[i]=reinterpret_cast<std::uintptr_t>(managerFields_[i]);}
    if(fog_&&constructorKnown_){o.field44=fog_->field44;o.field48=fog_->field48;o.field4C=fog_->field4C;o.dirty60=fog_->dirty60;
        o.far50=fog_->far50;o.near54=fog_->near54;o.gap58=fog_->gap58;}
    if(fog_&&rangeKnown_){o.color30=fog_->color30;o.start34=fog_->start34;o.end38=fog_->end38;
        o.visibility3C=fog_->visibility3C;o.haziness40=fog_->haziness40;}
    o.fogOnBorrowKnown=onBorrowKnown_;if(onBorrowKnown_)o.fogOnBorrow30=reinterpret_cast<std::uintptr_t>(on_->fog30);return o;
}
}
