#pragma once
// Cold source-derived Pod01 vehicle+348 authoring. No captured RAM, guest
// execution, ground-height substitute, or hot loading. Runtime namespace and
// preserved owner bits are explicit inputs, not invented constructor defaults.
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/type2_bounds50by.h"
#include "../../src/game/terrain_prepare48m.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/pod_physics49n.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
    const char* archivePath="work/native/project/disc/PODS/POD01/POD01.RES") {
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
    std::ifstream file(archivePath,std::ios::binary);if(!file)return detail::fail(error,"actual Pod01 archive unavailable");
    std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file),{}},podM;
    assets::ResArchive48C archive;assets::DecodeError48C decode;
    if(!assets::decodeResource48C(bytes,archive,decode)||!assets::extractResMember48C(archive,"Pods\\Pod01\\podboxm.col",podM,decode)||podM.size()<92)
        return detail::fail(error,"actual Pod01 podboxm unavailable");
    const auto nr=detail::u32(podM.data()+80),nn=detail::u32(podM.data()+84);
    if(nr>32768||nn>32767||nr!=main690.resource->rayCount||nn!=main690.resource->nodeCount||
       (nr&&!main690.resource->rays)||(nn&&!main690.resource->nodes))return detail::fail(error,"690 resource extent differs from actual podboxm");
    std::vector<Type2Node49X> nodes(nn);std::vector<Type2Ray49X> rays(nr);
    std::vector<Type2OpaqueEndpointWords49X> opaque(nr);std::vector<std::uint32_t> topology(nn+nr);
    for(std::size_t i=0;i<nr;++i){opaque[i].first=std::bit_cast<std::uint32_t>(main690.resource->rays[i].first.v[3]);
        opaque[i].second=std::bit_cast<std::uint32_t>(main690.resource->rays[i].second.v[3]);}
    Type2ResourceView49X authentic{};
    if(decodeType2Resource49X(podM.data(),podM.size(),opaque.data(),opaque.size(),nodes.data(),nodes.size(),rays.data(),rays.size(),
       {topology.data(),topology.size()},authentic)!=Type2ResourceStatus49X::Complete||authentic.root!=main690.resource->root||
       authentic.maxDepth!=main690.resource->maxDepth||std::memcmp(authentic.localMatrix.v,main690.resource->localMatrix.v,sizeof(BodyMatrix48G))||
       std::memcmp(nodes.data(),main690.resource->nodes,nodes.size()*sizeof(Type2Node49X))||
       std::memcmp(rays.data(),main690.resource->rays,rays.size()*sizeof(Type2Ray49X)))return detail::fail(error,"690 is not actual native-decoded podboxm");
    Type2BoundsInput50BY input{};input.resource=&authentic;input.preparedWorld=detail::identity();CollectionBoundsResult48S bounds{};
    if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)return detail::fail(error,"source2121C8 podboxm bounds rejected");
    receipt.vehicle324=bounds.half.v[1];receipt.archiveBytes=bytes.size();receipt.podBoxMBytes=podM.size();
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
    auto& q=staged.metadata;q.filter.identity=binding.identity;q.filter.categoryA4=0;q.filter.acceptMaskB8=0xffffffffu;
    q.filter.flags31=static_cast<std::uint8_t>((binding.flags31&0xf0u)|3u);
    q.filter.flagsF8=static_cast<std::uint8_t>((binding.preservedFlagsF8&0xc0u)|6u);
    q.filter.identityLists[0].mode=q.filter.identityLists[1].mode=1;q.filter.categoryModeD4=0;q.filter.categoryMasks.push_back(binding.floorCategory);
    q.callbackMasks={binding.staticObjectCategory,binding.tubeCategory,binding.physicsBodyCategory};
    q.copied40=q.copied50={{0,0,0,1}};q.collisionTypeTag=binding.collisionTypeTag;q.groupTypeTag=binding.groupTypeTag;q.terrainCallbackEnabled=true;
    staged.prepared=true;out=std::move(staged);error.clear();return true;
}
}
