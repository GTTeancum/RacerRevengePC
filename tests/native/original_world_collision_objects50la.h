#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include "../../src/game/original_application_entities50la.h"
#include "ra_static_candidates50la.h"
#include <limits>
#include <optional>
#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_static_model_cold50LA {class Owner;}
namespace rr::game {
// Same collision collection, exclusive actual1B4EC0 descriptor-source mode
// or legacy native271230/reached27B040 custody. Source mode owns no decoded
// candidates, geometry, query entities, registry or physics readiness.
class OriginalWorldCollisionObjects50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_static_model_cold50LA::Owner;
    struct SourceDescriptor {
        std::array<char,260> name00;std::size_t nameKnown=0;
        std::array<std::uint32_t,4> position110,look120,up130,smokeOffset150,smokeVelocity160;
        std::uint32_t vtable170=0,useLighting140=0,dynamic144=0,shadow148=0,smoker14C=0;
        std::uint32_t field180=0,field184=0;std::uint8_t vectorKnown=0;
        bool constructed=false,loaded=false;
        //Native backing initialization is not source knowledge. Unwritten
        //name tail,174/178/17C/188/18C have no source consumer or value.
    };
    struct Descriptor {
        std::optional<SourceDescriptor> sourceFields;
        const assets::WorldInstance48F* source=nullptr;
        std::string name00;
        std::array<float,4> position110{0,0,0,1},look120{0,0,1,1},up130{0,1,0,1};
        std::array<float,4> smokeOffset150{0,0,0,1},smokeVelocity160{0,0,0,1};
        std::uint32_t useLighting140=1,useDynamicLighting144=0,field148=1,isSmoker14C=0;
        std::uint32_t field180=0,field184=0;
        //3CA8F0..3CA94C constructs actual442F80=(0,0,0,1).
    };
    struct NativeQuery {
        OriginalWreckComponent50LA entity; // Actual source collision+8 identity.
        OriginalApplicationEntities50LA* application=nullptr;
        const Type2StaticCandidate49X* candidate=nullptr;
        const assets::WorldInstance48F* source=nullptr;
        const std::uint32_t* liveTerrain180=nullptr;
        std::string name,member;
        std::size_t resourceIndex=std::numeric_limits<std::size_t>::max();
        std::uint32_t sourceCategoryA4=0;
        std::uint8_t sourceType02=0xff,sourceTerrainA8=0,sourceMaterialA9=0;
        // Only source-written lanes; upper unknown bits are not fabricated.
        std::uint8_t queryFlags03KnownMask=7,queryFlags03Value=0;
        std::uint8_t flagsF8KnownMask=0x37,flagsF8Value=6;
        bool counted=false,typed=false,flagsApplied=false;
        bool resourceStarted=false,loaded=false,resourceWithdrawn=false;
        bool appended=false,registered=false,retired=false,terminal=false;
        bool negativeLoadReturned=false;
        //Typed native temporary280890 frame; NOT persistent collisionE0.
        struct TemporaryObserver {
            const void* next00=nullptr;
            const OriginalWreckComponent50LA* child04=nullptr;
            const void* method08=nullptr;
            std::uint32_t field0C=0,field10=0,slot14=0;
            std::array<float,4> center20{{0.f,0.f,0.f,1.f}};
            //Source18/1C untouched; no inferred values or consumers.
        };
        std::unique_ptr<TemporaryObserver> observerToken;
        bool observerSharedConstructed=false,placementAdopted=false,observerAttempted=false;
        std::size_t primaryZone=std::numeric_limits<std::size_t>::max();
        std::uint32_t primarySlot=0;

        std::uint32_t failurePC=0;
    };
    struct Wrapper {
        bool sourceMode=false,sourceTypeKnown=false,sourceDescriptorPublished=false;
        std::uint32_t sourceVtable0C=0;
        std::uint32_t type00=1;
        OriginalWreckComponent50LA* collision04=nullptr;
        std::unique_ptr<Descriptor> descriptor08;
        const ra_cold_sources50LA::CollisionOwner* decodedCollision=nullptr;
        // Immutable authored identity survives source descriptor08 destruction.
        const assets::WorldInstance48F* source=nullptr;
        std::unique_ptr<NativeQuery> nativeQuery;
        bool nativeAttempted=false,descriptorRetired=false;
    };
    struct Zone {
        OriginalWreckComponent50LA entity;
        std::uint32_t count30=0;
        std::vector<Wrapper> entries34;
        bool counted=false;
        bool sourceMode=false,sourceEnrolled=false,sourceCountKnown=false,sourceArrayReturned=false;
        std::uint32_t sourceVtable08=0;
        std::unique_ptr<Wrapper*[]> sourceEntries34;
        std::size_t sourceEntriesKnown=0;
    };
    bool sourceMode_=false;
    std::array<std::unique_ptr<Zone>,10> sourceZones_;
    std::array<std::vector<std::unique_ptr<Wrapper>>,10> sourceWrappers_;
    std::unique_ptr<OriginalWreckComponent50LA*[]> sourceArray3CC_;
    std::size_t sourceArrayKnown_=0;
    bool sourceArrayReturned_=false;
    std::array<bool,10> sourceGroupAllocationReturned_{},sourceLogAttempted_{};
    std::array<std::size_t,10> sourceLogWritten_{};
    std::size_t sourceCountedGroups()const noexcept {
        std::size_t n=0;for(const auto& p:sourceZones_)if(p&&p->counted)++n;return n;
    }
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::array<Zone,10> zones_;
    OriginalWorldCollisionObjects50LA()noexcept=default;
    bool prepareBacking(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(sourceMode_||source_||!source||!source->sourcesDecoded())return false;
        const auto& s=*source->sourcesDecoded();const auto& w=s.world;
        if(w.zones.size()!=10||w.instances.size()!=1261)return false;
        try {
            std::size_t wi=0,ci=0;
            for(std::size_t z=0;z<10;++z){const auto& authored=w.zones[z];
                if(authored.zone!=z+1||!authored.models||authored.parameter_block>=w.parameters.blocks.size())return false;
                // Actual RA fallback2764EC..276508: no explicit count means
                // NumberOfStaticModels and ModelN names, never ObjectN names.
                for(const auto& f:w.parameters.blocks[authored.parameter_block].fields)
                    if(f.name=="numberofcollisionobjects"||f.name=="collisionobjects")return false;
                auto& zone=zones_[z];zone.entries34.resize(authored.models);
                for(auto& wrapper:zone.entries34){
                    if(wi>=w.instances.size())return false;
                    const auto& in=w.instances[wi++];
                    if(in.zone!=z+1||in.ordinal!=&wrapper-zone.entries34.data()+1||
                       in.parameter_block>=w.parameters.blocks.size())return false;
                    const auto& b=w.parameters.blocks[in.parameter_block];if(b.name!=in.section)return false;
                    bool type=false,slt=false,lighting=false;
                    auto d=std::make_unique<Descriptor>();d->source=&in;
                    for(const auto& f:b.fields){
                        if(f.name=="statictype"){if(type||f.value!="Standard")return false;type=true;}
                        else if(f.name=="slt"){
                            if(slt)return false;slt=true;const auto dot=f.value.find('.');
                            if(dot==std::string::npos||!dot||dot>50)return false;
                            d->name00=f.value.substr(0,dot);
                        }else if(f.name=="uselighting"){
                            if(lighting||(f.value!="T"&&f.value!="F"))return false;
                            lighting=true;d->useLighting140=f.value=="T";
                        }else if(f.name=="col"||f.name=="usedynamiclighting"||f.name=="issmoker"||
                                 f.name=="smokeoffset"||f.name=="smokevelocity")return false;
                    }
                    if(!slt)return false;
                    for(unsigned a=0;a<3;++a){
                        const double values[]={in.position[a],in.look[a],in.up[a]};
                        float* dest[]={&d->position110[a],&d->look120[a],&d->up130[a]};
                        for(unsigned n=0;n<3;++n){const float v=static_cast<float>(values[n]);
                            if(!std::isfinite(v)||double(v)!=values[n])return false;*dest[n]=v;}
                    }
                    std::string member="geometry\\"+d->name00+".col";
                    for(char& c:member)if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                    if(member!=in.collision_member)return false;
                    std::size_t archiveMatches=0;
                    for(const auto& entry:s.archive.entries){
                        if(entry.name.size()!=member.size())continue;
                        bool equal=true;
                        for(std::size_t n=0;n<member.size();++n){char c=entry.name[n];
                            if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                            if(c!=member[n]){equal=false;break;}}
                        if(equal)++archiveMatches;
                    }
                    if(archiveMatches>1)return false;
                    if(ci<s.collisions.size()&&s.collisions[ci].zone==in.zone&&s.collisions[ci].ordinal==in.ordinal){
                        if(s.collisions[ci].member!=in.collision_member)return false;
                        wrapper.decodedCollision=&s.collisions[ci++];
                    }
                    if(bool(wrapper.decodedCollision)!=(archiveMatches==1))return false;
                    wrapper.source=&in;wrapper.descriptor08=std::move(d);
                }
            }
            if(wi!=w.instances.size()||ci!=s.collisions.size())return false;
        }catch(...){return false;}
        source_=std::move(source);return true;
    }
    static std::uint64_t nativeKey(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    Wrapper* nativeSlot(std::size_t zone,std::size_t row)noexcept {
        return !sourceMode_&&zone<zones_.size()&&row<zones_[zone].entries34.size()?
            &zones_[zone].entries34[row]:nullptr;
    }
    // Private caller authenticates actual current directory/category producer
    // and candidate owner. Terrain comes from the EXISTING272520 native alias,
    // not stale cold Descriptor180. No global authority enrollment here: the
    // World caller owns the explicit private typed parent/child forest.
    bool beginNativeQuery27B040(std::size_t zone,std::size_t row,
        OriginalApplicationEntities50LA& app,std::uint64_t generation,
        const Type2StaticCandidate49X* candidate,std::size_t resourceIndex,
        std::uint32_t sourceCategory,std::uint32_t& pc)noexcept {
        pc=0x27b0ac;auto* w=nativeSlot(zone,row);
        if(!w||!source_||!w->source||!w->descriptor08||w->descriptorRetired||
           w->descriptor08->source!=w->source||w->type00!=1||w->nativeAttempted||
           w->nativeQuery||w->collision04||!generation||app.attachmentLock423F58()||
           !app.application10()||!zones_[zone].counted||
           zones_[zone].entity.sessionGeneration!=generation||
           app.objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        const auto& d=*w->descriptor08;
        if(d.name00.empty()||d.name00=="none"||d.field184!=0||
           bool(candidate)!=bool(w->decodedCollision))return false;
        if(candidate){
            const auto* decoded=source_->sourcesDecoded();
            if(!decoded||resourceIndex>=decoded->collisions.size()||
               w->decodedCollision!=&decoded->collisions[resourceIndex]||
               w->decodedCollision->zone!=w->source->zone||
               w->decodedCollision->ordinal!=w->source->ordinal||
               candidate->candidate.collision.geometry.mesh!=&w->decodedCollision->mesh||
               !candidate->terrainDescriptor180||*candidate->terrainDescriptor180>1u)
                return false;
        }else if(resourceIndex!=std::numeric_limits<std::size_t>::max()||d.field180!=0)return false;
        w->nativeAttempted=true; // Allocation failure terminal; never recount.
        try {
            w->nativeQuery=std::make_unique<NativeQuery>();auto& q=*w->nativeQuery;
            q.application=&app;q.source=w->source;q.candidate=candidate;
            q.name=d.name00;q.member=w->source->collision_member;
            q.resourceIndex=resourceIndex;q.sourceCategoryA4=sourceCategory;
            q.liveTerrain180=candidate?candidate->terrainDescriptor180:nullptr;
            q.sourceTerrainA8=static_cast<std::uint8_t>(q.liveTerrain180?*q.liveTerrain180:d.field180);
        }catch(...){if(w->nativeQuery){w->nativeQuery->terminal=true;w->nativeQuery->failurePC=pc;}return false;}
        auto& q=*w->nativeQuery;
        pc=0x141300;
        const auto result=initializeOriginalWreckComponent50LA(q.entity,
            OriginalWreckComponentKind50LA::collision11B190,generation,nativeKey(&q.entity),
            true,app.objectCount423F60());
        if(result.status!=OriginalWreckComponentStatus50LA::complete){q.terminal=true;q.failurePC=pc;return false;}
        q.counted=true;
        // Real constructor type service, before wrapper04 publication. Failure
        // retains its already counted prefix; no native candidate tag rewrite.
        pc=0x11b270;const auto type=app.types383408().collisionObject11B270();
        if(!type.okay()){q.terminal=true;q.failurePC=type.sourcePC;return false;}
        q.sourceType02=type.type;q.typed=true;
        pc=0x27b0c4;w->collision04=&q.entity;
        //11B570(0xD): bit3=1,bit1=0,bit2=1. No fullF8 or source
        //global3E0730 publication claimed by these private constructor fields.
        q.flagsF8KnownMask=0x3f;q.flagsF8Value=0x0c;q.flagsApplied=true;
        pc=0x27b118;return true;
    }
    bool nativeQueryCurrent27B040(const Wrapper& w)const noexcept {
        if(!w.nativeQuery)return false;const auto& q=*w.nativeQuery;const auto& e=q.entity;
        if(!q.application||q.source!=w.source||q.retired||!q.counted||
           e.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e.objectKey!=nativeKey(&e)||!e.sessionGeneration||e.reference00!=1||
           e.category28!=0xff||e.flags29Value()!=3||e.nativeFlags29||
           e.firstChild18||e.mask20||e.descendants24)return false;
        if(q.typed&&w.collision04!=&e)return false;
        if(q.candidate){
            if(!w.decodedCollision||q.candidate->candidate.collision.geometry.mesh!=&w.decodedCollision->mesh||
               q.liveTerrain180!=q.candidate->terrainDescriptor180||!q.liveTerrain180||
               static_cast<std::uint8_t>(*q.liveTerrain180)!=q.sourceTerrainA8)return false;
        }else if(w.decodedCollision||q.liveTerrain180)return false;
        return true;
    }
    // The caller invokes this immediately BEFORE actual existing resource
    // load11B920, preserving a failed allocation/publication for cleanup.
    bool beginNativeResource11B920(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x27b11c;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(q.terminal||!q.typed||!q.flagsApplied||q.resourceStarted||q.loaded||q.retired)return false;
        q.resourceStarted=true;return true;
    }
    // Private parent calls only after actual resource load returned, not from
    // prepared(). On false, retain all partial resource/query ownership.
    bool finishNativeResource11B920(std::size_t zone,std::size_t row,bool returned,
        std::uint32_t failurePC,std::uint32_t& pc)noexcept {
        pc=0x27b124;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(!q.resourceStarted||q.loaded||q.terminal||!q.candidate)return false;
        if(!returned){q.terminal=true;q.failurePC=failurePC?failurePC:0x11b920;pc=q.failurePC;return false;}
        q.loaded=true;return true;
    }
    bool canRetireNativeQuery11B4A0(const Wrapper& w)const noexcept {
        if(!w.nativeQuery)return !w.collision04;
        const auto& q=*w.nativeQuery;const auto& e=q.entity;
        if(q.retired)return !q.counted&&!w.collision04&&!e.reference00;
        if(q.observerSharedConstructed||q.observerToken||q.appended||q.registered||e.parent1C||e.previous10||e.next14||e.firstChild18||
           e.mask20||e.descendants24||(q.resourceStarted&&!q.resourceWithdrawn))return false;
        if(!q.counted)return !e.reference00&&!w.collision04;
        return q.application&&q.application->objectCount423F60()>0&&
            !q.application->attachmentLock423F58()&&e.reference00==1&&
            e.kind==OriginalWreckComponentKind50LA::collision11B190&&
            e.objectKey==nativeKey(&e)&&e.category28==0xff&&e.flags29Value()==3&&
            !e.nativeFlags29&&(!w.collision04||w.collision04==&e);
    }
    bool retireNativeQuery11B4A0(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x11b4a0;auto* w=nativeSlot(zone,row);
        if(!w||!canRetireNativeQuery11B4A0(*w))return false;
        if(!w->nativeQuery)return true;auto& q=*w->nativeQuery;
        if(q.retired)return true;
        // Owned e0, dynamic identity lists and callbacks were never introduced
        // in this bounded route. Actual resource withdrawal must precede this.
        if(q.counted){--q.entity.reference00;--q.application->objectCount423F60();q.counted=false;}
        w->collision04=nullptr;q.candidate=nullptr;q.liveTerrain180=nullptr;q.application=nullptr;
        q.retired=true;pc=0x1413e0;return true;
    }
    // The caller's SAME resolver + mounted archive provider must both prove
    // this exact member absent immediately before entry. Original guest fallback
    // history remains unknown: this is the bounded native negative-load route.
    bool completeNativeNegativeLoad27B12C(std::size_t zone,std::size_t row,
        const std::string& authenticatedAbsentMember,std::uint32_t& pc)noexcept {
        pc=0x27b12c;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(q.candidate||q.resourceIndex!=std::numeric_limits<std::size_t>::max()||
           !q.resourceStarted||q.loaded||q.terminal||q.appended||q.registered||
           authenticatedAbsentMember!=q.member)return false;
        q.negativeLoadReturned=true;q.resourceWithdrawn=true; // No acquired shape on file miss.
        if(!retireNativeQuery11B4A0(zone,row,pc))return false;
        pc=0x27b2a4;w->descriptor08.reset();w->descriptorRetired=true;pc=0x27b2b0;return true;
    }
    bool freeNativeDescriptor27B2A4(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x27b2a4;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w)||!w->descriptor08||w->descriptorRetired)return false;
        const auto& q=*w->nativeQuery;
        if(q.terminal||!q.loaded||!q.appended||!q.registered||q.resourceWithdrawn)return false;
        w->descriptor08.reset();w->descriptorRetired=true;pc=0x27b2b0;return true;
    }

public:
    ~OriginalWorldCollisionObjects50LA()noexcept {
        for(const auto& p:sourceZones_)if(p&&p->counted)std::terminate();
        for(const auto& z:zones_){
            if(z.counted)std::terminate();
            for(const auto& w:z.entries34)if(w.collision04||
                (w.nativeQuery&&w.nativeQuery->counted))std::terminate();
        }
    }
};
}
