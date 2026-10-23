#include "original_ui_garage_particles50kg.h"
#include "original_racer_model_scene50la.h"
#include "original_gc_fire_emission50kg.h"
// The existing shared smoke template predates this leaf's strict warning lane.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#include "original_smoke_numeric50rc.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include "water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>
namespace rr::game { namespace {
using S=OriginalGcSprayStatus50KG;
using D=assets::OriginalUiPageDiagnostic50KG;
using E=assets::OriginalUiPageError50KG;
using Section=assets::OriginalUiDatSection50KG;
using Definition=OriginalUiGarageEmitterDefinition50KG;
using Receipt=OriginalGcSprayReceipt50KG;
using Math=water_wake_detail50jc::Math;
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
std::uint32_t word(float f) noexcept {std::uint32_t u;std::memcpy(&u,&f,4);return u;}
Receipt result(S s,std::uint32_t pc) noexcept {Receipt r;r.status=s;r.sourcePC=pc;return r;}
OriginalUiGarageParticleVisit50KG visit(S s,std::uint32_t pc,std::uint32_t i=0) noexcept {
    OriginalUiGarageParticleVisit50KG r;r.status=s;r.sourcePC=pc;r.emitterOrdinal=i;return r;
}
char lower(char c) noexcept{return c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const char* b) noexcept {
    const auto n=std::strlen(b);if(a.size()!=n)return false;
    for(std::size_t i=0;i<n;++i) { if(lower(a[i])!=lower(b[i]))return false; }
    return true;
}
bool space(char c) noexcept{return c==' '||c=='\t'||c=='\r';}
// Bounded source159440 cleaning and158648 first-present lookup. Unknown keys
// and duplicates remain in source order; quoted empty differs from missing.
// This is text authoring only, not a BPB/archive decoder or runtime interpreter.
bool sections(const std::vector<std::uint8_t>& bytes,std::vector<Section>& out,D& d) {
    auto fail=[&](E e,std::size_t at){d={e,at,0x159440u};return false;};
    if(bytes.empty()||bytes.size()>32768)return fail(E::capacity,0);
    auto size=bytes.size();if(bytes.back()==0)--size;
    for(std::size_t i=0;i<size;++i)if((bytes[i]<32&&bytes[i]!=9&&bytes[i]!=10&&bytes[i]!=13)||bytes[i]>126)
        return fail(E::unsupportedEncoding,i);
    for(std::size_t at=0;at<size;){
        const auto start=at;while(at<size&&bytes[at]!='\n')++at;auto end=at;
        if(at<size){++at;if(end>start&&bytes[end-1]=='\r')--end;}
        if(end-start>2048)return fail(E::capacity,start);
        auto first=start;while(first<end&&space(char(bytes[first])))++first;
        if(first==end||bytes[first]==';'||bytes[first]==':')continue;
        // Colon-prefixed EMT annotations cannot match any consumed source key.
        //158724/158800 skips a line without an '=' value; colon key/value
        // annotations also fail the requested-key comparison at15880C.
        const bool header=bytes[first]=='[';bool quoted=false;int brackets=0;
        std::string line;std::vector<std::size_t> offsets;
        for(auto i=start;i<end;++i){const char c=char(bytes[i]);
            if(header&&c==';'&&!quoted&&brackets==0&&!line.empty()&&line.back()==']')break;
            if(c=='"')quoted=!quoted;else if(c=='[')++brackets;else if(c==']')--brackets;
            if(brackets<0||brackets>16)return fail(E::textSyntax,i);
            if(space(c)&&!quoted&&brackets==0)continue;
            line.push_back(c);offsets.push_back(i);
        }
        if(line.empty()||line[0]==';')continue;
        if(brackets)return fail(E::textSyntax,start);
        if(line[0]=='['){
            if(line.size()<3||line.back()!=']')return fail(E::textSyntax,start);
            auto name=line.substr(1,line.size()-2);
            if(name.size()>127||name.find_first_of("[]=\"")!=std::string::npos)return fail(E::textSyntax,start);
            for(const auto& s:out)if(same(s.name,name.c_str()))return fail(E::duplicateSection,start);
            if(out.size()==64)return fail(E::capacity,start);
            out.push_back({std::move(name),offsets[1],{}});continue;
        }
        const auto equal=line.find('=');
        if(equal==std::string::npos)continue; //158800: no value token.
        if(out.empty())continue; //No current requested source block yet.
        if(!equal||equal>127)return fail(E::textSyntax,start);
        auto key=line.substr(0,equal);if(key.find_first_of("[];\"")!=std::string::npos)return fail(E::textSyntax,start);
        auto va=equal+1,vb=line.size();const bool present=va<vb;
        if(va<vb&&line[va]=='"')++va;
        if(va<vb&&line[vb-1]=='"')--vb;
        if(out.back().values.size()==256)return fail(E::capacity,start);
        const auto valueOffset=va<offsets.size()?offsets[va]:offsets[equal]+1;
        out.back().values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
    }
    return !out.empty()||fail(E::textSyntax,0);
}
const Section& section(const std::vector<Section>& doc,const char* name,const Section& absent) noexcept {
    for(const auto& s:doc) { if(same(s.name,name))return s; }
    return absent;
}

S parseDefinition(const std::vector<std::uint8_t>& input,Definition& f,D& d){
        std::vector<Section> doc;if(!sections(input,doc,d))return S::authoredData;
        const Section absent{};
        const auto& general=section(doc,"General Info",absent);
        const auto& properties=section(doc,"Properties",absent);
        const auto& animation=section(doc,"Animation",absent);
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(general,key,{},value,d,pc);};
        //15C34C calls common15AEA0 before smoke-derived fields.
        if(!i(general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
           !n(general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
           !n(general,"InitialSize",1,f.initialSize184,0x15af8c)||
           !n(general,"Growth",0,f.growth180,0x15afa4)||
           !n(general,"EmitRate",500,f.emitRate188,0x15afc0)||
           !n(general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
           !v("Velocity",f.velocity60,0x15b004)||
           !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
           !n(general,"Restitution",bits(0x3f0ccccdu),f.restitution,0x15b0a0))return S::authoredData;
        const auto timing=readOriginalGcCommonEmitterTiming50KG(input,f.timing,d);
        if(timing!=S::complete)return timing;
        Math m;f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;
            if(value)f.particleFlags198|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return S::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200u;
        // The unused1/0 at15B4B0 has no consumer for the actual FramesPerSet0
        // members. Its flag400 is clear. Preserve that inactive admission.
        f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.particleFlags198|=0x400u;
        std::int32_t forced=0;
        if(!i(animation,"NumberOfForcedSets",0,forced,0x15b4c0))return S::authoredData;
        if(forced>0){d.sourcePC=0x15b508;return S::unsupported;}
        if(f.type==0){ //Actual15C330 smoke reader, not the pod subtype.
        if(!n(general,"MinimumPercentLife",.5f,f.minimumPercentLife21C,0x15c378)||
           !n(general,"MinimumPercentLifeConvergance",1,f.minimumPercentLifeConvergance220,0x15c438)||
           !n(general,"MinimumInitialSize",0,f.minimumInitialSize208,0x15c4d8)||
           !n(general,"MaximumInitialSize",5,f.maximumInitialSize20C,0x15c57c)||
           !n(general,"InitialSizeToSpeedRatio",bits(0x3c23d70au),f.initialSizeToSpeedRatio210,0x15c624)||
           !n(general,"MinimumLife",500,f.minimumLife214,0x15c6c8)||
           !n(general,"MaximumLife",3500,f.maximumLife218,0x15c770)||
           !n(general,"LifeToSpeedRatio",5,f.lifeToSpeedRatio224,0x15c814)||
           !n(general,"MinimumEmitRate",0,f.minimumEmitRate22C,0x15c8b4)||
           !n(general,"MaximumEmitRate",10,f.maximumEmitRate230,0x15c958)||
           !n(general,"ParticlesToDistanceRatio",bits(0x3c23d70au),f.particlesToDistanceRatio234,0x15ca00)||
           !n(general,"ParticlesToSpeedRatio",0,f.particlesToSpeedRatio238,0x15caa0)||
           !n(general,"InitialPercentAge",0,f.initialPercentAge228,0x15cb40)||
           !n(general,"MinimumEmissionSpeed",0,f.minimumEmissionSpeed23C,0x15cbe0)||
           !v("MinimumVelocityNoise",f.minimumVelocityNoise240,0x15cc84)||
           !v("MaximumVelocityNoise",f.maximumVelocityNoise250,0x15cca0))return S::authoredData;
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        }

        if(f.type==8||f.type==9||f.type==11){
            if(!v("MinRange",f.minRange,f.type==11?0x15fd14:(f.type==8?0x15e710:0x15efb0))||
               !v("MaxRange",f.maxRange,f.type==11?0x15fd30:(f.type==8?0x15e72c:0x15efcc)))return S::authoredData;
        }
        if(f.type==8||f.type==9){
            if(!assets::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,
                f.applySpreadToVelocity,d,f.type==8?0x15e744:0x15efe4))return S::authoredData;
            f.spreadVelocityScale220=f.velocity60[1];
        }
        if(f.type==11){
            if(!n(general,"MinStreamLength",3,f.minStreamLength,0x15fc50)||
               !n(general,"MaxStreamLength",5,f.maxStreamLength,0x15fc6c)||
               !n(general,"TailThickness",1,f.tailThickness,0x15fc88)||
               !n(general,"HeadThickness",2,f.headThickness,0x15fca4)||
               !n(general,"ParticleSeparationFactor",1.5f,f.separation,0x15fcc0)||
               !n(general,"TailLifeModifier",bits(0x3f19999a),f.tailLife,0x15fce0)||
               !assets::originalUiBoolean50KG(general,"SuperbrightHead",true,f.superbrightHead,d,0x15fcf8)||
               !assets::originalUiBoolean50KG(general,"MovingEmitter",false,f.moving,d,0x15fd48))return S::authoredData;
            // Every actual Garage type11 has null74 and MovingEmitter=false.
            // Do not impersonate the separate moving pod/wreck identity.
            if(f.moving)return S::unsupported;
        }
        f.parsed=true;return S::complete;
}
} //namespace
S OriginalUiGarageParticleAssets50KG::load(
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> scene,D& d) noexcept {
    d={};if(loaded())return S::alreadyPrepared;
    if(!scene||!scene->loaded()||!scene->archive()||!scene->toyDat())return S::unprepared;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try{
        std::array<Definition,7> next{};
        const auto* toy=scene->toyDat();const auto* header=toy->section("ParticleEmitters");
        std::int32_t count=0;
        if(!header||!assets::originalUiInteger50KG(*header,"NumberOfEmitters",0,count,d,0x27d3a0)||count!=7)
            return S::authoredData;
        constexpr const char* names[]={"garageSparks.emt","pd_fire.emt","pd_smoke.emt","garageExplosion.emt","garageExplSparks.emt","pd_ext.emt","pd_fall.emt"};
        constexpr int types[]={11,8,0,9,11,0,11};
        for(unsigned k=0;k<7;++k){
            auto& f=next[k];const auto* row=toy->section("ParticleEmitter"+std::to_string(k+1));
            if(!row)return S::authoredData;
            const auto* name=row->find("EmitterFilename");if(!name||!same(name->text,names[k]))return S::assetIdentity;
            f.filename=name->text;f.source=scene->archive()->find("particle/"+f.filename);
            if(!f.source.data||!f.source.identity)return S::assetIdentity;
            std::int32_t instances=0;
            if(!assets::originalUiInteger50KG(*row,"TypeID",0,f.type,d,0x27d450)||f.type!=types[k]||
               !assets::originalUiInteger50KG(*row,"StartingZone",0,f.zone,d,0x27d490)||f.zone!=1||
               !assets::originalUiInteger50KG(*row,"NumberOfInstances",0,instances,d,0x27d660)||instances!=1||
               !assets::originalUiBoolean50KG(*row,"StartOn",true,f.startOn,d,0x27d900)||
               !assets::originalUiBoolean50KG(*row,"Additive",false,f.additiveToy,d,0x27d930)||
               !assets::originalUiTriple50KG(*row,"Position",{},f.position,d,0x27d700)||
               !assets::originalUiTriple50KG(*row,"OffsetFromSoultree",{},f.offset,d,0x27d580))return S::authoredData;
            if(const auto* a=row->find("AttachToSoultree"))f.attachMcf=a->text;
            if(const auto* a=row->find("ChildObject"))f.childName=a->text;
            if(f.startOn!=(k==0)||((k==1||k==2)?!same(f.attachMcf,"pd_fire.mcf"):
               (k==5?!same(f.attachMcf,"nozzle.mcf"):!f.attachMcf.empty())))return S::authoredData;
            if((k==1||k==2)?!same(f.childName,"Plane01"):!f.childName.empty())return S::authoredData;
            std::vector<std::uint8_t> bytes(f.source.data,f.source.data+f.source.size);
            const auto status=parseDefinition(bytes,f,d);if(status!=S::complete)return status;
        }
        const auto atlas=scene->archive()->find("particle/garageprt.prt");
        if(!atlas.data||!atlas.identity)return S::assetIdentity;
        definitions_=std::move(next);atlas_=atlas;scene_=std::move(scene);return S::complete;
    }catch(...){return S::allocation;}
}
const Definition* OriginalUiGarageParticleAssets50KG::emitter(OriginalUiGarageEmitter50KG id) const noexcept {
    const auto i=static_cast<unsigned>(id);return loaded()&&i<7?&definitions_[i]:nullptr;
}

namespace {
struct Runtime {
    EffectControlState50BC controls{};
    const Definition* definition=nullptr;
    Definition liveDefinition; // Actual reached15AEA0/derived reader result.
    // Authentic15A6D8 and derived15DE40/15F13C writes. No caller-provided
    // false visibility flag stands in for the actual source-null19C field.
    std::uint32_t sourceField19C=0; //15A6D8/15DE40 own field, NOT node observer.
    const OriginalRacerSceneObject50LA* sourceAttachment74=nullptr;
    bool sourceConstructed=false,sourceRead=false;
    bool sourceActivated=false,sourceFinished=false,sourceRetired=false;
    OriginalGcCommonAttachment50KG attachment{};
    OriginalUiGarageEmitterRenderBinding50KG render{};
    float delay1DC=0,fractional224=0,remainder200=0,carriedSize18C=0;
    std::uint32_t duration1E4=0,managerOwnerId=0;
    std::int32_t currentFrame190=0,firstFrame1B0=0,lastFrame1B4=0;
    bool enabled=false,attachmentBound=false,failed=false;
    Receipt stoppedAt{};
};
bool start(void*,EffectControlState50BC& c) noexcept {startEffect50BC(c);return true;}
bool stop(void* p,EffectControlState50BC& c) noexcept {
    const auto& r=*static_cast<const Runtime*>(p);
    //15F070 explosion stop is protected while its active one-shot remains.
    if(r.definition->type==9&&r.definition->timing.mode1D8==1&&(c.flags1D4&2u)&&bits(r.duration1E4)>0)return true;
    stopEffect50BC(c);return true;
}
Receipt reset(Runtime& r,std::uint32_t& raw,bool cold) noexcept {
    auto result=::rr::game::result(S::complete,cold?0x15b1c4:0x15ade0);
    const auto& t=r.definition->timing;r.delay1DC=t.delayTime1E0;
    if(t.mode1D8==1||t.mode1D8==3)r.duration1E4=word(t.maxTimeOn1EC);
    else if(t.mode1D8==2){
        const auto random=drawOriginalGcParticleRandom50KG(raw);++result.randomDraws;
        Math m;const auto duration=m.add(t.minTimeOn1E8,m.mul(m.sub(t.maxTimeOn1EC,t.minTimeOn1E8,
            cold?0x15b170:0x15adb8),random,cold?0x15b174:0x15adbc),cold?0x15b178:0x15adc0);
        if(m.badPC){result.status=S::arithmetic;result.sourcePC=m.badPC;return result;}
        r.duration1E4=word(duration);
    }
    return result;
}
Receipt explosion(Runtime& state,const Definition& f,OriginalGcParticleManager50KG& manager,
    float dt,std::uint32_t& raw,OriginalWreckExplosionGlobals50LA& globals) noexcept {
    //15EB08 beq attachment74==null ->15EB54 is the authentic Garage branch.
    auto r=result(S::complete,0x15ef48);Math m;
    auto fail=[&](S status,std::uint32_t pc){
        r.status=status;r.sourcePC=pc;state.failed=true;state.stoppedAt=r;return r;};
    auto arithmetic=[&](){return fail(S::arithmetic,m.badPC);};
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x15eb54);
    const auto demand=m.add(.5f,m.mul(f.emitRate188,dt,0x15eb64),0x15eb68);
    auto count=m.cvtWord(demand,0x15eb6c);
    if(m.badPC)return arithmetic();
    if(count<=0)count=1; //15EB78..80 even for a zero emission product.
    if(!globals.lazy84CC){globals.emissionCounter433978=0;globals.lazy84CC=true;}
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    const auto& c=state.controls;
    for(std::int32_t index=0;index<count;++index){
        auto* p=manager.nextParticle();
        if(!p){r.poolFull=true;r.sourcePC=0x15ef28;return r;}
        ++globals.emissionCounter433978; //15EBC4..15EBD8, shared process counter.
        const float randomX=draw(),randomY=draw(),randomZ=draw();
        const auto spreadX=m.add(f.minRange[0],m.mul(randomX,
            m.sub(f.maxRange[0],f.minRange[0],0x15ec64),0x15ec68),0x15ec6c);
        const auto spreadZ=m.add(f.minRange[2],m.mul(randomZ,
            m.sub(f.maxRange[2],f.minRange[2],0x15ec80),0x15ec84),0x15ec88);
        if(m.badPC)return arithmetic();
        const std::int32_t set=randomZ<=.5f?0:1; //C.OLE.S15EC50, not a fresh draw.
        // The source copies previous40, preserving Y/W; range Y is not applied.
        std::memcpy(p->position.data(),c.previous40,12);
        std::memcpy(&p->positionW,c.previous40+3,4);
        p->position[0]=m.add(p->position[0],spreadX,0x15ed78);
        if(m.badPC)return arithmetic();
        p->position[2]=m.add(p->position[2],spreadZ,0x15ed88);
        if(m.badPC)return arithmetic();
        p->size=p->initialSize=f.initialSize184;p->ownerId=state.managerOwnerId;
        const auto lifeStep=m.integer(static_cast<std::int32_t>(globals.emissionCounter433978&3u),0x15edb8);
        const auto lifeReduction=m.mul(.125f,m.mul(f.particleLife17C,lifeStep,0x15edbc),0x15edc0);
        p->lifeMilliseconds=m.sub(f.particleLife17C,lifeReduction,0x15edc4);
        if(m.badPC)return arithmetic();
        p->ageMilliseconds=f.ageAtBirth1A0;
        const auto frame=m.add(m.integer(state.firstFrame1B0,0x15eddc),
            m.mul(m.integer(set,0x15ed40),f.framesPerSet,0x15ede0),0x15ede4);
        p->frame=m.cvtWord(frame,0x15ede8);
        if(m.badPC)return arithmetic();
        p->inverseLife=m.div(1,p->lifeMilliseconds,0x15edfc);
        if(m.badPC)return arithmetic();
        p->growth=f.growth180;
        //15EE18's conditional zero-growth flag clear is overwritten at15EE44.
        p->flags=f.particleFlags198;
        if(f.randomMirrorX&&randomX>.5f)p->flags|=0x4000;else p->flags&=~0x4000u;
        if(f.randomMirrorY&&randomY>.5f)p->flags|=0x8000;else p->flags&=~0x8000u;
        std::memcpy(p->velocity.data(),c.velocity60,12);
        std::memcpy(&p->velocityW,c.velocity60+3,4);
        if(f.applySpreadToVelocity){
            p->velocity[1]=m.add(p->velocity[1],m.mul(f.spreadVelocityScale220,randomY,0x15ef00),0x15ef04);
            if(m.badPC)return arithmetic();
        }
        if(!manager.commitParticle(state.managerOwnerId))return fail(S::storage,0x15ef20);
        ++r.spawned;
    }
    // No derived reset of previous40,19C,18C or the one-shot duration exists.
    return r;
}
Receipt sparks(Runtime& state,const Definition& f,OriginalGcParticleManager50KG& manager,
    float dt,std::uint32_t& raw) noexcept {
    // All three Garage type11 emitters have source-null74 and moving=false.
    auto r=result(S::complete,0x15fbc8);Math m;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;state.failed=true;state.stoppedAt=r;return r;};
    auto arithmetic=[&](){return fail(S::arithmetic,m.badPC);};
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x15f3c0);
    m.read(dt,0x15f450);if(m.badPC)return arithmetic();
    if(!(dt>0)||dt>1.f)return fail(S::unsupported,0x15f450);
    auto& c=state.controls;
    const auto timer=m.read(bits(c.word19C),0x15f3c0);if(m.badPC)return arithmetic();
    if(timer>.25f){std::memcpy(c.previous40,c.position30,16);return r;}
    const float emission=m.mul(f.emitRate188,dt,0x15f450);
    if(m.badPC)return arithmetic();
    if(emission<0||emission>=2147483648.f)return fail(S::unsupported,0x15fba4);
    float velocity[3],position[3];for(unsigned i=0;i<3;++i){
        velocity[i]=m.read(bits(c.velocity60[i]),0x15f454u+4u*i);
        position[i]=m.read(bits(c.position30[i]),0x15f5c8u+8u*i);}
    if(m.badPC)return arithmetic();
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    auto finish=[&](){state.carriedSize18C=f.initialSize184;c.word19C=0;return r;};
    // Source compares CVT.S(s0) directly to rate*dt: positive fractional
    // demand runs one last stream; do not truncate it as a particle count.
    for(std::int32_t stream=0;m.integer(stream,0x15fba0)<emission;++stream){
        const float randomX=draw(),randomY=draw(),randomZ=draw();
        const float random[3]{randomX,randomY,randomZ};float spread[3];
        for(unsigned axis=0;axis<3;++axis)spread[axis]=m.add(f.minRange[axis],
            m.mul(random[axis],m.sub(f.maxRange[axis],f.minRange[axis],0x15f550u+28u*axis),
                0x15f554u+28u*axis),0x15f558u+28u*axis);
        float point[3],particleVelocity[3];
        for(unsigned axis=0;axis<3;++axis){point[axis]=m.sub(position[axis],m.mul(dt,velocity[axis],
            0x15f5b8u+4u*axis),0x15f5e4u+4u*axis);particleVelocity[axis]=m.add(velocity[axis],spread[axis],0x15f6a8u+4u*axis);}
        const float growth=m.mul(f.growth180,draw(),0x15f630);
        const float life=m.mul(f.particleLife17C,draw(),0x15f67c);
        (void)m.div(1,life,0x15f694); // Source computes before the first capacity guard.
        std::uint32_t flags=f.particleFlags198;
        if(f.randomMirrorX&&randomX>.5f)flags|=0x4000;else flags&=~0x4000u;
        if(f.randomMirrorY&&randomY>.5f)flags|=0x8000;else flags&=~0x8000u;
        const auto squared=m.add(m.add(m.mul(particleVelocity[0],particleVelocity[0],0x15f780),
            m.mul(particleVelocity[1],particleVelocity[1],0x15f784),0x15f788),
            m.mul(particleVelocity[2],particleVelocity[2],0x15f78c),0x15f78c);
        float step[3]{};
        if(squared!=0){const auto inverse=m.div(1,original_smoke_numeric_detail50RC::squareRoot(squared,m,0x15f7bc),0x15f7bc);
            for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(particleVelocity[axis],inverse,0x15f7ccu+12u*axis);}
        const auto separation=m.mul(f.separation,f.initialSize184,0x15f7fc);
        for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(step[axis],separation,0x15f800u+12u*axis);
        const float lengthRandom=draw();float length;
        length=m.add(.5f,m.add(f.minStreamLength,m.mul(m.sub(f.maxStreamLength,f.minStreamLength,
            0x15f8bc),lengthRandom,0x15f8c0),0x15f8c4),0x15f8c8);
        const auto sections=m.cvtWord(length,0x15f880);
        const auto sectionFloat=m.integer(sections,0x15f8e0);
        if(m.badPC)return arithmetic();
        if(sections<=0)return fail(S::unsupported,0x15f91c);
        const auto reciprocal=m.div(1,sectionFloat,0x15f91c);
        for(unsigned axis=0;axis<3;++axis)particleVelocity[axis]=m.add(particleVelocity[axis],
            m.mul(sectionFloat,step[axis],0x15f8f8u+4u*axis),0x15f928u+12u*axis);
        for(std::int32_t sectionIndex=0;sectionIndex<sections;++sectionIndex){
            const auto thickness=m.add(.5f,m.add(f.tailThickness,m.mul(reciprocal,m.mul(
                m.integer(sectionIndex,0x15f95c),m.sub(f.headThickness,f.tailThickness,0x15f960),0x15f964),
                0x15f968),0x15f96c),0x15f978);
            auto copies=m.cvtWord(thickness,0x15f97c);
            if(sectionIndex==sections-1&&f.superbrightHead){
                if(copies==std::numeric_limits<std::int32_t>::max())return fail(S::unsupported,0x15f99c);
                ++copies;
            }
            const auto ratio=m.div(m.integer(sectionIndex+1,0x15f9bc),sectionFloat,0x15f9c0);
            const float taper=std::max(bits(0x3dcccccdu),ratio); //MAX.S15F9CC, not POW.
            const auto complement=m.sub(1,taper,0x15f9f4);
            if(m.badPC)return arithmetic();
            for(std::int32_t copy=0;copy<copies;++copy){
                auto* p=manager.nextParticle();if(!p){r.poolFull=true;return finish();}
                p->ageMilliseconds=0;p->position={point[0],point[1],point[2]};p->positionW=1;
                p->frame=state.currentFrame190;p->growth=growth;p->size=p->initialSize=f.initialSize184;p->ownerId=state.managerOwnerId;
                //15FA48's zero-growth flag clear is overwritten unconditionally
                // by15FA9C. Do not propagate stale free-slot flags into a particle.
                p->lifeMilliseconds=m.sub(life,m.mul(complement,m.mul(f.tailLife,life,0x15fa74),0x15fa78),0x15fa7c);
                p->inverseLife=m.div(1,p->lifeMilliseconds,0x15fa88);
                p->velocity={particleVelocity[0],particleVelocity[1],particleVelocity[2]};p->velocityW=1;p->flags=flags;
                if(m.badPC)return arithmetic();
                if(!manager.commitParticle(state.managerOwnerId))return fail(S::storage,0x15fab0);
                ++r.spawned;
            }
            const auto taperSquared=m.mul(taper,taper,0x15fae0);
            particleVelocity[0]=m.sub(particleVelocity[0],m.mul(step[0],taperSquared,0x15faf4),0x15faf8);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(complement,step[1],0x15fb04),0x15fb08);
            particleVelocity[2]=m.sub(particleVelocity[2],m.mul(step[2],taperSquared,0x15fb14),0x15fb18);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(step[1],taper,0x15fb24),0x15fb28);
            for(unsigned axis=0;axis<3;++axis)point[axis]=m.add(point[axis],step[axis],0x15fb34u+12u*axis);
            if(m.badPC)return arithmetic();
        }
        if(state.currentFrame190==std::numeric_limits<std::int32_t>::max())return fail(S::unsupported,0x15fb68);
        ++state.currentFrame190;if(state.currentFrame190>state.lastFrame1B4)state.currentFrame190=state.firstFrame1B0;
    }
    if(m.badPC)return arithmetic();
    return finish();
}
OriginalUiGarageParticleVisit50KG fireDefinedCount(
    const OriginalGcFireEmissionDefinition50KG& f,OriginalGcParticleManager50KG& manager,
    std::uint32_t owner,float& fractional,const std::array<float,3>& previous,
    std::uint32_t previousW,const std::array<float,3>& velocity,float dt,
    std::uint32_t& raw,std::uint32_t& serial,bool& lazy84C8) noexcept {
    static_assert(originalGcParticleCapacity50KG==5000,"source Garage capacity proof");
    using S=OriginalGcSprayStatus50KG;using Math=water_wake_detail50jc::Math;
    OriginalUiGarageParticleVisit50KG r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    const auto* common=manager.emitter(owner);const auto* group=manager.group(f.particleGroup);
    if(!manager.prepared()||!owner||!common||!group)return fail(S::unprepared,0x15e2d0);
    if(common->particleGroup!=f.particleGroup||common->framesPerSet!=f.framesPerSet||
       group->first!=f.firstFrame||group->last!=f.lastFrame)return fail(S::storage,0x15e534);
    if(&raw==&serial)return fail(S::storage,0x15e2e8);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x15e1d0);
    Math m;auto failMath=[&](){return fail(S::arithmetic,m.badPC);};
    m.read(dt,0x15e238);m.read(fractional,0x15e260);
    if(m.badPC)return failMath();
    // Native arithmetic admission, not a replacement/clamp for caller time.
    if(dt<=0||dt>1||fractional<0||f.emitRate<0||f.particleLife<=0||f.framesPerSet<=0)
        return fail(S::unsupported,0x15e238);
    const auto xx=m.mul(velocity[0],velocity[0],0x15e1e0);
    const auto yy=m.mul(velocity[1],velocity[1],0x15e1e4);
    const auto xy=m.add(xx,yy,0x15e1e8);
    const auto square=m.add(xy,m.mul(velocity[2],velocity[2],0x15e1ec),0x15e1ec);
    if(m.badPC)return failMath();
    volatile float speed=std::sqrt(square);m.read(speed,0x15e1f0);
    const auto birthFactor=std::min(1.f,m.div(speed,650.f,0x15e214));
    const auto emission=m.mul(.6f,m.mul(f.emitRate,dt,0x15e238),0x15e23c);
    if(m.badPC)return failMath();
    std::int32_t requested=0;bool unknownInheritedS1=false;
    if(emission>1){requested=m.cvtWord(emission,0x15e250);if(m.badPC)return failMath();fractional=0;}
    else {
        const auto accumulated=m.add(fractional,emission,0x15e26c);
        if(m.badPC)return failMath();
        fractional=accumulated;
        if(accumulated==0)requested=0;
        else if(accumulated>1){requested=m.cvtWord(accumulated,0x15e29c);if(m.badPC)return failMath();fractional=0;}
        else {
            // Garage lists are page children, not the 1B31F0 scene path.
            // Preserve the original +224 write but do not invent the live
            // caller s1 used as a signed loop bound on this branch.
            r.callerCarryUnavailable=true;unknownInheritedS1=true;
        }
    }
    //15E2A8 lazy shared process serial occurs after carry calculation.
    if(!lazy84C8){serial=0;lazy84C8=true;}
    //15E294 branches over the count assignment to15E2A8. Lazy globals
    //are fully determined;15E2CC SLT is the first read of inherited s1.
    if(unknownInheritedS1)return fail(S::unsupported,0x15e2cc);
    r.status=S::complete;r.sourcePC=0x15e6a8;
    for(std::int32_t n=0;n<requested;++n){
        //15E2D0..E0 checks capacity before serial, RNG, or particle writes.
        auto* p=manager.nextParticle();
        if(!p){r.poolFull=true;r.sourcePC=0x15e688;return r;}
        ++serial;
        const auto x=drawOriginalGcParticleRandom50KG(raw);
        const auto y=drawOriginalGcParticleRandom50KG(raw);
        const auto z=drawOriginalGcParticleRandom50KG(raw);r.randomDraws+=3;
        const auto spreadX=m.add(f.minRange[0],m.mul(x,m.sub(f.maxRange[0],f.minRange[0],0x15e388),0x15e38c),0x15e390);
        const auto spreadZ=m.add(f.minRange[2],m.mul(z,m.sub(f.maxRange[2],f.minRange[2],0x15e3a4),0x15e3a8),0x15e3ac);
        if(m.badPC)return failMath();
        std::int32_t set=0;if(z>.33f)set=1;if(z>.66f)set=2;
        // Authenticated callers prove forced-set pointer1C4 null. A generic
        // forced-set branch is intentionally not fabricated by this kernel.
        p->position=previous;std::memcpy(&p->positionW,&previousW,4);
        p->position[0]=m.add(p->position[0],spreadX,0x15e4d0);
        if(m.badPC)return failMath();
        p->position[2]=m.add(p->position[2],spreadZ,0x15e4e0);
        if(m.badPC)return failMath();
        p->size=f.initialSize;p->initialSize=f.initialSize;p->ownerId=owner;
        const auto quarter=m.integer(static_cast<std::int32_t>(serial&3u),0x15e510);
        p->lifeMilliseconds=m.sub(f.particleLife,m.mul(.125f,m.mul(f.particleLife,quarter,0x15e514),0x15e518),0x15e51c);
        if(m.badPC)return failMath();
        p->ageMilliseconds=m.mul(m.mul(.5f,p->lifeMilliseconds,0x15e528),birthFactor,0x15e52c);
        if(m.badPC)return failMath();
        const auto frame=m.add(m.integer(f.firstFrame,0x15e53c),m.mul(m.integer(set,0x15e494),f.framesPerSet,0x15e540),0x15e544);
        const auto selected=m.cvtWord(frame,0x15e548);if(m.badPC)return failMath();
        if(selected<f.firstFrame||selected>f.lastFrame)return fail(S::authoredData,0x15e554);
        p->frame=selected;p->inverseLife=m.div(1,p->lifeMilliseconds,0x15e55c);
        if(m.badPC)return failMath();
        p->growth=f.growth;
        if(f.growth==0)p->flags&=~8u; //15E578..584 before the following overwrite.
        p->flags=f.particleFlags;
        if(x>.5f&&f.randomMirrorX)p->flags|=0x4000u;else p->flags&=~0x4000u;
        if(y>.5f&&f.randomMirrorY)p->flags|=0x8000u;else p->flags&=~0x8000u;
        p->velocity=velocity;p->velocityW=1;
        if(f.applySpreadToVelocity){
            p->velocity[1]=m.add(p->velocity[1],m.mul(f.authoredVelocityY220,y,0x15e660),0x15e664);
            if(m.badPC)return failMath();
        }
        if(!manager.commitParticle(owner))return fail(S::storage,0x15e680);
        ++r.spawned;
    }
    return r;
}
} //namespace

struct OriginalUiGarageParticles50KG::Storage {
    std::shared_ptr<const OriginalUiGarageParticleAssets50KG> assets;
    OriginalGcParticleManager50KG manager;
    std::array<Runtime,7> rows{};
    bool prepared=false,failed=false,managerPrepared=false,emitterAttempted=false;
    std::size_t emitterCursor=0;
    OriginalUiGarageParticleVisit50KG stoppedAt{};
};
OriginalUiGarageParticles50KG::OriginalUiGarageParticles50KG() noexcept=default;
OriginalUiGarageParticles50KG::~OriginalUiGarageParticles50KG()=default;
bool OriginalUiGarageParticles50KG::prepared() const noexcept {return storage_&&storage_->prepared;}
const OriginalGcParticleManager50KG* OriginalUiGarageParticles50KG::manager() const noexcept {
    return storage_?&storage_->manager:nullptr;
}
bool OriginalUiGarageParticles50KG::managerPrepared160E00()const noexcept {
    return storage_&&storage_->managerPrepared&&!storage_->failed&&storage_->manager.prepared();
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::prepareManager160E00(
    std::shared_ptr<const OriginalUiGarageParticleAssets50KG> assets,
    const std::vector<std::uint8_t>& livePrtReadBuffer)noexcept {
    if(storage_)return storage_->failed?storage_->stoppedAt:visit(S::alreadyPrepared,0x160e00);
    if(!assets||!assets->loaded())return visit(S::unprepared,0x160e00);
    if(std::fegetround()!=FE_TONEAREST)return visit(S::arithmetic,0x160e00);
    const auto atlas=assets->atlas();
    if(!atlas.data||atlas.size!=livePrtReadBuffer.size()||livePrtReadBuffer.empty()||
       std::memcmp(atlas.data,livePrtReadBuffer.data(),atlas.size))return visit(S::authoredData,0x1617a0);
    try{
        storage_=std::make_unique<Storage>();storage_->assets=std::move(assets);
        const auto result=storage_->manager.prepareTrackAtlas160E00(livePrtReadBuffer);
        if(result!=OriginalGcParticleStatus50KG::complete){
            storage_->failed=true;storage_->stoppedAt=visit(S::authoredData,0x1617a0);return storage_->stoppedAt;
        }
        storage_->managerPrepared=true;return visit(S::complete,0x161aac);
    }catch(...){
        const auto out=visit(S::allocation,0x160e00);
        if(storage_){storage_->failed=true;storage_->stoppedAt=out;}
        return out;
    }
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::prepare(
    std::shared_ptr<const OriginalUiGarageParticleAssets50KG> assets,std::uint32_t& raw) noexcept {
    if(!assets||!assets->loaded())return visit(S::unprepared,0x27d350);
    try{
        const auto atlas=assets->atlas();
        const std::vector<std::uint8_t> bytes(atlas.data,atlas.data+atlas.size);
        const auto initial=prepareManager160E00(std::move(assets),bytes);
        if(!initial.okay())return initial;
        return prepareEmitters274370(raw);
    }catch(...){return visit(S::allocation,0x160e00);}
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::prepareEmitters274370(std::uint32_t& raw)noexcept {
    if(!managerPrepared160E00())return storage_&&storage_->failed?storage_->stoppedAt:visit(S::unprepared,0x274370);
    if(storage_->emitterAttempted)return visit(S::alreadyPrepared,0x274370);
    if(std::fegetround()!=FE_TONEAREST)return visit(S::arithmetic,0x15b148);
    storage_->emitterAttempted=true;
    try{
        auto& st=*storage_;auto out=visit(S::complete,0x27dc88);
        auto fail=[&](S s,std::uint32_t pc,unsigned k){out.status=s;out.sourcePC=pc;out.emitterOrdinal=k+1;
            st.failed=true;st.stoppedAt=out;return out;};
        for(unsigned k=0;k<7;++k){
            auto& r=st.rows[k];r.definition=st.assets->emitter(static_cast<OriginalUiGarageEmitter50KG>(k));
            const auto& f=*r.definition;r.controls.flags1D4=1;
            r.controls.position30[3]=r.controls.previous40[3]=r.controls.velocity60[3]=0x3f800000;
            r.controls.intensity204=f.type==0?0x3f800000:0;
            for(unsigned axis=0;axis<3;++axis)r.controls.velocity60[axis]=word(f.velocity60[axis]);
            if(f.attachMcf.empty()){
                std::uint32_t pos[4]{word(f.position[0]),word(f.position[1]),word(f.position[2]),0x3f800000};
                positionEffect50BC(r.controls,pos);
                positionEffect50BC(r.controls,pos); //27DA30/40 and analogous null74 branches.
            }
            const auto* group=st.manager.group(f.particleGroup1B8);
            if(!group)return fail(S::authoredData,0x15b010,k);
            OriginalGcParticleEmitterState50KG common;
            common.particleGroup=f.particleGroup1B8;common.framesPerSet=f.framesPerSet;
            common.inverseFramesPerSet=f.inverseFramesPerSet;common.gravity=f.gravityMultiplier;
            common.bouncePlane=f.groundLevel;common.bounceFactor=f.restitution;
            r.managerOwnerId=st.manager.registerEmitter(common);
            if(!r.managerOwnerId)return fail(S::allocation,0x15b010,k);
            r.firstFrame1B0=r.currentFrame190=group->first;r.lastFrame1B4=group->last;
            const auto initialized=reset(r,raw,true);out.randomDraws+=initialized.randomDraws;
            if(!initialized.okay())return fail(initialized.status,initialized.sourcePC,k);
            if(f.startOn){start(nullptr,r.controls);r.enabled=true;}
            else {stop(&r,r.controls);r.enabled=false;}
        }
        st.prepared=true;return out;
    }catch(...){
        const auto out=visit(S::allocation,0x27d350);
        if(storage_){storage_->failed=true;storage_->stoppedAt=out;}
        return out;
    }
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::constructEmitter15A5B0(
    OriginalUiGarageEmitter50KG id)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!managerPrepared160E00()||i>=7||storage_->emitterCursor!=i||storage_->rows[i].sourceConstructed||
       storage_->prepared||storage_->emitterAttempted)return visit(S::unprepared,0x27c7b0,i+1);
    auto& r=storage_->rows[i];r.definition=storage_->assets->emitter(id);
    if(!r.definition)return visit(S::assetIdentity,0x27c7b0,i+1);
    r.controls.flags1D4=1;
    r.controls.position30[3]=r.controls.previous40[3]=r.controls.velocity60[3]=0x3f800000;
    r.controls.intensity204=r.definition->type==0?0x3f800000:0;
    r.sourceField19C=0;r.sourceAttachment74=nullptr;
    r.sourceConstructed=true;r.enabled=true;
    return visit(S::complete,0x15a71c,i+1);
}
bool OriginalUiGarageParticles50KG::bindSourceAttachment15DEF0(
    OriginalUiGarageEmitter50KG id,const SceneGraph49P& graph,SceneNodeIndex49P node,
    std::uint32_t sourceRow,const OriginalRacerSceneObject50LA& actualNode)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!managerPrepared160E00()||i>=7||storage_->emitterCursor!=i||
       !storage_->rows[i].sourceConstructed||storage_->rows[i].sourceRead||
       storage_->rows[i].attachmentBound||(i!=1&&i!=2&&i!=5)||!validateSceneGraph49P(graph))return false;
    const auto index=static_cast<std::int32_t>(node);
    if(index<0||static_cast<unsigned>(index)>=graph.count||actualNode.sceneIndex!=node||
       !actualNode.constructed||actualNode.released)return false;
    const auto* actor=storage_->assets->scene()->actor(i==5?8:5);
    assets::PsgNode49O original{};
    if(!actor||!actor->model||!assets::readPsgNode49O(actor->model->hierarchy,sourceRow,original)||
       (i==5?sourceRow!=0:!same(original.name,"Plane01")))return false;
    auto& r=storage_->rows[i];r.attachment.graph=&graph;r.attachment.node=node;
    for(unsigned axis=0;axis<3;++axis)r.attachment.offset50[axis]=r.definition->offset[axis];
    r.sourceAttachment74=&actualNode;r.attachmentBound=true;return true;
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::readEmitter15AEA0(
    OriginalUiGarageEmitter50KG id,const std::vector<std::uint8_t>& bytes,std::uint32_t& raw)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!managerPrepared160E00()||i>=7||storage_->emitterCursor!=i||
       !storage_->rows[i].sourceConstructed||storage_->rows[i].sourceRead)return visit(S::unprepared,0x15ae00,i+1);
    auto& st=*storage_;auto& r=st.rows[i];
    const auto fail=[&](S status,std::uint32_t pc){st.failed=true;st.stoppedAt=visit(status,pc,i+1);return st.stoppedAt;};
    const auto* authenticated=st.assets->emitter(id);
    if(!authenticated||bytes.empty()||bytes.size()!=authenticated->source.size||
       std::memcmp(bytes.data(),authenticated->source.data,bytes.size()))return fail(S::assetIdentity,0x1577d0);
    if(!authenticated->attachMcf.empty()&&!r.attachmentBound)return fail(S::unprepared,0x15df30);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x15b148);
    try {
        // Parse the actual opened Config's bytes. Preflight definitions neither
        //supply these records nor replace the reached157320/1577D0 lifetime.
        auto& f=r.liveDefinition;f.source=authenticated->source;f.filename=authenticated->filename;
        f.attachMcf=authenticated->attachMcf;f.childName=authenticated->childName;
        f.type=authenticated->type;f.zone=authenticated->zone;f.position=authenticated->position;
        f.offset=authenticated->offset;f.startOn=authenticated->startOn;f.additiveToy=authenticated->additiveToy;
        D diagnostic;const auto decoded=parseDefinition(bytes,f,diagnostic);
        if(decoded!=S::complete)return fail(decoded,diagnostic.sourcePC);
        r.definition=&f;
        for(unsigned axis=0;axis<3;++axis)r.controls.velocity60[axis]=word(f.velocity60[axis]);
        const auto* group=st.manager.group(f.particleGroup1B8);
        if(!group)return fail(S::authoredData,0x15aee0);
        OriginalGcParticleEmitterState50KG common;common.particleGroup=f.particleGroup1B8;
        common.framesPerSet=f.framesPerSet;common.inverseFramesPerSet=f.inverseFramesPerSet;
        common.gravity=f.gravityMultiplier;common.bouncePlane=f.groundLevel;common.bounceFactor=f.restitution;
        r.managerOwnerId=st.manager.registerEmitter(common);
        if(!r.managerOwnerId)return fail(S::allocation,0x15aee0);
        r.firstFrame1B0=r.currentFrame190=group->first;r.lastFrame1B4=group->last;
        const auto initialized=reset(r,raw,true);
        if(!initialized.okay()){auto out=fail(initialized.status,initialized.sourcePC);
            out.randomDraws=initialized.randomDraws;st.stoppedAt=out;return out;}
        r.sourceRead=true;auto out=visit(S::complete,0x15ae50,i+1);out.randomDraws=initialized.randomDraws;return out;
    }catch(...){return fail(S::allocation,0x15ae00);}
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::activateEmitter27D7A0(
    OriginalUiGarageEmitter50KG id)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!managerPrepared160E00()||i>=7||storage_->emitterCursor!=i||
       !storage_->rows[i].sourceRead||storage_->rows[i].sourceActivated)return visit(S::unprepared,0x27d7a0,i+1);
    auto& r=storage_->rows[i];const auto& f=*r.definition;
    if(f.attachMcf.empty()){
        const std::uint32_t pos[4]{word(f.position[0]),word(f.position[1]),word(f.position[2]),0x3f800000};
        positionEffect50BC(r.controls,pos);positionEffect50BC(r.controls,pos);
    }
    // Source start80/stop84 precedes the caller's actual Entity20/18 dispatch.
    if(f.startOn){start(nullptr,r.controls);r.enabled=true;}
    else {stop(&r,r.controls);r.enabled=false;}
    r.sourceActivated=true;return visit(S::complete,0x27dc70,i+1);
}
bool OriginalUiGarageParticles50KG::finishEmitter27D75C(OriginalUiGarageEmitter50KG id)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!managerPrepared160E00()||i>=7||storage_->emitterCursor!=i||
       !storage_->rows[i].sourceActivated||storage_->rows[i].sourceFinished)return false;
    storage_->rows[i].sourceFinished=true;++storage_->emitterCursor;
    storage_->prepared=storage_->emitterCursor==7;return true;
}
bool OriginalUiGarageParticles50KG::retireEmitter15A720(OriginalUiGarageEmitter50KG id)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!storage_||i>=7||!storage_->rows[i].sourceConstructed)return false;
    auto& r=storage_->rows[i];if(r.sourceRetired)return true;
    for(unsigned later=i+1;later<7;++later)
        if(storage_->rows[later].sourceConstructed&&!storage_->rows[later].sourceRetired)return false;
    // Only cold owners are admitted here. A live pool can retain source48
    //emitter pointers; its actual later retirement must be joined separately.
    if(storage_->manager.particleCount())return false;
    r.sourceRetired=true;storage_->prepared=false;return true;
}
bool OriginalUiGarageParticles50KG::retainedPoolCurrentForEmitterRetirement15A720()const noexcept {
    if(!storage_||!storage_->managerPrepared||storage_->failed||storage_->emitterAttempted||
       storage_->emitterCursor!=storage_->rows.size()||!storage_->assets||!storage_->assets->loaded()||
       !storage_->manager.prepared()||!storage_->manager.particleCount()||
       storage_->manager.particleCount()>originalGcParticleCapacity50KG||
       storage_->manager.emitterRegistryReservation().registeredEmitters!=storage_->rows.size())return false;
    const auto& st=*storage_;
    for(std::size_t i=0;i<st.rows.size();++i){const auto& r=st.rows[i];
        if(!r.sourceConstructed||!r.sourceRead||!r.sourceActivated||!r.sourceFinished||r.failed||
           r.definition!=&r.liveDefinition||!r.liveDefinition.parsed||r.managerOwnerId!=i+1||
           !st.manager.emitter(r.managerOwnerId))return false;
    }
    // Source48 is a native integer handle into this SAME retained value-only
    // registry. No particle borrows a retiring Entity wrapper or graph here.
    for(std::size_t i=0;i<st.manager.particleCount();++i){const auto* p=st.manager.particleAt(i);
        if(!p||!p->ownerId||p->ownerId>st.rows.size()||
           st.rows[p->ownerId-1].managerOwnerId!=p->ownerId||!st.manager.emitter(p->ownerId))return false;
    }
    return true;
}
bool OriginalUiGarageParticles50KG::retireRetainedPoolEmitter15A720(OriginalUiGarageEmitter50KG id)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(i>=7||!retainedPoolCurrentForEmitterRetirement15A720())return false;
    auto& r=storage_->rows[i];if(r.sourceRetired)return true;
    for(unsigned later=i+1;later<7;++later)
        if(!storage_->rows[later].sourceRetired)return false;
    //15A720 does not unlink, clear or step particles. Preserve the complete
    //pool, owner handles, registry and Runtime backing until actual160C70.
    r.sourceRetired=true;storage_->prepared=false;return true;
}
bool OriginalUiGarageParticles50KG::emitterRowsRetired15A720()const noexcept {
    if(!storage_)return true;
    for(const auto& r:storage_->rows)if(r.sourceConstructed&&!r.sourceRetired)return false;
    return !storage_->emitterAttempted;
}
bool OriginalUiGarageParticles50KG::snapshot(OriginalUiGarageEmitter50KG id,OriginalUiGarageEmitterSnapshot50KG& out) const noexcept {
    const auto i=static_cast<unsigned>(id);if(!storage_||i>=7||(!prepared()&&!storage_->rows[i].sourceConstructed))return false;
    const auto& r=storage_->rows[i];out={r.controls,r.delay1DC,r.fractional224,r.remainder200,r.carriedSize18C,
        r.duration1E4,r.managerOwnerId,r.enabled,r.attachmentBound};return true;
}
bool OriginalUiGarageParticles50KG::bindAttachment(OriginalUiGarageEmitter50KG id,const SceneGraph49P& graph,
    SceneNodeIndex49P node,std::uint32_t sourceRow,const OriginalUiGarageEmitterRenderBinding50KG& render) noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!prepared()||storage_->failed||(i!=1&&i!=2&&i!=5)||storage_->rows[i].attachmentBound||!validateSceneGraph49P(graph))return false;
    const auto index=static_cast<std::int32_t>(node);if(index<0||static_cast<unsigned>(index)>=graph.count)return false;
    const auto* actor=storage_->assets->scene()->actor(i==5?8:5);if(!actor||!actor->model)return false;
    assets::PsgNode49O original{};
    if(!assets::readPsgNode49O(actor->model->hierarchy,sourceRow,original))return false;
    if(i==5?sourceRow!=0:!same(original.name,"Plane01"))return false;
    if(i==1&&(!render.attachment19CPresent||(*render.attachment19CPresent&&
       (!render.child04TagA4||!render.currentTag8BE0))))return false;
    auto& r=storage_->rows[i];r.attachment.graph=&graph;r.attachment.node=node;
    for(unsigned axis=0;axis<3;++axis)r.attachment.offset50[axis]=r.definition->offset[axis];
    r.render=render;r.attachmentBound=true;return true;
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::reset88(OriginalUiGarageEmitter50KG id,std::uint32_t& raw) noexcept {
    const auto i=static_cast<unsigned>(id);if(!prepared()||i>=7)return visit(S::unprepared,0x15ad50,i+1);
    if(storage_->failed)return storage_->stoppedAt;
    const auto r=reset(storage_->rows[i],raw,false);auto out=visit(r.status,r.sourcePC,i+1);
    out.randomDraws=r.randomDraws;if(!r.okay()){storage_->failed=true;storage_->stoppedAt=out;}return out;
}
bool OriginalUiGarageParticles50KG::enable20(OriginalUiGarageEmitter50KG id) noexcept {
    const auto i=static_cast<unsigned>(id);if(!prepared()||storage_->failed||i>=7)return false;
    storage_->rows[i].enabled=true;return true;
}
bool OriginalUiGarageParticles50KG::disable18(OriginalUiGarageEmitter50KG id) noexcept {
    const auto i=static_cast<unsigned>(id);if(!prepared()||storage_->failed||i>=7)return false;
    storage_->rows[i].enabled=false;return true;
}
bool OriginalUiGarageParticles50KG::start80(OriginalUiGarageEmitter50KG id) noexcept {
    const auto i=static_cast<unsigned>(id);if(!prepared()||storage_->failed||i>=7)return false;
    return start(nullptr,storage_->rows[i].controls);
}
bool OriginalUiGarageParticles50KG::stop84(OriginalUiGarageEmitter50KG id) noexcept {
    const auto i=static_cast<unsigned>(id);if(!prepared()||storage_->failed||i>=7)return false;
    auto& r=storage_->rows[i];return stop(&r,r.controls);
}
OriginalGcParticleStatus50KG OriginalUiGarageParticles50KG::normalizeAtlas161234(float width,float height) noexcept {
    if(!prepared()||storage_->failed)return OriginalGcParticleStatus50KG::unsupported;
    return storage_->manager.normalizeGarageAtlas161234(width,height);
}
OriginalGcParticleReceipt50KG OriginalUiGarageParticles50KG::stepPool(float dt,bool paused) noexcept {
    if(!prepared()||storage_->failed)return {};
    return storage_->manager.advance(dt,paused);
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::updateSourceCommon15A7A0(
    OriginalUiGarageEmitter50KG id,float dt,bool paused,std::uint32_t& raw)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!prepared()||storage_->failed||i>=7)return visit(S::unprepared,0x15a7a0,i+1);
    auto& r=storage_->rows[i];
    if(!r.sourceConstructed||!r.sourceRead||!r.sourceActivated||!r.sourceFinished||
       r.sourceRetired||r.failed||!r.definition)return visit(S::unprepared,0x15a7a0,i+1);
    const auto& f=*r.definition;const OriginalGcCommonEmitterDispatch50KG calls{&r,start,stop};
    const auto common=advanceOriginalGcCommonEmitter50KG(r.controls,f.timing,r.delay1DC,
        r.duration1E4,!f.attachMcf.empty(),dt,paused,raw,&calls,r.attachmentBound?&r.attachment:nullptr);
    OriginalUiGarageParticleVisit50KG out;static_cast<OriginalGcSprayReceipt50KG&>(out)=common;
    out.emitterOrdinal=i+1;
    if(!common.okay()){storage_->failed=true;storage_->stoppedAt=out;}
    return out;
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::continueSourceEmitterAfter15A7A0(
    OriginalUiGarageEmitter50KG id,float dt,std::uint32_t& raw,
    SharedRandomOwner50ID& selectors,HostMonotonicClock50ID* clock,
    OriginalWreckExplosionGlobals50LA& explosionGlobals)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!prepared()||i>=7)return visit(S::unprepared,0x15A7A0,i+1);
    auto& st=*storage_;if(st.failed)return st.stoppedAt;
    auto& r=st.rows[i];
    if(!r.sourceConstructed||!r.sourceRead||!r.sourceActivated||!r.sourceFinished||
       r.sourceRetired||r.failed||!r.definition)return visit(S::unprepared,0x15A7A0,i+1);
    const auto& f=*r.definition;
    Receipt derived;
    if(f.type==11){
        // Real Garage type11 is source-null74/moving=false. Existing sparks
        // preserves its actual RNG, pool-capacity and partial-write ordering.
        derived=sparks(r,f,st.manager,dt,raw);
    }else if(f.type==0){
        if(!clock)derived=result(S::unprepared,0x15BB40);
        else derived=original_smoke_numeric_detail50RC::emit<0x15bb40u>(
            r,f,st.manager,r.controls,dt,raw,selectors,*clock);
    }else if(f.type==9){
        derived=explosion(r,f,st.manager,dt,raw,explosionGlobals);
    }else{
        //Fire requires the intervening actual live observer read, not a
        //caller visibility flag. Its private two-part path is below.
        derived=result(S::unsupported,f.type==8?0x15E10Cu:0x141F60u);
    }
    OriginalUiGarageParticleVisit50KG out;
    static_cast<OriginalGcSprayReceipt50KG&>(out)=derived;out.emitterOrdinal=i+1;
    if(!out.okay()){st.failed=true;st.stoppedAt=out;}
    return out;
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::beginSourceFire15E10C(
    OriginalUiGarageEmitter50KG id,float dt,const OriginalRacerSceneObject50LA*& attachment)noexcept {
    attachment=nullptr;const auto i=static_cast<unsigned>(id);
    if(!prepared()||i>=7)return visit(S::unprepared,0x15E10C,i+1);
    auto& st=*storage_;if(st.failed)return st.stoppedAt;auto& r=st.rows[i];
    auto stop=[&](S status,std::uint32_t pc){auto out=visit(status,pc,i+1);
        st.failed=true;st.stoppedAt=out;return out;};
    if(!r.sourceConstructed||!r.sourceRead||!r.sourceActivated||!r.sourceFinished||
       r.sourceRetired||r.failed||!r.definition||r.definition->type!=8)
        return stop(S::unprepared,0x15E10C);
    Math m;std::array<float,3> velocity{};const auto inverse=m.div(1,dt,0x15E140);
    for(unsigned axis=0;axis<3;++axis)
        velocity[axis]=m.mul(m.sub(bits(r.controls.position30[axis]),bits(r.controls.previous40[axis]),
            0x15E128u+8u*axis),inverse,0x15E160u+4u*axis);
    if(m.badPC)return stop(S::arithmetic,m.badPC);
    for(unsigned axis=0;axis<3;++axis)r.controls.velocity60[axis]=word(velocity[axis]);
    r.controls.velocity60[3]=0x3f800000;
    //Exact15E184 reads the real retained source pointer AFTER velocity
    //writes. An authored attachment must already have its genuine row bind.
    if((!r.definition->attachMcf.empty()&&(!r.attachmentBound||!r.sourceAttachment74))||
       (r.sourceAttachment74&&r.sourceAttachment74->released))return stop(S::unprepared,0x15E184);
    attachment=r.sourceAttachment74;return visit(S::complete,0x15E184,i+1);
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::finishSourceFire15E1D0(
    OriginalUiGarageEmitter50KG id,float dt,std::uint32_t& raw,std::uint32_t& serial,bool& lazy)noexcept {
    const auto i=static_cast<unsigned>(id);
    if(!prepared()||i>=7)return visit(S::unprepared,0x15E1D0,i+1);
    auto& st=*storage_;if(st.failed)return st.stoppedAt;auto& r=st.rows[i];
    if(!r.sourceConstructed||!r.sourceRead||!r.sourceActivated||!r.sourceFinished||
       r.sourceRetired||r.failed||!r.definition||r.definition->type!=8)
        return visit(S::unprepared,0x15E1D0,i+1);
    const auto& f=*r.definition;std::array<float,3> previous{},velocity{};
    for(unsigned axis=0;axis<3;++axis){previous[axis]=bits(r.controls.previous40[axis]);
        velocity[axis]=bits(r.controls.velocity60[axis]);}
    OriginalGcFireEmissionDefinition50KG fire;
    fire.particleGroup=f.particleGroup1B8;fire.firstFrame=r.firstFrame1B0;fire.lastFrame=r.lastFrame1B4;
    fire.particleLife=f.particleLife17C;fire.initialSize=f.initialSize184;fire.growth=f.growth180;
    fire.emitRate=f.emitRate188;fire.framesPerSet=f.framesPerSet;fire.minRange=f.minRange;fire.maxRange=f.maxRange;
    fire.authoredVelocityY220=f.spreadVelocityScale220;fire.particleFlags=f.particleFlags198;
    fire.randomMirrorX=f.randomMirrorX;fire.randomMirrorY=f.randomMirrorY;fire.applySpreadToVelocity=f.applySpreadToVelocity;
    const auto receipt=fireDefinedCount(fire,st.manager,r.managerOwnerId,r.fractional224,previous,
        r.controls.previous40[3],velocity,dt,raw,serial,lazy);
    OriginalUiGarageParticleVisit50KG out;static_cast<OriginalGcSprayReceipt50KG&>(out)=receipt;
    out.emitterOrdinal=i+1;out.callerCarryUnavailable=receipt.callerCarryUnavailable;
    if(!out.okay()){st.failed=true;st.stoppedAt=out;}return out;
}
OriginalUiGarageParticleVisit50KG OriginalUiGarageParticles50KG::stepPageEmitters(
    float dt,bool paused,OriginalUiGarageParticleShared50KG& shared) noexcept {
    if(!prepared())return visit(S::unprepared,0x27dfc0);
    auto& st=*storage_;if(st.failed)return st.stoppedAt;
    auto out=visit(S::complete,0x27dfc0);
    auto absorb=[&](const Receipt& r,unsigned i){
        out.randomDraws+=r.randomDraws;out.spawned+=r.spawned;out.poolFull|=r.poolFull;
        if(!r.okay()){out.status=r.status;out.sourcePC=r.sourcePC;out.emitterOrdinal=i+1;st.failed=true;st.stoppedAt=out;return false;}
        return true;
    };
    for(unsigned i=0;i<7;++i){
        auto& r=st.rows[i];if(!r.enabled)continue;
        const auto& f=*r.definition;const bool attached=!f.attachMcf.empty();
        const OriginalGcCommonEmitterDispatch50KG dispatch{&r,start,stop};
        const auto common=advanceOriginalGcCommonEmitter50KG(r.controls,f.timing,r.delay1DC,r.duration1E4,
            attached,dt,paused,shared.raw308030,&dispatch,r.attachmentBound?&r.attachment:nullptr);
        if(!absorb(common,i))return out;
        if(common.commonSkipped){out.commonSkipped=true;continue;}
        Receipt derived;
        if(f.type==0)derived=original_smoke_numeric_detail50RC::emit<0x15bb40u>(r,f,st.manager,r.controls,dt,
            shared.raw308030,shared.lazyRandom,shared.clock);
        else if(f.type==11)derived=sparks(r,f,st.manager,dt,shared.raw308030);
        else if(f.type==9)derived=explosion(r,f,st.manager,dt,shared.raw308030,shared.explosion);
        else if(f.type==8){
            Math m;std::array<float,3> previous{},velocity{};
            const auto inverse=m.div(1,dt,0x15e140);
            for(unsigned axis=0;axis<3;++axis){previous[axis]=bits(r.controls.previous40[axis]);
                velocity[axis]=m.mul(m.sub(bits(r.controls.position30[axis]),previous[axis],0x15e128u+8u*axis),inverse,0x15e160u+4u*axis);}
            if(m.badPC){if(!absorb(result(S::arithmetic,m.badPC),i))return out;}
            for(unsigned axis=0;axis<3;++axis)r.controls.velocity60[axis]=word(velocity[axis]);
            r.controls.velocity60[3]=0x3f800000;
            //15E184 follows velocity writes, and the actual render state is
            //read every visit, never replaced by an always-visible flag.
            if(r.sourceConstructed){
                //15E184 reads emitter74, THEN15E198 reads that scene object's
                //observer19C. Emitter's own15DE40 field19C is unrelated. Read
                //the exact borrowed live field on every visit; never supply a
                //separate null/ready flag or infer it from the constructor.
                if(!r.sourceAttachment74||r.sourceAttachment74->released){
                    absorb(result(S::unprepared,0x15e184),i);return out;}
                if(r.sourceAttachment74->observer19C){
                    //The nonnull owner's child04/tagA4 and actual global8BE0
                    //are not yet owned by this route. Keep the reached numeric
                    //and RNG prefix, but do not invent or bypass this gate.
                    absorb(result(S::unsupported,0x15e1a4),i);return out;}
            }else if(!r.render.attachment19CPresent){absorb(result(S::unprepared,0x15e198),i);return out;}
            if(!r.sourceConstructed&&*r.render.attachment19CPresent){
                if(!r.render.child04TagA4||!r.render.currentTag8BE0){absorb(result(S::unprepared,0x15e1a4),i);return out;}
                if(*r.render.child04TagA4!=*r.render.currentTag8BE0){out.renderGateSkipped=true;continue;}
            }
            OriginalGcFireEmissionDefinition50KG fire;
            fire.particleGroup=f.particleGroup1B8;fire.firstFrame=r.firstFrame1B0;fire.lastFrame=r.lastFrame1B4;
            fire.particleLife=f.particleLife17C;fire.initialSize=f.initialSize184;fire.growth=f.growth180;
            fire.emitRate=f.emitRate188;fire.framesPerSet=f.framesPerSet;fire.minRange=f.minRange;fire.maxRange=f.maxRange;
            fire.authoredVelocityY220=f.spreadVelocityScale220;fire.particleFlags=f.particleFlags198;
            fire.randomMirrorX=f.randomMirrorX;fire.randomMirrorY=f.randomMirrorY;fire.applySpreadToVelocity=f.applySpreadToVelocity;
            const auto receipt=fireDefinedCount(fire,st.manager,r.managerOwnerId,r.fractional224,previous,
                r.controls.previous40[3],velocity,dt,shared.raw308030,shared.fireSerial433970,shared.fireLazy84C8);
            out.callerCarryUnavailable|=receipt.callerCarryUnavailable;derived=receipt;
        }else derived=result(S::unsupported,0x27d450);
        if(!absorb(derived,i))return out;
    }
    return out;
}
} //namespace rr::game
