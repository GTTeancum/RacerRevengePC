#include "effect_authoring50bg.h"
#include "impact_authoring49y.h"
#include <cstring>
namespace rr::assets {namespace {
struct Slice{const char*p;std::size_t n;};
Slice trim(Slice s){while(s.n&&(s.p[0]==' '||s.p[0]=='\t'||s.p[0]=='\r')){++s.p;--s.n;}while(s.n&&(s.p[s.n-1]==' '||s.p[s.n-1]=='\t'||s.p[s.n-1]=='\r'))--s.n;return s;}
bool equal(Slice s,const char*name){std::size_t n=std::strlen(name);if(s.n!=n)return false;for(std::size_t i=0;i<n;++i){char a=s.p[i],b=name[i];if(a>='A'&&a<='Z')a=static_cast<char>(a+32);if(b>='A'&&b<='Z')b=static_cast<char>(b+32);if(a!=b)return false;}return true;}
bool number(Slice s,float&v){s=trim(s);return parseImpactNumber49Y(s.p,s.n,v);}
}
bool readEffectControlAuthoring50BG(const char*text,std::size_t bytes,EffectControlAuthoring50BG&out) noexcept {
 if(!text||!bytes||bytes>32768)return false;
 for(std::size_t i=0;i<bytes;++i){unsigned char c=static_cast<unsigned char>(text[i]);if((c<32&&c!='\r'&&c!='\n'&&c!='\t')||c>126)return false;}
 EffectControlAuthoring50BG next{};bool active=false,found=false;
 for(std::size_t at=0;at<bytes;){const auto start=at;while(at<bytes&&text[at]!='\n')++at;const auto length=at-start;if(at<bytes)++at;if(length>255)return false;auto line=trim({text+start,length});if(!line.n||line.p[0]==';'||line.p[0]==':')continue;
  if(line.p[0]=='['){if(line.n<3||line.p[line.n-1]!=']')return false;active=equal(trim({line.p+1,line.n-2}),"General Info");if(active){if(found)return false;found=true;}continue;}
  if(!active)continue;std::size_t split=0;while(split<line.n&&line.p[split]!='=')++split;if(split==line.n)return false;auto key=trim({line.p,split});auto value=trim({line.p+split+1,line.n-split-1});
  if(equal(key,"EmitMode")){if(next.hasMode||value.n!=1||value.p[0]<'0'||value.p[0]>'3')return false;next.hasMode=true;next.mode=static_cast<unsigned>(value.p[0]-'0');}
  else if(equal(key,"OneShotTime")){if(next.hasOneShot||!number(value,next.oneShot))return false;next.hasOneShot=true;}
  else if(equal(key,"DelayTime")){if(next.hasDelay||!number(value,next.delay))return false;next.hasDelay=true;}
  else if(equal(key,"Velocity")){if(next.hasVelocity)return false;std::size_t begin=0;for(unsigned i=0;i<3;++i){std::size_t end=begin;while(end<value.n&&value.p[end]!=',')++end;if((i<2&&end==value.n)||(i==2&&end!=value.n)||!number({value.p+begin,end-begin},next.velocity[i]))return false;begin=end+1;}next.hasVelocity=true;}
 }
 if(!found)return false;out=next;return true;
}
bool prepareEffectTiming50BG(const EffectControlAuthoring50BG&in,std::uint32_t&mode,std::uint32_t&duration) noexcept {
 const auto selected=in.hasMode?in.mode:0u;if(selected>1)return false;std::uint32_t word=0;if(selected==1&&in.hasOneShot)std::memcpy(&word,&in.oneShot,4);mode=selected;duration=word;return true;
}
}
