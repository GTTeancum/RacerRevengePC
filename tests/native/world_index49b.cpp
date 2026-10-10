#include "../../src/assets/world_index49b.h"
#include "../../platform/windows/native/import_service48f.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Serial requested-C++-heap accounting, not allocator usable-size or RSS.
// Headers/alignment slack, preloaded input bytes, stack and allocator metadata
// are excluded. An epoch counts only requests made inside the measured load.
namespace heap49b {
struct Header { void* raw; std::size_t requested; std::uint64_t epoch; };
std::uint64_t calls=0, epoch=0;
std::size_t live=0, peak=0;
bool enabled=false;
void* allocate(std::size_t n,std::size_t alignment) {
    ++calls;
    alignment=std::max(alignment,alignof(Header));
    if(n>std::numeric_limits<std::size_t>::max()-alignment-sizeof(Header))throw std::bad_alloc();
    void* raw=std::malloc((n?n:1)+alignment+sizeof(Header));
    if(!raw)throw std::bad_alloc();
    const auto start=reinterpret_cast<std::uintptr_t>(raw)+sizeof(Header);
    const auto address=(start+alignment-1)&~(std::uintptr_t(alignment)-1);
    auto* h=reinterpret_cast<Header*>(address)-1;
    h->raw=raw;h->requested=n;h->epoch=enabled?epoch:0;
    if(enabled){live+=n;peak=std::max(peak,live);}
    return reinterpret_cast<void*>(address);
}
void release(void* p) noexcept {
    if(!p)return;
    auto* h=static_cast<Header*>(p)-1;
    if(enabled&&h->epoch==epoch)live-=h->requested;
    std::free(h->raw);
}
void begin(){++epoch;live=peak=0;enabled=true;}
void end(){enabled=false;}
}
void* operator new(std::size_t n){return heap49b::allocate(n,alignof(std::max_align_t));}
void* operator new[](std::size_t n){return ::operator new(n);}
void* operator new(std::size_t n,std::align_val_t a){return heap49b::allocate(n,static_cast<std::size_t>(a));}
void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
void operator delete(void* p)noexcept{heap49b::release(p);}
void operator delete[](void* p)noexcept{heap49b::release(p);}
void operator delete(void* p,std::size_t)noexcept{heap49b::release(p);}
void operator delete[](void* p,std::size_t)noexcept{heap49b::release(p);}
void operator delete(void* p,std::align_val_t)noexcept{heap49b::release(p);}
void operator delete[](void* p,std::align_val_t)noexcept{heap49b::release(p);}
void operator delete(void* p,std::size_t,std::align_val_t)noexcept{heap49b::release(p);}
void operator delete[](void* p,std::size_t,std::align_val_t)noexcept{heap49b::release(p);}

namespace {
using namespace rr::assets;
using Bytes=std::vector<std::uint8_t>;
using Clock=std::chrono::steady_clock;
static_assert(std::is_same<decltype(rr::windows::RuntimeWorld48F{}.world_index),OwnedWorldIndex49B>::value,"production runtime retains typed metadata");
static_assert(std::is_nothrow_move_constructible<rr::windows::RuntimeWorld48F>::value,"production runtime moves immutable metadata ownership");
namespace fs=std::filesystem;
std::uint64_t checks=0, comparedDoubles=0, malformedCases=0, copyFailures=0, typedFailures=0;
volatile std::uint64_t benchmarkSink=0;
void require(bool condition,const char* label){++checks;if(!condition)throw std::runtime_error(label);}
std::uint64_t bits(double d){std::uint64_t u;std::memcpy(&u,&d,8);return u;}
template<std::size_t N>void exact(const std::array<double,N>& a,const std::array<double,N>& b){
    for(std::size_t i=0;i<N;++i){require(bits(a[i])==bits(b[i]),"binary64 bit identity");++comparedDoubles;}
}
Bytes read(const fs::path& path){
    const auto size=fs::file_size(path);require(size<=max_world_asset_bytes48f,"fixture extent");
    Bytes bytes(static_cast<std::size_t>(size));std::ifstream file(path,std::ios::binary);
    if(size)file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(size));
    require(bool(file),"fixture read");return bytes;
}
void put32(Bytes& b,std::size_t at,std::uint32_t n){require(at+4<=b.size(),"mutation extent");for(unsigned i=0;i<4;++i)b[at+i]=std::uint8_t(n>>(8*i));}
std::uint64_t mix(std::uint64_t h,std::uint64_t n){return (h^n)*1099511628211ull;}
std::uint64_t hashText(std::uint64_t h,std::string_view s){h=mix(h,s.size());for(unsigned char c:s)h=mix(h,c);return h;}
template<std::size_t N>std::uint64_t hashValues(std::uint64_t h,const std::array<double,N>& a){for(double d:a)h=mix(h,bits(d));return h;}
std::uint64_t digest(WorldIndexView49B v){
    std::uint64_t h=1469598103934665603ull;
    for(const auto& z:v.zones()){h=mix(h,z.zone);h=mix(h,z.placement_count);for(auto id:v.zoneIncidentPortals(z.zone))h=mix(h,id);}
    for(const auto& p:v.placements()){
        h=mix(h,p.zone);h=mix(h,p.ordinal);h=mix(h,p.use_lighting);h=mix(h,p.resource_pair);
        h=hashValues(h,p.position);h=hashValues(h,p.look);h=hashValues(h,p.up);h=hashValues(h,p.matrix);
        WorldResourceText49B t;require(v.resourceText(p.resource_pair,t),"digest resource");h=hashText(h,t.geometry_member);h=hashText(h,t.collision_member);
    }
    for(const auto& p:v.portals()){h=mix(h,p.ordinal);h=mix(h,p.zones[0]);h=mix(h,p.zones[1]);for(const auto& xyz:p.vertices)h=hashValues(h,xyz);}
    return h;
}
void compare(const World48F& w,WorldIndexView49B v,const WorldIndexProvenance49B* provenance=nullptr){
    const auto q=v.requirements();
    require(q.zones==w.zones.size()&&q.placements==w.instances.size()&&q.portals==w.portals.size(),"full ordered counts");
    require(q.incident_portal_ids==2*w.portals.size(),"one incidence per ordered endpoint");
    std::map<std::pair<std::string,std::string>,std::uint32_t> pairs;
    std::size_t first=0,incident=0,textBytes=0;
    for(std::size_t i=0;i<w.zones.size();++i){const auto& a=w.zones[i];const auto& b=v.zones()[i];
        require(b.zone==a.zone&&b.first_placement==first&&b.placement_count==a.models,"zone identity and direct ordered span");
        require(v.zone(a.zone)==v.zones().data+i,"zone lookup identity");
        const auto members=v.zonePlacements(a.zone);require(members.size==a.models,"zone placement count");
        for(std::size_t j=0;j<members.size;++j)require(&members[j]==v.placement(a.zone,std::uint32_t(j+1))&&&members[j]==v.placementById(std::uint32_t(first+j)),"zone order and placement lookup");
        const auto ids=v.zoneIncidentPortals(a.zone);std::size_t count=0;
        for(std::size_t j=0;j<w.portals.size();++j){const auto& p=w.portals[j];for(auto endpoint:p.zones)if(endpoint==a.zone){require(count<ids.size&&ids[count]==j,"incident portal original order");++count;}}
        require(ids.size==count&&b.first_incident_portal==incident&&b.incident_portal_count==count,"incident span counts");
        first+=a.models;incident+=count;
    }
    for(std::size_t i=0;i<w.instances.size();++i){const auto& a=w.instances[i];const auto& b=v.placements()[i];
        require(a.section=="zone"+std::to_string(b.zone)+"-model"+std::to_string(b.ordinal),"lossless placement section identity");
        require(a.zone==b.zone&&a.ordinal==b.ordinal&&b.use_lighting==std::uint32_t(a.use_lighting),"placement identity lighting");
        exact(a.position,b.position);exact(a.look,b.look);exact(a.up,b.up);exact(a.matrix,b.matrix);
        const auto key=std::make_pair(a.geometry_member,a.collision_member);auto found=pairs.find(key);
        if(found==pairs.end()){const auto id=std::uint32_t(pairs.size());found=pairs.emplace(key,id).first;textBytes+=key.first.size()+key.second.size();}
        require(b.resource_pair==found->second,"resource pair first-appearance identity");WorldResourceText49B text;
        require(v.resourceText(b.resource_pair,text)&&text.geometry_member==a.geometry_member&&text.collision_member==a.collision_member,"both resource names byte-exact");
    }
    for(std::size_t i=0;i<w.portals.size();++i){const auto& a=w.portals[i];const auto& b=v.portals()[i];
        require(a.section=="portal"+std::to_string(b.ordinal)&&b.ordinal==i+1&&a.zones==b.zones&&b.reserved==0,"portal identity and ordered endpoints");
        require(v.portalById(std::uint32_t(i))==v.portals().data+i,"portal lookup identity");for(unsigned j=0;j<4;++j)exact(a.vertices[j],b.vertices[j]);
    }
    require(q.resource_pairs==pairs.size()&&q.text_bytes==textBytes,"interned resource counts and exact text extent");
    require(q.payload_bytes==q.zones*sizeof(WorldZoneRecord49B)+q.placements*sizeof(WorldPlacement49B)+q.portals*sizeof(WorldPortalRecord49B)+q.resource_pairs*sizeof(WorldResourcePair49B)+q.text_bytes+q.incident_portal_ids*sizeof(std::uint32_t),"typed element payload arithmetic");
    if(provenance){const auto& p=*provenance;require(p.scene_info_block==w.scene_info_block,"scene source identity");
        require(p.zone_parameter_blocks.size()==w.zones.size()&&p.portal_parameter_blocks.size()==w.portals.size()&&p.placement_parameter_blocks.size()==w.instances.size()&&p.placement_source_block_offsets.size()==w.instances.size(),"optional provenance lengths");
        for(std::size_t i=0;i<w.zones.size();++i)require(p.zone_parameter_blocks[i]==w.zones[i].parameter_block,"zone source block identity");
        for(std::size_t i=0;i<w.portals.size();++i)require(p.portal_parameter_blocks[i]==w.portals[i].parameter_block,"portal source block identity");
        for(std::size_t i=0;i<w.instances.size();++i)require(p.placement_parameter_blocks[i]==w.instances[i].parameter_block&&p.placement_source_block_offsets[i]==w.instances[i].source_block_offset,"placement source block and byte offset exact");
    }
}
struct Storage {
    std::vector<WorldZoneRecord49B> zones;
    std::vector<WorldPlacement49B> placements;
    std::vector<WorldPortalRecord49B> portals;
    std::vector<WorldResourcePair49B> resources;
    std::vector<char> text;
    std::vector<std::uint32_t> incidents;
    explicit Storage(WorldIndexRequirements49B q,std::size_t spare=0):zones(q.zones+spare),placements(q.placements+spare),portals(q.portals+spare),resources(q.resource_pairs+spare),text(q.text_bytes+spare),incidents(q.incident_portal_ids+spare){}
    WorldIndexStorage49B spans(){return {{zones.data(),zones.size()},{placements.data(),placements.size()},{portals.data(),portals.size()},{resources.data(),resources.size()},{text.data(),text.size()},{incidents.data(),incidents.size()}};}
    template<class T>static void append(Bytes& b,const std::vector<T>& v){const auto* p=reinterpret_cast<const std::uint8_t*>(v.data());if(!v.empty())b.insert(b.end(),p,p+v.size()*sizeof(T));}
    Bytes snapshot()const{Bytes b;append(b,zones);append(b,placements);append(b,portals);append(b,resources);append(b,text);append(b,incidents);return b;}
    template<class T>static void poison(std::vector<T>& v){if(!v.empty())std::memset(static_cast<void*>(v.data()),0xa5,v.size()*sizeof(T));}
    void poison(){poison(zones);poison(placements);poison(portals);poison(resources);poison(text);poison(incidents);}
    template<class T>static void guard(const std::vector<T>& v,std::size_t used){const auto* p=reinterpret_cast<const unsigned char*>(v.data());for(std::size_t i=used*sizeof(T);i<v.size()*sizeof(T);++i)require(p[i]==0xa5,"caller capacity slack not overwritten");}
    void guards(WorldIndexRequirements49B q)const{guard(zones,q.zones);guard(placements,q.placements);guard(portals,q.portals);guard(resources,q.resource_pairs);guard(text,q.text_bytes);guard(incidents,q.incident_portal_ids);}
};
void ownsStorage(WorldIndexView49B view,const WorldIndexStorage49B& storage,WorldIndexRequirements49B q){
    require(view.zones().data==storage.zones.data&&view.zones().size==q.zones,"copied zone view owns exact caller extent");
    require(view.placements().data==storage.placements.data&&view.placements().size==q.placements,"copied placement view owns exact caller extent");
    require(view.portals().data==storage.portals.data&&view.portals().size==q.portals,"copied portal view owns exact caller extent");
    require(view.resourcePairs().data==storage.resource_pairs.data&&view.resourcePairs().size==q.resource_pairs,"copied resource view owns exact caller extent");
    require(view.text().data==storage.text.data&&view.text().size==q.text_bytes,"copied text view owns exact caller extent");
    require(view.incidentPortalIds().data==storage.incident_portal_ids.data&&view.incidentPortalIds().size==q.incident_portal_ids,"copied incident view owns exact caller extent");
}
void boundaries(WorldIndexView49B v){
    for(std::uint32_t id:{0u,std::uint32_t(v.zones().size+1),0xffffffffu}){require(!v.zone(id)&&!v.placement(id,1)&&v.zonePlacements(id).empty()&&v.zoneIncidentPortals(id).empty(),"invalid zone rejected");}
    for(const auto& z:v.zones())for(std::uint32_t ordinal:{0u,z.placement_count+1,0xffffffffu})require(!v.placement(z.zone,ordinal),"placement ordinal boundaries");
    for(std::uint32_t id:{std::uint32_t(v.placements().size),0xffffffffu})require(!v.placementById(id),"placement ID boundaries");
    for(std::uint32_t id:{std::uint32_t(v.portals().size),0xffffffffu})require(!v.portalById(id),"portal ID boundaries");
    WorldResourceText49B t{"sentinel geometry","sentinel member"};
    for(std::uint32_t id:{std::uint32_t(v.resourcePairs().size),0xffffffffu})require(!v.resourceText(id,t)&&t.geometry_member=="sentinel geometry"&&t.collision_member=="sentinel member","resource failure output atomicity");
}
World48F synthetic(bool populated){
    ParameterDocument48F p;
    auto block=[&](std::string name,std::initializer_list<std::pair<const char*,const char*>> fields){ParameterBlock48F b;b.name=std::move(name);std::size_t offset=1;for(const auto& f:fields){ParameterField48F v;v.name=f.first;v.value=f.second;v.value_offset=offset++;b.fields.push_back(std::move(v));}std::sort(b.fields.begin(),b.fields.end(),[](const auto& a,const auto& b){return a.name<b.name;});p.blocks.push_back(std::move(b));};
    if(populated)block("portal1",{{"connect1","2"},{"connect2","1"},{"vertex1","-0,1,2"},{"vertex2","3,4,5"},{"vertex3","6,7,8"},{"vertex4","9,10,11"}});
    block("sceneinfo",{{"numzones",populated?"2":"1"},{"numportals",populated?"1":"0"}});
    block("unknown-metadata",{{"opaque","retain me in RRWORLD1"}});
    block("zone1",{{"numberofstaticmodels",populated?"2":"0"}});
    if(populated){
        block("zone1-model1",{{"slt","Alpha.SLT"},{"position","-0,0.125,-8.5"},{"lookvector","1,2,3"},{"upvector","0,1,0"},{"uselighting","T"}});
        block("zone1-model2",{{"slt","beta.slt"},{"position","1.401298464324817e-45,-0,0"},{"lookvector","0,0,1"},{"upvector","0,1,0"},{"uselighting","F"}});
        block("zone2",{{"numberofstaticmodels","1"}});
        block("zone2-model1",{{"slt","alpha.slt"},{"position","1,2,3"},{"lookvector","0,0,1"},{"upvector","0,1,0"},{"uselighting","T"}});
    }
    std::sort(p.blocks.begin(),p.blocks.end(),[](const auto& a,const auto& b){return a.name<b.name;});
    Bytes raw;DecodeError48C e;World48F w;require(encodeParameters48F(p,raw,e)&&decodeWorld48F(raw,w,e),"analytical source fixture through original parameter encoder/decoder");return w;
}
void copyTests(WorldIndexView49B source){
    Storage storage(source.requirements(),3);storage.poison();auto descriptor=storage.spans();WorldIndexView49B out=source;WorldIndexCopyError49B error;
    const auto before=storage.snapshot();const auto expected=digest(source);
    auto fail=[&](const WorldIndexStorage49B& d,WorldIndexCopyError49B wanted){
        const auto allocations=heap49b::calls;require(!copyWorldIndex49B(source,d,out,error)&&error==wanted,"copy expected failure");require(heap49b::calls==allocations,"failed copy no allocations");
        require(storage.snapshot()==before&&digest(out)==expected,"failed copy destination and view untouched");++copyFailures;
    };
    auto shortBuffer=descriptor;shortBuffer.zones.size=source.zones().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    shortBuffer=descriptor;shortBuffer.placements.size=source.placements().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    shortBuffer=descriptor;shortBuffer.portals.size=source.portals().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    shortBuffer=descriptor;shortBuffer.resource_pairs.size=source.resourcePairs().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    shortBuffer=descriptor;shortBuffer.text.size=source.text().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    shortBuffer=descriptor;shortBuffer.incident_portal_ids.size=source.incidentPortalIds().size-1;fail(shortBuffer,WorldIndexCopyError49B::insufficient_storage);
    for(unsigned i=0;i<6;++i){auto nullBuffer=descriptor;switch(i){case 0:nullBuffer.zones.data=nullptr;break;case 1:nullBuffer.placements.data=nullptr;break;case 2:nullBuffer.portals.data=nullptr;break;case 3:nullBuffer.resource_pairs.data=nullptr;break;case 4:nullBuffer.text.data=nullptr;break;default:nullBuffer.incident_portal_ids.data=nullptr;break;}fail(nullBuffer,WorldIndexCopyError49B::null_storage);}
    auto alias=descriptor;alias.zones.data=const_cast<WorldZoneRecord49B*>(source.zones().data);alias.zones.size=source.zones().size;fail(alias,WorldIndexCopyError49B::overlapping_storage);
    alias=descriptor;alias.text={reinterpret_cast<char*>(storage.placements.data()),source.text().size};fail(alias,WorldIndexCopyError49B::overlapping_storage);
    alias=descriptor;alias.text={reinterpret_cast<char*>(&source),sizeof(source)};fail(alias,WorldIndexCopyError49B::overlapping_storage);
    alias=descriptor;alias.text={reinterpret_cast<char*>(&alias),sizeof(alias)};fail(alias,WorldIndexCopyError49B::overlapping_storage);
    alias=descriptor;alias.text={reinterpret_cast<char*>(&out),sizeof(out)};fail(alias,WorldIndexCopyError49B::overlapping_storage);
    auto badExtent=descriptor;badExtent.placements.size=std::numeric_limits<std::size_t>::max();fail(badExtent,WorldIndexCopyError49B::invalid_extent);
    auto badAlignment=descriptor;badAlignment.placements.data=reinterpret_cast<WorldPlacement49B*>(reinterpret_cast<unsigned char*>(storage.placements.data())+1);fail(badAlignment,WorldIndexCopyError49B::misaligned_storage);
    WorldIndexView49B empty;require(!copyWorldIndex49B(empty,descriptor,out,error)&&error==WorldIndexCopyError49B::invalid_source&&storage.snapshot()==before&&digest(out)==expected,"default source is failure, never empty success");
    const auto count=heap49b::calls;
    for(unsigned i=0;i<1000;++i){require(copyWorldIndex49B(source,descriptor,out,error)&&error==WorldIndexCopyError49B::none,"caller-owned successful repeated copy");ownsStorage(out,descriptor,source.requirements());boundaries(out);benchmarkSink=digest(out);}
    require(heap49b::calls==count,"core copy/query repeated loops allocate zero");
    require(digest(out)==expected,"caller storage all values retained");storage.guards(source.requirements());
    // Exact in-place is rejected, including when source and output are one view.
    const auto resident=out;auto inplace=descriptor;
    require(!copyWorldIndex49B(out,inplace,out,error)&&error==WorldIndexCopyError49B::overlapping_storage&&digest(out)==digest(resident),"in-place output/source alias failure atomicity");
    Storage second(source.requirements());auto inout=source;require(copyWorldIndex49B(inout,second.spans(),inout,error),"source/output view object alias allowed with disjoint arrays");ownsStorage(inout,second.spans(),source.requirements());require(digest(inout)==expected,"source/output view alias exact content");
}
WorldIndexProvenance49B provenanceSentinel(){WorldIndexProvenance49B p;p.scene_info_block=987;p.zone_parameter_blocks={11,22};p.portal_parameter_blocks={33};p.placement_parameter_blocks={44};p.placement_source_block_offsets={55};return p;}
bool sameProvenance(const WorldIndexProvenance49B& a,const WorldIndexProvenance49B& b){return a.scene_info_block==b.scene_info_block&&a.zone_parameter_blocks==b.zone_parameter_blocks&&a.portal_parameter_blocks==b.portal_parameter_blocks&&a.placement_parameter_blocks==b.placement_parameter_blocks&&a.placement_source_block_offsets==b.placement_source_block_offsets;}
void badInputTests(const Bytes& good,const World48F& world){
    std::string error;OwnedWorldIndex49B target;require(loadWorldIndex49B(good,target,error),"negative-test target");const auto saved=digest(target.view());
    auto provenance=provenanceSentinel();const auto expected=provenance;
    auto reject=[&](const Bytes& bad){World48F parsed;parsed.scene_info_block=123456;parsed.parameters.original_bytes={9,8,7};
        require(!parseWorldAsset48F(bad,parsed,error)&&!error.empty()&&parsed.scene_info_block==123456&&parsed.parameters.original_bytes==Bytes({9,8,7})&&parsed.instances.empty(),"original parser malformed rejection atomicity");
        require(!loadWorldIndex49B(bad,target,error,&provenance)&&!error.empty()&&digest(target.view())==saved&&sameProvenance(provenance,expected),"frontdoor malformed rejection both outputs atomic");++malformedCases;
    };
    for(std::size_t cut=0;cut<good.size();cut+=std::max<std::size_t>(1,good.size()/53)){Bytes bad(good.begin(),good.begin()+cut);if(bad.size()>=16)put32(bad,12,std::uint32_t(bad.size()));reject(bad);}
    for(auto cut:{std::size_t(0),std::size_t(47),good.size()-1}){Bytes bad(good.begin(),good.begin()+cut);reject(bad);}
    for(const auto& m:std::vector<std::pair<std::size_t,std::uint32_t>>{{0,0},{8,2},{12,1},{16,2},{20,0xffffffffu},{24,0xffffffffu},{28,0},{28,8193},{32,0},{32,257},{36,4097},{40,100001},{44,0xffffffffu},{48,0},{48+7,2},{48+world.parameters.header_table_offset,0xffffffffu},{good.size()-4,2}}){auto bad=good;put32(bad,m.first,m.second);reject(bad);}
    Bytes oversized(max_world_asset_bytes48f+1,0);reject(oversized);
    auto suffix=good;suffix.push_back(0);put32(suffix,12,std::uint32_t(suffix.size()));reject(suffix);
    auto nonfinite=good;put32(nonfinite,good.size()-12,0);put32(nonfinite,good.size()-8,0x7ff00000);reject(nonfinite);
    auto mismatch=good;const auto basis=good.size()-4-16*8;const auto wrong=bits(2.0);for(unsigned i=0;i<8;++i)mismatch[basis+i]=std::uint8_t(wrong>>(8*i));reject(mismatch);
    for(unsigned kind=0;kind<23;++kind){auto changed=world;switch(kind){
        case 0:changed.zones[0].zone=0;break;case 1:++changed.zones[0].models;break;
        case 2:changed.zones[0].parameter_block=0xffffffffu;break;case 3:changed.instances[0].zone=0;break;
        case 4:changed.instances[0].ordinal=0;break;case 5:changed.instances[0].section="wrong";break;
        case 6:changed.instances[0].parameter_block=0xffffffffu;break;case 7:++changed.instances[0].source_block_offset;break;
        case 8:changed.instances[0].geometry_member="geometry\\other.psg";changed.instances[0].collision_member="geometry\\other.col";break;
        case 9:changed.instances[0].position[0]+=1;changed.instances[0].matrix[12]+=1;break;
        case 10:changed.instances[0].look[0]+=1;break;case 11:changed.instances[0].up[0]+=1;break;
        case 12:changed.instances[0].matrix[0]+=0.25;break;case 13:changed.instances[0].use_lighting=!changed.instances[0].use_lighting;break;
        case 14:changed.instances[0].matrix[0]=std::numeric_limits<double>::quiet_NaN();break;
        case 15:changed.portals[0].zones[0]=0;break;case 16:changed.portals[0].zones[0]=changed.portals[0].zones[1];break;
        case 17:changed.portals[0].vertices[0][0]+=1;break;case 18:changed.portals[0].section="portal999";break;
        case 19:changed.portals[0].parameter_block=0xffffffffu;break;case 20:changed.parameters.original_bytes[0]^=1;break;
        case 21:changed.parameters.blocks[0].fields[0].value="changed";break;default:changed.scene_info_block=0xffffffffu;break;}
        require(!buildWorldIndex49B(changed,target,error,&provenance)&&!error.empty()&&digest(target.view())==saved&&sameProvenance(provenance,expected),"typed invalid source rejected with both outputs atomic");++typedFailures;
    }
    World48F emptySource;require(!buildWorldIndex49B(emptySource,target,error,&provenance)&&digest(target.view())==saved&&sameProvenance(provenance,expected),"empty typed world never accepted as empty success");++typedFailures;
    const auto q=target.view().requirements();std::uint32_t maxFields=0,maxModels=0;for(const auto& b:world.parameters.blocks)maxFields=std::max(maxFields,std::uint32_t(b.fields.size()));for(const auto& z:world.zones)maxModels=std::max(maxModels,z.models);
    WorldIndexLimits49B exactLimits;exactLimits.asset_bytes=good.size();exactLimits.world.input_bytes=world.parameters.original_bytes.size();exactLimits.world.blocks=std::uint32_t(world.parameters.blocks.size());exactLimits.world.fields_per_block=maxFields;exactLimits.world.zones=std::uint32_t(q.zones);exactLimits.world.portals=std::uint32_t(q.portals);exactLimits.world.instances=std::uint32_t(q.placements);exactLimits.world.instances_per_zone=maxModels;exactLimits.resource_pairs=std::uint32_t(q.resource_pairs);exactLimits.text_bytes=q.text_bytes;exactLimits.payload_bytes=q.payload_bytes;
    OwnedWorldIndex49B equal;require(loadWorldIndex49B(good,equal,error,nullptr,exactLimits)&&digest(equal.view())==saved,"all limits inclusive exact equality");
    for(unsigned kind=0;kind<11;++kind){auto limits=exactLimits;switch(kind){case 0:--limits.asset_bytes;break;case 1:--limits.world.input_bytes;break;case 2:--limits.world.blocks;break;case 3:--limits.world.fields_per_block;break;case 4:--limits.world.zones;break;case 5:--limits.world.portals;break;case 6:--limits.world.instances;break;case 7:--limits.world.instances_per_zone;break;case 8:--limits.resource_pairs;break;case 9:--limits.text_bytes;break;default:--limits.payload_bytes;break;}
        require(!loadWorldIndex49B(good,target,error,&provenance,limits)&&digest(target.view())==saved&&sameProvenance(provenance,expected),"strictly smaller load limit fails atomically");
        require(!buildWorldIndex49B(world,target,error,&provenance,limits)&&digest(target.view())==saved&&sameProvenance(provenance,expected),"strictly smaller build limit fails atomically");
    }
}
struct LoadMetric {std::size_t live=0,peak=0,provenanceLive=0;std::uint64_t calls=0;double ms=0,minMs=0,maxMs=0;};
LoadMetric measureLoadOnce(const Bytes& bytes,unsigned mode){
    LoadMetric m;std::string error;heap49b::begin();const auto calls=heap49b::calls;const auto start=Clock::now();
    {World48F full;OwnedWorldIndex49B compact;WorldIndexProvenance49B provenance;
        const bool ok=mode==0?parseWorldAsset48F(bytes,full,error):loadWorldIndex49B(bytes,compact,error,mode==2?&provenance:nullptr);
        const auto stop=Clock::now();require(ok,"measured load success");m.ms=std::chrono::duration<double,std::milli>(stop-start).count();m.live=heap49b::live;m.peak=heap49b::peak;m.calls=heap49b::calls-calls;
        if(mode){require(compact.allocatedPayloadBytes()+provenance.zone_parameter_blocks.capacity()*4+provenance.portal_parameter_blocks.capacity()*4+provenance.placement_parameter_blocks.capacity()*4+provenance.placement_source_block_offsets.capacity()*4==m.live,"reported vector capacities equal actual retained requests");m.provenanceLive=m.live-compact.allocatedPayloadBytes();}
    }
    require(heap49b::live==0,"load measurement objects freed");heap49b::end();return m;
}
LoadMetric measureLoad(const Bytes& bytes,unsigned mode){
    std::array<double,5> times{};LoadMetric result;
    for(unsigned trial=0;trial<times.size();++trial){const auto m=measureLoadOnce(bytes,mode);times[trial]=m.ms;
        if(trial==0)result=m;else require(result.live==m.live&&result.peak==m.peak&&result.provenanceLive==m.provenanceLive&&result.calls==m.calls,"allocation measurements deterministic across five serial trials");
    }
    std::sort(times.begin(),times.end());result.ms=times[2];result.minMs=times.front();result.maxMs=times.back();return result;
}
std::uint64_t queryFull(const World48F& w,unsigned iterations){
    std::uint64_t h=0;
    for(unsigned i=0;i<iterations;++i){const auto zone=std::uint32_t(i%w.zones.size()+1);const auto& wanted=w.instances[(std::size_t(i)*7919)%w.instances.size()];
        for(const auto& z:w.zones)if(z.zone==zone){h=mix(h,z.models);break;}
        for(const auto& p:w.instances)if(p.zone==zone)h=mix(h,p.ordinal);
        for(std::size_t id=0;id<w.portals.size();++id)for(auto endpoint:w.portals[id].zones)if(endpoint==zone)h=mix(h,id);
        const WorldInstance48F* placement=nullptr;for(const auto& p:w.instances)if(p.zone==wanted.zone&&p.ordinal==wanted.ordinal){placement=&p;break;}
        h=hashValues(h,placement->matrix);h=hashText(h,placement->geometry_member);h=hashText(h,placement->collision_member);
        const auto& portal=w.portals[i%w.portals.size()];h=mix(h,portal.zones[0]);h=mix(h,portal.zones[1]);for(const auto& xyz:portal.vertices)h=hashValues(h,xyz);
    }return h;
}
std::uint64_t queryCompact(const World48F& selector,WorldIndexView49B v,unsigned iterations){
    std::uint64_t h=0;
    for(unsigned i=0;i<iterations;++i){const auto zone=std::uint32_t(i%v.zones().size+1);const auto& wanted=selector.instances[(std::size_t(i)*7919)%selector.instances.size()];
        h=mix(h,v.zone(zone)->placement_count);for(const auto& p:v.zonePlacements(zone))h=mix(h,p.ordinal);for(auto id:v.zoneIncidentPortals(zone))h=mix(h,id);
        const auto* p=v.placement(wanted.zone,wanted.ordinal);WorldResourceText49B text;const bool ok=v.resourceText(p->resource_pair,text);if(!ok)return ~h;
        h=hashValues(h,p->matrix);h=hashText(h,text.geometry_member);h=hashText(h,text.collision_member);
        const auto* portal=v.portalById(std::uint32_t(i%v.portals().size));h=mix(h,portal->zones[0]);h=mix(h,portal->zones[1]);for(const auto& xyz:portal->vertices)h=hashValues(h,xyz);
    }return h;
}
}

int main(int argc,char** argv){try{
    if(argc!=2){std::cerr<<"Usage: world_index49b REPOSITORY\n";return 2;}
    const fs::path root=fs::u8path(argv[1]);const auto fixtureRoot=root/"evidence/nativePc48K-build-20261003-174328-460";
    const std::array<const char*,12> names{{"ga","gb","gc","ma","mb","ra","rc","sa","sc","ta","tb","tc"}};
    std::size_t zones=0,placements=0,portals=0,incidents=0,pairs=0,textBytes=0,inputBytes=0,originalBytes=0;
    std::size_t oldLive=0,newLive=0,auditLive=0,maxOldPeak=0,maxNewPeak=0,maxAuditPeak=0;
    std::cout<<"MEASUREMENT_POLICY requested C++ new/new[] bytes; excludes preloaded RRWORLD1 input, stack, allocation headers/alignment, allocator metadata, unrelated preexisting allocations and RSS. Loads serial; individual fresh-load peak includes importer/staging overlap. This does not measure the Windows typed-build assembly path or replacement of an existing RuntimeWorld. Reduced acceptance limits do not cap the preceding parser allocation. Optional source IDs separately retained. Metadata adds retained functionality; existing renderer previously discarded World48F, so this is not a current-renderer RAM saving. Query baseline is explicit unindexed World48F zone/placement/incidence scans plus direct portal and resource access. Load and query timings are medians of five serial trials; query order alternates. No gameplay/FPS/GPU claim.\n";
    for(const auto* name:names){
        const auto file=std::string(name)+"-tracks_"+name+".rrworld";
        auto bytes=read(fixtureRoot/"x64-world-fixtures"/file);require(bytes==read(fixtureRoot/"x86-world-fixtures"/file),"x64/x86 fixtures identical, not an x86 executable test");
        World48F w;std::string error;require(parseWorldAsset48F(bytes,w,error),"unchanged production RRWORLD1 parser");Bytes canonical;require(encodeWorldAsset48F(w,canonical,error)&&canonical==bytes,"RRWORLD1 byte-identity roundtrip");
        World48F decoded;DecodeError48C decodeError;require(decodeWorld48F(w.parameters.original_bytes,decoded,decodeError),"embedded original SCB production decode");Bytes recoded;require(encodeWorldAsset48F(decoded,recoded,error)&&recoded==bytes,"embedded original canonical identity");
        OwnedWorldIndex49B compact,direct;WorldIndexProvenance49B provenance,directProvenance;
        require(loadWorldIndex49B(bytes,compact,error,&provenance)&&buildWorldIndex49B(w,direct,error,&directProvenance),"production load and typed-build frontdoors");
        compare(w,compact.view(),&provenance);compare(decoded,direct.view(),&directProvenance);boundaries(compact.view());
        const auto q=compact.view().requirements();zones+=q.zones;placements+=q.placements;portals+=q.portals;incidents+=q.incident_portal_ids;pairs+=q.resource_pairs;textBytes+=q.text_bytes;inputBytes+=bytes.size();originalBytes+=w.parameters.original_bytes.size();
        const auto full=measureLoad(bytes,0),slim=measureLoad(bytes,1),audit=measureLoad(bytes,2);oldLive+=full.live;newLive+=slim.live;auditLive+=audit.provenanceLive;maxOldPeak=std::max(maxOldPeak,full.peak);maxNewPeak=std::max(maxNewPeak,slim.peak);maxAuditPeak=std::max(maxAuditPeak,audit.peak);
        const unsigned repeats=4096;const auto allocationCalls=heap49b::calls;std::array<double,5> oldTimes{},compactTimes{};
        for(unsigned trial=0;trial<oldTimes.size();++trial){std::uint64_t oldHash=0,newHash=0;
            auto oldRun=[&]{const auto start=Clock::now();oldHash=queryFull(w,repeats);oldTimes[trial]=std::chrono::duration<double,std::milli>(Clock::now()-start).count();};
            auto compactRun=[&]{const auto start=Clock::now();newHash=queryCompact(w,compact.view(),repeats);compactTimes[trial]=std::chrono::duration<double,std::milli>(Clock::now()-start).count();};
            if(trial%2){compactRun();oldRun();}else{oldRun();compactRun();}
            require(oldHash==newHash,"equivalent actual query-workload checksum");benchmarkSink=newHash;
        }
        require(heap49b::calls==allocationCalls,"old and compact measured queries no allocations");std::sort(oldTimes.begin(),oldTimes.end());std::sort(compactTimes.begin(),compactTimes.end());
        std::cout<<"WORLD "<<file<<" zones="<<q.zones<<" placements="<<q.placements<<" portals="<<q.portals<<" pairs="<<q.resource_pairs<<" text_bytes="<<q.text_bytes<<" payload_bytes="<<q.payload_bytes<<" old_live="<<full.live<<" old_peak="<<full.peak<<" compact_live="<<slim.live<<" compact_peak="<<slim.peak<<" optional_provenance_live="<<audit.provenanceLive<<" with_provenance_peak="<<audit.peak<<" old_load_ms="<<full.ms<<" compact_load_ms="<<slim.ms<<" with_provenance_load_ms="<<audit.ms<<" old_load_allocations="<<full.calls<<" compact_load_allocations="<<slim.calls<<" query_iterations="<<repeats<<" old_queries_ms="<<oldTimes[2]<<" compact_queries_ms="<<compactTimes[2]<<" old_queries_min_ms="<<oldTimes.front()<<" old_queries_max_ms="<<oldTimes.back()<<" compact_queries_min_ms="<<compactTimes.front()<<" compact_queries_max_ms="<<compactTimes.back()<<" old_load_min_ms="<<full.minMs<<" old_load_max_ms="<<full.maxMs<<" compact_load_min_ms="<<slim.minMs<<" compact_load_max_ms="<<slim.maxMs<<'\n';
        Storage borrowed(q);WorldIndexView49B copied;WorldIndexCopyError49B copyError;const auto expected=digest(compact.view());
        require(copyWorldIndex49B(compact.view(),borrowed.spans(),copied,copyError),"all-corpus caller-owned copy");ownsStorage(copied,borrowed.spans(),q);compare(w,copied);
        if(std::string_view(name)=="ta")badInputTests(bytes,w);
        w={};decoded={};bytes={};bytes.shrink_to_fit();canonical={};canonical.shrink_to_fit();recoded={};recoded.shrink_to_fit();direct={};provenance={};directProvenance={};
        require(digest(compact.view())==expected,"owner queries after parsed worlds and original bytes destroyed");
        auto independent=compact;require(independent.view().zones().data!=compact.view().zones().data&&independent.view().placements().data!=compact.view().placements().data&&independent.view().portals().data!=compact.view().portals().data&&independent.view().resourcePairs().data!=compact.view().resourcePairs().data&&independent.view().text().data!=compact.view().text().data&&independent.view().incidentPortalIds().data!=compact.view().incidentPortalIds().data,"copied owner arrays have independent addresses");compact={};require(digest(independent.view())==expected,"copied owner independent lifetime");independent={};require(digest(copied)==expected,"caller view after all source owners destroyed");boundaries(copied);
    }
    require(zones==186&&placements==15340&&portals==206&&incidents==412&&pairs==7180&&textBytes==357608&&inputBytes==15945212&&originalBytes==6366352,"authentic corpus exact independently audited totals");
    auto analytic=synthetic(true);OwnedWorldIndex49B small;std::string error;require(buildWorldIndex49B(analytic,small,error),"analytical compact fixture");compare(analytic,small.view());require(bits(small.view().placements()[0].position[0])==0x8000000000000000ull,"signed negative zero preserved");require(bits(small.view().placements()[1].position[0])==bits(1.401298464324817e-45),"binary32 subnormal represented in binary64 preserved");copyTests(small.view());
    auto emptyWorld=synthetic(false);OwnedWorldIndex49B empty;require(buildWorldIndex49B(emptyWorld,empty,error),"valid zone with no placements or portals");compare(emptyWorld,empty.view());Storage emptyStorage(empty.view().requirements());WorldIndexView49B emptyCopy;WorldIndexCopyError49B emptyError;require(copyWorldIndex49B(empty.view(),emptyStorage.spans(),emptyCopy,emptyError)&&emptyCopy.zone(1)&&emptyCopy.zonePlacements(1).empty()&&emptyCopy.zoneIncidentPortals(1).empty(),"valid zero-sized arrays allow null storage");
    boundaries(WorldIndexView49B{});
    std::cout<<"TOTAL worlds=12 architecture_bits="<<8*sizeof(void*)<<" zones="<<zones<<" placements="<<placements<<" portals="<<portals<<" incidences="<<incidents<<" pairs="<<pairs<<" text_bytes="<<textBytes<<" old_retained_sum="<<oldLive<<" compact_retained_sum="<<newLive<<" optional_provenance_sum="<<auditLive<<" max_individual_old_load_peak="<<maxOldPeak<<" max_individual_compact_load_peak="<<maxNewPeak<<" max_individual_with_provenance_peak="<<maxAuditPeak<<" compared_binary64_values="<<comparedDoubles<<" malformed_cases="<<malformedCases<<" typed_invalid_cases="<<typedFailures<<" copy_failures="<<copyFailures<<" checks="<<checks<<" status=PASS\n";
    return 0;
}catch(const std::exception& e){heap49b::end();std::cerr<<"FAIL world_index49b: "<<e.what()<<" checks="<<checks<<'\n';return 1;}}
