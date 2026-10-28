#pragma once
#include "source_announcer_authoring50ie_fixture.h"
#include "../../src/game/lap_host_clock50id.h"
#include "../../src/game/lap_event6_subscriber50id.h"
#include "../../platform/linux/native/announcer_episode50ic.h"

namespace rr::game { class OriginalRaceMusicManager50LA; }
namespace source_announcer50IE {
class Owner final {
    friend class rr::platform::OriginalApplicationAural50KG;
    friend class rr::game::OriginalRaceMusicManager50LA;
public:
    Owner()=default;~Owner()noexcept;
    Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    bool prepare(const std::filesystem::path&discRoot,const std::filesystem::path&audioRoot,
        rr::platform::AudioMixer50HD&mixer,rr::platform::RaceAudioGroups50IC&groups,
        rr::platform::AnnouncerEpisode50IC&episode,rr::game::LapEventRandomOwner50ID&lazy,
        rr::game::HostMonotonicClock50ID&clock,std::uint32_t&sharedRaw,std::string&error,
        std::uint32_t playerRacer60=1,const std::uint32_t* actualRacer60=nullptr,
        std::size_t actualRacerCount=0,rr::game::DynamicContactRangeOwner50IW* sharedRange=nullptr) {
        namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50IE requires a fresh stable owner";return false;}attempted_=true;
        try{
            d::need(groups.uses(mixer)&&episode.uses(mixer)&&!episode.active()&&!episode.episode(),
                "50IE must borrow the idle shared Dax episode, never another voice");
            if(!source_announcer_authoring50IE::readAuthoring(discRoot,authoring_,error,playerRacer60,
                    actualRacer60,actualRacerCount))return false;
            std::vector<rr::assets::AudioRequest50HC> requests;requests.reserve(authoring_.cues.size());
            const auto normalized=[](std::string name){
                for(auto& c:name){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c-'A'+'a');}
                return name;
            };
            // Source named and generic event keys can share one original VAG.
            // POD03 keys12/13/14/15 overlap generic42/41/45/51 respectively.
            // Deduplicate only cold PCM ownership. Actual semantic keys and
            // original per-bank history rows remain distinct and unmodified.
            for(const auto&cue:authoring_.cues){
                bool exists=false;
                for(const auto& request:requests)if(normalized(request.name)==normalized(cue.request.name)){
                    d::need(request.loop==cue.request.loop&&request.stereo==cue.request.stereo&&
                        request.stream==cue.request.stream,"50IE aliased source sample delivery flags differ");
                    exists=true;break;
                }
                if(!exists)requests.push_back(cue.request);
            }
            d::need(!authoring_.cues.empty()&&authoring_.cues.size()<=218,
                "50IE original semantic cue count outside bounded roster");
            clips_.resize(authoring_.cues.size());availability_.resize(authoring_.cues.size());
            if(!rr::assets::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            d::need(bank_.clips.size()==requests.size(),"50IE incomplete original PCM bank registry");
            for(std::size_t i=0;i<clips_.size();++i){
                const auto* asset=bank_.find(authoring_.cues[i].request.name);
                d::need(asset,"50IE semantic cue lacks original PCM registry entry");
                const auto& c=*asset;availability_[i]=c.state;
                if(c.state!=rr::assets::AudioAssetState50HC::ready)continue;
                const auto&p=c.pcm;clips_[i]={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
                d::need(rr::platform::AudioMixer50HD::validClip(clips_[i])&&p.channels==1,
                    "50IE ready original Dax PCM invalid");
            }
            d::need(rr::game::initializeAnnouncer50IE(authoring_.parameters,state_),"50IE source initialization rejected");
            d::need(episode.enableOriginalPolicy50IE(),"50IE shared episode already active or policy selected");
            mixer_=&mixer;groups_=&groups;episode_=&episode;lazy_=&lazy;clock_=&clock;raw_=&sharedRaw;
            providers_={this,stopAll,end,begin,play,activity,seed,leader,sharedRange};
            prepared_=true;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
    // true means2C2C20 actually returned, including legitimate source rejection
    // or missing PCM. It does NOT mean audible playback or clip completion.
    bool request(std::uint32_t type,std::uint32_t racer,std::uint32_t variant,
                 std::uint32_t mode11258,std::uint8_t managerFlags58)noexcept {
        using P=rr::game::AnnouncerPhase50IE;using U=rr::game::AnnouncerUpdatePhase50IE;
        if(!current()||!hasNamedBank(racer)||(type<=2&&!authoring_.collisionRequests)||
           (update_.phase!=U::Idle&&update_.phase!=U::Done))return false;
        const rr::game::AnnouncerRequest50IE request{type,racer,variant};
        if(cursor_.phase==P::Idle||cursor_.phase==P::Done){
            if(!rr::game::beginAnnouncerRequest50IE(request,{mode11258,managerFlags58},cursor_)){
                last_={rr::game::AnnouncerStatus50IE::UnsupportedRequest,rr::game::AnnouncerBoundary50IE::None,0x2c2c20};return false;}
        }else if(cursor_.request.type!=type||cursor_.request.racer60!=racer||cursor_.request.variant!=variant)return false;
        last_=rr::game::resumeAnnouncerRequest50IE(state_,cursor_,providers_,*raw_);
        return last_.status==rr::game::AnnouncerStatus50IE::Complete;
    }
    // One accepted WORLD update, after common mixing, in source seconds. The
    // caller owns this explicit HOST schedule, including countdown; not one
    // update per racer, per event, per rendering frame or while paused.
    bool update(float sourceSeconds,const rr::game::AnnouncerLeader50IE&leaderInput)noexcept {
        using P=rr::game::AnnouncerPhase50IE;using U=rr::game::AnnouncerUpdatePhase50IE;
        if(!current()||(cursor_.phase!=P::Idle&&cursor_.phase!=P::Done))return false;
        if(update_.phase==U::Idle||update_.phase==U::Done){
            leader_=leaderInput;
            if(!rr::game::beginAnnouncerUpdate50IE(sourceSeconds,state_,update_))return false;
        }
        last_=rr::game::resumeAnnouncerUpdate50IE(state_,update_,providers_);
        return last_.status==rr::game::AnnouncerStatus50IE::Complete;
    }
    // Host owner closure, not a fabricated original constructor/reset call.
    // Stop before immutable PCM backing is released. A session restart creates
    // a new owner. The caller owns the application clock and shared lazy/raw
    // state; the current HOST resets lazy with its fresh-session physics RNG.
    bool reset()noexcept {
        if(!prepared_)return true;
        if(!episode_->stop())return false;
        prepared_=false;return true;
    }
    bool prepared()const noexcept{return current();}
    bool hasNamedBank(std::uint32_t racer)const noexcept {
        return racer<authoring_.namedBanks.size()&&authoring_.namedBanks[racer];
    }
    bool collisionRequestsEnabled()const noexcept{return current()&&authoring_.collisionRequests;}
    //2C4E70 ORs activity from27 possible named banks and the generic bank.
    // All admitted bank requests already share this actual native episode
    // transport. Inspect that owned voice; never use queued speech/timers as
    // activity or manufacture absent bank voices. False return is a boundary,
    // whereas true with active=false is a genuine inactive observation.
    bool anyActive2C4E70(bool& active)const noexcept {
        active=false;
        if(!current())return false;
        rr::platform::AudioVoiceObservation50HD observed{};
        const auto slot=episode_->slot();
        if(!mixer_->inspectVoice(slot,observed)||!observed.groupKnown||
           groups_->binding(slot)!=rr::game::RaceAudioGroup50IC::Announcer120)return false;
        if(!observed.voice.active)return true;
        if(!episode_->episode()||!playbackRevision_||observed.playbackRevision!=playbackRevision_)
            return false;
        const auto& live=observed.voice;
        if(live.loop||live.gain!=1.f||live.pitch!=1.f||live.pan!=0.f)return false;
        for(std::size_t i=0;i<clips_.size();++i){const auto& clip=clips_[i];
            if(availability_[i]==rr::assets::AudioAssetState50HC::ready&&
               live.clip.samples==clip.samples&&live.clip.frames==clip.frames&&
               live.clip.sampleRate==clip.sampleRate&&live.clip.channels==clip.channels&&
               live.clip.loopStart==clip.loopStart&&live.clip.loopEnd==clip.loopEnd){
                active=true;return true;
            }
        }
        return false;
    }
    const rr::assets::AudioBank50HC&bank()const noexcept{return bank_;}
    const rr::game::AnnouncerState50IE&state()const noexcept{return state_;}
    const rr::game::AnnouncerCursor50IE&lastRequest()const noexcept{return cursor_;}
    const rr::game::AnnouncerResult50IE&lastResult()const noexcept{return last_;}
private:
    bool canStopResultsNative26D070(const rr::platform::AudioMixer50HD& mixer,
        const rr::platform::RaceAudioGroups50IC& groups)const noexcept {
        bool active=false;
        return mixer_==&mixer&&groups_==&groups&&current()&&
            anyActive2C4E70(active); //Exact active episode/revision/cue PCM; false activity is genuine.
    }
    std::size_t resultsStopVoice26D070()const noexcept {return episode_->slot();}
    void stopResultsNativePreflighted26D070()noexcept {
        // Hardware command0B stops the actual source voice, NOT sourceEnd.
        // Preserve episode_, queued source state, playback token and ducking;
        // original later activity/update owns the actual end callback.
        mixer_->stop(episode_->slot());
    }
    bool current()const noexcept {
        return prepared_&&mixer_&&groups_->uses(*mixer_)&&episode_->uses(*mixer_)&&episode_->originalPolicy50IE();
    }
    static Owner&self(void*context)noexcept{return *static_cast<Owner*>(context);}
    static bool stopAll(void*c)noexcept{return self(c).episode_->sourceStopAll50IE();}
    static bool end(void*c)noexcept{return self(c).episode_->sourceEnd50IE();}
    static bool begin(void*c)noexcept{return self(c).episode_->sourceBegin50IE();}
    static bool play(void*c,std::uint32_t bank,std::uint32_t key,bool&started)noexcept {
        auto&o=self(c);started=false;
        for(std::size_t i=0;i<o.authoring_.cues.size();++i){const auto&a=o.authoring_.cues[i];
            if(a.bankRacer!=bank||a.key!=key)continue;
            // A real cold resolution failure is an actual unavailable return,
            // not a fake acknowledgement of an unimplemented event helper.
            if(o.availability_[i]!=rr::assets::AudioAssetState50HC::ready)return true;
            const bool returned=o.episode_->sourcePlay50IE(o.clips_[i],started);
            if(returned&&started)o.playbackRevision_=o.mixer_->playbackRevision(o.episode_->slot());
            return returned;
        }
        return false;
    }
    static bool activity(void*c,bool&active)noexcept {
        return self(c).anyActive2C4E70(active);
    }
    static bool seed(void*c,std::uint32_t&raw)noexcept {
        auto&o=self(c);if(&raw!=o.raw_)return false;
        auto&lazy=*o.lazy_;
        if(!lazy.lazy80A8){lazy.seed3D6B30=0;lazy.lazy80A8=true;}
        if(!lazy.seed3D6B30){
            std::uint32_t timestamp=0;
            if(!rr::game::sampleHostTimestamp50ID(*o.clock_,timestamp))return false;
            lazy.seed3D6B30=timestamp;raw=timestamp; // Exact308020 seed store.
        }
        return true;
    }
    static bool leader(void*c,bool&value)noexcept {
        return rr::game::announcerLeaderTail50IE(self(c).leader_,value);
    }
    source_announcer_authoring50IE::Authoring authoring_{};
    rr::assets::AudioBank50HC bank_{};
    // Resized only during cold prepare, then immutable until owner closure.
    std::vector<rr::platform::AudioClip50HD> clips_;
    std::vector<rr::assets::AudioAssetState50HC> availability_;
    rr::platform::AudioMixer50HD*mixer_=nullptr;
    rr::platform::RaceAudioGroups50IC*groups_=nullptr;
    rr::platform::AnnouncerEpisode50IC*episode_=nullptr;
    rr::game::LapEventRandomOwner50ID*lazy_=nullptr;
    rr::game::HostMonotonicClock50ID*clock_=nullptr;
    std::uint32_t*raw_=nullptr;
    rr::game::AnnouncerState50IE state_{};
    rr::game::AnnouncerCursor50IE cursor_{};
    rr::game::AnnouncerUpdateCursor50IE update_{};
    rr::game::AnnouncerProviders50IE providers_{};
    rr::game::AnnouncerLeader50IE leader_{};
    rr::game::AnnouncerResult50IE last_{};
    std::uint64_t playbackRevision_=0;
    bool attempted_=false,prepared_=false;
};
} // namespace source_announcer50IE
