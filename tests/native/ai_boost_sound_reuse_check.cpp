// Reproduces the lap-2 AI boost crash path with the real native mixer and
// logical sound owner (no device, track or game state):
//   1. a boost sound starts (gets a channel token),
//   2. its one-shot PCM plays to the end on its own (voice inactive, but the
//      logical channel2C keeps the old token),
//   3. the next boost onset near the listener runs 233AE0 on that sound.
// Step 3 must not be refused: a finished voice is simply not playing.
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/logical_audio_owner.h"
#include "../../src/game/original_ai_logical_sound.h"
#include <cfenv>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
unsigned checks=0;
void need(bool value,const char* what){
    ++checks;
    if(!value)throw std::runtime_error(std::string("check failed: ")+what);
}
}

int main(){
    try{
        need(std::fegetround()==FE_TONEAREST,"rounding nearest");
        rr::platform::AudioVoice50HD voices[1]{};
        rr::platform::AudioMixer50HD mixer;
        rr::platform::LogicalAudioOwner channels;
        std::int16_t samples[4]{100,-100,100,-100}; // Short one-shot clip.
        need(mixer.initialize(voices,1,48000),"mixer initialize");
        need(channels.bind(mixer,0,1),"logical owner bind");
        const rr::platform::AudioClip50HD clip{samples,4,48000,1,0,0};
        need(channels.registerClip(0,clip,true),"register clip");
        auto* boost=channels.sound(0);need(boost!=nullptr,"sound slot");

        rr::game::OriginalSoundModeFields50KG mode{};
        const float listener[4]{0.f,0.f,0.f,1.f};

        // First boost: not playing yet, placement then start.
        auto spatial=rr::game::spatialAiLogicalSound233AE0(channels,*boost,mode,100.f,0,listener,false);
        need(spatial.status==rr::game::AiLogicalSoundStatus::complete,"first placement");
        need(channels.start(*boost,1.f,1.f,false)==rr::platform::LogicalAudioOwner::Result::complete,"first start");
        need(boost->channel2C!=-1&&channels.active(*boost),"first boost playing");

        // Let the one-shot finish on its own.
        std::int16_t out[2*256]{};
        need(mixer.mixStereo16(out,256),"mix");
        need(!mixer.active(0),"voice finished");
        std::printf("after finish: channel2C=%d active=%d\n",boost->channel2C,channels.active(*boost)?1:0);

        // Second boost onset near the listener: placement on the finished sound.
        spatial=rr::game::spatialAiLogicalSound233AE0(channels,*boost,mode,100.f,0,listener,false);
        std::printf("second placement: status=%u source_pc=0x%x returned=%d channel2C=%d\n",
            static_cast<unsigned>(spatial.status),spatial.sourcePC,spatial.sourceReturned?1:0,boost->channel2C);
        need(spatial.status==rr::game::AiLogicalSoundStatus::complete,"second placement on finished sound");
        need(boost->channel2C==-1,"finished channel cleared");
        // The boost then starts again exactly like the first one.
        need(channels.start(*boost,1.f,1.f,false)==rr::platform::LogicalAudioOwner::Result::complete,"second start");
        need(boost->channel2C!=-1&&channels.active(*boost),"second boost playing");
        // While it IS playing, placement still controls the live voice.
        spatial=rr::game::spatialAiLogicalSound233AE0(channels,*boost,mode,100.f,0,listener,false);
        need(spatial.sourcePC==0x233d78u,"live voice still reaches the control path");

        std::printf("ai boost sound reuse checks=%u passed\n",checks);
        return 0;
    }catch(const std::exception& e){
        std::fprintf(stderr,"%s\n",e.what());return 1;
    }
}
