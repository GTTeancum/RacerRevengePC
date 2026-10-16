// Source-only recovery; no build, test, decoder or gameplay execution performed.
#include "original_ui_controls_model50kg.h"
#include "original_ui_light_manager_lifetime50kg.h"
#include "original_ui_user_rotate50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::game { namespace {
using I=SceneNodeIndex49P;
using D=OriginalUiControlsModelDiagnostic50KG;
using E=OriginalUiControlsModelError50KG;
using Matrix=std::array<float,16>;
bool fail(D& d,E e,std::uint32_t pc)noexcept{d.error=e;d.sourcePC=pc;return false;}
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
float div(float a,float b)noexcept{volatile float r=a/b;return r;}
Matrix product(const Matrix& a,const Matrix& b)noexcept{
 Matrix out{};for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){float x=mul(b[c],a[4*r]);x=add(x,mul(b[4+c],a[4*r+1]));x=add(x,mul(b[8+c],a[4*r+2]));out[4*r+c]=add(x,mul(b[12+c],a[4*r+3]));}return out;
}
bool finite(const Matrix& m)noexcept{for(float x:m)if(!std::isfinite(x))return false;return true;}
bool normalize(std::array<float,3>& v)noexcept{
 const float q=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));if(!std::isfinite(q)||q<=0)return false;
 volatile float inv=1.f/std::sqrt(q);for(auto& x:v){x=mul(x,inv);if(!std::isfinite(x))return false;}return true;
}
struct ModelGraph {
    std::array<SceneNode49P,maxSceneNodes49P> nodes{};
    std::array<I,assets::maxModelNodes49O> sources{};
    std::array<std::array<char,128>,assets::maxModelNodes49O> names{};
    std::array<std::uint8_t,assets::maxModelNodes49O> palette{};
    std::uint32_t count=0;
    SceneGraph49P view() noexcept {return {nodes.data(),count?count+1:0};}
    bool build(const assets::PsgHierarchyView49O& psg,const std::array<float,3>& position,
        const std::array<float,3>& look,const std::array<float,3>& up,bool character) noexcept {
        if (!psg.nodeCount||psg.nodeCount>assets::maxModelNodes49O||psg.nodeCount+1>maxSceneNodes49P)
            return false;
        count=psg.nodeCount;
        //2893A0 starts local identity, world zero and dirty80. Root pose below
        // replaces local through native289EC0-equivalent branch. PSG source
        // loader supplies its original local matrices and flagsA4.
        auto& root=nodes[0];root={};
        root.sceneFlags190=character?0x88:0x80; // common character root retains its character flag
        if (!originalUiUserRotatePose50KG(position,look,up,root.localC0)) return false;
        for (std::uint32_t n=0;n<count;++n) {
            assets::PsgNode49O source{};if (!assets::readPsgNode49O(psg,n,source)) return false;
            if (n==0?source.parent!=-1:source.parent<0) return false;
            const auto index=n+1;
            const auto parent=source.parent<0?0u:static_cast<std::uint32_t>(source.parent)+1;
            auto& node=nodes[index];node={};node.sceneFlags190=0xa4;
            std::memcpy(node.localC0.v,source.local,sizeof source.local);
            node.parent180=static_cast<I>(parent);
            auto& first=nodes[parent].firstChild184;
            if (first==I::none) first=static_cast<I>(index);
            else {
                auto tail=static_cast<std::uint32_t>(first);
                while (nodes[tail].nextSibling188!=I::none)
                    tail=static_cast<std::uint32_t>(nodes[tail].nextSibling188);
                nodes[tail].nextSibling188=static_cast<I>(index);
            }
            sources[n]=static_cast<I>(index);palette[n]=static_cast<std::uint8_t>(n);
            std::memcpy(names[n].data(),source.name,sizeof source.name);
        }
        return validateSceneGraph49P(view());
    }
    static bool equal(const char* a,const char* b) noexcept {
        for (;*a&&*b;++a,++b) {
            const auto fold=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
            if (fold(*a)!=fold(*b)) return false;
        }
        return *a==*b;
    }
    I findFrom(I start,const char* name) const noexcept {
        if (start==I::none) return I::none;
        const auto index=static_cast<std::uint32_t>(start);
        if (index&&equal(names[index-1].data(),name)) return start;
        //28C590: this node, next sibling subtree, then first child subtree.
        const auto sibling=findFrom(nodes[index].nextSibling188,name);
        return sibling!=I::none?sibling:findFrom(nodes[index].firstChild184,name);
    }
    I find(const char* name) const noexcept {return findFrom(static_cast<I>(0),name);}
    bool project(std::uint8_t selected,ModelPalette49S& output,D& d) noexcept {
        ModelPaletteInput49S input{};input.phase=ModelPalettePhase49S::selectedRenderPreparation;
        input.suppliedState=ModelPaletteSceneState49S::completeCurrentScene;
        input.modelFlags191=selected;input.modelRoot=static_cast<I>(0);
        input.sourceNodes=sources.data();input.sourceNodeCount=count;
        input.paletteIds=palette.data();input.paletteCount=count;
        ModelPaletteWorkspace49S scratch{};
        return projectModelPalette49S(view(),input,output,scratch,d.palette)||fail(d,E::palette,0x2412E4);
    }
};
// Exact263A00/263E20 native grid key. 2AEF70 derives from the same sparse
// quadtree base; the Controls direct-render subset needs the registered key and
// observer zone, not a replacement scene search or guessed visibility result.
struct Grid {
 // Bounded real native registration for this directly traversed widget only.
 // No scene enumerator reads this subset as the complete background scene.
 struct Member {const SceneNode49P* root=nullptr;std::uint32_t key=0;} member;
 void publish(const SceneNode49P* root,std::uint32_t key)noexcept{member=key?Member{root,key}:Member{};}
 bool contains(const SceneNode49P* root,std::uint32_t key)const noexcept{return key?(member.root==root&&member.key==key):member.root==nullptr;}
 float xmin=0,xmax=0,zmin=0,zmax=0,scale=0;
 std::array<std::int32_t,12> levels{};std::int32_t depth=0;
 bool initialize(const assets::OriginalUiDat50KG& scn,D& d){
  const auto* info=scn.section("SceneInfo");const auto* env=scn.section("Environment");
  const auto* fallback=scn.section("StaticModels");
  if(!info||!env||!fallback||scn.section("Zone1"))return fail(d,E::authoring,0x275E30);
  std::int32_t zones=1,ports=0;
  if(!assets::originalUiInteger50KG(*info,"NumZones",1,zones,d.text,0x270A00)||zones!=1||
     !assets::originalUiInteger50KG(*info,"NumPortals",0,ports,d.text,0x2709D4)||ports!=0)return fail(d,E::authoring,0x2729E0);
  // Authenticated fallback contains no search override. All four source method
  // defaults are2;272C90 therefore installs one shared grid in34 and38.
  for(const auto& f:fallback->values)if(f.key.find("Search")!=std::string::npos)return fail(d,E::authoring,0x275E30);
  float width=0,breadth=0,ws=0,bs=0,border=0;
  if(!assets::originalUiNumber50KG(*env,"TerrainWidth",0,width,d.text,0x276F4C)||
     !assets::originalUiNumber50KG(*env,"TerrainBreadth",0,breadth,d.text,0x276F64)||
     !assets::originalUiNumber50KG(*env,"TerrainWidthScale",0,ws,d.text,0x276F7C)||
     !assets::originalUiNumber50KG(*env,"TerrainBreadthScale",0,bs,d.text,0x276F94)||
     !assets::originalUiNumber50KG(*env,"TerrainBorderWidth",0,border,d.text,0x276FAC)||
     !assets::originalUiNumber50KG(*env,"Xmin",-border,xmin,d.text,0x276FC4)||
     !assets::originalUiNumber50KG(*env,"Xmax",add(border,mul(256,mul(width,ws))),xmax,d.text,0x277020)||
     !assets::originalUiNumber50KG(*env,"Zmin",-border,zmin,d.text,0x277034)||
     !assets::originalUiNumber50KG(*env,"Zmax",add(border,mul(256,mul(breadth,bs))),zmax,d.text,0x277090))return fail(d,E::authoring,0x276F3C);
  if(!(xmax>xmin&&zmax>zmin))return fail(d,E::authoring,0x263A00);
  scale=div(32768,std::max(sub(xmax,xmin),sub(zmax,zmin)));
  const float threshold=mul(96,scale);if(!std::isfinite(threshold)||threshold<0||threshold>=2147483648.f)return fail(d,E::arithmetic,0x263AC0);
  std::int32_t half=32768,count=0;do{half>>=1;levels[count++]=half;}while(count<12&&static_cast<std::int32_t>(threshold)<half);
  depth=count-1;return true;
 }
 bool key(const std::array<float,4>& center,const std::array<float,4>& extent,std::uint32_t& out)const noexcept{
  float lx=std::max(sub(center[0],extent[0]),xmin),hx=std::min(add(center[0],extent[0]),xmax);
  float lz=std::max(sub(center[2],extent[2]),zmin),hz=std::min(add(center[2],extent[2]),zmax);
  lx=sub(lx,xmin);hx=sub(hx,xmin);lz=sub(lz,zmin);hz=sub(hz,zmin);
  if(hx<0||lx>sub(xmax,xmin)||hz<0||lz>sub(zmax,zmin)){out=0;return true;}
  // Source EE CVT.W.S truncation, matching the existing native UI conversion.
  const auto rounded=[&](float x,std::int32_t& y){x=mul(x,scale);if(!std::isfinite(x)||x<0||x>32768)return false;y=static_cast<std::int32_t>(x);return true;};
  std::int32_t x=0,z=0,xx=0,zz=0;if(!rounded(lx,x)||!rounded(lz,z)||!rounded(hx,xx)||!rounded(hz,zz))return false;
  ++xx;++zz;const auto dx=xx-x,dz=zz-z;std::int32_t level=0;
  while(level<=depth&&dx<levels[level]&&dz<levels[level])++level;if(level>depth)level=depth;
  auto step=levels[level];auto mask=~(step-1);x&=mask;xx&=mask;z&=mask;zz&=mask;
  if((xx-x>step||zz-z>step)&&level){step+=step;mask=~(step-1);--level;x&=mask;xx&=mask;z&=mask;zz&=mask;}
  std::uint32_t flags=0x10;if(zz!=z)flags|=0x20;if(xx!=x)flags|=0x40;if((flags&0x60)==0x60)flags|=0x80;
  if(x>=32768)x=32767;if(z>=32768)z=32767;
  out=(static_cast<std::uint32_t>(x)<<17)|(static_cast<std::uint32_t>(z)<<5)|flags|static_cast<std::uint32_t>(level);return true;
 }
};
}
struct OriginalUiControlsModel50KG::Storage {
 std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
 std::shared_ptr<const assets::OriginalUiControlsModelAssets50KG> assets;
 const OriginalUiMainSceneOwner50KG* background=nullptr;
 const OriginalUiLightManagerLifetime50KG* lightOwner=nullptr;
 std::uint64_t lightGeneration=0,generation=0,serial=0;
 ModelGraph model;assets::MotionNodeBindings49O bindings{};
 std::array<ModelPlaybackMotion50KG,2> motions{};ModelPlayback50KG playback{};
 ModelPlaybackWorkspace50KG workspace{};
 OriginalUiControlsLighting50KG lighting{};
 Grid grid;std::uint32_t key14=0;
 ModelBoundsResult49O bounds{};
 OriginalUiControlsModelSelection50KG selection{};
 std::uint8_t flags191=0,flags1B2=0;
 float fogStart=0,fogEnd=0,fogSlope=0;std::uint32_t fogColor=0;
 bool shown=true,selected=false,registered=false;
 bool observer(D& d)noexcept{
  if(!refreshSceneWorld49P(model.view(),static_cast<I>(0))||
     !prepareOriginalUiRiderModelBounds50KG(*assets->hierarchy(),model.view(),bounds))return fail(d,E::graph,0x28E230);
  const auto& world=model.nodes[0].world100;
  std::array<float,4> center{},extent{};
  for(unsigned a=0;a<3;++a){
   float c=mul(world.v[a],bounds.center160[0]);c=add(c,mul(world.v[4+a],bounds.center160[1]));c=add(c,mul(world.v[8+a],bounds.center160[2]));center[a]=add(c,world.v[12+a]);
   //28E4DC: Z contribution + (X contribution + Y contribution), not a
   // independently unioned world box or reordered sum.
   extent[a]=add(std::fabs(mul(world.v[8+a],bounds.halfExtent170[2])),add(std::fabs(mul(world.v[a],bounds.halfExtent170[0])),std::fabs(mul(world.v[4+a],bounds.halfExtent170[1]))));
   if(!std::isfinite(center[a])||!std::isfinite(extent[a]))return fail(d,E::arithmetic,0x28E230);
  }
  center[3]=1;
  std::uint32_t key=0;if(!grid.key(center,extent,key))return fail(d,E::membership,0x263E20);
  //2809C0 register /281380 retain, remove or transfer the sole local model
  // record. Key zero means no grid bucket, while observer+4 retains its zone.
  key14=key;if(registered)grid.publish(&model.nodes[0],key);selection={};return true;
 }
 bool sample(float dt,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept{
  ModelPlaybackIk50KG noIk{};
  if(!advanceModelPlayback50KG(dt,bindings,model.view(),noIk,playback,shared,ik,workspace,d.playback))return fail(d,E::playback,0x154480);
  return true;
 }
 bool restore(D& d)noexcept{
  if(playback.savedMatrixCount1F4!=bindings.count)return fail(d,E::playback,0x1556C0);
  for(std::uint32_t i=0;i<bindings.count;++i)
   if(!setLocalMatrix49P(model.view(),static_cast<I>(bindings.rows[i].sceneNode),playback.savedMatrices1F4[i]))return fail(d,E::graph,0x1556C0);
  //1556C0 restores matrices only. It does not stop a still-running motion or
  // replace current18/time34. Entry must preserve those original semantics.
  selection={};return true;
 }
};
OriginalUiControlsModel50KG::OriginalUiControlsModel50KG()noexcept=default;
OriginalUiControlsModel50KG::~OriginalUiControlsModel50KG()=default;
bool OriginalUiControlsModel50KG::prepare(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
 const OriginalUiMainSceneOwner50KG& background,D& d)noexcept{
 d={};if(storage_)return fail(d,E::alreadyPrepared,0x2A8770);
 if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options)||
    !background.prepared()||background.archiveIdentity()!=archive->identity()||!background.sceneAuthoring()||std::fegetround()!=FE_TONEAREST)return fail(d,E::asset,0x1D06A8);
 try{
  auto n=std::make_unique<Storage>();n->archive=std::move(archive);n->background=&background;
  auto asset=std::make_shared<assets::OriginalUiControlsModelAssets50KG>();
  if(!asset->load(n->archive,36,d.asset))return fail(d,E::asset,d.asset.sourcePC);
  n->assets=std::move(asset);
  if(!n->grid.initialize(*background.sceneAuthoring(),d))return false;
  if(!n->model.build(*n->assets->hierarchy(),*n->assets->position(),*n->assets->look(),*n->assets->up(),true))return fail(d,E::graph,0x2A8770);
  n->bindings=*n->assets->bindings();for(std::uint32_t i=0;i<n->bindings.count;++i){auto& row=n->bindings.rows[i].sceneNode;if(row>=n->model.count)return fail(d,E::binding,0x153218);++row;}
  for(std::size_t i=0;i<2;++i){const auto* m=n->assets->motion(i);if(!m)return fail(d,E::asset,0x1523F0);n->motions[i]={&m->motion,m->looping};}
  //15376C constructor stopped10=1, current18=null. No authored InitialMotion.
  n->playback.stopped10=true;
  if(!captureModelPlaybackMatrices50KG(n->bindings,n->model.view(),n->playback,d.playback))return fail(d,E::playback,0x153F1C);
  assets::OriginalUiDat50KG ui;
  if(!assets::readOriginalUiDat50KG(*n->archive,"UI/UIOption/ui.dat",ui,d.text,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,E::authoring,0x1CDB10);
  // Base UI first registers ui.dat lights, but272A48 resets those zone
  // lists.273AA0/277FCC replaces them with actual background SCN lights.
  //1D06E0 is2A4B30 resource finalization, NOT another light population.
  const auto& scn=*background.sceneAuthoring();
  const auto* lights=scn.section("Lights");const auto* a=scn.section("Light1");const auto* b=scn.section("Light2");const auto* fog=ui.section("Fog");std::int32_t count=0;
  if(!lights||!a||!b||!fog||!assets::originalUiInteger50KG(*lights,"NumberOfLights",2,count,d.text,0x273AA0)||count!=2||
     !a->find("Type")||a->find("Type")->text!="Ambient"||!b->find("Type")||b->find("Type")->text!="Directional"||b->find("LookVector"))return fail(d,E::authoring,0x277390);
  std::array<float,3> ambient{},directional{},position{},target{},rgb{};
  if(!assets::originalUiTriple50KG(*a,"ColorRGB",{},ambient,d.text,0x2774A8)||
     !assets::originalUiTriple50KG(*b,"ColorRGB",{},directional,d.text,0x2774A8)||
     !assets::originalUiTriple50KG(*b,"Position",{},position,d.text,0x277578)||
     !assets::originalUiTriple50KG(*b,"TargetPosition",{},target,d.text,0x2775CC))return fail(d,E::authoring,0x2774A8);
  n->lighting.uiIdentity=ui.identity();n->lighting.sceneIdentity=scn.identity();
  for(unsigned i=0;i<3;++i){if(ambient[i]<0||ambient[i]>255||directional[i]<0||directional[i]>255)return fail(d,E::authoring,0x146FDC);
   n->lighting.ambient50[i]=mul(ambient[i],.5f);n->lighting.directional3C[i]=mul(directional[i],.5f);n->lighting.directionalLook78[i]=sub(target[i],position[i]);}
  n->lighting.ambient50[3]=64;n->lighting.ambientSlot60=n->lighting.ambient50;n->lighting.directional3C[3]=127.5f;
  //2775DC subtracts target-position without normalization;147200 normalizes once.
  if(!normalize(n->lighting.directionalLook78))return fail(d,E::arithmetic,0x147200);
  float visibility=1,haziness=.5f;
  if(!assets::originalUiNumber50KG(*fog,"Visibility",1,visibility,d.text,0x1CE81C)||
     !assets::originalUiNumber50KG(*fog,"Haziness",.5f,haziness,d.text,0x1CE8BC)||
     !assets::originalUiTriple50KG(*fog,"ColorRGB",{},rgb,d.text,0x1CE964))return fail(d,E::authoring,0x1CE7B0);
  if(visibility<0||visibility>1)visibility=1; // source rechecks Visibility, not Haziness
  n->fogEnd=add(900,mul(visibility,sub(1200,900)));n->fogStart=mul(sub(1,haziness),sub(n->fogEnd,200));
  const float range=sub(n->fogStart,n->fogEnd);n->fogSlope=range==0?0:div(255,range);
  if(!std::isfinite(n->fogEnd)||n->fogEnd<=1||!std::isfinite(n->fogStart)||!std::isfinite(n->fogSlope))return fail(d,E::arithmetic,0x13E830);
  for(unsigned i=0;i<3;++i){if(!std::isfinite(rgb[i])||rgb[i]<0||rgb[i]>255)return fail(d,E::authoring,0x1CE964);n->fogColor|=static_cast<std::uint32_t>(rgb[i])<<(i*8);}
  storage_=std::move(n);return true;
 }catch(...){return fail(d,E::allocation,0x2A8770);}
}
bool OriginalUiControlsModel50KG::publishRegistry(OriginalUiLightManagerLifetime50KG& owner,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::asset,0x277FCC);auto& s=*storage_;
 OriginalUiLightManagerLifetimeDiagnostic50KG light;
 if(!owner.publishControlsRegistry(s.lighting,light))return fail(d,E::lighting,light.sourcePC);
 s.lightOwner=&owner;s.lightGeneration=owner.managerGeneration();return true;
}
bool OriginalUiControlsModel50KG::apply(const OriginalUiControlsEffect50KG& effect,
 ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::asset,0x2A8770);auto& s=*storage_;
 using Kind=OriginalUiControlsEffectKind50KG;using A=OriginalUiControlsModelAction50KG;using O=OriginalUiControlOperation50KG;
 if(effect.widgetId!=36)return fail(d,E::operation,effect.sourcePC);
 s.selection={};
 if(effect.kind==Kind::model){
  switch(effect.modelAction){
  case A::bindEntryModel:
   if(s.generation==std::numeric_limits<std::uint64_t>::max())return fail(d,E::membership,0x28E8A0);
   if(!s.observer(d)||!s.key14)return fail(d,E::membership,0x280B40);
   ++s.generation;s.grid.publish(&s.model.nodes[0],s.key14);s.registered=true;return true;
  case A::restoreEntryTransforms:return s.restore(d);
  case A::selectBackMotion:return startModelPlayback50KG(s.motions[0],s.playback,d.playback)||fail(d,E::playback,0x155800);
  case A::selectFrontMotion:return startModelPlayback50KG(s.motions[1],s.playback,d.playback)||fail(d,E::playback,0x155800);
  case A::advanceRotation001:return s.sample(.001f,shared,ik,d);
  }
 }
 if(effect.kind!=Kind::widget)return fail(d,E::operation,effect.sourcePC);
 switch(effect.operation){
 case O::show:s.shown=true;return true;
 case O::hide:s.shown=false;return true;
 case O::setEnabled:s.selected=effect.value;s.shown=effect.value;return true;
 case O::reset:{BodyMatrix48G pose{};
  if(!originalUiUserRotatePose50KG(*s.assets->position(),*s.assets->look(),*s.assets->up(),pose)||!setLocalMatrix49P(s.model.view(),static_cast<I>(0),pose))return fail(d,E::graph,0x2A8C90);
  s.shown=true;return true;}
 case O::select:case O::deselect:case O::clearSelection:
  // SelectionMotion1 and DeSelectionMotion1 restart controller. All four
  // authored visibility-on-selection flags are absent/default false.
  if(!startModelPlayback50KG(s.motions[0],s.playback,d.playback)||!s.sample(.001f,shared,ik,d))return fail(d,E::playback,0x2A8D30);
  s.selected=effect.operation==O::select;return true;
 default:return fail(d,E::operation,effect.sourcePC);
 }
}
bool OriginalUiControlsModel50KG::advance(float dt,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept{
 d={};if(!storage_||!std::isfinite(dt)||dt<0)return fail(d,E::asset,0x2A8BB0);
 //2A8BB0 invokes154480 even when stopped or without current18, retaining the
 // actual shared update lazy record. Entity scheduling determines invocation.
 if(!storage_->sample(dt,shared,ik,d))return false;
 return !storage_->registered||storage_->observer(d);
}
bool OriginalUiControlsModel50KG::observation(bool& stopped)const noexcept{if(!storage_)return false;stopped=storage_->playback.stopped10;return true;}
bool OriginalUiControlsModel50KG::prepared()const noexcept{return !!storage_;}
const assets::Scene48D* OriginalUiControlsModel50KG::scene()const noexcept{return storage_?storage_->assets->scene():nullptr;}
const assets::OriginalUiMemberIdentity50KG* OriginalUiControlsModel50KG::widgetIdentity()const noexcept{return storage_?storage_->assets->widgetSource():nullptr;}
bool OriginalUiControlsModel50KG::isCurrentSelection(const OriginalUiControlsModelSelection50KG& v)const noexcept{
 if(!storage_||!storage_->registered||v.owner!=this||v.sceneOwner!=storage_->background||!v.generation||v.generation!=storage_->generation)return false;
 if(!storage_->grid.contains(&storage_->model.nodes[0],storage_->key14)||v.sourceModelFlags191!=storage_->flags191||v.sourceRenderFlags1B2!=storage_->flags1B2)return false;
 const auto& x=storage_->selection;return v.serial&&v.serial==x.serial&&v.gridKey14==x.gridKey14&&v.sourceZoneIndex48==0&&v.sourceModelFlags191==x.sourceModelFlags191&&v.sourceRenderFlags1B2==x.sourceRenderFlags1B2&&v.disabled==x.disabled;
}
bool OriginalUiControlsModel50KG::preparePacket(const OriginalUiOptionsCamera50KG& camera,
 OriginalUiLightManagerLifetime50KG& lights,OriginalUiControlsModelPacket50KG& out,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::asset,0x241280);auto& s=*storage_;
 if(!s.registered||!s.background||!s.background->prepared()||s.background->archiveIdentity()!=s.archive->identity()||
    s.lightOwner!=&lights||s.lightGeneration!=lights.managerGeneration()||lights.activeUiIdentity()!=s.archive->identity()||camera.uiIdentity!=s.lighting.uiIdentity||
    s.serial==std::numeric_limits<std::uint64_t>::max()||std::fegetround()!=FE_TONEAREST)return fail(d,E::membership,0x240C00);
 // Source13EA30 active-camera continuation. Only the original near/far fields
 // change; viewC0 and borrowed manager camera identity remain actual Options.
 // Rider-named numeric structures below are shared numeric leaves. Their
 // identity is Options ui.dat and is never presented to a Rider owner.
 OriginalUiRiderCamera50KG numeric;
 numeric.pageIdentity=camera.uiIdentity;numeric.position=camera.position;
 numeric.look=camera.look;numeric.up=camera.up;numeric.fovDegrees=camera.fovDegrees;
 numeric.rollRadians=camera.rollRadians;numeric.aspect1DC=camera.aspect1DC;numeric.aspect1F8=camera.aspect1F8;numeric.core=camera.core;
 auto& core=numeric.core;
 if(!finite(core.view)||!finite(core.projection))return fail(d,E::camera,0x13EA30);
 core.sceneAfter.nearPlane=1;core.sceneAfter.farPlane=std::min(s.fogEnd,100000.f);
 const float z=div(1,sub(1,div(core.sceneAfter.nearPlane,core.sceneAfter.farPlane)));
 core.projection[10]=z;core.projection[14]=mul(-z,core.sceneAfter.nearPlane);core.combined=product(core.view,core.projection);
 OriginalUiRiderMembershipCamera50KG viewport;OriginalUiRiderClipCamera50KG guard;
 if(!finite(core.combined)||prepareOriginalUiParticleCameraFromView50KG(core,640,448,numeric.sourceScreen)!=OriginalUiMainParticleCameraStatus50KG::ok||
    !prepareOriginalUiRiderMembershipCamera50KG(numeric,viewport)||!prepareOriginalUiRiderClipCamera50KG(numeric,guard))return fail(d,E::camera,0x27F650);
 OriginalUiControlsModelPacket50KG p{};p.scene=s.assets->scene();p.packedFogColorGs=s.fogColor;
 p.context.viewC0=core.view;p.context.projection80=core.projection;p.context.clipProjection290=guard.clipProjection290;
 p.context.screen140=numeric.sourceScreen.sourceScreen140;p.context.cameraPosition180=camera.position;
 p.context.fogEnd33Z=s.fogEnd;p.context.fogSlope33W=s.fogSlope;
 auto selection=OriginalUiControlsModelSelection50KG{this,s.background,s.generation,s.serial+1,0,s.key14,s.flags191,s.flags1B2,!s.shown};
 if(!s.shown){selection.sourceModelFlags191=static_cast<std::uint8_t>(s.flags191&~1u);}
 else{
  // Source observer28EA20 is the update boundary; this selected render refresh
  // only obtains current world and bounds, never invents a registration.
  if(!refreshSceneWorld49P(s.model.view(),static_cast<I>(0))||!prepareOriginalUiRiderModelBounds50KG(*s.assets->hierarchy(),s.model.view(),s.bounds))return fail(d,E::graph,0x240C00);
  p.sourceModelWorld=s.model.nodes[0].world100;Matrix world{};std::memcpy(world.data(),p.sourceModelWorld.v,sizeof p.sourceModelWorld.v);
  std::array<float,4> center{},extent{};std::memcpy(center.data(),s.bounds.center160,sizeof s.bounds.center160);std::memcpy(extent.data(),s.bounds.halfExtent170,sizeof s.bounds.halfExtent170);
  OriginalUiRiderBoxClip50KG clip{},guarded{};
  if(!projectOriginalUiRiderBoxClip50KG(product(world,viewport.matrix60),center,extent,clip))return fail(d,E::arithmetic,0x2B0580);
  auto flags=static_cast<std::uint8_t>((s.flags1B2&~(8u|128u))|4u);bool selected=!clip.outside;auto remaining=clip.unionMask;
  if(selected){if(!remaining)flags=static_cast<std::uint8_t>(flags|8u);else{
   if(!projectOriginalUiRiderBoxClip50KG(product(world,guard.combinedClip2D0),center,extent,guarded))return fail(d,E::arithmetic,0x240FE4);
   remaining=guarded.unionMask;if(!remaining)flags=static_cast<std::uint8_t>(flags|8u); // original zero4354F0
  }}
  remaining=static_cast<std::uint8_t>(remaining&~0x10u);
  if(selected)flags=static_cast<std::uint8_t>((flags&~5u)|(remaining?5u:0u));
  selection.sourceRenderFlags1B2=flags;selection.sourceModelFlags191=static_cast<std::uint8_t>((s.flags191&~1u)|(selected?1u:0u));
 }
 // Selection belongs to this actual observer, frame and scene. Only then can
 //14A0E8 consult the shared application lifetime and model's zone index.
 s.flags191=selection.sourceModelFlags191;s.flags1B2=selection.sourceRenderFlags1B2;
 s.serial=selection.serial;s.selection=selection;p.selection=selection;
 if(selection.selected()){
  OriginalUiLightManagerLifetimeDiagnostic50KG ld;
  if(!lights.selectControlsModel(selection,p.context.lights,ld))return fail(d,E::lighting,ld.sourcePC);
  OriginalUiRiderModelFog50KG fog;
  if(!prepareOriginalUiRiderModelFog50KG(*s.assets->hierarchy(),s.model.view(),camera.position,s.fogStart,fog))return fail(d,E::graph,0x26F070);
  p.computeRadialFog=fog.computeRadialFog1B2;
  if(!s.model.project(selection.sourceModelFlags191,p.palette,d))return false;
  s.flags1B2=static_cast<std::uint8_t>((s.flags1B2&~2u)|(p.computeRadialFog?2u:0u));
  s.selection.sourceRenderFlags1B2=s.flags1B2;p.selection=s.selection;
  p.emitsGeometry=true;
 }
 out=p;return true;
}
}
