#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "../../src/game/event_dispatch48z.h"
#include "../../platform/linux/native/original_ui_shared_dialogs50kg.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"

namespace original_main_entry50KG {
bool Application::passResultsProcessPredecessors50LA(const rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::InputEvent48Z* event,rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    using S=rr::game::OriginalResults104Diagnostic50LA::Status;
    d={};const auto stop=[&](S s,std::uint32_t pc,const char* why)noexcept {
        d.status=s;d.sourcePC=pc;d.operation=why;return false;
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return stop(S::owner,0x140ef0,"Process input requires Application owner thread");
    if(sourceRace28COwner_.get()!=&race||race.application_!=entitiesOwner_||
       race.resultsFeedbackThread_!=std::this_thread::get_id()||!entitiesOwner_||
       entitiesOwner_->attachmentLock423F58())
        return stop(S::owner,0x140ef0,"Process input actual Race/Application custody changed");
    if(event&&(event->category!=2u||event->transition!=1u||event->deviceId>1u))
        return stop(S::source,0x140ef0,"Process input requires actual fixed native category2 press");
    const auto view=entitiesOwner_->entityAuthority_.view();
    auto& outer=entitiesOwner_->outer26C_;auto* root=entitiesOwner_->application10();
    if(!root||!contains(view,&outer)||!contains(view,root)||!contains(view,&race.entity_)||
       !rr::game::original_entity_chain_detail50LA::forest(view)||outer.firstChild18!=root||
       root->parent1C!=&outer||root->previous10||root->next14||
       !(outer.descendants24&0x400u)||!(root->descendants24&0x400u)||
       !(root->flags29&1u)||(root->flags29&8u))
        return stop(S::owner,0x142350,"Process input actual outer26C -> root10 traversal unavailable");
    // Preserve the earlier packet's bounded no-predecessor domain. The normal
    //process below is not rewritten into this special topology.
    if(root->firstChild18==&race.entity_&&!race.entity_.previous10&&race.entity_.parent1C==root){
        d.status=S::complete;return true;
    }
    const auto light=sourceLightManager426F60Owner_;const auto aural=sourceAural00Owner_;
    const auto scope=sharedUiScopeOwner_;
    if(!light||!light->current()||!aural||!aural->current()||!scope||scope->issuer_!=this||
       scope->application_!=entitiesOwner_||!scope->live().okay()||!scope->dispatcherLive_||
       !nativeSourceAudioDispatcherCurrent(*aural))
        return stop(S::owner,0x142350,"Actual retained Light/Aural/dispatcher prefix unavailable");
    const auto& l=light->entity_;const auto& a=aural->entity_;const auto& dispatcher=scope->dispatcher_;
    const bool attached=race.entity_.parent1C==root;
    if((!attached&&(race.entity_.parent1C||race.entity_.previous10||race.entity_.next14||event))||
       root->firstChild18!=&l||l.parent1C!=root||l.previous10||l.next14!=&a||
       a.parent1C!=root||a.previous10!=&l||a.next14!=&dispatcher||dispatcher.parent1C!=root||
       dispatcher.previous10!=&a||dispatcher.next14!=(attached?&race.entity_:nullptr)||
       (attached&&race.entity_.previous10!=&dispatcher))
        return stop(S::owner,0x142350,"Actual Light -> Aural -> dispatcher -> Race order differs");
    const auto eligible=[](const rr::game::OriginalWreckComponent50LA& c)noexcept {
        return (c.flags29&1u)&&!(c.flags29&8u);
    };
    const auto baseChildren=[&](const rr::game::OriginalWreckComponent50LA& parent)noexcept {
        //142350 tests parent descendants400, not a child's own mask or bit2.
        //Inactive actual children are skipped, with next14 loaded afterward.
        //Any eligible child requires its real callback: never fake a zero.
        if(parent.descendants24&0x400u)
            for(auto* child=parent.firstChild18;child;child=child->next14)
                if(eligible(*child))return stop(S::incomplete,0x1423b4,
                    "Earlier process child input callback is active; Results cannot claim this edge");
        return true;
    };
    //Null event is effects-free native domain preflight BEFORE claiming a poll.
    //With an actual event these are the concrete reached callbacks, in source
    //sibling order. They do not mutate this supported category2 domain.
    if(eligible(l)){
        //147EF4 installs3CD580, +64=148250. Its mandatory142350 comes
        //FIRST at148268, even if a later debug query would consume the event.
        if(!baseChildren(l))return false;
        if(event){
            //1277D0 compares code then category. Category2 cannot match either
            //148280 (4,0,modifier80) or148324 (15,0,modifier80); no modifier
            //device/global, light float, log or toggle field is reached.
            if((event->code==4u&&event->category==0u)||(event->code==15u&&event->category==0u))
                return stop(S::incomplete,0x148280,"Original Light debug-key branch unsupported");
        }
    }
    if(eligible(a)){
        //3CC900+64=103B10: debug query(22,0,80) PRECEDES mandatory142350.
        //Source category mismatch skips A0/267B70, not the base-child tail.
        if(event&&event->code==22u&&event->category==0u)
            return stop(S::incomplete,0x103b4c,"Original Aural debug-key branch unsupported");
        if(!baseChildren(a))return false;
    }
    //1B715C installs3CF310; +64 is literally142350. The persistent two
    //dialogs normally retain Entity flags3: inactivity is NOT Entity-disable.
    //Authenticate the real current process state and each typed retained node.
    if(eligible(dispatcher)&&(dispatcher.descendants24&0x400u)){
        for(auto* child=dispatcher.firstChild18;child;child=child->next14){
            if(!eligible(*child))continue;
            const auto process=state;
            const auto dialogs=process?process->sharedDialogs:nullptr;
            if(!dialogs||dialogs->application_.lock()!=process||dialogs->scope_!=scope||
               scope->sharedDialogsOwner_!=dialogs.get()||!dialogs->current())
                return stop(S::owner,0x1423b4,"Reached dispatcher child lacks actual retained process dialog owner");
            if(child==scope->sharedDialogs_[0]&&child==&dialogs->dialog470_.entity){
                if(dialogs->dialog470_.vtable!=0x3d0910u||
                   !scope->sharedDialogPrefixCurrent(*dialogs,0,*child))
                    return stop(S::owner,0x1423b4,"Actual470 callback ancestry/lifetime changed");
                //3D0910+64 is142350, NOT the active298 update/display body.
                if(!baseChildren(*child))return false;
            }else if(child==scope->sharedDialogs_[1]&&child==&dialogs->controller47C_.entity){
                if(dialogs->controller47C_.vtable!=0x3d0890u||
                   !scope->sharedDialogPrefixCurrent(*dialogs,1,*child))
                    return stop(S::owner,0x2062d0,"Actual47C callback ancestry/lifetime changed");
                const auto& popup=process->controllerPopupState;
                //2062E4/2F0 read3CC FIRST. Inactive skips3D8 and event8;
                //active result!=1 also skips event8. Do not eagerly validate
                //unreached popup timers, input maps, resources or audio state.
                if(popup.active3CC&&popup.result3D8==1u){
                    //Null event is native preclaim policy only. A potentially
                    //consuming47C branch keeps all host edges for its own UI.
                    if(!event||event->deviceId==0u)
                        return stop(S::incomplete,0x206314,
                            "Earlier actual47C confirmation branch owns input; Results not admitted");
                }
                //206400 ALWAYS calls142350, including the inactive branch.
                if(!baseChildren(*child))return false;
            }else return stop(S::incomplete,0x1423b4,
                "Earlier active dispatcher child callback is not represented; edge remains unclaimed");
            //Source1423CC loads next14 only after that actual concrete return.
        }
    }
    d.status=S::complete;d.sourcePC=0x1a87a0;return true;
}
}
