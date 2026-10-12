// Reconstructed 2026-10-12, source-only, no execution validation.
#include "original_ui_course_hologram_scene50kg.h"
#include "original_ui_user_rotate50kg.h"
#include <cstring>
#include <cmath>
namespace rr::game {
namespace {
using I=SceneNodeIndex49P;
using D=OriginalUiCourseHologramSceneDiagnostic50KG;
using E=OriginalUiCourseHologramSceneError50KG;
bool fail(D& d,E e,std::uint32_t pc)noexcept{d.error=e;d.sourcePC=pc;return false;}
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

}
struct OriginalUiCourseHologramScene50KG::Storage {
 std::shared_ptr<const assets::OriginalUiCourseHologramAssets50KG> assets;
 ModelGraph model;assets::MotionNodeBindings49O bindings{};
 ModelPlaybackMotion50KG motion{};ModelPlayback50KG playback{};
 bool shown=false,selected=false;
 bool apply(OriginalUiControlOperation50KG op,bool value,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept {
  using O=OriginalUiControlOperation50KG;
  if(op==O::show){shown=true;return true;}
  if(op==O::hide){shown=false;return true;}
  if(op==O::setEnabled){selected=value;shown=value;return true;}
  if(op==O::reset){
   BodyMatrix48G root;
   if(!originalUiUserRotatePose50KG(*assets->position(),*assets->look(),*assets->up(),root)||!setLocalMatrix49P(model.view(),static_cast<I>(0),root))return fail(d,E::graph,0x2A8C90);
   shown=true;return true;
  }
  if(op!=O::select&&op!=O::deselect&&op!=O::clearSelection)return fail(d,E::operation,0x2A8D30);
  // 2A8D30/2A8DE0/2A8E90 all issue original motion1 restart and .001 sample.
  ModelPlaybackWorkspace50KG workspace{};ModelPlaybackIk50KG noIk{};
  if(!startModelPlayback50KG(motion,playback,d.playback)||
     !advanceModelPlayback50KG(.001f,bindings,model.view(),noIk,playback,shared,ik,workspace,d.playback))return fail(d,E::playback,0x2A8D30);
  selected=op==O::select;
  if(op==O::select)shown=true;
  else if(op==O::deselect)shown=false;
  return true;
 }
};
OriginalUiCourseHologramScene50KG::OriginalUiCourseHologramScene50KG()noexcept=default;
OriginalUiCourseHologramScene50KG::~OriginalUiCourseHologramScene50KG()=default;
bool OriginalUiCourseHologramScene50KG::load(std::shared_ptr<const assets::OriginalUiCourseHologramAssets50KG> a,D& d)noexcept{
 d={};if(storage_)return fail(d,E::alreadyLoaded,0x2A8770);
 if(!a||!a->loaded()||!a->hierarchy()||!a->bindings()||!a->motion(0))return fail(d,E::asset,0x2A8770);
 try{
  auto n=std::make_unique<Storage>();n->assets=std::move(a);
  if(!n->model.build(*n->assets->hierarchy(),*n->assets->position(),*n->assets->look(),*n->assets->up(),true)||
     n->model.find("l_IK")!=I::none||n->model.find("r_IK")!=I::none)return fail(d,E::graph,0x2A8770);
  n->bindings=*n->assets->bindings();
  for(std::uint32_t i=0;i<n->bindings.count;++i){auto& index=n->bindings.rows[i].sceneNode;if(index>=n->model.count)return fail(d,E::binding,0x153218);++index;}
  const auto* m=n->assets->motion(0);n->motion={&m->motion,m->looping};
  storage_=std::move(n);return true;
 }catch(...){return fail(d,E::allocation,0x2A8770);}
}
bool OriginalUiCourseHologramScene50KG::preflightApply(OriginalUiControlOperation50KG op,bool value,const ModelMotionSharedState49T& shared,const ModelMode0IkShared50KG& ik,D& d)const noexcept{
 d={};if(!storage_)return fail(d,E::asset,0x2A8770);
 try{auto n=std::make_unique<Storage>(*storage_);
  if(n->playback.current18)n->playback.current18=&n->motion;
  if(n->playback.previous1C)n->playback.previous1C=&n->motion;
  auto ms=shared;auto is=ik;return n->apply(op,value,ms,is,d);
 }catch(...){return fail(d,E::allocation,0x2A8770);}
}
bool OriginalUiCourseHologramScene50KG::apply(OriginalUiControlOperation50KG op,bool value,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::asset,0x2A8770);
 try{auto n=std::make_unique<Storage>(*storage_);
  if(n->playback.current18)n->playback.current18=&n->motion;
  if(n->playback.previous1C)n->playback.previous1C=&n->motion;
  auto ms=shared;auto is=ik;if(!n->apply(op,value,ms,is,d))return false;
  storage_=std::move(n);shared=ms;ik=is;return true;
 }catch(...){return fail(d,E::allocation,0x2A8770);}
}
bool OriginalUiCourseHologramScene50KG::advance(float dt,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ik,D& d)noexcept{
 d={};if(!storage_||!std::isfinite(dt)||dt<0)return fail(d,E::asset,0x2A8770);
 auto& s=*storage_;if(!s.shown||!s.playback.current18)return true;
 ModelPlaybackWorkspace50KG w{};ModelPlaybackIk50KG noIk{};
 return advanceModelPlayback50KG(dt,s.bindings,s.model.view(),noIk,s.playback,shared,ik,w,d.playback)||fail(d,E::playback,0x2A8770);
}
bool OriginalUiCourseHologramScene50KG::projectPalette(std::uint8_t flags,ModelPalette49S& out,D& d)noexcept{d={};return storage_?storage_->model.project(flags,out,d):fail(d,E::asset,0x2412E4);}
bool OriginalUiCourseHologramScene50KG::loaded()const noexcept{return !!storage_;}
bool OriginalUiCourseHologramScene50KG::shown()const noexcept{return storage_&&storage_->shown;}
bool OriginalUiCourseHologramScene50KG::selected()const noexcept{return storage_&&storage_->selected;}
const assets::Scene48D* OriginalUiCourseHologramScene50KG::scene()const noexcept{return storage_?storage_->assets->scene():nullptr;}
SceneGraph49P OriginalUiCourseHologramScene50KG::graph()noexcept{return storage_?storage_->model.view():SceneGraph49P{};}
}
