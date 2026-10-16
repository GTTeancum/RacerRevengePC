#pragma once
#include "original_racer_model_scene50la.h"
#include <exception>
#include <memory>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
// Source2CB5A0 arrays only. External particle children are owned by their
//actual forest; allocating these pointer slots does not construct a child.
class OriginalWreckEffectArrays50LA final {
    friend class OriginalWreckRacerStorage50LA;
    using O=OriginalRacerSceneObject50LA;
    std::unique_ptr<OriginalWreckComponent50LA*[]> smoke160_,fire164_;
    std::unique_ptr<const O*[]> anchors170_;
    std::size_t count_=0,anchorsWritten_=0,smokeWritten_=0;
    bool attempted_=false,retired_=false;
    std::uint32_t boundary_=0;
    static bool damageName(const std::string& name)noexcept {
        constexpr char expected[]="damage";
        if(name.size()!=6)return false;
        for(std::size_t i=0;i<6;++i){auto c=name[i];if(c>='A'&&c<='Z')c=char(c+32);
            if(c!=expected[i])return false;}return true;
    }
    static bool findDamage28C590(const OriginalRacerModelSceneOwner50LA& model,
        const SceneGraph49P& graph,const O*& found)noexcept {
        found=nullptr;
        if(model.released()||!model.model()||!model.sourceRowCount()||!graph.nodes)return false;
        std::size_t remaining=model.sourceRowCount()+1u;
        auto walk=[&](auto&& self,SceneNodeIndex49P index)->bool {
            if(index==SceneNodeIndex49P::none)return true;
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining--||n>=graph.count)return false;
            const O* object=model.model()->sceneIndex==index?model.model():nullptr;
            for(std::uint32_t i=0;!object&&i<model.sourceRowCount();++i){
                const auto* row=model.sourceRow(i);if(row&&row->sceneIndex==index)object=row;}
            if(!object||!object->constructed||object->released)return false;
            if(damageName(object->name38)){found=object;return true;}
            if(!self(self,graph.nodes[n].nextSibling188))return false;
            if(found)return true;
            return self(self,graph.nodes[n].firstChild184);
        };
        return walk(walk,model.model()->sceneIndex);
    }
    bool allocateAndFind2CB634(std::uint32_t count,const OriginalRacerModelSceneOwner50LA& model,
        const SceneGraph49P& graph)noexcept {
        if(attempted_||retired_||!count||count>512)return false;
        attempted_=true;count_=count;
        try {
            boundary_=0x2cb634u;smoke160_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            boundary_=0x2cb644u;fire164_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            // This fresh piece's2CB4B0 initialized170=NULL, so allocation is
            //genuinely reached. No existing170 array is silently replaced.
            boundary_=0x2cb660u;anchors170_=std::make_unique<const O*[]>(count_);
        }catch(...){return false;}
        //3C2670 is exactly "damage", with no integer conversion. Source
        //passes the loop index to sprintf but resolves the same name each time.
        for(std::size_t i=0;i<count_;++i){
            const O* found=nullptr;boundary_=0x2cb694u;
            if(!findDamage28C590(model,graph,found))return false;
            anchors170_[i]=found;++anchorsWritten_; //2CB6AC; genuine miss=NULL
        }
        boundary_=0x2cb6ccu;return true; //before first external child allocation
    }
    bool release2CB500()noexcept {
        if(!attempted_||retired_)return false;
        //2CB534/548/55C free pointer arrays without reading/releasing pointees.
        //External children are separately owned; this owner never disposes them.
        smoke160_.reset();fire164_.reset();anchors170_.reset();
        retired_=true;boundary_=0x2cb568u;return true;
    }
public:
    OriginalWreckEffectArrays50LA()=default;
    OriginalWreckEffectArrays50LA(const OriginalWreckEffectArrays50LA&)=delete;
    OriginalWreckEffectArrays50LA& operator=(const OriginalWreckEffectArrays50LA&)=delete;
    ~OriginalWreckEffectArrays50LA(){if(smoke160_||fire164_||anchors170_)std::terminate();}
    bool attempted()const noexcept{return attempted_;}
    bool retired()const noexcept{return retired_;}
    std::size_t count()const noexcept{return count_;}
    std::size_t anchorsWritten()const noexcept{return anchorsWritten_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    // Never expose uninitialized smoke/fire entries as source NULL children.
};
}
