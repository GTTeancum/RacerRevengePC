#include "world_asset48f.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace rr { namespace assets {
namespace {
using Bytes = std::vector<std::uint8_t>;
void need(bool ok, const char* code) { if (!ok) throw std::runtime_error(code); }
bool textOk(const std::string& s) { for (unsigned char c:s) if(c<32||c>=127)return false;return true; }
struct Writer {
    Bytes bytes;
    void raw(const std::uint8_t* p,std::size_t n){need(n<=max_world_asset_bytes48f-bytes.size(),"rrworld.output_limit");if(n)bytes.insert(bytes.end(),p,p+n);}
    void u32(std::size_t v){need(v<=0xffffffffu,"rrworld.integer_range");std::uint8_t b[4];for(unsigned i=0;i<4;++i)b[i]=static_cast<std::uint8_t>(v>>(8*i));raw(b,4);}
    void f64(double v){need(std::isfinite(v),"rrworld.nonfinite");std::uint64_t b;std::memcpy(&b,&v,8);std::uint8_t x[8];for(unsigned i=0;i<8;++i)x[i]=static_cast<std::uint8_t>(b>>(8*i));raw(x,8);}
    void text(const std::string& s){need(s.size()<=511&&textOk(s),"rrworld.string");u32(s.size());raw(reinterpret_cast<const std::uint8_t*>(s.data()),s.size());}
    template<std::size_t N>void values(const std::array<double,N>& a){for(double d:a)f64(d);}
};
struct Reader {
    const Bytes& bytes;std::size_t at=0;
    void range(std::size_t n)const{need(at<=bytes.size()&&n<=bytes.size()-at,"rrworld.truncated");}
    std::uint32_t u32(){range(4);const auto n=std::uint32_t(bytes[at])|(std::uint32_t(bytes[at+1])<<8)|(std::uint32_t(bytes[at+2])<<16)|(std::uint32_t(bytes[at+3])<<24);at+=4;return n;}
    double f64(){range(8);std::uint64_t n=0;for(unsigned i=0;i<8;++i)n|=std::uint64_t(bytes[at+i])<<(8*i);at+=8;double v;std::memcpy(&v,&n,8);need(std::isfinite(v),"rrworld.nonfinite");return v;}
    std::string text(){const auto n=u32();need(n<=511,"rrworld.string_limit");range(n);std::string s(bytes.begin()+at,bytes.begin()+at+n);at+=n;need(textOk(s),"rrworld.string");return s;}
    template<std::size_t N>void values(std::array<double,N>& a){for(double& d:a)d=f64();}
};
void rawString(const Bytes& raw,std::size_t at,std::size_t count,const std::string& value,bool padded=false){
    need(at<=raw.size()&&count<=raw.size()-at&&count>value.size(),"rrworld.provenance_extent");
    need(padded||count==value.size()+1,"rrworld.provenance_length");
    for(std::size_t i=0;i<value.size();++i)need(raw[at+i]==static_cast<unsigned char>(value[i]),"rrworld.provenance_string");
    for(std::size_t i=value.size();i<count;++i)need(raw[at+i]==0,"rrworld.provenance_suffix");
}
bool lowercase(const std::string& s){if(s.empty()||!textOk(s))return false;for(char c:s)if(c>='A'&&c<='Z')return false;return true;}
std::uint32_t original32(const Bytes& raw,std::size_t at){need(at<=raw.size()&&4<=raw.size()-at,"rrworld.original_u32_extent");return std::uint32_t(raw[at])|(std::uint32_t(raw[at+1])<<8)|(std::uint32_t(raw[at+2])<<16)|(std::uint32_t(raw[at+3])<<24);}
void validate(const World48F& w){
    const auto& p=w.parameters;const auto& raw=p.original_bytes;
    need(raw.size()>=15&&raw.size()<=2u*1024u*1024u&&p.version==1,"rrworld.original_extent_version");
    need(!p.blocks.empty()&&p.blocks.size()<=8192,"rrworld.block_count");
    need(std::memcmp(raw.data(),"BINARY",7)==0&&original32(raw,7)==p.version&&original32(raw,11)==p.blocks.size(),"rrworld.original_header");
    need(!w.zones.empty()&&w.zones.size()<=256&&w.portals.size()<=4096&&w.instances.size()<=100000,"rrworld.world_counts");
    need(w.scene_info_block<p.blocks.size()&&p.blocks[w.scene_info_block].name=="sceneinfo","rrworld.sceneinfo");
    std::size_t nameNext=15,next=p.header_table_offset+p.blocks.size()*8,totalFields=0;std::string previous;
    need(next<=raw.size(),"rrworld.table_extent");
    for(std::size_t i=0;i<p.blocks.size();++i){const auto& b=p.blocks[i];
        need(lowercase(b.name)&&b.name>previous&&b.name.size()<512,"rrworld.block_names");previous=b.name;
        need(b.name_offset==nameNext+4,"rrworld.name_offset");rawString(raw,b.name_offset,b.name.size()+1,b.name);nameNext=b.name_offset+b.name.size()+1;
        need(original32(raw,b.name_offset-4)==b.name.size()+1,"rrworld.original_name_length");
        need(b.table_entry_offset==p.header_table_offset+i*8&&b.offset==next,"rrworld.block_offsets");
        need(original32(raw,b.table_entry_offset)==b.offset&&original32(raw,b.table_entry_offset+4)==b.size_without_count,"rrworld.original_block_table");
        need(b.offset<=raw.size()&&4<=raw.size()-b.offset&&b.size_without_count<=raw.size()-b.offset-4,"rrworld.block_extent");
        need(!b.fields.empty()&&b.fields.size()<=1024&&b.fields.size()*43<=b.size_without_count,"rrworld.field_count");
        need(original32(raw,b.offset)==b.fields.size(),"rrworld.original_field_count");
        totalFields+=b.fields.size();need(totalFields<=raw.size()/43,"rrworld.total_fields");const auto base=b.offset+4;std::string fieldPrevious;
        std::vector<std::pair<std::size_t,std::size_t>> coverage;
        for(std::size_t j=0;j<b.fields.size();++j){const auto& f=b.fields[j];
            need(lowercase(f.name)&&f.name.size()<35&&f.name>fieldPrevious,"rrworld.field_names");fieldPrevious=f.name;
            need(f.entry_offset==base+j*43,"rrworld.field_offset");rawString(raw,f.entry_offset,35,f.name,true);
            need(original32(raw,f.entry_offset+35)==f.value_offset-base&&original32(raw,f.entry_offset+39)==f.value_bytes,"rrworld.original_value_table");
            need(f.value_bytes>=1&&f.value_bytes<=512&&f.value_offset>=base+b.fields.size()*43&&f.value_offset<=base+b.size_without_count&&f.value_bytes<=base+b.size_without_count-f.value_offset,"rrworld.value_extent");
            rawString(raw,f.value_offset,f.value_bytes,f.value);coverage.push_back({f.value_offset,f.value_offset+f.value_bytes});
        }
        std::sort(coverage.begin(),coverage.end());std::size_t valueNext=base+b.fields.size()*43;
        for(const auto& c:coverage){need(c.first==valueNext,"rrworld.value_coverage");valueNext=c.second;}
        need(valueNext==base+b.size_without_count,"rrworld.block_suffix");next=base+b.size_without_count;
    }
    need(nameNext==p.header_table_offset&&next==raw.size(),"rrworld.provenance_coverage");
    std::size_t instance=0;
    for(std::size_t i=0;i<w.zones.size();++i){const auto& z=w.zones[i];need(z.zone==i+1&&z.models<=10000&&z.parameter_block<p.blocks.size(),"rrworld.zone");
        need(p.blocks[z.parameter_block].name=="zone"+std::to_string(z.zone),"rrworld.zone_block");
        for(unsigned j=1;j<=z.models;++j){need(instance<w.instances.size(),"rrworld.instance_count");const auto& x=w.instances[instance++];
            need(x.zone==z.zone&&x.ordinal==j&&x.section=="zone"+std::to_string(z.zone)+"-model"+std::to_string(j),"rrworld.instance_identity");
            need(x.parameter_block<p.blocks.size()&&p.blocks[x.parameter_block].name==x.section&&x.source_block_offset==p.blocks[x.parameter_block].offset,"rrworld.instance_provenance");
            need(x.geometry_member.size()>13&&x.geometry_member.compare(0,9,"geometry\\")==0&&x.geometry_member.substr(x.geometry_member.size()-4)==".psg","rrworld.geometry_reference");
            const auto stem=x.geometry_member.substr(9,x.geometry_member.size()-13);for(char c:stem)need((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-',"rrworld.geometry_basename");
            need(x.collision_member=="geometry\\"+stem+".col","rrworld.collision_reference");
            for(unsigned k=0;k<3;++k){need(std::isfinite(x.position[k])&&std::isfinite(x.look[k])&&std::isfinite(x.up[k]),"rrworld.instance_nonfinite");need(x.matrix[12+k]==x.position[k],"rrworld.translation");}
            for(double v:x.matrix)need(std::isfinite(v),"rrworld.matrix_nonfinite");need(x.matrix[3]==0&&x.matrix[7]==0&&x.matrix[11]==0&&x.matrix[15]==1,"rrworld.affine_matrix");
        }
    }
    need(instance==w.instances.size(),"rrworld.instance_count");
    for(std::size_t i=0;i<w.portals.size();++i){const auto& v=w.portals[i];need(v.section=="portal"+std::to_string(i+1)&&v.parameter_block<p.blocks.size()&&p.blocks[v.parameter_block].name==v.section,"rrworld.portal_identity");
        need(v.zones[0]>=1&&v.zones[0]<=w.zones.size()&&v.zones[1]>=1&&v.zones[1]<=w.zones.size()&&v.zones[0]!=v.zones[1],"rrworld.portal_zones");for(const auto& point:v.vertices)for(double x:point)need(std::isfinite(x),"rrworld.portal_nonfinite");}
    DecodeError48C semantic;const bool validSemantics=validateWorldSemantics48F(w,semantic);need(validSemantics,semantic.code.c_str());
}
}
bool encodeWorldAsset48F(const World48F& world,Bytes& output,std::string& error){
    error.clear();try{validate(world);Writer w;const auto& p=world.parameters;
        const char magic[]="RRWORLD1";w.raw(reinterpret_cast<const std::uint8_t*>(magic),8);w.u32(1);w.u32(0);w.u32(p.version);w.u32(p.header_table_offset);w.u32(p.original_bytes.size());w.u32(p.blocks.size());w.u32(world.zones.size());w.u32(world.portals.size());w.u32(world.instances.size());w.u32(world.scene_info_block);
        w.raw(p.original_bytes.data(),p.original_bytes.size());
        for(const auto& b:p.blocks){w.text(b.name);w.u32(b.name_offset);w.u32(b.table_entry_offset);w.u32(b.offset);w.u32(b.size_without_count);w.u32(b.fields.size());
            for(const auto& f:b.fields){w.text(f.name);w.text(f.value);w.u32(f.entry_offset);w.u32(f.value_offset);w.u32(f.value_bytes);}}
        for(const auto& z:world.zones){w.u32(z.zone);w.u32(z.models);w.u32(z.parameter_block);}
        for(const auto& ptl:world.portals){w.text(ptl.section);w.u32(ptl.parameter_block);for(auto z:ptl.zones)w.u32(z);for(const auto& v:ptl.vertices)w.values(v);}
        for(const auto& x:world.instances){w.text(x.section);w.text(x.geometry_member);w.text(x.collision_member);w.u32(x.zone);w.u32(x.ordinal);w.u32(x.parameter_block);w.u32(x.source_block_offset);w.values(x.position);w.values(x.look);w.values(x.up);w.values(x.matrix);w.u32(x.use_lighting?1:0);}
        const auto size=w.bytes.size();for(unsigned i=0;i<4;++i)w.bytes[12+i]=static_cast<std::uint8_t>(size>>(8*i));output=std::move(w.bytes);return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool parseWorldAsset48F(const Bytes& input,World48F& output,std::string& error){
    error.clear();try{static_assert(sizeof(double)==8&&std::numeric_limits<double>::is_iec559,"IEEE binary64 required");
        need(input.size()>=48&&input.size()<=max_world_asset_bytes48f,"rrworld.input_extent");need(std::memcmp(input.data(),"RRWORLD1",8)==0,"rrworld.magic");Reader r{input,8};need(r.u32()==1,"rrworld.version");need(r.u32()==input.size(),"rrworld.file_size");
        World48F world;auto& p=world.parameters;p.version=r.u32();p.header_table_offset=r.u32();const auto rawCount=r.u32(),blocks=r.u32(),zones=r.u32(),portals=r.u32(),instances=r.u32();world.scene_info_block=r.u32();
        need(rawCount>=15&&rawCount<=2u*1024u*1024u&&blocks>=1&&blocks<=8192&&zones>=1&&zones<=256&&portals<=4096&&instances<=100000&&instances<=blocks,"rrworld.header_limits");
        r.range(rawCount);p.original_bytes.assign(input.begin()+r.at,input.begin()+r.at+rawCount);r.at+=rawCount;std::size_t totalFields=0;
        for(unsigned i=0;i<blocks;++i){ParameterBlock48F b;b.name=r.text();b.name_offset=r.u32();b.table_entry_offset=r.u32();b.offset=r.u32();b.size_without_count=r.u32();const auto n=r.u32();totalFields+=n;need(n>=1&&n<=1024&&totalFields<=rawCount/43,"rrworld.field_limits");
            for(unsigned j=0;j<n;++j){ParameterField48F f;f.name=r.text();f.value=r.text();f.entry_offset=r.u32();f.value_offset=r.u32();f.value_bytes=r.u32();b.fields.push_back(std::move(f));}p.blocks.push_back(std::move(b));}
        for(unsigned i=0;i<zones;++i){WorldZone48F z;z.zone=r.u32();z.models=r.u32();z.parameter_block=r.u32();world.zones.push_back(z);}
        for(unsigned i=0;i<portals;++i){WorldPortal48F ptl;ptl.section=r.text();ptl.parameter_block=r.u32();for(auto& z:ptl.zones)z=r.u32();for(auto& v:ptl.vertices)r.values(v);world.portals.push_back(std::move(ptl));}
        for(unsigned i=0;i<instances;++i){WorldInstance48F x;x.section=r.text();x.geometry_member=r.text();x.collision_member=r.text();x.zone=r.u32();x.ordinal=r.u32();x.parameter_block=r.u32();x.source_block_offset=r.u32();r.values(x.position);r.values(x.look);r.values(x.up);r.values(x.matrix);const auto flag=r.u32();need(flag<=1,"rrworld.lighting_flag");x.use_lighting=flag!=0;world.instances.push_back(std::move(x));}
        need(r.at==input.size(),"rrworld.trailing_bytes");validate(world);output=std::move(world);return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
} }
