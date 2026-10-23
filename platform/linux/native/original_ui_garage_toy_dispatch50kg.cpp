#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_dispatch50kg.h"
#include "original_ui_main_session50kg.h"
#include <cstring>
#include <exception>
#include <stdexcept>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
using Dat=assets::OriginalUiDat50KG;
using Section=assets::OriginalUiDatSection50KG;
bool toyFailure(D& d,S status,std::uint32_t pc)noexcept{d={};d.status=status;d.sourcePC=pc;return false;}
const assets::OriginalUiDatValue50KG* toyValue(const Section& s,const std::string& key)noexcept{
    const auto* value=s.find(key);return value&&value->sourceValuePresent?value:nullptr;
}
std::string toyText(const Section& s,const std::string& key,const char* fallback){
    const auto* value=toyValue(s,key);return value?value->text:std::string(fallback);
}
bool toyInteger(const Section& s,const std::string& key,std::int32_t fallback,std::int32_t& out,D& d,std::uint32_t pc){
    assets::OriginalUiPageDiagnostic50KG diagnostic;
    return assets::originalUiInteger50KG(s,key,fallback,out,diagnostic,pc)||toyFailure(d,S::source,pc);
}
bool toyCount(const Dat& file,const char* section,const char* key,std::int32_t& out,D& d,std::uint32_t pc){
    const auto* s=file.section(section);out=0;return !s||toyInteger(*s,key,0,out,d,pc);
}
bool noSoultree(const char* value)noexcept{
    const char expected[]="NONE";
    for(std::size_t i=0;i<4;++i){unsigned char c=static_cast<unsigned char>(value[i]);
        if(c>='a'&&c<='z')c=static_cast<unsigned char>(c-'a'+'A');
        if(c!=static_cast<unsigned char>(expected[i]))return false;
    }
    return value[4]=='\0';
}
}
void OriginalUiGarageToyFields50KG::Text260::set(const std::string& value){
    if(value.size()>=bytes.size())throw std::length_error("260-byte emitter field");
    std::memcpy(bytes.data(),value.c_str(),value.size()+1);
}
OriginalUiGarageToyFields50KG::EmitterTemp::EmitterTemp(){
    //Constructor3BA330 is lowercase none; missing read defaults3BB488 are NONE.
    attachToSoultree08.set("none");emitterFilename210.set("none");
}
OriginalUiGarageToyFields50KG::~OriginalUiGarageToyFields50KG()noexcept{
    if(!closed_||(triggers3D0_&&triggers3D0_->live)||(particles3D8_&&particles3D8_->live))std::terminate();
}
bool OriginalUiGaragePodScene50KG::finishToyDispatch271CF0(D& d)noexcept{
    d={};const auto* file=liveToyConfig34();
    if(loadStage_!=LoadStage::toyDescriptors||!file||!sceneDispatch_||!sceneDispatch_->terrainComplete||
       !actorOwner3C8_||!nativeProfile_||!grant_||!grant_->current()||toyFields_)
        return toyFailure(d,S::phase,0x271D3C);
    using Fields=OriginalUiGarageToyFields50KG;
    const auto fail=[&](S status,std::uint32_t pc){configFailure_=true;return toyFailure(d,status,pc);};
    const auto stopped=[&](){configFailure_=true;return false;};
    try{
        toyFields_=std::unique_ptr<Fields>(new Fields());auto& out=*toyFields_;
        //SlotA8:1B5000 unconditionally constructs the genuine2DC320 Entity,
        //even though Garage lacks Triggers and immediately destroys it again.
        out.triggers3D0_=std::make_unique<Fields::TriggerList>();
        auto made=grant_->constructPrefix(out.triggers3D0_->entity,K::entity141300);
        if(!made.okay())return fail(S::phase,made.sourcePC);
        out.triggers3D0_->live=true;
        if(const auto* triggers=file->section("Triggers")){
            if(!toyInteger(*triggers,"NumberOfTriggers",0,out.triggers3D0_->count30,d,0x2DC4A8))return stopped();
            if(out.triggers3D0_->count30!=0)return fail(S::source,0x2DC4B0);
        }else{
            const auto retired=grant_->retireEmptyPrefix(out.triggers3D0_->entity,K::entity141300,nullptr);
            if(!retired.okay())return fail(S::phase,retired.sourcePC);
            out.triggers3D0_->live=false;out.triggers3D0_.reset(); //1B5070.
        }
        //SlotAC: source does not allocate4B4 unless the real read count is>0.
        if(!toyCount(*file,"PhysicsObjects","NumberOfPhysicsObjects",out.physicsCount4B0_,d,0x2718F8))return stopped();
        if(out.physicsCount4B0_!=0)return fail(S::source,0x27190C);
        //SlotB8 is actual integer camera-zone descriptors, not live Cameras.
        if(!toyCount(*file,"CSCameras","NumberOfCameras",out.cameraCount3E4_,d,0x2723B8))return stopped();
        if(out.cameraCount3E4_!=0)return fail(S::source,0x2723CC);
        //SlotC0:1B46E0 consults the ACTUAL retained live profile at this call.
        //The read branch is retained even with zero SunFlares, not forced on.
        out.sunFlareModeA1_=((nativeProfile_->playerBits1125C>>5)&3u)!=1u;
        if(!toyCount(*file,"SunFlares","NumberOfSunFlares",out.sunFlareCount4A8_,d,0x271E48))return stopped();
        if(out.sunFlareCount4A8_!=0)return fail(S::source,0x271E5C);
        //SlotC4:2722F0 ->27DC90, distinct actual list Entity at3D8.
        out.particles3D8_=std::make_unique<Fields::ParticleList>();auto& particles=*out.particles3D8_;
        made=grant_->constructPrefix(particles.entity,K::entity141300);
        if(!made.okay())return fail(S::phase,made.sourcePC);
        particles.live=true;
        const auto* emitters=file->section("ParticleEmitters");
        if(!emitters){
            const auto retired=grant_->retireEmptyPrefix(particles.entity,K::entity141300,nullptr);
            if(!retired.okay())return fail(S::phase,retired.sourcePC);
            particles.live=false;out.particles3D8_.reset(); //272364 on empty failed read.
        }else{
            if(!toyInteger(*emitters,"NumberOfEmitters",0,particles.count30,d,0x27DE64))return stopped();
            if(particles.count30!=0&&particles.count30!=7)return fail(S::source,0x27DE6C);
            if(particles.count30>0){
                particles.records34=std::make_unique<Fields::EmitterWrapper*[]>(static_cast<std::size_t>(particles.count30));
                for(std::int32_t i=0;i<particles.count30;++i){
                    const auto* row=file->section("ParticleEmitter"+std::to_string(i+1));
                    if(!row)return fail(S::source,0x27DEF0);
                    auto& wrapper=particles.records[static_cast<std::size_t>(i)];
                    wrapper=std::make_unique<Fields::EmitterWrapper>();particles.records34[i]=wrapper.get();
                    wrapper->temp04=std::make_unique<Fields::EmitterTemp>();auto& temp=*wrapper->temp04;
                    if(!toyInteger(*row,"StartingZone",0,temp.startingZone00,d,0x27C628)||
                       !toyInteger(*row,"TypeID",0,temp.typeID04,d,0x27C640))return stopped();
                    temp.attachToSoultree08.set(toyText(*row,"AttachToSoultree","NONE"));
                    std::array<float,3> xyz{};assets::OriginalUiPageDiagnostic50KG textDiagnostic;
                    if(noSoultree(temp.attachToSoultree08.get())){
                        if(!assets::originalUiTriple50KG(*row,"Position",{},xyz,textDiagnostic,0x27C694))
                            return fail(S::source,0x27C694);
                        temp.numInstances330=1; //Unattached ignores NumberOfInstances.
                    }else{
                        const auto child=toyText(*row,"ChildObject","NONE");
                        if(child.size()>=260)return fail(S::source,0x27C6C0);
                        temp.childObject10C=child;
                        if(!assets::originalUiTriple50KG(*row,"OffsetFromSoultree",{},xyz,textDiagnostic,0x27C6DC))
                            return fail(S::source,0x27C6DC);
                        std::int32_t instances=1;
                        if(!toyInteger(*row,"NumberOfInstances",1,instances,d,0x27C6F4))return stopped();
                        temp.numInstances330=instances;
                    }
                    for(std::size_t lane=0;lane<3;++lane)temp.position320[lane]=xyz[lane]; //w remains1.
                    temp.emitterFilename210.set(toyText(*row,"EmitterFilename","NONE"));
                    bool start=true;
                    if(!assets::originalUiBoolean50KG(*row,"StartOn",true,start,textDiagnostic,0x27C72C))
                        return fail(S::source,0x27C72C);
                    temp.startOn334=start;
                }
            }
        }
        //SlotC8 precedes B0. Both counts are read through the live Config34.
        if(!toyCount(*file,"Buoys","NumberOfBuoys",out.buoyCount538_,d,0x1B50E0))return stopped();
        if(out.buoyCount538_!=0)return fail(S::source,0x1B50F4);
        if(!toyCount(*file,"StaticModelParams","NumberOfStaticModelParams",out.staticParameterReadCount_,d,0x271A4C))return stopped();
        if(out.staticParameterReadCount_!=0)return fail(S::source,0x271A5C);
        out.complete_=true;loadStage_=LoadStage::toyTail;return true;
    }catch(...){return fail(S::allocation,0x271CF0);}
}
OriginalUiGaragePodScene50KG::C* OriginalUiGaragePodScene50KG::pageEntity3D8()noexcept{
    if(configFailure_||loadStage_<LoadStage::toyTail||!currentPrefix()||!grant_||!grant_->current()||
       !toyFields_||toyFields_->closed_||!toyFields_->complete_||!toyFields_->particles3D8_)
        return nullptr;
    auto& list=*toyFields_->particles3D8_;auto& entity=list.entity;
    if(!list.live||entity.kind!=K::entity141300||entity.reference00!=1||
       entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||entity.mask20||entity.descendants24)
        return nullptr;
    if(list.count30!=0&&list.count30!=7)return nullptr;
    for(std::int32_t i=0;i<list.count30;++i){const auto& row=list.records[static_cast<std::size_t>(i)];
        if(!list.records34||!row||list.records34[i]!=row.get()||row->live00||!row->temp04||
           !row->temp04->numInstances330||!row->temp04->startOn334)return nullptr;
    }
    return &entity; //Only this actual list; not Scene/Page readiness.
}
bool OriginalUiGaragePodScene50KG::closeToyFieldsPartial271CF0(D& d)noexcept{
    d={};
    if(!toyFields_)return true;
    auto& out=*toyFields_;
    if(out.closed_)return true;
    const auto detached=[&](const C& entity){return entity.kind==K::entity141300&&entity.reference00==1&&
        entity.sessionGeneration==grant_->generation_&&
        entity.objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))&&
        !entity.parent1C&&!entity.previous10&&!entity.next14&&!entity.firstChild18&&!entity.mask20&&!entity.descendants24;};
    if(!grant_||!grant_->current())return toyFailure(d,S::phase,0x142590);
    if(out.triggers3D0_&&out.triggers3D0_->live&&(!detached(out.triggers3D0_->entity)||
       out.triggers3D0_->records34))return toyFailure(d,S::phase,0x2DC360);
    if(out.particles3D8_&&out.particles3D8_->live){
        if(!detached(out.particles3D8_->entity))return toyFailure(d,S::phase,0x27DCD0);
        for(const auto& row:out.particles3D8_->records)if(row&&row->live00)return toyFailure(d,S::phase,0x27DCD0);
    }
    //Detached constructor-abort only. No normal Page/Scene destructor ordering
    //is claimed. Actual native temp/wrapper/pointer arrays precede Entity base.
    if(out.particles3D8_){auto& list=*out.particles3D8_;
        for(auto& row:list.records){row.reset();}
        list.records34.reset();
        if(list.live){const auto retired=grant_->retireEmptyPrefix(list.entity,K::entity141300,nullptr);
            if(!retired.okay())return toyFailure(d,S::phase,retired.sourcePC);
            list.live=false;
        }
        out.particles3D8_.reset();
    }
    if(out.triggers3D0_){auto& list=*out.triggers3D0_;list.records34.reset();
        if(list.live){const auto retired=grant_->retireEmptyPrefix(list.entity,K::entity141300,nullptr);
            if(!retired.okay())return toyFailure(d,S::phase,retired.sourcePC);
            list.live=false;
        }
        out.triggers3D0_.reset();
    }
    out.cameraZones3E8_.reset();out.physics4B4_.reset();out.sunFlares4A4_.reset();out.buoys53C_.reset();
    out.complete_=false;out.closed_=true;toyFields_.reset();configFailure_=true;return true;
}
}
