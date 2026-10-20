#pragma once
#include "tb_cold_sources50la.h"
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <thread>

namespace tb_navigation50TB {
namespace g=rr::game;namespace storage=g::navigation_storage_detail50TB;
enum class Phase : std::uint8_t {Empty,Source,Prepared,Topology,MapIndices,AiIndices,RuntimeReady,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::string message;g::TrackNavigationDiagnostic50EU navigation{};};
// Separate from cold source: final indices retain the genuine aggregate, while
// that aggregate retains only cold source. No ownership cycle is possible.
class Owner final {
    std::shared_ptr<const tb_cold_sources50LA::Owner> source_;
    std::shared_ptr<storage::Indices> indices_;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    bool prepareAttempted_=false,bindAttempted_=false;
    Phase phase_=Phase::Empty;
    bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;return false;}
    bool topologyMatches(const g::LiveZoneWorld50EN& actual) const {
        if(actual.zoneCount!=10||actual.portalCount!=11||actual.portalLinkCount!=22||
           !actual.portalHeads||!actual.portalLinks||!actual.portals)return false;
        // Ephemeral ordered verification only; no alternate live world escapes.
        g::CollectionRef50BX heads[10]{};g::LiveZonePortalLink50EN links[22]{};
        g::PortalGeometry50EO portals[11]{};g::LiveZoneWorld50EN expected{};
        const g::LiveZoneWorldStorage50EN backing{heads,10,links,22,portals,11};
        if(g::prepareLiveZoneWorld50EN(source_->navigation_->authoredPortals,11,10,backing,expected)!=
            g::LiveZoneStatus50EN::Complete)return false;
        for(std::size_t i=0;i<10;++i)if(actual.portalHeads[i]!=heads[i])return false;
        for(std::size_t i=0;i<22;++i){const auto& p=actual.portalLinks[i];
            if(p.next!=links[i].next||p.zone!=links[i].zone||p.portal!=links[i].portal)return false;}
        for(std::size_t i=0;i<11;++i){
            if(std::memcmp(actual.portals[i].vertices,portals[i].vertices,sizeof portals[i].vertices)||
               std::memcmp(&actual.portals[i].normal,&portals[i].normal,sizeof portals[i].normal)||
               std::memcmp(actual.portals[i].edgeNormals,portals[i].edgeNormals,sizeof portals[i].edgeNormals))return false;}
        return true;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner(){if(ownerThread_!=std::this_thread::get_id())std::terminate();}
    // Charge these final binding records separately from cold source payload.
    static constexpr std::size_t persistentPayloadBytes() noexcept {return sizeof(storage::Indices);}
    static constexpr std::size_t ownerBytes() noexcept {return sizeof(Owner);}
    static constexpr std::size_t topologyScratchBytes() noexcept {
        return 10*sizeof(g::CollectionRef50BX)+22*sizeof(g::LiveZonePortalLink50EN)+11*sizeof(g::PortalGeometry50EO)+sizeof(g::LiveZoneWorld50EN);
    }
    Phase phase() const noexcept {return phase_;}
    bool runtimeReady() const noexcept {return ownerThread_==std::this_thread::get_id()&&source_&&source_->ready()&&
        indices_&&indices_->world.valid()&&indices_->world.sourceIdentity()==source_.get();}
    bool prepare(std::shared_ptr<const tb_cold_sources50LA::Owner> source,Diagnostic& d){
        d={};if(ownerThread_!=std::this_thread::get_id())return fail(d,"TB navigation owner thread mismatch");
        if(prepareAttempted_)return fail(d,"TB navigation source preparation is one-attempt");
        prepareAttempted_=true;phase_=Phase::Source;
        if(!source||!source->ready()||source->map().count()!=270||source->ai().count()!=269)
            return fail(d,"TB authentic complete cold source required");
        source_=std::move(source);phase_=Phase::Prepared;d.phase=phase_;return true;
    }
    bool bindWorld(const g::TbCollisionWorldLease& lease,Diagnostic& d){
        d={};if(ownerThread_!=std::this_thread::get_id())return fail(d,"TB navigation owner thread mismatch");
        if(!source_||!source_->ready()||bindAttempted_)return fail(d,"TB world binding requires one prepared source owner");
        bindAttempted_=true;phase_=Phase::Topology;
        if(!lease.valid()||lease.sourceIdentity()!=source_.get()||!topologyMatches(*lease.world()))
            return fail(d,"TB retained same-source live-world topology mismatch");
        try{
            auto next=std::shared_ptr<storage::Indices>(new storage::Indices(lease));
            struct Graph {const g::UndertrackNode50ET* nodes;std::uint32_t count;};
            struct Navigation {Graph track;const g::TrackNavigationNode50EU* nodes;
                std::uint8_t* nodeIndex78;bool gatesPrepared,indicesPrepared;};
            const auto& s=*source_->navigation_;
            Navigation map{{s.map.track.nodes,270},s.map.nodes,next->map,true,false};
            Navigation ai{{s.ai.track.nodes,269},s.ai.nodes,next->ai,true,false};
            phase_=Phase::MapIndices;
            if(!g::navigation_detail50MB::bind<270>(*lease.world(),map,d.navigation))return fail(d,"TB full270 MAP zone indices rejected");
            phase_=Phase::AiIndices;
            if(!g::navigation_detail50MB::bind<269>(*lease.world(),ai,d.navigation))return fail(d,"TB full269 AI zone indices rejected");
            indices_=std::move(next);phase_=Phase::RuntimeReady;d.phase=phase_;return true;
        }catch(const std::exception& e){return fail(d,e.what());}
    }
    // Cold clients use source.map()/ai(). No final-owner view is issued before
    // successful binding, and existing cold views are never upgraded in place.
    g::TrackNavigationView50TB map() const noexcept {return runtimeReady()?
        g::TrackNavigationView50TB{source_,source_->navigation_,indices_,g::NavigationRole50TB::Map}:g::TrackNavigationView50TB{};}
    g::TrackNavigationView50TB ai() const noexcept {return runtimeReady()?
        g::TrackNavigationView50TB{source_,source_->navigation_,indices_,g::NavigationRole50TB::Ai}:g::TrackNavigationView50TB{};}
    g::AiTrackMetadataView50TB mapMetadata() const noexcept {return g::AiTrackMetadataView50TB{map()};}
    g::AiTrackMetadataView50TB aiMetadata() const noexcept {return g::AiTrackMetadataView50TB{ai()};}
    bool close() noexcept {
        if(ownerThread_!=std::this_thread::get_id()||(indices_&&indices_.use_count()!=1))return false;
        indices_.reset();source_.reset();prepareAttempted_=bindAttempted_=true;phase_=Phase::Closed;return true;
    }
};
}
