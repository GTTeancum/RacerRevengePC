// Common rewrite (area E): the ONE track ground-effects owner
// (track_particle_emitters50CR::Owner, declared frozen in track_particle_emitters50cr.h),
// for every course and every TOB [ParticleEmitters] row.
//
// Original, traced in work\scratch\dis_all.txt:
//  27DE00/27C600  rows (src\assets\original_particle_emitters50pe.h, the one reader).
//  27C7B0         per row: EmitterFilename "NONE" / NumberOfInstances <= 0 / bad TypeID
//                 -> not loaded (the reader marks it; skipped here, logged).
//                 27C898..27C8E4 TypeID switch allocates NumberOfInstances emitters of
//                 the type: 0 smoke 15B860, 1 dust 15CDC0, 2 sparks 15D5E0, 8 fire
//                 15DE00, 9 explosion 15E800, 11 firework 15F100, 12 fog 15FE30.
//                 Attached rows: 27CC4C..27CFBC walks every zone's model group
//                 (World+34C zones, +3C4 groups) in world order and takes each model
//                 whose SoulTree name matches AttachToSoultree (308F00, no case), up to
//                 NumberOfInstances; ChildObject != "NONE" replaces the scene object by
//                 28C590 (name search, no case: node, then its siblings, then its
//                 children). A second pass over World+3C8 dynamic objects (27CFC8..)
//                 and the 27D6C4 assertion (fewer matches than instances) follow.
//  27DBE0         per emitter load: StartOn != 0 -> virtual 80 start + 20 enable, else
//                 84 stop + 18 disable.
//  15AEA0         common EMT fields; 15A7A0 common per-frame prefix.
//  15E128..15E16C fire velocity, 15E1D0.. fire emission; 15D938..15DCD4 sparks;
//  15F3C0..15FBC8 firework; 15BB40.. smoke; 160160 fog phase; 160520 fog populate;
//  1602B0 fog fields; 15A500 / 15A560 trigger TurnOn / TurnOff.
//
// The effect is chosen ONLY by the row's TypeID (data). No member, function or
// branch names a course or a pod. Storage: the three Blocks below are sized once
// from the table's own counts (live rows, distinct EmitterFilename count, sum of
// instances); every write bounds-checked; nothing writes guest memory (all of
// this is host memory; the manager rows written are the rows these placements
// registered).
#include "track_particle_emitters50cr.h"
#include "track_particle_emitter_sink50cr.h"   // compile coverage for the trigger sink
#include "mc_type2_numeric50mc.h"
#include "gb_type11_numeric50gb.h"
#include "stock_course_receipts50cr.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/assets/original_particle_emitters50pe.h"
#include "../../src/assets/original_ui_pages50kg.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/original_gc_fire_emission50kg.h"
#include "../../src/game/original_smoke_numeric50rc.h"
#include "../../src/game/original_rc_fog_numeric50rc.h"
#include "../../src/game/scene_graph49p.h"
#include "../../src/game/effect_control50bc.h"
#include "../../src/game/water_wake50jc.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <string>
#include <thread>
#include <vector>

namespace track_particle_emitters50CR {
namespace {
namespace g=rr::game;namespace a=rr::assets;namespace pe=rr::assets::particle_emitters50PE;
namespace ct=common_track50CR;namespace receipts=stock_course_receipts50CR;
using Status=g::OriginalGcSprayStatus50KG;
constexpr auto loadStep=ct::log::Step::particles1B4DC0;
constexpr std::size_t none=std::numeric_limits<std::size_t>::max();
// EMT extraction cap: the 159440 text reader's own bound (32 KiB, sections()
// below refuses anything larger), so the cap admits every file that reader can
// read and nothing more. Stock EMTs are <= 2911 bytes (receipts, not a size).
constexpr std::size_t emtCap=32768;
// PSG extraction cap for a ChildObject hierarchy (transient, released after the
// graph is built). PORT STAND-IN (flagged): allocation guard only; the original
// reads the model's scene object, which the port rebuilds from its PSG file.
constexpr std::size_t psgCap=4u*1024u*1024u;

float bits(std::uint32_t word)noexcept {float f;std::memcpy(&f,&word,4);return f;}
float word(std::uint32_t n)noexcept {float f;std::memcpy(&f,&n,4);return f;}
void put(std::uint32_t& n,float f)noexcept {std::memcpy(&n,&f,4);}
bool sameNoCase(std::string_view x,std::string_view y)noexcept {
    if(x.size()!=y.size())return false;
    for(std::size_t i=0;i<x.size();++i){char p=x[i],q=y[i];if(p>='A'&&p<='Z')p=char(p+32);if(q>='A'&&q<='Z')q=char(q+32);if(p!=q)return false;}
    return true;
}
const char* effectName(Effect e)noexcept {
    switch(e){case Effect::smoke:return "smoke";case Effect::sparks:return "sparks";case Effect::fire:return "fire";
        case Effect::firework:return "firework";case Effect::fog:return "fog";}
    return "?";
}
bool effectOf(std::int32_t typeId,Effect& out)noexcept {
    switch(typeId){case 0:out=Effect::smoke;return true;case 2:out=Effect::sparks;return true;case 8:out=Effect::fire;return true;
        case 11:out=Effect::firework;return true;case 12:out=Effect::fog;return true;default:return false;}
}
// One common_track_data line for a dotted sub-role; the receipt is looked up
// only for a stock course. Never sizes anything.
void trackDataNamed(const ct::Track& t,std::string_view role,std::string_view file,std::uint64_t value){
    const auto* r=t.source()==ct::Source::stock?receipts::find(t.code(),role):nullptr;
    ct::log::trackDataNamed(role,t.code(),file,value,r!=nullptr,r?r->value:0);
}
bool refuse(ct::Diagnostic& d,std::uint32_t pc,const std::string& why)noexcept {
    try{
        std::cerr<<"common_track_emitters refused pc=0x"<<std::hex<<pc<<std::dec<<" reason="<<why<<'\n';
        d.refuse(loadStep,pc,why);
    }catch(...){d.refused=true;d.pc=pc;d.step=loadStep;}
    return false;
}

// ------------------------------------------------------------------ EMT definition
// One row per distinct EmitterFilename, carrying the fields of all five types
// (named by their original object offsets). Defaults are each reader's own.
struct Definition {
    Effect effect=Effect::smoke;
    g::OriginalGcCommonEmitterTiming50KG timing{};
    // 15AEA0 common
    std::array<float,3> velocity60{},minRange{},maxRange{};
    std::int32_t particleGroup1B8=-1;
    float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
    float gravityMultiplier=3,groundLevel=std::numeric_limits<float>::max(),restitution=.55f;
    float framesPerSet=-1,inverseFramesPerSet=-1;std::uint32_t particleFlags198=0;
    bool randomMirrorX=false,randomMirrorY=false;
    // 15E6D0 fire
    bool applySpreadToVelocity=true;
    // 15DD00 sparks (TypeID 2)
    bool movingEmitter200=true;
    // 15FC10 firework (TypeID 11)
    float minStreamLength200=3,maxStreamLength204=5,particleSeparation208=1.5f,tailLife20C=.6f;
    float tailThickness210=1,headThickness214=2;bool superbrightHead218=true,movingEmitter21C=false;
    // 15C330 smoke (TypeID 0)
    std::array<float,3> minimumVelocityNoise240{},maximumVelocityNoise250{};
    float minimumInitialSize208=0,maximumInitialSize20C=5,initialSizeToSpeedRatio210=.01f;
    float minimumLife214=500,maximumLife218=3500,minimumPercentLife21C=.5f;
    float minimumPercentLifeConvergance220=1,lifeToSpeedRatio224=5,initialPercentAge228=0;
    float minimumEmitRate22C=0,maximumEmitRate230=10,particlesToDistanceRatio234=.01f;
    float particlesToSpeedRatio238=0,minimumEmissionSpeed23C=0;
    // 1602B0 fog (TypeID 12)
    float distanceSquared220=0,inverseDistanceSquared224=0,particlesInVolume228=0,oscillationPeriod22C=0,oscillationMagnitude234=0;
    bool parsed=false;
};

using Section=a::OriginalUiDatSection50KG;
using TextDiagnostic=a::OriginalUiPageDiagnostic50KG;
using TextError=a::OriginalUiPageError50KG;
bool space(char c)noexcept{return c==' '||c=='\t'||c=='\r';}
// Bounded source 159440 cleaning and 158648 first-present lookup (the lanes'
// reader, unchanged). Unknown keys and duplicates remain in source order;
// quoted empty differs from missing. Text authoring only.
bool sections(const std::vector<std::uint8_t>& bytes,std::vector<Section>& out,TextDiagnostic& d){
    auto fail=[&](TextError e,std::size_t at){d={e,at,0x159440u};return false;};
    if(bytes.empty()||bytes.size()>32768)return fail(TextError::capacity,0);
    auto size=bytes.size();if(bytes.back()==0)--size;
    for(std::size_t i=0;i<size;++i)if((bytes[i]<32&&bytes[i]!=9&&bytes[i]!=10&&bytes[i]!=13)||bytes[i]>126)
        return fail(TextError::unsupportedEncoding,i);
    for(std::size_t at=0;at<size;){
        const auto start=at;while(at<size&&bytes[at]!='\n')++at;auto end=at;
        if(at<size){++at;if(end>start&&bytes[end-1]=='\r')--end;}
        if(end-start>2048)return fail(TextError::capacity,start);
        auto first=start;while(first<end&&space(char(bytes[first])))++first;
        if(first==end||bytes[first]==';'||bytes[first]==':')continue;
        // 158724/158800 skips a line without an '=' value; colon key/value
        // annotations also fail the requested-key comparison at 15880C.
        const bool header=bytes[first]=='[';bool quoted=false;int brackets=0;
        std::string line;std::vector<std::size_t> offsets;
        for(auto i=start;i<end;++i){const char c=char(bytes[i]);
            if(header&&c==';'&&!quoted&&brackets==0&&!line.empty()&&line.back()==']')break;
            if(c=='"')quoted=!quoted;else if(c=='[')++brackets;else if(c==']')--brackets;
            if(brackets<0||brackets>16)return fail(TextError::textSyntax,i);
            if(space(c)&&!quoted&&brackets==0)continue;
            line.push_back(c);offsets.push_back(i);
        }
        if(line.empty()||line[0]==';')continue;
        if(brackets)return fail(TextError::textSyntax,start);
        if(line[0]=='['){
            if(line.size()<3||line.back()!=']')return fail(TextError::textSyntax,start);
            auto name=line.substr(1,line.size()-2);
            if(name.size()>127||name.find_first_of("[]=\"")!=std::string::npos)return fail(TextError::textSyntax,start);
            for(const auto& s:out)if(sameNoCase(s.name,name))return fail(TextError::duplicateSection,start);
            if(out.size()==64)return fail(TextError::capacity,start);
            out.push_back({std::move(name),offsets[1],{}});continue;
        }
        const auto equal=line.find('=');
        if(equal==std::string::npos)continue; //158800: no value token.
        if(out.empty())continue; //No current requested source block yet.
        if(!equal||equal>127)return fail(TextError::textSyntax,start);
        auto key=line.substr(0,equal);if(key.find_first_of("[];\"")!=std::string::npos)return fail(TextError::textSyntax,start);
        auto va=equal+1,vb=line.size();const bool present=va<vb;
        if(va<vb&&line[va]=='"')++va;if(va<vb&&line[vb-1]=='"')--vb;
        if(out.back().values.size()==256)return fail(TextError::capacity,start);
        const auto valueOffset=va<offsets.size()?offsets[va]:offsets[equal]+1;
        out.back().values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
    }
    return !out.empty()||fail(TextError::textSyntax,0);
}
const Section& section(const std::vector<Section>& doc,const char* name,const Section& absent)noexcept {
    for(const auto& s:doc)if(sameNoCase(s.name,name))return s;return absent;
}
// The per-TypeID reader then 15AEA0, in the original order of each subtype
// reader (derived fields read before 15AEA0 where the subtype does so).
bool parseDefinition(const std::vector<std::uint8_t>& bytes,Effect effect,Definition& out,TextDiagnostic& d,std::string& why){
    if(std::fegetround()!=FE_TONEAREST){why="rounding mode";return false;}
    std::vector<Section> doc;if(!sections(bytes,doc,d)){why="EMT text (159440)";return false;}
    const Section absent{};const auto& general=section(doc,"General Info",absent);
    const auto& properties=section(doc,"Properties",absent);const auto& animation=section(doc,"Animation",absent);
    Definition f;f.effect=effect;g::water_wake_detail50jc::Math m;
    auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){return a::originalUiNumber50KG(s,key,fallback,value,d,pc);};
    auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){return a::originalUiInteger50KG(s,key,fallback,value,d,pc);};
    auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){return a::originalUiTriple50KG(general,key,{},value,d,pc);};
    auto b=[&](const Section& s,const char* key,bool fallback,bool& value,std::uint32_t pc){return a::originalUiBoolean50KG(s,key,fallback,value,d,pc);};
    why="EMT field";
    if(effect==Effect::fog){float distance=0;
        //1602B0 reads derived fog fields BEFORE 15AEA0.
        if(!v("MinRange",f.minRange,0x1602f0)||!v("MaxRange",f.maxRange,0x16030c)||
           !n(general,"DistanceToFade",200,distance,0x160328))return false;
        f.distanceSquared220=m.mul(distance,distance,0x160344);
        f.inverseDistanceSquared224=m.div(1,f.distanceSquared220,0x160354);
        if(!n(general,"ParticlesInVolume",100,f.particlesInVolume228,0x16036c)||
           !n(general,"OscillationSpeed",10,f.oscillationPeriod22C,0x160388)||
           !n(general,"OscillationMagnitude",1,f.oscillationMagnitude234,0x1603a4))return false;
    }
    if(effect==Effect::fire){
        //15E6D0 reads the two derived ranges and spread flag BEFORE 15AEA0.
        if(!v("MinRange",f.minRange,0x15e710)||!v("MaxRange",f.maxRange,0x15e72c)||
           !b(general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,0x15e744))return false;
    }
    if(effect==Effect::sparks){
        //15DD00 reads MovingEmitter default 1 and both ranges before 15AEA0.
        if(!b(general,"MovingEmitter",true,f.movingEmitter200,0x15dd3c)||
           !v("MinRange",f.minRange,0x15dd58)||!v("MaxRange",f.maxRange,0x15dd74))return false;
    }
    if(effect==Effect::firework){
        //15FC10 reads the stream fields before 15AEA0.
        if(!n(general,"MinStreamLength",3,f.minStreamLength200,0x15fc50)||
           !n(general,"MaxStreamLength",5,f.maxStreamLength204,0x15fc6c)||
           !n(general,"TailThickness",1,f.tailThickness210,0x15fc88)||
           !n(general,"HeadThickness",2,f.headThickness214,0x15fca4)||
           !n(general,"ParticleSeparationFactor",1.5f,f.particleSeparation208,0x15fcc0)||
           !n(general,"TailLifeModifier",bits(0x3f19999a),f.tailLife20C,0x15fce0)||
           !b(general,"SuperbrightHead",true,f.superbrightHead218,0x15fcf8)||
           !v("MinRange",f.minRange,0x15fd14)||!v("MaxRange",f.maxRange,0x15fd30)||
           !b(general,"MovingEmitter",false,f.movingEmitter21C,0x15fd48))return false;
    }
    if(!i(general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
       !n(general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
       !n(general,"InitialSize",1,f.initialSize184,0x15af8c)||
       !n(general,"Growth",0,f.growth180,0x15afa4)||
       !n(general,"EmitRate",500,f.emitRate188,0x15afc0)||
       !n(general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
       !v("Velocity",f.velocity60,0x15b004)||
       !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
       !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
       !n(general,"Restitution",bits(0x3f0ccccd),f.restitution,0x15b0a0))return false;
    if(g::readOriginalGcCommonEmitterTiming50KG(bytes,f.timing,d)!=Status::complete){why="EMT common timing (15AEA0)";return false;}
    f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
    const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
    const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
    const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
    for(unsigned k=0;k<8;++k){bool value=k==7;if(!b(properties,keys[k],k==7,value,pcs[k]))return false;
        if(k==7)value=!value;if(value)f.particleFlags198|=masks[k];}
    if(!b(properties,"RandomMirrorAboutX",false,f.randomMirrorX,0x15b2a8)||
       !b(properties,"RandomMirrorAboutY",false,f.randomMirrorY,0x15b2c0)||
       !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return false;
    if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200u;
    // Existing native policy for unused 1/0: actual 0 has animation flag 400
    // clear; no active numerical consumer reads the stored reciprocal.
    f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
    if(f.framesPerSet>1)f.particleFlags198|=0x400u;
    std::int32_t forced=0;if(!i(animation,"NumberOfForcedSets",0,forced,0x15b4c0))return false;
    // PORT STAND-IN (flagged): forced animation sets (15B4C0 > 0) have no native
    // consumer (every lane refused them); such a definition is refused, not ignored.
    if(forced!=0){why="PORT STAND-IN: NumberOfForcedSets > 0 has no native consumer";return false;}
    if(effect==Effect::fog)f.particleFlags198=(f.particleFlags198|0x80u)&~8u;
    if(effect==Effect::smoke){
        // Generic 15C330 reader, after 15AEA0; keys/defaults from its own reader.
        if(!n(general,"MinimumPercentLife",.5f,f.minimumPercentLife21C,0x15c378)||
           !n(general,"MinimumPercentLifeConvergance",1,f.minimumPercentLifeConvergance220,0x15c438)||
           !n(general,"MinimumInitialSize",0,f.minimumInitialSize208,0x15c4d8)||
           !n(general,"MaximumInitialSize",5,f.maximumInitialSize20C,0x15c57c)||
           !n(general,"InitialSizeToSpeedRatio",bits(0x3c23d70a),f.initialSizeToSpeedRatio210,0x15c624)||
           !n(general,"MinimumLife",500,f.minimumLife214,0x15c6c8)||
           !n(general,"MaximumLife",3500,f.maximumLife218,0x15c770)||
           !n(general,"LifeToSpeedRatio",5,f.lifeToSpeedRatio224,0x15c814)||
           !n(general,"MinimumEmitRate",0,f.minimumEmitRate22C,0x15c8b4)||
           !n(general,"MaximumEmitRate",10,f.maximumEmitRate230,0x15c958)||
           !n(general,"ParticlesToDistanceRatio",bits(0x3c23d70a),f.particlesToDistanceRatio234,0x15ca00)||
           !n(general,"ParticlesToSpeedRatio",0,f.particlesToSpeedRatio238,0x15caa0)||
           !n(general,"InitialPercentAge",0,f.initialPercentAge228,0x15cb40)||
           !n(general,"MinimumEmissionSpeed",0,f.minimumEmissionSpeed23C,0x15cbe0)||
           !v("MinimumVelocityNoise",f.minimumVelocityNoise240,0x15cc84)||
           !v("MaximumVelocityNoise",f.maximumVelocityNoise250,0x15cca0))return false;
    }
    if(m.badPC){d.sourcePC=m.badPC;why="EMT arithmetic";return false;}
    f.parsed=true;out=f;why.clear();return true;
}

// ------------------------------------------------------------------ stored rows
struct Row {                       // Block particleEmitters, one per live 27DE00 row
    Descriptor pub{};
    std::array<float,4> offset{{0,0,0,1}};   // Position (unattached) or OffsetFromSoultree (attached); W 1
    bool loaded=false,attached=false;
    std::string soulTree,childObject,filename;
};
struct DefinitionRow {             // Block particleDefinitions, one per distinct EmitterFilename
    Definition def{};
    std::string filename;
};
struct Placement {                 // Block particleAttachments, one per emitter instance
    Attachment pub{};
    std::size_t graph=none;        // distinct placed world instance (attached only)
    g::SceneNodeIndex49P node=static_cast<g::SceneNodeIndex49P>(0);
    // The emitter object's own mutable fields (constructor + loader + 15AEA0).
    g::EffectControlState50BC controls{};
    float delay1DC=0,fractional224=0,carriedSize18C=0,remainder200=0;
    std::uint32_t duration1E4=0,managerOwnerId=0;
    std::int32_t currentFrame190=0,firstFrame1B0=0,lastFrame1B4=0;
    bool enabled=false;
    // Read by the 15BB40 smoke leaf (its runtime contract); latched on failure.
    bool failed=false;g::OriginalGcSprayReceipt50KG stoppedAt{};
};
// One scene graph per distinct placed world instance. The root node's local is
// the SCB instance matrix (World48F: replaces the saved PSG root transform, as
// every lane did); a ChildObject placement also needs the PSG's child nodes.
struct GraphSlot {
    std::unique_ptr<g::SceneNode49P[]> nodes;
    g::SceneGraph49P graph{nullptr,0};
    const a::WorldInstance48F* instance=nullptr;
    std::size_t worldIndex=0;
    bool hierarchy=false;
};
} // namespace

struct Owner::State {
    const std::thread::id thread=std::this_thread::get_id();
    bool attempted=false,prepared=false,failed=false,quiesced=false,closed=false,restartFlagged=false;
    ct::Block<Row> rows;
    ct::Block<DefinitionRow> definitions;
    ct::Block<Placement> placements;
    std::unique_ptr<GraphSlot[]> graphs;std::size_t graphCount=0;
    std::shared_ptr<g::OriginalTrackParticleCold50LA> cold;
    g::OriginalGcParticleManager50KG* manager=nullptr;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> process;
    std::shared_ptr<native_gc_water_session50KG::Application> globals;
    std::shared_ptr<g::HostMonotonicClock50ID> clock;
    // PORT STAND-IN (flagged): LoadInputs carries no particle-globals pointer this
    // round, so with Inputs::globals null the owner keeps its own copy of the
    // process BSS 84D0/433980 (TypeID 11 lazy speedScale 20). It should be the
    // process-shared one (native_gc_water_session50KG::Application::sparksGlobals);
    // request in w0-requests\round3-E.txt.
    g::OriginalGcPodSparksGlobals50KG ownSparks{};
    std::uint64_t generation=0;
    bool thread_()const noexcept{return thread==std::this_thread::get_id();}
    g::OriginalGcPodSparksGlobals50KG& sparks()noexcept{return globals?globals->sparksGlobals:ownSparks;}
    bool live()const noexcept{return prepared&&!failed&&!quiesced&&!closed&&thread_();}
    bool managerCurrent()const noexcept{return cold&&cold->prepared()&&manager&&cold->particles()==manager;}
};

namespace {
using State=Owner::State;
g::OriginalGcCommonEmitterDispatch50KG dispatch(Placement& p)noexcept {
    // virtual 80 start (15ACE0) / 84 stop (15AD00) of the concrete emitter; the
    // fog subtype's 160490 takes the same control suffix.
    return {&p,
        [](void* x,g::EffectControlState50BC& c){auto& r=*static_cast<Placement*>(x);if(&r.controls!=&c)return false;g::startEffect50BC(c);return true;},
        [](void* x,g::EffectControlState50BC& c){auto& r=*static_cast<Placement*>(x);if(&r.controls!=&c)return false;g::stopEffect50BC(c);return true;}};
}
const g::SceneGraph49P* graphOf(const State& s,const Placement& p)noexcept {
    return p.graph<s.graphCount?&s.graphs[p.graph].graph:nullptr;
}
// Attached fog cold position, 28BA74..15FF8C: node world translation plus the
// offset rotated by the node's basis; previous = position.
bool fogPosition(const State& s,Placement& p,const Row& row,ct::Diagnostic& d)noexcept {
    const auto* graph=graphOf(s,p);
    if(!graph||!g::refreshSceneWorld49P(*graph,p.node))return refuse(d,0x15ff6cu,"fog attachment graph refresh rejected");
    const auto& m=graph->nodes[static_cast<std::int32_t>(p.node)].world100;g::water_wake_detail50jc::Math math;std::array<float,3> rotated{};
    for(unsigned k=0;k<3;++k)put(p.controls.position30[k],m.v[12+k]);
    for(unsigned k=0;k<3;++k){const auto x=math.mul(m.v[k],row.offset[0],0x28ba74u);
        const auto y=math.mul(m.v[4+k],row.offset[1],0x28ba78u);const auto xy=math.add(x,y,0x28ba78u);
        rotated[k]=math.add(xy,math.mul(m.v[8+k],row.offset[2],0x28ba7cu),0x28ba7cu);}
    if(math.badPC)return refuse(d,math.badPC,"fog attachment offset arithmetic");
    for(unsigned k=0;k<3;++k){const auto value=math.add(word(p.controls.position30[k]),rotated[k],0x15ff8cu+16u*k);
        if(math.badPC)return refuse(d,math.badPC,"fog attachment position arithmetic");put(p.controls.position30[k],value);}
    std::memcpy(p.controls.previous40,p.controls.position30,16);return true;
}
// The emitter's authored start state: constructor control words, loader
// position (unattached rows), 15AEA0 velocity/delay/duration, the group's frame
// range, fog cold population (160520), then 27DBE0 StartOn. `registerRow` adds
// the 15AEA0 manager registration (cold only; restart keeps the registered row).
bool initialize(State& s,Placement& p,std::size_t index,bool registerRow,ct::Diagnostic& d)noexcept {
    const auto* row=s.rows.at(p.pub.descriptor);
    const auto* defRow=row?s.definitions.at(row->pub.definition):nullptr;
    if(!row||!defRow)return refuse(d,0x27c7b0u,"placement "+std::to_string(index)+" lost its row/definition");
    const auto& f=defRow->def;const auto* group=s.manager->group(f.particleGroup1B8);
    if(!group||group->first>group->last)return refuse(d,0x15aee0u,"placement group missing from the manager atlas");
    p.controls={};p.controls.flags1D4=1;
    p.controls.position30[3]=p.controls.previous40[3]=p.controls.velocity60[3]=0x3f800000u;
    for(unsigned k=0;k<3;++k){put(p.controls.velocity60[k],f.velocity60[k]);if(!row->attached)put(p.controls.position30[k],row->offset[k]);}
    std::memcpy(p.controls.previous40,p.controls.position30,16);
    if(row->pub.effect==Effect::smoke)p.controls.intensity204=0x3f800000u;
    p.delay1DC=f.timing.delayTime1E0;put(p.duration1E4,f.timing.maxTimeOn1EC);
    p.fractional224=0;p.carriedSize18C=0;p.remainder200=0;p.failed=false;p.stoppedAt={};
    p.currentFrame190=p.firstFrame1B0=group->first;p.lastFrame1B4=group->last;
    if(registerRow){
        g::OriginalGcParticleEmitterState50KG e{};e.particleGroup=f.particleGroup1B8;e.framesPerSet=f.framesPerSet;
        e.inverseFramesPerSet=f.inverseFramesPerSet;e.gravity=f.gravityMultiplier;e.bouncePlane=f.groundLevel;e.bounceFactor=f.restitution;
        if(row->pub.effect==Effect::fog){e.fogPeriod=f.oscillationPeriod22C;e.fogIncreasing=true;
            e.fogDistanceSquared220=f.distanceSquared220;e.fogInverseDistanceSquared224=f.inverseDistanceSquared224;}
        const auto id=s.manager->registerEmitter(e);
        if(!id)return refuse(d,0x15aea0u,"manager registry insertion failed for placement "+std::to_string(index));
        if(index){const auto* prior=s.placements.at(index-1);
            if(prior&&prior->managerOwnerId&&id!=prior->managerOwnerId+1)
                return refuse(d,0x15aea0u,"manager registry insertion left source order");}
        p.managerOwnerId=id;
    }else if(row->pub.effect==Effect::fog){
        // Restart: the placement's own registered row back to its registration phase.
        auto* e=s.manager->emitter(p.managerOwnerId);
        if(!e)return refuse(d,0x160160u,"fog row unavailable at restart");
        e->fogPhase=0;e->fogPhaseRatio=0;e->fogIncreasing=true;
    }
    if(row->pub.effect==Effect::fog){
        if(row->attached&&!fogPosition(s,p,*row,d))return false;
        const auto r=g::original_rc_fog_numeric_detail50RC::populate(f,*s.manager,p.managerOwnerId,p.currentFrame190,p.controls,
            s.process->rawRandom308030,s.process->sharedRandomSelectors,*s.clock);
        if(!r.okay())return refuse(d,r.sourcePC,"fog cold population (160520) stopped");
    }
    if(row->pub.initiallyOn){g::startEffect50BC(p.controls);p.enabled=true;}
    else {g::stopEffect50BC(p.controls);p.enabled=false;}
    return true;
}
bool receipt(State& s,Placement& p,const g::OriginalGcSprayReceipt50KG& r,ct::Diagnostic& d,const char* what)noexcept {
    if(r.okay())return true;
    p.failed=true;p.stoppedAt=r;s.failed=true;
    return refuse(d,r.sourcePC,std::string(what)+" stopped status="+std::to_string(static_cast<unsigned>(r.status)));
}
bool advancePlacement(State& s,std::size_t i,float dt,Completion completion,ct::Diagnostic& d)noexcept {
    auto* p=s.placements.at(i);const auto* row=p?s.rows.at(p->pub.descriptor):nullptr;
    const auto* defRow=row?s.definitions.at(row->pub.definition):nullptr;
    if(!p||!row||!defRow){s.failed=true;return refuse(d,0x27dc60u,"placement storage lost");}
    const auto& f=defRow->def;const auto effect=row->pub.effect;
    auto& raw=s.process->rawRandom308030;
    // PORT STAND-IN (flagged): the Application pause word 11C is not reachable from
    // Inputs (its view is issued only by the Application); paused=false here and
    // the race loop must not call advance while the Application is paused.
    constexpr bool paused=false;
    if(effect==Effect::fog){
        //160160 phase before common 15A7A0 (no pause gate of its own).
        auto* e=s.manager->emitter(p->managerOwnerId);
        if(!e){s.failed=true;return refuse(d,0x160160u,"fog row unavailable");}
        if(!receipt(s,*p,g::original_rc_fog_numeric_detail50RC::phase(*e,dt),d,"fog phase 160160"))return false;
    }
    g::OriginalGcCommonAttachment50KG attachment{};
    if(row->attached){
        attachment.graph=graphOf(s,*p);attachment.node=p->node;
        for(unsigned k=0;k<4;++k)attachment.offset50[k]=row->offset[k];
    }
    const auto calls=dispatch(*p);
    const auto common=g::advanceOriginalGcCommonEmitter50KG(p->controls,f.timing,p->delay1DC,p->duration1E4,
        row->attached,dt,paused,raw,&calls,row->attached?&attachment:nullptr);
    if(!receipt(s,*p,common,d,"common emitter 15A7A0"))return false;
    if(common.commonSkipped)return true;
    switch(effect){
    case Effect::fog:return true;   // fog has no derived per-frame emission after 15A7A0
    case Effect::smoke:{
        const auto r=g::original_smoke_numeric_detail50RC::emit<0x15bb40u>(*p,f,*s.manager,p->controls,dt,raw,
            s.process->sharedRandomSelectors,*s.clock);
        return receipt(s,*p,r,d,"smoke 15BB40");}
    case Effect::sparks:{
        if(word(p->controls.word19C)>.25f){std::memcpy(p->controls.previous40,p->controls.position30,16);return true;}
        if(row->attached&&completion==Completion::unavailable){s.failed=true;return refuse(d,0x15d8ecu,"attached sparks need a completed placement (unavailable)");}
        if(row->attached&&completion==Completion::stale){p->carriedSize18C=f.initialSize184;p->controls.word19C=0;return true;}
        const auto r=mc_type2_numeric50MC::emitAfterCommon(f,*s.manager,p->managerOwnerId,p->controls,p->currentFrame190,
            p->carriedSize18C,dt,raw);
        return receipt(s,*p,r,d,"sparks 15D938");}
    case Effect::fire:{
        g::water_wake_detail50jc::Math m;std::array<float,3> previous{},delta{},velocity{};
        for(unsigned k=0;k<3;++k){previous[k]=word(p->controls.previous40[k]);delta[k]=m.sub(word(p->controls.position30[k]),previous[k],0x15e128u+8u*k);}
        const auto inverse=m.div(1.f,dt,0x15e140u);
        velocity[1]=m.mul(delta[1],inverse,0x15e160u);velocity[0]=m.mul(delta[0],inverse,0x15e164u);velocity[2]=m.mul(delta[2],inverse,0x15e16cu);
        if(m.badPC){s.failed=true;return refuse(d,m.badPC,"fire velocity arithmetic (15E128)");}
        for(unsigned k=0;k<3;++k)put(p->controls.velocity60[k],velocity[k]);p->controls.velocity60[3]=0x3f800000u;
        if(row->attached&&completion==Completion::unavailable){s.failed=true;return refuse(d,0x15e198u,"attached fire needs a completed placement (unavailable)");}
        if(row->attached&&completion==Completion::stale)return true;
        g::OriginalGcFireEmissionDefinition50KG emission{};
        emission.particleGroup=f.particleGroup1B8;emission.firstFrame=p->firstFrame1B0;emission.lastFrame=p->lastFrame1B4;
        emission.particleLife=f.particleLife17C;emission.initialSize=f.initialSize184;emission.growth=f.growth180;emission.emitRate=f.emitRate188;
        emission.framesPerSet=f.framesPerSet;emission.minRange=f.minRange;emission.maxRange=f.maxRange;emission.authoredVelocityY220=f.velocity60[1];
        emission.particleFlags=f.particleFlags198;emission.randomMirrorX=f.randomMirrorX;emission.randomMirrorY=f.randomMirrorY;
        emission.applySpreadToVelocity=f.applySpreadToVelocity;
        // Native undefined-carry policy true = the GC 15E288 fire update (as every lane).
        const auto r=g::emitOriginalGcFireParticles50KG(emission,*s.manager,p->managerOwnerId,p->fractional224,previous,
            p->controls.previous40[3],velocity,dt,raw,s.process->fireSerial433970,true);
        return receipt(s,*p,r,d,"fire 15E1D0");}
    case Effect::firework:{
        using C=gb_type11_numeric50GB::PlacementCompletion;
        const auto gate=!row->attached?C::Current:completion==Completion::current?C::Current:
            completion==Completion::stale?C::Stale:C::Unavailable;
        const auto r=gb_type11_numeric50GB::afterCommon(f,*s.manager,p->managerOwnerId,p->controls,p->currentFrame190,
            p->firstFrame1B0,p->lastFrame1B4,p->carriedSize18C,dt,gate,raw,s.sparks());
        return receipt(s,*p,r,d,"firework 15F3C0");}
    }
    s.failed=true;return refuse(d,0x27c898u,"effect outside the TypeID switch");
}
// 28C590: node, then (recursively) its next sibling, then its first child.
std::int32_t findChild(const std::vector<a::PsgNode49O>& psg,const g::SceneNode49P* nodes,std::uint32_t count,
    std::int32_t n,const std::string& name,unsigned depth)noexcept {
    if(n<0||static_cast<std::uint32_t>(n)>=count||depth>g::maxSceneNodes49P)return -1;
    const auto& rec=psg[static_cast<std::size_t>(n)];
    if(sameNoCase(std::string_view(rec.name,strnlen(rec.name,sizeof rec.name)),name))return n;
    const auto r=findChild(psg,nodes,count,static_cast<std::int32_t>(nodes[n].nextSibling188),name,depth+1);
    if(r>=0)return r;
    return findChild(psg,nodes,count,static_cast<std::int32_t>(nodes[n].firstChild184),name,depth+1);
}
} // namespace

// ------------------------------------------------------------------ Owner
Owner::Owner()noexcept:state_(new(std::nothrow) State()){}
Owner::~Owner()=default;

bool Owner::prepare(const ct::Track& track,const Inputs& inputs,ct::Diagnostic& d)noexcept {
    if(!state_)return refuse(d,0x27de00u,"emitter owner state allocation failed");
    auto& s=*state_;
    if(!s.thread_())return refuse(d,0x27de00u,"emitter owner used off its thread");
    if(s.attempted)return refuse(d,0x27de00u,"emitter owner prepare is one attempt");
    s.attempted=true;
    try{
        if(!inputs.manager||!inputs.manager->prepared()||!inputs.manager->particles())
            return refuse(d,0x1b4dc0u,"particle manager (1B4DC0) not prepared");
        s.cold=inputs.manager;s.manager=s.cold->particles();s.generation=inputs.generation;
        s.process=inputs.process;s.globals=inputs.globals;
        s.clock=inputs.clock?inputs.clock:(inputs.process?inputs.process->clock101A50:nullptr);
        const std::string code(track.code());
        // ---- 27DE00 rows (the one reader).
        auto table=std::make_unique<pe::Table>();
        if(!pe::read(track.documents.tobDocument,code.c_str(),*table))
            return refuse(d,0x27de00u,"emitter table refused by the common reader (see the particle_emitters line)");
        if(!s.rows.size(ct::BlockId::particleEmitters,table->live,0x27de00u))
            return refuse(d,0x27de00u,"emitter rows exceed the particle_emitters block");
        // ---- classify rows; count distinct definitions and placements.
        std::vector<std::string> names;std::vector<Effect> nameEffects;std::size_t placements=0;
        for(std::size_t i=0;i<table->live;++i){
            const auto& e=table->rows[i];auto* r=s.rows.slot(i,0x27de00u);
            if(!r)return refuse(d,0x27de00u,"emitter row slot");
            r->pub={};r->pub.emitterNumber=e.ordinal;r->pub.typeId=static_cast<std::uint32_t>(e.typeId);
            r->pub.initiallyOn=e.startOn!=0;r->loaded=e.loaded();r->attached=e.attached;
            r->soulTree=e.attachToSoulTree;r->childObject=e.childObject;r->filename=e.filename;
            for(unsigned k=0;k<3;++k)r->offset[k]=e.positionOrOffset[k];r->offset[3]=1;
            if(!r->loaded){
                // 27C7B0 does not load it (the reader logged why); no placements, no definition.
                std::cerr<<"common_track_emitters course="<<code<<" emitter="<<e.ordinal<<" skipped: not loaded by 27C7B0\n";
                r->pub.instances=0;continue;
            }
            if(!effectOf(e.typeId,r->pub.effect))
                return refuse(d,0x27c898u,"emitter "+std::to_string(e.ordinal)+" TypeID "+std::to_string(e.typeId)+
                    " ("+pe::detail::typeName(e.typeId)+") has no native runtime; refused, not guessed");
            if(e.instances<=0)return refuse(d,0x27cbf4u,"loaded emitter without instances");
            r->pub.instances=static_cast<std::uint32_t>(e.instances);
            if(r->pub.instances>ct::blockRow(ct::BlockId::particleAttachments).ceiling-placements)
                return refuse(d,0x27c7b0u,"emitter instances exceed the particle_attachments block");
            placements+=r->pub.instances;
            std::size_t k=0;while(k<names.size()&&!sameNoCase(names[k],r->filename))++k;
            if(k==names.size()){names.push_back(r->filename);nameEffects.push_back(r->pub.effect);}
            else if(nameEffects[k]!=r->pub.effect)
                return refuse(d,0x27c7b0u,"EMT file "+r->filename+" authored with two TypeIDs ("+effectName(nameEffects[k])+", "+effectName(r->pub.effect)+")");
            r->pub.definition=k;
        }
        if(!s.definitions.size(ct::BlockId::particleDefinitions,names.size(),0x15aea0u))
            return refuse(d,0x15aea0u,"distinct EMT files exceed the particle_definitions block");
        if(!s.placements.size(ct::BlockId::particleAttachments,placements,0x27c7b0u))
            return refuse(d,0x27c7b0u,"emitter instances exceed the particle_attachments block");
        // ---- EMT definitions ("particle\\" + EmitterFilename), validated against the
        // manager's actual atlas groups (no atlas extents assumed).
        for(std::size_t k=0;k<names.size();++k){
            auto* dr=s.definitions.slot(k,0x15aea0u);if(!dr)return refuse(d,0x15aea0u,"definition slot");
            dr->filename=names[k];const auto member="particle\\"+names[k];
            ct::Bytes bytes;ct::Diagnostic md;
            if(!track.archive.member(member,bytes,emtCap,md)||bytes.empty())
                return refuse(d,0x15aea0u,"EMT member "+member+" missing or above the 159440 reader bound");
            TextDiagnostic td{};std::string why;
            if(!parseDefinition(bytes,nameEffects[k],dr->def,td,why))
                return refuse(d,td.sourcePC?td.sourcePC:0x15aea0u,"EMT "+member+" rejected: "+why);
            const auto& f=dr->def;const auto* group=s.manager->group(f.particleGroup1B8);
            if(!group||group->first<0||group->first>group->last)
                return refuse(d,0x15aee0u,"EMT "+member+" ParticleGroup "+std::to_string(f.particleGroup1B8)+" is not a manager atlas group");
            // Group type: checked where the kept numerics read it (sparks 15DD98 -> 6)
            // and where the lanes proved it (fire -> 8). Other types: existence only.
            if((f.effect==Effect::sparks&&group->type!=6)||(f.effect==Effect::fire&&group->type!=8))
                return refuse(d,0x15aee0u,"EMT "+member+" ParticleGroup type "+std::to_string(group->type)+" does not match its emitter type");
            std::cerr<<"common_track_emitters course="<<code<<" definition="<<k<<" member="<<member<<" type="<<effectName(f.effect)
                     <<" group="<<f.particleGroup1B8<<" mode="<<f.timing.mode1D8<<'\n';
        }
        // ---- placements: unattached rows use Position; attached rows walk the world
        // instances in world order (27CC4C..27CFBC), StartingZone not filtering.
        const auto& world=track.documents.world;std::size_t idx=0,attached=0;
        for(std::size_t i=0;i<s.rows.size();++i){
            auto* r=s.rows.at(i);if(!r||!r->loaded)continue;
            r->pub.firstAttachment=idx;
            if(!r->attached){
                auto* p=s.placements.slot(idx,0x27c7b0u);if(!p)return refuse(d,0x27c7b0u,"placement slot");
                // Unattached: no world instance (worldInstance=nullptr, worldIndex=SIZE_MAX).
                p->pub={i,1u,none,nullptr};p->graph=none;++idx;continue;
            }
            std::uint32_t matched=0;
            for(std::size_t w=0;w<world.instances.size()&&matched<r->pub.instances;++w){
                const auto& inst=world.instances[w];
                if(inst.parameter_block>=world.parameters.blocks.size())return refuse(d,0x27cc4cu,"world instance parameter block out of range");
                const auto* slt=pe::detail::find(world.parameters.blocks[inst.parameter_block],"slt");
                if(!slt||!sameNoCase(*slt,r->soulTree))continue;
                auto* p=s.placements.slot(idx,0x27cc4cu);if(!p)return refuse(d,0x27cc4cu,"placement slot");
                p->pub={i,matched+1u,w,&inst};++matched;++idx;++attached;
            }
            if(matched<r->pub.instances)
                // 27D6C4 asserts here. PORT STAND-IN (flagged): the 27CFC8 second pass
                // over the World+3C8 dynamic objects is not modelled.
                return refuse(d,0x27d6c4u,"emitter "+std::to_string(r->pub.emitterNumber)+" AttachToSoultree "+r->soulTree+
                    " matched "+std::to_string(matched)+" of "+std::to_string(r->pub.instances)+" world instances");
        }
        if(idx!=s.placements.capacity())return refuse(d,0x27c7b0u,"placement count changed while placing");
        // ---- one scene graph per distinct placed world instance.
        std::size_t distinct=0;
        for(std::size_t i=0;i<idx;++i){const auto* p=s.placements.at(i);if(!p->pub.worldInstance)continue;
            bool prior=false;for(std::size_t j=0;j<i&&!prior;++j)prior=s.placements.at(j)->pub.worldInstance==p->pub.worldInstance;
            if(!prior)++distinct;}
        if(distinct){s.graphs.reset(new(std::nothrow) GraphSlot[distinct]);if(!s.graphs)return refuse(d,0x27cc4cu,"graph storage allocation");}
        for(std::size_t i=0;i<idx;++i){auto* p=s.placements.at(i);if(!p->pub.worldInstance)continue;
            const auto* r=s.rows.at(p->pub.descriptor);
            std::size_t gi=0;while(gi<s.graphCount&&s.graphs[gi].instance!=p->pub.worldInstance)++gi;
            if(gi==s.graphCount){if(gi>=distinct)return refuse(d,0x27cc4cu,"graph slot outside its storage");
                s.graphs[gi].instance=p->pub.worldInstance;s.graphs[gi].worldIndex=p->pub.worldIndex;++s.graphCount;}
            if(!sameNoCase(r->childObject,"NONE"))s.graphs[gi].hierarchy=true;
            p->graph=gi;
        }
        for(std::size_t gi=0;gi<s.graphCount;++gi){
            auto& gs=s.graphs[gi];const auto& inst=*gs.instance;
            std::vector<a::PsgNode49O> psg;std::uint32_t count=1;
            if(gs.hierarchy){
                ct::Bytes bytes;ct::Diagnostic md;a::PsgHierarchyView49O view{};a::GeometryDiagnostic49O gd{};
                if(!track.archive.member(inst.geometry_member,bytes,psgCap,md)||bytes.empty()||
                   !a::readPsgHierarchy49O(bytes.data(),bytes.size(),view,gd)||!view.nodeCount||view.nodeCount>g::maxSceneNodes49P)
                    return refuse(d,0x28c590u,"ChildObject model "+inst.geometry_member+" hierarchy unreadable or above the scene node store");
                count=view.nodeCount;psg.resize(count);
                for(std::uint32_t n=0;n<count;++n)if(!a::readPsgNode49O(view,n,psg[n]))return refuse(d,0x28c590u,"ChildObject PSG node");
                if(psg[0].parent!=-1)return refuse(d,0x28c590u,"ChildObject PSG node 0 is not the root");
            }
            gs.nodes.reset(new(std::nothrow) g::SceneNode49P[count]());if(!gs.nodes)return refuse(d,0x27cc4cu,"graph node allocation");
            for(std::uint32_t n=0;n<count;++n){auto& node=gs.nodes[n];
                if(gs.hierarchy){std::memcpy(node.localC0.v,psg[n].local,sizeof psg[n].local);std::memcpy(node.world100.v,psg[n].local,sizeof psg[n].local);}
                node.parent180=node.firstChild184=node.nextSibling188=g::SceneNodeIndex49P::none;node.sceneFlags190=0;}
            // Child lists in PSG node order (first child = lowest index, siblings ascending).
            for(std::uint32_t n=1;n<count;++n){const auto parent=psg[n].parent;
                if(parent<0||static_cast<std::uint32_t>(parent)>=count||static_cast<std::uint32_t>(parent)==n)
                    return refuse(d,0x28c590u,"ChildObject PSG parent index");
                gs.nodes[n].parent180=static_cast<g::SceneNodeIndex49P>(parent);
                auto* link=&gs.nodes[parent].firstChild184;
                while(*link!=g::SceneNodeIndex49P::none)link=&gs.nodes[static_cast<std::int32_t>(*link)].nextSibling188;
                *link=static_cast<g::SceneNodeIndex49P>(n);
            }
            gs.graph={gs.nodes.get(),count};
            g::BodyMatrix48G matrix{};
            for(unsigned m=0;m<16;++m){matrix.v[m]=static_cast<float>(inst.matrix[m]);
                if(!std::isfinite(matrix.v[m]))return refuse(d,0x289b20u,"placement matrix not finite");}
            if(!g::validateSceneGraph49P(gs.graph)||
               !g::setParentlessWorldMatrix49P(gs.graph,static_cast<g::SceneNodeIndex49P>(0),matrix)||
               !g::refreshSceneWorld49P(gs.graph,static_cast<g::SceneNodeIndex49P>(0)))
                return refuse(d,0x2897b0u,"placement scene graph rejected");
            // ChildObject placements on this graph: 28C590 from the root.
            for(std::size_t i=0;i<idx;++i){auto* p=s.placements.at(i);if(p->graph!=gi)continue;
                const auto* r=s.rows.at(p->pub.descriptor);
                if(sameNoCase(r->childObject,"NONE")){p->node=static_cast<g::SceneNodeIndex49P>(0);continue;}
                const auto found=findChild(psg,gs.nodes.get(),count,0,r->childObject,0);
                if(found<0)return refuse(d,0x28c590u,"ChildObject "+r->childObject+" not found in "+inst.geometry_member);
                p->node=static_cast<g::SceneNodeIndex49P>(found);
            }
        }
        // ---- registry reservation (15AEA0) and the authored start state of every
        // placement in source order.
        if(idx){
            if(!s.process||!s.clock||(inputs.clock&&inputs.process&&inputs.process->clock101A50&&inputs.clock!=inputs.process->clock101A50)||
               &s.process->rawRandom308030==&s.process->fireSerial433970)
                return refuse(d,0x27d7a0u,"emitters need the process RNG 308030, fire serial 433970 and clock 101A50");
            if(std::fegetround()!=FE_TONEAREST)return refuse(d,0x27d7a0u,"rounding mode");
            // PORT STAND-IN (flagged): registry allocation guard. The manager registry is
            // shared with every later racer emitter for the race; the quota covers the
            // registry's own limit (5000 rows) through one growth with old and new
            // blocks both charged. Not an original value.
            constexpr std::size_t quota=3u*g::originalGcParticleCapacity50KG*sizeof(g::OriginalGcParticleEmitterState50KG);
            g::OriginalGcEmitterRegistryReservation50KG reservation{};
            const auto st=s.manager->reserveAdditionalEmitters(idx,quota,reservation);
            if(st==g::OriginalGcParticleStatus50KG::alreadyPrepared)
                std::cerr<<"common_track_emitters course="<<code<<" registry already reserved by another owner; rows share its quota\n";
            else if(st!=g::OriginalGcParticleStatus50KG::complete)
                return refuse(d,0x15aea0u,"manager registry reservation refused status="+std::to_string(static_cast<unsigned>(st)));
            for(std::size_t i=0;i<idx;++i){auto* p=s.placements.at(i);if(!initialize(s,*p,i,true,d))return false;}
        }
        trackDataNamed(track,"particles.emitters",track.identity.tobMember,s.rows.size());
        trackDataNamed(track,"particles.attachments",track.identity.tobMember,s.placements.size());
        trackDataNamed(track,"particles.definitions",track.identity.tobMember,s.definitions.size());
        std::cerr<<"common_track_emitters course="<<code<<" rows="<<s.rows.size()<<" definitions="<<s.definitions.size()
                 <<" placements="<<s.placements.size()<<" attached="<<attached<<" graphs="<<s.graphCount
                 <<" sparks_globals="<<(s.globals?"process":"owner(PORT STAND-IN)")<<'\n';
        s.prepared=true;return true;
    }catch(const std::exception& e){try{return refuse(d,0x27de00u,e.what());}catch(...){return false;}}
     catch(...){return refuse(d,0x27de00u,"emitter owner prepare exception");}
}
bool Owner::ready()const noexcept {return state_&&state_->live();}
std::size_t Owner::emitterCount()const noexcept {return state_&&state_->prepared&&!state_->closed?state_->rows.size():0;}
std::size_t Owner::attachedCount()const noexcept {return state_&&state_->prepared&&!state_->closed?state_->placements.size():0;}
std::size_t Owner::definitionCount()const noexcept {return state_&&state_->prepared&&!state_->closed?state_->definitions.size():0;}
const Descriptor* Owner::descriptor(std::size_t row)const noexcept {
    if(!state_||!state_->prepared||state_->closed)return nullptr;const auto* r=state_->rows.at(row);return r?&r->pub:nullptr;
}
const Attachment* Owner::attachment(std::size_t placement)const noexcept {
    if(!state_||!state_->prepared||state_->closed)return nullptr;const auto* p=state_->placements.at(placement);return p?&p->pub:nullptr;
}
bool Owner::applyTriggerAction(std::uint32_t emitterNumber,std::uint32_t instanceNumber,bool turnOn,ct::Diagnostic& d)noexcept {
    if(!state_||!state_->live())return refuse(d,0x15a500u,"trigger Particles action reached an emitter owner that is not live");
    auto& s=*state_;
    const Row* row=nullptr;std::size_t rowIndex=0;
    for(std::size_t i=0;i<s.rows.size()&&!row;++i){const auto* r=s.rows.at(i);if(r&&r->pub.emitterNumber==emitterNumber){row=r;rowIndex=i;}}
    if(!row||!emitterNumber)return refuse(d,0x15a500u,"trigger Particles action names unknown emitter "+std::to_string(emitterNumber));
    if(!row->pub.instances)return refuse(d,0x15a500u,"trigger Particles action names emitter "+std::to_string(emitterNumber)+" that 27C7B0 did not load");
    if(instanceNumber>row->pub.instances)
        return refuse(d,0x15a500u,"trigger Particles action names instance "+std::to_string(instanceNumber)+" of emitter "+
            std::to_string(emitterNumber)+" ("+std::to_string(row->pub.instances)+" instances)");
    const auto* defRow=s.definitions.at(row->pub.definition);
    if(!defRow)return refuse(d,0x15a500u,"trigger Particles action row lost its definition");
    const std::size_t first=row->pub.firstAttachment+(instanceNumber?instanceNumber-1u:0u);
    const std::size_t count=instanceNumber?1u:row->pub.instances;
    for(std::size_t i=first;i<first+count;++i){auto* p=s.placements.at(i);
        if(!p||p->pub.descriptor!=rowIndex)return refuse(d,0x15a500u,"trigger Particles action placement outside its row");}
    for(std::size_t i=first;i<first+count;++i){auto& p=*s.placements.at(i);
        if(turnOn){
            //15A500: virtual 88 (15AD68 delay, 15AD78..80 duration), 80 start, 20 enable.
            p.delay1DC=defRow->def.timing.delayTime1E0;put(p.duration1E4,defRow->def.timing.maxTimeOn1EC);
            g::startEffect50BC(p.controls);p.enabled=true;
        }else{
            //15A560: virtual 84 stop, 18 disable.
            g::stopEffect50BC(p.controls);p.enabled=false;
        }
    }
    return true;
}
bool Owner::advance(float dt,std::span<const Completion> completed,ct::Diagnostic& d)noexcept {
    if(!state_||!state_->live())return refuse(d,0x161180u,"emitter advance outside the owner's live lifetime");
    auto& s=*state_;
    if(completed.size()<s.placements.size())
        return refuse(d,0x27dc60u,"completion span "+std::to_string(completed.size())+" shorter than the "+
            std::to_string(s.placements.size())+" placements");
    if(!std::isfinite(dt)||!(dt>0))return refuse(d,0x161180u,"frame time must be positive and finite");
    if(!s.managerCurrent()){s.failed=true;return refuse(d,0x161180u,"particle manager closed or replaced");}
    // 161180 once (pause: see advancePlacement's stand-in), then every enabled
    // placement in source order.
    const auto mr=s.manager->advance(dt,false);
    if(!mr.okay()){s.failed=true;return refuse(d,0x161180u,"manager advance 161180 stopped status="+std::to_string(static_cast<unsigned>(mr.status)));}
    for(std::size_t i=0;i<s.placements.size();++i){const auto* p=s.placements.at(i);
        if(!p||!p->enabled)continue;
        if(!advancePlacement(s,i,dt,completed[i],d))return false;}
    return true;
}
bool Owner::restart161F60(ct::Diagnostic& d)noexcept {
    if(!state_||!state_->live())return refuse(d,0x161f60u,"emitter restart outside the owner's live lifetime");
    auto& s=*state_;
    if(!s.managerCurrent())return refuse(d,0x161f60u,"particle manager closed or replaced");
    // The manager's own 161F60 removal through its cold owner (World+4A0), then
    // every placement back to its authored start state.
    {
        std::uint32_t pc=0x161f60u;
        if(!s.cold||!s.cold->restartRemoval161F60(pc)){s.failed=true;return refuse(d,pc,"manager 161F60 restart removal refused");}
    }
    for(std::size_t i=0;i<s.placements.size();++i){auto* p=s.placements.at(i);
        if(!p||!initialize(s,*p,i,false,d)){s.failed=true;return false;}}
    return true;
}
bool Owner::quiesce()noexcept {
    if(!state_)return true;
    if(!state_->thread_())return false;
    state_->quiesced=true;return true;
}
bool Owner::closeAfterPool()noexcept {
    if(!state_)return true;
    auto& s=*state_;
    if(!s.thread_())return false;
    if(s.closed)return true;
    if(s.prepared&&!s.quiesced)return false;          // quiesce first
    if(s.cold&&!s.cold->closed())return false;        // only after the manager pool closed
    s.manager=nullptr;
    for(std::size_t i=0;i<s.graphCount;++i){s.graphs[i].graph={nullptr,0};s.graphs[i].nodes.reset();}
    s.graphs.reset();s.graphCount=0;
    s.placements.release();s.definitions.release();s.rows.release();
    s.cold.reset();s.process.reset();s.globals.reset();s.clock.reset();
    s.closed=true;return true;
}

} // namespace track_particle_emitters50CR
