#pragma once
// Test-only bounded original-word secondary-wrapper execution. No native header or application
// formulas are included. Expected state is produced solely by extracted words.
// Instruction operation ordering follows the inherited48H opcode interpreter,
// extended only for the recorded bounded wrapper block set and with mandatory per-PC/branch accounting.
#include "../../evidence/secondaryWrapper49M/numeric-source/secondary_words49m.h"
#include "../../work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
namespace secondary49m_oracle {
using U=std::uint32_t;
inline U bits(float x){U w;std::memcpy(&w,&x,4);return w;}
inline float fp(U x){float f;std::memcpy(&f,&x,4);return f;}
inline float add(float a,float b){volatile float x=a+b;return x;}
inline float sub(float a,float b){volatile float x=a-b;return x;}
inline float mul(float a,float b){volatile float x=a*b;return x;}
inline float div(float a,float b){volatile float x=a/b;return x;}
struct Machine {
 static constexpr U joint=0x1000,bodyA=0x2000,bodyB=0x3000,sceneA=0x4000,sceneB=0x5000,input=0x6000,stack=0xf000,stop=0xfffffffcu;
 std::array<std::array<U,4>,32> g{};
 std::array<float,32> f{};
 std::array<std::array<float,4>,32> v{};
 std::array<float,4> va{};
 std::array<U,0x10000/4> memory{};
 std::array<U,16> staticData{};
 std::array<U,20> effectiveGlobals{};
 std::array<U,16> rootStaticData{};
 std::array<bool,0x10000/4> initialized{};
 float q=0;
 U cursor=stop;
 std::array<U,32> callTargets{};std::size_t callCount=0;
 mutable std::array<std::uint64_t,0x10000/4> reads{};
 std::array<std::uint64_t,0x10000/4> writes{};
 std::array<std::uint64_t,secondary49m_words::total_words> hits{},taken{},notTaken{};
 U pc=0,flags=0;float acc=0;bool condition=false;std::uint64_t steps=0,callSteps=0;
 std::size_t index(U p) const {
  for(const auto& b:secondary49m_words::blocks)if(p>=b.start&&p-b.start<b.count*4&&!(p&3))return b.index+(p-b.start)/4;
  throw std::runtime_error("oracle PC outside secondary-wrapper blocks: "+std::to_string(p));
 }
 U word(U p) const {
  for(const auto& b:secondary49m_words::blocks)if(p>=b.start&&p-b.start<b.count*4&&!(p&3))return b.words[(p-b.start)/4];
  throw std::runtime_error("oracle unknown PC "+std::to_string(p));
 }
 U read(U a) const {
  if(a>=0x433b80&&a<0x433bc0&&!(a&3))return rootStaticData[(a-0x433b80)/4];
  if(a>=0x434390&&a<0x4343d0&&!(a&3))return staticData[(a-0x434390)/4];
  if(a>=0x4346c0&&a<0x434710&&!(a&3))return effectiveGlobals[(a-0x4346c0)/4];
  if((a&3)||a/4>=memory.size())throw std::runtime_error("oracle read outside fixture: "+std::to_string(a)+" pc="+std::to_string(pc));
  if(!initialized[a/4])throw std::runtime_error("oracle uninitialized synthetic field: "+std::to_string(a)+" pc="+std::to_string(pc));
  ++reads[a/4];return memory[a/4];
 }
 void write(U a,U w){
  if(a>=0x433b80&&a<0x433bc0&&!(a&3)){rootStaticData[(a-0x433b80)/4]=w;return;}
  if(a>=0x434390&&a<0x4343d0&&!(a&3)){staticData[(a-0x434390)/4]=w;return;}
  if(a>=0x4346c0&&a<0x434710&&!(a&3)){effectiveGlobals[(a-0x4346c0)/4]=w;return;}
  if((a&3)||a/4>=memory.size())throw std::runtime_error("oracle write outside fixture: "+std::to_string(a));
  ++writes[a/4];initialized[a/4]=true;memory[a/4]=w;
 }
 void store(U a,float x){write(a,bits(x));}
 float load(U a)const{return fp(read(a));}
 bool execute(U p,U& target){
  pc=p;++steps;if(++callSteps>20000)throw std::runtime_error("oracle bounded instruction budget");
  const auto ix=index(p);++hits[ix];const U w=word(p),op=w>>26,rs=(w>>21)&31u,rt=(w>>16)&31u,rd=(w>>11)&31u;
  const auto im=static_cast<std::int16_t>(w&65535u);
  auto address=[&](){return g[rs][0]+im;};
  auto branch=[&](bool take){++(take?taken[ix]:notTaken[ix]);target=take?p+4+static_cast<std::int32_t>(im)*4:p+8;return true;};
  if(w==0)return false;
  if(op==9)g[rt][0]=g[rs][0]+im;
  else if(op==12)g[rt][0]=g[rs][0]&(w&65535u);
  else if(op==13)g[rt][0]=g[rs][0]|(w&65535u);
  else if(op==15)g[rt][0]=(w&65535u)<<16;
  else if(op==0x23)g[rt][0]=read(address());
  else if(op==0x20){U a=address();g[rt][0]=static_cast<std::int8_t>((read(a&~3u)>>((a&3)*8))&255u);}
  else if(op==0x28){U a=address(),old=read(a&~3u),s=(a&3u)*8;write(a&~3u,(old&~(255u<<s))|((g[rt][0]&255u)<<s));}
  else if(op==0x24){U a=address();g[rt][0]=(read(a&~3u)>>((a&3)*8))&255u;}
  else if(op==0x2b)write(address(),g[rt][0]);
  else if(op==0x31)f[rt]=load(address());
  else if(op==0x39)store(address(),f[rt]);
  else if(op==0x1e){U a=address();for(unsigned i=0;i<4;++i)g[rt][i]=read(a+4*i);}
  else if(op==0x1f){U a=address();for(unsigned i=0;i<4;++i)write(a+4*i,g[rt][i]);}
  else if(op==0x36){U a=address();for(unsigned i=0;i<4;++i)v[rt][i]=load(a+4*i);}
  else if(op==0x3e){U a=address();for(unsigned i=0;i<4;++i)store(a+4*i,v[rt][i]);}
  else if(op==4)return branch(g[rs][0]==g[rt][0]);
  else if(op==5)return branch(g[rs][0]!=g[rt][0]);
  else if(op==7)return branch(static_cast<std::int32_t>(g[rs][0])>0);
  else if(op==3){g[31][0]=p+8;target=(w&0x3ffffffu)*4;return true;}
  else if(op==0&&(w&63u)==9){target=g[rs][0];g[rd][0]=p+8;if(callCount>=callTargets.size())throw std::runtime_error("wrapper call budget");callTargets[callCount++]=target;return true;}
  else if(op==0&&(w&63u)==8){target=g[rs][0];return true;}
  else if((w&0xfc0007ffu)==0x70000628u&&rt==0)g[rd]=g[rs];
  else if(op==0x11){
   if(rs==4)f[rd]=fp(g[rt][0]);
   else if(rs==8)return branch(condition==((rt&1)!=0));
   else if(rs==16){
    const U fd=(w>>6)&31u,fn=w&63u;
    switch(fn){
     case 0:f[fd]=add(f[rd],f[rt]);break;
     case 1:f[fd]=sub(f[rd],f[rt]);break;
     case 2:f[fd]=mul(f[rd],f[rt]);break;
     case 3:f[fd]=div(f[rd],f[rt]);break;
     case 4:f[fd]=Ps2EeSqrt14(f[rt],flags);break;
     case 6:f[fd]=f[rd];break;
     case 7:f[fd]=-f[rd];break;
     case 0x18:acc=add(f[rd],f[rt]);break;
     case 0x19:acc=sub(f[rd],f[rt]);break;
     case 0x1a:acc=mul(f[rd],f[rt]);break;
     case 0x1c:f[fd]=add(acc,mul(f[rd],f[rt]));break;
     case 0x1d:f[fd]=sub(acc,mul(f[rd],f[rt]));break;
     case 0x28:{const auto a=static_cast<std::int32_t>(bits(f[rd])),b=static_cast<std::int32_t>(bits(f[rt]));f[fd]=fp(static_cast<U>((a<0&&b<0)?(a<b?a:b):(a>b?a:b)));break;}
     case 0x32:condition=f[rd]==f[rt];break;
     case 0x34:condition=f[rd]<f[rt];break;
     case 0x36:condition=f[rd]<=f[rt];break;
     default:throw std::runtime_error("oracle unsupported FPU instruction "+std::to_string(w));
    }
   }else throw std::runtime_error("oracle unsupported COP1");
  }else if(op==0x12){
   const U fn=w&63u,fd=(w>>6)&31u,mask=(w>>21)&15u,sp=w&0x7ffu;
   if(w==0x48222800u){for(unsigned i=0;i<4;++i)g[2][i]=bits(v[5][i]);}
   else if(rs>=16&&(fn<16||(fn>=0x18&&fn<=0x1b)||fn==0x28||fn==0x2a||fn==0x2c)){
    auto dst=v[fd];for(unsigned i=0;i<4;++i)if(mask&(8u>>i)){
     const float a=v[rd][i],b=v[rt][fn==0x28||fn==0x2a||fn==0x2c?i:(fn&3)];
     if(fn<4||fn==0x28)dst[i]=add(a,b);
     else if(fn<8||fn==0x2c)dst[i]=sub(a,b);
     else if(fn<12)dst[i]=add(va[i],mul(a,b));
     else if(fn<16)dst[i]=sub(va[i],mul(a,b));
     else dst[i]=mul(a,b);
    }v[fd]=dst;
   }else if(rs>=16&&((sp>=0x1bc&&sp<=0x1bf)||(sp>=0xbc&&sp<=0xbf))){
    for(unsigned i=0;i<4;++i)if(mask&(8u>>i)){float product=mul(v[rd][i],v[rt][sp&3]);va[i]=sp>=0x1bc?product:add(va[i],product);}
   }else if(sp==0x2fe){for(unsigned i=0;i<3;++i)va[i]=mul(v[rd][(i+1)%3],v[rt][(i+2)%3]);}
   else if(fn==0x2e){auto dst=v[fd];for(unsigned i=0;i<3;++i)dst[i]=sub(va[i],mul(v[rd][(i+1)%3],v[rt][(i+2)%3]));v[fd]=dst;}
   else if(w==0x4be5233cu)v[5]=v[4];
   else if(w==0x4b05283du)va[0]=add(v[5][0],v[5][1]);
   else if(w==0x4a6503beu){const float d=v[5][0];if(d<0||!std::isfinite(d))throw std::runtime_error("outside finite nonnegative normalizer domain");if(d==0)q=fp((bits(d)&0x80000000u)|0x7f7fffffu);else{volatile float root=std::sqrt(d);q=div(1.0f,root);}}
   else if(w==0x4a0003bfu){}
   else if(w==0x4bc0219cu){for(unsigned i=0;i<3;++i)v[6][i]=mul(v[4][i],q);}
   else throw std::runtime_error("oracle unsupported VU instruction "+std::to_string(w)+" pc="+std::to_string(pc));
  }else throw std::runtime_error("oracle unsupported instruction "+std::to_string(w)+" pc="+std::to_string(pc));
  g[0]={};v[0]={0,0,0,1};return false;
 }
 void run(U entry){
  U p=entry;callSteps=0;g[31][0]=stop;g[29][0]=stack;v[0]={0,0,0,1};
  while(p!=stop){U next=0,unused=0;if(execute(p,next)){if(execute(p+4,unused))throw std::runtime_error("branch in original delay slot");p=next;}else p+=4;}
 }
 void constructor(){
  // Numeric/default continuation only. Base prefix and saved frame are an
  // explicit synthetic boundary, never a simulated inherited constructor.
  U p=0x1b6214;callSteps=0;g[29][0]=stack-0xe0;v[0]={0,0,0,1};
  write(stack-0xe0,stop);write(stack-0xe0+0x1c,joint);
  while(p!=stop){U next=0,unused=0;if(execute(p,next)){if(execute(p+4,unused))throw std::runtime_error("branch in delay slot");p=next;}else p+=4;}
 }
 void call(U entry,U a0=joint,U a1=0,U a2=0){g[4][0]=a0;g[5][0]=a1;g[6][0]=a2;run(entry);}
 void beginWrapper(U vehicle,float dt){g[4][0]=vehicle;g[29][0]=stack;g[28][0]=0x10000;g[31][0]=stop;f[12]=dt;v[0]={0,0,0,1};cursor=0x2188b0;callSteps=0;callCount=0;}
 // Execute to an original PC or a specifically supplied callback entry. The
 // callback's JALR and delay instruction have already executed on return.
 void until(U boundary){while(cursor!=boundary){if(cursor==stop)throw std::runtime_error("wrapper ended before boundary");U next=0,unused=0;if(execute(cursor,next)){if(execute(cursor+4,unused))throw std::runtime_error("branch in wrapper delay slot");cursor=next;}else cursor+=4;}}
 void resumeSuppliedCallback(){cursor=g[31][0];}
 explicit Machine(bool initializeStatic=true){v[0]={0,0,0,1};for(U a=stack-0x2000;a<stack;a+=4)write(a,0);if(initializeStatic){run(0x3c9cf0);run(0x3c9290);}}
};
}
