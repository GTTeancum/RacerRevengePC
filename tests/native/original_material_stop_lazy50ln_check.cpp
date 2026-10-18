// Proposal-only focused check; not compiled or executed by its author.
#include "original_material_input_diagnostics50ln.h"
#include <sstream>
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace rr::game;
using G=OriginalPodMaterialInputGuard;
namespace diag=original_material_input_diagnostics50LN;
namespace core=original_pod_material_detail;
unsigned checks=0;
void need(bool value){++checks;if(!value)throw std::runtime_error("material input diagnostic assertion "+std::to_string(checks));}
float word(std::uint32_t v){float f;std::memcpy(&f,&v,4);return f;}
OriginalPodMaterialAudioReceipt call(const OriginalPodMaterialAudioInput& in,
 const OriginalAiAudioListener* listener,std::size_t count,const OriginalSoundModeFields50KG& mode){
 OriginalPodMaterialAudioState state;OriginalAiEmitterIdentity identity{};identity.session=1;
 need(projectOriginalPodMaterialSetup2C21C0(state));state.vehicle248=&identity;
 CoupledNoiseContext49M noise{};const auto before=noise.sharedRandomState;std::uint8_t flags=8;
 const auto result=core::dispatch179070(state,identity,flags,true,0,in,listener,count,mode,noise,[]()noexcept{return false;});
 if(result.status==OriginalPodMaterialAudioStatus::input){
  need(!state.inDispatch&&!state.blocked&&flags==8&&noise.sharedRandomState==before&&result.randomDraws==0&&result.sourcePC==0);
 }
 return result;
}
void stopRegression();
int main(){try{
 need(std::fegetround()==FE_TONEAREST);
 OriginalPodMaterialAudioInput in;in.channelArgument=2;in.vehicle58=0;in.position.v[3]=1;in.velocity.v[3]=1;
 OriginalAiAudioListener listener{};OriginalSoundModeFields50KG mode{};
 need(call(in,&listener,1,mode).status==OriginalPodMaterialAudioStatus::complete);
 auto verify=[&](OriginalPodMaterialAudioInput value,const OriginalAiAudioListener* listeners,std::size_t count,
     OriginalSoundModeFields50KG m,G guard,std::size_t row=0,unsigned axis=0){
  auto r=call(value,listeners,count,m);need(r.status==OriginalPodMaterialAudioStatus::input&&r.inputGuard==guard&&r.inputListener==row&&r.inputAxis==axis);
  std::ostringstream out;diag::write(out,value,0,listeners,count,m,std::fegetround(),r);
  need(out.str().find(std::string("guard=")+diag::guardName(guard))!=std::string::npos);
 };
 auto bad=in;bad.channelArgument=0;verify(bad,&listener,1,mode,G::channelArgument);
 bad=in;bad.vehicle58=1;verify(bad,&listener,1,mode,G::vehicleClass);
 bad=in;bad.contactMaterialA9=256;verify(bad,&listener,1,mode,G::contactMaterial);
 verify(in,&listener,5,mode,G::listenerCount);verify(in,nullptr,1,mode,G::listenerPointer);
 auto badMode=mode;badMode.reverseAngle9918=1;verify(in,&listener,1,badMode,G::soundMode);
 need(std::fesetround(FE_DOWNWARD)==0);verify(in,&listener,1,mode,G::rounding);need(std::fesetround(FE_TONEAREST)==0);
 for(unsigned axis=0;axis<4;++axis){
  bad=in;bad.position.v[axis]=word(0x7f800000);verify(bad,&listener,1,mode,G::positionScalar,0,axis);
  bad=in;bad.velocity.v[axis]=word(0x7fc00001);verify(bad,&listener,1,mode,G::velocityScalar,0,axis);
 }
 OriginalAiAudioListener listeners[2]{};
 for(unsigned axis=0;axis<3;++axis){
  listeners[1].position[axis]=word(0xff800000);verify(in,listeners,2,mode,G::listenerPositionScalar,1,axis);listeners[1].position[axis]=0;
  listeners[1].forward[axis]=word(0x7fc00001);verify(in,listeners,2,mode,G::listenerForwardScalar,1,axis);listeners[1].forward[axis]=0;
 }
 // Exact existing native predicate remains authoritative under current DAZ/FTZ.
 bad=in;bad.velocity.v[2]=word(1);const auto tiny=call(bad,&listener,1,mode);
 need(core::scalar(bad.velocity.v[2])?tiny.status==OriginalPodMaterialAudioStatus::complete:
      tiny.status==OriginalPodMaterialAudioStatus::input&&tiny.inputGuard==G::velocityScalar&&tiny.inputAxis==2);
 need(diag::bits(bad.velocity.v[2])==1&&std::string(diag::bitClass(bad.velocity.v[2]))=="subnormal"&&!diag::scalarBits(bad.velocity.v[2]));
 need(diag::scalarBits(-0.f)&&diag::bits(-0.f)==0x80000000u);
 need(!diag::scalarBits(word(0x7f800000))&&!diag::scalarBits(word(0x7fc00001)));
 bad=in;bad.channelArgument=0;bad.position.v[0]=word(0x7f800000);verify(bad,&listener,1,mode,G::channelArgument);
 bad=in;bad.position.v[1]=word(0x7f800000);bad.velocity.v[0]=word(0x7f800000);verify(bad,&listener,1,mode,G::velocityScalar,0,0);
 stopRegression();
 std::cout<<"material stop-lazy regression checks="<<checks<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

namespace {
// Deliberately cold test fixture: real native logical channels/mixer, two tiny
// test PCM clips, no device, application, track, source sample or gameplay claim.
struct StopFixture {
 rr::platform::AudioVoice50HD voices[2]{};
 rr::platform::AudioMixer50HD mixer;
 rr::platform::LogicalAudioOwner channels;
 std::int16_t samples[2]{1,-1};std::uint32_t group=0;
 OriginalPodMaterialAudioState state;
 OriginalAiEmitterIdentity identity{};
 CoupledNoiseContext49M noise{};
 OriginalPodMaterialAudioInput input{};
 OriginalAiAudioListener listener{};
 OriginalSoundModeFields50KG mode{};
 std::uint8_t flags=0xafu;
 StopFixture(){
  identity.session=1;need(projectOriginalPodMaterialSetup2C21C0(state));state.vehicle248=&identity;
  noise.sharedRandomState=0x12345678u;
  input.channelArgument=3;input.vehicle58=1;input.contactMaterialA9=-1;
  input.position.v[3]=input.velocity.v[3]=1;
  const std::uint32_t forward[4]={0x3cb317ffu,0x006447c3u,0x3f7ff056u,0x3f800000u};
  std::memcpy(listener.forward,forward,sizeof forward);listener.position[3]=1;
  need(mixer.initialize(voices,2,48000));need(channels.bind(mixer,0,2));
  rr::platform::AudioClip50HD clip{samples,2,48000,1,0,0};
  for(unsigned i=0;i<2;++i){
   need(channels.registerClip(i,clip,true));auto* sound=channels.sound(i);need(sound!=nullptr);
   need(channels.start(*sound,1.f,1.f,true)==rr::platform::LogicalAudioOwner::Result::complete);
   sound->elapsedBits24=10+i;sound->word7C=1;
   state.drive[i].sound.nativeOwner=&channels;state.drive[i].sound.nativeSound=sound;
   state.drive[i].sound.groupState44=&group;
  }
  state.driveCount=2;state.activeDriveKey23C=7;state.driveMaterial240=5;state.driveKey244=6;
 }
 OriginalPodMaterialAudioReceipt dispatch(bool ready=true,std::size_t count=1,bool present=true){
  return core::dispatch179070(state,identity,flags,ready,1,input,present?&listener:nullptr,count,mode,noise,[]()noexcept{return false;});
 }
};
}
void stopRegression(){
 need(!core::scalar(word(0x006447c3u))); // Captured runtime has DAZ/FTZ disabled.
 const std::uint32_t tiny[]={0x006447c3u,0x806447c3u,1u,0x80000001u,0x007fffffu,0x807fffffu};
 for(auto y:tiny){
  StopFixture f;f.listener.forward[1]=word(y);const auto seed=f.noise.sharedRandomState;
  const auto receipt=f.dispatch();
  need(receipt.status==OriginalPodMaterialAudioStatus::complete&&receipt.sourcePC==0x1792a8u&&receipt.inputGuard==G::none);
  need(receipt.stopped==2&&receipt.randomDraws==0&&f.noise.sharedRandomState==seed);
  need(f.flags==0xa7u&&!f.state.inDispatch&&!f.state.blocked);
  need(f.state.activeDriveKey23C==-1&&f.state.driveMaterial240==-1&&f.state.driveKey244==-1);
  for(unsigned i=0;i<2;++i){auto* s=f.channels.sound(i);need(!f.mixer.active(i)&&s->channel2C==-1&&s->elapsedBits24==0&&s->word7C==0);}
  need(diag::bits(f.listener.forward[1])==y); // No canonical input normalization.
 }
 // Channels1/2 retain exactly the previous scalar rejection and atomic input
 // boundary for Y, including the captured positive value and negative cases.
 for(unsigned channel=1;channel<=2;++channel)for(auto y:tiny){
  OriginalPodMaterialAudioInput in;in.channelArgument=channel;in.vehicle58=0;
  in.position.v[3]=in.velocity.v[3]=1;OriginalAiAudioListener l{};l.forward[2]=1;l.forward[1]=word(y);
  const auto r=call(in,&l,1,{});need(r.status==OriginalPodMaterialAudioStatus::input&&r.inputGuard==G::listenerForwardScalar&&r.inputAxis==1);
 }
 // Nonfinite/consumed-lane behavior remains unchanged on spatial channels.
 for(unsigned channel=1;channel<=2;++channel)for(unsigned axis=0;axis<3;++axis)
  for(auto bad:{0x7f800000u,0xff800000u,0x7fc00001u}){
   OriginalPodMaterialAudioInput in;in.channelArgument=channel;in.vehicle58=0;
   in.position.v[3]=in.velocity.v[3]=1;OriginalAiAudioListener l{};l.forward[2]=1;l.forward[axis]=word(bad);
   const auto r=call(in,&l,1,{});need(r.status==OriginalPodMaterialAudioStatus::input&&r.inputGuard==G::listenerForwardScalar&&r.inputAxis==axis);
  }
 for(unsigned channel=1;channel<=2;++channel)for(unsigned axis:{0u,2u})for(auto v:tiny){
  OriginalPodMaterialAudioInput in;in.channelArgument=channel;in.vehicle58=0;
  in.position.v[3]=in.velocity.v[3]=1;OriginalAiAudioListener l{};l.forward[2]=1;l.forward[axis]=word(v);
  const auto r=call(in,&l,1,{});need(r.status==OriginalPodMaterialAudioStatus::input&&r.inputGuard==G::listenerForwardScalar&&r.inputAxis==axis);
 }
 // No numerical lane is source-consumed by stop3. Verify that their raw bits
 // are retained while both actual channels still stop, rather than fake success.
 for(unsigned lane=0;lane<17;++lane)for(auto v:{1u,0x80000001u,0x7f800000u,0xff800000u,0x7fc00001u}){
  StopFixture f;float* value=lane<4?&f.input.position.v[lane]:lane<8?&f.input.velocity.v[lane-4]:
      lane<12?&f.listener.position[lane-8]:lane<16?&f.listener.forward[lane-12]:&f.input.speedRatio70;
  *value=word(v);const auto beforeInput=f.input;const auto beforeListener=f.listener;
  const auto seed=f.noise.sharedRandomState;const auto r=f.dispatch();
  need(r.status==OriginalPodMaterialAudioStatus::complete&&r.stopped==2&&f.flags==0xa7u);
  need(!f.mixer.active(0)&&!f.mixer.active(1)&&r.randomDraws==0&&f.noise.sharedRandomState==seed);
  need(diag::bits(*value)==v&&std::memcmp(&beforeInput,&f.input,sizeof f.input)==0&&
       std::memcmp(&beforeListener,&f.listener,sizeof f.listener)==0);
 }
 // Actual stop failure keeps the original committed prefix: first row stopped,
 // onset cleared, later invalid row blocked, remembered keys NOT reset.
 {
  StopFixture f;f.state.drive[1].sound.nativeOwner=nullptr;const auto seed=f.noise.sharedRandomState;
  auto r=f.dispatch();need(r.status==OriginalPodMaterialAudioStatus::owner&&r.sourcePC==0x293424u&&r.stopped==1);
  need(f.flags==0xa7u&&f.state.blocked&&!f.state.inDispatch&&f.noise.sharedRandomState==seed);
  need(!f.mixer.active(0)&&f.mixer.active(1));
  need(f.state.activeDriveKey23C==7&&f.state.driveMaterial240==5&&f.state.driveKey244==6);
 }
 {
  StopFixture f;auto r=f.dispatch(false);need(r.status==OriginalPodMaterialAudioStatus::owner&&r.sourcePC==0);
  need(f.flags==0xafu&&!f.state.inDispatch&&!f.state.blocked&&f.mixer.active(0)&&f.mixer.active(1));
 }
 // Listener shape and floating-point mode remain host admission for stop3.
 for(unsigned failure=0;failure<3;++failure){
  StopFixture f;
  if(failure==2)need(std::fesetround(FE_DOWNWARD)==0);
  const auto r=f.dispatch(true,failure==0?5:1,failure!=1);
  if(failure==2)need(std::fesetround(FE_TONEAREST)==0);
  const auto expected=failure==0?G::listenerCount:failure==1?G::listenerPointer:G::rounding;
  need(r.status==OriginalPodMaterialAudioStatus::input&&r.inputGuard==expected&&r.sourcePC==0);
  need(f.flags==0xafu&&!f.state.blocked&&f.mixer.active(0)&&f.mixer.active(1)&&r.randomDraws==0);
 }
 // The existing structural preflight is still mandatory for stop3.
 for(unsigned failure=0;failure<4;++failure){
  StopFixture f;
  if(failure==0)f.input.vehicle58=0;
  if(failure==1)f.input.contactMaterialA9=256;
  if(failure==2)f.mode.reverseAngle9918=1;
  if(failure==3)f.input.channelArgument=4;
  const auto r=f.dispatch();need(r.status==OriginalPodMaterialAudioStatus::input&&r.sourcePC==0);
  need(f.flags==0xafu&&!f.state.blocked&&f.mixer.active(0)&&f.mixer.active(1)&&r.randomDraws==0);
 }
}

// Keep auxiliary stop preflight cases separate from the matrix above.
