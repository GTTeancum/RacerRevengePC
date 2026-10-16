#pragma once
#include "original_wreck_assets50la.h"
#include "reset_visibility50ey.h"
#include <cstring>
#include <limits>
#include <memory>

namespace rr::game {
struct OriginalWreckVisibilityWorld50LA {
    // This is actual application4C0, separate from all E0/E4/E8 application4C4
    // collision backing. Its cold loader must admit the authored dynamic List
    // method, as for the existing reset/camera visibility owners.
    ResetVisibilityWorld50EY* world=nullptr;
    LiveZoneWorkspace50EN* workspace=nullptr;
    CollectionRef50BX modelObject=0;
};
enum class OriginalWreckVisibilityStatus50LA : std::uint8_t {
    complete,binding,allocation,graph,modelBounds,registry
};
struct OriginalWreckVisibilityResult50LA {
    OriginalWreckVisibilityStatus50LA status=OriginalWreckVisibilityStatus50LA::binding;
    std::uint32_t sourcePC=0;
    bool removalCommitted=false,registrationCommitted=false;
    LiveZoneResult50EN registry{};
};

// Actual per-model19C owner used by28E8A0/28EA20/28EA80. The body graph's
// observer is this real registry owner; it is not vehicle7F0, a draw-visible
// flag, or a callback that asserts an update happened. Backing is allocated
// once cold and never relocated. A model remains registered while its body
// is disabled, just as source attached wreck pieces do.
class OriginalWreckVisibility50LA final {
    OriginalWreckAssets50LA* assets_=nullptr;
    OriginalWreckVisibilityWorld50LA world_{};
    std::unique_ptr<CollectionZoneLink50BX[]> links_;
    std::unique_ptr<LiveZoneEntry50EN[]> entries_;
    std::size_t capacity_=0;
    LiveZoneMembership50EN membership_{};
    // Borrowed field identities only. Their values belong to the genuine
    // application-wide27F650/27F930 zone-admission owner, never this observer.
    const std::uint16_t* const* rendererZoneTagA4Fields_=nullptr;
    std::size_t rendererZoneCount_=0;
    const std::uint16_t* rendererCurrentTag8BE0_=nullptr;
    const std::uint16_t* rendererPrimaryZoneTagA4_=nullptr;
public:
    OriginalWreckVisibility50LA()=default;
    OriginalWreckVisibility50LA(const OriginalWreckVisibility50LA&)=delete;
    OriginalWreckVisibility50LA& operator=(const OriginalWreckVisibility50LA&)=delete;
    OriginalWreckVisibility50LA(OriginalWreckVisibility50LA&&)=delete;
    OriginalWreckVisibility50LA& operator=(OriginalWreckVisibility50LA&&)=delete;
    bool prepareCold(OriginalWreckAssets50LA& assets,const OriginalWreckVisibilityWorld50LA& w) noexcept {
        if(assets_||!assets.prepared()||!w.world||!w.workspace||!w.modelObject||
           !w.world->lifecycle.perBlock||w.modelObject>w.world->lifecycle.objectCount||
           w.world->portals.portalCount==std::numeric_limits<std::size_t>::max()||
           !w.world->lifecycle.storage.registrations||
           w.world->lifecycle.storage.registrationCapacity<w.world->lifecycle.objectCount)return false;
        const auto& r=w.world->lifecycle.storage.registrations[w.modelObject-1u];
        if(r.owner||r.slot||r.encodedObject)return false;
        try {
            const auto count=w.world->portals.portalCount+1u;
            auto links=std::make_unique<CollectionZoneLink50BX[]>(count);
            auto entries=std::make_unique<LiveZoneEntry50EN[]>(count);
            links_=std::move(links);entries_=std::move(entries);capacity_=count;
            membership_.storage={links_.get(),entries_.get(),capacity_};
            assets_=&assets;world_=w;return true;
        }catch(...){return false;}
    }
    bool prepared()const noexcept{return assets_!=nullptr;}
    bool boundTo(const OriginalWreckAssets50LA& a)const noexcept{return assets_==&a;}
    bool attached()const noexcept{return prepared()&&membership_.attached;}
    LiveZoneMembership50EN& membership19C() noexcept{return membership_;}
    const LiveZoneMembership50EN& membership19C()const noexcept{return membership_;}

    //19C->04 is the PRIMARY VISIBILITY ZONE, not a child model or a draw
    //receipt. The immutable table contains the actual zone+A4 addresses in
    //this exact4C0 world's authored-ref order. It and every pointed-to field
    //must outlive this owner. Cold binding while detached consumes no tag
    //values and writes none. It must precede constructGroup/activation.
    //Missing upstream zone admission must leave this unbound; allocating zero
    //tags, copying the host frame, or marking every registered zone is invalid.
    bool bindRendererZoneFields(const ResetVisibilityWorld50EY& actualWorld,
        const std::uint16_t* const* actualZoneTagA4Fields,std::size_t zoneCount,
        const std::uint16_t& actualCurrentTag8BE0) noexcept {
        if(!coherent()||membership_.attached||rendererZoneTagA4Fields_||&actualWorld!=world_.world||
           !actualZoneTagA4Fields||!zoneCount||zoneCount!=actualWorld.lifecycle.zoneCount)return false;
        for(std::size_t i=0;i<zoneCount;++i){
            if(!actualZoneTagA4Fields[i]||actualZoneTagA4Fields[i]==&actualCurrentTag8BE0)return false;
            for(std::size_t j=0;j<i;++j)
                if(actualZoneTagA4Fields[i]==actualZoneTagA4Fields[j])return false;
        }
        rendererZoneTagA4Fields_=actualZoneTagA4Fields;rendererZoneCount_=zoneCount;
        rendererCurrentTag8BE0_=&actualCurrentTag8BE0;
        retargetRendererPrimaryZone();
        return true;
    }
    //Stable address-of-field for OriginalWreckEffectRendererBinding50LA. The
    //existing attach/update/remove operations retarget it synchronously after
    //the actual primary membership changes; no per-tick Session copy is needed.
    const std::uint16_t* const* rendererChild04TagA4Field() const noexcept {
        return &rendererPrimaryZoneTagA4_;
    }
    const std::uint16_t* rendererCurrentTag8BE0() const noexcept {
        return rendererCurrentTag8BE0_;
    }

    OriginalWreckVisibilityResult50LA attach(CollectionRef50BX zone) noexcept {
        using S=OriginalWreckVisibilityStatus50LA;
        if(!coherent())return {S::binding,0x28e8a0u};
        OriginalWreckVisibilityResult50LA result{};
        //28E8B8 destroys an existing observer before allocating its replacement.
        //Real removal is retained on a later bounds/attach rejection.
        if(membership_.attached){result=remove();if(result.status!=S::complete)return result;}
        LiveZoneBounds50EN b{};const auto prepared=makeBounds(b);
        if(prepared!=S::complete){result.status=prepared;result.sourcePC=0x28e230u;return result;}
        auto& w=*world_.world;
        result.registry=attachLiveZoneMembership50EN(w.lifecycle,w.portals,membership_,
            {links_.get(),entries_.get(),capacity_},world_.modelObject,zone,
            ZoneMethod50CL::Second,0.f,b,*world_.workspace);
        retargetRendererPrimaryZone();
        result.registrationCommitted=result.registry.status==LiveZoneStatus50EN::Complete;
        result.status=result.registrationCommitted?S::complete:S::registry;
        result.sourcePC=result.registrationCommitted?0u:0x2809c0u;return result;
    }
    //28EA20 checks the real observer first. Detached means the actual null19C
    //route, and consumes no model pose/bounds. Attached invokes actual280D40
    //with model.radius198, rather than the initial attach's zero radius.
    OriginalWreckVisibilityResult50LA update() noexcept {
        using S=OriginalWreckVisibilityStatus50LA;
        if(!coherent())return {S::binding,0x28ea20u};
        if(!membership_.attached)return {S::complete,0u};
        LiveZoneBounds50EN b{};const auto prepared=makeBounds(b);
        if(prepared!=S::complete)return {prepared,0x28e230u};
        auto& w=*world_.world;auto r=updateLiveZoneMembership50EN(w.lifecycle,w.portals,
            membership_,b,*world_.workspace);
        retargetRendererPrimaryZone();
        return {r.status==LiveZoneStatus50EN::Complete?S::complete:S::registry,
            r.status==LiveZoneStatus50EN::Complete?0u:0x280d40u,false,
            r.status==LiveZoneStatus50EN::Complete,r};
    }
    OriginalWreckVisibilityResult50LA remove() noexcept {
        using S=OriginalWreckVisibilityStatus50LA;
        if(!coherent())return {S::binding,0x28ea80u};
        if(!membership_.attached)return {S::complete,0u};
        auto& w=*world_.world;auto r=removeLiveZoneMembership50EN(w.lifecycle,w.portals,
            membership_,*world_.workspace);
        retargetRendererPrimaryZone();
        return {r.status==LiveZoneStatus50EN::Complete?S::complete:S::registry,
            r.status==LiveZoneStatus50EN::Complete?0u:0x281490u,
            r.status==LiveZoneStatus50EN::Complete,false,r};
    }
private:
    void retargetRendererPrimaryZone() noexcept {
        rendererPrimaryZoneTagA4_=nullptr;
        if(!rendererZoneTagA4Fields_||!rendererCurrentTag8BE0_||!coherent()||
           !membership_.attached||!capacity_||!entries_[0].used)return;
        const auto zone=links_[0].zone;
        if(!zone||zone>rendererZoneCount_||rendererZoneCount_!=world_.world->lifecycle.zoneCount)return;
        const auto& lifecycle=world_.world->lifecycle;
        const auto& primary=lifecycle.storage.registrations[world_.modelObject-1u];
        const auto& entry=entries_[0].registration;
        const auto registryIndex=(static_cast<std::size_t>(zone)-1u)*2u+1u;
        if(!lifecycle.storage.registries||registryIndex>=lifecycle.storage.registryCapacity||
           primary.owner!=&lifecycle.storage.registries[registryIndex]||
           primary.encodedObject!=(world_.modelObject<<2u)||
           entry.owner!=primary.owner||entry.slot!=primary.slot||
           entry.encodedObject!=primary.encodedObject)return;
        rendererPrimaryZoneTagA4_=rendererZoneTagA4Fields_[zone-1u];
    }
    bool coherent()const noexcept {
        if(!assets_)return false;
        if(membership_.storage.links!=links_.get()||membership_.storage.entries!=entries_.get()||
           membership_.storage.capacity!=capacity_)return false;
        if(membership_.attached)return membership_.object==world_.modelObject&&
            membership_.method==ZoneMethod50CL::Second;
        const auto& r=world_.world->lifecycle.storage.registrations[world_.modelObject-1u];
        return !membership_.object&&!r.owner&&!r.slot&&!r.encodedObject;
    }
    OriginalWreckVisibilityStatus50LA makeBounds(LiveZoneBounds50EN& out) noexcept {
        using S=OriginalWreckVisibilityStatus50LA;auto graph=assets_->graph();
        const auto index=static_cast<std::int32_t>(assets_->model14C());
        if(!validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count)return S::graph;
        auto& root=graph.nodes[index];
        //Actual slot90=26F4B0 on precisely the source28E260..28E278 gate.
        //The cold166400 inertia box is a different producer and cannot stand
        //in for this live model160/170/198 cache (flat row0 is also included).
        if((root.sceneFlags190&0x80u)&&((root.sceneFlags190&8u)||!(root.sceneFlags190&0x40u))) {
            if(!assets_->refreshCachedModelBounds())return S::modelBounds;
        }
        if(!assets_->cachedModelBoundsReady())return S::modelBounds;
        if(!refreshSceneWorld49P(graph,assets_->model14C()))return S::graph;
        const auto& b=assets_->cachedModelBounds();out.bounds.shapeTag=1; //common box arithmetic, not a fake collision shape
        out.bounds.shapeWorld=root.world100;
        std::memcpy(out.bounds.localCenter.v,b.center160,sizeof b.center160);
        std::memcpy(out.bounds.localHalf.v,b.halfExtent170,sizeof b.halfExtent170);
        out.bounds.cachedRadius=b.radius198;
        CollectionBoundsResult48S actual{};
        if(prepareCollectionBounds48S(out.bounds,actual)!=CollectionBoundsStatus48S::Complete)return S::modelBounds;
        out.bounds.cachedCenter=actual.center;return S::complete;
    }
};
} // namespace rr::game
