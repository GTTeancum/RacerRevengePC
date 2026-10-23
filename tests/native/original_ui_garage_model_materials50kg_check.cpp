// Cold source/data/ownership checks only; never starts a page or renderer.
#include "original_ui_garage_scene_assets50kg.h"
#include "original_ui_garage_model_materials50kg.h"
#include "original_ui_ipum_texture_cache50kg.h"
#include "original_ui_rider_material_lifetime50kg.h"
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
    auto garage=load(root,Id::garage),pod=load(root,Id::garagePod19),app=load(root,Id::appStart);
    auto a=std::make_shared<OriginalUiGarageSceneAssets50KG>();OriginalUiGarageSceneAssetDiagnostic50KG ad;
    check(a->load(garage,pod,ad),"scene asset load failed");
    auto metadata=std::make_shared<OriginalStreamMetadata50KZ>();StreamMetadataDiagnostic50KZ sd;
    check(metadata->prepare(*app,sd),"stream metadata failed");
    auto watto=std::make_shared<OriginalUiGarageWattoAssets50KG>();OriginalUiGarageWattoAssetDiagnostic50KG wd;
    check(watto->load(garage,metadata,wd),"authentic Watto asset closure failed");
    OriginalUiIpumTextureCache50KG cache;
    for(std::size_t i=0;i<cache.garageCatalogSourceCount();++i){OriginalUiIpumCacheDiagnostic50KG cd;
        const auto archive=cache.garageSourceArchive(i)==garage->identity()?garage:pod;
        check(cache.loadGarageOriginal(i,archive,cd),"authentic Garage IPUM cache failed");}
    OriginalUiMaterialRuntime50KG runtime;runtime.overrideKnown=true;
    std::vector<std::unique_ptr<OriginalUiGarageModelMaterials50KG>> held;
    unsigned passCount=0;std::set<std::uint32_t> rows;
    for(unsigned i=0;i<42;++i){
        auto m=std::make_unique<OriginalUiGarageModelMaterials50KG>();OriginalUiGarageModelMaterialDiagnostic50KG d;
        const bool ok=i<30?m->loadStatic(a,i,app,runtime,&cache,d):i<40?m->loadActor(a,i-30,app,runtime,&cache,d):i==40?m->loadSelected(a,app,runtime,&cache,d):m->loadWatto(watto,garage,app,runtime,&cache,d);
        if(!ok){std::cerr<<"domain="<<i<<" error="<<unsigned(d.error)<<" material="<<d.material<<" pass="<<d.pass<<" field="<<d.materialDetail.field<<" source="<<std::hex<<d.materialDetail.sourcePC<<std::dec<<" image="<<d.images.image<<" texture="<<d.images.texture.code<<"\n";throw std::runtime_error("authentic model material preparation failed");}
        rr::game::OriginalUiRiderMaterialFootprint50KG f;rr::game::OriginalUiRiderMaterialLifetimeDiagnostic50KG fd;
        check(i<30?f.garageStatic(*app,*a,i,f,fd):i<40?f.garageActor(*app,*a,i-30,f,fd):i==40?f.garageSelected(*app,*a,f,fd):f.garageWatto(*app,*watto,f,fd),"canonical footprint failed");
        check(f.widget()==(i==41?10u:0u),"fabricated widget identity");if(i<40)check(f.garageOrdinal()==(i<30?i+1:i-29),"wrong source ordinal");
        for(std::size_t r=0;r<f.rowCount();++r)rows.insert(f.row(r)->rank());
        for(unsigned slot=0;slot<m->materialCount();++slot)for(unsigned p=0;p<m->passCount(slot);++p){
            const auto* pass=m->pass(slot,p);check(pass&&pass->source.pass==p+1&&pass->source.identity&&pass->source.inheritedTemplate,"original pass provenance missing");++passCount;
            if(pass->source.textureCount)check(m->images()&&m->images()->sourceProvenance(pass->imageIndex),"original texture provenance missing");
        }
        held.push_back(std::move(m));
    }
    check(held[40]->passCount(0)==2&&held[40]->pass(0,0)->source.microprogram=="PreLitCompressed"&&
          held[40]->pass(0,1)->source.microprogram=="SpecularMapCompressed","selected original pass order lost");
    std::cout<<"AUTHENTIC Garage immutable material domains="<<held.size()<<" slot-passes="<<passCount<<" distinct canonical rows="<<rows.size()<<"\n";
    a.reset();garage.reset();pod.reset();app.reset();
    check(held.back()->images()->sourceProvenance(0)&&held.back()->modelIdentity(),"retained source lifetime lost");
}
}
int main(int argc,char** argv){try{check(argc==2,"usage: garage-scene-assets-check /path/to/original/disc");run(argv[1]);
    std::cout<<"Garage immutable material/footprint checks passed. No live renderer admission.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
