#include "original_gc_gameplay_lighting_candidate.h"
#include "../assets/track_start50ad.h"
#include "../assets/original_light_pose_defaults_candidate.h"
#include "../assets/zone_search_authoring50cn.h"
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <utility>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "GC source lighting candidate requires strict finite IEEE arithmetic"
#endif

namespace rr::game::original_gc_gameplay_lighting_candidate {
namespace {
namespace light=original_racer_lighting50KM;
bool fail(Diagnostic& d,Status s,std::uint32_t pc) noexcept {d.status=s;d.sourcePC=pc;return false;}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
bool normalizeOnce(light::Vec3& v) noexcept {
    const float square=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if(!std::isfinite(square)||square<std::numeric_limits<float>::min())return false;
    // Explicit ordinary native binary32 normalization. This is not a VU RSQRT
    // exceptional-mode/bit-parity claim. GC scene reader does NOT normalize first.
    volatile float inverse=1.f/std::sqrt(square);
    for(float& x:v){x=mul(x,inverse);if(!std::isfinite(x))return false;}
    return true;
}
const assets::ParameterBlock48F* block(const assets::ParameterDocument48F& p,const char* name) noexcept {
    const assets::ParameterBlock48F* found=nullptr;
    for(const auto& b:p.blocks)if(b.name==name){if(found)return nullptr;found=&b;}
    return found;
}
const assets::ParameterField48F* field(const assets::ParameterBlock48F& b,const char* name) noexcept {
    const assets::ParameterField48F* found=nullptr;
    for(const auto& f:b.fields)if(f.name==name){if(found)return nullptr;found=&f;}
    return found;
}
bool textField(const assets::ParameterBlock48F& b,const char* key,const char* text) noexcept {
    const auto* f=field(b,key);return f&&f->value==text;
}
bool triple(const assets::ParameterBlock48F& b,const char* key,light::Vec3& out) noexcept {
    const auto* f=field(b,key);if(!f)return false;std::size_t at=0;
    for(unsigned c=0;c<3;++c){const auto comma=f->value.find(',',at);
        if((c<2)==(comma==std::string::npos))return false;
        const auto end=c<2?comma:f->value.size();
        auto first=at,last=end;
        while(first<last&&f->value[first]==' ')++first;
        while(last>first&&f->value[last-1]==' ')--last;
        if(!assets::parseTrackNumber50AD(f->value.data()+first,last-first,out[c]))return false;
        at=end+1;
    }
    return true;
}
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
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
bool authenticScb(const std::vector<std::uint8_t>& input,bool ta,bool ga) noexcept {
    if ((ta&&ga)||input.size() != (ga?292650u:ta?130511u:359830u)) return false;
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; input.size() - at >= 64; at += 64) shaBlock(input.data() + at, h);
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = input.size() - at;
    if (remainder) std::memcpy(tail.data(), input.data() + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(input.size()) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    const char* expected = ga ?
        "ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7" : ta ?
        "c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d" :
        "d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d";
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u]) return false;
    return true;
}
bool validRoot(SceneNodeIndex49P root,std::uint32_t count) noexcept {
    const auto i=static_cast<std::int32_t>(root);return i>=0&&static_cast<std::uint32_t>(i)<count;
}
std::array<SceneNodeIndex49P,3> modelRoots(const VehicleFrameBindings49W& b) noexcept {
    return {b.lifecycle.model1B4,b.visual.left1BC,b.visual.right1B8};
}
bool frameBinding(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b) noexcept {
    if(!validVehicleSceneCount49W(s,b)||!validRoot(b.lifecycle.main190,s.sceneNodeCount))return false;
    const auto roots=modelRoots(b);
    for(auto root:roots)if(!validRoot(root,s.sceneNodeCount))return false;
    return roots[0]==b.animation.chariot1B4&&roots[1]==b.animation.left1BC&&
        roots[1]==b.lifecycle.left1BC&&roots[2]==b.animation.right1B8&&roots[2]==b.lifecycle.right1B8&&
        roots[0]!=roots[1]&&roots[0]!=roots[2]&&roots[1]!=roots[2];
}
LiveZoneBounds50EN shapeBounds(const PodShapeState49W& shape,const PodShapeSource49W& source) noexcept {
    LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=0;bounds.bounds.shapeWorld=shape.composed00;
    bounds.bounds.cachedCenter=shape.center60;bounds.bounds.cachedRadius=shape.radius80;
    bounds.type0Resource=&source;return bounds;
}
}

struct Owner::Storage {
    // Frozen authenticated authored TA session, deliberately not a projection
    // of source Light+D0 or LightManager+88. No updates are invented.
    struct AuthoredSpot {
        light::Vec3 position{},look{},rgb{},attenuation{};
        float innerCos=0,outerCos=0;
        std::uint32_t zone=0;
    };
    bool taNative=false,gaNative=false;
    std::uint32_t spotCount=0;
    std::array<AuthoredSpot,36> spots{};
    light::Vec3 sunLook{};
    std::array<light::Vec4,gcZones> sun44{};
    bool prepareTa(Diagnostic&);
    bool prepareGa(Diagnostic&);
    bool selectTa(const LiveZoneMembership50EN&,const light::Vec3&,
        light::LightMatrices&,Diagnostic&) const noexcept;

    assets::World48F world;
    light::SmallDirectionalRegistry registry{};
    std::array<light::Vec4,gcZones> ambient60{};
    light::Vec4 globalAmbient50{};
    bool fixedGcGlobalAmbient=false;
    ResetVisibilityWorld50EY visibility{};
    std::array<SearchStorage50BS,gcZones*2> registries{},scratchRegistries{};
    std::array<ZoneRegistration50CJ,maximumRacers+1u> registrations{};
    std::array<CollectionTree50BX,gcZones*2> methods{};
    std::array<CollectionZone50BX,gcZones> zones{};
    std::array<SearchListBlock50BR,gcZones*4> blocks{};
    std::array<CollectionRef50BX,gcZones> portalHeads{};
    std::array<LiveZonePortalLink50EN,maximumPortals*2> portalLinks{},scratchPortalLinks{};
    std::array<PortalGeometry50EO,maximumPortals> portalGeometry{};
    std::array<CollectionZoneLink50BX,maximumPortals+1> scratchLinks{};
    std::array<LiveZoneEntry50EN,maximumPortals+1> scratchEntries{};
    LiveZoneWorkspace50EN workspace{};
    struct Vehicle {
        LiveZoneMembership50EN member{};
        std::array<CollectionZoneLink50BX,maximumPortals+1> links{};
        std::array<LiveZoneEntry50EN,maximumPortals+1> entries{};
        PodShapeSource49W source{};
        PodShapeState49W preparedPrefix{};
        SceneNodeIndex49P mainRoot=SceneNodeIndex49P::none;
        std::array<SceneNodeIndex49P,3> roots{};
        std::uint64_t graphicsSequence=0;
        bool pendingInitial=false,attached=false;
    };
    std::array<Vehicle,maximumRacers> vehicles{};
    std::array<light::LightMatrices,3> selected{};
    std::array<bool,3> selectedReady{};
    std::uint32_t racerCount=0;
    std::uint64_t drawGeneration=0;
    const DrawLease* activeLease=nullptr;
    const CameraVisibilityLease* activeCamera=nullptr;
    std::uint64_t cameraGeneration=0;
    bool bound(std::uint32_t slot) const noexcept {
        if(slot>=racerCount)return false;
        const auto& v=vehicles[slot];const auto& m=v.member;
        if(!v.attached||!m.attached||m.object!=slot+1u||m.method!=ZoneMethod50CL::Second||
           m.storage.links!=v.links.data()||m.storage.entries!=v.entries.data()||
           m.storage.capacity!=v.links.size()||!v.entries[0].used)return false;
        const auto zone=v.links[0].zone;if(!zone||zone>world.zones.size())return false;
        const auto& r=registrations[slot];const auto& e=v.entries[0].registration;
        return r.owner==&registries[(zone-1u)*2u+1u]&&r.encodedObject==((slot+1u)<<2u)&&
            r.owner==e.owner&&r.slot==e.slot&&r.encodedObject==e.encodedObject;
    }
};
bool Owner::Storage::prepareTa(Diagnostic& d) {
    spotCount=17;
    const auto& params=world.parameters;
    const auto* lights=block(params,"lights");
    if(!lights||!textField(*lights,"numberoflights","26"))return fail(d,Status::sourceFields,0x2783d0);
    auto scalar=[](const assets::ParameterBlock48F& b,const char* key,float& value){
        const auto* f=field(b,key);
        return f&&assets::parseTrackNumber50AD(f->value.data(),f->value.size(),value);
    };
    auto zoneOf=[&](const assets::ParameterBlock48F& b,std::uint32_t& zone){
        float raw=1.f;
        if(field(b,"initialzone")&&!scalar(b,"initialzone",raw))return false;
        if(raw<1.f||raw>world.zones.size()||std::trunc(raw)!=raw)return false;
        zone=static_cast<std::uint32_t>(raw);return true;
    };
    // Source construction order is numeric Light1..Light26, never lexical.
    for(unsigned i=1;i<=26;++i){
        const auto name="light"+std::to_string(i);const auto* b=block(params,name.c_str());
        if(!b||field(*b,"hasflasher"))return fail(d,Status::sourceFields,0x277390);
        light::Vec3 rgb{};std::uint32_t zone=0;
        if(!triple(*b,"colorrgb",rgb)||!zoneOf(*b,zone))return fail(d,Status::sourceFields,0x277494);
        for(float& c:rgb)c=mul(c,.5f); //146FDC..147014, not RGB normalization.
        if(i==1){
            light::Vec3 position{},target{};
            if(!textField(*b,"type","Directional")||zone!=1||
               !triple(*b,"position",position)||!triple(*b,"targetposition",target))
                return fail(d,Status::sourceFields,0x2775fc);
            for(unsigned c=0;c<3;++c)sunLook[c]=sub(target[c],position[c]);
            if(!normalizeOnce(sunLook))return fail(d,Status::arithmetic,0x147200);
            for(auto& row:sun44)row={{rgb[0],rgb[1],rgb[2],0.f}};
        }else if(i<=9){
            if(!textField(*b,"type","Ambient"))return fail(d,Status::sourceFields,0x148f30);
            const light::Vec4 row{{rgb[0],rgb[1],rgb[2],64.f}};
            if(zone==1)for(auto& current:ambient60)current=row;
            else ambient60[zone-1]=row;
        }else{
            auto& spot=spots[i-10];float inner=0,outer=0;
            if(!textField(*b,"type","Spotlight")||!triple(*b,"position",spot.position)||
               !triple(*b,"lookvector",spot.look)||!triple(*b,"attenuation",spot.attenuation)||
               !scalar(*b,"innerconeangle",inner)||!scalar(*b,"outerconeangle",outer)||
               !normalizeOnce(spot.look)||inner<0.f||inner>=outer||outer>180.f)
                return fail(d,Status::sourceFields,0x277390);
            for(float a:spot.attenuation)if(!std::isfinite(a)||a<0.f)return fail(d,Status::sourceFields,0x146f60);
            // Ordinary native trigonometry from authored full-angle degrees.
            // Mathematical conversion, not recovered EE cosine bit parity.
            constexpr float halfDegree=3.14159265358979323846f/360.f;
            spot.innerCos=std::cos(mul(inner,halfDegree));spot.outerCos=std::cos(mul(outer,halfDegree));
            spot.zone=zone;spot.rgb=rgb;
            if(!(spot.innerCos>spot.outerCos))return fail(d,Status::arithmetic,0x149fe4);
        }
    }
    // Unlike Light::Init RGB, source override reader278370..2783A4 and
    // setter1498A0 copy these values DIRECTLY. Do not halve30,32,32 again.
    for(std::size_t z=0;z<world.zones.size();++z){
        const auto key="directionallightcolorforzone"+std::to_string(z+1);
        if(field(*lights,key.c_str())){
            light::Vec3 rgb{};if(!triple(*lights,key.c_str(),rgb))return fail(d,Status::sourceFields,0x278370);
            for(unsigned c=0;c<3;++c)sun44[z][c]=rgb[c];
        }
    }
    return true;
}
bool Owner::Storage::prepareGa(Diagnostic& d) {
    const auto& params=world.parameters;
    const auto* lights=block(params,"lights");
    const auto* ambient=block(params,"light1");
    const auto* sun=block(params,"light2");
    const auto* environment=block(params,"environment");
    if(!lights||!ambient||!sun||!environment||
       !textField(*lights,"numberoflights","38")||
       !textField(*ambient,"type","Ambient")||!textField(*sun,"type","Directional")||
       field(*ambient,"initialzone")||field(*sun,"initialzone")||
       !textField(*sun,"emitslight","T")||!textField(*sun,"placelightatinfinity","T")||
       field(*environment,"ambientcolorflashminidletime"))
        return fail(d,Status::sourceFields,0x277390);
    light::Vec3 ambientRgb{},sunRgb{},sunPosition{},sunTarget{};
    if(!triple(*ambient,"colorrgb",ambientRgb)||!triple(*sun,"colorrgb",sunRgb)||
       !triple(*sun,"position",sunPosition)||!triple(*sun,"targetposition",sunTarget))
        return fail(d,Status::sourceFields,0x277494);
    for(unsigned c=0;c<3;++c)sunLook[c]=sub(sunTarget[c],sunPosition[c]);
    if(!normalizeOnce(sunLook))return fail(d,Status::arithmetic,0x147200);
    light::Vec4 ambientRow{{mul(ambientRgb[0],.5f),mul(ambientRgb[1],.5f),mul(ambientRgb[2],.5f),64.f}};
    for(auto& row:ambient60)row=ambientRow;
    for(auto& row:sun44)row={{mul(sunRgb[0],.5f),mul(sunRgb[1],.5f),mul(sunRgb[2],.5f),0.f}};
    // Original148F90..148FFC also writes the same first ambient to global+50.
    // Exact GA SCB has no ambient-flash enabling field; only source+50 is
    // published to Binder, never the selected negative-light-adjusted row.
    globalAmbient50=ambientRow;fixedGcGlobalAmbient=true;
    auto scalar=[](const assets::ParameterBlock48F& b,const char* key,float& value){
        const auto* f=field(b,key);
        return f&&assets::parseTrackNumber50AD(f->value.data(),f->value.size(),value);
    };
    spotCount=36;
    for(std::uint32_t index=0;index<spotCount;++index){
        const auto ordinal=index+3u;
        const auto name="light"+std::to_string(ordinal);const auto* b=block(params,name.c_str());
        if(!b||!textField(*b,"type","Spotlight")||field(*b,"hasflasher"))
            return fail(d,Status::sourceFields,0x277390);
        auto& spot=spots[index];light::Vec3 rgb{};float inner=0.f,outer=0.f;
        if(!triple(*b,"colorrgb",rgb)||!triple(*b,"attenuation",spot.attenuation)||
           !scalar(*b,"innerconeangle",inner)||!scalar(*b,"outerconeangle",outer)||
           inner<0.f||inner>=outer||outer>180.f)
            return fail(d,Status::sourceFields,0x277390);
        namespace poseSource=assets::original_light_pose_defaults_candidate;
        poseSource::FreshPose pose;poseSource::Diagnostic poseDiagnostic;
        if(!poseSource::readFreshPose(*b,static_cast<std::uint32_t>(world.zones.size()),pose,poseDiagnostic))
            return fail(d,Status::sourceFields,poseDiagnostic.sourcePC);
        if(ordinal==9u){
            // Authenticate the actual missing-field branch. The helper retains
            // reader look18/B0 zero and cached effective78 +X separately.
            if(pose.positionAuthored||pose.zoneAuthored||
               pose.lookSource!=poseSource::LookSource::constructorDefault||pose.lookWasNormalized)
                return fail(d,Status::sourceFields,0x277390);
        }else if(!pose.positionAuthored||!pose.zoneAuthored||
                 pose.lookSource!=poseSource::LookSource::lookVector)
            return fail(d,Status::sourceFields,0x277390);
        for(unsigned c=0;c<3;++c)spot.position[c]=pose.positionA0[c];
        spot.look=pose.effectiveLook78; // Already initialized; never normalize again.
        spot.zone=pose.initialZone;
        for(unsigned c=0;c<3;++c)spot.rgb[c]=mul(rgb[c],.5f);
        for(float a:spot.attenuation)if(!std::isfinite(a)||a<0.f)return fail(d,Status::sourceFields,0x146f60);
        constexpr float halfDegree=3.14159265358979323846f/360.f;
        spot.innerCos=std::cos(mul(inner,halfDegree));spot.outerCos=std::cos(mul(outer,halfDegree));
        if(!(spot.innerCos>spot.outerCos))return fail(d,Status::arithmetic,0x149fe4);
    }
    return true;
}
bool Owner::Storage::selectTa(const LiveZoneMembership50EN& member,
    const light::Vec3& position,light::LightMatrices& output,Diagnostic& d) const noexcept {
    for(float x:position)if(!std::isfinite(x))return fail(d,Status::arithmetic,0x2413a8);
    if(!member.attached||!member.storage.links||!member.storage.capacity)
        return fail(d,Status::identity,0x14a118);
    const auto primary=member.storage.links[0].zone;
    if(!primary||primary>world.zones.size())return fail(d,Status::identity,0x14a120);
    light::LightMatrices next{};
    next.diffuse[3]=ambient60[primary-1];next.diffuse[0]=sun44[primary-1];
    next.specular[0]=sun44[0]; // Native host renderer currently ignores specular.
    for(unsigned c=0;c<3;++c)next.directions[c][0]=-sunLook[c];
    //149680 exact high-level portal color blend, actual visibility-zone links.
    //500 is original14A698's radius, not a visual calibration constant.
    light::Vec3 ambientSum{},sunSum{};float total=0.f;
    auto link=visibility.portals.portalHeads[primary-1];std::size_t visited=0;
    const Vector3_48C center{{position[0],position[1],position[2]}};
    while(link){
        if(link>visibility.portals.portalLinkCount||++visited>visibility.portals.portalLinkCount)
            return fail(d,Status::registration,0x149700);
        const auto& edge=visibility.portals.portalLinks[link-1];bool hit=false;float distance=0.f;
        if(!edge.zone||edge.zone>world.zones.size()||!edge.portal||edge.portal>visibility.portals.portalCount)
            return fail(d,Status::registration,0x149720);
        if(overlapPortal231150_50EO(visibility.portals.portals[edge.portal-1],center,500.f,hit,distance)!=PortalGeometryStatus50EO::Complete)
            return fail(d,Status::arithmetic,0x14970c);
        if(hit){
            volatile float weight=sub(500.f,distance)/500.f;
            total=add(total,weight);
            for(unsigned c=0;c<3;++c){
                ambientSum[c]=add(ambientSum[c],mul(weight,ambient60[edge.zone-1][c]));
                sunSum[c]=add(sunSum[c],mul(weight,sun44[edge.zone-1][c]));
            }
        }
        link=edge.next;
    }
    volatile float reciprocal=1.f/add(1.f,total);
    for(unsigned c=0;c<3;++c){
        next.diffuse[3][c]=mul(reciprocal,add(next.diffuse[3][c],ambientSum[c]));
        next.diffuse[0][c]=mul(reciprocal,add(next.diffuse[0][c],sunSum[c]));
    }
    // DELIBERATE HOST QUERY: authored initial-zone matches any genuine current
    // racer membership zone. No fake source Light+D0 or manager+88 registry.
    // Bounded scan in numeric authored order; retain two strongest by reciprocal
    // attenuation, ties retain earlier authored lights. Sun reserves slot0.
    std::array<float,2> scores{{std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity()}};
    std::array<light::Vec3,2> directions{},colors{};
    if(spotCount>spots.size())return fail(d,Status::bounds,0x14a118);
    for(std::uint32_t spotIndex=0;spotIndex<spotCount;++spotIndex){
        const auto& spot=spots[spotIndex];
        bool eligible=false;auto memberLink=CollectionRef50BX{1};std::size_t count=0;
        while(memberLink){
            if(memberLink>member.storage.capacity||++count>member.storage.capacity)
                return fail(d,Status::registration,0x14a118);
            const auto& current=member.storage.links[memberLink-1];
            if(!current.zone||current.zone>world.zones.size())return fail(d,Status::registration,0x14a120);
            eligible=eligible||current.zone==spot.zone;memberLink=current.next;
        }
        if(!eligible)continue;
        light::Vec3 toward{};float square=0.f;
        for(unsigned c=0;c<3;++c){toward[c]=sub(spot.position[c],position[c]);square=add(square,mul(toward[c],toward[c]));}
        if(!std::isfinite(square))return fail(d,Status::arithmetic,0x14a2dc);
        // No arbitrary epsilon or substituted normal. At exact coincidence the
        // native directional Lambert contribution is undefined and is omitted.
        if(square==0.f)continue;
        const float distance=std::sqrt(square);if(!normalizeOnce(toward))return fail(d,Status::arithmetic,0x14a5cc);
        float cosine=0.f;for(unsigned c=0;c<3;++c)cosine=add(cosine,mul(toward[c],-spot.look[c]));
        float penalty=1.f;
        if(cosine<spot.outerCos)penalty=5.f;
        else if(cosine<=spot.innerCos){
            volatile float fraction=sub(spot.innerCos,cosine)/sub(spot.innerCos,spot.outerCos);
            penalty=add(sub(1.f,fraction),mul(5.f,fraction));
        }
        const float denominator=mul(penalty,add(add(spot.attenuation[0],mul(spot.attenuation[1],distance)),mul(spot.attenuation[2],square)));
        if(!std::isfinite(denominator)||denominator<=0.f)return fail(d,Status::arithmetic,0x14a37c);
        const unsigned slot=denominator<scores[0]?0u:denominator<scores[1]?1u:2u;
        if(slot==2)continue;
        if(slot==0){scores[1]=scores[0];directions[1]=directions[0];colors[1]=colors[0];}
        scores[slot]=denominator;directions[slot]=toward;
        for(unsigned c=0;c<3;++c){volatile float color=spot.rgb[c]/denominator;colors[slot][c]=color;}
    }
    for(unsigned slot=0;slot<2;++slot)if(std::isfinite(scores[slot]))for(unsigned c=0;c<3;++c){
        next.directions[c][slot+1]=directions[slot][c];
        next.diffuse[slot+1][c]=next.specular[slot+1][c]=colors[slot][c];
    }
    // Source149A50: only selected negative contributions affect sun/ambient.
    // Folding every authored negative light globally would be incorrect.
    if(light::foldSelectedNegativeLights(next,0,output)!=light::Status::complete)
        return fail(d,Status::arithmetic,0x149a50);
    return true;
}

Owner::Owner() noexcept=default;
Owner::~Owner(){
    if(storage_&&storage_->activeLease)endDraw(*const_cast<DrawLease*>(storage_->activeLease));
    if(storage_&&storage_->activeCamera)endCameraVisibility(*const_cast<CameraVisibilityLease*>(storage_->activeCamera));
}
DrawLease::~DrawLease(){if(owner_)owner_->endDraw(*this);}
const assets::World48F* Owner::world() const noexcept {return storage_?&storage_->world:nullptr;}
bool Owner::prepare(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,false,false,d);
}
bool Owner::prepareTaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,true,false,d);
}
bool Owner::prepareGaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,false,true,d);
}
bool Owner::prepareScene(const std::vector<std::uint8_t>& scb,std::uint32_t racers,bool ta,bool ga,Diagnostic& d) noexcept {
    d={};if(invalidated_)return fail(d,Status::unprepared,0x272a48);
    if(storage_)return fail(d,Status::alreadyPrepared,0x272a48);
    if(!racers||racers>maximumRacers)return fail(d,Status::identity,0x1ae578);
    if(!authenticScb(scb,ta,ga))return fail(d,Status::sourceIdentity,0x270b10);
    if(!light::selection_detail::arithmeticReady())return fail(d,Status::arithmetic,0x146f60);
    try {
        auto next=std::make_unique<Storage>();
        if(!assets::decodeWorld48F(scb,next->world,d.decode))return fail(d,Status::sourceShape,0x270b10);
        const auto& w=next->world;
        if(w.zones.size()!=(ta?9u:gcZones)||w.portals.size()!=(ga?gaPortals:ta?10u:gcPortals))
            return fail(d,Status::unsupportedScene,0x272a48);
        next->taNative=ta;next->gaNative=ga;
        if(ga){if(!next->prepareGa(d))return false;}
        else if(ta){if(!next->prepareTa(d))return false;}else{
        const auto* first=block(w.parameters,"light1");const auto* second=block(w.parameters,"light2");
        const auto* lights=block(w.parameters,"lights");
        if(!first||!second||!lights||!textField(*lights,"numberoflights","2")||
            !textField(*first,"type","Ambient")||!textField(*second,"type","Directional")||
            field(*first,"initialzone")||field(*second,"initialzone")||field(*second,"lookvector")||
            !textField(*second,"emitslight","T")||!textField(*second,"placelightatinfinity","T"))
            return fail(d,Status::sourceFields,0x277390);
        light::Vec3 ambient{},diffuse{},position{},target{};
        if(!triple(*first,"colorrgb",ambient)||!triple(*second,"colorrgb",diffuse)||
            !triple(*second,"position",position)||!triple(*second,"targetposition",target))
            return fail(d,Status::sourceFields,0x277494);
        next->registry.count=2;next->registry.registered[0].type=4;
        auto& directional=next->registry.registered[1];directional.type=2;
        light::Vec4 row{};
        for(unsigned c=0;c<3;++c){
            row[c]=mul(ambient[c],.5f);
            directional.diffuse3C[c]=directional.specular4C[c]=mul(diffuse[c],.5f);
            directional.direction78[c]=sub(target[c],position[c]);
        }
        if(!normalizeOnce(directional.direction78))return fail(d,Status::arithmetic,0x147200);
        row[3]=64.f;
        //2774C8 default1,2774FC subtract1 -> source ordinal0.148F30 first
        // registers ambient,149000..149040 copies it into EVERY actual row.
        for(auto& zoneAmbient:next->ambient60)zoneAmbient=row;
        next->registry.selectedZoneAmbient=row;
        //148F90/FB4/FD8/FFC publishes this SAME registered row to+50.
        //270E9C enables Scene390bit0 only if this actual key is present;
        //275544 then overrides149900, and275558 restores149950. The exact
        //authenticated GC SCB has no key, so this fixed lifetime closes.
        const auto* environment=block(w.parameters,"environment");
        if(!environment||field(*environment,"ambientcolorflashminidletime"))
            return fail(d,Status::sourceFields,0x270e9c);
        next->globalAmbient50=row;next->fixedGcGlobalAmbient=true;
        }
        next->racerCount=racers;
        for(std::size_t z=0;z<w.zones.size();++z){
            const auto read=assets::readZoneSearchSelection50CN(w,z);
            if(w.zones[z].zone!=z+1||read.status!=assets::ZoneSearchReadStatus50CN::Complete||
               read.selection.visibilityDynamic114!=assets::ZoneSearchKind50CN::List)
                return fail(d,Status::unsupportedScene,0x272a58);
        }
        const ZoneLifecycleStorage50CL lifeStorage{next->registries.data(),next->registries.size(),
            next->registrations.data(),next->registrations.size(),
            {next->methods.data(),next->methods.size(),next->zones.data(),next->zones.size(),
             next->blocks.data(),next->blocks.size()}};
        if(initializeZoneLifecycle50CL(next->visibility.lifecycle,lifeStorage,w.zones.size(),racers+(ta?0u:1u),1024,
            next->workspace.transfer)!=SearchStorageStatus50BS::Complete)
            return fail(d,Status::registration,0x272a58);
        std::array<LiveZonePortalInput50EN,maximumPortals> input{};
        for(std::size_t i=0;i<w.portals.size();++i){
            input[i].zoneA=w.portals[i].zones[0];input[i].zoneB=w.portals[i].zones[1];
            const auto index=w.portals[i].parameter_block;
            if(index>=w.parameters.blocks.size())return fail(d,Status::sourceShape,0x272b3c);
            constexpr const char* keys[]={"vertex1","vertex2","vertex3","vertex4"};
            for(unsigned v=0;v<4;++v){
                light::Vec3 value{};
                // Parse exact retained source text with50AD, not World48F's
                // host-parser float projection, even when it round-trips.
                if(!triple(w.parameters.blocks[index],keys[v],value))
                    return fail(d,Status::sourceFields,0x272b3c);
                for(unsigned c=0;c<3;++c)input[i].vertices[v].v[c]=value[c];
            }
        }
        const LiveZoneWorldStorage50EN portalStorage{next->portalHeads.data(),next->portalHeads.size(),
            next->portalLinks.data(),next->portalLinks.size(),next->portalGeometry.data(),next->portalGeometry.size()};
        if(prepareLiveZoneWorld50EN(input.data(),w.portals.size(),w.zones.size(),portalStorage,next->visibility.portals)!=LiveZoneStatus50EN::Complete)
            return fail(d,Status::registration,0x272bc8);
        next->workspace.registries=next->scratchRegistries.data();next->workspace.registryCapacity=next->scratchRegistries.size();
        next->workspace.links=next->scratchLinks.data();next->workspace.entries=next->scratchEntries.data();
        next->workspace.membershipCapacity=next->scratchLinks.size();
        next->workspace.portalLinks=next->scratchPortalLinks.data();next->workspace.portalLinkCapacity=next->scratchPortalLinks.size();
        storage_=std::move(next);return true;
    }catch(...){return fail(d,Status::allocation,0x272a48);}
}
bool Owner::attachFreshVehicle(std::uint32_t slot,VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x229f20);
    auto& o=*storage_;
    if(o.activeLease||o.activeCamera)return fail(d,Status::activeDraw,0x2167a0);
    if(slot>=o.racerCount||!frameBinding(s,b))return fail(d,Status::identity,0x2167a0);
    auto& v=o.vehicles[slot];
    if(v.attached||v.member.attached)return fail(d,Status::alreadyPrepared,0x229f30);
    if(s.phase!=VehicleFramePhase49W::Ready||s.bodies.main.frameFactor!=0.f)
        return fail(d,Status::phase,0x229f44);
    const auto& main=s.nodes[static_cast<std::uint32_t>(b.lifecycle.main190)];
    if(!(main.sceneFlags190&1u)||main.parent180!=SceneNodeIndex49P::none||
       s.shape.registrationE0||s.shape.type88||
       std::memcmp(&main.world100,&s.bodies.main.components.root.scene,sizeof(BodyMatrix48G)))
        return fail(d,Status::identity,0x21684c);
    if(v.pendingInitial){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape))||
           std::memcmp(&v.preparedPrefix,&s.shape,sizeof(s.shape))||
           std::memcmp(&main.world100,&s.shape.world80,sizeof(BodyMatrix48G)))
            return fail(d,Status::identity,0x21684c);
    }else{
        PodShapeState49W prepared=s.shape;
        if(!transformPodShape49W(p.shape,main.world100,prepared))
            return fail(d,Status::bounds,0x21684c);
        // Source-completed11FE50 numeric prefix survives a later registration
        // boundary. This is the actual frame's694, not a parallel shape cache.
        s.shape=prepared;v.preparedPrefix=prepared;v.source=p.shape;
        v.mainRoot=b.lifecycle.main190;v.roots=modelRoots(b);v.pendingInitial=true;
    }
    const auto bounds=shapeBounds(s.shape,p.shape);
    const LiveZoneStorage50EN backing{v.links.data(),v.entries.data(),v.links.size()};
    // Source C30 was ACTUALLY initialized from visibility4C0[0] at1AE578;
    // sourceRef1 is that same first row in this owner's separate namespace.
    d.membership=attachLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,v.member,backing,
        slot+1u,1u,ZoneMethod50CL::Second,s.shape.radius80,bounds,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x21688c);
    v.attached=true;v.pendingInitial=false;return true;
}
bool Owner::updateAfterGraphics(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint64_t graphicsSequence,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x228844);
    auto& o=*storage_;
    if(o.activeLease||o.activeCamera)return fail(d,Status::activeDraw,0x228844);
    if(slot>=o.racerCount||!frameBinding(s,b)||!o.bound(slot))
        return fail(d,Status::identity,0x228858);
    auto& v=o.vehicles[slot];
    if(s.phase!=VehicleFramePhase49W::PostGraphics||!graphicsSequence||
       v.graphicsSequence==std::numeric_limits<std::uint64_t>::max()||
       graphicsSequence!=v.graphicsSequence+1u)return fail(d,Status::phase,0x228844);
    if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||s.shape.type88||
       std::memcmp(&v.source,&p.shape,sizeof(p.shape)))
        return fail(d,Status::identity,0x228844);
    // Deliberately do not compare694.world80 to current main.world100 and do
    // not reprepare694.228844 calls1202E0 directly on its EXISTING cached state.
    // A later219FD8 can advance694 without advancing7F0 until the next graphics.
    const auto bounds=shapeBounds(s.shape,p.shape);
    d.membership=updateLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,v.member,bounds,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x228884);
    v.graphicsSequence=graphicsSequence;return true;
}
CameraVisibilityLease::~CameraVisibilityLease(){if(owner_)owner_->endCameraVisibility(*this);}
bool Owner::cameraLeaseCurrent(const CameraVisibilityLease& lease) const noexcept {
    if(!storage_||invalidated_||lease.owner_!=this)return false;
    const auto& o=*storage_;
    return o.activeCamera==&lease&&!o.activeLease&&!o.taNative&&
        lease.generation_&&lease.generation_==o.cameraGeneration&&
        lease.racer_<o.racerCount&&o.bound(lease.racer_);
}
bool Owner::beginCameraVisibility(std::uint32_t slot,CameraVisibilityLease& lease,
    Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x175224);
    auto& o=*storage_;
    if(o.activeLease||o.activeCamera||lease.owner_)return fail(d,Status::activeDraw,0x175224);
    if(o.taNative)return fail(d,Status::unsupportedScene,0x175224);
    if(slot>=o.racerCount||!o.bound(slot)||o.visibility.lifecycle.objectCount!=o.racerCount+1u)
        return fail(d,Status::identity,0x175224);
    if(o.cameraGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::stalePublication,0x175224);
    lease.owner_=this;lease.generation_=++o.cameraGeneration;lease.racer_=slot;
    o.activeCamera=&lease;return true;
}
void Owner::endCameraVisibility(CameraVisibilityLease& lease) noexcept {
    if(storage_&&storage_->activeCamera==&lease)storage_->activeCamera=nullptr;
    lease.owner_=nullptr;lease.generation_=0;lease.racer_=0;
}
ResetVisibilityWorld50EY* CameraVisibilityLease::world() noexcept {
    return owner_&&owner_->cameraLeaseCurrent(*this)?&owner_->storage_->visibility:nullptr;
}
LiveZoneWorkspace50EN* CameraVisibilityLease::workspace() noexcept {
    return owner_&&owner_->cameraLeaseCurrent(*this)?&owner_->storage_->workspace:nullptr;
}
const LiveZoneMembership50EN* CameraVisibilityLease::racerVisibility7F0() const noexcept {
    return owner_&&owner_->cameraLeaseCurrent(*this)?&owner_->storage_->vehicles[racer_].member:nullptr;
}
CollectionRef50BX CameraVisibilityLease::cameraObject30() const noexcept {
    return owner_&&owner_->cameraLeaseCurrent(*this)?owner_->storage_->racerCount+1u:0u;
}
std::uint64_t CameraVisibilityLease::zoneKey4C0(std::size_t ordinal) const noexcept {
    if(!owner_||!owner_->cameraLeaseCurrent(*this)||ordinal>=owner_->storage_->visibility.lifecycle.zoneCount)return 0;
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&owner_->storage_->zones[ordinal]));
}
std::size_t CameraVisibilityLease::zoneCount() const noexcept {
    return owner_&&owner_->cameraLeaseCurrent(*this)?owner_->storage_->visibility.lifecycle.zoneCount:0u;
}
bool Owner::beginDraw(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,std::uint64_t expectedGraphicsSequence,
    DrawLease& lease,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x229e08);
    auto& o=*storage_;
    if(o.activeLease||o.activeCamera||lease.owner_)return fail(d,Status::activeDraw,0x229e08);
    if(slot>=o.racerCount||!frameBinding(s,b)||!o.bound(slot))
        return fail(d,Status::identity,0x229e08);
    if(s.phase!=VehicleFramePhase49W::Ready&&s.phase!=VehicleFramePhase49W::PostGraphics)
        return fail(d,Status::phase,0x229e08);
    const auto& v=o.vehicles[slot];
    if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||!v.graphicsSequence||
       v.graphicsSequence!=expectedGraphicsSequence||
       o.drawGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::identity,0x229e08);
    lease.owner_=this;lease.frame_=&s;lease.generation_=++o.drawGeneration;
    lease.membershipRevision_=v.member.revision;lease.racer_=slot;lease.roots_=v.roots;
    o.selectedReady.fill(false);o.activeLease=&lease;return true;
}
bool Owner::select(const DrawLease& lease,std::uint32_t component,SceneNodeIndex49P root,
    CachePolicy policy,light::LightMatrices& output,Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x2413a8);
    auto& o=*storage_;
    if(lease.owner_!=this||o.activeLease!=&lease||lease.generation_!=o.drawGeneration||
       lease.racer_>=o.racerCount||!o.bound(lease.racer_)||component>=3u||
       lease.roots_[component]!=root||
       o.vehicles[lease.racer_].member.revision!=lease.membershipRevision_)
        return fail(d,Status::stalePublication,0x2413d0);
    if(policy!=(o.gaNative?CachePolicy::nativeGaAuthoredZoneSpotSelection:
        o.taNative?CachePolicy::nativeTaAuthoredZoneSpotSelection:
        CachePolicy::inactiveCandidateNativePerPublicationRecompute))
        return fail(d,Status::unsupportedPolicy,0x14a0e8);
    // INTENTIONALLY INACTIVE NATIVE POLICY: do not reproduce retail A4 hits.
    // It can differ from14A860's membership-key retained matrix. No source
    // null/miss/completeness assertion is manufactured by choosing this branch.
    if(!o.selectedReady[component]){
        if(o.taNative||o.gaNative){
            if(!lease.frame_||!validRoot(root,lease.frame_->sceneNodeCount))
                return fail(d,Status::identity,0x2413d0);
            const auto& node=lease.frame_->nodes[static_cast<std::uint32_t>(root)];
            if(!(node.sceneFlags190&1u))return fail(d,Status::phase,0x2412e4);
            const light::Vec3 position{{node.world100.v[12],node.world100.v[13],node.world100.v[14]}};
            light::LightMatrices next{};
            if(!o.selectTa(o.vehicles[lease.racer_].member,position,next,d))return false;
            o.selected[component]=next;o.selectedReady[component]=true;
        }else{
        const auto zone=o.vehicles[lease.racer_].links[0].zone;
        auto selected=o.registry;selected.selectedZoneAmbient=o.ambient60[zone-1u];
        light::LightMatrices next{};
        if(light::projectSmallDirectionalRegistry(selected,next)!=light::Status::complete)
            return fail(d,Status::arithmetic,0x149bb0);
        o.selected[component]=next;o.selectedReady[component]=true;
        }
    }
    output=o.selected[component];return true;
}
bool Owner::publishNativeDraw(const DrawLease& lease,CachePolicy policy,
    NativeGcSelectedLightsCandidate& output,Diagnostic& d) noexcept {
    NativeGcSelectedLightsCandidate next{};
    for(unsigned c=0;c<3;++c)
        if(!select(lease,c,lease.roots_[c],policy,next.matrices_[c],d))return false;
    next.ownerIdentity_=reinterpret_cast<std::uintptr_t>(this);
    next.graphicsSequence_=storage_->vehicles[lease.racer_].graphicsSequence;
    next.publication_=lease.generation_;next.racerSlot_=lease.racer_;
    output=next;return true;
}
void Owner::endDraw(DrawLease& lease) noexcept {
    if(storage_&&storage_->activeLease==&lease){
        storage_->selectedReady.fill(false);storage_->activeLease=nullptr;
    }
    lease.owner_=nullptr;lease.frame_=nullptr;lease.generation_=0;lease.membershipRevision_=0;
    lease.racer_=0;lease.roots_={};
}
void Owner::invalidate() noexcept {
    invalidated_=true;
    if(storage_)storage_->selectedReady.fill(false);
}
light::Status prepareDiffuseNormalAxes(const BodyMatrix48G& world,float normalQ,
    const light::LightMatrices& selected,light::DiffuseProgramInputs& output) noexcept {
    if(!light::selection_detail::arithmeticReady())return light::Status::arithmetic;
    for(float v:world.v)if(!std::isfinite(v))return light::Status::input;
    if(!std::isfinite(normalQ)||normalQ<=0||world.v[3]!=0||world.v[7]!=0||
       world.v[11]!=0||world.v[15]!=1)return light::Status::input;
    for(const auto& row:selected.directions)for(float v:row)
        if(!std::isfinite(v))return light::Status::input;
    for(const auto& row:selected.diffuse)for(float v:row)
        if(!std::isfinite(v))return light::Status::input;
    light::DiffuseProgramInputs next{};
    //3400D0..3400E0 scales direction lanes before3400E8..340140's basis
    // composition. Preserve each separate operation, including the zero term.
    for(unsigned axis=0;axis<3;++axis)for(unsigned lane=0;lane<3;++lane){
        float value=mul(mul(selected.directions[0][lane],normalQ),world.v[axis*4]);
        value=add(value,mul(mul(selected.directions[1][lane],normalQ),world.v[axis*4+1]));
        value=add(value,mul(mul(selected.directions[2][lane],normalQ),world.v[axis*4+2]));
        value=add(value,mul(0.f,world.v[axis*4+3]));
        if(!std::isfinite(value))return light::Status::arithmetic;
        next.normalAxes[axis][lane]=value;
    }
    for(unsigned row=0;row<4;++row)for(unsigned c=0;c<3;++c)
        next.colorRows[row][c]=selected.diffuse[row][c];
    output=next;return light::Status::complete;
}
bool recoverCompressedNormal(const std::array<double,3>& expanded,double normalQ,
    light::CompressedNormal& output) noexcept {
    if(!std::isfinite(normalQ)||normalQ<=0||static_cast<double>(static_cast<float>(normalQ))!=normalQ)
        return false;
    light::CompressedNormal next{};
    for(unsigned c=0;c<3;++c){
        const double raw=expanded[c]/normalQ;
        if(!std::isfinite(expanded[c])||!std::isfinite(raw)||raw<-128.||raw>127.||
           std::trunc(raw)!=raw||raw*normalQ!=expanded[c])return false;
        next[c]=static_cast<std::int8_t>(raw);
    }
    output=next;return true;
}
} // namespace rr::game::original_gc_gameplay_lighting_candidate

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::currentGcGlobalAmbient50(std::array<float,4>& out) const noexcept {
    if(!storage_||invalidated_||storage_->taNative||storage_->gaNative||!storage_->fixedGcGlobalAmbient)return false;
    out=storage_->globalAmbient50;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::currentSourceGlobalAmbient50(std::array<float,4>& out) const noexcept {
    if(!storage_||invalidated_||storage_->taNative||!storage_->fixedGcGlobalAmbient)return false;
    out=storage_->globalAmbient50;return true;
}
}
