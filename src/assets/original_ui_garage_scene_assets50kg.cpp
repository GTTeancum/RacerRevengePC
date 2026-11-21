// Authenticated original-source reconstruction. Cold preparation only.
#include "original_ui_garage_scene_assets50kg.h"
#include <bit>
#include <cstring>
#include <utility>

namespace rr::assets {
struct OriginalUiGarageSceneAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> garage,pod19;
    OriginalUiDat50KG scene,toy;
    std::array<OriginalUiGarageStaticModel50KG,originalUiGarageStaticModelCount50KG> statics;
    std::array<OriginalUiGarageActorAsset50KG,originalUiGarageActorCount50KG> actors;
    OriginalUiGarageSelectedPodAsset50KG selected;
    int addonCareer=-1; // DLC career: the anchor 100C0; -1 = stock (career from the catalog identity)
};
namespace {
using D=OriginalUiGarageSceneAssetDiagnostic50KG;
using E=OriginalUiGarageSceneAssetError50KG;
bool fail(D& d,E error,std::uint32_t pc) noexcept {d.error=error;d.sourcePC=pc;return false;}
bool archiveIs(const std::shared_ptr<const OriginalUiArchive50KG>& a,OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==originalUiArchiveIdentity50KG(id);
}
bool textIs(const OriginalUiDatSection50KG& s,const char* key,const std::string& expected) noexcept {
    const auto* v=s.find(key);return v&&v->text==expected;
}
bool simpleStem(const std::string& name,const char* extension,std::string& stem) {
    const auto dot=name.find_last_of('.');
    if(dot==std::string::npos||name.substr(dot)!=extension||!dot||dot>63)return false;
    stem=name.substr(0,dot);
    for(unsigned char c:stem)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||
            (c>='0'&&c<='9')||c=='_'))return false;
    return true;
}
std::shared_ptr<const OriginalUiGarageModelAsset50KG> model(
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,const std::string& name,std::uint32_t nodes,D& d) {
    auto result=std::make_shared<OriginalUiGarageModelAsset50KG>();result->archive=archive;result->source=archive->find(name);
    if(!result->source.identity){fail(d,E::member,0x26E990);return {};}
    if(!readPsgHierarchy49O(result->source.data,result->source.size,result->hierarchy,d.model)||
       result->hierarchy.nodeCount!=nodes){fail(d,E::hierarchy,0x28CAC0);return {};}
    auto payload=std::make_shared<OriginalStandardModelResource50LA>();
    const std::vector<std::uint8_t> bytes(result->source.data,result->source.data+result->source.size);
    // Reuse the real semantic decoder and preserve its actual strip-derived
    // order. No strict header-strip equality and no fabricated palette data.
    if(!payload->prepare(result->source.identity->name,bytes,d.resource)){
        fail(d,E::geometry,d.resource.sourcePC);return {};
    }
    const auto* scene=payload->scene();
    if(!scene||scene->nodes.size()!=nodes||scene->world_space||
       scene->identity_matrix_palette!=(nodes>1)||scene->lods.size()!=1){fail(d,E::geometry,0x26EE24);return {};}
    result->payload=std::move(payload);return result;
}
constexpr const char* staticStems[]{
    "batterypack","batterypack01","charger","elan_engine","ground",
    "lightray01","lightray02","lightray03","lightray04","lightray05","lightray06",
    "OilBox","OilBox01","OilBox02","pump","tube01","tube02","tube03","tube04",
    "tube05","tube06","tubes","tubes01","tubes02","tubes03","tubes04","tubes05","tubes06","tubes07","tubes08"};
constexpr const char* actorStems[]{"pd_01","pd_02","pd_03","pd_bx","pd_lt","pd_fire","pd_ext","tank","nozzle","pd_fall"};
static_assert(std::size(staticStems)==originalUiGarageStaticModelCount50KG);
static_assert(std::size(actorStems)==originalUiGarageActorCount50KG);
}
OriginalUiGarageSceneAssets50KG::OriginalUiGarageSceneAssets50KG() noexcept=default;
OriginalUiGarageSceneAssets50KG::~OriginalUiGarageSceneAssets50KG()=default;
bool OriginalUiGarageSceneAssets50KG::loadAddon(std::shared_ptr<const OriginalUiArchive50KG> garage,
    std::shared_ptr<const OriginalUiArchive50KG> addonUi,const std::string& garageModel,int anchorCareer,D& d) noexcept {
    // PORT EXTENSION, NOT ORIGINAL: see the header. The statics and actors are
    // built by the one stock body (loadImpl); only the selected model's source differs.
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1FB0F0);
    if(!addonUi||!addonUi->loaded()||!addonUi->addon()||garageModel.empty()||anchorCareer<1||anchorCareer>26)
        return fail(d,E::archive,0x1FB0F0);
    return loadImpl(std::move(garage),std::move(addonUi),&garageModel,anchorCareer,d);
}
bool OriginalUiGarageSceneAssets50KG::addon()const noexcept{return storage_&&storage_->addonCareer>=0;}
bool OriginalUiGarageSceneAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> garage,
    std::shared_ptr<const OriginalUiArchive50KG> pod19,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1FB0F0);
    if(!archiveIs(garage,OriginalUiArchiveId50KG::garage)||
       (!pod19||!pod19->loaded()||!pod19->identity()||
        originalUiGaragePodCareer50KG(pod19->identity()->id)<0||
        !archiveIs(pod19,pod19->identity()->id)))return fail(d,E::archive,0x1FB0F0);
    return loadImpl(std::move(garage),std::move(pod19),nullptr,-1,d);
}
bool OriginalUiGarageSceneAssets50KG::loadImpl(std::shared_ptr<const OriginalUiArchive50KG> garage,
    std::shared_ptr<const OriginalUiArchive50KG> pod19,const std::string* addonModel,int addonCareer,D& d) noexcept {
    if(!archiveIs(garage,OriginalUiArchiveId50KG::garage))return fail(d,E::archive,0x1FB0F0);
    try {
        auto n=std::make_unique<Storage>();n->garage=std::move(garage);n->pod19=std::move(pod19);
        if(!readOriginalUiDat50KG(*n->garage,"UI/UIGarage/garage.scn",n->scene,d.text,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !readOriginalUiDat50KG(*n->garage,"UI/UIGarage/garage.toy",n->toy,d.text,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
            return fail(d,E::config,0x1FB0F0);
        const auto* statics=n->scene.section("StaticModels");const auto* animations=n->toy.section("Animations");
        std::int32_t count=0;
        if(!statics||!animations||!textIs(*statics,"SearchType","List")||
           !originalUiInteger50KG(*statics,"NumberOfStaticModels",0,count,d.text,0x272800)||count!=30||
           !originalUiInteger50KG(*animations,"NumberOfAnimations",0,count,d.text,0x2D8A60)||count!=10)
            return fail(d,E::config,0x1FB0F0);
        for(std::size_t i=0;i<n->statics.size();++i){
            d.ordinal=static_cast<std::uint32_t>(i+1);auto& s=n->statics[i];s.ordinal=d.ordinal;
            const auto* row=n->scene.section("Model"+std::to_string(i+1));
            s.slt=std::string(staticStems[i])+".slt";
            if(!row||!textIs(*row,"SLT",s.slt)||!row->find("Position")||!row->find("LookVector")||row->find("UpVector")||
               !originalUiTriple50KG(*row,"Position",{},s.position,d.text,0x272800)||
               !originalUiTriple50KG(*row,"LookVector",{},s.look,d.text,0x272800))return fail(d,E::config,0x272800);
            s.model=model(n->garage,"UI/Geometry/"+std::string(staticStems[i])+".psg",1,d);
            if(!s.model)return false;
        }
        // Eight actors share original pd.psg payload, not their mutable model,
        // source row table, playback state, Entity or material reference count.
        const auto pd=model(n->garage,"UI/Anim/pd.psg",18,d);if(!pd)return false;
        const auto tank=model(n->garage,"UI/Anim/tank.psg",2,d);if(!tank)return false;
        const auto nozzle=model(n->garage,"UI/Anim/nozzle.psg",2,d);if(!nozzle)return false;
        for(std::size_t i=0;i<n->actors.size();++i){
            d.ordinal=static_cast<std::uint32_t>(i+1);auto& a=n->actors[i];a.ordinal=d.ordinal;
            const auto* row=n->toy.section("Animation"+std::to_string(i+1));a.mcfName=std::string(actorStems[i])+".mcf";
            if(!row||!textIs(*row,"AnimationType","KeyFramed")||!textIs(*row,"MCF",a.mcfName)||
               !originalUiInteger50KG(*row,"StartingZone",0,count,d.text,0x2D8A60)||count!=1||
               !row->find("Offset")||!originalUiTriple50KG(*row,"Offset",{},a.offset,d.text,0x2D8A60)||a.offset!=std::array<float,3>{}||
               !originalUiBoolean50KG(*row,"UseLighting",false,a.useLighting,d.text,0x2D8A60)||!a.useLighting||
               !originalUiBoolean50KG(*row,"UseBlendedKeys",false,a.useBlendedKeys,d.text,0x2D8A60)||!a.useBlendedKeys||
               !originalUiBoolean50KG(*row,"UseCollision",false,a.useCollision,d.text,0x2D8A60)||a.useCollision)
                return fail(d,E::config,0x2D8A60);
            a.startingZone=1;if(const auto* name=row->find("ReferenceName"))a.referenceName=name->text;
            if(!readOriginalUiDat50KG(*n->garage,"UI/Anim/"+a.mcfName,a.mcf,d.text,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
                return fail(d,E::config,0x1538B0);
            const auto* g=a.mcf.section("General info");const auto expectedCount=i==3||i==4?4:1;
            const std::string geometryStem=i==7?"tank":i==8?"nozzle":"pd";
            if(!g||!textIs(*g,"SLTFile",geometryStem+".slt")||!textIs(*g,"VUTfilename",geometryStem+".vut")||
               !textIs(*g,"RotationType","SHORTAXIS")||
               (g->find("ContentDirectory")&&!g->find("ContentDirectory")->text.empty())||
               !originalUiInteger50KG(*g,"NumberOfMotions",0,count,d.text,0x1538B0)||count!=expectedCount||
               !originalUiInteger50KG(*row,"NumberOfMotions",0,count,d.text,0x2D8A60)||count!=expectedCount)
                return fail(d,E::config,0x1538B0);
            a.motionCount=static_cast<std::uint32_t>(expectedCount);a.model=i==7?tank:i==8?nozzle:pd;
            a.vut=n->garage->find("UI/Anim/"+geometryStem+".vut");
            if(!a.vut.identity)return fail(d,E::member,0x153218);
            if(!readMotionNodeBindings49O(a.model->hierarchy,reinterpret_cast<const char*>(a.vut.data),a.vut.size,a.bindings,d.binding))
                return fail(d,E::binding,0x153218);
            for(std::uint32_t j=0;j<a.motionCount;++j){
                auto& m=a.motions[j];m.name=actorStems[i];if(a.motionCount>1)m.name+="_"+std::to_string(j+1);
                const auto* r=a.mcf.section("MOTION "+std::to_string(j+1));std::string vue;
                if(!r||!textIs(*r,"NAME",m.name)||!r->find("VUE")||!simpleStem(r->find("VUE")->text,".vue",vue)||vue!=m.name||
                   !textIs(*row,("MotionToPlay"+std::to_string(j+1)).c_str(),m.name)||!r->find("Looping")||
                   !originalUiBoolean50KG(*r,"Looping",false,m.looping,d.text,0x1523F0)||m.looping||
                   !originalUiInteger50KG(*r,"FrameCount",0,m.authoredFrames,d.text,0x1523F0)||m.authoredFrames<=0||
                   !originalUiNumber50KG(*r,"FPS",0,m.authoredFps,d.text,0x1523F0)||m.authoredFps<=0)
                    return fail(d,E::config,0x1523F0);
                m.source=n->garage->find("UI/Anim/"+m.name+".mot");if(!m.source.identity)return fail(d,E::member,0x152540);
                if(!readMot49O(m.source.data,m.source.size,m.motion,d.model))return fail(d,E::motion,0x152570);
                for(std::uint32_t k=0;k<m.motion.rotationCount;++k)if(m.motion.rotation[k].rowIndex>=a.bindings.count)return fail(d,E::motion,0x152570);
                for(std::uint32_t k=0;k<m.motion.positionCount;++k)if(m.motion.position[k].rowIndex>=a.bindings.count)return fail(d,E::motion,0x152570);
                // Runtime duration is the actual MOT header, not the authored
                // MCF FrameCount/FPS (which is double duration here).
            }
        }
        const int career=addonModel?addonCareer:originalUiGaragePodCareer50KG(n->pod19->identity()->id);
        // DLC career: the add-on's own garage model member (ui.garage_model).
        const std::string selectedPath=addonModel?*addonModel:"UI/Geometry/"+std::string(career<10?"0":"")+
            std::to_string(career)+"e.psg";
        n->addonCareer=addonModel?addonCareer:-1;
        d.ordinal=0;n->selected.model=model(n->pod19,selectedPath.c_str(),1,d);
        if(!n->selected.model)return false;
        //1D1D94..1D1DCC immediate source words, NOT PSG bind translation.
        n->selected.position={{std::bit_cast<float>(0x4278ab14u),std::bit_cast<float>(0x4141c3edu),std::bit_cast<float>(0x423dd6d4u)}};
        n->selected.look={{0,0,1}};n->selected.up={{0,1,0}};
        storage_=std::move(n);d={};return true;
    }catch(...){return fail(d,E::allocation,0x1FB0F0);}
}
bool OriginalUiGarageSceneAssets50KG::loaded()const noexcept{return bool(storage_);}
const OriginalUiGarageStaticModel50KG* OriginalUiGarageSceneAssets50KG::staticModel(std::size_t i)const noexcept{return storage_&&i<storage_->statics.size()?&storage_->statics[i]:nullptr;}
const OriginalUiGarageActorAsset50KG* OriginalUiGarageSceneAssets50KG::actor(std::size_t i)const noexcept{return storage_&&i<storage_->actors.size()?&storage_->actors[i]:nullptr;}
const OriginalUiGarageSelectedPodAsset50KG* OriginalUiGarageSceneAssets50KG::selectedPod()const noexcept{return storage_?&storage_->selected:nullptr;}
const OriginalUiDat50KG* OriginalUiGarageSceneAssets50KG::sceneDat()const noexcept{return storage_?&storage_->scene:nullptr;}
const OriginalUiDat50KG* OriginalUiGarageSceneAssets50KG::toyDat()const noexcept{return storage_?&storage_->toy:nullptr;}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiGarageSceneAssets50KG::archive()const noexcept{return storage_?storage_->garage:nullptr;}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiGarageSceneAssets50KG::selectedArchive()const noexcept{return storage_?storage_->pod19:nullptr;}
}

namespace rr::assets {
int OriginalUiGarageSceneAssets50KG::selectedCareer()const noexcept {
    if(storage_&&storage_->addonCareer>=0)return storage_->addonCareer; // DLC career: the anchor 100C0
    const auto archive=selectedArchive();
    return archive&&archive->identity()?originalUiGaragePodCareer50KG(archive->identity()->id):-1;
}
}
