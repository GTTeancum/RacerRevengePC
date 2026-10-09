#define main secondary_suite_unused
#include "secondary_activation49v.cpp"
#undef main
#include "main_reset50ar.h"
static void projectMain(bool load, std::array<unsigned char, 0x330>& bytes, MainBodyState49K& main, MainSphereLanes49M& lanes, SecondaryActivationSupplement49V& extra) {
    auto& root = main.components.root; auto& body = root.body;
    field(load, bytes, 0x34, main.cachedSpeed34);
    field(load, bytes, 0x60, body.position);
    field(load, bytes, 0x70, body.velocity); field(load, bytes, 0x7c, lanes.velocity70W);
    field(load, bytes, 0x80, root.localVelocity80); field(load, bytes, 0x8c, extra.localVelocity80W);
    field(load, bytes, 0x90, root.previousVelocity90); field(load, bytes, 0x9c, extra.previousVelocity90W);
    field(load, bytes, 0xa0, root.worldAngularVelocityA0);
    field(load, bytes, 0xb0, body.angularVelocity); field(load, bytes, 0xbc, lanes.angularVelocityB0W);
    field(load, bytes, 0xc0, body.linearAcceleration); field(load, bytes, 0xcc, extra.linearAccelerationC0W);
    field(load, bytes, 0xd0, body.angularAcceleration); field(load, bytes, 0xdc, extra.angularAccelerationD0W);
    field(load, bytes, 0xe0, main.cachedAnglesE0);
    field(load, bytes, 0x180, root.loads.force); field(load, bytes, 0x18c, lanes.force180W);
    field(load, bytes, 0x190, root.loads.torque); field(load, bytes, 0x19c, lanes.torque190W);
    field(load, bytes, 0x1a0, root.positionCorrection1a0); field(load, bytes, 0x1b0, root.rotationCorrection1b0);
    field(load, bytes, 0x1c0, root.accumulator1c0); field(load, bytes, 0x1d0, root.accumulator1d0);
    field(load, bytes, 0x1f0, body.orientation);
    field(load, bytes, 0x20c, main.components.body20C); field(load, bytes, 0x238, root.callbackState238);
    field(load, bytes, 0x2b8, extra.positionDerivative2B8); field(load, bytes, 0x2c4, body.previousPositionDerivative);
    field(load, bytes, 0x2d0, extra.linearAcceleration2D0); field(load, bytes, 0x2dc, body.previousLinearAcceleration);
    field(load, bytes, 0x2e8, body.orientationDerivative); field(load, bytes, 0x2f8, body.previousOrientationDerivative);
    field(load, bytes, 0x308, extra.angularAcceleration308); field(load, bytes, 0x314, body.previousAngularAcceleration);
}

int main(){unsigned cases=0;constexpr int modes[]={FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO};for(unsigned seed:{0u,1u,17u,89u,213u,254u})for(unsigned pattern=0;pattern<3;++pattern)for(int rounding:modes){Machine oracle(seed);SecondaryActivationGlobals49V globals{};initializedSecondaryActivationGlobals49V(globals);if(pattern==1){const U v[4]={0x80000000u,0x7fa12345u,0xff800000u,0x12345678u},q[4]={0x7fc54321u,0x80000000u,1,0xffa56789u};std::memcpy(globals.vector433B80,v,16);std::memcpy(globals.quaternion382B60,q,16);}if(pattern==2){const U v[4]={0x3f000000,0xbf800000,0x40800000,0x80000000},q[4]={0x41200000,0x42200000,0x43200000,0x44200000};std::memcpy(globals.vector433B80,v,16);std::memcpy(globals.quaternion382B60,q,16);}std::memcpy(oracle.vectors.data(),globals.vector433B80,16);std::memcpy(oracle.quaternion.data(),globals.quaternion382B60,16);
MainBodyState49K native{};MainSphereLanes49M lanes{};SecondaryActivationSupplement49V extra{};native.vehicle380=seed+123;lanes.sceneFlags190=seed^0x12345678;auto image=oracle.body;projectMain(true,image,native,lanes,extra);auto expected=native;auto expectedLanes=lanes;auto expectedExtra=extra;const auto beforeGlobals=globals;oracle.g[4][0]=Machine::bodyAddress;oracle.run(0x167a50);check(oracle.helperCount==6,"six original zero-fill helpers");projectMain(true,oracle.body,expected,expectedLanes,expectedExtra);std::fesetround(rounding);std::feclearexcept(FE_ALL_EXCEPT);auto before=allocations;countAllocations=true;resetMainRigidBody50AR(native,lanes,extra,globals);countAllocations=false;check(allocations==before,"main reset zero hot allocations");check(std::fetestexcept(FE_ALL_EXCEPT)==0,"raw main reset raises no FP exceptions");check(std::fegetround()==rounding,"rounding mode unchanged");check(same(native,expected)&&same(lanes,expectedLanes)&&same(extra,expectedExtra),"whole canonical main and supplemental state exact");check(same(globals,beforeGlobals),"source globals preserved");projectMain(false,image,native,lanes,extra);check(image==oracle.body,"all816 original bytes match");++cases;}std::fesetround(FE_TONEAREST);std::printf("main_reset50ar: PASS %u cases, %u checks, %u hot allocations, %u full-object bytes compared\n",cases,checks,allocations,cases*0x330);return 0;}
