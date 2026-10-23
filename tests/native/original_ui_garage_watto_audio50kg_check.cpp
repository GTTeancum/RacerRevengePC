#include "../../platform/linux/native/original_ui_main_audio50kg.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
namespace p=rr::platform;namespace a=rr::assets;
namespace {
std::size_t checks=0;
void check(bool x,const char* why){++checks;if(!x){std::cerr<<"FAIL "<<why<<"\n";std::exit(1);}}
std::vector<std::uint8_t> read(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);check(bool(f),"original file opens");return {std::istreambuf_iterator<char>(f),{}};}
template<std::size_t voiceCount=49>struct FixtureBase {
 std::array<p::AudioVoice50HD,voiceCount> voices{};p::AudioMixer50HD mixer;
 std::shared_ptr<p::OriginalSoundMode50KG> mode=std::make_shared<p::OriginalSoundMode50KG>();
 p::OriginalUiMainAudio50KG audio;
 FixtureBase(){check(mixer.initialize(voices.data(),voices.size()),"mixer initializes exact requested shape");check(mode->initializeFromProfile1101C(1),"source stereo mode");check(mixer.bindOriginalSoundMode50KG(mode),"real application mode bind");}
 void prepare(const std::filesystem::path& root,std::shared_ptr<const a::OriginalStreamMetadata50KZ> meta){std::string e;const bool okay=voiceCount==53?audio.prepareGarageSceneFrontend(root,std::move(meta),mixer,e):audio.prepareGarageFrontend(root,std::move(meta),mixer,e);if(!okay)std::cerr<<e<<"\n";check(okay,"prepare all40 actual Sounds, wing and Song6");}
};
using Fixture=FixtureBase<>;using SceneFixture=FixtureBase<53>;
void mix(p::OriginalUiMainAudio50KG& audio,std::size_t frames){std::vector<std::int16_t> data(96000);while(frames){auto n=std::min(frames,std::size_t(48000));check(audio.mixStereo16(data.data(),n),"actual frontend producer mixes finite PCM");frames-=n;}}
}
int main(int argc,char** argv){
 constexpr float sourcePitch=p::OriginalUiGarageWattoAudio50KG::nativeRendererPitch;
 check(p::OriginalUiGarageWattoAudio50KG::originalBasePitch==1881&&sourcePitch==0.99968111515045166015625f,"authentic1881 pitch retained through native float ratio");
 check(argc==2,"original disc root argument");const auto disc=std::filesystem::path(argv[1]),root=disc/"AUDIO";
 auto app=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG ad;
 check(app->load(a::OriginalUiArchiveId50KG::appStart,read(disc/"APPSTART.RES"),ad),"APPSTART authenticates");
 auto meta=std::make_shared<a::OriginalStreamMetadata50KZ>();a::StreamMetadataDiagnostic50KZ md;
 check(meta->prepare(*app,md),"original1743 stream metadata prepares");
 {Fixture f;std::string e;check(!f.audio.prepare(root,f.mixer,e),"default API does not accept arbitrary49 mixer");}
 {std::array<p::AudioVoice50HD,8> voices; p::AudioMixer50HD mixer;p::OriginalUiMainAudio50KG old;std::string e;
  check(mixer.initialize(voices.data(),8),"legacy8 initialize");check(old.prepare(root,mixer,e),"legacy8 Main preserved");
  check(old.prepareRiderMenu(root,e)&&old.prepareCourseMenu(root,e)&&old.courseMenuReady(),"legacy exact8 Course preserved");check(!old.garageMenuReady()&&!old.garageWattoAudio(),"legacy never gains Garage authority");
  p::AudioVoiceObservation50HD voice;check(old.startMainLoop9()&&mixer.inspectVoice(0,voice)&&voice.voice.pitch==1.f,"legacy Main pitch policy unchanged");}
 {Fixture f;std::string e;check(!f.audio.prepareGarageFrontend(root,{},f.mixer,e),"metadata-only absent source rejects");}
 {Fixture f;f.prepare(root,meta);std::string e;
  check(f.audio.prepareRiderMenu(root,e)&&f.audio.prepareCourseMenu(root,e),"explicit exact49 subgroup permits cold Course preparation");
  auto* g=f.audio.garageWattoAudio();check(g&&g->quiet()&&g->canExecute(2,true),"concrete quietgroup readiness");
  for(std::uint32_t id=1;id<=40;++id){float d=0;check(g->queryDuration(id,d),"all40 metadata queries");check(d==g->eventMetadata(id)->admittedDurationSeconds,"duration is original Sound20 not PCM duration");check(g->eventMetadata(id)->originalPitch==1881&&g->eventMetadata(id)->originalSampleRate==22050,"all40 actual stream pitch and rate descriptors match");}
  float unchanged=99;check(!g->queryDuration(0,unchanged)&&unchanged==99&&!g->playResponse(41)&&!g->playResponse(1,1,0)&&!g->playWing(0),"invalid commands no fallback");
  check(f.audio.startRiderLoop7(),"same frontend starts Song7");p::OriginalUiMainAudio50KG::SteadyTransportObservation steady;
  check(f.audio.inspectSteadyRiderTransport(steady),"Rider lease accepts authenticated quiet subgroup");
  const auto musicRevision=f.mixer.playbackRevision(0);check(f.audio.startGarageLoop6()&&f.audio.activeMusicSong()==6&&f.mixer.playbackRevision(0)>musicRevision,"Song6 replaces SAME music voice");
  p::AudioVoiceObservation50HD music;check(f.mixer.inspectVoice(0,music)&&music.voice.clip.frames==464296&&music.voice.clip.channels==2&&music.voice.loop&&music.voice.pitch==sourcePitch,"genuine WATTOSHP stereo loop uses its authenticated1881 pitch");
  check(g->playResponse(18),"first response18");mix(f.audio,192000);
  p::OriginalUiGarageWattoAudio50KG::Observation before,after;check(g->inspect(before)&&before.activeInstances==1,"long speech still active at4sec");
  const double expectedFourSecondCursor=192000.0*(22050.0/48000.0)*sourcePitch;
  check(before.voices[0].voice.pitch==sourcePitch&&std::abs(before.voices[0].voice.sourceFrame-expectedFourSecondCursor)<.00001,"response cursor advances at recovered SPU rate represented by float ratio");
  check(f.mixer.inspectVoice(0,music)&&std::abs(music.voice.sourceFrame-expectedFourSecondCursor)<.00001,"same Song6 voice uses corresponding recovered rate");
  check(g->playResponse(18)&&g->inspect(after)&&after.activeInstances==2,"flags0 duplicate uses new instance, no truncation");
  check(after.voices[0].voice.sourceFrame==before.voices[0].voice.sourceFrame&&after.voices[0].playbackRevision==before.voices[0].playbackRevision,"prior instance cursor and identity unchanged");
  check(g->stopAllResponses()&&g->inspect(after)&&after.activeInstances==1&&after.voices[0].voice.active&&!after.voices[1].voice.active,"stopAll latest handle only, older duplicate survives");
  check(g->drainInstances()&&g->quiet(),"widget drain stops older instances too");
  check(g->playWing(2)&&g->inspect(after)&&after.activeInstances==1,"actual wing immediate command");
  const auto wi=std::find(after.soundIds.begin(),after.soundIds.end(),41)-after.soundIds.begin();
  check(wi<41&&!after.voices[wi].voice.loop&&after.voices[wi].voice.gain==1.f&&after.voices[wi].voice.pitch==sourcePitch&&after.voices[wi].originalLeft==.17f&&after.voices[wi].originalRight==.17f,"wing source17 routed exactly once; authentic sample pitch; flags2 is not loop");
  mix(f.audio,8840);check(!g->quiet(),"wing remains active through last fractional source frame at new rate");
  mix(f.audio,1);check(g->quiet(),"wing actual PCM cursor completes at output frame8841 before next normal phase wrap");
  check(f.audio.fadeGarageMusic()&&f.audio.update(.5f)&&f.audio.mainGain()==.2f&&f.audio.update(.5f)&&f.audio.mainGain()==0.f,"Song6 uses existing exact1sec fade");
  check(f.mixer.inspectVoice(0,music)&&music.voice.pitch==sourcePitch,"Song6 fade preserves recovered rate instead of restoring nominal unity");
  check(f.audio.update(.35f)&&!f.mixer.active(0),"source existing silent retirement margin");
  check(f.audio.startRiderLoop7()&&f.audio.inspectSteadyRiderTransport(steady),"quiet Garage returns to steady Rider lease");
  check(steady.music.voice.pitch==1.f,"returning legacy Rider explicitly restores unchanged unity policy");
  check(g->playResponse(1)&&!f.audio.inspectSteadyRiderTransport(steady),"live Garage voices cannot certify Rider steady");
  f.audio.pause(true);const auto rev=f.mixer.controlRevision();check(!g->canExecute(1,false)&&!g->playWing(2)&&!g->playResponse(1)&&f.mixer.controlRevision()==rev,"paused persistent subgroup pointer cannot play");
  f.audio.pause(false);check(g->drainInstances(),"drain before capacity vector");
  for(unsigned id=1;id<=40;++id)check(g->playResponse(id),"all40 native Sounds really play");
  check(g->playWing(2)&&g->inspect(after)&&after.activeInstances==41,"bounded native capacity41, not original channel count");
  const auto full=f.mixer.controlRevision();check(!g->canExecute(1,true)&&!g->playResponse(18)&&!g->playWing(2)&&full==f.mixer.controlRevision(),"exhaustion fails before stealing or mutation");
  check(g->stopAllResponses()&&g->inspect(after)&&after.activeInstances==1,"all40 logical latest voices stop; wing distinct");
  check(g->drainInstances()&&g->quiet(),"complete native group drain");
  check(g->playResponse(16),"noPurchase actual stream sample");float duration=0;check(g->queryDuration(16,duration)&&duration==6.587936f,"noPurchase duration independent original metadata");
  check(f.audio.fadeMain(0.f),"stop music independently for real speech PCM check");
  std::array<std::int16_t,8192> audible{};check(f.audio.mixStereo16(audible.data(),4096),"real noPurchase speech renders through producer");
  check(std::any_of(audible.begin(),audible.end(),[](auto v){return v!=0;}),"original speech PCM is not silent substitute");
  f.audio.reset();for(std::size_t i=0;i<49;++i)check(!f.mixer.active(i),"owner destruction drains all borrowed sample voices");
 }
 {Fixture f;f.prepare(root,meta);auto* g=f.audio.garageWattoAudio();check(g->playResponse(18),"mutation case plays");
  check(f.mixer.setVoiceGroupGain50IC(8,.5f)&&!g->ready()&&!f.audio.ready(),"foreign group mutation invalidates producer and closure");
  std::int16_t data[2]{123,456};check(!f.audio.mixStereo16(data,1)&&data[0]==123&&data[1]==456,"invalid group fails closed, no fake silence");
  f.audio.reset();check(!f.mixer.active(8),"poisoned subgroup still drains at teardown");
 }
 {Fixture f;f.prepare(root,meta);auto* g=f.audio.garageWattoAudio();check(g->playResponse(17),"replacement case plays actual response");
  p::AudioVoiceObservation50HD observed;check(f.mixer.inspectVoice(8,observed),"read real clip identity");
  check(f.mixer.playSourceVolume(8,observed.voice.clip,100,1.f,false,0),"external sameclip restart changes actual token");
  f.mixer.stop(8);check(!g->ready()&&!f.audio.garageWattoAudio(),"replaced then stopped slot cannot forge quiet owner");
  f.audio.reset();check(!f.mixer.active(8),"replacement case teardown safe");
 }
 {SceneFixture f;std::string e;check(!f.audio.prepare(root,f.mixer,e),"legacy API rejects53 without an aggregate owner");}
 {SceneFixture f;std::string e;check(!f.audio.prepareGarageFrontend(root,meta,f.mixer,e),"49-voice API is not silently widened to53");}
 {SceneFixture f;f.prepare(root,meta);std::string e;
  check(f.audio.prepareRiderMenu(root,e)&&f.audio.prepareCourseMenu(root,e),"authenticated53 aggregate admits cold Course without relaxing defaults");
  auto* g=f.audio.garageWattoAudio();auto* d=f.audio.garageDroidAudio();
  check(g&&d&&d->ready()&&d->quiet()&&f.audio.garageSceneMenuReady(),"distinct genuine Watto and droid owners prepared");
  check(f.audio.startRiderLoop7(),"aggregate starts legacy Song7");p::OriginalUiMainAudio50KG::SteadyTransportObservation steady;
  check(f.audio.inspectSteadyRiderTransport(steady)&&steady.voiceCount==53,"quiet authenticated53 admits Rider lease");
  const std::array<float,4> position{{1,2,3,1}};
  check(d->afterSample(0,.1f,false,position)&&!f.audio.inspectSteadyRiderTransport(steady),"real scene playback blocks steady Rider lease");
  check(g->playResponse(18)&&f.mixer.active(8)&&f.mixer.active(49),"response and scene occupy disjoint native slots");
  f.audio.pause(true);const auto revision=f.mixer.controlRevision();
  check(!d->afterSample(5,.1f,false,position)&&!d->serviceGroup235720(.1f)&&f.mixer.controlRevision()==revision,"Main pause blocks persistent scene callback pointer");
  f.audio.pause(false);
  check(d->retire2353D0()&&!d->ready()&&d->transportValid()&&d->quiet()&&f.audio.ready()&&!f.audio.garageSceneMenuReady(),"real scene retirement preserves only authenticated quiet transport");
  check(g->drainInstances()&&f.audio.inspectSteadyRiderTransport(steady),"both drained groups permit Rider return after scene teardown");
  check(!d->afterSample(5,.1f,false,position),"retired scene cannot restart or impersonate a fresh lifetime");
  f.audio.reset();for(std::size_t i=0;i<53;++i)check(!f.mixer.active(i),"aggregate teardown drains all borrowed PCM");
 }
 {SceneFixture f;f.prepare(root,meta);auto* d=f.audio.garageDroidAudio();
  check(d->afterSample(5,.1f,false,{{1,2,3,1}}),"actual scene sound starts before corruption");
  p::AudioVoiceObservation50HD voice;check(f.mixer.inspectVoice(50,voice),"observe real scene clip");
  check(f.mixer.playSourceVolume(50,voice.voice.clip,100,sourcePitch,false,0),"foreign same-clip droid restart");f.mixer.stop(50);
  check(!d->transportValid()&&!f.audio.ready(),"inactive foreign scene revision cannot forge aggregate readiness");
  f.audio.reset();check(!f.mixer.active(50),"poisoned aggregate teardown remains safe");
 }
 {Fixture f;f.prepare(root,meta);check(f.audio.startGarageLoop6(),"Song6 corruption case starts");
  check(f.mixer.controls(0,.4f,1.f,0.f)&&!f.audio.ready(),"foreign Song6 pitch mutation invalidates new Garage transport");
  f.audio.reset();check(!f.mixer.active(0),"corrupted owned Song6 drains at teardown");
 }
 std::cout<<"PASS "<<checks<<" production Garage audio checks\n";
}
