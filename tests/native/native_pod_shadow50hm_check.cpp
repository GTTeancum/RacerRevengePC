// Bounded pure-host shadow-footprint checks. Not gameplay or original parity.
#include "../../platform/linux/native/native_pod_shadow50hm.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
namespace p = rr::platform;
namespace g = rr::game;
namespace a = rr::assets;
int main() {
    a::Vertex48B vertices[8]{};
    const float points[8][3] = {{-2,10,-6},{2,10,-6},{2,10,6},{-2,10,6},
        {-200,10,-600},{200,10,-600},{200,10,600},{-200,10,600}};
    for (unsigned i=0;i<8;++i) std::memcpy(vertices[i].position,points[i],12);
    a::SceneSpan48D spans[2]{};
    spans[0].vertex_count=4; spans[1].vertex_start=4;spans[1].vertex_count=4;
    g::VehicleDrawSubmission50CM mesh{};
    mesh.vertices=vertices;mesh.vertexCount=8;mesh.spans=spans;mesh.spanCount=2;
    p::NativePodShadow50HM shadow{};
    assert(p::buildNativePodShadow50HM(mesh,1,shadow));
    assert(std::abs(shadow.centerAndReach[0])<1e-5 && shadow.centerAndReach[1]==10 &&
        std::abs(shadow.centerAndReach[2])<1e-5);
    assert(std::abs(shadow.shapeAndOpacity[0]-1.f/(2.2f*2.2f))<1e-6f);
    assert(std::abs(shadow.shapeAndOpacity[2]-1.f/(6.6f*6.6f))<1e-6f);
    assert(shadow.shapeAndOpacity[3]==.48f);
    // An oversized transparent effect never contributes to opaque fitting.
    p::NativePodShadow50HM withEffect{};
    assert(p::buildNativePodShadow50HM(mesh,3,withEffect));
    assert(withEffect.shapeAndOpacity[0] < shadow.shapeAndOpacity[0]/1000.f);
    // Native projection follows current geometry's translation and rotation.
    for (auto& v:vertices) {float x=v.position[0];v.position[0]=v.position[2]+50000;
        v.position[2]=-x-25000;v.position[1]+=80;}
    p::NativePodShadow50HM rotated{};
    assert(p::buildNativePodShadow50HM(mesh,1,rotated));
    assert(rotated.centerAndReach[0]==50000 && rotated.centerAndReach[1]==90 &&
        rotated.centerAndReach[2]==-25000);
    assert(std::abs(rotated.shapeAndOpacity[0]-shadow.shapeAndOpacity[2])<1e-6f);
    assert(std::abs(rotated.shapeAndOpacity[2]-shadow.shapeAndOpacity[0])<1e-6f);
    // Excluded nonfinite effects do not poison an otherwise valid opaque body.
    vertices[4].position[0]=std::numeric_limits<float>::quiet_NaN();
    assert(p::buildNativePodShadow50HM(mesh,1,rotated));
    const auto retained=rotated;
    assert(!p::buildNativePodShadow50HM(mesh,3,rotated));
    assert(std::memcmp(&retained,&rotated,sizeof(retained))==0);
    assert(!p::buildNativePodShadow50HM(mesh,4,rotated));
    spans[1].vertex_start=3;
    assert(!p::buildNativePodShadow50HM(mesh,1,rotated));
    spans[1].vertex_start=4;
    assert(p::buildNativePodShadow50HM(mesh,0,rotated));
    assert(rotated.shapeAndOpacity[3]==0);
    std::cout << "native pod shadow: opaque selection, rotation/translation, finite checks, bounds, disable PASS\n";
}
