#include "indexed_type2_contact50cc.h"
#include "collision_mesh_admission50cc.h"
#include "type2_triangle_axes49x.h"
#include "native_gate_geometry50kg.h"
#include <algorithm>
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(const Vector3_48C& v) noexcept {for(float f:v.v)if(!std::isfinite(f))return false;return true;}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
Vector3_48C vector(const std::array<float,3>& a) noexcept {return {{a[0],a[1],a[2]}};}
Vector3_48C difference(const Vector3_48C& a,const Vector3_48C& b) noexcept {
    return {{sub(a.v[0],b.v[0]),sub(a.v[1],b.v[1]),sub(a.v[2],b.v[2])}};
}
Vector3_48C direction(const BodyMatrix48G& m,const Vector3_48C& a) noexcept {
    Vector3_48C r{};
    for(unsigned i=0;i<3;++i)r.v[i]=add(add(mul(m.v[i],a.v[0]),mul(m.v[4+i],a.v[1])),mul(m.v[8+i],a.v[2]));
    return r;
}
Vector3_48C point(const BodyMatrix48G& m,const Vector3_48C& a) noexcept {
    auto r=direction(m,a);for(unsigned i=0;i<3;++i)r.v[i]=add(r.v[i],m.v[12+i]);return r;
}
BodyMatrix48G relative(const BodyMatrix48G& target,const BodyMatrix48G& query) noexcept {
    // 108BA4..108C08 transposes basis, with the original add-zero operations;
    // it does not perform a general inverse or repair a nonorthogonal basis.
    BodyMatrix48G inverse{};inverse.v[15]=1.f;
    for(unsigned c=0;c<3;++c)for(unsigned i=0;i<3;++i)inverse.v[c*4+i]=add(target.v[i*4+c],0.f);
    auto t=direction(inverse,{{target.v[12],target.v[13],target.v[14]}});
    for(unsigned i=0;i<3;++i)inverse.v[12+i]=mul(t.v[i],-1.f);
    BodyMatrix48G r{};
    for(unsigned c=0;c<4;++c)for(unsigned i=0;i<4;++i)
        r.v[c*4+i]=add(add(add(mul(inverse.v[i],query.v[c*4]),mul(inverse.v[4+i],query.v[c*4+1])),mul(inverse.v[8+i],query.v[c*4+2])),mul(inverse.v[12+i],query.v[c*4+3]));
    return r;
}
bool segmentBox(const assets::CollisionMeshNode48I& n,const Vector3_48C& first,const Vector3_48C& second) noexcept {
    const auto a=difference(first,vector(n.center)),b=difference(second,vector(n.center));
    Vector3_48C mid{},half{},absHalf{};
    for(unsigned i=0;i<3;++i) {
        mid.v[i]=mul(.5f,add(a.v[i],b.v[i]));
        half.v[i]=sub(mid.v[i],a.v[i]);absHalf.v[i]=std::fabs(half.v[i]);
        if(!(sub(sub(std::fabs(mid.v[i]),n.half[i]),absHalf.v[i])<=0.f))return false;
    }
    // Keep the three original separate multiply/add or multiply/subtract pairs.
    const unsigned a0[3]={1,0,0},a1[3]={2,2,1};
    for(unsigned k=0;k<3;++k) {
        const unsigned i=a0[k],j=a1[k];
        const float lhs=std::fabs(sub(mul(mid.v[i],half.v[j]),mul(mid.v[j],half.v[i])));
        const float rhs=add(mul(n.half[i],absHalf.v[j]),mul(n.half[j],absHalf.v[i]));
        if(!(lhs<=rhs))return false;
    }
    return true;
}
float scalarLength(const Vector3_48C& d) noexcept {
    const float squared=add(add(mul(d.v[0],d.v[0]),mul(d.v[1],d.v[1])),mul(d.v[2],d.v[2]));
    if(squared==0.f)return 0.f;
    // EE scalar positive RSQRT interface: double root/divide, rounded once;
    // then original reciprocal to get length (1086B0/1086BC).
    volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(squared)));
    return div(1.f,reciprocal);
}

bool finite(const CollisionSphere48J& s) noexcept {return finite(s.center)&&std::isfinite(s.radius)&&s.radius>=0.f;}
bool member(const std::vector<std::uint32_t>& values,std::uint32_t id) noexcept {
    return std::find(values.begin(),values.end(),id)!=values.end();
}
bool identitiesPass(const CollisionIdentityList48J& list,std::uint32_t other) noexcept {
    // Empty identity lists bypass the mode test in the original.
    if(list.identities.empty())return true;
    const bool found=member(list.identities,other);
    if(found&&list.mode==1)return false;
    if(!found&&list.mode==0)return false;
    return true;
}
bool categoriesPass(const CollisionFilter48J& f,std::uint32_t other) noexcept {
    bool found=false;for(auto mask:f.categoryMasks)if(mask&other){found=true;break;}
    // Unlike identity lists, an empty category allow-list rejects.
    return f.categoryModeD4==1?!found:found;
}
bool filter(const CollisionFilter48J& query,const CollisionFilter48J& candidate) noexcept {
    if(query.identity==candidate.identity||!(candidate.flagsF8&4u)||!(candidate.flags31&1u))return false;
    if(!(candidate.categoryA4&query.acceptMaskB8))return false;
    for(unsigned i=0;i<2;++i)
        if(!identitiesPass(query.identityLists[i],candidate.identity)||
           !identitiesPass(candidate.identityLists[i],query.identity))return false;
    return categoriesPass(query,candidate.categoryA4)&&categoriesPass(candidate,query.categoryA4);
}
bool spheres(const CollisionSphere48J& a,const CollisionSphere48J& b,bool& overlap) noexcept {
    const float x=sub(a.center.v[0],b.center.v[0]);
    const float y=sub(a.center.v[1],b.center.v[1]);
    const float z=sub(a.center.v[2],b.center.v[2]);
    const float squared=add(add(mul(x,x),mul(y,y)),mul(z,z));
    const float radius=add(a.radius,b.radius);
    volatile float distance=std::sqrt(squared);
    const float gap=sub(distance,radius);
    if(!std::isfinite(gap))return false;
    overlap=gap<=0.f;return true;
}
static_assert(std::is_trivially_copyable<SpringVector4_48N>::value,"Opaque vector transport must be byte-copyable");
static_assert(std::is_trivially_copyable<Type2Selected49X>::value,"Selected state must be byte-copyable");
static_assert(std::is_trivially_copyable<Type2StaticResult49X>::value,"Result state must be byte-copyable");
static_assert(std::is_trivially_copyable<TerrainCandidateEffects48J>::value,"Candidate effects must be byte-copyable");
using Status=Type2StaticStatus49X;
constexpr std::uint16_t leaf=0x8000u;
Vector3_48C xyz(const SpringVector4_48N& p) noexcept {return {{p.v[0],p.v[1],p.v[2]}};}
void setXYZ(SpringVector4_48N& p,const Vector3_48C& a) noexcept {std::memcpy(p.v,a.v,3*sizeof(float));}
SpringVector4_48N vector4(const Vector3_48C& a,float w) noexcept {return {{a.v[0],a.v[1],a.v[2],w}};}
bool sphereBox(const Vector3_48C& transformedCenter,const std::array<float,3>& center,
    const std::array<float,3>& half,float radius) noexcept {
    float residual[3];
    for(unsigned i=0;i<3;++i){
        const float a=std::fabs(sub(transformedCenter.v[i],center[i]));
        if(!(sub(sub(a,half[i]),radius)<=0.f))return false;
        // 107910 selects componentwise min with the original strict compare.
        const float closest=a<half[i]?a:half[i];
        residual[i]=sub(a,closest);
    }
    const float squared=add(add(mul(residual[0],residual[0]),mul(residual[1],residual[1])),mul(residual[2],residual[2]));
    return !(mul(radius,radius)<=squared); // tangent equality is rejected
}
Vector3_48C localCorrection(const TriangleHit48H& hit,const DirectedSegment48H& segment,
    const assets::CollisionMeshFace48I& face) noexcept {
    float scale;const std::uint32_t bits=0x38000100u;std::memcpy(&scale,&bits,4);
    Vector3_48C normal{},remaining{};
    for(unsigned i=0;i<3;++i){
        normal.v[i]=mul(scale,static_cast<float>(face.normal[i]));
        remaining.v[i]=mul(sub(1.f,hit.fraction),mul(segment.length,segment.direction.v[i]));
    }
    const float projected=add(add(mul(remaining.v[0],normal.v[0]),mul(remaining.v[1],normal.v[1])),mul(1.f,mul(remaining.v[2],normal.v[2])));
    for(unsigned i=0;i<3;++i)remaining.v[i]=mul(projected,normal.v[i]);
    return remaining;
}
void worldDirection(SpringVector4_48N& a,const BodyMatrix48G& m,float w) noexcept {
    auto v=direction(m,xyz(a));for(float& f:v.v)f=add(f,0.f);setXYZ(a,v);a.v[3]=w;
}
void worldPoint(SpringVector4_48N& a,const BodyMatrix48G& m) noexcept {setXYZ(a,point(m,xyz(a)));a.v[3]=1.f;}
struct Range {std::uintptr_t begin=0,end=0;};
bool range(const void* p,std::size_t count,std::size_t size,Range& out) noexcept {
    out={};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/size)return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p),bytes=count*size;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    out={first,first+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool aligned(const T* p,std::size_t n) noexcept {return !n||reinterpret_cast<std::uintptr_t>(p)%alignof(T)==0;}
Status validate(const Type2StaticQuery49X& q,const Type2StaticCandidate49X* candidates,std::size_t ownerCount,const std::size_t* indices,std::size_t count,
    const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,
    const Type2StaticDestination49X& d,Type2StaticResult49X& result,
    const Type2WorldContinuation50CC* continuation,std::uint32_t* categoryOutput,
    Type2WorldGeometryReached50CC* geometryPrefix,
    const Type2BorrowedTargetFilter50CC* borrowedFilter) noexcept {
    Range writes[16]{};unsigned nw=0;
    const auto write=[&](auto* p,std::size_t n) noexcept {
        if(nw==16||!aligned(p,n)||!range(p,n,sizeof(*p),writes[nw]))return false;
        for(unsigned i=0;i<nw;++i)if(overlap(writes[nw],writes[i]))return false;
        ++nw;return true;
    };
    if(!write(w.pairs,w.pairCapacity)||!write(w.metadata,w.metadataCapacity)||!write(w.records,w.recordCapacity)||
       !write(w.effects,w.effectCapacity)||!write(w.stack,w.stackCapacity)||!write(w.trace,w.traceCapacity)||
       !write(d.pairs,d.pairCapacity)||!write(d.metadata,d.metadataCapacity)||!write(d.records,d.recordCapacity)||
       !write(d.effects,d.effectCapacity)||!write(&result,1)||
       (categoryOutput&&!write(categoryOutput,1))||
       (geometryPrefix&&!write(geometryPrefix,1)))return Status::InvalidInput;
    // Admit one shared GC Gate OR one shared bounded trigger group as a writable
    // track owner before all immutable spans. Mixing these schemas is rejected;
    // duplicate candidate visits may share the same owner.
    Range ownerRange{},indexRange{};GateTriggerBinding50KG* gateOwner=nullptr;NativeTriggerGroup50GA* triggerOwner=nullptr;NativeTriggerGroup50TC* tcOwner=nullptr;NativeTriggerGroup50TB* tbOwner=nullptr;NativeTriggerGroup50MC* mcOwner=nullptr;NativeTriggerGroup50GB* gbOwner=nullptr;
    if(!aligned(candidates,ownerCount)||!range(candidates,ownerCount,sizeof(*candidates),ownerRange)||
       !aligned(indices,count)||!range(indices,count,sizeof(*indices),indexRange))return Status::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        if(indices[i]>=ownerCount)return Status::InvalidInput;
        const auto& candidate=candidates[indices[i]];
        // Native trigger state belongs only to an admitted shape5/6 owner.
        // Reject stray pointers/indices before any group dereference or write.
        if(((candidate.nativeTriggers||candidate.nativeTriggersTC||candidate.nativeTriggersMC||candidate.nativeTriggersTB||candidate.nativeTriggersGB)&&candidate.shapeType!=5&&candidate.shapeType!=6)||
           (!candidate.nativeTriggers&&!candidate.nativeTriggersTC&&!candidate.nativeTriggersMC&&!candidate.nativeTriggersTB&&!candidate.nativeTriggersGB&&candidate.nativeTriggerIndex!=0))return Status::InvalidInput;
        if(candidate.nativeTriggersGB&&candidate.shapeType!=5)return Status::InvalidInput;
        if(candidate.raOriginalTriggerSegments&&
           (candidate.shapeType!=5||candidate.gate||candidate.nativeTriggersTC||candidate.nativeTriggersMC||candidate.nativeTriggersTB||candidate.nativeTriggersGB||!candidate.nativeTriggers||candidate.nativeTriggerIndex>=2))
            return Status::InvalidInput;
        if(candidate.shapeType==5||candidate.shapeType==6){
            if((candidate.shapeType==6&&candidate.gate)||(unsigned(bool(candidate.gate))+unsigned(bool(candidate.nativeTriggers))+unsigned(bool(candidate.nativeTriggersTC))+unsigned(bool(candidate.nativeTriggersMC))+unsigned(bool(candidate.nativeTriggersTB))+unsigned(bool(candidate.nativeTriggersGB))!=1u))return Status::InvalidInput;
            if(candidate.gate){
                if(mcOwner||gbOwner||tbOwner||tcOwner||triggerOwner||(gateOwner&&gateOwner!=candidate.gate))return Status::UnsupportedScope;
                gateOwner=candidate.gate;
            }else if(candidate.nativeTriggersTC){
                if(mcOwner||gbOwner||tbOwner||gateOwner||triggerOwner||(tcOwner&&tcOwner!=candidate.nativeTriggersTC))return Status::InvalidInput;
                tcOwner=candidate.nativeTriggersTC;
            }else if(candidate.nativeTriggersMC){
                if(tbOwner||gbOwner||tcOwner||gateOwner||triggerOwner||(mcOwner&&mcOwner!=candidate.nativeTriggersMC))return Status::InvalidInput;
                mcOwner=candidate.nativeTriggersMC;
            }else if(candidate.nativeTriggersTB){
                if(mcOwner||gbOwner||tcOwner||gateOwner||triggerOwner||(tbOwner&&tbOwner!=candidate.nativeTriggersTB))return Status::InvalidInput;
                tbOwner=candidate.nativeTriggersTB;
            }else if(candidate.nativeTriggersGB){
                if(mcOwner||tbOwner||tcOwner||gateOwner||triggerOwner||(gbOwner&&gbOwner!=candidate.nativeTriggersGB))return Status::InvalidInput;
                gbOwner=candidate.nativeTriggersGB;
            }else{
                if(mcOwner||gbOwner||tbOwner||tcOwner||gateOwner||(triggerOwner&&triggerOwner!=candidate.nativeTriggers))return Status::UnsupportedScope;
                triggerOwner=candidate.nativeTriggers;
            }
        }
    }
    if(gateOwner&&!write(gateOwner,1))return Status::InvalidInput;
    if(triggerOwner&&(!write(triggerOwner,1)||!validNativeTriggerGroup50GA(*triggerOwner)))return Status::InvalidInput;
    if(tcOwner&&(!write(tcOwner,1)||!validNativeTriggerGroup50TC(*tcOwner)))return Status::InvalidInput;
    if(tbOwner&&(!write(tbOwner,1)||!validNativeTriggerGroup50TB(*tbOwner)))return Status::InvalidInput;
    if(mcOwner&&(!write(mcOwner,1)||!validNativeTriggerGroup50MC(*mcOwner)||!write(mcOwner->chance.raw,1)||!write(mcOwner->chance.shared,1)||!write(mcOwner->chance.clock,1)))return Status::InvalidInput;
    if(gbOwner&&(!write(gbOwner,1)||!validNativeTriggerGroup50GB(*gbOwner)))return Status::InvalidInput;
    const auto read=[&](const auto* p,std::size_t n) noexcept {
        Range input{};if(!aligned(p,n)||!range(p,n,sizeof(*p),input))return false;
        for(unsigned i=0;i<nw;++i)if(overlap(input,writes[i]))return false;
        return true;
    };
    if(!read(&q,1)||!read(&initial,1)||!read(&w,1)||!read(&d,1)||!read(candidates,ownerCount)||!read(indices,count)||
       !read(q.resource,1)||!read(q.filter,1)||!read(initial.pairs,initial.pairSlots)||
       !read(initial.metadata,initial.pairSlots)||!read(initial.recordSlots,initial.recordCount))return Status::InvalidInput;
    if(tcOwner&&!nativeTriggerQueryBound50TC(*tcOwner,q.filter->identity))return Status::InvalidInput;
    if(tbOwner&&!nativeTriggerQueryBound50TB(*tbOwner,q.filter->identity))return Status::InvalidInput;
    if(mcOwner&&!nativeTriggerQueryBound50MC(*mcOwner,q.filter->identity))return Status::InvalidInput;
    if(geometryPrefix&&(!continuation||count!=1u||ownerCount!=1u||candidates[0].shapeType!=0u))
        return Status::UnsupportedScope;
    if(continuation){
        if(!read(continuation,1)||!categoryOutput||count!=1||ownerCount!=1||
           (continuation->previous&&!read(continuation->previous,1))||
           (continuation->targetWorldOverride&&!read(continuation->targetWorldOverride,1))||
           (continuation->currentEffect&&!read(continuation->currentEffect,1)))return Status::InvalidInput;
        if(candidates[0].shapeType!=0&&candidates[0].shapeType!=1)return Status::UnsupportedScope;
        if(continuation->targetWorldOverride&&candidates[0].shapeType!=0)return Status::InvalidInput;
        if(continuation->currentEffect&&continuation->currentEffect->identity!=
           candidates[0].candidate.collision.filter.identity)return Status::InvalidInput;
    }
    const auto readVector=[&](const auto& v) noexcept {return read(v.data(),v.size());};
    const auto readFilter=[&](const CollisionFilter48J& f) noexcept {
        return readVector(f.categoryMasks)&&readVector(f.identityLists[0].identities)&&readVector(f.identityLists[1].identities);
    };
    if(borrowedFilter){
        if(!geometryPrefix||!read(borrowedFilter,1)||!read(borrowedFilter->lists,1)||
           !borrowedFilter->lists)return Status::InvalidInput;
        const auto& f=*borrowedFilter->lists;
        const auto& carrier=candidates[0].candidate.collision.filter;
        if(carrier.identity!=borrowedFilter->identity||carrier.flagsF8!=borrowedFilter->flagsF8||
           carrier.categoryA4!=f.categoryA4||carrier.acceptMaskB8!=f.acceptMaskB8||
           carrier.flags31!=f.flags31||!carrier.identityLists[0].identities.empty()||
           !carrier.identityLists[1].identities.empty()||!carrier.categoryMasks.empty())return Status::InvalidInput;
        const CollectionWords50BX* spans[]={&f.identities[0],&f.identities[1],&f.categories};
        for(const auto* list:spans)
            if(list->count<0||static_cast<std::size_t>(list->count)>list->capacity||
               !read(list->values,list->capacity))return Status::InvalidInput;
    }
    const auto& resource=*q.resource;
    const auto& records=continuation&&continuation->previous?continuation->previous->records:initial.records;
    if(!read(resource.nodes,resource.nodeCount)||!read(resource.rays,resource.rayCount)||!readFilter(*q.filter))return Status::InvalidInput;
    if(!resource.rayCount||resource.rayCount>32768||resource.nodeCount>32768||
       resource.maxDepth>resource.nodeCount+resource.rayCount||std::fegetround()!=FE_TONEAREST||
       !affine(q.worldMatrix)||!finite(q.sphere)||records.generatedCapacity88<0||
       records.generatedCount84<0)return Status::InvalidInput;
    if(continuation&&continuation->previous&&
       (continuation->previous->pairCount>resource.rayCount||continuation->previous->pairCount>initial.pairSlots))return Status::InvalidInput;
    if(continuation&&continuation->previous){
        for(std::size_t i=0;i<continuation->previous->pairCount;++i){
            const auto tag=initial.metadata[i].queryTag;
            if(!(tag&leaf)||(tag&0x7fffu)>=resource.rayCount)return Status::InvalidInput;
            for(std::size_t j=0;j<i;++j)if(initial.metadata[j].queryTag==tag)return Status::InvalidInput;
        }
    }
    if(resource.root&leaf){if((resource.root&0x7fff)>=resource.rayCount)return Status::InvalidInput;}
    else if(resource.root>=resource.nodeCount)return Status::InvalidInput;
    if(count>std::numeric_limits<std::size_t>::max()/resource.rayCount)return Status::InvalidInput;
    if(initial.pairSlots<resource.rayCount||w.pairCapacity<initial.pairSlots||w.metadataCapacity<initial.pairSlots||
       d.pairCapacity<initial.pairSlots||d.metadataCapacity<initial.pairSlots||
       initial.recordCount<static_cast<std::size_t>(records.generatedCapacity88)||
       w.recordCapacity<initial.recordCount||d.recordCapacity<initial.recordCount||w.effectCapacity<count||d.effectCapacity<count)
        return Status::InsufficientCapacity;
    for(std::size_t i=0;i<resource.rayCount;++i)
        if(!finite(xyz(resource.rays[i].first))||!finite(xyz(resource.rays[i].second)))return Status::InvalidInput;
    for(std::size_t i=0;i<resource.nodeCount;++i){
        const auto& n=resource.nodes[i];
        if(!std::isfinite(n.volume)||!std::isfinite(n.radius)||n.radius<0.f||!finite(vector(n.center))||!finite(vector(n.half)))return Status::InvalidInput;
        for(unsigned j=0;j<3;++j)if(n.half[j]<0.f)return Status::InvalidInput;
        for(auto t:n.children)if((t&leaf)?(t&0x7fff)>=resource.rayCount:t>=resource.nodeCount)return Status::InvalidInput;
    }
    std::size_t aggregateVisitBound=0;
    const auto validateMesh=[&](const assets::CollisionMesh48I* mesh,const BodyMatrix48G& worldMatrix,
        const float* radii,std::size_t radiusCount,std::size_t maxDepth) noexcept -> Status {
        if(!read(mesh,1)||!read(radii,radiusCount))return Status::InvalidInput;
        if(!readVector(mesh->nodes)||!readVector(mesh->faces)||!readVector(mesh->vertices)||!readVector(mesh->vertex_normals))return Status::InvalidInput;
        if(mesh->reverse_mode!=0||mesh->nodes.empty()||mesh->faces.empty()||mesh->nodes.size()>32768||mesh->faces.size()>32768||
           radiusCount!=mesh->nodes.size()||!affine(worldMatrix))return Status::InvalidInput;
        if(maxDepth>std::numeric_limits<std::size_t>::max()-resource.maxDepth-1)return Status::InvalidInput;
        if(w.stackCapacity<1+resource.maxDepth+maxDepth)return Status::InsufficientCapacity;
        if(resource.nodeCount+resource.rayCount>std::numeric_limits<std::size_t>::max()/(mesh->nodes.size()+mesh->faces.size()))return Status::InvalidInput;
        const auto candidateVisitBound=(resource.nodeCount+resource.rayCount)*(mesh->nodes.size()+mesh->faces.size());
        if(candidateVisitBound>std::numeric_limits<std::size_t>::max()-aggregateVisitBound)return Status::InvalidInput;
        aggregateVisitBound+=candidateVisitBound;
        // Node/vertex/face walk: the mesh and its radii are cold-owned and
        // immutable for the session, so a pass is cached per mesh per session
        // scope (collision_mesh_admission50cc.h). Same answer as the walk.
        if(!collisionMeshContentsAdmitted50CC(*mesh,radii,radiusCount))return Status::InvalidInput;
        return Status::Complete;
    };
    for(std::size_t i=0;i<count;++i){
        if(indices[i]>=ownerCount)return Status::InvalidInput;
        const auto& candidate=candidates[indices[i]];const auto& c=candidate.candidate.collision;
        if(!readFilter(c.filter)||!read(candidate.candidate.sceneName,candidate.candidate.sceneNameBytes)||!finite(c.sphere))return Status::InvalidInput;
        if(candidate.shapeType==0){
            const auto& world=continuation&&continuation->targetWorldOverride?*continuation->targetWorldOverride:c.geometry.worldMatrix;
            const auto status=validateMesh(c.geometry.mesh,world,candidate.nodeRadii,candidate.nodeRadiusCount,candidate.maxDepth);
            if(status!=Status::Complete)return status;
        }else if(candidate.shapeType==1){
            const auto* compound=candidate.compound;
            if(!read(compound,1)||compound->count>maxCompoundMembers50IJ||!read(compound->parts,compound->count)||
               !affine(compound->ownerWorld)||!compound50ij_detail::box(compound->center,compound->half))return Status::InvalidInput;
            for(const auto& axis:compound->coarseAxes)if(!finite(axis))return Status::InvalidInput;
            for(std::size_t j=0;j<compound->count;++j){const auto& part=compound->parts[j];
                const auto status=validateMesh(part.mesh,part.worldMatrix,part.nodeRadii,part.nodeRadiusCount,part.maxDepth);
                if(status!=Status::Complete)return status;
            }
        }else if(candidate.shapeType==5||candidate.shapeType==6){
            if(!q.nativeGatePolicy||!q.callbackEnabled||q.skipMask!=8u||
               c.shapeType!=candidate.shapeType||c.compound||c.geometry.mesh||c.filter.categoryA4!=8u||
               !c.hasResponseCallbackE8)return Status::UnsupportedScope;
            if(candidate.gate){
                if(candidate.gate!=gateOwner||!candidate.gate->callbacks.activateAnimation)return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersTC){
                if(candidate.nativeTriggersTC!=tcOwner||!tcOwner||candidate.nativeTriggerIndex>=tcOwner->triggers.size()||
                   c.filter.identity!=tcOwner->triggers[candidate.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50TC(*tcOwner,q.filter->identity))return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersMC){
                if(candidate.nativeTriggersMC!=mcOwner||!mcOwner||candidate.nativeTriggerIndex>=mcOwner->triggers.size()||
                   c.filter.identity!=mcOwner->triggers[candidate.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50MC(*mcOwner,q.filter->identity))return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersTB){
                if(candidate.nativeTriggersTB!=tbOwner||!tbOwner||candidate.nativeTriggerIndex>=tbOwner->triggers.size()||
                   c.filter.identity!=tbOwner->triggers[candidate.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50TB(*tbOwner,q.filter->identity))return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersGB){
                if(candidate.shapeType!=5||candidate.nativeTriggersGB!=gbOwner||!gbOwner||candidate.nativeTriggerIndex>=gbOwner->triggers.size()||
                   c.filter.identity!=gbOwner->triggers[candidate.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50GB(*gbOwner,q.filter->identity))return Status::UnsupportedScope;
            }else if(candidate.nativeTriggers!=triggerOwner||!triggerOwner||
                candidate.nativeTriggerIndex>=triggerOwner->triggers.size()||
                c.filter.identity!=triggerOwner->triggers[candidate.nativeTriggerIndex].collider.identity||
                nativeTriggerCollider50GA(*triggerOwner,q.filter->identity)==triggerOwner->colliders.size())return Status::UnsupportedScope;
            if(candidate.shapeType==6&&!validNativeTriggerBox50MA(candidate.nativeTriggerBox))return Status::InvalidInput;
            if(!candidate.raOriginalTriggerSegments&&q.nativeGateHasPrevious&&!affine(q.nativeGatePreviousWorld))return Status::InvalidInput;
        }else return Status::UnsupportedScope;
    }
    return Status::Complete;
}
std::uint16_t tag16(std::uint32_t tag) noexcept {
    return static_cast<std::uint16_t>((tag&0x7fffu)|((tag&assets::collision_mesh_leaf48i)?leaf:0));
}
}

bool evaluateType2CandidateSphere50CC(const CollisionSphere48J& query,
    const CollisionSphere48J& candidate,bool& overlaps) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!finite(query)||!finite(candidate))return false;
    bool value=false;if(!spheres(query,candidate,value))return false;
    overlaps=value;return true;
}
bool passType2CandidateFilter50CC(const CollisionFilter48J& query,
    const CollisionFilter48J& candidate) noexcept {return filter(query,candidate);}

bool passType2BorrowedCandidateFilter50CC(const CollisionFilter48J& query,
    const Type2BorrowedTargetFilter50CC& target,bool& pass) noexcept {
    Range descriptor{},listsRange{},output{};
    if(!aligned(target.lists,1)||!range(target.lists,1,sizeof(*target.lists),listsRange)||
       !range(&target,1,sizeof(target),descriptor)||!range(&pass,1,sizeof(pass),output)||
       overlap(output,descriptor)||overlap(output,listsRange))return false;
    Range queryRange{};if(!range(&query,1,sizeof(query),queryRange)||overlap(output,queryRange))return false;
    const auto& f=*target.lists;
    const CollectionWords50BX* spans[]={&f.identities[0],&f.identities[1],&f.categories};
    for(const auto* list:spans){Range r{};
        if(list->count<0||static_cast<std::size_t>(list->count)>list->capacity||
           !aligned(list->values,list->capacity)||!range(list->values,list->capacity,sizeof(*list->values),r)||
           overlap(output,r))return false;
    }
    const std::vector<std::uint32_t>* queryLists[]={&query.identityLists[0].identities,
        &query.identityLists[1].identities,&query.categoryMasks};
    for(const auto* list:queryLists){Range r{};
        if(!range(list->data(),list->size(),sizeof(std::uint32_t),r)||overlap(output,r))return false;
    }
    const auto identityPass=[](const CollectionWords50BX& list,std::uint32_t identity) noexcept {
        if(!list.count)return true;
        bool found=false;
        for(std::int32_t i=0;i<list.count;++i)if(list.values[i]==identity){found=true;break;}
        return !(found&&list.mode==1)&&!(!found&&list.mode==0);
    };
    bool value=query.identity!=target.identity&&(target.flagsF8&4u)&&(f.flags31&1u)&&
        (f.categoryA4&query.acceptMaskB8);
    for(unsigned i=0;value&&i<2;++i)
        value=identitiesPass(query.identityLists[i],target.identity)&&identityPass(f.identities[i],query.identity);
    if(value){
        bool found=false;
        for(std::int32_t i=0;i<f.categories.count;++i)if(f.categories.values[i]&query.categoryA4){found=true;break;}
        value=categoriesPass(query,f.categoryA4)&&(f.categories.mode==1?!found:found);
    }
    pass=value;return true;
}

static Type2StaticStatus49X runIndexedType2Contacts50CC(const Type2StaticQuery49X& query,
    const Type2StaticCandidate49X* candidates,std::size_t ownerCount,const std::size_t* indices,std::size_t count,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    Type2StaticResult49X& output,const Type2WorldContinuation50CC* continuation,
    std::uint32_t* categoryOutput,Type2WorldGeometryReached50CC* geometryPrefix,
    const Type2BorrowedTargetFilter50CC* borrowedFilter=nullptr) noexcept {
    const auto checked=validate(query,candidates,ownerCount,indices,count,initial,w,d,output,continuation,categoryOutput,geometryPrefix,borrowedFilter);
    if(checked!=Status::Complete)return checked;
    if(query.normalMode54!=0u)return Status::UnsupportedScope;
    const auto& resource=*query.resource;
    Type2StaticResult49X result{};Type2WorldGeometryReached50CC reached{};
    if(continuation&&continuation->previous){
        std::memcpy(&result,continuation->previous,sizeof(result));
        result.traceUsed=0; // per-visit trace scratch is not a retained slot span
    }else{
        std::memcpy(&result.selected,&initial.selected,sizeof(result.selected));result.selected.fraction=1.f;
        result.selected.first={{0.f,0.f,0.f,1.f}};result.selected.second={{0.f,0.f,0.f,1.f}};
        result.records=initial.records;if(query.resetRecordsBefore)resetSpringRecords48N(result.records);
        result.currentCandidateIdentity=query.initialQueryCurrentIdentity;
        result.queryFlagsF8=static_cast<std::uint8_t>((query.filter->flagsF8|0x10u)&~0x20u);
    }
    result.normalModeWrite.bound=query.normalMode54Bound;
    std::memcpy(w.pairs,initial.pairs,initial.pairSlots*sizeof(*w.pairs));
    std::memcpy(w.metadata,initial.metadata,initial.pairSlots*sizeof(*w.metadata));
    if(initial.recordCount)std::memcpy(w.records,initial.recordSlots,initial.recordCount*sizeof(*w.records));
    if(count>std::numeric_limits<std::size_t>::max()-result.candidateCount)return Status::InvalidInput;
    result.candidateCount+=count;
    auto currentCategory=continuation&&continuation->previous?continuation->currentCategory:query.initialCurrentCategory;
    // Last-seen position per owner handle (host stack memory, local to this
    // call, so a nested query cannot disturb it). It gives the same nearest
    // earlier equal handle as the backward scan below, without O(count^2).
    // Small queries, very large ones and huge owner tables keep the scan.
    constexpr std::size_t lastSeenSlots=2048;   // power of two, 16 KB
    constexpr std::uint32_t lastSeenEmpty=0xffffffffu;
    struct LastSeenSlot {std::uint32_t handle,position;};
    LastSeenSlot lastSeen[lastSeenSlots];
    const bool lastSeenUsed=count>32&&count<=lastSeenSlots/2&&ownerCount<lastSeenEmpty;
    std::size_t lastSeenMask=0;
    if(lastSeenUsed){
        std::size_t slots=64;while(slots<2*count)slots<<=1; // at most half full
        lastSeenMask=slots-1;
        for(std::size_t i=0;i<slots;++i)lastSeen[i]={lastSeenEmpty,0u};
    }
    for(std::size_t candidateIndex=0;candidateIndex<count;++candidateIndex){
        const auto& candidate=candidates[indices[candidateIndex]];const auto& c=candidate.candidate.collision;
        auto& effect=w.effects[candidateIndex];effect.identity=c.filter.identity;effect.flagsF8=c.filter.flagsF8;
        std::memcpy(effect.copied40.data(),c.copied40.data(),sizeof(effect.copied40));
        std::memcpy(effect.copied50.data(),c.copied50.data(),sizeof(effect.copied50));
        if(continuation&&continuation->currentEffect)std::memcpy(&effect,continuation->currentEffect,sizeof effect);
        // Repeated handles identify one physical owner; earlier writes must be
        // visible on its next visit even though caller's owner table is const.
        if(lastSeenUsed){
            // validate() proved indices[i]<ownerCount<lastSeenEmpty.
            const auto handle=static_cast<std::uint32_t>(indices[candidateIndex]);
            std::size_t slot=static_cast<std::size_t>(handle*2654435761u)&lastSeenMask;
            while(lastSeen[slot].handle!=lastSeenEmpty&&lastSeen[slot].handle!=handle)slot=(slot+1)&lastSeenMask;
            if(lastSeen[slot].handle==handle)effect=w.effects[lastSeen[slot].position];
            lastSeen[slot]={handle,static_cast<std::uint32_t>(candidateIndex)};
        }else for(std::size_t previous=candidateIndex;previous>0;--previous)
            if(indices[previous-1]==indices[candidateIndex]){
                effect=w.effects[previous-1];break;
            }
        if(!continuation&&c.typeTag!=query.collisionTypeTag){
            if(c.typeTag==query.groupTypeTag&&!(result.queryFlagsF8&1u))return Status::UnsupportedScope;
            continue;
        }
        result.currentCandidateIdentity=c.filter.identity;effect.flagsF8=static_cast<std::uint8_t>(effect.flagsF8|0x10u);
        if(!continuation){
            bool sphereOverlap=false;if(!spheres(query.sphere,c.sphere,sphereOverlap))return Status::InvalidInput;
            // A registered Gate keeps bilateral filtering but its continuous
            // narrowphase may cross while the current endpoint sphere is outside.
            if((!sphereOverlap&&!((candidate.shapeType==5||candidate.shapeType==6)&&!candidate.raOriginalTriggerSegments&&query.nativeGateHasPrevious))||
               !filter(*query.filter,c.filter))continue;
        }
        // validate() admitted this exact shape5/6 group's writable range and
        // identity/index before scratch or shared action state can be changed.
        if((candidate.shapeType==5||candidate.shapeType==6)&&candidate.nativeTriggers&&
           !nativeTriggerPairAllowed50GA(*candidate.nativeTriggers,
            candidate.nativeTriggerIndex,query.filter->identity))continue;
        if((candidate.shapeType==5||candidate.shapeType==6)&&candidate.nativeTriggersTC&&
           !nativeTriggerPairAllowed50TC(*candidate.nativeTriggersTC,candidate.nativeTriggerIndex,query.filter->identity))continue;
        if((candidate.shapeType==5||candidate.shapeType==6)&&candidate.nativeTriggersMC&&
           !nativeTriggerPairAllowed50MC(*candidate.nativeTriggersMC,candidate.nativeTriggerIndex,query.filter->identity))continue;
        if((candidate.shapeType==5||candidate.shapeType==6)&&candidate.nativeTriggersTB&&
           !nativeTriggerPairAllowed50TB(*candidate.nativeTriggersTB,candidate.nativeTriggerIndex,query.filter->identity))continue;
        if(candidate.shapeType==5&&candidate.nativeTriggersGB&&
           !nativeTriggerPairAllowed50GB(*candidate.nativeTriggersGB,candidate.nativeTriggerIndex,query.filter->identity))continue;
        if(c.geometry.id!=c.filter.identity)return Status::InvalidInput;
        //121438 follows the genuine outer sphere/filter gate and precedes
        // target-shape dispatch, including a later coarse/triangle miss.
        recordCollisionNormalMode50JA(result.normalModeWrite,query.normalMode54);
        bool candidateHit=false;
        const auto visitMesh=[&](const assets::CollisionMesh48I& mesh,const BodyMatrix48G& worldMatrix,
            const float* radii) noexcept -> Status {
        const auto queryToTarget=relative(worldMatrix,query.worldMatrix);
        const auto targetToQuery=relative(query.worldMatrix,worldMatrix);
        if(!affine(queryToTarget)||!affine(targetToQuery))return Status::InvalidInput;
        std::size_t used=1,visits=0;w.stack[0]={resource.root,0};
        result.stackHighWater=std::max(result.stackHighWater,used);
        const auto visitLimit=(resource.nodeCount+resource.rayCount)*(mesh.nodes.size()+mesh.faces.size());
        bool partHit=false;
        while(used){
            if(visits++==visitLimit)return Status::InvalidInput;
            ++result.visits;const auto item=w.stack[--used];
            const auto qi=static_cast<std::size_t>(item.queryTag&0x7fff),ti=static_cast<std::size_t>(item.targetTag&0x7fff);
            const bool qleaf=(item.queryTag&leaf)!=0,tleaf=(item.targetTag&leaf)!=0;
            std::uint8_t outcome=0;bool splitQuery=false,splitTarget=false;
            if(qleaf&&tleaf){
                const auto& ray=resource.rays[qi];const auto first=point(queryToTarget,xyz(ray.first)),second=point(queryToTarget,xyz(ray.second));
                if(!finite(first)||!finite(second))return Status::InvalidInput;
                const auto delta=difference(second,first);const float length=scalarLength(delta);
                if(!std::isfinite(length))return Status::InvalidInput;
                if(length!=0.f){
                    const float inverse=div(1.f,length);
                    const DirectedSegment48H segment{first,{{mul(delta.v[0],inverse),mul(delta.v[1],inverse),mul(delta.v[2],inverse)}},length};
                    const auto& face=mesh.faces[ti];Triangle48H triangle{};
                    // 10C4C8..10C4F4 mode0 always returns geometric normal,
                    // independently of the mesh's optional normal resource.
                    triangle.hasVertexNormals=false;
                    for(unsigned j=0;j<3;++j)triangle.vertices[j]=vector(mesh.vertices[face.vertices[j]]);
                    TriangleHit48H hit{};const auto status=intersectTriangle48H(segment,triangle,hit);
                    if(status==TriangleQueryStatus48H::InvalidInput)return Status::InvalidInput;
                    if(status==TriangleQueryStatus48H::UnsupportedInterpolationBoundary)return Status::UnsupportedScope;
                    if(status==TriangleQueryStatus48H::Hit){
                        outcome=1;partHit=true;++result.leafHits;
                        std::size_t pair=0;while(pair<result.pairCount&&w.metadata[pair].queryTag!=item.queryTag)++pair;
                        const bool fresh=pair==result.pairCount;
                        if(fresh&&(pair>=resource.rayCount||pair>=w.pairCapacity||pair>=w.metadataCapacity))
                            return Status::InsufficientCapacity;
                        if(fresh||hit.fraction<w.metadata[pair].fraction){
                            auto& m=w.metadata[pair];auto& p=w.pairs[pair];
                            if(fresh){std::memcpy(&p.first,&ray.first,sizeof(p.first));std::memcpy(&p.second,&ray.second,sizeof(p.second));m.queryTag=item.queryTag;++result.pairCount;}
                            m.fraction=hit.fraction;m.worldConverted=0;m.candidateIdentity=c.geometry.id;m.face=static_cast<std::uint32_t>(ti);
                            p.normal=vector4(hit.normal,0.f);p.surfacePoint=vector4(hit.point,1.f);
                        }
                        if(hit.fraction<=result.selected.fraction){
                            const auto correction=localCorrection(hit,segment,face);
                            if(!finite(correction))return Status::InvalidInput;
                            auto& selected=result.selected;std::memcpy(&selected.first,&ray.first,sizeof(selected.first));std::memcpy(&selected.second,&ray.second,sizeof(selected.second));
                            selected.normal=vector4(hit.normal,0.f);selected.surfacePoint=vector4(hit.point,1.f);
                            selected.correction=vector4(correction,1.f);selected.fraction=hit.fraction;
                            selected.flags=static_cast<std::uint8_t>(selected.flags&~1u);
                            selected.candidateIdentity=c.geometry.id;selected.face=static_cast<std::uint32_t>(ti);selected.queryTag=item.queryTag;
                        }
                    }
                }
            }else if(qleaf){
                const auto& ray=resource.rays[qi];
                const auto first=point(queryToTarget,xyz(ray.first)),second=point(queryToTarget,xyz(ray.second));
                if(!finite(first)||!finite(second))return Status::InvalidInput;
                splitTarget=segmentBox(mesh.nodes[ti],first,second);
            }else if(tleaf){
                const auto& node=resource.nodes[qi];const auto& face=mesh.faces[ti];
                // Standalone source-exact107940 helper; no generic SAT substitute.
                type2_axis49x::Input axis{};axis.center=node.center;axis.half=node.half;axis.biasedFaceNormal=face.normal;
                std::copy_n(targetToQuery.v,16,axis.targetToQuery.begin());
                for(unsigned j=0;j<3;++j)axis.vertex[j]=mesh.vertices[face.vertices[j]];
                const auto overlap=type2_axis49x::overlap(axis);
                if(overlap==type2_axis49x::Outcome::InvalidNumeric)return Status::InvalidInput;
                splitQuery=overlap==type2_axis49x::Outcome::Overlap;
            }else{
                const auto& qnode=resource.nodes[qi];const auto& tnode=mesh.nodes[ti];
                const auto tc=point(targetToQuery,vector(tnode.center));
                if(!finite(tc))return Status::InvalidInput;
                // 107698 rejects before1076C0 computes the second center.
                // Do not numerically evaluate an unreachable reverse test.
                if(sphereBox(tc,qnode.center,qnode.half,radii[ti])){
                    const auto qc=point(queryToTarget,vector(qnode.center));
                    if(!finite(qc))return Status::InvalidInput;
                    if(sphereBox(qc,tnode.center,tnode.half,qnode.radius)){
                        splitQuery=qnode.volume>tnode.volume;splitTarget=!splitQuery;
                    }
                }
            }
            if(splitQuery||splitTarget){
                if(w.stackCapacity-used<2)return Status::InsufficientCapacity;
                if(splitQuery){const auto& n=resource.nodes[qi];w.stack[used++]={n.children[1],item.targetTag};w.stack[used++]={n.children[0],item.targetTag};outcome=2;}
                else{const auto& n=mesh.nodes[ti];w.stack[used++]={item.queryTag,tag16(n.children[1])};w.stack[used++]={item.queryTag,tag16(n.children[0])};outcome=3;}
                result.stackHighWater=std::max(result.stackHighWater,used);
            }
            if(w.trace){
                if(result.traceUsed==w.traceCapacity)return Status::InsufficientCapacity;
                w.trace[result.traceUsed++]={c.geometry.id,item.queryTag,item.targetTag,outcome};
            }
        }
        if(!partHit)return Status::Complete;
        candidateHit=true;
        if(!(result.selected.flags&1u)){
            worldDirection(result.selected.normal,worldMatrix,2.f);
            worldDirection(result.selected.correction,worldMatrix,3.f);
            worldPoint(result.selected.surfacePoint,worldMatrix);
            if(!finite(xyz(result.selected.normal))||!finite(xyz(result.selected.correction))||
               !finite(xyz(result.selected.surfacePoint)))return Status::InvalidInput;
            result.selected.flags=static_cast<std::uint8_t>(result.selected.flags|1u);
        }
        for(std::size_t i=0;i<result.pairCount;++i)if(!w.metadata[i].worldConverted){
            worldDirection(w.pairs[i].normal,worldMatrix,2.f);worldPoint(w.pairs[i].surfacePoint,worldMatrix);
            if(!finite(xyz(w.pairs[i].normal))||!finite(xyz(w.pairs[i].surfacePoint)))return Status::InvalidInput;
            w.metadata[i].worldConverted=1;
        }
            return Status::Complete;
        };
        if(candidate.shapeType==0){
            const auto& world=continuation&&continuation->targetWorldOverride?*continuation->targetWorldOverride:c.geometry.worldMatrix;
            const auto status=visitMesh(*c.geometry.mesh,world,candidate.nodeRadii);
            if(status!=Status::Complete)return status;
        }else if(candidate.shapeType==5||candidate.shapeType==6){
            const auto* previous=(!candidate.raOriginalTriggerSegments&&query.nativeGateHasPrevious)?&query.nativeGatePreviousWorld:nullptr;
            const bool valid=candidate.shapeType==6
                ?nativeType2TriggerBoxOverlap50MA(resource,query.worldMatrix,previous,candidate.nativeTriggerBox,candidateHit)
                :nativeType2GateOverlap50KG(resource,query.worldMatrix,previous,c.sphere,candidateHit);
            if(!valid)return Status::InvalidInput;
        }else {
            //121640/122BA0: parent-box coarse test precedes active-child visits.
            const auto& compound=*candidate.compound;Vector3_48C center{},half{};
            if(resource.nodeCount){center=vector(resource.nodes[0].center);half=vector(resource.nodes[0].half);}
            else for(unsigned j=0;j<3;++j){
                center.v[j]=mul(.5f,add(resource.rays[0].second.v[j],resource.rays[0].first.v[j]));
                half.v[j]=mul(.5f,sub(resource.rays[0].second.v[j],resource.rays[0].first.v[j]));
            }
            bool coarse=false;if(!compoundCoarseOverlap50IJ(center,half,query.worldMatrix,compound,coarse))return Status::InvalidInput;
            if(!coarse)continue;
            //12165C..121870: results accumulate and world-convert per hit part;
            // filters/effects/callbacks belong to the one outer owner only.
            for(std::size_t j=0;j<compound.count;++j){const auto& part=compound.parts[j];if(!part.active)continue;
                const auto status=visitMesh(*part.mesh,part.worldMatrix,part.nodeRadii);
                if(status!=Status::Complete)return status;
            }
        }
        if(!candidateHit)continue;
        ++result.geometryHits;result.hasGeometryHit=true;result.queryFlagsF8=static_cast<std::uint8_t>(result.queryFlagsF8|0x20u);
        std::memcpy(effect.copied40.data(),query.copied40.data(),sizeof(effect.copied40));
        std::memcpy(effect.copied50.data(),query.copied50.data(),sizeof(effect.copied50));
        if(geometryPrefix){
            //Explicit122860 boundary after geometry/hit/copied-vector effects.
            //Never disable a source callback or claim this candidate returned.
            reached.hit=true;reached.queryE4Pending=query.callbackEnabled;
            reached.targetE8Pending=c.hasResponseCallbackE8;
            continue;
        }
        if(query.callbackEnabled){
            ++result.callbacks;const auto& material=candidate.candidate;
            const SpringStaticCallbackInput48N callback{w.pairs,result.pairCount,c.filter.identity,c.filter.categoryA4,
                currentCategory,query.skipMask,query.dynamicMask,query.materialMask,2,query.queryScene,
                material.hasMaterialScene,material.sceneName,material.sceneNameBytes};
            SpringStaticCallbackResult48N appended{};const auto before=result.records.currentCandidate130;
            const auto called=applySpringStaticCallback48N(callback,w.records,initial.recordCount,result.records,appended);
            if(called==SpringCallbackStatus48N::InvalidInput)return Status::InvalidInput;
            if(called==SpringCallbackStatus48N::InsufficientCapacity)return Status::InsufficientCapacity;
            if(called==SpringCallbackStatus48N::UnsupportedScope)return Status::UnsupportedScope;
            if(called==SpringCallbackStatus48N::Complete){
                result.appended+=appended.appended;result.saturated+=appended.saturated;
                if(!before||(currentCategory&query.dynamicMask))currentCategory=c.filter.categoryA4;
            }
        }
        if(c.hasResponseCallbackE8){
            if(candidate.shapeType!=5&&candidate.shapeType!=6)return Status::UnsupportedScope;
            //122860..88: E4 skips TRIGGER8 first; E8 then commits its action
            // synchronously. A later numerical failure cannot undo this event.
            if(candidate.nativeTriggersTC){
                const auto receipt=dispatchNativeTriggerContact50TC(*candidate.nativeTriggersTC,
                    candidate.nativeTriggerIndex,query.filter->identity,candidateIndex);
                if(receipt.status!=SourceTriggerContactStatus50TC::Complete)return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersMC){
                const auto receipt=dispatchNativeTriggerContact50MC(*candidate.nativeTriggersMC,
                    candidate.nativeTriggerIndex,query.filter->identity,candidateIndex);
                if(receipt.status!=SourceTriggerContactStatus50MC::Complete)return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersTB){
                const auto receipt=dispatchNativeTriggerContact50TB(*candidate.nativeTriggersTB,
                    candidate.nativeTriggerIndex,query.filter->identity,candidateIndex);
                if(receipt.status!=SourceTriggerContactStatus50TB::Complete)return Status::UnsupportedScope;
            }else if(candidate.nativeTriggersGB){
                const auto receipt=dispatchNativeTriggerContact50GB(*candidate.nativeTriggersGB,
                    candidate.nativeTriggerIndex,query.filter->identity,candidateIndex);
                if(receipt.status!=SourceTriggerContactStatus50GB::Complete)return Status::UnsupportedScope;
            }else if(candidate.nativeTriggers){
                const auto receipt=dispatchNativeTriggerContact50GA(*candidate.nativeTriggers,candidate.nativeTriggerIndex,query.filter->identity);
                if(receipt.status!=TriggerEntryStatus50IN::Complete||receipt.randomConsumed)return Status::UnsupportedScope;
            }else{
                const auto receipt=dispatchGateContact50KG(*candidate.gate,query.filter->identity);
                if(receipt.status==GateTriggerStatus50KG::ActionCallbackFailed)return Status::UnsupportedScope;
            }
        }
    }
    std::memcpy(d.pairs,w.pairs,initial.pairSlots*sizeof(*d.pairs));
    std::memcpy(d.metadata,w.metadata,initial.pairSlots*sizeof(*d.metadata));
    if(initial.recordCount)std::memcpy(d.records,w.records,initial.recordCount*sizeof(*d.records));
    if(count)std::memcpy(d.effects,w.effects,count*sizeof(*d.effects));
    std::memcpy(&output,&result,sizeof(output));
    if(categoryOutput)*categoryOutput=currentCategory;
    if(geometryPrefix)*geometryPrefix=reached;
    return Status::Complete;
}

Type2StaticStatus49X produceIndexedStaticType2Contacts50CC(const Type2StaticQuery49X& query,
    const Type2StaticCandidate49X* candidates,std::size_t ownerCount,const std::size_t* indices,std::size_t count,
    const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    Type2StaticResult49X& output) noexcept {
    return runIndexedType2Contacts50CC(query,candidates,ownerCount,indices,count,initial,w,d,output,nullptr,nullptr,nullptr);
}

Type2StaticStatus49X continueType2WorldCandidate50CC(const Type2StaticQuery49X& query,
    const Type2StaticCandidate49X& candidate,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    const Type2WorldContinuation50CC& continuation,Type2StaticResult49X& output,
    std::uint32_t& currentCategory) noexcept {
    const std::size_t index=0;
    return runIndexedType2Contacts50CC(query,&candidate,1,&index,1,initial,w,d,output,&continuation,&currentCategory,nullptr);
}
Type2WorldGeometryReceipt50CC prepareType2WorldGeometryPrefix50CC(const Type2StaticQuery49X& query,
    const Type2StaticCandidate49X& candidate,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    const Type2WorldContinuation50CC& continuation,Type2WorldGeometryOutput50CC& output) noexcept {
    const std::size_t index=0;
    const auto status=runIndexedType2Contacts50CC(query,&candidate,1,&index,1,initial,w,d,
        output.numerical,&continuation,&output.retainedCategory,&output.reached);
    return {status==Status::Complete?Type2WorldGeometryPhase50CC::GeometryPrefixReady:
        Type2WorldGeometryPhase50CC::Rejected,status};
}

Type2WorldGeometryReceipt50CC prepareType2BorrowedWorldGeometryPrefix50CC(const Type2StaticQuery49X& query,
    const Type2StaticCandidate49X& candidate,const Type2BorrowedTargetFilter50CC& borrowed,
    const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    const Type2WorldContinuation50CC& continuation,Type2WorldGeometryOutput50CC& output) noexcept {
    const std::size_t index=0;
    const auto status=runIndexedType2Contacts50CC(query,&candidate,1,&index,1,initial,w,d,
        output.numerical,&continuation,&output.retainedCategory,&output.reached,&borrowed);
    return {status==Status::Complete?Type2WorldGeometryPhase50CC::GeometryPrefixReady:
        Type2WorldGeometryPhase50CC::Rejected,status};
}

}
