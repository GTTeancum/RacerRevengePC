// Authentic cold resource/numeric tests only. No live page or render admission.
#include "original_ui_garage_particles50kg.h"
#include <bit>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace rr::assets;
using namespace rr::game;
namespace {
using Id=OriginalUiArchiveId50KG;
using Emit=OriginalUiGarageEmitter50KG;
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
std::shared_ptr<OriginalUiArchive50KG> load(const std::string& root,Id id){
    const auto* identity=originalUiArchiveIdentity50KG(id);check(identity,"missing catalog identity");
    std::ifstream f(root+"/"+identity->discPath,std::ios::binary|std::ios::ate);check(bool(f),"original archive unavailable");
    check(f.tellg()==static_cast<std::streamoff>(identity->bytes),"archive length mismatch");
    std::vector<std::uint8_t> b(identity->bytes);f.seekg(0);f.read(reinterpret_cast<char*>(b.data()),b.size());
    check(bool(f),"archive read failed");auto a=std::make_shared<OriginalUiArchive50KG>();OriginalUiArchiveDiagnostic50KG d;
    check(a->loadOwned(id,std::move(b),d),"archive authentication failed");return a;
}
struct Graph {
    SceneNode49P nodes[64]{};SceneGraph49P graph{nodes,0};std::uint32_t plane=0;
    explicit Graph(const OriginalUiGarageActorAsset50KG& a){
        graph.count=a.model->hierarchy.nodeCount;
        for(unsigned i=0;i<graph.count;++i){PsgNode49O source{};check(readPsgNode49O(a.model->hierarchy,i,source),"PSG source node");
            auto& n=nodes[i];std::memcpy(n.localC0.v,source.local,64);n.parent180=static_cast<SceneNodeIndex49P>(source.parent);
            n.sceneFlags190=0xa4;if(std::string(source.name)=="Plane01")plane=i;
            if(source.parent>=0){auto& p=nodes[source.parent];
                if(p.firstChild184==SceneNodeIndex49P::none)p.firstChild184=static_cast<SceneNodeIndex49P>(i);
                else{auto child=static_cast<unsigned>(p.firstChild184);while(nodes[child].nextSibling188!=SceneNodeIndex49P::none) {
                    child=static_cast<unsigned>(nodes[child].nextSibling188); }
                    nodes[child].nextSibling188=static_cast<SceneNodeIndex49P>(i);}}
        }
        check(validateSceneGraph49P(graph),"authentic PSG graph invalid");
    }
};
void activate(OriginalUiGarageParticles50KG& bank,Emit id,std::uint32_t& raw){
    check(bank.reset88(id,raw).okay()&&bank.enable20(id)&&bank.start80(id),"source reset-enable-start failed");
}
void run(const std::string& root){
    auto scene=std::make_shared<OriginalUiGarageSceneAssets50KG>();OriginalUiGarageSceneAssetDiagnostic50KG sd;
    check(scene->load(load(root,Id::garage),load(root,Id::garagePod19),sd),"scene assets failed");
    auto assets=std::make_shared<OriginalUiGarageParticleAssets50KG>();OriginalUiPageDiagnostic50KG d;
    const auto status=assets->load(scene,d);
    if(status!=OriginalGcSprayStatus50KG::complete){std::cerr<<"parse "<<unsigned(status)<<" pc "<<std::hex<<d.sourcePC<<std::dec<<" text "<<unsigned(d.error)<<" offset "<<d.byteOffset<<'\n';}
    check(status==OriginalGcSprayStatus50KG::complete,"authentic particle resources failed");
    check(assets->atlas().size==7639,"wrong particle atlas");
    constexpr int groups[]{8,4,0,5,8,0,8};constexpr float life[]{4325,235,4500,1000,5500,50,1925};
    constexpr float rate[]{250,180,.1f,1,400,1000,200};
    for(unsigned i=0;i<7;++i){const auto* f=assets->emitter(static_cast<Emit>(i));
        check(f&&f->parsed&&f->source.identity&&f->particleGroup1B8==groups[i]&&f->particleLife17C==life[i]&&f->emitRate188==rate[i],"seven exact definitions changed");}
    check(assets->emitter(Emit::garageSparks)->timing.maxTimeOff1F4==0&&
        assets->emitter(Emit::garageSparks)->timing.minTimeOff1F0==3.3f,"original duplicate off-min defect lost");
    check(assets->emitter(Emit::pdSmoke)->minimumLife214==8300&&assets->emitter(Emit::pdExt)->maximumEmitRate230==1700,
        "colon annotated smoke fields lost");
    check(assets->emitter(Emit::pdSmoke)->framesPerSet==0&&
        !(assets->emitter(Emit::pdSmoke)->particleFlags198&0x400),"inactive zero-frames divisor rejected");
    Graph pd(*scene->actor(5)),nozzle(*scene->actor(8));bool renderPresent=true;std::uint16_t stamp=7,current=7;
    const OriginalUiGarageEmitterRenderBinding50KG render{&renderPresent,&stamp,&current};
    for(unsigned tested=0;tested<7;++tested){
        OriginalUiGarageParticles50KG bank;std::uint32_t raw=0x12345678,expectedRaw=raw,serial=11;bool fireLazy=true;
        (void)drawOriginalGcParticleRandom50KG(expectedRaw);
        const auto cold=bank.prepare(assets,raw);check(cold.okay()&&cold.randomDraws==1&&raw==expectedRaw,"cold on-duration RNG order wrong");
        check(bank.manager()&&bank.manager()->group(8)->first==104&&bank.manager()->group(4)->first==52,"atlas groups wrong");
        check(bank.bindAttachment(Emit::pdFire,pd.graph,static_cast<SceneNodeIndex49P>(pd.plane),pd.plane,render),"fire graph binding failed");
        check(bank.bindAttachment(Emit::pdSmoke,pd.graph,static_cast<SceneNodeIndex49P>(pd.plane),pd.plane,{}),"smoke graph binding failed");
        check(bank.bindAttachment(Emit::pdExt,nozzle.graph,static_cast<SceneNodeIndex49P>(0),0,{}),"nozzle graph binding failed");
        check(!bank.bindAttachment(Emit::garageExplosion,pd.graph,static_cast<SceneNodeIndex49P>(pd.plane),pd.plane,render),"null74 explosion accepted fake attachment");
        check(bank.disable18(Emit::garageSparks),"disable first emitter failed");
        const auto id=static_cast<Emit>(tested);if(tested)activate(bank,id,raw);else check(bank.enable20(id),"enable spark");
        SharedRandomOwner50ID lazy;lazy.seed.lazy80A8=true;lazy.seed.seed3D6B30=1234;lazy.range.lazy80AC=true;lazy.range.rangeCheck3D6B38=1;
        HostMonotonicClock50ID clock;OriginalWreckExplosionGlobals50LA explosion;
        OriginalUiGarageParticleShared50KG shared{raw,fireLazy,serial,explosion,lazy,clock};
        const auto before=raw;auto visit=bank.stepPageEmitters(.016f,true,shared);
        check(visit.okay()&&raw==before&&bank.manager()->particleCount()==0,"paused emitter mutated RNG/pool");
        visit=bank.stepPageEmitters(.016f,false,shared);
        check(visit.okay()&&visit.commonSkipped&&raw==before&&bank.manager()->particleCount()==0,"first-update not skipped");
        visit=bank.stepPageEmitters(.016f,false,shared);
        if(!visit.okay())std::cerr<<"emitter "<<tested<<" status "<<unsigned(visit.status)<<" pc "<<std::hex<<visit.sourcePC<<std::dec<<" born "<<visit.spawned<<" skipped "<<visit.commonSkipped<<'\n';
        if(tested==2){
            OriginalUiGarageEmitterSnapshot50KG carry;
            check(visit.okay()&&visit.spawned==0&&bank.snapshot(id,carry)&&carry.remainder200>0,
                "small smoke demand did not preserve source carry");
            // The source adds old carry twice, not a conventional accumulator.
            const auto old=carry.remainder200;visit=bank.stepPageEmitters(.016f,false,shared);
            check(visit.okay()&&visit.spawned==0&&bank.snapshot(id,carry)&&carry.remainder200>2*old,
                "source smoke double-carry changed");
            visit=bank.stepPageEmitters(.016f,false,shared);
        }
        check(visit.okay()&&visit.spawned>0&&visit.spawned==bank.manager()->particleCount(),"concrete emitter numeric did not run");
        if(tested==3){check(visit.spawned==1&&visit.randomDraws==3&&explosion.emissionCounter433978==1,"unattached explosion semantics wrong");
            check(bank.stop84(id),"explosion stop failed");OriginalUiGarageEmitterSnapshot50KG snap;
            check(bank.snapshot(id,snap)&&(snap.controls.flags1D4&2),"one-shot protected stop erased active explosion");}
        if(tested==1){
            stamp=6;const auto gateRaw=raw;visit=bank.stepPageEmitters(.001f,false,shared);
            check(visit.okay()&&visit.renderGateSkipped&&visit.spawned==0&&raw==gateRaw,"live fire visibility gate ignored");stamp=7;
        }
        const auto* first=bank.manager()->particleAt(0);check(first&&first->ageMilliseconds>=0,"particle publication invalid");
        check(bank.stepPool(.016f,false).okay(),"real fixed-pool advance failed");
        if(tested==1){
            const auto heldRaw=raw,heldSerial=serial;
            visit=bank.stepPageEmitters(.001f,false,shared);
            OriginalUiGarageEmitterSnapshot50KG snap;
            check(visit.status==OriginalGcSprayStatus50KG::unsupported&&visit.sourcePC==0x15e298&&
                visit.callerCarryUnavailable&&visit.spawned==0&&raw==heldRaw&&serial==heldSerial&&
                bank.snapshot(id,snap)&&snap.fractional224>0&&snap.fractional224<=1,
                "unproved small-dt caller count was guessed or its carry prefix lost");
            const auto heldCarry=snap.fractional224;
            visit=bank.stepPageEmitters(.016f,false,shared);
            check(visit.callerCarryUnavailable&&bank.snapshot(id,snap)&&snap.fractional224==heldCarry&&raw==heldRaw,
                "terminal unsupported prefix was retried");
        }
    }
    // Always-on random mode really cycles while the Entity remains enabled.
    {
        OriginalUiGarageParticles50KG bank;std::uint32_t raw=0xabcdef12,serial=0;bool fireLazy=true;
        check(bank.prepare(assets,raw).okay(),"random-mode preparation");
        SharedRandomOwner50ID lazy;HostMonotonicClock50ID clock;OriginalWreckExplosionGlobals50LA explosion;
        OriginalUiGarageParticleShared50KG shared{raw,fireLazy,serial,explosion,lazy,clock};
        check(bank.stepPageEmitters(.016f,false,shared).commonSkipped,"random first update");
        unsigned transitions=0;
        for(unsigned tick=0;tick<300&&transitions<2;++tick){
            OriginalUiGarageEmitterSnapshot50KG before,after;
            check(bank.snapshot(Emit::garageSparks,before),"random snapshot");
            auto expected=raw;const auto unit=drawOriginalGcParticleRandom50KG(expected);
            const auto r=bank.stepPageEmitters(.016f,false,shared);
            check(r.okay()&&bank.snapshot(Emit::garageSparks,after)&&after.enabled,"random-mode owner lost");
            if((before.controls.flags1D4^after.controls.flags1D4)&2u){
                ++transitions;
                if(!(after.controls.flags1D4&2u)){
                    volatile float span=0.f-3.3f;volatile float product=span*unit;volatile float duration=3.3f+product;
                    check(r.randomDraws==1&&r.spawned==0&&raw==expected&&
                        after.duration1E4==std::bit_cast<std::uint32_t>(float(duration)),
                        "random off transition repaired max-off defect or reordered RNG");
                } else check(r.randomDraws>=1&&r.spawned>0,"random restart omitted same-tick derived emission");
            }
            check(bank.stepPool(.016f,false).okay(),"random-mode pool step");
        }
        check(transitions==2,"always-on emitter never completed on/off cycle");
    }
    // The no-trigger argc domain does not share pointer-domain invariance.
    // These are counterexamples, not an assertion about actual source argc.
    auto boundedBirths=[](std::int32_t bound){unsigned count=0;
        for(std::int32_t i=0;i<bound;++i){if(count==5000)break;++count;}
        return count;};
    check(boundedBirths(0)==0&&boundedBirths(1)==1&&boundedBirths(2)==2,
        "external startup argc dependency was incorrectly erased");
    // Bounded independent source-loop oracle for conditional pointer domain.
    for(const std::int32_t pointer:{0x44f600,0x700000,0x1eff000})for(const unsigned capacity:{0u,1u,37u,5000u})
      for(const unsigned prior:{0u,capacity/2,capacity}){
        unsigned count=prior,serial=13,draws=0;bool full=false;
        for(std::int32_t i=0;i<pointer;++i){if(count>=capacity){full=true;break;}++serial;draws+=3;++count;}
        check(full&&count==capacity&&serial==13+capacity-prior&&draws==3*(capacity-prior),"conditional bounded source-loop proof failed");
      }
}
}
int main(int argc,char** argv){try{check(argc==2,"original disc path required");run(argv[1]);
 std::cout<<"Garage seven authenticated particle defined numeric branches passed; caller-dependent fire carry held. No live page/material/Entity/render admission.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
