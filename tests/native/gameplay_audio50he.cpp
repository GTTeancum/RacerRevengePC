#include "gameplay_audio_provider50he_fixture.h"
#include "pod_audio_owner50he_fixture.h"
#include "../../src/game/lap_completion50gg.h"
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <new>

static std::atomic<std::size_t> allocations{0};
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
int main(){
    PodEngineControls50HE engine{};PodEngineParameters50HE tuning{};
    assert(podEngineControls50HE(tuning,{},engine));
    assert(engine.enginePitchWord==1310&&engine.binderPitchWord==4096);
    assert(std::fabs(engine.engineGain-.45f)<.00001f&&std::fabs(engine.binderGain-.405f)<.00001f);
    PodEngineInput50HE speed{};speed.speedRatio70=.5f;
    assert(podEngineControls50HE(tuning,speed,engine));
    // Source SQRT.S changes half-speed pitch substantially versus abs(sin).
    assert(engine.enginePitchWord==4617);
    speed.speedRatio70=-.5f;assert(podEngineControls50HE(tuning,speed,engine));
    assert(engine.enginePitchWord==4617);
    speed.speedRatio70=1;
    assert(podEngineControls50HE(tuning,speed,engine));assert(engine.enginePitchWord==5242);
    speed.boostPitch=true;assert(podEngineControls50HE(tuning,speed,engine));
    assert(engine.enginePitchWord==6028&&engine.binderPitchWord==4710);
    const auto saved=engine;speed.speedRatio70=3;
    assert(!podEngineControls50HE(tuning,speed,engine));assert(engine.enginePitchWord==saved.enginePitchWord);
    PodWarningInput50HE input{};PodWarningState50HE state{},next{};AudioCommand50HE commands[5]{};
    input.damage7D4=.7f;input.vehicle64=2;input.flags10CC=2;input.probe118=1;input.speedRatio70=.333f;
    std::size_t count=99;
    assert(updatePodWarnings50HE(input,state,next,commands,4,count)==AudioEventStatus50HE::Capacity);
    assert(next.flags4A4==0&&count==99);
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Complete);
    assert(count==5&&next.flags4A4==60);
    assert(commands[0].cue.key==4&&commands[1].cue.key==3&&commands[2].cue.key==5&&commands[3].cue.key==6);
    assert(commands[4].operation==AudioOperation50HE::Controls&&commands[4].gain==.33f);
    state=next;
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Complete&&count==1);
    input.sprayInputKnown=false;input.probe118=0;
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Complete&&count==0&&(next.flags4A4&32u));
    input.flagsCD8=1;input.damage7D0=std::numeric_limits<float>::quiet_NaN();
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Complete&&count==4);
    assert(next.flags4A4==0&&commands[0].operation==AudioOperation50HE::Stop);
    input.flagsCD8=0;input.manager1B0=16;
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Complete&&count==0&&next.flags4A4==60);
    input.manager1B0=0;count=91;
    assert(updatePodWarnings50HE(input,state,next,commands,5,count)==AudioEventStatus50HE::Input&&count==91);
    PodBoostAudioState50HE boost{},boostNext{};
    assert(updatePodBoostAudio50HE(true,.1f,0,boost,boostNext,commands,5,count)==AudioEventStatus50HE::Complete);
    assert(count==1&&boostNext.pitchBoost&&commands[0].cue.bank==AudioBank50HE::PodEngine&&commands[0].cue.key==3);
    boost=boostNext;
    assert(updatePodBoostAudio50HE(true,.1f,1,boost,boostNext,commands,5,count)==AudioEventStatus50HE::Complete&&count==0);
    assert(updatePodBoostAudio50HE(false,0,2,boost,boostNext,commands,5,count)==AudioEventStatus50HE::Complete&&count==0&&!boostNext.pitchBoost);
    float damage[4]={0,0,0,0};
    assert(repairCompletionAudio50HE(2,0,damage,commands,5,count)==AudioEventStatus50HE::Complete&&count==1&&commands[0].cue.key==2);
    damage[2]=.001f;
    assert(repairCompletionAudio50HE(2,0,damage,commands,5,count)==AudioEventStatus50HE::Complete&&count==0);
    std::string error;std::vector<rr::assets::AudioRequest50HC> requests;
    assert(rr::assets::readRetainedAudioRequests50HC("work/native/project/disc",requests,error)&&requests.size()==60);
    rr::assets::AudioBank50HC missing;
    assert(rr::assets::loadAudioBank50HC("solo-audio50he/explicit-missing-test-root",requests,missing,error));
    assert(missing.missing==60&&missing.ready==0);
    rr::platform::AudioVoice50HD voices[64]{};rr::platform::AudioMixer50HD mixer;
    assert(mixer.initialize(voices,64));
    gameplay_audio50HE::Provider absentProvider;
    assert(absentProvider.prepare(missing,mixer,0,error)&&absentProvider.loaded()==0);
    AudioCommand50HE start{};start.cue={AudioBank50HE::PodEvent,1};
    assert(absentProvider.apply(start)==gameplay_audio50HE::ApplyResult::Unavailable);
    pod_audio50HE::Owner absentLap;
    assert(pod_audio50HE::prepareFromPod01(absentLap,mixer,missing,60,2,error));
    auto missingReceipt=pod_audio50HE::dispatchLap(absentLap);
    assert(missingReceipt.completed()&&missingReceipt.originalV0==0&&!missingReceipt.playbackCalled&&absentLap.playbackCalls==0);
    assert(absentLap.sounds[0].fields.channel2C==-1&&absentLap.sounds[0].fields.word7C==0);
    // Explicit synthetic PCM exercises backend plumbing only. These values are
    // NOT game assets, are not written to an audio file, and are never fallback.
    auto synthetic=missing;
    for(auto& clip:synthetic.clips){clip.state=rr::assets::AudioAssetState50HC::ready;
        clip.pcm.samples={1200,2400,-1200,-2400};clip.pcm.frames=4;clip.pcm.channels=1;clip.pcm.sampleRate=48000;}
    gameplay_audio50HE::Provider provider;assert(provider.prepare(synthetic,mixer,0,error)&&provider.loaded()==60);
    pod_audio50HE::Owner lap;assert(pod_audio50HE::prepareFromPod01(lap,mixer,synthetic,60,2,error));
    const auto hotAllocations=allocations.load();
    for(const auto& cue:gameplay_audio50HE::catalog){start.cue={cue.bank,cue.key};start.loop=cue.authoredLoop;
        assert(provider.apply(start)==gameplay_audio50HE::ApplyResult::Started);assert(provider.active(start.cue));}
    assert(provider.starts()==60);provider.stopAll();
    start.cue={AudioBank50HE::PodEngine,1};start.loop=true;assert(provider.apply(start)==gameplay_audio50HE::ApplyResult::Started);
    std::int16_t mixed[32]{};assert(mixer.mixStereo16(mixed,16));assert(provider.active(start.cue));
    bool nonzero=false;for(auto sample:mixed)nonzero|=sample!=0;assert(nonzero);
    start.operation=AudioOperation50HE::Controls;start.pitch=.5f;assert(provider.apply(start)==gameplay_audio50HE::ApplyResult::Controlled);
    start.operation=AudioOperation50HE::Stop;assert(provider.apply(start)==gameplay_audio50HE::ApplyResult::Stopped);
    auto played=pod_audio50HE::dispatchLap(lap);assert(played.completed()&&played.originalV0==1&&played.channel==60&&mixer.active(60));
    assert(played.before.word7C==0&&played.after.word7C==1&&played.playbackCalled&&played.selectionStored);
    assert(pod_audio50HE::dispatchLap(lap).channel==61);
    auto full=pod_audio50HE::dispatchLap(lap);assert(full.status==pod_audio50HE::DispatchStatus::CompletedNoChannel&&full.originalV0==0);
    LapCompletionCursor50GG cursor{};auto pending=beginLapCompletion50GG(cursor);
    // Void50GG acknowledgement comes only after the genuine dispatch returned.
    LapCompletionResult50GG returned{};
    assert(acceptLapCompletionReturn50GG(cursor,{pending.request.sequence,pending.request.kind,0,false},returned));
    LapCompletionPlayer50GG player{};LapCompletionConfig50GG config{2,0,0};
    assert(advanceLapCompletion50GG(player,config,cursor).status==LapCompletionStatus50GG::Complete);
    assert(allocations.load()==hotAllocations);
    std::cout<<"PASS 50HE source controls/warnings/boost/repair;60 authoredcue mappings; real missing-provider path; synthetic-only positive native mix/loop/lap/capacity;zero hot allocations\n";
}
