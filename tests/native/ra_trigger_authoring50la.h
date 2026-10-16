#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/trigger_actions50in.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <utility>

namespace ra_trigger_authoring50LA {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=2,tobBytes=95558;
inline constexpr const char* tobSha="7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524";
struct Source {
    std::string name;
    std::uint32_t zone=0;
    g::CollisionSphere48J sphere{};
    g::LostAiAction50IN action{};
    bool startEnabled=false,oneTimeOnly=false,ignorePlayer=false;
    std::int32_t randomTriggerPercent90=0;
};
// Authenticated authoring only. No Entry event, collider identity, PLAYER
// resolution, action destination, zone membership or query policy is invented.
class Owner final {
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::vector<std::uint8_t> tob_;
    a::ParameterDocument48F parameters_;
    std::array<Source,count> sources_{};
    bool attempted_=false,decoded_=false;
    static void need(bool b,const char* m){if(!b)throw std::runtime_error(m);}
    static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
        const a::ParameterBlock48F* found=nullptr;
        for(const auto& b:p.blocks)if(b.name==name){need(!found,"duplicate RA block");found=&b;}
        need(found,"missing RA block");return *found;
    }
    static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
        const std::string* found=nullptr;
        for(const auto& f:b.fields)if(f.name==name){need(!found,"duplicate RA field");found=&f.value;}
        return found;
    }
    static const std::string& field(const a::ParameterBlock48F& b,const char* name){
        const auto* v=optional(b,name);need(v,"missing RA field");return *v;
    }
    static float number(const std::string& text){
        float value=0;need(a::parseTrackNumber50AD(text.data(),text.size(),value),"RA original decimal rejected");return value;
    }
    static g::Vector3_48C vector(const std::string& text){
        g::Vector3_48C v{};std::size_t begin=0;
        for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
            need((end==std::string::npos)==(i==2),"RA original vector extent");
            v.v[i]=number(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
        }return v;
    }
public:
    struct Diagnostic {const char* failure=nullptr;a::DecodeError48C decode;};
    Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    bool decode(std::shared_ptr<const ra_cold_sources50LA::Owner> source,Diagnostic& d)noexcept {
        d={};if(attempted_){d.failure="RA trigger decode already attempted";return false;}attempted_=true;
        try {
            const auto* s=source?source->sourcesDecoded():nullptr;
            need(s&&s->world.zones.size()==10&&s->ai.track.count==113&&s->ai.indicesPrepared,
                "actual RA source/navigation owner required");
            source_=std::move(source);
            need(a::extractResMember48C(s->archive,"tracks\\ra.tob",tob_,d.decode,tobBytes),"RA original TOB absent");
            need(tob_.size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(tob_)==tobSha,"RA original TOB identity");
            need(a::decodeParameters48F(tob_,parameters_,d.decode),"RA original TOB syntax");
            need(field(block(parameters_,"triggers"),"numberoftriggers")=="2","RA original trigger count");
            constexpr const char* names[count]={"rightSkull","leftskull"};
            constexpr const char* positions[count]={"8040.589844,2228.016113,8484.408203","7809.741699,2201.172119,8290.046875"};
            constexpr const char* radii[count]={"170.587753","169.766968"};
            constexpr const char* targetText[count]={"29","104"};
            constexpr std::uint32_t targets[count]={29,104};
            std::array<Source,count> next{};
            for(std::size_t i=0;i<count;++i){
                const auto label="trigger"+std::to_string(i+1);const auto& b=block(parameters_,label);
                const auto& action=block(parameters_,label+"action1");auto& out=next[i];
                need(field(b,"name")==names[i]&&field(b,"position")==positions[i]&&field(b,"radius")==radii[i]&&
                    field(b,"zone")=="4"&&field(b,"numberofactions")=="1","RA original sphere authoring");
                need(field(b,"startenabled")=="TRUE"&&field(b,"onetimeonly")=="FALSE"&&
                    field(b,"randomtriggerpercent")=="100.000000"&&field(b,"ignoretrigger")=="PLAYER",
                    "RA original activation/filter authoring");
                need(!optional(b,"attachtosoultree")&&!optional(b,"size")&&!optional(b,"extents"),"RA unsupported shape association");
                need(field(action,"actiontype")=="LostAI"&&field(action,"targetnodeindex")==targetText[i]&&
                    targets[i]<s->ai.track.count&&!optional(action,"reset"),"RA original LostAI authoring");
                out.name=names[i];out.zone=4;out.sphere.center=vector(positions[i]);out.sphere.radius=number(radii[i]);
                need(out.sphere.radius>0.f,"RA positive sphere radius required");
                out.action={targets[i],false}; // original1B5A60 omitted Reset default
                out.startEnabled=true;out.oneTimeOnly=false;out.ignorePlayer=true;out.randomTriggerPercent90=100;
            }
            sources_=std::move(next);decoded_=true;return true;
        }catch(...){d.failure="RA trigger authoring rejected; private prefix retained";return false;}
    }
    const std::array<Source,count>* sources()const noexcept{return decoded_?&sources_:nullptr;}
};
} // namespace ra_trigger_authoring50LA
