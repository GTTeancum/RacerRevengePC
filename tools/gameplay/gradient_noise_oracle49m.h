#pragma once
// Test only. Original-word execution of 0x14eec0 and 0x308030 with an explicit
// dependency projection of 0x14efd0. This is NOT a full initializer emulator.
// It includes no production implementation, formulas, state types, or helpers.
#include "../../evidence/secondaryWrapper49M/noise-source/gradient_noise_words49m.h"
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace original49m {
inline std::uint32_t bits(float value) {
    std::uint32_t word; std::memcpy(&word,&value,sizeof(word)); return word;
}
inline float fp(std::uint32_t word) {
    float value; std::memcpy(&value,&word,sizeof(value)); return value;
}
inline std::int32_t signedWord(std::uint32_t word) {
    std::int32_t value; std::memcpy(&value,&word,sizeof(value)); return value;
}
struct State {
    std::array<std::uint32_t,256> permutation{};
    std::array<float,256> gradient1D{};
    std::int32_t scratchCell=0;
    bool initialized=false;
};
struct Outcome {
    State state{};
    std::uint32_t raw=0;
    float result=0;
    unsigned draws=0;
};
struct ExcludedSpan {
    std::uint32_t begin,end; // Half-open original PC interval.
};

struct Machine {
    static constexpr std::uint32_t noiseStart=0x14eec0, noiseEnd=0x14efc4;
    static constexpr std::uint32_t initializeStart=0x14efd0, initializeEnd=0x14f2f0;
    static constexpr std::uint32_t randomStart=0x308030, randomEnd=0x308060;
    static constexpr std::size_t noiseCount=(noiseEnd-noiseStart)/4;
    static constexpr std::size_t initializeCount=(initializeEnd-initializeStart)/4;
    static constexpr std::size_t randomCount=(randomEnd-randomStart)/4;
    static constexpr std::size_t pcCount=noiseCount+initializeCount+randomCount;
    static constexpr std::uint32_t permutationBase=0x42fa60, gradientBase=0x432aa0;
    static constexpr std::uint32_t vector2Base=0x431a90, vector3Base=0x430270;
    static constexpr std::uint32_t scratchAddress=0x4332a8, flagAddress=0x382b40;
    static constexpr std::uint32_t randomPointerAddress=0x38544c;
    static constexpr std::uint32_t syntheticRandomObject=0x400, stackBase=0x1000;
    static constexpr std::uint32_t returnSentinel=0xfffffff0u;
    static_assert(sizeof(gradient49m_words::noise)/sizeof(std::uint32_t)==noiseCount,
                  "49M frozen noise extent changed");
    static_assert(sizeof(gradient49m_words::initialize)/sizeof(std::uint32_t)==initializeCount,
                  "49M frozen initializer extent changed");
    static_assert(sizeof(gradient49m_words::random)/sizeof(std::uint32_t)==randomCount,
                  "49M frozen RNG extent changed");
    static_assert(gradient49m_words::noise_start==noiseStart &&
                  gradient49m_words::initialize_start==initializeStart &&
                  gradient49m_words::random_start==randomStart,
                  "49M frozen PC origins changed");

    // Dataflow projection, rather than fake implementations of three callees:
    //  * The first five spans compute/store only normalized g2/g3 and dead
    //    caller-clobbered values. Every RNG jal and all integer loop state are
    //    retained. 0x14f0d8/dc, 0x14f17c..184, 0x14f18c/190, and 0x14f1b4/b8
    //    are deliberately outside the exclusions.
    //  * [0x14f228,0x14f2c8) copies suffixes. noise masks both cell indices to
    //    0..255, p is initialized to 0..255 and only permuted, and g1 is indexed
    //    through p. Thus no reachable noise load observes a suffix or g2/g3.
    //    The suffix-copy loop's first 256 destinations are beyond the first
    //    256 elements; its two further source elements are earlier copies.
    //  * All excluded helpers are normalization conversions/square root, with
    //    no RNG dependency. No skipped call is represented by a callback.
    // Only span entries are counted: omitted loops are NOT claimed executed.
    static const std::array<ExcludedSpan,6>& excludedSpans() {
        static const std::array<ExcludedSpan,6> spans{{
            {0x14f0b0,0x14f0d8}, {0x14f0e0,0x14f0f8},
            {0x14f148,0x14f17c}, {0x14f188,0x14f18c},
            {0x14f194,0x14f1b4}, {0x14f228,0x14f2c8}
        }};
        return spans;
    }

    State state{};
    std::uint32_t raw=0;
    // Only low GPR words are consumed in the included slice. sq/lq preserve
    // those low words; synthetic zero padding represents unobserved upper
    // words, rather than a claim to execute 128-bit register arithmetic.
    std::array<std::uint32_t,32> g{}, f{};
    std::array<std::uint32_t,0x1000/4> stack{};
    std::array<std::uint32_t,512> unnormalized2{};
    std::array<std::uint32_t,768> unnormalized3{};
    std::array<unsigned,pcCount> visits{};
    std::bitset<pcCount> executedPcs{},branchTakenPcs{},branchNotTakenPcs{},excludedPcs{};
    std::array<unsigned,6> excludedSpanEntries{};
    unsigned steps=0,draws=0,initializerCalls=0;
    unsigned permutationReads=0,gradientReads=0,vector2Writes=0,vector3Writes=0;
    unsigned maxPermutationReadIndex=0,maxGradientReadIndex=0;

    static std::size_t pcIndex(std::uint32_t pc) {
        if(pc&3u) throw std::runtime_error("49M oracle unaligned PC");
        if(pc>=noiseStart && pc<noiseEnd) return (pc-noiseStart)/4;
        if(pc>=initializeStart && pc<initializeEnd) return noiseCount+(pc-initializeStart)/4;
        if(pc>=randomStart && pc<randomEnd) return noiseCount+initializeCount+(pc-randomStart)/4;
        throw std::runtime_error("49M oracle PC outside frozen intervals");
    }
    static std::uint32_t word(std::uint32_t pc) {
        (void)pcIndex(pc);
        if(pc>=noiseStart && pc<noiseEnd) return gradient49m_words::noise[(pc-noiseStart)/4];
        if(pc>=initializeStart && pc<initializeEnd) return gradient49m_words::initialize[(pc-initializeStart)/4];
        return gradient49m_words::random[(pc-randomStart)/4];
    }
    static bool within(std::uint32_t address,std::uint32_t base,std::size_t words) {
        return address>=base && address-base<words*4;
    }
    std::uint32_t read(std::uint32_t address) {
        if(address&3u) throw std::runtime_error("49M oracle unaligned word read");
        if(address==flagAddress) return state.initialized?0u:1u;
        if(address==scratchAddress) return static_cast<std::uint32_t>(state.scratchCell);
        if(address==randomPointerAddress) return syntheticRandomObject;
        if(address==syntheticRandomObject+0x58) return raw;
        if(within(address,permutationBase,state.permutation.size())) {
            const auto index=(address-permutationBase)/4;
            ++permutationReads;
            if(index>maxPermutationReadIndex) maxPermutationReadIndex=index;
            const auto value=state.permutation[index];
            if(value>=256) throw std::runtime_error("49M oracle reached invalid permutation fixture");
            return value;
        }
        if(within(address,gradientBase,state.gradient1D.size())) {
            const auto index=(address-gradientBase)/4;
            ++gradientReads;
            if(index>maxGradientReadIndex) maxGradientReadIndex=index;
            const auto value=state.gradient1D[index];
            if(!(value>=-1.0f && value<=0.99609375f))
                throw std::runtime_error("49M oracle reached invalid gradient fixture");
            return bits(value);
        }
        if(within(address,stackBase,stack.size())) return stack[(address-stackBase)/4];
        // Included instructions never read g2/g3. Allowing those reads would
        // conceal a dependency on a deliberately omitted normalization.
        throw std::runtime_error("49M oracle read outside projected memory");
    }
    void write(std::uint32_t address,std::uint32_t value) {
        if(address&3u) throw std::runtime_error("49M oracle unaligned word write");
        if(address==flagAddress) {state.initialized=(value==0);return;}
        if(address==scratchAddress) {state.scratchCell=signedWord(value);return;}
        if(address==syntheticRandomObject+0x58) {raw=value;return;}
        if(within(address,permutationBase,state.permutation.size())) {
            if(value>=256) throw std::runtime_error("49M oracle permutation escaped first 256");
            state.permutation[(address-permutationBase)/4]=value; return;
        }
        if(within(address,gradientBase,state.gradient1D.size())) {
            state.gradient1D[(address-gradientBase)/4]=fp(value); return;
        }
        if(within(address,vector2Base,unnormalized2.size())) {
            unnormalized2[(address-vector2Base)/4]=value; ++vector2Writes; return;
        }
        if(within(address,vector3Base,unnormalized3.size())) {
            unnormalized3[(address-vector3Base)/4]=value; ++vector3Writes; return;
        }
        if(within(address,stackBase,stack.size())) {stack[(address-stackBase)/4]=value;return;}
        throw std::runtime_error("49M oracle write outside projected memory");
    }
    bool exclude(std::uint32_t& pc) {
        const auto& spans=excludedSpans();
        for(std::size_t i=0;i<spans.size();++i) {
            if(pc<spans[i].begin || pc>=spans[i].end) continue;
            if(pc!=spans[i].begin) throw std::runtime_error("49M oracle entered exclusion interior");
            ++excludedSpanEntries[i];
            for(auto omitted=spans[i].begin;omitted<spans[i].end;omitted+=4)
                excludedPcs.set(pcIndex(omitted));
            pc=spans[i].end;
            return true;
        }
        return false;
    }
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        if(++steps>100000) throw std::runtime_error("49M oracle instruction budget");
        const auto index=pcIndex(pc);
        ++visits[index]; executedPcs.set(index);
        if(pc==randomStart) ++draws;
        if(pc==initializeStart) ++initializerCalls;
        const auto w=word(pc),op=w>>26,rs=(w>>21)&31u,rt=(w>>16)&31u;
        const auto rd=(w>>11)&31u,sa=(w>>6)&31u,fn=w&63u;
        const auto immediate=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs]+static_cast<std::uint32_t>(immediate);};
        auto branch=[&](bool taken) {
            (taken?branchTakenPcs:branchNotTakenPcs).set(index);
            target=taken?pc+4+static_cast<std::int32_t>(immediate)*4:pc+8;
            return true;
        };
        if(!w) return false;
        if(op==9) g[rt]=address();
        else if(op==10) g[rt]=signedWord(g[rs])<immediate?1u:0u;
        else if(op==12) g[rt]=g[rs]&(w&65535u);
        else if(op==13) g[rt]=g[rs]|(w&65535u);
        else if(op==15) g[rt]=(w&65535u)<<16;
        else if(op==0x23) g[rt]=read(address());
        else if(op==0x2b) write(address(),g[rt]);
        else if(op==0x24) {
            const auto a=address();
            if(a!=scratchAddress) throw std::runtime_error("49M oracle unexpected byte read");
            g[rt]=read(a)&255u;
        }
        else if(op==0x31) f[rt]=read(address());
        else if(op==0x39) write(address(),f[rt]);
        else if(op==0x1f) {
            for(unsigned i=0;i<4;++i) write(address()+i*4,i?0u:g[rt]);
        }
        else if(op==0x1e) {
            g[rt]=read(address());
            for(unsigned i=1;i<4;++i)
                if(read(address()+i*4)!=0) throw std::runtime_error("49M oracle nonzero projected upper GPR");
        }
        else if(op==0x1c && fn==0x28 && sa==0x18) g[rd]=g[rs]|g[rt];
        else if(op==4) return branch(g[rs]==g[rt]);
        else if(op==5) return branch(g[rs]!=g[rt]);
        else if(op==1 && rt==1) return branch(signedWord(g[rs])>=0);
        else if(op==3) {
            target=((pc+4)&0xf0000000u)|((w&0x3ffffffu)<<2);
            if(target!=initializeStart && target!=randomStart)
                throw std::runtime_error("49M oracle reached excluded external call");
            g[31]=pc+8; return true;
        }
        else if(op==0) {
            if(fn==0) g[rd]=g[rt]<<sa;
            else if(fn==8) {target=g[rs];return true;}
            else if(fn==0x21) g[rd]=g[rs]+g[rt];
            else if(fn==0x24) g[rd]=g[rs]&g[rt];
            else if(fn==0x18) {
                // EE MULT rd,rs,rt has an explicit low-word destination.
                const auto product=static_cast<std::int64_t>(signedWord(g[rs]))*signedWord(g[rt]);
                g[rd]=static_cast<std::uint32_t>(product);
            } else throw std::runtime_error("49M oracle unsupported SPECIAL instruction");
        }
        else if(op==0x11) {
            if(rs==4) f[rd]=g[rt];
            else if(rs==0) g[rt]=f[rd];
            else if(rs==20 && fn==0x20) {
                volatile float value=static_cast<float>(signedWord(f[rd])); f[sa]=bits(value);
            }
            else if(rs==16) {
                const float a=fp(f[rd]),b=fp(f[rt]);
                volatile float value=0;
                if(fn==0) value=a+b;
                else if(fn==1) value=a-b;
                else if(fn==2) value=a*b;
                else if(fn==3) value=a/b;
                else if(fn==0x24) {
                    if(!(a>=-2147483648.0f && a<2147483648.0f))
                        throw std::runtime_error("49M oracle cvt.w.s outside signed truncation domain");
                    f[sa]=static_cast<std::uint32_t>(static_cast<std::int32_t>(a));
                    return false;
                } else throw std::runtime_error("49M oracle unsupported FPU instruction");
                f[sa]=bits(value);
            } else throw std::runtime_error("49M oracle unsupported COP1 format");
        } else throw std::runtime_error("49M oracle unsupported instruction");
        g[0]=0;
        return false;
    }
    Outcome run(const State& initial,std::uint32_t randomRaw,float x) {
        *this=Machine{};
        state=initial; raw=randomRaw;
        const bool cold=!initial.initialized;
        g[29]=stackBase+0x1000; g[31]=returnSentinel; f[12]=bits(x);
        std::uint32_t pc=noiseStart;
        while(pc!=returnSentinel) {
            if(exclude(pc)) continue;
            std::uint32_t target=pc+4;
            if(execute(pc,target)) {
                std::uint32_t ignored=0;
                if(execute(pc+4,ignored)) throw std::runtime_error("49M oracle branch in delay slot");
                pc=target;
            } else pc+=4;
        }
        if(draws!=(cold?1791u:0u) || initializerCalls!=(cold?1u:0u))
            throw std::runtime_error("49M oracle projected RNG/control invariant");
        if(vector2Writes!=(cold?512u:0u) || vector3Writes!=(cold?768u:0u))
            throw std::runtime_error("49M oracle omitted component draws or stores");
        for(std::size_t i=0;i<excludedSpanEntries.size();++i)
            if(excludedSpanEntries[i]!=(cold?(i==5?1u:256u):0u))
                throw std::runtime_error("49M oracle exclusion-entry invariant");
        if((executedPcs&excludedPcs).any() || gradientReads!=2)
            throw std::runtime_error("49M oracle projected observable invariant");
        return {state,raw,fp(f[0]),draws};
    }
};
} // namespace original49m
