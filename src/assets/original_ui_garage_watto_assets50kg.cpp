#include "original_ui_garage_watto_assets50kg.h"
#include <cmath>
#include <cstring>
#include <utility>
namespace rr::assets {
struct OriginalUiGarageWattoAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    std::shared_ptr<const OriginalStreamMetadata50KZ> streams;
    OriginalUiMemberView50KG psg{},vut{};
    const OriginalUiMemberIdentity50KG* audio=nullptr;
    PsgHierarchyView49O hierarchy{};MotionNodeBindings49O bindings{};Scene48D scene;
    std::array<OriginalUiGarageWattoMotion50KG,32> motions{};
    std::array<OriginalUiGarageWattoEvent50KG,40> events{};
    std::array<std::uint32_t,64> mask{};
    std::array<float,3> position{},look{},up{};
    float random=0,switchSide=0;
};
namespace {
using E=OriginalUiGarageWattoAssetError50KG;
bool fail(OriginalUiGarageWattoAssetDiagnostic50KG& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
constexpr const char* motionNames[]{"wg_ent","w_lt_br2","w_rt_br2","w_lt_rt_1","w_rt_lt_1",
    "w_rt_1s","w_lt_1s","w_rt_2s","w_lt_2s","w_rt_3s","w_lt_3s","w_rt_4s","w_lt_4s",
    "w_rt_6s","w_lt_6s","w_rt_7s","w_lt_7s","w_rt_8s","w_lt_8s","w_rt_9s","w_lt_9s",
    "wp_lt_3s","wp_rt_3s","wp_rt_5s","wp_rt_6s","wp_rt_7s","wp_rt_8s","wp_lt_5s","wp_lt_6s","wp_lt_7s","wp_lt_8s","wings_01"};
bool textIs(const OriginalUiDatSection50KG& s,const char* key,const char* value) {
    const auto* f=s.find(key);return f&&f->text==value;
}
}
OriginalUiGarageWattoAssets50KG::OriginalUiGarageWattoAssets50KG() noexcept=default;
OriginalUiGarageWattoAssets50KG::~OriginalUiGarageWattoAssets50KG()=default;
bool OriginalUiGarageWattoAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::shared_ptr<const OriginalStreamMetadata50KZ> streams,OriginalUiGarageWattoAssetDiagnostic50KG& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1D4510);
    if(!archive||!archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage)||
       !streams||!streams->prepared())return fail(d,E::archive,0x1D4510);
    try {
        auto n=std::make_unique<Storage>();n->archive=std::move(archive);n->streams=std::move(streams);
        OriginalUiDat50KG widgets,mcf,audio;
        if(!readOriginalUiDat50KG(*n->archive,"UI/UIGarage/Widgets.Dat",widgets,d.text,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !readOriginalUiDat50KG(*n->archive,"UI/Anim/wg.mcf",mcf,d.text,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !readOriginalUiDat50KG(*n->archive,"audio/UIWatto.ini",audio,d.text))return fail(d,E::config,0x1D4510);
        const auto* w=widgets.section("Widget 10");const auto* g=mcf.section("General info");
        if(!w||!g||!textIs(*w,"Type","Watto")||!textIs(*w,"MCFFile","ui\\anim\\wg.mcf")||
           !textIs(*g,"SLTFile","wg.slt")||!textIs(*g,"VUTfilename","wg.vut")||
           !textIs(*g,"RotationType","SHORTAXIS")||!w->find("Position")||!w->find("LookVector")||!w->find("UpVector"))return fail(d,E::widget,0x1D4510);
        const auto* directory=g->find("ContentDirectory");std::int32_t count=0;
        if((directory&&!directory->text.empty())||
           !originalUiInteger50KG(*g,"NumberOfMotions",0,count,d.text,0x1538B0)||count!=32||
           !originalUiTriple50KG(*w,"Position",{},n->position,d.text,0x1D4564)||
           !originalUiTriple50KG(*w,"LookVector",{},n->look,d.text,0x1D4590)||
           !originalUiTriple50KG(*w,"UpVector",{},n->up,d.text,0x1D45BC)||
           !originalUiNumber50KG(*w,"RandomMotionPct",0.12f,n->random,d.text,0x1D4AD0)||
           !originalUiNumber50KG(*w,"SwitchSidesPct",0.12f,n->switchSide,d.text,0x1D4AF0))return fail(d,E::config,0x1538B0);
        n->psg=n->archive->find("UI/Anim/wg.psg");n->vut=n->archive->find("UI/Anim/wg.vut");
        if(!n->psg.identity||!n->vut.identity)return fail(d,E::member,0x1538B0);
        if(!readPsgHierarchy49O(n->psg.data,n->psg.size,n->hierarchy,d.model))return fail(d,E::hierarchy,0x152EA0);
        if(!readMotionNodeBindings49O(n->hierarchy,reinterpret_cast<const char*>(n->vut.data),n->vut.size,n->bindings,d.binding))return fail(d,E::binding,0x153218);
        n->mask.fill(1);
        for(const char* name:{"L_WING","R_WING","L_WING_TIP","R_WING_TIP"}){
            bool found=false;for(std::uint32_t row=0;row<n->bindings.count;++row)if(std::strcmp(n->bindings.rows[row].name,name)==0){n->mask[row]=0;found=true;break;}
            if(!found)return fail(d,E::binding,0x156580);
        }
        for(std::size_t i=0;i<n->motions.size();++i){
            d.ordinal=static_cast<std::uint32_t>(i+1);auto& m=n->motions[i];const auto* s=mcf.section("MOTION "+std::to_string(i+1));
            if(!s||!textIs(*s,"NAME",motionNames[i])||!textIs(*s,"VUE",(std::string(motionNames[i])+".vue").c_str()))return fail(d,E::config,0x1523F0);
            m.name=motionNames[i];
            if(!s->find("Looping")||!originalUiBoolean50KG(*s,"Looping",false,m.looping,d.text,0x1523F0)||
               !originalUiInteger50KG(*s,"FrameCount",0,m.authoredFrames,d.text,0x1523F0)||
               !originalUiNumber50KG(*s,"FPS",0,m.authoredFps,d.text,0x1523F0)||m.authoredFrames<=0||m.authoredFps<=0)return fail(d,E::config,0x1523F0);
            m.source=n->archive->find("UI/Anim/"+m.name+".mot");
            // The total-key opt-in is reachable only through this authenticated
            // loaded Garage archive and its exact pinned source member25..56.
            // A caller-provided name, digest, view or enum is not sufficient.
            if(!m.source.identity||m.source.identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::garage,25+i))
                return fail(d,E::member,0x152540);
            if(!readMot49O(m.source.data,m.source.size,m.motion,d.model,MotKeyAdmission50KG::authenticatedGarageWatto5587))
                return fail(d,E::motion,0x152570);
            for(std::uint32_t k=0;k<m.motion.rotationCount;++k)if(m.motion.rotation[k].rowIndex>=n->bindings.count)return fail(d,E::motion,0x152570);
            for(std::uint32_t k=0;k<m.motion.positionCount;++k)if(m.motion.position[k].rowIndex>=n->bindings.count)return fail(d,E::motion,0x152570);
        }
        const std::vector<std::uint8_t> bytes(n->psg.data,n->psg.data+n->psg.size);
        if(!decodeScene48D(bytes,n->scene,d.geometry)||n->scene.nodes.size()!=n->hierarchy.nodeCount||
           n->scene.world_space||!n->scene.identity_matrix_palette||n->scene.lods.empty())return fail(d,E::geometry,0x1538B0);
        const auto* r=audio.section("WattoResponses");const auto* e=audio.section("ResponseEmitter");
        if(!r||!e)return fail(d,E::audio,0x1D4850);
        constexpr const char* keys[]{"NumberOfSliderResponses","NumberOfPurchaseResponses","NumberOfNoPurchaseResponses","NumberOfNoMoneyResponses","NumberOfRandomResponses","NumberOfTransitionResponses"};
        constexpr std::int32_t counts[]{2,13,1,2,17,5};
        for(std::size_t i=0;i<6;++i)if(!originalUiInteger50KG(*r,keys[i],0,count,d.text,0x1D489C)||count!=counts[i])return fail(d,E::audio,0x1D4850);
        if(!originalUiInteger50KG(*e,"NumEvents",0,count,d.text,0x2926B8)||count!=40)return fail(d,E::audio,0x2926B8);
        for(std::uint32_t i=0;i<40;++i){auto& event=n->events[i];d.ordinal=i+1;event.id=i+1;
            const auto* s=audio.section("Event"+std::to_string(i+1));if(!s||!s->find("FileName"))return fail(d,E::audio,0x292700);
            event.filename=s->find("FileName")->text;
            if(!originalUiBoolean50KG(*s,"IsStream",false,event.stream,d.text,0x29274C)||
               !originalUiBoolean50KG(*s,"IsStereo",false,event.stereo,d.text,0x29271C)||
               !originalUiBoolean50KG(*s,"IsLoop",false,event.loop,d.text,0x292734)||
               !event.stream||event.stereo||event.loop)return fail(d,E::audio,0x292650);
            event.metadata=n->streams->findForStreamRequest(event.stream,event.filename);
            if(!event.metadata||!std::isfinite(event.metadata->admittedDurationSeconds)||event.metadata->admittedDurationSeconds<=0||
               event.metadata->admittedDurationSeconds>=10)return fail(d,E::audio,0x233234);
        }
        n->audio=audio.identity();storage_=std::move(n);d={};return true;
    }catch(...){return fail(d,E::allocation,0x1D4510);}
}
bool OriginalUiGarageWattoAssets50KG::loaded()const noexcept{return bool(storage_);}
const OriginalUiGarageWattoMotion50KG* OriginalUiGarageWattoAssets50KG::motion(std::size_t i)const noexcept{return storage_&&i<32?&storage_->motions[i]:nullptr;}
const OriginalUiGarageWattoMotion50KG* OriginalUiGarageWattoAssets50KG::findMotion(std::string_view name)const noexcept{if(storage_)for(const auto& m:storage_->motions)if(m.name==name)return &m;return nullptr;}
const OriginalUiGarageWattoEvent50KG* OriginalUiGarageWattoAssets50KG::event(std::uint32_t id)const noexcept{return storage_&&id&&id<=40?&storage_->events[id-1]:nullptr;}
const PsgHierarchyView49O* OriginalUiGarageWattoAssets50KG::hierarchy()const noexcept{return storage_?&storage_->hierarchy:nullptr;}
const MotionNodeBindings49O* OriginalUiGarageWattoAssets50KG::bindings()const noexcept{return storage_?&storage_->bindings:nullptr;}
const Scene48D* OriginalUiGarageWattoAssets50KG::scene()const noexcept{return storage_?&storage_->scene:nullptr;}
const std::array<std::uint32_t,64>* OriginalUiGarageWattoAssets50KG::wingMask()const noexcept{return storage_?&storage_->mask:nullptr;}
const std::array<float,3>* OriginalUiGarageWattoAssets50KG::position()const noexcept{return storage_?&storage_->position:nullptr;}
const std::array<float,3>* OriginalUiGarageWattoAssets50KG::look()const noexcept{return storage_?&storage_->look:nullptr;}
const std::array<float,3>* OriginalUiGarageWattoAssets50KG::up()const noexcept{return storage_?&storage_->up:nullptr;}
float OriginalUiGarageWattoAssets50KG::randomMotionProbability()const noexcept{return storage_?storage_->random:0;}
float OriginalUiGarageWattoAssets50KG::switchSideProbability()const noexcept{return storage_?storage_->switchSide:0;}
const OriginalUiMemberIdentity50KG* OriginalUiGarageWattoAssets50KG::geometrySource()const noexcept{return storage_?storage_->psg.identity:nullptr;}
const OriginalUiMemberIdentity50KG* OriginalUiGarageWattoAssets50KG::audioSource()const noexcept{return storage_?storage_->audio:nullptr;}
}
