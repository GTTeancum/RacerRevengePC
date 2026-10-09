#include "flame_bindings50v.h"
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
bool overlap(const void*a,std::size_t an,const void*b,std::size_t bn)noexcept{if(!an||!bn)return false;auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);return x<=y?y-x<an:x-y<bn;}
bool valid(const void*p,std::size_t n,std::size_t size)noexcept{return !n||(p&&reinterpret_cast<std::uintptr_t>(p)%alignof(std::uint32_t)==0&&n<=std::numeric_limits<std::size_t>::max()/size&&reinterpret_cast<std::uintptr_t>(p)<=std::numeric_limits<std::uintptr_t>::max()-n*size);}
bool range(std::uint32_t first,std::uint32_t count,std::size_t total)noexcept{return first<=total&&count<=total-first;}
}
FlameResult50V bindPreparedFlameModels50V(const FlameModel50V(&models)[4],std::uint32_t owner,const FlameViews50V&v,FlameRecord50V*records,std::size_t count,FlameWorkspace50V&w)noexcept{
 auto fail=[](FlameStatus50V status,std::uint32_t pc){return FlameResult50V{status,pc,0};};
 if(count>128||v.materialCount>64||v.passCount>256||v.instanceCount>512||v.referenceCount>64)return fail(FlameStatus50V::Limit,0);
 if(!valid(records,count,sizeof(*records))||!valid(v.materials,v.materialCount,sizeof(*v.materials))||!valid(v.passes,v.passCount,sizeof(*v.passes))||!valid(v.instances,v.instanceCount,sizeof(*v.instances))||!valid(v.references,v.referenceCount,sizeof(*v.references)))return fail(FlameStatus50V::Storage,0);
 struct Span{const void*p;std::size_t n;};const Span reads[]={{&models,sizeof models},{&v,sizeof v},{v.materials,v.materialCount*sizeof(*v.materials)},{v.passes,v.passCount*sizeof(*v.passes)},{v.instances,v.instanceCount*sizeof(*v.instances)},{v.references,v.referenceCount*sizeof(*v.references)}};
 if(overlap(records,count*sizeof(*records),&w,sizeof w))return fail(FlameStatus50V::Storage,0);
 for(const auto&s:reads)if(overlap(s.p,s.n,records,count*sizeof(*records))||overlap(s.p,s.n,&w,sizeof w))return fail(FlameStatus50V::Storage,0);
 if(!owner||!models[0].modelId||!models[1].modelId)return fail(FlameStatus50V::InvalidBinding,0x2208d0);
 for(std::size_t i=0;i<v.materialCount;++i)if(!range(v.materials[i].firstPass,v.materials[i].passCount,v.passCount)||v.materials[i].passCount>4)return fail(FlameStatus50V::InvalidBinding,0x29bd00);
 for(std::size_t i=0;i<v.passCount;++i){const auto&p=v.passes[i];if((p.prototype!=noFlameRecord50V&&p.prototype>=count)||!range(p.firstInstance,p.instanceCount,v.instanceCount)||(!p.instanceTablePresent&&p.instanceCount))return fail(FlameStatus50V::InvalidBinding,0x29b1d0);
  for(unsigned j=0;j<p.instanceCount;++j){const auto&pair=v.instances[p.firstInstance+j];if(pair.record>=count||(j&&v.instances[p.firstInstance+j-1].modelId>=pair.modelId))return fail(FlameStatus50V::InvalidBinding,0x29b228);}}
 for(const auto&m:models)if(m.modelId&&(!range(m.firstReference,m.referenceCount,v.referenceCount)||m.referenceCount>16))return fail(FlameStatus50V::InvalidBinding,0x26ef78);
 if(count)std::memcpy(w.next,records,count*sizeof(*records));std::uint32_t writes=0;
 auto lookup=[&](const FlamePass50V&p,std::uint32_t model){if(p.prototype==noFlameRecord50V)return noFlameRecord50V;if(!w.next[p.prototype].words[7]||!model)return p.prototype;if(!p.instanceTablePresent)return noFlameRecord50V;for(unsigned i=0;i<p.instanceCount;++i){const auto&x=v.instances[p.firstInstance+i];if(x.modelId>=model)return x.modelId==model?x.record:noFlameRecord50V;}return noFlameRecord50V;};
 for(unsigned which=0;which<4;++which){const auto&m=models[which];if(!m.modelId)continue;
  for(unsigned j=0;j<m.referenceCount;++j){const auto id=v.references[m.firstReference+j];if(id>=v.materialCount)return fail(FlameStatus50V::InvalidBinding,0x298e60);const auto&mat=v.materials[id];for(unsigned k=0;k<mat.passCount;++k){const auto&p=v.passes[mat.firstPass+k];if(p.prototype!=noFlameRecord50V&&w.next[p.prototype].words[7]&&lookup(p,m.modelId)==noFlameRecord50V)return fail(FlameStatus50V::NeedsColdInstance,0x29b050);}}
  for(unsigned j=0;j<m.referenceCount;++j){const auto id=v.references[m.firstReference+j];if(m.flameFilter!=0xffffffffu&&id!=m.flameFilter)continue;const auto&mat=v.materials[id];bool installed=false;
   for(unsigned k=0;k<mat.passCount;++k){const auto&p=v.passes[mat.firstPass+k];if(!p.instanceTablePresent)continue;const auto target=lookup(p,m.modelId);if(target==noFlameRecord50V)continue;if(writes==256)return fail(FlameStatus50V::Limit,0x29bde4);const auto tag=(which&1u)?0x20ee90u:0x20f110u;w.next[target].words[0]=tag;w.next[target].words[1]=owner;w.writes[writes++]={target,tag,owner};installed=true;}
   if(!installed)return fail(FlameStatus50V::UnsupportedDiagnostic,0x29be10);
  }
 }
 if(count)std::memcpy(records,w.next,count*sizeof(*records));return {FlameStatus50V::Complete,0,writes};
}
}
