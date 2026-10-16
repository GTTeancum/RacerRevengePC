#pragma once
#include "original_standard_material_parameters50la.h"
#include "original_standard_program_table50la.h"
#include "original_ui_pages50kg.h"
#include "../game/original_ui_texture_animation50kg.h"
#include "original_standard_texture_resource50la.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <new>
#include <utility>
#include <vector>

namespace rr::game { class OriginalRacerModelSceneOwner50LA; }
namespace rr::assets {
class OriginalStandardMaterialObject50LA;
class OriginalStandardMaterialAnimation50LA;

enum class OriginalStandardMaterialAnimationStatus50LA : std::uint8_t {
    complete,phase,mode,program,parameters,document,uvLeaf,model,allocation
};
struct OriginalStandardMaterialAnimationDiagnostic50LA {
    OriginalStandardMaterialAnimationStatus50LA status=OriginalStandardMaterialAnimationStatus50LA::complete;
    std::uint32_t sourcePC=0;
    const char* field=nullptr;
    OriginalUiPageDiagnostic50KG document{};
};

// Native semantic replacement for PSXVertexAnimationData (RTTI3B7380,
// vtable3D2180). These are ordinary float rows, never VIF words, guest
// pointers, uploaded code, or VU execution. Only the three contiguous authored
// TexGen rows consumed by20EE90/20F110 are admitted here.
struct OriginalStandardVertexRows50LA {
    float rows[3][4]{};
};

// One29AB50 private clone. A pass owns this object at a stable native address;
// the banked racer arena may borrow its pointer but MUST NOT be stored here.
// Row/stamp state lives exclusively here on the joined material path. The
// immutable pass parameter34 data and animation prototype remain independent.
class OriginalStandardMaterialAnimationInstance50LA final {
public:
    ~OriginalStandardMaterialAnimationInstance50LA()=default;
    OriginalStandardMaterialAnimationInstance50LA(const OriginalStandardMaterialAnimationInstance50LA&)=delete;
    OriginalStandardMaterialAnimationInstance50LA& operator=(const OriginalStandardMaterialAnimationInstance50LA&)=delete;
    const OriginalStandardMaterialAnimation50LA* prototype() const noexcept {return prototype_;}
    const rr::game::OriginalRacerModelSceneOwner50LA* model() const noexcept {return model_;}
    std::uint32_t nativeModelId() const noexcept {return nativeModelId_;}
    const OriginalStandardVertexRows50LA& rows() const noexcept {return rows_;}
    std::uint32_t frameStamp08() const noexcept {return frameStamp08_;}
    std::uint32_t callback00() const noexcept {return callback00_;}
    std::uint32_t callbackOwner04() const noexcept {return callbackOwner04_;}
    bool animatesWhilePaused18() const noexcept {return animatesWhilePaused18_;}
    std::uint64_t revision() const noexcept {return revision_;}

    //29BD80/29BDE4 installs the callback in the record returned by29B1D0.
    // The caller supplies the result of the actual model/flame binding walk,
    // not a guessed side or a separate vehicle identity. Repeating the same
    // installation has no row/stamp/revision effect.
    bool canBindEngine29BD80(std::uint32_t callback,std::uint32_t vehicleOwner) const noexcept {
        return (callback==0x20EE90u||callback==0x20F110u)&&vehicleOwner&&
            ((callback00_==callback&&callbackOwner04_==vehicleOwner)||
             revision_!=std::numeric_limits<std::uint64_t>::max());
    }
    bool bindEngine29BD80(std::uint32_t callback,std::uint32_t vehicleOwner) noexcept {
        if(!canBindEngine29BD80(callback,vehicleOwner))return false;
        if(callback00_==callback&&callbackOwner04_==vehicleOwner)return true;
        callback00_=callback;callbackOwner04_=vehicleOwner;++revision_;return true;
    }

    //29AC90 gates BEFORE callbacks: repeated application timestamp, then
    // pause unless this authored controller explicitly permits it. A skipped
    // call does not write the stamp or consume RNG. Callback arithmetic is
    // already recovered in engine_flare50gv and remains there.
    bool shouldDispatch29AC90(std::uint32_t stamp,bool applicationPaused) const noexcept {
        return frameStamp08_!=stamp&&(!applicationPaused||animatesWhilePaused18_);
    }

    // Preflight all participating instances before committing an accepted
    // render. Revision is a native stale-result guard, not an original field.
    // The exact recovered engine callbacks may write only row2.y and row3.x;
    // all other authored row bits must survive unchanged.
    bool canCommitEngine29AD58(std::uint64_t expectedRevision,
        const OriginalStandardVertexRows50LA& candidate,std::uint32_t stamp) const noexcept {
        if(expectedRevision!=revision_||revision_==std::numeric_limits<std::uint64_t>::max()||
           stamp==0xFFFFFFFFu||!callbackOwner04_||
           (callback00_!=0x20EE90u&&callback00_!=0x20F110u))return false;
        for(std::size_t row=0;row<3;++row)for(std::size_t lane=0;lane<4;++lane){
            const float value=candidate.rows[row][lane];
            if(!std::isfinite(value)||std::fpclassify(value)==FP_SUBNORMAL)return false;
            if((row==1&&lane==1)||(row==2&&lane==0))continue;
            if(std::memcmp(&value,&rows_.rows[row][lane],sizeof(float)))return false;
        }
        // A repeated stamp cannot author new callback effects.
        return stamp!=frameStamp08_||!std::memcmp(&candidate,&rows_,sizeof(candidate));
    }
    bool commitEngine29AD58(std::uint64_t expectedRevision,
        const OriginalStandardVertexRows50LA& candidate,std::uint32_t stamp) noexcept {
        if(!canCommitEngine29AD58(expectedRevision,candidate,stamp))return false;
        if(stamp==frameStamp08_)return true;
        rows_=candidate;frameStamp08_=stamp;++revision_;return true;
    }
private:
    friend class OriginalStandardMaterialAnimation50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalStandardMaterialAnimationInstance50LA(
        const OriginalStandardMaterialAnimation50LA* prototype,
        const rr::game::OriginalRacerModelSceneOwner50LA* model,
        std::uint32_t nativeModelId,const OriginalStandardVertexRows50LA& rows,bool paused) noexcept
        :prototype_(prototype),model_(model),nativeModelId_(nativeModelId),rows_(rows),
         animatesWhilePaused18_(paused) {}
    const OriginalStandardMaterialAnimation50LA* prototype_=nullptr;
    const rr::game::OriginalRacerModelSceneOwner50LA* model_=nullptr;
    std::uint32_t nativeModelId_=0;
    OriginalStandardVertexRows50LA rows_{};
    std::uint32_t frameStamp08_=0xFFFFFFFFu,callback00_=0,callbackOwner04_=0;
    bool animatesWhilePaused18_=false;
    std::uint64_t revision_=0;
};

// Bounded genuine29B890/29BAF0 owner for the authored engine-flame domain:
// texture and pixel leaves absent; unique PSX vertex leaf; no uvs_animate
// subordinate controller. "unique" does not register a3833F8 named owner.
// Model registration is separately required by26EF10/29BCB0/29B050.
class OriginalStandardMaterialAnimation50LA final {
public:
    OriginalStandardMaterialAnimation50LA()=default;
    ~OriginalStandardMaterialAnimation50LA(){
        for(auto& instance:instances14_)instance.reset(); //29AE00 ascending registered entries.
        prototype10_.reset(); //29AEC4 only after all private clones.
    }
    OriginalStandardMaterialAnimation50LA(const OriginalStandardMaterialAnimation50LA&)=delete;
    OriginalStandardMaterialAnimation50LA& operator=(const OriginalStandardMaterialAnimation50LA&)=delete;
    bool constructed29B890() const noexcept {return constructed_;}
    bool unique1C() const noexcept {return constructed_;}
    bool animatesWhilePaused18() const noexcept {return animatesWhilePaused18_;}
    const OriginalStandardVertexRows50LA* prototypeRows() const noexcept {return constructed_?&prototypeRows_:nullptr;}
    const OriginalStandardMaterialAnimationInstance50LA* prototype10() const noexcept {return prototype10_.get();}
    const std::array<OriginalStandardMaterialParameter50LA,3>* vertexParameters() const noexcept {
        return constructed_?&vertexParameters_:nullptr;
    }
    std::size_t instanceCount14() const noexcept {return instances14_.size();}

    //29B050 duplicate suppression and sorted model identity lookup. The
    // selected model owner must outlive its borrowed registration. Each heap
    // clone remains stable when the pointer table grows or is reordered.
    bool registerModel29B050(const rr::game::OriginalRacerModelSceneOwner50LA& actualModel,
        std::uint32_t nativeModelId,OriginalStandardMaterialAnimationInstance50LA*& output,
        OriginalStandardMaterialAnimationDiagnostic50LA& diagnostic) noexcept {
        diagnostic={};
        if(!constructed_)return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::phase,0x29B064u);
        if(!nativeModelId)return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::model,0x29B174u);
        const auto less=std::less<const rr::game::OriginalRacerModelSceneOwner50LA*>{};
        std::size_t index=0;
        while(index<instances14_.size()&&less(instances14_[index]->model(),&actualModel))++index;
        if(index<instances14_.size()&&instances14_[index]->model()==&actualModel){
            if(instances14_[index]->nativeModelId()!=nativeModelId)
                return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::model,0x29B0F8u);
            output=instances14_[index].get();return true;
        }
        try {
            auto instance=std::unique_ptr<OriginalStandardMaterialAnimationInstance50LA>(
                new OriginalStandardMaterialAnimationInstance50LA(this,&actualModel,nativeModelId,
                    prototypeRows_,animatesWhilePaused18_));
            auto* result=instance.get();
            instances14_.insert(instances14_.begin()+index,std::move(instance));
            output=result;return true;
        }catch(const std::bad_alloc&){return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::allocation,0x29B170u);}
        catch(...){return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::allocation,0x29B104u);}
    }
    OriginalStandardMaterialAnimationInstance50LA* instance29B1D0(
        const rr::game::OriginalRacerModelSceneOwner50LA& actualModel) const noexcept {
        if(!constructed_)return nullptr;
        const auto less=std::less<const rr::game::OriginalRacerModelSceneOwner50LA*>{};
        for(const auto& instance:instances14_){
            if(instance->model()==&actualModel)return instance.get();
            if(less(&actualModel,instance->model()))break;
        }
        return nullptr; // Missing unique registration is never a prototype fallback.
    }
private:
    friend class OriginalStandardMaterialObject50LA;
    std::array<OriginalStandardMaterialParameter50LA,3> vertexParameters_{};
    OriginalStandardVertexRows50LA prototypeRows_{};
    bool animatesWhilePaused18_=false,constructed_=false;
    std::unique_ptr<OriginalStandardMaterialAnimationInstance50LA> prototype10_;
    //29ADC0 releases registered clones before the prototype.
    std::vector<std::unique_ptr<OriginalStandardMaterialAnimationInstance50LA>> instances14_;

    static char fold(char value) noexcept {return value>='A'&&value<='Z'?char(value+'a'-'A'):value;}
    static bool same(const std::string& text,const char* expected) noexcept {
        std::size_t index=0;
        for(;index<text.size()&&expected[index];++index)if(fold(text[index])!=fold(expected[index]))return false;
        return index==text.size()&&!expected[index];
    }
    static bool mode(const OriginalUiDatSection50KG& section,const char* key,const char* expected) {
        const auto* value=section.find(key);return value?same(value->text,expected):std::strcmp("none",expected)==0;
    }
    static bool fail(OriginalStandardMaterialAnimationDiagnostic50LA& diagnostic,
        OriginalStandardMaterialAnimationStatus50LA status,std::uint32_t pc,const char* field=nullptr) noexcept {
        diagnostic.status=status;diagnostic.sourcePC=pc;diagnostic.field=field;return false;
    }
    bool construct29B890(const OriginalUiDatSection50KG& section,
        const OriginalStandardMaterialParameters50LA& parameters,
        const OriginalStandardProgramTable50LA& programs,
        OriginalStandardMaterialAnimationDiagnostic50LA& diagnostic) noexcept {
        diagnostic={};
        try {
        if(constructed_)return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::phase,0x29B890u);
        if(!mode(section,"TextureAnimation","none"))
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::mode,0x29B950u,"TextureAnimation");
        if(!mode(section,"VertexAnimation","unique"))
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::mode,0x29BB28u,"VertexAnimation");
        if(!mode(section,"PixelAnimation","none"))
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::mode,0x29BBD0u,"PixelAnimation");
        const auto* programName=section.find("microProgramName");
        std::int16_t program=-1;OriginalStandardProgramDiagnostic50LA programDiagnostic;
        if(!programName||!same(programName->text,"DiffuseTexTransCompressed")||
           !programs.find298C80(programName->text,program,programDiagnostic)||program!=43)
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::program,0x2471BCu,"microProgramName");
        bool uvAnimation=false,paused=false;
        if(!originalUiBoolean50KG(section,"uvs_animate",false,uvAnimation,diagnostic.document,0x29A354u))
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::document,0x29A354u,"uvs_animate");
        if(uvAnimation)return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::uvLeaf,0x29A368u,"uvs_animate");
        if(!originalUiBoolean50KG(section,"animatesWhilePaused",false,paused,diagnostic.document,0x29AB30u))
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::document,0x29AB30u,"animatesWhilePaused");
        //246B70's row14/18/1C pointers address the first three present
        // parameter records in registry order, not a name-to-row repair.
        if(parameters.size()!=3)
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::parameters,0x246B80u,"three contiguous TexGen rows");
        for(std::size_t row=0;row<3;++row){
            OriginalStandardProgramParameterRowView50LA registry;
            if(!programs.parameterRow382B78(static_cast<std::int32_t>(row),registry)||
               parameters[row].row382B78!=static_cast<std::int16_t>(row)||
               registry.value!=static_cast<std::int32_t>(28+row)||parameters[row].destination!=registry.value)
                return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::parameters,0x246A48u,"ordered TexGen destinations");
            for(std::size_t lane=0;lane<4;++lane){
                const float value=parameters[row].value[lane];
                if(!std::isfinite(value)||std::fpclassify(value)==FP_SUBNORMAL)
                    return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::parameters,0x246A80u,"finite vertex rows");
            }
        }
        for(std::size_t row=0;row<3;++row){
            vertexParameters_[row]=parameters[row];
            for(std::size_t lane=0;lane<4;++lane)prototypeRows_.rows[row][lane]=parameters[row].value[lane];
        }
        //29AAA0 owns a real prototype even before any26EF10 registration.
        // Its callback/context are null and prior timestamp is-1.
        prototype10_=std::unique_ptr<OriginalStandardMaterialAnimationInstance50LA>(
            new OriginalStandardMaterialAnimationInstance50LA(this,nullptr,0,prototypeRows_,paused));
        animatesWhilePaused18_=paused;constructed_=true;return true;
        }catch(const std::bad_alloc&){
            return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::allocation,0x29B8F4u);
        }catch(...){return fail(diagnostic,OriginalStandardMaterialAnimationStatus50LA::allocation,0x29B8F4u);}
    }
};
//Distinct29B950 texture leaf and29AAA0 prototype domain. This bounded owner
//has no callback, vertex/pixel leaf, named-manager registration or racer clone.
//Only its actual material pass can construct it.29B1D0 selects this SAME
//prototype for a null ScreenTexture key OR an actual model key when table14
//is absent. No model registration/callback path is implemented by this owner;
//DefaultMT prototype construction does not certify a model draw/service join.
class OriginalStandardMaterialTextureAnimation50LA final {
public:
    OriginalStandardMaterialTextureAnimation50LA()=default;
    OriginalStandardMaterialTextureAnimation50LA(const OriginalStandardMaterialTextureAnimation50LA&)=delete;
    OriginalStandardMaterialTextureAnimation50LA& operator=(const OriginalStandardMaterialTextureAnimation50LA&)=delete;
    bool unique18() const noexcept {return constructed_;}
    bool animatesWhilePaused18() const noexcept {return paused_;}
    std::uint32_t frameStamp08() const noexcept {return stamp08_;}
    const OriginalStandardMaterialObject50LA* materialOwner() const noexcept {return material_;}
    std::size_t passOrdinal() const noexcept {return pass_;}
private:
    friend class OriginalStandardMaterialObject50LA;
    const OriginalStandardMaterialObject50LA* material_=nullptr;
    std::size_t pass_=0;
    rr::game::OriginalUiTextureAnimation50KG state_{};
    std::uint32_t stamp08_=0xFFFFFFFFu;
    bool paused_=false,constructed_=false;
    bool construct29B950(const OriginalUiDatSection50KG& section,
        const OriginalStandardMaterialObject50LA& material,std::size_t pass,
        OriginalStandardMaterialAnimationDiagnostic50LA& d) noexcept {
        d={};
        try {
        const auto fail=[&](std::uint32_t pc,const char* field){
            d.status=OriginalStandardMaterialAnimationStatus50LA::document;d.sourcePC=pc;d.field=field;return false;
        };
        if(constructed_)return fail(0x29B950,"texture prototype already constructed");
        auto config=rr::game::OriginalUiTextureAnimationConfig50KG{};
        if(!originalUiNumber50KG(section,"animatedMapInitialFrame",0.f,config.initialFrame,d.document,0x29B9DC))
            return fail(0x29B9DC,"animatedMapInitialFrame");
        if(!originalUiNumber50KG(section,"animatedMapFramesPerSecond",1.f,config.framesPerSecond,d.document,0x29B9F8))
            return fail(0x29B9F8,"animatedMapFramesPerSecond");
        if(!originalUiBoolean50KG(section,"animatedMapLooping",true,config.looping,d.document,0x29BA24))
            return fail(0x29BA24,"animatedMapLooping");
        if(!originalUiBoolean50KG(section,"animatedMapReverseLoopOnEnd",false,config.reverseLoopOnEnd,d.document,0x29BA50))
            return fail(0x29BA50,"animatedMapReverseLoopOnEnd");
        if(!originalUiInteger50KG(section,"animatedMapLoopOnEndToFrame",-1,config.loopOnEndToFrame,d.document,0x29BA7C))
            return fail(0x29BA7C,"animatedMapLoopOnEndToFrame");
        if(!originalUiBoolean50KG(section,"animatesWhilePaused",false,paused_,d.document,0x29AB30))
            return fail(0x29AB30,"animatesWhilePaused");
        //Count is deliberately unavailable until the actual383400 aggregate
        //is populated. Do not use a filename, decoded bank or guessed9 here.
        if(!std::isfinite(config.initialFrame)||config.initialFrame<0.f||config.initialFrame>=64.f||
           !std::isfinite(config.framesPerSecond)||config.framesPerSecond<=0.f||config.framesPerSecond>60.f||
           config.loopOnEndToFrame< -1||config.loopOnEndToFrame>=64)
            return fail(0x29B950,"bounded authored animation values");
        state_.config=config;state_.phase=config.initialFrame;
        material_=&material;pass_=pass;constructed_=true;return true;
        }catch(...){d.status=OriginalStandardMaterialAnimationStatus50LA::allocation;
            d.sourcePC=0x29B950;return false;}
    }
    bool select29AF90(const OriginalStandardTextureResource50LA& aggregate,
        const OriginalStandardTextureResource50LA*& selected) const noexcept {
        selected=nullptr;
        const auto* count=aggregate.frameCount20();
        if(!constructed_||!count||*count<=0)return false;
        auto current=state_;current.frameCount=static_cast<std::uint32_t>(*count);
        std::uint32_t ordinal=0;
        if(!rr::game::selectOriginalUiTextureAnimationFrame50KG(current,ordinal))return false;
        selected=aggregate.frame102050(static_cast<std::int32_t>(ordinal));return selected!=nullptr;
    }
};
} // namespace rr::assets
