#include "original_results104_storage50la.h"
#include "original_ui_text_position50kg.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_ui_main_entry50kg.h"
#include <algorithm>
#include <bit>
#include <cfenv>
#include <cmath>

namespace rr::game {
OriginalResults10450LA::Slot48CpuPrefix OriginalResults10450LA::captureNativeSlot48Cpu1C17A0(
    const ::original_main_entry50KG::Application& root,const OriginalRaceConstructor50LA& race,
    const ::original_camera_session50KX::Session& camera,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    using P=Slot48CpuPrefix;d={};
    if(root.garageAudioOwnerThread_!=std::this_thread::get_id()||
       race.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_||
       impl_->nativeCaptureThread!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c17a0;return P::none;
    }
    auto& p=*impl_;
    const auto reject=[&](std::uint32_t pc,const char* why){
        p.stop(d,Impl::S::owner,pc,why);return P::none;
    };
    if(root.sourceRace28COwner_.get()!=&race||root.entitiesOwner_!=p.application||
       root.riderProfile!=race.profile_||root.nativeTaRootRace_!=&race||
       root.nativeTaRootGeneration_!=p.component.sessionGeneration||root.nativeTaRootEntered_||
       root.nativeTaRootAborted_||root.sourceApplicationMode22C_!=0||root.sourceUpdateRenderBypass228Bit3_||
       race.results104_.get()!=this||race.application_!=p.application||!race.counted_||race.rolledBack_||
       race.entity_.sessionGeneration!=p.component.sessionGeneration||!constructed()||!p.loadComplete||
       !p.base||!p.derived||!retirementCurrent()||!race.ownsNativeCameraAdapter34(camera))
        return reject(0x1c17a0,"Results slot48 exact completed native Root/Race/camera owner absent");
    if(!root.resultsPublishedRoot50LA(race,d))return P::none;
    const auto timing=root.nativeTaRootTiming_;
    original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!timing.readPrepared(sample,publication)||publication!=root.nativeTaRootPublication_||
       !p.updateCpu||publication!=p.updateCpu->publication||
       p.updateCpu->prefix!=UpdateCpuPrefix::baseReturned||p.updateCpu->nextSourcePC!=0x1c8c1c||
       publication<=p.lastNativeSlot48AttemptPublication)
        return reject(0x1c17a0,"Results slot48 exact unreplayed completed update publication absent");
    const auto current=[&]()noexcept {
        original_ui_source_timing50LH::Sample now;std::uint64_t serial=0;
        return root.sourceRace28COwner_.get()==&race&&!root.nativeTaRootEntered_&&!root.nativeTaRootAborted_&&
            root.sourceApplicationMode22C_==0&&!root.sourceUpdateRenderBypass228Bit3_&&
            root.nativeTaRootPublication_==publication&&timing.sameOwner(root.nativeTaRootTiming_)&&
            timing.readPrepared(now,serial)&&serial==publication&&now.timestamp==sample.timestamp&&
            now.deltaMilliseconds==sample.deltaMilliseconds&&
            std::bit_cast<std::uint32_t>(now.raw250)==std::bit_cast<std::uint32_t>(sample.raw250)&&
            std::bit_cast<std::uint32_t>(now.updateSeconds)==std::bit_cast<std::uint32_t>(sample.updateSeconds)&&
            root.riderProfile==race.profile_&&root.entitiesOwner_==p.application&&retirementCurrent()&&
            race.ownsNativeCameraAdapter34(camera);
    };
    if(!current())return reject(0x1c17a0,"Results slot48 root changed before source effects");
    p.lastNativeSlot48AttemptPublication=publication;
    p.nativeDisplay.reset(); //No stale successful capture survives a reset/failure branch.
    const auto layout=prepareSlot48LayoutCpuPrefix1C17A0(race,observer,d);
    if(layout!=P::renderGate||!p.slot48Cpu||p.slot48Cpu->interrupted||
       p.slot48Cpu->nextSourcePC!=0x1c2124)return layout;
    auto& out=*p.slot48Cpu;
    // From here a failed native join must not replay earlier layout/name effects.
    out.interrupted=true;
    if(!current())return reject(0x1c2124,"Results slot48 root changed after layout");
    std::uint8_t flags=0;
    // Counted B4 certifies bit0 known and reads the actual manager byte once.
    // No other returned bit is interpreted; this is NOT native ALL8 authority.
    if(!race.readResultsManagerFlags58(camera,flags,d))return out.prefix;
    bool enterShared=(flags&1u)!=0;
    if(!enterShared){
        // Source short circuit: Profile is not fetched on the bit0-set path.
        const auto* profile=race.profile_->commonProfile();
        if(!profile||!profile->known)return reject(0x1c2148,"Results actual Profile mode34 unavailable");
        if(profile->mode34==0){
            std::uint32_t subtype=0;
            if(!race.profile_->resultsSubtype11258(subtype))
                return reject(0x1c215c,"Results actual Profile subtype11258 unavailable");
            enterShared=subtype==3u;
        }
    }
    if(!current())return reject(0x1c2170,"Results slot48 owner changed after eligibility reads");
    if(!enterShared){
        // Only the actual1C2184 branch performs these three source writes.
        p.base->opening30=.25f;p.derived->acknowledgement20C=0;p.derived->field390=0;
        out.prefix=P::resetBranch;out.nextSourcePC=0x1c2198;out.interrupted=false;
        p.stop(d,Impl::S::incomplete,0x1c2198,"Results slot48 reset CPU branch returned; no render admission");
        return out.prefix;
    }
    out.nextSourcePC=0x1c2170;
    captureNativeSingleHumanBaseCpu1B9220(root,race,observer,d);
    if(!p.nativeDisplay||!p.nativeDisplay->boundedSharedReturned)return out.prefix;
    out.prefix=P::sharedBaseCpu;out.nextSourcePC=0x1c2198;out.interrupted=false;
    p.stop(d,Impl::S::incomplete,0x1c2198,
        "Results slot48 shared CPU branch returned; source last-camera/Frame/GPU lifetime still unjoined");
    return out.prefix;
}

OriginalResults10450LA::Slot48CpuPrefix OriginalResults10450LA::prepareSlot48LayoutCpuPrefix1C17A0(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    using P=Slot48CpuPrefix;d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c17a0;return P::none;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1c17a0;return P::none;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||!p.base||!p.derived||!p.icons210||!p.titles490||
       !p.context||!p.context->live()||!p.sourceData||!p.sourceData->sourceLoaded()||
       !retirementCurrent()||race.results104_.get()!=this||!race.counted_||race.rolledBack_||
       race.application_!=p.application||race.entity_.sessionGeneration!=p.component.sessionGeneration||
       !p.application->entityAuthority_.canRetire(race.entity_)||!race.profile_||!race.profile_->initialized()){
        p.stop(d,Impl::S::owner,0x1c17a0,"Results slot48 layout requires its actual retained Race/Profile/resources");return P::none;
    }
    // A native failure after partial1C9B10 stores has no recovered resumable
    //callee. Fail closed on a retry instead of erasing/replaying those effects.
    if(p.slot48Cpu&&p.slot48Cpu->interrupted){
        p.stop(d,Impl::S::incomplete,p.slot48Cpu->nextSourcePC,
            "Results interrupted layout invocation cannot replay its partial source effects");return p.slot48Cpu->prefix;
    }
    p.slot48Cpu.emplace();auto& out=*p.slot48Cpu;
    bool effectsReached=false;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* why){
        out.nextSourcePC=pc;out.interrupted=effectsReached;p.stop(d,status,pc,why);return out.prefix;
    };
    if(!p.dirtyLayout208)return stop(Impl::S::incomplete,0x1c17b4,"Results208 has no reached mode-change producer; not a render-disable flag");
    const auto stamp=race.profile_->stamp();
    const auto current=[&](){const auto now=race.profile_->stamp();
        return p.context->live()&&now.generation==stamp.generation&&now.mode34==stamp.mode34;};
    const auto nameQuery=[&](std::uint32_t caller){
        out.nextSourcePC=caller;effectsReached=true;
        //The concrete first-human method ends at the actual1CA314 epilogue.
        //Only that exact returned CPU prefix allows the enclosing source call
        //to continue. No record insertion, earned credit or render is implied.
        const auto result=prepareNameEntryCpu1C9B10(race,observer,d);
        if(result!=NameEntryCpuPrefix::recordQueries||!p.nameEntryCpu||
           p.nameEntryCpu->nextSourcePC!=0x1ca314||d.status!=Impl::S::incomplete||!current()){
            out.nextSourcePC=d.sourcePC?d.sourcePC:caller;out.interrupted=true;return false;
        }
        return true;
    };
    const auto localize=[&](Impl::Text& text,std::size_t id,std::uint32_t pc){
        out.nextSourcePC=pc;
        if(!text.string8){stop(Impl::S::owner,pc,"Results owned layout Text backing absent");return false;}
        if(!p.localized(text,id,d)){out.nextSourcePC=d.sourcePC;out.interrupted=true;return false;}
        return true;
    };
    try {
        if(*p.dirtyLayout208){
            p.displayCpu.reset();const auto mode=p.derived->mode200;
            if(mode==1u||mode==2u){
                effectsReached=true;
                p.position((*p.icons210)[0],462.f,382.f);
                if(!localize((*p.titles490)[0],mode==1u?8u:9u,mode==1u?0x1c1874:0x1c1e14))return out.prefix;
                p.position((*p.titles490)[0],495.f,389.f);
                p.position((*p.icons210)[2],260.f,382.f);
                if(!localize((*p.titles490)[2],401,mode==1u?0x1c1a40:0x1c1fe0))return out.prefix;
                p.position((*p.titles490)[2],293.f,389.f);
            }else if(mode==3u||mode==4u){
                if(p.derived->previous204!=3u&&!nameQuery(0x1c1b88))return out.prefix;
                effectsReached=true;p.position((*p.icons210)[0],514.f,382.f);
                if(!localize((*p.titles490)[0],4,0x1c1c3c))return out.prefix;
                p.position((*p.titles490)[0],545.f,389.f);
            }
        }
        //1C2108 executes EVEN when208 was already zero or mode was other.
        effectsReached=true;p.dirtyLayout208=0u;out.prefix=P::dirtyLayout;out.nextSourcePC=0x1c210c;
        if(p.derived->fieldAE0){
            if(!nameQuery(0x1c2118))return out.prefix;
            p.derived->fieldAE0=0u; //Only AFTER that exact callee returned.
        }
        out.prefix=P::afterRefreshGate;
        if(!current())return stop(Impl::S::owner,0x1c2124,"Results Profile changed after slot48 layout effects");
        out.prefix=P::renderGate;out.nextSourcePC=0x1c2124;
        p.stop(d,Impl::S::incomplete,0x1c2124,
            "Results layout returned; actual RaceB4 flags58/Profile gate, last-camera and renderer remain unjoined");
        return out.prefix;
    }catch(...){return stop(Impl::S::allocation,out.nextSourcePC,"Results layout allocation failed; reached source effects retained");}
}

namespace {
float layoutAdd(float a,float b)noexcept {volatile float r=a+b;return r;}
float layoutSub(float a,float b)noexcept {volatile float r=a-b;return r;}
float layoutMul(float a,float b)noexcept {volatile float r=a*b;return r;}
float layoutDiv(float a,float b)noexcept {volatile float r=a/b;return r;}
}
bool OriginalResults10450LA::Impl::advanceBaseGeometryCpu1B9324(float actual250,D& d)noexcept {
    //Called only inside the future concrete native Application/Frame borrow.
    //This typed body supplies no public delta setter or last-camera bypass.
    if(!loadComplete||!base||!context||!context->live()||!center40||!backdropsA0||depth!=0xffffffu)
        return stop(d,S::owner,0x1b9324,"Results actual shared-base resources absent");
    const bool needsElapsed=base->opening30>0.f||(base->opening30<0.f&&base->transition164>0.f);
    if(std::fegetround()!=FE_TONEAREST||
       (needsElapsed&&(!std::isfinite(actual250)||actual250<0.f))||
       !std::isfinite(base->opening30)||!std::isfinite(base->transition164))
        return stop(d,S::source,0x1b9324,"Results bounded native shared-base arithmetic domain");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return stop(d,S::source,0x1b9324,"Results shared geometry requires strict native floating point");
#endif
    for(std::size_t i=0;i<2;++i){const auto& poly=polygons[i];
        if(!poly||poly->type0C!=1||poly->count04!=4||!poly->vertices00||!poly->packets44)
            return stop(d,S::owner,0x1b9324,"Results actual kind1 four-vertex shared polygon absent");
        for(std::size_t v=0;v<4;++v)if(poly->vertices00[v].stq14!=&poly->packets44[0].vertices[v].stq)
            return stop(d,S::owner,0x1b9324,"Results polygon authored/STQ alias changed");
    }
    if(!center40->buffer58||!(*backdropsA0)[0].buffer58||!(*backdropsA0)[1].buffer58)
        return stop(d,S::owner,0x1b9324,"Results actual shared texture packets absent");
    const auto animate=[&](float progress){
        const float scaled=layoutMul(128.f,progress);
        const float extent=std::min(128.f,scaled); //Source MIN.S, no invented lower clamp.
        const float top=layoutSub(196.f,extent),bottom=layoutAdd(196.f,extent);
        const float height=layoutSub(bottom,top),upperY=layoutSub(top,16.f);
        if(!std::isfinite(top)||!std::isfinite(bottom)||!std::isfinite(height)||!std::isfinite(upperY))return false;
        auto& center=*center40;
        //Source individual setters and packet Z stores, not a Pause owner.
        position(center,0.f,top);dimension(center,center.height38,height);center.field40=1.f;
        for(auto& vertex:center.buffer58->vertices)vertex.xyz[2]=16777214.f;
        position((*backdropsA0)[0],0.f,upperY);position((*backdropsA0)[1],0.f,bottom);
        return true;
    };
    const auto polygonY=[&](std::size_t i,std::size_t v,float y){
        auto& poly=*polygons[i];auto& vertex=poly.vertices00[v];
        vertex.authored00[1]=y;*vertex.stq14=vertex.authored00;poly.dirty3C=1;
    };
    const auto countdown=[&](float& value){
        if(value>0.f){value=layoutSub(value,actual250);if(value<=0.f)value=0.f;}
        else if(value==0.f)value=-1.f;
    };
    if(base->opening30>=0.f){
        const float progress=layoutSub(1.f,layoutDiv(base->opening30,.25f));
        if(!animate(progress))return stop(d,S::source,0x1b9368,"Results opening geometry outside native finite domain");
        const float y=layoutSub(448.f,layoutMul(65.f,progress));
        if(!std::isfinite(y))return stop(d,S::source,0x1b95e0,"Results opening polygon ordinate outside native domain");
        polygonY(0,0,y);polygonY(0,1,y);countdown(base->opening30);
    }else if(base->transition164>=0.f){
        const float ratio=layoutDiv(base->transition164,.25f);
        const float progress=base->hidden160?ratio:layoutSub(1.f,ratio);
        if(!animate(progress))return stop(d,S::source,0x1b97e0,"Results transition geometry outside native finite domain");
        const float y=layoutMul(65.f,layoutSub(1.f,progress));
        if(!std::isfinite(y))return stop(d,S::source,0x1b9a80,"Results transition polygon ordinate outside native domain");
        polygonY(1,2,y);polygonY(1,3,y);countdown(base->transition164);
    }
    //1B9BF4..9C20 executes even when neither countdown branch ran.
    for(auto& poly:polygons){poly->field18=1.f;poly->dirty3C=1;}
    return true; //Only this private numeric body, not1B9220/slot48 completion.
}

bool OriginalResults10450LA::Impl::projectPolygonCpu2E2398(std::size_t index,
    std::uint32_t caller,D& d)noexcept {
    if(nativeDisplay&&nativeDisplay->building){
        if(nativeDisplay->thread!=std::this_thread::get_id()||!application)
            return stop(d,S::owner,caller,"Results native polygon occurrence owner thread");
        if(!application->apertureRenderEnabled383680_)return true; //Independent source gate before projection.
    }
    if(index>=2||depth!=0xffffffu||!polygons[index]||!context||!context->live())
        return stop(d,S::owner,caller,"Results actual shared polygon owner absent");
    auto& poly=*polygons[index];
    if(poly.type0C!=1||poly.count04!=4||poly.count08!=4||poly.count34!=1||poly.count48!=1||
       poly.capacity4C!=1||poly.scale10!=1.f||poly.field14!=0.f||poly.field18!=1.f||poly.flags40!=3||
       !poly.vertices00||!poly.packets44||!poly.primitives30||!poly.primitives30[0]||
       poly.primitives30[0]->reference!=3u+index||poly.primitives30[0]->packet10!=poly.packets44.get())
        return stop(d,S::source,caller,"Results bounded kind1 polygon representation");
    const auto* ref=references.reference(poly.primitives30[0]->reference);
    if(!ref||!ref->retained)return stop(d,S::material,caller,"Results polygon lost its own material association");
    auto packet=poly.packets44[0];
    for(std::size_t i=0;i<4;++i){auto& vertex=poly.vertices00[i];
        if(vertex.stq14!=&poly.packets44[0].vertices[i].stq||vertex.rgba1C!=&poly.packets44[0].vertices[i].rgba||
           vertex.xyz24!=&poly.packets44[0].vertices[i].xyz)
            return stop(d,S::owner,caller,"Results polygon packet aliases changed");
        if(poly.dirty3C){
            std::int32_t x=0,y=0;
            if(!roundOriginalUiTextMetric50KG(vertex.authored00[0],poly.scale10,x)||
               !roundOriginalUiTextMetric50KG(vertex.authored00[1],poly.scale10,y))
                return stop(d,S::source,caller,"Results bounded source polygon rounding domain");
            packet.vertices[i].stq[0]=float(x);packet.vertices[i].stq[1]=float(y);
            packet.vertices[i].stq[2]=16777214.f;
        }
        for(std::size_t lane=0;lane<3;++lane)if(!std::isfinite(packet.vertices[i].stq[lane]))
            return stop(d,S::source,caller,"Results finite polygon STQ domain");
        if(packet.vertices[i].stq[2]!=16777214.f)
            return stop(d,S::source,caller,"Results polygon retained depth");
    }
    if(poly.dirty3C){
        for(std::size_t i=0;i<4;++i)poly.vertices00[i].authored00[2]=16777214.f;
        poly.packets44[0]=packet;poly.dirty3C=0;
    }
    //Original kind1 is Gouraud triangle-strip/TME0. The native renderer will
    //consume owned STQ coordinates; preserve untouched XYZ and fourth lanes.
    //This does NOT run2E21D0's mode/global gates, packet headers or246760.
    //The separate2E1980 destructor drains renderer20(1)/24 before freeing;
    //native GPU custody must join that retirement, not invent a draw drain.
    return !nativeDisplay||!nativeDisplay->building||copyNativePolygon2E21D0(index,caller,d);
}
}
