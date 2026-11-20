#include "original_native_list_method_custody50la.h"
#include <thread>
#include "../../tests/native/common_track50cr.h"
#include "original_gc_gameplay_lighting_candidate.h"
#include "original_light_flash50cr.h"
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
#error "Track lighting owner requires strict finite IEEE arithmetic"
#endif

namespace rr::game::original_gc_gameplay_lighting_candidate {
namespace {
namespace light=original_racer_lighting50KM;
bool fail(Diagnostic& d,Status s,std::uint32_t pc) noexcept {d.status=s;d.sourcePC=pc;return false;}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
// Bounded finite cone half-angles for every course. Same13BBA0 polynomial
// already recovered privately in graph_body49w/control_shaping48v. No new libm
// owner. Authored0..180 degrees needs no reflection. Separate volatile
// operations retain the source146F30 degree conversion and149FF4 half-angle
// chronology.
float coneCosine13BBA0(float degrees) noexcept {
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
    // Authentic authored local lights (every LightN that is not Ambient or
    // Directional), deliberately not a projection of source Light+D0 or
    // LightManager+88. Timed flashers update their RGB through serviceTimedLights.
    struct AuthoredLocalLight {
        light::Vec3 position{},look{},rgb{},attenuation{};
        float innerCos=0,outerCos=0;
        std::uint32_t zone=0,sourceOrdinal=0,effectiveSourceOrdinal=0;
        bool spotlight=true,emitsLight=true;
        // innerCos>outerCos. An inverted or degenerate authored cone (RA Light129
        // 60/0, MB Light226 0/0) never enters the 149FF4..14A02C interpolation.
        bool coneInterpolates=true;
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
    // One store of N default-constructed rows, sized ONCE from a data count.
    // Rows never move after sizing (spans and lifecycle pointers bind to them).
    template<class T> struct Rows {
        std::unique_ptr<T[]> data;std::size_t count=0;
        void size(std::size_t n){data=n?std::make_unique<T[]>(n):nullptr;count=n;}
        std::span<T> span()noexcept{return {data.get(),count};}
        bool disjoint(const std::uint32_t* field)const noexcept {
            return !count||disjointResetField(field,data.get(),count*sizeof(T));
        }
    };
    // The visibility topology of THIS track: Z = SCB zones, P = SCB portals
    // (270900 SceneInfo), sized once at prepare. Replaces the fixed legacy 25/25,
    // SC 30/34, TC 25/26 and MC 28/28 backings.
    struct Topology {
        Rows<light::Vec4> sun44,ambient60;
        Rows<SearchStorage50BS> registries,scratchRegistries;       // Z*2
        Rows<SelectedMethod> selectedMethods;                       // Z*2
        Rows<CollectionTree50BX> methods;                           // Z*2
        Rows<CollectionZone50BX> zones;                             // Z
        Rows<ZoneProjection> zoneProjection;                        // Z
        Rows<AuthoredRenderZone> renderAuthoring;                   // Z
        Rows<SearchListBlock50BR> blocks;                           // Z*4
        Rows<CollectionRef50BX> portalHeads;                        // Z
        Rows<LiveZonePortalLink50EN> portalLinks,scratchPortalLinks; // P*2
        Rows<PortalGeometry50EO> portalGeometry;                    // P
        Rows<CollectionZoneLink50BX> scratchLinks;                  // P+1
        Rows<LiveZoneEntry50EN> scratchEntries;                     // P+1
        std::array<Rows<CollectionZoneLink50BX>,maximumRacers> vehicleLinks; // P+1 each
        std::array<Rows<LiveZoneEntry50EN>,maximumRacers> vehicleEntries;    // P+1 each
        void size(std::size_t z,std::size_t p){
            sun44.size(z);ambient60.size(z);registries.size(z*2u);scratchRegistries.size(z*2u);
            selectedMethods.size(z*2u);methods.size(z*2u);zones.size(z);zoneProjection.size(z);
            renderAuthoring.size(z);blocks.size(z*4u);portalHeads.size(z);
            portalLinks.size(p*2u);scratchPortalLinks.size(p*2u);portalGeometry.size(p);
            scratchLinks.size(p+1u);scratchEntries.size(p+1u);
            for(auto& r:vehicleLinks)r.size(p+1u);for(auto& r:vehicleEntries)r.size(p+1u);
        }
        bool disjoint(const std::uint32_t* f)const noexcept {
            if(!(sun44.disjoint(f)&&ambient60.disjoint(f)&&registries.disjoint(f)&&scratchRegistries.disjoint(f)&&
                 selectedMethods.disjoint(f)&&methods.disjoint(f)&&zones.disjoint(f)&&zoneProjection.disjoint(f)&&
                 renderAuthoring.disjoint(f)&&blocks.disjoint(f)&&portalHeads.disjoint(f)&&portalLinks.disjoint(f)&&
                 scratchPortalLinks.disjoint(f)&&portalGeometry.disjoint(f)&&scratchLinks.disjoint(f)&&
                 scratchEntries.disjoint(f)))return false;
            for(const auto& r:vehicleLinks)if(!r.disjoint(f))return false;
            for(const auto& r:vehicleEntries)if(!r.disjoint(f))return false;
            return true;
        }
        std::size_t bytes()const noexcept {
            std::size_t n=sun44.count*sizeof(light::Vec4)*2u+registries.count*sizeof(SearchStorage50BS)*2u+
                selectedMethods.count*sizeof(SelectedMethod)+methods.count*sizeof(CollectionTree50BX)+
                zones.count*sizeof(CollectionZone50BX)+zoneProjection.count*sizeof(ZoneProjection)+
                renderAuthoring.count*sizeof(AuthoredRenderZone)+blocks.count*sizeof(SearchListBlock50BR)+
                portalHeads.count*sizeof(CollectionRef50BX)+portalLinks.count*sizeof(LiveZonePortalLink50EN)*2u+
                portalGeometry.count*sizeof(PortalGeometry50EO)+scratchLinks.count*sizeof(CollectionZoneLink50BX)+
                scratchEntries.count*sizeof(LiveZoneEntry50EN);
            for(const auto& r:vehicleLinks)n+=r.count*sizeof(CollectionZoneLink50BX);
            for(const auto& r:vehicleEntries)n+=r.count*sizeof(LiveZoneEntry50EN);
            return n;
        }
    };
    Topology topology;
    LightTally tally{};
    // = blockRow(common_track50CR::BlockId::lights).ceiling (allocation guard).
    static constexpr std::uint32_t maximumLights=1024u;
    // Every non-ambient, non-directional LightN in numeric order (emitting or
    // not; the selection skips non-emitters). Capacity reserved ONCE from
    // NumberOfLights; every append is bounds-checked against it.
    std::vector<AuthoredLocalLight> localLights;
    std::uint32_t localLightCount=0;
    light::Vec3 sunLook{};
    light::Vec4 authoredSunSpecular{};
    light::Vec4 sunPositionA0{};       // directional Light+A0 (149408), particle lighting
    std::span<light::Vec4> sun44{};
    bool readLights2783D0(Diagnostic&);
    bool selectAuthored(const LiveZoneMembership50EN&,const light::Vec3&,
        light::LightMatrices&,Diagnostic&) const noexcept;

    bool selectAuthoredZones(std::uint32_t,const LiveZoneMembership50EN*,
        const light::Vec3&,light::LightMatrices&,Diagnostic&) const noexcept;

    assets::World48F world;
    std::span<light::Vec4> ambient60{};
    light::Vec4 globalAmbient50{};
    bool fixedGcGlobalAmbient=false; // a numeric-order ambient registration was read (148F90)
    // 2754E0 ambient flash and 147650 point flashers, as authored. Null when the
    // track authors neither.
    struct TimedLights {
        bool ambientFlash=false;
        original_light_flash50CR::AmbientState ambient{};
        light::Vec4 savedGlobal{},flash{};
        std::vector<light::Vec4> savedZones;
        struct Point {
            original_light_flash50CR::PointState state{};
            light::Vec4 color{};
            std::size_t local=0;
            std::uint32_t sourceOrdinal=0;
        };
        std::vector<Point> points;
        std::uint64_t revision=0;
    };
    std::unique_ptr<TimedLights> timed;
    const ParticleLightLease* activeParticleLight=nullptr;
    std::thread::id particleThread=std::this_thread::get_id();

    ResetVisibilityWorld50EY visibility{};
    std::span<SearchStorage50BS> registries{};
    std::span<SearchStorage50BS> scratchRegistries{};
    // Allocate once before publishing lifecycle pointers. Identities: racers
    // 1..N, the camera N+1, then one per authored SCB instance (static models),
    // the World's flare identities and the animation lane rows.
    struct RegistrationBacking {
        std::vector<ZoneRegistration50CJ> rows;
        void resize(std::size_t count){rows.resize(count);} // exactly identityCount()
        ZoneRegistration50CJ* data()noexcept{return rows.data();}
        const ZoneRegistration50CJ* data()const noexcept{return rows.data();}
        std::size_t size()const noexcept{return rows.size();}
        ZoneRegistration50CJ& operator[](std::size_t i)noexcept{return rows[i];}
        const ZoneRegistration50CJ& operator[](std::size_t i)const noexcept{return rows[i];}
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
    // ONE visual-zone lane for the track's animation rows (every course; the
    // count is the course's [Animations] rows, planTrackAnimationRows). Row k is
    // registration animationBase()+k / object animationBase()+1+k.
    struct AnimationLane {
        struct Row {
            const ::track_animation_binding50AN::Binding* owner=nullptr;
            const LiveZoneMembership50EN* membership=nullptr;
            bool access=false,complete=false;
        };
        std::vector<Row> rows;
        bool claimed()const noexcept{for(const auto& r:rows)if(r.owner)return true;return false;}
        bool active()const noexcept{for(const auto& r:rows)if(r.access)return true;return false;}
        bool busy()const noexcept{for(const auto& r:rows)if(r.access||(r.owner&&!r.complete))return true;return false;}
    };
    std::unique_ptr<AnimationLane> animations;
    bool staticBusy()const noexcept{return activeParticleLight||staticAccess||(staticOwner&&!staticRegistrationComplete)||
        (animations&&animations->busy());}
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
    std::size_t animationBase()const noexcept{return racerCount+1u+staticModels.size()+raFlareCount;}
    std::size_t identityCount()const noexcept{return animationBase()+(animations?animations->rows.size():0u);}
    bool disjointStaticBacking(const std::uint32_t* field)const noexcept {
        if(animations&&(!disjointResetField(field,animations.get(),sizeof(*animations))||
           (!animations->rows.empty()&&!disjointResetField(field,animations->rows.data(),animations->rows.size()*sizeof(AnimationLane::Row)))))return false;
        if(!topology.disjoint(field))return false;
        if(!localLights.empty()&&!disjointResetField(field,localLights.data(),localLights.capacity()*sizeof(AuthoredLocalLight)))return false;
        if(!((registrations.rows.empty()||disjointResetField(field,registrations.rows.data(),
            registrations.rows.size()*sizeof(ZoneRegistration50CJ)))&&
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
    // 276C00 EnvironmentInfo of this track's SCB, retained for the race World
    // (publishNativeEnvironment3A4). Null when the reader refused (logged).
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
        // freshRetired: retireRaFreshVehicle; coldRetired: retireColdVehicle.
        bool freshRetired=false,coldRetired=false;
    };
    std::array<Vehicle,maximumRacers> vehicles{};
    void bindTopology() noexcept {
        auto& b=topology;
        sun44=b.sun44.span();
        ambient60=b.ambient60.span();
        registries=b.registries.span();
        scratchRegistries=b.scratchRegistries.span();
        selectedMethods=b.selectedMethods.span();
        methods=b.methods.span();
        zones=b.zones.span();
        zoneProjection=b.zoneProjection.span();
        renderAuthoring=b.renderAuthoring.span();
        blocks=b.blocks.span();
        portalHeads=b.portalHeads.span();
        portalLinks=b.portalLinks.span();
        scratchPortalLinks=b.scratchPortalLinks.span();
        portalGeometry=b.portalGeometry.span();
        scratchLinks=b.scratchLinks.span();
        scratchEntries=b.scratchEntries.span();
        for(std::size_t i=0;i<vehicles.size();++i){
            vehicles[i].links=b.vehicleLinks[i].span();vehicles[i].entries=b.vehicleEntries[i].span();
        }
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
// The one numeric light reader (2783D0) for every course. Absent [Lights] =
// no lights (reader defaults), not a refusal. Every count is the SCB's.
bool Owner::Storage::readLights2783D0(Diagnostic& d) {
    const auto& params=world.parameters;
    // 157D00 selects one block by name; a repeated [Lights]/[Environment] label
    // is refused, never read as absent.
    for(const char* name:{"lights","environment"}){
        std::size_t n=0;for(const auto& b:params.blocks)if(b.name==name)++n;
        if(n>1u)return fail(d,Status::sourceShape,0x157d00);
    }
    const auto* lights=block(params,"lights");
    const auto* environment=block(params,"environment");
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
    if(lights&&!ordinal(*lights,"numberoflights",0u,numberOfLights))
        return fail(d,Status::sourceFields,0x2782e0);
    tally.numberOfLights=numberOfLights;
    // Local-light store sized once from NumberOfLights (the reader never
    // registers more rows than that). PORT STAND-IN (flagged): the ceiling is
    // the common_track50CR "lights" block ceiling, an allocation guard only.
    if(numberOfLights>maximumLights)return fail(d,Status::bounds,0x2783dc);
    localLights.clear();localLights.reserve(numberOfLights);localLightCount=0;
    // [Environment] AmbientColorFlash* (270E9C: MinIdleTime enables the flash).
    if(environment&&field(*environment,"ambientcolorflashminidletime")){
        original_light_flash50CR::AmbientTimings t{};light::Vec3 flash{};
        // PORT STAND-IN (flagged): the original's defaults for the other four
        // keys are not recovered; a flash that omits one is refused, never guessed.
        if(!scalar(*environment,"ambientcolorflashminidletime",t.minIdle)||
           !scalar(*environment,"ambientcolorflashidletime",t.idle)||
           !scalar(*environment,"ambientcolorflashminduration",t.minDuration)||
           !scalar(*environment,"ambientcolorflashduration",t.duration)||
           !triple(*environment,"ambientcolorflashcolorrgb",flash))
            return fail(d,Status::sourceFields,0x270e9c);
        for(float c:flash)if(!std::isfinite(c)||c<0.f||c>255.f)return fail(d,Status::sourceFields,0x270e9c);
        timed=std::make_unique<TimedLights>();timed->ambientFlash=true;
        if(!original_light_flash50CR::initializeAmbient(t,timed->ambient))return fail(d,Status::sourceFields,0x270e9c);
        // 146FDC..147014 halves the authored colour, W 64 as every ambient row.
        timed->flash={{mul(flash[0],.5f),mul(flash[1],.5f),mul(flash[2],.5f),64.f}};
        tally.ambientFlash=true;
        tally.flashMinIdle=t.minIdle;tally.flashIdle=t.idle;tally.flashMinDuration=t.minDuration;
        tally.flashDuration=t.duration;tally.flashColor=flash;
    }
    //2783D0..278408 reads numeric Light1..NumberOfLights. A section scan
    //would wrongly include MA's unrelated Light276 or reorder Light10/Light2.
    const assets::ParameterBlock48F* selectedBlock=nullptr;std::uint32_t selectedOrdinal=0;
    for(std::uint32_t sourceOrdinal=1;sourceOrdinal<=numberOfLights;++sourceOrdinal){
        const auto name="light"+std::to_string(sourceOrdinal);
        std::size_t matches=0;const assets::ParameterBlock48F* b=nullptr;
        for(const auto& candidate:params.blocks)if(candidate.name==name){++matches;b=&candidate;}
        if(matches>1u)return fail(d,Status::sourceFields,0x157d00);
        std::uint32_t effectiveOrdinal=sourceOrdinal;
        if(!b){
            //2773CC ignores a failed 157D00; 157DD8 preserves the prior selector,
            //so the slot re-reads the previously selected LightN (RA 90/107, TB 93..104).
            if(!selectedBlock)return fail(d,Status::sourceFields,0x157dd8);
            b=selectedBlock;effectiveOrdinal=selectedOrdinal;++tally.missingSlots;
        }else{selectedBlock=b;selectedOrdinal=sourceOrdinal;}
        light::Vec3 rgb{};std::uint32_t zone=0;
        if(!triple(*b,"colorrgb",rgb)||!ordinal(*b,"initialzone",1u,zone)||
           !zone||zone>world.zones.size())return fail(d,Status::sourceFields,0x277494);
        for(float& c:rgb)c=mul(c,.5f); //146FDC..147014 Light::Init.
        if(textField(*b,"type","Ambient")){
            ++tally.ambient;
            const light::Vec4 row{{rgb[0],rgb[1],rgb[2],64.f}};
            //148F90/FB4/FD8/FFC writes+50 for EVERY numeric registration.
            globalAmbient50=row;fixedGcGlobalAmbient=true;
            //2774C8 absent InitialZone=1,2774FC -> source zone0.149000..149040
            //then propagates to all rows; later per-zone ambients replace rows.
            if(zone==1)for(auto& current:ambient60)current=row;
            else ambient60[zone-1]=row;
            continue;
        }
        bool emits=true,infinity=true,flasher=false;
        if(!flag(*b,"emitslight",true,emits)||!flag(*b,"placelightatinfinity",true,infinity)||
           !flag(*b,"hasflasher",false,flasher))
            return fail(d,Status::sourceFields,0x2776cc);
        namespace poseSource=assets::original_light_pose_defaults_candidate;
        poseSource::FreshPose pose;poseSource::Diagnostic poseDiagnostic;
        if(!poseSource::readFreshPose(*b,static_cast<std::uint32_t>(world.zones.size()),pose,poseDiagnostic))
            return fail(d,Status::sourceFields,poseDiagnostic.sourcePC);
        if(pose.initialZone!=zone)return fail(d,Status::sourceFields,0x277500);
        if(textField(*b,"type","Directional")){
            // PORT STAND-IN (flagged): the native selection has one sun slot;
            // a second Directional, or a non-emitting/finite/zoned sun, is refused.
            if(++tally.directional!=1u||!emits||!infinity||zone!=1u||flasher)
                return fail(d,Status::sourceFields,0x1490c0);
            sunLook=pose.effectiveLook78;
            sunPositionA0=pose.positionA0;
            authoredSunSpecular={{rgb[0],rgb[1],rgb[2],0.f}};
            //1490E0 propagates the initial directional color only for zone0.
            for(auto& row:sun44)row=authoredSunSpecular;
            continue;
        }
        const bool spotlight=textField(*b,"type","Spotlight");
        if(!spotlight&&!textField(*b,"type","ObjectPoint"))
            return fail(d,Status::sourceFields,0x277b70);
        AuthoredLocalLight local{};
        local.sourceOrdinal=sourceOrdinal;local.effectiveSourceOrdinal=effectiveOrdinal;
        local.zone=zone;local.spotlight=spotlight;
        local.emitsLight=emits;local.placeAtInfinity=infinity;
        for(unsigned c=0;c<3;++c)local.position[c]=pose.positionA0[c];
        local.look=pose.effectiveLook78;local.rgb=rgb;
        //277638..277678 reader defaults:0x3CF5C28F/3B03126F/3A83126F.
        //Flare-only ObjectPoints omit Attenuation. Retain those reader values;
        //do not evaluate them as emitted light (277DDC..277E04).
        local.attenuation={{.03f,.002f,.001f}};
        if(field(*b,"attenuation")&&!triple(*b,"attenuation",local.attenuation))
            return fail(d,Status::sourceFields,0x277674);
        for(float a:local.attenuation)if(!std::isfinite(a)||a<0.f)
            return fail(d,Status::sourceFields,0x146f60);
        if(spotlight){
            ++tally.spotlights;
            //277694..2776C8 defaults30/45 degrees.
            float inner=30.f,outer=45.f;
            if((field(*b,"innerconeangle")&&!scalar(*b,"innerconeangle",inner))||
               (field(*b,"outerconeangle")&&!scalar(*b,"outerconeangle",outer))||
               !std::isfinite(inner)||!std::isfinite(outer)||
               inner<0.f||outer<0.f||inner>180.f||outer>180.f)
                return fail(d,Status::sourceFields,0x2776a8);
            // The source 146F30 degree conversion and 13BBA0 cosine polynomial
            // (149FF4 half angle), the same for every course.
            local.innerCos=coneCosine13BBA0(inner);local.outerCos=coneCosine13BBA0(outer);
            if(!std::isfinite(local.innerCos)||!std::isfinite(local.outerCos))
                return fail(d,Status::arithmetic,0x149fe4);
            //149FFC first tests dot<outerCos; otherwise14A02C tests dot<=innerCos.
            //An inverted or equal pair never enters the interpolation denominator.
            local.coneInterpolates=local.innerCos>local.outerCos;
        }else{
            ++tally.points;if(emits)++tally.emittingPoints;
            //ObjectPoint has no cone. Absent look remains the fresh source+X
            //cache, but local direction is computed from position at draw time.
        }
        if(!emits)++tally.omittedNonEmitting;
        if(localLightCount>=localLights.capacity())return fail(d,Status::bounds,0x2783dc);
        // 2779E4 flasher: an emitting light's colour follows FlashPattern
        // (147650); a non-emitting one is flare-only and never lights.
        if(flasher){
            ++tally.flashers;
            if(emits){
                const auto* pattern=field(*b,"flashpattern");float period=0,filter=0;std::uint32_t bits=0;
                // PORT STAND-IN (flagged): the reader defaults of these three
                // keys are not recovered; a flasher omitting one is refused.
                if(!pattern||!original_light_flash50CR::parsePattern(pattern->value,bits)||
                   !scalar(*b,"flashperiod",period)||!scalar(*b,"flashfilter",filter))
                    return fail(d,Status::sourceFields,0x2779e4);
                if(!timed)timed=std::make_unique<TimedLights>();
                TimedLights::Point point{};point.local=localLightCount;point.sourceOrdinal=sourceOrdinal;
                //26FC10 defaults alpha255;146FDC..147014 halves all four.
                point.color={{rgb[0],rgb[1],rgb[2],127.5f}};
                if(!original_light_flash50CR::initializePoint(bits,period,filter,point.color,point.state))
                    return fail(d,Status::arithmetic,0x146b10);
                timed->points.push_back(point);
                if(tally.timedFlashers<tally.timedFlasherOrdinals.size())tally.timedFlasherOrdinals[tally.timedFlashers]=sourceOrdinal;
                if(!tally.timedFlashers){tally.flasherPeriod=period;tally.flasherFilter=filter;
                    for(unsigned c=0;c<3;++c)tally.flasherRgb[c]=mul(rgb[c],2.f);}
                ++tally.timedFlashers;
            }
        }
        localLights.push_back(local);++localLightCount;
    }
    tally.local=localLightCount;
    if(timed){
        timed->savedGlobal=globalAmbient50;
        timed->savedZones.assign(ambient60.begin(),ambient60.end());
    }
    //278370..2783A4 ->1498A0 DIRECT copy, after all light registrations.
    //These authored per-zone overrides must never be halved a second time.
    for(std::size_t z=0;lights&&z<world.zones.size();++z){
        const auto key="directionallightcolorforzone"+std::to_string(z+1u);
        if(field(*lights,key.c_str())){
            light::Vec3 rgb{};
            if(!triple(*lights,key.c_str(),rgb))return fail(d,Status::sourceFields,0x278370);
            for(unsigned c=0;c<3;++c)sun44[z][c]=rgb[c];
            ++tally.zoneColorOverrides;
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
    //DirectionalLightColorForZoneN overrides change diffuse44, not the
    //directional light's own4C (1498A0), on every course.
    next.specular[0]=authoredSunSpecular;
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
            else if(spot.coneInterpolates&&cosine<=spot.innerCos){
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
    return storage_&&!invalidated_&&
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
    if(storage_&&storage_->activeParticleLight)std::terminate();
    if(storage_)for(std::uint32_t slot=0;slot<storage_->racerCount;++slot)
        if(storage_->vehicles[slot].sourceResetBound)std::terminate();
    if(storage_&&(storage_->raFlareCount||storage_->staticOwner||storage_->sceneOwner||
        (storage_->animations&&storage_->animations->claimed())))std::terminate();
    if(storage_&&storage_->activeLease)endDraw(*const_cast<DrawLease*>(storage_->activeLease));
    if(storage_&&storage_->activeCamera)endCameraVisibility(*const_cast<CameraVisibilityLease*>(storage_->activeCamera));
    if(storage_&&storage_->activeReset)endResetVisibility(*const_cast<ResetVisibilityLease*>(storage_->activeReset));
}
DrawLease::~DrawLease(){if(owner_)owner_->endDraw(*this);}
const assets::World48F* Owner::world() const noexcept {return storage_?&storage_->world:nullptr;}
const LightTally* Owner::tally() const noexcept {return storage_&&!invalidated_?&storage_->tally:nullptr;}
// The one prepare (common_track50CR::prepareLights, 05.13): the track's own
// decoded SCB (Documents::world, decoded once by the 05.01 scene reader). No
// SCB identity pin: the SCB's bytes/SHA are receipts logged by prepareLights.
bool Owner::prepare(const ::common_track50CR::Track& track,std::uint32_t racers,Diagnostic& d) noexcept {
    return prepareWorld(track.documents.world,racers,d);
}
bool Owner::prepareWorld(const assets::World48F& source,std::uint32_t racers,Diagnostic& d) noexcept {
    d={};if(invalidated_)return fail(d,Status::unprepared,0x272a48);
    if(storage_)return fail(d,Status::alreadyPrepared,0x272a48);
    if(!racers||racers>maximumRacers)return fail(d,Status::identity,0x1ae578);
    if(!light::selection_detail::arithmeticReady())return fail(d,Status::arithmetic,0x146f60);
    try {
        auto next=std::make_unique<Storage>();
        next->world=source;
        const auto& w=next->world;
        // Zone/portal counts from the data (270900 SceneInfo); the topology is
        // sized once from them, bounded only by the allocation ceilings.
        if(w.zones.empty()||w.zones.size()>maximumZones||w.portals.size()>maximumPortals)
            return fail(d,Status::unsupportedScene,0x272a48);
        next->topology.size(w.zones.size(),w.portals.size());
        next->bindTopology();
        next->tally.zones=static_cast<std::uint32_t>(w.zones.size());
        next->tally.portals=static_cast<std::uint32_t>(w.portals.size());
        // 276C00 EnvironmentInfo for the race World (publishNativeEnvironment3A4),
        // every course. A refusal is kept as data (tally.environment3A4=false) and
        // refused by the World step that needs it, not by the lights.
        {
            auto environment=std::make_unique<original_world_environment50KG::Owner>();
            original_world_environment50KG::Diagnostic e;
            if(environment->prepare(w,e)&&environment->prepareSpatialBounds276F3C(e)){
                next->environment3A4=std::move(environment);next->tally.environment3A4=true;
            }
        }
        if(!next->readLights2783D0(d))return false;
        next->racerCount=racers;
        for(std::uint32_t slot=0;slot<racers;++slot){
            auto& v=next->vehicles[slot];v.resetOwner.object30=slot+1u;
            v.resetOwner.storage={v.links.data(),v.entries.data(),v.links.size()};
            // No254 field is invented by cold lighting preparation.
        }
        for(std::size_t z=0;z<w.zones.size();++z){
            const auto read=assets::readZoneSearchSelection50CN(w,z);
            if(w.zones[z].zone!=z+1||read.status!=assets::ZoneSearchReadStatus50CN::Complete||
               read.selection.visibilityDynamic114!=assets::ZoneSearchKind50CN::List)
                return fail(d,Status::unsupportedScene,0x272a58);
            // 27632C reads every zone's ExclusionList on every course (absent
            // = empty list).
            const auto index=w.zones[z].parameter_block;
            if(index>=w.parameters.blocks.size()||
               !readZoneRenderAuthoring(w.parameters.blocks[index],w.zones.size(),next->renderAuthoring[z]))
                return fail(d,Status::sourceFields,0x27632c);
        }
        next->renderAuthoringReady=true;
        // One identity per authored SCB instance (274080), for the World's static
        // models on every course (bindRaStaticOwner checks the static zone search).
        next->staticModels.resize(w.instances.size(),nullptr);
        next->staticSelection.resize(w.instances.size());
        // The course's animation rows (planned from its [Animations] table; 0 = no lane).
        if(plannedAnimationRows_){next->animations=std::make_unique<Storage::AnimationLane>();
            next->animations->rows.resize(plannedAnimationRows_);}
        next->registrations.resize(next->identityCount());
        const ZoneLifecycleStorage50CL lifeStorage{next->registries.data(),next->registries.size(),
            next->registrations.data(),next->registrations.size(),
            {next->methods.data(),next->methods.size(),next->zones.data(),next->zones.size(),
             next->blocks.data(),next->blocks.size()}};
        // Reserve the actual camera scene30 registration independently of
        // the existing racer1..N identities on every world.
        if(initializeZoneLifecycle50CL(next->visibility.lifecycle,lifeStorage,w.zones.size(),next->identityCount(),1024,
            next->workspace.transfer)!=SearchStorageStatus50BS::Complete)
            return fail(d,Status::registration,0x272a58);
        std::vector<LiveZonePortalInput50EN> input(w.portals.size());
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
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x2167a0);
    if(slot>=o.racerCount||!frameBinding(s,b))return fail(d,Status::identity,0x2167a0);
    auto& v=o.vehicles[slot];
    if(v.freshRetired||v.coldRetired||v.attached||v.resetOwner.membership7F0.attached)return fail(d,Status::alreadyPrepared,0x229f30);
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
    if(!storage_||invalidated_||slot>=storage_->racerCount||
       !storage_->resetBound(slot))return false;
    const auto& v=storage_->vehicles[slot];
    if(v.freshRetired||v.sourceTerminalRetired||!v.recordZoneC38)return false;
    value=*v.recordZoneC38;return true;
}
bool Owner::readRecordedCenterC40(std::uint32_t slot,std::array<std::uint32_t,4>& value)const noexcept {
    if(!storage_||invalidated_||slot>=storage_->racerCount||
       !storage_->resetBound(slot))return false;
    const auto& v=storage_->vehicles[slot];
    if(v.freshRetired||v.sourceTerminalRetired||!v.recordCenterC40)return false;
    std::memcpy(value.data(),v.recordCenterC40->v,16);return true;
}
bool Owner::recordAttachedEndRacer229F20(std::uint32_t slot,const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    Diagnostic& d)noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_)
        return fail(d,Status::identity,0x229f20);
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x229f20);
    if(slot>=o.racerCount||!frameBinding(frame,bindings)||!o.resetBound(slot))
        return fail(d,Status::identity,0x229f38);
    auto& v=o.vehicles[slot];
    if(v.freshRetired||v.sourceTerminalRetired||!o.bound(slot))
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
    if(!storage_||invalidated_||storage_->racerCount!=count)return false;
    for(std::uint32_t slot=0;slot<count;++slot)if(!storage_->resetBound(slot))return false;
    return true;
}
bool Owner::retireRaFreshVehicle(std::uint32_t slot,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x21688c);
    auto& o=*storage_;auto& v=o.vehicles[slot];
    if(o.staticAccess||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x2167a0);
    if(!o.resetBound(slot))return fail(d,Status::identity,0x21688c);
    // Terminal native teardown; failed removal retains its exact membership.
    v.freshRetired=true;
    d.membership=removeLiveZoneMembership50EN(o.visibility.lifecycle,o.visibility.portals,
        v.resetOwner.membership7F0,o.workspace);
    if(d.membership.status!=LiveZoneStatus50EN::Complete)
        return fail(d,Status::registration,0x21688c);
    v.attached=false;v.pendingInitial=false;return true;
}
// The one terminal cold-session unwind (every course; was five identical
// per-course copies). A failure before fresh attachment retains the genuine
// empty prefix; once attachment starts, source shape/root identity must match.
bool Owner::retireColdVehicle(std::uint32_t slot,const VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    std::uint32_t* field50,Diagnostic& d) noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||slot>=storage_->racerCount||
       !field50||!frameBinding(s,b))return fail(d,Status::identity,0x216a90);
    auto& o=*storage_;auto& v=o.vehicles[slot];auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    if(v.freshRetired||v.coldRetired||v.sourceTerminalRetired||!o.resetBound(slot)||
       (reset.object254Field50&&reset.object254Field50!=field50))
        return fail(d,Status::identity,0x216a90);
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
    v.attached=false;v.pendingInitial=false;v.coldRetired=true;
    o.selectedReady.fill(false);return true;
}
bool Owner::coldVehicleUntouched(std::uint32_t slot,Diagnostic& d) const noexcept {
    d={};d.racer=slot;
    if(!storage_||invalidated_||slot>=storage_->racerCount)
        return fail(d,Status::identity,0x216a90);
    const auto& o=*storage_;const auto& v=o.vehicles[slot];const auto& reset=v.resetOwner;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x216a90);
    const auto& registration=o.registrations[slot];const auto& member=reset.membership7F0;
    if(!o.resetBound(slot)||v.mainRoot!=SceneNodeIndex49P::none||v.pendingInitial||v.attached||
       v.resetPending||v.resetNeedsGraphics||v.resetFrame||v.graphicsFrame||v.graphicsSequence||
       v.graphicsMembershipRevision||v.sourceResetBound||v.sourceTerminalRetired||
       v.freshRetired||v.coldRetired||reset.object254Field50||
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
    // One native policy for every course (the former GC label names the same
    // per-publication recompute); an unspecified policy is refused.
    if(policy==CachePolicy::unspecified)
        return fail(d,Status::unsupportedPolicy,0x14a0e8);
    // INTENTIONALLY INACTIVE NATIVE POLICY: do not reproduce retail A4 hits.
    // It can differ from14A860's membership-key retained matrix. No source
    // null/miss/completeness assertion is manufactured by choosing this branch.
    if(!o.selectedReady[component]){
        {
            if(!lease.frame_||!validRoot(root,lease.frame_->sceneNodeCount))
                return fail(d,Status::identity,0x2413d0);
            const auto& node=lease.frame_->nodes[static_cast<std::uint32_t>(root)];
            if(!(node.sceneFlags190&1u))return fail(d,Status::phase,0x2412e4);
            const light::Vec3 position{{node.world100.v[12],node.world100.v[13],node.world100.v[14]}};
            light::LightMatrices next{};
            if(!o.selectAuthored(o.vehicles[lease.racer_].resetOwner.membership7F0,position,next,d))return false;
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
    if(!owner||!storage_||invalidated_||storage_->sceneOwner||
       storage_->activeLease||storage_->activeCamera||storage_->activeReset||storage_->resetPending())return nullptr;
    auto& s=*storage_;
    if(s.visibility.lifecycle.storage.views.zones!=s.zones.data())return nullptr;
    s.sceneOwner=owner;count=s.world.zones.size();return s.zones.data();
}
ResetVisibilityWorld50EY* Owner::nativeWreckVisibility165E90(const ::ra_pod_scene50LA::Owner* owner,
    const CollectionZone50BX* rows)noexcept {
    if(!owner||!storage_||invalidated_||storage_->sceneOwner!=owner||
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
// One visual-zone lane for the track's animation rows (replaces the TB door,
// TC gate, GB four and MC fourteen copies: the same checks, rows from the data,
// no zone/portal-count pins).
bool Owner::planTrackAnimationRows(std::size_t rows)noexcept {
 if(storage_||invalidated_||rows>maximumTrackAnimationRows)return false;
 plannedAnimationRows_=rows;return true;
}
std::size_t Owner::trackAnimationRows()const noexcept{return storage_&&storage_->animations?storage_->animations->rows.size():0u;}
std::size_t Owner::trackAnimationStorageReservationBytes(std::size_t rows)noexcept {
 // Lane pointer/padding, the lane and its rows, and the full registration
 // payload at the existing eight-racer maximum plus the planned rows.
 if(rows>maximumTrackAnimationRows)return std::numeric_limits<std::size_t>::max();
 return sizeof(std::unique_ptr<Storage::AnimationLane>)+2u*alignof(Storage)+sizeof(Storage::AnimationLane)+
        rows*sizeof(Storage::AnimationLane::Row)+(maximumRacers+1u+rows)*sizeof(ZoneRegistration50CJ);
}
bool Owner::bindTrackAnimationRow(const ::track_animation_binding50AN::Binding* owner,std::size_t index,
 const LiveZoneMembership50EN* membership)noexcept {
 if(!owner||!membership||!storage_||invalidated_||!storage_->animations||index>=storage_->animations->rows.size())return false;
 auto& o=*storage_;auto& lane=*o.animations;
 if(o.activeParticleLight||o.staticAccess||(o.staticOwner&&!o.staticRegistrationComplete)||
    lane.active()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    membership->attached||membership->object||o.visibility.lifecycle.objectCount!=o.identityCount())return false;
 auto& row=lane.rows[index];if(row.owner||row.membership)return false;
 // Rows are claimed in table order by one Binding.
 for(std::size_t i=0;i<lane.rows.size();++i){const auto& prior=lane.rows[i];
  if((i<index&&prior.owner!=owner)||(i>index&&prior.owner))return false;}
 const auto& registration=o.registrations[o.animationBase()+index];
 if(registration.owner||registration.slot||registration.encodedObject)return false;
 row.owner=owner;row.membership=membership;row.complete=false;return true;
}
bool Owner::beginTrackAnimationAccess(const ::track_animation_binding50AN::Binding* owner,std::size_t index,
 ResetVisibilityWorld50EY*& world,LiveZoneWorkspace50EN*& workspace,CollectionRef50BX& object)noexcept {
 world=nullptr;workspace=nullptr;object=0;
 if(!owner||!storage_||invalidated_||!storage_->animations||index>=storage_->animations->rows.size())return false;
 auto& o=*storage_;auto& lane=*o.animations;
 if(o.activeParticleLight||lane.rows[index].owner!=owner||lane.active()||o.staticAccess||
    o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return false;
 auto& row=lane.rows[index];row.access=true;row.complete=false;
 world=&o.visibility;workspace=&o.workspace;object=static_cast<CollectionRef50BX>(o.animationBase()+1u+index);return true;
}
void Owner::endTrackAnimationAccess(const ::track_animation_binding50AN::Binding* owner,std::size_t index)noexcept {
 if(storage_&&owner&&storage_->animations&&index<storage_->animations->rows.size()&&storage_->animations->rows[index].owner==owner)
  storage_->animations->rows[index].access=false;
}
bool Owner::commitTrackAnimationRow(const ::track_animation_binding50AN::Binding* owner,std::size_t index)noexcept {
 if(!storage_||invalidated_||!owner||!storage_->animations||index>=storage_->animations->rows.size())return false;
 auto& o=*storage_;auto& row=o.animations->rows[index];
 if(o.activeParticleLight||row.owner!=owner||!row.access||!row.membership)return false;
 const auto& m=*row.membership;const auto& r=o.registrations[o.animationBase()+index];
 const auto object=static_cast<CollectionRef50BX>(o.animationBase()+1u+index);
 if(!m.attached||m.object!=object||m.method!=ZoneMethod50CL::Second||!r.owner||!m.storage.entries||!m.storage.capacity)return false;
 const auto& primary=m.storage.entries[0].registration;
 if(primary.owner!=r.owner||primary.slot!=r.slot||primary.encodedObject!=r.encodedObject||r.encodedObject!=(object<<2u))return false;
 row.complete=true;return true;
}
bool Owner::releaseTrackAnimationRow(const ::track_animation_binding50AN::Binding* owner,std::size_t index)noexcept {
 if(!storage_||!owner||!storage_->animations||index>=storage_->animations->rows.size())return false;auto& o=*storage_;
 auto& row=o.animations->rows[index];
 if(o.activeParticleLight||row.owner!=owner||o.animations->active()||o.staticAccess||
    o.activeLease||o.activeCamera||o.activeReset||o.resetPending()||
    !row.membership||row.membership->attached||row.membership->object)return false;
 const auto& r=o.registrations[o.animationBase()+index];if(r.owner||r.slot||r.encodedObject)return false;
 row={};return true;
}
bool Owner::currentTrackAnimationRow(const ::track_animation_binding50AN::Binding* owner,std::size_t index)const noexcept {
 if(!storage_||invalidated_||!owner||!storage_->animations||index>=storage_->animations->rows.size())return false;
 const auto& o=*storage_;const auto& row=o.animations->rows[index];
 return row.owner==owner&&row.complete&&!o.animations->active()&&row.membership&&row.membership->attached&&
  row.membership->object==o.animationBase()+1u+index;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::timedLights()const noexcept {return storage_&&!invalidated_&&storage_->timed;}
// 2754E0 ambient flash and 147650 point flashers, as the SCB authors them.
bool Owner::serviceTimedLights(float delta,std::uint32_t& sharedRng,bool paused,Diagnostic& d)noexcept {
    d={};if(!storage_||invalidated_)return fail(d,Status::unprepared,0x2754e0);
    // No authored ambient flash and no emitting flashers: nothing timed to service.
    if(!storage_->timed)return true;
    auto& o=*storage_;
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x2754e0);
    if(o.particleThread!=std::this_thread::get_id())return fail(d,Status::phase,0x2754e0);
    if(paused)return true;
    if(o.timed->revision==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::arithmetic,0x2754e0);
    if(o.timed->savedZones.size()!=o.ambient60.size())return fail(d,Status::identity,0x2754e0);
    auto next=*o.timed;auto raw=sharedRng;bool flash=false;
    if(next.ambientFlash&&!original_light_flash50CR::advanceAmbient(next.ambient,delta,raw,flash))
        return fail(d,Status::arithmetic,0x2754e0);
    for(auto& point:next.points){
        if(point.local>=o.localLightCount||o.localLights[point.local].sourceOrdinal!=point.sourceOrdinal)
            return fail(d,Status::identity,0x147650);
        if(!original_light_flash50CR::advancePoint(point.state,delta,point.color,point.color))
            return fail(d,Status::arithmetic,0x147650);
    }
    // Publish atomically after all bounded scalar work succeeds. This is a
    // native fixed-boundary schedule, not original child-list chronology.
    if(next.ambientFlash){
        o.globalAmbient50=flash?next.flash:next.savedGlobal;
        for(std::size_t z=0;z<o.ambient60.size();++z)o.ambient60[z]=flash?next.flash:next.savedZones[z];
    }
    for(const auto& point:next.points)for(unsigned channel=0;channel<3u;++channel)
        o.localLights[point.local].rgb[channel]=point.color[channel];
    *o.timed=std::move(next);sharedRng=raw;++o.timed->revision;o.selectedReady.fill(false);return true;
}
}

namespace rr::game::original_gc_gameplay_lighting_candidate {
ParticleLightLease::~ParticleLightLease(){if(owner_)owner_->endParticleLightBorrow(*this);}
bool ParticleLightLease::read(ParticleLightInputs& out) const noexcept {
    return owner_&&owner_->readParticleLightBorrow(*this,out);
}
bool Owner::beginParticleLightBorrow(const std::shared_ptr<Owner>& self,
    ParticleLightLease& lease,Diagnostic& d) noexcept {
    d={};
    if(self.get()!=this||lease.owner_||!storage_||invalidated_||!storage_->tally.directional)
        return fail(d,Status::identity,0x149310);
    auto& o=*storage_;
    if(o.particleThread!=std::this_thread::get_id())return fail(d,Status::phase,0x149310);
    if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())
        return fail(d,Status::activeDraw,0x149310);
    lease.owner_=self;lease.revision_=o.timed?o.timed->revision:0u;o.activeParticleLight=&lease;
    return true;
}
bool Owner::readParticleLightBorrow(const ParticleLightLease& lease,ParticleLightInputs& out) const noexcept {
    if(!storage_||invalidated_||lease.owner_.get()!=this||
       storage_->activeParticleLight!=&lease||storage_->particleThread!=std::this_thread::get_id()||
       (storage_->timed?storage_->timed->revision:0u)!=lease.revision_)return false;
    const auto& o=*storage_;
    //1493EC registered directional RGB;149408 original positionA0;
    //1494D4 global ambient+50. No per-zone or local-point substitution.
    out={o.globalAmbient50,o.authoredSunSpecular,o.sunPositionA0,lease.revision_};return true;
}
void Owner::endParticleLightBorrow(ParticleLightLease& lease) noexcept {
    if(!storage_||lease.owner_.get()!=this||storage_->activeParticleLight!=&lease||
       storage_->particleThread!=std::this_thread::get_id())std::terminate();
    storage_->activeParticleLight=nullptr;lease.revision_=0;
    // Keep the owner alive until this member returns, even when this was its
    // last public retain. The moved retain dies only after all owner accesses.
    auto keepAlive=std::move(lease.owner_);
}
bool Owner::queryZoneLight(std::uint32_t zone,const std::array<float,3>& position,
 original_racer_lighting50KM::LightMatrices& out,Diagnostic& d)const noexcept{
 d={};if(!storage_||invalidated_)return fail(d,Status::unprepared,0x149680);
 const auto& o=*storage_;
 if(o.staticBusy()||o.activeLease||o.activeCamera||o.activeReset||o.resetPending())return fail(d,Status::activeDraw,0x149680);
 // Value query only, not a live membership/renderer receipt. Same real portal links and float ordering.
 return o.selectAuthoredZones(zone,nullptr,position,out,d);
}
std::size_t Owner::retainedPayloadBytes()const noexcept {
 if(!storage_)return 0;
 const auto& o=*storage_;std::size_t n=sizeof(Owner)+sizeof(Storage)+o.topology.bytes()+
  (o.animations?sizeof(Storage::AnimationLane)+o.animations->rows.capacity()*sizeof(Storage::AnimationLane::Row):0u)+
  (o.timed?sizeof(Storage::TimedLights)+o.timed->points.capacity()*sizeof(Storage::TimedLights::Point)+
   o.timed->savedZones.capacity()*sizeof(light::Vec4):0u);
 auto add=[&](std::size_t x){n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;};
 auto vector=[&](const auto& v){using T=typename std::decay_t<decltype(v)>::value_type;if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(v.capacity()*sizeof(T));};
 auto text=[&](const auto& t){add(t.capacity());add(1);};
 vector(o.registrations.rows);vector(o.staticModels);vector(o.staticSelection);vector(o.localLights);
 const auto& w=o.world;vector(w.parameters.original_bytes);vector(w.parameters.blocks);
 for(const auto& b:w.parameters.blocks){text(b.name);vector(b.fields);for(const auto& f:b.fields){text(f.name);text(f.value);}}
 vector(w.zones);vector(w.portals);vector(w.instances);for(const auto& p:w.portals)text(p.section);
 for(const auto& i:w.instances){text(i.section);text(i.geometry_member);text(i.collision_member);}
 for(const auto& m:o.selectedMethods)vector(m.selected18);
 return n; // Conservative string-capacity charge includes inline storage twice.
}
}

#include "original_ra_scene_roles270900.inl"
