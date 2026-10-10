#include "pod_speech_authoring50hg_fixture.h"
#include "../../src/game/pod_speech50hg.h"
#include <cassert>
#include <atomic>
#include <cstdlib>
#include <new>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
static std::atomic<std::size_t> allocations{0};
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
static float asFloat(std::uint32_t bits){float v;std::memcpy(&v,&bits,4);return v;}
static std::uint32_t asBits(float v){std::uint32_t b;std::memcpy(&b,&v,4);return b;}
int main(int argc,char** argv){
    PodSpeechCounts50HG counts{};std::string error;
    assert(pod_speech50HG::readPod01Counts(counts,error));
    std::array<TrackIntroLine50HG,3> lines{};assert(pod_speech50HG::readTAIntroLines(lines,error));
    if(argc>1){float chance=0;assert(pod_speech50HG::readPlayerSpeechChance(chance,error,argv[1]));assert(chance==.35f);}
    const auto hotAllocations=allocations.load();
    std::uint32_t random=1,expected=1;PodSpeechState50HG speech{};
    assert(initializePodSpeech50HG(counts,random,speech)==PodSpeechStatus50HG::Complete);
    for(unsigned n=0;n<6;++n)expected=expected*0x41c64e6du+0x3039u;
    assert(random==expected&&speech.initialized&&speech.cooldown224==5.f);
    const auto afterInit=random;
    const SpeechPosition50HG player{0,0};SpeechPosition50HG positions[2]{{0,0},{500,0}};
    SpeechListeners50HG listeners{positions,1};AudioCommand50HE commands[1]{};std::size_t count=99;
    auto selected=selectPodSpeech50HG(PodSpeechCategory50HG::LowDamage,counts,player,listeners,
        random,speech,commands,1,count);
    assert(selected.status==PodSpeechStatus50HG::Complete&&!selected.emitted&&count==0&&random==afterInit);
    UndertrackPlayerPrefix50ET prefix{};prefix.counter109C=asBits(.01f);
    assert(advanceSpeechTimers50HG(.016f,prefix,speech)&&asFloat(prefix.counter109C)<0.f);
    speech.cooldown224=0;positions[0]={200,0};count=99;
    selected=selectPodSpeech50HG(PodSpeechCategory50HG::LowDamage,counts,player,listeners,random,speech,commands,1,count);
    assert(!selected.audible&&!selected.emitted&&count==0&&random==afterInit);
    positions[1]={199,0};listeners.count=2;
    const auto cursor=speech.cursors[0];
    selected=selectPodSpeech50HG(PodSpeechCategory50HG::LowDamage,counts,player,listeners,random,speech,commands,0,count);
    assert(selected.status==PodSpeechStatus50HG::Capacity&&speech.cursors[0]==cursor&&random==afterInit);
    const unsigned sizes[]={4,4,6,5,6,6};const unsigned offsets[]={0,4,8,14,0,6};
    for(unsigned category=0;category<6;++category)for(unsigned n=0;n<sizes[category]*2;++n){
        speech.cooldown224=0;const auto previous=speech.cursors[category];
        selected=selectPodSpeech50HG(static_cast<PodSpeechCategory50HG>(category),counts,player,listeners,
            random,speech,commands,1,count);
        assert(selected.emitted&&count==1&&selected.randomDraws==0&&random==afterInit);
        assert(commands[0].cue.key==offsets[category]+previous+1);
        assert(speech.cursors[category]==(previous+1)%sizes[category]&&speech.cooldown224==5.f);
        assert(commands[0].cue.bank==(category<4?AudioBank50HE::PodCurses:AudioBank50HE::PodTaunts));
    }
    for(auto category:{PodSpeechCategory50HG::Death,PodSpeechCategory50HG::Win,PodSpeechCategory50HG::Lose}) {
        const auto before=random;selected=selectPodSpeech50HG(category,counts,player,listeners,random,speech,commands,1,count);
        assert(selected.emitted&&selected.randomDraws==1&&random==before*0x41c64e6du+0x3039u);
        assert(speech.cooldown224==5.f&&commands[0].cue.bank==AudioBank50HE::PodSpecial);
        if(category==PodSpeechCategory50HG::Death)assert(commands[0].cue.key>=1&&commands[0].cue.key<=4);
        if(category==PodSpeechCategory50HG::Win)assert(commands[0].cue.key>=5&&commands[0].cue.key<=7);
        if(category==PodSpeechCategory50HG::Lose)assert(commands[0].cue.key>=8&&commands[0].cue.key<=9);
    }
    DamageSpeechInput50HG input{};input.listeners=listeners;input.damage7D0=.8f;
    input.playerSpeechChance138=1.f;speech.cooldown224=0;prefix={0,0x82};const auto rawBefore=random;
    auto receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,0,count);
    assert(receipt.status==PodSpeechStatus50HG::Capacity&&prefix.counter109C==0&&prefix.flags10CC==0x82&&random==rawBefore);
    receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
    assert(receipt.status==PodSpeechStatus50HG::Complete&&receipt.eventProduced&&receipt.eventAccepted&&
        receipt.eventDispatched&&receipt.playerChancePassed&&receipt.voiceEmitted&&!receipt.announcerRequested);
    assert(count==1&&commands[0].cue.bank==AudioBank50HE::PodCurses&&commands[0].cue.key<=4);
    assert(asFloat(prefix.counter109C)==10.f&&prefix.flags10CC==0x86&&receipt.randomDraws==1);
    const auto afterDamage=random;receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
    assert(!receipt.eventProduced&&count==0&&random==afterDamage);
    assert(afterRepairSpeechState50HG(.6f,.4f,.5f,prefix)&&asFloat(prefix.counter109C)==3.f&&prefix.flags10CC==0x82);
    input.damage7D0=.7f;prefix.counter109C=0;
    receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);assert(!receipt.eventProduced);
    input.damage7D0=.8f;input.playerSpeechChance138=0.f;input.vehicle60=16;input.vehicle60Known=true;
    receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
    assert(receipt.eventProduced&&receipt.announcerRequested&&receipt.announcerType==12&&
        receipt.announcerRacer==16&&receipt.announcerVariant==1&&!receipt.voiceEmitted&&count==0);
    prefix.counter109C=0;
    receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
    assert(receipt.eventProduced&&!receipt.announcerRequested&&!receipt.voiceEmitted);
    for(unsigned which=0;which<3;++which){
        input.vehicle3C=which==0?1:0;input.manager1B0=which==1?16:0;input.vehicle40=which==2?1:0;
        prefix.counter109C=0;prefix.flags10CC=0;const auto before=random;
        receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
        assert(receipt.eventProduced&&!receipt.voiceEmitted&&!receipt.announcerRequested&&random==before);
    }
    input.vehicle3C=0;input.vehicle40=0;input.manager1B0=0;
    input.damage7D0=std::numeric_limits<float>::quiet_NaN();const auto before=random;count=71;
    receipt=damageSpeech50HG(input,counts,prefix,random,speech,commands,1,count);
    assert(receipt.status==PodSpeechStatus50HG::Input&&count==71&&random==before);
    // Production and later dispatch preserve shared-RNG phase order.
    input.damage7D0=.8f;input.playerSpeechChance138=1.f;prefix={};speech.cooldown224=0;
    DamageSpeechEvent50HG event{};const auto beforeQueue=random;
    receipt=produceDamageSpeechEvent50HG(input,prefix,event);
    assert(receipt.eventProduced&&event.pending&&random==beforeQueue);
    // An earlier effects stage may consume shared RNG before the event flush.
    random=random*0x41c64e6du+0x3039u;const auto beforeFlush=random;
    receipt=dispatchDamageSpeechEvent50HG(input,counts,event,random,speech,commands,0,count);
    assert(receipt.status==PodSpeechStatus50HG::Capacity&&event.pending&&random==beforeFlush);
    receipt=dispatchDamageSpeechEvent50HG(input,counts,event,random,speech,commands,1,count);
    assert(receipt.voiceEmitted&&!event.pending&&random==beforeFlush*0x41c64e6du+0x3039u);
    const auto afterFlush=random;receipt=dispatchDamageSpeechEvent50HG(input,counts,event,random,speech,commands,1,count);
    assert(count==0&&random==afterFlush);
    std::uint8_t flags=0xe1;
    for(unsigned camera=1;camera<=3;++camera){
        assert(trackIntroSpeech50HG(2,camera,lines.data(),3,flags,commands,1,count)==AudioEventStatus50HE::Complete);
        assert(count==1&&commands[0].cue.bank==AudioBank50HE::TrackIntro&&commands[0].cue.key==camera&&flags==0xe1);
    }
    flags=0xe5;
    assert(trackIntroSpeech50HG(2,1,lines.data(),3,flags,commands,1,count)==AudioEventStatus50HE::Complete&&count==0&&flags==0xe3);
    assert(trackIntroSpeech50HG(2,1,lines.data(),3,flags,commands,1,count)==AudioEventStatus50HE::Complete&&count==0&&flags==0xe1);
    assert(trackIntroSpeech50HG(0,1,lines.data(),3,flags,commands,1,count)==AudioEventStatus50HE::Complete&&count==0);
    assert(trackIntroSpeech50HG(2,1,lines.data(),3,flags,commands,0,count)==AudioEventStatus50HE::Capacity&&flags==0xe1);
    assert(allocations.load()==hotAllocations);
    std::cout<<"PASS 50HG: authenticated nine counts; six source shared-RNG cold draws; all40 Pod01 category selections; three authored TA intro transitions; exact rotation/cooldown/audibility; real queued damage event/chance/announcer boundary; capacity and error atomicity; zero hot allocations\n";
}
