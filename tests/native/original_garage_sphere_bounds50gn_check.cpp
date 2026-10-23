#include "../../src/game/original_garage_sphere_bounds50gn.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
using namespace rr::game;
static unsigned checks=0;
static void need(bool b,const char* s){++checks;if(!b){std::cerr<<"FAIL "<<s<<'\n';std::exit(1);}}
static SceneNodeIndex49P ix(unsigned n){return static_cast<SceneNodeIndex49P>(n);}
struct Fixture {
    std::array<SceneNode49P,3> nodes{};
    OriginalGarageSphereRow50GN root{ix(0),{{0,0,0,1}},std::nullopt};
    std::array<OriginalGarageSphereRow50GN,2> rows{{{ix(1),{{0,0,0,1}},1.f},{ix(2),{{0,0,0,1}},2.f}}};
    OriginalGarageSphereShared50GN shared;
    OriginalGarageSphereResult50GN out;
    OriginalGarageSphereDiagnostic50GN diagnostic;
    Fixture(){for(auto& n:nodes){for(unsigned i=0;i<16;i+=5)n.localC0.v[i]=n.world100.v[i]=1.f;n.sceneFlags190=0x85u;}
        nodes[0].sceneFlags190=0x99u;nodes[0].firstChild184=ix(1);
        nodes[1].parent180=nodes[2].parent180=ix(0);nodes[1].nextSibling188=ix(2);}
    bool run(unsigned n=2){return computeOriginalGarageSphereBounds26F4B050GN({nodes.data(),3},root,rows.data(),n,shared,out,diagnostic);}
};
int main(){
    {Fixture f;f.nodes[1].world100.v[12]=1;f.nodes[2].world100.v[12]=4;f.nodes[2].world100.v[13]=4;
     need(f.run(),"flat source sphere aggregation");need(f.out.center160==std::array<float,4>{{1,0,0,1}}&&f.out.radius198==6.f,"fixed first center and Chebyshev distance, not Euclidean or recentered union");
     need(f.shared.lazy8B5C&&f.shared.assertion442F60==-1&&f.out.rootSceneFlags190==0xd9u,"source lazy assertion, radius bit40 and preserved80");
     f.shared.assertion442F60=17;need(f.run()&&f.shared.assertion442F60==17,"retained lazy guard does not rewrite assertion per call");
     std::swap(f.rows[0],f.rows[1]);need(f.run()&&f.out.center160==std::array<float,4>{{4,4,0,1}}&&f.out.radius198==5,"authentic source row order affects fixed center and radius");}
    {Fixture f;f.rows[0].radius194=0;f.nodes[1].world100.v[12]=100;f.nodes[2].world100.v[12]=7;
     need(f.run()&&f.out.center160==std::array<float,4>{{7,0,0,1}}&&f.out.radius198==2,"zero radius is reseeded, not a permanent first center");}
    {Fixture f;f.nodes[0].sceneFlags190|=4;f.root.radius194=10;f.root.center140={{1,2,3,1}};
     f.nodes[0].world100.v[12]=10;f.nodes[0].world100.v[13]=20;f.nodes[0].world100.v[14]=30;
     need(f.run(0)&&f.out.center160==std::array<float,4>{{1,2,3,1}}&&f.out.radius198==10,"actual root04 center and stored194 seed");}
    {Fixture f;f.nodes[0].world100.v[0]=2;f.nodes[0].world100.v[5]=3;f.nodes[0].world100.v[10]=4;
     f.nodes[0].world100.v[12]=10;f.nodes[0].world100.v[13]=20;f.nodes[0].world100.v[14]=30;
     f.nodes[1].world100.v[12]=13;f.nodes[1].world100.v[13]=24;f.nodes[1].world100.v[14]=35;
     f.nodes[1].world100.v[0]=100;f.nodes[1].localC0.v[12]=-999;
     need(f.run(1)&&f.out.center160==std::array<float,4>{{6,12,20,1}},"28BD50 is source transpose operation, not general inverse; actual ready cache wins");
     need(f.out.radius198==1,"stored194 is not rescaled by world basis");}
    {Fixture f;f.nodes[1].sceneFlags190=0x80;f.nodes[1].localC0.v[0]=std::numeric_limits<float>::quiet_NaN();f.rows[0].radius194.reset();
     f.rows[0].center140.fill(std::numeric_limits<float>::quiet_NaN());f.nodes[2].world100.v[12]=3;
     need(f.run()&&f.out.center160[0]==3&&f.out.radius198==2&&f.nodes[1].sceneFlags190==0x80,"04-clear row not refreshed or numerically inspected");}
    {Fixture f;f.nodes[0].sceneFlags190&=~1u;f.nodes[0].localC0.v[12]=10;f.nodes[1].sceneFlags190&=~1u;f.nodes[1].localC0.v[12]=2;
     need(f.run(1)&&f.nodes[0].world100.v[12]==10&&f.nodes[1].world100.v[12]==12&&f.out.center160[0]==2,"actual lazy world chain refresh");}
    {Fixture f;f.rows[0].radius194.reset();f.nodes[0].sceneFlags190&=~1u;f.out.radius198=123;
     need(!f.run()&&f.diagnostic.sourcePC==0x28da64u&&f.out.radius198==123,"missing actual194 is unsupported, not zero substitution");
     need(f.shared.lazy8B5C&&(f.nodes[0].sceneFlags190&1u),"successful source lazy and graph prefix survives later failure");}
    {Fixture f;f.nodes[0].sceneFlags190|=0x40u;need(f.run(0)&&f.out.radius198==0&&(f.out.rootSceneFlags190&0x40u),"nonpositive radius does not clear preexisting40");
     f.nodes[0].sceneFlags190&=~0x40u;need(f.run(0)&&!(f.out.rootSceneFlags190&0x40u),"zero radius does not set40");}
    {Fixture f;f.rows[1].node=f.rows[0].node;need(!f.run()&&!f.shared.lazy8B5C,"duplicate source identity rejected before source prefix");}
    std::cout<<"PASS "<<checks<<" original Garage sphere bounds checks; no model publication or observer admission\n";
}
