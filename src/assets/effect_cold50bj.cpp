#include "effect_cold50bj.h"
#include <cstring>
namespace rr::assets {
bool buildEffectControlOwner50BJ(std::uint32_t id,rr::game::EffectProfile50BE profile,const char*text,std::size_t bytes,const EffectColdOpaque50BJ&opaque,rr::game::EffectOwner50BE&out) noexcept {
 using namespace rr::game;
 if(!id||static_cast<std::uint32_t>(profile)>static_cast<std::uint32_t>(EffectProfile50BE::Splash19DBE0))return false;
 EffectControlAuthoring50BG authored{};if(!readEffectControlAuthoring50BG(text,bytes,authored)||!authored.hasVelocity)return false;
 EffectOwner50BE next{};next.id=id;next.profile=profile;if(!prepareEffectTiming50BG(authored,next.mode1D8,next.duration1E4))return false;
 next.state.flags1D4=static_cast<std::uint8_t>((opaque.postBaseFlags1D4&0xf8u)|1u);
 next.state.position30[3]=next.state.previous40[3]=next.state.velocity60[3]=0x3f800000u;
 for(unsigned i=0;i<3;++i)std::memcpy(next.state.velocity60+i,authored.velocity+i,4);
 //19DC2C initializes the real splash+204 accumulator to zero. It is not
 // the19A5E0 intensity control or an opaque nonparticle adapter word.
 next.state.intensity204=profile==EffectProfile50BE::Particle19A5E0?0x3f800000u:
  profile==EffectProfile50BE::Splash19DBE0?0u:opaque.unusedNonParticleWord;
 // Original immediate Stop84 after registration. All fresh enabled bits are
 // already clear, including the conditional15F070 class; no deferred route.
 stopEffect50BC(next.state);out=next;return true;
}
}
