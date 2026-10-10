#pragma once
// Test-only instruction interpreter. No production headers, source formulas,
// callbacks, RAM images or previous runtime fixtures are consumed.
#include "../../evidence/bodyAdvancement49K/numeric-source/original_words49k.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
namespace advancement49k_oracle {
using U=std::uint32_t;using U64=std::uint64_t;
inline U bits(float f){U u;std::memcpy(&u,&f,4);return u;}
inline float fp(U u){float f;std::memcpy(&f,&u,4);return f;}
inline constexpr unsigned wordCount=sizeof(advancement49k_source::words)/sizeof(advancement49k_source::words[0]);
inline std::array<U64,wordCount> coverage{},taken{},notTaken{};
inline U64 instructionCount=0;
struct Store{U pc,address,word;};
struct Machine {
    std::array<std::array<U,4>,32> r{};
    std::array<float,32> f{};
    std::array<std::array<float,4>,32> v{};
    std::array<float,4> acc{};
    std::map<U,U> mem;
    std::vector<Store> stores;
    bool condition=false; U current=0;
    Machine(){v[0][3]=1.0f;r[29][0]=0x90000;r[31][0]=0xdead0000;}
    void put(U a,U b){mem[a]=b;}
    void putf(U a,float b){put(a,bits(b));}
    U read(U a)const{const auto it=mem.find(a);if(it==mem.end())throw std::runtime_error("49K original read outside supplied fixture "+std::to_string(a)+" at PC "+std::to_string(current));return it->second;}
    float readf(U a)const{return fp(read(a));}
    void write(U a,U w){mem[a]=w;stores.push_back({current,a,w});}
    static float op(float a,float b,unsigned fn){volatile float z=0;switch(fn){case 0:z=a+b;break;case 1:z=a-b;break;case 2:z=a*b;break;case 3:z=a/b;break;default:throw std::runtime_error("49K arithmetic opcode");}return z;}
    U64 full(unsigned n)const{return U64(r[n][0])|(U64(r[n][1])<<32);}
    void set64(unsigned n,U64 x){r[n][0]=U(x);r[n][1]=U(x>>32);}
    static U truncWord(float a){ // EE finite CVT.W.S, decoded from operand bits.
        U b=bits(a);int exponent=int((b>>23)&255)-127;
        if(exponent<0)return 0;
        if(exponent>30)throw std::runtime_error("49K oracle conversion exceeds admitted bound");
        U mantissa=0x800000u|(b&0x7fffffu);U mag=exponent>=23?mantissa<<unsigned(exponent-23):mantissa>>unsigned(23-exponent);
        return b&0x80000000u?U(0)-mag:mag;
    }
    bool execute(U pc,U& target){
        unsigned n=0;while(n<wordCount&&advancement49k_source::words[n].pc!=pc)++n;
        if(n==wordCount)throw std::runtime_error("49K nonselected PC "+std::to_string(pc));
        ++coverage[n];++instructionCount;current=pc;
        const U w=advancement49k_source::words[n].bits,op0=w>>26,rs=w>>21&31,rt=w>>16&31,rd=w>>11&31,sa=w>>6&31,fn=w&63;
        const auto imm=std::int32_t(std::int16_t(w&65535));
        auto address=[&](){return U(r[rs][0]+U(imm));};
        auto branch=[&](bool yes){if(yes)++taken[n];else ++notTaken[n];target=yes?pc+4+U(imm*4):pc+8;return true;};
        if(!w){}
        else if(op0==9||op0==25)set64(rt,full(rs)+std::uint64_t(std::int64_t(imm)));
        else if(op0==15)set64(rt,U64(std::int64_t(std::int32_t(w<<16))));
        else if(op0==12)r[rt][0]=r[rs][0]&(w&65535);
        else if(op0==13)r[rt][0]=r[rs][0]|(w&65535);
        else if(op0==35)set64(rt,U64(std::int64_t(std::int32_t(read(address())))));
        else if(op0==36)set64(rt,(read(address()&~3u)>>((address()&3u)*8u))&255u);
        else if(op0==43)write(address(),r[rt][0]);
        else if(op0==49)f[rt]=readf(address());
        else if(op0==57)write(address(),bits(f[rt]));
        else if(op0==30){const U a=address();for(unsigned i=0;i<4;++i)r[rt][i]=read(a+i*4);}
        else if(op0==31){for(unsigned i=0;i<4;++i)write(address()+i*4,r[rt][i]);}
        else if(op0==54){for(unsigned i=0;i<4;++i)v[rt][i]=readf(address()+i*4);}
        else if(op0==62){for(unsigned i=0;i<4;++i)write(address()+i*4,bits(v[rt][i]));}
        else if(op0==4)return branch(full(rs)==full(rt));
        else if(op0==5)return branch(full(rs)!=full(rt));
        else if(op0==0){
            if(fn==8){target=r[rs][0];return true;}
            if(fn==60)set64(rd,full(rt)<<(sa+32));
            else if(fn==62)set64(rd,full(rt)>>(sa+32));
            else if(fn!=8)throw std::runtime_error("49K special opcode");
        }
        else if((w&0xfc0007ffu)==0x70000628u){for(unsigned i=0;i<4;++i)r[rd][i]=r[rs][i]|r[rt][i];}
        else if(op0==17){
            if(rs==0)set64(rt,U64(std::int64_t(std::int32_t(bits(f[rd])))));
            else if(rs==4)f[rd]=fp(r[rt][0]);
            else if(rs==8)return branch(condition==bool(rt&1));
            else if(rs==20&&fn==32)f[sa]=static_cast<float>(static_cast<std::int32_t>(bits(f[rd])));
            else if(rs==16){
                if(fn<4)f[sa]=op(f[rd],f[rt],fn);
                else if(fn==6)f[sa]=f[rd];
                else if(fn==36)f[sa]=fp(truncWord(f[rd]));
                else if(fn==50)condition=f[rd]==f[rt];
                else if(fn==52)condition=f[rd]<f[rt];
                else if(fn==54)condition=f[rd]<=f[rt];
                else throw std::runtime_error("49K FPU opcode");
            }else throw std::runtime_error("49K COP1 format");
        }
        else if(op0==18){
            switch(w){
            case 0x4a20025b:v[9][3]=op(v[0][3],v[0][3],2);break;
            case 0x4bc821bc:for(unsigned i=0;i<3;++i)acc[i]=op(v[4][i],v[8][0],2);break;
            case 0x4bc828bd:for(unsigned i=0;i<3;++i)acc[i]=op(acc[i],op(v[5][i],v[8][1],2),0);break;
            case 0x4bc8324a:for(unsigned i=0;i<3;++i)v[9][i]=op(acc[i],op(v[6][i],v[8][2],2),0);break;
            case 0x4bc74a68:for(unsigned i=0;i<3;++i)v[9][i]=op(v[9][i],v[7][i],0);break;
            default:throw std::runtime_error("49K VU opcode");
            }
        }
        else throw std::runtime_error("49K unrecognized word "+std::to_string(w));
        r[0]={};return false;
    }
    U run(U pc,std::initializer_list<U> stops={0xdead0000}){
        for(unsigned step=0;step<1000;++step){
            for(U stop:stops)if(pc==stop)return pc;
            U target=0,unused=0;
            if(execute(pc,target)){if(execute(pc+4,unused))throw std::runtime_error("49K branch in delay slot");pc=target;}else pc+=4;
        }throw std::runtime_error("49K instruction budget exceeded");
    }
};
}
