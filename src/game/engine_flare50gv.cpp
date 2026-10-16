#include "engine_flare50gv.h"
#include "original_racer_model_scene50la.h"
#include "../assets/original_standard_material_animation50la.h"
#include "../assets/original_standard_material_table50la.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>

namespace rr::game {
namespace {
float fp(std::uint32_t word) noexcept { float value;std::memcpy(&value,&word,4);return value; }
std::uint32_t bits(float value) noexcept { std::uint32_t word;std::memcpy(&word,&value,4);return word; }
bool finite(float value) noexcept {const auto word=bits(value),e=word&0x7f800000u;return e!=0x7f800000u&&(e||(word&0x7fffffu)==0);}
struct Math {
    bool good=true;
    float check(float value) noexcept {if(!finite(value))good=false;return value;}
    float add(float a,float b) noexcept {volatile float v=a+b;return check(v);}
    float sub(float a,float b) noexcept {volatile float v=a-b;return check(v);}
    float mul(float a,float b) noexcept {volatile float v=a*b;return check(v);}
    float div(float a,float b) noexcept {volatile float v=a/b;return check(v);}
};
bool affine(const assets::FlameUniformRows50W& u) noexcept {
    for(const auto& row:u.rows)for(float v:row)if(!finite(v))return false;
    // Columns carried by the source TexGen rows form a 3x3 affine ST matrix.
    return u.rows[0][2]==0.f&&u.rows[1][2]==0.f&&u.rows[2][2]==1.f&&
           u.rows[0][3]==0.f&&u.rows[1][3]==0.f&&u.rows[2][3]==0.f;
}
void exportUV(const assets::FlameUniformRows50W& u,float(&uv)[6]) noexcept {
    uv[0]=u.rows[0][0];uv[1]=u.rows[1][0];uv[2]=u.rows[2][0];
    uv[3]=u.rows[0][1];uv[4]=u.rows[1][1];uv[5]=u.rows[2][1];
}
assets::OriginalStandardVertexRows50LA nativeRows(const assets::FlameUniformRows50W& source) noexcept {
    assets::OriginalStandardVertexRows50LA result;
    for(unsigned row=0;row<3;++row)for(unsigned lane=0;lane<4;++lane)
        result.rows[row][lane]=source.rows[row][lane];
    return result;
}
}
bool bindCanonicalEngineFlare50LA(assets::PodFlameOwners50W& owners,
    const OriginalRacerModelSceneOwner50LA& actualModel,
    const assets::OriginalStandardMaterialReferences50LA& references,
    unsigned modelOrdinal,EngineFlareDiagnostic50GV& diagnostic) noexcept {
    diagnostic={};
    const unsigned side=modelOrdinal&1u;
    const auto fail=[&](std::uint32_t pc){diagnostic={EngineFlareError50GV::binding,side,pc};return false;};
    if(owners.modelCount<2||owners.modelCount>assets::podFlameModelCapacity50W||
       modelOrdinal>=owners.modelCount||
       !actualModel.constructed2403A0()||!references.complete()||references.released26E4C0())
        return fail(0x26EF10u);
    const auto* sceneModel=actualModel.model();const auto& fields=actualModel.fields();
    const auto& model=owners.models[modelOrdinal];const auto& record=owners.records[modelOrdinal+2];
    const auto* count=references.count1C0();const auto* table=references.table382B38();
    if(!sceneModel||sceneModel->sceneIndex==SceneNodeIndex49P::none||
       model.modelId!=static_cast<std::uint32_t>(sceneModel->sceneIndex)+1u||
       model.flameFilter!=side+1u||!count||!table||fields.materialCount1C0<0||
       *count!=static_cast<std::uint32_t>(fields.materialCount1C0)||*count!=model.referenceCount||
       !model.referenceCount||model.referenceCount>assets::podFlameReferencesPerModel50W||
       model.firstReference>assets::podFlameReferenceCapacity50W||
       model.referenceCount>assets::podFlameReferenceCapacity50W-model.firstReference||
       fields.materials1C4!=references.handle1C4(0))return fail(0x26EF3Cu);
    const bool callbackAbsent=!record.words[0]&&!record.words[1];
    //220978/2209BC selects bottom-left1C4 with20F110;2209D4/220A18
    // selects bottom-right1C0 with20EE90. buildPodFlameOwners50W retains
    // L/R/BL/BR model order, two shared templates and records[2+model].
    // Authenticate that retained record/model association, never infer an
    // engine side from its scene index or register a lower clone as its upper.
    if(record.words[4]!=modelOrdinal+3u||record.words[6]!=side+1u||record.words[7]!=1u)
        return fail(0x29BDE4u);
    for(unsigned other=0;other<owners.modelCount;++other){
        const auto& paired=owners.records[other+2];
        if(!owners.models[other].modelId||
           (other!=modelOrdinal&&owners.models[other].modelId==model.modelId)||
           (callbackAbsent?(paired.words[0]||paired.words[1]):
            (paired.words[0]!=((other&1u)?0x20EE90u:0x20F110u)||
             !paired.words[1]||paired.words[1]!=record.words[1])))return fail(0x29BDE4u);
    }
    assets::OriginalStandardMaterialAnimation50LA* animation=nullptr;
    const assets::OriginalStandardMaterialParameters50LA* parameters=nullptr;
    std::uint32_t matches=0;
    // Preflight the same retained material slots before any clone allocation.
    // Static slots still require their actual canonical handles; matching
    // multiplicity is read from this source walk, never supplied as four.
    for(std::uint32_t slot=0;slot<model.referenceCount;++slot){
        const auto* handle=references.handle1C4(slot);
        if(!handle||handle!=fields.materials1C4+slot)return fail(0x26EF48u);
        const auto localMaterial=owners.references[model.firstReference+slot];
        if(localMaterial>=3)return fail(0x26EF50u);
        if(localMaterial!=model.flameFilter)continue;
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
        if(!table->resolve298E60(*handle,material,materialDiagnostic)||!material||
           !material->constructed29B550()||!material->row40()||*material->row40()!=*handle||
           !material->passCount48()||*material->passCount48()!=1)return fail(0x26EF5Cu);
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!material->pass4C(0,pass)||!pass.animation10||!pass.animation10->constructed29B890()||
           !pass.animation10->unique1C()||!pass.program18||*pass.program18!=43||
           !pass.mapCount20||*pass.mapCount20!=1||!pass.parameterCount28||*pass.parameterCount28!=3||
           !pass.parameters34||pass.parameters34->size()!=3)return fail(0x29BCE0u);
        if(animation&&(animation!=pass.animation10||parameters!=pass.parameters34))return fail(0x29B050u);
        // Cold bind the actual immutable origin payload to the vertex leaf's
        // retained destination order. Never retain the constructor's temporary
        // parameter vector, or substitute a merged representative's seeds.
        const auto* vertexParameters=pass.animation10->vertexParameters();
        if(!vertexParameters)return fail(0x246B80u);
        for(unsigned row=0;row<3;++row){
            const auto& seed=(*pass.parameters34)[row];
            const auto& vertex=(*vertexParameters)[row];
            if(seed.row382B78!=static_cast<std::int16_t>(row)||seed.destination!=28+static_cast<std::int32_t>(row)||
               vertex.row382B78!=seed.row382B78||vertex.destination!=seed.destination||
               std::memcmp(seed.value.data(),vertex.value.data(),sizeof(float)*4u))return fail(0x249420u);
        }
        animation=pass.animation10;parameters=pass.parameters34;++matches;
    }
    if(!matches||!animation)return fail(0x29BE10u);
    auto* existing=owners.canonicalInstances50LA[modelOrdinal];
    if(existing){
        return (existing->prototype()==animation&&existing->model()==&actualModel&&
            existing->nativeModelId()==model.modelId&&animation->instance29B1D0(actualModel)==existing&&
            owners.canonicalParameters50LA[modelOrdinal]==parameters&&
            existing->callback00()==record.words[0]&&existing->callbackOwner04()==record.words[1])||
            fail(0x29B1D0u);
    }
    if(owners.canonicalParameters50LA[modelOrdinal])return fail(0x29B1D0u);
    const auto* prototype=animation->prototypeRows();
    if(!prototype||record.words[2]!=0xFFFFFFFFu||
       std::memcmp(prototype->rows,owners.uniforms[side].rows,sizeof(prototype->rows))||
       std::memcmp(prototype->rows,owners.uniforms[modelOrdinal+2].rows,sizeof(prototype->rows)))return fail(0x29AB50u);
    // A borrow cleared during teardown cannot be used to reset a live clone.
    if(const auto* registered=animation->instance29B1D0(actualModel))
        if(registered->frameStamp08()!=0xFFFFFFFFu||
           (callbackAbsent&&(registered->callback00()||registered->callbackOwner04()))||
           std::memcmp(registered->rows().rows,prototype->rows,sizeof(prototype->rows)))return fail(0x29B0F8u);
    assets::OriginalStandardMaterialAnimationInstance50LA* instance=nullptr;
    assets::OriginalStandardMaterialAnimationDiagnostic50LA animationDiagnostic;
    for(std::uint32_t slot=0;slot<model.referenceCount;++slot){
        if(owners.references[model.firstReference+slot]!=model.flameFilter)continue;
        if(!animation->registerModel29B050(actualModel,model.modelId,instance,animationDiagnostic))
            return fail(animationDiagnostic.sourcePC);
    }
    if(!instance)return fail(0x29B198u);
    for(std::uint32_t slot=0;slot<model.referenceCount;++slot){
        if(owners.references[model.firstReference+slot]!=model.flameFilter)continue;
        if(!callbackAbsent&&!instance->bindEngine29BD80(record.words[0],record.words[1]))return fail(0x29BDE4u);
    }
    owners.canonicalParameters50LA[modelOrdinal]=parameters;
    owners.canonicalInstances50LA[modelOrdinal]=instance;return true;
}
bool bindCanonicalLeftEngineFlare50LA(assets::PodFlameOwners50W& owners,
    const OriginalRacerModelSceneOwner50LA& actualModel,
    const assets::OriginalStandardMaterialReferences50LA& references,
    EngineFlareDiagnostic50GV& diagnostic) noexcept {
    return bindCanonicalEngineFlare50LA(owners,actualModel,references,0u,diagnostic);
}
bool bindCanonicalRightEngineFlare50LA(assets::PodFlameOwners50W& owners,
    const OriginalRacerModelSceneOwner50LA& actualModel,
    const assets::OriginalStandardMaterialReferences50LA& references,
    EngineFlareDiagnostic50GV& diagnostic) noexcept {
    return bindCanonicalEngineFlare50LA(owners,actualModel,references,1u,diagnostic);
}
void clearCanonicalEngineFlareBorrows50LA(assets::PodFlameOwners50W& owners,
    const OriginalRacerModelSceneOwner50LA& actualModel) noexcept {
    for(unsigned model=0;model<assets::podFlameModelCapacity50W;++model){
        auto*& instance=owners.canonicalInstances50LA[model];
        if(instance&&instance->model()==&actualModel){
            owners.canonicalParameters50LA[model]=nullptr;instance=nullptr;
        }
    }
}
bool advanceEngineFlare50GV(const VehicleFrameState49W& frame,
    const BaseTailState50X& tail,const assets::PodFlameOwners50W& owners,
    std::uint32_t stamp,EngineFlareOutput50GV& output,
    EngineFlareDiagnostic50GV& diagnostic) noexcept {
    diagnostic={};
    const auto fail=[&](EngineFlareError50GV e,std::uint32_t side,std::uint32_t pc){diagnostic={e,side,pc};return false;};
    if(frame.phase!=VehicleFramePhase49W::Ready&&frame.phase!=VehicleFramePhase49W::PostGraphics)
        return fail(EngineFlareError50GV::phase,0,0x29ac90);
    if(std::fegetround()!=FE_TONEAREST)return fail(EngineFlareError50GV::roundingMode,0,0);
    const auto& main=frame.bodies.main;
    if(main.applicationActive11C)return fail(EngineFlareError50GV::unsupportedState,0,0x29acb8);
    if(stamp==0xffffffffu)return fail(EngineFlareError50GV::input,0,0x29acac);
    if(owners.modelCount<2||owners.modelCount>assets::podFlameModelCapacity50W)
        return fail(EngineFlareError50GV::binding,0,0x29acec);
    const auto owner=owners.records[2].words[1];
    if(!owner)return fail(EngineFlareError50GV::binding,0,0x29acec);
    for(unsigned model=0;model<owners.modelCount;++model){
        const auto& record=owners.records[model+2];
        if(!owners.models[model].modelId||record.words[1]!=owner||
            record.words[0]!=((model&1u)?0x20ee90u:0x20f110u))
            return fail(EngineFlareError50GV::binding,model&1u,0x29acec);
        for(unsigned earlier=0;earlier<model;++earlier)
            if(owners.models[earlier].modelId==owners.models[model].modelId)
                return fail(EngineFlareError50GV::binding,model&1u,0x29acec);
        const auto* instance=owners.canonicalInstances50LA[model];
        if(bool(instance)!=bool(owners.canonicalParameters50LA[model]))
            return fail(EngineFlareError50GV::binding,model&1u,0x24ba24u);
        if(instance&&(!instance->prototype()||!instance->prototype()->constructed29B890()||
           !instance->model()||!instance->model()->constructed2403A0()||
           instance->prototype()->instance29B1D0(*instance->model())!=instance||
           instance->nativeModelId()!=owners.models[model].modelId||
           instance->callback00()!=record.words[0]||instance->callbackOwner04()!=record.words[1]))
            return fail(EngineFlareError50GV::binding,model&1u,0x29b1d0);
    }
    EngineFlareOutput50GV next{};next.modelCount=owners.modelCount;
    next.sharedRandomState=frame.noiseOwner().sharedRandomState;
    for(unsigned side=0;side<2;++side)next.flameAC4[side]=tail.flameAC4[side];
    const bool steering=main.components.aero.lowSpeedDc&&(frame.fields.applicationByte58&2u);
    for(unsigned model=0;model<owners.modelCount;++model){
        const unsigned side=model&1u;
        auto* instance=owners.canonicalInstances50LA[model];
        next.canonicalInstances50LA[model]=instance;
        next.canonicalParameters50LA[model]=owners.canonicalParameters50LA[model];
        if(instance){
            next.canonicalRevisions50LA[model]=instance->revision();
            for(unsigned row=0;row<3;++row)for(unsigned lane=0;lane<4;++lane)
                next.uniforms[model].rows[row][lane]=instance->rows().rows[row][lane];
            next.frameStamp[model]=instance->frameStamp08();
        }else{
            next.uniforms[model]=owners.uniforms[model+2];
            next.frameStamp[model]=owners.records[model+2].words[2];
        }
        if(!affine(next.uniforms[model]))return fail(EngineFlareError50GV::input,side,0x246b70);
        if(instance?!instance->shouldDispatch29AC90(stamp,false):next.frameStamp[model]==stamp){
            exportUV(next.uniforms[model],next.uv[model]);continue;
        }
        Math math;
        float magnitude=0.f;
        if(steering){
            float drive=main.controls.player.raw.values[0];
            if(!finite(drive))return fail(EngineFlareError50GV::input,side,side?0x20ef54:0x20f1d4);
            if(side)drive=-drive;
            magnitude=math.mul(6000.f,drive>0.f?drive:0.f);
        }else{
            const auto& thrust=main.components.engine.localThrust[side].v;
            for(unsigned axis=0;axis<3;++axis)if(!finite(thrust[axis]))
                return fail(EngineFlareError50GV::input,side,side?0x20ef88:0x20f204);
            const float yy=math.mul(thrust[1],thrust[1]);
            const float xx=math.mul(thrust[0],thrust[0]);
            const float xy=math.add(xx,yy);
            const float xyz=math.add(xy,math.mul(thrust[2],thrust[2]));
            magnitude=math.check(std::sqrt(xyz));
        }
        next.thrustMagnitude[model]=magnitude;
        const bool active=(side?tail.flagsACC:tail.flagsAD0)!=0;
        if(active){
            const float old=fp(next.flameAC4[side]);
            if(!finite(old))return fail(EngineFlareError50GV::input,side,side?0x20efc8:0x20f244);
            const float rate=magnitude<old?fp(0x3d4ccccdu):fp(0x3e19999au);
            magnitude=math.add(old,math.mul(rate,math.sub(magnitude,old)));
            next.flameAC4[side]=bits(magnitude);
        }
        float normalized=math.div(active?math.add(4000.f,magnitude):magnitude,10000.f);
        if(active&&normalized<fp(0x3f19999au))normalized=fp(0x3f19999au);
        //308030 returns masked31-bit result, but stores the complete32-bit LCG state.
        next.sharedRandomState=next.sharedRandomState*0x41c64e6du+0x3039u;
        volatile float converted=static_cast<float>(next.sharedRandomState&0x7fffffffu);
        const float random=math.mul(fp(0x30000000u),converted);
        const float scale=math.add(math.div(normalized,fp(0x3f8ccccdu)),math.mul(fp(0x3dcccccdu),random));
        next.uniforms[model].rows[1][1]=scale;
        float scroll=math.add(next.uniforms[model].rows[2][0],fp(0x3de353f8u));
        if(scroll>1.f)scroll=math.sub(scroll,1.f);
        next.uniforms[model].rows[2][0]=scroll;
        if(!math.good)return fail(EngineFlareError50GV::arithmetic,side,side?0x20f0a8:0x20f324);
        next.frameStamp[model]=stamp;++next.callbacks;++next.randomDraws;
        exportUV(next.uniforms[model],next.uv[model]);
    }
    output=next;return true;
}
bool exportEngineFlareUv50LA(const EngineFlareOutput50GV& candidate,
    unsigned model,float (&uv)[6]) noexcept {
    if(candidate.modelCount<2||candidate.modelCount>assets::podFlameModelCapacity50W||
       model>=candidate.modelCount)return false;
    const auto* instance=candidate.canonicalInstances50LA[model];
    const auto* parameters=candidate.canonicalParameters50LA[model];
    if(!instance){
        if(parameters)return false;
        for(unsigned lane=0;lane<6;++lane)uv[lane]=candidate.uv[model][lane];
        return true;
    }
    if(!parameters||parameters->size()!=3||
       instance->revision()!=candidate.canonicalRevisions50LA[model])return false;
    assets::FlameUniformRows50W applied{};
    //24B3D0..24B404 applies origin seeds before24B408's per-model vertex
    // overlay. The immutable destinations were bound once from THIS pass.
    for(unsigned row=0;row<3;++row){
        const auto& seed=(*parameters)[row];
        if(seed.destination<28||seed.destination>30)return false;
        const auto destination=seed.destination-28;
        std::memcpy(applied.rows[destination],seed.value.data(),sizeof applied.rows[0]);
    }
    for(unsigned row=0;row<3;++row){
        const auto destination=(*parameters)[row].destination-28;
        std::memcpy(applied.rows[destination],candidate.uniforms[model].rows[row],sizeof applied.rows[0]);
    }
    if(!affine(applied))return false;
    exportUV(applied,uv);return true;
}
void commitEngineFlare50GV(const EngineFlareOutput50GV& result,VehicleFrameState49W& frame,
    BaseTailState50X& tail,assets::PodFlameOwners50W& owners) noexcept {
    // Successful projection against these same owners is the caller contract.
    if(result.modelCount<2||result.modelCount>assets::podFlameModelCapacity50W||
        result.modelCount!=owners.modelCount)return;
    // No accepted candidate may be committed to another racer's clone, or
    // after an intervening callback binding/row update. Preflight ALL joined
    // targets before changing any canonical state, source RNG, or legacy rows.
    for(unsigned model=0;model<result.modelCount;++model){
        auto* instance=owners.canonicalInstances50LA[model];
        if(instance!=result.canonicalInstances50LA[model]||
           owners.canonicalParameters50LA[model]!=result.canonicalParameters50LA[model])std::terminate();
        if(instance&&(!instance->canCommitEngine29AD58(result.canonicalRevisions50LA[model],
                nativeRows(result.uniforms[model]),result.frameStamp[model])||
           instance->callback00()!=owners.records[model+2].words[0]||
           instance->callbackOwner04()!=owners.records[model+2].words[1]))std::terminate();
    }
    for(unsigned side=0;side<2;++side)tail.flameAC4[side]=result.flameAC4[side];
    for(unsigned model=0;model<result.modelCount;++model){
        if(auto* instance=owners.canonicalInstances50LA[model]){
            if(!instance->commitEngine29AD58(result.canonicalRevisions50LA[model],
                nativeRows(result.uniforms[model]),result.frameStamp[model]))std::terminate();
        }else{
            owners.uniforms[model+2]=result.uniforms[model];
            owners.records[model+2].words[2]=result.frameStamp[model];
        }
    }
    frame.noiseOwner().sharedRandomState=result.sharedRandomState;
}
}
