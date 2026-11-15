// Exact existing non-template bodies, outlined without owner or layout changes.
#include "../../src/assets/dlc_files50dl.h"
#include "runtime_startup50fl_fixture.h"

namespace runtime_startup50fl {
Assets::Assets(const std::filesystem::path& root,AssetInput input){
        const auto directory=root/"evidence/authoredGeometry49O/asset-reader/fixtures";
        const char* ps[]={"48-englt1.psg","43-engrt1.psg","52-chariot.psg"};
        const char* vs[]={"49-englt1.vut","44-engrt1.vut","53-chariot.vut"};
        const char* ms[]={"50-englt1.mot","45-engrt1.mot","54-01_brklt.mot","55-01_brkrt.mot","56-01_forwd.mot","57-01_back.mot"};
        const U offsets[]={1,16,31};
        if(input==AssetInput::OriginalDisc){
            originalArchivePath=runtime_asset_paths50LB::resolve(root/"work/native/project/disc/PODS/POD01/POD01.RES");
            const auto* pin=original_player_identity50IK::find(1u);
            require(pin&&std::filesystem::file_size(rr::assets::dlc50DL::host(originalArchivePath))==pin->archiveBytes,
                "original POD01 bootstrap archive extent differs");
            originalArchive=readBytes(originalArchivePath);
            require(originalArchive.size()==pin->archiveBytes&&
                pod_materials50ed_fixture::detail::sha256(originalArchive)==pin->archiveSha256,
                "original POD01 bootstrap archive identity differs");
            rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
            require(rr::assets::decodeResource48C(originalArchive,archive,error),"original POD01 bootstrap archive rejected");
            const auto member=[&](const char* name){
                std::vector<std::uint8_t> bytes;
                if(!rr::assets::extractResMember48C(archive,std::string("Pods\\Pod01\\")+name,bytes,error))
                    throw std::runtime_error(std::string("original POD01 bootstrap member missing: ")+name);
                return bytes;
            };
            const char* originalPsg[]={"englt1.psg","engrt1.psg","chariot.psg"};
            const char* originalVut[]={"englt1.vut","engrt1.vut","chariot.vut"};
            const char* originalMot[]={"englt1.mot","engrt1.mot","01_brklt.mot","01_brkrt.mot","01_forwd.mot","01_back.mot"};
            for(U i=0;i<3;++i){psg[i]=member(originalPsg[i]);vut[i]=member(originalVut[i]);}
            for(U i=0;i<6;++i)mot[i]=member(originalMot[i]);
            phy=member("Pod01.phy");podShape=member("podboxf.col");
            require(pod_materials50ed_fixture::detail::sha256(phy)==pin->phySha256,
                "original POD01 bootstrap PHY identity differs");
        }else{
            for(U i=0;i<3;++i){psg[i]=readBytes(directory/ps[i]);vut[i]=readBytes(directory/vs[i]);}
            for(U i=0;i<6;++i)mot[i]=readBytes(directory/ms[i]);
            phy=readBytes(root/"evidence/authoredInitialization49N/authoring-source/Pod01.phy");
            podShape=readBytes(root/"evidence/frameBridge49W/shape/podboxf.col");
        }
        rr::assets::GeometryDiagnostic49O geometry{};rr::assets::MotionBindingDiagnostic49O binding{};
        for(U i=0;i<3;++i){
            require(rr::assets::readPsgHierarchy49O(psg[i].data(),psg[i].size(),hierarchy[i],geometry),"native PSG reader");
            require(rr::assets::readMotionNodeBindings49O(hierarchy[i],reinterpret_cast<const char*>(vut[i].data()),vut[i].size(),rows[i],binding),"native VUT reader");
            for(U row=0;row<rows[i].count;++row)rows[i].rows[row].sceneNode=std::uint8_t(rows[i].rows[row].sceneNode+offsets[i]);
        }
        require(hierarchy[0].nodeCount==14&&hierarchy[1].nodeCount==14&&hierarchy[2].nodeCount==18,"native startup pod topology");
        for(U i=0;i<6;++i){require(rr::assets::readMot49O(mot[i].data(),mot[i].size(),motions[i],geometry),"native MOT reader");}
    }

Graph Assets::graph()const{
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

// Separate native storage admission. The in-class native backing defaults are
// NOT guest constructor receipts. In particular this does not call graph(),
// animation/IK, PHY binders, body/joint setup, projection or FrameReady setters.
Fixture::Fixture(SourceFreshStorageOnly,const std::filesystem::path& root)
    :assets(root,AssetInput::OriginalDisc) {}

Fixture::Fixture(float initialSpeed,const std::filesystem::path& root,
    AssetInput input):assets(root,input){
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
  const auto& visualPhy=assets.phy;
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
  const auto& shape=assets.podShape;rr::assets::DecodeError48C error{};require(rr::assets::decodePodShapeSource49W(shape,parameters.shape,error),"authored pod shape");state.shape.registrationE0=0;
  m.currentPoint7C0[3]=-71;
 }
} // namespace runtime_startup50fl
