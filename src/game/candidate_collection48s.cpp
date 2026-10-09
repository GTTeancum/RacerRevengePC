#include "candidate_collection48s.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=CandidateCollectionStatus48S;
using Ref=CollectionRef48S;
struct Span {std::uintptr_t begin=0,end=0;};
template<class T> bool span(const T* p,std::size_t n,Span& r) noexcept {
    r={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto b=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-b)return false;
    r={b,b+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
struct Guard {
    Span writes[16]{};std::size_t used=0;
    template<class T>bool write(T* p,std::size_t n) noexcept {
        Span r;if(!span(p,n,r))return false;
        for(std::size_t i=0;i<used;++i)if(overlap(r,writes[i]))return false;
        writes[used++]=r;return true;
    }
    template<class T>bool read(const T* p,std::size_t n) const noexcept {
        Span r;if(!span(p,n,r))return false;
        for(std::size_t i=0;i<used;++i)if(overlap(r,writes[i]))return false;
        return true;
    }
};
bool writes(Guard& g,const CandidateCollectionBuffers48S& b) noexcept {
    return g.write(b.objects,b.objectCapacity)&&g.write(b.trees,b.treeCapacity)&&
        g.write(b.pools,b.poolCapacity)&&g.write(b.poolBlocks,b.poolBlockCapacity)&&
        g.write(b.poolEntries,b.poolEntryCapacity)&&g.write(b.identities,b.identityCapacity)&&
        g.write(b.shape0States,b.shape0Capacity);
}
bool capacities(const CandidateCollectionInput48S& i,const CandidateCollectionBuffers48S& b) noexcept {
    return b.objectCapacity>=i.objectCount&&b.treeCapacity>=i.treeCount&&b.poolCapacity>=i.poolCount&&
        b.poolBlockCapacity>=i.poolBlockCount&&b.poolEntryCapacity>=i.poolEntryCount&&b.shape0Capacity>=i.shape0Count;
}
template<class T>void copy(T* to,const T* from,std::size_t n) noexcept {
    if(n)std::memcpy(to,from,n*sizeof(T));
}
struct Math {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {volatile float f=keep(a)+keep(b);return keep(f);}
    float sub(float a,float b) noexcept {volatile float f=keep(a)-keep(b);return keep(f);}
    float mul(float a,float b) noexcept {volatile float f=keep(a)*keep(b);return keep(f);}
    float div(float a,float b) noexcept {volatile float f=keep(a)/keep(b);return keep(f);}
    float root(float a) noexcept {volatile float f=std::sqrt(keep(a));return keep(f);}
};
struct Engine {
    const CandidateCollectionInput48S& in;
    const CandidateCollectionWorkspace48S& work;
    CandidateCollectionBuffers48S b;
    CandidateCollectionResult48S result{};
    CollectionBoundsResult48S bounds{};
    Status status=Status::Complete;
    Ref listCursor=0,zone=0,tree=0;
    std::size_t zoneSteps=0;
    bool fail(Status s) noexcept {if(status==Status::Complete)status=s;return false;}
    bool ref(Ref r,std::size_t n) noexcept {return r&&static_cast<std::size_t>(r)<=n;}
    bool require(Ref r,std::size_t n) noexcept {return ref(r,n)||fail(Status::InvalidTopology);}
    bool numeric(const Math& m) noexcept {return m.valid||fail(Status::NonFinite);}
    Ref choose(Ref z,bool later=false) noexcept {
        if(!require(z,in.zoneCount))return 0;
        const auto& p=in.zones[z-1];
        Ref t=(in.methodMask&1u)?p.firstMethod:0;
        if(!t&&(in.methodMask&2u))t=p.secondMethod;
        if(!t){if(!later||in.laterMethodDiagnosticsEnabled)fail(Status::MissingMethod);return 0;}
        if(!require(t,in.treeCount))return 0;
        return t;
    }
    bool validBlockChain(Ref head) noexcept {
        std::size_t steps=0;
        for(Ref r=head;r;r=b.poolBlocks[r-1].next)
            if(++steps>in.poolBlockCount||!require(r,in.poolBlockCount))return fail(Status::InvalidTopology);
        return true;
    }
    bool validPool(Ref p) noexcept {
        if(!require(p,in.poolCount))return false;
        const auto& s=b.pools[p-1];const auto capacity=in.pools[p-1].perBlockCapacity;
        if(capacity<0||s.used20<0)return fail(Status::InvalidTopology);
        if(!validBlockChain(s.activeFirst14)||!validBlockChain(s.unusedFirst1C))return false;
        if(s.activeLast18&&!require(s.activeLast18,in.poolBlockCount))return false;
        Ref last=0;for(Ref r=s.activeFirst14;r;r=b.poolBlocks[r-1].next)last=r;
        if(last!=s.activeLast18)return fail(Status::InvalidTopology);
        // Already validated finite singly linked chains intersect iff they
        // share a nonnull tail. Check before close/reset can create a cycle;
        // this retains the full disjointness guard without an A*U scan.
        Ref unusedLast=0;
        for(Ref u=s.unusedFirst1C;u;u=b.poolBlocks[u-1].next)unusedLast=u;
        if(last&&last==unusedLast)return fail(Status::InvalidTopology);
        std::size_t steps=0;
        for(Ref e=s.freeEntry2C;e;e=b.poolEntries[e-1].next){
            if(++steps>in.poolEntryCount||!require(e,in.poolEntryCount))return fail(Status::InvalidTopology);
            for(Ref u=s.unusedFirst1C;u;u=b.poolBlocks[u-1].next){
                const auto& block=in.poolBlocks[u-1];
                if(e>=block.firstEntry&&static_cast<std::size_t>(e-block.firstEntry)<block.entryCount)return fail(Status::InvalidTopology);
            }
            if(s.activeLast18&&s.used20<capacity){
                const auto& block=in.poolBlocks[s.activeLast18-1];
                if(e>=block.firstEntry){const auto offset=static_cast<std::size_t>(e-block.firstEntry);
                    if(offset>=static_cast<std::size_t>(s.used20)&&offset<static_cast<std::size_t>(capacity))return fail(Status::InvalidTopology);
                }
            }
        }
        return true;
    }
    bool resetPool(Ref p) noexcept {
        if(!validPool(p))return false;
        auto& s=b.pools[p-1];
        if(s.activeFirst14){
            if(!require(s.activeFirst14,in.poolBlockCount))return false;
            if(s.unusedFirst1C&&s.activeLast18){
                if(!require(s.unusedFirst1C,in.poolBlockCount)||!require(s.activeLast18,in.poolBlockCount))return false;
                b.poolBlocks[s.activeLast18-1].next=s.unusedFirst1C;
            }
            s.unusedFirst1C=s.activeFirst14;
        }
        s.activeFirst14=0;s.activeLast18=0;s.bytesUsed34=0;
        s.used20=in.pools[p-1].perBlockCapacity;s.freeEntry2C=0;return true;
    }
    Ref acquire(Ref p) noexcept {
        if(!validPool(p))return 0;
        const auto capacity=in.pools[p-1].perBlockCapacity;auto& s=b.pools[p-1];
        if(capacity<0||s.used20<0){fail(Status::InvalidTopology);return 0;}
        Ref entry=0;
        if(s.used20<capacity){
            if(!require(s.activeLast18,in.poolBlockCount))return 0;
            const auto& chunk=in.poolBlocks[s.activeLast18-1];
            if(static_cast<std::size_t>(capacity)>chunk.entryCount||!require(chunk.firstEntry,in.poolEntryCount)||
               static_cast<std::size_t>(chunk.firstEntry-1)+static_cast<std::size_t>(capacity)>in.poolEntryCount){fail(Status::InvalidTopology);return 0;}
            entry=chunk.firstEntry+static_cast<Ref>(s.used20++);
        }else if(s.freeEntry2C){
            if(!require(s.freeEntry2C,in.poolEntryCount))return 0;
            entry=s.freeEntry2C;s.freeEntry2C=b.poolEntries[entry-1].next;
        }else if(s.unusedFirst1C){
            const Ref chunkRef=s.unusedFirst1C;
            if(!require(chunkRef,in.poolBlockCount))return 0;
            const auto& chunk=in.poolBlocks[chunkRef-1];
            if(!chunk.entryCount||static_cast<std::size_t>(capacity)>chunk.entryCount||!require(chunk.firstEntry,in.poolEntryCount)||
               static_cast<std::size_t>(chunk.firstEntry-1)+static_cast<std::size_t>(capacity)>in.poolEntryCount){fail(Status::InvalidTopology);return 0;}
            if(!s.activeFirst14){s.activeFirst14=chunkRef;s.activeLast18=chunkRef;}
            else{
                if(!require(s.activeLast18,in.poolBlockCount))return 0;
                b.poolBlocks[s.activeLast18-1].next=chunkRef;s.activeLast18=chunkRef;
            }
            s.unusedFirst1C=b.poolBlocks[chunkRef-1].next;
            b.poolBlocks[chunkRef-1].next=0;s.used20=1;entry=chunk.firstEntry;
        }else{fail(Status::AllocatorBoundary);return 0;}
        s.bytesUsed34+=8u;return entry;
    }
    bool push(std::size_t& depth,Ref node,std::int32_t x,std::int32_t z,std::int32_t half,bool all) noexcept {
        if(!require(node,in.nodeCount))return false;
        for(std::size_t j=0;j<depth;++j)if(work.stack[j].node==node)return fail(Status::InvalidTopology);
        if(depth>=work.stackCapacity)return fail(Status::InsufficientCapacity);
        work.stack[depth++]={node,x,z,half,0,all};return true;
    }
    bool selectNodes(Ref t) noexcept {
        const auto& def=in.trees[t-1];auto& s=b.trees[t-1];
        if(!def.root)return true;
        std::size_t depth=0;
        if(!push(depth,def.root,0,0,0x4000,false))return false;
        Ref head=0;
        while(depth){
            auto& f=work.stack[depth-1];const auto& n=in.nodes[f.node-1];
            if(f.nextChild<4&&(f.all||f.half>=def.quantum)){
                const unsigned child=f.nextChild++;
                const Ref c=n.children[child];if(!c)continue;
                // Source inclusive child boxes use half as child width. Root
                // origin0/width16384 and arithmetic are safe at this depth.
                const auto x=f.x+((child&2u)?f.half:0),z=f.z+((child&1u)?f.half:0);
                bool all=f.all;
                if(!all){
                    const auto maxx=x+f.half,maxz=z+f.half;
                    if(maxx<s.cache[0]||s.cache[2]<x||maxz<s.cache[1]||s.cache[3]<z)continue;
                    all=x>=s.cache[0]&&maxx<=s.cache[2]&&z>=s.cache[1]&&maxz<=s.cache[3];
                }
                if(!push(depth,c,x,z,f.half/2,all))return false;
                continue;
            }
            const Ref entry=acquire(def.pool);if(!entry)return false;
            // A malformed free/chunk configuration must not allocate the same
            // entry twice and commit a cycle merely because the caller stops
            // after the first returned object.
            for(Ref previous=head;previous;previous=b.poolEntries[previous-1].next)
                if(previous==entry)return fail(Status::InvalidTopology);
            b.poolEntries[entry-1]={head,f.node};head=entry;--depth;
        }
        s.currentHeadA8=head;return true;
    }
    bool open(Ref t) noexcept {
        if(!require(t,in.treeCount))return false;
        const auto& def=in.trees[t-1];auto& s=b.trees[t-1];
        if(def.method!=CollectionTree48S::Method::QuadTree)return fail(Status::UnsupportedMethod);
        Math m;
        float x0=m.sub(m.sub(bounds.center.v[0],bounds.half.v[0]),def.originX);
        float x1=m.sub(m.add(bounds.center.v[0],bounds.half.v[0]),def.originX);
        float z0=m.sub(m.sub(bounds.center.v[2],bounds.half.v[2]),def.originZ);
        float z1=m.sub(m.add(bounds.center.v[2],bounds.half.v[2]),def.originZ);
        if(!numeric(m))return false;
        if(x1<x0){const float tmp=x0;x0=x1;x1=tmp;}
        if(z1<z0){const float tmp=z0;z0=z1;z1=tmp;}
        if(x1<0.f)return true;
        if(!std::isfinite(def.extentX))return fail(Status::NonFinite);
        if(!(x0<=def.extentX)||z1<0.f)return true;
        if(!std::isfinite(def.extentZ))return fail(Status::NonFinite);
        if(!(z0<=def.extentZ))return true;
        const float coords[4]={m.mul(x0,def.scale),m.mul(z0,def.scale),m.mul(x1,def.scale),m.mul(z1,def.scale)};
        if(!numeric(m))return false;
        if(def.quantum<=0)return fail(Status::InvalidInput);
        std::int32_t rect[4]{};
        const auto quantum=static_cast<std::uint32_t>(def.quantum),mask=0u-quantum;
        for(unsigned j=0;j<4;++j){
            if(static_cast<double>(coords[j])<-2147483648.0||static_cast<double>(coords[j])>=2147483648.0)return fail(Status::NonFinite);
            const auto raw=static_cast<std::uint32_t>(static_cast<std::int32_t>(coords[j]))&mask;
            const std::uint32_t word=raw+(j>=2?quantum-1u:0u);std::memcpy(&rect[j],&word,sizeof(word));
        }
        if(s.flags16&1u){
            bool same=true;for(unsigned j=0;j<4;++j)same=same&&s.cache[j]==rect[j];
            if(same){s.currentHeadA8=s.savedHeadA4;s.slotAC=0;s.auxiliaryB4=0;++s.epoch;return true;}
            if(!resetPool(def.pool))return false;
            s.savedHeadA4=s.currentHeadA8=0;s.slotAC=0;s.auxiliaryB4=0;
            for(auto& x:s.cache)x=std::numeric_limits<std::int32_t>::min();
            s.flags16=static_cast<std::uint8_t>(s.flags16&~1u);
        }
        s.blocked898=0;s.flags16=static_cast<std::uint8_t>(s.flags16|1u);++s.epoch;
        for(unsigned j=0;j<4;++j)s.cache[j]=rect[j];
        result.activeTree=t;
        s.currentHeadA8=0;s.currentBlockB0=0;s.slotAC=0;s.auxiliaryB4=0;
        if(!selectNodes(t))return false;
        s.savedHeadA4=s.currentHeadA8;return true;
    }
    Ref treeNext(Ref t) noexcept {
        if(!require(t,in.treeCount))return 0;
        if(in.trees[t-1].method!=CollectionTree48S::Method::QuadTree){fail(Status::UnsupportedMethod);return 0;}
        auto& s=b.trees[t-1];
        if(!(s.flags16&1u)||s.blocked898)return 0;
        result.activeTree=t;
        std::size_t links=0,blocks=0;
        while(s.currentHeadA8){
            if(++links>in.poolEntryCount||!require(s.currentHeadA8,in.poolEntryCount)) {fail(Status::InvalidTopology);return 0;}
            const auto& link=b.poolEntries[s.currentHeadA8-1];
            if(!s.currentBlockB0){
                if(!require(link.node,in.nodeCount))return 0;
                s.currentBlockB0=in.nodes[link.node-1].firstBlock;
            }
            while(s.currentBlockB0){
                if(++blocks>in.itemBlockCount+in.poolEntryCount||!require(s.currentBlockB0,in.itemBlockCount)){fail(Status::InvalidTopology);return 0;}
                if(s.slotAC<0){fail(Status::InvalidTopology);return 0;}
                const auto& block=in.itemBlocks[s.currentBlockB0-1];
                while(s.slotAC<4){
                    const Ref o=block.objects[s.slotAC++];
                    if(!o)continue;
                    if(!require(o,in.objectCount))return 0;
                    auto& state=b.objects[o-1];
                    if(state.epoch==s.epoch)continue;
                    state.epoch=s.epoch;return o;
                }
                s.slotAC=0;s.currentBlockB0=block.next;
            }
            s.currentHeadA8=link.next;blocks=0;
        }
        return 0;
    }
    Ref next() noexcept {
        Ref object=treeNext(tree);if(object||status!=Status::Complete)return object;
        const auto& z=in.zones[zone-1];
        if(tree==z.firstMethod&&(in.methodMask&2u)&&z.secondMethod!=z.firstMethod)tree=z.secondMethod;
        else tree=0;
        if(!tree&&listCursor){
            if(++zoneSteps>in.zoneLinkCount){fail(Status::InvalidTopology);return 0;}
            listCursor=in.zoneLinks[listCursor-1].next;
            if(listCursor){
                if(!require(listCursor,in.zoneLinkCount))return 0;
                zone=in.zoneLinks[listCursor-1].zone;tree=choose(zone,true);if(status!=Status::Complete)return 0;
            }
        }
        if(!tree&&in.fallbackZone&&zone!=in.fallbackZone){zone=in.fallbackZone;tree=choose(zone,true);}
        if(!tree||status!=Status::Complete)return 0;
        if(!open(tree))return 0;
        return treeNext(tree); // Source deliberately does not loop past empty.
    }
    bool list(const CollectionWords48S& l,std::uint32_t other,bool category) noexcept {
        bool found=false;
        if(l.count>0){
            if(static_cast<std::size_t>(l.count)>l.capacity){fail(Status::InsufficientCapacity);return false;}
            for(std::int32_t j=0;j<l.count;++j){
                if(category?(l.values[j]&other)!=0:l.values[j]==other){
                    found=true;if(!category||l.mode==1)break;
                }
            }
        }
        if(category)return l.mode==1?!found:found;
        if(l.count<=0)return true;
        return !(found&&l.mode==1)&&!(!found&&l.mode==0);
    }
    bool filter(Ref o) noexcept {
        if(o==in.query)return false;
        const auto& query=in.objects[in.query-1];const auto& candidate=in.objects[o-1];
        const auto& q=query.filter;const auto& c=candidate.filter;
        if(!(b.objects[o-1].flagsF8&4u)||!(c.flags31&1u)||!(c.categoryA4&q.acceptMaskB8))return false;
        for(unsigned j=0;j<2;++j)
            if(!list(q.identities[j],candidate.identity,false)||!list(c.identities[j],query.identity,false))return false;
        return list(q.categories,c.categoryA4,true)&&list(c.categories,q.categoryA4,true);
    }
    bool sphere(Ref o) noexcept {
        const auto& q=b.objects[in.query-1];const auto& c=b.objects[o-1];Math m;
        const float radius=m.add(q.cachedRadius,c.cachedRadius);
        const float y=m.sub(q.cachedCenter.v[1],c.cachedCenter.v[1]);
        const float x=m.sub(q.cachedCenter.v[0],c.cachedCenter.v[0]);
        const float z=m.sub(q.cachedCenter.v[2],c.cachedCenter.v[2]);
        const float gap=m.sub(m.root(m.add(m.add(m.mul(x,x),m.mul(y,y)),m.mul(z,z))),radius);
        return numeric(m)&&gap<=0.f;
    }
    bool append(std::uint32_t identity) noexcept {
        if(result.count==std::numeric_limits<std::int32_t>::max()||
           static_cast<std::size_t>(result.count)>=b.identityCapacity)return fail(Status::InsufficientCapacity);
        b.identities[result.count++]=identity;return true;
    }
    bool group(Ref o) noexcept {
        const auto& object=in.objects[o-1];Math m;
        // XYZ are constructed at two different source points; initial coarse
        // check reads only X/Z even when sphere testing is skipped.
        const float x=m.mul(static_cast<float>(object.position[0]),in.groupPositionScale);
        const float z=m.mul(static_cast<float>(object.position[2]),in.groupPositionScale);
        if(!numeric(m)||!require(object.group,in.groupCount))return false;
        const auto& g=in.groups[object.group-1];
        if(!in.skipSphere){
            const float radius=m.add(g.base18C,m.div(m.mul(static_cast<float>(object.scale11),m.sub(g.other194,g.base18C)),128.f));
            const auto& q=b.objects[in.query-1];
            const float dx=m.sub(q.cachedCenter.v[0],x),dz=m.sub(q.cachedCenter.v[2],z);
            const float gap=m.sub(m.root(m.add(m.mul(dx,dx),m.mul(dz,dz))),m.add(q.cachedRadius,radius));
            if(!numeric(m))return false;
            if(!(gap<=0.f))return true;
        }
        if(g.memberCount<=0)return true;
        if(static_cast<std::size_t>(g.memberCount)>g.memberCapacity)return fail(Status::InsufficientCapacity);
        for(std::int32_t j=0;j<g.memberCount;++j){
            const Ref member=g.members[j];if(!require(member,in.objectCount))return false;
            const auto& def=in.objects[member-1];
            // The query has already supplied one real bounds shape. Keep that
            // identity authoritative if the same object is also a group member.
            const auto shapeTag=member==in.query?in.bounds.shapeTag:def.shapeTag;
            if(member!=in.query&&!def.hasShape)return fail(Status::UnsupportedMemberShape);
            const float y=m.mul(static_cast<float>(object.position[1]),in.groupPositionScale);
            const float scale18C=m.div(m.add(g.base18C,m.div(m.mul(static_cast<float>(object.scale11),m.sub(g.other194,g.base18C)),128.f)),g.base18C);
            const float scale180=m.div(m.add(g.base180,m.div(m.mul(static_cast<float>(object.scale10),m.sub(g.base180,g.other184)),128.f)),g.base180);
            if(!numeric(m))return false;
            auto& s=b.objects[member-1];
            if(shapeTag==0){
                if(!def.shape0Ref)return fail(Status::UnsupportedMemberShape);
                if(!require(def.shape0Ref,in.shape0Count))return false;
                const Vector3_48C translation{{x,y,z}};
                GroupShapeState48T shape{};GroupShapeSphere48T sphere{};
                const auto shapeStatus=setGroupShape0Bounded48T(in.shape0Sources[def.shape0Ref-1],translation,
                    b.shape0States[def.shape0Ref-1],shape,sphere);
                if(shapeStatus==GroupShapeStatus48T::MissingGeometry)return fail(Status::UnsupportedMemberShape);
                if(shapeStatus==GroupShapeStatus48T::NonFinite)return fail(Status::NonFinite);
                if(shapeStatus!=GroupShapeStatus48T::Complete)return fail(Status::InvalidInput);
                b.shape0States[def.shape0Ref-1]=shape;s.cachedCenter=sphere.center;s.cachedRadius=sphere.radius;
            }
            if(shapeTag==4||shapeTag==5){s.memberTranslation={{x,y,z}};s.memberScale18C=scale18C;s.memberScale180=scale180;}
            result.currentIdentity=def.identity;s.flagsF8=static_cast<std::uint8_t>(s.flagsF8|0x10u);
            if(!append(object.identity))return false;
        }
        return true;
    }
    Status run() noexcept {
        if(!require(in.query,in.objectCount))return status;
        result.currentIdentity=in.initialCurrentIdentity;result.activeTree=in.initialActiveTree;
        auto& query=b.objects[in.query-1];
        if(!(query.flagsF8&2u)){query.flagsF8=static_cast<std::uint8_t>(query.flagsF8&~0x20u);return status;}
        query.flagsF8=static_cast<std::uint8_t>(query.flagsF8|0x10u);
        auto input=in.bounds;input.cachedCenter=query.cachedCenter;input.cachedRadius=query.cachedRadius;
        const auto bs=prepareCollectionBounds48S(input,bounds);
        switch(bs){
        case CollectionBoundsStatus48S::Complete:break;
        case CollectionBoundsStatus48S::UnsupportedBounds:return Status::UnsupportedBounds;
        case CollectionBoundsStatus48S::NonFinite:return Status::NonFinite;
        case CollectionBoundsStatus48S::InsufficientCapacity:return Status::InsufficientCapacity;
        default:return Status::InvalidInput;
        }
        query.cachedCenter=bounds.cachedCenter;query.cachedRadius=bounds.cachedRadius;
        listCursor=in.zoneHead;if(!require(listCursor,in.zoneLinkCount))return status;
        zone=in.zoneLinks[listCursor-1].zone;tree=choose(zone);if(!tree||!open(tree))return status;
        while(status==Status::Complete){
            const Ref o=next();if(!o||status!=Status::Complete)break;
            if(!(result.count<in.nominalCapacity))break;
            const auto& object=in.objects[o-1];
            if(object.typeTag==in.directTypeTag){
                result.currentIdentity=object.identity;b.objects[o-1].flagsF8=static_cast<std::uint8_t>(b.objects[o-1].flagsF8|0x10u);
                if(!in.skipSphere&&!sphere(o))continue;
                if(filter(o)&&!append(object.identity))break;
            }else if(!in.ignoreGroups&&object.typeTag==in.groupTypeTag){if(!group(o))break;}
        }
        return status;
    }
};
}
Status collectCandidatesBounded48S(const CandidateCollectionInput48S& in,
    const CandidateCollectionState48S& initial,const CandidateCollectionWorkspace48S& work,
    const CandidateCollectionBuffers48S& destination,CandidateCollectionResult48S& output) noexcept {
    Guard guard;
    if(!writes(guard,work.buffers)||!writes(guard,destination)||!guard.write(work.stack,work.stackCapacity)||!guard.write(&output,1))return Status::InvalidInput;
    if(!guard.read(&in,1)||!guard.read(&initial,1)||!guard.read(&work,1)||!guard.read(&destination,1)||
       !guard.read(in.objects,in.objectCount)||!guard.read(in.groups,in.groupCount)||!guard.read(in.nodes,in.nodeCount)||
       !guard.read(in.itemBlocks,in.itemBlockCount)||!guard.read(in.trees,in.treeCount)||!guard.read(in.zones,in.zoneCount)||
       !guard.read(in.zoneLinks,in.zoneLinkCount)||!guard.read(in.pools,in.poolCount)||!guard.read(in.poolBlocks,in.poolBlockCount)||
       !guard.read(initial.objects,initial.objectCapacity)||!guard.read(initial.trees,initial.treeCapacity)||
       !guard.read(initial.pools,initial.poolCapacity)||!guard.read(initial.poolBlocks,initial.poolBlockCapacity)||
       !guard.read(initial.poolEntries,initial.poolEntryCapacity)||
       !guard.read(in.bounds.endpoints,in.bounds.endpointCount)||!guard.read(in.bounds.endpointScenes,in.bounds.endpointSceneCount)||
       !guard.read(in.bounds.scenes,in.bounds.sceneCount)||!guard.read(in.shape0Sources,in.shape0Count)||
       !guard.read(initial.shape0States,initial.shape0Capacity)||
       !guard.read(work.identityOrder,work.identityOrderCount)||!guard.read(work.blockOrder,work.blockOrderCount))return Status::InvalidInput;
    const std::size_t counts[]={in.objectCount,in.groupCount,in.nodeCount,in.itemBlockCount,in.treeCount,in.zoneCount,in.zoneLinkCount,in.poolCount,in.poolBlockCount,in.poolEntryCount,in.shape0Count};
    for(auto n:counts)if(n>std::numeric_limits<Ref>::max())return Status::InvalidInput;
    const bool identityOrder=work.identityOrder||work.identityOrderCount;
    const bool blockOrder=work.blockOrder||work.blockOrderCount;
    if((identityOrder&&work.identityOrderCount!=in.objectCount)||
       (blockOrder&&work.blockOrderCount!=in.poolBlockCount))return Status::InvalidInput;
    for(std::size_t j=0;j<in.objectCount;++j){
        if(!in.objects[j].identity)return Status::InvalidInput;
        const auto& f=in.objects[j].filter;
        if(!guard.read(f.identities[0].values,f.identities[0].capacity)||!guard.read(f.identities[1].values,f.identities[1].capacity)||
           !guard.read(f.categories.values,f.categories.capacity))return Status::InvalidInput;
        if(!identityOrder)for(std::size_t k=0;k<j;++k)
            if(in.objects[j].identity==in.objects[k].identity)return Status::InvalidInput;
    }
    if(identityOrder){
        std::uint32_t previous=0;
        for(std::size_t j=0;j<work.identityOrderCount;++j){
            const Ref r=work.identityOrder[j];
            if(!r||static_cast<std::size_t>(r)>in.objectCount)return Status::InvalidInput;
            const auto identity=in.objects[r-1].identity;
            if(identity<=previous)return Status::InvalidInput;
            previous=identity;
        }
    }
    for(std::size_t j=0;j<in.groupCount;++j)if(!guard.read(in.groups[j].members,in.groups[j].memberCapacity))return Status::InvalidInput;
    for(std::size_t j=0;j<in.shape0Count;++j)if(!guard.read(in.shape0Sources[j].geometry,in.shape0Sources[j].geometry?1:0))return Status::InvalidInput;
    for(std::size_t j=0;j<in.poolBlockCount;++j){
        const auto& block=in.poolBlocks[j];
        if(block.entryCount&&(!block.firstEntry||static_cast<std::size_t>(block.firstEntry)>in.poolEntryCount||
           block.entryCount>in.poolEntryCount-static_cast<std::size_t>(block.firstEntry-1)))return Status::InvalidInput;
        if(block.entryCount&&!blockOrder){
            const auto begin=static_cast<std::size_t>(block.firstEntry-1),end=begin+block.entryCount;
            for(std::size_t k=0;k<j;++k){const auto& other=in.poolBlocks[k];if(!other.entryCount)continue;
                const auto otherBegin=static_cast<std::size_t>(other.firstEntry-1),otherEnd=otherBegin+other.entryCount;
                if(begin<otherEnd&&otherBegin<end)return Status::InvalidInput;
            }
        }
    }
    if(blockOrder){
        Ref previousStart=0,previousRef=0;std::size_t previousEnd=0;
        for(std::size_t j=0;j<work.blockOrderCount;++j){
            const Ref r=work.blockOrder[j];
            if(!r||static_cast<std::size_t>(r)>in.poolBlockCount)return Status::InvalidInput;
            const auto& block=in.poolBlocks[r-1];
            const Ref start=block.entryCount?block.firstEntry:0;
            if(start<previousStart||(start==previousStart&&r<=previousRef))return Status::InvalidInput;
            previousStart=start;previousRef=r;
            if(block.entryCount){
                const auto begin=static_cast<std::size_t>(block.firstEntry-1);
                if(begin<previousEnd)return Status::InvalidInput;
                previousEnd=begin+block.entryCount;
            }
        }
    }
    if(!capacities(in,work.buffers)||!capacities(in,destination)||initial.objectCapacity<in.objectCount||
       initial.treeCapacity<in.treeCount||initial.poolCapacity<in.poolCount||initial.poolBlockCapacity<in.poolBlockCount||
       initial.poolEntryCapacity<in.poolEntryCount||initial.shape0Capacity<in.shape0Count)return Status::InsufficientCapacity;
    const auto& b=work.buffers;
    copy(b.objects,initial.objects,in.objectCount);copy(b.trees,initial.trees,in.treeCount);copy(b.pools,initial.pools,in.poolCount);
    copy(b.poolBlocks,initial.poolBlocks,in.poolBlockCount);copy(b.poolEntries,initial.poolEntries,in.poolEntryCount);
    copy(b.shape0States,initial.shape0States,in.shape0Count);
    Engine engine{in,work,b};
    const auto status=engine.run();if(status!=Status::Complete)return status;
    if(static_cast<std::size_t>(engine.result.count)>destination.identityCapacity)return Status::InsufficientCapacity;
    copy(destination.objects,b.objects,in.objectCount);copy(destination.trees,b.trees,in.treeCount);copy(destination.pools,b.pools,in.poolCount);
    copy(destination.poolBlocks,b.poolBlocks,in.poolBlockCount);copy(destination.poolEntries,b.poolEntries,in.poolEntryCount);
    copy(destination.identities,b.identities,static_cast<std::size_t>(engine.result.count));
    copy(destination.shape0States,b.shape0States,in.shape0Count);
    output=engine.result;return Status::Complete;
}
}
