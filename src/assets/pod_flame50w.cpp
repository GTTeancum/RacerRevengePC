#include "pod_flame50w.h"
#include <sstream>
#include <locale>
#include <cmath>
#include <cfenv>
#include <cstring>
namespace rr::assets {
namespace {
bool equal(const std::string&a,const char*b){std::size_t n=0;for(;b[n];++n){if(n>=a.size())return false;auto c=a[n];if(c>='A'&&c<='Z')c+=32;auto d=b[n];if(d>='A'&&d<='Z')d+=32;if(c!=d)return false;}return n==a.size();}
const std::string*field(const MaterialFields48E&f,const char*name){for(const auto&v:f.authored)if(equal(v.name,name))return &v.value;return nullptr;}
bool noOtherAnimation(const MaterialFields48E&f,bool flame){for(const auto&v:f.authored){std::string name=v.name;for(auto&c:name)if(c>='A'&&c<='Z')c+=32;if(name.find("animation")!=std::string::npos){if(flame&&name=="vertexanimation"&&equal(v.value,"unique"))continue;if(!equal(v.value,"none"))return false;}}return true;}
bool row(const std::string&s,float(&out)[4]){std::istringstream in(s);in.imbue(std::locale::classic());for(unsigned i=0;i<4;++i){if(!(in>>out[i])||!std::isfinite(out[i])||std::fpclassify(out[i])==FP_SUBNORMAL)return false;if(i<3){char comma;if(!(in>>comma)||comma!=',')return false;}}in>>std::ws;return in.eof();}
}
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,const Material48E&shared,const Material48E&leftFlame,const Material48E&rightFlame,std::uint32_t leftId,std::uint32_t rightId,PodFlameOwners50W&out,PodFlameError50W&error){
 auto fail=[&](PodFlameError50W e){error=e;return false;};
 if(std::fegetround()!=FE_TONEAREST)return fail(PodFlameError50W::Arithmetic);
 if(!leftId||!rightId||leftId==rightId)return fail(PodFlameError50W::Models);
 const Scene48D*scenes[]={&left,&right};const Material48E*flames[]={&leftFlame,&rightFlame};
 for(unsigned side=0;side<2;++side){if(scenes[side]->materials.size()!=8)return fail(PodFlameError50W::Materials);for(unsigned j=0;j<8;++j)if(!equal(scenes[side]->materials[j],j&1?(side?"flame01r":"flame01l"):"01_1"))return fail(PodFlameError50W::Materials);}
 if(shared.passes.size()!=1||!equal(shared.passes[0].microprogram,"SpecularMapDualCompressed")||!noOtherAnimation(shared.passes[0].fields,false))return fail(PodFlameError50W::Shader);
 PodFlameOwners50W next{};
 for(unsigned side=0;side<2;++side){const auto&material=*flames[side];if(material.passes.size()!=1)return fail(PodFlameError50W::Shader);const auto&pass=material.passes[0];if(!equal(pass.microprogram,"DiffuseTexTransCompressed")||pass.textures.size()!=1||pass.textures[0].slot!=1||!equal(pass.textures[0].name,"flame"))return fail(PodFlameError50W::Shader);
  const auto*animation=field(pass.fields,"VertexAnimation");if(!animation||!equal(*animation,"unique")||!noOtherAnimation(pass.fields,true))return fail(PodFlameError50W::Animation);
  const char*keys[]={"TexGenMatrixRow1","TexGenMatrixRow2","TexGenMatrixRow3"};for(unsigned j=0;j<3;++j){const auto*value=field(pass.fields,keys[j]);if(!value||!row(*value,next.uniforms[side].rows[j]))return fail(PodFlameError50W::Rows);}
  next.uniforms[side+2]=next.uniforms[side];const auto id=side?rightId:leftId;next.models[side]={id,side*8,8,side+1};next.materials[side+1]={side+1,1};next.passes[side+1]={side,side,1,true};next.instances[side]={id,side+2};
  for(unsigned j=0;j<8;++j)next.references[side*8+j]=(j&1)?side+1:0;
  for(unsigned which=side;which<4;which+=2){auto&rec=next.records[which];rec.words[2]=0xffffffffu;rec.words[4]=which+1;rec.words[6]=side+1;rec.words[7]=1;}
 }
 // Static material has no animation target in this flame-only projection. Its
 // authored renderer fields stay external; the admitted filters never select it.
 next.materials[0]={0,1};next.passes[0]={rr::game::noFlameRecord50V,0,0,false};
 out=next;error=PodFlameError50W::None;return true;
}
}
