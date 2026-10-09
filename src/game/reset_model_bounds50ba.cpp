#include "reset_model_bounds50ba.h"
#include <cstring>
namespace rr::game {
bool refreshResetModelBounds50BA(const SceneGraph49P&g,SceneNodeIndex49P root,const ResetBoundNode50BA*input,std::size_t count,ResetBoundsWorkspace50BA&w,ModelBoundsResult49O&out) noexcept {
 const auto ri=static_cast<std::int32_t>(root);
 if(!validateSceneGraph49P(g)||ri<0||static_cast<std::uint32_t>(ri)>=g.count||count>=maxSceneNodes49P||(count&&!input))return false;
 if(g.nodes[ri].parent180!=SceneNodeIndex49P::none||(g.nodes[ri].sceneFlags190&0x14u))return false;
 std::int32_t map[maxSceneNodes49P];for(auto&x:map)x=-1;
 for(std::size_t i=0;i<count;++i){const auto ni=static_cast<std::int32_t>(input[i].node);if(ni<0||static_cast<std::uint32_t>(ni)>=g.count||ni==ri||map[ni]>=0)return false;const auto&n=g.nodes[ni];if((n.sceneFlags190&5u)!=4u)return false;const auto pi=static_cast<std::int32_t>(n.parent180);if(pi!=ri&&(pi<0||map[pi]<0))return false;map[ni]=static_cast<std::int32_t>(i);w.flat[i]={n.localC0,input[i].center140,input[i].halfExtent150,pi==ri?-1:map[pi],n.sceneFlags190};}
 // Exact coverage of this model's descendants; unrelated roots stay untouched.
 for(std::uint32_t i=0;i<g.count;++i){if(i==static_cast<std::uint32_t>(ri))continue;auto p=g.nodes[i].parent180;for(unsigned depth=0;p!=SceneNodeIndex49P::none&&depth<g.count;++depth){if(p==root){if(map[i]<0)return false;break;}p=g.nodes[static_cast<std::int32_t>(p)].parent180;}}
 std::memcpy(w.nodes,g.nodes,g.count*sizeof(SceneNode49P));SceneGraph49P next{w.nodes,g.count};if(!refreshSceneWorld49P(next,root))return false;
 ModelBoundsContext49O context{};context.rootWorld100=w.nodes[ri].world100;context.rootSceneFlags190=w.nodes[ri].sceneFlags190;
 if(!computeModelBounds49O(w.flat,count,context,w.numeric,w.result))return false;
 for(std::size_t i=0;i<count;++i){auto&n=w.nodes[static_cast<std::int32_t>(input[i].node)];n.world100=w.numeric.world[i];n.sceneFlags190|=1u;}
 w.nodes[ri].sceneFlags190=w.result.rootSceneFlags190;
 std::memcpy(g.nodes,w.nodes,g.count*sizeof(SceneNode49P));out=w.result;return true;
}
}
