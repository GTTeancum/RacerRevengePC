#include "original_death_camera50kg.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using V=OriginalDeathVector50KG;using C=OriginalDeathCamera50KG;
using S=OriginalDeathCameraStatus50KG;using R=OriginalDeathCameraResult50KG;
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
bool scalar(float x) noexcept {return std::isfinite(x)&&(x==0||std::fabs(x)>=std::numeric_limits<float>::min());}
struct Math {
    bool ok=true;
    float keep(float x) noexcept {ok=ok&&scalar(x);return x;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0){ok=false;return 0;}volatile float x=a/b;return keep(x);}
    V add(V a,V b) noexcept {return {add(a.x,b.x),add(a.y,b.y),add(a.z,b.z),1};}
    V sub(V a,V b) noexcept {return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z),1};}
    V scale(V a,float s) noexcept {return {mul(s,a.x),mul(s,a.y),mul(s,a.z),1};}
    float dot(V a,V b) noexcept {return add(add(mul(a.x,b.x),mul(a.y,b.y)),mul(a.z,b.z));}
    float length(V a) noexcept {volatile float x=std::sqrt(dot(a,a));return keep(x);}
    V normalizeScalar(V a,const V& zero) noexcept {
        const float n=dot(a,a);if(n==0)return zero;
        volatile float root=std::sqrt(n);const float w=a.w;a=scale(a,div(1,keep(root)));a.w=w;return a;
    }
    // The recovered VU inline normalize sequence uses max-finite Q on zero.
    // This is native straight-line arithmetic, never guest instruction execution.
    V normalizeInline(V a) noexcept {
        const float n=dot(a,a);float q=std::numeric_limits<float>::max();
        if(n!=0){volatile float root=std::sqrt(std::fabs(n));q=div(1,keep(root));}
        const float w=add(a.w,0);a=scale(a,q);a.w=w;return a;
    }
    V cross(V a,V b) noexcept {return {sub(mul(a.y,b.z),mul(a.z,b.y)),
        sub(mul(a.z,b.x),mul(a.x,b.z)),sub(mul(a.x,b.y),mul(a.y,b.x)),0};}
    float filter(OriginalFollowFilter& f,float target,float dt) noexcept {
        keep(dt);keep(f.tau);f.blend=div(detail::scalarMin48O(dt,f.tau),f.tau);
        f.value=add(f.value,mul(f.blend,sub(target,f.value)));return f.value;
    }
};
R fail(R r,S s,std::uint32_t pc) noexcept {r.status=s;r.sourcePC=pc;return r;}
bool identity(const OriginalDeathCameraIdentity50KG& a,const OriginalDeathCameraIdentity50KG& b) noexcept {
    return a.sessionGeneration&&a.racerKey&&a.mainBodyKey&&a.death68Key&&
        a.sessionGeneration==b.sessionGeneration&&a.racerKey==b.racerKey&&
        a.mainBodyKey==b.mainBodyKey&&a.death68Key==b.death68Key&&a.body288==b.body288;
}
bool subject(const C& c,const OriginalDeathCameraSubject50KG& s) noexcept {
    const auto i=static_cast<std::int32_t>(s.identity.body288);
    return c.prepared&&identity(c.identity,s.identity)&&s.graph&&s.graph->nodes&&i>=0&&
        static_cast<std::uint32_t>(i)<s.graph->count&&validateSceneGraph49P(*s.graph)&&
        c.revision!=std::numeric_limits<std::uint64_t>::max();
}
bool body(const OriginalDeathCameraSubject50KG& s,V& pos,V& up) noexcept {
    if(!refreshSceneWorld49P(*s.graph,s.identity.body288))return false;
    const auto& a=s.graph->nodes[static_cast<std::uint32_t>(s.identity.body288)].world100.v;
    pos={a[12],a[13],a[14],1};up={a[4],a[5],a[6],1};return true;
}
SpringVector4_48N spring(V v) noexcept {return {{v.x,v.y,v.z,v.w}};}
V vector(const SpringVector4_48N& v) noexcept {return {v.v[0],v.v[1],v.v[2],v.v[3]};}

struct Span {std::uintptr_t first=0,last=0;};
bool span(const void* p,std::size_t count,std::size_t width,Span& out) noexcept {
    out={};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),n=count*width;
    if(n>std::numeric_limits<std::uintptr_t>::max()-a)return false;out={a,a+n};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
// Guard all native caller-visible mutation against the concrete query owners,
// then all borrowed ordered debris data. No pointer can stand in for a hit.
bool storage(C& c,const OriginalDeathCameraSubject50KG& subject,
    const OriginalDeathCameraDebrisView50KG* debrisView,OriginalDeathCameraGlobals50KG* globals,
    OriginalDeathCameraQueryOwner& q,TVCameraUpdateOutput49F* output) noexcept {
    if(!subject.graph)return false;
    OriginalCameraOwnerSpan writes[4]={{&c,sizeof c},{subject.graph->nodes,
        sizeof(SceneNode49P)*subject.graph->count},{globals,globals?sizeof *globals:0},{output,output?sizeof *output:0}};
    Span writable[4];for(unsigned i=0;i<4;++i){if(!span(writes[i].data,writes[i].bytes,1,writable[i]))return false;
        for(unsigned j=0;j<i;++j)if(overlap(writable[i],writable[j]))return false;}
    if(!admitOriginalDeathCameraQueryStorage(q,writes,4,nullptr,0))return false;
    auto read=[&](const void* p,std::size_t count,std::size_t width) noexcept {
        Span s;if(!span(p,count,width,s))return false;
        for(const auto& w:writable)if(overlap(s,w))return false;
        const OriginalCameraOwnerSpan borrowed{p,count*width};
        return admitOriginalDeathCameraQueryStorage(q,nullptr,0,&borrowed,1);
    };
    if(!read(&subject,1,sizeof subject)||!read(subject.graph,1,sizeof *subject.graph))return false;
    if(!debrisView)return true;
    if(debrisView->count370>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))return false;
    if(!read(debrisView,1,sizeof *debrisView)||
       !read(debrisView->collections374,debrisView->count370,sizeof(OriginalDeathPieceCollection50KG)))return false;
    for(std::size_t i=0;i<debrisView->count370;++i){const auto& collection=debrisView->collections374[i];
        if(collection.count30>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
           !read(collection.pieces38,collection.count30,sizeof(OriginalDeathPiece50KG)))return false;
        for(std::size_t j=0;j<collection.count30;++j){const auto& piece=collection.pieces38[j];
            if(!read(piece.flags29,1,sizeof(std::uint8_t)))return false;
            if(!(*piece.flags29&1u))continue;
            if(!piece.graph||!read(piece.graph,1,sizeof *piece.graph))return false;
            // A genuine shared scene forest is legal; only exact span identity
            // receives this exemption, not a partially overlapping allocation.
            if(piece.graph->nodes==subject.graph->nodes&&piece.graph->count==subject.graph->count)continue;
            if(!read(piece.graph->nodes,piece.graph->count,sizeof(SceneNode49P)))return false;
        }
    }
    return true;
}

bool debris(C& c,const OriginalDeathCameraDebrisView50KG& d,float dt,Math& m,R& r) noexcept {
    if(d.sessionGeneration!=c.identity.sessionGeneration||d.racerKey!=c.identity.racerKey||
       d.count370>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
       (d.count370&&!d.collections374))return false;
    V sum{0,0,0,1};float count=0;
    for(std::size_t i=0;i<d.count370;++i){const auto& col=d.collections374[i];
        if(col.count30>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
           (col.count30&&!col.pieces38))return false;
        for(std::size_t j=0;j<col.count30;++j){const auto& p=col.pieces38[j];
            if(!p.flags29)return false;
            if(!(*p.flags29&1u))continue;
            const auto k=static_cast<std::int32_t>(p.model14C);
            if(!p.graph||!p.graph->nodes||k<0||static_cast<std::uint32_t>(k)>=p.graph->count||
               !validateSceneGraph49P(*p.graph))return false;
            //28B180 is LOCAL F0: do not refresh/world-transform fragment nodes.
            const auto& a=p.graph->nodes[static_cast<std::uint32_t>(k)].localC0.v;
            const V delta=m.sub(V{a[12],a[13],a[14],1},c.debris110);
            const float length=m.length(delta);
            if(length<m.keep(c.radius7C)){
                const float weight=m.sub(1,m.div(length,c.radius7C));count=m.add(count,1);
                sum=m.add(sum,m.add(c.debris110,m.scale(delta,weight)));++r.debrisAccepted;
            }
        }
    }
    //22D6C8 has no count-zero branch. Native finite admission rejects it.
    const V next=m.scale(sum,m.div(1,count));if(!m.ok)return false;
    c.debris110=next;r.prefixCommitted=true;
    if(c.flags14CLow2&1u){m.filter(c.debris13C[0],next.x,dt);
        m.filter(c.debris13C[1],next.y,dt);m.filter(c.debris13C[2],next.z,dt);}
    return m.ok;
}
V target(C& c,V p,OriginalDeathCameraGlobals50KG& g,Math& m) noexcept {
    V out=p;out.y=m.add(out.y,3);
    if(c.mode148==3){if(!g.lazy8804){g.targetHeight4348D0=15;g.lazy8804=true;}
        out.y=m.add(out.y,g.targetHeight4348D0);
    }else if(c.mode148==4){
        if(c.flags14CLow2&2u)out=c.targetE0;
        else if(c.started50){V d=m.sub(c.targetE0,p);d.y=0;const float n=m.length(d);
            if(!g.lazy8808){g.lockDistance4348D8=bits(0x3f8ccccdu);g.lazy8808=true;}
            if(n<=m.keep(g.lockDistance4348D8)){out=c.targetE0;c.flags14CLow2|=2u;}
        }
    }
    return out;
}
float fov(C& c,V p,float dt,Math& m) noexcept {
    if(c.mode148==2||c.mode148==4){const float distance=m.length(m.sub(c.initial80,p));float f;
        if(m.keep(c.field70)<=m.keep(c.fov64)){
            if(distance<m.keep(c.distance68))f=c.field70;
            else{f=m.sub(c.fov64,m.mul(m.sub(c.fov64,c.minFov14),
                m.div(m.sub(c.distance68,distance),m.sub(c.distance18,c.distance1C))));
                if(f<c.minFov14)f=c.minFov14;if(!(f<=c.fov64))f=c.fov64;}
        }else{f=m.add(c.fov64,m.mul(m.sub(c.fov10,c.fov64),
                m.div(m.sub(c.distance18,distance),m.sub(c.distance18,c.distance1C))));
            if(f<c.fov64)f=c.fov64;if(!(f<=c.fov10))f=c.fov10;}
        if(m.ok)c.field70=f;return f;
    }
    return m.filter(c.fov12C,c.fov64,dt);
}
//22F560: each call below owns11CA80/11FF60/121920 on the ONE actual ray.
// The original loop has no iteration cap. Budget exhaustion is an explicit
// native capacity failure retaining all already completed query effects.
bool ray(OriginalDeathCameraQueryOwner& q,V a,V b,std::size_t budget,R& r,
    bool& hit,V& normal,V& point) noexcept {
    if(r.queriesCommitted>=budget){r.status=S::capacity;r.sourcePC=0x22f724;return false;}
    OriginalDeathCameraQueryResult qr{};
    const auto status=updateOriginalDeathCameraQuery120(q,spring(a),spring(b),qr);
    r.queriesCommitted+=qr.queriesCommitted;r.prefixCommitted=r.prefixCommitted||qr.updatesCommitted!=0;
    if(status!=OriginalDeathCameraQueryStatus::Complete){r.status=S::query;r.sourcePC=qr.sourcePC;return false;}
    hit=(q.state->query120.flagsF8&0x20u)!=0;
    if(hit){normal=vector(q.state->query120.type3.selectedNormal);point=vector(q.state->query120.type3.selectedPoint);}
    return true;
}
V slide(C& c,V& start,V end,V normal,V point,float clearance,bool subtractClearance,Math& m) noexcept {
    start=m.add(point,m.scale(normal,clearance));V d=m.sub(end,start);
    float length=m.sub(m.length(d),m.length(m.sub(point,start)));
    const float component=m.dot(d,normal);d=m.normalizeInline(m.sub(d,m.scale(normal,component)));
    if(subtractClearance){length=m.sub(length,clearance);c.velocity90=d;}
    return m.add(start,m.scale(d,length));
}
bool collision(C& c,OriginalDeathCameraQueryOwner& q,V start,V wanted,std::size_t budget,
    V& output,Math& m,R& r,const V& zero) noexcept {
    constexpr float clearance=5;
    const V direction=m.normalizeScalar(m.sub(wanted,start),zero);
    const float distance=m.length(m.sub(wanted,start));
    const V extended=m.add(start,m.scale(direction,m.add(distance,clearance)));
    V a=start,b=extended;
    if(!m.ok){r.status=S::arithmetic;r.sourcePC=0x22f6b4;return false;}
    for(;;){bool hit=false;V normal{},point{};
        if(!ray(q,a,b,budget,r,hit,normal,point))return false;
        if(hit){
            if(c.started50&&c.mode148==3){output=start;return true;}
            b=slide(c,a,b,normal,point,clearance,c.started50&&(c.mode148==2||c.mode148==4),m);
            if(!m.ok){r.status=S::arithmetic;r.sourcePC=0x22f8a0;return false;}
            continue;
        }
        if(!c.started50){a=start;
            if(!ray(q,a,b,budget,r,hit,normal,point))return false;
            if(hit){b=slide(c,a,b,normal,point,clearance,false,m);
                if(!m.ok){r.status=S::arithmetic;r.sourcePC=0x230010;return false;}
                output=b;return true;}
        }
        output=(b.x==extended.x&&b.y==extended.y&&b.z==extended.z)?wanted:b;return true;
    }
}

bool position(C& c,V p,V aim,V velocity,V up,V bodyPosition,float dt,
    OriginalDeathCameraGlobals50KG& g,OriginalDeathCameraQueryOwner& q,std::size_t budget,
    V& out,Math& m,R& r) noexcept {
    if(c.terrain60Present){r.status=S::terrainStub;r.sourcePC=0x2e83d0;return false;}
    const bool initial=!c.started50;
    V proposed{};
    if(c.mode148==2||c.mode148==4){
        if(initial){const V side=m.normalizeInline(m.cross(velocity,up));const float speed=m.length(velocity);
            float scale=c.mode148==2?bits(0x3f4ccccdu):bits(0x3e99999au);
            const float sideways=c.mode148==2?-18.f:-70.f;
            if(!(speed<1000.f))scale=c.mode148==2?bits(0x3ee66666u):bits(0x3dcccccdu);
            else if(!(speed<=600.f)){float f=m.div(m.sub(1000,speed),400);
                //22E3E4/22E400 really use MIN(0,f), then MAX(f,1).
                // Retain the source order; do not repair it into a clamp.
                f=detail::scalarMin48O(0,f);f=detail::scalarMax48O(f,1);
                scale=m.sub(scale,m.mul(c.mode148==2?bits(0x3eb33333u):bits(0x3dcccccdu),m.sub(1,f)));}
            proposed=m.add(m.add(p,m.scale(velocity,scale)),m.scale(side,sideways));
        }else{
            //22E69C computes speed even though the result is never consumed.
            (void)m.length(velocity);
            if(!(m.keep(c.scale74)<=0))c.scale74=m.filter(c.speed128,0,dt);
            proposed=m.add(c.positionB0,m.scale(c.velocity90,c.scale74));
        }
    }else if(c.mode148==3){
        if(initial){if(!g.lazy87FC){g.displacement4348C0=-65;g.lazy87FC=true;}
            V direction=c.velocity90;if(std::fabs(m.keep(direction.x))<1)direction.x=1;
            if(std::fabs(m.keep(direction.z))<1)direction.z=1;
            direction=m.normalizeScalar(direction,g.zero434880);
            proposed=m.add(p,m.scale(direction,g.displacement4348C0));
            if(!g.lazy8800){g.height4348C8=20;g.lazy8800=true;}
            proposed.y=m.add(proposed.y,g.height4348C8);
        }else{
            if(!(m.keep(c.scale74)<=0))c.scale74=m.filter(c.speed128,0,dt);
            proposed=m.add(c.positionB0,m.scale(V{-c.lookA0.x,-c.lookA0.y,-c.lookA0.z,1},c.scale74));
            c.lookA0.y=m.add(c.lookA0.y,bits(0x3c23d70au));
            c.lookA0=m.normalizeScalar(c.lookA0,g.zero434880);
            c.lookA0.y=m.filter(c.direction130[1],c.lookA0.y,dt);
        }
    }else{r.status=S::sourcePath;r.sourcePC=0x22e2bc;return false;}
    if(!m.ok){r.status=S::arithmetic;r.sourcePC=0x22e260;return false;}
    if(!collision(c,q,initial?bodyPosition:c.positionB0,proposed,budget,out,m,r,g.zero434880))return false;
    if(initial){c.initial80=out;
        if(c.mode148==2||c.mode148==4)c.velocity90=m.normalizeScalar(c.velocity90,g.zero434880);
        else{c.lookA0=m.normalizeScalar(m.sub(aim,out),g.zero434880);
            c.direction130[0].value=c.lookA0.x;c.direction130[1].value=c.lookA0.y;c.direction130[2].value=c.lookA0.z;}
        if(!m.ok){r.status=S::arithmetic;r.sourcePC=0x22f248;return false;}
        c.started50=true;
    }
    //22F51C saves position before22DFD0; null terrain60 returns true unchanged.
    c.positionB0=out;r.prefixCommitted=true;return true;
}
} // namespace

R prepareOriginalDeathCamera50KG(C& c,const OriginalDeathCameraIdentity50KG& id,
    const OriginalDeathCameraGlobals50KG& g) noexcept {
    R r;if(c.prepared||!id.sessionGeneration||!id.racerKey||!id.mainBodyKey||!id.death68Key||
       id.body288==SceneNodeIndex49P::none)return fail(r,S::owner,0x22cf20);
    C n=c;n.identity=id;n.fov0C=80;n.bodyScale20=0;n.targetHeight24=3;n.started50=false;
    n.terrain60Present=false;n.initial80=n.positionB0=n.previousC0=n.fieldD0=n.targetE0=V{0,0,0,1};
    n.velocity90=n.lookA0=n.fieldF0=n.field100=n.debris110=g.zero434880;
    n.scale74=1;n.field78=75;n.radius7C=100;n.mode148=2;n.flags14CLow2=0;
    //174F00..174F38 arguments,22D1C0 reorder into286D00.
    n.fov10=80;n.minFov14=40;n.distance18=20;n.distance1C=100;
    n.fov64=60;n.distance68=60;n.clearance6C=2;
    n.speed128={1,2.5f,1};n.fov12C={80,3.5f,1};
    for(auto& f:n.direction130)f={0,.5f,1};
    for(auto& f:n.debris13C)f={0,bits(0x3e99999au),1};
    n.prepared=true;n.active=false;n.revision=1;c=n;return r;
}
R beginOriginalDeathCamera22D810(C& c,const OriginalDeathCameraSubject50KG& s) noexcept {
    R r;if(!subject(c,s))return fail(r,S::owner,0x22d810);
    if(s.racerState40!=3&&s.racerState40!=4)return fail(r,S::sourcePath,0x22d978);
    if(std::fegetround()!=FE_TONEAREST)return fail(r,S::arithmetic,0x286d20);
    Math m;const float scale=m.mul(bits(0x3fbbbb84u),70);
    if(!m.ok)return fail(r,S::arithmetic,0x286d30);
    c.bodyScale20=scale;c.targetHeight24=3;r.prefixCommitted=true;return r;
}
R selectOriginalDeathCamera22D810(C& c,const OriginalDeathCameraSubject50KG& s,
    OriginalDeathCameraQueryOwner& q) noexcept {
    R r;if(!subject(c,s))return fail(r,S::owner,0x22d810);
    if(s.racerState40!=3&&s.racerState40!=4)return fail(r,S::sourcePath,0x22d978);
    // No raw guest identity and no caller boolean can substitute for this owner.
    if(!originalDeathCameraQueryAttached(q,c.identity.death68Key))return fail(r,S::query,0x22d964);
    if(!storage(c,s,nullptr,nullptr,q,nullptr))return fail(r,S::owner,0x22d810);
    if(std::fegetround()!=FE_TONEAREST)return fail(r,S::arithmetic,0x286d20);
    V p{},up{};if(!body(s,p,up))return fail(r,S::graph,0x22d924);
    if(s.racerState40==3){c.mode148=s.probe118?4:2;c.flags14CLow2|=1u;
        c.speed128={s.probe118?0.f:bits(0x3f59999au),bits(0x404ccccdu),1};
        c.fov12C={c.fov10,3.5f,1};
        if(s.probe118){c.targetE0=p;c.flags14CLow2=static_cast<std::uint8_t>(c.flags14CLow2&~2u);}
    }else{c.mode148=3;c.flags14CLow2=static_cast<std::uint8_t>(c.flags14CLow2&~1u);
        c.speed128={3,1,1};c.fov12C={c.fov64,1,1};}
    c.velocity90=s.bodyVelocity70;c.debris110=p;c.scale74=1;
    if(c.flags14CLow2&1u){c.debris13C[0].value=p.x;c.debris13C[1].value=p.y;c.debris13C[2].value=p.z;}
    r.prefixCommitted=true;
    OriginalDeathCameraQueryResult qr{};
    if(refreshOriginalDeathCameraQuery120(q,qr)!=OriginalDeathCameraQueryStatus::Complete)
        return fail(r,S::query,qr.sourcePC);
    c.started50=false;c.active=true;++c.revision;return r;
}
R advanceOriginalDeathCamera22DC90(C& c,const OriginalDeathCameraSubject50KG& s,
    const OriginalDeathCameraDebrisView50KG& d,float dt,OriginalDeathCameraGlobals50KG& globals,
    OriginalDeathCameraQueryOwner& q,TVCameraUpdateOutput49F& out,std::size_t budget) noexcept {
    R r;if(!subject(c,s)||!c.active)return fail(r,S::owner,0x22dc90);
    if(c.mode148!=2&&c.mode148!=3&&c.mode148!=4)return fail(r,S::sourcePath,0x22e2bc);
    if(std::fegetround()!=FE_TONEAREST||!scalar(dt)||dt<0)return fail(r,S::arithmetic,0x22dcbc);
    if(!originalDeathCameraQueryAttached(q,c.identity.death68Key))
        return fail(r,S::query,0x22f724);
    if(!storage(c,s,&d,&globals,q,&out))return fail(r,S::owner,0x22dc90);
    Math m;
    if(!debris(c,d,dt,m,r))return fail(r,m.ok?S::owner:S::arithmetic,m.ok?0x22d4ec:0x22d6c8);
    V bodyPosition{},up{};if(!body(s,bodyPosition,up))return fail(r,S::graph,0x22dd18);
    const V p=c.debris110;
    const V velocity=c.started50?m.scale(m.sub(p,c.previousC0),m.div(1,dt)):c.velocity90;
    if(!m.ok)return fail(r,S::arithmetic,0x22dd70);
    c.previousC0=p;V aim=target(c,p,globals,m);c.targetE0=aim;r.prefixCommitted=true;
    if(!m.ok)return fail(r,S::arithmetic,0x2301e0);
    V camera{};if(!position(c,p,aim,velocity,up,bodyPosition,dt,globals,q,budget,camera,m,r))return r;
    const float fieldOfView=fov(c,p,dt,m);
    if(!m.ok)return fail(r,S::arithmetic,0x22e090);
    //22DEB0/22DEE4/22DF14 publish before the optional up calculation; FOV is
    //last at22DF98. Retain this genuine output prefix on arithmetic rejection.
    out.position=camera;
    if(c.mode148==2||c.mode148==4){out.target=aim;out.mode=1;out.up=globals.up4348A0;}
    else{out.look=c.lookA0;out.mode=0;
        const V upResult=m.normalizeInline(m.cross(m.cross(out.look,globals.up4348A0),out.look));
        if(!m.ok)return fail(r,S::arithmetic,0x22df34);out.up=upResult;}
    out.fov=fieldOfView;++c.revision;r.poseCommitted=true;return r;
}
} // namespace rr::game
