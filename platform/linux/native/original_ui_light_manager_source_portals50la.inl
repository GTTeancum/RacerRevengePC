#pragma once
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::linkSourceRaPortal148D00(
    const ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,const C& portal,const C& parent,
    std::uint32_t zoneA,std::uint32_t zoneB,Diagnostic& d)noexcept {
    d={};
    if(!raZones_||!raZones_->sourceMode||raZones_->world!=&world||
       !raZones_->sourceReturned||raZones_->sourceNativeClosed||raZones_->sourcePortalLinksWithdrawn||
       ordinal>=10||zoneA>=10||zoneB>=10||
       !preflightSourceRaZonesNative(world)||
       !world.sourcePortalTailCurrent148D00(ordinal,&parent,&portal,zoneA,zoneB))
        return fail(d,Status::identity,0x148d00);
    auto& r=*raZones_;
    if(r.sourcePortalEntered[ordinal]||r.portalLinksPublished!=2*ordinal||
       (ordinal&&!r.sourcePortalReturned[ordinal-1])||(r.portalParent&&r.portalParent!=&parent))
        return fail(d,Status::phase,0x148d00);
    // Native borrow ledger, not guest retain or a second Portal owner.
    r.sourcePortalEntered[ordinal]=true;r.portals[ordinal]=&portal;r.portalParent=&parent;
    r.sourcePortalIndices[ordinal]={{zoneA,zoneB}};
    const auto at=[&](std::uint32_t pc){r.sourcePC=pc;};
    const auto stop=[&](Status status,std::uint32_t pc){at(pc);return fail(d,status,pc);};
    constexpr std::uint32_t reads[]={0x148d18,0x148da8,0x148e2c};
    constexpr std::uint32_t enables[]={0x148d38,0x148dbc,0x148e40};
    constexpr std::uint32_t guards[]={0x148d3c,0x148dc0,0x148e44};
    constexpr std::uint32_t enabledReads[]={0x148d44,0x148dc8,0x148e4c};
    constexpr std::uint32_t assertions[]={0x148d78,0x148dfc,0x148e94};
    for(unsigned g=0;g<3;++g){
        at(reads[g]);auto& guard=r.sourceGuardReadValues[ordinal][g];
        if(!source_portal_guards50LA::read(g*2+1,guard))return stop(Status::phase,r.sourcePC);
        r.sourceGuardReadMask[ordinal]|=1u<<g;
        if(!guard){
            at(enables[g]);if(!source_portal_guards50LA::storeOne(g*2))return stop(Status::phase,r.sourcePC);
            r.sourceGuardStoreMask[ordinal]|=1u<<g;
            at(guards[g]);if(!source_portal_guards50LA::storeOne(g*2+1))return stop(Status::phase,r.sourcePC);
            r.sourceGuardStoreMask[ordinal]|=1u<<(g+3);
        }
        at(enabledReads[g]);auto& enabled=r.sourceEnabledReadValues[ordinal][g];
        if(!source_portal_guards50LA::read(g*2,enabled))return stop(Status::phase,r.sourcePC);
        r.sourceGuardReadMask[ordinal]|=1u<<(g+3);
        if(enabled){
            // Authenticated indices make each bounds assertion argument1.
            //148E64/78 genuinely read the same88 slots, never canonical rows.
            if(g==0){at(0x148d58);if(zoneA>=count90_)return stop(Status::identity,r.sourcePC);}
            if(g==1){at(0x148ddc);if(zoneB>=count90_)return stop(Status::identity,r.sourcePC);}
            if(g==2){
                at(0x148e64);if(!nodes88_||!nodes88_[zoneA]||nodes88_[zoneA]!=r.zones[zoneA].get())return stop(Status::identity,r.sourcePC);
                at(0x148e78);if(!nodes88_[zoneB]||nodes88_[zoneB]!=r.zones[zoneB].get())return stop(Status::identity,r.sourcePC);
            }
            at(assertions[g]);r.sourceAssertMask[ordinal]|=1u<<g;
            //133660(argument1) returns0; no failure-log or BREAK path.
        }
    }
    for(unsigned direction=0;direction<2;++direction){
        const auto i=ordinal*2+direction;
        const auto from=direction?zoneB:zoneA,to=direction?zoneA:zoneB;
        at(direction?0x148f04:0x148ee0);
        if(!nodes88_||nodes88_[from]!=r.zones[from].get()||!r.zones[from]||
           nodes88_[to]!=r.zones[to].get()||!r.zones[to])return stop(Status::identity,r.sourcePC);
        at(0x27f3d0);r.sourceLinkAttempted[i]=true;
        r.portalLinks[i].reset(new(std::nothrow) RaPortalLink27F3B0);
        at(0x27f3d8);r.sourceLinkOldHeads[i]=r.zones[from]->field30;r.sourceLinkOldHeadKnown[i]=true;
        at(0x27f3e4);if(!r.portalLinks[i])return stop(Status::allocation,r.sourcePC);
        auto& link=*r.portalLinks[i];
        link.next00=static_cast<RaPortalLink27F3B0*>(const_cast<void*>(r.sourceLinkOldHeads[i]));r.sourceLinkStoreMask[i]|=1;
        at(0x27f3e8);link.neighbor04=&r.zones[to]->entity;r.sourceLinkStoreMask[i]|=2;
        at(0x27f3ec);link.portal08=&portal;r.sourceLinkStoreMask[i]|=4;
        at(0x27f3f0);link.field0C=0;r.sourceLinkStoreMask[i]|=8;
        at(0x27f3f4);link.field10=0x7f7fffff;r.sourceLinkStoreMask[i]|=16;
        at(0x27f3f8);r.zones[from]->field30=&link;r.sourceLinkStoreMask[i]|=32;++r.portalLinksPublished;
    }
    at(0x148f20);r.sourcePortalReturned[ordinal]=true;d={Status::complete,r.sourcePC};return true;
}
bool OriginalUiLightManagerOwner50KG::preflightSourceRaPortalLinksNative(
    const ::ra_cold_aggregate50LA::Owner& world)const noexcept {
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world)return false;
    const auto& r=*raZones_;if(r.portalLinksPublished>20)return false;
    std::array<const void*,10> heads{};bool gap=false;std::size_t entered=0;
    for(std::size_t o=0;o<10;++o){
        const auto read=r.sourceGuardReadMask[o],store=r.sourceGuardStoreMask[o],asserted=r.sourceAssertMask[o];
        if((read&~63u)||(store&~63u)||(asserted&~7u))return false;
        if(!r.sourcePortalEntered[o]){gap=true;if(r.portals[o]||r.sourcePortalReturned[o]||read||store||asserted||r.sourcePortalIndices[o][0]||r.sourcePortalIndices[o][1])return false;continue;}
        if(gap||(o&&!r.sourcePortalReturned[o-1])||!r.sourceReturned)return false;++entered;
        const auto a=r.sourcePortalIndices[o][0],b=r.sourcePortalIndices[o][1];if(a>=10||b>=10)return false;
        if(r.sourcePortalLinksWithdrawn){if(r.portals[o])return false;}
        else if(!r.portals[o]||!r.portalParent||!world.sourcePortalTailCurrent148D00(o,r.portalParent,r.portals[o],a,b))return false;
        for(unsigned g=0;g<3;++g){
            const bool guardRead=read&(1u<<g),enabledRead=read&(1u<<(g+3));
            const auto guard=r.sourceGuardReadValues[o][g],enabled=r.sourceEnabledReadValues[o][g];
            if(guard>1||enabled>1||(!guardRead&&guard)||(!enabledRead&&enabled))return false;
            if(g&&guardRead&&(!(read&(1u<<(g+2)))||!(asserted&(1u<<(g-1)))))return false;
            if((store&(1u<<g))&&(!guardRead||guard))return false;
            if((store&(1u<<(g+3)))&&!(store&(1u<<g)))return false;
            if(enabledRead&&(!guardRead||(!guard&&!(store&(1u<<(g+3))))||enabled!=1))return false;
            if((asserted&(1u<<g))&&(!enabledRead||!enabled))return false;
        }
        if(r.sourcePortalReturned[o]&&(read!=63||asserted!=7||r.portalLinksPublished<o*2+2))return false;
    }
    if((entered==0&&r.portalParent)||(r.sourcePortalLinksWithdrawn&&r.portalParent))return false;
    if(r.portalLinksPublished>entered*2)return false;
    for(std::size_t i=0;i<20;++i){
        const auto o=i/2;const auto mask=r.sourceLinkStoreMask[i];const auto* link=r.portalLinks[i].get();
        if(mask&~63u)return false;
        if(!r.sourceLinkAttempted[i]){if(link||mask||r.sourceLinkOldHeadKnown[i]||r.sourceLinkOldHeads[i]||i<r.portalLinksPublished)return false;continue;}
        if(!r.sourcePortalEntered[o]||r.sourceAssertMask[o]!=7||i>r.portalLinksPublished||
           !r.sourceLinkOldHeadKnown[i]||(i&&r.sourceLinkStoreMask[i-1]!=63))return false;
        const auto from=r.sourcePortalIndices[o][i%2],to=r.sourcePortalIndices[o][1-i%2];
        if(!r.sourcePortalLinksWithdrawn){
            if(r.sourceLinkOldHeads[i]!=heads[from])return false;
            if(link){
                std::uint32_t expected0C=0,expected10=0x7f7fffff;bool distanceWritten=false;
                //Latest authentic overlap stores win. Keep these receipts after native
                //light withdrawal; do not restore constructor distances or recompute geometry.
                for(const auto& typed:world.sourceTypedLights_)if(typed){
                    if(typed->secondaryCount>20)return false;
                    for(std::size_t k=0;k<typed->secondaryCount;++k){const auto& step=typed->secondary[k];
                        if(step.edgeIndex!=i||!step.edgeDistanceMask)continue;
                        if(!step.entered||step.edgeDistanceMask>3||!step.overlap.returned||!step.overlap.hit||!step.overlap.outputDistance)return false;
                        if(step.edgeDistanceMask&1)expected10=*step.overlap.outputDistance;
                        if(step.edgeDistanceMask&2)expected0C=*step.overlap.outputDistance;
                        if(step.edgeDistanceMask==3)distanceWritten=true;
                    }
                }
                if(mask!=63||link->next00!=heads[from]||!r.zones[to]||
                   link->neighbor04!=&r.zones[to]->entity||link->portal08!=r.portals[o]||
                   link->field0C!=expected0C||link->field10!=expected10||link->distanceWritten!=distanceWritten)return false;
            }
            else if(mask)return false;
        }else if(link)return false;
        if(i<r.portalLinksPublished){if(mask!=63)return false;heads[from]=link;}
        else if(mask)return false; // The only terminal unpublished case is NULL allocation.
    }
    for(std::size_t i=0;i<10;++i)if(r.zones[i]&&r.zones[i]->field30!=heads[i])return false;
    return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceRaPortalLinksNative(
    const ::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world))return fail(d,Status::phase,0x148110);
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world)return true;
    auto& r=*raZones_;if(r.sourcePortalLinksWithdrawn)return true;
    for(const auto& typed:world.sourceTypedLights_)if(typed&&typed->observerEntered&&!typed->observerWithdrawn)
        return fail(d,Status::phase,0x148110); //Active observer cursor/membership borrows portal nodes.
    // Native withdrawal, not execution of guest node30=NULL or shared-guard reset.
    for(auto& node:r.zones)if(node)node->field30=nullptr;
    for(auto& link:r.portalLinks)link.reset();
    r.portals.fill(nullptr);r.portalParent=nullptr;r.sourcePortalLinksWithdrawn=true;
    d={Status::complete,0};return true;
}
}
