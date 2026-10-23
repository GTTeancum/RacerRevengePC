#include "../../src/game/original_garage_character50gn.h"
#include <array>
#include <cstdlib>
#include <iostream>
using namespace rr::game;
unsigned checks=0;void need(bool b,const char*s){++checks;if(!b){std::cerr<<"FAIL "<<s<<'\n';std::exit(1);}}
int main(){
 std::array<SceneNode49P,2> nodes{};for(auto&n:nodes){for(unsigned j=0;j<16;j+=5)n.localC0.v[j]=n.world100.v[j]=1;n.sceneFlags190=0x80;}
 nodes[0].firstChild184=static_cast<SceneNodeIndex49P>(1);nodes[1].parent180=static_cast<SceneNodeIndex49P>(0);
 nodes[0].localC0.v[12]=10;nodes[1].localC0.v[12]=2;nodes[1].localC0.v[13]=3;nodes[1].localC0.v[14]=4;
 OriginalGarageCharacterShared50GN shared;OriginalGarageCharacterFields50GN f;OriginalGarageCharacterDiagnostic50GN d;
 initializeOriginalGarageCharacterFields11A75050GN(f);f.firstChild284=static_cast<SceneNodeIndex49P>(1);
 need(f.position220==std::array<float,4>{{0,0,0,1}}&&f.velocity230==f.position220&&f.position240==f.position220&&f.velocity250==f.position220&&f.contact260==f.position220&&f.normal270==f.position220,"six actual constructor vf0 vectors");
 need(!updateOriginalGarageCharacter11AAE050GN(0,shared,{nodes.data(),2},f,d)&&!shared.lazy816C,"dt0 unsupported; no zero-velocity substitute");
 need(updateOriginalGarageCharacter11AAE050GN(.5f,shared,{nodes.data(),2},f,d)&&shared.lazy816C&&shared.assertion3E02B0==-1,"actual lazy prefix then source world position update");
 need(f.position220==std::array<float,4>{{12,3,4,1}}&&f.velocity230==std::array<float,4>{{24,6,8,1}},"actual child world position, not root or local");
 SceneDisplacement49P p{{3,5,2,1}};need(setLocalTranslation49P({nodes.data(),2},static_cast<SceneNodeIndex49P>(1),p),"genuine graph motion setter");
 need(f.position220==std::array<float,4>{{12,3,4,1}},"sound before character tick still reads previous model220");
 need(updateOriginalGarageCharacter11AAE050GN(.25f,shared,{nodes.data(),2},f,d)&&f.position220==std::array<float,4>{{13,5,2,1}}&&f.velocity230==std::array<float,4>{{4,8,-8,1}},"next real Entity tick advances previous position");
 need(f.position240==std::array<float,4>{{0,0,0,1}}&&f.velocity250==f.position240&&f.contact260==f.position240&&f.normal270==f.position240,"absent collider leaves remaining constructor fields alone");
 std::cout<<"PASS "<<checks<<" Garage character numeric vectors; no Entity or scene admission\n";
}
