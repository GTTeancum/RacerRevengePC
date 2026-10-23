// Cold source/data/ownership checks only; never starts a page or renderer.
#include "original_ui_garage_scene_assets50kg.h"
#include "original_ui_garage_model_construction50kg.h"
#include <bit>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>

namespace {
using namespace rr::assets;
using Id=OriginalUiArchiveId50KG;
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
std::shared_ptr<OriginalUiArchive50KG> load(const std::string& root,Id id){
    const auto* identity=originalUiArchiveIdentity50KG(id);check(identity,"missing catalog identity");
    std::ifstream f(root+"/"+identity->discPath,std::ios::binary|std::ios::ate);check(bool(f),"original archive not available");
    check(f.tellg()==static_cast<std::streamoff>(identity->bytes),"archive length mismatch");
    std::vector<std::uint8_t> bytes(identity->bytes);f.seekg(0);f.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    check(bool(f),"archive read failed");auto a=std::make_shared<OriginalUiArchive50KG>();OriginalUiArchiveDiagnostic50KG d;
    check(a->loadOwned(id,std::move(bytes),d),"archive authentication failed");return a;
}
void run(const std::string& root){
    auto garage=load(root,Id::garage),pod=load(root,Id::garagePod19);
    std::weak_ptr<OriginalUiArchive50KG> garageWeak=garage,podWeak=pod;
    auto a=std::make_shared<OriginalUiGarageSceneAssets50KG>();OriginalUiGarageSceneAssetDiagnostic50KG d;
    check(!a->loaded()&&!a->actor(0)&&!a->staticModel(0)&&!a->selectedPod()&&!a->toyDat(),"empty collection exposes data");
    check(!a->load({},pod,d)&&d.error==OriginalUiGarageSceneAssetError50KG::archive&&!a->loaded(),"null archive accepted");
    check(!a->load(pod,garage,d)&&d.error==OriginalUiGarageSceneAssetError50KG::archive&&!a->loaded(),"wrong original archives accepted");
    if(!a->load(garage,pod,d)){
        std::cerr<<"scene load error="<<unsigned(d.error)<<" ordinal="<<d.ordinal<<" pc="<<std::hex<<d.sourcePC<<std::dec
                 <<" resource="<<unsigned(d.resource.status)<<" geometry="<<d.resource.geometry.code<<"\n";
        throw std::runtime_error("authentic scene asset load failed");
    }
    const auto* first=a->staticModel(0);check(first&&first->slt=="batterypack.slt"&&
        first->position==std::array<float,3>{{48.055893f,10.760490f,71.276047f}},"first static source placement changed");
    check(a->staticModel(29)&&a->staticModel(29)->slt=="tubes08.slt"&&!a->staticModel(30),"static population bounds wrong");
    std::set<const OriginalUiMemberIdentity50KG*> identities;
    for(std::size_t i=0;i<30;++i){const auto* s=a->staticModel(i);check(s&&s->ordinal==i+1&&s->model&&s->model->hierarchy.nodeCount==1&&
            s->model->payload->nodeCount()==1&&s->model->scene()->lods.size()==1,"static hierarchy or authored order changed");
        check(s->look==std::array<float,3>{{0,0,1}}&&s->model->source.identity==garage->find(s->model->source.identity->name).identity,
              "static source reference changed");identities.insert(s->model->source.identity);}
    check(identities.size()==30,"distinct authored static members collapsed");
    constexpr const char* names[]{"pd_01","pd_02","pd_03","pd_bx","pd_lt","pd_fire","pd_ext","tank","nozzle","pd_fall"};
    constexpr float durations[][4]{{8},{12},{6},{3,3,3.5f,2},{3,3,3,2},{2.8f},{3},{3},{3},{12.666666984558105f}};
    unsigned motionCount=0;const auto* pd=a->actor(0)->model.get();
    for(std::size_t i=0;i<10;++i){const auto* actor=a->actor(i);check(actor&&actor->ordinal==i+1&&actor->mcfName==std::string(names[i])+".mcf"&&
        actor->motionCount==((i==3||i==4)?4u:1u)&&actor->startingZone==1&&actor->offset==std::array<float,3>{}&&
        actor->useLighting&&actor->useBlendedKeys&&!actor->useCollision,"actor descriptor changed");
        check(actor->model->hierarchy.nodeCount==((i==7||i==8)?2u:18u)&&actor->bindings.count>0,"actor binding shape wrong");
        if(i!=7&&i!=8)check(actor->model.get()==pd,"shared immutable pd resource unexpectedly duplicated");
        check(actor->mcf.identity()==garage->find("UI/Anim/"+actor->mcfName).identity,"MCF provenance missing");
        for(unsigned j=0;j<actor->motionCount;++j){const auto& m=actor->motions[j];++motionCount;
            check(m.source.identity&&!m.looping&&m.motion.duration==durations[i][j]&&m.motion.data==m.source.data,"MOT duration/lifetime changed");
            check(m.motion.duration!=m.authoredFrames/m.authoredFps,"MCF authoring was substituted for MOT duration");}
        identities.insert(actor->model->source.identity);
    }
    check(motionCount==16&&!a->actor(10)&&identities.size()==33,"actor population changed or unrelated PSG counted live");
    const auto* selected=a->selectedPod();check(selected&&selected->model->source.identity==pod->find("UI/Geometry/19e.psg").identity&&
        selected->model->hierarchy.nodeCount==1,"normal selected19E missing");
    const auto* scene=selected->model->scene();check(scene&&scene->materials==std::vector<std::string>{"19e_1"}&&scene->lods.size()==1&&
        scene->lods[0].surfaces.size()==1,"selected geometry shape changed");
    const auto& surface=scene->lods[0].surfaces[0];
    check(surface.vertices.size()==6126&&surface.source_strips==874&&surface.strips.size()==927&&surface.continuations.size()==53,
          "actual19E strip continuation representation changed");
    check(surface.has_original_colors&&surface.component_word==0xff010f&&!scene->identity_matrix_palette,"selected original color/palette lost");
    check(std::bit_cast<std::uint32_t>(selected->position[0])==0x4278ab14u&&std::bit_cast<std::uint32_t>(selected->position[1])==0x4141c3edu&&
          std::bit_cast<std::uint32_t>(selected->position[2])==0x423dd6d4u,"manager selected position words changed");
    check(a->toyDat()->section("Animation4")->find("NumberOfSequence")&&
          !a->toyDat()->section("Animation4")->find("NumberOfSequences"),"authored typo was rewritten");
    check(!a->load(garage,pod,d)&&d.error==OriginalUiGarageSceneAssetError50KG::alreadyLoaded&&a->staticModel(0)==first,
          "reload replaced immutable assets");
    rr::game::OriginalUiGarageModelConstruction50KG graph;rr::game::OriginalUiGarageModelConstructionDiagnostic50KG gd;
    check(!graph.prepareSelected19E({},gd)&&!graph.prepared()&&!graph.root(),"empty graph preparation accepted");
    check(graph.prepareSelected19E(a,gd)&&graph.graphNodeCount()==1&&
          graph.sourceRowIndex(0)==static_cast<rr::game::SceneNodeIndex49P>(0)&&
          graph.sourceRowIndex(1)==rr::game::SceneNodeIndex49P::none,"selected graph has a fabricated extra root");
    const auto* node=graph.root();check(node->parent180==rr::game::SceneNodeIndex49P::none&&
        node->firstChild184==rr::game::SceneNodeIndex49P::none&&node->nextSibling188==rr::game::SceneNodeIndex49P::none&&node->sceneFlags190==0xa4,
        "selected root topology/flags wrong");
    constexpr float expected[]{1,0,0,0,0,1,0,0,0,0,1,0};
    for(unsigned i=0;i<12;++i)check(node->localC0.v[i]==expected[i],"selected source basis wrong");
    for(unsigned i=0;i<3;++i)check(node->localC0.v[12+i]==selected->position[i],"selected root translation not overwritten");
    check(node->localC0.v[15]==1&&node->localC0.v[12]!=graph.originalRoot()->local[12],"PSG bind translation was preserved erroneously");
    for(float f:node->world100.v)check(f==0,"unrefreshed source world was fabricated");
    check(!graph.prepareSelected19E(a,gd)&&gd.error==rr::game::OriginalUiGarageModelConstructionError50KG::alreadyPrepared&&graph.root()==node,
          "graph reprepare changed source ownership");
    auto detachedModel=first->model;a.reset();garage.reset();pod.reset();
    check(!garageWeak.expired()&&!podWeak.expired()&&graph.root()==node&&graph.source()->model->payload->prepared()&&
          detachedModel->source.data&&detachedModel->hierarchy.data,"retained source backing expired");
}
}
int main(int argc,char** argv){try{check(argc==2,"usage: garage-scene-assets-check /path/to/original/disc");run(argv[1]);
    std::cout<<"Authentic Garage cold assets passed: 30 static placements, 10 actors, 16 MOTs, selected19E direct root and retained lifetimes. No live scene/render/route admission.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
