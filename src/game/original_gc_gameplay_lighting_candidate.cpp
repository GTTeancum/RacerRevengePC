#include "original_native_list_method_custody50la.h"
#include <thread>
#include "../../tests/native/mc_cold_sources50la.h"
#include "../../tests/native/course_archive_receipt50ci.h"
#include "../../tests/native/mc_lighting_authoring50mc.h"
#include "original_gc_gameplay_lighting_candidate.h"
#include "original_gb_light_flash50gb.h"
#include "original_racer_entity_cold50la.h"
#include "original_camera_scene_owner.h"
#include "original_world_environment50kg.h"
#include "original_application_entities50la.h"
#include "original_racer_model_scene50la.h"
#include "original_racer_model_view50la.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_camera_start_native.h"
#include "../assets/original_standard_model_resource50la.h"
#include "../assets/original_standard_material_table50la.h"
#include <optional>
#include <type_traits>
#include <span>
#include <algorithm>
#include "../assets/track_start50ad.h"
#include "../assets/original_light_pose_defaults_candidate.h"
#include "../assets/zone_search_authoring50cn.h"
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
#include <new>
#include <exception>
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
// Bounded finite RA cone half-angles. Same13BBA0 polynomial already
// recovered privately in graph_body49w/control_shaping48v. No new libm owner.
// RA authored0..180 degrees needs no reflection. Separate volatile operations
// retain the source146F30 degree conversion and149FF4 half-angle chronology.
float raConeCosine(float degrees) noexcept {
    auto word=[](std::uint32_t bits){float f;std::memcpy(&f,&bits,sizeof(f));return f;};
    const float radians=mul(degrees,word(0x3c8efa34u));
    const float x=mul(.5f,radians),square=mul(x,x);
    float t=add(word(0x37d00d01u),mul(word(0xb493f27eu),square));
    t=add(word(0xbab60b61u),mul(square,t));
    t=add(word(0x3d2aaaabu),mul(square,t));
    t=add(-.5f,mul(square,t));
    return mul(1.f,add(1.f,mul(square,t)));
}
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
bool readZoneRenderAuthoring(const assets::ParameterBlock48F& b,std::size_t zoneCount,
    AuthoredRenderZone& out) noexcept {
    AuthoredRenderZone next{};
    const assets::ParameterField48F* exclusions=nullptr;
    for(const auto& f:b.fields){
        if(f.name=="exclusionlist"){
            if(exclusions)return false;
            exclusions=&f;
        }
        // Authenticated GA has no override: 275D74/275EE4 default to one.
        // Do not silently approximate the integer reader for unseen overrides.
        if(f.name=="environmentnodevisible")return false;
    }
    if(exclusions){
        const auto& text=exclusions->value;
        if(text.size()>=128)return false; // Original reader's 128-byte buffer.
        std::size_t at=0;
        while(at<text.size()){
            // Bounded admitted domain: positive decimal authored zone labels.
            // 27632C..27641C stores converted labels minus one. No sorting or
            // deduplication; no host stoi/locale or unproven malformed input.
            std::uint32_t label=0;
            const auto start=at;
            while(at<text.size()&&text[at]>='0'&&text[at]<='9'){
                const auto digit=static_cast<std::uint32_t>(text[at]-'0');
                if(label>(std::numeric_limits<std::uint32_t>::max()-digit)/10u)return false;
                label=label*10u+digit;++at;
            }
            // 2763E8..276410 stores atoi(token)-1 with no range check, so a
            // label outside the world (RC Zone5 lists 17 of 15) is kept and
            // simply never matches a zone. Do not reject it.
            (void)zoneCount;
            if(at==start||next.exclusionCount>=next.exclusions.size())return false;
            next.exclusions[next.exclusionCount++]=label-1u;
            if(at==text.size())break;
            if(text[at]!=','&&text[at]!='\n')return false;
            ++at;
            // 29FFA0 terminates on a single trailing delimiter. GA zones 3
            // and 7 actually use this form; it does not append an empty token.
            if(at==text.size())break;
            while(at<text.size()&&(text[at]==','||text[at]=='\n'))++at;
            // Multiple trailing delimiters remain outside the admitted domain.
            if(at==text.size())return false;
        }
    }
    out=next;return true;
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
bool authenticScb(const std::vector<std::uint8_t>& input,std::size_t expectedBytes,
    const char* expected) noexcept {
    if (input.size()!=expectedBytes||!expected) return false;
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
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u]) return false;
    return true;
}
bool validRoot(SceneNodeIndex49P root,std::uint32_t count) noexcept {
    const auto i=static_cast<std::int32_t>(root);return i>=0&&static_cast<std::uint32_t>(i)<count;
}
std::array<SceneNodeIndex49P,maxVehicleDrawComponents50CM> modelRoots(const VehicleFrameBindings49W& b) noexcept {
    return {b.lifecycle.model1B4,b.visual.left1BC,b.visual.right1B8,
        b.visual.bottomLeft1C4,b.visual.bottomRight1C0};
}
std::uint32_t modelCount(const VehicleFrameBindings49W& b) noexcept {
    return b.visual.bottomRight1C0!=SceneNodeIndex49P::none ? 5u :
        b.visual.bottomLeft1C4!=SceneNodeIndex49P::none ? 4u : defaultVehicleDrawComponents50CM;
}
bool frameBinding(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b) noexcept {
    if(!validVehicleSceneCount49W(s,b)||!validRoot(b.lifecycle.main190,s.sceneNodeCount))return false;
    const auto roots=modelRoots(b);
    const auto count=modelCount(b);
    for(std::uint32_t c=0;c<count;++c){
        if(!validRoot(roots[c],s.sceneNodeCount))return false;
        for(std::uint32_t earlier=0;earlier<c;++earlier)
            if(roots[c]==roots[earlier])return false;
    }
    return roots[0]==b.animation.chariot1B4&&roots[1]==b.animation.left1BC&&
        roots[1]==b.lifecycle.left1BC&&roots[2]==b.animation.right1B8&&roots[2]==b.lifecycle.right1B8&&
        roots[0]!=roots[1]&&roots[0]!=roots[2]&&roots[1]!=roots[2];
}
LiveZoneBounds50EN shapeBounds(const PodShapeState49W& shape,const PodShapeSource49W& source) noexcept {
    LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=0;bounds.bounds.shapeWorld=shape.composed00;
    bounds.bounds.cachedCenter=shape.center60;bounds.bounds.cachedRadius=shape.radius80;
    bounds.type0Resource=&source;return bounds;
}
bool disjointResetField(const std::uint32_t* field,const void* other,std::size_t size) noexcept {
    const auto a=reinterpret_cast<std::uintptr_t>(field);
    const auto b=reinterpret_cast<std::uintptr_t>(other);
    if(!field||a%alignof(std::uint32_t)||a>std::numeric_limits<std::uintptr_t>::max()-sizeof(*field)||
       !other||b>std::numeric_limits<std::uintptr_t>::max()-size)return false;
    return a+sizeof(*field)<=b||b+size<=a;
}
bool preparedResetShape(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b,
    const VehicleFrameParameters49W& p) noexcept {
    if(!frameBinding(s,b)||s.shape.type88||s.shape.registrationE0)return false;
    const auto& main=s.nodes[static_cast<std::uint32_t>(b.lifecycle.main190)];
    if(!(main.sceneFlags190&1u)||main.parent180!=SceneNodeIndex49P::none||
       std::memcmp(&main.world100,&s.bodies.main.components.root.scene,sizeof(BodyMatrix48G))||
       std::memcmp(&main.world100,&s.shape.world80,sizeof(BodyMatrix48G)))return false;
    // Verify the completed numeric prefix without mutating or preparing the
    // canonical694 again. previousC0 remains the actual preceding11FE50 result.
    auto checked=s.shape;
    if(!transformPodShape49W(p.shape,main.world100,checked))return false;
    return !std::memcmp(&checked.composed00,&s.shape.composed00,sizeof(BodyMatrix48G))&&
        !std::memcmp(&checked.center60,&s.shape.center60,sizeof(s.shape.center60))&&
        !std::memcmp(&checked.radius80,&s.shape.radius80,sizeof(s.shape.radius80));
}
}

struct Owner::Storage {
    // Frozen authentic authored local lights, deliberately not a projection
    // of source Light+D0 or LightManager+88. No light updates are invented.
    struct AuthoredLocalLight {
        light::Vec3 position{},look{},rgb{},attenuation{};
        float innerCos=0,outerCos=0;
        std::uint32_t zone=0,sourceOrdinal=0,effectiveSourceOrdinal=0;
        bool spotlight=true,emitsLight=true;
        //2776D4 defaults PlaceLightAtInfinity to1. Preserve the source flag;
        //it does not turn a type1/type3 attenuation query into a directional sun.
        bool placeAtInfinity=true;
    };
    struct SelectedMethod {
        OriginalNativeListMethodCustody50LA sourceList;
        std::vector<const OriginalWreckComponent50LA*> selected18;
        std::size_t count00=0,capacity0C=0;
        std::uint8_t type2C=0xffu,type2D=0xffu;
        bool constructed=false,complete=false;
        void resetNativeSelection()noexcept {
            decltype(selected18) empty;selected18.swap(empty);count00=capacity0C=0;type2C=type2D=0xffu;constructed=complete=false;
        }
    };
    struct ZoneProjection {
        //27F158 initializes the actual SearchNode44 to zero. Authoring is
        //published only at its retained StaticModels callback predecessor.
        std::uint32_t environmentVisible44=0;
        bool environment44Published=false;
        //27F15C explicitly initializes A0 null. The actual numeric owner is
        //created only when27FF38 reaches280230, not by camera publication.
        std::shared_ptr<OriginalZoneScissor50LA> scissorA0;
        std::optional<std::uint16_t> admissionA4,dispatchA6;
        std::optional<std::array<float,4>> bounds4C;
        std::optional<PortalZoneViewOutput49F> matrix60;
    };
    // Closed native extents. Legacy public limits remain25/25; SC has its own
    // authenticated30/34 topology and TC25/26, never caller-provided capacity.
    template<std::size_t Z,std::size_t P> struct TopologyStorage {
        std::array<light::Vec4,Z> sun44{};
        std::array<light::Vec4,Z> ambient60{};
        std::array<SearchStorage50BS,Z*2> registries{};
        std::array<SearchStorage50BS,Z*2> scratchRegistries{};
        std::array<SelectedMethod,Z*2> selectedMethods{};
        std::array<CollectionTree50BX,Z*2> methods{};
        std::array<CollectionZone50BX,Z> zones{};
        std::array<ZoneProjection,Z> zoneProjection{};
        std::array<AuthoredRenderZone,Z> renderAuthoring{};
        std::array<SearchListBlock50BR,Z*4> blocks{};
        std::array<CollectionRef50BX,Z> portalHeads{};
        std::array<LiveZonePortalLink50EN,P*2> portalLinks{};
        std::array<LiveZonePortalLink50EN,P*2> scratchPortalLinks{};
        std::array<PortalGeometry50EO,P> portalGeometry{};
        std::array<CollectionZoneLink50BX,P+1> scratchLinks{};
        std::array<LiveZoneEntry50EN,P+1> scratchEntries{};
        std::array<std::array<CollectionZoneLink50BX,P+1>,maximumRacers> vehicleLinks{};
        std::array<std::array<LiveZoneEntry50EN,P+1>,maximumRacers> vehicleEntries{};
    };
    using LegacyTopologyStorage=TopologyStorage<maximumZones,maximumPortals>;
    using ScTopologyStorage=TopologyStorage<30,34>;
    using TcTopologyStorage=TopologyStorage<25,26>;
    using McTopologyStorage=TopologyStorage<28,28>;
    std::unique_ptr<McTopologyStorage> mcTopology;
    std::shared_ptr<const ::mc_cold_sources50LA::Owner> mcSource;
    std::array<::mc_lighting_authoring50MC::Record,29> mcRecords{};
    const McParticleLightLease* activeMcParticleLight=nullptr;
    std::thread::id mcParticleThread=std::this_thread::get_id();
    // Anchors precede every borrower; only one is selected before publication.
    std::unique_ptr<LegacyTopologyStorage> legacyTopology;
    std::unique_ptr<ScTopologyStorage> scTopology;
    std::unique_ptr<TcTopologyStorage> tcTopology;
    Scene scene=Scene::gc;
    // SA:74 spotlights+166 non-emitting ObjectPoints, retained in numeric order.
    static constexpr std::uint32_t maximumLocalLights=240;
    std::uint32_t localLightCount=0;
    std::array<AuthoredLocalLight,maximumLocalLights> localLights{};
    light::Vec3 sunLook{};
    light::Vec4 authoredSunSpecular{};
    std::span<light::Vec4> sun44{};
    bool prepareTa(Diagnostic&);
    bool prepareGa(Diagnostic&);
    bool prepareMaSa(Diagnostic&);
    bool selectAuthored(const LiveZoneMembership50EN&,const light::Vec3&,
        light::LightMatrices&,Diagnostic&) const noexcept;

    bool selectAuthoredZones(std::uint32_t,const LiveZoneMembership50EN*,
        const light::Vec3&,light::LightMatrices&,Diagnostic&) const noexcept;

    assets::World48F world;
    light::SmallDirectionalRegistry registry{};
    std::span<light::Vec4> ambient60{};
    light::Vec4 globalAmbient50{};
    bool fixedGcGlobalAmbient=false;
    struct GbTimedLights {
    light::Vec4 particleSunPositionA0{};
    std::thread::id particleThread=std::this_thread::get_id();
    const GbParticleLightLease* activeGbParticleLight=nullptr;
    std::uint64_t gbTimedServiceRevision=0;

        original_gb_light_flash50GB::AmbientState ambient{};
        std::array<original_gb_light_flash50GB::PointState,2> points{};
        std::array<light::Vec4,2> pointColors{};
        std::array<std::size_t,2> localIndices{};
        light::Vec4 savedGlobal{},flash{{68.5f,70.f,80.f,64.f}};
        std::array<light::Vec4,13> savedZones{};
    };
    std::unique_ptr<GbTimedLights> gbTimed;

    ResetVisibilityWorld50EY visibility{};
    std::span<SearchStorage50BS> registries{};
    std::span<SearchStorage50BS> scratchRegistries{};
    // Allocate once before publishing lifecycle pointers. RA's additional
    // identities correspond one-for-one to authenticated authored instances,
    // including unresolved slots. They are not arbitrary spare racer refs.
    struct RegistrationBacking {
        std::array<ZoneRegistration50CJ,maximumRacers+1u> original{};
        std::array<ZoneRegistration50CJ,maximumRacers+2u> tc{};
        bool useTc=false;
        std::vector<ZoneRegistration50CJ> ra;
        void resize(std::size_t count){if(!useTc&&count>original.size())ra.resize(count);}
        ZoneRegistration50CJ* data()noexcept{return useTc?tc.data():(ra.empty()?original.data():ra.data());}
        const ZoneRegistration50CJ* data()const noexcept{return useTc?tc.data():(ra.empty()?original.data():ra.data());}
        std::size_t size()const noexcept{return useTc?tc.size():(ra.empty()?original.size():ra.size());}
        ZoneRegistration50CJ& operator[](std::size_t i)noexcept{return data()[i];}
        const ZoneRegistration50CJ& operator[](std::size_t i)const noexcept{return data()[i];}
    } registrations;
    std::vector<const OriginalRacerModelSceneOwner50LA*> staticModels;
    struct StaticSelectionBinding {
        OriginalRacerModelSceneOwner50LA* model=nullptr;
        SceneGraph49P graph{};
        const void* observer=nullptr;
        const LiveZoneMembership50EN* membership=nullptr;
    };
    std::vector<StaticSelectionBinding> staticSelection;
    const assets::OriginalStandardMaterialTable50LA* staticMaterials=nullptr;

    std::span<SelectedMethod> selectedMethods;
    const ::ra_static_model_cold50LA::Owner* staticOwner=nullptr;
    const OriginalApplicationEntities50LA* staticApplication=nullptr;
    bool staticAccess=false;
    bool staticRegistrationComplete=false;
    const ::tc_gate_visibility50TC::Binding* tcGateOwner=nullptr;
    const LiveZoneMembership50EN* tcGateMembership=nullptr;
    bool tcGateAccess=false,tcGateComplete=false;
    struct TbDoorState {
        const ::tb_door_binding50TB::Binding* owner=nullptr;
        const LiveZoneMembership50EN* membership=nullptr;
        bool access=false,complete=false;
    } tbDoor;
    struct GbAnimationState {
        struct Row {
            const ::gb_animation_binding50GB::Binding* owner=nullptr;
            const LiveZoneMembership50EN* membership=nullptr;
            bool access=false,complete=false;
        };
        std::array<Row,4> rows{};
        bool claimed()const noexcept{for(const auto& r:rows)if(r.owner)return true;return false;}
        bool active()const noexcept{for(const auto& r:rows)if(r.access)return true;return false;}
        bool busy()const noexcept{for(const auto& r:rows)if(r.access||(r.owner&&!r.complete))return true;return false;}
    };
    std::unique_ptr<GbAnimationState> gbAnimations;
    struct McAnimationState {
        struct Row {
            const ::mc_animation_binding50MC::Binding* owner=nullptr;
            const LiveZoneMembership50EN* membership=nullptr;
            bool access=false,complete=false;
        };
        std::array<Row,14> rows{};
        bool claimed()const noexcept{for(const auto& r:rows)if(r.owner)return true;return false;}
        bool active()const noexcept{for(const auto& r:rows)if(r.access)return true;return false;}
        bool busy()const noexcept{for(const auto& r:rows)if(r.access||(r.owner&&!r.complete))return true;return false;}
    };
    std::unique_ptr<McAnimationState> mcAnimations;
    bool staticBusy()const noexcept{return activeMcParticleLight||(gbTimed&&gbTimed->activeGbParticleLight)||staticAccess||(staticOwner&&!staticRegistrationComplete)||
        tcGateAccess||(tcGateOwner&&!tcGateComplete)||tbDoor.access||(tbDoor.owner&&!tbDoor.complete)||
        (mcAnimations&&mcAnimations->busy())||(gbAnimations&&gbAnimations->busy());}
    struct RaFlareIdentity {
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        const OriginalWreckComponent50LA* entity=nullptr;
        const std::uint8_t* type32=nullptr;const std::uint8_t* flags33=nullptr;
        std::uint32_t object=0;
    } raFlare; //First metadata/Entity identity remains at the original address.
    std::array<RaFlareIdentity,14> raFlareTail{};
    std::size_t raFlareCount=0; //Actual owned source-order domain, at most15.
    RaFlareIdentity& raFlareAt(std::size_t i)noexcept{return i?raFlareTail[i-1u]:raFlare;}
    const RaFlareIdentity& raFlareAt(std::size_t i)const noexcept{return i?raFlareTail[i-1u]:raFlare;}
    const RaFlareIdentity* findRaFlare(std::uint32_t object)const noexcept {
        for(std::size_t i=0;i<raFlareCount;++i)if(raFlareAt(i).object==object)return &raFlareAt(i);
        return nullptr;
    }
    const RaFlareIdentity* findRaFlare(const OriginalWreckComponent50LA* entity)const noexcept {
        for(std::size_t i=0;i<raFlareCount;++i)if(raFlareAt(i).entity==entity)return &raFlareAt(i);
        return nullptr;
    }
    std::size_t identityCount()const noexcept{return racerCount+1u+staticModels.size()+raFlareCount+((scene==Scene::tc||scene==Scene::tb)?1u:scene==Scene::gb?4u:scene==Scene::mc?14u:0u);}
    bool disjointStaticBacking(const std::uint32_t* field)const noexcept {
        if(mcAnimations&&!disjointResetField(field,mcAnimations.get(),sizeof(*mcAnimations)))return false;
        if(mcTopology&&!disjointResetField(field,mcTopology.get(),sizeof(*mcTopology)))return false;
        if(gbAnimations&&!disjointResetField(field,gbAnimations.get(),sizeof(*gbAnimations)))return false;
        if((legacyTopology&&!disjointResetField(field,legacyTopology.get(),sizeof(*legacyTopology)))||
           (scTopology&&!disjointResetField(field,scTopology.get(),sizeof(*scTopology)))||
           (tcTopology&&!disjointResetField(field,tcTopology.get(),sizeof(*tcTopology))))return false;
        if(!((registrations.ra.empty()||disjointResetField(field,registrations.ra.data(),
            registrations.ra.size()*sizeof(ZoneRegistration50CJ)))&&
            (staticModels.empty()||disjointResetField(field,staticModels.data(),
            staticModels.size()*sizeof(staticModels[0])))&&
            (staticSelection.empty()||disjointResetField(field,staticSelection.data(),
             staticSelection.size()*sizeof(staticSelection[0])))))return false;
        for(const auto& method:selectedMethods)if(!method.selected18.empty()&&
            !disjointResetField(field,method.selected18.data(),method.selected18.size()*sizeof(method.selected18[0])))return false;
        return true;
    }
    std::span<CollectionTree50BX> methods{};
    std::span<CollectionZone50BX> zones{};
    const ::ra_pod_scene50LA::Owner* sceneOwner=nullptr;
    // Stable fields beside the SAME4C0 identities/registries. Constructor27F120
    // does not initialize these: disengaged optional means unproduced, not a
    // claimed source zero. No raw stamp addresses are exported to wreck/draw.

    std::span<ZoneProjection> zoneProjection;
    const CameraVisibilityLease* zoneProjectionLease=nullptr;
    //Same authenticated RA scene authoring retained for the future272FC0
    //environment method. Bounds publication does not construct443108.
    std::unique_ptr<original_world_environment50KG::Owner> environment3A4;
    std::span<AuthoredRenderZone> renderAuthoring{};
    bool renderAuthoringReady=false;
    std::span<SearchListBlock50BR> blocks{};
    std::span<CollectionRef50BX> portalHeads{};
    std::span<LiveZonePortalLink50EN> portalLinks{};
    std::span<LiveZonePortalLink50EN> scratchPortalLinks{};
    std::span<PortalGeometry50EO> portalGeometry{};
    std::span<CollectionZoneLink50BX> scratchLinks{};
    std::span<LiveZoneEntry50EN> scratchEntries{};
    LiveZoneWorkspace50EN workspace{};
    struct Vehicle {
        // This owns the existing7F0 throughout its lifetime. Never copy an
        // attached membership into a temporary reset adapter.
        ResetVisibilityOwner50EY resetOwner{};
        // Sole native representation of this racer's source C38/C40 recording
        //cache. Unknown before actual229F20 stores; never membership.bounds.
        std::optional<std::uint32_t> recordZoneC38;
        std::optional<SpringVector4_48N> recordCenterC40;
        enum class EndRecordCut:std::uint8_t {zone,bounds,returned};
        EndRecordCut endRecordCut=EndRecordCut::zone;

        std::span<CollectionZoneLink50BX> links{};
        std::span<LiveZoneEntry50EN> entries{};
        PodShapeSource49W source{};
        PodShapeState49W preparedPrefix{};
        const VehicleFrameState49W* resetFrame=nullptr; // Pending reset identity only.
        const VehicleFrameState49W* graphicsFrame=nullptr; // Last successful update identity only.
        SceneNodeIndex49P mainRoot=SceneNodeIndex49P::none;
        std::array<SceneNodeIndex49P,maxVehicleDrawComponents50CM> roots{};
        std::uint64_t graphicsSequence=0,graphicsMembershipRevision=0;
        bool pendingInitial=false,attached=false,resetPending=false,resetNeedsGraphics=false;
        SourceTriggerVisibilityIdentity50MB sourceResetIdentity{};
        bool sourceResetBound=false,sourceTerminalRetired=false;
        bool raColdRetired=false,mbColdRetired=false,tcColdRetired=false,tbColdRetired=false,gbColdRetired=false,mcColdRetired=false;
    };
    std::array<Vehicle,maximumRacers> vehicles{};
    template<class Backing> void bindTopology(Backing& b) noexcept {
        sun44=b.sun44;
        ambient60=b.ambient60;
        registries=b.registries;
        scratchRegistries=b.scratchRegistries;
        selectedMethods=b.selectedMethods;
        methods=b.methods;
        zones=b.zones;
        zoneProjection=b.zoneProjection;
        renderAuthoring=b.renderAuthoring;
        blocks=b.blocks;
        portalHeads=b.portalHeads;
        portalLinks=b.portalLinks;
        scratchPortalLinks=b.scratchPortalLinks;
        portalGeometry=b.portalGeometry;
        scratchLinks=b.scratchLinks;
        scratchEntries=b.scratchEntries;
        for(std::size_t i=0;i<vehicles.size();++i){
            vehicles[i].links=b.vehicleLinks[i];vehicles[i].entries=b.vehicleEntries[i];
        }
    }
    void prepareTopology(Scene selected){
        if(selected==Scene::mc){mcTopology=std::make_unique<McTopologyStorage>();bindTopology(*mcTopology);}
        else if(selected==Scene::tc){tcTopology=std::make_unique<TcTopologyStorage>();bindTopology(*tcTopology);}
        else if(selected==Scene::sc){scTopology=std::make_unique<ScTopologyStorage>();bindTopology(*scTopology);}
        else{legacyTopology=std::make_unique<LegacyTopologyStorage>();bindTopology(*legacyTopology);}
    }

    std::array<light::LightMatrices,maxVehicleDrawComponents50CM> selected{};
    std::array<bool,maxVehicleDrawComponents50CM> selectedReady{};
    std::uint32_t racerCount=0;
    std::uint64_t drawGeneration=0;
    const DrawLease* activeLease=nullptr;
    const CameraVisibilityLease* activeCamera=nullptr;
    const ResetVisibilityLease* activeReset=nullptr;
    std::uint64_t cameraGeneration=0;
    std::uint64_t resetGeneration=0;
    bool resetPending() const noexcept {
        for(std::uint32_t slot=0;slot<racerCount;++slot)
            if(vehicles[slot].resetPending||vehicles[slot].sourceResetBound)return true;
        return false;
    }
    bool resetBound(std::uint32_t slot) const noexcept {
        if(slot>=racerCount)return false;
        const auto& v=vehicles[slot];const auto& reset=v.resetOwner;
        return reset.object30==slot+1u&&reset.storage.links==v.links.data()&&
            reset.storage.entries==v.entries.data()&&reset.storage.capacity==v.links.size();
    }
    bool bound(std::uint32_t slot) const noexcept {
        if(!resetBound(slot))return false;
        const auto& v=vehicles[slot];const auto& m=v.resetOwner.membership7F0;
        if(!v.attached||!m.attached||m.object!=slot+1u||m.method!=ZoneMethod50CL::Second||
           m.storage.links!=v.links.data()||m.storage.entries!=v.entries.data()||
           m.storage.capacity!=v.links.size()||!v.entries[0].used)return false;
        const auto zone=v.links[0].zone;if(!zone||zone>world.zones.size())return false;
        const auto& r=registrations[slot];const auto& e=v.entries[0].registration;
        return r.owner==&registries[(zone-1u)*2u+1u]&&r.encodedObject==((slot+1u)<<2u)&&
            r.owner==e.owner&&r.slot==e.slot&&r.encodedObject==e.encodedObject;
    }
    bool writeCablesField50(std::uint32_t slot,const VehicleFrameState49W& frame,
        std::uint32_t& field50,Diagnostic& d) noexcept {
        if(!bound(slot))return fail(d,Status::identity,0x174360);
        auto& v=vehicles[slot];auto& reset=v.resetOwner;
        if(v.resetNeedsGraphics||(reset.object254Field50&&reset.object254Field50!=&field50)||
           !disjointResetField(&field50,this,sizeof(*this))||
           !disjointStaticBacking(&field50)||
           !disjointResetField(&field50,&frame,sizeof(frame)))
            return fail(d,Status::identity,0x174360);
        for(std::uint32_t other=0;other<racerCount;++other)
            if(other!=slot&&vehicles[other].resetOwner.object254Field50==&field50)
                return fail(d,Status::identity,0x174360);
        // Sole canonical7F0 identity, not a truncated guest/host pointer.
        // Visibility attachment never manufactures this graphics-side store.
        reset.object254Field50=&field50;field50=reset.membership7F0.object;return true;
    }
};
bool Owner::Storage::prepareTa(Diagnostic& d) {
    localLightCount=17;
    const auto& params=world.parameters;
    const auto* lights=block(params,"lights");
    const auto* environment=block(params,"environment");
    if(!lights||!textField(*lights,"numberoflights","26")||!environment||
       field(*environment,"ambientcolorflashminidletime"))return fail(d,Status::sourceFields,0x2783d0);
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
            //148F90/FB4/FD8/FFC updates global+50 for EVERY registered
            // ambient, before the zone0-only all-row propagation branch.
            // Numeric Light2..9 order therefore leaves the actual last
            // ambient here; selected-zone60 is never a Binder substitute.
            globalAmbient50=row;fixedGcGlobalAmbient=true;
            if(zone==1)for(auto& current:ambient60)current=row;
            else ambient60[zone-1]=row;
        }else{
            auto& spot=localLights[i-10];spot.sourceOrdinal=i;float inner=0,outer=0;
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
    localLightCount=36;
    for(std::uint32_t index=0;index<localLightCount;++index){
        const auto ordinal=index+3u;
        const auto name="light"+std::to_string(ordinal);const auto* b=block(params,name.c_str());
        if(!b||!textField(*b,"type","Spotlight")||field(*b,"hasflasher"))
            return fail(d,Status::sourceFields,0x277390);
        auto& spot=localLights[index];spot.sourceOrdinal=ordinal;light::Vec3 rgb{};float inner=0.f,outer=0.f;
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
bool Owner::Storage::prepareMaSa(Diagnostic& d) {
    if(scene!=Scene::ma&&scene!=Scene::sa&&scene!=Scene::ra&&scene!=Scene::mb&&scene!=Scene::sc&&scene!=Scene::tc&&scene!=Scene::rc&&scene!=Scene::tb&&scene!=Scene::gb)return fail(d,Status::unsupportedScene,0x278290);
    const bool mb=scene==Scene::mb;
    const bool sc=scene==Scene::sc;
    const bool tc=scene==Scene::tc;
    const bool rc=scene==Scene::rc;
    const bool tb=scene==Scene::tb;
    const bool gb=scene==Scene::gb;
    const bool ra=scene==Scene::ra;
    const bool sa=scene==Scene::sa;
    const auto& params=world.parameters;
    const auto* lights=block(params,"lights");
    const auto* environment=block(params,"environment");
    if(!lights||!environment||(!gb&&field(*environment,"ambientcolorflashminidletime")))
        return fail(d,Status::sourceFields,0x270e9c);
    auto scalar=[](const assets::ParameterBlock48F& b,const char* key,float& value){
        const auto* f=field(b,key);
        return f&&assets::parseTrackNumber50AD(f->value.data(),f->value.size(),value);
    };
    auto ordinal=[](const assets::ParameterBlock48F& b,const char* key,
                    std::uint32_t fallback,std::uint32_t& value){
        const auto* f=field(b,key);value=fallback;if(!f)return true;
        if(f->value.empty())return false;
        value=0;
        for(char c:f->value){
            if(c<'0'||c>'9'||value>(std::numeric_limits<std::uint32_t>::max()-
                static_cast<unsigned>(c-'0'))/10u)return false;
            value=value*10u+static_cast<unsigned>(c-'0');
        }
        return true;
    };
    auto flag=[](const assets::ParameterBlock48F& b,const char* key,bool fallback,bool& value){
        const auto* f=field(b,key);value=fallback;if(!f)return true;
        // Narrow admission of the authentic authoring's integer/T spellings.
        //2776FC/27772C/277764 retain bit0 of the original integer-reader result.
        if(f->value=="1"||f->value=="T"){value=true;return true;}
        if(f->value=="0"||f->value=="F"){value=false;return true;}
        return false;
    };
    std::uint32_t numberOfLights=0;
    if(!ordinal(*lights,"numberoflights",0u,numberOfLights))
        return fail(d,Status::sourceFields,0x2782e0);
    course_archive_receipt50CI::countReceipt("lighting","lights",numberOfLights,(gb?62u:tb?104u:rc?16u:tc?2u:sc?311u:mb?384u:ra?185u:sa?263u:43u));
    std::uint32_t ambientCount=0,directionalCount=0,pointCount=0,emittingPoints=0,spotlights=0;
    std::uint32_t omittedMbNonEmit=0,mbFlasherCount=0,omittedScNonEmit=0;
    localLightCount=0;
    std::size_t gbFlasherCount=0;
    if(gb){
        float minIdle=0,idle=0,minDuration=0,duration=0;light::Vec3 flash{};
        if(!scalar(*environment,"ambientcolorflashminidletime",minIdle)||minIdle!=15.f||
           !scalar(*environment,"ambientcolorflashidletime",idle)||idle!=25.f||
           !scalar(*environment,"ambientcolorflashminduration",minDuration)||minDuration!=.2f||
           !scalar(*environment,"ambientcolorflashduration",duration)||duration!=.24f||
           !triple(*environment,"ambientcolorflashcolorrgb",flash)||flash!=light::Vec3{{137.f,140.f,160.f}})
            return fail(d,Status::sourceFields,0x270e9c);
        gbTimed=std::make_unique<GbTimedLights>();
    }
    //2783D0..278408 reads numeric Light1..NumberOfLights. A section scan
    //would wrongly include MA's unrelated Light276 or reorder Light10/Light2.
    for(std::uint32_t sourceOrdinal=1;sourceOrdinal<=numberOfLights;++sourceOrdinal){
        const auto name="light"+std::to_string(sourceOrdinal);
        const auto* b=block(params,name.c_str());
        std::uint32_t effectiveOrdinal=sourceOrdinal;
        //2773CC ignores failed157D00;157DD8 preserves the prior selector.
        //Only these two missing blocks occur in the authenticated RA corpus.
        if(ra&&(sourceOrdinal==90u||sourceOrdinal==107u)){
            if(b)return fail(d,Status::sourceFields,0x157dd8);
            effectiveOrdinal=sourceOrdinal-1u;
            const auto retained="light"+std::to_string(effectiveOrdinal);
            b=block(params,retained.c_str());
        }
        // Authenticated TB declares104 slots but ends atLight92. Failed
        //157D00 preserves the selected block, so each final slot rereads92.
        if(tb&&sourceOrdinal>=93u&&sourceOrdinal<=104u){
            if(b)return fail(d,Status::sourceFields,0x157dd8);
            effectiveOrdinal=92u;b=block(params,"light92");
        }
        if(!b||(!mb&&!gb&&field(*b,"hasflasher")))return fail(d,Status::sourceFields,0x277390);
        light::Vec3 rgb{};std::uint32_t zone=0;
        if(!triple(*b,"colorrgb",rgb)||!ordinal(*b,"initialzone",1u,zone)||
           !zone||zone>world.zones.size())return fail(d,Status::sourceFields,0x277494);
        for(float& c:rgb)c=mul(c,.5f); //146FDC..147014 Light::Init.
        if(textField(*b,"type","Ambient")){
            ++ambientCount;
            const light::Vec4 row{{rgb[0],rgb[1],rgb[2],64.f}};
            //148F90/FB4/FD8/FFC writes+50 for EVERY numeric registration.
            globalAmbient50=row;fixedGcGlobalAmbient=true;
            //2774C8 absent InitialZone=1,2774FC -> source zone0.149000..149040
            //then propagates to all rows. In SA both Light21 and Light22 take
            //this path, replacing the earlier per-zone ambients legitimately.
            if(zone==1)for(auto& current:ambient60)current=row;
            else ambient60[zone-1]=row;
            continue;
        }
        bool emits=true,infinity=true;
        if(!flag(*b,"emitslight",true,emits)||!flag(*b,"placelightatinfinity",true,infinity))
            return fail(d,Status::sourceFields,0x2776cc);
        namespace poseSource=assets::original_light_pose_defaults_candidate;
        poseSource::FreshPose pose;poseSource::Diagnostic poseDiagnostic;
        if(!poseSource::readFreshPose(*b,static_cast<std::uint32_t>(world.zones.size()),pose,poseDiagnostic))
            return fail(d,Status::sourceFields,poseDiagnostic.sourcePC);
        if(pose.initialZone!=zone)return fail(d,Status::sourceFields,0x277500);
        if(textField(*b,"type","Directional")){
            if(++directionalCount!=1u||!emits||!infinity||zone!=1u)
                return fail(d,Status::sourceFields,0x1490c0);
            sunLook=pose.effectiveLook78;
            if(gb)gbTimed->particleSunPositionA0=pose.positionA0;
            authoredSunSpecular={{rgb[0],rgb[1],rgb[2],0.f}};
            //1490E0 propagates the initial directional color only for zone0.
            for(auto& row:sun44)row=authoredSunSpecular;
            continue;
        }
        const bool spotlight=textField(*b,"type","Spotlight");
        if(!spotlight&&!textField(*b,"type","ObjectPoint"))
            return fail(d,Status::sourceFields,0x277b70);
        // Validate every numeric source row before native MB/SC non-emitter filtering.
        AuthoredLocalLight checked{};auto& local=checked;
        // Authenticated GB has44 explicit F rows; presence alone does not enable a flasher.
        if(const auto* flasher=field(*b,"hasflasher");flasher&&(!gb||flasher->value!="F")){
            if(gb){
                if(spotlight||!emits||flasher->value!="T"||gbFlasherCount>=2u||
                   sourceOrdinal!=(gbFlasherCount==0u?7u:34u))return fail(d,Status::sourceFields,0x277390);
                float period=0,filter=0;const auto* pattern=field(*b,"flashpattern");
                if(!pattern||pattern->value!="0,1,1,0,1,1,1,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1"||
                   !scalar(*b,"flashperiod",period)||period!=2.f||!scalar(*b,"flashfilter",filter)||filter!=.1f||
                   rgb!=light::Vec3{{127.5f,127.5f,127.5f}})return fail(d,Status::sourceFields,0x2779e4);
                //26FC10 defaults alpha255;146FDC..147014 halves all four.
                auto& color=gbTimed->pointColors[gbFlasherCount];color={{rgb[0],rgb[1],rgb[2],127.5f}};
                if(!original_gb_light_flash50GB::initializePoint(0x6ed66ebbu,color,gbTimed->points[gbFlasherCount]))
                    return fail(d,Status::arithmetic,0x146b10);
                gbTimed->localIndices[gbFlasherCount++]=localLightCount;
            }else{
                if(!mb||spotlight||emits||flasher->value!="T")return fail(d,Status::sourceFields,0x277390);
                ++mbFlasherCount;
            }
        }
        local.sourceOrdinal=sourceOrdinal;local.effectiveSourceOrdinal=effectiveOrdinal;
        local.zone=zone;local.spotlight=spotlight;
        local.emitsLight=emits;local.placeAtInfinity=infinity;
        for(unsigned c=0;c<3;++c)local.position[c]=pose.positionA0[c];
        local.look=pose.effectiveLook78;local.rgb=rgb;
        //277638..277678 reader defaults:0x3CF5C28F/3B03126F/3A83126F.
        //SA flare-only ObjectPoints omit Attenuation. Retain those reader
        //values; do not evaluate them as emitted light (277DDC..277E04).
        local.attenuation={{.03f,.002f,.001f}};
        if(field(*b,"attenuation")&&!triple(*b,"attenuation",local.attenuation))
            return fail(d,Status::sourceFields,0x277674);
        for(float a:local.attenuation)if(!std::isfinite(a)||a<0.f)
            return fail(d,Status::sourceFields,0x146f60);
        if(spotlight){
            ++spotlights;
            //277694..2776C8 defaults30/45 degrees. Actual admitted spotlights
            //supply both fields. MA/SA retain ordinary native cos; RA uses the
            //bounded recovered source polynomial below.
            float inner=30.f,outer=45.f;
            if((field(*b,"innerconeangle")&&!scalar(*b,"innerconeangle",inner))||
               (field(*b,"outerconeangle")&&!scalar(*b,"outerconeangle",outer))||
               (!ra&&(inner<0.f||outer<0.f||outer>180.f||!emits||
                 (mb&&sourceOrdinal==226u?(inner!=0.f||outer!=0.f):inner>=outer)))||
               (ra&&(inner<0.f||outer<0.f||inner>180.f||outer>180.f||!emits||
                 (sourceOrdinal!=129u&&inner>=outer)||
                 (sourceOrdinal==129u&&(inner!=60.f||outer!=0.f)))))
                return fail(d,Status::sourceFields,0x2776a8);
            if(ra){
                local.innerCos=raConeCosine(inner);local.outerCos=raConeCosine(outer);
                if(!std::isfinite(local.innerCos)||!std::isfinite(local.outerCos)||
                   (sourceOrdinal!=129u&&!(local.innerCos>local.outerCos)))
                    return fail(d,Status::arithmetic,0x149fe4);
                //129 is source-authored60/0.149FFC first tests dot<outerCos;
                //otherwise14A02C tests dot<=innerCos. For finite input this
                //inverted pair never enters the interpolation denominator.
            }else{
                constexpr float halfDegree=3.14159265358979323846f/360.f;
                local.innerCos=std::cos(mul(inner,halfDegree));
                local.outerCos=std::cos(mul(outer,halfDegree));
                if(!(local.innerCos>local.outerCos)&&
                   !(mb&&sourceOrdinal==226u&&local.innerCos==1.f&&local.outerCos==1.f))
                    return fail(d,Status::arithmetic,0x149fe4);
            }
        }else{
            ++pointCount;if(emits)++emittingPoints;
            //ObjectPoint has no cone. Absent look remains the fresh source+X
            //cache, but local direction is computed from position at draw time.
        }
        // Existing native selection skips every non-emitter before ranking.
        // Preserve that query exactly without increasing its240-slot owner.
        // Full authenticated source rows remain retained in world.parameters.
        if(mb&&!spotlight&&!emits){++omittedMbNonEmit;continue;}
        if(sc&&!spotlight&&!emits){++omittedScNonEmit;continue;}
        if(localLightCount>=localLights.size())return fail(d,Status::bounds,0x2783dc);
        localLights[localLightCount++]=checked;
    }
    // Stock light-type counts are receipts; every light was read from the data
    // and stored within the 240-slot local-light store (bounds-checked above).
    if(ambientCount!=(gb?1u:tb?10u:rc?15u:tc?1u:sc?30u:mb?16u:ra?4u:sa?22u:9u)||directionalCount!=1u||pointCount!=(gb?60u:tb?93u:(rc||tc)?0u:sc?165u:mb?337u:ra?15u:sa?166u:20u)||
       emittingPoints!=(gb?60u:tb?92u:(rc||tc)?0u:sc?19u:mb?124u:ra?15u:sa?0u:14u)||spotlights!=((gb||tb||rc||tc)?0u:sc?115u:mb?30u:ra?165u:sa?74u:13u)||localLightCount!=(gb?60u:tb?93u:(rc||tc)?0u:sc?134u:mb?154u:ra?180u:sa?240u:33u)||
       (mb&&(omittedMbNonEmit!=213u||mbFlasherCount!=144u))||
       (sc&&omittedScNonEmit!=146u)||(gb&&gbFlasherCount!=2u))
        std::cerr<<"course_count site=lighting light_types ambient="<<ambientCount<<" directional="<<directionalCount
                 <<" points="<<pointCount<<" emitting="<<emittingPoints<<" spots="<<spotlights<<" local="<<localLightCount<<" stock=0\n";
    if(gb){
        if(ambient60.size()<13u)return fail(d,Status::bounds,0x149000);
        gbTimed->savedGlobal=globalAmbient50;
        std::copy_n(ambient60.begin(),13u,gbTimed->savedZones.begin());
    }
    //278370..2783A4 ->1498A0 DIRECT copy, after all light registrations.
    //These authored per-zone overrides must never be halved a second time.
    for(std::size_t z=0;z<world.zones.size();++z){
        const auto key="directionallightcolorforzone"+std::to_string(z+1u);
        if(field(*lights,key.c_str())){
            light::Vec3 rgb{};
            if(!triple(*lights,key.c_str(),rgb))return fail(d,Status::sourceFields,0x278370);
            for(unsigned c=0;c<3;++c)sun44[z][c]=rgb[c];
        }
    }
    return true;
}
bool Owner::Storage::selectAuthored(const LiveZoneMembership50EN& member,
    const light::Vec3& position,light::LightMatrices& output,Diagnostic& d) const noexcept {
    for(float x:position)if(!std::isfinite(x))return fail(d,Status::arithmetic,0x2413a8);
    if(!member.attached||!member.storage.links||!member.storage.capacity)
        return fail(d,Status::identity,0x14a118);
    const auto primary=member.storage.links[0].zone;
    if(!primary||primary>world.zones.size())return fail(d,Status::identity,0x14a120);
    return selectAuthoredZones(primary,&member,position,output,d);
}
bool Owner::Storage::selectAuthoredZones(std::uint32_t primary,const LiveZoneMembership50EN* membership,
    const light::Vec3& position,light::LightMatrices& output,Diagnostic& d) const noexcept {
    // Null membership means precisely the owned static placement zone.
    for(float x:position)if(!std::isfinite(x))return fail(d,Status::arithmetic,0x2413a8);
    if(!primary||primary>world.zones.size())return fail(d,Status::identity,0x14a120);
    light::LightMatrices next{};
    next.diffuse[3]=ambient60[primary-1];next.diffuse[0]=sun44[primary-1];
    //MA/SA overrides change diffuse44, not the directional light's own4C.
    //Preserve the preexisting TA/GA native specular branch unchanged.
    next.specular[0]=(scene==Scene::ma||scene==Scene::sa||scene==Scene::ra||scene==Scene::mb||scene==Scene::sc||scene==Scene::tc||scene==Scene::rc||scene==Scene::tb||scene==Scene::gb||scene==Scene::mc)?authoredSunSpecular:sun44[0];
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
    if(localLightCount>localLights.size())return fail(d,Status::bounds,0x14a118);
    for(std::uint32_t localIndex=0;localIndex<localLightCount;++localIndex){
        const auto& spot=localLights[localIndex];
        // Explicit source EmitsLight=0 remains non-emitting.277DDC..277E04
        // gives its flare-only runtime light FLT_MAX attenuation. This native
        // branch omits it entirely, never inventing a rendered flare or light.
        if(!spot.emitsLight)continue;
        bool eligible=!membership&&spot.zone==primary;
        auto memberLink=CollectionRef50BX{membership?1u:0u};std::size_t count=0;
        while(memberLink){
            const auto& member=*membership;
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
        float penalty=1.f;
        //14A380..14A398 applies149F20 only to type3. MA type1 ObjectPoint
        //uses the same source-distance denominator without a cone multiplier.
        if(spot.spotlight){
            float cosine=0.f;for(unsigned c=0;c<3;++c)cosine=add(cosine,mul(toward[c],-spot.look[c]));
            if(cosine<spot.outerCos)penalty=5.f;
            else if(cosine<=spot.innerCos){
                volatile float fraction=sub(spot.innerCos,cosine)/sub(spot.innerCos,spot.outerCos);
                penalty=add(sub(1.f,fraction),mul(5.f,fraction));
            }
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

bool Owner::raStaticCurrent() const noexcept {
    return storage_&&!invalidated_&&storage_->scene==Scene::ra&&
        !storage_->staticBusy()&&!storage_->activeLease&&!storage_->activeCamera&&!storage_->activeReset&&!storage_->resetPending();
}
bool Owner::selectRaStatic(std::uint32_t zone,const std::array<float,3>& authoredAnchor,
    light::LightMatrices& out,Diagnostic& d) const noexcept {
    d={};if(!raStaticCurrent())return fail(d,Status::stalePublication,0x2413a8);
    for(float v:authoredAnchor)if(!std::isfinite(v))return fail(d,Status::arithmetic,0x2413a8);
    return storage_->selectAuthoredZones(zone,nullptr,authoredAnchor,out,d);
}

Owner::Owner() noexcept=default;
Owner::~Owner(){
    if(storage_&&storage_->activeMcParticleLight)std::terminate();
    if(storage_&&storage_->gbTimed&&storage_->gbTimed->activeGbParticleLight)std::terminate();
    if(storage_)for(std::uint32_t slot=0;slot<storage_->racerCount;++slot)
        if(storage_->vehicles[slot].sourceResetBound)std::terminate();
    if(storage_&&(storage_->raFlareCount||storage_->staticOwner||storage_->sceneOwner||storage_->tcGateOwner||storage_->tbDoor.owner||
        (storage_->mcAnimations&&storage_->mcAnimations->claimed())||(storage_->gbAnimations&&storage_->gbAnimations->claimed())))std::terminate();
    if(storage_&&storage_->activeLease)endDraw(*const_cast<DrawLease*>(storage_->activeLease));
    if(storage_&&storage_->activeCamera)endCameraVisibility(*const_cast<CameraVisibilityLease*>(storage_->activeCamera));
    if(storage_&&storage_->activeReset)endResetVisibility(*const_cast<ResetVisibilityLease*>(storage_->activeReset));
}
DrawLease::~DrawLease(){if(owner_)owner_->endDraw(*this);}
const assets::World48F* Owner::world() const noexcept {return storage_?(storage_->mcSource?&storage_->mcSource->world():&storage_->world):nullptr;}
bool Owner::prepare(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::gc,d);
}
bool Owner::prepareTaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::ta,d);
}
bool Owner::prepareGaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::ga,d);
}
bool Owner::prepareMaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::ma,d);
}
bool Owner::prepareSaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::sa,d);
}
bool Owner::prepareRaNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::ra,d);
}
bool Owner::prepareGbNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::gb,d);
}
bool Owner::prepareTbNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::tb,d);
}
bool Owner::prepareRcNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::rc,d);
}
bool Owner::prepareTcNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::tc,d);
}
bool Owner::prepareScNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::sc,d);
}
bool Owner::prepareMbNative(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareScene(scb,racers,Scene::mb,d);
}
bool Owner::prepareScene(const std::vector<std::uint8_t>& scb,std::uint32_t racers,Scene scene,Diagnostic& d,std::shared_ptr<const ::mc_cold_sources50LA::Owner> source) noexcept {
    d={};if(invalidated_)return fail(d,Status::unprepared,0x272a48);
    if(storage_)return fail(d,Status::alreadyPrepared,0x272a48);
    if(!racers||racers>maximumRacers)return fail(d,Status::identity,0x1ae578);
    std::size_t expectedBytes=0,expectedZones=0,expectedPortals=0;
    const char* expectedSha=nullptr;
    switch(scene){
    case Scene::mc:
        expectedBytes=187711u;expectedZones=28u;expectedPortals=28u;
        expectedSha="26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e";break;
    case Scene::gb:
        expectedBytes=401867u;expectedZones=13u;expectedPortals=17u;
        expectedSha="ed0eb494f8c91df5264e120bad3780016f67f297f259572d48a3e7a56aad27bb";break;
    case Scene::tb:
        expectedBytes=448107u;expectedZones=10u;expectedPortals=11u;
        expectedSha="c3536c6e76dc4d6836ee154aabc80f5e73bd80e85189b841538b3a3ce53e8184";break;
    case Scene::rc:
        expectedBytes=468918u;expectedZones=15u;expectedPortals=15u;
        expectedSha="1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a";break;
    case Scene::tc:
        expectedBytes=232284u;expectedZones=25u;expectedPortals=26u;
        expectedSha="795bbbf9882baf233b9d67f777d0bb1e36506dda0f1ad7e71a303faae3993d24";break;
    case Scene::sc:
        expectedBytes=1651653u;expectedZones=30u;expectedPortals=34u;
        expectedSha="784fb4f60a52ec23ee4c4f7082dd4fb44393421c5336b56f03d49b73f6b8e1ee";break;
    case Scene::mb:
        expectedBytes=621304u;expectedZones=16u;expectedPortals=17u;
        expectedSha="e64bd3c7085bd92ef0e6486bbfc2fcfed7aae4462858dcb73a5ef3c893233148";break;
    case Scene::ra:
        expectedBytes=540795u;expectedZones=10u;expectedPortals=10u;
        expectedSha="57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81";break;
    case Scene::gc:
        expectedBytes=359830u;expectedZones=gcZones;expectedPortals=gcPortals;
        expectedSha="d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d";break;
    case Scene::ta:
        expectedBytes=130511u;expectedZones=9u;expectedPortals=10u;
        expectedSha="c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d";break;
    case Scene::ga:
        expectedBytes=292650u;expectedZones=gcZones;expectedPortals=gaPortals;
        expectedSha="ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7";break;
    case Scene::ma:
        expectedBytes=404992u;expectedZones=maZones;expectedPortals=maPortals;
        expectedSha="5766af3e7a0bba0644058ae6f31eab802cadd9be7cc71911c349f603c7ca873b";break;
    case Scene::sa:
        expectedBytes=813441u;expectedZones=saZones;expectedPortals=saPortals;
        expectedSha="dd38f087f0438b2e1279651fe3b4f525b053c90d34cf5365d6bf4ec6c7d1d394";break;
    }
    // Common course rule (course_archive_receipt50ci.h): the SCB's stock
    // identity is a receipt; the scene is decoded from whatever SCB was read.
    if(!authenticScb(scb,expectedBytes,expectedSha))
        std::cerr<<"course_member_identity site=lighting member=scb bytes="<<scb.size()<<" stock=0\n";
    if(!light::selection_detail::arithmeticReady())return fail(d,Status::arithmetic,0x146f60);
    try {
        auto next=std::make_unique<Storage>();
        next->prepareTopology(scene);
        if(scene==Scene::mc){
            if(!source||!source->ready()||&scb!=&source->world().parameters.original_bytes)
                return fail(d,Status::sourceIdentity,0x270b10);
            next->mcSource=std::move(source);next->world=next->mcSource->world();
        }
        else if(!assets::decodeWorld48F(scb,next->world,d.decode))return fail(d,Status::sourceShape,0x270b10);
        const auto& w=next->world;
        // Zone/portal counts from the data, bounded by this scene's topology
        // storage (the stock counts are receipts).
        course_archive_receipt50CI::countReceipt("lighting","zones",w.zones.size(),expectedZones);
        course_archive_receipt50CI::countReceipt("lighting","portals",w.portals.size(),expectedPortals);
        if(w.zones.empty()||w.zones.size()>next->zones.size()||w.portals.size()>next->portalGeometry.size())
            return fail(d,Status::unsupportedScene,0x272a48);
        next->scene=scene;
        if(scene==Scene::ra){
            next->environment3A4=std::make_unique<original_world_environment50KG::Owner>();
            original_world_environment50KG::Diagnostic environment;
            if(!next->environment3A4->prepare(w,environment)||
               !next->environment3A4->prepareSpatialBounds276F3C(environment))
                return fail(d,Status::sourceFields,environment.sourcePC);
        }
        if(scene==Scene::mc){
            ::mc_lighting_authoring50MC::Owner authoring;::mc_lighting_authoring50MC::Diagnostic proof;
            if(!authoring.prepare(next->mcSource->world(),proof))return fail(d,Status::sourceFields,proof.sourcePC);
            const auto& rows=*authoring.source();next->mcRecords=rows.records;
            std::copy(rows.sun44.begin(),rows.sun44.end(),next->sun44.begin());
            std::copy(rows.ambient60.begin(),rows.ambient60.end(),next->ambient60.begin());
            next->globalAmbient50=rows.globalAmbient50;next->fixedGcGlobalAmbient=true;next->authoredSunSpecular=rows.authoredSunSpecular;
            next->sunLook=rows.sunLook;
        }
        else if(scene==Scene::ma||scene==Scene::sa||scene==Scene::ra||scene==Scene::mb||scene==Scene::sc||scene==Scene::tc||scene==Scene::rc||scene==Scene::tb||scene==Scene::gb){if(!next->prepareMaSa(d))return false;}
        else if(scene==Scene::ga){if(!next->prepareGa(d))return false;}
        else if(scene==Scene::ta){if(!next->prepareTa(d))return false;}else{
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
        for(std::uint32_t slot=0;slot<racers;++slot){
            auto& v=next->vehicles[slot];v.resetOwner.object30=slot+1u;
            v.resetOwner.storage={v.links.data(),v.entries.data(),v.links.size()};
            // No254 field is invented by cold lighting preparation.
        }
        for(std::size_t z=0;z<w.zones.size();++z){
            const auto read=assets::readZoneSearchSelection50CN(w,z);
            if(w.zones[z].zone!=z+1||read.status!=assets::ZoneSearchReadStatus50CN::Complete||
               read.selection.visibilityDynamic114!=assets::ZoneSearchKind50CN::List||
               (scene==Scene::ra&&read.selection.visibilityStatic110!=assets::ZoneSearchKind50CN::List))
                return fail(d,Status::unsupportedScene,0x272a58);
            // 27632C reads every zone's ExclusionList on every course (absent
            // = empty list, as on TB). Not limited to GA/RA.
            {
                const auto index=w.zones[z].parameter_block;
                if(index>=w.parameters.blocks.size()||
                   !readZoneRenderAuthoring(w.parameters.blocks[index],w.zones.size(),next->renderAuthoring[z]))
                    return fail(d,Status::sourceFields,0x27632c);
            }
        }
        next->renderAuthoringReady=true;
        if(scene==Scene::ra){next->staticModels.resize(w.instances.size(),nullptr);
            next->staticSelection.resize(w.instances.size());}
        if(scene==Scene::mc)next->mcAnimations=std::make_unique<Storage::McAnimationState>();
        if(scene==Scene::gb)next->gbAnimations=std::make_unique<Storage::GbAnimationState>();
        next->registrations.useTc=scene==Scene::tc;
        next->registrations.resize(next->identityCount());
        if(scene==Scene::gb&&next->registrations.ra.capacity()>maximumRacers+5u)
            return fail(d,Status::allocation,0x28e8a0);
        if(scene==Scene::tb&&next->registrations.ra.capacity()>maximumRacers+2u)
            return fail(d,Status::allocation,0x28e8a0);
        const ZoneLifecycleStorage50CL lifeStorage{next->registries.data(),next->registries.size(),
            next->registrations.data(),next->registrations.size(),
            {next->methods.data(),next->methods.size(),next->zones.data(),next->zones.size(),
             next->blocks.data(),next->blocks.size()}};
        // Reserve the actual camera scene30 registration independently of
        // the existing racer1..N identities on every authenticated world.
        if(initializeZoneLifecycle50CL(next->visibility.lifecycle,lifeStorage,w.zones.size(),next->identityCount(),1024,
            next->workspace.transfer)!=SearchStorageStatus50BS::Complete)
            return fail(d,Status::registration,0x272a58);
        auto preparePortals=[&]<std::size_t PortalCount>() -> bool {
        std::array<LiveZonePortalInput50EN,PortalCount> input{};
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
        return true;
        };
        if(!(scene==Scene::mc?preparePortals.template operator()<28>():
            scene==Scene::tc?preparePortals.template operator()<26>():
            scene==Scene::sc?preparePortals.template operator()<34>():
            preparePortals.template operator()<maximumPortals>()))return false;
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
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x2167a0);
    if(slot>=o.racerCount||!frameBinding(s,b))return fail(d,Status::identity,0x2167a0);
    auto& v=o.vehicles[slot];
    if(v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.mcColdRetired||v.gbColdRetired||v.attached||v.resetOwner.membership7F0.attached)return fail(d,Status::alreadyPrepared,0x229f30);
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
    d.membership=attachLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,v.resetOwner.membership7F0,backing,
        slot+1u,1u,ZoneMethod50CL::Second,s.shape.radius80,bounds,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x21688c);
    v.attached=true;v.pendingInitial=false;return true;
}
bool Owner::readRecordedZoneC38(std::uint32_t slot,std::uint32_t& value)const noexcept {
    if(!storage_||invalidated_||storage_->scene!=Scene::ra||slot>=storage_->racerCount||
       !storage_->resetBound(slot))return false;
    const auto& v=storage_->vehicles[slot];
    if(v.raColdRetired||v.sourceTerminalRetired||!v.recordZoneC38)return false;
    value=*v.recordZoneC38;return true;
}
bool Owner::readRecordedCenterC40(std::uint32_t slot,std::array<std::uint32_t,4>& value)const noexcept {
    if(!storage_||invalidated_||storage_->scene!=Scene::ra||slot>=storage_->racerCount||
       !storage_->resetBound(slot))return false;
    const auto& v=storage_->vehicles[slot];
    if(v.raColdRetired||v.sourceTerminalRetired||!v.recordCenterC40)return false;
    std::memcpy(value.data(),v.recordCenterC40->v,16);return true;
}
bool Owner::recordAttachedEndRacer229F20(std::uint32_t slot,const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    Diagnostic& d)noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::ra)
        return fail(d,Status::identity,0x229f20);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x229f20);
    if(slot>=o.racerCount||!frameBinding(frame,bindings)||!o.resetBound(slot))
        return fail(d,Status::identity,0x229f38);
    auto& v=o.vehicles[slot];
    if(v.raColdRetired||v.sourceTerminalRetired||!o.bound(slot))
        // Native missing/retired membership is not proof of source-null7F0.
        // The original2167A0 null branch remains unsupported, never simulated.
        return fail(d,Status::registration,0x229f38);
    using Cut=Storage::Vehicle::EndRecordCut;
    if(v.endRecordCut==Cut::zone){
        // Source7F0->4 is the ACTUAL primary zone (links[0]); authenticated
        //272AD4/AE4 assign zone+48 its zero-based authored ordinal. No nearest
        //zone, collision C34, static draw selection or cached bounds is used.
        v.recordZoneC38=v.links[0].zone-1u; //229F5C commits BEFORE bounds.
        v.endRecordCut=Cut::bounds;
    }
    if(v.endRecordCut==Cut::bounds){
        //694 is already live. Do NOT recompute11FE50, attach/update membership,
        //reset the frame, or use the previous membership.bounds snapshot.
        if(frame.shape.type88||std::memcmp(&v.source,&parameters.shape,sizeof(v.source)))
            return fail(d,Status::sourceShape,0x229f60);
        Type0BoundsInput50EW input;
        input.resource=&v.source;input.composed00=frame.shape.composed00;
        std::memcpy(input.cachedCenterWords,frame.shape.center60.v,16);
        std::memcpy(&input.cachedRadiusWord,&frame.shape.radius80,4);
        CollectionBoundsResult48S bounds;
        if(prepareType0Bounds50EW(input,bounds)!=CollectionBoundsStatus48S::Complete)
            return fail(d,Status::bounds,0x1202e0);
        //120CD4 writes full center vector to A1=C40. HalfXYZ go only to
        //229F34's zeroed stack20 temporary; half.W is untouched/discarded.
        v.recordCenterC40=bounds.center;
        v.endRecordCut=Cut::returned;
    }
    d.status=Status::complete;d.sourcePC=0x229f74;return true;
}
bool Owner::matchesRaFreshRoster(std::uint32_t count) const noexcept {
    if(!storage_||invalidated_||storage_->scene!=Scene::ra||storage_->racerCount!=count)return false;
    for(std::uint32_t slot=0;slot<count;++slot)if(!storage_->resetBound(slot))return false;
    return true;
}
bool Owner::retireRaFreshVehicle(std::uint32_t slot,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||storage_->scene!=Scene::ra||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x21688c);
    auto& o=*storage_;auto& v=o.vehicles[slot];
    if(o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x2167a0);
    if(!o.resetBound(slot))return fail(d,Status::identity,0x21688c);
    // Terminal native teardown; failed removal retains its exact membership.
    v.raColdRetired=true;
    d.membership=removeLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,
        v.resetOwner.membership7F0,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x21688c);
    v.attached=false;v.pendingInitial=false;return true;
}
bool Owner::retireMbColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::mb||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
    // A failure before fresh attachment retains the genuine empty prefix.
    // Once attachment starts, source shape/root identity must still match.
    if(v.mainRoot!=SceneNodeIndex49P::none){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape)))return fail(d,Status::identity,0x216a90);
    }else if(v.attached||v.pendingInitial||reset.membership7F0.attached||reset.membership7F0.object)
        return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=slot&&o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||
       registration.slot||registration.encodedObject||*field50)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.pendingInitial=false;v.mbColdRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::tcColdVehicleUntouched(std::uint32_t slot,Diagnostic& d) const noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::tc||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x216a90);
    const auto& o=*storage_;const auto& v=o.vehicles[slot];const auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    const auto& registration=o.registrations[slot];const auto& member=reset.membership7F0;
    if(!o.resetBound(slot)||v.mainRoot!=SceneNodeIndex49P::none||v.pendingInitial||v.attached||
       v.resetPending||v.resetNeedsGraphics||v.resetFrame||v.graphicsFrame||v.graphicsSequence||
       v.graphicsMembershipRevision||v.sourceResetBound||v.sourceTerminalRetired||
       v.raColdRetired||v.mbColdRetired||v.tcColdRetired||reset.object254Field50||
       member.attached||member.object||member.revision||member.storage.links||
       member.storage.entries||member.storage.capacity||registration.owner||registration.slot||
       registration.encodedObject)return fail(d,Status::identity,0x216a90);
    for(std::size_t i=0;i<v.entries.size();++i){
        const auto& e=v.entries[i];const auto& link=v.links[i];
        if(e.used||e.portal||e.registration.owner||e.registration.slot||e.registration.encodedObject||
           link.next||link.zone)return fail(d,Status::identity,0x216a90);
    }
    return true;
}
bool Owner::retireTcColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::tc||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(v.raColdRetired||v.mbColdRetired||v.sourceTerminalRetired||!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
    // A failure before fresh attachment retains the genuine empty prefix.
    // Once attachment starts, source shape/root identity must still match.
    if(v.mainRoot!=SceneNodeIndex49P::none){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape)))return fail(d,Status::identity,0x216a90);
    }else if(v.attached||v.pendingInitial||reset.membership7F0.attached||reset.membership7F0.object)
        return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=slot&&o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||
       registration.slot||registration.encodedObject||*field50)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.pendingInitial=false;v.tcColdRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::tbColdVehicleUntouched(std::uint32_t slot,Diagnostic& d) const noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::tb||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x216a90);
    const auto& o=*storage_;const auto& v=o.vehicles[slot];const auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    const auto& registration=o.registrations[slot];const auto& member=reset.membership7F0;
    if(!o.resetBound(slot)||v.mainRoot!=SceneNodeIndex49P::none||v.pendingInitial||v.attached||
       v.resetPending||v.resetNeedsGraphics||v.resetFrame||v.graphicsFrame||v.graphicsSequence||
       v.graphicsMembershipRevision||v.sourceResetBound||v.sourceTerminalRetired||
       v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.mcColdRetired||v.gbColdRetired||reset.object254Field50||
       member.attached||member.object||member.revision||member.storage.links||
       member.storage.entries||member.storage.capacity||registration.owner||registration.slot||
       registration.encodedObject)return fail(d,Status::identity,0x216a90);
    for(std::size_t i=0;i<v.entries.size();++i){
        const auto& e=v.entries[i];const auto& link=v.links[i];
        if(e.used||e.portal||e.registration.owner||e.registration.slot||e.registration.encodedObject||
           link.next||link.zone)return fail(d,Status::identity,0x216a90);
    }
    return true;
}
bool Owner::retireTbColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::tb||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.sourceTerminalRetired||!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
    // A failure before fresh attachment retains the genuine empty prefix.
    // Once attachment starts, source shape/root identity must still match.
    if(v.mainRoot!=SceneNodeIndex49P::none){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape)))return fail(d,Status::identity,0x216a90);
    }else if(v.attached||v.pendingInitial||reset.membership7F0.attached||reset.membership7F0.object)
        return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=slot&&o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||
       registration.slot||registration.encodedObject||*field50)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.pendingInitial=false;v.tbColdRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::gbColdVehicleUntouched(std::uint32_t slot,Diagnostic& d) const noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::gb||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x216a90);
    const auto& o=*storage_;const auto& v=o.vehicles[slot];const auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    const auto& registration=o.registrations[slot];const auto& member=reset.membership7F0;
    if(!o.resetBound(slot)||v.mainRoot!=SceneNodeIndex49P::none||v.pendingInitial||v.attached||
       v.resetPending||v.resetNeedsGraphics||v.resetFrame||v.graphicsFrame||v.graphicsSequence||
       v.graphicsMembershipRevision||v.sourceResetBound||v.sourceTerminalRetired||
       v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.mcColdRetired||v.gbColdRetired||reset.object254Field50||
       member.attached||member.object||member.revision||member.storage.links||
       member.storage.entries||member.storage.capacity||registration.owner||registration.slot||
       registration.encodedObject)return fail(d,Status::identity,0x216a90);
    for(std::size_t i=0;i<v.entries.size();++i){
        const auto& e=v.entries[i];const auto& link=v.links[i];
        if(e.used||e.portal||e.registration.owner||e.registration.slot||e.registration.encodedObject||
           link.next||link.zone)return fail(d,Status::identity,0x216a90);
    }
    return true;
}
bool Owner::retireGbColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::gb||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.sourceTerminalRetired||!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
    // A failure before fresh attachment retains the genuine empty prefix.
    // Once attachment starts, source shape/root identity must still match.
    if(v.mainRoot!=SceneNodeIndex49P::none){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape)))return fail(d,Status::identity,0x216a90);
    }else if(v.attached||v.pendingInitial||reset.membership7F0.attached||reset.membership7F0.object)
        return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=slot&&o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||
       registration.slot||registration.encodedObject||*field50)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.pendingInitial=false;v.gbColdRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::mcColdVehicleUntouched(std::uint32_t slot,Diagnostic& d) const noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::mc||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x216a90);
    const auto& o=*storage_;const auto& v=o.vehicles[slot];const auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    const auto& registration=o.registrations[slot];const auto& member=reset.membership7F0;
    if(!o.resetBound(slot)||v.mainRoot!=SceneNodeIndex49P::none||v.pendingInitial||v.attached||
       v.resetPending||v.resetNeedsGraphics||v.resetFrame||v.graphicsFrame||v.graphicsSequence||
       v.graphicsMembershipRevision||v.sourceResetBound||v.sourceTerminalRetired||
       v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.mcColdRetired||v.gbColdRetired||reset.object254Field50||
       member.attached||member.object||member.revision||member.storage.links||
       member.storage.entries||member.storage.capacity||registration.owner||registration.slot||
       registration.encodedObject)return fail(d,Status::identity,0x216a90);
    for(std::size_t i=0;i<v.entries.size();++i){
        const auto& e=v.entries[i];const auto& link=v.links[i];
        if(e.used||e.portal||e.registration.owner||e.registration.slot||e.registration.encodedObject||
           link.next||link.zone)return fail(d,Status::identity,0x216a90);
    }
    return true;
}
bool Owner::retireMcColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||storage_->scene!=Scene::mc||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(v.raColdRetired||v.mbColdRetired||v.tcColdRetired||v.tbColdRetired||v.sourceTerminalRetired||!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
    // A failure before fresh attachment retains the genuine empty prefix.
    // Once attachment starts, source shape/root identity must still match.
    if(v.mainRoot!=SceneNodeIndex49P::none){
        if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
           std::memcmp(&v.source,&p.shape,sizeof(p.shape)))return fail(d,Status::identity,0x216a90);
    }else if(v.attached||v.pendingInitial||reset.membership7F0.attached||reset.membership7F0.object)
        return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=slot&&o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||
       registration.slot||registration.encodedObject||*field50)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.pendingInitial=false;v.mcColdRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::updateAfterGraphics(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint64_t graphicsSequence,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x228844);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x228844);
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
    d.membership=updateLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,v.resetOwner.membership7F0,bounds,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x228884);
    v.graphicsSequence=graphicsSequence;v.graphicsFrame=&s;
    v.graphicsMembershipRevision=v.resetOwner.membership7F0.revision;
    v.resetNeedsGraphics=false;return true;
}
ResetVisibilityLease::~ResetVisibilityLease(){if(owner_)owner_->endResetVisibility(*this);}
// The 228BBC -> 2255B0 lease ("player" lease without input; rename with the
// next renderer-header batch). 228BBC runs for every racer after that racer's
// own 228844 graphics call, so the post-graphics publication in
// finishAiResetVisibility holds for the original AI exactly as for Player0.
static bool baseUndertrackRacer228BBC(const VehicleFrameState49W& s) noexcept {
    return s.phase==VehicleFramePhase49W::PostPackedBytes50S&&
        (s.fields.vehicle58==0u||(s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV));
}
bool Owner::resetLeaseCurrent(const ResetVisibilityLease& lease) const noexcept {
    if(!storage_||invalidated_||lease.owner_!=this||!lease.frame_||
       !lease.bindings_||!lease.parameters_)return false;
    const auto& o=*storage_;
    if(o.activeReset!=&lease||o.staticBusy()||o.activeLease||o.activeCamera||!lease.generation_||
       lease.generation_!=o.resetGeneration||!o.resetBound(lease.racer_))return false;
    const auto& v=o.vehicles[lease.racer_];
    const auto& s=*lease.frame_;const auto& b=*lease.bindings_;const auto& p=*lease.parameters_;
    const bool domain=lease.sourceTrigger_?
        s.phase==VehicleFramePhase49W::PostWorld&&s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV&&
        s.bodies.main.phase==MainBodyPhase49K::AwaitingWorldPreparation&&
        s.bodies.phase==CoupledBodyPhase49M::AwaitingWorldPreparation:
        lease.player_?
        (lease.input_?s.phase==VehicleFramePhase49W::Ready&&s.fields.vehicle58==0u:
            baseUndertrackRacer228BBC(s)):
        s.phase==VehicleFramePhase49W::PostPlayer&&s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV;
    return domain&&!v.sourceTerminalRetired&&
        (lease.sourceTrigger_?(v.sourceResetBound&&sameSourceTriggerVisibilityIdentity50MB(v.sourceResetIdentity,lease.sourceIdentity_)):!v.sourceResetBound)&&
        frameBinding(s,b)&&v.mainRoot==b.lifecycle.main190&&v.roots==modelRoots(b)&&
        !std::memcmp(&v.source,&p.shape,sizeof(p.shape))&&!s.shape.type88&&
        v.graphicsSequence==lease.graphicsSequence_&&v.resetOwner.object254Field50&&
        (!v.resetPending||v.resetFrame==&s);
}
bool Owner::beginSourceTriggerResetVisibility50MB(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,std::uint32_t* field50,
    const SourceTriggerVisibilityIdentity50MB& identity,ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    if(!identity.issuer||!identity.generation||!identity.querySerial||!identity.racer60)
        return fail(d,Status::identity,0x216a90);
    return beginResetVisibility(slot,s,b,p,field50,lease,d,false,false,&identity);
}
bool Owner::beginAiResetVisibility(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    return beginResetVisibility(slot,s,b,p,field50,lease,d,false);
}
bool Owner::beginPlayerResetVisibility(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    return beginResetVisibility(slot,s,b,p,field50,lease,d,true);
}
bool Owner::beginPlayerInputResetVisibility(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    return beginResetVisibility(slot,s,b,p,field50,lease,d,true,true);
}
bool Owner::beginResetVisibility(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,ResetVisibilityLease& lease,Diagnostic& d,bool player,bool input,const SourceTriggerVisibilityIdentity50MB* source) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x216a90);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||lease.owner_)
        return fail(d,Status::activeDraw,0x216a90);
    // Missing254 is not the original null7F0 branch. Never authorize any
    // registry mutation or manufacture a replacement field for this case.
    if(!field50)return fail(d,Status::missingResetOwner,0x216ac8);
    if(slot>=o.racerCount||!frameBinding(s,b)||!o.resetBound(slot))
        return fail(d,Status::identity,0x216a90);
    const bool domain=source?
        s.phase==VehicleFramePhase49W::PostWorld&&s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV&&
        s.bodies.main.phase==MainBodyPhase49K::AwaitingWorldPreparation&&
        s.bodies.phase==CoupledBodyPhase49M::AwaitingWorldPreparation:
        player?
        (input?s.phase==VehicleFramePhase49W::Ready&&s.fields.vehicle58==0u:
            baseUndertrackRacer228BBC(s)):
        s.phase==VehicleFramePhase49W::PostPlayer&&s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV;
    if(!domain||s.fields.vehicle3C)return fail(d,Status::phase,0x216a90);
    auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(v.sourceTerminalRetired||
       (v.sourceResetBound&&(!source||!sameSourceTriggerVisibilityIdentity50MB(v.sourceResetIdentity,*source)))||
       (v.resetPending&&!v.sourceResetBound&&source)||
       v.pendingInitial||v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||
       s.shape.type88||std::memcmp(&v.source,&p.shape,sizeof(p.shape))||
       (reset.object254Field50&&reset.object254Field50!=field50)||
       (v.resetPending&&v.resetFrame!=&s))return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof(o))||!disjointResetField(field50,&s,sizeof(s))||
       !o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&b,sizeof(b))||!disjointResetField(field50,&p,sizeof(p))||
       !disjointResetField(field50,&lease,sizeof(lease)))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)if(other!=slot){
        if(o.vehicles[other].resetPending||o.vehicles[other].sourceResetBound)return fail(d,Status::activeDraw,0x216a90);
        if(o.vehicles[other].resetOwner.object254Field50==field50)
            return fail(d,Status::identity,0x216ac8);
    }
    if(v.attached){
        if(!o.bound(slot))return fail(d,Status::identity,0x216a90);
    }else{
        const auto& registration=o.registrations[slot];
        if(!v.resetPending||reset.membership7F0.attached||reset.membership7F0.object||
           registration.owner||registration.slot||registration.encodedObject)
            return fail(d,Status::identity,0x216a90);
    }
    if(o.resetGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::stalePublication,0x216a90);
    reset.object254Field50=field50;
    if(source){v.sourceResetIdentity=*source;v.sourceResetBound=true;lease.sourceIdentity_=*source;lease.sourceTrigger_=true;}
    lease.owner_=this;lease.frame_=&s;lease.bindings_=&b;lease.parameters_=&p;
    lease.generation_=++o.resetGeneration;lease.graphicsSequence_=v.graphicsSequence;
    lease.racer_=slot;lease.player_=player;lease.input_=input;lease.removed_=v.resetPending;lease.attached_=v.resetPending&&v.attached;
    o.activeReset=&lease;return true;
}
bool Owner::removeAiResetVisibility(ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!resetLeaseCurrent(lease))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[lease.racer_];
    if(lease.removed_||lease.attached_||v.resetPending||!o.bound(lease.racer_))
        return fail(d,Status::phase,0x216a90);
    d.resetVisibility=removeResetVisibility50EY(o.visibility,v.resetOwner,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    v.attached=false;v.resetPending=true;v.resetNeedsGraphics=true;v.resetFrame=lease.frame_;
    o.selectedReady.fill(false);lease.removed_=true;return true;
}
bool Owner::attachAiResetVisibility(ResetVisibilityLease& lease,
    CollectionRef50BX visibilityZoneC30,Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!resetLeaseCurrent(lease))return fail(d,Status::identity,0x216874);
    auto& o=*storage_;auto& v=o.vehicles[lease.racer_];
    if(!lease.removed_||lease.attached_||!v.resetPending||v.attached)
        return fail(d,Status::phase,0x216874);
    if(!visibilityZoneC30||visibilityZoneC30>o.visibility.lifecycle.zoneCount)
        return fail(d,Status::identity,0x21687c);
    if(!preparedResetShape(*lease.frame_,*lease.bindings_,*lease.parameters_))
        return fail(d,Status::bounds,0x21684c);
    d.resetVisibility=attachResetVisibility50EY(o.visibility,v.resetOwner,*lease.frame_,
        *lease.bindings_,lease.parameters_->shape,visibilityZoneC30,o.workspace);
    d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    v.attached=true;lease.attached_=true;return true;
}
bool Owner::finishAiResetVisibility(ResetVisibilityLease& lease,Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!resetLeaseCurrent(lease))return fail(d,Status::identity,0x2169a0);
    auto& o=*storage_;auto& v=o.vehicles[lease.racer_];
    if(!lease.removed_||!lease.attached_||!v.resetPending||!o.bound(lease.racer_)||
       !lease.frame_->shape.registrationE0)return fail(d,Status::phase,0x2169a0);
    if(lease.player_&&!lease.input_){
        // Player2255B0 occurs AFTER this frame's genuine graphics call. Its
        //216F6C..21723C reset has now rebuilt every displayed model scene and
        // bound. Replaying graphics/228844 would be an extra source operation.
        // Retain the existing graphics serial, but publish the current reset
        // registration and freshly ready model worlds to the native draw gate.
        if(!v.graphicsSequence||v.graphicsSequence!=lease.graphicsSequence_)
            return fail(d,Status::stalePublication,0x21723c);
        const auto& frame=*lease.frame_;const auto count=modelCount(*lease.bindings_);
        for(std::uint32_t c=0;c<count;++c){
            const auto& node=frame.nodes[static_cast<std::uint32_t>(v.roots[c])];
            if(node.parent180!=SceneNodeIndex49P::none||!(node.sceneFlags190&1u)||
               std::memcmp(&node.localC0,&node.world100,sizeof(BodyMatrix48G)))
                return fail(d,Status::bounds,0x21723c);
        }
        v.graphicsFrame=&frame;v.graphicsMembershipRevision=v.resetOwner.membership7F0.revision;
        v.resetNeedsGraphics=false;
    }
    v.resetPending=false;v.resetFrame=nullptr;v.sourceResetBound=false;v.sourceResetIdentity={};return true;
}
bool Owner::retireSourceTriggerVisibility50MB(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,std::uint32_t* field50,
    const SourceTriggerVisibilityIdentity50MB& key,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x216a90);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset)
        return fail(d,Status::activeDraw,0x216a90);
    if(!key.issuer||!key.generation||!key.querySerial||!key.racer60||!field50||slot>=o.racerCount||
       !frameBinding(s,b)||!o.resetBound(slot)||s.phase!=VehicleFramePhase49W::PostWorld)
        return fail(d,Status::identity,0x216a90);
    auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(v.pendingInitial||v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||s.shape.type88||
       std::memcmp(&v.source,&p.shape,sizeof(p.shape))||
       (reset.object254Field50&&reset.object254Field50!=field50)||
       (v.sourceResetBound&&!sameSourceTriggerVisibilityIdentity50MB(v.sourceResetIdentity,key))||
       (v.resetPending&&(!v.sourceResetBound||v.resetFrame!=&s)))return fail(d,Status::identity,0x216a90);
    if(!disjointResetField(field50,&o,sizeof o)||!o.disjointStaticBacking(field50)||
       !disjointResetField(field50,&s,sizeof s)||!disjointResetField(field50,&b,sizeof b)||
       !disjointResetField(field50,&p,sizeof p))return fail(d,Status::identity,0x216ac8);
    for(std::uint32_t other=0;other<o.racerCount;++other)if(other!=slot&&
        (o.vehicles[other].resetPending||o.vehicles[other].sourceResetBound||o.vehicles[other].resetOwner.object254Field50==field50))
        return fail(d,Status::identity,0x216ac8);
    if(v.attached&&!o.bound(slot))return fail(d,Status::identity,0x216a90);
    if(v.sourceTerminalRetired&&!sameSourceTriggerVisibilityIdentity50MB(v.sourceResetIdentity,key))
        return fail(d,Status::identity,0x216a90);
    reset.object254Field50=field50;
    d.resetVisibility=removeResetVisibility50EY(o.visibility,reset,o.workspace);d.membership=d.resetVisibility.registry;
    if(d.resetVisibility.status!=ResetVisibilityStatus50EY::SourceSliceComplete)
        return fail(d,Status::registration,d.resetVisibility.sourcePC);
    const auto& registration=o.registrations[slot];
    if(reset.membership7F0.attached||reset.membership7F0.object||registration.owner||registration.slot||registration.encodedObject)
        return fail(d,Status::registration,0x216a90);
    v.attached=false;v.resetPending=false;v.resetFrame=nullptr;v.resetNeedsGraphics=true;
    v.sourceResetIdentity=key;v.sourceResetBound=false;v.sourceTerminalRetired=true;
    o.selectedReady.fill(false);return true;
}
void Owner::endResetVisibility(ResetVisibilityLease& lease) noexcept {
    if(storage_&&storage_->activeReset==&lease)storage_->activeReset=nullptr;
    lease.owner_=nullptr;lease.frame_=nullptr;lease.bindings_=nullptr;lease.parameters_=nullptr;
    lease.generation_=0;lease.graphicsSequence_=0;lease.racer_=0;
    lease.removed_=false;lease.attached_=false;lease.player_=false;lease.input_=false;lease.sourceTrigger_=false;lease.sourceIdentity_={};
}
std::uint64_t ResetVisibilityLease::zoneKey4C0(std::size_t ordinal) const noexcept {
    if(!owner_||!owner_->resetLeaseCurrent(*this)||
       ordinal>=owner_->storage_->visibility.lifecycle.zoneCount)return 0;
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&owner_->storage_->zones[ordinal]));
}
std::size_t ResetVisibilityLease::zoneCount() const noexcept {
    return owner_&&owner_->resetLeaseCurrent(*this)?owner_->storage_->visibility.lifecycle.zoneCount:0u;
}
CollectionRef50BX ResetVisibilityLease::zoneRef4C0(std::uint64_t actualKey) const noexcept {
    if(!actualKey||!owner_||!owner_->resetLeaseCurrent(*this))return 0;
    for(std::size_t ordinal=0;ordinal<owner_->storage_->visibility.lifecycle.zoneCount;++ordinal)
        if(zoneKey4C0(ordinal)==actualKey)return static_cast<CollectionRef50BX>(ordinal+1u);
    return 0;
}
CameraVisibilityLease::~CameraVisibilityLease(){if(owner_)owner_->endCameraVisibility(*this);}
bool Owner::cameraLeaseCurrent(const CameraVisibilityLease& lease) const noexcept {
    if(!storage_||invalidated_||lease.owner_!=this)return false;
    const auto& o=*storage_;
    return o.activeCamera==&lease&&!o.staticBusy()&&!o.activeLease&&!o.activeReset&&!o.resetPending()&&
        lease.generation_&&lease.generation_==o.cameraGeneration&&
        lease.racer_<o.racerCount&&o.bound(lease.racer_);
}
bool Owner::beginCameraVisibility(std::uint32_t slot,CameraVisibilityLease& lease,
    Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x175224);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||lease.owner_)return fail(d,Status::activeDraw,0x175224);
    if(slot>=o.racerCount||!o.bound(slot)||o.visibility.lifecycle.objectCount!=o.identityCount())
        return fail(d,Status::identity,0x175224);
    if(o.cameraGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::stalePublication,0x175224);
    lease.owner_=this;lease.generation_=++o.cameraGeneration;lease.racer_=slot;
    o.activeCamera=&lease;return true;
}
void Owner::endCameraVisibility(CameraVisibilityLease& lease) noexcept {
    // No partial matrix/stamp publication survives the actual camera borrow.
    // Retain source field values, invalidate only native consumption authority.
    if(storage_&&storage_->zoneProjectionLease==&lease){
        storage_->zoneProjectionLease=nullptr;
        for(auto& method:storage_->selectedMethods)method.complete=false;
    }
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
    return owner_&&owner_->cameraLeaseCurrent(*this)?&owner_->storage_->vehicles[racer_].resetOwner.membership7F0:nullptr;
}
const LiveZoneMembership50EN* Owner::cableCallbackVisibility7F0(
    const CameraVisibilityLease& view,std::uint32_t callbackRacer,
    const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const std::uint32_t& actualCableField50)const noexcept {
    // cameraLeaseCurrent checks the exclusive active pointer, generation,
    // owner, viewing racer, reset/static/draw exclusions. Do not change view.
    if(!cameraLeaseCurrent(view)||!frameBinding(frame,bindings))return nullptr;
    const auto& o=*storage_;
    if(callbackRacer>=o.racerCount||!o.bound(callbackRacer)||
       o.visibility.lifecycle.objectCount!=o.identityCount())return nullptr;
    const auto& v=o.vehicles[callbackRacer];
    if(v.mainRoot!=bindings.lifecycle.main190||v.roots!=modelRoots(bindings)||
       v.resetNeedsGraphics||
       (v.resetOwner.object254Field50&&v.resetOwner.object254Field50!=&actualCableField50)||
       !disjointResetField(&actualCableField50,this,sizeof(*this))||
       !disjointResetField(&actualCableField50,&o,sizeof(o))||
       !disjointResetField(&actualCableField50,&frame,sizeof(frame))||
       !o.disjointStaticBacking(&actualCableField50))return nullptr;
    for(std::uint32_t other=0;other<o.racerCount;++other)
        if(other!=callbackRacer&&o.vehicles[other].resetOwner.object254Field50==&actualCableField50)
            return nullptr;
    return &v.resetOwner.membership7F0;
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
bool CameraVisibilityLease::isScSourceTopology() const noexcept {
    if(!owner_||!owner_->cameraLeaseCurrent(*this))return false;
    const auto& o=*owner_->storage_;
    return o.scene==Owner::Scene::sc&&o.scTopology&&!o.legacyTopology&&!o.tcTopology&&
        o.visibility.lifecycle.zoneCount==o.world.zones.size()&&o.visibility.portals.zoneCount==o.world.zones.size()&&
        o.visibility.portals.portalCount==o.world.portals.size();
}
bool CameraVisibilityLease::isTcSourceTopology() const noexcept {
    if(!owner_||!owner_->cameraLeaseCurrent(*this))return false;
    const auto& o=*owner_->storage_;
    return o.scene==Owner::Scene::tc&&o.tcTopology&&!o.legacyTopology&&!o.scTopology&&
        o.visibility.lifecycle.zoneCount==25u&&o.visibility.portals.zoneCount==25u&&
        o.visibility.portals.portalCount==26u;
}
const AuthoredRenderZone* CameraVisibilityLease::authoredRenderZone(std::size_t ordinal) const noexcept {
    if(!owner_||!owner_->cameraLeaseCurrent(*this))return nullptr;
    const auto& o=*owner_->storage_;
    if(!o.renderAuthoringReady||ordinal>=o.visibility.lifecycle.zoneCount)return nullptr;
    return &o.renderAuthoring[ordinal];
}
bool Owner::beginDraw(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,std::uint64_t expectedGraphicsSequence,
    DrawLease& lease,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x229e08);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||lease.owner_)return fail(d,Status::activeDraw,0x229e08);
    if(slot>=o.racerCount||!frameBinding(s,b)||!o.bound(slot))
        return fail(d,Status::identity,0x229e08);
    if(s.phase!=VehicleFramePhase49W::Ready&&s.phase!=VehicleFramePhase49W::PostGraphics)
        return fail(d,Status::phase,0x229e08);
    const auto& v=o.vehicles[slot];
    if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||v.resetNeedsGraphics||!v.graphicsSequence||
       v.graphicsSequence!=expectedGraphicsSequence||
       o.drawGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Status::identity,0x229e08);
    lease.owner_=this;lease.frame_=&s;lease.generation_=++o.drawGeneration;
    lease.membershipRevision_=v.resetOwner.membership7F0.revision;lease.racer_=slot;lease.roots_=v.roots;
    lease.componentCount_=modelCount(b);
    o.selectedReady.fill(false);o.activeLease=&lease;return true;
}
bool Owner::writeCablesVisibility50(const DrawLease& lease,std::uint32_t& field50,
    Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x225588);
    auto& o=*storage_;
    if(lease.owner_!=this||o.activeLease!=&lease||o.activeCamera||o.activeReset||o.resetPending()||
       !lease.frame_||!lease.generation_||lease.generation_!=o.drawGeneration||
       !o.bound(lease.racer_))return fail(d,Status::stalePublication,0x225588);
    const auto& v=o.vehicles[lease.racer_];
    if(v.resetOwner.membership7F0.revision!=lease.membershipRevision_||
       !disjointResetField(&field50,&lease,sizeof(lease)))return fail(d,Status::identity,0x174360);
    return o.writeCablesField50(lease.racer_,*lease.frame_,field50,d);
}
bool Owner::writeCablesVisibilityAfterGraphics(std::uint32_t slot,
    const VehicleFrameState49W& s,const VehicleFrameBindings49W& b,
    const VehicleFrameParameters49W& p,std::uint64_t graphicsSequence,
    std::uint32_t& field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x225588);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x225588);
    if(slot>=o.racerCount||!frameBinding(s,b)||!o.bound(slot))
        return fail(d,Status::identity,0x225588);
    const auto& v=o.vehicles[slot];
    if(s.phase!=VehicleFramePhase49W::PostGraphics||!graphicsSequence||
       v.graphicsSequence!=graphicsSequence||v.graphicsFrame!=&s||
       v.graphicsMembershipRevision!=v.resetOwner.membership7F0.revision)
        return fail(d,Status::phase,0x225588);
    if(v.mainRoot!=b.lifecycle.main190||v.roots!=modelRoots(b)||s.shape.type88||
       std::memcmp(&v.source,&p.shape,sizeof(p.shape))||
       !disjointResetField(&field50,&b,sizeof(b))||!disjointResetField(&field50,&p,sizeof(p)))
        return fail(d,Status::identity,0x225588);
    return o.writeCablesField50(slot,s,field50,d);
}
bool Owner::select(const DrawLease& lease,std::uint32_t component,SceneNodeIndex49P root,
    CachePolicy policy,light::LightMatrices& output,Diagnostic& d) noexcept {
    d={};d.racer=lease.racer_;
    if(!storage_||invalidated_)return fail(d,Status::unprepared,0x2413a8);
    auto& o=*storage_;
    if(lease.owner_!=this||o.activeLease!=&lease||o.activeReset||o.resetPending()||lease.generation_!=o.drawGeneration||
       lease.racer_>=o.racerCount||!o.bound(lease.racer_)||component>=lease.componentCount_||
       lease.roots_[component]!=root||
       o.vehicles[lease.racer_].resetOwner.membership7F0.revision!=lease.membershipRevision_)
        return fail(d,Status::stalePublication,0x2413d0);
    if(policy!=(o.scene==Scene::mc?CachePolicy::nativeMcAuthoredZoneLightSelection:
        o.scene==Scene::gb?CachePolicy::nativeGbAuthoredZoneLightSelection:
        o.scene==Scene::tb?CachePolicy::nativeTbAuthoredZoneLightSelection:
        o.scene==Scene::rc?CachePolicy::nativeRcAuthoredZoneLightSelection:
        o.scene==Scene::tc?CachePolicy::nativeTcAuthoredZoneLightSelection:
        o.scene==Scene::sc?CachePolicy::nativeScAuthoredZoneLightSelection:
        o.scene==Scene::mb?CachePolicy::nativeMbAuthoredZoneLightSelection:
        o.scene==Scene::ra?CachePolicy::nativeRaAuthoredZoneLightSelection:
        o.scene==Scene::ma?CachePolicy::nativeMaAuthoredZoneLightSelection:
        o.scene==Scene::sa?CachePolicy::nativeSaAuthoredZoneLightSelection:
        o.scene==Scene::ga?CachePolicy::nativeGaAuthoredZoneSpotSelection:
        o.scene==Scene::ta?CachePolicy::nativeTaAuthoredZoneSpotSelection:
        CachePolicy::inactiveCandidateNativePerPublicationRecompute))
        return fail(d,Status::unsupportedPolicy,0x14a0e8);
    // INTENTIONALLY INACTIVE NATIVE POLICY: do not reproduce retail A4 hits.
    // It can differ from14A860's membership-key retained matrix. No source
    // null/miss/completeness assertion is manufactured by choosing this branch.
    if(!o.selectedReady[component]){
        if(o.scene!=Scene::gc){
            if(!lease.frame_||!validRoot(root,lease.frame_->sceneNodeCount))
                return fail(d,Status::identity,0x2413d0);
            const auto& node=lease.frame_->nodes[static_cast<std::uint32_t>(root)];
            if(!(node.sceneFlags190&1u))return fail(d,Status::phase,0x2412e4);
            const light::Vec3 position{{node.world100.v[12],node.world100.v[13],node.world100.v[14]}};
            light::LightMatrices next{};
            if(!o.selectAuthored(o.vehicles[lease.racer_].resetOwner.membership7F0,position,next,d))return false;
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
    for(unsigned c=0;c<lease.componentCount_;++c)
        if(!select(lease,c,lease.roots_[c],policy,next.matrices_[c],d))return false;
    next.componentCount_=lease.componentCount_;
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
    lease.racer_=0;lease.roots_={};lease.componentCount_=defaultVehicleDrawComponents50CM;
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
    if(!storage_||invalidated_||storage_->scene!=Scene::gc||!storage_->fixedGcGlobalAmbient)return false;
    out=storage_->globalAmbient50;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::currentSourceGlobalAmbient50(std::array<float,4>& out) const noexcept {
    if(!storage_||invalidated_||!storage_->fixedGcGlobalAmbient)return false;
    out=storage_->globalAmbient50;return true;
}
}

#include "original_ra_static_visibility50la.inl"
#include "original_ra_static_selection50la.inl"
#include "original_ra_zone_projection50la.inl"

#include "original_ra_zone_draw50la.inl"

namespace rr::game::original_gc_gameplay_lighting_candidate {
const CollectionZone50BX* Owner::retainSceneZones272A78(const ::ra_pod_scene50LA::Owner* owner,
    std::size_t& count)noexcept {
    count=0;
    if(!owner||!storage_||storage_->scene!=Scene::ra||invalidated_||storage_->sceneOwner||
       storage_->activeLease||storage_->activeCamera||storage_->activeReset||storage_->resetPending())return nullptr;
    auto& s=*storage_;
    if(s.visibility.lifecycle.storage.views.zones!=s.zones.data())return nullptr;
    s.sceneOwner=owner;count=s.world.zones.size();return s.zones.data();
}
ResetVisibilityWorld50EY* Owner::nativeWreckVisibility165E90(const ::ra_pod_scene50LA::Owner* owner,
    const CollectionZone50BX* rows)noexcept {
    if(!owner||!storage_||invalidated_||storage_->scene!=Scene::ra||storage_->sceneOwner!=owner||
       rows!=storage_->zones.data()||storage_->visibility.lifecycle.storage.views.zones!=rows||
       storage_->activeLease||storage_->activeCamera||storage_->activeReset||storage_->resetPending())return nullptr;
    return &storage_->visibility;
}
bool Owner::canReleaseSceneZones272A78(const ::ra_pod_scene50LA::Owner* owner)const noexcept {
    return storage_&&owner&&storage_->sceneOwner==owner&&!storage_->activeLease&&
       !storage_->activeCamera&&!storage_->activeReset&&!storage_->resetPending();
}
bool Owner::releaseSceneZones272A78(const ::ra_pod_scene50LA::Owner* owner)noexcept {
    if(!canReleaseSceneZones272A78(owner)||storage_->raFlareCount)return false;
    storage_->sceneOwner=nullptr;return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
const original_world_environment50KG::SpatialBounds* Owner::sceneBounds534(const ::ra_pod_scene50LA::Owner* owner)const noexcept {
    return storage_&&owner&&storage_->sceneOwner==owner&&storage_->environment3A4&&
        podEnvironment3A4(*owner)&&*podEnvironment3A4(*owner)==storage_->environment3A4.get()?
        (*podEnvironment3A4(*owner))->spatialBounds534():nullptr;
}
}

#include "original_ra_native_static_draw50la.inl"

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::bindTcGateOwner(const ::tc_gate_visibility50TC::Binding* owner,const LiveZoneMembership50EN* membership)noexcept {
 if(!owner||!membership||!storage_||invalidated_||storage_->scene!=Scene::tc)return false;
 auto& o=*storage_;
 if(o.tcGateOwner||o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    membership->attached||membership->object||o.visibility.lifecycle.objectCount!=o.identityCount())return false;
 const auto& registration=o.registrations[o.racerCount+1u];
 if(registration.owner||registration.slot||registration.encodedObject)return false;
 o.tcGateOwner=owner;o.tcGateMembership=membership;o.tcGateComplete=false;return true;
}
bool Owner::beginTcGateAccess(const ::tc_gate_visibility50TC::Binding* owner,ResetVisibilityWorld50EY*& world,
 LiveZoneWorkspace50EN*& workspace,CollectionRef50BX& object)noexcept {
 world=nullptr;workspace=nullptr;object=0;
 if(!owner||!storage_||invalidated_||storage_->scene!=Scene::tc)return false;
 auto& o=*storage_;
 if(o.tcGateOwner!=owner||o.tcGateAccess||o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return false;
 o.tcGateAccess=true;o.tcGateComplete=false;world=&o.visibility;workspace=&o.workspace;object=o.racerCount+2u;return true;
}
void Owner::endTcGateAccess(const ::tc_gate_visibility50TC::Binding* owner)noexcept {
 if(storage_&&owner&&storage_->tcGateOwner==owner)storage_->tcGateAccess=false;
}
bool Owner::commitTcGateOwner(const ::tc_gate_visibility50TC::Binding* owner)noexcept {
 if(!storage_||!owner||storage_->tcGateOwner!=owner||!storage_->tcGateAccess||!storage_->tcGateMembership)return false;
 auto& o=*storage_;const auto& m=*o.tcGateMembership;const auto& r=o.registrations[o.racerCount+1u];
 if(!m.attached||m.object!=o.racerCount+2u||m.method!=ZoneMethod50CL::Second||!r.owner)return false;
 o.tcGateComplete=true;return true;
}
bool Owner::releaseTcGateOwner(const ::tc_gate_visibility50TC::Binding* owner)noexcept {
 if(!storage_||!owner||storage_->tcGateOwner!=owner)return false;auto& o=*storage_;
 if(o.tcGateAccess||o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    !o.tcGateMembership||o.tcGateMembership->attached||o.tcGateMembership->object)return false;
 const auto& r=o.registrations[o.racerCount+1u];if(r.owner||r.slot||r.encodedObject)return false;
 o.tcGateOwner=nullptr;o.tcGateMembership=nullptr;o.tcGateComplete=false;return true;
}
bool Owner::currentTcGateOwner(const ::tc_gate_visibility50TC::Binding* owner)const noexcept {
 return storage_&&!invalidated_&&owner&&storage_->scene==Scene::tc&&storage_->tcGateOwner==owner&&
  storage_->tcGateComplete&&!storage_->tcGateAccess&&storage_->tcGateMembership&&storage_->tcGateMembership->attached;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
std::size_t Owner::tbDoorStorageReservationBytes()noexcept {
 // Common fields plus a padding bound, and full actual10-record vector payload
 // when eight racers need more than the unchanged nine inline legacy records.
 return sizeof(Storage::TbDoorState)+2u*alignof(Storage)+
        (maximumRacers+2u)*sizeof(ZoneRegistration50CJ);
}
bool Owner::bindTbDoorOwner(const ::tb_door_binding50TB::Binding* owner,const LiveZoneMembership50EN* membership)noexcept {
 if(!owner||!membership||!storage_||invalidated_||storage_->scene!=Scene::tb)return false;
 auto& o=*storage_;
 if(o.world.zones.size()!=10||o.world.portals.size()!=11||!o.legacyTopology||o.scTopology||o.tcTopology||
    o.tbDoor.owner||o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    membership->attached||membership->object||o.visibility.lifecycle.objectCount!=o.identityCount())return false;
 const auto& registration=o.registrations[o.racerCount+1u];
 if(registration.owner||registration.slot||registration.encodedObject)return false;
 o.tbDoor.owner=owner;o.tbDoor.membership=membership;o.tbDoor.complete=false;return true;
}
bool Owner::beginTbDoorAccess(const ::tb_door_binding50TB::Binding* owner,ResetVisibilityWorld50EY*& world,
 LiveZoneWorkspace50EN*& workspace,CollectionRef50BX& object)noexcept {
 world=nullptr;workspace=nullptr;object=0;
 if(!owner||!storage_||invalidated_||storage_->scene!=Scene::tb)return false;
 auto& o=*storage_;
 if(o.tbDoor.owner!=owner||o.tbDoor.access||o.staticAccess||o.tcGateAccess||
    o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return false;
 o.tbDoor.access=true;o.tbDoor.complete=false;world=&o.visibility;workspace=&o.workspace;object=o.racerCount+2u;return true;
}
void Owner::endTbDoorAccess(const ::tb_door_binding50TB::Binding* owner)noexcept {
 if(storage_&&owner&&storage_->tbDoor.owner==owner)storage_->tbDoor.access=false;
}
bool Owner::commitTbDoorOwner(const ::tb_door_binding50TB::Binding* owner)noexcept {
 if(!storage_||!owner||storage_->tbDoor.owner!=owner||!storage_->tbDoor.access||!storage_->tbDoor.membership)return false;
 auto& o=*storage_;const auto& m=*o.tbDoor.membership;const auto& r=o.registrations[o.racerCount+1u];
 if(!m.attached||m.object!=o.racerCount+2u||m.method!=ZoneMethod50CL::Second||!r.owner)return false;
 o.tbDoor.complete=true;return true;
}
bool Owner::releaseTbDoorOwner(const ::tb_door_binding50TB::Binding* owner)noexcept {
 if(!storage_||!owner||storage_->tbDoor.owner!=owner)return false;auto& o=*storage_;
 if(o.tbDoor.access||o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    !o.tbDoor.membership||o.tbDoor.membership->attached||o.tbDoor.membership->object)return false;
 const auto& r=o.registrations[o.racerCount+1u];if(r.owner||r.slot||r.encodedObject)return false;
 o.tbDoor={};return true;
}
bool Owner::currentTbDoorOwner(const ::tb_door_binding50TB::Binding* owner)const noexcept {
 return storage_&&!invalidated_&&owner&&storage_->scene==Scene::tb&&storage_->tbDoor.owner==owner&&
  storage_->tbDoor.complete&&!storage_->tbDoor.access&&storage_->tbDoor.membership&&storage_->tbDoor.membership->attached;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
std::size_t Owner::gbAnimationStorageReservationBytes()noexcept {
 // Honest common pointer/padding delta, GB-only four-row state, and the full
 // thirteen-record registration payload at the existing eight-racer maximum.
 return sizeof(std::unique_ptr<Storage::GbAnimationState>)+2u*alignof(Storage)+
        sizeof(Storage::GbAnimationState)+(maximumRacers+5u)*sizeof(ZoneRegistration50CJ);
}
bool Owner::bindGbAnimationOwner(const ::gb_animation_binding50GB::Binding* owner,std::size_t index,
 const LiveZoneMembership50EN* membership)noexcept {
 if(!owner||!membership||index>=4||!storage_||invalidated_||storage_->scene!=Scene::gb)return false;
 auto& o=*storage_;
 if((o.gbTimed&&o.gbTimed->activeGbParticleLight)||!o.gbAnimations||o.world.zones.size()!=13||o.world.portals.size()!=17||!o.legacyTopology||o.scTopology||o.tcTopology||
    o.staticAccess||(o.staticOwner&&!o.staticRegistrationComplete)||o.tcGateAccess||o.tcGateOwner||o.tbDoor.owner||
    o.gbAnimations->active()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    membership->attached||membership->object||o.visibility.lifecycle.objectCount!=o.identityCount())return false;
 auto& row=o.gbAnimations->rows[index];if(row.owner||row.membership)return false;
 // Selected native pre-staging claims authentic owner0..3 in source order.
 // Existing incomplete rows of this SAME Binding do not block the next row.
 for(std::size_t i=0;i<4;++i){const auto& prior=o.gbAnimations->rows[i];
  if((i<index&&prior.owner!=owner)||(i>index&&prior.owner))return false;}
 const auto& registration=o.registrations[o.racerCount+1u+index];
 if(registration.owner||registration.slot||registration.encodedObject)return false;
 row.owner=owner;row.membership=membership;row.complete=false;return true;
}
bool Owner::beginGbAnimationAccess(const ::gb_animation_binding50GB::Binding* owner,std::size_t index,
 ResetVisibilityWorld50EY*& world,LiveZoneWorkspace50EN*& workspace,CollectionRef50BX& object)noexcept {
 world=nullptr;workspace=nullptr;object=0;
 if(!owner||index>=4||!storage_||invalidated_||storage_->scene!=Scene::gb)return false;
 auto& o=*storage_;
 if((o.gbTimed&&o.gbTimed->activeGbParticleLight)||!o.gbAnimations||o.gbAnimations->rows[index].owner!=owner||o.gbAnimations->active()||o.staticAccess||
    o.tcGateAccess||o.tbDoor.access||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return false;
 auto& row=o.gbAnimations->rows[index];row.access=true;row.complete=false;
 world=&o.visibility;workspace=&o.workspace;object=o.racerCount+2u+static_cast<CollectionRef50BX>(index);return true;
}
void Owner::endGbAnimationAccess(const ::gb_animation_binding50GB::Binding* owner,std::size_t index)noexcept {
 if(storage_&&owner&&index<4&&storage_->gbAnimations&&storage_->gbAnimations->rows[index].owner==owner)
  storage_->gbAnimations->rows[index].access=false;
}
bool Owner::commitGbAnimationOwner(const ::gb_animation_binding50GB::Binding* owner,std::size_t index)noexcept {
 if(!storage_||invalidated_||storage_->scene!=Scene::gb||!owner||index>=4||!storage_->gbAnimations)return false;
 auto& o=*storage_;auto& row=o.gbAnimations->rows[index];
 if((o.gbTimed&&o.gbTimed->activeGbParticleLight)||row.owner!=owner||!row.access||!row.membership)return false;
 const auto& m=*row.membership;const auto& r=o.registrations[o.racerCount+1u+index];
 const auto object=o.racerCount+2u+static_cast<CollectionRef50BX>(index);
 if(!m.attached||m.object!=object||m.method!=ZoneMethod50CL::Second||!r.owner||!m.storage.entries||!m.storage.capacity)return false;
 const auto& primary=m.storage.entries[0].registration;
 if(primary.owner!=r.owner||primary.slot!=r.slot||primary.encodedObject!=r.encodedObject||r.encodedObject!=(object<<2u))return false;
 row.complete=true;return true;
}
bool Owner::releaseGbAnimationOwner(const ::gb_animation_binding50GB::Binding* owner,std::size_t index)noexcept {
 if(!storage_||!owner||index>=4||!storage_->gbAnimations)return false;auto& o=*storage_;
 auto& row=o.gbAnimations->rows[index];
 if((o.gbTimed&&o.gbTimed->activeGbParticleLight)||row.owner!=owner||o.gbAnimations->active()||o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    !row.membership||row.membership->attached||row.membership->object)return false;
 const auto& r=o.registrations[o.racerCount+1u+index];if(r.owner||r.slot||r.encodedObject)return false;
 row={};return true;
}
bool Owner::currentGbAnimationOwner(const ::gb_animation_binding50GB::Binding* owner,std::size_t index)const noexcept {
 if(!storage_||invalidated_||!owner||index>=4||storage_->scene!=Scene::gb||!storage_->gbAnimations)return false;
 const auto& o=*storage_;const auto& row=o.gbAnimations->rows[index];
 return row.owner==owner&&row.complete&&!o.gbAnimations->active()&&row.membership&&row.membership->attached&&
  row.membership->object==o.racerCount+2u+index;
}
bool Owner::bindMcAnimationOwner(const ::mc_animation_binding50MC::Binding* owner,std::size_t index,
 const LiveZoneMembership50EN* membership)noexcept {
 if(!owner||!membership||index>=14||!storage_||invalidated_||storage_->scene!=Scene::mc)return false;
 auto& o=*storage_;
 if(o.activeMcParticleLight||(o.gbTimed&&o.gbTimed->activeGbParticleLight)||!o.mcAnimations||o.world.zones.size()!=28||o.world.portals.size()!=28||!o.mcTopology||o.legacyTopology||o.scTopology||o.tcTopology||
    o.staticAccess||(o.staticOwner&&!o.staticRegistrationComplete)||o.tcGateAccess||o.tcGateOwner||o.tbDoor.owner||
    o.mcAnimations->active()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    membership->attached||membership->object||o.visibility.lifecycle.objectCount!=o.identityCount())return false;
 auto& row=o.mcAnimations->rows[index];if(row.owner||row.membership)return false;
 // Selected native pre-staging claims authentic owner0..3 in source order.
 // Existing incomplete rows of this SAME Binding do not block the next row.
 for(std::size_t i=0;i<14;++i){const auto& prior=o.mcAnimations->rows[i];
  if((i<index&&prior.owner!=owner)||(i>index&&prior.owner))return false;}
 const auto& registration=o.registrations[o.racerCount+1u+index];
 if(registration.owner||registration.slot||registration.encodedObject)return false;
 row.owner=owner;row.membership=membership;row.complete=false;return true;
}
bool Owner::beginMcAnimationAccess(const ::mc_animation_binding50MC::Binding* owner,std::size_t index,
 ResetVisibilityWorld50EY*& world,LiveZoneWorkspace50EN*& workspace,CollectionRef50BX& object)noexcept {
 world=nullptr;workspace=nullptr;object=0;
 if(!owner||index>=14||!storage_||invalidated_||storage_->scene!=Scene::mc)return false;
 auto& o=*storage_;
 if(o.activeMcParticleLight||(o.gbTimed&&o.gbTimed->activeGbParticleLight)||!o.mcAnimations||o.mcAnimations->rows[index].owner!=owner||o.mcAnimations->active()||o.staticAccess||
    o.tcGateAccess||o.tbDoor.access||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return false;
 auto& row=o.mcAnimations->rows[index];row.access=true;row.complete=false;
 world=&o.visibility;workspace=&o.workspace;object=o.racerCount+2u+static_cast<CollectionRef50BX>(index);return true;
}
void Owner::endMcAnimationAccess(const ::mc_animation_binding50MC::Binding* owner,std::size_t index)noexcept {
 if(storage_&&owner&&index<14&&storage_->mcAnimations&&storage_->mcAnimations->rows[index].owner==owner)
  storage_->mcAnimations->rows[index].access=false;
}
bool Owner::commitMcAnimationOwner(const ::mc_animation_binding50MC::Binding* owner,std::size_t index)noexcept {
 if(!storage_||invalidated_||storage_->scene!=Scene::mc||!owner||index>=14||!storage_->mcAnimations)return false;
 auto& o=*storage_;auto& row=o.mcAnimations->rows[index];
 if(o.activeMcParticleLight||(o.gbTimed&&o.gbTimed->activeGbParticleLight)||row.owner!=owner||!row.access||!row.membership)return false;
 const auto& m=*row.membership;const auto& r=o.registrations[o.racerCount+1u+index];
 const auto object=o.racerCount+2u+static_cast<CollectionRef50BX>(index);
 if(!m.attached||m.object!=object||m.method!=ZoneMethod50CL::Second||!r.owner||!m.storage.entries||!m.storage.capacity)return false;
 const auto& primary=m.storage.entries[0].registration;
 if(primary.owner!=r.owner||primary.slot!=r.slot||primary.encodedObject!=r.encodedObject||r.encodedObject!=(object<<2u))return false;
 row.complete=true;return true;
}
bool Owner::releaseMcAnimationOwner(const ::mc_animation_binding50MC::Binding* owner,std::size_t index)noexcept {
 if(!storage_||!owner||index>=14||!storage_->mcAnimations)return false;auto& o=*storage_;
 auto& row=o.mcAnimations->rows[index];
 if(o.activeMcParticleLight||(o.gbTimed&&o.gbTimed->activeGbParticleLight)||row.owner!=owner||o.mcAnimations->active()||o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    !row.membership||row.membership->attached||row.membership->object)return false;
 const auto& r=o.registrations[o.racerCount+1u+index];if(r.owner||r.slot||r.encodedObject)return false;
 row={};return true;
}
bool Owner::currentMcAnimationOwner(const ::mc_animation_binding50MC::Binding* owner,std::size_t index)const noexcept {
 if(!storage_||invalidated_||!owner||index>=14||storage_->scene!=Scene::mc||!storage_->mcAnimations)return false;
 const auto& o=*storage_;const auto& row=o.mcAnimations->rows[index];
 return row.owner==owner&&row.complete&&!o.mcAnimations->active()&&row.membership&&row.membership->attached&&
  row.membership->object==o.racerCount+2u+index;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
std::size_t Owner::gbTimedLightStorageReservationBytes()noexcept {
    return sizeof(std::unique_ptr<Storage::GbTimedLights>)+alignof(Storage)+sizeof(Storage::GbTimedLights);
}
bool Owner::serviceGbTimedLights(float delta,std::uint32_t& sharedRng,bool paused,Diagnostic& d)noexcept {
    d={};if(!storage_||invalidated_||storage_->scene!=Scene::gb||!storage_->gbTimed)
        return fail(d,Status::unprepared,0x2754e0);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x2754e0);
    if(o.gbTimed->particleThread!=std::this_thread::get_id())return fail(d,Status::phase,0x2754e0);
    if(paused)return true;
    if(o.gbTimed->gbTimedServiceRevision==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::arithmetic,0x2754e0);
    if(o.ambient60.size()<13u||o.localLightCount!=60u)return fail(d,Status::identity,0x2754e0);
    auto next=*o.gbTimed;auto raw=sharedRng;bool flash=false;
    if(!original_gb_light_flash50GB::advanceAmbient(next.ambient,delta,raw,flash))
        return fail(d,Status::arithmetic,0x2754e0);
    for(std::size_t i=0;i<2u;++i){
        const auto index=next.localIndices[i];
        if(index>=o.localLightCount||o.localLights[index].sourceOrdinal!=(i==0u?7u:34u))
            return fail(d,Status::identity,0x147650);
        if(!original_gb_light_flash50GB::advancePoint(next.points[i],delta,next.pointColors[i],next.pointColors[i]))
            return fail(d,Status::arithmetic,0x147650);
    }
    // Publish atomically after all bounded scalar work succeeds. This is a
    // native fixed-boundary schedule, not original child-list chronology.
    o.globalAmbient50=flash?next.flash:next.savedGlobal;
    for(std::size_t z=0;z<13u;++z)o.ambient60[z]=flash?next.flash:next.savedZones[z];
    for(std::size_t i=0;i<2u;++i)for(unsigned channel=0;channel<3u;++channel)
        o.localLights[next.localIndices[i]].rgb[channel]=next.pointColors[i][channel];
    *o.gbTimed=next;sharedRng=raw;++o.gbTimed->gbTimedServiceRevision;o.selectedReady.fill(false);return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
GbParticleLightLease::~GbParticleLightLease(){if(owner_)owner_->endGbParticleLightBorrow(*this);}
bool GbParticleLightLease::read(GbParticleLightInputs& out) const noexcept {
    return owner_&&owner_->readGbParticleLightBorrow(*this,out);
}
bool Owner::beginGbParticleLightBorrow(const std::shared_ptr<Owner>& self,
    GbParticleLightLease& lease,Diagnostic& d) noexcept {
    d={};
    if(self.get()!=this||lease.owner_||!storage_||invalidated_||
       storage_->scene!=Scene::gb||!storage_->gbTimed)return fail(d,Status::identity,0x149310);
    auto& o=*storage_;
    if(o.gbTimed->particleThread!=std::this_thread::get_id())return fail(d,Status::phase,0x149310);
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x149310);
    lease.owner_=self;lease.revision_=o.gbTimed->gbTimedServiceRevision;o.gbTimed->activeGbParticleLight=&lease;
    return true;
}
bool Owner::readGbParticleLightBorrow(const GbParticleLightLease& lease,GbParticleLightInputs& out) const noexcept {
    if(!storage_||invalidated_||lease.owner_.get()!=this||storage_->scene!=Scene::gb||
       storage_->gbTimed->activeGbParticleLight!=&lease||storage_->gbTimed->particleThread!=std::this_thread::get_id()||
       storage_->gbTimed->gbTimedServiceRevision!=lease.revision_)return false;
    const auto& o=*storage_;
    //1493EC registered directional RGB;149408 original positionA0;
    //1494D4 global ambient+50. No per-zone or local-point substitution.
    GbParticleLightInputs next{o.globalAmbient50,o.authoredSunSpecular,
        o.gbTimed->particleSunPositionA0,o.gbTimed->gbTimedServiceRevision};
    out=next;return true;
}
void Owner::endGbParticleLightBorrow(GbParticleLightLease& lease) noexcept {
    if(!storage_||lease.owner_.get()!=this||storage_->gbTimed->activeGbParticleLight!=&lease||
       storage_->gbTimed->particleThread!=std::this_thread::get_id())std::terminate();
    storage_->gbTimed->activeGbParticleLight=nullptr;lease.revision_=0;
    // Keep the owner alive until this member returns, even when this was its
    // last public retain. The moved retain dies only after all owner accesses.
    auto keepAlive=std::move(lease.owner_);
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::prepareMcNative(std::shared_ptr<const ::mc_cold_sources50LA::Owner> source,std::uint32_t racers,Diagnostic& d)noexcept{
 if(!source||!source->ready()){d={};return fail(d,Status::sourceIdentity,0x270b10);}
 const auto& bytes=source->world().parameters.original_bytes;
 return prepareScene(bytes,racers,Scene::mc,d,source);
}
const ::mc_cold_sources50LA::Owner* Owner::mcSourceIdentity()const noexcept{
 return storage_&&!invalidated_&&storage_->scene==Scene::mc&&storage_->mcSource&&storage_->mcSource->ready()?storage_->mcSource.get():nullptr;
}
const ::mc_lighting_authoring50MC::Record* Owner::mcLightRecord(std::size_t index)const noexcept{
 return mcSourceIdentity()&&index<29?&storage_->mcRecords[index]:nullptr;
}
bool Owner::queryMcZoneLight(std::uint32_t zone,const std::array<float,3>& position,
 original_racer_lighting50KM::LightMatrices& out,Diagnostic& d)const noexcept{
 d={};if(!mcSourceIdentity())return fail(d,Status::unprepared,0x149680);
 const auto& o=*storage_;
 if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x149680);
 // Value query only, not a live membership/renderer receipt. Same real portal links and float ordering.
 return o.selectAuthoredZones(zone,nullptr,position,out,d);
}
std::size_t Owner::mcAnimationStorageReservationBytes()noexcept{
 return sizeof(Storage::McAnimationState)+sizeof(Storage::McTopologyStorage)+
   (maximumRacers+15u)*sizeof(ZoneRegistration50CJ);
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
std::size_t Owner::mcRetainedPayloadBytes()const noexcept {
 if(!mcSourceIdentity())return 0;
 const auto& o=*storage_;std::size_t n=sizeof(Owner)+sizeof(Storage)+sizeof(Storage::McTopologyStorage)+sizeof(Storage::McAnimationState);
 auto add=[&](std::size_t x){n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;};
 auto vector=[&](const auto& v){using T=typename std::decay_t<decltype(v)>::value_type;if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(v.capacity()*sizeof(T));};
 auto text=[&](const auto& t){add(t.capacity());add(1);};
 vector(o.registrations.ra);vector(o.staticModels);vector(o.staticSelection);
 const auto& w=o.world;vector(w.parameters.original_bytes);vector(w.parameters.blocks);
 for(const auto& b:w.parameters.blocks){text(b.name);vector(b.fields);for(const auto& f:b.fields){text(f.name);text(f.value);}}
 vector(w.zones);vector(w.portals);vector(w.instances);for(const auto& p:w.portals)text(p.section);
 for(const auto& i:w.instances){text(i.section);text(i.geometry_member);text(i.collision_member);}
 for(const auto& r:o.mcRecords)if(r.directional)text(r.directional->lensFlareTexture);
 for(const auto& m:o.selectedMethods)vector(m.selected18);
 return n; // Conservative string-capacity charge includes inline storage twice.
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
McParticleLightLease::~McParticleLightLease(){if(owner_)owner_->endMcParticleLightBorrow(*this);}
bool McParticleLightLease::read(McParticleLightInputs& out)const noexcept{return owner_&&owner_->readMcParticleLightBorrow(*this,out);}
bool Owner::beginMcParticleLightBorrow(const std::shared_ptr<Owner>& self,McParticleLightLease& lease,Diagnostic& d)noexcept{
 d={};if(self.get()!=this||lease.owner_||!mcSourceIdentity())return fail(d,Status::identity,0x149310);
 auto& o=*storage_;if(o.mcParticleThread!=std::this_thread::get_id())return fail(d,Status::phase,0x149310);
 if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x149310);
 lease.owner_=self;lease.revision_=0;o.activeMcParticleLight=&lease;return true;
}
bool Owner::readMcParticleLightBorrow(const McParticleLightLease& lease,McParticleLightInputs& out)const noexcept{
 if(!mcSourceIdentity()||lease.owner_.get()!=this||storage_->activeMcParticleLight!=&lease||lease.revision_||storage_->mcParticleThread!=std::this_thread::get_id())return false;
 const auto& o=*storage_;const auto& sun=o.mcRecords[28];if(!sun.directional)return false;
 //1493EC registered RGB,149408 actual positionA0,1494D4 ambient+50.
 //MC has no timed flash or point registration: immutable source revision0.
 out={o.globalAmbient50,o.authoredSunSpecular,sun.directional->pose.positionA0,0};return true;
}
void Owner::endMcParticleLightBorrow(McParticleLightLease& lease)noexcept{
 if(!storage_||lease.owner_.get()!=this||storage_->activeMcParticleLight!=&lease||storage_->mcParticleThread!=std::this_thread::get_id())std::terminate();
 storage_->activeMcParticleLight=nullptr;lease.revision_=0;auto keepAlive=std::move(lease.owner_);
}
}

#include "original_ra_scene_roles270900.inl"
