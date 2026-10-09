#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "import_world48f.h"
#include "world_asset48f.h"
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
void check(bool ok,const char* label){++checks;if(!ok){++failures;if(failures<=30)std::fprintf(stderr,"FAIL: %s\n",label);}}
Bytes read(const fs::path& p){const auto n=fs::file_size(p);if(n>64u*1024u*1024u)throw std::runtime_error("fixture cap");std::ifstream f(p,std::ios::binary);Bytes b(static_cast<std::size_t>(n));if(n)f.read(reinterpret_cast<char*>(b.data()),static_cast<std::streamsize>(n));if(!f)throw std::runtime_error("fixture read");return b;}
Json json(const fs::path& p){auto b=read(p);return Json::parse(b.begin(),b.end());}
void write(const fs::path& p,const Bytes& b){fs::create_directories(p.parent_path());std::ofstream f(p,std::ios::binary);f.write(reinterpret_cast<const char*>(b.data()),static_cast<std::streamsize>(b.size()));if(!f)throw std::runtime_error("fixture output");}
std::string sha(const Bytes& b){BCRYPT_ALG_HANDLE a=nullptr;BCRYPT_HASH_HANDLE h=nullptr;if(BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)throw std::runtime_error("SHA provider");if(BCryptCreateHash(a,&h,nullptr,0,nullptr,0,0)<0){BCryptCloseAlgorithmProvider(a,0);throw std::runtime_error("SHA init");}std::array<unsigned char,32>d{};auto s=BCryptHashData(h,const_cast<PUCHAR>(b.data()),static_cast<ULONG>(b.size()),0);auto t=BCryptFinishHash(h,d.data(),32,0);BCryptDestroyHash(h);BCryptCloseAlgorithmProvider(a,0);if(s<0||t<0)throw std::runtime_error("SHA update");std::ostringstream o;for(auto x:d)o<<std::hex<<std::setfill('0')<<std::setw(2)<<unsigned(x);return o.str();}
template<std::size_t N>bool exact(const std::array<double,N>& a,const Json& b){if(b.size()!=N)return false;for(std::size_t i=0;i<N;++i){const double v=b[i].get<double>();if(std::memcmp(&a[i],&v,8)!=0)return false;}return true;}
Json fields(const ParameterBlock48F& b){Json result=Json::object();for(const auto& f:b.fields)result[f.name]=f.value;return result;}
void compareParameters(const ParameterDocument48F& p,const Json& expected){
    check(p.version==expected.at("version")&&p.header_table_offset==expected.at("header_table_offset"),"original parameter header");check(p.blocks.size()==expected.at("sections").size(),"all raw blocks retained");
    for(const auto& b:p.blocks){const auto& wanted=expected.at("sections").at(b.name);check(b.offset==wanted.at("offset")&&b.size_without_count==wanted.at("size_without_count"),"original block offsets");check(b.fields.size()==wanted.at("fields").size(),"all raw fields retained");
        for(std::size_t i=0;i<b.fields.size();++i){const auto& f=b.fields[i];const auto& x=wanted.at("fields").at(i);check(f.name==x.at("name")&&f.value==x.at("value")&&f.entry_offset==x.at("entry_offset")&&f.value_offset==x.at("value_offset")&&f.value_bytes==x.at("value_bytes"),"exact original field text and provenance");}}
}
void compareWorld(const World48F& w,const Json& expected,bool parameters=true){
    if(parameters)compareParameters(w.parameters,expected.at("parameter_blocks"));
    check(w.zones.size()==expected.at("zones").size()&&w.portals.size()==expected.at("portals").size()&&w.instances.size()==expected.at("instances").size(),"exact scene counts");
    for(std::size_t i=0;i<w.zones.size();++i){const auto& z=w.zones[i];const auto& x=expected.at("zones").at(i);check(z.zone==x.at("zone")&&z.models==x.at("models")&&fields(w.parameters.blocks[z.parameter_block])==x.at("original_parameters"),"zone counts and original parameters");}
    for(std::size_t i=0;i<w.portals.size();++i){const auto& p=w.portals[i];const auto& x=expected.at("portals").at(i);check(p.section==x.at("section")&&p.zones[0]==x.at("zones")[0]&&p.zones[1]==x.at("zones")[1],"portal identity and connections");for(unsigned j=0;j<4;++j)check(exact(p.vertices[j],x.at("vertices")[j]),"portal vertex bit-exact scalar values");check(fields(w.parameters.blocks[p.parameter_block])==x.at("original_parameters"),"portal original fields");}
    for(std::size_t i=0;i<w.instances.size();++i){const auto& p=w.instances[i];const auto& x=expected.at("instances").at(i);
        check(p.section==x.at("section")&&p.zone==x.at("zone")&&p.ordinal==x.at("ordinal")&&p.source_block_offset==x.at("source_block_offset"),"instance identity/provenance");
        check(p.geometry_member==x.at("geometry_member")&&p.collision_member==x.at("collision_member")&&p.use_lighting==x.at("use_lighting"),"original resource mapping and flag");
        check(exact(p.position,x.at("position"))&&exact(p.look,x.at("look"))&&exact(p.up,x.at("up")),"all source vectors bit-exact after binary32 rounding");
        check(exact(p.matrix,x.at("matrix")),"all placement matrix words exact against frozen scalar reference");
        check(fields(w.parameters.blocks[p.parameter_block])==x.at("original_parameters"),"instance original fields retained");
    }
}
void put32(Bytes& b,std::size_t at,std::uint32_t v){for(unsigned i=0;i<4;++i)b[at+i]=static_cast<std::uint8_t>(v>>(8*i));}
ParameterField48F& field(ParameterDocument48F& p,const std::string& block,const std::string& key){for(auto& b:p.blocks)if(b.name==block)for(auto& f:b.fields)if(f.name==key)return f;throw std::runtime_error("mutation field missing");}
struct Hook{bool cancel=false;unsigned calls=0,stop=0xffffffffu;};
bool cancelled(void* p){return static_cast<Hook*>(p)->cancel;}
void progress(void* p,const char*,std::uint32_t,std::uint32_t){auto& h=*static_cast<Hook*>(p);if(++h.calls>=h.stop)h.cancel=true;}
World48F sentinel(){World48F w;w.scene_info_block=998;w.parameters.original_bytes={8,7,6};return w;}
bool unchanged(const World48F& w){return w.scene_info_block==998&&w.parameters.original_bytes==Bytes({8,7,6})&&w.instances.empty();}
void malformedTests(const fs::path& root){
    const auto original=read(root/"evidence/worldScene48E/original-ta.scb");ParameterDocument48F p;DecodeError48C error;check(decodeParameters48F(original,p,error),"TA parameter fixture");
    std::set<std::size_t> cuts{0,6,7,10,11,14,original.size()-1,p.header_table_offset};for(std::size_t i=0;i<original.size();i+=original.size()/41)cuts.insert(i);for(const auto& b:p.blocks)cuts.insert(b.offset);
    for(auto n:cuts){ParameterDocument48F out;out.version=998;const Bytes prefix(original.begin(),original.begin()+n);check(!decodeParameters48F(prefix,out,error)&&error.kind==DecodeKind48C::malformed&&out.version==998,"original SCB truncation fatal and transactional");}
    const auto start=p.blocks[0].offset,table=p.header_table_offset;
    for(const auto& m:std::vector<std::pair<std::size_t,std::uint32_t>>{{7,2},{11,0xffffffffu},{15,0xffffffffu},{table,0xffffffffu},{table+4,0xffffffffu},{start,0xffffffffu},{start+39,0},{start+43,0xffffffffu}}){auto b=original;put32(b,m.first,m.second);ParameterDocument48F out;out.version=998;check(!decodeParameters48F(b,out,error)&&out.version==998,"bad parameter header/table/count rejected");}
    auto suffix=original;suffix.push_back(0);ParameterDocument48F out;check(!decodeParameters48F(suffix,out,error)&&error.kind==DecodeKind48C::malformed,"parameter suffix fatal");
    WorldLimits48F limits;limits.input_bytes=original.size()-1;World48F w=sentinel();check(!decodeWorld48F(original,w,error,nullptr,limits)&&error.kind==DecodeKind48C::limit&&unchanged(w),"world caller input cap");
    limits={};limits.instances=317;check(!decodeWorld48F(original,w,error,nullptr,limits)&&error.kind==DecodeKind48C::limit&&unchanged(w),"world caller instance cap");
    for(unsigned stop:{1u,20u,379u,380u,384u}){Hook state;state.stop=stop;DecodeHooks48C hooks{&state,cancelled,progress};w=sentinel();check(!decodeWorld48F(original,w,error,&hooks)&&error.kind==DecodeKind48C::cancelled&&unchanged(w),"parameter/world phase cancellation transactional");}
    struct Mutation{const char* block;const char* key;const char* value;};
    for(const auto& m:std::vector<Mutation>{{"sceneinfo","numzones","10"},{"sceneinfo","numzones","garbage"},{"zone1","numberofstaticmodels","147"},{"zone1-model1","slt","..\\ta_bigrocka.slt"},{"zone1-model1","position","nan,0,0"},{"zone1-model1","position","1e999,0,0"},{"zone1-model1","position","0x1,0,0"},{"zone1-model1","lookvector","0,1,0"},{"zone1-model1","uselighting","maybe"},{"portal1","connect1","10"}}){auto changed=p;field(changed,m.block,m.key).value=m.value;Bytes b;check(encodeParameters48F(changed,b,error),"independent parameter mutation serialization");w=sentinel();check(!decodeWorld48F(b,w,error)&&unchanged(w),"semantic mutation rejected without repair");}
    auto analytic=p;field(analytic,"zone1-model1","position").value="10,20,30";field(analytic,"zone1-model1","lookvector").value="1,0,0";field(analytic,"zone1-model1","upvector").value="0,1,0";Bytes b;check(encodeParameters48F(analytic,b,error)&&decodeWorld48F(b,w,error),"analytical orientation fixture");
    if(!w.instances.empty()){const auto& m=w.instances[0].matrix;check(1*m[0]+2*m[4]+3*m[8]+m[12]==13&&1*m[1]+2*m[5]+3*m[9]+m[13]==22&&1*m[2]+2*m[6]+3*m[10]+m[14]==29,"independent row-vector handedness and root replacement");}
    const auto decimalReference=json(root/"evidence/importWorld48F/decimal-reference.json");
    check(decimalReference.at("cases").size()==9,"all nine independent decimal fixtures present");
    for(const auto& row:decimalReference.at("cases")){
        auto changed=p;field(changed,"zone1-model1","position").value=row.at("input");Bytes bytes;
        check(encodeParameters48F(changed,bytes,error),"frozen decimal grammar fixture serialization");w=sentinel();const bool accepted=decodeWorld48F(bytes,w,error);
        check(accepted==row.at("accepted").get<bool>(),"decimal overflow/underflow acceptance matches unchanged Python48E");
        if(accepted)check(exact(w.instances[0].position,row.at("values")),"frozen binary64 decimal results including signed zero exact");
    }
    check(decodeWorld48F(original,w,error),"native cache fixture");Bytes cache;std::string why;check(encodeWorldAsset48F(w,cache,why),"native world serialization");
    for(std::size_t n=0;n<cache.size();n+=std::max<std::size_t>(1,cache.size()/47)){Bytes prefix(cache.begin(),cache.begin()+n);if(prefix.size()>=16)put32(prefix,12,static_cast<std::uint32_t>(prefix.size()));World48F target=sentinel();check(!parseWorldAsset48F(prefix,target,why)&&unchanged(target),"native cache truncation bounded/transactional even with adjusted size");}
    for(const auto& m:std::vector<std::pair<std::size_t,std::uint32_t>>{{8,2},{12,1},{24,0xffffffffu},{28,0xffffffffu},{32,0xffffffffu},{36,0xffffffffu},{40,0xffffffffu},{44,0xffffffffu},{48+7,2},{48+table,0xffffffffu},{48+start,0xffffffffu}}){auto bytes=cache;put32(bytes,m.first,m.second);World48F target=sentinel();check(!parseWorldAsset48F(bytes,target,why)&&unchanged(target),"native cache/header/raw-provenance mutation rejected");}
    for(unsigned kind=0;kind<8;++kind){auto changed=w;switch(kind){case 0:changed.instances[0].matrix[0]+=0.25;break;case 1:changed.instances[0].position[0]+=1;changed.instances[0].matrix[12]+=1;break;case 2:changed.instances[0].look[0]+=1;break;case 3:changed.instances[0].geometry_member="geometry\\different.psg";changed.instances[0].collision_member="geometry\\different.col";break;case 4:changed.parameters.original_bytes[0]^=1;break;case 5:changed.parameters.blocks[0].fields[0].value="different";break;case 6:changed.instances[0].use_lighting=!changed.instances[0].use_lighting;break;default:changed.portals[0].vertices[0][0]+=1;break;}Bytes target{7,8};check(!encodeWorldAsset48F(changed,target,why)&&target==Bytes({7,8}),"encoder rejects internally inconsistent world semantics/provenance");}
    // Native typed affine basis corruption, preserving raw data and all counts.
    // Last instance has fixed25 doubles + flag immediately before end-of-file.
    auto corrupt=cache;const auto basis=corrupt.size()-4-16*8;double wrong=2.0;std::uint64_t bits;std::memcpy(&bits,&wrong,8);for(unsigned i=0;i<8;++i)corrupt[basis+i]=static_cast<std::uint8_t>(bits>>(8*i));World48F target=sentinel();check(!parseWorldAsset48F(corrupt,target,why)&&unchanged(target),"cache affine basis must match original look/up operations");
}
}
int main(int argc,char** argv){try{
    if(argc<2||argc>4)throw std::runtime_error("usage: racer_native_world_check ROOT [REPORT_JSON] [OUTPUT_FIXTURE_DIRECTORY]");const fs::path root=fs::u8path(argv[1]);DecodeError48C error;std::string why;Json rows=Json::array();unsigned accepted=0,rejected=0;std::uint64_t totalInstances=0;
    for(const auto& item:json(root/"evidence/worldImport48F/reference-index.json")){
        const fs::path file=root/fs::u8path(item.at("file").get<std::string>());const auto original=read(file);check(sha(original)==item.at("source_sha256"),"original world member SHA256");World48F world;const bool ok=decodeWorld48F(original,world,error);check(ok==item.at("accepted").get<bool>(),"native/Python world accepted subset agrees");
        Json row{{"container",item.at("container")},{"source_sha256",sha(original)},{"accepted",ok}};
        if(!ok){++rejected;check(error.kind==DecodeKind48C::unsupported,"complete rejected original world is explicitly unsupported");row["code"]=error.code;row["detail"]=error.detail;}
        else{++accepted;totalInstances+=world.instances.size();auto reference=file;reference.replace_extension(".reference.json");const auto expected=json(reference);compareWorld(world,expected);Bytes reversible;check(encodeParameters48F(world.parameters,reversible,error)&&reversible==original,"all original parameter bytes roundtrip");
            Bytes cache;check(encodeWorldAsset48F(world,cache,why),"native world asset encode");World48F restored;check(parseWorldAsset48F(cache,restored,why),"typed native world cache parse");compareWorld(restored,expected);check(restored.parameters.original_bytes==original,"native cache retains complete raw provenance");Bytes again;check(encodeWorldAsset48F(restored,again,why)&&again==cache,"canonical native cache roundtrip");row["rrworld_sha256"]=sha(cache);row["rrworld_bytes"]=cache.size();row["instances"]=world.instances.size();
            if(argc==4){const fs::path destination=fs::u8path(argv[3])/(file.stem().u8string()+".rrworld");write(destination,cache);row["cache_fixture"]=destination.u8string();}}
        rows.push_back(std::move(row));
    }
    for(const auto& pair:std::vector<std::pair<const char*,const char*>>{{"original-ta.scb","ta-parameter-blocks.json"},{"original-ta.tob","ta-toy-parameter-blocks.json"}}){const auto bytes=read(root/"evidence/worldScene48E"/pair.first);ParameterDocument48F p;check(decodeParameters48F(bytes,p,error),"frozen SCB/TOB parameter decode");compareParameters(p,json(root/"evidence/worldScene48E"/pair.second));Bytes rebuilt;check(encodeParameters48F(p,rebuilt,error)&&rebuilt==bytes,"frozen SCB/TOB byte-exact reserialization");}
    World48F ta;check(decodeWorld48F(read(root/"evidence/worldScene48E/original-ta.scb"),ta,error),"frozen original TA world decode");compareWorld(ta,json(root/"evidence/worldScene48E/world.json"),false);
    malformedTests(root);Json report{{"status",failures?"FAIL":"PASS"},{"checks",checks},{"failures",failures},{"architecture_bits",8*sizeof(void*)},{"accepted",accepted},{"unsupported",rejected},{"instances",totalInstances},{"worlds",rows},{"scope","Portable original labelled blocks and typed native world cache; exact frozen scalar matrices, no raw SCB decode at runtime, no VU/visibility/collision/gameplay equivalence claim"}};
    if(argc>=3){std::ofstream out(fs::u8path(argv[2]),std::ios::binary);out<<report.dump(2)<<'\n';if(!out)throw std::runtime_error("report output");}report.erase("worlds");std::printf("%s\n",report.dump().c_str());return failures?1:0;
}catch(const std::exception& e){std::fprintf(stderr,"ERROR: %s\n",e.what());return 2;}}
