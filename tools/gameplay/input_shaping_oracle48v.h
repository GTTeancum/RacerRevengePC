#pragma once
// Test only: execute frozen ELF instructions with synthetic caller-owned state.
// Never included in production. No calls into native shaping/math helpers.
#include "input_shaping_words48v.h"
#include "../../src/game/control_shaping48v.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace original48v {
inline std::uint32_t bits(float value) { std::uint32_t word; std::memcpy(&word,&value,4); return word; }
inline float fp(std::uint32_t word) { float value; std::memcpy(&value,&word,4); return value; }
struct Machine {
    std::array<std::uint32_t,32> g{};
    std::array<std::uint32_t,32> f{};
    std::array<std::uint32_t,0x3000/4> memory{};
    std::array<std::uint32_t,2> globals{{0x3f123456u,0xbe987654u}};
    std::array<unsigned char,2> initialized{};
    std::array<unsigned,249> visits{};
    float acc=0;
    bool condition=false;
    unsigned steps=0, cosineCalls=0, reflections=0;
    std::uint32_t read(std::uint32_t a) const {
        if(a==0x4346b8) return globals[0];
        if(a==0x4346c0) return globals[1];
        if((a&3u)||a/4>=memory.size()) throw std::runtime_error("oracle read outside synthetic fixture");
        return memory[a/4];
    }
    void write(std::uint32_t a,std::uint32_t value) {
        if(a==0x4346b8) {globals[0]=value;return;}
        if(a==0x4346c0) {globals[1]=value;return;}
        if((a&3u)||a/4>=memory.size()) throw std::runtime_error("oracle write outside synthetic fixture");
        memory[a/4]=value;
    }
    std::uint32_t word(std::uint32_t pc) const {
        if(pc>=rr_input_48v_start && pc-rr_input_48v_start<sizeof(rr_input_48v)) return rr_input_48v[(pc-rr_input_48v_start)/4];
        if(pc>=rr_cosine_48v_start && pc-rr_cosine_48v_start<sizeof(rr_cosine_48v)) return rr_cosine_48v[(pc-rr_cosine_48v_start)/4];
        throw std::runtime_error("oracle PC outside frozen intervals");
    }
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        if(++steps>2000) throw std::runtime_error("oracle instruction budget");
        if(pc>=rr_input_48v_start && pc-rr_input_48v_start<sizeof(rr_input_48v)) ++visits[(pc-rr_input_48v_start)/4];
        if(pc==0x13bbc8||pc==0x13bc04) ++reflections;
        const auto w=word(pc),op=w>>26,rs=(w>>21)&31u,rt=(w>>16)&31u,rd=(w>>11)&31u,fd=(w>>6)&31u,fn=w&63u;
        const auto imm=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs]+static_cast<std::uint32_t>(imm);};
        auto branch=[&](bool take){target=take?pc+4+static_cast<std::int32_t>(imm)*4:pc+8;return true;};
        if(!w) return false;
        if(op==9) g[rt]=address();
        else if(op==13) g[rt]=g[rs]|(w&65535u);
        else if(op==15) g[rt]=(w&65535u)<<16;
        else if(op==0x23) g[rt]=read(address());
        else if(op==0x2b) write(address(),g[rt]);
        else if(op==0x31) f[rt]=read(address());
        else if(op==0x39) write(address(),f[rt]);
        else if(op==0x1f) { // Only preserved low words are consumed in this interval.
            for(unsigned i=0;i<4;++i) write(address()+i*4,i?0u:g[rt]);
        }
        else if(op==0x20||op==0x28) {
            const auto a=address();
            if(a!=0x873c&&a!=0x8740) throw std::runtime_error("oracle unexpected global flag");
            const unsigned index=a==0x873c?0:1;
            if(op==0x20) g[rt]=static_cast<std::uint32_t>(static_cast<std::int8_t>(initialized[index]));
            else initialized[index]=static_cast<unsigned char>(g[rt]);
        }
        else if(op==0x1c&&fn==0x28&&fd==0x18) g[rd]=g[rs]|g[rt]; // POR low word.
        else if(op==4) return branch(g[rs]==g[rt]);
        else if(op==5) return branch(g[rs]!=g[rt]);
        else if(op==3) {g[31]=pc+8;target=(w&0x3ffffffu)*4;++cosineCalls;return true;}
        else if(op==0&&fn==8) {target=g[rs];return true;}
        else if(op==0x11) {
            if(rs==4) f[rd]=g[rt];
            else if(rs==0) g[rt]=f[rd];
            else if(rs==8&&(rt==0||rt==1)) return branch(condition==(rt==1));
            else if(rs==20&&fn==0x20) {volatile float value=static_cast<float>(static_cast<std::int32_t>(f[rd]));f[fd]=bits(value);}
            else if(rs==16) {
                const float a=fp(f[rd]),b=fp(f[rt]);
                volatile float value=0;
                switch(fn) {
                    case 0:value=a+b;break;
                    case 1:value=a-b;break;
                    case 2:value=a*b;break;
                    case 6:f[fd]=f[rd];return false;
                    case 7:f[fd]=f[rd]^0x80000000u;return false;
                    case 0x1a:value=a*b;acc=value;return false;
                    case 0x1c:{volatile float product=a*b;value=acc+product;break;}
                    case 0x24:
                        if(!(a>=-2147483648.0f && a<2147483648.0f)) throw std::runtime_error("oracle integer conversion outside fixture domain");
                        f[fd]=static_cast<std::uint32_t>(static_cast<std::int32_t>(a));return false;
                    case 0x28:case 0x29:{
                        const auto x=static_cast<std::int32_t>(f[rd]),y=static_cast<std::int32_t>(f[rt]);
                        const bool minimum=(fn==0x29)!=(x<0&&y<0);
                        f[fd]=static_cast<std::uint32_t>(minimum?(x<y?x:y):(x>y?x:y));return false;
                    }
                    case 0x34:condition=a<b;return false;
                    case 0x36:condition=a<=b;return false;
                    default:throw std::runtime_error("oracle unsupported FPU instruction");
                }
                f[fd]=bits(value);
            } else throw std::runtime_error("oracle unsupported COP1 format");
        } else throw std::runtime_error("oracle unsupported instruction");
        g[0]=0;
        return false;
    }
    rr::game::ControlInputStep48V run(const rr::game::ControlChannels48B& raw,float history,
                                  std::uint32_t mode,const rr::game::ControlChannels48B& prior,
                                  rr::game::ControlRateLimits48B rates,float elapsed,unsigned flagMask=0) {
        g[4]=0x400;g[29]=0x2800;g[28]=0x10000;f[12]=bits(elapsed);
        initialized[0]=(flagMask&1)?0xff:0;initialized[1]=(flagMask&2)?7:0;
        for(unsigned i=0;i<4;++i) {write(0x400+0xb5c+i*4,bits(raw.values[i]));write(0x400+0xb6c+i*4,bits(prior.values[i]));}
        write(0x400+0xd28,bits(history));write(0x400+0x58,mode);
        write(0x400+0xb94,bits(rates.x));write(0x400+0xb98,bits(rates.y));
        std::uint32_t pc=rr_input_48v_start;
        while(pc!=0x2181c4) {
            std::uint32_t target=pc+4;
            if(execute(pc,target)) {
                std::uint32_t ignored=0;
                if(execute(pc+4,ignored)) throw std::runtime_error("oracle branch in delay slot");
                pc=target;
            } else pc+=4;
        }
        rr::game::ControlInputStep48V result{};
        for(unsigned i=0;i<4;++i) {result.shaping.targets.values[i]=fp(read(0x400+0xb5c+i*4));result.filtered.values[i]=fp(read(0x400+0xb6c+i*4));}
        result.shaping.smoothedChannel0D28=fp(read(0x400+0xd28));
        return result;
    }
};
}
