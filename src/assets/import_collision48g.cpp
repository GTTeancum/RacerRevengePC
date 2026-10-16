#include "import_collision48g.h"
#include <algorithm>
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace rr { namespace assets {
namespace {
using Bytes = std::vector<std::uint8_t>;
struct Failure { DecodeKind48C kind; const char* code; std::size_t offset; const char* detail; };
void need(bool ok, const char* code, std::size_t offset = 0, const char* detail = "Invalid collision data") {
    if (!ok) throw Failure{DecodeKind48C::malformed, code, offset, detail};
}
void supported(bool ok, const char* code, std::size_t offset, const char* detail) {
    if (!ok) throw Failure{DecodeKind48C::unsupported, code, offset, detail};
}
void limit(bool ok, const char* code) {
    if (!ok) throw Failure{DecodeKind48C::limit, code, 0, "Collision caller or hard resource limit"};
}
void tick(const DecodeHooks48C* h, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (h && h->progress) h->progress(h->context, stage, done, total);
    if (h && h->cancelled && h->cancelled(h->context))
        throw Failure{DecodeKind48C::cancelled, "col.cancelled", 0, "Collision conversion cancelled"};
}
struct Reader {
    const Bytes& b; std::size_t at = 0;
    void range(std::size_t n) const { need(at <= b.size() && n <= b.size()-at, "col.truncated", at); }
    std::uint32_t u32() { range(4); std::uint32_t v=0; for(unsigned j=0;j<4;++j)v|=std::uint32_t(b[at+j])<<(8*j);at+=4;return v; }
    std::uint16_t u16() { range(2); const auto v=std::uint16_t(b[at])|(std::uint16_t(b[at+1])<<8);at+=2;return static_cast<std::uint16_t>(v); }
    float rawFloat() { const auto bits=u32();float f;std::memcpy(&f,&bits,4);return f; }
    float f32() { const auto f=rawFloat();need(std::isfinite(f),"col.nonfinite",at-4);return f; }
    template<std::size_t N> void values(std::array<float,N>& a) { for(auto& x:a)x=f32(); }
};
struct Writer {
    Bytes b;
    void u32(std::size_t n) { need(n<=0xffffffffu,"col.integer_range");for(unsigned j=0;j<4;++j)b.push_back(static_cast<std::uint8_t>(n>>(j*8))); }
    void u16(std::uint32_t n) { need(n<=65535,"col.short_index_range");b.push_back(static_cast<std::uint8_t>(n));b.push_back(static_cast<std::uint8_t>(n>>8)); }
    void rawFloat(float f) { std::uint32_t bits;std::memcpy(&bits,&f,4);u32(bits); }
    void f32(float f) { need(std::isfinite(f),"col.nonfinite");rawFloat(f); }
    template<std::size_t N> void values(const std::array<float,N>& a) { for(auto x:a)f32(x); }
};
std::uint32_t originalChild(std::uint32_t child) {
    need((child & 0x7fffffffu)<32768,"col.child_range");
    return (child & 32767u) | ((child & collision_leaf48g)?32768u:0u);
}
std::uint32_t floatBits(float value) { std::uint32_t bits;std::memcpy(&bits,&value,4);return bits; }
// Original position prefixes: 106ED0/type3, 107AEC/type2 SAT,
// 108414/type2 leaf and 10B8DC/triangle. Signed16 -> CVT.S.W ->
// MUL.S by 0x38000100 -> MUL.S by original ScaleFactor at 383640.
// Keep both roundings; a direct integer-to-float cast is not the source path.
// Cold expansion is shared by all live readers, which then enter the same
// float-coordinate math as those original prefixes. No generated geometry.
float quantizedPosition(std::uint16_t word) {
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,
        "Original collision projection requires IEEE binary32");
    const std::int32_t signedValue=word<32768u?std::int32_t(word):std::int32_t(word)-65536;
    const std::uint32_t reciprocalBits=0x38000100u;
    float reciprocal;std::memcpy(&reciprocal,&reciprocalBits,4);
    volatile float converted=static_cast<float>(signedValue);
    volatile float normalized=reciprocal*converted;
    volatile float expanded=32767.0f*normalized; // Original scale is 32767.
    return expanded;
}
std::size_t validate(const Collision48G& c, bool inspectOriginalDegenerate = false, std::size_t vertexBase = 84) {
    need(c.version==1 && c.type==0 && c.quantized<=1,"col.typed_variant");
    need(c.reverse_mode<=1 && c.normals_present<=1,"col.boolean_flag");
    supported(!c.quantized||!c.normals_present,"col.quantized_normals",80,
        "Quantized optional vertex normals remain outside this bounded source projection");
    supported(!c.quantized||std::fegetround()==FE_TONEAREST,"col.quantized_rounding",80,
        "Quantized position projection requires individually rounded binary32 nearest-even");
    const auto nv=c.vertices.size(), nn=c.nodes.size(), nf=c.faces.size();
    need(nv>0 && nv<=65535 && nn>0 && nn<=32767 && nf>1 && nf<=32768 && nn+1==nf,"col.tree_counts");
    need(c.vertex_normals.size()==(c.normals_present?nv:0),"col.normal_count");
    need(c.matrix[3]==0 && c.matrix[7]==0 && c.matrix[11]==0 && c.matrix[15]==1,"col.affine_matrix");
    for(float x:c.matrix)need(std::isfinite(x),"col.matrix_nonfinite");
    const std::size_t stride=c.quantized?6u:12u;
    need(c.vertices_offset==vertexBase && c.normals_flag_offset==vertexBase+stride*nv &&
         c.nodes_offset==vertexBase+4+stride*nv*(1+c.normals_present) && c.faces_offset==c.nodes_offset+32*nn,"col.source_offsets");
    for(const auto& v:c.vertices)for(float x:v)need(std::isfinite(x),"col.vertex_nonfinite");
    for(const auto& v:c.vertex_normals)for(float x:v)need(std::isfinite(x),"col.normal_nonfinite");
    for(std::size_t i=0;i<nn;++i) {
        const auto& n=c.nodes[i];need(n.source_offset==c.nodes_offset+32*i,"col.node_offset");
        need(std::isfinite(n.volume)&&n.volume>=0,"col.node_volume",n.source_offset);
        for(unsigned j=0;j<3;++j)need(std::isfinite(n.center[j])&&std::isfinite(n.half[j])&&n.half[j]>=0,"col.node_bounds",n.source_offset);
        for(auto child:n.children) {
            if(child & collision_leaf48g)need((child & 0x7fffffffu)<nf,"col.leaf_link",n.source_offset+28);
            else need(child<nn,"col.node_link",n.source_offset+28);
        }
    }
    std::size_t firstDegenerate = 0;
    for(std::size_t i=0;i<nf;++i) {
        const auto& f=c.faces[i];need(f.source_offset==c.faces_offset+22*i,"col.face_offset");
        for(auto vertex:f.vertices)need(vertex<nv,"col.face_vertex",f.source_offset+16);
        const bool finite=std::isfinite(f.d)&&std::isfinite(f.normal[0])&&std::isfinite(f.normal[1])&&std::isfinite(f.normal[2]);
        if(!finite) {
            // Nine original disc members contain this exact exporter degeneracy. It is
            // retained exactly only with coincident finite positions. Runtime packing
            // follows the original signed conversion; no face or tree link is removed.
            const auto& a=c.vertices[f.vertices[0]];const auto& b=c.vertices[f.vertices[1]];const auto& d=c.vertices[f.vertices[2]];
            const bool authored=inspectOriginalDegenerate&&!c.quantized&&floatBits(f.d)==0x7fc00000u&&
                floatBits(f.normal[0])==0xffc00000u&&floatBits(f.normal[1])==0xffc00000u&&floatBits(f.normal[2])==0xffc00000u&&
                (a==b||a==d||b==d);
            need(authored,"col.plane_nonfinite",f.source_offset,"Nonfinite face plane outside the evidenced coincident-vertex source variant");
            if(!firstDegenerate)firstDegenerate=f.source_offset;
        }
    }
    // Full-tree walk detects cycles, shared subtrees, unused records and multiply referenced leaves.
    std::vector<std::uint8_t> visited(nn+nf,0);std::vector<std::uint32_t> stack{0};std::size_t count=0;
    while(!stack.empty()) {
        const auto key=stack.back();stack.pop_back();const bool leaf=(key & collision_leaf48g)!=0;
        const std::size_t index=leaf?nn+(key&0x7fffffffu):key;
        need(index<visited.size() && !visited[index],"col.tree_shared_or_cycle");visited[index]=1;++count;
        if(!leaf){stack.push_back(c.nodes[key].children[1]);stack.push_back(c.nodes[key].children[0]);}
    }
    need(count==nn+nf,"col.tree_unreachable");
    return firstDegenerate;
}
Bytes original(const Collision48G& c) {
    validate(c,true);Writer w;w.u32(1234);w.u32(0x006c6f63);w.u32(1);w.u32(0);
    for(unsigned row=0;row<4;++row)for(unsigned col=0;col<3;++col)w.f32(c.matrix[row*4+col]);
    w.u32(c.reverse_mode);w.u32(c.vertices.size());w.u32(c.nodes.size());w.u32(c.faces.size());w.u32(c.quantized);
    if(c.quantized){
        // Retain actual original int16 words, not a reverse-quantized estimate.
        // Every projected typed coordinate must still match that exact source.
        Reader raw{c.original_bytes,c.vertices_offset};
        for(const auto& v:c.vertices)for(float x:v){const auto word=raw.u16();
            need(floatBits(x)==floatBits(quantizedPosition(word)),"col.quantized_provenance",raw.at-2,
                "Expanded collision coordinate differs from original signed-short source");w.u16(word);}
    }else for(const auto& v:c.vertices)w.values(v);
    w.u32(c.normals_present);for(const auto& v:c.vertex_normals)w.values(v);
    for(const auto& n:c.nodes){w.f32(n.volume);w.values(n.center);w.values(n.half);for(auto x:n.children)w.u16(originalChild(x));}
    for(const auto& f:c.faces){w.rawFloat(f.d);for(auto n:f.normal)w.rawFloat(n);for(auto x:f.vertices)w.u16(x);}
    need(w.b.size()<=8u*1024u*1024u,"col.original_size");
    need(w.b==c.original_bytes,"col.provenance_mismatch",0,"Typed collision records do not match complete original bytes");
    return std::move(w.b);
}
// Headerless tree body reader shared by original125890 callers. Every source
// offset is absolute in the actual containing file; no stand-alone header is invented.
void decodeBody(Reader& r, Collision48G& c, const DecodeHooks48C* hooks,
                const CollisionLimits48G& limits, bool wholeFile=false) {
    const auto bodyBase=r.at,vertexBase=r.at+68;
    for(unsigned row=0;row<4;++row){for(unsigned col=0;col<3;++col)c.matrix[row*4+col]=r.f32();c.matrix[row*4+3]=row==3?1.0f:0.0f;}
    c.reverse_mode=r.u32();const auto nv=r.u32(),nn=r.u32(),nf=r.u32();c.quantized=r.u32();
    need(c.reverse_mode<=1&&c.quantized<=1,"col.boolean_flag",bodyBase+48);
    need(nv>0&&nv<=65535&&nn<=32767&&nf>0&&nf<=32768,"col.counts",bodyBase+52);
    need(nn+1==nf,"col.tree_counts",bodyBase+56);
    limit(nv<=limits.vertices&&nn<=limits.nodes&&nf<=limits.faces,"col.count_limit");
    const std::size_t stride=c.quantized?6:12;const std::size_t flag=vertexBase+stride*nv;
    r.range(stride*nv+4);r.at=flag;c.normals_present=r.u32();need(c.normals_present<=1,"col.normals_flag",flag);
    const std::size_t expected=flag+4+stride*nv*c.normals_present+32u*nn+22u*nf;
    need(wholeFile?expected==r.b.size():expected<=r.b.size(),"col.exact_extent",r.at,"Truncated COL body or trailing bytes");
    supported(!c.quantized||!c.normals_present,"col.quantized_normals",flag,
        "Quantized optional vertex normals remain outside this bounded source projection");
    supported(!c.quantized||std::fegetround()==FE_TONEAREST,"col.quantized_rounding",bodyBase+64,
        "Quantized position projection requires individually rounded binary32 nearest-even");
    supported(nn>0,"col.single_leaf",bodyBase+56,"No-node single-face root query policy is not yet supported");
    c.vertices_offset=static_cast<std::uint32_t>(vertexBase);c.normals_flag_offset=static_cast<std::uint32_t>(flag);
    r.at=vertexBase;c.vertices.resize(nv);
    for(std::uint32_t i=0;i<nv;++i){
        if(c.quantized)for(auto& x:c.vertices[i])x=quantizedPosition(r.u16());
        else r.values(c.vertices[i]);
        if((i&255u)==0)tick(hooks,"collision-vertices",i,nv);}
    r.u32();if(c.normals_present){c.vertex_normals.resize(nv);for(auto& v:c.vertex_normals)r.values(v);}
    c.nodes_offset=static_cast<std::uint32_t>(r.at);c.nodes.resize(nn);
    for(std::uint32_t i=0;i<nn;++i){auto& n=c.nodes[i];n.source_offset=static_cast<std::uint32_t>(r.at);n.volume=r.f32();r.values(n.center);r.values(n.half);
        for(auto& key:n.children){const auto raw=r.u16();key=(raw&32767u)|((raw&32768u)?collision_leaf48g:0u);}
        if((i&255u)==0)tick(hooks,"collision-nodes",i,nn);}
    c.faces_offset=static_cast<std::uint32_t>(r.at);c.faces.resize(nf);
    for(std::uint32_t i=0;i<nf;++i){auto& f=c.faces[i];f.source_offset=static_cast<std::uint32_t>(r.at);f.d=r.rawFloat();for(auto& n:f.normal)n=r.rawFloat();for(auto& v:f.vertices)v=r.u16();if((i&255u)==0)tick(hooks,"collision-faces",i,nf);}
    need(r.at==expected,"col.exact_extent",r.at);validate(c,true,vertexBase);
}
std::string message(const Failure& f) { return std::string(f.code)+" at "+std::to_string(f.offset)+": "+f.detail; }
bool matrixFinite(const std::array<double,16>& m) { for(double d:m)if(!std::isfinite(d))return false;return m[3]==0&&m[7]==0&&m[11]==0&&m[15]==1; }
}

bool decodeCollision48G(const Bytes& input, Collision48G& output, DecodeError48C& error,
                       const DecodeHooks48C* hooks, const CollisionLimits48G& limits) {
    error={};try {
        tick(hooks,"collision-header",0,1);limit(input.size()<=std::min<std::size_t>(limits.input_bytes,8u*1024u*1024u),"col.input_limit");
        Reader r{input};need(r.u32()==1234 && r.u32()==0x006c6f63,"col.magic");
        Collision48G c;c.version=r.u32();c.type=r.u32();
        supported(c.version==1,"col.version",8,"Only original COL version 1 is supported");
        supported(c.type==0,"col.type",12,"Only original type 0 triangle trees are supported");
        decodeBody(r,c,hooks,limits,true);
        need(r.at==input.size(),"col.exact_extent",r.at,"Truncated COL body or trailing bytes");
        c.original_bytes=input;
        tick(hooks,"collision-complete",1,1);output=std::move(c);return true;
    } catch(const Failure& f) {error={f.kind,f.code,f.offset,f.detail};}
      catch(const std::bad_alloc&) {error={DecodeKind48C::limit,"col.allocation",0,"Collision allocation failed"};}
    return false;
}

bool decodeCollisionCompound50IJ(const Bytes& input, CollisionCompound50IJ& output,
    DecodeError48C& error, const DecodeHooks48C* hooks, const CollisionCompoundLimits50IJ& limits) {
    error={};try {
        tick(hooks,"compound-collision-header",0,1);
        limit(input.size()<=std::min<std::size_t>(limits.tree.input_bytes,8u*1024u*1024u),"col.input_limit");
        Reader r{input};need(r.u32()==1234&&r.u32()==0x006c6f63,"col.magic");
        CollisionCompound50IJ c;c.version=r.u32();c.type=r.u32();
        supported(c.version==1,"col.version",8,"Only original COL version 1 is supported");
        supported(c.type==1,"col.type",12,"Compound reader requires original shape type 1");
        const auto count=r.u32();c.reverse_mode=r.u32();
        need(c.reverse_mode<=1,"col.boolean_flag",20);
        limit(count<=limits.members&&count<=1024,"col.compound_member_limit");
        // Minimum body extent bounds count before reserving. Exact per-tree extents
        // and rooted topology are checked by the same reader as static COL0.
        need(count<=(input.size()-r.at)/76,"col.compound_count_extent",16);
        c.members.reserve(count);
        for(std::uint32_t i=0;i<count;++i){
            CollisionCompoundMember50IJ member;
            member.source_offset=static_cast<std::uint32_t>(r.at);
            member.scene_node_index=r.u32();
            member.body_offset=static_cast<std::uint32_t>(r.at);
            decodeBody(r,member.tree,hooks,limits.tree);
            member.source_end=static_cast<std::uint32_t>(r.at);
            c.members.push_back(std::move(member));
            tick(hooks,"compound-collision-members",i+1,count);
        }
        need(r.at==input.size(),"col.exact_extent",r.at,"Compound COL trailing bytes");
        c.original_bytes=input;
        tick(hooks,"compound-collision-complete",1,1);output=std::move(c);return true;
    }catch(const Failure& f){error={f.kind,f.code,f.offset,f.detail};}
     catch(const std::bad_alloc&){error={DecodeKind48C::limit,"col.allocation",0,"Compound collision allocation failed"};}
    return false;
}

bool encodeOriginalCollision48G(const Collision48G& c, Bytes& output, std::string& error) {
    error.clear();try{auto b=original(c);output=std::move(b);return true;}
    catch(const Failure& f){error=message(f);}catch(const std::bad_alloc&){error="col.allocation";}return false;
}
bool encodeCollisionAsset48G(const Collision48G& c, Bytes& output, std::string& error) {
    error.clear();try {
        original(c);Writer w;for(char ch:std::string("RRCOL001"))w.b.push_back(static_cast<std::uint8_t>(ch));w.u32(1);w.u32(0);
        w.u32(c.version);w.u32(c.type);w.u32(c.reverse_mode);w.u32(c.quantized);w.u32(c.normals_present);
        w.u32(c.vertices.size());w.u32(c.nodes.size());w.u32(c.faces.size());w.u32(c.original_bytes.size());
        w.u32(c.vertices_offset);w.u32(c.normals_flag_offset);w.u32(c.nodes_offset);w.u32(c.faces_offset);w.values(c.matrix);
        for(const auto& v:c.vertices)w.values(v);for(const auto& v:c.vertex_normals)w.values(v);
        for(const auto& n:c.nodes){w.u32(n.source_offset);w.f32(n.volume);w.values(n.center);w.values(n.half);for(auto x:n.children)w.u32(x);}
        for(const auto& f:c.faces){w.u32(f.source_offset);w.rawFloat(f.d);for(auto n:f.normal)w.rawFloat(n);for(auto x:f.vertices)w.u32(x);}
        need(w.b.size()<=max_collision_asset_bytes48g-c.original_bytes.size(),"rrcol.size");
        w.b.insert(w.b.end(),c.original_bytes.begin(),c.original_bytes.end());
        const auto n=static_cast<std::uint32_t>(w.b.size());for(unsigned j=0;j<4;++j)w.b[12+j]=static_cast<std::uint8_t>(n>>(8*j));
        output=std::move(w.b);return true;
    }catch(const Failure& f){error=message(f);}catch(const std::bad_alloc&){error="rrcol.allocation";}return false;
}
bool parseCollisionAsset48G(const Bytes& input, Collision48G& output, std::string& error) {
    error.clear();try {
        need(input.size()>=132&&input.size()<=max_collision_asset_bytes48g,"rrcol.size");
        need(std::memcmp(input.data(),"RRCOL001",8)==0,"rrcol.magic");Reader r{input,8};need(r.u32()==1,"rrcol.version");need(r.u32()==input.size(),"rrcol.size");
        Collision48G c;c.version=r.u32();c.type=r.u32();c.reverse_mode=r.u32();c.quantized=r.u32();c.normals_present=r.u32();
        const auto nv=r.u32(),nn=r.u32(),nf=r.u32(),raw=r.u32();
        need(nv>0&&nv<=65535&&nn>0&&nn<=32767&&nf>1&&nf<=32768&&c.normals_present<=1,"rrcol.counts");
        need(raw<=8u*1024u*1024u,"rrcol.raw_limit");
        const std::size_t expected=132+12u*nv*(1+c.normals_present)+40u*nn+32u*nf+raw;
        need(expected==input.size(),"rrcol.exact_extent");
        c.vertices_offset=r.u32();c.normals_flag_offset=r.u32();c.nodes_offset=r.u32();c.faces_offset=r.u32();r.values(c.matrix);
        c.vertices.resize(nv);for(auto& v:c.vertices)r.values(v);if(c.normals_present){c.vertex_normals.resize(nv);for(auto& v:c.vertex_normals)r.values(v);}
        c.nodes.resize(nn);for(auto& n:c.nodes){n.source_offset=r.u32();n.volume=r.f32();r.values(n.center);r.values(n.half);for(auto& x:n.children)x=r.u32();}
        c.faces.resize(nf);for(auto& f:c.faces){f.source_offset=r.u32();f.d=r.rawFloat();for(auto& n:f.normal)n=r.rawFloat();for(auto& x:f.vertices)x=r.u32();}
        r.range(raw);c.original_bytes.assign(input.begin()+r.at,input.end());r.at+=raw;need(r.at==input.size(),"rrcol.exact_extent");
        original(c);output=std::move(c);return true;
    }catch(const Failure& f){error=message(f);}catch(const std::bad_alloc&){error="rrcol.allocation";}return false;
}
bool collisionPlacement48G(const std::array<double,3>& position, const std::array<double,3>& look,
                          const std::array<double,3>& up, std::array<double,16>& output, std::string& error) {
    error.clear();try {
        auto normalized=[](const std::array<double,3>& v){for(double d:v)need(std::isfinite(d),"col.placement_nonfinite");const double length=std::sqrt((v[0]*v[0]+v[1]*v[1])+v[2]*v[2]);need(std::isfinite(length)&&length>0,"col.placement_zero_axis");return std::array<double,3>{{v[0]/length,v[1]/length,v[2]/length}};};
        const auto u=normalized(up),l=normalized(look);
        const std::array<double,3> right{{u[1]*l[2]-u[2]*l[1],u[2]*l[0]-u[0]*l[2],u[0]*l[1]-u[1]*l[0]}};
        need((right[0]*right[0]+right[1]*right[1])+right[2]*right[2]>0,"col.placement_parallel_axes");
        std::array<double,16> m{{right[0],right[1],right[2],0,u[0],u[1],u[2],0,l[0],l[1],l[2],0,position[0],position[1],position[2],1}};
        need(matrixFinite(m),"col.placement_affine");output=m;return true;
    }catch(const Failure& f){error=message(f);}return false;
}
bool collisionWorldMatrix48G(const Collision48G& c, const std::array<double,16>& placement,
                             std::array<double,16>& output, std::string& error) {
    error.clear();try {
        need(matrixFinite(placement),"col.placement_affine");
        need(c.matrix[3]==0&&c.matrix[7]==0&&c.matrix[11]==0&&c.matrix[15]==1,"col.affine_matrix");
        for(float f:c.matrix)need(std::isfinite(f),"col.matrix_nonfinite");
        std::array<double,16> m{};
        for(unsigned i=0;i<4;++i)for(unsigned j=0;j<4;++j){double sum=double(c.matrix[i*4])*placement[j];sum+=double(c.matrix[i*4+1])*placement[4+j];sum+=double(c.matrix[i*4+2])*placement[8+j];sum+=double(c.matrix[i*4+3])*placement[12+j];m[i*4+j]=sum;}
        need(matrixFinite(m),"col.composition_nonfinite");output=m;return true;
    }catch(const Failure& f){error=message(f);}return false;
}
} }
