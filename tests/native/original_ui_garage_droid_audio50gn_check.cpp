#include "../../platform/linux/native/original_ui_main_audio50kg.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>
namespace p=rr::platform;namespace a=rr::assets;
namespace {
unsigned checks=0;
void need(bool b,const char* s){++checks;if(!b){std::cerr<<"FAIL "<<s<<'\n';std::exit(1);}}
std::vector<std::uint8_t> read(const std::filesystem::path&p){std::ifstream f(p,std::ios::binary);need(bool(f),"original opens");return {std::istreambuf_iterator<char>(f),{}};}
struct Fixture {
 std::array<p::AudioVoice50HD,53> voices{};p::AudioMixer50HD mixer;
 std::shared_ptr<p::OriginalSoundMode50KG> mode=std::make_shared<p::OriginalSoundMode50KG>();
 p::OriginalUiMainAudio50KG main;
 Fixture(const std::filesystem::path& root,std::shared_ptr<const a::OriginalStreamMetadata50KZ> metadata){
  need(mixer.initialize(voices.data(),voices.size()),"exact53 mixer");need(mode->initializeFromProfile1101C(1)&&mixer.bindOriginalSoundMode50KG(mode),"same actual soundmode");std::string error;
  if(!main.prepareGarageSceneFrontend(root,std::move(metadata),mixer,error)){std::cerr<<error<<'\n';need(false,"actual aggregate prepares");}}
 p::OriginalUiGarageDroidAudio50GN& droid(){auto* p=main.garageDroidAudio();need(p,"concrete actual scene subgroup");return *p;}
};
bool mix(p::OriginalUiMainAudio50KG& main,std::size_t frames){std::array<std::int16_t,8192> out{};bool audible=false;
 while(frames){auto n=std::min(frames,std::size_t(4096));need(main.mixStereo16(out.data(),n),"actual producer mixes");audible|=std::any_of(out.begin(),out.begin()+2*n,[](auto x){return x!=0;});frames-=n;}return audible;}
}
int main(int argc,char**argv){
 need(argc==2,"disc root argument");const auto disc=std::filesystem::path(argv[1]);
 auto archive=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG ad;
 need(archive->load(a::OriginalUiArchiveId50KG::appStart,read(disc/"APPSTART.RES"),ad),"authentic APPSTART");
 auto metadata=std::make_shared<a::OriginalStreamMetadata50KZ>();a::StreamMetadataDiagnostic50KZ md;need(metadata->prepare(*archive,md),"same original metadata");
 constexpr std::array<unsigned,4> actors{{0,5,6,9}},milliseconds{{8041,2705,3038,11710}};
 constexpr float pitch=0.99968111515045166015625f;
 {
  Fixture f(disc/"AUDIO",metadata);auto& d=f.droid();p::OriginalUiGarageDroidAudio50GN::Observation o;
  need(d.inspect(o)&&d.ready()&&d.quiet(),"fresh source sounds are silent and live");
  for(unsigned i=0;i<4;++i)need(!o.played108[i]&&o.basePitch50[i]==4096&&o.pitch40[i]==4096&&o.sourceDuration20[i]==0&&o.channel2C[i]==-1&&o.elapsedBits24[i]==0,"actual232ED0 cold values, no metadata prefetch");
  const std::array<float,4> position{{2,-3,4,std::numeric_limits<float>::quiet_NaN()}};
  for(unsigned i=0;i<4;++i)need(d.afterSample(actors[i],i==3?1.f:0.f,false,position),"strict authored time boundary skips callback");
  need(d.quiet(),"no invented >= threshold");
  for(unsigned i:{1,2,3,4,7,8})need(d.afterSample(i,2.f,false,position),"actual empty sound row skip");
  need(d.quiet(),"empty actors do not alias sounds");
  for(unsigned i=0;i<4;++i){const auto t=i==3?std::nextafter(1.f,2.f):.001f;
   need(d.afterSample(actors[i],t,false,position)&&d.inspect(o),"actual sound starts once");
   need(o.played108[i]&&o.basePitch50[i]==1881&&o.pitch40[i]==1881&&o.sourceDuration20[i]==float(milliseconds[i])/1000.f,"original IRX pitch and integer-millisecond duration");
   const auto& v=o.voices[i];need(v.voice.active&&!v.voice.loop&&v.voice.pitch==pitch&&v.voice.gain==1.f&&v.voice.pan==0&&v.originalLeft==1&&v.originalRight==1,"flags2 immediate; no loop or invented spatial audio");
   need(o.position60[i]==std::array<float,4>{{2,-3,4,1}},"233DB0 copies XYZ only, source W remains1");}
  need(d.serviceGroup235720(.25f)&&d.inspect(o),"actual group elapsed update");
  for(unsigned i=0;i<4;++i)need(std::bit_cast<float>(o.elapsedBits24[i])==.25f&&o.channel2C[i]>=0,"233E00 updates source channel elapsed");
  const auto before=o;need(mix(f.main,48000*13),"genuine original PCM is audible");need(d.quiet()&&d.inspect(o),"all four original nonloop voices complete naturally");
  for(unsigned i=0;i<4;++i){need(d.afterSample(actors[i],99.f,true,{{9,8,7,1}}),"ordinary playback restart cannot clear played108");}
  need(d.inspect(o)&&d.quiet(),"no replacement after natural completion");
  for(unsigned i=0;i<4;++i)need(o.played108[i]&&o.position60[i]==before.position60[i]&&o.voices[i].playbackRevision==before.voices[i].playbackRevision,"source once-only marker and captured position persist");
  need(d.serviceGroup235720(.5f)&&d.inspect(o),"source elapsed update retains channel after native completion");
  for(unsigned i=0;i<4;++i)need(std::bit_cast<float>(o.elapsedBits24[i])==.75f,"no invented automatic source channel retirement");
  need(d.retire2353D0()&&d.retire2353D0()&&!d.ready()&&d.transportValid()&&d.quiet()&&f.main.ready(),"actual group retire permits only quiet transport");
  need(!d.afterSample(0,1,true,position)&&!d.serviceGroup235720(1),"retired source callbacks remain closed");
  need(f.mixer.setVoiceGroupGain50IC(49,.5f)&&!d.retire2353D0()&&!d.transportValid(),"already-retired corruption cannot fake successful drain");
 }
 {
  Fixture f(disc/"AUDIO",metadata);auto& d=f.droid();
  need(d.afterSample(9,0,true,{{5,6,7,1}})&&f.mixer.active(52),"source stopped10 bypasses even FALL time1");
  f.main.pause(true);const auto rev=f.mixer.controlRevision();need(!d.afterSample(0,1,false,{{1,1,1,1}})&&!d.serviceGroup235720(.1f)&&f.mixer.controlRevision()==rev,"aggregate pause blocks cached subgroup pointer");
  f.main.pause(false);need(d.retire2353D0()&&d.quiet()&&f.main.ready(),"active scene teardown drains actual PCM and retains transport");
 }
 {
  Fixture f(disc/"AUDIO",metadata);auto& d=f.droid();need(d.afterSample(5,.1f,false,{{1,2,3,1}}),"foreign-restart vector starts original");
  p::AudioVoiceObservation50HD v;need(f.mixer.inspectVoice(50,v),"observe owned PCM");need(f.mixer.playSourceVolume(50,v.voice.clip,100,pitch,false,0),"foreign sameclip revision");
  need(!d.ready()&&!d.transportValid()&&!f.main.ready(),"sameclip restart cannot forge source ownership");f.main.reset();need(!f.mixer.active(50),"exclusive sameepoch PCM drains despite foreign revision");
 }
 std::cout<<"PASS "<<checks<<" genuine Garage droid source vectors; no page or spatial-render admission\n";
}
