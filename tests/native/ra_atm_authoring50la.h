#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include "../../src/game/original_ui_texture_animation50kg.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <utility>

namespace ra_atm_authoring50LA {
namespace a=rr::assets;namespace g=rr::game;
struct Source {
    std::vector<std::uint8_t> psmBytes,atmBytes;
    a::Material48E material;
    // Configuration evidence only; never a live animation instance or clock.
    g::OriginalUiTextureAnimationConfig50KG config;
    std::uint32_t pass=0,frameCount=0;
    bool clampU=false,clampV=false,animatesWhilePaused=false;
    // Existing indexed splitter can describe fholobeam. Crowd kind2 remains
    // raw authenticated bytes: no invented IPUM frame layout or decoded cache.
    a::OriginalUiAtm50KG indexedFrames;
};
class Owner final {
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::array<Source,2> sources_{};
    bool attempted_=false,decoded_=false;
    struct Failure {const char* why;};
    static void need(bool b,const char* why){if(!b)throw Failure{why};}
    static std::string field(const a::MaterialFields48E& fields,const char* key,const char* fallback){
        const std::string* result=nullptr;
        for(const auto& f:fields.authored)if(f.name==key){need(!result,"duplicate RA ATM field");result=&f.value;}
        return result?*result:fallback;
    }
public:
    struct Diagnostic {const char* failure=nullptr;std::uint32_t source=0;a::DecodeError48C decode{};};
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool decode(std::shared_ptr<const ra_cold_sources50LA::Owner> source,Diagnostic& d) noexcept {
        d={};if(attempted_){d.failure="RA ATM closure already attempted";return false;}attempted_=true;
        try {
            const auto* original=source?source->sourcesDecoded():nullptr;
            need(original&&original->world.zones.size()==10,"actual authenticated RA source owner required");
            source_=std::move(source);
            struct Pin {const char* psm;std::size_t psmBytes;const char* psmSha;const char* atm;
                std::size_t atmBytes;const char* atmSha;const char* texture;const char* program;
                std::uint32_t pass,passes,count,fps;};
            constexpr Pin pins[2]={
                {"textures\\crowd.psm",2738,"fec34de7468ae375ad3434b8c8113371773ea6100335027738ef25f1a03eb86f",
                 "textures\\crowd.atm",214578,"76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031",
                 "crowd","PrelitCompressed",1,1,6,10},
                {"textures\\finishline.psm",2833,"09ac1aeef799d7b0c884873cc457de96b879dcdfdbc9085267a6f7d8f4d895aa",
                 "textures\\fholobeam.atm",9676,"3013cb65a05f43857af0144e0d9f6e04b7e26173f94e363b5eacfb967b6ccb9f",
                 "fholobeam","Diffuse",2,2,16,16}
            };
            namespace bytes=pod_materials50ed_fixture::detail;
            for(std::uint32_t i=0;i<2;++i){
                d.source=i;auto& out=sources_[i];const auto& pin=pins[i];
                need(a::extractResMember48C(original->archive,pin.psm,out.psmBytes,d.decode,pin.psmBytes)&&
                     out.psmBytes.size()==pin.psmBytes&&bytes::sha256(out.psmBytes)==pin.psmSha,"RA exact animated PSM identity");
                need(a::extractResMember48C(original->archive,pin.atm,out.atmBytes,d.decode,pin.atmBytes)&&
                     out.atmBytes.size()==pin.atmBytes&&bytes::sha256(out.atmBytes)==pin.atmSha,"RA exact ATM identity");
                need(a::decodeMaterial48E(out.psmBytes,out.material,d.decode)&&out.material.passes.size()==pin.passes,
                     "RA actual declared material passes");
                const auto& pass=out.material.passes[pin.pass-1];
                need(pass.pass==pin.pass&&pass.microprogram==pin.program&&pass.textures.size()==1&&
                     pass.textures[0].slot==1&&pass.textures[0].name==pin.texture,"RA actual animated source pass");
                need(field(pass.fields,"TextureAnimation","none")=="unique"&&
                     field(pass.fields,"animatedMapInitialFrame","0")=="0"&&
                     field(pass.fields,"animatedMapFramesPerSecond","1")==std::to_string(pin.fps)&&
                     field(pass.fields,"animatedMapLooping","1")=="1"&&
                     field(pass.fields,"animatedMapReverseLoopOnEnd","0")=="0"&&
                     field(pass.fields,"animatedMapLoopOnEndToFrame","-1")=="0"&&
                     field(pass.fields,"animatesWhilePaused","0")=="0","RA exact unique animation configuration");
                const std::string wrap=i?"1":"0";
                need(field(pass.fields,"wrapModeHorizontal","0")==wrap&&
                     field(pass.fields,"wrapModeVertical","0")==wrap,"RA exact animation sampler authoring");
                need(out.atmBytes.size()>=12&&bytes::u32(out.atmBytes.data())==1&&
                     bytes::u32(out.atmBytes.data()+4)==pin.count&&bytes::u32(out.atmBytes.data()+8)==0,
                     "RA authentic ATM header count/storage");
                out.pass=pin.pass;out.frameCount=pin.count;out.clampU=out.clampV=i!=0;
                out.config.framesPerSecond=static_cast<float>(pin.fps);
                out.config.loopOnEndToFrame=0; //Other fields retain exact29B950 defaults.
                need(g::validOriginalUiTextureAnimationConfig50KG(out.config,out.frameCount),"RA bounded source configuration");
                if(i==1){
                    need(a::splitOriginalUiAtm50KG(out.atmBytes.data(),out.atmBytes.size(),out.indexedFrames,d.decode)&&
                         out.indexedFrames.frames.size()==16,"RA fholobeam original indexed frame extents");
                    for(const auto& frame:out.indexedFrames.frames)
                        need(frame.kind==0&&frame.mipCount==1&&frame.width==32&&frame.height==32,
                             "RA fholobeam original frame layout");
                }
            }
            decoded_=true;return true;
        }catch(const Failure& failure){d.failure=failure.why;return false;}
        catch(...){d.failure="RA ATM closure allocation failure";return false;}
    }
    //Borrow only within this stable owner lifetime. Failure publishes nothing;
    //retained partial raw records remain private until destruction, no retry.
    const std::array<Source,2>* sourceClosure()const noexcept{return decoded_?&sources_:nullptr;}
};
}
