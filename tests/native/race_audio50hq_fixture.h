#pragma once
// Bounded HOST race-audio subscriber. Original retained clips and their authored
// EventN categories are used; the complete original announcer scheduler, chance,
// priority and variant RNG are not reconstructed here. Never creates a race,
// lap, place or finish. Player0 is one explicitly selected retained original.
#include "race_session50hm_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/announcer_episode50ic.h"
#include "race_audio_groups50ic_fixture.h"
#include <array>
#include <charconv>
#include <string_view>

namespace race_audio50HQ {
inline constexpr std::size_t firstVoice=80,voiceCount=16,clipCount=22;
enum class Kind:std::uint8_t { Countdown3,Countdown2,Countdown1,Go,Lap1,Lap2,Lap3,
    Finish1,Finish2,Finish3,AchieveFirst,MoveUp,WasPassed,UnsupportedFinish };
enum class Result:std::uint8_t { Started,Unavailable,Busy,Unsupported,Rejected };
struct Event {Kind kind=Kind::Go;std::uint32_t authoredKey=0;Result result=Result::Rejected;};
struct Receipt {
    bool accepted=false;
    std::size_t eventCount=0;
    std::array<Event,3> events{};
};
struct Counters {
    std::uint64_t observations=0,starts=0,unavailable=0,busy=0,rejected=0;
    std::uint64_t countdownEvents=0,lapEvents=0,finishEvents=0,positionEvents=0;
    std::uint64_t unsupportedFinishes=0,resets=0,rebases=0;
};
namespace detail {
inline std::string_view trim(std::string_view s)noexcept {
    const auto a=s.find_first_not_of(" \t\r");if(a==s.npos)return {};
    return s.substr(a,s.find_last_not_of(" \t\r")-a+1);
}
// Values are parsed from the disc. The retained race file contains
// Event5..9 but NumEvents=4: those UI-looking trailing records are not loaded.
inline std::vector<rr::assets::AudioRequest50HC> readEvents(
    const std::vector<std::uint8_t>&bytes,unsigned expected) {
    using pod_materials50ed_fixture::detail::need;
    std::vector<rr::assets::AudioRequest50HC> out(expected);
    std::vector<bool> names(expected,false);unsigned section=0,count=0;
    std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
    for(std::size_t at=0;at<text.size();) {
        auto end=text.find('\n',at);if(end==text.npos)end=text.size();
        const auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){
            if(line=="[ResponseEmitter]"){section=0;continue;}
            need(line.size()>7&&line.substr(0,6)=="[Event"&&line.back()==']',"50HQ malformed event section");
            const auto n=line.substr(6,line.size()-7);unsigned id=0;
            const auto parsed=std::from_chars(n.data(),n.data()+n.size(),id);
            need(parsed.ec==std::errc{}&&parsed.ptr==n.data()+n.size()&&id>0,"50HQ invalid event key");
            section=id;continue;
        }
        const auto eq=line.find('=');need(eq!=line.npos,"50HQ malformed event value");
        const auto key=trim(line.substr(0,eq)),value=trim(line.substr(eq+1));
        if(!section){need(key=="NumEvents","50HQ unsupported response header");
            const auto parsed=std::from_chars(value.data(),value.data()+value.size(),count);
            need(parsed.ec==std::errc{}&&parsed.ptr==value.data()+value.size()&&count==expected,"50HQ event count differs");continue;}
        if(section>expected)continue;
        auto&r=out[section-1];
        if(key=="FileName"){need(!names[section-1]&&!value.empty(),"50HQ repeated/missing filename");r.name=value;names[section-1]=true;}
        else {need((key=="IsStereo"||key=="IsLoop"||key=="IsStream")&&(value=="0"||value=="1"),"50HQ unsupported event flag");
            if(key=="IsStereo")r.stereo=value=="1";
            if(key=="IsLoop")r.loop=value=="1";
            if(key=="IsStream")r.stream=value=="1";}
    }
    need(count==expected,"50HQ missing event count");for(bool n:names)need(n,"50HQ missing authored event");return out;
}
// Shared source filenames own one cold PCM allocation, while every authored
// race/speech row keeps its own semantic event and variant-history position.
// expectedRows is the registry extent of the racers actually loaded: the four
// RACE_SND rows plus the selected player's named rows (18 for every stock pod,
// so stock is clipCount exactly as before; 0 for an add-on pod, which has no
// named announcer: PORT EXTENSION, NOT ORIGINAL). Bounded by clipCount.
inline std::vector<rr::assets::AudioRequest50HC> uniquePcmRequests(
    const std::vector<rr::assets::AudioRequest50HC>& rows,std::size_t expectedRows=clipCount) {
    using pod_materials50ed_fixture::detail::need;
    need(expectedRows>=4&&expectedRows<=clipCount&&rows.size()==expectedRows,
        "50HQ logical event registry extent differs");
    const auto normalized=[](std::string name){
        for(auto& c:name){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c-'A'+'a');}
        return name;
    };
    std::vector<rr::assets::AudioRequest50HC> unique;unique.reserve(rows.size());
    for(const auto& row:rows){
        bool exists=false;
        for(const auto& request:unique)if(normalized(request.name)==normalized(row.name)){
            need(request.loop==row.loop&&request.stereo==row.stereo&&request.stream==row.stream,
                "50HQ aliased source sample delivery flags differ");
            exists=true;break;
        }
        if(!exists)unique.push_back(row);
    }
    return unique;
}
inline constexpr unsigned keys[clipCount]={1,2,3,4,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,27,28,29};
struct State {
    bool seeded=false,finished=false;
    race_session50HM::Phase phase=race_session50HM::Phase::Uninitialized;
    std::uint32_t number=0,laps=0,place=0;
    double seconds=0,lastPositionVoiceSeconds=-5.0;
    std::array<std::uint8_t,7> variants{};
};
template<race_session50HM::SessionType S>
inline bool validShared50MB(const S&s)noexcept {
    if(!race_session50HM::detail::lifetimeValid(s))return false;
    if(s.phase==race_session50HM::Phase::Uninitialized||s.error!=race_session50HM::Error::None||
       !s.racerCount||s.racerCount>race_session50HM::maxRacers||!s.config.lapTarget||
       !std::isfinite(s.raceSeconds)||s.raceSeconds<0||!std::isfinite(s.countdownRemaining)||s.countdownRemaining<0||
       s.countdownRemaining>3600)return false;
    for(std::uint32_t i=0;i<s.racerCount;++i)
        if(!s.racers[i].observedValid||s.racers[i].sampled||s.racers[i].observed.lapCallbackPending)return false;
    const auto&r=s.racers[0];
    return r.place>=1&&r.place<=s.racerCount&&(!r.finished||
        (r.finishPlace>=1&&r.finishPlace<=s.racerCount&&r.observed.completedLaps>=s.config.lapTarget));
}
// Audio observes the common committed rows of any authenticated session.
// Backend-specific map lifetime remains checked by validShared50MB.
template<race_session50HM::SessionType S>
inline bool valid(const S& s)noexcept {return validShared50MB(s);}
}
// Own at a stable address and keep the mixer alive longer. Cold PCM vectors are
// immutable after prepare. Only main/simulation thread calls these methods.
class Owner final {
    friend class rr::platform::OriginalApplicationAural50KG;
    // Only actual prepared RaceSnd's four non-speech transports. Scheduling
    // remains labeled HOST presentation; this does not reconstruct source
    // countdown/event admission or include the shared Announcer episode.
    bool canStopResultsCountdown26D070(const rr::platform::AudioMixer50HD& mixer,
        const rr::platform::RaceAudioGroups50IC& groups)const noexcept {
        if(!attempted_||mixer_!=&mixer||groups_!=&groups||!groups.uses(mixer)||
           epoch_!=mixer.groupEpoch50IC()||!mixer.canRecordControlChanges(4)||
           !announcer_||!announcer_->uses(mixer)||hostSpeechEnabled_||
           !announcer_->originalPolicy50IE())return false;
        for(std::size_t n=0;n<4;++n){const auto slot=firstVoice+n;
            rr::platform::AudioVoiceObservation50HD v;
            if(groups.binding(slot)!=rr::game::RaceAudioGroup50IC::Effects110||
               !mixer.inspectVoice(slot,v)||!v.groupKnown||v.playbackRevision!=revisions_[n])return false;
            if(!v.playbackRevision){if(v.voice.active)return false;continue;}
            const auto& a=v.voice.clip;const auto& b=clips_[n];
            if(availability_[n]!=rr::assets::AudioAssetState50HC::ready||
               !rr::platform::AudioMixer50HD::validClip(b)||a.samples!=b.samples||
               a.frames!=b.frames||a.sampleRate!=b.sampleRate||a.channels!=b.channels||
               a.loopStart!=b.loopStart||a.loopEnd!=b.loopEnd||v.voice.loop||
               v.voice.gain!=1.f||v.voice.pitch!=1.f||v.voice.pan!=0.f)return false;
        }
        return true;
    }
    static constexpr std::size_t resultsCountdownStopCount26D070()noexcept{return 4;}
    std::size_t resultsCountdownStopVoice26D070(std::size_t n)const noexcept {
        return n<4?firstVoice+n:rr::platform::audioMaximumVoices50HD;
    }
    void stopResultsCountdownPreflighted26D070()noexcept {
        for(std::size_t n=0;n<4;++n)mixer_->stop(firstVoice+n);
        // Not reset(): preserve observed race phase/variant state and never
        // call announcer.stop/sourceEnd or restore ducked groups early.
    }
public:
    Owner()=default;~Owner()noexcept;
    Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    bool prepare(const std::filesystem::path&discRoot,const std::filesystem::path&audioRoot,
        rr::platform::AudioMixer50HD&mixer,rr::platform::RaceAudioGroups50IC&groups,
        rr::platform::AnnouncerEpisode50IC&announcer,std::string&error,
        std::uint32_t playerRacer60=1) {
        namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HQ preparation requires a fresh stable owner";return false;}attempted_=true;
        try {
            d::need(mixer.count()>=firstVoice+voiceCount,"50HQ reserved voices80..95 unavailable");
            d::need(groups.uses(mixer),"50IC shared group owner differs from race mixer");
            constexpr std::size_t maxEventFile=32*1024;
            d::Inputs input;input.capacity=2*maxEventFile;
            const auto* identity=original_player_identity50IK::find(playerRacer60);
            d::need(identity,"50IK HOST named announcer player identity unsupported");
            // PORT EXTENSION, NOT ORIGINAL: an add-on pod row has no named
            // announcer INI (generic lines only); its named clips stay absent
            // (Unavailable), the four RACE_SND clips are unchanged.
            const bool named=!original_player_identity50IK::addonPodOf(identity->racer60);
            const auto raceBytes=input.read(discRoot/"AUDIO/RACE_SND.INI",maxEventFile);
            const auto speechBytes=named?input.read(original_player_identity50IK::announcerPath(discRoot,*identity),maxEventFile):
                std::vector<std::uint8_t>{};
            auto race=detail::readEvents(raceBytes,4);
            const auto speech=named?detail::readEvents(speechBytes,identity->announcerEvents):
                std::vector<rr::assets::AudioRequest50HC>{};
            const std::size_t present=named?clipCount:4;
            std::vector<rr::assets::AudioRequest50HC> requests;requests.reserve(clipCount);
            for(std::size_t i=0;i<present;++i){const auto&r=i<4?race[detail::keys[i]-1]:speech[detail::keys[i]-1];
                d::need(!r.loop&&!r.stereo&&r.stream==(i>=4),"50HQ authored delivery flags differ");requests.push_back(r);}
            const auto unique=detail::uniquePcmRequests(requests,present);
            if(!named)std::cerr<<"dlc_race_audio racer="<<identity->racer60<<" named_clips=0 race_clips=4 registry_rows="
                <<requests.size()<<" pcm_files="<<unique.size()<<'\n';
            if(!rr::assets::loadAudioBank50HC(audioRoot,unique,bank_,error))return false;
            d::need(bank_.clips.size()==unique.size(),"50HQ incomplete PCM registry");
            for(std::size_t i=0;i<present;++i){const auto* asset=bank_.find(requests[i].name);
                d::need(asset,"50HQ logical event lacks original PCM registry entry");
                const auto&c=*asset;availability_[i]=c.state;
                if(c.state!=rr::assets::AudioAssetState50HC::ready)continue;
                const auto&p=c.pcm;clips_[i]={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
                d::need(rr::platform::AudioMixer50HD::validClip(clips_[i]),"50HQ ready PCM invalid");}
            d::need(race_audio_groups50IC::bindProviderRange(groups,
                race_audio_groups50IC::ProviderRole::CountdownRaceSnd,firstVoice,4),
                "50IC countdown group binding rejected");
            d::need(announcer.prepare(mixer,groups,firstVoice+4),"50IC shared Dax episode ownership rejected");
            announcer_=&announcer;mixer_=&mixer;groups_=&groups;
            epoch_=mixer.groupEpoch50IC();error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
    // Observe seeded grid, every unpaused countdown tick and every successful
    // commitFrame. Calling twice is inert. A rejected/partial frame is refused.
    // Countdown beeps are no longer timed here: the original start overlay
    // draw 197C10 requests them through requestRaceEvent1A4B80 below.
    // Speech is native policy: authored-group round robin, one voice, five-second
    // place cooldown. Lap/finish preempt place speech; no queued stale messages.
    // Cold policy switch: mode1 keeps real countdown/GO here while the shared
    // original announcer consumes event5/event6. No duplicate HOST Dax lines.
    bool setHostSpeechEnabled(bool enabled)noexcept {
        if(state_.seeded)return false;hostSpeechEnabled_=enabled;return true;
    }
private:
    template<race_session50HM::SessionType S>
    Receipt observeShared50MB(const S&s)noexcept {
        Receipt result;if(!mixer_||!detail::valid(s)||!announcer_||!announcer_->pollCompletion()){
            ++counters_.rejected;return result;}
        const auto&r=s.racers[0];const auto number=race_session50HM::countdownNumber(s);
        if(state_.seeded&&(s.raceSeconds<state_.seconds||r.observed.completedLaps<state_.laps||
           r.observed.completedLaps-state_.laps>1||
           (state_.phase!=race_session50HM::Phase::Countdown&&s.phase==race_session50HM::Phase::Countdown)||
           (state_.finished&&!r.finished))){++counters_.rejected;return result;}
        result.accepted=true;++counters_.observations;
        (void)number;
        if(hostSpeechEnabled_&&state_.seeded&&s.raceSeconds>state_.seconds){
            if(r.finished&&!state_.finished){
                ++counters_.finishEvents;
                if(r.finishPlace==1)emit(result,Kind::Finish1,8,true);
                else if(r.finishPlace==2)emit(result,Kind::Finish2,variant(9,2,0),true);
                else if(r.finishPlace==3)emit(result,Kind::Finish3,variant(11,2,1),true);
                else{result.events[result.eventCount++]={Kind::UnsupportedFinish,0,Result::Unsupported};++counters_.unsupportedFinishes;}
            }else if(r.observed.completedLaps>state_.laps&&!r.finished){
                ++counters_.lapEvents;
                if(r.observed.completedLaps==1)emit(result,Kind::Lap1,4,true);
                else if(r.observed.completedLaps==2)emit(result,Kind::Lap2,5,true);
                else if(r.observed.completedLaps==3)emit(result,Kind::Lap3,variant(6,2,2),true);
                // No authored lap4+ line in the selected retained bank.
            }else if(!r.finished&&r.place!=state_.place&&s.raceSeconds-state_.lastPositionVoiceSeconds>=5.0){
                ++counters_.positionEvents;
                if(r.place==1)emit(result,Kind::AchieveFirst,variant(13,3,3),false);
                else if(r.place<state_.place)emit(result,Kind::MoveUp,variant(16,3,4),false);
                else emit(result,Kind::WasPassed,variant(19,3,5),false);
                state_.lastPositionVoiceSeconds=s.raceSeconds;
            }
        }
        remember(s);return result;
    }
public:
    // 1A4B80(race music manager+15C ResponseEmitter, event): the only callers
    // are 197DA4 (3), 197EE0 (2), 19801C (1) and 198150 (4, GO). RACE_SND.INI
    // EventN is clip index N-1 (start01/02/03, gotone01) on its own voice
    // 80+N-1, so the 26D070 Results stop still covers all four.
    Event requestRaceEvent1A4B80(std::uint32_t event)noexcept {
        Event e{Kind::Go,event,Result::Rejected};
        if(!mixer_||event<1||event>4){++counters_.rejected;return e;}
        Receipt r;const auto index=static_cast<std::size_t>(event-1);
        static constexpr Kind kinds[4]={Kind::Countdown1,Kind::Countdown2,Kind::Countdown3,Kind::Go};
        emit(r,kinds[index],index,false);++counters_.countdownEvents;
        return r.events[0];
    }
    // Countdown/go observation is independent of navigation backend. The
    // existing explicit hostSpeechEnabled policy still controls lap/place/
    // finish announcements; admitting an owner never enables those policies.
    template<race_session50HM::SessionType S>
    Receipt observe(const S& s)noexcept {return observeShared50MB(s);}
    // Following an actual zero-time recovery/rebase, suppress position lines
    // caused solely by that move. Laps/results must agree with the prior commit.
private:
    template<race_session50HM::SessionType S>
    bool rebaseShared50MB(const S&s)noexcept {
        if(!mixer_||!detail::valid(s)||!state_.seeded||s.raceSeconds!=state_.seconds||
           s.phase!=state_.phase||s.racers[0].observed.completedLaps!=state_.laps||s.racers[0].finished!=state_.finished){++counters_.rejected;return false;}
        remember(s);++counters_.rebases;return true;
    }
public:
    template<race_session50HM::SessionType S>
    bool rebase(const S& s)noexcept {return rebaseShared50MB(s);}
    void reset()noexcept {
        if(mixer_)for(std::size_t i=0;i<4;++i)mixer_->stop(firstVoice+i);
        if(announcer_&&!announcer_->stop())++counters_.rejected;
        state_={};++counters_.resets;
    }
    bool prepared()const noexcept{return mixer_!=nullptr;}
    const rr::assets::AudioBank50HC&bank()const noexcept{return bank_;}
    const Counters&counters()const noexcept{return counters_;}
private:
    std::size_t variant(std::size_t base,std::size_t count,std::size_t cursor)noexcept {
        const auto offset=state_.variants[cursor];state_.variants[cursor]=static_cast<std::uint8_t>((offset+1)%count);return base+offset;
    }
    template<race_session50HM::SessionType S>
    void remember(const S&s)noexcept {
        if(!race_session50HM::detail::lifetimeValid(s))return;
        const auto&r=s.racers[0];state_.seeded=true;state_.phase=s.phase;
        state_.number=race_session50HM::countdownNumber(s);state_.seconds=s.raceSeconds;
        state_.laps=r.observed.completedLaps;state_.place=r.place;state_.finished=r.finished;
    }
    void emit(Receipt&r,Kind kind,std::size_t index,bool preempt)noexcept {
        Event event{kind,detail::keys[index],Result::Rejected};
        const auto slot=firstVoice+(index<4?index:4);
        if(availability_[index]!=rr::assets::AudioAssetState50HC::ready){event.result=Result::Unavailable;++counters_.unavailable;}
        else if(index>=4){
            // Variant, cooldown and preemption choices above remain HOST50HQ.
            // Every actual Dax start/completion shares voice84 and callbacks.
            const auto result=announcer_->requestHost(clips_[index],preempt);
            if(result==rr::platform::HostAnnouncerResult50IC::Started){event.result=Result::Started;++counters_.starts;}
            else if(result==rr::platform::HostAnnouncerResult50IC::Busy){event.result=Result::Busy;++counters_.busy;}
            else ++counters_.rejected;
        }else if(mixer_->play(slot,clips_[index],1.f,1.f,false)){
            revisions_[index]=mixer_->playbackRevision(slot);
            event.result=Result::Started;++counters_.starts;
        }
        else ++counters_.rejected;
        r.events[r.eventCount++]=event;
    }
    rr::assets::AudioBank50HC bank_{};
    std::array<rr::platform::AudioClip50HD,clipCount>clips_{};
    std::array<rr::assets::AudioAssetState50HC,clipCount>availability_{};
    rr::platform::AudioMixer50HD*mixer_=nullptr;
    rr::platform::RaceAudioGroups50IC*groups_=nullptr;
    std::uint64_t epoch_=0;
    std::array<std::uint64_t,4> revisions_{};
    rr::platform::AnnouncerEpisode50IC*announcer_=nullptr;
    detail::State state_{};Counters counters_{};bool attempted_=false,hostSpeechEnabled_=true;
};
} // namespace race_audio50HQ
