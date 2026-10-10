#include "original_ui_main_fire50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <utility>

namespace rr::game {
namespace {
float add(float a,float b) noexcept{volatile float r=a+b;return r;}
float sub(float a,float b) noexcept{volatile float r=a-b;return r;}
float mul(float a,float b) noexcept{volatile float r=a*b;return r;}
float div(float a,float b) noexcept{volatile float r=a/b;return r;}
bool scalar(float v) noexcept{return std::isfinite(v)&&std::fpclassify(v)!=FP_SUBNORMAL;}
float draw(std::uint32_t& raw) noexcept {
    raw=raw*0x41c64e6du+0x3039u;
    volatile float integer=static_cast<float>(raw&0x7fffffffu);
    return mul(0x1p-31f,integer);
}
}
struct OriginalUiMainFireOwner50KG::Storage {
    OriginalUiMainSceneDefinition50KG definition;
    std::array<OriginalUiMainParticle50KG,originalUiMainParticleCapacity50KG> pool{};
    std::array<OriginalUiMainParticlePacket50KG,originalUiMainParticleCapacity50KG> drawPackets{};
    std::array<std::int32_t,originalUiMainDepthBuckets50KG> bucketHeads{};
    std::array<std::int32_t,originalUiMainParticleCapacity50KG> next{};
    std::array<float,originalUiMainParticleCapacity50KG> quantizedDepth{};
    std::size_t count=0,drawCount=0;
    bool firstUpdate=true;
    float fractional224=0;
};
OriginalUiMainFireOwner50KG::OriginalUiMainFireOwner50KG() noexcept=default;
OriginalUiMainFireOwner50KG::~OriginalUiMainFireOwner50KG()=default;
OriginalUiMainFireStatus50KG OriginalUiMainFireOwner50KG::initialize(
    const OriginalUiMainSceneOwner50KG& scene) noexcept {
    return initializeScene(scene,false);
}
OriginalUiMainFireStatus50KG OriginalUiMainFireOwner50KG::initializeOptionsMenu(
    const OriginalUiMainSceneOwner50KG& scene) noexcept {
    return initializeScene(scene,true);
}
OriginalUiMainFireStatus50KG OriginalUiMainFireOwner50KG::initializeScene(
    const OriginalUiMainSceneOwner50KG& scene,bool options) noexcept {
    using S=OriginalUiMainFireStatus50KG;
    if(storage_)return S::alreadyInitialized;
    const auto* definition=scene.definition();if(!definition)return S::unprepared;
    if(scene.archiveIdentity()!=assets::originalUiArchiveIdentity50KG(options?
        assets::OriginalUiArchiveId50KG::options:assets::OriginalUiArchiveId50KG::mainMenu))return S::definition;
    const auto& f=definition->fire;
    // Authenticated Main or explicit byte-identical Options scene only. Do not
    // broaden it to attached/racing emitters, other modes, or arbitrary EMTs.
    if(f.type!=8||f.instances!=1||f.particleGroup!=4||!f.startOn||
       f.emitMode!=0||f.delaySeconds!=0||f.forcedSetCount!=0||
       !f.applySpreadToVelocity||f.sourceParticleFlags!=0x1405u||
       f.framesPerSet!=8||f.lifeMilliseconds!=650||f.initialSize!=7||
       f.growth!=-6.5f||f.ratePerSecond!=200||f.ageAtBirth!=0||
       f.velocity[0]!=0||f.velocity[1]!=40||f.velocity[2]!=0||
       !f.randomMirrorX||!f.randomMirrorY)return S::definition;
    try{auto s=std::make_unique<Storage>();s->definition=*definition;
        storage_=std::move(s);return S::complete;}catch(...){return S::allocation;}
}
OriginalUiMainFireReceipt50KG OriginalUiMainFireOwner50KG::emit(float dt,bool paused,
    std::uint32_t& sharedRawRandom,std::uint32_t& sharedFireSerial) noexcept {
    using S=OriginalUiMainFireStatus50KG;OriginalUiMainFireReceipt50KG r;
    if(!storage_)return r;
    if(std::fegetround()!=FE_TONEAREST){r.status=S::arithmetic;return r;}
    // Upper bound is native arithmetic admission, not an original time clamp.
    if(!scalar(dt)||dt<0||dt>1){r.status=S::delta;return r;}
    if(&sharedRawRandom==&sharedFireSerial){r.status=S::storage;return r;}
    auto& s=*storage_;
    if(paused){r.status=S::complete;return r;}
    if(s.firstUpdate){s.firstUpdate=false;r.skippedFirstUpdate=true;r.status=S::complete;return r;}
    const auto& f=s.definition.fire;
    const float emission=mul(0.6f,mul(f.ratePerSecond,dt));
    std::uint32_t requested=0;
    if(emission>1){
        requested=static_cast<std::uint32_t>(emission);s.fractional224=0;
    }else{
        const float accumulated=add(s.fractional224,emission);
        if(accumulated==0)s.fractional224=0; //15E280 explicitly writes count0.
        else if(accumulated>1){
            requested=static_cast<std::uint32_t>(accumulated);s.fractional224=0;
        }else{
            //NATIVE COMPATIBILITY POLICY, not a source-equivalence claim:
            //15E240..15E2A8 does not define s1 for0<accumulated<=1.
            //Use integral emission and retain the fraction only on this
            //undefined source branch; every defined source branch above keeps
            //its original accumulator reset and RNG consumption behavior.
            requested=static_cast<std::uint32_t>(accumulated);
            s.fractional224=sub(accumulated,float(requested));
            r.nativeUndefinedCarryPolicy=true;
        }
    }
    const auto available=s.pool.size()-s.count;
    const auto count=std::min<std::size_t>(requested,available);
    auto raw=sharedRawRandom,serial=sharedFireSerial;
    for(std::size_t i=0;i<count;++i){
        ++serial; //433970 increments BEFORE all three draws, only after capacity guard.
        const float x=draw(raw),y=draw(raw),z=draw(raw);
        std::int32_t set=0;if(z>0.33f)set=1;if(z>0.66f)set=2;
        auto& p=s.pool[s.count++];p={};
        p.position=f.position;
        //15E37C..15E3B4 maps X/Z only. Source Min/MaxRangeY does not affect
        // stationary position. Second random sample scales authored Y velocity.
        p.position[0]=add(p.position[0],add(f.minRange[0],mul(x,sub(f.maxRange[0],f.minRange[0]))));
        p.position[2]=add(p.position[2],add(f.minRange[2],mul(z,sub(f.maxRange[2],f.minRange[2]))));
        p.velocity={{0,mul(f.velocity[1],y),0}};
        p.size=p.initialSize=f.initialSize;p.growth=f.growth;
        p.lifeMilliseconds=sub(f.lifeMilliseconds,mul(0.125f,mul(f.lifeMilliseconds,float(serial&3u))));
        p.inverseLife=div(1,p.lifeMilliseconds);
        //15E528 birth age is half life *min(movingEmitterSpeed/650,1).
        // This fixed unattached Main emitter has previous==current, hence zero.
        p.ageMilliseconds=0;
        p.frame=s.definition.groups[4].first+set*8;
        p.flags=f.sourceParticleFlags;
        if(x>0.5f)p.flags|=0x4000u;if(y>0.5f)p.flags|=0x8000u;
    }
    sharedRawRandom=raw;sharedFireSerial=serial;s.drawCount=0;
    r.randomDraws=static_cast<std::uint32_t>(count*3);r.spawned=static_cast<std::uint32_t>(count);
    r.poolFull=count<requested;r.status=S::complete;return r;
}
OriginalUiMainFireReceipt50KG OriginalUiMainFireOwner50KG::advance(float dt,bool paused) noexcept {
    using S=OriginalUiMainFireStatus50KG;OriginalUiMainFireReceipt50KG r;
    if(!storage_)return r;
    if(std::fegetround()!=FE_TONEAREST){r.status=S::arithmetic;return r;}
    if(!scalar(dt)||dt<0||dt>1){r.status=S::delta;return r;}
    auto& s=*storage_;if(paused){r.status=S::complete;return r;}
    const float milliseconds=mul(1000,dt);
    for(std::size_t i=0;i<s.count;++i){
        auto* p=&s.pool[i];p->ageMilliseconds=add(p->ageMilliseconds,milliseconds);
        if(p->ageMilliseconds>p->lifeMilliseconds){
            --s.count;++r.expired;
            if(i==s.count)break;
            std::swap(s.pool[i],s.pool[s.count]);p=&s.pool[i];
            //1613F8..161438 does NOT age the swapped-in particle again or
            // recheck its life. It still receives this step's motion/frame update.
        }
        for(unsigned lane=0;lane<3;++lane)p->position[lane]=add(p->position[lane],mul(dt,p->velocity[lane]));
        const auto first=s.definition.groups[4].first;
        const float inverseSet=div(1,s.definition.fire.framesPerSet);
        const float denominator=mul(p->lifeMilliseconds,inverseSet);
        auto phase=static_cast<std::int32_t>(div(p->ageMilliseconds,denominator));
        if(phase>0)--phase;
        const auto set=static_cast<std::int32_t>(mul(inverseSet,float(p->frame-first)));
        p->frame=first+set*8+phase; //161674..1616DC, all CVT.W.S truncate.
    }
    s.drawCount=0;r.status=S::complete;return r;
}
OriginalUiMainFireStatus50KG OriginalUiMainFireOwner50KG::preparePackets(
    const OriginalUiMainParticleView50KG& view) noexcept {
    using S=OriginalUiMainFireStatus50KG;
    if(!storage_)return S::unprepared;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    for(float v:view.sourceDepthColumn)if(!scalar(v))return S::arithmetic;
    if(!scalar(view.camera1D8)||view.camera1D8<=0)return S::arithmetic;
    auto& s=*storage_;s.drawCount=0;s.bucketHeads.fill(-1);s.next.fill(-1);
    const float maxDepth=16777000.f;
    const float scale=div(float(originalUiMainDepthBuckets50KG),maxDepth);
    const float inverseScale=div(1,scale);
    for(std::size_t i=0;i<s.count;++i){
        const auto& p=s.pool[i];
        if(p.frame<52||p.frame>75)return S::definition;
        const auto& c=view.sourceDepthColumn;
        const float depth=add(add(add(mul(p.position[0],c[0]),mul(p.position[1],c[1])),mul(p.position[2],c[2])),c[3]);
        if(!scalar(depth))return S::arithmetic;
        //Source excludes bucket>=2500. Negative/zero depths are outside;
        //never cast a huge negative value or manufacture a near clip plane.
        if(depth<=0||depth>=maxDepth)continue;
        const auto wholeDepth=static_cast<std::int32_t>(depth);
        const auto bucket=static_cast<std::int32_t>(sub(float(originalUiMainDepthBuckets50KG),mul(float(wholeDepth),scale)));
        if(bucket<0||bucket>=static_cast<std::int32_t>(originalUiMainDepthBuckets50KG))continue;
        s.quantizedDepth[i]=mul(float(static_cast<std::int32_t>(mul(depth,scale))),inverseScale);
        s.next[i]=s.bucketHeads[std::size_t(bucket)];s.bucketHeads[std::size_t(bucket)]=static_cast<std::int32_t>(i);
    }
    for(std::size_t n=s.bucketHeads.size();n>0;--n){
        const auto bucket=n-1;
        for(auto index=s.bucketHeads[bucket];index>=0;index=s.next[std::size_t(index)]){
            const auto& p=s.pool[std::size_t(index)];auto& packet=s.drawPackets[s.drawCount++];packet={};
            packet.position=p.position;packet.sizeTimesCamera1D8=mul(view.camera1D8,p.size);
            packet.quantizedSourceDepth=s.quantizedDepth[std::size_t(index)];
            //1625D4 executes FadeNonLinear with Fades=F. Preserve the authored
            //rising age/life brightness, not a conventional1-age/life fadeout.
            const float initialCube=mul(p.initialSize,mul(p.initialSize,p.initialSize));
            const float sizeCube=mul(p.size,mul(p.size,p.size));
            const float factor=mul(mul(p.ageMilliseconds,p.inverseLife),div(initialCube,sizeCube));
            packet.sourceBrightness=mul(64,factor);
            packet.normalizedUv=s.definition.frames[std::size_t(p.frame)].sourceUv;
            for(auto& uv:packet.normalizedUv){uv[0]=div(uv[0],128);uv[1]=div(uv[1],256);}
            //1626E0..1627A8: Y-only mirroring bypasses the mirror block.
            if(p.flags&0x4000u){
                auto original=packet.normalizedUv;
                packet.normalizedUv[0][0]=original[1][0];packet.normalizedUv[1][0]=original[0][0];
                packet.normalizedUv[2][0]=original[3][0];packet.normalizedUv[3][0]=original[2][0];
                if(p.flags&0x8000u){packet.normalizedUv[0][1]=original[2][1];packet.normalizedUv[1][1]=original[3][1];
                    packet.normalizedUv[2][1]=original[0][1];packet.normalizedUv[3][1]=original[1][1];}
            }
            packet.sourceParticleIndex=static_cast<std::uint16_t>(index);
            packet.sourceFrame=static_cast<std::uint16_t>(p.frame);
            packet.sourceDepthBucket=static_cast<std::uint16_t>(bucket);
            if(!scalar(packet.sizeTimesCamera1D8)||!scalar(packet.sourceBrightness)){s.drawCount=0;return S::arithmetic;}
        }
    }
    //Main particles are all additive. Preserve source batch/run boundary12.
    for(std::size_t i=0;i<s.drawCount;i+=12)
        s.drawPackets[i].signedRunCount=-static_cast<std::int32_t>(std::min<std::size_t>(12,s.drawCount-i));
    return S::complete;
}
std::size_t OriginalUiMainFireOwner50KG::particleCount() const noexcept{return storage_?storage_->count:0;}
const OriginalUiMainParticle50KG* OriginalUiMainFireOwner50KG::particles() const noexcept{return storage_?storage_->pool.data():nullptr;}
std::size_t OriginalUiMainFireOwner50KG::packetCount() const noexcept{return storage_?storage_->drawCount:0;}
const OriginalUiMainParticlePacket50KG* OriginalUiMainFireOwner50KG::packets() const noexcept{return storage_?storage_->drawPackets.data():nullptr;}
}
