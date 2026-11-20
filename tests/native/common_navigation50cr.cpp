// Common rewrite (R4, PLAN B-C12 / 2.1 step 08): the ONE navigation load for
// every course. 2A1160 TunnelTrack / 2A1260 spline reader over the course's
// own map (m<code>.spl) and AI (a<code>.spl) splines, 2A0F30/2A1260 AI
// metadata, 2A1D80 gates, then the 1ACF70 zone index against the one live
// collision world (roles.collision). Every navigation block is sized ONCE from
// the spline's own NumNodes (and the SCB's own portal count) through
// common_track50CR::Block; every write is bounds-checked. Replaces the five
// per-course navigation owners (mb/gb/mc/tb/tc_navigation50xx.h), the RC
// navigation lease and the RA cold navigation lease. No course is named here:
// the course code builds file names and looks up receipts only.
#include "common_track50cr.h"
#include "stock_course_receipts50cr.h"
#include "course_navigation_topology50nt.h"
#include "track_collision_world50cr.h"     // R3 NEW: the one collision world (roles.collision)
#include "../../src/game/navigation_view50cr.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include "../../src/assets/ai_authoring50iu.h"
#include <atomic>
#include <memory>
#include <string>

namespace common_track50CR {
namespace {
namespace g=rr::game;
namespace a=rr::assets;
constexpr std::size_t N=g::navigationNodeLimit50ET;   // reader guard (PORT STAND-IN, flagged in its header)

// Reader scratch of the shared 2A1260/2A1D80 templates (track_navigation_shared50mb.inl).
// Heap-owned for one read, released before the step returns. The persistent
// store is the Block-sized NavigationGraph below.
struct Graph {g::UndertrackNode50ET nodes[N]{};std::uint32_t count=0;};
struct Reader {Graph track{};std::uint8_t seen[N]{},fields[N]{},nextFields[N]{};};
struct Navigation {Graph track{};g::TrackNavigationNode50EU nodes[N]{};
    std::uint8_t nodeIndex78[N]{};bool gatesPrepared=false,indicesPrepared=false;};
struct Scratch {Reader reader{};Navigation candidate{};std::uint32_t fieldMask[N]{};};
struct Metadata {g::AiTrackNodeMetadata50IU nodes[N]{};float trackLength3C=0.f;std::uint32_t count=0;bool bound=false;};

bool refuse(Diagnostic& d,std::uint32_t pc,std::string why,std::uint64_t value=0){
    d.refuse(log::Step::navigation2A1260,pc,why);d.value=value;
    log::load(log::Step::navigation2A1260,false,value,why,nullptr,pc);
    return false;
}
// Sub-role receipt line (base roles nav_nodes/ai_nodes/lap_markers are printed
// by Track::logTrackData from Counts). Receipts never size anything.
void subRole(const Track& t,std::string_view role,std::string_view file,std::uint64_t value){
    const stock_course_receipts50CR::Receipt* r=t.source()==Source::stock?
        stock_course_receipts50CR::find(t.code(),role):nullptr;
    log::trackDataNamed(role,t.code(),file,value,r!=nullptr,r?r->value:0u);
}
bool vectorField(const a::ParameterBlock48F& block,const char* key,g::Vector3_48C& out){
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
// 2A26E0(node=0): the first authored Next0 pointing to node 0.
std::uint64_t predecessorOfZero(const Graph& g){
    for(std::uint32_t i=0;i<g.count;++i)if(g.nodes[i].nextCount&&g.nodes[i].next[0]==0)return i;
    return g.count;
}
bool splineBytes(Track& track,NavigationRole role,Bytes& out,std::string& name,Diagnostic& d){
    auto& held=role==NavigationRole::map?track.documents.mapSpline:track.documents.aiSpline;
    name=role==NavigationRole::map?track.identity.mapSpline:track.identity.aiSpline;
    // File-name building from the course code (allowed: data, not a code path).
    if(name.empty())name=std::string("tracks\\")+(role==NavigationRole::map?"m":"a")+track.identity.code.lower()+".spl";
    if(held.empty()&&!track.archive.member(name,held,g::maxUndertrackText50ET,d))
        return refuse(d,0x2a1160,"spline member missing or larger than the 2A1260 text limit: "+name);
    out=held;return true;
}
// One role: read 2A1260 + gates 2A1D80 + metadata 2A0F30, then size its blocks
// from the file's NumNodes and copy (bounds-checked writes).
bool readRole(Track& track,NavigationStorage& s,NavigationRole role,g::SourceLibmState49T& libm,Diagnostic& d){
    const bool map=role==NavigationRole::map;
    Bytes text;std::string name;
    if(!splineBytes(track,role,text,name,d))return false;
    auto scratch=std::make_unique<Scratch>();
    auto read=std::make_unique<Navigation>();
    g::TrackNavigationDiagnostic50EU nd{};
    if(!g::navigation_detail50MB::read<N>(reinterpret_cast<const char*>(text.data()),text.size(),libm,*scratch,*read,nd))
        return refuse(d,nd.sourcePC?nd.sourcePC:0x2a1260u,"2A1260 spline read refused: "+name,nd.node);
    auto metadata=std::make_unique<Metadata>();
    a::AiAuthoringDiagnostic50IU md{};
    if(!a::ai_authoring_detail50IU::readTrackMetadata<N>(reinterpret_cast<const char*>(text.data()),text.size(),*read,*metadata,md))
        return refuse(d,md.sourcePC?md.sourcePC:0x2a1260u,"2A0F30/2A1260 AI metadata refused: "+name,md.node);
    const std::uint32_t count=read->track.count;
    auto& graph=map?s.map:s.ai;
    const std::uint32_t pc=0x2a1260;
    if(!graph.nodes.size(map?BlockId::navMapNodes:BlockId::navAiNodes,count,pc)||
       !graph.gates.size(map?BlockId::navMapGates:BlockId::navAiGates,count,pc)||
       !graph.nodeIndex78.size(map?BlockId::navMapIndex:BlockId::navAiIndex,count,pc)||
       !graph.metadata.size(map?BlockId::navMapMetadata:BlockId::navAiMetadata,count,pc))
        return refuse(d,pc,"navigation block sizing refused: "+name,count);
    for(std::uint32_t i=0;i<count;++i){
        if(!graph.nodes.write(i,read->track.nodes[i],pc)||!graph.gates.write(i,read->nodes[i],pc)||
           !graph.nodeIndex78.write(i,0u,pc)||!graph.metadata.write(i,metadata->nodes[i],pc))
            return refuse(d,pc,"navigation block write refused: "+name,i);
    }
    if(map){
        if(!s.mapFieldMask.size(BlockId::navFieldMask,count,pc))return refuse(d,pc,"map field mask sizing refused",count);
        for(std::uint32_t i=0;i<count;++i)if(!s.mapFieldMask.write(i,scratch->fieldMask[i],pc))
            return refuse(d,pc,"map field mask write refused",i);
    }
    graph.count=count;graph.trackLength3C=metadata->trackLength3C;
    graph.gatesPrepared=read->gatesPrepared;graph.indicesPrepared=false;graph.metadataBound=metadata->bound;
    // Counts (Track::logTrackData prints nav_nodes / ai_nodes / lap_markers from these).
    std::uint32_t markers=0;
    for(std::uint32_t i=0;i<count;++i)if(metadata->nodes[i].progressNode)++markers;
    if(map){track.counts.navMapNodes=count;track.counts.lapMarkers=markers;}
    else track.counts.navAiNodes=count;
    const auto predecessor=predecessorOfZero(read->track);
    subRole(track,map?"nav_nodes.mspl_bytes":"ai_nodes.aspl_bytes",name,text.size());
    subRole(track,map?"nav_nodes.predecessor_of_zero":"ai_nodes.predecessor_of_zero",name,predecessor);
    if(map){subRole(track,"lap_markers.node_count",name,count);subRole(track,"lap_markers.predecessor_of_zero",name,predecessor);}
    return true;
}
std::atomic<std::uint64_t> navigationGeneration{0};
} // namespace

bool readNavigation2A1260(Track& track,Diagnostic& d)noexcept {
    try {
        d={};d.step=log::Step::navigation2A1260;
        if(track.roles.navigation)return refuse(d,0x2a1160,"navigation already read for this track (one attempt)");
        for(BlockId b:{BlockId::navMapNodes,BlockId::navAiNodes,BlockId::navMapGates,BlockId::navAiGates,BlockId::navMapIndex,
                BlockId::navAiIndex,BlockId::navMapMetadata,BlockId::navAiMetadata,BlockId::navFieldMask})
            if(blockRow(b).ceiling<N)return refuse(d,0x2a1260,std::string("reader guard above block ceiling: ")+blockRow(b).name);
        // SCB topology for the zone index (SCB portals, 270900 already decoded by 05.01).
        const auto& world=track.documents.world;
        if(world.zones.empty()||world.zones.size()>256)
            return refuse(d,0x1acf70,"SCB zone count outside the live-world rule (1..256)",world.zones.size());
        for(std::size_t i=0;i<world.zones.size();++i)if(world.zones[i].zone!=i+1)
            return refuse(d,0x1acf70,"SCB zone order rejected",i);
        auto s=std::make_shared<NavigationStorage>();
        const std::size_t portals=world.portals.size();
        if(!s->authoredPortals.size(BlockId::navPortals,portals,0x1acf70))return refuse(d,0x1acf70,"SCB portal block sizing refused",portals);
        for(std::size_t i=0;i<portals;++i){const auto& p=world.portals[i];
            if(p.parameter_block>=world.parameters.blocks.size())return refuse(d,0x1acf70,"SCB portal parameter block invalid",i);
            g::LiveZonePortalInput50EN in{};in.zoneA=p.zones[0];in.zoneB=p.zones[1];
            for(unsigned k=0;k<4;++k)if(!vectorField(world.parameters.blocks[p.parameter_block],
                ("vertex"+std::to_string(k+1)).c_str(),in.vertices[k]))
                return refuse(d,0x1acf70,"SCB portal binary32 vertex rejected",i);
            if(!s->authoredPortals.write(i,in,0x1acf70))return refuse(d,0x1acf70,"SCB portal write refused",i);
        }
        s->authoredZoneCount=static_cast<std::uint32_t>(world.zones.size());
        s->authoredPortalCount=static_cast<std::uint32_t>(portals);
        subRole(track,"nav_nodes.zones",track.identity.scbMember,s->authoredZoneCount);
        subRole(track,"nav_nodes.portals",track.identity.scbMember,s->authoredPortalCount);
        // PORT STAND-IN (flagged, w0-requests\r4.txt): the frozen signature has no
        // libm owner, so the 2A1D80 acos runs on a fresh default source libm state
        // (mode 1, errno 0) instead of the session's serial owner.
        g::SourceLibmState49T libm{};
        if(!readRole(track,*s,NavigationRole::map,libm,d)||!readRole(track,*s,NavigationRole::ai,libm,d))return false;
        s->gatesReady=s->map.gatesPrepared&&s->ai.gatesPrepared&&s->map.metadataBound&&s->ai.metadataBound;
        if(!s->gatesReady)return refuse(d,0x2a1d80,"navigation gates or metadata not prepared");
        track.roles.navigation=std::move(s);
        d.value=track.counts.navMapNodes;
        log::load(log::Step::navigation2A1260,true,track.counts.navMapNodes,"2A1160/2A1260 map+AI splines",nullptr,0x2a1260);
        return true;
    }catch(const std::exception& e){return refuse(d,0x2a1260,std::string("navigation read failed: ")+e.what());}
    catch(...){return refuse(d,0x2a1260,"navigation read failed");}
}

bool bindNavigation1ACF70(Track& track,Diagnostic& d)noexcept {
    try {
        d={};d.step=log::Step::navigation2A1260;
        auto& s=track.roles.navigation;
        if(!s||!s->gatesReady||s->world||s->worldLease)
            return refuse(d,0x1acf70,"1ACF70 needs one gates-ready, unbound navigation store");
        if(!track.roles.collision)return refuse(d,0x1acf70,"1ACF70 needs the one collision world (roles.collision)");
        auto& collision=*track.roles.collision;
        auto borrow=collision.borrow();
        const g::LiveZoneWorld50EN* world=borrow.world();
        std::shared_ptr<const void> lease=collision.lease();
        if(!world||!lease)return refuse(d,0x1acf70,"collision world not geometry-ready");
        if(!course_navigation_topology50NT::matches(s->authoredPortals.data(),s->authoredPortalCount,s->authoredZoneCount,*world))
            return refuse(d,0x1acf70,"live collision world topology differs from the SCB portals",s->authoredZoneCount);
        struct GraphView {const g::UndertrackNode50ET* nodes;std::uint32_t count;};
        struct BindTarget {GraphView track;const g::TrackNavigationNode50EU* nodes;std::uint8_t* nodeIndex78;bool gatesPrepared,indicesPrepared;};
        for(NavigationGraph* graph:{&s->map,&s->ai}){
            auto index=std::make_unique<std::uint8_t[]>(N);   // template writes N bytes (its guard), then copy the live count
            BindTarget t{{graph->nodes.data(),graph->count},graph->gates.data(),index.get(),graph->gatesPrepared,false};
            g::TrackNavigationDiagnostic50EU nd{};
            if(graph->count>N||!g::navigation_detail50MB::bind<N>(*world,t,nd))
                return refuse(d,nd.sourcePC?nd.sourcePC:0x1acf70u,graph==&s->map?"map zone index refused":"AI zone index refused",nd.node);
            for(std::uint32_t i=0;i<graph->count;++i)if(!graph->nodeIndex78.write(i,index[i],0x1acf70))
                return refuse(d,0x1acf70,"zone index write refused",i);
            graph->indicesPrepared=true;
        }
        s->world=world;s->worldLease=std::move(lease);s->generation=++navigationGeneration;
        d.value=s->authoredZoneCount;
        log::load(log::Step::navigation2A1260,true,s->authoredZoneCount,"1ACF70 zone index",nullptr,0x1acf70);
        return true;
    }catch(const std::exception& e){return refuse(d,0x1acf70,std::string("zone index failed: ")+e.what());}
    catch(...){return refuse(d,0x1acf70,"zone index failed");}
}

NavigationView navigationView(const Track& track,NavigationRole role)noexcept {
    if(!track.roles.navigation)return {};
    return NavigationView{std::shared_ptr<const NavigationStorage>(track.roles.navigation),role};
}

} // namespace common_track50CR
