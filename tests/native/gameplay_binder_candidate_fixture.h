#pragma once
// Source-only cold/runtime join candidate. No synthetic band or UI material reuse.
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include "../../src/assets/original_ui_material50kg.h"
#include "../../src/game/original_ui_texture_animation50kg.h"
#include "../../src/game/original_gameplay_binder_candidate.h"
#include <array>
#include <cmath>
#include <cstdlib>

namespace gameplay_binder_candidate_fixture {
namespace a=rr::assets;
namespace g=rr::game;
namespace core=g::original_gameplay_binder_candidate;
namespace shared=pod_materials50ed_fixture::detail;
struct Diagnostic {
    std::string message;
    a::DecodeError48C decode{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::array<std::array<core::Bindings,2>,8> bindings{};
        std::array<std::uint32_t,8> bindingCount{};
        std::array<bool,8> bound{};
        // Ordinary Binder150450 receives a9=1: draw23CA50 selects the
        // strip's authored color70, not LightManager+50 (the field40=0 path).
        std::array<std::array<float,4>,8> color70{};
        //213C20 ->298C80 and229B24 ->298E60 resolve the same canonical
        // textures\\binder material. 29B950 leaves named animation+18 clear;
        //29AAA0/29B1D0 therefore share its base controller across all quads.
        // One session material lifetime; no UI 'unique' per-object clone.
        g::OriginalUiTextureAnimation50KG animation;
        std::uint32_t frame=0;
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const core::Bindings* bindings(std::uint32_t slot,std::uint32_t pair=0)const noexcept {
        return storage_&&slot<8&&storage_->bound[slot]&&pair<storage_->bindingCount[slot]?
            &storage_->bindings[slot][pair]:nullptr;
    }
    const std::array<float,4>* color70(std::uint32_t slot)const noexcept {
        return storage_&&slot<8&&storage_->bound[slot]?&storage_->color70[slot]:nullptr;
    }
    std::uint32_t bindingCount(std::uint32_t slot)const noexcept {
        return storage_&&slot<8&&storage_->bound[slot]?storage_->bindingCount[slot]:0;
    }
    std::uint32_t selectedFrame()const noexcept{return storage_?storage_->frame:0xffffffffu;}
    // Invoke at24A758's material/render preparation with the SAME application
    // timestamp already published by the race/world.229B40 only refreshes
    // texture residency through249580; it does not advance this controller.
    //29AC90 repeat suppression stays in the native leaf. Caller owns pause.
    bool advance(std::uint32_t sourceMilliseconds)noexcept {
        if(!storage_)return false;
        return g::advanceOriginalUiTextureAnimation50KG(storage_->animation,sourceMilliseconds,storage_->frame);
    }
    void reset()noexcept {
        if(!storage_)return;
        auto config=storage_->animation.config;
        g::initializeOriginalUiTextureAnimation50KG(storage_->animation,config,10);
        storage_->frame=0;
    }
    // Every selected pod authenticates its OWN RES, PSM and ATM before its
    // attachment mapping is admitted. Identical immutable image payloads may
    // share the ten-frame bank. No fallback to Pod01 is allowed.
    bool bind(std::uint32_t slot,const native_racer50HW::Rig& rig,
        const g::VehicleFrameState49W& state,const g::VehicleFrameBindings49W& frame,
        const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,Diagnostic& d)noexcept {
        d={};try {
            using shared::need;
            need(slot<8&&(!storage_||!storage_->bound[slot]),"Binder duplicate/out-of-range racer slot");
            need(runtime.overrideKnown&&!runtime.override435670,"Binder requires an owned ordinary material runtime");
            const auto* defaults=templates.get(a::OriginalUiTemplate50KG::defaultMt);
            need(defaults&&defaults->identity&&std::string(defaults->identity->sha256)==
                "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7",
                "Binder authentic DefaultMT owner missing");
            need(g::validVehicleSceneCount49W(state,frame)&&state.sceneNodeCount==rig.nodeCount&&
                frame.lifecycle.left1BC==rig.roots[native_racer50HW::Rig::left]&&
                frame.lifecycle.right1B8==rig.roots[native_racer50HW::Rig::right],"Binder source/current rig differs");
            std::array<core::Bindings,2> binding;
            const std::uint32_t pairCount=rig.modelCount==5?2u:1u;
            if(pairCount==2){
                // Original213AE8..213B34 anchor ownership, then229DB4..229DE8 draw order.
                binding[0].left230=rig.find(native_racer50HW::Rig::bottomLeft,"binderleft1");
                binding[0].right238=rig.find(native_racer50HW::Rig::right,"binderright1");
                binding[0].pair=core::Pair::bottomLeftToPrimaryRight;
                binding[1].left230=rig.find(native_racer50HW::Rig::left,"binderleft2");
                binding[1].right238=rig.find(native_racer50HW::Rig::bottomRight,"binderright2");
                binding[1].pair=core::Pair::primaryLeftToBottomRight;
                need(frame.visual.bottomLeft1C4==rig.roots[native_racer50HW::Rig::bottomLeft]&&
                    frame.visual.bottomRight1C0==rig.roots[native_racer50HW::Rig::bottomRight],
                    "Binder optional source/current rig differs");
            }else{
                binding[0].left230=rig.find(native_racer50HW::Rig::left,"binderleft1");
                binding[0].right238=rig.find(native_racer50HW::Rig::right,"binderright1");
            }
            for(unsigned pair=0;pair<pairCount;++pair)
                need(binding[pair].left230!=native_racer50HW::Index::none&&
                    binding[pair].right238!=native_racer50HW::Index::none,"Original Binder named endpoint missing");
            const auto phy=native_racer50HW::rig_detail::ini(rig.phy);
            const auto section=phy.find("binder");
            //215BC8..215BFC: [Binder] BinderMeshHalfHeight through Config's
            // float read (158B80), default 2.25 (0x40100000) when absent. Every
            // pod takes the value its own PHY authors (no stock PHY authors it).
            float halfHeight25C=2.25f;
            if(section!=phy.end()){
                const auto authored=section->second.find("bindermeshhalfheight");
                if(authored!=section->second.end()){
                    std::string text=authored->second;
                    while(!text.empty()&&(text.back()==' '||text.back()=='\t'||text.back()=='\r'))text.pop_back();
                    std::size_t lead=0;while(lead<text.size()&&(text[lead]==' '||text[lead]=='\t'))++lead;
                    text.erase(0,lead);
                    char* end=nullptr;const float value=text.empty()?0.f:std::strtof(text.c_str(),&end);
                    need(!text.empty()&&end&&*end=='\0'&&std::isfinite(value),
                        "Binder BinderMeshHalfHeight requires one finite decimal");
                    halfHeight25C=value;
                }
            }
            //215C08..215C98 reads this selected PHY's Binder/VertexColor,
            // default00FFFFFF. 213A40/150450 sets field40=1, then23D750
            // publishes each integer RGB channel >>1 and alpha64 at color70.
            //23CA50 uses that row when field40 is nonzero, not scene ambient.
            std::array<std::uint32_t,3> rgb{{255u,255u,255u}};
            if(section!=phy.end()){
                const auto authored=section->second.find("vertexcolor");
                if(authored!=section->second.end()){
                    const auto& text=authored->second;std::size_t pos=0;
                    for(unsigned channel=0;channel<3;++channel){
                        while(pos<text.size()&&(text[pos]==' '||text[pos]=='\t'))++pos;
                        need(pos<text.size()&&text[pos]>='0'&&text[pos]<='9',"Binder VertexColor requires three decimal bytes");
                        unsigned value=0;
                        while(pos<text.size()&&text[pos]>='0'&&text[pos]<='9'){
                            value=value*10u+unsigned(text[pos++]-'0');
                            need(value<=255u,"Binder VertexColor channel exceeds byte domain");
                        }
                        rgb[channel]=value;
                        while(pos<text.size()&&(text[pos]==' '||text[pos]=='\t'||text[pos]=='\r'))++pos;
                        if(channel<2)need(pos<text.size()&&text[pos++]==',',"Binder VertexColor comma missing");
                        else need(pos==text.size(),"Binder VertexColor suffix unsupported");
                    }
                }
            }
            const std::array<float,4> color70{{float(rgb[0]>>1),float(rgb[1]>>1),float(rgb[2]>>1),64.f}};
            for(auto& pair:binding)pair.halfHeight25C=halfHeight25C;
            const auto* identity=original_player_identity50IK::find(rig.podId);
            need(identity,"Unadmitted original pod for Binder");
            const auto bytes=native_racer50HW::f::readBytes(rig.resourceFile);
            need(bytes.size()==identity->archiveBytes&&shared::sha256(bytes)==identity->archiveSha256,
                "Binder original POD archive identity differs");
            a::ResArchive48C archive;
            need(a::decodeResource48C(bytes,archive,d.decode),"Binder original POD RES decode");
            std::vector<std::uint8_t> psm,atm;
            need(a::extractResMember48C(archive,"textures\\binder.psm",psm,d.decode)&&
                a::extractResMember48C(archive,"textures\\binder.atm",atm,d.decode),"Binder original material/ATM missing");
            need(psm.size()==2791&&shared::sha256(psm)=="a9e37280356a21ce899735a4a367f9d820861b05900b9e185f9c5b1e0c2a3f4f"&&
                atm.size()==51492&&shared::sha256(atm)=="7dc77ff532963c80df154c9d3d401004d472e431ceb92bab7b125968ed8740d4",
                "Binder actual gameplay PSM/ATM identity differs");
            // Hash-closed one-pass profile: Diffuse; A0/B1/C0/D1, TFX0/TCC1;
            // alpha test GT0 (DefaultMT), depth LEQUAL, no depth write,
            // inherited fog ON, repeat/linear sampling, named16fps animation.
            if(!storage_){
                auto next=std::make_unique<Storage>();a::OriginalUiAtm50KG ranges;
                need(a::splitOriginalUiAtm50KG(atm.data(),atm.size(),ranges,d.decode)&&ranges.frames.size()==10,
                    "Binder original ATM frame table");
                std::array<std::vector<std::uint8_t>,10> encoded;
                std::array<a::MaterialTextureAssetInput50EA,10> images;
                for(unsigned i=0;i<10;++i){
                    const auto& range=ranges.frames[i];
                    need(range.pstOffset==12u+i*5148u&&range.pstBytes==5148&&range.kind==1&&
                        range.mipCount==1&&range.width==64&&range.height==64,"Binder stored PST range differs");
                    std::vector<std::uint8_t> pst(atm.begin()+range.pstOffset,atm.begin()+range.pstOffset+range.pstBytes);
                    a::Texture48E decoded;need(a::decodeTexture48E(pst,decoded,d.decode),"Binder genuine PST decode");
                    encoded[i]=std::move(decoded.base_rrtex);images[i]={0xb1d00000ull+i,&encoded[i]};
                }
                const a::MaterialTextureAssetsInput50EA input{images.data(),10,nullptr,0,{10,0,256u*1024u,256u*1024u,64}};
                need(next->bank.load(input,d.bank),"Binder immutable ten-frame bank");
                g::OriginalUiTextureAnimationConfig50KG config;config.framesPerSecond=16.f;config.loopOnEndToFrame=0;
                need(g::initializeOriginalUiTextureAnimation50KG(next->animation,config,10),"Binder original animation defaults");
                next->color70[slot]=color70;next->bindings[slot]=binding;next->bindingCount[slot]=pairCount;next->bound[slot]=true;storage_=std::move(next);
            }else{storage_->color70[slot]=color70;storage_->bindings[slot]=binding;storage_->bindingCount[slot]=pairCount;storage_->bound[slot]=true;}
            return true;
        }catch(const std::exception& e){d.message=e.what();return false;}
        catch(...){d.message="Binder cold owner failure";return false;}
    }
};
}
