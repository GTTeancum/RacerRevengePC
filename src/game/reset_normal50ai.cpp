#include "reset_normal50ai.h"
namespace rr::game {
Type3LeafStatus48P intersectResetTriangle50AI(const Type3Segment48P&s,
 const Type3Triangle48P&t,ResetLeafResult50AH&out) noexcept{
 if(t.hasVertexNormals&&t.normalMode>1)return Type3LeafStatus48P::UnsupportedNormalMode;
 Type3PairLeafState48P p{};p.fraction=out.fraction;p.normal=out.normal;p.point=out.point;
 Type3SelectedLeafState48P unused{};unused.fraction=-1.f;
 const auto status=applyType3Triangle48P(s,t,p,unused);
 if(status==Type3LeafStatus48P::Hit)out={p.fraction,p.normal,p.point};
 return status;
}
}
