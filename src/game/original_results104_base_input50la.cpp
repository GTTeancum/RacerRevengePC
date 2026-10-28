#include "original_results104_storage50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../platform/linux/native/game_host50er.h"
#include <thread>

namespace rr::game {
bool OriginalResults10450LA::updateBaseInput1B9FE0(const OriginalRaceConstructor50LA& race,
    const platform::GameHost50ER& host,OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1b9fe0;return false;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1b9fe0;return false;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||!p.base||race.results104_.get()!=this||
       !race.counted_||race.rolledBack_||race.application_!=p.application||
       race.entity_.sessionGeneration!=p.component.sessionGeneration||!retirementCurrent()||
       !p.updateCpu||p.updateCpu->prefix!=UpdateCpuPrefix::timers||
       p.updateCpu->nextSourcePC!=0x1c8c10)
        return p.stop(d,Impl::S::owner,0x1b9fe0,"Results same reached timer/base invocation absent");
    auto& out=*p.updateCpu;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* why){
        out.nextSourcePC=pc;return p.stop(d,status,pc,why);
    };
    if(!(race.flags1B0KnownMask_&8u))
        return stop(Impl::S::incomplete,0x1ba008,"Results canonical Race1B0 bit3 unknown");
    if(!(race.flags1B0KnownValue_&8u)){
        p.displayCpu.reset();
        p.mode1F0=0u; //1BA01C, before Profile/device reads.
        p.device1F4=0u; //1BA020.
        out.sourceEffectsCommitted=true;
        if(!race.profile_)
            return stop(Impl::S::owner,0x1ba080,"Results actual Profile2DC absent");
        const auto* profile=race.profile_->commonProfile();
        if(!profile||!profile->known)
            return stop(Impl::S::owner,0x1ba08c,"Results actual Profile1125C unavailable");
        const auto humans=(profile->playerBits1125C>>5)&3u;
        for(std::uint32_t index=0;index<humans;++index){
            // Explicit native adapter at the exact original239C10 read.
            // Never alias feedbackDeviceReady, samplePacket(true), or a host
            // driver/test packet to original6C4. Unknown is NOT unavailable0.
            if(!host.ownerThread()||!host.created()||!host.hasWindow()||
               !host.resultsInput50LA_.current_||index>=host.resultsInput50LA_.available_.size())
                return stop(Impl::S::incomplete,0x1ba044,"Results actual current native Window device observation absent");
            p.nativeAvailabilityPoll=host.resultsInput50LA_.revision_;
            if(!host.resultsInput50LA_.available_[index]){
                if(index==1u)p.device1F4=1u; //1BA060 before1F0.
                p.mode1F0=1u;break;          //1BA06C delay slot.
            }
        }
        if(*p.mode1F0&&p.base->hidden160==1u){
            p.base->transition164=.25f; //1BA124.
            p.base->hidden160=0u;       //1BA12C, not an ack or mode change.
        }
    }
    // Mandatory141ED0: THIS retained Results Entity has no Entity children.
    // retirementCurrent proves both18 and24 empty. Do not manufacture a
    // successful unknown child callback or call the renderer in its place.
    out.prefix=UpdateCpuPrefix::baseReturned;out.nextSourcePC=0x1c8c1c;
    d.status=Impl::S::complete;d.sourcePC=0x1c8c1c;return true;
}
}
