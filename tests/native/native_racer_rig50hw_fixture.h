#pragma once
// Cold, authentic per-racer PSG/MCF/VUT/MOT owner. All views borrow this stable
// nonmovable object; snapshot banks copy only bindings and mutable frame state.
// No registry fallback, Anakin motion substitution, or rigid model transform.
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "pod_scene_rebase50fb.h"
#include "effect_names50bl.h"
#include "json.hpp"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include <set>
#include <map>
#include <cstddef>
#include <string>
#include <type_traits>
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
inline const char* bindingErrorName(a::MotionBindingError49O error) noexcept {
    switch(error){
        case a::MotionBindingError49O::none:return "none";
        case a::MotionBindingError49O::hierarchy:return "hierarchy";
        case a::MotionBindingError49O::name:return "name";
        case a::MotionBindingError49O::limit:return "limit";
        case a::MotionBindingError49O::line:return "line";
        case a::MotionBindingError49O::unknownNode:return "unknownNode";
    }
    return "unknown-error";
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
inline J json(const std::filesystem::path& requested){const auto path=runtime_asset_paths50LB::resolve(requested);std::ifstream stream(path);need(bool(stream),"Roster metadata missing: "+path.string());J out;stream>>out;return out;}
inline std::string withExtension(std::string member,const char* extension){
    const auto dot=member.find_last_of('.');need(dot!=std::string::npos,"Roster member extension missing");return member.substr(0,dot)+extension;
}
}

class Rig final {
public:
    // The retained/native asset order used by runtime_startup50fl.
    static constexpr unsigned left=0,right=1,chariot=2,bottomLeft=3,bottomRight=4,maxModels=5;
    unsigned modelCount=3;
    enum Motion : unsigned {EngineLeft,EngineRight,BrakeLeft,BrakeRight,Forward,Back,Mid,EngineRightMid,EngineLeftMid,MotionCount};
    std::filesystem::path directory,resourceFile;
    std::string podId,name;
    J manifest;
    // Direct-disc ownership retains the native RES directory and decompressed
    // bytes at this Rig's stable address. Metadata below describes this actual
    // archive, never a historical importer generation or an on-disk cache.
    bool directOriginal=false;
    a::ResArchive48C originalArchive;
    std::string displayName;
    const a::ResourceEntry48C* originalEntry(const std::string& member)const {
        const auto wanted=rig_detail::lower(member);const a::ResourceEntry48C* found=nullptr;
        for(const auto& entry:originalArchive.entries)if(rig_detail::lower(entry.name)==wanted){
            rig_detail::need(!found,"Original pod archive member is ambiguous: "+member);found=&entry;}
        return found;
    }
    std::vector<std::uint8_t> originalMember(const std::string& member)const {
        rig_detail::need(directOriginal,"Direct original member requires an archive-backed Rig");
        const auto* entry=originalEntry(member);
        rig_detail::need(entry,"Original pod archive member missing: "+member);
        std::vector<std::uint8_t> bytes;a::DecodeError48C error;
        rig_detail::need(a::extractResMember48C(originalArchive,entry->name,bytes,error),
            "Original pod member extraction failed: "+member+" "+error.code);
        return bytes;
    }
    std::vector<std::uint8_t> phy;
    std::array<std::vector<std::uint8_t>,maxModels> psg,vut,mcf;
    // Exact authenticated archive-member names for subsequent model lifetime
    // construction. These belong to this same Rig, not a second parsed model.
    std::array<std::string,maxModels> psgMembers,mcfMembers;
    std::array<std::vector<std::uint8_t>,MotionCount> mot;
    // Exact original spelling used by the shared151270 motion library. The
    // numeric slot remains only this Rig's existing gameplay binding.
    std::array<std::string,MotionCount> motMembers,motionNames;
    std::array<a::PsgHierarchyView49O,maxModels> hierarchy{};
    std::array<a::MotionNodeBindings49O,maxModels> localRows{},rows{};
    std::array<a::MotionBindingDiagnostic49O,maxModels> bindingDiagnostics{};
    std::array<a::MotionView49O,MotionCount> motions{};
    std::array<Index,maxModels> roots{};
    Index sourceNodes[maxModels][a::maxModelNodes49O]{};
    g::PodSceneRebaseModel50FB rebaseModels[maxModels]{};
    g::FreshPodGeometryResult49O geometry{};
    std::uint32_t nodeCount=0;
    Index physical=Index::none,main=Index::none,container=Index::none,hidden=Index::none;

    Rig(const std::filesystem::path& repository,const std::filesystem::path& podJson);
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
            need(a::readPsgNode49O(hierarchy[model],row,node),"Roster "+podId+" model="+std::to_string(model)+" source node="+std::to_string(row));
            if(lower(node.name)==lower(wanted)){need(found==Index::none,"Roster "+podId+" model="+std::to_string(model)+" ambiguous source node "+wanted);found=sourceNodes[model][row];}}
        return found;
    }
    // Installs a FRESH per-racer graph before any frame. Keeps the body's caller-
    // selected grid/orientation context, but derives all model and IK nodes from
    // this PSG. Call handling bind, physical-pose setup and rebase afterward.
    template<class RightModelConstruction=std::nullptr_t,class LeftModelConstruction=std::nullptr_t,
        class ChariotModelConstruction=std::nullptr_t,class BottomRightModelConstruction=std::nullptr_t,
        class BottomLeftModelConstruction=std::nullptr_t>
    void install(g::VehicleFrameState49W& state,g::VehicleFrameBindings49W& bindings,
        g::ChariotIkPair49T& hands,RightModelConstruction rightModel=nullptr,
        LeftModelConstruction leftModel=nullptr,ChariotModelConstruction chariotModel=nullptr,
        BottomRightModelConstruction bottomRightModel=nullptr,BottomLeftModelConstruction bottomLeftModel=nullptr,
        bool preserveSharedMotion=false)const{
        using namespace rig_detail;using namespace g;
        need(state.phase==VehicleFramePhase49W::Ready&&state.bodies.main.frameFactor==0,"Roster rig install requires cold state");
        need(state.fields.cableMode48.construct20F6B4(),"Roster racer48 cold constructor already projected");
        const auto previousMain=state.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
        const auto previousPhysical=state.nodes[static_cast<unsigned>(bindings.lifecycle.physical19C)];
        const auto previousContainer=state.nodes[static_cast<unsigned>(bindings.lifecycle.container194)];
        const auto previousHidden=state.nodes[static_cast<unsigned>(bindings.lifecycle.hidden198)];
        for(auto& node:state.nodes){node={};node.localC0=node.world100=f::identity();}
        // GB has already sampled its four world owners against this exact
        // process-shared motion lane. A fresh racer may reset its local
        // animation records, but must not erase that live global chronology.
        const auto sharedMotion=state.animationShared.motion;
        state.animationShared={}; //Before retained single-record28EB90/libm effects.
        if(preserveSharedMotion)state.animationShared.motion=sharedMotion;
        // Complete each actual owner's constructor/setup in represented source
        // right/left/bottom-right/bottom-left/chariot order before normalization,
        // motion sample or rebase. This differs from the Rig's asset role order.
        // A model without a canonical owner retains its generic installation.
        //1543A8 restores the shared model directory before its sibling saves
        // it; nesting two154140 prefixes would lose that actual path.
        if constexpr(!std::is_same_v<RightModelConstruction,std::nullptr_t>){
            if(rightModel){
                rightModel->constructGraph({state.nodes,nodeCount});
                rightModel->prepareAnimation({state.nodes,nodeCount});
            }
        }
        if constexpr(!std::is_same_v<LeftModelConstruction,std::nullptr_t>){
            if(leftModel){
                leftModel->constructGraph({state.nodes,nodeCount});
                leftModel->prepareAnimation({state.nodes,nodeCount});
            }
        }
        if constexpr(!std::is_same_v<BottomRightModelConstruction,std::nullptr_t>){
            if(bottomRightModel){
                bottomRightModel->constructGraph({state.nodes,nodeCount});
                bottomRightModel->prepareAnimation({state.nodes,nodeCount});
            }
        }
        if constexpr(!std::is_same_v<BottomLeftModelConstruction,std::nullptr_t>){
            if(bottomLeftModel){
                bottomLeftModel->constructGraph({state.nodes,nodeCount});
                bottomLeftModel->prepareAnimation({state.nodes,nodeCount});
            }
        }
        if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>){
            if(chariotModel){
                chariotModel->constructGraph({state.nodes,nodeCount});
                chariotModel->prepareAnimation({state.nodes,nodeCount});
            }
        }
        for(unsigned model=0;model<modelCount;++model){
            if constexpr(!std::is_same_v<RightModelConstruction,std::nullptr_t>){
                if(model==right&&rightModel)continue;
            }
            if constexpr(!std::is_same_v<LeftModelConstruction,std::nullptr_t>){
                if(model==left&&leftModel)continue;
            }
            if constexpr(!std::is_same_v<BottomRightModelConstruction,std::nullptr_t>){
                if(model==bottomRight&&bottomRightModel)continue;
            }
            if constexpr(!std::is_same_v<BottomLeftModelConstruction,std::nullptr_t>){
                if(model==bottomLeft&&bottomLeftModel)continue;
            }
            if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>){
                if(model==chariot&&chariotModel)continue;
            }
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
        for(unsigned model=0;model<modelCount;++model)if(model!=chariot){
            // Retained owners captured source cold operands before this point.
            // Their real28AB90 invalidates any cache refreshed by28B1A0.
            if constexpr(!std::is_same_v<RightModelConstruction,std::nullptr_t>){
                if(model==right&&rightModel){rightModel->normalizeColdEngineBeforeMotion({state.nodes,nodeCount});continue;}
            }
            if constexpr(!std::is_same_v<LeftModelConstruction,std::nullptr_t>){
                if(model==left&&leftModel){leftModel->normalizeColdEngineBeforeMotion({state.nodes,nodeCount});continue;}
            }
            if constexpr(!std::is_same_v<BottomRightModelConstruction,std::nullptr_t>){
                if(model==bottomRight&&bottomRightModel){bottomRightModel->normalizeColdEngineBeforeMotion({state.nodes,nodeCount});continue;}
            }
            if constexpr(!std::is_same_v<BottomLeftModelConstruction,std::nullptr_t>){
                if(model==bottomLeft&&bottomLeftModel){bottomLeftModel->normalizeColdEngineBeforeMotion({state.nodes,nodeCount});continue;}
            }
            for(unsigned lane=12;lane<15;++lane)
                state.nodes[static_cast<unsigned>(sourceNodes[model][0])].localC0.v[lane]=0;
        }
        auto owner=[&](Index index,const SceneNode49P& original){auto& target=state.nodes[static_cast<unsigned>(index)];target=original;
            target.parent180=target.firstChild184=target.nextSibling188=Index::none;};
        owner(physical,previousPhysical);
        bool retainedScenes=false;
        if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>)
            retainedScenes=chariotModel!=nullptr;
        if(!retainedScenes){
            owner(main,previousMain);owner(container,previousContainer);owner(hidden,previousHidden);
            state.nodes[static_cast<unsigned>(main)].firstChild184=container;
            state.nodes[static_cast<unsigned>(container)].parent180=main;
        }
        // Retained190/198/194 were constructed before the chariot and190->194
        // parented at211A34. Never overwrite them with later numeric snapshots.
        const auto leftCable=find(chariot,"chariotcableleft1"),rightCable=find(chariot,"chariotcableright1");
        need(leftCable!=Index::none&&rightCable!=Index::none,"Roster lifecycle cable anchors");
        bindings.lifecycle={main,container,hidden,physical,roots[chariot],roots[left],roots[right],leftCable,rightCable};
        bindings.visual.main190=main;bindings.visual.container194=container;
        bindings.visual.left1BC=roots[left];bindings.visual.right1B8=roots[right];
        bindings.visual.extra1C0Present=bindings.visual.extra1C4Present=modelCount==5;
        bindings.visual.bottomRight1C0=modelCount==5?roots[bottomRight]:Index::none;
        bindings.visual.bottomLeft1C4=modelCount==5?roots[bottomLeft]:Index::none;
        state.fields.bottomRight191=state.fields.bottomLeft191=modelCount==5?1:0;
        auto& animation=bindings.animation;animation={};
        animation.chariot1B4=roots[chariot];animation.left1BC=roots[left];animation.right1B8=roots[right];animation.bodyScene288=main;
        animation.chariotRows=&rows[chariot];animation.leftRows=rows[left].count?&rows[left]:nullptr;animation.rightRows=rows[right].count?&rows[right]:nullptr;
        if constexpr(!std::is_same_v<RightModelConstruction,std::nullptr_t>){
            if(rightModel)animation.rightRows=&rightModel->animationSetup().mappedRows1EC();
        }
        if constexpr(!std::is_same_v<LeftModelConstruction,std::nullptr_t>){
            if(leftModel)animation.leftRows=&leftModel->animationSetup().mappedRows1EC();
        }
        if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>){
            if(chariotModel)animation.chariotRows=&chariotModel->animationSetup().mappedRows1EC();
        }
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
        // Original211A54..211B3C independently guards each wrist/parent chain:
        // a wrist (handlt/handrt) absent from this pod's chariot PSG leaves that
        // hand without a28EB90 record (+208/+20C stay null), for every pod.
        // No dummy nodes, synthetic IK records, or constructor/libm effects.
        // One hand present and one absent needs the per-record constructor and
        // is refused here rather than paired with an absent chain.
        const bool leftWrist=find(chariot,"handlt")!=Index::none;
        const bool rightWrist=find(chariot,"handrt")!=Index::none;
        const bool absentHands=!leftWrist&&!rightWrist;
        if(retainedScenes){
            if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>)
                chariotModel->bindColdWrists(animation.hands,hands,animation.handRecords);
        }else if(absentHands){
            const ChariotIkChainBinding49T absent{Index::none,Index::none,Index::none,Index::none};
            animation.hands={absent,absent};animation.handRecords=nullptr;
        }else{
            need(leftWrist&&rightWrist,"Roster single-wrist chariot requires per-record 28EB90 construction");
            animation.hands={chain("handlt","l_hand_dummy"),chain("handrt","r_hand_dummy")};animation.handRecords=&hands;
        }
        state.animation={};
        need(initializeVehicleAnimationFilters49T(0,0,state.animation),"Roster cold animation filters");
        state.sceneNodeCount=bindings.sceneNodeCount=nodeCount;
        const SceneGraph49P graph{state.nodes,state.sceneNodeCount};
        ChariotIkWorkspace49T ikWork{};ChariotIkDiagnostic49T ikDiagnostic{};
        if(!retainedScenes&&!absentHands)need(initializeChariotIk49T(animation.hands,graph,hands,state.animationShared.libm,ikWork,ikDiagnostic),"Roster original pre-motion IK constructors");
        // Original213830 then213854. These mutate only this model's source rows.
        ModelMotionWorkspace49T motionWork{};ModelMotionDiagnostic49T motionDiagnostic{};
        if(motion(Forward)&&motion(Back))need(sampleModelMotionBlend49T(motions[Forward],motions[Back],*animation.chariotRows,.5f,0,
            graph,state.animation.chariotModel,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold Forward/Back sample");
        if(motion(Mid))need(sampleModelMotion49T(motions[Mid],*animation.chariotRows,.5f,graph,
            state.animation.chariotModel,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold Mid sample");
        // Original21385C..213A20 initializes right then left engine: optional
        // primary motion at0, identity root orientation, optional Mid at.5.
        // Absent engine records do not cause an invented sampler call.
        for(unsigned model:{right,left}){
            const auto primary=model==right?EngineRight:EngineLeft;
            const auto middle=model==right?EngineRightMid:EngineLeftMid;
            auto& modelState=model==right?state.animation.rightModel:state.animation.leftModel;
            const auto* mappedRows=model==right?animation.rightRows:animation.leftRows;
            const auto& modelRows=mappedRows?*mappedRows:rows[model];
            if(motion(primary))need(sampleModelMotion49T(motions[primary],modelRows,0,
                graph,modelState,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold engine sample");
            need(setLocalMatrix49P(graph,roots[model],f::identity()),"Roster cold engine root orientation");
            if(motion(middle))need(sampleModelMotion49T(motions[middle],modelRows,.5f,
                graph,modelState,state.animationShared.motion,motionWork,motionDiagnostic),"Roster cold engine Mid sample");
        }
        // Display root carries the current secondary pose; never collapse the
        // per-node hierarchy or replace future native animation with this pose.
        need(setLocalMatrix49P(graph,roots[chariot],previousPhysical.localC0),"Roster initial chariot display pose");
        // Native caller placement, after the concrete original constructor.
        // Preserve its identity/hierarchy and use only the caller's saved local
        // pose.2893A0 intentionally leaves world100 zero; canonical refresh is
        // required before project and handling consume the placed main world.
        // This is not an assertion of another original210770 placement call.
        if(retainedScenes)
            need(setLocalMatrix49P(graph,main,previousMain.localC0)&&
                refreshSceneWorld49P(graph,main),"Roster retained main caller placement");
        for(unsigned i=0;i<vehicleFrameNodeCount49W;++i)
            bindings.topology[i]={state.nodes[i].parent180,state.nodes[i].firstChild184,state.nodes[i].nextSibling188};
        if constexpr(!std::is_same_v<ChariotModelConstruction,std::nullptr_t>)
            if(chariotModel)chariotModel->constructColdMain211F6C(graph);
        f::project(state,bindings);
    }
    void buildEffectNames(const g::VehicleFrameState49W& state,a::EffectNames50BL& names)const{
        a::ModelRootName50BL rootName{};a::initializeFreshModelRootName50BL(rootName);
        a::EffectNameModel50BL models[4]{};
        const unsigned engineModels[]={left,right,bottomLeft,bottomRight};
        // Preserve the historical left/right/chariot metadata scope for normal
        // pods; the optional topology needs only its four reached engine trees.
        const unsigned namedCount=modelCount==5?4u:3u;
        for(unsigned i=0;i<namedCount;++i){const auto model=modelCount==5?engineModels[i]:i;
            models[i]={roots[model],&rootName,hierarchy[model],sourceNodes[model],hierarchy[model].nodeCount};}
        rig_detail::need(a::buildEffectNames50BL({const_cast<g::SceneNode49P*>(state.nodes),state.sceneNodeCount},models,namedCount,names),"Roster effect source-name scope");
    }
};
}
