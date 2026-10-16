// Private partial114D00 ->27F650 operation beside actual4C0 Storage. There is
// deliberately no public projection/stamp accessor or drawing capability.
namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::prepareRaZonePrefix114D00(CameraVisibilityLease& lease,
    OriginalApplicationEntities50LA& app,const CameraViewOutput49F& camera,
    std::uint32_t parent38,const OriginalCameraViewportFields& viewport,
    const assets::OriginalStandardMaterialTable50LA& materials,
    std::array<OriginalRacerEntityCold50LA,8>& racers,const OriginalCameraSceneOwner& actualCamera,
    const RaRacerSelectionDispatch50LA& dispatch,const OriginalRendererDispatchFields& rendererFields,
    platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& rendererDiagnostic,
    Diagnostic& d)noexcept {
    d={};
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->scene!=Scene::ra)
        return fail(d,Status::identity,0x114d28);
    auto& s=*storage_;auto& world=s.visibility.portals;
    if(s.staticApplication!=&app||!s.staticOwner||!s.staticRegistrationComplete||
       !s.renderAuthoringReady||!app.portalViewportConstructed_||app.released_||!app.portalEnabled36ADA8_||
       !parent38||parent38>world.zoneCount||world.zoneCount!=s.world.zones.size()||
       world.portalHeads!=s.portalHeads.data()||world.portalLinks!=s.portalLinks.data()||
       world.portals!=s.portalGeometry.data()||s.zoneProjectionLease)
        return fail(d,Status::identity,0x27f650);
    // A bounded native domain: do not pretend unknown dynamic-heap stamps
    // were initialized by27F120, or let16-bit wrap turn them into admissions.
    // Source wraps; this inactive adapter explicitly refuses that boundary.
    if(app.portalAdmission8BE0_==0xffffu||app.portalDispatch8BE4_==0xffffu||
       app.portalDepth443120_||app.diagnostic14_)
        return fail(d,Status::phase,0x27f6a8);
    const auto root=parent38-1u;
    s.zoneProjectionLease=&lease;
    ++app.portalAdmission8BE0_; //27F6A8..BC, process lifetime, not host frame.
    auto& first=s.zoneProjection[root];
    first.admissionA4=app.portalAdmission8BE0_;
    first.bounds4C=std::array<float,4>{{-1.f,1.f,-1.f,1.f}}; //27F6C4..D4
    // Actual Application14 is null from13FBF8; no diagnostic branch is erased
    // on behalf of a caller-supplied boolean.
    app.portalDebug443100_=0; //27F7B4..BC
    BodyMatrix48G combined;std::memcpy(combined.v,camera.combined.data(),sizeof combined.v);
    const Vector3_48C position{{camera.position.x,camera.position.y,camera.position.z}};
    using P=PortalProjectionStatus50EO;
    // Native stack/work admission only. Hitting a bound is failure, never cull.
    std::size_t operations=0;
    constexpr std::size_t operationLimit=65536,depthLimit=256;
    auto visit=[&](auto&& self,std::uint32_t zone,const PortalProjectionQuad50EO& incoming,
                   std::size_t depth)->bool {
        if(zone>=world.zoneCount||depth>depthLimit||++operations>operationLimit)
            return fail(d,Status::bounds,0x27f930);
        auto& row=s.zoneProjection[zone];
        //27F988..B4: unknown native stamp is explicitly not current. All
        // produced source values remain retained across invalidated leases.
        if(!row.admissionA4||*row.admissionA4!=app.portalAdmission8BE0_)
            row.bounds4C=std::array<float,4>{{std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max()}};
        if(!row.bounds4C)return fail(d,Status::identity,0x27f9b8);
        const auto old=*row.bounds4C;auto next=old;
        for(const auto& point:incoming.xy){
            if(!std::isfinite(point[0])||!std::isfinite(point[1]))
                return fail(d,Status::arithmetic,0x27f9e0);
            if(point[0]<next[0])next[0]=point[0];
            if(point[0]>next[1])next[1]=point[0];
            if(point[1]<next[2])next[2]=point[1];
            if(point[1]>next[3])next[3]=point[1];
        }
        if(next==old)return true; //27FA50..27FB28, no recursive expansion.
        row.bounds4C=next;row.admissionA4=app.portalAdmission8BE0_; //27FB2C..C4
        auto link=world.portalHeads[zone];std::size_t walked=0;
        while(link){
            if(link>world.portalLinkCount||++walked>world.portalLinkCount)
                return fail(d,Status::registration,0x27fbc8);
            const auto& edge=world.portalLinks[link-1];
            if(!edge.zone||edge.zone>world.zoneCount||!edge.portal||edge.portal>world.portalCount)
                return fail(d,Status::registration,0x27fbd0);
            PortalProjectionQuad50EO projected,intersection;
            auto result=projectPortal230880_50EO(world.portals[edge.portal-1],position,
                camera.sceneAfter.nearPlane,combined,app.portalViewport434940_,projected);
            if(result!=P::Miss){
                if(result!=P::Hit)return fail(d,Status::arithmetic,0x27fbd8);
                // Incoming polygon, NOT the accumulated bounds rectangle.
                result=intersectPortalViewport263260_50EO(incoming,projected,intersection);
                if(result!=P::Miss){
                    if(result!=P::Hit)return fail(d,Status::arithmetic,0x27fbf0);
                    if(!self(self,edge.zone-1u,intersection,depth+1u))return false;
                }
            }
            link=edge.next; // Original linked order, no sorting/dedup.
        }
        return true;
    };
    auto link=world.portalHeads[root];std::size_t walked=0;
    while(link){
        if(link>world.portalLinkCount||++walked>world.portalLinkCount)
            return fail(d,Status::registration,0x27f7c0);
        const auto& edge=world.portalLinks[link-1];
        if(!edge.zone||edge.zone>world.zoneCount||!edge.portal||edge.portal>world.portalCount)
            return fail(d,Status::registration,0x27f7c8);
        PortalProjectionQuad50EO projected,intersection;
        auto result=projectPortal230880_50EO(world.portals[edge.portal-1],position,
            camera.sceneAfter.nearPlane,combined,app.portalViewport434940_,projected);
        if(result!=P::Miss){
            if(result!=P::Hit)return fail(d,Status::arithmetic,0x27f7d0);
            result=intersectPortalViewport263260_50EO(app.portalViewport434940_,projected,intersection);
            if(result!=P::Miss){
                if(result!=P::Hit)return fail(d,Status::arithmetic,0x27f7ec);
                bool excluded=false;const auto& rules=s.renderAuthoring[root];
                for(std::uint32_t i=0;i<rules.exclusionCount;++i)
                    if(rules.exclusions[i]==edge.zone-1u){excluded=true;break;}
                //2806B0 applies only to the first hop; recursion has no copy.
                if(!excluded&&!visit(visit,edge.zone-1u,intersection,1))return false;
            }
        }
        link=edge.next;
    }
    PortalZoneViewOutput49F matrix;
    if(!composePortalZoneView280720(camera,*first.bounds4C,matrix))
        return fail(d,Status::arithmetic,0x280720);
    first.matrix60=matrix; //27F830, only this zone before its method callbacks.
    ++app.portalDispatch8BE4_;first.dispatchA6=app.portalDispatch8BE4_; //27F838..48
    if(s.zones[root].firstMethod&&!selectRaStaticMethod44(lease,app,s.zones[root].firstMethod,
        camera,viewport,materials,rendererFields,renderer,rendererDiagnostic,d))return false;
    if(s.zones[root].secondMethod&&s.zones[root].secondMethod!=s.zones[root].firstMethod&&
       !selectRaDynamicMethod44(lease,app,s.zones[root].secondMethod,racers,actualCamera,dispatch,
           rendererFields,renderer,rendererDiagnostic,d))return false;
    //27F890 /27FC40: neighboring selection is PREORDER, unlike27FEE0 draw.
    // This continuation is reached only if every prior real callback returned.
    operations=0;
    auto neighbors=[&](auto&& self,std::uint32_t zone,std::size_t depth)->bool {
        if(zone>=world.zoneCount||depth>depthLimit||++operations>operationLimit)
            return fail(d,Status::bounds,0x27fc40u);
        auto& row=s.zoneProjection[zone];
        if(row.dispatchA6&&*row.dispatchA6==app.portalDispatch8BE4_)return true;
        row.dispatchA6=app.portalDispatch8BE4_; // Before admission test27FCAC.
        if(!row.admissionA4||*row.admissionA4!=app.portalAdmission8BE0_)return true;
        if(!row.bounds4C)return fail(d,Status::identity,0x280700u);
        PortalZoneViewOutput49F view;
        if(!composePortalZoneView280720(camera,*row.bounds4C,view))
            return fail(d,Status::arithmetic,0x280720u);
        row.matrix60=view;
        if(app.portalDebug443100_||app.diagnostic14_)return fail(d,Status::phase,0x27fcc4u);
        const auto& methods=s.zones[zone];
        if(methods.firstMethod&&!selectRaStaticMethod44(lease,app,methods.firstMethod,camera,
            viewport,materials,rendererFields,renderer,rendererDiagnostic,d))return false;
        if(methods.secondMethod&&methods.secondMethod!=methods.firstMethod&&
           !selectRaDynamicMethod44(lease,app,methods.secondMethod,racers,actualCamera,dispatch,
               rendererFields,renderer,rendererDiagnostic,d))return false;
        auto edge=world.portalHeads[zone];std::size_t links=0;
        while(edge){
            if(edge>world.portalLinkCount||++links>world.portalLinkCount)
                return fail(d,Status::registration,0x27fe24u);
            const auto& child=world.portalLinks[edge-1u];
            if(!child.zone||child.zone>world.zoneCount)return fail(d,Status::registration,0x27fe24u);
            if(!self(self,child.zone-1u,depth+1u))return false;
            edge=child.next;
        }
        return true;
    };
    link=world.portalHeads[root];walked=0;
    while(link){
        if(link>world.portalLinkCount||++walked>world.portalLinkCount)
            return fail(d,Status::registration,0x27f89cu);
        const auto& edge=world.portalLinks[link-1u];
        if(!edge.zone||edge.zone>world.zoneCount)return fail(d,Status::registration,0x27f89cu);
        if(!neighbors(neighbors,edge.zone-1u,1u))return false;
        link=edge.next;
    }
    //27F8B8 consumes the actual SAME root44, produced before that zone's
    //static wrapper registration. Authored metadata alone is not this write.
    if(!first.environment44Published)return fail(d,Status::phase,0x27f8b8u);
    if(first.environmentVisible44){
        //The global443108 owner is distinct from every authored4C0 zone.
        //Its real world4BC attachment and VisibilityQuadTree method lifetime
        //are not yet joined. Stop before its load; never assume null/empty.
        return fail(d,Status::phase,0x27f8c8u);
    }
    //The source zero44 route returns from27F650. The enclosing114D00 still
    //owes its real141C30 camera-child traversal, so issue no completion here.
    return fail(d,Status::phase,0x114d34u);
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::prepareRaZoneScissor280230(const CameraVisibilityLease& lease,std::uint32_t zone,
    const OriginalApplicationEntities50LA& app,const OriginalCameraViewportFields& viewport,Diagnostic& d)noexcept {
    d={};
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->scene!=Scene::ra||
       storage_->staticApplication!=&app||storage_->zoneProjectionLease!=&lease||
       zone>=storage_->world.zones.size()||!app.portalDepth443120_||!viewport.prepared)
        return fail(d,Status::phase,0x27ff38u);
    auto& row=storage_->zoneProjection[zone];
    if(!row.bounds4C||!row.admissionA4||*row.admissionA4!=app.portalAdmission8BE0_||
       !row.dispatchA6||*row.dispatchA6!=app.portalDispatch8BE4_)
        return fail(d,Status::phase,0x27ff38u);
    // Only the bounded positive viewport domain is admitted. Source reads
    // these words unsigned; do not reinterpret negative host values as valid.
    if(viewport.x1E0<0||viewport.y1E4<0||viewport.width1E8<=0||viewport.height1EC<=0||
       std::fegetround()!=FE_TONEAREST)return fail(d,Status::arithmetic,0x28036cu);
    try {
        if(!row.scissorA0)row.scissorA0=std::shared_ptr<OriginalZoneScissor50LA>(new OriginalZoneScissor50LA(zone));
        auto& out=*row.scissorA0;
        if(out.revision_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::phase,0x28036cu);
        const auto& b=*row.bounds4C;std::array<std::uint32_t,4> coordinates{};
        for(unsigned i=0;i<4;++i){
            //28036C..2805D4: float add/sub, divide, multiply, add, then
            //trunc.w.s. No OpenGL origin flip, endpoint shrink or clamping.
            if(!std::isfinite(b[i]))return fail(d,Status::arithmetic,0x28036cu);
            volatile float shifted=i<2?1.f+b[i]:1.f-b[i==2?3:2];
            volatile float half=shifted/2.f;
            volatile float extent=half*static_cast<float>(i<2?viewport.width1E8:viewport.height1EC);
            volatile float pixel=extent+static_cast<float>(i<2?viewport.x1E0:viewport.y1E4);
            if(!std::isfinite(pixel)||pixel<0.f||pixel>2047.f)
                return fail(d,Status::arithmetic,0x2805b4u); // Broader packed overflow remains unsupported.
            coordinates[i]=static_cast<std::uint32_t>(pixel);
        }
        out.endpoints_=coordinates;++out.revision_;
        // Produces authentic A0 numeric data only.280118 publication cannot
        //occur until the real27FEE0 child traversal has returned in order.
        return true;
    }catch(...){return fail(d,Status::allocation,0x280250u);}
}
}
