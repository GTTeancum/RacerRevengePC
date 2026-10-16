#pragma once
#include "model_playback50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

// Native animation allocation owner. Numeric nodes remain in the caller's
// SINGLE real graph. This file has no Rig/fixture dependency.
namespace original_racer_animation_setup50LA {
namespace a=rr::assets;
namespace g=rr::game;
using Index=g::SceneNodeIndex49P;
using Ini=std::map<std::string,std::map<std::string,std::string>>;
struct Configuration {
    std::string definition48,vut98,mirrorE8,slt138,directory190;
    std::uint32_t rigOverride188=0,field18C=1;
    std::int32_t motionCount14=0,interpolatePosition20=1,interpolateRotation24=1;
    std::uint32_t rotationType28=0,positionType2C=1,mirrorAxis0C=0;
};
struct Input {
    const void* actualRig=nullptr;
    unsigned model=0;
    a::PsgHierarchyView49O hierarchy{};
    const Index* sourceNodes=nullptr;
    std::uint32_t sceneNodeCount=0;
    Configuration configuration;
    Ini parsedConfiguration;
    // Absence must come from the authenticated actual archive directory,
    // never an incomplete extracted manifest or caller readiness flag.
    std::optional<std::vector<std::uint8_t>> mirror,vut;
    std::string archiveDigest,mirrorMember,vutMember;
};
namespace setup_detail {
inline void need(bool b,const char* why){if(!b)throw std::runtime_error(why);}
inline std::string upper(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-32);return s;}
inline bool identifier(const std::string& s){
    if(s.empty()||s.size()>49)return false;
    for(unsigned char c:s)if(c<=32||c>=127||c=='='||c==',')return false;
    return true;
}
}
class Setup final {
    const void* rig_=nullptr;
    unsigned model_=0;
    a::PsgHierarchyView49O hierarchy_{};
    const Index* sourceNodes_=nullptr;
    std::uint32_t sceneNodeCount_=0;
    g::SceneNode49P* captureGraph_=nullptr;
    Configuration config_;
    Ini parsed_;
    a::MotionNodeBindings49O local_{},mapped_{};
    // Native semantic allocation for the source1EC rows. local_/mapped_ are
    // immutable numeric-binding views; this storage has its own source lifetime.
    std::vector<a::MotionNodeBinding49O> records1EC_;
    a::MotionBindingDiagnostic49O vutDiagnostic_{};
    std::vector<std::optional<std::uint8_t>> mirror_;
    std::vector<g::BodyMatrix48G> saved_;
    std::string archiveDigest_,vutMember_,mirrorMember_;
    bool prepared_=false,vutOpened_=false,mirrorOpened_=false,captured_=false,restored_=false;
    bool beforeMotionReleased_=false,afterMotionReleased_=false;
    std::uint32_t field1F8_=0;
    void readMirror(const std::vector<std::uint8_t>& bytes){
        using namespace setup_detail;
        need(bytes.size()<=16384,"original MIR extent outside admitted roster text");
        //153558 allocation precedes reads. Unwritten words are unproduced,
        // not an invented identity mapping or zero-filled source table.
        mirror_.resize(local_.count);const std::string text(bytes.begin(),bytes.end());
        for(std::size_t begin=0;begin<text.size();){
            auto end=text.find('\n',begin);if(end==std::string::npos)end=text.size();
            auto line=text.substr(begin,end-begin);begin=end==text.size()?end:end+1;
            if(!line.empty()&&line.back()=='\r')line.pop_back();
            need(!line.empty()&&line.size()<=255,"original MIR line outside admission");
            line=upper(std::move(line));const auto split=line.find_first_of("=,");
            need(split!=std::string::npos,"original MIR pair delimiter absent");
            const auto lhs=line.substr(0,split),rhs=line.substr(split+1);
            need(identifier(lhs)&&identifier(rhs),"original MIR pair name outside admission");
            auto index=[&](const std::string& name){
                std::uint32_t row=0;while(row<local_.count&&name!=local_.rows[row].name)++row;
                //153450 never checks the returnedFF before indexing.
                need(row<local_.count,"original MIR unresolved name would index FF");return row;
            };
            const auto first=index(lhs),second=index(rhs);
            mirror_[first]=static_cast<std::uint8_t>(second);mirror_[second]=static_cast<std::uint8_t>(first);
        }
    }
    void checkGraph(const g::SceneGraph49P& graph)const{
        using namespace setup_detail;
        need(prepared_&&!beforeMotionReleased_&&!afterMotionReleased_&&rig_&&sourceNodes_&&graph.nodes&&graph.count==sceneNodeCount_&&
            g::validateSceneGraph49P(graph),"original animation setup requires actual complete Rig graph");
        for(std::uint32_t row=0;row<local_.count;++row){
            const auto source=local_.rows[row].sceneNode;
            need(source<hierarchy_.nodeCount&&mapped_.rows[row].sceneNode==
                static_cast<std::uint32_t>(sourceNodes_[source])&&mapped_.rows[row].sceneNode<graph.count,
                "original animation source row mapping changed");
        }
    }
public:
    Setup()=default;
    Setup(const Setup&)=delete;Setup& operator=(const Setup&)=delete;
    Setup(Setup&&)=delete;Setup& operator=(Setup&&)=delete;
    //152EA0 ->153450 ->1530E0. Actual model load/append must precede this.
    // Configuration is from153CA0 or exact warm156300 copies. No153610 implied.
    void prepareTables154140(Input input){
        using namespace setup_detail;
        need(!prepared_&&!rig_&&input.actualRig&&input.sourceNodes&&input.hierarchy.data&&
            input.hierarchy.nodeCount&&input.sceneNodeCount<=g::maxSceneNodes49P,
            "original animation table input or lifetime rejected");
        a::MotionBindingDiagnostic49O d;
        need(a::readMotionNodeEnumeration49O(input.hierarchy,local_,d),"original152EA0 source enumeration failed");
        if(input.mirror){readMirror(*input.mirror);mirrorOpened_=true;}
        if(input.vut){
            //153248..15325C skips ANY absent LHS in order. An opened empty
            // VUT is successful EOF; all enumeration rows survive unchanged.
            if(!input.vut->empty())need(a::readMotionNodeBindings49O(input.hierarchy,
                reinterpret_cast<const char*>(input.vut->data()),input.vut->size(),local_,vutDiagnostic_,
                a::MotionBindingMissingNode49O::sourceDiagnosticSkip),"original VUT source text rejected");
            vutOpened_=true;field1F8_=1;
        }
        mapped_=local_;
        for(std::uint32_t row=0;row<mapped_.count;++row){
            const auto index=static_cast<std::int32_t>(input.sourceNodes[local_.rows[row].sceneNode]);
            need(index>=0&&static_cast<std::uint32_t>(index)<input.sceneNodeCount&&index<256,
                "original animation graph mapping outside native representation");
            mapped_.rows[row].sceneNode=static_cast<std::uint8_t>(index);
        }
        records1EC_.assign(mapped_.rows,mapped_.rows+mapped_.count);
        rig_=input.actualRig;model_=input.model;hierarchy_=input.hierarchy;sourceNodes_=input.sourceNodes;
        sceneNodeCount_=input.sceneNodeCount;config_=std::move(input.configuration);parsed_=std::move(input.parsedConfiguration);
        archiveDigest_=std::move(input.archiveDigest);vutMember_=std::move(input.vutMember);
        mirrorMember_=std::move(input.mirrorMember);prepared_=true;
    }
    void capture155730(const g::SceneGraph49P& graph){
        using namespace setup_detail;checkGraph(graph);need(!captured_&&!restored_,"original155730 cold capture repeated");
        for(std::uint32_t row=0;row<local_.count;++row){
            a::PsgNode49O source{};
            need(a::readPsgNode49O(hierarchy_,local_.rows[row].sceneNode,source)&&
                std::memcmp(graph.nodes[mapped_.rows[row].sceneNode].localC0.v,source.local,sizeof source.local)==0,
                "original155730 must precede nullcg normalization and sampling");
        }
        g::ModelPlayback50KG scratch;g::ModelPlaybackDiagnostic50KG d;
        need(g::captureModelPlaybackMatrices50KG(mapped_,graph,scratch,d),"original155730 matrix capture rejected");
        saved_.assign(scratch.savedMatrices1F4,scratch.savedMatrices1F4+scratch.savedMatrixCount1F4);
        captureGraph_=graph.nodes;captured_=true;
    }
    //15642C..1564AC: separate allocation,64-byte copy per row,1F8=1.
    // Does not copy prototype graph pointers,1EC,1F0 or1E4 ownership.
    void copyWarmMatrices15642C(const Setup& prototype,const g::SceneGraph49P& graph){
        using namespace setup_detail;checkGraph(graph);
        need(!captured_&&!restored_&&prototype.captured_&&prototype.saved_.size()==mapped_.count&&
            prototype.local_.count==local_.count,"original156300 prototype matrix table differs");
        saved_=prototype.saved_;captureGraph_=graph.nodes;captured_=true;field1F8_=1;
    }
    void restore1556C0(const g::SceneGraph49P& graph){
        using namespace setup_detail;checkGraph(graph);
        need(captured_&&!restored_&&captureGraph_==graph.nodes&&saved_.size()==mapped_.count&&
            std::fegetround()==FE_TONEAREST,"original1556C0 requires same-graph capture allocation");
        for(const auto& matrix:saved_)for(float v:matrix.v)need(std::isfinite(v),"original1556C0 nonfinite matrix");
        for(std::uint32_t row=0;row<mapped_.count;++row)
            need(g::setLocalMatrix49P(graph,static_cast<Index>(mapped_.rows[row].sceneNode),saved_[row]),
                "original1556C0 ordered restore failed");
        restored_=true;
    }
    //153FA4/153FB8, before motion1E4 releases. These are actual native storage
    // frees. The external animation fields retain their original stale pointer
    // values; no source1F0/1F4 null write is claimed. Repeat cleanup is native
    // lifetime bookkeeping, not a second source destructor dispatch.
    void releaseBeforeMotion153FA4(){
        if(beforeMotionReleased_)return;
        setup_detail::need(!afterMotionReleased_,"original animation table release order differs");
        std::vector<std::optional<std::uint8_t>>().swap(mirror_);
        std::vector<g::BodyMatrix48G>().swap(saved_);
        beforeMotionReleased_=true;
    }
    //1540C4 after the caller's153610 motion-set release and optional later
    // animation allocation releases. Does not dispose live config08: its real
    //1573B0 owner must already have been closed at153C38/153C6C.
    void releaseAfterMotion1540C4(){
        if(afterMotionReleased_)return;
        setup_detail::need(beforeMotionReleased_,"original1EC release precedes1F0/1F4 release");
        std::vector<a::MotionNodeBinding49O>().swap(records1EC_);
        Ini().swap(parsed_);afterMotionReleased_=true;
    }
    bool prepared()const noexcept{return prepared_;}
    bool captured155730()const noexcept{return captured_;}
    bool restored1556C0()const noexcept{return restored_;}
    const void* actualRig()const noexcept{return rig_;}
    unsigned model()const noexcept{return model_;}
    const Configuration& configuration()const noexcept{return config_;}
    const Ini& parsedConfiguration()const noexcept{return parsed_;}
    const a::MotionNodeBindings49O& localRows1EC()const noexcept{return local_;}
    const a::MotionNodeBindings49O& mappedRows1EC()const noexcept{return mapped_;}
    const std::vector<a::MotionNodeBinding49O>& records1EC()const noexcept{return records1EC_;}
    const std::vector<std::optional<std::uint8_t>>& mirrorRows1F0()const noexcept{return mirror_;}
    const std::vector<g::BodyMatrix48G>& savedMatrices1F4()const noexcept{return saved_;}
    const a::MotionBindingDiagnostic49O& vutDiagnostic()const noexcept{return vutDiagnostic_;}
    bool mirrorOpened153450()const noexcept{return mirrorOpened_;}
    bool vutOpened1530E0()const noexcept{return vutOpened_;}
    std::uint32_t field1F8()const noexcept{return field1F8_;}
    bool releasedBeforeMotion153FA4()const noexcept{return beforeMotionReleased_;}
    bool releasedAfterMotion1540C4()const noexcept{return afterMotionReleased_;}
    const std::string& archiveDigest()const noexcept{return archiveDigest_;}
    const std::string& vutMember()const noexcept{return vutMember_;}
    const std::string& mirrorMember()const noexcept{return mirrorMember_;}
};
}
