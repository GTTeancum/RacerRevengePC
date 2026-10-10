#include "original_ui_main_scene50kg.h"
#include <cmath>
#include <utility>

namespace rr::game {
using namespace assets;
struct OriginalUiMainSceneOwner50KG::Storage {
    const OriginalUiArchiveIdentity50KG* archive=nullptr;
    OriginalUiMainSceneDefinition50KG definition;
    OriginalUiDat50KG scn,toy,prt,emt,psm;
    Texture48E texture;
};
namespace {
struct Reader {
    OriginalUiMainSceneDiagnostic50KG& d;
    const OriginalUiDatSection50KG* s=nullptr;
    bool section(const OriginalUiDat50KG& doc,const char* name) {
        s=doc.section(name);
        if(!s){d.page.error=OriginalUiPageError50KG::missingSection;return false;}
        return true;
    }
    bool present(const char* key) {
        if(s && s->find(key))return true;
        d.page={OriginalUiPageError50KG::missingKey,s?s->sourceOffset:0,0};return false;
    }
    bool i(const char* k,std::int32_t& v,std::uint32_t pc=0) {
        return present(k)&&originalUiInteger50KG(*s,k,0,v,d.page,pc);
    }
    bool n(const char* k,float& v,std::uint32_t pc=0) {
        return present(k)&&originalUiNumber50KG(*s,k,0,v,d.page,pc);
    }
    bool b(const char* k,bool& v,std::uint32_t pc=0) {
        return present(k)&&originalUiBoolean50KG(*s,k,false,v,d.page,pc);
    }
    bool v(const char* k,std::array<float,3>& a,std::uint32_t pc=0) {
        return present(k)&&originalUiTriple50KG(*s,k,{},a,d.page,pc);
    }
    bool text(const char* k,const char* expected) {
        return present(k)&&s->find(k)->text==expected;
    }
};
bool readDefinition(const OriginalUiDat50KG& scn,const OriginalUiDat50KG& toy,
    const OriginalUiDat50KG& prt,const OriginalUiDat50KG& emt,const OriginalUiDat50KG& psm,
    OriginalUiMainSceneDefinition50KG& out,OriginalUiMainSceneDiagnostic50KG& d) {
    Reader r{d};auto& f=out.fire;std::int32_t count=0;
    d.member=scn.identity()->name;
    if(!r.section(scn,"SceneInfo")||!r.text("SceneName","MainMenu")||
       !r.section(scn,"StaticModels")||!r.i("NumberOfStaticModels",out.staticModelCount)||
       !r.section(scn,"Environment")||!r.text("ParticleTextureMapName","mmprt.tga")||
       !r.section(scn,"Fog")||!r.v("ColorRGB",out.fogColor)||
       !r.n("Visibility",out.fogVisibility)||!r.n("Haziness",out.fogHaziness)||
       !r.section(scn,"Lights")||!r.i("NumberOfLights",count)||count!=2||
       !r.section(scn,"Light1")||!r.text("Type","Ambient")||!r.v("ColorRGB",out.ambientColor)||
       !r.section(scn,"Light2")||!r.text("Type","Directional")||
       !r.v("ColorRGB",out.directionalColor)||!r.v("Position",out.lightPosition)||
       !r.v("TargetPosition",out.lightTarget))return false;
    d.member=toy.identity()->name;
    if(!r.section(toy,"CSCameras")||!r.i("NumberOfCameras",out.sceneCameraCount)||
       !r.section(toy,"Animations")||!r.i("NumberOfAnimations",out.animationCount)||
       !r.section(toy,"ParticleEmitters")||!r.i("NumberOfEmitters",count,0x27de64)||count!=1||
       !r.section(toy,"ParticleEmitter1")||!r.i("StartingZone",f.startingZone,0x27c628)||
       !r.i("TypeID",f.type,0x27c640)||!r.v("Position",f.position,0x27c694)||
       !r.text("EmitterFilename","mmfire.emt")||!r.i("NumberOfInstances",f.instances)||
       !r.b("StartOn",f.startOn,0x27c72c)||!r.b("Additive",f.authoredToyAdditive))return false;
    // 27C678 takes absent AttachToSoultree=NONE branch, which sets instances=1.
    // The explicit authored NumberOfInstances agrees; Toy Additive is retained
    // metadata. Actual additive runtime bit is read from EMT Properties.
    if(r.s->find("AttachToSoultree")||f.instances!=1||f.type!=8||
       out.staticModelCount!=0||out.sceneCameraCount!=0||out.animationCount!=0)return false;
    d.member="particle\\mmfire.emt";
    if(!r.section(emt,"General Info")||!r.i("ParticleGroup",f.particleGroup,0x15aee0)||
       !r.n("ParticleLife",f.lifeMilliseconds,0x15af70)||!r.n("InitialSize",f.initialSize,0x15af8c)||
       !r.n("Growth",f.growth,0x15afa4)||!r.n("EmitRate",f.ratePerSecond,0x15afc0)||
       !r.n("AgeAtBirth",f.ageAtBirth,0x15afd8)||!r.v("Velocity",f.velocity,0x15b004)||
       !r.v("MinRange",f.minRange,0x15e710)||!r.v("MaxRange",f.maxRange,0x15e72c)||
       !originalUiInteger50KG(*r.s,"EmitMode",0,f.emitMode,d.page,0x15b01c)||
       !originalUiNumber50KG(*r.s,"DelayTime",0,f.delaySeconds,d.page,0x15b03c)||
       !originalUiBoolean50KG(*r.s,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d.page,0x15e744)||
       !r.section(emt,"Properties")||!r.b("Fades",f.fades,0x15b1e8)||
       !r.b("Additive",f.additive,0x15b260)||!r.b("HasVelocity",f.hasVelocity,0x15b200)||
       !r.b("Grows",f.grows,0x15b218)||!r.b("FadeNonLinear",f.fadeNonLinear,0x15b230)||
       !r.b("Glows",f.glows,0x15b248)||!r.b("RandomMirrorAboutX",f.randomMirrorX,0x15b2a8)||
       !r.b("RandomMirrorAboutY",f.randomMirrorY,0x15b2c0)||
       !originalUiBoolean50KG(*r.s,"Gravity",false,f.gravity,d.page,0x15b278)||
       !originalUiBoolean50KG(*r.s,"Lit",true,f.lit,d.page,0x15b290)||
       !r.section(emt,"Animation")||!r.n("FramesPerSet",f.framesPerSet,0x15b490)||
       !originalUiInteger50KG(*r.s,"NumberOfForcedSets",0,f.forcedSetCount,d.page,0x15b4c0))return false;
    f.sourceParticleFlags=(f.additive?1u:0u)|(f.fades?2u:0u)|(f.hasVelocity?4u:0u)|
       (f.grows?8u:0u)|(f.gravity?0x20u:0u)|(f.fadeNonLinear?0x1000u:0u)|
       (f.glows?0x2000u:0u)|(!f.lit?0x800u:0u)|(f.framesPerSet>1?0x400u:0u);
    if(f.particleGroup!=4||f.lifeMilliseconds<=0||f.initialSize<=0||f.framesPerSet!=8||
       f.emitMode!=0||f.delaySeconds!=0||f.forcedSetCount!=0||f.gravity||f.fades||f.grows)return false;
    d.member="particle\\mmprt.prt";
    if(!r.section(prt,"General Info")||!r.text("TextureMap","mmprt.tga")||
       !r.i("NumFrames",count,0x161874)||count!=104||
       !r.i("NumGroups",count,0x16188c)||count!=8)return false;
    for(unsigned i=0;i<out.groups.size();++i){
        auto& g=out.groups[i];const auto name="Group"+std::to_string(i);
        if(!r.section(prt,name.c_str())||!r.i("TypeID",g.type,0x16190c)||
           !r.i("FirstFrame",g.first,0x161928)||!r.i("LastFrame",g.last,0x161944)||
           g.first<0||g.last<g.first||g.last>=104)return false;
    }
    for(unsigned i=0;i<out.frames.size();++i){
        float l=0,t=0,rr=0,b=0;const auto name="Frame"+std::to_string(i);
        if(!r.section(prt,name.c_str())||!r.n("UVLeft",l,0x1619c8)||
           !r.n("UVRight",rr,0x1619dc)||!r.n("UVTop",t,0x1619f0)||
           !r.n("UVBottom",b,0x161a04)||l>rr||t>b)return false;
        out.frames[i].sourceUv={{{{l,t}},{{rr,t}},{{rr,b}},{{l,b}}}};
    }
    const auto& group=out.groups[4];
    if(group.type!=f.type||group.first!=52||group.last!=75)return false;
    // Validate the actual two-map ParticleCode authoring. Blank state values
    // remain absent; the generic ScreenObject defaults are NOT applied here.
    d.member="particle\\mmprt.psm";
    if(!r.section(psm,"General")||!r.i("TotalShaderPasses",count)||count!=1||
       !r.section(psm,"ShaderPass_1")||!r.text("microProgramName","ParticleCode")||
       !r.i("totalTextureMaps",count)||count!=2||
       !r.text("TextureMap_1","mmprt")||!r.text("TextureMap_2","mmprt"))return false;
    return true;
}
}
OriginalUiMainSceneOwner50KG::OriginalUiMainSceneOwner50KG() noexcept=default;
OriginalUiMainSceneOwner50KG::~OriginalUiMainSceneOwner50KG()=default;
bool OriginalUiMainSceneOwner50KG::prepare(const OriginalUiArchive50KG& a,
    OriginalUiMainSceneDiagnostic50KG& d) noexcept {
    return prepareScene(a,false,d);
}
bool OriginalUiMainSceneOwner50KG::prepareOptionsMenu(const OriginalUiArchive50KG& a,
    OriginalUiMainSceneDiagnostic50KG& d) noexcept {
    return prepareScene(a,true,d);
}
bool OriginalUiMainSceneOwner50KG::prepareScene(const OriginalUiArchive50KG& a,bool options,
    OriginalUiMainSceneDiagnostic50KG& d) noexcept {
    d={};
    if(storage_){d.status=OriginalUiMainSceneStatus50KG::alreadyPrepared;return false;}
    if(!a.loaded()||a.identity()!=originalUiArchiveIdentity50KG(options?OriginalUiArchiveId50KG::options:OriginalUiArchiveId50KG::mainMenu)){
        d.status=OriginalUiMainSceneStatus50KG::archive;return false;}
    try {
        auto p=std::make_unique<Storage>();p->archive=a.identity();
        const char* names[]={options?"ui\\UIOption\\opt.scn":"ui\\UIMain\\mm.scn",
            options?"ui\\UIOption\\opt.toy":"ui\\UIMain\\mm.toy","particle\\mmprt.prt",
            "particle\\mmfire.emt","particle\\mmprt.psm"};
        OriginalUiDat50KG* docs[]={&p->scn,&p->toy,&p->prt,&p->emt,&p->psm};
        for(unsigned i=0;i<5;++i){d.member=names[i];
            if(!readOriginalUiDat50KG(a,names[i],*docs[i],d.page)){
                d.status=OriginalUiMainSceneStatus50KG::authoredData;return false;}}
        if(!readDefinition(p->scn,p->toy,p->prt,p->emt,p->psm,p->definition,d)){
            d.status=OriginalUiMainSceneStatus50KG::unsupported;return false;}
        d.member="particle\\mmprt.pst";const auto texture=a.find(d.member);
        if(!texture.identity||!texture.data){d.status=OriginalUiMainSceneStatus50KG::archive;return false;}
        const std::vector<std::uint8_t> bytes(texture.data,texture.data+texture.size);
        if(!decodeTexture48E(bytes,p->texture,d.texture)||p->texture.mips.size()!=1||
           p->texture.mips[0].width!=128||p->texture.mips[0].height!=256){
            d.status=OriginalUiMainSceneStatus50KG::texture;return false;}
        // Bounds concern ONLY the selected group. Other PRT groups are shared
        // legacy metadata and reference a larger unrelated atlas layout.
        for(unsigned i=52;i<=75;++i)for(const auto& uv:p->definition.frames[i].sourceUv)
            if(!std::isfinite(uv[0])||!std::isfinite(uv[1])||uv[0]<0||uv[0]>=128||uv[1]<0||uv[1]>=256){
                d.status=OriginalUiMainSceneStatus50KG::unsupported;return false;}
        storage_=std::move(p);d={};return true;
    }catch(...){d.status=OriginalUiMainSceneStatus50KG::allocation;return false;}
}
bool OriginalUiMainSceneOwner50KG::prepared() const noexcept{return bool(storage_);}
const OriginalUiArchiveIdentity50KG* OriginalUiMainSceneOwner50KG::archiveIdentity() const noexcept {
    return storage_?storage_->archive:nullptr;
}
const OriginalUiMainSceneDefinition50KG* OriginalUiMainSceneOwner50KG::definition() const noexcept{
    return storage_?&storage_->definition:nullptr;}
const Texture48E* OriginalUiMainSceneOwner50KG::particleTexture() const noexcept{
    return storage_?&storage_->texture:nullptr;}
const OriginalUiDat50KG* OriginalUiMainSceneOwner50KG::particleMaterial() const noexcept{
    return storage_?&storage_->psm:nullptr;}
const OriginalUiDat50KG* OriginalUiMainSceneOwner50KG::sceneAuthoring() const noexcept{
    return storage_?&storage_->scn:nullptr;}
const OriginalUiDat50KG* OriginalUiMainSceneOwner50KG::toyAuthoring() const noexcept{
    return storage_?&storage_->toy:nullptr;}
const OriginalUiDat50KG* OriginalUiMainSceneOwner50KG::emitterAuthoring() const noexcept{
    return storage_?&storage_->emt:nullptr;}
const OriginalUiDat50KG* OriginalUiMainSceneOwner50KG::atlasAuthoring() const noexcept{
    return storage_?&storage_->prt:nullptr;}

}
