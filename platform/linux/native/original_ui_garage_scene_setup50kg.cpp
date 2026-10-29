#include "original_ui_garage_scene_setup50kg.h"
#include "../../../src/game/original_world_environment_history50la.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_main_session50kg.h"
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <limits>
#include <stdexcept>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
using Section=assets::OriginalUiDatSection50KG;
bool failure(D& d,S status,std::uint32_t pc)noexcept{d={};d.status=status;d.sourcePC=pc;return false;}
const assets::OriginalUiDatValue50KG* present(const Section& s,const std::string& key)noexcept{
    const auto* v=s.find(key);return v&&v->sourceValuePresent?v:nullptr;
}
std::string text(const Section& s,const std::string& key,const char* fallback){
    const auto* v=present(s,key);return v?v->text:std::string(fallback);
}
bool integer(const Section& s,const std::string& key,std::int32_t fallback,std::int32_t& out,D& d,std::uint32_t pc){
    assets::OriginalUiPageDiagnostic50KG p;
    return assets::originalUiInteger50KG(s,key,fallback,out,p,pc)||failure(d,S::source,pc);
}
bool number(const Section& s,const std::string& key,float fallback,float& out,D& d,std::uint32_t pc){
    assets::OriginalUiPageDiagnostic50KG p;
    return assets::originalUiNumber50KG(s,key,fallback,out,p,pc)||failure(d,S::source,pc);
}
bool boolean(const Section& s,const std::string& key,bool fallback,bool& out,D& d,std::uint32_t pc){
    assets::OriginalUiPageDiagnostic50KG p;
    return assets::originalUiBoolean50KG(s,key,fallback,out,p,pc)||failure(d,S::source,pc);
}
bool vector3(const Section& s,const std::string& key,const std::array<float,3>& fallback,
    std::array<float,4>& out,D& d,std::uint32_t pc){
    assets::OriginalUiPageDiagnostic50KG p;std::array<float,3> xyz;
    if(!assets::originalUiTriple50KG(s,key,fallback,xyz,p,pc))return failure(d,S::source,pc);
    for(std::size_t i=0;i<3;++i)out[i]=xyz[i];
    return true; //158C00 never writesw.
}
bool color(const std::string& value,std::array<float,4>& out,D& d,std::uint32_t pc){
    //26FA60's output is integer-valued floatRGBA, NEVER normalized RGB.
    out={255,255,255,255};if(value=="undef"||value=="none"||value.empty())return true;
    std::size_t begin=0,lane=0;
    while(begin<value.size()&&lane<4){
        const auto end=value.find(',',begin);const auto token=value.substr(begin,end==std::string::npos?end:end-begin);
        errno=0;char* tail=nullptr;const long v=std::strtol(token.c_str(),&tail,10);
        while(tail&&(*tail==' '||*tail=='\t'))++tail;
        //Native bounded admission, not an assertion about malformed retail atoi.
        if(errno==ERANGE||tail==token.c_str()||!tail||*tail||v<std::numeric_limits<std::int32_t>::min()||
           v>std::numeric_limits<std::int32_t>::max())return failure(d,S::source,pc);
        out[lane++]=static_cast<float>(v);if(end==std::string::npos)return true;begin=end+1;
    }
    return begin>=value.size()?true:failure(d,S::source,pc);
}
std::int32_t searchType(const std::string& value)noexcept{return value=="QuadTree"?2:value=="List"?1:0;}
bool sameFog(const std::array<float,4>& a,float av,float ah,const std::array<float,4>& b,float bv,float bh)noexcept{
    return a==b&&av==bv&&ah==bh;
}
}
void OriginalUiGarageSceneSetup50KG::Text260::set(const std::string& value){
    if(value.size()>=bytes.size())throw std::length_error("260-byte source text");
    std::memcpy(bytes.data(),value.c_str(),value.size()+1);defined=true;
}
OriginalUiGarageSceneSetup50KG::Environment::Environment(){terrain0C.set("none");water110.set("none");}
OriginalUiGarageSceneSetup50KG::Zone::Zone(){name04.set("StaticModels");}
OriginalUiGarageInstanceMaster50KG::OriginalUiGarageInstanceMaster50KG()
    :records00_(new std::uint8_t[10u*0x108u]){}
OriginalUiGarageInstanceMaster50KG::OriginalUiGarageInstanceMaster50KG(SourceTag)noexcept {
    sourceMode_=true;
    //Actual2BFEA0. No constructed Instance records, callbacks or Entity count.
    capacity04_=10; //2BFEAC
    growth08_=100000; //2BFEB0
    used0C_=0; //2BFEB4
    try{records00_.reset(new std::uint8_t[10u*0x108u]);}catch(...){records00_.reset();}
    sourceArrayReturned_=true; //2BFED8 publishes raw00, includingNULL.
    //Enclosing caller still owes global2BFD24 THEN count10 store2BFD28.
}
void OriginalUiGarageInstanceMaster50KG::close2BFD40()noexcept{records00_.reset();}
OriginalUiGarageSceneSetup50KG::~OriginalUiGarageSceneSetup50KG()noexcept{
    //Normal Page/Scene Lights retirement is NOT established by this owner.
    //A caller must explicitly execute the detached partial-construction path.
    if(!closed_||(lights3A8_&&lights3A8_->entityLive))std::terminate();
}
bool OriginalUiGarageSceneSetup50KG::readStartData275BC0(const Dat& file,D& d){
    if(!startData348_)return failure(d,S::phase,0x275BC0);
    const auto* info=file.section("SceneInfo");
    if(!info){
        //2729B8 deletes the actual newly allocated record when slot8 fails.
        startData348_.reset();return failure(d,S::source,0x275BE0);
    }
    auto& start=*startData348_;
    const std::array<float,3> positionDefault{{start.position00[0],start.position00[1],start.position00[2]}};
    const std::array<float,3> directionDefault{{start.direction10[0],start.direction10[1],start.direction10[2]}};
    return vector3(*info,"DefaultStartPosition",positionDefault,start.position00,d,0x275C0C)&&
        vector3(*info,"DefaultStartDirection",directionDefault,start.direction10,d,0x275C28);
}
bool OriginalUiGarageSceneSetup50KG::readEnvironment276C00(const Dat& file,std::int32_t zones,D& d){
    const auto* s=file.section("Environment");if(!s){environment3A4_.reset();return failure(d,S::source,0x276C00);}
    auto& e=*environment3A4_;
    if(!integer(*s,"NumberOfSkyCubes",1,e.cubeCount00,d,0x276C3C)||e.cubeCount00<0||e.cubeCount00>64)
        return failure(d,S::source,0x276C3C);
    if(e.cubeCount00>0){
        e.allocatedCubeRows=static_cast<std::size_t>(e.cubeCount00);
        e.cubeFiles04=std::make_unique<std::unique_ptr<char[]>[]>(e.allocatedCubeRows);
        for(std::size_t i=0;i<e.allocatedCubeRows;++i){
            auto name=text(*s,"CubeFile"+std::to_string(i+1),"none");
            if(i==0&&!present(*s,"CubeFile1"))name=text(*s,"CubeFile","none");
            if(name.size()>=260)return failure(d,S::source,0x276D14);
            e.cubeFiles04[i]=std::unique_ptr<char[]>(new char[260]);
            std::memcpy(e.cubeFiles04[i].get(),name.c_str(),name.size()+1);
        }
        if(zones>0){
            rr::game::original_world_environment50KG::EnvironmentHistory50LA::invalidateCubeNative();
            e.zoneCube08=std::make_unique<std::int32_t[]>(static_cast<std::size_t>(zones));
            for(std::int32_t i=0;i<zones;++i){std::int32_t cube=1;
                if(!integer(*s,"CubeFileForZone"+std::to_string(i+1),1,cube,d,0x276DDC)||cube<1||cube>e.cubeCount00)
                    return failure(d,S::source,0x276DDC);
                e.zoneCube08[i]=cube-1;
            }
        }
        if(e.cubeCount00==1&&std::strcmp(e.cubeFiles04[0].get(),"none")==0){
            e.cubeFiles04[0].reset();e.cubeCount00=0; //Outer AND zone arrays survive.
        }
    }
    e.ecosystem318.set(text(*s,"EcosystemFile","none"));
    //Non-none needs the original namespace/suffix loader, not an ignored name.
    if(std::strcmp(e.ecosystem318.get(),"none"))return failure(d,S::source,0x276ED0);
    e.terrain0C.set(text(*s,"TerrainFile","none"));
    if(!number(*s,"TerrainWidth",0,e.width520,d,0x276F34)||
       !number(*s,"TerrainBreadth",0,e.breadth524,d,0x276F50)||
       !number(*s,"TerrainWidthScale",0,e.widthScale528,d,0x276F6C)||
       !number(*s,"TerrainBreadthScale",0,e.breadthScale52C,d,0x276F88)||
       !number(*s,"TerrainBorderWidth",0,e.border530,d,0x276FA4))return false;
    //Force source binary32 operation order, including default negative zero.
    volatile float x=e.width520*e.widthScale528;volatile float xs=256.f*x;
    volatile float z=e.breadth524*e.breadthScale52C;volatile float zs=256.f*z;
    const float xmax=e.border530+xs,zmax=e.border530+zs;
    if(!number(*s,"Xmin",-e.border530,e.xmin534,d,0x276FD0)||
       !number(*s,"Xmax",xmax,e.xmax538,d,0x27700C)||
       !number(*s,"Zmin",-e.border530,e.zmin53C,d,0x277034)||
       !number(*s,"Zmax",zmax,e.zmax540,d,0x277070))return false;
    const float dx=e.xmax538-e.xmin534,dz=e.zmax540-e.zmin53C;
    rr::game::original_world_environment50KG::EnvironmentHistory50LA::invalidateBoundsNative();
    if(!std::isfinite(dx)||!std::isfinite(dz)||std::fabs(dx)<1.f||std::fabs(dz)<1.f)
        return failure(d,S::source,0x27708C);
    float distance=0;if(!number(*s,"EcoDistance2D",180,distance,d,0x277198))return false;e.ecoDistance2D544=distance;
    if(!number(*s,"EcoDistance3D",120,distance,d,0x2771B4))return false;e.ecoDistance3D548=distance;
    e.water110.set(text(*s,"WaterFile","none"));e.particle41C.set(text(*s,"ParticleTextureMapName","none"));return true;
}
bool OriginalUiGarageSceneSetup50KG::readLights278290(const Dat& file,D& d){
    const auto* s=file.section("Lights");if(!s)return failure(d,S::source,0x278290);
    auto& lights=*lights3A8_;
    if(!integer(*s,"NumberOfLights",0,lights.count30,d,0x278290)||lights.count30<0||lights.count30>64)
        return failure(d,S::source,0x278290);
    //A real allocated array exists even at source count0 (cookie-backed there).
    lights.lights38=std::unique_ptr<Light[]>(new Light[static_cast<std::size_t>(lights.count30)]);
    for(std::int32_t i=0;i<*lights.zoneCount34;++i){
        const auto* value=present(*s,"DirectionalLightColorForZone"+std::to_string(i+1));
        if(!value)lights.zoneColors3C[i].lanes[0]=-256.f;
        else{std::array<float,4> rgba;if(!color(value->text,rgba,d,0x278290))return false;
            for(std::size_t lane=0;lane<4;++lane)lights.zoneColors3C[i].lanes[lane]=rgba[lane];}
    }
    for(std::int32_t i=0;i<lights.count30;++i){
        auto& wrapper=lights.lights38[i];wrapper.temp04=std::make_unique<LightTemp>();auto& l=*wrapper.temp04;
        const auto* row=file.section("Light"+std::to_string(i+1));
        if(!row)return failure(d,S::source,0x277390);
        const auto type=text(*row,"Type","none");
        l.type00=type=="Ambient"?4:type=="Directional"?2:type=="ObjectPoint"?1:type=="Spotlight"?3:4;
        if(!color(text(*row,"ColorRGB","none"),l.color04,d,0x2774B4)||
           !integer(*row,"InitialZone",1,l.zone20,d,0x2774EC))return false;
        if(l.zone20==std::numeric_limits<std::int32_t>::min())return failure(d,S::source,0x2774F4);
        --l.zone20;if(l.type00==4)continue;
        l.position14=std::make_unique<std::array<float,4>>(std::array<float,4>{0,0,0,1});
        l.look18=std::make_unique<std::array<float,4>>(std::array<float,4>{0,0,0,1});
        l.attenuation1C=std::make_unique<std::array<float,4>>(std::array<float,4>{0,0,0,1});
        if(!vector3(*row,"Position",{0,0,0},*l.position14,d,0x277578))return false;
        if(present(*row,"LookVector")){
            if(!vector3(*row,"LookVector",{0,0,0},*l.look18,d,0x277598))return false;
        }else if(present(*row,"TargetPosition")){
            std::array<float,4> target{0,0,0,1};
            if(!vector3(*row,"TargetPosition",{0,0,0},target,d,0x2775CC))return false;
            for(std::size_t lane=0;lane<3;++lane)(*l.look18)[lane]=target[lane]-(*l.position14)[lane];
        }
        if(!vector3(*row,"Attenuation",{.03f,.002f,.001f},*l.attenuation1C,d,0x277390)||
           !number(*row,"Range",0,l.range24,d,0x277390))return false;
        float angle=0;
        if(!number(*row,"InnerConeAngle",30,angle,d,0x277390))return false;l.innerCone2C=angle;
        if(!number(*row,"OuterConeAngle",45,angle,d,0x277390))return false;l.outerCone30=angle;
        const std::array<const char*,7> keys{{"PlaceLightAtInfinity","CastsShadows","EmitsLight","HasLensFlare",
            "HasLightFlare","HasFlasher","ShowDebugSphere"}};
        for(std::size_t bit=0;bit<keys.size();++bit){bool enabled=false;
            if(!boolean(*row,keys[bit],bit<3,enabled,d,0x277390))return false;
            const auto mask=static_cast<std::uint8_t>(1u<<bit);
            l.flags28=static_cast<std::uint8_t>((l.flags28&~mask)|(enabled?mask:0));
        }
        if(l.flags28&8u){
            if(!color(text(*row,"LensFlareColorRGB","undef"),l.lensColor3C,d,0x277390)||
               !number(*row,"LensFlareBrightness",1,l.lensBrightness4C,d,0x277390))return false;
            const auto layers=text(*row,"LensFlareLayersVisible","T,T,T,T,T");
            std::size_t start=0,lane=0;l.lensLayers34.fill(0);
            while(start<layers.size()&&lane<5){
                const auto end=layers.find(',',start);const auto token=layers.substr(start,end==std::string::npos?end:end-start);
                if(token!="T"&&token!="F")return failure(d,S::source,0x277390);
                l.lensLayers34[lane++]=static_cast<std::uint8_t>(token=="T");
                if(end==std::string::npos){start=layers.size();break;}start=end+1;
            }
            if(start<layers.size())return failure(d,S::source,0x277390);
            l.lensTexture50=text(*row,"LensFlareTextureMapName","none");
            if(l.lensTexture50.size()>=64)return failure(d,S::source,0x277390);
        }
        //The admitted Garage has neither branch. Do not pretend unknown
        //flare/flasher resource/runtime owners have been implemented.
        if(l.flags28&0x30u)return failure(d,S::source,0x277390);
    }
    return true;
}
bool OriginalUiGarageSceneSetup50KG::readFog278570(const Dat& file,D& d){
    const auto* s=file.section("Fog");if(!s){fog3AC_.reset();return failure(d,S::source,0x278570);}
    auto& fog=*fog3AC_;
    return color(text(*s,"ColorRGB","undef"),fog.color00,d,0x278570)&&
        number(*s,"Visibility",1,fog.visibility10,d,0x278570)&&
        number(*s,"Haziness",.5f,fog.haziness14,d,0x278570);
}
bool OriginalUiGarageSceneSetup50KG::readZones275E30(const Dat& file,D& d){
    if(!fog3AC_)return failure(d,S::phase,0x275E30);
    for(std::size_t i=0;i<zoneCount_;++i){auto& zone=zones350_[i];zone.index00=static_cast<std::int32_t>(i);
        const std::string zoneName="Zone"+std::to_string(i+1);
        const auto* s=file.section(zoneName);bool fallback=false;
        if(!s&&i==0){s=file.section("StaticModels");fallback=s!=nullptr;}
        //Retail returns1 with constructor state; native continuation must not
        //advertise a populated zone when its required block was not selected.
        if(!s)return failure(d,S::source,0x275E30);
        zone.name04.set(fallback?"StaticModels":zoneName);
        zone.flags148=static_cast<std::uint8_t>((zone.flags148&~4u)|(fallback?4u:0u));
        bool visible=true;if(!boolean(*s,"EnvironmentNodeVisible",true,visible,d,0x275E30))return false;
        zone.flags148=static_cast<std::uint8_t>((zone.flags148&~2u)|(visible?2u:0u));
        if(!integer(*s,"NumberOfStaticModels",0,zone.staticCount108,d,0x275E30)||zone.staticCount108<0)
            return failure(d,S::source,0x275E30);
        if(const auto* mode=present(*s,"SearchType"))zone.search110.fill(searchType(mode->text));
        else{
            if(const auto* mode=present(*s,"VisibilitySearchType"))zone.search110[0]=zone.search110[2]=searchType(mode->text);
            else{zone.search110[2]=searchType(text(*s,"StaticVisibilitySearchType","QuadTree"));
                zone.search110[0]=searchType(text(*s,"DynamicVisibilitySearchType","QuadTree"));}
            if(const auto* mode=present(*s,"CollisionSearchType")){
                zone.search110[1]=searchType(mode->text);zone.search110[2]=zone.search110[0]; //Original quirk.
            }else{zone.search110[3]=searchType(text(*s,"StaticCollisionSearchType","QuadTree"));
                zone.search110[1]=searchType(text(*s,"DynamicCollisionSearchType","QuadTree"));}
        }
        for(const auto mode:zone.search110)if(!mode)return failure(d,S::source,0x276530);
        const std::array<const char*,4> keys{{"Xmin","Xmax","Zmin","Zmax"}};
        for(std::size_t j=0;j<4;++j){float bound=0;if(!number(*s,keys[j],0,bound,d,0x275E30))return false;zone.bounds14C[j]=bound;}
        if(!integer(*s,"ZoneReverb",0,zone.reverb128,d,0x275E30))return false;
        const auto c=text(*s,"FogColorRGB","undef");
        if(c=="undef")zone.fog12C.color00=fog3AC_->color00;
        else if(!color(c,zone.fog12C.color00,d,0x275E30))return false;
        if(!number(*s,"FogVisibility",fog3AC_->visibility10,zone.fog12C.visibility10,d,0x275E30)||
           !number(*s,"FogHaziness",fog3AC_->haziness14,zone.fog12C.haziness14,d,0x275E30))return false;
        if(!sameFog(zone.fog12C.color00,zone.fog12C.visibility10,zone.fog12C.haziness14,
            fog3AC_->color00,fog3AC_->visibility10,fog3AC_->haziness14))perZoneFog4F0_=true;
        //No Garage exclusion list. Nonempty branches need their complete
        //source tokenization and invalid-index contract, never ignored entries.
        if(present(*s,"ExclusionList"))return failure(d,S::source,0x275E30);
        const auto* collision=file.section(zoneName);if(!collision&&i==0)collision=file.section("CollisionObjects");
        std::int32_t count=-1;
        if(collision&&!integer(*collision,"NumberOfCollisionObjects",-1,count,d,0x275E30))return false;
        if(count==-1){zone.collisionCount10C=zone.staticCount108;zone.flags148=static_cast<std::uint8_t>(zone.flags148&~1u);}
        else{if(count<0)return failure(d,S::source,0x275E30);zone.collisionCount10C=count;}
    }
    return true;
}
void OriginalUiGarageSceneSetup50KG::initLists27C110(std::int32_t zones){
    lists3D4_=std::make_unique<Lists>();auto& list=*lists3D4_;list.count00=zones;
    list.capacities04=std::make_unique<std::int32_t[]>(static_cast<std::size_t>(zones));
    list.used08=std::make_unique<std::int32_t[]>(static_cast<std::size_t>(zones));
    list.rows0C=std::make_unique<std::unique_ptr<C*[]>[]>(static_cast<std::size_t>(zones));
}
bool OriginalUiGaragePodScene50KG::prepareSceneStructure270900(D& d)noexcept{
    d={};const auto* file=liveSceneConfig34();
    if(sceneStructureAttempted_||loadStage_!=LoadStage::sceneInfo||!file||!nativeApplication_||
       !grant_||!grant_->current()||sceneSetup_||numZones34C_!=1||numPortals358_!=0||useEnvironment35C_!=0)
        return failure(d,S::phase,0x270A48);
    sceneStructureAttempted_=true;
    const auto stopped=[&](){configFailure_=true;return false;};
    const auto fail=[&](S status,std::uint32_t pc){configFailure_=true;return failure(d,status,pc);};
    using Setup=OriginalUiGarageSceneSetup50KG;
    try{
        sceneSetup_=std::unique_ptr<Setup>(new Setup());auto& setup=*sceneSetup_;
        //270A50 slotF8 ->272970 is unconditional, even with NumPortals0.
        //It precedes Environment3A4, Lights3A8, Fog3AC and Zone350 allocation.
        const auto application=grant_->application();
        if(!application)return fail(S::phase,0x272980);
        setup.startData348_=std::make_unique<Setup::StartData>(
            application->worldOrigin442F80_,application->worldForward442FB0_);
        if(!setup.readStartData275BC0(*file,d))return stopped();
        setup.environment3A4_=std::make_unique<Setup::Environment>();
        if(!setup.readEnvironment276C00(*file,numZones34C_,d))return stopped();
        setup.lights3A8_=std::make_unique<Setup::Lights>();
        auto& lights=*setup.lights3A8_;
        //278170(a1=1) constructs one actual shared-authority Entity, not a
        //counter adjustment, fake guest vtable or live Light/render object.
        const auto made=grant_->constructPrefix(lights.entity,K::entity141300);
        if(!made.okay())return fail(S::phase,made.sourcePC);
        lights.entityLive=true;lights.zoneCount34=numZones34C_;
        lights.zoneColors3C=std::unique_ptr<Setup::DirectionalColor[]>(new Setup::DirectionalColor[numZones34C_]);
        if(!file->section("Lights")){
            //Proven270AF8 failed-read virtual10 release. This native helper
            //combines Entity reference/base retirement; no Page teardown claim.
            const auto released=grant_->retireEmptyPrefix(lights.entity,K::entity141300,nullptr);
            if(!released.okay())return fail(S::phase,released.sourcePC);
            lights.entityLive=false;setup.lights3A8_.reset();return fail(S::source,0x278290);
        }
        if(!setup.readLights278290(*file,d))return stopped();
        setup.fog3AC_=std::make_unique<Setup::Fog>();
        if(!setup.readFog278570(*file,d))return stopped();
        setup.zoneCount_=static_cast<std::size_t>(numZones34C_);
        setup.zones350_=std::unique_ptr<Setup::Zone[]>(new Setup::Zone[setup.zoneCount_]);
        if(!setup.readZones275E30(*file,d))return stopped();
        setup.initLists27C110(numZones34C_);
        //Slot84=270FD0 is unconditional, even with no InstanceMasters block.
        //Retain any displaced original raw allocation for Application lifetime.
        //Never reuse an old object or silently destroy it on global overwrite.
        setup.instanceMaster_=std::shared_ptr<OriginalUiGarageInstanceMaster50KG>(new OriginalUiGarageInstanceMaster50KG());
        if(nativeApplication_->instanceMaster444078)
            nativeApplication_->overwrittenInstanceMasters.push_back(nativeApplication_->instanceMaster444078);
        nativeApplication_->instanceMaster444078=setup.instanceMaster_;
        if(const auto* masters=file->section("InstanceMasters")){
            if(!integer(*masters,"NumberOfInstanceMasters",0,setup.instanceCount3E0_,d,0x271028))return stopped();
        }
        if(setup.instanceCount3E0_!=0)return fail(S::source,0x27103C);
        setup.complete_=true;loadStage_=LoadStage::sceneStructure;return true;
    }catch(...){return fail(S::allocation,0x270900);}
}
bool OriginalUiGaragePodScene50KG::readScenePrefix270900(D& d)noexcept{
    return readSceneInfo270900Prefix(d)&&prepareSceneStructure270900(d);
}
bool OriginalUiGaragePodScene50KG::closeSceneStructurePartial270900(D& d)noexcept{
    d={};if(!sceneSetup_)return true;auto& setup=*sceneSetup_;
    if(setup.closed_)return true;
    //Explicit detached-construction fallback only. There is no assertion here
    //that normal Page/Scene retirement reaches3A8 or orders these releases.
    if(setup.lights3A8_){auto& lights=*setup.lights3A8_;
        if(lights.entityLive){
            if(!grant_||!grant_->current()||lights.entity.parent1C||lights.entity.previous10||
               lights.entity.next14||lights.entity.firstChild18||lights.entity.reference00!=1)
                return failure(d,S::phase,0x142590);
            for(std::int32_t i=0;i<lights.count30;++i)
                if(lights.lights38&&lights.lights38[i].liveLight00)return failure(d,S::phase,0x277C30);
        }
    }
    if(setup.lists3D4_){const auto& list=*setup.lists3D4_;
        for(std::int32_t i=0;i<list.count00;++i)
            if((list.used08&&list.used08[i])||(list.capacities04&&list.capacities04[i])||
               (list.rows0C&&list.rows0C[i]))return failure(d,S::phase,0x27C330);
    }
    if(setup.lights3A8_&&setup.lights3A8_->entityLive){
        //Host authority's empty-prefix primitive combines104970/1413E0.
        //This fallback is NOT represented as source normal destruction order.
        const auto released=grant_->retireEmptyPrefix(setup.lights3A8_->entity,K::entity141300,nullptr);
        if(!released.okay())return failure(d,S::phase,released.sourcePC);
        setup.lights3A8_->entityLive=false;
    }
    setup.lights3A8_.reset(); //Wrapper temps/vectors then arrays2781B0.
    setup.startData348_.reset(); //Detached partial cleanup, not normal26FDEC proof.
    setup.zones350_.reset();setup.zoneCount_=0;
    setup.environment3A4_.reset();setup.fog3AC_.reset();setup.lists3D4_.reset();
    setup.instanceDescriptors3DC_.reset();
    if(setup.instanceMaster_&&nativeApplication_){
        //26FEF8 targets the CURRENT global, not a per-Scene backpointer. Old
        //overwritten allocations remain retained rather than retrospectively
        //being given a destructor call that the source did not make.
        const auto current=nativeApplication_->instanceMaster444078;
        if(current)current->close2BFD40();
        nativeApplication_->instanceMaster444078.reset();
    }
    setup.instanceMaster_.reset();setup.complete_=false;setup.closed_=true;
    sceneSetup_.reset();configFailure_=true;return true;
}
}
