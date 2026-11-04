#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_config_process_globals50la.h"
#include "original_standard_archive_provider50la.h"
#include "original_particle_forced_sets50la.h"
#include "../game/original_text_config_primitives50la.h"
#include "../game/original_source_particle_random50la.h"
#include "../game/original_particle_random_mode50la.h"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <cstring>
#include <thread>
#include <exception>
namespace ra_cold_aggregate50LA {class Owner;}
namespace original_main_entry50KG {struct Application;}
namespace rr::platform {struct OriginalUiMainApplication50KG;}
namespace rr::assets {
// Fresh stack Config occurrences at1617A0 and15AE00, explicitly distinct.
// No World-derived allocation, shared numeric
// handle, predecoded PRT, or public path/root receipt. Cold World alone lends
// its genuine same-App source invocation; no issuer is installed here.
class OriginalParticleSourceConfig50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    using Text=rr::game::OriginalTextConfigPrimitives50LA;
    struct Source final {
        std::int64_t file00;
        std::uint32_t binary08,opened0C,mode10,count14,index18;
        const void* text1C;std::uint32_t selected20;const void* child24;
        std::array<char,1024> name28; // Only byte zero is constructor-written.
        std::uint32_t field428,field42C,field430,field434,cacheCount438;
        std::array<std::uint32_t,32> cache43C;
        std::uint8_t *names4CC,*index4BC;std::uint32_t indexBytes4C0;
        const void* block4C4;std::uint32_t field4C8;
        Source()noexcept {
            file00=-1;binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
            text1C=nullptr;selected20=0;child24=nullptr;name28[0]=0;
            field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
            cache43C.fill(0);names4CC=nullptr;index4BC=nullptr;
            indexBytes4C0=0;block4C4=nullptr;field4C8=0;
        }
    };
    enum class Occurrence {manager1617A0,emitter15AE00};
    struct Emitter15AE00Tag {};
    const Occurrence occurrence_=Occurrence::manager1617A0;
    const void* emitterOwner_=nullptr; //Native occurrence identity, never source data.
    OriginalParticleForcedSets50LA* emitterForcedSets_=nullptr; //Borrowed from that same emitter.
    Source storage_;Source* source_=&storage_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    const void* selectedRoot_=nullptr;
    std::int32_t handle_=-1;
    std::string filename_;
    bool attempted_=false,borrowed_=false,nativeClosed_=false;
    bool sourceDestructorAttempted_=false,sourceClosed_=false;
    std::optional<bool> sourceOpenReturned_;
    std::uint32_t sourcePC_=0;
    struct Line {std::array<char,1024> bytes;std::array<bool,1024> known{};bool present=false;std::size_t position=0;};
    Text::Table textTable_;Line textInput_;
    Text::Bytes textBytes_,textLine_,textName_;
    bool textAttempted_=false,textReturned_=false;
    std::uint32_t textLineOrdinal_=0;
    OriginalParticleSourceConfig50LA()noexcept=default;
    explicit OriginalParticleSourceConfig50LA(Emitter15AE00Tag,const void* emitter,
        OriginalParticleForcedSets50LA& forcedSets)noexcept
        :occurrence_(Occurrence::emitter15AE00),emitterOwner_(emitter),emitterForcedSets_(&forcedSets){sourcePC_=0x1573a4;}
    bool beginEmitter15AE00(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,const std::string&,const void*,D&)noexcept;
    bool emitterOccurrenceCurrentNative(const void*,const OriginalStandardTextureResolver50LA*,
        const OriginalStandardArchiveProvider50LA*,const OriginalParticleForcedSets50LA&,
        const std::array<std::uint32_t,0x260/4>&,const std::array<std::uint32_t,0x260/4>&)const noexcept;
    bool beginFile1577D0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,const std::string&,std::uint32_t,D&)noexcept;
    bool begin1617A0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,const std::string&,D&)noexcept;
    bool current()const noexcept;
    bool readLine(Line&,D&)noexcept;
    bool tell(std::int64_t&,D&)noexcept;
    bool seek(std::size_t,D&)noexcept;
    bool abortNative(D&)noexcept;
    bool closeStack1573B0(D&)noexcept;
    bool closeEmitterStack1573B0(const void*,D&)noexcept;
    bool closeOwnedTextStack1573B0(bool,D&)noexcept;
    bool emitterType8Returned_=false,emitterType0Returned_=false,emitterType1Returned_=false,emitterType2Returned_=false,emitterType9Returned_=false,emitterType11Returned_=false,emitterType12Returned_=false;
    bool emitterPropertiesReturnedNative()const noexcept {
        if(emitterPropertyTarget_==0x15c330u)return emitterType0Returned_;
        if(emitterPropertyTarget_==0x15d560u)return emitterType1Returned_;
        if(emitterPropertyTarget_==0x15dd00u)return emitterType2Returned_;
        if(emitterPropertyTarget_==0x15e6d0u)return emitterType8Returned_;
        if(emitterPropertyTarget_==0x15ef70u)return emitterType9Returned_;
        if(emitterPropertyTarget_==0x15fc10u)return emitterType11Returned_;
        if(emitterPropertyTarget_==0x1602b0u)return emitterType12Returned_;
        return false;
    }
    bool checkEmitterGroupLookup161EA0(std::int32_t index,std::int32_t count)noexcept {
        if(occurrence_!=Occurrence::emitter15AE00||!emitterOwner_||!commonReturned_||!current())return false;
        return OriginalConfigProcessGlobals50LA::groupLookup161EC0(index,count,sourcePC_);
    }
    bool missingOpenCurrent()const noexcept;
    void syncTextFields()noexcept {
        source_->count14=textTable_.count14;
        source_->index18=textTable_.capacity18;
        source_->text1C=textTable_.sourceStorage;
    }
    //Actual157500 call after same occurrence's nonbinary open+seek0. No
    //external callback, supplied reader success or fabricated tell is admitted.
    bool parseText157500(D& d)noexcept {
        if(textAttempted_||!source_||sourcePC_!=0x157ABCu||source_->binary08||
           source_->opened0C!=1||source_->mode10||source_->count14||source_->index18||
           source_->text1C||!current())return false;
        textAttempted_=true;textLineOrdinal_=0;
        for(;;){
            sourcePC_=0x157760u;
            if(!readLine(textInput_,d))return false;
            sourcePC_=0x157768u;
            if(!textInput_.present){
                const auto final=Text::finishLastRow157770(textTable_,textLineOrdinal_);
                syncTextFields();sourcePC_=final.pc;
                if(final.stop!=Text::Stop::complete)return false;
                textReturned_=true;return true;
            }
            //Transfer only genuinely known destination bytes, never copy an
            //indeterminate std::array wholesale. Persistent buffers retain
            //partial source stack state across any bounded return.
            for(std::size_t i=0;i<textInput_.bytes.size();++i){
                textBytes_.known[i]=textInput_.known[i];
                if(textInput_.known[i])textBytes_.value[i]=static_cast<std::uint8_t>(textInput_.bytes[i]);
            }
            auto step=Text::split159440(textBytes_,textLine_);sourcePC_=step.pc;
            if(step.stop!=Text::Stop::complete)return false;
            if(textLine_.value[0]=='['){
                step=Text::sectionName157548(textLine_,textName_);sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
                const auto found=Text::lookupText157C44(textTable_,textName_);sourcePC_=found.pc;
                if(found.stop!=Text::Stop::complete)return false;
                if(found.index!=-1){
                    //Duplicate follows warning/shared assertion state at1576C8;
                    //that diagnostic owner is not joined. Do not merge, skip,
                    //increment line count, or return parser success past it.
                    sourcePC_=0x1576C8u;return false;
                }
                Text::AllocationRequest request;
                step=Text::prepareNewRow15758C(textTable_,request);
                syncTextFields();sourcePC_=step.pc;
                if(step.stop==Text::Stop::needsAllocation){
                    step=Text::continueAllocation(textTable_);
                    syncTextFields();sourcePC_=step.pc;
                }
                if(step.stop!=Text::Stop::complete)return false;
                step=Text::copyRowName15761C(textTable_,textName_);sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
                std::int64_t tellValue=0;sourcePC_=0x15762Cu;
                if(!tell(tellValue,d))return false;
                step=Text::finishRow157654(textTable_,static_cast<std::uint64_t>(tellValue),textLineOrdinal_);
                syncTextFields();sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
            }
            //157544 branch delay for nonheaders,157748 for successful headers.
            ++textLineOrdinal_;
        }
    }

#include "original_particle_source_section50la.inl"
#include "original_particle_source_field50la.inl"
#include "original_particle_source_integer50la.inl"

    // Native retained backing of genuine158C00 stack locals; none are
    // manager1617A0 getter receipts. Source lifetime ends on getter return.
    bool emitterPropertiesAttempted_=false,emitterGeneralReturned_=false;
    std::optional<std::uint32_t> emitterPropertyTarget_;
    bool type0GeneralSelectionAttempted_=false,type0GeneralReturned_=false;
    std::uint8_t type0ScalarCount_=0,type0AssertionCount_=0;
    std::optional<bool> type0GeneralFound_;
    std::array<std::array<char,128>,2> type0Noise30_;
    std::array<std::optional<bool>,2> type0NoiseFound_;
    std::array<std::size_t,2> type0NoiseKnownBytes_{{0,0}};
    std::array<std::uint8_t,2> type0NoiseWritten_{{0,0}};
    std::uint8_t type0NoiseReturned_=0;
    bool type2GeneralReturned_=false,type2MovingReturned_=false;
    std::optional<bool> type2GeneralFound_,type2MovingFound_;
    std::array<std::array<char,128>,2> type2Range30_;
    std::array<std::optional<bool>,2> type2RangeFound_;
    std::array<std::size_t,2> type2RangeKnownBytes_{{0,0}};
    std::array<std::uint8_t,2> type2RangeWritten_{{0,0}};
    std::uint8_t type2RangeReturned_=0;
    bool type9GeneralReturned_=false,type9SpreadReturned_=false;
    std::optional<bool> type9GeneralFound_,type9SpreadFound_;
    std::array<std::array<char,128>,2> type9Range30_;
    std::array<std::optional<bool>,2> type9RangeFound_;
    std::array<std::size_t,2> type9RangeKnownBytes_{{0,0}};
    std::array<std::uint8_t,2> type9RangeWritten_{{0,0}};
    std::uint8_t type9RangeReturned_=0;
    bool type11GeneralReturned_=false,type11HeadReturned_=false,type11MovingReturned_=false;
    std::optional<bool> type11GeneralFound_,type11HeadFound_,type11MovingFound_;
    std::uint8_t type11ScalarCount_=0;
    std::array<std::array<char,128>,2> type11Range30_;
    std::array<std::optional<bool>,2> type11RangeFound_;
    std::array<std::size_t,2> type11RangeKnownBytes_{{0,0}};
    std::array<std::uint8_t,2> type11RangeWritten_{{0,0}};
    std::uint8_t type11RangeReturned_=0;
    bool type12GeneralReturned_=false,type12DistanceSquared_=false,type12ReciprocalWritten_=false;
    std::optional<bool> type12GeneralFound_,type12OscillationEnabled_;
    std::optional<std::uint32_t> type12DistanceInput_,type12SquaredBits_,type12ReciprocalBits_;
    std::array<std::array<char,128>,2> type12Range30_;
    std::array<std::optional<bool>,2> type12RangeFound_;
    std::array<std::size_t,2> type12RangeKnownBytes_{{0,0}};
    std::array<std::uint8_t,2> type12RangeWritten_{{0,0}};
    std::uint8_t type12RangeReturned_=0,type12TailScalarCount_=0,type12OscillationMask_=0,type12FlagMask_=0;
    std::optional<bool> emitterGeneralFound_,emitterMinRangeFound_,emitterMaxRangeFound_;
    std::array<char,128> emitterMinRange30_,emitterMaxRange30_;
    std::size_t emitterMinRangeKnownBytes_=0,emitterMaxRangeKnownBytes_=0;
    std::array<std::uint8_t,2> emitterVectorWritten_{{0,0}};
    bool emitterMinRangeReturned_=false,emitterMaxRangeReturned_=false,emitterSpreadReturned_=false;
    // Exact subset of302E10 double then310440 float, no host locale parser.
    // Decimal dyadics with <=24-bit significand and <=9 fractional digits
    // are exactly representable in both stages (including signed zero).
    bool exactEmitterDecimal302E10(std::string_view text,std::uint32_t& out)noexcept {
        sourcePC_=0x30AB28u;
        if(!OriginalConfigProcessGlobals50LA::reentrancyCurrentNative())return false;
        sourcePC_=0x302E10u;
        const auto space=[](char c){return c==' '||(c>='\t'&&c<='\r');};
        while(!text.empty()&&space(text.front()))text.remove_prefix(1);
        while(!text.empty()&&space(text.back()))text.remove_suffix(1);
        bool negative=false;
        if(!text.empty()&&(text.front()=='-'||text.front()=='+')){
            negative=text.front()=='-';text.remove_prefix(1);
        }
        if(text.empty()||text.size()>15)return false; //Proved <=15-digit fast source path.
        std::uint32_t n=0,fraction=0;bool dot=false,any=false;
        for(char c:text){
            if(c=='.'&&!dot){dot=true;continue;}
            if(c<'0'||c>'9')return false;
            const auto digit=static_cast<std::uint32_t>(c-'0');
            if(n>(0x01000000u-digit)/10u)return false;
            n=n*10u+digit;any=true;if(dot&&++fraction>9)return false;
        }
        if(!any)return false;
        for(std::uint32_t i=0;i<fraction;++i){if(n%5)return false;n/=5;}
        // Construct exact IEEE32 bits; no native floating conversion or
        // dependency on a host rounding mode. n/2^fraction is normal or0.
        const std::uint32_t sign=negative?0x80000000u:0;
        if(!n){out=sign;sourcePC_=0x310440u;return true;}
        std::uint32_t high=0;for(auto v=n;v>1;v>>=1)++high;
        const auto significand=high<=23?n<<(23-high):n>>(high-23);
        out=sign|((127u+high-fraction)<<23)|(significand&0x7fffffu);
        sourcePC_=0x310440u;return true;
    }
    bool readOwnedEmitterVector158C00(const char* key,std::size_t base,std::array<char,128>& input,
            std::size_t& bytes,std::optional<bool>& foundReceipt,std::uint8_t& written,
            std::array<std::uint32_t,0x260/4>& words,std::array<std::uint32_t,0x260/4>& known)noexcept {
        if(occurrence_!=Occurrence::emitter15AE00||!emitterOwner_||(!emitterGeneralReturned_&&!type2GeneralReturned_&&!type9GeneralReturned_&&!type11GeneralReturned_&&!type12GeneralReturned_&&!commonGeneralReturned_)||
           !ownedTextSectionForFieldCurrent()||foundReceipt||base+2>=words.size())return false;
        std::string_view value;bool found=false;
        if(!findOwnedTextField158240(key,value,found))return false;
        foundReceipt=found;
        const auto write=[&](std::size_t lane,std::uint32_t bits,std::uint32_t pc){
            sourcePC_=pc;words[base+lane]=bits;known[base+lane]=0xffffffffu;
            written|=static_cast<std::uint8_t>(1u<<lane);
        };
        if(found){
            sourcePC_=0x15887Cu;
            for(std::size_t i=0;i<128;++i)input[i]=i<value.size()?value[i]:0;
            sourcePC_=0x1588B0u;input[value.size()<127?value.size():127]=0;bytes=128;
            for(std::size_t lane=0;lane<3;++lane){
                std::optional<std::size_t> token;
                if(!OriginalConfigProcessGlobals50LA::token30AB70(&input,input.data(),128,lane==0,
                        lane==2?'\n':',',token,sourcePC_))return false;
                sourcePC_=0x302E10u;if(!token)return false; //Original NULL conversion is not repaired.
                std::size_t length=0;while(*token+length<128&&input[*token+length])++length;
                if(*token+length>=128)return false;
                std::uint32_t bits;
                if(!exactEmitterDecimal302E10(std::string_view(input.data()+*token,length),bits))return false;
                write(lane,bits,lane==0?0x158C7Cu:lane==1?0x158CA0u:0x158CC4u);
            }
            OriginalConfigProcessGlobals50LA::expireStackNative(&input);
        }else{
            input[0]=0;bytes=1;sourcePC_=0x158CC8u;
            std::array<std::uint32_t,3> defaults;
            if(!OriginalConfigProcessGlobals50LA::readDefault4338B0(defaults))return false;
            write(0,defaults[0],0x158CD0u);write(1,defaults[1],0x158CD8u);write(2,defaults[2],0x158CE0u);
        }
        //Original XYZ getter never writes W in any caller destination.
        sourcePC_=0x158CF4u;return true;
    }
    bool readEmitterVector158C00(bool maximum,std::array<std::uint32_t,0x260/4>& words,
                                std::array<std::uint32_t,0x260/4>& known)noexcept {
        if(maximum?!emitterMinRangeReturned_:emitterMinRangeReturned_)return false;
        if(!readOwnedEmitterVector158C00(maximum?"MaxRange":"MinRange",maximum?0x210/4:0x200/4,
            maximum?emitterMaxRange30_:emitterMinRange30_,maximum?emitterMaxRangeKnownBytes_:emitterMinRangeKnownBytes_,
            maximum?emitterMaxRangeFound_:emitterMinRangeFound_,emitterVectorWritten_[maximum?1:0],words,known))return false;
        (maximum?emitterMaxRangeReturned_:emitterMinRangeReturned_)=true;return true;
    }
    bool beginType1Properties15D560(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                  std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15d560u;sourcePC_=0x15d560u;
        sourcePC_=0x15d56cu;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15d574u;return false; //Original next manager/group operation, not callback return.
    }

    bool beginType0Properties15C330(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                  std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15c330u;sourcePC_=0x15c330u;
        //Unlike type8, subtype0 calls the common base BEFORE any selection
        //or subtype field. Do not manufacture Min/MaxRange or spread receipts.
        sourcePC_=0x15c34cu;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15c35cu;type0GeneralSelectionAttempted_=true;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        type0GeneralReturned_=true;type0GeneralFound_=selected; //Source ignores return.
        struct Scalar {const char* key;std::uint32_t fallback,offset,call;};
        constexpr Scalar fields[]={
            {"MinimumPercentLife",0x3f000000u,0x21c,0x15c378},
            {"MinimumPercentLifeConvergance",0x3f800000u,0x220,0x15c438},
            {"MinimumInitialSize",0,0x208,0x15c4d8},
            {"MaximumInitialSize",0x40a00000u,0x20c,0x15c57c},
            {"InitialSizeToSpeedRatio",0x3c23d70au,0x210,0x15c624},
            {"MinimumLife",0x43fa0000u,0x214,0x15c6c8},
            {"MaximumLife",0x455ac000u,0x218,0x15c770},
            {"LifeToSpeedRatio",0x40a00000u,0x224,0x15c814},
            {"MinimumEmitRate",0,0x22c,0x15c8b4},
            {"MaximumEmitRate",0x41200000u,0x230,0x15c958},
            {"ParticlesToDistanceRatio",0x3c23d70au,0x234,0x15ca00},
            {"ParticlesToSpeedRatio",0,0x238,0x15caa0},
            {"InitialPercentAge",0,0x228,0x15cb40},
            {"MinimumEmissionSpeed",0,0x23c,0x15cbe0}};
        for(std::size_t i=0;i<14;++i){const auto& field=fields[i];sourcePC_=field.call;
            if(!readEmitterScalar158B80(field.key,field.fallback,field.offset,words,known))return false;
            ++type0ScalarCount_;
            if(!OriginalConfigProcessGlobals50LA::checkType0Scalar15C380(i,words[field.offset/4],sourcePC_))return false;
            ++type0AssertionCount_;
        }
        for(std::size_t i=0;i<2;++i){
            sourcePC_=i?0x15cca0u:0x15cc84u;
            if(!readOwnedEmitterVector158C00(i?"MaximumVelocityNoise":"MinimumVelocityNoise",
                (i?0x250:0x240)/4,type0Noise30_[i],type0NoiseKnownBytes_[i],type0NoiseFound_[i],
                type0NoiseWritten_[i],words,known))return false;
            ++type0NoiseReturned_;
        }
        sourcePC_=0x15cca8u;return false; //Manager/group read is not yet joined; not callback return.
    }

    bool beginType2Properties15DD00(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                  std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15dd00u;sourcePC_=0x15dd00u;
        sourcePC_=0x15dd24u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        type2GeneralReturned_=true;type2GeneralFound_=selected; //Source ignores selection result.
        sourcePC_=0x15dd3cu;std::string_view value;bool found=false;
        if(!findOwnedTextField158240("MovingEmitter",value,found))return false;
        type2MovingFound_=found;std::int32_t actual=1;
        if(found){if(!convertInteger158920(value,actual))return false;}
        else sourcePC_=0x158b60u;
        //158B30 writes the raw integer, not a normalized native boolean.
        words[0x200/4]=static_cast<std::uint32_t>(actual);known[0x200/4]=0xffffffffu;
        type2MovingReturned_=true;
        for(std::size_t i=0;i<2;++i){
            sourcePC_=i?0x15dd74u:0x15dd58u;
            if(!readOwnedEmitterVector158C00(i?"MaxRange":"MinRange",(i?0x220:0x210)/4,
                type2Range30_[i],type2RangeKnownBytes_[i],type2RangeFound_[i],
                type2RangeWritten_[i],words,known))return false;
            ++type2RangeReturned_;
        }
        sourcePC_=0x15dd80u;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15dd88u;return false; //Owned manager/group continuation, not a callback return.
    }

    bool beginType9Properties15EF70(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                  std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15ef70u;sourcePC_=0x15ef70u;
        sourcePC_=0x15ef94u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        type9GeneralReturned_=true;type9GeneralFound_=selected; //Source ignores selection result.
        for(std::size_t i=0;i<2;++i){
            sourcePC_=i?0x15efccu:0x15efb0u;
            if(!readOwnedEmitterVector158C00(i?"MaxRange":"MinRange",(i?0x210:0x200)/4,
                type9Range30_[i],type9RangeKnownBytes_[i],type9RangeFound_[i],
                type9RangeWritten_[i],words,known))return false;
            ++type9RangeReturned_;
        }
        sourcePC_=0x15efe4u;std::string_view value;bool found=false;
        if(!findOwnedTextField158240("ApplySpreadToVelocity",value,found))return false;
        type9SpreadFound_=found;std::int32_t actual=1;
        if(found){if(!convertInteger158920(value,actual))return false;}
        else sourcePC_=0x158b60u;
        //The raw signed getter result belongs at224, not subtype8's228.
        words[0x224/4]=static_cast<std::uint32_t>(actual);known[0x224/4]=0xffffffffu;
        type9SpreadReturned_=true;
        sourcePC_=0x15eff0u;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15eff8u;return false; //Owned velocity/group continuation, not callback return.
    }

    bool beginType11Properties15FC10(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                   std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15fc10u;sourcePC_=0x15fc10u;
        sourcePC_=0x15fc34u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        type11GeneralReturned_=true;type11GeneralFound_=selected; //Source ignores selection result.
        struct Scalar {const char* key;std::uint32_t fallback,offset,call;};
        constexpr Scalar fields[]={
            {"MinStreamLength",0x40400000u,0x200,0x15fc50},
            {"MaxStreamLength",0x40a00000u,0x204,0x15fc6c},
            {"TailThickness",0x3f800000u,0x210,0x15fc88},
            {"HeadThickness",0x40000000u,0x214,0x15fca4},
            {"ParticleSeparationFactor",0x3fc00000u,0x208,0x15fcc0},
            {"TailLifeModifier",0x3f19999au,0x20c,0x15fce0}};
        for(const auto& field:fields){
            sourcePC_=field.call;
            if(!readEmitterScalar158B80(field.key,field.fallback,field.offset,words,known))return false;
            ++type11ScalarCount_;
        }
        sourcePC_=0x15fcf8u;std::string_view value;bool found=false;
        if(!findOwnedTextField158240("SuperbrightHead",value,found))return false;
        type11HeadFound_=found;std::int32_t head=1;
        if(found){if(!convertInteger158920(value,head))return false;}
        else sourcePC_=0x158b60u;
        words[0x218/4]=static_cast<std::uint32_t>(head);known[0x218/4]=0xffffffffu;type11HeadReturned_=true;
        for(std::size_t i=0;i<2;++i){
            sourcePC_=i?0x15fd30u:0x15fd14u;
            if(!readOwnedEmitterVector158C00(i?"MaxRange":"MinRange",(i?0x230:0x220)/4,
                type11Range30_[i],type11RangeKnownBytes_[i],type11RangeFound_[i],
                type11RangeWritten_[i],words,known))return false;
            ++type11RangeReturned_;
        }
        sourcePC_=0x15fd48u;
        if(!findOwnedTextField158240("MovingEmitter",value,found))return false;
        type11MovingFound_=found;std::int32_t moving=0;
        if(found){if(!convertInteger158920(value,moving))return false;}
        else sourcePC_=0x158b60u;
        //Both158B30 destinations retain raw signed integers; do not normalize.
        words[0x21c/4]=static_cast<std::uint32_t>(moving);known[0x21c/4]=0xffffffffu;type11MovingReturned_=true;
        sourcePC_=0x15fd54u;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15fd5cu;return false; //Owned group continuation, not callback return.
    }

    bool beginType12Properties1602B0(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                   std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x1602b0u;sourcePC_=0x1602b0u;
        sourcePC_=0x1602d4u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        type12GeneralReturned_=true;type12GeneralFound_=selected;
        for(std::size_t i=0;i<2;++i){
            sourcePC_=i?0x16030cu:0x1602f0u;
            if(!readOwnedEmitterVector158C00(i?"MaxRange":"MinRange",(i?0x210:0x200)/4,
                type12Range30_[i],type12RangeKnownBytes_[i],type12RangeFound_[i],
                type12RangeWritten_[i],words,known))return false;
            ++type12RangeReturned_;
        }
        sourcePC_=0x160328u;
        if(!readEmitterScalar158B80("DistanceToFade",0x43480000u,0x220,words,known))return false;
        sourcePC_=0x160330u;type12DistanceInput_=words[0x220/4];
        std::uint32_t squared=0;
        const bool exact=exactMultiplySourceSingle(*type12DistanceInput_,*type12DistanceInput_,squared);
        //The arithmetic helper is bit-exact only within its existing domain;
        //retain this actual MUL instruction, not its other caller's15AFF8 PC.
        sourcePC_=0x160344u;if(!exact)return false;
        type12SquaredBits_=squared;
        sourcePC_=0x16034cu;words[0x220/4]=squared;known[0x220/4]=0xffffffffu;type12DistanceSquared_=true;
        sourcePC_=0x160354u;
        const auto exponent=(squared>>23)&255u;
        //Exact normal power-of-two reciprocals only. The default200 squares
        //to40000, which remains at this division without a fabricated224 store.
        if((squared&0x807fffffu)||exponent==0||exponent>=254)return false;
        type12ReciprocalBits_=(254u-exponent)<<23;
        sourcePC_=0x16036cu;sourcePC_=0x160370u; //Actual next getter call's delay store.
        words[0x224/4]=*type12ReciprocalBits_;known[0x224/4]=0xffffffffu;type12ReciprocalWritten_=true;
        if(!readEmitterScalar158B80("ParticlesInVolume",0x42c80000u,0x228,words,known))return false;
        ++type12TailScalarCount_;sourcePC_=0x160388u;
        if(!readEmitterScalar158B80("OscillationSpeed",0x41200000u,0x22c,words,known))return false;
        ++type12TailScalarCount_;sourcePC_=0x1603a4u;
        if(!readEmitterScalar158B80("OscillationMagnitude",0x3f800000u,0x234,words,known))return false;
        ++type12TailScalarCount_;
        sourcePC_=0x1603acu;sourcePC_=0x1603b0u;sourcePC_=0x1603b4u;sourcePC_=0x1603bcu;
        //The bounded getters produce finite normal/zero values. Exact -1
        //takes the source equality branch and leaves230/238/23C unknown.
        type12OscillationEnabled_=words[0x22c/4]!=0xbf800000u;
        sourcePC_=0x1603c4u;sourcePC_=0x1603c8u;
        if(*type12OscillationEnabled_){
            sourcePC_=0x1603ccu;words[0x230/4]=0;known[0x230/4]=0xffffffffu;type12OscillationMask_=1;
            sourcePC_=0x1603d0u;words[0x238/4]=0;known[0x238/4]=0xffffffffu;type12OscillationMask_=3;
            sourcePC_=0x1603e8u;words[0x23c/4]|=1u;known[0x23c/4]|=1u;type12OscillationMask_=7;
        }
        sourcePC_=0x1603f0u;if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x1603f8u;if(known[0x198/4]!=0xffffffffu)return false;
        sourcePC_=0x160404u;words[0x198/4]|=0x80u;type12FlagMask_=1;
        sourcePC_=0x160410u;words[0x198/4]&=~8u;type12FlagMask_=3;
        sourcePC_=0x160414u;return false; //Group continuation, then mandatory population remains bounded.
    }

    bool beginType8Properties15E6D0(const void* emitter,std::array<std::uint32_t,0x260/4>& words,
                                  std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        d={};
        if(occurrence_!=Occurrence::emitter15AE00||!emitter||emitterOwner_!=emitter||
           emitterPropertiesAttempted_||!ownedTextConfigCurrent()||textSelectAttempted_)return false;
        emitterPropertiesAttempted_=true;emitterPropertyTarget_=0x15e6d0u;sourcePC_=0x15E6D0u;
        sourcePC_=0x15E6F4u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        emitterGeneralReturned_=true;emitterGeneralFound_=selected; //Source ignores selection return.
        sourcePC_=0x15E710u;if(!readEmitterVector158C00(false,words,known))return false;
        sourcePC_=0x15E72Cu;if(!readEmitterVector158C00(true,words,known))return false;
        sourcePC_=0x15E744u;std::string_view value;bool found=false;
        if(!findOwnedTextField158240("ApplySpreadToVelocity",value,found))return false;
        std::int32_t actual=1;
        if(found){if(!convertInteger158920(value,actual))return false;}
        else sourcePC_=0x158B60u; //Actual default1, not normalization of present integer.
        words[0x228/4]=static_cast<std::uint32_t>(actual);known[0x228/4]=0xffffffffu;
        emitterSpreadReturned_=true;sourcePC_=0x15E750u;
        if(!beginCommonProperties15AEA0(words,known,d))return false;
        sourcePC_=0x15E758u;return false; //Base returned; type8 continuation is not yet joined.
    }

    bool commonPropertiesAttempted_=false,commonGeneralReturned_=false;
    std::optional<bool> commonGeneralFound_,commonGroupFound_,commonVelocityFound_;
    bool commonGroupWritten_=false,commonGroupAssertionReturned_=false,commonAgeScaled_=false,commonVelocityReturned_=false;
    std::uint32_t commonScalarCount_=0;
    std::array<char,128> commonVelocity30_;
    std::size_t commonVelocityKnownBytes_=0;std::uint8_t commonVelocityWritten_=0;
    bool exactMultiplySourceSingle(std::uint32_t a,std::uint32_t b,std::uint32_t& result)noexcept {
        sourcePC_=0x15AFF8u;
        const auto ea=(a>>23)&255u,eb=(b>>23)&255u;
        if(ea==255||eb==255)return false; //Unjoined EE NaN/overflow semantics.
        const auto sign=(a^b)&0x80000000u;
        if((a&0x7fffffffu)==0||(b&0x7fffffffu)==0){
            //Subnormal operand handling is unjoined even when paired with0.
            if((!ea&&(a&0x7fffffu))||(!eb&&(b&0x7fffffu)))return false;
            result=sign;return true;
        }
        if(!ea||!eb)return false;
        const std::uint64_t product=std::uint64_t((a&0x7fffffu)|0x800000u)*((b&0x7fffffu)|0x800000u);
        const std::uint32_t high=(product&(std::uint64_t(1)<<47))?47u:46u;
        const auto shift=high-23;
        //Admit only exact normal products, independent of rounding mode.
        if(product&((std::uint64_t(1)<<shift)-1))return false;
        const auto exponent=static_cast<std::int32_t>(ea)+static_cast<std::int32_t>(eb)-127+static_cast<std::int32_t>(high)-46;
        if(exponent<=0||exponent>=255)return false;
        result=sign|(static_cast<std::uint32_t>(exponent)<<23)|static_cast<std::uint32_t>((product>>shift)&0x7fffffu);
        return true;
    }
    bool beginCommonProperties15AEA0(std::array<std::uint32_t,0x260/4>& words,
                                    std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        if(occurrence_!=Occurrence::emitter15AE00||!emitterOwner_||!emitterPropertiesAttempted_||
           commonPropertiesAttempted_||!emitterForcedSets_||
           !emitterForcedSets_->currentNative(emitterOwner_,words,known))return false;
        if(emitterPropertyTarget_==0x15e6d0u){
            if(!emitterSpreadReturned_||!ownedTextSectionForFieldCurrent())return false;
        }else if(emitterPropertyTarget_==0x1602b0u){
            if(!type12OscillationEnabled_||type12TailScalarCount_!=3||
               type12OscillationMask_!=(*type12OscillationEnabled_?7u:0u)||!ownedTextSectionForFieldCurrent())return false;
        }else if(emitterPropertyTarget_==0x15fc10u){
            if(!type11MovingReturned_||!ownedTextSectionForFieldCurrent())return false;
        }else if(emitterPropertyTarget_==0x15ef70u){
            if(!type9SpreadReturned_||!ownedTextSectionForFieldCurrent())return false;
        }else if(emitterPropertyTarget_==0x15dd00u){
            if(type2RangeReturned_!=2||!ownedTextSectionForFieldCurrent())return false;
        }else if(emitterPropertyTarget_==0x15c330u||emitterPropertyTarget_==0x15d560u){
            if(textSelectAttempted_||!ownedTextConfigCurrent())return false;
        }else return false;
        commonPropertiesAttempted_=true;sourcePC_=0x15AEA0u;
        sourcePC_=0x15AEC8u;bool selected=false;
        if(!selectTextSection157D00("General Info",selected,d))return false;
        commonGeneralReturned_=true;commonGeneralFound_=selected;
        sourcePC_=0x15AEE0u;std::string_view value;bool found=false;
        if(!findOwnedTextField158240("ParticleGroup",value,found))return false;
        commonGroupFound_=found;std::int32_t group=-1;
        if(found){if(!convertInteger158920(value,group))return false;}
        else sourcePC_=0x158B60u;
        words[0x1B8/4]=static_cast<std::uint32_t>(group);known[0x1B8/4]=0xffffffffu;commonGroupWritten_=true;
        if(!OriginalConfigProcessGlobals50LA::checkParticleGroup15AEE8(group,sourcePC_))return false;
        commonGroupAssertionReturned_=true;
        struct Scalar {const char* key;std::uint32_t offset,defaultBits,call;};
        //Exact original order, including authored defaults differing from ctor.
        constexpr Scalar scalars[]={
            {"ParticleLife",0x17c,0x447a0000,0x15af70},
            {"InitialSize",0x184,0x3f800000,0x15af8c},
            {"Growth",0x180,0,0x15afa4},
            {"EmitRate",0x188,0x43fa0000,0x15afc0},
            {"AgeAtBirth",0x1a0,0,0x15afd8}};
        for(const auto& field:scalars){
            sourcePC_=field.call;
            if(!findOwnedTextField158240(field.key,value,found))return false;
            std::uint32_t bits=field.defaultBits;
            if(found){if(!exactEmitterDecimal302E10(value,bits))return false;sourcePC_=0x158BC4u;}
            else sourcePC_=0x158BDCu;
            words[field.offset/4]=bits;known[field.offset/4]=0xffffffffu;++commonScalarCount_;
        }
        std::uint32_t age;
        if(!exactMultiplySourceSingle(words[0x1a0/4],words[0x17c/4],age))return false;
        sourcePC_=0x15B008u;words[0x1a0/4]=age;commonAgeScaled_=true;
        //15B008 is the delay-slot store before entered158C00 Velocity.
        if(!readOwnedEmitterVector158C00("Velocity",0x60/4,commonVelocity30_,commonVelocityKnownBytes_,
            commonVelocityFound_,commonVelocityWritten_,words,known))return false;
        commonVelocityReturned_=true;sourcePC_=0x15B01Cu;
        return continueCommonTail15B01C(words,known,d);
    }

    std::optional<std::int32_t> commonEmitMode5C_;
    bool commonTailAttempted_=false,commonModeStored_=false,commonDelayCopied_=false;
    std::uint32_t commonTailScalarCount_=0,commonModeScalarCount_=0;
    bool commonModeTimeCopied_=false,commonPropertiesSelectionAttempted_=false,commonPropertiesSelected_=false;
    std::optional<bool> commonPropertiesFound_,commonAnimationFound_;
    std::array<std::int32_t,8> commonPropertyStack_;
    std::uint32_t commonPropertyCount_=0,commonMirrorCount_=0,commonFlagCount_=0;
    bool commonGroundFlagWritten_=false,commonAnimationSelectionAttempted_=false,commonAnimationSelected_=false;
    bool commonFramesWritten_=false,commonReciprocalWritten_=false,commonForcedCountWritten_=false;
    bool commonAnimationFlagWritten_=false,commonReturned_=false;
    //Only the actual Owner's synchronous15AE48 stack may lend this adapter.
    //It never becomes a source callback, async capability or retained callable.
    struct Mode2DrawAdapter {
        virtual bool draw15B148(OriginalParticleSourceConfig50LA&,std::uint32_t&)noexcept=0;
        virtual ~Mode2DrawAdapter()=default;
    };
    Mode2DrawAdapter* commonMode2DrawAdapter_=nullptr;
    class Mode2DrawScope final {
        OriginalParticleSourceConfig50LA& config_;
        Mode2DrawAdapter& adapter_;
        bool bound_=false;
    public:
        Mode2DrawScope(OriginalParticleSourceConfig50LA& config,Mode2DrawAdapter& adapter)noexcept
            :config_(config),adapter_(adapter){
            if(!config_.commonMode2DrawAdapter_&&config_.occurrence_==Occurrence::emitter15AE00&&
               !config_.nativeClosed_&&!config_.sourceClosed_&&config_.thread_==std::this_thread::get_id()){
                config_.commonMode2DrawAdapter_=&adapter_;bound_=true;
            }
        }
        ~Mode2DrawScope(){
            if(bound_&&config_.commonMode2DrawAdapter_==&adapter_)config_.commonMode2DrawAdapter_=nullptr;
        }
        bool bound()const noexcept{return bound_;}
        Mode2DrawScope(const Mode2DrawScope&)=delete;
        Mode2DrawScope& operator=(const Mode2DrawScope&)=delete;
    };
    struct CommonMode2History final {
        //Pin the actual process owner, never copy/reseed its shared RNG. The
        //typed Application identity is checked by Owner before live withdrawal.
        std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> applicationPin;
        const original_main_entry50KG::Application* application=nullptr;
        std::uint32_t rawBefore=0,rawAfter=0,rawValue=0;
        bool drawEntered=false,drawReturned=false;
        std::optional<std::uint32_t> minimum,converted,maximum,unit,difference,product,sum,stored;
        std::uint32_t pc=0;
    } commonMode2_;
    bool commonMode2TimeStored_=false;
    bool commonMode2CurrentNative(const std::array<std::uint32_t,0x260/4>&,
                                 const std::array<std::uint32_t,0x260/4>&)const noexcept;
    bool initializeCommonMode2Timer15B148(std::array<std::uint32_t,0x260/4>& words,
                                        std::array<std::uint32_t,0x260/4>& known)noexcept {
        auto& h=commonMode2_;
        if(h.pc||h.drawEntered||h.drawReturned||commonMode2TimeStored_||commonModeTimeCopied_||
           !commonEmitMode5C_||*commonEmitMode5C_!=2||commonTailScalarCount_!=4||
           commonModeScalarCount_!=4||!ownedTextSectionForFieldCurrent())return false;
        h.pc=sourcePC_=0x15B148u;
        if(!commonMode2DrawAdapter_)return false;
        std::uint32_t random;
        if(!commonMode2DrawAdapter_->draw15B148(*this,random))return false;
        //The Owner publishes authentic308030 entry/return and raw chronology.
        //No numeric admission precedes or rolls back that single real draw.
        if(!h.drawEntered||!h.drawReturned||!h.applicationPin||!h.application||
           h.pc!=0x30805Cu||h.rawValue!=random||h.rawAfter!=h.rawBefore*0x41c64e6du+0x3039u||
           h.rawValue!=(h.rawAfter&0x7fffffffu))return false;
        h.pc=sourcePC_=0x15B150u; //Source MTC1 preserves actual low31 before CVT.
        h.pc=sourcePC_=0x15B154u;
        if(known[0x1e8/4]!=0xffffffffu)return false;
        h.minimum=words[0x1e8/4];
        std::uint32_t result;
        h.pc=sourcePC_=0x15B158u;
        if(!rr::game::detail::sourceParticleTruncateInteger50LA(random,result))return false;
        h.converted=result;
        h.pc=sourcePC_=0x15B15Cu;
        h.pc=sourcePC_=0x15B160u;
        if(known[0x1ec/4]!=0xffffffffu)return false;
        h.maximum=words[0x1ec/4];
        h.pc=sourcePC_=0x15B164u;h.pc=sourcePC_=0x15B168u;
        h.pc=sourcePC_=0x15B16Cu;
        //Operand order is source fs=2^-31, ft=converted. The pinned bounded
        //EE model includes its left-power-of-two Booth correction here.
        if(!rr::game::detail::sourceParticleMode2Scale(*h.converted,result))return false;
        h.unit=result;
        h.pc=sourcePC_=0x15B170u;
        if(!rr::game::detail::sourceParticleMode2Difference(*h.maximum,*h.minimum,result))return false;
        h.difference=result;
        h.pc=sourcePC_=0x15B174u;
        if(!rr::game::detail::sourceParticleMode2Product(*h.difference,*h.unit,result))return false;
        h.product=result;
        h.pc=sourcePC_=0x15B178u;
        if(!rr::game::detail::sourceParticleMode2Sum(*h.minimum,*h.product,result))return false;
        h.sum=result;
        h.pc=sourcePC_=0x15B17Cu; //Unconditional branch, then its actual delay store.
        h.pc=sourcePC_=0x15B180u;
        words[0x1e4/4]=*h.sum;known[0x1e4/4]=0xffffffffu;
        h.stored=*h.sum;commonMode2TimeStored_=true;
        h.pc=sourcePC_=0x15B1C8u;h.pc=sourcePC_=0x15B1CCu;
        h.pc=sourcePC_=0x15B1D0u;return true;
    }
    bool readForcedSet158B30(OriginalParticleForcedSets50LA::Entry15B540& entry,
                           std::uint32_t index)noexcept {
        if(!emitterForcedSets_||!entry.formatReturned||entry.lookupAttempted||
           !ownedTextSectionForFieldCurrent())return false;
        entry.lookupAttempted=true;
        std::string_view value;bool found=false;
        //The request is the exact same occurrence's generated ForcedSet%d,
        //not a supplied field name or data from an alternate parser.
        if(!findOwnedTextField158240(entry.key.data(),value,found))return false;
        entry.found=found;entry.lookupReturnPC=sourcePC_;
        std::uint32_t raw=0xffffffffu;
        if(found){
            entry.conversionAttempted=true;std::int32_t converted;
            const bool returned=convertInteger158920(value,converted);
            entry.conversionPC=sourcePC_;
            if(!returned)return false;
            raw=static_cast<std::uint32_t>(converted);entry.convertedWord=raw;
        }else sourcePC_=0x158B60u; //Actual missing-field default store, not a parse failure.
        //158B30's a3 is the ACTUAL array element, not a stack temporary.
        //Lookup/conversion precede any native destination admission failure.
        auto& owner=*emitterForcedSets_;
        if(!owner.publication15B534_||!owner.pointer1C4_||
           *owner.pointer1C4_!=owner.values_.get()||!owner.values_||index>=owner.capacity_)return false;
        owner.values_[index]=raw;entry.rawWord=raw;
        sourcePC_=0x158B68u;sourcePC_=0x158B6Cu;sourcePC_=0x158B70u;
        sourcePC_=0x158B74u;sourcePC_=0x158B78u;
        entry.getterReturnPC=sourcePC_;entry.getterReturned=true;return true;
    }
    bool continueForcedSets15B508(std::array<std::uint32_t,0x260/4>& words,
                                 std::array<std::uint32_t,0x260/4>& known)noexcept {
        if(!emitterForcedSets_||!commonAnimationFlagWritten_||commonReturned_||
           !ownedTextSectionForFieldCurrent()||
           !emitterForcedSets_->currentNative(emitterOwner_,words,known))return false;
        auto& owner=*emitterForcedSets_;
        if(owner.countRead15B508_)return false; //Fresh occurrence only; no reload/copy/replacement admission.
        sourcePC_=0x15B508u;
        if(known[0x1c8/4]!=0xffffffffu)return false;
        owner.countRead15B508_=words[0x1c8/4];
        const auto count=*owner.countRead15B508_;
        sourcePC_=0x15B50Cu;
        if(count&&(count&0x80000000u)==0){
            sourcePC_=0x15B514u;
            if(!owner.pointer1C4_||*owner.pointer1C4_||known[0x1c4/4]!=0xffffffffu||words[0x1c4/4])return false;
            owner.oldPointerRead15B514_=true; //Only the actual NULL branch skips2E8AE0.
            sourcePC_=0x15B528u;owner.countRead15B528_=words[0x1c8/4];
            sourcePC_=0x15B52Cu;
            //Native extent/metadata admission precedes entering the call.
            //Refusal here is not an executed allocator or a sourceNULL result.
            if(*owner.countRead15B528_!=count||!owner.prepareExtentNative(count))return false;
            sourcePC_=0x15B52Cu;sourcePC_=0x15B530u;
            sourcePC_=0x2E8CA0u;sourcePC_=0x2E8D90u;
            if(!owner.allocate2E8D90Native())return false;
            sourcePC_=0x15B534u;owner.pointer1C4_=owner.values_.get();owner.publication15B534_=true;
            //There is no guest address encoding for this host pointer. Keep
            //the old zero as inert backing, and withdraw its numeric knownness.
            known[0x1c4/4]=0;
            sourcePC_=0x15B53Cu;owner.indexS0_=0;owner.indexInitialized15B53C_=true;
            sourcePC_=0x15B590u;owner.initialCount15B590_=words[0x1c8/4];
            if(*owner.initialCount15B590_!=count)return false;
            sourcePC_=0x15B598u;owner.initialLoop15B598_=true;
            for(std::uint32_t index=0;index<count;++index){
                if(owner.indexS0_!=index||index>=owner.capacity_)return false;
                auto& entry=owner.entries_[index];
                sourcePC_=0x15B544u;entry.ordinal=index+1u;
                //Exact bounded308C90("ForcedSet%d", positive signed ordinal).
                //At most19 bytes plusNUL fit the actual source stack40..7F.
                sourcePC_=0x15B54Cu;sourcePC_=0x15B550u;sourcePC_=0x308C90u;
                constexpr char prefix[]="ForcedSet";
                for(std::size_t i=0;i<9;++i)entry.key[i]=prefix[i];
                std::array<char,10> reverse;std::size_t digits=0;
                for(auto ordinal=*entry.ordinal;ordinal;ordinal/=10u)reverse[digits++]=static_cast<char>('0'+ordinal%10u);
                for(std::size_t i=0;i<digits;++i)entry.key[9+i]=reverse[digits-1-i];
                entry.key[9+digits]=0;entry.keyKnownBytes=10+digits;
                sourcePC_=0x15B554u;entry.formatReturned=true;++owner.entriesStarted_;
                sourcePC_=0x15B554u;
                if(!owner.pointer1C4_||*owner.pointer1C4_!=owner.values_.get())return false;
                sourcePC_=0x15B568u;if(!readForcedSet158B30(entry,index))return false;
                sourcePC_=0x15B570u;
                if(!owner.pointer1C4_||*owner.pointer1C4_!=owner.values_.get())return false;
                sourcePC_=0x15B578u;++owner.indexS0_;entry.indexAdvanced=true;
                sourcePC_=0x15B580u;
                if(!entry.rawWord||!owner.values_||index>=owner.capacity_)return false;
                entry.loaded15B580=owner.values_[index];
                sourcePC_=0x15B584u;entry.decremented15B584=*entry.loaded15B580-1u;
                sourcePC_=0x15B588u;owner.values_[index]=*entry.decremented15B584;
                entry.decrementStored15B588=true;++owner.entriesStored_;
                sourcePC_=0x15B590u;entry.countRead15B590=words[0x1c8/4];
                if(*entry.countRead15B590!=count)return false;
                sourcePC_=0x15B598u;entry.loopAgain15B598=owner.indexS0_<count;
            }
            sourcePC_=0x15B5A0u; //Only the complete positive loop supplies sourceV0=1 here.
        }
        //Nonpositive count uses the genuine15B50C/510 ->15B5A4 success path.
        sourcePC_=0x15B5A4u;sourcePC_=0x15B5A8u;sourcePC_=0x15B5B4u;
        sourcePC_=0x15B5B8u;sourcePC_=0x15B5BCu;sourcePC_=0x15B5C0u;sourcePC_=0x15B5C4u;
        owner.returned15B5C4_=true;commonReturned_=true;return true;
    }
    bool readEmitterInteger158B30(const char* key,std::int32_t fallback,std::int32_t& out)noexcept {
        if(occurrence_!=Occurrence::emitter15AE00||!emitterOwner_||!ownedTextSectionForFieldCurrent())return false;
        std::string_view value;bool found=false;
        if(!findOwnedTextField158240(key,value,found))return false;
        if(found)return convertInteger158920(value,out);
        sourcePC_=0x158B60u;out=fallback;return true;
    }
    bool readEmitterScalar158B80(const char* key,std::uint32_t fallback,std::uint32_t offset,
            std::array<std::uint32_t,0x260/4>& words,std::array<std::uint32_t,0x260/4>& known)noexcept {
        if(occurrence_!=Occurrence::emitter15AE00||!emitterOwner_||!ownedTextSectionForFieldCurrent()||offset/4>=words.size())return false;
        std::string_view value;bool found=false;
        if(!findOwnedTextField158240(key,value,found))return false;
        std::uint32_t bits=fallback;
        if(found){if(!exactEmitterDecimal302E10(value,bits))return false;sourcePC_=0x158BC4u;}
        else sourcePC_=0x158BDCu;
        words[offset/4]=bits;known[offset/4]=0xffffffffu;return true;
    }
    bool continueCommonTail15B01C(std::array<std::uint32_t,0x260/4>& words,
                                 std::array<std::uint32_t,0x260/4>& known,D& d)noexcept {
        if(!commonVelocityReturned_||commonTailAttempted_||!ownedTextSectionForFieldCurrent())return false;
        commonTailAttempted_=true;sourcePC_=0x15B01Cu;
        std::int32_t mode;
        if(!readEmitterInteger158B30("EmitMode",0,mode))return false;
        commonEmitMode5C_=mode;
        //Source loads stack5C then stores1D8 in DelayTime call's delay slot.
        sourcePC_=0x15B040u;words[0x1d8/4]=static_cast<std::uint32_t>(mode);known[0x1d8/4]=0xffffffffu;commonModeStored_=true;
        if(!readEmitterScalar158B80("DelayTime",0,0x1e0,words,known))return false;
        ++commonTailScalarCount_;
        sourcePC_=0x15B068u;words[0x1dc/4]=words[0x1e0/4];known[0x1dc/4]=0xffffffffu;commonDelayCopied_=true;
        if(!readEmitterScalar158B80("GroundLevel",0x7f7fffff,0x1a4,words,known))return false;
        ++commonTailScalarCount_;sourcePC_=0x15B080u;
        if(!readEmitterScalar158B80("GravityMultiplier",0x40400000,0x1a8,words,known))return false;
        ++commonTailScalarCount_;sourcePC_=0x15B0A0u;
        if(!readEmitterScalar158B80("Restitution",0x3f0ccccd,0x1ac,words,known))return false;
        ++commonTailScalarCount_;
        if(mode==1){
            sourcePC_=0x15B0C8u;if(!readEmitterScalar158B80("OneShotTime",0,0x1ec,words,known))return false;
            ++commonModeScalarCount_;sourcePC_=0x15B0D8u;
            words[0x1e4/4]=words[0x1ec/4];known[0x1e4/4]=0xffffffffu;commonModeTimeCopied_=true;
        }else if(mode==2){
            struct F{const char* key;std::uint32_t offset,pc;};
            constexpr F fields[]={{"RandomMinTimeOn",0x1e8,0x15b0f8},{"RandomMaxTimeOn",0x1ec,0x15b110},
                {"RandomMinTimeOff",0x1f0,0x15b128},{"RandomMinTimeOff",0x1f0,0x15b140}};
            for(const auto& f:fields){sourcePC_=f.pc;if(!readEmitterScalar158B80(f.key,0,f.offset,words,known))return false;++commonModeScalarCount_;}
            //The original repeats RandomMinTimeOff/1F0: do not repair it to
            //MaxTimeOff/1F4. Continue through the actual process RNG owner.
            if(!initializeCommonMode2Timer15B148(words,known))return false;
        }else if(mode==3){
            sourcePC_=0x15B1A0u;if(!readEmitterScalar158B80("TimedTimeOn",0,0x1ec,words,known))return false;
            ++commonModeScalarCount_;sourcePC_=0x15B1B8u;
            if(!readEmitterScalar158B80("TimedTimeOff",0,0x1f4,words,known))return false;
            ++commonModeScalarCount_;sourcePC_=0x15B1C4u;
            words[0x1e4/4]=words[0x1ec/4];known[0x1e4/4]=0xffffffffu;commonModeTimeCopied_=true;
        }
        sourcePC_=0x15B1D0u;commonPropertiesSelectionAttempted_=true;bool selected=false;
        if(!selectTextSection157D00("Properties",selected,d))return false;
        commonPropertiesSelected_=true;commonPropertiesFound_=selected;
        struct P{const char* key;std::int32_t fallback;std::uint32_t pc;};
        constexpr P properties[]={{"Fades",0,0x15b1e8},{"HasVelocity",0,0x15b200},{"Grows",0,0x15b218},
            {"FadeNonLinear",0,0x15b230},{"Glows",0,0x15b248},{"Additive",0,0x15b260},
            {"Gravity",0,0x15b278},{"Lit",1,0x15b290}};
        for(const auto& p:properties){sourcePC_=p.pc;
            if(!readEmitterInteger158B30(p.key,p.fallback,commonPropertyStack_[commonPropertyCount_]))return false;
            ++commonPropertyCount_;
        }
        std::int32_t mirror;
        sourcePC_=0x15B2A8u;if(!readEmitterInteger158B30("RandomMirrorAboutX",0,mirror))return false;
        words[0x1cc/4]=static_cast<std::uint32_t>(mirror);known[0x1cc/4]=0xffffffffu;++commonMirrorCount_;
        sourcePC_=0x15B2C0u;if(!readEmitterInteger158B30("RandomMirrorAboutY",0,mirror))return false;
        words[0x1d0/4]=static_cast<std::uint32_t>(mirror);known[0x1d0/4]=0xffffffffu;++commonMirrorCount_;
        struct Flag{std::uint32_t local,bit,on,off;bool invert;};
        constexpr Flag flags[]={{0,2,0x15b2e0,0x15b2f0,false},{1,4,0x15b30c,0x15b31c,false},
            {2,8,0x15b338,0x15b348,false},{3,0x1000,0x15b364,0x15b374,false},
            {5,1,0x15b390,0x15b3a0,false},{4,0x2000,0x15b3bc,0x15b3cc,false},
            {6,0x20,0x15b3e8,0x15b3f8,false},{7,0x800,0x15b414,0x15b424,true}};
        for(const auto& f:flags){
            const bool set=(commonPropertyStack_[f.local]!=0)!=f.invert;
            if(known[0x198/4]!=0xffffffffu){sourcePC_=(set?f.on:f.off)-12;return false;}
            sourcePC_=set?f.on:f.off;
            words[0x198/4]=set?(words[0x198/4]|f.bit):(words[0x198/4]&~f.bit);++commonFlagCount_;
        }
        //Admitted scalar forms are finite normal/zero; GroundLevel also has
        //the real FLT_MAX default. Bit equality is exact for this sentinel.
        const bool finiteGround=words[0x1a4/4]!=0x7f7fffffu;
        sourcePC_=finiteGround?0x15B458u:0x15B468u;
        words[0x198/4]=finiteGround?(words[0x198/4]|0x200u):(words[0x198/4]&~0x200u);commonGroundFlagWritten_=true;
        sourcePC_=0x15B474u;commonAnimationSelectionAttempted_=true;
        if(!selectTextSection157D00("Animation",selected,d))return false;
        commonAnimationSelected_=true;commonAnimationFound_=selected;
        sourcePC_=0x15B490u;if(!readEmitterScalar158B80("FramesPerSet",0xbf800000,0x1bc,words,known))return false;
        commonFramesWritten_=true;sourcePC_=0x15B4B0u;
        const auto frames=words[0x1bc/4],exponent=(frames>>23)&255u;
        //Only exact normal power-of-two reciprocals;0, NaN, subnormals and
        //non-power-of-two denominators remain at the original division.
        if((frames&0x7fffffu)||exponent==0||exponent>=254)return false;
        const auto reciprocal=(frames&0x80000000u)|((254u-exponent)<<23);
        sourcePC_=0x15B4C4u;words[0x1c0/4]=reciprocal;known[0x1c0/4]=0xffffffffu;commonReciprocalWritten_=true;
        std::int32_t forced; //Actual delay-store above precedes158B30.
        if(!readEmitterInteger158B30("NumberOfForcedSets",0,forced))return false;
        words[0x1c8/4]=static_cast<std::uint32_t>(forced);known[0x1c8/4]=0xffffffffu;commonForcedCountWritten_=true;
        const bool animated=!(frames&0x80000000u)&&frames>0x3f800000u;
        sourcePC_=animated?0x15B4F4u:0x15B504u;
        words[0x198/4]=animated?(words[0x198/4]|0x400u):(words[0x198/4]&~0x400u);commonAnimationFlagWritten_=true;
        return continueForcedSets15B508(words,known); //Common return only; never a subtype/runtime receipt.
    }

public:
    ~OriginalParticleSourceConfig50LA(){if(borrowed_||commonMode2DrawAdapter_)std::terminate();
        OriginalConfigProcessGlobals50LA::expireStackNative(&emitterMinRange30_);
        OriginalConfigProcessGlobals50LA::expireStackNative(&emitterMaxRange30_);
        OriginalConfigProcessGlobals50LA::expireStackNative(&commonVelocity30_);
        for(auto& input:type0Noise30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);}
    OriginalParticleSourceConfig50LA(const OriginalParticleSourceConfig50LA&)=delete;
    OriginalParticleSourceConfig50LA& operator=(const OriginalParticleSourceConfig50LA&)=delete;
};
}
