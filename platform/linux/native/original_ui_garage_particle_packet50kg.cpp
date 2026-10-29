#include "../../../src/game/original_parameter_packet_history50la.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <new>

namespace {
std::uint32_t bits(float f)noexcept {std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float mul(float a,float b)noexcept {volatile float v=a*b;return v;}
float sub(float a,float b)noexcept {volatile float v=a-b;return v;}
float div(float a,float b)noexcept {volatile float v=a/b;return v;}
void word(std::uint8_t* p,std::uint32_t v)noexcept {std::memcpy(p,&v,4);}
}
namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::buildParticlePacket162010(
    OriginalUiGarageSceneParticles50KG& pool,Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::thirdPass||callback_!=&pool.entity||
       !page_||!page_->scene4C0_||page_->scene4C0_->sceneParticles_.get()!=&pool||
       page_->scenePageChildren_[9]!=&pool.entity||pool.drawPublication!=publication_||
       !publication_||pool.drawSourcePC!=0x162010||pool.nativeGarageFifoPins_||
       pool.packetTerminalPC||!pool.field12C)return fail(d,0x162010);
    const auto terminal=[&](std::uint32_t pc)noexcept {
        pool.packetTerminalPC=pc;sourcePC_=pc;return fail(d,pc);
    };
    auto& primitive=pool.primitive160;
    if(primitive.released133FA0)return terminal(0x134000);
    rr::game::OriginalParameterPacketHistory50LA::invalidateNative134xxx();
    for(auto& w:primitive.unwritten00To1C)w=0; //Actual134000 first32-byte clear.
    primitive.nativePayload04=nullptr;primitive.field2C=1;
    if(!primitive.buffer20){
        if(primitive.ownedBuffer20)return terminal(0x134000);
        primitive.count24=128; //Source capacity is WORDS, not qwords.
        primitive.ownedBuffer20.reset(new(std::nothrow) OriginalUiGarageSceneParticles50KG::Primitive::Buffer);
        if(!primitive.ownedBuffer20)return terminal(0x134000);
        primitive.buffer20=primitive.ownedBuffer20->words.data();
    }else if(!primitive.ownedBuffer20||primitive.buffer20!=primitive.ownedBuffer20->words.data()||
             primitive.count24!=128)return terminal(0x134000);
    primitive.count28=0;
    auto& words=primitive.ownedBuffer20->words;
    const auto put=[&](std::uint32_t v)noexcept {words[primitive.count28++]=v;};
    put(0x11000000);put(0x03000000);put(0x02000000);
    put(0x6C018029);put(6);put(0);put(0);put(0);
    put(0x6C01802A);for(unsigned i=0;i<4;++i)put(bits(1.f));
    put(0x6C04802E);
    for(const auto& corner:pool.quadRemainder){
        if(!corner[1])return terminal(0x162158);
        const auto rgba=*corner[1];
        put(bits(static_cast<float>((rgba>>16)&255)));
        put(bits(static_cast<float>((rgba>>8)&255)));
        put(bits(static_cast<float>(rgba&255)));
        put(64); //Source162158 passes integer64 to134470, not float64.
    }
    put(0x6C048000);
    for(unsigned i=0;i<16;++i)put(bits((i%5)==0?1.f:0.f)); //Actual14D840 constructor.
    put(0x14000010);put(0x10000000);put(0x030000B5);put(0x02000190);
    //1342B0 pads the transfer without increasing the source count28.
    if(primitive.count28!=51)return terminal(0x1342B0);
    words[51]=0;
    primitive.unwritten00To1C[0]=0x3000000D;
    primitive.unwritten00To1C[1].reset(); //Native pointer-width relocation, no fake guest integer.
    primitive.nativePayload04=words.data();
    primitive.unwritten00To1C[2]=0;primitive.unwritten00To1C[3]=0;
    pool.drawSourcePC=sourcePC_=0x162DE8;return true;
}

bool OriginalUiGarageDrawContribution50KG::buildParticleScratch162230(
    OriginalUiGarageSceneParticles50KG& pool,Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::thirdPass||callback_!=&pool.entity||
       !page_||!page_->scene4C0_||page_->scene4C0_->sceneParticles_.get()!=&pool||
       page_->scenePageChildren_[9]!=&pool.entity||pool.drawPublication!=publication_||
       !publication_||pool.drawSourcePC!=0x162230||pool.nativeGarageFifoPins_||
       pool.packetTerminalPC||!pool.numeric||!pool.numeric->manager()||!pool.material150)
        return fail(d,0x162230);
    const auto terminal=[&](std::uint32_t pc)noexcept {
        pool.packetTerminalPC=pc;sourcePC_=pc;return fail(d,pc);
    };
    auto& manager=*pool.numeric->manager();
    if(!pool.scratch15C||!pool.field12C||pool.field12C>game::originalGcParticleCapacity50KG||
       !pool.minimum120||!pool.maximum124||*pool.minimum120<0||*pool.maximum124<*pool.minimum120||
       *pool.maximum124>=pool.depthCount118||pool.buckets11C.size()!=std::size_t(pool.depthCount118)||
       pool.scratchBytes!=(game::originalGcParticleCapacity50KG+
           game::originalGcParticleCapacity50KG/12+1)*64+16)
        return terminal(0x162258);
    OriginalUiGarageCameraFrame50KG::Fields camera;std::uint32_t cameraPC=0;
    if(!cameraFrame_.read(camera,cameraPC))return fail(d,cameraPC?cameraPC:0x16262C);
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(camera.focal1D8))
        return terminal(0x162630);
    auto* const bytes=pool.scratch15C.get();
    pool.scratchUsedBytes=0;pool.scratchPacketCount=0;pool.scratchPayload04=nullptr;
    std::array<bool,game::originalGcParticleCapacity50KG> seen{};
    const game::OriginalGcParticle50KG* next=nullptr;
    std::int32_t bucket=*pool.maximum124+1;
    std::size_t offset=16,written=0;
    while(written<pool.field12C){
        const auto count=std::min<std::size_t>(12,pool.field12C-written);
        if(offset>pool.scratchBytes||count*64+64>pool.scratchBytes-offset)
            return terminal(0x1622D4);
        word(bytes+offset,0);word(bytes+offset+4,0);word(bytes+offset+8,0);
        word(bytes+offset+12,0x6C008000u|static_cast<std::uint32_t>((count*4+2)<<16));
        offset+=16;
        const std::uint64_t tag=static_cast<std::uint64_t>(count*4)|(1ull<<15)|
            (1ull<<46)|(0x7Cull<<47)|(3ull<<60);
        word(bytes+offset,static_cast<std::uint32_t>(tag));
        word(bytes+offset+4,static_cast<std::uint32_t>(tag>>32));
        word(bytes+offset+8,0x412);word(bytes+offset+12,0);offset+=16;
        word(bytes+offset,static_cast<std::uint32_t>(count));
        word(bytes+offset+4,0);word(bytes+offset+8,0);word(bytes+offset+12,0);offset+=16;
        std::array<const game::OriginalGcParticle50KG*,12> rows{};
        std::size_t runStart=offset,runLength=0;bool additive=false;
        const auto finishRun=[&]()noexcept {
            const auto run=static_cast<std::int32_t>(runLength);
            word(bytes+runStart+12,static_cast<std::uint32_t>(additive?-run:run));
        };
        for(std::size_t row=0;row<count;++row){
            while(!next){
                if(--bucket<*pool.minimum120)return terminal(0x162420);
                const auto& entry=pool.buckets11C[std::size_t(bucket)];
                if(entry.epoch00==pool.epoch114)next=entry.head04;
            }
            std::size_t slot=0;
            if(!manager.readGaragePoolSlot162448(next,slot)||slot>=seen.size()||
               seen[slot]||!pool.next20Written[slot])return terminal(0x162448);
            seen[slot]=true;rows[row]=next;
            const auto& particle=*next;
            //Source copies ALL four position words; only real run heads later replaceW.
            for(unsigned c=0;c<3;++c)word(bytes+offset+c*4,bits(particle.position[c]));
            word(bytes+offset+12,bits(particle.positionW));
            const bool state=(particle.flags&1u)!=0;
            if(!row){runStart=offset;runLength=1;additive=state;}
            else if(state==additive)++runLength;
            else{finishRun();runStart=offset;runLength=1;additive=state;}
            float brightness=(particle.flags&0x2000u)?pool.glowScalar148:64.f;
            if(particle.flags&0x80u){
                //The reached emitter220/224 fog owner is not GC's independent
                //fog-authoring adapter. Stop with prior source writes retained.
                return terminal(0x16253C);
            }else if(particle.flags&2u){
                brightness=mul(brightness,sub(1.f,mul(particle.ageMilliseconds,particle.inverseLife)));
            }else if(particle.flags&0x1000u){
                const float initialCube=mul(particle.initialSize,mul(particle.initialSize,particle.initialSize));
                const float sizeCube=mul(particle.size,mul(particle.size,particle.size));
                brightness=mul(brightness,mul(mul(particle.ageMilliseconds,particle.inverseLife),
                    div(initialCube,sizeCube)));
            }
            const float size=mul(camera.focal1D8,particle.size);
            if(!std::isfinite(brightness)||!std::isfinite(size))return terminal(0x162630);
            word(bytes+offset+16,bits(size));word(bytes+offset+20,bits(size));
            word(bytes+offset+24,particle.reserved24);word(bytes+offset+28,bits(brightness));
            pool.packetOrigins[written+row]={static_cast<std::uint16_t>(slot),
                static_cast<std::uint16_t>(particle.frame),static_cast<std::uint16_t>(bucket)};
            next=pool.next20[slot];offset+=32;
        }
        finishRun();
        //The original resets the SAME iterator to this group's start for UVs.
        //rows retain precisely those authenticated pointers, not another sort.
        for(std::size_t row=0;row<count;++row){
            const auto& particle=*rows[row];
            if(particle.frame<0)return terminal(0x1626D0);
            const game::OriginalGcParticleFrame50KG* frames=nullptr;std::size_t frameCount=0;
            if(!manager.readGarageFrames1626D0(frames,frameCount)||
               static_cast<std::size_t>(particle.frame)>=frameCount)return terminal(0x1626D0);
            const auto* frame=frames+particle.frame;
            for(unsigned corner=0;corner<4;++corner){
                unsigned x=corner,y=corner;
                if(particle.flags&0x4000u){
                    x=corner^1u;
                    if(particle.flags&0x8000u)y=corner^2u;
                }
                word(bytes+offset+corner*8,bits(frame->sourceUv[x][0]));
                word(bytes+offset+corner*8+4,bits(frame->sourceUv[y][1]));
            }
            offset+=32;
        }
        word(bytes+offset,0);word(bytes+offset+4,0);word(bytes+offset+8,0);
        word(bytes+offset+12,0x17000000);offset+=16;
        written+=count;++pool.scratchPacketCount;
    }
    //No substituted guest pointer bits. The actual transfer owns bytes+16.
    word(bytes,static_cast<std::uint32_t>((offset-16)/16)|0x30000000u);
    pool.scratchPayload04=bytes+16;
    word(bytes+8,0);word(bytes+12,0);
    pool.scratchUsedBytes=offset;
    pool.material150->fields_.value20=static_cast<std::uint16_t>(pool.field12C*4);
    pool.material150->fields_.value22=static_cast<std::uint16_t>(pool.field12C);
    pool.drawSourcePC=sourcePC_=0x162E14;return true;
}

bool OriginalUiGarageSceneParticles50KG::borrowPacket162E50(PacketBorrow& out)const noexcept {
    if(!prepared||retiring||retired||!entityLive||!drawPublication||packetTerminalPC||
       (drawSourcePC!=0x162E50u&&drawSourcePC!=0x162E68u)||!packetRevision_||!field12C||
       field12C>game::originalGcParticleCapacity50KG||!material150||!scratch15C||
       scratchPayload04!=scratch15C.get()+16||!scratchUsedBytes||scratchUsedBytes>scratchBytes||
       material150->coldFields().allocation10!=scratch15C.get()||
       material150->coldFields().value22!=field12C||material150->coldFields().value20!=field12C*4||
       primitive160.released133FA0||!primitive160.ownedBuffer20||
       primitive160.buffer20!=primitive160.ownedBuffer20->words.data()||
       primitive160.nativePayload04!=primitive160.ownedBuffer20->words.data()||
       primitive160.count24!=128||primitive160.count28!=51||
       primitive160.unwritten00To1C[0]!=0x3000000Du)return false;
    out={&primitive160,material150.get(),primitive160.nativePayload04,51,13,
        scratch15C.get(),scratchPayload04,scratchBytes,scratchUsedBytes,
        packetOrigins.data(),field12C,drawPublication,packetRevision_};
    return true; //Pinned immutable reads are permitted; mutation is separately forbidden.
}
}
