#pragma once
#include "opponent_engine_audio50hx_fixture.h"
#include "../../src/game/original_ai_aural.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <array>
#include <memory>

namespace native_original_ai_audio {
// Actual native-owned D40/480/sound/resource instances for the eight AI bank
// rows. No mutable physics, thermal, listener or RNG owner is copied here.
// Native phase policy: D48/D40 after the represented body children; Aural at
// the accepted world-audio boundary. Full retail child chronology is unclaimed.
class Owner final {
    using E=rr::game::OriginalAiEngineEmitter;
    using Sound=rr::game::OriginalAiEmitterSound;
    using Channel=rr::platform::LogicalAudioOwner;
    struct Racer {
        E engine{};rr::game::OriginalAiEmitterSpatial binder{};
        std::array<Sound,7> sounds{};
        Channel loops{},wrecks{};
        float jukeboxCooldown224=5.f; //2C1000, actual D48's ordinary2C20C0.
        Owner* owner=nullptr;std::size_t slot=0;
    };
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){close();}
    bool prepare(const std::array<opponent_audio50HX::Source,8>& sources,
        const std::filesystem::path& audioRoot,const rr::assets::AudioBank50HC& events,
        rr::platform::AudioMixer50HD& mixer,std::uint64_t session,std::string& error) {
        namespace d=pod_materials50ed_fixture::detail;namespace a=rr::assets;
        if(attempted_||!session){error="AI audio requires fresh native ownership";return false;}
        attempted_=true;
        try {
            mode_=mixer.originalSoundMode50KG();
            d::need(mode_&&mode_->ready(),"AI audio requires the actual original sound-mode owner");
            std::vector<a::AudioRequest50HC> requests;
            for(std::size_t i=0;i<8;++i){mappings_[i]=opponent_audio50HX::detail::readMapping(sources[i]);
                for(const auto* name:{&mappings_[i].engineName,&mappings_[i].binderName}){
                    bool exists=false;for(const auto& request:requests)if(request.name==*name)exists=true;
                    if(!exists)requests.push_back({*name,false,false,false});
                }
            }
            if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            auto clip=[](const a::AudioClip50HC* asset,rr::platform::AudioClip50HD& out){
                if(!asset)return false;
                if(asset->state!=a::AudioAssetState50HC::ready)return true;
                const auto& p=asset->pcm;
                out={p.samples.data(),p.frames,p.sampleRate,p.channels,
                    p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};
                return p.channels==1&&rr::platform::AudioMixer50HD::validClip(out);
            };
            for(std::size_t i=0;i<8;++i){auto& r=racers_[i];r.owner=this;r.slot=i;
                const auto* identity=original_player_identity50IK::find(sources[i].podId);
                d::need(identity,"AI audio racer identity unavailable");
                // Explicit stable native object IDs, never PS2 addresses.
                const auto base=std::uint64_t(i+1)*16;
                rr::game::OriginalAiEmitterIdentity id{session,base+1,base+2,base+3,
                    static_cast<std::uint32_t>(i),identity->racer60};
                d::need(rr::game::projectFreshOriginalAiEngineEmitter(r.engine,id)&&
                    rr::game::projectFreshOriginalAiSecondaryEmitter(r.binder,session,base+4)&&
                    rr::game::projectOriginalAiEmitterSpatialSetup2911E0(r.engine,r.binder),
                    "AI source emitter construction/setup rejected");
                const auto first=i?96+(i-1)*3:57;
                d::need(r.loops.bind(mixer,first,3)&&r.wrecks.bind(mixer,160+i*4,4),
                    "AI logical sound ranges unavailable");
                for(std::size_t part=0;part<7;++part){
                    const a::AudioClip50HC* asset=nullptr;
                    if(part<2)asset=bank_.find(part?mappings_[i].binderName:mappings_[i].engineName);
                    else asset=events.find(gameplay_audio50HE::cueFilename({part==2?
                        rr::game::AudioBank50HE::PodEngine:rr::game::AudioBank50HE::PodWreck,
                        part==2?3u:static_cast<std::uint32_t>(part-2)}));
                    rr::platform::AudioClip50HD pcm{};
                    d::need(clip(asset,pcm),"AI original sound PCM record invalid");
                    auto& owner=part<3?r.loops:r.wrecks;const auto ordinal=part<3?part:part-3;
                    d::need(owner.registerClip(ordinal,pcm,asset->state==a::AudioAssetState50HC::ready)&&
                        rr::game::bindOriginalAiNativeSound(r.sounds[part],session,base+5+part,owner,ordinal),
                        "AI original logical sound registration rejected");
                }
                auto& engine=*r.sounds[0].nativeSound;auto& binder=*r.sounds[1].nativeSound;
                engine.loop1B0=binder.loop1B0=1; //232DB0 flagsA9 /291810 flags84.
                engine.pitch40=819;binder.pitch40=4096; //16DE60, constructor44C/454.
                auto volume=[](float value){return static_cast<std::int32_t>(
                    rr::game::detail::scalarMin48O(rr::game::detail::scalarMax48O(0.f,value),100.f));};
                engine.volume38=volume(mappings_[i].parameters.engineVolume);
                binder.volume38=volume(mappings_[i].parameters.binderVolume);
                r.sounds[2].nativeSound->volume38=90; //178AE0 boost.
                r.binder.sound1F0=&r.sounds[1];r.engine.spatial.sound1F0=&r.sounds[0];
                //16DD60 secondary registration precedes primary registration.
                d::need(aural_.registerEmitter(r.binder),"AI binder Aural registration rejected");
                engine.multiplier54=binder.multiplier54=0.f;
                d::need(rr::game::stopOriginalAiSpatial291A20(r.engine,r.engine.spatial)&&
                    rr::game::stopOriginalAiSpatial291A20(r.engine,r.binder),"AI cold stop rejected");
                const auto& p=mappings_[i].parameters;
                d::need(rr::game::bindOriginalAiEngineEmitterAfterSetup(r.engine,r.sounds[0],r.binder,
                    p.idlePitch,p.pitchRange,p.idleBinder,p.binderRange,&r.sounds[2])&&
                    aural_.registerEmitter(r.engine.spatial),"AI D40 ownership join rejected");
            }
            prepared_=true;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="AI source audio preparation failed";return false;}
    }
    // Native race activation owns these source lifecycle events. Not a claim
    // that the missing retail application event dispatcher was traversed.
    bool activate()noexcept {
        if(!prepared_||activated_)return false;
        for(auto& r:racers_)for(auto operation:{rr::game::OriginalAiEmitterLifecycle::startEngine16DB80,
            rr::game::OriginalAiEmitterLifecycle::startBinder16DC00})
            if(rr::game::applyOriginalAiEmitterLifecycle(r.engine,operation).status!=
                rr::game::OriginalAiEmitterStatus::complete)return false;
        activated_=true;return true;
    }
    bool setListener(const rr::game::OriginalAiAudioListener& listener)noexcept {
        for(unsigned i=0;i<3;++i)if(!std::isfinite(listener.position[i])||
            !std::isfinite(listener.forward[i]))return false;
        listener_=listener;listenerReady_=true;return true;
    }
    bool thermal(std::size_t slot,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,const rr::game::BoostInput50FK& input,
        const rr::game::BoostParameters50FK& parameters,rr::game::BoostState50FK& state,
        rr::game::BoostDiagnostic50FK& diagnostic)noexcept {
        if(!prepared_||slot>=8||!frame.fields.nativeAiOwnersBound50IV||
           frame.fields.vehicle58!=1u)return false;auto& r=racers_[slot];
        rr::game::OriginalAiEmitterRacerInput observed;
        if(!rr::game::readOriginalAiEmitterRacerInput(r.engine.identity,frame,bindings,state.speedRatio70,observed))return false;
        const rr::game::OriginalAiEmitterBoostBackend backend{&r,attachBoost,startBoost};
        return rr::game::advanceOriginalAiThermalWithD40(r.engine,observed,input,parameters,state,
            backend,diagnostic).status==rr::game::OriginalAiEmitterStatus::complete;
    }
    bool children(std::size_t slot,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,const rr::game::BoostState50FK& thermal)noexcept {
        if(!prepared_||slot>=8||!frame.fields.nativeAiOwnersBound50IV||
           frame.fields.vehicle58!=1u)return false;auto& r=racers_[slot];
        rr::game::OriginalAiEmitterRacerInput observed;
        if(!rr::game::readOriginalAiEmitterRacerInput(r.engine.identity,frame,bindings,thermal.speedRatio70,observed))return false;
        // Actual2C20C0 D48 ordinary update. Its141300 child list starts empty;
        //2C1170 loads logical sounds, and does not append object children.
        if(r.jukeboxCooldown224>0.f){volatile float remaining=r.jukeboxCooldown224-observed.elapsed;
            if(!std::isfinite(remaining))return false;r.jukeboxCooldown224=remaining;}
        return rr::game::advanceOriginalAiEngineEmitter16DA80(r.engine,observed,{}).status==
            rr::game::OriginalAiEmitterStatus::complete;
    }
    bool mixBoundary(rr::game::CoupledNoiseContext49M& shared)noexcept {
        if(!prepared_||!activated_||!listenerReady_||!mode_)return false;
        last_=aural_.update(&listener_,1,mode_->fields(),shared);
        return last_.status==rr::game::OriginalAiAuralStatus::complete;
    }
    void close()noexcept {for(auto& r:racers_){r.wrecks.close();r.loops.close();}prepared_=false;}
    bool prepared()const noexcept{return prepared_;}
private:
    static bool startBoost(void* context,Sound& sound,std::uint32_t flags)noexcept {
        auto& r=*static_cast<Racer*>(context);
        if(&sound!=&r.sounds[2]||!sound.nativeOwner||!sound.nativeSound)return false;
        const auto result=rr::game::startAiLogicalSound233410(*sound.nativeOwner,*sound.nativeSound,flags);
        return result.status==rr::game::AiLogicalSoundStatus::complete;
    }
    static bool attachBoost(void* context,Sound& sound,const rr::game::OriginalAiEmitterVector& position,
        const rr::game::OriginalAiEmitterVector&,float nearDistance,float farDistance,bool& returned)noexcept {
        auto& r=*static_cast<Racer*>(context);auto& owner=*r.owner;
        if(&sound!=&r.sounds[2]||!owner.listenerReady_||!owner.mode_||!sound.nativeSound)return false;
        using namespace rr::game::original_ai_aural_detail;
        const auto& l=owner.listener_;const auto distanceXZ=distance(position.v[0],position.v[2],l.position[0],l.position[2]);
        if(!std::isfinite(distanceXZ)||nearDistance!=0.f||farDistance!=450.f)return false;
        returned=false;if(!(distanceXZ<farDistance))return true;
        float forward=0.f,target=0.f;
        if(!rr::game::originalAngle48G(l.forward[2],l.forward[0],forward)||
           !rr::game::originalAngle48G(sub(position.v[2],l.position[2]),sub(position.v[0],l.position[0]),target))return false;
        const auto degrees=literal(0x42652ee1u);
        auto angle=static_cast<std::int32_t>(mul(target,degrees))-static_cast<std::int32_t>(mul(forward,degrees));
        if(angle>180)angle-=359;else if(angle< -180)angle+=359;
        for(unsigned i=0;i<3;++i)sound.nativeSound->position60[i]=position.v[i];
        const float attenuation=distanceXZ<=nearDistance?100.f:
            sub(100.f,mul(100.f,div(sub(distanceXZ,nearDistance),sub(farDistance,nearDistance))));
        const auto result=rr::game::spatialAiLogicalSound233AE0(*sound.nativeOwner,*sound.nativeSound,
            owner.mode_->fields(),attenuation,angle,l.position,false);
        if(result.status!=rr::game::AiLogicalSoundStatus::complete)return false;
        returned=true;return true;
    }
    std::array<Racer,8> racers_{};
    std::array<opponent_audio50HX::Mapping,8> mappings_{};
    rr::assets::AudioBank50HC bank_{};
    rr::game::OriginalAiAural aural_{};
    rr::game::OriginalAiAuralReceipt last_{};
    rr::game::OriginalAiAudioListener listener_{};
    rr::platform::OriginalSoundMode50KG* mode_=nullptr;
    bool attempted_=false,prepared_=false,activated_=false,listenerReady_=false;
};
}
