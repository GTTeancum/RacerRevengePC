#include "material_audio50hi.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
namespace rr::assets { namespace {
void need(bool x,const char*m){if(!x)throw std::runtime_error(m);}
const ParameterBlock48F& block(const ParameterDocument48F&p,const std::string&name){for(const auto&b:p.blocks)if(b.name==name)return b;throw std::runtime_error("50HI missing AUB block");}
const std::string* optionalField(const ParameterBlock48F&b,const char*name){const std::string* value=nullptr;for(const auto&f:b.fields)if(f.name==name){need(!value,"50HI consumed AUB field is duplicated");value=&f.value;}return value;}
const std::string& field(const ParameterBlock48F&b,const char*name){const auto* value=optionalField(b,name);need(value,"50HI missing AUB field");return *value;}
const ParameterBlock48F* optionalBlock(const ParameterDocument48F&p,const std::string&name){for(const auto&b:p.blocks)if(b.name==name)return &b;return nullptr;}
int number(const std::string&s){int v=0;const auto r=std::from_chars(s.data(),s.data()+s.size(),v);need(r.ec==std::errc{}&&r.ptr==s.data()+s.size(),"50HI invalid integer authoring");return v;}
std::string filename(const ParameterBlock48F&b){auto n=field(b,"soundresourcefile");need(!n.empty()&&n.size()<200&&n.find('.')==std::string::npos,"50HI invalid sound resource name");for(auto&c:n){need((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='\\'||c=='/',"50HI invalid sound resource path");if(c=='\\')c='/';}need(n.rfind("sfx/race/",0)==0&&n.find("//")==std::string::npos,"50HI unexpected resource family");return n+".vag";}
bool readCatalog50HI(const std::vector<std::uint8_t>&aub,MaterialAudioCatalog50HI&out,std::vector<AudioRequest50HC>&requests,std::string&error){
    try{
        ParameterDocument48F p;DecodeError48C d;WorldLimits48F limits{};limits.retainDuplicateLabels=true;need(decodeParameters48F(aub,p,d,nullptr,limits),"50HI AUB parameter decode failed");
        const auto&s=block(p,"sounds");const int rows=number(field(s,"nummaterialcollisionsounds"));
        const int driveRows=number(field(s,"nummaterialdriveonsounds"));
        // Bounds: the native row stores. Stock shapes are receipts.
        need(rows>=1&&static_cast<std::size_t>(rows)<=rr::game::maxMaterialAudioRows50HI&&
            driveRows>=1&&static_cast<std::size_t>(driveRows)<=rr::game::maxMaterialDriveRows50HI,
            "50HI material row counts outside the native sound stores");
        MaterialAudioCatalog50HI next;std::vector<AudioRequest50HC> unique;
        next.parameters.rowCount=static_cast<std::uint32_t>(rows);
        next.parameters.driveRowCount=static_cast<std::uint32_t>(driveRows);
        const int materials=number(field(s,"nummaterials"));need(materials>=0&&materials<=255,"50HI material assignment count outside one material byte");
        next.materialCount=static_cast<std::uint32_t>(materials);
        const auto* defaultText=optionalField(s,"defaultmaterial");const int fallback=defaultText?number(*defaultText):0;
        need(fallback>=0&&fallback<=255,"50HI invalid default material byte");next.defaultMaterial=static_cast<std::uint32_t>(fallback);
        //271674..271804: increasing material order; missing list means0.
        //2717E4 is STRICT less-than. Preserve this source off-by-one behavior.
        for(int material=1;material<=materials;++material){
            const auto* objects=optionalBlock(p,"collisionobjectsmaterial"+std::to_string(material));
            const auto* text=objects?optionalField(*objects,"numcollisionobjects"):nullptr;
            const int count=text?number(*text):0;need(count>=0&&count<=10000,"50HI material-object bound");
            for(int object=1;object<count;++object){
                const auto* row=optionalBlock(p,"material"+std::to_string(material)+"-object"+std::to_string(object));
                const auto* name=row?optionalField(*row,"obj"):nullptr;
                next.orderedObjectMaterials.push_back({static_cast<std::uint32_t>(material),name?*name:std::string{}});
            }
        }
        auto add=[&](const std::string&name,bool loop){for(auto&r:unique)if(r.name==name){
            // The SHA-bound TB loader constructs independent292AF0 flags1 and
            //292CB0 flags81 owners from these same four immutable VAG names.
            // AudioBank decoding ignores request.loop; each D44/host row keeps
            //its own playback flag. Never broaden a legacy duplicate policy.
            // One PCM shared by a one-shot collision row and a looping drive
            // row is two owners of one clip (stock TB's WOODHIT1..4); the
            // first request's loop flag never governs decoding. Any other
            // flag conflict is refused.
            const bool rowLocalAlias=!r.loop&&loop;
            need(r.loop==loop||rowLocalAlias,"50HI conflicting duplicate clip flags");return;
        }unique.push_back({name,loop,false,false});};
        //279B60 and279E50 allocate and traverse their own authored count.
        //157D00/157DD0 returns on a missing section WITHOUT replacing the
        //current section.279C1C/279F04 ignore that return and still read it.
        //Keep the last selected section, not a fabricated numbered filename.
        const ParameterBlock48F* selected=&s;std::uint32_t selectedKey=0;
        for(unsigned i=0;i<static_cast<unsigned>(rows);++i){
            if(const auto* row=optionalBlock(p,"materialcollisionsound"+std::to_string(i+1))){selected=row;selectedKey=i+1;}
            need(selectedKey!=0,"50HI initial collision source section absent");
            next.collision[i]=filename(*selected);next.collisionSectionKeys[i]=selectedKey;
        }
        // The drive loader selects Sounds again before its independent loop.
        selected=&s;selectedKey=0;
        for(unsigned i=0;i<static_cast<unsigned>(driveRows);++i){
            if(const auto* row=optionalBlock(p,"materialdriveonsound"+std::to_string(i+1))){selected=row;selectedKey=i+1;}
            need(selectedKey!=0,"50HI initial drive source section absent");
            const auto& b=*selected;next.drive[i]=filename(b);next.driveSectionKeys[i]=selectedKey;
            auto&v=next.parameters.drive[i];v.sampleSpeed=number(field(b,"materialsamplespeed"));v.minVolume=number(field(b,"materialminvolume"));v.minSpeed=number(field(b,"materialminspeed"));v.maxSpeed=number(field(b,"materialmaxspeed"));v.resonantSpeed=number(field(b,"materialresonantfreqspeed"));const auto&t=field(b,"materialfreqslope");const auto r=std::from_chars(t.data(),t.data()+t.size(),v.frequencySlope);need(r.ec==std::errc{}&&r.ptr==t.data()+t.size()&&std::isfinite(v.frequencySlope),"50HI invalid material frequency slope");
            need(v.sampleSpeed>0&&v.minVolume>=0&&v.minVolume<=100&&v.minSpeed!=v.resonantSpeed&&v.maxSpeed!=v.resonantSpeed&&v.frequencySlope>=0,"50HI invalid material control authoring");
        }
        // Keep the legacy unique-bank request order. Only PCM filenames are
        // deduplicated; every collision/drive owner above stays independent.
        for(unsigned i=0;i<static_cast<unsigned>(std::max(rows,driveRows));++i){
            if(i<static_cast<unsigned>(rows))add(next.collision[i],false);
            if(i<static_cast<unsigned>(driveRows))add(next.drive[i],true);
        }
        out=std::move(next);requests=std::move(unique);error.clear();return true;
    }catch(const std::exception&e){error=e.what();return false;}
}
} // namespace
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error);
}
bool applyTrackMaterialAssignments50HI(const MaterialAudioCatalog50HI& catalog,const World48F& world,
    std::uint32_t zone,std::uint32_t ordinal,rr::game::ContactMaterial50HI& value,std::string& error){
    try{const WorldInstance48F* original=nullptr;for(const auto& i:world.instances)if(i.zone==zone&&i.ordinal==ordinal){need(!original,"50HI duplicate placement");original=&i;}
        need(original&&original->parameter_block<world.parameters.blocks.size(),"50HI missing actual placement");
        const auto& p=world.parameters.blocks[original->parameter_block];
        const auto* col=optionalField(p,"col");const auto* slt=optionalField(p,"slt");need(col||slt,"50HI original collision resource name absent");
        const auto& authored=col?*col:*slt;
        const auto delimiter=authored.find('.');need(delimiter!=std::string::npos,"50HI unsupported original collision basename");
        const auto name=authored.substr(0,std::min<std::size_t>(delimiter,50));
        std::uint32_t material=value.materialA9;
        //271728 uses exact strcmp; a later authored match overwrites an earlier.
        for(const auto& item:catalog.orderedObjectMaterials){
            if(name==item.object)material=item.material;
            else if(catalog.defaultMaterial&&material==0)material=catalog.defaultMaterial;
        }
        need(material<=255&&material<catalog.parameters.rowCount/4u,"50HI assigned material lacks a complete collision sound family");
        value.materialA9=static_cast<std::uint8_t>(material);error.clear();return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool readTrackContactMaterial50HI(const World48F&world,std::uint32_t zone,std::uint32_t ordinal,std::uint32_t identity,rr::game::ContactMaterial50HI&out,std::string&error){
    try{need(identity!=0,"50HI zero collider identity");const WorldInstance48F*found=nullptr;for(const auto&i:world.instances)if(i.zone==zone&&i.ordinal==ordinal){need(!found,"50HI duplicate SCB instance identity");found=&i;}need(found&&found->parameter_block<world.parameters.blocks.size(),"50HI absent collider SCB instance");
        int material=1;bool seen=false;for(const auto&f:world.parameters.blocks[found->parameter_block].fields)if(f.name=="materialid"){need(!seen,"50HI duplicate MaterialID");seen=true;material=number(f.value);}
        need(material>=0&&material<=255,"50HI unsupported MaterialID byte");rr::game::ContactMaterial50HI next{identity,static_cast<std::uint8_t>(material)};out=next;error.clear();return true;
    }catch(const std::exception&e){error=e.what();return false;}
}
}
