#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "../../../src/assets/import_collision48g.h"
#include "../../../src/assets/track_collision_tags50an.h"
#include "../../../src/game/type2_resource49x.h"
#include "../../../src/game/original_native_collision_categories.h"
#include <cstring>
#include <limits>
#include <exception>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool collisionFail(D& d,S status,std::uint32_t pc)noexcept {d={};d.status=status;d.sourcePC=pc;return false;}
bool sameFile(assets::OriginalStandardModelFileId50LA a,assets::OriginalStandardModelFileId50LA b)noexcept {
    return a.resolver==b.resolver&&a.row==b.row;
}
}
OriginalUiGarageSceneCollision50KG::~OriginalUiGarageSceneCollision50KG()noexcept {
    if(attempted_&&!closed_)std::terminate();
}
bool OriginalUiGarageSceneCollision50KG::belongsTo(const OriginalUiGaragePodScene50KG& scene,
    const std::shared_ptr<Grant>& grant,
    const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>& assets)const noexcept {
    return attempted_&&!closed_&&scene_==&scene&&grant_&&grant_==grant&&
        scene.grant_==grant_&&scene.assets_==assets&&resources_&&resources_->domain_&&resources_==scene.configResources_&&
        visibility_==scene.visibility_&&provider_&&provider_->sealed()&&
        provider_->revision()==providerRevision_&&resolver_==resources_->domain_->resolver3E1A70();
}
bool OriginalUiGarageSceneCollision50KG::populate2743B0(OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(attempted_||closed_||!scene.currentPrefix()||!scene.sceneDispatch_||
       !scene.sceneDispatch_->sceneComplete||scene.sceneDispatch_->closed||scene.numZones34C_!=1||
       !scene.staticModels_||!scene.staticModels_->populationCurrent()||
       !scene.visibility_||!scene.visibility_->current()||!scene.configResources_||
       !scene.grant_||!scene.grant_->current())return collisionFail(d,S::phase,0x2743B0);
    auto& dispatch=*scene.sceneDispatch_;
    if(!dispatch.collisionLists3CC||!dispatch.collisionZone||
       dispatch.collisionLists3CC[0]!=dispatch.collisionZone.get()||
       !dispatch.collisionZone->live||!dispatch.collisionZone->complete||
       dispatch.collisionZone->retiring||dispatch.collisionZone->retired||
       dispatch.collisionZone->count30!=count||!dispatch.collisionZone->records34)
        return collisionFail(d,S::phase,0x2743F4);
    const auto& resources=scene.configResources_;
    if(!resources->application_||resources->application_!=scene.grant_->application()||
       !resources->domain_||!resources->domain_->prepared()||!resources->context_||
       !resources->context_->live()||resources->generation_!=scene.grant_->generation_)
        return collisionFail(d,S::phase,0x1B5590);
    const auto provider=resources->domain_->archiveProvider3E1A70();
    const auto resolver=resources->domain_->resolver3E1A70();
    if(!provider||!provider->sealed()||!resolver)return collisionFail(d,S::phase,0x132820);
    for(std::size_t i=0;i<count;++i){const auto& wrapper=dispatch.collisionZone->records[i];
        if(!wrapper||dispatch.collisionZone->records34[i]!=wrapper.get()||wrapper->type00!=1||
           wrapper->liveCollision04||!wrapper->temporary08)return collisionFail(d,S::phase,0x27BED4);
    }
    attempted_=true;scene_=&scene;grant_=scene.grant_;resources_=resources;
    provider_=provider;resolver_=resolver;providerRevision_=provider_->revision();
    visibility_=scene.visibility_;zone3C4_=&scene.staticModels_->list3C4_;
    boundary_=0x2743E4;scene.currentZone354_=0;
    if(!visibility_->beginCollisionPopulation2743B0(*this,d))return false;
    visibilityBound_=true;
    for(std::size_t i=0;i<count;++i){
        rows_[i].wrapper=dispatch.collisionZone->records34[i];
        rows_[i].wrapperIdentity=rows_[i].wrapper;
        rows_[i].descriptorIdentity=rows_[i].wrapper->temporary08.get();
        if(!populateRow1B5590(i,d))return false;
        ++completed_;
    }
    complete_=true;boundary_=0x274458;
    return current2743B0(scene)?true:collisionFail(d,S::phase,boundary_);
}
bool OriginalUiGarageSceneCollision50KG::populateRow1B5590(std::size_t ordinal,D& d)noexcept {
    if(ordinal>=count||!belongsTo(*scene_,grant_,scene_->assets_)||!zone3C4_||
       scene_->currentZone354_!=0)return collisionFail(d,S::phase,0x27BECC);
    auto& row=rows_[ordinal];auto* wrapper=row.wrapper;
    if(row.attempted||!wrapper||wrapper!=row.wrapperIdentity||wrapper->type00!=1||wrapper->liveCollision04||
       !wrapper->temporary08||wrapper->temporary08.get()!=row.descriptorIdentity)
        return collisionFail(d,S::phase,0x1B5590);
    const auto& descriptor=*wrapper->temporary08;
    // Authored Garage Standard-only branch. The real "none" early return
    // leaves the temp live, and is NOT a successful populated row.
    if(descriptor.stem00=="none")return collisionFail(d,S::source,0x1B55EC);
    row.attempted=true;
    try {
        row.originalStem=descriptor.stem00;row.lowerStem=row.originalStem;
        if(row.lowerStem.empty()||row.lowerStem.size()>50)return collisionFail(d,S::source,0x1B55F4);
        for(char& c:row.lowerStem){const auto b=static_cast<unsigned char>(c);
            if(b<32||b>126)return collisionFail(d,S::source,0x1B5644);
            if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');}
        row.query=std::make_unique<Query>();
    }catch(...){return collisionFail(d,S::allocation,0x1B5600);}
    auto& q=*row.query;
    auto step=grant_->constructGarageSceneCollision1B5590(scene_,this,ordinal,q.entity8);
    if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
    q.entityLive=true;boundary_=0x11B270;
    auto app=grant_->application();if(!app)return collisionFail(d,S::phase,boundary_);
    const auto type=app->types383408().collisionObject11B270();
    if(!type.okay())return collisionFail(d,S::source,type.sourcePC);
    q.type02=type.type;
    q.flags03=static_cast<std::uint8_t>(q.flags03&0xf8u);
    q.flagsF8=static_cast<std::uint8_t>((q.flagsF8&0xc8u)|6u); //11B190: bit3 preserved.
    q.copied40={{0,0,0,1}};q.copied50=q.copied40;q.previous70={{0,-10000,0,1}};
    q.shape.registrationE0=0;
    wrapper->liveCollision04=&q;boundary_=0x1B5618;
    q.flagsF8=static_cast<std::uint8_t>((q.flagsF8&~14u)|12u); //11B570(13).
    app->collisionGlobalValid3E0730_=0; //11B5F4, not a private cache reset.
    // This is the existing application's declared native category namespace,
    // shared with actual track/filter consumers; these are NOT guest addresses
    // or assertions that their source runtime bit numbers are4/16/32/64.
    assets::TrackCollisionMasks50AN masks{};
    const auto category=[&](const char* name,std::uint32_t& out)noexcept {
        bool found=false;
        for(const auto& binding:app->nativeCollisionCategoryBindings_){
            if(!binding.name||std::strcmp(binding.name,name))continue;
            if(found||!binding.bit)return false;
            out=binding.bit;found=true;
        }
        return found;
    };
    if(!category("STATICOBJECT",masks.staticObject)||!category("FLOOR",masks.floor)||
       !category("WATER",masks.water)||!category("TUBE",masks.tube))
        return collisionFail(d,S::source,0x1B5638);
    if(!assets::classifyTrackCollision50AN(row.lowerStem.data(),row.lowerStem.size(),masks,q.categoryA4))
        return collisionFail(d,S::source,0x1B566C);
    if(descriptor.material184>0)q.materialA9=static_cast<std::uint8_t>(descriptor.material184);
    q.materialA8=static_cast<std::uint8_t>(descriptor.field180);
    if(!load11B920(ordinal,d))return false;
    if(q.providerAbsent){
        // Exact11BA34 failed-open branch, not missing selected source input.
        // Query constructor/count/category work really occurred before release.
        if(!retireQuery11B4A0(ordinal,d))return false;
    }else {
        step=grant_->attachGarageSceneCollision1B577C(scene_,this,ordinal,q.entity8,*zone3C4_);
        if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
        std::array<float,16> placement{};
        if(!assets::placementCollision48I(descriptor.position110,descriptor.look120,descriptor.up130,placement))
            return collisionFail(d,S::source,0x1B5808);
        game::BodyMatrix48G matrix{};std::memcpy(matrix.v,placement.data(),sizeof matrix.v);
        // Both calls matter: second copies freshly written world80 topreviousC0.
        boundary_=0x1B587C;
        if(!game::transformPodShape49W(q.source,matrix,q.shape))return collisionFail(d,S::source,0x11F680);
        boundary_=0x1B5888;
        if(!game::transformPodShape49W(q.source,matrix,q.shape))return collisionFail(d,S::source,0x11F680);
        boundary_=0x1B58A8;
        if(!visibility_->attachCollision1B58A8(*this,ordinal,d))return false;
    }
    // Descriptor must remain the ORIGINAL live object through every consumed
    // field and registration; no immutable preparse replaces this destruction.
    if(wrapper->temporary08.get()!=row.descriptorIdentity)return collisionFail(d,S::phase,0x1B58B0);
    wrapper->temporary08.reset();boundary_=0x1B58BC;row.completed=true;return true;
}
bool OriginalUiGarageSceneCollision50KG::load11B920(std::size_t ordinal,D& d)noexcept {
    auto& row=rows_[ordinal];auto& q=*row.query;auto app=grant_->application();
    if(!app||q.loaded||q.providerAbsent||q.shape8C||!q.entityLive||q.entity8.reference00!=1)
        return collisionFail(d,S::phase,0x11B920);
    try {
        const auto& directory=app->collisionDirectory36B0F0_;
        // The bounded namespace has no extension in its directory. Do not
        // reinterpret1169D0 on an unknown dotted directory as basename logic.
        if(directory.find('.')!=std::string::npos||directory.size()>127)
            return collisionFail(d,S::source,0x11B9BC);
        row.requestedMember=directory;
        if(!row.requestedMember.empty()&&row.requestedMember.back()!='\\')row.requestedMember+='\\';
        row.requestedMember+=row.lowerStem;row.requestedMember+=".COL";
        if(row.lowerStem.size()+1>app->collisionName3E0730_.size())return collisionFail(d,S::source,0x11BA00);
        std::memcpy(app->collisionName3E0730_.data(),row.lowerStem.c_str(),row.lowerStem.size()+1);
        boundary_=0x11BA2C;
        const auto lookup=provider_->lookup132820(row.requestedMember,row.member);
        assets::OriginalStandardTextureDiagnostic50LA cache;
        assets::OriginalStandardModelFileView50LA file;
        if(lookup==Provider::Lookup::absent){
            if(resolver_->lookupCollision132820(row.requestedMember,file,cache)!=
               assets::OriginalStandardCollisionLookup50LA::absent)return collisionFail(d,S::source,0x132820);
            q.providerAbsent=true;boundary_=0x11BA50;return true;
        }
        if(lookup!=Provider::Lookup::present||!row.member.data||!row.member.member)
            return collisionFail(d,S::source,0x132820);
        std::vector<std::uint8_t> raw(row.member.data,row.member.data+row.member.size);
        assets::OriginalStandardModelFileId50LA id;
        if(!resolver_->bindModelMember(*row.member.member,assets::OriginalStandardModelFileKind50LA::colType0,raw,id,cache)||
           resolver_->lookupCollision132820(row.requestedMember,file,cache)!=assets::OriginalStandardCollisionLookup50LA::present||
           !sameFile(id,file.id)||!file.bytes||*file.bytes!=raw)
            return collisionFail(d,S::source,cache.sourcePC?cache.sourcePC:0x132820);
        q.fileB0=id;
        auto resource=file.collision125014;
        if(resource){
            if(!resource->live()||resource->resolver_!=resolver_||!sameFile(resource->file_,id)||
               resource->reference60_==std::numeric_limits<std::int32_t>::max())return collisionFail(d,S::phase,0x125500);
            q.shape8C=true;q.resource124=resource;++resource->reference60_;boundary_=0x1255A0;
        }else {
            assets::Collision48G decoded;assets::DecodeError48C why;
            std::vector<std::uint8_t> nativeCache;std::string error;
            // Actual positive source COL0 only. A compound/type2 source is a
            // reached unsupported branch, never a visual PSG or fake COL0.
            if(!assets::decodeCollision48G(raw,decoded,why)||decoded.type!=0||decoded.reverse_mode!=0||
               (decoded.quantized&&!decoded.vertex_normals.empty()))
                return collisionFail(d,S::source,0x124FC0);
            q.shape8C=true;boundary_=0x125890;
            q.loading=std::make_shared<Resource>();auto& r=*q.loading;
            r.resolver_=resolver_;r.file_=id;
            if(!assets::encodeCollisionAsset48G(decoded,nativeCache,error)||
               !assets::loadCollisionMesh48I(nativeCache,r.mesh_,error)||r.mesh_.nodes.empty())
                return collisionFail(d,S::source,0x125890);
            r.radii_.resize(r.mesh_.nodes.size());
            std::vector<std::uint32_t> topology(r.mesh_.nodes.size()+r.mesh_.faces.size());std::size_t depth=0;
            if(game::prepareStaticTargetRadii49X(r.mesh_,r.radii_.data(),r.radii_.size(),
                {topology.data(),topology.size()},depth)!=game::Type2ResourceStatus49X::Complete)
                return collisionFail(d,S::source,0x126200);
            r.vertexCount04_=static_cast<std::uint32_t>(decoded.vertices.size());r.packed18_=decoded.quantized;r.root1C_=0;
            r.constructed_=true;r.reference60_=1;q.resource124=std::move(q.loading);boundary_=0x1250D4;
            if(!resolver_->publishCollision1326A0(id,q.resource124,cache))
                return collisionFail(d,S::source,cache.sourcePC);
        }
        const auto& mesh=q.resource124->mesh();
        if(mesh.nodes.empty()||q.resource124->radii().size()!=mesh.nodes.size())return collisionFail(d,S::source,0x125890);
        std::memcpy(q.source.local40.v,mesh.local_matrix.data(),sizeof q.source.local40.v);
        std::memcpy(q.source.rootCenter.v,mesh.nodes[0].center.data(),sizeof q.source.rootCenter.v);
        std::memcpy(q.source.rootHalf.v,mesh.nodes[0].half.data(),sizeof q.source.rootHalf.v);
        q.loaded=true;boundary_=0x11BBA4;return true;
    }catch(...){return collisionFail(d,S::allocation,boundary_?boundary_:0x11B920);}
}
bool OriginalUiGarageSceneCollision50KG::releaseShape11B600(Query& q,D& d)noexcept {
    if(q.resource124){auto& resource=*q.resource124;
        if(!resource.live()||resource.resolver_!=resolver_)return collisionFail(d,S::phase,0x11B650);
        resolver_->clearCollision1327D0(&resource); //Even when another query holds positive ref60.
        if(!--resource.reference60_){resource.releaseArrays11AEE0();resource.retired_=true;}
        q.resource124.reset();
    }
    if(q.loading){q.loading->releaseArrays11AEE0();q.loading.reset();}
    q.shape8C=false;q.loaded=false;return true;
}
bool OriginalUiGarageSceneCollision50KG::retireQuery11B4A0(std::size_t ordinal,D& d)noexcept {
    d={};if(ordinal>=count||!rows_[ordinal].query)return collisionFail(d,S::phase,0x11B4A0);
    auto& row=rows_[ordinal];auto& q=*row.query;if(q.retired)return true;
    if((row.wrapper&&row.wrapper!=row.wrapperIdentity)||(rawListRetirementPage_&&row.wrapper))
        return collisionFail(d,S::phase,0x11B4A0);
    if(!q.entityLive||!grant_||!grant_->current()||q.entity8.firstChild18)
        return collisionFail(d,S::phase,0x142590);
    const C* parent=q.entity8.parent1C;
    if(parent&&parent!=zone3C4_)return collisionFail(d,S::phase,0x142590);
    if(!q.retiring){
        auto step=grant_->preflightGarageSceneCollisionRetirement(scene_,this,ordinal,q.entity8,parent);
        if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
        step=grant_->beginGarageSceneCollisionRetirement(scene_,this,ordinal,q.entity8,parent);
        if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
        q.retiring=true;
    }
    //F8.bit3 used a temporary observer: sourceE0 is NULL, so11B4D8 skips
    //280920 here. Only a reached native partial temporary needs unwinding.
    if(q.sharedObserver&&!visibility_->releaseCollisionTemporary120200(*this,ordinal,d))return false;
    if(!releaseShape11B600(q,d))return false;
    const auto step=grant_->finishGarageSceneCollisionRetirement(scene_,this,ordinal,q.entity8,parent);
    if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
    q.entityLive=false;q.retiring=false;q.retired=true;
    // The raw Page ordinal6 wrapper was already freed by27BB80 when this
    // is reached from StaticModels ordinal5. Never follow its tombstone.
    if(row.wrapper&&row.wrapper==row.wrapperIdentity&&row.wrapper->liveCollision04==&q)
        row.wrapper->liveCollision04=nullptr;
    boundary_=rawListRetirementPage_?0x11B558:0x1B574C;return true;
}
bool OriginalUiGarageSceneCollision50KG::current2743B0(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(!complete_||completed_!=count||rawListRetirementPage_||failedRawListPage_||
       !belongsTo(scene,grant_,scene.assets_)||scene.currentZone354_!=0||
       !scene.sceneDispatch_||!scene.sceneDispatch_->collisionZone||!zone3C4_)return false;
    const auto& list=*scene.sceneDispatch_->collisionZone;
    if(!list.live||!list.complete||list.retiring||list.retired||list.count30!=count||!list.records34)return false;
    for(std::size_t i=0;i<count;++i){const auto& row=rows_[i];const auto* q=row.query.get();
        if(!row.completed||!row.wrapper||row.wrapper!=row.wrapperIdentity||!q||row.wrapper!=list.records34[i]||
           row.wrapper->temporary08)return false;
        if(q->providerAbsent){
            Provider::MemberView absent;
            if(!q->retired||q->entityLive||row.wrapper->liveCollision04||q->resource124||q->shape8C||
               provider_->lookup132820(row.requestedMember,absent)!=Provider::Lookup::absent)return false;
        }else if(!q->loaded||!q->resource124||!q->resource124->live()||!q->entityLive||q->retired||
                 row.wrapper->liveCollision04!=q||q->entity8.reference00!=1||q->entity8.parent1C!=zone3C4_||
                 !visibility_->collisionMembershipCurrent(*this,i))return false;
    }
    return true;
}
bool OriginalUiGarageSceneCollision50KG::staticSuffixCurrent(const OriginalUiGarageStaticModels50KG& owner,
    const C* previous,const C* child)const noexcept {
    if(!scene_||rawListRetirementPage_||failedRawListPage_||scene_->staticModels_.get()!=&owner||zone3C4_!=&owner.list3C4_||
       !belongsTo(*scene_,grant_,scene_->assets_)||!complete_||completed_!=count)return false;
    for(const auto& row:rows_){const auto* q=row.query.get();
        if(!row.completed||!row.wrapper||row.wrapper!=row.wrapperIdentity||!q)return false;
        if(q->providerAbsent){if(!q->retired||q->entityLive||row.wrapper->liveCollision04)return false;continue;}
        const auto* entity=&q->entity8;
        if(q->retired||!q->entityLive||!q->loaded||row.wrapper->liveCollision04!=q||
           child!=entity||entity->parent1C!=zone3C4_||entity->previous10!=previous||
           entity->reference00!=1||entity->mask20||entity->descendants24||entity->firstChild18)return false;
        previous=entity;child=entity->next14;
    }
    return child==nullptr;
}
bool OriginalUiGarageSceneCollision50KG::rawListRetired27BB80()const noexcept {
    if(!rawListRetirementPage_||!scene_||!scene_->sceneDispatch_||
       !scene_->sceneDispatch_->collisionZone)return false;
    const auto& list=*scene_->sceneDispatch_->collisionZone;
    if(list.live||list.retiring||!list.retired||list.count30!=count||list.records34||
       list.entity.reference00||list.entity.firstChild18||list.entity.next14||
       !list.entity.parent1C||list.entity.mask20!=2u)return false;
    for(std::size_t i=0;i<count;++i)
        if(list.records[i]||rows_[i].wrapper||!rows_[i].wrapperIdentity)return false;
    return true;
}
bool OriginalUiGarageSceneCollision50KG::retirePageRawList27BB80(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(failedRawListPage_||!scene_||!grant_||!grant_->current()||!belongsTo(*scene_,grant_,scene_->assets_)||
       scene_->sceneCollision_.get()!=this||!complete_||completed_!=count||!visibility_||!visibilityBound_||
       !scene_->sceneDispatch_||!scene_->sceneDispatch_->collisionZone||
       !scene_->sceneDispatch_->collisionLists3CC||scene_->numZones34C_!=1||
       (rawListRetirementPage_&&rawListRetirementPage_!=&page))
        return collisionFail(d,S::phase,0x27BB80);
    auto& list=*scene_->sceneDispatch_->collisionZone;
    if(scene_->sceneDispatch_->collisionLists3CC[0]!=&list||list.count30!=count)
        return collisionFail(d,S::phase,0x27BB80);
    if(list.retired)return rawListRetired27BB80()?true:collisionFail(d,S::phase,0x1413E0);
    if(!list.live||!list.complete||pageQueriesRetired_)return collisionFail(d,S::phase,0x27BB80);
    if(!list.retiring){
        if(rawListRetirementPage_||!list.records34||
           !grant_->garagePageCollisionListRetirementCurrent(page,*scene_,list.entity))
            return collisionFail(d,S::phase,0x142590);
        // Only the fully populated authentic List is admitted. The absent
        // provider branch already destroyed its query during1B5590; positive
        // queries remain live under StaticModels3C4, not this raw List3CC.
        // In particular no liveCollision04 read/delete belongs to27BB80.
        for(std::size_t i=0;i<count;++i){const auto& row=rows_[i];const auto* q=row.query.get();
            if(!row.attempted||!row.completed||!row.wrapperIdentity||!row.descriptorIdentity||
               !row.wrapper||row.wrapper!=row.wrapperIdentity||list.records[i].get()!=row.wrapper||
               list.records34[i]!=row.wrapper||row.wrapper->type00!=1||row.wrapper->temporary08||
               !q||q->retiring||q->sharedObserver)
                return collisionFail(d,S::phase,0x27BBCC);
            if(q->providerAbsent){
                if(!q->retired||q->entityLive||q->entity8.reference00||q->entity8.parent1C||
                   q->resource124||q->loading||q->shape8C||q->loaded||q->membership.attached)
                    return collisionFail(d,S::phase,0x11B4A0);
            }else if(q->retired||!q->entityLive||q->entity8.reference00!=1||
                     q->entity8.parent1C!=zone3C4_||!q->loaded||!q->shape8C||
                     !q->resource124||!q->resource124->live()||q->loading||
                     !visibility_->collisionMembershipCurrent(*this,i))
                return collisionFail(d,S::phase,0x1B58A8);
        }
        const auto step=grant_->beginGaragePageCollisionListRetirement142590(page,*scene_,list.entity);
        if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
        rawListRetirementPage_=&page;list.retiring=true;boundary_=0x27BB80;
    }
    if(rawListRetirementPage_!=&page||list.entity.reference00||list.entity.firstChild18||
       list.entity.next14||!list.entity.parent1C||list.entity.mask20!=2u||
       !grant_->garagePageCollisionListBaseRetirementCurrent(page,*scene_,list.entity))
        return collisionFail(d,S::phase,0x27BB80);
    if(!releaseRawWrappers27BBCC(d))return false;
    const auto step=grant_->finishGaragePageCollisionListRetirement1413E0(page,*scene_,list.entity);
    if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
    // No count30=0, link clearing, collisionLists3CC free, query deletion,
    // SearchList release, or whole Scene/Collision-owner completion here.
    list.retiring=false;list.retired=true;list.live=false;boundary_=0x27BC58;return true;
}
bool OriginalUiGarageSceneCollision50KG::releaseRawWrappers27BBCC(D& d)noexcept {
    if(!scene_||!scene_->sceneDispatch_||!scene_->sceneDispatch_->collisionZone)
        return collisionFail(d,S::phase,0x27BBCC);
    auto& list=*scene_->sceneDispatch_->collisionZone;
    // Both callers already entered their distinct authentic last-reference
    // authority. This shared leaf owns payload only, never Entity/count proof.
    if(!list.live||!list.retiring||list.retired||list.count30!=count||
       list.entity.reference00||list.entity.firstChild18)
        return collisionFail(d,S::phase,0x27BBCC);
    if(list.records34){
        // Preflight all native borrows before the first source free, so a
        // rejected identity cannot leave an unrecorded half-destroyed array.
        for(std::size_t i=0;i<count;++i){const auto& row=rows_[i];
            if(!list.records[i]||row.wrapper!=list.records[i].get()||
               row.wrapper!=row.wrapperIdentity||list.records34[i]!=row.wrapper)
                return collisionFail(d,S::phase,0x27BBCC);
        }
        for(std::size_t i=0;i<count;++i){auto& row=rows_[i];
            list.records[i]->temporary08.reset();boundary_=0x27BC08;
            row.wrapper=nullptr; // Native borrow ends with the actual wrapper.
            list.records[i].reset();boundary_=0x27BC14;
            list.records34[i]=nullptr;boundary_=0x27BC28;
        }
        list.records34.reset();boundary_=0x27BC50;
    }
    for(std::size_t i=0;i<count;++i)
        if(list.records[i]||rows_[i].wrapper)return collisionFail(d,S::phase,0x27BC58);
    return true;
}
bool OriginalUiGarageSceneCollision50KG::preflightFailedRawList27BB80(
    const OriginalUiGaragePageOwner50KG& page,D& d)const noexcept {
    d={};
    if(rawListRetirementPage_||failedRawListPage_||pageQueriesRetired_||!scene_||!grant_||
       !grant_->current()||!page.partialSceneAttachmentFailureCurrent()||
       page.scene4C0_!=scene_||page.grant_!=grant_||
       !scene_->sourceLoadReturned1FB59C()||!current2743B0(*scene_)||
       !scene_->sceneDispatch_||!scene_->sceneDispatch_->collisionZone||
       !scene_->sceneDispatch_->collisionLists3CC||scene_->numZones34C_!=1)
        return collisionFail(d,S::phase,0x27BB80);
    const auto& list=*scene_->sceneDispatch_->collisionZone;
    if(scene_->sceneDispatch_->collisionLists3CC[0]!=&list||!list.records34||
       list.count30!=count||list.entity.parent1C||list.entity.previous10||
       list.entity.next14||list.entity.mask20||list.entity.reference00!=1||
       list.entity.firstChild18||list.entity.descendants24)
        return collisionFail(d,S::phase,0x142590);
    // Same actual inputs as27BB80: wrappers own temporary08, not query04.
    // Prove every borrow before the first reference or allocation is released.
    for(std::size_t i=0;i<count;++i){const auto& row=rows_[i];const auto* q=row.query.get();
        if(!row.attempted||!row.completed||!row.wrapperIdentity||!row.descriptorIdentity||
           !row.wrapper||row.wrapper!=row.wrapperIdentity||list.records[i].get()!=row.wrapper||
           list.records34[i]!=row.wrapper||row.wrapper->type00!=1||row.wrapper->temporary08||
           !q||q->retiring||q->sharedObserver)
            return collisionFail(d,S::phase,0x27BBCC);
        if(q->providerAbsent){
            if(!q->retired||q->entityLive||q->entity8.reference00||q->entity8.parent1C||
               q->resource124||q->loading||q->shape8C||q->loaded||q->membership.attached)
                return collisionFail(d,S::phase,0x11B4A0);
        }else if(q->retired||!q->entityLive||q->entity8.reference00!=1||
                 q->entity8.parent1C!=zone3C4_||!q->loaded||!q->shape8C||
                 !q->resource124||!q->resource124->live()||q->loading||
                 !visibility_->collisionMembershipCurrent(*this,i))
            return collisionFail(d,S::phase,0x1B58A8);
    }
    const auto step=grant_->preflightGarageFailedCollisionList142590(page,*scene_,list.entity);
    if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
    return true;
}
bool OriginalUiGarageSceneCollision50KG::failedRawListRetired27BB80(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(failedRawListPage_!=&page||rawListRetirementPage_||!scene_||!grant_||!grant_->current()||
       !page.partialSceneAttachmentFailureCurrent()||page.scene4C0_!=scene_||page.grant_!=grant_||
       !scene_->sceneDispatch_||!scene_->sceneDispatch_->collisionZone)return false;
    const auto& list=*scene_->sceneDispatch_->collisionZone;
    if(scene_->sceneDispatch_->collisionLists3CC){
        if(scene_->sceneDispatch_->collisionLists3CC[0]!=&list)return false;
    }else if(scene_->failedPageRetirementOwner_!=&page||
             scene_->retirementStage_<OriginalUiGaragePodScene50KG::RetirementStage::collisionPointersReleased)
        return false; // Only the actual later26FF44 array free, not absent initial storage.
    if(list.live||list.retiring||!list.retired||
       list.count30!=count||list.records34||list.entity.reference00||list.entity.firstChild18||
       list.entity.parent1C||list.entity.previous10||list.entity.next14||list.entity.mask20)return false;
    for(std::size_t i=0;i<count;++i)
        if(list.records[i]||rows_[i].wrapper||!rows_[i].wrapperIdentity)return false;
    return true;
}
bool OriginalUiGarageSceneCollision50KG::retireFailedRawList27BB80(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};
    if(rawListRetirementPage_||!scene_||!grant_||!grant_->current()||
       !page.partialSceneAttachmentFailureCurrent()||page.scene4C0_!=scene_||page.grant_!=grant_||
       (failedRawListPage_&&failedRawListPage_!=&page)||
       !scene_->sceneDispatch_||!scene_->sceneDispatch_->collisionZone)
        return collisionFail(d,S::phase,0x27BB80);
    auto& list=*scene_->sceneDispatch_->collisionZone;
    if(list.retired)return failedRawListRetired27BB80(page)?true:collisionFail(d,S::phase,0x1413E0);
    if(!failedRawListPage_){
        if(!preflightFailedRawList27BB80(page,d))return false;
        const auto step=grant_->beginGarageFailedCollisionList142590(page,*scene_,list.entity);
        if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
        failedRawListPage_=&page;list.retiring=true;boundary_=0x27BB80;
    }
    if(!list.live||!list.retiring||list.entity.reference00||list.entity.firstChild18||
       list.entity.parent1C||list.entity.previous10||list.entity.next14||list.entity.mask20||
       !grant_->garageFailedCollisionListBaseCurrent(page,*scene_,list.entity))
        return collisionFail(d,S::phase,0x27BB80);
    if(!releaseRawWrappers27BBCC(d))return false;
    const auto step=grant_->finishGarageFailedCollisionList1413E0(page,*scene_,list.entity);
    if(!step.okay())return collisionFail(d,S::phase,step.sourcePC);
    // Keep the actual3CC pointer array, count30, query objects, static children
    // and Search rows for their own later callbacks. Slot6 was never attached.
    list.retiring=false;list.retired=true;list.live=false;boundary_=0x27BC58;
    return true;
}
bool OriginalUiGarageSceneCollision50KG::retirePageQueries1424E0(
    const OriginalUiGaragePageOwner50KG& page,OriginalUiGarageStaticModels50KG& owner,D& d)noexcept {
    d={};if(rawListRetirementPage_!=&page||failedRawListPage_||!rawListRetired27BB80())
        return collisionFail(d,S::phase,0x1424E0);
    return retirePageQueriesBody1424E0(page,owner,d);
}
bool OriginalUiGarageSceneCollision50KG::retireFailedPageQueries1424E0(
    const OriginalUiGaragePageOwner50KG& page,OriginalUiGarageStaticModels50KG& owner,D& d)noexcept {
    d={};if(!failedRawListRetired27BB80(page)||!page.partialSceneAttachmentFailureCurrent()||
       !owner.failedPageRetirement_||owner.pageRetirementOwner_!=&page)
        return collisionFail(d,S::phase,0x1424E0);
    return retirePageQueriesBody1424E0(page,owner,d);
}
bool OriginalUiGarageSceneCollision50KG::retirePageQueriesBody1424E0(
    const OriginalUiGaragePageOwner50KG& page,OriginalUiGarageStaticModels50KG& owner,D& d)noexcept {
    d={};if(!scene_||!grant_||!grant_->current()||!belongsTo(*scene_,grant_,scene_->assets_)||
       scene_->sceneCollision_.get()!=this||scene_->staticModels_.get()!=&owner||
       zone3C4_!=&owner.list3C4_||owner.sceneOwner_!=scene_||owner.grant_!=grant_||
       owner.assets_!=scene_->assets_||owner.resources_!=resources_||owner.visibility_!=visibility_||
       !complete_||completed_!=count||!visibility_||!visibilityBound_)return collisionFail(d,S::phase,0x1424E0);
    // A receipt for this exact suffix, not permission to retire the static
    // parent or release the still-attached dead secondary SearchList rows.
    if(pageQueriesRetired_)return true;
    if(!owner.populationComplete_||owner.populated_!=count||!owner.listLive_||owner.retiring_||
       owner.closed_||owner.count30_!=count||!owner.entries34_||
       !grant_->garagePageStaticListRetirementCurrent(page,*scene_,owner.list3C4_))
        return collisionFail(d,S::phase,0x142590);
    // Validate the real visual prefix and exact query suffix before walking.
    // After a failed destructor, only an already-retired suffix and at most
    // one in-progress tail may be missing from the live ownership chain.
    const C* previous=nullptr;const C* child=zone3C4_->firstChild18;
    for(std::size_t i=0;i<count;++i){const auto& model=owner.models_[i];
        if(!model||!owner.enrolled_[i]||!owner.attached_[i]||!model->current())
            return collisionFail(d,S::phase,0x1425D8);
        const auto* entity=model->scene_.modelEntity8();
        if(!entity||child!=entity||entity->parent1C!=zone3C4_||entity->previous10!=previous||
           entity->reference00!=1||entity->mask20||entity->descendants24||entity->firstChild18)
            return collisionFail(d,S::phase,0x1425D8);
        previous=entity;child=entity->next14;
    }
    const C* originalPrevious=previous;bool deadSuffix=false,pendingTail=false;
    for(std::size_t i=0;i<count;++i){const auto& row=rows_[i];const auto* q=row.query.get();
        if(!row.attempted||!row.completed||row.wrapper||!row.wrapperIdentity||!q||q->sharedObserver||
           q->shape.registrationE0||q->model110)return collisionFail(d,S::phase,0x11B4A0);
        //11B4E8/FC/510 only see the three genuinely empty constructor lists.
        for(const auto& filter:q->filters)if(filter.data||filter.count)
            return collisionFail(d,S::phase,0x11B4E8);
        if(q->providerAbsent){
            if(!q->retired||q->retiring||q->entityLive||q->entity8.reference00||
               q->entity8.parent1C||q->entity8.previous10||q->entity8.next14||q->entity8.firstChild18||
               q->resource124||q->loading||q->shape8C||q->loaded||q->membership.attached)
                return collisionFail(d,S::phase,0x1B574C);
            continue; // Not a new11B4A0 call during this Page child walk.
        }
        const auto& entity=q->entity8;
        if(entity.parent1C!=zone3C4_||entity.previous10!=originalPrevious||
           entity.mask20||entity.descendants24||entity.firstChild18||
           !visibility_->collisionMembershipCurrent(*this,i))
            return collisionFail(d,S::phase,0x1424E0);
        originalPrevious=&entity;
        if(q->retired){
            deadSuffix=true;
            if(q->retiring||q->entityLive||entity.reference00||entity.next14||q->resource124||
               q->loading||q->shape8C||q->loaded)return collisionFail(d,S::phase,0x11B558);
        }else if(q->retiring){
            if(deadSuffix||pendingTail||!q->entityLive||entity.reference00||entity.next14)
                return collisionFail(d,S::phase,0x11B4A0);
            pendingTail=true;deadSuffix=true;
        }else {
            if(deadSuffix||!q->entityLive||entity.reference00!=1||child!=&entity||
               entity.previous10!=previous||!q->loaded||!q->shape8C||q->loading||
               !q->resource124||!q->resource124->live())return collisionFail(d,S::phase,0x1424E0);
            previous=&entity;child=entity.next14;
        }
    }
    if(child)return collisionFail(d,S::phase,0x1424E0);
    for(std::size_t i=count;i--;){auto& q=*rows_[i].query;
        if(q.providerAbsent||q.retired)continue;
        if(!grant_->garagePageStaticListRetirementCurrent(page,*scene_,owner.list3C4_))
            return collisionFail(d,S::phase,0x142590);
        const C* tail=nullptr;std::size_t remaining=count*2;
        for(const C* node=zone3C4_->firstChild18;node;node=node->next14){
            if(!remaining--||node->parent1C!=zone3C4_||node->previous10!=tail||node->reference00!=1)
                return collisionFail(d,S::phase,0x1424E0);
            tail=node;
        }
        //1424E0 reaches the actual current tail, never an ordinal-only
        // detached cleanup. A pending destructor has already unlinked itself
        // but preserves parent/previous, and must finish before the next tail.
        if(q.entity8.next14||(!q.retiring?tail!=&q.entity8:tail!=q.entity8.previous10))
            return collisionFail(d,S::phase,0x142564);
        if(!retireQuery11B4A0(i,d))return false;
    }
    //11B4A0 does not remove its dead SearchList row because sourceE0 is NULL.
    // Keep every Query, membership buffer and visibility binding until the
    // distinct later real SearchList allocation release is implemented.
    pageQueriesRetired_=true;boundary_=0x142578;return true;
}
bool OriginalUiGarageSceneCollision50KG::closePartial2743B0(D& d)noexcept {
    d={};if(closed_)return true;if(!attempted_){closed_=true;return true;}
    if(rawListRetirementPage_||failedRawListPage_)return collisionFail(d,S::phase,0x26FF6C);
    // Positive rows are tail children after the original visual30. Reverse
    // native abort cannot discard them when static model cleanup is attempted.
    for(std::size_t i=count;i--;){auto& row=rows_[i];
        if(row.query&&row.query->entityLive&&!retireQuery11B4A0(i,d))return false;
    }
    if(visibilityBound_&&!visibility_->endCollisionPopulationRetirement(*this,d))return false;
    visibilityBound_=false;complete_=false;closed_=true;return true;
}
}
