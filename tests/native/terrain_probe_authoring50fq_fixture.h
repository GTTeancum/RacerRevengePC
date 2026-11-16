#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "runtime_asset_paths50lb.h"
// Cold source-derived original-pod vehicle+348 authoring. No captured RAM, guest
// execution, ground-height substitute, or hot loading. Runtime namespace and
// preserved owner bits are explicit inputs, not invented constructor defaults.
#include "native_racer_collision50hw_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/type2_bounds50by.h"
#include "../../src/game/terrain_prepare48m.h"
#include "../../src/game/source_acos49t.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/pod_physics49n.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <optional>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rr::game {class OriginalRacerTerrainQueryOwner50LA;}
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace terrain_probe_authoring50fq_fixture {
using namespace rr::game;
namespace assets=rr::assets;
struct Bindings {
    // All identities/masks/tags must use the SAME native namespace as the live
    // collection and candidate owners. These are not historical guest values.
    std::uint32_t identity=0,physicsBodyCategory=0,staticObjectCategory=0;
    std::uint32_t floorCategory=0,tubeCategory=0;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    // Only flags31's high nibble survives fresh141300;212060 supplies a1=1,
    // giving low nibble3. F8 preserves only the high two constructor bits.
    std::uint8_t flags31=0,preservedFlagsF8=0;
};
using SourceBindings=Bindings;
struct Receipt {
    float authoredWeight=0,authoredLift=0,authoredOrientationSpring=0;
    float bodyMass=0,vehicle324=0;
    SpringVector4_48N probe130{},probe140{},localOffset1C0{};
    std::size_t archiveBytes=0,podBoxMBytes=0;
    std::uint32_t constructor=0x1b0ee0,setter=0x1b1240,loaderCall=0x21494c;
};
struct Owner {
    TerrainPrepareState48M initial{};
    TerrainFollowParameters48G follow{};
    Vector3_48C lowerEndpoint140{};
    // Canonical348+1F0 from the admitted1B1240 setup, distinct from the
    // vehicle7EC parameter and from the cold authoring receipt.
    float lift1F0=0;
    //Distinct348 lanes, unknown in the old projected owner. The counted
    //source constructor and separate same-row native1B1240 continuation own
    //their own writes. Known native values do not certify counted source348.
    std::optional<float> spring1F4,damping1F8,gravity1FC,sideForce200;
    TerrainQuery48J metadata{};
    Receipt receipt{};
    bool prepared=false;
};
namespace detail {
inline bool fail(std::string& error,const char* why){error=why;return false;}
inline std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
inline float add(float a,float b) noexcept {volatile float v=a+b;return v;}
inline float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
inline float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
inline float div(float a,float b) noexcept {volatile float v=a/b;return v;}
inline BodyMatrix48G identity() noexcept {BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;return m;}
inline std::string_view trim(std::string_view s) noexcept {
    while(!s.empty()&&(s.front()==' '||s.front()=='\t'||s.front()=='\r'))s.remove_prefix(1);
    while(!s.empty()&&(s.back()==' '||s.back()=='\t'||s.back()=='\r'))s.remove_suffix(1);return s;
}
inline bool equal(std::string_view a,std::string_view b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i){auto x=a[i],y=b[i];if(x>='A'&&x<='Z')x+=32;if(y>='A'&&y<='Z')y+=32;if(x!=y)return false;}return true;
}
inline bool authored(const std::vector<std::uint8_t>& phy,Receipt& r,std::string& error){
    unsigned section=0,seen=0;
    const std::string_view text(reinterpret_cast<const char*>(phy.data()),phy.size());
    for(std::size_t at=0;at<text.size();){auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();
        auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){if(line.size()<3||line.back()!=']')return fail(error,"invalid PHY section");
            const auto name=trim(line.substr(1,line.size()-2));section=equal(name,"Repulsor")?1u:equal(name,"Physical Properties")?2u:0u;continue;}
        const auto split=line.find('=');if(!section||split==std::string_view::npos)continue;
        const auto key=trim(line.substr(0,split));unsigned bit=0;float* destination=nullptr;
        if(section==1&&equal(key,"MaxLiftHeight")){bit=1;destination=&r.authoredLift;}
        if(section==1&&equal(key,"OrientationSpring")){bit=2;destination=&r.authoredOrientationSpring;}
        if(section==2&&equal(key,"Weight")){bit=4;destination=&r.authoredWeight;}
        if(!bit)continue;const auto token=trim(line.substr(split+1));
        if((seen&bit)||!assets::parsePodPhysicsNumber49N(token.data(),token.size(),*destination))return fail(error,"invalid or duplicate probe PHY value");seen|=bit;
    }
    return seen==7||fail(error,"missing probe Weight/MaxLiftHeight/OrientationSpring authoring");
}
// Constructor identity/inverse and107140 conversion preserve the source XYZ
// multiplication/addition order. Pair endpoint W is explicitly forced to one.
inline Vector3_48C identityPoint(const SpringVector4_48N& p) noexcept {
    const auto m=identity();Vector3_48C out{};
    for(unsigned i=0;i<3;++i)out.v[i]=add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),0.f);
    return out;
}
inline bool oneBit(std::uint32_t x) noexcept {return x&&!(x&(x-1));}
}

// main690 is the already-owned actual podboxm resource. Re-decode from the real
// archive and compare every node/ray/matrix field before using its constructor
// bounds. Opaque file-absent endpoint W words are preserved ONLY for comparison.
// Output is fresh cold state, not a reset of any moving owner. Identity, tags,
// preserved flags and collection attachment are application-owned boundaries.
inline bool prepare(Owner& out,const Type2StaticQuery49X& main690,
    const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
    const Bindings& binding,std::string& error,
    const char* archivePath="work/native/project/disc/PODS/POD01/POD01.RES",
    const char* mainMember="Pods\\Pod01\\podboxm.col",
    const native_racer_collision50HW::Owner* collisionOwner=nullptr) {
    if(out.prepared||std::fegetround()!=FE_TONEAREST||!main690.resource||!main690.queryScene.value||
       !binding.identity||(main690.filter&&binding.identity==main690.filter->identity)||
       binding.collisionTypeTag==binding.groupTypeTag)return detail::fail(error,"invalid fresh probe owner binding");
    const std::uint32_t masks[]={binding.physicsBodyCategory,binding.staticObjectCategory,binding.floorCategory,binding.tubeCategory};
    for(unsigned i=0;i<4;++i){if(!detail::oneBit(masks[i]))return detail::fail(error,"probe category registry mask is unbound");
        for(unsigned j=0;j<i;++j)if(masks[i]==masks[j])return detail::fail(error,"probe category registry names alias");}
    Owner staged{};
    if(!detail::authored(phy,staged.receipt,error))return false;
    auto& receipt=staged.receipt;
    receipt.bodyMass=parameters.bodies.main.body.mass;
    const auto conversion=std::bit_cast<float>(0x4200cccdu);
    const float expectedMass=detail::div(receipt.authoredWeight,conversion);
    if(!(expectedMass>0)||!std::isfinite(expectedMass)||
       std::bit_cast<std::uint32_t>(expectedMass)!=std::bit_cast<std::uint32_t>(receipt.bodyMass)||
       std::bit_cast<std::uint32_t>(receipt.authoredLift)!=std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC))
        return detail::fail(error,"canonical probe mass/lift differs from authored PHY");
    Type2ResourceView49X decodedResource{};
    const Type2ResourceView49X* authentic=nullptr;
    std::vector<Type2Node49X> nodes;std::vector<Type2Ray49X> rays;
    if(collisionOwner){
        // This stable owner has already decoded/validated the selected RES once.
        // Its exact borrowed identity prevents a different pod query from being
        // silently used to derive this racer's probe authoring.
        if(!collisionOwner->prepared()||main690.resource!=&collisionOwner->main())
            return detail::fail(error,"probe query is not the supplied original-pod collision owner");
        authentic=&collisionOwner->main();
        receipt.archiveBytes=collisionOwner->receipt().archiveBytes;
        receipt.podBoxMBytes=collisionOwner->receipt().memberBytes[0];
    }else{
        if(!archivePath||!mainMember)return detail::fail(error,"probe collision archive/member is unbound");
        std::ifstream file(rr::assets::dlc50DL::host(runtime_asset_paths50LB::resolve(archivePath)),std::ios::binary);if(!file)return detail::fail(error,"actual pod archive unavailable");
        std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file),{}},podM;
        assets::ResArchive48C archive;assets::DecodeError48C decode;
        if(!assets::decodeResource48C(bytes,archive,decode)||!assets::extractResMember48C(archive,mainMember,podM,decode)||podM.size()<92)
            return detail::fail(error,"actual podboxm unavailable");
        const auto nr=detail::u32(podM.data()+80),nn=detail::u32(podM.data()+84);
        if(nr>32768||nn>32767||nr!=main690.resource->rayCount||nn!=main690.resource->nodeCount||
           (nr&&!main690.resource->rays)||(nn&&!main690.resource->nodes))return detail::fail(error,"690 resource extent differs from actual podboxm");
        nodes.resize(nn);rays.resize(nr);
        std::vector<Type2OpaqueEndpointWords49X> opaque(nr);std::vector<std::uint32_t> topology(nn+nr);
        for(std::size_t i=0;i<nr;++i){opaque[i].first=std::bit_cast<std::uint32_t>(main690.resource->rays[i].first.v[3]);
            opaque[i].second=std::bit_cast<std::uint32_t>(main690.resource->rays[i].second.v[3]);}
        if(decodeType2Resource49X(podM.data(),podM.size(),opaque.data(),opaque.size(),nodes.data(),nodes.size(),rays.data(),rays.size(),
           {topology.data(),topology.size()},decodedResource)!=Type2ResourceStatus49X::Complete||decodedResource.root!=main690.resource->root||
           decodedResource.maxDepth!=main690.resource->maxDepth||std::memcmp(decodedResource.localMatrix.v,main690.resource->localMatrix.v,sizeof(BodyMatrix48G))||
           std::memcmp(nodes.data(),main690.resource->nodes,nodes.size()*sizeof(Type2Node49X))||
           std::memcmp(rays.data(),main690.resource->rays,rays.size()*sizeof(Type2Ray49X)))return detail::fail(error,"690 is not actual native-decoded podboxm");
        authentic=&decodedResource;receipt.archiveBytes=bytes.size();receipt.podBoxMBytes=podM.size();
    }
    Type2BoundsInput50BY input{};input.resource=authentic;input.preparedWorld=detail::identity();CollectionBoundsResult48S bounds{};
    if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)return detail::fail(error,"source2121C8 podboxm bounds rejected");
    receipt.vehicle324=bounds.half.v[1];
    //214948 supplies f12=7EC;1B1270 publishes it to this actual348 owner.
    // prepare has already verified selected-body binding and authored lift.
    staged.lift1F0=receipt.authoredLift;
    const float offset[3]={0,receipt.vehicle324,0},up[3]={0,1,0};
    const float fiveLift=detail::mul(5.f,receipt.authoredLift);
    for(unsigned i=0;i<3;++i){
        receipt.probe130.v[i]=detail::add(parameters.bodies.main.centerOffset.v[i],offset[i]);
        const float base=detail::sub(receipt.probe130.v[i],detail::mul(2.f,offset[i]));
        receipt.probe140.v[i]=detail::sub(base,detail::mul(fiveLift,up[i]));
        receipt.localOffset1C0.v[i]=detail::sub(base,detail::mul(receipt.authoredLift,up[i]));
        if(!std::isfinite(receipt.probe130.v[i])||!std::isfinite(receipt.probe140.v[i])||!std::isfinite(receipt.localOffset1C0.v[i]))
            return detail::fail(error,"probe authored endpoint arithmetic rejected");
        staged.lowerEndpoint140.v[i]=receipt.probe140.v[i];staged.follow.localOffset1c0.v[i]=receipt.localOffset1C0.v[i];
    }
    receipt.probe130.v[3]=receipt.probe140.v[3]=receipt.localOffset1C0.v[3]=1;
    staged.follow.airborneTorque204=std::fabs(detail::mul(conversion,detail::mul(receipt.authoredOrientationSpring,receipt.bodyMass)));
    if(!std::isfinite(staged.follow.airborneTorque204))return detail::fail(error,"probe authored torque arithmetic rejected");
    staged.follow.detachAngle208=std::bit_cast<float>(0x3f1c61a9u);staged.follow.detachDistance20c=5.f;
    staged.initial.segment.first=detail::identityPoint(receipt.probe130);staged.initial.segment.second=detail::identityPoint(receipt.probe140);
    staged.initial.segment.worldMatrix=detail::identity();
    //1B106C..1B1098: initial normal170=up; prior normal190 and point180=zero.
    staged.initial.terrain.surface.normal170={{0,1,0}};
    auto& q=staged.metadata;
    //1B1468/1B16CC pass1 to11C3F0;11C524 stores the actual348 shape+54.
    q.normalMode54=1u;q.normalMode54Bound=true;
    q.filter.identity=binding.identity;q.filter.categoryA4=0;q.filter.acceptMaskB8=0xffffffffu;
    q.filter.flags31=static_cast<std::uint8_t>((binding.flags31&0xf0u)|3u);
    q.filter.flagsF8=static_cast<std::uint8_t>((binding.preservedFlagsF8&0xc0u)|6u);
    q.filter.identityLists[0].mode=q.filter.identityLists[1].mode=1;q.filter.categoryModeD4=0;q.filter.categoryMasks.push_back(binding.floorCategory);
    q.callbackMasks={binding.staticObjectCategory,binding.tubeCategory,binding.physicsBodyCategory};
    q.copied40=q.copied50={{0,0,0,1}};q.collisionTypeTag=binding.collisionTypeTag;q.groupTypeTag=binding.groupTypeTag;q.terrainCallbackEnabled=true;
    staged.prepared=true;out=std::move(staged);error.clear();return true;
}
// Preferred heterogeneous cold path: reuse the authenticated resource owner;
// do not reload/decode its archive or replace any ray endpoint W words.
inline bool prepare(Owner& out,const Type2StaticQuery49X& main690,
    const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
    const Bindings& binding,std::string& error,
    const native_racer_collision50HW::Owner& collisionOwner) {
    return prepare(out,main690,parameters,phy,binding,error,nullptr,nullptr,&collisionOwner);
}

// Install a different fresh racer into an already-sealed execution arena.
// Vector-backed filter lists are immutable shared namespace bindings and must
// match exactly. Only registered POD fields change; their addresses stay put.
// Cold slot initialization only, never reset a moving/live racer this way.
inline bool installProbeCold(Owner& arena,live_terrain_probe50fr::Owner& live,
                            const Owner& authored,std::string& error) {
    if(!arena.prepared||!authored.prepared)
        return detail::fail(error,"unprepared cold terrain-probe installation");
    const auto& a=arena.metadata;const auto& b=authored.metadata;
    const auto& af=a.filter;const auto& bf=b.filter;
    if(af.identity!=bf.identity||af.categoryA4!=bf.categoryA4||af.acceptMaskB8!=bf.acceptMaskB8||
       af.categoryModeD4!=bf.categoryModeD4||af.flags31!=bf.flags31||af.categoryMasks!=bf.categoryMasks||
       a.callbackMasks.collect!=b.callbackMasks.collect||a.callbackMasks.forceAngle!=b.callbackMasks.forceAngle||
       a.callbackMasks.excludeOwner!=b.callbackMasks.excludeOwner||a.collisionTypeTag!=b.collisionTypeTag||
       a.groupTypeTag!=b.groupTypeTag||a.terrainCallbackEnabled!=b.terrainCallbackEnabled||
       a.copied40!=b.copied40||a.copied50!=b.copied50)
        return detail::fail(error,"cold terrain-probe immutable namespace differs");
    for(unsigned i=0;i<2;++i)
        if(af.identityLists[i].mode!=bf.identityLists[i].mode||
           af.identityLists[i].identities!=bf.identityLists[i].identities)
            return detail::fail(error,"cold terrain-probe identity lists differ");
    arena.initial=authored.initial;arena.follow=authored.follow;
    arena.lift1F0=authored.lift1F0;
    //These are per-slot knownness lanes. Cold authoring does not synthesize
    //source values, and installing a peer must not inherit another slot's.
    arena.spring1F4=authored.spring1F4;arena.damping1F8=authored.damping1F8;
    arena.gravity1FC=authored.gravity1FC;arena.sideForce200=authored.sideForce200;
    arena.lowerEndpoint140=authored.lowerEndpoint140;arena.receipt=authored.receipt;
    arena.prepared=authored.prepared;arena.metadata.filter.flagsF8=authored.metadata.filter.flagsF8;
    // initialize() has no fallible branch for an empty owner. Initialize at its
    // final arena address, so resource.rays borrows live.pair, never a temporary.
    live={};live_terrain_probe50fr::initialize(live,arena.initial.segment);
    error.clear();return true;
}

// Source-staged native path only. Immutable reservation is NOT1B1240, a
// constructed collision shape, or a prepared numeric probe. The legacy public
// prepare/install API above is unchanged. Only the same-world bridge can use
// this prerequisite; it does not confer Session/grid/query readiness.
class SourceStage348 final {
    friend class ::rr::game::OriginalRacerTerrainQueryOwner50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    Owner* owner_=nullptr;
    live_terrain_probe50fr::Owner* live_=nullptr;
    const Type2StaticQuery49X* main_=nullptr;
    const native_racer_collision50HW::Owner* collision_=nullptr;
    const VehicleFrameParameters49W* parameters_=nullptr; //address only until21494C
    Bindings binding_{};
    Receipt immutable_{};
    const std::vector<std::uint8_t>* phy_=nullptr; //retained immutable roster authoring
    float authoredSpring_=0,authoredDamping_=0,authoredSideForce_=0;
    bool reserved_=false,constructorKnown_=false,sideWritten_=false,gravityWritten_=false;
    bool withdrawn_=false; //One-way counted owner retirement; no numeric fallback.
    bool setupAttempted_=false,setupApplied_=false;
    static bool authoredSetup(const std::vector<std::uint8_t>& phy,float& spring,float& damping,
        float& side,std::string& error){
        const std::string_view text(reinterpret_cast<const char*>(phy.data()),phy.size());
        bool repulsor=false;unsigned seen=0;
        for(std::size_t at=0;at<text.size();){auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();
            auto line=detail::trim(text.substr(at,end-at));at=end+1;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){repulsor=line.size()>2&&line.back()==']'&&
                detail::equal(detail::trim(line.substr(1,line.size()-2)),"Repulsor");continue;}
            const auto split=line.find('=');if(!repulsor||split==std::string_view::npos)continue;
            const auto key=detail::trim(line.substr(0,split));float* value=nullptr;unsigned bit=0;
            if(detail::equal(key,"Spring")){value=&spring;bit=1;}
            if(detail::equal(key,"Damping")){value=&damping;bit=2;}
            if(detail::equal(key,"SideForceCoefficient")){value=&side;bit=4;}
            if(!bit)continue;const auto token=detail::trim(line.substr(split+1));
            if((seen&bit)||!assets::parsePodPhysicsNumber49N(token.data(),token.size(),*value))
                return detail::fail(error,"invalid/duplicate source Repulsor setup lane");
            seen|=bit;
        }
        return seen==7||detail::fail(error,"source Repulsor setup lanes absent");
    }

    static bool validNamespace(const Bindings& b)noexcept {
        if(!b.identity||b.collisionTypeTag==b.groupTypeTag)return false;
        const std::uint32_t masks[]={b.physicsBodyCategory,b.staticObjectCategory,b.floorCategory,b.tubeCategory};
        for(unsigned i=0;i<4;++i){if(!detail::oneBit(masks[i]))return false;
            for(unsigned j=0;j<i;++j)if(masks[i]==masks[j])return false;}
        return true;
    }
    static bool namespaceCurrent(const Owner& owner,const Bindings& b)noexcept {
        const auto& q=owner.metadata;const auto& f=q.filter;
        return f.identity==b.identity&&f.categoryA4==0&&f.acceptMaskB8==0xffffffffu&&
            f.flags31==static_cast<std::uint8_t>((b.flags31&0xf0u)|3u)&&
            f.flagsF8==static_cast<std::uint8_t>((b.preservedFlagsF8&0xc0u)|6u)&&
            f.identityLists[0].mode==1&&f.identityLists[1].mode==1&&
            f.identityLists[0].identities.empty()&&f.identityLists[1].identities.empty()&&
            f.categoryModeD4==0&&f.categoryMasks.size()==1&&f.categoryMasks[0]==b.floorCategory&&
            q.callbackMasks.collect==b.staticObjectCategory&&q.callbackMasks.forceAngle==b.tubeCategory&&
            q.callbackMasks.excludeOwner==b.physicsBodyCategory&&q.collisionTypeTag==b.collisionTypeTag&&
            q.groupTypeTag==b.groupTypeTag&&q.terrainCallbackEnabled&&
            q.copied40==std::array<float,4>{{0,0,0,1}}&&q.copied50==std::array<float,4>{{0,0,0,1}};
    }
    // Fresh source-arena factory ONLY, before Bank registration. Allocate the
    // immutable namespace/list schema without publishing numeric setup or54.
    // These planned lanes cannot be consumed: prepared remains false.
    static bool allocateNamespace(Owner& out,const Bindings& b,std::string& error){
        if(!validNamespace(b)||out.prepared||out.receipt.archiveBytes||out.metadata.filter.identity||
           out.metadata.normalMode54Bound||!out.metadata.filter.categoryMasks.empty()||
           !out.metadata.filter.identityLists[0].identities.empty()||
           !out.metadata.filter.identityLists[1].identities.empty())
            return detail::fail(error,"source348 requires a genuinely fresh pre-Bank arena");
        TerrainQuery48J q{};
        q.filter.identity=b.identity;q.filter.categoryA4=0;q.filter.acceptMaskB8=0xffffffffu;
        q.filter.flags31=static_cast<std::uint8_t>((b.flags31&0xf0u)|3u);
        q.filter.flagsF8=static_cast<std::uint8_t>((b.preservedFlagsF8&0xc0u)|6u);
        q.filter.identityLists[0].mode=q.filter.identityLists[1].mode=1;
        q.filter.categoryModeD4=0;q.filter.categoryMasks.push_back(b.floorCategory);
        q.callbackMasks={b.staticObjectCategory,b.tubeCategory,b.physicsBodyCategory};
        q.copied40=q.copied50={{0,0,0,1}};
        q.collisionTypeTag=b.collisionTypeTag;q.groupTypeTag=b.groupTypeTag;q.terrainCallbackEnabled=true;
        out.metadata=std::move(q);error.clear();return true;
    }
    bool reserve(Owner& owner,live_terrain_probe50fr::Owner& live,
        const Type2StaticQuery49X& main,const native_racer_collision50HW::Owner& collision,
        const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
        const Bindings& binding,std::string& error){
        if(reserved_||owner.prepared||owner.receipt.archiveBytes||live.initialized||
           owner.metadata.normalMode54Bound||!namespaceCurrent(owner,binding)||
           !validNamespace(binding)||!collision.prepared()||main.resource!=&collision.main()||
           !main.queryScene.value||!main.filter||main.filter->identity==binding.identity)
            return detail::fail(error,"source348 reservation differs from fresh same-owner query allocation");
        Receipt immutable;
        float spring=0,damping=0,side=0;
        if(!detail::authored(phy,immutable,error)||!authoredSetup(phy,spring,damping,side,error))return false;
        Type2BoundsInput50BY input{};input.resource=&collision.main();input.preparedWorld=detail::identity();
        CollectionBoundsResult48S bounds{};
        if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)
            return detail::fail(error,"source348 immutable podboxm bounds rejected");
        immutable.vehicle324=bounds.half.v[1];
        immutable.archiveBytes=collision.receipt().archiveBytes;immutable.podBoxMBytes=collision.receipt().memberBytes[0];
        if(!immutable.archiveBytes||!immutable.podBoxMBytes)return detail::fail(error,"source348 asset receipt absent");
        //No parameter value, body mass,7EC, or endpoint is read here.
        authoredSpring_=spring;authoredDamping_=damping;authoredSideForce_=side;
        phy_=&phy;binding_=binding;immutable_=immutable;owner_=&owner;live_=&live;
        main_=&main;collision_=&collision;parameters_=&parameters;reserved_=true;
        error.clear();return true;
    }
    //Actual counted OriginalRacerTerrainQueryOwner50LA now writes these
    //constructor lanes at their original positions. No numerical-only shortcut.
    bool writeSideForce2148F0(float value)noexcept {
        if(withdrawn_||!constructorKnown_||sideWritten_||setupAttempted_||!std::isfinite(value)||
           std::bit_cast<std::uint32_t>(value)!=std::bit_cast<std::uint32_t>(authoredSideForce_))return false;
        owner_->sideForce200=value;sideWritten_=true;return true;
    }
    bool writeGravity214918(float actual378)noexcept {
        if(withdrawn_||!constructorKnown_||!sideWritten_||gravityWritten_||setupAttempted_||!std::isfinite(actual378))return false;
        owner_->gravity1FC=actual378;gravityWritten_=true;return true;
    }
    //Source-ordered1B1240 positive-body path. ShapePublication is the bridge's
    //concrete same-row11C3F0 resource/result owner, not a readiness callback.
    //Immutable/schema admission precedes the first setter store. Arithmetic
    //and shape failures retain the reached actual store prefix unready.
    template<class ShapePublication>
    bool applySetup21494C(Owner& owner,live_terrain_probe50fr::Owner& live,
        const Type2StaticQuery49X& main,const VehicleFrameParameters49W& parameters,
        float actual324,float actualLift7EC,float sourceSpring,float sourceDamping,float sourceOrientation,
        ShapePublication&& publishShape,std::string& error){
        if(withdrawn_||!reserved_||!constructorKnown_||!sideWritten_||!gravityWritten_||setupAttempted_||setupApplied_||
           owner_!=&owner||live_!=&live||main_!=&main||parameters_!=&parameters||!collision_||
           main.resource!=&collision_->main()||owner.prepared||live.initialized||
           owner.metadata.normalMode54Bound||!namespaceCurrent(owner,binding_)||
           std::bit_cast<std::uint32_t>(actual324)!=std::bit_cast<std::uint32_t>(immutable_.vehicle324)||
           std::bit_cast<std::uint32_t>(actualLift7EC)!=std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC)||
           std::bit_cast<std::uint32_t>(actualLift7EC)!=std::bit_cast<std::uint32_t>(immutable_.authoredLift)||
           std::bit_cast<std::uint32_t>(sourceSpring)!=std::bit_cast<std::uint32_t>(authoredSpring_)||
           std::bit_cast<std::uint32_t>(sourceDamping)!=std::bit_cast<std::uint32_t>(authoredDamping_)||
           std::bit_cast<std::uint32_t>(sourceOrientation)!=std::bit_cast<std::uint32_t>(immutable_.authoredOrientationSpring))
            return detail::fail(error,"source348 setup requires untouched same-owner and actual source argument lanes");
        //Complete-authored-input slice only: absent PHY keys are rejected by
        //reserve, not replaced with projected defaults or the ctor10 fallback.
        if(std::fegetround()!=FE_TONEAREST)return detail::fail(error,"source348 rounding mode unsupported");
        const auto conversion=std::bit_cast<float>(0x4200cccdu);
        if(std::bit_cast<std::uint32_t>(parameters.bodies.main.body.mass)!=
           std::bit_cast<std::uint32_t>(detail::div(immutable_.authoredWeight,conversion)))
            return detail::fail(error,"source348 actual mass differs from retained source input");
        setupAttempted_=true; //No replay after any actual setter prefix.
        owner.lift1F0=actualLift7EC;                           //1B1270
        const float mass=parameters.bodies.main.body.mass;
        const float spring=std::fabs(detail::mul(conversion,detail::mul(sourceSpring,mass)));
        if(!std::isfinite(spring))return detail::fail(error,"source348 spring prefix retained");
        owner.spring1F4=spring;                              //1B12A4
        const float torque=std::fabs(detail::mul(conversion,detail::mul(sourceOrientation,mass)));
        if(!std::isfinite(torque))return detail::fail(error,"source348 torque prefix retained");
        owner.follow.airborneTorque204=torque;                //1B12CC
        float damping=0.f;
        if(mass>0.f&&spring>0.f){
            const float product=detail::mul(mass,spring);
            const float twice=detail::mul(2.f,sourceDamping);std::uint32_t result=0;
            if(!SourceTerrainSetupMath50LA::damping1B12FC(std::bit_cast<std::uint32_t>(product),
                std::bit_cast<std::uint32_t>(twice),result))return detail::fail(error,"source348 damping prefix retained");
            damping=std::bit_cast<float>(result);
        }
        owner.damping1F8=damping;                            //1B1338/3C
        const float offset[3]={0,actual324,0},up[3]={0,1,0};
        SpringVector4_48N first{},second{},local{};
        for(unsigned i=0;i<3;++i){
            first.v[i]=detail::add(parameters.bodies.main.centerOffset.v[i],offset[i]);
            if(!std::isfinite(first.v[i]))return detail::fail(error,"source348 first endpoint prefix retained");
        }
        first.v[3]=1;owner.receipt.probe130=first;              //1B1390
        const float fiveLift=detail::mul(5.f,actualLift7EC);
        for(unsigned i=0;i<3;++i){
            const float base=detail::sub(first.v[i],detail::mul(2.f,offset[i]));
            second.v[i]=detail::sub(base,detail::mul(fiveLift,up[i]));
            if(!std::isfinite(second.v[i]))return detail::fail(error,"source348 second endpoint prefix retained");
        }
        second.v[3]=1;owner.receipt.probe140=second;            //1B145C
        for(unsigned i=0;i<3;++i)owner.lowerEndpoint140.v[i]=second.v[i];
        owner.initial.segment.first=detail::identityPoint(first);
        owner.initial.segment.second=detail::identityPoint(second);
        owner.initial.segment.worldMatrix=detail::identity();
        if(!publishShape(owner.initial.segment))return detail::fail(error,"source348 actual11C3F0 prefix retained");
        owner.metadata.normalMode54=1u;owner.metadata.normalMode54Bound=true;
        owner.metadata.terrainCallbackEnabled=true;           //1B1478/7C represented typed self callback
        for(unsigned i=0;i<3;++i){
            const float base=detail::sub(first.v[i],detail::mul(2.f,offset[i]));
            local.v[i]=detail::sub(base,detail::mul(actualLift7EC,up[i]));
            if(!std::isfinite(local.v[i]))return detail::fail(error,"source348 local offset prefix retained");
        }
        local.v[3]=1;
        for(unsigned i=0;i<3;++i)owner.follow.localOffset1c0.v[i]=local.v[i]; //1B1540
        owner.receipt=immutable_; //Native diagnostic only, retain actual endpoints.
        owner.receipt.bodyMass=mass;owner.receipt.probe130=first;
        owner.receipt.probe140=second;owner.receipt.localOffset1C0=local;
        owner.prepared=true;setupApplied_=true;error.clear();return true;
    }
};

}
