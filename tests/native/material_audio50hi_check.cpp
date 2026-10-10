#include "material_audio_provider50hi_fixture.h"
#include <array>
#include <atomic>
#include <bit>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <new>
#include <vector>
namespace {bool watch=false;std::size_t allocations=0,checks=0;void require(bool x,const char*label){++checks;if(!x){std::cerr<<label<<'\n';std::exit(1);}}}
void*operator new(std::size_t n){if(watch)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;namespace a=rr::assets;namespace p=rr::platform;
int main(){
    std::array<p::AudioVoice50HD,128>voices{};p::AudioMixer50HD mixer;require(mixer.initialize(voices.data(),voices.size(),48000),"mixer initialize");material_audio50HI::Provider provider;std::string error;
    require(provider.prepare("work/native/project/disc","work/native/project/disc/AUDIO",mixer,64,error),error.c_str());
    require(provider.bank().ready==13&&provider.bank().missing==0&&provider.bank().unsupported==0&&provider.bank().invalid==0,"13 actual decoded material clips");
    require(provider.catalog().collision[0]=="sfx/race/pod_coll.vag"&&provider.catalog().collision[3]==provider.catalog().collision[0]&&provider.catalog().collision[4]=="sfx/race/rockhit1.vag"&&provider.catalog().drive[7]=="sfx/race/rckscrp4.vag","actual material source rows");
    for(auto&v:provider.catalog().parameters.drive)require(v.sampleSpeed==100&&v.minVolume==0&&v.minSpeed==1&&v.maxSpeed==60&&v.resonantSpeed==100&&v.frequencySlope==.75f,"actual six drive parameters");
    std::ifstream f("work/native/project/disc/TRACKS/TA.RES",std::ios::binary);std::vector<std::uint8_t>raw((std::istreambuf_iterator<char>(f)),{}),scb;a::ResArchive48C archive;a::DecodeError48C decode;a::World48F world;
    require(a::decodeResource48C(raw,archive,decode)&&a::extractResMember48C(archive,"tracks\\ta.scb",scb,decode)&&a::decodeWorld48F(scb,world,decode),"actual SCB for contacts");
    std::array<ContactMaterial50HI,64>bindings{};std::size_t bindingCount=0;
    auto lower=[](std::string x){for(auto&c:x)if(c>='A'&&c<='Z')c+=32;return x;};
    for(const auto&i:world.instances){bool exists=false;for(const auto&e:archive.entries)if(lower(e.name)==lower(i.collision_member)){exists=true;break;}if(!exists)continue;
        require(a::readTrackContactMaterial50HI(world,i.zone,i.ordinal,static_cast<std::uint32_t>((bindingCount+1)*4),bindings[bindingCount],error),"actual collider material binding");require(bindings[bindingCount].materialA9==1,"source track MaterialID default1");++bindingCount;}
    require(bindingCount==37,"37 retained collider A9 bindings");std::int32_t material=-2;
    require(resolveContactMaterial50HI(4,bindings.data(),bindingCount,material)&&material==1,"currentCandidate130 exact binding");
    require(resolveContactMaterial50HI(0,bindings.data(),bindingCount,material)&&material==-1,"actual no contact");material=99;require(!resolveContactMaterial50HI(99,bindings.data(),bindingCount,material)&&material==99,"unknown identity no made-up material");
    MaterialAudioInput50HI input{};input.channelArgument=2;input.contactMaterialA9=1;input.speedRatio70=.6f;std::uint32_t rng=1,expectedRng=rng;
    std::uint32_t draws[3]{};for(auto&v:draws){expectedRng=expectedRng*0x41c64e6du+0x3039u;v=expectedRng&0x7fffffffu;}
    auto variant=[](std::uint32_t n){volatile float f=static_cast<float>(n);volatile float x=(0x1p-31f*f)*4.f;return static_cast<unsigned>(x)+1;};
    watch=true;auto r=provider.update(input,rng);watch=false;
    require(r.okay()&&r.commandCount==2&&rng==expectedRng&&allocations==0,"source onset collision and scrape, three shared RNG draws, zero hot allocations");
    require(r.commands[0].kind==MaterialCueKind50HI::Collision&&r.commands[0].key==4+variant(draws[0])&&r.commands[0].gain==.6f,"source rock collision variant/gain");
    require(r.commands[1].kind==MaterialCueKind50HI::DriveOn&&r.commands[1].key==4+variant(draws[1])&&r.commands[1].loop&&r.commands[1].gain==.59f,"source scrape variant, loop, gain");
    volatile float pitchDrawFloat=static_cast<float>(draws[2]);volatile float pitchDrawScaled=(0x1p-31f*pitchDrawFloat)*31.f;const auto pitchDraw=static_cast<int>(pitchDrawScaled);
    require(r.commands[1].pitch==static_cast<float>(4096-1200-pitchDraw)/4096.f,"source lazy slope30 and shared random pitch subtraction");
    require(r.results[0]==material_audio50HI::ApplyResult::Started&&r.results[1]==material_audio50HI::ApplyResult::Started,"real PCM mixed sound starts");
    const auto scrapeKey=r.commands[1].key;const auto previousRng=rng;watch=true;r=provider.update(input,rng);watch=false;require(r.okay()&&r.commandCount==1&&r.commands[0].key==scrapeKey&&r.commands[0].operation==AudioOperation50HE::Controls&&rng==previousRng*0x41c64e6du+0x3039u,"continued contact controls retained loop and one pitch RNG draw");
    std::array<std::int16_t,1536>pcm{};std::size_t nonzero=0;for(unsigned tick=0;tick<12;++tick){require(mixer.mixStereo16(pcm.data(),768),"actual material mix");for(auto s:pcm)nonzero+=s!=0;}require(nonzero>0,"audible-energy original clip PCM");
    input.contactMaterialA9=-1;const auto beforeMissing=provider.state();const auto beforeRng=rng;watch=true;r=provider.update(input,rng);watch=false;require(r.okay()&&r.commandCount==0&&rng==beforeRng&&provider.state().driveKey244==beforeMissing.driveKey244,"no contact source lazy skip retains prior state");
    input.channelArgument=3;watch=true;r=provider.update(input,rng);watch=false;require(r.okay()&&r.commandCount==1&&r.commands[0].operation==AudioOperation50HE::Stop&&provider.state().driveKey244==-1&&!(provider.state().emitterFlags22C&8u)&&rng==beforeRng,"timer inactive clears bit and stops actual scrape");
    MaterialAudioState50HI state{},dest{};MaterialAudioCommand50HI command[3]{};std::size_t count=123;input.channelArgument=2;input.contactMaterialA9=1;
    const auto before=dest;std::uint32_t rawRng=1;watch=true;auto status=updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,1,count);watch=false;
    require(status==AudioEventStatus50HE::Capacity&&rawRng==1&&count==123&&std::memcmp(&dest,&before,sizeof dest)==0,"insufficient output capacity transactional");
    input.contactMaterialA9=2;require(updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,3,count)==AudioEventStatus50HE::Input&&rawRng==1&&count==123,"unknown material rejects no substitute");
    input.contactMaterialA9=1;input.disabled24C=true;require(updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,3,count)==AudioEventStatus50HE::Complete&&count==0&&rawRng==1&&(dest.emitterFlags22C&8u),"source disabled owner sets emitter bit but makes no sound");
    input.disabled24C=false;input.vehicle220Present=false;dest={};require(updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,3,count)==AudioEventStatus50HE::Complete&&count==0&&rawRng==1&&dest.emitterFlags22C==0,"missing vehicle owner lazy skip");
    input.vehicle220Present=true;input.listenerDistance=540;require(updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,3,count)==AudioEventStatus50HE::Complete&&count==0&&rawRng==1,"original 540-unit audibility guard");
    input.listenerDistance=52;rawRng=1;count=123;require(updateMaterialAudio50HI(provider.catalog().parameters,input,state,dest,rawRng,command,3,count)==AudioEventStatus50HE::Complete&&rawRng==expectedRng&&count==2&&command[0].gain==.6f&&command[1].gain==.59f,"actual offset listener source gate, final raw volume overwrites spatial attenuation");
    require(allocations==0,"all watched hot calls allocated zero times");
    material_audio50HI::Provider missing;std::array<p::AudioVoice50HD,128>missingVoices{};p::AudioMixer50HD missingMixer;require(missingMixer.initialize(missingVoices.data(),missingVoices.size(),48000),"missing mixer");require(missing.prepare("work/native/project/disc","/tmp/rr-50hi-absent-audio-input",missingMixer,64,error),"missing audio preparation records absence");input.listenerDistance=0;rawRng=1;auto negative=missing.update(input,rawRng);require(missing.bank().missing==13&&negative.okay()&&negative.commandCount==2&&negative.results[0]==material_audio50HI::ApplyResult::Unavailable&&!missingMixer.active(64),"missing clips explicitly unavailable, no replacement");
    std::cout<<"{\"checks\":"<<checks<<",\"actual_pcm_ready\":13,\"material_sound_rows\":16,\"retained_collider_bindings\":"<<bindingCount<<",\"hot_allocations\":"<<allocations<<",\"mixed_nonzero_samples\":"<<nonzero<<",\"guest_execution\":false,\"scope\":\"focused material subscriber; not full D40/spatial audio or full-game regression\"}\n";
}
