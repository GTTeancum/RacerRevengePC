// Native current-camera static selection. Reuses original portal, box and LOD
// arithmetic on actual retained owners. No retail dispatch stamps, selected18
// buffers, model flags191 or callback-completion tokens are manufactured.
namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::prepareNativeRaStaticDraw(const CameraVisibilityLease& lease,
    const OriginalApplicationEntities50LA& app,const CameraViewOutput49F& camera,
    std::uint32_t parent,const OriginalCameraViewportFields& viewport,
    std::vector<NativeRaStaticDraw50LA>& output,Diagnostic& d)const noexcept {
    d={};
    if(!cameraLeaseCurrent(lease)||!storage_||storage_->scene!=Scene::ra||
       storage_->staticApplication!=&app||!storage_->staticRegistrationComplete||
       !storage_->renderAuthoringReady||!app.portalViewportConstructed_||
       !app.portalEnabled36ADA8_||app.released_||!viewport.prepared||
       viewport.x1E0<0||viewport.y1E4<0||viewport.width1E8<=0||viewport.height1EC<=0||
       !parent||parent>storage_->world.zones.size()||std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::identity,0x27f650u);
    const auto& s=*storage_;const auto& portals=s.visibility.portals;
    if(portals.zoneCount!=s.world.zones.size()||portals.portalHeads!=s.portalHeads.data()||
       portals.portalLinks!=s.portalLinks.data()||portals.portals!=s.portalGeometry.data())
        return fail(d,Status::identity,0x27f650u);
    // These genuine source controls would request debug rendering rather than
    // static material submission. Do not quietly treat them as ordinary draws.
    if(app.diagnostic14_||app.portalDrawDebug443110_)
        return fail(d,Status::phase,0x27ffb8u);
    try {
        struct Zone {bool admitted=false,drawn=false;std::array<float,4> bounds{};};
        std::array<Zone,maximumZones> zones{};
        const auto root=parent-1u;const auto& rules=s.renderAuthoring[root];
        if(rules.exclusionCount>rules.exclusions.size())return fail(d,Status::bounds,0x2806b0u);
        auto excluded=[&](std::uint32_t z){for(std::uint32_t i=0;i<rules.exclusionCount;++i)
            if(rules.exclusions[i]==z)return true;return false;};
        BodyMatrix48G combined;std::memcpy(combined.v,camera.combined.data(),sizeof combined.v);
        const Vector3_48C eye{{camera.position.x,camera.position.y,camera.position.z}};
        using P=PortalProjectionStatus50EO;
        std::size_t operations=0;
        auto edges=[&](std::uint32_t zone,auto&& each)->bool {
            auto link=portals.portalHeads[zone];std::size_t walked=0;
            while(link){
                if(link>portals.portalLinkCount||++walked>portals.portalLinkCount)
                    return fail(d,Status::registration,0x27fbc8u);
                const auto& edge=portals.portalLinks[link-1u];
                if(!edge.zone||edge.zone>portals.zoneCount||!edge.portal||edge.portal>portals.portalCount)
                    return fail(d,Status::registration,0x27fbd0u);
                if(!each(edge))return false;link=edge.next;
            }
            return true;
        };
        // Original27F930 expands accumulated bounds but intersects each next
        // portal with the incoming polygon, never with that accumulated box.
        auto visit=[&](auto&& self,std::uint32_t zone,const PortalProjectionQuad50EO& incoming,
                       std::size_t depth)->bool {
            if(depth>256u||++operations>65536u)return fail(d,Status::bounds,0x27f930u);
            auto& row=zones[zone];
            auto next=row.admitted?row.bounds:std::array<float,4>{{
                std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()}};
            for(const auto& p:incoming.xy){
                if(!std::isfinite(p[0])||!std::isfinite(p[1]))return fail(d,Status::arithmetic,0x27f9e0u);
                next[0]=std::min(next[0],p[0]);next[1]=std::max(next[1],p[0]);
                next[2]=std::min(next[2],p[1]);next[3]=std::max(next[3],p[1]);
            }
            if(row.admitted&&next==row.bounds)return true;
            row.bounds=next;row.admitted=true;
            return edges(zone,[&](const auto& edge){
                PortalProjectionQuad50EO projected,intersection;
                auto result=projectPortal230880_50EO(portals.portals[edge.portal-1u],eye,
                    camera.sceneAfter.nearPlane,combined,app.portalViewport434940_,projected);
                if(result==P::Miss)return true;
                if(result!=P::Hit)return fail(d,Status::arithmetic,0x27fbd8u);
                result=intersectPortalViewport263260_50EO(incoming,projected,intersection);
                if(result==P::Miss)return true;
                return result==P::Hit?self(self,edge.zone-1u,intersection,depth+1u):
                    fail(d,Status::arithmetic,0x27fbf0u);
            });
        };
        zones[root].admitted=true;zones[root].bounds={{-1.f,1.f,-1.f,1.f}};
        if(!edges(root,[&](const auto& edge){
            PortalProjectionQuad50EO projected,intersection;
            auto result=projectPortal230880_50EO(portals.portals[edge.portal-1u],eye,
                camera.sceneAfter.nearPlane,combined,app.portalViewport434940_,projected);
            if(result==P::Miss)return true;
            if(result!=P::Hit)return fail(d,Status::arithmetic,0x27f7d0u);
            result=intersectPortalViewport263260_50EO(app.portalViewport434940_,projected,intersection);
            if(result==P::Miss)return true;
            if(result!=P::Hit)return fail(d,Status::arithmetic,0x27f7ecu);
            return excluded(edge.zone-1u)||visit(visit,edge.zone-1u,intersection,1u);
        }))return false;
        namespace n=original_racer_model_view_detail50LA;
        auto product=[&](const BodyMatrix48G& world,const std::array<float,16>& projection,
                         std::array<float,16>& out){
            for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
                float v=n::mul(projection[c],world.v[4*r]);
                v=n::add(v,n::mul(projection[4+c],world.v[4*r+1]));
                v=n::add(v,n::mul(projection[8+c],world.v[4*r+2]));
                v=n::add(v,n::mul(projection[12+c],world.v[4*r+3]));
                if(!std::isfinite(v))return false;out[4*r+c]=v;
            }return true;
        };
        std::vector<NativeRaStaticDraw50LA> next;
        auto model=[&](std::size_t instance,std::uint32_t zone,
                       const std::array<std::uint32_t,4>& scissor)->bool {
            if(instance>=s.staticSelection.size())return fail(d,Status::identity,0x27eb60u);
            const auto& binding=s.staticSelection[instance];
            if(!binding.model||binding.model!=s.staticModels[instance]||!binding.membership||
               !binding.membership->attached||!validateSceneGraph49P(binding.graph)||binding.graph.count!=1)
                return fail(d,Status::identity,0x26efc0u);
            const auto* m=binding.model->model();const auto& fields=binding.model->fields();
            if(!m||m->released||m->observer19C!=binding.observer||m->sceneIndex!=SceneNodeIndex49P(0))
                return fail(d,Status::identity,0x26efc0u);
            if(!(m->flags03&1u)||(m->flags03&4u)||!(m->entity().flags29&1u))return true;
            if(m->type02!=s.selectedMethods[zone*2u].type2C){
                if(m->type02==s.selectedMethods[zone*2u].type2D)return fail(d,Status::phase,0x27ec18u);
                return true;
            }
            if(!refreshSceneWorld49P(binding.graph,SceneNodeIndex49P(0)))
                return fail(d,Status::arithmetic,0x2897b0u);
            const auto& node=binding.graph.nodes[0];
            std::array<float,16> projection=camera.combined;
            if(app.useMembership3833D8_){
                const auto& member=*binding.membership;
                if(!member.storage.links||!member.storage.entries||!member.storage.capacity||
                   !member.storage.entries[0].used)return fail(d,Status::registration,0x240cacu);
                // Source240CAC reads retained bounds even for non-admitted
                // memberships. A current native publication has no stale row:
                // union only this camera's admitted membership viewports.
                // Every submitted registry entry must include its draw zone.
                std::array<float,4> bounds{};std::uint32_t selectedZone=0;
                std::size_t admitted=0,walked=0;bool containsDrawZone=false;
                auto link=std::uint32_t{1};
                const bool multiple=member.storage.links[0].next!=0;
                while(link){
                    if(link>member.storage.capacity||++walked>member.storage.capacity||
                       !member.storage.entries[link-1u].used)return fail(d,Status::registration,0x240ce0u);
                    const auto& other=member.storage.links[link-1u];
                    if(!other.zone||other.zone>portals.zoneCount)
                        return fail(d,Status::registration,0x240ce4u);
                    containsDrawZone|=other.zone-1u==zone;
                    if(zones[other.zone-1u].admitted){
                        const auto& b=zones[other.zone-1u].bounds;
                        if(!admitted++){
                            bounds=multiple?std::array<float,4>{{b[0],b[1],b[0],b[1]}}:b;
                            selectedZone=other.zone-1u;
                        }else{
                            // Literal original240CBC..240DCC uses X for both
                            // pairs in its multi-membership union.
                            bounds[0]=std::min(bounds[0],b[0]);bounds[1]=std::max(bounds[1],b[1]);
                            bounds[2]=std::min(bounds[2],b[0]);bounds[3]=std::max(bounds[3],b[1]);
                            if(bounds[0]==b[0]&&bounds[1]==b[1]&&bounds[2]==b[0]&&bounds[3]==b[1])
                                selectedZone=other.zone-1u;
                        }
                    }
                    link=other.next;
                }
                if(!admitted||!containsDrawZone)return fail(d,Status::registration,0x240cacu);
                if(multiple){
                    const auto& b=zones[selectedZone].bounds;
                    if(bounds[0]==b[0]&&bounds[1]==b[1]&&bounds[2]==b[0]&&bounds[3]==b[1])
                        bounds=b; // Exact source row reuse retains that row's Y pair.
                }
                PortalZoneViewOutput49F selected;
                if(!composePortalZoneView280720(camera,bounds,selected))return fail(d,Status::arithmetic,0x280720u);
                projection=selected.matrix60;
            }
            std::array<float,16> clipMatrix;OriginalUiRiderBoxClip50KG clipped;
            if(!product(node.world100,projection,clipMatrix)||
               !projectOriginalUiRiderBoxClip50KG(clipMatrix,m->bounds160,m->bounds170,clipped))
                return fail(d,Status::arithmetic,0x240f04u);
            if(clipped.outside)return true;
            const auto radius=m->nodeCount1A0==1?m->radius194:m->radius198;
            const auto& center=m->nodeCount1A0==1?m->bounds140:m->bounds160;
            if(!radius||!std::isfinite(*radius)||*radius<0.f||!fields.geometry1B4||
               !fields.geometry1B4->payload()||!fields.geometry1B4->scale14())
                return fail(d,Status::identity,0x26f090u);
            float distance=0;
            const std::array<float,3> local{{node.localC0.v[12],node.localC0.v[13],node.localC0.v[14]}};
            const std::array<float,3> position{{camera.position.x,camera.position.y,camera.position.z}};
            if(!projectOriginalRacerDistance26F070(center,local,position,distance))return fail(d,Status::arithmetic,0x26f190u);
            const float denominator=std::max(.1f,n::sub(distance,*radius));
            volatile float scale=viewport.focalPixels1D8/denominator;
            const float threshold=n::mul(scale,app.modelLod383360_);
            const auto* resource=fields.geometry1B4->payload()->scene();
            if(!std::isfinite(scale)||!std::isfinite(threshold)||!resource||resource->lods.empty()||resource->lods.size()>256)
                return fail(d,Status::arithmetic,0x26f248u);
            std::uint32_t lod=0;
            for(std::size_t i=0;i+1<resource->lods.size();++i){
                const float cutoff=static_cast<float>(resource->lods[i].cutoff);
                if(!std::isfinite(cutoff)||double(cutoff)!=resource->lods[i].cutoff)return fail(d,Status::arithmetic,0x26f21cu);
                const float value=n::mul(*fields.geometry1B4->scale14(),n::mul(threshold,cutoff));
                if(!std::isfinite(value))return fail(d,Status::arithmetic,0x26f224u);
                if(value<1.f)lod=static_cast<std::uint32_t>(i+1u);
            }
            // Previous demand is only a bounded allocation hint, never reused selection.
            if(next.empty())next.reserve(std::min(output.size(),s.staticSelection.size()));
            next.push_back({instance,lod,zone,scissor,{{viewport.x1E0,viewport.y1E4,viewport.width1E8,viewport.height1EC}}});
            return true;
        };
        // Native ordered records follow original child-first27FEE0 zone order
        // and the actual static registry slot order. Dynamic racers remain
        // owned by the existing racer draw path; no AI state is changed here.
        operations=0;
        auto draw=[&](auto&& self,std::uint32_t zone,std::size_t depth)->bool {
            if(depth>256u||++operations>65536u)return fail(d,Status::bounds,0x27fee0u);
            auto& row=zones[zone];if(row.drawn)return true;row.drawn=true;
            if(!edges(zone,[&](const auto& edge){const auto child=edge.zone-1u;
                return !zones[child].admitted||zones[child].drawn||excluded(child)||self(self,child,depth+1u);
            }))return false;
            if(app.portalDrawDepth443118_&&app.portalDrawDepth443118_!=depth)return true;
            std::array<std::uint32_t,4> scissor{};
            for(unsigned i=0;i<4;++i){
                const auto& b=row.bounds;
                volatile float shifted=i<2?1.f+b[i]:1.f-b[i==2?3:2];
                volatile float half=shifted/2.f;
                volatile float extent=half*static_cast<float>(i<2?viewport.width1E8:viewport.height1EC);
                volatile float pixel=extent+static_cast<float>(i<2?viewport.x1E0:viewport.y1E4);
                if(!std::isfinite(pixel)||pixel<0.f||pixel>2047.f)return fail(d,Status::arithmetic,0x2805b4u);
                scissor[i]=static_cast<std::uint32_t>(pixel);
            }
            const auto& registry=s.registries[zone*2u];
            if(!registry.perBlock||registry.perBlock>1024u||registry.blockCount>1024u/registry.perBlock||
               registry.lastUsed>registry.perBlock)return fail(d,Status::registration,0x27e894u);
            for(std::uint32_t block=0;block<registry.blockCount;++block){
                const auto count=block+1u==registry.blockCount?registry.lastUsed:registry.perBlock;
                for(std::uint32_t slot=0;slot<count;++slot){
                    const auto word=registry.words[block*registry.perBlock+slot];if(word&2u)continue;
                    const auto ref=(word&~1u)>>2u;
                    if(!(word&~1u)||((word&~1u)&3u)||ref<s.racerCount+2u)
                        return fail(d,Status::identity,0x27eb60u);
                    if(const auto* flareEntry=s.findRaFlare(ref)){
                        const auto& flare=*flareEntry;
                        if(!flare.world||flare.world!=s.sceneOwner||!flare.entity||!flare.type32||!flare.flags33||
                           !flare.world->nativeLocalFlareIdentityCurrent(flare.entity)||
                           flare.world->nativeLocalFlareZoneIndex(flare.entity)!=std::optional<std::size_t>(zone)||
                           *flare.type32!=s.selectedMethods[zone*2u].type2D)
                            return fail(d,Status::identity,0x27ec18u);
                        // Distinct native typed projection of the real SearchableGameObject.
                        // No retail27EC18 callback/stamp receipt, no model relabelling.
                        if((*flare.flags33&1u)&&!(*flare.flags33&4u)&&(flare.entity->flags29&1u)){
                            NativeRaStaticDraw50LA record{};record.zone=zone;record.scissor=scissor;
                            record.viewport={{viewport.x1E0,viewport.y1E4,viewport.width1E8,viewport.height1EC}};
                            record.kind=NativeRaStaticDraw50LA::Kind::lightFlare;record.flareIdentity=flare.entity;
                            next.push_back(record);
                        }
                        continue;
                    }
                    if(!model(ref-s.racerCount-2u,zone,scissor))return false;
                }
            }
            return true;
        };
        if(!draw(draw,root,1u)||!cameraLeaseCurrent(lease))return false;
        output=std::move(next);return true;
    }catch(...){return fail(d,Status::allocation,0x27eea0u);}
}
}
