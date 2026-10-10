// Standalone production-only49V hot path; adapted from unchanged49U methodology. Fixture I/O and oracle comparisons
// happen before timing. No source interpreter, test hooks, or JSON is linked.
#include "game/vehicle_animation49t.h"
#include "game/chariot_lifecycle49v.h"
#include "game/chariot_scene49u.h"
#include "assets/chariot_root_authoring49u.h"
#include "game/model_palette49s.h"
#include "game/rigid_vertices49s.h"
#include <array>
#include <cfenv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>
using namespace rr::game;
namespace a = rr::assets;
using U = std::uint32_t;
using H = std::uint64_t;
using I = SceneNodeIndex49P;
using Graph = std::array<SceneNode49P,53>;
static constexpr H basis=1469598103934665603ull;
#ifdef PROBE49V
static H allocations=0;
static bool countAllocations=false;
void* operator new(std::size_t n) {
    if(countAllocations) ++allocations;
    if(void* p=std::malloc(n?n:1)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p,std::size_t) noexcept { std::free(p); }
void operator delete[](void* p,std::size_t) noexcept { std::free(p); }
#endif
static void need(bool ok,const char* what) {
    if(!ok) { std::fprintf(stderr,"FAIL %s\n",what);std::exit(2); }
}
static U bits(float x) { U u;std::memcpy(&u,&x,4);return u; }
static void hash(H& h,U u) { for(U i=0;i<4;++i) { h^=(u>>(8*i))&255u;h*=1099511628211ull; } }
static void floatsHash(H& h,const float* p,U n) { for(U i=0;i<n;++i) hash(h,bits(p[i])); }
static void lazyHash(H& h,const VisualLazyWord49R& v) { hash(h,v.guard);hash(h,v.word); }
static H graphHash(const Graph& g) {
    H h=basis;
    for(const auto& n:g) {
        floatsHash(h,n.localC0.v,16);floatsHash(h,n.world100.v,16);
        hash(h,n.sceneFlags190);hash(h,U(n.parent180));hash(h,U(n.firstChild184));hash(h,U(n.nextSibling188));
    }
    return h;
}
struct Reader {
    std::FILE* f;
    explicit Reader(const char* path):f(std::fopen(path,"rb")) { need(f!=nullptr,"fixture open"); }
    void bytes(void* p,std::size_t n) { need(std::fread(p,1,n,f)==n,"fixture read"); }
    U u() { U x;bytes(&x,4);return x; }
    H h() { H x;bytes(&x,8);return x; }
    float value() { float x;bytes(&x,4);return x; }
    void fs(float* p,U n) { bytes(p,n*4); }
    void lazy(VisualLazyWord49R& x) { x.guard=std::uint8_t(u());x.word=u(); }
    void graph(Graph& g) {
        for(auto& n:g) { fs(n.localC0.v,16);fs(n.world100.v,16);n.sceneFlags190=std::uint8_t(u());
            n.parent180=I(u());n.firstChild184=I(u());n.nextSibling188=I(u()); }
    }
    void close() { need(std::fgetc(f)==EOF,"fixture trailing bytes");need(std::fclose(f)==0,"fixture close"); }
};
struct Live {
    Graph nodes{};
    VehicleAnimationState49T animation{};
    VehicleAnimationShared49T animationShared{};
    VehicleVisualState49R visual{};
    VisualShared49R shared{};
    CoupledNoiseContext49M noise{};
    ChariotLifecycleState49V root{};
    ChariotRootShared49U rootShared{};
    CoupledBodyState49M body{}; // Canonical838 and secondary864, never replayed per frame.
    SecondaryActivationSupplement49V supplement{};

};
static void loadSecondary(Reader& r,SecondaryBodyState49M& s,SecondaryActivationSupplement49V& x) {
    r.fs(s.root.body.position.v,3);
    r.fs(s.root.body.velocity.v,3);
    r.fs(s.root.localVelocity80.v,3);
    r.fs(s.root.previousVelocity90.v,3);
    r.fs(s.root.worldAngularVelocityA0.v,3);
    r.fs(s.root.body.angularVelocity.v,3);
    r.fs(s.root.body.linearAcceleration.v,3);
    r.fs(s.root.body.angularAcceleration.v,3);
    r.fs(s.root.loads.force.v,3);
    r.fs(s.root.loads.torque.v,3);
    r.fs(s.root.positionCorrection1a0.v,3);
    r.fs(s.root.rotationCorrection1b0.v,3);
    r.fs(s.root.accumulator1c0.v,3);
    r.fs(s.root.accumulator1d0.v,3);
    r.fs(s.root.body.orientation.v,4);
    r.fs(s.root.body.previousPositionDerivative.v,3);
    r.fs(s.root.body.previousLinearAcceleration.v,3);
    r.fs(s.root.body.previousAngularAcceleration.v,3);
    r.fs(s.root.body.orientationDerivative.v,4);
    r.fs(s.root.body.previousOrientationDerivative.v,4);
    s.root.callbackState238=r.u();s.cachedSpeed34=r.value();r.fs(s.cachedAnglesE0.v,3);
    s.body20C=r.u();s.velocity70W=r.value();s.angularVelocityB0W=r.value();s.force180W=r.value();s.torque190W=r.value();
    x.localVelocity80W=r.value();x.previousVelocity90W=r.value();x.linearAccelerationC0W=r.value();x.angularAccelerationD0W=r.value();
    r.fs(x.positionDerivative2B8.v,3);r.fs(x.linearAcceleration2D0.v,3);r.fs(x.angularAcceleration308.v,3);
    r.fs(s.root.scene.v,16);s.sceneFlags190=r.u();s.time864=r.value();s.phase=SecondaryPhase49M(r.u());s.scenePresent=r.u()!=0;s.enabled=r.u()!=0;
}
static void secondaryHash(H& h,const SecondaryBodyState49M& s,const SecondaryActivationSupplement49V& x) {
    floatsHash(h,s.root.body.position.v,3);
    floatsHash(h,s.root.body.velocity.v,3);
    floatsHash(h,s.root.localVelocity80.v,3);
    floatsHash(h,s.root.previousVelocity90.v,3);
    floatsHash(h,s.root.worldAngularVelocityA0.v,3);
    floatsHash(h,s.root.body.angularVelocity.v,3);
    floatsHash(h,s.root.body.linearAcceleration.v,3);
    floatsHash(h,s.root.body.angularAcceleration.v,3);
    floatsHash(h,s.root.loads.force.v,3);
    floatsHash(h,s.root.loads.torque.v,3);
    floatsHash(h,s.root.positionCorrection1a0.v,3);
    floatsHash(h,s.root.rotationCorrection1b0.v,3);
    floatsHash(h,s.root.accumulator1c0.v,3);
    floatsHash(h,s.root.accumulator1d0.v,3);
    floatsHash(h,s.root.body.orientation.v,4);
    floatsHash(h,s.root.body.previousPositionDerivative.v,3);
    floatsHash(h,s.root.body.previousLinearAcceleration.v,3);
    floatsHash(h,s.root.body.previousAngularAcceleration.v,3);
    floatsHash(h,s.root.body.orientationDerivative.v,4);
    floatsHash(h,s.root.body.previousOrientationDerivative.v,4);
    hash(h,s.root.callbackState238);hash(h,bits(s.cachedSpeed34));floatsHash(h,s.cachedAnglesE0.v,3);
    hash(h,s.body20C);hash(h,bits(s.velocity70W));hash(h,bits(s.angularVelocityB0W));hash(h,bits(s.force180W));hash(h,bits(s.torque190W));
    hash(h,bits(x.localVelocity80W));hash(h,bits(x.previousVelocity90W));hash(h,bits(x.linearAccelerationC0W));hash(h,bits(x.angularAccelerationD0W));
    floatsHash(h,x.positionDerivative2B8.v,3);floatsHash(h,x.linearAcceleration2D0.v,3);floatsHash(h,x.angularAcceleration308.v,3);
    floatsHash(h,s.root.scene.v,16);hash(h,s.sceneFlags190);hash(h,bits(s.time864));hash(h,U(s.phase));hash(h,s.scenePresent);hash(h,s.enabled);
}
static void loadLive(Reader& r,Live& l) {
    r.graph(l.nodes);
    static_assert(sizeof(VehicleAnimationState49T)==19*4,"animation float/model layout");
    r.bytes(&l.animation,sizeof l.animation);
    auto& s=l.animationShared;
    s.enableGuard3D60F4=std::uint8_t(r.u());s.enable434748=r.u();
    s.motion.interpolationValue4332F0=r.u();s.motion.pairValue433478=r.u();
    s.motion.interpolationGuard3D5D8C=std::uint8_t(r.u());s.motion.pairGuard3D5DC4=std::uint8_t(r.u());
    r.lazy(s.ik.sentinel443450);r.lazy(s.ik.sentinel443458);
    s.libm.mode=std::int32_t(r.u());s.libm.errnoValue=std::int32_t(r.u());
    s.libm.defaultMatherr=r.u()!=0;s.libm.coldNanRecord=r.u()!=0;
    static_assert(sizeof(VehicleVisualState49R)==44*4,"visual float layout");
    r.bytes(&l.visual,sizeof l.visual);
    for(auto& v:l.shared.globals) r.lazy(v);
    r.lazy(l.shared.orientationSentinel4433D8);r.fs(l.shared.zero434630.v,4);
    for(auto& p:l.noise.noise.permutation) p=r.u();
    r.fs(l.noise.noise.gradient1D,256);l.noise.noise.scratchCell=std::int32_t(r.u());
    l.noise.noise.initialized=r.u()!=0;l.noise.sharedRandomState=r.u();
    static_assert(sizeof(ChariotLifecycleState49V)==21*4,"lifecycle state layout");
    l.body.vehicle838=r.u();r.bytes(&l.root,sizeof l.root);r.lazy(l.rootShared.speed434750);r.lazy(l.rootShared.yaw434758);
    r.fs(l.rootShared.up434650.v,4);loadSecondary(r,l.body.secondary,l.supplement);
}
static H liveHash(const Live& l) {
    H h=basis;U words[19];std::memcpy(words,&l.animation,sizeof words);for(U u:words) hash(h,u);
    const auto& s=l.animationShared;
    hash(h,s.enableGuard3D60F4);hash(h,s.enable434748);hash(h,s.motion.interpolationValue4332F0);hash(h,s.motion.pairValue433478);
    hash(h,s.motion.interpolationGuard3D5D8C);hash(h,s.motion.pairGuard3D5DC4);
    lazyHash(h,s.ik.sentinel443450);lazyHash(h,s.ik.sentinel443458);
    hash(h,U(s.libm.mode));hash(h,U(s.libm.errnoValue));hash(h,s.libm.defaultMatherr);hash(h,s.libm.coldNanRecord);
    U visualWords[44];std::memcpy(visualWords,&l.visual,sizeof visualWords);for(U u:visualWords) hash(h,u);
    for(const auto& v:l.shared.globals) lazyHash(h,v);
    lazyHash(h,l.shared.orientationSentinel4433D8);floatsHash(h,l.shared.zero434630.v,4);
    for(U p:l.noise.noise.permutation) hash(h,p);
    floatsHash(h,l.noise.noise.gradient1D,256);hash(h,U(l.noise.noise.scratchCell));
    hash(h,l.noise.noise.initialized);hash(h,l.noise.sharedRandomState);
    hash(h,l.body.vehicle838);U rootWords[21];std::memcpy(rootWords,&l.root,sizeof rootWords);for(U u:rootWords) hash(h,u);
    lazyHash(h,l.rootShared.speed434750);lazyHash(h,l.rootShared.yaw434758);
    floatsHash(h,l.rootShared.up434650.v,4);secondaryHash(h,l.body.secondary,l.supplement);return h;
}
static H noiseHash(const CoupledNoiseContext49M& n) {
    H h=basis;hash(h,n.noise.initialized);hash(h,n.sharedRandomState);hash(h,U(n.noise.scratchCell));
    for(U i=0;i<256;++i) {hash(h,n.noise.permutation[i]);hash(h,bits(n.noise.gradient1D[i]));}return h;
}
static std::vector<std::uint8_t> file(const std::string& path) {
    auto* f=std::fopen(path.c_str(),"rb");need(f!=nullptr,"asset open");
    need(std::fseek(f,0,SEEK_END)==0,"asset seek");long n=std::ftell(f);need(n>=0,"asset size");
    std::rewind(f);std::vector<std::uint8_t> b(static_cast<std::size_t>(n));
    need(std::fread(b.data(),1,b.size(),f)==b.size(),"asset read");need(std::fclose(f)==0,"asset close");return b;
}
struct Assets {
    std::array<std::vector<std::uint8_t>,6> mot;
    a::MotionNodeBindings49O rows[3]{};
    a::MotionView49O motions[6]{};
    std::size_t retainedBytes=0;
    a::ChariotRootAuthoredValues49U rootValues{};
    FreshPodGeometryResult49O geometry{};
    explicit Assets(const std::string& root) {
        const auto dir=root+"/evidence/authoredGeometry49O/asset-reader/fixtures/";
        const char* ps[]={"48-englt1.psg","43-engrt1.psg","52-chariot.psg"};
        const char* vs[]={"49-englt1.vut","44-engrt1.vut","53-chariot.vut"};
        const char* ms[]={"50-englt1.mot","45-engrt1.mot","54-01_brklt.mot","55-01_brkrt.mot","56-01_forwd.mot","57-01_back.mot"};
        const U offsets[]={1,16,31};a::GeometryDiagnostic49O gd{};a::MotionBindingDiagnostic49O bd{};
        for(U i=0;i<3;++i) {
            auto psg=file(dir+ps[i]),vut=file(dir+vs[i]);a::PsgHierarchyView49O hierarchy{};
            need(a::readPsgHierarchy49O(psg.data(),psg.size(),hierarchy,gd),"PSG reader");
            need(a::readMotionNodeBindings49O(hierarchy,reinterpret_cast<const char*>(vut.data()),vut.size(),rows[i],bd),"VUT reader");
            for(U r=0;r<rows[i].count;++r) rows[i].rows[r].sceneNode=std::uint8_t(rows[i].rows[r].sceneNode+offsets[i]);
        }
        for(U i=0;i<6;++i) { mot[i]=file(dir+ms[i]);retainedBytes+=mot[i].size();need(a::readMot49O(mot[i].data(),mot[i].size(),motions[i],gd),"MOT reader"); }
        auto chariot=file(dir+ps[2]),right=file(dir+ps[1]),vut=file(dir+vs[2]);
        FreshPodGeometrySources49O source{};
        need(a::readPsgHierarchy49O(chariot.data(),chariot.size(),source.chariot,gd),"fresh Chariot PSG");
        need(a::readPsgHierarchy49O(right.data(),right.size(),source.rightEngine,gd),"fresh right PSG");
        need(a::readMotionNodeBindings49O(source.chariot,reinterpret_cast<const char*>(vut.data()),vut.size(),source.chariotBindings,bd),"fresh binding");
        source.forward=motions[4];source.back=motions[5];FreshPodGeometryWorkspace49O work{};FreshPodGeometryDiagnostic49O diagnostic{};
        need(buildFreshPodGeometry49O(source,work,geometry,diagnostic),"actual fresh geometry");
        auto phy=file(root+"/evidence/authoredInitialization49N/authoring-source/Pod01.phy");
        a::ChariotRootAuthoringDiagnostic49U authoredDiagnostic{};
        need(a::readChariotRootAuthoring49U(reinterpret_cast<const char*>(phy.data()),phy.size(),rootValues,authoredDiagnostic),"actual PHY reader");
    }
};
struct Frame {
    VehicleAnimationInput49T animation{};
    VehicleVisualInput49R visual{};
    ChariotLifecycleInput49V root{};
    U vehicle40=0,probe10C=0;float cachedSpeed=0,angularY=0;Vector3_48C localVelocity{};VisualVector49R probe{};
    H afterAnimation,afterVisual,afterRoot,afterPalette,records,palettes,positions,noiseAfterVisual,afterGetter,recordsAfterGetter,secondaryBefore;
    U hasGetter=0,entryPhysical=0,entryVirtual=0,reversed=0;VisualVector49R getterExpected{};

};
static void loadFrame(Reader& r,Frame& f) {
    auto& in=f.animation;in.phase=VehicleAnimationPhase49T(r.u());
    in.animationDelta23C=r.value();in.aeroE4=r.value();in.aeroE8=r.value();
    in.chariot191=std::uint8_t(r.u());in.chariot1B0=std::uint8_t(r.u());in.left191=std::uint8_t(r.u());in.right191=std::uint8_t(r.u());
    in.body340Present=r.u()!=0;in.midRecordsAbsent=r.u()!=0;r.fs(in.bodyAccelerationC0.v,4);
    auto& v=f.visual;v.phase=VehicleVisualPhase49R(r.u());v.vehicle834=r.u();v.vehicle844=r.u();v.vehicle40=r.u();v.applicationByte58=std::uint8_t(r.u());
    v.probe108=r.u();v.aeroD8=r.u();v.aeroDC=r.u();v.vehicleCFC=r.u();v.vehicleD00=r.u();
    float* values[]={&v.frameFactor,&v.cachedBodySpeed34,&v.rawB5C,&v.objectD34_10,&v.engine68,&v.engine6C,&v.engine70,&v.engine74,&v.aeroE4,&v.aeroE8,&v.damage7D0,&v.damage7D8,&v.damage7D4,&v.damage7DC};
    for(float* x:values) *x=r.value();
    r.fs(v.probeVector1D0.v,4);r.fs(v.impactBA0.v,4);
    auto& u=f.root;u.phase=ChariotRootPhase49U(r.u());
    f.vehicle40=r.u();u.vehicle58=r.u();u.aeroD8=r.u();u.vehicleCFC=r.u();u.vehicleD00=r.u();f.probe10C=r.u();
    u.frameFactor=r.value();f.cachedSpeed=r.value();f.angularY=r.value();u.objectD34_10=r.value();
    r.fs(f.localVelocity.v,3);r.fs(u.impactBA0.v,4);r.fs(f.probe.v,4);
    f.afterAnimation=r.h();f.afterVisual=r.h();f.afterRoot=r.h();f.afterPalette=r.h();f.records=r.h();f.palettes=r.h();f.positions=r.h();f.noiseAfterVisual=r.h();
    f.afterGetter=r.h();f.recordsAfterGetter=r.h();f.secondaryBefore=r.h();
    f.hasGetter=r.u();f.entryPhysical=r.u();f.entryVirtual=r.u();f.reversed=r.u();r.fs(f.getterExpected.v,4);

}
struct Context {
    VehicleAnimationBindings49T bindings{};
    ChariotIkPair49T records{};
    VehicleVisualParameters49R parameters{};
    ChariotRootParameters49U rootParameters{};
    ChariotRootBindings49U rootBindings{I(50),I(51),I(52),I(49),I(30),I(0),I(15),I(35),I(36)};
    ChariotLifecycleWorkspace49V rootWork{};
    ChariotLifecycleDiagnostic49V rootDiagnostic{};
    SecondaryActivationGlobals49V activationGlobals{};
    VisualVector49R getterPosition{};
    detail::SceneVectorContext49U getterContext{};
    VisualSceneBindings49R visualBindings{I(50),I(51),I(15),I(0),false,false,0};
    VehicleAnimationWorkspace49T animationWork{};
    VehicleAnimationDiagnostic49T animationDiagnostic{};
    VehicleVisualWorkspace49R visualWork{};
    VehicleVisualDiagnostic49R visualDiagnostic{};
    ModelPalette49S palettes[3]{};
    ModelPaletteWorkspace49S paletteWork{};
    ModelPaletteDiagnostic49S paletteDiagnostic{};
    ModelPaletteInput49S paletteInputs[3]{};
    I source[46]{};
    std::uint8_t ids[46]{};
    RigidVertex49S vertices[5755]{};
    RigidPosition49S positions[5755]{};
    RigidVertexDiagnostic49S rigidDiagnostic{};
    explicit Context(const Assets& a) {
        initializedSecondaryActivationGlobals49V(activationGlobals);
        auto& b=bindings;b.chariot1B4=I(30);b.left1BC=I(0);b.right1B8=I(15);b.bodyScene288=I(50);
        b.chariotRows=&a.rows[2];b.leftRows=&a.rows[0];b.rightRows=&a.rows[1];
        b.brakeLeft28C=&a.motions[2];b.brakeRight290=&a.motions[3];b.forward280=&a.motions[4];b.back284=&a.motions[5];b.left2A0=&a.motions[0];b.right29C=&a.motions[1];
        b.hands={{I(41),I(43),I(44),I(47)},{I(42),I(45),I(46),I(48)}};b.handRecords=&records;
        const U counts[]={18,14,14},roots[]={30,0,15},first[]={31,1,16},starts[]={0,18,32};
        for(U side=0;side<3;++side) {
            for(U j=0;j<counts[side];++j) {source[starts[side]+j]=I(first[side]+j);ids[starts[side]+j]=std::uint8_t(j);}
            paletteInputs[side]={ModelPalettePhase49S::selectedRenderPreparation,ModelPaletteSceneState49S::completeCurrentScene,1,I(roots[side]),source+starts[side],counts[side],ids+starts[side],counts[side]};
        }
    }
    bool animation(Live& l,const Frame& f) {
        return advanceVehicleAnimation49T(f.animation,bindings,{l.nodes.data(),53},l.animation,l.animationShared,l.shared,animationWork,animationDiagnostic);
    }
    bool visual(Live& l,const Frame& f) {
        return updateVehicleVisual49R(f.visual,parameters,visualBindings,{l.nodes.data(),53},l.visual,l.shared,l.noise,visualWork,visualDiagnostic);
    }
    // These are supplied main/control/acceleration snapshots, never secondary rewrites.
    void prepareMain(Live& l,const Frame& f) {
        auto& m=l.body.main;m.vehicleState40=f.vehicle40;m.cachedSpeed34=f.cachedSpeed;
        m.components.probe348.field10C=f.probe10C;std::memcpy(m.components.probe348.vector1D0,f.probe.v,16);
        m.components.root.localVelocity80=f.localVelocity;m.components.root.body.velocity={{0,0,f.cachedSpeed}};
        m.components.root.body.angularVelocity.v[1]=f.angularY;
        std::memcpy(m.components.root.body.linearAcceleration.v,f.animation.bodyAccelerationC0.v,12);
    }
    void bindMainScene(Live& l) {
        l.body.main.components.root.scene=l.nodes[50].world100;
        l.body.mainSphere.sceneFlags190=(l.body.mainSphere.sceneFlags190&0xffffff00u)|l.nodes[50].sceneFlags190;
    }
    bool root(Live& l,const Frame& f) {
        return updateChariotLifecycle49V(f.root,rootParameters,rootBindings,{l.nodes.data(),53},l.body.main,l.body.mainSphere,l.body.secondary,l.supplement,activationGlobals,l.body.vehicle838,l.root,l.rootShared,l.shared.zero434630,l.noise,rootWork,rootDiagnostic);
    }
    // Declared diagnostic218C60->28B1A0 seam, strictly after the exact return
    // and palette/vertex consumers. No omitted cache/force/physics execution.
    bool getter(Live& l) {
        if(!l.body.vehicle838) return true;
        getterContext={0.0f};getterPosition={{3,4,5,6}};
        if(!detail::position49U({l.nodes.data(),53},rootBindings.physical19C,rootBindings.main190,getterContext,getterPosition)) return false;
        l.body.secondary.root.scene=l.nodes[49].world100;
        l.body.secondary.sceneFlags190=(l.body.secondary.sceneFlags190&0xffffff00u)|l.nodes[49].sceneFlags190;
        return true;
    }
    bool palette(Live& l) {
        for(U side=0;side<3;++side) if(!projectModelPalette49S({l.nodes.data(),53},paletteInputs[side],palettes[side],paletteWork,paletteDiagnostic)) return false;
        return true;
    }
    bool rigid() {
        const U starts[]={0,2884,4295},counts[]={2884,1411,1460};
        for(U side=0;side<3;++side) if(!projectRigidVertices49S({vertices+starts[side],counts[side]},{palettes[side].nodeWorld,palettes[side].entryCount},{positions+starts[side],counts[side]},rigidDiagnostic)) return false;
        return true;
    }
    H paletteHash() const {
        H h=basis;for(const auto& p:palettes) {floatsHash(h,p.rootWorld.v,16);for(U n=0;n<p.entryCount;++n) floatsHash(h,p.nodeWorld[n].v,16);hash(h,p.entryCount);}return h;
    }
    H positionsHash() const { H h=basis;for(const auto& p:positions) floatsHash(h,p.v,3);return h; }
};
static void sizes() {
#define SIZE(T) std::printf("%s %zu %zu\n",#T,sizeof(T),alignof(T))
    using SceneNodes53=SceneNode49P[53];using SceneNodes64=SceneNode49P[64];
    using MotionViews6=a::MotionView49O[6];using MotionBindings3=a::MotionNodeBindings49O[3];
    using Palettes3=ModelPalette49S[3];using PaletteInputs3=ModelPaletteInput49S[3];
    using PaletteNodes46=I[46];using PaletteIds46=std::uint8_t[46];
    using RigidVertices5755=RigidVertex49S[5755];using RigidPositions5755=RigidPosition49S[5755];
    SIZE(ChariotRootState49U);SIZE(ChariotRootShared49U);SIZE(ChariotRootInput49U);SIZE(ChariotRootParameters49U);
    using LaterGetterPosition49V=VisualVector49R;
    SIZE(LaterGetterPosition49V);SIZE(detail::SceneVectorContext49U);
    SIZE(ChariotLifecycleState49V);SIZE(ChariotLifecycleInput49V);SIZE(ChariotLifecycleWorkspace49V);SIZE(ChariotLifecycleDiagnostic49V);
    SIZE(SecondaryActivationSupplement49V);SIZE(SecondaryActivationGlobals49V);SIZE(CoupledBodyState49M);SIZE(MainBodyState49K);SIZE(MainSphereLanes49M);
    SIZE(ChariotRootBindings49U);SIZE(ChariotRootWorkspace49U);SIZE(ChariotRootDiagnostic49U);SIZE(SecondaryBodyState49M);
    SIZE(a::ChariotRootAuthoredValues49U);SIZE(a::ChariotRootAuthoredProvenance49U);SIZE(a::ChariotRootAuthoringDiagnostic49U);
    SIZE(FreshPodGeometryWorkspace49O);SIZE(FreshPodGeometryResult49O);
    SIZE(SceneNodes53);SIZE(SceneNode49P);SIZE(SceneNodes64);SIZE(SceneGraph49P);SIZE(VehicleAnimationState49T);SIZE(VehicleAnimationShared49T);
    SIZE(VehicleAnimationInput49T);SIZE(VehicleAnimationBindings49T);SIZE(VehicleAnimationWorkspace49T);SIZE(VehicleAnimationDiagnostic49T);
    SIZE(ChariotIkPair49T);SIZE(ChariotIkWorkspace49T);SIZE(ChariotIkDiagnostic49T);SIZE(ModelMotionWorkspace49T);SIZE(SourceLibmState49T);
    SIZE(VehicleVisualState49R);SIZE(VehicleVisualParameters49R);SIZE(VehicleVisualInput49R);SIZE(VisualSceneBindings49R);SIZE(VisualShared49R);SIZE(CoupledNoiseContext49M);
    SIZE(VehicleVisualWorkspace49R);SIZE(VehicleVisualDiagnostic49R);SIZE(MotionViews6);SIZE(MotionBindings3);
    SIZE(ModelPalette49S);SIZE(Palettes3);SIZE(PaletteInputs3);SIZE(PaletteNodes46);SIZE(PaletteIds46);SIZE(ModelPaletteWorkspace49S);SIZE(ModelPaletteDiagnostic49S);
    SIZE(RigidVertices5755);SIZE(RigidPositions5755);SIZE(RigidVertexDiagnostic49S);SIZE(Live);SIZE(Context);SIZE(Frame);
#undef SIZE
}
int main(int argc,char** argv) {
    if(argc==2&&!std::strcmp(argv[1],"--sizes")) { sizes();return 0; }
    need(argc==5,"usage fixture root histories mode");need(std::fegetround()==FE_TONEAREST,"rounding mode");
    const U endian=1;need(*reinterpret_cast<const unsigned char*>(&endian)==1,"little-endian fixture host");
    const U histories=U(std::strtoul(argv[3],nullptr,10));
    const bool onlyRoot=!std::strcmp(argv[4],"lifecycle");
    const bool onlyGetter=!std::strcmp(argv[4],"getter");
    const bool excludeGetter=!std::strcmp(argv[4],"animation_visual_lifecycle_palette");
    const bool doRigid=!std::strcmp(argv[4],"animation_visual_lifecycle_palette_rigid_getter");
    need(onlyRoot||onlyGetter||excludeGetter||doRigid||!std::strcmp(argv[4],"animation_visual_lifecycle_palette_getter"),"mode");
    Assets assets(argv[2]);Context c(assets);Live initial{};Frame frames[240]{};Graph constructor{};
    Reader r(argv[1]);char magic[8];r.bytes(magic,8);need(!std::memcmp(magic,"RR49VB01",8),"magic");need(r.u()==240,"frame count");
    r.graph(constructor);const H constructorExpected=r.h();loadLive(r,initial);
    initial.body.phase=CoupledBodyPhase49M::AwaitingPostBase;initial.body.main.phase=MainBodyPhase49K::AwaitingPostBase;
    initial.body.main.components.root.scene=initial.nodes[50].localC0;
    initial.body.main.components.root.worldAngularVelocityA0={{3,4,5}};
    initial.body.main.components.root.body.angularVelocity={{.03125f,0,-.0625f}};
    initial.body.mainSphere={0x1823a581u,-0.0f,.75f,8,9};
    static_assert(sizeof(ChariotIkPair49T)==40*4,"IK records layout");r.bytes(&c.records,sizeof c.records);
    static_assert(sizeof(VehicleVisualParameters49R)==35*4,"parameter layout");r.bytes(&c.parameters,sizeof c.parameters);
    static_assert(sizeof(ChariotRootParameters49U)==23*4,"root parameter layout");r.bytes(&c.rootParameters,sizeof c.rootParameters);
    const U expectedUnconsumedA90=r.u();ChariotRootParameters49U actualRoot{};a::ChariotRootAuthoringDiagnostic49U rd{};float a90=0;
    need(a::bindFreshChariotRootParameters49U(assets.rootValues,assets.geometry,c.parameters,actualRoot,rd,&a90),"actual fresh PHY binding");
    need(std::memcmp(&actualRoot,&c.rootParameters,sizeof actualRoot)==0&&bits(a90)==expectedUnconsumedA90,"connected actual PHY parameter parity");
    for(auto& v:c.vertices) {r.bytes(v.positionLocal,24);v.paletteIndex=std::uint16_t(r.u());}
    for(auto& f:frames) loadFrame(r,f);
    r.close();
    ChariotIkPair49T initialized{};SourceLibmState49T libm{};ChariotIkWorkspace49T initWork{};ChariotIkDiagnostic49T initDiagnostic{};
#ifdef PROBE49V
    countAllocations=true;
#endif
    const bool initializedOk=initializeChariotIk49T(c.bindings.hands,{constructor.data(),53},initialized,libm,initWork,initDiagnostic);
#ifdef PROBE49V
    countAllocations=false;need(allocations==0,"initializer allocated");
#endif
    need(initializedOk&&std::memcmp(&initialized,&c.records,sizeof initialized)==0,"constructor coefficient parity");
    need(graphHash(constructor)==constructorExpected&&libm.errnoValue==initial.animationShared.libm.errnoValue,"constructor graph/errno parity");
    ChariotRootState49U ctor{};ChariotRootShared49U ctorShared{};float ctorPhase=1;
    a::initializeChariotRootConstructorRecords49U(ctor,ctorPhase);a::initializeChariotRootSharedColdRecords49U(ctorShared);
    const ChariotLifecycleState49V ctorLife{ctor.vehicle83C,ctor.vehicle840,1,0,ctor.previous85C,ctor.response};
    need(ctor.vehicle838==initial.body.vehicle838&&std::memcmp(&ctorLife,&initial.root,sizeof ctorLife)==0,"lifecycle constructor parity");
    need(ctorShared.speed434750.guard==initial.rootShared.speed434750.guard&&ctorShared.speed434750.word==initial.rootShared.speed434750.word&&ctorShared.yaw434758.guard==initial.rootShared.yaw434758.guard&&ctorShared.yaw434758.word==initial.rootShared.yaw434758.word&&std::memcmp(&ctorShared.up434650,&initial.rootShared.up434650,sizeof ctorShared.up434650)==0&&bits(ctorPhase)==bits(initial.body.secondary.time864),"cold shared/clock parity");
    Live live=initial;U physical=0,virt=0,blends=0,reverses=0,getters=0,dirtyGetters=0,transitions=0;
    for(U n=0;n<240;++n) {
        const auto& f=frames[n];c.prepareMain(live,f);
        need(c.animation(live,f)&&graphHash(live.nodes)==f.afterAnimation,"animation graph parity");
        need(c.visual(live,f)&&graphHash(live.nodes)==f.afterVisual,"visual graph parity");
        need(noiseHash(live.noise)==f.noiseAfterVisual,"shared noise after49R parity");c.bindMainScene(live);
        H bodyHash=basis;secondaryHash(bodyHash,live.body.secondary,live.supplement);need(bodyHash==f.secondaryBefore,"persistent secondary before lifecycle");
        need(live.body.vehicle838==f.entryPhysical&&live.root.vehicle83C==f.entryVirtual,"entry mode parity");
        physical+=f.entryPhysical&&!f.entryVirtual;virt+=!f.entryPhysical&&f.entryVirtual;blends+=f.entryPhysical&&f.entryVirtual;
        const auto mainBefore=live.body.main;const auto lanesBefore=live.body.mainSphere;
        need(c.root(live,f)&&graphHash(live.nodes)==f.afterRoot,"exact lifecycle return graph parity");
        need(std::memcmp(&mainBefore,&live.body.main,sizeof mainBefore)==0&&std::memcmp(&lanesBefore,&live.body.mainSphere,sizeof lanesBefore)==0,"immutable main body/lanes");
        need(liveHash(live)==f.records,"all exact-return persistent words/history/clock");
        reverses+=f.reversed;transitions+=(f.entryPhysical!=live.body.vehicle838||f.entryVirtual!=live.root.vehicle83C);
        if(f.reversed) need(!(live.nodes[49].sceneFlags190&1)&&bits(live.body.secondary.cachedSpeed34)==0&&bits(live.root.elapsed84C)==0&&live.body.vehicle838==1&&live.root.vehicle83C,"reverse dirty/stale reset boundary");
        need(c.palette(live)&&graphHash(live.nodes)==f.afterPalette&&c.paletteHash()==f.palettes,"palette graph/words parity");
        need(c.rigid()&&c.positionsHash()==f.positions,"all5755 rigid position words");
        need(f.hasGetter==(live.body.vehicle838!=0),"getter condition parity");
        if(f.hasGetter) {++getters;dirtyGetters+=!(live.nodes[49].sceneFlags190&1);}
        need(c.getter(live)&&graphHash(live.nodes)==f.afterGetter&&liveHash(live)==f.recordsAfterGetter,"later getter graph/body parity");
        if(f.hasGetter) need(std::memcmp(&c.getterPosition,&f.getterExpected,sizeof c.getterPosition)==0,"later relative getter result parity");
    }
    need(physical==36&&virt==82&&blends==122&&reverses==2&&getters==157&&dirtyGetters==2&&transitions==6,"exact240-frame mode/getter history");
    H elapsed=0,checksum=0;bool success=true;
#ifdef PROBE49V
    allocations=0;
#endif
    for(U h=0;h<histories;++h) {
        // Only an entire finite240-frame history is restarted. Canonical body,
        // modes, clock and noise persist across every frame within that history.
        live=initial;
#ifdef PROBE49V
        countAllocations=true;
#else
        const auto historyBegin=std::chrono::steady_clock::now();
#endif
        for(const auto& f:frames) {
#ifndef PROBE49V
            const auto frameBegin=excludeGetter?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
#endif
            c.prepareMain(live,f);success=c.animation(live,f)&&success;success=c.visual(live,f)&&success;c.bindMainScene(live);
#ifndef PROBE49V
            const auto rootBegin=onlyRoot?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
#endif
            success=c.root(live,f)&&success;
#ifndef PROBE49V
            if(onlyRoot) elapsed+=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-rootBegin).count();
#endif
            success=c.palette(live)&&success;if(doRigid) success=c.rigid()&&success;
#ifndef PROBE49V
            if(excludeGetter) elapsed+=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-frameBegin).count();
#endif
            const bool activeGetter=live.body.vehicle838!=0;
#ifndef PROBE49V
            const auto getterBegin=(onlyGetter&&activeGetter)?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
#endif
            success=c.getter(live)&&success;
#ifndef PROBE49V
            if(onlyGetter&&activeGetter) elapsed+=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-getterBegin).count();
#else
            (void)activeGetter;
#endif
        }
#ifdef PROBE49V
        countAllocations=false;
#else
        if(!onlyRoot&&!onlyGetter&&!excludeGetter) elapsed+=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-historyBegin).count();
#endif
        need(success,"hot call admitted");need(graphHash(live.nodes)==frames[239].afterGetter&&liveHash(live)==frames[239].recordsAfterGetter,"history final graph/records");
        need(c.paletteHash()==frames[239].palettes,"history final palettes");if(doRigid) need(c.positionsHash()==frames[239].positions,"history final vertices");
        checksum+=graphHash(live.nodes)^liveHash(live)^c.paletteHash()^(doRigid?c.positionsHash():0);
    }
#ifdef PROBE49V
    need(allocations==0,"hot ordinary new allocated");const H counted=allocations;const U counterEnabled=1;
#else
    const H counted=0;const U counterEnabled=0;
#endif
    std::printf("mode=%s histories=%u frames=%u nanoseconds=%llu checksum=%llu cpp_new_allocations=%llu allocation_counter_enabled=%u retained_motion_bytes=%zu final_graph=%llu final_records=%llu final_palettes=%llu final_positions=%llu validated_physical_calls=%u validated_virtual_calls=%u validated_blend_calls=%u validated_reverse_calls=%u validated_getter_calls=%u validated_dirty_getter_calls=%u validated_transitions=%u\n",argv[4],histories,histories*240,static_cast<unsigned long long>(elapsed),static_cast<unsigned long long>(checksum),static_cast<unsigned long long>(counted),counterEnabled,assets.retainedBytes,static_cast<unsigned long long>(graphHash(live.nodes)),static_cast<unsigned long long>(liveHash(live)),static_cast<unsigned long long>(c.paletteHash()),static_cast<unsigned long long>(c.positionsHash()),physical,virt,blends,reverses,getters,dirtyGetters,transitions);
}
