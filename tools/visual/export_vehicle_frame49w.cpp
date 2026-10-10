#include <string>
#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include "../../evidence/frameBridge49W/connected/authored_inputs49w.h"
#include "../../src/game/control_presets49d.h"
#include <set>
namespace advancing49w {
namespace f=frame49w_test;
namespace old=chariotlifecycle49v;
namespace c=connected49r;
namespace t=vehicle_animation49t_test;
using namespace rr::game;
using J=nlohmann::json;
using U=std::uint32_t;
constexpr U frameCount=144;
using Bytes=std::vector<std::uint8_t>;
namespace fs=std::filesystem;
template<class T>J raw(const T& object){static_assert(sizeof(T)%4==0);std::array<U,sizeof(T)/4> words{};std::memcpy(words.data(),&object,sizeof object);return J(words);}
static c::Graph graphCopy(const VehicleFrameState49W&s){c::Graph g{};std::copy_n(s.nodes,53,g.begin());return g;}
static J body(const RootBodyState48H&r,const SecondaryActivationSupplement49V&s,const GraphBodyExtraLanes49W&e){static_assert(sizeof(RootBodyState48H)==320);return {{"root_words",raw(r)},{"orientation1F0",raw(r.body.orientation)},{"derivative_supplement_words",raw(s)},{"extra_lane_words",raw(e)}};}
static J joint(const PodSphereJoint49L&j){return {{"stiffness_damping",old::scalarWords({j.stiffness30,j.damping34})},{"flags",{j.enabled38,j.unilateral3C,j.suppressed40}},{"body_present",{j.bodyAPresent44,j.bodyBPresent48}},{"tail_words",old::words(&j.length4C,(sizeof(j)-offsetof(PodSphereJoint49L,length4C))/4)}};}
// advancing49w.trace.v1 uses stable phase IDs, not the runtime enum's ordinal.
// The impact/repair stages inserted into the runtime ordering must not renumber
// existing trace states. Keep new stage IDs distinct without changing runtime
// ordering or schedule admission.
constexpr U tracePhase49W(VehicleFramePhase49W p) {
 using P=VehicleFramePhase49W;
 switch(p) {
 case P::Ready:return 0; case P::PostPlayer:return 1;
 case P::BeforeGraphics:return 2; case P::PostGraphics:return 3;
 case P::BetweenBodyChildren:return 4; case P::AfterBodyChildren:return 5;
 case P::SubstepReady:return 6; case P::PostWorld:return 7;
 case P::Post354:return 8; case P::Post368:return 9;
 case P::MainIntegration:return 10; case P::Query219120:return 11;
 case P::PostImpactConsumers:return 12; case P::PostImpactRepair:return 13;
 case P::AfterImpactBlock:return 14;
 }
 throw std::runtime_error("unrepresented frame trace phase");
}
static_assert(tracePhase49W(VehicleFramePhase49W::SubstepReady)==6);
static_assert(tracePhase49W(VehicleFramePhase49W::AfterImpactBlock)==14);
static J state(const VehicleFrameState49W&s){const auto&m=s.bodies.main;const auto&z=s.bodies.secondary;return {
 {"graph",old::graph(graphCopy(s))},{"main",body(m.components.root,s.mainSupplement,s.mainExtra)},{"secondary",body(z.root,s.secondarySupplement,s.secondaryExtra)},
 {"main_w_lanes",old::scalarWords({s.bodies.mainSphere.velocity70W,s.bodies.mainSphere.angularVelocityB0W,s.bodies.mainSphere.force180W,s.bodies.mainSphere.torque190W})},
 {"secondary_w_lanes",old::scalarWords({z.velocity70W,z.angularVelocityB0W,z.force180W,z.torque190W})},{"main_scene_flags",s.bodies.mainSphere.sceneFlags190},{"secondary_scene_flags",z.sceneFlags190},
 {"main_cached_speed",bits(m.cachedSpeed34)},{"secondary_cached_speed",bits(z.cachedSpeed34)},{"main_cached_angles",raw(m.cachedAnglesE0)},{"secondary_cached_angles",raw(z.cachedAnglesE0)},
 {"joint",joint(s.bodies.joint)},{"physical838",s.bodies.vehicle838},{"phase864",bits(z.time864)},{"lifecycle",old::rootState(s.lifecycle,s.bodies.vehicle838)},{"lifecycle_shared",old::rootShared(s.lifecycleShared)},
 {"animation",old::animationState(s.animation)},{"animation_shared",old::animationShared(s.animationShared)},{"visual",old::visualState(s.visual)},{"visual_shared",old::sharedState(s.visualShared,s.noise)},
 {"engine",raw(m.components.engine)},{"aero",{{"timer140",bits(m.components.aero.timer140)},{"timer144",bits(m.components.aero.timer144)},{"low_speed",m.components.aero.lowSpeedDc}}},
 {"bindings",old::bindings(m.components.bindings)},{"conditioned_controls",raw(m.controls.components)},{"player_actions",old::scalarWords({m.controls.player.actionB7C,m.controls.player.actionB80})},{"shaping",raw(m.controls.bound.control.shaping)},{"mirrored_bound_bindings",old::bindings(m.controls.bound.bindings)},{"raw_controls",raw(m.controls.player.raw)},{"filtered",raw(m.controls.bound.control.filtered)},{"previousD28",bits(m.controls.bound.control.shaping.smoothedChannel0D28)},{"flagsB9C",m.controls.player.flagsB9C},
 {"probe",raw(m.components.probe348)},{"feedback",raw(m.components.feedback)},{"main20C",m.components.body20C},{"secondary20C",z.body20C},{"gravity378_37C",old::scalarWords({s.fields.gravity378,s.fields.gravity37C})},{"impactBA0",raw(s.fields.impactBA0)},{"inverseBB0XYZ",raw(s.fields.inverseImpactBB0XYZ)},
 {"current7C0",old::words(m.currentPoint7C0,4)},{"prior7B0",old::words(m.priorPoint7B0,4)},{"vehicle380",m.vehicle380},{"main_domain",{m.domain.body320,m.domain.bodyChildCount,m.domain.sceneChildCount,m.domain.sceneObserverCount,U(m.domain.sceneAttached),U(m.domain.sceneHasParent),U(m.domain.bodyEnabled),U(m.domain.secondBodyPresent),U(m.domain.override3C),U(m.domain.resetRequested),m.domain.probe100,U(m.domain.probe120Present),U(m.domain.probe124Present)}},{"secondary_domain",{s.bodies.secondaryDomain.body320,s.bodies.secondaryDomain.registrationMask,s.bodies.secondaryDomain.bodyChildCount,s.bodies.secondaryDomain.sceneChildCount,s.bodies.secondaryDomain.sceneObserverCount,U(s.bodies.secondaryDomain.sceneHasParent)}},{"secondary_present_enabled",{z.scenePresent,z.enabled}},{"last_constraint_magnitude",bits(s.bodies.lastConstraintMagnitude)},{"selected_gravity",bits(m.selectedGravity)},{"selected_engine_flags",{m.selectedEngineFlags.engine34,m.selectedEngineFlags.engine38}},{"response354",{m.response354_3C,m.response354_44}},{"vehicle40_application11C",{m.vehicleState40,m.applicationActive11C}},{"live_substep_scale",bits(m.liveSubstepScale)},{"frame_factor",bits(m.frameFactor)},{"substeps",m.schedule.count},{"substep_factor",bits(m.schedule.factorPerSubstep)},{"substep_index",m.substepIndex},
 {"shape",{{"composed",raw(s.shape.composed00)},{"current",raw(s.shape.world80)},{"previous",raw(s.shape.previousC0)},{"center",raw(s.shape.center60)},{"radius",bits(s.shape.radius80)},{"type",s.shape.type88},{"registration",s.shape.registrationE0}}},
 {"fields_integer",{s.fields.vehicle58,s.fields.vehicle3C,s.fields.vehicle834,s.fields.vehicle844,s.fields.vehicleCFC,s.fields.vehicleD00,U(s.fields.applicationByte58),U(s.fields.chariot191),U(s.fields.chariot1B0),U(s.fields.left191),U(s.fields.right191)}},{"fields_float",old::scalarWords({s.fields.objectD34_10,s.fields.damage7D0,s.fields.damage7D8,s.fields.damage7D4,s.fields.damage7DC,s.fields.gravity378,s.fields.gravity37C})},{"phase",tracePhase49W(s.phase)},{"body_phase",U(s.bodies.phase)},{"main_phase",U(m.phase)},{"secondary_phase",U(z.phase)}};}
static FreshPodGeometryResult49O geometry(f::Fixture&x){FreshPodGeometrySources49O s{};s.chariot=x.assets.hierarchy[2];s.rightEngine=x.assets.hierarchy[1];s.forward=x.assets.motions[4];s.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O d{};f::require(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),s.chariotBindings,d),"connected original motion binding");FreshPodGeometryWorkspace49O w{};FreshPodGeometryResult49O r{};FreshPodGeometryDiagnostic49O e{};f::require(buildFreshPodGeometry49O(s,w,r,e),"connected authored geometry");return r;}
static frame49w_authored::Result initialize(f::Fixture&x){
 ControlPresets49D presets{};initializeControlPresets49D(presets);f::require(selectControlPreset49D(presets,0,4),"supplied preset4 selection");SelectedControlPreset49D selected{};f::require(resolveControlPreset49D(presets,0,0,selected),"separate action/config indices0");x.actionCodes=selected.actions;x.configurationEntryNonzero=selected.configurationEntryNonzero;
 auto& s=x.state;auto&m=s.bodies.main;auto&z=s.bodies.secondary;
 rr::assets::PsgNode49O right{};f::require(rr::assets::readPsgNode49O(x.assets.hierarchy[1],0,right),"actual right nullcg");
 constexpr float ax=2903.56103515625f,ay=1292.14404296875f,az=7649.8837890625f;
 // One supplied initial placement only; every subsequent pose is native output.
 s.nodes[50].localC0=s.nodes[50].world100=c::v::matrix(0,ax,ay,float(double(az)+double(right.local[14])));
 s.nodes[49].localC0=s.nodes[49].world100=c::v::matrix(0,ax,ay,az);
 f::require(setLocalMatrix49P({s.nodes,53},t::ix(30),s.nodes[49].localC0),"one supplied initial displayed pose");
 f::project(s,x.bindings);
 for(auto* r:{&m.components.root,&z.root}){f::require(matrixToQuaternion48G(r->scene,r->body.orientation),"supplied coherent initial orientation");r->body.linearAcceleration={};r->body.angularAcceleration={};r->body.angularVelocity={};r->worldAngularVelocityA0={};r->body.previousPositionDerivative=r->body.velocity;}
 s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
 // Unrecovered damping/center/limit inputs stay declared diagnostic values,
 // not purported authoring defaults. This damping is the retained49O fixture.
 x.parameters.bodies.main.aero.velocityDamping130={{.001f,.002f,.001f}};
 const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,selected.configurationEntryNonzero,false,false,{}};
 const auto bytes=c::e::readBytes("evidence/authoredInitialization49N/authoring-source/Pod01.phy");
 const auto result=frame49w_authored::bind(bytes,geometry(x),selection,x.parameters,s,x.selectors,x.secondaryInputs);
 // Declared diagnostic initial placement: put the physical secondary at the
 // authored target relative to the supplied identity-oriented main. This also
 // satisfies the authored3-unit anchor/rest relation; it is not race spawn.
 auto secondaryPose=s.nodes[50].localC0;
 for(U axis=0;axis<3;++axis){volatile float v=secondaryPose.v[12+axis]+x.secondaryInputs.targetRelative2F0.v[axis];secondaryPose.v[12+axis]=v;}
 s.nodes[49].localC0=s.nodes[49].world100=secondaryPose;
 f::require(setLocalMatrix49P({s.nodes,53},t::ix(30),secondaryPose),"supplied authored-relative initial pose");
 f::project(s,x.bindings);
 return result;
}
static DualShockAxisBytes48X packet(unsigned frame){if(frame<32)return {128,80,128,48};if(frame<48)return {136,128,120,128};return {128,255,128,255};}
static J meshRow(unsigned frame,double elapsed,const char*label,const Bytes&mesh,const Bytes&full,const Bytes&base,const old::PalettePair&palettes,const std::string&path){return {
 {"frame",frame},{"elapsed_seconds",elapsed},{"label",label},{"pose_source",std::string(label)=="full-motion"?"source-ordered-native-graphics-and-body-frame":"counterfactual-display-setters"},
 {"mesh_path",path},{"mesh_fnv64",old::meshDigest(mesh)},{"vertices",5755},{"indices",10713},{"component_vertices",{{"chariot",2884},{"left_engine",1411},{"right_engine",1460}}},
 {"changed_vertices_vs49H",old::changed(base,mesh)},{"changed_vertices_vs_full",old::changed(full,mesh)},
 {"palettes",{old::palette(palettes[0]),old::palette(palettes[1]),old::palette(palettes[2])}}};}
static int run(int argc,char**argv){
 f::require(argc<=3,"usage export_vehicle_frame49w [output-directory [--no-support]|--probe|--probe-no-support|--verify-no-support]");f::require(std::fesetround(FE_TONEAREST)==0,"nearest");
 const std::string option=argc>1?argv[1]:"";
 const bool verifyNoSupport=option=="--verify-no-support";
 const bool probe=option=="--probe"||option=="--probe-no-support";
 const bool supportEnabled=!(verifyNoSupport||option=="--probe-no-support"||(argc==3&&std::string(argv[2])=="--no-support"));
 f::require(argc<3||std::string(argv[2])=="--no-support","unknown scenario argument");
 old::verifyOnly=argc==1||verifyNoSupport;
 const fs::path output=old::verifyOnly?(fs::path("evidence/frameBridge49W/connected")/(supportEnabled?"":"no-support")):fs::path(argv[1]);if(!probe&&!old::verifyOnly)fs::create_directories(output);
 auto fixture=std::make_unique<f::Fixture>(120);auto&x=*fixture;const auto authored=initialize(x);old::MeshInputs meshes(x.assets);
 FrameTimingState49I clock{1000,1000,0,0,7,0,0};U timestamp=1000;double elapsed=0;
 J initial=state(x.state);initial["authoring"]={{"selected_class",U(authored.binding.selectedClass)},{"profile_overwrite",false},{"preset",4},{"action_index",0},{"configuration_index",0},{"configuration_nonzero",x.configurationEntryNonzero},{"source_parameter_instructions",authored.parameterInstructions},{"source_geometry_instructions",authored.geometryInstructions},{"source_joint_instructions",authored.jointInstructions},{"compared_words",authored.comparedWords},{"main_mass_bits",bits(x.parameters.bodies.main.body.mass)},{"secondary_mass_bits",bits(x.parameters.bodies.secondary.body.mass)},{"main_engine_parameters",raw(x.parameters.bodies.main.engine)},{"main_aero_parameters",raw(x.parameters.bodies.main.aero)},{"secondary_target",raw(x.secondaryInputs.targetRelative2F0)},{"remaining_supplied_parameters","Main/secondary centers, limits/axis flags outside49O writes, disabled probe inputs and unproved aero130 are explicit retained diagnostic inputs. No liveprofile or race-grid claim."}};
 initial["scope"]="Already-initialized ordinary vehicle; actual53-node graph and type0POD01 shape. Named preserving player/base/other-child cuts, a separately declared scene-preserving contact-force fixture at354 and supplied no-hit world query. Initial bodies/pose/history are supplied once; later state always comes from preceding native result. No full original frame or authentic race/spawn/contact claim.";
 initial["contact_control"]=supportEnabled?"Explicit synthetic354 counter-gravity support forces; this is a supplied fixture, not authentic terrain contact generation.":"No support loads are added. The same no-hit query assumption permits falling through authored track geometry; this control deliberately demonstrates the missing terrain-contact boundary.";
 initial["scope"]=initial["scope"].get<std::string>()+" "+initial["contact_control"].get<std::string>();
 initial["palette_projection"]="After each completed native frame, original/native49S palettes and rigid vertices are evaluated on a disposable graph snapshot. This observation does not change canonical next-frame caches, and it never replaces graphics roots with postphysics body poses.";
 J rows=J::array(),frames=J::array(),transitions=J::array();std::ostringstream goldens;std::array<BodyMatrix48G,3> frozenRoots{};
 const std::set<unsigned> selected{0,31,95,143};std::uint64_t positionChanges=0,orientationChanges=0;U previousP=x.state.bodies.vehicle838,previousV=x.state.lifecycle.vehicle83C;
 for(unsigned frame=0;frame<frameCount;++frame){
  const J before=state(x.state);timestamp+=(frame%3?17:16);const float dt=c::preparedFactor(clock,timestamp,0);elapsed+=double(dt);const auto bytes=packet(frame);
  x.beginFrame(bytes,dt,60);const J graphics=state(x.state);
  J suppliedContacts=J::array();
  const f::Fixture::ContactModifier support=[&](ContactEffects49W& effects,unsigned frameNumber,unsigned step){
   // Explicit synthetic354 test inputs, not a recovered terrain response.
   // Counter-gravity force keeps the numeric body advance in the inspection
   // view without writing any position, orientation, velocity or history.
   volatile float mainSupport=supportEnabled?x.parameters.bodies.main.body.mass*x.state.bodies.main.selectedGravity:0.0f;
   if(supportEnabled){volatile float fy=effects.main.loads.force.v[1]+mainSupport;effects.main.loads.force.v[1]=fy;}
   float secondarySupport=0;
   if(supportEnabled&&x.state.bodies.vehicle838){volatile float value=x.parameters.bodies.secondary.body.mass*x.state.fields.gravity37C;secondarySupport=value;volatile float sy=effects.secondary.loads.force.v[1]+secondarySupport;effects.secondary.loads.force.v[1]=sy;}
   suppliedContacts.push_back({{"frame",frameNumber},{"substep",step},{"main_support_y_bits",bits(mainSupport)},{"secondary_support_y_bits",bits(secondarySupport)},{"physical838",x.state.bodies.vehicle838}});
  };
  do{x.advanceSubstep(support);}while(x.state.phase!=VehicleFramePhase49W::Ready);++x.frame;++f::frames;
  const J after=state(x.state);f::require(after["main"]["root_words"]!=before["main"]["root_words"],"actual main body advanced");
  if(std::memcmp(&x.state.nodes[50].world100.v[12],&x.state.nodes[50].localC0.v[12],12))throw std::runtime_error("main terminal world projection");
  bool moved=false;for(U i=12;i<15;++i)moved=moved||before["graph"][50]["world_bits"][i]!=after["graph"][50]["world_bits"][i];positionChanges+=moved;
  orientationChanges+=before["main"]["orientation1F0"]!=after["main"]["orientation1F0"];
  for(U i:{0u,15u,30u})f::require(graphics["graph"][i]["local_bits"]==after["graph"][i]["local_bits"],"graphics roots must retain prephysics phase");
  if(previousP!=x.state.bodies.vehicle838||previousV!=x.state.lifecycle.vehicle83C)transitions.push_back({{"frame",frame},{"before",{previousP,previousV}},{"after",{x.state.bodies.vehicle838,x.state.lifecycle.vehicle83C}}});previousP=x.state.bodies.vehicle838;previousV=x.state.lifecycle.vehicle83C;
  auto observed=graphCopy(x.state);const auto canonical=after;const auto pal=old::palettes(observed,meshes.ids);auto mesh=meshes.deform(pal);f::require(state(x.state)==canonical,"diagnostic palette observation changed canonical frame");
  if(frame==0){for(U i=0;i<3;++i)frozenRoots[i]=x.state.nodes[std::array<U,3>{30,0,15}[i]].localC0;}
  J row={{"frame",frame},{"elapsed_seconds",elapsed},{"packet_bytes",{bytes.packet4,bytes.packet5,bytes.packet6,bytes.packet7}},{"before",before},{"supplied354_support_forces",suppliedContacts},{"after_graphics_before_physics",graphics},{"after",after},{"palettes",{old::palette(pal[0]),old::palette(pal[1]),old::palette(pal[2])}},{"mesh_fnv64",old::meshDigest(mesh)}};frames.push_back(row);
  goldens<<frame<<' '<<old::meshDigest(mesh)<<' '<<c::graphHash(graphCopy(x.state))<<' '<<c::noiseHash(x.state.noise)<<'\n';
  if(probe&&((frame%8)==7||frame==0))std::cout<<J{{"frame",frame},{"position",{x.state.nodes[50].world100.v[12],x.state.nodes[50].world100.v[13],x.state.nodes[50].world100.v[14]}},{"cached_speed",x.state.bodies.main.cachedSpeed34},{"modes",{x.state.bodies.vehicle838,x.state.lifecycle.vehicle83C}}}.dump()<<'\n';
  if(selected.count(frame)&&!probe){const auto name="frame"+std::to_string(frame)+"-full-motion.rrmesh";old::write(output/name,mesh);rows.push_back(meshRow(frame,elapsed,"full-motion",mesh,mesh,meshes.base,pal,name));}
  if(frame==31&&!probe&&supportEnabled){for(unsigned variant=0;variant<2;++variant){auto control=graphCopy(x.state);const char*label=variant?"frozen-roots":"physical-copy-only";if(variant){for(U i=0;i<3;++i)f::require(setLocalMatrix49P({control.data(),53},t::ix(std::array<U,3>{30,0,15}[i]),frozenRoots[i]),"frozen root display control");}else f::require(setLocalMatrix49P({control.data(),53},t::ix(30),x.state.nodes[49].localC0),"postphysics physical-copy display control");const auto cp=old::palettes(control,meshes.ids);auto cm=meshes.deform(cp);const auto name="frame31-"+std::string(label)+".rrmesh";old::write(output/name,cm);rows.push_back(meshRow(frame,elapsed,label,cm,mesh,meshes.base,cp,name));}}
 }
 J summary={{"passed",true},{"frames",frameCount},{"substeps",f::substeps},{"native_calls",f::nativeCalls},{"checks",f::checks},{"graphics_original_comparisons",f::graphicsComparisons},{"body_cache_original_comparisons",f::cacheComparisons},{"tail_original_comparisons",f::tailComparisons},{"body_original_comparisons",f::bodyComparisons},{"main_force_original_comparisons",f::forceComparisons},{"secondary_original_comparisons",f::secondaryComparisons},{"input_original_comparisons",f::inputComparisons},{"shape_original_comparisons",f::shapeComparisons},{"native_hot_allocations",0},{"position_changed_frames",positionChanges},{"orientation_state_changed_frames",orientationChanges},{"native_vertices_all_frames",frameCount*5755},{"graph_nodes",53},{"palette_entries",46},{"mode_transitions",transitions},{"native_postphysics_graphics_pose_copies",0}};
 if(probe){std::cout<<summary.dump()<<'\n';return 0;}
 J trace={{"schema","advancing49w.trace.v1"},{"scenario",supportEnabled?"synthetic-support":"no-support"},{"initial",initial},{"frames",frames},{"summary",summary}};
 J report={{"schema","advancing49w.export.v1"},{"scenario",supportEnabled?"synthetic-support":"no-support"},{"passed",true},{"frames",rows},{"summary",summary},{"limits",initial["scope"]},{"palette_projection",initial["palette_projection"]}};
 old::write(output/"trace.json",trace.dump(2)+'\n');old::write(output/"export-report.native.json",report.dump(2)+'\n');old::write(output/"exact-goldens49w.txt",goldens.str());std::cout<<summary.dump()<<'\n';return 0;
}
}
int main(int argc,char**argv){try{return advancing49w::run(argc,argv);}catch(const std::exception&e){std::cerr<<"FAIL advancing49W: "<<e.what()<<'\n';return 1;}}
