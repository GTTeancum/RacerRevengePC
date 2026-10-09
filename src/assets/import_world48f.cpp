#include "import_world48f.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <set>
#include <utility>

namespace rr { namespace assets {
namespace {
using Bytes = std::vector<std::uint8_t>;
using Vec = std::array<double, 3>;
struct Failure { DecodeError48C error; };
void require(bool ok, DecodeKind48C kind, const char* code, std::size_t at, const char* detail) {
    if (!ok) throw Failure{{kind, code, at, detail}};
}
void malformed(bool ok, const char* code, std::size_t at, const char* detail) {
    require(ok, DecodeKind48C::malformed, code, at, detail);
}
void checkCancel(const DecodeHooks48C* hooks, std::size_t at) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context), DecodeKind48C::cancelled,
        "world.cancelled", at, "Cancelled before output commit");
}
void progress(const DecodeHooks48C* hooks, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (hooks && hooks->progress) hooks->progress(hooks->context, stage, done, total);
}
bool ascii(const std::string& text) { for (unsigned char c : text) if (c < 32 || c >= 127) return false; return true; }
bool lowerName(const std::string& text) {
    if (text.empty() || !ascii(text)) return false;
    for (char c : text) if (c >= 'A' && c <= 'Z') return false; return true;
}
struct Reader {
    const Bytes& bytes;
    void range(std::size_t at, std::size_t size) const {
        malformed(at <= bytes.size() && size <= bytes.size() - at, "parameter.truncated", at, "Read exceeds original parameter extent");
    }
    std::uint32_t u32(std::size_t at) const {
        range(at, 4); return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at + 1]) << 8) |
            (std::uint32_t(bytes[at + 2]) << 16) | (std::uint32_t(bytes[at + 3]) << 24);
    }
    std::string text(std::size_t at, std::size_t size, bool padded = false) const {
        malformed(size >= 1 && size <= 512, "parameter.string_extent", at, "String extent outside1..512"); range(at, size);
        std::size_t count = 0;
        while (count < size && bytes[at + count]) {
            malformed(bytes[at + count] >= 32 && bytes[at + count] < 127, "parameter.string_ascii", at + count, "String is not printable ASCII"); ++count;
        }
        malformed(count < size && (padded || count + 1 == size), "parameter.string_terminator", at, "String terminator or padding is invalid");
        for (std::size_t i = count; i < size; ++i) malformed(bytes[at + i] == 0, "parameter.string_suffix", at + i, "Nonzero string suffix");
        return std::string(bytes.begin() + at, bytes.begin() + at + count);
    }
};
void put32(Bytes& bytes, std::size_t at, std::uint32_t n) {
    for (unsigned i = 0; i < 4; ++i) bytes[at + i] = static_cast<std::uint8_t>(n >> (8 * i));
}
void append32(Bytes& bytes, std::uint32_t n) { const auto at = bytes.size(); bytes.resize(at + 4); put32(bytes, at, n); }
Bytes encode(const ParameterDocument48F& p) {
    require(p.version == 1, DecodeKind48C::unsupported, "parameter.version", 7, "Only BINARY version1 supported");
    malformed(!p.blocks.empty() && p.blocks.size() <= 8192, "parameter.block_count", 11, "Block count outside1..8192");
    Bytes bytes{'B','I','N','A','R','Y',0}; append32(bytes, 1); append32(bytes, static_cast<std::uint32_t>(p.blocks.size()));
    std::string previous;
    for (const auto& block : p.blocks) {
        malformed(lowerName(block.name) && block.name.size() < 512 && block.name > previous,
            "parameter.block_names", 0, "Block names must be unique lowercase sorted ASCII"); previous = block.name;
        append32(bytes, static_cast<std::uint32_t>(block.name.size() + 1)); bytes.insert(bytes.end(), block.name.begin(), block.name.end()); bytes.push_back(0);
    }
    const auto table = bytes.size(); bytes.resize(table + p.blocks.size() * 8);
    for (std::size_t i = 0; i < p.blocks.size(); ++i) {
        const auto& block = p.blocks[i]; const auto count = block.fields.size();
        malformed(count >= 1 && count <= 1024, "parameter.field_count", 0, "Field count outside1..1024");
        const auto start = bytes.size(); append32(bytes, static_cast<std::uint32_t>(count)); const auto base = bytes.size(); bytes.resize(base + count * 43);
        std::vector<std::size_t> order; order.reserve(count); previous.clear();
        for (std::size_t j = 0; j < count; ++j) {
            const auto& field = block.fields[j];
            malformed(lowerName(field.name) && field.name.size() < 35 &&
                (p.preservesDuplicateLabels ? field.name >= previous : field.name > previous), "parameter.field_names", 0,
                "Field names must be unique lowercase sorted ASCII"); previous = field.name;
            malformed(field.value.size() < 512 && ascii(field.value), "parameter.value", 0, "Value is not bounded printable ASCII"); order.push_back(j);
        }
        std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) { return block.fields[a].value_offset < block.fields[b].value_offset; });
        for (std::size_t k = 0; k < order.size(); ++k) {
            const auto j = order[k]; const auto& field = block.fields[j];
            malformed(k == 0 || field.value_offset != block.fields[order[k - 1]].value_offset, "parameter.value_order", 0, "Ambiguous physical value order");
            const auto entry = base + j * 43; std::copy(field.name.begin(), field.name.end(), bytes.begin() + entry);
            put32(bytes, entry + 35, static_cast<std::uint32_t>(bytes.size() - base)); put32(bytes, entry + 39, static_cast<std::uint32_t>(field.value.size() + 1));
            bytes.insert(bytes.end(), field.value.begin(), field.value.end()); bytes.push_back(0);
            require(bytes.size() <= 2u * 1024u * 1024u, DecodeKind48C::limit, "parameter.output_limit", 0, "Encoded parameter exceeds2 MiB");
        }
        put32(bytes, table + i * 8, static_cast<std::uint32_t>(start)); put32(bytes, table + i * 8 + 4, static_cast<std::uint32_t>(bytes.size() - start - 4));
    }
    return bytes;
}
void allocation(DecodeError48C& error) { error = {DecodeKind48C::limit, "world.allocation", 0, "Allocation failed inside bounded decoder"}; }
std::uint32_t findBlock(const ParameterDocument48F& p, const std::string& name) {
    const auto found = std::lower_bound(p.blocks.begin(), p.blocks.end(), name, [](const ParameterBlock48F& a, const std::string& b) { return a.name < b; });
    malformed(found != p.blocks.end() && found->name == name, "world.missing_block", 0, "Required scene block is missing");
    return static_cast<std::uint32_t>(found - p.blocks.begin());
}
const ParameterField48F& field(const ParameterBlock48F& block, const std::string& name) {
    const auto found = std::lower_bound(block.fields.begin(), block.fields.end(), name, [](const ParameterField48F& a, const std::string& b) { return a.name < b; });
    malformed(found != block.fields.end() && found->name == name, "world.missing_field", block.offset, "Required scene parameter is missing"); return *found;
}
std::uint32_t number(const ParameterField48F& f, std::uint32_t maximum) {
    malformed(!f.value.empty(), "world.integer_syntax", f.value_offset, "Empty unsigned decimal integer"); std::uint32_t n = 0;
    for (char c : f.value) {
        malformed(c >= '0' && c <= '9', "world.integer_syntax", f.value_offset, "Scene count/reference must be unsigned decimal");
        require(n <= maximum / 10 && unsigned(c - '0') <= maximum - n * 10, DecodeKind48C::limit,
            "world.integer_limit", f.value_offset, "Scene count/reference exceeds bound"); n = n * 10 + unsigned(c - '0');
    }
    return n;
}
double decimal(std::string s, std::size_t at) {
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
Vec vector(const ParameterField48F& f) {
    Vec v{}; std::size_t begin = 0;
    for (unsigned i = 0; i < 3; ++i) {
        auto end = f.value.find(',', begin);
        malformed((i == 2) == (end == std::string::npos), "world.vector_syntax", f.value_offset, "Scene vector requires three components");
        if (end == std::string::npos) end = f.value.size(); v[i] = decimal(f.value.substr(begin, end - begin), f.value_offset + begin); begin = end + 1;
    }
    return v;
}
Vec cross(const Vec& a, const Vec& b) { return {{a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]}}; }
Vec normalize(const Vec& v, std::size_t at) {
    const double square = (v[0]*v[0]+v[1]*v[1])+v[2]*v[2];
    malformed(std::isfinite(square) && square > 1e-20, "world.orientation", at, "Degenerate or nonfinite orientation");
    const double length = std::sqrt(square); return {{v[0]/length, v[1]/length, v[2]/length}};
}
std::array<double,16> placement(const Vec& position,const Vec& look0,const Vec& up0,std::size_t at){
    const auto right0=cross(up0,look0),look1=cross(right0,up0),up=normalize(up0,at),look=normalize(look1,at),right=cross(up,look);
    std::array<double,16> matrix{};for(unsigned k=0;k<3;++k){matrix[k]=right[k];matrix[4+k]=up[k];matrix[8+k]=look[k];matrix[12+k]=position[k];}matrix[15]=1;return matrix;
}
template<std::size_t N>bool same(const std::array<double,N>& a,const std::array<double,N>& b){return std::memcmp(a.data(),b.data(),N*sizeof(double))==0;}
bool modelName(const std::string& text) {
    if (text.compare(0, 4, "zone") != 0) return false; std::size_t i = 4, begin = i;
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') ++i;
    if (i == begin || text.compare(i, 6, "-model") != 0) return false; i += 6; begin = i;
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') ++i; return i > begin && i == text.size();
}
}

bool encodeParameters48F(const ParameterDocument48F& parameters, Bytes& output, DecodeError48C& error) {
    error = {}; try { auto bytes = encode(parameters); output = std::move(bytes); return true; }
    catch (const Failure& f) { error = f.error; } catch (const std::bad_alloc&) { allocation(error); } return false;
}
bool decodeParameters48F(const Bytes& input, ParameterDocument48F& output, DecodeError48C& error,
    const DecodeHooks48C* hooks, const WorldLimits48F& limits) {
    error = {};
    try {
        checkCancel(hooks, 0); require(input.size() <= std::min(limits.input_bytes, std::size_t(2u*1024u*1024u)), DecodeKind48C::limit,
            "parameter.input_limit", 0, "Parameter input exceeds byte cap"); Reader r{input}; r.range(0, 15);
        const char magic[] = "BINARY"; require(std::memcmp(input.data(), magic, 7) == 0, DecodeKind48C::unsupported,
            "parameter.magic", 0, "Only BINARY labelled-block parameters supported");
        require(r.u32(7) == 1, DecodeKind48C::unsupported, "parameter.version", 7, "Only BINARY version1 supported");
        const auto count = r.u32(11); malformed(count > 0, "parameter.block_count", 11, "Empty block list");
        require(count <= std::min(limits.blocks, 8192u), DecodeKind48C::limit, "parameter.block_count", 11, "Block count cap");
        ParameterDocument48F parsed; parsed.preservesDuplicateLabels=limits.retainDuplicateLabels; std::size_t at = 15, duplicateOffset = 0; std::string previous;
        for (std::uint32_t i = 0; i < count; ++i) {
            const auto length = r.u32(at); at += 4; ParameterBlock48F block; block.name_offset = at; block.name = r.text(at, length); at += length;
            malformed(lowerName(block.name) && block.name > previous, "parameter.block_names", block.name_offset, "Block names must be unique lowercase sorted ASCII");
            previous = block.name; parsed.blocks.push_back(std::move(block));
        }
        parsed.header_table_offset = at; r.range(at, std::size_t(count)*8); std::size_t physicalNext = at + std::size_t(count)*8;
        progress(hooks, "parameter.blocks", 0, count);
        for (std::uint32_t i = 0; i < count; ++i) {
            checkCancel(hooks, physicalNext); auto& block = parsed.blocks[i]; block.table_entry_offset = at + std::size_t(i)*8;
            block.offset = r.u32(block.table_entry_offset); block.size_without_count = r.u32(block.table_entry_offset + 4);
            malformed(block.offset == physicalNext, "parameter.block_coverage", block.table_entry_offset, "Block coverage gap/overlap or unproved data-line variant");
            r.range(block.offset, 4); r.range(block.offset + 4, block.size_without_count);
            const auto number = r.u32(block.offset); malformed(number > 0, "parameter.field_count", block.offset, "Empty label table");
            require(number <= std::min(limits.fields_per_block, 1024u), DecodeKind48C::limit, "parameter.field_count", block.offset, "Field count cap");
            malformed(std::size_t(number)*43 <= block.size_without_count, "parameter.field_table", block.offset, "Label table exceeds block");
            const auto base = block.offset + 4, end = base + block.size_without_count; previous.clear(); std::vector<std::pair<std::size_t,std::size_t>> coverage;
            for (std::uint32_t j = 0; j < number; ++j) {
                ParameterField48F f; f.entry_offset = base + std::size_t(j)*43; f.name = r.text(f.entry_offset, 35, true);
                malformed(lowerName(f.name) && f.name >= previous, "parameter.field_names", f.entry_offset, "Label names must be lowercase sorted ASCII");
                if(f.name==previous&&!duplicateOffset)duplicateOffset=f.entry_offset;previous = f.name;
                const auto offset = r.u32(f.entry_offset + 35); f.value_bytes = r.u32(f.entry_offset + 39);
                malformed(offset >= number*43 && offset <= block.size_without_count && f.value_bytes <= block.size_without_count-offset,
                    "parameter.value_extent", f.entry_offset + 35, "Value aliases table or exceeds block");
                f.value_offset = base + offset; f.value = r.text(f.value_offset, f.value_bytes); coverage.push_back({f.value_offset,f.value_offset+f.value_bytes}); block.fields.push_back(std::move(f));
            }
            std::sort(coverage.begin(), coverage.end()); std::size_t next = base + std::size_t(number)*43;
            for (const auto& c : coverage) { malformed(c.first == next, "parameter.value_coverage", c.first, "Value overlap or gap"); next = c.second; }
            malformed(next == end, "parameter.block_suffix", next, "Unconsumed block suffix"); physicalNext = end;
            progress(hooks, "parameter.blocks", i+1, count);
        }
        malformed(physicalNext == input.size(), "parameter.file_suffix", physicalNext, "Unconsumed file suffix");
        require(!duplicateOffset||limits.retainDuplicateLabels,DecodeKind48C::unsupported,"parameter.duplicate_label",duplicateOffset,
            "Complete original labelled block repeats keys; precedence semantics are unproved");
        malformed(encode(parsed) == input, "parameter.roundtrip", 0, "Reversible parameter serialization differs");
        checkCancel(hooks, physicalNext); parsed.original_bytes = input; output = std::move(parsed); return true;
    } catch (const Failure& f) { error = f.error; } catch (const std::bad_alloc&) { allocation(error); } return false;
}
bool decodeWorld48F(const Bytes& input, World48F& output, DecodeError48C& error,
    const DecodeHooks48C* hooks, const WorldLimits48F& limits) {
    error = {};
    try {
        require(!limits.retainDuplicateLabels,DecodeKind48C::unsupported,"world.duplicate_label_policy",0,
            "World semantic decoding requires unambiguous field names");
        World48F world; if (!decodeParameters48F(input, world.parameters, error, hooks, limits)) return false;
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
                instance.position=vector(field(b,"position"));instance.look=vector(field(b,"lookvector"));instance.up=vector(field(b,"upvector"));
                const auto& lighting=field(b,"uselighting");require(lighting.value=="T"||lighting.value=="F",DecodeKind48C::unsupported,"world.lighting_flag",lighting.value_offset,"Only original T/F flags supported");instance.use_lighting=lighting.value=="T";
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
bool validateWorldSemantics48F(const World48F& w,DecodeError48C& error){
    error={};try{const auto& p=w.parameters;const auto info=findBlock(p,"sceneinfo");malformed(info==w.scene_info_block,"world.cache_sceneinfo",0,"Scene-info index disagrees");
        malformed(number(field(p.blocks[info],"numzones"),256)==w.zones.size()&&number(field(p.blocks[info],"numportals"),4096)==w.portals.size(),"world.cache_counts",0,"Typed counts disagree with retained fields");
        for(const auto& z:w.zones){const auto index=findBlock(p,"zone"+std::to_string(z.zone));malformed(index==z.parameter_block&&number(field(p.blocks[index],"numberofstaticmodels"),10000)==z.models,"world.cache_zone",0,"Zone fields disagree");}
        std::set<std::string> used;
        for(const auto& x:w.instances){const auto index=findBlock(p,x.section);const auto& b=p.blocks[index];malformed(index==x.parameter_block,"world.cache_instance",0,"Instance block disagrees");
            const auto pos=vector(field(b,"position")),look=vector(field(b,"lookvector")),up=vector(field(b,"upvector"));
            malformed(same(pos,x.position)&&same(look,x.look)&&same(up,x.up),"world.cache_vectors",b.offset,"Typed vectors differ from retained decimal source");
            malformed(same(placement(pos,look,up,b.offset),x.matrix),"world.cache_matrix",b.offset,"Typed matrix differs from original placement operations");
            auto name=field(b,"slt").value;for(char& c:name)if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
            malformed(name.size()>4&&name.substr(name.size()-4)==".slt"&&x.geometry_member=="geometry\\"+name.substr(0,name.size()-4)+".psg"&&x.collision_member=="geometry\\"+name.substr(0,name.size()-4)+".col","world.cache_reference",b.offset,"Typed references differ from SLT field");
            const auto& flag=field(b,"uselighting");malformed((flag.value=="T"||flag.value=="F")&&x.use_lighting==(flag.value=="T"),"world.cache_lighting",flag.value_offset,"Typed lighting flag disagrees");used.insert(x.section);}
        for(const auto& b:p.blocks)malformed(!modelName(b.name)||used.count(b.name)!=0,"world.cache_model_headers",b.offset,"Model header outside typed instances");
        for(const auto& portal:w.portals){const auto index=findBlock(p,portal.section);const auto& b=p.blocks[index];malformed(index==portal.parameter_block,"world.cache_portal",0,"Portal block disagrees");
            for(unsigned i=0;i<2;++i)malformed(number(field(b,"connect"+std::to_string(i+1)),static_cast<std::uint32_t>(w.zones.size()))==portal.zones[i],"world.cache_portal_zones",b.offset,"Portal zone differs from original");
            for(unsigned i=0;i<4;++i)malformed(same(vector(field(b,"vertex"+std::to_string(i+1))),portal.vertices[i]),"world.cache_portal_vertex",b.offset,"Portal vertex differs from original");}
        return true;
    }catch(const Failure& f){error=f.error;}catch(const std::bad_alloc&){allocation(error);}return false;
}
} }
