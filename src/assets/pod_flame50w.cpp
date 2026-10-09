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
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const std::vector<const Material48E*>&leftMaterials,
 const std::vector<const Material48E*>&rightMaterials,
 std::uint32_t leftId,std::uint32_t rightId,PodFlameOwners50W&out,PodFlameError50W&error){
 auto fail=[&](PodFlameError50W e){error=e;return false;};
 if(std::fegetround()!=FE_TONEAREST)return fail(PodFlameError50W::Arithmetic);
 if(!leftId||!rightId||leftId==rightId)return fail(PodFlameError50W::Models);
 const Scene48D*scenes[]={&left,&right};
 const std::vector<const Material48E*>*tables[]={&leftMaterials,&rightMaterials};
 for(unsigned side=0;side<2;++side){
  const auto count=scenes[side]->materials.size();
  if(!count||count>podFlameReferencesPerModel50W||tables[side]->size()!=count)
   return fail(PodFlameError50W::Materials);
 }
 PodFlameOwners50W next{};
 const std::string*flameNames[2]={};std::uint32_t referenceBase=0;
 for(unsigned side=0;side<2;++side){
  const auto&names=scenes[side]->materials;const auto&materials=*tables[side];
  unsigned flameReferences=0;
  for(std::size_t j=0;j<names.size();++j){
   if(names[j].empty()||!materials[j])return fail(PodFlameError50W::Materials);
   const auto&material=*materials[j];
   if(material.passes.size()!=1)return fail(PodFlameError50W::Shader);
   if(!noOtherAnimation(material.general,false))return fail(PodFlameError50W::Animation);
   const auto&pass=material.passes[0];
   if(equal(pass.microprogram,"SpecularMapDualCompressed")){
    if(!noOtherAnimation(pass.fields,false))return fail(PodFlameError50W::Animation);
    next.references[referenceBase+j]=0;continue;
   }
   if(!equal(pass.microprogram,"DiffuseTexTransCompressed")||pass.textures.size()!=1||
      pass.textures[0].slot!=1||!equal(pass.textures[0].name,"flame"))
    return fail(PodFlameError50W::Shader);
   const auto*animation=field(pass.fields,"VertexAnimation");
   if(!animation||!equal(*animation,"unique")||!noOtherAnimation(pass.fields,true))
    return fail(PodFlameError50W::Animation);
   if(flameNames[side]&&!equal(names[j],flameNames[side]->c_str()))
    return fail(PodFlameError50W::Materials);
   FlameUniformRows50W uniform{};
   const char*keys[]={"TexGenMatrixRow1","TexGenMatrixRow2","TexGenMatrixRow3"};
   for(unsigned k=0;k<3;++k){const auto*value=field(pass.fields,keys[k]);
    if(!value||!row(*value,uniform.rows[k]))return fail(PodFlameError50W::Rows);
   }
   // Every repeated reference must resolve to the same authored template even
   // when the cold caller decoded separate copies of the original PSM.
   if(flameNames[side]&&std::memcmp(&next.uniforms[side],&uniform,sizeof uniform))
    return fail(PodFlameError50W::Rows);
   flameNames[side]=&names[j];next.uniforms[side]=uniform;
   next.references[referenceBase+j]=side+1;++flameReferences;
  }
  if(flameReferences!=4)return fail(PodFlameError50W::Materials);
  next.uniforms[side+2]=next.uniforms[side];const auto id=side?rightId:leftId;
  const auto referenceCount=static_cast<std::uint32_t>(names.size());
  next.models[side]={id,referenceBase,referenceCount,side+1};
  next.materials[side+1]={side+1,1};next.passes[side+1]={side,side,1,true};
  next.instances[side]={id,side+2};referenceBase+=referenceCount;
  for(unsigned which=side;which<4;which+=2){auto&rec=next.records[which];rec.words[2]=0xffffffffu;rec.words[4]=which+1;rec.words[6]=side+1;rec.words[7]=1;}
 }
 if(equal(*flameNames[0],flameNames[1]->c_str()))return fail(PodFlameError50W::Materials);
 // Static materials have no animation target in this flame-only projection.
 // Their authored renderer fields stay external; the filters never select them.
 // This includes POD09's extra static 09_2 reference, without manufacturing a
 // flame at its slot or discarding the real flame reference that follows it.
 next.materials[0]={0,1};next.passes[0]={rr::game::noFlameRecord50V,0,0,false};
 out=next;error=PodFlameError50W::None;return true;
}
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,const Material48E&shared,const Material48E&leftFlame,const Material48E&rightFlame,std::uint32_t leftId,std::uint32_t rightId,PodFlameOwners50W&out,PodFlameError50W&error){
 auto fail=[&](PodFlameError50W e){error=e;return false;};
 if(std::fegetround()!=FE_TONEAREST)return fail(PodFlameError50W::Arithmetic);
 if(!leftId||!rightId||leftId==rightId)return fail(PodFlameError50W::Models);
 const Scene48D*scenes[]={&left,&right};const Material48E*flames[]={&leftFlame,&rightFlame};
 std::vector<const Material48E*>materials[2];
 for(unsigned side=0;side<2;++side){
  if(scenes[side]->materials.size()!=8)return fail(PodFlameError50W::Materials);
  for(unsigned j=0;j<8;++j){
   if(!equal(scenes[side]->materials[j],j&1?(side?"flame01r":"flame01l"):"01_1"))
    return fail(PodFlameError50W::Materials);
   materials[side].push_back(j&1?flames[side]:&shared);
  }
 }
 return buildPodFlameOwners50W(left,right,materials[0],materials[1],leftId,rightId,out,error);
}
}
