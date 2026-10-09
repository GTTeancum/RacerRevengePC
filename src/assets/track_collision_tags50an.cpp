#include "track_collision_tags50an.h"
namespace rr::assets {
bool classifyTrackCollision50AN(const char*name,std::size_t n,TrackCollisionMasks50AN m,std::uint32_t&out)noexcept{
 if(!name||!n||n>127)return false;char copy[128]{};
 for(std::size_t i=0;i<n;++i){auto c=static_cast<unsigned char>(name[i]);if(c<32||c>126)return false;copy[i]=static_cast<char>((c>='A'&&c<='Z')?c+32:c);}
 auto contains=[&](const char*s,std::size_t len){for(std::size_t i=0;i+len<=n;++i){bool same=true;for(std::size_t k=0;k<len;++k)if(copy[i+k]!=s[k]){same=false;break;}if(same)return true;}return false;};
 auto c=m.staticObject;if(contains("floor",5))c|=m.floor;if(contains("water",5))c|=m.water;if(contains("tube",4))c|=m.tube;out=c;return true;
}
}
