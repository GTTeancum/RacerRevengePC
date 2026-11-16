#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "runtime_asset_paths50lb.h"
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/import_decode48c.h"
// Native-only cold owner for the development client. This preserves the explicit
// supplied development context of frame49w_test::Fixture(0); geometry, IK,
// visual/root/physics parameters and joints are computed by native production
// functions from the same authored assets. No saved output or guest Machine is
// loaded or executed. Full application/player construction remains out of scope.
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/pod_model_geometry49o.h"
#include "../../src/assets/pod_shape49w.h"
#include "../../src/assets/vehicle_visual_authoring49r.h"
#include "../../src/assets/chariot_root_authoring49u.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <type_traits>

namespace ra_cold_roster50LA { class FreshExecutionArena; }
namespace runtime_startup50fl {
using namespace rr::game;
using U=std::uint32_t;
using Index=SceneNodeIndex49P;
using State=VehicleFrameState49W;
using Graph=std::array<SceneNode49P,vehicleFrameNodeCount49W>;
inline void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
inline Index ix(U n){return static_cast<Index>(n);}
inline float fp(U word){float value;std::memcpy(&value,&word,4);return value;}
inline BodyMatrix48G matrix(float angle=0,float tx=0,float ty=0,float tz=0){
    const float cosine=std::cos(angle),sine=std::sin(angle);
    return {{cosine,0,-sine,0,0,1,0,0,sine,0,cosine,0,tx,ty,tz,1}};
}
inline BodyMatrix48G identity(){BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1;return m;}
inline std::vector<std::uint8_t> readBytes(const std::filesystem::path& requested){
    const auto path=rr::assets::dlc50DL::host(runtime_asset_paths50LB::resolve(requested));
    std::ifstream file(path,std::ios::binary|std::ios::ate);
    if(!file)throw std::runtime_error("native startup asset missing: "+path.string());
    const auto size=file.tellg();require(size>=0,"native startup asset size");
    std::vector<std::uint8_t> out(static_cast<std::size_t>(size));file.seekg(0);
    file.read(reinterpret_cast<char*>(out.data()),size);
    require(bool(file),"native startup asset read");return out;
}
inline PreservingStage49W cut(PreservingCut49W p){return {p,true,true};}
inline void project(State& s,const VehicleFrameBindings49W& b){
    auto& m=s.bodies.main;auto& z=s.bodies.secondary;
    m.components.root.scene=s.nodes[U(b.lifecycle.main190)].world100;
    z.root.scene=s.nodes[U(b.lifecycle.physical19C)].world100;
    s.bodies.mainSphere.sceneFlags190=(s.bodies.mainSphere.sceneFlags190&0xffffff00u)|s.nodes[U(b.lifecycle.main190)].sceneFlags190;
    z.sceneFlags190=(z.sceneFlags190&0xffffff00u)|s.nodes[U(b.lifecycle.physical19C)].sceneFlags190;
}
inline BodyParameters48C bodyParameters(){
    BodyParameters48C p{};p.mass=100;p.inverseMass=.01f;
    for(unsigned i=0;i<3;++i){p.inverseInertiaColumns[i].v[i]=.001f;p.translationEnabled[i]=p.rotationEnabled[i]=true;}
    p.linearAccelerationLimitSquared=p.angularAccelerationLimitSquared=p.linearVelocityLimitSquared=p.angularVelocityLimitSquared=1e8f;
    return p;
}
// Explicit cold input selection. The ordinary RA route owns the original
// POD01 bytes in memory; older diagnostic callers retain their loose inputs.
// This changes only input ownership, not Fixture's development arena semantics.
enum class AssetInput { RetainedDevelopment, OriginalDisc };
struct Assets {
    std::vector<std::uint8_t> originalArchive,phy,podShape;
    std::filesystem::path originalArchivePath;
    std::array<std::vector<std::uint8_t>,3> psg,vut;
    std::array<std::vector<std::uint8_t>,6> mot;
    std::array<rr::assets::PsgHierarchyView49O,3> hierarchy{};
    std::array<rr::assets::MotionNodeBindings49O,3> rows{};
    std::array<rr::assets::MotionView49O,6> motions{};
    explicit Assets(const std::filesystem::path& root,AssetInput input=AssetInput::RetainedDevelopment);
    Graph graph()const;
};
struct Fixture {
private:
    friend class ::ra_cold_roster50LA::FreshExecutionArena;
    struct SourceFreshStorageOnly {};
    Fixture(SourceFreshStorageOnly,const std::filesystem::path& root);
public:
    Assets assets;
    ChariotIkPair49T handRecords{};
    State state{};
    VehicleFrameBindings49W bindings{};
    VehicleFrameParameters49W parameters{};
    VehicleFrameWorkspace49W workspace{};
    VehicleFrameDiagnostic49W diagnostic{};
    SecondaryInputs49M secondaryInputs{};
    SubstepSelectionContext49K selectors{};
    unsigned frame=0;
    PlayerActionCodes48X actionCodes{-1,-1,-1,-1,-1};
    bool configurationEntryNonzero=true;
    explicit Fixture(float initialSpeed=0,const std::filesystem::path& root=".",
        AssetInput input=AssetInput::RetainedDevelopment);
    Fixture(const Fixture&)=delete;
    Fixture& operator=(const Fixture&)=delete;
};

// Same native49N/49O operations as the old diagnostic binder, without its
// source-word execution and comparisons. Outputs retain exactly one canonical
// parameter/state owner and the existing serial fresh-initialization contract.
template<class ColdOwner=std::nullptr_t>
inline AuthoredBinding49N bind(const std::vector<std::uint8_t>& phy,
    const FreshPodGeometryResult49O& geometry,const AuthoredSelectionContext49N& selection,
    VehicleFrameParameters49W& parameters,State& state,
    SubstepSelectionContext49K& gravity,SecondaryInputs49M& secondary,
    ColdOwner* retained=nullptr,Fixture* actual=nullptr){
    AuthoredParameters49N raw{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
    require(rr::assets::readPodPhysics49N(reinterpret_cast<const char*>(phy.data()),phy.size(),raw,diagnostic),"native authored PHY admission");
    AuthoredBinding49N previous{},binding{};previous.main=parameters.bodies.main;
    previous.controls=parameters.control;previous.rates=parameters.rates;
    previous.gravity378=gravity.vehicle378;previous.airborneGravity37C=gravity.vehicle37C;
    require(bindAuthoredParameters49N(raw,selection,previous,binding),"native authored main binding");
    require(projectAuthoredCoupledParameters49N(binding,parameters.bodies,parameters.bodies),"native authored parameter projection");
    parameters.control=binding.controls;parameters.rates=binding.rates;
    require(projectAuthoredGravity49N(binding,gravity,secondary,gravity,secondary),"native authored gravity projection");
    auto projectedGeometry=geometry.parameters;
    if constexpr(!std::is_same_v<ColdOwner,std::nullptr_t>){
        if(retained){require(actual,"retained cold geometry requires its actual Fixture");
            retained->preserveColdTargetForHandling(*actual,projectedGeometry);
            retained->constructNativeSecondaryContacts212538(*actual,state);}
    }
    require(projectAuthoredGeometry49O(projectedGeometry,parameters.bodies,secondary,parameters.bodies,secondary),"native authored secondary geometry projection");
    SecondaryBodyState49M mainView{};mainView.root=state.bodies.main.components.root;
    mainView.scenePresent=state.bodies.main.domain.sceneAttached;mainView.sceneFlags190=state.bodies.mainSphere.sceneFlags190;
    mainView.velocity70W=state.bodies.mainSphere.velocity70W;mainView.angularVelocityB0W=state.bodies.mainSphere.angularVelocityB0W;
    mainView.force180W=state.bodies.mainSphere.force180W;mainView.torque190W=state.bodies.mainSphere.torque190W;
    SecondaryBodyParameters49M mainParameters{};mainParameters.body=parameters.bodies.main.body;
    mainParameters.centerOffset=parameters.bodies.main.centerOffset;mainParameters.centerOffsetW=parameters.bodies.mainSphere.centerOffsetW;
    std::memcpy(mainParameters.inverseInertiaExtra,parameters.bodies.mainSphere.inverseInertiaExtra,sizeof mainParameters.inverseInertiaExtra);
    PodSphereBody49L mainSphere{};makeSecondarySphereBody49M(mainView,mainParameters,mainSphere);
    if constexpr(!std::is_same_v<ColdOwner,std::nullptr_t>){
        if(retained){
            retained->setupColdJoint2126(*actual,projectedGeometry,mainSphere,state.bodies.joint);
            // Preserve the actual partial858 publication in the working state
            // that handling will commit; the canonical owner was written first.
            state.fields.length858=actual->state.fields.length858;
        }else require(setupAuthoredPodSphere49O(geometry.parameters,mainSphere,state.bodies.joint,state.bodies.joint),"native authored sphere setup");
    }else require(setupAuthoredPodSphere49O(geometry.parameters,mainSphere,state.bodies.joint,state.bodies.joint),"native authored sphere setup");
    return binding;
}
}
