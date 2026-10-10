#include "spatial_items50bp.h"
#include <cstring>
#include <cstdint>
namespace rr::game{
namespace{bool overlap(const void*a,std::size_t n,const void*b,std::size_t m)noexcept{auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);return x<=y?y-x<n:x-y<m;}}
SpatialItemStatus50BP editSpatialItems50BP(SpatialItemState50BP& state,std::uint32_t nodeIndex,std::uint32_t identity,bool add,SpatialItemWorkspace50BP& work,SpatialItemResult50BP& result)noexcept{
 using S=SpatialItemStatus50BP;
 if(!identity||!state.nodeCount||state.nodeCount>32||state.blockCount>128||nodeIndex>=state.nodeCount||overlap(&state,sizeof state,&work,sizeof work)||overlap(&state,sizeof state,&result,sizeof result)||overlap(&work,sizeof work,&result,sizeof result))return S::InvalidInput;
 std::memset(work.membership,0,sizeof work.membership);unsigned live=0;
 for(unsigned n=0;n<state.nodeCount;++n)for(auto ref=state.nodes[n].firstBlock;ref;ref=state.blocks[ref-1].next){if(ref>state.blockCount||work.membership[ref-1])return S::InvalidTopology;bool nonempty=false;for(auto id:state.blocks[ref-1].identities)nonempty=nonempty||id!=0;if(!nonempty)return S::InvalidTopology;work.membership[ref-1]=1;++live;}
 for(auto ref=state.freeHead;ref;ref=state.blocks[ref-1].next){if(ref>state.blockCount||work.membership[ref-1])return S::InvalidTopology;work.membership[ref-1]=2;}
 for(unsigned i=0;i<state.blockCount;++i)if(!work.membership[i])return S::InvalidTopology;
 if(state.bytesUsed!=live*20)return S::InvalidTopology;
 work.staged=state;auto& s=work.staged;auto& node=s.nodes[nodeIndex];
 if(add){auto ref=node.firstBlock;std::uint32_t last=0;bool inserted=false;
  while(ref&&!inserted){auto& block=s.blocks[ref-1];for(auto& slot:block.identities)if(!slot){slot=identity;inserted=true;break;}last=ref;ref=block.next;}
  if(!inserted){const auto fresh=s.freeHead;if(!fresh)return S::AllocatorBoundary;auto& block=s.blocks[fresh-1];s.freeHead=block.next;block={};block.identities[0]=identity;s.bytesUsed+=20;if(last)s.blocks[last-1].next=fresh;else node.firstBlock=fresh;}
 }else{std::uint32_t previous=0,ref=node.firstBlock;
  while(ref){auto& block=s.blocks[ref-1];bool empty=true,found=false;for(auto& slot:block.identities){if(slot==identity){slot=0;found=true;}else if(slot)empty=false;}
   const auto next=block.next;if(empty){if(previous)s.blocks[previous-1].next=next;else node.firstBlock=next;block.next=s.freeHead;s.freeHead=ref;s.bytesUsed-=20;}
   if(found)break;
   // Valid live blocks are nonempty; freeing therefore implies found/break.
   previous=ref;ref=next;
  }
 }
 bool alive=node.firstBlock!=0;for(auto child:node.children)alive=alive||child!=0;
 state=s;result.hasItemsOrChildren=alive;return S::Complete;
}
}
