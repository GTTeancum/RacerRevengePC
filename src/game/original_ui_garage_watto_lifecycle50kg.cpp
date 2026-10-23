#include "original_ui_garage_watto_lifecycle50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>
namespace rr::game {
namespace {
using E=OriginalUiGarageWattoLifecycleError50KG;
using D=OriginalUiGarageWattoLifecycleDiagnostic50KG;
using R=OriginalUiGarageWattoReceipt50KG;
using O=OriginalUiGarageWattoOperation50KG;
bool fail(D& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
std::uint32_t positiveSourceFloatInteger(std::uint32_t value) noexcept {
    // Positive CVT.S.W in the source truncating mode. Same narrow conversion
    // as rider voice choice, not host nearest rounding of a large integer.
    unsigned shift=0;for(auto n=value;n>=0x01000000u;n>>=1)++shift;
    return shift?value&~((std::uint32_t{1}<<shift)-1u):value;
}
bool complete(const OriginalUiGarageWattoBackend50KG& b) noexcept {
    return b.preflight&&b.bindObserver1D4800&&b.baseEntry2A78C0&&b.notifyObserver28EA20&&b.baseTick141ED0&&
        b.stopAllResponses&&b.playResponse&&b.responseDuration&&b.playWing&&b.drainDetach;
}
bool finiteGraph(const SceneGraph49P& graph) noexcept {
    if(!validateSceneGraph49P(graph)||!graph.count||std::fegetround()!=FE_TONEAREST)return false;
    for(std::uint32_t n=0;n<graph.count;++n)for(unsigned i=0;i<16;++i)
        if(!std::isfinite(graph.nodes[n].localC0.v[i])||!std::isfinite(graph.nodes[n].world100.v[i]))return false;
    return true;
}
}
struct OriginalUiGarageWattoLifecycle50KG::Storage {
    std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets;
    const assets::MotionNodeBindings49O* bindings=nullptr;SceneGraph49P graph{};
    ModelMotionSharedState49T* motionShared=nullptr;ModelMode0IkShared50KG* ikShared=nullptr;
    CoupledNoiseContext49M* noise=nullptr;SharedRandomOwner50ID* random=nullptr;HostMonotonicClock50ID* clock=nullptr;
    OriginalUiGarageWattoBackend50KG backend{};
    std::array<ModelPlaybackMotion50KG,32> descriptors{};
    ModelMotionMask50KG mask{};ModelPlayback50KG playback{};ModelPlaybackWorkspace50KG workspace{};
    ModelPlaybackIk50KG ik{};OriginalUiGarageWattoFields50KG fields{};
    int current=-1;bool entered=false,poisoned=false,externalEffects=false;
    bool poison(D& d,E e,std::uint32_t pc) noexcept {poisoned=true;return fail(d,e,pc);}
    bool check(O op,D& d) noexcept {
        if(poisoned)return fail(d,E::poisoned,0x1D4B10);
        if(!finiteGraph(graph))return fail(d,E::input,0x1D4B10);
        return backend.preflight(backend.context,op,*assets,*bindings,graph)||fail(d,E::preflight,0x1D4B10);
    }
    int index(const char* name) const noexcept {
        for(unsigned i=0;i<32;++i)if(assets->motion(i)->name==name)return static_cast<int>(i);
        return -1;
    }
    int speechMotion(int seconds,bool leaving) const noexcept {
        // No allocation or runtime file-name resolution. The complete original
        // descriptor set was admitted before any possible speech side effect.
        if(!leaving&&seconds==5)seconds=4;
        char name[16]{};const char* side=fields.sideA8?"rt":"lt";
        unsigned p=0;name[p++]='w';if(leaving)name[p++]='p';name[p++]='_';name[p++]=side[0];name[p++]=side[1];
        name[p++]='_';if(seconds<0||seconds>9)return -1;name[p++]=static_cast<char>('0'+seconds);name[p++]='s';
        return index(name);
    }
    bool choice(std::uint32_t first,std::uint32_t count,std::uint32_t& event,R& r,D& d) noexcept {
        if(!random->seed.lazy80A8){random->seed.seed3D6B30=0;random->seed.lazy80A8=true;}
        if(!random->seed.seed3D6B30){
            std::uint32_t timestamp=0;
            if(!sampleHostTimestamp50ID(*clock,timestamp))return poison(d,E::clock,0x101A50);
            random->seed.seed3D6B30=timestamp;seedOriginalSharedRng50IU(*noise,timestamp);r.seeded=true;
        }
        const auto raw=drawOriginalSharedRng50IU(*noise);++r.draws308030;
        // Exact integer floor for the positive source multiply/truncation.
        // Count<=17; each possible integer boundary is exactly representable.
        const auto selected=static_cast<std::uint32_t>((std::uint64_t{positiveSourceFloatInteger(raw)}*count)>>31);
        if(!random->range.lazy80AC){random->range.rangeCheck3D6B38=1;random->range.lazy80AC=true;}
        if(selected>=count)return poison(d,E::range,0x1D4F84);
        event=first+selected;r.event=event;return true;
    }
    float probability(R& r) noexcept {
        ++r.draws308030;
        return static_cast<float>(positiveSourceFloatInteger(drawOriginalSharedRng50IU(*noise)))*0x1p-31f;
    }
    bool duration(std::uint32_t event,int& seconds,D& d) noexcept {
        float value=0;
        if(!backend.responseDuration(backend.context,event,value))return poison(d,E::audio,0x292510);
        const auto* source=assets->event(event);
        if(!source||!source->metadata||!std::isfinite(value)||value<=0||value>=10||
           value!=source->metadata->admittedDurationSeconds)return poison(d,E::audio,0x233234);
        seconds=static_cast<int>(value);return true;
    }
    bool play(std::uint32_t event,D& d) noexcept {
        return backend.playResponse(backend.context,event,0,0)||poison(d,E::audio,0x292360);
    }
    bool stop(D& d) noexcept {return backend.stopAllResponses(backend.context)||poison(d,E::audio,0x2924A0);}
    bool transition(int selected,float blend,D& d) noexcept {
        if(selected<0||selected>=32)return poison(d,E::motion,0x1523F0);
        // Source stores current64 before155830. Preserve this partial update
        // on a post-effects failure rather than pretending rollback succeeded.
        current=selected;
        return transitionModelPlayback50KG(descriptors[static_cast<unsigned>(current)],blend,*bindings,graph,playback,d.playback)||
            poison(d,E::motion,0x155830);
    }
    bool randomSpeech(int& selected,R& r,D& d) noexcept {
        if(!stop(d))return false;
        std::uint32_t event=0,attempts=0;bool collision=false;
        do {
            if(!choice(19,17,event,r,d))return false;
            ++attempts;collision=false;for(auto old:fields.historyFC)if(old==static_cast<std::int32_t>(event))collision=true;
        }while(collision&&attempts<17);
        if(attempts>=17)event=static_cast<std::uint32_t>(fields.historyFC[4]);
        for(unsigned i=4;i>0;--i)fields.historyFC[i]=fields.historyFC[i-1];
        fields.historyFC[0]=static_cast<std::int32_t>(event);r.event=event;
        int seconds=0;if(!duration(event,seconds,d)||!play(event,d))return false;
        fields.lastRandomEventEC=static_cast<std::uint8_t>(event);
        selected=speechMotion(seconds,false);fields.idleCooldownF4=2;fields.readyA4=false;
        r.accepted=true;return true;
    }
    bool switchSide(int& selected,R& r,D& d) noexcept {
        if(fields.sideA8){selected=index("w_rt_lt_1");fields.sideA8=0;}
        else{selected=index("w_lt_rt_1");fields.sideA8=1;}
        fields.sideCooldownF8=30;
        std::uint32_t event=0;if(!choice(36,5,event,r,d)||!play(event,d))return false;
        fields.idleCooldownF4=2;fields.readyA4=false;r.accepted=true;return true;
    }
};
OriginalUiGarageWattoLifecycle50KG::OriginalUiGarageWattoLifecycle50KG() noexcept=default;
OriginalUiGarageWattoLifecycle50KG::~OriginalUiGarageWattoLifecycle50KG(){
    // Never free descriptors behind an attached/queued/poisoned real backend.
    // The caller must drain explicitly while its backend context is alive.
    if(storage_&&storage_->externalEffects)(void)storage_.release();
}
bool OriginalUiGarageWattoLifecycle50KG::prepare(std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets,
    const assets::MotionNodeBindings49O& bindings,const SceneGraph49P& graph,ModelMotionSharedState49T& motionShared,
    ModelMode0IkShared50KG& ikShared,CoupledNoiseContext49M& noise,SharedRandomOwner50ID& random,HostMonotonicClock50ID& clock,
    OriginalUiGarageWattoBackend50KG backend,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyPrepared,0x1D4510);
    if(!assets||!assets->loaded()||!complete(backend)||!finiteGraph(graph)||
       !assets->bindings()||bindings.count!=assets->bindings()->count||!bindings.count||bindings.count>64)
        return fail(d,E::input,0x1D46C0);
    // Scene indices may be rebased by the genuine scene owner; row identity,
    // source flags and one-to-one node mapping may not be invented/reordered.
    for(std::uint32_t row=0;row<bindings.count;++row){
        const auto& b=bindings.rows[row];const auto& a=assets->bindings()->rows[row];
        if(std::memcmp(b.name,a.name,sizeof b.name)||b.translationFlag38!=a.translationFlag38||
           b.listedFlag3c!=a.listedFlag3c||b.sceneNode>=graph.count)return fail(d,E::input,0x156580);
        for(std::uint32_t prior=0;prior<row;++prior)if(bindings.rows[prior].sceneNode==b.sceneNode)return fail(d,E::input,0x156580);
    }
    if(!backend.preflight(backend.context,O::prepare,*assets,bindings,graph))return fail(d,E::preflight,0x1D46C0);
    try {
        auto n=std::make_unique<Storage>();n->assets=std::move(assets);n->bindings=&bindings;n->graph=graph;
        n->motionShared=&motionShared;n->ikShared=&ikShared;n->noise=&noise;n->random=&random;n->clock=&clock;n->backend=backend;
        for(unsigned i=0;i<32;++i){const auto* m=n->assets->motion(i);if(!m||!std::isfinite(m->motion.duration)||m->motion.duration<=0)return fail(d,E::input,0x1523F0);
            n->descriptors[i]={&m->motion,m->looping};}
        n->mask.count=bindings.count;for(std::uint32_t i=0;i<bindings.count;++i)n->mask.rows[i]=(*n->assets->wingMask())[i];
        // Constructor1D46C0 captures the genuine model's original matrices.
        if(!captureModelPlaybackMatrices50KG(bindings,graph,n->playback,d.playback))return fail(d,E::motion,0x155730);
        storage_=std::move(n);return true;
    }catch(...){return fail(d,E::allocation,0x1D4510);}
}
bool enterOriginalGarageWattoMotion1D5CA850KG(const ModelPlaybackMotion50KG& motion,
    const assets::MotionNodeBindings49O& rows,const SceneGraph49P& graph,const ModelPlaybackIk50KG& ik,
    ModelPlayback50KG& playback,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& mode0,
    ModelPlaybackWorkspace50KG& workspace,OriginalUiGarageWattoFields50KG& fields,D& d) noexcept {
    d={};
    if(!startModelPlayback50KG(motion,playback,d.playback))return fail(d,E::motion,0x155800);
    if(!advanceModelPlayback50KG(0.001f,rows,graph,ik,playback,shared,mode0,workspace,d.playback))
        return fail(d,E::motion,0x154480);
    fields.sideA8=1;fields.readyA4=false;return true;
}
bool OriginalUiGarageWattoLifecycle50KG::startEntry(R& r,D& d) noexcept {
    r={};d={};if(!storage_)return fail(d,E::unprepared,0x1D5C40);auto& s=*storage_;
    if(!s.check(O::entry,d))return false;
    r.effectsStarted=true;s.externalEffects=true;
    if(!s.backend.bindObserver1D4800(s.backend.context,s.graph))return s.poison(d,E::observer,0x1D4800);
    if(!s.backend.baseEntry2A78C0(s.backend.context))return s.poison(d,E::base,0x2A78C0);
    // wings_01 is descriptor31. It was admitted before any backend operation.
    s.current=s.index("wg_ent");
    if(s.current<0)return s.poison(d,E::motion,0x155800);
    if(!enterOriginalGarageWattoMotion1D5CA850KG(s.descriptors[static_cast<unsigned>(s.current)],
        *s.bindings,s.graph,s.ik,s.playback,*s.motionShared,*s.ikShared,s.workspace,s.fields,d))
        return s.poison(d,E::motion,d.sourcePC);
    s.entered=true;r.accepted=true;return true;
}
bool OriginalUiGarageWattoLifecycle50KG::advance(float dt,R& r,D& d) noexcept {
    r={};d={};if(!storage_)return fail(d,E::unprepared,0x1D4B10);auto& s=*storage_;
    if(!s.entered)return fail(d,E::notEntered,0x1D4B10);
    if(!std::isfinite(dt)||dt<0||!std::isfinite(5*dt))return fail(d,E::input,0x1D4B10);
    if(!s.check(O::tick,d))return false;
    r.effectsStarted=true;s.externalEffects=true;const bool oldReady=s.fields.readyA4;
    if(s.fields.sideCooldownF8>=0)s.fields.sideCooldownF8-=dt;
    if(s.current>=0){const float duration=s.descriptors[static_cast<unsigned>(s.current)].motion->duration;
        const float ratio=s.playback.time34/duration;
        if(ratio>0.8f&&!s.fields.exitA0){
            const float blend=0.25f*duration;s.fields.readyA4=true;r.eligible=true;int selected=-1;
            if(s.fields.idleCooldownF4<=0){
                if(s.probability(r)<s.assets->randomMotionProbability()){if(!s.randomSpeech(selected,r,d))return false;}
                else if(s.fields.sideCooldownF8<=0&&s.probability(r)<s.assets->switchSideProbability()){
                    if(!s.switchSide(selected,r,d))return false;
                }
            }
            if(s.fields.readyA4&&!oldReady){selected=s.index(s.fields.sideA8?"w_rt_br2":"w_lt_br2");s.fields.idleCooldownF4=2;}
            if(selected>=0){if(!s.transition(selected,blend,d))return false;}
            else s.fields.idleCooldownF4-=dt;
        }
    }
    if(!advanceModelPlayback50KG(dt,*s.bindings,s.graph,s.ik,s.playback,*s.motionShared,*s.ikShared,s.workspace,d.playback))return s.poison(d,E::motion,0x154480);
    ModelMotionState49T overlayState{1};
    if(!sampleModelMotionMasked50KG(*s.descriptors[31].motion,*s.bindings,s.mask,s.fields.wingPhaseF0,s.graph,
        overlayState,*s.motionShared,s.workspace.motion,d.overlay))return s.poison(d,E::motion,0x1559F0);
    s.playback.current18=nullptr; //155A34, still clear while observer28EA20 runs.
    if(!s.backend.notifyObserver28EA20(s.backend.context,s.graph))return s.poison(d,E::observer,0x28EA20);
    s.playback.current18=s.current<0?nullptr:&s.descriptors[static_cast<unsigned>(s.current)];
    const float phaseDelta=5*dt;s.fields.wingPhaseF0+=phaseDelta;
    if(s.fields.wingPhaseF0>1){s.fields.wingPhaseF0=0;if(!s.backend.playWing(s.backend.context,2))return s.poison(d,E::audio,0x233410);}
    if(!s.backend.baseTick141ED0(s.backend.context,dt))return s.poison(d,E::base,0x141ED0);
    return true;
}
bool OriginalUiGarageWattoLifecycle50KG::respond(OriginalUiGarageWattoResponse50KG response,R& r,D& d) noexcept {
    r={};d={};if(!storage_)return fail(d,E::unprepared,0x1D5320);auto& s=*storage_;
    const auto kind=static_cast<unsigned>(response);
    if(kind<1||kind>3)return fail(d,E::input,0x1D5320); // Purchase0 is not admitted.
    if(s.poisoned)return fail(d,E::poisoned,0x1D5320);
    if(!s.entered)return fail(d,E::notEntered,0x1D5320);
    if(kind!=3&&!s.fields.readyA4)return true; // Source no-op: no preflight/RNG/audio.
    const O op=kind==1?O::noMoney:kind==2?O::slider:O::noPurchase;
    if(!s.check(op,d))return false;
    r.effectsStarted=true;r.eligible=true;s.externalEffects=true;std::uint32_t event=0;
    if(!s.choice(kind==1?17u:kind==2?1u:16u,kind==3?1u:2u,event,r,d))return false;
    if(kind!=3&&!(s.probability(r)<0.25f))return true;
    int seconds=0;
    if(kind==3){if(!s.stop(d)||!s.play(event,d)||!s.duration(event,seconds,d))return false;if(seconds<3)seconds=3;}
    else if(!s.duration(event,seconds,d)||!s.play(event,d))return false;
    const int selected=s.speechMotion(seconds,kind==3);
    s.fields.idleCooldownF4=2;s.fields.readyA4=false;if(kind==3)s.fields.exitA0=true;
    if(!s.transition(selected,1.0f,d))return false;
    r.accepted=true;return true;
}
bool OriginalUiGarageWattoLifecycle50KG::drain(D& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.externalEffects&&!s.backend.drainDetach(s.backend.context))return s.poison(d,E::drain,0x1D4410);
    storage_.reset();return true;
}
bool OriginalUiGarageWattoLifecycle50KG::prepared()const noexcept{return bool(storage_);}
bool OriginalUiGarageWattoLifecycle50KG::entered()const noexcept{return storage_&&storage_->entered;}
bool OriginalUiGarageWattoLifecycle50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
const OriginalUiGarageWattoFields50KG* OriginalUiGarageWattoLifecycle50KG::fields()const noexcept{return storage_?&storage_->fields:nullptr;}
const ModelPlayback50KG* OriginalUiGarageWattoLifecycle50KG::playback()const noexcept{return storage_?&storage_->playback:nullptr;}
const assets::OriginalUiGarageWattoMotion50KG* OriginalUiGarageWattoLifecycle50KG::currentMotion()const noexcept{return storage_&&storage_->current>=0?storage_->assets->motion(static_cast<unsigned>(storage_->current)):nullptr;}
float OriginalUiGarageWattoLifecycle50KG::remainingCurrentMotion()const noexcept{const auto* m=currentMotion();return m?m->motion.duration-storage_->playback.time34:0;}
}
