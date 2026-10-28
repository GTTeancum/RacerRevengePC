#include "original_race_constructor50la.h"
#include "original_position_stats_adapter50la.h"
#include "../../src/game/original_pause_storage50la.h"

namespace rr::game {
bool OriginalRaceConstructor50LA::passResultsInputPredecessors1A87A0(
    const ::original_position_stats_adapter50LA::Owner& observer,const InputEvent48Z& event,
    const platform::OriginalResultsHostInput50LA& input,OriginalResults104Diagnostic50LA& d)const noexcept {
    using S=OriginalResults104Diagnostic50LA::Status;
    d={};const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;return false;
    };
    if(resultsFeedbackThread_!=std::this_thread::get_id())
        return stop(S::owner,0x1a87a0,"Race input requires its immutable owner thread");
    if(!counted_||rolledBack_||!application_||!profile_||!pause100_||!pause100_->entity_||
       !results104_||!observer.camera_||!observer.manager_||!input.fixedUiCurrent()||
       input.claimedRevision_!=input.revision_||application_->attachmentLock423F58())
        return stop(S::owner,0x1a87a0,"Race input actual synchronous owners unavailable");
    try {
        if(!observer.identityCurrent()||!results104_->endDispatchOwnerCurrent1AB520(*this,observer))
            return stop(S::owner,0x1a87b8,"Race input requires same loaded Bank/current manager");
        const auto mapped=[&](std::uint32_t action,std::uint32_t pc,bool& matches)noexcept {
            if(!input.matchesNativeDefaultAction(action,event,matches))
                return stop(S::owner,pc,"Unsupported or changed native fixed UI mapping; no originalCBC claim");
            return true;
        };
        auto& pause=*pause100_;
        //Actual parent142350 skips disabled/bit3 children, NOT bit2-paused
        //children. Race table3CEE80+64 is1A87A0, never generic142350.
        if((entity_.flags29&1u)&&!(entity_.flags29&8u)){
            //The SAME retained Camera/native manager carries actual flags58.
            //This is a native manager ownership join, not an original B4
            //constructor receipt, stand-in finished flag or caller snapshot.
            const auto& manager=observer.manager_->manager;
            if(!(manager.flags58&1u)){
                const bool helper1B0B80=(manager.flags58&2u)&&!(manager.flags58&1u);
                if(helper1B0B80||(manager.flags58&4u)){
                    if((flags1B0KnownMask_&3u)!=3u)
                        return stop(S::incomplete,0x1a87f8,"Race canonical low2 producer not reached");
                    if(static_cast<std::int32_t>(event.deviceId)<std::int32_t(flags1B0KnownValue_&3u)){
                        if(!(flags1B1KnownMask_&1u))
                            return stop(S::incomplete,0x1a8810,"Race canonical1B1 bit0 unknown");
                        if(!(flags1B1KnownValue_&1u)){
                            bool matches=false;if(!mapped(8,0x1a882c,matches))return false;
                            if(matches&&pause.active200_==0u)
                                return stop(S::incomplete,0x1a8860,"Race Pause-open F8/HUD/store tail not joined");
                        }
                    }
                }
            }
            //1A8890 is unconditional even when initial Pause admission failed.
            if(event.deviceId==pause.controller884_){
                bool matches=false;if(!mapped(8,0x1a88b0,matches))return false;
                if(matches&&pause.active200_==1u)
                    return stop(S::incomplete,0x1a88d4,"Race actual Pause-close tail not joined");
            }
            platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
            if(!profile_->courseRaceSnapshot(selection))
                return stop(S::owner,0x1a8900,"Race actual current Profile11258 unavailable");
            if(selection.subtype==2u){
                if(!(flags1B0KnownMask_&0x10u))
                    return stop(S::incomplete,0x1a890c,"Race canonical1B0 bit4 unknown");
                if(flags1B0KnownValue_&0x10u){
                    bool matches=false;if(!mapped(5,0x1a8928,matches))return false;
                    if(matches)return stop(S::incomplete,0x1a8938,"Race subtype2 source mutation/reset/audio tail not joined");
                    if(!mapped(4,0x1a89a4,matches))return false;
                    if(matches)return stop(S::incomplete,0x1a89d0,"Race subtype2 fade/music/Application transition tail not joined");
                }
            }
        }
        return resultsInputPredecessorTopology50LA(d);
    }catch(...){return stop(S::owner,0x1a87b8,"Race synchronous native manager ownership read failed");}
}

bool OriginalRaceConstructor50LA::resultsInputPredecessorTopology50LA(
    OriginalResults104Diagnostic50LA& d)const noexcept {
    using S=OriginalResults104Diagnostic50LA::Status;
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;return false;
    };
    if(resultsFeedbackThread_!=std::this_thread::get_id())
        return stop(S::owner,0x142350,"Input topology requires actual immutable owner thread");
    if(!counted_||rolledBack_||!application_||!pause100_||!pause100_->entity_||!results104_||
       application_->attachmentLock423F58()||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))
        return stop(S::owner,0x142350,"Input predecessor topology owner unavailable");
    //Native domain preflight before claim; reused at the reached source tail.
    //It proves concrete custody only, never executes an earlier callback twice.
    if((entity_.flags29&1u)&&!(entity_.flags29&8u)){
        //Mandatory1A8A20 ->142350: live typed children in real order. No
        //flat surrogate tree, ownMask test or generic unknown-child success.
        if(entity_.descendants24&0x400u){
            for(auto* child=entity_.firstChild18;child;child=child->next14){
                if(!(child->flags29&1u)||(child->flags29&8u))continue;
                bool knownBase=false;
                if(raceAudio_&&raceAudio_->coldThread_==std::this_thread::get_id()&&
                   raceAudio_->coldApplication_==application_&&raceAudio_->coldSession_==entity_.sessionGeneration&&
                   !raceAudio_->coldRetiring_){
                    for(std::size_t i=0;i<raceGroupCount_&&i<raceGroups_.size();++i)if(raceGroups_[i]==child){
                        const auto& actual=raceAudio_->coldGroups_[static_cast<std::size_t>(groupOrder_[i])];
                        if(&actual.entity!=child||!actual.constructed||!actual.loaded||
                           actual.slot40<0||std::size_t(actual.slot40)>=application_->soundGroups434A10_.size()||
                           application_->soundGroups434A10_[std::size_t(actual.slot40)]!=child)
                            return stop(S::owner,0x1423b4,"Reached SoundGroup is not its actual retained2350D0 owner");
                        //2350FC installs3D1AA0; slot64=142350. Sound list4C
                        //membership is NOT an Entity child and is not cleared.
                        knownBase=true;break;
                    }
                }
                if(!knownBase&&f8Owner_&&f8Source_==f8Owner_.get()&&f8Attached_&&
                   f8Owner_->belongsTo(application_,entity_.sessionGeneration)&&f8Owner_->entity()==child){
                    //1983D0 installs3CE6A0; slot64=142350. ScreenObject
                    //resource arrays are NOT generic Entity descendants.
                    knownBase=true;
                }
                if(!knownBase)return stop(S::incomplete,0x1423b4,
                    "Reached Race child concrete input callback is not represented");
                if(child->firstChild18||child->descendants24)
                    return stop(S::incomplete,0x142350,"Reached typed child has unjoined Entity input descendants");
                //Concrete empty142350 returns0. Load next14 only afterward.
            }
        }
    }
    //The same real Application10 sibling chain continues after Race0.
    //Pause3CF420+64=1BD7C0 cannot be omitted merely because Results is up.
    const auto& p=pause100_->entity_->component;
    if(entity_.next14!=&p||p.next14!=results104_->entity())
        return stop(S::owner,0x1423cc,"Actual Race -> Pause -> Results input order changed");
    if((p.flags29&1u)&&!(p.flags29&8u))
        return stop(S::incomplete,0x1bd7c0,"Earlier active Pause concrete input owns this event; Results not reached");
    d.status=S::complete;d.sourcePC=0x1c8c30;return true;
}
}
