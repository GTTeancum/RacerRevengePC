#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace rr::game {
// Numeric geometry/allocation arithmetic only. No World400, provider, resource
// return, source allocation, process global or callback authority is created.
class OriginalWaterReadbackLayout50LA final {
public:
    struct FactoryArguments {
        std::uint32_t constructorA2=0,constructorA3=0;
        std::uint32_t initializerT0=0,initializerF12Bits=0;
    };
    struct NativeExtent {
        std::uint32_t columns=0,bands=0;
        std::size_t allocationBytes=0,bandRecordsBytes=0,extraRecordsBytes=0;
    };
private:
    std::uint32_t argumentA2_=0,argumentA3_=0,columns_=0,words388_=0,fieldE0_=0,allocationWord_=0;
    std::optional<NativeExtent> extent_;
    OriginalWaterReadbackLayout50LA(std::uint32_t a2,std::uint32_t a3)noexcept
        :argumentA2_(a2),argumentA3_(a3) {
        //2B845C..84BC: every shift/add is a wrapping32-bit source operation.
        columns_=(a2<<1)+a2+1u;
        words388_=(((columns_<<1)+columns_)<<3)-columns_;
        fieldE0_=((a3<<2)+a3)<<2;
        //2B871C..8744: add1 BEFORE multiplying by bands, then multiply368.
        //Do not substitute368*columns*bands or the rounded transfer height.
        const auto records=(columns_+1u)*a3;
        allocationWord_=((((records<<1)+records)<<3)-records)<<4;
        //Separate conservative native span. Source bits above exist for all
        //raw inputs; no wrapped/negative guest extent becomes host memory.
        const std::uint64_t c=std::uint64_t(a2)*3u+1u;
        if(!a3||(a3&0x80000000u)||c>0x7fffffffu)return;
        const std::uint64_t n=(c+1u)*a3;
        if(n>std::uint64_t{0xffffffffu}/368u)return;
        const auto total=n*368u;
        if(total>std::numeric_limits<std::size_t>::max())return;
        const auto body=c*a3*368u;
        extent_=NativeExtent{std::uint32_t(c),a3,std::size_t(total),std::size_t(body),
            std::size_t(total-body)};
    }
public:
    OriginalWaterReadbackLayout50LA(const OriginalWaterReadbackLayout50LA&)=default;
    OriginalWaterReadbackLayout50LA& operator=(const OriginalWaterReadbackLayout50LA&)=delete;
    static OriginalWaterReadbackLayout50LA fromConstructorWords(std::uint32_t a2,std::uint32_t a3)noexcept {
        return OriginalWaterReadbackLayout50LA(a2,a3);
    }
    //2745D4..46BC, DATA383370/390/3B0. This does NOT select the World
    //factory's environment/file/resource branch. Table mode0..2 requires an
    //in-range numeric index. Original out-of-table reads remain unsupported.
    //Other modes take the fixed default and do not consume the table index.
    static std::optional<FactoryArguments> factoryArgumentsNumeric(
        std::uint32_t modeA1,std::uint32_t indexA3)noexcept {
        if(modeA1>2u)return FactoryArguments{45,2,90,0x41200000u};
        if(indexA3>=4u)return {};
        constexpr std::uint32_t table[3][4][2]={
            {{85,4},{45,4},{37,3},{29,2}},
            {{45,4},{37,3},{37,2},{29,2}},
            {{77,2},{69,2},{61,2},{53,2}}
        };
        return FactoryArguments{table[modeA1][indexA3][0],table[modeA1][indexA3][1],
            modeA1?90u:120u,modeA1?0x41200000u:0x41000000u};
    }
    std::uint32_t sourceArgumentA2Word()const noexcept{return argumentA2_;}
    std::uint32_t sourceArgumentA3Word()const noexcept{return argumentA3_;}
    std::uint32_t sourceColumns384AndDCWord()const noexcept{return columns_;}
    std::uint32_t sourceWords388Word()const noexcept{return words388_;}
    std::uint32_t sourceFieldE0Word()const noexcept{return fieldE0_;}
    std::uint32_t sourceAllocationByteCountWord()const noexcept{return allocationWord_;}
    const std::optional<NativeExtent>& checkedNativeExtent()const noexcept{return extent_;}
};
}
