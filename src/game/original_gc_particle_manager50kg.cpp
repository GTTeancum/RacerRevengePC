#include "original_gc_particle_manager50kg.h"
#include "../assets/original_particle_emitters50pe.h"
#include <iostream>
#include "original_particle_fog_velocity50la.h"
#include "original_particle_fog_lifetime50la.h"
#include "import_world48f.h"
#include "original_ui_pages50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>
#include <limits>
#include <new>
#include <string>
#include <type_traits>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalGcParticleStatus50KG;
using Bytes=std::vector<std::uint8_t>;
using Section=assets::OriginalUiDatSection50KG;
using Document=std::vector<Section>;
// Shared bounded source scalar primitives. Volatile stores prevent FMA
// contraction/reassociation across the original single-precision operations.
float add(float a,float b) noexcept{volatile float r=a+b;return r;}
float sub(float a,float b) noexcept{volatile float r=a-b;return r;}
float mul(float a,float b) noexcept{volatile float r=a*b;return r;}
float divide(float a,float b) noexcept{volatile float r=a/b;return r;}
bool scalar(float v) noexcept{return std::isfinite(v)&&std::fpclassify(v)!=FP_SUBNORMAL;}
// Reused source-independent SHA256 from original_follow_cam.cpp.
// cancellation-only wrapper removed. This authenticates data, not guest code.
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight(w[i-15],7) ^ rotateRight(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight(w[i-2],17) ^ rotateRight(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight(e,6) ^ rotateRight(e,11) ^ rotateRight(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight(a,2) ^ rotateRight(a,13) ^ rotateRight(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
bool hashMatches(const std::uint8_t* data, std::size_t size,
    const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
// Admission is only the authenticated original ASCII PRT/EMT grammar. These
// scalar readers are shared below Main/Options identity owners; no UI scene
// is impersonated or widened to accept GC assets.
bool readText(const Bytes& bytes,Document& out) {
    std::size_t at=0;
    while(at<bytes.size()) {
        const auto start=at;while(at<bytes.size()&&bytes[at]!='\n')++at;
        std::string line;line.reserve(at-start);
        bool quoted=false;int brackets=0;
        for(auto i=start;i<at;++i) {
            const auto c=bytes[i];
            if(c=='"')quoted=!quoted;
            else if(c=='[')++brackets;
            else if(c==']')--brackets;
            if(brackets<0||brackets>16)return false;
            if((c==' '||c=='\t'||c=='\r')&&!quoted&&brackets==0)continue;
            if(c<32||c>126)return false;line.push_back(char(c));
        }
        if(at<bytes.size())++at;
        if(line.empty()||line[0]==';')continue;
        if(line[0]=='[') {
            const auto close=line.find(']');
            if(close==std::string::npos||close==1||
               (close+1<line.size()&&line[close+1]!=';'))return false;
            const auto name=line.substr(1,close-1);
            for(const auto& section:out)if(section.name==name)return false;
            out.push_back({name,start,{}});continue;
        }
        const auto equal=line.find('=');
        if(out.empty()||equal==std::string::npos||equal==0)return false;
        auto& section=out.back();const auto key=line.substr(0,equal);
        if(section.find(key))return false;
        section.values.push_back({key,line.substr(equal+1),start,start+equal+1,equal+1<line.size()});
    }
    return !out.empty();
}
const Section* findSection(const Document& doc,const std::string& name) noexcept {
    for(const auto& s:doc)if(s.name==name)return &s;return nullptr;
}
bool number(const Section* section,const char* key,float& value) noexcept {
    if(!section||!section->find(key))return false;
    assets::OriginalUiPageDiagnostic50KG d{};
    return assets::originalUiNumber50KG(*section,key,0,value,d,0);
}
bool integer(const Section* section,const char* key,std::int32_t& value) noexcept {
    if(!section||!section->find(key))return false;
    assets::OriginalUiPageDiagnostic50KG d{};
    return assets::originalUiInteger50KG(*section,key,0,value,d,0);
}
bool boolean(const Section* section,const char* key,bool& value) noexcept {
    if(!section||!section->find(key))return false;
    assets::OriginalUiPageDiagnostic50KG d{};
    return assets::originalUiBoolean50KG(*section,key,false,value,d,0);
}
bool triple(const Section* section,const char* key,std::array<float,3>& value) noexcept {
    if(!section||!section->find(key))return false;
    assets::OriginalUiPageDiagnostic50KG d{};
    return assets::originalUiTriple50KG(*section,key,{},value,d,0);
}
bool textIs(const Section* section,const char* key,const char* value) noexcept {
    const auto* f=section?section->find(key):nullptr;return f&&f->text==value;
}
struct FogDefinition {
    std::array<float,3> minRange{},maxRange{},velocity{};
    float life=0,size=0,growth=0,rate=0,birthFraction=0,birthAge=0;
    float distance=0,distanceSquared=0,inverseDistanceSquared=0,count=0,period=0,magnitude=0;
    std::uint32_t flags=0;
    std::int32_t group=0;float framesPerSet=0;
    bool mirrorX=false,mirrorY=false;
};
bool readFog(const Document& doc,FogDefinition& f) noexcept {
    const auto* s=findSection(doc,"General Info");
    if(!integer(s,"ParticleGroup",f.group)||!number(s,"ParticleLife",f.life)||
       !number(s,"InitialSize",f.size)||!number(s,"Growth",f.growth)||
       !number(s,"EmitRate",f.rate)||!number(s,"AgeAtBirth",f.birthFraction)||
       !triple(s,"Velocity",f.velocity)||!triple(s,"MinRange",f.minRange)||
       !triple(s,"MaxRange",f.maxRange)||!number(s,"DistanceToFade",f.distance)||
       !number(s,"ParticlesInVolume",f.count)||!number(s,"OscillationSpeed",f.period)||
       !number(s,"OscillationMagnitude",f.magnitude))return false;
    bool fades=false,additive=false,hasVelocity=false,grows=false,nonlinear=false,glows=false,gravity=false,lit=false;
    s=findSection(doc,"Properties");
    if(!boolean(s,"Fades",fades)||!boolean(s,"Additive",additive)||
       !boolean(s,"HasVelocity",hasVelocity)||!boolean(s,"Grows",grows)||
       !boolean(s,"FadeNonLinear",nonlinear)||!boolean(s,"Glows",glows)||
       !boolean(s,"Gravity",gravity)||!boolean(s,"Lit",lit)||
       !boolean(s,"RandomMirrorAboutX",f.mirrorX)||!boolean(s,"RandomMirrorAboutY",f.mirrorY)||
       !number(findSection(doc,"Animation"),"FramesPerSet",f.framesPerSet))return false;
    f.flags=0x80u|(additive?1u:0u)|(fades?2u:0u)|(hasVelocity?4u:0u)|
       (gravity?0x20u:0u)|(nonlinear?0x1000u:0u)|(glows?0x2000u:0u)|
       (!lit?0x800u:0u)|(f.framesPerSet>1?0x400u:0u);
    f.birthAge=mul(f.life,f.birthFraction);
    f.distanceSquared=mul(f.distance,f.distance);f.inverseDistanceSquared=divide(1,f.distanceSquared);
    return f.group==0&&f.life==100&&f.size==80&&f.growth==0&&f.rate==0&&
        f.birthFraction==0.6f&&f.distance==200&&f.count==80&&f.period==10&&f.magnitude==0.02f&&
        f.minRange==std::array<float,3>{{-300,0,-300}}&&f.maxRange==std::array<float,3>{{300,110,300}}&&
        f.velocity==std::array<float,3>{}&&f.flags==0x80u&&f.framesPerSet==0&&f.mirrorX&&f.mirrorY;
}
}
bool readOriginalGcParticleText50KG(const Bytes& bytes,std::size_t expectedBytes,
    const char* expectedSha256,Document& output) noexcept {
    if(!expectedSha256||std::strlen(expectedSha256)!=64||bytes.size()!=expectedBytes||
       expectedBytes>assets::originalUiDatByteLimit50KG||
       !hashMatches(bytes.data(),bytes.size(),expectedSha256))return false;
    try {Document result;if(!readText(bytes,result))return false;output=std::move(result);return true;}
    catch(...){return false;}
}
namespace {
// Allocator copies borrow this stable subobject of the nonmoving Storage.
// It is declared before the vector and therefore destroyed after the vector.
// No callback, allocation, shared_ptr or independent control block is needed.
struct EmitterAllocation50KG {
    std::size_t live=0,peak=0,quota=0;
    bool bound=false,attempted=false,terminal=false;
    Status failure=Status::complete;
    void reject(Status status) noexcept {
        if(attempted){terminal=true;if(failure==Status::complete)failure=status;}
    }
};
template<class T> struct EmitterAllocator50KG {
    using value_type=T;
    using is_always_equal=std::false_type;
    using propagate_on_container_move_assignment=std::false_type;
    using propagate_on_container_swap=std::false_type;
    EmitterAllocation50KG* state=nullptr;
    explicit EmitterAllocator50KG(EmitterAllocation50KG* s=nullptr) noexcept:state(s){}
    template<class U> EmitterAllocator50KG(const EmitterAllocator50KG<U>& other) noexcept:state(other.state){}
    T* allocate(std::size_t n) {
        if(!state)throw std::bad_alloc{};
        if(n>std::numeric_limits<std::size_t>::max()/sizeof(T)){
            state->reject(Status::capacity);throw std::bad_alloc{};
        }
        const auto bytes=n*sizeof(T);
        if(state->terminal||bytes>std::numeric_limits<std::size_t>::max()-state->live||
           (state->bound&&(state->live>state->quota||bytes>state->quota-state->live))){
            state->reject(Status::capacity);throw std::bad_alloc{};
        }
        // Keep the old vector allocation charged. A reserve/push_back request
        // may exceed the requested capacity; it must fit WITHOUT guessing the
        // implementation's growth factor. Reject before the upstream call.
        state->live+=bytes;state->peak=std::max(state->peak,state->live);
        try{return std::allocator<T>{}.allocate(n);}
        catch(...){state->live-=bytes;state->reject(Status::allocation);throw;}
    }
    void deallocate(T* pointer,std::size_t n) noexcept {
        if(!state||n>std::numeric_limits<std::size_t>::max()/sizeof(T)||
           n*sizeof(T)>state->live)std::terminate();
        std::allocator<T>{}.deallocate(pointer,n);
        state->live-=n*sizeof(T);
    }
    template<class U> bool operator==(const EmitterAllocator50KG<U>& other) const noexcept{return state==other.state;}
    template<class U> bool operator!=(const EmitterAllocator50KG<U>& other) const noexcept{return !(*this==other);}
};
using EmitterVector50KG=std::vector<OriginalGcParticleEmitterState50KG,
    EmitterAllocator50KG<OriginalGcParticleEmitterState50KG>>;
static_assert(std::is_nothrow_copy_constructible<OriginalGcParticleEmitterState50KG>::value&&
    std::is_nothrow_move_constructible<OriginalGcParticleEmitterState50KG>::value,
    "registry reservation does not add a throwing element migration");
}
// Exclusive pending backing inside the SAME numeric authority. No native
// contiguous Storage is created in this mode, and no record address is moved.
struct OriginalGcParticleManager50KG::SourcePool {
    std::unique_ptr<OriginalGcParticle50KG*[]> pointers;
    std::unique_ptr<OriginalGcParticle50KG> unpublishedRecord;
    std::size_t allocationAttempts=0,published=0;
    bool pointersPublished=false,loopAttempted=false,loopReturned=false;
    // Every nonNULL published/unpublished record has ONLY position00 and
    // velocity10 source-known, initially two VF0 vectors. The private Fog
    //1606FC/16088C prefixes may replace position00/velocity10. The two bounded
    //Fog occurrences below separately know their completed scalar/owner stores; no other
    //host-initialized lanes are interpreted as source data.
    static constexpr std::size_t knownRecordPrefixBytes=0x20;
    struct FogFields1608AC {
        const OriginalGcParticle50KG* record=nullptr;
        const void* emitter=nullptr; //Native occurrence binding, not source48.
        const void* owner48=nullptr; //Actual source lane, known only at count>=3.
        std::uint8_t storedCount=0; //38,3C,48,30,2C,28 in exact source order.
    } fogFields;
    FogFields1608AC successorFogFields; //Only inactive slot1; independent source48 knownness and six stores.
    struct FogTail1608E0 {
        std::uint8_t completedOps=0;
        //Only completed operations populate these native receipts. The same
        //fogFields binding supplies record/emitter/authoritative source48.
        std::array<std::uint32_t,7> pcs{},values{};
    } fogTail;
    FogTail1608E0 successorFogTail; //Seven actual slot1 operations, independent of slot0.
    ~SourcePool()noexcept {
        for(std::size_t i=0;i<published;++i)delete pointers[i];
        //unique_ptr retains the attempted record when138 wasNULL at160F48.
        //No source particle fields/unknown pointer-array slots are observed.
    }
};
bool OriginalGcParticleManager50KG::allocateSourcePointers160F08(std::uint32_t& pc)noexcept {
    pc=0x160f08;
    if(poolMode_!=PoolMode::empty||storage_||sourcePool_)return false;
    sourcePool_.reset(new(std::nothrow) SourcePool);
    if(!sourcePool_)return false; //Native metadata capacity, before source allocation.
    poolMode_=PoolMode::sourcePending;
    sourcePool_->pointers.reset(new(std::nothrow) OriginalGcParticle50KG*[originalGcParticleCapacity50KG]);
    sourcePool_->pointersPublished=true;pc=0x160f10;return true; //NULL is a real publication.
}
const void* OriginalGcParticleManager50KG::sourcePointerPublication138()const noexcept {
    return poolMode_==PoolMode::sourcePending&&sourcePool_&&sourcePool_->pointersPublished?
        sourcePool_->pointers.get():nullptr;
}
bool OriginalGcParticleManager50KG::constructSourceRecords160F1C(std::uint32_t& pc)noexcept {
    pc=0x160f1c;
    if(poolMode_!=PoolMode::sourcePending||storage_||!sourcePool_||
       !sourcePool_->pointersPublished||sourcePool_->loopAttempted)return false;
    auto& pool=*sourcePool_;pool.loopAttempted=true;
    while(pool.allocationAttempts<originalGcParticleCapacity50KG){
        std::unique_ptr<OriginalGcParticle50KG> record(new(std::nothrow) OriginalGcParticle50KG);
        ++pool.allocationAttempts;
        if(record){
            record->position={{0,0,0}};record->positionW=1;
            record->velocity={{0,0,0}};record->velocityW=1;
        }
        //Original still allocates record0 if the pointer-array malloc failed.
        //Store its exact attempted prefix safely, then stop before NULL SW.
        pc=0x160f48;
        if(!pool.pointers){pool.unpublishedRecord=std::move(record);return false;}
        pool.pointers[pool.published++]=record.release(); //Publish evenNULL, then continue.
    }
    pool.loopReturned=true;pc=0x160f60;return true;
}
// The source pointer-array allocation is fixed5000 in this admitted caller.
// Host-width indexing adapts EE index*4; it does not encode/truncate a pointer.
bool OriginalGcParticleManager50KG::readSourcePositionSlot1606E4(
    const void* publication138,std::int32_t index128,
    const OriginalGcParticle50KG*& record)const noexcept {
    if(poolMode_!=PoolMode::sourcePending||storage_||!sourcePool_)return false;
    const auto& pool=*sourcePool_;
    if(!pool.pointersPublished||!pool.loopAttempted||!pool.loopReturned||
       !pool.pointers||publication138!=pool.pointers.get()||pool.unpublishedRecord||
       pool.allocationAttempts!=originalGcParticleCapacity50KG||
       pool.published!=originalGcParticleCapacity50KG||index128<0)return false;
    const auto index=static_cast<std::size_t>(index128);
    if(index>=originalGcParticleCapacity50KG||index>=pool.published)return false;
    record=pool.pointers[index]; //1606E4 pointer read, including NULL.
    return true; //No record dereference before the reached1606FC store.
}
bool OriginalGcParticleManager50KG::sourcePositionSlotCurrentNative(
    const void* publication138,std::int32_t index128,
    const OriginalGcParticle50KG* expected,
    const std::array<std::uint32_t,4>* committedPosition)const noexcept {
    const OriginalGcParticle50KG* actual=nullptr;
    if(!readSourcePositionSlot1606E4(publication138,index128,actual)||actual!=expected)return false;
    if(!committedPosition)return true; //NULL slot remains a genuine read result.
    if(!actual)return false;
    static_assert(offsetof(OriginalGcParticle50KG,position)==0&&
        offsetof(OriginalGcParticle50KG,positionW)==0x0c,"source position00 layout");
    std::array<std::uint32_t,4> bits{};
    std::memcpy(bits.data(),actual->position.data(),12);
    std::memcpy(&bits[3],&actual->positionW,4);
    return bits==*committedPosition;
}
bool OriginalGcParticleManager50KG::storeSourcePosition1606FC(
    const void* publication138,std::int32_t index128,
    const OriginalGcParticle50KG* expected,const std::array<std::uint32_t,4>& after,
    std::array<std::uint32_t,4>& before)noexcept {
    // This native validation is not a second source slot load or source test.
    // Exact identity comes from the private array already owned by this pool.
    if(!expected||after[3]!=0x3f800000u||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    auto* record=sourcePool_->pointers[static_cast<std::size_t>(index128)];
    std::array<std::uint32_t,4> previous{};
    //Only the genuine constructor-known00..0F prefix is observed. Neither
    //unknown host defaults nor ownerId's legacy native representation is used.
    std::memcpy(previous.data(),record->position.data(),12);
    std::memcpy(&previous[3],&record->positionW,4);
    if(previous[3]!=0x3f800000u)return false;
    //One logical SQ: all guards precede these nonthrowing byte copies. No
    //later operation can fail between the source write and its receipt.
    std::memcpy(record->position.data(),after.data(),12);
    std::memcpy(&record->positionW,&after[3],4);
    before=previous;
    return true;
}
bool OriginalGcParticleManager50KG::sourceVelocitySlotCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const std::array<std::uint32_t,4>* committedVelocity)const noexcept {
    if(index128!=0||!expected||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    if(!committedVelocity)return true; //No source velocity read before its reached store.
    if(!detail::sourceParticleFogVelocityVector50LA(*committedVelocity))return false;
    static_assert(offsetof(OriginalGcParticle50KG,velocity)==0x10&&
        offsetof(OriginalGcParticle50KG,velocityW)==0x1c,"source velocity10 layout");
    std::array<std::uint32_t,4> bits{};
    std::memcpy(bits.data(),expected->velocity.data(),12);
    std::memcpy(&bits[3],&expected->velocityW,4);
    //Later common-tail scalar/owner writes do not invalidate this earlier SQ.
    return bits==*committedVelocity;
}
bool OriginalGcParticleManager50KG::storeSourceVelocity16088C(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const std::array<std::uint32_t,4>& after,std::array<std::uint32_t,4>& before)noexcept {
    if(!detail::sourceParticleFogVelocityVector50LA(after)||
       !sourceVelocitySlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->fogFields;
    if(fields.record||fields.emitter||fields.owner48||fields.storedCount)return false;
    auto* record=sourcePool_->pointers[0];
    std::array<std::uint32_t,4> previous{};
    //Only constructor-known10..1F is read. No unknown scalar/owner lane or
    //whole-record snapshot is observed; source owner48 remains untouched.
    std::memcpy(previous.data(),record->velocity.data(),12);
    std::memcpy(&previous[3],&record->velocityW,4);
    if(previous[3]!=0x3f800000u)return false;
    //All guards precede one logical full SQ. Nothing can fail between these
    //nonthrowing copies and the returned pre-store receipt.
    std::memcpy(record->velocity.data(),after.data(),12);
    std::memcpy(&record->velocityW,&after[3],4);
    before=previous;
    return true;
}
bool OriginalGcParticleManager50KG::sourceFogFieldsCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t storedCount,const std::array<std::uint32_t,5>& values)const noexcept {
    if(index128!=0||!expected||!emitter||storedCount>6||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->fogFields;
    if(fields.storedCount!=storedCount)return false;
    if(!storedCount)return !fields.record&&!fields.emitter&&!fields.owner48;
    if(fields.record!=expected||fields.emitter!=emitter||
       (storedCount>=3?fields.owner48!=emitter:fields.owner48!=nullptr))return false;
    //Only completed source writes make these lanes readable. No previous
    //unknown value or whole-record copy is used for native validation.
    std::uint32_t bits=0;
    if(storedCount>=1){std::memcpy(&bits,&expected->size,4);if(bits!=values[0])return false;}
    if(storedCount>=2){std::memcpy(&bits,&expected->initialSize,4);if(bits!=values[1])return false;}
    if(storedCount>=4){std::memcpy(&bits,&expected->lifeMilliseconds,4);if(bits!=values[2])return false;}
    if(storedCount>=5){std::memcpy(&bits,&expected->ageMilliseconds,4);if(bits!=values[3])return false;}
    if(storedCount>=6){std::memcpy(&bits,&expected->frame,4);if(bits!=values[4])return false;}
    return true;
}
bool OriginalGcParticleManager50KG::storeSourceFogField1608AC(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorCount,std::uint32_t sourcePC,std::uint32_t bits)noexcept {
    constexpr std::uint32_t stores[]={0x1608ac,0x1608b8,0x1608bc,0x1608c4,0x1608cc,0x1608d4};
    if(priorCount>=6||sourcePC!=stores[priorCount]||index128!=0||!expected||!emitter||
       (priorCount==2&&bits)||!sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    auto& fields=sourcePool_->fogFields;
    if(fields.storedCount!=priorCount)return false;
    if(!priorCount){if(fields.record||fields.emitter||fields.owner48)return false;}
    else if(fields.record!=expected||fields.emitter!=emitter||
            (priorCount>=3?fields.owner48!=emitter:fields.owner48!=nullptr))return false;
    static_assert(offsetof(OriginalGcParticle50KG,size)==0x38&&
        offsetof(OriginalGcParticle50KG,initialSize)==0x3c&&
        offsetof(OriginalGcParticle50KG,lifeMilliseconds)==0x30&&
        offsetof(OriginalGcParticle50KG,ageMilliseconds)==0x2c&&
        offsetof(OriginalGcParticle50KG,frame)==0x28,"source Fog scalar layout");
    auto* record=sourcePool_->pointers[0];
    //All refusal points precede the one source store. Embedded metadata and
    //the completed-count update cannot allocate or throw after its effect.
    switch(priorCount){
        case 0:std::memcpy(&record->size,&bits,4);break;
        case 1:std::memcpy(&record->initialSize,&bits,4);break;
        case 2:fields.owner48=emitter;break; //No ownerId write/truncation/registry.
        case 3:std::memcpy(&record->lifeMilliseconds,&bits,4);break;
        case 4:std::memcpy(&record->ageMilliseconds,&bits,4);break;
        case 5:std::memcpy(&record->frame,&bits,4);break;
    }
    if(!priorCount){fields.record=expected;fields.emitter=emitter;}
    ++fields.storedCount;
    return true;
}
//Independent slot1 velocity/field custody. The first fogFields/fogTail are
//never reset or adopted, and this native source48 is not a legacy ownerId.
bool OriginalGcParticleManager50KG::sourceSuccessorVelocitySlotCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const std::array<std::uint32_t,4>* committedVelocity)const noexcept {
    if(index128!=1||!expected||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    if(expected==sourcePool_->fogFields.record)return false;
    if(!committedVelocity)return true; //No source velocity read before its reached store.
    if(!detail::sourceParticleFogVelocityVector50LA(*committedVelocity))return false;
    static_assert(offsetof(OriginalGcParticle50KG,velocity)==0x10&&
        offsetof(OriginalGcParticle50KG,velocityW)==0x1c,"source velocity10 layout");
    std::array<std::uint32_t,4> bits{};
    std::memcpy(bits.data(),expected->velocity.data(),12);
    std::memcpy(&bits[3],&expected->velocityW,4);
    //Later common-tail scalar/owner writes do not invalidate this earlier SQ.
    return bits==*committedVelocity;
}
bool OriginalGcParticleManager50KG::storeSourceSuccessorVelocity16088C(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const std::array<std::uint32_t,4>& after,std::array<std::uint32_t,4>& before)noexcept {
    if(!detail::sourceParticleFogVelocityVector50LA(after)||
       !sourceSuccessorVelocitySlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->successorFogFields;
    if(fields.record||fields.emitter||fields.owner48||fields.storedCount)return false;
    auto* record=sourcePool_->pointers[1];
    std::array<std::uint32_t,4> previous{};
    //Only constructor-known10..1F is read. No unknown scalar/owner lane or
    //whole-record snapshot is observed; source owner48 remains untouched.
    std::memcpy(previous.data(),record->velocity.data(),12);
    std::memcpy(&previous[3],&record->velocityW,4);
    if(previous[3]!=0x3f800000u)return false;
    //All guards precede one logical full SQ. Nothing can fail between these
    //nonthrowing copies and the returned pre-store receipt.
    std::memcpy(record->velocity.data(),after.data(),12);
    std::memcpy(&record->velocityW,&after[3],4);
    before=previous;
    return true;
}
bool OriginalGcParticleManager50KG::sourceSuccessorFogFieldsCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t storedCount,const std::array<std::uint32_t,5>& values)const noexcept {
    if(index128!=1||!expected||!emitter||storedCount>6||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->successorFogFields;
    if(expected==sourcePool_->fogFields.record||emitter==sourcePool_->fogFields.emitter||fields.storedCount!=storedCount)return false;
    if(!storedCount)return !fields.record&&!fields.emitter&&!fields.owner48;
    if(fields.record!=expected||fields.emitter!=emitter||
       (storedCount>=3?fields.owner48!=emitter:fields.owner48!=nullptr))return false;
    //Only completed source writes make these lanes readable. No previous
    //unknown value or whole-record copy is used for native validation.
    std::uint32_t bits=0;
    if(storedCount>=1){std::memcpy(&bits,&expected->size,4);if(bits!=values[0])return false;}
    if(storedCount>=2){std::memcpy(&bits,&expected->initialSize,4);if(bits!=values[1])return false;}
    if(storedCount>=4){std::memcpy(&bits,&expected->lifeMilliseconds,4);if(bits!=values[2])return false;}
    if(storedCount>=5){std::memcpy(&bits,&expected->ageMilliseconds,4);if(bits!=values[3])return false;}
    if(storedCount>=6){std::memcpy(&bits,&expected->frame,4);if(bits!=values[4])return false;}
    return true;
}
bool OriginalGcParticleManager50KG::storeSourceSuccessorFogField1608AC(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorCount,std::uint32_t sourcePC,std::uint32_t bits)noexcept {
    constexpr std::uint32_t stores[]={0x1608ac,0x1608b8,0x1608bc,0x1608c4,0x1608cc,0x1608d4};
    if(priorCount>=6||sourcePC!=stores[priorCount]||index128!=1||!expected||!emitter||
       (priorCount==2&&bits)||!sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    auto& fields=sourcePool_->successorFogFields;
    if(expected==sourcePool_->fogFields.record||emitter==sourcePool_->fogFields.emitter||fields.storedCount!=priorCount)return false;
    if(!priorCount){if(fields.record||fields.emitter||fields.owner48)return false;}
    else if(fields.record!=expected||fields.emitter!=emitter||
            (priorCount>=3?fields.owner48!=emitter:fields.owner48!=nullptr))return false;
    static_assert(offsetof(OriginalGcParticle50KG,size)==0x38&&
        offsetof(OriginalGcParticle50KG,initialSize)==0x3c&&
        offsetof(OriginalGcParticle50KG,lifeMilliseconds)==0x30&&
        offsetof(OriginalGcParticle50KG,ageMilliseconds)==0x2c&&
        offsetof(OriginalGcParticle50KG,frame)==0x28,"source Fog scalar layout");
    auto* record=sourcePool_->pointers[1];
    //All refusal points precede the one source store. Embedded metadata and
    //the completed-count update cannot allocate or throw after its effect.
    switch(priorCount){
        case 0:std::memcpy(&record->size,&bits,4);break;
        case 1:std::memcpy(&record->initialSize,&bits,4);break;
        case 2:fields.owner48=emitter;break; //No ownerId write/truncation/registry.
        case 3:std::memcpy(&record->lifeMilliseconds,&bits,4);break;
        case 4:std::memcpy(&record->ageMilliseconds,&bits,4);break;
        case 5:std::memcpy(&record->frame,&bits,4);break;
    }
    if(!priorCount){fields.record=expected;fields.emitter=emitter;}
    ++fields.storedCount;
    return true;
}
bool OriginalGcParticleManager50KG::readSourceFogRetirementSlot16056C(
    const void* publication138,const OriginalGcParticle50KG*& record)const noexcept {
    //Shared private backing admission, distinct source16056C load identity.
    return readSourcePositionSlot1606E4(publication138,0,record);
}
bool OriginalGcParticleManager50KG::readSourceFogOwner160570(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void*& owner48)const noexcept {
    if(index128!=0||!expected||!sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->fogFields;
    if(fields.record!=expected||!fields.emitter||fields.storedCount<3||fields.storedCount>6||
       fields.owner48!=fields.emitter)return false;
    owner48=fields.owner48;return true; //Actual pointer-width source lane; never a legacy numeric owner.
}
bool OriginalGcParticleManager50KG::readSourceFogLife1608D8(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint32_t& bits)const noexcept {
    if(index128!=0||!expected||!emitter||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->fogFields;
    if(fields.record!=expected||fields.emitter!=emitter||fields.owner48!=emitter||fields.storedCount!=6)return false;
    std::memcpy(&bits,&expected->lifeMilliseconds,4); //Actual1608D8 load, not replay of17C.
    return true;
}
bool OriginalGcParticleManager50KG::sourceFogTailCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t completedOps,const std::array<std::uint32_t,7>& pcs,
    const std::array<std::uint32_t,7>& values)const noexcept {
    if(index128!=0||!expected||!emitter||completedOps>7||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    const auto& fields=sourcePool_->fogFields;const auto& tail=sourcePool_->fogTail;
    if(fields.record!=expected||fields.emitter!=emitter||fields.owner48!=emitter||fields.storedCount!=6||
       tail.completedOps!=completedOps||tail.pcs!=pcs||tail.values!=values)return false;
    static_assert(sizeof(float)==4&&sizeof(std::uint32_t)==4&&
        offsetof(OriginalGcParticle50KG,inverseLife)==0x34&&
        offsetof(OriginalGcParticle50KG,growth)==0x40&&
        offsetof(OriginalGcParticle50KG,flags)==0x44,"source Fog post-DIV layout");
    //No unknown34/40/44 before-image. Exceptional EE words stay raw bytes;
    //they must never be interpreted as host floats (where they encode NaNs).
    std::uint32_t bits=0;
    if(completedOps>=1){std::memcpy(&bits,&expected->inverseLife,4);if(bits!=values[0])return false;}
    if(completedOps>=2){std::memcpy(&bits,&expected->growth,4);if(bits!=values[1])return false;}
    if(completedOps>=3){
        const auto latest=completedOps>=7?6u:(completedOps>=5?4u:2u);
        std::memcpy(&bits,&expected->flags,4);if(bits!=values[latest])return false;
    }
    return true;
}
bool OriginalGcParticleManager50KG::storeSourceFogTail1608E0(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorOps,std::uint32_t sourcePC,std::uint32_t bits)noexcept {
    if(!sourcePool_||priorOps>=7)return false;
    auto& tail=sourcePool_->fogTail;
    if(!sourceFogTailCurrentNative(publication138,index128,expected,emitter,priorOps,tail.pcs,tail.values))return false;
    //Only this exact seven-operation chain is admitted. Mirror branch
    //selection belongs to the caller's genuine saved-unit/enable receipts.
    switch(priorOps){
        case 0:if(sourcePC!=0x1608e0u||!detail::sourceParticleFogReciprocalWord50LA(bits))return false;break;
        case 1:if(sourcePC!=0x1608e4u||bits)return false;break;
        case 2:if(sourcePC!=0x1608f0u)return false;break;
        case 4:{
            const bool set=tail.pcs[3]==0x160930u;
            if((!set&&tail.pcs[3]!=0x160940u)||sourcePC!=(set?0x16093cu:0x16094cu)||
               bits!=(set?(tail.values[3]|0x4000u):(tail.values[3]&0xffffbfffu)))return false;
            break;
        }
        case 6:{
            const bool set=tail.pcs[5]==0x160964u;
            if((!set&&tail.pcs[5]!=0x160974u)||sourcePC!=(set?0x160970u:0x160984u)||
               bits!=(set?(tail.values[5]|0x8000u):(tail.values[5]&0xffff7fffu)))return false;
            break;
        }
        default:return false; //The two44 source loads cannot be skipped.
    }
    auto* record=sourcePool_->pointers[0];
    //Every refusal is above the actual store. Both source bytes and the
    //embedded completed-operation receipt commit without allocation/throw.
    if(priorOps==0)std::memcpy(&record->inverseLife,&bits,4);
    else if(priorOps==1)std::memcpy(&record->growth,&bits,4);
    else std::memcpy(&record->flags,&bits,4);
    tail.pcs[priorOps]=sourcePC;tail.values[priorOps]=bits;++tail.completedOps;
    return true;
}
bool OriginalGcParticleManager50KG::readSourceFogFlags160930(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorOps,std::uint32_t sourcePC,std::uint32_t& bits)noexcept {
    if(!sourcePool_||(priorOps!=3&&priorOps!=5))return false;
    auto& tail=sourcePool_->fogTail;
    if((priorOps==3?(sourcePC!=0x160930u&&sourcePC!=0x160940u):
                       (sourcePC!=0x160964u&&sourcePC!=0x160974u))||
       !sourceFogTailCurrentNative(publication138,index128,expected,emitter,priorOps,tail.pcs,tail.values))return false;
    //Actual source LW from this exact record, not tail.values[2 or4].
    //Native receipt bookkeeping is not a source write to the particle.
    std::uint32_t actual=0;std::memcpy(&actual,&expected->flags,4);
    tail.pcs[priorOps]=sourcePC;tail.values[priorOps]=actual;++tail.completedOps;
    bits=actual;return true;
}
//A second independently bound record; the first field/tail ledgers remain intact.
bool OriginalGcParticleManager50KG::readSourceSuccessorFogLife1608D8(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint32_t& bits)const noexcept {
    if(index128!=1||!expected||!emitter||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    if(expected==sourcePool_->fogFields.record||emitter==sourcePool_->fogFields.emitter)return false;
    const auto& fields=sourcePool_->successorFogFields;
    if(fields.record!=expected||fields.emitter!=emitter||fields.owner48!=emitter||fields.storedCount!=6)return false;
    std::memcpy(&bits,&expected->lifeMilliseconds,4); //Actual1608D8 load, not replay of17C.
    return true;
}
bool OriginalGcParticleManager50KG::sourceSuccessorFogTailCurrentNative(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t completedOps,const std::array<std::uint32_t,7>& pcs,
    const std::array<std::uint32_t,7>& values)const noexcept {
    if(index128!=1||!expected||!emitter||completedOps>7||
       !sourcePositionSlotCurrentNative(publication138,index128,expected,nullptr))return false;
    if(expected==sourcePool_->fogFields.record||emitter==sourcePool_->fogFields.emitter)return false;
    const auto& fields=sourcePool_->successorFogFields;const auto& tail=sourcePool_->successorFogTail;
    if(fields.record!=expected||fields.emitter!=emitter||fields.owner48!=emitter||fields.storedCount!=6||
       tail.completedOps!=completedOps||tail.pcs!=pcs||tail.values!=values)return false;
    static_assert(sizeof(float)==4&&sizeof(std::uint32_t)==4&&
        offsetof(OriginalGcParticle50KG,inverseLife)==0x34&&
        offsetof(OriginalGcParticle50KG,growth)==0x40&&
        offsetof(OriginalGcParticle50KG,flags)==0x44,"source Fog post-DIV layout");
    //No unknown34/40/44 before-image. Exceptional EE words stay raw bytes;
    //they must never be interpreted as host floats (where they encode NaNs).
    std::uint32_t bits=0;
    if(completedOps>=1){std::memcpy(&bits,&expected->inverseLife,4);if(bits!=values[0])return false;}
    if(completedOps>=2){std::memcpy(&bits,&expected->growth,4);if(bits!=values[1])return false;}
    if(completedOps>=3){
        const auto latest=completedOps>=7?6u:(completedOps>=5?4u:2u);
        std::memcpy(&bits,&expected->flags,4);if(bits!=values[latest])return false;
    }
    return true;
}
bool OriginalGcParticleManager50KG::storeSourceSuccessorFogTail1608E0(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorOps,std::uint32_t sourcePC,std::uint32_t bits)noexcept {
    if(!sourcePool_||priorOps>=7)return false;
    auto& tail=sourcePool_->successorFogTail;
    if(!sourceSuccessorFogTailCurrentNative(publication138,index128,expected,emitter,priorOps,tail.pcs,tail.values))return false;
    //Only this exact seven-operation chain is admitted. Mirror branch
    //selection belongs to the caller's genuine saved-unit/enable receipts.
    switch(priorOps){
        case 0:if(sourcePC!=0x1608e0u||!detail::sourceParticleFogReciprocalWord50LA(bits))return false;break;
        case 1:if(sourcePC!=0x1608e4u||bits)return false;break;
        case 2:if(sourcePC!=0x1608f0u)return false;break;
        case 4:{
            const bool set=tail.pcs[3]==0x160930u;
            if((!set&&tail.pcs[3]!=0x160940u)||sourcePC!=(set?0x16093cu:0x16094cu)||
               bits!=(set?(tail.values[3]|0x4000u):(tail.values[3]&0xffffbfffu)))return false;
            break;
        }
        case 6:{
            const bool set=tail.pcs[5]==0x160964u;
            if((!set&&tail.pcs[5]!=0x160974u)||sourcePC!=(set?0x160970u:0x160984u)||
               bits!=(set?(tail.values[5]|0x8000u):(tail.values[5]&0xffff7fffu)))return false;
            break;
        }
        default:return false; //The two44 source loads cannot be skipped.
    }
    auto* record=sourcePool_->pointers[1];
    //Every refusal is above the actual store. Both source bytes and the
    //embedded completed-operation receipt commit without allocation/throw.
    if(priorOps==0)std::memcpy(&record->inverseLife,&bits,4);
    else if(priorOps==1)std::memcpy(&record->growth,&bits,4);
    else std::memcpy(&record->flags,&bits,4);
    tail.pcs[priorOps]=sourcePC;tail.values[priorOps]=bits;++tail.completedOps;
    return true;
}
bool OriginalGcParticleManager50KG::readSourceSuccessorFogFlags160930(
    const void* publication138,std::int32_t index128,const OriginalGcParticle50KG* expected,
    const void* emitter,std::uint8_t priorOps,std::uint32_t sourcePC,std::uint32_t& bits)noexcept {
    if(!sourcePool_||(priorOps!=3&&priorOps!=5))return false;
    auto& tail=sourcePool_->successorFogTail;
    if((priorOps==3?(sourcePC!=0x160930u&&sourcePC!=0x160940u):
                       (sourcePC!=0x160964u&&sourcePC!=0x160974u))||
       !sourceSuccessorFogTailCurrentNative(publication138,index128,expected,emitter,priorOps,tail.pcs,tail.values))return false;
    //Actual source LW from this exact record, not tail.values[2 or4].
    //Native receipt bookkeeping is not a source write to the particle.
    std::uint32_t actual=0;std::memcpy(&actual,&expected->flags,4);
    tail.pcs[priorOps]=sourcePC;tail.values[priorOps]=actual;++tail.completedOps;
    bits=actual;return true;
}
struct OriginalGcParticleManager50KG::Storage {
    std::array<OriginalGcParticle50KG,originalGcParticleCapacity50KG> pool{};
    // Source active/free pointer permutation survives removals; swapping whole
    // particle objects would change stable emitter/particle identity.
    std::array<OriginalGcParticle50KG*,originalGcParticleCapacity50KG> order{};
    std::vector<OriginalGcParticleGroup50KG> groups;
    std::vector<OriginalGcParticleFrame50KG> frames;
    //Native access units, not source158/readiness. Starting normalization
    //revokes the pixel-only capability before any lane can change.
    enum class AtlasUnits {pixels,garageNormalizationStarted,garageNormalized};
    AtlasUnits atlasUnits=AtlasUnits::pixels;
    std::string atlasTextureMap;
    EmitterAllocation50KG emitterAllocation{};
    EmitterVector50KG emitters;
    FogDefinition fog{};
    OriginalGcParticleAuxiliaryChild50KG auxiliaryChild{};
    std::array<float,3> fireOffset{};
    std::array<std::array<float,3>,3> fogPositions{};
    std::array<std::uint32_t,3> fogOwnerIds{};
    std::array<std::int32_t,3> fogCurrentFrames{};
    std::size_t count=0;
    bool gcFogAuthoring=false,fogPopulated=false,fogPopulationAttempted=false;
    Storage():emitters(EmitterAllocator50KG<OriginalGcParticleEmitterState50KG>{&emitterAllocation}){
        for(std::size_t i=0;i<pool.size();++i)order[i]=&pool[i];
    }
    Storage(const Storage&)=delete;
    Storage& operator=(const Storage&)=delete;
    Storage(Storage&&)=delete;
    Storage& operator=(Storage&&)=delete;
};
// Private real-pool write, reached only from Draw's authentic162870 cursor.
// No const-cast: Storage owns mutable particles; the public manager borrow is
// intentionally read-only and exposes no corresponding write operation.
bool OriginalGcParticleManager50KG::readGarageFrames1626D0(
    const OriginalGcParticleFrame50KG*& frames,std::size_t& count)const noexcept {
    if(!storage_||!prepared()||storage_->atlasUnits!=Storage::AtlasUnits::garageNormalized||
       storage_->frames.empty())return false;
    frames=storage_->frames.data();count=storage_->frames.size();return true;
}
bool OriginalGcParticleManager50KG::readGaragePoolSlot162448(
    const OriginalGcParticle50KG* expected,std::size_t& actualPoolSlot)const noexcept {
    if(!storage_||!prepared()||storage_->atlasUnits!=Storage::AtlasUnits::garageNormalized||!expected)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(storage_->pool.data());
    const auto pointer=reinterpret_cast<std::uintptr_t>(expected);
    const auto extent=sizeof(OriginalGcParticle50KG)*storage_->pool.size();
    if(pointer<begin||pointer-begin>=extent||(pointer-begin)%sizeof(OriginalGcParticle50KG))return false;
    actualPoolSlot=(pointer-begin)/sizeof(OriginalGcParticle50KG);
    return &storage_->pool[actualPoolSlot]==expected;
}
bool OriginalGcParticleManager50KG::publishGarageProjection162A24(
    std::size_t activeIndex,const OriginalGcParticle50KG* expected,float depth,
    std::size_t& actualPoolSlot)const noexcept {
    if(!storage_||!prepared()||storage_->atlasUnits!=Storage::AtlasUnits::garageNormalized||
       activeIndex>=storage_->count||!expected||storage_->order[activeIndex]!=expected||!std::isfinite(depth))
        return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(storage_->pool.data());
    const auto pointer=reinterpret_cast<std::uintptr_t>(expected);
    const auto extent=sizeof(OriginalGcParticle50KG)*storage_->pool.size();
    if(pointer<begin||pointer-begin>=extent||(pointer-begin)%sizeof(OriginalGcParticle50KG))return false;
    actualPoolSlot=(pointer-begin)/sizeof(OriginalGcParticle50KG);
    auto& actual=storage_->pool[actualPoolSlot];
    if(&actual!=expected)return false;
    std::memcpy(&actual.reserved24,&depth,sizeof depth); //Actual float source24 bits.
    return true;
}
OriginalGcParticleManager50KG::OriginalGcParticleManager50KG() noexcept=default;
OriginalGcParticleManager50KG::~OriginalGcParticleManager50KG()=default;
OriginalGcParticleStatus50KG OriginalGcParticleManager50KG::prepare(const Bytes& prt,
    const Bytes& emt,const Bytes& tob) noexcept {
    if(poolMode_!=PoolMode::empty||storage_||sourcePool_)return Status::alreadyPrepared;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    // The course TOB is the data: its stock identity (9623 bytes, e677f80d..)
    // is a receipt. PORT STAND-IN (flagged): the GC atlas (PRT) and fog EMT
    // stay pinned because this native fog/fire store holds exactly the stock
    // 9-group/129-frame atlas and one fog definition (header storage, ~188 TUs).
    if(prt.size()!=9210||emt.size()!=1298||
       !hashMatches(prt.data(),prt.size(),"d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f")||
       !hashMatches(emt.data(),emt.size(),"3ca0cc75636e0f357e6ed1a8ada590bdc51862fef90cd30a0a3d5085e152e17c"))return Status::assetIdentity;
    if(tob.empty()||tob.size()>2u*1024u*1024u)return Status::authoredData;
    if(tob.size()!=9623||!hashMatches(tob.data(),tob.size(),"e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454"))
        std::cerr<<"course_member_identity site=particles_GC member=tracks\\gc.tob bytes="<<tob.size()<<" stock=0\n";
    try {
        auto s=std::make_unique<Storage>();Document atlas,emitterDoc;
        if(!readText(prt,atlas)||!readText(emt,emitterDoc)||!readFog(emitterDoc,s->fog))return Status::authoredData;
        const auto* info=findSection(atlas,"General Info");std::int32_t count=0;
        if(!textIs(info,"TextureMap","demoprt.tga")||!integer(info,"NumFrames",count)||count!=129||
           !integer(info,"NumGroups",count)||count!=9)return Status::authoredData;
        s->groups.resize(9);s->frames.resize(129);s->atlasTextureMap="demoprt.tga";
        for(std::size_t i=0;i<s->groups.size();++i) {
            auto& g=s->groups[i];const auto* section=findSection(atlas,"Group"+std::to_string(i));
            if(!integer(section,"TypeID",g.type)||!integer(section,"FirstFrame",g.first)||
               !integer(section,"LastFrame",g.last)||g.first<0||g.last<g.first||g.last>=129)return Status::authoredData;
        }
        for(std::size_t i=0;i<s->frames.size();++i) {
            const auto* section=findSection(atlas,"Frame"+std::to_string(i));float l=0,t=0,r=0,b=0;
            if(!number(section,"UVLeft",l)||!number(section,"UVRight",r)||
               !number(section,"UVTop",t)||!number(section,"UVBottom",b)||l>r||t>b)return Status::authoredData;
            s->frames[i].sourceUv={{{{l,t}},{{r,t}},{{r,b}},{{l,b}}}};
        }
        if(s->groups[0].type!=0||s->groups[0].first!=0||s->groups[0].last!=15||
           s->groups[6].type!=6||s->groups[6].first!=105||s->groups[6].last!=128)return Status::authoredData;
        assets::ParameterDocument48F compiled;assets::DecodeError48C error{};
        if(!assets::decodeParameters48F(tob,compiled,error))return Status::authoredData;
        Document toy;toy.reserve(compiled.blocks.size());
        for(const auto& block:compiled.blocks) {
            Section section{block.name,block.offset,{}};section.values.reserve(block.fields.size());
            for(const auto& f:block.fields)section.values.push_back({f.name,f.value,f.entry_offset,f.value_offset,!f.value.empty()});
            toy.push_back(std::move(section));
        }
        // ONE common [ParticleEmitters] reader (27DE00/27C600/27C7B0,
        // src\\assets\\original_particle_emitters50pe.h) reads the table.
        namespace pe=assets::particle_emitters50PE;pe::Table table;
        if(!pe::read(compiled,"GC",table))return Status::authoredData;
        // PORT STAND-IN (flagged): this native GC fog/fire chain implements one
        // attached generator-fire emitter (row 1, read by its own source owner)
        // followed by exactly three positioned fog emitters (fixed store in the
        // header, ~188 TUs). Another table is refused here, not ignored.
        if(table.live!=4||table.loadedCount!=4)return Status::authoredData;
        const auto& fireRow=table.rows[0];
        if(fireRow.typeId!=8||!fireRow.attached||fireRow.startOn==0||fireRow.childObject!="NONE"||
           fireRow.startingZone<1||!pe::detail::sameNoCase(fireRow.filename,"genratorfire.emt"))return Status::authoredData;
        // Stock receipts: zone 10, 6 instances, shade_a.slt.
        if(fireRow.startingZone!=10||fireRow.instances!=6||fireRow.attachToSoulTree!="shade_a.slt")
            std::cerr<<"course_count site=particles_GC fire_zone="<<fireRow.startingZone<<" instances="<<fireRow.instances<<" stock_layout=0\n";
        // Vector values keep this file's own decimal reader (bit-identical stock values).
        const auto* fire=findSection(toy,"particleemitter"+std::to_string(fireRow.ordinal));
        if(!triple(fire,"offsetfromsoultree",s->fireOffset))return Status::authoredData;
        // Emitter1's attached generator fires are a separate source owner;
        // this cold path neither invents their particles nor consumes their RNG.
        for(std::size_t i=0;i<3;++i) {
            const auto& row=table.rows[i+1];
            if(row.typeId!=12||row.attached||row.instances!=1||row.startOn==0||
               !pe::detail::sameNoCase(row.filename,"fog.emt"))return Status::authoredData;
            if(row.startingZone!=10)std::cerr<<"course_count site=particles_GC fog="<<i<<" zone="<<row.startingZone<<" stock=10\n";
            const auto* section=findSection(toy,"particleemitter"+std::to_string(row.ordinal));
            if(!triple(section,"position",s->fogPositions[i]))return Status::authoredData;
            s->fogCurrentFrames[i]=s->groups[0].first;
        }
        s->gcFogAuthoring=true;storage_=std::move(s);poolMode_=PoolMode::nativePrepared;return Status::complete;
    }catch(...){return Status::allocation;}
}
OriginalGcParticleStatus50KG OriginalGcParticleManager50KG::prepareTrackAtlas160E00(
    const Bytes& prt) noexcept {
    if(poolMode_!=PoolMode::empty||storage_||sourcePool_)return Status::alreadyPrepared;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    if(prt.empty()||prt.size()>assets::originalUiDatByteLimit50KG)return Status::authoredData;
    try {
        Document atlas;if(!readText(prt,atlas))return Status::authoredData;
        const auto* info=findSection(atlas,"General Info");
        const auto* texture=info?info->find("TextureMap"):nullptr;
        std::int32_t frames=0,groups=0;
        //161874/16188C read source counts;1618BC/161974 size their backing.
        // Restrict the accepted native resource size, never pad to GC's9/129.
        if(!texture||texture->text.empty()||texture->text.size()>255||
           !integer(info,"NumFrames",frames)||frames<=0||frames>4096||
           !integer(info,"NumGroups",groups)||groups<=0||groups>4096)
            return Status::authoredData;
        auto next=std::make_unique<Storage>();next->atlasTextureMap=texture->text;
        next->groups.resize(static_cast<std::size_t>(groups));
        next->frames.resize(static_cast<std::size_t>(frames));
        // Original161900..161948 reads metadata without clamping LastFrame.
        // Authenticated MC declares152 frames but Group9 ends at152. Preserve
        // that exact source metadata, not a synthetic153rd frame. frame(152)
        // remains null and real render consumers fail closed before dereference.
        const bool mcAuthoredEndpoint=prt.size()==10770&&hashMatches(prt.data(),prt.size(),
            "a92a82d85adea560886fc0e6911baede2f11e9448b9ac4365bf7037e9e809020");
        for(std::size_t i=0;i<next->groups.size();++i){
            auto& g=next->groups[i];const auto* section=findSection(atlas,"Group"+std::to_string(i));
            if(!integer(section,"TypeID",g.type)||!integer(section,"FirstFrame",g.first)||
               !integer(section,"LastFrame",g.last)||g.first<0||g.last<g.first||
               (g.last>=frames&&!(mcAuthoredEndpoint&&frames==152&&groups==10&&
                 i==9&&g.type==8&&g.first==129&&g.last==152)))
                return Status::authoredData;
        }
        for(std::size_t i=0;i<next->frames.size();++i){
            const auto* section=findSection(atlas,"Frame"+std::to_string(i));float l=0,t=0,r=0,b=0;
            if(!number(section,"UVLeft",l)||!number(section,"UVRight",r)||
               !number(section,"UVTop",t)||!number(section,"UVBottom",b)||
               !std::isfinite(l)||!std::isfinite(t)||!std::isfinite(r)||!std::isfinite(b)||l>r||t>b)
                return Status::authoredData;
            //161A0C..161A90 preserves source UV order and units. Normalizing
            // pixels requires the actual material/texture owner at161200.
            next->frames[i].sourceUv={{{{l,t}},{{r,t}},{{r,b}},{{l,b}}}};
        }
        // Storage establishes the same5000 stable particle slots/free order.
        // No fog/fire provider, emitter or particle is manufactured here.
        storage_=std::move(next);poolMode_=PoolMode::nativePrepared;return Status::complete;
    }catch(...){return Status::allocation;}
}
bool OriginalGcParticleManager50KG::prepared() const noexcept {
    return poolMode_==PoolMode::nativePrepared&&storage_&&!sourcePool_;
}
bool OriginalGcParticleManager50KG::gcFogAuthoringPrepared() const noexcept {
    return storage_&&storage_->gcFogAuthoring;
}
const std::string* OriginalGcParticleManager50KG::atlasTextureMapName() const noexcept {
    return storage_?&storage_->atlasTextureMap:nullptr;
}
const OriginalGcParticleGroup50KG* OriginalGcParticleManager50KG::group(std::int32_t i) const noexcept {
    return storage_&&i>=0&&std::size_t(i)<storage_->groups.size()?&storage_->groups[std::size_t(i)]:nullptr;
}
OriginalGcParticleStatus50KG OriginalGcParticleManager50KG::normalizeGarageAtlas161234(float width,float height) noexcept {
    if(!storage_||storage_->atlasUnits!=Storage::AtlasUnits::pixels)return Status::unsupported;
    if(std::fegetround()!=FE_TONEAREST||!scalar(width)||!scalar(height)||width<=0||height<=0)
        return Status::arithmetic;
    storage_->atlasUnits=Storage::AtlasUnits::garageNormalizationStarted;
    //The actual single atlas vector, frame order then U/V lane order. No
    //second normalized table or replacement numeric manager is allocated.
    //A failure retains earlier writes; the Scene callback forbids replay.
    for(auto& frame:storage_->frames)for(auto& corner:frame.sourceUv)for(unsigned lane=0;lane<2;++lane){
        if(!scalar(corner[lane]))return Status::arithmetic;
        volatile float normalized=corner[lane]/(lane?height:width);
        if(!scalar(normalized))return Status::arithmetic;
        corner[lane]=normalized;
    }
    storage_->atlasUnits=Storage::AtlasUnits::garageNormalized;return Status::complete;
}
const OriginalGcParticleFrame50KG* OriginalGcParticleManager50KG::frame(std::int32_t i) const noexcept {
    return storage_&&storage_->atlasUnits==Storage::AtlasUnits::pixels&&i>=0&&std::size_t(i)<storage_->frames.size()?&storage_->frames[std::size_t(i)]:nullptr;
}
const std::array<float,3>* OriginalGcParticleManager50KG::generatorFireAttachmentOffset() const noexcept {
    return gcFogAuthoringPrepared()?&storage_->fireOffset:nullptr;
}
OriginalGcParticleAuxiliaryChild50KG* OriginalGcParticleManager50KG::auxiliaryChild() noexcept {
    return storage_?&storage_->auxiliaryChild:nullptr;
}
const OriginalGcParticleAuxiliaryChild50KG* OriginalGcParticleManager50KG::auxiliaryChild() const noexcept {
    return storage_?&storage_->auxiliaryChild:nullptr;
}
std::uint32_t OriginalGcParticleManager50KG::registerEmitter(const OriginalGcParticleEmitterState50KG& e) noexcept {
    if(!storage_||storage_->emitterAllocation.terminal||!group(e.particleGroup)||
       storage_->emitters.size()>=originalGcParticleCapacity50KG)return 0;
    try{storage_->emitters.push_back(e);return static_cast<std::uint32_t>(storage_->emitters.size());}
    catch(...){storage_->emitterAllocation.reject(Status::allocation);return 0;}
}
std::size_t OriginalGcParticleManager50KG::retainedStoragePayloadBytes() const noexcept {
    if(!storage_)return 0;
    return sizeof(Storage)+storage_->groups.capacity()*sizeof(OriginalGcParticleGroup50KG)+
        storage_->frames.capacity()*sizeof(OriginalGcParticleFrame50KG)+storage_->atlasTextureMap.capacity()+1+
        storage_->emitters.capacity()*sizeof(OriginalGcParticleEmitterState50KG);
}
std::size_t OriginalGcParticleManager50KG::emitterRegistryReservationMetadataBytes() noexcept {
    // Charge the entire replacement vector descriptor rather than guessing
    // an ABI-specific difference from std::vector<Row>. Add a whole Storage
    // alignment for each inserted/replaced field, conservatively covering
    // inter-field/tail padding. No new heap control block is created.
    return sizeof(EmitterAllocation50KG)+sizeof(EmitterVector50KG)+2u*alignof(Storage);
}
OriginalGcEmitterRegistryReservation50KG OriginalGcParticleManager50KG::emitterRegistryReservation() const noexcept {
    OriginalGcEmitterRegistryReservation50KG result{};
    if(!storage_)return result;
    const auto& a=storage_->emitterAllocation;
    result.registeredEmitters=storage_->emitters.size();result.reservedEmitters=storage_->emitters.capacity();
    result.retainedPayloadBytes=a.live;result.peakPayloadBytes=a.peak;result.reservedPeakPayloadBytes=a.quota;
    result.allocationBound=a.bound;result.reservationAttempted=a.attempted;result.terminal=a.terminal;
    return result;
}
std::size_t OriginalGcParticleManager50KG::emitterRegistryPayloadReservationBytes() const noexcept {
    return storage_&&storage_->emitterAllocation.bound?storage_->emitterAllocation.quota:0u;
}
OriginalGcParticleStatus50KG OriginalGcParticleManager50KG::reserveAdditionalEmitters(
    std::size_t additional,std::size_t quota,OriginalGcEmitterRegistryReservation50KG& result) noexcept {
    result=emitterRegistryReservation();if(!storage_)return Status::unprepared;
    auto& s=*storage_;auto& a=s.emitterAllocation;
    if(a.attempted)return Status::alreadyPrepared;
    a.attempted=true;
    const auto fail=[&](Status status){a.reject(status);result=emitterRegistryReservation();return status;};
    // This API cannot certify the parent's shared ledger. The inherited
    // native 32 MiB maximum is only a final bound, never a second allowance.
    if(!quota||quota>32u*1024u*1024u||a.live>quota||
       s.emitters.size()>originalGcParticleCapacity50KG||
       additional>originalGcParticleCapacity50KG-s.emitters.size())
        return fail(Status::capacity);
    a.bound=true;a.quota=quota;a.peak=a.live;
    try {
        s.emitters.reserve(s.emitters.size()+additional);
        result=emitterRegistryReservation();return Status::complete;
    }catch(...){return fail(a.failure==Status::complete?Status::allocation:a.failure);}
}
std::size_t OriginalGcParticleManager50KG::emitterStoragePayloadBytes() const noexcept {
    return storage_?storage_->emitters.capacity()*sizeof(OriginalGcParticleEmitterState50KG):0u;
}
OriginalGcParticleEmitterState50KG* OriginalGcParticleManager50KG::emitter(std::uint32_t id) noexcept {
    return storage_&&id&&id<=storage_->emitters.size()?&storage_->emitters[id-1]:nullptr;
}
const OriginalGcParticleEmitterState50KG* OriginalGcParticleManager50KG::emitter(std::uint32_t id) const noexcept {
    return storage_&&id&&id<=storage_->emitters.size()?&storage_->emitters[id-1]:nullptr;
}
OriginalGcParticle50KG* OriginalGcParticleManager50KG::nextParticle() noexcept {
    return storage_&&storage_->count<storage_->order.size()?storage_->order[storage_->count]:nullptr;
}
bool OriginalGcParticleManager50KG::commitParticle(std::uint32_t id) noexcept {
    auto* p=nextParticle();if(!p||!emitter(id))return false;p->ownerId=id;++storage_->count;return true;
}
std::size_t OriginalGcParticleManager50KG::particleCount() const noexcept{return storage_?storage_->count:0;}
OriginalGcParticle50KG* OriginalGcParticleManager50KG::particleAt(std::size_t i) noexcept {
    return storage_&&i<storage_->count?storage_->order[i]:nullptr;
}
const OriginalGcParticle50KG* OriginalGcParticleManager50KG::particleAt(std::size_t i) const noexcept {
    return storage_&&i<storage_->count?storage_->order[i]:nullptr;
}
float drawOriginalGcParticleRandom50KG(std::uint32_t& raw) noexcept {
    raw=raw*0x41c64e6du+0x3039u;volatile float converted=static_cast<float>(raw&0x7fffffffu);
    return mul(0x1p-31f,converted);
}
bool seedOriginalGcParticleRandom50KG(std::uint32_t& raw,SharedRandomOwner50ID& shared,
    HostMonotonicClock50ID& clock) noexcept {
    if(!shared.seed.lazy80A8){shared.seed.seed3D6B30=0;shared.seed.lazy80A8=true;}
    if(shared.seed.seed3D6B30==0) {
        std::uint32_t timestamp=0;if(!sampleHostTimestamp50ID(clock,timestamp))return false;
        shared.seed.seed3D6B30=timestamp;raw=timestamp;
    }
    return true;
}
bool checkOriginalGcParticleRandomRange50KG(std::int32_t selected,std::int32_t first,
    std::int32_t last,SharedRandomOwner50ID& shared) noexcept {
    if(!shared.range.lazy80AC){shared.range.rangeCheck3D6B38=1;shared.range.lazy80AC=true;}
    return !shared.range.rangeCheck3D6B38||(selected>=first&&selected<=last);
}
OriginalGcParticleReceipt50KG OriginalGcParticleManager50KG::populateColdFog(std::uint32_t& raw,
    SharedRandomOwner50ID& random,HostMonotonicClock50ID& clock) noexcept {
    OriginalGcParticleReceipt50KG result{};
    if(!storage_)return result;
    if(!gcFogAuthoringPrepared()){result.status=Status::unsupported;return result;}
    if(std::fegetround()!=FE_TONEAREST){result.status=Status::arithmetic;return result;}
    auto& s=*storage_;if(s.fogPopulationAttempted){result.status=Status::alreadyPrepared;return result;}
    s.fogPopulationAttempted=true;const auto& f=s.fog;const auto& g=s.groups[0];
    for(std::size_t volume=0;volume<3;++volume) {
        // Source constructs each fog owner in TOB order, after Emitter1's
        // attached fire owners. Parent registers those owners before this call.
        OriginalGcParticleEmitterState50KG state{};state.particleGroup=f.group;
        state.fogPeriod=f.period;state.fogIncreasing=true;
        s.fogOwnerIds[volume]=registerEmitter(state);
        if(!s.fogOwnerIds[volume]){result.status=Status::allocation;return result;}
        const auto owner=s.fogOwnerIds[volume];
        // Source16055C retires only this emitter's previous particles. On the
        // admitted cold construction there are none. Other owners stay intact.
        for(std::size_t i=0;i<s.count;++i)if(s.order[i]->ownerId==owner) {
            s.order[i]->flags&=~0x80u;s.order[i]->ageMilliseconds=s.order[i]->lifeMilliseconds;
        }
        for(std::int32_t i=0;float(i)<f.count;++i) {
            auto* p=nextParticle();if(!p){result.poolFull=true;break;}
            const float x=drawOriginalGcParticleRandom50KG(raw),y=drawOriginalGcParticleRandom50KG(raw),z=drawOriginalGcParticleRandom50KG(raw);
            result.randomDraws+=3;
            const std::array<float,3> r{{x,y,z}};
            for(unsigned lane=0;lane<3;++lane)
                p->position[lane]=add(s.fogPositions[volume][lane],add(f.minRange[lane],mul(r[lane],sub(f.maxRange[lane],f.minRange[lane]))));
            p->positionW=1;
            if(f.period!=-1) {
                const auto dz=mul(f.magnitude,drawOriginalGcParticleRandom50KG(raw));
                const auto dy=mul(f.magnitude,drawOriginalGcParticleRandom50KG(raw));
                const auto dx=mul(f.magnitude,drawOriginalGcParticleRandom50KG(raw));
                p->velocity={{dx,dy,dz}};p->velocityW=1;
                for(unsigned lane=0;lane<3;++lane)
                    if(drawOriginalGcParticleRandom50KG(raw)>0.5f)p->velocity[lane]=mul(p->velocity[lane],-1);
                result.randomDraws+=6;
            }
            p->size=p->initialSize=f.size;p->lifeMilliseconds=f.life;p->ageMilliseconds=f.birthAge;
            p->frame=s.fogCurrentFrames[volume];p->inverseLife=divide(1,p->lifeMilliseconds);
            p->growth=0;p->flags=f.flags;
            if(x>0.5f&&f.mirrorX)p->flags|=0x4000u;
            if(y>0.5f&&f.mirrorY)p->flags|=0x8000u;
            if(!commitParticle(owner)){result.status=Status::storage;return result;}
            ++result.spawned;
            // Original increments population before lazy clock/seed and draw10.
            if(!seedOriginalGcParticleRandom50KG(raw,random,clock)){result.status=Status::clock;return result;}
            const float selected=mul(float(g.last-g.first+1),drawOriginalGcParticleRandom50KG(raw));++result.randomDraws;
            const auto next=g.first+static_cast<std::int32_t>(selected);
            if(!checkOriginalGcParticleRandomRange50KG(next,g.first,g.last,random)) {
                result.status=Status::sourceRangeAssertion;return result;
            }
            s.fogCurrentFrames[volume]=next;
        }
    }
    s.fogPopulated=true;result.status=Status::complete;return result;
}
bool OriginalGcParticleManager50KG::removeRestartParticles161F60(std::uint32_t& pc)noexcept {
    auto& r=restartRemoval_;
    // 161F60 runs on every call: 1A824C (each 1A8158 replay loop) and 1AC978 (every 1AC8E0
    // restart). Each invocation starts its own cursor; nothing carries over between calls.
    r={};r.attempted=true;pc=r.pc=0x161f60;
    if(!storage_)return false;
    auto& s=*storage_;
    r.indexA2=0;r.offsetA3=0; //161F68/70, before count128 read.
    for(;;){
        pc=r.pc=0x161fd8;
        if(s.count>s.order.size())return false;
        r.countT1=static_cast<std::uint32_t>(s.count);
        if(r.indexA2>=r.countT1)break; //signed source words within native5000 extent.
        pc=r.pc=0x161f7c;r.reachedT0=s.order[r.indexA2];
        // Native pointer identity at the reached dereference; no other particle data.
        const auto owns=[&s](const OriginalGcParticle50KG* q){
            for(const auto& slot:s.pool)if(q==&slot)return true;
            return false;
        };
        pc=r.pc=0x161f80;if(!owns(r.reachedT0))return false;
        r.flagsV1=r.reachedT0->flags;
        if(r.flagsV1&0x80u){
            r.offsetA3+=4; //BNEZL161F88 taken delay161F8C.
        }else{
            pc=r.pc=0x161f98;r.tailV1=s.order[r.countT1-1u];
            // Read pointer value only. No particle data/owner/age prevalidation.
            s.order[r.indexA2]=r.tailV1; //161F9C first pointer store.
            pc=r.pc=0x161fa0;
            if(!s.count||s.count>s.order.size())return false;
            s.order[s.count-1u]=r.reachedT0; //161FB0 second pointer store.
            --s.count; //161FBC count128 store, AFTER both pointer stores.
            pc=r.pc=0x161fc0;
            if(r.indexA2==s.count)break;
            r.offsetA3+=4; //161FD0. Swapped-in particle is NOT revisited.
        }
        ++r.indexA2; //161FD4, even after removal.
    }
    pc=r.pc=0x161fe8;r.returned=true;return true; //Auxiliary still required.
}
OriginalGcParticleReceipt50KG OriginalGcParticleManager50KG::advance(float dt,bool paused) noexcept {
    OriginalGcParticleReceipt50KG result{};if(!storage_)return result;
    // Original pause precedes atlas/material resolution and every age update.
    if(paused){result.status=Status::complete;return result;}
    if(std::fegetround()!=FE_TONEAREST){result.status=Status::arithmetic;return result;}
    if(!scalar(dt)||dt<0||!scalar(mul(1000,dt))){result.status=Status::delta;return result;}
    auto& s=*storage_;const float milliseconds=mul(1000,dt);
    //1612F8..131C visits the constructed auxiliary child before particles.
    // The admitted cold state is known dormant, not source-null. Enabling it
    // requires its real virtual-update owner; never silently skip that branch.
    if(!s.auxiliaryChild.constructed || s.auxiliaryChild.enabled18) {
        result.status=Status::unsupported;return result;
    }
    for(std::size_t i=0;i<s.count;++i) {
        auto* p=s.order[i];
        if(!(p->flags&0x80u)) {
            p->ageMilliseconds=add(p->ageMilliseconds,milliseconds);
            if(p->ageMilliseconds>p->lifeMilliseconds) {
                // Source swaps pointer entries, leaving the dead slot in the
                // free tail. The swapped-in live particle is NOT aged/rechecked.
                --s.count;++result.expired;std::swap(s.order[i],s.order[s.count]);
                if(i==s.count)break;p=s.order[i];
            }
        }
        if(!(p->flags&0x6acu))continue;
        const auto* e=emitter(p->ownerId);if(!e){result.status=Status::storage;return result;}
        if(p->flags&8u) {
            const float growth=mul(p->growth,dt);
            p->size=std::max(0.0f,add(p->size,growth));p->position[1]=add(p->position[1],growth);
        }
        if(p->flags&4u)for(unsigned lane=0;lane<3;++lane)p->position[lane]=add(p->position[lane],mul(dt,p->velocity[lane]));
        if(p->flags&0x20u)p->velocity[1]=sub(p->velocity[1],mul(mul(32.2f,e->gravity),dt));
        if((p->flags&0x80u)&&e->fogPeriod!=-1)
            for(unsigned lane=0;lane<3;++lane)p->position[lane]=add(p->position[lane],mul(e->fogPhaseRatio,p->velocity[lane]));
        if(p->flags&0x200u) {
            const float penetration=sub(p->position[1],e->bouncePlane);
            if(penetration<0&&p->velocity[1]<0) {
                for(float& v:p->velocity)v=mul(v,e->bounceFactor);
                p->velocity[1]=penetration<=-0.1f?-p->velocity[1]:0;
            }
        }
        if(p->flags&0x400u) {
            const auto* g=group(e->particleGroup);
            if(!g||!scalar(e->framesPerSet)||e->framesPerSet<=0||!scalar(e->inverseFramesPerSet)||e->inverseFramesPerSet<=0||p->lifeMilliseconds<=0) {
                result.status=Status::unsupported;return result;
            }
            const float denominator=mul(p->lifeMilliseconds,e->inverseFramesPerSet);
            const float phaseFloat=divide(p->ageMilliseconds,denominator);
            const float setFloat=mul(e->inverseFramesPerSet,float(p->frame-g->first));
            if(!scalar(phaseFloat)||!scalar(setFloat)||std::fabs(phaseFloat)>2147483000.f||std::fabs(setFloat)>2147483000.f) {
                result.status=Status::arithmetic;return result;
            }
            auto phase=static_cast<std::int32_t>(phaseFloat);if(phase>0)--phase;
            const auto set=static_cast<std::int32_t>(setFloat),frames=static_cast<std::int32_t>(e->framesPerSet);
            const auto frame=std::int64_t(g->first)+std::int64_t(set)*frames+phase;
            if(frame<std::numeric_limits<std::int32_t>::min()||frame>std::numeric_limits<std::int32_t>::max()) {
                result.status=Status::arithmetic;return result;
            }
            p->frame=static_cast<std::int32_t>(frame);
        }
    }
    result.status=Status::complete;return result;
}
OriginalGcParticleStatus50KG OriginalGcParticleManager50KG::advanceFogPhases(float dt) noexcept {
    if(!storage_)return Status::unprepared;
    if(!gcFogAuthoringPrepared())return Status::unsupported;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    if(!scalar(dt)||dt<0)return Status::delta;
    for(const auto id:storage_->fogOwnerIds) {
        auto* e=emitter(id);if(!e)return Status::storage;
        if(e->fogPeriod==-1)continue;
        e->fogPhase=e->fogIncreasing?add(e->fogPhase,dt):sub(e->fogPhase,dt);
        e->fogPhaseRatio=divide(e->fogPhase,e->fogPeriod);
        // Ratio is published BEFORE endpoint correction, exactly1601C8.
        if(e->fogPhase>=0) {
            if(e->fogPhaseRatio>1){e->fogIncreasing=false;e->fogPhase=sub(e->fogPeriod,divide(e->fogPeriod,1000));}
        }else if(e->fogPhaseRatio<-1) {
            e->fogIncreasing=true;e->fogPhase=add(-e->fogPeriod,divide(e->fogPeriod,1000));
        }
    }
    return Status::complete;
}
float OriginalGcParticleManager50KG::fogBrightness(const OriginalGcParticle50KG& p,
    const std::array<float,3>& camera) const noexcept {
    if(!gcFogAuthoringPrepared()||!(p.flags&0x80u))return std::numeric_limits<float>::quiet_NaN();
    const auto& f=storage_->fog;float distanceSquared=0;
    for(unsigned lane=0;lane<3;++lane){const auto d=sub(p.position[lane],camera[lane]);distanceSquared=add(distanceSquared,mul(d,d));}
    float birth=sub(1,mul(p.ageMilliseconds,p.inverseLife));
    if(distanceSquared<f.distanceSquared)birth=mul(birth,mul(distanceSquared,f.inverseDistanceSquared));
    return mul(birth,64);
}
bool OriginalGcParticleManager50KG::coldFogPopulated() const noexcept{return gcFogAuthoringPrepared()&&storage_->fogPopulated;}
} // namespace rr::game
