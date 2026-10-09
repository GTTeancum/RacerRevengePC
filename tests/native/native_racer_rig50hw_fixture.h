#pragma once
// Cold, authentic per-racer PSG/MCF/VUT/MOT owner. All views borrow this stable
// nonmovable object; snapshot banks copy only bindings and mutable frame state.
// No registry fallback, Anakin motion substitution, or rigid model transform.
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "pod_scene_rebase50fb.h"
#include "effect_names50bl.h"
#include "json.hpp"
#include <map>
#include <string>
#include <utility>

namespace native_racer50HW {
namespace g=rr::game;
namespace a=rr::assets;
namespace f=runtime_startup50fl;
using J=nlohmann::json;
using Index=g::SceneNodeIndex49P;

namespace rig_detail {
inline void need(bool value,const std::string& why){if(!value)throw std::runtime_error(why);}
inline std::string lower(std::string text){for(char& c:text){if(c>='A'&&c<='Z')c=char(c+32);if(c=='\\')c='/';}return text;}
inline std::string trim(std::string text){
    const auto first=text.find_first_not_of(" \t\r\n");if(first==std::string::npos)return {};
    text=text.substr(first,text.find_last_not_of(" \t\r\n")-first+1);
    if(text.size()>=2&&text.front()=='"'&&text.back()=='"')text=text.substr(1,text.size()-2);
    return text;
}
using Ini=std::map<std::string,std::map<std::string,std::string>>;
inline Ini ini(const std::vector<std::uint8_t>& bytes){
    need(!bytes.empty()&&bytes.size()<=65536,"Roster authoring text extent");
    const std::string text(bytes.begin(),bytes.end());Ini result;std::string section;
    for(std::size_t start=0;start<text.size();){
        auto end=text.find('\n',start);if(end==std::string::npos)end=text.size();
        const auto line=trim(text.substr(start,end-start));start=end+1;
        if(line.empty()||line[0]==';')continue;
        if(line.front()=='['){need(line.size()>2&&line.back()==']',"Roster malformed authoring section");
            section=lower(trim(line.substr(1,line.size()-2)));
            need(result.emplace(section,std::map<std::string,std::string>{}).second,"Roster duplicate authoring section");
        }else{const auto equal=line.find('=');need(!section.empty()&&equal!=std::string::npos,"Roster malformed authoring field");
            need(result[section].emplace(lower(trim(line.substr(0,equal))),trim(line.substr(equal+1))).second,
                "Roster duplicate authoring field");}
    }
    return result;
}
inline const std::string& value(const Ini& source,const std::string& section,const std::string& key){
    const auto s=source.find(section);need(s!=source.end(),"Roster missing section "+section);
    const auto k=s->second.find(key);need(k!=s->second.end(),"Roster missing field "+section+"/"+key);return k->second;
}
inline J json(const std::filesystem::path& path){std::ifstream stream(path);need(bool(stream),"Roster metadata missing: "+path.string());J out;stream>>out;return out;}
inline std::string withExtension(std::string member,const char* extension){
    const auto dot=member.find_last_of('.');need(dot!=std::string::npos,"Roster member extension missing");return member.substr(0,dot)+extension;
}
}

class Rig final {
public:
    // The retained/native asset order used by runtime_startup50fl.
    static constexpr unsigned left=0,right=1,chariot=2;
    enum Motion : unsigned {EngineLeft,EngineRight,BrakeLeft,BrakeRight,Forward,Back,Mid,EngineRightMid,EngineLeftMid,MotionCount};
    std::filesystem::path directory,resourceFile;
    std::string podId,name;
    J manifest;
    std::vector<std::uint8_t> phy;
    std::array<std::vector<std::uint8_t>,3> psg,vut,mcf;
    std::array<std::vector<std::uint8_t>,MotionCount> mot;
    std::array<a::PsgHierarchyView49O,3> hierarchy{};
    std::array<a::MotionNodeBindings49O,3> localRows{},rows{};
    std::array<a::MotionView49O,MotionCount> motions{};
    std::array<Index,3> roots{};
    Index sourceNodes[3][a::maxModelNodes49O]{};
    g::PodSceneRebaseModel50FB rebaseModels[3]{};
    g::FreshPodGeometryResult49O geometry{};
    std::uint32_t nodeCount=0;
    Index physical=Index::none,main=Index::none,container=Index::none,hidden=Index::none;

    Rig(const std::filesystem::path& repository,const std::filesystem::path& podJson){
        using namespace rig_detail;
        const auto file=podJson.is_absolute()?podJson:repository/podJson;
        directory=file.parent_path();const auto pod=json(file);manifest=json(directory/"import.json");
        podId=pod.at("pod_id").get<std::string>();name=pod.at("name").get<std::string>();
        need(podId.size()==5&&podId.substr(0,3)=="POD"&&podId[3]>='0'&&podId[3]<='9'&&podId[4]>='0'&&podId[4]<='9',"Roster pod identity");
        resourceFile=repository/"work/native/project/disc/PODS"/podId/(podId+".RES");
        struct Member {std::filesystem::path path;std::string digest;std::size_t bytes;};
        std::map<std::string,Member> members;
        for(const auto& entry:manifest.at("members")){
            const auto path=std::filesystem::path(entry.at("source_file").get<std::string>());
            need(!path.is_absolute()&&path==path.filename(),"Roster imported member path escapes owner");
            need(members.emplace(lower(entry.at("source_member").get<std::string>()),Member{directory/path,entry.at("source_sha256").get<std::string>(),entry.at("source_bytes").get<std::size_t>()}).second,
                "Roster duplicate source member");
        }
        auto read=[&](const std::string& member){const auto found=members.find(lower(member));
            need(found!=members.end(),"Roster source member missing: "+member);
            auto bytes=f::readBytes(found->second.path);
            need(bytes.size()==found->second.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==found->second.digest,
                "Roster retained original member identity differs: "+member);return bytes;};
        phy=read(pod.at("phy").at("source_member").get<std::string>());
        const auto setup=ini(phy);const auto prefix=value(setup,"pod setup","prefix");
        need(prefix==podId.substr(3),"Roster PHY prefix identity differs");
        const std::string base="pods/"+lower(podId)+"/";
        const char* roles[]={"leftengine","rightengine","chariot"};
        const char* suffixes[]={"engineleft","engineright","brakeleft","brakeright","forward","back","mid","enginerightmid","engineleftmid"};
        const unsigned motionModels[]={left,right,chariot,chariot,chariot,chariot,chariot,right,left};
        std::uint32_t next=0;
        for(unsigned model=0;model<3;++model){
            const auto selected=base+lower(value(setup,"pod setup",roles[model]));
            psg[model]=read(withExtension(selected,".psg"));
            a::GeometryDiagnostic49O gd{};
            need(a::readPsgHierarchy49O(psg[model].data(),psg[model].size(),hierarchy[model],gd),"Roster native PSG admission");
            roots[model]=static_cast<Index>(next++);
            need(hierarchy[model].nodeCount<=a::maxModelNodes49O&&next+hierarchy[model].nodeCount+4<=g::vehicleFrameNodeCount49W,
                "Roster forest exceeds native scene capacity");
            for(unsigned n=0;n<hierarchy[model].nodeCount;++n)sourceNodes[model][n]=static_cast<Index>(next+n);
            next+=hierarchy[model].nodeCount;
            mcf[model]=read(withExtension(selected,".mcf"));const auto config=ini(mcf[model]);
            std::size_t consumed=0;const auto count=std::stoul(value(config,"general info","numberofmotions"),&consumed);
            need(consumed==value(config,"general info","numberofmotions").size()&&count<=MotionCount,"Roster MCF motion count");
            const auto content=lower(value(config,"general info","contentdirectory"));
            need(content==base,"Roster MCF content directory differs from selected pod");
            if(count){
                vut[model]=read(content+lower(value(config,"general info","vutfilename")));
                a::MotionBindingDiagnostic49O bd{};
                need(a::readMotionNodeBindings49O(hierarchy[model],reinterpret_cast<const char*>(vut[model].data()),vut[model].size(),localRows[model],bd),"Roster native VUT admission");
                rows[model]=localRows[model];
                for(unsigned n=0;n<rows[model].count;++n)
                    rows[model].rows[n].sceneNode=static_cast<std::uint8_t>(sourceNodes[model][localRows[model].rows[n].sceneNode]);
            }
            // NumberOfMotions=0 is authentic. Its absent VUT/MOT stays absent;
            // the procedural engine response still owns its real PSG subtree.
            for(unsigned n=1;n<=count;++n){
                const auto section="motion "+std::to_string(n);
                const auto identity=lower(value(config,section,"name"));unsigned slot=MotionCount;
                for(unsigned i=0;i<MotionCount;++i)if(identity==prefix+"_"+suffixes[i])slot=i;
                need(slot<MotionCount&&motionModels[slot]==model&&!motions[slot].data,"Roster unexpected/duplicate MCF motion identity");
                mot[slot]=read(content+withExtension(lower(value(config,section,"vue")),".mot"));
                need(a::readMot49O(mot[slot].data(),mot[slot].size(),motions[slot],gd),"Roster native MOT admission");
                for(unsigned channel=0;channel<2;++channel){const auto tracks=channel?motions[slot].positionCount:motions[slot].rotationCount;
                    for(unsigned t=0;t<tracks;++t)need((channel?motions[slot].position[t]:motions[slot].rotation[t]).rowIndex<rows[model].count,"Roster MOT/VUT row binding");}
            }
        }
        physical=static_cast<Index>(next++);main=static_cast<Index>(next++);
        container=static_cast<Index>(next++);hidden=static_cast<Index>(next++);nodeCount=next;
        need(motions[Forward].data&&motions[Back].data,"Roster chariot Forward/Back unavailable");
        const unsigned order[]={chariot,right,left};
        for(unsigned i=0;i<3;++i){const auto model=order[i];rebaseModels[i]={roots[model],hierarchy[model],sourceNodes[model],hierarchy[model].nodeCount};}
        const auto sources=geometrySources();g::FreshPodGeometryWorkspace49O work{};g::FreshPodGeometryDiagnostic49O diagnostic{};
        need(g::buildFreshPodGeometry49O(sources,work,geometry,diagnostic),"Roster source-derived fresh geometry");
    }
    Rig(const Rig&)=delete;Rig& operator=(const Rig&)=delete;
    Rig(Rig&&)=delete;Rig& operator=(Rig&&)=delete;

    const a::MotionView49O* motion(Motion which)const noexcept{return motions[which].data?&motions[which]:nullptr;}
    const a::MotionView49O& forward()const noexcept{return motions[Forward];}
    const a::MotionView49O& back()const noexcept{return motions[Back];}
    g::FreshPodGeometrySources49O geometrySources()const noexcept{
        return {hierarchy[chariot],hierarchy[right],motions[Forward],motions[Back],localRows[chariot],motion(Mid)};
    }
    Index find(unsigned model,const char* wanted)const{
        using namespace rig_detail;Index found=Index::none;
        for(unsigned row=0;row<hierarchy[model].nodeCount;++row){a::PsgNode49O node{};
            need(a::readPsgNode49O(hierarchy[model],row,node),"Roster source node read");
            if(lower(node.name)==lower(wanted)){need(found==Index::none,"Roster ambiguous named source node");found=sourceNodes[model][row];}}
        return found;
    }
    // Installs a FRESH per-racer graph before any frame. Keeps the body's caller-
    // selected grid/orientation context, but derives all model and IK nodes from
    // this PSG. Call handling bind, physical-pose setup and rebase afterward.
    void install(g::VehicleFrameState49W& state,g::VehicleFrameBindings49W& bindings,
        g::ChariotIkPair49T& hands)const{
        using namespace rig_detail;using namespace g;
        need(state.phase==VehicleFramePhase49W::Ready&&state.bodies.main.frameFactor==0,"Roster rig install requires cold state");
        const auto previousMain=state.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
        const auto previousPhysical=state.nodes[static_cast<unsigned>(bindings.lifecycle.physical19C)];
        const auto previousContainer=state.nodes[static_cast<unsigned>(bindings.lifecycle.container194)];
        const auto previousHidden=state.nodes[static_cast<unsigned>(bindings.lifecycle.hidden198)];
        for(auto& node:state.nodes){node={};node.localC0=node.world100=f::identity();}
        for(unsigned model=0;model<3;++model){
            state.nodes[static_cast<unsigned>(roots[model])].sceneFlags190=0x80;
            for(unsigned row=0;row<hierarchy[model].nodeCount;++row){
                a::PsgNode49O source{};need(a::readPsgNode49O(hierarchy[model],row,source),"Roster graph PSG node");
                const auto index=sourceNodes[model][row];auto& node=state.nodes[static_cast<unsigned>(index)];
                std::memcpy(node.localC0.v,source.local,sizeof source.local);node.sceneFlags190=0xa4;
                node.parent180=source.parent<0?roots[model]:sourceNodes[model][source.parent];
                auto& parent=state.nodes[static_cast<unsigned>(node.parent180)];
                if(parent.firstChild184==Index::none)parent.firstChild184=index;
                else{auto sibling=parent.firstChild184;while(state.nodes[static_cast<unsigned>(sibling)].nextSibling188!=Index::none)
                    sibling=state.nodes[static_cast<unsigned>(sibling)].nextSibling188;
                    state.nodes[static_cast<unsigned>(sibling)].nextSibling188=index;}
            }
        }
        // Source engine-nullcg normalization precedes graphics response. Fresh
        // mass/target geometry above consumed the untouched PSG values first.
        for(unsigned model:{left,right})for(unsigned lane=12;lane<15;++lane)
            state.nodes[static_cast<unsigned>(sourceNodes[model][0])].localC0.v[lane]=0;
        auto owner=[&](Index index,const SceneNode49P& original){auto& target=state.nodes[static_cast<unsigned>(index)];target=original;
            target.parent180=target.firstChild184=target.nextSibling188=Index::none;};
        owner(main,previousMain);owner(physical,previousPhysical);owner(container,previousContainer);owner(hidden,previousHidden);
        state.nodes[static_cast<unsigned>(main)].firstChild184=container;
        state.nodes[static_cast<unsigned>(container)].parent180=main;
        const auto leftCable=find(chariot,"chariotcableleft1"),rightCable=find(chariot,"chariotcableright1");
        need(leftCable!=Index::none&&rightCable!=Index::none,"Roster lifecycle cable anchors");
        bindings.lifecycle={main,container,hidden,physical,roots[chariot],roots[left],roots[right],leftCable,rightCable};
        bindings.visual.main190=main;bindings.visual.container194=container;
        bindings.visual.left1BC=roots[left];bindings.visual.right1B8=roots[right];
        auto& animation=bindings.animation;animation={};
        animation.chariot1B4=roots[chariot];animation.left1BC=roots[left];animation.right1B8=roots[right];animation.bodyScene288=main;
        animation.chariotRows=&rows[chariot];animation.leftRows=rows[left].count?&rows[left]:nullptr;animation.rightRows=rows[right].count?&rows[right]:nullptr;
        animation.brakeLeft28C=motion(BrakeLeft);animation.brakeRight290=motion(BrakeRight);
        animation.forward280=motion(Forward);animation.back284=motion(Back);
        animation.left2A0=motion(EngineLeft);animation.right29C=motion(EngineRight);
        animation.mid288=motion(Mid);animation.rightMid294=motion(EngineRightMid);animation.leftMid298=motion(EngineLeftMid);
        auto chain=[&](const char* wristName,const char* targetName){
            const auto wrist=find(chariot,wristName),target=find(chariot,targetName);
            need(wrist!=Index::none&&target!=Index::none,"Roster original hand anchors");
            const auto elbow=state.nodes[static_cast<unsigned>(wrist)].parent180;
            need(elbow!=Index::none&&elbow!=roots[chariot],"Roster original elbow parent");
            const auto shoulder=state.nodes[static_cast<unsigned>(elbow)].parent180;
            need(shoulder!=Index::none&&shoulder!=roots[chariot],"Roster original shoulder parent");
            return ChariotIkChainBinding49T{shoulder,elbow,wrist,target};
        };
        animation.hands={chain("handlt","l_hand_dummy"),chain("handrt","r_hand_dummy")};animation.handRecords=&hands;
        state.animation={};state.animationShared={};
        need(initializeVehicleAnimationFilters49T(0,0,state.animation),"Roster cold animation filters");
        state.sceneNodeCount=bindings.sceneNodeCount=nodeCount;
        const SceneGraph49P graph{state.nodes,state.sceneNodeCount};
        ChariotIkWorkspace49T ikWork{};ChariotIkDiagnostic49T ikDiagnostic{};
        need(initializeChariotIk49T(animation.hands,graph,hands,state.animationShared.libm,ikWork,ikDiagnostic),"Roster original pre-motion IK constructors");
        // Original213830 then213854. These mutate only this model's source rows.
        ModelMotionWorkspace49T motionWork{};ModelMotionDiagnostic49T motionDiagnostic{};
        need(sampleModelMotionBlend49T(motions[Forward],motions[Back],rows[chariot],.5f,0,
            graph,state.animation.chariotModel,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold Forward/Back sample");
        if(motion(Mid))need(sampleModelMotion49T(motions[Mid],rows[chariot],.5f,graph,
            state.animation.chariotModel,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold Mid sample");
        // Original21385C..213A20 initializes right then left engine: optional
        // primary motion at0, identity root orientation, optional Mid at.5.
        // Absent engine records do not cause an invented sampler call.
        for(unsigned model:{right,left}){
            const auto primary=model==right?EngineRight:EngineLeft;
            const auto middle=model==right?EngineRightMid:EngineLeftMid;
            auto& modelState=model==right?state.animation.rightModel:state.animation.leftModel;
            if(motion(primary))need(sampleModelMotion49T(motions[primary],rows[model],0,
                graph,modelState,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold engine sample");
            need(setLocalMatrix49P(graph,roots[model],f::identity()),"Roster cold engine root orientation");
            if(motion(middle))need(sampleModelMotion49T(motions[middle],rows[model],.5f,
                graph,modelState,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold engine Mid sample");
        }
        // Display root carries the current secondary pose; never collapse the
        // per-node hierarchy or replace future native animation with this pose.
        need(setLocalMatrix49P(graph,roots[chariot],previousPhysical.localC0),"Roster initial chariot display pose");
        for(unsigned i=0;i<vehicleFrameNodeCount49W;++i)
            bindings.topology[i]={state.nodes[i].parent180,state.nodes[i].firstChild184,state.nodes[i].nextSibling188};
        f::project(state,bindings);
    }
    void buildEffectNames(const g::VehicleFrameState49W& state,a::EffectNames50BL& names)const{
        a::ModelRootName50BL rootName{};a::initializeFreshModelRootName50BL(rootName);
        a::EffectNameModel50BL models[3]{};
        for(unsigned model=0;model<3;++model)models[model]={roots[model],&rootName,hierarchy[model],sourceNodes[model],hierarchy[model].nodeCount};
        rig_detail::need(a::buildEffectNames50BL({const_cast<g::SceneNode49P*>(state.nodes),state.sceneNodeCount},models,3,names),"Roster effect source-name scope");
    }
};
}
