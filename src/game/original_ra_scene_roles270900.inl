// Included only after this actual lighting Owner's private Storage definition.
// No new EnvironmentInfo, light registry, topology, or decoded World is built.
namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::publishNativeEnvironment3A4(::ra_pod_scene50LA::Owner& owner,
    const assets::World48F& source,std::uint32_t& pc)noexcept {
    pc=0x270a70;
    if(!podApplication(owner)||podApplication(owner)->nativeOwnerThread_!=std::this_thread::get_id()||
       !podColdConstructed(owner)||podTopologyClosed(owner)||podLighting(owner).get()!=this||
       podApplication(owner)->attachmentLock423F58()||!podEnvironment3A4(owner)||
       !storage_||invalidated_||storage_->scene!=Scene::ra||
       (storage_->sceneOwner&&storage_->sceneOwner!=&owner)||storage_->staticBusy()||
       storage_->activeLease||storage_->activeCamera||storage_->activeReset||storage_->resetPending())return false;
    auto& s=*storage_;
    if(!s.environment3A4||!s.environment3A4->prepared()||!s.environment3A4->world()||
       source.parameters.original_bytes!=s.world.parameters.original_bytes||
       s.environment3A4->world()->parameters.original_bytes!=source.parameters.original_bytes||
       source.zones.size()!=10u||s.environment3A4->zoneCount()!=10u||
       !s.environment3A4->spatialBounds534())return false;
    //A same-owner repeat observes a prior publication; it does not reconstruct
    //or reset the backing, nor manufacture276950/276F3C source receipts.
    if(*podEnvironment3A4(owner))return *podEnvironment3A4(owner)==s.environment3A4.get();
    podEnvironment3A4(owner)=s.environment3A4.get();
    return true;
}
bool Owner::withdrawNativeEnvironment3A4(::ra_pod_scene50LA::Owner& owner)noexcept {
    if(!podEnvironment3A4(owner))return false;
    if(!*podEnvironment3A4(owner))return true;
    if(!storage_||podLighting(owner).get()!=this||
       (storage_->sceneOwner&&storage_->sceneOwner!=&owner)||storage_->activeLease||
       storage_->activeCamera||storage_->activeReset||storage_->resetPending()||
       *podEnvironment3A4(owner)!=storage_->environment3A4.get())return false;
    //Native borrower withdrawal, NOT destruction/replacement of EnvironmentInfo.
    podEnvironment3A4(owner)=nullptr;return true;
}

bool Owner::nativeSceneRoles270900Current(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* actualRows,std::uint32_t& pc)const noexcept {
    pc=0x270a50;
    if(!owner||!storage_||invalidated_||storage_->scene!=Scene::ra||
       storage_->sceneOwner!=owner||storage_->staticBusy()||storage_->activeLease||
       storage_->activeCamera||storage_->activeReset||storage_->resetPending())return false;
    const auto& s=*storage_;
    if(source.parameters.original_bytes!=s.world.parameters.original_bytes||
       source.zones.size()!=10u||source.portals.size()!=10u||
       s.zones.data()!=actualRows||s.zones.size()<10u||
       s.visibility.lifecycle.storage.views.zones!=actualRows||s.visibility.lifecycle.zoneCount!=10u)
        return false;
    pc=0x270a70; //Existing native EnvironmentInfo3A4, not276950 construction.
    if(!podEnvironment3A4(*owner)||*podEnvironment3A4(*owner)!=s.environment3A4.get()||
       !s.environment3A4||!s.environment3A4->prepared()||!s.environment3A4->world()||
       s.environment3A4->world()->parameters.original_bytes!=source.parameters.original_bytes||
       s.environment3A4->zoneCount()!=10u||!s.environment3A4->spatialBounds534())return false;
    pc=0x270ac4; //Existing actual per-zone light/ambient source rows.
    if(s.sun44.size()<10u||s.ambient60.size()<10u||!s.fixedGcGlobalAmbient)return false;
    pc=0x270c78; //Distinct4C0 portal backing, from the SAME authored portals.
    const auto& portals=s.visibility.portals;
    if(portals.zoneCount!=10u||portals.portalCount!=10u||portals.portalLinkCount!=20u||
       portals.portalHeads!=s.portalHeads.data()||portals.portalLinks!=s.portalLinks.data()||
       portals.portals!=s.portalGeometry.data())return false;
    pc=0x270e80;
    //270E9C absent-key branch is genuinely supported by this RA owner. No
    //synthetic flash clock, copied global-light lifetime, or default white.
    const auto* environment=block(s.world.parameters,"environment");
    if(!environment||field(*environment,"ambientcolorflashminidletime"))return false;
    pc=0x270fb0;return true; //Native role validation, NOT270900's return value.
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const AuthoredRenderZone* Owner::nativeRaZoneExclusions272B08(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::size_t ordinal,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc)||ordinal>=10u||
       !storage_->renderAuthoringReady||storage_->renderAuthoring.size()<10u)return nullptr;
    pc=0x272b08;return &storage_->renderAuthoring[ordinal];
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const PortalGeometry50EO* Owner::nativeRaPortal230450(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,
    const OriginalApplicationEntities50LA* app,std::size_t ordinal,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc)||ordinal>=10u||
       !app||storage_->staticApplication!=app||!storage_->staticOwner||
       !app->portalViewportConstructed_||storage_->portalGeometry.size()<10u)return nullptr;
    //Actual earlier bindRaStaticOwner produced THIS Application434940 and
    //THIS visibility-owner geometry. Never substitute collision-side backing.
    const auto& native=storage_->portalGeometry[ordinal];
    const auto& authored=source.portals[ordinal];
    const auto& retained=storage_->world.portals[ordinal];
    if(authored.zones!=retained.zones||authored.parameter_block!=retained.parameter_block||
       authored.vertices!=retained.vertices)return nullptr;
    if(authored.parameter_block>=source.parameters.blocks.size())return nullptr;
    //The real producer parses retained decimal text with50AD. World48F's
    //binary64 decimal projection is NOT promoted native binary32 geometry.
    constexpr const char* keys[]={"vertex1","vertex2","vertex3","vertex4"};
    for(std::size_t v=0;v<4;++v){
        light::Vec3 value{};
        if(!triple(source.parameters.blocks[authored.parameter_block],keys[v],value))return nullptr;
        for(std::size_t axis=0;axis<3;++axis)if(native.vertices[v].v[axis]!=value[axis])return nullptr;
    }
    pc=0x230450;return &native;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::raVisibilityListCustodyCurrent(const ::ra_pod_scene50LA::Owner* owner,
    const OriginalNativeListMethodCustody50LA* custody)const noexcept {
    if(!owner||!custody||!storage_||storage_->scene!=Scene::ra||storage_->selectedMethods.empty()||
       storage_->methods.empty()||storage_->registries.empty()||storage_->activeLease||
       storage_->activeCamera||storage_->activeReset||storage_->resetPending())return false;
    const auto& s=*storage_;const auto index=custody->ordinal_;
    if(index>=20u||index>=s.selectedMethods.size()||index>=s.methods.size()||index>=s.registries.size())return false;
    const auto& method=s.selectedMethods[index];const auto& c=method.sourceList;
    if(custody!=&c||!c.begun_||c.world_!=owner||!c.application_||
       c.method_!=&s.methods[index]||c.registry_!=&s.registries[index]||c.selectionOwner_!=&method||
       c.reference00_!=std::uint64_t(c.arrayReference_)+std::uint64_t(c.nodeReference_)||!c.reference00_||
       c.usage10_!=(c.nodeAliasWasAcquired_?((index&1u)?2u:1u):0u)||(c.nodeReference_&&!c.nodeAliasWasAcquired_)||!method.constructed||
       method.count00>method.capacity0C||method.capacity0C!=method.selected18.size())return false;
    return true; //May outlive staticOwner lease; SAME payload deliberately retained.
}
OriginalNativeListMethodCustody50LA* Owner::adoptRaVisibilityList(
    const ::ra_pod_scene50LA::Owner* owner,const assets::World48F& world,const CollectionZone50BX* rows,
    const OriginalApplicationEntities50LA* app,std::size_t index,std::uint32_t& pc)noexcept {
    if(!nativeSceneRoles270900Current(owner,world,rows,pc)||!app||storage_->staticApplication!=app||
       !storage_->staticOwner||storage_->selectedMethods.empty()||storage_->methods.empty()||
       storage_->registries.empty()||index>=20u||index>=storage_->selectedMethods.size()||
       index>=storage_->methods.size()||index>=storage_->registries.size())return nullptr;
    const auto selection=assets::readZoneSearchSelection50CN(world,index/2u);
    if(selection.status!=assets::ZoneSearchReadStatus50CN::Complete||
       ((index&1u)?selection.selection.visibilityStatic110:selection.selection.collisionStatic118)!=assets::ZoneSearchKind50CN::List)return nullptr;
    auto& s=*storage_;auto& method=s.selectedMethods[index];auto& c=method.sourceList;
    if(c.reference00_)return raVisibilityListCustodyCurrent(owner,&c)?&c:nullptr;
    pc=(index&1u)?0x273668:0x2731d8;
    const auto& registry=s.registries[index];const auto& collection=s.methods[index];
    if(c.begun_||!method.constructed||method.count00||method.capacity0C!=30u||method.selected18.size()!=30u||
       method.type2C==0xffu||method.type2D==0xffu||collection.method!=CollectionTree50BX::Method::SearchList||
       ((index&1u)?s.zones[index/2u].secondMethod:s.zones[index/2u].firstMethod)!=index+1u||registry.perBlock!=0x400u||registry.blockCount>1u||
       registry.lastUsed>0x400u||collection.searchList.perBlock30!=0x400||
       collection.searchList.blockCount!=static_cast<std::int32_t>(registry.blockCount)||
       collection.searchList.lastUsed38!=static_cast<std::int32_t>(registry.lastUsed))return nullptr;
    for(const auto* selected:method.selected18)if(selected)return nullptr;
    if(registry.blockCount&&(!collection.searchList.blocks||collection.searchList.blockCapacity<2u||
       collection.searchList.blocks[0].words!=registry.words||collection.searchList.blocks[0].capacity!=0x400u))return nullptr;
    //Explicit native owner-local lifetime initialization. Existing real registry
    //memberships, selected30 allocation and App type IDs are adopted unchanged;
    //no27E260 replay, empty registry claim or duplicated List backend.
    c.world_=owner;c.application_=app;c.method_=&s.methods[index];c.registry_=&s.registries[index];
    c.selectionOwner_=&method;c.ordinal_=index;c.reference00_=1;c.usage10_=0;
    c.arrayReference_=true;c.nodeReference_=false;c.begun_=true;
    pc=(index&1u)?0x27367c:0x2731ec;return &c;
}
bool Owner::retainRaVisibilityListNode(const ::ra_pod_scene50LA::Owner* owner,
    OriginalNativeListMethodCustody50LA* c,std::uint32_t& pc)noexcept {
    pc=c&&(c->ordinal_&1u)?0x273694:0x273204;
    if(!raVisibilityListCustodyCurrent(owner,c)||!c->arrayReference_||c->nodeReference_||c->reference00_!=1u)return false;
    c->usage10_=(c->ordinal_&1u)?2u:1u;c->nodeReference_=true;c->nodeAliasWasAcquired_=true;++c->reference00_; //27F458/468.
    pc=(c->ordinal_&1u)?0x27369c:0x27320c;return true;
}
bool Owner::releaseRaVisibilityListReference(const ::ra_pod_scene50LA::Owner* owner,
    OriginalNativeListMethodCustody50LA* c,bool nodeAlias)noexcept {
    if(!raVisibilityListCustodyCurrent(owner,c)||
       (nodeAlias?!c->nodeReference_:!c->arrayReference_))return false;
    if(nodeAlias)c->nodeReference_=false;else c->arrayReference_=false;
    --c->reference00_;
    if(!c->reference00_){
        c->world_=nullptr;c->application_=nullptr;c->method_=nullptr;c->registry_=nullptr;c->selectionOwner_=nullptr;
        //Native static lease already ended: release deferred SAME selected30
        //payload now. Registry/collection backing stays with canonical Storage
        //until its existing consumers/owners retire; no duplicated free.
        if(!storage_->staticOwner)storage_->selectedMethods[c->ordinal_].resetNativeSelection();
    }
    return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::nativeRaLightIdentity(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::size_t index,
    RaAuthoredLightIdentity& out,std::uint32_t& pc)const noexcept {
    if(index>=185u||!nativeSceneRoles270900Current(owner,source,rows,pc))return false;
    pc=0x273ba8;
    try {
        const auto& s=*storage_;const auto* lights=block(s.world.parameters,"lights");
        if(!lights||!textField(*lights,"numberoflights","185")||s.localLightCount!=180u)return false;
        const auto sourceOrdinal=static_cast<std::uint32_t>(index+1u);
        const auto effective=(sourceOrdinal==90u||sourceOrdinal==107u)?sourceOrdinal-1u:sourceOrdinal;
        const auto name="light"+std::to_string(sourceOrdinal);
        if(effective!=sourceOrdinal&&block(s.world.parameters,name.c_str()))return false;
        const auto effectiveName="light"+std::to_string(effective);
        const auto* b=block(s.world.parameters,effectiveName.c_str());if(!b)return false;
        std::uint32_t zone=1u; //Actual2774C8 missing-key default1, then zero-based2774FC.
        if(const auto* authoredZone=field(*b,"initialzone")){
            zone=0;
            for(std::uint32_t candidate=1;candidate<=10u;++candidate)
                if(authoredZone->value==std::to_string(candidate)){zone=candidate;break;}
            if(!zone)return false; //Bounded authenticated decimal spellings, not generic atoi.
        }
        RaAuthoredLightIdentity result;result.retainedSource=b;result.canonicalStorage=&s;
        result.ordinal=sourceOrdinal;result.effectiveOrdinal=effective;result.zone20=zone-1u;
        if(textField(*b,"type","Ambient"))result.type00=4;
        else if(textField(*b,"type","Directional"))result.type00=2;
        else if(textField(*b,"type","ObjectPoint"))result.type00=1;
        else if(textField(*b,"type","Spotlight"))result.type00=3;
        else return false;
        std::size_t matches=0;
        for(std::size_t i=0;i<s.localLightCount;++i){const auto& local=s.localLights[i];
            if(local.sourceOrdinal!=sourceOrdinal)continue;
            if(local.effectiveSourceOrdinal!=effective||local.zone!=zone||
               (local.spotlight?3u:1u)!=result.type00)return false;
            result.localPrimitive=&local;++matches;
        }
        if(matches!=((result.type00==1u||result.type00==3u)?1u:0u))return false;
        //Final ambient/sun arrays collapse source contributions. They are NOT
        //five already-existing Light objects; retain distinct authored identity
        //and same canonical backend without fabricating per-light runtime rows.
        out=result;return true;
    }catch(...){return false;}
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::nativeRaDirectionalInputs(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,
    RaDirectionalLightInputs& out,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc)||std::fegetround()!=FE_TONEAREST)return false;
    pc=0x277390;
    const auto& s=*storage_;const auto* b=block(s.world.parameters,"light2");
    if(!b||!textField(*b,"type","Directional")||!textField(*b,"colorrgb","206,206,185")||
       !textField(*b,"haslensflare","T")||field(*b,"haslightflare")||field(*b,"hasflasher")||
       field(*b,"range")||field(*b,"attenuation")||field(*b,"initialzone"))return false;
    namespace pose=assets::original_light_pose_defaults_candidate;
    pose::FreshPose native;pose::Diagnostic diagnostic;
    if(!pose::readFreshPose(*b,10u,native,diagnostic)||native.initialZone!=1u||
       !native.lookWasNormalized||native.effectiveLook78!=s.sunLook)return false;
    RaDirectionalLightInputs next;next.positionA0=native.positionA0;next.lookB0=native.lookB0;
    next.look78=s.sunLook;next.attenuation8C={{.03f,.002f,.001f}};
    //Explicit bounded native binary32 adaptation shared with existing Garage
    //Init. Source147280/28C/294/298/29C sequence; no original FCSR/VU parity.
    const auto& a=next.attenuation8C;
    const float square=mul(a[1],a[1]);const float fourC=mul(4.f,a[2]);
    const float aMinusTwo=sub(a[0],2.f);const float discriminant=sub(square,mul(fourC,aMinusTwo));
    if(!std::isfinite(discriminant)||discriminant<=0.f)return false;
    volatile float root=std::sqrt(discriminant);
    volatile float radius=add(-a[1],root)/mul(2.f,a[2]);
    if(!std::isfinite(radius)||radius<=0.f)return false;
    next.range84=radius;out=next;pc=0x147340;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::nativeRaFirstLocalInputs(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,
    RaDirectionalLightInputs& out,std::uint32_t& pc)const noexcept {
    return nativeRaLocalInputs(owner,source,rows,2u,out,pc);
}
bool Owner::nativeRaLocalInputs(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::size_t authoredIndex,
    RaDirectionalLightInputs& out,std::uint32_t& pc)const noexcept {
    if(authoredIndex<2u||authoredIndex>16u)return false;
    const auto zone=authoredIndex<7u?1u:7u;
    RaAuthoredLightIdentity identity;
    if(std::fegetround()!=FE_TONEAREST||!nativeRaLightIdentity(owner,source,rows,authoredIndex,identity,pc)||
       identity.type00!=1u||identity.zone20!=zone||!identity.localPrimitive)return false;
    const auto* b=identity.retainedSource;
    if(!textField(*b,"colorrgb","255,255,255")||!textField(*b,"emitslight","1")||
       !textField(*b,"haslightflare","1")||field(*b,"range")||field(*b,"attenuation")||
       field(*b,"lookvector")||field(*b,"targetposition")||field(*b,"hasflasher")||
       !textField(*b,"lightflarecolorrgb","255,255,255")||
       !textField(*b,"lightflaresize","30.000000")||!textField(*b,"lightflaretexturemapname","flare1")||
       field(*b,"lightflareturning"))return false;
    namespace pose=assets::original_light_pose_defaults_candidate;
    pose::FreshPose native;pose::Diagnostic diagnostic;
    if(!pose::readFreshPose(*b,10u,native,diagnostic)||native.initialZone!=zone+1u||native.lookWasNormalized)return false;
    const auto* actual=static_cast<const Storage::AuthoredLocalLight*>(identity.localPrimitive);
    for(std::size_t i=0;i<3;++i)if(actual->position[i]!=native.positionA0[i]||
       actual->look[i]!=native.effectiveLook78[i]||actual->rgb[i]!=127.5f)return false;
    RaDirectionalLightInputs next;next.positionA0=native.positionA0;next.lookB0=native.lookB0;
    next.look78=native.effectiveLook78;next.attenuation8C=actual->attenuation;
    if(next.attenuation8C!=light::Vec3{{.03f,.002f,.001f}})return false;
    const auto& a=next.attenuation8C;const float square=mul(a[1],a[1]);
    const float discriminant=sub(square,mul(mul(4.f,a[2]),sub(a[0],2.f)));
    if(!std::isfinite(discriminant)||discriminant<=0.f)return false;
    volatile float root=std::sqrt(discriminant);
    volatile float radius=add(-a[1],root)/mul(2.f,a[2]);
    if(!std::isfinite(radius)||radius<=0.f)return false;
    next.range84=radius;out=next;pc=0x147340;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::raFlareDomainIdle(const ::ra_pod_scene50LA::Owner* owner,
    const OriginalWreckComponent50LA& entity)const noexcept {
    if(!owner||!storage_||invalidated_||storage_->scene!=Scene::ra||storage_->sceneOwner!=owner||
       !storage_->staticOwner||!storage_->staticApplication||!storage_->staticRegistrationComplete||
       storage_->staticBusy()||storage_->staticAccess||storage_->activeLease||storage_->activeCamera||
       storage_->activeReset||storage_->resetPending()||storage_->zoneProjectionLease||
       !podFlareIdentityCurrent(*owner,&entity))return false;
    const auto& s=*storage_;const auto& life=s.visibility.lifecycle;
    if(s.raFlareCount>15u)return false;
    const auto first=s.racerCount+2u+s.staticModels.size();
    for(std::size_t i=0;i<s.raFlareCount;++i){const auto& f=s.raFlareAt(i);
        if(f.world!=owner||!f.entity||!f.type32||!f.flags33||f.object!=first+i||
           !podFlareIdentityCurrent(*owner,f.entity)||
           podFlareAuthoredIndex(*owner,f.entity)!=std::optional<std::size_t>(i+2u)||
           podFlareZoneIndex(*owner,f.entity)!=std::optional<std::size_t>(i<5u?1u:7u))return false;
    }
    return !s.registrations.useTc&&!s.registrations.ra.empty()&&
        life.storage.registrations==s.registrations.data()&&
        life.storage.registrationCapacity==s.registrations.size()&&
        life.objectCount==s.identityCount()&&s.registrations.size()==s.identityCount()&&
        life.storage.registries==s.registries.data()&&life.storage.views.methods==s.methods.data()&&
        life.storage.views.zones==s.zones.data()&&s.world.zones.size()==10u&&life.zoneCount==10u&&
        s.registries.size()>=20u&&s.methods.size()>=20u&&s.selectedMethods.size()>=20u;
}
bool Owner::bindRaFlareIdentity(const ::ra_pod_scene50LA::Owner* owner,
    OriginalWreckComponent50LA& entity,const std::uint8_t& type32,const std::uint8_t& flags33,
    std::uint32_t& identity,std::uint32_t& pc)noexcept {
    pc=0x14bad0;
    if(!raFlareDomainIdle(owner,entity)||entity.kind!=OriginalWreckComponentKind50LA::lightFlare14AA10||
       type32==0xffu||(flags33&7u)!=3u)return false;
    auto& s=*storage_;
    if(const auto* f=s.findRaFlare(&entity)){
        if(f->world!=owner||f->type32!=&type32||f->flags33!=&flags33||(identity&&identity!=f->object))return false;
        identity=f->object;return true;
    }
    const auto zone=podFlareZoneIndex(*owner,&entity);
    if(identity||s.raFlareCount>=15u||!zone||*zone!=(s.raFlareCount<5u?1u:7u)||
       podFlareAuthoredIndex(*owner,&entity)!=std::optional<std::size_t>(2u+s.raFlareCount)||
       s.identityCount()>=(std::numeric_limits<std::uint32_t>::max()>>2u)||
       s.selectedMethods[*zone*2u].type2D!=type32)return false;
    auto& f=s.raFlareAt(s.raFlareCount);
    if(f.world||f.entity||f.type32||f.flags33||f.object)return false;
    const auto count=s.identityCount();
    try {
        //Same canonical vector, no membership replay. Strong failure guarantee
        //for trivial registration elements; all existing values/IDs survive.
        s.registrations.ra.resize(count+1u);
    }catch(...){return false;}
    //No allocation, callback or possible failure after pointer publication.
    auto& life=s.visibility.lifecycle;
    life.storage.registrations=s.registrations.data();
    life.storage.registrationCapacity=s.registrations.size();
    f={owner,&entity,&type32,&flags33,static_cast<std::uint32_t>(count+1u)};
    ++s.raFlareCount;life.objectCount=count+1u;identity=f.object;return true;
}
bool Owner::raFlareIdentityCurrent(const ::ra_pod_scene50LA::Owner* owner,
    const OriginalWreckComponent50LA& entity,std::uint32_t identity,
    const OriginalNativeListMethodCustody50LA* method,std::uint32_t slot)const noexcept {
    if(!raFlareDomainIdle(owner,entity))return false;
    const auto& s=*storage_;const auto* f=s.findRaFlare(identity);
    const auto zone=podFlareZoneIndex(*owner,&entity);
    if(!f||f->world!=owner||f->entity!=&entity||!f->type32||!f->flags33||*f->type32==0xffu||
       (*f->flags33&7u)!=3u||!zone||(*zone!=1u&&*zone!=7u)||!identity||identity>s.identityCount())return false;
    const auto ordinal=*zone*2u;const auto& registration=s.registrations[identity-1u];
    if(!method&&!slot)return !registration.owner&&!registration.slot&&!registration.encodedObject;
    if(!method||!slot||method!=&s.selectedMethods[ordinal].sourceList||method->ordinal_!=ordinal||
       !raVisibilityListCustodyCurrent(owner,method)||!method->nodeReference_||
       !method->arrayReference_||method->reference00_!=2u||method->usage10_!=1u||
       method->registry_!=&s.registries[ordinal]||s.zones[*zone].firstMethod!=ordinal+1u||
       registration.owner!=&s.registries[ordinal]||registration.slot!=slot||
       registration.encodedObject!=(identity<<2u))return false;
    const auto& registry=*registration.owner;
    if(!registry.perBlock||registry.perBlock!=1024u||registry.blockCount!=1u||registry.lastUsed>1024u||
       slot>registry.lastUsed||registry.words[slot-1u]!=(identity<<2u))return false;
    const auto& view=s.methods[ordinal];
    return view.method==CollectionTree50BX::Method::SearchList&&view.searchList.blocks&&
        view.searchList.blockCapacity>=2u&&view.searchList.blocks[0].words==registry.words&&
        view.searchList.blocks[0].capacity==1024u&&view.searchList.blockCount==1&&
        view.searchList.perBlock30==1024&&view.searchList.lastUsed38==static_cast<std::int32_t>(registry.lastUsed);
}
bool Owner::registerRaFlarePrimary(const ::ra_pod_scene50LA::Owner* owner,
    OriginalNativeListMethodCustody50LA* method,const OriginalWreckComponent50LA& entity,
    std::uint32_t identity,std::uint32_t& slot,std::uint32_t& pc)noexcept {
    pc=0x27e3b0;
    if(!raFlareDomainIdle(owner,entity)||!method)return false;
    const auto zone=podFlareZoneIndex(*owner,&entity);
    if(!zone||(*zone!=1u&&*zone!=7u))return false;
    const auto ordinal=*zone*2u;
    if(method!=&storage_->selectedMethods[ordinal].sourceList||
       !raVisibilityListCustodyCurrent(owner,method)||method->ordinal_!=ordinal||!method->arrayReference_||
       !method->nodeReference_||method->reference00_!=2u||method->usage10_!=1u)return false;
    auto& s=*storage_;const auto* f=s.findRaFlare(identity);
    if(!f||f->world!=owner||f->entity!=&entity||!identity||identity>s.identityCount())return false;
    const auto& record=s.registrations[identity-1u];
    if(record.owner){
        if((slot&&slot!=record.slot)||!raFlareIdentityCurrent(owner,entity,identity,method,record.slot))return false;
        slot=record.slot;pc=0x27e5a8;return true;
    }
    if(slot||!raFlareIdentityCurrent(owner,entity,identity,nullptr,0u))return false;
    ZoneRegistrationScratch50CJ scratch;
    //Actual authored node.first, same List/slot/free-chain/selected-array state.
    const auto result=updateZoneRegistration50CL(s.visibility.lifecycle,identity,
        static_cast<std::uint32_t>(*zone+1u),ZoneMethod50CL::First,scratch);
    if(result!=SearchStorageStatus50BS::Complete)return false;
    slot=s.registrations[identity-1u].slot;pc=0x27e5a8;return true;
}
bool Owner::releaseRaFlarePrimary(const ::ra_pod_scene50LA::Owner* owner,
    const OriginalWreckComponent50LA& entity,std::uint32_t identity,std::uint32_t& pc)noexcept {
    pc=0x281524;
    if(!raFlareDomainIdle(owner,entity))return false;
    auto& s=*storage_;const auto* f=s.findRaFlare(identity);
    const auto zone=podFlareZoneIndex(*owner,&entity);
    if(!f||f->world!=owner||f->entity!=&entity||!identity||identity>s.identityCount()||
       !zone||(*zone!=1u&&*zone!=7u))return false;
    const auto& record=s.registrations[identity-1u];
    if(!record.owner)return raFlareIdentityCurrent(owner,entity,identity,nullptr,0u);
    if(!raFlareIdentityCurrent(owner,entity,identity,&s.selectedMethods[*zone*2u].sourceList,record.slot))return false;
    ZoneRegistrationScratch50CJ scratch;
    if(removeZoneRegistration50CL(s.visibility.lifecycle,identity,scratch)!=SearchStorageStatus50BS::Complete)return false;
    pc=0x27e5b0;return true;
}
bool Owner::releaseRaFlareIdentity(const ::ra_pod_scene50LA::Owner* owner,
    const OriginalWreckComponent50LA& entity,std::uint32_t identity,std::uint32_t& pc)noexcept {
    pc=0x14b9b0;
    // Refusal diagnostic (logs only): names the gate and the identity order.
    const auto refuse=[&](const char* gate)noexcept{
        const auto& st=*storage_;
        std::fprintf(stderr,"ra_flare_identity_release_refused gate=%s identity=%u identity_count=%zu flares=%zu racers=%zu static_models=%zu\n",
            gate,unsigned(identity),st.identityCount(),st.raFlareCount,std::size_t(st.racerCount),st.staticModels.size());
        return false;};
    if(!raFlareIdentityCurrent(owner,entity,identity,nullptr,0u))return refuse("identity_current");
    auto& s=*storage_;
    //Only the actual final identity may disappear: no older IDs are renumbered.
    if(!s.raFlareCount||identity!=s.identityCount()||s.raFlareAt(s.raFlareCount-1u).entity!=&entity)return refuse("not_last");
    //This cohort admits only primary visibility membership. Reject a remaining
    //live secondary token rather than erasing/replaying any existing registry.
    for(const auto& registry:s.registries){
        // A registry never constructed (the storage array is sized for the
        // largest zone count; RA uses 2 x 10) holds no token: nothing to scan.
        if(!registry.perBlock&&!registry.blockCount&&!registry.lastUsed&&!registry.freeHead)continue;
        if(registry.perBlock!=1024u||registry.blockCount>1u||registry.lastUsed>1024u){
            std::fprintf(stderr,"ra_flare_identity_registry per_block=%u blocks=%u last_used=%u\n",
                registry.perBlock,registry.blockCount,registry.lastUsed);
            return refuse("registry_shape");
        }
        const auto used=registry.blockCount?registry.lastUsed:0u;
        for(std::uint32_t i=0;i<used;++i)if(!(registry.words[i]&2u)&&registry.words[i]==(identity<<2u))return refuse("live_token");
    }
    s.registrations.ra.pop_back();s.raFlareAt(--s.raFlareCount)={};
    auto& life=s.visibility.lifecycle;
    life.storage.registrations=s.registrations.data();life.storage.registrationCapacity=s.registrations.size();
    life.objectCount=s.identityCount();return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::nativeRaRemainingInputs(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::size_t authoredIndex,
    RaRemainingLightInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x277390;
    if(authoredIndex<17u||authoredIndex>=185u||std::fegetround()!=FE_TONEAREST)return false;
    RaAuthoredLightIdentity identity;
    if(!nativeRaLightIdentity(owner,source,rows,authoredIndex,identity,pc)||!identity.retainedSource)return false;
    const auto& b=*identity.retainedSource;
    //Complete selected-row authentication. These exact allowed field sets
    //prove every omitted resource/flare/flasher/vector branch, rather than
    //treating a failed first-field lookup as whole-document absence.
    for(std::size_t i=0;i<b.fields.size();++i)for(std::size_t j=0;j<i;++j)
        if(b.fields[i].name==b.fields[j].name)return false;
    const bool ambient=authoredIndex>=172u&&authoredIndex<=174u;
    for(const auto& f:b.fields){const auto& k=f.name;
        const bool common=k=="type"||k=="colorrgb"||k=="initialzone";
        const bool spot=k=="position"||k=="lookvector"||k=="attenuation"||
            k=="innerconeangle"||k=="outerconeangle";
        if(!common&&(ambient||!spot))return false;
    }
    RaRemainingLightInputs next{};next.type=identity.type00;next.zone=identity.zone20;
    if(ambient){
        const std::uint32_t expectedZone=authoredIndex==172u?0u:authoredIndex==173u?1u:7u;
        if(identity.type00!=4u||identity.localPrimitive||identity.zone20!=expectedZone||
           b.fields.size()!=3u||!textField(b,"type","Ambient")||!textField(b,"colorrgb","25,25,25"))return false;
        //146C40's Ambient branch: four halved color lanes in5C, zeros3C/4C.
        //No pose vectors are read and no local observer/flare is requested.
        //The unused pose value is NOT authority to overwrite constructor state.
        next.ambient={{12.5f,12.5f,12.5f,127.5f}};
        out=next;pc=0x146e30;return true;
    }
    if(identity.type00!=3u||!identity.localPrimitive||!textField(b,"type","Spotlight")||
       (b.fields.size()!=7u&&b.fields.size()!=8u)||!field(b,"position")||!field(b,"lookvector")||
       !textField(b,"attenuation","1.0,0.000001,0.000002")||
       !field(b,"innerconeangle")||!field(b,"outerconeangle"))return false;
    namespace pose=assets::original_light_pose_defaults_candidate;
    pose::FreshPose native;pose::Diagnostic diagnostic;
    if(!pose::readFreshPose(b,10u,native,diagnostic)||native.initialZone!=identity.zone20+1u)return false;
    light::Vec3 color{},attenuation{};
    if(!triple(b,"colorrgb",color)||!triple(b,"attenuation",attenuation)||
       attenuation!=light::Vec3{{1.f,.000001f,.000002f}})return false;
    const auto& actual=*static_cast<const Storage::AuthoredLocalLight*>(identity.localPrimitive);
    if(!actual.spotlight||!actual.emitsLight||!actual.placeAtInfinity||
       actual.sourceOrdinal!=authoredIndex+1u||actual.effectiveSourceOrdinal!=identity.effectiveOrdinal||
       actual.zone!=native.initialZone||actual.attenuation!=attenuation)return false;
    for(std::size_t i=0;i<3u;++i){
        //Signed source RGB, including -80 -> -40, is not clamped or made unsigned.
        const auto half=mul(color[i],.5f);
        if(!std::isfinite(half)||actual.rgb[i]!=half||actual.position[i]!=native.positionA0[i]||
           actual.look[i]!=native.effectiveLook78[i])return false;
        next.diffuse[i]=next.specular[i]=half;
    }
    next.diffuse[3]=next.specular[3]=127.5f; //26FC10 default alpha255, then146F60 /2.
    next.pose.positionA0=native.positionA0;next.pose.lookB0=native.lookB0;
    next.pose.look78=native.effectiveLook78;next.pose.attenuation8C=attenuation;
    next.lookRevision=native.lookWasNormalized; //Actual unequal-to-fresh-B0 branch147194..230.
    //Explicit bounded native arithmetic, matching the existing pose/range
    //adaptation. No inferred original FCSR/EE sqrt or VU normalization parity.
    const float square=mul(attenuation[1],attenuation[1]);
    const float discriminant=sub(square,mul(mul(4.f,attenuation[2]),sub(attenuation[0],2.f)));
    if(!std::isfinite(discriminant)||discriminant<=0.f)return false;
    volatile float root=std::sqrt(discriminant);
    volatile float radius=add(-attenuation[1],root)/mul(2.f,attenuation[2]);
    if(!std::isfinite(radius)||radius<=0.f)return false;
    next.pose.range84=radius;
    //Same required-field parser used by this owner's canonical light loader.
    //No default or unrelated track's scalar policy is admitted here.
    const auto scalar=[](const assets::ParameterBlock48F& block,const char* key,float& value) noexcept {
        const auto* f=field(block,key);
        return f&&assets::parseTrackNumber50AD(f->value.data(),f->value.size(),value);
    };
    float inner=0,outer=0;
    if(!scalar(b,"innerconeangle",inner)||!scalar(b,"outerconeangle",outer)||
       !std::isfinite(inner)||!std::isfinite(outer)||inner<0.f||inner>180.f||outer<0.f||outer>180.f)return false;
    const std::uint32_t conversionBits=0x3c8efa34u;float conversion;
    std::memcpy(&conversion,&conversionBits,4);
    const float radiansInner=mul(inner,conversion),radiansOuter=mul(outer,conversion);
    //Actual146F30 stores radians, not the canonical backend's later half-angle
    //cosines. No inner<outer rule: authenticated Light129 is60/0.
    std::memcpy(&next.cone98,&radiansInner,4);std::memcpy(&next.cone9C,&radiansOuter,4);
    out=next;pc=0x146f50;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const std::string* Owner::nativeRaParticleSelector1B4DC0(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc))return nullptr;
    // Actual existing EnvironmentInfo3A4 owns this immutable deep copy. This
    // native41C alias does not claim earlier276C00 ran or allocate a new Env.
    const auto& doc=(*podEnvironment3A4(*owner))->world()->parameters;
    const assets::ParameterBlock48F* environment=nullptr;
    for(const auto& candidate:doc.blocks)if(candidate.name=="environment"){
        if(environment)return nullptr;environment=&candidate;
    }
    if(!environment||environment->offset!=37657u)return nullptr;
    const assets::ParameterField48F* selector=nullptr;
    for(const auto& f:environment->fields)if(f.name=="particletexturemapname"){
        if(selector)return nullptr;selector=&f;
    }
    pc=0x1b4de0;
    if(!selector||selector->value_offset!=38488u||selector->value!="demoprt.tga")return nullptr;
    return &selector->value;
}
const std::string* Owner::nativeRaSkySelector274AE0(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc))return nullptr;
    const auto& actual=**podEnvironment3A4(*owner);pc=0x274b0c;
    if(!actual.prepared()||actual.cubeCount()!=1u||actual.zoneCount()!=10u)return nullptr;
    const auto* name=actual.cubeFile(0);if(!name||*name!="ra.cub")return nullptr;
    for(std::size_t i=0;i<10u;++i){const auto* index=actual.zoneCubeIndex(static_cast<std::uint32_t>(i));
        const auto expected=(i==0u||i==1u||i==7u)?-1:0;
        if(!index||*index!=expected)return nullptr;
    }
    // SAME immutable EnvironmentInfo3A4, not a second reader or fabricated
    //276C00 execution. Source sky-owned objects remain a separate obligation.
    return name;
}

}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const char* Owner::nativeRaWaterSelector274570(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc))return nullptr;
    auto& actual=**podEnvironment3A4(*owner);
    if(!actual.prepared()||!actual.world())return nullptr;
    const auto* field=actual.nativeWaterFile110();pc=0x2745b0;
    if(!field||!field->defaulted||std::strcmp(field->value.data(),"none"))return nullptr;
    return field->value.data(); // SAME actual EnvironmentInfo+110 metadata.
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const char* Owner::nativeRaEcosystemSelector1AA410(const ::ra_pod_scene50LA::Owner* owner,
    const assets::World48F& source,const CollectionZone50BX* rows,std::uint32_t& pc)const noexcept {
    if(!nativeSceneRoles270900Current(owner,source,rows,pc))return nullptr;
    auto& actual=**podEnvironment3A4(*owner);
    if(!actual.prepared()||!actual.world())return nullptr;
    const auto* field=actual.nativeEcosystemFile318();pc=0x1aa410;
    if(!field||!field->defaulted||std::strcmp(field->value.data(),"none"))return nullptr;
    return field->value.data();
}
}
