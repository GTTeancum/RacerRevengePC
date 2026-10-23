#include "original_garage_character50gn.h"
#include <cfenv>
#include <cmath>
namespace rr::game {
void initializeOriginalGarageCharacterFields11A75050GN(OriginalGarageCharacterFields50GN& f)noexcept {
 constexpr std::array<float,4> vf0{{0,0,0,1}};
 f.position220=vf0;f.velocity230=vf0;f.position240=vf0;f.velocity250=vf0;f.contact260=vf0;f.normal270=vf0;
 f.firstChild284=SceneNodeIndex49P::none;
}
bool updateOriginalGarageCharacter11AAE050GN(float dt,OriginalGarageCharacterShared50GN& shared,
 const SceneGraph49P& graph,OriginalGarageCharacterFields50GN& f,OriginalGarageCharacterDiagnostic50GN& d)noexcept {
 d={};const auto index=static_cast<std::int32_t>(f.firstChild284);
 if(std::fegetround()!=FE_TONEAREST||!std::isfinite(dt)||dt<=0||std::fpclassify(dt)==FP_SUBNORMAL||
    !validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count){d.sourcePC=0x11aae0;return false;}
 if(!shared.lazy816C){shared.assertion3E02B0=-1;shared.lazy816C=true;} //11AB00..110.
 if(!refreshSceneWorld49P(graph,f.firstChild284)){d.sourcePC=0x28b1a0;return false;}
 const auto& w=graph.nodes[index].world100;
 std::array<float,4> position{{w.v[12],w.v[13],w.v[14],1.f}},velocity{{0,0,0,1.f}},difference{};
 for(unsigned i=0;i<3;++i){if(!std::isfinite(position[i])||!std::isfinite(f.position220[i])){d.sourcePC=0x11ab68;return false;}
  volatile float delta=position[i]-f.position220[i];difference[i]=delta;if(!std::isfinite(difference[i])){d.sourcePC=0x11ab68;return false;}}
 volatile float inverse=1.f/dt;if(!std::isfinite(inverse)){d.sourcePC=0x11ab84;return false;}
 // Source scalar instruction order is Y, X, Z, before contiguous230 stores.
 for(unsigned i:{1u,0u,2u}){volatile float product=difference[i]*inverse;velocity[i]=product;
  if(!std::isfinite(velocity[i])||std::fpclassify(velocity[i])==FP_SUBNORMAL){d.sourcePC=0x11ab90;return false;}}
 f.velocity230=velocity;f.position220=position;d.sourcePC=0x11acF0;return true;
}
}
