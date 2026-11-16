#pragma once
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/game/track_navigation_view50tc.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace tc_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace storage=g::navigation_storage_detail50TC;
inline constexpr std::size_t archiveBytes=3607868,decodedBytes=16048128;
inline constexpr std::uint32_t archiveEntries=509,zoneCount=25,portalCount=26,mapNodeCount=268,aiNodeCount=293;
inline constexpr const char* archiveSha="7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\tc.scb",98304,232284,"795bbbf9882baf233b9d67f777d0bb1e36506dda0f1ad7e71a303faae3993d24"},
 {"tracks\\mtc.spl",10960896,55327,"3de93f96583cc7e06a901f172f68d192ff0f43dd7c925924417048eff9e1d6d6"},
 {"tracks\\Atc.spl",10887168,60231,"567e7d0711f34a82b7b4f2e927301ff836294785b9942c9077b7d8bd40d626c4"},
 {"tracks\\tc.tob",0,25512,"e080f96e2657e1331ac1f829faa880a5e984ebf1c5fab393daa6539534dd0573"},
 {"tracks\\tc.tun",16023552,976,"f2a0779f27374d9006629ff6fd09e55ae97da931ed5a6bcb95368fafe02a94f1"}
}};
enum class Phase : std::uint8_t {Empty,Authenticate,Archive,World,Tob,MapGates,MapMetadata,AiGates,AiMetadata,Tuning,Ready,Closed};
struct Diagnostic {
    Phase phase=Phase::Empty;
    std::string message;
    bool mapGatesCommitted=false,aiGatesCommitted=false;
    std::size_t retainedPayloadBytes=0,requestedPayloadBytes=0;
    a::DecodeError48C decode{};
    g::TrackNavigationDiagnostic50EU navigation{};
    a::AiAuthoringDiagnostic50IU metadata{};
};
// One authentic source identity. It owns no final index/world lease, so the
// aggregate may retain it without a navigation -> world -> source cycle.
class Owner final : public std::enable_shared_from_this<Owner> {
    friend class ::tc_navigation50TC::Owner;
    a::ResArchive48C archive_;
    a::World48F world_;
    a::ParameterDocument48F tob_;
    g::AiTrackTuning50IU tuning_{};
    std::shared_ptr<storage::Storage> navigation_;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    bool attempted_=false,ready_=false;
    Phase phase_=Phase::Empty;
    static std::string canonical(std::string s){for(auto& c:s){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c+32);}return s;}
    static void add(std::size_t& n,std::size_t amount) noexcept {
        if(amount>std::numeric_limits<std::size_t>::max()-n)n=std::numeric_limits<std::size_t>::max();else n+=amount;
    }
    template<class T> static void vectorPayload(std::size_t& n,const std::vector<T>& v) noexcept {
        if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();
        else add(n,v.capacity()*sizeof(T));
    }
    static void textPayload(std::size_t& n,const std::string& s) noexcept {add(n,s.capacity());add(n,1);}
    static void parametersPayload(std::size_t& n,const a::ParameterDocument48F& p) noexcept {
        vectorPayload(n,p.original_bytes);vectorPayload(n,p.blocks);
        for(const auto& b:p.blocks){textPayload(n,b.name);vectorPayload(n,b.fields);
            for(const auto& f:b.fields){textPayload(n,f.name);textPayload(n,f.value);}}
    }
    void requireOwnerThread() const noexcept {if(ownerThread_!=std::this_thread::get_id())std::terminate();}
    bool fail(Diagnostic& d,const char* message) {
        d.phase=phase_;d.message=message;d.retainedPayloadBytes=retainedPayloadBytes();return false;
    }
    bool budget(std::size_t available,std::size_t extra,Diagnostic& d) const noexcept {
        d.retainedPayloadBytes=retainedPayloadBytes();d.requestedPayloadBytes=d.retainedPayloadBytes;
        add(d.requestedPayloadBytes,extra);
        return d.requestedPayloadBytes<=available;
    }
    bool member(std::size_t index,std::vector<std::uint8_t>& bytes,Diagnostic& d) const {
        const auto& pin=sources[index];const a::ResourceEntry48C* match=nullptr;
        for(const auto& entry:archive_.entries)if(canonical(entry.name)==canonical(pin.name)){
            if(match)return false;match=&entry;}
        // Found by name; the stock offset/size/SHA is a receipt (common rule).
        if(!match||!a::extractResMember48C(archive_,match->name,bytes,d.decode,match->bytes)||bytes.size()!=match->bytes)return false;
        course_archive_receipt50CI::memberReceipt("tc_cold_sources",pin.name,bytes,pin.offset,match->logical_offset,pin.bytes,pin.sha);
        return true;
    }
    static bool vectorField(const a::ParameterBlock48F& block,const char* key,g::Vector3_48C& out){
        const std::string* value=nullptr;
        for(const auto& field:block.fields)if(field.name==key){if(value)return false;value=&field.value;}
        if(!value)return false;std::size_t begin=0;
        for(unsigned axis=0;axis<3;++axis){auto end=value->find(',',begin);
            if(axis<2&&end==std::string::npos)return false;
            if(axis==2&&end!=std::string::npos)return false;
            if(end==std::string::npos)end=value->size();
            auto first=begin,last=end;while(first<last&&(*value)[first]==' ')++first;
            while(last>first&&(*value)[last-1]==' ')--last;
            if(!a::parseTrackNumber50AD(value->data()+first,last-first,out.v[axis]))return false;
            begin=end+1;}
        return begin==value->size()+1;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner(){if(ownerThread_!=std::this_thread::get_id())std::terminate();}
    bool ready() const noexcept {return ownerThread_==std::this_thread::get_id()&&ready_;}
    Phase phase() const noexcept {return ownerThread_==std::this_thread::get_id()?phase_:Phase::Empty;}
    // Element/capacity accounting, deliberately conservative for inline string
    // storage. Not allocator/control-block or hardware-memory parity accounting.
    std::size_t retainedPayloadBytes() const noexcept {
        if(ownerThread_!=std::this_thread::get_id())return 0;
        std::size_t n=sizeof(Owner);vectorPayload(n,archive_.decoded);vectorPayload(n,archive_.entries);
        vectorPayload(n,archive_.blocks);for(const auto& e:archive_.entries)textPayload(n,e.name);
        parametersPayload(n,world_.parameters);vectorPayload(n,world_.zones);vectorPayload(n,world_.portals);
        for(const auto& p:world_.portals)textPayload(n,p.section);
        vectorPayload(n,world_.instances);for(const auto& i:world_.instances){textPayload(n,i.section);textPayload(n,i.geometry_member);textPayload(n,i.collision_member);}
        parametersPayload(n,tob_);if(navigation_)add(n,sizeof(storage::Storage));return n;
    }
    static constexpr std::size_t gateScratchBytes() noexcept {
        return sizeof(storage::Scratch<g::courseNavigationCapacity50MB>)+sizeof(storage::Metadata<g::courseNavigationCapacity50MB>)+g::courseNavigationCapacity50MB;
    }
    static constexpr std::size_t navigationPayloadBytes() noexcept {return sizeof(storage::Storage);}
    // Actual native decode hard ceilings stay unchanged. Phase reservations
    // precede transient extraction/gate allocation; actual retained capacities
    // are checked after native transactional decodes and before publication.
    bool decode(const std::vector<std::uint8_t>& original,g::SourceLibmState49T& actualLibm,
        std::size_t availableCpuPayloadBytes,Diagnostic& d){
        if(ownerThread_!=std::this_thread::get_id()){d={};d.message="TC cold owner thread mismatch";return false;}
        if(attempted_){d.message="TC cold source preparation is one-attempt";d.phase=phase_;return false;}
        attempted_=true;d={};phase_=Phase::Authenticate;
        try{
            // Common rule (course_archive_receipt50ci.h): the stock identity,
            // entry count and decoded size are receipts; shared decode limits.
            course_archive_receipt50CI::receipt("tc_cold_sources","TC",original,archiveSha,archiveBytes);
            if(!budget(availableCpuPayloadBytes,decodedBytes+sizeof(storage::Storage)+gateScratchBytes(),d))
                return fail(d,"TC source/navigation reservation exceeds existing remaining CPU budget");
            phase_=Phase::Archive;
            if(!a::decodeResource48C(original,archive_,d.decode,nullptr,course_archive_receipt50CI::limits())||archive_.entries.empty())
                return fail(d,"TC archive framing rejected");
            course_archive_receipt50CI::countReceipt("tc_cold_sources","entries",archive_.entries.size(),archiveEntries);
            course_archive_receipt50CI::countReceipt("tc_cold_sources","decoded_bytes",archive_.decoded.size(),decodedBytes);
            if(!budget(availableCpuPayloadBytes,sizeof(storage::Storage)+gateScratchBytes(),d))
                return fail(d,"TC decoded archive plus navigation exceeds remaining CPU budget");
            {
                std::set<std::string> names;
                for(const auto& e:archive_.entries)if(!names.insert(canonical(e.name)).second)return fail(d,"TC ambiguous member identity");
            }
            navigation_.reset(new storage::Storage(this));
            phase_=Phase::World;
            {
                if(!budget(availableCpuPayloadBytes,sources[0].bytes,d))return fail(d,"TC SCB extraction exceeds remaining CPU budget");
                std::vector<std::uint8_t> raw;
                if(!member(0,raw,d)||!a::decodeWorld48F(raw,world_,d.decode)||world_.zones.empty()||world_.zones.size()>256||
                   world_.portals.size()>g::courseNavigationPortalCapacity50MB)
                    return fail(d,"TC SCB topology decode rejected or beyond the shared navigation capacity");
                course_archive_receipt50CI::countReceipt("tc_cold_sources","zones",world_.zones.size(),zoneCount);
                course_archive_receipt50CI::countReceipt("tc_cold_sources","portals",world_.portals.size(),portalCount);
                for(std::size_t i=0;i<world_.zones.size();++i)if(world_.zones[i].zone!=i+1)return fail(d,"TC SCB zone order rejected");
                navigation_->authoredZoneCount=static_cast<std::uint32_t>(world_.zones.size());
                navigation_->authoredPortalCount=static_cast<std::uint32_t>(world_.portals.size());
                for(std::size_t i=0;i<world_.portals.size();++i){const auto& p=world_.portals[i];
                    if(p.parameter_block>=world_.parameters.blocks.size())return fail(d,"TC portal source block invalid");
                    auto& target=navigation_->authoredPortals[i];target.zoneA=p.zones[0];target.zoneB=p.zones[1];
                    for(unsigned axis=0;axis<4;++axis)if(!vectorField(world_.parameters.blocks[p.parameter_block],
                        ("vertex"+std::to_string(axis+1)).c_str(),target.vertices[axis]))return fail(d,"TC portal source binary32 vertex rejected");}
            }
            phase_=Phase::Tob;
            {
                if(!budget(availableCpuPayloadBytes,sources[3].bytes,d))return fail(d,"TC TOB extraction exceeds remaining CPU budget");
                std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.retainDuplicateLabels=true;
                // Representation only: preserve every original sorted field,
                // including model3 duplicates. No winning-value policy here.
                if(!member(3,raw,d)||!a::decodeParameters48F(raw,tob_,d.decode,nullptr,options))return fail(d,"TC original TOB retained-label decode rejected");
            }
            phase_=Phase::MapGates;
            {
                if(!budget(availableCpuPayloadBytes,sources[1].bytes+gateScratchBytes(),d))return fail(d,"TC MAP gate reservation exceeds remaining CPU budget");
                std::vector<std::uint8_t> raw;if(!member(1,raw,d))return fail(d,"TC full MAP member authentication rejected");
                {
                    auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
                    if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->map,d.navigation))return fail(d,"TC MAP gates rejected");
                    d.mapGatesCommitted=true;
                    course_archive_receipt50CI::countReceipt("tc_cold_sources","map_nodes",navigation_->map.track.count,mapNodeCount);
                    std::copy(std::begin(scratch->fieldMask),std::end(scratch->fieldMask),navigation_->mapFieldMask.begin());
                }
                phase_=Phase::MapMetadata;
                if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),
                    navigation_->map,navigation_->mapMetadata,d.metadata))return fail(d,"TC full268 MAP metadata rejected");
            }
            phase_=Phase::AiGates;
            {
                if(!budget(availableCpuPayloadBytes,sources[2].bytes+gateScratchBytes(),d))return fail(d,"TC AI gate reservation exceeds remaining CPU budget");
                std::vector<std::uint8_t> raw;if(!member(2,raw,d))return fail(d,"TC full AI member authentication rejected");
                {
                    auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
                    if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->ai,d.navigation))return fail(d,"TC AI gates rejected");
                    d.aiGatesCommitted=true;
                    course_archive_receipt50CI::countReceipt("tc_cold_sources","ai_nodes",navigation_->ai.track.count,aiNodeCount);
                }
                phase_=Phase::AiMetadata;
                if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),
                    navigation_->ai,navigation_->aiMetadata,d.metadata))return fail(d,"TC full293 AI metadata rejected");
            }
            phase_=Phase::Tuning;
            {
                if(!budget(availableCpuPayloadBytes,sources[4].bytes,d))return fail(d,"TC tuning extraction exceeds remaining CPU budget");
                std::vector<std::uint8_t> raw;
                if(!member(4,raw,d)||!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),tuning_,d.metadata))
                    return fail(d,"TC original AI tuning rejected");
            }
            if(!budget(availableCpuPayloadBytes,0,d))return fail(d,"TC retained source payload exceeds remaining CPU budget");
            navigation_->gatesReady=true;ready_=true;phase_=Phase::Ready;d.phase=phase_;return true;
        }catch(const std::exception& e){return fail(d,e.what());}
        catch(...){return fail(d,"TC cold source exception; reached prefix retained");}
    }
    // Stable owner-backed references; all clients require ready() first. Failed
    // private prefixes do not become a source admission receipt.
    const a::ResArchive48C& archive() const noexcept {requireOwnerThread();return archive_;}
    const a::World48F& world() const noexcept {requireOwnerThread();return world_;}
    const std::vector<std::uint8_t>& originalTob() const noexcept {requireOwnerThread();return tob_.original_bytes;}
    const a::ParameterDocument48F& triggerParameters() const noexcept {requireOwnerThread();return tob_;}
    const g::AiTrackTuning50IU& aiTuning() const noexcept {requireOwnerThread();return tuning_;}
    const std::array<std::uint32_t,g::courseNavigationCapacity50MB>& mapFieldMask() const noexcept {
        requireOwnerThread();static const std::array<std::uint32_t,g::courseNavigationCapacity50MB> empty{};return navigation_?navigation_->mapFieldMask:empty;
    }
    g::TrackNavigationView50TC map() const noexcept {
        auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50TC{std::move(self),navigation_,{},g::NavigationRole50TC::Map}:g::TrackNavigationView50TC{};
    }
    g::TrackNavigationView50TC ai() const noexcept {
        auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50TC{std::move(self),navigation_,{},g::NavigationRole50TC::Ai}:g::TrackNavigationView50TC{};
    }
    g::AiTrackMetadataView50TC mapMetadata() const noexcept {return g::AiTrackMetadataView50TC{map()};}
    g::AiTrackMetadataView50TC aiMetadata() const noexcept {return g::AiTrackMetadataView50TC{ai()};}
    bool close() noexcept {
        if(ownerThread_!=std::this_thread::get_id()||weak_from_this().use_count()>1||
           (navigation_&&navigation_.use_count()!=1))return false;
        ready_=false;navigation_.reset();archive_={};world_={};tob_={};tuning_={};attempted_=true;phase_=Phase::Closed;return true;
    }
};
}
