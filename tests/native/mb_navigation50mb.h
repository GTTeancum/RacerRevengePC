#pragma once
#include "mb_original_world50lm.h"
#include "course_archive_receipt50ci.h"
#include "course_navigation_topology50nt.h"
#include "../../src/game/track_navigation_view50mb.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include "../../src/assets/ai_authoring50iu.h"
#include <cstring>
#include <memory>
#include <string>

namespace mb_navigation50MB {
namespace g=rr::game;namespace a=rr::assets;
namespace storage=g::navigation_storage_detail50MB;
enum class Phase : std::uint8_t {
    Empty,Authenticate,MapGates,MapMetadata,AiGates,AiMetadata,GatesReady,
    Topology,MapIndices,AiIndices,RuntimeReady,Closed
};
struct Diagnostic {
    Phase phase=Phase::Empty;
    std::string message;
    g::TrackNavigationDiagnostic50EU navigation{};
    a::AiAuthoringDiagnostic50IU metadata{};
};
// Single-attempt cold owner. Per-track reader publication is transactional;
// successfully reached earlier tracks have already committed their libm state.
// A later failure retains the exact reached phase and cannot be replayed.
class Owner {
    std::shared_ptr<storage::Storage> storage_;
    std::shared_ptr<storage::Indices> indices_;
    bool gateAttempted_=false,bindAttempted_=false;
    Phase phase_=Phase::Empty;
    static bool vectorField(const a::ParameterBlock48F& block,const char* key,g::Vector3_48C& out){
        const std::string* value=nullptr;
        for(const auto& f:block.fields)if(f.name==key){if(value)return false;value=&f.value;}
        if(!value)return false;std::size_t begin=0;
        for(unsigned axis=0;axis<3;++axis){
            auto end=value->find(',',begin);
            if(axis<2&&end==std::string::npos)return false;
            if(axis==2&&end!=std::string::npos)return false;
            if(end==std::string::npos)end=value->size();
            auto first=begin,last=end;while(first<last&&(*value)[first]==' ')++first;
            while(last>first&&(*value)[last-1]==' ')--last;
            if(!a::parseTrackNumber50AD(value->data()+first,last-first,out.v[axis]))return false;
            begin=end+1;
        }
        return begin==value->size()+1;
    }
    bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;return false;}
    bool topologyMatches(const g::LiveZoneWorld50EN& actual,Diagnostic& d) const {
        // One rule (course_navigation_topology50nt.h): the SCB's own zone and
        // portal counts, read at prepareGates.
        (void)d;return course_navigation_topology50NT::matches(storage_->authoredPortals,
            storage_->authoredPortalCount,storage_->authoredZoneCount,actual);
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    // Native element payload, not allocator/control-block bookkeeping. Charge
    // this against the existing session CPU budget before prepareGates.
    static constexpr std::size_t persistentPayloadBytes() noexcept {
        return sizeof(storage::Storage)+sizeof(storage::Indices);
    }
    static constexpr std::size_t coldScratchBytes() noexcept {return sizeof(storage::Scratch<g::courseNavigationCapacity50MB>);}
    static constexpr std::size_t ownerBytes() noexcept {return sizeof(Owner);}
    Phase phase() const noexcept {return phase_;}
    bool gatesReady() const noexcept {return storage_&&storage_->gatesReady;}
    bool runtimeReady() const noexcept {return gatesReady()&&indices_&&indices_->world.valid();}
    bool prepareGates(const mb_original_world50LM::Owner& source,g::SourceLibmState49T& libm,
        std::size_t availableCpuPayloadBytes,Diagnostic& d){
        d={};if(gateAttempted_)return fail(d,"MB navigation gate preparation is one-attempt");
        gateAttempted_=true;phase_=Phase::Authenticate;
        if(availableCpuPayloadBytes<persistentPayloadBytes())return fail(d,"MB navigation exceeds remaining session CPU payload budget");
        const auto* archive=source.archive();
        if(!source.loaded()||!archive||archive->entries.empty())
            return fail(d,"actual loaded MB archive owner required");
        // Common course rule (course_archive_receipt50ci.h): the stock archive
        // extent and the three stock member pins are receipts; members by name.
        course_archive_receipt50CI::countReceipt("mb_navigation","entries",archive->entries.size(),487);
        try {
            a::DecodeError48C error;std::vector<std::uint8_t> map,ai,scene;
            const auto extract=[&](const char* member,std::size_t bytes,const char* hash,std::vector<std::uint8_t>& out){
                if(!a::extractResMember48C(*archive,member,out,error,course_archive_receipt50CI::maximumArchiveBytes())||out.empty())return false;
                course_archive_receipt50CI::memberReceipt("mb_navigation",member,out,0,0,bytes,hash);return true;
            };
            if(!extract("tracks\\mmb.spl",56754,"09319ed7e51176fc80ff6597ff3e62f85137ccb221c4290d53e01d30a9a432f4",map)||
               !extract("tracks\\amb.spl",60710,"8e931fc08aa7cb4cb5a8922ac5b177307f6bea51ed2a7a86d74e2adb4fc9af19",ai)||
               !extract("tracks\\mb.scb",mb_original_world50LM::scbBytesCount,mb_original_world50LM::scbSha,scene))
                return fail(d,"MB navigation M/A spline or SCB member missing");
            storage_.reset(new storage::Storage);
            {
                a::World48F world;
                if(!a::decodeWorld48F(scene,world,error)||world.zones.empty()||world.zones.size()>256||
                   world.portals.size()>g::courseNavigationPortalCapacity50MB)
                    return fail(d,"MB navigation SCB topology decode failed or exceeds the shared navigation capacity");
                course_archive_receipt50CI::countReceipt("mb_navigation","zones",world.zones.size(),16);
                course_archive_receipt50CI::countReceipt("mb_navigation","portals",world.portals.size(),17);
                for(std::size_t i=0;i<world.zones.size();++i)if(world.zones[i].zone!=i+1)
                    return fail(d,"MB navigation SCB zone order rejected");
                storage_->authoredZoneCount=static_cast<std::uint32_t>(world.zones.size());
                storage_->authoredPortalCount=static_cast<std::uint32_t>(world.portals.size());
                for(std::size_t i=0;i<world.portals.size();++i){const auto& p=world.portals[i];
                    if(p.parameter_block>=world.parameters.blocks.size())return fail(d,"MB portal block invalid");
                    auto& out=storage_->authoredPortals[i];out.zoneA=p.zones[0];out.zoneB=p.zones[1];
                    for(unsigned k=0;k<4;++k)if(!vectorField(world.parameters.blocks[p.parameter_block],
                        ("vertex"+std::to_string(k+1)).c_str(),out.vertices[k]))
                        return fail(d,"MB source portal binary32 vertex rejected");
                }
            }
            scene.clear();scene.shrink_to_fit();
            phase_=Phase::MapGates;
            {
                auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
                if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(map.data()),map.size(),
                    libm,*scratch,storage_->map,d.navigation))
                    return fail(d,"MB map gate preparation failed");
                course_archive_receipt50CI::countReceipt("mb_navigation","map_nodes",storage_->map.track.count,265);
            }
            phase_=Phase::MapMetadata;
            if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(map.data()),map.size(),
                storage_->map,storage_->mapMetadata,d.metadata))return fail(d,"MB full map metadata failed");
            map.clear();map.shrink_to_fit();
            phase_=Phase::AiGates;
            {
                auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
                if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(ai.data()),ai.size(),
                    libm,*scratch,storage_->ai,d.navigation))
                    return fail(d,"MB AI gate preparation failed");
                course_archive_receipt50CI::countReceipt("mb_navigation","ai_nodes",storage_->ai.track.count,292);
            }
            phase_=Phase::AiMetadata;
            if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(ai.data()),ai.size(),
                storage_->ai,storage_->aiMetadata,d.metadata))return fail(d,"MB full AI metadata failed");
            storage_->gatesReady=true;phase_=Phase::GatesReady;d.phase=phase_;return true;
        } catch(const std::exception& e){return fail(d,e.what());}
    }
    bool bindWorld(const g::MbCollisionWorldLease& lease,Diagnostic& d){
        d={};if(!gatesReady()||bindAttempted_)return fail(d,"MB world binding requires one fresh gates-ready owner");
        bindAttempted_=true;phase_=Phase::Topology;
        if(!lease.valid()||!topologyMatches(*lease.world(),d))return fail(d,"actual retained MB live-world ordered topology mismatch");
        try {
            auto next=std::shared_ptr<storage::Indices>(new storage::Indices);next->world=lease;
            struct Graph {const g::UndertrackNode50ET* nodes;std::uint32_t count;};
            struct Navigation {Graph track;const g::TrackNavigationNode50EU* nodes;
                std::uint8_t* nodeIndex78;bool gatesPrepared,indicesPrepared;};
            Navigation map{{storage_->map.track.nodes,storage_->map.track.count},storage_->map.nodes,next->map,true,false};
            Navigation ai{{storage_->ai.track.nodes,storage_->ai.track.count},storage_->ai.nodes,next->ai,true,false};
            phase_=Phase::MapIndices;
            if(!g::navigation_detail50MB::bind<g::courseNavigationCapacity50MB>(*lease.world(),map,d.navigation))return fail(d,"MB map zone indices failed");
            phase_=Phase::AiIndices;
            if(!g::navigation_detail50MB::bind<g::courseNavigationCapacity50MB>(*lease.world(),ai,d.navigation))return fail(d,"MB AI zone indices failed");
            indices_=std::move(next);phase_=Phase::RuntimeReady;d.phase=phase_;return true;
        } catch(const std::exception& e){return fail(d,e.what());}
    }
    g::TrackNavigationView50EU map() const noexcept {
        return gatesReady()?g::TrackNavigationView50EU{storage_,indices_,g::NavigationRole50MB::Map}:g::TrackNavigationView50EU{};
    }
    g::TrackNavigationView50EU ai() const noexcept {
        return gatesReady()?g::TrackNavigationView50EU{storage_,indices_,g::NavigationRole50MB::Ai}:g::TrackNavigationView50EU{};
    }
    g::AiTrackMetadataView50IU mapMetadata() const noexcept {return g::AiTrackMetadataView50IU{map()};}
    g::AiTrackMetadataView50IU aiMetadata() const noexcept {return g::AiTrackMetadataView50IU{ai()};}
    bool close() noexcept {
        if((storage_&&storage_.use_count()!=1)||(indices_&&indices_.use_count()!=1))return false;
        indices_.reset();storage_.reset();gateAttempted_=true;bindAttempted_=true;phase_=Phase::Closed;return true;
    }
};
}
