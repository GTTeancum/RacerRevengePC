#include "material_audio50hi.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <stdexcept>
namespace rr::assets { namespace {
void need(bool x,const char*m){if(!x)throw std::runtime_error(m);}
const ParameterBlock48F& block(const ParameterDocument48F&p,const std::string&name){for(const auto&b:p.blocks)if(b.name==name)return b;throw std::runtime_error("50HI missing AUB block");}
const std::string* optionalField(const ParameterBlock48F&b,const char*name){const std::string* value=nullptr;for(const auto&f:b.fields)if(f.name==name){need(!value,"50HI consumed AUB field is duplicated");value=&f.value;}return value;}
const std::string& field(const ParameterBlock48F&b,const char*name){const auto* value=optionalField(b,name);need(value,"50HI missing AUB field");return *value;}
const ParameterBlock48F* optionalBlock(const ParameterDocument48F&p,const std::string&name){for(const auto&b:p.blocks)if(b.name==name)return &b;return nullptr;}
int number(const std::string&s){int v=0;const auto r=std::from_chars(s.data(),s.data()+s.size(),v);need(r.ec==std::errc{}&&r.ptr==s.data()+s.size(),"50HI invalid integer authoring");return v;}
std::string filename(const ParameterBlock48F&b){auto n=field(b,"soundresourcefile");need(!n.empty()&&n.size()<200&&n.find('.')==std::string::npos,"50HI invalid sound resource name");for(auto&c:n){need((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='\\'||c=='/',"50HI invalid sound resource path");if(c=='\\')c='/';}need(n.rfind("sfx/race/",0)==0&&n.find("//")==std::string::npos,"50HI unexpected resource family");return n+".vag";}
std::uint32_t rotateRight50HI(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
void shaBlock50HI(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight50HI(w[i-15],7) ^ rotateRight50HI(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight50HI(w[i-2],17) ^ rotateRight50HI(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight50HI(e,6) ^ rotateRight50HI(e,11) ^ rotateRight50HI(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight50HI(a,2) ^ rotateRight50HI(a,13) ^ rotateRight50HI(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
bool hashMatches50HI(const std::uint8_t* data, std::size_t size,
    const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock50HI(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock50HI(tail.data(), h);
    if (padded == 128) shaBlock50HI(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
enum class CatalogShape50HI { legacy, authenticatedMb, authenticatedTc, authenticatedTb, authenticatedMc };
bool readCatalog50HI(const std::vector<std::uint8_t>&aub,MaterialAudioCatalog50HI&out,std::vector<AudioRequest50HC>&requests,std::string&error,CatalogShape50HI shape){
    try{
        const bool mc=shape==CatalogShape50HI::authenticatedMc;
        if(mc)need(aub.size()==50722u&&hashMatches50HI(aub.data(),aub.size(),
            "bbb238196c131979092c7a5cc5066fdfc131156ecb6ec632786279786f779554"),
            "50HI exact MC AUB identity differs");
        const bool tb=shape==CatalogShape50HI::authenticatedTb;
        if(tb)need(aub.size()==50826u&&hashMatches50HI(aub.data(),aub.size(),
            "00729a0e848ca3a28996b312bcd789dbc3a3798c8b803ab3c612f4379dd94a19"),
            "50HI exact TB AUB identity differs");
        const bool mb=shape==CatalogShape50HI::authenticatedMb;
        const bool tc=shape==CatalogShape50HI::authenticatedTc;
        if(tc)need(aub.size()==44646u&&hashMatches50HI(aub.data(),aub.size(),
            "a4e331f8ac8a4849e717f6a37588304b1642129b8b690b39d7b818449eab838d"),
            "50HI exact TC AUB identity differs");
        if(mb)need(aub.size()==57210u&&hashMatches50HI(aub.data(),aub.size(),
            "669a20fc6275a7463a1a4a8f529b41ef45ce971f8f4d49b8631f5279699a0d89"),
            "50HI exact MB AUB identity differs");
        ParameterDocument48F p;DecodeError48C d;WorldLimits48F limits{};limits.retainDuplicateLabels=true;need(decodeParameters48F(aub,p,d,nullptr,limits),"50HI AUB parameter decode failed");
        const auto&s=block(p,"sounds");const int rows=number(field(s,"nummaterialcollisionsounds"));
        const int driveRows=number(field(s,"nummaterialdriveonsounds"));
        need(mc?(rows==12&&driveRows==8):tb?(rows==24&&driveRows==20):tc?(rows==16&&driveRows==12):mb?(rows==20&&driveRows==12):
            (((rows==8||rows==12||rows==20)&&driveRows==rows)||
             ((rows==16||rows==32)&&driveRows==12)),"50HI unsupported independent material row counts");
        MaterialAudioCatalog50HI next;std::vector<AudioRequest50HC> unique;
        next.parameters.rowCount=static_cast<std::uint32_t>(rows);
        next.parameters.driveRowCount=static_cast<std::uint32_t>(driveRows);
        const int materials=number(field(s,"nummaterials"));need((mc||tb||tc)?materials==5:mb?materials==3:(materials>=1&&materials<=4),"50HI unsupported material assignment count");
        next.materialCount=static_cast<std::uint32_t>(materials);
        const auto* defaultText=optionalField(s,"defaultmaterial");const int fallback=defaultText?number(*defaultText):0;
        need(fallback>=0&&fallback<=255,"50HI invalid default material byte");next.defaultMaterial=static_cast<std::uint32_t>(fallback);
        if(mb)need(fallback==1,"50HI MB default material differs");
        if(tc)need(fallback==1,"50HI TC default material differs");
        if(mc)need(fallback==1,"50HI MC default material differs");
        if(tb)need(fallback==1,"50HI TB default material differs");
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
            const bool tbWood=tb&&!r.loop&&loop&&(name=="sfx/race/woodhit1.vag"||
                name=="sfx/race/woodhit2.vag"||name=="sfx/race/woodhit3.vag"||name=="sfx/race/woodhit4.vag");
            need(r.loop==loop||tbWood,"50HI conflicting duplicate clip flags");return;
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
        if(mb){
            need(next.orderedObjectMaterials.size()==29u&&unique.size()==25u,
                "50HI MB material source traversal differs");
            for(std::uint32_t i=0;i<20u;++i)need(next.collisionSectionKeys[i]==i+1u,
                "50HI MB collision section identity differs");
            for(std::uint32_t i=0;i<12u;++i)need(next.driveSectionKeys[i]==i+1u,
                "50HI MB drive section identity differs");
        }
        if(tc){
            need(next.orderedObjectMaterials.size()==8u&&unique.size()==21u,
                "50HI TC material source traversal differs");
            for(std::uint32_t i=0;i<8u;++i)need(next.orderedObjectMaterials[i].material==2u&&
                next.orderedObjectMaterials[i].object=="tc_colbeacon0"+std::to_string(i+1u),
                "50HI TC strict-less-than assignment order differs");
            for(std::uint32_t i=0;i<16u;++i)need(next.collisionSectionKeys[i]==i+1u,
                "50HI TC collision section identity differs");
            for(std::uint32_t i=0;i<12u;++i)need(next.driveSectionKeys[i]==i+1u,
                "50HI TC drive section identity differs");
        }
        if(tb){
            need(next.orderedObjectMaterials.size()==54u&&unique.size()==33u,
                "50HI TB material source traversal differs");
            for(std::uint32_t i=0;i<24u;++i)need(next.collisionSectionKeys[i]==i+1u,
                "50HI TB collision section identity differs");
            for(std::uint32_t i=0;i<20u;++i)need(next.driveSectionKeys[i]==i+1u,
                "50HI TB drive section identity differs");
            for(std::uint32_t i=0;i<4u;++i)need(next.collision[16u+i]==next.drive[16u+i]&&
                next.collision[16u+i]=="sfx/race/woodhit"+std::to_string(i+1u)+".vag",
                "50HI TB row-local loop alias differs");
        }
        if(mc){
            need(next.orderedObjectMaterials.size()==3u&&unique.size()==15u,"50HI MC source traversal differs");
            constexpr const char* names[]{"colshieldx","colforces","colc_beacon"};
            for(std::uint32_t i=0;i<3;++i)need(next.orderedObjectMaterials[i].material==2&&next.orderedObjectMaterials[i].object==names[i],"50HI MC strict-less-than source assignment differs");
            for(std::uint32_t i=0;i<12;++i)need(next.collisionSectionKeys[i]==i+1,"50HI MC collision source ordinal differs");
            for(std::uint32_t i=0;i<8;++i)need(next.driveSectionKeys[i]==i+1,"50HI MC drive source ordinal differs");
        }
        out=std::move(next);requests=std::move(unique);error.clear();return true;
    }catch(const std::exception&e){error=e.what();return false;}
}
} // namespace
bool readMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error,CatalogShape50HI::legacy);
}
bool readMbMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error,CatalogShape50HI::authenticatedMb);
}
bool readTcMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error,CatalogShape50HI::authenticatedTc);
}
bool readTbMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error,CatalogShape50HI::authenticatedTb);
}
bool readMcMaterialAudioCatalog50HI(const std::vector<std::uint8_t>& aub,
    MaterialAudioCatalog50HI& out,std::vector<AudioRequest50HC>& requests,std::string& error){
    return readCatalog50HI(aub,out,requests,error,CatalogShape50HI::authenticatedMc);
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
