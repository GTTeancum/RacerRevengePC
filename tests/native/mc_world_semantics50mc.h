#pragma once
// MC-only source-backed semantic projection. Not compiled or run in this packet.
// Shared decodeWorld48F/validateWorldSemantics48F remain unchanged and strict.
#include <iostream>
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_world48f.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <set>
#include <utility>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "MC world projection requires strict finite IEEE arithmetic"
#endif
namespace mc_world_semantics50MC {
using namespace rr::assets;
namespace detail {
using Bytes = std::vector<std::uint8_t>;
using Vec = std::array<double, 3>;
struct Failure { DecodeError48C error; };
inline void require(bool ok, DecodeKind48C kind, const char* code, std::size_t at, const char* detail) {
    if (!ok) throw Failure{{kind, code, at, detail}};
}
inline void malformed(bool ok, const char* code, std::size_t at, const char* detail) {
    require(ok, DecodeKind48C::malformed, code, at, detail);
}
inline void checkCancel(const DecodeHooks48C* hooks, std::size_t at) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context), DecodeKind48C::cancelled,
        "world.cancelled", at, "Cancelled before output commit");
}
inline void progress(const DecodeHooks48C* hooks, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (hooks && hooks->progress) hooks->progress(hooks->context, stage, done, total);
}
inline void allocation(DecodeError48C& error) { error = {DecodeKind48C::limit, "world.allocation", 0, "Allocation failed inside bounded decoder"}; }
inline std::uint32_t findBlock(const ParameterDocument48F& p, const std::string& name) {
    const auto found = std::lower_bound(p.blocks.begin(), p.blocks.end(), name, [](const ParameterBlock48F& a, const std::string& b) { return a.name < b; });
    malformed(found != p.blocks.end() && found->name == name, "world.missing_block", 0, "Required scene block is missing");
    return static_cast<std::uint32_t>(found - p.blocks.begin());
}
inline const ParameterField48F& field(const ParameterBlock48F& block, const std::string& name) {
    const auto found = std::lower_bound(block.fields.begin(), block.fields.end(), name, [](const ParameterField48F& a, const std::string& b) { return a.name < b; });
    malformed(found != block.fields.end() && found->name == name, "world.missing_field", block.offset, "Required scene parameter is missing"); return *found;
}
inline std::uint32_t number(const ParameterField48F& f, std::uint32_t maximum) {
    malformed(!f.value.empty(), "world.integer_syntax", f.value_offset, "Empty unsigned decimal integer"); std::uint32_t n = 0;
    for (char c : f.value) {
        malformed(c >= '0' && c <= '9', "world.integer_syntax", f.value_offset, "Scene count/reference must be unsigned decimal");
        require(n <= maximum / 10 && unsigned(c - '0') <= maximum - n * 10, DecodeKind48C::limit,
            "world.integer_limit", f.value_offset, "Scene count/reference exceeds bound"); n = n * 10 + unsigned(c - '0');
    }
    return n;
}
inline double decimal(std::string s, std::size_t at) {
    const auto first = s.find_first_not_of(' '); malformed(first != std::string::npos, "world.vector_syntax", at, "Empty vector coordinate");
    const auto last = s.find_last_not_of(' '); s = s.substr(first, last - first + 1);
    std::size_t i = 0; if (s[i] == '+' || s[i] == '-') ++i;
    const auto digits = i; while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i;
    std::size_t digitCount = i - digits;
    if (i < s.size() && s[i] == '.') { const auto begin = ++i; while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i; digitCount += i - begin; }
    malformed(digitCount != 0, "world.vector_syntax", at, "Coordinate lacks decimal digits");
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        ++i; if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
        const auto begin = i; while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i;
        malformed(i > begin, "world.vector_syntax", at, "Coordinate exponent lacks digits");
    }
    malformed(i == s.size(), "world.vector_syntax", at, "Unrecognized decimal coordinate syntax");
    double value = 0; const char* begin = s.data() + (s[0] == '+' ? 1 : 0);
    const auto converted = std::from_chars(begin, s.data() + s.size(), value, std::chars_format::general);
    // from_chars reports out_of_range for underflow as well as overflow.
    // The frozen Python oracle retains signed zero for the former, including
    // tiny decimals written without an exponent. Determine decimal magnitude
    // from the already validated grammar; never use a locale-dependent fallback.
    if (converted.ec == std::errc::result_out_of_range) {
        const auto exponentAt = s.find_first_of("eE"); const auto mantissaEnd = exponentAt == std::string::npos ? s.size() : exponentAt;
        int exponent = 0;
        if(exponentAt!=std::string::npos){std::size_t e=exponentAt+1;const bool negative=s[e]=='-';if(s[e]=='+'||s[e]=='-')++e;
            for(;e<s.size();++e)exponent=std::min(10000,exponent*10+(s[e]-'0'));if(negative)exponent=-exponent;}
        int beforePoint=0,ordinal=0,firstNonzero=-1;bool afterPoint=false;
        for(std::size_t j=(s[0]=='+'||s[0]=='-')?1:0;j<mantissaEnd;++j){if(s[j]=='.'){afterPoint=true;continue;}
            if(!afterPoint)++beforePoint;if(firstNonzero<0&&s[j]!='0')firstNonzero=ordinal;++ordinal;}
        malformed(firstNonzero<0||exponent+beforePoint-firstNonzero-1 < -308, "world.vector_range", at, "Coordinate exceeds binary64");
        value = s[0] == '-' ? -0.0 : 0.0;
    } else malformed(converted.ec == std::errc{} && converted.ptr == s.data() + s.size(), "world.vector_range", at, "Coordinate exceeds binary64");
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559, "IEEE binary32 required");
    const float rounded = static_cast<float>(value); malformed(std::isfinite(rounded), "world.vector_range", at, "Coordinate exceeds finite binary32");
    return rounded;
}
inline Vec vector(const ParameterField48F& f) {
    Vec v{}; std::size_t begin = 0;
    for (unsigned i = 0; i < 3; ++i) {
        auto end = f.value.find(',', begin);
        malformed((i == 2) == (end == std::string::npos), "world.vector_syntax", f.value_offset, "Scene vector requires three components");
        if (end == std::string::npos) end = f.value.size(); v[i] = decimal(f.value.substr(begin, end - begin), f.value_offset + begin); begin = end + 1;
    }
    return v;
}
inline Vec cross(const Vec& a, const Vec& b) { return {{a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]}}; }
inline Vec normalize(const Vec& v, std::size_t at) {
    const double square = (v[0]*v[0]+v[1]*v[1])+v[2]*v[2];
    malformed(std::isfinite(square) && square > 1e-20, "world.orientation", at, "Degenerate or nonfinite orientation");
    const double length = std::sqrt(square); return {{v[0]/length, v[1]/length, v[2]/length}};
}
inline std::array<double,16> placement(const Vec& position,const Vec& look0,const Vec& up0,std::size_t at){
    const auto right0=cross(up0,look0),look1=cross(right0,up0),up=normalize(up0,at),look=normalize(look1,at),right=cross(up,look);
    std::array<double,16> matrix{};for(unsigned k=0;k<3;++k){matrix[k]=right[k];matrix[4+k]=up[k];matrix[8+k]=look[k];matrix[12+k]=position[k];}matrix[15]=1;return matrix;
}
template<std::size_t N>inline bool same(const std::array<double,N>& a,const std::array<double,N>& b){return std::memcmp(a.data(),b.data(),N*sizeof(double))==0;}
inline bool modelName(const std::string& text) {
    if (text.compare(0, 4, "zone") != 0) return false; std::size_t i = 4, begin = i;
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') ++i;
    if (i == begin || text.compare(i, 6, "-model") != 0) return false; i += 6; begin = i;
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') ++i; return i > begin && i == text.size();
}
inline const ParameterField48F* optionalField(const ParameterBlock48F& b,const char* key) {
 const ParameterField48F* result=nullptr;
 for(const auto& f:b.fields)if(f.name==key){malformed(!result,"mc.world.duplicate_used_field",f.entry_offset,"Duplicate interpreted field");result=&f;}
 return result;
}
inline Vec sourceUp(const ParameterBlock48F& b){
 const auto* f=optionalField(b,"upvector");
 // 27A5A0..27A5AC ->158CC8..158CE0; static producer3CA910..3CA95C.
 return f?vector(*f):Vec{{0.,1.,0.}};
}
inline bool sourceLighting(const ParameterBlock48F& b){
 const auto* f=optionalField(b,"uselighting");
 // 27A5BC supplies1;158B60 stores that default when lookup fails.
 if(!f)return true;
 require(f->value=="T"||f->value=="t"||f->value=="F",DecodeKind48C::unsupported,"world.lighting_flag",f->value_offset,"Only authenticated MC F/t and proven T flags supported");
 return f->value=="T"||f->value=="t";
}

}
inline bool decodeWorld(const std::vector<std::uint8_t>& input, World48F& output, DecodeError48C& error,
    const DecodeHooks48C* hooks=nullptr, const WorldLimits48F& limits=WorldLimits48F{}) {
    using namespace detail;
    error = {};
    try {
        // Common course rule (course_archive_receipt50ci.h): the stock SCB
        // identity is a receipt; the decoder's own rules below admit the data.
        if(!(input.size()==187711&&pod_materials50ed_fixture::detail::sha256(input)=="26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e"))
            std::cerr<<"course_member_identity site=mc_world_semantics member=tracks\\mc.scb bytes="<<input.size()<<" stock=0\n";
        World48F world;auto representationLimits=limits;representationLimits.retainDuplicateLabels=true;
        if(!decodeParameters48F(input,world.parameters,error,hooks,representationLimits))return false;
        for(const auto& block:world.parameters.blocks)for(std::size_t i=1;i<block.fields.size();++i)
          malformed(block.fields[i-1].name!=block.fields[i].name ||
            (block.name=="environment"&&(block.fields[i].name=="type"||block.fields[i].name=="colorrgb")),
            "mc.world.duplicate_scope",block.fields[i].entry_offset,"Only retained unused MC Environment duplicates admitted");
        const auto& p = world.parameters; world.scene_info_block = findBlock(p, "sceneinfo"); const auto& info = p.blocks[world.scene_info_block];
        bool hasZones=false,hasPortals=false,hasFlat=false;
        for(const auto& f:info.fields){hasZones=hasZones||f.name=="numzones";hasPortals=hasPortals||f.name=="numportals";}
        for(const auto& b:p.blocks)hasFlat=hasFlat||b.name=="staticmodels";
        if(!hasZones&&!hasPortals&&hasFlat){
            // END uses a complete unzoned StaticModels/modelN schema, without
            // the proven up/lighting fields. Validate its known structural
            // signature; do not turn arbitrary missing fields into a skip.
            const auto& flat=p.blocks[findBlock(p,"staticmodels")];const auto count=number(field(flat,"numberofstaticmodels"),10000);
            (void)field(info,"scenename");(void)vector(field(info,"defaultstartposition"));(void)vector(field(info,"defaultstartdirection"));
            std::set<std::string> expected;
            for(unsigned i=1;i<=count;++i){const auto key="model"+std::to_string(i);const auto& b=p.blocks[findBlock(p,key)];
                (void)field(b,"slt");(void)vector(field(b,"position"));(void)vector(field(b,"lookvector"));expected.insert(key);}
            for(const auto& b:p.blocks){if(b.name.compare(0,5,"model")!=0)continue;bool digits=b.name.size()>5;for(std::size_t i=5;i<b.name.size();++i)digits=digits&&b.name[i]>='0'&&b.name[i]<='9';
                malformed(!digits||expected.count(b.name)!=0,"world.flat_model_headers",b.offset,"Flat model headers disagree with count");}
            require(false,DecodeKind48C::unsupported,"world.flat_static_schema",flat.offset,"Complete original unzoned static-model schema needs separate placement proof");
        }
        const auto nz = number(field(info,"numzones"), std::min(limits.zones,256u)), np = number(field(info,"numportals"), std::min(limits.portals,4096u));
        malformed(nz != 0, "world.no_zones", info.offset, "No zones declared"); std::set<std::string> used;
        progress(hooks,"world.zones",0,nz);
        for (std::uint32_t zone = 1; zone <= nz; ++zone) {
            checkCancel(hooks,0); const auto zi = findBlock(p,"zone"+std::to_string(zone)); const auto& zb = p.blocks[zi];
            const auto count = number(field(zb,"numberofstaticmodels"), std::min(limits.instances_per_zone,10000u));
            require(world.instances.size()+count <= std::min(limits.instances,100000u), DecodeKind48C::limit,"world.instance_limit",zb.offset,"Total instance cap");
            world.zones.push_back({zone,count,zi});
            for (std::uint32_t ordinal=1; ordinal<=count; ++ordinal) {
                WorldInstance48F instance; instance.section="zone"+std::to_string(zone)+"-model"+std::to_string(ordinal);
                instance.zone=zone; instance.ordinal=ordinal; instance.parameter_block=findBlock(p,instance.section); const auto& b=p.blocks[instance.parameter_block];
                checkCancel(hooks,b.offset); instance.source_block_offset=b.offset; const auto& slt=field(b,"slt");
                std::string name=slt.value; for(char& c:name)if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                bool valid=name.size()>4 && name.substr(name.size()-4)==".slt";
                if(valid)for(std::size_t i=0;i<name.size()-4;++i){const char c=name[i];valid=valid&&((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-');}
                require(valid,DecodeKind48C::unsupported,"world.slt_reference",slt.value_offset,"Unproved SLT basename/path syntax");
                const auto stem=name.substr(0,name.size()-4);instance.geometry_member="geometry\\"+stem+".psg";instance.collision_member="geometry\\"+stem+".col";
                instance.position=vector(field(b,"position"));instance.look=vector(field(b,"lookvector"));instance.up=sourceUp(b);
                instance.use_lighting=sourceLighting(b);
                instance.matrix=placement(instance.position,instance.look,instance.up,b.offset);
                used.insert(instance.section);world.instances.push_back(std::move(instance));
            }
            progress(hooks,"world.zones",zone,nz);
        }
        for(const auto& b:p.blocks)malformed(!modelName(b.name)||used.count(b.name)!=0,"world.instance_headers",b.offset,"Model headers disagree with zone counts");
        for(std::uint32_t portal=1;portal<=np;++portal){
            WorldPortal48F result;result.section="portal"+std::to_string(portal);result.parameter_block=findBlock(p,result.section);const auto& b=p.blocks[result.parameter_block];checkCancel(hooks,b.offset);
            for(unsigned j=0;j<2;++j)result.zones[j]=number(field(b,"connect"+std::to_string(j+1)),nz);
            malformed(result.zones[0]&&result.zones[1]&&result.zones[0]!=result.zones[1],"world.portal_zones",b.offset,"Portal requires distinct nonzero zone references");
            for(unsigned j=0;j<4;++j)result.vertices[j]=vector(field(b,"vertex"+std::to_string(j+1)));world.portals.push_back(std::move(result));
        }
        checkCancel(hooks,input.size());output=std::move(world);return true;
    }catch(const Failure& f){error=f.error;}catch(const std::bad_alloc&){allocation(error);}return false;
}

// Re-decode authenticated raw bytes and compare every retained provenance field
// and typed semantic member. No edited/re-encoded surrogate SCB is constructed.
inline bool validateWorld(const World48F& w,DecodeError48C& error){
 using namespace detail;
 World48F expected;if(!decodeWorld(w.parameters.original_bytes,expected,error))return false;
 try {
 const auto& a=w.parameters;const auto& b=expected.parameters;
 malformed(a.version==b.version&&a.header_table_offset==b.header_table_offset&&
 a.preservesDuplicateLabels==b.preservesDuplicateLabels&&a.blocks.size()==b.blocks.size(),
 "mc.world.parameter_header",0,"Retained document header differs");
 for(std::size_t i=0;i<a.blocks.size();++i){const auto& x=a.blocks[i];const auto& y=b.blocks[i];
 malformed(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),"mc.world.parameter_block",x.offset,"Retained block differs");
 for(std::size_t j=0;j<x.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
 malformed(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,"mc.world.parameter_field",f.entry_offset,"Retained field differs");}}
 malformed(w.scene_info_block==expected.scene_info_block&&w.zones.size()==expected.zones.size()&&w.portals.size()==expected.portals.size()&&w.instances.size()==expected.instances.size(),"mc.world.counts",0,"World counts differ");
 for(std::size_t i=0;i<w.zones.size();++i){const auto& x=w.zones[i];const auto& y=expected.zones[i];malformed(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,"mc.world.zone",0,"Zone differs");}
 for(std::size_t i=0;i<w.portals.size();++i){const auto& x=w.portals[i];const auto& y=expected.portals[i];malformed(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,"mc.world.portal",0,"Portal differs");for(unsigned j=0;j<4;++j)malformed(same(x.vertices[j],y.vertices[j]),"mc.world.portal_vertex",0,"Portal vertex differs");}
 for(std::size_t i=0;i<w.instances.size();++i){const auto& x=w.instances[i];const auto& y=expected.instances[i];
 malformed(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&x.source_block_offset==y.source_block_offset&&same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix)&&x.use_lighting==y.use_lighting,"mc.world.instance",x.source_block_offset,"Instance differs from source-backed projection");}
 return true;
 }catch(const Failure& f){error=f.error;}catch(const std::bad_alloc&){allocation(error);}return false;
}

inline bool decode(const std::vector<std::uint8_t>& b,World48F& w,DecodeError48C& e,const DecodeHooks48C* h=nullptr,const WorldLimits48F& l=WorldLimits48F{}){return decodeWorld(b,w,e,h,l);}
inline bool validate(const World48F& w,DecodeError48C& e){return validateWorld(w,e);}

}
