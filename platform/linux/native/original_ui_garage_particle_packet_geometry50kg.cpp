#include "original_ui_garage_scene_particles50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>

namespace {
std::uint32_t word(const std::uint8_t* p)noexcept {std::uint32_t v;std::memcpy(&v,p,4);return v;}
float real(std::uint32_t u)noexcept {float v;std::memcpy(&v,&u,4);return v;}
float add(float a,float b)noexcept {volatile float v=a+b;return v;}
float sub(float a,float b)noexcept {volatile float v=a-b;return v;}
float mul(float a,float b)noexcept {volatile float v=a*b;return v;}
}
namespace rr::platform {
bool OriginalUiGarageSceneParticles50KG::lowerPacket3212C8(
    const game::OriginalUiMainParticleCamera50KG& camera,
    const game::OriginalUiMainParticleRenderConstants50KG& rendererConstants,
    const std::array<float,3>& cameraPosition,
    OriginalUiGarageParticlePacketGeometry50KG& output,
    assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=assets::OriginalStandardMaterialStatus50LA;
    d={};const auto fail=[&](S status,std::uint32_t pc)noexcept {
        d.status=status;d.sourcePC=pc;return false;
    };
    PacketBorrow borrow;
    //Only actual queued immutable ownership may lower this packet. The caller
    //also authenticates the same FIFO record, revision/publication and leaf14C.
    if(!nativeGarageFifoPins_||!borrowPacket162E50(borrow)||!numeric||!numeric->manager())
        return fail(S::phase,0x24B26C);
    const game::OriginalGcParticleFrame50KG* frames=nullptr;std::size_t frameCount=0;
    if(!numeric->manager()->readGarageFrames1626D0(frames,frameCount)||!frames||
       !frameCount||frameCount>std::numeric_limits<std::uint16_t>::max())
        return fail(S::input,0x3212C8);
    for(float v:cameraPosition)if(!std::isfinite(v))return fail(S::input,0x24BBAC);
    const auto* constants=borrow.constants;
    if(constants[3]!=0x6C018029u||constants[4]!=6||constants[8]!=0x6C01802Au||
       constants[13]!=0x6C04802Eu||constants[30]!=0x6C048000u||
       constants[47]!=0x14000010u||constants[48]!=0x10000000u||
       constants[49]!=0x030000B5u||constants[50]!=0x02000190u||constants[51]!=0)
        return fail(S::source,0x162010);
    auto actual=rendererConstants;
    for(unsigned corner=0;corner<4;++corner){
        for(unsigned lane=0;lane<3;++lane){
            const float value=real(constants[14+corner*4+lane]);
            if(!std::isfinite(value)||value<0||value>255||std::trunc(value)!=value)
                return fail(S::source,0x321300);
            actual.sourceLighting[corner][lane]=static_cast<std::uint8_t>(value);
        }
        if(constants[17+corner*4]!=64)return fail(S::source,0x162158);
    }
    const auto* bytes=borrow.scratch15C;
    if(word(bytes)!=((borrow.scratchUsedBytes-16)/16|0x30000000u)||
       word(bytes+8)||word(bytes+12))return fail(S::source,0x162810);
    try{
        OriginalUiGarageParticlePacketGeometry50KG next;
        next.rows.reserve(borrow.particleCount);
        std::size_t offset=16,seen=0,group=0;
        while(seen<borrow.particleCount){
            const auto count=std::min<std::size_t>(12,borrow.particleCount-seen);
            if(offset>borrow.scratchUsedBytes||count*64+64>borrow.scratchUsedBytes-offset)
                return fail(S::source,0x1622D4);
            const std::uint64_t tag=std::uint64_t(count*4)|(1ull<<15)|(1ull<<46)|
                (0x7Cull<<47)|(3ull<<60);
            if(word(bytes+offset)||word(bytes+offset+4)||word(bytes+offset+8)||
               word(bytes+offset+12)!=(0x6C008000u|std::uint32_t((count*4+2)<<16))||
               word(bytes+offset+16)!=std::uint32_t(tag)||
               word(bytes+offset+20)!=std::uint32_t(tag>>32)||
               word(bytes+offset+24)!=0x412u||word(bytes+offset+28)||
               word(bytes+offset+32)!=count||word(bytes+offset+36)||
               word(bytes+offset+40)||word(bytes+offset+44))
                return fail(S::source,0x321350);
            offset+=48;
            const std::size_t positions=offset,uvs=offset+count*32;
            std::size_t runRemaining=0;std::uint8_t state=0;
            for(std::size_t row=0;row<count;++row){
                const auto* position=bytes+positions+row*32;
                const auto* uv=bytes+uvs+row*32;
                OriginalUiGarageParticlePacketGeometry50KG::Row lowered;
                lowered.groupOrdinal=static_cast<std::uint16_t>(group);
                lowered.firstInGroup=row==0;
                if(!runRemaining){
                    const auto raw=word(position+12);std::int32_t run;std::memcpy(&run,&raw,4);
                    if(!run||run==std::numeric_limits<std::int32_t>::min())
                        return fail(S::source,0x3213A8);
                    const auto length=static_cast<std::size_t>(run<0?-run:run);
                    if(length>count-row)return fail(S::source,0x3213A8);
                    state=run<0?1:0;runRemaining=length;
                    lowered.firstInRun=true;lowered.signedRunLength=run;
                }
                game::OriginalUiMainParticlePacket50KG packet;
                for(unsigned lane=0;lane<3;++lane)packet.position[lane]=real(word(position+lane*4));
                //The numeric carrier describes only interpreted run heads;
                //non-head sourceW bits remain untouched in the retained scratch.
                packet.signedRunCount=lowered.firstInRun?lowered.signedRunLength:0;
                packet.sizeTimesCamera1D8=real(word(position+16));
                if(word(position+16)!=word(position+20))return fail(S::source,0x162634);
                packet.quantizedSourceDepth=real(word(position+24));
                packet.sourceBrightness=real(word(position+28));
                for(unsigned corner=0;corner<4;++corner)
                    for(unsigned lane=0;lane<2;++lane)
                        packet.normalizedUv[corner][lane]=real(word(uv+corner*8+lane*4));
                const auto& origin=borrow.origins[seen+row];
                if(origin.poolSlot>=game::originalGcParticleCapacity50KG||
                   origin.frame>=frameCount||origin.depthBucket>=game::originalGcParticleDepthBuckets50KG)
                    return fail(S::source,0x1626D0);
                packet.sourceParticleIndex=origin.poolSlot;packet.sourceFrame=origin.frame;
                packet.sourceDepthBucket=origin.depthBucket;
                //321390 SUB position-camera;3213C0 ELENG.xyz;3214B0 MFP.z.
                //This is the live fog operand, NOT the bucket-depth source24.
                const float dx=sub(packet.position[0],cameraPosition[0]);
                const float dy=sub(packet.position[1],cameraPosition[1]);
                const float dz=sub(packet.position[2],cameraPosition[2]);
                const float square=add(add(mul(dx,dx),mul(dy,dy)),mul(dz,dz));
                if(!std::isfinite(square)||square<0)return fail(S::source,0x3213C0);
                const float distance=std::sqrt(square);
                const auto status=game::projectOriginalParticleQuadWithDistance50KG(
                    packet,state,camera,actual,static_cast<std::uint16_t>(frameCount),255.f,
                    distance,lowered.projected);
                if(status!=game::OriginalUiMainParticleGeometryStatus50KG::complete)
                    return fail(S::source,0x321518);
                //3212C8..338 initializes STQ.z=1. MOVE.xy later copies the
                //actual UV without reciprocal-W. Read that provenance directly,
                //including genuinely clipped rows, not back from divided UV.
                constexpr unsigned nativeUv[]={3,0,1,2};
                for(unsigned corner=0;corner<4;++corner)
                    lowered.sourceSTQ[corner]={packet.normalizedUv[nativeUv[corner]][0],
                        packet.normalizedUv[nativeUv[corner]][1],1.f};
                if(lowered.projected.visible)++next.visibleQuads;
                next.rows.push_back(std::move(lowered));--runRemaining;
            }
            if(runRemaining)return fail(S::source,0x3213A8);
            offset=uvs+count*32;
            if(word(bytes+offset)||word(bytes+offset+4)||word(bytes+offset+8)||
               word(bytes+offset+12)!=0x17000000u)return fail(S::source,0x1627E4);
            offset+=16;seen+=count;++group;
        }
        if(offset!=borrow.scratchUsedBytes||group!=scratchPacketCount)
            return fail(S::source,0x162810);
        next.publication=borrow.publication;next.revision=borrow.revision;
        output=std::move(next);d.status=S::complete;d.sourcePC=0x3217A0;return true;
    }catch(const std::bad_alloc&){return fail(S::allocation,0x3212C8);}
    catch(...){return fail(S::input,0x3212C8);}
}
}
