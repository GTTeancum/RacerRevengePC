#pragma once
#include "track_materials50fa_fixture.h"
#include "mb_original_world50lm.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/game/authored_texture_animation.h"

// Exact selected-track ATM resource/PSM/frame closure. Source hashes and
// genuine retained RRTEX hashes are independent. Native cold decoding only;
// no generated frame files, subprocesses, or hot-path image decoding.
namespace mb_atm_animation_fixture {
namespace a=rr::assets;
constexpr std::uint32_t maxFrames=9u*64u, maxAnimations=4096u;
constexpr std::size_t maxRgbaBytes=64u*1024u*1024u;
struct Diagnostic {
    std::uint32_t animation=a::unresolvedMaterialTexture50EA, frame=a::unresolvedMaterialTexture50EA;
    std::string message;
    a::DecodeError48C source{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
struct Animation {
    std::uint32_t sourceSpanIndex=0,worldInstanceIndex=0,materialSelection=0;
    std::uint32_t sourceTextureIndex=a::unresolvedMaterialTexture50EA;
    std::uint32_t firstTextureIndex=0,frameCount=0,initialFrame=0;
    float framesPerSecond=0.f;
    bool clampU=false,clampV=false;
};
struct Sampling { bool clampU=false,clampV=false; };
namespace detail {
namespace shared=pod_materials50ed_fixture::detail;
// Generated metadata uses '/', while retained RES names can use '\\'.
// Resolve both spellings symmetrically, reject aliases, then extract using
// the exact retained name. Archive/member byte and hash checks stay intact.
inline const std::string& archiveMember(const a::ResArchive48C& archive,const char* requested) {
    const auto key=shared::normalized(requested);const std::string* match=nullptr;
    for(const auto& entry:archive.entries)if(shared::normalized(entry.name)==key){
        shared::need(!match,"Ambiguous normalized ATM archive member");match=&entry.name;
    }
    shared::need(match,"Original ATM archive member absent");return *match;
}
struct Source {
    const char* member; const char* sha;
    std::uint32_t bytes,frameCount,firstTextureIndex;
};
struct Entry {
    const char* materialName; const char* textureName; const char* materialMember; const char* materialSha;
    std::uint32_t materialBytes,sourceIndex,pass,initialFrame;
    float fps;
    std::uint32_t placedSpans;
};
struct Frame {
    std::uint32_t offset,bytes,width,height,mip;
    const char* sourceSha; const char* nativeSha; const char* filename;
};
// Cold source-only conversion. Call only after the caller authenticates the
// full archive, ATM and exact embedded PST slice. No path or fallback exists.
inline std::vector<std::uint8_t> decodeOriginalFrame(const std::vector<std::uint8_t>& pst,
    const Frame& frame,a::DecodeError48C& error) {
    using shared::need;
    const auto range=[&](std::size_t at,std::size_t n){return at<=pst.size()&&n<=pst.size()-at;};
    const auto word=[&](std::size_t at){need(range(at,4),"Truncated original MB ATM PST");return shared::u32(pst.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    need(pst.size()<=8u*1024u*1024u&&word(0)==1&&word(8)==frame.mip+1,
        "Original MB ATM PST header/mip identity");
    std::vector<std::uint8_t> output;
    if(word(4)==2){
        // Kind2 is not supported by decodeTexture48E. Validate the complete
        // ascending PST envelope, then decode only the pinned largest image.
        const auto count=word(8);need(count>=1&&count<=7,"Original ATM IPUM mip count");
        std::size_t at=12,payload=0,payloadBytes=0;std::uint32_t priorW=0,priorH=0;
        for(std::uint32_t i=0;i<count;++i){
            need(range(at,20),"Original ATM IPUM mip header extent");
            const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
            need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
                (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),
                "Original ATM IPUM mip extent/order");
            need(!std::memcmp(pst.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
                (std::uint32_t(pst[p+8])|(std::uint32_t(pst[p+9])<<8))==w&&
                (std::uint32_t(pst[p+10])|(std::uint32_t(pst[p+11])<<8))==h&&
                word(p+12)==1&&pst[p+16]==0,"Original ATM IPUM embedded header");
            constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
            need(!std::memcmp(pst.data()+p+bytes-8,end,8),"Original ATM IPUM terminators");
            payload=p;payloadBytes=bytes;at=p+bytes;priorW=w;priorH=h;
        }
        need(at==pst.size()&&priorW==frame.width&&priorH==frame.height,
            "Original ATM IPUM full extent/selected dimensions");
        a::TextureMip48E mip;mip.width=frame.width;mip.height=frame.height;
        std::string why;
        const bool decoded=a::decodeOriginalIpuRgba(pst.data()+payload,payloadBytes,frame.width,frame.height,mip.rgba,why);
        need(decoded,"Original MB ATM native IPUM decode: "+why);
        need(a::encodeTextureLevel48E(mip,output,error),"Original ATM IPUM output framing");
    }else{
        need(word(4)==0||word(4)==1,"Unsupported original ATM PST kind");
        a::Texture48E decoded;
        need(a::decodeTexture48E(pst,decoded,error)&&decoded.mips.size()==frame.mip+1&&
            decoded.mips.back().width==frame.width&&decoded.mips.back().height==frame.height,
            "Original MB ATM indexed frame decode/identity");
        output=std::move(decoded.base_rrtex);
    }
    const auto size=std::size_t(frame.width)*frame.height*4+16;
    need(output.size()==size&&shared::sha256(output)==frame.nativeSha&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==frame.width&&
        shared::u32(output.data()+12)==frame.height,"Original ATM decoded frame identity/framing");
    return output;
}
struct Profile {
    const Source* sources; std::uint32_t sourceCount;
    const Entry* entries; std::uint32_t entryCount;
    const Frame* frames; std::uint32_t frameCount;
};
inline constexpr Source mbSources[]={
#include "mb_atm_source_index.inc"
};
inline constexpr Entry mbEntries[]={
#include "mb_atm_animation_index.inc"
};
inline constexpr Frame mbFrames[]={
#include "mb_atm_frame_index.inc"
};
inline Profile profile(track_source_fixture::Track track) {
    shared::need(track==track_source_fixture::Track::mb,"Exact MB ATM role required");
    return {mbSources,5,mbEntries,5,mbFrames,40};
}
struct ArchiveProfile {std::size_t archiveBytes;const char* archiveSha;std::size_t archiveDecodedBytes,archiveEntries;};
inline constexpr ArchiveProfile mbArchive{mb_original_world50LM::archiveBytes,mb_original_world50LM::archiveSha,mb_original_world50LM::decodedBytes,mb_original_world50LM::entryCount};
inline std::string field(const a::MaterialFields48E& fields,const char* name,const char* fallback) {
    std::string value=fallback;bool found=false;
    for(const auto& f:fields.authored)if(f.name==name){shared::need(!found,"Repeated ATM material field");value=f.value;found=true;}
    return value;
}
inline float number(const std::string& text) {
    std::istringstream in(text);in.imbue(std::locale::classic());float value=0;
    in>>value;shared::need(bool(in)&&std::isfinite(value),"Original ATM finite scalar");
    in>>std::ws;shared::need(in.eof(),"Original ATM scalar suffix");return value;
}
}
class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::vector<Animation> animations;
        std::vector<rr::game::AuthoredTextureAnimation> state,scratchState;
        std::vector<std::uint32_t> selectedFrames,scratchFrames;
        std::vector<Sampling> sampling;
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::vector<Animation>* animations()const noexcept{return storage_?&storage_->animations:nullptr;}
    const std::vector<std::uint32_t>* selectedFrames()const noexcept{return storage_?&storage_->selectedFrames:nullptr;}
    const std::vector<Sampling>* sampling()const noexcept{return storage_?&storage_->sampling:nullptr;}
    // Common accepted-world timestamp only. No private clock, pause correction,
    // allocation or file access occurs on this hot path. Failure is transactional.
    bool advance(std::uint32_t sourceApplicationTimestamp)noexcept {
        if(!storage_)return false;
        for(std::size_t i=0;i<storage_->animations.size();++i){
            storage_->scratchState[i]=storage_->state[i];storage_->scratchFrames[i]=storage_->selectedFrames[i];
            const auto& animation=storage_->animations[i];
            if(!rr::game::advanceAuthoredTextureAnimation(storage_->scratchState[i],sourceApplicationTimestamp,
                animation.framesPerSecond,animation.frameCount,storage_->scratchFrames[i]))return false;
        }
        for(std::size_t i=0;i<storage_->animations.size();++i){
            storage_->state[i]=storage_->scratchState[i];storage_->selectedFrames[i]=storage_->scratchFrames[i];
        }
        return true;
    }
    void reset()noexcept {
        if(!storage_)return;
        for(std::size_t i=0;i<storage_->animations.size();++i){
            storage_->state[i]={float(storage_->animations[i].initialFrame),0,false};
            storage_->selectedFrames[i]=storage_->animations[i].initialFrame;
        }
    }
    bool load(const std::filesystem::path& sourceArchive,
        track_source_fixture::Track selectedTrack,const track_materials50fa_fixture::Owner& track,
        Diagnostic& d)noexcept {
        return load<track_materials50fa_fixture::Owner>(sourceArchive,selectedTrack,track,d);
    }
    // Narrow read-only static-owner contract. Both generated-backed and
    // direct-original owners publish these exact record/bank types. Borrow
    // only during this cold load; preserve every submitted source index.
    template<class StaticMaterialOwner>
    bool load(const std::filesystem::path& sourceArchive,
        track_source_fixture::Track selectedTrack,const StaticMaterialOwner& track,
        Diagnostic& d)noexcept {
        using namespace detail;using shared::need;
        try{
            d={};need(!storage_&&track.loaded(),"MB ATM requires loaded static source materials");
            const a::MaterialTextureAssets50EA* bank=track.bank();
            const std::vector<track_materials50fa_fixture::Selection>* selections=track.selections();
            const std::vector<track_materials50fa_fixture::Span>* spans=track.spans();
            need(bank&&selections&&spans,"MB ATM static source bank/selections/spans unavailable");
            need(!selections->empty()&&selections->size()<=1000000&&
                !spans->empty()&&spans->size()<=1000000,"MB ATM static source record capacity");
            const auto p=profile(selectedTrack);const auto& archiveProfile=mbArchive;
            need(p.frameCount&&p.frameCount<=maxFrames&&p.entryCount&&p.entryCount<=64,"Bounded MB ATM metadata");
            // Preserve bounded accumulated output before any image allocation.
            std::size_t rgbaBytes=0;
            for(std::uint32_t i=0;i<p.frameCount;++i){const auto& frame=p.frames[i];
                need(frame.width>=16&&frame.width<=1024&&frame.height>=16&&frame.height<=1024&&frame.mip<=6,
                    "Original ATM pinned dimensions/mip budget");
                shared::add(rgbaBytes,std::size_t(frame.width)*frame.height*4,maxRgbaBytes,
                    "Original ATM decoded RGBA budget");
            }
            shared::Inputs inputs;inputs.capacity=maxRgbaBytes+8u*1024u*1024u;
            auto bytes=inputs.read(sourceArchive,archiveProfile.archiveBytes);
            need(bytes.size()==archiveProfile.archiveBytes&&shared::sha256(bytes)==archiveProfile.archiveSha,"Original ATM archive identity");
            a::ResArchive48C archive;a::DecodeLimits48C limits;
            limits.resource_file_bytes=archiveProfile.archiveBytes;limits.resource_decoded_bytes=archiveProfile.archiveDecodedBytes;
            limits.resource_entries=archiveProfile.archiveEntries;
            need(a::decodeResource48C(bytes,archive,d.source,nullptr,limits),"Original ATM archive decode");
            auto next=std::make_unique<Storage>();next->sampling.resize(p.frameCount);
            std::vector<std::vector<std::uint8_t>> native(p.frameCount);
            std::vector<a::MaterialTextureAssetInput50EA> images(p.frameCount);
            std::uint32_t nextFrame=0;
            for(std::uint32_t si=0;si<p.sourceCount;++si){
                d.animation=si;const auto& source=p.sources[si];
                need(source.firstTextureIndex==nextFrame&&source.frameCount&&source.frameCount<=64&&
                    source.frameCount<=p.frameCount-nextFrame,"Original ATM contiguous bank extent");
                std::vector<std::uint8_t> original;
                need(a::extractResMember48C(archive,archiveMember(archive,source.member),original,d.source,1024u*1024u)&&
                    original.size()==source.bytes&&shared::sha256(original)==source.sha,"Exact original ATM source identity");
                need(original.size()>=12&&shared::u32(original.data())==1&&
                    shared::u32(original.data()+4)==source.frameCount&&shared::u32(original.data()+8)==0,"Original ATM header");
                std::size_t end=12;
                for(std::uint32_t f=0;f<source.frameCount;++f){
                    d.frame=f;const auto index=nextFrame+f;const auto& frame=p.frames[index];
                    need(frame.offset==end&&frame.offset<=original.size()&&frame.bytes<=original.size()-frame.offset&&
                        frame.width>=16&&frame.height>=16&&frame.width<=1024&&frame.height<=1024&&frame.mip<=6,
                        "Original ATM frame extent/dimensions");
                    std::vector<std::uint8_t> pst(original.begin()+frame.offset,original.begin()+frame.offset+frame.bytes);
                    need(shared::sha256(pst)==frame.sourceSha,"Original embedded PST identity");
                    native[index]=decodeOriginalFrame(pst,frame,d.source);
                    images[index]={std::uint64_t(index)+1,&native[index]};end+=frame.bytes;
                }
                need(end==original.size(),"Original ATM complete frame coverage");nextFrame+=source.frameCount;
            }
            need(nextFrame==p.frameCount,"Original ATM complete bank coverage");
            std::vector<Animation> definitions(p.entryCount);
            std::vector<bool> samplerSet(p.frameCount,false);
            std::vector<std::uint32_t> selectionEntry(selections->size(),a::unresolvedMaterialTexture50EA),placed(p.entryCount,0);
            for(std::uint32_t i=0;i<p.entryCount;++i){
                d.animation=i;const auto& entry=p.entries[i];need(entry.sourceIndex<p.sourceCount,"ATM binding source index");
                const auto& source=p.sources[entry.sourceIndex];auto& def=definitions[i];
                need(entry.pass>=1&&entry.pass<=8&&entry.initialFrame<source.frameCount&&entry.fps>0&&entry.fps<=60&&
                    entry.placedSpans<=maxAnimations,"Supported original ATM source binding subset");
                std::vector<std::uint8_t> psm;
                need(a::extractResMember48C(archive,archiveMember(archive,entry.materialMember),psm,d.source,16384)&&
                    psm.size()==entry.materialBytes&&shared::sha256(psm)==entry.materialSha,"Exact original animated PSM identity");
                a::Material48E material;
                need(a::decodeMaterial48E(psm,material,d.source)&&entry.pass<=material.passes.size(),
                    "Original animated material declared pass decode");
                // Retention authenticates the declared pass in the complete PSM.
                // Original finishline has no static placements and names its
                // genuine fholobeam source and clamp policy in ShaderPass_2.
                const auto& pass=material.passes[entry.pass-1];
                need(pass.pass==entry.pass&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                    shared::normalized(pass.textures[0].name)==shared::normalized(entry.textureName)&&
                    shared::normalized(std::string("textures/")+entry.textureName+".atm")==shared::normalized(source.member)&&
                    field(pass.fields,"TextureAnimation","none")=="unique"&&
                    number(field(pass.fields,"animatedMapInitialFrame","0"))==float(entry.initialFrame)&&
                    number(field(pass.fields,"animatedMapFramesPerSecond","1"))==entry.fps&&
                    field(pass.fields,"animatedMapLooping","1")=="1"&&
                    field(pass.fields,"animatedMapReverseLoopOnEnd","0")=="0"&&
                    field(pass.fields,"animatedMapLoopOnEndToFrame","-1")=="0","Original ATM forward-loop scope/rate/start");
                const auto wrapU=field(pass.fields,"wrapModeHorizontal","0"),wrapV=field(pass.fields,"wrapModeVertical","0");
                need((wrapU=="0"||wrapU=="1")&&(wrapV=="0"||wrapV=="1"),"Original ATM wrap policy");
                def.firstTextureIndex=source.firstTextureIndex;def.frameCount=source.frameCount;
                def.initialFrame=entry.initialFrame;def.framesPerSecond=entry.fps;def.clampU=wrapU=="1";def.clampV=wrapV=="1";
                for(std::uint32_t f=0;f<source.frameCount;++f){
                    const auto index=source.firstTextureIndex+f;auto& sampler=next->sampling[index];
                    need(!samplerSet[index]||(sampler.clampU==def.clampU&&sampler.clampV==def.clampV),"Shared ATM sampler conflict");
                    sampler={def.clampU,def.clampV};samplerSet[index]=true;
                }
                // Live static animation remains the recovered one-pass subset;
                // retaining an unused layered source never admits its draw semantics.
                need(!entry.placedSpans||(entry.pass==1&&material.passes.size()==1),
                    "Placed ATM requires the original single-pass live subset");
                const auto key=shared::sha256(std::string("rr-native-material-48E-v1\n")+entry.materialSha);
                for(std::uint32_t mi=0;mi<selections->size();++mi){
                    const auto& selection=selections->at(mi);
                    if(shared::normalized(selection.materialName)!=shared::normalized(entry.materialName)||selection.pass!=entry.pass)continue;
                    need(entry.placedSpans&&entry.pass==1&&material.passes.size()==1,
                        "Unplaced or layered ATM source cannot acquire a live static attachment");
                    need(selectionEntry[mi]==a::unresolvedMaterialTexture50EA&&selection.track==selectedTrack&&selection.materialKey==key&&
                        selection.slot==1&&selection.passCount==1&&selection.program==pass.microprogram&&selection.staticAnimationFrame&&
                        !selection.projectedPrelit&&shared::normalized(selection.textureName)==shared::normalized(entry.textureName)&&
                        shared::normalized(selection.decodedSourceMember)==shared::normalized(source.member)&&
                        selection.decodedSourceSha256==source.sha&&selection.textureIndex!=a::unresolvedMaterialTexture50EA&&
                        selection.clampU==def.clampU&&selection.clampV==def.clampV,"Original ATM static material association");
                    selectionEntry[mi]=i;
                    const auto* initial=bank->texture(selection.textureIndex);const auto& retained=native[source.firstTextureIndex+entry.initialFrame];
                    need(initial&&initial->texture.width==p.frames[source.firstTextureIndex+entry.initialFrame].width&&
                        initial->texture.height==p.frames[source.firstTextureIndex+entry.initialFrame].height&&
                        initial->texture.rgba.size()==retained.size()-16&&
                        std::equal(initial->texture.rgba.begin(),initial->texture.rgba.end(),retained.begin()+16),"Original ATM initial image association");
                }
            }
            for(bool assigned:samplerSet)need(assigned,"Original ATM source lacks material sampler ownership");
            for(std::uint32_t si=0;si<spans->size();++si){
                const auto& span=spans->at(si);const auto& selection=selections->at(span.materialSelection);
                const auto entry=selectionEntry.at(span.materialSelection);
                if(entry==a::unresolvedMaterialTexture50EA){need(!selection.staticAnimationFrame,"Unowned original static ATM span");continue;}
                need(next->animations.size()<maxAnimations&&span.sourcePass==1&&span.sourcePassCount==1&&span.projectionPass==0&&
                    span.textureIndex==selection.textureIndex,"Original ATM placed span bound/pass/texture");
                auto animation=definitions[entry];animation.sourceSpanIndex=si;animation.worldInstanceIndex=span.worldInstanceIndex;
                animation.materialSelection=span.materialSelection;animation.sourceTextureIndex=selection.textureIndex;
                next->animations.push_back(animation);++placed[entry];
            }
            for(std::uint32_t i=0;i<p.entryCount;++i)need(placed[i]==p.entries[i].placedSpans,"Exact original ATM placed-span count");
            need(!next->animations.empty(),"Original track contains no placed ATM animation");
            next->state.resize(next->animations.size());next->scratchState.resize(next->animations.size());
            next->selectedFrames.resize(next->animations.size());next->scratchFrames.resize(next->animations.size());
            for(std::size_t i=0;i<next->animations.size();++i){
                next->state[i]={float(next->animations[i].initialFrame),0,false};next->selectedFrames[i]=next->animations[i].initialFrame;
            }
            const a::MaterialTextureBudgets50EA budget{p.frameCount,0,maxRgbaBytes+std::size_t(p.frameCount)*16,maxRgbaBytes,1024};
            const a::MaterialTextureAssetsInput50EA input{images.data(),p.frameCount,nullptr,0,budget};
            need(next->bank.load(input,d.bank),"Original native-decoded ATM frame bank rejected");
            storage_=std::move(next);d.animation=d.frame=a::unresolvedMaterialTexture50EA;return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
        catch(...){try{d.message="Unexpected original MB ATM preparation failure";}catch(...){}return false;}
    }
};
}
