#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/game/authored_texture_animation.h"
#include <array>

namespace ga_atm_animation_fixture {
namespace a=rr::assets;
constexpr std::uint32_t sourceAnimationCount=4, animationCount=57, frameTextureCount=35;
struct Diagnostic {
    std::uint32_t animation=a::unresolvedMaterialTexture50EA, frame=a::unresolvedMaterialTexture50EA;
    std::string message;
    a::DecodeError48C source{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
struct Animation {
    std::uint32_t sourceSpanIndex=0, worldInstanceIndex=0, materialSelection=0;
    std::uint32_t sourceTextureIndex=a::unresolvedMaterialTexture50EA;
    std::uint32_t firstTextureIndex=0, frameCount=0, initialFrame=0;
    float framesPerSecond=0.f;
    bool clampU=false,clampV=false;
};
namespace detail {
namespace shared=pod_materials50ed_fixture::detail;
struct Entry {
    const char* name; const char* sourceSha; const char* materialSha;
    std::uint32_t sourceBytes,materialBytes,frameCount,firstTextureIndex,initialFrame;
    float fps;
};
struct Frame {
    std::uint32_t offset,bytes,width,height,mip;
    const char* sourceSha; const char* nativeSha;
};
#if __has_include("ga_atm_animation_index.inc") && __has_include("ga_atm_frame_index.inc")
inline constexpr bool metadataAvailable=true;
inline constexpr Entry entries[]={
#include "ga_atm_animation_index.inc"
};
inline constexpr Frame frames[]={
#include "ga_atm_frame_index.inc"
};
static_assert(sizeof(entries)/sizeof(entries[0])==sourceAnimationCount,"Exact GA ATM closure");
static_assert(sizeof(frames)/sizeof(frames[0])==frameTextureCount,"Exact stored original ATM frames");
#else
inline constexpr bool metadataAvailable=false;
inline constexpr std::array<Entry,sourceAnimationCount> entries{};
inline constexpr std::array<Frame,frameTextureCount> frames{};
#endif
inline std::string field(const a::MaterialFields48E& fields,const char* name,const char* fallback) {
    std::string value=fallback;bool found=false;
    for(const auto& f:fields.authored)if(f.name==name){shared::need(!found,"Repeated ATM material field");value=f.value;found=true;}
    return value;
}
}
class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::array<Animation,animationCount> animations;
        std::array<rr::game::AuthoredTextureAnimation,animationCount> state{};
        std::array<std::uint32_t,animationCount> selectedFrames{};
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::array<Animation,animationCount>* animations()const noexcept{return storage_?&storage_->animations:nullptr;}
    const std::array<std::uint32_t,animationCount>* selectedFrames()const noexcept{return storage_?&storage_->selectedFrames:nullptr;}
    // First caller timestamp retains authored initial frames. Subsequent calls
    // consume the existing common world timestamp; a repeated timestamp does
    // not advance. The caller owns pause policy; no wall-clock gap is silently
    // subtracted here. Restart resets this state to original initial frames.
    bool advance(std::uint32_t sourceMilliseconds)noexcept {
        if(!storage_)return false;
        auto states=storage_->state;auto frames=storage_->selectedFrames;
        for(std::uint32_t i=0;i<animationCount;++i)
            if(!rr::game::advanceAuthoredTextureAnimation(states[i],sourceMilliseconds,
                storage_->animations[i].framesPerSecond,storage_->animations[i].frameCount,frames[i]))return false;
        storage_->state=states;storage_->selectedFrames=frames;return true;
    }
    void reset()noexcept {
        if(!storage_)return;
        for(std::uint32_t i=0;i<animationCount;++i){
            storage_->state[i]={float(storage_->animations[i].initialFrame),0,false};
            storage_->selectedFrames[i]=storage_->animations[i].initialFrame;
        }
    }
    // The original static selections already authenticate each actual material
    // and initial frame. Recheck those associations before adding frame banks.
    bool load(const std::filesystem::path& sourceArchive,
        const track_materials50fa_fixture::Owner& track,Diagnostic& d)noexcept {
        return load<track_materials50fa_fixture::Owner>(sourceArchive,track,d);
    }
    // Narrow read-only static-owner contract: loaded(), bank(), selections()
    // and spans(). Both retained and direct-original owners expose the same
    // authenticated selection/span records. No owner is copied or retained.
    template<class StaticMaterialOwner>
    bool load(const std::filesystem::path& sourceArchive,
        const StaticMaterialOwner& track,Diagnostic& d)noexcept {
        using namespace detail;using shared::need;
        try{
            d={};need(metadataAvailable,"GA ATM profiles have not been cold-prepared and reviewed");
            need(!storage_ && track.loaded(),"GA ATM requires a loaded static source material owner");
            const auto* bank=track.bank();const auto* selections=track.selections();const auto* spans=track.spans();
            need(bank&&selections&&spans,"GA ATM static source owner bank/selections/spans unavailable");
            shared::Inputs inputs;inputs.capacity=8u*1024u*1024u;
            auto bytes=inputs.read(sourceArchive,2834942);
            need(bytes.size()==2834942&&shared::sha256(bytes)==
                "063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9","GA ATM original archive identity");
            a::ResArchive48C archive;a::DecodeLimits48C limits;
            limits.resource_file_bytes=2834942;limits.resource_decoded_bytes=26099712;limits.resource_entries=970;
            need(a::decodeResource48C(bytes,archive,d.source,nullptr,limits),"GA ATM original archive decode");
            std::unique_ptr<Storage> next(new Storage);
            std::array<Animation,sourceAnimationCount> definitions{};
            std::array<std::vector<std::uint8_t>,frameTextureCount> native;
            std::array<a::MaterialTextureAssetInput50EA,frameTextureCount> images{};
            for(std::uint32_t i=0;i<sourceAnimationCount;++i){
                d.animation=i;const auto& entry=entries[i];auto& out=definitions[i];
                out.firstTextureIndex=entry.firstTextureIndex;out.frameCount=entry.frameCount;
                out.initialFrame=entry.initialFrame;out.framesPerSecond=entry.fps;
                const std::string stem=std::string("textures\\")+entry.name;
                std::vector<std::uint8_t> original,psm;
                need(a::extractResMember48C(archive,stem+".atm",original,d.source,512u*1024u)&&
                    original.size()==entry.sourceBytes&&shared::sha256(original)==entry.sourceSha,
                    "GA original ATM identity");
                need(original.size()>=12&&shared::u32(original.data())==1&&
                    shared::u32(original.data()+4)==entry.frameCount&&shared::u32(original.data()+8)==0,
                    "GA original ATM frame header");
                need(a::extractResMember48C(archive,stem+".psm",psm,d.source,4096)&&
                    psm.size()==entry.materialBytes&&shared::sha256(psm)==entry.materialSha,
                    "GA original animated PSM identity");
                a::Material48E material;
                need(a::decodeMaterial48E(psm,material,d.source)&&material.passes.size()==1,
                    "GA original animated material decode");
                const auto& pass=material.passes[0];
                need(pass.pass==1&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==entry.name&&
                    field(pass.fields,"TextureAnimation","none")=="unique"&&
                    field(pass.fields,"animatedMapInitialFrame","0")==std::to_string(entry.initialFrame)&&
                    field(pass.fields,"animatedMapFramesPerSecond","1")==std::to_string(unsigned(entry.fps))&&
                    field(pass.fields,"animatedMapLooping","1")=="1"&&
                    field(pass.fields,"animatedMapReverseLoopOnEnd","0")=="0"&&
                    field(pass.fields,"animatedMapLoopOnEndToFrame","-1")=="0",
                    "GA authored animation scope/rate/start/loop policy");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+entry.materialSha);
                bool found=false;
                for(const auto& selection:*selections){
                    if(selection.textureName!=entry.name)continue;
                    need(selection.track==track_source_fixture::Track::ga&&selection.materialKey==materialKey&&
                        selection.pass==1&&selection.slot==1&&selection.passCount==1&&selection.program==pass.microprogram&&
                        selection.staticAnimationFrame&&selection.decodedSourceSha256==entry.sourceSha&&
                        selection.textureIndex!=a::unresolvedMaterialTexture50EA,
                        "GA ATM material association is not the authenticated original source");
                    if(found)need(out.sourceTextureIndex==selection.textureIndex&&out.clampU==selection.clampU&&out.clampV==selection.clampV,
                        "GA ATM shared source bank/sampler conflict");
                    out.sourceTextureIndex=selection.textureIndex;out.clampU=selection.clampU;out.clampV=selection.clampV;found=true;
                }
                need(found,"GA original animated source has no retained draw association");
                std::size_t end=12;
                for(std::uint32_t f=0;f<entry.frameCount;++f){
                    d.frame=f;const auto index=entry.firstTextureIndex+f;const auto& frame=detail::frames[index];
                    need(frame.offset==end&&frame.offset<=original.size()&&frame.bytes<=original.size()-frame.offset,
                        "GA original embedded frame extent/order");
                    std::vector<std::uint8_t> pst(original.begin()+frame.offset,original.begin()+frame.offset+frame.bytes);
                    need(shared::sha256(pst)==frame.sourceSha,"GA original embedded PST identity");
                    a::Texture48E decoded;
                    need(a::decodeTexture48E(pst,decoded,d.source)&&decoded.mips.size()==frame.mip+1&&
                        decoded.mips.back().width==frame.width&&decoded.mips.back().height==frame.height&&
                        shared::sha256(decoded.base_rrtex)==frame.nativeSha,"GA native stored animation frame decode/identity");
                    if(f==entry.initialFrame){
                        const auto* initial=bank->texture(out.sourceTextureIndex);
                        need(initial&&initial->texture.width==frame.width&&initial->texture.height==frame.height&&
                            initial->texture.rgba==decoded.mips.back().rgba,
                            "GA authored initial frame differs from retained original image");
                    }
                    native[index]=std::move(decoded.base_rrtex);images[index]={std::uint64_t(index)+1,&native[index]};end+=frame.bytes;
                }
                need(end==original.size(),"GA original ATM trailing bytes");
            }
            std::uint32_t placed=0;
            for(std::uint32_t si=0;si<spans->size();++si){
                const auto& span=spans->at(si);
                const auto& selection=selections->at(span.materialSelection);
                for(std::uint32_t i=0;i<sourceAnimationCount;++i){
                    if(selection.textureName!=entries[i].name)continue;
                    need(placed<animationCount&&span.sourcePass==1&&span.sourcePassCount==1&&span.projectionPass==0,
                        "GA unique placed animation source pass/extent");
                    for(std::uint32_t previous=0;previous<placed;++previous)
                        need(next->animations[previous].worldInstanceIndex!=span.worldInstanceIndex||
                            next->animations[previous].materialSelection!=span.materialSelection,
                            "GA animated material instance repeats a surface outside retained subset");
                    auto& animation=next->animations[placed];animation=definitions[i];
                    animation.sourceSpanIndex=si;animation.worldInstanceIndex=span.worldInstanceIndex;
                    animation.materialSelection=span.materialSelection;
                    next->state[placed]={float(animation.initialFrame),0,false};
                    next->selectedFrames[placed]=animation.initialFrame;++placed;
                }
            }
            need(placed==animationCount,"GA exact unique placed material animation count");
            // Images only: do not fabricate material TextureMap slots for frames.
            const a::MaterialTextureBudgets50EA budget{frameTextureCount,0,4u*1024u*1024u,4u*1024u*1024u,128};
            const a::MaterialTextureAssetsInput50EA input{images.data(),frameTextureCount,nullptr,0,budget};
            need(next->bank.load(input,d.bank),"GA cold original ATM frame bank rejected");
            storage_=std::move(next);d.animation=d.frame=a::unresolvedMaterialTexture50EA;return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
        catch(...){try{d.message="Unexpected original GA ATM preparation failure";}catch(...){}return false;}
    }
};
}
