#include "material_audio50hi.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <stdexcept>
namespace rr::assets { namespace {
void need(bool x,const char*m){if(!x)throw std::runtime_error(m);}
const ParameterBlock48F& block(const ParameterDocument48F&p,const std::string&name){for(const auto&b:p.blocks)if(b.name==name)return b;throw std::runtime_error("50HI missing AUB block");}
const std::string& field(const ParameterBlock48F&b,const char*name){for(const auto&f:b.fields)if(f.name==name)return f.value;throw std::runtime_error("50HI missing AUB field");}
int number(const std::string&s){int v=0;const auto r=std::from_chars(s.data(),s.data()+s.size(),v);need(r.ec==std::errc{}&&r.ptr==s.data()+s.size(),"50HI invalid integer authoring");return v;}
std::string filename(const ParameterBlock48F&b){auto n=field(b,"soundresourcefile");need(!n.empty()&&n.size()<200&&n.find('.')==std::string::npos,"50HI invalid sound resource name");for(auto&c:n){need((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='\\'||c=='/',"50HI invalid sound resource path");if(c=='\\')c='/';}need(n.rfind("sfx/race/",0)==0&&n.find("//")==std::string::npos,"50HI unexpected resource family");return n+".vag";}
}
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>&aub,MaterialAudioCatalog50HI&out,std::vector<AudioRequest50HC>&requests,std::string&error){
    try{ParameterDocument48F p;DecodeError48C d;need(decodeParameters48F(aub,p,d),"50HI AUB parameter decode failed");
        const auto&s=block(p,"sounds");need(number(field(s,"nummaterialcollisionsounds"))==8&&number(field(s,"nummaterialdriveonsounds"))==8&&number(field(s,"nummaterials"))==1,"50HI unsupported material row count");
        MaterialAudioCatalog50HI next;std::vector<AudioRequest50HC> unique;
        auto add=[&](const std::string&name,bool loop){for(auto&r:unique)if(r.name==name){need(r.loop==loop,"50HI conflicting duplicate clip flags");return;}unique.push_back({name,loop,false,false});};
        for(unsigned i=0;i<8;++i){const auto&c=block(p,"materialcollisionsound"+std::to_string(i+1));const auto&b=block(p,"materialdriveonsound"+std::to_string(i+1));next.collision[i]=filename(c);next.drive[i]=filename(b);add(next.collision[i],false);add(next.drive[i],true);
            auto&v=next.parameters.drive[i];v.sampleSpeed=number(field(b,"materialsamplespeed"));v.minVolume=number(field(b,"materialminvolume"));v.minSpeed=number(field(b,"materialminspeed"));v.maxSpeed=number(field(b,"materialmaxspeed"));v.resonantSpeed=number(field(b,"materialresonantfreqspeed"));const auto&t=field(b,"materialfreqslope");const auto r=std::from_chars(t.data(),t.data()+t.size(),v.frequencySlope);need(r.ec==std::errc{}&&r.ptr==t.data()+t.size()&&std::isfinite(v.frequencySlope),"50HI invalid material frequency slope");
            need(v.sampleSpeed>0&&v.minVolume>=0&&v.minVolume<=100&&v.minSpeed!=v.resonantSpeed&&v.maxSpeed!=v.resonantSpeed&&v.frequencySlope>=0,"50HI invalid material control authoring");}
        out=std::move(next);requests=std::move(unique);error.clear();return true;
    }catch(const std::exception&e){error=e.what();return false;}
}
bool readTrackContactMaterial50HI(const World48F&world,std::uint32_t zone,std::uint32_t ordinal,std::uint32_t identity,rr::game::ContactMaterial50HI&out,std::string&error){
    try{need(identity!=0,"50HI zero collider identity");const WorldInstance48F*found=nullptr;for(const auto&i:world.instances)if(i.zone==zone&&i.ordinal==ordinal){need(!found,"50HI duplicate SCB instance identity");found=&i;}need(found&&found->parameter_block<world.parameters.blocks.size(),"50HI absent collider SCB instance");
        int material=1;bool seen=false;for(const auto&f:world.parameters.blocks[found->parameter_block].fields)if(f.name=="materialid"){need(!seen,"50HI duplicate MaterialID");seen=true;material=number(f.value);}
        need(material>=0&&material<=255,"50HI unsupported MaterialID byte");rr::game::ContactMaterial50HI next{identity,static_cast<std::uint8_t>(material)};out=next;error.clear();return true;
    }catch(const std::exception&e){error=e.what();return false;}
}
}
