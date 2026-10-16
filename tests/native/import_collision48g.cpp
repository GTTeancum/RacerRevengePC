#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "import_collision48g.h"
#include "json.hpp"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace {
using namespace rr::assets;using Bytes=std::vector<std::uint8_t>;using Json=nlohmann::json;namespace fs=std::filesystem;
unsigned checks=0,failures=0;
void check(bool value,const char* label){++checks;if(!value){++failures;if(failures<=30)std::fprintf(stderr,"FAIL: %s\n",label);}}
Bytes read(const fs::path& p){const auto n=fs::file_size(p);if(n>64u*1024u*1024u)throw std::runtime_error("fixture cap");std::ifstream f(p,std::ios::binary);Bytes b(static_cast<std::size_t>(n));if(n)f.read(reinterpret_cast<char*>(b.data()),static_cast<std::streamsize>(n));if(!f)throw std::runtime_error("fixture read");return b;}
Json json(const fs::path& p){const auto b=read(p);return Json::parse(b.begin(),b.end());}
void write(const fs::path& p,const Bytes& b){fs::create_directories(p.parent_path());std::ofstream f(p,std::ios::binary);f.write(reinterpret_cast<const char*>(b.data()),static_cast<std::streamsize>(b.size()));if(!f)throw std::runtime_error("fixture output");}
std::string sha(const Bytes& b){BCRYPT_ALG_HANDLE a=nullptr;BCRYPT_HASH_HANDLE h=nullptr;if(BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)throw std::runtime_error("SHA provider");if(BCryptCreateHash(a,&h,nullptr,0,nullptr,0,0)<0){BCryptCloseAlgorithmProvider(a,0);throw std::runtime_error("SHA init");}std::array<unsigned char,32>d{};const auto s=BCryptHashData(h,const_cast<PUCHAR>(b.data()),static_cast<ULONG>(b.size()),0);const auto t=BCryptFinishHash(h,d.data(),32,0);BCryptDestroyHash(h);BCryptCloseAlgorithmProvider(a,0);if(s<0||t<0)throw std::runtime_error("SHA update");std::ostringstream o;for(auto x:d)o<<std::hex<<std::setfill('0')<<std::setw(2)<<unsigned(x);return o.str();}
template<class T,std::size_t N>bool exact(const std::array<T,N>& a,const Json& b){if(b.size()!=N)return false;for(std::size_t i=0;i<N;++i){const T v=b[i].get<T>();if(std::memcmp(&a[i],&v,sizeof(T)))return false;}return true;}
bool exactFloat(float a,const Json& b){const float v=b.get<float>();return std::memcmp(&a,&v,4)==0;}
void compare(const Collision48G& c,const Json& j){
    check(c.version==j.at("version")&&c.type==j.at("type")&&c.reverse_mode==j.at("reverse_mode")&&c.quantized==j.at("quantized")&&c.normals_present==j.at("normals_present"),"all original header fields");
    check(exact(c.matrix,j.at("matrix")),"exact affine source matrix including signed zero");
    check(c.vertices_offset==j.at("vertices_offset")&&c.normals_flag_offset==j.at("normals_flag_offset")&&c.nodes_offset==j.at("nodes_offset")&&c.faces_offset==j.at("faces_offset"),"exact source array offsets");
    check(c.vertices.size()==j.at("vertices").size()&&c.vertex_normals.size()==j.at("vertex_normals").size()&&c.nodes.size()==j.at("nodes").size()&&c.faces.size()==j.at("faces").size(),"all source record counts");
    for(std::size_t i=0;i<c.vertices.size();++i)check(exact(c.vertices[i],j.at("vertices")[i]),"source vertex exact");
    for(std::size_t i=0;i<c.vertex_normals.size();++i)check(exact(c.vertex_normals[i],j.at("vertex_normals")[i]),"optional source normal exact");
    for(std::size_t i=0;i<c.nodes.size();++i){const auto& n=c.nodes[i];const auto& x=j.at("nodes")[i];check(exactFloat(n.volume,x.at("volume"))&&exact(n.center,x.at("center"))&&exact(n.half,x.at("half"))&&exact(n.children,x.at("children"))&&n.source_offset==x.at("source_offset"),"source node AABB and both branch tags exact");}
    for(std::size_t i=0;i<c.faces.size();++i){const auto& f=c.faces[i];const auto& x=j.at("faces")[i];check(exactFloat(f.d,x.at("d"))&&exact(f.normal,x.at("normal"))&&exact(f.vertices,x.at("vertices"))&&f.source_offset==x.at("source_offset"),"source plane and triangle indices exact");}
}
void put32(Bytes& b,std::size_t at,std::uint32_t n){for(unsigned j=0;j<4;++j)b.at(at+j)=static_cast<std::uint8_t>(n>>(8*j));}
void put16(Bytes& b,std::size_t at,std::uint16_t n){b.at(at)=static_cast<std::uint8_t>(n);b.at(at+1)=static_cast<std::uint8_t>(n>>8);}
std::uint32_t get32(const Bytes& b,std::size_t at){std::uint32_t n=0;for(unsigned j=0;j<4;++j)n|=std::uint32_t(b.at(at+j))<<(8*j);return n;}
Collision48G sentinel(){Collision48G c;c.type=999;c.original_bytes={7,8,9};return c;}
bool unchanged(const Collision48G& c){return c.type==999&&c.original_bytes==Bytes({7,8,9})&&c.nodes.empty();}
struct Hook{unsigned calls=0,stop=1;bool cancel=false;};
bool cancel(void* p){return static_cast<Hook*>(p)->cancel;}
void progress(void* p,const char*,std::uint32_t,std::uint32_t){auto& h=*static_cast<Hook*>(p);if(++h.calls>=h.stop)h.cancel=true;}
void malformed(const Bytes& original){
    Collision48G base;DecodeError48C e;std::string error;check(decodeCollision48G(original,base,e),"mutation base decode");
    std::set<std::size_t> cuts{0,3,7,8,11,12,15,16,63,64,67,68,71,72,75,76,79,80,83,84,base.normals_flag_offset,base.nodes_offset,base.faces_offset,original.size()-1};
    for(std::size_t n=0;n<original.size();n+=std::max<std::size_t>(1,original.size()/41))cuts.insert(n);
    for(auto n:cuts){const Bytes prefix(original.begin(),original.begin()+n);auto target=sentinel();check(!decodeCollision48G(prefix,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"original truncation fatal and transactional");}
    auto suffix=original;suffix.push_back(0);auto target=sentinel();check(!decodeCollision48G(suffix,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"trailing original byte rejected");
    for(const auto& change:std::vector<std::pair<std::size_t,std::uint32_t>>{{0,1235},{4,0},{64,2},{68,0xffffffffu},{72,0xffffffffu},{76,0},{80,2},{base.normals_flag_offset,2},{84,0x7fc00000},{base.nodes_offset,0xbf800000},{base.nodes_offset+16,0xbf800000}}){auto b=original;put32(b,change.first,change.second);target=sentinel();check(!decodeCollision48G(b,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"invalid counts flags finite values and AABBs rejected");}
    for(const auto& change:std::vector<std::pair<std::size_t,std::uint16_t>>{{base.nodes_offset+28,0},{base.nodes_offset+28,32767},{base.nodes_offset+28,65535},{base.faces_offset+16,65535}}){auto b=original;put16(b,change.first,change.second);target=sentinel();check(!decodeCollision48G(b,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"node cycle/out of range and triangle index corruption rejected");}
    auto shared=original;shared[base.nodes_offset+30]=shared[base.nodes_offset+28];shared[base.nodes_offset+31]=shared[base.nodes_offset+29];target=sentinel();check(!decodeCollision48G(shared,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"shared subtree and unreachable branch rejected");
    for(auto at:{8u,12u}){auto b=original;put32(b,at,2);target=sentinel();check(!decodeCollision48G(b,target,e)&&e.kind==DecodeKind48C::unsupported&&unchanged(target),"named unsupported original version/type");}
    CollisionLimits48G limits;limits.input_bytes=original.size()-1;target=sentinel();check(!decodeCollision48G(original,target,e,nullptr,limits)&&e.kind==DecodeKind48C::limit&&unchanged(target),"caller input cap");limits={};limits.vertices=1;check(!decodeCollision48G(original,target,e,nullptr,limits)&&e.kind==DecodeKind48C::limit&&unchanged(target),"caller vertex cap");
    for(unsigned stop=1;stop<=5;++stop){Hook h;h.stop=stop;DecodeHooks48C hooks{&h,cancel,progress};target=sentinel();check(!decodeCollision48G(original,target,e,&hooks)&&e.kind==DecodeKind48C::cancelled&&unchanged(target),"all conversion phases cancel transactionally");}
    Bytes native;check(encodeCollisionAsset48G(base,native,error),"cache mutation fixture");
    for(std::size_t n=0;n<native.size();n+=std::max<std::size_t>(1,native.size()/43)){Bytes prefix(native.begin(),native.begin()+n);if(n>=16)put32(prefix,12,static_cast<std::uint32_t>(n));target=sentinel();check(!parseCollisionAsset48G(prefix,target,error)&&unchanged(target),"native truncation with adjusted size rejected transactionally");}
    for(auto at:{0u,8u,12u,16u,20u,32u,36u,40u,44u,48u,52u,68u,132u}){auto b=native;put32(b,at,0xffffffffu);target=sentinel();check(!parseCollisionAsset48G(b,target,error)&&unchanged(target),"native header typed and provenance corruption rejected");}
    auto rawChange=native;rawChange.back()^=1;target=sentinel();check(!parseCollisionAsset48G(rawChange,target,error)&&unchanged(target),"native raw provenance mismatch rejected");
    for(unsigned which=0;which<7;++which){auto c=base;switch(which){case 0:c.matrix[12]+=1;break;case 1:c.vertices[0][0]+=1;break;case 2:c.nodes[0].children[0]=0;break;case 3:c.faces[0].vertices[0]=65535;break;case 4:c.faces[0].d+=1;break;case 5:c.original_bytes[0]^=1;break;default:c.faces_offset+=1;break;}Bytes out{8,9};check(!encodeCollisionAsset48G(c,out,error)&&out==Bytes({8,9}),"encoder rejects inconsistent typed data/provenance transactionally");}
}
void analytic(){
    // Two triangles forming a unit square. Hand-written file records, independent of encoder.
    Bytes b;auto u32=[&](std::uint32_t n){for(unsigned j=0;j<4;++j)b.push_back(static_cast<std::uint8_t>(n>>(8*j)));};auto f=[&](float x){std::uint32_t n;std::memcpy(&n,&x,4);u32(n);};auto u16=[&](unsigned n){b.push_back(static_cast<std::uint8_t>(n));b.push_back(static_cast<std::uint8_t>(n>>8));};
    u32(1234);u32(0x006c6f63);u32(1);u32(0);for(float x:std::array<float,12>{{1,0,0,0,1,0,0,0,1,2,3,4}})f(x);u32(0);u32(4);u32(1);u32(2);u32(0);
    for(float x:std::array<float,12>{{0,0,0,0,0,1,1,0,0,1,0,1}})f(x);u32(0);for(float x:std::array<float,7>{{0,0.5f,0,0.5f,0.5f,0,0.5f}})f(x);u16(32768);u16(32769);
    for(unsigned i=0;i<2;++i){f(0);f(0);f(1);f(0);for(auto x:i?std::array<unsigned,3>{{2,1,3}}:std::array<unsigned,3>{{0,1,2}})u16(x);}
    Collision48G c;DecodeError48C e;std::string error;check(decodeCollision48G(b,c,e),"independent two triangle original fixture");check(c.vertices.size()==4&&c.nodes.size()==1&&c.faces.size()==2&&c.nodes[0].children[1]==(collision_leaf48g|1)&&c.faces[1].vertices[2]==3,"hand-derived triangle/tree semantic invariants");
    Bytes out;check(encodeOriginalCollision48G(c,out,error)&&out==b,"independent source serialization roundtrip");
    std::array<double,16> m,world;check(collisionPlacement48G({{10,20,30}},{{1,0,0}},{{0,1,0}},m,error)&&collisionWorldMatrix48G(c,m,world,error),"analytical nonidentity local composition");
    check(world[12]==14&&world[13]==23&&world[14]==28,"COL local translation rotates before scene translation");
    check(collisionPlacement48G({{0,0,0}},{{0,1,1}},{{0,1,0}},m,error)&&m[9]>0.7&&m[5]==1,"collision preserves nonorthogonal source look unlike geometry");
    const auto before=m;check(!collisionPlacement48G({{0,0,0}},{{0,1,0}},{{0,2,0}},m,error)&&m==before,"parallel axes rejected transactionally");
    check(!collisionPlacement48G({{0,0,0}},{{0,0,0}},{{0,1,0}},m,error)&&m==before,"zero axis rejected transactionally");
    auto badPlane=b;for(unsigned j=0;j<4;++j)put32(badPlane,c.faces_offset+4*j,j?0xffc00000u:0x7fc00000u);auto target=sentinel();
    check(!decodeCollision48G(badPlane,target,e)&&e.kind==DecodeKind48C::malformed&&unchanged(target),"evidenced NaN pattern on nondegenerate face remains malformed");
}
void sourceDegenerates(const fs::path& root){
    const auto audit=json(root/"evidence/importCollision48G/nonfinite-originals/audit.json");
    check(audit.at("counts").at("containers")==103&&audit.at("counts").at("members")==2505&&audit.at("nonfinite_members").size()==9,"complete original COL nonfinite audit inventory");
    std::size_t faces=0,fields=0;
    for(const auto& row:audit.at("nonfinite_members")){
        const auto bytes=read(root/fs::u8path(row.at("fixture").get<std::string>()));check(sha(bytes)==row.at("source_sha256")&&bytes.size()==row.at("source_bytes"),"exact authored degenerate original bytes and SHA");
        const auto first=row.at("triangles")[0].at("source_offset").get<std::size_t>();DecodeError48C error;auto target=sentinel();
        const bool accepted=decodeCollision48G(bytes,target,error);
        check(accepted,"exact authored coincident-plane source is retained");
        if(accepted){
            Bytes original,cache,roundtrip;std::string why;Collision48G parsed;
            check(encodeOriginalCollision48G(target,original,why)&&original==bytes,
                "authored degenerate source roundtrip preserves all original bits");
            const bool cached=encodeCollisionAsset48G(target,cache,why)&&parseCollisionAsset48G(cache,parsed,why);
            check(cached&&encodeOriginalCollision48G(parsed,roundtrip,why)&&roundtrip==bytes,
                "authored degenerate cache retains exact planes, indices and tree");
        }
        faces+=row.at("triangles").size();fields+=row.at("fields").size();
        for(const auto& face:row.at("triangles")){
            const auto at=face.at("source_offset").get<std::size_t>();const auto& v=face.at("vertices");
            check((v[0]==v[1]||v[0]==v[2]||v[1]==v[2])&&face.at("cross")==Json::array({0.0,0.0,0.0}),"independent original face has exact coincident positions and zero area");
            check(get32(bytes,at)==0x7fc00000u&&get32(bytes,at+4)==0xffc00000u&&get32(bytes,at+8)==0xffc00000u&&get32(bytes,at+12)==0xffc00000u,"original degenerate plane exact signed NaN bit pattern");
        }
        const auto nv=get32(bytes,68),nn=get32(bytes,72);const auto normals=get32(bytes,84+12u*nv);const std::size_t nodes=88+12u*nv*(1+normals);
        for(const auto& change:std::vector<std::pair<std::size_t,std::uint32_t>>{{first,0xffc00000u},{first,0x7fc00001u},{first,0x7f800000u},{first,0u},{first+4,0x7fc00000u},{first+8,0u},{84,0x7fc00000u},{nodes,0x7fc00000u}}){auto altered=bytes;put32(altered,change.first,change.second);target=sentinel();check(!decodeCollision48G(altered,target,error)&&error.kind==DecodeKind48C::malformed&&unchanged(target),"unknown NaN patterns partial planes or nonface NaNs remain malformed");}
        for(const auto at:std::array<std::size_t,3>{{first+16,nodes+28,nodes+32u*(nn-1)+28}}){auto altered=bytes;put16(altered,at,65535);target=sentinel();check(!decodeCollision48G(altered,target,error)&&error.kind==DecodeKind48C::malformed&&unchanged(target),"bad indices anywhere in authored degenerate source remain malformed");}
        auto cyclic=bytes;put16(cyclic,nodes+28,0);target=sentinel();check(!decodeCollision48G(cyclic,target,error)&&error.kind==DecodeKind48C::malformed&&unchanged(target),"complete tree still checked for authored degenerate source");
        for(const auto n:std::array<std::size_t,3>{{first,first+15,bytes.size()-1}}){const Bytes cut(bytes.begin(),bytes.begin()+n);target=sentinel();check(!decodeCollision48G(cut,target,error)&&error.kind==DecodeKind48C::malformed&&unchanged(target),"truncated authored degenerate source remains malformed");}
    }
    check(faces==54&&fields==216,"all 54 authored degenerate planes account for all 216 original nonfinite words");
}
}
int main(int argc,char** argv){try{
    if(argc<2||argc>4)throw std::runtime_error("usage: racer_native_collision_check ROOT [REPORT_JSON] [OUTPUT_FIXTURE_DIRECTORY]");const fs::path root=fs::u8path(argv[1]);const auto base=root/"evidence/importCollision48G";const auto index=json(base/"reference-index.json");
    check(index.at("members").size()==38&&index.at("static_bindings").size()==37,"complete independent TA fixture inventory");
    DecodeError48C error;std::string why;Json rows=Json::array();std::map<std::string,Collision48G> meshes;std::uint64_t vertices=0,nodes=0,faces=0;unsigned accepted=0;
    for(const auto& row:index.at("members")){
        const auto source=read(base/fs::u8path(row.at("source_file").get<std::string>()));check(sha(source)==row.at("source_sha256"),"original member SHA256");Collision48G c;const bool ok=decodeCollision48G(source,c,error);check(ok==row.at("accepted").get<bool>(),"independent decoder accepted subset");if(!ok)throw std::runtime_error(error.code+": "+error.detail);
        ++accepted;vertices+=c.vertices.size();nodes+=c.nodes.size();faces+=c.faces.size();const auto expected=json(base/fs::u8path(row.at("reference_file").get<std::string>()));compare(c,expected);
        Bytes original;check(encodeOriginalCollision48G(c,original,why)&&original==source,"complete original bytes reserialized exactly");Bytes cache;check(encodeCollisionAsset48G(c,cache,why),"native collision cache encode");Collision48G restored;check(parseCollisionAsset48G(cache,restored,why),"native collision cache parse");compare(restored,expected);check(restored.original_bytes==source,"complete raw source retained");Bytes again;check(encodeCollisionAsset48G(restored,again,why)&&again==cache,"canonical native cache roundtrip");
        Json record{{"member",row.at("member")},{"source_sha256",sha(source)},{"rrcol_sha256",sha(cache)},{"rrcol_bytes",cache.size()},{"vertices",c.vertices.size()},{"nodes",c.nodes.size()},{"faces",c.faces.size()}};
        if(argc==4){const auto destination=fs::u8path(argv[3])/(fs::u8path(row.at("source_file").get<std::string>()).stem().u8string()+".rrcol");write(destination,cache);record["cache_fixture"]=destination.u8string();}
        std::string name=row.at("member");for(char& ch:name)if(ch>='A'&&ch<='Z')ch=char(ch+32);meshes.emplace(name,std::move(c));rows.push_back(std::move(record));
    }
    for(const auto& row:index.at("static_bindings")){std::array<double,16> m{},w{};const auto p=row.at("position").get<std::array<double,3>>(),l=row.at("look").get<std::array<double,3>>(),u=row.at("up").get<std::array<double,3>>();check(collisionPlacement48G(p,l,u,m,why)&&exact(m,row.at("matrix")),"all original collision placements match independent scalar reference");const auto& c=meshes.at(row.at("member").get<std::string>());check(collisionWorldMatrix48G(c,m,w,why)&&w==m,"all TA identity COL transforms compose to original instance placement");}
    check(vertices==10199&&nodes==14548&&faces==14586&&accepted==38,"independent complete TA collision counts");
    malformed(read(base/fs::u8path(index.at("members")[0].at("source_file").get<std::string>())));analytic();sourceDegenerates(root);
    const Json report{{"status",failures?"FAIL":"PASS"},{"checks",checks},{"failures",failures},{"architecture_bits",8*sizeof(void*)},{"accepted",accepted},{"vertices",vertices},{"nodes",nodes},{"faces",faces},{"static_bindings",index.at("static_bindings").size()},{"collisions",rows},{"scope","Original COL1/type0/unquantized semantic trees and independent collision placements; no query traversal or gameplay equivalence claim"}};
    if(argc>=3){const auto p=fs::u8path(argv[2]);fs::create_directories(p.parent_path());std::ofstream f(p);f<<report.dump(2)<<'\n';if(!f)throw std::runtime_error("report write");}
    std::printf("%u checks, %u failures; %u collision assets; %llu vertices, %llu nodes, %llu faces\n",checks,failures,accepted,static_cast<unsigned long long>(vertices),static_cast<unsigned long long>(nodes),static_cast<unsigned long long>(faces));return failures?1:0;
}catch(const std::exception& e){std::fprintf(stderr,"collision check error: %s\n",e.what());return 2;}}
