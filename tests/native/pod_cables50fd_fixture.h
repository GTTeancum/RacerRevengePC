#pragma once
#include "pod_materials50ed_fixture.h"
#include "source_cable_texture50fc_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/game/pod_cables50fd.h"

// Cold development adapter for the authenticated Pod01 source data. This is
// not a packaged importer. The runtime generator never reads original files.
namespace pod_cables50fd_fixture {
namespace a=rr::assets;
namespace g=rr::game;
struct Diagnostic {
    std::string message;
    a::DecodeError48C decoder{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
    std::uint32_t width=0,height=0;
};
constexpr const char* sourceArchive="work/native/project/disc/PODS/POD01/POD01.RES";
constexpr std::uint64_t cableIdentity=0x50fd00000001ull;
namespace detail {
using pod_materials50ed_fixture::detail::need;
using pod_materials50ed_fixture::detail::sha256;
inline std::vector<std::uint8_t> read(const char* path) {
    std::ifstream f(runtime_asset_paths50LB::resolve(path),std::ios::binary|std::ios::ate);
    need(bool(f),"Cannot open original Pod01.RES for cable");
    const auto n=f.tellg();need(n>0&&static_cast<std::uint64_t>(n)<=1024u*1024u,"Pod01.RES cable input size");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(n));f.seekg(0);
    f.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    need(f.good(),"Pod01.RES cable read failed");return bytes;
}
inline g::SceneNodeIndex49P find(const a::Scene48D& scene,g::SceneNodeIndex49P root,
    const char* name) {
    g::SceneNodeIndex49P result=g::SceneNodeIndex49P::none;
    for(std::uint32_t i=0;i<scene.nodes.size();++i)if(scene.nodes[i].name==name) {
        need(result==g::SceneNodeIndex49P::none,"Duplicate authored cable attachment");
        result=static_cast<g::SceneNodeIndex49P>(static_cast<std::int32_t>(root)+1+i);
    }
    return result;
}
inline void map(const a::Scene48D& scene,g::SceneNodeIndex49P root,
    const g::VehicleFrameState49W& state) {
    const auto r=static_cast<std::int32_t>(root);
    need(r>=0&&static_cast<std::uint32_t>(r)+1+scene.nodes.size()<=g::vehicleFrameNodeCount49W,
        "Cable source scene map range");
    for(std::uint32_t i=0;i<scene.nodes.size();++i) {
        const auto p=scene.nodes[i].parent;
        need(p>=-1&&(p<0||static_cast<std::uint32_t>(p)<scene.nodes.size()),"Cable source parent range");
        const auto expected=static_cast<g::SceneNodeIndex49P>(p<0?r:r+1+p);
        need(state.nodes[r+1+i].parent180==expected,"Cable source/current scene mapping differs");
    }
}
}
// Independent cold source smoke is possible without a GL context or a game
// launch. Validates original archive and member hashes, material pass/slot and
// alpha policy, then uses the source-derived50FC kind0 decoder and unchanged50EA bank.
inline bool loadTexture(a::MaterialTextureAssets50EA& bank,Diagnostic& d,
    const char* archive=sourceArchive) noexcept {
    d={};
    try {
        using detail::need;using detail::sha256;
        const auto input=detail::read(archive);
        need(sha256(input)=="08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3",
            "Original Pod01.RES cable source hash mismatch");
        a::ResArchive48C res;
        need(a::decodeResource48C(input,res,d.decoder),"Pod01.RES cable decode failed");
        std::vector<std::uint8_t> psm,pst;
        need(a::extractResMember48C(res,"textures\\cable.psm",psm,d.decoder)&&
             a::extractResMember48C(res,"textures\\cable.pst",pst,d.decoder),"Original cable material/texture missing");
        need(sha256(psm)=="701039c97a0f4b74a65e90fdb1e386b40cbbb17bcedb2347196884234d0eee33"&&
             sha256(pst)=="e6b0abd320de50c784816a4e15dc6bfa056c8241a173eb26481243ff84dc301f",
             "Original cable material/texture hash mismatch");
        a::Material48E material;
        need(a::decodeMaterial48E(psm,material,d.decoder),"Original cable material decode failed");
        need(material.passes.size()==1&&material.passes[0].pass==1&&
             material.passes[0].microprogram=="StripOptFogCode"&&
             material.passes[0].textures.size()==1&&material.passes[0].textures[0].slot==1&&
             material.passes[0].textures[0].name=="cable","Original cable material selection differs");
        bool opaque=false,textured=false;
        for(const auto& field:material.passes[0].fields.authored) {
            if(field.name=="alphaBlendingOnOff")opaque=field.value=="0";
            if(field.name=="textureMappingOnOff")textured=field.value=="1";
        }
        need(opaque&&textured,"Original cable material opaque/textured policy differs");
        std::vector<std::uint8_t> texture;std::string textureError;
        need(source_cable_texture50fc_fixture::decodeCableTexture50FC(pst,texture,textureError),
            textureError.empty()?"Original cable kind0 PST decode failed":textureError);
        const a::MaterialTextureAssetInput50EA asset{cableIdentity,&texture};
        const a::MaterialTextureSlotInput50EA slot{{cableIdentity,0,1,1},cableIdentity,
            a::MaterialTextureRequirement50EA::required};
        const a::MaterialTextureAssetsInput50EA inputBank{&asset,1,&slot,1,
            {1,1,64u*1024u,64u*1024u,256}};
        need(bank.load(inputBank,d.bank),"Original cable native texture bank failed");
        d.width=bank.texture(0)->texture.width;d.height=bank.texture(0)->texture.height;
        return true;
    }catch(const std::exception& e){d.message=e.what();return false;}
    catch(...){d.message="Unexpected original cable cold-load failure";return false;}
}
class Owner final {
public:
    bool load(const a::VehicleDrawAssets50CR& assets,const g::VehicleFrameBindings49W& frame,
        const g::VehicleFrameState49W& state,Diagnostic& d) noexcept {
        d={};
        try {
            using detail::need;
            need(!bank_,"Cable owner already loaded");
            const auto* chariot=assets.scene(g::VehicleDrawComponent50CM::chariot);
            const auto* left=assets.scene(g::VehicleDrawComponent50CM::leftEngine);
            const auto* right=assets.scene(g::VehicleDrawComponent50CM::rightEngine);
            need(chariot&&left&&right,"Cable actual vehicle scenes unavailable");
            const auto& b=frame.lifecycle;
            detail::map(*chariot,b.model1B4,state);detail::map(*left,b.left1BC,state);detail::map(*right,b.right1B8,state);
            g::PodCableBindings50FD next{};
            const char* draw[2]={"drawcableleft1","drawcableright1"};
            const char* fallback[2]={"chariotcableleft1","chariotcableright1"};
            const g::SceneNodeIndex49P lifecycle[2]={b.leftCable1E0,b.rightCable1E4};
            for(unsigned side=0;side<2;++side) {
                const auto physics=detail::find(*chariot,b.model1B4,fallback[side]);
                need(physics==lifecycle[side]&&physics!=g::SceneNodeIndex49P::none,"Cable lifecycle/source attachment mismatch");
                next.chariot[side]=detail::find(*chariot,b.model1B4,draw[side]);
                if(next.chariot[side]==g::SceneNodeIndex49P::none)next.chariot[side]=physics;
            }
            next.engine[0]=detail::find(*left,b.left1BC,"enginecableleft1");
            next.engine[1]=detail::find(*right,b.right1B8,"enginecableright1");
            need(next.engine[0]!=g::SceneNodeIndex49P::none&&next.engine[1]!=g::SceneNodeIndex49P::none,
                "Cable named engine attachments unavailable");
            auto bank=std::make_unique<a::MaterialTextureAssets50EA>();
            if(!loadTexture(*bank,d))return false;
            bindings_=next;bank_=std::move(bank);return true;
        }catch(const std::exception& e){d.message=e.what();return false;}
        catch(...){d.message="Unexpected cable binding failure";return false;}
    }
    const a::MaterialTextureAssets50EA* bank() const noexcept{return bank_.get();}
    const g::PodCableBindings50FD& bindings() const noexcept{return bindings_;}
private:
    g::PodCableBindings50FD bindings_{};
    std::unique_ptr<a::MaterialTextureAssets50EA> bank_;
};
}
