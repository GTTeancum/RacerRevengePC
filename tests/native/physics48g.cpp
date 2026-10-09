#include "../../src/game/vehicle_contact48g.h"
#include "../../tools/gameplay/contact_oracle48g.h"
#include <algorithm>

static constexpr std::uint32_t bodyAddress=0x400,terrainAddress=0x800,sceneAddress=0x1000;
static constexpr std::uint32_t coveragePc[]={0x1b1de8,0x1b1b48,0x1b1ad8,0x1b1e28,0x1b1eb8,0x1b20a8,
    0x1b210c,0x1b22cc,0x1b230c,0x1b23c0,0x1b2278,0x1b238c};
static void put(Original48G&m,const TerrainFollowState48G&s,const TerrainFollowParameters48G&p,
    const TerrainSurface48G&t,const TerrainFollowBody48G&b) {
    m.write(terrainAddress+0x128,bodyAddress);m.write(bodyAddress+0x288,sceneAddress);
    m.write(terrainAddress+0x104,s.active104);m.write(terrainAddress+0x10c,s.pending10c);m.write(terrainAddress+0x11c,s.detached11c);
    m.write(terrainAddress+0x108,t.forceAngleTest108);m.write(terrainAddress+0x110,t.hit110);m.write(terrainAddress+0x114,t.hit114);
    m.store(terrainAddress+0x150,t.point150);m.store(terrainAddress+0x170,t.normal170);
    m.store(terrainAddress+0x180,s.previousPoint180);m.store(terrainAddress+0x190,s.previousNormal190);
    m.store(terrainAddress+0x1c0,p.localOffset1c0);m.store(terrainAddress+0x1d0,s.largestVelocityChange1d0);
    m.store(terrainAddress+0x204,p.airborneTorque204);m.store(terrainAddress+0x208,p.detachAngle208);m.store(terrainAddress+0x20c,p.detachDistance20c);
    m.store(bodyAddress+0x60,b.position60);m.store(bodyAddress+0x70,b.worldVelocity70);m.store(bodyAddress+0x80,b.cachedLocalVelocity80);
    m.store(bodyAddress+0xe8,b.angleE8);m.store(bodyAddress+0x180,b.loads.force);m.store(bodyAddress+0x190,b.loads.torque);
    for(unsigned i=0;i<3;++i) {m.store(sceneAddress+0xc0+i*16,b.scene.basis.columns[i]);m.store(sceneAddress+0x100+i*16,b.scene.basis.columns[i]);}
    m.store(sceneAddress+0xf0,b.scene.position);m.store(sceneAddress+0x130,b.scene.position);
    m.store(sceneAddress+0xfc,1.0f);m.store(sceneAddress+0x13c,1.0f);m.write(sceneAddress+0x190,1);
    // Ready root scene, no child-notification graph. Original numeric scene
    // setters and cache refresh still execute their actual instructions.
    m.write(sceneAddress+0x180,0);m.write(sceneAddress+0x184,0);
    m.g[28][0]=0x9600;m.write(0x1c40,1);m.write(0x1c44,1);m.write(0x1c48,1);m.write(0x222c,1);
}
static void run(Original48G&m,std::array<bool,12>&coverage) {
    m.steps=0;m.g[4][0]=terrainAddress;m.g[29][0]=0x3000;m.g[31][0]=0xf00df00d;m.v[0][3]=1;
    auto pc=rr_terrain_48g_start;
    while(pc!=0xf00df00d) {
        for(unsigned i=0;i<coverage.size();++i)if(pc==coveragePc[i])coverage[i]=true;
        std::uint32_t target=0,unused=0;
        if(m.execute(pc,target)) {
            if(!m.annulDelay) {
                for(unsigned i=0;i<coverage.size();++i)if(pc+4==coveragePc[i])coverage[i]=true;
                if(m.execute(pc+4,unused))throw std::runtime_error("branch in delay slot");
            }
            pc=target;
        } else pc+=4;
    }
    if(m.g[29][0]!=0x3000)throw std::runtime_error("terrain oracle did not truly return");
}
static TerrainFollowResult48G get(const Original48G&m) {
    TerrainFollowResult48G r{};auto&s=r.state;auto&b=r.body;
    s.active104=m.read(terrainAddress+0x104);s.pending10c=m.read(terrainAddress+0x10c);s.detached11c=m.read(terrainAddress+0x11c);
    s.previousPoint180=m.vec(terrainAddress+0x180);s.previousNormal190=m.vec(terrainAddress+0x190);s.largestVelocityChange1d0=m.vec(terrainAddress+0x1d0);
    b.position60=m.vec(bodyAddress+0x60);b.worldVelocity70=m.vec(bodyAddress+0x70);b.cachedLocalVelocity80=m.vec(bodyAddress+0x80);
    b.angleE8=m.readFloat(bodyAddress+0xe8);b.loads={m.vec(bodyAddress+0x180),m.vec(bodyAddress+0x190)};
    for(unsigned i=0;i<3;++i)b.scene.basis.columns[i]=m.vec(sceneAddress+0xc0+i*16);
    b.scene.position=m.vec(sceneAddress+0xf0);return r;
}
static float saved(const std::uint32_t*data,unsigned o){return fp(data[o/4]);}
static Vector3_48C savedVec(const std::uint32_t*data,unsigned o){return {{saved(data,o),saved(data,o+4),saved(data,o+8)}};}
int main() {
    std::uint64_t checks=0;std::array<bool,12>coverage{};unsigned caseNumber=0;int mode=0;
    auto require=[&](bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);};
    auto equal=[&](const void*actual,const void*expected,unsigned n,const char*why){
        const auto*a=static_cast<const unsigned char*>(actual);const auto*b=static_cast<const unsigned char*>(expected);
        for(unsigned i=0;i<n;++i){std::uint32_t x,y;std::memcpy(&x,a+i*4,4);std::memcpy(&y,b+i*4,4);++checks;
            if(x!=y)throw std::runtime_error(std::string(why)+" lane="+std::to_string(i)+" got="+std::to_string(x)+" expected="+std::to_string(y)+" case="+std::to_string(caseNumber)+" round="+std::to_string(mode));}
    };
    auto compare=[&](const TerrainFollowState48G&s,const TerrainFollowParameters48G&p,const TerrainSurface48G&t,const TerrainFollowBody48G&b){
        ++caseNumber;Original48G m;put(m,s,p,t,b);run(m,coverage);const auto expected=get(m);TerrainFollowResult48G actual{};
        require(applyTerrainFollowing48G(s,p,t,b,actual),"finite terrain response rejected");
        equal(&actual.state,&expected.state,12,"terrain state");equal(&actual.body,&expected.body,28,"terrain body");return actual;
    };
    auto sv=[](unsigned o){return savedVec(rr_saved_terrain_48g,o);};
    TerrainFollowState48G savedState{rr_saved_terrain_48g[0x104/4],rr_saved_terrain_48g[0x10c/4],rr_saved_terrain_48g[0x11c/4],sv(0x180),sv(0x190),sv(0x1d0)};
    TerrainFollowParameters48G p{sv(0x1c0),saved(rr_saved_terrain_48g,0x204),saved(rr_saved_terrain_48g,0x208),saved(rr_saved_terrain_48g,0x20c)};
    TerrainSurface48G surface{rr_saved_terrain_48g[0x108/4],rr_saved_terrain_48g[0x110/4],rr_saved_terrain_48g[0x114/4],sv(0x150),sv(0x170)};
    TerrainFollowBody48G body{};body.position60=savedVec(rr_saved_body_48g,0x60);body.worldVelocity70=savedVec(rr_saved_body_48g,0x70);
    body.cachedLocalVelocity80=savedVec(rr_saved_body_48g,0x80);body.angleE8=saved(rr_saved_body_48g,0xe8);
    body.loads={savedVec(rr_saved_body_48g,0x180),savedVec(rr_saved_body_48g,0x190)};
    for(unsigned i=0;i<3;++i)body.scene.basis.columns[i]=savedVec(rr_saved_scene_48g,0x100+i*16);
    body.scene.position=savedVec(rr_saved_scene_48g,0x130);
    std::uint32_t rng=0x48a1b1a3u;
    auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
    auto random=[&](float div){return static_cast<float>(static_cast<int>(next()&65535)-32768)/div;};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            mode=rounding;require(std::fesetround(rounding)==0,"rounding setup");compare(savedState,p,surface,body);
            for(unsigned n=0;n<1400;++n) {
                TerrainFollowState48G s{};auto b=body;auto t=surface;auto parameters=p;
                s.active104=next()%3;s.pending10c=next()%3;s.detached11c=next()%3;
                t.hit110=next()%3;t.hit114=next()%3;t.forceAngleTest108=next()%3;
                parameters.detachAngle208=random(16384);parameters.detachDistance20c=random(8192);
                parameters.airborneTorque204=random(2);b.angleE8=random(32768);
                for(unsigned i=0;i<3;++i) {
                    b.position60.v[i]=random(8);b.scene.position.v[i]=random(8);t.point150.v[i]=random(8);
                    b.worldVelocity70.v[i]=random(128);b.cachedLocalVelocity80.v[i]=random(128);
                    b.loads.force.v[i]=random(2);b.loads.torque.v[i]=random(32);
                    s.previousPoint180.v[i]=n%3 ? random(8):0;s.previousNormal190.v[i]=n%3 ? random(32768):0;
                    s.largestVelocityChange1d0.v[i]=n%5 ? random(128):0;t.normal170.v[i]=random(32768);
                    parameters.localOffset1c0.v[i]=random(1024);
                    for(unsigned j=0;j<3;++j)b.scene.basis.columns[i].v[j]=random(32768);
                }
                if(n%6==0){b.worldVelocity70={{0,0,0}};b.cachedLocalVelocity80={{0,0,0}};}
                if(n%11==0)t.normal170={{0,0,0}};
                compare(s,parameters,t,b);
            }
            // Explicit normal impact, tangent-speed preservation, force/velocity
            // reverse clipping, and crest detach/recontact branch boundaries.
            auto b=body;TerrainFollowState48G s{};TerrainSurface48G t{0,0,1,{{0,0,0}},{{0,1,0}}};
            b.scene.basis.columns[0]={{1,0,0}};b.scene.basis.columns[1]={{0,1,0}};b.scene.basis.columns[2]={{0,0,1}};
            b.scene.position={{0,8,0}};b.position60={{0,8,0}};
            for(float y:{-100.0f,-1.0f,0.0f,1.0f,100.0f}) {
                b.worldVelocity70={{1,y,2}};b.cachedLocalVelocity80={{1,y,2}};compare(s,p,t,b);
                b.cachedLocalVelocity80.v[2]=-2;b.loads.force={{0,-32.2f,-100}};compare(s,p,t,b);
            }
            // State chaining with queried surfaces held fixed exercises history
            // and pending flags, not a fabricated full driving simulation.
            for(unsigned n=0;n<64;++n){auto r=compare(s,p,t,b);s=r.state;b=r.body;}
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
        for(unsigned i=0;i<coverage.size();++i){if(!coverage[i])std::cout<<"uncovered PC "<<std::hex<<coveragePc[i]<<std::dec<<'\n';require(coverage[i],"terrain branch coverage");}
        TerrainFollowResult48G untouched{savedState,body},out=untouched;auto bad=surface;bad.normal170.v[0]=std::numeric_limits<float>::infinity();
        require(!applyTerrainFollowing48G(savedState,p,bad,body,out),"invalid terrain normal accepted");require(std::memcmp(&out,&untouched,sizeof(out))==0,"invalid terrain mutated output");
        bad=surface;bad.normal170.v[0]=std::numeric_limits<float>::max();
        auto noDetach=savedState;noDetach.detached11c=0;noDetach.pending10c=0;
        require(!applyTerrainFollowing48G(noDetach,p,bad,body,out),"overflow terrain accepted");require(std::memcmp(&out,&untouched,sizeof(out))==0,"overflow terrain mutated output");
        std::cout<<"Original terrain-following response48G: "<<checks<<" checks passed; "<<caseNumber<<" original-word calls, 12 branches, 4 host rounding modes.\n";
        return 0;
    } catch(const std::exception&e){std::cerr<<"FAIL after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}
}

