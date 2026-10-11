#include "undertrack_reposition50et.h"
#include "../assets/track_start50ad.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using E=UndertrackError50ET;
bool fail(UndertrackDiagnostic50ET& d,E e,std::uint32_t pc,std::size_t at=0) noexcept {
    d={e,pc,at};return false;
}
bool overlap(const void* a,std::size_t an,const void* b,std::size_t bn) noexcept {
    if(!an||!bn)return false;
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return x<=y?y-x<an:x-y<bn;
}
struct Span {const void* p;std::size_t bytes;};
bool separate(const Span* spans,std::size_t count) noexcept {
    for(std::size_t i=0;i<count;++i){
        const auto a=reinterpret_cast<std::uintptr_t>(spans[i].p);
        if(spans[i].bytes&&(!spans[i].p||spans[i].bytes>std::numeric_limits<std::uintptr_t>::max()-a))return false;
        for(std::size_t j=0;j<i;++j)
            if(overlap(spans[i].p,spans[i].bytes,spans[j].p,spans[j].bytes))return false;
    }
    return true;
}
float fp(std::uint32_t x) noexcept {float y;std::memcpy(&y,&x,4);return y;}
std::uint32_t bits(float x) noexcept {std::uint32_t y;std::memcpy(&y,&x,4);return y;}
struct Vec {float v[4];};
struct Math {
    bool valid=true;
    float read(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {read(a);read(b);volatile float v=a+b;return read(v);}
    float sub(float a,float b) noexcept {read(a);read(b);volatile float v=a-b;return read(v);}
    float mul(float a,float b) noexcept {read(a);read(b);volatile float v=a*b;return read(v);}
    float div(float a,float b) noexcept {read(a);read(b);if(b==0.f){valid=false;return 0.f;}volatile float v=a/b;return read(v);}
    Vec difference(const float* a,const float* b) noexcept {
        return {{sub(a[0],b[0]),sub(a[1],b[1]),sub(a[2],b[2]),1.f}};
    }
    float square(const Vec& a) noexcept {
        const float x=mul(a.v[0],a.v[0]),y=mul(a.v[1],a.v[1]),z=mul(a.v[2],a.v[2]);
        return add(add(x,y),z);
    }
    float scalarRoot(float x) noexcept {
        read(x);
        if((bits(x)&0x7f800000u)==0)return fp(bits(x)&0x80000000u);
        volatile float v=static_cast<float>(std::sqrt(static_cast<double>(x)));return read(v);
    }
    Vec normalize(const Vec& a) noexcept {
        // Same VMUL/VMULA/VMADDA/VMADD/VRSQRT/VMULq sequence retained49U.
        const float x=mul(a.v[0],a.v[0]),y=mul(a.v[1],a.v[1]),z=mul(a.v[2],a.v[2]);
        const float norm=add(add(x,y),mul(1.f,z));float q=0.f;
        if(norm!=0.f&&(bits(norm)&0x7f800000u)==0)valid=false;
        if(norm==0.f)q=fp((bits(norm)&0x80000000u)|0x7f7fffffu);
        else {volatile float root=std::sqrt(norm);q=div(1.f,read(root));}
        return {{mul(a.v[0],q),mul(a.v[1],q),mul(a.v[2],q),1.f}};
    }
    float dot(const Vec& a,const Vec& b) noexcept {
        const float x=mul(a.v[0],b.v[0]),y=mul(a.v[1],b.v[1]),z=mul(a.v[2],b.v[2]);
        return add(add(x,y),mul(1.f,z));
    }
    float minimum(float a,float b) noexcept {
        read(a);read(b);const auto x=static_cast<std::int32_t>(bits(a)),y=static_cast<std::int32_t>(bits(b));
        return fp(static_cast<std::uint32_t>((x<0&&y<0)?(x>y?x:y):(x<y?x:y)));
    }
    Vec cross(const Vec& a,const Vec& b) noexcept {
        Vec result{{0,0,0,0}};
        for(unsigned i=0;i<3;++i){const unsigned j=(i+1)%3,k=(i+2)%3;
            result.v[i]=sub(mul(a.v[j],b.v[k]),mul(a.v[k],b.v[j]));}
        return result;
    }
};
struct Slice {const char* p;std::size_t n,offset;};
char lower(char c) noexcept {return c>='A'&&c<='Z'?static_cast<char>(c+32):c;}
bool whitespace(char c) noexcept {return c==' '||c=='\t'||c=='\r';}
Slice trim(Slice s) noexcept {
    while(s.n&&whitespace(*s.p)){++s.p;--s.n;++s.offset;}
    while(s.n&&whitespace(s.p[s.n-1]))--s.n;return s;
}
bool equal(Slice s,const char* p) noexcept {
    if(s.n!=std::strlen(p))return false;
    for(std::size_t i=0;i<s.n;++i)if(lower(s.p[i])!=lower(p[i]))return false;return true;
}
bool prefix(Slice s,const char* p) noexcept {
    const auto n=std::strlen(p);if(s.n<n)return false;
    for(std::size_t i=0;i<n;++i)if(lower(s.p[i])!=lower(p[i]))return false;return true;
}
bool integer(Slice s,std::uint32_t& value) noexcept {
    if(!s.n||s.n>3)return false;std::uint32_t n=0;
    for(std::size_t i=0;i<s.n;++i){if(s.p[i]<'0'||s.p[i]>'9')return false;n=n*10+static_cast<unsigned>(s.p[i]-'0');}
    value=n;return true;
}
bool position(Slice s,float (&out)[4]) noexcept {
    std::size_t begin=0;
    for(unsigned i=0;i<3;++i){std::size_t end=begin;while(end<s.n&&s.p[end]!=',')++end;
        if((i<2&&end==s.n)||(i==2&&end!=s.n))return false;
        const auto part=trim({s.p+begin,end-begin,s.offset+begin});
        if(!rr::assets::parseTrackNumber50AD(part.p,part.n,out[i]))return false;begin=end+1;}
    out[3]=1.f;return true;
}
bool validTrack(const UndertrackTrack50ET& track) noexcept {
    if(!track.count||track.count>maxUndertrackNodes50ET)return false;
    for(std::size_t i=0;i<track.count;++i){const auto& n=track.nodes[i];
        if(n.nextCount>maxUndertrackNext50ET)return false;
        for(std::size_t j=0;j<n.nextCount;++j)if(n.next[j]>=track.count)return false;}
    return true;
}
}

bool readUndertrackTrack50ET(const char* text,std::size_t bytes,UndertrackReadScratch50ET& scratch,
    UndertrackTrack50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    const Span spans[]={{text,bytes},{&scratch,sizeof scratch},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(spans,4))return false;
    if(!text||!bytes)return fail(d,E::Input,0x2a1260);
    if(bytes>maxUndertrackText50ET)return fail(d,E::Limit,0x2a1260);
    for(std::size_t i=0;i<bytes;++i){const auto c=static_cast<unsigned char>(text[i]);
        if((c<32&&c!=9&&c!=10&&c!=13)||c>126)return fail(d,E::Syntax,0x2a1260,i);}
    scratch={};bool trackSeen=false,countSeen=false;int section=-2;std::size_t at=0;
    while(at<bytes){const auto begin=at;while(at<bytes&&text[at]!='\n')++at;
        auto line=trim({text+begin,at-begin,begin});if(at<bytes)++at;
        if(line.n>255)return fail(d,E::Limit,0x2a1260,line.offset);
        if(!line.n||line.p[0]==';')continue;
        if(line.p[0]=='['){
            if(line.n<3||line.p[line.n-1]!=']')return fail(d,E::Syntax,0x2a1260,line.offset);
            const auto name=trim({line.p+1,line.n-2,line.offset+1});
            if(equal(name,"Track")){if(trackSeen)return fail(d,E::Duplicate,0x2a1260,line.offset);trackSeen=true;section=-1;}
            else {std::uint32_t index=0;
                if(!prefix(name,"Node")||!integer({name.p+4,name.n-4,name.offset+4},index))return fail(d,E::Syntax,0x2a1260,line.offset);
                if(index>=maxUndertrackNodes50ET)return fail(d,E::Limit,0x2a1260,line.offset);
                if(scratch.seen[index])return fail(d,E::Duplicate,0x2a1260,line.offset);
                scratch.seen[index]=1;section=static_cast<int>(index);}
            continue;
        }
        if(section==-2)return fail(d,E::Syntax,0x2a1260,line.offset);
        std::size_t split=0;while(split<line.n&&line.p[split]!='=')++split;
        if(!split||split==line.n)return fail(d,E::Syntax,0x2a1260,line.offset);
        const auto key=trim({line.p,split,line.offset}),value=trim({line.p+split+1,line.n-split-1,line.offset+split+1});
        if(!key.n||!value.n)return fail(d,E::Syntax,0x2a1260,line.offset);
        if(section==-1){if(equal(key,"NumNodes")){
            if(countSeen)return fail(d,E::Duplicate,0x2a1260,line.offset);countSeen=true;
            if(!integer(value,scratch.track.count)||!scratch.track.count||scratch.track.count>maxUndertrackNodes50ET)
                return fail(d,E::Number,0x2a1260,value.offset);}
            continue;
        }
        const auto index=static_cast<unsigned>(section);auto& n=scratch.track.nodes[index];
        auto& fields=scratch.fields[index];
        if(equal(key,"Position")){
            if(fields&1)return fail(d,E::Duplicate,0x2a1260,line.offset);
            if(!position(value,n.position))return fail(d,E::Number,0x2a1260,value.offset);fields|=1;
        }else if(equal(key,"NumNextNodes")){
            if(fields&2)return fail(d,E::Duplicate,0x2a1260,line.offset);
            if(!integer(value,n.nextCount)||n.nextCount>maxUndertrackNext50ET)return fail(d,E::Limit,0x2a1260,value.offset);fields|=2;
        }else if(prefix(key,"Next")){
            std::uint32_t next=0,target=0;
            if(!integer({key.p+4,key.n-4,key.offset+4},next)||next>=maxUndertrackNext50ET)return fail(d,E::Limit,0x2a1260,key.offset);
            if(scratch.nextFields[index]&(1u<<next))return fail(d,E::Duplicate,0x2a1260,line.offset);
            if(!integer(value,target)||target>=maxUndertrackNodes50ET)return fail(d,E::Number,0x2a1260,value.offset);
            n.next[next]=target;scratch.nextFields[index]=static_cast<std::uint8_t>(scratch.nextFields[index]|(1u<<next));
        }
    }
    if(!trackSeen||!countSeen)return fail(d,E::Missing,0x2a1260,bytes);
    for(unsigned i=0;i<maxUndertrackNodes50ET;++i){
        if(i<scratch.track.count){const auto& n=scratch.track.nodes[i];
            if(!scratch.seen[i]||scratch.fields[i]!=3||scratch.nextFields[i]!=((1u<<n.nextCount)-1u))
                return fail(d,E::Missing,0x2a1260,bytes);
            for(unsigned j=0;j<n.nextCount;++j)if(n.next[j]>=scratch.track.count)return fail(d,E::Track,0x2a1260,bytes);
        }else if(scratch.seen[i])return fail(d,E::Limit,0x2a1260,bytes);
    }
    out=scratch.track;d={};return true;
}

bool selectUndertrackReposition50ET(const UndertrackTrack50ET& track,const UndertrackProgress50ET& progress,
    const UndertrackInput50ET& in,UndertrackSelection50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    const Span spans[]={{&track,sizeof track},{&progress,sizeof progress},{&in,sizeof in},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(spans,5))return false;
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x2255b0);
    if(!validTrack(track))return fail(d,E::Track,0x2a2850);
    const auto anchor=in.vehicle58?progress.vehicleC80:progress.vehicleC8C;
    if(anchor>=track.count)return fail(d,E::Track,0x225644);
    auto selected=progress.vehicleC88;
    UndertrackSelection50ET result{};result.anchorNode=anchor;result.writesC80=in.vehicle58!=0;
    if(in.vehicle58){if(!track.nodes[anchor].nextCount)return fail(d,E::Track,0x225634);
        selected=track.nodes[anchor].next[0];result.nextC80=selected;}
    else result.nextC80=progress.vehicleC80;
    if(selected>=track.count)return fail(d,E::Track,0x22564c);
    const auto& node=track.nodes[anchor];Math f;
    const Vec direction=f.normalize(f.difference(in.worldPosition,node.position));
    if(!f.valid)return fail(d,E::Arithmetic,0x2256dc);
    if(node.nextCount){
        float best=f.dot(direction,f.normalize(f.difference(track.nodes[selected].position,node.position)));
        if(!f.valid)return fail(d,E::Arithmetic,0x225750);
        for(unsigned i=0;i<node.nextCount;++i){const auto candidate=node.next[i];
            const float alignment=f.dot(direction,f.normalize(f.difference(track.nodes[candidate].position,node.position)));
            if(!f.valid)return fail(d,E::Arithmetic,0x225800);
            // Original C.LE.S skips replacement on equality. Preserve NextN order.
            if(!(alignment<=best)){selected=candidate;best=alignment;}
        }
    }
    const auto& next=track.nodes[selected];const Vec delta=f.difference(next.position,node.position);
    const float distance=f.scalarRoot(f.square(delta));
    const float fromSelected=f.scalarRoot(f.square(f.difference(next.position,in.localPosition)));
    if(!f.valid||distance==0.f)return fail(d,E::Arithmetic,0x225990);
    const float fraction=f.sub(1.f,f.minimum(1.f,f.div(fromSelected,distance)));
    for(unsigned i=0;i<3;++i)result.unusedInterpolatedPosition[i]=f.add(node.position[i],f.mul(fraction,delta.v[i]));
    result.unusedInterpolatedPosition[3]=1.f;
    const Vec heading=f.normalize(delta);std::memcpy(result.headingBD0,heading.v,16);
    for(unsigned i=0;i<3;++i)result.positionBC0[i]=f.add(node.position[i],i==1?in.lift7EC:0.f);
    result.positionBC0[3]=1.f;
    if(!f.valid)return fail(d,E::Arithmetic,0x225ac8);
    result.selectedNextNode=selected;std::memcpy(result.anchorPosition,node.position,16);
    out=result;d={};return true;
}

bool bindUndertrackPlacement50ET(const UndertrackSelection50ET& selection,
    const UndertrackTableBindings50ET& b,UndertrackPlacement50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    if(b.nodeCount>maxUndertrackNodes50ET||b.tableCount>256)return fail(d,E::Limit,0x225a38);
    const Span controls[]={{&selection,sizeof selection},{&b,sizeof b},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(controls,4))return false;
    if(!b.nodeIndex78||selection.anchorNode>=b.nodeCount)return fail(d,E::MissingNodeIndex,0x225a38);
    const auto index=b.nodeIndex78[selection.anchorNode];
    if(!b.application4C0||!b.application4C4||index>=b.tableCount)return fail(d,E::MissingApplicationTable,0x225a74);
    const Span arrays[]={{b.nodeIndex78,b.nodeCount},{b.application4C0,b.tableCount*sizeof(std::uint32_t)},
        {b.application4C4,b.tableCount*sizeof(std::uint32_t)}};
    // Read-only arrays may share backing; none may alias any output/control.
    for(const auto& a:arrays){const auto address=reinterpret_cast<std::uintptr_t>(a.p);
        if(a.bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
        for(const auto& c:controls)if(overlap(a.p,a.bytes,c.p,c.bytes))return false;}
    UndertrackPlacement50ET result{};
    std::memcpy(result.positionBC0,selection.positionBC0,16);std::memcpy(result.headingBD0,selection.headingBD0,16);
    result.vehicleC30=b.application4C0[index];result.vehicleC34=b.application4C4[index];result.index78=index;
    out=result;d={};return true;
}

void resetUndertrackPlayerPrefix50ET(UndertrackPlayerPrefix50ET& s) noexcept {
    s.counter109C=0;
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfbu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xefu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfeu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfdu);
}

void resetUndertrackController50ET(UndertrackController50ET& s) noexcept {
    s.field474=0;s.field4AC=0;
    s.flags4A4=static_cast<std::uint8_t>(s.flags4A4&0xfeu);
    s.flags4A4=static_cast<std::uint8_t>(s.flags4A4&0xfdu);
    s.field484=0x3f800000u;s.field490=0x3f800000u;
    s.field494=0;s.field498=0;s.field49C=0;s.field4A0=0;
    s.field4A5=3;s.field4A8=0;s.flags22C=static_cast<std::uint8_t>(s.flags22C&0xfbu);
}

bool resetUndertrackBasePrefix50ET(std::uint8_t* flags,std::uint32_t* field24C) noexcept {
    if(!flags)return true;
    if(field24C&&overlap(flags,1,field24C,sizeof(*field24C)))return false;
    //216A78 commits before216A7C reads D44 and216A80 calls2C2810.
    // A missing later owner stops that call; it cannot undo the D40 store.
    *flags=static_cast<std::uint8_t>(*flags&0xfbu);
    if(!field24C)return false;
    *field24C=0;return true;
}

void resetUndertrackCameraMotion50ET(UndertrackCameraMotion50ET& s,
    const std::uint32_t shared433DB0[4]) noexcept {
    std::memcpy(s.field180,shared433DB0,16);std::memcpy(s.field1A0,shared433DB0,16);
}

bool prepareUndertrackResetPose50ET(const float positionBC0[4],const float headingBD0[4],
    const float up434650[4],BodyMatrix48G& out,UndertrackDiagnostic50ET& d) noexcept {
    const Span spans[]={{positionBC0,16},{headingBD0,16},{up434650,16},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(spans,5))return false;
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x216af0);
    Math f;Vec heading{},up{};std::memcpy(heading.v,headingBD0,16);std::memcpy(up.v,up434650,16);
    const Vec initialRight=f.cross(up,heading),correctedUp=f.cross(heading,initialRight);
    const Vec normalizedUp=f.normalize(correctedUp),normalizedHeading=f.normalize(heading);
    const Vec right=f.cross(normalizedUp,normalizedHeading);BodyMatrix48G result{};
    for(unsigned i=0;i<3;++i){result.v[i]=right.v[i];result.v[4+i]=normalizedUp.v[i];
        result.v[8+i]=normalizedHeading.v[i];result.v[12+i]=f.read(positionBC0[i]);}
    result.v[15]=1.f;
    if(!f.valid)return fail(d,E::Arithmetic,0x28a090);
    out=result;d={};return true;
}

bool setUndertrackResetScene50ET(const SceneGraph49P& graph,SceneNodeIndex49P index,
    const BodyMatrix48G& pose,UndertrackSceneWorkspace50ET& w,UndertrackDiagnostic50ET& d,
    UndertrackSceneOrder50ET order) noexcept {
    if(!validateSceneGraph49P(graph))return fail(d,E::Scene,0x28a090);
    const auto i=static_cast<std::int32_t>(index);
    if(i<0||static_cast<std::uint32_t>(i)>=graph.count||graph.nodes[i].parent180!=SceneNodeIndex49P::none)
        return fail(d,E::Scene,0x28a090);
    const Span spans[]={{&graph,sizeof graph},{graph.nodes,graph.count*sizeof(SceneNode49P)},
        {&pose,sizeof pose},{&w,sizeof w},{&d,sizeof d}};
    if(!separate(spans,5))return false;
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x28a090);
    if(order!=UndertrackSceneOrder50ET::OrientationThenPosition&&order!=UndertrackSceneOrder50ET::PositionThenOrientation)
        return fail(d,E::Input,0x28a090);
    // Only admit the actual matrix domain produced by the source basis helper.
    for(float x:pose.v)if(!std::isfinite(x))return fail(d,E::Arithmetic,0x28a090);
    if(bits(pose.v[3])||bits(pose.v[7])||bits(pose.v[11])||bits(pose.v[15])!=0x3f800000u)
        return fail(d,E::Input,0x28a090);
    std::memcpy(w.nodes,graph.nodes,graph.count*sizeof(SceneNode49P));const SceneGraph49P staged{w.nodes,graph.count};
    if(order==UndertrackSceneOrder50ET::PositionThenOrientation){
        BodyMatrix48G translated=w.nodes[i].localC0;std::memcpy(translated.v+12,pose.v+12,12);
        if(!setLocalMatrix49P(staged,index,translated))return fail(d,E::Scene,0x28add0);
    }
    BodyMatrix48G orientation=pose;std::memcpy(orientation.v+12,w.nodes[i].localC0.v+12,12);
    if(!setLocalMatrix49P(staged,index,orientation))return fail(d,E::Scene,0x28a090);
    if(order==UndertrackSceneOrder50ET::OrientationThenPosition){
        BodyMatrix48G translated=w.nodes[i].localC0;std::memcpy(translated.v+12,pose.v+12,12);
        if(!setLocalMatrix49P(staged,index,translated))return fail(d,E::Scene,0x28add0);
    }
    std::memcpy(graph.nodes,w.nodes,graph.count*sizeof(SceneNode49P));d={};return true;
}

bool resetUndertrackMainBlock50ET(MainBodyState49K& main,MainSphereLanes49M& lanes,
    SecondaryActivationSupplement49V& supplement,std::uint32_t& active,
    ChariotLifecycleState49V& lifecycle,const SecondaryActivationGlobals49V& bodyGlobals,
    const ResetLifecycleGlobals50AT& lifecycleGlobals) noexcept {
    const Span spans[]={{&main,sizeof main},{&lanes,sizeof lanes},{&supplement,sizeof supplement},
        {&active,sizeof active},{&lifecycle,sizeof lifecycle},{&bodyGlobals,sizeof bodyGlobals},
        {&lifecycleGlobals,sizeof lifecycleGlobals}};
    if(!separate(spans,7))return false;
    auto stagedLifecycle=lifecycle;auto stagedActive=active;
    if(!resetLifecycleRecords50AT(stagedActive,stagedLifecycle,lifecycleGlobals))return false;
    resetMainRigidBody50AR(main,lanes,supplement,bodyGlobals);
    lifecycle=stagedLifecycle;active=stagedActive;return true;
}

void resetUndertrackPriorPoint50ET(MainBodyState49K& main,std::uint32_t& vehicle850,
    const std::uint32_t shared434630[4]) noexcept {
    std::memcpy(main.priorPoint7B0,shared434630,16);vehicle850=0;
}

void finishUndertrackControlHistory50ET(MainBodyState49K& main) noexcept {
    const std::uint32_t zero=0;
    std::memcpy(&main.controls.bound.control.shaping.smoothedChannel0D28,&zero,4);
}
}
