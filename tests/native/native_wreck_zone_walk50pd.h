#pragma once
// Stand-in item a (work/scratch/destroy-standins/PLAN.md): the order in which
// a wreck piece's D8 detector visits nearby track colliders.
//
// Original 295630 -> 11FF60 (11FE50 prepare, 120230 membership update) ->
// 121920(E0, 0): pass 1 walks the piece's member zones (primary, then each
// secondary in link order, then fallback 443108 when nonzero and not the
// current zone) through each zone's FIRST-method SearchList in slot order;
// after 294740 opens a new list it reads once and an empty read ends the pass.
// Pass 2 (only when a member zone's two methods differ) walks the SECOND-
// method lists, no fallback. No duplicate suppression. 22C6A0 attaches every
// piece's E0 (120070, mask 2, radius 0) to the destroyed racer's current
// primary zone (racer+690 -> +E0 -> +4).
//
// Native: the piece's membership is a private shadow (own ZoneLifecycle50CL
// registries, a private copy of the track's portal world), driven by the
// existing 120070/120230/120290 kernels (live_zone_membership50en). The
// walk reads the TRACK's SearchLists read-only with local cursors that are
// never written back. Object refs 1..terrain map to track candidate ref-1;
// higher refs are query objects (never static targets) and are skipped.
// STAND-INS (flagged): fallback 443108 is 0 as everywhere in the port; the
// shadow registration is not in the track lists, so pass 2's empty-read end
// is not applied (the original's second lists always hold the piece itself);
// the original's F8 0x10 / query+F4 writes on visited objects are not made
// (other owners' state); RA keeps the storage-order stand-in (separate list
// domain). Everything is sized at race activation; frames never allocate.
#include "../../src/game/live_zone_membership50en.h"
#include "../../src/game/search_list50br.h"
#include <cstdint>
#include <iostream>
#include <vector>

namespace native_wreck_debris50PD::zones {
namespace g=rr::game;
inline constexpr std::size_t maxPieces=512;
inline constexpr std::uint32_t shadowPerBlock=512;

struct Walk {
    bool bound=false;
    // Track (read-only borrows).
    const g::CollectionZone50BX* zones=nullptr;std::size_t zoneCount=0;
    const g::CollectionTree50BX* trees=nullptr;std::size_t treeCount=0;
    std::size_t terrainRefs=0;
    g::CollectionRef50BX fallback443108=0;
    // Shadow lifecycle and portal world (owned).
    std::vector<g::SearchStorage50BS> registries,scratchRegistries;
    std::vector<g::ZoneRegistration50CJ> registrations;
    std::vector<g::CollectionTree50BX> shadowTrees;
    std::vector<g::CollectionZone50BX> shadowZones;
    std::vector<g::SearchListBlock50BR> blocks;
    g::ZoneLifecycle50CL lifecycle{};
    g::ZoneRegistrationScratch50CJ scratch{};
    std::vector<g::CollectionRef50BX> portalHeads;
    std::vector<g::LiveZonePortalLink50EN> portalLinks,portalScratch;
    std::vector<g::PortalGeometry50EO> portals;
    g::LiveZoneWorld50EN world{};
    std::vector<g::CollectionZoneLink50BX> scratchLinks;
    std::vector<g::LiveZoneEntry50EN> scratchEntries;
    g::LiveZoneWorkspace50EN work{};
    // Per live piece (index = live index; object ref = index + 1).
    std::size_t membershipCapacity=0;
    std::vector<g::CollectionZoneLink50BX> links;
    std::vector<g::LiveZoneEntry50EN> entries;
    std::vector<g::LiveZoneMembership50EN> members;
    // Counters.
    std::uint64_t attaches=0,attachRefused=0,updates=0,updateRefused=0,walks=0,visits=0,duplicates=0,
        queryObjects=0,refused=0,standinWalks=0,pass2=0,storageOnly=0,walkOnly=0;
    bool refusalLogged=false;
};
inline Walk& walk() noexcept { static Walk owner; return owner; }

// Race activation only (allocates). zones/trees/portal world are the track
// storage's; candidates must be exactly the debris track span in order.
inline bool bind(const g::CollectionZone50BX* zones,std::size_t zoneCount,const g::CollectionTree50BX* trees,
    std::size_t treeCount,std::size_t terrainRefs,const g::LiveZoneWorld50EN& trackWorld,std::size_t membershipCapacity) {
    auto& w=walk();w=Walk{};
    if(!zones||!zoneCount||!trees||treeCount<zoneCount*2u||!membershipCapacity||
       trackWorld.zoneCount!=zoneCount)return false;
    w.zones=zones;w.zoneCount=zoneCount;w.trees=trees;w.treeCount=treeCount;w.terrainRefs=terrainRefs;
    w.registries.resize(zoneCount*2u);w.scratchRegistries.resize(zoneCount*2u);w.registrations.resize(maxPieces);
    w.shadowTrees.resize(zoneCount*2u);w.shadowZones.resize(zoneCount);w.blocks.resize(zoneCount*4u);
    const g::ZoneLifecycleStorage50CL storage{w.registries.data(),w.registries.size(),w.registrations.data(),
        w.registrations.size(),{w.shadowTrees.data(),w.shadowTrees.size(),w.shadowZones.data(),w.shadowZones.size(),
        w.blocks.data(),w.blocks.size()}};
    if(g::initializeZoneLifecycle50CL(w.lifecycle,storage,zoneCount,maxPieces,shadowPerBlock,w.scratch)!=
       g::SearchStorageStatus50BS::Complete)return false;
    w.portalHeads.assign(trackWorld.portalHeads,trackWorld.portalHeads+trackWorld.zoneCount);
    w.portalLinks.assign(trackWorld.portalLinks,trackWorld.portalLinks+trackWorld.portalLinkCount);
    w.portalScratch.resize(trackWorld.portalLinkCount);
    w.portals.assign(trackWorld.portals,trackWorld.portals+trackWorld.portalCount);
    w.world={w.portalHeads.data(),w.portalHeads.size(),w.portalLinks.data(),w.portalLinks.size(),w.portals.data(),w.portals.size()};
    w.membershipCapacity=membershipCapacity;
    w.scratchLinks.resize(membershipCapacity);w.scratchEntries.resize(membershipCapacity);
    w.work={w.scratchRegistries.data(),w.scratchRegistries.size(),w.scratchLinks.data(),w.scratchEntries.data(),
        membershipCapacity,w.portalScratch.data(),w.portalScratch.size(),{}};
    w.links.resize(maxPieces*membershipCapacity);w.entries.resize(maxPieces*membershipCapacity);w.members.resize(maxPieces);
    w.bound=true;return true;
}
inline g::LiveZoneStorage50EN storageFor(Walk& w,std::size_t live) noexcept {
    return {w.links.data()+live*w.membershipCapacity,w.entries.data()+live*w.membershipCapacity,w.membershipCapacity};
}
inline g::LiveZoneBounds50EN boundsFor(const g::Type2ResourceView49X& e0,const g::BodyMatrix48G& preparedWorld) noexcept {
    g::LiveZoneBounds50EN b{};b.bounds.shapeTag=2;b.bounds.shapeWorld=preparedWorld;b.type2Resource=&e0;return b;
}
// 22C6A0 -> 120070(E0, zone, mask 2): attach at the racer's primary zone.
inline bool attach(std::size_t live,g::CollectionRef50BX zone,const g::Type2ResourceView49X& e0,const g::BodyMatrix48G& world) noexcept {
    auto& w=walk();
    if(!w.bound||live>=maxPieces||!zone||zone>w.zoneCount){++w.attachRefused;return false;}
    auto& m=w.members[live];m=g::LiveZoneMembership50EN{};
    const auto r=g::attachLiveZoneMembership50EN(w.lifecycle,w.world,m,storageFor(w,live),
        static_cast<g::CollectionRef50BX>(live+1),zone,g::ZoneMethod50CL::Second,0.f,boundsFor(e0,world),w.work);
    if(r.status!=g::LiveZoneStatus50EN::Complete){++w.attachRefused;
        if(!w.refusalLogged){w.refusalLogged=true;std::cerr<<"debris_zone_walk attach_refused live="<<live<<" zone="<<zone
            <<" status="<<int(r.status)<<'\n';}
        m=g::LiveZoneMembership50EN{};return false;}
    ++w.attaches;return true;
}
inline const g::LiveZoneMembership50EN* membership(std::size_t live) noexcept {
    auto& w=walk();if(!w.bound||live>=maxPieces||!w.members[live].attached)return nullptr;return &w.members[live];
}
// 120230 at the detector's prepared world. A refusal detaches the shadow
// (the piece then uses the storage-order stand-in), counted and logged.
inline bool update(std::size_t live,const g::Type2ResourceView49X& e0,const g::BodyMatrix48G& prepared) noexcept {
    auto& w=walk();if(!w.bound||live>=maxPieces||!w.members[live].attached)return false;
    const auto r=g::updateLiveZoneMembership50EN(w.lifecycle,w.world,w.members[live],boundsFor(e0,prepared),w.work);
    if(r.status!=g::LiveZoneStatus50EN::Complete){++w.updateRefused;
        if(!w.refusalLogged){w.refusalLogged=true;std::cerr<<"debris_zone_walk update_refused live="<<live<<" status="<<int(r.status)<<'\n';}
        (void)g::removeLiveZoneMembership50EN(w.lifecycle,w.world,w.members[live],w.work);
        w.members[live]=g::LiveZoneMembership50EN{};return false;}
    ++w.updates;return true;
}
// 120290 at retirement.
inline void remove(std::size_t live) noexcept {
    auto& w=walk();if(!w.bound||live>=maxPieces||!w.members[live].attached)return;
    (void)g::removeLiveZoneMembership50EN(w.lifecycle,w.world,w.members[live],w.work);
    w.members[live]=g::LiveZoneMembership50EN{};
}
// The 121920 pass 1 / pass 2 visit order for one membership. admit(index)
// returns false to refuse the whole query (capacity). Returns false on a
// topology/capacity refusal (caller falls back to the storage order, logged).
template<class Admit>
inline bool order(const g::LiveZoneMembership50EN& m,Admit&& admit) noexcept {
    auto& w=walk();++w.walks;
    const auto& st=m.storage;
    if(!st.links||!st.capacity||!st.links[0].zone)return false;
    auto methodOf=[&](g::CollectionRef50BX zone,unsigned pass)->g::CollectionRef50BX{
        if(!zone||zone>w.zoneCount)return 0;const auto& z=w.zones[zone-1];return pass==1?z.firstMethod:z.secondMethod;};
    bool secondDiffers=false;
    {std::size_t steps=0;for(g::CollectionRef50BX l=1;l;l=st.links[l-1].next){
        if(++steps>st.capacity||l>st.capacity)return false;
        const auto z=st.links[l-1].zone;if(!z||z>w.zoneCount)return false;
        if(w.zones[z-1].firstMethod!=w.zones[z-1].secondMethod)secondDiffers=true;}}
    for(unsigned pass=1;pass<=2;++pass){
        if(pass==2){if(!secondDiffers)break;++w.pass2;}
        g::CollectionRef50BX link=1,zone=st.links[0].zone;std::size_t steps=0;
        g::CollectionRef50BX method=methodOf(zone,pass);
        bool opened=true;          // the first list is opened by 294600 begin
        g::SearchListCursor50BR cursor{};g::resetSearchList50BR(cursor,true);
        bool firstRead=false;      // pass 1: one read after 294740 opens a new list
        for(std::size_t guard=0;;++guard){
            if(guard>1000000u)return false;
            std::uint32_t word=0;
            if(method){
                if(method>w.treeCount||w.trees[method-1].method!=g::CollectionTree50BX::Method::SearchList)return false;
                if(g::nextSearchList50BR(w.trees[method-1].searchList,cursor,word)!=g::SearchListStatus50BR::Complete)return false;
            }
            if(word){
                firstRead=false;
                if(word&3u)return false;
                const auto ref=word>>2u;
                if(!ref)return false;
                if(ref>w.terrainRefs){++w.queryObjects;continue;}
                ++w.visits;if(!admit(static_cast<std::size_t>(ref-1u)))return false;
                continue;
            }
            if(firstRead&&pass==1)break;   // newly opened list read empty: the pass ends
            // Advance to the next member zone (then the fallback in pass 1).
            method=0;
            if(link){
                if(++steps>st.capacity)return false;
                link=st.links[link-1].next;
                if(link){if(link>st.capacity)return false;zone=st.links[link-1].zone;method=methodOf(zone,pass);
                    if(!method)return false;}
            }
            if(!method&&pass==1&&w.fallback443108&&zone!=w.fallback443108){zone=w.fallback443108;method=methodOf(zone,pass);}
            if(!method)break;
            g::resetSearchList50BR(cursor,true);opened=true;firstRead=true;
        }
        (void)opened;
    }
    return true;
}
} // namespace native_wreck_debris50PD::zones
