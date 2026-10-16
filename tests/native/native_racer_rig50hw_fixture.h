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

    Rig(const std::filesystem::path& repository,const std::filesystem::path& podJson){
        using namespace rig_detail;
        const auto resolved=runtime_asset_paths50LB::resolve(podJson);
        const auto file=resolved.is_absolute()?resolved:repository/resolved;
        directory=file.parent_path();const auto pod=json(file);manifest=json(directory/"import.json");
        podId=pod.at("pod_id").get<std::string>();name=pod.at("name").get<std::string>();
        need(podId.size()==5&&podId.substr(0,3)=="POD"&&podId[3]>='0'&&podId[3]<='9'&&podId[4]>='0'&&podId[4]<='9',"Roster pod identity");
        resourceFile=repository/runtime_asset_paths50LB::resolve("work/native/project/disc/PODS")/podId/(podId+".RES");
        struct Member {std::filesystem::path path;std::string digest;std::size_t bytes;std::string sourceName;};
        std::map<std::string,Member> members;
        for(const auto& entry:manifest.at("members")){
            const auto path=std::filesystem::path(entry.at("source_file").get<std::string>());
            need(!path.is_absolute()&&path==path.filename(),"Roster imported member path escapes owner");
            need(members.emplace(lower(entry.at("source_member").get<std::string>()),Member{directory/path,entry.at("source_sha256").get<std::string>(),entry.at("source_bytes").get<std::size_t>(),entry.at("source_member").get<std::string>()}).second,
                "Roster duplicate source member");
        }
        auto read=[&](const std::string& member){const auto found=members.find(lower(member));
            need(found!=members.end(),"Roster "+podId+" source member missing: "+member);
            auto bytes=f::readBytes(found->second.path);
            need(bytes.size()==found->second.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==found->second.digest,
                "Roster "+podId+" retained original member identity differs: "+member);return bytes;};
        phy=read(pod.at("phy").at("source_member").get<std::string>());
        const auto setup=ini(phy);const auto prefix=value(setup,"pod setup","prefix");
        need(prefix==podId.substr(3),"Roster PHY prefix identity differs");
        const std::string base="pods/"+lower(podId)+"/";
        const char* roles[]={"leftengine","rightengine","chariot","bottomleftengine","bottomrightengine"};
        const auto& authoredSetup=setup.at("pod setup");
        const bool bottomL=authoredSetup.count("bottomleftengine")!=0;
        const bool bottomR=authoredSetup.count("bottomrightengine")!=0;
        need(bottomL==bottomR,"Roster partial bottom-engine topology unavailable");
        if(bottomL){
            need(podId=="POD02"&&value(setup,"pod setup","enginemotion")=="5",
                "Roster optional engine topology requires original POD02 EngineMotion5");
            modelCount=5;
        }
        const char* suffixes[]={"engineleft","engineright","brakeleft","brakeright","forward","back","mid","enginerightmid","engineleftmid"};
        const unsigned motionModels[]={left,right,chariot,chariot,chariot,chariot,chariot,right,left};
        std::uint32_t next=0;
        for(unsigned model=0;model<modelCount;++model){
            const auto selected=base+lower(value(setup,"pod setup",roles[model]));
            try {
            const auto psgMember=withExtension(selected,".psg");
            psg[model]=read(psgMember);psgMembers[model]=members.at(lower(psgMember)).sourceName;
            a::GeometryDiagnostic49O gd{};
            if(!a::readPsgHierarchy49O(psg[model].data(),psg[model].size(),hierarchy[model],gd))
                need(false,"native PSG admission: "+psgMember+" error="+
                    std::to_string(static_cast<unsigned>(gd.code))+" byte="+std::to_string(gd.byteOffset));
            roots[model]=static_cast<Index>(next++);
            need(hierarchy[model].nodeCount<=a::maxModelNodes49O&&next+hierarchy[model].nodeCount+4<=g::vehicleFrameNodeCount49W,
                "Roster forest exceeds native scene capacity");
            for(unsigned n=0;n<hierarchy[model].nodeCount;++n)sourceNodes[model][n]=static_cast<Index>(next+n);
            next+=hierarchy[model].nodeCount;
            const auto mcfMember=withExtension(selected,".mcf");
            mcf[model]=read(mcfMember);mcfMembers[model]=members.at(lower(mcfMember)).sourceName;
            const auto config=ini(mcf[model]);
            std::size_t consumed=0;const auto count=std::stoul(value(config,"general info","numberofmotions"),&consumed);
            need(consumed==value(config,"general info","numberofmotions").size()&&count<=MotionCount,"Roster MCF motion count");
            const auto content=lower(value(config,"general info","contentdirectory"));
            need(content==base,"Roster MCF content directory differs from selected pod");
            if(count){
                const auto vutMember=content+lower(value(config,"general info","vutfilename"));
                vut[model]=read(vutMember);
                auto& bd=bindingDiagnostics[model];
                // Authentic POD09 englt1.vut lists damage04..07, while its PSG
                // owns damage00..03. Original153248..15325C logs and skips these
                // four absent names; it never maps them onto the existing four.
                // Other selected models retain the strict historical admission.
                const bool originalMissingDamage=podId=="POD09"&&model==left;
                const bool originalMissingBinder=podId=="POD12"&&model==left;
                const bool originalWrongBinder=podId=="POD20"&&model==right;
                const bool originalWrongFlames=podId=="POD19"&&model==left;
                if(originalMissingDamage){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="af57258b551e3a52223ac2d70bb6b6403c5586242b884088413668525154982f"&&
                        members.at(psgMember).digest=="d6282f412bf96e20e06c57261a3c31ce2073f467e3b95d1716bfc04ac969d12f",
                        "source missing-name admission requires original POD09 left-engine PSG/VUT identities");
                }
                // Authentic POD12 spells its VUT key binderleftt1; the PSG
                // owns binderleft1. Preserve original153248..15325C's skip,
                // rather than repairing the name or manufacturing a node.
                if(originalMissingBinder){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="cb7148c2199a65c9a6378f0d3c8a14e646359925700bea4133dc94389db38894"&&
                        members.at(psgMember).digest=="bbd1a0452fecc07503104461c73d60e2d086b728ae04765e5c1cd57d4c1cdcb2",
                        "source missing-name admission requires original POD12 left-engine PSG/VUT identities");
                }
                if(originalWrongBinder){
                    need(vutMember==base+"engrt1.vut"&&psgMember==base+"engrt1.psg"&&
                        members.at(vutMember).digest=="b50ef5b5ac03ca6aac484fd4b67364b7b435701d796fc3053a1a43f7d968da65"&&
                        members.at(psgMember).digest=="be9a0b394caf482167db3fbdb9080476eebdb818b7458dd3c0924e58d25e116f",
                        "source missing-name admission requires original POD20 right-engine PSG/VUT identities");
                }
                // Authentic POD19 left VUT lists engrtflame01/02 at181/208,
                // but its PSG owns engltflame01/02. Original153248..15325C
                // skips these absent lookup keys; it never aliases the real
                // left flame rows or changes their unlisted flags.
                if(originalWrongFlames){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="37e4cd4e0fff2472b2a045ef5da8a8e4ca02d1775e7c0d9afbca4affa3ae3524"&&
                        members.at(psgMember).digest=="a5f24cf472e3f52f989cc053236865503c0f6d838a3ce9a18c3b555c0b524b9e",
                        "source missing-name admission requires original POD19 left-engine PSG/VUT identities");
                }
                const auto missing=(originalMissingDamage||originalMissingBinder||originalWrongBinder||originalWrongFlames)?a::MotionBindingMissingNode49O::sourceDiagnosticSkip:
                    a::MotionBindingMissingNode49O::reject;
                if(!a::readMotionNodeBindings49O(hierarchy[model],reinterpret_cast<const char*>(vut[model].data()),
                    vut[model].size(),localRows[model],bd,missing))
                    need(false,"native VUT admission: "+vutMember+" error="+bindingErrorName(bd.error)+
                        " byte="+std::to_string(bd.offset)+" skippedUnknown="+std::to_string(bd.skippedUnknownNodes));
                need(bd.skippedUnknownNodes==(originalMissingDamage?4u:originalWrongFlames?2u:(originalMissingBinder||originalWrongBinder)?1u:0u)&&
                    (!originalMissingDamage||bd.firstUnknownOffset==57u)&&
                    (!originalWrongFlames||bd.firstUnknownOffset==181u)&&
                    (!(originalMissingBinder||originalWrongBinder)||bd.firstUnknownOffset==15u),
                    "native VUT source missing-name count differs: "+vutMember+" skippedUnknown="+
                    std::to_string(bd.skippedUnknownNodes)+" firstByte="+std::to_string(bd.firstUnknownOffset));
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
                const auto motMember=content+withExtension(lower(value(config,section,"vue")),".mot");
                mot[slot]=read(motMember);
                motMembers[slot]=members.at(lower(motMember)).sourceName;
                motionNames[slot]=value(config,section,"name");
                if(!a::readMot49O(mot[slot].data(),mot[slot].size(),motions[slot],gd))
                    need(false,"native MOT admission: "+motMember+" identity="+identity+" error="+
                        std::to_string(static_cast<unsigned>(gd.code))+" byte="+std::to_string(gd.byteOffset));
                for(unsigned channel=0;channel<2;++channel){const auto tracks=channel?motions[slot].positionCount:motions[slot].rotationCount;
                    for(unsigned t=0;t<tracks;++t)need((channel?motions[slot].position[t]:motions[slot].rotation[t]).rowIndex<rows[model].count,"Roster MOT/VUT row binding");}
            }
            } catch(const std::exception& error) {
                throw std::runtime_error("Roster "+podId+" "+roles[model]+" ["+selected+"]: "+error.what());
            }
        }
        physical=static_cast<Index>(next++);main=static_cast<Index>(next++);
        container=static_cast<Index>(next++);hidden=static_cast<Index>(next++);nodeCount=next;
        //21380C/213818 skip the pair if either actual MCF lookup is absent.
        const unsigned order[]={chariot,right,left,bottomRight,bottomLeft};
        for(unsigned i=0;i<modelCount;++i){const auto model=order[i];rebaseModels[i]={roots[model],hierarchy[model],sourceNodes[model],hierarchy[model].nodeCount};}
        const auto sources=geometrySources();g::FreshPodGeometryWorkspace49O work{};g::FreshPodGeometryDiagnostic49O diagnostic{};
        if(!g::buildFreshPodGeometry49O(sources,work,geometry,diagnostic))
            need(false,"Roster "+podId+" chariot fresh geometry error="+
                std::to_string(static_cast<unsigned>(diagnostic.error))+" motionError="+
                std::to_string(static_cast<unsigned>(diagnostic.motion.error))+" track="+
                std::to_string(diagnostic.motion.track));
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
        BottomRightModelConstruction bottomRightModel=nullptr,BottomLeftModelConstruction bottomLeftModel=nullptr)const{
        using namespace rig_detail;using namespace g;
        need(state.phase==VehicleFramePhase49W::Ready&&state.bodies.main.frameFactor==0,"Roster rig install requires cold state");
        need(state.fields.cableMode48.construct20F6B4(),"Roster racer48 cold constructor already projected");
        const auto previousMain=state.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
        const auto previousPhysical=state.nodes[static_cast<unsigned>(bindings.lifecycle.physical19C)];
        const auto previousContainer=state.nodes[static_cast<unsigned>(bindings.lifecycle.container194)];
        const auto previousHidden=state.nodes[static_cast<unsigned>(bindings.lifecycle.hidden198)];
        for(auto& node:state.nodes){node={};node.localC0=node.world100=f::identity();}
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
        owner(main,previousMain);owner(physical,previousPhysical);owner(container,previousContainer);owner(hidden,previousHidden);
        state.nodes[static_cast<unsigned>(main)].firstChild184=container;
        state.nodes[static_cast<unsigned>(container)].parent180=main;
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
        // Original211A54..211B3C independently guards each wrist/parent chain.
        // POD20 has neither hand nor target: retain absent bindings, with no
        // dummy nodes, synthetic IK records, or constructor/libm side effects.
        const bool absentHands=podId=="POD20";
        if(absentHands){
            need(pod_materials50ed_fixture::detail::sha256(psg[chariot])=="136b0690d569ef1ed641055534371f9e99a02ac7d0a33599683f5c6d170c9ca3"&&
                find(chariot,"handlt")==Index::none&&find(chariot,"handrt")==Index::none&&
                find(chariot,"l_hand_dummy")==Index::none&&find(chariot,"r_hand_dummy")==Index::none,
                "Roster original POD20 absent hand/target identities");
            const ChariotIkChainBinding49T absent{Index::none,Index::none,Index::none,Index::none};
            animation.hands={absent,absent};animation.handRecords=nullptr;
        }else{
            animation.hands={chain("handlt","l_hand_dummy"),chain("handrt","r_hand_dummy")};animation.handRecords=&hands;
        }
        state.animation={};state.animationShared={};
        need(initializeVehicleAnimationFilters49T(0,0,state.animation),"Roster cold animation filters");
        state.sceneNodeCount=bindings.sceneNodeCount=nodeCount;
        const SceneGraph49P graph{state.nodes,state.sceneNodeCount};
        ChariotIkWorkspace49T ikWork{};ChariotIkDiagnostic49T ikDiagnostic{};
        if(!absentHands)need(initializeChariotIk49T(animation.hands,graph,hands,state.animationShared.libm,ikWork,ikDiagnostic),"Roster original pre-motion IK constructors");
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
        for(unsigned i=0;i<vehicleFrameNodeCount49W;++i)
            bindings.topology[i]={state.nodes[i].parent180,state.nodes[i].firstChild184,state.nodes[i].nextSibling188};
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
