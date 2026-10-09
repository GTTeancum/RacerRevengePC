#include "effect_names50bl.h"
namespace rr::assets {namespace {
bool match(const char(&name)[128],std::uint8_t&out){std::size_t n=0;while(n<128&&name[n]){const auto c=static_cast<unsigned char>(name[n]);if(c<32||c>126)return false;++n;}if(n==128)return false;out=n==8;constexpr char key[]="damage00";if(out)for(unsigned i=0;i<8;++i){char c=name[i];if(c>='A'&&c<='Z')c=static_cast<char>(c+32);if(c!=key[i]){out=0;break;}}return true;}
}
void initializeFreshModelRootName50BL(ModelRootName50BL&name) noexcept {name.text[0]=0;name.known=true;}
bool buildEffectNames50BL(const rr::game::SceneGraph49P&g,const EffectNameModel50BL*models,std::size_t count,EffectNames50BL&out) noexcept {
 using namespace rr::game;if(count>4||(count&&!models)||!validateSceneGraph49P(g))return false;EffectNames50BL next{};
 for(std::size_t m=0;m<count;++m){const auto&model=models[m];const auto root=static_cast<std::int32_t>(model.root);if(root<0||static_cast<std::uint32_t>(root)>=g.count||g.nodes[root].parent180!=SceneNodeIndex49P::none||!model.rootName||!model.rootName->known||model.nodeCount!=model.hierarchy.nodeCount||model.nodeCount>=maxSceneNodes49P||(model.nodeCount&&!model.sourceNodes))return false;
  const auto rootBit=std::uint64_t(1)<<root;if(next.knownNodes&rootBit)return false;next.knownNodes|=rootBit;next.modelRoots|=rootBit;if(!match(model.rootName->text,next.damage00[root]))return false;std::uint64_t members=rootBit;
  for(std::size_t i=0;i<model.nodeCount;++i){const auto index=static_cast<std::int32_t>(model.sourceNodes[i]);if(index<0||static_cast<std::uint32_t>(index)>=g.count)return false;const auto bit=std::uint64_t(1)<<index;if(next.knownNodes&bit)return false;PsgNode49O source{};if(!readPsgNode49O(model.hierarchy,static_cast<std::uint32_t>(i),source)||source.parent<-1||source.parent>=static_cast<std::int32_t>(i))return false;const auto parent=source.parent<0?model.root:model.sourceNodes[source.parent];if(g.nodes[index].parent180!=parent||!match(source.name,next.damage00[index]))return false;next.knownNodes|=bit;members|=bit;}
  for(std::uint32_t i=0;i<g.count;++i){auto p=g.nodes[i].parent180;for(unsigned depth=0;p!=SceneNodeIndex49P::none&&depth<g.count;++depth){if(p==model.root){if(!(members&(std::uint64_t(1)<<i)))return false;break;}p=g.nodes[static_cast<std::int32_t>(p)].parent180;}}
 }
 out=next;return true;
}
}
