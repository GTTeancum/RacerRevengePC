#include "collision_mesh_admission50cc.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

namespace rr::game {
namespace {
struct Key {
    const void* mesh=nullptr;
    const void* nodes=nullptr;std::size_t nodeCount=0;
    const void* faces=nullptr;std::size_t faceCount=0;
    const void* vertices=nullptr;std::size_t vertexCount=0;
    const void* normals=nullptr;std::size_t normalCount=0;
    const float* radii=nullptr;std::size_t radiusCount=0;
    std::uint32_t reverseMode=0;
};
bool same(const Key& a,const Key& b) noexcept {
    return a.mesh==b.mesh&&a.nodes==b.nodes&&a.nodeCount==b.nodeCount&&a.faces==b.faces&&a.faceCount==b.faceCount&&
        a.vertices==b.vertices&&a.vertexCount==b.vertexCount&&a.normals==b.normals&&a.normalCount==b.normalCount&&
        a.radii==b.radii&&a.radiusCount==b.radiusCount&&a.reverseMode==b.reverseMode;
}
Key keyOf(const assets::CollisionMesh48I& mesh,const float* radii,std::size_t radiusCount) noexcept {
    Key k{};
    k.mesh=&mesh;
    k.nodes=mesh.nodes.data();k.nodeCount=mesh.nodes.size();
    k.faces=mesh.faces.data();k.faceCount=mesh.faces.size();
    k.vertices=mesh.vertices.data();k.vertexCount=mesh.vertices.size();
    k.normals=mesh.vertex_normals.data();k.normalCount=mesh.vertex_normals.size();
    k.radii=radii;k.radiusCount=radiusCount;k.reverseMode=mesh.reverse_mode;
    return k;
}
std::uint64_t mix(std::uint64_t h,std::uint64_t v) noexcept {
    h^=v+0x9e3779b97f4a7c15ull+(h<<6)+(h>>2);
    return h;
}
std::uint64_t hashOf(const Key& k) noexcept {
    std::uint64_t h=0;
    h=mix(h,reinterpret_cast<std::uintptr_t>(k.mesh));
    h=mix(h,reinterpret_cast<std::uintptr_t>(k.nodes));
    h=mix(h,reinterpret_cast<std::uintptr_t>(k.faces));
    h=mix(h,reinterpret_cast<std::uintptr_t>(k.vertices));
    h=mix(h,reinterpret_cast<std::uintptr_t>(k.radii));
    h^=h>>33;h*=0xff51afd7ed558ccdull;h^=h>>33;
    return h;
}
bool finite3(const std::array<float,3>& a) noexcept {
    for(float f:a)if(!std::isfinite(f))return false;
    return true;
}

// Sizes are the only definition of this table's layout. 8192 slots holding at
// most 4096 entries keeps linear probing short; GB, the largest course, has
// about 640 candidate meshes plus compound parts.
constexpr std::size_t slotCount=8192;     // power of two
constexpr std::size_t entryCapacity=4096; // inserts beyond this are refused
static_assert((slotCount&(slotCount-1))==0&&entryCapacity<=slotCount/2,"admission table layout");
constexpr unsigned disagreementLineCap=20;

struct Entry {
    Key key{};
    std::uint32_t generation=0;
    bool used=false;
};
// Host memory only (static storage of this translation unit).
struct Table {
    Entry slots[slotCount]{};
    std::size_t entries=0;
    std::uint32_t generation=0;
    bool open=false,checkMode=false;
    std::thread::id owner{};
    CollisionAdmissionStats50CC stats{};
    unsigned disagreementLines=0;
};
Table table;

void clear(Table& t) noexcept {
    for(auto& e:t.slots)e=Entry{};
    t.entries=0;
}
void nextGeneration(Table& t) noexcept {
    ++t.generation;
    if(t.generation==0)t.generation=1; // 0 is never a live generation
    clear(t);
}
bool ownerThread(const Table& t) noexcept {
    return t.open&&std::this_thread::get_id()==t.owner;
}
// Returns the matching slot index, or slotCount when absent. firstFree receives
// the first unused slot met on the probe path (slotCount when none).
std::size_t find(const Table& t,const Key& k,std::size_t& firstFree) noexcept {
    firstFree=slotCount;
    std::size_t index=static_cast<std::size_t>(hashOf(k))&(slotCount-1);
    for(std::size_t probe=0;probe<slotCount;++probe,index=(index+1)&(slotCount-1)){
        const auto& e=t.slots[index];
        if(!e.used){firstFree=index;return slotCount;}
        if(e.generation==t.generation&&same(e.key,k))return index;
    }
    return slotCount;
}
void insert(Table& t,const Key& k,std::size_t firstFree) noexcept {
    // Bounds checks before the write: capacity, then slot index and emptiness.
    if(t.entries>=entryCapacity||firstFree>=slotCount||t.slots[firstFree].used){++t.stats.tableFull;return;}
    auto& e=t.slots[firstFree];
    e.key=k;e.generation=t.generation;e.used=true;
    ++t.entries;++t.stats.inserts;
}
void report(const Table& t,const char* when) noexcept {
    const auto& s=t.stats;
    if(!s.checks)return;
    std::fprintf(stderr,"collision_admission at=%s generation=%u checks=%llu cached_hits=%llu full_checks=%llu"
        " inserts=%llu entries=%llu table_full=%llu disagreements=%llu check_mode=%d\n",
        when,t.generation,static_cast<unsigned long long>(s.checks),static_cast<unsigned long long>(s.cachedHits),
        static_cast<unsigned long long>(s.fullChecks),static_cast<unsigned long long>(s.inserts),
        static_cast<unsigned long long>(t.entries),static_cast<unsigned long long>(s.tableFull),
        static_cast<unsigned long long>(s.disagreements),t.checkMode?1:0);
}
} // namespace

bool collisionMeshContentsValid50CC(const assets::CollisionMesh48I& mesh,const float* radii) noexcept {
    // Same loops, order and conditions as the former validateMesh tail.
    for(std::size_t j=0;j<mesh.nodes.size();++j){
        const auto& n=mesh.nodes[j];
        if(!std::isfinite(radii[j])||radii[j]<0.f||!std::isfinite(n.volume)||
           !finite3(n.center)||!finite3(n.half))return false;
        for(unsigned k=0;k<3;++k)if(n.half[k]<0.f)return false;
        for(auto t:n.children){const auto index=t&~assets::collision_mesh_leaf48i;
            if((t&assets::collision_mesh_leaf48i)?index>=mesh.faces.size():index>=mesh.nodes.size())return false;
        }
    }
    for(const auto& vertex:mesh.vertices)if(!finite3(vertex))return false;
    for(const auto& face:mesh.faces)for(auto index:face.vertices)if(index>=mesh.vertices.size())return false;
    return true;
}

bool collisionMeshContentsAdmitted50CC(const assets::CollisionMesh48I& mesh,
    const float* radii,std::size_t radiusCount) noexcept {
    auto& t=table;
    // Outside a session scope, or on any other thread: full check, no table use.
    if(!ownerThread(t))return collisionMeshContentsValid50CC(mesh,radii);
    ++t.stats.checks;
    const auto k=keyOf(mesh,radii,radiusCount);
    std::size_t firstFree=slotCount;
    const auto found=find(t,k,firstFree);
    if(found<slotCount){
        ++t.stats.cachedHits;
        if(!t.checkMode)return true;
        ++t.stats.fullChecks;
        const bool full=collisionMeshContentsValid50CC(mesh,radii);
        if(!full){
            ++t.stats.disagreements;
            if(t.disagreementLines<disagreementLineCap){++t.disagreementLines;
                std::fprintf(stderr,"collision_admission_disagreement generation=%u mesh=%p nodes=%zu faces=%zu vertices=%zu\n",
                    t.generation,static_cast<const void*>(&mesh),mesh.nodes.size(),mesh.faces.size(),mesh.vertices.size());}
        }
        return full; // the full answer always wins in check mode
    }
    ++t.stats.fullChecks;
    const bool full=collisionMeshContentsValid50CC(mesh,radii);
    if(full)insert(t,k,firstFree); // only passing meshes are ever recorded
    return full;
}

void openCollisionAdmissionScope50CC() noexcept {
    auto& t=table;
    if(t.open)report(t,"reopen");
    nextGeneration(t);
    t.stats=CollisionAdmissionStats50CC{};t.disagreementLines=0;
    const char* v=std::getenv("RR_COLLISION_ADMISSION_CHECK");
    t.checkMode=v&&v[0]=='1'&&!v[1];
    t.owner=std::this_thread::get_id();
    t.open=true;
}
void closeCollisionAdmissionScope50CC() noexcept {
    auto& t=table;
    if(!t.open)return;
    if(std::this_thread::get_id()!=t.owner)return; // only the opening thread closes
    report(t,"close");
    t.open=false;t.owner=std::thread::id{};
    nextGeneration(t);
}
void reportCollisionAdmission50CC(const char* when) noexcept {
    if(ownerThread(table))report(table,when?when:"now");
}
CollisionAdmissionStats50CC collisionAdmissionStats50CC() noexcept {
    auto s=table.stats;s.entries=table.entries;s.generation=table.generation;return s;
}

} // namespace rr::game
