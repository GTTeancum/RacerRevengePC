#pragma once
#include "mc_cold_sources50la.h"
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <thread>

namespace mc_navigation50MC {
namespace g=rr::game;namespace storage=g::navigation_storage_detail50MC;
enum class Phase : std::uint8_t {Empty,Source,Prepared,Topology,MapIndices,AiIndices,RuntimeReady,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::string message;g::TrackNavigationDiagnostic50EU navigation{};};
// Separate from cold source: final indices retain the genuine aggregate, while
// that aggregate retains only cold source. No ownership cycle is possible.
class Owner final {
    std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
    std::shared_ptr<storage::Indices> indices_;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    bool prepareAttempted_=false,bindAttempted_=false;
    Phase phase_=Phase::Empty;
    bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;return false;}
    bool topologyMatches(const g::LiveZoneWorld50EN& actual) const {
        if(actual.zoneCount!=28||actual.portalCount!=28||actual.portalLinkCount!=56||
           !actual.portalHeads||!actual.portalLinks||!actual.portals)return false;
        // Ephemeral ordered verification only; no alternate live world escapes.
        g::CollectionRef50BX heads[28]{};g::LiveZonePortalLink50EN links[56]{};
        g::PortalGeometry50EO portals[28]{};g::LiveZoneWorld50EN expected{};
        const g::LiveZoneWorldStorage50EN backing{heads,28,links,56,portals,28};
        if(g::prepareLiveZoneWorld50EN(source_->navigation_->authoredPortals,28,28,backing,expected)!=
            g::LiveZoneStatus50EN::Complete)return false;
        for(std::size_t i=0;i<28;++i)if(actual.portalHeads[i]!=heads[i])return false;
        for(std::size_t i=0;i<56;++i){const auto& p=actual.portalLinks[i];
            if(p.next!=links[i].next||p.zone!=links[i].zone||p.portal!=links[i].portal)return false;}
        for(std::size_t i=0;i<28;++i){
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
        return 28*sizeof(g::CollectionRef50BX)+56*sizeof(g::LiveZonePortalLink50EN)+28*sizeof(g::PortalGeometry50EO)+sizeof(g::LiveZoneWorld50EN);
    }
    Phase phase() const noexcept {return phase_;}
    bool runtimeReady() const noexcept {return ownerThread_==std::this_thread::get_id()&&source_&&source_->ready()&&
        indices_&&indices_->world.valid()&&indices_->world.sourceIdentity()==source_.get();}
    bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,Diagnostic& d){
        d={};if(ownerThread_!=std::this_thread::get_id())return fail(d,"MC navigation owner thread mismatch");
        if(prepareAttempted_)return fail(d,"MC navigation source preparation is one-attempt");
        prepareAttempted_=true;phase_=Phase::Source;
        if(!source||!source->ready()||source->map().count()!=173||source->ai().count()!=173)
            return fail(d,"MC authentic complete cold source required");
        source_=std::move(source);phase_=Phase::Prepared;d.phase=phase_;return true;
    }
    bool bindWorld(const g::McCollisionWorldLease& lease,Diagnostic& d){
        d={};if(ownerThread_!=std::this_thread::get_id())return fail(d,"MC navigation owner thread mismatch");
        if(!source_||!source_->ready()||bindAttempted_)return fail(d,"MC world binding requires one prepared source owner");
        bindAttempted_=true;phase_=Phase::Topology;
        if(!lease.valid()||lease.sourceIdentity()!=source_.get()||!topologyMatches(*lease.world()))
            return fail(d,"MC retained same-source live-world topology mismatch");
        try{
            auto next=std::shared_ptr<storage::Indices>(new storage::Indices(lease));
            struct Graph {const g::UndertrackNode50ET* nodes;std::uint32_t count;};
            struct Navigation {Graph track;const g::TrackNavigationNode50EU* nodes;
                std::uint8_t* nodeIndex78;bool gatesPrepared,indicesPrepared;};
            const auto& s=*source_->navigation_;
            Navigation map{{s.map.track.nodes,173},s.map.nodes,next->map,true,false};
            Navigation ai{{s.ai.track.nodes,173},s.ai.nodes,next->ai,true,false};
            phase_=Phase::MapIndices;
            if(!g::navigation_detail50MB::bind<173>(*lease.world(),map,d.navigation))return fail(d,"MC full173 MAP zone indices rejected");
            phase_=Phase::AiIndices;
            if(!g::navigation_detail50MB::bind<173>(*lease.world(),ai,d.navigation))return fail(d,"MC full173 AI zone indices rejected");
            indices_=std::move(next);phase_=Phase::RuntimeReady;d.phase=phase_;return true;
        }catch(const std::exception& e){return fail(d,e.what());}
    }
    // Cold clients use source.map()/ai(). No final-owner view is issued before
    // successful binding, and existing cold views are never upgraded in place.
    g::TrackNavigationView50MC map() const noexcept {return runtimeReady()?
        g::TrackNavigationView50MC{source_,source_->navigation_,indices_,g::NavigationRole50MC::Map}:g::TrackNavigationView50MC{};}
    g::TrackNavigationView50MC ai() const noexcept {return runtimeReady()?
        g::TrackNavigationView50MC{source_,source_->navigation_,indices_,g::NavigationRole50MC::Ai}:g::TrackNavigationView50MC{};}
    g::AiTrackMetadataView50MC mapMetadata() const noexcept {return g::AiTrackMetadataView50MC{map()};}
    g::AiTrackMetadataView50MC aiMetadata() const noexcept {return g::AiTrackMetadataView50MC{ai()};}
    bool close() noexcept {
        if(ownerThread_!=std::this_thread::get_id()||(indices_&&indices_.use_count()!=1))return false;
        indices_.reset();source_.reset();prepareAttempted_=bindAttempted_=true;phase_=Phase::Closed;return true;
    }
};
}
