// Private native114E00 ->27FEE0 prefix. Selection27F650/27FC40 remains
// preorder and separate. No caller from gameplay or completion receipt exists.
namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::drawRaSelectedMethod48(const CameraVisibilityLease& lease,
    std::uint32_t methodRef,Diagnostic& d)noexcept {
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->zoneProjectionLease!=&lease||
       !methodRef||methodRef>storage_->world.zones.size()*2u)
        return fail(d,Status::identity,0x27e920u);
    auto& s=*storage_;const auto index=methodRef-1u;
    const auto& zone=s.zones[index/2u];
    if(((index&1u)?zone.secondMethod:zone.firstMethod)!=methodRef)
        return fail(d,Status::identity,0x27e920u);
    const auto& registry=s.registries[index];const auto& method=s.selectedMethods[index];
    if(!method.constructed||!method.complete||!registry.perBlock||registry.perBlock>1024u||
       registry.blockCount>1024u/registry.perBlock||registry.lastUsed>registry.perBlock||
       (!registry.blockCount&&registry.lastUsed!=registry.perBlock)||
       method.count00>method.capacity0C||method.capacity0C!=method.selected18.size())
        return fail(d,Status::registration,0x27e920u);
    //27E920 invokes27ECA0 once for EACH physical block, even if its used
    // count is zero.27ECA0 rereads the SAME selected collection, not that
    // block's registrations. Never clear, sort, deduplicate or rescan here.
    for(std::uint32_t block=0;block<registry.blockCount;++block){
        for(std::size_t selected=0;selected<method.count00;++selected){
            const auto* entity=method.selected18[selected]; // Actual27F000 result.
            if(!entity)return fail(d,Status::identity,0x27ecc8u);
            // Concrete model2418E0 ->241280/racer229D20 and their material/GPU owner
            // routes are not joined to this borrow. Stop BEFORE slot48. A
            // selected entity is not permission to draw diagnostic geometry,
            // use a racer DrawLease, accept a FIFO record, or claim success.
            return fail(d,Status::phase,0x27ecd0u);
        }
    }
    return true; // Only genuinely empty/unallocated selected dispatch paths.
}

bool Owner::drawRaZonePrefix27FEE0(CameraVisibilityLease& lease,
    OriginalApplicationEntities50LA& app,std::uint32_t parent38,
    const OriginalCameraViewportFields& viewport,OriginalZoneScissorPublication50LA& collector,
    Diagnostic& d)noexcept {
    d={};
    if(!cameraLeaseCurrent(lease)||!storage_||
       storage_->zoneProjectionLease!=&lease||storage_->staticApplication!=&app||
       collector.application_!=&app||!parent38||parent38>storage_->world.zones.size()||
       !storage_->renderAuthoringReady||!app.portalEnabled36ADA8_||app.released_||!viewport.prepared)
        return fail(d,Status::identity,0x114e00u);
    auto& s=*storage_;const auto& world=s.visibility.portals;
    if(world.zoneCount!=s.world.zones.size()||world.portalHeads!=s.portalHeads.data()||
       world.portalLinks!=s.portalLinks.data()||world.portals!=s.portalGeometry.data()||
       app.portalDepth443120_||app.portalDispatch8BE4_==0xffffu)
        return fail(d,Status::phase,0x27fee0u);
    // Root argument is unchanged through every recursive call.2806B0 uses
    // THIS root's ExclusionList against each candidate's source ordinal48.
    const auto root=parent38-1u;const auto& exclusions=s.renderAuthoring[root];
    if(exclusions.exclusionCount>exclusions.exclusions.size())
        return fail(d,Status::bounds,0x2806b0u);
    std::size_t operations=0;
    auto draw=[&](auto&& self,std::uint32_t zone)->bool {
        if(zone>=world.zoneCount||app.portalDepth443120_>=256u||++operations>65536u)
            return fail(d,Status::bounds,0x27fee0u);
        auto& row=s.zoneProjection[zone];
        ++app.portalDepth443120_; //27FF04/0C, including a later unsupported leaf.
        if(app.portalDepth443120_==1u)++app.portalDispatch8BE4_; //27FF20..28 only at top.
        row.dispatchA6=app.portalDispatch8BE4_; //27FF30 BEFORE280230/children.
        if(!prepareRaZoneScissor280230(lease,zone,app,viewport,d))return false;
        auto link=world.portalHeads[zone];std::size_t walked=0;
        while(link){
            if(link>world.portalLinkCount||++walked>world.portalLinkCount)
                return fail(d,Status::registration,0x27ff50u);
            const auto& edge=world.portalLinks[link-1u];
            if(!edge.zone||edge.zone>world.zoneCount)
                return fail(d,Status::registration,0x27ff50u);
            const auto child=edge.zone-1u;const auto& next=s.zoneProjection[child];
            if(next.admissionA4&&*next.admissionA4==app.portalAdmission8BE0_&&
               (!next.dispatchA6||*next.dispatchA6!=app.portalDispatch8BE4_)){
                bool excluded=false;
                for(std::uint32_t i=0;i<exclusions.exclusionCount;++i)
                    if(exclusions.exclusions[i]==child){excluded=true;break;}
                if(!excluded){
                    if(!self(self,child))return false; // Children FIRST, original link order.
                    // Only after a child actually returned, not on filtered or
                    // already-dispatched links. No fabricated portal debug mesh.
                    if(app.portalDrawDebug443110_)return fail(d,Status::phase,0x27ffb8u);
                }
            }
            link=edge.next;
        }
        //443118 filters only this zone's publication/methods. Recursion and
        // the top-level fallback still happen; zero means no depth filter.
        if(!app.portalDrawDepth443118_||app.portalDrawDepth443118_==app.portalDepth443120_){
            if(app.portalDebug443100_)return fail(d,Status::phase,0x28006cu);
            if(app.diagnostic14_)return fail(d,Status::phase,0x2800fcu);
            if(!row.scissorA0||!row.scissorA0->revision_)
                return fail(d,Status::identity,0x280114u);
            // Genuine280118 publication to the SAME Application collector.
            // The slot retains A0 identity (not copied endpoints); node18 will
            // borrow it only if actual acceptance is implemented later.
            collector.owner_=this;collector.cameraGeneration_=lease.generation_;
            collector.value_=row.scissorA0;
            const auto& methods=s.zones[zone];
            if(methods.firstMethod&&!drawRaSelectedMethod48(lease,methods.firstMethod,d))return false;
            if(methods.secondMethod&&methods.secondMethod!=methods.firstMethod&&
               !drawRaSelectedMethod48(lease,methods.secondMethod,d))return false;
        }
        if(app.portalDepth443120_==1u){
            //280178's443108 is a distinct same-world environment owner, NOT
            // an authored zone, null default, or the root method34. Its A4
            // admission, debug calls and first-method48 must be joined here
            // EVEN when443118 filtered this root. Never skip this tail.
            return fail(d,Status::phase,0x280178u);
        }
        --app.portalDepth443120_; //2801FC..210 only after all reached work returned.
        return true;
    };
    // Unsupported leaves retain prefix mutations, including unfinished depth.
    // RAII unwind/reset would falsely simulate source return. No retry token,
    // completed drain, or draw-ready publication is produced on this path.
    return draw(draw,root);
}
}
