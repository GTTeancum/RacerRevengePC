#pragma once
#include "original_ai_engine_emitter.h"
#include "original_ai_logical_sound.h"
#include "original_shared_rng50iu.h"
#include "body_scene48g.h"
#include <array>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
struct OriginalAiAudioListener {
    float position[4]{},forward[4]{};
};
enum class OriginalAiAuralStatus : std::uint8_t { complete,input,owner,arithmetic,backend };
struct OriginalAiAuralReceipt {
    OriginalAiAuralStatus status=OriginalAiAuralStatus::input;
    std::uint32_t sourcePC=0;
    std::size_t started=0,stopped=0,controlled=0,unavailable=0,randomDraws=0;
};
namespace original_ai_aural_detail {
inline float add(float a,float b)noexcept{volatile float c=a+b;return c;}
inline float sub(float a,float b)noexcept{volatile float c=a-b;return c;}
inline float mul(float a,float b)noexcept{volatile float c=a*b;return c;}
inline float div(float a,float b)noexcept{volatile float c=a/b;return c;}
inline float literal(std::uint32_t bits)noexcept{float value;std::memcpy(&value,&bits,4);return value;}
inline bool sound(OriginalAiEmitterSound* s)noexcept {
    return s&&s->session&&s->soundKey&&s->nativeOwner&&s->nativeSound&&
        s->multiplier54==&s->nativeSound->multiplier54&&
        s->position60==s->nativeSound->position60.data();
}
inline float distance(float x,float z,float lx,float lz)noexcept {
    const float dx=std::fabs(sub(x,lx)),dz=std::fabs(sub(z,lz));
    return std::sqrt(add(mul(dx,dx),mul(dz,dz)));
}
}

// Actual ordered registration list for the bounded native AI audio service.
// Secondary480 is registered inside103B80 before173970 registers primaryD40.
// Native exclusive per-sound voices remove hardware scarcity/stealing; this
// does not reproduce hardware channel IDs, queue transport or full retail
// application-child order. The source admission/start/stop/control operations
// still act on the actual emitter and the same registered logical sound.
class OriginalAiAural final {
public:
    bool registerEmitter(OriginalAiEmitterSpatial& emitter)noexcept {
        if(blocked_||count_==entries_.size()||!emitter.setupApplied||!emitter.session||
           !emitter.emitterKey||!original_ai_aural_detail::sound(emitter.sound1F0))return false;
        for(std::size_t i=0;i<count_;++i)if(entries_[i]==&emitter||
            entries_[i]->session!=emitter.session||
            entries_[i]->sound1F0->nativeSound==emitter.sound1F0->nativeSound)return false;
        entries_[count_++]=&emitter;return true;
    }
    OriginalAiAuralReceipt update(const OriginalAiAudioListener* listeners,std::size_t count,
        const OriginalSoundModeFields50KG& mode,CoupledNoiseContext49M& shared)noexcept {
        using namespace original_ai_aural_detail;
        OriginalAiAuralReceipt r;
        if(blocked_||count>4||(count&&!listeners)||std::fegetround()!=FE_TONEAREST||
           !validOriginalSoundModeFields50KG(mode))return r;
        for(std::size_t i=0;i<count;++i)for(unsigned k=0;k<3;++k)
            if(!std::isfinite(listeners[i].position[k])||!std::isfinite(listeners[i].forward[k]))return r;
        auto fail=[&](OriginalAiAuralStatus status,std::uint32_t pc){blocked_=true;r.status=status;r.sourcePC=pc;return r;};
        //103280's first pass:291350 updates distance/eligibility for EVERY
        // actual registered emitter, including currently disabled ones.
        for(std::size_t i=0;i<count_;++i){auto& e=*entries_[i];
            if(!e.word1ECKnown||!e.word1E8Known||!sound(e.sound1F0)||
               !std::isfinite(e.far1BC)||e.far1BC<0.f||
               !std::isfinite(e.near1B8)||e.near1B8<0.f||e.near1B8>e.far1BC||
               !std::isfinite(e.chance1E0)||!std::isfinite(e.trigger1DC))
                return fail(OriginalAiAuralStatus::owner,0x291350u);
            e.distance1D0=std::numeric_limits<float>::max();
            const float x=(e.flags158&2u)?e.position170.v[0]:add(e.position170.v[0],e.velocity180.v[0]);
            const float z=(e.flags158&2u)?e.position170.v[2]:add(e.position170.v[2],e.velocity180.v[2]);
            for(std::size_t l=0;l<count;++l){
                const float d=distance(x,z,listeners[l].position[0],listeners[l].position[2]);
                if(!std::isfinite(d))return fail(OriginalAiAuralStatus::arithmetic,0x2914a4u);
                if(d<e.distance1D0)e.distance1D0=d;
            }
            e.flags1D4Low5=static_cast<std::uint8_t>(e.flags1D4Low5&~2u);
            if(e.flags158&1u){
                if(e.distance1D0<=e.trigger1DC)e.flags1D4Low5|=2u;
                else {if((e.flags158&16u)&&(e.flags1D4Low5&1u))e.flags1D4Low5&=~1u;
                    e.flags1D4Low5&=~16u;}
            }else if(e.distance1D0<mul(literal(0x3f99999au),e.far1BC))e.flags1D4Low5|=2u;
        }
        std::array<OriginalAiEmitterSpatial*,16> active{};std::size_t activeCount=0;
        for(std::size_t i=0;i<count_;++i){auto& e=*entries_[i];auto& view=*e.sound1F0;
            auto& owner=*view.nativeOwner;auto& s=*view.nativeSound;
            if(!e.word1EC)continue;
            const bool playing=e.word1E8!=0||owner.active(s); //2915D0.
            if(!(e.flags1D4Low5&2u)){
                if(playing){
                    e.flags1D4Low5&=~16u;e.word1E8=0;
                    if(owner.stop233670(s)!=platform::LogicalAudioOwner::Result::complete)
                        return fail(OriginalAiAuralStatus::backend,0x2917f4u);
                    ++r.stopped;
                }
                continue;
            }
            if(playing){active[activeCount++]=&e;continue;}
            //103360 saves/restores source volume around virtual78=291A40.
            const auto savedVolume=s.volume38;s.volume38=0;
            bool sourceReturned=false,attempt=true;
            if((e.flags158&1u)&&(e.flags1D4Low5&1u))attempt=false;
            if(attempt&&e.chance1E0<100.f){
                if(e.flags1D4Low5&16u){sourceReturned=true;attempt=false;}
                else {e.flags1D4Low5|=16u;
                    const auto random=drawOriginalSharedRng50IU(shared);++r.randomDraws;
                    const float chance=mul(100.f,mul(0x1p-31f,static_cast<float>(random)));
                    if(!(chance<=e.chance1E0)){sourceReturned=true;attempt=false;}
                }
            }
            if(attempt){
                if(e.flags158&1u)e.flags1D4Low5|=1u;
                if(e.flags158&8u)e.word1E8=1;
                const auto started=startAiLogicalSound233410(owner,s,0);
                // Restore only after actual native completion; a reached
                // unsupported backend preserves source's earlier writes.
                if(started.status!=AiLogicalSoundStatus::complete)
                    return fail(OriginalAiAuralStatus::backend,started.sourcePC);
                sourceReturned=started.sourceReturned;
                if(sourceReturned)++r.started;else ++r.unavailable;
            }
            if(sourceReturned)active[activeCount++]=&e;
            s.volume38=savedVolume;
        }
        //103480..1036E8 nearest listener uses current position, unlike the
        // secondary emitter's position+velocity admission above.
        for(std::size_t i=0;i<activeCount;++i){auto& e=*active[i];
            if(!count)return fail(OriginalAiAuralStatus::owner,0x103590u);
            std::size_t nearest=0;
            float d=distance(e.position170.v[0],e.position170.v[2],listeners[0].position[0],listeners[0].position[2]);
            for(std::size_t l=1;l<count;++l){const auto next=distance(e.position170.v[0],e.position170.v[2],
                    listeners[l].position[0],listeners[l].position[2]);
                if(!std::isfinite(next))return fail(OriginalAiAuralStatus::arithmetic,0x10356cu);
                if(next<d){d=next;nearest=l;}}
            if(!std::isfinite(d))return fail(OriginalAiAuralStatus::arithmetic,0x10356cu);
            if(!(d<e.far1BC))continue; //102950 false; no spatial controls.
            const float attenuation=d<=e.near1B8?100.f:
                sub(100.f,mul(100.f,div(sub(d,e.near1B8),sub(e.far1BC,e.near1B8))));
            const auto& listener=listeners[nearest];float forwardAngle=0.f,targetAngle=0.f;
            if(!originalAngle48G(listener.forward[2],listener.forward[0],forwardAngle)||
               !originalAngle48G(sub(e.position170.v[2],listener.position[2]),
                    sub(e.position170.v[0],listener.position[0]),targetAngle))
                return fail(OriginalAiAuralStatus::arithmetic,0x102a28u);
            const float degrees=literal(0x42652ee1u);
            auto angle=static_cast<std::int32_t>(mul(degrees,targetAngle))-
                static_cast<std::int32_t>(mul(degrees,forwardAngle));
            if(angle>180)angle-=359;else if(angle< -180)angle+=359;
            auto& view=*e.sound1F0;
            const auto controls=spatialAiLogicalSound233AE0(*view.nativeOwner,*view.nativeSound,
                mode,attenuation,angle,listener.position,false);
            if(controls.status!=AiLogicalSoundStatus::complete)
                return fail(OriginalAiAuralStatus::backend,controls.sourcePC);
            ++r.controlled;
        }
        r.status=OriginalAiAuralStatus::complete;r.sourcePC=0x103710u;return r;
    }
private:
    std::array<OriginalAiEmitterSpatial*,16> entries_{};
    std::size_t count_=0;bool blocked_=false;
};
}
