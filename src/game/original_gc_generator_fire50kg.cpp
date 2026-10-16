#include "original_gc_generator_fire50kg.h"
#include "original_gc_fire_emission50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalGcParticleStatus50KG;
using Section=assets::OriginalUiDatSection50KG;
float add(float a,float b) noexcept{volatile float r=a+b;return r;}
float sub(float a,float b) noexcept{volatile float r=a-b;return r;}
float mul(float a,float b) noexcept{volatile float r=a*b;return r;}
float div(float a,float b) noexcept{volatile float r=a/b;return r;}
bool scalar(float v) noexcept{return std::isfinite(v)&&std::fpclassify(v)!=FP_SUBNORMAL;}
const Section* section(const std::vector<Section>& doc,const char* name) noexcept {
    for(const auto& s:doc)if(s.name==name)return &s;return nullptr;
}
struct Definition {
    std::int32_t group=0,mode=0,forcedSets=0;
    std::array<float,3> velocity{},minRange{},maxRange{};
    float life=0,size=0,growth=0,rate=0,birthFraction=0,frames=0,delay=0;
    bool mirrorX=false,mirrorY=false,spread=true;
    std::uint32_t flags=0;
};
bool read(const std::vector<Section>& doc,Definition& f) noexcept {
    assets::OriginalUiPageDiagnostic50KG d{};const auto* g=section(doc,"General Info");
    const auto* p=section(doc,"Properties");const auto* a=section(doc,"Animation");if(!g||!p||!a)return false;
    if(!assets::originalUiInteger50KG(*g,"ParticleGroup",-1,f.group,d,0x15aee0)||
       !assets::originalUiNumber50KG(*g,"ParticleLife",1000,f.life,d,0x15af70)||
       !assets::originalUiNumber50KG(*g,"InitialSize",1,f.size,d,0x15af8c)||
       !assets::originalUiNumber50KG(*g,"Growth",0,f.growth,d,0x15afa4)||
       !assets::originalUiNumber50KG(*g,"EmitRate",500,f.rate,d,0x15afc0)||
       !assets::originalUiNumber50KG(*g,"AgeAtBirth",0,f.birthFraction,d,0x15afd8)||
       !assets::originalUiTriple50KG(*g,"Velocity",{},f.velocity,d,0x15b004)||
       !assets::originalUiTriple50KG(*g,"MinRange",{},f.minRange,d,0x15e710)||
       !assets::originalUiTriple50KG(*g,"MaxRange",{},f.maxRange,d,0x15e72c)||
       !assets::originalUiBoolean50KG(*g,"ApplySpreadToVelocity",true,f.spread,d,0x15e744)||
       !assets::originalUiInteger50KG(*g,"EmitMode",0,f.mode,d,0x15b01c)||
       !assets::originalUiNumber50KG(*g,"DelayTime",0,f.delay,d,0x15b03c)||
       !assets::originalUiNumber50KG(*a,"FramesPerSet",-1,f.frames,d,0x15b490)||
       !assets::originalUiInteger50KG(*a,"NumberOfForcedSets",0,f.forcedSets,d,0x15b4c0))return false;
    bool fades=false,additive=false,velocity=false,grows=false,nonlinear=false,glows=false,gravity=false,lit=true;
    if(!assets::originalUiBoolean50KG(*p,"Fades",false,fades,d,0)||
       !assets::originalUiBoolean50KG(*p,"Additive",false,additive,d,0)||
       !assets::originalUiBoolean50KG(*p,"HasVelocity",false,velocity,d,0)||
       !assets::originalUiBoolean50KG(*p,"Grows",false,grows,d,0)||
       !assets::originalUiBoolean50KG(*p,"FadeNonLinear",false,nonlinear,d,0)||
       !assets::originalUiBoolean50KG(*p,"Glows",false,glows,d,0)||
       !assets::originalUiBoolean50KG(*p,"Gravity",false,gravity,d,0)||
       !assets::originalUiBoolean50KG(*p,"Lit",true,lit,d,0)||
       !assets::originalUiBoolean50KG(*p,"RandomMirrorAboutX",false,f.mirrorX,d,0)||
       !assets::originalUiBoolean50KG(*p,"RandomMirrorAboutY",false,f.mirrorY,d,0))return false;
    f.flags=(additive?1u:0u)|(fades?2u:0u)|(velocity?4u:0u)|(grows?8u:0u)|
        (gravity?0x20u:0u)|(!lit?0x800u:0u)|(nonlinear?0x1000u:0u)|(glows?0x2000u:0u)|(f.frames>1?0x400u:0u);
    return f.group==4&&f.mode==0&&f.delay==0&&f.forcedSets==0&&f.life==650&&f.size==12&&
        f.growth==-4&&f.rate==50&&f.birthFraction==0&&f.frames==8&&f.spread&&
        f.velocity==std::array<float,3>{{0,70,0}}&&f.minRange==std::array<float,3>{{-6.5f,0,-6.5f}}&&
        f.maxRange==std::array<float,3>{{6.5f,4,6.5f}}&&f.flags==0x3405u&&f.mirrorX&&f.mirrorY;
}
std::array<float,3> attachedPosition(const BodyMatrix48G& m,const std::array<float,3>& offset) noexcept {
    std::array<float,3> p{};
    for(unsigned lane=0;lane<3;++lane) {
        const float direction=add(add(mul(offset[0],m.v[lane]),mul(offset[1],m.v[4+lane])),mul(offset[2],m.v[8+lane]));
        p[lane]=add(m.v[12+lane],direction);
    }
    return p;
}
}
struct OriginalGcGeneratorFireOwner50KG::Storage {
    struct Emitter {
        std::uint32_t owner=0;
        std::array<float,3> current{},previous{},velocity{};
        float fractional224=0;
        bool firstUpdate=true,enabled=true;
    };
    OriginalGcParticleManager50KG* manager=nullptr;
    Definition definition{};
    OriginalGcFireEmissionDefinition50KG emission{};
    std::array<float,3> attachmentOffset{};
    std::array<Emitter,6> emitters{};
};
OriginalGcGeneratorFireOwner50KG::OriginalGcGeneratorFireOwner50KG() noexcept=default;
OriginalGcGeneratorFireOwner50KG::~OriginalGcGeneratorFireOwner50KG()=default;
bool OriginalGcGeneratorFireOwner50KG::prepared() const noexcept{return bool(storage_);}
OriginalGcParticleStatus50KG OriginalGcGeneratorFireOwner50KG::prepare(OriginalGcParticleManager50KG& manager,
    const std::vector<std::uint8_t>& bytes) noexcept {
    if(storage_)return Status::alreadyPrepared;
    if(!manager.prepared()||manager.coldFogPopulated())return Status::unprepared;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    try {
        std::vector<Section> doc;
        if(!readOriginalGcParticleText50KG(bytes,792,
           "9964592e987a2a550e87b81eb278b4222752ad398372e3e9623c49dd6ae681ec",doc))return Status::assetIdentity;
        auto s=std::make_unique<Storage>();if(!read(doc,s->definition))return Status::unsupported;
        const auto* g=manager.group(4);const auto* offset=manager.generatorFireAttachmentOffset();
        if(!g||g->type!=8||g->first!=52||g->last!=75||!offset)return Status::authoredData;
        s->manager=&manager;s->attachmentOffset=*offset;
        const auto& f=s->definition;auto& emission=s->emission;
        emission.particleGroup=f.group;emission.firstFrame=g->first;emission.lastFrame=g->last;
        emission.particleLife=f.life;emission.initialSize=f.size;emission.growth=f.growth;
        emission.emitRate=f.rate;emission.framesPerSet=f.frames;emission.minRange=f.minRange;
        emission.maxRange=f.maxRange;emission.authoredVelocityY220=f.velocity[1];
        emission.particleFlags=f.flags;emission.randomMirrorX=f.mirrorX;
        emission.randomMirrorY=f.mirrorY;emission.applySpreadToVelocity=f.spread;
        OriginalGcParticleEmitterState50KG common{};common.particleGroup=4;
        common.framesPerSet=s->definition.frames;common.inverseFramesPerSet=div(1,common.framesPerSet);
        for(auto& e:s->emitters) {e.owner=manager.registerEmitter(common);if(!e.owner)return Status::allocation;}
        // Actual15A618/61C constructor origin remains until common update:
        //27DA20 skips unattached double-set15ACB0 for these attached sources.
        storage_=std::move(s);return Status::complete;
    }catch(...){return Status::allocation;}
}
OriginalGcFireReceipt50KG OriginalGcGeneratorFireOwner50KG::update(float dt,bool paused,
    const std::array<OriginalGcFireAttachment50KG,6>& attachments,std::uint16_t frameStamp,
    std::uint32_t& raw,std::uint32_t& serial,bool allowCarry) noexcept {
    OriginalGcFireReceipt50KG result{};if(!storage_)return result;
    if(paused){result.status=Status::complete;return result;}
    if(std::fegetround()!=FE_TONEAREST){result.status=Status::arithmetic;return result;}
    if(!scalar(dt)||dt<=0||dt>1){result.status=Status::delta;return result;}
    if(&raw==&serial){result.status=Status::storage;return result;}
    auto& s=*storage_;
    for(std::size_t index=0;index<s.emitters.size();++index) {
        auto& e=s.emitters[index];const auto& attachment=attachments[index];
        //15A7D0 first-update bit returns before the attachment is sampled.
        if(e.firstUpdate){e.firstUpdate=false;e.previous=e.current;++result.firstUpdatesSkipped;continue;}
        for(float v:attachment.actualWorldMatrix.v)if(!scalar(v)){result.status=Status::arithmetic;return result;}
        e.previous=e.current;e.current=attachedPosition(attachment.actualWorldMatrix,s.attachmentOffset);
        if(!e.enabled){e.previous=e.current;continue;}
        const float inverseDt=div(1,dt);
        for(unsigned lane=0;lane<3;++lane)e.velocity[lane]=mul(sub(e.current[lane],e.previous[lane]),inverseDt);
        if(attachment.renderOwnerPresent&&attachment.renderStamp!=frameStamp){++result.renderGateSkips;continue;}
        const auto emitted=emitOriginalGcFireParticles50KG(s.emission,*s.manager,e.owner,
            e.fractional224,e.previous,0x3f800000u,e.velocity,dt,raw,serial,allowCarry);
        result.spawned+=emitted.spawned;result.randomDraws+=emitted.randomDraws;
        result.poolFull=result.poolFull||emitted.poolFull;
        result.nativeUndefinedCarryPolicy=result.nativeUndefinedCarryPolicy||emitted.nativeUndefinedCarryPolicy;
        if(!emitted.okay()){
            using S=OriginalGcSprayStatus50KG;
            switch(emitted.status){
            case S::unprepared:result.status=Status::unprepared;break;
            case S::authoredData:result.status=Status::authoredData;break;
            case S::arithmetic:result.status=Status::arithmetic;break;
            case S::storage:result.status=Status::storage;break;
            case S::capacity:result.status=Status::capacity;break;
            default:result.status=Status::unsupported;break;
            }
            return result;
        }
    }
    result.status=Status::complete;return result;
}
} // namespace rr::game
