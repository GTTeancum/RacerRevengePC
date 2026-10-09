#include "track_navigation50eu.h"
#include "chariot_scene49u.h"
#include "../assets/track_start50ad.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using E=TrackNavigationError50EU;
bool fail(TrackNavigationDiagnostic50EU& d,E e,std::uint32_t pc,std::uint32_t node=0) noexcept {d={e,pc,node};return false;}
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
float fp(std::uint32_t u) noexcept {float x;std::memcpy(&x,&u,4);return x;}
using V=VisualVector49R;
struct Math {
    bool valid=true;
    float read(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {read(a);read(b);volatile float x=a+b;return read(x);}
    float sub(float a,float b) noexcept {read(a);read(b);volatile float x=a-b;return read(x);}
    float mul(float a,float b) noexcept {read(a);read(b);volatile float x=a*b;return read(x);}
    float div(float a,float b) noexcept {read(a);read(b);if(b==0){valid=false;return 0;}volatile float x=a/b;return read(x);}
    float dot(const V& a,const V& b) noexcept {
        const float x=mul(a.v[0],b.v[0]),y=mul(a.v[1],b.v[1]),z=mul(a.v[2],b.v[2]);
        return add(add(x,y),mul(1.f,z));
    }
    V normal(V a) noexcept {
        const float norm=dot(a,a);float q=0.f;
        if(norm!=0.f&&(bits(norm)&0x7f800000u)==0)valid=false;
        if(norm==0.f)q=fp((bits(norm)&0x80000000u)|0x7f7fffffu);
        else {volatile float root=std::sqrt(norm);q=div(1.f,read(root));}
        return {{mul(a.v[0],q),mul(a.v[1],q),mul(a.v[2],q),1.f}};
    }
};
bool validTrack(const TrackNavigation50EU& t) noexcept {
    if(!t.track.count||t.track.count>maxUndertrackNodes50ET)return false;
    for(std::uint32_t i=0;i<t.track.count;++i){const auto& n=t.track.nodes[i];
        if(!n.nextCount||n.nextCount>maxUndertrackNext50ET)return false;
        for(unsigned j=0;j<n.nextCount;++j)if(n.next[j]>=t.track.count)return false;
        for(unsigned j=0;j<3;++j)if(!std::isfinite(n.position[j]))return false;}
    return true;
}
struct Slice {const char* p;std::size_t n;};
char lower(char c) noexcept {return c>='A'&&c<='Z'?static_cast<char>(c+32):c;}
Slice trim(Slice s) noexcept {auto space=[](char c){return c==' '||c=='\t'||c=='\r';};while(s.n&&space(*s.p)){++s.p;--s.n;}while(s.n&&space(s.p[s.n-1]))--s.n;return s;}
bool equal(Slice s,const char* p) noexcept {if(s.n!=std::strlen(p))return false;for(std::size_t i=0;i<s.n;++i)if(lower(s.p[i])!=lower(p[i]))return false;return true;}
bool integer(Slice s,std::int32_t& out) noexcept {
    if(!s.n||s.n>11)return false;
    std::size_t i=0;bool negative=false;
    if(s.p[0]=='-'||s.p[0]=='+'){negative=s.p[0]=='-';if(++i==s.n)return false;}
    std::uint64_t n=0;for(;i<s.n;++i){if(s.p[i]<'0'||s.p[i]>'9')return false;n=n*10+unsigned(s.p[i]-'0');if(n>2147483648ull)return false;}
    if(!negative&&n>2147483647ull)return false;
    out=negative?static_cast<std::int32_t>(-static_cast<std::int64_t>(n)):static_cast<std::int32_t>(n);return true;
}
bool supplement(const char* text,std::size_t bytes,TrackNavigationScratch50EU& scratch,
    TrackNavigationDiagnostic50EU& d) noexcept {
    auto& t=scratch.candidate;std::size_t at=0;int section=-2;float trackWidth=0;bool widthSeen=false;
    while(at<bytes){const auto begin=at;while(at<bytes&&text[at]!='\n')++at;auto line=trim({text+begin,at-begin});if(at<bytes)++at;
        if(!line.n||line.p[0]==';')continue;
        if(line.p[0]=='['){auto name=trim({line.p+1,line.n-2});if(equal(name,"Track"))section=-1;else {std::int32_t index=0;if(name.n<5||!integer({name.p+4,name.n-4},index)||index<0||static_cast<unsigned>(index)>=t.track.count)return fail(d,E::Text,0x2a1260);section=index;}continue;}
        std::size_t split=0;while(split<line.n&&line.p[split]!='=')++split;if(split==line.n)return fail(d,E::Text,0x2a1260);
        auto key=trim({line.p,split}),value=trim({line.p+split+1,line.n-split-1});
        if(section==-1){if(equal(key,"TrackWidth")){if(widthSeen||!assets::parseTrackNumber50AD(value.p,value.n,trackWidth))return fail(d,E::Text,0x2a136c);widthSeen=true;}continue;}
        if(section<0)return fail(d,E::Text,0x2a1260);
        auto& n=t.nodes[section];unsigned field=0;float* number=nullptr;unsigned flag=0;
        if(equal(key,"NodeHeight")){field=1;number=&n.height50;}
        else if(equal(key,"LeftTrackWidth")){field=2;number=&n.left54;}
        else if(equal(key,"RightTrackWidth")){field=4;number=&n.right58;}
        else if(equal(key,"BranchID"))field=8;
        else if(equal(key,"ProgressNode"))field=16;
        else {const char* names[]={"NodeBlocked","NoCombatZone","ShortestBranch","SkipPlayerTest","DriveFullSpeed","SecretBranch","Pretty","Ugly"};for(unsigned j=0;j<8;++j)if(equal(key,names[j])){flag=1u<<j;field=32u<<j;break;}}
        if(!field)continue;
        if(scratch.fieldMask[section]&field)return fail(d,E::Text,0x2a1260,section);
        scratch.fieldMask[section]|=field;
        if(number){if(!assets::parseTrackNumber50AD(value.p,value.n,*number))return fail(d,E::Text,0x2a152c,section);}
        else {std::int32_t x;if(!integer(value,x))return fail(d,E::Text,0x2a18cc,section);
            if(field==8)n.branch68=x;else if(field!=16)n.flags79=static_cast<std::uint8_t>(n.flags79|flag);}
    }
    if(!widthSeen)return fail(d,E::Text,0x2a136c);
    Math f;
    unsigned progressNodes=0;
    for(std::uint32_t i=0;i<t.track.count;++i){auto& n=t.nodes[i];
        if(scratch.fieldMask[i]&16){if(++progressNodes>10)return fail(d,E::Track,0x2a1948,i);n.height50=10000.f;}
        if(scratch.fieldMask[i]&2)n.left54=f.add(f.mul(.5f,trackWidth),n.left54);
        if(scratch.fieldMask[i]&4)n.right58=f.add(f.mul(.5f,trackWidth),n.right58);}

    return f.valid||fail(d,E::Arithmetic,0x2a158c);
}
bool gates(TrackNavigation50EU& t,SourceLibmState49T& libm,TrackNavigationDiagnostic50EU& d) noexcept {
    Math f;const V up{{0.f,1.f,0.f,1.f}}; // Static constructor3CB0C0..3CB13C,443930.
    for(std::uint32_t i=0;i<t.track.count;++i)for(unsigned j=0;j<t.track.nodes[i].nextCount;++j){
        const auto target=t.track.nodes[i].next[j];auto& meta=t.nodes[target];
        const auto* previous=t.track.nodes[i].position;const auto* current=t.track.nodes[target].position;
        const auto* next=t.track.nodes[t.track.nodes[target].next[0]].position;
        V forward{{f.sub(next[0],current[0]),0.f,f.sub(next[2],current[2]),1.f}};
        V backward{{f.sub(previous[0],current[0]),0.f,f.sub(previous[2],current[2]),1.f}};
        forward=f.normal(forward);backward=f.normal(backward);const float dot=f.dot(forward,backward);float angle=0.f;
        if(dot< -1.f)angle=fp(0x40490fdbu);
        else if(dot<=1.f){std::uint32_t result=0;if(!sourceAcos49T(bits(dot),libm,result))return fail(d,E::Arithmetic,0x2a1f6c,target);angle=fp(result);}
        const float half=f.div(angle,2.f);V rotated{{0,0,0,1}};
        if(!f.valid||!detail::rotateVector49U(up,forward,half,rotated))return fail(d,E::Arithmetic,0x2a1fa4,target);
        rotated=f.normal(rotated);const bool reverse=f.dot(backward,rotated)<0.f;
        if(reverse&&!detail::rotateVector49U(up,forward,-half,rotated))return fail(d,E::Arithmetic,0x2a2044,target);
        const float left=reverse?meta.left54:-meta.left54,right=reverse?-meta.right58:meta.right58;
        const float wideLeft=reverse?f.add(500.f,meta.left54):f.sub(-meta.left54,500.f);
        const float wideRight=reverse?f.sub(-meta.right58,500.f):f.add(500.f,meta.right58);
        const float scale[]={left,right,wideLeft,wideRight};float* out[]={meta.gate10,meta.gate20,meta.wideGate30,meta.wideGate40};
        for(unsigned k=0;k<4;++k){for(unsigned lane=0;lane<3;++lane)out[k][lane]=f.add(current[lane],f.mul(scale[k],rotated.v[lane]));out[k][3]=1.f;}
        if(!f.valid)return fail(d,E::Arithmetic,0x2a23ec,target);
    }
    t.gatesPrepared=true;return true;
}
bool predecessor(const TrackNavigation50EU& t,std::uint32_t node,std::uint32_t& out) noexcept {
    if(node&&t.nodes[node].branch68==t.nodes[node-1].branch68){out=node-1;return true;}
    if(!node){for(std::uint32_t i=0;i<t.track.count;++i)if(t.track.nodes[i].next[0]==0){out=i;return true;}return false;}
    for(std::uint32_t i=node;i>0;--i)for(unsigned j=0;j<t.track.nodes[i].nextCount;++j)if(t.track.nodes[i].next[j]==node){out=i;return true;}
    return false;
}
bool worldValid(const LiveZoneWorld50EN& w) noexcept {
    if(!w.zoneCount||w.zoneCount>256||!w.portalHeads||(w.portalLinkCount&&!w.portalLinks)||(w.portalCount&&!w.portals))return false;
    for(std::size_t i=0;i<w.zoneCount;++i){auto ref=w.portalHeads[i];std::size_t count=0;while(ref){if(ref>w.portalLinkCount||++count>w.portalLinkCount)return false;const auto& p=w.portalLinks[ref-1];if(!p.zone||p.zone>w.zoneCount||!p.portal||p.portal>w.portalCount)return false;ref=p.next;}}
    return true;
}
bool walk(const LiveZoneWorld50EN& w,const Vector3_48C& from,const Vector3_48C& to,
    std::uint32_t& zone,TrackNavigationDiagnostic50EU& d,std::uint32_t node) noexcept {
    std::uint32_t previous=0;auto link=w.portalHeads[zone-1];std::size_t visits=0;
    const auto limit=(w.zoneCount+1u)*(w.portalLinkCount+1u);
    while(link){if(++visits>limit)return fail(d,E::Traversal,0x280df0,node);const auto& p=w.portalLinks[link-1];
        if(p.zone!=previous){bool crossed=false;if(crossPortal230BD0_50EO(w.portals[p.portal-1],from,to,crossed)!=PortalGeometryStatus50EO::Complete)return fail(d,E::Portal,0x280dc8,node);
            if(crossed){previous=zone;zone=p.zone;link=w.portalHeads[zone-1];continue;}}
        link=p.next;
    }
    return true;
}
bool crossing(const TrackNavigation50EU& t,std::uint32_t node,std::uint32_t current,
    const TrackNavigationInput50EU& in,float height,bool& out,Math& f) noexcept {
    out=false;const auto& meta=t.nodes[node];const float* left=node==current?meta.wideGate30:meta.gate10;const float* right=node==current?meta.wideGate40:meta.gate20;
    const float px=f.read(in.previousCA0[0]),pz=f.read(in.previousCA0[2]);if(px==0.f&&pz==0.f)return f.valid;
    const float dx=f.sub(in.currentLocal190[0],px),dz=f.sub(in.currentLocal190[2],pz),gx=f.sub(right[0],left[0]),gz=f.sub(right[2],left[2]);
    //142D00, all accumulator steps retain separate binary32 rounding.
    const float denominator=f.sub(f.mul(dz,gx),f.mul(gz,dx));if(denominator==0.f)return f.valid;
    const float a=f.div(f.add(f.mul(gx,f.sub(left[2],pz)),f.mul(gz,f.sub(px,left[0]))),denominator);
    const float b=gx!=0.f?f.div(f.sub(f.add(px,f.mul(dx,a)),left[0]),gx):f.div(f.sub(f.add(pz,f.mul(dz,a)),left[2]),gz);
    if(!f.valid)return false;
    if(a<0.f||a>1.f||b<0.f||b>1.f)return true;
    const float dy=std::fabs(f.sub(t.track.nodes[node].position[1],in.currentLocal190[1]));
    const float limit=in.mainBody104?height:f.add(700.f,height);out=dy<=limit;return f.valid;
}
bool advanceOne(const TrackNavigation50EU& t,const TrackNavigationInput50EU& in,bool map,
    TrackNavigationProgress50EU& p,std::uint32_t& sequential,std::uint32_t& scanned,
    TrackNavigationDiagnostic50EU& d,bool scanOnly,
    const TrackNavigationMapEffects50GH* effects=nullptr) noexcept {
    auto& current=map?p.reset.vehicleC88:p.reset.vehicleC80;auto& previous=map?p.reset.vehicleC8C:p.vehicleC84;
    if(current>=t.track.count||previous>=t.track.count)return fail(d,E::Track,0x1a0da0,current);
    Math f;std::uint8_t visited[maxUndertrackNodes50ET]{};
    while(!scanOnly){bool hit=false;const float height=map?t.nodes[current].height50:800.f;
        if(!crossing(t,current,current,in,height,hit,f))return fail(d,E::Arithmetic,map?0x1a08a0:0x1a0aa0,current);
        if(!hit)break;
        if(visited[current])return fail(d,E::Traversal,0x1a0f78,current);
        visited[current]=1;
        if(map&&effects&&!effects->accepted(effects->context,true,current,current))
            return fail(d,E::EffectBoundary,0x1a0e60,current);
        previous=current;current=t.track.nodes[current].next[0];++sequential;
    }
    if(!scanOnly||sequential)return true;
    const auto branch=t.nodes[current].branch68;
    for(std::uint32_t i=0;i<t.track.count;++i){const auto& meta=t.nodes[i];if(meta.branch68!=branch&&(meta.flags79&8u))continue;
        bool hit=false;if(!crossing(t,i,current,in,map?meta.height50:800.f,hit,f))return fail(d,E::Arithmetic,map?0x1a10bc:0x1a0fdc,i);
        if(hit){if(map&&effects&&!effects->accepted(effects->context,false,i,current))
                return fail(d,E::EffectBoundary,0x1a10d0,i);
            if(map||!(meta.flags79&64u))previous=i;current=t.track.nodes[i].next[0];++scanned;}}
    return true;
}
} // namespace
bool readTrackNavigation50EU(const char* text,std::size_t bytes,SourceLibmState49T& libm,
    TrackNavigationScratch50EU& scratch,TrackNavigation50EU& out,TrackNavigationDiagnostic50EU& d) noexcept {
    if(&scratch.candidate==&out)return fail(d,E::Input,0x2a1260);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::Arithmetic,0x2a1260);
    scratch={};UndertrackDiagnostic50ET td{};
    if(!readUndertrackTrack50ET(text,bytes,scratch.reader,scratch.candidate.track,td))return fail(d,E::Text,td.sourcePC);
    if(!validTrack(scratch.candidate))return fail(d,E::Track,0x2a1b7c);
    if(!supplement(text,bytes,scratch,d))return false;
    auto stagedLibm=libm;
    if(!gates(scratch.candidate,stagedLibm,d))return false;
    out=scratch.candidate;libm=stagedLibm;d={};return true;
}
bool bindTrackNavigationZones50EU(const LiveZoneWorld50EN& world,TrackNavigation50EU& t,
    TrackNavigationDiagnostic50EU& d) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::Arithmetic,0x1acf70);
    if(!validTrack(t)||!worldValid(world))return fail(d,E::Input,0x1acf70);
    std::uint8_t index[maxUndertrackNodes50ET]{}; //2A0F30 constructor, including node0.
    for(std::uint32_t i=1;i<t.track.count;++i){std::uint32_t before=0;if(!predecessor(t,i,before))return fail(d,E::MissingPredecessor,0x2a26e0,i);
        const auto* a=t.track.nodes[before].position;const auto* b=t.track.nodes[i].position;
        const Vector3_48C from{{a[0],a[1],a[2]}},to{{b[0],b[1],b[2]}};
        const Vector3_48C middle=b[1]>a[1]?Vector3_48C{{a[0],b[1],a[2]}}:Vector3_48C{{b[0],a[1],b[2]}};
        std::uint32_t zone=static_cast<std::uint32_t>(index[before])+1u;
        if(zone>world.zoneCount)return fail(d,E::Input,0x1ad0f0,i);
        if(!walk(world,from,middle,zone,d,i)||!walk(world,middle,to,zone,d,i))return false;
        index[i]=static_cast<std::uint8_t>(zone-1u);
    }
    std::memcpy(t.nodeIndex78,index,sizeof index);t.indicesPrepared=true;d={};return true;
}
void initializeTrackNavigationProgress50EU(TrackNavigationProgress50EU& p) noexcept {p.reset={1,1,0};p.vehicleC84=0;}
bool advancePlayerTrackNavigation50EU(const TrackNavigation50EU& map,const TrackNavigation50EU& ai,
    const TrackNavigationInput50EU& in,TrackNavigationProgress50EU& progress,
    TrackNavigationReceipt50EU& receipt,TrackNavigationDiagnostic50EU& d) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::Arithmetic,0x1a0da0);
    if(!map.gatesPrepared||!ai.gatesPrepared)return fail(d,E::NotPrepared,0x1a0da0);
    if(!validTrack(map)||!validTrack(ai))return fail(d,E::Track,0x1a0da0);
    for(unsigned j=0;j<3;++j)if(!std::isfinite(in.previousCA0[j])||!std::isfinite(in.currentLocal190[j]))return fail(d,E::Input,0x1a08a0);
    auto next=progress;TrackNavigationReceipt50EU result{};
    // Preserve original loop ordering; this projection still commits only after
    // all four stages succeed and has no external map-effect provider.
    if(!advanceOne(ai,in,false,next,result.aiSequential,result.aiScan,d,false)||
       !advanceOne(map,in,true,next,result.mapSequential,result.mapScan,d,false)||
       !advanceOne(ai,in,false,next,result.aiSequential,result.aiScan,d,true)||
       !advanceOne(map,in,true,next,result.mapSequential,result.mapScan,d,true))return false;
    progress=next;receipt=result;d={};return true;
}
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigation50EU& map,
    const TrackNavigation50EU& ai,const TrackNavigationInput50EU& in,
    const TrackNavigationMapEffects50GH& effects,TrackNavigationProgress50EU& p,
    TrackNavigationReceipt50EU& receipt,TrackNavigationDiagnostic50EU& d) noexcept {
    if(!effects.accepted)return fail(d,E::Input,0x1a0da0);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::Arithmetic,0x1a0da0);
    if(!map.gatesPrepared||!ai.gatesPrepared)return fail(d,E::NotPrepared,0x1a0da0);
    if(!validTrack(map)||!validTrack(ai))return fail(d,E::Track,0x1a0da0);
    for(unsigned j=0;j<3;++j)if(!std::isfinite(in.previousCA0[j])||!std::isfinite(in.currentLocal190[ j]))
        return fail(d,E::Input,0x1a08a0);
    receipt={};
    if(!advanceOne(ai,in,false,p,receipt.aiSequential,receipt.aiScan,d,false)||
       !advanceOne(map,in,true,p,receipt.mapSequential,receipt.mapScan,d,false,&effects)||
       !advanceOne(ai,in,false,p,receipt.aiSequential,receipt.aiScan,d,true)||
       !advanceOne(map,in,true,p,receipt.mapSequential,receipt.mapScan,d,true,&effects))return false;
    d={};return true;
}
bool undertrackBindings50EU(const TrackNavigation50EU& t,const std::uint32_t* visibility,
    const std::uint32_t* collision,std::size_t count,UndertrackTableBindings50ET& out) noexcept {
    if(!t.indicesPrepared||!validTrack(t)||!visibility||!collision||!count||count>256)return false;
    for(std::size_t i=0;i<count;++i)if(!visibility[i]||!collision[i])return false;
    for(std::uint32_t i=0;i<t.track.count;++i)if(t.nodeIndex78[i]>=count)return false;
    out={t.nodeIndex78,t.track.count,visibility,collision,count};return true;
}
}
