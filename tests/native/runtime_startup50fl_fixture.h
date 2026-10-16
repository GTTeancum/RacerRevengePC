#pragma once
#include "runtime_asset_paths50lb.h"
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
    const auto path=runtime_asset_paths50LB::resolve(requested);
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
struct Assets {
    std::array<std::vector<std::uint8_t>,3> psg,vut;
    std::array<std::vector<std::uint8_t>,6> mot;
    std::array<rr::assets::PsgHierarchyView49O,3> hierarchy{};
    std::array<rr::assets::MotionNodeBindings49O,3> rows{};
    std::array<rr::assets::MotionView49O,6> motions{};
    explicit Assets(const std::filesystem::path& root){
        const auto directory=root/"evidence/authoredGeometry49O/asset-reader/fixtures";
        const char* ps[]={"48-englt1.psg","43-engrt1.psg","52-chariot.psg"};
        const char* vs[]={"49-englt1.vut","44-engrt1.vut","53-chariot.vut"};
        const char* ms[]={"50-englt1.mot","45-engrt1.mot","54-01_brklt.mot","55-01_brkrt.mot","56-01_forwd.mot","57-01_back.mot"};
        const U offsets[]={1,16,31};
        rr::assets::GeometryDiagnostic49O geometry{};rr::assets::MotionBindingDiagnostic49O binding{};
        for(U i=0;i<3;++i){
            psg[i]=readBytes(directory/ps[i]);vut[i]=readBytes(directory/vs[i]);
            require(rr::assets::readPsgHierarchy49O(psg[i].data(),psg[i].size(),hierarchy[i],geometry),"native PSG reader");
            require(rr::assets::readMotionNodeBindings49O(hierarchy[i],reinterpret_cast<const char*>(vut[i].data()),vut[i].size(),rows[i],binding),"native VUT reader");
            for(U row=0;row<rows[i].count;++row)rows[i].rows[row].sceneNode=std::uint8_t(rows[i].rows[row].sceneNode+offsets[i]);
        }
        require(hierarchy[0].nodeCount==14&&hierarchy[1].nodeCount==14&&hierarchy[2].nodeCount==18,"native startup pod topology");
        for(U i=0;i<6;++i){mot[i]=readBytes(directory/ms[i]);require(rr::assets::readMot49O(mot[i].data(),mot[i].size(),motions[i],geometry),"native MOT reader");}
    }
    Graph graph()const{
        Graph graph{};
        for(auto& n:graph){n.localC0=identity();n.parent180=n.firstChild184=n.nextSibling188=Index::none;n.sceneFlags190=0x84;}
        const U roots[]={0,15,30},offsets[]={1,16,31};
        for(U model=0;model<3;++model)for(U row=0;row<hierarchy[model].nodeCount;++row){
            rr::assets::PsgNode49O node{};require(rr::assets::readPsgNode49O(hierarchy[model],row,node),"native PSG node");
            const U at=offsets[model]+row,parent=node.parent<0?roots[model]:offsets[model]+U(node.parent);
            std::memcpy(graph[at].localC0.v,node.local,64);graph[at].parent180=ix(parent);
            auto& first=graph[parent].firstChild184;
            if(first==Index::none)first=ix(at);
            else{U previous=U(first);while(graph[previous].nextSibling188!=Index::none)previous=U(graph[previous].nextSibling188);graph[previous].nextSibling188=ix(at);}
        }
        // Existing fresh engine-nullcg normalization, before any animation.
        for(U node:{1u,16u})for(U lane:{12u,13u,14u})graph[node].localC0.v[lane]=0;
        graph[50].firstChild184=ix(51);graph[51].parent180=ix(50);
        graph[52].sceneFlags190=0x80;
        // Retain all original53-node words; only new spare capacity is identity.
        for(U node=53;node<vehicleFrameNodeCount49W;++node){graph[node].world100=identity();graph[node].sceneFlags190=0;}
        return graph;
    }
};
struct Fixture {
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
    explicit Fixture(float initialSpeed=0,const std::filesystem::path& root="."):assets(root){
        require(std::fegetround()==FE_TONEAREST&&std::isfinite(initialSpeed),"native startup numeric context");
        const auto graph=assets.graph();std::copy(graph.begin(),graph.end(),state.nodes);
        auto& animation=bindings.animation;
        animation.chariot1B4=ix(30);animation.left1BC=ix(0);animation.right1B8=ix(15);animation.bodyScene288=ix(50);
        animation.chariotRows=&assets.rows[2];animation.leftRows=&assets.rows[0];animation.rightRows=&assets.rows[1];
        animation.brakeLeft28C=&assets.motions[2];animation.brakeRight290=&assets.motions[3];animation.forward280=&assets.motions[4];animation.back284=&assets.motions[5];animation.left2A0=&assets.motions[0];animation.right29C=&assets.motions[1];
        animation.hands={{ix(41),ix(43),ix(44),ix(47)},{ix(42),ix(45),ix(46),ix(48)}};animation.handRecords=&handRecords;
        ChariotIkWorkspace49T ikWorkspace{};ChariotIkDiagnostic49T ikDiagnostic{};
        require(initializeChariotIk49T(animation.hands,{state.nodes,53},handRecords,state.animationShared.libm,ikWorkspace,ikDiagnostic),"native authored IK constructors");
        require(initializeVehicleAnimationFilters49T(0,0,state.animation),"native animation filters");
        // Explicit, unchanged development model/cache/lazy-record inputs. These
        // are caller-owned seeds, not captured constructor outputs or poses.
        state.animation.chariotModel.model18=0xabc123;state.animation.leftModel.model18=0x987fed;state.animation.rightModel.model18=0xdead123;
        state.animationShared.enable434748=1;state.animationShared.enableGuard3D60F4=1;
        state.animationShared.motion.interpolationValue4332F0=0x12345678;state.animationShared.motion.pairValue433478=0x9abcdef0;
        state.animationShared.ik.sentinel443450={0,0x87654321};state.animationShared.ik.sentinel443458={0,0xfedcba98};
  for(unsigned i=0;i<30;++i)state.nodes[i].world100=matrix();
  auto g=SceneGraph49P{state.nodes,53};
  state.nodes[50]={matrix(.05f,10,20,30),matrix(.05f,10,20,30),Index::none,ix(51),Index::none,0x85};
  state.nodes[51]={matrix(),matrix(),ix(50),Index::none,Index::none,0x84};
  state.nodes[49]={matrix(.05f,10,20,25),matrix(.05f,10,20,25),Index::none,Index::none,Index::none,0x85};
  require(setLocalMatrix49P(g,ix(30),state.nodes[49].localC0),"initial displayed pose");
  bindings.visual={ix(50),ix(51),ix(15),ix(0),false,false,0};
  bindings.lifecycle={ix(50),ix(51),ix(52),ix(49),ix(30),ix(0),ix(15),ix(35),ix(36)};
  for(unsigned i=0;i<vehicleFrameNodeCount49W;++i)bindings.topology[i]={state.nodes[i].parent180,state.nodes[i].firstChild184,state.nodes[i].nextSibling188};
  bindings.mainDomain=bindings.secondaryDomain={2,false,false,GraphBodyIntegrator49W::OriginalAdamsBashforth2,GraphBodyCallback49W::Clear238};
  rr::assets::initializeVehicleVisualSharedColdRecords49R(state.visualShared);
  const auto visualPhy=readBytes(root/"evidence/authoredInitialization49N/authoring-source/Pod01.phy");
  rr::assets::VisualAuthoredValues49R visualValues{};
  rr::assets::VisualAuthoringDiagnostic49R visualDiagnostic{};
  require(rr::assets::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(visualPhy.data()),visualPhy.size(),visualValues,visualDiagnostic),"native visual PHY");
  require(rr::assets::bindFreshVehicleVisualParameters49R(visualValues,assets.hierarchy[1],assets.hierarchy[0],parameters.visual,visualDiagnostic),"native visual parameters");
  state.visual.upC20=state.visual.publishedUpAF0={{0,1,0,1}};
  state.visual.right.position=parameters.visual.rightBase2C0;state.visual.left.position=parameters.visual.leftBase2B0;
  state.visual.right.linearVelocity=state.visual.left.linearVelocity=state.visual.right.angularVelocity=state.visual.left.angularVelocity={{0,0,0,1}};
  state.visual.right.orientation=state.visual.left.orientation={{1,0,0,0}};
  const auto& phy=visualPhy;
  rr::assets::ChariotRootAuthoredValues49U raw{};rr::assets::ChariotRootAuthoringDiagnostic49U rd{};
  require(rr::assets::readChariotRootAuthoring49U(reinterpret_cast<const char*>(phy.data()),phy.size(),raw,rd),"root PHY");
  FreshPodGeometrySources49O gs{};gs.chariot=assets.hierarchy[2];gs.rightEngine=assets.hierarchy[1];gs.forward=assets.motions[4];gs.back=assets.motions[5];rr::assets::MotionBindingDiagnostic49O bd{};
  require(rr::assets::readMotionNodeBindings49O(assets.hierarchy[2],reinterpret_cast<const char*>(assets.vut[2].data()),assets.vut[2].size(),gs.chariotBindings,bd),"geometry bindings");
  FreshPodGeometryWorkspace49O gw{};FreshPodGeometryResult49O geom{};FreshPodGeometryDiagnostic49O gd{};require(buildFreshPodGeometry49O(gs,gw,geom,gd),"geometry");
  require(rr::assets::bindFreshChariotRootParameters49U(raw,geom,parameters.visual,parameters.lifecycle,rd),"root params");
  AuthoredParameters49N authored{};rr::assets::PodPhysicsDiagnostic49N pd{};require(rr::assets::readPodPhysics49N(reinterpret_cast<const char*>(phy.data()),phy.size(),authored,pd),"control PHY");
  auto value=[&](AuthoredKey49N k){require(authored.values[U(k)].present,"control key");return authored.values[U(k)].value;};
  parameters.control={value(AuthoredKey49N::SteerSpeed1),value(AuthoredKey49N::SteerScale1),value(AuthoredKey49N::SteerSpeed2),value(AuthoredKey49N::SteerScale2),.5f,.25f};
  parameters.rates={value(AuthoredKey49N::XAxisRateLimit),value(AuthoredKey49N::YAxisRateLimit)};
  auto&m=state.bodies.main;auto&z=state.bodies.secondary;
  m.domain={1,0,1,0,true,false,true,true,false,false,0,false,false};
  state.bodies.secondaryDomain={1,2,0,0,0,false};
  m.components.root.body.orientation={{1,0,0,0}};m.components.root.body.velocity={{0,0,initialSpeed}};m.components.root.body.previousPositionDerivative=m.components.root.body.velocity;m.components.root.localVelocity80={{0,0,initialSpeed}};m.cachedSpeed34=initialSpeed;
  state.bodies.mainSphere={0xa7000085u,-0.0f,.75f,1,1};
  z.scenePresent=z.enabled=true;z.sceneFlags190=0xab74c585u;z.phase=SecondaryPhase49M::Ready;z.root.body.orientation={{1,0,0,0}};z.root.body.velocity=m.components.root.body.velocity;z.root.body.previousPositionDerivative=z.root.body.velocity;z.root.localVelocity80=z.root.body.velocity;z.cachedSpeed34=initialSpeed;z.velocity70W=-0.0f;z.angularVelocityB0W=.75f;z.force180W=z.torque190W=1;
  m.phase=MainBodyPhase49K::FrameReady;state.bodies.phase=CoupledBodyPhase49M::FrameReady;state.phase=VehicleFramePhase49W::Ready;
  m.controls.player.flagsB9C=0x80;m.components.probe348.vector1D0[3]=1;state.mainSupplement.localVelocity80W=state.secondarySupplement.localVelocity80W=1;state.mainSupplement.linearAccelerationC0W=state.secondarySupplement.linearAccelerationC0W=1;
  initializedSecondaryActivationGlobals49V(parameters.activation);
  ChariotRootState49U init{};rr::assets::initializeChariotRootConstructorRecords49U(init,z.time864);state.bodies.vehicle838=init.vehicle838;state.lifecycle={init.vehicle83C,init.vehicle840,1,0,init.previous85C,init.response};rr::assets::initializeChariotRootSharedColdRecords49U(state.lifecycleShared);
  state.noise.sharedRandomState=0x12345678;
  state.fields.chariot191=state.fields.left191=state.fields.right191=1;state.fields.impactBA0={{0,0,0,1}};state.fields.objectD34_10=.5f;
  parameters.bodies.main.body=parameters.bodies.secondary.body=bodyParameters();
  parameters.bodies.main.engine={100,150,60,.1f,.05f,10,1000,1000,1000,{{{0,0,0}},{{0,0,0}}}};
  parameters.bodies.main.probeLocal140[3]=1;parameters.bodies.main.effective434260[3]=1;parameters.bodies.main.effective4342C8=.3f;
  parameters.bodies.mainSphere.centerOffsetW=parameters.bodies.secondary.centerOffsetW=1;parameters.bodies.mainSphere.inverseInertiaExtra[6]=parameters.bodies.secondary.inverseInertiaExtra[6]=1;
  SecondaryForceContext49M def{};initializedSecondaryGlobals49M(def);
  secondaryInputs={9.8f,12,{{0,0,-5,1}},def.effective4346C8,def.effective4346D0,def.effective4346D8,def.effective4346E0,def.effective4346E8,def.effective4346F0,def.effective4346F8,def.effective434700,def.effective434708};
  selectors={9.8f,12,fp(0x43c13334),fp(0x3f34fdf4),0,0};
  project(state,bindings);
  PodSphereBody49L a{},bb{};makeSecondarySphereBody49M(z,parameters.bodies.secondary,a);auto mainView=z;mainView.root=m.components.root;mainView.sceneFlags190=state.bodies.mainSphere.sceneFlags190;makeSecondarySphereBody49M(mainView,parameters.bodies.secondary,bb);
  require(initializePodSphere49L(state.bodies.joint)&&setupPodSphereA49L(state.bodies.joint,a,{{0,0,0,1}})&&setupPodSphereB49L(state.bodies.joint,&bb,{{0,0,0,1}},5),"initial joint");
  auto shape=readBytes(root/"evidence/frameBridge49W/shape/podboxf.col");rr::assets::DecodeError48C error{};require(rr::assets::decodePodShapeSource49W(shape,parameters.shape,error),"authored pod shape");state.shape.registrationE0=0;
  m.currentPoint7C0[3]=-71;
 }
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
            retained->preserveColdTargetForHandling(*actual,projectedGeometry);}
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
